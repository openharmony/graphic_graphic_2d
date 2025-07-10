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
#include "render/rs_distortion_shader_filter.h"
#include "render/rs_foreground_effect_filter.h"
#include "render/rs_render_kawase_blur_filter.h"
#include "render/rs_render_linear_gradient_blur_filter.h"
#include "render/rs_skia_filter.h"
#include "render/rs_render_magnifier_filter.h"
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
} // namespace

const bool RSPropertiesPainter::BLUR_ENABLED = RSSystemProperties::GetBlurEnabled();

std::shared_ptr<Drawing::RuntimeEffect> RSPropertiesPainter::greyAdjustEffect_ = nullptr;

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

bool RSPropertiesPainter::GetScalingModeMatrix(ScalingMode scalingMode, RectF bounds,
    float bufferWidth, float bufferHeight, Drawing::Matrix& scalingModeMatrix)
{
    if (ROSEN_EQ(bufferWidth, bounds.width_) && ROSEN_EQ(bufferHeight, bounds.height_)) {
        return false;
    }
    scalingModeMatrix = Drawing::Matrix();
    switch (scalingMode) {
        case ScalingMode::SCALING_MODE_SCALE_CROP: {
            if (ROSEN_EQ(bufferWidth, 0.f) || ROSEN_EQ(bufferHeight, 0.f)) {
                return false;
            }
            float scale = std::max(bounds.width_ / bufferWidth, bounds.height_ / bufferHeight);
            if (ROSEN_NE(scale, 0.f)) {
                scalingModeMatrix.PreScale(scale, scale);
                scalingModeMatrix.PreTranslate((bounds.width_ / scale - bufferWidth) / PARAM_DOUBLE,
                    (bounds.height_ / scale - bufferHeight) / PARAM_DOUBLE);
                return true;
            }
            return false;
        }
        case ScalingMode::SCALING_MODE_SCALE_FIT: {
            if (ROSEN_EQ(bufferWidth, 0.f) || ROSEN_EQ(bufferHeight, 0.f)) {
                return false;
            }
            float scale = std::min(bounds.width_ / bufferWidth, bounds.height_ / bufferHeight);
            if (ROSEN_NE(scale, 0.f)) {
                scalingModeMatrix.PreScale(scale, scale);
                scalingModeMatrix.PreTranslate((bounds.width_ / scale - bufferWidth) / PARAM_DOUBLE,
                    (bounds.height_ / scale - bufferHeight) / PARAM_DOUBLE);
                return true;
            }
            return false;
        }
        case ScalingMode::SCALING_MODE_NO_SCALE_CROP: {
            if (ROSEN_EQ(bufferWidth, 0.f) || ROSEN_EQ(bufferHeight, 0.f)) {
                return false;
            }
            scalingModeMatrix.PreTranslate((bounds.width_ - bufferWidth) / PARAM_DOUBLE,
                (bounds.height_ - bufferHeight) / PARAM_DOUBLE);
            return true;
        }
        case ScalingMode::SCALING_MODE_FREEZE: [[fallthrough]];
        case ScalingMode::SCALING_MODE_SCALE_TO_WINDOW: {
            return true;
        }
        default: {
            ROSEN_LOGE("GetScalingModeMatrix: Unknown ScalingMode=[%{public}d]", static_cast<int>(scalingMode));
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
    // avoid 1 pixel reduction caused by converting float to int
    shadowRect.MakeOutset(1, 1);

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
        node->InternalDrawContent(canvas, false);
    }
}

void RSPropertiesPainter::DrawShadowInner(
    const RSProperties& properties, RSPaintFilterCanvas& canvas, Drawing::Path& path)
{
    path.Offset(properties.GetShadowOffsetX(), properties.GetShadowOffsetY());
    Color spotColor = properties.GetShadowColor();
    auto deviceClipBounds = canvas.GetDeviceClipBounds();

    // The translation of the matrix is rounded to improve the hit ratio of skia blurfilter cache,
    // the function <compute_key_and_clip_bounds> in <skia/src/gpu/GrBlurUtil.cpp> for more details.
    RSAutoCanvasRestore rst(&canvas);
    auto matrix = canvas.GetTotalMatrix();
    matrix.Set(Drawing::Matrix::TRANS_X, std::ceil(matrix.Get(Drawing::Matrix::TRANS_X)));
    matrix.Set(Drawing::Matrix::TRANS_Y, std::ceil(matrix.Get(Drawing::Matrix::TRANS_Y)));
    canvas.SetMatrix(matrix);

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
            spotColor.GetAlpha(), spotColor.GetRed(), spotColor.GetGreen(), spotColor.GetBlue()));
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
#ifdef RS_ENABLE_GPU
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
#else
    return nullptr;
