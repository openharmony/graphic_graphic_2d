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
    COLOR,      // hue and saturation of source with luminosity of destination
    LUMINOSITY, // luminosity of source with hue and saturation of destination

    MAX = LUMINOSITY,
};

// color blend apply mode
enum class RSColorBlendApplyType : int16_t {
    FAST,        // Apply blending by drawing the content with the blend mode, without using an offscreen buffer

    SAVE_LAYER,  // Apply blending by drawing the content onto an offscreen buffer and blend it when drawing it back to
                 // the screen
    MAX = SAVE_LAYER
};

class Decoration final {
public:
    Decoration() {}
    ~Decoration() {}
    std::shared_ptr<RSShader> bgShader_ = nullptr;
    std::shared_ptr<RSImage> bgImage_ = nullptr;
    RectF bgImageRect_ = RectF();
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
        return !lightSources_.empty();
    }
    bool IsIlluminatedValid() const
    {
        return illuminatedType_ != IlluminatedType::NONE;
    }
    const IlluminatedType& GetPreIlluminatedType() const
    {
        return preIlluminatedType_;
    }
    void AddLightSource(const std::shared_ptr<RSLightSource>& lightSourcePtr)
    {
        lightSources_.emplace(lightSourcePtr);
    }
    void ClearLightSource()
    {
        lightSources_.clear();
    }
    std::unordered_set<std::shared_ptr<RSLightSource>>& GetLightSources()
    {
        return lightSources_;
    }

private:
    IlluminatedType illuminatedType_ = IlluminatedType::NONE;
    float bloomIntensity_ = 0.f;
    float illuminatedBorderWidth_ = 0.f;
    IlluminatedType preIlluminatedType_ = IlluminatedType::NONE;
    // record the light sources that the node is illuminated by.
    std::unordered_set<std::shared_ptr<RSLightSource>> lightSources_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PROPERTY_RS_PROPERTIES_DEF_H
