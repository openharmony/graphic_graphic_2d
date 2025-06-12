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

#include "modifier_ng/appearance/rs_dynamic_light_up_modifier.h"

namespace OHOS::Rosen::ModifierNG {
void RSDynamicLightUpModifier::SetDynamicLightUpRate(const std::optional<float>& rate)
{
    SetterOptional<RSAnimatableProperty>(RSPropertyType::DYNAMIC_LIGHT_UP_RATE, rate);
}

std::optional<float> RSDynamicLightUpModifier::GetDynamicLightUpRate() const
{
    return GetterOptional<float>(RSPropertyType::DYNAMIC_LIGHT_UP_RATE);
}

void RSDynamicLightUpModifier::SetDynamicLightUpDegree(const std::optional<float>& lightUpDegree)
{
    SetterOptional<RSAnimatableProperty>(RSPropertyType::DYNAMIC_LIGHT_UP_DEGREE, lightUpDegree);
}

std::optional<float> RSDynamicLightUpModifier::GetDynamicLightUpDegree() const
{
    return GetterOptional<float>(RSPropertyType::DYNAMIC_LIGHT_UP_DEGREE);
}
} // namespace OHOS::Rosen::ModifierNG
