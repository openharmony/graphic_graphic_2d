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
#ifdef ROSEN_OHOS
#include <surface.h>

#include "platform/drawing/rs_platform_canvas.h"
#endif

namespace OHOS {
namespace Rosen {
RSRootRenderNode::RSRootRenderNode(NodeId id)
    : RSRenderNode(id), platformCanvas_(std::make_shared<PlatformCanvas>()) {}

RSRootRenderNode::~RSRootRenderNode() {}

void RSRootRenderNode::AttachSurface(uintptr_t surfaceProducer, int width, int height)
{
#ifdef ROSEN_OHOS
    if (platformCanvas_) {
        platformCanvas_->SetSurface(reinterpret_cast<Surface*>(surfaceProducer));
        platformCanvas_->SetSurfaceSize(width, height);
    }
#endif
}

void RSRootRenderNode::AttachRSSurface(std::shared_ptr<RSSurface> rsSurface, int width, int height)
{
    rsSurface_ = rsSurface;
    surfaceWidth_ = width;
    surfaceHeight_ = height;
}

int32_t RSRootRenderNode::GetWidth() const
{
    return surfaceWidth_;
}

int32_t RSRootRenderNode::GetHeight() const
{
    return surfaceHeight_;
}

std::shared_ptr<RSSurface> RSRootRenderNode::GetSurface()
{
    return rsSurface_;
}

std::shared_ptr<PlatformCanvas> RSRootRenderNode::GetPlatformCanvas()
{
    return platformCanvas_;
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
