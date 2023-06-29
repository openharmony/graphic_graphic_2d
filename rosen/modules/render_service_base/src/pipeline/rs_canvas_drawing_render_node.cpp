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

#include "pipeline/rs_canvas_drawing_render_node.h"

#include "include/core/SkCanvas.h"

#include "common/rs_common_def.h"
#include "common/rs_obj_abs_geometry.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "platform/common/rs_log.h"
#include "property/rs_properties_painter.h"
#include "visitor/rs_node_visitor.h"

namespace OHOS {
namespace Rosen {
RSCanvasDrawingRenderNode::RSCanvasDrawingRenderNode(NodeId id, std::weak_ptr<RSContext> context)
    : RSCanvasRenderNode(id, context)
{}

RSCanvasDrawingRenderNode::~RSCanvasDrawingRenderNode() {}

void RSCanvasDrawingRenderNode::ProcessRenderContents(RSPaintFilterCanvas& canvas)
{
    if (GetRenderProperties().GetBoundsWidth() <= 0 || GetRenderProperties().GetBoundsHeight() <= 0) {
        RS_LOGE("RSCanvasDrawingRenderNode::ProcessRenderContents: The width or height of the canvas is less than or "
                "equal to 0");
        return;
    }
    if (!skSurface_ ||
        static_cast<int>(GetRenderProperties().GetBoundsWidth()) != static_cast<int>(skSurface_->width()) ||
        static_cast<int>(GetRenderProperties().GetBoundsHeight()) != static_cast<int>(skSurface_->height())) {
        SkImageInfo info = SkImageInfo::Make(GetRenderProperties().GetBoundsWidth(),
            GetRenderProperties().GetBoundsHeight(), kRGBA_8888_SkColorType, kPremul_SkAlphaType);

#if (defined RS_ENABLE_GL) && (defined RS_ENABLE_EGLIMAGE)
#ifdef NEW_SKIA
        auto grContext = canvas.recordingContext();
#else
        auto grContext = canvas.getGrContext();
#endif
        if (grContext == nullptr) {
            RS_LOGE("RSCanvasDrawingRenderNode::ProcessRenderContents: GrContext is nullptr");
            return;
        }
        skSurface_ = SkSurface::MakeRenderTarget(grContext, SkBudgeted::kNo, info);
#else
        skSurface_ = SkSurface::MakeRaster(info);
#endif
        canvas_ = std::make_unique<RSPaintFilterCanvas>(skSurface_.get());
    }

    RSModifierContext context = { GetMutableRenderProperties(), canvas_.get() };
    ApplyDrawCmdModifier(context, RSModifierType::CONTENT_STYLE);

    auto image = skSurface_->makeImageSnapshot();
    auto srcRect = SkRect::MakeXYWH(0, 0, image->width(), image->height());
    auto dstRect =
        SkRect::MakeXYWH(0, 0, GetRenderProperties().GetFrameWidth(), GetRenderProperties().GetFrameHeight());
#ifdef NEW_SKIA
    canvas.drawImageRect(image, srcRect, dstRect, SkSamplingOptions(), nullptr, SkCanvas::kFast_SrcRectConstraint);
#else
    canvas.drawImageRect(image, srcRect, dstRect, nullptr, SkCanvas::kFast_SrcRectConstraint);
#endif
}

void RSCanvasDrawingRenderNode::ApplyDrawCmdModifier(RSModifierContext& context, RSModifierType type) const
{
    auto it = drawCmdModifiers_.find(type);
    if (it == drawCmdModifiers_.end() || it->second.empty()) {
        return;
    }
    for (const auto& modifier : it->second) {
        modifier->Apply(context);
        auto prop = modifier->GetProperty();
        auto cmd = std::static_pointer_cast<RSRenderProperty<DrawCmdListPtr>>(prop)->Get();
        cmd->ClearOp();
    }
}

bool RSCanvasDrawingRenderNode::GetBitmap(SkBitmap& bitmap)
{
    if (skSurface_ == nullptr) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetBitmap: SkSurface is nullptr");
        return false;
    }
    sk_sp<SkImage> image = skSurface_->makeImageSnapshot();
    if (image == nullptr) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetBitmap: SkImage is nullptr");
        return false;
    }
    if (!image->asLegacyBitmap(&bitmap)) {
        RS_LOGE("RSCanvasDrawingRenderNode::GetBitmap: asLegacyBitmap failed");
        return false;
    }
    return true;
}
} // namespace Rosen
} // namespace OHOS