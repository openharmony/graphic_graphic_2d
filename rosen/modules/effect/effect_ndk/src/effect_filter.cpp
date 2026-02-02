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

#include "effect_filter.h"

#include "filter/filter.h"

#include "utils/log.h"

using namespace OHOS;
using namespace Rosen;

namespace {
    static constexpr uint8_t COLOR_MAX_COUNTS = 5; // The colors max counts of mapColorByBrightness
    static constexpr uint8_t COLOR_MIN_COUNTS = 1; // The colors min counts of mapColorByBrightness
}

static Filter* CastToFilter(OH_Filter* filter)
{
    return reinterpret_cast<Filter*>(filter);
}

static OH_Filter* CastToOH_Filter(Filter* filter)
{
    return reinterpret_cast<OH_Filter*>(filter);
}

EffectErrorCode OH_Filter_CreateEffect(OH_PixelmapNative* pixelmap, OH_Filter** filter)
{
    if (!pixelmap || !filter) {
        return EFFECT_BAD_PARAMETER;
    }
    *filter = CastToOH_Filter(new Filter(pixelmap->GetInnerPixelmap()));
    if (*filter == nullptr) {
        return EFFECT_BAD_PARAMETER;
    }
    return EFFECT_SUCCESS;
}

EffectErrorCode OH_Filter_Release(OH_Filter* filter)
{
    if (filter == nullptr) {
        return EFFECT_BAD_PARAMETER;
    }
    delete CastToFilter(filter);
    return EFFECT_SUCCESS;
}

EffectErrorCode OH_Filter_Blur(OH_Filter* filter, float radius)
{
    if (!filter || !(CastToFilter(filter)->Blur(radius))) {
        return EFFECT_BAD_PARAMETER;
    }
    return EFFECT_SUCCESS;
}

EffectErrorCode OH_Filter_BlurWithTileMode(OH_Filter* filter, float radius, EffectTileMode tileMode)
{
    Drawing::TileMode drawingTileMode = static_cast<Drawing::TileMode>(tileMode);
    if (!filter || !(CastToFilter(filter)->Blur(radius, drawingTileMode))) {
        return EFFECT_BAD_PARAMETER;
    }
    return EFFECT_SUCCESS;
}

EffectErrorCode OH_Filter_BlurWithDirection(OH_Filter* filter, float radius, float effectDirection,
    EffectTileMode tileMode)
{
    Drawing::TileMode drawingTileMode = static_cast<Drawing::TileMode>(tileMode);
    if (!filter || !(CastToFilter(filter)->Blur(radius, effectDirection, drawingTileMode))) {
        return EFFECT_BAD_PARAMETER;
    }
    return EFFECT_SUCCESS;
}

EffectErrorCode OH_Filter_Brighten(OH_Filter* filter, float brightness)
{
    if (!filter || !(CastToFilter(filter)->Brightness(brightness))) {
        return EFFECT_BAD_PARAMETER;
    }
    return EFFECT_SUCCESS;
}

EffectErrorCode OH_Filter_GrayScale(OH_Filter* filter)
{
    if (!filter || !(CastToFilter(filter)->Grayscale())) {
        return EFFECT_BAD_PARAMETER;
    }
    return EFFECT_SUCCESS;
}

EffectErrorCode OH_Filter_Invert(OH_Filter* filter)
{
    if (!filter || !(CastToFilter(filter)->Invert())) {
        return EFFECT_BAD_PARAMETER;
    }
    return EFFECT_SUCCESS;
}

EffectErrorCode OH_Filter_SetColorMatrix(OH_Filter* filter, OH_Filter_ColorMatrix* matrix)
{
    if (!filter || !matrix) {
        return EFFECT_BAD_PARAMETER;
    }
    Drawing::ColorMatrix colorMatrix;
    float matrixArr[Drawing::ColorMatrix::MATRIX_SIZE] = { 0 };
    for (size_t i = 0; i < Drawing::ColorMatrix::MATRIX_SIZE; i++) {
        matrixArr[i] = matrix->val[i];
    }
    colorMatrix.SetArray(matrixArr);
    if (!(CastToFilter(filter)->SetColorMatrix(colorMatrix))) {
        return EFFECT_BAD_PARAMETER;
    }
    return EFFECT_SUCCESS;
}

EffectErrorCode OH_Filter_MapColorByBrightness(OH_Filter* filter, OH_Filter_MapColorByBrightnessParams* params)
{
    bool isInvalid = !filter || !params || !params->colors || !params->positions ||
        params->colorsNum < COLOR_MIN_COUNTS;
    if (isInvalid) {
        return EFFECT_BAD_PARAMETER;
    }

    std::vector<Vector4f> colors;
    std::vector<float> positions;
    size_t n = params->colorsNum > COLOR_MAX_COUNTS ? COLOR_MAX_COUNTS : params->colorsNum;
    for (size_t i = 0; i < n; i++) {
        Vector4f color = {params->colors[i].red, params->colors[i].green,
            params->colors[i].blue, params->colors[i].alpha};
        colors.push_back(color);
        positions.push_back(params->positions[i]);
    }
    if (!(CastToFilter(filter)->MapColorByBrightness(colors, positions))) {
        return EFFECT_BAD_PARAMETER;
    }
    return EFFECT_SUCCESS;
}

