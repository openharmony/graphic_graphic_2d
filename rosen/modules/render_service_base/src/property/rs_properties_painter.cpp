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

#include "common/rs_optional_trace.h"
#include "common/rs_obj_abs_geometry.h"
#include "common/rs_vector2.h"
#include "pipeline/rs_draw_cmd_list.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_root_render_node.h"
#include "platform/common/rs_log.h"
#include "render/rs_blur_filter.h"
#include "render/rs_image.h"
#include "render/rs_mask.h"
#include "render/rs_path.h"
#include "render/rs_shader.h"
#include "render/rs_skia_filter.h"
#include "animation/rs_render_particle.h"

#ifdef USE_ROSEN_DRAWING
#include <cstdint>

#include "draw/canvas.h"
#include "draw/clip.h"
#include "drawing/draw/core_canvas.h"
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
#include "src/image/SkImage_Base.h"
#ifdef NEW_SKIA
#include "include/effects/SkImageFilters.h"
#include "include/effects/SkRuntimeEffect.h"
#else
#include "include/effects/SkBlurImageFilter.h"
#endif
#endif

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
#ifndef USE_ROSEN_DRAWING
constexpr static float FLOAT_ZERO_THRESHOLD = 0.001f;
constexpr static uint8_t DIRECTION_NUM = 4;
#endif
} // namespace

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
{
    if (w == rect.width_ && h == rect.height_) {
        return false;
    }
    mat.reset();
    switch (gravity) {
        case Gravity::CENTER: {
            mat.preTranslate((rect.width_ - w) / PARAM_DOUBLE, (rect.height_ - h) / PARAM_DOUBLE);
            return true;
        }
        case Gravity::TOP: {
            mat.preTranslate((rect.width_ - w) / PARAM_DOUBLE, 0);
            return true;
        }
        case Gravity::BOTTOM: {
            mat.preTranslate((rect.width_ - w) / PARAM_DOUBLE, rect.height_ - h);
            return true;
        }
        case Gravity::LEFT: {
            mat.preTranslate(0, (rect.height_ - h) / PARAM_DOUBLE);
            return true;
        }
        case Gravity::RIGHT: {
            mat.preTranslate(rect.width_ - w, (rect.height_ - h) / PARAM_DOUBLE);
            return true;
        }
        case Gravity::TOP_LEFT: {
            return false;
        }
        case Gravity::TOP_RIGHT: {
            mat.preTranslate(rect.width_ - w, 0);
            return true;
        }
        case Gravity::BOTTOM_LEFT: {
            mat.preTranslate(0, rect.height_ - h);
            return true;
        }
        case Gravity::BOTTOM_RIGHT: {
            mat.preTranslate(rect.width_ - w, rect.height_ - h);
            return true;
        }
        case Gravity::RESIZE: {
            if (ROSEN_EQ(w, 0.f) || ROSEN_EQ(h, 0.f)) {
                return false;
            }
            mat.preScale(rect.width_ / w, rect.height_ / h);
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
            mat.preScale(scale, scale);
            mat.preTranslate((rect.width_ / scale - w) / PARAM_DOUBLE, (rect.height_ / scale - h) / PARAM_DOUBLE);
            return true;
        }
        case Gravity::RESIZE_ASPECT_TOP_LEFT: {
            if (ROSEN_EQ(w, 0.f) || ROSEN_EQ(h, 0.f)) {
                return false;
            }
            float scale = std::min(rect.width_ / w, rect.height_ / h);
            mat.preScale(scale, scale);
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
            mat.preScale(scale, scale);
            mat.preTranslate(rect.width_ / scale - w, rect.height_ / scale - h);
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
            mat.preScale(scale, scale);
            mat.preTranslate((rect.width_ / scale - w) / PARAM_DOUBLE, (rect.height_ / scale - h) / PARAM_DOUBLE);
            return true;
        }
        case Gravity::RESIZE_ASPECT_FILL_TOP_LEFT: {
            if (ROSEN_EQ(w, 0.f) || ROSEN_EQ(h, 0.f)) {
                return false;
            }
            float scale = std::max(rect.width_ / w, rect.height_ / h);
            mat.preScale(scale, scale);
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
            mat.preScale(scale, scale);
            mat.preTranslate(rect.width_ / scale - w, rect.height_ / scale - h);
            return true;
        }
        default: {
            ROSEN_LOGE("GetGravityMatrix unknow gravity=[%d]", gravity);
            return false;
        }
    }
}
#else
bool RSPropertiesPainter::GetGravityMatrix(Gravity gravity, RectF rect, float w, float h, Drawing::Matrix& mat)
{
    if (w == rect.width_ && h == rect.height_) {
        return false;
    }
    mat = Drawing::Matrix();
    switch (gravity) {
        case Gravity::CENTER: {
            mat.PreTranslate((rect.width_ - w) / PARAM_DOUBLE, (rect.height_ - h) / PARAM_DOUBLE);
            return true;
        }
        case Gravity::TOP: {
            mat.PreTranslate((rect.width_ - w) / PARAM_DOUBLE, 0);
            return true;
        }
        case Gravity::BOTTOM: {
            mat.PreTranslate((rect.width_ - w) / PARAM_DOUBLE, rect.height_ - h);
            return true;
        }
        case Gravity::LEFT: {
            mat.PreTranslate(0, (rect.height_ - h) / PARAM_DOUBLE);
            return true;
        }
        case Gravity::RIGHT: {
            mat.PreTranslate(rect.width_ - w, (rect.height_ - h) / PARAM_DOUBLE);
            return true;
        }
        case Gravity::TOP_LEFT: {
            return false;
        }
        case Gravity::TOP_RIGHT: {
            mat.PreTranslate(rect.width_ - w, 0);
            return true;
        }
        case Gravity::BOTTOM_LEFT: {
            mat.PreTranslate(0, rect.height_ - h);
            return true;
        }
        case Gravity::BOTTOM_RIGHT: {
            mat.PreTranslate(rect.width_ - w, rect.height_ - h);
            return true;
        }
        case Gravity::RESIZE: {
            if (ROSEN_EQ(w, 0.f) || ROSEN_EQ(h, 0.f)) {
                return false;
            }
            mat.PreScale(rect.width_ / w, rect.height_ / h);
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
            mat.PreScale(scale, scale);
            mat.PreTranslate((rect.width_ / scale - w) / PARAM_DOUBLE, (rect.height_ / scale - h) / PARAM_DOUBLE);
            return true;
        }
        case Gravity::RESIZE_ASPECT_TOP_LEFT: {
            if (ROSEN_EQ(w, 0.f) || ROSEN_EQ(h, 0.f)) {
                return false;
            }
            float scale = std::min(rect.width_ / w, rect.height_ / h);
            mat.PreScale(scale, scale);
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
            mat.PreScale(scale, scale);
            mat.PreTranslate(rect.width_ / scale - w, rect.height_ / scale - h);
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
            mat.PreScale(scale, scale);
            mat.PreTranslate((rect.width_ / scale - w) / PARAM_DOUBLE, (rect.height_ / scale - h) / PARAM_DOUBLE);
            return true;
        }
        case Gravity::RESIZE_ASPECT_FILL_TOP_LEFT: {
            if (ROSEN_EQ(w, 0.f) || ROSEN_EQ(h, 0.f)) {
                return false;
            }
            float scale = std::max(rect.width_ / w, rect.height_ / h);
            mat.PreScale(scale, scale);
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
            mat.PreScale(scale, scale);
            mat.PreTranslate(rect.width_ / scale - w, rect.height_ / scale - h);
            return true;
        }
        default: {
            ROSEN_LOGE("GetGravityMatrix unknow gravity=[%d]", gravity);
            return false;
        }
    }
}
#endif

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
    if (properties.GetShadowPath() && !properties.GetShadowPath()->GetDrawingPath().IsValid()) {
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
        // spotRect outset (transRatio * DEFAULT_LIGHT_RADIUS, transRatio * DEFAULT_LIGHT_RADIUS)
        spotRect.SetLeft(spotRect.GetLeft() - transRatio * DEFAULT_LIGHT_RADIUS);
        spotRect.SetTop(spotRect.GetTop() - transRatio * DEFAULT_LIGHT_RADIUS);
        spotRect.SetRight(spotRect.GetRight() + transRatio * DEFAULT_LIGHT_RADIUS);
        spotRect.SetBottom(spotRect.GetBottom() + transRatio * DEFAULT_LIGHT_RADIUS);

        shadowRect = ambientRect;
        std::min(elevation * 0.5f, MAX_AMBIENT_RADIUS);
        float ambientBlur = std::min(elevation * 0.5f, MAX_AMBIENT_RADIUS);
        // shadowRect outset (ambientBlur, ambientBlur)
        shadowRect.SetLeft(shadowRect.GetLeft() - ambientBlur);
        shadowRect.SetTop(shadowRect.GetTop() - ambientBlur);
        shadowRect.SetRight(shadowRect.GetRight() + ambientBlur);
        shadowRect.SetBottom(shadowRect.GetBottom() + ambientBlur);

        shadowRect.Join(spotRect);
        // shadowRect outset (1, 1)
        shadowRect.SetLeft(shadowRect.GetLeft() - 1);
        shadowRect.SetTop(shadowRect.GetTop() - 1);
        shadowRect.SetRight(shadowRect.GetRight() + 1);
        shadowRect.SetBottom(shadowRect.GetBottom() + 1);
    } else {
        Drawing::Brush brush;
        brush.SetAntiAlias(true);
        Drawing::Filter filter;
        filter.SetMaskFilter(
            Drawing::MaskFilter::CreateBlurMaskFilter(Drawing::BlurType::NORMAL, properties.GetShadowRadius()));
        brush.SetFilter(filter);
    }

    auto geoPtr = (properties.GetBoundsGeometry());
    Drawing::Matrix matrix = geoPtr ? geoPtr->GetAbsMatrix() : Drawing::Matrix();
    matrix.MapRect(shadowRect, shadowRect);

    dirtyShadow.left_ = shadowRect.GetLeft();
    dirtyShadow.top_ = shadowRect.GetTop();
    dirtyShadow.width_ = shadowRect.GetWidth();
    dirtyShadow.height_ = shadowRect.GetHeight();
}
#endif

