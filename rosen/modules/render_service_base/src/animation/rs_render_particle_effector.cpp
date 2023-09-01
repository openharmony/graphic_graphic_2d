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
RSRenderParticleEffector::RSRenderParticleEffector(const std::shared_ptr<ParticleRenderParams> particleParams)
{
    particleParams_ = particleParams;
}

void RSRenderParticleEffector::UpdateColor(
    const std::shared_ptr<RSRenderParticle>& particle, float deltaTime, int64_t activeTime)
{
    auto colorUpdator = particleParams_->GetColorUpdator();
    activeTime /= NS_PER_MS;
    if (colorUpdator == ParticleUpdator::RANDOM) {
        Color color = particle->GetColor();
        int16_t red = color.GetRed();
        int16_t green = color.GetGreen();
        int16_t blue = color.GetBlue();
        int16_t alpha = color.GetAlpha();

        if (ROSEN_EQ(redSpeed_, 0.f) && ROSEN_EQ(greenSpeed_, 0.f) && ROSEN_EQ(blueSpeed_, 0.f) && ROSEN_EQ(alphaSpeed_, 0.f)) {
            redSpeed_ =
                RSRenderParticle::GetRandomValue(particleParams_->GetRedRandomStart(), particleParams_->GetRedRandomEnd());
            greenSpeed_ = RSRenderParticle::GetRandomValue(
                particleParams_->GetGreenRandomStart(), particleParams_->GetGreenRandomEnd());
            blueSpeed_ = RSRenderParticle::GetRandomValue(
                particleParams_->GetBlueRandomStart(), particleParams_->GetBlueRandomEnd());
            alphaSpeed_ = RSRenderParticle::GetRandomValue(
                particleParams_->GetAlphaRandomStart(), particleParams_->GetAlphaRandomEnd());
        }
        if (!((red <= 0 && redSpeed_ <= 0.f) || (red >= 255 && redSpeed_ >= 0.f))) {
            red += redSpeed_ * deltaTime;
            red = std::clamp<int16_t>(red, 0, UINT8_MAX);
            color.SetRed(red);
        }
        if (!((green <= 0 && greenSpeed_ <= 0.f) || (green >= 255 && greenSpeed_ >= 0.f))) {
            green += greenSpeed_ * deltaTime;
            green = std::clamp<int16_t>(green, 0, UINT8_MAX);
            color.SetGreen(green);
        }
        if (!((blue <= 0 && blueSpeed_ <= 0.f) || (blue >= 255 && blueSpeed_ >= 0.f))) {
            blue += blueSpeed_ * deltaTime;
            blue = std::clamp<int16_t>(blue, 0, UINT8_MAX);
            color.SetBlue(blue);
        }
        if (!((alpha <= 0 && alphaSpeed_ <= 0.f) || (alpha >= 255 && alphaSpeed_ >= 0.f))) {
            alpha += alphaSpeed_ * deltaTime;
            alpha = std::clamp<int16_t>(alpha, 0, UINT8_MAX);
            color.SetAlpha(alpha);
        }
        particle->SetColor(color);
    } else if (colorUpdator == ParticleUpdator::CURVE) {
        auto valChangeOverLife = particleParams_->color_.valChangeOverLife_;
        for (size_t i = 0; i < valChangeOverLife.size(); i++) {
            Color startValue = valChangeOverLife[i]->fromValue_;
            Color endValue = valChangeOverLife[i]->toValue_;
            int startTime = valChangeOverLife[i]->startMillis_;
            int endTime = valChangeOverLife[i]->endMillis_;
            auto interpolator = valChangeOverLife[i]->interpolator_;

            if (activeTime >= startTime && activeTime < endTime) {
                Color color = GenerateValue(startValue, endValue, startTime, endTime, activeTime, interpolator);
                int16_t red = std::clamp<int16_t>(color.GetRed(), 0, UINT8_MAX);
                int16_t green = std::clamp<int16_t>(color.GetGreen(), 0, UINT8_MAX);
                int16_t blue = std::clamp<int16_t>(color.GetBlue(), 0, UINT8_MAX);
                int16_t alpha = std::clamp<int16_t>(color.GetAlpha(), 0, UINT8_MAX);
                color.SetRed(red);
                color.SetGreen(green);
                color.SetBlue(blue);
                color.SetAlpha(alpha);
                particle->SetColor(color);
            }
        }
    }
}

