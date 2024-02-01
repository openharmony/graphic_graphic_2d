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

#include "property/rs_properties_painter.h"
#include "rs_trace.h"

#include "common/rs_obj_abs_geometry.h"
#include "common/rs_optional_trace.h"
#include "pipeline/rs_effect_render_node.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_root_render_node.h"
#include "platform/common/rs_log.h"
#include "property/rs_point_light_manager.h"
#include "property/rs_properties_def.h"
#include "render/rs_blur_filter.h"
#include "render/rs_skia_filter.h"
#include "render/rs_material_filter.h"
#include "platform/common/rs_system_properties.h"

#ifdef USE_ROSEN_DRAWING
#include <cstdint>
#include <algorithm>

#include "draw/canvas.h"
#include "draw/clip.h"
#include "drawing/draw/core_canvas.h"
#include "effect/runtime_blender_builder.h"
#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"
#include "utils/rect.h"
#else
#include "include/core/SkCanvas.h"
#include "include/core/SkColorFilter.h"
#include "include/core/SkMaskFilter.h"
#include "include/core/SkPaint.h"
#include "include/core/SkPoint3.h"
#include "include/core/SkRRect.h"
#include "include/core/SkSurface.h"
#include "include/effects/Sk1DPathEffect.h"
#include "include/effects/SkDashPathEffect.h"
#include "include/effects/SkGradientShader.h"
#include "include/effects/SkLumaColorFilter.h"
#include "include/utils/SkShadowUtils.h"
#include "include/effects/SkImageFilters.h"
#include "include/effects/SkRuntimeEffect.h"
#endif
#include "src/image/SkImage_Base.h"

