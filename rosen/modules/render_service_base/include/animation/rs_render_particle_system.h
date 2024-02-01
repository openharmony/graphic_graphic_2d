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


namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSRenderParticleSystem {
public:
    explicit RSRenderParticleSystem(const std::vector<std::shared_ptr<ParticleRenderParams>>& particlesRenderParams);
    virtual ~RSRenderParticleSystem() = default;
    RSRenderParticleSystem() = default;
    void CreateEmitter();
    void ClearEmitter();
    void CreateEffector();
    std::vector<std::shared_ptr<RSRenderParticle>> Simulation(int64_t deltaTime);
    void Emit(int64_t deltaTime);
    void UpdateParticle(int64_t deltaTime);
    std::vector<std::shared_ptr<RSRenderParticle>> GetActiveParticles();
    bool IsFinish();

private:
    std::vector<std::shared_ptr<ParticleRenderParams>> particlesRenderParams_ = {};
    std::vector<std::shared_ptr<RSRenderParticleEmitter>> emitters_ = {};
    std::vector<std::shared_ptr<RSRenderParticle>> activeParticles_ = {};
    
    std::shared_ptr<RSRenderParticleEffector> effector_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_PARTICLE_SYSTEM_H
