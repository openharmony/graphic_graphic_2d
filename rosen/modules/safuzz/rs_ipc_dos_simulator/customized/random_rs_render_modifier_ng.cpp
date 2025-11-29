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

#include "common/safuzz_log.h"
#include "customized/random_rs_render_modifier_ng.h"

#include <memory>

#include "animation/rs_spring_model.h"
#include "modifier_ng/rs_modifier_ng_type.h"
#include "random/random_data.h"
#include "random/random_engine.h"

namespace OHOS {
namespace Rosen {

ModifierNG::RSModifierType RandomRSRenderModifier::GetRandomRSModifierType()
{
    static constexpr uint16_t MAX_INDEX = static_cast<uint16_t>(OHOS::Rosen::ModifierNG::RSModifierType::MAX) - 1;
    static constexpr uint16_t MIN_INDEX = static_cast<uint16_t>(OHOS::Rosen::ModifierNG::RSModifierType::INVALID) + 1;
    int randomIndex = RandomEngine::GetRandomIndex(MAX_INDEX, MIN_INDEX);
    auto modifierType = static_cast<OHOS::Rosen::ModifierNG::RSModifierType>(randomIndex);
    return modifierType;
}

std::shared_ptr<ModifierNG::RSRenderModifier> RandomRSRenderModifier::GetRandomRSRenderModifier()
{
    auto modifierType = GetRandomRSModifierType();
    auto& constructor = ModifierNG::RSRenderModifier::ConstructorLUT_[static_cast<uint16_t>(modifierType)];
    while (constructor == nullptr) {
        modifierType = GetRandomRSModifierType();
        constructor = ModifierNG::RSRenderModifier::ConstructorLUT_[static_cast<uint16_t>(modifierType)];
    }
    auto renderModifier = constructor();
    if (renderModifier == nullptr) {
        SAFUZZ_LOGE("RandomRSRenderModifier::GetRandomRSRenderModifier constructor return nullptr");
        return nullptr;
    }
    renderModifier->id_ = RandomData::GetRandomUint64();

    static constexpr uint16_t PROPERTY_SIZE_MAX = static_cast<uint16_t>(RandomRSPropertyType::MAX) - 1;
    static constexpr uint16_t PROPERTY_SIZE_MIN = static_cast<uint16_t>(RandomRSPropertyType::INVALID) + 1;
    uint16_t count = RandomEngine::GetRandomIndex(PROPERTY_SIZE_MAX, PROPERTY_SIZE_MIN);
    for (uint16_t i = 0; i < count; ++i) {
        auto renderPropertiesBase = RandomRSRenderPropertyBase::GetRandomRSRenderPropertyBase();
        if (renderPropertiesBase == nullptr) {
            continue;
        }
        ModifierNG::RSPropertyType propertyType = RandomRSRenderModifier::GetRandomRSPropertyType();
        renderModifier->AttachProperty(propertyType, renderPropertiesBase);
    }
    return renderModifier;
}

ModifierNG::RSPropertyType RandomRSRenderModifier::GetRandomRSPropertyType()
{
    static constexpr uint16_t MAX_INDEX = static_cast<uint16_t>(RandomRSPropertyType::MAX) - 1;
    static constexpr uint16_t MIN_INDEX = static_cast<uint16_t>(RandomRSPropertyType::INVALID) + 1;
    int randomIndex = RandomEngine::GetRandomIndex(MAX_INDEX, MIN_INDEX);
    auto propertyType = static_cast<ModifierNG::RSPropertyType>(randomIndex);
    return propertyType;
}
} // namespace Rosen
} // namespace OHOS