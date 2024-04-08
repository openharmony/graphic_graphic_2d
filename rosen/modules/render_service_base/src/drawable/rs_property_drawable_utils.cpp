/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "drawable/rs_property_drawable_utils.h"

#include "common/rs_optional_trace.h"
#include "platform/common/rs_log.h"
#include "render/rs_material_filter.h"
#include "property/rs_properties_painter.h"

namespace OHOS {
namespace Rosen {
namespace {
// when the blur radius > SNAPSHOT_OUTSET_BLUR_RADIUS_THRESHOLD,
// the snapshot should call outset before blur to shrink by 1px.
constexpr static float SNAPSHOT_OUTSET_BLUR_RADIUS_THRESHOLD = 40.0f;
} // namespace

std::shared_ptr<Drawing::RuntimeEffect> RSPropertyDrawableUtils::binarizationShaderEffect_ = nullptr;
std::shared_ptr<Drawing::RuntimeEffect> RSPropertyDrawableUtils::dynamicDimShaderEffect_ = nullptr;

Drawing::RoundRect RSPropertyDrawableUtils::RRect2DrawingRRect(const RRect& rr)
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
    return { rect, radii };
}

Drawing::Rect RSPropertyDrawableUtils::Rect2DrawingRect(const RectF& r)
{
    return { r.left_, r.top_, r.left_ + r.width_, r.top_ + r.height_ };
}

RRect RSPropertyDrawableUtils::GetRRectForDrawingBorder(
    const RSProperties& properties, const std::shared_ptr<RSBorder>& border, const bool& isOutline)
{
    if (!border) {
        return {};
    }

    return isOutline ? RRect(properties.GetRRect().rect_.MakeOutset(border->GetWidthFour()), border->GetRadiusFour())
                     : properties.GetRRect();
}

RRect RSPropertyDrawableUtils::GetInnerRRectForDrawingBorder(
    const RSProperties& properties, const std::shared_ptr<RSBorder>& border, const bool& isOutline)
{
    if (!border) {
        return {};
    }
    return isOutline ? properties.GetRRect() : properties.GetInnerRRect();
}

bool RSPropertyDrawableUtils::PickColor(Drawing::Canvas& canvas,
    const std::shared_ptr<RSColorPickerCacheTask>& colorPickerTask, Drawing::Path& drPath, Drawing::Matrix& matrix,
    RSColor& colorPicked)
{
    Drawing::Rect clipBounds = drPath.GetBounds();
    Drawing::RectI clipIBounds = { static_cast<int>(clipBounds.GetLeft()), static_cast<int>(clipBounds.GetTop()),
        static_cast<int>(clipBounds.GetRight()), static_cast<int>(clipBounds.GetBottom()) };
    Drawing::Surface* drSurface = canvas.GetSurface();
    if (drSurface == nullptr) {
        return false;
    }

    if (!colorPickerTask) {
        ROSEN_LOGE("RSPropertyDrawableUtils::PickColor colorPickerTask is null");
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

    if (RSColorPickerCacheTask::PostPartialColorPickerTask(colorPickerTask, shadowRegionImage) &&
        colorPickerTask->GetColor(colorPicked)) {
        colorPickerTask->GetColorAverage(colorPicked);
        colorPickerTask->SetStatus(CacheProcessStatus::WAITING);
        return true;
    }
    colorPickerTask->GetColorAverage(colorPicked);
    return true;
}

void RSPropertyDrawableUtils::GetDarkColor(RSColor& color)
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

void RSPropertyDrawableUtils::CeilMatrixTrans(Drawing::Canvas* canvas)
{
    // The translation of the matrix is rounded to improve the hit ratio of skia blurfilter cache,
    // the function <compute_key_and_clip_bounds> in <skia/src/gpu/GrBlurUtil.cpp> for more details.
    Drawing::AutoCanvasRestore rst(*canvas, true);
    auto matrix = canvas->GetTotalMatrix();
    matrix.Set(Drawing::Matrix::TRANS_X, std::ceil(matrix.Get(Drawing::Matrix::TRANS_X)));
    matrix.Set(Drawing::Matrix::TRANS_Y, std::ceil(matrix.Get(Drawing::Matrix::TRANS_Y)));
    canvas->SetMatrix(matrix);
}

void RSPropertyDrawableUtils::DrawFilter(Drawing::Canvas* canvas,
    const std::shared_ptr<RSFilter>& rsFilter, const std::unique_ptr<RSFilterCacheManager>& cacheManager,
    const bool isForegroundFilter, bool shouldClearFilteredCache)
{
    if (!RSSystemProperties::GetBlurEnabled()) {
        ROSEN_LOGD("RSPropertyDrawableUtils::DrawFilter close blur.");
        return;
    }
    if (rsFilter == nullptr) {
        ROSEN_LOGE("RSPropertyDrawableUtils::DrawFilter null filter.");
        return;
    }

    bool needSnapshotOutset = true;
    if (rsFilter->GetFilterType() == RSFilter::MATERIAL) {
        auto material = std::static_pointer_cast<RSMaterialFilter>(rsFilter);
        needSnapshotOutset = (material->GetRadius() >= SNAPSHOT_OUTSET_BLUR_RADIUS_THRESHOLD);
    }
    RS_OPTIONAL_TRACE_NAME("DrawFilter " + rsFilter->GetDescription());

    auto filter = std::static_pointer_cast<RSDrawingFilter>(rsFilter);
    auto surface = canvas->GetSurface();
    if (surface == nullptr) {
        ROSEN_LOGE("RSPropertyDrawableUtils::DrawFilter surface null");
        Drawing::Brush brush = filter->GetBrush();
        Drawing::SaveLayerOps slr(nullptr, &brush, Drawing::SaveLayerOps::Flags::INIT_WITH_PREVIOUS);
        canvas->SaveLayer(slr);
        filter->PostProcess(*canvas);
        return;
    }

#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    // Optional use cacheManager to draw filter
    if (auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(canvas);
        !paintFilterCanvas->GetDisableFilterCache() && cacheManager != nullptr && RSProperties::FilterCacheEnabled) {
        cacheManager->DrawFilter(*paintFilterCanvas, filter, needSnapshotOutset);
        cacheManager->CompactFilterCache(shouldClearFilteredCache); // flag for clear witch cache after drawing
        return;
    }
#endif

    auto clipIBounds = canvas->GetDeviceClipBounds();
    auto imageClipIBounds = clipIBounds;
    if (needSnapshotOutset) {
        imageClipIBounds.MakeOutset(-1, -1);
    }
    auto imageSnapshot = surface->GetImageSnapshot(imageClipIBounds);
    if (imageSnapshot == nullptr) {
        ROSEN_LOGE("RSPropertyDrawableUtils::DrawFilter image null");
        return;
    }
    if (RSSystemProperties::GetImageGpuResourceCacheEnable(imageSnapshot->GetWidth(), imageSnapshot->GetHeight())) {
        ROSEN_LOGE("RSPropertyDrawableUtils::DrawFilter cache image resource(w:%{public}d, h:%{public}d).",
            imageSnapshot->GetWidth(), imageSnapshot->GetHeight());
        imageSnapshot->HintCacheGpuResource();
    }

    filter->PreProcess(imageSnapshot);
    Drawing::AutoCanvasRestore acr(*canvas, true);
    canvas->ResetMatrix();
    Drawing::Rect srcRect = Drawing::Rect(0, 0, imageSnapshot->GetWidth(), imageSnapshot->GetHeight());
    Drawing::Rect dstRect = clipIBounds;
    filter->DrawImageRect(*canvas, imageSnapshot, srcRect, dstRect);
    filter->PostProcess(*canvas);
}

void RSPropertyDrawableUtils::DrawBackgroundEffect(
    RSPaintFilterCanvas* canvas, const std::shared_ptr<RSFilter>& rsFilter,
    const std::unique_ptr<RSFilterCacheManager>& cacheManager, bool shouldClearFilteredCache)
{
    if (rsFilter == nullptr) {
        ROSEN_LOGE("RSPropertyDrawableUtils::DrawBackgroundEffect null filter");
        return;
    }
    auto surface = canvas->GetSurface();
    if (surface == nullptr) {
        ROSEN_LOGE("RSPropertyDrawableUtils::DrawBackgroundEffect surface null");
        return;
    }
    auto clipIBounds = canvas->GetDeviceClipBounds();
    auto filter = std::static_pointer_cast<RSDrawingFilter>(rsFilter);
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    // Optional use cacheManager to draw filter
    if (RSProperties::FilterCacheEnabled && cacheManager != nullptr && !canvas->GetDisableFilterCache()) {
        auto&& data = cacheManager->GeneratedCachedEffectData(*canvas, filter, clipIBounds, clipIBounds);
        cacheManager->CompactFilterCache(shouldClearFilteredCache); // flag for clear witch cache after drawing
        canvas->SetEffectData(data);
        return;
    }
#endif
    RS_OPTIONAL_TRACE_NAME("RSPropertyDrawableUtils::DrawBackgroundEffect " + rsFilter->GetDescription());
    auto imageRect = clipIBounds;
    auto imageSnapshot = surface->GetImageSnapshot(imageRect);
    if (imageSnapshot == nullptr) {
        ROSEN_LOGE("RSPropertyDrawableUtils::DrawBackgroundEffect image snapshot null");
        return;
    }
    filter->PreProcess(imageSnapshot);
    // create a offscreen skSurface
    std::shared_ptr<Drawing::Surface> offscreenSurface =
        surface->MakeSurface(imageSnapshot->GetWidth(), imageSnapshot->GetHeight());
    if (offscreenSurface == nullptr) {
        ROSEN_LOGE("RSPropertyDrawableUtils::DrawBackgroundEffect offscreenSurface null");
        return;
    }
    RSPaintFilterCanvas offscreenCanvas(offscreenSurface.get());
    auto clipBounds = Drawing::Rect(0, 0, imageRect.GetWidth(), imageRect.GetHeight());
    auto imageSnapshotBounds = Drawing::Rect(0, 0, imageSnapshot->GetWidth(), imageSnapshot->GetHeight());
    filter->DrawImageRect(offscreenCanvas, imageSnapshot, imageSnapshotBounds, clipBounds);
    filter->PostProcess(offscreenCanvas);

    auto imageCache = offscreenSurface->GetImageSnapshot();
    if (imageCache == nullptr) {
        ROSEN_LOGE("RSPropertyDrawableUtils::DrawBackgroundEffect imageCache snapshot null");
        return;
    }
    auto data = std::make_shared<RSPaintFilterCanvas::CachedEffectData>(std::move(imageCache), std::move(imageRect));
    canvas->SetEffectData(std::move(data));
}

void RSPropertyDrawableUtils::DrawColorFilter(
    Drawing::Canvas* canvas, const std::shared_ptr<Drawing::ColorFilter>& colorFilter)
{
    if (colorFilter == nullptr) {
        ROSEN_LOGE("RSPropertyDrawableUtils::DrawColorFilter null colorFilter.");
        return;
    }

    Drawing::Brush brush;
    brush.SetAntiAlias(true);
    Drawing::Filter filter;
    filter.SetColorFilter(colorFilter);
    brush.SetFilter(filter);
    auto surface = canvas->GetSurface();
    if (surface == nullptr) {
        ROSEN_LOGE("RSPropertyDrawableUtils::DrawColorFilter surface is null");
        return;
    }
    auto clipBounds = canvas->GetDeviceClipBounds();
    auto imageSnapshot = surface->GetImageSnapshot(clipBounds);
    if (imageSnapshot == nullptr) {
        ROSEN_LOGE("RSPropertyDrawableUtils::DrawColorFilter image is null");
        return;
    }
    imageSnapshot->HintCacheGpuResource();
    Drawing::AutoCanvasRestore acr(*canvas, true);
    canvas->ResetMatrix();
    Drawing::SamplingOptions options(Drawing::FilterMode::NEAREST, Drawing::MipmapMode::NONE);
    canvas->AttachBrush(brush);
    canvas->DrawImageRect(*imageSnapshot, clipBounds, options);
    canvas->DetachBrush();
}

void RSPropertyDrawableUtils::DrawLightUpEffect(Drawing::Canvas* canvas, const float lightUpEffectDegree)
{
    if (!ROSEN_GE(lightUpEffectDegree, 0.0) || !ROSEN_LNE(lightUpEffectDegree, 1.0)) {
        ROSEN_LOGE(
            "RSPropertyDrawableUtils::DrawLightUpEffect Invalid lightUpEffectDegree %{public}f", lightUpEffectDegree);
        return;
    }

    Drawing::Surface* surface = canvas->GetSurface();
    if (surface == nullptr) {
        ROSEN_LOGE("RSPropertyDrawableUtils::DrawLightUpEffect surface is null");
        return;
    }

    auto clipBounds = canvas->GetDeviceClipBounds();
    auto image = surface->GetImageSnapshot(clipBounds);
    if (image == nullptr) {
        ROSEN_LOGE("RSPropertyDrawableUtils::DrawLightUpEffect image is null");
        return;
    }

    Drawing::Matrix scaleMat;
    auto imageShader = Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), scaleMat);
    auto shader = Drawing::ShaderEffect::CreateLightUp(lightUpEffectDegree, *imageShader);
    Drawing::Brush brush;
    brush.SetShaderEffect(shader);
    Drawing::AutoCanvasRestore acr(*canvas, true);
    canvas->ResetMatrix();
    canvas->Translate(clipBounds.GetLeft(), clipBounds.GetTop());
    canvas->DrawBackground(brush);
}

