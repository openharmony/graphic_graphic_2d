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
#include "render/rs_drawing_filter.h"
#include "render/rs_foreground_effect_filter.h"
#include "render/rs_linear_gradient_blur_shader_filter.h"
#include "render/rs_skia_filter.h"
#include "render/rs_material_filter.h"
#include "platform/common/rs_system_properties.h"

#include <cstdint>
#include <algorithm>

#include "draw/canvas.h"
#include "draw/clip.h"
#include "drawing/draw/core_canvas.h"
#include "effect/runtime_blender_builder.h"
#include "effect/runtime_effect.h"
#include "effect/runtime_shader_builder.h"
#include "utils/rect.h"
#include "src/image/SkImage_Base.h"

namespace OHOS {
namespace Rosen {
namespace {
bool g_forceBgAntiAlias = true;
constexpr int PARAM_DOUBLE = 2;
constexpr int TRACE_LEVEL_TWO = 2;
constexpr float MIN_TRANS_RATIO = 0.0f;
constexpr float MAX_TRANS_RATIO = 0.95f;
constexpr float MIN_SPOT_RATIO = 1.0f;
constexpr float MAX_SPOT_RATIO = 1.95f;
constexpr float MAX_AMBIENT_RADIUS = 150.0f;
constexpr int MAX_LIGHT_SOURCES = 12;
// when the blur radius > SNAPSHOT_OUTSET_BLUR_RADIUS_THRESHOLD,
// the snapshot should call outset before blur to shrink by 1px.
constexpr static float SNAPSHOT_OUTSET_BLUR_RADIUS_THRESHOLD = 40.0f;
} // namespace

const bool RSPropertiesPainter::BLUR_ENABLED = RSSystemProperties::GetBlurEnabled();

std::shared_ptr<Drawing::RuntimeEffect> RSPropertiesPainter::greyAdjustEffect_ = nullptr;
std::shared_ptr<Drawing::RuntimeEffect> RSPropertiesPainter::binarizationShaderEffect_ = nullptr;
std::shared_ptr<Drawing::RuntimeEffect> RSPropertiesPainter::lightUpEffectShaderEffect_ = nullptr;
std::shared_ptr<Drawing::RuntimeEffect> RSPropertiesPainter::dynamicLightUpBlenderEffect_ = nullptr;
std::shared_ptr<Drawing::RuntimeEffect> RSPropertiesPainter::dynamicDimShaderEffect_ = nullptr;

Drawing::Rect RSPropertiesPainter::Rect2DrawingRect(const RectF& r)
{
    return Drawing::Rect(r.left_, r.top_, r.left_ + r.width_, r.top_ + r.height_);
}

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
            ROSEN_LOGE("GetGravityMatrix unknow gravity=[%{public}d]", gravity);
            return false;
        }
    }
}

void RSPropertiesPainter::Clip(Drawing::Canvas& canvas, RectF rect, bool isAntiAlias)
{
    canvas.ClipRect(Rect2DrawingRect(rect), Drawing::ClipOp::INTERSECT, isAntiAlias);
}

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

    auto& geoPtr = (properties.GetBoundsGeometry());
    Drawing::Matrix matrix = (geoPtr && isAbsCoordinate) ? geoPtr->GetAbsMatrix() : Drawing::Matrix();
    matrix.MapRect(shadowRect, shadowRect);

    dirtyShadow.left_ = shadowRect.GetLeft();
    dirtyShadow.top_ = shadowRect.GetTop();
    dirtyShadow.width_ = shadowRect.GetWidth();
    dirtyShadow.height_ = shadowRect.GetHeight();
}

