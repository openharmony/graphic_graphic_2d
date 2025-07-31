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
#include "modifier_render_thread/rs_modifiers_draw.h"
#include "platform/common/rs_log.h"
#include "qos.h"
#include "render_context/shader_cache.h"
#include "concurrent_task_client.h"

namespace OHOS {
namespace Rosen {
std::atomic<bool> RSModifiersDrawThread::isStarted_ = false;
std::recursive_mutex RSModifiersDrawThread::transactionDataMutex_;
bool RSModifiersDrawThread::isFirstFrame_ = true;

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

bool RSModifiersDrawThread::GetIsFirstFrame()
{
    return isFirstFrame_;
}

void RSModifiersDrawThread::SetIsFirstFrame(bool isFirstFrame)
{
    isFirstFrame_ = isFirstFrame;
}

std::unique_ptr<RSTransactionData>& RSModifiersDrawThread::ConvertTransaction(
    std::unique_ptr<RSTransactionData>& transactionData,
    std::shared_ptr<RSIRenderClient> renderServiceClient,
    bool& isNeedCommit)
{
    // 1. extract the drawCmdList that enable hybrid render
    std::vector<DrawOpInfo> targetCmds;
    uint32_t enableHybridTextOpCnt = 0;
    bool isEnableHybridByOpCnt =
        RSModifiersDraw::SeperateHybridRenderCmdList(transactionData, targetCmds, enableHybridTextOpCnt);
    if (targetCmds.empty()) {
        return transactionData;
    }

    // 2. check if the number of op exceeds the limit
    bool isFirstFrame = RSModifiersDrawThread::GetIsFirstFrame();
    if (!isEnableHybridByOpCnt) {
        if (isFirstFrame) {
            RS_TRACE_NAME_FMT("the first frame's op exceeds the limit, commit original transactionData");
            renderServiceClient->CommitTransaction(transactionData);
            // still need playback when firstFrame is true
            isNeedCommit = false;
        } else {
            return transactionData;
        }
    }

    // 3. convert drawCmdList
    RS_TRACE_NAME_FMT("%s opCnt=%zu, textOpCnt=%" PRIu32 ", isEnableHybridByOpCnt=%d, isFirstFrame=%d",
        __func__, targetCmds.size(), enableHybridTextOpCnt, isEnableHybridByOpCnt, isFirstFrame);
    RSModifiersDraw::MergeOffTreeNodeSet();
    if (RSSystemProperties::GetHybridRenderParallelConvertEnabled()) {
        RSModifiersDraw::ConvertTransactionWithFFRT(transactionData, renderServiceClient, isNeedCommit, targetCmds);
    } else {
        RSModifiersDraw::ConvertTransactionWithoutFFRT(transactionData, targetCmds);
    }

    // 4. get fence from semaphore and add pixelMap to drawOp
    RSModifiersDraw::GetFenceAndAddDrawOp(targetCmds);
    return transactionData;
}
} // namespace Rosen
} // namespace OHOS
