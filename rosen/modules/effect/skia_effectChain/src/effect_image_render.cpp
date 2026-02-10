/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "effect_image_render.h"
#include "effect_utils.h"
#include "rs_trace.h"

namespace OHOS::Rosen {
static constexpr float GRAYSCALE_PARAONE = 0.2126f;
static constexpr float GRAYSCALE_PARATWO = 0.7152f;
static constexpr float GRAYSCALE_PARATHREE = 0.0722f;
static constexpr float RADIUS_THRESHOLD = 0.0f;
static constexpr float BRIGHTNESS_MIN_THRESHOLD = 0.0f;
static constexpr float BRIGHTNESS_MAX_THRESHOLD = 1.0f;
static constexpr float POSITIVE_MIN_THRESHOLD = 0.0f;
static constexpr float GAMMA_MIN_THRESHOLD = 1e-6;
// limits for mapColorByBrightness filter pamameters
static constexpr std::pair<float, float> COLOR_POSITION_LIMITS{0.0f, 1.0f};

static Vector4f GetLimitedPara(const Vector4f& para, float minThreshold)
{
    return Vector4f(std::max(para.x_, minThreshold), std::max(para.y_, minThreshold),
        std::max(para.z_, minThreshold), std::max(para.w_, minThreshold));
}

std::shared_ptr<EffectImageFilter> EffectImageFilter::Blur(float radius, Drawing::TileMode tileMode)
{
    if (radius < RADIUS_THRESHOLD) {
        return nullptr;
    }

    return std::make_shared<EffectImageBlurFilter>(radius, tileMode);
}

std::shared_ptr<EffectImageFilter> EffectImageFilter::Blur(float radius, float angle, Drawing::TileMode tileMode)
{
    if (radius < RADIUS_THRESHOLD) {
        return nullptr;
    }

    return std::make_shared<EffectImageBlurFilter>(radius, angle, tileMode);
}

std::shared_ptr<EffectImageFilter> EffectImageFilter::MapColorByBrightness(const std::vector<Vector4f>& colors,
    const std::vector<float>& positions)
{
    bool isInvalid = colors.empty() || positions.empty() || colors.size() != positions.size();
    if (isInvalid) {
        EFFECT_LOG_E("EffectImageFilter::MapColorByBrightness: params is invalid.");
        return nullptr;
    }
    std::vector<Vector4f> colorValues;
    std::vector<float> positionValues;
    for (size_t i = 0; i < colors.size(); i++) {
        Vector4f color = GetLimitedPara(colors[i], POSITIVE_MIN_THRESHOLD);
        float position = std::clamp(positions[i], COLOR_POSITION_LIMITS.first, COLOR_POSITION_LIMITS.second);
        colorValues.push_back(color);
        positionValues.push_back(position);
    }
    return std::make_shared<OHOS::Rosen::EffectImageMapColorByBrightnessFilter>(colorValues, positionValues);
}

std::shared_ptr<EffectImageFilter> EffectImageFilter::GammaCorrection(float gamma)
{
    if (gamma < GAMMA_MIN_THRESHOLD) {
        return nullptr;
    }
    return std::make_shared<OHOS::Rosen::EffectImageGammaCorrectionFilter>(gamma);
}

std::shared_ptr<EffectImageFilter> EffectImageFilter::EllipticalGradientBlur(float blurRadius, float centerX,
    float centerY, float maskRadiusX, float maskRadiusY, const std::vector<float> &positions,
    const std::vector<float> &degrees)
{
    return std::make_shared<EffectImageEllipticalGradientBlurFilter>(
        blurRadius, centerX, centerY, maskRadiusX, maskRadiusY, positions, degrees);
}

std::shared_ptr<EffectImageFilter> EffectImageFilter::Brightness(float degree)
{
    if (degree < BRIGHTNESS_MIN_THRESHOLD || degree > BRIGHTNESS_MAX_THRESHOLD) {
        return nullptr;
    }

    float matrix[20] = {
        1.0f, 0.0f, 0.0f, 0.0f, degree,
        0.0f, 1.0f, 0.0f, 0.0f, degree,
        0.0f, 0.0f, 1.0f, 0.0f, degree,
        0.0f, 0.0f, 0.0f, 1.0f, 0.0f
    };
    auto colorFilter = Drawing::ColorFilter::CreateFloatColorFilter(matrix, Drawing::Clamp::NO_CLAMP);
    auto filter = Drawing::ImageFilter::CreateColorFilterImageFilter(*colorFilter, nullptr);
    return std::make_shared<EffectImageDrawingFilter>(filter);
}

std::shared_ptr<EffectImageFilter> EffectImageFilter::MaskTransition(
    const std::shared_ptr<Media::PixelMap>& topLayerMap,
    const std::shared_ptr<Drawing::GEShaderMask>& mask, float factor, bool inverse)
{
    return std::make_shared<EffectImageMaskTransitionFilter>(topLayerMap, mask, factor, inverse);
}

std::shared_ptr<EffectImageFilter> EffectImageFilter::WaterDropletTransition(
    const std::shared_ptr<OHOS::Media::PixelMap>& topLayerMap,
    const std::shared_ptr<Drawing::GEWaterDropletTransitionFilterParams>& geWaterDropletParams)
{
    return std::make_shared<EffectImageWaterDropletTransitionFilter>(topLayerMap, geWaterDropletParams);
}

DrawingError EffectImageEllipticalGradientBlurFilter::Apply(const std::shared_ptr<EffectImageChain> &image)
{
    if (image == nullptr) {
        return DrawingError::ERR_IMAGE_NULL;
    }

    return image->ApplyEllipticalGradientBlur(
        blurRadius_, centerX_, centerY_, maskRadiusX_, maskRadiusY_, positions_, degrees_);
}

std::shared_ptr<EffectImageFilter> EffectImageFilter::Grayscale()
{
    float matrix[20] = {
        GRAYSCALE_PARAONE, GRAYSCALE_PARATWO, GRAYSCALE_PARATHREE, 0.0f, 0.0f,
        GRAYSCALE_PARAONE, GRAYSCALE_PARATWO, GRAYSCALE_PARATHREE, 0.0f, 0.0f,
        GRAYSCALE_PARAONE, GRAYSCALE_PARATWO, GRAYSCALE_PARATHREE, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f, 0.0f
    };
    auto colorFilter = Drawing::ColorFilter::CreateFloatColorFilter(matrix, Drawing::Clamp::NO_CLAMP);
    auto filter = Drawing::ImageFilter::CreateColorFilterImageFilter(*colorFilter, nullptr);
    return std::make_shared<EffectImageDrawingFilter>(filter);
}

std::shared_ptr<EffectImageFilter> EffectImageFilter::CreateSDF(int spreadFactor, bool generateDerivs)
{
    return std::make_shared<EffectImageSdfFromImageFilter>(spreadFactor, generateDerivs);
}

std::shared_ptr<EffectImageFilter> EffectImageFilter::Invert()
{
    /* invert matrix */
    float matrix[20] = {
        -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 0.0f, 1.0f, 0.0f
    };
    auto colorFilter = Drawing::ColorFilter::CreateFloatColorFilter(matrix, Drawing::Clamp::NO_CLAMP);
    auto filter = Drawing::ImageFilter::CreateColorFilterImageFilter(*colorFilter, nullptr);
    return std::make_shared<EffectImageDrawingFilter>(filter);
}

std::shared_ptr<EffectImageFilter> EffectImageFilter::ApplyColorMatrix(const Drawing::ColorMatrix& colorMatrix)
{
    float matrix[20] = {};
    colorMatrix.GetArray(matrix);
    auto colorFilter = Drawing::ColorFilter::CreateFloatColorFilter(matrix, Drawing::Clamp::NO_CLAMP);
    auto filter = Drawing::ImageFilter::CreateColorFilterImageFilter(*colorFilter, nullptr);
    return std::make_shared<EffectImageDrawingFilter>(filter);
}

DrawingError EffectImageDrawingFilter::Apply(const std::shared_ptr<EffectImageChain>& image)
{
    if (image == nullptr) {
        return DrawingError::ERR_IMAGE_NULL;
    }

    return image->ApplyDrawingFilter(filter_);
}

DrawingError EffectImageBlurFilter::Apply(const std::shared_ptr<EffectImageChain>& image)
{
    if (image == nullptr) {
        return DrawingError::ERR_IMAGE_NULL;
    }

    return image->ApplyBlur(radius_, tileMode_, isDirectionBlur_, angle_);
}

DrawingError EffectImageMapColorByBrightnessFilter::Apply(const std::shared_ptr<EffectImageChain>& image)
{
    if (image == nullptr) {
        return DrawingError::ERR_IMAGE_NULL;
    }
    return image->ApplyMapColorByBrightness(colors_, positions_);
}

DrawingError EffectImageGammaCorrectionFilter::Apply(const std::shared_ptr<EffectImageChain>& image)
{
    if (image == nullptr) {
        return DrawingError::ERR_IMAGE_NULL;
    }
    return image->ApplyGammaCorrection(gamma_);
}

DrawingError EffectImageSdfFromImageFilter::Apply(const std::shared_ptr<EffectImageChain>& image)
{
    if (image == nullptr) {
        return DrawingError::ERR_IMAGE_NULL;
    }
    return image->ApplySDFCreation(spreadFactor_, generateDerivs_);
}

DrawingError EffectImageMaskTransitionFilter::Apply(const std::shared_ptr<EffectImageChain>& image)
{
    if (image == nullptr) {
        return DrawingError::ERR_IMAGE_NULL;
    }
 
    return image->ApplyMaskTransitionFilter(topLayerMap_, mask_, factor_, inverse_);
}

DrawingError EffectImageWaterDropletTransitionFilter::Apply(const std::shared_ptr<EffectImageChain>& image)
{
    if (image == nullptr) {
        return DrawingError::ERR_IMAGE_NULL;
    }
 
    return image->ApplyWaterDropletTransitionFilter(topLayerMap_, waterDropletParams_);
}

DrawingError EffectImageRender::Render(const std::shared_ptr<Media::PixelMap>& srcPixelMap,
    const std::vector<std::shared_ptr<EffectImageFilter>>& effectFilters, bool forceCPU,
    std::shared_ptr<Media::PixelMap>& dstPixelMap)
{
    ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "EffectImageRender::Render");

