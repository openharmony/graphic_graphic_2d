/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "modifier_render_thread/rs_modifiers_draw_thread.h"

#ifdef ACCESSIBILITY_ENABLE
#include "transaction/rs_render_service_client.h"
#endif
#include "command/rs_canvas_node_command.h"
#include "command/rs_command.h"
#include "command/rs_node_command.h"
#include "ffrt_inner.h"
#include "modifier_render_thread/rs_modifiers_draw.h"
#include "pipeline/rs_node_map.h"
#include "platform/common/rs_log.h"
#include "platform/ohos/backend/rs_vulkan_context.h"
#include "qos.h"
#include "render_context/shader_cache.h"
#include "concurrent_task_client.h"

namespace OHOS {
namespace Rosen {
std::atomic<bool> RSModifiersDrawThread::isStarted_ = false;
std::recursive_mutex RSModifiersDrawThread::transactionDataMutex_;

constexpr uint32_t DEFAULT_MODIFIERS_DRAW_THREAD_LOOP_NUM = 3;
constexpr uint32_t HYBRID_MAX_PIXELMAP_WIDTH = 8192;  // max width value from PhysicalDeviceProperties
constexpr uint32_t HYBRID_MAX_PIXELMAP_HEIGHT = 8192;  // max height value from PhysicalDeviceProperties
constexpr uint32_t HYBRID_MAX_ENABLE_OP_CNT = 12; // max value for enable hybrid op
RSModifiersDrawThread::RSModifiersDrawThread()
{
    ::atexit(&RSModifiersDrawThread::Destroy);
}

RSModifiersDrawThread::~RSModifiersDrawThread()
{
    if (!RSModifiersDrawThread::isStarted_) {
        return;
    }
    if (handler_ != nullptr) {
        handler_->RemoveAllEvents();
        handler_ = nullptr;
    }
    if (runner_ != nullptr) {
        runner_->Stop();
        runner_ = nullptr;
    }
#ifdef ACCESSIBILITY_ENABLE
    UnsubscribeHighContrastChange();
#endif
}

void RSModifiersDrawThread::Destroy()
{
    if (!RSModifiersDrawThread::isStarted_) {
        return;
    }
    auto task = []() {
        RSModifiersDrawThread::Instance().ClearEventResource();
    };
    RSModifiersDrawThread::Instance().PostSyncTask(task);
    auto& instancePtr = RSModifiersDrawThread::InstancePtr();
    instancePtr.reset();
}

void RSModifiersDrawThread::ClearEventResource()
{
    if (handler_ != nullptr) {
        handler_->RemoveAllEvents();
        handler_ = nullptr;
    }
    if (runner_ != nullptr) {
        runner_->Stop();
        runner_ = nullptr;
    }
}

RSModifiersDrawThread& RSModifiersDrawThread::Instance()
{
    static RSModifiersDrawThread instance;
    return instance;
}

std::unique_ptr<RSModifiersDrawThread>& RSModifiersDrawThread::InstancePtr()
{
    static std::unique_ptr<RSModifiersDrawThread> instancePtr = std::make_unique<RSModifiersDrawThread>();
    return instancePtr;
}

void RSModifiersDrawThread::SetCacheDir(const std::string& path)
{
    auto& cache = ShaderCache::Instance();
    cache.SetFilePath(path);
}

#ifdef ACCESSIBILITY_ENABLE
void RSModifiersDrawThread::SubscribeHighContrastChange()
{
    if (!RSSystemProperties::GetHybridRenderEnabled() || highContrastObserver_ != nullptr) {
        return;
    }
    auto& config = AccessibilityConfig::AccessibilityConfig::GetInstance();
    if (!config.InitializeContext()) {
        RS_LOGE("%{public}s AccessibilityConfig InitializeContext fail", __func__);
        return;
    }
    highContrastObserver_ = std::make_shared<Detail::HighContrastObserver>(highContrast_);
    // Non-system app, the first highContrast value in highContrastObserver_ is incorrect, so get it from RS.
    if (!highContrastObserver_->IsSystemApp()) {
        auto renderServiceClient =
            std::static_pointer_cast<RSRenderServiceClient>(RSIRenderClient::CreateRenderServiceClient());
        if (renderServiceClient != nullptr) {
            highContrast_ = renderServiceClient->GetHighContrastTextState();
        } else {
            RS_LOGE("%{public}s GetHighContrastTextState, renderServiceClient is null", __func__);
        }
    }
    config.SubscribeConfigObserver(AccessibilityConfig::CONFIG_ID::CONFIG_HIGH_CONTRAST_TEXT, highContrastObserver_);
}

void RSModifiersDrawThread::UnsubscribeHighContrastChange()
{
    if (highContrastObserver_ == nullptr) {
        return;
    }
    auto& config = AccessibilityConfig::AccessibilityConfig::GetInstance();
    if (!config.InitializeContext()) {
        RS_LOGE("%{public}s AccessibilityConfig InitializeContext fail", __func__);
    }
    config.UnsubscribeConfigObserver(AccessibilityConfig::CONFIG_ID::CONFIG_HIGH_CONTRAST_TEXT, highContrastObserver_);
    highContrastObserver_ = nullptr;
}

bool RSModifiersDrawThread::GetHighContrast() const
{
    return highContrast_;
}
#endif

bool RSModifiersDrawThread::GetIsStarted() const
{
    return RSModifiersDrawThread::isStarted_;
}

void RSModifiersDrawThread::Start()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (RSModifiersDrawThread::isStarted_) {
        return;
    }
    runner_ = AppExecFwk::EventRunner::Create("ModifiersDraw");
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    runner_->Run();
#ifdef ACCESSIBILITY_ENABLE
    SubscribeHighContrastChange();
#endif
    RSModifiersDrawThread::isStarted_ = true;
    PostTask([] {
        OHOS::ConcurrentTask::IntervalReply reply;
        reply.tid = gettid();
        OHOS::ConcurrentTask::ConcurrentTaskClient::GetInstance().QueryInterval(
            OHOS::ConcurrentTask::QUERY_MODIFIER_DRAW, reply);
        SetThreadQos(QOS::QosLevel::QOS_USER_INTERACTIVE);
        // Init shader cache
        std::string vkVersion = std::to_string(VK_API_VERSION_1_2);
        auto size = vkVersion.size();
        auto& cache = ShaderCache::Instance();
        cache.InitShaderCache(vkVersion.c_str(), size, true);
    });
    RS_LOGI("%{public}s RSModifiersDrawThread started", __func__);
}

void RSModifiersDrawThread::PostTask(const std::function<void()>&& task, const std::string& name, int64_t delayTime)
{
    if (!RSModifiersDrawThread::isStarted_) {
        Start();
    }
    if (handler_ != nullptr) {
        handler_->PostTask(task, name, delayTime, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

void RSModifiersDrawThread::RemoveTask(const std::string& name)
{
    if (handler_) {
        handler_->RemoveTask(name);
    }
}

void RSModifiersDrawThread::PostSyncTask(const std::function<void()>&& task)
{
    if (!RSModifiersDrawThread::isStarted_) {
        Start();
    }
    if (handler_ != nullptr) {
        handler_->PostSyncTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

bool RSModifiersDrawThread::TargetCommand(
    Drawing::DrawCmdList::HybridRenderType hybridRenderType, uint16_t type, uint16_t subType, bool cmdListEmpty)
{
    if (hybridRenderType == Drawing::DrawCmdList::HybridRenderType::NONE) {
        return false;
    }
    if (hybridRenderType != Drawing::DrawCmdList::HybridRenderType::CANVAS && cmdListEmpty) {
        return false;
    }
#ifdef ACCESSIBILITY_ENABLE
    if (RSModifiersDrawThread::Instance().GetHighContrast() &&
        hybridRenderType == Drawing::DrawCmdList::HybridRenderType::TEXT) {
            return false;
        }
#endif
    bool targetCmd = false;
    switch (type) {
        case RSCommandType::RS_NODE:
            if (subType == OHOS::Rosen::RSNodeCommandType::UPDATE_MODIFIER_DRAW_CMD_LIST ||
                subType == OHOS::Rosen::RSNodeCommandType::ADD_MODIFIER) {
                targetCmd = true;
            }
            break;
        case RSCommandType::CANVAS_NODE:
            if (subType == OHOS::Rosen::RSCanvasNodeCommandType::CANVAS_NODE_UPDATE_RECORDING) {
                targetCmd = true;
            }
            break;
        default:
            break;
    }
    return targetCmd;
}

bool RSModifiersDrawThread::LimitEnableHybridOpCnt(std::unique_ptr<RSTransactionData>& transactionData)
{
    int enableHybridOpCnt = 0;
    for (const auto& [nodeId, followType, command] : transactionData->GetPayload()) {
        auto drawCmdList = command == nullptr ? nullptr : command->GetDrawCmdList();
        if (drawCmdList == nullptr) {
            continue;
        }
        auto hybridRenderType = drawCmdList->GetHybridRenderType();
        // CanvasDrawingNode does not use FFRT, so exclude it
        bool enableType = hybridRenderType >= Drawing::DrawCmdList::HybridRenderType::TEXT &&
            hybridRenderType <= Drawing::DrawCmdList::HybridRenderType::HMSYMBOL;
        if (!enableType ||
            !TargetCommand(hybridRenderType, command->GetType(), command->GetSubType(), drawCmdList->IsEmpty()) ||
            !drawCmdList->IsHybridRenderEnabled(HYBRID_MAX_PIXELMAP_WIDTH, HYBRID_MAX_PIXELMAP_HEIGHT)) {
            continue;
        }
        enableHybridOpCnt++;
    }
    return enableHybridOpCnt <= HYBRID_MAX_ENABLE_OP_CNT;
}

std::unique_ptr<RSTransactionData>& RSModifiersDrawThread::ConvertTransaction(
    std::unique_ptr<RSTransactionData>& transactionData)
{
    if (!LimitEnableHybridOpCnt(transactionData)) {
        return transactionData;
    }
    static std::unique_ptr<ffrt::queue> queue = std::make_unique<ffrt::queue>(ffrt::queue_concurrent, "ModifiersDraw",
        ffrt::queue_attr().qos(ffrt::qos_user_interactive).max_concurrency(DEFAULT_MODIFIERS_DRAW_THREAD_LOOP_NUM));
    std::vector<ffrt::task_handle> handles;
    bool hasCanvasCmdList = false;
    for (auto& [nodeId, followType, command] : transactionData->GetPayload()) {
        auto drawCmdList = command == nullptr ? nullptr : command->GetDrawCmdList();
        if (drawCmdList == nullptr) {
            continue;
        }
        auto hybridRenderType = drawCmdList->GetHybridRenderType();
        if (!TargetCommand(hybridRenderType, command->GetType(), command->GetSubType(), drawCmdList->IsEmpty()) ||
            !drawCmdList->IsHybridRenderEnabled(HYBRID_MAX_PIXELMAP_WIDTH, HYBRID_MAX_PIXELMAP_HEIGHT)) {
            continue;
        }

        RS_OPTIONAL_TRACE_NAME_FMT("RSModifiersDrawThread hybridRenderType=%d, width=%d, height=%d, nodeId=%" PRId64,
            hybridRenderType, drawCmdList->GetWidth(), drawCmdList->GetHeight(), command->GetNodeId());
        RSModifiersDraw::MergeOffTreeNodeSet();
        switch (hybridRenderType) {
            case Drawing::DrawCmdList::HybridRenderType::CANVAS:
                RSModifiersDraw::ConvertCmdListForCanvas(drawCmdList, command->GetNodeId());
                hasCanvasCmdList = true;
                break;
            case Drawing::DrawCmdList::HybridRenderType::TEXT:
            case Drawing::DrawCmdList::HybridRenderType::SVG:
            case Drawing::DrawCmdList::HybridRenderType::HMSYMBOL:
                if (RSSystemProperties::GetHybridRenderParallelConvertEnabled()) {
                    handles.emplace_back(queue->submit_h(
                        [cmdList = std::move(drawCmdList), nodeId = command->GetNodeId()]() {
                        RSModifiersDraw::ConvertCmdList(cmdList, nodeId);
                        RSModifiersDraw::PurgeContextResource();
                    }));
                } else {
                    RSModifiersDraw::ConvertCmdList(drawCmdList, command->GetNodeId());
                }
                break;
            default:
                break;
        }
    }
    if (hasCanvasCmdList) {
        RSModifiersDrawThread::Instance().ScheduleTask([] { RSModifiersDraw::CreateNextFrameSurface(); });
    }
    for (auto& handle : handles) {
        queue->wait(handle);
    }
    RSModifiersDraw::PurgeContextResource();

    return transactionData;
}
} // namespace Rosen
} // namespace OHOS
