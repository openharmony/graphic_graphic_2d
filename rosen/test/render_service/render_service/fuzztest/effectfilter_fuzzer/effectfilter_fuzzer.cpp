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
#include "image/pixelmap_native.h"

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
constexpr uint8_t TEST_CASE_COUNT = 18;
constexpr int32_t PIXEL_ALPHA_TYPE_OPAQUE = 2;
constexpr int32_t PIXEL_MAP_HEIGHT = 4;
constexpr int32_t PIXEL_MAP_WIDTH = 4;
constexpr int32_t PIXEL_FORMAT_RGBA_8888 = 4;
constexpr size_t PIXEL_MAP_DATA_LENGTH = 64;
constexpr int32_t INVALID_MASK_TYPE = 99;

enum TestCaseIndex {
    TEST_CASE_CREATE_EFFECT_NULL = 0,
    TEST_CASE_CREATE_EFFECT = 1,
    TEST_CASE_BRIGHTEN = 2,
    TEST_CASE_GRAY_SCALE = 3,
    TEST_CASE_SCALE = 4,
    TEST_CASE_INVERT = 5,
    TEST_CASE_SET_COLOR_MATRIX = 6,
    TEST_CASE_MAP_COLOR_BY_BRIGHTNESS = 7,
    TEST_CASE_GAMMA_CORRECTION = 8,
    TEST_CASE_MASK_TRANSITION = 9,
    TEST_CASE_WATER_DROPLET_TRANSITION = 10,
    TEST_CASE_WATER_GLASS = 11,
    TEST_CASE_REEDED_GLASS = 12,
    TEST_CASE_GET_EFFECT_PIXEL_MAP = 13,
    TEST_CASE_RELEASE_NULL = 14,
    TEST_CASE_BLUR = 15,
    TEST_CASE_BLUR_WITH_TILE_MODE = 16,
    TEST_CASE_BLUR_WITH_DIRECTION = 17
};

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

OH_PixelmapNative* CreatePixelMap()
{
    OH_Pixelmap_InitializationOptions *ops = nullptr;
    OH_PixelmapInitializationOptions_Create(&ops);
    OH_PixelmapInitializationOptions_SetAlphaType(ops, PIXEL_ALPHA_TYPE_OPAQUE);
    OH_PixelmapInitializationOptions_SetHeight(ops, PIXEL_MAP_HEIGHT);
    OH_PixelmapInitializationOptions_SetWidth(ops, PIXEL_MAP_WIDTH);
    OH_PixelmapInitializationOptions_SetPixelFormat(ops, PIXEL_FORMAT_RGBA_8888);
    uint8_t data[] = {
        255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255,
        255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255,
        255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255,
        255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255, 255, 255, 0, 255
    };
    size_t dataLength = PIXEL_MAP_DATA_LENGTH;
    OH_PixelmapNative *pixelMap = nullptr;
    OH_PixelmapNative_CreatePixelmap(data, dataLength, ops, &pixelMap);
    OH_PixelmapInitializationOptions_Release(ops);
    return pixelMap;
}

OH_Filter* CreateFilter()
{
    OH_PixelmapNative *pixelMap = CreatePixelMap();
    if (pixelMap == nullptr) {
        return nullptr;
    }
    OH_Filter *filter = nullptr;
    EffectErrorCode ret = OH_Filter_CreateEffect(pixelMap, &filter);
    OH_PixelmapNative_Release(pixelMap);
    if (ret != EFFECT_SUCCESS || filter == nullptr) {
        return nullptr;
    }
    return filter;
}

void ReleaseFilter(OH_Filter* filter)
{
    if (filter != nullptr) {
        OH_Filter_Release(filter);
    }
}

void DoCreateEffectNullTest()
{
    OH_Filter* filter = nullptr;
    OH_Filter_CreateEffect(nullptr, &filter);
    OH_Filter_CreateEffect(reinterpret_cast<OH_PixelmapNative*>(0x1), nullptr);

    OH_PixelmapNative* pixelMap = CreatePixelMap();
    if (pixelMap != nullptr) {
        OH_Filter_CreateEffect(pixelMap, &filter);
        if (filter != nullptr) {
            OH_Filter_Release(filter);
        }
        OH_PixelmapNative_Release(pixelMap);
    }
}

