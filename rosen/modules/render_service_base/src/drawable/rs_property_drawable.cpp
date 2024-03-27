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

#include "common/rs_optional_trace.h"
#include "drawable/rs_property_drawable_utils.h"
#include "pipeline/rs_recording_canvas.h"
#include "pipeline/rs_render_node.h"
#include "platform/common/rs_log.h"
#include "property/rs_filter_cache_manager.h"

namespace OHOS::Rosen {
constexpr int AIBAR_CACHE_UPDATE_INTERVAL = 5;
namespace DrawableV2 {
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
    clearFilterredCache_ = stagingClearFilterredCache_;
    
    filterHashChanged_ = false;
    filterRegionChanged_ = false;
    filterInteractWithDirty_ = false;
 
    stagingClearFilterredCache_ = false;
    clearType_ = CACHE_TYPE_BOTH;
    isLargeArea_ = false;
    isFilterCacheValid_ = false;
    needSync_ = false;
}

Drawing::RecordingCanvas::DrawFunc RSFilterDrawable::CreateDrawFunc() const
{
    auto ptr = std::static_pointer_cast<const RSFilterDrawable>(shared_from_this());
    return [ptr](Drawing::Canvas* canvas, const Drawing::Rect* rect) {
        if (canvas && ptr->filter_) {
            RSPropertyDrawableUtils::DrawFilter(canvas, ptr->filter_,
                ptr->cacheManager_, ptr->clearFilterredCache_, ptr->IsForeground());
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

void RSFilterDrawable::MarkFilterRegionIsLargeArea()
{
    isLargeArea_ = true;
}
 
void RSFilterDrawable::MarkNeedClearFilterCache()
{
    bool clearFilterredCacheAfterDrawing = IsClearFilterredSnapshotCacheAferDrawing();
    stagingClearFilterredCache_ = clearFilterredCacheAfterDrawing;
 
    RS_OPTIONAL_TRACE_NAME_FMT("RSFilterDrawable::MarkNeedClearFilterCache hashChanged:%d, regionChanged_:%d, "
        "belowDirty_:%d, isLastClearFilterredCahe:%d, isCrrentClearFilterredCahe:%d, "
        "cacheUpdateInterval_:%d, canSkip:%d, isLargeArea:%d", filterHashChanged_, filterRegionChanged_,
        filterInteractWithDirty_, lastClearIsFilterredCache_, clearFilterredCacheAfterDrawing,
        cacheUpdateInterval_, canSkipFrame_, isLargeArea_);
 
    // clear both two type cache: 1. filter region changed
    // 2. background changed and skip frame unenalbled
    // 3. background changed and last frame when skip-frame enabled
    // 4.clear snapshot cache last frame and clear filterred cache current frame
    if (filterRegionChanged_ || (filterInteractWithDirty_ && cacheUpdateInterval_ <= 0) ||
        (!lastClearIsFilterredCache_ && filterHashChanged_)) {
        if (filterInteractWithDirty_ && cacheUpdateInterval_ <= 0) {
            cacheUpdateInterval_ =filterType_ == RSFilter::AIBAR ? AIBAR_CACHE_UPDATE_INTERVAL : 
            (isLargeArea_ && canSkipFrame_ ? RSSystemProperties::GetFilterCacheUpdateInterval() : 0);
        }
        clearType_ = FilterCacheType::CACHE_TYPE_BOTH;
        lastClearIsFilterredCache_ = clearFilterredCacheAfterDrawing;
        isFilterCacheValid_ = false;
        return;
    }
 
    // when blur filter changes, we need to clear filterred cache if it valid.
    clearType_ = filterHashChanged_ ?
        FilterCacheType::CACHE_TYPE_FILTERED_SNAPSHOT : FilterCacheType::CACHE_TYPE_NONE;
    lastClearIsFilterredCache_ = clearFilterredCacheAfterDrawing;
    isFilterCacheValid_ = true;
    cacheUpdateInterval_--;
}
 
bool RSFilterDrawable::IsFilterCacheValid() const
{
    return isFilterCacheValid_;
}
 
void RSFilterDrawable::RecordFilterInfos(const std::shared_ptr<RSFilter>& rsFilter)
{
    filterHashChanged_ = true;
    auto filter = std::static_pointer_cast<RSDrawingFilter>(rsFilter);
    if (filter != nullptr) {
        filterType_ = filter->GetFilterType();
        canSkipFrame_ = filter->CanSkipFrame();
    }
}

void RSFilterDrawable::ClearFilterCache()
{
    if (!RSProperties::FilterCacheEnabled || cacheManager_ == nullptr || filter_ == nullptr) {
        ROSEN_LOGD("Clear filter cache faild or no need to clear cache, filterCacheEnbaled:%{public}d,"
            "cacheManager:%{public}d, filter:%{public}d", RSProperties::FilterCacheEnabled,
            cacheManager_ != nullptr, filter_ == nullptr);
        return;
    }
    RS_OPTIONAL_TRACE_NAME_FMT("RSFilterDrawable::ClearFilterCache clearType:%d", clearType_);
    cacheManager_->InvalidateFilterCache(clearType_);
}
 
bool RSFilterDrawable::IsClearFilterredSnapshotCacheAferDrawing()
{
    if (filterHashChanged_ && !filterRegionChanged_) {
        return true;
    }
    return false;
}
} // namespace DrawableV2
} // namespace OHOS::Rosen
