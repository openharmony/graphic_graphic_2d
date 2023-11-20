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

#include "property/rs_property_drawable_frame_geometry.h"

#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_render_node.h"
#include "platform/common/rs_log.h"
#include "property/rs_properties.h"
#include "property/rs_properties_painter.h"

namespace OHOS::Rosen {
void RSFrameGeometryDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
#ifndef USE_ROSEN_DRAWING
    canvas.translate(node.GetRenderProperties().GetFrameOffsetX(), node.GetRenderProperties().GetFrameOffsetY());
#else
    canvas.Translate(node.GetRenderProperties().GetFrameOffsetX(), node.GetRenderProperties().GetFrameOffsetY());
#endif
}
RSPropertyDrawable::DrawablePtr RSFrameGeometryDrawable::Generate(const RSPropertyDrawableGenerateContext& context)
{
    return std::make_unique<RSFrameGeometryDrawable>();
}

void RSColorFilterDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    // if useEffect defined, use color filter from parent EffectView.
#ifndef USE_ROSEN_DRAWING
    SkAutoCanvasRestore acr(&canvas, true);
    canvas.clipRRect(RSPropertiesPainter::RRect2SkRRect(node.GetMutableRenderProperties().GetRRect()), true);
    auto skSurface = canvas.GetSurface();
    if (skSurface == nullptr) {
        ROSEN_LOGE("RSColorFilterDrawable::Draw skSurface is null");
        return;
    }
    auto clipBounds = canvas.getDeviceClipBounds();
    auto imageSnapshot = skSurface->makeImageSnapshot(clipBounds);
    if (imageSnapshot == nullptr) {
        ROSEN_LOGE("RSColorFilterDrawable::Draw image is null");
        return;
    }
    canvas.resetMatrix();
    SkSamplingOptions options(SkFilterMode::kNearest, SkMipmapMode::kNone);
    canvas.drawImageRect(imageSnapshot, SkRect::Make(clipBounds), options, &paint_);
#endif
}

std::unique_ptr<RSPropertyDrawable> RSColorFilterDrawable::Generate(const RSPropertyDrawableGenerateContext& context)
{
    auto& colorFilter = context.properties_.GetColorFilter();
    if (colorFilter == nullptr) {
        return nullptr;
    }
#ifndef USE_ROSEN_DRAWING
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setColorFilter(colorFilter);
    return std::make_unique<RSColorFilterDrawable>(std::move(paint));
#else
    // Drawing need to be adapted furture
    return nullptr;
#endif
}

RSPropertyDrawable::DrawablePtr RSClipFrameDrawable::Generate(const RSPropertyDrawableGenerateContext& context)
{
    // PLANNING: cache frame rect, and update when frame rect changed
    return context.properties_.GetClipToFrame() ? std::make_unique<RSClipFrameDrawable>() : nullptr;
}

void RSClipFrameDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
#ifndef USE_ROSEN_DRAWING
    canvas.clipRect(RSPropertiesPainter::Rect2SkRect(node.GetRenderProperties().GetFrameRect()));
#else
    canvas.ClipRect(RSPropertiesPainter::Rect2DrawingRect(node.GetRenderProperties().GetFrameRect()),
        Drawing::ClipOp::INTERSECT, false);
#endif
}
} // namespace OHOS::Rosen
