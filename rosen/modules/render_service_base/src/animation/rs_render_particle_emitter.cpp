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

#include <cmath>
#include <cstdint>
#include <vector>

namespace OHOS {
namespace Rosen {
RSRenderParticleEmitter::RSRenderParticleEmitter(std::shared_ptr<ParticleRenderParams> particleParams)
    : particleParams_(particleParams)
{}

void RSRenderParticleEmitter::PreEmit()
{
    auto particleType = particleParams_->GetParticleType();
    auto opacityUpdater = particleParams_->GetOpacityUpdator();
    auto scaleUpdater = particleParams_->GetScaleUpdator();
    if (opacityUpdater == ParticleUpdator::NONE && particleParams_->GetOpacityStartValue() <= 0.f) {
        emitFinish_ = true;
        return;
    } else if (opacityUpdater == ParticleUpdator::RANDOM && particleParams_->GetOpacityStartValue() <= 0.f &&
               particleParams_->GetOpacityRandomStart() <= 0.f) {
        emitFinish_ = true;
        return;
    }
    if (scaleUpdater == ParticleUpdator::NONE && particleParams_->GetScaleStartValue() <= 0.f) {
        emitFinish_ = true;
        return;
    } else if (scaleUpdater == ParticleUpdator::RANDOM && particleParams_->GetScaleStartValue() <= 0.f &&
               particleParams_->GetScaleRandomStart() <= 0.f) {
        emitFinish_ = true;
        return;
    }
    if (particleType == ParticleType::IMAGES) {
        auto particleImage = particleParams_->GetParticleImage();
        auto imageSize = particleParams_->GetImageSize();
        if (particleImage == nullptr || (imageSize.x_ <= 0.f || imageSize.y_ <= 0.f)) {
            emitFinish_ = true;
            return;
        }
    }
    if (particleType == ParticleType::POINTS) {
        auto radius = particleParams_->GetParticleRadius();
        if (radius <= 0.f) {
            emitFinish_ = true;
            return;
        }
    }
}

void RSRenderParticleEmitter::EmitParticle(int64_t deltaTime)
{
    PreEmit();
    if (emitFinish_ == true) {
        return;
    }
    auto emitRate = particleParams_->GetEmitRate();
    auto maxParticle = particleParams_->GetParticleCount();
    auto lifeTimeStart = particleParams_->GetLifeTimeStartValue();
    auto lifeTimeEnd = particleParams_->GetLifeTimeEndValue();
    float last = particleCount_;
    particles_.clear();
    if (maxParticle == -1) {
        maxParticle = INT32_MAX;
    }
    if (maxParticle <= 0 || (lifeTimeStart == 0 && lifeTimeEnd == 0) || emitRate == 0 ||
        last > static_cast<float>(maxParticle)) {
        emitFinish_ = true;
        return;
    }
    particleCount_ += static_cast<float>(emitRate * deltaTime) / NS_TO_S;
    spawnNum_ += particleCount_ - last;
    if (ROSEN_EQ(last, 0.f)) {
        for (int32_t i = 0; i < std::min(static_cast<int32_t>(spawnNum_), maxParticle); i++) {
            auto particle = std::make_shared<RSRenderParticle>(particleParams_);
            particles_.push_back(particle);
            spawnNum_ -= 1.f;
        }
        if (particleCount_ > static_cast<float>(maxParticle)) {
            return;
        }
    }
    while (spawnNum_ >= 1.f && std::ceil(last) <= static_cast<float>(maxParticle)) {
        auto particle = std::make_shared<RSRenderParticle>(particleParams_);
        particles_.push_back(particle);
        spawnNum_ -= 1.f;
        last += 1.f;
    }
}

bool RSRenderParticleEmitter::IsEmitterFinish()
{
    return emitFinish_;
}

const std::vector<std::shared_ptr<RSRenderParticle>>& RSRenderParticleEmitter::GetParticles()
{
    return particles_;
}

} // namespace Rosen
} // namespace OHOS
