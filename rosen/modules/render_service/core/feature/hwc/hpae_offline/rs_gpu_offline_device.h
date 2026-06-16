/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef RS_CORE_FEATURE_HWC_GPU_OFFLINE_DEVICE_H
#define RS_CORE_FEATURE_HWC_GPU_OFFLINE_DEVICE_H

#include <cmath>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <mutex>

#include "drawable/rs_surface_render_node_drawable.h"
#include "drawable/rs_screen_render_node_drawable.h"
#include "feature/hwc/hpae_offline/rs_gpu_offline_buffer.h"
#include "feature/hwc/hpae_offline/rs_gpu_offline_thread.h"
#include "feature/hwc/hpae_offline/rs_offline_device.h"
#include "feature/hwc/hpae_offline/rs_hpae_offline_process_syncer.h"
#include "feature/hwc/hpae_offline/rs_hpae_offline_thread_manager.h"
#include "feature/hwc/hpae_offline/rs_offline_result.h"

namespace OHOS {
namespace Rosen {

struct GPUOfflineDrawParams {
    float sdrNit = 0.0f;
    float displayNit = 0.0f;
    float brightnessRatio = 1.0f;
    std::vector<float> layerLinearMatrix = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
    bool useBilinearInterpolation = false;
    bool ignoreAlpha = false;
    bool colorFollow = false;
    ScreenId screenId = INVALID_SCREEN_ID;
    bool hdrPresent = false;
    uint8_t gAlpha = 0;
    GraphicTransformType transformType = GraphicTransformType::GRAPHIC_ROTATE_NONE;
    GraphicColorGamut targetColorGamut = GraphicColorGamut::GRAPHIC_COLOR_GAMUT_SRGB;
    int32_t resultWidth = 0;
    int32_t resultHeight = 0;

    bool operator==(const GPUOfflineDrawParams& other) const
    {
        if (std::fabs(sdrNit - other.sdrNit) > 1e-5f ||
            std::fabs(displayNit - other.displayNit) > 1e-5f ||
            std::fabs(brightnessRatio - other.brightnessRatio) > 1e-5f ||
            useBilinearInterpolation != other.useBilinearInterpolation ||
            ignoreAlpha != other.ignoreAlpha ||
            colorFollow != other.colorFollow ||
            screenId != other.screenId ||
            targetColorGamut != other.targetColorGamut ||
            hdrPresent != other.hdrPresent ||
            gAlpha != other.gAlpha ||
            transformType != other.transformType ||
            resultWidth != other.resultWidth ||
            resultHeight != other.resultHeight) {
            return false;
        }
        if (layerLinearMatrix.size() != other.layerLinearMatrix.size()) {
            return false;
        }
        constexpr float EPSILON = 1e-5f;
        for (size_t i = 0; i < layerLinearMatrix.size(); ++i) {
            if (std::fabs(layerLinearMatrix[i] - other.layerLinearMatrix[i]) > EPSILON) {
                return false;
            }
        }
        return true;
    }
};

struct GPUOfflineSubThreadData {
    NodeId nodeId = 0; // only used in GPUOfflineThread (copy)
    std::shared_ptr<RSGPUOfflineBuffer> offlineBuffer = nullptr; // only used in GPUOfflineThread (copy)
    GPUOfflineDrawParams drawParams; // only used in GPUOfflineThread (copy)
};

struct GPUOfflineContext {
    NodeId nodeId = 0; // shared:copy to GPUOfflineThread
    std::shared_ptr<RSGPUOfflineBuffer> offlineBuffer = nullptr; // shared:copy to GPUOfflineThread
    GPUOfflineDrawParams drawParams; // shared:copy to GPUOfflineThread
    std::atomic<bool> skipDraw = false;
    std::atomic<bool> hasDrawn = false;
    std::atomic<size_t> invalidFrames = 0;
    std::atomic<bool> timeout = false;
    int64_t timestamp = 0; // mainthread only (for LRU)
};

class RSGPUOfflineDevice : public RSOfflineDevice, public std::enable_shared_from_this<RSGPUOfflineDevice> {
public:
    explicit RSGPUOfflineDevice();
    ~RSGPUOfflineDevice() override;
    OfflineDeviceType GetDeviceType() const override { return OfflineDeviceType::GPU_OFFLINE_DEVICE; }
    static bool CheckCondition(const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode);
    bool IsRSOfflineDeviceReady(std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) override;
    bool PostProcessOfflineTask(std::shared_ptr<DrawableV2::RSSurfaceRenderNodeDrawable>& surfaceDrawable,
        offlineTaskId taskId) override;
    bool PostProcessOfflineTask(std::shared_ptr<RSSurfaceRenderNode>& node,
        offlineTaskId taskId) override;
    bool WaitForProcessOfflineResult(offlineTaskId taskId, std::chrono::milliseconds timeout,
        ProcessOfflineResult& result) override;
    void CheckAndPostClearOfflineResourceTask(const std::vector<uint64_t>& offlineNodeIds) override;
    bool CanDeleteDevice() override;

private:
    static constexpr size_t MAX_CACHE_SIZE = 4;