namespace OHOS {
namespace Rosen {
namespace {
bool g_forceBgAntiAlias = true;
constexpr int PARAM_DOUBLE = 2;
constexpr float MIN_TRANS_RATIO = 0.0f;
constexpr float MAX_TRANS_RATIO = 0.95f;
constexpr float MIN_SPOT_RATIO = 1.0f;
constexpr float MAX_SPOT_RATIO = 1.95f;
constexpr float MAX_AMBIENT_RADIUS = 150.0f;
// when the blur radius > SNAPSHOT_OUTSET_BLUR_RADIUS_THRESHOLD,
// the snapshot should call outset before blur to shrink by 1px.
constexpr static float SNAPSHOT_OUTSET_BLUR_RADIUS_THRESHOLD = 40.0f;
} // namespace

const bool RSPropertiesPainter::BLUR_ENABLED = RSSystemProperties::GetBlurEnabled();

#ifndef USE_ROSEN_DRAWING
#else
std::shared_ptr<Drawing::RuntimeEffect> RSPropertiesPainter::greyAdjustEffect_ = nullptr;
std::shared_ptr<Drawing::RuntimeEffect> RSPropertiesPainter::binarizationShaderEffect_ = nullptr;
std::shared_ptr<Drawing::RuntimeEffect> RSPropertiesPainter::lightUpEffectShaderEffect_ = nullptr;
std::shared_ptr<Drawing::RuntimeEffect> RSPropertiesPainter::dynamicLightUpBlenderEffect_ = nullptr;
#endif

#ifndef USE_ROSEN_DRAWING
SkRect RSPropertiesPainter::Rect2SkRect(const RectF& r)
{
    return SkRect::MakeXYWH(r.left_, r.top_, r.width_, r.height_);
}
#else
Drawing::Rect RSPropertiesPainter::Rect2DrawingRect(const RectF& r)
{
    return Drawing::Rect(r.left_, r.top_, r.left_ + r.width_, r.top_ + r.height_);
}
#endif

#ifndef USE_ROSEN_DRAWING
SkRRect RSPropertiesPainter::RRect2SkRRect(const RRect& rr)
{
    SkRect rect = SkRect::MakeXYWH(rr.rect_.left_, rr.rect_.top_, rr.rect_.width_, rr.rect_.height_);
    SkRRect rrect = SkRRect::MakeEmpty();

    // set radius for all 4 corner of RRect
    constexpr uint32_t NUM_OF_CORNERS_IN_RECT = 4;
    SkVector vec[NUM_OF_CORNERS_IN_RECT];
    for (uint32_t i = 0; i < NUM_OF_CORNERS_IN_RECT; i++) {
        vec[i].set(rr.radius_[i].x_, rr.radius_[i].y_);
    }

    rrect.setRectRadii(rect, vec);
    return rrect;
}
#else
Drawing::RoundRect RSPropertiesPainter::RRect2DrawingRRect(const RRect& rr)
{
    Drawing::Rect rect = Drawing::Rect(
        rr.rect_.left_, rr.rect_.top_, rr.rect_.left_ + rr.rect_.width_, rr.rect_.top_ + rr.rect_.height_);

    // set radius for all 4 corner of RRect
    constexpr uint32_t NUM_OF_CORNERS_IN_RECT = 4;
    std::vector<Drawing::Point> radii(NUM_OF_CORNERS_IN_RECT);
    for (uint32_t i = 0; i < NUM_OF_CORNERS_IN_RECT; i++) {
        radii.at(i).SetX(rr.radius_[i].x_);
        radii.at(i).SetY(rr.radius_[i].y_);
    }
    return Drawing::RoundRect(rect, radii);
}
#endif

#ifndef USE_ROSEN_DRAWING
bool RSPropertiesPainter::GetGravityMatrix(Gravity gravity, RectF rect, float w, float h, SkMatrix& mat)
#else
bool RSPropertiesPainter::GetGravityMatrix(Gravity gravity, RectF rect, float w, float h, Drawing::Matrix& mat)
#endif
{
    if (w == rect.width_ && h == rect.height_) {
        return false;
    }
#ifndef USE_ROSEN_DRAWING
    mat.reset();
#else
    mat = Drawing::Matrix();
#endif

    switch (gravity) {
        case Gravity::CENTER: {
#ifndef USE_ROSEN_DRAWING
            mat.preTranslate((rect.width_ - w) / PARAM_DOUBLE, (rect.height_ - h) / PARAM_DOUBLE);
#else
            mat.PreTranslate((rect.width_ - w) / PARAM_DOUBLE, (rect.height_ - h) / PARAM_DOUBLE);
#endif
            return true;
        }
        case Gravity::TOP: {
#ifndef USE_ROSEN_DRAWING
            mat.preTranslate((rect.width_ - w) / PARAM_DOUBLE, 0);
#else
            mat.PreTranslate((rect.width_ - w) / PARAM_DOUBLE, 0);
#endif
            return true;
        }
        case Gravity::BOTTOM: {
#ifndef USE_ROSEN_DRAWING
            mat.preTranslate((rect.width_ - w) / PARAM_DOUBLE, rect.height_ - h);
#else
            mat.PreTranslate((rect.width_ - w) / PARAM_DOUBLE, rect.height_ - h);
#endif
            return true;
        }
        case Gravity::LEFT: {
#ifndef USE_ROSEN_DRAWING
            mat.preTranslate(0, (rect.height_ - h) / PARAM_DOUBLE);
#else
            mat.PreTranslate(0, (rect.height_ - h) / PARAM_DOUBLE);
#endif
            return true;
        }
        case Gravity::RIGHT: {
#ifndef USE_ROSEN_DRAWING
            mat.preTranslate(rect.width_ - w, (rect.height_ - h) / PARAM_DOUBLE);
#else
            mat.PreTranslate(rect.width_ - w, (rect.height_ - h) / PARAM_DOUBLE);
#endif
            return true;
        }
        case Gravity::TOP_LEFT: {
            return false;
        }
        case Gravity::TOP_RIGHT: {
#ifndef USE_ROSEN_DRAWING
            mat.preTranslate(rect.width_ - w, 0);
#else
            mat.PreTranslate(rect.width_ - w, 0);
#endif
            return true;
        }
        case Gravity::BOTTOM_LEFT: {
#ifndef USE_ROSEN_DRAWING
            mat.preTranslate(0, rect.height_ - h);
#else
            mat.PreTranslate(0, rect.height_ - h);
#endif
            return true;
        }
        case Gravity::BOTTOM_RIGHT: {
#ifndef USE_ROSEN_DRAWING
            mat.preTranslate(rect.width_ - w, rect.height_ - h);
#else
            mat.PreTranslate(rect.width_ - w, rect.height_ - h);
#endif
            return true;
        }
        case Gravity::RESIZE: {
            if (ROSEN_EQ(w, 0.f) || ROSEN_EQ(h, 0.f)) {
                return false;
            }
#ifndef USE_ROSEN_DRAWING
            mat.preScale(rect.width_ / w, rect.height_ / h);
#else
            mat.PreScale(rect.width_ / w, rect.height_ / h);
#endif
            return true;
        }
        case Gravity::RESIZE_ASPECT: {
            if (ROSEN_EQ(w, 0.f) || ROSEN_EQ(h, 0.f)) {
                return false;
            }
            float scale = std::min(rect.width_ / w, rect.height_ / h);
            if (ROSEN_EQ(scale, 0.f)) {
                return false;
            }
#ifndef USE_ROSEN_DRAWING
            mat.preScale(scale, scale);
            mat.preTranslate((rect.width_ / scale - w) / PARAM_DOUBLE, (rect.height_ / scale - h) / PARAM_DOUBLE);
#else
            mat.PreScale(scale, scale);
            mat.PreTranslate((rect.width_ / scale - w) / PARAM_DOUBLE, (rect.height_ / scale - h) / PARAM_DOUBLE);
#endif
            return true;
        }
        case Gravity::RESIZE_ASPECT_TOP_LEFT: {
            if (ROSEN_EQ(w, 0.f) || ROSEN_EQ(h, 0.f)) {
                return false;
            }
            float scale = std::min(rect.width_ / w, rect.height_ / h);
#ifndef USE_ROSEN_DRAWING
            mat.preScale(scale, scale);
#else
            mat.PreScale(scale, scale);
#endif
            return true;
        }
        case Gravity::RESIZE_ASPECT_BOTTOM_RIGHT: {
            if (ROSEN_EQ(w, 0.f) || ROSEN_EQ(h, 0.f)) {
                return false;
            }
            float scale = std::min(rect.width_ / w, rect.height_ / h);
            if (ROSEN_EQ(scale, 0.f)) {
                return false;
            }
#ifndef USE_ROSEN_DRAWING
            mat.preScale(scale, scale);
            mat.preTranslate(rect.width_ / scale - w, rect.height_ / scale - h);
#else
            mat.PreScale(scale, scale);
            mat.PreTranslate(rect.width_ / scale - w, rect.height_ / scale - h);
#endif
            return true;
        }
        case Gravity::RESIZE_ASPECT_FILL: {
            if (ROSEN_EQ(w, 0.f) || ROSEN_EQ(h, 0.f)) {
                return false;
            }
            float scale = std::max(rect.width_ / w, rect.height_ / h);
            if (ROSEN_EQ(scale, 0.f)) {
                return false;
            }
#ifndef USE_ROSEN_DRAWING
            mat.preScale(scale, scale);
            mat.preTranslate((rect.width_ / scale - w) / PARAM_DOUBLE, (rect.height_ / scale - h) / PARAM_DOUBLE);
#else
            mat.PreScale(scale, scale);
            mat.PreTranslate((rect.width_ / scale - w) / PARAM_DOUBLE, (rect.height_ / scale - h) / PARAM_DOUBLE);
#endif
            return true;
        }
        case Gravity::RESIZE_ASPECT_FILL_TOP_LEFT: {
            if (ROSEN_EQ(w, 0.f) || ROSEN_EQ(h, 0.f)) {
                return false;
            }
            float scale = std::max(rect.width_ / w, rect.height_ / h);
#ifndef USE_ROSEN_DRAWING
            mat.preScale(scale, scale);
#else
            mat.PreScale(scale, scale);
#endif
            return true;
        }
        case Gravity::RESIZE_ASPECT_FILL_BOTTOM_RIGHT: {
            if (ROSEN_EQ(w, 0.f) || ROSEN_EQ(h, 0.f)) {
                return false;
            }
            float scale = std::max(rect.width_ / w, rect.height_ / h);
            if (ROSEN_EQ(scale, 0.f)) {
                return false;
            }
#ifndef USE_ROSEN_DRAWING
            mat.preScale(scale, scale);
            mat.preTranslate(rect.width_ / scale - w, rect.height_ / scale - h);
#else
            mat.PreScale(scale, scale);
            mat.PreTranslate(rect.width_ / scale - w, rect.height_ / scale - h);
#endif
            return true;
        }
        default: {
            ROSEN_LOGE("GetGravityMatrix unknow gravity=[%{public}d]", gravity);
            return false;
        }
    }
}

#ifndef USE_ROSEN_DRAWING
void RSPropertiesPainter::Clip(SkCanvas& canvas, RectF rect, bool isAntiAlias)
{
    // isAntiAlias is false only the method is called in ProcessAnimatePropertyBeforeChildren().
    canvas.clipRect(Rect2SkRect(rect), isAntiAlias);
}
#else
void RSPropertiesPainter::Clip(Drawing::Canvas& canvas, RectF rect, bool isAntiAlias)
{
    canvas.ClipRect(Rect2DrawingRect(rect), Drawing::ClipOp::INTERSECT, isAntiAlias);
}
#endif

#ifndef USE_ROSEN_DRAWING
void RSPropertiesPainter::GetShadowDirtyRect(RectI& dirtyShadow, const RSProperties& properties,
    const RRect* rrect, bool isAbsCoordinate, bool radiusInclude)
{
    // [Planning]: After Skia being updated, we should directly call SkShadowUtils::GetLocalBounds here.
    if (!properties.IsShadowValid()) {
        return;
    }
    SkPath skPath;
    if (properties.GetShadowPath() && !properties.GetShadowPath()->GetSkiaPath().isEmpty()) {
        skPath = properties.GetShadowPath()->GetSkiaPath();
    } else if (properties.GetClipBounds()) {
        skPath = properties.GetClipBounds()->GetSkiaPath();
    } else {
        if (rrect != nullptr) {
            skPath.addRRect(RRect2SkRRect(*rrect));
        } else {
            skPath.addRRect(RRect2SkRRect(properties.GetRRect()));
        }
    }
    skPath.offset(properties.GetShadowOffsetX(), properties.GetShadowOffsetY());

    SkRect shadowRect = skPath.getBounds();
    if (properties.GetShadowElevation() > 0.f) {
        float elevation = properties.GetShadowElevation() + DEFAULT_TRANSLATION_Z;

        float userTransRatio =
            (elevation != DEFAULT_LIGHT_HEIGHT) ? elevation / (DEFAULT_LIGHT_HEIGHT - elevation) : MAX_TRANS_RATIO;
        float transRatio = std::max(MIN_TRANS_RATIO, std::min(userTransRatio, MAX_TRANS_RATIO));

        float userSpotRatio = (elevation != DEFAULT_LIGHT_HEIGHT)
                                  ? DEFAULT_LIGHT_HEIGHT / (DEFAULT_LIGHT_HEIGHT - elevation)
                                  : MAX_SPOT_RATIO;
        float spotRatio = std::max(MIN_SPOT_RATIO, std::min(userSpotRatio, MAX_SPOT_RATIO));

        SkRect ambientRect = skPath.getBounds();
        SkRect spotRect = SkRect::MakeLTRB(ambientRect.left() * spotRatio, ambientRect.top() * spotRatio,
            ambientRect.right() * spotRatio, ambientRect.bottom() * spotRatio);
        spotRect.offset(-transRatio * DEFAULT_LIGHT_POSITION_X, -transRatio * DEFAULT_LIGHT_POSITION_Y);
        spotRect.outset(transRatio * DEFAULT_LIGHT_RADIUS, transRatio * DEFAULT_LIGHT_RADIUS);

        shadowRect = ambientRect;
        float ambientBlur = std::min(elevation * 0.5f, MAX_AMBIENT_RADIUS);
        shadowRect.outset(ambientBlur, ambientBlur);
        shadowRect.join(spotRect);
        shadowRect.outset(1, 1);
    } else {
        SkPaint paint;
        paint.setAntiAlias(true);
        paint.setMaskFilter(SkMaskFilter::MakeBlur(kNormal_SkBlurStyle, properties.GetShadowRadius()));
        if (paint.canComputeFastBounds() && radiusInclude) {
            paint.computeFastBounds(shadowRect, &shadowRect);
        }
    }

    auto geoPtr = (properties.GetBoundsGeometry());
    SkMatrix matrix = (geoPtr && isAbsCoordinate) ? geoPtr->GetAbsMatrix() : SkMatrix::I();
    matrix.mapRect(&shadowRect);

    dirtyShadow.left_ = shadowRect.left();
    dirtyShadow.top_ = shadowRect.top();
    dirtyShadow.width_ = shadowRect.width();
    dirtyShadow.height_ = shadowRect.height();
}
#else
void RSPropertiesPainter::GetShadowDirtyRect(RectI& dirtyShadow, const RSProperties& properties,
    const RRect* rrect, bool isAbsCoordinate, bool radiusInclude)
{
    // [Planning]: After Skia being updated, we should directly call SkShadowUtils::GetLocalBounds here.
    if (!properties.IsShadowValid()) {
        return;
    }
    Drawing::Path path;
    if (properties.GetShadowPath() && properties.GetShadowPath()->GetDrawingPath().IsValid()) {
        path = properties.GetShadowPath()->GetDrawingPath();
    } else if (properties.GetClipBounds()) {
        path = properties.GetClipBounds()->GetDrawingPath();
    } else {
        if (rrect != nullptr) {
            path.AddRoundRect(RRect2DrawingRRect(*rrect));
        } else {
            path.AddRoundRect(RRect2DrawingRRect(properties.GetRRect()));
        }
    }
    path.Offset(properties.GetShadowOffsetX(), properties.GetShadowOffsetY());

    Drawing::Rect shadowRect = path.GetBounds();
    if (properties.GetShadowElevation() > 0.f) {
        float elevation = properties.GetShadowElevation() + DEFAULT_TRANSLATION_Z;

        float userTransRatio =
            (elevation != DEFAULT_LIGHT_HEIGHT) ? elevation / (DEFAULT_LIGHT_HEIGHT - elevation) : MAX_TRANS_RATIO;
        float transRatio = std::max(MIN_TRANS_RATIO, std::min(userTransRatio, MAX_TRANS_RATIO));

        float userSpotRatio = (elevation != DEFAULT_LIGHT_HEIGHT)
                                  ? DEFAULT_LIGHT_HEIGHT / (DEFAULT_LIGHT_HEIGHT - elevation)
                                  : MAX_SPOT_RATIO;
        float spotRatio = std::max(MIN_SPOT_RATIO, std::min(userSpotRatio, MAX_SPOT_RATIO));

        Drawing::Rect ambientRect = path.GetBounds();
        Drawing::Rect spotRect = Drawing::Rect(ambientRect.GetLeft() * spotRatio, ambientRect.GetTop() * spotRatio,
            ambientRect.GetRight() * spotRatio, ambientRect.GetBottom() * spotRatio);
        spotRect.Offset(-transRatio * DEFAULT_LIGHT_POSITION_X, -transRatio * DEFAULT_LIGHT_POSITION_Y);
        spotRect.MakeOutset(transRatio * DEFAULT_LIGHT_RADIUS, transRatio * DEFAULT_LIGHT_RADIUS);

        shadowRect = ambientRect;
        float ambientBlur = std::min(elevation * 0.5f, MAX_AMBIENT_RADIUS);
        shadowRect.MakeOutset(ambientBlur, ambientBlur);

        shadowRect.Join(spotRect);
        shadowRect.MakeOutset(1, 1);
    } else {
        Drawing::Brush brush;
        brush.SetAntiAlias(true);
        Drawing::Filter filter;
        filter.SetMaskFilter(
            Drawing::MaskFilter::CreateBlurMaskFilter(Drawing::BlurType::NORMAL, properties.GetShadowRadius()));
        brush.SetFilter(filter);
        if (brush.CanComputeFastBounds() && radiusInclude) {
            brush.ComputeFastBounds(shadowRect, &shadowRect);
        }
    }

    auto geoPtr = (properties.GetBoundsGeometry());
    Drawing::Matrix matrix = (geoPtr && isAbsCoordinate) ? geoPtr->GetAbsMatrix() : Drawing::Matrix();
    matrix.MapRect(shadowRect, shadowRect);

    dirtyShadow.left_ = shadowRect.GetLeft();
    dirtyShadow.top_ = shadowRect.GetTop();
    dirtyShadow.width_ = shadowRect.GetWidth();
    dirtyShadow.height_ = shadowRect.GetHeight();
}
#endif

#ifndef USE_ROSEN_DRAWING
void RSPropertiesPainter::DrawShadow(const RSProperties& properties, RSPaintFilterCanvas& canvas, const RRect* rrect)
{
    // skip shadow if not valid or cache is enabled
    if (properties.IsSpherizeValid() || !properties.IsShadowValid() ||
        canvas.GetCacheType() == RSPaintFilterCanvas::CacheType::ENABLED) {
        return;
    }

    RSAutoCanvasRestore acr(&canvas);
    SkPath skPath;
    if (properties.GetShadowPath() && !properties.GetShadowPath()->GetSkiaPath().isEmpty()) {
        skPath = properties.GetShadowPath()->GetSkiaPath();
        if (!properties.GetShadowIsFilled()) {
            canvas.clipPath(skPath, SkClipOp::kDifference, true);
        }
    } else if (properties.GetClipBounds()) {
        skPath = properties.GetClipBounds()->GetSkiaPath();
        if (!properties.GetShadowIsFilled()) {
            canvas.clipPath(skPath, SkClipOp::kDifference, true);
        }
    } else {
        if (rrect != nullptr) {
            skPath.addRRect(RRect2SkRRect(*rrect));
            if (!properties.GetShadowIsFilled()) {
                canvas.clipRRect(RRect2SkRRect(*rrect), SkClipOp::kDifference, true);
            }
        } else {
            skPath.addRRect(RRect2SkRRect(properties.GetRRect()));
            if (!properties.GetShadowIsFilled()) {
                canvas.clipRRect(RRect2SkRRect(properties.GetRRect()), SkClipOp::kDifference, true);
            }
        }
    }
    if (properties.GetShadowMask()) {
        DrawColorfulShadowInner(properties, canvas, skPath);
    } else {
        DrawShadowInner(properties, canvas, skPath);
    }
}
#else
void RSPropertiesPainter::DrawShadow(const RSProperties& properties, RSPaintFilterCanvas& canvas, const RRect* rrect)
{
    // skip shadow if not valid or cache is enabled
    if (properties.IsSpherizeValid() || !properties.IsShadowValid() ||
        canvas.GetCacheType() == RSPaintFilterCanvas::CacheType::ENABLED) {
        return;
    }
    Drawing::AutoCanvasRestore acr(canvas, true);
    Drawing::Path path;
    if (properties.GetShadowPath() && properties.GetShadowPath()->GetDrawingPath().IsValid()) {
        path = properties.GetShadowPath()->GetDrawingPath();
        if (!properties.GetShadowIsFilled()) {
            canvas.ClipPath(path, Drawing::ClipOp::DIFFERENCE, true);
        }
    } else if (properties.GetClipBounds()) {
        path = properties.GetClipBounds()->GetDrawingPath();
        if (!properties.GetShadowIsFilled()) {
            canvas.ClipPath(path, Drawing::ClipOp::DIFFERENCE, true);
        }
    } else {
        if (rrect != nullptr) {
            path.AddRoundRect(RRect2DrawingRRect(*rrect));
            if (!properties.GetShadowIsFilled()) {
                canvas.ClipRoundRect(RRect2DrawingRRect(*rrect), Drawing::ClipOp::DIFFERENCE, true);
            }
        } else {
            path.AddRoundRect(RRect2DrawingRRect(properties.GetRRect()));
            if (!properties.GetShadowIsFilled()) {
                canvas.ClipRoundRect(RRect2DrawingRRect(properties.GetRRect()), Drawing::ClipOp::DIFFERENCE, true);
            }
        }
    }
    if (properties.GetShadowMask()) {
        DrawColorfulShadowInner(properties, canvas, path);
    } else {
        DrawShadowInner(properties, canvas, path);
    }
}
#endif

#ifndef USE_ROSEN_DRAWING
void RSPropertiesPainter::DrawColorfulShadowInner(
    const RSProperties& properties, RSPaintFilterCanvas& canvas, SkPath& skPath)
{
    // blurRadius calculation is based on the formula in SkShadowUtils::DrawShadow, 0.25f and 128.0f are constants
    const SkScalar blurRadius =
        properties.GetShadowElevation() > 0.f
            ? 0.25f * properties.GetShadowElevation() * (1 + properties.GetShadowElevation() / 128.0f)
            : properties.GetShadowRadius();

    // save layer, draw image with clipPath, blur and draw back
    SkPaint blurPaint;
    blurPaint.setImageFilter(SkImageFilters::Blur(blurRadius, blurRadius, SkTileMode::kDecal, nullptr));
    canvas.saveLayer(nullptr, &blurPaint);

    canvas.translate(properties.GetShadowOffsetX(), properties.GetShadowOffsetY());

    canvas.clipPath(skPath);
    // draw node content as shadow
    // [PLANNING]: maybe we should also draw background color / image here, and we should cache the shadow image
    if (auto node = RSBaseRenderNode::ReinterpretCast<RSCanvasRenderNode>(properties.backref_.lock())) {
        node->InternalDrawContent(canvas);
    }
}
#else
void RSPropertiesPainter::DrawColorfulShadowInner(
    const RSProperties& properties, RSPaintFilterCanvas& canvas, Drawing::Path& path)
{
    // blurRadius calculation is based on the formula in Canvas::DrawShadow, 0.25f and 128.0f are constants
    const Drawing::scalar blurRadius =
        properties.GetShadowElevation() > 0.f
            ? 0.25f * properties.GetShadowElevation() * (1 + properties.GetShadowElevation() / 128.0f)
            : properties.GetShadowRadius();

    // save layer, draw image with clipPath, blur and draw back
    Drawing::Brush blurBrush;
    Drawing::Filter filter;
    filter.SetImageFilter(Drawing::ImageFilter::CreateBlurImageFilter(
        blurRadius, blurRadius, Drawing::TileMode::DECAL, nullptr));
    blurBrush.SetFilter(filter);

    canvas.SaveLayer({nullptr, &blurBrush});

    canvas.Translate(properties.GetShadowOffsetX(), properties.GetShadowOffsetY());

    canvas.ClipPath(path, Drawing::ClipOp::INTERSECT, false);
    // draw node content as shadow
    // [PLANNING]: maybe we should also draw background color / image here, and we should cache the shadow image
    if (auto node = RSBaseRenderNode::ReinterpretCast<RSCanvasRenderNode>(properties.backref_.lock())) {
        node->InternalDrawContent(canvas);
    }
}
#endif

void RSPropertiesPainter::GetDarkColor(RSColor& color)
{
    // convert to lab
    float minColorRange = 0;
    float maxColorRange = 255;
    float R = float(color.GetRed()) / maxColorRange;
    float G = float(color.GetGreen()) / maxColorRange;
    float B = float(color.GetBlue()) / maxColorRange;

    float X = 0.4124 * R + 0.3576 * G + 0.1805 * B;
    float Y = 0.2126 * R + 0.7152 * G + 0.0722 * B;
    float Z = 0.0193 * R + 0.1192 * G + 0.9505 * B;

    float Xn = 0.9505;
    float Yn = 1.0000;
    float Zn = 1.0889999;
    float Fx = (X / Xn) > 0.008856 ? pow((X / Xn), 1.0 / 3) : (7.787 * (X / Xn) + 16.0 / 116);
    float Fy = (Y / Yn) > 0.008856 ? pow((Y / Yn), 1.0 / 3) : (7.787 * (Y / Yn) + 16.0 / 116);
    float Fz = (Z / Zn) > 0.008856 ? pow((Z / Zn), 1.0 / 3) : (7.787 * (Z / Zn) + 16.0 / 116);
    float L = 116 * Fy - 16;
    float a = 500 * (Fx - Fy);
    float b = 200 * (Fy - Fz);

    float standardLightness = 75.0;
    if (L > standardLightness) {
        float L1 = standardLightness;
        float xw = 0.9505;
        float yw = 1.0000;
        float zw = 1.0889999;

        float fy = (L1 + 16) / 116;
        float fx = fy + (a / 500);
        float fz = fy - (b / 200);

        float X1 = xw * ((pow(fx, 3) > 0.008856) ? pow(fx, 3) : ((fx - 16.0 / 116) / 7.787));
        float Y1 = yw * ((pow(fy, 3) > 0.008856) ? pow(fy, 3) : ((fy - 16.0 / 116) / 7.787));
        float Z1 = zw * ((pow(fz, 3) > 0.008856) ? pow(fz, 3) : ((fz - 16.0 / 116) / 7.787));

        float DarkR = 3.2406 * X1 - 1.5372 * Y1 - 0.4986 * Z1;
        float DarkG = -0.9689 * X1 + 1.8758 * Y1 + 0.0415 * Z1;
        float DarkB = 0.0557 * X1 - 0.2040 * Y1 + 1.0570 * Z1;

        DarkR = std::clamp(maxColorRange * DarkR, minColorRange, maxColorRange);
        DarkG = std::clamp(maxColorRange * DarkG, minColorRange, maxColorRange);
        DarkB = std::clamp(maxColorRange * DarkB, minColorRange, maxColorRange);

        color = RSColor(DarkR, DarkG, DarkB, color.GetAlpha());
    }
}

#ifndef USE_ROSEN_DRAWING
bool RSPropertiesPainter::PickColor(const RSProperties& properties, RSPaintFilterCanvas& canvas, SkPath& skPath,
    SkMatrix& matrix, SkIRect& deviceClipBounds, RSColor& colorPicked)
{
    SkRect clipBounds = skPath.getBounds();
    SkIRect clipIBounds = clipBounds.roundIn();
    SkSurface* skSurface = canvas.GetSurface();
    if (skSurface == nullptr) {
        return false;
    }

    auto& colorPickerTask = properties.GetColorPickerCacheTaskShadow();
    colorPickerTask->SetIsShadow(true);
    int deviceWidth = 0;
    int deviceHeight = 0;
    int deviceClipBoundsW = skSurface->width();
    int deviceClipBoundsH = skSurface->height();
    if (!colorPickerTask->GetDeviceSize(deviceWidth, deviceHeight)) {
        colorPickerTask->SetDeviceSize(deviceClipBoundsW, deviceClipBoundsH);
        deviceWidth = deviceClipBoundsW;
        deviceHeight = deviceClipBoundsH;
    }
    int32_t fLeft = std::clamp(int(matrix.getTranslateX()), 0, deviceWidth - 1);
    int32_t fTop = std::clamp(int(matrix.getTranslateY()), 0, deviceHeight - 1);
    int32_t fRight = std::clamp(int(fLeft + clipIBounds.width()), 0, deviceWidth - 1);
    int32_t fBottom = std::clamp(int(fTop + clipIBounds.height()), 0, deviceHeight - 1);
    if (fLeft == fRight || fTop == fBottom) {
        return false;
    }

    SkIRect regionBounds = SkIRect::MakeLTRB(fLeft, fTop, fRight, fBottom);
    sk_sp<SkImage> shadowRegionImage = skSurface->makeImageSnapshot(regionBounds);

    if (shadowRegionImage == nullptr) {
        return false;
    }

    // when color picker task resource is waitting for release, use color picked last frame
    if (colorPickerTask->GetWaitRelease()) {
        colorPickerTask->GetColorAverage(colorPicked);
        return true;
    }

    if (RSColorPickerCacheTask::PostPartialColorPickerTask(colorPickerTask, shadowRegionImage)
        && colorPickerTask->GetColor(colorPicked)) {
        colorPickerTask->GetColorAverage(colorPicked);
        colorPickerTask->SetStatus(CacheProcessStatus::WAITING);
        return true;
    }
    colorPickerTask->GetColorAverage(colorPicked);
    return true;
}
#else
bool RSPropertiesPainter::PickColor(const RSProperties& properties, RSPaintFilterCanvas& canvas, Drawing::Path& drPath,
    Drawing::Matrix& matrix, Drawing::RectI& deviceClipBounds, RSColor& colorPicked)
{
    Drawing::Rect clipBounds = drPath.GetBounds();
    Drawing::RectI clipIBounds = { static_cast<int>(clipBounds.GetLeft()), static_cast<int>(clipBounds.GetTop()),
        static_cast<int>(clipBounds.GetRight()), static_cast<int>(clipBounds.GetBottom()) };
    Drawing::Surface* drSurface = canvas.GetSurface();
    if (drSurface == nullptr) {
        return false;
    }

    auto& colorPickerTask = properties.GetColorPickerCacheTaskShadow();
    if (!colorPickerTask) {
        ROSEN_LOGE("RSPropertiesPainter::PickColor colorPickerTask is null");
        return false;
    }
    colorPickerTask->SetIsShadow(true);
    int deviceWidth = 0;
    int deviceHeight = 0;
    int deviceClipBoundsW = drSurface->Width();
    int deviceClipBoundsH = drSurface->Height();
    if (!colorPickerTask->GetDeviceSize(deviceWidth, deviceHeight)) {
        colorPickerTask->SetDeviceSize(deviceClipBoundsW, deviceClipBoundsH);
        deviceWidth = deviceClipBoundsW;
        deviceHeight = deviceClipBoundsH;
    }
    int32_t fLeft = std::clamp(int(matrix.Get(Drawing::Matrix::Index::TRANS_X)), 0, deviceWidth - 1);
    int32_t fTop = std::clamp(int(matrix.Get(Drawing::Matrix::Index::TRANS_Y)), 0, deviceHeight - 1);
    int32_t fRight = std::clamp(int(fLeft + clipIBounds.GetWidth()), 0, deviceWidth - 1);
    int32_t fBottom = std::clamp(int(fTop + clipIBounds.GetHeight()), 0, deviceHeight - 1);
    if (fLeft == fRight || fTop == fBottom) {
        return false;
    }

    Drawing::RectI regionBounds = { fLeft, fTop, fRight, fBottom };
    std::shared_ptr<Drawing::Image> shadowRegionImage = drSurface->GetImageSnapshot(regionBounds);

    if (shadowRegionImage == nullptr) {
        return false;
    }

    // when color picker task resource is waitting for release, use color picked last frame
    if (colorPickerTask->GetWaitRelease()) {
        colorPickerTask->GetColorAverage(colorPicked);
        return true;
    }

    if (RSColorPickerCacheTask::PostPartialColorPickerTask(colorPickerTask, shadowRegionImage)
        && colorPickerTask->GetColor(colorPicked)) {
        colorPickerTask->GetColorAverage(colorPicked);
        colorPickerTask->SetStatus(CacheProcessStatus::WAITING);
        return true;
    }
    colorPickerTask->GetColorAverage(colorPicked);
    return true;
}
#endif

#ifndef USE_ROSEN_DRAWING
void RSPropertiesPainter::DrawShadowInner(const RSProperties& properties, RSPaintFilterCanvas& canvas, SkPath& skPath)
{
    skPath.offset(properties.GetShadowOffsetX(), properties.GetShadowOffsetY());
    Color spotColor = properties.GetShadowColor();
    // shadow alpha follow setting
    auto shadowAlpha = spotColor.GetAlpha();
    auto deviceClipBounds = canvas.getDeviceClipBounds();

    // The translation of the matrix is rounded to improve the hit ratio of skia blurfilter cache,
    // the function <compute_key_and_clip_bounds> in <skia/src/gpu/GrBlurUtil.cpp> for more details.
    RSAutoCanvasRestore rst(&canvas);
    auto matrix = canvas.getTotalMatrix();
    matrix.setTranslateX(std::ceil(matrix.getTranslateX()));
    matrix.setTranslateY(std::ceil(matrix.getTranslateY()));
    canvas.setMatrix(matrix);

    RSColor colorPicked;
    auto& colorPickerTask = properties.GetColorPickerCacheTaskShadow();
    if (colorPickerTask != nullptr &&
        properties.GetShadowColorStrategy() != SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_NONE) {
        if (PickColor(properties, canvas, skPath, matrix, deviceClipBounds, colorPicked)) {
            GetDarkColor(colorPicked);
        } else {
            shadowAlpha = 0;
        }
        if (!colorPickerTask->GetFirstGetColorFinished()) {
            shadowAlpha = 0;
        }
    } else {
        shadowAlpha = spotColor.GetAlpha();
        colorPicked = spotColor;
    }

    if (properties.GetShadowElevation() > 0.f) {
        SkPoint3 planeParams = { 0.0f, 0.0f, properties.GetShadowElevation() };
        SkPoint pt = { skPath.getBounds().centerX(), skPath.getBounds().centerY() };
        canvas.getTotalMatrix().mapPoints(&pt, 1);
        SkPoint3 lightPos = {pt.fX, pt.fY, DEFAULT_LIGHT_HEIGHT};
        Color ambientColor = Color::FromArgbInt(DEFAULT_AMBIENT_COLOR);
        ambientColor.MultiplyAlpha(canvas.GetAlpha());
        spotColor.MultiplyAlpha(canvas.GetAlpha());
        SkShadowUtils::DrawShadow(&canvas, skPath, planeParams, lightPos, DEFAULT_LIGHT_RADIUS,
            ambientColor.AsArgbInt(), spotColor.AsArgbInt(), SkShadowFlags::kTransparentOccluder_ShadowFlag);
    } else {
        SkPaint paint;
        paint.setColor(SkColorSetARGB(shadowAlpha, colorPicked.GetRed(),
            colorPicked.GetGreen(), colorPicked.GetBlue()));
        paint.setAntiAlias(true);
        paint.setMaskFilter(SkMaskFilter::MakeBlur(kNormal_SkBlurStyle, properties.GetShadowRadius()));
        canvas.drawPath(skPath, paint);
    }
}
#else
void RSPropertiesPainter::DrawShadowInner(
    const RSProperties& properties, RSPaintFilterCanvas& canvas, Drawing::Path& path)
{
    path.Offset(properties.GetShadowOffsetX(), properties.GetShadowOffsetY());
    Color spotColor = properties.GetShadowColor();
    // shadow alpha follow setting
    auto shadowAlpha = spotColor.GetAlpha();
    auto deviceClipBounds = canvas.GetDeviceClipBounds();

    // The translation of the matrix is rounded to improve the hit ratio of skia blurfilter cache,
    // the function <compute_key_and_clip_bounds> in <skia/src/gpu/GrBlurUtil.cpp> for more details.
    RSAutoCanvasRestore rst(&canvas);
    auto matrix = canvas.GetTotalMatrix();
    matrix.Set(Drawing::Matrix::TRANS_X, std::ceil(matrix.Get(Drawing::Matrix::TRANS_X)));
    matrix.Set(Drawing::Matrix::TRANS_Y, std::ceil(matrix.Get(Drawing::Matrix::TRANS_Y)));
    canvas.SetMatrix(matrix);

    RSColor colorPicked;
    auto& colorPickerTask = properties.GetColorPickerCacheTaskShadow();
    if (colorPickerTask != nullptr &&
        properties.GetShadowColorStrategy() != SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_NONE) {
        if (PickColor(properties, canvas, path, matrix, deviceClipBounds, colorPicked)) {
            GetDarkColor(colorPicked);
        } else {
            shadowAlpha = 0;
        }
        if (!colorPickerTask->GetFirstGetColorFinished()) {
            shadowAlpha = 0;
        }
    } else {
        shadowAlpha = spotColor.GetAlpha();
        colorPicked = spotColor;
    }

    if (properties.GetShadowElevation() > 0.f) {
        Drawing::Point3 planeParams = { 0.0f, 0.0f, properties.GetShadowElevation() };
        std::vector<Drawing::Point> pt{{path.GetBounds().GetLeft() + path.GetBounds().GetWidth() / 2,
            path.GetBounds().GetTop() + path.GetBounds().GetHeight() / 2}};
        canvas.GetTotalMatrix().MapPoints(pt, pt, 1);
        Drawing::Point3 lightPos = {pt[0].GetX(), pt[0].GetY(), DEFAULT_LIGHT_HEIGHT};
        Color ambientColor = Color::FromArgbInt(DEFAULT_AMBIENT_COLOR);
        ambientColor.MultiplyAlpha(canvas.GetAlpha());
        spotColor.MultiplyAlpha(canvas.GetAlpha());
        canvas.DrawShadow(path, planeParams, lightPos, DEFAULT_LIGHT_RADIUS,
            Drawing::Color(ambientColor.AsArgbInt()), Drawing::Color(spotColor.AsArgbInt()),
            Drawing::ShadowFlags::TRANSPARENT_OCCLUDER);
    } else {
        Drawing::Brush brush;
        brush.SetColor(Drawing::Color::ColorQuadSetARGB(
            shadowAlpha, colorPicked.GetRed(), colorPicked.GetGreen(), colorPicked.GetBlue()));
        brush.SetAntiAlias(true);
        Drawing::Filter filter;
        filter.SetMaskFilter(
            Drawing::MaskFilter::CreateBlurMaskFilter(Drawing::BlurType::NORMAL, properties.GetShadowRadius()));
        brush.SetFilter(filter);
        canvas.AttachBrush(brush);
        canvas.DrawPath(path);
        canvas.DetachBrush();
    }
}
#endif

#ifndef USE_ROSEN_DRAWING
sk_sp<SkRuntimeEffect> RSPropertiesPainter::MakeGreyAdjustmentEffect()
{
    SkString GreyGradationString(R"(
        uniform shader imageShader;
        uniform float coefficient1;
        uniform float coefficient2;

        float poww(float x, float y) {
            return (x < 0) ? -pow(-x, y) : pow(x, y);
        }

        float calculateT_y(float rgb) {
            if (rgb > 127.5) { rgb = 255 - rgb; }
            float b = 38.0;
            float c = 45.0;
            float d = 127.5;
            float A = 106.5;    // 3 * b - 3 * c + d;
            float B = -93;      // 3 * (c - 2 * b);
            float C = 114;      // 3 * b;
            float p = 0.816240163988;                   // (3 * A * C - pow(B, 2)) / (3 * pow(A, 2));
            float q = -rgb / 106.5 + 0.262253485943;    // -rgb/A - B*C/(3*pow(A,2)) + 2*pow(B,3)/(27*pow(A,3))
            float s1 = -(q / 2.0);
            float s2 = sqrt(pow(s1, 2) + pow(p / 3, 3));
            return poww((s1 + s2), 1.0 / 3) + poww((s1 - s2), 1.0 / 3) - (B / (3 * A));
        }

        float calculateGreyAdjustY(float rgb) {
            float t_r = calculateT_y(rgb);
            return (rgb < 127.5) ? (rgb + coefficient1 * pow((1 - t_r), 3)) : (rgb - coefficient2 * pow((1 - t_r), 3));
        }

        half4 main(float2 coord) {
            vec3 color = vec3(imageShader.eval(coord).r, imageShader.eval(coord).g, imageShader.eval(coord).b);
            float Y = (0.299 * color.r + 0.587 * color.g + 0.114 * color.b) * 255;
            float U = (-0.147 * color.r - 0.289 * color.g + 0.436 * color.b) * 255;
            float V = (0.615 * color.r - 0.515 * color.g - 0.100 * color.b) * 255;
            Y = calculateGreyAdjustY(Y);
            color.r = (Y + 1.14 * V) / 255.0;
            color.g = (Y - 0.39 * U - 0.58 * V) / 255.0;
            color.b = (Y + 2.03 * U) / 255.0;

            return vec4(color, 1.0);
        }
    )");
    if (!greyAdjustEffect_) {
        auto [greyAdjustEffect, greyAdjustError] = SkRuntimeEffect::MakeForShader(GreyGradationString);
        if (!greyAdjustEffect) {
            return nullptr;
        }
        greyAdjustEffect_ = std::move(greyAdjustEffect);
    }

    return greyAdjustEffect_;
}
#else
std::shared_ptr<Drawing::RuntimeEffect> RSPropertiesPainter::MakeGreyAdjustmentEffect()
{
    static const std::string GreyGradationString(R"(
        uniform shader imageShader;
        uniform float coefficient1;
        uniform float coefficient2;

        float poww(float x, float y) {
            return (x < 0) ? -pow(-x, y) : pow(x, y);
        }

        float calculateT_y(float rgb) {
            if (rgb > 127.5) { rgb = 255 - rgb; }
            float b = 38.0;
            float c = 45.0;
            float d = 127.5;
            float A = 106.5;    // 3 * b - 3 * c + d;
            float B = -93;      // 3 * (c - 2 * b);
            float C = 114;      // 3 * b;
            float p = 0.816240163988;                   // (3 * A * C - pow(B, 2)) / (3 * pow(A, 2));
            float q = -rgb / 106.5 + 0.262253485943;    // -rgb/A - B*C/(3*pow(A,2)) + 2*pow(B,3)/(27*pow(A,3))
            float s1 = -(q / 2.0);
            float s2 = sqrt(pow(s1, 2) + pow(p / 3, 3));
            return poww((s1 + s2), 1.0 / 3) + poww((s1 - s2), 1.0 / 3) - (B / (3 * A));
        }

        float calculateGreyAdjustY(float rgb) {
            float t_r = calculateT_y(rgb);
            return (rgb < 127.5) ? (rgb + coefficient1 * pow((1 - t_r), 3)) : (rgb - coefficient2 * pow((1 - t_r), 3));
        }

        half4 main(float2 coord) {
            vec3 color = vec3(imageShader.eval(coord).r, imageShader.eval(coord).g, imageShader.eval(coord).b);
            float Y = (0.299 * color.r + 0.587 * color.g + 0.114 * color.b) * 255;
            float U = (-0.147 * color.r - 0.289 * color.g + 0.436 * color.b) * 255;
            float V = (0.615 * color.r - 0.515 * color.g - 0.100 * color.b) * 255;
            Y = calculateGreyAdjustY(Y);
            color.r = (Y + 1.14 * V) / 255.0;
            color.g = (Y - 0.39 * U - 0.58 * V) / 255.0;
            color.b = (Y + 2.03 * U) / 255.0;

            return vec4(color, 1.0);
        }
    )");
    if (!greyAdjustEffect_) {
        std::shared_ptr<Drawing::RuntimeEffect> greyAdjustEffect =
            Drawing::RuntimeEffect::CreateForShader(GreyGradationString);
        if (!greyAdjustEffect) {
            return nullptr;
        }
        greyAdjustEffect_ = std::move(greyAdjustEffect);
    }

    return greyAdjustEffect_;
}
#endif

#ifndef USE_ROSEN_DRAWING
sk_sp<SkImage> RSPropertiesPainter::DrawGreyAdjustment(SkCanvas& canvas, const sk_sp<SkImage>& image,
    const float greyCoef1, const float greyCoef2)
{
    if (image == nullptr) {
        ROSEN_LOGE("RSPropertiesPainter::DrawGreyAdjustment image is null");
        return nullptr;
    }
    RS_TRACE_NAME_FMT("RSPropertiesPainter::DrawGreyAdjustment, greyCoef1 is: %f, greyCoef2 is: %f",
        greyCoef1, greyCoef2);
    auto greyAdjustEffect = MakeGreyAdjustmentEffect();
    if (!greyAdjustEffect) {
        ROSEN_LOGE("RSPropertiesPainter::DrawGreyAdjustment greyAdjustEffect is null");
        return nullptr;
    }
    SkRuntimeShaderBuilder builder(greyAdjustEffect);
    auto imageShader = image->makeShader(SkSamplingOptions(SkFilterMode::kLinear));
    builder.child("imageShader") = imageShader;
    builder.uniform("coefficient1") = greyCoef1;
    builder.uniform("coefficient2") = greyCoef2;

    return builder.makeImage(canvas.recordingContext(), nullptr, image->imageInfo(), false);
}
#else
std::shared_ptr<Drawing::Image> RSPropertiesPainter::DrawGreyAdjustment(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image>& image, const float greyCoef1, const float greyCoef2)
{
    if (image == nullptr) {
        ROSEN_LOGE("RSPropertiesPainter::DrawGreyAdjustment image is null");
        return nullptr;
    }
    RS_TRACE_NAME_FMT("RSPropertiesPainter::DrawGreyAdjustment, greyCoef1 is: %f, greyCoef2 is: %f",
        greyCoef1, greyCoef2);
    auto greyAdjustEffect = MakeGreyAdjustmentEffect();
    if (!greyAdjustEffect) {
        ROSEN_LOGE("RSPropertiesPainter::DrawGreyAdjustment greyAdjustEffect is null");
        return nullptr;
    }
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder =
        std::make_shared<Drawing::RuntimeShaderBuilder>(greyAdjustEffect);
    Drawing::Matrix matrix;
    auto imageShader = Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), matrix);
    builder->SetChild("imageShader", imageShader);
    builder->SetUniform("coefficient1", greyCoef1);
    builder->SetUniform("coefficient2", greyCoef2);
    return builder->MakeImage(canvas.GetGPUContext().get(), nullptr, image->GetImageInfo(), false);
}
#endif

