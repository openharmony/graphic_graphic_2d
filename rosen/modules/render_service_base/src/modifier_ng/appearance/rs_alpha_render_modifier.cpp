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

#include "modifier_ng/appearance/rs_alpha_render_modifier.h"

#include "modifier/rs_render_property.h"
#include "property/rs_properties.h"

namespace OHOS::Rosen::ModifierNG {
const RSAlphaRenderModifier::LegacyPropertyApplierMap RSAlphaRenderModifier::LegacyPropertyApplierMap_ = {
    { RSPropertyType::ALPHA,
        RSRenderModifier::PropertyApplyHelperMultiply<float, &RSProperties::SetAlpha, &RSProperties::GetAlpha> },
    { RSPropertyType::ALPHA_OFFSCREEN, RSRenderModifier::PropertyApplyHelper<bool, &RSProperties::SetAlphaOffscreen> }
};

void RSAlphaRenderModifier::ResetProperties(RSProperties& properties)
{
    properties.SetAlpha(1.f);
    properties.SetAlphaOffscreen(false);
}

void RSAlphaRenderModifier::Draw(RSPaintFilterCanvas& canvas, Drawing::Rect& rect)
{
    if (renderAlphaOffscreen_) {
        Drawing::Brush brush;
        brush.SetAlpha(std::clamp(renderAlpha_, 0.f, 1.f) * UINT8_MAX);
        Drawing::SaveLayerOps slr(&rect, &brush);
        canvas.SaveLayer(slr);
    } else {
        canvas.MultiplyAlpha(renderAlpha_);
    }
}

/*
 * Protected Methods
 */
bool RSAlphaRenderModifier::OnApply(RSModifierContext& context)
{
    stagingAlpha_ = Getter(RSPropertyType::ALPHA, 1.0f);
    stagingAlphaOffscreen_ = Getter(RSPropertyType::ALPHA_OFFSCREEN, false);
    return ROSEN_EQ(stagingAlpha_, 1.0f);
}

void RSAlphaRenderModifier::OnSync()
{
    std::swap(stagingAlpha_, renderAlpha_);
    std::swap(stagingAlphaOffscreen_, renderAlphaOffscreen_);
}
} // namespace OHOS::Rosen::ModifierNG
