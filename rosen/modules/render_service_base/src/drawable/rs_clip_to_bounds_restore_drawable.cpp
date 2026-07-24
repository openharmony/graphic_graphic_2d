/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "drawable/rs_clip_to_bounds_restore_drawable.h"

#include "drawable/rs_property_drawable_utils.h"
#include "effect/rs_render_shape_base.h"
#include "ge_visual_effect.h"
#include "ge_visual_effect_container.h"
#include "memory/rs_tag_tracker.h"
#include "pipeline/rs_render_node.h"
#include "platform/common/rs_log.h"
#include "property/rs_properties.h"

namespace OHOS::Rosen {
namespace DrawableV2 {
Drawing::Rect RSClipToBoundsRestoreDrawable::CalcSdfDrawRect(
    const RSRenderNode& node, const RSNGRenderShapeBase& sdfShape)
{
    const RSProperties& properties = node.GetRenderProperties();
    auto bounds = properties.GetBoundsRect();
    auto boundingBox = node.CalcBoundingBox();
    const auto& shapeRect = sdfShape.GetTransformDrawRect();
    auto rect = RSPropertyDrawableUtils::Rect2DrawingRect(
        (bounds == boundingBox && !shapeRect.IsEmpty()) ? shapeRect : boundingBox);
    rect.MakeOutset(1.0f, 1.0f); // expand 1px to avoid edge precision loss
    return rect;
}

RSDrawable::Ptr RSClipToBoundsRestoreDrawable::OnGenerate(const RSRenderNode& node, std::shared_ptr<uint32_t> content)
{
    auto ret = std::make_shared<RSClipToBoundsRestoreDrawable>(content);
    ret->OnUpdate(node);
    ret->OnSync();
    return std::move(ret);
}

bool RSClipToBoundsRestoreDrawable::OnUpdate(const RSRenderNode& node)
{
    const RSProperties& properties = node.GetRenderProperties();
    stagingSdfShape_ = nullptr;
    // SDF mode: no clipBounds + resolved SDF shape; otherwise standard RestoreToCount.
    // Always return true: this drawable must balance BG_SAVE_BOUNDS's Save in both modes.
    if (properties.GetClipBounds() != nullptr) {
        needSync_ = true;
        return true;
    }
    stagingSdfShape_ = RSPropertyDrawableUtils::GetResolvedSDFShape(properties);
    if (stagingSdfShape_) {
        stagingSdfDrawRect_ = CalcSdfDrawRect(node, *stagingSdfShape_);
    }
    needSync_ = true;
    return true;
}

void RSClipToBoundsRestoreDrawable::OnSync()
{
    if (!needSync_) {
        return;
    }
    sdfShape_ = stagingSdfShape_;
    sdfDrawRect_ = stagingSdfDrawRect_;
    needSync_ = false;
}

void RSClipToBoundsRestoreDrawable::OnDraw(Drawing::Canvas* canvas, const Drawing::Rect* rect) const
{
    auto paintFilterCanvas = static_cast<RSPaintFilterCanvas*>(canvas);
#ifdef RS_ENABLE_GPU
    RSTagTracker tagTracker(paintFilterCanvas ? paintFilterCanvas->GetGPUContext() : nullptr,
        RSTagTracker::SOURCETYPE::SOURCE_OTHER);
#endif
    if (!paintFilterCanvas) {
        ROSEN_LOGE("RSClipToBoundsRestoreDrawable::OnDraw paintFilterCanvas is nullptr");
        return;
    }
    if (sdfShape_) {
        auto geVisualEffect = sdfShape_->GenerateGEVisualEffect();
        auto geShape = geVisualEffect ? geVisualEffect->GenerateShaderShape() : nullptr;
        auto geFilter = std::make_shared<Drawing::GEVisualEffect>(
            Drawing::GE_SHADER_SDF_CLIP, Drawing::DrawingPaintType::BRUSH);
        geFilter->SetParam(Drawing::GE_SHADER_SDF_CLIP_SHAPE, geShape);
        auto geContainer = std::make_shared<Drawing::GEVisualEffectContainer>();
        geContainer->AddToChainedFilter(geFilter);
        // Pass the OnDraw rect (node frame, includes border) as the SDF shader geometry so the
        // border is not clipped (matches the original CLIP_SDF which used the OnDraw rect).
        RSPropertyDrawableUtils::DrawSdfClip(*paintFilterCanvas, geContainer, sdfDrawRect_, rect);
    }
    if (content_) {
        // Restore canvas state to balance BG_SAVE_BOUNDS's Save (both SDF and standard modes)
        paintFilterCanvas->RestoreToCount(*content_);
    }
}
} // namespace DrawableV2
} // namespace OHOS::Rosen
