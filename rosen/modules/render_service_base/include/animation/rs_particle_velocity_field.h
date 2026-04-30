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

#ifndef ROSEN_ENGINE_CORE_ANIMATION_RS_PARTICLE_VELOCITY_FIELD_H
#define ROSEN_ENGINE_CORE_ANIMATION_RS_PARTICLE_VELOCITY_FIELD_H

#include <cmath>
#include <memory>
#include <vector>

#include "animation/rs_particle_field_base.h"
#include "animation/rs_particle_noise_field.h"
#include "common/rs_vector2.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {

class RSB_EXPORT ParticleVelocityField : public ParticleFieldBase {
public:
    Vector2f velocity_;                 // Incremental velocity vector (x, y components)

    ParticleVelocityField() = default;

    explicit ParticleVelocityField(const Vector2f& velocity)
        : velocity_(velocity) {}

    ParticleVelocityField(const ParticleVelocityField& field) = default;
    ParticleVelocityField& operator=(const ParticleVelocityField& field) = default;
    ~ParticleVelocityField() override = default;

    ParticleFieldType GetType() const override { return ParticleFieldType::VELOCITY; }
    Vector2f Apply(const Vector2f& position, float deltaTime) override;
    bool Equals(const ParticleFieldBase& rhs) const override;
    void Dump(std::string& out) const override;
    bool MarshalSpecific(Parcel& parcel) const override;
    bool UnmarshalSpecific(Parcel& parcel) override;

    Vector2f ApplyVelocityField(const Vector2f& particlePos, float deltaTime);
    bool IsPointInRegion(const Vector2f& point) const override;

    bool operator==(const ParticleVelocityField& rhs) const
    {
        return ParticleFieldBase::operator==(rhs);
    }
};

class RSB_EXPORT ParticleVelocityFields {
public:
    std::vector<std::shared_ptr<ParticleVelocityField>> velocityFields_;

    ParticleVelocityFields() = default;

    void AddVelocityField(const std::shared_ptr<ParticleVelocityField>& field);
    void RemoveVelocityField(size_t index);
    std::shared_ptr<ParticleVelocityField> GetVelocityField(size_t index);
    size_t GetVelocityFieldCount() const;
    Vector2f ApplyAllVelocityFields(const Vector2f& position, float deltaTime);
    void ClearVelocityFields();
    bool operator==(const ParticleVelocityFields& rhs) const;
    void Dump(std::string& out) const;
};

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_ENGINE_CORE_ANIMATION_RS_PARTICLE_VELOCITY_FIELD_H