void RSRenderParticleEffector::UpdateOpacity(
    const std::shared_ptr<RSRenderParticle>& particle, float deltaTime, int64_t activeTime)
{
    auto opacityUpdator = particleParams_->GetOpacityUpdator();
    activeTime /= NS_PER_MS;
    if (opacityUpdator == ParticleUpdator::RANDOM) {
        auto opacity = particle->GetOpacity();
        if (opacity <= 0 && particleParams_->GetOpacityRandomStart() <= 0 &&
            particleParams_->GetOpacityRandomEnd() <= 0) {
            particle->SetIsDead();
            return;
        }
        if (ROSEN_EQ(opacitySpeed_, 0.f)) {
            opacitySpeed_ = RSRenderParticle::GetRandomValue(
                particleParams_->GetOpacityRandomStart(), particleParams_->GetOpacityRandomEnd());
        }
        if ((opacity <= 0.f && opacitySpeed_ <= 0.f) || (opacity >= 1.0 && opacitySpeed_ >= 0.f)) {
            return;
        }
        opacity += opacitySpeed_ * deltaTime;
        opacity = std::clamp<float>(opacity, 0.f, 1.f);
        particle->SetOpacity(opacity);
    } else if (opacityUpdator == ParticleUpdator::CURVE) {
        auto valChangeOverLife = particleParams_->opacity_.valChangeOverLife_;
        for (size_t i = 0; i < valChangeOverLife.size(); i++) {
            float startValue = valChangeOverLife[i]->fromValue_;
            float endValue = valChangeOverLife[i]->toValue_;
            int startTime = valChangeOverLife[i]->startMillis_;
            int endTime = valChangeOverLife[i]->endMillis_;
            auto interpolator = valChangeOverLife[i]->interpolator_;
            if (activeTime >= startTime && activeTime < endTime) {
                float value = 0.f;
                if (!interpolator) {
                    value = GenerateValue(startValue, endValue, startTime, endTime, activeTime);
                } else {
                    value = GenerateValue(startValue, endValue, startTime, endTime, activeTime, interpolator);
                }
                value = std::clamp<float>(value, 0.f, 1.f);
                particle->SetOpacity(value);
            }
        }
    }
}

void RSRenderParticleEffector::UpdateScale(
    const std::shared_ptr<RSRenderParticle>& particle, float deltaTime, int64_t activeTime)
{
    auto scaleUpdator = particleParams_->GetScaleUpdator();
    activeTime /= NS_PER_MS;
    if (scaleUpdator == ParticleUpdator::RANDOM) {
        auto scale = particle->GetScale();
        if (scale <= 0 && particleParams_->GetScaleRandomStart() <= 0 && particleParams_->GetScaleRandomEnd() <= 0) {
            particle->SetIsDead();
            return;
        }
        if (ROSEN_EQ(scaleSpeed_, 0.f)) {
            scaleSpeed_ = RSRenderParticle::GetRandomValue(
                particleParams_->GetScaleRandomStart(), particleParams_->GetScaleRandomEnd());
        }
        if (scale <= 0.f && scaleSpeed_ <= 0.f) {
            return;
        }
        scale += scaleSpeed_ * deltaTime;
        particle->SetScale(scale);
    } else if (scaleUpdator == ParticleUpdator::CURVE) {
        auto valChangeOverLife = particleParams_->scale_.valChangeOverLife_;
        for (size_t i = 0; i < valChangeOverLife.size(); i++) {
            float startValue = valChangeOverLife[i]->fromValue_;
            float endValue = valChangeOverLife[i]->toValue_;
            int startTime = valChangeOverLife[i]->startMillis_;
            int endTime = valChangeOverLife[i]->endMillis_;
            auto interpolator = valChangeOverLife[i]->interpolator_;
            if (activeTime >= startTime && activeTime < endTime) {
                float value = 0.f;
                if (!interpolator) {
                    value = GenerateValue(startValue, endValue, startTime, endTime, activeTime);
                } else {
                    value = GenerateValue(startValue, endValue, startTime, endTime, activeTime, interpolator);
                }
                particle->SetScale(value);
            }
        }
    }
}