void RSPropertyDrawableUtils::DrawDynamicDim(Drawing::Canvas* canvas, const float dynamicDimDegree)
{
    if (!ROSEN_GE(dynamicDimDegree, 0.0) || !ROSEN_LNE(dynamicDimDegree, 1.0)) {
        ROSEN_LOGE(
            "RSPropertyDrawableUtils::DrawDynamicDim Invalid dynamicDimDegree %{public}f", dynamicDimDegree);
        return;
    }

    Drawing::Surface* surface = canvas->GetSurface();
    if (surface == nullptr) {
        ROSEN_LOGE("RSPropertyDrawableUtils::DrawDynamicDim surface is null");
        return;
    }

    auto clipBounds = canvas->GetDeviceClipBounds();
    auto image = surface->GetImageSnapshot(clipBounds);
    if (image == nullptr) {
        ROSEN_LOGE("RSPropertyDrawableUtils::DrawDynamicDim image is null");
        return;
    }

    Drawing::Matrix scaleMat;
    auto imageShader = Drawing::ShaderEffect::CreateImageShader(*image, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), scaleMat);
    auto shader = MakeDynamicDimShader(dynamicDimDegree, imageShader);
    if (shader == nullptr) {
        ROSEN_LOGE("RSPropertyDrawableUtils::DrawDynamicDim shader is null");
        return;
    }
    Drawing::Brush brush;
    brush.SetShaderEffect(shader);
    Drawing::AutoCanvasRestore acr(*canvas, true);
    canvas->ResetMatrix();
    canvas->Translate(clipBounds.GetLeft(), clipBounds.GetTop());
    canvas->DrawBackground(brush);
}