#ifndef USE_ROSEN_DRAWING
void RSPropertiesPainter::DrawFilter(const RSProperties& properties, RSPaintFilterCanvas& canvas,
    FilterType filterType, const std::optional<SkRect>& rect, const std::shared_ptr<RSFilter>& externalFilter)
#else
void RSPropertiesPainter::DrawFilter(const RSProperties& properties, RSPaintFilterCanvas& canvas,
    FilterType filterType, const std::optional<Drawing::Rect>& rect, const std::shared_ptr<RSFilter>& externalFilter)
#endif
{
    if (!BLUR_ENABLED) {
        ROSEN_LOGD("RSPropertiesPainter::DrawFilter close blur.");
        return;
    }
    // use provided filter if not null
    auto& RSFilter = externalFilter ? externalFilter
        : ((filterType == FilterType::BACKGROUND_FILTER) ? properties.GetBackgroundFilter() : properties.GetFilter());
    if (RSFilter == nullptr) {
        return;
    }

    bool needSnapshotOutset = true;
    if (RSFilter->GetFilterType() == RSFilter::MATERIAL) {
        auto material = std::static_pointer_cast<RSMaterialFilter>(RSFilter);
        needSnapshotOutset = (material->GetRadius() >= SNAPSHOT_OUTSET_BLUR_RADIUS_THRESHOLD);
    }
    RS_OPTIONAL_TRACE_NAME("DrawFilter " + RSFilter->GetDescription());
    g_blurCnt++;
#ifndef USE_ROSEN_DRAWING
    SkAutoCanvasRestore acr(&canvas, true);
    if (RSSystemProperties::GetPropertyDrawableEnable()) {
        // do nothing
    } else if (rect.has_value()) {
        canvas.clipRect((*rect), true);
    } else if (properties.GetClipBounds() != nullptr) {
        canvas.clipPath(properties.GetClipBounds()->GetSkiaPath(), true);
    } else { // we always do clip for DrawFilter, even if ClipToBounds is false
        canvas.clipRRect(RRect2SkRRect(properties.GetRRect()), true);
    }

    auto filter = std::static_pointer_cast<RSSkiaFilter>(RSFilter);
    filter->SetGreyCoef(properties.GetGreyCoef1(), properties.GetGreyCoef2(), properties.IsGreyAdjustmentValid());
    auto skSurface = canvas.GetSurface();
    if (skSurface == nullptr) {
        ROSEN_LOGD("RSPropertiesPainter::DrawFilter skSurface null");
        auto paint = filter->GetPaint();
        SkCanvas::SaveLayerRec slr(nullptr, &paint, SkCanvas::kInitWithPrevious_SaveLayerFlag);
        canvas.saveLayer(slr);
        filter->PostProcess(canvas);
        return;
    }
#else
    Drawing::AutoCanvasRestore acr(canvas, true);
    if (RSSystemProperties::GetPropertyDrawableEnable()) {
        // do nothing
    } else if (rect.has_value()) {
        canvas.ClipRect((*rect), Drawing::ClipOp::INTERSECT, true);
    } else if (properties.GetClipBounds() != nullptr) {
        canvas.ClipPath(properties.GetClipBounds()->GetDrawingPath(), Drawing::ClipOp::INTERSECT, true);
    } else { // we always do clip for DrawFilter, even if ClipToBounds is false
        canvas.ClipRoundRect(RRect2DrawingRRect(properties.GetRRect()), Drawing::ClipOp::INTERSECT, true);
    }

    auto filter = std::static_pointer_cast<RSDrawingFilter>(RSFilter);
    filter->SetGreyCoef(properties.GetGreyCoef1(), properties.GetGreyCoef2(), properties.IsGreyAdjustmentValid());
    auto surface = canvas.GetSurface();
    if (surface == nullptr) {
        ROSEN_LOGD("RSPropertiesPainter::DrawFilter surface null");
        Drawing::Brush brush = filter->GetBrush();
        Drawing::SaveLayerOps slr(nullptr, &brush, Drawing::SaveLayerOps::Flags::INIT_WITH_PREVIOUS);
        canvas.SaveLayer(slr);
        filter->PostProcess(canvas);
        return;
    }
#endif

    // for foreground filter, when do online opacity, rendering result already applied opacity,
    // so drawImage should not apply opacity again
    RSAutoCanvasRestore autoCanvasRestore(&canvas,
        filterType == FilterType::FOREGROUND_FILTER ? RSPaintFilterCanvas::kAlpha : RSPaintFilterCanvas::kNone);
    if (filterType == FilterType::FOREGROUND_FILTER) {
        canvas.SetAlpha(1.0);
    }

#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    // Optional use cacheManager to draw filter
    if (auto& cacheManager = properties.GetFilterCacheManager(filterType == FilterType::FOREGROUND_FILTER);
        cacheManager != nullptr && !canvas.GetDisableFilterCache()) {
        cacheManager->DrawFilter(canvas, filter, needSnapshotOutset);
        return;
    }
#endif

#ifndef USE_ROSEN_DRAWING
    auto clipIBounds = canvas.getDeviceClipBounds();
    auto imageSnapshot = skSurface->makeImageSnapshot(
        needSnapshotOutset ? clipIBounds.makeOutset(-1, -1) : clipIBounds);
#else
    auto clipIBounds = canvas.GetDeviceClipBounds();
    auto imageClipIBounds = clipIBounds;
    if (needSnapshotOutset) {
        imageClipIBounds.MakeOutset(-1, -1);
    }
    auto imageSnapshot = surface->GetImageSnapshot(imageClipIBounds);
#endif
    if (imageSnapshot == nullptr) {
        ROSEN_LOGD("RSPropertiesPainter::DrawFilter image null");
        return;
    }
#ifndef USE_ROSEN_DRAWING
    if (RSSystemProperties::GetImageGpuResourceCacheEnable(imageSnapshot->width(), imageSnapshot->height())) {
        ROSEN_LOGD("DrawFilter cache image resource(width:%{public}d, height:%{public}d).",
            imageSnapshot->width(), imageSnapshot->height());
        as_IB(imageSnapshot)->hintCacheGpuResource();
#else
    if (RSSystemProperties::GetImageGpuResourceCacheEnable(imageSnapshot->GetWidth(), imageSnapshot->GetHeight())) {
        ROSEN_LOGD("DrawFilter cache image resource(width:%{public}d, height:%{public}d).",
            imageSnapshot->GetWidth(), imageSnapshot->GetHeight());
        imageSnapshot->HintCacheGpuResource();
#endif
    }

    filter->PreProcess(imageSnapshot);
#ifndef USE_ROSEN_DRAWING
    canvas.resetMatrix();
    auto visibleIRect = canvas.GetVisibleRect().round();
    if (!visibleIRect.isEmpty()) {
        canvas.clipIRect(visibleIRect);
    }
    filter->DrawImageRect(
        canvas, imageSnapshot, SkRect::Make(imageSnapshot->bounds()), SkRect::Make(clipIBounds));
#else
    canvas.ResetMatrix();
    auto visibleRect = canvas.GetVisibleRect();
    visibleRect.Round();
    auto visibleIRect = Drawing::RectI(
        static_cast<int>(visibleRect.GetLeft()), static_cast<int>(visibleRect.GetTop()),
        static_cast<int>(visibleRect.GetRight()), static_cast<int>(visibleRect.GetBottom()));
    if (!visibleIRect.IsEmpty()) {
        canvas.ClipIRect(visibleIRect, Drawing::ClipOp::INTERSECT);
    }
    Drawing::Rect srcRect = Drawing::Rect(0, 0, imageSnapshot->GetWidth(), imageSnapshot->GetHeight());
    Drawing::Rect dstRect = clipIBounds;
    filter->DrawImageRect(canvas, imageSnapshot, srcRect, dstRect);
#endif
    filter->PostProcess(canvas);
}

void RSPropertiesPainter::DrawBackgroundImageAsEffect(const RSProperties& properties, RSPaintFilterCanvas& canvas)
{
    RS_TRACE_FUNC();
    auto boundsRect = properties.GetBoundsRect();

    // Optional use cacheManager to draw filter, cache is valid, skip drawing background image
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    if (auto& cacheManager = properties.GetFilterCacheManager(false);
        cacheManager != nullptr && !canvas.GetDisableFilterCache() && cacheManager->IsCacheValid()) {
        // no need to validate parameters, the caller already do it
#ifndef USE_ROSEN_DRAWING
        canvas.clipRect(RSPropertiesPainter::Rect2SkRect(boundsRect));
        auto filter = std::static_pointer_cast<RSSkiaFilter>(properties.GetBackgroundFilter());
#else
        canvas.ClipRect(RSPropertiesPainter::Rect2DrawingRect(boundsRect));
        auto filter = std::static_pointer_cast<RSDrawingFilter>(properties.GetBackgroundFilter());
#endif
        // extract cache data from cacheManager
        auto&& data = cacheManager->GeneratedCachedEffectData(canvas, filter);
        canvas.SetEffectData(data);
        return;
    }
#endif

    auto surface = canvas.GetSurface();
    if (!surface) {
        ROSEN_LOGE("RSPropertiesPainter::DrawBackgroundImageAsEffect surface null");
        return;
    }
    // create offscreen surface with same size as current surface (PLANNING: use bounds size instead)
#ifndef USE_ROSEN_DRAWING
    auto offscreenSurface = surface->makeSurface(surface->width(), surface->height());
#else
    auto offscreenSurface = surface->MakeSurface(canvas.GetWidth(), canvas.GetHeight());
#endif
    auto offscreenCanvas = std::make_shared<RSPaintFilterCanvas>(offscreenSurface.get());
    // copy matrix and other properties to offscreen canvas
#ifndef USE_ROSEN_DRAWING
    offscreenCanvas->setMatrix(canvas.getTotalMatrix());
#else
    offscreenCanvas->SetMatrix(canvas.GetTotalMatrix());
#endif
    offscreenCanvas->CopyConfiguration(canvas);
    // draw background onto offscreen canvas
    RSPropertiesPainter::DrawBackground(properties, *offscreenCanvas);
    // generate effect data
    RSPropertiesPainter::DrawBackgroundEffect(properties, *offscreenCanvas);
    // extract effect data from offscreen canvas and set to canvas
    canvas.SetEffectData(offscreenCanvas->GetEffectData());
}

void RSPropertiesPainter::DrawBackgroundEffect(
    const RSProperties& properties, RSPaintFilterCanvas& canvas)
{
    auto& RSFilter = properties.GetBackgroundFilter();
    if (RSFilter == nullptr) {
        return;
    }
    g_blurCnt++;
    RS_TRACE_NAME("DrawBackgroundEffect " + RSFilter->GetDescription());
    auto surface = canvas.GetSurface();
    if (surface == nullptr) {
        ROSEN_LOGE("RSPropertiesPainter::DrawBackgroundEffect surface null");
        return;
    }

#ifndef USE_ROSEN_DRAWING
    canvas.save();
    canvas.clipRect(Rect2SkRect(properties.GetBoundsRect()));
    auto bounds = canvas.getDeviceClipBounds();
    canvas.restore();
    auto filter = std::static_pointer_cast<RSSkiaFilter>(RSFilter);
#else
    canvas.Save();
    canvas.ClipRect(Rect2DrawingRect(properties.GetBoundsRect()));
    auto bounds = canvas.GetDeviceClipBounds();
    canvas.Restore();
    auto filter = std::static_pointer_cast<RSDrawingFilter>(RSFilter);
#endif

#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    // Optional use cacheManager to draw filter
    if (auto& cacheManager = properties.GetFilterCacheManager(false);
        cacheManager != nullptr && !canvas.GetDisableFilterCache()) {
        auto&& data = cacheManager->GeneratedCachedEffectData(canvas, filter, bounds, bounds);
        canvas.SetEffectData(data);
        return;
    }
#endif

    auto imageRect = bounds;
#ifndef USE_ROSEN_DRAWING
    auto imageSnapshot = surface->makeImageSnapshot(imageRect);
#else
    auto imageSnapshot = surface->GetImageSnapshot(imageRect);
#endif
    if (imageSnapshot == nullptr) {
        ROSEN_LOGE("RSPropertiesPainter::DrawBackgroundEffect image snapshot null");
        return;
    }

    filter->PreProcess(imageSnapshot);
    // create a offscreen skSurface
#ifndef USE_ROSEN_DRAWING
    sk_sp<SkSurface> offscreenSurface = surface->makeSurface(imageSnapshot->imageInfo());
    if (offscreenSurface == nullptr) {
        ROSEN_LOGE("RSPropertiesPainter::DrawBackgroundEffect offscreenSurface null");
        return;
    }
    RSPaintFilterCanvas offscreenCanvas(offscreenSurface.get());
    auto clipBounds = SkRect::MakeIWH(imageRect.width(), imageRect.height());
    filter->DrawImageRect(offscreenCanvas, imageSnapshot, SkRect::Make(imageSnapshot->bounds()), clipBounds);
    filter->PostProcess(offscreenCanvas);

    auto imageCache = offscreenSurface->makeImageSnapshot();
#else
    std::shared_ptr<Drawing::Surface> offscreenSurface =
        surface->MakeSurface(imageSnapshot->GetWidth(), imageSnapshot->GetHeight());
    if (offscreenSurface == nullptr) {
        ROSEN_LOGE("RSPropertiesPainter::DrawBackgroundEffect offscreenSurface null");
        return;
    }
    RSPaintFilterCanvas offscreenCanvas(offscreenSurface.get());
    auto clipBounds = Drawing::Rect(0, 0, imageRect.GetWidth(), imageRect.GetHeight());
    auto imageSnapshotBounds = Drawing::Rect(0, 0, imageSnapshot->GetWidth(), imageSnapshot->GetHeight());
    filter->DrawImageRect(offscreenCanvas, imageSnapshot, imageSnapshotBounds, clipBounds);
    filter->PostProcess(offscreenCanvas);

    auto imageCache = offscreenSurface->GetImageSnapshot();
#endif
    if (imageCache == nullptr) {
        ROSEN_LOGE("RSPropertiesPainter::DrawBackgroundEffect imageCache snapshot null");
        return;
    }
    auto data = std::make_shared<RSPaintFilterCanvas::CachedEffectData>(std::move(imageCache), std::move(imageRect));
    canvas.SetEffectData(std::move(data));
}

void RSPropertiesPainter::ApplyBackgroundEffectFallback(const RSProperties& properties, RSPaintFilterCanvas& canvas)
{
    RS_TRACE_FUNC();
    auto parentNode = properties.backref_.lock();
    while (parentNode && !parentNode->IsInstanceOf<RSEffectRenderNode>()) {
        parentNode = parentNode->GetParent().lock();
    }
    if (!parentNode) {
        ROSEN_LOGD("RSPropertiesPainter::ApplyBackgroundEffectFallback: parentNode null, draw filter failed.");
        return;
    }
    auto& filter = parentNode->GetRenderProperties().GetBackgroundFilter();
    if (filter == nullptr || !filter->IsValid()) {
        ROSEN_LOGD("RSPropertiesPainter::ApplyBackgroundEffectFallback: parent EffectRenderNode has no filter, "
                   "draw filter failed.");
        return;
    }
    DrawFilter(properties, canvas, FilterType::BACKGROUND_FILTER, std::nullopt, filter);
}

void RSPropertiesPainter::ClipVisibleCanvas(const RSProperties& properties, RSPaintFilterCanvas& canvas)
{
#ifndef USE_ROSEN_DRAWING
    if (RSSystemProperties::GetPropertyDrawableEnable()) {
        // do nothing
    } else if (properties.GetClipBounds() != nullptr) {
        canvas.clipPath(properties.GetClipBounds()->GetSkiaPath(), true);
    } else { // we always do clip for ApplyBackgroundEffect, even if ClipToBounds is false
        canvas.clipRRect(RRect2SkRRect(properties.GetRRect()), true);
    }
    canvas.resetMatrix();
    auto visibleIRect = canvas.GetVisibleRect().round();
    if (!visibleIRect.isEmpty()) {
        canvas.clipIRect(visibleIRect);
    }
#else
    if (RSSystemProperties::GetPropertyDrawableEnable()) {
        // do nothing
    } else if (properties.GetClipBounds() != nullptr) {
        canvas.ClipPath(properties.GetClipBounds()->GetDrawingPath(), Drawing::ClipOp::INTERSECT, true);
    } else { // we always do clip for ApplyBackgroundEffect, even if ClipToBounds is false
        canvas.ClipRoundRect(RRect2DrawingRRect(properties.GetRRect()), Drawing::ClipOp::INTERSECT, true);
    }
    canvas.ResetMatrix();
    auto visibleRect = canvas.GetVisibleRect();
    visibleRect.Round();
    auto visibleIRect = Drawing::RectI(
        static_cast<int>(visibleRect.GetLeft()), static_cast<int>(visibleRect.GetTop()),
        static_cast<int>(visibleRect.GetRight()), static_cast<int>(visibleRect.GetBottom()));
    if (!visibleIRect.IsEmpty()) {
        canvas.ClipIRect(visibleIRect, Drawing::ClipOp::INTERSECT);
    }
#endif
}

void RSPropertiesPainter::ApplyBackgroundEffect(const RSProperties& properties, RSPaintFilterCanvas& canvas)
{
    const auto& effectData = canvas.GetEffectData();
    if (effectData == nullptr || effectData->cachedImage_ == nullptr
        || !RSSystemProperties::GetEffectMergeEnabled()) {
        // no effectData available, draw background filter in fallback method
        ROSEN_LOGD("RSPropertiesPainter::ApplyBackgroundEffect: effectData null, try fallback method.");
        ApplyBackgroundEffectFallback(properties, canvas);
        return;
    }
    RS_TRACE_FUNC();
#ifndef USE_ROSEN_DRAWING
    SkAutoCanvasRestore acr(&canvas, true);
    ClipVisibleCanvas(properties, canvas);
    SkPaint defaultPaint;
    // dstRect: canvas clip region
    auto dstRect = SkRect::Make(canvas.getDeviceClipBounds());
    // srcRect: map dstRect onto cache coordinate
    auto srcRect = dstRect.makeOffset(-effectData->cachedRect_.left(), -effectData->cachedRect_.top());
    canvas.drawImageRect(effectData->cachedImage_, srcRect, dstRect, SkSamplingOptions(), &defaultPaint,
        SkCanvas::kFast_SrcRectConstraint);
#else
    Drawing::AutoCanvasRestore acr(canvas, true);
    ClipVisibleCanvas(properties, canvas);
    Drawing::Brush brush;
    canvas.AttachBrush(brush);
    // dstRect: canvas clip region
    Drawing::Rect dstRect = canvas.GetDeviceClipBounds();
    // srcRect: map dstRect onto cache coordinate
    Drawing::Rect srcRect = dstRect;
    srcRect.Offset(-effectData->cachedRect_.GetLeft(), -effectData->cachedRect_.GetTop());
    canvas.DrawImageRect(*effectData->cachedImage_, srcRect, dstRect,
                         Drawing::SamplingOptions(), Drawing::SrcRectConstraint::FAST_SRC_RECT_CONSTRAINT);
    canvas.DetachBrush();
#endif
}

void RSPropertiesPainter::DrawPixelStretch(const RSProperties& properties, RSPaintFilterCanvas& canvas)
{
    auto& pixelStretch = properties.GetPixelStretch();
    if (!pixelStretch.has_value()) {
        return;
    }
#ifndef USE_ROSEN_DRAWING
    auto skSurface = canvas.GetSurface();
    if (skSurface == nullptr) {
        ROSEN_LOGE("RSPropertiesPainter::DrawPixelStretch skSurface null");
        return;
    }

    canvas.save();
    auto bounds = RSPropertiesPainter::Rect2SkRect(properties.GetBoundsRect());
    canvas.clipRect(bounds);
    auto clipBounds = canvas.getDeviceClipBounds();
    clipBounds.setXYWH(clipBounds.left(), clipBounds.top(), clipBounds.width() - 1, clipBounds.height() - 1);
    canvas.restore();

    /* Calculates the relative coordinates of the clipbounds
        with respect to the origin of the current canvas coordinates */
    SkMatrix worldToLocalMat;
    if (!canvas.getTotalMatrix().invert(&worldToLocalMat)) {
        ROSEN_LOGE("RSPropertiesPainter::DrawPixelStretch get invert matrix failed.");
    }
    SkRect localClipBounds;
    SkRect fClipBounds = SkRect::MakeXYWH(clipBounds.x(), clipBounds.y(), clipBounds.width(), clipBounds.height());
    if (!worldToLocalMat.mapRect(&localClipBounds, fClipBounds)) {
        ROSEN_LOGE("RSPropertiesPainter::DrawPixelStretch map rect failed.");
    }

    if (!bounds.intersect(localClipBounds)) {
        ROSEN_LOGE("RSPropertiesPainter::DrawPixelStretch intersect clipbounds failed");
    }

    auto scaledBounds = SkRect::MakeLTRB(bounds.left() - pixelStretch->x_, bounds.top() - pixelStretch->y_,
        bounds.right() + pixelStretch->z_, bounds.bottom() + pixelStretch->w_);
    if (scaledBounds.isEmpty() || bounds.isEmpty() || clipBounds.isEmpty()) {
        ROSEN_LOGE("RSPropertiesPainter::DrawPixelStretch invalid scaled bounds");
        return;
    }

    auto image = skSurface->makeImageSnapshot(clipBounds);
    if (image == nullptr) {
        ROSEN_LOGE("RSPropertiesPainter::DrawPixelStretch image null");
        return;
    }

    SkPaint paint;
    SkMatrix inverseMat, rotateMat;
    auto boundsGeo = (properties.GetBoundsGeometry());
    if (boundsGeo && !boundsGeo->IsEmpty()) {
        auto transMat = canvas.getTotalMatrix();
        /* transMat.getSkewY() is the sin of the rotation angle(sin0 = 0,sin90 =1 sin180 = 0,sin270 = -1),
            if transMat.getSkewY() is not 0 or -1 or 1,the rotation angle is not a multiple of 90,not Stretch*/
        if (ROSEN_EQ(transMat.getSkewY(), 0.f) || ROSEN_EQ(transMat.getSkewY(), 1.f) ||
            ROSEN_EQ(transMat.getSkewY(), -1.f)) {
        } else {
            ROSEN_LOGD("rotate degree is not 0 or 90 or 180 or 270,return.");
            return;
        }
        rotateMat.setScale(transMat.getScaleX(), transMat.getScaleY());
        rotateMat.setSkewX(transMat.getSkewX());
        rotateMat.setSkewY(transMat.getSkewY());
        rotateMat.preTranslate(-bounds.x(), -bounds.y());
        rotateMat.postTranslate(bounds.x(), bounds.y());

        SkRect transBounds = rotateMat.mapRect(bounds);

        rotateMat.setTranslateX(bounds.x() - transBounds.x());
        rotateMat.setTranslateY(bounds.y() - transBounds.y());
        if (!rotateMat.invert(&inverseMat)) {
            ROSEN_LOGE("RSPropertiesPainter::DrawPixelStretch get invert matrix failed.");
        }
    }

    canvas.save();
    canvas.translate(bounds.x(), bounds.y());

    // NOTE: Ensure that EPS is consistent with rs_properties.cpp
    constexpr static float EPS = 1e-5f;
    if (pixelStretch->x_ > EPS || pixelStretch->y_ > EPS || pixelStretch->z_ > EPS || pixelStretch->w_ > EPS) {
        paint.setShader(image->makeShader(SkTileMode::kClamp, SkTileMode::kClamp, SkSamplingOptions(), &inverseMat));
        canvas.drawRect(
            SkRect::MakeXYWH(-pixelStretch->x_, -pixelStretch->y_, scaledBounds.width(), scaledBounds.height()), paint);
    } else {
        inverseMat.postScale(scaledBounds.width() / bounds.width(), scaledBounds.height() / bounds.height());
        paint.setShader(image->makeShader(SkTileMode::kClamp, SkTileMode::kClamp, SkSamplingOptions(), &inverseMat));
        canvas.translate(-pixelStretch->x_, -pixelStretch->y_);
        canvas.drawRect(SkRect::MakeXYWH(pixelStretch->x_, pixelStretch->y_, bounds.width(), bounds.height()), paint);
    }
    canvas.restore();
}
#else
    auto surface = canvas.GetSurface();
    if (surface == nullptr) {
        ROSEN_LOGE("RSPropertiesPainter::DrawPixelStretch surface null");
        return;
    }

    canvas.Save();
    auto bounds = RSPropertiesPainter::Rect2DrawingRect(properties.GetBoundsRect());
    canvas.ClipRect(bounds, Drawing::ClipOp::INTERSECT, false);
    auto tmpBounds = canvas.GetDeviceClipBounds();
    Drawing::Rect clipBounds(
        tmpBounds.GetLeft(), tmpBounds.GetTop(), tmpBounds.GetRight() - 1, tmpBounds.GetBottom() - 1);
    canvas.Restore();

    /*  Calculates the relative coordinates of the clipbounds
        with respect to the origin of the current canvas coordinates */
    Drawing::Matrix worldToLocalMat;
    if (!canvas.GetTotalMatrix().Invert(worldToLocalMat)) {
        ROSEN_LOGE("RSPropertiesPainter::DrawPixelStretch get invert matrix failed.");
    }
    Drawing::Rect localClipBounds;
    Drawing::Rect fClipBounds(clipBounds.GetLeft(), clipBounds.GetTop(), clipBounds.GetRight(),
        clipBounds.GetBottom());
    if (!worldToLocalMat.MapRect(localClipBounds, fClipBounds)) {
        ROSEN_LOGE("RSPropertiesPainter::DrawPixelStretch map rect failed.");
    }

    if (!bounds.Intersect(localClipBounds)) {
        ROSEN_LOGE("RSPropertiesPainter::DrawPixelStretch intersect clipbounds failed");
    }

    auto scaledBounds = Drawing::Rect(bounds.GetLeft() - pixelStretch->x_, bounds.GetTop() - pixelStretch->y_,
        bounds.GetRight() + pixelStretch->z_, bounds.GetBottom() + pixelStretch->w_);
    if (!scaledBounds.IsValid() || !bounds.IsValid() || !clipBounds.IsValid()) {
        ROSEN_LOGE("RSPropertiesPainter::DrawPixelStretch invalid scaled bounds");
        return;
    }

    Drawing::RectI rectI(static_cast<int>(fClipBounds.GetLeft()), static_cast<int>(fClipBounds.GetTop()),
        static_cast<int>(fClipBounds.GetRight()), static_cast<int>(fClipBounds.GetBottom()));
    auto image = surface->GetImageSnapshot(rectI);
    if (image == nullptr) {
        ROSEN_LOGE("RSPropertiesPainter::DrawPixelStretch image null");
        return;
    }

    Drawing::Brush brush;
    Drawing::Matrix inverseMat, rotateMat;
    auto boundsGeo = (properties.GetBoundsGeometry());
    if (boundsGeo && !boundsGeo->IsEmpty()) {
        auto transMat = canvas.GetTotalMatrix();
        /* transMat.getSkewY() is the sin of the rotation angle(sin0 = 0,sin90 =1 sin180 = 0,sin270 = -1),
            if transMat.getSkewY() is not 0 or -1 or 1,the rotation angle is not a multiple of 90,not Stretch*/
        auto skewY = transMat.Get(Drawing::Matrix::SKEW_Y);
        if (ROSEN_EQ(skewY, 0.f) || ROSEN_EQ(skewY, 1.f) ||
            ROSEN_EQ(skewY, -1.f)) {
        } else {
            ROSEN_LOGD("rotate degree is not 0 or 90 or 180 or 270,return.");
            return;
        }
        rotateMat.SetScale(transMat.Get(Drawing::Matrix::SCALE_X), transMat.Get(Drawing::Matrix::SCALE_Y));
        rotateMat.Set(Drawing::Matrix::SKEW_X, transMat.Get(Drawing::Matrix::SKEW_X));
        rotateMat.Set(Drawing::Matrix::SKEW_Y, transMat.Get(Drawing::Matrix::SKEW_Y));
        rotateMat.PreTranslate(-bounds.GetLeft(), -bounds.GetTop());
        rotateMat.PostTranslate(bounds.GetLeft(), bounds.GetTop());

        Drawing::Rect transBounds;
        rotateMat.MapRect(transBounds, bounds);
        rotateMat.Set(Drawing::Matrix::TRANS_X, bounds.GetLeft() - transBounds.GetLeft());
        rotateMat.Set(Drawing::Matrix::TRANS_Y, bounds.GetTop() - transBounds.GetTop());
        if (!rotateMat.Invert(inverseMat)) {
            ROSEN_LOGE("RSPropertiesPainter::DrawPixelStretch get invert matrix failed.");
        }
    }

    canvas.Save();
    canvas.Translate(bounds.GetLeft(), bounds.GetTop());
    Drawing::SamplingOptions samplingOptions;
    constexpr static float EPS = 1e-5f;
    if (pixelStretch->x_ > EPS || pixelStretch->y_ > EPS || pixelStretch->z_ > EPS || pixelStretch->w_ > EPS) {
        brush.SetShaderEffect(Drawing::ShaderEffect::CreateImageShader(
            *image, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, samplingOptions, inverseMat));
        canvas.AttachBrush(brush);
        canvas.DrawRect(Drawing::Rect(-pixelStretch->x_, -pixelStretch->y_,
            -pixelStretch->x_ + scaledBounds.GetWidth(), -pixelStretch->y_ + scaledBounds.GetHeight()));
        canvas.DetachBrush();
    } else {
        inverseMat.PostScale(scaledBounds.GetWidth() / bounds.GetWidth(), scaledBounds.GetHeight() / bounds.GetHeight());
        brush.SetShaderEffect(Drawing::ShaderEffect::CreateImageShader(
            *image, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, samplingOptions, inverseMat));

        canvas.Translate(-pixelStretch->x_, -pixelStretch->y_);
        canvas.AttachBrush(brush);
        canvas.DrawRect(Drawing::Rect(pixelStretch->x_, pixelStretch->y_,
            pixelStretch->x_ + bounds.GetWidth(), pixelStretch->y_ + bounds.GetHeight()));
        canvas.DetachBrush();
    }
    canvas.Restore();
}
#endif

