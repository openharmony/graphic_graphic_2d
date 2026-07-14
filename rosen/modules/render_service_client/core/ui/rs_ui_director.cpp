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
#include "common/rs_optional_trace.h"
#include "sandbox_utils.h"
#include "platform/common/rs_system_properties.h"

#include "animation/rs_animation_trace_utils.h"
#include "command/rs_message_processor.h"
#include "command/rs_ui_director_command.h"
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
#include "ui/rs_ui_context.h"
#include "ui/rs_ui_context_manager.h"
#ifndef ROSEN_CROSS_PLATFORM
#include "platform/ohos/transaction/zidl/rs_iconnect_to_render_process.h"
#endif

#ifdef _WIN32
#include <windows.h>
#define gettid GetCurrentThreadId
#endif

#ifdef __APPLE__
#define gettid getpid
#endif

#ifdef __gnu_linux__
#include <sys/syscall.h>
#include <sys/types.h>
#define gettid []()->int32_t { return static_cast<int32_t>(syscall(SYS_gettid)); }
#endif

namespace OHOS {
namespace Rosen {
constexpr int32_t INSTANCE_ID_UNDEFINED_TASK_RUNNER = 0;
static std::mutex g_vsyncCallbackMutex;
static std::once_flag g_initDumpNodeTreeProcessorFlag;
constexpr int NODE_ID = 1; // Image size difference

std::shared_ptr<RSUIDirector> RSUIDirector::Create(sptr<IRemoteObject> connectToRenderRemote,
    std::shared_ptr<RSUIContext> rsUIContext)
{
    std::shared_ptr<RSUIDirector> rsUIDirector = std::shared_ptr<RSUIDirector>(new RSUIDirector());
    rsUIDirector->Init(connectToRenderRemote, rsUIContext);
    return rsUIDirector;
}

RSUIDirector::~RSUIDirector()
{
    auto uiContext = rsUIContext_;
    Destroy();
    if (uiContext != nullptr) {
        uiContext->DestroyModifiersDraw();
    }
}

void RSUIDirector::Init(sptr<IRemoteObject>& connectToRenderRemote, std::shared_ptr<RSUIContext> rsUIContext)
{
    AnimationCommandHelper::SetAnimationCallbackProcessor(AnimationCallbackProcessor);
    AnimationCommandHelper::SetAnimationDestroyInRenderProcessor(AnimationDestroyInRenderCallbackProcessor);
    RSNodeCommandHelper::SetColorPickerCallbackProcessor(ColorPickerCallbackProcessor);
    std::call_once(g_initDumpNodeTreeProcessorFlag,
        []() { RSNodeCommandHelper::SetDumpNodeTreeProcessor(RSUIDirector::DumpNodeTreeProcessor); });

    if (rsUIContext != nullptr) {
        rsUIContext_ = rsUIContext;
        skipDestroyUIContext_ = true;
    } else {
        rsUIContext_ = RSUIContextManager::MutableInstance().CreateRSUIContext(connectToRenderRemote);
    }

    isUniRenderEnabled_ = RSSystemProperties::GetUniRenderEnabled();
    if (!isUniRenderEnabled_) {
        auto renderThreadClient = RSIRenderClient::CreateRenderThreadClient();
        if (rsUIContext_ != nullptr) {
            auto transaction = rsUIContext_->GetRSTransaction();
            if (transaction != nullptr) {
                transaction->SetRenderThreadClient(renderThreadClient);
            }
        } else {
            auto transactionProxy = RSTransactionProxy::GetInstance();
            if (transactionProxy != nullptr) {
                transactionProxy->SetRenderThreadClient(renderThreadClient);
            }
        }

        RsFrameReport::InitDeadline();
        RSRenderThread::Instance().Start();
    } else {
        // force fallback animaiions send to RS if no render thread
        RSNodeMap::Instance().GetAnimationFallbackNode()->isRenderServiceNode_ = true;
#ifdef RS_MODIFIERS_DRAW_ENABLE
        InitHybridRender();
#endif
    }
    if (!cacheDir_.empty()) {
        RSRenderThread::Instance().SetCacheDir(cacheDir_);
    }
    if (auto rsApplicationAgent = RSApplicationAgentImpl::Instance()) {
        rsApplicationAgent->RegisterRSApplicationAgent(rsUIContext_);
    }
    GoCreate();
    GoResume();
    GoForeground();
    RSInterpolator::Init();
    RSAnimationFraction::Init();
}

void RSUIDirector::SetFlushEmptyCallback(FlushEmptyCallback flushEmptyCallback)
{
    if (rsUIContext_) {
        auto transaction = rsUIContext_->GetRSTransaction();
        if (transaction != nullptr) {
            transaction->SetFlushEmptyCallback(flushEmptyCallback);
        }
    } else {
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            transactionProxy->SetFlushEmptyCallback(flushEmptyCallback);
        }
    }
}
#ifdef RS_MODIFIERS_DRAW_ENABLE
void RSUIDirector::InitHybridRender()
{
    if (!RSSystemProperties::GetHybridRenderCanvasEnabled()) {
        return;
    }
    if (rsUIContext_ == nullptr) {
        return;
    }
    if (auto transaction = rsUIContext_->GetRSTransaction()) {
        transaction->SetCommitTransactionCallback(rsUIContext_->CreateCommitTransactionCallback());
    }
}
#endif // RS_MODIFIERS_DRAW_ENABLE

RSUIDirectorLifecycleState RSUIDirector::GetCurrentState() const
{
    return currentUIDirectorState_;
}

bool RSUIDirector::GetHybridRenderCanvasEnabled()
{
    return RSSystemProperties::GetHybridRenderCanvasEnabled();
}

std::shared_ptr<RSUIDirector> RSUIDirector::CreateRSUIDirector()
{
    std::shared_ptr<RSUIDirector> rsUIDirector = std::shared_ptr<RSUIDirector>(new RSUIDirector());
    return rsUIDirector;
}

void RSUIDirector::StartTextureExport(std::shared_ptr<RSUIContext> rsUIContext)
{
    isUniRenderEnabled_ = RSSystemProperties::GetUniRenderEnabled();
    if (isUniRenderEnabled_) {
        auto renderThreadClient = RSIRenderClient::CreateRenderThreadClient();
        if (rsUIContext) {
            auto transaction = rsUIContext->GetRSTransaction();
            if (transaction != nullptr) {
                transaction->SetRenderThreadClient(renderThreadClient);
            }
        } else {
            auto transactionProxy = RSTransactionProxy::GetInstance();
            if (transactionProxy != nullptr) {
                transactionProxy->SetRenderThreadClient(renderThreadClient);
            }
        }
        RSRenderThread::Instance().Start();
    }
    RSRenderThread::Instance().UpdateWindowStatus(true);
}

template <typename CommandType>
void RSUIDirector::AddUIDirectorCommand()
{
    if (rsUIContext_ == nullptr) {
        RS_LOGE("RSUIDirector::AddUIDirectorCommand, RSUIContext is nullptr");
        return;
    }

    auto transaction = rsUIContext_->GetRSTransaction();
    if (transaction == nullptr) {
        RS_LOGE("RSUIDirector::AddUIDirectorCommand, transaction is nullptr");
        return;
    }

    auto rootNode = rootNode_.lock();
    static pid_t pid = getpid();
    NodeId nodeId = rootNode ? rootNode->GetId() : (((NodeId)pid << 32) | NODE_ID);
    std::unique_ptr<RSCommand> command =
        std::make_unique<CommandType>(nodeId, rsUIContext_ ? rsUIContext_->GetToken() : 0);
    RS_TRACE_NAME_FMT(
        "RSUIDirector::AddUIDirectorCommand type is %d, token is %lu", command->GetSubType(), rsUIContext_->GetToken());
    transaction->AddCommand(command, true);
}

void RSUIDirector::GoCreate()
{
    RS_TRACE_NAME_FMT("RSUIDirector::GoCreate CurrentState:%d, UIContext:%lu",
        static_cast<int>(currentUIDirectorState_), rsUIContext_ ? rsUIContext_->GetToken() : 0);
    RS_LOGI("RSUIDirector::GoCreate CurrentState:%{public}d, UIContext:%{public}" PRIu64,
        static_cast<int>(currentUIDirectorState_), rsUIContext_ ? rsUIContext_->GetToken() : 0);
    // Generate command
    ExecuteGoCreate();
    AddUIDirectorCommand<RSUIDirectorGoCreate>();
    // Change the state
    currentUIDirectorState_ = RSUIDirectorLifecycleState::CREATE;
}

void RSUIDirector::ExecuteGoCreate()
{}

void RSUIDirector::GoResume()
{
    RS_TRACE_NAME_FMT("RSUIDirector::GoResume CurrentState:%d, UIContext:%lu",
        static_cast<int>(currentUIDirectorState_), rsUIContext_ ? rsUIContext_->GetToken() : 0);
    RS_LOGI("RSUIDirector::GoResume CurrentState:%{public}d, UIContext:%{public}" PRIu64,
        static_cast<int>(currentUIDirectorState_), rsUIContext_ ? rsUIContext_->GetToken() : 0);
    // Invalid states
    if (currentUIDirectorState_ == RSUIDirectorLifecycleState::DESTROYED ||
        currentUIDirectorState_ == RSUIDirectorLifecycleState::FOREGROUND ||
        currentUIDirectorState_ == RSUIDirectorLifecycleState::BACKGROUND) {
        RS_LOGE("RSUIDirector::GoResume, CurrentState %{public}d is invalid.",
            static_cast<int>(currentUIDirectorState_));
        return;
    }
    // Normal states
    if (currentUIDirectorState_ == RSUIDirectorLifecycleState::CREATE ||
        currentUIDirectorState_ == RSUIDirectorLifecycleState::STOP) {
        // Resume directly
        ExecuteGoResume();
        AddUIDirectorCommand<RSUIDirectorGoResume>();
    }
    currentUIDirectorState_ = RSUIDirectorLifecycleState::RESUME;
}

// Excute resume
void RSUIDirector::ExecuteGoResume()
{
    if (RSSystemProperties::IsRenderNodeRebuildEnabled() && RSSystemProperties::GetBackgroundRebuildEnabled()) {
        RebuildNodeTree();
    }
}

void RSUIDirector::RebuildNodeTree()
{
    if (auto rootNode = rootNode_.lock()) {
        RS_TRACE_NAME_FMT("GoResume");
        if (rsUIContext_) {
            rsUIContext_->SetRebuildState(RebuildState::Rebuilding);
            auto transaction = rsUIContext_->GetRSTransaction();
            if (RSSystemProperties::GetRebuildSceneEnabled()) {
                transaction->SetRSTransactionDataScene(RSTransactionDataScenes::Rebuild);
            }
        }
        rootNode->RebuildTree();
        if (rsUIContext_) {
            rsUIContext_->SetRebuildState(RebuildState::Normal);
#ifdef RS_MODIFIERS_DRAW_ENABLE
            rsUIContext_->FlushCanvasDrawingNodeBuffers();
#endif
        }
    }
}

void RSUIDirector::GoForeground(bool isTextureExport)
{
    RS_TRACE_NAME_FMT("RSUIDirector::GoForeground CurrentState:%d, UIContext:%lu",
        static_cast<int>(currentUIDirectorState_), rsUIContext_ ? rsUIContext_->GetToken() : 0);
    RS_LOGI("RSUIDirector::GoForeground CurrentState:%{public}d, UIContext:%{public}" PRIu64,
        static_cast<int>(currentUIDirectorState_), rsUIContext_ ? rsUIContext_->GetToken() : 0);
    // Invalid states
    if (currentUIDirectorState_ == RSUIDirectorLifecycleState::DESTROYED ||
        currentUIDirectorState_ == RSUIDirectorLifecycleState::CREATE ||
        currentUIDirectorState_ == RSUIDirectorLifecycleState::STOP) {
        RS_LOGE("RSUIDirector::GoForeground, CurrentState %{public}d is invalid.",
            static_cast<int>(currentUIDirectorState_));
        return;
    }
    // Normal states
    if (currentUIDirectorState_ == RSUIDirectorLifecycleState::RESUME ||
        currentUIDirectorState_ == RSUIDirectorLifecycleState::BACKGROUND) {
        ExecuteGoForeground(isTextureExport);
        AddUIDirectorCommand<RSUIDirectorGoForeground>();
    }
    currentUIDirectorState_ = RSUIDirectorLifecycleState::FOREGROUND;
}

void RSUIDirector::ExecuteGoForeground(bool isTextureExport)
{
    if (!isActive_) {
        auto nowTime = std::chrono::system_clock::now().time_since_epoch();
        lastUiSkipTimestamp_ = std::chrono::duration_cast<std::chrono::milliseconds>(nowTime).count();
        if (!isUniRenderEnabled_ || isTextureExport) {
            RSRenderThread::Instance().UpdateWindowStatus(true);
        }
        isActive_ = true;
        auto node = rootNode_.lock();
        if (node) {
            node->RebuildTree();
            node->SetEnableRender(true);
        }
        auto surfaceNode = surfaceNode_.lock();
        if (surfaceNode) {
            surfaceNode->MarkUIHidden(false);
            surfaceNode->SetAbilityState(RSSurfaceNodeAbilityState::FOREGROUND);
        }
    }
}

void RSUIDirector::ReportUiSkipEvent(const std::string& abilityName)
{
    auto nowTime = std::chrono::system_clock::now().time_since_epoch();
    int64_t nowMs = std::chrono::duration_cast<std::chrono::milliseconds>(nowTime).count();
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->ReportUiSkipEvent(abilityName, nowMs, lastUiSkipTimestamp_);
    }
}

