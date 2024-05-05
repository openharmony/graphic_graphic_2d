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
namespace OHOS {
namespace Rosen {

class RSRenderParticleEffector {
public:
    RSRenderParticleEffector();

    Vector4<int16_t> CalculateColorInt(const std::shared_ptr<RSRenderParticle>& particle, Vector4<int16_t> colorInt,
        Vector4<float> colorF, Vector4<float> colorSpeed, float deltaTime);

    void UpdateCurveValue(float& value,
        const std::vector<std::shared_ptr<ChangeInOverLife<float>>>& valChangeOverLife, int64_t activeTime);

    void UpdateColorCurveValue(Color& color,
        const std::vector<std::shared_ptr<ChangeInOverLife<Color>>>& valChangeOverLife, int64_t activeTime);

    void UpdateColor(const std::shared_ptr<RSRenderParticle>& particle, float deltaTime);

    void UpdateOpacity(const std::shared_ptr<RSRenderParticle>& particle, float deltaTime);

    void UpdateScale(const std::shared_ptr<RSRenderParticle>& particle, float deltaTime);

    void UpdateSpin(const std::shared_ptr<RSRenderParticle>& particle, float deltaTime);

    void UpdateAccelerationAngle(const std::shared_ptr<RSRenderParticle>& particle, float deltaTime);

    void UpdateAccelerationValue(const std::shared_ptr<RSRenderParticle>& particle, float deltaTime);

    void UpdatePosition(const std::shared_ptr<RSRenderParticle>& particle,
        const std::shared_ptr<ParticleNoiseFields>& particleNoiseFields, float deltaTime);

    void UpdateActiveTime(const std::shared_ptr<RSRenderParticle>& particle, int64_t deltaTime);

    void Update(const std::shared_ptr<RSRenderParticle>& particle,
        const std::shared_ptr<ParticleNoiseFields>& particleNoiseFields, int64_t deltaTime);
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_PARTICLE_EFFECTOR_H
