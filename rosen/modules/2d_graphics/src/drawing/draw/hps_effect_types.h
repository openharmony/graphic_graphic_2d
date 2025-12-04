/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.. All rights reserved.
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

#ifndef HPS_EFFECT_TYPES_H
#define HPS_EFFECT_TYPES_H

#include "array"
#include "common/rs_macros.h"
#include "drawing/engine_adapter/impl_interface/core_canvas_impl.h"
#include "utils/drawing_macros.h"
#include "utils/rect.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
static constexpr float ZERO_THRESHOLD = 1e-6;
static constexpr int MATRIX_3X3_SIZE = 9; // equals to Drawing::Matrix::MATRIX_SIZE

/**
 * @enum HpsEffect
 * @brief Enumeration of different hps processing effects.
 */
typedef enum HpsEffect {
    BLUR,
    GREY,
    AIBAR,
    STRETCH,
    LINEAR_GRADIENT_BLUR,
    MESA,
    STATISTICS,
    EDGE_LIGHT,
} HpsEffect;
/**
 * @enum HpsEffect
 * @brief Enumeration of different hps processing effects.
 */
typedef enum HpsMask {
    PIXEL_MAP_MASK,
    RADIAL_GRADIENT_MASK,
} HpsMask;
/**
 * @enum HpsImageStatistics
 * @brief Enumeration of different hps Image Statistics types.
 */
typedef enum HpsStatisticsType {
    MEAN = 0,
} HpsStatisticsType;
/**
 * @class HpsEffectParameter
 * @brief Base class for all hps effect arguments.
 */
class HpsEffectParameter {
public:
    Rect src;
    Rect dst;
    std::array<float, MATRIX_3X3_SIZE> transformMatrix = { 1, 0, 0, 0, 1, 0, 0, 0, 1 };  // Identity matrix
    HpsEffectParameter(const Rect& s, const Rect& d)
        : src(s), dst(d) {}
    virtual ~HpsEffectParameter() = default;
    virtual HpsEffect GetEffectType() = 0;
};
/**
 * @class HpsMaskParameter
 * @brief Base class for all hps mask arguments.
 */
class HpsMaskParameter {
public:
    bool isSDFShaderMask = false;
    HpsMaskParameter(bool isSDFShaderMask) : isSDFShaderMask(isSDFShaderMask)
    {}
    virtual ~HpsMaskParameter() = default;
    virtual HpsMask GetMaskType() = 0;
};
/**
 * @class HpsBlurEffectParameter
 * @brief Arguments for the gaussian blur effect.
 * @param sigma Blur radius
 * @param pNoiseRatio When pNoiseRatio is null, no processing be performed
 */
class HpsBlurEffectParameter : public HpsEffectParameter {
public:
    scalar sigma { ZERO_THRESHOLD };
    float saturation { 1.0 };
    float brightness { 1.0 };
    std::shared_ptr<float> pNoiseRatio { nullptr };
    HpsBlurEffectParameter(const Rect& s, const Rect& d, const scalar& sgm,
        float satura, float bright, std::shared_ptr<float> noise = nullptr)
        : HpsEffectParameter(s, d), sigma(sgm), saturation(satura), brightness(bright), pNoiseRatio(noise) {}
    HpsEffect GetEffectType() override
    {
        return HpsEffect::BLUR;
    }
};
/**
 * @class HpsGreyParameter
 * @brief Arguments for the greyscale effect.
 * @param greyCoefLow the degree of black brightness
 * @param greyCoefHigh the degree of white color reduction
 */
class HpsGreyParameter : public HpsEffectParameter {
public:
    float greyCoefLow { ZERO_THRESHOLD };
    float greyCoefHigh { ZERO_THRESHOLD };
    HpsGreyParameter(const Rect& s, const Rect& d, float gcl, float gch)
        : HpsEffectParameter(s, d), greyCoefLow(gcl), greyCoefHigh(gch) {}
    HpsEffect GetEffectType() override
    {
        return HpsEffect::GREY;
    }
};
/**
 * @class HpsMesaParameter
 * @brief Arguments for the Mesa effect.
 * @param sigma Blur radius (sigma).
 * @param greyCoefLow the degree of black brightness
 * @param greyCoefHigh the degree of white color reduction
 * @param offsetX Extended pixel size at the left of the image
 * @param offsetY Extended pixel size at the top of the image
 * @param offsetZ Extended pixel size on the right of the image
 * @param offsetW Extended pixel size at the bottom of the image
 * @param tileMode Out of Range Sampling Mode
 */
