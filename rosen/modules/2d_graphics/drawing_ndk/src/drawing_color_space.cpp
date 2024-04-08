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

#include "drawing_color_space.h"
#include "effect/color_space.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

static ColorSpace* CastToColorSpace(OH_Drawing_ColorSpace* colorSpace)
{
    return reinterpret_cast<ColorSpace*>(colorSpace);
}

OH_Drawing_ColorSpace* OH_Drawing_ColorSpaceCreateSrgb()
{
    return (OH_Drawing_ColorSpace*)new ColorSpace(ColorSpace::ColorSpaceType::SRGB);
}

OH_Drawing_ColorSpace* OH_Drawing_ColorSpaceCreateSrgbLinear()
{
    return (OH_Drawing_ColorSpace*)new ColorSpace(ColorSpace::ColorSpaceType::SRGB_LINEAR);
}

void OH_Drawing_ColorSpaceDestroy(OH_Drawing_ColorSpace* colorSpace)
{
    if (colorSpace == nullptr) {
        return;
    }
    delete CastToColorSpace(colorSpace);
}