void RSUIDirector::GoBackground(bool isTextureExport)
{
    RS_TRACE_NAME_FMT("RSUIDirector::GoBackground CurrentState:%d, UIContext:%lu",
        static_cast<int>(currentUIDirectorState_), rsUIContext_ ? rsUIContext_->GetToken() : 0);
    RS_LOGI("RSUIDirector::GoBackground CurrentState:%{public}d, UIContext:%{public}" PRIu64,
        static_cast<int>(currentUIDirectorState_), rsUIContext_ ? rsUIContext_->GetToken() : 0);
    // Invalid states
    if (currentUIDirectorState_ != RSUIDirectorLifecycleState::FOREGROUND) {
        RS_LOGE("RSUIDirector::GoForeground, currentUIDirectorState_ is not FOREGROUND.");
        return;
    }
    ExecuteGoBackground(isTextureExport);
    AddUIDirectorCommand<RSUIDirectorGoBackground>();
    currentUIDirectorState_ = RSUIDirectorLifecycleState::BACKGROUND;
}

void RSUIDirector::ExecuteGoBackground(bool isTextureExport)
{
    if (isActive_) {
        if (!isUniRenderEnabled_ || isTextureExport) {
            RSRenderThread::Instance().UpdateWindowStatus(false);
        }
        isActive_ = false;
        ReportUiSkipEvent(abilityName_);
        auto node = rootNode_.lock();
        if (node) {
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

void RSUIDirector::GoStop()
{
    RS_TRACE_NAME_FMT("RSUIDirector::GoStop CurrentState:%d, UIContext:%lu",
        static_cast<int>(currentUIDirectorState_), rsUIContext_ ? rsUIContext_->GetToken() : 0);
    RS_LOGI("RSUIDirector::GoStop CurrentState:%{public}d, UIContext:%{public}" PRIu64,
        static_cast<int>(currentUIDirectorState_), rsUIContext_ ? rsUIContext_->GetToken() : 0);
    // Invalid states
    if (currentUIDirectorState_ == RSUIDirectorLifecycleState::DESTROYED ||
        currentUIDirectorState_ == RSUIDirectorLifecycleState::FOREGROUND) {
        RS_LOGE("RSUIDirector::GoStop, CurrentState %{public}d is invalid.",
            static_cast<int>(currentUIDirectorState_));
        return;
    }
    // Normal states
    if (currentUIDirectorState_ == RSUIDirectorLifecycleState::BACKGROUND ||
        currentUIDirectorState_ == RSUIDirectorLifecycleState::RESUME) {
        if (RSSystemProperties::IsRenderNodeRebuildEnabled() && RSSystemProperties::GetBackgroundRebuildEnabled()) {
            ExecuteGoStop();
            AddUIDirectorCommand<RSUIDirectorGoStop>();
        }
    }
    currentUIDirectorState_ = RSUIDirectorLifecycleState::STOP;
}

void RSUIDirector::ExecuteGoStop()
{
    ReleaseRenderNode();
}

void RSUIDirector::ReleaseRenderNode()
{
    if (!rsUIContext_ || skipDestroyUIContext_) {
        return;
    }
    const auto& map = rsUIContext_->GetNodeMap();
    map.TraversalNodes([](const std::shared_ptr<RSBaseNode>& baseNode) {
        if (baseNode == nullptr) {
            return;
        }
        auto surfaceNode = baseNode->ReinterpretCastTo<RSSurfaceNode>();
        if (surfaceNode && (surfaceNode->IsAppWindow() || surfaceNode->IsTextureExportNode())) {
            RS_OPTIONAL_TRACE_NAME_FMT(
                "RSUIDirector::ReleaseRenderNode skip release AppWindow or TextureExportNode id:%llu, name:%s",
                surfaceNode->GetId(), surfaceNode->GetName().c_str());
            return;
        }
        if (!baseNode->HasCreateRenderNodeInRS()) {
            return;
        }
        if (baseNode->GetNodeState() != RSNodeState::LAZY_LOAD) {
            baseNode->ReleaseInRender();
        }
    });
}

void RSUIDirector::Destroy(bool isTextureExport)
{
    RS_TRACE_NAME_FMT("RSUIDirector::Destroy CurrentState:%d, UIContext:%lu",
        static_cast<int>(currentUIDirectorState_), rsUIContext_ ? rsUIContext_->GetToken() : 0);
    RS_LOGI("RSUIDirector::Destroy CurrentState:%{public}d, UIContext:%{public}" PRIu64,
        static_cast<int>(currentUIDirectorState_), rsUIContext_ ? rsUIContext_->GetToken() : 0);
    ExecuteGoDestroy(isTextureExport);
    AddUIDirectorCommand<RSUIDirectorGoDestroy>();
    currentUIDirectorState_ = RSUIDirectorLifecycleState::DESTROYED;
}

void RSUIDirector::GoDestroy() {}

void RSUIDirector::ExecuteGoDestroy(bool isTextureExport)
{
    if (auto node = rootNode_.lock()) {
        if (!isUniRenderEnabled_ || isTextureExport) {
            node->RemoveFromTree();
        }
        rootNode_.reset();
    }
    if (rsUIContext_ != nullptr) {
        // When a child window reuses the instance of the parent window, do not remove the UIContext from the
        // UIContextManager when the child window is destroyed, as this would cause the parent window or newly created
        // child windows to be unable to find the UIContext during animation callback.
        if (!skipDestroyUIContext_) {
            RSUIContextManager::MutableInstance().DestroyContext(rsUIContext_->GetToken());
        }
        rsUIContext_ = nullptr;
    }
    {
        std::unique_lock<std::mutex> lock(uiTaskRunnersVisitorMutex_);
        uiTaskRunners_.erase(this);
    }
    {
        std::unique_lock<std::mutex> lock(g_vsyncCallbackMutex);
        requestVsyncCallbacks_.erase(this);
    }
}

void RSUIDirector::SetRSSurfaceNode(std::shared_ptr<RSSurfaceNode> surfaceNode)
{
    surfaceNode_ = surfaceNode;
    AttachSurface();
}

std::shared_ptr<RSSurfaceNode> RSUIDirector::GetRSSurfaceNode() const
{
    return surfaceNode_.lock();
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
    auto surfaceNode = surfaceNode_.lock();
    auto node = rootNode_.lock();
    if (node != nullptr && surfaceNode != nullptr) {
        node->AttachRSSurfaceNode(surfaceNode);
        ROSEN_LOGD("RSUIDirector::AttachSurface [%{public}" PRIu64, surfaceNode->GetId());
    } else {
        ROSEN_LOGD("RSUIDirector::AttachSurface not ready");
    }
}

void RSUIDirector::SetRSRootNode(std::shared_ptr<RSRootNode> rootNode)
{
    if (rootNode_.lock() == rootNode) {
        return;
    }
    if (rootNode != nullptr && rsUIContext_ != nullptr) {
        rsUIContext_->rootNodeId_ = rootNode->GetId();
    }
    rootNode_ = rootNode;
    AttachSurface();
}

size_t RSUIDirector::GetUIDescendantCount() const
{
    auto node = rootNode_.lock();
    if (node == nullptr) {
        ROSEN_LOGE("RSUIDirector::GetUIDescendantCount, rootNode is nullptr");
        return 0;
    }
    size_t count = node->GetDescendantCount();
    ROSEN_LOGD("RSUIDirector::GetUIDescendantCount, count=%{public}zu", count);
    return count;
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
    if (callback == nullptr) {
        return;
    }
    std::unique_lock<std::mutex> lock(g_vsyncCallbackMutex);
    if (rsUIContext_ && RSSystemProperties::GetUniRenderEnabled()) {
        rsUIContext_->SetRequestVsyncCallback(callback);
    } else {
        requestVsyncCallbacks_[this] = callback;
    }
}

void RSUIDirector::SetTimeStamp(uint64_t timeStamp, const std::string& abilityName)
{
    timeStamp_ = timeStamp;
    abilityName_ = abilityName;
    dvsyncUpdate_ = false;
}

void RSUIDirector::SetDVSyncUpdate(uint64_t dvsyncTime)
{
    dvsyncUpdate_ = true;
    dvsyncTime_ = dvsyncTime;
}

void RSUIDirector::SetCacheDir(const std::string& cacheFilePath)
{
    cacheDir_ = cacheFilePath;
#ifdef RS_MODIFIERS_DRAW_ENABLE
    if (rsUIContext_ == nullptr) {
        return;
    }
    if (cacheDir_.empty()) {
        return;
    }
    if (!RSSystemProperties::GetHybridRenderCanvasEnabled()) {
        return;
    }
    if (auto canvasModifiersDrawAgent = rsUIContext_->GetCanvasModifiersDrawAgent()) {
        canvasModifiersDrawAgent->SetCacheDir(cacheDir_);
    }
#endif
}

bool RSUIDirector::FlushAnimation(uint64_t timeStamp, int64_t vsyncPeriod)
{
    auto rsUIContext = rsUIContext_;
    auto modifierManager = rsUIContext ? rsUIContext->GetRSModifierManager()
                                        : RSModifierManagerMap::Instance()->GetModifierManager();
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
    auto rsUIContext = rsUIContext_;
    auto modifierManager = rsUIContext ? rsUIContext->GetRSModifierManager()
                                        : RSModifierManagerMap::Instance()->GetModifierManager();
    if (modifierManager != nullptr) {
        return modifierManager->GetAndResetFirstFrameAnimationState();
    }
    return false;
}

void RSUIDirector::FlushAnimationStartTime(uint64_t timeStamp)
{
    auto rsUIContext = rsUIContext_;
    auto modifierManager = rsUIContext ? rsUIContext->GetRSModifierManager()
                                        : RSModifierManagerMap::Instance()->GetModifierManager();
    if (modifierManager != nullptr) {
        modifierManager->FlushStartAnimation(timeStamp);
    }
}

void RSUIDirector::FlushModifier()
{
    std::shared_ptr<RSModifierManager> modifierManager = nullptr;
    if (rsUIContext_ == nullptr) {
        modifierManager = RSModifierManagerMap::Instance()->GetModifierManager();
    } else {
        modifierManager = rsUIContext_->GetRSModifierManager();
#ifdef RS_MODIFIERS_DRAW_ENABLE
        rsUIContext_->FlushCanvasDrawingNodeBuffers();
#endif
    }
    if (modifierManager == nullptr) {
        return;
    }

    modifierManager->Draw();
    // post animation finish callback(s) to task queue
    RSUIDirector::RecvMessages();
}

bool RSUIDirector::HasUIRunningAnimation()
{
    auto rsUIContext = rsUIContext_;
    auto modifierManager = rsUIContext ? rsUIContext->GetRSModifierManager()
                                        : RSModifierManagerMap::Instance()->GetModifierManager();
    if (modifierManager != nullptr) {
        return modifierManager->HasUIRunningAnimation();
    }
    return false;
}

void RSUIDirector::SetUITaskRunner(const TaskRunner& uiTaskRunner, int32_t instanceId, bool useMultiInstance)
{
    if (!useMultiInstance || !RSSystemProperties::GetUniRenderEnabled()) {
        std::unique_lock<std::mutex> lock(uiTaskRunnersVisitorMutex_);
        ROSEN_LOGI("RSUIDirector::SetUITaskRunner:%{public}d", instanceId);
        instanceId_ = instanceId;
        uiTaskRunners_[this] = uiTaskRunner;
        if (!isHgmConfigChangeCallbackReg_) {
            RSFrameRatePolicy::GetInstance()->RegisterHgmConfigChangeCallback();
            isHgmConfigChangeCallbackReg_ = true;
        }
        return;
    }

    if (rsUIContext_ == nullptr) {
        ROSEN_LOGE("multi-instance, RSUIDirector::SetUITaskRunner, rsUIContext_ is null!");
        return;
    }
    rsUIContext_->SetUITaskRunner(uiTaskRunner);
    if (!isHgmConfigChangeCallbackReg_) {
        RSFrameRatePolicy::GetInstance()->RegisterHgmConfigChangeCallback();
        isHgmConfigChangeCallbackReg_ = true;
    }
}

void RSUIDirector::SendMessages()
{
    if (rsUIContext_) {
        RS_TRACE_NAME_FMT("SendCommands, rsUIContext_:%lu", rsUIContext_->GetToken());
        auto transaction = rsUIContext_->GetRSTransaction();
        if (transaction != nullptr) {
            transaction->FlushImplicitTransaction(timeStamp_, abilityName_, dvsyncUpdate_, dvsyncTime_);
            index_ = transaction->GetTransactionDataIndex();
        } else {
            RS_LOGE_LIMIT(__func__, __line__, "RSUIDirector::SendMessages failed, transaction is nullptr");
        }
    } else {
        ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "SendCommands");
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            transactionProxy->FlushImplicitTransaction(timeStamp_, abilityName_, dvsyncUpdate_, dvsyncTime_);
            index_ = transactionProxy->GetTransactionDataIndex();
        } else {
            RS_LOGE_LIMIT(__func__, __line__, "RSUIDirector::SendMessages failed, transactionProxy is nullptr");
        }
        ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
    }
}

void RSUIDirector::SendMessages(std::function<void()> callback)
{
    if (rsUIContext_) {
        ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "multi-intance SendCommands With Callback");
        RS_TRACE_NAME_FMT("multi-instance SendCommands, rsUIContext_:%lu", rsUIContext_->GetToken());
        auto transaction = rsUIContext_->GetRSTransaction();
        if (transaction != nullptr && !transaction->IsEmpty()) {
            if (callback != nullptr) {
                RS_LOGD("RSUIDirector:: multi-intance SendMessages with callback, timeStamp: %{public}"
                    PRIu64 " token: %{public}" PRIu64, timeStamp_, rsUIContext_->GetToken());
                if (auto rsRenderInterface = rsUIContext_->GetRSRenderInterface()) {
                    rsRenderInterface->RegisterTransactionDataCallback(rsUIContext_->GetToken(), timeStamp_, callback);
                }
            }
            transaction->FlushImplicitTransaction(timeStamp_, abilityName_, dvsyncUpdate_, dvsyncTime_);
            index_ = transaction->GetTransactionDataIndex();
        } else {
            RS_LOGE_LIMIT(__func__, __line__, "RSUIDirector:: multi-intance SendMessages failed, \
                transaction is nullptr");
        }
        ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
    }
}

