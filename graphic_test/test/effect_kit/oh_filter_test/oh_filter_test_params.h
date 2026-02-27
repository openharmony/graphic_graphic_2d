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

#ifndef OH_FILTER_TEST_PARAMS_H
#define OH_FILTER_TEST_PARAMS_H

#include <array>
#include <vector>
#include "effect_ndk/include/effect_types.h"

namespace OHOS::Rosen {

enum class OHFilterTestDataGroupParamsType {
    INVALID_DATA_MIN,
    VALID_DATA1,
    VALID_DATA2,
    VALID_DATA3,
    INVALID_AND_VALID_DATA,
    INVALID_DATA_MAX,
    COUNT
};

constexpr int WATER_DROPLET_PARAMS_COUNT = 10;
const std::vector<std::array<float, WATER_DROPLET_PARAMS_COUNT>> waterDropletParams = {
    {0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
    {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 10.0f, 10.0f, 10.0f},
    {0.3f, 0.2f, 0.2f, 0.2f, 0.2f, 0.2f, 0.2f, 0.2f, 1.2f, 0.2f},
    {0.5f, 0.4f, 0.4f, 0.4f, 0.4f, 0.4f, 0.4f, 1.4f, 0.4f, 0.4f},
    {0.5f, 0.6f, 0.6f, 0.6f, 0.6f, 0.6f, 0.6f, 0.6f, 0.6f, 1.6f},
    {0.5f, 0.8f, 0.8f, 0.8f, 0.8f, 0.8f, 0.8f, 0.8f, 0.8f, 0.8f}
};

const std::vector<float> waterDropletProgressParams = {
    0.0f, 10.0f, 0.2f, 0.4f, 0.6f, 1.8f
};

constexpr int WATER_GLASS_PARAMS_COUNT = 23;
const std::vector<std::array<float, WATER_GLASS_PARAMS_COUNT>> waterGlassParams = {
    {-0.5f, -0.5f, -0.5f, -0.5f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,
     -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,
     -1.0f, -1.0f, -1.0f},
    {0.5f, 0.5f, 0.3f, 0.3f, 20.0f, 20.0f, 1.0f, 1.0f, 0.5f, 0.5f,
     0.5f, 0.5f, 0.5f, 0.2f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
     0.5f, 1.0f},
    {0.5f, 0.5f, 0.5f, 0.5f, 40.0f, 40.0f, 3.0f, 3.0f, 2.0f, 1.0f,
     1.0f, 1.0f, 0.7f, 0.5f, 1.0f, 0.5f, 0.3f, 0.3f, 0.3f, 0.3f,
     0.5f, 2.0f},
    {0.5f, 0.5f, 0.8f, 0.8f, 60.0f, 60.0f, 5.0f, 5.0f, 3.0f, 1.5f,
     2.0f, 2.0f, 0.9f, 0.8f, 1.5f, 0.5f, 0.2f, 0.2f, 0.2f, 0.2f,
     0.5f, 3.0f},
    {0.5f, 0.5f, 1.5f, 1.5f, 100.0f, 100.0f, 10.0f, 10.0f, 5.0f, 2.0f,
     5.0f, 10.0f, 1.0f, 1.0f, 2.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
     0.0f, 5.0f},
    {0.5f, 0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
     0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f,
     0.0f, 10.0f}
};

constexpr int REEDED_GLASS_PARAMS_COUNT = 19;
const std::vector<std::array<float, REEDED_GLASS_PARAMS_COUNT>> reededGlassParams = {
    {-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,
     -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f},
    {0.5f, 0.3f, 0.2f, 0.5f, 10.0f, 1.0f, 0.5f, 0.5f, 1.0f, 1.0f,
     1.0f, 1.0f, 0.3f, 0.7f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f},
    {0.1f, 0.1f, 0.1f, 0.2f, 5.0f, 1.0f, 0.3f, 0.3f, 1.0f, 1.0f,
     1.0f, 1.0f, 0.2f, 0.8f, 0.3f, 0.3f, 0.7f, 0.3f, 0.3f},
    {0.9f, 0.8f, 0.5f, 0.8f, 20.0f, 1.5f, 0.8f, 0.8f, 1.0f, 1.0f,
     1.0f, 1.0f, 0.3f, 0.7f, 0.8f, 0.8f, 0.2f, 0.8f, 0.8f},
    {1.0f, 1.0f, 1.0f, 1.0f, 100.0f, 2.0f, 1.0f, 1.0f, 1.0f, 1.0f,
     1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
    {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
     0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}
};

const std::vector<float> gammaCorrectionParams = {
    -1.0f, 0.5f, 1.0f, 1.8f, 5.0f, 0.0f
};

struct MapColorByBrightnessTestData {
    std::vector<OH_Filter_Color> colors;
    std::vector<float> positions;
};

const std::vector<MapColorByBrightnessTestData> mapColorByBrightnessParams1 = {
    {{}, {}},
    {{{0.96f, 0.7f, 0.74f, 1.0f}}, {0.5f}},
    {{{0.96f, 0.7f, 0.74f, 1.0f}, {0.88f, 0.97f, 0.58f, 1.0f}}, {0.2f, 0.7f}},
    {{{0.96f, 0.7f, 0.74f, 1.0f}, {0.88f, 0.97f, 0.58f, 1.0f}, {0.53f, 0.9f, 0.59f, 1.0f}},
     {0.5f, 0.2f, 0.8f}},
    {{{0.96f, 0.7f, 0.74f, 1.0f}, {0.88f, 0.97f, 0.58f, 1.0f}, {0.53f, 0.9f, 0.59f, 1.0f},
      {0.87f, 0.86f, 0.87f, 1.0f}},
     {0.1f, 0.2f, 0.6f, 0.9f}},
    {{{0.96f, 0.7f, 0.74f, 1.0f}, {0.88f, 0.97f, 0.58f, 1.0f}, {0.53f, 0.9f, 0.59f, 1.0f},
      {0.87f, 0.86f, 0.87f, 1.0f}, {0.99f, 0.22f, 0.87f, 1.0f}},
     {0.1f, 0.4f, 0.6f, 0.8f, 0.95f}}
};

const std::vector<MapColorByBrightnessTestData> mapColorByBrightnessParams2 = {
    {{{0.96f, 0.7f, 0.74f, 1.0f}, {0.88f, 0.97f, 0.58f, 1.0f}, {0.53f, 0.9f, 0.59f, 1.0f},
      {0.87f, 0.86f, 0.87f, 1.0f}, {0.09f, 0.22f, 0.87f, 1.0f}, {0.99f, 0.0f, 0.0f, 1.0f}},
     {0.1f, 0.4f, 0.6f, 0.8f, 0.95f, 1.0f}},
    {{{0.96f, 0.7f, 0.74f, 1.0f}, {0.88f, 0.97f, 0.58f, 1.0f}, {0.53f, 0.9f, 0.59f, 1.0f}},
     {0.1f, 0.45f, 0.58f}},
    {{{0.96f, 0.7f, 0.74f, 1.0f}, {0.88f, 0.97f, 0.58f, 1.0f}, {0.53f, 0.9f, 0.59f, 1.0f}},
     {0.75f, 0.35f, 0.58f}},
    {{{0.96f, 0.7f, 0.74f, 1.0f}, {0.88f, 0.97f, 0.58f, 1.0f}, {0.53f, 0.9f, 0.59f, 1.0f}},
     {0.35f, 0.1f, 0.85f}},
    {{{0.639f, 0.77f, 0.95f, 1.0f}, {0.69f, 0.67f, 0.76f, 1.0f}, {0.97f, 0.72f, 0.23f, 1.0f},
      {0.90f, 0.34f, 0.003f, 1.0f}, {0.97f, 0.82f, 0.44f, 1.0f}},
     {0.106f, 0.49f, 0.70f, 0.84f, 1.0f}},
    {{{0.639f, 0.77f, 0.95f, 1.0f}, {0.69f, 0.67f, 0.76f, 1.0f}, {0.97f, 0.72f, 0.23f, 1.0f},
      {0.90f, 0.34f, 0.003f, 1.0f}},
     {0.106f, 0.70f, 0.49f, 0.84f}}
};

struct BlurWithDirectionTestData {
    float radius;
    float effectDirection;
    EffectTileMode tileMode;
};

const std::vector<BlurWithDirectionTestData> blurWithDirectionParams1 = {
    {50.0f, 0.0f, EffectTileMode::CLAMP},
    {30.0f, 180.0f, EffectTileMode::CLAMP},
    {50.0f, 90.0f, EffectTileMode::CLAMP},
    {-10.0f, 20.0f, EffectTileMode::CLAMP},
    {500.0f, 0.0f, EffectTileMode::CLAMP},
    {100.0f, 90.0f, EffectTileMode::CLAMP}
};

const std::vector<BlurWithDirectionTestData> blurWithDirectionParams2 = {
    {50.0f, 45.0f, EffectTileMode::CLAMP},
    {50.0f, 75.0f, EffectTileMode::CLAMP},
    {50.0f, 135.0f, EffectTileMode::CLAMP},
    {100.0f, 200.0f, EffectTileMode::REPEAT},
    {200.0f, -10.0f, EffectTileMode::REPEAT},
    {300.0f, 30.0f, EffectTileMode::REPEAT}
};

struct LinearGradientMaskTestData {
    OH_Filter_Vec2 startPosition;
    OH_Filter_Vec2 endPosition;
    std::vector<OH_Filter_Vec2> fractionStops;
    float factor;
    bool inverse;
};

const std::vector<LinearGradientMaskTestData> linearGradientMaskParams = {
    {{0.0f, 0.0f}, {1.0f, 0.0f},
     {{0.0f, 0.0f}, {0.0f, 0.4f}, {1.0f, 0.6f}, {1.0f, 1.0f}}, 1.0f, false},
    {{0.0f, 0.0f}, {0.0f, 1.0f},
     {{0.0f, 0.0f}, {0.0f, 0.4f}, {1.0f, 0.6f}, {1.0f, 1.0f}}, 0.0f, false},
    {{0.5f, 0.0f}, {0.0f, 0.5f},
     {{0.0f, 0.0f}, {0.0f, 0.4f}, {1.0f, 0.6f}, {1.0f, 1.0f}}, 0.5f, false},
    {{0.0f, 0.0f}, {0.0f, 0.5f},
     {{0.0f, 0.0f}, {0.0f, 0.4f}, {1.0f, 0.6f}, {1.0f, 1.0f}}, 1.0f, true},
    {{1.0f, 1.0f}, {1.0f, 1.0f},
     {{0.0f, 0.0f}, {1.0f, 1.0f}}, 1.0f, false},
    {{1.0f, 0.0f}, {0.0f, 0.0f},
     {{0.0f, 0.0f}, {0.0f, 0.1f}, {1.0f, 0.2f}, {1.0f, 0.3f},
      {0.0f, 0.4f}, {0.0f, 0.45f}, {1.0f, 0.5f}, {1.0f, 0.6f},
      {0.0f, 0.7f}, {0.0f, 0.8f}, {1.0f, 0.9f}, {1.0f, 1.0f}}, 1.0f, false}
};

struct RadialGradientMaskTestData {
    OH_Filter_Vec2 center;
    float radiusX;
    float radiusY;
    std::vector<OH_Filter_Vec2> fractionStops;
    float factor;
    bool inverse;
};

const std::vector<RadialGradientMaskTestData> radialGradientMaskParams = {
    {{0.5f, 0.5f}, 1.0f, 1.0f,
     {{0.0f, 0.0f}, {0.0f, 0.4f}, {1.0f, 0.6f}, {1.0f, 1.0f}}, 1.0f, false},
    {{0.0f, 0.0f}, 0.5f, 0.5f,
     {{0.0f, 0.0f}, {0.0f, 0.4f}, {1.0f, 0.6f}, {1.0f, 1.0f}}, 0.0f, false},
    {{0.5f, 0.5f}, 1.0f, 0.5f,
     {{0.0f, 0.0f}, {0.0f, 0.4f}, {1.0f, 0.6f}, {1.0f, 1.0f}}, 0.5f, false},
    {{1.0f, 1.0f}, 0.5f, 0.5f,
     {{0.0f, 0.0f}, {0.0f, 0.4f}, {1.0f, 0.6f}, {1.0f, 1.0f}}, 1.0f, true},
    {{0.5f, 0.5f}, 1.0f, 0.5f,
     {{0.0f, 0.0f}, {1.0f, 1.0f}}, 1.0f, false},
    {{0.5f, 0.5f}, 2.0f, 2.0f,
     {{0.0f, 0.0f}, {0.0f, 0.1f}, {1.0f, 0.2f}, {1.0f, 0.3f},
      {0.0f, 0.4f}, {0.0f, 0.45f}, {1.0f, 0.5f}, {1.0f, 0.6f},
      {0.0f, 0.7f}, {0.0f, 0.8f}, {1.0f, 0.9f}, {1.0f, 1.0f}}, 1.0f, false}
};

constexpr int COLOR_MATRIX_PARAMS_COUNT = 20;
const std::vector<std::array<float, COLOR_MATRIX_PARAMS_COUNT>> setColorMatrixParams = {
    {-1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,
     -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f},
    {-1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
     0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f},
    {0.299f, 0.587f, 0.114f, 0.0f, 0.0f, 0.299f, 0.587f, 0.114f, 0.0f, 0.0f,
     0.299f, 0.587f, 0.114f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f},
    {0.393f, 0.769f, 0.189f, 0.0f, 0.0f, 0.349f, 0.686f, 0.168f, 0.0f, 0.0f,
     0.272f, 0.534f, 0.131f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f},
    {2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 2.0f, 0.0f, 0.0f, 0.0f,
     0.0f, 0.0f, 2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f},
    {999.0f, 999.0f, 999.0f, 999.0f, 999.0f, 999.0f, 999.0f, 999.0f, 999.0f, 999.0f,
     999.0f, 999.0f, 999.0f, 999.0f, 999.0f, 999.0f, 999.0f, 999.0f, 999.0f, 999.0f}
};

}  // namespace OHOS::Rosen

#endif // OH_FILTER_TEST_PARAMS_H
