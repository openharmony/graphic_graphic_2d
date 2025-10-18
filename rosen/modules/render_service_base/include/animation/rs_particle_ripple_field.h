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

#ifndef ROSEN_ENGINE_CORE_ANIMATION_RS_PARTICLE_RIPPLE_FIELD_H
#define ROSEN_ENGINE_CORE_ANIMATION_RS_PARTICLE_RIPPLE_FIELD_H

#include <cmath>
#include <memory>
#include <vector>

#include "animation/rs_particle_noise_field.h"
#include "common/rs_vector2.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {

class RSB_EXPORT ParticleRippleField {
public:
    Vector2f center_;                    // Center position relative to region top-left
    float amplitude_ { 100.0f };         // A - amplitude
    float wavelength_ { 50.0f };         // λ - wavelength (k = 2π/λ)
    float waveSpeed_ { 200.0f };         // c - wave speed (ω = c*k)
    float attenuation_ { 0.0f };         // a - attenuation coefficient

    // Region support
    ShapeType regionShape_ { ShapeType::RECT };
    Vector2f regionPosition_ { 0.0f, 0.0f };
    Vector2f regionSize_ { 0.0f, 0.0f };  // [width, height] as diameters (consistent with NoiseField)

    // Runtime state
    float lifeTime_ { 0.0f };

    ParticleRippleField() = default;

    explicit ParticleRippleField(const Vector2f& center, float amplitude, float wavelength,
                                  float waveSpeed, float attenuation = 0.0f)
        : center_(center), amplitude_(amplitude), wavelength_(wavelength),
          waveSpeed_(waveSpeed), attenuation_(attenuation) {}
    
    ParticleRippleField(const ParticleRippleField& field) = default;
    ParticleRippleField& operator=(const ParticleRippleField& field) = default;
    ~ParticleRippleField() = default;
    
    Vector2f ApplyRippleField(const Vector2f& particlePos, float deltaTime);
    void UpdateRipple(float deltaTime);
    float CalculateForceStrength(float distance);
    
    bool operator==(const ParticleRippleField& rhs) const
    {
        return (this->center_ == rhs.center_) &&
               ROSEN_EQ(this->amplitude_, rhs.amplitude_) &&
               ROSEN_EQ(this->wavelength_, rhs.wavelength_) &&
               ROSEN_EQ(this->waveSpeed_, rhs.waveSpeed_) &&
               ROSEN_EQ(this->attenuation_, rhs.attenuation_) &&
               (this->regionShape_ == rhs.regionShape_) &&
               (this->regionPosition_ == rhs.regionPosition_) &&
               (this->regionSize_ == rhs.regionSize_) &&
               ROSEN_EQ(this->lifeTime_, rhs.lifeTime_);
    }
    
    void Dump(std::string& out) const;

private:
    bool IsPointInRegion(const Vector2f& point) const;
};

class RSB_EXPORT ParticleRippleFields {
public:
    std::vector<std::shared_ptr<ParticleRippleField>> rippleFields_;

    ParticleRippleFields() = default;

    void AddRippleField(const std::shared_ptr<ParticleRippleField>& field);
    void RemoveRippleField(size_t index);
    std::shared_ptr<ParticleRippleField> GetRippleField(size_t index);
    size_t GetRippleFieldCount() const;
    Vector2f ApplyAllRippleFields(const Vector2f& position, float deltaTime);
    void ClearRippleFields();
    void UpdateAllRipples(float deltaTime);
    bool operator==(const ParticleRippleFields& rhs) const;
    void Dump(std::string& out) const;
};

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_ENGINE_CORE_ANIMATION_RS_PARTICLE_RIPPLE_FIELD_H