std::shared_ptr<Drawing::ShaderEffect> RSPropertyDrawableUtils::MakeDynamicDimShader(
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
            ROSEN_LOGE("MakeDynamicDimShader::RuntimeShader effect error\n");
            return nullptr;
        }
    }
    std::shared_ptr<Drawing::ShaderEffect> children[] = {imageShader};
    size_t childCount = 1;
    auto data = std::make_shared<Drawing::Data>();
    data->BuildWithCopy(&dynamicDimDeg, sizeof(dynamicDimDeg));
    return dynamicDimShaderEffect_->MakeShader(data, children, childCount, nullptr, false);
}

std::shared_ptr<Drawing::ShaderEffect> RSPropertyDrawableUtils::MakeBinarizationShader(
    float low, float high, float thresholdLow, float thresholdHigh, std::shared_ptr<Drawing::ShaderEffect> imageShader)
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
            ROSEN_LOGE("RSPropertyDrawableUtils::MakeBinarizationShader effect error\n");
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

void RSPropertyDrawableUtils::DrawBinarization(Drawing::Canvas* canvas, const std::optional<Vector4f>& aiInvert)
{
    if (!aiInvert.has_value()) {
        ROSEN_LOGE("RSPropertyDrawableUtils::DrawBinarization aiInvert has no value");
        return;
    }
    auto drSurface = canvas->GetSurface();
    if (drSurface == nullptr) {
        ROSEN_LOGE("RSPropertyDrawableUtils::DrawBinarization drSurface is null");
        return;
    }
    auto clipBounds = canvas->GetDeviceClipBounds();
    auto imageSnapshot = drSurface->GetImageSnapshot(clipBounds);
    if (imageSnapshot == nullptr) {
        ROSEN_LOGE("RSPropertyDrawableUtils::DrawBinarization image is null");
        return;
    }
    Drawing::Matrix matrix;
    auto imageShader = Drawing::ShaderEffect::CreateImageShader(*imageSnapshot, Drawing::TileMode::CLAMP,
        Drawing::TileMode::CLAMP, Drawing::SamplingOptions(Drawing::FilterMode::LINEAR), matrix);
    float thresholdLow = aiInvert->z_ - aiInvert->w_;
    float thresholdHigh = aiInvert->z_ + aiInvert->w_;
    auto shader = MakeBinarizationShader(aiInvert->x_, aiInvert->y_, thresholdLow, thresholdHigh, imageShader);
    Drawing::Brush brush;
    brush.SetShaderEffect(shader);
    brush.SetAntiAlias(true);
    Drawing::AutoCanvasRestore acr(*canvas, true);
    canvas->ResetMatrix();
    canvas->Translate(clipBounds.GetLeft(), clipBounds.GetTop());
    canvas->DrawBackground(brush);
}

