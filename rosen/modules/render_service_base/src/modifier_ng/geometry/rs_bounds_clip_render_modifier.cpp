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

#include "modifier_ng/geometry/rs_bounds_clip_render_modifier.h"

namespace OHOS::Rosen::ModifierNG {
const RSBoundsClipRenderModifier::LegacyPropertyApplierMap RSBoundsClipRenderModifier::LegacyPropertyApplierMap_ = {
    { RSPropertyType::CLIP_RRECT, RSRenderModifier::PropertyApplyHelper<RRect, &RSProperties::SetClipRRect> },
    { RSPropertyType::CLIP_BOUNDS,
        RSRenderModifier::PropertyApplyHelper<std::shared_ptr<RSPath>, &RSProperties::SetClipBounds> },
    { RSPropertyType::CLIP_TO_BOUNDS, RSRenderModifier::PropertyApplyHelper<bool, &RSProperties::SetClipToBounds> },
    { RSPropertyType::CORNER_RADIUS, RSRenderModifier::PropertyApplyHelperAdd<Vector4f, &RSProperties::SetCornerRadius,
                                         &RSProperties::GetCornerRadius> },
};

void RSBoundsClipRenderModifier::ResetProperties(RSProperties& properties)
{
    properties.SetClipRRect(RRect());
    properties.SetClipBounds(nullptr);
    properties.SetClipToBounds(false);
    properties.SetCornerRadius(Vector4f());
}
} // namespace OHOS::Rosen::ModifierNG
