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

bool Filter::Render(bool forceCPU)
{
    if (srcPixelMap_ == nullptr) {
        return false;
    }
    Rosen::SKImageChain skImage(srcPixelMap_);
    DrawError error = skImage.Render(skFilters_, forceCPU, dstPixelMap_);
    return error == DrawError::ERR_OK;
}

void Filter::AddNextFilter(sk_sp<SkImageFilter> filter)
{
    skFilters_.emplace_back(filter);
}

std::shared_ptr<OHOS::Media::PixelMap> Filter::GetPixelMap()
{
    if (!Render(false)) {
        return nullptr;
    }
    return dstPixelMap_;
}

bool Filter::Blur(float radius, SkTileMode skTileMode)
{
    auto blur = Rosen::SKImageFilterFactory::Blur(radius, skTileMode);
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