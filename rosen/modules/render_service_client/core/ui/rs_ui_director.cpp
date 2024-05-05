/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#include "ui/rs_ui_director.h"

#include "rs_trace.h"
#include "sandbox_utils.h"

#include "command/rs_message_processor.h"
#include "hyper_graphic_manager/core/utils/hgm_command.h"
#include "modifier/rs_modifier_manager.h"
#include "modifier/rs_modifier_manager_map.h"
#include "pipeline/rs_node_map.h"
#include "pipeline/rs_render_thread.h"
#include "platform/common/rs_log.h"
#include "rs_frame_report.h"
#include "transaction/rs_application_agent_impl.h"
#include "transaction/rs_interfaces.h"
#include "transaction/rs_transaction.h"
#include "transaction/rs_transaction_proxy.h"
#include "ui/rs_frame_rate_policy.h"
#include "ui/rs_root_node.h"
#include "ui/rs_surface_extractor.h"
#include "ui/rs_surface_node.h"
#ifdef NEW_RENDER_CONTEXT
#include "render_context/memory_handler.h"
#endif

#ifdef _WIN32
#include <windows.h>
#define gettid GetCurrentThreadId
#endif

#ifdef __APPLE__
#define gettid getpid
#endif

#ifdef __gnu_linux__
#include <sys/types.h>
#include <sys/syscall.h>
#define gettid []() -> int32_t { return static_cast<int32_t>(syscall(SYS_gettid)); }
#endif

namespace OHOS {
namespace Rosen {
static std::unordered_map<RSUIDirector*, TaskRunner> g_uiTaskRunners;
static std::mutex g_uiTaskRunnersVisitorMutex;
std::function<void()> RSUIDirector::requestVsyncCallback_ = nullptr;
static std::mutex g_vsyncCallbackMutex;

std::shared_ptr<RSUIDirector> RSUIDirector::Create()
{
    return std::shared_ptr<RSUIDirector>(new RSUIDirector());
}

RSUIDirector::~RSUIDirector()
{
    Destroy();
}

void RSUIDirector::Init(bool shouldCreateRenderThread)
{
    AnimationCommandHelper::SetAnimationCallbackProcessor(AnimationCallbackProcessor);

    isUniRenderEnabled_ = RSSystemProperties::GetUniRenderEnabled();
    if (shouldCreateRenderThread && !isUniRenderEnabled_) {
        auto renderThreadClient = RSIRenderClient::CreateRenderThreadClient();
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            transactionProxy->SetRenderThreadClient(renderThreadClient);
        }

        RsFrameReport::GetInstance().Init();
        if (!cacheDir_.empty()) {
            RSRenderThread::Instance().SetCacheDir(cacheDir_);
        }
        RSRenderThread::Instance().Start();
    } else {
        // force fallback animaiions send to RS if no render thread
        RSNodeMap::Instance().GetAnimationFallbackNode()->isRenderServiceNode_ = true;
    }
    if (auto rsApplicationAgent = RSApplicationAgentImpl::Instance()) {
        rsApplicationAgent->RegisterRSApplicationAgent();
    }

    GoForeground();
}

void RSUIDirector::SetFlushEmptyCallback(FlushEmptyCallback flushEmptyCallback)
{
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->SetFlushEmptyCallback(flushEmptyCallback);
    }
}

void RSUIDirector::StartTextureExport()
{
    isUniRenderEnabled_ = RSSystemProperties::GetUniRenderEnabled();
    if (isUniRenderEnabled_) {
        auto renderThreadClient = RSIRenderClient::CreateRenderThreadClient();
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            transactionProxy->SetRenderThreadClient(renderThreadClient);
        }
        RSRenderThread::Instance().Start();
    }
    RSRenderThread::Instance().UpdateWindowStatus(true);
}

void RSUIDirector::GoForeground(bool isTextureExport)
{
    ROSEN_LOGD("RSUIDirector::GoForeground");
    if (!isActive_) {
        if (!isUniRenderEnabled_ || isTextureExport) {
            RSRenderThread::Instance().UpdateWindowStatus(true);
        }
        isActive_ = true;
        if (auto node = RSNodeMap::Instance().GetNode<RSRootNode>(root_)) {
            node->SetEnableRender(true);
        }
        auto surfaceNode = surfaceNode_.lock();
        if (surfaceNode) {
            surfaceNode->MarkUIHidden(false);
        }
    }
}

