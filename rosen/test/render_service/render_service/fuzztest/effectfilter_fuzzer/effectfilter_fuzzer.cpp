/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
 
#include "effectfilter_fuzzer.h"
 
#include <securec.h>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <string>
#include <vector>
 
#include "effect_filter.h"
#include "effect_types.h"
 
#include <cstdint>
#include <cstring>
#include <memory>
 
namespace {
size_t g_size = 0;
size_t g_pos = 0;
const uint8_t* DATA = nullptr;
 
constexpr size_t FUZZ_DATA_MIN_SIZE = 10;
constexpr int32_t COLOR_MATRIX_SIZE = 20;
constexpr uint8_t MAX_COLORS_NUM = 6;
constexpr uint32_t MIN_FRACTION_STOPS_LENGTH = 2;
constexpr uint32_t MAX_FRACTION_STOPS_LENGTH = 12;
constexpr size_t WATER_DROPLET_PARAMS_COUNT = 13;
constexpr size_t WATER_GLASS_PARAMS_COUNT = 26;
constexpr size_t REEDED_GLASS_PARAMS_COUNT = 19;
 
template<class T>
T GetData()
{
    T object = T();
    size_t objectSize = sizeof(object);
    if (DATA == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, DATA + g_pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    return object;
}
 
bool HasEnoughData(size_t bytesNeeded)
{
    return DATA != nullptr && bytesNeeded <= g_size - g_pos;
}
 
void DoCreateEffectNullTest()
{
    OH_Filter* filter = nullptr;
 
    OH_Filter_CreateEffect(nullptr, &filter);
    OH_Filter_CreateEffect(reinterpret_cast<OH_PixelmapNative*>(0x1), nullptr);
}
 
void DoCreateEffectTest()
{
    (void)GetData<uintptr_t>();
    OH_Filter* filter = nullptr;
    EffectErrorCode ret = OH_Filter_CreateEffect(nullptr, &filter);
    if (ret == EFFECT_SUCCESS && filter != nullptr) {
        OH_Filter_Release(filter);
    }
}
 
void DoBrightenTest()
{
    (void)GetData<uintptr_t>();
    float brightness = GetData<float>();
    OH_Filter_Brighten(nullptr, brightness);
}
 
void DoGrayScaleTest()
{
    (void)GetData<uintptr_t>();
    OH_Filter_GrayScale(nullptr);
}
 
void DoScaleTest()
{
    (void)GetData<uintptr_t>();
    float scaleX = GetData<float>();
    float scaleY = GetData<float>();
    OH_Filter_ScaleMode scaleMode = static_cast<OH_Filter_ScaleMode>(GetData<uint8_t>());
    OH_Filter_MipmapMode mipmapMode = static_cast<OH_Filter_MipmapMode>(GetData<uint8_t>());
    OH_Filter_Scale(nullptr, scaleX, scaleY, scaleMode, mipmapMode);
}
 
void DoInvertTest()
{
    (void)GetData<uintptr_t>();
    OH_Filter_Invert(nullptr);
}
 
void DoSetColorMatrixTest()
{
    (void)GetData<uintptr_t>();
    OH_Filter_ColorMatrix matrix;
    for (int i = 0; i < COLOR_MATRIX_SIZE; i++) {
        matrix.val[i] = GetData<float>();
    }
 
    OH_Filter_SetColorMatrix(nullptr, &matrix);
    OH_Filter_SetColorMatrix(nullptr, nullptr);
}
 
void DoMapColorByBrightnessTest()
{
    (void)GetData<uintptr_t>();
    if (!HasEnoughData(sizeof(uint8_t))) {
        return;
    }
 
    uint8_t colorsNum = GetData<uint8_t>();
    colorsNum = colorsNum > 0 ? colorsNum : 1;
    colorsNum = colorsNum > MAX_COLORS_NUM ? MAX_COLORS_NUM : colorsNum;
 
    if (!HasEnoughData(colorsNum * (sizeof(OH_Filter_Color) + sizeof(float)))) {
        return;
    }
 
    std::vector<OH_Filter_Color> colors(colorsNum);
    std::vector<float> positions(colorsNum);
 
    for (uint8_t i = 0; i < colorsNum; i++) {
        colors[i].red = GetData<float>();
        colors[i].green = GetData<float>();
        colors[i].blue = GetData<float>();
        colors[i].alpha = GetData<float>();
        positions[i] = GetData<float>();
    }
 
    OH_Filter_MapColorByBrightnessParams params {
        .colors = colors.data(),
        .positions = positions.data(),
        .colorsNum = colorsNum
    };
 
    OH_Filter_MapColorByBrightness(nullptr, &params);
    OH_Filter_MapColorByBrightness(nullptr, nullptr);
}
 
void DoGammaCorrectionTest()
{
    (void)GetData<uintptr_t>();
    float gamma = GetData<float>();
    OH_Filter_GammaCorrection(nullptr, gamma);
}
 
void DoMaskTransitionTest()
{
    (void)GetData<uintptr_t>();
    constexpr size_t baseSize = sizeof(float) * 4;
    constexpr size_t vec2Size = sizeof(float) * 2;
    if (!HasEnoughData(sizeof(uintptr_t) + sizeof(uint8_t) + baseSize + sizeof(bool))) {
        return;
    }

    OH_PixelmapNative* transitionImage = nullptr;
    uint32_t fractionStopsLength = GetData<uint8_t>();
    fractionStopsLength =
        fractionStopsLength < MIN_FRACTION_STOPS_LENGTH ? MIN_FRACTION_STOPS_LENGTH : fractionStopsLength;
    fractionStopsLength =
        fractionStopsLength > MAX_FRACTION_STOPS_LENGTH ? MAX_FRACTION_STOPS_LENGTH : fractionStopsLength;

    if (!HasEnoughData(fractionStopsLength * vec2Size + sizeof(float) + sizeof(bool))) {
        return;
    }
 
    auto fractionStops = std::make_unique<OH_Filter_Vec2[]>(fractionStopsLength);
    for (uint32_t i = 0; i < fractionStopsLength; i++) {
        fractionStops[i].x = GetData<float>();
        fractionStops[i].y = GetData<float>();
    }
 
    OH_Filter_LinearGradientMask linearMask {
        .startPosition = { GetData<float>(), GetData<float>() },
        .endPosition = { GetData<float>(), GetData<float>() },
        .fractionStops = fractionStops.get(),
        .fractionStopsLength = fractionStopsLength
    };
 
    float factor = GetData<float>();
    bool inverse = GetData<bool>();
 
    OH_Filter_MaskTransition(nullptr, transitionImage, &linearMask,
        EffectMaskType::LINEAR_GRADIENT_MASK, factor, inverse);
}
 
void DoWaterDropletTransitionTest()
{
    (void)GetData<uintptr_t>();
    if (!HasEnoughData(sizeof(uintptr_t) + sizeof(float) * WATER_DROPLET_PARAMS_COUNT + sizeof(bool))) {
        return;
    }
 
    OH_PixelmapNative* transitionImage = nullptr;
    OH_Filter_WaterDropletParams params {
        .position = { GetData<float>(), GetData<float>() },
        .radius = GetData<float>(),
        .transitionFadeWidth = GetData<float>(),
        .distortionIntensity = GetData<float>(),
        .distortionThickness = GetData<float>(),
        .lightStrength = GetData<float>(),
        .lightSoftness = GetData<float>(),
        .noiseScaleX = GetData<float>(),
        .noiseScaleY = GetData<float>(),
        .noiseStrengthX = GetData<float>(),
        .noiseStrengthY = GetData<float>(),
        .progress = GetData<float>()
    };
 
    bool inverse = GetData<bool>();
    OH_Filter_WaterDropletTransition(nullptr, transitionImage, &params, inverse);
}
 
void DoWaterGlassTest()
{
    (void)GetData<uintptr_t>();
    if (!HasEnoughData(sizeof(float) * WATER_GLASS_PARAMS_COUNT)) {
        return;
    }
 
    OH_Filter_WaterGlassDataParams params {
        .speed = GetData<float>(),
        .distortSpeed = GetData<float>(),
        .refractionSpeed = { GetData<float>(), GetData<float>() },
        .progress = GetData<float>(),
        .shakingDirection1 = { GetData<float>(), GetData<float>() },
        .shakingDirection2 = { GetData<float>(), GetData<float>() },
        .waveDensityXY = { GetData<float>(), GetData<float>() },
        .waveStrength = GetData<float>(),
        .waveRefraction = GetData<float>(),
        .waveSpecular = GetData<float>(),
        .waveFrequency = GetData<float>(),
        .waveShapeDistortion = GetData<float>(),
        .waveDistortionAngle = GetData<float>(),
        .rippleXWave = GetData<float>(),
        .rippleYWave = GetData<float>(),
        .borderRadius = GetData<float>(),
        .borderThickness = GetData<float>(),
        .waveInnerMaskXY = { GetData<float>(), GetData<float>() },
        .waveInnerMaskRadius = GetData<float>(),
        .waveInnerMaskSmoothness = GetData<float>(),
        .waveOuterMaskPadding = GetData<float>(),
        .waveSpecularPower = GetData<float>(),
        .refractionDetailDark = GetData<float>(),
        .refractionDetailWhite = GetData<float>(),
        .detailStrength = GetData<float>()
    };
 
    OH_Filter_WaterGlass(nullptr, &params);
    OH_Filter_WaterGlass(nullptr, nullptr);
}
 
void DoReededGlassTest()
{
    (void)GetData<uintptr_t>();
    if (!HasEnoughData(sizeof(float) * REEDED_GLASS_PARAMS_COUNT + sizeof(uint8_t))) {
        return;
    }
 
    OH_Filter_ReededGlassDataParams params {
        .refractionFactor = GetData<float>(),
        .horizontalPatternNumber = GetData<uint8_t>(),
        .gridLightStrength = GetData<float>(),
        .gridLightPositionStart = GetData<float>(),
        .gridLightPositionEnd = GetData<float>(),
        .gridShadowStrength = GetData<float>(),
        .gridShadowPositionStart = GetData<float>(),
        .gridShadowPositionEnd = GetData<float>(),
        .portalLightSize = { GetData<float>(), GetData<float>() },
        .portalLightTilt = { GetData<float>(), GetData<float>() },
        .portalLightPosition = { GetData<float>(), GetData<float>() },
        .portalLightDisperseAttenuation = GetData<float>(),
        .portalLightDisperse = GetData<float>(),
        .portalLightSmoothBorder = GetData<float>(),
        .portalLightShadowBorder = GetData<float>(),
        .portalLightShadowPositionShift = GetData<float>(),
        .portalLightStrength = GetData<float>()
    };
 
    OH_Filter_ReededGlass(nullptr, &params);
    OH_Filter_ReededGlass(nullptr, nullptr);
}
 
void DoGetEffectPixelMapTest()
{
    (void)GetData<uintptr_t>();
    OH_PixelmapNative* resultPixelmap = nullptr;
    OH_Filter_GetEffectPixelMap(nullptr, &resultPixelmap);
    OH_Filter_GetEffectPixelMap(nullptr, nullptr);
}
 
void DoReleaseNullTest()
{
    OH_Filter_Release(nullptr);
}
 
bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < FUZZ_DATA_MIN_SIZE) {
        return false;
    }
 
    DATA = data;
    g_size = size;
    g_pos = 0;
 
    DoCreateEffectNullTest();
    DoCreateEffectTest();
    DoBrightenTest();
    DoGrayScaleTest();
    DoScaleTest();
    DoInvertTest();
    DoSetColorMatrixTest();
    DoMapColorByBrightnessTest();
    DoGammaCorrectionTest();
    DoMaskTransitionTest();
    DoWaterDropletTransitionTest();
    DoWaterGlassTest();
    DoReededGlassTest();
    DoGetEffectPixelMapTest();
    DoReleaseNullTest();
    return true;
}
 
}
 
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}