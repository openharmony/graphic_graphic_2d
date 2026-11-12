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

#include "animation/rs_render_particle_pool.h"

#include <algorithm>
#include <cstddef>

#include "animation/rs_particle_ripple_field.h"
#include "platform/common/rs_log.h"
namespace OHOS {
namespace Rosen {
constexpr int64_t MAX_PRESIZE_PARTICLES = 65536;

RSRenderParticleSystem::RSRenderParticleSystem(
    const std::vector<std::shared_ptr<ParticleRenderParams>>& particlesRenderParams)
    : particlesRenderParams_(particlesRenderParams)
{
    particlePool_ = std::make_shared<RSRenderParticlePool>();
    CreateEmitter();
}

void RSRenderParticleSystem::EnsureParticlePool()
{
    if (particlePool_ == nullptr) {
        particlePool_ = std::make_shared<RSRenderParticlePool>();
    }
}

void RSRenderParticleSystem::ResetParticlePool()
{
    particlePool_ = std::make_shared<RSRenderParticlePool>();
}

const std::shared_ptr<ParticleFieldCollection>& RSRenderParticleSystem::GetLegacyFields()
{
    if (legacyFields_ == nullptr) {
        legacyFields_ = std::make_shared<ParticleFieldCollection>();
    }
    legacyFields_->Clear();
    if (particleNoiseFields_ != nullptr) {
        for (const auto& field : particleNoiseFields_->fields_) {
            legacyFields_->Add(field);
        }
    }
    if (particleRippleFields_ != nullptr) {
        for (const auto& field : particleRippleFields_->rippleFields_) {
            legacyFields_->Add(field);
        }
    }
    if (particleVelocityFields_ != nullptr) {
        for (const auto& field : particleVelocityFields_->velocityFields_) {
            legacyFields_->Add(field);
        }
    }
    return legacyFields_;
}

void RSRenderParticleSystem::CreateEmitter()
{
    emitters_.clear();
    emitterParams_.clear();
    imageVector_.clear();
    // A re-created emitter set starts a new population: stale slots would point at old parameter indices.
    EnsureParticlePool();
    particlePool_->Clear();
    // Reserve from the configured count; an above-cap count is treated as runaway and left to grow on demand.
    size_t imageIndex = 0;
    int64_t presize = 0;
    for (auto& particleRenderParams : particlesRenderParams_) {
        if (particleRenderParams == nullptr) {
            continue;
        }
        const int64_t configuredCount = particleRenderParams->GetParticleCount();
        if (configuredCount > 0) {
            presize = std::min<int64_t>(presize + configuredCount, MAX_PRESIZE_PARTICLES);
        }
        if (particleRenderParams->GetParticleType() == ParticleType::IMAGES) {
            particleRenderParams->SetImageIndex(imageIndex++);
            imageVector_.push_back(particleRenderParams->GetParticleImage());
        }
        emitterParams_.push_back(particleRenderParams);
        emitters_.push_back(std::make_shared<RSRenderParticleEmitter>(particleRenderParams));
    }
    if (particlePool_ != nullptr && presize > 0) {
        particlePool_->Resize(static_cast<size_t>(presize));
    }
}

void RSRenderParticleSystem::ClearEmitter()
{
    emitters_.clear();
    emitterParams_.clear();
}

void RSRenderParticleSystem::EmitFromEmitter(int64_t deltaTime, size_t emitterIndex)
{
    if (emitterIndex >= emitters_.size() || emitters_[emitterIndex] == nullptr) {
        return;
    }
    emitters_[emitterIndex]->EmitParticle(*particlePool_, static_cast<uint32_t>(emitterIndex), deltaTime);
}

void RSRenderParticleSystem::Emit(int64_t deltaTime, std::vector<std::shared_ptr<RSImage>>& imageVector)
{
    EnsureParticlePool();
    for (size_t i = 0; i < emitters_.size(); i++) {
        if (particlePool_->Count() >= PARTICLE_UPPER_LIMIT) {
            break;
        }
        EmitFromEmitter(deltaTime, i);
    }
    imageVector = imageVector_;
}
void RSRenderParticleSystem::UpdateParticle(int64_t deltaTime)
{
    if (particlePool_ == nullptr || particlePool_->Count() == 0) {
        return;
    }
    const float deltaSeconds = static_cast<float>(deltaTime) / static_cast<float>(NS_TO_S);
    std::shared_ptr<ParticleFieldCollection> fields = particleFields_;
    if (fields != nullptr) {
        fields->UpdateAll(deltaSeconds);
    } else {
        if (particleRippleFields_ != nullptr) {
            particleRippleFields_->UpdateAllRipples(deltaSeconds);
        }
        fields = GetLegacyFields();
    }
    groupedSlots_.resize(emitterParams_.size());
    for (auto& group : groupedSlots_) {
        group.clear();
    }
    for (size_t slot = 0; slot < particlePool_->Count(); slot++) {
        if (!particlePool_->IsAlive(slot)) {
            continue;
        }
        const uint32_t index = particlePool_->Data().paramsIndex[slot];
        if (index < groupedSlots_.size()) {
            groupedSlots_[index].push_back(slot);
        }
    }
    for (size_t index = 0; index < groupedSlots_.size(); index++) {
        if (groupedSlots_[index].empty() || emitterParams_[index] == nullptr) {
            continue;
        }
        UpdateGroup(*particlePool_, groupedSlots_[index], *emitterParams_[index], fields, deltaTime);
    }
    particlePool_->Update(deltaSeconds);
    if (particlePool_->HasDead()) {
        particlePool_->Compact();
    }
}

void RSRenderParticleSystem::Warmup(int64_t totalTimeNs, std::vector<std::shared_ptr<RSImage>>& imageVector)
{
    if (totalTimeNs <= 0) {
        return;
    }
    EnsureParticlePool();
    WarmupFields(totalTimeNs);
    for (size_t i = 0; i < emitters_.size(); i++) {
        if (particlePool_->Count() >= PARTICLE_UPPER_LIMIT) {
            break;
        }
        WarmupEmitter(i, totalTimeNs);
    }
    if (particlePool_->HasDead()) {
        particlePool_->Compact();
    }
    imageVector = imageVector_;
}

void RSRenderParticleSystem::WarmupFields(int64_t totalTimeNs)
{
    const float totalSec = static_cast<float>(totalTimeNs) / NS_TO_S;
    if (particleRippleFields_ != nullptr) {
        for (auto& rf : particleRippleFields_->rippleFields_) {
            if (rf != nullptr) {
                rf->lifeTime_ = 0.f;
            }
        }
        particleRippleFields_->UpdateAllRipples(totalSec);
    }
    if (particleFields_ != nullptr) {
        for (auto& f : particleFields_->fields_) {
            if (f != nullptr && f->GetType() == ParticleFieldType::RIPPLE) {
                static_cast<ParticleRippleField*>(f.get())->lifeTime_ = 0.f;
            }
        }
        particleFields_->UpdateAll(totalSec);
    }
}

void RSRenderParticleSystem::WarmupEmitter(size_t emitterIndex, int64_t totalTimeNs)
{
    constexpr int MAX_EMIT_RATE = 5000;
    constexpr int64_t INFINITE_LIFE_NS = -1 * NS_PER_MS;
    constexpr int MAX_FILL_PER_EMITTER = 20000;
    if (emitterIndex >= emitters_.size() || emitters_[emitterIndex] == nullptr) {
        return;
    }
    const auto& params = emitters_[emitterIndex]->GetParticleParams();
    if (params == nullptr) {
        return;
    }
    int emitRate = std::min(params->GetEmitRate(), MAX_EMIT_RATE);
    if (emitRate <= 0) {
        return;
    }
    int maxParticle = params->GetParticleCount();
    bool infiniteCount = (maxParticle < 0);
    int64_t lifeStartNs = params->GetLifeTimeStartValue();
    int64_t lifeEndNs = params->GetLifeTimeEndValue();
    bool infiniteLife = (lifeStartNs == INFINITE_LIFE_NS && lifeEndNs == INFINITE_LIFE_NS);
    const float totalSec = static_cast<float>(totalTimeNs) / NS_TO_S;
    float emittedF = static_cast<float>(emitRate) * totalSec;
    if (!infiniteCount) {
        emittedF = std::min(emittedF, static_cast<float>(maxParticle));
    }
    int emittedCount = std::min(static_cast<int>(emittedF), MAX_FILL_PER_EMITTER);
    std::shared_ptr<ParticleFieldCollection> fields = particleFields_;
    if (fields == nullptr) {
        fields = GetLegacyFields();
    }
    for (int k = 0; k < emittedCount; k++) {
        int64_t activeTimeNs = static_cast<int64_t>(static_cast<float>(k) / static_cast<float>(emitRate) * NS_TO_S);
        if (activeTimeNs > totalTimeNs || (!infiniteLife && activeTimeNs >= lifeEndNs)) {
            break;
        }
        const size_t slot = particlePool_->Acquire();
        InitParticle(*particlePool_, slot, *params, static_cast<uint32_t>(emitterIndex));
        UpdateToActiveTime(*particlePool_, slot, *params, fields, activeTimeNs);
        if (!particlePool_->IsAlive(slot)) {
            particlePool_->MarkDead(slot);
        }
    }
    emitters_[emitterIndex]->SetEmittedCount(emittedF);
}

bool RSRenderParticleSystem::IsFinish()
{
    if (particlePool_ != nullptr && particlePool_->Count() > 0) {
        return false;
    }
    bool finish = true;
    for (auto& emitter : emitters_) {
        if (emitter != nullptr) {
            finish = finish && emitter->IsEmitterFinish();
        }
    }
    return finish;
}

void RSRenderParticleSystem::UpdateEmitter(
    const std::vector<std::shared_ptr<ParticleRenderParams>>& particlesRenderParams)
{
    particlesRenderParams_ = particlesRenderParams;
}

void RSRenderParticleSystem::UpdateNoiseField(const std::shared_ptr<ParticleNoiseFields>& particleNoiseFields)
{
    particleNoiseFields_ = particleNoiseFields;
}

void RSRenderParticleSystem::UpdateRippleField(const std::shared_ptr<ParticleRippleFields>& particleRippleFields,
    bool isIncrementalUpdate)
{
    if (!isIncrementalUpdate) {
        particleRippleFields_ = particleRippleFields;
        return;
    }

    if (!particleRippleFields || !particleRippleFields_) {
        particleRippleFields_ = particleRippleFields;
        return;
    }

    auto& existingFields = particleRippleFields_->rippleFields_;
    auto& newFields = particleRippleFields->rippleFields_;

    for (size_t i = 0; i < newFields.size() && i < existingFields.size(); ++i) {
        const auto& newField = newFields[i];
        const auto& existingField = existingFields[i];

        if (newField && existingField &&
            newField->center_ == existingField->center_ &&
            ROSEN_EQ(newField->amplitude_, existingField->amplitude_) &&
            ROSEN_EQ(newField->wavelength_, existingField->wavelength_) &&
            ROSEN_EQ(newField->waveSpeed_, existingField->waveSpeed_) &&
            ROSEN_EQ(newField->attenuation_, existingField->attenuation_) &&
            newField->regionShape_ == existingField->regionShape_ &&
            newField->regionPosition_ == existingField->regionPosition_ &&
            newField->regionSize_ == existingField->regionSize_) {
            newField->lifeTime_ = existingField->lifeTime_;
        }
    }

    particleRippleFields_ = particleRippleFields;
}

void RSRenderParticleSystem::UpdateVelocityField(const std::shared_ptr<ParticleVelocityFields>& particleVelocityFields)
{
    particleVelocityFields_ = particleVelocityFields;
}

namespace {
constexpr uint32_t FIELD_TYPE_MAX = static_cast<uint32_t>(ParticleFieldType::MAX);

// Collect existing fields of a specific type (used for ripple lifeTime preservation).
std::vector<std::shared_ptr<ParticleFieldBase>> CollectFieldsByType(
    const ParticleFieldCollection& fields, ParticleFieldType type)
{
    std::vector<std::shared_ptr<ParticleFieldBase>> result;
    for (const auto& f : fields.fields_) {
        if (f != nullptr && f->GetType() == type) {
            result.push_back(f);
        }
    }
    return result;
}

// Check if two ripples have identical parameters (excluding lifeTime_).
bool IsSameRippleParam(const ParticleRippleField& lhs, const ParticleRippleField& rhs)
{
    return lhs.center_ == rhs.center_ &&
           ROSEN_EQ(lhs.amplitude_, rhs.amplitude_) &&
           ROSEN_EQ(lhs.wavelength_, rhs.wavelength_) &&
           ROSEN_EQ(lhs.waveSpeed_, rhs.waveSpeed_) &&
           ROSEN_EQ(lhs.attenuation_, rhs.attenuation_) &&
           lhs.regionShape_ == rhs.regionShape_ &&
           lhs.regionPosition_ == rhs.regionPosition_ &&
           lhs.regionSize_ == rhs.regionSize_;
}

// Preserve lifeTime_ from the matching old ripple (by index) when parameters are identical.
void TryPreserveRippleLifeTime(
    ParticleFieldBase& newField,
    const std::vector<std::shared_ptr<ParticleFieldBase>>& oldRipples,
    size_t rippleIndex)
{
    if (rippleIndex >= oldRipples.size()) {
        return;
    }
    const auto& oldPtr = oldRipples[rippleIndex];
    if (oldPtr == nullptr) {
        return;
    }
    if (newField.GetType() != ParticleFieldType::RIPPLE ||
        oldPtr->GetType() != ParticleFieldType::RIPPLE) {
        return;
    }
    auto& newRipple = static_cast<ParticleRippleField&>(newField);
    const auto& oldRipple = static_cast<const ParticleRippleField&>(*oldPtr);
    if (IsSameRippleParam(newRipple, oldRipple)) {
        newRipple.lifeTime_ = oldRipple.lifeTime_;
    }
}
} // namespace

void RSRenderParticleSystem::UpdateFields(const std::shared_ptr<ParticleFieldCollection>& fields)
{
    if (fields == nullptr) {
        return;
    }
    if (particleFields_ == nullptr) {
        particleFields_ = fields;
        return;
    }
    // Guard against self-assignment (same Collection passed in twice).
    if (particleFields_.get() == fields.get()) {
        return;
    }

    // Snapshot the new fields into a local vector to avoid any aliasing risk
    // with particleFields_->fields_ during the mutation below.
    const std::vector<std::shared_ptr<ParticleFieldBase>> newFields = fields->fields_;

    bool hasType[FIELD_TYPE_MAX] = {};
    for (const auto& f : newFields) {
        if (f == nullptr) {
            continue;
        }
        uint32_t idx = static_cast<uint32_t>(f->GetType());
        if (idx < FIELD_TYPE_MAX) {
            hasType[idx] = true;
        }
    }

    std::vector<std::shared_ptr<ParticleFieldBase>> oldRipples;
    if (hasType[static_cast<uint32_t>(ParticleFieldType::RIPPLE)]) {
        oldRipples = CollectFieldsByType(*particleFields_, ParticleFieldType::RIPPLE);
    }

    for (uint32_t i = 0; i < FIELD_TYPE_MAX; ++i) {
        if (hasType[i]) {
            particleFields_->RemoveByType(static_cast<ParticleFieldType>(i));
        }
    }

    size_t rippleIndex = 0;
    for (const auto& newField : newFields) {
        if (newField == nullptr) {
            continue;
        }
        if (newField->GetType() == ParticleFieldType::RIPPLE) {
            TryPreserveRippleLifeTime(*newField, oldRipples, rippleIndex);
            rippleIndex++;
        }
        particleFields_->Add(newField);
    }
}
} // namespace Rosen
} // namespace OHOS
