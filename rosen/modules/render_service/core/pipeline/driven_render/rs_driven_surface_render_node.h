/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CORE_RS_DRIVEN_SURFACE_RENDER_NODE_H
#define RENDER_SERVICE_CORE_RS_DRIVEN_SURFACE_RENDER_NODE_H

#include <ibuffer_consumer_listener.h>
#include <memory>
#include <surface.h>

#include "common/rs_rect.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_handler.h"
#include "platform/drawing/rs_surface.h"
#include "render_context/render_context.h"
#include "sync_fence.h"

namespace OHOS {
namespace Rosen {
class RSDrivenSurfaceRenderNode : public RSRenderNode, public RSSurfaceHandler {
public:
    using WeakPtr = std::weak_ptr<RSDrivenSurfaceRenderNode>;
    using SharedPtr = std::shared_ptr<RSDrivenSurfaceRenderNode>;

    explicit RSDrivenSurfaceRenderNode(NodeId id, std::weak_ptr<RSContext> context = {});
    ~RSDrivenSurfaceRenderNode() override = default;

    const RectI& GetSrcRect() const;
    const RectI& GetDstRect() const;

    void SetDrivenCanvasNode(RSBaseRenderNode::SharedPtr node);
    RSBaseRenderNode::SharedPtr GetDrivenCanvasNode() const;

protected:
    RectI srcRect_;
    RectI dstRect_;

    RSBaseRenderNode::SharedPtr drivenCanvasNode_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CORE_RS_DRIVEN_SURFACE_RENDER_NODE_H
