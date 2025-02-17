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

#include "animation/rs_animation_trace_utils.h"
#include "command/rs_message_processor.h"
#include "command/rs_node_command.h"
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
#include "feature/hyper_graphic_manager/rs_frame_rate_policy.h"
#include "ui/rs_root_node.h"
#include "ui/rs_surface_extractor.h"
#include "ui/rs_surface_node.h"

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
std::function<void()> RSUIDirector::requestVsyncCallback_ = nullptr;
static std::mutex g_vsyncCallbackMutex;
static std::once_flag g_initDumpNodeTreeProcessorFlag;

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
    std::call_once(g_initDumpNodeTreeProcessorFlag, [] () {
        RSNodeCommandHelper::SetDumpNodeTreeProcessor(RSUIDirector::DumpNodeTreeProcessor);
    });

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
    if (!cacheDir_.empty()) {
        RSRenderThread::Instance().SetCacheDir(cacheDir_);
    }
    if (auto rsApplicationAgent = RSApplicationAgentImpl::Instance()) {
        rsApplicationAgent->RegisterRSApplicationAgent();
    }

    GoForeground();
    RSInterpolator::Init();
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
            surfaceNode->SetAbilityState(RSSurfaceNodeAbilityState::FOREGROUND);
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
            surfaceNode->SetAbilityState(RSSurfaceNodeAbilityState::BACKGROUND);
        }
        if (isTextureExport || isUniRenderEnabled_) {
            return;
        }
        // clean bufferQueue cache
        RSRenderThread::Instance().PostTask([surfaceNode]() {
            if (surfaceNode != nullptr) {
                std::shared_ptr<RSSurface> rsSurface = RSSurfaceExtractor::ExtractRSSurface(surfaceNode);
                if (rsSurface == nullptr) {
                    ROSEN_LOGE("rsSurface is nullptr");
                    return;
                }
                rsSurface->ClearBuffer();
            }
        });