void RSUIDirector::GoBackground(bool isTextureExport)
{
    ROSEN_LOGD("RSUIDirector::GoBackground");
    if (isActive_) {
        if (!isUniRenderEnabled_ || isTextureExport) {
            RSRenderThread::Instance().UpdateWindowStatus(false);
        }
        isActive_ = false;
        if (auto node = RSNodeMap::Instance().GetNode<RSRootNode>(root_)) {
            node->SetEnableRender(false);
        }
        auto surfaceNode = surfaceNode_.lock();
        if (surfaceNode) {
            surfaceNode->MarkUIHidden(true);
        }
        if (isTextureExport || isUniRenderEnabled_) {
            return;
        }
        // clean bufferQueue cache
        RSRenderThread::Instance().PostTask([surfaceNode]() {
            if (surfaceNode != nullptr) {
#ifdef NEW_RENDER_CONTEXT
                std::shared_ptr<RSRenderSurface> rsSurface = RSSurfaceExtractor::ExtractRSSurface(surfaceNode);
#else
                std::shared_ptr<RSSurface> rsSurface = RSSurfaceExtractor::ExtractRSSurface(surfaceNode);
#endif
                rsSurface->ClearBuffer();
            }
        });
#if defined(ACE_ENABLE_GL) || defined(ACE_ENABLE_VK)
        RSRenderThread::Instance().PostTask([this]() {
            auto renderContext = RSRenderThread::Instance().GetRenderContext();
            if (renderContext != nullptr) {
#ifndef ROSEN_CROSS_PLATFORM
#if defined(NEW_RENDER_CONTEXT)
                auto drawingContext = RSRenderThread::Instance().GetDrawingContext();
                MemoryHandler::ClearRedundantResources(drawingContext->GetDrawingContext());
#else
                renderContext->ClearRedundantResources();
#endif
#endif
            }
        });
#endif
    }
}

void RSUIDirector::Destroy(bool isTextureExport)
{
    if (root_ != 0) {
        if (!isUniRenderEnabled_ || isTextureExport) {
            if (auto node = RSNodeMap::Instance().GetNode<RSRootNode>(root_)) {
                node->RemoveFromTree();
            }
        }
        root_ = 0;
    }
    GoBackground(isTextureExport);
    std::unique_lock<std::mutex> lock(g_uiTaskRunnersVisitorMutex);
    g_uiTaskRunners.erase(this);
}

void RSUIDirector::SetRSSurfaceNode(std::shared_ptr<RSSurfaceNode> surfaceNode)
{
    surfaceNode_ = surfaceNode;
    AttachSurface();
}

void RSUIDirector::SetAbilityBGAlpha(uint8_t alpha)
{
    auto node = surfaceNode_.lock();
    if (!node) {
        ROSEN_LOGI("RSUIDirector::SetAbilityBGAlpha, surfaceNode_ is nullptr");
        return;
    }
    node->SetAbilityBGAlpha(alpha);
}

void RSUIDirector::SetRTRenderForced(bool isRenderForced)
{
    RSRenderThread::Instance().SetRTRenderForced(isRenderForced);
}

void RSUIDirector::SetContainerWindow(bool hasContainerWindow, float density)
{
    auto node = surfaceNode_.lock();
    if (!node) {
        ROSEN_LOGI("RSUIDirector::SetContainerWindow, surfaceNode_ is nullptr");
        return;
    }
    node->SetContainerWindow(hasContainerWindow, density);
}

void RSUIDirector::SetRoot(NodeId root)
{
    if (root_ == root) {
        ROSEN_LOGW("RSUIDirector::SetRoot, root_ is not change");
        return;
    }
    root_ = root;
    AttachSurface();
}

