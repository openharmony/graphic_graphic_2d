/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "pipeline/rs_render_thread.h"

#include <cstdint>

#include "rs_trace.h"
#include "sandbox_utils.h"

#include "animation/rs_animation_fraction.h"
#include "command/rs_surface_node_command.h"
#include "delegate/rs_functional_delegate.h"
#include "pipeline/rs_draw_cmd_list.h"
#include "pipeline/rs_frame_report.h"
#include "pipeline/rs_node_map.h"
#include "pipeline/rs_render_node_map.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_accessibility.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "property/rs_property_trace.h"
#include "transaction/rs_render_service_client.h"
#include "ui/rs_surface_extractor.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_ui_director.h"

#ifdef OHOS_RSS_CLIENT
#include "res_sched_client.h"
#include "res_type.h"
#endif
#ifdef ROSEN_OHOS
#include <sys/prctl.h>
#include <unistd.h>
#include "accessibility_config.h"
#include "frame_collector.h"
#include "render_frame_trace.h"
#include "platform/ohos/overdraw/rs_overdraw_controller.h"

static const std::string RT_INTERVAL_NAME = "renderthread";
#endif

static void SystemCallSetThreadName(const std::string& name)
{
#ifdef ROSEN_OHOS
    if (prctl(PR_SET_NAME, name.c_str()) < 0) {
        return;
    }
#endif
}

