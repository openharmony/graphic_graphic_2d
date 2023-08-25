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
RSRenderParticleEmitter::RSRenderParticleEmitter(std::shared_ptr<ParticleRenderParams> particleParams)
    : particleParams_(particleParams)
{}

void RSRenderParticleEmitter::EmitParticle(int64_t deltaTime)
{
    auto emitRate = particleParams_->GetEmitRate();
    auto maxParticle = particleParams_->GetParticleCount();
    float last = particleCount_;
    particleCount_ += static_cast<float>(emitRate * deltaTime) / NS_TO_S;
    spawnNum_ += particleCount_ - last;
    particles_.clear();
    while (spawnNum_ > 0.f && particleCount_ < static_cast<float>(maxParticle)) {
        auto particle = std::make_shared<RSRenderParticle>(particleParams_);
        particles_.push_back(particle);
        spawnNum_ -= 1.f;
    }
}

std::vector<std::shared_ptr<RSRenderParticle>> RSRenderParticleEmitter::GetParticles()
{
    return particles_;
}

} // namespace Rosen
} // namespace OHOS