void RSPropertiesPainter::DrawShadow(const RSProperties& properties, RSPaintFilterCanvas& canvas, const RRect* rrect)
{
    // skip shadow if not valid or cache is enabled
    if (properties.IsSpherizeValid() || !properties.IsShadowValid() ||
        canvas.GetCacheType() == RSPaintFilterCanvas::CacheType::ENABLED) {
        return;
    }
    RS_OPTIONAL_TRACE_NAME_FMT_LEVEL(TRACE_LEVEL_TWO,
        "RSPropertiesPainter::DrawShadow, ShadowElevation: %f, ShadowRadius: %f, ShadowOffsetX: "
        "%f, ShadowOffsetY: %f, bounds: %s",
        properties.GetShadowElevation(), properties.GetShadowRadius(), properties.GetShadowOffsetX(),
        properties.GetShadowOffsetY(), properties.GetBoundsGeometry()->GetAbsRect().ToString().c_str());
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
        canvas.DrawShadowStyle(path, planeParams, lightPos, DEFAULT_LIGHT_RADIUS,
            Drawing::Color(ambientColor.AsArgbInt()), Drawing::Color(spotColor.AsArgbInt()),
            Drawing::ShadowFlags::TRANSPARENT_OCCLUDER, true);
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

        vec4 main(vec2 drawing_coord) {
            vec3 color = vec3(imageShader(drawing_coord).r, imageShader(drawing_coord).g, imageShader(drawing_coord).b);
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

std::shared_ptr<Drawing::Image> RSPropertiesPainter::DrawGreyAdjustment(Drawing::Canvas& canvas,
    const std::shared_ptr<Drawing::Image>& image, const Vector2f& greyCoeff)
{
    if (image == nullptr) {
        ROSEN_LOGE("RSPropertiesPainter::DrawGreyAdjustment image is null");
        return nullptr;
    }
    RS_TRACE_NAME_FMT("RSPropertiesPainter::DrawGreyAdjustment, greyCoef1 is: %f, greyCoef2 is: %f",
        greyCoeff.x_, greyCoeff.y_);
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
    builder->SetUniform("coefficient1", greyCoeff.x_);
    builder->SetUniform("coefficient2", greyCoeff.y_);
    return builder->MakeImage(canvas.GetGPUContext().get(), nullptr, image->GetImageInfo(), false);
}

void RSPropertiesPainter::DrawForegroundFilter(const RSProperties& properties, RSPaintFilterCanvas& canvas)
{
    RS_OPTIONAL_TRACE_NAME("DrawForegroundFilter restore");
    auto surface = canvas.GetSurface();
    std::shared_ptr<Drawing::Image> imageSnapshot = nullptr;
    if (surface) {
        imageSnapshot = surface->GetImageSnapshot();
    } else {
        ROSEN_LOGD("RSPropertiesPainter::DrawForegroundFilter Surface null");
    }

    canvas.RestorePCanvasList();
    canvas.SwapBackMainScreenData();

    auto& RSFilter = properties.GetForegroundFilter();
    if (RSFilter == nullptr) {
        return;
    }

    if (imageSnapshot == nullptr) {
        ROSEN_LOGD("RSPropertiesPainter::DrawForegroundFilter image null");
        return;
    }
    auto foregroundFilter = std::static_pointer_cast<RSDrawingFilterOriginal>(RSFilter);

    if (foregroundFilter->GetFilterType() == RSFilter::MOTION_BLUR) {
        auto canvasOriginal = canvas.GetOriginalCanvas();
        foregroundFilter->SetGeometry(*canvasOriginal, 0.f, 0.f);
    }

    foregroundFilter->DrawImageRect(canvas, imageSnapshot, Drawing::Rect(0, 0, imageSnapshot->GetWidth(),
        imageSnapshot->GetHeight()), Drawing::Rect(0, 0, imageSnapshot->GetWidth(), imageSnapshot->GetHeight()));
}

void RSPropertiesPainter::DrawFilter(const RSProperties& properties, RSPaintFilterCanvas& canvas,
    FilterType filterType, const std::optional<Drawing::Rect>& rect, const std::shared_ptr<RSFilter>& externalFilter)
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

    if (RSFilter->GetFilterType() == RSFilter::MATERIAL) {
        float radius = 0.f;
        if (filterType == FilterType::BACKGROUND_FILTER) {
            radius = properties.GetBackgroundBlurRadius();
        } else if (filterType == FilterType::FOREGROUND_FILTER) {
            radius = properties.GetForegroundBlurRadius();
        }
        RSFilter->SetSnapshotOutset(radius >= SNAPSHOT_OUTSET_BLUR_RADIUS_THRESHOLD);
    }
    RS_OPTIONAL_TRACE_NAME("DrawFilter " + RSFilter->GetDescription());
    RS_OPTIONAL_TRACE_NAME_FMT_LEVEL(TRACE_LEVEL_TWO, "DrawFilter, filterType: %d, %s, bounds: %s", filterType,
        RSFilter->GetDetailedDescription().c_str(), properties.GetBoundsGeometry()->GetAbsRect().ToString().c_str());
    g_blurCnt++;
    Drawing::AutoCanvasRestore acr(canvas, true);

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
        std::shared_ptr<RSShaderFilter> rsShaderFilter =
        filter->GetShaderFilterWithType(RSShaderFilter::LINEAR_GRADIENT_BLUR);
        if (rsShaderFilter != nullptr) {
            auto tmpFilter = std::static_pointer_cast<RSLinearGradientBlurShaderFilter>(rsShaderFilter);
            tmpFilter->IsOffscreenCanvas(true);
            tmpFilter->SetGeometry(canvas, properties.GetFrameWidth(), properties.GetFrameHeight());
            filter->SetSnapshotOutset(false);
        }
        // RSFilterCacheManger has no more logic for evaluating filtered snapshot clearing
        // Should be passed as secnod argument, if required (see RSPropertyDrawableUtils::DrewFiler())
        cacheManager->DrawFilter(canvas, filter, {RSFilter->NeedSnapshotOutset(), false });
        return;
    }
#endif

    std::shared_ptr<RSShaderFilter> rsShaderFilter =
        filter->GetShaderFilterWithType(RSShaderFilter::LINEAR_GRADIENT_BLUR);
    if (rsShaderFilter != nullptr) {
        auto tmpFilter = std::static_pointer_cast<RSLinearGradientBlurShaderFilter>(rsShaderFilter);
        tmpFilter->IsOffscreenCanvas(true);
        tmpFilter->SetGeometry(canvas, properties.GetFrameWidth(), properties.GetFrameHeight());
        filter->SetSnapshotOutset(false);
    }
    auto clipIBounds = canvas.GetDeviceClipBounds();
    auto imageClipIBounds = clipIBounds;
    if (RSFilter->NeedSnapshotOutset()) {
        imageClipIBounds.MakeOutset(-1, -1);
    }
    auto imageSnapshot = surface->GetImageSnapshot(imageClipIBounds);
    if (imageSnapshot == nullptr) {
        ROSEN_LOGD("RSPropertiesPainter::DrawFilter image null");
        return;
    }
    if (RSSystemProperties::GetImageGpuResourceCacheEnable(imageSnapshot->GetWidth(), imageSnapshot->GetHeight())) {
        ROSEN_LOGD("DrawFilter cache image resource(width:%{public}d, height:%{public}d).",
            imageSnapshot->GetWidth(), imageSnapshot->GetHeight());
        imageSnapshot->HintCacheGpuResource();
    }

    filter->PreProcess(imageSnapshot);
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
        canvas.ClipRect(RSPropertiesPainter::Rect2DrawingRect(boundsRect));
        auto filter = std::static_pointer_cast<RSDrawingFilter>(properties.GetBackgroundFilter());
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
    auto offscreenSurface = surface->MakeSurface(canvas.GetWidth(), canvas.GetHeight());
    auto offscreenCanvas = std::make_shared<RSPaintFilterCanvas>(offscreenSurface.get());
    // copy matrix and other properties to offscreen canvas
    offscreenCanvas->SetMatrix(canvas.GetTotalMatrix());
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
    RS_OPTIONAL_TRACE_NAME_FMT_LEVEL(TRACE_LEVEL_TWO, "EffectComponent, %s, bounds: %s",
        RSFilter->GetDetailedDescription().c_str(), properties.GetBoundsGeometry()->GetAbsRect().ToString().c_str());
    auto surface = canvas.GetSurface();
    if (surface == nullptr) {
        ROSEN_LOGE("RSPropertiesPainter::DrawBackgroundEffect surface null");
        return;
    }

    canvas.Save();
    canvas.ClipRect(Rect2DrawingRect(properties.GetBoundsRect()));
    auto bounds = canvas.GetDeviceClipBounds();
    canvas.Restore();
    auto filter = std::static_pointer_cast<RSDrawingFilter>(RSFilter);

#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    // Optional use cacheManager to draw filter
    if (auto& cacheManager = properties.GetFilterCacheManager(false);
        cacheManager != nullptr && !canvas.GetDisableFilterCache()) {
        auto node = properties.backref_.lock();
        if (node == nullptr) {
            ROSEN_LOGE("DrawBackgroundEffect::node is null");
            return;
        }
        auto effectNode = node->ReinterpretCastTo<RSEffectRenderNode>();
        if (effectNode == nullptr) {
            ROSEN_LOGE("DrawBackgroundEffect::node reinterpret cast failed.");
            return;
        }
        auto&& data = cacheManager->GeneratedCachedEffectData(canvas, filter, bounds, bounds);
        canvas.SetEffectData(data);
        return;
    }
#endif

    auto imageRect = bounds;
    auto imageSnapshot = surface->GetImageSnapshot(imageRect);
    if (imageSnapshot == nullptr) {
        ROSEN_LOGE("RSPropertiesPainter::DrawBackgroundEffect image snapshot null");
        return;
    }

    filter->PreProcess(imageSnapshot);
    // create a offscreen skSurface
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
    canvas.ResetMatrix();
    auto visibleRect = canvas.GetVisibleRect();
    visibleRect.Round();
    auto visibleIRect = Drawing::RectI(
        static_cast<int>(visibleRect.GetLeft()), static_cast<int>(visibleRect.GetTop()),
        static_cast<int>(visibleRect.GetRight()), static_cast<int>(visibleRect.GetBottom()));
    if (!visibleIRect.IsEmpty()) {
        canvas.ClipIRect(visibleIRect, Drawing::ClipOp::INTERSECT);
    }
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
}