#ifndef USE_ROSEN_DRAWING
SkColor RSPropertiesPainter::CalcAverageColor(sk_sp<SkImage> imageSnapshot)
{
    // create a 1x1 SkPixmap
    uint32_t pixel[1] = { 0 };
    auto single_pixel_info = SkImageInfo::Make(1, 1, kRGBA_8888_SkColorType, kPremul_SkAlphaType);
    SkPixmap single_pixel(single_pixel_info, pixel, single_pixel_info.bytesPerPixel());

    // resize snapshot to 1x1 to calculate average color
    // kMedium_SkFilterQuality will do bilerp + mipmaps for down-scaling, we can easily get average color
    imageSnapshot->scalePixels(single_pixel, SkSamplingOptions(SkFilterMode::kLinear, SkMipmapMode::kLinear));
    // convert color format and return average color
    return SkColor4f::FromBytes_RGBA(pixel[0]).toSkColor();
}
#else
Drawing::ColorQuad RSPropertiesPainter::CalcAverageColor(std::shared_ptr<Drawing::Image> imageSnapshot)
{
    // create a 1x1 SkPixmap
    uint32_t pixel[1] = { 0 };
    Drawing::ImageInfo single_pixel_info(1, 1, Drawing::ColorType::COLORTYPE_RGBA_8888,
        Drawing::AlphaType::ALPHATYPE_PREMUL);
    Drawing::Bitmap single_pixel;
    single_pixel.Build(single_pixel_info, single_pixel_info.GetBytesPerPixel());
    single_pixel.SetPixels(pixel);

    // resize snapshot to 1x1 to calculate average color
    // kMedium_SkFilterQuality will do bilerp + mipmaps for down-scaling, we can easily get average color
    imageSnapshot->ScalePixels(single_pixel,
        Drawing::SamplingOptions(Drawing::FilterMode::LINEAR, Drawing::MipmapMode::LINEAR));
    // convert color format and return average color
    return SkColor4f::FromBytes_RGBA(pixel[0]).toSkColor();
}
#endif

