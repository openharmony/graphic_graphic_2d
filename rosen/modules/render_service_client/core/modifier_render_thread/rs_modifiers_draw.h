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

#ifndef RS_MODIFIERS_DRAW_H
#define RS_MODIFIERS_DRAW_H

#include <unordered_map>
#include <unordered_set>

#include "common/rs_optional_trace.h"
#include "platform/ohos/backend/native_buffer_utils.h"
#include "pipeline/rs_draw_cmd.h"
#include "recording/cmd_list_helper.h"
#include "recording/draw_cmd_list.h"
#include "surface_buffer.h"

namespace OHOS {
namespace Rosen {
class RSModifiersDraw {
public:
    static void ConvertCmdListForCanvas(const std::shared_ptr<Drawing::DrawCmdList>& cmdList, NodeId nodeId);

    static void ConvertCmdList(const std::shared_ptr<Drawing::DrawCmdList>& cmdList, NodeId nodeId);

    static void RemoveSurfaceByNodeId(NodeId nodeId, bool postTask = false);

    static bool ResetSurfaceByNodeId(
        int32_t width, int32_t height, NodeId nodeId, bool needResetMatrix = false, bool postTask = false);

    static std::unique_ptr<Media::PixelMap> GetPixelMapByNodeId(NodeId nodeId, bool useDMA = false);

    static void CreateNextFrameSurface();

    static void ClearOffTreeNodeMemory(NodeId nodeId);

    static void InsertOffTreeNode(NodeId instanceId, NodeId nodeId);

    static void EraseOffTreeNode(NodeId instanceId, NodeId nodeId);

    static void MergeOffTreeNodeSet();

    static void AddDrawRegions(NodeId nodeId, std::shared_ptr<RectF> rect);

    static void EraseDrawRegions(NodeId nodeId);

    static bool CheckIfModifiersDrawThreadInited();

    static void InsertForegroundRoot(NodeId nodeId);

    static void EraseForegroundRoot(NodeId nodeId);

    static bool IsBackground();

    static void ClearBackGroundMemory();

    static void DestroySemaphore();

    static void PurgeContextResource();
private:
    struct SurfaceEntry {
        std::shared_ptr<Drawing::Surface> surface = nullptr;
        std::shared_ptr<Media::PixelMap> pixelMap = nullptr;
        std::shared_ptr<Drawing::Image> snapshot = nullptr;
        std::weak_ptr<Media::PixelMap> lastPixelMap;
        int lastWidth = 0;
        int lastHeight = 0;
        sptr<SurfaceBuffer> currentSurfaceBuffer = nullptr;
        sptr<SurfaceBuffer> preAllocSurfaceBuffer = nullptr;
        bool isNeedGenSnapshot = false;
        std::optional<Drawing::Matrix> matrix;
    };

    static sptr<SurfaceBuffer> DmaMemAlloc(
        int32_t width, int32_t height, const std::unique_ptr<Media::PixelMap>& pixelMap);

    static sptr<SurfaceBuffer> CreateSurfaceBuffer(
        const std::unique_ptr<Media::PixelMap>& pixelMap, int32_t width, int32_t height);

    static std::shared_ptr<Drawing::Surface> CreateSurfaceFromGpuContext(sptr<SurfaceBuffer> surfaceBufferTmp);

    static std::shared_ptr<Drawing::Surface> CreateSurfaceFromCpuContext(
        const std::unique_ptr<Media::PixelMap>& pixelMap);

    static std::shared_ptr<Drawing::Surface> CreateSurface(std::unique_ptr<Media::PixelMap>& pixelMap,
        int32_t width, int32_t height, sptr<SurfaceBuffer> surfaceBufferTmp);

    static bool Playback(const std::shared_ptr<Drawing::Surface>& surface,
        const std::shared_ptr<Drawing::DrawCmdList>& cmdList, bool isCanvasType, int32_t& fence);

    static void FlushSurfaceWithFence(const std::shared_ptr<Drawing::Surface>& surface,
        VkSemaphore& semaphore, int32_t& fence);

    static void DrawSnapshot(std::shared_ptr<Drawing::Canvas>& canvas, std::shared_ptr<Drawing::Image>& snapshot);

    static bool CheckAndDrawSnapshot(SurfaceEntry& surfaceEntry,
        const std::shared_ptr<Drawing::DrawCmdList>& cmdList, NodeId nodeId);

    static void AddPixelMapDrawOp(const std::shared_ptr<Drawing::DrawCmdList>& cmdList,
        const std::shared_ptr<Media::PixelMap>& pixelMap, int32_t width, int32_t height,
        bool isRenderWithForegroundColor, sptr<SyncFence> fence = SyncFence::INVALID_FENCE);

    static std::unique_ptr<Media::PixelMap> CreatePixelMap(int32_t width, int32_t height, bool useDMA = true);

    static SurfaceEntry GetSurfaceEntryByNodeId(NodeId nodeId);

    static bool CheckNodeIsOffTree(NodeId nodeId);

    static void UpdateSize(const std::shared_ptr<Drawing::DrawCmdList>& cmdList, int32_t& width, int32_t& height);

    static std::unordered_map<NodeId, SurfaceEntry> surfaceEntryMap_;

    static std::mutex surfaceEntryMutex_;

    static std::unordered_set<NodeId> dirtyNodes_;

    static std::mutex dirtyNodeMutex_;

    static std::unordered_map<NodeId, std::unordered_set<NodeId>> offTreeNodes_;

    static bool offTreeNodesChange_;

    static std::unordered_set<NodeId> allOffTreeNodes_;

    static std::mutex nodeStatusMutex_;

    static std::unordered_set<NodeId> foregroundRootSet_;

    static std::mutex foregroundRootSetMutex_;
    static bool needClearBackgroundMemory_;

    static std::mutex semaphoreInfoMutex_;
    static std::vector<DestroySemaphoreInfo*> semaphoreInfoVec_;

    static std::unordered_map<NodeId, std::shared_ptr<RectF>> drawRegions_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_MODIFIERS_DRAW_H
