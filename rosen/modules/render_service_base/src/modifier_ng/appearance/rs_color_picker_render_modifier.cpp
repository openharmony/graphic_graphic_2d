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

#include "modifier_ng/appearance/rs_color_picker_render_modifier.h"
#include "draw/color.h"
#include "platform/common/rs_log.h"

namespace OHOS::Rosen::ModifierNG {
const RSColorPickerRenderModifier::LegacyPropertyApplierMap
    RSColorPickerRenderModifier::LegacyPropertyApplierMap_ = {
        { RSPropertyType::COLOR_PICKER_PLACEHOLDER, RSRenderModifier::PropertyApplyHelper<int,
            &RSProperties::SetColorPickerPlaceholder> },
        { RSPropertyType::COLOR_PICKER_STRATEGY, RSRenderModifier::PropertyApplyHelper<int,
            &RSProperties::SetColorPickerStrategy> },
        { RSPropertyType::COLOR_PICKER_INTERVAL, RSRenderModifier::PropertyApplyHelper<int,
            &RSProperties::SetColorPickerInterval> },
        { RSPropertyType::COLOR_PICKER_NOTIFY_THRESHOLD, RSRenderModifier::PropertyApplyHelper<int,
            &RSProperties::SetColorPickerNotifyThreshold> },
        { RSPropertyType::COLOR_PICKER_RECT, RSRenderModifier::PropertyApplyHelper<Vector4f,
            &RSProperties::SetColorPickerRect> },
    };

void RSColorPickerRenderModifier::ResetProperties(RSProperties& properties)
{
    properties.SetColorPickerPlaceholder(static_cast<int>(ColorPlaceholder::NONE));
    properties.SetColorPickerStrategy(static_cast<int>(ColorPickStrategyType::NONE));
    properties.SetColorPickerInterval(0);
    properties.SetColorPickerNotifyThreshold(0); // packed value: both dark and light thresholds = 0
}
} // namespace OHOS::Rosen::ModifierNG
