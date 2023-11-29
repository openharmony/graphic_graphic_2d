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

#ifdef USE_ROSEN_DRAWING
#include <cstdint>

#include "draw/canvas.h"
#include "draw/clip.h"
#include "drawing/draw/core_canvas.h"
#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"
#include "recording/recording_path.h"
#include "recording/recording_shader_effect.h"
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
constexpr static float FLOAT_ZERO_THRESHOLD = 0.001f;
constexpr static uint8_t DIRECTION_NUM = 4;
} // namespace

const bool RSPropertiesPainter::BLUR_ENABLED = RSSystemProperties::GetBlurEnabled();

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
    const RRect* rrect, bool isAbsCoordinate)
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
    if (properties.shadow_->GetHardwareAcceleration()) {
        if (properties.GetShadowElevation() <= 0.f) {
            return;
        }
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
        if (paint.canComputeFastBounds()) {
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
    const RRect* rrect, bool isAbsCoordinate)
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
    if (properties.shadow_->GetHardwareAcceleration()) {
        if (properties.GetShadowElevation() <= 0.f) {
            return;
        }
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
        if (brush.CanComputeFastBounds()) {
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
        properties.shadow_->GetHardwareAcceleration()
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
        properties.shadow_->GetHardwareAcceleration()
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
    }
    int32_t fLeft = std::clamp(int(matrix.getTranslateX()), 0, deviceWidth - 1);
    int32_t fTop = std::clamp(int(matrix.getTranslateY()), 0, deviceHeight - 1);

    SkIRect regionBounds = SkIRect::MakeXYWH(fLeft, fTop, clipIBounds.width(), clipIBounds.height());
    sk_sp<SkImage> shadowRegionImage = skSurface->makeImageSnapshot(regionBounds);

    if (shadowRegionImage == nullptr) {
        return false;
    }

    // when color picker task resource is waitting for release, use color picked last frame
    if (colorPickerTask->GetWaitRelease()) {
        colorPickerTask->GetColorAverage(colorPicked);
        return;
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

    if (properties.shadow_->GetHardwareAcceleration()) {
        if (properties.GetShadowElevation() <= 0.f) {
            return;
        }
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

    // The translation of the matrix is rounded to improve the hit ratio of skia blurfilter cache,
    // the function <compute_key_and_clip_bounds> in <skia/src/gpu/GrBlurUtil.cpp> for more details.
    RSAutoCanvasRestore rst(&canvas);
    auto matrix = canvas.GetTotalMatrix();
    matrix.Set(Drawing::Matrix::TRANS_X, std::ceil(matrix.Get(Drawing::Matrix::TRANS_X)));
    matrix.Set(Drawing::Matrix::TRANS_Y, std::ceil(matrix.Get(Drawing::Matrix::TRANS_Y)));
    canvas.SetMatrix(matrix);

    if (properties.shadow_->GetHardwareAcceleration()) {
        if (properties.GetShadowElevation() <= 0.f) {
            return;
        }
        Drawing::Point3 planeParams = { 0.0f, 0.0f, properties.GetShadowElevation() };
        Drawing::scalar centerX = path.GetBounds().GetLeft() + path.GetBounds().GetWidth() / 2;
        Drawing::scalar centerY = path.GetBounds().GetTop() + path.GetBounds().GetHeight() / 2;
        Drawing::Point3 lightPos = {
            canvas.GetTotalMatrix().Get(Drawing::Matrix::TRANS_X) + centerX,
            canvas.GetTotalMatrix().Get(Drawing::Matrix::TRANS_Y) + centerY, DEFAULT_LIGHT_HEIGHT };
        Color ambientColor = Color::FromArgbInt(DEFAULT_AMBIENT_COLOR);
        ambientColor.MultiplyAlpha(canvas.GetAlpha());
        spotColor.MultiplyAlpha(canvas.GetAlpha());
        canvas.DrawShadow(path, planeParams, lightPos, DEFAULT_LIGHT_RADIUS,
            Drawing::Color(ambientColor.AsArgbInt()), Drawing::Color(spotColor.AsArgbInt()),
            Drawing::ShadowFlags::TRANSPARENT_OCCLUDER);
    } else {
        Drawing::Brush brush;
        brush.SetColor(Drawing::Color(spotColor.AsArgbInt()));
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

void RSPropertiesPainter::TransformGradientBlurDirection(uint8_t& direction, const uint8_t directionBias)
{
    if (direction == static_cast<uint8_t>(GradientDirection::LEFT_BOTTOM)) {
        direction += 2; // 2 is used to transtorm diagnal direction.
    } else if (direction == static_cast<uint8_t>(GradientDirection::RIGHT_TOP) ||
                    direction == static_cast<uint8_t>(GradientDirection::RIGHT_BOTTOM)) {
        direction -= 1; // 1 is used to transtorm diagnal direction.
    }
    if (direction <= static_cast<uint8_t>(GradientDirection::BOTTOM)) {
        if (direction < directionBias) {
            direction += DIRECTION_NUM;
        }
        direction -= directionBias;
    } else {
        direction -= DIRECTION_NUM;
        if (direction < directionBias) {
            direction += DIRECTION_NUM;
        }
        direction -= directionBias;
        direction += DIRECTION_NUM;
    }
    if (direction == static_cast<uint8_t>(GradientDirection::RIGHT_BOTTOM)) {
        direction -= 2; // 2 is used to restore diagnal direction.
    } else if (direction == static_cast<uint8_t>(GradientDirection::LEFT_BOTTOM) ||
                        direction == static_cast<uint8_t>(GradientDirection::RIGHT_TOP)) {
        direction += 1; // 1 is used to restore diagnal direction.
    }
}

#ifndef USE_ROSEN_DRAWING
bool RSPropertiesPainter::GetGradientDirectionPoints(
    SkPoint (&pts)[2], const SkRect& clipBounds, GradientDirection direction)
#else
bool RSPropertiesPainter::GetGradientDirectionPoints(
    Drawing::Point (&pts)[2], const Drawing::Rect& clipBounds, GradientDirection direction)
#endif
{
    switch (direction) {
        case GradientDirection::BOTTOM: {
#ifndef USE_ROSEN_DRAWING
            pts[0].set(clipBounds.width() / 2, 0); // 2 represents middle of width;
            pts[1].set(clipBounds.width() / 2, clipBounds.height()); // 2 represents middle of width;
#else
            pts[0].Set(clipBounds.GetWidth() / 2, 0); // 2 represents middle of width;
            pts[1].Set(clipBounds.GetWidth() / 2, clipBounds.GetHeight()); // 2 represents middle of width;
#endif
            break;
        }
        case GradientDirection::TOP: {
#ifndef USE_ROSEN_DRAWING
            pts[0].set(clipBounds.width() / 2, clipBounds.height()); // 2 represents middle of width;
            pts[1].set(clipBounds.width() / 2, 0); // 2 represents middle of width;
#else
            pts[0].Set(clipBounds.GetWidth() / 2, clipBounds.GetHeight()); // 2 represents middle of width;
            pts[1].Set(clipBounds.GetWidth() / 2, 0); // 2 represents middle of width;
#endif
            break;
        }
        case GradientDirection::RIGHT: {
#ifndef USE_ROSEN_DRAWING
            pts[0].set(0, clipBounds.height() / 2); // 2 represents middle of height;
            pts[1].set(clipBounds.width(), clipBounds.height() / 2); // 2 represents middle of height;
#else
            pts[0].Set(0, clipBounds.GetHeight() / 2); // 2 represents middle of height;
            pts[1].Set(clipBounds.GetWidth(), clipBounds.GetHeight() / 2); // 2 represents middle of height;
#endif
            break;
        }
        case GradientDirection::LEFT: {
#ifndef USE_ROSEN_DRAWING
            pts[0].set(clipBounds.width(), clipBounds.height() / 2); // 2 represents middle of height;
            pts[1].set(0, clipBounds.height() / 2); // 2 represents middle of height;
#else
            pts[0].Set(clipBounds.GetWidth(), clipBounds.GetHeight() / 2); // 2 represents middle of height;
            pts[1].Set(0, clipBounds.GetHeight() / 2); // 2 represents middle of height;
#endif
            break;
        }
        case GradientDirection::RIGHT_BOTTOM: {
#ifndef USE_ROSEN_DRAWING
            pts[0].set(0, 0);
            pts[1].set(clipBounds.width(), clipBounds.height());
#else
            pts[0].Set(0, 0);
            pts[1].Set(clipBounds.GetWidth(), clipBounds.GetHeight());
#endif
            break;
        }
        case GradientDirection::LEFT_TOP: {
#ifndef USE_ROSEN_DRAWING
            pts[0].set(clipBounds.width(), clipBounds.height());
            pts[1].set(0, 0);
#else
            pts[0].Set(clipBounds.GetWidth(), clipBounds.GetHeight());
            pts[1].Set(0, 0);
#endif
            break;
        }
        case GradientDirection::LEFT_BOTTOM: {
#ifndef USE_ROSEN_DRAWING
            pts[0].set(clipBounds.width(), 0);
            pts[1].set(0, clipBounds.height());
#else
            pts[0].Set(clipBounds.GetWidth(), 0);
            pts[1].Set(0, clipBounds.GetHeight());
#endif
            break;
        }
        case GradientDirection::RIGHT_TOP: {
#ifndef USE_ROSEN_DRAWING
            pts[0].set(0, clipBounds.height());
            pts[1].set(clipBounds.width(), 0);
#else
            pts[0].Set(0, clipBounds.GetHeight());
            pts[1].Set(clipBounds.GetWidth(), 0);
#endif
            break;
        }
        default: {
            return false;
        }
    }
    return true;
}

#ifndef USE_ROSEN_DRAWING
sk_sp<SkShader> RSPropertiesPainter::MakeAlphaGradientShader(
    const SkRect& clipBounds, const std::shared_ptr<RSLinearGradientBlurPara>& para, uint8_t directionBias)
{
    std::vector<SkColor> c;
    std::vector<SkScalar> p;
    SkPoint pts[2];
#else
std::shared_ptr<Drawing::ShaderEffect> RSPropertiesPainter::MakeAlphaGradientShader(
    const Drawing::Rect& clipBounds, const std::shared_ptr<RSLinearGradientBlurPara>& para, uint8_t directionBias)
{
    std::vector<Drawing::ColorQuad> c;
    std::vector<Drawing::scalar> p;
    Drawing::Point pts[2];
#endif

    uint8_t direction = static_cast<uint8_t>(para->direction_);
    if (directionBias != 0) {
        TransformGradientBlurDirection(direction, directionBias);
    }
    bool result = GetGradientDirectionPoints(
        pts, clipBounds, static_cast<GradientDirection>(direction));
    if (!result) {
        return nullptr;
    }
    uint8_t ColorMax = 255;
    uint8_t ColorMin = 0;
    if (para->fractionStops_[0].second > 0.01) {  // 0.01 represents the fraction bias
#ifndef USE_ROSEN_DRAWING
        c.emplace_back(SkColorSetARGB(ColorMin, ColorMax, ColorMax, ColorMax));
#else
        c.emplace_back(Drawing::Color::ColorQuadSetARGB(ColorMin, ColorMax, ColorMax, ColorMax));
#endif
        p.emplace_back(para->fractionStops_[0].second - 0.01); // 0.01 represents the fraction bias
    }
    for (size_t i = 0; i < para->fractionStops_.size(); i++) {
#ifndef USE_ROSEN_DRAWING
        c.emplace_back(SkColorSetARGB(
            static_cast<uint8_t>(para->fractionStops_[i].first * ColorMax), ColorMax, ColorMax, ColorMax));
#else
        c.emplace_back(Drawing::Color::ColorQuadSetARGB(
            static_cast<uint8_t>(para->fractionStops_[i].first * ColorMax), ColorMax, ColorMax, ColorMax));
#endif
        p.emplace_back(para->fractionStops_[i].second);
    }
    // 0.01 represents the fraction bias
    if (para->fractionStops_[para->fractionStops_.size() - 1].second < (1 - 0.01)) {
#ifndef USE_ROSEN_DRAWING
        c.emplace_back(SkColorSetARGB(ColorMin, ColorMax, ColorMax, ColorMax));
#else
        c.emplace_back(Drawing::Color::ColorQuadSetARGB(ColorMin, ColorMax, ColorMax, ColorMax));
#endif
        // 0.01 represents the fraction bias
        p.emplace_back(para->fractionStops_[para->fractionStops_.size() - 1].second + 0.01);
    }
#ifndef USE_ROSEN_DRAWING
    auto shader = SkGradientShader::MakeLinear(pts, &c[0], &p[0], p.size(), SkTileMode::kClamp);
    return shader;
#else
    return Drawing::ShaderEffect::CreateLinearGradient(pts[0], pts[1], c, p, Drawing::TileMode::CLAMP);
#endif
}

#ifndef USE_ROSEN_DRAWING
sk_sp<SkShader> RSPropertiesPainter::MakeHorizontalMeanBlurShader(
    float radiusIn, sk_sp<SkShader> shader, sk_sp<SkShader> gradientShader)
#else
std::shared_ptr<Drawing::ShaderEffect> RSPropertiesPainter::MakeHorizontalMeanBlurShader(float radiusIn,
    std::shared_ptr<Drawing::ShaderEffect> shader, std::shared_ptr<Drawing::ShaderEffect> gradientShader)
#endif
{
    const char* prog = R"(
        uniform half r;
        uniform shader imageShader;
        uniform shader gradientShader;
        half4 meanFilter(float2 coord, half radius)
        {
            half4 sum = vec4(0.0);
            half div = 0;
            for (half x = -30.0; x < 30.0; x += 1.0) {
                if (x > radius) {
                    break;
                }
                if (abs(x) < radius) {
                    div += 1;
                    sum += imageShader.eval(coord + float2(x, 0));
                }
            }
            return half4(sum.xyz / div, 1.0);
        }
        half4 main(float2 coord)
        {
            if (abs(gradientShader.eval(coord).a - 0) < 0.001) {
                return imageShader.eval(coord);
            }
            float val = clamp(r * gradientShader.eval(coord).a, 1.0, r);
            return meanFilter(coord, val);
        }
    )";
#ifndef USE_ROSEN_DRAWING
    auto [effect, err] = SkRuntimeEffect::MakeForShader(SkString(prog));
    sk_sp<SkShader> children[] = {shader, gradientShader};
    size_t childCount = 2;
    return effect->makeShader(SkData::MakeWithCopy(
        &radiusIn, sizeof(radiusIn)), children, childCount, nullptr, false);
#else
    std::shared_ptr<Drawing::RuntimeEffect> effect = Drawing::RuntimeEffect::CreateForShader(prog);
    std::shared_ptr<Drawing::ShaderEffect> children[] = {shader, gradientShader};
    size_t childCount = 2;
    std::shared_ptr<Drawing::Data> data = std::make_shared<Drawing::Data>();
    data->BuildWithCopy(&radiusIn, sizeof(radiusIn));
    return effect->MakeShader(data, children, childCount, nullptr, false);
#endif
}

#ifndef USE_ROSEN_DRAWING
sk_sp<SkShader> RSPropertiesPainter::MakeVerticalMeanBlurShader(
    float radiusIn, sk_sp<SkShader> shader, sk_sp<SkShader> gradientShader)
#else
std::shared_ptr<Drawing::ShaderEffect> RSPropertiesPainter::MakeVerticalMeanBlurShader(float radiusIn,
    std::shared_ptr<Drawing::ShaderEffect> shader, std::shared_ptr<Drawing::ShaderEffect> gradientShader)
#endif
{
    const char* prog = R"(
        uniform half r;
        uniform shader imageShader;
        uniform shader gradientShader;
        half4 meanFilter(float2 coord, half radius)
        {
            half4 sum = vec4(0.0);
            half div = 0;
            for (half y = -30.0; y < 30.0; y += 1.0) {
                if (y > radius) {
                    break;
                }
                if (abs(y) < radius) {
                    div += 1;
                    sum += imageShader.eval(coord + float2(0, y));
                }
            }
            return half4(sum.xyz / div, 1.0);
        }
        half4 main(float2 coord)
        {
            if (abs(gradientShader.eval(coord).a - 0) < 0.001) {
                return imageShader.eval(coord);
            }
            float val = clamp(r * gradientShader.eval(coord).a, 1.0, r);
            return meanFilter(coord, val);
        }
    )";
#ifndef USE_ROSEN_DRAWING
    auto [effect, err] = SkRuntimeEffect::MakeForShader(SkString(prog));
    sk_sp<SkShader> children[] = {shader, gradientShader};
    size_t childCount = 2;
    return effect->makeShader(SkData::MakeWithCopy(
        &radiusIn, sizeof(radiusIn)), children, childCount, nullptr, false);
#else
    std::shared_ptr<Drawing::RuntimeEffect> effect = Drawing::RuntimeEffect::CreateForShader(prog);
    std::shared_ptr<Drawing::ShaderEffect> children[] = {shader, gradientShader};
    size_t childCount = 2;
    std::shared_ptr<Drawing::Data> data = std::make_shared<Drawing::Data>();
    data->BuildWithCopy(&radiusIn, sizeof(radiusIn));
    return effect->MakeShader(data, children, childCount, nullptr, false);
#endif
}

#ifndef USE_ROSEN_DRAWING
void RSPropertiesPainter::DrawHorizontalLinearGradientBlur(SkSurface* skSurface, RSPaintFilterCanvas& canvas,
    float radius, sk_sp<SkShader> alphaGradientShader, const SkIRect& clipIPadding)
{
    auto image = skSurface->makeImageSnapshot(clipIPadding);
    if (image == nullptr) {
        return;
    }
    auto imageShader = image->makeShader(SkSamplingOptions(SkFilterMode::kLinear));
    auto shader = MakeHorizontalMeanBlurShader(radius, imageShader, alphaGradientShader);
    SkPaint paint;
    paint.setShader(shader);
    canvas.drawRect(SkRect::Make(clipIPadding.makeOffset(-clipIPadding.left(), -clipIPadding.top())), paint);
}
#else
void RSPropertiesPainter::DrawHorizontalLinearGradientBlur(Drawing::Surface* surface, RSPaintFilterCanvas& canvas,
    float radius, std::shared_ptr<Drawing::ShaderEffect> alphaGradientShader, const Drawing::RectI& clipIPadding)
{
    auto image = surface->GetImageSnapshot(clipIPadding);
    if (image == nullptr) {
        return;
    }
    Drawing::Matrix m;
    auto imageShader = Drawing::ShaderEffect::CreateImageShader(
        *image, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP,
        Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), m);
    auto shader = MakeHorizontalMeanBlurShader(radius, imageShader, alphaGradientShader);
    Drawing::Brush brush;
    brush.SetShaderEffect(shader);
    canvas.AttachBrush(brush);
    Drawing::Rect rect = clipIPadding;
    rect.Offset(-clipIPadding.GetLeft(), -clipIPadding.GetTop());
    canvas.DrawRect(rect);
    canvas.DetachBrush();
}
#endif

#ifndef USE_ROSEN_DRAWING
void RSPropertiesPainter::DrawVerticalLinearGradientBlur(SkSurface* skSurface, RSPaintFilterCanvas& canvas,
    float radius, sk_sp<SkShader> alphaGradientShader, const SkIRect& clipIPadding)
{
    auto image = skSurface->makeImageSnapshot(clipIPadding);
    if (image == nullptr) {
        return;
    }
    auto imageShader = image->makeShader(SkSamplingOptions(SkFilterMode::kLinear));
    auto shader = MakeVerticalMeanBlurShader(radius, imageShader, alphaGradientShader);
    SkPaint paint;
    paint.setShader(shader);
    canvas.drawRect(SkRect::Make(clipIPadding.makeOffset(-clipIPadding.left(), -clipIPadding.top())), paint);
}
#else
void RSPropertiesPainter::DrawVerticalLinearGradientBlur(Drawing::Surface* surface, RSPaintFilterCanvas& canvas,
    float radius, std::shared_ptr<Drawing::ShaderEffect> alphaGradientShader, const Drawing::RectI& clipIPadding)
{
    auto image = surface->GetImageSnapshot(clipIPadding);
    if (image == nullptr) {
        return;
    }
    Drawing::Matrix m;
    auto imageShader = Drawing::ShaderEffect::CreateImageShader(
        *image, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP,
        Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), m);
    auto shader = MakeVerticalMeanBlurShader(radius, imageShader, alphaGradientShader);
    Drawing::Brush brush;
    brush.SetShaderEffect(shader);
    canvas.AttachBrush(brush);
    Drawing::Rect rect = clipIPadding;
    rect.Offset(-clipIPadding.GetLeft(), -clipIPadding.GetTop());
    canvas.DrawRect(rect);
    canvas.DetachBrush();
}
#endif

#ifndef USE_ROSEN_DRAWING
uint8_t RSPropertiesPainter::CalcDirectionBias(const SkMatrix& mat)
{
    uint8_t directionBias = 0;
    // 1 and 3 represents rotate matrix's index
    if ((mat.get(1) > FLOAT_ZERO_THRESHOLD) && (mat.get(3) < (0 - FLOAT_ZERO_THRESHOLD))) {
        directionBias = 1; // 1 represents rotate 90 degree
    // 0 and 4 represents rotate matrix's index
    } else if ((mat.get(0) < (0 - FLOAT_ZERO_THRESHOLD)) && (mat.get(4) < (0 - FLOAT_ZERO_THRESHOLD))) {
        directionBias = 2; // 2 represents rotate 180 degree
    // 1 and 3 represents rotate matrix's index
    } else if ((mat.get(1) < (0 - FLOAT_ZERO_THRESHOLD)) && (mat.get(3) > FLOAT_ZERO_THRESHOLD)) {
        directionBias = 3; // 3 represents rotate 270 degree
    }
    return directionBias;
}
#else
uint8_t RSPropertiesPainter::CalcDirectionBias(const Drawing::Matrix& mat)
{
    uint8_t directionBias = 0;
    // 1 and 3 represents rotate matrix's index
    if ((mat.Get(1) > FLOAT_ZERO_THRESHOLD) && (mat.Get(3) < (0 - FLOAT_ZERO_THRESHOLD))) {
        directionBias = 1; // 1 represents rotate 90 degree
    // 0 and 4 represents rotate matrix's index
    } else if ((mat.Get(0) < (0 - FLOAT_ZERO_THRESHOLD)) && (mat.Get(4) < (0 - FLOAT_ZERO_THRESHOLD))) {
        directionBias = 2; // 2 represents rotate 180 degree
    // 1 and 3 represents rotate matrix's index
    } else if ((mat.Get(1) < (0 - FLOAT_ZERO_THRESHOLD)) && (mat.Get(3) > FLOAT_ZERO_THRESHOLD)) {
        directionBias = 3; // 3 represents rotate 270 degree
    }
    return directionBias;
}
#endif

#ifndef USE_ROSEN_DRAWING
void RSPropertiesPainter::DrawLinearGradientBlurFilter(
    const RSProperties& properties, RSPaintFilterCanvas& canvas, const std::optional<SkRect>& rect)
#else
void RSPropertiesPainter::DrawLinearGradientBlurFilter(
    const RSProperties& properties, RSPaintFilterCanvas& canvas, const std::optional<Drawing::Rect>& rect)
#endif
{
    const auto& para = properties.GetLinearGradientBlurPara();
    if (para == nullptr || para->blurRadius_ <= 0) {
        ROSEN_LOGE("RSPropertiesPainter::DrawLinearGradientBlurFilter para invalid");
        return;
    }
    RS_TRACE_NAME("DrawLinearGradientBlurFilter");
#ifndef USE_ROSEN_DRAWING
    SkSurface* surface = canvas.GetSurface();
    if (surface == nullptr) {
        return;
    }
    SkAutoCanvasRestore acr(&canvas, true);
    if (RSSystemProperties::GetPropertyDrawableEnable()) {
        // do nothing
    } else if (rect.has_value()) {
        canvas.clipRect((*rect), true);
    } else if (properties.GetClipBounds() != nullptr) {
        canvas.clipPath(properties.GetClipBounds()->GetSkiaPath(), true);
    } else { // we always do clip for DrawLinearGradientBlurFilter, even if ClipToBounds is false
        canvas.clipRRect(RRect2SkRRect(properties.GetRRect()), true);
    }

    auto clipBounds = canvas.getDeviceClipBounds();
    auto clipIPadding = clipBounds.makeOutset(-1, -1);
    SkMatrix mat = canvas.getTotalMatrix();
#else
    Drawing::Surface* surface = canvas.GetSurface();
    if (surface == nullptr) {
        return;
    }
    Drawing::AutoCanvasRestore acr(canvas, true);
    if (RSSystemProperties::GetPropertyDrawableEnable()) {
        // do nothing
    } else if (rect.has_value()) {
        canvas.ClipRect((*rect), Drawing::ClipOp::INTERSECT, true);
    } else if (properties.GetClipBounds() != nullptr) {
        canvas.ClipPath(properties.GetClipBounds()->GetDrawingPath(), Drawing::ClipOp::INTERSECT, true);
    } else { // we always do clip for DrawLinearGradientBlurFilter, even if ClipToBounds is false
        canvas.ClipRoundRect(RRect2DrawingRRect(properties.GetRRect()), Drawing::ClipOp::INTERSECT, true);
    }

    auto clipBounds = canvas.GetDeviceClipBounds();
    auto clipIPadding = clipBounds;
    clipBounds.MakeOutset(-1, -1);
    Drawing::Matrix mat = canvas.GetTotalMatrix();
#endif
    uint8_t directionBias = CalcDirectionBias(mat);

#ifndef USE_ROSEN_DRAWING
    auto alphaGradientShader = MakeAlphaGradientShader(SkRect::Make(clipIPadding), para, directionBias);
#else
    auto alphaGradientShader = MakeAlphaGradientShader(clipIPadding, para, directionBias);
#endif
    if (alphaGradientShader == nullptr) {
        ROSEN_LOGE("RSPropertiesPainter::DrawLinearGradientBlurFilter alphaGradientShader null");
        return;
    }

    RS_TRACE_NAME("DrawLinearBlur");
#ifndef USE_ROSEN_DRAWING
    canvas.resetMatrix();
    canvas.translate(clipIPadding.left(), clipIPadding.top());
#else
    canvas.ResetMatrix();
    canvas.Translate(clipIPadding.GetLeft(), clipIPadding.GetTop());
#endif

    DrawLinearGradientBlur(surface, canvas, para, alphaGradientShader, clipIPadding);
}

#ifndef USE_ROSEN_DRAWING
void RSPropertiesPainter::DrawLinearGradientBlur(SkSurface* surface, RSPaintFilterCanvas& canvas,
    const std::shared_ptr<RSLinearGradientBlurPara>& para, sk_sp<SkShader> alphaGradientShader,
    const SkIRect& clipIPadding)
#else
void RSPropertiesPainter::DrawLinearGradientBlur(Drawing::Surface* surface, RSPaintFilterCanvas& canvas,
    const std::shared_ptr<RSLinearGradientBlurPara>& para, std::shared_ptr<Drawing::ShaderEffect> alphaGradientShader,
    const Drawing::RectI& clipIPadding)
#endif
{
    if (para == nullptr) {
        ROSEN_LOGE("RSPropertiesPainter::DrawLinearGradientBlur para null");
        return;
    }

    if (RSSystemProperties::GetMaskLinearBlurEnabled()) {
        // use faster LinearGradientBlur if valid
        if (para->LinearGradientBlurFilter_ == nullptr) {
            ROSEN_LOGE("RSPropertiesPainter::DrawLinearGradientBlur blurFilter null");
            return;
        }
        auto& RSFilter = para->LinearGradientBlurFilter_;
        auto filter = std::static_pointer_cast<RSSkiaFilter>(RSFilter);
        DrawMaskLinearGradientBlur(surface, canvas, filter, alphaGradientShader, clipIPadding);
    } else {
        // use original LinearGradientBlur
        float radius = para->blurRadius_ / 2;
        DrawHorizontalLinearGradientBlur(surface, canvas, radius, alphaGradientShader, clipIPadding);
        DrawVerticalLinearGradientBlur(surface, canvas, radius, alphaGradientShader, clipIPadding);
        DrawHorizontalLinearGradientBlur(surface, canvas, radius, alphaGradientShader, clipIPadding);
        DrawVerticalLinearGradientBlur(surface, canvas, radius, alphaGradientShader, clipIPadding);
    }
}

#ifndef USE_ROSEN_DRAWING
void RSPropertiesPainter::DrawMaskLinearGradientBlur(SkSurface* skSurface, RSPaintFilterCanvas& canvas,
    std::shared_ptr<RSSkiaFilter>& blurFilter, sk_sp<SkShader> alphaGradientShader, const SkIRect& clipIPadding)
{
    auto image = skSurface->makeImageSnapshot(clipIPadding);
    if (image == nullptr) {
        ROSEN_LOGE("RSPropertiesPainter::DrawMaskLinearGradientBlur image is null");
        return;
    }

    auto offscreenRect = clipIPadding;
    auto offscreenSurface = skSurface->makeSurface(offscreenRect.width(), offscreenRect.height());
    RSPaintFilterCanvas offscreenCanvas(offscreenSurface.get());

    blurFilter->DrawImageRect(offscreenCanvas, image, SkRect::Make(image->bounds().makeOutset(-1, -1)),
        SkRect::Make(image->bounds()));
    auto filteredSnapshot = offscreenSurface->makeImageSnapshot();
    auto srcImageShader = image->makeShader(SkSamplingOptions(SkFilterMode::kLinear));
    auto blurImageShader = filteredSnapshot->makeShader(SkSamplingOptions(SkFilterMode::kLinear));
    auto shader = MakeMeanBlurShader(srcImageShader, blurImageShader, alphaGradientShader);

    SkPaint paint;
    paint.setShader(shader);
    canvas.drawRect(SkRect::Make(clipIPadding.makeOffset(-clipIPadding.left(), -clipIPadding.top())), paint);
}
#else
void RSPropertiesPainter::DrawMaskLinearGradientBlur(Drawing::Surface* surface, RSPaintFilterCanvas& canvas,
    std::shared_ptr<RSDrawingFilter>& blurFilter, std::shared_ptr<Drawing::ShaderEffect> alphaGradientShader,
    const Drawing::RectI& clipIPadding)
{
}
#endif

#ifndef USE_ROSEN_DRAWING
sk_sp<SkShader> RSPropertiesPainter::MakeMeanBlurShader(sk_sp<SkShader> srcImageShader,
        sk_sp<SkShader> blurImageShader, sk_sp<SkShader> gradientShader)
#else
std::shared_ptr<Drawing::ShaderEffect> RSPropertiesPainter::MakeMeanBlurShader(
    std::shared_ptr<Drawing::ShaderEffect> srcImageShader, std::shared_ptr<Drawing::ShaderEffect> blurImageShader,
    std::shared_ptr<Drawing::ShaderEffect> gradientShader)
#endif
{
    const char* prog = R"(
        uniform shader srcImageShader;
        uniform shader blurImageShader;
        uniform shader gradientShader;
        half4 meanFilter(float2 coord)
        {
            vec3 srcColor = vec3(srcImageShader.eval(coord).r,
                srcImageShader.eval(coord).g, srcImageShader.eval(coord).b);
            vec3 blurColor = vec3(blurImageShader.eval(coord).r,
                blurImageShader.eval(coord).g, blurImageShader.eval(coord).b);
            float gradient = gradientShader.eval(coord).a;

            vec3 color = blurColor * gradient + srcColor * (1 - gradient);
            return vec4(color, 1.0);
        }
        half4 main(float2 coord)
        {
            if (abs(gradientShader.eval(coord).a) < 0.001) {
                return srcImageShader.eval(coord);
            }
            
            if (abs(gradientShader.eval(coord).a) > 0.999) {
                return blurImageShader.eval(coord);
            }

            return meanFilter(coord);
        }
    )";

#ifndef USE_ROSEN_DRAWING
    auto [effect, err] = SkRuntimeEffect::MakeForShader(SkString(prog));
    if (!effect) {
        ROSEN_LOGE("MakeMeanBlurShader::RuntimeShader effect error: %{public}s\n", err.c_str());
        return nullptr;
    }

    SkRuntimeShaderBuilder builder(effect);
    builder.child("srcImageShader") = srcImageShader;
    builder.child("blurImageShader") = blurImageShader;
    builder.child("gradientShader") = gradientShader;
    return builder.makeShader(nullptr, false);
#else
    std::shared_ptr<Drawing::RuntimeEffect> effect = Drawing::RuntimeEffect::CreateForShader(prog);
    if (!effect) {
        ROSEN_LOGE("MakeMeanBlurShader::RuntimeShader effect error\n");
        return nullptr;
    }

    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder = std::make_shared<Drawing::RuntimeShaderBuilder>(effect);
    builder->SetChild("srcImageShader", srcImageShader);
    builder->SetChild("blurImageShader", blurImageShader);
    builder->SetChild("gradientShader", gradientShader);
    return builder->MakeShader(nullptr, false);
#endif
}

sk_sp<SkShader> RSPropertiesPainter::MakeGreyAdjustmentShader(const float coef1, const float coef2,
    sk_sp<SkShader> imageShader)
{
    SkString GrayGradationString(R"(
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
            if (rgb < 127.5) {
                return (rgb + coefficient1 * pow((1 - t_r), 3));
            } else {
                return (rgb - coefficient2 * pow((1 - t_r), 3));
            }
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
    auto [GrayAdjustEffect, GrayAdjustError] = SkRuntimeEffect::MakeForShader(GrayGradationString);

    SkRuntimeShaderBuilder builder(GrayAdjustEffect);
    builder.child("imageShader") = imageShader;
    builder.uniform("coefficient1") = coef1;
    builder.uniform("coefficient2") = coef2;
    auto grayShader = builder.makeShader(nullptr, false);
    return grayShader;
}

void RSPropertiesPainter::DrawGreyAdjustment(const RSProperties& properties, RSPaintFilterCanvas& canvas)
{
    RS_TRACE_NAME("RSPropertiesPainter::DrawGreyAdjustment");
    auto skSurface = canvas.GetSurface();
    if (skSurface == nullptr) {
        ROSEN_LOGE("RSPropertiesPainter::DrawGreyAdjustment skSurface null");
        return;
    }
    auto clipBounds = canvas.getDeviceClipBounds();
    auto imageSnapshot = skSurface->makeImageSnapshot(clipBounds);
    if (imageSnapshot == nullptr) {
        ROSEN_LOGE("RSPropertiesPainter::DrawGreyAdjustment image is null");
        return;
    }
    auto imageShader = imageSnapshot->makeShader(SkSamplingOptions(SkFilterMode::kLinear));
    float coef1 = properties.GetGreyCoef1();
    float coef2 = properties.GetGreyCoef2();

    RS_TRACE_NAME("coef1" + std::to_string(coef1) + " coef2" + std::to_string(coef2));
    auto grayedImageShader = MakeGreyAdjustmentShader(coef1, coef2, imageShader);
    
    SkPaint paint;
    paint.setShader(grayedImageShader);
    canvas.resetMatrix();
    canvas.translate(clipBounds.left(), clipBounds.top());
    canvas.drawPaint(paint);
}

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

    if (properties.IsGreyAdjustmenValid()) {
        DrawGreyAdjustment(properties, canvas);
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
    if (needSnapshotOutset) {
        clipIBounds.MakeOutset(-1, -1);
    }
    auto imageSnapshot = surface->GetImageSnapshot(clipIBounds);
#endif
    if (imageSnapshot == nullptr) {
        ROSEN_LOGE("RSPropertiesPainter::DrawFilter image null");
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
        as_IB(imageSnapshot->ExportSkImage().get())->hintCacheGpuResource();
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

#ifndef USE_ROSEN_DRAWING
void RSPropertiesPainter::DrawBackgroundEffect(
    const RSProperties& properties, RSPaintFilterCanvas& canvas, const SkRect& rect)
#else
void RSPropertiesPainter::DrawBackgroundEffect(
    const RSProperties& properties, RSPaintFilterCanvas& canvas, const Drawing::Rect& rect)
#endif
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
    SkAutoCanvasRestore acr(&canvas, true);
    canvas.clipRect(rect);
    auto filter = std::static_pointer_cast<RSSkiaFilter>(RSFilter);
#else
    Drawing::AutoCanvasRestore acr(canvas, true);
    canvas.ClipRect(rect);
    auto filter = std::static_pointer_cast<RSDrawingFilter>(RSFilter);
#endif

#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    // Optional use cacheManager to draw filter
    if (auto& cacheManager = properties.GetFilterCacheManager(false);
        cacheManager != nullptr && !canvas.GetDisableFilterCache()) {
        auto&& data = cacheManager->GeneratedCachedEffectData(canvas, filter);
        canvas.SetEffectData(data);
        return;
    }
#endif

#ifndef USE_ROSEN_DRAWING
    auto imageRect = canvas.getDeviceClipBounds();
    auto imageSnapshot = surface->makeImageSnapshot(imageRect);
#else
    auto imageRect = canvas.GetDeviceClipBounds();
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
    auto parentNode = properties.backref_.lock();
    while (parentNode && !parentNode->IsInstanceOf<RSEffectRenderNode>()) {
        parentNode = parentNode->GetParent().lock();
    }
    if (!parentNode) {
        ROSEN_LOGE("RSPropertiesPainter::ApplyBackgroundEffectFallback: parentNode null, draw filter failed.");
        return;
    }
    auto& filter = parentNode->GetRenderProperties().GetBackgroundFilter();
    if (filter == nullptr || !filter->IsValid()) {
        ROSEN_LOGE("RSPropertiesPainter::ApplyBackgroundEffectFallback: parent EffectRenderNode has no filter, "
                   "draw filter failed.");
        return;
    }
    DrawFilter(properties, canvas, FilterType::BACKGROUND_FILTER, std::nullopt, filter);
}

void RSPropertiesPainter::ApplyBackgroundEffect(const RSProperties& properties, RSPaintFilterCanvas& canvas)
{
    const auto& effectData = canvas.GetEffectData();
    if (effectData == nullptr || effectData->cachedImage_ == nullptr) {
        // no effectData available, draw background filter in fallback method
        ROSEN_LOGD("RSPropertiesPainter::ApplyBackgroundEffect: effectData null, try fallback method.");
        ApplyBackgroundEffectFallback(properties, canvas);
        return;
    }
    RS_TRACE_NAME("ApplyBackgroundEffect");
#ifndef USE_ROSEN_DRAWING
    SkAutoCanvasRestore acr(&canvas, true);
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

    SkPaint defaultPaint;
    // dstRect: canvas clip region
    auto dstRect = SkRect::Make(canvas.getDeviceClipBounds());
    // srcRect: map dstRect onto cache coordinate
    auto srcRect = dstRect.makeOffset(-effectData->cachedRect_.left(), -effectData->cachedRect_.top());
    canvas.drawImageRect(effectData->cachedImage_, srcRect, dstRect, SkSamplingOptions(), &defaultPaint,
        SkCanvas::kFast_SrcRectConstraint);
#else
    Drawing::AutoCanvasRestore acr(canvas, true);
    if (properties.GetClipBounds() != nullptr) {
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
        tmpBounds.GetLeft(), tmpBounds.GetTop(), tmpBounds.GetWidth() - 1, tmpBounds.GetHeight() - 1);
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
    Drawing::Matrix inverseMat, scaleMat;
    auto boundsGeo = (properties.GetBoundsGeometry());
    if (boundsGeo && !boundsGeo->IsEmpty()) {
        if (!canvas.GetTotalMatrix().Invert(inverseMat)) {
            ROSEN_LOGE("RSPropertiesPainter::DrawPixelStretch get inverse matrix failed.");
        }
        scaleMat.Set(Drawing::Matrix::SCALE_X, inverseMat.Get(Drawing::Matrix::SCALE_X));
        scaleMat.Set(Drawing::Matrix::SCALE_Y, inverseMat.Get(Drawing::Matrix::SCALE_Y));
    }

    canvas.Save();
    canvas.Translate(bounds.GetLeft(), bounds.GetTop());
    Drawing::SamplingOptions samplingOptions;
    if (pixelStretch->x_ < 0) {
        brush.SetShaderEffect(Drawing::ShaderEffect::CreateImageShader(
            *image, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, samplingOptions, scaleMat));
        canvas.AttachBrush(brush);
        canvas.DrawRect(Drawing::Rect(-pixelStretch->x_, -pixelStretch->y_,
            -pixelStretch->x_ + scaledBounds.GetWidth(), -pixelStretch->y_ + scaledBounds.GetHeight()));
        canvas.DetachBrush();
    } else {
        scaleMat.PostScale(scaledBounds.GetWidth() / bounds.GetWidth(), scaledBounds.GetHeight() / bounds.GetHeight());
        brush.SetShaderEffect(Drawing::ShaderEffect::CreateImageShader(
            *image, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, samplingOptions, scaleMat));

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
    auto single_pixel_info = SkImageInfo::Make(1, 1, kRGBA_8888_SkColorType, kPremul_SkAlphaType);
    SkPixmap single_pixel(single_pixel_info, pixel, single_pixel_info.bytesPerPixel());

    // resize snapshot to 1x1 to calculate average color
    // kMedium_SkFilterQuality will do bilerp + mipmaps for down-scaling, we can easily get average color
    imageSnapshot->ExportSkImage()->scalePixels(
        single_pixel, SkSamplingOptions(SkFilterMode::kLinear, SkMipmapMode::kLinear));
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
    if (bgColor != RgbPalette::Transparent()) {
        brush.SetColor(Drawing::Color(bgColor.AsArgbInt()));
        canvas.AttachBrush(brush);
        canvas.DrawRoundRect(RRect2DrawingRRect(properties.GetRRect()));
        canvas.DetachBrush();
    }
    if (const auto& bgShader = properties.GetBackgroundShader()) {
        Drawing::AutoCanvasRestore acr(canvas, true);
        canvas.ClipRoundRect(RRect2DrawingRRect(properties.GetRRect()), Drawing::ClipOp::INTERSECT, antiAlias);
        auto shaderEffect = bgShader->GetDrawingShader();
        if (shaderEffect && shaderEffect->GetDrawingType() == Drawing::DrawingType::RECORDING) {
            auto shader =
                std::static_pointer_cast<Drawing::RecordingShaderEffect>(shaderEffect)->GetCmdList()->Playback();
            brush.SetShaderEffect(shader);
        } else {
            brush.SetShaderEffect(shaderEffect);
        }
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

RRect RSPropertiesPainter::GetRRectForDrawingBorder(const RRect& rr, const std::shared_ptr<RSBorder>& border,
                                                    const Vector4f& outset, const bool& isFirstLayerBorder)
{
    if (!border) {
        return RRect();
    }

    return isFirstLayerBorder ? rr : RRect(rr.rect_.MakeOutset(outset), border->GetRadiusFour());
}

RRect RSPropertiesPainter::GetInnerRRectForDrawingBorder(const RSProperties& properties,
                                                         const std::shared_ptr<RSBorder>& border,
                                                         const Vector4f& innerOutset, const bool& isFirstLayerBorder)
{
    if (!border) {
        return RRect();
    }
    auto innerRRect = properties.GetInnerRRect();
    return isFirstLayerBorder ? innerRRect :
        RRect(innerRRect.rect_.MakeOutset(innerOutset), border->GetRadiusFour() - border->GetWidthFour());
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
        uniform vec3 lightPos;
        uniform vec3 viewPos;
        uniform vec3 specularStrength;

        half4 main(float2 fragCoord) {
            vec3 lightColor = vec3(1.0, 1.0, 1.0);
            float ambientStrength = 0.0;
            vec3 diffuseColor = vec3(1.0, 1.0, 1.0);
            float diffuseStrength = 0.0;
            vec3 specularColor = vec3(1.0, 1.0, 1.0);
            float shininess = 8.0;
            half4 fragColor;
            vec4 NormalMap = vec4(0.0, 0.0, 1.0, 0.0);
            // ambient
            vec4 ambient = lightColor.rgb1 * ambientStrength;
            vec3 norm = normalize(NormalMap.rgb);
            vec3 lightDir = normalize(vec3(lightPos.xy - fragCoord, lightPos.z));
            // diffuse
            float diff = max(dot(norm, lightDir), 0.0);
            vec4 diffuse = diff * lightColor.rgb1;
            vec3 viewDir = normalize(vec3(viewPos.xy - fragCoord, viewPos.z)); //视角向量
            vec3 halfwayDir = normalize(lightDir + viewDir); //半向量
            float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess); //夹角的指数关系
            vec4 specular = lightColor.rgb1 * spec; //乘入射光颜色
            vec4 o = ambient + diffuse * diffuseStrength * diffuseColor.rgb1; //混合环境光 漫反射
            fragColor = o + specular * specularStrength.r * specularColor.rgb1; //高光反射
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
void RSPropertiesPainter::DrawLight(const RSProperties& properties, SkCanvas& canvas)
{
    auto lightBuilder = GetPhongShaderBuilder();
    if (!lightBuilder) {
        return;
    }
    auto& lightSourceMap = RSPointLightManager::Instance()->GetLightSourceMap();
    std::vector<std::shared_ptr<RSLightSource>> lightSourceList;
    EraseIf(lightSourceMap, [&lightSourceList](const auto& pair) -> bool {
        auto lightSourceNodePtr = pair.second.lock();
        if (!lightSourceNodePtr) {
            return true;
        }
        auto& lightSource = lightSourceNodePtr->GetRenderProperties().GetLightSource();
        lightSourceList.push_back(lightSource);
        return false;
    });
    if (lightSourceList.empty()) {
        ROSEN_LOGD("RSPropertiesPainter::DrawLight lightSourceList is empty!");
        return;
    }
    auto& geoPtr = (properties.GetBoundsGeometry());
    auto& contentAbsRect = geoPtr->GetAbsRect();
    auto& lightPosition = lightSourceList[0]->GetAbsLightPosition();
    auto lightIntensity = lightSourceList[0]->GetLightIntensity();
    auto lightX = lightPosition[0] - contentAbsRect.left_;
    auto lightY = lightPosition[1] - contentAbsRect.top_;
    auto lightZ = lightPosition[2];
    lightBuilder->uniform("lightPos") = SkV3 { lightX, lightY, lightZ };
    lightBuilder->uniform("viewPos") = SkV3 { lightX, lightY, 300 };
    SkPaint paint;
    paint.setAntiAlias(true);
    auto illuminatedType = properties.GetIlluminated()->GetIlluminatedType();
    ROSEN_LOGD("RSPropertiesPainter::DrawLight illuminatedType:%{public}d", illuminatedType);
    if (illuminatedType == IlluminatedType::CONTENT) {
        DrawContentLight(properties, canvas, lightBuilder, paint, lightIntensity);
    } else if (illuminatedType == IlluminatedType::BORDER) {
        DrawBorderLight(properties, canvas, lightBuilder, paint, lightIntensity);
    } else if (illuminatedType == IlluminatedType::BORDER_CONTENT) {
        DrawContentLight(properties, canvas, lightBuilder, paint, lightIntensity);
        DrawBorderLight(properties, canvas, lightBuilder, paint, lightIntensity);
    }
}

void RSPropertiesPainter::DrawContentLight(const RSProperties& properties, SkCanvas& canvas,
    std::shared_ptr<SkRuntimeShaderBuilder> lightBuilder, SkPaint& paint, float lightIntensity)
{
    // content light
    sk_sp<SkShader> shader;
    auto contentStrength = lightIntensity * 0.3;
    lightBuilder->uniform("specularStrength") = SkV3 { contentStrength, 0, 0 };
    shader = lightBuilder->makeShader(nullptr, false);
    paint.setShader(shader);
    canvas.drawRRect(RRect2SkRRect(properties.GetRRect()), paint);
}
void RSPropertiesPainter::DrawBorderLight(const RSProperties& properties, SkCanvas& canvas,
    std::shared_ptr<SkRuntimeShaderBuilder> lightBuilder, SkPaint& paint, float lightIntensity)
{
    // border light
    sk_sp<SkShader> shader;
    auto strength = lightIntensity;
    lightBuilder->uniform("specularStrength") = SkV3 { strength, 0, 0 };
    shader = lightBuilder->makeShader(nullptr, false);
    paint.setShader(shader);
    paint.setStrokeWidth(6);
    paint.setStyle(SkPaint::Style::kStroke_Style);
    auto borderRect = properties.GetRRect().rect_;
    float borderRadius = properties.GetRRect().radius_[0].x_;
    auto borderRRect =
        RRect(RectF(borderRect.left_ + 3, borderRect.top_ + 3, borderRect.width_ - 6, borderRect.height_ - 6),
            borderRadius - 3, borderRadius - 3);
    canvas.drawRRect(RRect2SkRRect(borderRRect), paint);
}
#endif

#ifndef USE_ROSEN_DRAWING
void RSPropertiesPainter::DrawBorderBase(const RSProperties& properties, SkCanvas& canvas,
                                         const std::shared_ptr<RSBorder>& border, Vector4f& outset,
                                         Vector4f& innerOutset, const bool isFirstLayerBorder)
{
    if (!border || !border->HasBorder()) {
        return;
    }

    if (!isFirstLayerBorder) {
        outset = outset + border->GetWidthFour();
    }
    SkPaint paint;
    paint.setAntiAlias(true);
    if (border->ApplyFillStyle(paint)) {
        auto skRRect = RRect2SkRRect(GetRRectForDrawingBorder(
            properties.GetRRect(), border, outset, isFirstLayerBorder));
        auto innerSkRRect = RRect2SkRRect(GetInnerRRectForDrawingBorder(
            properties, border, innerOutset, isFirstLayerBorder));
        canvas.drawDRRect(skRRect, innerSkRRect, paint);
    } else {
        bool isZero = isFirstLayerBorder ? properties.GetCornerRadius().IsZero() : border->GetRadiusFour().IsZero();
        if (isZero && border->ApplyFourLine(paint)) {
            RectF rectf = properties.GetBoundsRect();
            border->PaintFourLine(canvas, paint, rectf.MakeOutset(outset));
        } else if (border->ApplyPathStyle(paint)) {
            auto borderWidth = border->GetWidth();
            RRect rrect = GetRRectForDrawingBorder(properties.GetRRect(), border, outset, isFirstLayerBorder);
            rrect.rect_.width_ -= borderWidth;
            rrect.rect_.height_ -= borderWidth;
            rrect.rect_.Move(borderWidth / PARAM_DOUBLE, borderWidth / PARAM_DOUBLE);
            SkPath borderPath;
            borderPath.addRRect(RRect2SkRRect(rrect));
            canvas.drawPath(borderPath, paint);
        } else {
            SkAutoCanvasRestore acr(&canvas, true);
            auto innerSkRRect = RRect2SkRRect(GetInnerRRectForDrawingBorder(
                properties, border, innerOutset, isFirstLayerBorder));
            canvas.clipRRect(innerSkRRect, SkClipOp::kDifference, true);
            auto rrect = RRect2SkRRect(GetRRectForDrawingBorder(
                properties.GetRRect(), border, outset, isFirstLayerBorder));
            paint.setStyle(SkPaint::Style::kStroke_Style);
            border->PaintTopPath(canvas, paint, rrect);
            border->PaintRightPath(canvas, paint, rrect);
            border->PaintBottomPath(canvas, paint, rrect);
            border->PaintLeftPath(canvas, paint, rrect);
        }
    }

    innerOutset = innerOutset + border->GetWidthFour();
}

void RSPropertiesPainter::DrawBorder(const RSProperties& properties, SkCanvas& canvas)
{
    Vector4f outset, innerOutset;
    auto innerBorder = properties.GetBorder();
    if (innerBorder && innerBorder->HasBorder()) {
        DrawBorderBase(properties, canvas, innerBorder, outset, innerOutset, true);
    }

    auto outerBorder = properties.GetOuterBorder();
    if (outerBorder && outerBorder->HasBorder()) {
        DrawBorderBase(properties, canvas, outerBorder, outset, innerOutset, false);
    }
}
#else
void RSPropertiesPainter::DrawBorder(const RSProperties& properties, Drawing::Canvas& canvas)
{
    auto border = properties.GetBorder();
    if (!border || !border->HasBorder()) {
        return;
    }
    Drawing::Pen pen;
    Drawing::Brush brush;
    pen.SetAntiAlias(true);
    brush.SetAntiAlias(true);
    if (border->ApplyFillStyle(brush)) {
        canvas.AttachBrush(brush);
        canvas.DrawNestedRoundRect(
            RRect2DrawingRRect(properties.GetRRect()), RRect2DrawingRRect(properties.GetInnerRRect()));
        canvas.DetachBrush();
    } else if (properties.GetCornerRadius().IsZero() && border->ApplyFourLine(pen)) {
        RectF rect = properties.GetBoundsRect();
        border->PaintFourLine(canvas, pen, rect);
    } else if (border->ApplyPathStyle(pen)) {
        auto borderWidth = border->GetWidth();
        RRect rrect = properties.GetRRect();
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
        canvas.ClipRoundRect(RRect2DrawingRRect(properties.GetInnerRRect()), Drawing::ClipOp::DIFFERENCE, true);
        Drawing::RoundRect rrect = RRect2DrawingRRect(properties.GetRRect());
        border->PaintTopPath(canvas, pen, rrect);
        border->PaintRightPath(canvas, pen, rrect);
        border->PaintBottomPath(canvas, pen, rrect);
        border->PaintLeftPath(canvas, pen, rrect);
    }
}
#endif

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
    } else if (mask->IsPathMask() || mask->GetMaskPath()) {
        auto path = mask->GetMaskPath();
        std::shared_ptr<Drawing::Path> maskPath;
        if (path->GetDrawingType() == Drawing::DrawingType::RECORDING) {
            maskPath = std::static_pointer_cast<Drawing::RecordingPath>(path)->GetCmdList()->Playback();
        } else {
            maskPath = path;
        }
        Drawing::AutoCanvasRestore maskSave(canvas, true);
        canvas.Translate(maskBounds.GetLeft(), maskBounds.GetTop());
        canvas.AttachBrush(mask->GetMaskBrush());
        canvas.DrawPath(*maskPath);
        canvas.DetachBrush();
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
    canvas.ResetMatrix();
    Drawing::SamplingOptions options(Drawing::FilterMode::NEAREST, Drawing::MipmapMode::NONE);
    canvas.AttachBrush(brush);
    canvas.DrawImageRect(*imageSnapshot, clipBounds, options);
    canvas.DetachBrush();
#endif
}

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
    float imageWidth = clipBounds.width();
    float imageHeight = clipBounds.height();
    auto shader = MakeBinarizationShader(
        aiInvert->x_, aiInvert->y_, aiInvert->z_, thresholdLow, thresholdHigh, imageWidth, imageHeight, imageShader);
    SkPaint paint;
    paint.setShader(shader);
    paint.setAntiAlias(true);
    canvas.resetMatrix();
    canvas.translate(clipBounds.left(), clipBounds.top());
    canvas.drawPaint(paint);
}

#ifndef USE_ROSEN_DRAWING
sk_sp<SkShader> RSPropertiesPainter::MakeBinarizationShader(float low, float high, float threshold, float thresholdLow,
    float thresholdHigh, float imageWidth, float imageHeight, sk_sp<SkShader> imageShader)
{
    static constexpr char prog[] = R"(
        uniform half low;
        uniform half high;
        uniform half threshold;
        uniform half thresholdLow;
        uniform half thresholdHigh;
        uniform half imageWidth;
        uniform half imageHeight;
        uniform shader imageShader;
        half4 sumFilter(float2 coord, half imageWidth, half imageHeight, half threshold, half thresholdLow,
            half thresholdHigh, half low, half high)
        {
            half sumLow = 0;
            half sumHigh = 0;
            if (imageWidth > imageHeight) {
                for (half x = 0.0; x < 500; x += 1.0) {
                    if (x > imageWidth) {
                        break;
                    }
                    half3 imgSample = imageShader.eval(float2(x, coord.y)).rgb;
                    float graySample = 0.299 * imgSample.r + 0.587 * imgSample.g + 0.114 * imgSample.b;
                    if (thresholdLow < graySample && graySample < threshold) {
                        sumLow += 1;
                    }
                    if (threshold < graySample && graySample < thresholdHigh) {
                        sumHigh += 1;
                    }
                }
            } else {
                for (half y = 0.0; y < 500; y += 1.0) {
                    if (y > imageHeight) {
                        break;
                    }
                    half3 imgSample = imageShader.eval(float2(coord.x, y)).rgb;
                    float graySample = 0.299 * imgSample.r + 0.587 * imgSample.g + 0.114 * imgSample.b;
                    if (thresholdLow < graySample && graySample < threshold) {
                        sumLow += 1;
                    }
                    if (threshold < graySample && graySample < thresholdHigh) {
                        sumHigh += 1;
                    }
                }
            }
            if (sumLow > sumHigh) {
                return half4(high, high, high, 1.0);
            } else {
                return half4(low, low, low, 1.0);
            }
        }
        half4 main(float2 coord)
        {
            half3 img = imageShader.eval(coord).rgb;
            float gray = 0.299 * img.r + 0.587 * img.g + 0.114 * img.b;
            half4 res = half4(0);
            if (gray < thresholdLow) {
                res = half4(high, high, high, 1.0);
            } else if (gray > thresholdHigh) {
                res = half4(low, low, low, 1.0);
            } else {
                res = sumFilter(coord, imageWidth, imageHeight, threshold, thresholdLow, thresholdHigh, low, high);
            }
            return res;
        }
    )";
    auto [effect, err] = SkRuntimeEffect::MakeForShader(SkString(prog));
    if (!effect) {
        ROSEN_LOGE("MakeBinarizationShader::RuntimeShader effect error: %{public}s\n", err.c_str());
        return nullptr;
    }
    SkRuntimeShaderBuilder builder(effect);
    builder.child("imageShader") = imageShader;
    builder.uniform("low") = low;
    builder.uniform("high") = high;
    builder.uniform("threshold") = threshold;
    builder.uniform("thresholdLow") = thresholdLow;
    builder.uniform("thresholdHigh") = thresholdHigh;
    builder.uniform("imageWidth") = imageWidth;
    builder.uniform("imageHeight") = imageHeight;
    return builder.makeShader(nullptr, false);
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
    std::shared_ptr<Drawing::RuntimeEffect> effect = Drawing::RuntimeEffect::CreateForShader(prog);
    std::shared_ptr<Drawing::ShaderEffect> children[] = {imageShader};
    size_t childCount = 1;
    auto data = std::make_shared<Drawing::Data>();
    data->BuildWithCopy(&lightUpDeg, sizeof(lightUpDeg));
    return effect->MakeShader(data, children, childCount, nullptr, false);
#endif
}

void RSPropertiesPainter::DrawDynamicLightUp(const RSProperties& properties, RSPaintFilterCanvas& canvas)
{
#ifndef USE_ROSEN_DRAWING
    SkSurface* skSurface = canvas.GetSurface();
    if (skSurface == nullptr) {
        ROSEN_LOGD("RSPropertiesPainter::DrawDynamicLightUp skSurface is null");
        return;
    }
    SkAutoCanvasRestore acr(&canvas, true);
    if (RSSystemProperties::GetPropertyDrawableEnable()) {
        // do nothing
    } else if (properties.GetClipBounds() != nullptr) {
        canvas.clipPath(properties.GetClipBounds()->GetSkiaPath(), true);
    } else {
        canvas.clipRRect(RRect2SkRRect(properties.GetRRect()), true);
    }

    auto clipBounds = canvas.getDeviceClipBounds();
    auto image = skSurface->makeImageSnapshot(clipBounds);
    if (image == nullptr) {
        ROSEN_LOGE("RSPropertiesPainter::DrawDynamicLightUp image is null");
        return;
    }
    auto imageShader = image->makeShader(SkSamplingOptions(SkFilterMode::kLinear));
    auto shader = MakeDynamicLightUpShader(
        properties.GetDynamicLightUpRate().value(), properties.GetDynamicLightUpDegree().value(), imageShader);
    SkPaint paint;
    paint.setShader(shader);
    canvas.resetMatrix();
    canvas.translate(clipBounds.left(), clipBounds.top());
    canvas.drawPaint(paint);
#else
    Drawing::Surface* surface = canvas.GetSurface();
    if (surface == nullptr) {
        ROSEN_LOGD("RSPropertiesPainter::DrawDynamicLightUp surface is null");
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
        ROSEN_LOGE("RSPropertiesPainter::DrawDynamicLightUp image is null");
        return;
    }
    Drawing::Matrix scaleMat;
    auto imageShader = Drawing::ShaderEffect::CreateImageShader(
        *image, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP,
        Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), scaleMat);
    auto shader = MakeDynamicLightUpShader(
        properties.GetDynamicLightUpRate().value(), properties.GetDynamicLightUpDegree().value(), imageShader);
    Drawing::Brush brush;
    brush.SetShaderEffect(shader);
    canvas.ResetMatrix();
    canvas.Translate(clipBounds.GetLeft(), clipBounds.GetTop());
    canvas.DrawBackground(brush);
#endif
}

#ifndef USE_ROSEN_DRAWING
sk_sp<SkShader> RSPropertiesPainter::MakeDynamicLightUpShader(
    float dynamicLightUpRate, float dynamicLightUpDeg, sk_sp<SkShader> imageShader)
#else
std::shared_ptr<Drawing::ShaderEffect> RSPropertiesPainter::MakeDynamicLightUpShader(
    float dynamicLightUpRate, float dynamicLightUpDeg, std::shared_ptr<Drawing::ShaderEffect> imageShader)
#endif
{
    static constexpr char prog[] = R"(
        uniform half dynamicLightUpRate;
        uniform half dynamicLightUpDeg;
        uniform shader imageShader;

        half4 main(float2 coord) {
            vec3 c = vec3(imageShader.eval(coord).r * 255,
                imageShader.eval(coord).g * 255, imageShader.eval(coord).b * 255);
            float x = 0.299 * c.r + 0.587 * c.g + 0.114 * c.b;
            float y = (0 - dynamicLightUpRate) * x + dynamicLightUpDeg * 255;
            float R = clamp((c.r + y) / 255, 0.0, 1.0);
            float G = clamp((c.g + y) / 255, 0.0, 1.0);
            float B = clamp((c.b + y) / 255, 0.0, 1.0);
            return vec4(R, G, B, 1.0);
        }
    )";
#ifndef USE_ROSEN_DRAWING
    auto [effect, err] = SkRuntimeEffect::MakeForShader(SkString(prog));
    if (!effect) {
        ROSEN_LOGE("MakeDynamicLightUpShader::RuntimeShader effect error: %{public}s\n", err.c_str());
        return nullptr;
    }
    SkRuntimeShaderBuilder builder(effect);
    builder.child("imageShader") = imageShader;
    builder.uniform("dynamicLightUpRate") = dynamicLightUpRate;
    builder.uniform("dynamicLightUpDeg") = dynamicLightUpDeg;
    return builder.makeShader(nullptr, false);
#else
    std::shared_ptr<Drawing::RuntimeEffect> effect = Drawing::RuntimeEffect::CreateForShader(prog);
    if (!effect) {
        ROSEN_LOGE("MakeDynamicLightUpShader::RuntimeShader effect error\n");
        return nullptr;
    }
    std::shared_ptr<Drawing::RuntimeShaderBuilder> builder = std::make_shared<Drawing::RuntimeShaderBuilder>(effect);
    builder->SetChild("imageShader", imageShader);
    builder->SetUniform("dynamicLightUpRate", dynamicLightUpRate);
    builder->SetUniform("dynamicLightUpDeg", dynamicLightUpDeg);
    return builder->MakeShader(nullptr, false);
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
    for (const auto & particle : particles) {
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
                canvas.DrawCircle(Drawing::Point(position.x_, position.y_), radius);
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
                canvas.Rotate(particles[i]->GetSpin(), imageSize.x_ * scale / 2.f, imageSize.y_ * scale / 2.f);
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
} // namespace Rosen
} // namespace OHOS