int RSPropertiesPainter::GetAndResetBlurCnt()
{
    auto blurCnt = g_blurCnt;
    g_blurCnt = 0;
    return blurCnt;
}

void RSPropertiesPainter::DrawBackground(const RSProperties& properties, RSPaintFilterCanvas& canvas,
    bool isAntiAlias, bool isSurfaceView)
{
    // only disable antialias when background is rect and g_forceBgAntiAlias is false
    bool antiAlias = g_forceBgAntiAlias || !properties.GetCornerRadius().IsZero();
    // clip
#ifndef USE_ROSEN_DRAWING
    if (properties.GetClipBounds() != nullptr) {
        canvas.clipPath(properties.GetClipBounds()->GetSkiaPath(), antiAlias);
    } else if (properties.GetClipToBounds()) {
        // In NEW_SKIA version, L476 code will cause crash if the second parameter is true.
        // so isAntiAlias is false only the method is called in ProcessAnimatePropertyBeforeChildren().
        if (properties.GetCornerRadius().IsZero()) {
            canvas.clipRect(Rect2SkRect(properties.GetBoundsRect()), isAntiAlias);
        } else {
            canvas.clipRRect(RRect2SkRRect(properties.GetRRect()), antiAlias);
        }
    } else if (properties.GetClipToRRect()) {
        canvas.clipRRect(RRect2SkRRect(properties.GetClipRRect()), antiAlias);
    }
    // paint backgroundColor
    SkPaint paint;
    paint.setAntiAlias(antiAlias);
    auto bgColor = properties.GetBackgroundColor();
    if (bgColor != RgbPalette::Transparent() && !isSurfaceView) {
        paint.setColor(bgColor.AsArgbInt());
        canvas.drawRRect(RRect2SkRRect(properties.GetInnerRRect()), paint);
    }
    if (const auto& bgShader = properties.GetBackgroundShader()) {
        SkAutoCanvasRestore acr(&canvas, true);
        canvas.clipRRect(RRect2SkRRect(properties.GetRRect()), antiAlias);
        paint.setShader(bgShader->GetSkShader());
        canvas.drawPaint(paint);
    }
    if (const auto& bgImage = properties.GetBgImage()) {
        SkAutoCanvasRestore acr(&canvas, true);
        canvas.clipRRect(RRect2SkRRect(properties.GetRRect()), antiAlias);
        auto boundsRect = Rect2SkRect(properties.GetBoundsRect());
        bgImage->SetDstRect(properties.GetBgImageRect());
        bgImage->CanvasDrawImage(canvas, boundsRect, SkSamplingOptions(), paint, true);
    }
#else
    if (properties.GetClipBounds() != nullptr) {
        auto& path = properties.GetClipBounds()->GetDrawingPath();
        canvas.ClipPath(path, Drawing::ClipOp::INTERSECT, antiAlias);
    } else if (properties.GetClipToBounds()) {
        if (properties.GetCornerRadius().IsZero()) {
            canvas.ClipRect(Rect2DrawingRect(properties.GetBoundsRect()), Drawing::ClipOp::INTERSECT, isAntiAlias);
        } else {
            canvas.ClipRoundRect(RRect2DrawingRRect(properties.GetRRect()), Drawing::ClipOp::INTERSECT, antiAlias);
        }
    } else if (properties.GetClipToRRect()) {
        canvas.ClipRoundRect(RRect2DrawingRRect(properties.GetClipRRect()), Drawing::ClipOp::INTERSECT, antiAlias);
    }
    // paint backgroundColor
    Drawing::Brush brush;
    brush.SetAntiAlias(antiAlias);
    auto bgColor = properties.GetBackgroundColor();
    if (bgColor != RgbPalette::Transparent() && !isSurfaceView) {
        brush.SetColor(Drawing::Color(bgColor.AsArgbInt()));
        canvas.AttachBrush(brush);
        canvas.DrawRoundRect(RRect2DrawingRRect(properties.GetRRect()));
        canvas.DetachBrush();
    }
    if (const auto& bgShader = properties.GetBackgroundShader()) {
        Drawing::AutoCanvasRestore acr(canvas, true);
        canvas.ClipRoundRect(RRect2DrawingRRect(properties.GetRRect()), Drawing::ClipOp::INTERSECT, antiAlias);
        auto shaderEffect = bgShader->GetDrawingShader();
        brush.SetShaderEffect(shaderEffect);
        canvas.DrawBackground(brush);
    }
    if (const auto& bgImage = properties.GetBgImage()) {
        Drawing::AutoCanvasRestore acr(canvas, true);
        canvas.ClipRoundRect(RRect2DrawingRRect(properties.GetRRect()), Drawing::ClipOp::INTERSECT, antiAlias);
        auto boundsRect = Rect2DrawingRect(properties.GetBoundsRect());
        bgImage->SetDstRect(properties.GetBgImageRect());
        canvas.AttachBrush(brush);
        bgImage->CanvasDrawImage(canvas, boundsRect, Drawing::SamplingOptions(), true);
        canvas.DetachBrush();
    }
#endif
}

void RSPropertiesPainter::SetBgAntiAlias(bool forceBgAntiAlias)
{
    g_forceBgAntiAlias = forceBgAntiAlias;
}

bool RSPropertiesPainter::GetBgAntiAlias()
{
    return g_forceBgAntiAlias;
}

#ifndef USE_ROSEN_DRAWING
void RSPropertiesPainter::DrawFrame(const RSProperties& properties, RSPaintFilterCanvas& canvas, DrawCmdListPtr& cmds)
{
    if (cmds == nullptr) {
        return;
    }
    SkMatrix mat;
    if (GetGravityMatrix(
            properties.GetFrameGravity(), properties.GetFrameRect(), cmds->GetWidth(), cmds->GetHeight(), mat)) {
        canvas.concat(mat);
    }
    auto frameRect = Rect2SkRect(properties.GetFrameRect());
    cmds->Playback(canvas, &frameRect);
}
#else
void RSPropertiesPainter::DrawFrame(
    const RSProperties& properties, RSPaintFilterCanvas& canvas, Drawing::DrawCmdListPtr& cmds)
{
    if (cmds == nullptr) {
        return;
    }
    Drawing::Matrix mat;
    if (GetGravityMatrix(
            properties.GetFrameGravity(), properties.GetFrameRect(), cmds->GetWidth(), cmds->GetHeight(), mat)) {
        canvas.ConcatMatrix(mat);
    }
    auto frameRect = Rect2DrawingRect(properties.GetFrameRect());
    cmds->Playback(canvas, &frameRect);
}
#endif

RRect RSPropertiesPainter::GetRRectForDrawingBorder(const RSProperties& properties,
    const std::shared_ptr<RSBorder>& border, const bool& isOutline)
{
    if (!border) {
        return RRect();
    }

    return isOutline ?
        RRect(properties.GetRRect().rect_.MakeOutset(border->GetWidthFour()), border->GetRadiusFour()) :
        properties.GetRRect();
}

RRect RSPropertiesPainter::GetInnerRRectForDrawingBorder(const RSProperties& properties,
    const std::shared_ptr<RSBorder>& border, const bool& isOutline)
{
    if (!border) {
        return RRect();
    }
    return isOutline ? properties.GetRRect() : properties.GetInnerRRect();
}

#ifndef USE_ROSEN_DRAWING
static std::shared_ptr<SkRuntimeShaderBuilder> phongShaderBuilder;

const std::shared_ptr<SkRuntimeShaderBuilder>& RSPropertiesPainter::GetPhongShaderBuilder()
{
    if (phongShaderBuilder) {
        return phongShaderBuilder;
    }
    sk_sp<SkRuntimeEffect> lightEffect_;
    SkString lightString(R"(
        uniform mat4 lightPos;
        uniform mat4 viewPos;
        uniform vec4 specularStrength;

        half4 main(float2 fragCoord) {
            vec3 lightColor = vec3(1.0, 1.0, 1.0);
            float ambientStrength = 0.0;
            vec3 diffuseColor = vec3(1.0, 1.0, 1.0);
            float diffuseStrength = 0.0;
            vec3 specularColor = vec3(1.0, 1.0, 1.0);
            float shininess = 8.0;
            half4 fragColor = half4(0.0, 0.0, 0.0, 0.0);
            vec4 NormalMap = vec4(0.0, 0.0, 1.0, 0.0);
            // ambient
            vec4 ambient = lightColor.rgb1 * ambientStrength;
            vec3 norm = normalize(NormalMap.rgb);

            for (int i = 0; i < 4; i++) {
                if (abs(specularStrength[i]) > 0.01) {
                    vec3 lightDir = normalize(vec3(lightPos[i].xy - fragCoord, lightPos[i].z));
                    float diff = max(dot(norm, lightDir), 0.0);
                    vec4 diffuse = diff * lightColor.rgb1;
                    vec3 viewDir = normalize(vec3(viewPos[i].xy - fragCoord, viewPos[i].z)); // view vector
                    vec3 halfwayDir = normalize(lightDir + viewDir); // half vector
                    float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess); // exponential relationship of angle
                    vec4 specular = lightColor.rgb1 * spec; // multiply color of incident light
                    vec4 o = ambient + diffuse * diffuseStrength * diffuseColor.rgb1; // diffuse reflection
                    fragColor = fragColor + o + specular * specularStrength[i] * specularColor.rgb1;
                }
            }
            return half4(fragColor);
        }
    )");
    auto [lightEffect, error] = SkRuntimeEffect::MakeForShader(lightString);
    if (!lightEffect) {
        ROSEN_LOGE("light effect errro: %{public}s", error.c_str());
        return phongShaderBuilder;
    }
    lightEffect_ = std::move(lightEffect);
    phongShaderBuilder = std::make_shared<SkRuntimeShaderBuilder>(lightEffect_);
    return phongShaderBuilder;
}
#else
static std::shared_ptr<Drawing::RuntimeShaderBuilder> phongShaderBuilder = nullptr;

const std::shared_ptr<Drawing::RuntimeShaderBuilder>& RSPropertiesPainter::GetPhongShaderBuilder()
{
    if (phongShaderBuilder) {
        return phongShaderBuilder;
    }
    std::shared_ptr<Drawing::RuntimeEffect> lightEffect_;
    std::string lightString(R"(
        uniform mat4 lightPos;
        uniform mat4 viewPos;
        uniform vec4 specularStrength;

        half4 main(float2 fragCoord) {
            vec3 lightColor = vec3(1.0, 1.0, 1.0);
            float ambientStrength = 0.0;
            vec3 diffuseColor = vec3(1.0, 1.0, 1.0);
            float diffuseStrength = 0.0;
            vec3 specularColor = vec3(1.0, 1.0, 1.0);
            float shininess = 8.0;
            half4 fragColor = half4(0.0, 0.0, 0.0, 0.0);
            vec4 NormalMap = vec4(0.0, 0.0, 1.0, 0.0);
            // ambient
            vec4 ambient = lightColor.rgb1 * ambientStrength;
            vec3 norm = normalize(NormalMap.rgb);

            for (int i = 0; i < 4; i++) {
                if (abs(specularStrength[i]) > 0.01) {
                    vec3 lightDir = normalize(vec3(lightPos[i].xy - fragCoord, lightPos[i].z));
                    float diff = max(dot(norm, lightDir), 0.0);
                    vec4 diffuse = diff * lightColor.rgb1;
                    vec3 viewDir = normalize(vec3(viewPos[i].xy - fragCoord, viewPos[i].z)); // view vector
                    vec3 halfwayDir = normalize(lightDir + viewDir); // half vector
                    float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess); // exponential relationship of angle
                    vec4 specular = lightColor.rgb1 * spec; // multiply color of incident light
                    vec4 o = ambient + diffuse * diffuseStrength * diffuseColor.rgb1; // diffuse reflection
                    fragColor = fragColor + o + specular * specularStrength[i] * specularColor.rgb1;
                }
            }
            return half4(fragColor);
        }
    )");
    std::shared_ptr<Drawing::RuntimeEffect> lightEffect = Drawing::RuntimeEffect::CreateForShader(lightString);
    if (!lightEffect) {
        ROSEN_LOGE("light effect error");
        return phongShaderBuilder;
    }
    lightEffect_ = std::move(lightEffect);
    phongShaderBuilder = std::make_shared<Drawing::RuntimeShaderBuilder>(lightEffect_);
    return phongShaderBuilder;
}
#endif

#ifndef USE_ROSEN_DRAWING
void RSPropertiesPainter::DrawLight(const RSProperties& properties, SkCanvas& canvas)
#else
void RSPropertiesPainter::DrawLight(const RSProperties& properties, Drawing::Canvas& canvas)
#endif
{
    auto lightBuilder = GetPhongShaderBuilder();
    if (!lightBuilder) {
        return;
    }
    const auto& lightSources = properties.GetIlluminated()->GetLightSources();
    if (lightSources.empty()) {
        ROSEN_LOGD("RSPropertiesPainter::DrawLight lightSourceList is empty!");
        return;
    }
    const auto& geoPtr = (properties.GetBoundsGeometry());
    if (!geoPtr || geoPtr->IsEmpty()) {
        return;
    }
    DrawLightInner(properties, canvas, lightBuilder, lightSources, geoPtr);
}

#ifndef USE_ROSEN_DRAWING
void RSPropertiesPainter::DrawLightInner(const RSProperties& properties, SkCanvas& canvas,
    std::shared_ptr<SkRuntimeShaderBuilder>& lightBuilder,
    const std::unordered_set<std::shared_ptr<RSLightSource>>& lightSources,
    const std::shared_ptr<RSObjAbsGeometry>& geoPtr)
{
    auto iter = lightSources.begin();
    auto cnt = 0;
    SkM44 lightPositionMatrix;
    SkM44 viewPosMatrix;
    SkV4 lightIntensityV4;
    constexpr int MAX_LIGHT_SOUCES = 4;
    while (iter != lightSources.end() && cnt < MAX_LIGHT_SOUCES) {
        auto lightPos = RSPointLightManager::Instance()->CalculateLightPosForIlluminated((*iter), geoPtr);
        auto lightIntensity = (*iter)->GetLightIntensity();
        auto lightPositionV4 = SkV4 { lightPos.x_, lightPos.y_, lightPos.z_, lightPos.w_ };
        auto viewPosV4 = SkV4 { lightPos.x_, lightPos.y_, lightPos.z_, lightPos.w_ };
        lightIntensityV4[cnt] = lightIntensity;
        lightPositionMatrix.setCol(cnt, lightPositionV4);
        viewPosMatrix.setCol(cnt, viewPosV4);
        iter++;
        cnt++;
    }
    lightBuilder->uniform("lightPos") = lightPositionMatrix;
    lightBuilder->uniform("viewPos") = viewPosMatrix;
    SkPaint paint;
    paint.setAntiAlias(true);
    auto illuminatedType = properties.GetIlluminated()->GetIlluminatedType();
    ROSEN_LOGD("RSPropertiesPainter::DrawLight illuminatedType:%{public}d", illuminatedType);
    if (illuminatedType == IlluminatedType::CONTENT) {
        DrawContentLight(properties, canvas, lightBuilder, paint, lightIntensityV4);
    } else if (illuminatedType == IlluminatedType::BORDER) {
        DrawBorderLight(properties, canvas, lightBuilder, paint, lightIntensityV4);
    } else if (illuminatedType == IlluminatedType::BORDER_CONTENT) {
        DrawContentLight(properties, canvas, lightBuilder, paint, lightIntensityV4);
        DrawBorderLight(properties, canvas, lightBuilder, paint, lightIntensityV4);
    }
}
#else
void RSPropertiesPainter::DrawLightInner(const RSProperties& properties, Drawing::Canvas& canvas,
    std::shared_ptr<Drawing::RuntimeShaderBuilder>& lightBuilder,
    const std::unordered_set<std::shared_ptr<RSLightSource>>& lightSources,
    const std::shared_ptr<RSObjAbsGeometry>& geoPtr)
{
    auto iter = lightSources.begin();
    auto cnt = 0;
    Drawing::Matrix44 lightPositionMatrix;
    Drawing::Matrix44 viewPosMatrix;
    Vector4f lightIntensityV4;
    constexpr int MAX_LIGHT_SOUCES = 4;
    while (iter != lightSources.end() && cnt < MAX_LIGHT_SOUCES) {
        auto lightPos = RSPointLightManager::Instance()->CalculateLightPosForIlluminated((*iter), geoPtr);
        auto lightIntensity = (*iter)->GetLightIntensity();
        lightIntensityV4[cnt] = lightIntensity;
        lightPositionMatrix.SetCol(cnt, lightPos.x_, lightPos.y_, lightPos.z_, lightPos.w_);
        viewPosMatrix.SetCol(cnt, lightPos.x_, lightPos.y_, lightPos.z_, lightPos.w_);
        iter++;
        cnt++;
    }
    lightBuilder->SetUniform("lightPos", lightPositionMatrix);
    lightBuilder->SetUniform("viewPos", viewPosMatrix);
    Drawing::Pen pen;
    Drawing::Brush brush;
    pen.SetAntiAlias(true);
    brush.SetAntiAlias(true);
    auto illuminatedType = properties.GetIlluminated()->GetIlluminatedType();
    ROSEN_LOGD("RSPropertiesPainter::DrawLight illuminatedType:%{public}d", illuminatedType);
    if (illuminatedType == IlluminatedType::CONTENT) {
        DrawContentLight(properties, canvas, lightBuilder, brush, lightIntensityV4);
    } else if (illuminatedType == IlluminatedType::BORDER) {
        DrawBorderLight(properties, canvas, lightBuilder, pen, lightIntensityV4);
    } else if (illuminatedType == IlluminatedType::BORDER_CONTENT) {
        DrawContentLight(properties, canvas, lightBuilder, brush, lightIntensityV4);
        DrawBorderLight(properties, canvas, lightBuilder, pen, lightIntensityV4);
    }
}
#endif

