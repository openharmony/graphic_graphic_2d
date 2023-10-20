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

#include "platform/common/rs_log.h"
#include "property/rs_properties.h"
#include "property/rs_properties_painter.h"

namespace OHOS::Rosen {
void RSFrameGeometryDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    canvas.translate(frameOffsetX_, frameOffsetY_);
}
void RSFrameGeometryDrawable::OnBoundsChange(const RSProperties& properties)
{
    frameOffsetX_ = properties.GetFrameOffsetX();
    frameOffsetY_ = properties.GetFrameOffsetY();
}
RSPropertyDrawable::DrawablePtr RSFrameGeometryDrawable::Generate(const RSPropertyDrawableGenerateContext& context)
{
    return std::make_unique<RSFrameGeometryDrawable>();
}

void RSColorFilterDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    // if useEffect defined, use color filter from parent EffectView.
#ifndef USE_ROSEN_DRAWING
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
    SkAutoCanvasRestore acr(&canvas, true);
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
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setColorFilter(colorFilter);
    return std::make_unique<RSColorFilterDrawable>(std::move(paint));
}

RSPropertyDrawable::DrawablePtr RSClipFrameDrawable::Generate(const RSPropertyDrawableGenerateContext& context)
{
    return context.properties_.GetClipToFrame()
               ? std::make_unique<RSClipFrameDrawable>(
                     RSPropertiesPainter::Rect2SkRect(context.properties_.GetFrameRect()))
               : nullptr;
}

void RSClipFrameDrawable::Draw(RSRenderNode& node, RSPaintFilterCanvas& canvas)
{
    canvas.clipRect(content_);
}
} // namespace OHOS::Rosen
