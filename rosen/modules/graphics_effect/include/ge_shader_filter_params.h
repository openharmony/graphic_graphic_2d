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
#ifndef GRAPHICS_EFFECT_GE_SHADER_FILTER_PARAMS_H
#define GRAPHICS_EFFECT_GE_SHADER_FILTER_PARAMS_H

#include <memory>
#include <vector>
#include <utility>

#include "utils/matrix.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

constexpr char GE_FILTER_AI_BAR[] = "AIBAR";
constexpr char GE_FILTER_AI_BAR_LOW[] = "AIBAR_LOW";
constexpr char GE_FILTER_AI_BAR_HIGH[] = "AIBAR_HIGH";
constexpr char GE_FILTER_AI_BAR_THRESHOLD[] = "AIBAR_THRESHOLD";
constexpr char GE_FILTER_AI_BAR_OPACITY[] = "AIBAR_OPACITY";
constexpr char GE_FILTER_AI_BAR_SATURATION[] = "AIBAR_SATURATION";
struct GEAIBarShaderFilterParams {
    float aiBarLow;
    float aiBarHigh;
    float aiBarThreshold;
    float aiBarOpacity;
    float aiBarSaturation;
};

constexpr char GE_FILTER_WATER_RIPPLE[] = "WATER_RIPPLE";
constexpr char GE_FILTER_WATER_RIPPLE_PROGRESS[] = "PROGRESS";
constexpr char GE_FILTER_WATER_RIPPLE_WAVE_NUM[] = "WAVE_NUM";
constexpr char GE_FILTER_WATER_RIPPLE_RIPPLE_CENTER_X[] = "RIPPLE_CENTER_X";
constexpr char GE_FILTER_WATER_RIPPLE_RIPPLE_CENTER_Y[] = "RIPPLE_CENTER_Y";
constexpr char GE_FILTER_WATER_RIPPLE_RIPPLE_MODE[] = "RIPPLE_MODE";
struct GEWaterRippleFilterParams {
    float progress = 0.0f;
    uint32_t waveCount = 2;
    float rippleCenterX = 0.5f;
    float rippleCenterY = 0.7f;
    uint32_t rippleMode = 1;
};

constexpr char GE_FILTER_GREY[] = "GREY";
constexpr char GE_FILTER_GREY_COEF_1[] = "GREY_COEF_1";
constexpr char GE_FILTER_GREY_COEF_2[] = "GREY_COEF_2";
struct GEGreyShaderFilterParams {
    float greyCoef1;
    float greyCoef2;
};

constexpr char GE_FILTER_KAWASE_BLUR[] = "KAWASE_BLUR";
constexpr char GE_FILTER_KAWASE_BLUR_RADIUS[] = "KAWASE_BLUR_RADIUS";
struct GEKawaseBlurShaderFilterParams {
    int radius;
};

constexpr char GE_FILTER_LINEAR_GRADIENT_BLUR[] = "LINEAR_GRADIENT_BLUR";
constexpr char GE_FILTER_LINEAR_GRADIENT_BLUR_DIRECTION[] = "DIRECTION";
constexpr char GE_FILTER_LINEAR_GRADIENT_BLUR_IS_OFF_SCREEN[] = "ISOFFSCREEN";
constexpr char GE_FILTER_LINEAR_GRADIENT_BLUR_CANVAS_MAT[] = "CANVASMAT";
constexpr char GE_FILTER_LINEAR_GRADIENT_BLUR_FRACTION_STOPS[] = "FRACTIONSTOPS";
constexpr char GE_FILTER_LINEAR_GRADIENT_BLUR_RADIUS[] = "BLURRADIUS";
constexpr char GE_FILTER_LINEAR_GRADIENT_BLUR_GEO_WIDTH[] = "GEOWIDTH";
constexpr char GE_FILTER_LINEAR_GRADIENT_BLUR_GEO_HEIGHT[] = "GEOHEIGHT";
constexpr char GE_FILTER_LINEAR_GRADIENT_BLUR_TRAN_X[] = "TRANX";
constexpr char GE_FILTER_LINEAR_GRADIENT_BLUR_TRAN_Y[] = "TRANY";
struct GELinearGradientBlurShaderFilterParams {
    float blurRadius;
    std::vector<std::pair<float, float>> fractionStops;
    int direction;
    float geoWidth;
    float geoHeight;
    Drawing::Matrix mat;
    float tranX;
    float tranY;
    bool isOffscreenCanvas;
};

constexpr char GE_FILTER_MAGNIFIER[] = "MAGNIFIER";
constexpr char GE_FILTER_MAGNIFIER_FACTOR[] = "FACTOR";
constexpr char GE_FILTER_MAGNIFIER_WIDTH[] = "WIDTH";
constexpr char GE_FILTER_MAGNIFIER_HEIGHT[] = "HEIGHT";
constexpr char GE_FILTER_MAGNIFIER_CORNER_RADIUS[] = "CORNERRADIUS";
constexpr char GE_FILTER_MAGNIFIER_BORDER_WIDTH[] = "BORDERWIDTH";
constexpr char GE_FILTER_MAGNIFIER_SHADOW_OFFSET_X[] = "SHADOWOFFSETX";
constexpr char GE_FILTER_MAGNIFIER_SHADOW_OFFSET_Y[] = "SHADOWOFFSETY";
constexpr char GE_FILTER_MAGNIFIER_SHADOW_SIZE[] = "SHADOWSIZE";
constexpr char GE_FILTER_MAGNIFIER_SHADOW_STRENGTH[] = "SHADOWSTRENGTH";
constexpr char GE_FILTER_MAGNIFIER_GRADIENT_MASK_COLOR_1[] = "GRADIENTMASKCOLOR1";
constexpr char GE_FILTER_MAGNIFIER_GRADIENT_MASK_COLOR_2[] = "GRADIENTMASKCOLOR2";
constexpr char GE_FILTER_MAGNIFIER_OUTER_CONTOUR_COLOR_1[] = "OUTERCONTOURCOLOR1";
constexpr char GE_FILTER_MAGNIFIER_OUTER_CONTOUR_COLOR_2[] = "OUTERCONTOURCOLOR2";
constexpr char GE_FILTER_MAGNIFIER_ROTATE_DEGREE[] = "ROTATEDEGREE";
struct GEMagnifierShaderFilterParams {
    float factor = 0.f;
    float width = 0.f;
    float height = 0.f;
    float cornerRadius = 0.f;
    float borderWidth = 0.f;

    float shadowOffsetX = 0.f;
    float shadowOffsetY = 0.f;
    float shadowSize = 0.f;
    float shadowStrength = 0.f;

    // rgba
    uint32_t gradientMaskColor1 = 0x00000000;
    uint32_t gradientMaskColor2 = 0x00000000;
    uint32_t outerContourColor1 = 0x00000000;
    uint32_t outerContourColor2 = 0x00000000;

    int32_t rotateDegree = 0;
};

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

#endif // GRAPHICS_EFFECT_GE_SHADER_FILTER_PARAMS_H