void RSRenderParticleEffector::UpdateSpin(
    const std::shared_ptr<RSRenderParticle>& particle, float deltaTime, int64_t activeTime)
{
    auto spinUpdator = particleParams_->GetSpinUpdator();
    activeTime /= NS_PER_MS;
    if (spinUpdator == ParticleUpdator::RANDOM) {
        if (ROSEN_EQ(spinSpeed_, 0.f)) {
            spinSpeed_ = RSRenderParticle::GetRandomValue(
                particleParams_->GetSpinRandomStart(), particleParams_->GetSpinRandomEnd());
        }
        auto spin = particle->GetSpin();
        spin += spinSpeed_ * deltaTime;
        particle->SetSpin(spin);
    } else if (spinUpdator == ParticleUpdator::CURVE) {
        auto valChangeOverLife = particleParams_->spin_.valChangeOverLife_;
        for (size_t i = 0; i < valChangeOverLife.size(); i++) {
            float startValue = valChangeOverLife[i]->fromValue_;
            float endValue = valChangeOverLife[i]->toValue_;
            int startTime = valChangeOverLife[i]->startMillis_;
            int endTime = valChangeOverLife[i]->endMillis_;
            auto interpolator = valChangeOverLife[i]->interpolator_;
            if (activeTime >= startTime && activeTime < endTime) {
                float value = 0.f;
                if (!interpolator) {
                    value = GenerateValue(startValue, endValue, startTime, endTime, activeTime);
                } else {
                    value = GenerateValue(startValue, endValue, startTime, endTime, activeTime, interpolator);
                }
                particle->SetSpin(value);
            }
        }
    }
}

void RSRenderParticleEffector::UpdateAccelerate(
    const std::shared_ptr<RSRenderParticle>& particle, float deltaTime, int64_t activeTime)
{
    auto acceValueUpdator = particleParams_->GetAccelerationValueUpdator();
    auto acceAngleUpdator = particleParams_->GetAccelerationAngleUpdator();
    float acceValueChange = 0.f;
    float acceAngleChange = 0.f;
    float value = 0.f;
    float angle = 0.f;
    activeTime /= NS_PER_MS;
    if (acceValueUpdator == ParticleUpdator::RANDOM) {
        float acceValueSpeed = RSRenderParticle::GetRandomValue(
            particleParams_->GetAccelRandomValueStart(), particleParams_->GetAccelRandomValueEnd());
        acceValueChange = acceValueSpeed * deltaTime;
    } else if (acceValueUpdator == ParticleUpdator::CURVE) {
        auto valChangeOverLife = particleParams_->acceleration_.accelerationValue_.valChangeOverLife_;
        for (size_t i = 0; i < valChangeOverLife.size(); i++) {
            float startValue = valChangeOverLife[i]->fromValue_;
            float endValue = valChangeOverLife[i]->toValue_;
            int startTime = valChangeOverLife[i]->startMillis_;
            int endTime = valChangeOverLife[i]->endMillis_;
            auto interpolator = valChangeOverLife[i]->interpolator_;
            if (activeTime >= startTime && activeTime < endTime) {
                if (!interpolator) {
                    value = GenerateValue(startValue, endValue, startTime, endTime, activeTime);
                } else {
                    value = GenerateValue(startValue, endValue, startTime, endTime, activeTime, interpolator);
                }
            }
        }
    }
    if (acceAngleUpdator == ParticleUpdator::RANDOM) {
        float acceAngleSpeed = RSRenderParticle::GetRandomValue(
            particleParams_->GetAccelRandomAngleStart(), particleParams_->GetAccelRandomAngleStart());
        acceAngleChange = acceAngleSpeed * deltaTime;
    } else if (acceAngleUpdator == ParticleUpdator::CURVE) {
        auto valChangeOverLife = particleParams_->acceleration_.accelerationAngle_.valChangeOverLife_;
        for (size_t i = 0; i < valChangeOverLife.size(); i++) {
            float startValue = valChangeOverLife[i]->fromValue_;
            float endValue = valChangeOverLife[i]->toValue_;
            int startTime = valChangeOverLife[i]->startMillis_;
            int endTime = valChangeOverLife[i]->endMillis_;
            auto interpolator = valChangeOverLife[i]->interpolator_;
            if (activeTime >= startTime && activeTime < endTime) {
                if (!interpolator) {
                    angle = GenerateValue(startValue, endValue, startTime, endTime, activeTime);
                } else {
                    angle = GenerateValue(startValue, endValue, startTime, endTime, activeTime, interpolator);
                }
            }
        }
    }
    if (acceValueUpdator == ParticleUpdator::RANDOM && acceAngleUpdator == ParticleUpdator::RANDOM) {
        auto acceleration = particle->GetAcceleration();
        acceleration.x_ += acceValueChange * std::cos(acceAngleChange);
        acceleration.y_ += acceValueChange * std::sin(acceAngleChange);
        particle->SetAcceleration(acceleration);
    } else if (acceValueUpdator == ParticleUpdator::CURVE && acceAngleUpdator == ParticleUpdator::CURVE) {
        particle->SetAcceleration({ value * std::cos(angle), value * std::sin(angle) });
    }
}

