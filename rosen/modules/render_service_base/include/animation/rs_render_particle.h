/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_PARTICLE_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_PARTICLE_H

#include <memory>
#include <sys/types.h>
#include <vector>

#include "animation/rs_interpolator.h"
#include "common/rs_color.h"
#include "common/rs_common_def.h"
#include "common/rs_macros.h"
#include "common/rs_vector2.h"
#include "render/rs_image.h"
namespace OHOS {
namespace Rosen {
enum class ParticleUpdator { NONE = 0, RANDOM, CURVE };

enum class ShapeType { RECT = 0, CIRCLE, ELLIPSE };

enum class ParticleType {
    POINTS,
    IMAGES,
};

template<typename T>
struct Range {
    T start_, end_;
    Range() : start_(), end_() {}
    Range(T a, T b) : start_(a), end_(b) {}

    T Width() const
    {
        return end_ - start_;
    }
};

template<typename T>
class RSB_EXPORT ChangeInOverLife {
public:
    T fromValue_;
    T toValue_;
    int startMillis_;
    int endMillis_;
    std::shared_ptr<RSInterpolator> interpolator_ { RSInterpolator::DEFAULT };
    ChangeInOverLife() : fromValue_(), toValue_(), startMillis_(), endMillis_(), interpolator_() {}
    ChangeInOverLife(const T& fromValue, const T& toValue, const int& startMillis, const int& endMillis,
        const std::shared_ptr<RSInterpolator>& interpolator)
    {
        fromValue_ = fromValue;
        toValue_ = toValue;
        startMillis_ = startMillis;
        endMillis_ = endMillis;
        interpolator_ = interpolator;
    }
    ChangeInOverLife(const ChangeInOverLife& change) = default;
    ChangeInOverLife& operator=(const ChangeInOverLife& change) = default;
    ~ChangeInOverLife() = default;
};

template<typename T>
class RSB_EXPORT RenderParticleParaType {
public:
    Range<T> val_;
    ParticleUpdator updator_;
    Range<float> random_;
    std::vector<std::shared_ptr<ChangeInOverLife<T>>> valChangeOverLife_;
    RenderParticleParaType(const Range<T>& val, const ParticleUpdator& updator, const Range<float>& random,
        const std::vector<std::shared_ptr<ChangeInOverLife<T>>>& valChangeOverLife)
    {
        val_ = val;
        updator_ = updator;
        random_ = random;
        for (size_t i = 0; i < valChangeOverLife.size(); i++) {
            auto change = valChangeOverLife[i];
            valChangeOverLife_.push_back(change);
        }
    }
    RenderParticleParaType() : val_(), updator_(ParticleUpdator::NONE), random_() {}
    RenderParticleParaType(const RenderParticleParaType& paraType) = default;
    RenderParticleParaType& operator=(const RenderParticleParaType& paraType) = default;
    ~RenderParticleParaType() = default;
};

class RSB_EXPORT EmitterConfig {
public:
    int emitRate_;
    ShapeType emitShape_;
    Vector2f position_;
    Vector2f emitSize_;
    int particleCount_;
    int64_t lifeTime_;
    ParticleType type_;
    float radius_;
    std::shared_ptr<RSImage> image_;
    Vector2f imageSize_;

    EmitterConfig()
        : emitRate_(), emitShape_(ShapeType::RECT), position_(), emitSize_(), particleCount_(), lifeTime_(),
          type_(ParticleType::POINTS), radius_(), image_(), imageSize_()
    {}
    EmitterConfig(const int& emitRate, const ShapeType& emitShape, const Vector2f& position, const Vector2f& emitSize,
        const int& particleCount, const int& lifeTime, const ParticleType& type, const float& radius,
        const std::shared_ptr<RSImage>& image, Vector2f imageSize)
    {
        emitRate_ = emitRate;
        emitShape_ = emitShape;
        position_ = position;
        emitSize_ = emitSize;
        particleCount_ = particleCount;
        lifeTime_ = lifeTime * NS_PER_MS;
        type_ = type;
        radius_ = radius;
        image_ = image;
        imageSize_ = imageSize;
    }
    EmitterConfig(const EmitterConfig& config) = default;
    EmitterConfig& operator=(const EmitterConfig& config) = default;
    ~EmitterConfig() = default;
};

class RSB_EXPORT ParticleVelocity {
public:
    Range<float> velocityValue_;
    Range<float> velocityAngle_;

