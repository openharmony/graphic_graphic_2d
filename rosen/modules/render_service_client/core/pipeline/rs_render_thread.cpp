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
#include "pipeline/rs_root_render_node.h"

#ifdef ROSEN_OHOS
#include <sys/prctl.h>
#include <unistd.h>

#include "platform/drawing/rs_platform_canvas.h"
#endif
#include "common/rs_trace.h"
#include "pipeline/rs_render_node_map.h"
#include "pipeline/rs_render_thread_visitor.h"
#include "platform/common/rs_log.h"
#include "ui/rs_ui_director.h"
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
    mainFunc_ = [&]() {
        {
            std::unique_lock<std::mutex> cmdLock(cmdMutex_);
            if (timestamp_ == prevTimestamp_) {
                if (hasRunningAnimation_) {
                    RSRenderThread::Instance().RequestNextVSync();
                }
                return;
            }
            ROSEN_TRACE_BEGIN("", "DrawFrame");
            prevTimestamp_ = timestamp_;
            if (!cmds_.empty()) {
                ProcessCommands();
            }
        }

        Animate(prevTimestamp_);
        Render();
        SendCommands();
        ROSEN_TRACE_END("");
    };
}

RSRenderThread::~RSRenderThread() {}

void RSRenderThread::Start()
{
    ROSEN_LOGI("RSRenderThread start.");
    running_.store(true);
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
    ROSEN_LOGI("RSRenderThread stopped.");
}

void RSRenderThread::WakeUp()
{
    if (rendererLooper_ != nullptr) {
        rendererLooper_->WakeUp();
    }
}

void RSRenderThread::RecvTransactionData(std::unique_ptr<RSTransactionData>& transactionData)
{
    StopTimer();
    {
        std::unique_lock<std::mutex> cmdLock(cmdMutex_);
        cmds_.emplace_back(std::move(transactionData));
    }
    // todo process in next vsync (temporarily)
    RSRenderThread::Instance().RequestNextVSync();
}

void RSRenderThread::RequestNextVSync()
{
    if (vsyncClient_ != nullptr) {
        ROSEN_LOGI("RSRenderThread RequestNextVSync.");
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
    std::shared_ptr<RSBaseRenderNode> rootNode = std::make_shared<RSBaseRenderNode>(0);
    context_.SetGlobalRootRenderNode(rootNode);
#ifdef ROSEN_OHOS
    tid_ = gettid();
    vsyncClient_ = RSVsyncClient::Create();
    if (vsyncClient_) {
        vsyncClient_->SetVsyncCallback(std::bind(&RSRenderThread::OnVsync, this, std::placeholders::_1));
    }
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
    ROSEN_LOGI("RSRenderThread OnVsync(%lld).", timestamp);
    {
        std::unique_lock<std::mutex> cmdLock(cmdMutex_);
        timestamp_ = timestamp;
    }
    StartTimer(0); // start render-loop now
}

void RSRenderThread::StartTimer(uint64_t interval)
{
    ROSEN_LOGI("RSRenderThread StartTimer(%lld).", interval);
    if (threadHandler_ != nullptr) {
        if (timeHandle_ == nullptr) {
            timeHandle_ = RSThreadHandler::StaticCreateTask(mainFunc_);
        }
        threadHandler_->PostTaskDelay(timeHandle_, interval);
    }
}

void RSRenderThread::StopTimer()
{
    ROSEN_LOGI("RSRenderThread StopTimer.");
    if (threadHandler_ != nullptr) {
        if (timeHandle_ != nullptr) {
            threadHandler_->CancelTask(timeHandle_);
        }
    }
}

void RSRenderThread::ProcessCommands()
{
    ROSEN_LOGI("RSRenderThread::ProcessCommands() size: %lu\n", cmds_.size());
    ROSEN_TRACE_BEGIN("", "ProcessCommands");
    for (auto& cmdData : cmds_) {
        cmdData->Process(context_);
    }
    cmds_.clear();
    ROSEN_TRACE_END("");
}

void RSRenderThread::Animate(uint64_t timestamp)
{
    ROSEN_TRACE_BEGIN("", "Animate");
    hasRunningAnimation_ = false;
    for (const auto& [id, node] : context_.GetNodeMap().renderNodeMap_) {
        if (auto renderNode = RSBaseRenderNode::ReinterpretCast<RSPropertyRenderNode>(node)) {
            hasRunningAnimation_ |= renderNode->Animate(timestamp);
        }
    }

    if (hasRunningAnimation_) {
        RSRenderThread::Instance().RequestNextVSync();
    }
    ROSEN_TRACE_END("");
}

void RSRenderThread::Render()
{
    ROSEN_TRACE_BEGIN("", "Render");
    const auto& rootNode = context_.GetGlobalRootRenderNode();
    if (rootNode == nullptr) {
        return;
    }
    std::shared_ptr<RSNodeVisitor> visitor = std::make_shared<RSRenderThreadVisitor>();
    rootNode->Prepare(visitor);
    rootNode->Process(visitor);
    ROSEN_TRACE_END("");
}

void RSRenderThread::SendCommands()
{
    RSUIDirector::RecvMessages();
}

void RSRenderThread::Detach(NodeId id)
{
    if (auto node = context_.GetNodeMap().GetRenderNode<RSRootRenderNode>(id)) {
        if (auto platformCanvas = node->GetPlatformCanvas()) {
            platformCanvas->Disable();
        }
        context_.GetGlobalRootRenderNode()->RemoveChild(node);
    }
}
} // namespace Rosen
} // namespace OHOS