uint32_t RSUIDirector::GetIndex() const
{
    return index_;
}

std::shared_ptr<RSUIContext> RSUIDirector::GetRSUIContext() const
{
    return rsUIContext_;
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
        ROSEN_LOGD("RSUIDirector::RecvMessages cmd empty");
        return;
    }
    ROSEN_LOGD("ProcessMessages begin");
    RSUIDirector::ProcessMessages(cmds);
}

void RSUIDirector::ProcessInstanceMessages(
    std::map<int32_t, std::vector<std::unique_ptr<RSCommand>>>& cmdMap, uint32_t messageId)
{
    for (auto& [instanceId, commands] : cmdMap) {
        ROSEN_LOGD("Post messageId:%{public}u, cmdCount:%{public}lu, instanceId:%{public}d", messageId,
            static_cast<unsigned long>(commands.size()), instanceId);
        PostTask(
            [cmds = std::make_shared<std::vector<std::unique_ptr<RSCommand>>>(std::move(commands)), messageId,
                tempInstanceId = instanceId] {
                RS_TRACE_NAME_FMT("RSUIDirector::ProcessInstanceMessages Process messageId:%lu", messageId);
                ROSEN_LOGD("Process messageId:%{public}u, cmdCount:%{public}lu, instanceId:%{public}d", messageId,
                    static_cast<unsigned long>(cmds->size()), tempInstanceId);
                for (auto& cmd : *cmds) {
                    RSContext context; // RSCommand->process() needs it
                    cmd->Process(context);
                }
                RequestVsyncCallback(tempInstanceId); // request vsync callback for each instance
                ROSEN_LOGD("ProcessInstanceMessages end");
            },
            instanceId);
    }
}

