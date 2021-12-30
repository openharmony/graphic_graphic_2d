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
#include "common/rs_trace.h"
#include "pipeline/rs_node_map.h"
#include "pipeline/rs_render_thread.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_transaction_proxy.h"
#include "ui/rs_root_node.h"
#include "ui/rs_surface_extractor.h"
#include "ui/rs_surface_node.h"

namespace OHOS {
namespace Rosen {
static TaskRunner g_uiTaskRunner;

std::shared_ptr<RSUIDirector> RSUIDirector::Create()
{
    auto instance = std::shared_ptr<RSUIDirector>(new RSUIDirector());
    return instance;
}

RSUIDirector::~RSUIDirector()
{
    Destory();
}

void RSUIDirector::Init()
{
    auto renderThreadClient = RSIRenderClient::CreateRenderThreadClient();
    RSTransactionProxy::GetInstance().SetRenderThreadClient(renderThreadClient);
    RSRenderThread::Instance().Start();
}

void RSUIDirector::Destory()
{
    if (root_ != 0) {
        RSRenderThread::Instance().Detach(root_);
        root_ = 0;
    }
}

// ori version, input is Surface*
void RSUIDirector::SetPlatformSurface(Surface* surface)
{
    surface_ = reinterpret_cast<uintptr_t>(surface);
}

void RSUIDirector::SetSurfaceSize(int width, int height)
{
    surfaceWidth_ = width;
    surfaceHeight_ = height;
    if (root_ != 0 && surface_ != 0) {
        auto node = RSNodeMap::Instance().GetNode(root_).lock();
        if (node != nullptr) {
            std::static_pointer_cast<RSRootNode>(node)->AttachSurface(surface_, surfaceWidth_, surfaceHeight_);
        }
    }
}
// end of ori version

// new version, input is std::shared_ptr<RSSurfaceNode>
void RSUIDirector::SetRSSurfaceNode(std::shared_ptr<RSSurfaceNode> surfaceNode)
{
    surfaceNode_ = surfaceNode;
    if (root_ == 0) {
        ROSEN_LOGE("no root exists");
        return;
    }
    auto node = RSNodeMap::Instance().GetNode(root_).lock();
    if (node == nullptr) {
        ROSEN_LOGE("get root node failed");
        return;
    }
    if (surfaceNode == nullptr) {
        std::static_pointer_cast<RSRootNode>(node)->AttachRSSurface(0, 0, 0);
    } else {
        std::static_pointer_cast<RSRootNode>(node)->AttachRSSurface(
            RSSurfaceExtractor::ExtractRSSurface(surfaceNode_),
            static_cast<int>(surfaceNode_->GetStagingProperties().GetBoundsWidth()),
            static_cast<int>(surfaceNode_->GetStagingProperties().GetBoundsHeight()));
    }
}

void RSUIDirector::SetSurfaceNodeSize(int width, int height)
{
    if ((root_ == 0) || (surfaceNode_ == nullptr)) {
        ROSEN_LOGE("No root or SurfaceNode exists");
        return;
    }
    auto node = RSNodeMap::Instance().GetNode(root_).lock();
    if (node != nullptr) {
        std::static_pointer_cast<RSRootNode>(node)->AttachRSSurface(
            RSSurfaceExtractor::ExtractRSSurface(surfaceNode_), width, height);
    }
}
// end of new version

void RSUIDirector::SetRoot(NodeId root)
{
    if (root_ == root) {
        return;
    }
    root_ = root;
    auto node = RSNodeMap::Instance().GetNode(root_).lock();
    if (node == nullptr) {
        ROSEN_LOGE("fail to get node");
        return;
    }
    // contains two versions, one for weston and one for RSSurfaceNode
    // TODO: remove the weston version
    if ((surface_ == 0) && (surfaceNode_ == nullptr)) {
        ROSEN_LOGE("No Surface or SurfaceNode found");
        return;
    }
    if (surface_ != 0) {
        std::static_pointer_cast<RSRootNode>(node)->AttachSurface(surface_, surfaceWidth_, surfaceHeight_);
    } else {
        std::static_pointer_cast<RSRootNode>(node)->AttachRSSurface(
            RSSurfaceExtractor::ExtractRSSurface(surfaceNode_),
            static_cast<int>(surfaceNode_->GetStagingProperties().GetBoundsWidth()),
            static_cast<int>(surfaceNode_->GetStagingProperties().GetBoundsHeight()));
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
    ROSEN_TRACE_BEGIN("", "SendCommands");
    auto nodePtr = std::static_pointer_cast<RSNode>(RSNodeMap::Instance().GetNode(root_).lock());
    if (nodePtr != nullptr) {
        nodePtr->UpdateRecording();
    }
    RSTransactionProxy::GetInstance().FlushImplicitTransaction();
    ROSEN_TRACE_END("");
}

void RSUIDirector::RecvMessages()
{
    if (g_uiTaskRunner == nullptr) {
        ROSEN_LOGE("Notify ui message failed, uiTaskRunner is null");
        return;
    }

    std::queue<std::shared_ptr<RSCommand>> cmds;
    RSMessageProcessor::Instance().CommitUIMsg(cmds);
    g_uiTaskRunner([msgs = cmds]() { RSUIDirector::ProcessMessages(msgs); });
}

void RSUIDirector::ProcessMessages(std::queue<std::shared_ptr<RSCommand>> cmds)
{
    while (!(cmds.empty())) {
        auto msg = cmds.front();
        if (msg != nullptr) {
            auto cmd = std::static_pointer_cast<RSAnimationFinishCallback>(msg);
            auto nodePtr = RSNodeMap::Instance().GetNode(cmd->parameter1_).lock();
            if (nodePtr == nullptr) {
                return;
            }
            std::static_pointer_cast<RSNode>(nodePtr)->AnimationFinish(cmd->parameter2_);
        }
        cmds.pop();
    }
}
} // namespace Rosen
} // namespace OHOS
