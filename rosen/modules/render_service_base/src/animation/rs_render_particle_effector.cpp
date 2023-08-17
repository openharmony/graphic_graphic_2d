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
        float redSpeed =
            RSRenderParticle::GetRandomValue(particleParams_->GetRedRandomStart(), particleParams_->GetRedRandomEnd());
        float greenSpeed = RSRenderParticle::GetRandomValue(
            particleParams_->GetGreenRandomStart(), particleParams_->GetGreenRandomEnd());
        float blueSpeed = RSRenderParticle::GetRandomValue(
            particleParams_->GetBlueRandomStart(), particleParams_->GetBlueRandomEnd());
        float alphaSpeed = RSRenderParticle::GetRandomValue(
            particleParams_->GetAlphaRandomStart(), particleParams_->GetAlphaRandomEnd());
        Color color = particle->GetColor();
        int16_t red = color.GetRed() + redSpeed * deltaTime;
        red = std::clamp<int16_t>(red, 0, UINT8_MAX);
        int16_t green = color.GetGreen() + greenSpeed * deltaTime;
        green = std::clamp<int16_t>(green, 0, UINT8_MAX);
        int16_t blue = color.GetBlue() + blueSpeed * deltaTime;
        blue = std::clamp<int16_t>(blue, 0, UINT8_MAX);
        int16_t alpha = color.GetAlpha() + alphaSpeed * deltaTime;
        alpha = std::clamp<int16_t>(alpha, 0, UINT8_MAX);
        color.SetRed(red);
        color.SetGreen(green);
        color.SetBlue(blue);
        color.SetAlpha(alpha);
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
        float opacitySpeed = RSRenderParticle::GetRandomValue(
            particleParams_->GetOpacityRandomStart(), particleParams_->GetOpacityRandomEnd());
        auto opacity = particle->GetOpacity();
        opacity += opacitySpeed * deltaTime;
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
        float scaleSpeed = RSRenderParticle::GetRandomValue(
            particleParams_->GetScaleRandomStart(), particleParams_->GetScaleRandomEnd());
        auto scale = particle->GetScale();
        scale += scaleSpeed * deltaTime;
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
        float spinSpeed = RSRenderParticle::GetRandomValue(
            particleParams_->GetSpinRandomStart(), particleParams_->GetSpinRandomEnd());
        auto spin = particle->GetSpin();
        spin += spinSpeed * deltaTime;
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
    velocity.x_ += acceleration.x_ * dt;
    velocity.y_ += acceleration.y_ * dt;
    Vector2f position = particle->GetPosition();
    position.x_ += velocity.x_ * dt;
    position.y_ += velocity.y_ * dt;
    particle->SetVelocity(velocity);
    particle->SetPosition(position);
    float opacity = particle->GetOpacity();
    Color color = particle->GetColor();
    color.SetAlpha(color.GetAlpha() * opacity);

    auto scale = particle->GetScale();
    if (particle->GetParticleType() == ParticleType::IMAGES) {
        std::shared_ptr<RSImage> image = particle->GetImage();
        image->SetScale(scale);
        particle->SetImage(image);
    }
    activeTime += deltaTime;
    particle->SetActiveTime(activeTime);
}

template<typename T>
T RSRenderParticleEffector::GenerateValue(
    T startValue, T endValue, int startTime, int endTime, int currentTime, std::shared_ptr<RSInterpolator> interpolator)
{
    float t = static_cast<float>(currentTime - startTime) / static_cast<float>(endTime - startTime);
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
    float t = static_cast<float>(currentTime - startTime) / static_cast<float>(endTime - startTime);
    auto interpolationValue = startValue * (1.0f - t) + endValue * t;
    return interpolationValue;
#endif
}

} // namespace Rosen
} // namespace OHOS
