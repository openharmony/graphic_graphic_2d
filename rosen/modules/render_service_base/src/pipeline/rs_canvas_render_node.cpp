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

#include "pipeline/rs_canvas_render_node.h"

#include <algorithm>

#ifdef ROSEN_OHOS
#include "common/rs_obj_abs_geometry.h"
#include "include/core/SkCanvas.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "property/rs_properties_painter.h"
#include "render/rs_blur_filter.h"
#endif
#include "platform/common/rs_log.h"
#include "visitor/rs_node_visitor.h"

namespace OHOS {
namespace Rosen {

RSCanvasRenderNode::RSCanvasRenderNode(NodeId id, std::weak_ptr<RSContext> context) : RSRenderNode(id, context) {}

RSCanvasRenderNode::~RSCanvasRenderNode() {}

void RSCanvasRenderNode::UpdateRecording(std::shared_ptr<DrawCmdList> drawCmds, bool drawContentLast)
{
    drawCmdList_ = drawCmds;
    drawContentLast_ = drawContentLast;
    SetDirty();
}

void RSCanvasRenderNode::Prepare(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    visitor->PrepareCanvasRenderNode(*this);
}

void RSCanvasRenderNode::Process(const std::shared_ptr<RSNodeVisitor>& visitor)
{
    visitor->ProcessCanvasRenderNode(*this);
}

void RSCanvasRenderNode::ProcessRenderContents(RSPaintFilterCanvas& canvas)
{
#ifdef ROSEN_OHOS
    RSPropertiesPainter::DrawFrame(GetRenderProperties(), canvas, drawCmdList_);
#endif
}

void RSCanvasRenderNode::ProcessRenderBeforeChildren(RSPaintFilterCanvas& canvas)
{
#ifdef ROSEN_OHOS
    RSRenderNode::ProcessRenderBeforeChildren(canvas);

    RSPropertiesPainter::DrawBackground(GetRenderProperties(), canvas);
    auto filter = std::static_pointer_cast<RSSkiaFilter>(GetRenderProperties().GetFilter());
    if (filter != nullptr) {
        RSPropertiesPainter::SaveLayerForFilter(GetRenderProperties(), canvas, filter);
    }

    canvas.save();
    canvas.translate(GetRenderProperties().GetFrameOffsetX(), GetRenderProperties().GetFrameOffsetY());

    if (GetRenderProperties().GetClipToFrame()) {
        RSPropertiesPainter::Clip(canvas, GetRenderProperties().GetFrameRect());
    }
    if (!drawContentLast_) {
        ProcessRenderContents(canvas);
    }
#endif
}

void RSCanvasRenderNode::ProcessRenderAfterChildren(RSPaintFilterCanvas& canvas)
{
#ifdef ROSEN_OHOS
    if (drawContentLast_) {
        ProcessRenderContents(canvas);
    }
    canvas.restore();

    auto filter = std::static_pointer_cast<RSSkiaFilter>(GetRenderProperties().GetFilter());
    if (filter != nullptr) {
        RSPropertiesPainter::RestoreForFilter(canvas);
    }

    RSPropertiesPainter::DrawBorder(GetRenderProperties(), canvas);
    RSPropertiesPainter::DrawForegroundColor(GetRenderProperties(), canvas);
    RSRenderNode::ProcessRenderAfterChildren(canvas);
#endif
}

} // namespace Rosen
} // namespace OHOS