void RSPropertyDrawableUtils::DrawPixelStretch(Drawing::Canvas* canvas, const std::optional<Vector4f>& pixelStretch,
    const RectF& boundsRect, const bool boundsGeoValid)
{
    if (!pixelStretch.has_value()) {
        ROSEN_LOGE("RSPropertyDrawableUtils::DrawPixelStretch pixelStretch has no value");
        return;
    }
    auto surface = canvas->GetSurface();
    if (surface == nullptr) {
        ROSEN_LOGE("RSPropertyDrawableUtils::DrawPixelStretch surface null");
        return;
    }

    /*  Calculates the relative coordinates of the clipbounds
        with respect to the origin of the current canvas coordinates */
    Drawing::Matrix worldToLocalMat;
    if (!canvas->GetTotalMatrix().Invert(worldToLocalMat)) {
        ROSEN_LOGE("RSPropertyDrawableUtils::DrawPixelStretch get invert matrix failed.");
    }
    Drawing::Rect localClipBounds;
    auto tmpBounds = canvas->GetDeviceClipBounds();
    Drawing::Rect clipBounds(
        tmpBounds.GetLeft(), tmpBounds.GetTop(), tmpBounds.GetRight() - 1, tmpBounds.GetBottom() - 1);
    Drawing::Rect fClipBounds(clipBounds.GetLeft(), clipBounds.GetTop(), clipBounds.GetRight(), clipBounds.GetBottom());
    if (!worldToLocalMat.MapRect(localClipBounds, fClipBounds)) {
        ROSEN_LOGE("RSPropertyDrawableUtils::DrawPixelStretch map rect failed.");
    }
    auto bounds = Rect2DrawingRect(boundsRect);
    if (!bounds.Intersect(localClipBounds)) {
        ROSEN_LOGE("RSPropertyDrawableUtils::DrawPixelStretch intersect clipbounds failed");
    }

    auto scaledBounds = Drawing::Rect(bounds.GetLeft() - pixelStretch->x_, bounds.GetTop() - pixelStretch->y_,
        bounds.GetRight() + pixelStretch->z_, bounds.GetBottom() + pixelStretch->w_);
    if (!scaledBounds.IsValid() || !bounds.IsValid() || !clipBounds.IsValid()) {
        ROSEN_LOGE("RSPropertyDrawableUtils::DrawPixelStretch invalid scaled bounds");
        return;
    }

    Drawing::RectI rectI(static_cast<int>(fClipBounds.GetLeft()), static_cast<int>(fClipBounds.GetTop()),
        static_cast<int>(fClipBounds.GetRight()), static_cast<int>(fClipBounds.GetBottom()));
    auto image = surface->GetImageSnapshot(rectI);
    if (image == nullptr) {
        ROSEN_LOGE("RSPropertyDrawableUtils::DrawPixelStretch image null");
        return;
    }

    Drawing::Brush brush;
    Drawing::Matrix inverseMat;
    Drawing::Matrix rotateMat;
    if (boundsGeoValid) {
        auto transMat = canvas->GetTotalMatrix();
        /* transMat.getSkewY() is the sin of the rotation angle(sin0 = 0,sin90 =1 sin180 = 0,sin270 = -1),
            if transMat.getSkewY() is not 0 or -1 or 1,the rotation angle is not a multiple of 90,not Stretch*/
        auto skewY = transMat.Get(Drawing::Matrix::SKEW_Y);
        if (ROSEN_EQ(skewY, 0.f) || ROSEN_EQ(skewY, 1.f) || ROSEN_EQ(skewY, -1.f)) {
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
            ROSEN_LOGE("RSPropertyDrawableUtils::DrawPixelStretch get invert matrix failed.");
        }
    }

    Drawing::AutoCanvasRestore acr(*canvas, true);
    canvas->Translate(bounds.GetLeft(), bounds.GetTop());
    Drawing::SamplingOptions samplingOptions;
    constexpr static float EPS = 1e-5f;
    if (pixelStretch->x_ > EPS || pixelStretch->y_ > EPS || pixelStretch->z_ > EPS || pixelStretch->w_ > EPS) {
        brush.SetShaderEffect(Drawing::ShaderEffect::CreateImageShader(
            *image, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, samplingOptions, inverseMat));
        canvas->AttachBrush(brush);
        canvas->DrawRect(Drawing::Rect(-pixelStretch->x_, -pixelStretch->y_,
            -pixelStretch->x_ + scaledBounds.GetWidth(), -pixelStretch->y_ + scaledBounds.GetHeight()));
        canvas->DetachBrush();
    } else {
        inverseMat.PostScale(
            scaledBounds.GetWidth() / bounds.GetWidth(), scaledBounds.GetHeight() / bounds.GetHeight());
        brush.SetShaderEffect(Drawing::ShaderEffect::CreateImageShader(
            *image, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, samplingOptions, inverseMat));

        canvas->Translate(-pixelStretch->x_, -pixelStretch->y_);
        canvas->AttachBrush(brush);
        canvas->DrawRect(Drawing::Rect(pixelStretch->x_, pixelStretch->y_, pixelStretch->x_ + bounds.GetWidth(),
            pixelStretch->y_ + bounds.GetHeight()));
        canvas->DetachBrush();
    }
}