#endif
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

    RS_OPTIONAL_TRACE_NAME("DrawFilter " + RSFilter->GetDescription());
    RS_OPTIONAL_TRACE_NAME_FMT_LEVEL(TRACE_LEVEL_TWO, "DrawFilter, filterType: %d, %s, bounds: %s", filterType,
        RSFilter->GetDetailedDescription().c_str(), properties.GetBoundsGeometry()->GetAbsRect().ToString().c_str());
    g_blurCnt++;
    Drawing::AutoCanvasRestore acr(canvas, true);

    auto filter = std::static_pointer_cast<RSDrawingFilter>(RSFilter);
    auto surface = canvas.GetSurface();
    if (surface == nullptr) {
        ROSEN_LOGD("RSPropertiesPainter::DrawFilter surface null");
        Drawing::Brush brush;
        brush.SetAntiAlias(true);
        Drawing::Filter filterForBrush;
        auto imageFilter = filter->GetImageFilter();
        // Since using Kawase blur (shader) in the screenshot scene would lead to failure,
        // a Gaussian blur filter (imageFilter) is regenerated here instead.
        auto kawaseShaderFilter = filter->GetShaderFilterWithType(RSUIFilterType::KAWASE);
        if (kawaseShaderFilter != nullptr) {
            auto tmpFilter = std::static_pointer_cast<RSKawaseBlurShaderFilter>(kawaseShaderFilter);
            auto radius = tmpFilter->GetRadius();
            std::shared_ptr<Drawing::ImageFilter> blurFilter = Drawing::ImageFilter::CreateBlurImageFilter(
                radius, radius, Drawing::TileMode::CLAMP, nullptr);
            imageFilter = Drawing::ImageFilter::CreateComposeImageFilter(imageFilter, blurFilter);
        }
        filterForBrush.SetImageFilter(imageFilter);
        brush.SetFilter(filterForBrush);
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

    auto clipIBounds = canvas.GetDeviceClipBounds();
    auto imageClipIBounds = clipIBounds;
    auto magnifierShaderFilter = filter->GetShaderFilterWithType(RSUIFilterType::MAGNIFIER);
    if (magnifierShaderFilter != nullptr) {
        auto tmpFilter = std::static_pointer_cast<RSMagnifierShaderFilter>(magnifierShaderFilter);
        auto canvasMatrix = canvas.GetTotalMatrix();
        tmpFilter->SetMagnifierOffset(canvasMatrix);
        imageClipIBounds.Offset(tmpFilter->GetMagnifierOffsetX(), tmpFilter->GetMagnifierOffsetY());
    }

#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    // Optional use cacheManager to draw filter
    if (auto& cacheManager = properties.GetFilterCacheManager(filterType == FilterType::FOREGROUND_FILTER);
        cacheManager != nullptr && !canvas.GetDisableFilterCache()) {
        auto rsShaderFilter = filter->GetShaderFilterWithType(RSUIFilterType::LINEAR_GRADIENT_BLUR);
        if (rsShaderFilter != nullptr) {
            auto tmpFilter = std::static_pointer_cast<RSLinearGradientBlurShaderFilter>(rsShaderFilter);
            tmpFilter->IsOffscreenCanvas(true);
            tmpFilter->SetGeometry(canvas, properties.GetFrameWidth(), properties.GetFrameHeight());
        }
        // RSFilterCacheManger has no more logic for evaluating filtered snapshot clearing
        // (see RSPropertyDrawableUtils::DrawFilter())
        cacheManager->DrawFilter(canvas, filter, 0, true, false);
        return;
    }
#endif

    auto rsShaderFilter = filter->GetShaderFilterWithType(RSUIFilterType::LINEAR_GRADIENT_BLUR);
    if (rsShaderFilter != nullptr) {
        auto tmpFilter = std::static_pointer_cast<RSLinearGradientBlurShaderFilter>(rsShaderFilter);
        tmpFilter->IsOffscreenCanvas(true);
        tmpFilter->SetGeometry(canvas, properties.GetFrameWidth(), properties.GetFrameHeight());
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

    // Optional use cacheManager to draw filter, cache is valid, skip drawing background image
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    auto boundsRect = properties.GetBoundsRect();
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
    offscreenCanvas->CopyConfigurationToOffscreenCanvas(canvas);
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
        ROSEN_LOGE("RSPropertiesPainter::DrawBackgroundEffect RSFilter null");
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
    auto bounds = canvas.GetRoundInDeviceClipBounds();
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
    auto geoPtr = properties.GetBoundsGeometry();
    Drawing::Matrix matrix = (geoPtr && isAbsCoordinate) ? geoPtr->GetAbsMatrix() : Drawing::Matrix();
    auto drawingRect = Rect2DrawingRect(scaledBounds);
    matrix.MapRect(drawingRect, drawingRect);
    dirtyPixelStretch.left_ = std::floor(drawingRect.GetLeft());
    dirtyPixelStretch.top_ = std::floor(drawingRect.GetTop());
    dirtyPixelStretch.width_ = std::ceil(drawingRect.GetWidth()) + PARAM_DOUBLE;
    dirtyPixelStretch.height_ = std::ceil(drawingRect.GetHeight()) + PARAM_DOUBLE;
}

void RSPropertiesPainter::GetForegroundEffectDirtyRect(RectI& dirtyForegroundEffect,
    const RSProperties& properties, const bool isAbsCoordinate)
{
    std::shared_ptr<RSFilter> foregroundFilter = nullptr;
    if (RSProperties::IS_UNI_RENDER) {
        foregroundFilter = properties.GetForegroundFilterCache();
    } else {
        foregroundFilter = properties.GetForegroundFilter();
    }
    if (!foregroundFilter) {
        return;
    }

    if (foregroundFilter->GetFilterType() == RSFilter::FOREGROUND_EFFECT) {
        float dirtyExtension =
            std::static_pointer_cast<RSForegroundEffectFilter>(foregroundFilter)->GetDirtyExtension();
        auto boundsRect = properties.GetBoundsRect();
        auto scaledBounds = boundsRect.MakeOutset(dirtyExtension);
        auto& geoPtr = properties.GetBoundsGeometry();
        Drawing::Matrix matrix = (geoPtr && isAbsCoordinate) ? geoPtr->GetAbsMatrix() : Drawing::Matrix();
        auto drawingRect = Rect2DrawingRect(scaledBounds);
        matrix.MapRect(drawingRect, drawingRect);
        dirtyForegroundEffect.left_ = std::floor(drawingRect.GetLeft());
        dirtyForegroundEffect.top_ = std::floor(drawingRect.GetTop());
        dirtyForegroundEffect.width_ = std::ceil(drawingRect.GetWidth()) + PARAM_DOUBLE;
        dirtyForegroundEffect.height_ = std::ceil(drawingRect.GetHeight()) + PARAM_DOUBLE;
    } else if (foregroundFilter->GetFilterType() == RSFilter::COLORFUL_SHADOW) {
        if (properties.IsShadowValid()) {
            GetShadowDirtyRect(dirtyForegroundEffect, properties, nullptr, false, true);
        }
    }
}

// calculate the distortion effect's dirty area
void RSPropertiesPainter::GetDistortionEffectDirtyRect(RectI& dirtyDistortionEffect, const RSProperties& properties)
{
    // if the distortionK > 0, set the dirty bounds to its maximum range value
    auto distortionK = properties.GetDistortionK();
    if (distortionK.has_value() && *distortionK > 0) {
        int dirtyWidth = static_cast<int>(std::numeric_limits<int16_t>::max());
        int dirtyBeginPoint = static_cast<int>(std::numeric_limits<int16_t>::min()) / PARAM_DOUBLE;
        dirtyDistortionEffect.left_ = dirtyBeginPoint;
        dirtyDistortionEffect.top_ = dirtyBeginPoint;
        dirtyDistortionEffect.width_ = dirtyWidth;
        dirtyDistortionEffect.height_ = dirtyWidth;
    }
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
        ROSEN_LOGE("RSPropertiesPainter::DrawFrame cmds null");
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
            RSBorderGeo borderGeo;
            borderGeo.rrect = RRect2DrawingRRect(GetRRectForDrawingBorder(properties, border, isOutline));
            borderGeo.innerRRect = RRect2DrawingRRect(GetInnerRRectForDrawingBorder(properties, border, isOutline));
            auto centerX = borderGeo.innerRRect.GetRect().GetLeft() + borderGeo.innerRRect.GetRect().GetWidth() / 2;
            auto centerY = borderGeo.innerRRect.GetRect().GetTop() + borderGeo.innerRRect.GetRect().GetHeight() / 2;
            borderGeo.center = { centerX, centerY };
            auto rect = borderGeo.rrect.GetRect();
            Drawing::AutoCanvasRestore acr(canvas, false);
            Drawing::SaveLayerOps slr(&rect, nullptr);
            canvas.SaveLayer(slr);
            border->DrawBorders(canvas, pen, borderGeo);
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
    uint32_t tmpLayer = canvas.GetSaveCount();

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
