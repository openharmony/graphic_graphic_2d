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
#include "pipeline/rs_main_thread.h"

#include "command/rs_message_processor.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_render_service_util.h"
#include "pipeline/rs_render_service_visitor.h"
#include "platform/common/rs_log.h"
#include "platform/drawing/rs_vsync_client.h"
#include "rs_trace.h"
#include "screen_manager/rs_screen_manager.h"
#include "transaction/rs_transaction_proxy.h"

namespace OHOS {
namespace Rosen {
RSMainThread* RSMainThread::Instance()
{
    static RSMainThread instance;
    return &instance;
}

RSMainThread::RSMainThread() : mainThreadId_(std::this_thread::get_id())
{
}

RSMainThread::~RSMainThread() noexcept
{
}

void RSMainThread::Init()
{
    mainLoop_ = [&]() {
        ROSEN_LOGI("RsDebug mainLoop start");
        ROSEN_TRACE_BEGIN(BYTRACE_TAG_GRAPHIC_AGP, "RSMainThread::DoComposition");
        ProcessCommand();
        Animate(timestamp_);
        Render();
        SendCommands();
        ROSEN_TRACE_END(BYTRACE_TAG_GRAPHIC_AGP);
        ROSEN_LOGI("RsDebug mainLoop end");
    };

    threadLooper_ = RSThreadLooper::Create();
    threadHandler_ = RSThreadHandler::Create();

    sptr<VSyncConnection> conn = new VSyncConnection(rsVSyncDistributor_, "rs");
    rsVSyncDistributor_->AddConnection(conn);
    receiver_ = std::make_shared<VSyncReceiver>(conn);
    receiver_->Init();
    RsRenderServiceUtil::InitEnableClient();
}

void RSMainThread::Start()
{
    while (true) {
        threadLooper_->ProcessAllMessages(-1);
    }
}

void RSMainThread::ProcessCommand()
{
    {
        std::lock_guard<std::mutex> lock(transitionDataMutex_);
        std::swap(cacheCommandQueue_, effectCommandQueue_);
    }
    while (!effectCommandQueue_.empty())
    {
        auto rsTransaction = std::move(effectCommandQueue_.front());
        effectCommandQueue_.pop();
        if (rsTransaction) {
            rsTransaction->Process(context_);
        }
    }
}

void RSMainThread::Render()
{
    const std::shared_ptr<RSBaseRenderNode> rootNode = context_.GetGlobalRootRenderNode();
    if (rootNode == nullptr) {
        ROSEN_LOGE("RSMainThread::Draw GetGlobalRootRenderNode fail");
        return;
    }
    std::shared_ptr<RSNodeVisitor> visitor = std::make_shared<RSRenderServiceVisitor>();
    rootNode->Prepare(visitor);
    rootNode->Process(visitor);
}

void RSMainThread::RequestNextVSync()
{
    RS_TRACE_FUNC();
    VSyncReceiver::FrameCallback fcb = {
        .userData_ = this,
        .callback_ = std::bind(&RSMainThread::OnVsync, this, ::std::placeholders::_1, ::std::placeholders::_2),
    };
    if (receiver_ != nullptr) {
        receiver_->RequestNextVSync(fcb);
    }
}

void RSMainThread::OnVsync(uint64_t timestamp, void *data)
{
    ROSEN_TRACE_BEGIN(BYTRACE_TAG_GRAPHIC_AGP, "RSMainThread::OnVsync");
    timestamp_ = timestamp;
    if (threadHandler_) {
        if (!taskHandle_) {
            taskHandle_ = RSThreadHandler::StaticCreateTask(mainLoop_);
        }
        threadHandler_->PostTaskDelay(taskHandle_, 0);

        auto screenManager_ = CreateOrGetScreenManager();
        if (screenManager_ != nullptr) {
            PostTask([=](){
                screenManager_->ProcessScreenHotPlugEvents();
            });
        }
    }
    ROSEN_TRACE_END(BYTRACE_TAG_GRAPHIC_AGP);
}

void RSMainThread::Animate(uint64_t timestamp)
{
    RS_TRACE_FUNC();

    if (context_.animatingNodeList_.empty()) {
        return;
    }

    // iterate and animate all animating nodes, remove if animation finished
    std::__libcpp_erase_if_container(context_.animatingNodeList_, [timestamp](const auto& iter) -> bool {
        auto node = iter.second.lock();
        if (node == nullptr) {
            ROSEN_LOGD("RSMainThread::Animate removing expired animating node");
            return true;
        }
        bool animationFinished = !node->Animate(timestamp);
        if (animationFinished) {
            ROSEN_LOGD("RSMainThread::Animate removing finished animating node %llu", node->GetId());
        }
        return animationFinished;
    });

    RequestNextVSync();
}

void RSMainThread::RecvRSTransactionData(std::unique_ptr<RSTransactionData>& rsTransactionData)
{
    {
        std::lock_guard<std::mutex> lock(transitionDataMutex_);
        cacheCommandQueue_.push(std::move(rsTransactionData));
    }
    RequestNextVSync();
}

void RSMainThread::PostTask(RSTaskMessage::RSTask task)
{
    if (threadHandler_) {
        auto taskHandle = threadHandler_->CreateTask(task);
        threadHandler_->PostTask(taskHandle, 0);
    }
}

void RSMainThread::RegisterApplicationRenderThread(uint32_t pid, sptr<IApplicationRenderThread> app)
{
    applicationRenderThreadMap_.emplace(pid, app);
}

void RSMainThread::UnregisterApplicationRenderThread(sptr<IApplicationRenderThread> app)
{
    std::__libcpp_erase_if_container(applicationRenderThreadMap_, [&app](auto& iter) { return iter.second == app; });
}

void RSMainThread::SendCommands()
{
    RS_TRACE_FUNC();
    if (!RSMessageProcessor::Instance().HasTransaction()) {
        return;
    }

    // dispatch messages to corresponding application
    auto transactionMapPtr = std::make_shared<std::unordered_map<uint32_t, RSTransactionData>>(
        RSMessageProcessor::Instance().GetAllTransactions());
    PostTask([this, transactionMapPtr]() {
        for (auto& transactionIter : *transactionMapPtr) {
            auto pid = transactionIter.first;
            auto appIter = applicationRenderThreadMap_.find(pid);
            if (appIter == applicationRenderThreadMap_.end()) {
                ROSEN_LOGI("RSMainThread::SendCommand no application found for pid %d", pid);
                continue;
            }
            auto& app = appIter->second;
            auto transactionPtr = std::make_shared<RSTransactionData>(std::move(transactionIter.second));
            app->OnTransaction(transactionPtr);
        }
    });
}
} // namespace Rosen
} // namespace OHOS
