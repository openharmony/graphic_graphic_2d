/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "sk_image_filter_factory.h"
#include "include/effects/SkImageFilters.h"
#include "include/core/SkColorFilter.h"

namespace OHOS {
namespace Rosen {
static constexpr float GRAYSCALE_PARAONE = 0.2126f;
static constexpr float GRAYSCALE_PARATWO = 0.7152f;
static constexpr float GRAYSCALE_PARATHREE = 0.0722f;

SkTileMode ConvertToSkTileMode(TileMode tileMode)
{
    switch (tileMode) {
        case TileMode::CLAMP:
            return SkTileMode::kClamp;
        case TileMode::REPEAT:
            return SkTileMode::kRepeat;
        case TileMode::MIRROR:
            return SkTileMode::kMirror;
        case TileMode::DECAL:
            return SkTileMode::kDecal;
        default:
            return SkTileMode::kDecal;
    }
}

sk_sp<SkImageFilter> SKImageFilterFactory::Blur(float radius, TileMode tileMode)
{
    SkTileMode skTileMode = ConvertToSkTileMode(tileMode);
    return SkImageFilters::Blur(radius, radius, skTileMode, nullptr);
}

sk_sp<SkImageFilter> SKImageFilterFactory::Brightness(float degree)
{
    float matrix[20] = {
        1, 0, 0, 0, degree,
        0, 1, 0, 0, degree,
        0, 0, 1, 0, degree,
        0, 0, 0, 1, 0
    };

    return SkImageFilters::ColorFilter(SkColorFilters::Matrix(matrix), nullptr);
}

sk_sp<SkImageFilter> SKImageFilterFactory::Grayscale()
{
    float matrix[20] = {
        GRAYSCALE_PARAONE, GRAYSCALE_PARATWO, GRAYSCALE_PARATHREE, 0, 0,
        GRAYSCALE_PARAONE, GRAYSCALE_PARATWO, GRAYSCALE_PARATHREE, 0, 0,
        GRAYSCALE_PARAONE, GRAYSCALE_PARATWO, GRAYSCALE_PARATHREE, 0, 0,
        0, 0, 0, 1, 0
    };

    return SkImageFilters::ColorFilter(SkColorFilters::Matrix(matrix), nullptr);
}

sk_sp<SkImageFilter> SKImageFilterFactory::Invert()
{
    /* invert matrix */
    float matrix[20] = {
        -1.0, 0, 0, 0, 1,
        0, -1.0, 0, 0, 1,
        0, 0, -1.0, 0, 1,
        0, 0, 0, 1, 0
    };

    return SkImageFilters::ColorFilter(SkColorFilters::Matrix(matrix), nullptr);
}

sk_sp<SkImageFilter> SKImageFilterFactory::ApplyColorMatrix(const PixelColorMatrix &matrix)
{
    return SkImageFilters::ColorFilter(SkColorFilters::Matrix(matrix.val), nullptr);
}
} // namespcae Rosen
} // namespace OHOS