void RSPropertiesPainter::GetPixelStretchDirtyRect(RectI& dirtyPixelStretch,
    const RSProperties& properties, const bool isAbsCoordinate)
{
    auto& pixelStretch = properties.GetPixelStretch();
    if (!pixelStretch.has_value()) {
        return;
    }
    auto boundsRect = properties.GetBoundsRect();
    auto scaledBounds = RectF(boundsRect.left_ - pixelStretch->x_, boundsRect.top_ - pixelStretch->y_,
        boundsRect.width_ + pixelStretch->x_ + pixelStretch->z_,
        boundsRect.height_ + pixelStretch->y_ + pixelStretch->w_);
    auto& geoPtr = properties.GetBoundsGeometry();
    Drawing::Matrix matrix = (geoPtr && isAbsCoordinate) ? geoPtr->GetAbsMatrix() : Drawing::Matrix();
    auto drawingRect = Rect2DrawingRect(scaledBounds);
    matrix.MapRect(drawingRect, drawingRect);
    dirtyPixelStretch.left_ = std::floor(drawingRect.GetLeft());
    dirtyPixelStretch.top_ = std::floor(drawingRect.GetTop());
    dirtyPixelStretch.width_ = std::ceil(drawingRect.GetWidth()) + PARAM_DOUBLE;
    dirtyPixelStretch.height_ = std::ceil(drawingRect.GetHeight()) + PARAM_DOUBLE;
}

