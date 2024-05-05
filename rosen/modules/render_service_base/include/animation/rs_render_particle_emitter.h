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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_PARTICLE_EMITTER_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_PARTICLE_EMITTER_H

#include <memory>

#include "rs_render_particle_effector.h"

namespace OHOS {
namespace Rosen {

class RSRenderParticleEmitter {
public:
    RSRenderParticleEmitter(std::shared_ptr<ParticleRenderParams> particleParams);
    void PreEmit();
    void EmitParticle(int64_t deltaTime);
    const std::vector<std::shared_ptr<RSRenderParticle>>& GetParticles();
    bool IsEmitterFinish();
    const std::shared_ptr<ParticleRenderParams>& GetParticleParams()
    {
        return particleParams_;
    }

private:
    std::vector<std::shared_ptr<RSRenderParticle>> particles_ = {};
    std::shared_ptr<ParticleRenderParams> particleParams_ = {};
    float particleCount_ = 0.f;
    float spawnNum_ = 0.f;
    bool emitFinish_ = false;
};

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_PARTICLE_EMITTER_H
