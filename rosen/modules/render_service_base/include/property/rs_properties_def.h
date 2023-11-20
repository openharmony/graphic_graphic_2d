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
#ifndef USE_ROSEN_DRAWING
#include "include/core/SkMatrix.h"
#else
#include "utils/matrix.h"
#endif

#include "common/rs_color_palette.h"
#include "common/rs_rect.h"
#include "common/rs_vector4.h"

#define INVALID_INTENSITY (-1.f)
namespace OHOS {
namespace Rosen {
class RSObjGeometry;
class RSImage;
class RSShader;

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

enum class RSColorBlendModeType : int16_t {
    NONE = 0,
    DST_IN,
    SRC_IN,
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
#ifndef USE_ROSEN_DRAWING
    std::optional<SkMatrix> matrix_;
#else
    std::optional<Drawing::Matrix> matrix_;
#endif
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
    float GetPreLigthIntensity() const
    {
        return preIntensity_;
    }

private:
    Vector4f lightPosition_;
    Vector4f absLightPosition_; // absolute light Position;
    float intensity_;
    float preIntensity_;
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
    void SetIsIlluminated(bool isIlluminated)
    {
        isIlluminated_ = isIlluminated;
    }

    void SetBloomIntensity(float bloomIntensity)
    {
        bloomIntensity_ = bloomIntensity;
    }
    float GetBloomIntensity() const
    {
        return bloomIntensity_;
    }

    const IlluminatedType& GetIlluminatedType() const
    {
        return illuminatedType_;
    }

    bool IsIlluminated() const
    {
        return isIlluminated_;
    }
    bool IsIlluminatedValid() const
    {
        return illuminatedType_ != IlluminatedType::NONE;
    }
    const IlluminatedType& GetPreIlluminatedType() const
    {
        return preIlluminatedType_;
    }

private:
    IlluminatedType illuminatedType_;
    bool isIlluminated_;
    float bloomIntensity_;
    IlluminatedType preIlluminatedType_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PROPERTY_RS_PROPERTIES_DEF_H
