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

#include "modifier_ng/appearance/rs_pixel_stretch_modifier.h"

namespace OHOS::Rosen::ModifierNG {
void RSPixelStretchModifier::SetPixelStretchSize(const Vector4f& stretchSize)
{
    Setter(RSPropertyType::PIXEL_STRETCH_SIZE, stretchSize);
}

Vector4f RSPixelStretchModifier::GetPixelStretchSize() const
{
    return Getter<Vector4f>(RSPropertyType::PIXEL_STRETCH_SIZE, {});
}

void RSPixelStretchModifier::SetPixelStretchTileMode(int stretchTileMode)
{
    Setter<RSProperty>(RSPropertyType::PIXEL_STRETCH_TILE_MODE, stretchTileMode);
}

int RSPixelStretchModifier::GetPixelStretchTileMode() const
{
    return Getter<int>(RSPropertyType::PIXEL_STRETCH_TILE_MODE, {});
}

void RSPixelStretchModifier::SetPixelStretchPercent(const Vector4f& stretchPercent)
{
    Setter(RSPropertyType::PIXEL_STRETCH_PERCENT, stretchPercent);
}

Vector4f RSPixelStretchModifier::GetPixelStretchPercent() const
{
    return Getter<Vector4f>(RSPropertyType::PIXEL_STRETCH_PERCENT, {});
}
} // namespace OHOS::Rosen::ModifierNG