void RSUIDirector::ProcessUIContextMessages(
    std::map<uint64_t, std::vector<std::unique_ptr<RSCommand>>>& cmdMap, uint32_t messageId)
{
    auto counter = std::make_shared<std::atomic_size_t>(cmdMap.size());
    for (auto& [token, commands] : cmdMap) {
        ROSEN_LOGD("Post messageId:%{public}u, cmdCount:%{public}lu, token:%{public}" PRIu64, messageId,
            static_cast<unsigned long>(commands.size()), token);
        auto rsUICtx = RSUIContextManager::Instance().GetRSUIContext(token);
        if (rsUICtx == nullptr) {
            ROSEN_LOGE(
                "RSUIDirector::ProcessUIContextMessages, can not get rsUIContext with token:%{public}" PRIu64, token);
            rsUICtx = RSUIContextManager::Instance().GetRandomUITaskRunnerCtx();
            if (rsUICtx == nullptr) {
                RS_LOGE("RSUIDirector::ProcessUIContextMessages, not taskrunner exist");
                RS_TRACE_NAME("RSUIDirector::ProcessUIContextMessages, not taskrunner exist");
                continue;
            }
        }
        rsUICtx->PostTask([cmds = std::make_shared<std::vector<std::unique_ptr<RSCommand>>>(std::move(commands)),
                              counter, messageId, tempToken = token, rsUICtx] {
            RS_TRACE_NAME_FMT("RSUIDirector::ProcessUIContextMessages Process messageId:%lu", messageId);
            ROSEN_LOGD("Process messageId:%{public}u, cmdCount:%{public}lu, token:%{public}" PRIu64, messageId,
                static_cast<unsigned long>(cmds->size()), tempToken);
            for (auto& cmd : *cmds) {
                RSContext context; // RSCommand->process() needs it
                cmd->Process(context);
            }
            rsUICtx->RequestVsyncCallback();
        });
    }
}

