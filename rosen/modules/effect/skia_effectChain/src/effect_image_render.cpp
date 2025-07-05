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

std::shared_ptr<EffectImageFilter> EffectImageFilter::Blur(float radius, Drawing::TileMode tileMode)
{
    if (radius < RADIUS_THRESHOLD) {
        return nullptr;
    }

    return std::make_shared<EffectImageBlurFilter>(radius, tileMode);
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

    return image->ApplyBlur(radius_, tileMode_);
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
    } while (false);

    ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
    return ret;
}
} // namespace OHOS::Rosen