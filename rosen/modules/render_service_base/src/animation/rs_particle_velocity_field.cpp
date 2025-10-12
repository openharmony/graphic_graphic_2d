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

#include <cmath>

#include "animation/rs_particle_velocity_field.h"

namespace OHOS {
namespace Rosen {

Vector2f ParticleVelocityField::ApplyVelocityField(const Vector2f& particlePos, float deltaTime)
{
    // Check if particle is within the region
    if (!IsPointInRegion(particlePos)) {
        return Vector2f(0.0f, 0.0f);
    }

    // Return the velocity vector directly
    return velocity_;
}

bool ParticleVelocityField::IsPointInRegion(const Vector2f& point) const
{
    switch (regionShape_) {
        case ShapeType::RECT: {
            float halfWidth = regionSize_.x_ / 2.0f;
            float halfHeight = regionSize_.y_ / 2.0f;
            Vector2f delta = point - regionPosition_;
            return (std::abs(delta.x_) <= halfWidth && std::abs(delta.y_) <= halfHeight);
        }
        case ShapeType::CIRCLE: {
            Vector2f delta = point - regionPosition_;
            float radius = regionSize_.x_ / 2.0f;
            float distanceSquared = delta.x_ * delta.x_ + delta.y_ * delta.y_;
            return distanceSquared <= (radius * radius);
        }
        case ShapeType::ELLIPSE: {
            Vector2f delta = point - regionPosition_;
            float radiusX = regionSize_.x_ / 2.0f;
            float radiusY = regionSize_.y_ / 2.0f;

            if (radiusX < 1e-6f || radiusY < 1e-6f) {
                return false;
            }

            // Ellipse equation: (x/rx)^2 + (y/ry)^2 <= 1
            float normalizedDist = (delta.x_ * delta.x_) / (radiusX * radiusX) +
                                   (delta.y_ * delta.y_) / (radiusY * radiusY);
            return normalizedDist <= 1.0f;
        }
        default:
            return false;
    }
}

void ParticleVelocityField::Dump(std::string& out) const
{
    out += "ParticleVelocityField[";
    out += "velocity:(" + std::to_string(velocity_.x_) + "," + std::to_string(velocity_.y_) + ")";
    out += " region:{shape:" + std::to_string(static_cast<uint32_t>(regionShape_));
    out += " pos:(" + std::to_string(regionPosition_.x_) + "," + std::to_string(regionPosition_.y_) + ")";
    out += " size:(" + std::to_string(regionSize_.x_) + "," + std::to_string(regionSize_.y_) + ")}";
    out += "]";
}

void ParticleVelocityFields::AddVelocityField(const std::shared_ptr<ParticleVelocityField>& field)
{
    if (field) {
        velocityFields_.push_back(field);
    }
}

void ParticleVelocityFields::RemoveVelocityField(size_t index)
{
    if (index < velocityFields_.size()) {
        velocityFields_.erase(velocityFields_.begin() + index);
    }
}

std::shared_ptr<ParticleVelocityField> ParticleVelocityFields::GetVelocityField(size_t index)
{
    if (index < velocityFields_.size()) {
        return velocityFields_[index];
    }
    return nullptr;
}

size_t ParticleVelocityFields::GetVelocityFieldCount() const
{
    return velocityFields_.size();
}

Vector2f ParticleVelocityFields::ApplyAllVelocityFields(const Vector2f& position, float deltaTime)
{
    Vector2f totalVelocity = {0.0f, 0.0f};
    for (auto& field : velocityFields_) {
        if (field) {
            totalVelocity += field->ApplyVelocityField(position, deltaTime);
        }
    }
    return totalVelocity;
}

void ParticleVelocityFields::ClearVelocityFields()
{
    velocityFields_.clear();
}

bool ParticleVelocityFields::operator==(const ParticleVelocityFields& rhs) const
{
    if (velocityFields_.size() != rhs.velocityFields_.size()) {
        return false;
    }
    for (size_t i = 0; i < velocityFields_.size(); ++i) {
        if (!velocityFields_[i] || !rhs.velocityFields_[i]) {
            if (velocityFields_[i] != rhs.velocityFields_[i]) {
                return false;
            }
        } else if (!(*velocityFields_[i] == *rhs.velocityFields_[i])) {
            return false;
        }
    }
    return true;
}

void ParticleVelocityFields::Dump(std::string& out) const
{
    out += "ParticleVelocityFields[";
    bool first = true;
    for (const auto& field : velocityFields_) {
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
