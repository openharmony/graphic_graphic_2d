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

#include "animation/rs_render_particle_emitter.h"

#include <vector>
namespace OHOS {
namespace Rosen {
std::vector<std::shared_ptr<RSRenderParticle>> RSRenderParticleEmitter::activeParticles_ = {};
RSRenderParticleEmitter::RSRenderParticleEmitter(std::shared_ptr<ParticleRenderParams> particleParams)
    : particleParams_(particleParams)
{}

void RSRenderParticleEmitter::EmitParticle(int64_t deltaTime)
{
    auto emitRate = particleParams_->GetEmitRate();
    auto maxParticle = particleParams_->GetParticleCount();
    for (int64_t i = 0; i * NS_TO_S < emitRate * deltaTime; i++) {
        if (particles_.size() < maxParticle) {
            auto particle = std::make_shared<RSRenderParticle>(particleParams_);
            particles_.push_back(particle);
            activeParticles_.push_back(particle);
        }
    }
}

void RSRenderParticleEmitter::UpdateParticle(int64_t deltaTime)
{
    for (auto particle : activeParticles_) {
        auto particleRenderParams = particle->GetParticleRenderParams();
        auto effect = RSRenderParticleEffector(particleRenderParams);
        effect.ApplyEffectorToParticle(particle, deltaTime);
    }
    for (auto it = activeParticles_.begin(); it != activeParticles_.end();) {
        std::shared_ptr<RSRenderParticle> particle = *it;
        if (!particle->IsAlive()) {
            it = activeParticles_.erase(it);
        } else {
            ++it;
        }
    }
}

std::vector<std::shared_ptr<RSRenderParticle>> RSRenderParticleEmitter::GetParticles()
{
    return particles_;
}

std::vector<std::shared_ptr<RSRenderParticle>> RSRenderParticleEmitter::GetActiveParticles()
{
    return activeParticles_;
}

} // namespace Rosen
} // namespace OHOS
