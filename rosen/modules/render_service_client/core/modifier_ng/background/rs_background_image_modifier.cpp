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

#include "modifier_ng/background/rs_background_image_modifier.h"

namespace OHOS::Rosen::ModifierNG {
void RSBackgroundImageModifier::SetBgImage(const std::shared_ptr<RSImage>& image)
{
    Setter<RSProperty, std::shared_ptr<RSImage>>(RSPropertyType::BG_IMAGE, image);
}

std::shared_ptr<RSImage> RSBackgroundImageModifier::GetBgImage() const
{
    return Getter<std::shared_ptr<RSImage>>(RSPropertyType::BG_IMAGE, nullptr);
}

void RSBackgroundImageModifier::SetBgImageInnerRect(const Vector4f& innerRect)
{
    Setter(RSPropertyType::BG_IMAGE_INNER_RECT, innerRect);
}

Vector4f RSBackgroundImageModifier::GetBgImageInnerRect() const
{
    return Getter(RSPropertyType::BG_IMAGE_INNER_RECT, Vector4f(0.f));
}

void RSBackgroundImageModifier::SetBgImageWidth(float width)
{
    Vector4f dstRect = RSBackgroundImageModifier::GetBgImageDstRect();
    dstRect[2] = width; // 2 index of width
    SetBgImageDstRect(dstRect);
}

float RSBackgroundImageModifier::GetBgImageWidth() const
{
    Vector4f dstRect = RSBackgroundImageModifier::GetBgImageDstRect();
    return dstRect[2]; // 2 index of width
}

void RSBackgroundImageModifier::SetBgImageHeight(float height)
{
    Vector4f dstRect = RSBackgroundImageModifier::GetBgImageDstRect();
    dstRect[3] = height; // 3 index of height
    SetBgImageDstRect(dstRect);
}

float RSBackgroundImageModifier::GetBgImageHeight() const
{
    Vector4f dstRect = RSBackgroundImageModifier::GetBgImageDstRect();
    return dstRect[3]; // 3 index of height
}

void RSBackgroundImageModifier::SetBgImagePositionX(float positionX)
{
    Vector4f dstRect = RSBackgroundImageModifier::GetBgImageDstRect();
    dstRect[0] = positionX; // 0 index of position x
    SetBgImageDstRect(dstRect);
}

float RSBackgroundImageModifier::GetBgImagePositionX() const
{
    Vector4f dstRect = RSBackgroundImageModifier::GetBgImageDstRect();
    return dstRect[0]; // 0 index of position x
}

void RSBackgroundImageModifier::SetBgImagePositionY(float positionY)
{
    Vector4f dstRect = RSBackgroundImageModifier::GetBgImageDstRect();
    dstRect[1] = positionY; // 1 index of position y
    SetBgImageDstRect(dstRect);
}

float RSBackgroundImageModifier::GetBgImagePositionY() const
{
    Vector4f dstRect = RSBackgroundImageModifier::GetBgImageDstRect();
    return dstRect[1]; // 1 index of position y
}

void RSBackgroundImageModifier::SetBgImageDstRect(const Vector4f& dstRect)
{
    Setter(RSPropertyType::BG_IMAGE_RECT, dstRect);
}

Vector4f RSBackgroundImageModifier::GetBgImageDstRect() const
{
    return Getter(RSPropertyType::BG_IMAGE_RECT, Vector4f(0.f));
}
} // namespace OHOS::Rosen::ModifierNG
