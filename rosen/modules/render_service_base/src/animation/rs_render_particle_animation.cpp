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

#include "animation/rs_render_particle_animation.h"

#include <memory>

#include "animation/rs_value_estimator.h"
#include "command/rs_animation_command.h"
#include "common/rs_optional_trace.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_marshalling_helper.h"

namespace OHOS {
namespace Rosen {
RSRenderParticleAnimation::RSRenderParticleAnimation(AnimationId id, const PropertyId& propertyId,
    const std::vector<std::shared_ptr<ParticleRenderParams>>& particlesRenderParams)
    : RSRenderPropertyAnimation(id, propertyId), particlesRenderParams_(particlesRenderParams),
      particleSystem_(std::make_shared<RSRenderParticleSystem>(particlesRenderParams_))
{}

void RSRenderParticleAnimation::DumpAnimationInfo(std::string& out) const
{
    out.append("Type:RSRenderParticleAnimation");
}

bool RSRenderParticleAnimation::Animate(int64_t time, int64_t& minLeftDelayTime, bool isCustom, bool isOnTree)
{
    RS_TRACE_NAME("RSRenderParticleAnimation::Animate");
    minLeftDelayTime = 0;
    auto target = GetTarget();
    if (!target) {
        return true;
    }
    if (!IsRunning()) {
        if (IsFinished()) {
            RemoveDrawModifier(target);
            return true;
        }
        return false;
    }
    if (!target->GetRenderProperties().GetVisible()) {
        RemoveDrawModifier(target);
        return true;
    }
    if (GetNeedUpdateStartTime()) {
        SetStartTime(time);
        return IsFinished();
    }

    FillRebuildProgress();
    int64_t deltaTime = time - animationFraction_.GetLastFrameTime();
    animationFraction_.SetLastFrameTime(time);
    runningTimeNs_ += deltaTime;
    if (particleSystem_ != nullptr) {
        particleSystem_->Emit(
            deltaTime, renderParticleVector_.renderParticleVector_, renderParticleVector_.imageVector_);
        particleSystem_->UpdateParticle(deltaTime, renderParticleVector_.renderParticleVector_);
    }
    if (auto property = std::static_pointer_cast<RSRenderProperty<RSRenderParticleVector>>(property_)) {
        property->Set(renderParticleVector_);
    }

    if (particleSystem_ == nullptr || particleSystem_->IsFinish(renderParticleVector_.renderParticleVector_)) {
        RemoveDrawModifier(target);
        return true;
    }
    return false;
}

void RSRenderParticleAnimation::RemoveDrawModifier(RSRenderNode* target)
{
    if (target == nullptr) {
        return;
    }
    if (auto modifierNG = property_->GetModifierNG().lock()) {
        target->RemoveModifierNG(modifierNG->GetId());
    }
}

void RSRenderParticleAnimation::FillRebuildProgress()
{
    if (rebuildRunningTimeNs_ <= 0 || particleSystem_ == nullptr) {
        return;
    }
    renderParticleVector_.renderParticleVector_.clear();
    particleSystem_->Warmup(
        rebuildRunningTimeNs_, renderParticleVector_.renderParticleVector_, renderParticleVector_.imageVector_);
    runningTimeNs_ = rebuildRunningTimeNs_;
    rebuildRunningTimeNs_ = 0;
    if (auto property = std::static_pointer_cast<RSRenderProperty<RSRenderParticleVector>>(property_)) {
        property->Set(renderParticleVector_);
    }
}

void RSRenderParticleAnimation::UpdateEmitter(const std::vector<std::shared_ptr<EmitterUpdater>>& emitterUpdaters)
{
    if (emitterUpdaters.empty()) {
        return;
    }
    for (auto emitterUpdater : emitterUpdaters) {
        if (emitterUpdater) {
            uint32_t index = emitterUpdater->emitterIndex_;
            if (index >= particlesRenderParams_.size()) {
                continue;
            }
            if (particlesRenderParams_[index] == nullptr) {
                continue;
            }
            if (emitterUpdater->position_.has_value() &&
                emitterUpdater->position_.value() != particlesRenderParams_[index]->emitterConfig_.position_) {
                particlesRenderParams_[index]->emitterConfig_.position_ = emitterUpdater->position_.value();
            }
            if (emitterUpdater->emitSize_.has_value() &&
                emitterUpdater->emitSize_.value() != particlesRenderParams_[index]->emitterConfig_.emitSize_) {
                particlesRenderParams_[index]->emitterConfig_.emitSize_ = emitterUpdater->emitSize_.value();
            }
            if (emitterUpdater->emitRate_.has_value() &&
                emitterUpdater->emitRate_.value() != particlesRenderParams_[index]->emitterConfig_.emitRate_) {
                particlesRenderParams_[index]->emitterConfig_.emitRate_ = emitterUpdater->emitRate_.value();
            }
            UpdateParamsIfChanged(emitterUpdater->shape_, particlesRenderParams_[index]->emitterConfig_.shape_);
        }
    }
    if (particleSystem_) {
        particleSystem_->UpdateEmitter(particlesRenderParams_);
    } else {
        particleSystem_ = std::make_shared<RSRenderParticleSystem>(particlesRenderParams_);
    }
}

void RSRenderParticleAnimation::UpdateNoiseField(const std::shared_ptr<ParticleNoiseFields>& particleNoiseFields)
{
    if (particleNoiseFields == nullptr) {
        return;
    } else if (particleNoiseFields_ != nullptr && *particleNoiseFields_ == *particleNoiseFields) {
        return;
    }
    particleNoiseFields_ = particleNoiseFields;
    if (particleSystem_) {
        particleSystem_->UpdateNoiseField(particleNoiseFields);
    }
}

void RSRenderParticleAnimation::UpdateRippleField(const std::shared_ptr<ParticleRippleFields>& particleRippleFields)
{
    if (particleRippleFields == nullptr) {
        return;
    } else if (particleRippleFields_ != nullptr && *particleRippleFields_ == *particleRippleFields) {
        return;
    }
    particleRippleFields_ = particleRippleFields;
    if (particleSystem_) {
        particleSystem_->UpdateRippleField(particleRippleFields);
    }
}

void RSRenderParticleAnimation::UpdateVelocityField(
    const std::shared_ptr<ParticleVelocityFields>& particleVelocityFields)
{
    if (particleVelocityFields == nullptr) {
        return;
    } else if (particleVelocityFields_ != nullptr && *particleVelocityFields_ == *particleVelocityFields) {
        return;
    }
    particleVelocityFields_ = particleVelocityFields;
    if (particleSystem_) {
        particleSystem_->UpdateVelocityField(particleVelocityFields);
    }
}

void RSRenderParticleAnimation::UpdateFields(const std::shared_ptr<ParticleFieldCollection>& fields)
{
    if (fields == nullptr) {
        return;
    } else if (particleFields_ != nullptr && *particleFields_ == *fields) {
        return;
    }
    particleFields_ = fields;
    if (particleSystem_) {
        particleSystem_->UpdateFields(fields);
    }
}

void RSRenderParticleAnimation::OnAttach()
{
    auto target = GetTarget();
    if (target == nullptr) {
        ROSEN_LOGE("RSRenderParticleAnimation::OnAttach, target is nullptr");
        return;
    }
    auto animationManager = target->GetAnimationManager();
    if (!animationManager) {
        return;
    }
    auto particleAnimations = animationManager->GetParticleAnimations();
    if (!particleAnimations.empty()) {
        for (const auto& pair : particleAnimations) {
            auto property = target->GetProperty(pair.first);
            auto modifierNG = property != nullptr ? property->GetModifierNG().lock() : nullptr;
            if (modifierNG != nullptr) {
                target->RemoveModifierNG(modifierNG->GetId());
            }
            animationManager->RemoveAnimation(pair.second);
            animationManager->UnregisterParticleAnimation(pair.first, pair.second);
        }
    }
    animationManager->RegisterParticleAnimation(GetPropertyId(), GetAnimationId());
}

void RSRenderParticleAnimation::OnDetach()
{
    auto target = GetTarget();
    if (target == nullptr) {
        ROSEN_LOGE("RSRenderParticleAnimation::OnDetach, target is nullptr");
        return;
    }
    if (particleSystem_ != nullptr) {
        particleSystem_->ClearEmitter();
        particleSystem_.reset();
    }
    auto propertyId = GetPropertyId();
    auto id = GetAnimationId();
    if (auto animationManager = target->GetAnimationManager()) {
        animationManager->UnregisterParticleAnimation(propertyId, id);
    }
}

bool RSRenderParticleAnimation::Marshalling(Parcel& parcel) const
{
    auto id = GetAnimationId();
    if (!(parcel.WriteUint64(id))) {
        ROSEN_LOGE("RSRenderParticleAnimation::Marshalling, write id failed");
        return false;
    }
    if (!parcel.WriteUint64(propertyId_)) {
        ROSEN_LOGE("RSRenderParticleAnimation::Marshalling, write PropertyId failed");
        return false;
    }
    if (!RSMarshallingHelper::Marshalling(parcel, particlesRenderParams_)) {
        ROSEN_LOGE("RSRenderParticleAnimation::Marshalling, write particlesRenderParams failed");
        return false;
    }
    // token for finding ui instance when executing animation callback in client
    if (!parcel.WriteUint64(GetToken())) {
        ROSEN_LOGE("multi-instance, RSRenderParticleAnimation::Marshalling, write token failed");
        return false;
    }
    if (!parcel.WriteInt32(GetRepeatCount())) {
        ROSEN_LOGE("RSRenderParticleAnimation::Marshalling, write repeatCount failed");
        return false;
    }
    if (!parcel.WriteInt64(rebuildRunningTimeNs_)) {
        ROSEN_LOGE("RSRenderParticleAnimation::Marshalling, write rebuildRunningTimeNs failed");
        return false;
    }
    if (!RSMarshallingHelper::Marshalling(parcel, particleNoiseFields_)) {
        ROSEN_LOGE("RSRenderParticleAnimation::Marshalling, write particleNoiseFields failed");
        return false;
    }
    if (!RSMarshallingHelper::Marshalling(parcel, particleRippleFields_)) {
        ROSEN_LOGE("RSRenderParticleAnimation::Marshalling, write particleRippleFields failed");
        return false;
    }
    if (!RSMarshallingHelper::Marshalling(parcel, particleVelocityFields_)) {
        ROSEN_LOGE("RSRenderParticleAnimation::Marshalling, write particleVelocityFields failed");
        return false;
    }
    return true;
}

RSRenderParticleAnimation* RSRenderParticleAnimation::Unmarshalling(Parcel& parcel)
{
    RSRenderParticleAnimation* renderParticleAnimation = new RSRenderParticleAnimation();
    if (!renderParticleAnimation->ParseParam(parcel)) {
        ROSEN_LOGE("RSRenderParticleAnimation::Unmarshalling, failed");
        delete renderParticleAnimation;
        return nullptr;
    }
    return renderParticleAnimation;
}

bool RSRenderParticleAnimation::ParseParam(Parcel& parcel)
{
    AnimationId id = 0;
    if (!RSMarshallingHelper::UnmarshallingPidPlusId(parcel, id)) {
        ROSEN_LOGE("RSRenderParticleAnimation::ParseParam, Unmarshalling animationId failed");
        return false;
    }
    SetAnimationId(id);
    if (!(RSMarshallingHelper::UnmarshallingPidPlusId(parcel, propertyId_) &&
            RSMarshallingHelper::Unmarshalling(parcel, particlesRenderParams_))) {
        ROSEN_LOGE("RSRenderParticleAnimation::ParseParam, Unmarshalling failed");
        return false;
    }
    particleSystem_ = std::make_shared<RSRenderParticleSystem>(particlesRenderParams_);
    uint64_t token = 0;
    if (!parcel.ReadUint64(token)) {
        ROSEN_LOGE("RSRenderParticleAnimation::ParseParam, Unmarshalling token failed");
        return false;
    }
    SetToken(token);
    int32_t repeatCount = 1;
    if (!parcel.ReadInt32(repeatCount)) {
        ROSEN_LOGE("RSRenderParticleAnimation::ParseParam, Unmarshalling repeatCount failed");
        return false;
    }
    SetRepeatCount(repeatCount);
    int64_t rebuildRunningTimeNs = 0;
    if (!parcel.ReadInt64(rebuildRunningTimeNs)) {
        ROSEN_LOGE("RSRenderParticleAnimation::ParseParam, Unmarshalling rebuildRunningTimeNs failed");
        return false;
    }
    rebuildRunningTimeNs_ = rebuildRunningTimeNs;
    if (!(RSMarshallingHelper::Unmarshalling(parcel, particleNoiseFields_) &&
            RSMarshallingHelper::Unmarshalling(parcel, particleRippleFields_) &&
            RSMarshallingHelper::Unmarshalling(parcel, particleVelocityFields_))) {
        ROSEN_LOGE("RSRenderParticleAnimation::ParseParam, Unmarshalling fields failed");
        return false;
    }
    if (particleNoiseFields_ != nullptr) {
        particleSystem_->UpdateNoiseField(particleNoiseFields_);
    }
    if (particleRippleFields_ != nullptr) {
        particleSystem_->UpdateRippleField(particleRippleFields_);
    }
    if (particleVelocityFields_ != nullptr) {
        particleSystem_->UpdateVelocityField(particleVelocityFields_);
    }
    return true;
}

} // namespace Rosen
} // namespace OHOS