#ifndef USE_ROSEN_DRAWING
void RSPropertiesPainter::DrawContentLight(const RSProperties& properties, SkCanvas& canvas,
    std::shared_ptr<SkRuntimeShaderBuilder>& lightBuilder, SkPaint& paint, SkV4& lightIntensity)
#else
void RSPropertiesPainter::DrawContentLight(const RSProperties& properties, Drawing::Canvas& canvas,
    std::shared_ptr<Drawing::RuntimeShaderBuilder>& lightBuilder, Drawing::Brush& brush, Vector4f& lightIntensity)
#endif
{
    // content light
#ifndef USE_ROSEN_DRAWING
    sk_sp<SkShader> shader;
#else
    std::shared_ptr<Drawing::ShaderEffect> shader;
#endif
    auto contentStrength = lightIntensity * 0.3f;
#ifndef USE_ROSEN_DRAWING
    lightBuilder->uniform("specularStrength") = contentStrength;
    shader = lightBuilder->makeShader(nullptr, false);
    paint.setShader(shader);
    canvas.drawRRect(RRect2SkRRect(properties.GetRRect()), paint);
#else
    lightBuilder->SetUniformVec4("specularStrength", contentStrength.x_,
        contentStrength.y_, contentStrength.z_, contentStrength.w_);
    shader = lightBuilder->MakeShader(nullptr, false);
    brush.SetShaderEffect(shader);
    canvas.AttachBrush(brush);
    canvas.DrawRoundRect(RRect2DrawingRRect(properties.GetRRect()));
    canvas.DetachBrush();
#endif
}

#ifndef USE_ROSEN_DRAWING
void RSPropertiesPainter::DrawBorderLight(const RSProperties& properties, SkCanvas& canvas,
    std::shared_ptr<SkRuntimeShaderBuilder>& lightBuilder, SkPaint& paint, SkV4& lightIntensity)
#else
void RSPropertiesPainter::DrawBorderLight(const RSProperties& properties, Drawing::Canvas& canvas,
    std::shared_ptr<Drawing::RuntimeShaderBuilder>& lightBuilder, Drawing::Pen& pen, Vector4f& lightIntensity)
#endif
{
    // border light
#ifndef USE_ROSEN_DRAWING
    sk_sp<SkShader> shader;
#else
    std::shared_ptr<Drawing::ShaderEffect> shader;
#endif
#ifndef USE_ROSEN_DRAWING
    lightBuilder->uniform("specularStrength") = lightIntensity;
    shader = lightBuilder->makeShader(nullptr, false);
    paint.setShader(shader);
    float borderWidth = std::ceil(properties.GetIlluminatedBorderWidth());
    paint.setStrokeWidth(borderWidth);
    paint.setStyle(SkPaint::Style::kStroke_Style);
#else
    lightBuilder->SetUniformVec4("specularStrength", lightIntensity.x_,
        lightIntensity.y_, lightIntensity.z_, lightIntensity.w_);
    shader = lightBuilder->MakeShader(nullptr, false);
    pen.SetShaderEffect(shader);
    float borderWidth = std::ceil(properties.GetIlluminatedBorderWidth());
    pen.SetWidth(borderWidth);
#endif
    auto borderRect = properties.GetRRect().rect_;
    float borderRadius = properties.GetRRect().radius_[0].x_;
    auto borderRRect = RRect(RectF(borderRect.left_ + borderWidth / 2.0f, borderRect.top_ + borderWidth / 2.0f,
        borderRect.width_ - borderWidth, borderRect.height_ - borderWidth),
        borderRadius - borderWidth / 2.0f, borderRadius - borderWidth / 2.0f);
#ifndef USE_ROSEN_DRAWING
    canvas.drawRRect(RRect2SkRRect(borderRRect), paint);
#else
    canvas.AttachPen(pen);
    canvas.DrawRoundRect(RRect2DrawingRRect(borderRRect));
    canvas.DetachPen();
#endif
}

#ifndef USE_ROSEN_DRAWING
void RSPropertiesPainter::DrawBorderBase(const RSProperties& properties, SkCanvas& canvas,
    const std::shared_ptr<RSBorder>& border, const bool& isOutline)
{
    if (!border || !border->HasBorder()) {
        return;
    }

    SkPaint paint;
    paint.setAntiAlias(true);
    if (border->ApplyFillStyle(paint)) {
        auto skRRect = RRect2SkRRect(GetRRectForDrawingBorder(
            properties, border, isOutline));
        auto innerSkRRect = RRect2SkRRect(GetInnerRRectForDrawingBorder(
            properties, border, isOutline));
        canvas.drawDRRect(skRRect, innerSkRRect, paint);
    } else {
        bool isZero = isOutline ? border->GetRadiusFour().IsZero() : properties.GetCornerRadius().IsZero();
        if (isZero && border->ApplyFourLine(paint)) {
            RectF rectf = isOutline ?
                properties.GetBoundsRect().MakeOutset(border->GetWidthFour()) : properties.GetBoundsRect();
            border->PaintFourLine(canvas, paint, rectf);
        } else if (border->ApplyPathStyle(paint)) {
            auto borderWidth = border->GetWidth();
            RRect rrect = GetRRectForDrawingBorder(properties, border, isOutline);
            rrect.rect_.width_ -= borderWidth;
            rrect.rect_.height_ -= borderWidth;
            rrect.rect_.Move(borderWidth / PARAM_DOUBLE, borderWidth / PARAM_DOUBLE);
            SkPath borderPath;
            borderPath.addRRect(RRect2SkRRect(rrect));
            canvas.drawPath(borderPath, paint);
        } else {
            SkAutoCanvasRestore acr(&canvas, true);
            auto rrect = RRect2SkRRect(GetRRectForDrawingBorder(
                properties, border, isOutline));
            canvas.clipRRect(rrect, true);
            auto innerSkRRect = RRect2SkRRect(GetInnerRRectForDrawingBorder(
                properties, border, isOutline));
            canvas.clipRRect(innerSkRRect, SkClipOp::kDifference, true);
            paint.setStyle(SkPaint::Style::kStroke_Style);
            SkPoint center = { innerSkRRect.rect().centerX(), innerSkRRect.rect().centerY() };
            border->PaintTopPath(canvas, paint, rrect, center);
            border->PaintRightPath(canvas, paint, rrect, center);
            border->PaintBottomPath(canvas, paint, rrect, center);
            border->PaintLeftPath(canvas, paint, rrect, center);
        }
    }
}
#else
void RSPropertiesPainter::DrawBorderBase(const RSProperties& properties, Drawing::Canvas& canvas,
    const std::shared_ptr<RSBorder>& border, const bool& isOutline)
{
    if (!border || !border->HasBorder()) {
        return;
    }

    Drawing::Brush brush;
    Drawing::Pen pen;
    brush.SetAntiAlias(true);
    pen.SetAntiAlias(true);
    if (border->ApplyFillStyle(brush)) {
        auto roundRect = RRect2DrawingRRect(GetRRectForDrawingBorder(properties, border, isOutline));
        auto innerRoundRect = RRect2DrawingRRect(GetInnerRRectForDrawingBorder(
            properties, border, isOutline));
        canvas.AttachBrush(brush);
        canvas.DrawNestedRoundRect(roundRect, innerRoundRect);
        canvas.DetachBrush();
    } else {
        bool isZero = isOutline ? properties.GetCornerRadius().IsZero() : border->GetRadiusFour().IsZero();
        if (isZero && border->ApplyFourLine(pen)) {
            RectF rectf = isOutline ?
                properties.GetBoundsRect().MakeOutset(border->GetWidthFour()) : properties.GetBoundsRect();
            border->PaintFourLine(canvas, pen, rectf);
        } else if (border->ApplyPathStyle(pen)) {
            auto borderWidth = border->GetWidth();
            RRect rrect = GetRRectForDrawingBorder(properties, border, isOutline);
            rrect.rect_.width_ -= borderWidth;
            rrect.rect_.height_ -= borderWidth;
            rrect.rect_.Move(borderWidth / PARAM_DOUBLE, borderWidth / PARAM_DOUBLE);
            Drawing::Path borderPath;
            borderPath.AddRoundRect(RRect2DrawingRRect(rrect));
            canvas.AttachPen(pen);
            canvas.DrawPath(borderPath);
            canvas.DetachPen();
        } else {
            Drawing::AutoCanvasRestore acr(canvas, true);
            auto rrect = RRect2DrawingRRect(GetRRectForDrawingBorder(properties, border, isOutline));
            canvas.ClipRoundRect(rrect, Drawing::ClipOp::INTERSECT, true);
            auto innerRoundRect = RRect2DrawingRRect(GetInnerRRectForDrawingBorder(
                properties, border, isOutline));
            canvas.ClipRoundRect(innerRoundRect, Drawing::ClipOp::DIFFERENCE, true);
            Drawing::scalar centerX = innerRoundRect.GetRect().GetLeft() + innerRoundRect.GetRect().GetWidth() / 2;
            Drawing::scalar centerY = innerRoundRect.GetRect().GetTop() + innerRoundRect.GetRect().GetHeight() / 2;
            Drawing::Point center = { centerX, centerY };
            auto rect = rrect.GetRect();
            Drawing::SaveLayerOps slr(&rect, nullptr);
            canvas.SaveLayer(slr);
            border->PaintTopPath(canvas, pen, rrect, center);
            border->PaintRightPath(canvas, pen, rrect, center);
            border->PaintBottomPath(canvas, pen, rrect, center);
            border->PaintLeftPath(canvas, pen, rrect, center);
        }
    }
}
#endif

#ifndef USE_ROSEN_DRAWING
void RSPropertiesPainter::DrawBorder(const RSProperties& properties, SkCanvas& canvas)
#else
void RSPropertiesPainter::DrawBorder(const RSProperties& properties, Drawing::Canvas& canvas)
#endif
{
    auto border = properties.GetBorder();
    if (border && border->HasBorder()) {
        DrawBorderBase(properties, canvas, border, false);
    }
}

void RSPropertiesPainter::GetOutlineDirtyRect(RectI& dirtyOutline,
    const RSProperties& properties, const bool& isAbsCoordinate)
{
    auto outline = properties.GetOutline();
    if (!outline || !outline->HasBorder()) {
        return;
    }

    auto geoPtr = properties.GetBoundsGeometry();
#ifndef USE_ROSEN_DRAWING
    SkMatrix matrix = (geoPtr && isAbsCoordinate) ? geoPtr->GetAbsMatrix() : SkMatrix::I();
    auto skRect = Rect2SkRect(GetRRectForDrawingBorder(properties, outline, true).rect_);
    matrix.MapRect(&skRect);
    dirtyOutline.left_ = std::floor(skRect.left());
    dirtyOutline.top_ = std::floor(skRect.top());
    dirtyOutline.width_ = std::ceil(skRect.width()) + PARAM_DOUBLE;
    dirtyOutline.height_ = std::ceil(skRect.height()) + PARAM_DOUBLE;
#else
    Drawing::Matrix matrix = (geoPtr && isAbsCoordinate) ? geoPtr->GetAbsMatrix() : Drawing::Matrix();
    auto drawingRect = Rect2DrawingRect(GetRRectForDrawingBorder(properties, outline, true).rect_);
    matrix.MapRect(drawingRect, drawingRect);
    dirtyOutline.left_ = std::floor(drawingRect.GetLeft());
    dirtyOutline.top_ = std::floor(drawingRect.GetTop());
    dirtyOutline.width_ = std::ceil(drawingRect.GetWidth()) + PARAM_DOUBLE;
    dirtyOutline.height_ = std::ceil(drawingRect.GetHeight()) + PARAM_DOUBLE;
#endif
}

#ifndef USE_ROSEN_DRAWING
void RSPropertiesPainter::DrawOutline(const RSProperties& properties, SkCanvas& canvas)
#else
void RSPropertiesPainter::DrawOutline(const RSProperties& properties, Drawing::Canvas& canvas)
#endif
{
    auto outline = properties.GetOutline();
    if (outline && outline->HasBorder()) {
        DrawBorderBase(properties, canvas, outline, true);
    }
}

#ifndef USE_ROSEN_DRAWING
void RSPropertiesPainter::DrawForegroundColor(const RSProperties& properties, SkCanvas& canvas)
{
    auto bgColor = properties.GetForegroundColor();
    if (bgColor == RgbPalette::Transparent()) {
        return;
    }
    // clip
    if (properties.GetClipBounds() != nullptr) {
        canvas.clipPath(properties.GetClipBounds()->GetSkiaPath(), true);
    } else if (properties.GetClipToBounds()) {
        canvas.clipRect(Rect2SkRect(properties.GetBoundsRect()), true);
    } else if (properties.GetClipToRRect()) {
        canvas.clipRRect(RRect2SkRRect(properties.GetClipRRect()), true);
    }

    SkPaint paint;
    paint.setColor(bgColor.AsArgbInt());
    paint.setAntiAlias(true);
    canvas.drawRRect(RRect2SkRRect(properties.GetRRect()), paint);
}
#else
void RSPropertiesPainter::DrawForegroundColor(const RSProperties& properties, Drawing::Canvas& canvas)
{
    auto bgColor = properties.GetForegroundColor();
    if (bgColor == RgbPalette::Transparent()) {
        return;
    }
    // clip
    if (properties.GetClipBounds() != nullptr) {
        canvas.ClipPath(properties.GetClipBounds()->GetDrawingPath(), Drawing::ClipOp::INTERSECT, true);
    } else if (properties.GetClipToBounds()) {
        canvas.ClipRect(Rect2DrawingRect(properties.GetBoundsRect()), Drawing::ClipOp::INTERSECT, true);
    } else if (properties.GetClipToRRect()) {
        canvas.ClipRoundRect(RRect2DrawingRRect(properties.GetClipRRect()), Drawing::ClipOp::INTERSECT, true);
    }

    Drawing::Brush brush;
    brush.SetColor(Drawing::Color(bgColor.AsArgbInt()));
    brush.SetAntiAlias(true);
    canvas.AttachBrush(brush);
    canvas.DrawRoundRect(RRect2DrawingRRect(properties.GetRRect()));
    canvas.DetachBrush();
}
#endif

#ifndef USE_ROSEN_DRAWING
void RSPropertiesPainter::DrawMask(const RSProperties& properties, SkCanvas& canvas, SkRect maskBounds)
{
    std::shared_ptr<RSMask> mask = properties.GetMask();
    if (mask == nullptr) {
        return;
    }
    if (mask->IsSvgMask() && !mask->GetSvgDom() && !mask->GetSvgPicture()) {
        ROSEN_LOGD("RSPropertiesPainter::DrawMask not has Svg Mask property");
        return;
    }

    canvas.save();
    canvas.saveLayer(maskBounds, nullptr);
    int tmpLayer = canvas.getSaveCount();

    SkPaint maskfilter;
    auto filter = SkColorFilters::Compose(SkLumaColorFilter::Make(), SkColorFilters::SRGBToLinearGamma());
    maskfilter.setColorFilter(filter);
    canvas.saveLayer(maskBounds, &maskfilter);
    if (mask->IsSvgMask()) {
        SkAutoCanvasRestore maskSave(&canvas, true);
        canvas.translate(maskBounds.fLeft + mask->GetSvgX(), maskBounds.fTop + mask->GetSvgY());
        canvas.scale(mask->GetScaleX(), mask->GetScaleY());
        if (mask->GetSvgDom()) {
            mask->GetSvgDom()->render(&canvas);
        } else if (mask->GetSvgPicture()) {
            canvas.drawPicture(mask->GetSvgPicture());
        }
    } else if (mask->IsGradientMask()) {
        SkAutoCanvasRestore maskSave(&canvas, true);
        canvas.translate(maskBounds.fLeft, maskBounds.fTop);
        SkRect skRect = SkRect::MakeIWH(maskBounds.fRight - maskBounds.fLeft, maskBounds.fBottom - maskBounds.fTop);
        canvas.drawRect(skRect, mask->GetMaskPaint());
    } else if (mask->IsPathMask()) {
        SkAutoCanvasRestore maskSave(&canvas, true);
        canvas.translate(maskBounds.fLeft, maskBounds.fTop);
        canvas.drawPath(mask->GetMaskPath(), mask->GetMaskPaint());
    }

    // back to mask layer
    canvas.restoreToCount(tmpLayer);
    // create content layer
    SkPaint maskPaint;
    maskPaint.setBlendMode(SkBlendMode::kSrcIn);
    canvas.saveLayer(maskBounds, &maskPaint);
    canvas.clipRect(maskBounds, true);
}
#else
void RSPropertiesPainter::DrawMask(const RSProperties& properties, Drawing::Canvas& canvas, Drawing::Rect maskBounds)
{
    std::shared_ptr<RSMask> mask = properties.GetMask();
    if (mask == nullptr) {
        return;
    }
    if (mask->IsSvgMask() && !mask->GetSvgDom() && !mask->GetSvgPicture()) {
        ROSEN_LOGD("RSPropertiesPainter::DrawMask not has Svg Mask property");
        return;
    }

    canvas.Save();
    Drawing::SaveLayerOps slr(&maskBounds, nullptr);
    canvas.SaveLayer(slr);
    int tmpLayer = canvas.GetSaveCount();

    Drawing::Brush maskfilter;
    Drawing::Filter filter;
    filter.SetColorFilter(Drawing::ColorFilter::CreateComposeColorFilter(
        *(Drawing::ColorFilter::CreateLumaColorFilter()), *(Drawing::ColorFilter::CreateSrgbGammaToLinear())));
    maskfilter.SetFilter(filter);
    Drawing::SaveLayerOps slrMask(&maskBounds, &maskfilter);
    canvas.SaveLayer(slrMask);
    if (mask->IsSvgMask()) {
        Drawing::AutoCanvasRestore maskSave(canvas, true);
        canvas.Translate(maskBounds.GetLeft() + mask->GetSvgX(), maskBounds.GetTop() + mask->GetSvgY());
        canvas.Scale(mask->GetScaleX(), mask->GetScaleY());
        if (mask->GetSvgDom()) {
            canvas.DrawSVGDOM(mask->GetSvgDom());
        } else if (mask->GetSvgPicture()) {
            canvas.DrawPicture(*mask->GetSvgPicture());
        }
    } else if (mask->IsGradientMask()) {
        Drawing::AutoCanvasRestore maskSave(canvas, true);
        canvas.Translate(maskBounds.GetLeft(), maskBounds.GetTop());
        Drawing::Rect rect = Drawing::Rect(
            0, 0, maskBounds.GetWidth(), maskBounds.GetHeight());
        canvas.AttachBrush(mask->GetMaskBrush());
        canvas.DrawRect(rect);
        canvas.DetachBrush();
    } else if (mask->IsPathMask()) {
        Drawing::AutoCanvasRestore maskSave(canvas, true);
        canvas.Translate(maskBounds.GetLeft(), maskBounds.GetTop());
        canvas.AttachBrush(mask->GetMaskBrush());
        canvas.AttachPen(mask->GetMaskPen());
        canvas.DrawPath(*mask->GetMaskPath());
        canvas.DetachBrush();
        canvas.DetachPen();
    }

    // back to mask layer
    canvas.RestoreToCount(tmpLayer);
    // create content layer
    Drawing::Brush maskPaint;
    maskPaint.SetBlendMode(Drawing::BlendMode::SRC_IN);
    Drawing::SaveLayerOps slrContent(&maskBounds, &maskPaint);
    canvas.SaveLayer(slrContent);
    canvas.ClipRect(maskBounds, Drawing::ClipOp::INTERSECT, true);
}
#endif

