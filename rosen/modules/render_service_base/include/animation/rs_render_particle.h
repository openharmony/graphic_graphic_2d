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
#include <utility>
#include <vector>

#include "pixel_map.h"

#include "animation/rs_interpolator.h"
#include "common/rs_color.h"
#include "common/rs_color_palette.h"
#include "common/rs_macros.h"
#include "common/rs_vector2.h"
#include "render/rs_image.h"

namespace OHOS {
namespace Rosen {
enum class ParticleUpdator: uint32_t { NONE = 0, RANDOM, CURVE };

enum class ShapeType: uint32_t { RECT = 0, CIRCLE, ELLIPSE };

enum class ParticleType: uint32_t {
    POINTS = 0,
    IMAGES
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
    std::shared_ptr<RSInterpolator> interpolator_ = nullptr;
    ChangeInOverLife() : fromValue_(), toValue_(), startMillis_(), endMillis_(), interpolator_() {}
    ChangeInOverLife(const T& fromValue, const T& toValue, const int& startMillis, const int& endMillis,
        std::shared_ptr<RSInterpolator> interpolator)
        : fromValue_(fromValue), toValue_(toValue), startMillis_(startMillis), endMillis_(endMillis),
          interpolator_(std::move(interpolator))
    {}
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
        : val_(val), updator_(updator), random_(random), valChangeOverLife_(valChangeOverLife)
    {}
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
    int32_t particleCount_;
    Range<int64_t> lifeTime_;
    ParticleType type_;
    float radius_;
    std::shared_ptr<RSImage> image_;
    Vector2f imageSize_;

    EmitterConfig()
        : emitRate_(), emitShape_(ShapeType::RECT), position_(), emitSize_(), particleCount_(), lifeTime_(),
          type_(ParticleType::POINTS), radius_(), image_(), imageSize_()
    {}
    EmitterConfig(const int& emitRate, const ShapeType& emitShape, const Vector2f& position, const Vector2f& emitSize,
        const int32_t& particleCount, const Range<int64_t>& lifeTime, const ParticleType& type, const float& radius,
        std::shared_ptr<RSImage> image, Vector2f imageSize)
        : emitRate_(emitRate), emitShape_(emitShape), position_(position), emitSize_(emitSize),
          particleCount_(particleCount), lifeTime_(lifeTime), type_(type), radius_(radius), image_(std::move(image)),
          imageSize_(imageSize)
    {
        if (image_ != nullptr) {
            if (const auto& pixelMap = image_->GetPixelMap()) {
                image_->SetDstRect(RectF(position_.x_, position_.y_, static_cast<float>(pixelMap->GetWidth()),
                    static_cast<float>(pixelMap->GetHeight())));
            }
        }
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
        : velocityValue_(velocityValue), velocityAngle_(velocityAngle)
    {}
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
        : accelerationValue_(accelerationValue), accelerationAngle_(accelerationAngle)
    {}
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
        const Range<float>& alphaRandom, std::vector<std::shared_ptr<ChangeInOverLife<Color>>> valChangeOverLife)
        : colorVal_(colorVal), updator_(updator), redRandom_(redRandom), greenRandom_(greenRandom),
          blueRandom_(blueRandom), alphaRandom_(alphaRandom), valChangeOverLife_(std::move(valChangeOverLife))
    {}
    RenderParticleColorParaType()
        : colorVal_(), updator_(ParticleUpdator::NONE), redRandom_(), greenRandom_(), blueRandom_(), alphaRandom_()
    {}
    RenderParticleColorParaType(const RenderParticleColorParaType& color) = default;
    RenderParticleColorParaType& operator=(const RenderParticleColorParaType& color) = default;
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
        : emitterConfig_(emitterConfig), velocity_(velocity), acceleration_(acceleration), color_(color),
          opacity_(opacity), scale_(scale), spin_(spin)
    {}
    ParticleRenderParams() : emitterConfig_(), velocity_(), acceleration_(), color_(), opacity_(), scale_(), spin_() {};
    ParticleRenderParams(const ParticleRenderParams& params) = default;
    ParticleRenderParams& operator=(const ParticleRenderParams& params) = default;
    ~ParticleRenderParams() = default;

