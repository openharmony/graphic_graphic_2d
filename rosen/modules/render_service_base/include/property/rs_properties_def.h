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

#ifndef RENDER_SERVICE_CLIENT_CORE_PROPERTY_RS_PROPERTIES_DEF_H
#define RENDER_SERVICE_CLIENT_CORE_PROPERTY_RS_PROPERTIES_DEF_H

#include "common/rs_common_def.h"
#include "utils/matrix.h"

#include "common/rs_color_palette.h"
#include "common/rs_rect.h"
#include "common/rs_vector4.h"

namespace OHOS {
namespace Rosen {
class RSObjGeometry;
class RSImage;
class RSShader;

constexpr float INVALID_INTENSITY = -1.f;
constexpr int RGB_NUM = 3;
constexpr float SHADOW_BLENDER_LOWER_LIMIT = -100.f;
constexpr float SHADOW_BLENDER_UPPER_LIMIT = 100.f;

enum class Gravity {
    CENTER = 0,
    TOP,
    BOTTOM,
    LEFT,
    RIGHT,
    TOP_LEFT,
    TOP_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_RIGHT,
    RESIZE,
    RESIZE_ASPECT,
    RESIZE_ASPECT_TOP_LEFT,
    RESIZE_ASPECT_BOTTOM_RIGHT,
    RESIZE_ASPECT_FILL,
    RESIZE_ASPECT_FILL_TOP_LEFT,
    RESIZE_ASPECT_FILL_BOTTOM_RIGHT,

    DEFAULT = TOP_LEFT
};

enum class ForegroundColorStrategyType {
    INVALID = 0,
    INVERT_BACKGROUNDCOLOR,
};

enum class OutOfParentType {
    WITHIN = 0,
    OUTSIDE,
    UNKNOWN
};

// color blend mode, add NONE based on SkBlendMode
enum class RSColorBlendMode : int16_t {
    NONE = 0, // Note: The NONE blend mode is different from SRC_OVER. When using it with
              // RSColorBlendApplyType::SAVE_LAYER, it does not create an offscreen buffer. However, when using it
              // with RSColorBlendApplyType::FAST, it does not modify the blend mode of subsequent content.

    CLEAR,    // r = 0
    SRC,      // r = s
    DST,      // r = d
    SRC_OVER, // r = s + (1-sa)*d
    DST_OVER, // r = d + (1-da)*s
    SRC_IN,   // r = s * da
    DST_IN,   // r = d * sa
    SRC_OUT,  // r = s * (1-da)
    DST_OUT,  // r = d * (1-sa)
    SRC_ATOP, // r = s*da + d*(1-sa)
    DST_ATOP, // r = d*sa + s*(1-da)
    XOR,      // r = s*(1-da) + d*(1-sa)
    PLUS,     // r = min(s + d, 1)
    MODULATE, // r = s*d
    SCREEN,   // r = s + d - s*d

    OVERLAY,     // multiply or screen, depending on destination
    DARKEN,      // rc = s + d - max(s*da, d*sa), ra = SRC_OVER
    LIGHTEN,     // rc = s + d - min(s*da, d*sa), ra = SRC_OVER
    COLOR_DODGE, // brighten destination to reflect source
    COLOR_BURN,  // darken destination to reflect source
    HARD_LIGHT,  // multiply or screen, depending on source
    SOFT_LIGHT,  // lighten or darken, depending on source
    DIFFERENCE,  // rc = s + d - 2*(min(s*da, d*sa)), ra = SRC_OVER
    EXCLUSION,   // rc = s + d - two(s*d), ra = SRC_OVER
    MULTIPLY,    // r = s*(1-da) + d*(1-sa) + s*d

    HUE,        // hue of source with saturation and luminosity of destination
    SATURATION, // saturation of source with hue and luminosity of destination
    COLOUR,     // hue and saturation of source with luminosity of destination
    LUMINOSITY, // luminosity of source with hue and saturation of destination

    MAX = LUMINOSITY,
};

// color blend apply mode
enum class RSColorBlendApplyType : int16_t {
    FAST,        // Apply blending by drawing the content with the blend mode, without using an offscreen buffer

    SAVE_LAYER,  // Apply blending by drawing the content onto an offscreen buffer and blend it when drawing it back to
                 // the screen
    SAVE_LAYER_ALPHA,   // Similar to SAVE_LAYER, but when drawing back to the screen, it will be multiplied by the
                        // alpha of the screen twice. We add this type to avoid incompatibility changes. Don't use
                        // it in the future unless you have a clear understanding of what will happen!
    // init with a copy of the background for advanced use.
    //  Do NOT use this unless you have a clear understanding of what will happen!
    SAVE_LAYER_INIT_WITH_PREVIOUS_CONTENT,
    MAX = SAVE_LAYER_INIT_WITH_PREVIOUS_CONTENT
};

struct RSDynamicBrightnessPara {
    Vector4f rates_ {};
    float saturation_ = 0.0f;
    Vector4f posCoeff_ {};
    Vector4f negCoeff_ {};
    float fraction_ = 1.0;
    bool enableHdr_ = false;

