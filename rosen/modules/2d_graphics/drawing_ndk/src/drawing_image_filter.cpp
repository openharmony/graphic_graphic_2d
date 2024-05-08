/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "drawing_image_filter.h"
#include "effect/image_filter.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

static ColorFilter* CastToColorFilter(OH_Drawing_ColorFilter* cColorFilter)
{
    return reinterpret_cast<ColorFilter*>(cColorFilter);
}

static std::shared_ptr<ImageFilter> CastToImageFilter(OH_Drawing_ImageFilter* cImageFilter)
{
    return std::shared_ptr<ImageFilter>{reinterpret_cast<ImageFilter*>(cImageFilter), [](auto p) {}};
}

OH_Drawing_ImageFilter* OH_Drawing_ImageFilterCreateBlur(float sigmaX, float sigmaY, OH_Drawing_TileMode cTileMode,
    OH_Drawing_ImageFilter* input)
{
    return (OH_Drawing_ImageFilter*)new ImageFilter(ImageFilter::FilterType::BLUR, sigmaX, sigmaY,
        static_cast<TileMode>(cTileMode), CastToImageFilter(input), ImageBlurType::GAUSS);
}

OH_Drawing_ImageFilter* OH_Drawing_ImageFilterCreateFromColorFilter(
    OH_Drawing_ColorFilter* cf, OH_Drawing_ImageFilter* input)
{
    if (cf == nullptr) {
        return nullptr;
    }
    return (OH_Drawing_ImageFilter*)new ImageFilter(
        ImageFilter::FilterType::COLOR_FILTER, *CastToColorFilter(cf), CastToImageFilter(input));
}

void OH_Drawing_ImageFilterDestroy(OH_Drawing_ImageFilter* cImageFilter)
{
    delete reinterpret_cast<ImageFilter*>(cImageFilter);
}