class HpsMesaParameter : public HpsEffectParameter {
public:
    scalar sigma { ZERO_THRESHOLD };
    float greyCoefLow { ZERO_THRESHOLD };
    float greyCoefHigh { ZERO_THRESHOLD };
    float offsetX { ZERO_THRESHOLD };
    float offsetY { ZERO_THRESHOLD };
    float offsetZ { ZERO_THRESHOLD };
    float offsetW { ZERO_THRESHOLD };
    int tileMode { 0 };
    float width { ZERO_THRESHOLD };
    float height { ZERO_THRESHOLD };
    HpsMesaParameter(const Rect& s, const Rect& d, const scalar& sgm,
        float gcl, float gch, float offX, float offY, float offZ, float offW, int tMode, float w, float h)
        : HpsEffectParameter(s, d), sigma(sgm), greyCoefLow(gcl), greyCoefHigh(gch), offsetX(offX), offsetY(offY),
        offsetZ(offZ), offsetW(offW), tileMode(tMode), width(w), height(h) {}
    HpsEffect GetEffectType() override
    {
        return HpsEffect::MESA;
    }
};
/**
 * @class HpsAiBarParameter
 * @brief Arguments for the AI-based bar effect.
 * @param low lower bound of binary value
 * @param high Upper limit of binary value
 * @param threshold threshold of the binary value
 */
class HpsAiBarParameter : public HpsEffectParameter {
public:
    float low { ZERO_THRESHOLD };
    float high { ZERO_THRESHOLD };
    float threshold { ZERO_THRESHOLD };
    float opacity { ZERO_THRESHOLD };
    float saturation { ZERO_THRESHOLD };
    HpsAiBarParameter(const Rect& s, const Rect& d, float low, float high, float threshold,
        float opacity, float saturation) : HpsEffectParameter(s, d), low(low), high(high), threshold(threshold),
        opacity(opacity), saturation(saturation) {}
    HpsEffect GetEffectType() override
    {
        return HpsEffect::AIBAR;
    }
};
/**
 * @class HpsStretchParameter
 * @brief Arguments for the pixel expansion effect.
 * @param tileMode Out of Range Sampling Mode
 */
class HpsStretchParameter : public HpsEffectParameter {
public:
    float offsetX { ZERO_THRESHOLD };
    float offsetY { ZERO_THRESHOLD };
    float offsetZ { ZERO_THRESHOLD };
    float offsetW { ZERO_THRESHOLD };
    int tileMode { 0 };
    float width { ZERO_THRESHOLD };
    float height { ZERO_THRESHOLD };
    HpsStretchParameter(const Rect& s, const Rect& d, float offsetX, float offsetY, float offsetZ,
        float offsetW, int tileMode, float width, float height) : HpsEffectParameter(s, d), offsetX(offsetX),
        offsetY(offsetY), offsetZ(offsetZ), offsetW(offsetW), tileMode(tileMode), width(width), height(height) {}
    HpsEffect GetEffectType() override
    {
        return HpsEffect::STRETCH;
    }
};
/**
 * @class HpsGradientBlurParameter
 * @brief Arguments for the linear gradient blur effect.
 * @param fractionStops Gradient effect color-position pair array
 * @param fractionStopsCount Gradient Effect Color-Position vs. Quantity
 * @param mat Coordinate Transformation Matrix of Gradient Start Point
 */
