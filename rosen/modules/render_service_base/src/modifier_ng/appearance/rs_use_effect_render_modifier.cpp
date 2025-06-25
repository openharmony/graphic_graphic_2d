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

#include "modifier_ng/appearance/rs_use_effect_render_modifier.h"

namespace OHOS::Rosen::ModifierNG {
const RSUseEffectRenderModifier::LegacyPropertyApplierMap RSUseEffectRenderModifier::LegacyPropertyApplierMap_ = {
    { RSPropertyType::USE_EFFECT, RSRenderModifier::PropertyApplyHelper<bool, &RSProperties::SetUseEffect> },
    { RSPropertyType::USE_EFFECT_TYPE, RSRenderModifier::PropertyApplyHelper<int, &RSProperties::SetUseEffectType> },
};

void RSUseEffectRenderModifier::ResetProperties(RSProperties& properties)
{
    properties.SetUseEffect(false);
    properties.SetUseEffectType(0);
}
} // namespace OHOS::Rosen::ModifierNG
