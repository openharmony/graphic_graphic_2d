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

#include "animation/rs_particle_animation.h"

#include "animation/rs_render_particle_animation.h"
#include "command/rs_animation_command.h"
#include "modifier/rs_property.h"
#include "modifier_ng/appearance/rs_particle_effect_modifier.h"
#include "platform/common/rs_log.h"
#include "ui/rs_node.h"
#include "ui/rs_ui_context.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr int REPEAT_COUNT_INFINITE = -1;
}

RSParticleAnimation::RSParticleAnimation(const std::shared_ptr<RSUIContext>& rsUIContext,
    std::vector<std::shared_ptr<ParticleRenderParams>> particlesRenderParams, ModifierId modifierId)
    : RSAnimation(rsUIContext), particlesRenderParams_(std::move(particlesRenderParams)), modifierId_(modifierId),
      particleUIContext_(rsUIContext)
{
    isInfinite_ = IsInfiniteParticle();
    if (isInfinite_) {
        SetRepeatCount(REPEAT_COUNT_INFINITE);
    }
}

bool RSParticleAnimation::IsInfiniteParticle() const
{
    constexpr int INFINITE_PARTICLE_COUNT = -1;
    constexpr int64_t INFINITE_LIFETIME_MS = -1;
    for (const auto& p : particlesRenderParams_) {
        if (p == nullptr) {
            continue;
        }
        if (p->GetParticleCount() == INFINITE_PARTICLE_COUNT) {
            return true;
        }
        if (p->emitterConfig_.lifeTime_.start_ == INFINITE_LIFETIME_MS &&
            p->emitterConfig_.lifeTime_.end_ == INFINITE_LIFETIME_MS) {
            return true;
        }
    }
    return false;
}

std::shared_ptr<RSRenderParticleAnimation> RSParticleAnimation::CreateRenderAnimation()
{
    auto particlesRenderParams = particlesRenderParams_;
    auto animation =
        std::make_shared<RSRenderParticleAnimation>(GetId(), propertyId_, std::move(particlesRenderParams));
    if (isInfinite_) {
        animation->SetRepeatCount(REPEAT_COUNT_INFINITE);
    }
    if (auto rsUIContext = particleUIContext_.lock()) {
        animation->SetParticleAnimationToken(rsUIContext->GetToken());
    }
    return animation;
}

void RSParticleAnimation::OnStart()
{
    auto target = GetTarget().lock();
    if (target == nullptr) {
        ROSEN_LOGE("Failed to start particle animation, target is null!");
        return;
    }
    if (propertyId_ == 0) {
        auto property = std::make_shared<RSProperty<int>>();
        propertyId_ = property->GetId();
    }
    auto animation = CreateRenderAnimation();
    SendCreateParticleCommand(target, animation);
}

void RSParticleAnimation::RebuildInRender()
{
    auto target = GetTarget().lock();
    if (target == nullptr) {
        ROSEN_LOGE("Failed to rebuild particle animation, target is null!");
        return;
    }
    auto animation = CreateRenderAnimation();
    animation->SetRebuildRunningTimeNs(static_cast<int64_t>(GetRebuildParam().fraction));
    if (auto modifier = target->GetModifierByType(ModifierNG::RSModifierType::PARTICLE_EFFECT)) {
        auto particleModifier = std::static_pointer_cast<ModifierNG::RSParticleEffectModifier>(modifier);
        animation->UpdateNoiseField(particleModifier->GetParticleNoiseFields());
        animation->UpdateRippleField(particleModifier->GetParticleRippleFields());
        animation->UpdateVelocityField(particleModifier->GetParticleVelocityFields());
    }
    SetRebuildParam({});
    SendCreateParticleCommand(target, animation);
}

void RSParticleAnimation::SendCreateParticleCommand(
    const std::shared_ptr<RSNode>& target, const std::shared_ptr<RSRenderParticleAnimation>& animation)
{
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCreateParticleNG>(target->GetId(), modifierId_, animation);
    target->AddCommand(command, target->IsRenderServiceNode(), target->GetFollowType(), target->GetId());
    if (target->NeedForcedSendToRemote()) {
        std::unique_ptr<RSCommand> cmdForRemote =
            std::make_unique<RSAnimationCreateParticleNG>(target->GetId(), modifierId_, animation);
        target->AddCommand(cmdForRemote, true, target->GetFollowType(), target->GetId());
    }
}
} // namespace Rosen
} // namespace OHOS
