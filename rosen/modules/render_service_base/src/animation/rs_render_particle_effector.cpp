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
RSRenderParticleEffector::RSRenderParticleEffector(const std::shared_ptr<ParticleRenderParams> particleParams)
{
    particleParams_ = particleParams;
}

int16_t RSRenderParticleEffector::CalculateRedInt(
    const std::shared_ptr<RSRenderParticle>& particle, int16_t redInt, float redF, float redSpeed, float deltaTime)
{
    if (!((redInt <= 0 && redSpeed <= 0.f) || (redInt >= UINT8_MAX && redSpeed >= 0.f))) {
        redF += redSpeed * deltaTime;
        if (std::abs(redF) >= 1.f) {
            redInt += static_cast<int16_t>(redF);
            redF -= std::floor(redF);
        }
        particle->SetRedF(redF);
        redInt = std::clamp<int16_t>(redInt, 0, UINT8_MAX);
    }
    return redInt;
}

int16_t RSRenderParticleEffector::CalculateGreenInt(const std::shared_ptr<RSRenderParticle>& particle, int16_t greenInt,
    float greenF, float greenSpeed, float deltaTime)
{
    if (!((greenInt <= 0 && greenSpeed <= 0.f) || (greenInt >= UINT8_MAX && greenSpeed >= 0.f))) {
        greenF += greenSpeed * deltaTime;
        if (std::abs(greenF) >= 1.f) {
            greenInt += static_cast<int16_t>(greenF);
            greenF -= std::floor(greenF);
        }
        particle->SetGreenF(greenF);
        greenInt = std::clamp<int16_t>(greenInt, 0, UINT8_MAX);
    }
    return greenInt;
}

int16_t RSRenderParticleEffector::CalculateBlueInt(
    const std::shared_ptr<RSRenderParticle>& particle, int16_t blueInt, float blueF, float blueSpeed, float deltaTime)
{
    if (!((blueInt <= 0 && blueSpeed <= 0.f) || (blueInt >= UINT8_MAX && blueSpeed >= 0.f))) {
        blueF += blueSpeed * deltaTime;
        if (std::abs(blueF) >= 1.f) {
            blueInt += static_cast<int16_t>(blueF);
            blueF -= std::floor(blueF);
        }
        particle->SetBlueF(blueF);
        blueInt = std::clamp<int16_t>(blueInt, 0, UINT8_MAX);
    }
    return blueInt;
}

int16_t RSRenderParticleEffector::CalculateAlphaInt(const std::shared_ptr<RSRenderParticle>& particle, int16_t alphaInt,
    float alphaF, float alphaSpeed, float deltaTime)
{
    if (!((alphaInt <= 0 && alphaSpeed <= 0.f) || (alphaInt >= UINT8_MAX && alphaSpeed >= 0.f))) {
        alphaF += alphaSpeed * deltaTime;
        if (std::abs(alphaF) >= 1.f) {
            alphaInt += static_cast<int16_t>(alphaF);
            alphaF -= std::floor(alphaF);
        }
        particle->SetAlphaF(alphaF);
        alphaInt = std::clamp<int16_t>(alphaInt, 0, UINT8_MAX);
    }
    return alphaInt;
}

Color RSRenderParticleEffector::UpdateColorRandom(
    const std::shared_ptr<RSRenderParticle>& particle, float deltaTime, Color color)
{
    int16_t redInt = color.GetRed();
    int16_t greenInt = color.GetGreen();
    int16_t blueInt = color.GetBlue();
    int16_t alphaInt = color.GetAlpha();
    float redSpeed = particle->GetRedSpeed();
    float greenSpeed = particle->GetGreenSpeed();
    float blueSpeed = particle->GetBlueSpeed();
    float alphaSpeed = particle->GetAlphaSpeed();
    float redF = particle->GetRedF();
    float greenF = particle->GetGreenF();
    float blueF = particle->GetBlueF();
    float alphaF = particle->GetAlphaF();

    redInt = CalculateRedInt(particle, redInt, redF, redSpeed, deltaTime);
    greenInt = CalculateGreenInt(particle, greenInt, greenF, greenSpeed, deltaTime);
    blueInt = CalculateBlueInt(particle, blueInt, blueF, blueSpeed, deltaTime);
    alphaInt = CalculateAlphaInt(particle, alphaInt, alphaF, alphaSpeed, deltaTime);
    color.SetRed(redInt);
    color.SetGreen(greenInt);
    color.SetBlue(blueInt);
    color.SetAlpha(alphaInt);
    return color;
}

