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
#include "ge_linear_gradient_shader_mask.h"
#include "ge_radial_gradient_shader_mask.h"

#include "utils/log.h"

using namespace OHOS;
using namespace Rosen;

namespace {
static constexpr uint8_t COLOR_MAX_COUNTS = 5; // The colors max counts of mapColorByBrightness
static constexpr uint8_t COLOR_MIN_COUNTS = 1; // The colors min counts of mapColorByBrightness
static constexpr uint8_t FRACTION_STOPS_LENGTH_MIN = 2;
static constexpr uint8_t FRACTION_STOPS_LENGTH_MAX = 12;
static constexpr float RADIAL_GRADIENT_MASK_RADIUS_MAX = 10.0f;
static constexpr float WATER_DROPLET_RADIUS_MAX = 10.0f;
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

EffectErrorCode LinearGradientMaskTransition(Filter* effectFilter,
    std::shared_ptr<OHOS::Media::PixelMap> topLayerPixelmap, const OH_Filter_LinearGradientMask* linearGradientMask,
    float factor, bool inverse)
{
    if (linearGradientMask->fractionStopsLength > FRACTION_STOPS_LENGTH_MAX ||
        linearGradientMask->fractionStopsLength < FRACTION_STOPS_LENGTH_MIN ||
        !linearGradientMask->fractionStops) {
        return EFFECT_BAD_PARAMETER;
    }
 
    std::vector<std::pair<float, float>> fractionStops;
    for (uint32_t i = 0; i < linearGradientMask->fractionStopsLength; ++i) {
        fractionStops.push_back({ std::clamp(linearGradientMask->fractionStops[i].x, 0.0f, 1.0f),
            std::clamp(linearGradientMask->fractionStops[i].y, 0.0f, 1.0f) });
    }
 
    int32_t width = effectFilter->GetSrcPixelMap()->GetWidth();
    int32_t height = effectFilter->GetSrcPixelMap()->GetHeight();
    auto startPosition = Drawing::Point(std::clamp(linearGradientMask->startPosition.x, 0.0f, 1.0f) * width,
        std::clamp(linearGradientMask->startPosition.y, 0.0f, 1.0f) * height);
    auto endPosition = Drawing::Point(std::clamp(linearGradientMask->endPosition.x, 0.0f, 1.0f) * width,
        std::clamp(linearGradientMask->endPosition.y, 0.0f, 1.0f) * height);
    Drawing::GELinearGradientShaderMaskParams geLinearGradientMaskParams{
        fractionStops, startPosition, endPosition};
    auto transitionMask = std::static_pointer_cast<Drawing::GEShaderMask>(
        std::make_shared<Drawing::GELinearGradientShaderMask>(geLinearGradientMaskParams));
    if (!effectFilter->MaskTransition(topLayerPixelmap, transitionMask, std::clamp(factor, 0.0f, 1.0f), inverse)) {
        return EFFECT_BAD_PARAMETER;
    }
    return EFFECT_SUCCESS;
}
 
EffectErrorCode RadialGradientMaskTransition(Filter* effectFilter,
    std::shared_ptr<OHOS::Media::PixelMap> topLayerPixelmap, const OH_Filter_RadialGradientMask* radialGradientMask,
    float factor, bool inverse)
{
    if (radialGradientMask->fractionStopsLength > FRACTION_STOPS_LENGTH_MAX ||
        radialGradientMask->fractionStopsLength < FRACTION_STOPS_LENGTH_MIN ||
        !radialGradientMask->fractionStops) {
        return EFFECT_BAD_PARAMETER;
    }
 
    std::vector<float> colors;
    std::vector<float> positions;
    for (uint32_t i = 0; i < radialGradientMask->fractionStopsLength; ++i) {
        // colors and positions clamped in the mask shader implementation
        colors.push_back(radialGradientMask->fractionStops[i].x);
        positions.push_back(radialGradientMask->fractionStops[i].y);
    }

    Drawing::GERadialGradientShaderMaskParams geRadialGradientMaskParams{
        { std::clamp(radialGradientMask->center.x, -RADIAL_GRADIENT_MASK_RADIUS_MAX, RADIAL_GRADIENT_MASK_RADIUS_MAX),
        std::clamp(radialGradientMask->center.y, -RADIAL_GRADIENT_MASK_RADIUS_MAX, RADIAL_GRADIENT_MASK_RADIUS_MAX) },
        std::clamp(radialGradientMask->radiusX, 0.0f, RADIAL_GRADIENT_MASK_RADIUS_MAX),
        std::clamp(radialGradientMask->radiusY, 0.0f, RADIAL_GRADIENT_MASK_RADIUS_MAX),
        colors, positions};
    auto transitionMask = std::static_pointer_cast<Drawing::GEShaderMask>(
        std::make_shared<Drawing::GERadialGradientShaderMask>(geRadialGradientMaskParams));
    if (!effectFilter->MaskTransition(topLayerPixelmap, transitionMask, std::clamp(factor, 0.0f, 1.0f), inverse)) {
        return EFFECT_BAD_PARAMETER;
    }
    return EFFECT_SUCCESS;
}

EffectErrorCode OH_Filter_MaskTransition(OH_Filter* filter,
    OH_PixelmapNative* transitionImage, void* mask, EffectMaskType maskType, float factor, bool inverse)
{
    auto effectFilter = CastToFilter(filter);
    if (!effectFilter || !transitionImage || !mask) {
        return EFFECT_BAD_PARAMETER;
    }
    auto topLayerPixelmap = transitionImage->GetInnerPixelmap();
    switch (maskType) {
        case EffectMaskType::LINEAR_GRADIENT_MASK: {
            OH_Filter_LinearGradientMask* linearGradientMask = static_cast<OH_Filter_LinearGradientMask*>(mask);
            return LinearGradientMaskTransition(effectFilter, topLayerPixelmap, linearGradientMask, factor, inverse);
        }
        case EffectMaskType::RADIAL_GRADIENT_MASK: {
            OH_Filter_RadialGradientMask* radialGradientMask = static_cast<OH_Filter_RadialGradientMask*>(mask);
            return RadialGradientMaskTransition(effectFilter, topLayerPixelmap, radialGradientMask, factor, inverse);
        }
        default: {
            return EFFECT_BAD_PARAMETER;
        }
    }
    return EFFECT_SUCCESS;
}
 
EffectErrorCode OH_Filter_WaterDropletTransition(OH_Filter* filter,
    OH_PixelmapNative* transitionImage, OH_Filter_WaterDropletParams* waterDropletParams, bool inverse)
{
    auto effectFilter = CastToFilter(filter);
    if (!effectFilter || !transitionImage || !waterDropletParams) {
        return EFFECT_BAD_PARAMETER;
    }
    auto topLayerPixelmap = transitionImage->GetInnerPixelmap();
    std::shared_ptr<Drawing::GEWaterDropletTransitionFilterParams> geWaterDropletParams =
        std::make_shared<Drawing::GEWaterDropletTransitionFilterParams>();

    geWaterDropletParams->inverse = inverse;
    geWaterDropletParams->progress = waterDropletParams->progress;
    geWaterDropletParams->radius = waterDropletParams->radius;
    geWaterDropletParams->transitionFadeWidth = waterDropletParams->transitionFadeWidth;
    geWaterDropletParams->distortionIntensity = waterDropletParams->distortionIntensity;
    geWaterDropletParams->distortionThickness = waterDropletParams->distortionThickness;
    geWaterDropletParams->lightStrength = waterDropletParams->lightStrength;
    geWaterDropletParams->lightSoftness = waterDropletParams->lightSoftness;
    geWaterDropletParams->noiseScaleX = waterDropletParams->noiseScaleX;
    geWaterDropletParams->noiseScaleY = waterDropletParams->noiseScaleY;
    geWaterDropletParams->noiseStrengthX = waterDropletParams->noiseStrengthX;
    geWaterDropletParams->noiseStrengthY = waterDropletParams->noiseStrengthY;

    geWaterDropletParams->progress = geWaterDropletParams->progress < 0.0f ? 0.0f : geWaterDropletParams->progress;
    geWaterDropletParams->radius = std::clamp(geWaterDropletParams->radius, 0.0f, WATER_DROPLET_RADIUS_MAX);
    geWaterDropletParams->transitionFadeWidth = std::clamp(geWaterDropletParams->transitionFadeWidth, 0.0f, 1.0f);
    geWaterDropletParams->distortionIntensity = std::clamp(geWaterDropletParams->distortionIntensity, 0.0f, 1.0f);
    geWaterDropletParams->distortionThickness = std::clamp(geWaterDropletParams->distortionThickness, 0.0f, 1.0f);
    geWaterDropletParams->lightStrength = std::clamp(geWaterDropletParams->lightStrength, 0.0f, 1.0f);
    geWaterDropletParams->lightSoftness = std::clamp(geWaterDropletParams->lightSoftness, 0.0f, 1.0f);
    geWaterDropletParams->noiseScaleX = std::clamp(geWaterDropletParams->noiseScaleX, 0.0f, 10.0f);
    geWaterDropletParams->noiseScaleY = std::clamp(geWaterDropletParams->noiseScaleY, 0.0f, 10.0f);
    geWaterDropletParams->noiseStrengthX = std::clamp(geWaterDropletParams->noiseStrengthX, 0.0f, 10.0f);
    geWaterDropletParams->noiseStrengthY = std::clamp(geWaterDropletParams->noiseStrengthY, 0.0f, 10.0f);

    if (!effectFilter->WaterDropletTransition(topLayerPixelmap, geWaterDropletParams)) {
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

EffectErrorCode OH_Filter_GetEffectNativeBuffer(OH_Filter* filter, OH_NativeBuffer* dstNativeBuffer)
{
    if (!dstNativeBuffer || !filter) {
        return EFFECT_BAD_PARAMETER;
    }
    CastToFilter(filter)->RenderNativeBuffer(false, dstNativeBuffer);
    if (dstNativeBuffer == nullptr) {
        return EFFECT_BAD_PARAMETER;
    }
    return EFFECT_SUCCESS;
}
