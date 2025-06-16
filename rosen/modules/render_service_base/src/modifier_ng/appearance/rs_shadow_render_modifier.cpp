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

#include "modifier_ng/appearance/rs_shadow_render_modifier.h"

#include "render/rs_shadow.h"

namespace OHOS::Rosen::ModifierNG {
const RSShadowRenderModifier::LegacyPropertyApplierMap RSShadowRenderModifier::LegacyPropertyApplierMap_ = {
    { RSPropertyType::SHADOW_COLOR, RSRenderModifier::PropertyApplyHelper<Color, &RSProperties::SetShadowColor> },
    { RSPropertyType::SHADOW_OFFSET_X, RSRenderModifier::PropertyApplyHelper<float, &RSProperties::SetShadowOffsetX> },
    { RSPropertyType::SHADOW_OFFSET_Y, RSRenderModifier::PropertyApplyHelper<float, &RSProperties::SetShadowOffsetY> },
    { RSPropertyType::SHADOW_ALPHA, RSRenderModifier::PropertyApplyHelper<float, &RSProperties::SetShadowAlpha> },
    { RSPropertyType::SHADOW_ELEVATION,
        RSRenderModifier::PropertyApplyHelper<float, &RSProperties::SetShadowElevation> },
    { RSPropertyType::SHADOW_RADIUS, RSRenderModifier::PropertyApplyHelper<float, &RSProperties::SetShadowRadius> },
    { RSPropertyType::SHADOW_PATH,
        RSRenderModifier::PropertyApplyHelper<std::shared_ptr<RSPath>, &RSProperties::SetShadowPath> },
    { RSPropertyType::SHADOW_MASK, RSRenderModifier::PropertyApplyHelper<int, &RSProperties::SetShadowMask> },
    { RSPropertyType::SHADOW_COLOR_STRATEGY,
        RSRenderModifier::PropertyApplyHelper<int, &RSProperties::SetShadowColorStrategy> },
    { RSPropertyType::SHADOW_IS_FILLED, RSRenderModifier::PropertyApplyHelper<bool, &RSProperties::SetShadowIsFilled> },
    { RSPropertyType::USE_SHADOW_BATCHING,
        RSRenderModifier::PropertyApplyHelper<bool, &RSProperties::SetUseShadowBatching> },
};

void RSShadowRenderModifier::ResetProperties(RSProperties& properties)
{
    properties.SetShadowColor(Color());
    properties.SetShadowOffsetX(0.f);
    properties.SetShadowOffsetY(0.f);
    properties.SetShadowAlpha(0.f);
    properties.SetShadowElevation(0.f);
    properties.SetShadowRadius(0.f);
    properties.SetShadowPath(nullptr);
    properties.SetShadowMask(SHADOW_MASK_STRATEGY::MASK_NONE);
    properties.SetShadowColorStrategy(static_cast<int>(SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_NONE));
    properties.SetShadowIsFilled(false);
    properties.SetUseShadowBatching(false);
}
} // namespace OHOS::Rosen::ModifierNG
