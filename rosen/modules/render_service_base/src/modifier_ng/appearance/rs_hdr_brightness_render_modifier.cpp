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

#include "modifier_ng/appearance/rs_hdr_brightness_render_modifier.h"

#include "pipeline/rs_render_node.h"

namespace OHOS::Rosen::ModifierNG {
const RSHDRBrightnessRenderModifier::LegacyPropertyApplierMap
    RSHDRBrightnessRenderModifier::LegacyPropertyApplierMap_ = {
        { RSPropertyType::HDR_UI_BRIGHTNESS,
            RSRenderModifier::PropertyApplyHelper<float, &RSProperties::SetHDRUIBrightness> },
        { RSPropertyType::HDR_BRIGHTNESS_FACTOR,
            RSRenderModifier::PropertyApplyHelper<float, &RSProperties::SetHDRBrightnessFactor> },
    };

void RSHDRBrightnessRenderModifier::ResetProperties(RSProperties& properties)
{
    properties.SetHDRUIBrightness(1.0f);
    properties.SetHDRBrightnessFactor(1.0f);
}

void RSHDRBrightnessRenderModifier::OnSetDirty()
{
    if (auto node = target_.lock()) {
        node->MarkNonGeometryChanged();
        node->SetContentDirty();
    }
}
} // namespace OHOS::Rosen::ModifierNG
