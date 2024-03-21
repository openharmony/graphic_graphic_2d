/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "drawable/rs_property_drawable.h"

#include "rs_trace.h"

#include "drawable/rs_property_drawable_utils.h"
#include "pipeline/rs_recording_canvas.h"
#include "pipeline/rs_render_node.h"
#include "platform/common/rs_log.h"
#include "property/rs_filter_cache_manager.h"

namespace OHOS::Rosen {
void RSPropertyDrawable::OnSync()
{
    if (!needSync_) {
        return;
    }
    drawCmdList_ = std::move(stagingDrawCmdList_);
    needSync_ = false;
}

Drawing::RecordingCanvas::DrawFunc RSPropertyDrawable::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSPropertyDrawable>(shared_from_this());
    return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) { ptr->drawCmdList_->Playback(*canvas); };
}

// ============================================================================
// Updater
RSPropertyDrawCmdListUpdater::RSPropertyDrawCmdListUpdater(int width, int height, RSPropertyDrawable* target)
    : target_(target)
{
    // PLANNING: use RSRenderNode to determine the correct recording canvas size
    recordingCanvas_ = ExtendRecordingCanvas::Obtain(10, 10, false);
}

RSPropertyDrawCmdListUpdater::~RSPropertyDrawCmdListUpdater()
{
    if (recordingCanvas_ && target_) {
        target_->stagingDrawCmdList_ = recordingCanvas_->GetDrawCmdList();
        target_->needSync_ = true;
        ExtendRecordingCanvas::Recycle(recordingCanvas_);
        recordingCanvas_.reset();
        target_ = nullptr;
    } else {
        ROSEN_LOGE("Update failed, recording canvas is null!");
    }
}

const std::unique_ptr<ExtendRecordingCanvas>& RSPropertyDrawCmdListUpdater::GetRecordingCanvas() const
{
    return recordingCanvas_;
}

// ============================================================================
RSDrawable::Ptr RSFrameOffsetDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSFrameOffsetDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
};

bool RSFrameOffsetDrawable::OnUpdate(const RSRenderNode& node)
{
    const RSProperties& properties = node.GetRenderProperties();
    auto frameOffsetX = properties.GetFrameOffsetX();
    auto frameOffsetY = properties.GetFrameOffsetY();
    if (frameOffsetX == 0 && frameOffsetY == 0) {
        return false;
    }

    // regenerate stagingDrawCmdList_
    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    updater.GetRecordingCanvas()->Translate(frameOffsetX, frameOffsetY);
    return true;
}

// ============================================================================
RSDrawable::Ptr RSClipToBoundsDrawable::OnGenerate(const RSRenderNode& node)
{
    auto ret = std::make_shared<RSClipToBoundsDrawable>();
    ret->OnUpdate(node);
    ret->OnSync();
    return std::move(ret);
};

bool RSClipToBoundsDrawable::OnUpdate(const RSRenderNode& node)
{
    const RSProperties& properties = node.GetRenderProperties();
    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    auto& canvas = *updater.GetRecordingCanvas();
    if (properties.GetClipBounds() != nullptr) {
        canvas.ClipPath(properties.GetClipBounds()->GetDrawingPath(), Drawing::ClipOp::INTERSECT, true);
    } else if (properties.GetClipToRRect()) {
        canvas.ClipRoundRect(
            RSPropertyDrawableUtils::RRect2DrawingRRect(properties.GetClipRRect()), Drawing::ClipOp::INTERSECT, false);
    } else if (!properties.GetCornerRadius().IsZero()) {
        canvas.ClipRoundRect(
            RSPropertyDrawableUtils::RRect2DrawingRRect(properties.GetRRect()), Drawing::ClipOp::INTERSECT, true);
    } else {
        canvas.ClipRect(
            RSPropertyDrawableUtils::Rect2DrawingRect(properties.GetBoundsRect()), Drawing::ClipOp::INTERSECT, true);
    }
    return true;
}

RSDrawable::Ptr RSClipToFrameDrawable::OnGenerate(const RSRenderNode& node)
{
    if (auto ret = std::make_shared<RSClipToFrameDrawable>(); ret->OnUpdate(node)) {
        return std::move(ret);
    }
    return nullptr;
}

bool RSClipToFrameDrawable::OnUpdate(const RSRenderNode& node)
{
    const RSProperties& properties = node.GetRenderProperties();
    if (!properties.GetClipToFrame()) {
        return false;
    }

    RSPropertyDrawCmdListUpdater updater(0, 0, this);
    updater.GetRecordingCanvas()->ClipRect(
        RSPropertyDrawableUtils::Rect2DrawingRect(properties.GetFrameRect()), Drawing::ClipOp::INTERSECT, false);
    return true;
}

RSFilterDrawable::RSFilterDrawable()
{
    if (RSProperties::FilterCacheEnabled) {
        cacheManager_ = std::make_unique<RSFilterCacheManager>();
    }
}
 
void RSFilterDrawable::OnSync()
{
    if (needSync_) {
        filter_ = std::move(stagingFilter_);
        needSync_ = false;
    }
 
    ClearFilterCache();
 
    filterRegionChanged_ = false;
    filterInteractWithDirty_ = false;
    needSync_ = false;
}
 
Drawing::RecordingCanvas::DrawFunc RSFilterDrawable::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSFilterDrawable>(shared_from_this());
    return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
        if (canvas && ptr->filter_) {
            RSPropertyDrawableUtils::DrawFilter(canvas, ptr->filter_, ptr->cacheManager_, ptr->IsForeground());
        }
    };
}
 
void RSFilterDrawable::MarkFilterRegionChanged()
{
    filterRegionChanged_ = true;
}
 
void RSFilterDrawable::MarkFilterRegionInteractWithDirty()
{
    filterInteractWithDirty_ = true;
}
 
void RSFilterDrawable::ClearFilterCache()
{
    if (cacheManager_ == nullptr || filter_ == nullptr) {
        return;
    }
 
    cacheManager_->UpdateCacheStateWithFilterHash(filter_);
    if (cacheManager_->IsCacheValid() && filterRegionChanged_) {
        cacheManager_->UpdateCacheStateWithFilterRegion();
    }
 
    if (cacheManager_->IsCacheValid() && filterInteractWithDirty_) {
        cacheManager_->UpdateCacheStateWithDirtyRegion();
    }
}
} // namespace OHOS::Rosen