void RSUIDirector::AttachSurface()
{
    auto node = RSNodeMap::Instance().GetNode<RSRootNode>(root_);
    auto surfaceNode = surfaceNode_.lock();
    if (node != nullptr && surfaceNode != nullptr) {
        node->AttachRSSurfaceNode(surfaceNode);
        ROSEN_LOGD("RSUIDirector::AttachSurface [%{public}" PRIu64, surfaceNode->GetId());
    } else {
        ROSEN_LOGD("RSUIDirector::AttachSurface not ready");
    }
}

void RSUIDirector::SetAppFreeze(bool isAppFreeze)
{
    auto surfaceNode = surfaceNode_.lock();
    if (surfaceNode != nullptr) {
        surfaceNode->SetFreeze(isAppFreeze);
    }
}

void RSUIDirector::SetRequestVsyncCallback(const std::function<void()>& callback)
{
    std::unique_lock<std::mutex> lock(g_vsyncCallbackMutex);
    requestVsyncCallback_ = callback;
}

void RSUIDirector::SetTimeStamp(uint64_t timeStamp, const std::string& abilityName)
{
    timeStamp_ = timeStamp;
    abilityName_ = abilityName;
}

void RSUIDirector::SetCacheDir(const std::string& cacheFilePath)
{
    cacheDir_ = cacheFilePath;
}

bool RSUIDirector::FlushAnimation(uint64_t timeStamp, int64_t vsyncPeriod)
{
    bool hasRunningAnimation = false;
    auto modifierManager = RSModifierManagerMap::Instance()->GetModifierManager(gettid());
    if (modifierManager != nullptr) {
        modifierManager->SetDisplaySyncEnable(GetCurrentRefreshRateMode() == HGM_REFRESHRATE_MODE_AUTO);
        modifierManager->SetFrameRateGetFunc([](const RSPropertyUnit unit, float velocity) -> int32_t {
            return RSFrameRatePolicy::GetInstance()->GetExpectedFrameRate(unit, velocity);
        });
        hasRunningAnimation = modifierManager->Animate(timeStamp, vsyncPeriod);
    }
    return hasRunningAnimation;
}

void RSUIDirector::FlushAnimationStartTime(uint64_t timeStamp)
{
    auto modifierManager = RSModifierManagerMap::Instance()->GetModifierManager(gettid());
    if (modifierManager != nullptr) {
        modifierManager->FlushStartAnimation(timeStamp);
    }
}

void RSUIDirector::FlushModifier()
{
    auto modifierManager = RSModifierManagerMap::Instance()->GetModifierManager(gettid());
    if (modifierManager == nullptr) {
        return;
    }

    modifierManager->Draw();
    // post animation finish callback(s) to task queue
    RSUIDirector::RecvMessages();
}

bool RSUIDirector::HasUIAnimation()
{
    auto modifierManager = RSModifierManagerMap::Instance()->GetModifierManager(gettid());
    if (modifierManager != nullptr) {
        return modifierManager->HasUIAnimation();
    }
    return false;
}

void RSUIDirector::SetUITaskRunner(const TaskRunner& uiTaskRunner, int32_t instanceId)
{
    std::unique_lock<std::mutex> lock(g_uiTaskRunnersVisitorMutex);
    instanceId_ = instanceId;
    g_uiTaskRunners[this] = uiTaskRunner;
    if (!isHgmConfigChangeCallbackReg_) {
        RSFrameRatePolicy::GetInstance()->RegisterHgmConfigChangeCallback();
        isHgmConfigChangeCallbackReg_ = true;
    }
}

void RSUIDirector::SendMessages()
{
    ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "SendCommands");
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->FlushImplicitTransaction(timeStamp_, abilityName_);
    }
    ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
}

void RSUIDirector::RecvMessages()
{
    if (GetRealPid() == -1) {
        return;
    }
    static const uint32_t pid = static_cast<uint32_t>(GetRealPid());
    static std::mutex recvMessagesMutex;
    std::unique_lock<std::mutex> lock(recvMessagesMutex);
    if (RSMessageProcessor::Instance().HasTransaction(pid)) {
        auto transactionDataPtr = RSMessageProcessor::Instance().GetTransaction(pid);
        RecvMessages(transactionDataPtr);
    }
}