Color RSRenderParticleEffector::UpdateColorCurve(int64_t activeTime, Color color)
{
    auto valChangeOverLife = particleParams_->color_.valChangeOverLife_;
    for (size_t i = 0; i < valChangeOverLife.size(); i++) {
        Color startValue = valChangeOverLife[i]->fromValue_;
        Color endValue = valChangeOverLife[i]->toValue_;
        int startTime = valChangeOverLife[i]->startMillis_;
        int endTime = valChangeOverLife[i]->endMillis_;
        auto interpolator = valChangeOverLife[i]->interpolator_;

        if (activeTime >= startTime && activeTime < endTime) {
            color = GenerateValue(startValue, endValue, startTime, endTime, activeTime, interpolator);
            int16_t red = std::clamp<int16_t>(color.GetRed(), 0, UINT8_MAX);
            int16_t green = std::clamp<int16_t>(color.GetGreen(), 0, UINT8_MAX);
            int16_t blue = std::clamp<int16_t>(color.GetBlue(), 0, UINT8_MAX);
            int16_t alpha = std::clamp<int16_t>(color.GetAlpha(), 0, UINT8_MAX);
            color.SetRed(red);
            color.SetGreen(green);
            color.SetBlue(blue);
            color.SetAlpha(alpha);
        }
    }
    return color;
}

void RSRenderParticleEffector::UpdateColor(
    const std::shared_ptr<RSRenderParticle>& particle, float deltaTime, int64_t activeTime)
{
    auto colorUpdator = particleParams_->GetColorUpdator();
    activeTime /= NS_PER_MS;
    Color color = particle->GetColor();
    if (colorUpdator == ParticleUpdator::RANDOM) {
        color = UpdateColorRandom(particle, deltaTime, color);
    } else if (colorUpdator == ParticleUpdator::CURVE) {
        color = UpdateColorCurve(activeTime, color);
    }
    particle->SetColor(color);
}