    auto ret = DrawingError::ERR_OK;
    do {
        auto effectImage = std::make_shared<EffectImageChain>();
        ret = effectImage->Prepare(srcPixelMap, forceCPU);
        if (ret != DrawingError::ERR_OK) {
            EFFECT_LOG_E("EffectImageRender::Render: Failed to prepare image, ret=%{public}d.", ret);
            break;
        }

        for (const auto& filter : effectFilters) {
            if (filter == nullptr) {
                continue;
            }

            ret = filter->Apply(effectImage);
            if (ret != DrawingError::ERR_OK) {
                EFFECT_LOG_E("EffectImageRender::Render: Failed to apply filter, ret=%{public}d.", ret);
                break;
            }
        }
        if (ret != DrawingError::ERR_OK) {
            break;
        }

        ret = effectImage->Draw();
        if (ret != DrawingError::ERR_OK) {
            EFFECT_LOG_E("EffectImageRender::Render: Failed to draw image, ret=%{public}d.", ret);
            break;
        }

        dstPixelMap = effectImage->GetPixelMap();
        if(dstPixelMap == nullptr){
            EFFECT_LOG_E("EffectImageRender::Render: dstPixelMap null.");
            break;
        }
        dstPixelMap->MarkDirty();
    } while (false);

    ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
    return ret;
}

std::shared_ptr<EffectImageFilter> EffectImageFilter::WaterGlass(
    const std::shared_ptr<Drawing::GEWaterGlassDataParams>& params)
{
    return std::make_shared<EffectImageWaterGlassFilter>(params);
}

std::shared_ptr<EffectImageFilter> EffectImageFilter::ReededGlass(
    const std::shared_ptr<Drawing::GEReededGlassDataParams>& params)
{
    return std::make_shared<EffectImageReededGlassFilter>(params);
}

DrawingError EffectImageWaterGlassFilter::Apply(const std::shared_ptr<EffectImageChain>& image)
{
    if (image == nullptr) {
        return DrawingError::ERR_IMAGE_NULL;
    }

    return image->ApplyWaterGlass(waterGlassData_);
}

DrawingError EffectImageReededGlassFilter::Apply(const std::shared_ptr<EffectImageChain>& image)
{
    if (image == nullptr) {
        return DrawingError::ERR_IMAGE_NULL;
    }

    return image->ApplyReededGlass(reededGlassData_);
}

} // namespace OHOS::Rosen