void RSUIDirector::RecvMessages(std::shared_ptr<RSTransactionData> cmds)
{
    if (cmds == nullptr || cmds->IsEmpty()) {
        return;
    }
    ROSEN_LOGD("ProcessMessages begin");
    RSUIDirector::ProcessMessages(cmds);
}

void RSUIDirector::ProcessMessages(std::shared_ptr<RSTransactionData> cmds)
{
    std::map<int32_t, std::vector<std::unique_ptr<RSCommand>>> m;
    for (auto &[id, _, cmd] : cmds->GetPayload()) {
        m[RSNodeMap::Instance().GetNodeInstanceId(id)].push_back(std::move(cmd));
    }
    auto counter = std::make_shared<std::atomic_size_t>(m.size());
    for (auto &[instanceId, commands] : m) {
        PostTask([cmds = std::make_shared<std::vector<std::unique_ptr<RSCommand>>>(std::move(commands)), counter] {
            for (auto &cmd : *cmds) {
                RSContext context; // RSCommand->process() needs it
                cmd->Process(context);
            }
            if (counter->fetch_sub(1) == 1) {
                std::unique_lock<std::mutex> lock(g_vsyncCallbackMutex);
                if (requestVsyncCallback_ != nullptr) {
                    requestVsyncCallback_();
                } else {
                    RSTransaction::FlushImplicitTransaction();
                }
                ROSEN_LOGD("ProcessMessages end");
            }
        }, instanceId);
    }
}

void RSUIDirector::AnimationCallbackProcessor(NodeId nodeId, AnimationId animId, AnimationCallbackEvent event)
{
    // try find the node by nodeId
    if (auto nodePtr = RSNodeMap::Instance().GetNode<RSNode>(nodeId)) {
        if (!nodePtr->AnimationCallback(animId, event)) {
            ROSEN_LOGE("RSUIDirector::AnimationCallbackProcessor, could not find animation %{public}" PRIu64 ""
                "on node %{public}" PRIu64, animId, nodeId);
        }
        return;
    }

    // if node not found, try fallback node
    auto& fallbackNode = RSNodeMap::Instance().GetAnimationFallbackNode();
    if (fallbackNode && fallbackNode->AnimationCallback(animId, event)) {
        ROSEN_LOGD("RSUIDirector::AnimationCallbackProcessor, found animation %{public}" PRIu64 " on fallback node.",
            animId);
    } else {
        ROSEN_LOGE("RSUIDirector::AnimationCallbackProcessor, could not find animation %{public}" PRIu64 " on"
            " fallback node.", animId);
    }
}

void RSUIDirector::PostFrameRateTask(const std::function<void()>& task)
{
    PostTask(task);
}

void RSUIDirector::PostTask(const std::function<void()>& task, int32_t instanceId)
{
    std::unique_lock<std::mutex> lock(g_uiTaskRunnersVisitorMutex);
    for (const auto &[director, taskRunner] : g_uiTaskRunners) {
        if (director->instanceId_ != instanceId) {
            continue;
        }
        ROSEN_LOGD("RSUIDirector::PostTask instanceId=%{public}d success", instanceId);
        taskRunner(task);
        return;
    }
    if (instanceId != INSTANCE_ID_UNDEFINED) {
        ROSEN_LOGW("RSUIDirector::PostTask instanceId=%{public}d not found", instanceId);
    }
    for (const auto &[_, taskRunner] : g_uiTaskRunners) {
        ROSEN_LOGD("RSUIDirector::PostTask success");
        taskRunner(task);
        return;
    }
}

int32_t RSUIDirector::GetCurrentRefreshRateMode()
{
    return RSFrameRatePolicy::GetInstance()->GetRefreshRateModeName();
}

int32_t RSUIDirector::GetAnimateExpectedRate() const
{
    int32_t animateRate = 0;
    auto modifierManager = RSModifierManagerMap::Instance()->GetModifierManager(gettid());
    if (modifierManager != nullptr) {
        auto& range = modifierManager->GetFrameRateRange();
        if (range.IsValid()) {
            animateRate = range.preferred_;
        }
    }
    return animateRate;
}
} // namespace Rosen
} // namespace OHOS