Drawing::Path RSPropertyDrawableUtils::CreateShadowPath(const std::shared_ptr<RSPath> rsPath,
    const std::shared_ptr<RSPath>& clipBounds, const RRect& rrect)
{
    Drawing::Path path;
    if (rsPath && rsPath->GetDrawingPath().IsValid()) {
        path = rsPath->GetDrawingPath();
    } else if (clipBounds) {
        path = clipBounds->GetDrawingPath();
    } else {
        path.AddRoundRect(RRect2DrawingRRect(rrect));
    }
    return path;
}

void RSPropertyDrawableUtils::DrawShadow(Drawing::Canvas* canvas, Drawing::Path& path, const float& offsetX,
    const float& offsetY, const float& elevation, const bool& isFilled, Color spotColor)
{
    CeilMatrixTrans(canvas);
    Drawing::AutoCanvasRestore acr(*canvas, true);
    if (!isFilled) {
        canvas->ClipPath(path, Drawing::ClipOp::DIFFERENCE, true);
    }
    path.Offset(offsetX, offsetY);
    Drawing::Point3 planeParams = {0.0f, 0.0f, elevation};
    std::vector<Drawing::Point> pt{{path.GetBounds().GetLeft() + path.GetBounds().GetWidth() / 2,
        path.GetBounds().GetTop() + path.GetBounds().GetHeight() / 2}};
    canvas->GetTotalMatrix().MapPoints(pt, pt, 1);
    Drawing::Point3 lightPos = {pt[0].GetX(), pt[0].GetY(), DEFAULT_LIGHT_HEIGHT};
    Color ambientColor = Color::FromArgbInt(DEFAULT_AMBIENT_COLOR);
    ambientColor.MultiplyAlpha(canvas->GetAlpha());
    spotColor.MultiplyAlpha(canvas->GetAlpha());
    canvas->DrawShadow(path, planeParams, lightPos, DEFAULT_LIGHT_RADIUS, Drawing::Color(ambientColor.AsArgbInt()),
        Drawing::Color(spotColor.AsArgbInt()), Drawing::ShadowFlags::TRANSPARENT_OCCLUDER);
}

