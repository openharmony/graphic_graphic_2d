/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_PARTICLE_ANIMATION_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_PARTICLE_ANIMATION_H

#include <memory>
#include <vector>

#include "animation/rs_animation.h"
#include "animation/rs_render_particle.h"
#include "modifier_ng/rs_modifier_ng_type.h"

namespace OHOS {
namespace Rosen {
class RSNode;
class RSRenderParticleAnimation;

class RSC_EXPORT RSParticleAnimation : public RSAnimation {
public:
    RSParticleAnimation(const std::shared_ptr<RSUIContext>& rsUIContext,
        std::vector<std::shared_ptr<ParticleRenderParams>> particlesRenderParams, ModifierId modifierId);
    ~RSParticleAnimation() override = default;

    bool IsSupportInteractiveAnimator() override
    {
        return false;
    }

    bool IsParticleAnimation() const override
    {
        return true;
    }

protected:
    void OnStart() override;

    void RebuildInRender() override;

private:
    std::shared_ptr<RSRenderParticleAnimation> CreateRenderAnimation();
    bool IsInfiniteParticle() const;
    void SendCreateParticleCommand(
        const std::shared_ptr<RSNode>& target, const std::shared_ptr<RSRenderParticleAnimation>& animation);

    std::vector<std::shared_ptr<ParticleRenderParams>> particlesRenderParams_;
    PropertyId propertyId_ { 0 };
    ModifierId modifierId_ { 0 };
    std::weak_ptr<RSUIContext> particleUIContext_;
    bool isInfinite_ { false };
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_PARTICLE_ANIMATION_H
