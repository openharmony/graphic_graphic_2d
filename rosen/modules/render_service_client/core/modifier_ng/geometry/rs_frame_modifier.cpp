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

#include "modifier_ng/geometry/rs_frame_modifier.h"

#include <memory>
#include <optional>

#include "rs_frame_modifier.h"
#include "modifier_ng/rs_modifier_ng.h"

namespace OHOS::Rosen::ModifierNG {
void RSFrameModifier::SetFrame(Vector4f frame)
{
    Setter(RSPropertyType::FRAME, frame);
}

void RSFrameModifier::SetFrameGravity(const Gravity& gravity)
{
    Setter<RSProperty, Gravity>(RSPropertyType::FRAME_GRAVITY, gravity);
}

void RSFrameModifier::SetFrameSize(Vector2f size)
{
    Vector4f oldFrame = GetFrame();
    SetFrame({ oldFrame.x_, oldFrame.y_, size.x_, size.y_ });
}

void RSFrameModifier::SetFrameWidth(float width)
{
    Vector4f oldFrame = GetFrame();
    SetFrame({ oldFrame.x_, oldFrame.y_, width, oldFrame.w_ });
}

void RSFrameModifier::SetFrameHeight(float height)
{
    Vector4f oldFrame = GetFrame();
    SetFrame({ oldFrame.x_, oldFrame.y_, oldFrame.z_, height });
}

void RSFrameModifier::SetFramePosition(Vector2f position)
{
    Vector4f oldFrame = GetFrame();
    SetFrame({ position.x_, position.y_, oldFrame.z_, oldFrame.w_ });
}

void RSFrameModifier::SetFramePositionX(float positionX)
{
    Vector4f oldFrame = GetFrame();
    SetFrame({ positionX, oldFrame.y_, oldFrame.z_, oldFrame.w_ });
}

void RSFrameModifier::SetFramePositionY(float positionY)
{
    Vector4f oldFrame = GetFrame();
    SetFrame({ oldFrame.x_, positionY, oldFrame.z_, oldFrame.w_ });
}

Vector4f RSFrameModifier::GetFrame() const
{
    return Getter(RSPropertyType::FRAME, Vector4f(0.0f, 0.0f, 0.0f, 0.0f));
}

Gravity RSFrameModifier::GetFrameGravity() const
{
    return Getter<Gravity>(RSPropertyType::FRAME_GRAVITY, {});
}

Vector2f RSFrameModifier::GetFrameSize() const
{
    Vector4f frame = GetFrame();
    return { frame.z_, frame.w_ };
}

float RSFrameModifier::GetFrameWidth() const
{
    return GetFrame().z_;
}

float RSFrameModifier::GetFrameHeight() const
{
    return GetFrame().w_;
}

Vector2f RSFrameModifier::GetFramePosition() const
{
    Vector4f frame = GetFrame();
    return { frame.x_, frame.y_ };
}

float RSFrameModifier::GetFramePositionX() const
{
    return GetFrame().x_;
}

float RSFrameModifier::GetFramePositionY() const
{
    return GetFrame().y_;
}
} // namespace OHOS::Rosen::ModifierNG
