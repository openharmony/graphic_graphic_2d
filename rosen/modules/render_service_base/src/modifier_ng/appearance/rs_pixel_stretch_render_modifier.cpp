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

#include "modifier_ng/appearance/rs_pixel_stretch_render_modifier.h"

namespace OHOS::Rosen::ModifierNG {
const RSPixelStretchRenderModifier::LegacyPropertyApplierMap RSPixelStretchRenderModifier::LegacyPropertyApplierMap_ = {
    { RSPropertyType::PIXEL_STRETCH_SIZE,
        RSRenderModifier::PropertyApplyHelper<Vector4f, &RSProperties::SetPixelStretch> },
    { RSPropertyType::PIXEL_STRETCH_TILE_MODE,
        RSRenderModifier::PropertyApplyHelper<int, &RSProperties::SetPixelStretchTileMode> },
    { RSPropertyType::PIXEL_STRETCH_PERCENT,
        RSRenderModifier::PropertyApplyHelper<Vector4f, &RSProperties::SetPixelStretchPercent> },
};

void RSPixelStretchRenderModifier::ResetProperties(RSProperties& properties)
{
    properties.SetPixelStretch({});
    properties.SetPixelStretchPercent({});
    properties.SetPixelStretchTileMode(0);
}
} // namespace OHOS::Rosen::ModifierNG