    int GetEmitRate() const;
    const ShapeType& GetEmitShape() const;
    const Vector2f& GetEmitPosition() const;
    const Vector2f& GetEmitSize() const;
    int32_t GetParticleCount() const;
    int64_t GetLifeTimeStartValue() const;
    int64_t GetLifeTimeEndValue() const;
    const ParticleType& GetParticleType() const;
    float GetParticleRadius() const;
    const std::shared_ptr<RSImage>& GetParticleImage();
    const Vector2f& GetImageSize() const;

    float GetVelocityStartValue() const;
    float GetVelocityEndValue() const;
    float GetVelocityStartAngle() const;
    float GetVelocityEndAngle() const;

    float GetAccelerationStartValue() const;
    float GetAccelerationEndValue() const;
    float GetAccelerationStartAngle() const;
    float GetAccelerationEndAngle() const;
    const ParticleUpdator& GetAccelerationValueUpdator();
    const ParticleUpdator& GetAccelerationAngleUpdator();
    float GetAccelRandomValueStart() const;
    float GetAccelRandomValueEnd() const;
    float GetAccelRandomAngleStart() const;
    float GetAccelRandomAngleEnd() const;

    const Color& GetColorStartValue();
    const Color& GetColorEndValue();
    const ParticleUpdator& GetColorUpdator();
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
    const ParticleUpdator& GetOpacityUpdator();
    float GetOpacityRandomStart() const;
    float GetOpacityRandomEnd() const;

    float GetScaleStartValue();
    float GetScaleEndValue();
    const ParticleUpdator& GetScaleUpdator();
    float GetScaleRandomStart() const;
    float GetScaleRandomEnd() const;

    float GetSpinStartValue();
    float GetSpinEndValue();
    const ParticleUpdator& GetSpinUpdator();
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
    explicit RSRenderParticle(const std::shared_ptr<ParticleRenderParams>& particleParams);
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
    void SetAccelerationValue(float accelerationValue);
    void SetAccelerationAngle(float accelerationAngle);
    void SetRedF(float redF);
    void SetGreenF(float greenF);
    void SetBlueF(float blueF);
    void SetAlphaF(float alphaF);

    // Get methods
    const Vector2f& GetPosition();
    const Vector2f& GetVelocity();
    const Vector2f& GetAcceleration();
    float GetSpin();
    float GetOpacity();
    const Color& GetColor();
    float GetScale();
    float GetRadius();
    float GetAccelerationValue();
    float GetAccelerationAngle();
    float GetRedSpeed();
    float GetGreenSpeed();
    float GetBlueSpeed();
    float GetAlphaSpeed();
    float GetOpacitySpeed();
    float GetScaleSpeed();
    float GetSpinSpeed();
    float GetAccelerationValueSpeed();
    float GetAccelerationAngleSpeed();
    float GetRedF();
    float GetGreenF();
    float GetBlueF();
    float GetAlphaF();
    const std::shared_ptr<RSImage>& GetImage();
    const Vector2f& GetImageSize();
    const ParticleType& GetParticleType();
    int64_t GetActiveTime();
    const std::shared_ptr<ParticleRenderParams>& GetParticleRenderParams();

    const ParticleUpdator& GetAccelerationValueUpdator();
    const ParticleUpdator& GetAccelerationAngleUpdator();
    const ParticleUpdator& GetColorUpdator();
    const ParticleUpdator& GetOpacityUpdator();
    const ParticleUpdator& GetScaleUpdator();
    const ParticleUpdator& GetSpinUpdator();
    const std::vector<std::shared_ptr<ChangeInOverLife<float>>>& GetAcceValChangeOverLife();
    const std::vector<std::shared_ptr<ChangeInOverLife<float>>>& GetAcceAngChangeOverLife();
    const std::vector<std::shared_ptr<ChangeInOverLife<float>>>& GetOpacityChangeOverLife();
    const std::vector<std::shared_ptr<ChangeInOverLife<float>>>& GetScaleChangeOverLife();
    const std::vector<std::shared_ptr<ChangeInOverLife<float>>>& GetSpinChangeOverLife();
    const std::vector<std::shared_ptr<ChangeInOverLife<Color>>>& GetColorChangeOverLife();