// Apply effector to particle
void RSRenderParticleEffector::ApplyEffectorToParticle(
    const std::shared_ptr<RSRenderParticle>& particle, int64_t deltaTime)
{
    int64_t activeTime = particle->GetActiveTime();
    float dt = static_cast<float>(deltaTime) / NS_TO_S;
    UpdateAccelerate(particle, dt, activeTime);
    UpdateColor(particle, dt, activeTime);
    UpdateOpacity(particle, dt, activeTime);
    UpdateScale(particle, dt, activeTime);
    UpdateSpin(particle, dt, activeTime);

    auto acceleration = particle->GetAcceleration();
    Vector2f velocity = particle->GetVelocity();
    if (!(ROSEN_EQ(acceleration.x_, 0.f) && ROSEN_EQ(acceleration.y_, 0.f))) {
        velocity.x_ += acceleration.x_ * dt;
        velocity.y_ += acceleration.y_ * dt;
        particle->SetVelocity(velocity);
    }
    Vector2f position = particle->GetPosition();
    if (!(ROSEN_EQ(velocity.x_, 0.f) && ROSEN_EQ(velocity.y_, 0.f))) {
        position.x_ += velocity.x_ * dt;
        position.y_ += velocity.y_ * dt;
        particle->SetPosition(position);
    }
    activeTime += deltaTime;
    particle->SetActiveTime(activeTime);
}

template<typename T>
T RSRenderParticleEffector::GenerateValue(
    T startValue, T endValue, int startTime, int endTime, int currentTime, std::shared_ptr<RSInterpolator> interpolator)
{
    float t = 0.f;
    if (endTime - startTime != 0) {
        t = static_cast<float>(currentTime - startTime) / static_cast<float>(endTime - startTime);
    }
    float fraction = interpolator->Interpolate(t);
    auto interpolationValue = startValue * (1.0f - fraction) + endValue * fraction;
    return interpolationValue;
}

float RSRenderParticleEffector::GenerateValue(
    float startValue, float endValue, int startTime, int endTime, int currentTime)
{
#ifdef ENABLE_RUST
    return generate_value(startValue, endValue, startTime, endTime, currentTime);
#else
    float t = 0.f;
    if (endTime - startTime != 0) {
        t = static_cast<float>(currentTime - startTime) / static_cast<float>(endTime - startTime);
    }
    auto interpolationValue = startValue * (1.0f - t) + endValue * t;
    return interpolationValue;
#endif
}

} // namespace Rosen
} // namespace OHOS
