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

#include "sk_image_chain.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
Filter::Filter(std::shared_ptr<OHOS::Media::PixelMap> pixelMap) : srcPixelMap_(pixelMap)
{}

void Filter::Render(bool forceCPU)
{
    Rosen::SKImageChain skImage(srcPixelMap_);
    for (auto filter : skFilters_) {
        skImage.SetFilters(filter);
    }
    skImage.ForceCPU(forceCPU);
    skImage.Draw();
    dstPixelMap_ =  skImage.GetPixelMap();
}

void Filter::AddNextFilter(sk_sp<SkImageFilter> filter)
{
    skFilters_.emplace_back(filter);
}

std::shared_ptr<OHOS::Media::PixelMap> Filter::GetPixelMap()
{
    Render(false);
    return dstPixelMap_;
}

bool Filter::Blur(float radius)
{
    auto blur = Rosen::SKImageFilterFactory::Blur(radius);
    if (!blur) {
        return false;
    }
    AddNextFilter(blur);
    return true;
}

bool Filter::Brightness(float brightness)
{
    auto bright = Rosen::SKImageFilterFactory::Brightness(brightness);
    if (!bright) {
        return false;
    }
    AddNextFilter(bright);
    return true;
}

bool Filter::Grayscale()
{
    auto grayscale = Rosen::SKImageFilterFactory::Grayscale();
    if (!grayscale) {
        return false;
    }
    AddNextFilter(grayscale);
    return true;
}

bool Filter::Invert()
{
    auto invert = Rosen::SKImageFilterFactory::Invert();
    if (!invert) {
        return false;
    }
    AddNextFilter(invert);
    return true;
}

bool Filter::SetColorMatrix(const PixelColorMatrix& matrix)
{
    auto applyColorMatrix = Rosen::SKImageFilterFactory::ApplyColorMatrix(matrix);
    if (!applyColorMatrix) {
        return false;
    }
    AddNextFilter(applyColorMatrix);
    return true;
}
} // namespace Rosen
} // namespace OHOS