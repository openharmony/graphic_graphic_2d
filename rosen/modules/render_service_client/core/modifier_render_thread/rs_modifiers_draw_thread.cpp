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
bool RSModifiersDrawThread::isFirstFrame_ = true;
std::mutex RSModifiersDrawThread::ffrtMutex_;
std::condition_variable RSModifiersDrawThread::cv_;
std::atomic<uint32_t> RSModifiersDrawThread::ffrtTaskNum_ = 0;

constexpr uint32_t DEFAULT_MODIFIERS_DRAW_THREAD_LOOP_NUM = 3;
constexpr uint32_t HYBRID_MAX_PIXELMAP_WIDTH = 8192;  // max width value from PhysicalDeviceProperties
constexpr uint32_t HYBRID_MAX_PIXELMAP_HEIGHT = 8192;  // max height value from PhysicalDeviceProperties
constexpr uint32_t HYBRID_MAX_ENABLE_OP_CNT = 11;  // max value for enable hybrid op
constexpr uint32_t HYBRID_MAX_TEXT_ENABLE_OP_CNT = 1;  // max value for enable text hybrid op
constexpr int64_t FFRT_WAIT_TIMEOUT = 30; // ms
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
}

void RSModifiersDrawThread::ClearEventResource()
{
    RSModifiersDraw::ClearBackGroundMemory();
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
            if (subType == OHOS::Rosen::RSNodeCommandType::UPDATE_MODIFIER_DRAW_CMD_LIST_NG ||
                subType == OHOS::Rosen::RSNodeCommandType::ADD_MODIFIER_NG) {
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

bool RSModifiersDrawThread::GetIsFirstFrame()
{
    return isFirstFrame_;
}

void RSModifiersDrawThread::SetIsFirstFrame(bool isFirstFrame)
{
    isFirstFrame_ = isFirstFrame;
}

void RSModifiersDrawThread::SeperateHybridRenderCmdList(std::unique_ptr<RSTransactionData>& transactionData,
    std::vector<DrawOpInfo>& targetCmds, uint32_t& enableTextHybridOpCnt)
{
    for (const auto& [_, __, command] : transactionData->GetPayload()) {
        auto drawCmdList = command == nullptr ? nullptr : command->GetDrawCmdList();
        if (drawCmdList == nullptr) {
            continue;
        }
        auto hybridRenderType = drawCmdList->GetHybridRenderType();
        if (!TargetCommand(hybridRenderType, command->GetType(), command->GetSubType(), drawCmdList->IsEmpty()) ||
            !drawCmdList->IsHybridRenderEnabled(HYBRID_MAX_PIXELMAP_WIDTH, HYBRID_MAX_PIXELMAP_HEIGHT)) {
            continue;
        }
        switch (hybridRenderType) {
            case Drawing::DrawCmdList::HybridRenderType::TEXT: {
                enableTextHybridOpCnt++;
                DrawOpInfo textOpInfo {.nodeId = command->GetNodeId(), .cmdList = drawCmdList};
                targetCmds.push_back(textOpInfo);
                break;
            }
            case Drawing::DrawCmdList::HybridRenderType::SVG:
            case Drawing::DrawCmdList::HybridRenderType::HMSYMBOL: {
                DrawOpInfo targetCmd {.nodeId = command->GetNodeId(), .cmdList = drawCmdList};
                targetCmds.push_back(targetCmd);
                break;
            }
            default:
                break;
        }
    }
}

void RSModifiersDrawThread::TraverseDrawOpInfo(std::vector<DrawOpInfo>& targetCmds, std::atomic<size_t>& cmdIndex)
{
    bool needSubmit = false;
    for (auto index = cmdIndex.fetch_add(1); index < targetCmds.size(); index = cmdIndex.fetch_add(1)) {
        auto& targetCmd = targetCmds[index];
        RSModifiersDraw::ConvertCmdList(targetCmd);
        if (targetCmd.pixelMap != nullptr) {
            needSubmit = true;
        }
    }
    if (!needSubmit) {
        return;
    }
    auto gpuContext = RsVulkanContext::GetSingleton().GetRecyclableDrawingContext();
    gpuContext->Submit();
    RSModifiersDraw::PurgeContextResource();
    if (RSSystemProperties::GetHybridRenderParallelConvertEnabled()) {
        uint32_t taskNum = ffrtTaskNum_.fetch_add(-1);
        if (taskNum - 1 == 0) {
            cv_.notify_one();
        }
    }
}

void RSModifiersDrawThread::ConvertTransactionForCanvas(std::unique_ptr<RSTransactionData>& transactionData)
{
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
        if (hybridRenderType == Drawing::DrawCmdList::HybridRenderType::CANVAS) {
            RSModifiersDraw::ConvertCmdListForCanvas(drawCmdList, command->GetNodeId());
            hasCanvasCmdList = true;
        }
    }
    if (hasCanvasCmdList) {
        RSModifiersDrawThread::Instance().ScheduleTask([] { RSModifiersDraw::CreateNextFrameSurface(); });
    }
}

void RSModifiersDrawThread::ConvertTransactionWithFFRT(std::unique_ptr<RSTransactionData>& transactionData,
    std::shared_ptr<RSIRenderClient>& renderServiceClient, bool& isNeedCommit, std::vector<DrawOpInfo>& targetCmds)
{
    static std::unique_ptr<ffrt::queue> queue = std::make_unique<ffrt::queue>(ffrt::queue_concurrent, "ModifiersDraw",
        ffrt::queue_attr().qos(ffrt::qos_user_interactive).max_concurrency(DEFAULT_MODIFIERS_DRAW_THREAD_LOOP_NUM));
    std::vector<ffrt::task_handle> handles;
    std::atomic<size_t> cmdIndex = 0;

    // send ffrt task
    auto ffrtTask = [&targetCmds, &cmdIndex]() {
        TraverseDrawOpInfo(targetCmds, cmdIndex);
    };

    uint32_t totalPostTask =
        std::min(DEFAULT_MODIFIERS_DRAW_THREAD_LOOP_NUM, static_cast<uint32_t>(targetCmds.size()));
    ffrtTaskNum_.store(totalPostTask);
    for (uint32_t i = 0; i < totalPostTask; i++) {
        handles.emplace_back(queue->submit_h(ffrtTask));
    }

    // MD Thread convert CDN
    ConvertTransactionForCanvas(transactionData);

    // check ffrt timeout
    std::unique_lock<std::mutex> lock(ffrtMutex_);
    bool ffrtTimeout = !cv_.wait_for(lock, std::chrono::milliseconds(FFRT_WAIT_TIMEOUT),
        [] { return ffrtTaskNum_.load() == 0; });
    if (ffrtTimeout && isNeedCommit) {
        RS_LOGD("ffrt task execution timeout, commit original transactionData");
        renderServiceClient->CommitTransaction(transactionData);
        isNeedCommit = false;
    }
    // wait ffrt task
    for (auto& handle : handles) {
        queue->wait(handle);
    }
}

void RSModifiersDrawThread::ConvertTransactionWithoutFFRT(
    std::unique_ptr<RSTransactionData>& transactionData, std::vector<DrawOpInfo>& targetCmds)
{
    std::atomic<size_t> cmdIndex = 0;
    TraverseDrawOpInfo(targetCmds, cmdIndex);
    ConvertTransactionForCanvas(transactionData);
}

std::unique_ptr<RSTransactionData>& RSModifiersDrawThread::ConvertTransaction(
    std::unique_ptr<RSTransactionData>& transactionData,
    std::shared_ptr<RSIRenderClient> renderServiceClient,
    bool& isNeedCommit)
{
    // 1. extract the drawCmdList that enable hybrid render
    std::vector<DrawOpInfo> targetCmds;
    uint32_t enableHybridTextOpCnt = 0;
    // canvasDrawingNode does not use FFRT, so when collecting targetCmds, it needs to be excluded
    SeperateHybridRenderCmdList(transactionData, targetCmds, enableHybridTextOpCnt);
    if (targetCmds.empty()) {
        return transactionData;
    }

    // 2. check if the number of op exceeds the limit
    bool isEnableHybridByOpCnt = (targetCmds.size() <= HYBRID_MAX_ENABLE_OP_CNT) &&
        (enableHybridTextOpCnt <= HYBRID_MAX_TEXT_ENABLE_OP_CNT);
    bool isFirstFrame = RSModifiersDrawThread::GetIsFirstFrame();
    if (!isEnableHybridByOpCnt) {
        if (isFirstFrame) {
            RS_LOGD("the first frame's op exceeds the limit, commit original transactionData:\
                opCnt[%zu], textOpCnt[%" PRIu32 "]", targetCmds.size(), enableHybridTextOpCnt);
            renderServiceClient->CommitTransaction(transactionData);
            // still need playback when firstFrame is true
            isNeedCommit = false;
        } else {
            return transactionData;
        }
    }

    // 3. convert drawCmdList
    RS_TRACE_NAME_FMT("%s opCnt=%zu, textOpCnt=%" PRIu32 ", isFirstFrame=%d",
        __func__, targetCmds.size(), enableHybridTextOpCnt, isFirstFrame);
    RSModifiersDraw::MergeOffTreeNodeSet();
    if (RSSystemProperties::GetHybridRenderParallelConvertEnabled()) {
        ConvertTransactionWithFFRT(transactionData, renderServiceClient, isNeedCommit, targetCmds);
    } else {
        ConvertTransactionWithoutFFRT(transactionData, targetCmds);
    }

    // 4. get fence from semaphore and add pixelMap to drawOp
    RSModifiersDraw::GetFenceAndAddDrawOp(targetCmds);
    return transactionData;
}
} // namespace Rosen
} // namespace OHOS
