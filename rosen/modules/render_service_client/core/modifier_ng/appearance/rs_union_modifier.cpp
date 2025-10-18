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

#include "modifier_ng/appearance/rs_union_modifier.h"

namespace OHOS::Rosen::ModifierNG {

void RSUnionModifier::SetUseUnion(bool useUnion)
{
    Setter<RSProperty>(RSPropertyType::USE_UNION, useUnion);
}

bool RSUnionModifier::GetUseUnion() const
{
    return Getter(RSPropertyType::USE_UNION, false);
}

void RSUnionModifier::SetUnionSpacing(float spacing)
{
    Setter(RSPropertyType::UNION_SPACING, spacing);
}

float RSUnionModifier::GetUnionSpacing() const
{
    return Getter(RSPropertyType::UNION_SPACING, 0.f);
}

void RSUnionModifier::SetSDFMask(const std::shared_ptr<RSNGMaskBase>& mask)
{
    Setter<RSProperty, std::shared_ptr<RSNGMaskBase>>(RSPropertyType::SDF_MASK, mask);
}

std::shared_ptr<RSNGMaskBase> RSUnionModifier::GetSDFMask() const
{
    return Getter<std::shared_ptr<RSNGMaskBase>>(RSPropertyType::SDF_MASK, nullptr);
}
} // namespace OHOS::Rosen::ModifierNG