#if defined(ACE_ENABLE_GL) || defined(ACE_ENABLE_VK)
        RSRenderThread::Instance().PostTask([this]() {
            auto renderContext = RSRenderThread::Instance().GetRenderContext();
            if (renderContext != nullptr) {
#ifndef ROSEN_CROSS_PLATFORM
                renderContext->ClearRedundantResources();
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
    std::unique_lock<std::mutex> lock(uiTaskRunnersVisitorMutex);
    uiTaskRunners.erase(this);
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

void RSUIDirector::SetContainerWindow(bool hasContainerWindow, RRect rrect)
{
    auto node = surfaceNode_.lock();
    if (!node) {
        ROSEN_LOGI("RSUIDirector::SetContainerWindow, surfaceNode_ is nullptr");
        return;
    }
    node->SetContainerWindow(hasContainerWindow, rrect);
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
    auto modifierManager = RSModifierManagerMap::Instance()->GetModifierManager(gettid());
    if (modifierManager != nullptr) {
        modifierManager->SetDisplaySyncEnable(true);
        modifierManager->SetFrameRateGetFunc(
            [](const RSPropertyUnit unit, float velocity, int32_t area, int length) -> int32_t {
                return RSFrameRatePolicy::GetInstance()->GetExpectedFrameRate(unit, velocity);
            }
        );
        return modifierManager->Animate(timeStamp, vsyncPeriod);
    }
    return false;
}

bool RSUIDirector::HasFirstFrameAnimation()
{
    auto modifierManager = RSModifierManagerMap::Instance()->GetModifierManager(gettid());
    if (modifierManager != nullptr) {
        return modifierManager->GetAndResetFirstFrameAnimationState();
    }
    return false;
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

bool RSUIDirector::HasUIRunningAnimation()
{
    auto modifierManager = RSModifierManagerMap::Instance()->GetModifierManager(gettid());
    if (modifierManager != nullptr) {
        return modifierManager->HasUIRunningAnimation();
    }
    return false;
}

void RSUIDirector::SetUITaskRunner(const TaskRunner& uiTaskRunner, int32_t instanceId)
{
    std::unique_lock<std::mutex> lock(uiTaskRunnersVisitorMutex);
    instanceId_ = instanceId;
    uiTaskRunners[this] = uiTaskRunner;
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
        index_ = transactionProxy->GetTransactionDataIndex();
    } else {
        RS_LOGE_LIMIT(__func__, __line__, "RSUIDirector::SendMessages failed, transactionProxy is nullptr");
    }
    ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
}

uint32_t RSUIDirector::GetIndex() const
{
    return index_;
}

void RSUIDirector::RecvMessages()
{
    if (GetRealPid() == -1) {
        return;
    }
    static const uint32_t pid = static_cast<uint32_t>(GetRealPid());
    if (!RSMessageProcessor::Instance().HasTransaction(pid)) {
        return;
    }
    auto transactionDataPtr = RSMessageProcessor::Instance().GetTransaction(pid);
    RecvMessages(transactionDataPtr);
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
    // message ID for correspondence UI thread and IPC thread
    static uint32_t messageId = 0;
    std::map<int32_t, std::vector<std::unique_ptr<RSCommand>>> m;
    for (auto &[id, _, cmd] : cmds->GetPayload()) {
        NodeId realId = (id == 0 && cmd) ? cmd->GetNodeId() : id;
        int32_t instanceId = RSNodeMap::Instance().GetNodeInstanceId(realId);
        if (instanceId == INSTANCE_ID_UNDEFINED) {
            instanceId = RSNodeMap::Instance().GetInstanceIdForReleasedNode(realId);
        }
        m[instanceId].push_back(std::move(cmd));
    }
    auto msgId = ++messageId;
    RS_TRACE_NAME_FMT("RSUIDirector::ProcessMessages Post [messageId:%lu,cmdIndex:%llu,cmdCount:%lu]",
        msgId, cmds->GetIndex(), cmds->GetCommandCount());
    auto counter = std::make_shared<std::atomic_size_t>(m.size());
    for (auto &[instanceId, commands] : m) {
        ROSEN_LOGI("Post messageId:%{public}d, cmdCount:%{public}lu, instanceId:%{public}d", msgId,
            static_cast<unsigned long>(commands.size()), instanceId);
        PostTask(
            [cmds = std::make_shared<std::vector<std::unique_ptr<RSCommand>>>(std::move(commands)),
                counter, msgId, tempInstanceId = instanceId] {
                RS_TRACE_NAME_FMT("RSUIDirector::ProcessMessages Process messageId:%lu", msgId);
                ROSEN_LOGI("Process messageId:%{public}d, cmdCount:%{public}lu, instanceId:%{public}d",
                    msgId, static_cast<unsigned long>(cmds->size()), tempInstanceId);
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
    RSAnimationTraceUtils::GetInstance().addAnimationFinishTrace(
        "Animation FinishCallback Processor", nodeId, animId, false);
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

void RSUIDirector::DumpNodeTreeProcessor(NodeId nodeId, pid_t pid, uint32_t taskId)
{
    RS_TRACE_NAME_FMT("DumpClientNodeTree dump task[%u] node[%" PRIu64 "]", taskId, nodeId);
    ROSEN_LOGI("DumpNodeTreeProcessor task[%{public}u] node[%" PRIu64 "]", taskId, nodeId);

    std::string out;
    // use for dump transactionFlags [pid,index] in client tree dump
    int32_t instanceId = RSNodeMap::Instance().GetNodeInstanceId(nodeId);
    {
        std::unique_lock<std::mutex> lock(uiTaskRunnersVisitorMutex);
        for (const auto &[director, taskRunner] : uiTaskRunners) {
            if (director->instanceId_ == instanceId) {
                out.append("transactionFlags:[ ").append(std::to_string(pid).append(", ")
                    .append(std::to_string(director->index_)).append("]\r"));
                break;
            }
        }
    }

    if (auto node = RSNodeMap::Instance().GetNode(nodeId)) {
        constexpr int TOP_LEVEL_DEPTH = 1;
        node->DumpTree(TOP_LEVEL_DEPTH, out);
    }

    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        std::unique_ptr<RSCommand> command = std::make_unique<RSCommitDumpClientNodeTree>(
            nodeId, getpid(), taskId, out);
        transactionProxy->AddCommand(command, true);
        RSTransaction::FlushImplicitTransaction();
    }
}

void RSUIDirector::PostFrameRateTask(const std::function<void()>& task)
{
    PostTask(task);
}

void RSUIDirector::PostTask(const std::function<void()>& task, int32_t instanceId)
{
    PostDelayTask(task, 0, instanceId);
}

void RSUIDirector::PostDelayTask(const std::function<void()>& task, uint32_t delay, int32_t instanceId)
{
    std::unique_lock<std::mutex> lock(uiTaskRunnersVisitorMutex);
    for (const auto &[director, taskRunner] : uiTaskRunners) {
        if (director->instanceId_ != instanceId) {
            continue;
        }
        ROSEN_LOGD("RSUIDirector::PostTask instanceId=%{public}d success", instanceId);
        taskRunner(task, delay);
        return;
    }
    if (instanceId != INSTANCE_ID_UNDEFINED) {
        ROSEN_LOGW("RSUIDirector::PostTask instanceId=%{public}d not found", instanceId);
    }
    for (const auto &[_, taskRunner] : uiTaskRunners) {
        ROSEN_LOGD("RSUIDirector::PostTask success");
        taskRunner(task, delay);
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
