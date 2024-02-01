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

#include "animation/rs_render_particle_effector.h"

#include <cmath>
#include <iostream>
#include <random>
#ifdef ENABLE_RUST
#include "cxx.h"
#include "lib.rs.h"
#endif

#include "animation/rs_value_estimator.h"
namespace OHOS {
namespace Rosen {
constexpr float DEGREE_TO_RADIAN = M_PI / 180;
RSRenderParticleEffector::RSRenderParticleEffector(const std::vector<std::shared_ptr<RSRenderParticle>> activeParticles)
{
    activeParticles_ = activeParticles;
}

Vector4<int16_t> RSRenderParticleEffector::CalculateColorInt(
    const std::shared_ptr<RSRenderParticle>& particle, Vector4<int16_t> colorInt,
    Vector4<float> colorF, Vector4<float> colorSpeed, float deltaTime)
{
    for (int i = 0; i < 4; i ++) { // 4 is color tunnel count
        if (!((colorInt.data_[i] <= 0 && colorSpeed.data_[i] <= 0.f)
            || (colorInt.data_[i] >= UINT8_MAX && colorSpeed.data_[i] >= 0.f))) {
            colorF.data_[i] += colorSpeed.data_[i] * deltaTime;
            if (std::abs(colorF.data_[i]) >= 1.f) {
                colorInt.data_[i] += static_cast<int16_t>(colorF.data_[i]);
                colorF.data_[i] -= std::floor(colorF.data_[i]);
            }
            particle->SetRedF(colorF.data_[i]);
            colorInt.data_[i] = std::clamp<int16_t>(colorInt.data_[i], 0, UINT8_MAX);
        }
    }
    return colorInt;
}

float RSRenderParticleEffector::UpdateCurveValue(
    const std::vector<std::shared_ptr<ChangeInOverLife<float>>>& valChangeOverLife, int64_t activeTime)
{
    float value = 0.f;
    for (size_t i = 0; i < valChangeOverLife.size(); i++) {
        int startTime = valChangeOverLife[i]->startMillis_;
        int endTime = valChangeOverLife[i]->endMillis_;
        if (activeTime < startTime || activeTime > endTime) {
            continue;
        }
        float startValue = valChangeOverLife[i]->fromValue_;
        float endValue = valChangeOverLife[i]->toValue_;
#ifdef ENABLE_RUST
        value = generate_value(startValue, endValue, startTime, endTime, activeTime);
#else
        float t = 0.f;
        if (endTime - startTime != 0) {
            t = static_cast<float>(activeTime - startTime) / static_cast<float>(endTime - startTime);
            auto interpolator = valChangeOverLife[i]->interpolator_;
            t = (interpolator != nullptr) ? interpolator->Interpolate(t) : t;
            value = startValue * (1.0f - t) + endValue * t;
        } else {
            value = startValue;
        }
#endif
    }
    return value;
}

Color RSRenderParticleEffector::UpdateColorCurveValue(
    const std::vector<std::shared_ptr<ChangeInOverLife<Color>>>& valChangeOverLife, int64_t activeTime)
{
    Color color;
    for (size_t i = 0; i < valChangeOverLife.size(); i++) {
        int startTime = valChangeOverLife[i]->startMillis_;
        int endTime = valChangeOverLife[i]->endMillis_;
        if (activeTime < startTime || activeTime > endTime) {
            continue;
        }
        Color startValue = valChangeOverLife[i]->fromValue_;
        Color endValue = valChangeOverLife[i]->toValue_;
        float t = 0.f;
        if (endTime - startTime != 0) {
            t = static_cast<float>(activeTime - startTime) / static_cast<float>(endTime - startTime);
            auto interpolator = valChangeOverLife[i]->interpolator_;
            t = (interpolator != nullptr) ? interpolator->Interpolate(t) : t;
            auto interpolationValue = startValue * (1.0f - t) + endValue * t;
            color = interpolationValue;
        } else {
            color = startValue;
        }
        color = Color(color.AsArgbInt());
    }
    return color;
}

void RSRenderParticleEffector::UpdateColor(float deltaTime)
{
    for (auto &particle : activeParticles_) {
        if (particle == nullptr) {
            return;
        }
        if (particle->GetParticleType() == ParticleType::IMAGES) {
            return;
        }
        auto colorUpdator = particle->GetColorUpdator();
        Color color = particle->GetColor();
        if (colorUpdator == ParticleUpdator::RANDOM) {
            auto colorInt = Vector4<int16_t>(color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha());
            auto colorSpeed = Vector4f(particle->GetRedSpeed(), particle->GetGreenSpeed(),
                particle->GetBlueSpeed(), particle->GetAlphaSpeed());
            auto colorF =  Vector4f(particle->GetRedF(), particle->GetGreenF(),
                particle->GetBlueF(), particle->GetAlphaF());
            colorInt = CalculateColorInt(particle, colorInt, colorF, colorSpeed, deltaTime);
            color.SetRed(colorInt.x_);
            color.SetGreen(colorInt.y_);
            color.SetBlue(colorInt.z_);
            color.SetAlpha(colorInt.w_);
        } else if (colorUpdator == ParticleUpdator::CURVE) {
            int64_t activeTime = particle->GetActiveTime() / NS_PER_MS;
            auto valChangeOverLife = particle->GetColorChangeOverLife();
            color = UpdateColorCurveValue(valChangeOverLife, activeTime);
        }
        particle->SetColor(color);
    }
}

void RSRenderParticleEffector::UpdateOpacity(float deltaTime)
{
    for (auto &particle : activeParticles_) {
        if (particle == nullptr) {
            return;
        }
        auto opacityUpdator = particle->GetOpacityUpdator();
        auto opacity = particle->GetOpacity();
        if (opacityUpdator == ParticleUpdator::RANDOM) {
            auto opacitySpeed = particle->GetOpacitySpeed();
            if (opacity <= 0 && opacitySpeed <= 0) {
                particle->SetIsDead();
                return;
            }
            if (opacity >= 1.0 && opacitySpeed >= 0.f) {
                return;
            }
            opacity += opacitySpeed * deltaTime;
            opacity = std::clamp<float>(opacity, 0.f, 1.f);
        } else if (opacityUpdator == ParticleUpdator::CURVE) {
            int64_t activeTime = particle->GetActiveTime() / NS_PER_MS;
            auto valChangeOverLife = particle->GetOpacityChangeOverLife();
            opacity = UpdateCurveValue(valChangeOverLife, activeTime);
        }
        particle->SetOpacity(opacity);
    }
}

void RSRenderParticleEffector::UpdateScale(float deltaTime)
{
    for (auto &particle : activeParticles_) {
        if (particle == nullptr) {
            return;
        }
        auto scaleUpdator = particle->GetScaleUpdator();
        auto scale = particle->GetScale();
        if (scaleUpdator == ParticleUpdator::RANDOM) {
            auto scaleSpeed = particle->GetScaleSpeed();
            if (scale <= 0 && scaleSpeed <= 0) {
                particle->SetIsDead();
                return;
            }
            scale += scaleSpeed * deltaTime;
        } else if (scaleUpdator == ParticleUpdator::CURVE) {
            int64_t activeTime = particle->GetActiveTime() / NS_PER_MS;
            auto valChangeOverLife = particle->GetScaleChangeOverLife();
            scale = UpdateCurveValue(valChangeOverLife, activeTime);
        }
        particle->SetScale(scale);
    }
}

void RSRenderParticleEffector::UpdateSpin(float deltaTime)
{
    for (auto &particle : activeParticles_) {
        if (particle == nullptr) {
            return;
        }
        auto spinUpdator = particle->GetSpinUpdator();
        auto spin = particle->GetSpin();
        if (spinUpdator == ParticleUpdator::RANDOM) {
            auto spinSpeed = particle->GetSpinSpeed();
            spin += spinSpeed * deltaTime;
        } else if (spinUpdator == ParticleUpdator::CURVE) {
            int64_t activeTime = particle->GetActiveTime() / NS_PER_MS;
            auto valChangeOverLife = particle->GetSpinChangeOverLife();
            spin = UpdateCurveValue(valChangeOverLife, activeTime);
        }
        particle->SetSpin(spin);
    }
}

void RSRenderParticleEffector::UpdateAccelerationAngle(float deltaTime)
{
    for (auto &particle : activeParticles_) {
        if (particle == nullptr) {
            return;
        }
        auto accelerationAngle = particle->GetAccelerationAngle();
        auto acceAngleUpdator = particle->GetAccelerationAngleUpdator();
        if (acceAngleUpdator == ParticleUpdator::RANDOM) {
            float acceAngleSpeed = particle->GetAccelerationAngleSpeed();
            accelerationAngle += acceAngleSpeed * deltaTime;
        } else if (acceAngleUpdator == ParticleUpdator::CURVE) {
            int64_t activeTime = particle->GetActiveTime() / NS_PER_MS;
            auto valChangeOverLife = particle->GetAcceAngChangeOverLife();
            accelerationAngle = UpdateCurveValue(valChangeOverLife, activeTime);
        }
        particle->SetAccelerationAngle(accelerationAngle);
    }
}

void RSRenderParticleEffector::UpdateAccelerationValue(float deltaTime)
{
    for (auto &particle : activeParticles_) {
        if (particle == nullptr) {
            return;
        }
        auto accelerationValue = particle->GetAccelerationValue();
        auto acceValueUpdator = particle->GetAccelerationValueUpdator();
        if (acceValueUpdator == ParticleUpdator::RANDOM) {
            float acceValueSpeed = particle->GetAccelerationValueSpeed();
            accelerationValue += acceValueSpeed * deltaTime;
        } else if (acceValueUpdator == ParticleUpdator::CURVE) {
            int64_t activeTime = particle->GetActiveTime() / NS_PER_MS;
            auto valChangeOverLife = particle->GetAcceValChangeOverLife();
            accelerationValue = UpdateCurveValue(valChangeOverLife, activeTime);
        }
        particle->SetAccelerationValue(accelerationValue);
    }
}

void RSRenderParticleEffector::UpdatePosition(float deltaTime)
{
    for (auto &particle : activeParticles_) {
        if (particle == nullptr) {
            return;
        }
        auto accelerationValue = particle->GetAccelerationValue();
        auto accelerationAngle = particle->GetAccelerationAngle();
        accelerationAngle *= DEGREE_TO_RADIAN;
        auto acceleration = Vector2f { accelerationValue * std::cos(accelerationAngle),
                accelerationValue * std::sin(accelerationAngle) };
        particle->SetAcceleration(acceleration);

        Vector2f velocity = particle->GetVelocity();
        if (!(ROSEN_EQ(acceleration.x_, 0.f) && ROSEN_EQ(acceleration.y_, 0.f))) {
            velocity.x_ += acceleration.x_ * deltaTime;
            velocity.y_ += acceleration.y_ * deltaTime;
            particle->SetVelocity(velocity);
        }

        Vector2f position = particle->GetPosition();
        if (!(ROSEN_EQ(velocity.x_, 0.f) && ROSEN_EQ(velocity.y_, 0.f))) {
            position.x_ += velocity.x_ * deltaTime;
            position.y_ += velocity.y_ * deltaTime;
            particle->SetPosition(position);
        }
    }
}

void RSRenderParticleEffector::UpdateActiveTime(int64_t deltaTime)
{
    for (auto &particle : activeParticles_) {
        if (particle != nullptr) {
            int64_t activeTime = particle->GetActiveTime();
            activeTime += deltaTime;
            particle->SetActiveTime(activeTime);
        }
    }
}

// Apply effector to particle
void RSRenderParticleEffector::ApplyEffectorToParticle(int64_t deltaTime)
{
    float dt = static_cast<float>(deltaTime) / NS_TO_S;
    UpdateAccelerationValue(dt);
    UpdateAccelerationAngle(dt);
    UpdateColor(dt);
    UpdateOpacity(dt);
    UpdateScale(dt);
    UpdateSpin(dt);
    UpdatePosition(dt);
    UpdateActiveTime(deltaTime);
}

void RSRenderParticleEffector::Update(
    const std::vector<std::shared_ptr<RSRenderParticle>>& activeParticles, int64_t deltaTime)
{
    activeParticles_ = activeParticles;
    ApplyEffectorToParticle(deltaTime);
}

} // namespace Rosen
} // namespace OHOS
