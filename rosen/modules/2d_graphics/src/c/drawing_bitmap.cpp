/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "c/drawing_bitmap.h"

#include "draw/color.h"
#include "image/bitmap.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

static Bitmap* CastToBitmap(OH_Drawing_Bitmap* cBitmap)
{
    return reinterpret_cast<Bitmap*>(cBitmap);
}

static OHOS::Rosen::Drawing::ColorType CColorFormatCastToColorType(OH_Drawing_ColorFormat cColorFormat)
{
    OHOS::Rosen::Drawing::ColorType colorType = OHOS::Rosen::Drawing::COLORTYPE_UNKNOWN;
    switch (cColorFormat) {
#define CASE_MAP(S, T) case (S): colorType = (T); break
        CASE_MAP(COLOR_FORMAT_UNKNOWN, OHOS::Rosen::Drawing::COLORTYPE_UNKNOWN);
        CASE_MAP(COLOR_FORMAT_ALPHA_8, OHOS::Rosen::Drawing::COLORTYPE_ALPHA_8);
        CASE_MAP(COLOR_FORMAT_RGB_565, OHOS::Rosen::Drawing::COLORTYPE_RGB_565);
        CASE_MAP(COLOR_FORMAT_ARGB_4444, OHOS::Rosen::Drawing::COLORTYPE_ARGB_4444);
        CASE_MAP(COLOR_FORMAT_RGBA_8888, OHOS::Rosen::Drawing::COLORTYPE_RGBA_8888);
        CASE_MAP(COLOR_FORMAT_BGRA_8888, OHOS::Rosen::Drawing::COLORTYPE_BGRA_8888);
#undef CASE_MAP
    }
    return colorType;
}

static OHOS::Rosen::Drawing::AlphaType CAlphaFormatCastToAlphaType(OH_Drawing_AlphaFormat cAlphaFormat)
{
    OHOS::Rosen::Drawing::AlphaType alphaType = OHOS::Rosen::Drawing::ALPHATYPE_UNKNOWN;
    switch (cAlphaFormat) {
#define CASE_MAP(S, T) case (S): alphaType = (T); break
        CASE_MAP(ALPHA_FORMAT_UNKNOWN, OHOS::Rosen::Drawing::ALPHATYPE_UNKNOWN);
        CASE_MAP(ALPHA_FORMAT_OPAQUYE, OHOS::Rosen::Drawing::ALPHATYPE_OPAQUYE);
        CASE_MAP(ALPHA_FORMAT_PREMUL, OHOS::Rosen::Drawing::ALPHATYPE_PREMUL);
        CASE_MAP(ALPHA_FORMAT_UNPREMUL, OHOS::Rosen::Drawing::ALPHATYPE_UNPREMUL);
#undef CASE_MAP
    }
    return alphaType;
}

OH_Drawing_Bitmap* OH_Drawing_BitmapCreate()
{
    return (OH_Drawing_Bitmap*)new Bitmap;
}

void OH_Drawing_BitmapDestory(OH_Drawing_Bitmap* cBitmap)
{
    delete CastToBitmap(cBitmap);
}

void OH_Drawing_BitmapBuild(OH_Drawing_Bitmap* cBitmap, const uint32_t width, const uint32_t height,
    const OH_Drawing_BitmapFormat* cBitmapFormat)
{
    ColorType colorType = CColorFormatCastToColorType(cBitmapFormat->colorFormat);
    AlphaType alphaType = CAlphaFormatCastToAlphaType(cBitmapFormat->alphaFormat);

    BitmapFormat format { colorType, alphaType };
    CastToBitmap(cBitmap)->Build(width, height, format);
}

uint32_t OH_Drawing_BitmapGetWidth(OH_Drawing_Bitmap* cBitmap)
{
    return CastToBitmap(cBitmap)->GetWidth();
}

uint32_t OH_Drawing_BitmapGetHeight(OH_Drawing_Bitmap* cBitmap)
{
    return CastToBitmap(cBitmap)->GetHeight();
}

void* OH_Drawing_BitmapGetPixels(OH_Drawing_Bitmap* cBitmap)
{
    return CastToBitmap(cBitmap)->GetPixels();
}