void RSRenderParticleEffector::UpdateOpacity(
    const std::shared_ptr<RSRenderParticle>& particle, float deltaTime, int64_t activeTime)
{
    auto opacityUpdator = particleParams_->GetOpacityUpdator();
    activeTime /= NS_PER_MS;
    if (opacityUpdator == ParticleUpdator::RANDOM) {
        auto opacity = particle->GetOpacity();
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
        auto scaleSpeed = particle->GetScaleSpeed();
        if (scale <= 0 && scaleSpeed <= 0) {
            particle->SetIsDead();
            return;
        }
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
        auto spin = particle->GetSpin();
        auto spinSpeed = particle->GetSpinSpeed();
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

void RSRenderParticleEffector::UpdateAccelerationAngle(
    const std::shared_ptr<RSRenderParticle>& particle, float deltaTime, int64_t activeTime)
{
    auto accelerationAngle = particle->GetAccelerationAngle();
    auto acceAngleUpdator = particleParams_->GetAccelerationAngleUpdator();
    float acceAngleChange = 0.f;
    if (acceAngleUpdator == ParticleUpdator::RANDOM) {
        float acceAngleSpeed = particle->GetAccelerationAngleSpeed();
        acceAngleChange = acceAngleSpeed * deltaTime;
        accelerationAngle += acceAngleChange;
    } else if (acceAngleUpdator == ParticleUpdator::CURVE) {
        auto valChangeOverLife = particleParams_->acceleration_.accelerationAngle_.valChangeOverLife_;
        for (size_t i = 0; i < valChangeOverLife.size(); i++) {
            float startValue = valChangeOverLife[i]->fromValue_;
            float endValue = valChangeOverLife[i]->toValue_;
            int startTime = valChangeOverLife[i]->startMillis_;
            int endTime = valChangeOverLife[i]->endMillis_;
            auto interpolator = valChangeOverLife[i]->interpolator_;
            if (activeTime >= startTime && activeTime < endTime) {
                accelerationAngle = (interpolator != nullptr)
                    ? GenerateValue(startValue, endValue, startTime, endTime, activeTime, interpolator)
                    : GenerateValue(startValue, endValue, startTime, endTime, activeTime);
            }
        }
    }
    particle->SetAccelerationAngle(accelerationAngle);
}

void RSRenderParticleEffector::UpdateAccelerationValue(
    const std::shared_ptr<RSRenderParticle>& particle, float deltaTime, int64_t activeTime)
{
    auto accelerationValue = particle->GetAccelerationValue();
    auto acceValueUpdator = particleParams_->GetAccelerationValueUpdator();
    float acceValueChange = 0.f;
    if (acceValueUpdator == ParticleUpdator::RANDOM) {
        float acceValueSpeed = particle->GetAccelerationValueSpeed();
        acceValueChange = acceValueSpeed * deltaTime;
        accelerationValue += acceValueChange;
    } else if (acceValueUpdator == ParticleUpdator::CURVE) {
        auto valChangeOverLife = particleParams_->acceleration_.accelerationValue_.valChangeOverLife_;
        for (size_t i = 0; i < valChangeOverLife.size(); i++) {
            float startValue = valChangeOverLife[i]->fromValue_;
            float endValue = valChangeOverLife[i]->toValue_;
            int startTime = valChangeOverLife[i]->startMillis_;
            int endTime = valChangeOverLife[i]->endMillis_;
            auto interpolator = valChangeOverLife[i]->interpolator_;
            if (activeTime >= startTime && activeTime < endTime) {
                accelerationValue = (interpolator != nullptr)
                    ? GenerateValue(startValue, endValue, startTime, endTime, activeTime, interpolator)
                    : GenerateValue(startValue, endValue, startTime, endTime, activeTime);
            }
        }
    }
    particle->SetAccelerationValue(accelerationValue);
}

void RSRenderParticleEffector::UpdateAccelerate(
    const std::shared_ptr<RSRenderParticle>& particle, float deltaTime, int64_t activeTime)
{
    activeTime /= NS_PER_MS;
    auto acceValueUpdator = particleParams_->GetAccelerationValueUpdator();
    auto acceAngleUpdator = particleParams_->GetAccelerationAngleUpdator();
    UpdateAccelerationValue(particle, deltaTime, activeTime);
    UpdateAccelerationAngle(particle, deltaTime, activeTime);
    auto accelerationValue = particle->GetAccelerationValue();
    auto accelerationAngle = particle->GetAccelerationAngle();
    accelerationAngle *= DEGREE_TO_RADIAN;
    if (acceValueUpdator == ParticleUpdator::RANDOM && acceAngleUpdator == ParticleUpdator::RANDOM) {
        auto acceleration = particle->GetAcceleration();
        acceleration = Vector2f { accelerationValue * std::cos(accelerationAngle),
            accelerationValue * std::sin(accelerationAngle) };
        particle->SetAcceleration(acceleration);
    } else if (acceValueUpdator == ParticleUpdator::CURVE && acceAngleUpdator == ParticleUpdator::CURVE) {
        particle->SetAcceleration(
            { accelerationValue * std::cos(accelerationAngle), accelerationValue * std::sin(accelerationAngle) });
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