void RSPropertyDrawableUtils::DrawUseEffect(RSPaintFilterCanvas* canvas)
{
    const auto& effectData = canvas->GetEffectData();
    if (effectData == nullptr || effectData->cachedImage_ == nullptr || !RSSystemProperties::GetEffectMergeEnabled()) {
        return;
    }
    RS_TRACE_FUNC();
    Drawing::AutoCanvasRestore acr(*canvas, true);
    canvas->ResetMatrix();
    auto visibleRect = canvas->GetVisibleRect();
    visibleRect.Round();
    auto visibleIRect = Drawing::RectI(
        static_cast<int>(visibleRect.GetLeft()), static_cast<int>(visibleRect.GetTop()),
        static_cast<int>(visibleRect.GetRight()), static_cast<int>(visibleRect.GetBottom()));
    if (!visibleIRect.IsEmpty()) {
        canvas->ClipIRect(visibleIRect, Drawing::ClipOp::INTERSECT);
    }
    Drawing::Brush brush;
    canvas->AttachBrush(brush);
    // dstRect: canvas clip region
    Drawing::Rect dstRect = canvas->GetDeviceClipBounds();
    // srcRect: map dstRect onto cache coordinate
    Drawing::Rect srcRect = dstRect;
    srcRect.Offset(-effectData->cachedRect_.GetLeft(), -effectData->cachedRect_.GetTop());
    canvas->DrawImageRect(*effectData->cachedImage_, srcRect, dstRect,
        Drawing::SamplingOptions(), Drawing::SrcRectConstraint::FAST_SRC_RECT_CONSTRAINT);
    canvas->DetachBrush();
}

