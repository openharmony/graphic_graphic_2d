/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "pipeline/rs_render_node_map.h"
#include "pipeline/rs_root_render_node.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"
#include "ui/rs_ui_director.h"
#ifdef ROSEN_OHOS
#include <sys/prctl.h>
#include <unistd.h>

#include "platform/ohos/rs_render_service_connect_hub.h"
#endif
#ifdef USE_FLUTTER_TEXTURE
#include "pipeline/rs_texture_render_node.h"
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
RSRenderThread& RSRenderThread::Instance()
{
    static RSRenderThread renderThread;
    return renderThread;
}

RSRenderThread::RSRenderThread()
{
#ifdef ACE_ENABLE_GL
    // renderContext_ = *(RenderContextFactory::GetInstance().CreateEngine());
    renderContext_ = new RenderContext();
    ROSEN_LOGD("Create RenderContext, its pointer is %p", renderContext_);
#endif
    mainFunc_ = [&]() {
        ROSEN_TRACE_BEGIN(BYTRACE_TAG_GRAPHIC_AGP, "RSRenderThread::DrawFrame");
        {
            prevTimestamp_ = timestamp_;
            ProcessCommands();
        }

        ROSEN_LOGD("RSRenderThread DrawFrame(%llu) in %s", prevTimestamp_, renderContext_ ? "GPU" : "CPU");
        Animate(prevTimestamp_);
        Render();
        RS_ASYNC_TRACE_BEGIN("waiting GPU running", 1111); // 1111 means async trace code for gpu
        SendCommands();
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            transactionProxy->FlushImplicitTransactionFromRT();
        }
        ROSEN_TRACE_END(BYTRACE_TAG_GRAPHIC_AGP);
    };

#ifdef ROSEN_OHOS
    RSRenderServiceConnectHub::SetOnConnectCallback([this](sptr<RSIRenderServiceConnection>& conn) {
        sptr<IApplicationRenderThread> renderThreadSptr = sptr<RSRenderThread>(this);
        conn->RegisterApplicationRenderThread(getpid(), renderThreadSptr);
    });
#endif
}

RSRenderThread::~RSRenderThread()
{
    Stop();

    if (renderContext_ != nullptr) {
        ROSEN_LOGD("Destory renderContext!!");
        delete renderContext_;
        renderContext_ = nullptr;
    }
}

void RSRenderThread::Start()
{
    ROSEN_LOGD("RSRenderThread start.");
    running_.store(true);
    SetBackgroundStatus(false);
    if (thread_ == nullptr) {
        thread_ = std::make_unique<std::thread>(&RSRenderThread::RenderLoop, this);
    }
}

void RSRenderThread::Stop()
{
    running_.store(false);
    WakeUp();

    if (thread_ != nullptr && thread_->joinable()) {
        thread_->join();
    }

    thread_ = nullptr;
    ROSEN_LOGD("RSRenderThread stopped.");
}

void RSRenderThread::WakeUp()
{
    if (rendererLooper_ != nullptr) {
        rendererLooper_->WakeUp();
    }
}

void RSRenderThread::RecvTransactionData(std::unique_ptr<RSTransactionData>& transactionData)
{
    // StopTimer();
    {
        std::unique_lock<std::mutex> cmdLock(cmdMutex_);
        cmds_.emplace_back(std::move(transactionData));
    }
    // todo process in next vsync (temporarily)
    RSRenderThread::Instance().RequestNextVSync();
}

void RSRenderThread::RequestNextVSync()
{
    RS_TRACE_FUNC();
    if (vsyncClient_ != nullptr) {
        vsyncClient_->RequestNextVsync();
    }
}

int32_t RSRenderThread::GetTid()
{
    return tid_;
}

void RSRenderThread::RenderLoop()
{
    SystemCallSetThreadName("RSRenderThread");
    rendererLooper_ = RSThreadLooper::Create();
    threadHandler_ = RSThreadHandler::Create();

#ifdef ROSEN_OHOS
    tid_ = gettid();
    vsyncClient_ = RSVsyncClient::Create();
    if (vsyncClient_) {
        vsyncClient_->SetVsyncCallback(std::bind(&RSRenderThread::OnVsync, this, std::placeholders::_1));
    }
#endif
#ifdef ACE_ENABLE_GL
    renderContext_->InitializeEglContext(); // init egl context on RT
#endif

    while (running_.load()) {
        if (rendererLooper_ == nullptr) {
            break;
        }
        {
            if (preTask_ != nullptr) {
                threadHandler_->PostTask(preTask_);
                preTask_ = nullptr;
            }
        }
        if (running_.load()) {
            rendererLooper_->ProcessAllMessages(-1);
        }
    }

    StopTimer();
    vsyncClient_ = nullptr;
    rendererLooper_ = nullptr;
    threadHandler_ = nullptr;
}