void RSPropertiesPainter::DrawPixelStretch(const RSProperties& properties, RSPaintFilterCanvas& canvas)
{
    auto& pixelStretch = properties.GetPixelStretch();
    if (!pixelStretch.has_value()) {
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
        tmpBounds.GetLeft(), tmpBounds.GetTop(), tmpBounds.GetRight() - 1, tmpBounds.GetBottom() - 1);
    canvas.Restore();

    RS_OPTIONAL_TRACE_NAME_FMT_LEVEL(TRACE_LEVEL_TWO, "RSPropertiesPainter::DrawPixelStretch, right: %f, bottom: %f",
        tmpBounds.GetRight(), tmpBounds.GetBottom());

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
    auto& boundsGeo = (properties.GetBoundsGeometry());
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
    auto pixelStretchTileMode = static_cast<Drawing::TileMode>(properties.GetPixelStretchTileMode());
    if (pixelStretch->x_ > EPS || pixelStretch->y_ > EPS || pixelStretch->z_ > EPS || pixelStretch->w_ > EPS) {
        brush.SetShaderEffect(Drawing::ShaderEffect::CreateImageShader(
            *image, pixelStretchTileMode, pixelStretchTileMode, samplingOptions, inverseMat));
        canvas.AttachBrush(brush);
        canvas.DrawRect(Drawing::Rect(-pixelStretch->x_, -pixelStretch->y_,
            -pixelStretch->x_ + scaledBounds.GetWidth(), -pixelStretch->y_ + scaledBounds.GetHeight()));
        canvas.DetachBrush();
    } else {
        inverseMat.PostScale(scaledBounds.GetWidth() / bounds.GetWidth(), scaledBounds.GetHeight() / bounds.GetHeight());
        brush.SetShaderEffect(Drawing::ShaderEffect::CreateImageShader(
            *image, pixelStretchTileMode, pixelStretchTileMode, samplingOptions, inverseMat));

        canvas.Translate(-pixelStretch->x_, -pixelStretch->y_);
        canvas.AttachBrush(brush);
        canvas.DrawRect(Drawing::Rect(pixelStretch->x_, pixelStretch->y_,
            pixelStretch->x_ + bounds.GetWidth(), pixelStretch->y_ + bounds.GetHeight()));
        canvas.DetachBrush();
    }
    canvas.Restore();
}

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
}

void RSPropertiesPainter::SetBgAntiAlias(bool forceBgAntiAlias)
{
    g_forceBgAntiAlias = forceBgAntiAlias;
}

bool RSPropertiesPainter::GetBgAntiAlias()
{
    return g_forceBgAntiAlias;
}

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

RRect RSPropertiesPainter::GetRRectForDrawingBorder(const RSProperties& properties,
    const std::shared_ptr<RSBorder>& border, const bool isOutline)
{
    if (!border) {
        return RRect();
    }

    return isOutline ?
        RRect(properties.GetRRect().rect_.MakeOutset(border->GetWidthFour()), border->GetRadiusFour()) :
        properties.GetRRect();
}

RRect RSPropertiesPainter::GetInnerRRectForDrawingBorder(const RSProperties& properties,
    const std::shared_ptr<RSBorder>& border, const bool isOutline)
{
    if (!border) {
        return RRect();
    }
    return isOutline ? properties.GetRRect() : properties.GetInnerRRect();
}

static std::shared_ptr<Drawing::RuntimeShaderBuilder> phongShaderBuilder = nullptr;