    void CheckAndPostPreAllocBuffersTask();
    void CheckAndHandleTimeoutEvent(std::shared_ptr<ProcessOfflineFuture>& futurePtr, NodeId nodeId);
    void OfflineTaskFuncWithData(RSSurfaceRenderParams* surfaceParams,
        std::shared_ptr<ProcessOfflineFuture>& futurePtr, const GPUOfflineSubThreadData& taskContext);
    bool DoProcessOfflineWithContext(RSSurfaceRenderParams& surfaceParams,
        ProcessOfflineResult& result, const GPUOfflineSubThreadData& taskContext);
    bool SetResultWhenSkipDraw(std::shared_ptr<GPUOfflineContext>& offlineContext,
        RSSurfaceRenderParams* surfaceParams, offlineTaskId taskId);
    bool FillOfflineResult(std::shared_ptr<RSGPUOfflineBuffer>& offlineBuffer,
        RSSurfaceRenderParams* surfaceParams, ProcessOfflineResult& result);
    bool PostOfflineTaskCommon(std::shared_ptr<GPUOfflineContext>& offlineContext,
        RSSurfaceRenderParams* surfaceParams, offlineTaskId taskId);
    bool DrawHDRImage(RSSurfaceRenderParams& surfaceParams, const GPUOfflineSubThreadData& taskContext);
    BufferDrawParam CreateBufferDrawParam(const GPUOfflineSubThreadData& taskContext);
    std::shared_ptr<GPUOfflineContext> GetOrCreateOfflineContext(NodeId nodeId);
    std::shared_ptr<GPUOfflineContext> GetOfflineContext(NodeId nodeId);
    NodeId FindOldestEntry();
    void TryRemoveContext(NodeId nodeId);
    bool UpdateContext(std::shared_ptr<RSSurfaceRenderNode>& surfaceNode,
        std::shared_ptr<GPUOfflineContext>& offlineContext);
    BufferRequestConfig GetGPUBufferConfig(const GPUOfflineSubThreadData& taskContext);
    GPUOfflineDrawParams CollectDrawParams(std::shared_ptr<RSSurfaceRenderNode> surfaceNode,
        const std::shared_ptr<RSSurfaceHandler> surfaceHandler, RSSurfaceRenderParams& surfaceParams);
    void ClearContextCache(NodeId nodeId);

    RSHpaeOfflineProcessSyncer offlineResultSync_;
    RSGPUOfflineThread offlineThread_;
    std::map<NodeId, std::shared_ptr<GPUOfflineContext>> offlineContextCache_;
    mutable std::mutex cacheMutex_;
    uint64_t currentVsyncId_ = 0;
    size_t currentFrameNodeCount_ = 0;
};

} // namespace Rosen
} // namespace OHOS

#endif // RS_CORE_FEATURE_HWC_GPU_OFFLINE_DEVICE_H