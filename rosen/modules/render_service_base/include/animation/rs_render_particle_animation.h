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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_PARTICLE_ANIMATION_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_PARTICLE_ANIMATION_H

#include <memory>

#include "rs_render_particle.h"

#include "animation/rs_interpolator.h"
#include "animation/rs_render_particle_system.h"
#include "animation/rs_render_property_animation.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSRenderParticleAnimation : public RSRenderPropertyAnimation {
public:
    RSRenderParticleAnimation(
        AnimationId id, const std::vector<std::shared_ptr<ParticleRenderParams>> particlesRenderParams);

    virtual ~RSRenderParticleAnimation() = default;
    RSRenderParticleAnimation() = default;

    bool Marshalling(Parcel& parcel) const override;

    [[nodiscard]] static RSRenderParticleAnimation* Unmarshalling(Parcel& parcel);
    std::vector<std::shared_ptr<RSRenderParticle>> GetRenderParticle()
    {
        return renderParticle_;
    }

protected:
    bool Animate(int64_t time) override;

private:
    bool ParseParam(Parcel& parcel) override;
    RSRenderParticleSystem particleSystem_;
    std::vector<std::shared_ptr<ParticleRenderParams>> particlesRenderParams_;
    std::vector<std::shared_ptr<RSRenderParticle>> renderParticle_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_RENDER_PARTICLE_ANIMATION_H
