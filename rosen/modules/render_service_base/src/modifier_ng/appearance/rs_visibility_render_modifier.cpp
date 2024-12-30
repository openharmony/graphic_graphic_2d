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

#include "modifier_ng/appearance/rs_visibility_render_modifier.h"

#include "property/rs_properties.h"

namespace OHOS::Rosen::ModifierNG {
const RSVisibilityRenderModifier::LegacyPropertyApplierMap RSVisibilityRenderModifier::LegacyPropertyApplierMap_ = {
    { RSPropertyType::VISIBLE, RSRenderModifier::PropertyApplyHelper<bool, &RSProperties::SetVisible> }
};

void RSVisibilityRenderModifier::Draw(RSPaintFilterCanvas& canvas, Drawing::Rect& rect)
{
    // TODO:??
}

void RSVisibilityRenderModifier::ResetProperties(RSProperties& properties)
{
    properties.SetVisible(true);
}

/*
 * Protected Methods
 */
bool RSVisibilityRenderModifier::OnApply(RSModifierContext& context)
{
    // TODO: the if-else can be written as a template - later
    if (properties_.count(RSPropertyType::VISIBLE)) {
        stagingVisibility_ =
            std::static_pointer_cast<RSRenderProperty<bool>>(properties_.at(RSPropertyType::VISIBLE))->Get();
    } else {
        stagingVisibility_ = false;
    }
    return true;
}

void RSVisibilityRenderModifier::OnSync()
{
    std::swap(stagingVisibility_, renderVisibility_);
}
} // namespace OHOS::Rosen::ModifierNG