    ParticleVelocity() : velocityValue_(), velocityAngle_() {}
    ParticleVelocity(const Range<float>& velocityValue, const Range<float>& velocityAngle)
    {
        velocityValue_ = Range(velocityValue.start_, velocityValue.end_);
        velocityAngle_ = Range(velocityAngle.start_, velocityAngle.end_);
    }
    ParticleVelocity(const ParticleVelocity& velocity) = default;
    ParticleVelocity& operator=(const ParticleVelocity& velocity) = default;
    ~ParticleVelocity() = default;
};

class RSB_EXPORT RenderParticleAcceleration {
public:
    RenderParticleParaType<float> accelerationValue_;
    RenderParticleParaType<float> accelerationAngle_;

    RenderParticleAcceleration() = default;
    RenderParticleAcceleration(
        const RenderParticleParaType<float>& accelerationValue, const RenderParticleParaType<float>& accelerationAngle)
    {
        accelerationValue_ = accelerationValue;
        accelerationAngle_ = accelerationAngle;
    }
    RenderParticleAcceleration(const RenderParticleAcceleration& acceleration) = default;
    RenderParticleAcceleration& operator=(const RenderParticleAcceleration& acceleration) = default;
    ~RenderParticleAcceleration() = default;
};

class RSB_EXPORT RenderParticleColorParaType {
public:
    Range<Color> colorVal_;
    ParticleUpdator updator_;
    Range<float> redRandom_;
    Range<float> greenRandom_;
    Range<float> blueRandom_;
    Range<float> alphaRandom_;

    std::vector<std::shared_ptr<ChangeInOverLife<Color>>> valChangeOverLife_;
    RenderParticleColorParaType(const Range<Color>& colorVal, const ParticleUpdator& updator,
        const Range<float>& redRandom, const Range<float>& greenRandom, const Range<float>& blueRandom,
        const Range<float>& alphaRandom, std::vector<std::shared_ptr<ChangeInOverLife<Color>>>& valChangeOverLife)
    {
        colorVal_ = colorVal;
        updator_ = updator;
        redRandom_ = redRandom;
        greenRandom_ = greenRandom;
        blueRandom_ = blueRandom;
        alphaRandom_ = alphaRandom;
        for (size_t i = 0; i < valChangeOverLife.size(); i++) {
            auto change = valChangeOverLife[i];
            valChangeOverLife_.push_back(change);
        }
    }
    RenderParticleColorParaType()
        : colorVal_(), updator_(ParticleUpdator::NONE), redRandom_(), greenRandom_(), blueRandom_(), alphaRandom_()
    {}
    RenderParticleColorParaType(const RenderParticleColorParaType& velocity) = default;
    RenderParticleColorParaType& operator=(const RenderParticleColorParaType& velocity) = default;
    ~RenderParticleColorParaType() = default;
};

class RSB_EXPORT ParticleRenderParams {
public:
    EmitterConfig emitterConfig_;
    ParticleVelocity velocity_;
    RenderParticleAcceleration acceleration_;
    RenderParticleColorParaType color_;
    RenderParticleParaType<float> opacity_;
    RenderParticleParaType<float> scale_;
    RenderParticleParaType<float> spin_;
    explicit ParticleRenderParams(const EmitterConfig& emitterConfig, const ParticleVelocity& velocity,
        const RenderParticleAcceleration& acceleration, const RenderParticleColorParaType& color,
        const RenderParticleParaType<float>& opacity, const RenderParticleParaType<float>& scale,
        const RenderParticleParaType<float>& spin)
    {
        emitterConfig_ = emitterConfig;
        velocity_ = velocity;
        acceleration_ = acceleration;
        color_ = color;
        opacity_ = opacity;
        scale_ = scale;
        spin_ = spin;
    }
    ParticleRenderParams() : emitterConfig_(), velocity_(), acceleration_(), color_(), opacity_(), scale_(), spin_() {};
    ParticleRenderParams(const ParticleRenderParams& params) = default;
    ParticleRenderParams& operator=(const ParticleRenderParams& params) = default;
    ~ParticleRenderParams() = default;

    int GetEmitRate() const;
    ShapeType GetEmitShape() const;
    Vector2f GetEmitPosition() const;
    Vector2f GetEmitSize() const;
    uint32_t GetParticleCount() const;
    int64_t GetParticleLifeTime() const;
    ParticleType GetParticleType() const;
    float GetParticleRadius() const;
    std::shared_ptr<RSImage> GetParticleImage();
    Vector2f GetImageSize() const;

