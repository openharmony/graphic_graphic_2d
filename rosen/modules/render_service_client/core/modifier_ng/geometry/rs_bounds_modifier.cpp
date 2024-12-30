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

#include "modifier_ng/geometry/rs_bounds_modifier.h"

#include <memory>
#include <optional>

namespace OHOS::Rosen::ModifierNG {
void RSBoundsModifier::SetBounds(Vector4f bounds)
{
    Setter(RSPropertyType::BOUNDS, bounds);
}

void RSBoundsModifier::SetBoundsSize(Vector2f size)
{
    Vector4f oldBounds = GetBounds();
    SetBounds({ oldBounds.x_, oldBounds.y_, size.x_, size.y_ });
}

void RSBoundsModifier::SetBoundsWidth(float width)
{
    Vector4f oldBounds = GetBounds();
    SetBounds({ oldBounds.x_, oldBounds.y_, width, oldBounds.w_ });
}

void RSBoundsModifier::SetBoundsHeight(float height)
{
    Vector4f oldBounds = GetBounds();
    SetBounds({ oldBounds.x_, oldBounds.y_, oldBounds.z_, height });
}

void RSBoundsModifier::SetBoundsPosition(Vector2f position)
{
    Vector4f oldBounds = GetBounds();
    SetBounds({ position.x_, position.y_, oldBounds.z_, oldBounds.w_ });
}

void RSBoundsModifier::SetBoundsPositionX(float positionX)
{
    Vector4f oldBounds = GetBounds();
    SetBounds({ positionX, oldBounds.y_, oldBounds.z_, oldBounds.w_ });
}

void RSBoundsModifier::SetBoundsPositionY(float positionY)
{
    Vector4f oldBounds = GetBounds();
    SetBounds({ oldBounds.x_, positionY, oldBounds.z_, oldBounds.w_ });
}

Vector4f RSBoundsModifier::GetBounds() const
{
    return Getter(RSPropertyType::BOUNDS, Vector4f());
}

Vector2f RSBoundsModifier::GetBoundsSize() const
{
    Vector4f bounds = GetBounds();
    return { bounds.z_, bounds.w_ };
}

float RSBoundsModifier::GetBoundsWidth() const
{
    return GetBounds().z_;
}

float RSBoundsModifier::GetBoundsHeight() const
{
    return GetBounds().w_;
}

Vector2f RSBoundsModifier::GetBoundsPosition() const
{
    Vector4f bounds = GetBounds();
    return { bounds.x_, bounds.y_ };
}

float RSBoundsModifier::GetBoundsPositionX() const
{
    return GetBounds().x_;
}

float RSBoundsModifier::GetBoundsPositionY() const
{
    return GetBounds().y_;
}
} // namespace OHOS::Rosen::ModifierNG