#ifndef USE_ROSEN_DRAWING
void RSPropertiesPainter::DrawMask(const RSProperties& properties, SkCanvas& canvas)
{
    SkRect maskBounds = Rect2SkRect(properties.GetBoundsRect());
    DrawMask(properties, canvas, maskBounds);
}
#else
void RSPropertiesPainter::DrawMask(const RSProperties& properties, Drawing::Canvas& canvas)
{
    Drawing::Rect maskBounds = Rect2DrawingRect(properties.GetBoundsRect());
    DrawMask(properties, canvas, maskBounds);
}
#endif

#ifndef USE_ROSEN_DRAWING
RectF RSPropertiesPainter::GetCmdsClipRect(DrawCmdListPtr& cmds)
{
#if defined(RS_ENABLE_DRIVEN_RENDER)
    RectF clipRect;
    if (cmds == nullptr) {
        return clipRect;
    }
    SkRect rect;
    cmds->CheckClipRect(rect);
    clipRect = { rect.left(), rect.top(), rect.width(), rect.height() };
    return clipRect;
#else
    return RectF { 0.0f, 0.0f, 0.0f, 0.0f };
#endif
}
#else
RectF RSPropertiesPainter::GetCmdsClipRect(Drawing::DrawCmdListPtr& cmds)
{
    ROSEN_LOGE("Drawing Unsupport RSPropertiesPainter::GetCmdsClipRect");
    return RectF { 0.0f, 0.0f, 0.0f, 0.0f };
}
#endif

#ifndef USE_ROSEN_DRAWING
void RSPropertiesPainter::DrawFrameForDriven(const RSProperties& properties, RSPaintFilterCanvas& canvas,
                                             DrawCmdListPtr& cmds)
#else
void RSPropertiesPainter::DrawFrameForDriven(const RSProperties& properties, RSPaintFilterCanvas& canvas,
                                             Drawing::DrawCmdListPtr& cmds)
#endif
{
#if defined(RS_ENABLE_DRIVEN_RENDER)
    if (cmds == nullptr) {
        return;
    }
#ifndef USE_ROSEN_DRAWING
    SkMatrix mat;
    if (GetGravityMatrix(
            properties.GetFrameGravity(), properties.GetFrameRect(), cmds->GetWidth(), cmds->GetHeight(), mat)) {
        canvas.concat(mat);
    }
    auto frameRect = Rect2SkRect(properties.GetFrameRect());
#else
    Rosen::Drawing::Matrix mat;
    if (GetGravityMatrix(
            properties.GetFrameGravity(), properties.GetFrameRect(), cmds->GetWidth(), cmds->GetHeight(), mat)) {
        canvas.ConcatMatrix(mat);
    }
    auto frameRect = Rect2DrawingRect(properties.GetFrameRect());
#endif
    // temporary solution for driven content clip
#ifndef USE_ROSEN_DRAWING
    cmds->ReplaceDrivenCmds();
    cmds->Playback(canvas, &frameRect);
    cmds->RestoreOriginCmdsForDriven();
#else
    cmds->Playback(canvas, &frameRect);
#endif
#endif
}

#ifndef USE_ROSEN_DRAWING
void RSPropertiesPainter::DrawSpherize(const RSProperties& properties, RSPaintFilterCanvas& canvas,
    const sk_sp<SkSurface>& spherizeSurface)
{
    if (spherizeSurface == nullptr) {
        return;
    }
    SkAutoCanvasRestore acr(&canvas, true);
    float canvasWidth = properties.GetBoundsRect().GetWidth();
    float canvasHeight = properties.GetBoundsRect().GetHeight();
    if (spherizeSurface->width() == 0 || spherizeSurface->height() == 0) {
        return;
    }
    canvas.scale(canvasWidth / spherizeSurface->width(), canvasHeight / spherizeSurface->height());

    auto imageSnapshot = spherizeSurface->makeImageSnapshot();
    if (imageSnapshot == nullptr) {
        ROSEN_LOGE("RSPropertiesPainter::DrawCachedSpherizeSurface image  is null");
        return;
    }

    SkPaint paint;
    paint.setBlendMode(SkBlendMode::kSrcOver);
    paint.setShader(imageSnapshot->makeShader(SkTileMode::kClamp, SkTileMode::kClamp, SkSamplingOptions()));

    float width = imageSnapshot->width();
    float height = imageSnapshot->height();
    float degree = properties.GetSpherize();
    bool isWidthGreater = width > height;
    ROSEN_LOGI("RSPropertiesPainter::DrawCachedSpherizeSurface spherize degree [%{public}f]", degree);

    const SkPoint texCoords[4] = {
        {0.0f, 0.0f}, {width, 0.0f}, {width, height}, {0.0f, height}
    };
    float offsetSquare = 0.f;
    if (isWidthGreater) {
        offsetSquare = (width - height) * degree / 2.0; // half of the change distance
        width = width - (width - height) * degree;
    } else {
        offsetSquare = (height - width) * degree / 2.0; // half of the change distance
        height = height - (height - width) * degree;
    }

    float segmentWidthOne = width / 3.0;
    float segmentWidthTwo = width / 3.0 * 2.0;
    float segmentHeightOne = height / 3.0;
    float segmentHeightTwo = height / 3.0 * 2.0;
    float offsetSphereWidth = width / 6 * degree;
    float offsetSphereHeight = height / 6  * degree;

    SkPoint ctrlPoints[12] = {
        // top edge control points
        {0.0f, 0.0f}, {segmentWidthOne, 0.0f}, {segmentWidthTwo, 0.0f}, {width, 0.0f},
        // right edge control points
        {width, segmentHeightOne}, {width, segmentHeightTwo},
        // bottom edge control points
        {width, height}, {segmentWidthTwo, height}, {segmentWidthOne, height}, {0.0f, height},
        // left edge control points
        {0.0f, segmentHeightTwo}, {0.0f, segmentHeightOne}
    };
    ctrlPoints[0].offset(offsetSphereWidth, offsetSphereHeight); // top left control point
    ctrlPoints[3].offset(-offsetSphereWidth, offsetSphereHeight); // top right control point
    ctrlPoints[6].offset(-offsetSphereWidth, -offsetSphereHeight); // bottom right control point
    ctrlPoints[9].offset(offsetSphereWidth, -offsetSphereHeight); // bottom left control point
    if (isWidthGreater) {
        SkPoint::Offset(ctrlPoints, SK_ARRAY_COUNT(ctrlPoints), offsetSquare, 0);
    } else {
        SkPoint::Offset(ctrlPoints, SK_ARRAY_COUNT(ctrlPoints), 0, offsetSquare);
    }
    SkPath path;
    path.moveTo(ctrlPoints[0]);
    path.cubicTo(ctrlPoints[1], ctrlPoints[2], ctrlPoints[3]); // upper edge
    path.cubicTo(ctrlPoints[4], ctrlPoints[5], ctrlPoints[6]); // right edge
    path.cubicTo(ctrlPoints[7], ctrlPoints[8], ctrlPoints[9]); // bottom edge
    path.cubicTo(ctrlPoints[10], ctrlPoints[11], ctrlPoints[0]); // left edge
    canvas.clipPath(path, true);
    canvas.drawPatch(ctrlPoints, nullptr, texCoords, SkBlendMode::kSrcOver, paint);
}
#else
void RSPropertiesPainter::DrawSpherize(const RSProperties& properties, RSPaintFilterCanvas& canvas,
    const std::shared_ptr<Drawing::Surface>& spherizeSurface)
{
    if (spherizeSurface == nullptr) {
        return;
    }
    if (spherizeSurface->Width() == 0 || spherizeSurface->Height() == 0) {
        return;
    }
    Drawing::AutoCanvasRestore acr(canvas, true);
    float canvasWidth = properties.GetBoundsRect().GetWidth();
    float canvasHeight = properties.GetBoundsRect().GetHeight();

    auto imageSnapshot = spherizeSurface->GetImageSnapshot();
    if (imageSnapshot == nullptr) {
        ROSEN_LOGE("RSPropertiesPainter::DrawCachedSpherizeSurface image is null");
        return;
    }
    int imageWidth = imageSnapshot->GetWidth();
    int imageHeight = imageSnapshot->GetHeight();
    if (imageWidth == 0 || imageHeight == 0) {
        return;
    }
    canvas.Scale(canvasWidth / imageWidth, canvasHeight / imageHeight);

    float width = imageWidth;
    float height = imageHeight;
    float degree = properties.GetSpherize();
    bool isWidthGreater = width > height;
    ROSEN_LOGI("RSPropertiesPainter::DrawCachedSpherizeSurface spherize degree [%{public}f]", degree);

    Drawing::Brush brush;
    brush.SetBlendMode(Drawing::BlendMode::SRC_OVER);
    Drawing::SamplingOptions samplingOptions;
    Drawing::Matrix scaleMat;
    brush.SetShaderEffect(Drawing::ShaderEffect::CreateImageShader(
        *imageSnapshot, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, samplingOptions, scaleMat));
    canvas.AttachBrush(brush);

    const Drawing::Point texCoords[4] = {
        {0.0f, 0.0f}, {width, 0.0f}, {width, height}, {0.0f, height}
    };
    float offsetSquare = 0.f;
    if (isWidthGreater) {
        offsetSquare = (width - height) * degree / 2.0; // half of the change distance
        width = width - (width - height) * degree;
    } else {
        offsetSquare = (height - width) * degree / 2.0; // half of the change distance
        height = height - (height - width) * degree;
    }

    float segmentWidthOne = width / 3.0;
    float segmentWidthTwo = width / 3.0 * 2.0;
    float segmentHeightOne = height / 3.0;
    float segmentHeightTwo = height / 3.0 * 2.0;
    float offsetSphereWidth = width / 6 * degree;
    float offsetSphereHeight = height / 6  * degree;

    const int PointNum = 12;
    Drawing::Point ctrlPoints[PointNum] = {
        // top edge control points
        {0.0f, 0.0f}, {segmentWidthOne, 0.0f}, {segmentWidthTwo, 0.0f}, {width, 0.0f},
        // right edge control points
        {width, segmentHeightOne}, {width, segmentHeightTwo},
        // bottom edge control points
        {width, height}, {segmentWidthTwo, height}, {segmentWidthOne, height}, {0.0f, height},
        // left edge control points
        {0.0f, segmentHeightTwo}, {0.0f, segmentHeightOne}
    };
    ctrlPoints[0].Offset(offsetSphereWidth, offsetSphereHeight); // top left control point
    ctrlPoints[3].Offset(-offsetSphereWidth, offsetSphereHeight); // top right control point
    ctrlPoints[6].Offset(-offsetSphereWidth, -offsetSphereHeight); // bottom right control point
    ctrlPoints[9].Offset(offsetSphereWidth, -offsetSphereHeight); // bottom left control point
    if (isWidthGreater) {
        for (int i = 0; i < PointNum; ++i) {
            ctrlPoints[i].Offset(offsetSquare, 0);
        }
    } else {
        for (int i = 0; i < PointNum; ++i) {
            ctrlPoints[i].Offset(0, offsetSquare);
        }
    }
    Drawing::Path path;
    path.MoveTo(ctrlPoints[0].GetX(), ctrlPoints[0].GetY());
    path.CubicTo(ctrlPoints[1], ctrlPoints[2], ctrlPoints[3]); // upper edge
    path.CubicTo(ctrlPoints[4], ctrlPoints[5], ctrlPoints[6]); // right edge
    path.CubicTo(ctrlPoints[7], ctrlPoints[8], ctrlPoints[9]); // bottom edge
    path.CubicTo(ctrlPoints[10], ctrlPoints[11], ctrlPoints[0]); // left edge
    canvas.ClipPath(path, Drawing::ClipOp::INTERSECT, true);
    canvas.DrawPatch(ctrlPoints, nullptr, texCoords, Drawing::BlendMode::SRC_OVER);
    canvas.DetachBrush();
}
#endif

void RSPropertiesPainter::DrawColorFilter(const RSProperties& properties, RSPaintFilterCanvas& canvas)
{
    // if useEffect defined, use color filter from parent EffectView.
    auto& colorFilter = properties.GetColorFilter();
    if (colorFilter == nullptr) {
        return;
    }
#ifndef USE_ROSEN_DRAWING
    SkAutoCanvasRestore acr(&canvas, true);
    canvas.clipRRect(RRect2SkRRect(properties.GetRRect()), true);
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setColorFilter(colorFilter);
    auto skSurface = canvas.GetSurface();
    if (skSurface == nullptr) {
        ROSEN_LOGE("RSPropertiesPainter::DrawColorFilter skSurface is null");
        return;
    }
    auto clipBounds = canvas.getDeviceClipBounds();
    auto imageSnapshot = skSurface->makeImageSnapshot(clipBounds);
    if (imageSnapshot == nullptr) {
        ROSEN_LOGE("RSPropertiesPainter::DrawColorFilter image is null");
        return;
    }
    as_IB(imageSnapshot)->hintCacheGpuResource();
    canvas.resetMatrix();
    SkSamplingOptions options(SkFilterMode::kNearest, SkMipmapMode::kNone);
    canvas.drawImageRect(imageSnapshot, SkRect::Make(clipBounds), options, &paint);
#else
    Drawing::AutoCanvasRestore acr(canvas, true);
    canvas.ClipRoundRect(RRect2DrawingRRect(properties.GetRRect()), Drawing::ClipOp::INTERSECT, true);
    Drawing::Brush brush;
    brush.SetAntiAlias(true);
    Drawing::Filter filter;
    filter.SetColorFilter(colorFilter);
    brush.SetFilter(filter);
    auto surface = canvas.GetSurface();
    if (surface == nullptr) {
        ROSEN_LOGE("RSPropertiesPainter::DrawColorFilter surface is null");
        return;
    }
    auto clipBounds = canvas.GetDeviceClipBounds();
    auto imageSnapshot = surface->GetImageSnapshot(clipBounds);
    if (imageSnapshot == nullptr) {
        ROSEN_LOGE("RSPropertiesPainter::DrawColorFilter image is null");
        return;
    }
    imageSnapshot->HintCacheGpuResource();
    canvas.ResetMatrix();
    Drawing::SamplingOptions options(Drawing::FilterMode::NEAREST, Drawing::MipmapMode::NONE);
    canvas.AttachBrush(brush);
    canvas.DrawImageRect(*imageSnapshot, clipBounds, options);
    canvas.DetachBrush();
#endif
}

#ifndef USE_ROSEN_DRAWING
void RSPropertiesPainter::DrawBinarizationShader(const RSProperties& properties, RSPaintFilterCanvas& canvas)
{
    SkAutoCanvasRestore acr(&canvas, true);
    canvas.clipRRect(RRect2SkRRect(properties.GetRRect()), true);
    auto skSurface = canvas.GetSurface();
    if (skSurface == nullptr) {
        ROSEN_LOGE("RSPropertiesPainter::DrawColorFilter skSurface is null");
        return;
    }
    auto clipBounds = canvas.getDeviceClipBounds();
    auto imageSnapshot = skSurface->makeImageSnapshot(clipBounds);
    if (imageSnapshot == nullptr) {
        ROSEN_LOGE("RSPropertiesPainter::DrawColorFilter image is null");
        return;
    }
    auto& aiInvert = properties.GetAiInvert();
    auto imageShader = imageSnapshot->makeShader(SkSamplingOptions(SkFilterMode::kLinear));
    float thresholdLow = aiInvert->z_ - aiInvert->w_;
    float thresholdHigh = aiInvert->z_ + aiInvert->w_;
    auto shader = MakeBinarizationShader(
        aiInvert->x_, aiInvert->y_, thresholdLow, thresholdHigh, imageShader);
    SkPaint paint;
    paint.setShader(shader);
    paint.setAntiAlias(true);
    canvas.resetMatrix();
    canvas.translate(clipBounds.left(), clipBounds.top());
    canvas.drawPaint(paint);
}
#else
void RSPropertiesPainter::DrawBinarizationShader(const RSProperties& properties, RSPaintFilterCanvas& canvas)
{
    Drawing::AutoCanvasRestore acr(canvas, true);
    canvas.ClipRoundRect(RRect2DrawingRRect(properties.GetRRect()), Drawing::ClipOp::INTERSECT, true);
    auto drSurface = canvas.GetSurface();
    if (drSurface == nullptr) {
        ROSEN_LOGE("RSPropertiesPainter::DrawColorFilter drSurface is null");
        return;
    }
    auto clipBounds = canvas.GetDeviceClipBounds();
    auto imageSnapshot = drSurface->GetImageSnapshot(clipBounds);
    if (imageSnapshot == nullptr) {
        ROSEN_LOGE("RSPropertiesPainter::DrawColorFilter image is null");
        return;
    }
    auto& aiInvert = properties.GetAiInvert();
    Drawing::Matrix matrix;
    auto imageShader = Drawing::ShaderEffect::CreateImageShader(*imageSnapshot, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), matrix);
    float thresholdLow = aiInvert->z_ - aiInvert->w_;
    float thresholdHigh = aiInvert->z_ + aiInvert->w_;
    auto shader = MakeBinarizationShader(
        aiInvert->x_, aiInvert->y_, thresholdLow, thresholdHigh, imageShader);
    Drawing::Brush brush;
    brush.SetShaderEffect(shader);
    brush.SetAntiAlias(true);
    canvas.ResetMatrix();
    canvas.Translate(clipBounds.GetLeft(), clipBounds.GetTop());
    canvas.DrawBackground(brush);
}
#endif
 