void RSUIDirector::ProcessMessages(std::shared_ptr<RSTransactionData> cmds)
{
    // message ID for correspondence UI thread and IPC thread
    static uint32_t messageId = 0;
    std::map<int32_t, std::vector<std::unique_ptr<RSCommand>>> instanceCmdMap;
    std::map<uint64_t, std::vector<std::unique_ptr<RSCommand>>> uiContextCmdMap;
    if (!RSSystemProperties::GetUniRenderEnabled()) {
        for (auto& [id, _, cmd] : cmds->GetPayload()) {
            NodeId realId = (id == 0 && cmd) ? cmd->GetNodeId() : id;
            int32_t instanceId = RSNodeMap::Instance().GetNodeInstanceId(realId);
            if (instanceId == INSTANCE_ID_UNDEFINED) {
                instanceId = RSNodeMap::Instance().GetInstanceIdForReleasedNode(realId);
                instanceId = (instanceId == INSTANCE_ID_UNDEFINED) ? INSTANCE_ID_UNDEFINED_TASK_RUNNER : instanceId;
            }
            instanceCmdMap[instanceId].push_back(std::move(cmd));
        }
    } else {
        for (auto& [id, _, cmd] : cmds->GetPayload()) {
            uint64_t token = cmd->GetToken();
            if (token == 0) {
                NodeId realId = (id == 0 && cmd) ? cmd->GetNodeId() : id;
                int32_t instanceId = RSNodeMap::Instance().GetNodeInstanceId(realId);
                if (instanceId == INSTANCE_ID_UNDEFINED) {
                    instanceId = RSNodeMap::Instance().GetInstanceIdForReleasedNode(realId);
                }
                instanceCmdMap[instanceId].push_back(std::move(cmd));
            } else {
                uiContextCmdMap[token].push_back(std::move(cmd));
            }
        }
    }
    auto msgId = ++messageId;
    RS_TRACE_NAME_FMT("RSUIDirector::ProcessMessages Post [messageId:%lu,cmdIndex:%llu,cmdCount:%lu]", msgId,
        cmds->GetIndex(), cmds->GetCommandCount());
    ProcessInstanceMessages(instanceCmdMap, msgId);
    ProcessUIContextMessages(uiContextCmdMap, msgId);
}