    RSDynamicBrightnessPara() = default;

    RSDynamicBrightnessPara(float rate, float lightUpDegree, float cubicCoeff, float quadCoeff,
        float saturation, std::array<float, RGB_NUM> posRGB, std::array<float, RGB_NUM> negRGB, bool enableHdr = false)
    {
        constexpr size_t INDEX_0 = 0;
        constexpr size_t INDEX_1 = 1;
        constexpr size_t INDEX_2 = 2;
        rates_ = Vector4f(cubicCoeff, quadCoeff, rate, lightUpDegree);
        saturation_ = saturation;
        posCoeff_ = Vector4f(posRGB[INDEX_0], posRGB[INDEX_1], posRGB[INDEX_2], 0.0f);
        negCoeff_ =  Vector4f(negRGB[INDEX_0], negRGB[INDEX_1], negRGB[INDEX_2], 0.0f);
        fraction_ = 1.0f;
        enableHdr_ = enableHdr;
    }

    inline bool IsValid() const
    {
        return ROSEN_LNE(fraction_, 1.0);
    }

    bool operator==(const RSDynamicBrightnessPara& other) const
    {
        return (rates_ == other.rates_ && saturation_ == other.saturation_ && posCoeff_ == other.posCoeff_ &&
            negCoeff_ == other.negCoeff_ && fraction_ == other.fraction_);
    }
};

struct RSShadowBlenderPara {
    float cubic_ = 0.0f;
    float quadratic_ = 0.0f;
    float linear_ = 0.0f;
    float constant_ = 0.0f;

    RSShadowBlenderPara() = default;

    RSShadowBlenderPara(float cubic, float quadratic, float linear, float constant)
        : cubic_(cubic), quadratic_(quadratic), linear_(linear), constant_(constant) {}

    inline bool IsValid() const
    {
        return ROSEN_GNE(cubic_, SHADOW_BLENDER_LOWER_LIMIT) && ROSEN_LNE(cubic_, SHADOW_BLENDER_UPPER_LIMIT) &&
            ROSEN_GNE(quadratic_, SHADOW_BLENDER_LOWER_LIMIT) && ROSEN_LNE(quadratic_, SHADOW_BLENDER_UPPER_LIMIT) &&
            ROSEN_GNE(linear_, SHADOW_BLENDER_LOWER_LIMIT) && ROSEN_LNE(linear_, SHADOW_BLENDER_UPPER_LIMIT) &&
            ROSEN_GNE(constant_, SHADOW_BLENDER_LOWER_LIMIT) && ROSEN_LNE(constant_, SHADOW_BLENDER_UPPER_LIMIT);
    }

    bool operator==(const RSShadowBlenderPara& other) const
    {
        return (cubic_ == other.cubic_ && quadratic_ == other.quadratic_ &&
            linear_ == other.linear_ && constant_ == other.constant_);
    }
};

struct RSWaterRipplePara {
    uint32_t waveCount = 0;
    float rippleCenterX = 0.5f;
    float rippleCenterY = 0.7f;
    uint32_t rippleMode = 1;
    bool operator==(const RSWaterRipplePara& other) const
    {
        return (waveCount == other.waveCount && ROSEN_EQ(rippleCenterX, other.rippleCenterX) &&
        ROSEN_EQ(rippleCenterY, other.rippleCenterY) && rippleMode == other.rippleMode);
    }
};

struct RSFlyOutPara {
    uint32_t flyMode = 0;
    bool operator==(const RSFlyOutPara& other) const
    {
        return (flyMode == other.flyMode);
    }
};

class Decoration final {
public:
    Decoration() {}
    ~Decoration() {}
    std::shared_ptr<RSShader> bgShader_ = nullptr;
    float bgShaderProgress_ = 0.0f;
    std::shared_ptr<RSImage> bgImage_ = nullptr;
    RectF bgImageRect_ = RectF();
    Vector4f bgImageInnerRect_ = Vector4f();
    Color backgroundColor_ = RgbPalette::Transparent();
    Color foregroundColor_ = RgbPalette::Transparent();
};

class Sandbox final {
public:
    Sandbox() {}
    ~Sandbox() {}
    std::optional<Vector2f> position_;
    std::optional<Drawing::Matrix> matrix_;
};

enum class IlluminatedType : uint32_t {
    INVALID = -1,
    NONE = 0,
    BORDER,
    CONTENT,
    BORDER_CONTENT,
    BLOOM_BORDER,
    BLOOM_BORDER_CONTENT,
    BLEND_BORDER,
    BLEND_CONTENT,
    BLEND_BORDER_CONTENT,
    FEATHERING_BORDER,
};

class RSLightSource final {
public:
    RSLightSource() = default;
    ~RSLightSource() {}
    void SetLightPosition(const Vector4f& lightPosition)
    {
        lightPosition_ = lightPosition;
        radius_ = CalculateLightRadius(lightPosition_.z_);
    }
    void SetLightIntensity(float lightIntensity)
    {
        if (!ROSEN_EQ(intensity_, INVALID_INTENSITY)) {
            preIntensity_ = intensity_;
        }
        intensity_ = lightIntensity;
    }
    void SetLightColor(Color lightColor)
    {
        lightColor_ = lightColor;
    }
    void SetAbsLightPosition(const Vector4f& absLightPosition)
    {
        absLightPosition_ = absLightPosition;
    }

