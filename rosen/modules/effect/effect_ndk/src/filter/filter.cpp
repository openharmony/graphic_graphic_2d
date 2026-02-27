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

#include "filter.h"

#include "utils/log.h"

namespace OHOS {
namespace Rosen {
Filter::Filter(std::shared_ptr<OHOS::Media::PixelMap> pixelMap) : srcPixelMap_(pixelMap)
{}

std::shared_ptr<OHOS::Media::PixelMap> Filter::GetSrcPixelMap()
{
    return srcPixelMap_;
}

bool Filter::Render(bool forceCPU)
{
    if (srcPixelMap_ == nullptr) {
        return false;
    }
    EffectImageRender imageRender;
    auto error = imageRender.Render(srcPixelMap_, effectFilters_, forceCPU, dstPixelMap_);
    return error == DrawingError::ERR_OK;
}
 
bool Filter::RenderNativeBuffer(bool forceCPU, OH_NativeBuffer* dstNativeBuffer)
{
    if (srcPixelMap_ == nullptr) {
        return false;
    }
    EffectImageRender imageRender;
    auto dstNativeBufferSharedPtr = std::shared_ptr<OH_NativeBuffer>(dstNativeBuffer, [](OH_NativeBuffer* p) {});
    auto error = imageRender.RenderDstNative(srcPixelMap_, dstNativeBufferSharedPtr, effectFilters_, forceCPU);
    return error == DrawingError::ERR_OK;
}

void Filter::AddNextFilter(std::shared_ptr<EffectImageFilter> filter)
{
    effectFilters_.emplace_back(filter);
}

std::shared_ptr<OHOS::Media::PixelMap> Filter::GetPixelMap(bool useCpuRender)
{
    if (!Render(useCpuRender)) {
        return nullptr;
    }
    return dstPixelMap_;
}

bool Filter::Blur(float radius, Drawing::TileMode tileMode)
{
    auto blur = EffectImageFilter::Blur(radius, tileMode);
    if (!blur) {
        return false;
    }
    AddNextFilter(blur);
    return true;
}

bool Filter::Blur(float radius, float angle, Drawing::TileMode tileMode)
{
    auto blur = EffectImageFilter::Blur(radius, angle, tileMode);
    if (!blur) {
        return false;
    }
    AddNextFilter(blur);
    return true;
}

bool Filter::Brightness(float brightness)
{
    auto bright = EffectImageFilter::Brightness(brightness);
    if (!bright) {
        return false;
    }
    AddNextFilter(bright);
    return true;
}

bool Filter::Grayscale()
{
    auto grayscale = EffectImageFilter::Grayscale();
    if (!grayscale) {
        return false;
    }
    AddNextFilter(grayscale);
    return true;
}

bool Filter::Invert()
{
    auto invert = EffectImageFilter::Invert();
    if (!invert) {
        return false;
    }
    AddNextFilter(invert);
    return true;
}

bool Filter::MapColorByBrightness(const std::vector<Vector4f>& colors, const std::vector<float>& positions)
{
    auto filter = EffectImageFilter::MapColorByBrightness(colors, positions);
    if (!filter) {
        return false;
    }
    AddNextFilter(filter);
    return true;
}

bool Filter::GammaCorrection(float gamma)
{
    auto filter = EffectImageFilter::GammaCorrection(gamma);
    if (!filter) {
        return false;
    }
    AddNextFilter(filter);
    return true;
}

bool Filter::SetColorMatrix(const Drawing::ColorMatrix& matrix)
{
    auto applyColorMatrix = EffectImageFilter::ApplyColorMatrix(matrix);
    if (!applyColorMatrix) {
        return false;
    }
    AddNextFilter(applyColorMatrix);
    return true;
}

bool Filter::MaskTransition(const std::shared_ptr<OHOS::Media::PixelMap>& topLayer,
    const std::shared_ptr<Drawing::GEShaderMask>& mask, float factor, bool inverse)
{
    auto filter = EffectImageFilter::MaskTransition(topLayer, mask, factor, inverse);
    if (!filter) {
        return false;
    }
    AddNextFilter(filter);
    return true;
}

bool Filter::WaterDropletTransition(const std::shared_ptr<OHOS::Media::PixelMap>& topLayer,
    const std::shared_ptr<Drawing::GEWaterDropletTransitionFilterParams>& geWaterDropletParams)
{
    auto filter = EffectImageFilter::WaterDropletTransition(topLayer, geWaterDropletParams);
    if (!filter) {
        return false;
    }
    AddNextFilter(filter);
    return true;
}

bool Filter::WaterGlass(const std::shared_ptr<Drawing::GEWaterGlassDataParams>& params)
{
    auto glass = EffectImageFilter::WaterGlass(params);
    AddNextFilter(glass);

    return true;
}

bool Filter::ReededGlass(const std::shared_ptr<Drawing::GEReededGlassDataParams>& params)
{
    auto glass = EffectImageFilter::ReededGlass(params);

    AddNextFilter(glass);

    return true;
}
} // namespace Rosen
} // namespace OHOS