const std::shared_ptr<Drawing::RuntimeShaderBuilder>& RSPropertiesPainter::GetPhongShaderBuilder()
{
    if (phongShaderBuilder) {
        return phongShaderBuilder;
    }
    std::shared_ptr<Drawing::RuntimeEffect> lightEffect_;
    std::string lightString(R"(
        uniform vec4 lightPos[12];
        uniform vec4 viewPos[12];
        uniform vec4 specularLightColor[12];
        uniform float specularStrength[12];

        mediump vec4 main(vec2 drawing_coord) {
            vec4 lightColor = vec4(1.0, 1.0, 1.0, 1.0);
            float ambientStrength = 0.0;
            vec4 diffuseColor = vec4(1.0, 1.0, 1.0, 1.0);
            float diffuseStrength = 0.0;
            float shininess = 8.0;
            mediump vec4 fragColor = vec4(0.0, 0.0, 0.0, 0.0);
            vec4 NormalMap = vec4(0.0, 0.0, 1.0, 0.0);
            // ambient
            vec4 ambient = lightColor * ambientStrength;
            vec3 norm = normalize(NormalMap.rgb);

            for (int i = 0; i < 12; i++) {
                if (abs(specularStrength[i]) > 0.01) {
                    vec3 lightDir = normalize(vec3(lightPos[i].xy - drawing_coord, lightPos[i].z));
                    float diff = max(dot(norm, lightDir), 0.0);
                    vec4 diffuse = diff * lightColor;
                    vec3 viewDir = normalize(vec3(viewPos[i].xy - drawing_coord, viewPos[i].z)); // view vector
                    vec3 halfwayDir = normalize(lightDir + viewDir); // half vector
                    float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess); // exponential relationship of angle
                    vec4 specular = lightColor * spec; // multiply color of incident light
                    vec4 o = ambient + diffuse * diffuseStrength * diffuseColor; // diffuse reflection
                    vec4 specularColor = specularLightColor[i];
                    fragColor = fragColor + o + specular * specularStrength[i] * specularColor;
                }
            }
            return fragColor;
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

void RSPropertiesPainter::DrawLight(const RSProperties& properties, Drawing::Canvas& canvas)
{
    auto lightBuilder = GetPhongShaderBuilder();
    if (!lightBuilder) {
        return;
    }
    const auto& lightSourcesAndPosMap = properties.GetIlluminated()->GetLightSourcesAndPosMap();
    if (lightSourcesAndPosMap.empty()) {
        ROSEN_LOGD("RSPropertiesPainter::DrawLight lightSourceList is empty!");
        return;
    }
    const auto& geoPtr = (properties.GetBoundsGeometry());
    if (!geoPtr || geoPtr->IsEmpty()) {
        return;
    }
    std::vector<std::pair<std::shared_ptr<RSLightSource>, Vector4f>> lightSourcesAndPosVec(
        lightSourcesAndPosMap.begin(), lightSourcesAndPosMap.end());
    if (lightSourcesAndPosVec.size() > MAX_LIGHT_SOURCES) {
        std::sort(lightSourcesAndPosVec.begin(), lightSourcesAndPosVec.end(), [](const auto& x, const auto& y) {
            return x.second.x_ * x.second.x_ + x.second.y_ * x.second.y_ <
                   y.second.x_ * y.second.x_ + y.second.y_ * y.second.y_;
        });
    }
    DrawLightInner(properties, canvas, lightBuilder, lightSourcesAndPosVec, geoPtr);
}

void RSPropertiesPainter::DrawLightInner(const RSProperties& properties, Drawing::Canvas& canvas,
    std::shared_ptr<Drawing::RuntimeShaderBuilder>& lightBuilder,
    const std::vector<std::pair<std::shared_ptr<RSLightSource>, Vector4f>>& lightSourcesAndPosVec,
    const std::shared_ptr<RSObjAbsGeometry>& geoPtr)
{
    auto cnt = 0;
    constexpr int vectorLen = 4;
    float lightPosArray[vectorLen * MAX_LIGHT_SOURCES] = { 0 };
    float viewPosArray[vectorLen * MAX_LIGHT_SOURCES] = { 0 };
    float lightColorArray[vectorLen * MAX_LIGHT_SOURCES] = { 0 };
    float lightIntensityArray[MAX_LIGHT_SOURCES] = { 0 };

    auto iter = lightSourcesAndPosVec.begin();
    while (iter != lightSourcesAndPosVec.end() && cnt < MAX_LIGHT_SOURCES) {
        auto lightPos = iter->second;
        auto lightIntensity = iter->first->GetLightIntensity();
        auto lightColor = iter->first->GetLightColor();
        Vector4f lightColorVec =
            Vector4f(lightColor.GetRed(), lightColor.GetGreen(), lightColor.GetBlue(), lightColor.GetAlpha());
        for (int i = 0; i < vectorLen; i++) {
            lightPosArray[cnt * vectorLen + i] = lightPos[i];
            viewPosArray[cnt * vectorLen + i] = lightPos[i];
            lightColorArray[cnt * vectorLen + i] = lightColorVec[i] / UINT8_MAX;
        }
        lightIntensityArray[cnt] = lightIntensity;
        iter++;
        cnt++;
    }
    lightBuilder->SetUniform("lightPos", lightPosArray, vectorLen * MAX_LIGHT_SOURCES);
    lightBuilder->SetUniform("viewPos", viewPosArray, vectorLen * MAX_LIGHT_SOURCES);
    lightBuilder->SetUniform("specularLightColor", lightColorArray, vectorLen * MAX_LIGHT_SOURCES);
    Drawing::Pen pen;
    Drawing::Brush brush;
    pen.SetAntiAlias(true);
    brush.SetAntiAlias(true);
    auto illuminatedType = properties.GetIlluminated()->GetIlluminatedType();
    ROSEN_LOGD("RSPropertiesPainter::DrawLight illuminatedType:%{public}d", illuminatedType);
    if (illuminatedType == IlluminatedType::CONTENT) {
        DrawContentLight(properties, canvas, lightBuilder, brush, lightIntensityArray);
    } else if (illuminatedType == IlluminatedType::BORDER) {
        DrawBorderLight(properties, canvas, lightBuilder, pen, lightIntensityArray);
    } else if (illuminatedType == IlluminatedType::BORDER_CONTENT) {
        DrawContentLight(properties, canvas, lightBuilder, brush, lightIntensityArray);
        DrawBorderLight(properties, canvas, lightBuilder, pen, lightIntensityArray);
    }
}

void RSPropertiesPainter::DrawContentLight(const RSProperties& properties, Drawing::Canvas& canvas,
    std::shared_ptr<Drawing::RuntimeShaderBuilder>& lightBuilder, Drawing::Brush& brush,
    const float lightIntensityArray[])
{
    // content light
    std::shared_ptr<Drawing::ShaderEffect> shader;
    constexpr float contentIntensityCoefficient = 0.3f;
    float specularStrengthArr[MAX_LIGHT_SOURCES] = { 0 };
    for (int i = 0; i < MAX_LIGHT_SOURCES; i++) {
        specularStrengthArr[i] = lightIntensityArray[i] * contentIntensityCoefficient;
    }
    lightBuilder->SetUniform("specularStrength", specularStrengthArr, MAX_LIGHT_SOURCES);
    shader = lightBuilder->MakeShader(nullptr, false);
    brush.SetShaderEffect(shader);
    canvas.AttachBrush(brush);
    canvas.DrawRoundRect(RRect2DrawingRRect(properties.GetRRect()));
    canvas.DetachBrush();
}

void RSPropertiesPainter::DrawBorderLight(const RSProperties& properties, Drawing::Canvas& canvas,
    std::shared_ptr<Drawing::RuntimeShaderBuilder>& lightBuilder, Drawing::Pen& pen,
    const float lightIntensityArray[])
{
    // border light
    std::shared_ptr<Drawing::ShaderEffect> shader;
    lightBuilder->SetUniform("specularStrength", lightIntensityArray, MAX_LIGHT_SOURCES);
    shader = lightBuilder->MakeShader(nullptr, false);
    pen.SetShaderEffect(shader);
    float borderWidth = std::ceil(properties.GetIlluminatedBorderWidth());
    pen.SetWidth(borderWidth);
    auto borderRect = properties.GetRRect().rect_;
    float borderRadius = properties.GetRRect().radius_[0].x_;
    auto borderRRect = RRect(RectF(borderRect.left_ + borderWidth / 2.0f, borderRect.top_ + borderWidth / 2.0f,
        borderRect.width_ - borderWidth, borderRect.height_ - borderWidth),
        borderRadius - borderWidth / 2.0f, borderRadius - borderWidth / 2.0f);
    canvas.AttachPen(pen);
    canvas.DrawRoundRect(RRect2DrawingRRect(borderRRect));
    canvas.DetachPen();
}

void RSPropertiesPainter::DrawBorderBase(const RSProperties& properties, Drawing::Canvas& canvas,
    const std::shared_ptr<RSBorder>& border, const bool isOutline)
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
        bool isZero = isOutline ? border->GetRadiusFour().IsZero() : properties.GetCornerRadius().IsZero();
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

void RSPropertiesPainter::DrawBorder(const RSProperties& properties, Drawing::Canvas& canvas)
{
    auto border = properties.GetBorder();
    if (border && border->HasBorder()) {
        DrawBorderBase(properties, canvas, border, false);
    }
}

void RSPropertiesPainter::GetOutlineDirtyRect(RectI& dirtyOutline,
    const RSProperties& properties, const bool isAbsCoordinate)
{
    auto outline = properties.GetOutline();
    if (!outline || !outline->HasBorder()) {
        return;
    }

    auto& geoPtr = properties.GetBoundsGeometry();
    Drawing::Matrix matrix = (geoPtr && isAbsCoordinate) ? geoPtr->GetAbsMatrix() : Drawing::Matrix();
    auto drawingRect = Rect2DrawingRect(GetRRectForDrawingBorder(properties, outline, true).rect_);
    matrix.MapRect(drawingRect, drawingRect);
    dirtyOutline.left_ = std::floor(drawingRect.GetLeft());
    dirtyOutline.top_ = std::floor(drawingRect.GetTop());
    dirtyOutline.width_ = std::ceil(drawingRect.GetWidth()) + PARAM_DOUBLE;
    dirtyOutline.height_ = std::ceil(drawingRect.GetHeight()) + PARAM_DOUBLE;
}

void RSPropertiesPainter::DrawOutline(const RSProperties& properties, Drawing::Canvas& canvas)
{
    auto outline = properties.GetOutline();
    if (outline && outline->HasBorder()) {
        DrawBorderBase(properties, canvas, outline, true);
    }
}

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
    } else if (mask->IsPixelMapMask()) {
        Drawing::AutoCanvasRestore arc(canvas, true);
        if (mask->GetImage()) {
            canvas.DrawImage(*mask->GetImage(), 0.f, 0.f, Drawing::SamplingOptions());
        }
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

void RSPropertiesPainter::DrawMask(const RSProperties& properties, Drawing::Canvas& canvas)
{
    Drawing::Rect maskBounds = Rect2DrawingRect(properties.GetBoundsRect());
    DrawMask(properties, canvas, maskBounds);
}

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

void RSPropertiesPainter::DrawColorFilter(const RSProperties& properties, RSPaintFilterCanvas& canvas)
{
    // if useEffect defined, use color filter from parent EffectView.
    auto& colorFilter = properties.GetColorFilter();
    if (colorFilter == nullptr) {
        return;
    }
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
}

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

std::shared_ptr<Drawing::ShaderEffect> RSPropertiesPainter::MakeBinarizationShader(float low, float high,
    float thresholdLow, float thresholdHigh, std::shared_ptr<Drawing::ShaderEffect> imageShader)
{
    static constexpr char prog[] = R"(
        uniform mediump float ubo_low;
        uniform mediump float ubo_high;
        uniform mediump float ubo_thresholdLow;
        uniform mediump float ubo_thresholdHigh;
        uniform shader imageShader;
        mediump vec4 main(vec2 drawing_coord) {
            mediump vec3 c = imageShader(drawing_coord).rgb;
            float gray = 0.299 * c.r + 0.587 * c.g + 0.114 * c.b;
            float lowRes = mix(ubo_high, -1.0, step(ubo_thresholdLow, gray));
            float highRes = mix(-1.0, ubo_low, step(ubo_thresholdHigh, gray));
            float midRes = (ubo_thresholdHigh - gray) * (ubo_high - ubo_low) /
            (ubo_thresholdHigh - ubo_thresholdLow) + ubo_low;
            float invertedGray = mix(midRes, max(lowRes, highRes), step(-0.5, max(lowRes, highRes)));
            mediump vec3 invert = vec3(invertedGray);
            mediump vec4 res = vec4(invert, 1.0);
            return res;
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
    builder->SetUniform("ubo_low", low);
    builder->SetUniform("ubo_high", high);
    builder->SetUniform("ubo_thresholdLow", thresholdLow);
    builder->SetUniform("ubo_thresholdHigh", thresholdHigh);
    return builder->MakeShader(nullptr, false);
}

void RSPropertiesPainter::DrawLightUpEffect(const RSProperties& properties, RSPaintFilterCanvas& canvas)
{
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
}

std::shared_ptr<Drawing::ShaderEffect> RSPropertiesPainter::MakeLightUpEffectShader(
    float lightUpDeg, std::shared_ptr<Drawing::ShaderEffect> imageShader)
{
    // Realizations locate in SkiaShaderEffect::InitWithLightUp & DDGRShaderEffect::InitWithLightUp
    return Drawing::ShaderEffect::CreateLightUp(lightUpDeg, *imageShader);
}


void RSPropertiesPainter::DrawDynamicLightUp(const RSProperties& properties, RSPaintFilterCanvas& canvas)
{
    RS_OPTIONAL_TRACE_NAME_FMT_LEVEL(TRACE_LEVEL_TWO, "DrawDynamicLightUp, rate: %f, degree: %f, bounds: %s",
        properties.GetDynamicLightUpRate().value(), properties.GetDynamicLightUpDegree().value(),
        properties.GetBoundsGeometry()->GetAbsRect().ToString().c_str());
    Drawing::Surface* surface = canvas.GetSurface();
    if (surface == nullptr) {
        ROSEN_LOGD("RSPropertiesPainter::DrawDynamicLightUp surface is null");
        return;
    }

    auto blender = MakeDynamicLightUpBlender(properties.GetDynamicLightUpRate().value() * canvas.GetAlpha(),
        properties.GetDynamicLightUpDegree().value() * canvas.GetAlpha());
    Drawing::Brush brush;
    brush.SetBlender(blender);
    canvas.DrawBackground(brush);
}

std::shared_ptr<Drawing::Blender> RSPropertiesPainter::MakeDynamicLightUpBlender(
    float dynamicLightUpRate, float dynamicLightUpDeg)
{
    static constexpr char prog[] = R"(
        uniform float dynamicLightUpRate;
        uniform float dynamicLightUpDeg;

        vec4 main(vec4 drawing_src, vec4 drawing_dst) {
            float x = 0.299 * drawing_dst.r + 0.587 * drawing_dst.g + 0.114 * drawing_dst.b;
            float y = (0 - dynamicLightUpRate) * x + dynamicLightUpDeg;
            float R = clamp((drawing_dst.r + y), 0.0, 1.0);
            float G = clamp((drawing_dst.g + y), 0.0, 1.0);
            float B = clamp((drawing_dst.b + y), 0.0, 1.0);
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
}

void RSPropertiesPainter::DrawDynamicDim(const RSProperties& properties, RSPaintFilterCanvas& canvas)
{
    Drawing::Surface* surface = canvas.GetSurface();
    if (surface == nullptr) {
        ROSEN_LOGD("RSPropertiesPainter::DrawDynamicDim surface is null");
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
        ROSEN_LOGE("RSPropertiesPainter::DrawDynamicDim image is null");
        return;
    }
    Drawing::Matrix scaleMat;
    auto imageShader = Drawing::ShaderEffect::CreateImageShader(
        *image, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP,
        Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), scaleMat);
    auto shader = MakeDynamicDimShader(properties.GetDynamicDimDegree().value(), imageShader);
    if (shader == nullptr) {
        ROSEN_LOGE("RSPropertiesPainter::DrawDynamicDim shader is null");
        return;
    }
    Drawing::Brush brush;
    brush.SetShaderEffect(shader);
    canvas.ResetMatrix();
    canvas.Translate(clipBounds.GetLeft(), clipBounds.GetTop());
    canvas.DrawBackground(brush);
}

std::shared_ptr<Drawing::ShaderEffect> RSPropertiesPainter::MakeDynamicDimShader(
    float dynamicDimDeg, std::shared_ptr<Drawing::ShaderEffect> imageShader)
{
    static constexpr char prog[] = R"(
        uniform half dynamicDimDeg;
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
            hsv.y = hsv.y * 0.8;
            hsv.z = min(hsv.z * dynamicDimDeg, 1.0);
            return vec4(hsv2rgb(hsv), 1.0);
        }
    )";
    if (dynamicDimShaderEffect_ == nullptr) {
        dynamicDimShaderEffect_ = Drawing::RuntimeEffect::CreateForShader(prog);
        if (dynamicDimShaderEffect_ == nullptr) {
            ROSEN_LOGE("MakeBinarizationShader::RuntimeShader effect error\n");
            return nullptr;
        }
    }
    std::shared_ptr<Drawing::ShaderEffect> children[] = {imageShader};
    size_t childCount = 1;
    auto data = std::make_shared<Drawing::Data>();
    data->BuildWithCopy(&dynamicDimDeg, sizeof(dynamicDimDeg));
    return dynamicDimShaderEffect_->MakeShader(data, children, childCount, nullptr, false);
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
            Drawing::Brush brush;
            brush.SetAntiAlias(true);
            brush.SetAlphaF(opacity);
            auto clipBounds = Drawing::Rect(
                bounds->left_, bounds->top_, bounds->left_ + bounds->width_, bounds->top_ + bounds->height_);
            canvas.ClipRect(clipBounds, Drawing::ClipOp::INTERSECT, true);

            if (particleType == ParticleType::POINTS) {
                auto radius = particle->GetRadius();
                Color color = particle->GetColor();
                auto alpha = color.GetAlpha();
                color.SetAlpha(alpha * opacity);
                brush.SetColor(color.AsArgbInt());
                canvas.AttachBrush(brush);
                canvas.DrawCircle(Drawing::Point(position.x_, position.y_), radius * scale);
                canvas.DetachBrush();
            } else {
                auto imageSize = particle->GetImageSize();
                auto image = particle->GetImage();
                float left = position.x_;
                float top = position.y_;
                float right = position.x_ + imageSize.x_ * scale;
                float bottom = position.y_ + imageSize.y_ * scale;
                canvas.Save();
                canvas.Translate(position.x_, position.y_);
                canvas.Rotate(particle->GetSpin(), imageSize.x_ * scale / 2.f, imageSize.y_ * scale / 2.f);
                RectF destRect(left, top, right, bottom);
                image->SetDstRect(destRect);
                image->SetScale(scale);
                image->SetImageRepeat(0);
                Drawing::Rect rect { left, top, right, bottom };
                canvas.AttachBrush(brush);
                image->CanvasDrawImage(canvas, rect, Drawing::SamplingOptions(), false);
                canvas.DetachBrush();
                canvas.Restore();
            }
        }
    }
}

