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
#include "modifier_ng/geometry/rs_frame_render_modifier.h"

#include "drawable/rs_property_drawable.h"
#include "modifier_ng/rs_render_modifier_utils.h"
#include "pipeline/rs_recording_canvas.h"
#include "property/rs_properties.h"

namespace OHOS::Rosen::ModifierNG {
const RSFrameRenderModifier::LegacyPropertyApplierMap RSFrameRenderModifier::LegacyPropertyApplierMap_ = {
    { RSPropertyType::FRAME, RSRenderModifier::PropertyApplyHelper<Vector4f, &RSProperties::SetFrame> },
    { RSPropertyType::FRAME_GRAVITY, RSRenderModifier::PropertyApplyHelper<Gravity, &RSProperties::SetFrameGravity> },
};

void RSFrameRenderModifier::ResetProperties(RSProperties& properties)
{
    properties.SetFrameGravity(Gravity::DEFAULT);
}

bool RSFrameRenderModifier::OnApply(RSModifierContext& context)
{
    if (!HasProperty(RSPropertyType::FRAME)) {
        return false;
    }

    auto frame = Getter<Vector4f>(RSPropertyType::FRAME, Vector4f());
    auto frameGravity = Getter<Gravity>(RSPropertyType::FRAME_GRAVITY, Gravity::DEFAULT);
    context.frame_.SetLeft(frame.x_);
    context.frame_.SetTop(frame.y_);
    context.frame_.SetRight(frame.z_);
    context.frame_.SetBottom(frame.w_);
    context.frameGravity_ = frameGravity;
    RSDisplayListModifierUpdater updater(this);
    CalculateFrameOffset(context);
    updater.GetRecordingCanvas()->Translate(frameOffsetX_, frameOffsetY_);
    return true;
}

void RSFrameRenderModifier::CalculateFrameOffset(RSModifierContext& context)
{
    frameOffsetX_ = context.frame_.GetLeft() - context.bounds_.GetLeft();
    frameOffsetY_ = context.frame_.GetTop() - context.bounds_.GetTop();
    if (isinf(frameOffsetX_)) {
        frameOffsetX_ = 0.f;
    }
    if (isinf(frameOffsetY_)) {
        frameOffsetY_ = 0.f;
    }
    if (frameOffsetX_ != 0.f || frameOffsetY_ != 0.f) {
        isDrawn_ = true;
    }
}
} // namespace OHOS::Rosen::ModifierNG
