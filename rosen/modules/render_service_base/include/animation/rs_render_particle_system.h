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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_PARTICLE_SYSTEM_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_PARTICLE_SYSTEM_H

#include "rs_render_particle_emitter.h"
#include "rs_particle_field_collection.h"
namespace OHOS {
namespace Rosen {
class RSRenderParticlePool;
class RSB_EXPORT RSRenderParticleSystem : RSRenderParticleEffector {
public:
    explicit RSRenderParticleSystem(const std::vector<std::shared_ptr<ParticleRenderParams>>& particlesRenderParams);
    virtual ~RSRenderParticleSystem() = default;
    RSRenderParticleSystem() = default;
    void CreateEmitter();
    void ClearEmitter();
    void Emit(int64_t deltaTime, std::vector<std::shared_ptr<RSImage>>& imageVector);
    void UpdateParticle(int64_t deltaTime);
    void Warmup(int64_t totalTimeNs, std::vector<std::shared_ptr<RSImage>>& imageVector);
    bool IsFinish();
    void UpdateEmitter(const std::vector<std::shared_ptr<ParticleRenderParams>>& particlesRenderParams);
    void UpdateNoiseField(const std::shared_ptr<ParticleNoiseFields>& particleNoiseFields);
    void UpdateRippleField(const std::shared_ptr<ParticleRippleFields>& particleRippleFields,
        bool isIncrementalUpdate = true);
    void UpdateVelocityField(const std::shared_ptr<ParticleVelocityFields>& particleVelocityFields);
    void UpdateFields(const std::shared_ptr<ParticleFieldCollection>& fields);
    const std::vector<std::shared_ptr<RSRenderParticleEmitter>>& GetParticleEmitter() const
    {
        return emitters_;
    }
    const std::shared_ptr<ParticleNoiseFields>& GetParticleNoiseFields() const
    {
        return particleNoiseFields_;
    }
    const std::shared_ptr<ParticleRippleFields>& GetParticleRippleFields() const
    {
        return particleRippleFields_;
    }
    const std::shared_ptr<ParticleVelocityFields>& GetParticleVelocityFields() const
    {
        return particleVelocityFields_;
    }
    bool HasAnyField() const
    {
        return (particleFields_ != nullptr && particleFields_->Count() > 0) ||
               (particleNoiseFields_ != nullptr && particleNoiseFields_->GetFieldCount() > 0) ||
               (particleRippleFields_ != nullptr && particleRippleFields_->GetRippleFieldCount() > 0) ||
               (particleVelocityFields_ != nullptr && particleVelocityFields_->GetVelocityFieldCount() > 0);
    }

    const std::shared_ptr<RSRenderParticlePool>& GetParticlePool() const
    {
        return particlePool_;
    }

    // Replaces the buffer with an empty one; a rebuilt population must not inherit the old slots.
    void ResetParticlePool();

private:
    void EnsureParticlePool();
    // Emits from one emitter straight into the pool; keeps the per-emitter nesting out of Emit().
    void EmitFromEmitter(int64_t deltaTime, size_t emitterIndex);
    // Rebuilds the legacy noise / ripple / velocity field set once per frame instead of per particle.
    const std::shared_ptr<ParticleFieldCollection>& GetLegacyFields();
    void WarmupFields(int64_t totalTimeNs);
    void WarmupEmitter(size_t emitterIndex, int64_t totalTimeNs);
    std::vector<std::shared_ptr<ParticleRenderParams>> particlesRenderParams_ = {};
    std::vector<std::shared_ptr<RSRenderParticleEmitter>> emitters_ = {};
    // Indexed by the PARAMS_INDEX row, so a slot can find its emitter parameters after compaction.
    std::vector<std::shared_ptr<ParticleRenderParams>> emitterParams_ = {};
    std::shared_ptr<ParticleNoiseFields> particleNoiseFields_;
    std::shared_ptr<ParticleRippleFields> particleRippleFields_;
    std::shared_ptr<ParticleVelocityFields> particleVelocityFields_;
    std::shared_ptr<ParticleFieldCollection> particleFields_;
    std::vector<std::shared_ptr<RSImage>> imageVector_;
    std::shared_ptr<RSRenderParticlePool> particlePool_;
    std::vector<std::vector<size_t>> groupedSlots_;
    std::shared_ptr<ParticleFieldCollection> legacyFields_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_PARTICLE_SYSTEM_H