    const Vector4f& GetLightPosition() const
    {
        return lightPosition_;
    }
    const Vector4f& GetAbsLightPosition() const
    {
        return absLightPosition_;
    }
    float GetLightIntensity() const
    {
        return intensity_;
    }
    Color GetLightColor() const
    {
        return lightColor_;
    }

    bool IsLightSourceValid() const
    {
        return !ROSEN_EQ(intensity_, 0.f);
    }
    float GetPreLightIntensity() const
    {
        return preIntensity_;
    }
    float GetLightRadius() const
    {
        return radius_;
    }

private:
    static float CalculateLightRadius(float lightPosZ)
    {
        float num = 1.0f / 255;
        float count = 8;
        float cos = std::pow(num, 1.f / count);
        float tan = std::sqrt(static_cast<float>(1 - pow(cos, 2))) / cos;
        return lightPosZ * tan;
    }
    Vector4f lightPosition_ = Vector4f();
    Vector4f absLightPosition_ = Vector4f(); // absolute light Position;
    float intensity_ = 0.f;
    float preIntensity_ = 0.f;
    Color lightColor_ = RgbPalette::White();
    float radius_ = 0.f;
};

class RSIlluminated final {
public:
    void SetIlluminatedType(IlluminatedType& illuminatedType)
    {
        if (illuminatedType_ != IlluminatedType::INVALID) {
            preIlluminatedType_ = illuminatedType_;
        }
        illuminatedType_ = illuminatedType;
    }
    void SetBloomIntensity(float bloomIntensity)
    {
        bloomIntensity_ = bloomIntensity;
    }
    void SetIlluminatedBorderWidth(float illuminatedBorderWidth)
    {
        illuminatedBorderWidth_ = illuminatedBorderWidth;
    }
    float GetBloomIntensity() const
    {
        return bloomIntensity_;
    }
    const IlluminatedType& GetIlluminatedType() const
    {
        return illuminatedType_;
    }
    float GetIlluminatedBorderWidth() const
    {
        return illuminatedBorderWidth_;
    }
    bool IsIlluminated() const
    {
        return !lightSourcesAndPosMap_.empty();
    }
    bool IsIlluminatedValid() const
    {
        return illuminatedType_ != IlluminatedType::NONE;
    }
    const IlluminatedType& GetPreIlluminatedType() const
    {
        return preIlluminatedType_;
    }
    void AddLightSourcesAndPos(const std::shared_ptr<RSLightSource>& lightSourcePtr, const Vector4f& lightPos)
    {
        lightSourcesAndPosMap_.insert(std::make_pair(lightSourcePtr, lightPos));
    }
    void ClearLightSourcesAndPosMap()
    {
        lightSourcesAndPosMap_.clear();
    }
    std::unordered_map<std::shared_ptr<RSLightSource>, Vector4f>& GetLightSourcesAndPosMap()
    {
        return lightSourcesAndPosMap_;
    }

private:
    IlluminatedType illuminatedType_ = IlluminatedType::NONE;
    float bloomIntensity_ = 0.f;
    float illuminatedBorderWidth_ = 0.f;
    IlluminatedType preIlluminatedType_ = IlluminatedType::NONE;
    // saves the mapping between the light source that illuminates the node and the position of lightSource relative to
    // the node.
    std::unordered_map<std::shared_ptr<RSLightSource>, Vector4f> lightSourcesAndPosMap_;
};

enum class UseEffectType : int16_t {
    EFFECT_COMPONENT = 0,
    BEHIND_WINDOW,
    DEFAULT = EFFECT_COMPONENT,
    MAX = BEHIND_WINDOW
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PROPERTY_RS_PROPERTIES_DEF_H