bool RSPropertiesPainter::IsDangerousBlendMode(int blendMode, int blendApplyType)
{
    static const uint32_t fastDangerousBit =
        (1 << static_cast<int>(Drawing::BlendMode::CLEAR)) +
        (1 << static_cast<int>(Drawing::BlendMode::SRC_OUT)) +
        (1 << static_cast<int>(Drawing::BlendMode::DST_OUT)) +
        (1 << static_cast<int>(Drawing::BlendMode::XOR));
    static const uint32_t offscreenDangerousBit =
        (1 << static_cast<int>(Drawing::BlendMode::CLEAR)) +
        (1 << static_cast<int>(Drawing::BlendMode::SRC)) +
        (1 << static_cast<int>(Drawing::BlendMode::SRC_IN)) +
        (1 << static_cast<int>(Drawing::BlendMode::DST_IN)) +
        (1 << static_cast<int>(Drawing::BlendMode::SRC_OUT)) +
        (1 << static_cast<int>(Drawing::BlendMode::DST_OUT)) +
        (1 << static_cast<int>(Drawing::BlendMode::DST_ATOP)) +
        (1 << static_cast<int>(Drawing::BlendMode::XOR)) +
        (1 << static_cast<int>(Drawing::BlendMode::MODULATE));
    uint32_t tmp = 1 << blendMode;
    if (blendApplyType == static_cast<int>(RSColorBlendApplyType::FAST)) {
        return tmp & fastDangerousBit;
    }
    return tmp & offscreenDangerousBit;
}
} // namespace Rosen
} // namespace OHOS
