/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <algorithm>
#include <cmath>

#include "animation/rs_particle_ripple_field.h"

namespace OHOS {
namespace Rosen {

constexpr float K_MAX_EXP_TIME = 50.0f;

Vector2f ParticleRippleField::ApplyRippleField(const Vector2f& particlePos, float deltaTime)
{
    if (!IsPointInRegion(particlePos)) {
        return Vector2f(0.0f, 0.0f);
    }
    Vector2f to = particlePos - center_;
    float r = std::sqrt(to.x_ * to.x_ + to.y_ * to.y_);
    if (ROSEN_EQ(r, 0.0f)) {
        return {0.0f, 0.0f};
    }
    Vector2f dir = { to.x_ / r, to.y_ / r };
    float disp = CalculateForceStrength(r);
    if (!std::isfinite(disp)) disp = 0.0f;
    return { dir.x_ * disp, dir.y_ * disp };
}

void ParticleRippleField::UpdateRipple(float deltaTime)
{
    lifeTime_ += deltaTime;
}

float ParticleRippleField::CalculateForceStrength(float distance)
{
    const float k = 2.0f * static_cast<float>(M_PI) / std::max(1e-3f, wavelength_);
    const float w = waveSpeed_ * k;

    const float t  = std::max(0.0f, lifeTime_);
    const float ct = waveSpeed_ * t;

    const float gateWidth = std::max(1.0f, 0.05f * wavelength_);
    float gate = 0.0f;
    if (distance <= ct - gateWidth) {
        gate = 1.0f;
    } else if (distance >= ct + gateWidth) {
        gate = 0.0f;
    } else {
        float x = (ct + gateWidth - distance) / (2.0f * gateWidth);
        gate = x * x * (3.0f - 2.0f * x);
    }

    const float decay = std::exp(-attenuation_ * std::min(t, K_MAX_EXP_TIME));

    const float radialAtten = 1.0f / (1.0f + 0.002f * distance);

    const float phase = k * distance + w * t;
    float disp = amplitude_ * decay * radialAtten * std::sin(phase);

    return gate * disp;
}

bool ParticleRippleField::IsPointInRegion(const Vector2f& point) const
{
    Vector2f relativePos = point - regionPosition_;

    switch (regionShape_) {
        case ShapeType::RECT: {
            float halfWidth = regionSize_.x_ / 2.0f;
            float halfHeight = regionSize_.y_ / 2.0f;
            return std::abs(relativePos.x_) <= halfWidth && std::abs(relativePos.y_) <= halfHeight;
        }
        case ShapeType::CIRCLE: {
            float radius = regionSize_.x_ / 2.0f;
            return relativePos.GetLength() <= radius;
        }
        case ShapeType::ELLIPSE: {
            float radiusX = regionSize_.x_ / 2.0f;
            float radiusY = regionSize_.y_ / 2.0f;
            if (ROSEN_EQ(radiusX, 0.0f) || ROSEN_EQ(radiusY, 0.0f)) {
                return false;
            }
            float normalizedDist = (relativePos.x_ * relativePos.x_) / (radiusX * radiusX) +
                                   (relativePos.y_ * relativePos.y_) / (radiusY * radiusY);
            return normalizedDist <= 1.0f;
        }
        default:
            return true;
    }
}

void ParticleRippleField::Dump(std::string& out) const
{
    out += "ParticleRippleField[";
    out += "center:(" + std::to_string(center_.x_) + "," + std::to_string(center_.y_) + ")";
    out += " amplitude:" + std::to_string(amplitude_);
    out += " wavelength:" + std::to_string(wavelength_);
    out += " waveSpeed:" + std::to_string(waveSpeed_);
    out += " attenuation:" + std::to_string(attenuation_);
    out += " lifeTime:" + std::to_string(lifeTime_);
    out += " region:{shape:" + std::to_string(static_cast<uint32_t>(regionShape_));
    out += " pos:(" + std::to_string(regionPosition_.x_) + "," + std::to_string(regionPosition_.y_) + ")";
    out += " size:(" + std::to_string(regionSize_.x_) + "," + std::to_string(regionSize_.y_) + ")}";
    out += "]";
}

void ParticleRippleFields::AddRippleField(const std::shared_ptr<ParticleRippleField>& field)
{
    if (field) {
        rippleFields_.push_back(field);
    }
}

void ParticleRippleFields::RemoveRippleField(size_t index)
{
    if (index < rippleFields_.size()) {
        rippleFields_.erase(rippleFields_.begin() + index);
    }
}

std::shared_ptr<ParticleRippleField> ParticleRippleFields::GetRippleField(size_t index)
{
    if (index < rippleFields_.size()) {
        return rippleFields_[index];
    }
    return nullptr;
}

size_t ParticleRippleFields::GetRippleFieldCount() const
{
    return rippleFields_.size();
}

Vector2f ParticleRippleFields::ApplyAllRippleFields(const Vector2f& position, float deltaTime)
{
    Vector2f totalEffect = {0.0f, 0.0f};
    for (auto& field : rippleFields_) {
        if (field) {
            totalEffect += field->ApplyRippleField(position, deltaTime);
        }
    }
    return totalEffect;
}

void ParticleRippleFields::ClearRippleFields()
{
    rippleFields_.clear();
}

void ParticleRippleFields::UpdateAllRipples(float deltaTime)
{
    for (auto& field : rippleFields_) {
        if (field) {
            field->UpdateRipple(deltaTime);
        }
    }
}

bool ParticleRippleFields::operator==(const ParticleRippleFields& rhs) const
{
    if (rippleFields_.size() != rhs.rippleFields_.size()) {
        return false;
    }
    for (size_t i = 0; i < rippleFields_.size(); ++i) {
        if (!rippleFields_[i] || !rhs.rippleFields_[i]) {
            if (rippleFields_[i] != rhs.rippleFields_[i]) {
                return false;
            }
        } else if (!(*rippleFields_[i] == *rhs.rippleFields_[i])) {
            return false;
        }
    }
    return true;
}

void ParticleRippleFields::Dump(std::string& out) const
{
    out += "ParticleRippleFields[";
    bool first = true;
    for (const auto& field : rippleFields_) {
        if (field) {
            if (!first) {
                out += " ";
            }
            field->Dump(out);
            first = false;
        }
    }
    out += "]";
}

} // namespace Rosen
} // namespace OHOS
