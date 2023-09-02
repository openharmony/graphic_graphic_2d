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

#include <math.h>
#include <vector>
namespace OHOS {
namespace Rosen {
RSRenderParticleEmitter::RSRenderParticleEmitter(std::shared_ptr<ParticleRenderParams> particleParams)
    : particleParams_(particleParams)
{}

void RSRenderParticleEmitter::EmitParticle(int64_t deltaTime)
{
    auto particleType = particleParams_->GetParticleType();
    auto opacityUpdater = particleParams_->GetOpacityUpdator();
    auto scaleUpdater = particleParams_->GetScaleUpdator();
    if (opacityUpdater == ParticleUpdator::NONE && particleParams_->GetOpacityStartValue() <= 0.f &&
        particleParams_->GetOpacityEndValue() <= 0.f) {
        return;
    } else if (opacityUpdater == ParticleUpdator::RANDOM &&
        particleParams_->GetOpacityStartValue() <= 0.f && particleParams_->GetOpacityEndValue() <= 0.f &&
        particleParams_->GetOpacityRandomStart() <= 0.f && particleParams_->GetOpacityRandomEnd() <= 0.f) {
        return;
    }
    if (scaleUpdater == ParticleUpdator::NONE &&
        particleParams_->GetScaleStartValue() <= 0.f && particleParams_->GetScaleEndValue() <= 0.f) {
        return;
    } else if (scaleUpdater == ParticleUpdator::RANDOM &&
        particleParams_->GetScaleStartValue() <= 0.f && particleParams_->GetScaleEndValue() <= 0.f &&
        particleParams_->GetScaleRandomStart() <= 0.f && particleParams_->GetScaleRandomEnd() <= 0.f) {
        return;
    }

    if (particleType == ParticleType::IMAGES) {
        auto particleImage = particleParams_->GetParticleImage();
        auto imageSize = particleParams_->GetImageSize();
        if (particleImage == nullptr || (imageSize.x_ <= 0.f || imageSize.y_ <= 0.f)) {
            return;
        } else {
            auto image = particleImage->GetImage();
            auto pixelMap = particleImage->GetPixelMap();
            if (pixelMap == nullptr &&  image == nullptr) {
                return;
            }
        }
    }
    if (particleType == ParticleType::POINTS) {
        auto radius = particleParams_->GetParticleRadius();
        if (radius <= 0.f) {
            return;
        }
    }

    auto emitRate = particleParams_->GetEmitRate();
    auto maxParticle = particleParams_->GetParticleCount();
    auto lifeTime = particleParams_->GetParticleLifeTime();
    float last = particleCount_;
    particleCount_ += static_cast<float>(emitRate * deltaTime) / NS_TO_S;
    spawnNum_ += particleCount_ - last;
    particles_.clear();
    if (maxParticle <= 0 || lifeTime == 0) {
        return;
    }
    if (ROSEN_EQ(last, 0.f)) {
        for (uint32_t i = 0; i <= std::min(static_cast<uint32_t>(spawnNum_), maxParticle); i++) {
            auto particle = std::make_shared<RSRenderParticle>(particleParams_);
            particles_.push_back(particle);
            spawnNum_ -= 1.f;
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
    auto maxParticle = particleParams_->GetParticleCount();
    bool isEmitterFinish = false;
    if (particleCount_ == static_cast<float>(maxParticle)) {
        isEmitterFinish = true;
    }
    return isEmitterFinish;
}

std::vector<std::shared_ptr<RSRenderParticle>> RSRenderParticleEmitter::GetParticles()
{
    return particles_;
}

} // namespace Rosen
} // namespace OHOS
