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

#include "pipeline/rs_root_render_node.h"

#ifdef NEW_RENDER_CONTEXT
#include "rs_render_surface.h"
#else
#include "platform/drawing/rs_surface.h"
#endif
#include "transaction/rs_transaction_proxy.h"
#include "visitor/rs_node_visitor.h"
#ifndef ROSEN_CROSS_PLATFORM
#include <surface.h>
#endif

namespace OHOS {
namespace Rosen {
RSRootRenderNode::RSRootRenderNode(NodeId id, const std::weak_ptr<RSContext>& context, bool isTextureExportNode)
    : RSCanvasRenderNode(id, context, isTextureExportNode), dirtyManager_(std::make_shared<RSDirtyRegionManager>())
{
#ifndef ROSEN_ARKUI_X
    MemoryInfo info = {sizeof(*this), ExtractPid(id), id, MEMORY_TYPE::MEM_RENDER_NODE};
    MemoryTrack::Instance().AddNodeRecord(id, info);
#endif
}

RSRootRenderNode::~RSRootRenderNode()
{
#ifndef ROSEN_ARKUI_X
    MemoryTrack::Instance().RemoveNodeRecord(GetId());
#endif
}

void RSRootRenderNode::AttachRSSurfaceNode(NodeId surfaceNodeId)
{
    surfaceNodeId_ = surfaceNodeId;
}

float RSRootRenderNode::GetSuggestedBufferWidth() const
{
    return suggestedBufferWidth_;
}

float RSRootRenderNode::GetSuggestedBufferHeight() const
{
    return suggestedBufferHeight_;
}

void RSRootRenderNode::UpdateSuggestedBufferSize(float width, float height)
{
    suggestedBufferHeight_ = height;
    suggestedBufferWidth_ = width;
}
#ifdef NEW_RENDER_CONTEXT
std::shared_ptr<RSRenderSurface> RSRootRenderNode::GetSurface()
#else
std::shared_ptr<RSSurface> RSRootRenderNode::GetSurface()
#endif
{
    return rsSurface_;
}

NodeId RSRootRenderNode::GetRSSurfaceNodeId()
{
    return surfaceNodeId_;
}

std::shared_ptr<RSDirtyRegionManager> RSRootRenderNode::GetDirtyManager() const
{
    return dirtyManager_;
}

void RSRootRenderNode::Prepare(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    if (!visitor) {
        return;
    }
    RSRenderNode::ApplyModifiers();
    visitor->PrepareRootRenderNode(*this);
}

void RSRootRenderNode::Process(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    if (!visitor) {
        return;
    }
    RSRenderNode::RenderTraceDebug();
    visitor->ProcessRootRenderNode(*this);
}
} // namespace Rosen
} // namespace OHOS