    // Other methods
    void InitProperty();
    bool IsAlive() const;
    void SetIsDead();
    static float GetRandomValue(float min, float max);
    Vector2f CalculateParticlePosition(const ShapeType& emitShape, const Vector2f& position, const Vector2f& emitSize);
    Color Lerp(const Color& start, const Color& end, float t);
    std::shared_ptr<ParticleRenderParams> particleParams_;

    bool operator==(const RSRenderParticle& rhs)
    {
        return (position_ == rhs.position_) && (velocity_ == rhs.velocity_) &&
               (acceleration_ == rhs.acceleration_) && (scale_ == rhs.scale_) && (spin_ == rhs.spin_) &&
               (opacity_ == rhs.opacity_) && (color_ == rhs.color_) && (radius_ == rhs.radius_) &&
               (particleType_ == rhs.particleType_) && (activeTime_ == rhs.activeTime_) &&
               (lifeTime_ == rhs.lifeTime_) && (imageSize_ == rhs.imageSize_);
    }

private:
    Vector2f position_ = {0.f, 0.f};
    Vector2f velocity_ = {0.f, 0.f};
    Vector2f acceleration_ = {0.f, 0.f};
    float scale_ = 1.f;
    float spin_ = 0.f;
    float opacity_ = 1.f;
    Color color_ = RgbPalette::White();
    float radius_ = 0.f;
    std::shared_ptr<RSImage> image_;
    Vector2f imageSize_;
    ParticleType particleType_ = ParticleType::POINTS;
    int64_t activeTime_ = 0;
    int64_t lifeTime_ = 0;
    bool dead_ = false;
    float accelerationValue_ = 0.f;
    float accelerationAngle_ = 0.f;
    float redSpeed_ = 0.f;
    float greenSpeed_ = 0.f;
    float blueSpeed_ = 0.f;
    float alphaSpeed_ = 0.f;
    float opacitySpeed_ = 0.f;
    float scaleSpeed_ = 0.f;
    float spinSpeed_ = 0.f;
    float accelerationValueSpeed_ = 0.f;
    float accelerationAngleSpeed_ = 0.f;
    float redF_ = 0.f;
    float greenF_ = 0.f;
    float blueF_ = 0.f;
    float alphaF_ = 0.f;
};

class RSB_EXPORT RSRenderParticleVector {
public:
    explicit RSRenderParticleVector(std::vector<std::shared_ptr<RSRenderParticle>>&& renderParticleVector)
        : renderParticleVector_(std::move(renderParticleVector))
    {}
    RSRenderParticleVector() = default;
    ~RSRenderParticleVector() = default;
    RSRenderParticleVector(const RSRenderParticleVector& other) : renderParticleVector_(other.renderParticleVector_) {}

    RSRenderParticleVector& operator=(const RSRenderParticleVector& other)
    {
        if (this != &other) {
            renderParticleVector_ = other.renderParticleVector_;
        }
        return *this;
    }

    int GetParticleSize() const
    {
        return renderParticleVector_.size();
    }

    const std::vector<std::shared_ptr<RSRenderParticle>>& GetParticleVector() const
    {
        return renderParticleVector_;
    }

    bool operator==(const RSRenderParticleVector& rhs) const
    {
        bool equal = false;
        if (this->renderParticleVector_.size() != rhs.renderParticleVector_.size()) {
            return false;
        }
        if (this->renderParticleVector_.size() == 0) {
            return true;
        }
        for (size_t i = 0; i < this->renderParticleVector_.size(); i++) {
            equal = equal && (this->renderParticleVector_[i] == rhs.renderParticleVector_[i]) &&
                    (*(this->renderParticleVector_[i]) == *(rhs.renderParticleVector_[i]));
        }
        return equal;
    }

    friend class RSRenderParticleAnimation;
private:
    std::vector<std::shared_ptr<RSRenderParticle>> renderParticleVector_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_PARTICLE_H
