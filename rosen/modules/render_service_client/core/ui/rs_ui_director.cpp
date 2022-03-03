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

#include "ui/rs_ui_director.h"

#include "command/rs_animation_command.h"
#include "command/rs_message_processor.h"
#include "pipeline/rs_frame_report.h"
#include "pipeline/rs_node_map.h"
#include "pipeline/rs_render_thread.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"
#include "transaction/rs_interfaces.h"
#include "transaction/rs_transaction_proxy.h"
#include "ui/rs_root_node.h"
#include "ui/rs_surface_extractor.h"
#include "ui/rs_surface_node.h"

namespace OHOS {
namespace Rosen {
static TaskRunner g_uiTaskRunner;

std::shared_ptr<RSUIDirector> RSUIDirector::Create()
{
    return std::shared_ptr<RSUIDirector>(new RSUIDirector());
}

RSUIDirector::~RSUIDirector()
{
    Destory();
}

void RSUIDirector::Init()
{
    auto renderThreadClient = RSIRenderClient::CreateRenderThreadClient();
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->SetRenderThreadClient(renderThreadClient);
    }

    AnimationCommandHelper::SetFinisCallbackProcessor(AnimationCallbackProcessor);

    RsFrameReport::GetInstance().Init();
    RSRenderThread::Instance().Start();
    GoForeground();
}

void RSUIDirector::GoForeground()
{
    ROSEN_LOGI("RSUIDirector::GoForeground");
    if (!isActive_) {
        RSRenderThread::Instance().UpdateWindowStatus(true);
        isActive_ = true;
        if (auto node = RSNodeMap::Instance().GetNode<RSRootNode>(root_)) {
            node->SetVisible(true);
        }
    }
}

void RSUIDirector::GoBackground()
{
    ROSEN_LOGI("RSUIDirector::GoBackground");
    if (isActive_) {
        RSRenderThread::Instance().UpdateWindowStatus(false);
        isActive_ = false;
        if (auto node = RSNodeMap::Instance().GetNode<RSRootNode>(root_)) {
            node->SetVisible(false);
        }
    }
}

void RSUIDirector::Destory()
{
    if (root_ != 0) {
        RSRenderThread::Instance().Detach(root_);
        root_ = 0;
    }
    GoBackground();
}

void RSUIDirector::SetRSSurfaceNode(std::shared_ptr<RSSurfaceNode> surfaceNode)
{
    surfaceNode_ = surfaceNode;
    AttachSurface();
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
    if (node != nullptr && surfaceNode_ != nullptr) {
        node->AttachRSSurfaceNode(surfaceNode_);
        ROSEN_LOGD("RSUIDirector::AttachSurface [%llu]", surfaceNode_->GetId());
    } else {
        ROSEN_LOGD("RSUIDirector::AttachSurface not ready");
    }
}

void RSUIDirector::SetTimeStamp(uint64_t timeStamp)
{
    timeStamp_ = timeStamp;
}

void RSUIDirector::SetUITaskRunner(const TaskRunner& uiTaskRunner)
{
    g_uiTaskRunner = uiTaskRunner;
}

void RSUIDirector::SendMessages()
{
    ROSEN_TRACE_BEGIN(BYTRACE_TAG_GRAPHIC_AGP, "SendCommands");
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->FlushImplicitTransaction();
    }
    ROSEN_TRACE_END(BYTRACE_TAG_GRAPHIC_AGP);
}

void RSUIDirector::RecvMessages()
{
    if (getpid() == -1) {
        return;
    }
    static const uint32_t pid = static_cast<uint32_t>(getpid());
    if (!RSMessageProcessor::Instance().HasTransaction(pid)) {
        return;
    }
    auto transactionDataPtr = std::make_shared<RSTransactionData>(RSMessageProcessor::Instance().GetTransaction(pid));
    RecvMessages(transactionDataPtr);
}

void RSUIDirector::RecvMessages(std::shared_ptr<RSTransactionData> cmds)
{
    if (g_uiTaskRunner == nullptr) {
        ROSEN_LOGE("RSUIDirector::RecvMessages, Notify ui message failed, uiTaskRunner is null");
        return;
    }
    if (cmds == nullptr || cmds->IsEmpty()) {
        return;
    }

    g_uiTaskRunner([cmds]() { RSUIDirector::ProcessMessages(cmds); });
}

void RSUIDirector::ProcessMessages(std::shared_ptr<RSTransactionData> cmds)
{
    static RSContext context; // RSCommand->process() needs it
    cmds->Process(context);
}

void RSUIDirector::AnimationCallbackProcessor(NodeId nodeId, AnimationId animId)
{
    if (auto nodePtr = RSNodeMap::Instance().GetNode<RSNode>(nodeId)) {
        nodePtr->AnimationFinish(animId);
    }
}
} // namespace Rosen
} // namespace OHOS
