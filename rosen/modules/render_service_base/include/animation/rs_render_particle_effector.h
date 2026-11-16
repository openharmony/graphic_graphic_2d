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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_PARTICLE_EFFECTOR_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_PARTICLE_EFFECTOR_H

#include "rs_render_particle.h"
#include "rs_particle_noise_field.h"
#include "rs_particle_ripple_field.h"
#include "rs_particle_velocity_field.h"
#include "rs_particle_field_collection.h"
namespace OHOS {
namespace Rosen {

class RSRenderParticlePool;

// Every entry point takes (pool, slot): there is no per-particle object to carry the state.
class RSRenderParticleEffector {
public:
    RSRenderParticleEffector();

    Vector4<int16_t> CalculateColorInt(RSRenderParticlePool& pool, size_t slot, Vector4<int16_t> colorInt,
        Vector4<float> colorF, Vector4<float> colorSpeed, float deltaTime);

    void UpdateCurveValue(float& value,
        const std::vector<std::shared_ptr<ChangeInOverLife<float>>>& valChangeOverLife, int64_t activeTime);

    void UpdateColorCurveValue(Color& color,
        const std::vector<std::shared_ptr<ChangeInOverLife<Color>>>& valChangeOverLife, int64_t activeTime);

    void UpdateColor(RSRenderParticlePool& pool, size_t slot, ParticleRenderParams& params, float deltaTime);
    void UpdateOpacity(RSRenderParticlePool& pool, size_t slot, ParticleRenderParams& params, float deltaTime);
    void UpdateScale(RSRenderParticlePool& pool, size_t slot, ParticleRenderParams& params, float deltaTime);
    void UpdateSpin(RSRenderParticlePool& pool, size_t slot, ParticleRenderParams& params, float deltaTime);
    void UpdateAccelerationAngle(RSRenderParticlePool& pool, size_t slot, ParticleRenderParams& params,
        float deltaTime);
    void UpdateAccelerationValue(RSRenderParticlePool& pool, size_t slot, ParticleRenderParams& params,
        float deltaTime);
    void UpdatePosition(RSRenderParticlePool& pool, size_t slot,
        const std::shared_ptr<ParticleFieldCollection>& fields, float deltaTime);

    void Update(RSRenderParticlePool& pool, size_t slot, ParticleRenderParams& params,
        const std::shared_ptr<ParticleFieldCollection>& fields, int64_t deltaTime);
    // One emitter group: skips NONE properties and vectorizes the RANDOM ones over the slot list.
    void UpdateGroup(RSRenderParticlePool& pool, const std::vector<size_t>& slots, ParticleRenderParams& params,
        const std::shared_ptr<ParticleFieldCollection>& fields, int64_t deltaTime);
    void UpdateAccelerationGroup(RSRenderParticlePool& pool, const std::vector<size_t>& slots,
        ParticleRenderParams& params, float deltaTime);
    void UpdateColorGroup(RSRenderParticlePool& pool, const std::vector<size_t>& slots,
        ParticleRenderParams& params, float deltaTime);
    void UpdateOpacityGroup(RSRenderParticlePool& pool, const std::vector<size_t>& slots,
        ParticleRenderParams& params, float deltaTime);
    void UpdateScaleGroup(RSRenderParticlePool& pool, const std::vector<size_t>& slots,
        ParticleRenderParams& params, float deltaTime);
    void UpdateSpinGroup(RSRenderParticlePool& pool, const std::vector<size_t>& slots,
        ParticleRenderParams& params, float deltaTime);
    void UpdatePositionGroup(RSRenderParticlePool& pool, const std::vector<size_t>& slots,
        const std::shared_ptr<ParticleFieldCollection>& fields, float deltaTime);
    void UpdateToActiveTime(RSRenderParticlePool& pool, size_t slot, ParticleRenderParams& params,
        const std::shared_ptr<ParticleFieldCollection>& fields, int64_t activeTimeNs);
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_PARTICLE_EFFECTOR_H
