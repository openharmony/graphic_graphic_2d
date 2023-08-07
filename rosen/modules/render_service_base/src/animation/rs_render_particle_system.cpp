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

#include "animation/rs_render_particle_system.h"

#include <cstddef>
namespace OHOS {
namespace Rosen {
RSRenderParticleSystem::RSRenderParticleSystem(
    const std::vector<std::shared_ptr<ParticleRenderParams>>& particlesRenderParams)
    : particlesRenderParams_(particlesRenderParams)
{
    CreateEmitter();
}

void RSRenderParticleSystem::CreateEmitter()
{
    for (size_t iter = 0; iter < particlesRenderParams_.size(); iter++) {
        auto particleRenderParams = particlesRenderParams_[iter];
        emitters_.push_back(std::make_shared<RSRenderParticleEmitter>(particleRenderParams));
    }
}

void RSRenderParticleSystem::Emit(int64_t deltaTime)
{
    for (size_t iter = 0; iter < emitters_.size(); iter++) {
        emitters_[iter]->EmitParticle(deltaTime);
    }
}

std::vector<std::shared_ptr<RSRenderParticle>> RSRenderParticleSystem::Simulation(int64_t deltaTime)
{
    Emit(deltaTime);
    RSRenderParticleEmitter::UpdateParticle(deltaTime);
    return RSRenderParticleEmitter::GetActiveParticles();
}
} // namespace Rosen
} // namespace OHOS