#ifndef USE_ROSEN_DRAWING
void RSPropertiesPainter::DrawShadow(const RSProperties& properties,
    RSPaintFilterCanvas& canvas, const RRect* rrect, bool isLeashWindow)
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
        if (!isLeashWindow) {
            canvas.clipPath(skPath, SkClipOp::kDifference, true);
        }
    } else if (properties.GetClipBounds()) {
        skPath = properties.GetClipBounds()->GetSkiaPath();
        if (!isLeashWindow) {
            canvas.clipPath(skPath, SkClipOp::kDifference, true);
        }
    } else {
        if (rrect != nullptr) {
            skPath.addRRect(RRect2SkRRect(*rrect));
            if (!isLeashWindow) {
                canvas.clipRRect(RRect2SkRRect(*rrect), SkClipOp::kDifference, true);
            }
        } else {
            skPath.addRRect(RRect2SkRRect(properties.GetRRect()));
            if (!isLeashWindow) {
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
void RSPropertiesPainter::DrawShadow(const RSProperties& properties,
    RSPaintFilterCanvas& canvas, const RRect* rrect, bool isLeashWindow)
{
    // skip shadow if not valid or cache is enabled
    //Todo isCacheEnabled
    if (properties.IsSpherizeValid() || !properties.IsShadowValid()) {
        return;
    }
    Drawing::AutoCanvasRestore acr(canvas, true);
    Drawing::Path path;
    if (properties.GetShadowPath() && !properties.GetShadowPath()->GetDrawingPath().IsValid()) {
        path = properties.GetShadowPath()->GetDrawingPath();
        if (!isLeashWindow) {
            canvas.ClipPath(path, Drawing::ClipOp::DIFFERENCE, true);
        }
    } else if (properties.GetClipBounds()) {
        path = properties.GetClipBounds()->GetDrawingPath();
        if (!isLeashWindow) {
            canvas.ClipPath(path, Drawing::ClipOp::DIFFERENCE, true);
        }
    } else {
        if (rrect != nullptr) {
            path.AddRoundRect(RRect2DrawingRRect(*rrect));
            if (!isLeashWindow) {
                canvas.ClipRoundRect(RRect2DrawingRRect(*rrect), Drawing::ClipOp::DIFFERENCE, true);
            }
        } else {
            path.AddRoundRect(RRect2DrawingRRect(properties.GetRRect()));
            if (!isLeashWindow) {
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
#ifdef NEW_SKIA
    blurPaint.setImageFilter(SkImageFilters::Blur(blurRadius, blurRadius, SkTileMode::kDecal, nullptr));
#else
    blurPaint.setImageFilter(SkBlurImageFilter::Make(blurRadius, blurRadius, SkTileMode::kDecal, nullptr));
#endif
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

#ifndef USE_ROSEN_DRAWING
void RSPropertiesPainter::DrawShadowInner(const RSProperties& properties, RSPaintFilterCanvas& canvas, SkPath& skPath)
{
    skPath.offset(properties.GetShadowOffsetX(), properties.GetShadowOffsetY());
    Color spotColor = properties.GetShadowColor();

    // The translation of the matrix is rounded to improve the hit ratio of skia blurfilter cache,
    // the function <compute_key_and_clip_bounds> in <skia/src/gpu/GrBlurUtil.cpp> for more details.
    RSAutoCanvasRestore rst(&canvas);
    auto matrix = canvas.getTotalMatrix();
    matrix.setTranslateX(std::ceil(matrix.getTranslateX()));
    matrix.setTranslateY(std::ceil(matrix.getTranslateY()));
    // Round the scaleX/scaleY to 2 decimal places.
    matrix.setScaleX(std::round(matrix.getScaleX() * 100) / 100);
    matrix.setScaleY(std::round(matrix.getScaleY() * 100) / 100);
    canvas.setMatrix(matrix);

    if (properties.shadow_->GetHardwareAcceleration()) {
        if (properties.GetShadowElevation() <= 0.f) {
            return;
        }
        SkPoint3 planeParams = { 0.0f, 0.0f, properties.GetShadowElevation() };
        SkPoint3 lightPos = { canvas.getTotalMatrix().getTranslateX() + skPath.getBounds().centerX(),
            canvas.getTotalMatrix().getTranslateY() + skPath.getBounds().centerY(), DEFAULT_LIGHT_HEIGHT };
        Color ambientColor = Color::FromArgbInt(DEFAULT_AMBIENT_COLOR);
        ambientColor.MultiplyAlpha(canvas.GetAlpha());
        spotColor.MultiplyAlpha(canvas.GetAlpha());
        SkShadowUtils::DrawShadow(&canvas, skPath, planeParams, lightPos, DEFAULT_LIGHT_RADIUS,
            ambientColor.AsArgbInt(), spotColor.AsArgbInt(), SkShadowFlags::kTransparentOccluder_ShadowFlag);
    } else {
        SkPaint paint;
        paint.setColor(spotColor.AsArgbInt());
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
    if (properties.shadow_->GetHardwareAcceleration()) {
        if (properties.GetShadowElevation() <= 0.f) {
            return;
        }
        Drawing::Point3 planeParams = { 0.0f, 0.0f, properties.GetShadowElevation() };
        Drawing::scalar centerX = path.GetBounds().GetLeft() + path.GetBounds().GetWidth() / 2;
        Drawing::scalar centerY = path.GetBounds().GetTop() + path.GetBounds().GetHeight() / 2;
        Drawing::Point3 lightPos = {
            canvas.GetTotalMatrix().Get(Drawing::Matrix::TRANS_X) + centerX,
            canvas.GetTotalMatrix().Get(Drawing::Matrix::TRANS_X) + centerY, DEFAULT_LIGHT_HEIGHT };
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

#ifndef USE_ROSEN_DRAWING
#ifdef NEW_SKIA
bool RSPropertiesPainter::GetGradientDirectionPoints(
    SkPoint (&pts)[2], const SkRect& clipBounds, GradientDirection direction)
{
    switch (direction) {
        case GradientDirection::BOTTOM: {
            pts[0].set(clipBounds.width() / 2, 0); // 2 represents middle of width;
            pts[1].set(clipBounds.width() / 2, clipBounds.height()); // 2 represents middle of width;
            break;
        }
        case GradientDirection::TOP: {
            pts[0].set(clipBounds.width() / 2, clipBounds.height()); // 2 represents middle of width;
            pts[1].set(clipBounds.width() / 2, 0); // 2 represents middle of width;
            break;
        }
        case GradientDirection::RIGHT: {
            pts[0].set(0, clipBounds.height() / 2); // 2 represents middle of height;
            pts[1].set(clipBounds.width(), clipBounds.height() / 2); // 2 represents middle of height;
            break;
        }
        case GradientDirection::LEFT: {
            pts[0].set(clipBounds.width(), clipBounds.height() / 2); // 2 represents middle of height;
            pts[1].set(0, clipBounds.height() / 2); // 2 represents middle of height;
            break;
        }
        case GradientDirection::RIGHT_BOTTOM: {
            pts[0].set(0, 0);
            pts[1].set(clipBounds.width(), clipBounds.height());
            break;
        }
        case GradientDirection::LEFT_TOP: {
            pts[0].set(clipBounds.width(), clipBounds.height());
            pts[1].set(0, 0);
            break;
        }
        case GradientDirection::LEFT_BOTTOM: {
            pts[0].set(clipBounds.width(), 0);
            pts[1].set(0, clipBounds.height());
            break;
        }
        case GradientDirection::RIGHT_TOP: {
            pts[0].set(0, clipBounds.height());
            pts[1].set(clipBounds.width(), 0);
            break;
        }
        default: {
            return false;
        }
    }
    return true;
}
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

sk_sp<SkShader> RSPropertiesPainter::MakeAlphaGradientShader(
    const SkRect& clipBounds, const std::shared_ptr<RSLinearGradientBlurPara>& para, uint8_t directionBias)
{
    std::vector<SkColor> c;
    std::vector<SkScalar> p;
    SkPoint pts[2];
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
        c.emplace_back(SkColorSetARGB(ColorMin, ColorMax, ColorMax, ColorMax));
        p.emplace_back(para->fractionStops_[0].second - 0.01); // 0.01 represents the fraction bias
    }
    for (size_t i = 0; i < para->fractionStops_.size(); i++) {
        c.emplace_back(SkColorSetARGB(
            static_cast<uint8_t>(para->fractionStops_[i].first * ColorMax), ColorMax, ColorMax, ColorMax));
        p.emplace_back(para->fractionStops_[i].second);
    }
    // 0.01 represents the fraction bias
    if (para->fractionStops_[para->fractionStops_.size() - 1].second < (1 - 0.01)) {
        c.emplace_back(SkColorSetARGB(ColorMin, ColorMax, ColorMax, ColorMax));
        // 0.01 represents the fraction bias
        p.emplace_back(para->fractionStops_[para->fractionStops_.size() - 1].second + 0.01);
    }
    auto shader = SkGradientShader::MakeLinear(pts, &c[0], &p[0], p.size(), SkTileMode::kClamp);
    return shader;
}

sk_sp<SkShader> RSPropertiesPainter::MakeHorizontalMeanBlurShader(
    float radiusIn, sk_sp<SkShader> shader, sk_sp<SkShader> gradientShader)
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
    auto [effect, err] = SkRuntimeEffect::MakeForShader(SkString(prog));
    sk_sp<SkShader> children[] = {shader, gradientShader};
    size_t childCount = 2;
    return effect->makeShader(SkData::MakeWithCopy(
        &radiusIn, sizeof(radiusIn)), children, childCount, nullptr, false);
}

sk_sp<SkShader> RSPropertiesPainter::MakeVerticalMeanBlurShader(
    float radiusIn, sk_sp<SkShader> shader, sk_sp<SkShader> gradientShader)
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
    auto [effect, err] = SkRuntimeEffect::MakeForShader(SkString(prog));
    sk_sp<SkShader> children[] = {shader, gradientShader};
    size_t childCount = 2;
    return effect->makeShader(SkData::MakeWithCopy(
        &radiusIn, sizeof(radiusIn)), children, childCount, nullptr, false);
}

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
#endif
#endif

#ifndef USE_ROSEN_DRAWING
void RSPropertiesPainter::DrawLinearGradientBlurFilter(
    const RSProperties& properties, RSPaintFilterCanvas& canvas, const std::optional<SkRect>& rect)
{
    const auto& para = properties.GetLinearGradientBlurPara();
    if (para == nullptr || para->blurRadius_ <= 0) {
        return;
    }
#ifdef NEW_SKIA
    SkSurface* skSurface = canvas.GetSurface();
    if (skSurface == nullptr) {
        return;
    }
    SkAutoCanvasRestore acr(&canvas, true);
    if (rect.has_value()) {
        canvas.clipRect((*rect), true);
    } else if (properties.GetClipBounds() != nullptr) {
        canvas.clipPath(properties.GetClipBounds()->GetSkiaPath(), true);
    } else { // we always do clip for DrawLinearGradientBlurFilter, even if ClipToBounds is false
        canvas.clipRRect(RRect2SkRRect(properties.GetRRect()), true);
    }

    auto clipBounds = canvas.getDeviceClipBounds();
    auto clipIPadding = clipBounds.makeOutset(-1, -1);
    SkMatrix mat = canvas.getTotalMatrix();
    uint8_t directionBias = CalcDirectionBias(mat);

    auto alphaGradientShader = MakeAlphaGradientShader(SkRect::Make(clipIPadding), para, directionBias);
    if (alphaGradientShader == nullptr) {
        ROSEN_LOGE("RSPropertiesPainter::DrawLinearGradientBlurFilter alphaGradientShader null");
        return;
    }
    float radius = para->blurRadius_ / 2;
    canvas.resetMatrix();
    canvas.translate(clipIPadding.left(), clipIPadding.top());

    DrawHorizontalLinearGradientBlur(skSurface, canvas, radius, alphaGradientShader, clipIPadding);
    DrawVerticalLinearGradientBlur(skSurface, canvas, radius, alphaGradientShader, clipIPadding);
    DrawHorizontalLinearGradientBlur(skSurface, canvas, radius, alphaGradientShader, clipIPadding);
    DrawVerticalLinearGradientBlur(skSurface, canvas, radius, alphaGradientShader, clipIPadding);
#endif
}
#else
void RSPropertiesPainter::DrawLinearGradientBlurFilter(
    const RSProperties& properties, RSPaintFilterCanvas& canvas, const std::optional<Drawing::Rect>& rect)
{
}
#endif


#ifndef USE_ROSEN_DRAWING
void RSPropertiesPainter::DrawFilter(const RSProperties& properties, RSPaintFilterCanvas& canvas,
    FilterType filterType, const std::optional<SkRect>& rect)
{
    auto& RSFilter =
        (filterType == FilterType::BACKGROUND_FILTER) ? properties.GetBackgroundFilter() : properties.GetFilter();
    if (RSFilter == nullptr || !RSFilter->IsValid()) {
        return;
    }
#ifdef NEW_SKIA
    RS_OPTIONAL_TRACE_BEGIN("DrawFilter " + RSFilter->GetDescription());
    g_blurCnt++;
    SkAutoCanvasRestore acr(&canvas, true);
    if (rect.has_value()) {
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
        RS_OPTIONAL_TRACE_END();
        return;
    }

    // for foreground filter, when do online opacity, rendering result already applied opacity,
    // so drawImage should not apply opacity again
    RSAutoCanvasRestore autoCanvasRestore(&canvas,
        filterType == FilterType::FOREGROUND_FILTER ? RSAutoCanvasRestore::kAlpha : RSAutoCanvasRestore::kNone);
    if (filterType == FilterType::FOREGROUND_FILTER) {
        canvas.SetAlpha(1.0);
    }

    // Optional use cacheManager to draw filter
    if (auto& cacheManager = properties.GetFilterCacheManager(filterType == FilterType::FOREGROUND_FILTER)) {
        cacheManager->DrawFilter(canvas, filter);
        RS_OPTIONAL_TRACE_END();
        return;
    }

    auto clipIBounds = canvas.getDeviceClipBounds();
    auto imageSnapshot = skSurface->makeImageSnapshot(clipIBounds);
    if (imageSnapshot == nullptr) {
        ROSEN_LOGE("RSPropertiesPainter::DrawFilter image null");
        RS_OPTIONAL_TRACE_END();
        return;
    }
    if (RSSystemProperties::GetImageGpuResourceCacheEnable(imageSnapshot->width(), imageSnapshot->height())) {
        ROSEN_LOGD("DrawFilter cache image resource(width:%d, height:%d).",
            imageSnapshot->width(), imageSnapshot->height());
        as_IB(imageSnapshot)->hintCacheGpuResource();
    }

    filter->PreProcess(imageSnapshot);
    canvas.resetMatrix();
    auto visibleIRect = canvas.GetVisibleRect().round();
    if (!visibleIRect.isEmpty()) {
        canvas.clipIRect(visibleIRect);
    }
    filter->DrawImageRect(
        canvas, imageSnapshot, SkRect::Make(imageSnapshot->bounds().makeOutset(-1, -1)), SkRect::Make(clipIBounds));
    filter->PostProcess(canvas);
    RS_OPTIONAL_TRACE_END();
#endif
}
#else
void RSPropertiesPainter::DrawFilter(const RSProperties& properties, RSPaintFilterCanvas& canvas,
    FilterType filterType, const std::optional<Drawing::Rect>& rect)
{
    auto& RSFilter =
        (filterType == FilterType::BACKGROUND_FILTER) ? properties.GetBackgroundFilter() : properties.GetFilter();
    if (RSFilter == nullptr || !RSFilter->IsValid()) {
        return;
    }
    RS_TRACE_NAME("DrawFilter " + RSFilter->GetDescription());
    g_blurCnt++;
    Drawing::AutoCanvasRestore acr(canvas, true);
    if (rect.has_value()) {
        canvas.ClipRect((*rect), Drawing::ClipOp::INTERSECT, true);
    } else if (properties.GetClipBounds() != nullptr) {
        canvas.ClipPath(properties.GetClipBounds()->GetDrawingPath(), Drawing::ClipOp::INTERSECT, true);
    } else {
        canvas.ClipRoundRect(RRect2DrawingRRect(properties.GetRRect()), Drawing::ClipOp::INTERSECT, true);
    }
    auto filter = std::static_pointer_cast<RSDrawingFilter>(RSFilter);
    auto surface = canvas.GetSurface();
    if (surface == nullptr) {
        ROSEN_LOGD("RSPropertiesPainter::DrawFilter surface null");
        auto brush = filter->GetBrush();
        Drawing::SaveLayerOps slr(nullptr, &brush, Drawing::SaveLayerOps::Flags::INIT_WITH_PREVIOUS);
        canvas.SaveLayer(slr);
        filter->PostProcess(canvas);
        return;
    }

    // for foreground filter, when do online opacity, rendering result already applied opacity,
    // so drawImage should not apply opacity again
    RSAutoCanvasRestore autoCanvasRestore(&canvas,
        filterType == FilterType::FOREGROUND_FILTER ? RSAutoCanvasRestore::kAlpha : RSAutoCanvasRestore::kNone);
    if (filterType == FilterType::FOREGROUND_FILTER) {
        canvas.SetAlpha(1.0);
    }

    auto clipIBounds = canvas.GetDeviceClipBounds();
    auto clipIPadding = Drawing::RectI(clipIBounds.GetLeft() + 1, clipIBounds.GetTop() + 1,
        clipIBounds.GetRight() - 1, clipIBounds.GetBottom() - 1);
    auto imageSnapshot = surface->GetImageSnapshot(clipIPadding);
    if (imageSnapshot == nullptr) {
        ROSEN_LOGE("RSPropertiesPainter::DrawFilter image null");
        return;
    }
    filter->PreProcess(imageSnapshot);
    canvas.ResetMatrix();
    auto visibleRect = canvas.GetVisibleRect();
    auto visibleIRect = Drawing::RectI(visibleRect.GetLeft(), visibleRect.GetTop(),
        visibleRect.GetRight(), visibleRect.GetBottom());
    if (visibleIRect.Intersect(clipIBounds)) {
        auto clipRect = Drawing::Rect(visibleIRect.GetLeft(), visibleIRect.GetTop(),
            visibleIRect.GetRight(), visibleIRect.GetBottom());
        canvas.ClipRect(clipRect, Drawing::ClipOp::INTERSECT, false);
        auto srcRect = Drawing::Rect(visibleIRect.GetLeft() + 1, visibleIRect.GetTop() + 1,
            visibleIRect.GetRight() - 1, visibleIRect.GetBottom() - 1);
        srcRect.Offset(-clipIPadding.GetLeft(), -clipIPadding.GetTop());
        filter->DrawImageRect(canvas, imageSnapshot, srcRect, clipRect);
    } else {
        auto srcRect = Drawing::Rect(0, 0, clipIPadding.GetWidth(), clipIPadding.GetHeight());
        auto dstRect = Drawing::Rect(clipIBounds.GetLeft(), clipIBounds.GetTop(),
            clipIBounds.GetRight(), clipIBounds.GetBottom());
        filter->DrawImageRect(canvas, imageSnapshot, srcRect, dstRect);
    }
    filter->PostProcess(canvas);
}
#endif

#ifndef USE_ROSEN_DRAWING
void RSPropertiesPainter::DrawBackgroundEffect(
    const RSProperties& properties, RSPaintFilterCanvas& canvas, const SkIRect& rect)
{
    auto& RSFilter = properties.GetBackgroundFilter();
    if (RSFilter == nullptr || !RSFilter->IsValid()) {
        return;
    }
    auto filter = std::static_pointer_cast<RSSkiaFilter>(RSFilter);
    RS_TRACE_NAME("DrawBackgroundEffect " + filter->GetDescription());
    auto skSurface = canvas.GetSurface();
    if (skSurface == nullptr) {
        ROSEN_LOGE("RSPropertiesPainter::DrawBackgroundEffect skSurface null");
        return;
    }
    g_blurCnt++;

    // Optional use cacheManager to draw filter
    if (auto& cacheManager = properties.GetFilterCacheManager(false)) {
        // the input rect is in global coordinate, so we need to save/reset matrix before clip
        SkAutoCanvasRestore acr(&canvas, true);
        canvas.resetMatrix();
#ifdef NEW_SKIA
        canvas.clipIRect(rect);
#endif
        auto data = cacheManager->GeneratedCachedEffectData(canvas, filter);
        canvas.SetEffectData(data);
        return;
    }

    SkIRect imageRect = SkIRect::MakeWH(skSurface->width(), skSurface->height());
    imageRect.intersect(rect);
    auto imageSnapshot = skSurface->makeImageSnapshot(imageRect);
    if (imageSnapshot == nullptr) {
        ROSEN_LOGE("RSPropertiesPainter::DrawBackgroundEffect image snapshot null");
        return;
    }

    filter->PreProcess(imageSnapshot);
    // create a offscreen skSurface
    sk_sp<SkSurface> offscreenSurface = skSurface->makeSurface(imageSnapshot->imageInfo());
    if (offscreenSurface == nullptr) {
        ROSEN_LOGE("RSPropertiesPainter::DrawBackgroundEffect offscreenSurface null");
        return;
    }
    RSPaintFilterCanvas offscreenCanvas(offscreenSurface.get());
    auto clipBounds = SkRect::MakeIWH(imageRect.width(), imageRect.height());
    filter->DrawImageRect(offscreenCanvas, imageSnapshot, SkRect::Make(imageSnapshot->bounds()), clipBounds);
    filter->PostProcess(offscreenCanvas);

    auto imageCache = offscreenSurface->makeImageSnapshot();
    if (imageCache == nullptr) {
        ROSEN_LOGE("RSPropertiesPainter::DrawBackgroundEffect imageCache snapshot null");
        return;
    }
    CachedEffectData data = { imageCache, imageRect };
    canvas.SetEffectData(data);
}
#else
void RSPropertiesPainter::DrawBackgroundEffect(
    const RSProperties& properties, RSPaintFilterCanvas& canvas, const Drawing::RectI& rect)
{
}
#endif

void RSPropertiesPainter::ApplyBackgroundEffect(const RSProperties& properties, RSPaintFilterCanvas& canvas)
{
#ifndef USE_ROSEN_DRAWING
    RS_TRACE_NAME("ApplyBackgroundEffect");
    SkAutoCanvasRestore acr(&canvas, true);
    if (properties.GetClipBounds() != nullptr) {
        canvas.clipPath(properties.GetClipBounds()->GetSkiaPath(), true);
    } else { // we always do clip for ApplyBackgroundEffect, even if ClipToBounds is false
        canvas.clipRRect(RRect2SkRRect(properties.GetRRect()), true);
    }
    canvas.resetMatrix();
    auto visibleIRect = canvas.GetVisibleRect().round();
    #ifdef NEW_SKIA
    if (!visibleIRect.isEmpty()) {
        canvas.clipIRect(visibleIRect);
    }
    #endif

    const auto& [bgImage, imageIRect, unused] = canvas.GetEffectData();
    if (bgImage == nullptr) {
        ROSEN_LOGE("RSPropertiesPainter::ApplyBackgroundEffect bgImage null");
        return;
    }
    SkPaint defaultPaint;
    // dstRect: canvas clip region
    auto dstRect = SkRect::Make(canvas.getDeviceClipBounds());
    // srcRect: map dstRect onto cache coordinate
    auto srcRect = dstRect.makeOffset(-imageIRect.left(), -imageIRect.top());
#ifdef NEW_SKIA
    canvas.drawImageRect(
        bgImage, srcRect, dstRect, SkSamplingOptions(), &defaultPaint, SkCanvas::kStrict_SrcRectConstraint);
#else
    auto clipIBounds = canvas.getDeviceClipBounds();
    auto clipIPadding = clipIBounds.makeOutset(-1, -1);
    canvas.drawImageRect(bgImage, SkRect::Make(clipIPadding.makeOffset(-imageIRect.left(), -imageIRect.top())),
        SkRect::Make(clipIPadding), &defaultPaint);
#endif
#endif
}

void RSPropertiesPainter::DrawForegroundEffect(const RSProperties& properties, RSPaintFilterCanvas& canvas)
{
#ifndef USE_ROSEN_DRAWING
    const auto& data = canvas.GetEffectData();
    if (data.childrenPath_.isEmpty()) {
        return;
    }
    auto& colorFilter = properties.GetColorFilter();
    if (colorFilter == nullptr) {
        return;
    }
    RS_TRACE_NAME("DrawForegroundEffect");
    SkAutoCanvasRestore acr(&canvas, true);
    canvas.resetMatrix();
    canvas.clipPath(data.childrenPath_, true);
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setColorFilter(colorFilter);
    auto pathBounds = data.childrenPath_.getBounds();
    SkCanvas::SaveLayerRec slr(&pathBounds, &paint, SkCanvas::kInitWithPrevious_SaveLayerFlag);
    canvas.saveLayer(slr);
#endif
}

static Vector4f GetStretchSize(const RSProperties& properties)
{
    Vector4f stretchSize;
    if (properties.IsPixelStretchValid()) {
        stretchSize = properties.GetPixelStretch();
    } else {
        if (properties.IsPixelStretchPercentValid()) {
            stretchSize = properties.GetPixelStretchByPercent();
        }
    }

    return stretchSize;
}

#ifndef USE_ROSEN_DRAWING
void RSPropertiesPainter::DrawPixelStretch(const RSProperties& properties, RSPaintFilterCanvas& canvas)
{
    if (!properties.IsPixelStretchValid() && !properties.IsPixelStretchPercentValid()) {
        return;
    }

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

    Vector4f stretchSize = GetStretchSize(properties);
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

    auto scaledBounds = SkRect::MakeLTRB(bounds.left() - stretchSize.x_, bounds.top() - stretchSize.y_,
        bounds.right() + stretchSize.z_, bounds.bottom() + stretchSize.w_);
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
    SkMatrix inverseMat, scaleMat;
    auto boundsGeo = (properties.GetBoundsGeometry());
    if (boundsGeo && !boundsGeo->IsEmpty()) {
        if (!canvas.getTotalMatrix().invert(&inverseMat)) {
            ROSEN_LOGE("RSPropertiesPainter::DrawPixelStretch get inverse matrix failed.");
        }
        scaleMat.setScale(inverseMat.getScaleX(), inverseMat.getScaleY());
    }

    canvas.save();
    canvas.translate(bounds.x(), bounds.y());
    if (properties.IsPixelStretchExpanded()) {
#ifdef NEW_SKIA
        paint.setShader(image->makeShader(SkTileMode::kClamp, SkTileMode::kClamp, SkSamplingOptions(), &scaleMat));
#else
        paint.setShader(image->makeShader(SkTileMode::kClamp, SkTileMode::kClamp, &scaleMat));
#endif
        canvas.drawRect(SkRect::MakeXYWH(-stretchSize.x_, -stretchSize.y_, scaledBounds.width(), scaledBounds.height()),
            paint);
    } else {
        scaleMat.setScale(scaledBounds.width() / bounds.width() * scaleMat.getScaleX(),
            scaledBounds.height() / bounds.height() * scaleMat.getScaleY());
#ifdef NEW_SKIA
        paint.setShader(image->makeShader(SkTileMode::kClamp, SkTileMode::kClamp, SkSamplingOptions(), &scaleMat));
#else
        paint.setShader(image->makeShader(SkTileMode::kClamp, SkTileMode::kClamp, &scaleMat));
#endif
        canvas.translate(-stretchSize.x_, -stretchSize.y_);
        canvas.drawRect(SkRect::MakeXYWH(stretchSize.x_, stretchSize.y_, bounds.width(), bounds.height()), paint);
    }
    canvas.restore();
}
#else
void RSPropertiesPainter::DrawPixelStretch(const RSProperties& properties, RSPaintFilterCanvas& canvas)
{
    if (!properties.IsPixelStretchValid() && !properties.IsPixelStretchPercentValid()) {
        return;
    }

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

    Vector4f stretchSize = GetStretchSize(properties);
    /*  Calculates the relative coordinates of the clipbounds
        with respect to the origin of the current canvas coordinates */
    Drawing::Matrix worldToLocalMat;
    if (!canvas.GetTotalMatrix().Invert(worldToLocalMat)) {
        ROSEN_LOGE("RSPropertiesPainter::DrawPixelStretch get invert matrix failed.");
    }
    Drawing::Rect localClipBounds;
    Drawing::Rect fClipBounds(clipBounds.GetLeft(), clipBounds.GetTop(), clipBounds.GetWidth(),
        clipBounds.GetBottom());
    if (!worldToLocalMat.MapRect(localClipBounds, fClipBounds)) {
        ROSEN_LOGE("RSPropertiesPainter::DrawPixelStretch map rect failed.");
    }

    if (!bounds.Intersect(localClipBounds)) {
        ROSEN_LOGE("RSPropertiesPainter::DrawPixelStretch intersect clipbounds failed");
    }

    auto scaledBounds = Drawing::Rect(bounds.GetLeft() - stretchSize.x_, bounds.GetTop() - stretchSize.y_,
        bounds.GetRight() + stretchSize.z_, bounds.GetBottom() + stretchSize.w_);
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
    if (properties.IsPixelStretchExpanded()) {
        brush.SetShaderEffect(Drawing::ShaderEffect::CreateImageShader(
            *image, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, samplingOptions, scaleMat));
        canvas.AttachBrush(brush);
        canvas.DrawRect(Drawing::Rect(-stretchSize.x_, -stretchSize.y_,
            -stretchSize.x_ + scaledBounds.GetWidth(), -stretchSize.y_ + scaledBounds.GetHeight()));
        canvas.DetachBrush();
    } else {
        scaleMat.Scale(
            scaledBounds.GetWidth() / bounds.GetWidth(), scaledBounds.GetHeight() / bounds.GetHeight(), 0, 0);
        brush.SetShaderEffect(Drawing::ShaderEffect::CreateImageShader(
            *image, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, samplingOptions, scaleMat));

        canvas.Translate(-stretchSize.x_, -stretchSize.y_);
        canvas.AttachBrush(brush);
        canvas.DrawRect(Drawing::Rect(
            stretchSize.x_, stretchSize.y_, stretchSize.x_ + bounds.GetWidth(), stretchSize.y_ + bounds.GetHeight()));
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
    auto single_pixel_info = SkImageInfo::MakeN32Premul(1, 1);
    SkPixmap single_pixel(single_pixel_info, pixel, single_pixel_info.bytesPerPixel());

    // resize snapshot to 1x1 to calculate average color
    // kMedium_SkFilterQuality will do bilerp + mipmaps for down-scaling, we can easily get average color
#ifdef NEW_SKIA
    imageSnapshot->scalePixels(single_pixel, SkSamplingOptions(SkFilterMode::kLinear, SkMipmapMode::kLinear));
#else
    imageSnapshot->scalePixels(single_pixel, SkFilterQuality::kMedium_SkFilterQuality);
#endif
    // convert color format and return average color
    return SkColor4f::FromBytes_RGBA(pixel[0]).toSkColor();
}
#else
Drawing::ColorQuad RSPropertiesPainter::CalcAverageColor(std::shared_ptr<Drawing::Image> imageSnapshot)
{
    // create a 1x1 SkPixmap
    return Drawing::ColorQuad();
}
#endif

int RSPropertiesPainter::GetAndResetBlurCnt()
{
    auto blurCnt = g_blurCnt;
    g_blurCnt = 0;
    return blurCnt;
}

void RSPropertiesPainter::DrawBackground(const RSProperties& properties, RSPaintFilterCanvas& canvas, bool isAntiAlias)
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
#ifdef NEW_SKIA
        if (properties.GetCornerRadius().IsZero()) {
            canvas.clipRect(Rect2SkRect(properties.GetBoundsRect()), isAntiAlias);
        } else {
            canvas.clipRRect(RRect2SkRRect(properties.GetRRect()), antiAlias);
        }
#else
        canvas.clipRRect(RRect2SkRRect(properties.GetRRect()), antiAlias);
#endif
    } else if (properties.GetClipToRRect()) {
        canvas.clipRRect(RRect2SkRRect(properties.GetClipRRect()), antiAlias);
    }
    // paint backgroundColor
    SkPaint paint;
    paint.setAntiAlias(antiAlias);
    auto bgColor = properties.GetBackgroundColor();
    if (bgColor != RgbPalette::Transparent()) {
        paint.setColor(bgColor.AsArgbInt());
        canvas.drawRRect(RRect2SkRRect(properties.GetRRect()), paint);
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
#ifdef NEW_SKIA
        bgImage->CanvasDrawImage(canvas, boundsRect, SkSamplingOptions(), paint, true);
#else
        bgImage->CanvasDrawImage(canvas, boundsRect, paint, true);
#endif
    }
#else
    if (properties.GetClipBounds() != nullptr) {
        auto& path = properties.GetClipBounds()->GetDrawingPath();
        if (path.GetDrawingType() == Drawing::DrawingType::RECORDING) {
            auto clipPath = static_cast<const Drawing::RecordingPath&>(path).GetCmdList()->Playback();
            canvas.ClipPath(*clipPath, Drawing::ClipOp::INTERSECT, antiAlias);
        } else {
            canvas.ClipPath(path, Drawing::ClipOp::INTERSECT, antiAlias);
        }
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
        canvas.Save();
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
        canvas.Restore();
    }
    if (const auto& bgImage = properties.GetBgImage()) {
        canvas.Save();
        canvas.ClipRoundRect(RRect2DrawingRRect(properties.GetRRect()), Drawing::ClipOp::INTERSECT, antiAlias);
        auto boundsRect = Rect2DrawingRect(properties.GetBoundsRect());
        bgImage->SetDstRect(properties.GetBgImageRect());
        canvas.AttachBrush(brush);
        bgImage->CanvasDrawImage(canvas, boundsRect, true);
        canvas.DetachBrush();
        canvas.Restore();
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

#ifndef USE_ROSEN_DRAWING
void RSPropertiesPainter::DrawBorder(const RSProperties& properties, SkCanvas& canvas)
{
    auto border = properties.GetBorder();
    if (!border || !border->HasBorder()) {
        return;
    }
    SkPaint paint;
    paint.setAntiAlias(true);
    if (border->ApplyFillStyle(paint)) {
        canvas.drawDRRect(RRect2SkRRect(properties.GetRRect()), RRect2SkRRect(properties.GetInnerRRect()), paint);
    } else if (properties.GetCornerRadius().IsZero() && border->ApplyFourLine(paint)) {
        RectF rect = properties.GetBoundsRect();
        border->PaintFourLine(canvas, paint, rect);
    } else if (border->ApplyPathStyle(paint)) {
        auto borderWidth = border->GetWidth();
        RRect rrect = properties.GetRRect();
        rrect.rect_.width_ -= borderWidth;
        rrect.rect_.height_ -= borderWidth;
        rrect.rect_.Move(borderWidth / PARAM_DOUBLE, borderWidth / PARAM_DOUBLE);
        SkPath borderPath;
        borderPath.addRRect(RRect2SkRRect(rrect));
        canvas.drawPath(borderPath, paint);
    } else {
        SkAutoCanvasRestore acr(&canvas, true);
        canvas.clipRRect(RRect2SkRRect(properties.GetInnerRRect()), SkClipOp::kDifference, true);
        SkRRect rrect = RRect2SkRRect(properties.GetRRect());
        paint.setStyle(SkPaint::Style::kStroke_Style);
        border->PaintTopPath(canvas, paint, rrect);
        border->PaintRightPath(canvas, paint, rrect);
        border->PaintBottomPath(canvas, paint, rrect);
        border->PaintLeftPath(canvas, paint, rrect);
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
    if (properties.GetCornerRadius().IsZero() && border->ApplyFourLine(pen)) {
        RectF rect = properties.GetBoundsRect();
        border->PaintFourLine(canvas, pen, rect);
    } else if (border->ApplyFillStyle(brush)) {
        canvas.AttachBrush(brush);
        canvas.DrawNestedRoundRect(
            RRect2DrawingRRect(properties.GetRRect()), RRect2DrawingRRect(properties.GetInnerRRect()));
        canvas.DetachBrush();
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
            0, 0, maskBounds.GetRight() - maskBounds.GetLeft(), maskBounds.GetBottom() - maskBounds.GetTop());
        canvas.AttachBrush(mask->GetMaskBrush());
        canvas.DrawRect(rect);
        canvas.DetachBrush();
    } else if (mask->IsPathMask()) {
        Drawing::AutoCanvasRestore maskSave(canvas, true);
        canvas.Translate(maskBounds.GetLeft(), maskBounds.GetTop());
        canvas.AttachBrush(mask->GetMaskBrush());
        canvas.DrawPath(mask->GetMaskPath());
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
#else
RectF RSPropertiesPainter::GetCmdsClipRect(Drawing::DrawCmdListPtr& cmds)
#endif
{
#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
    RectF clipRect;
    if (cmds == nullptr) {
        return clipRect;
    }
#ifndef USE_ROSEN_DRAWING
    SkRect rect;
    cmds->CheckClipRect(rect);
    clipRect = { rect.left(), rect.top(), rect.width(), rect.height() };
#else
    Drawing::Rect rect;
    cmds->CheckClipRect(rect);
    clipRect = { rect.GetLeft(), rect.GetTop(), rect.GetWidth(), rect.GetHeight() };
#endif
    return clipRect;
#else
    return RectF { 0.0f, 0.0f, 0.0f, 0.0f };
#endif
}

#ifndef USE_ROSEN_DRAWING
void RSPropertiesPainter::DrawFrameForDriven(const RSProperties& properties, RSPaintFilterCanvas& canvas,
                                             DrawCmdListPtr& cmds)
#else
void RSPropertiesPainter::DrawFrameForDriven(const RSProperties& properties, RSPaintFilterCanvas& canvas,
                                             Drawing::DrawCmdListPtr& cmds)
#endif
{
#if defined(RS_ENABLE_DRIVEN_RENDER) && defined(RS_ENABLE_GL)
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
    cmds->ReplaceDrivenCmds();
    cmds->Playback(canvas, &frameRect);
    cmds->RestoreOriginCmdsForDriven();
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
#ifdef NEW_SKIA
    paint.setShader(imageSnapshot->makeShader(SkTileMode::kClamp, SkTileMode::kClamp, SkSamplingOptions()));
#else
    paint.setShader(imageSnapshot->makeShader(SkTileMode::kClamp, SkTileMode::kClamp));
#endif

    float width = imageSnapshot->width();
    float height = imageSnapshot->height();
    float degree = properties.GetSpherize();
    bool isWidthGreater = width > height;
    ROSEN_LOGI("RSPropertiesPainter::DrawCachedSpherizeSurface spherize degree [%f]", degree);

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
    SkSamplingOptions options;
    canvas.drawImageRect(imageSnapshot, Rect2SkRect(properties.GetBoundsRect()), options, &paint);
#endif
}

void RSPropertiesPainter::DrawLightUpEffect(const RSProperties& properties, RSPaintFilterCanvas& canvas)
{
#ifndef USE_ROSEN_DRAWING
#ifdef NEW_SKIA
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
#endif
#endif
}

#ifndef USE_ROSEN_DRAWING
#ifdef NEW_SKIA
sk_sp<SkShader> RSPropertiesPainter::MakeLightUpEffectShader(float lightUpDeg, sk_sp<SkShader> imageShader)
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
    auto [effect, err] = SkRuntimeEffect::MakeForShader(SkString(prog));
    sk_sp<SkShader> children[] = {imageShader};
    size_t childCount = 1;
    return effect->makeShader(SkData::MakeWithCopy(
        &lightUpDeg, sizeof(lightUpDeg)), children, childCount, nullptr, false);
}
#endif
#endif

void RSPropertiesPainter::DrawDynamicLightUp(const RSProperties& properties, RSPaintFilterCanvas& canvas)
{
#ifndef USE_ROSEN_DRAWING
#ifdef NEW_SKIA
    SkSurface* skSurface = canvas.GetSurface();
    if (skSurface == nullptr) {
        ROSEN_LOGD("RSPropertiesPainter::DrawDynamicLightUp skSurface is null");
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
#endif
#endif
}

#ifndef USE_ROSEN_DRAWING
#ifdef NEW_SKIA
sk_sp<SkShader> RSPropertiesPainter::MakeDynamicLightUpShader(
    float dynamicLightUpRate, float dynamicLightUpDeg, sk_sp<SkShader> imageShader)
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
    auto [effect, err] = SkRuntimeEffect::MakeForShader(SkString(prog));
    if (!effect) {
        ROSEN_LOGE("MakeDynamicLightUpShader::RuntimeShader effect error: %s\n", err.c_str());
        return nullptr;
    }
    SkRuntimeShaderBuilder builder(effect);
    builder.child("imageShader") = imageShader;
    builder.uniform("dynamicLightUpRate") = dynamicLightUpRate;
    builder.uniform("dynamicLightUpDeg") = dynamicLightUpDeg;
    return builder.makeShader(nullptr, false);
}
#endif
#endif

void RSPropertiesPainter::DrawParticle(const RSProperties& properties, RSPaintFilterCanvas& canvas)
{
    auto particles = properties.GetParticles();
    auto bounds = properties.GetBoundsRect();
    for (size_t i = 0; i < particles.size(); i++) {
        if (particles[i]->IsAlive()) {
            // Get particle properties
            auto position = particles[i]->GetPosition();
            if (!(bounds.Intersect(position.x_, position.y_))) {
                continue;
            }
            float opacity = particles[i]->GetOpacity();
            auto particleType = particles[i]->GetParticleType();
            SkPaint paint;
            paint.setAntiAlias(true);
            paint.setAlphaf(opacity);

            if (particleType == ParticleType::POINTS) {
                auto radius = particles[i]->GetRadius();
                Color color = particles[i]->GetColor();
                auto alpha = color.GetAlpha();
                color.SetAlpha(alpha * opacity);
                paint.setColor(color.AsArgbInt());
                canvas.drawCircle(position.x_, position.y_, radius, paint);
            } else {
                auto image = particles[i]->GetImage();
                canvas.rotate(particles[i]->GetSpin());
                float fLeft = position.x_;
                float ftop = position.y_;
                auto imageSize = particles[i]->GetImageSize();
                float fRight = imageSize.x_;
                float fBottom = imageSize.y_;
                SkRect rect { fLeft, ftop, fRight, fBottom };
#ifdef NEW_SKIA
                image->CanvasDrawImage(canvas, rect, SkSamplingOptions(), paint, false);
#else
                image->CanvasDrawImage(canvas, rect, paint, false);
#endif
            }
        }
    }
}

} // namespace Rosen
} // namespace OHOS
