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

#ifndef RENDER_SERVICE_CLIENT_CORE_MODIFIER_RENDER_THREAD_RS_CANVAS_MODIFIERS_DRAW_H
#define RENDER_SERVICE_CLIENT_CORE_MODIFIER_RENDER_THREAD_RS_CANVAS_MODIFIERS_DRAW_H

#include <unordered_map>

#include "surface_buffer.h"

#include "common/rs_common_def.h"
#include "platform/drawing/rs_surface.h"
#include "transaction/rs_buffer_transaction.h"
#include "transaction/rs_render_interface.h"

namespace OHOS {
namespace Media {
class PixelMap;
}
namespace Rosen {
struct DestroySemaphoreInfo;
class RSCanvasDrawingNode;
class RSCanvasModifiersDraw {
public:
    struct SurfaceEntry {
        int width = 0;
        int height = 0;
        bool forceFlushBuffer = false;
        bool needResetCanvas = false;
        int64_t lastFlushBufferTime = 0;
        std::shared_ptr<RSSurface> producerSurface = nullptr;
        std::shared_ptr<Drawing::Surface> drawingSurface = nullptr;
        std::unique_ptr<RSSurfaceFrame> surfaceFrame = nullptr;
        VkSemaphore semaphore = VK_NULL_HANDLE;
        std::unique_ptr<std::vector<Drawing::DrawCmdListPtr>> drawCmdListCache_ = nullptr;

        void Reset()
        {
            width = 0;
            height = 0;
            forceFlushBuffer = false;
            needResetCanvas = false;
            drawingSurface = nullptr;
            surfaceFrame = nullptr;
            semaphore = VK_NULL_HANDLE;
            if (drawCmdListCache_ != nullptr) {
                drawCmdListCache_->clear();
            }
        }
    };

    RSCanvasModifiersDraw() = default;
    ~RSCanvasModifiersDraw() = default;
    RSCanvasModifiersDraw(const RSCanvasModifiersDraw&) = delete;
    RSCanvasModifiersDraw(const RSCanvasModifiersDraw&&) = delete;
    RSCanvasModifiersDraw& operator=(const RSCanvasModifiersDraw&) = delete;
    RSCanvasModifiersDraw& operator=(const RSCanvasModifiersDraw&&) = delete;

    void SetCacheDir(const std::string& path);
    std::string GetCacheDir();

    void CleanCanvasDrawingNodeCache(NodeId nodeId, std::weak_ptr<RSRenderInterface> weakRenderInterface);

    void OnProducerSurfaceCreated(NodeId nodeId, std::shared_ptr<RSSurface> producerSurface);

    void CacheDrawCmdList(NodeId nodeId, Drawing::DrawCmdListPtr drawCmdList);

    void ResetSurface(
        int width, int height, NodeId nodeId, bool sizeOutOfGpuLimit, std::weak_ptr<RSCanvasDrawingNode> weakNode);

    bool GetPixelMap(std::shared_ptr<Media::PixelMap> pixelMap, const Drawing::Rect* rect,
        std::shared_ptr<RSCanvasDrawingNode> node, Drawing::DrawCmdListPtr drawCmdList);

    bool GetBitmap(Drawing::Bitmap& bitmap, std::shared_ptr<RSCanvasDrawingNode> node);

    void UpdateCanvasContent(NodeId nodeId, std::weak_ptr<RSCanvasDrawingNode> weakNode, bool forceFlushBuffer);

    void SubmitAndCollectCanvasBuffers();

    void SwapTransactionConfigList(std::vector<RSTransactionConfig>& transactionConfigList);

    void CleanFreeBuffers(int64_t maxDuration);

private:
    std::shared_ptr<Drawing::Image> GetImage(NodeId nodeId, const SurfaceEntry& surfaceEntry,
        const Drawing::BitmapFormat& bitmapFormat, std::shared_ptr<Drawing::GPUContext> gpuContext);

    void Playback(const std::shared_ptr<Drawing::Surface>& surface, const Drawing::DrawCmdListPtr& cmdList);

    void ConvertCmdListForCanvas(NodeId nodeId);

    void ConvertCmdListByBuffer(NodeId nodeId, SurfaceEntry& surfaceEntry);

    void RequestCanvasBuffer(NodeId nodeId);

    void FlushCanvasSurfaceWithSemaphore(const std::shared_ptr<Drawing::Surface>& surface, VkSemaphore& semaphore);

    void DestroyCanvasSemaphore();

    std::unique_ptr<RSSurfaceFrame> CheckAndDrawHistory(SurfaceEntry& surfaceEntry, NodeId nodeId);

    void AppendTransactionConfig(sptr<SurfaceBuffer> buffer, int fenceFd, NodeId nodeId);

    std::string cacheDir_;

    std::unordered_map<NodeId, SurfaceEntry> surfaceEntryMap_;

    std::unordered_map<NodeId, std::weak_ptr<RSCanvasDrawingNode>> canvasDrawingNodeMap_;

    std::vector<DestroySemaphoreInfo*> canvasNewSemaphoreInfos_;

    std::vector<DestroySemaphoreInfo*> canvasExpiredSemaphoreInfos_;

    std::vector<RSTransactionConfig> transactionConfigList_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_MODIFIER_RENDER_THREAD_RS_CANVAS_MODIFIERS_DRAW_H