void RSPropertyDrawableUtils::BeginBlendMode(RSPaintFilterCanvas& canvas, int blendMode, int blendModeApplyType)
{
    // fast blend mode
    if (blendModeApplyType == static_cast<int>(RSColorBlendApplyType::FAST)) {
        canvas.SetBlendMode({ blendMode - 1 }); // map blendMode to SkBlendMode
        return;
    }

    // save layer mode
    auto matrix = canvas.GetTotalMatrix();
    matrix.Set(Drawing::Matrix::TRANS_X, std::ceil(matrix.Get(Drawing::Matrix::TRANS_X)));
    matrix.Set(Drawing::Matrix::TRANS_Y, std::ceil(matrix.Get(Drawing::Matrix::TRANS_Y)));
    canvas.SetMatrix(matrix);
    Drawing::Brush blendBrush_;
    blendBrush_.SetAlphaF(canvas.GetAlpha());
    blendBrush_.SetBlendMode(static_cast<Drawing::BlendMode>(blendMode - 1)); // map blendMode to Drawing::BlendMode
    Drawing::SaveLayerOps maskLayerRec(nullptr, &blendBrush_, 0);
    canvas.SaveLayer(maskLayerRec);
    canvas.SetBlendMode(std::nullopt);
    canvas.SaveAlpha();
    canvas.SetAlpha(1.0f);
}

