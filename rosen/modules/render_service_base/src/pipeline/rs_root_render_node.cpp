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

#include "pipeline/rs_root_render_node.h"

#include "platform/drawing/rs_surface.h"
#include "visitor/rs_node_visitor.h"
#include "command/rs_surface_node_command.h"
#include "transaction/rs_transaction_proxy.h"
#ifdef ROSEN_OHOS
#include <surface.h>

#endif

namespace OHOS {
namespace Rosen {
RSRootRenderNode::RSRootRenderNode(NodeId id) : RSCanvasRenderNode(id) {}

RSRootRenderNode::~RSRootRenderNode() {}


void RSRootRenderNode::AttachRSSurfaceNode(NodeId surfaceNodeId, int width, int height)
{
    surfaceNodeId_ = surfaceNodeId;
    surfaceWidth_ = width;
    surfaceHeight_ = height;
}

int32_t RSRootRenderNode::GetSurfaceWidth() const
{
    return surfaceWidth_;
}

int32_t RSRootRenderNode::GetSurfaceHeight() const
{
    return surfaceHeight_;
}

std::shared_ptr<RSSurface> RSRootRenderNode::GetSurface()
{
    return rsSurface_;
}

NodeId RSRootRenderNode::GetRSSurfaceNodeId()
{
    return surfaceNodeId_;
}

void RSRootRenderNode::AddSurfaceRenderNode(NodeId id)
{
    childSurfaceNodeId_.push_back(id);
}

void RSRootRenderNode::ClearSurfaceNodeInRS()
{
    for (auto childId : childSurfaceNodeId_) {
        std::unique_ptr<RSCommand> command = std::make_unique<RSSurfaceNodeRemoveSelf>(childId);
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            transactionProxy->AddCommand(command, true);
        }
    }
    childSurfaceNodeId_.clear();
}

void RSRootRenderNode::Prepare(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    visitor->PrepareRootRenderNode(*this);
}

void RSRootRenderNode::Process(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    visitor->ProcessRootRenderNode(*this);
}
} // namespace Rosen
} // namespace OHOS
