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

#include "modifier_ng/geometry/rs_frame_clip_modifier.h"

#include <memory>
#include <optional>

namespace OHOS::Rosen::ModifierNG {
void RSFrameClipModifier::SetClipToFrame(bool clipToFrame)
{
    Setter<RSProperty>(RSPropertyType::CLIP_TO_FRAME, clipToFrame);
}

void RSFrameClipModifier::SetCustomClipToFrame(const Vector4f& customClipToFrame)
{
    Setter(RSPropertyType::CUSTOM_CLIP_TO_FRAME, customClipToFrame);
}
} // namespace OHOS::Rosen::ModifierNG
