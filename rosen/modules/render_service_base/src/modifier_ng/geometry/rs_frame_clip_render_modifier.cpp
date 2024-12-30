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

#include "drawable/rs_property_drawable.h"
#include "drawable/rs_property_drawable_utils.h"
#include "drawable/rs_render_node_drawable_adapter.h"
#include "modifier_ng/rs_render_modifier_utils.h"
#include "pipeline/rs_recording_canvas.h"

namespace OHOS::Rosen::ModifierNG {
const RSFrameClipRenderModifier::LegacyPropertyApplierMap RSFrameClipRenderModifier::LegacyPropertyApplierMap_ = {
    { RSPropertyType::CLIP_TO_FRAME, RSRenderModifier::PropertyApplyHelper<bool, &RSProperties::SetClipToFrame> },
    // { RSPropertyType::CUSTOM_CLIP_TO_FRAME,
    //     RSRenderModifier::PropertyApplyHelper<Vector4f, &RSProperties::SetCustomClipToFrame> },
};

void RSFrameClipRenderModifier::ResetProperties(RSProperties& properties)
{
    properties.SetClipToFrame(false);
}

void RSFrameClipRenderModifier::Draw(RSPaintFilterCanvas& canvas, Drawing::Rect& rect)
{
    if (clipToFrame) {
        canvas.ClipRect(renderClipRect_, Drawing::ClipOp::INTERSECT, false);
    } else {
        canvas.ClipRect(renderClipRect_);
    }
}

void RSFrameClipRenderModifier::OnSync()
{
    std::swap(stagingClipRect_, renderClipRect_);
}

bool RSFrameClipRenderModifier::OnApply(RSModifierContext& context)
{
    if (HasProperty(RSPropertyType::CLIP_TO_FRAME)) {
        clipToFrame = Getter<bool>(RSPropertyType::CLIP_TO_FRAME);
        if (!clipToFrame) {
            stagingClipRect_ = RSPropertyDrawableUtils::Rect2DrawingRect({ context.frame_.GetLeft(),
                context.frame_.GetTop(), context.frame_.GetWidth(), context.frame_.GetHeight() });
            return true;
        }
    }
    if (HasProperty(RSPropertyType::CUSTOM_CLIP_TO_FRAME)) {
        auto customClipToFrame = Getter<Vector4f>(RSPropertyType::CUSTOM_CLIP_TO_FRAME);
        stagingClipRect_ =
            Drawing::Rect(customClipToFrame.x_, customClipToFrame.y_, customClipToFrame.z_, customClipToFrame.w_);
        return true;
    }
    return false;
}
} // namespace OHOS::Rosen::ModifierNG
