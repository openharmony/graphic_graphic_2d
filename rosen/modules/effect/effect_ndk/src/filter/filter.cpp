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

bool Filter::Render(bool forceCPU)
{
    if (srcPixelMap_ == nullptr) {
        return false;
    }
    EffectImageRender imageRender;
    auto error = imageRender.Render(srcPixelMap_, effectFilters_, forceCPU, dstPixelMap_);
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

bool Filter::SetColorMatrix(const Drawing::ColorMatrix& matrix)
{
    auto applyColorMatrix = EffectImageFilter::ApplyColorMatrix(matrix);
    if (!applyColorMatrix) {
        return false;
    }
    AddNextFilter(applyColorMatrix);
    return true;
}
} // namespace Rosen
} // namespace OHOS