/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef ROSEN_ENGINE_CORE_ANIMATION_RS_PARTICLE_FIELD_BASE_H
#define ROSEN_ENGINE_CORE_ANIMATION_RS_PARTICLE_FIELD_BASE_H

#include <cmath>
#include <string>

#include "animation/rs_render_particle.h"
#include "common/rs_macros.h"
#include "common/rs_vector2.h"

namespace OHOS {

class Parcel;

namespace Rosen {

enum class ParticleFieldType : uint32_t { NOISE = 0, RIPPLE = 1, VELOCITY = 2, MAX };

class RSB_EXPORT ParticleFieldBase {
public:
    ShapeType regionShape_ { ShapeType::RECT };
    Vector2f regionPosition_ { 0.0f, 0.0f };
    Vector2f regionSize_ { 0.0f, 0.0f };

    virtual ~ParticleFieldBase();

    virtual ParticleFieldType GetType() const = 0;
    virtual Vector2f Apply(const Vector2f& position, float deltaTime) = 0;
    virtual bool Equals(const ParticleFieldBase& rhs) const = 0;
    virtual void Dump(std::string& out) const = 0;
    virtual bool MarshalSpecific(Parcel& parcel) const = 0;
    virtual bool UnmarshalSpecific(Parcel& parcel) = 0;
    virtual void Update(float deltaTime) {}
    virtual bool IsPointInRegion(const Vector2f& point) const;
    bool MarshalBase(Parcel& parcel) const;
    bool UnmarshalBase(Parcel& parcel);
    bool operator==(const ParticleFieldBase& rhs) const;

protected:
    ParticleFieldBase() = default;
    ParticleFieldBase(const ParticleFieldBase&) = default;
    ParticleFieldBase& operator=(const ParticleFieldBase&) = default;
};

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_ENGINE_CORE_ANIMATION_RS_PARTICLE_FIELD_BASE_H
