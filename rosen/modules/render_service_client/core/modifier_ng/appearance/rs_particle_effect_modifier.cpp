/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "modifier_ng/appearance/rs_particle_effect_modifier.h"

namespace OHOS::Rosen::ModifierNG {
void RSParticleEffectModifier::SetEmitterUpdater(const std::vector<std::shared_ptr<EmitterUpdater>>& para)
{
    Setter<RSProperty, std::vector<std::shared_ptr<EmitterUpdater>>>(RSPropertyType::PARTICLE_EMITTER_UPDATER, para);
}

std::vector<std::shared_ptr<EmitterUpdater>> RSParticleEffectModifier::GetEmitterUpdater() const
{
    return Getter<std::vector<std::shared_ptr<EmitterUpdater>>>(RSPropertyType::PARTICLE_EMITTER_UPDATER, {});
}

void RSParticleEffectModifier::SetParticleNoiseFields(const std::shared_ptr<ParticleNoiseFields>& para)
{
    Setter<RSProperty, std::shared_ptr<ParticleNoiseFields>>(RSPropertyType::PARTICLE_NOISE_FIELD, para);
}

std::shared_ptr<ParticleNoiseFields> RSParticleEffectModifier::GetParticleNoiseFields() const
{
    return Getter<std::shared_ptr<ParticleNoiseFields>>(RSPropertyType::PARTICLE_NOISE_FIELD, {});
}
} // namespace OHOS::Rosen::ModifierNG
