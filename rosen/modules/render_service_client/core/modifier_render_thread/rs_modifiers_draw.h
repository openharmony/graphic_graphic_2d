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

#include "command/rs_canvas_node_command.h"
#include "command/rs_node_command.h"
#include "common/rs_optional_trace.h"
#include "platform/ohos/backend/native_buffer_utils.h"
#include "pipeline/rs_draw_cmd.h"
#include "recording/cmd_list_helper.h"
#include "recording/draw_cmd_list.h"
#include "recording/draw_cmd.h"
#include "surface_buffer.h"
#include "transaction/rs_irender_client.h"
#include "transaction/rs_transaction_data.h"

constexpr uint32_t DEFAULT_MODIFIERS_DRAW_THREAD_LOOP_NUM = 3;
constexpr uint32_t HYBRID_MAX_PIXELMAP_WIDTH = 8192;  // max width value from PhysicalDeviceProperties
constexpr uint32_t HYBRID_MAX_PIXELMAP_HEIGHT = 8192;  // max height value from PhysicalDeviceProperties
constexpr uint32_t HYBRID_MAX_ENABLE_OP_CNT = 11;  // max value for enable hybrid op
constexpr uint32_t HYBRID_MAX_TEXT_ENABLE_OP_CNT = 1;  // max value for enable text hybrid op
constexpr int64_t FFRT_WAIT_TIMEOUT = 30; // ms
namespace OHOS {
namespace Rosen {
struct DrawOpInfo {
    bool isRenderWithForegroundColor = false;
    NodeId nodeId = INVALID_NODEID;
    VkSemaphore semaphore = VK_NULL_HANDLE;
    std::shared_ptr<Drawing::DrawCmdList> cmdList = nullptr;
    std::shared_ptr<Media::PixelMap> pixelMap = nullptr;
};

class RSModifiersDraw {
public:
    static void ConvertCmdListForCanvas(const std::shared_ptr<Drawing::DrawCmdList>& cmdList, NodeId nodeId);

    static void ConvertCmdList(DrawOpInfo& targetCmd);

    static void RemoveSurfaceByNodeId(NodeId nodeId, bool postTask = false);

    static bool ResetSurfaceByNodeId(
        int32_t width, int32_t height, NodeId nodeId, bool needResetOpItems = false, bool postTask = false);

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

    static void GetFenceAndAddDrawOp(std::vector<DrawOpInfo>& targetCmds);

    static bool SeperateHybridRenderCmdList(std::unique_ptr<RSTransactionData>& transactionData,
        std::vector<DrawOpInfo>& targetCmds, uint32_t& enableTextHybridOpCnt);

    static void TraverseDrawOpInfo(std::vector<DrawOpInfo>& targetCmds, std::atomic<size_t>& cmdIndex);

    static void ConvertTransactionForCanvas(std::unique_ptr<RSTransactionData>& transactionData);

    static void ConvertTransactionWithFFRT(std::unique_ptr<RSTransactionData>& transactionData,
        std::shared_ptr<RSIRenderClient>& renderServiceClient, bool& isNeedCommit, std::vector<DrawOpInfo>& targetCmds);

    static void ConvertTransactionWithoutFFRT(
        std::unique_ptr<RSTransactionData>& transactionData, std::vector<DrawOpInfo>& targetCmds);

    static bool IsTargetCommand(
        Drawing::DrawCmdList::HybridRenderType hybridRenderType, uint16_t type, uint16_t subType, bool cmdListEmpty);
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
        std::vector<std::shared_ptr<Drawing::DrawOpItem>> preDrawOpItems;
        uint32_t saveLayerCnt = 0;
    };

    static sptr<SurfaceBuffer> DmaMemAlloc(
        int32_t width, int32_t height, const std::unique_ptr<Media::PixelMap>& pixelMap);

    static sptr<SurfaceBuffer> CreateSurfaceBuffer(
        const std::unique_ptr<Media::PixelMap>& pixelMap, int32_t width, int32_t height);

    static std::shared_ptr<Drawing::Surface> CreateSurfaceFromGpuContext(sptr<SurfaceBuffer> surfaceBufferTmp);

    static std::shared_ptr<Drawing::Surface> CreateSurfaceFromCpuContext(
        const std::unique_ptr<Media::PixelMap>& pixelMap);

    static void ClearCanvasDrawingNodeMemory();

    static void ClearDrawingContextMemory();

    static std::shared_ptr<Drawing::Surface> CreateSurface(std::unique_ptr<Media::PixelMap>& pixelMap,
        int32_t width, int32_t height, sptr<SurfaceBuffer> surfaceBufferTmp);

    static bool Playback(const std::shared_ptr<Drawing::Surface>& surface,
        const std::shared_ptr<Drawing::DrawCmdList>& cmdList, bool isCanvasType, VkSemaphore& semaphore);

    static void FlushSurfaceWithSemaphore(const std::shared_ptr<Drawing::Surface>& surface,
        VkSemaphore& semaphore);

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

    static void ConvertDrawOpItems(const std::vector<std::shared_ptr<Drawing::DrawOpItem>>& drawOpItems,
        std::vector<std::shared_ptr<Drawing::DrawOpItem>>& dstOpItems, uint32_t& saveLayerCnt);

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

    static std::mutex semaphoreInfoMutex_;
    static std::vector<DestroySemaphoreInfo*> semaphoreInfoVec_;

    static std::unordered_map<NodeId, std::shared_ptr<RectF>> drawRegions_;

    static std::unordered_set<uint32_t> typesToSave_;

    // attention : ffrtMutex_ only use for cv_
    static std::mutex ffrtMutex_;
    static std::condition_variable cv_;
    static std::atomic<uint32_t> ffrtTaskNum_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_MODIFIERS_DRAW_H
