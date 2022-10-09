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

namespace OHOS {
namespace Rosen {
RSDirtyRegionManager::RSDirtyRegionManager()
{
    dirtyHistory_.resize(HISTORY_QUEUE_MAX_SIZE);
    debugRegionEnabled_.resize(DebugRegionType::TYPE_MAX);
}

void RSDirtyRegionManager::MergeDirtyRect(const RectI& rect)
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

void RSDirtyRegionManager::IntersectDirtyRect(const RectI& rect)
{
    dirtyRegion_ = dirtyRegion_.IntersectRect(rect);
}

void RSDirtyRegionManager::ClipDirtyRectWithinSurface()
{
    int left = std::max(std::max(dirtyRegion_.left_, 0), surfaceRect_.left_);
    int top = std::max(std::max(dirtyRegion_.top_, 0), surfaceRect_.top_);
    int width = std::min(dirtyRegion_.GetRight(), surfaceRect_.GetRight()) - left;
    int height = std::min(dirtyRegion_.GetBottom(), surfaceRect_.GetBottom()) - top;
    // If new region is invalid, dirtyRegion would be reset as [0, 0, 0, 0]
    dirtyRegion_ = ((width <= 0) || (height <= 0)) ? RectI() : RectI(left, top, width, height);
}

const RectI& RSDirtyRegionManager::GetDirtyRegion() const
{
    return dirtyRegion_;
}

RectI RSDirtyRegionManager::GetDirtyRegionFlipWithinSurface() const
{
    RectI glRect = dirtyRegion_;
    // left-top to left-bottom corner(in current surface)
    glRect.top_ = surfaceRect_.height_ - dirtyRegion_.top_ - dirtyRegion_.height_;
    return glRect;
}

RectI RSDirtyRegionManager::GetRectFlipWithinSurface(const RectI& rect) const
{
    RectI glRect = rect;
    // left-top to left-bottom corner(in current surface)
    glRect.top_ = surfaceRect_.height_ - rect.top_ - rect.height_;
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
    dirtyCanvasNodes_.clear();
    dirtySurfaceNodes_.clear();
    UpdateDebugRegionTypeEnable();
}

bool RSDirtyRegionManager::IsDirty() const
{
    return (dirtyRegion_.width_ > 0) && (dirtyRegion_.height_ > 0);
}

void RSDirtyRegionManager::UpdateDirty()
{
    PushHistory(dirtyRegion_);
    dirtyRegion_ = MergeHistory(bufferAge_, dirtyRegion_);
}

void RSDirtyRegionManager::UpdateDirtyByAligned(int32_t alignedBits)
{
    dirtyRegion_ = GetPixelAlignedRect(dirtyRegion_, alignedBits);
}

void RSDirtyRegionManager::UpdateDirtyCanvasNodes(NodeId id, const RectI& rect)
{
    dirtyCanvasNodes_[id] = rect;
}

void RSDirtyRegionManager::UpdateDirtySurfaceNodes(NodeId id, const RectI& rect)
{
    dirtySurfaceNodes_[id] = rect;
}

void RSDirtyRegionManager::GetDirtyCanvasNodes(std::map<NodeId, RectI>& target) const
{
    target = dirtyCanvasNodes_;
}

void RSDirtyRegionManager::GetDirtySurfaceNodes(std::map<NodeId, RectI>& target) const
{
    target = dirtySurfaceNodes_;
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

void RSDirtyRegionManager::ResetDirtyAsSurfaceSize()
{
    dirtyRegion_ = surfaceRect_;
}

void RSDirtyRegionManager::UpdateDebugRegionTypeEnable()
{
    DirtyRegionDebugType dirtyDebugType = RSSystemProperties::GetDirtyRegionDebugType();
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
    if (historySize_ == HISTORY_QUEUE_MAX_SIZE) {
        i = (i + historyHead_) % HISTORY_QUEUE_MAX_SIZE;
    }
    return dirtyHistory_[i];
}
} // namespace Rosen
} // namespace OHOS