class HpsGradientBlurParameter : public HpsEffectParameter {
public:
    scalar blurRadius { ZERO_THRESHOLD };
    std::shared_ptr<std::vector<float>> fractionStops;
    uint32_t fractionStopsCount { 0 };
    int direction { 0 };
    float geoWidth { ZERO_THRESHOLD };
    float geoHeight { ZERO_THRESHOLD };
    std::array<float, MATRIX_3X3_SIZE> mat;
    HpsGradientBlurParameter(const Rect& s, const Rect& d, const scalar& sgm,
        std::shared_ptr<std::vector<float>> fractionStops, uint32_t fractionStopsCount, int direction,
        float geoWidth, float geoHeight, std::array<float, MATRIX_3X3_SIZE> mat) : HpsEffectParameter(s, d),
        blurRadius(sgm), fractionStops(fractionStops), fractionStopsCount(fractionStopsCount), direction(direction),
        geoWidth(geoWidth), geoHeight(geoHeight), mat(mat) {}
    HpsEffect GetEffectType() override
    {
        return HpsEffect::LINEAR_GRADIENT_BLUR;
    }
};
/**
 * @class HpsStatisticsParameter
 * @brief Arguments for the image statistics effect.
 * @param statisticsType Statistics effect type of Image
 */
class HpsStatisticsParameter : public HpsEffectParameter {
public:
    HpsStatisticsType statisticsType;
    HpsStatisticsParameter(const Rect& s, const Rect& d, const HpsStatisticsType& statisticsType)
        : HpsEffectParameter(s, d), statisticsType(statisticsType) {}
    HpsEffect GetEffectType() override
    {
        return HpsEffect::STATISTICS;
    }
};
/**
 * @class HpsEdgeLightParameter
 * @brief Arguments for the edgeLight effect.
 */
class HpsEdgeLightParameter : public HpsEffectParameter {
public:
    struct EdgeSobelParameter {
        float edgeThreshold { ZERO_THRESHOLD };
        float edgeIntensity { ZERO_THRESHOLD };
        float edgeSoftThreshold { ZERO_THRESHOLD };
        std::vector<float> edgeDetectColor;
    };
    float alpha { ZERO_THRESHOLD };
    bool bloom { false };
    bool useRawColor { false };
    std::vector<float> color;
    std::shared_ptr<HpsMaskParameter> mask { nullptr };
    EdgeSobelParameter edgeSobelParams;
    uint32_t updatedType = 0;
    HpsEdgeLightParameter(const Rect& s, const Rect& d, float alpha, bool bloom, bool useRawColor,
        const std::vector<float>& color, std::shared_ptr<HpsMaskParameter> mask,
        const EdgeSobelParameter& edgeSobelParams, uint32_t updatedType)
        : HpsEffectParameter(s, d), alpha(alpha), bloom(bloom), useRawColor(useRawColor), color(color), mask(mask),
        edgeSobelParams(edgeSobelParams), updatedType(updatedType) {}
    HpsEffect GetEffectType() override
    {
        return HpsEffect::EDGE_LIGHT;
    }
};
/**
 * @class HpsPixelMapMaskParameter
 * @brief Arguments for the pixelMap mask.
 */
class HpsPixelMapMaskParameter : public HpsMaskParameter {
public:
    Rect visibleRegion;
    std::array<float, MATRIX_3X3_SIZE> transformMatrix;
    std::vector<float> fillColor;
    std::shared_ptr<Drawing::Image> image;
    HpsPixelMapMaskParameter(std::shared_ptr<Drawing::Image> image, const Rect& visibleRegion,
        std::array<float, MATRIX_3X3_SIZE>& transformMatrix, const std::vector<float>& fillColor)
        : HpsMaskParameter(false), visibleRegion(visibleRegion), transformMatrix(transformMatrix),
          fillColor(fillColor), image(image) {}
    HpsMask GetMaskType() override
    {
        return HpsMask::PIXEL_MAP_MASK;
    }
};
/**
 * @class HpsRadialGradientMaskParameter
 * @brief Arguments for the radialGradient mask.
 */
class HpsRadialGradientMaskParameter : public HpsMaskParameter {
public:
    float centerX = 0.f;
    float centerY = 0.f;
    float radiusX = 0.f;
    float radiusY = 0.f;
    std::vector<float> colors;
    std::vector<float> positions;
    HpsRadialGradientMaskParameter(float centerX, float centerY, float radiusX, float radiusY,
        const std::vector<float>& colors, const std::vector<float>& positions)
        : HpsMaskParameter(false), centerX(centerX), centerY(centerY), radiusX(radiusX),
          radiusY(radiusY), colors(colors), positions(positions) {}
    HpsMask GetMaskType() override
    {
        return HpsMask::RADIAL_GRADIENT_MASK;
    }
};
}
}
}

#endif