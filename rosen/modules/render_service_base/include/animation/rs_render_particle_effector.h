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
namespace OHOS {
namespace Rosen {

class RSRenderParticleEffector {
public:
    RSRenderParticleEffector(const std::vector<std::shared_ptr<RSRenderParticle>> activeParticles);

    Vector4<int16_t> CalculateColorInt(const std::shared_ptr<RSRenderParticle>& particle, Vector4<int16_t> colorInt,
        Vector4<float> colorF, Vector4<float> colorSpeed, float deltaTime);

    float UpdateCurveValue(
        const std::vector<std::shared_ptr<ChangeInOverLife<float>>>& valChangeOverLife, int64_t activeTime);

    Color UpdateColorCurveValue(
        const std::vector<std::shared_ptr<ChangeInOverLife<Color>>>& valChangeOverLife, int64_t activeTime);

    void UpdateColor(float deltaTime);

    void UpdateOpacity(float deltaTime);

    void UpdateScale(float deltaTime);

    void UpdateSpin(float deltaTime);

    void UpdateAccelerationAngle(float deltaTime);

    void UpdateAccelerationValue(float deltaTime);

    void UpdatePosition(float deltaTime);

    void UpdateActiveTime(int64_t deltaTime);

    // Apply effector to particle
    void ApplyEffectorToParticle(int64_t deltaTime);

    void Update(const std::vector<std::shared_ptr<RSRenderParticle>>& activeParticles, int64_t deltaTime);

private:
    std::vector<std::shared_ptr<RSRenderParticle>> activeParticles_;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_PARTICLE_EFFECTOR_H
