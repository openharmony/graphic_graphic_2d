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
#include "modifier_ng/geometry/rs_bounds_render_modifier.h"

#include "property/rs_properties.h"

namespace OHOS::Rosen::ModifierNG {
const RSBoundsRenderModifier::LegacyPropertyApplierMap RSBoundsRenderModifier::LegacyPropertyApplierMap_ = {
    { RSPropertyType::BOUNDS, RSRenderModifier::PropertyApplyHelper<Vector4f, &RSProperties::SetBounds> },
};

bool RSBoundsRenderModifier::OnApply(RSModifierContext& context)
{
    auto bounds = Getter<Vector4f>(RSPropertyType::BOUNDS, Vector4f());
    auto cornerRadius = Getter<Vector4f>(RSPropertyType::CORNER_RADIUS, Vector4f());
    context.bounds_.SetLeft(bounds.x_);
    context.bounds_.SetTop(bounds.y_);
    context.bounds_.SetRight(bounds.z_);
    context.bounds_.SetBottom(bounds.w_);
    context.clipBoundsParams_.borderRadius_ = cornerRadius;
    return true;
}
} // namespace OHOS::Rosen::ModifierNG
