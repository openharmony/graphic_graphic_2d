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

#include "modifier_ng/appearance/rs_border_render_modifier.h"

namespace OHOS::Rosen::ModifierNG {
const RSBorderRenderModifier::LegacyPropertyApplierMap RSBorderRenderModifier::LegacyPropertyApplierMap_ = {
    { RSPropertyType::BORDER_COLOR,
        RSRenderModifier::PropertyApplyHelper<Vector4<Color>, &RSProperties::SetBorderColor> },
    { RSPropertyType::BORDER_WIDTH, RSRenderModifier::PropertyApplyHelper<Vector4f, &RSProperties::SetBorderWidth> },
    { RSPropertyType::BORDER_STYLE,
        RSRenderModifier::PropertyApplyHelper<Vector4<uint32_t>, &RSProperties::SetBorderStyle> },
    { RSPropertyType::BORDER_DASH_WIDTH,
        RSRenderModifier::PropertyApplyHelper<Vector4f, &RSProperties::SetBorderDashWidth> },
    { RSPropertyType::BORDER_DASH_GAP,
        RSRenderModifier::PropertyApplyHelper<Vector4f, &RSProperties::SetBorderDashGap> },
};

void RSBorderRenderModifier::ResetProperties(RSProperties& properties)
{
    properties.ResetBorder(false);
}
} // namespace OHOS::Rosen::ModifierNG
