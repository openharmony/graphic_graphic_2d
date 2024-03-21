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

#include "drawing_shader_effect.h"

#include "effect/shader_effect.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

static const Point& CastToPoint(const OH_Drawing_Point& cPoint)
{
    return reinterpret_cast<const Point&>(cPoint);
}

static ShaderEffect* CastToShaderEffect(OH_Drawing_ShaderEffect* cShaderEffect)
{
    return reinterpret_cast<ShaderEffect*>(cShaderEffect);
}

OH_Drawing_ShaderEffect* OH_Drawing_ShaderEffectCreateLinearGradient(const OH_Drawing_Point* cStartPt,
    const OH_Drawing_Point* cEndPt, const uint32_t* colors, const float* pos, uint32_t size,
    OH_Drawing_TileMode cTileMode)
{
    if (cStartPt == nullptr || cEndPt == nullptr || colors == nullptr || pos == nullptr) {
        return nullptr;
    }
    std::vector<ColorQuad> colorsVector;
    std::vector<scalar> posVector;
    for (uint32_t i = 0; i < size; i++) {
        colorsVector.emplace_back(colors[i]);
        posVector.emplace_back(pos[i]);
    }
    return (OH_Drawing_ShaderEffect*)new ShaderEffect(ShaderEffect::ShaderEffectType::LINEAR_GRADIENT,
        CastToPoint(*cStartPt), CastToPoint(*cEndPt), colorsVector, posVector, static_cast<TileMode>(cTileMode));
}

OH_Drawing_ShaderEffect* OH_Drawing_ShaderEffectCreateRadialGradient(const OH_Drawing_Point* cCenterPt, float radius,
    const uint32_t* colors, const float* pos, uint32_t size, OH_Drawing_TileMode cTileMode)
{
    if (cCenterPt == nullptr || colors == nullptr || pos == nullptr) {
        return nullptr;
    }
    std::vector<ColorQuad> colorsVector;
    std::vector<scalar> posVector;
    for (uint32_t i = 0; i < size; i++) {
        colorsVector.emplace_back(colors[i]);
        posVector.emplace_back(pos[i]);
    }
    return (OH_Drawing_ShaderEffect*)new ShaderEffect(ShaderEffect::ShaderEffectType::RADIAL_GRADIENT,
        CastToPoint(*cCenterPt), radius, colorsVector, posVector, static_cast<TileMode>(cTileMode));
}

OH_Drawing_ShaderEffect* OH_Drawing_ShaderEffectCreateSweepGradient(const OH_Drawing_Point* cCenterPt,
    const uint32_t* colors, const float* pos, uint32_t size, OH_Drawing_TileMode cTileMode)
{
    if (cCenterPt == nullptr || colors == nullptr || pos == nullptr) {
        return nullptr;
    }
    std::vector<ColorQuad> colorsVector;
    std::vector<scalar> posVector;
    for (uint32_t i = 0; i < size; i++) {
        colorsVector.emplace_back(colors[i]);
        posVector.emplace_back(pos[i]);
    }
    return (OH_Drawing_ShaderEffect*)new ShaderEffect(ShaderEffect::ShaderEffectType::SWEEP_GRADIENT,
        CastToPoint(*cCenterPt), colorsVector, posVector, static_cast<TileMode>(cTileMode), 0,
        360, nullptr); // 360: endAngle
}

void OH_Drawing_ShaderEffectDestroy(OH_Drawing_ShaderEffect* cShaderEffect)
{
    delete CastToShaderEffect(cShaderEffect);
}