void DoCreateEffectTest()
{
    OH_Filter* filter = nullptr;
    EffectErrorCode ret = OH_Filter_CreateEffect(nullptr, &filter);
    if (ret == EFFECT_SUCCESS && filter != nullptr) {
        OH_Filter_Release(filter);
    }

    OH_PixelmapNative* pixelMap = CreatePixelMap();
    if (pixelMap != nullptr) {
        ret = OH_Filter_CreateEffect(pixelMap, &filter);
        if (ret == EFFECT_SUCCESS && filter != nullptr) {
            OH_Filter_Release(filter);
        }
        OH_PixelmapNative_Release(pixelMap);
    }
}

void DoBrightenTest()
{
    float brightness = GetData<float>();
    OH_Filter_Brighten(nullptr, brightness);

    OH_Filter* filter = CreateFilter();
    if (filter != nullptr) {
        OH_Filter_Brighten(filter, brightness);
        ReleaseFilter(filter);
    }
}

void DoGrayScaleTest()
{
    OH_Filter_GrayScale(nullptr);

    OH_Filter* filter = CreateFilter();
    if (filter != nullptr) {
        OH_Filter_GrayScale(filter);
        ReleaseFilter(filter);
    }
}

void DoScaleTest()
{
    float scaleX = GetData<float>();
    float scaleY = GetData<float>();
    OH_Filter_ScaleMode scaleMode = static_cast<OH_Filter_ScaleMode>(GetData<uint8_t>());
    OH_Filter_MipmapMode mipmapMode = static_cast<OH_Filter_MipmapMode>(GetData<uint8_t>());
    OH_Filter_Scale(nullptr, scaleX, scaleY, scaleMode, mipmapMode);

    OH_Filter* filter = CreateFilter();
    if (filter != nullptr) {
        OH_Filter_Scale(filter, scaleX, scaleY, scaleMode, mipmapMode);
        ReleaseFilter(filter);
    }
}

void DoInvertTest()
{
    OH_Filter_Invert(nullptr);

    OH_Filter* filter = CreateFilter();
    if (filter != nullptr) {
        OH_Filter_Invert(filter);
        ReleaseFilter(filter);
    }
}

void DoSetColorMatrixTest()
{
    OH_Filter_ColorMatrix matrix;
    for (int i = 0; i < COLOR_MATRIX_SIZE; i++) {
        matrix.val[i] = GetData<float>();
    }

    OH_Filter_SetColorMatrix(nullptr, &matrix);
    OH_Filter_SetColorMatrix(nullptr, nullptr);

    OH_Filter* filter = CreateFilter();
    if (filter != nullptr) {
        OH_Filter_SetColorMatrix(filter, &matrix);
        OH_Filter_SetColorMatrix(filter, nullptr);
        ReleaseFilter(filter);
    }
}

void DoMapColorByBrightnessTest()
{
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

    OH_Filter* filter = CreateFilter();
    if (filter != nullptr) {
        OH_Filter_MapColorByBrightness(filter, &params);
        OH_Filter_MapColorByBrightness(filter, nullptr);
        ReleaseFilter(filter);
    }
}

void DoGammaCorrectionTest()
{
    float gamma = GetData<float>();
    OH_Filter_GammaCorrection(nullptr, gamma);

    OH_Filter* filter = CreateFilter();
    if (filter != nullptr) {
        OH_Filter_GammaCorrection(filter, gamma);
        ReleaseFilter(filter);
    }
}

void BuildLinearMaskParams(OH_Filter_LinearGradientMask& linearMask,
    std::unique_ptr<OH_Filter_Vec2[]>& fractionStops, uint32_t fractionStopsLength)
{
    fractionStops = std::make_unique<OH_Filter_Vec2[]>(fractionStopsLength);
    for (uint32_t i = 0; i < fractionStopsLength; i++) {
        fractionStops[i].x = GetData<float>();
        fractionStops[i].y = GetData<float>();
    }

    linearMask = OH_Filter_LinearGradientMask {
        .startPosition = { GetData<float>(), GetData<float>() },
        .endPosition = { GetData<float>(), GetData<float>() },
        .fractionStops = fractionStops.get(),
        .fractionStopsLength = fractionStopsLength
    };
}