#ifndef USE_ROSEN_DRAWING
sk_sp<SkShader> RSPropertiesPainter::MakeBinarizationShader(float low, float high, float thresholdLow,
    float thresholdHigh, sk_sp<SkShader> imageShader)
{
    static constexpr char prog[] = R"(
        uniform half low;
        uniform half high;
        uniform half thresholdLow;
        uniform half thresholdHigh;
        uniform shader imageShader;

        half4 main(float2 coord) {
            half3 c = imageShader.eval(float2(coord.x, coord.y)).rgb;
            float gray = 0.299 * c.r + 0.587 * c.g + 0.114 * c.b;
            float lowRes = mix(high, -1.0, step(thresholdLow, gray));
            float highRes = mix(-1.0, low, step(thresholdHigh, gray));
            float midRes = (thresholdHigh - gray) * (high - low) / (thresholdHigh - thresholdLow) + low;
            float invertedGray = mix(midRes, max(lowRes, highRes), step(-0.5, max(lowRes, highRes)));
            half3 invert = half3(invertedGray);
            return half4(invert, 1.0);
        }
    )";
    auto [effect, err] = SkRuntimeEffect::MakeForShader(SkString(prog));
    if (!effect) {
        ROSEN_LOGE("MakeBinarizationShader::RuntimeShader effect error: %{public}s\n", err.c_str());
        return nullptr;
    }
    thresholdHigh = thresholdHigh <= thresholdLow ? thresholdHigh + 1e-6 : thresholdHigh;
    SkRuntimeShaderBuilder builder(effect);
    builder.child("imageShader") = imageShader;
    builder.uniform("low") = low;
    builder.uniform("high") = high;
    builder.uniform("thresholdLow") = thresholdLow;
    builder.uniform("thresholdHigh") = thresholdHigh;
    return builder.makeShader(nullptr, false);
}
#else
std::shared_ptr<Drawing::ShaderEffect> RSPropertiesPainter::MakeBinarizationShader(float low, float high,
    float thresholdLow, float thresholdHigh, std::shared_ptr<Drawing::ShaderEffect> imageShader)
{
    static constexpr char prog[] = R"(
        uniform half low;
        uniform half high;
        uniform half thresholdLow;
        uniform half thresholdHigh;
        uniform shader imageShader;

        half4 main(float2 coord) {
            half3 c = imageShader.eval(float2(coord.x, coord.y)).rgb;
            float gray = 0.299 * c.r + 0.587 * c.g + 0.114 * c.b;
            float lowRes = mix(high, -1.0, step(thresholdLow, gray));
            float highRes = mix(-1.0, low, step(thresholdHigh, gray));
            float midRes = (thresholdHigh - gray) * (high - low) / (thresholdHigh - thresholdLow) + low;
            float invertedGray = mix(midRes, max(lowRes, highRes), step(-0.5, max(lowRes, highRes)));
            half3 invert = half3(invertedGray);
            return half4(invert, 1.0);
        }
    )";
    if (binarizationShaderEffect_ == nullptr) {
        binarizationShaderEffect_ = Drawing::RuntimeEffect::CreateForShader(prog);
        if (binarizationShaderEffect_ == nullptr) {
            ROSEN_LOGE("MakeBinarizationShader::RuntimeShader effect error\n");
            return nullptr;
        }
    }
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder =
        std::make_shared<Drawing::RuntimeShaderBuilder>(binarizationShaderEffect_);
    thresholdHigh = thresholdHigh <= thresholdLow ? thresholdHigh + 1e-6 : thresholdHigh;
    builder->SetChild("imageShader", imageShader);
    builder->SetUniform("low", low);
    builder->SetUniform("high", high);
    builder->SetUniform("thresholdLow", thresholdLow);
    builder->SetUniform("thresholdHigh", thresholdHigh);
    return builder->MakeShader(nullptr, false);
}
#endif

void RSPropertiesPainter::DrawLightUpEffect(const RSProperties& properties, RSPaintFilterCanvas& canvas)
{
#ifndef USE_ROSEN_DRAWING
    SkSurface* skSurface = canvas.GetSurface();
    if (skSurface == nullptr) {
        ROSEN_LOGD("RSPropertiesPainter::DrawLightUpEffect skSurface is null");
        return;
    }
    SkAutoCanvasRestore acr(&canvas, true);
    if (properties.GetClipBounds() != nullptr) {
        canvas.clipPath(properties.GetClipBounds()->GetSkiaPath(), true);
    } else {
        canvas.clipRRect(RRect2SkRRect(properties.GetRRect()), true);
    }

    auto clipBounds = canvas.getDeviceClipBounds();
    auto image = skSurface->makeImageSnapshot(clipBounds);
    if (image == nullptr) {
        ROSEN_LOGE("RSPropertiesPainter::DrawLightUpEffect image is null");
        return;
    }
    auto imageShader = image->makeShader(SkSamplingOptions(SkFilterMode::kLinear));
    auto shader = MakeLightUpEffectShader(properties.GetLightUpEffect(), imageShader);
    SkPaint paint;
    paint.setShader(shader);
    canvas.resetMatrix();
    canvas.translate(clipBounds.left(), clipBounds.top());
    canvas.drawPaint(paint);
#else
    Drawing::Surface* surface = canvas.GetSurface();
    if (surface == nullptr) {
        ROSEN_LOGD("RSPropertiesPainter::DrawLightUpEffect surface is null");
        return;
    }
    Drawing::AutoCanvasRestore acr(canvas, true);
    if (properties.GetClipBounds() != nullptr) {
        canvas.ClipPath(properties.GetClipBounds()->GetDrawingPath(), Drawing::ClipOp::INTERSECT, true);
    } else {
        canvas.ClipRoundRect(RRect2DrawingRRect(properties.GetRRect()), Drawing::ClipOp::INTERSECT, true);
    }

    auto clipBounds = canvas.GetDeviceClipBounds();
    auto image = surface->GetImageSnapshot(clipBounds);
    if (image == nullptr) {
        ROSEN_LOGE("RSPropertiesPainter::DrawLightUpEffect image is null");
        return;
    }
    Drawing::Matrix scaleMat;
    auto imageShader = Drawing::ShaderEffect::CreateImageShader(
        *image, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP,
        Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), scaleMat);
    auto shader = MakeLightUpEffectShader(properties.GetLightUpEffect(), imageShader);
    Drawing::Brush brush;
    brush.SetShaderEffect(shader);
    canvas.ResetMatrix();
    canvas.Translate(clipBounds.GetLeft(), clipBounds.GetTop());
    canvas.DrawBackground(brush);
#endif
}

#ifndef USE_ROSEN_DRAWING
sk_sp<SkShader> RSPropertiesPainter::MakeLightUpEffectShader(float lightUpDeg, sk_sp<SkShader> imageShader)
#else
std::shared_ptr<Drawing::ShaderEffect> RSPropertiesPainter::MakeLightUpEffectShader(
    float lightUpDeg, std::shared_ptr<Drawing::ShaderEffect> imageShader)
#endif
{
    static constexpr char prog[] = R"(
        uniform half lightUpDeg;
        uniform shader imageShader;
        vec3 rgb2hsv(in vec3 c)
        {
            vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
            vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
            vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));
            float d = q.x - min(q.w, q.y);
            float e = 1.0e-10;
            return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
        }
        vec3 hsv2rgb(in vec3 c)
        {
            vec3 rgb = clamp(abs(mod(c.x * 6.0 + vec3(0.0, 4.0, 2.0), 6.0) - 3.0) - 1.0, 0.0, 1.0);
            return c.z * mix(vec3(1.0), rgb, c.y);
        }
        half4 main(float2 coord)
        {
            vec3 hsv = rgb2hsv(imageShader.eval(coord).rgb);
            float satUpper = clamp(hsv.y * 1.2, 0.0, 1.0);
            hsv.y = mix(satUpper, hsv.y, lightUpDeg);
            hsv.z += lightUpDeg - 1.0;
            return vec4(hsv2rgb(hsv), 1.0);
        }
    )";
#ifndef USE_ROSEN_DRAWING
    auto [effect, err] = SkRuntimeEffect::MakeForShader(SkString(prog));
    sk_sp<SkShader> children[] = {imageShader};
    size_t childCount = 1;
    return effect->makeShader(SkData::MakeWithCopy(
        &lightUpDeg, sizeof(lightUpDeg)), children, childCount, nullptr, false);
#else
    if (lightUpEffectShaderEffect_ == nullptr) {
        lightUpEffectShaderEffect_ = Drawing::RuntimeEffect::CreateForShader(prog);
        if (lightUpEffectShaderEffect_ == nullptr) {
            return nullptr;
        }
    }
    std::shared_ptr<Drawing::ShaderEffect> children[] = {imageShader};
    size_t childCount = 1;
    auto data = std::make_shared<Drawing::Data>();
    data->BuildWithCopy(&lightUpDeg, sizeof(lightUpDeg));
    return lightUpEffectShaderEffect_->MakeShader(data, children, childCount, nullptr, false);
#endif
}

void RSPropertiesPainter::DrawDynamicLightUp(const RSProperties& properties, RSPaintFilterCanvas& canvas)
{
#ifndef USE_ROSEN_DRAWING
    SkAutoCanvasRestore acr(&canvas, true);
    if (RSSystemProperties::GetPropertyDrawableEnable()) {
        // do nothing
    } else if (properties.GetClipBounds() != nullptr) {
        canvas.clipPath(properties.GetClipBounds()->GetSkiaPath(), true);
    } else {
        canvas.clipRRect(RRect2SkRRect(properties.GetRRect()), true);
    }

    auto blender = MakeDynamicLightUpBlender(properties.GetDynamicLightUpRate().value() * canvas.GetAlpha(),
        properties.GetDynamicLightUpDegree().value() * canvas.GetAlpha());
    SkPaint paint;
    paint.setBlender(blender);
    canvas.drawPaint(paint);
#else
    Drawing::Surface* surface = canvas.GetSurface();
    if (surface == nullptr) {
        ROSEN_LOGD("RSPropertiesPainter::DrawDynamicLightUp surface is null");
        return;
    }
    Drawing::AutoCanvasRestore acr(canvas, true);
    if (RSSystemProperties::GetPropertyDrawableEnable()) {
        // do nothing
    } else if (properties.GetClipBounds() != nullptr) {
        canvas.ClipPath(properties.GetClipBounds()->GetDrawingPath(), Drawing::ClipOp::INTERSECT, true);
    } else {
        canvas.ClipRoundRect(RRect2DrawingRRect(properties.GetRRect()), Drawing::ClipOp::INTERSECT, true);
    }

    auto blender = MakeDynamicLightUpBlender(properties.GetDynamicLightUpRate().value() * canvas.GetAlpha(),
        properties.GetDynamicLightUpDegree().value() * canvas.GetAlpha());
    Drawing::Brush brush;
    brush.SetBlender(blender);
    canvas.DrawBackground(brush);
#endif
}

#ifndef USE_ROSEN_DRAWING
sk_sp<SkBlender> RSPropertiesPainter::MakeDynamicLightUpBlender(
    float dynamicLightUpRate, float dynamicLightUpDeg)
#else
std::shared_ptr<Drawing::Blender> RSPropertiesPainter::MakeDynamicLightUpBlender(
    float dynamicLightUpRate, float dynamicLightUpDeg)
#endif
{
#ifndef USE_ROSEN_DRAWING
    static constexpr char prog[] = R"(
        uniform half dynamicLightUpRate;
        uniform half dynamicLightUpDeg;

        vec4 main(vec4 src, vec4 dst) {
            float x = 0.299 * dst.r + 0.587 * dst.g + 0.114 * dst.b;
            float y = (0 - dynamicLightUpRate) * x + dynamicLightUpDeg;
            float R = clamp((dst.r + y), 0.0, 1.0);
            float G = clamp((dst.g + y), 0.0, 1.0);
            float B = clamp((dst.b + y), 0.0, 1.0);
            return vec4(R, G, B, 1.0);
        }
    )";

    auto [effect, err] = SkRuntimeEffect::MakeForBlender(SkString(prog));
    if (!effect) {
        ROSEN_LOGE("MakeDynamicLightUpBlender::RuntimeBlender effect error: %{public}s\n", err.c_str());
        return nullptr;
    }
    SkRuntimeBlendBuilder builder(effect);
    builder.uniform("dynamicLightUpRate") = dynamicLightUpRate;
    builder.uniform("dynamicLightUpDeg") = dynamicLightUpDeg;
    return builder.makeBlender();
#else
    static constexpr char prog[] = R"(
        uniform half dynamicLightUpRate;
        uniform half dynamicLightUpDeg;
        uniform shader imageShader;

        vec4 main(vec4 src, vec4 dst) {
            float x = 0.299 * dst.r + 0.587 * dst.g + 0.114 * dst.b;
            float y = (0 - dynamicLightUpRate) * x + dynamicLightUpDeg;
            float R = clamp((dst.r + y), 0.0, 1.0);
            float G = clamp((dst.g + y), 0.0, 1.0);
            float B = clamp((dst.b + y), 0.0, 1.0);
            return vec4(R, G, B, 1.0);
        }
    )";
    if (dynamicLightUpBlenderEffect_ == nullptr) {
        dynamicLightUpBlenderEffect_ = Drawing::RuntimeEffect::CreateForBlender(prog);
        if (dynamicLightUpBlenderEffect_ == nullptr) {
            ROSEN_LOGE("MakeDynamicLightUpBlender::MakeDynamicLightUpBlender effect error!\n");
            return nullptr;
        }
    }
    std::shared_ptr<Drawing::RuntimeBlenderBuilder> builder =
        std::make_shared<Drawing::RuntimeBlenderBuilder>(dynamicLightUpBlenderEffect_);
    builder->SetUniform("dynamicLightUpRate", dynamicLightUpRate);
    builder->SetUniform("dynamicLightUpDeg", dynamicLightUpDeg);
    return builder->MakeBlender();
#endif
}

void RSPropertiesPainter::DrawParticle(const RSProperties& properties, RSPaintFilterCanvas& canvas)
{
    const auto& particleVector = properties.GetParticles();
    if (particleVector.GetParticleSize() == 0) {
        return;
    }
    const auto& particles = particleVector.GetParticleVector();
    auto bounds = properties.GetDrawRegion();
    for (const auto& particle : particles) {
        if (particle != nullptr && particle->IsAlive()) {
            // Get particle properties
            auto position = particle->GetPosition();
            float opacity = particle->GetOpacity();
            float scale = particle->GetScale();
            if (!(bounds->Intersect(position.x_, position.y_)) || opacity <= 0.f || scale <= 0.f) {
                continue;
            }
            auto particleType = particle->GetParticleType();
#ifndef USE_ROSEN_DRAWING
            SkPaint paint;
            paint.setAntiAlias(true);
            paint.setAlphaf(opacity);
            auto clipBounds = SkRect::MakeXYWH(bounds->left_, bounds->top_, bounds->width_, bounds->height_);
            canvas.clipRect(clipBounds, true);
#else
            Drawing::Brush brush;
            brush.SetAntiAlias(true);
            brush.SetAlphaF(opacity);
            auto clipBounds = Drawing::Rect(
                bounds->left_, bounds->top_, bounds->left_ + bounds->width_, bounds->top_ + bounds->height_);
            canvas.ClipRect(clipBounds, Drawing::ClipOp::INTERSECT, true);
#endif

            if (particleType == ParticleType::POINTS) {
                auto radius = particle->GetRadius();
                Color color = particle->GetColor();
                auto alpha = color.GetAlpha();
                color.SetAlpha(alpha * opacity);
#ifndef USE_ROSEN_DRAWING
                paint.setColor(color.AsArgbInt());
                canvas.drawCircle(position.x_, position.y_, radius * scale, paint);
#else
                brush.SetColor(color.AsArgbInt());
                canvas.AttachBrush(brush);
                canvas.DrawCircle(Drawing::Point(position.x_, position.y_), radius * scale);
                canvas.DetachBrush();
#endif
            } else {
                auto imageSize = particle->GetImageSize();
                auto image = particle->GetImage();
                float left = position.x_;
                float top = position.y_;
                float right = position.x_ + imageSize.x_ * scale;
                float bottom = position.y_ + imageSize.y_ * scale;
#ifndef USE_ROSEN_DRAWING
                canvas.save();
                canvas.translate(position.x_, position.y_);
                canvas.rotate(particle->GetSpin(), imageSize.x_ * scale / 2.f, imageSize.y_ * scale / 2.f);
#else
                canvas.Save();
                canvas.Translate(position.x_, position.y_);
                canvas.Rotate(particle->GetSpin(), imageSize.x_ * scale / 2.f, imageSize.y_ * scale / 2.f);
#endif
                RectF destRect(left, top, right, bottom);
                image->SetDstRect(destRect);
                image->SetScale(scale);
                image->SetImageRepeat(0);
#ifndef USE_ROSEN_DRAWING
                SkRect rect { left, top, right, bottom };
                image->CanvasDrawImage(canvas, rect, SkSamplingOptions(), paint, false);
                canvas.restore();
#else
                Drawing::Rect rect { left, top, right, bottom };
                canvas.AttachBrush(brush);
                image->CanvasDrawImage(canvas, rect, Drawing::SamplingOptions(), false);
                canvas.DetachBrush();
                canvas.Restore();
#endif
            }
        }
    }
}

void RSPropertiesPainter::BeginBlendMode(RSPaintFilterCanvas& canvas, const RSProperties& properties)
{
    auto blendMode = properties.GetColorBlendMode();
    int blendModeApplyType = properties.GetColorBlendApplyType();

    if (blendMode == 0) {
        // no blend
        return;
    }

#ifndef USE_ROSEN_DRAWING
    canvas.save();
    canvas.clipRRect(RRect2SkRRect(properties.GetRRect()), true);
#else
    canvas.Save();
    canvas.ClipRoundRect(RRect2DrawingRRect(properties.GetRRect()), Drawing::ClipOp::INTERSECT, true);
#endif

    // fast blend mode
    if (blendModeApplyType == static_cast<int>(RSColorBlendApplyType::FAST)) {
        canvas.SaveBlendMode();
        canvas.SetBlendMode({ blendMode - 1 }); // map blendMode to SkBlendMode
        return;
    }

    // save layer mode
#ifndef USE_ROSEN_DRAWING
    auto matrix = canvas.getTotalMatrix();
    matrix.setTranslateX(std::ceil(matrix.getTranslateX()));
    matrix.setTranslateY(std::ceil(matrix.getTranslateY()));
    canvas.setMatrix(matrix);
    SkPaint blendPaint_;
    blendPaint_.setAlphaf(canvas.GetAlpha());
    blendPaint_.setBlendMode(static_cast<SkBlendMode>(blendMode - 1)); // map blendMode to SkBlendMode
    canvas.saveLayer(nullptr, &blendPaint_);
#else
    auto matrix = canvas.GetTotalMatrix();
    matrix.Set(Drawing::Matrix::TRANS_X, std::ceil(matrix.Get(Drawing::Matrix::TRANS_X)));
    matrix.Set(Drawing::Matrix::TRANS_Y, std::ceil(matrix.Get(Drawing::Matrix::TRANS_Y)));
    canvas.SetMatrix(matrix);
    Drawing::Brush blendBrush_;
    blendBrush_.SetAlphaF(canvas.GetAlpha());
    blendBrush_.SetBlendMode(static_cast<Drawing::BlendMode>(blendMode - 1)); // map blendMode to Drawing::BlendMode
    Drawing::SaveLayerOps maskLayerRec(nullptr, &blendBrush_, 0);
    canvas.SaveLayer(maskLayerRec);
#endif
    canvas.SaveBlendMode();
    canvas.SetBlendMode(std::nullopt);
    canvas.SaveAlpha();
    canvas.SetAlpha(1.0f);
}

void RSPropertiesPainter::EndBlendMode(RSPaintFilterCanvas& canvas, const RSProperties& properties)
{
    auto blendMode = properties.GetColorBlendMode();
    int blendModeApplyType = properties.GetColorBlendApplyType();

    if (blendMode == 0) {
        // no blend
        return;
    }

    if (blendModeApplyType == static_cast<int>(RSColorBlendApplyType::FAST)) {
        canvas.RestoreBlendMode();
    } else {
        canvas.RestoreBlendMode();
        canvas.RestoreAlpha();
#ifndef USE_ROSEN_DRAWING
        canvas.restore();
#else
        canvas.Restore();
#endif
    }
#ifndef USE_ROSEN_DRAWING
        canvas.restore();
#else
        canvas.Restore();
#endif
}
} // namespace Rosen
} // namespace OHOS
