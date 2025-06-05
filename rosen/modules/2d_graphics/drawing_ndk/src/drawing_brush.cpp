/*
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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

#include "drawing_brush.h"

#include "drawing_canvas_utils.h"
#include "drawing_helper.h"

#include "draw/brush.h"
#include "draw/color.h"
#include "ndk_color_space.h"
#include "utils/colorspace_convertor.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

static Brush* CastToBrush(OH_Drawing_Brush* cBrush)
{
    return reinterpret_cast<Brush*>(cBrush);
}

static const Brush& CastToBrush(const OH_Drawing_Brush& cBrush)
{
    return reinterpret_cast<const Brush&>(cBrush);
}

static const Filter& CastToFilter(const OH_Drawing_Filter& cFilter)
{
    return reinterpret_cast<const Filter&>(cFilter);
}

static const Filter* CastToFilter(const OH_Drawing_Filter* cFilter)
{
    return reinterpret_cast<const Filter*>(cFilter);
}

OH_Drawing_Brush* OH_Drawing_BrushCreate()
{
    return (OH_Drawing_Brush*)new Brush;
}

OH_Drawing_Brush* OH_Drawing_BrushCopy(OH_Drawing_Brush* cBrush)
{
    Brush* brush = CastToBrush(cBrush);
    if (brush == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return nullptr;
    }
    return (OH_Drawing_Brush*)new Brush(*brush);
}

void OH_Drawing_BrushDestroy(OH_Drawing_Brush* cBrush)
{
    if (!cBrush) {
        return;
    }
    delete CastToBrush(cBrush);
}

bool OH_Drawing_BrushIsAntiAlias(const OH_Drawing_Brush* cBrush)
{
    if (cBrush == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return false;
    }
    return CastToBrush(*cBrush).IsAntiAlias();
}

void OH_Drawing_BrushSetAntiAlias(OH_Drawing_Brush* cBrush, bool aa)
{
    Brush* brush = CastToBrush(cBrush);
    if (brush == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    brush->SetAntiAlias(aa);
}

uint32_t OH_Drawing_BrushGetColor(const OH_Drawing_Brush* cBrush)
{
    if (cBrush == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return 0;
    }
    return CastToBrush(*cBrush).GetColor().CastToColorQuad();
}

void OH_Drawing_BrushSetColor(OH_Drawing_Brush* cBrush, uint32_t color)
{
    Brush* brush = CastToBrush(cBrush);
    if (brush == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    brush->SetColor(color);
}

uint8_t OH_Drawing_BrushGetAlpha(const OH_Drawing_Brush* cBrush)
{
    if (cBrush == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return 0;
    }
    return CastToBrush(*cBrush).GetAlpha();
}

void OH_Drawing_BrushSetAlpha(OH_Drawing_Brush* cBrush, uint8_t alpha)
{
    Brush* brush = CastToBrush(cBrush);
    if (brush == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    brush->SetAlpha(alpha);
}

void OH_Drawing_BrushSetShaderEffect(OH_Drawing_Brush* cBrush, OH_Drawing_ShaderEffect* cShaderEffect)
{
    Brush* brush = CastToBrush(cBrush);
    if (brush == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    if (cShaderEffect == nullptr) {
        brush->SetShaderEffect(nullptr);
        return;
    }
    auto shaderEffectHandle = Helper::CastTo<OH_Drawing_ShaderEffect*, NativeHandle<ShaderEffect>*>(cShaderEffect);
    brush->SetShaderEffect(shaderEffectHandle->value);
}

void OH_Drawing_BrushSetShadowLayer(OH_Drawing_Brush* cBrush, OH_Drawing_ShadowLayer* cShadowLayer)
{
    Brush* brush = CastToBrush(cBrush);
    if (brush == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    if (cShadowLayer == nullptr) {
        brush->SetLooper(nullptr);
        return;
    }
    auto blurDrawLooperHandle = Helper::CastTo<OH_Drawing_ShadowLayer*, NativeHandle<BlurDrawLooper>*>(cShadowLayer);
    brush->SetLooper(blurDrawLooperHandle->value);
}

void OH_Drawing_BrushSetFilter(OH_Drawing_Brush* cBrush, OH_Drawing_Filter* cFilter)
{
    Brush* brush = CastToBrush(cBrush);
    if (brush == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    if (cFilter == nullptr) {
        Filter filter;
        brush->SetFilter(filter);
        return;
    }
    brush->SetFilter(CastToFilter(*cFilter));
}

void OH_Drawing_BrushGetFilter(OH_Drawing_Brush* cBrush, OH_Drawing_Filter* cFilter)
{
    Brush* brush = CastToBrush(cBrush);
    if (brush == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    Filter* filter = const_cast<Filter*>(CastToFilter(cFilter));
    if (filter == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    *filter = brush->GetFilter();
}

void OH_Drawing_BrushSetBlendMode(OH_Drawing_Brush* cBrush, OH_Drawing_BlendMode cBlendMode)
{
    Brush* brush = CastToBrush(cBrush);
    if (brush == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    if (cBlendMode < BLEND_MODE_CLEAR || cBlendMode > BLEND_MODE_LUMINOSITY) {
        g_drawingErrorCode = OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE;
        return;
    }
    brush->SetBlendMode(static_cast<BlendMode>(cBlendMode));
}

static NativeColorSpaceManager* OHNativeColorSpaceManagerToNativeColorSpaceManager(
    OH_NativeColorSpaceManager* nativeColorSpaceManager)
{
    return reinterpret_cast<NativeColorSpaceManager*>(nativeColorSpaceManager);
}

OH_Drawing_ErrorCode OH_Drawing_BrushSetColor4f(OH_Drawing_Brush* cBrush, float a, float r, float g, float b,
    OH_NativeColorSpaceManager* colorSpaceManager)
{
    Brush* brush = CastToBrush(cBrush);
    if (brush == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    NativeColorSpaceManager* nativeColorSpaceManager =
        OHNativeColorSpaceManagerToNativeColorSpaceManager(colorSpaceManager);
    std::shared_ptr<ColorSpace> drawingColorSpace = nullptr;
    if (nativeColorSpaceManager) {
        auto colorManagerColorSpace = std::make_shared<OHOS::ColorManager::ColorSpace>(
            nativeColorSpaceManager->GetInnerColorSpace());
        drawingColorSpace =
            Drawing::ColorSpaceConvertor::ColorSpaceConvertToDrawingColorSpace(colorManagerColorSpace);
    }

    brush->SetColor(Color4f{r, g, b, a}, drawingColorSpace);
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode OH_Drawing_BrushGetAlphaFloat(const OH_Drawing_Brush* brush, float* a)
{
    if (brush == nullptr || a == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    *a = CastToBrush(*brush).GetAlphaF();
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode OH_Drawing_BrushGetRedFloat(const OH_Drawing_Brush* brush, float* r)
{
    if (brush == nullptr || r == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    *r = CastToBrush(*brush).GetRedF();
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode OH_Drawing_BrushGetGreenFloat(const OH_Drawing_Brush* brush, float* g)
{
    if (brush == nullptr || g == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    *g = CastToBrush(*brush).GetGreenF();
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode OH_Drawing_BrushGetBlueFloat(const OH_Drawing_Brush* brush, float* b)
{
    if (brush == nullptr || b == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    *b = CastToBrush(*brush).GetBlueF();
    return OH_DRAWING_SUCCESS;
}

void OH_Drawing_BrushReset(OH_Drawing_Brush* cBrush)
{
    Brush* brush = CastToBrush(cBrush);
    if (brush == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return;
    }
    brush->Reset();
}
