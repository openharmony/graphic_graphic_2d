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

#include "modifier_ng/geometry/rs_bounds_clip_modifier.h"

namespace OHOS::Rosen::ModifierNG {
void RSBoundsClipModifier::SetClipRectWithRadius(const Vector4f& clipRect, const Vector4f& clipRadius)
{
    auto rrectPtr = std::make_shared<RRect>(clipRect, clipRadius);
    Setter(RSPropertyType::CLIP_RRECT, rrectPtr ? *rrectPtr : RRect());
}

void RSBoundsClipModifier::SetClipRRect(const std::shared_ptr<RRect>& rrect)
{
    Setter(RSPropertyType::CLIP_RRECT, rrect ? *rrect : RRect());
}

void RSBoundsClipModifier::SetClipBounds(const std::shared_ptr<RSPath>& clipToBounds)
{
    Setter<RSProperty>(RSPropertyType::CLIP_BOUNDS, clipToBounds);
}

void RSBoundsClipModifier::SetClipToBounds(bool clipToBounds)
{
    Setter<RSProperty>(RSPropertyType::CLIP_TO_BOUNDS, clipToBounds);
}

void RSBoundsClipModifier::SetCornerRadius(const Vector4f& cornerRadius)
{
    Setter(RSPropertyType::CORNER_RADIUS, cornerRadius);
}

Vector4f RSBoundsClipModifier::GetCornerRadius() const
{
    return Getter(RSPropertyType::CORNER_RADIUS, Vector4f(0.f));
}
} // namespace OHOS::Rosen::ModifierNG
