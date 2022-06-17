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
    if ((dirtyRegion_.width_ <= 0) || (dirtyRegion_.height_ <= 0)) {
        dirtyRegion_ = rect;
    } else {
        dirtyRegion_ = dirtyRegion_.JoinRect(rect);
    }
}

void RSDirtyRegionManager::IntersectDirtyRect(const RectI& rect)
{
    dirtyRegion_ = dirtyRegion_.IntersectRect(rect);
}

const RectI& RSDirtyRegionManager::GetDirtyRegion() const
{
    return dirtyRegion_;
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
    if (bufferAge_ == 0) {
        dirtyRegion_.left_ = 0;
        dirtyRegion_.top_ = 0;
        dirtyRegion_.width_ = surfaceWidth_;
        dirtyRegion_.height_ = surfaceHeight_;
    } else if (bufferAge_ > 1) {
        dirtyRegion_ = MergeHistory(bufferAge_, dirtyRegion_);
    }
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

RectI RSDirtyRegionManager::GetAllHistoryMerge()
{
    PushHistory(dirtyRegion_);
    return MergeHistory(bufferAge_, dirtyRegion_);
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
        case DirtyRegionDebugType::DISABLED:
        default:
            break;
    }
}

RectI RSDirtyRegionManager::MergeHistory(int age, RectI rect) const
{
    int size = static_cast<int>(historySize_);
    if (age > size) {
        rect.left_ = 0;
        rect.top_ = 0;
        rect.width_ = surfaceWidth_;
        rect.height_ = surfaceHeight_;
    } else {
        for (int i = size - 1; i > size - age; --i) {
            rect = rect.JoinRect(GetHistory(i));
        }
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

RectI RSDirtyRegionManager::GetHistory(unsigned i) const
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