void BuildRadialMaskParams(OH_Filter_RadialGradientMask& radialMask,
    std::unique_ptr<OH_Filter_Vec2[]>& fractionStops, uint32_t fractionStopsLength)
{
    radialMask = OH_Filter_RadialGradientMask {
        .center = { GetData<float>(), GetData<float>() },
        .radiusX = GetData<float>(),
        .radiusY = GetData<float>(),
        .fractionStops = fractionStops.get(),
        .fractionStopsLength = fractionStopsLength
    };
}

void DoMaskTransitionTest()
{
    constexpr size_t baseSize = sizeof(float) * 4;
    constexpr size_t vec2Size = sizeof(float) * 2;
    if (!HasEnoughData(sizeof(uint8_t) + baseSize + sizeof(bool))) {
        return;
    }

    uint32_t fractionStopsLength = GetData<uint8_t>();
    fractionStopsLength =
        fractionStopsLength < MIN_FRACTION_STOPS_LENGTH ? MIN_FRACTION_STOPS_LENGTH : fractionStopsLength;
    fractionStopsLength =
        fractionStopsLength > MAX_FRACTION_STOPS_LENGTH ? MAX_FRACTION_STOPS_LENGTH : fractionStopsLength;

    if (!HasEnoughData(fractionStopsLength * vec2Size + sizeof(float) + sizeof(bool))) {
        return;
    }

    std::unique_ptr<OH_Filter_Vec2[]> fractionStops;
    OH_Filter_LinearGradientMask linearMask;
    BuildLinearMaskParams(linearMask, fractionStops, fractionStopsLength);

    float factor = GetData<float>();
    bool inverse = GetData<bool>();

    OH_Filter_MaskTransition(nullptr, nullptr, &linearMask,
        EffectMaskType::LINEAR_GRADIENT_MASK, factor, inverse);

    OH_Filter* filter = CreateFilter();
    OH_PixelmapNative* transitionImage = CreatePixelMap();
    if (filter != nullptr && transitionImage != nullptr) {
        OH_Filter_MaskTransition(filter, transitionImage, &linearMask,
            EffectMaskType::LINEAR_GRADIENT_MASK, factor, inverse);
        OH_Filter_MaskTransition(filter, nullptr, &linearMask,
            EffectMaskType::LINEAR_GRADIENT_MASK, factor, inverse);

        OH_Filter_RadialGradientMask radialMask;
        BuildRadialMaskParams(radialMask, fractionStops, fractionStopsLength);
        OH_Filter_MaskTransition(filter, transitionImage, &radialMask,
            EffectMaskType::RADIAL_GRADIENT_MASK, factor, inverse);

        OH_Filter_MaskTransition(filter, transitionImage, &linearMask,
            static_cast<EffectMaskType>(INVALID_MASK_TYPE), factor, inverse);
    }
    ReleaseFilter(filter);
    if (transitionImage != nullptr) {
        OH_PixelmapNative_Release(transitionImage);
    }
}

void DoWaterDropletTransitionTest()
{
    if (!HasEnoughData(sizeof(float) * WATER_DROPLET_PARAMS_COUNT + sizeof(bool))) {
        return;
    }

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
    OH_Filter_WaterDropletTransition(nullptr, nullptr, &params, inverse);

    OH_Filter* filter = CreateFilter();
    OH_PixelmapNative* transitionImage = CreatePixelMap();
    if (filter != nullptr && transitionImage != nullptr) {
        OH_Filter_WaterDropletTransition(filter, transitionImage, &params, inverse);
        OH_Filter_WaterDropletTransition(filter, nullptr, &params, inverse);
    }
    ReleaseFilter(filter);
    if (transitionImage != nullptr) {
        OH_PixelmapNative_Release(transitionImage);
    }
}

void DoWaterGlassTest()
{
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

    OH_Filter* filter = CreateFilter();
    if (filter != nullptr) {
        OH_Filter_WaterGlass(filter, &params);
        OH_Filter_WaterGlass(filter, nullptr);
        ReleaseFilter(filter);
    }
}

void DoReededGlassTest()
{
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

    OH_Filter* filter = CreateFilter();
    if (filter != nullptr) {
        OH_Filter_ReededGlass(filter, &params);
        OH_Filter_ReededGlass(filter, nullptr);
        ReleaseFilter(filter);
    }
}