    float GetVelocityStartValue() const;
    float GetVelocityEndValue() const;
    float GetVelocityStartAngle() const;
    float GetVelocityEndAngle() const;

    float GetAccelerationStartValue() const;
    float GetAccelerationEndValue() const;
    float GetAccelerationStartAngle() const;
    float GetAccelerationEndAngle() const;
    ParticleUpdator GetAccelerationValueUpdator();
    ParticleUpdator GetAccelerationAngleUpdator();
    float GetAccelRandomValueStart() const;
    float GetAccelRandomValueEnd() const;
    float GetAccelRandomAngleStart() const;
    float GetAccelRandomAngleEnd() const;

    Color GetColorStartValue();
    Color GetColorEndValue();
    ParticleUpdator GetColorUpdator();
    float GetRedRandomStart() const;
    float GetRedRandomEnd() const;
    float GetGreenRandomStart() const;
    float GetGreenRandomEnd() const;
    float GetBlueRandomStart() const;
    float GetBlueRandomEnd() const;
    float GetAlphaRandomStart() const;
    float GetAlphaRandomEnd() const;

    float GetOpacityStartValue();
    float GetOpacityEndValue();
    ParticleUpdator GetOpacityUpdator();
    float GetOpacityRandomStart() const;
    float GetOpacityRandomEnd() const;

    float GetScaleStartValue();
    float GetScaleEndValue();
    ParticleUpdator GetScaleUpdator();
    float GetScaleRandomStart() const;
    float GetScaleRandomEnd() const;

    float GetSpinStartValue();
    float GetSpinEndValue();
    ParticleUpdator GetSpinUpdator();
    float GetSpinRandomStart() const;
    float GetSpinRandomEnd() const;

    void SetEmitConfig(const EmitterConfig& emitConfig);
    void SetParticleVelocity(const ParticleVelocity& velocity);
    void SetParticleAcceleration(const RenderParticleAcceleration& acceleration);
    void SetParticleColor(const RenderParticleColorParaType& color);
    void SetParticleOpacity(const RenderParticleParaType<float>& opacity);
    void SetParticleScale(const RenderParticleParaType<float>& scale);
    void SetParticleSpin(const RenderParticleParaType<float>& spin);
};

class RSB_EXPORT RSRenderParticle {
public:
    explicit RSRenderParticle(std::shared_ptr<ParticleRenderParams> particleParams);
    RSRenderParticle() = default;
    ~RSRenderParticle() = default;

    // Set methods
    void SetPosition(const Vector2f& position);
    void SetVelocity(const Vector2f& velocity);
    void SetAcceleration(const Vector2f& acceleration);
    void SetSpin(const float& spin);
    void SetOpacity(const float& opacity);
    void SetColor(const Color& color);
    void SetScale(const float& scale);
    void SetRadius(const float& radius);
    void SetImage(const std::shared_ptr<RSImage>& image);
    void SetImageSize(const Vector2f& imageSize);
    void SetParticleType(const ParticleType& particleType);
    void SetActiveTime(const int64_t& activeTime);

    // Get methods
    Vector2f GetPosition();
    Vector2f GetVelocity();
    Vector2f GetAcceleration();
    float GetSpin();
    float GetOpacity();
    Color GetColor();
    float GetScale();
    float GetRadius();
    std::shared_ptr<RSImage> GetImage();
    Vector2f GetImageSize();
    ParticleType GetParticleType();
    int64_t GetActiveTime();
    std::shared_ptr<ParticleRenderParams> GetParticleRenderParams();

    // Other methods
    void InitProperty(std::shared_ptr<ParticleRenderParams> particleParams);
    bool IsAlive() const;
    static float GetRandomValue(float min, float max);
    Vector2f CalculateParticlePosition(const ShapeType& emitShape, const Vector2f& position, const Vector2f& emitSize);
    Color Lerp(const Color& start, const Color& end, float t);
    std::shared_ptr<ParticleRenderParams> particleRenderParams_;

private:
    Vector2f position_;
    Vector2f velocity_;
    Vector2f acceleration_;
    float scale_;
    float spin_;
    float opacity_;
    Color color_;
    float radius_;
    std::shared_ptr<RSImage> image_;
    Vector2f imageSize_;
    ParticleType particleType_;
    int64_t activeTime_;
    int64_t lifeTime_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_PARTICLE_H
