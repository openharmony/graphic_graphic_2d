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

#include "rs_driven_surface_render_node.h"

#include "common/rs_obj_abs_geometry.h"
#include "platform/common/rs_log.h"
#include "platform/ohos/backend/rs_surface_ohos_gl.h"
#include "platform/ohos/backend/rs_surface_ohos_raster.h"
#include "screen_manager/screen_types.h"
#include "visitor/rs_node_visitor.h"

namespace OHOS {
namespace Rosen {
RSDrivenSurfaceRenderNode::RSDrivenSurfaceRenderNode(NodeId id, std::weak_ptr<RSContext> context)
    : RSRenderNode(id, context), RSSurfaceHandler(id)
{}

void RSDrivenSurfaceRenderNode::SetDrivenCanvasNode(RSBaseRenderNode::SharedPtr node)
{
    drivenCanvasNode_ = node;
}

RSBaseRenderNode::SharedPtr RSDrivenSurfaceRenderNode::GetDrivenCanvasNode() const
{
    return drivenCanvasNode_;
}

const RectI& RSDrivenSurfaceRenderNode::GetSrcRect() const
{
    return srcRect_;
}

const RectI& RSDrivenSurfaceRenderNode::GetDstRect() const
{
    return dstRect_;
}
} // namespace Rosen
} // namespace OHOS