void DoGetEffectPixelMapTest()
{
    OH_PixelmapNative* resultPixelmap = nullptr;
    OH_Filter_GetEffectPixelMap(nullptr, &resultPixelmap);
    OH_Filter_GetEffectPixelMap(nullptr, nullptr);

    OH_Filter* filter = CreateFilter();
    if (filter != nullptr) {
        OH_Filter_GetEffectPixelMap(filter, &resultPixelmap);
        OH_Filter_GetEffectPixelMap(filter, nullptr);
        ReleaseFilter(filter);
    }
    if (resultPixelmap != nullptr) {
        OH_PixelmapNative_Release(resultPixelmap);
    }
}

void DoReleaseNullTest()
{
    OH_Filter_Release(nullptr);
}

void DoBlurTest()
{
    float radius = GetData<float>();
    OH_Filter_Blur(nullptr, radius);

    OH_Filter* filter = CreateFilter();
    if (filter != nullptr) {
        OH_Filter_Blur(filter, radius);
        ReleaseFilter(filter);
    }
}

void DoBlurWithTileModeTest()
{
    float radius = GetData<float>();
    EffectTileMode tileMode = static_cast<EffectTileMode>(GetData<uint8_t>());
    OH_Filter_BlurWithTileMode(nullptr, radius, tileMode);

    OH_Filter* filter = CreateFilter();
    if (filter != nullptr) {
        OH_Filter_BlurWithTileMode(filter, radius, tileMode);
        ReleaseFilter(filter);
    }
}

void DoBlurWithDirectionTest()
{
    float radius = GetData<float>();
    float direction = GetData<float>();
    EffectTileMode tileMode = static_cast<EffectTileMode>(GetData<uint8_t>());
    OH_Filter_BlurWithDirection(nullptr, radius, direction, tileMode);

    OH_Filter* filter = CreateFilter();
    if (filter != nullptr) {
        OH_Filter_BlurWithDirection(filter, radius, direction, tileMode);
        ReleaseFilter(filter);
    }
}

bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < FUZZ_DATA_MIN_SIZE) {
        return false;
    }

    DATA = data;
    g_size = size;
    g_pos = 0;

    uint8_t choice = GetData<uint8_t>() % TEST_CASE_COUNT;
    switch (choice) {
        case TEST_CASE_CREATE_EFFECT_NULL:
            DoCreateEffectNullTest();
            break;
        case TEST_CASE_CREATE_EFFECT:
            DoCreateEffectTest();
            break;
        case TEST_CASE_BRIGHTEN:
            DoBrightenTest();
            break;
        case TEST_CASE_GRAY_SCALE:
            DoGrayScaleTest();
            break;
        case TEST_CASE_SCALE:
            DoScaleTest();
            break;
        case TEST_CASE_INVERT:
            DoInvertTest();
            break;
        case TEST_CASE_SET_COLOR_MATRIX:
            DoSetColorMatrixTest();
            break;
        case TEST_CASE_MAP_COLOR_BY_BRIGHTNESS:
            DoMapColorByBrightnessTest();
            break;
        case TEST_CASE_GAMMA_CORRECTION:
            DoGammaCorrectionTest();
            break;
        case TEST_CASE_MASK_TRANSITION:
            DoMaskTransitionTest();
            break;
        case TEST_CASE_WATER_DROPLET_TRANSITION:
            DoWaterDropletTransitionTest();
            break;
        case TEST_CASE_WATER_GLASS:
            DoWaterGlassTest();
            break;
        case TEST_CASE_REEDED_GLASS:
            DoReededGlassTest();
            break;
        case TEST_CASE_GET_EFFECT_PIXEL_MAP:
            DoGetEffectPixelMapTest();
            break;
        case TEST_CASE_RELEASE_NULL:
            DoReleaseNullTest();
            break;
        case TEST_CASE_BLUR:
            DoBlurTest();
            break;
        case TEST_CASE_BLUR_WITH_TILE_MODE:
            DoBlurWithTileModeTest();
            break;
        case TEST_CASE_BLUR_WITH_DIRECTION:
            DoBlurWithDirectionTest();
            break;
        default:
            break;
    }
    return true;
}

}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}