void RSUIDirector::AnimationCallbackProcessor(NodeId nodeId, AnimationId animId, uint64_t token,
    AnimationCallbackEvent event)
{
    RSAnimationTraceUtils::GetInstance().AddAnimationFinishTrace(
        "Animation FinishCallback Processor", nodeId, animId, false);
    auto rsUIContext = RSUIContextManager::Instance().GetRSUIContext(token);
    // try find the node by nodeId
    if (auto nodePtr = rsUIContext ? rsUIContext->GetNodeMap().GetNode<RSNode>(nodeId)
                                   : RSNodeMap::Instance().GetNode<RSNode>(nodeId)) {
        if (!nodePtr->AnimationCallback(animId, event)) {
            ROSEN_LOGE("RSUIDirector::AnimationCallbackProcessor, could not find animation %{public}" PRIu64 ""
                "on node %{public}" PRIu64, animId, nodeId);
        }
        return;
    }

    // if node not found, try rsUIContext
    if (rsUIContext && rsUIContext->AnimationCallback(animId, event)) {
        ROSEN_LOGD("multi-instance, RSUIDirector::AnimationCallbackProcessor, found animation %{public}" PRIu64
                   " on fallback node.", animId);
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

void RSUIDirector::AnimationDestroyInRenderCallbackProcessor(
    NodeId nodeId, AnimationId animId, uint64_t token, float fraction, bool isReverseCycle)
{
    auto rsUIContext = RSUIContextManager::Instance().GetRSUIContext(token);
    if (rsUIContext == nullptr) {
        ROSEN_LOGE("RSUIDirector::AnimationDestroyInRenderCallbackProcessor, rsUIContext is nullptr, "
                   "token[%{public}" PRIu64 "] nodeId[%{public}" PRIu64 "] animId[%{public}" PRIu64 "]",
            token, nodeId, animId);
        return;
    }
    auto nodePtr = rsUIContext->GetNodeMap().GetNode<RSNode>(nodeId);
    if (nodePtr == nullptr) {
        ROSEN_LOGE("RSUIDirector::AnimationDestroyInRenderCallbackProcessor, could not find node %{public}" PRIu64,
            nodeId);
        return;
    }
    nodePtr->AnimationDestroyInRenderCallback(animId, fraction, isReverseCycle);
}

void RSUIDirector::ColorPickerCallbackProcessor(NodeId nodeId, uint64_t token, uint32_t color)
{
    auto rsUICtx = RSUIContextManager::Instance().GetRSUIContext(token);
    // Try to find the node in the node map
    if (auto nodePtr =
            rsUICtx ? rsUICtx->GetNodeMap().GetNode<RSNode>(nodeId) : RSNodeMap::Instance().GetNode<RSNode>(nodeId)) {
        nodePtr->FireColorPickerCallback(color);
        return;
    }
    ROSEN_LOGE("RSUIDirector::ColorPickerCallbackProcessor, could not find node %{public}" PRIu64, nodeId);
}

void RSUIDirector::DumpNodeTreeProcessor(NodeId nodeId, pid_t pid, uint64_t token, uint32_t taskId)
{
    RS_TRACE_NAME_FMT("DumpClientNodeTree dump task[%u] node[%" PRIu64 "]", taskId, nodeId);
    ROSEN_LOGI("DumpNodeTreeProcessor task[%{public}u] node[%" PRIu64 "]", taskId, nodeId);

    std::string out;
    // use for dump transactionFlags [pid,index] in client tree dump
    if (auto rsUICtx = RSUIContextManager::Instance().GetRSUIContext(token)) {
        rsUICtx->DumpNodeTreeProcessor(nodeId, pid, taskId, out);
        return;
    }
    int32_t instanceId = RSNodeMap::Instance().GetNodeInstanceId(nodeId);
    {
        std::unique_lock<std::mutex> lock(uiTaskRunnersVisitorMutex_);
        for (const auto &[director, taskRunner] : uiTaskRunners_) {
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

    auto transactionProxy = RSTransactionProxy::GetInstance(); // planing
    if (transactionProxy != nullptr) {
        std::unique_ptr<RSCommand> command = std::make_unique<RSCommitDumpClientNodeTree>(
            nodeId, getpid(), taskId, out);
        transactionProxy->AddCommand(command, true);
        RSTransaction::FlushImplicitTransaction();
    }
}

void RSUIDirector::PostFrameRateTask(const std::function<void()>& task, bool useMultiInstance)
{
    if (!useMultiInstance || !RSSystemProperties::GetUniRenderEnabled()) {
        PostTask(task);
        return;
    }
    auto ctx = RSUIContextManager::Instance().GetRandomUITaskRunnerCtx();
    if (ctx == nullptr) {
        ROSEN_LOGD("multi-instance RSUIDirector::PostFrameRateTask, no taskRunner exists");
        return;
    }
    ctx->PostTask(task);
}

void RSUIDirector::PostTask(const std::function<void()>& task, int32_t instanceId)
{
    PostDelayTask(task, 0, instanceId);
}

void RSUIDirector::PostDelayTask(const std::function<void()>& task, uint32_t delay, int32_t instanceId)
{
    std::unique_lock<std::mutex> lock(uiTaskRunnersVisitorMutex_);
    for (const auto &[director, taskRunner] : uiTaskRunners_) {
        if (director->instanceId_ != instanceId) {
            continue;
        }
        ROSEN_LOGD("RSUIDirector::PostTask instanceId=%{public}d success", instanceId);
        taskRunner(task, delay);
        return;
    }
    if (instanceId != INSTANCE_ID_UNDEFINED) {
        ROSEN_LOGW("RSUIDirector::PostTask instanceId=%{public}d not found, taskRunnerSize=%{public}zu", instanceId,
            uiTaskRunners_.size());
    }
    for (const auto &[_, taskRunner] : uiTaskRunners_) {
        ROSEN_LOGD("RSUIDirector::PostTask success");
        taskRunner(task, delay);
        return;
    }
}

bool RSUIDirector::RequestVsyncCallback(int32_t instanceId)
{
    std::unique_lock<std::mutex> lock(g_vsyncCallbackMutex);
    if (requestVsyncCallbacks_.empty()) {
        return false;
    }
    auto iter = std::find_if(requestVsyncCallbacks_.begin(), requestVsyncCallbacks_.end(),
        [instanceId](const auto &callbacks) -> bool { return callbacks.first->instanceId_ == instanceId; });
    if (iter != requestVsyncCallbacks_.end()) {
        ROSEN_LOGD("RSUIDirector::RequestVsyncCallback for target instance[%{public}d]", iter->first->instanceId_);
        iter->second();
        return true;
    }
    if (instanceId != INSTANCE_ID_UNDEFINED) {
        ROSEN_LOGW(
            "RSUIDirector::RequestVsyncCallback instanceId=%{public}d not found, requestVsyncCallback size=%{public}zu",
            instanceId, requestVsyncCallbacks_.size());
    }
    for (const auto &[director, requestVsyncCallback] : requestVsyncCallbacks_) {
        ROSEN_LOGD("RSUIDirector::RequestVsyncCallback for each instance[%{public}d]", director->instanceId_);
        requestVsyncCallback();
    }
    return true;
}

int32_t RSUIDirector::GetCurrentRefreshRateMode()
{
    return RSFrameRatePolicy::GetInstance()->GetRefreshRateModeName();
}

int32_t RSUIDirector::GetAnimateExpectedRate() const
{
    int32_t animateRate = 0;
    auto modifierManager = rsUIContext_ ? rsUIContext_->GetRSModifierManager()
                                        : RSModifierManagerMap::Instance()->GetModifierManager();
    if (modifierManager != nullptr) {
        auto& range = modifierManager->GetFrameRateRange();
        if (range.IsValid()) {
            animateRate = range.preferred_;
        }
    }
    return animateRate;
}

void RSUIDirector::SetContainerWindowTransparent(bool isContainerWindowTransparent)
{
    if (auto node = surfaceNode_.lock()) {
        node->SetContainerWindowTransparent(isContainerWindowTransparent);
    }
}
} // namespace Rosen
} // namespace OHOS