void RSPropertyDrawableUtils::EndBlendMode(RSPaintFilterCanvas& canvas)
{
    canvas.RestoreEnv();
    canvas.RestoreAlpha();
    canvas.Restore();
}

Color RSPropertyDrawableUtils::CalculateInvertColor(const Color& backgroundColor)
{
    uint32_t a = backgroundColor.GetAlpha();
    uint32_t r = 255 - backgroundColor.GetRed(); // 255 max red channel value
    uint32_t g = 255 - backgroundColor.GetGreen(); // 255 max green channel value
    uint32_t b = 255 - backgroundColor.GetBlue(); // 255 max blue channel value
    return Color(r, g, b, a);
}

Color RSPropertyDrawableUtils::GetInvertBackgroundColor(RSPaintFilterCanvas& canvas, bool needClipToBounds,
    const Vector4f& boundsRect, const Color& backgroundColor)
{
    Drawing::AutoCanvasRestore acr(canvas, true);
    if (!needClipToBounds) {
        RS_LOGI("RSPropertyDrawableUtils::GetInvertBackgroundColor not GetClipToBounds");
        Drawing::Rect rect = Drawing::Rect(0, 0, boundsRect.z_, boundsRect.w_);
        canvas.ClipRect(rect, Drawing::ClipOp::INTERSECT, false);
    }
    if (backgroundColor.GetAlpha() == 0xff) { // 0xff = 255, max alpha value
        RS_LOGI("RSPropertyDrawableUtils::GetInvertBackgroundColor not alpha");
        return RSPropertyDrawableUtils::CalculateInvertColor(backgroundColor);
    }
    auto imageSnapshot = canvas.GetSurface()->GetImageSnapshot(canvas.GetDeviceClipBounds());
    if (imageSnapshot == nullptr) {
        RS_LOGI("RSPropertyDrawableUtils::GetInvertBackgroundColor imageSnapshot null");
        return Color(0);
    }
    auto colorPicker = RSPropertiesPainter::CalcAverageColor(imageSnapshot);
    return RSPropertyDrawableUtils::CalculateInvertColor(Color(
        Drawing::Color::ColorQuadGetR(colorPicker), Drawing::Color::ColorQuadGetG(colorPicker),
        Drawing::Color::ColorQuadGetB(colorPicker), Drawing::Color::ColorQuadGetA(colorPicker)));
}
} // namespace Rosen
} // namespace OHOS