namespace OHOS {
namespace Rosen {
namespace {
    static constexpr uint64_t REFRESH_PERIOD = 16666667;
    static constexpr uint64_t REFRESH_FREQ_IN_UNI_RENDER = 1200;
}

void SendFrameEvent(bool start)
{
#ifdef ROSEN_OHOS
    FrameCollector::GetInstance().MarkFrameEvent(start ? FrameEventType::WaitVsyncStart : FrameEventType::WaitVsyncEnd);
#endif
}

RSRenderThread& RSRenderThread::Instance()
{
    static RSRenderThread renderThread;
    RSAnimationFraction::Init();
    return renderThread;
}

RSRenderThread::RSRenderThread()
{
    mainFunc_ = [&]() {
        uint64_t renderStartTimeStamp;
        if (needRender_) {
            renderStartTimeStamp = jankDetector_->GetSysTimeNs();
        }
        RS_TRACE_BEGIN("RSRenderThread DrawFrame: " + std::to_string(timestamp_));
#ifdef ROSEN_OHOS
        FRAME_TRACE::RenderFrameTrace::GetInstance().RenderStartFrameTrace(RT_INTERVAL_NAME);
#endif
        prevTimestamp_ = timestamp_;
        ProcessCommands();
        ROSEN_LOGD("RSRenderThread DrawFrame(%" PRIu64 ") in %s", prevTimestamp_, renderContext_ ? "GPU" : "CPU");
        Animate(prevTimestamp_);
        Render();
        SendCommands();
#ifdef ROSEN_OHOS
        FRAME_TRACE::RenderFrameTrace::GetInstance().RenderEndFrameTrace(RT_INTERVAL_NAME);
#endif
        if (needRender_) {
            jankDetector_->CalculateSkippedFrame(renderStartTimeStamp, jankDetector_->GetSysTimeNs());
        }
        RS_TRACE_END();
    };

    context_ = std::make_shared<RSContext>();
    jankDetector_ = std::make_shared<RSJankDetector>();
    RSAccessibility::GetInstance().ListenHighContrastChange([](bool newHighContrast) {
        auto& renderThread = RSRenderThread::Instance();
        renderThread.SetHighContrast(newHighContrast);
    });
}

RSRenderThread::~RSRenderThread()
{
    Stop();

    if (renderContext_ != nullptr) {
        ROSEN_LOGD("Destroy renderContext!!");
#if !defined(_WIN32) && !defined(__APPLE__) && !defined(__gnu_linux__)
        delete renderContext_;
        renderContext_ = nullptr;
#endif
    }
}

void RSRenderThread::Start()
{
    ROSEN_LOGD("RSRenderThread start.");
    running_.store(true);
    if (thread_ == nullptr) {
        thread_ = std::make_unique<std::thread>(&RSRenderThread::RenderLoop, this);
    }
}

void RSRenderThread::Stop()
{
    running_.store(false);

    if (handler_) {
        handler_->RemoveAllEvents();
        handler_ = nullptr;
    }
    receiver_ = nullptr;
    if (runner_) {
        runner_->Stop();
    }

    if (thread_ != nullptr && thread_->joinable()) {
        thread_->join();
    }

    thread_ = nullptr;
    ROSEN_LOGD("RSRenderThread stopped.");
}

void RSRenderThread::RecvTransactionData(std::unique_ptr<RSTransactionData>& transactionData)
{
    {
        std::unique_lock<std::mutex> cmdLock(cmdMutex_);
        std::string str = "RecvCommands ptr:" + std::to_string(reinterpret_cast<uintptr_t>(transactionData.get()));
        commandTimestamp_ = transactionData->GetTimestamp();
        ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, str.c_str());
        cmds_.emplace_back(std::move(transactionData));
        ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
    }
    // [PLANNING]: process in next vsync (temporarily)
    RSRenderThread::Instance().RequestNextVSync();
}

void RSRenderThread::RequestNextVSync()
{
    if (handler_) {
        RS_TRACE_FUNC();
        SendFrameEvent(true);
        VSyncReceiver::FrameCallback fcb = {
            .userData_ = this,
            .callback_ = std::bind(&RSRenderThread::OnVsync, this, std::placeholders::_1),
        };
        if (receiver_ != nullptr) {
            receiver_->RequestNextVSync(fcb);
        } else {
            hasSkipVsync_ = true;
        }
    } else {
        hasSkipVsync_ = true;
    }
}

int32_t RSRenderThread::GetTid()
{
    return tid_;
}

void RSRenderThread::CreateAndInitRenderContextIfNeed()
{
#ifdef ACE_ENABLE_GL
    if (needRender_ && renderContext_ == nullptr) {
#if !defined(_WIN32) && !defined(__APPLE__) && !defined(__gnu_linux__)
        renderContext_ = new RenderContext();
#endif
        ROSEN_LOGD("Create RenderContext, its pointer is %p", renderContext_);
        RS_TRACE_NAME("InitializeEglContext");
#if !defined(_WIN32) && !defined(__APPLE__) && !defined(__gnu_linux__)
        renderContext_->InitializeEglContext(); // init egl context on RT
        if (!cacheDir_.empty()) {
            renderContext_->SetCacheDir(cacheDir_);
        }
#endif
    }
#endif
}

void RSRenderThread::RenderLoop()
{
    SystemCallSetThreadName("RSRenderThread");

#ifdef OHOS_RSS_CLIENT
    std::unordered_map<std::string, std::string> payload;
    payload["uid"] = std::to_string(getuid());
    payload["pid"] = std::to_string(GetRealPid());
    ResourceSchedule::ResSchedClient::GetInstance().ReportData(
        ResourceSchedule::ResType::RES_TYPE_REPORT_RENDER_THREAD, gettid(), payload);
#endif
#ifdef ROSEN_OHOS
    tid_ = gettid();
#endif
    if (RSSystemProperties::GetUniRenderEnabled()) {
        needRender_ = std::static_pointer_cast<RSRenderServiceClient>(RSIRenderClient::CreateRenderServiceClient())
            ->QueryIfRTNeedRender();
        RSSystemProperties::SetRenderMode(!needRender_);
        DrawCmdListManager::Instance().MarkForceClear(!needRender_);
    }
    CreateAndInitRenderContextIfNeed();
    std::string name = "RSRenderThread_" + std::to_string(GetRealPid());
    runner_ = AppExecFwk::EventRunner::Create(false);
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    auto rsClient = std::static_pointer_cast<RSRenderServiceClient>(RSIRenderClient::CreateRenderServiceClient());
    receiver_ = rsClient->CreateVSyncReceiver(name, handler_);
    if (receiver_ == nullptr) {
        ROSEN_LOGE("RSRenderThread CreateVSyncReceiver Error");
        return;
    }
    receiver_->Init();
    if (hasSkipVsync_) {
        hasSkipVsync_ = false;
        RSRenderThread::Instance().RequestNextVSync();
    }

#ifdef ROSEN_OHOS
    FrameCollector::GetInstance().SetRepaintCallback([this]() { this->RequestNextVSync(); });

    auto delegate = RSFunctionalDelegate::Create();
    delegate->SetRepaintCallback([this]() { this->RequestNextVSync(); });
    RSOverdrawController::GetInstance().SetDelegate(delegate);
#endif

    if (runner_) {
        runner_->Run();
    }
}

void RSRenderThread::OnVsync(uint64_t timestamp)
{
    ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "RSRenderThread::OnVsync");
    SendFrameEvent(false);
    mValue = (mValue + 1) % 2; // 1 and 2 is Calculated parameters
    RS_TRACE_INT("Vsync-client", mValue);
    timestamp_ = timestamp;
    if (activeWindowCnt_.load() > 0 &&
        (needRender_ || (timestamp_ - prevTimestamp_) / REFRESH_PERIOD >= REFRESH_FREQ_IN_UNI_RENDER)) {
        mainFunc_(); // start render-loop now
    }
    ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
}

