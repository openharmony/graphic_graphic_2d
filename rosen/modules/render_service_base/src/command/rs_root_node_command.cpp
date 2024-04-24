/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "command/rs_root_node_command.h"

#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "platform/common/rs_log.h"

#ifdef NEW_RENDER_CONTEXT
#include "rs_render_surface.h"
#else
#include "platform/drawing/rs_surface.h"
#endif
#include "pipeline/rs_render_node_gc.h"

namespace OHOS {
namespace Rosen {

void RootNodeCommandHelper::Create(RSContext& context, NodeId id, bool isTextureExportNode)
{
    auto node = std::shared_ptr<RSRootRenderNode>(new RSRootRenderNode(id,
        context.weak_from_this(), isTextureExportNode), RSRenderNodeGC::NodeDestructor);
    context.GetMutableNodeMap().RegisterRenderNode(node);
}

void RootNodeCommandHelper::AttachRSSurfaceNode(RSContext& context, NodeId id, NodeId surfaceNodeId)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSRootRenderNode>(id)) {
        node->AttachRSSurfaceNode(surfaceNodeId);
        context.GetGlobalRootRenderNode()->AddChild(node);
    }
}

void RootNodeCommandHelper::SetEnableRender(RSContext& context, NodeId id, bool flag)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSRootRenderNode>(id)) {
        node->SetEnableRender(flag);
    }
}

void RootNodeCommandHelper::AttachToUniSurfaceNode(RSContext& context, NodeId id, NodeId surfaceNodeId)
{
    auto& nodeMap = context.GetNodeMap();
    auto parent = nodeMap.GetRenderNode<RSSurfaceRenderNode>(surfaceNodeId);
    auto node = nodeMap.GetRenderNode<RSRootRenderNode>(id);
    if (!parent || !node) {
        RS_LOGE("unirender: RootNodeCommandHelper::AttachToUniSurfaceNode surfaceNodeId:%{public}" PRIu64 " id"
            ":%{public}" PRIu64 ", parent valid:%{public}d, node valid:%{public}d",
            surfaceNodeId, id, parent != nullptr, node != nullptr);
        return;
    }
    parent->AddChild(node);
    parent->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
}

void RootNodeCommandHelper::UpdateSuggestedBufferSize(RSContext& context, NodeId id, float width, float height)
{
    if (auto node = context.GetNodeMap().GetRenderNode<RSRootRenderNode>(id)) {
        node->UpdateSuggestedBufferSize(width, height);
    }
}
} // namespace Rosen
} // namespace OHOS
