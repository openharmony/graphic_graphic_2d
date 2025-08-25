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

#include "modifier_ng/geometry/rs_frame_clip_render_modifier.h"

#include "pipeline/rs_render_node.h"

namespace OHOS::Rosen::ModifierNG {
const RSFrameClipRenderModifier::LegacyPropertyApplierMap RSFrameClipRenderModifier::LegacyPropertyApplierMap_ = {
    { RSPropertyType::CLIP_TO_FRAME, RSRenderModifier::PropertyApplyHelper<bool, &RSProperties::SetClipToFrame> },
    { RSPropertyType::FRAME_GRAVITY, RSRenderModifier::PropertyApplyHelper<Gravity, &RSProperties::SetFrameGravity> },
};

void RSFrameClipRenderModifier::ResetProperties(RSProperties& properties)
{
    properties.SetClipToFrame(false);
    properties.SetFrameGravity(Gravity::DEFAULT);
}

void RSFrameClipRenderModifier::OnSetDirty()
{
    RSRenderModifier::OnSetDirty();
    if (HasProperty(RSPropertyType::CUSTOM_CLIP_TO_FRAME)) {
        if (auto node = target_.lock()) {
            node->SetContentDirty();
        }
    }
}
} // namespace OHOS::Rosen::ModifierNG