void RSRenderThread::OnVsync(uint64_t timestamp)
{
    ROSEN_TRACE_BEGIN(BYTRACE_TAG_GRAPHIC_AGP, "RSRenderThread::OnVsync");
    mValue = (mValue + 1) % 2;
    RS_TRACE_INT("Vsync-client", mValue);
    timestamp_ = timestamp;
    if (!backgroundStatus_.load()) {
        StartTimer(0); // start render-loop now
    }
    ROSEN_TRACE_END(BYTRACE_TAG_GRAPHIC_AGP);
}

void RSRenderThread::SetBackgroundStatus(bool status)
{
    backgroundStatus_.store(status);
}

void RSRenderThread::StartTimer(uint64_t interval)
{
    if (threadHandler_ != nullptr) {
        if (timeHandle_ == nullptr) {
            timeHandle_ = RSThreadHandler::StaticCreateTask(mainFunc_);
        }
        threadHandler_->PostTaskDelay(timeHandle_, interval);
    }
}

void RSRenderThread::StopTimer()
{
    ROSEN_LOGD("RSRenderThread StopTimer.");
    if (threadHandler_ != nullptr) {
        if (timeHandle_ != nullptr) {
            threadHandler_->CancelTask(timeHandle_);
        }
    }
}

void RSRenderThread::ProcessCommands()
{
    std::unique_lock<std::mutex> cmdLock(cmdMutex_);
    if (cmds_.empty()) {
        return;
    }

    ROSEN_LOGD("RSRenderThread ProcessCommands size: %lu\n", cmds_.size());
    std::vector<std::unique_ptr<RSTransactionData>> cmds;
    std::swap(cmds, cmds_);
    cmdLock.unlock();

    ROSEN_TRACE_BEGIN(BYTRACE_TAG_GRAPHIC_AGP, "ProcessCommands");
    for (auto& cmdData : cmds) {
        cmdData->Process(context_);
    }
    ROSEN_TRACE_END(BYTRACE_TAG_GRAPHIC_AGP);
}

void RSRenderThread::Animate(uint64_t timestamp)
{
    ROSEN_TRACE_BEGIN(BYTRACE_TAG_GRAPHIC_AGP, "Animate");
    hasRunningAnimation_ = false;
    for (const auto& [id, node] : context_.GetNodeMap().renderNodeMap_) {
        hasRunningAnimation_ = node->Animate(timestamp) || hasRunningAnimation_;
    }

    if (hasRunningAnimation_) {
        RSRenderThread::Instance().RequestNextVSync();
    }
    ROSEN_TRACE_END(BYTRACE_TAG_GRAPHIC_AGP);
}

void RSRenderThread::Render()
{
    ROSEN_TRACE_BEGIN(BYTRACE_TAG_GRAPHIC_AGP, "RSRenderThread::Render");
    std::unique_lock<std::mutex> lock(mutex_);
    const auto& rootNode = context_.GetGlobalRootRenderNode();
    if (rootNode == nullptr) {
        ROSEN_LOGE("RSRenderThread::Render, rootNode is nullptr");
        return;
    }
    if (visitor_ == nullptr) {
        visitor_ = std::make_shared<RSRenderThreadVisitor>();
    }
    rootNode->Prepare(visitor_);
    rootNode->Process(visitor_);
    ROSEN_TRACE_END(BYTRACE_TAG_GRAPHIC_AGP);
}

void RSRenderThread::SendCommands()
{
    ROSEN_TRACE_BEGIN(BYTRACE_TAG_GRAPHIC_AGP, "RSRenderThread::SendCommands");
    RSUIDirector::RecvMessages();
    ROSEN_TRACE_END(BYTRACE_TAG_GRAPHIC_AGP);
}

void RSRenderThread::OnTransaction(std::shared_ptr<RSTransactionData> transactionData)
{
    ROSEN_TRACE_BEGIN(BYTRACE_TAG_GRAPHIC_AGP, "RSRenderThread::OnTransaction");
    RSUIDirector::RecvMessages(transactionData);
    ROSEN_TRACE_END(BYTRACE_TAG_GRAPHIC_AGP);
}

void RSRenderThread::Detach(NodeId id)
{
    if (auto node = context_.GetNodeMap().GetRenderNode<RSRootRenderNode>(id)) {
        std::unique_lock<std::mutex> lock(mutex_);
        context_.GetGlobalRootRenderNode()->RemoveChild(node);
    }
}

void RSRenderThread::PostTask(RSTaskMessage::RSTask task)
{
    if (threadHandler_) {
        auto taskHandle = threadHandler_->CreateTask(task);
        threadHandler_->PostTask(taskHandle, 0);
    }
}

} // namespace Rosen
} // namespace OHOS