void RSRenderThread::UpdateWindowStatus(bool active)
{
    if (active) {
        activeWindowCnt_++;
    } else {
        activeWindowCnt_--;
    }
    ROSEN_LOGD("RSRenderThread UpdateWindowStatus %d, cur activeWindowCnt_ %d", active, activeWindowCnt_.load());
}

void RSRenderThread::UpdateRenderMode(bool needRender)
{
    if (handler_) {
        handler_->PostTask([needRender = needRender, this]() {
            RequestNextVSync();
            if (!needRender) { // change to uni render, should move surfaceView's position
                UpdateSurfaceNodeParentInRS();
            } else {
                needRender_ = needRender;
                MarkNeedUpdateSurfaceNode();
                CreateAndInitRenderContextIfNeed();
                DrawCmdListManager::Instance().MarkForceClear(!needRender_);
            }
        }, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

void RSRenderThread::NotifyClearBufferCache()
{
    if (handler_) {
        handler_->PostTask([this]() {
            needRender_ = false;
            ClearBufferCache();
            DrawCmdListManager::Instance().MarkForceClear(!needRender_);
        }, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

void RSRenderThread::UpdateSurfaceNodeParentInRS()
{
    auto& nodeMap = context_->GetMutableNodeMap();
    std::unordered_map<NodeId, NodeId> surfaceNodeMap; // [surfaceNodeId, parentId]
    nodeMap.TraverseSurfaceNodes([&surfaceNodeMap](const std::shared_ptr<RSSurfaceRenderNode>& node) mutable {
        if (!node) {
            return;
        }
        auto parent = node->GetParent().lock();
        if (!parent) {
            return;
        }
        surfaceNodeMap.emplace(node->GetId(), parent->GetId());
    });
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        for (auto& [surfaceNodeId, parentId] : surfaceNodeMap) {
            std::unique_ptr<RSCommand> command =
                std::make_unique<RSSurfaceNodeUpdateParentWithoutTransition>(surfaceNodeId, parentId);
            transactionProxy->AddCommandFromRT(command, surfaceNodeId, FollowType::FOLLOW_TO_SELF);
        }
        transactionProxy->FlushImplicitTransactionFromRT(uiTimestamp_);
    }
}

void RSRenderThread::ClearBufferCache()
{
    const auto& rootNode = context_->GetGlobalRootRenderNode();
    if (rootNode == nullptr) {
        ROSEN_LOGE("RSRenderThread::ClearBufferCache, rootNode is nullptr");
        return;
    }
    for (auto& child : rootNode->GetSortedChildren()) {
        if (!child || !child->IsInstanceOf<RSRootRenderNode>()) {
            continue;
        }
        auto childNode = child->ReinterpretCastTo<RSRootRenderNode>();
        auto surfaceNode = RSNodeMap::Instance().GetNode<RSSurfaceNode>(childNode->GetRSSurfaceNodeId());
        auto rsSurface = RSSurfaceExtractor::ExtractRSSurface(surfaceNode);
        if (rsSurface != nullptr) {
            rsSurface->ClearAllBuffer();
        }
    }
    rootNode->ResetSortedChildren();
}


void RSRenderThread::MarkNeedUpdateSurfaceNode()
{
    const auto& rootNode = context_->GetGlobalRootRenderNode();
    if (rootNode == nullptr) {
        ROSEN_LOGE("RSRenderThread::MarkNeedUpdateSurfaceNode, rootNode is nullptr");
        return;
    }
    for (auto& child : rootNode->GetSortedChildren()) {
        if (!child || !child->IsInstanceOf<RSRootRenderNode>()) {
            continue;
        }
        auto childNode = child->ReinterpretCastTo<RSRootRenderNode>();
        childNode->SetNeedUpdateSurfaceNode(true);
    }
    rootNode->ResetSortedChildren();
}

void RSRenderThread::ProcessCommands()
{
    // Attention: there are two situations
    // 1. when commandTimestamp_ != 0, it means that UIDirector has called
    // "RSRenderThread::Instance().RequestNextVSync()", which equals there are some commands form UIThread need to be
    // executed. To make commands from UIThread sync with buffer flushed by RenderThread, we choose commandTimestamp_ as
    // uiTimestamp_ which would be used in RenderThreadVisitor when we call flushFrame.
    // 2. when cmds_.empty() is true or commandTimestamp_ = 0,
    // it means that some thread except UIThread like RSRenderThread::Animate
    // has called "RSRenderThread::Instance().RequestNextVSync()", which equals that some commands form RenderThread
    // need to be executed. To make commands from RenderThread sync with buffer flushed by RenderThread, we choose
    // (prevTimestamp_ - 1) as uiTimestamp_ which would be used in RenderThreadVisitor when we call flushFrame.

    // The reason why prevTimestamp_ need to be minus 1 is that timestamp used in UIThread is always less than (for now)
    // timestamp used in RenderThread. If we do not do this, when RenderThread::Animate execute flushFrame and use
    // prevTimestamp_ as buffer timestamp which equals T0, UIDirector send messages in the same vsync period, and the
    // commandTimestamp_ would also be T0, RenderService would execute commands from UIDirector and composite buffer
    // which rendering is executed by RSRenderThread::Animate for they have the same timestamp. To avoid this situation,
    // we should always use "prevTimestamp_ - 1".

    std::unique_lock<std::mutex> cmdLock(cmdMutex_);
    if (cmds_.empty()) {
        uiTimestamp_ = prevTimestamp_ - 1;
        return;
    }
    if (RsFrameReport::GetInstance().GetEnable() && needRender_) {
        RsFrameReport::GetInstance().ProcessCommandsStart();
    }

    if (commandTimestamp_ != 0) {
        uiTimestamp_ = commandTimestamp_;
        commandTimestamp_ = 0;
    } else {
        uiTimestamp_ = prevTimestamp_ - 1;
    }

    ROSEN_LOGD("RSRenderThread ProcessCommands size: %lu\n", cmds_.size());
    std::vector<std::unique_ptr<RSTransactionData>> cmds;
    std::swap(cmds, cmds_);
    cmdLock.unlock();

    // To improve overall responsiveness, we make animations start on LAST frame instead of THIS frame.
    // If last frame is too far away (earlier than 2 vsync from now), we use currentTimestamp_ - REFRESH_PERIOD as
    // 'virtual' last frame timestamp.
    if (timestamp_ - lastAnimateTimestamp_ > 2 * REFRESH_PERIOD) { // 2: if last frame is earlier than 2 vsync from now
        context_->currentTimestamp_ = timestamp_ - REFRESH_PERIOD;
    } else {
        context_->currentTimestamp_ = lastAnimateTimestamp_;
    }
    uint64_t uiEndTimeStamp = 0;
    if (needRender_) {
        uiEndTimeStamp = jankDetector_->GetSysTimeNs();
    }
    for (auto& cmdData : cmds) {
        std::string str = "ProcessCommands ptr:" + std::to_string(reinterpret_cast<uintptr_t>(cmdData.get()));
        ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, str.c_str());
        // only set transactionTimestamp_ in UniRender mode
        context_->transactionTimestamp_ = RSSystemProperties::GetUniRenderEnabled() ? cmdData->GetTimestamp() : 0;
        cmdData->Process(*context_);
        if (needRender_) {
            jankDetector_->UpdateUiDrawFrameMsg(cmdData->GetTimestamp(), uiEndTimeStamp, cmdData->GetAbilityName());
        }
        ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
    }
}

void RSRenderThread::Animate(uint64_t timestamp)
{
    RS_TRACE_FUNC();

    if (RsFrameReport::GetInstance().GetEnable() && needRender_) {
        RsFrameReport::GetInstance().AnimateStart();
    }

    lastAnimateTimestamp_ = timestamp;

    if (context_->animatingNodeList_.empty()) {
        return;
    }

    bool needRequestNextVsync = false;
    // iterate and animate all animating nodes, remove if animation finished
    EraseIf(context_->animatingNodeList_,
        [timestamp, &needRequestNextVsync](const auto& iter) -> bool {
        auto node = iter.second.lock();
        if (node == nullptr) {
            ROSEN_LOGD("RSRenderThread::Animate removing expired animating node");
            return true;
        }
        auto result = node->Animate(timestamp);
        if (!result.first) {
            ROSEN_LOGD("RSRenderThread::Animate removing finished animating node %" PRIu64, node->GetId());
        }
        needRequestNextVsync = needRequestNextVsync || result.second;
        return !result.first;
    });

    if (needRequestNextVsync) {
        RSRenderThread::Instance().RequestNextVSync();
    }
}

void RSRenderThread::Render()
{
    if (!needRender_) {
        return;
    }
    if (RSSystemProperties::GetRenderNodeTraceEnabled()) {
        RSPropertyTrace::GetInstance().RefreshNodeTraceInfo();
    }
    ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "RSRenderThread::Render");
    if (RsFrameReport::GetInstance().GetEnable()) {
        RsFrameReport::GetInstance().RenderStart();
    }
    std::unique_lock<std::mutex> lock(mutex_);
    const auto& rootNode = context_->GetGlobalRootRenderNode();

    if (rootNode == nullptr) {
        ROSEN_LOGE("RSRenderThread::Render, rootNode is nullptr");
        return;
    }
    if (visitor_ == nullptr) {
        visitor_ = std::make_shared<RSRenderThreadVisitor>();
    }
    // get latest partial render status from system properties and set it to RTvisitor_
    visitor_->SetPartialRenderStatus(RSSystemProperties::GetPartialRenderEnabled(), isRTRenderForced_);
    rootNode->Prepare(visitor_);
    rootNode->Process(visitor_);
    ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
}

void RSRenderThread::SendCommands()
{
    ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "RSRenderThread::SendCommands");
    if (RsFrameReport::GetInstance().GetEnable() && needRender_) {
        RsFrameReport::GetInstance().SendCommandsStart();
    }

    RSUIDirector::RecvMessages(needRender_);
    ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
}

void RSRenderThread::Detach(NodeId id)
{
    if (auto node = context_->GetNodeMap().GetRenderNode<RSRootRenderNode>(id)) {
        std::unique_lock<std::mutex> lock(mutex_);
        context_->GetGlobalRootRenderNode()->RemoveChild(node);
    }
}

void RSRenderThread::PostTask(RSTaskMessage::RSTask task)
{
    if (handler_) {
        handler_->PostTask(task);
    }
}

void RSRenderThread::PostPreTask()
{
    if (handler_ && preTask_) {
        handler_->PostTask(preTask_);
    }
}
} // namespace Rosen
} // namespace OHOS
