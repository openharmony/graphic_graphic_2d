/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "pipeline/rs_dirty_region_manager.h"

#include <string>

#include "rs_trace.h"

#include "platform/common/rs_log.h"
namespace OHOS {
namespace Rosen {
RSDirtyRegionManager::RSDirtyRegionManager()
{
    dirtyHistory_.resize(HISTORY_QUEUE_MAX_SIZE);
    debugRegionEnabled_.resize(DebugRegionType::TYPE_MAX);
}

RSDirtyRegionManager::RSDirtyRegionManager(bool isDisplayDirtyManager)
{
    dirtyHistory_.resize(HISTORY_QUEUE_MAX_SIZE);
    debugRegionEnabled_.resize(DebugRegionType::TYPE_MAX);
    isDisplayDirtyManager_ = isDisplayDirtyManager;
}

void RSDirtyRegionManager::MergeDirtyRect(const RectI& rect, bool isDebugRect)
{
    if (rect.IsEmpty()) {
        return;
    }
    if (currentFrameDirtyRegion_.IsEmpty()) {
        currentFrameDirtyRegion_ = rect;
    } else {
        currentFrameDirtyRegion_ = currentFrameDirtyRegion_.JoinRect(rect);
    }
    if (isDisplayDirtyManager_) {
        mergedDirtyRegions_.emplace_back(rect);
    }
    if (isDebugRect) {
        debugRect_ = rect;
    }
}

bool RSDirtyRegionManager::MergeDirtyRectIfIntersect(const RectI& rect)
{
    if (!currentFrameDirtyRegion_.Intersect(rect)) {
        return false;
    }
    currentFrameDirtyRegion_ = currentFrameDirtyRegion_.JoinRect(rect);
    if (isDisplayDirtyManager_) {
        mergedDirtyRegions_.emplace_back(rect);
    }
    return true;
}

void RSDirtyRegionManager::MergeDirtyRectAfterMergeHistory(const RectI& rect)
{
    if (rect.IsEmpty()) {
        return;
    }
    if (dirtyRegion_.IsEmpty()) {
        dirtyRegion_ = rect;
    } else {
        dirtyRegion_ = dirtyRegion_.JoinRect(rect);
    }
}

void RSDirtyRegionManager::UpdateVisitedDirtyRects(const std::vector<RectI>& rects)
{
    if (rects.empty()) {
        visitedDirtyRegions_.clear();
        return;
    }
    visitedDirtyRegions_ = rects;
}

RectI RSDirtyRegionManager::GetIntersectedVisitedDirtyRect(const RectI& absRect) const
{
    RectI belowDirty = currentFrameDirtyRegion_;
    for (const auto& subDirty : visitedDirtyRegions_) {
        if (absRect.IsInsideOf(belowDirty)) {
            return belowDirty;
        }
        belowDirty = belowDirty.JoinRect(subDirty.IntersectRect(absRect));
    }
    return belowDirty;
}

void RSDirtyRegionManager::UpdateCacheableFilterRect(const RectI& rect)
{
    if (rect.IsEmpty()) {
        return;
    }
    cacheableFilterRects_.emplace_back(rect);
}

bool RSDirtyRegionManager::IfCacheableFilterRectFullyCover(const RectI& targetRect)
{
    for (auto rect : cacheableFilterRects_) {
        if (targetRect.IsInsideOf(rect)) {
            return true;
        }
    }
    return false;
}

void RSDirtyRegionManager::IntersectDirtyRect(const RectI& rect)
{
    currentFrameDirtyRegion_ = currentFrameDirtyRegion_.IntersectRect(rect);
}

void RSDirtyRegionManager::ClipDirtyRectWithinSurface()
{
    int left = std::max(std::max(currentFrameDirtyRegion_.left_, 0), surfaceRect_.left_);
    int top = std::max(std::max(currentFrameDirtyRegion_.top_, 0), surfaceRect_.top_);
    int width = std::min(currentFrameDirtyRegion_.GetRight(), surfaceRect_.GetRight()) - left;
    int height = std::min(currentFrameDirtyRegion_.GetBottom(), surfaceRect_.GetBottom()) - top;
    // If new region is invalid, currentFrameDirtyRegion_ would be reset as [0, 0, 0, 0]
    currentFrameDirtyRegion_ = ((width <= 0) || (height <= 0)) ? RectI() : RectI(left, top, width, height);
}

const RectI& RSDirtyRegionManager::GetCurrentFrameDirtyRegion()
{
    return currentFrameDirtyRegion_;
}

const RectI& RSDirtyRegionManager::GetDirtyRegion() const
{
    return dirtyRegion_;
}

void RSDirtyRegionManager::SetCurrentFrameDirtyRect(const RectI& dirtyRect)
{
    currentFrameDirtyRegion_ = dirtyRect;
}

void RSDirtyRegionManager::OnSync(std::shared_ptr<RSDirtyRegionManager> targetManager)
{
    if (!targetManager) {
        return;
    }
    targetManager->surfaceRect_ = surfaceRect_;
    targetManager->dirtyRegion_ = dirtyRegion_;
    targetManager->currentFrameDirtyRegion_ = currentFrameDirtyRegion_;
    targetManager->debugRect_ = debugRect_;
    if (RSSystemProperties::GetDirtyRegionDebugType() != DirtyRegionDebugType::DISABLED) {
        targetManager->dirtySurfaceNodeInfo_ = dirtySurfaceNodeInfo_;
        targetManager->dirtyCanvasNodeInfo_ = dirtyCanvasNodeInfo_;
    }
}

RectI RSDirtyRegionManager::GetDirtyRegionFlipWithinSurface() const
{
    RectI glRect;
    if (isDirtyRegionAlignedEnable_) {
        glRect = GetPixelAlignedRect(dirtyRegion_);
    } else {
        glRect = dirtyRegion_;
    }

    if (!RSSystemProperties::IsUseVulkan()) {
        // left-top to left-bottom corner(in current surface)
        glRect.top_ = surfaceRect_.height_ - glRect.top_ - glRect.height_;
    }
    return glRect;
}

RectI RSDirtyRegionManager::GetRectFlipWithinSurface(const RectI& rect) const
{
    RectI glRect = rect;

    if (!RSSystemProperties::IsUseVulkan()) {
        // left-top to left-bottom corner(in current surface)
        glRect.top_ = surfaceRect_.height_ - rect.top_ - rect.height_;
    }
    return glRect;
}

const RectI& RSDirtyRegionManager::GetLatestDirtyRegion() const
{
    if (historyHead_ < 0) {
        return dirtyRegion_;
    }
    return dirtyHistory_[historyHead_];
}

RectI RSDirtyRegionManager::GetPixelAlignedRect(const RectI& rect, int32_t alignedBits)
{
    RectI newRect = rect;
    if (alignedBits > 1) {
        int32_t left = (rect.left_ / alignedBits) * alignedBits;
        int32_t top = (rect.top_ / alignedBits) * alignedBits;
        int32_t width = ((rect.GetRight() + alignedBits - 1) / alignedBits) * alignedBits - left;
        int32_t height = ((rect.GetBottom() + alignedBits - 1) / alignedBits) * alignedBits - top;
        newRect = RectI(left, top, width, height);
    }
    return newRect;
}

void RSDirtyRegionManager::Clear()
{
    dirtyRegion_.Clear();
    currentFrameDirtyRegion_.Clear();
    visitedDirtyRegions_.clear();
    mergedDirtyRegions_.clear();
    cacheableFilterRects_.clear();
    dirtyCanvasNodeInfo_.clear();
    dirtyCanvasNodeInfo_.resize(DirtyRegionType::TYPE_AMOUNT);
    dirtySurfaceNodeInfo_.clear();
    dirtySurfaceNodeInfo_.resize(DirtyRegionType::TYPE_AMOUNT);
    isDfxTarget_ = false;
    isFilterCacheRectValid_ = true;
}

bool RSDirtyRegionManager::IsCurrentFrameDirty() const
{
    return !currentFrameDirtyRegion_.IsEmpty() && currentFrameDirtyRegion_ != debugRect_;
}

bool RSDirtyRegionManager::IsDirty() const
{
    return !dirtyRegion_.IsEmpty();
}

void RSDirtyRegionManager::UpdateDirty(bool enableAligned)
{
    if (enableAligned) {
        UpdateDirtyByAligned();
    }
    // if last frame doesn't align and current frame need align, we should align history ditry regions.
    if (!isDirtyRegionAlignedEnable_ && enableAligned) {
        AlignHistory();
    }
    isDirtyRegionAlignedEnable_ = enableAligned;
    PushHistory(currentFrameDirtyRegion_);
    dirtyRegion_ = MergeHistory(bufferAge_, currentFrameDirtyRegion_);
}

void RSDirtyRegionManager::UpdateDirtyByAligned(int32_t alignedBits)
{
    currentFrameDirtyRegion_ = GetPixelAlignedRect(currentFrameDirtyRegion_, alignedBits);
}

void RSDirtyRegionManager::UpdateDirtyRegionInfoForDfx(NodeId id, RSRenderNodeType nodeType,
    DirtyRegionType dirtyType, const RectI& rect)
{
    if (dirtyType >= DirtyRegionType::TYPE_AMOUNT || dirtyType < 0 || rect.IsEmpty()) {
        return;
    }
    if (nodeType == RSRenderNodeType::CANVAS_NODE) {
        dirtyCanvasNodeInfo_[dirtyType].emplace(std::make_pair(id, rect));
    } else if (nodeType == RSRenderNodeType::SURFACE_NODE) {
        dirtySurfaceNodeInfo_[dirtyType].emplace(std::make_pair(id, rect));
    }
}

void RSDirtyRegionManager::GetDirtyRegionInfo(std::map<NodeId, RectI>& target,
    RSRenderNodeType nodeType, DirtyRegionType dirtyType) const
{
    target.clear();
    if (dirtyType >= DirtyRegionType::TYPE_AMOUNT || dirtyType < 0) {
        return;
    }
    if (nodeType == RSRenderNodeType::CANVAS_NODE) {
        target = dirtyCanvasNodeInfo_[dirtyType];
    } else if (nodeType == RSRenderNodeType::SURFACE_NODE) {
        target = dirtySurfaceNodeInfo_[dirtyType];
    }
}

bool RSDirtyRegionManager::HasOffset()
{
    return hasOffset_;
}
void RSDirtyRegionManager::SetOffset(int offsetX, int offsetY)
{
    offsetX_ = offsetX;
    offsetY_ = offsetY;
}
RectI RSDirtyRegionManager::GetOffsetedDirtyRegion() const
{
    return GetDirtyRegion().Offset(offsetX_, offsetY_);
}

bool RSDirtyRegionManager::SetBufferAge(const int age)
{
    if (age < 0) {
        bufferAge_ = 0; // reset invalid age
        return false;
    }
    bufferAge_ = static_cast<unsigned int>(age);
    return true;
}

bool RSDirtyRegionManager::SetSurfaceSize(const int32_t width, const int32_t height)
{
    if (width < 0 || height < 0) {
        return false;
    }
    surfaceRect_ = RectI(0, 0, width, height);
    return true;
}

void RSDirtyRegionManager::MergeSurfaceRect()
{
    return MergeDirtyRect(GetSurfaceRect());
}

void RSDirtyRegionManager::ResetDirtyAsSurfaceSize()
{
    dirtyRegion_ = surfaceRect_;
    currentFrameDirtyRegion_ = surfaceRect_;
}

void RSDirtyRegionManager::UpdateDebugRegionTypeEnable(DirtyRegionDebugType dirtyDebugType)
{
    debugRegionEnabled_.assign(DebugRegionType::TYPE_MAX, false);
    switch (dirtyDebugType) {
        case DirtyRegionDebugType::CURRENT_SUB:
            debugRegionEnabled_[DebugRegionType::CURRENT_SUB] = true;
            break;
        case DirtyRegionDebugType::CURRENT_WHOLE:
            debugRegionEnabled_[DebugRegionType::CURRENT_WHOLE] = true;
            break;
        case DirtyRegionDebugType::MULTI_HISTORY:
            debugRegionEnabled_[DebugRegionType::MULTI_HISTORY] = true;
            break;
        case DirtyRegionDebugType::CURRENT_SUB_AND_WHOLE:
            debugRegionEnabled_[DebugRegionType::CURRENT_SUB] = true;
            debugRegionEnabled_[DebugRegionType::CURRENT_WHOLE] = true;
            break;
        case DirtyRegionDebugType::CURRENT_WHOLE_AND_MULTI_HISTORY:
            debugRegionEnabled_[DebugRegionType::CURRENT_WHOLE] = true;
            debugRegionEnabled_[DebugRegionType::MULTI_HISTORY] = true;
            break;
        case DirtyRegionDebugType::EGL_DAMAGE:
            debugRegionEnabled_[DebugRegionType::EGL_DAMAGE] = true;
            break;
        case DirtyRegionDebugType::DISABLED:
        default:
            break;
    }
}

RectI RSDirtyRegionManager::MergeHistory(unsigned int age, RectI rect) const
{
    if (age == 0 || age > historySize_) {
        return surfaceRect_;
    }
    // GetHistory(historySize_) is equal to dirtyHistory_[historyHead_] (latest his rect)
    // therefore, this loop merges rect with age frames' dirtyRect
    // Attention: should not set i >= 0 for unsigned int!!!!!
    for (unsigned int i = historySize_; i > historySize_ - age; --i) {
        auto subRect = GetHistory((i - 1));
        if (subRect.IsEmpty()) {
            continue;
        }
        if (rect.IsEmpty()) {
            rect = subRect;
            continue;
        }
        // only join valid his dirty region
        rect = rect.JoinRect(subRect);
    }
    return rect;
}

void RSDirtyRegionManager::PushHistory(RectI rect)
{
    int next = (historyHead_ + 1) % HISTORY_QUEUE_MAX_SIZE;
    dirtyHistory_[next] = rect;
    if (historySize_ < HISTORY_QUEUE_MAX_SIZE) {
        ++historySize_;
    }
    historyHead_ = next;
}

RectI RSDirtyRegionManager::GetHistory(unsigned int i) const
{
    if (i >= HISTORY_QUEUE_MAX_SIZE) {
        i %= HISTORY_QUEUE_MAX_SIZE;
    }
    if (historySize_ > 0) {
        i = (i + historyHead_) % historySize_;
    }
    return dirtyHistory_[i];
}

void RSDirtyRegionManager::AlignHistory()
{
    for (uint8_t i = 0; i < dirtyHistory_.size(); i++) {
        dirtyHistory_[i] = GetPixelAlignedRect(dirtyHistory_[i]);
    }
}

} // namespace Rosen
} // namespace OHOS
