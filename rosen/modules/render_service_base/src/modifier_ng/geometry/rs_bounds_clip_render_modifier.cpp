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
#include "modifier_ng/geometry/rs_bounds_clip_render_modifier.h"

#include "drawable/rs_property_drawable_utils.h"
#include "modifier_ng/rs_render_modifier_utils.h"
#include "pipeline/rs_recording_canvas.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_render_node.h"

namespace OHOS::Rosen::ModifierNG {
const RSBoundsClipRenderModifier::LegacyPropertyApplierMap RSBoundsClipRenderModifier::LegacyPropertyApplierMap_ = {
    { RSPropertyType::CLIP_RRECT, RSRenderModifier::PropertyApplyHelper<RRect, &RSProperties::SetClipRRect> },
    { RSPropertyType::CLIP_BOUNDS,
        RSRenderModifier::PropertyApplyHelper<std::shared_ptr<RSPath>, &RSProperties::SetClipBounds> },
    { RSPropertyType::CLIP_TO_BOUNDS, RSRenderModifier::PropertyApplyHelper<bool, &RSProperties::SetClipToBounds> },
};

void RSBoundsClipRenderModifier::ResetProperties(RSProperties& properties)
{
    properties.SetClipRRect({});
    properties.SetClipBounds({});
    properties.SetClipToBounds(false);
}

bool RSBoundsClipRenderModifier::OnApply(RSModifierContext& context)
{
    clipRRect_ = Getter<std::shared_ptr<RRect>>(RSPropertyType::CLIP_RRECT);
    auto clipBounds_ = Getter<std::shared_ptr<RSPath>>(RSPropertyType::CLIP_BOUNDS);
    auto clipToBounds_ = Getter<bool>(RSPropertyType::CLIP_TO_BOUNDS, false);
    auto node = target_.lock();
    RSDisplayListModifierUpdater updater(this);
    auto& canvas = *updater.GetRecordingCanvas();
    if (clipBounds_ != nullptr) {
        canvas.ClipPath(clipBounds_->GetDrawingPath(), Drawing::ClipOp::INTERSECT, true);
    } else if (GetClipToRRect()) {
        canvas.ClipRoundRect(
            RSPropertyDrawableUtils::RRect2DrawingRRect(*clipRRect_), Drawing::ClipOp::INTERSECT, true);
    } else if (context.clipBoundsParams_.borderRadius_.IsZero()) {
        canvas.ClipRoundRect(
            RSPropertyDrawableUtils::RRect2DrawingRRect(context.GetRRect()), Drawing::ClipOp::INTERSECT, true);
    } else if (node->GetType() == RSRenderNodeType::SURFACE_NODE && RSSystemProperties::GetCacheEnabledForRotation() &&
               node->ReinterpretCastTo<RSSurfaceRenderNode>()->IsAppWindow()) {
        Drawing::Rect rect = RSPropertyDrawableUtils::Rect2DrawingRect(context.GetBoundsRect());
        Drawing::RectI iRect(rect.GetLeft(), rect.GetTop(), rect.GetRight(), rect.GetBottom());
        canvas.ClipIRect(iRect, Drawing::ClipOp::INTERSECT);
    } else {
        canvas.ClipRect(
            RSPropertyDrawableUtils::Rect2DrawingRect(context.GetBoundsRect()), Drawing::ClipOp::INTERSECT, false);
    }
    context.clipBoundsParams_.clipToBounds_ = clipToBounds_;
    context.clipBoundsParams_.clipBounds_ = clipBounds_;
    context.clipBoundsParams_.clipRRect_ = *clipRRect_;
    return true;
}

bool RSBoundsClipRenderModifier::GetClipToRRect()
{
    return clipRRect_ && !clipRRect_->rect_.IsEmpty();
}
} // namespace OHOS::Rosen::ModifierNG
