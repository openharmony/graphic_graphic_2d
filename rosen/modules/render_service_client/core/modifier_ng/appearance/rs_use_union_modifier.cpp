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

#include "modifier_ng/appearance/rs_use_union_modifier.h"

namespace OHOS::Rosen::ModifierNG {
void RSUseUnionModifier::SetUseUnion(bool useUnion)
{
    Setter<RSProperty>(RSPropertyType::USE_UNION, useUnion);
}

bool RSUseUnionModifier::GetUseUnion() const
{
    return Getter(RSPropertyType::USE_UNION, false);
}

void RSUseUnionModifier::SetUnionSpacing(float unionSpacing)
{
    Setter(RSPropertyType::UNION_SPACING, unionSpacing);
}

float RSUseUnionModifier::GetUnionSpacing() const
{
    return Getter(RSPropertyType::UNION_SPACING, 0.f);
}

void RSUseUnionModifier::SetSDFUnionMode(int sdfUnionMode)
{
    Setter<RSProperty>(RSPropertyType::SDF_UNION_MODE, sdfUnionMode);
}

int RSUseUnionModifier::GetSDFUnionMode() const
{
    return Getter(RSPropertyType::SDF_UNION_MODE, 0);
}

void RSUseUnionModifier::SetGravityPullCenterFlag(bool gravityPullCenterFlag)
{
    Setter<RSProperty>(RSPropertyType::GRAVITY_CENTER_FLAG, gravityPullCenterFlag);
}

bool RSUseUnionModifier::GetGravityPullCenterFlag() const
{
    return Getter(RSPropertyType::GRAVITY_CENTER_FLAG, false);
}

void RSUseUnionModifier::SetGravityPullStrength(float gravityPullStrength)
{
    Setter(RSPropertyType::GRAVITY_UNION_STRENGTH, gravityPullStrength);
}

float RSUseUnionModifier::GetGravityPullStrength() const
{
    return Getter(RSPropertyType::GRAVITY_UNION_STRENGTH, 0.f);
}

void RSUseUnionModifier::SetGravityHotZone(float gravityHotZone)
{
    Setter(RSPropertyType::GRAVITY_HOT_ZONE, gravityHotZone);
}

float RSUseUnionModifier::GetGravityHotZone() const
{
    return Getter(RSPropertyType::GRAVITY_HOT_ZONE, 0.f);
}
} // namespace OHOS::Rosen::ModifierNG
