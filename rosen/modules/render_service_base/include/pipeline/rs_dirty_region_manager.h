/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "common/rs_macros.h"
#include "common/rs_rect.h"
#include "platform/common/rs_system_properties.h"

namespace OHOS {
namespace Rosen {
// classify dfx debug options
enum DebugRegionType {
    CURRENT_SUB = 0,
    CURRENT_WHOLE,
    MULTI_HISTORY,
    EGL_DAMAGE,
    TYPE_MAX
};

// classify types that cause region dirty
enum DirtyRegionType {
    UPDATE_DIRTY_REGION = 0,
    OVERLAY_RECT,
    FILTER_RECT,
    REMOVE_CHILD,
    TYPE_AMOUNT
};

class RSB_EXPORT RSDirtyRegionManager final {
public:
    static constexpr int32_t ALIGNED_BITS = 32;
    RSDirtyRegionManager();
    ~RSDirtyRegionManager() = default;
    void MergeDirtyRect(const RectI& rect);
    void IntersectDirtyRect(const RectI& rect);
    // Clip dirtyRegion intersected with surfaceRect
    void ClipDirtyRectWithinSurface();
    void Clear();
    // return merged historical region
    const RectI& GetDirtyRegion() const;
    // return merged historical region upsize down in surface
    RectI GetDirtyRegionFlipWithinSurface() const;
    // return current frame's region
    const RectI& GetLatestDirtyRegion() const;
    // return merged historical region upsize down in surface
    RectI GetRectFlipWithinSurface(const RectI& rect) const;
    // Get aligned rect as times of alignedBits
    static RectI GetPixelAlignedRect(const RectI& rect, int32_t alignedBits = ALIGNED_BITS);
    bool IsDirty() const;
    void UpdateDirty(bool enableAligned = false);
    void UpdateDirtyByAligned(int32_t alignedBits = ALIGNED_BITS);
    bool SetBufferAge(const int age);
    bool SetSurfaceSize(const int32_t width, const int32_t height);
    RectI GetSurfaceRect() const
    {
        return surfaceRect_;
    }
    void ResetDirtyAsSurfaceSize();

    void UpdateDebugRegionTypeEnable(DirtyRegionDebugType dirtyDebugType);
    
    inline bool IsDebugRegionTypeEnable(DebugRegionType var) const
    {
        if (var < DebugRegionType::TYPE_MAX) {
            return debugRegionEnabled_[var];
        }
        return false;
    }

    // added for dirty region dfx
    void UpdateDirtyRegionInfoForDfx(NodeId id, RSRenderNodeType nodeType = RSRenderNodeType::CANVAS_NODE,
        DirtyRegionType dirtyType = DirtyRegionType::UPDATE_DIRTY_REGION, const RectI& rect = RectI());
    void GetDirtyRegionInfo(std::map<NodeId, RectI>& target,
        RSRenderNodeType nodeType = RSRenderNodeType::CANVAS_NODE,
        DirtyRegionType dirtyType = DirtyRegionType::UPDATE_DIRTY_REGION) const;

private:
    RectI MergeHistory(unsigned int age, RectI rect) const;
    void PushHistory(RectI rect);
    // get his rect according to index offset
    RectI GetHistory(unsigned int i) const;
    void AlignHistory();

    RectI surfaceRect_;
    RectI dirtyRegion_;
    // added for dfx
    std::vector<std::map<NodeId, RectI>> dirtyCanvasNodeInfo_;
    std::vector<std::map<NodeId, RectI>> dirtySurfaceNodeInfo_;
    std::vector<bool> debugRegionEnabled_;
    std::vector<RectI> dirtyHistory_;
    int historyHead_ = -1;
    unsigned int historySize_ = 0;
    const unsigned HISTORY_QUEUE_MAX_SIZE = 4;
    // may add new set function for bufferAge
    unsigned int bufferAge_ = HISTORY_QUEUE_MAX_SIZE;
    bool isDirtyRegionAlignedEnable_ = false;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_DIRTY_REGION_MANAGER_H