EffectErrorCode OH_Filter_GammaCorrection(OH_Filter* filter, float gamma)
{
    if (!filter || !(CastToFilter(filter)->GammaCorrection(gamma))) {
        return EFFECT_BAD_PARAMETER;
    }
    return EFFECT_SUCCESS;
}

EffectErrorCode OH_Filter_GetEffectPixelMap(OH_Filter* filter, OH_PixelmapNative** pixelmap)
{
    if (!pixelmap || !filter) {
        return EFFECT_BAD_PARAMETER;
    }
    *pixelmap = new OH_PixelmapNative(CastToFilter(filter)->GetPixelMap());
    if (*pixelmap == nullptr) {
        return EFFECT_BAD_PARAMETER;
    }
    return EFFECT_SUCCESS;
}

EffectErrorCode OH_Filter_WaterGlass(OH_Filter* filter, OH_Filter_WaterGlassDataParams* waterGlassParams)
{
    if (filter == nullptr || waterGlassParams == nullptr) {
        return EFFECT_BAD_PARAMETER;
    }

    std::shared_ptr<Drawing::GEWaterGlassDataParams> geWaterParams =
        std::make_shared<Drawing::GEWaterGlassDataParams>();

    static constexpr float waveRefractionK = 3.f;

    geWaterParams->waveCenter.SetX(waterGlassParams->waveCenter.x);
    geWaterParams->waveCenter.SetY(waterGlassParams->waveCenter.y);
    geWaterParams->waveSourceXY.SetX(waterGlassParams->waveSourceXY.x);
    geWaterParams->waveSourceXY.SetY(waterGlassParams->waveSourceXY.y);
    geWaterParams->waveDistortXY.SetX(waterGlassParams->waveDistortXY.x);
    geWaterParams->waveDistortXY.SetY(waterGlassParams->waveDistortXY.y);
    geWaterParams->waveDensityXY.SetX(waterGlassParams->waveDensityXY.x);
    geWaterParams->waveDensityXY.SetY(waterGlassParams->waveDensityXY.y);
    geWaterParams->waveStrength = waterGlassParams->waveStrength;
    geWaterParams->waveLightStrength = waterGlassParams->waveLightStrength;
    geWaterParams->waveRefraction = waterGlassParams->waveRefraction * waveRefractionK;
    geWaterParams->waveSpecular = waterGlassParams->waveSpecular;
    geWaterParams->waveFrequency = waterGlassParams->waveFrequency;
    geWaterParams->waveShapeDistortion = waterGlassParams->waveShapeDistortion;
    geWaterParams->waveNoiseStrength = waterGlassParams->waveNoiseStrength;
    geWaterParams->waveMaskSize.SetX(waterGlassParams->waveMaskSize.x);
    geWaterParams->waveMaskSize.SetY(waterGlassParams->waveMaskSize.y);
    geWaterParams->waveMaskRadius = waterGlassParams->waveMaskRadius;
    geWaterParams->borderRadius = waterGlassParams->borderRadius;
    geWaterParams->borderThickness = waterGlassParams->borderThickness;
    geWaterParams->borderScope = waterGlassParams->borderScope;
    geWaterParams->borderStrength = waterGlassParams->borderStrength;
    geWaterParams->progress = waterGlassParams->progress;
    CastToFilter(filter)->WaterGlass(geWaterParams);

    return EFFECT_SUCCESS;
}

EffectErrorCode OH_Filter_ReededGlass(OH_Filter* filter, OH_Filter_ReededGlassDataParams* reededGlassParams)
{
    if (filter == nullptr || reededGlassParams == nullptr) {
        return EFFECT_BAD_PARAMETER;
    }
    std::shared_ptr<Drawing::GEReededGlassDataParams> geReededparams =
        std::make_shared<Drawing::GEReededGlassDataParams>();

    geReededparams->refractionFactor = reededGlassParams->refractionFactor;
    geReededparams->dispersionStrength = reededGlassParams->dispersionStrength;
    geReededparams->roughness = reededGlassParams->roughness;
    geReededparams->noiseFrequency = reededGlassParams->noiseFrequency;
    geReededparams->horizontalPatternNumber = reededGlassParams->horizontalPatternNumber;
    geReededparams->saturationFactor = reededGlassParams->saturationFactor;
    geReededparams->borderLightStrength = reededGlassParams->borderLightStrength;
    geReededparams->pointLightColor.redF_ = reededGlassParams->pointLightColor.red;
    geReededparams->pointLightColor.greenF_ = reededGlassParams->pointLightColor.green;
    geReededparams->pointLightColor.blueF_ = reededGlassParams->pointLightColor.blue;
    geReededparams->borderLightWidth = reededGlassParams->borderLightWidth;
    geReededparams->pointLight1Position.SetX(reededGlassParams->pointLight1Position.x);
    geReededparams->pointLight1Position.SetY(reededGlassParams->pointLight1Position.y);
    geReededparams->pointLight1Strength = reededGlassParams->pointLight1Strength;
    geReededparams->pointLight2Position.SetX(reededGlassParams->pointLight2Position.x);
    geReededparams->pointLight2Position.SetY(reededGlassParams->pointLight2Position.y);
    geReededparams->pointLight2Strength = reededGlassParams->pointLight2Strength;
    CastToFilter(filter)->ReededGlass(geReededparams);

    return EFFECT_SUCCESS;
}
