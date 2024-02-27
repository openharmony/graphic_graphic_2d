/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "c/drawing_brush.h"

#include "draw/brush.h"

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

static ShaderEffect* CastToShaderEffect(OH_Drawing_ShaderEffect* cShaderEffect)
{
    return reinterpret_cast<ShaderEffect*>(cShaderEffect);
}

static const Filter& CastToFilter(const OH_Drawing_Filter& cFilter)
{
    return reinterpret_cast<const Filter&>(cFilter);
}

OH_Drawing_Brush* OH_Drawing_BrushCreate()
{
    return (OH_Drawing_Brush*)new Brush;
}

void OH_Drawing_BrushDestroy(OH_Drawing_Brush* cBrush)
{
    delete CastToBrush(cBrush);
}

bool OH_Drawing_BrushIsAntiAlias(const OH_Drawing_Brush* cBrush)
{
    if (cBrush == nullptr) {
        return false;
    }
    return CastToBrush(*cBrush).IsAntiAlias();
}

void OH_Drawing_BrushSetAntiAlias(OH_Drawing_Brush* cBrush, bool aa)
{
    Brush* brush = CastToBrush(cBrush);
    if (brush == nullptr) {
        return;
    }
    brush->SetAntiAlias(aa);
}

uint32_t OH_Drawing_BrushGetColor(const OH_Drawing_Brush* cBrush)
{
    if (cBrush == nullptr) {
        return 0;
    }
    return CastToBrush(*cBrush).GetColor().CastToColorQuad();
}

void OH_Drawing_BrushSetColor(OH_Drawing_Brush* cBrush, uint32_t color)
{
    Brush* brush = CastToBrush(cBrush);
    if (brush == nullptr) {
        return;
    }
    brush->SetColor(color);
}

uint8_t OH_Drawing_BrushGetAlpha(const OH_Drawing_Brush* cBrush)
{
    if (cBrush == nullptr) {
        return 0;
    }
    return CastToBrush(*cBrush).GetAlpha();
}

void OH_Drawing_BrushSetAlpha(OH_Drawing_Brush* cBrush, uint8_t alpha)
{
    Brush* brush = CastToBrush(cBrush);
    if (brush == nullptr) {
        return;
    }
    brush->SetAlpha(alpha);
}

void OH_Drawing_BrushSetShaderEffect(OH_Drawing_Brush* cBrush, OH_Drawing_ShaderEffect* cShaderEffect)
{
    Brush* brush = CastToBrush(cBrush);
    if (brush == nullptr) {
        return;
    }
    brush->SetShaderEffect(std::shared_ptr<ShaderEffect>{CastToShaderEffect(cShaderEffect), [](auto p) {}});
}

void OH_Drawing_BrushSetFilter(OH_Drawing_Brush* cBrush, OH_Drawing_Filter* cFilter)
{
    Brush* brush = CastToBrush(cBrush);
    if (brush == nullptr) {
        return;
    }
    if (cFilter == nullptr) {
        Filter filter;
        brush->SetFilter(filter);
        return;
    }
    brush->SetFilter(CastToFilter(*cFilter));
}

void OH_Drawing_BrushSetBlendMode(OH_Drawing_Brush* cBrush, OH_Drawing_BlendMode cBlendMode)
{
    Brush* brush = CastToBrush(cBrush);
    if (brush == nullptr) {
        return;
    }
    brush->SetBlendMode(static_cast<BlendMode>(cBlendMode));
}