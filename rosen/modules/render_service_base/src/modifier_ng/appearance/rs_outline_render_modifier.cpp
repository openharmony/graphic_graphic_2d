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

#include "modifier_ng/appearance/rs_outline_render_modifier.h"

namespace OHOS::Rosen::ModifierNG {
const RSOutlineRenderModifier::LegacyPropertyApplierMap RSOutlineRenderModifier::LegacyPropertyApplierMap_ = {
    { RSPropertyType::OUTLINE_COLOR,
        RSRenderModifier::PropertyApplyHelper<Vector4<Color>, &RSProperties::SetOutlineColor> },
    { RSPropertyType::OUTLINE_WIDTH, RSRenderModifier::PropertyApplyHelper<Vector4f, &RSProperties::SetOutlineWidth> },
    { RSPropertyType::OUTLINE_STYLE,
        RSRenderModifier::PropertyApplyHelper<Vector4<uint32_t>, &RSProperties::SetOutlineStyle> },
    { RSPropertyType::OUTLINE_DASH_WIDTH,
        RSRenderModifier::PropertyApplyHelper<Vector4f, &RSProperties::SetOutlineDashWidth> },
    { RSPropertyType::OUTLINE_DASH_GAP,
        RSRenderModifier::PropertyApplyHelper<Vector4f, &RSProperties::SetOutlineDashGap> },
    { RSPropertyType::OUTLINE_RADIUS, RSRenderModifier::PropertyApplyHelperAdd<Vector4f,
                                          &RSProperties::SetOutlineRadius, &RSProperties::GetOutlineRadius> },
};

void RSOutlineRenderModifier::ResetProperties(RSProperties& properties)
{
    properties.ResetBorder(true);
}
} // namespace OHOS::Rosen::ModifierNG
