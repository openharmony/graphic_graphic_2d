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

void RSRenderParticleSystem::ClearEmitter()
{
    emitters_.clear();
}

void RSRenderParticleSystem::Emit(int64_t deltaTime, std::vector<std::shared_ptr<RSRenderParticle>>& activeParticles_)
{
    for (size_t iter = 0; iter < emitters_.size(); iter++) {
        if (emitters_[iter] != nullptr) {
            emitters_[iter]->EmitParticle(deltaTime);
            auto& particles = emitters_[iter]->GetParticles();
            activeParticles_.insert(activeParticles_.end(), particles.begin(), particles.end());
        }
    }
}

void RSRenderParticleSystem::UpdateParticle(
    int64_t deltaTime, std::vector<std::shared_ptr<RSRenderParticle>>& activeParticles_)
{
    if (activeParticles_.empty()) {
        return;
    }
    for (auto it = activeParticles_.begin(); it != activeParticles_.end();) {
        // std::shared_ptr<RSRenderParticle> particle = *it;
        if ((*it) == nullptr || !(*it)->IsAlive()) {
            it = activeParticles_.erase(it);
        } else {
            Update((*it), deltaTime);
            ++it;
        }
    }
}

bool RSRenderParticleSystem::IsFinish(const std::vector<std::shared_ptr<RSRenderParticle>>& activeParticles_)
{
    bool finish = true;
    if (!activeParticles_.empty()) {
        return false;
    }
    for (size_t iter = 0; iter < emitters_.size(); iter++) {
        if (emitters_[iter] != nullptr) {
            finish = finish && emitters_[iter]->IsEmitterFinish();
        }
    }
    return finish;
}

} // namespace Rosen
} // namespace OHOS
