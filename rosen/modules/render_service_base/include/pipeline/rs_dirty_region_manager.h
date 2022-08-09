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
#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_DIRTY_REGION_MANAGER_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_DIRTY_REGION_MANAGER_H

#include <map>
#include <vector>

#include "common/rs_rect.h"
#include "platform/common/rs_system_properties.h"

namespace OHOS {
namespace Rosen {
enum DebugRegionType {
    CURRENT_SUB,
    CURRENT_WHOLE,
    MULTI_HISTORY,
    TYPE_MAX
};

class RSDirtyRegionManager final {
public:
    RSDirtyRegionManager();
    ~RSDirtyRegionManager() = default;
    void MergeDirtyRect(const RectI& rect);
    void IntersectDirtyRect(const RectI& rect);
    void IntersectDirtyRectWithSurfaceRect();
    void Clear();
    // return merged historical region
    const RectI& GetDirtyRegion() const;
    // return merged historical region upsize down in surface
    RectI GetDirtyRegionFlipWithinSurface() const;
    // return current frame's region
    const RectI& GetLatestDirtyRegion() const;
    bool IsDirty() const;
    void UpdateDirty();
    void UpdateDirtyCanvasNodes(NodeId id, const RectI& rect);
    void UpdateDirtySurfaceNodes(NodeId id, const RectI& rect);
    void GetDirtyCanvasNodes(std::map<NodeId, RectI>& target) const;
    void GetDirtySurfaceNodes(std::map<NodeId, RectI>& target) const;
    bool SetBufferAge(const int age);
    bool SetSurfaceSize(const int width, const int height);

    void UpdateDebugRegionTypeEnable();
    
    inline bool IsDebugRegionTypeEnable(DebugRegionType var) const
    {
        if (var < DebugRegionType::TYPE_MAX) {
            return debugRegionEnabled_[var];
        }
        return false;
    }
    
    inline bool IsDebugEnabled() const
    {
        return RSSystemProperties::GetDirtyRegionDebugType() != DirtyRegionDebugType::DISABLED;
    }

private:
    RectI MergeHistory(unsigned int age, RectI rect) const;
    void PushHistory(RectI rect);
    RectI GetHistory(unsigned int i) const;

    RectI surfaceRect_;
    RectI dirtyRegion_;
    std::map<NodeId, RectI> dirtyCanvasNodes_;
    std::map<NodeId, RectI> dirtySurfaceNodes_;
    std::vector<bool> debugRegionEnabled_;
    std::vector<RectI> dirtyHistory_;
    int historyHead_ = -1;
    unsigned int historySize_ = 0;
    const unsigned HISTORY_QUEUE_MAX_SIZE = 4;
    // may add new set function for bufferAge
    unsigned int bufferAge_ = HISTORY_QUEUE_MAX_SIZE;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_DIRTY_REGION_MANAGER_H