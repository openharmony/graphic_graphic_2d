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
    SHADOW_RECT,
    PREPARE_CLIP_RECT,
    REMOVE_CHILD_RECT,
    RENDER_PROPERTIES_RECT,
    CANVAS_NODE_SKIP_RECT,
    OUTLINE_RECT,
    TYPE_AMOUNT
};

class RSB_EXPORT RSDirtyRegionManager final {
    friend class RSFilterCacheManager;
public:
    static constexpr int32_t ALIGNED_BITS = 32;
    RSDirtyRegionManager();
    RSDirtyRegionManager(bool isDisplayDirtyManager);
    ~RSDirtyRegionManager() = default;
    // update/expand current frame dirtyregion
    void MergeDirtyRect(const RectI& rect);
    // update/expand current frame dirtyregion if intersect
    bool MergeDirtyRectIfIntersect(const RectI& rect);
    // update/expand dirtyregion after merge history
    void MergeDirtyRectAfterMergeHistory(const RectI& rect);
    // clip dirtyregion in current frame
    void IntersectDirtyRect(const RectI& rect);
    // Clip currentframe dirtyRegion intersected with surfaceRect
    void ClipDirtyRectWithinSurface();
    // clear allinfo except dirtyregion history
    void Clear();
    
    // update current frame's visited dirtyregion
    void UpdateVisitedDirtyRects(const std::vector<RectI>& rects);
    RectI GetIntersectedVisitedDirtyRect(const RectI& absRect) const;
    void UpdateCacheableFilterRect(const RectI& rect);
    bool IfCacheableFilterRectFullyCover(const RectI& targetRect);
    bool IsCacheableFilterRectEmpty() const
    {
        return cacheableFilterRects_.empty();
    }

    void InvalidateFilterCacheRect()
    {
        isFilterCacheRectValid_ = false;
    }

    bool IsFilterCacheRectValid()
    {
        return isFilterCacheRectValid_;
    }

    // return current frame dirtyregion, can be changed in prepare and process (displaynode) stage
    const RectI& GetCurrentFrameDirtyRegion();
    // return merged historical region
    const RectI& GetDirtyRegion() const;
    // return mapAbs dirtyRegion
    const RectI& GetCurrentFrameMpsAbsDirtyRect() const;
    void SetCurrentFrameDirtyRect(const RectI& dirtyRect);
    /*  return merged historical region upside down in left-bottom origin coordinate
        reason: when use OpenGL SetDamageRegion, coordinate system conversion exists.
    */
    RectI GetDirtyRegionFlipWithinSurface() const;
    // return current frame's region from dirtyregion history
    const RectI& GetLatestDirtyRegion() const;
    // return merged historical region upside down in left-bottom origin coordinate
    RectI GetRectFlipWithinSurface(const RectI& rect) const;
    // get aligned rect as times of alignedBits
    static RectI GetPixelAlignedRect(const RectI& rect, int32_t alignedBits = ALIGNED_BITS);
    // return true if current frame dirtyregion is not empty
    bool IsCurrentFrameDirty() const;
    // return true if dirtyregion after merge history is not empty
    bool IsDirty() const;
    // push currentframe dirtyregion into history, and merge history according to bufferage
    void UpdateDirty(bool enableAligned = false);
    // align current frame dirtyregion before merge history
    void UpdateDirtyByAligned(int32_t alignedBits = ALIGNED_BITS);
    bool SetBufferAge(const int age);
    bool SetSurfaceSize(const int32_t width, const int32_t height);
    RectI GetSurfaceRect() const
    {
        return surfaceRect_;
    }
    void MergeSurfaceRect();
    // Reset current frame dirtyregion to surfacerect to realize full refreshing
    void ResetDirtyAsSurfaceSize();

    void UpdateDebugRegionTypeEnable(DirtyRegionDebugType dirtyDebugType);

    inline bool IsDebugRegionTypeEnable(DebugRegionType var) const
    {
        if (var < DebugRegionType::TYPE_MAX) {
            return debugRegionEnabled_[var];
        }
        return false;
    }
    // OnSync must be excuted after UpdateDirty API
    void OnSync();

    // added for dirty region dfx
    void UpdateDirtyRegionInfoForDfx(NodeId id, RSRenderNodeType nodeType = RSRenderNodeType::CANVAS_NODE,
        DirtyRegionType dirtyType = DirtyRegionType::UPDATE_DIRTY_REGION, const RectI& rect = RectI());
    void GetDirtyRegionInfo(std::map<NodeId, RectI>& target,
        RSRenderNodeType nodeType = RSRenderNodeType::CANVAS_NODE,
        DirtyRegionType dirtyType = DirtyRegionType::UPDATE_DIRTY_REGION) const;

    void MarkAsTargetForDfx()
    {
        isDfxTarget_ = true;
    }

    bool IsTargetForDfx() {
        return isDfxTarget_;
    }

    bool HasOffset();
    void SetOffset(int offsetX, int offsetY);
    RectI GetOffsetedDirtyRegion() const;

    const std::vector<RectI>& GetMergedDirtyRegions() const
    {
        return mergedDirtyRegions_;
    }

private:
    RectI MergeHistory(unsigned int age, RectI rect) const;
    void PushHistory(RectI rect);
    // get his rect according to index offset
    RectI GetHistory(unsigned int i) const;
    void AlignHistory();

    RectI surfaceRect_;             // dirtyregion clipbounds
    RectI dirtyRegion_;             // dirtyregion after merge history
    RectI currentFrameDirtyRegion_; // dirtyRegion in current frame
    RectI syncCurrentFrameDirtyRegion_; // sync currentFrameDirtyRegion_ for replay.
    std::vector<RectI> visitedDirtyRegions_ = {};  // visited app's dirtyRegion
    std::vector<RectI> cacheableFilterRects_ = {};  // node's region if filter cachable
    std::vector<RectI> mergedDirtyRegions_ = {};

    // added for dfx
    std::vector<std::map<NodeId, RectI>> dirtyCanvasNodeInfo_;
    std::vector<std::map<NodeId, RectI>> dirtySurfaceNodeInfo_;
    std::vector<bool> debugRegionEnabled_;
    bool isDfxTarget_ = false;
    std::vector<RectI> dirtyHistory_;
    int historyHead_ = -1;
    unsigned int historySize_ = 0;
    const unsigned HISTORY_QUEUE_MAX_SIZE = 5;
    // may add new set function for bufferAge
    unsigned int bufferAge_ = HISTORY_QUEUE_MAX_SIZE;
    bool isDirtyRegionAlignedEnable_ = false;
    bool isFilterCacheRectValid_ = true;
    bool isDisplayDirtyManager_ = false;
    std::atomic<bool> isSync_ = false;

    // Used for coordinate switch, i.e. dirtyRegion = dirtyRegion + offset.
    // For example when dirtymanager is used in cachesurface when surfacenode's
    // shadow and surfacenode are cached in a surface, dirty region's coordinate should start
    // from shadow's left-top rather than that of displaynode.
    // Normally, this value should be set to:
    //      offsetX_ =  - surfacePos.x + shadowWidth
    //      offsetY_ =  - surfacePos.y + shadowHeight
    bool hasOffset_ = false;
    int offsetX_ = 0;
    int offsetY_ = 0;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_DIRTY_REGION_MANAGER_H
