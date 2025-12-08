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

#ifndef RENDER_SERVICE_CORE_MEMORY_RS_CANVAS_DMA_BUFFER_CACHE_H
#define RENDER_SERVICE_CORE_MEMORY_RS_CANVAS_DMA_BUFFER_CACHE_H

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
#include "surface_buffer.h"

#include "ipc_callbacks/rs_icanvas_surface_buffer_callback.h"

namespace OHOS {
namespace Rosen {
class RSCanvasDmaBufferCache : public std::enable_shared_from_this<RSCanvasDmaBufferCache> {
public:
    RSCanvasDmaBufferCache() = default;
    ~RSCanvasDmaBufferCache() = default;
    RSCanvasDmaBufferCache(const RSCanvasDmaBufferCache&) = delete;
    RSCanvasDmaBufferCache(const RSCanvasDmaBufferCache&&) = delete;
    RSCanvasDmaBufferCache& operator=(const RSCanvasDmaBufferCache&) = delete;
    RSCanvasDmaBufferCache& operator=(const RSCanvasDmaBufferCache&&) = delete;

    static RSCanvasDmaBufferCache& GetInstance();

    void RegisterCanvasCallback(pid_t remotePid, const sptr<RSICanvasSurfaceBufferCallback>& callback);

    void NotifyCanvasSurfaceBufferChanged(
        NodeId nodeId, const sptr<SurfaceBuffer>& buffer, uint32_t resetSurfaceIndex) const;

    bool AddPendingBuffer(NodeId nodeId, const sptr<SurfaceBuffer>& buffer, uint32_t resetSurfaceIndex);

    sptr<SurfaceBuffer> AcquirePendingBuffer(NodeId nodeId, uint32_t resetSurfaceIndex);

    void RemovePendingBuffer(NodeId nodeId, uint32_t resetSurfaceIndex);

    void ClearPendingBufferByNodeId(NodeId nodeId);

    void ClearPendingBufferByPid(pid_t pid);

private:
    std::unordered_map<pid_t, sptr<RSICanvasSurfaceBufferCallback>> canvasSurfaceBufferCallbackMap_;
    mutable std::mutex canvasCallbackMutex_; // Protects canvasSurfaceBufferCallbackMap_

    using BufferMap = std::map<uint32_t, sptr<SurfaceBuffer>>;
    // Canvas pre-allocated buffer map: nodeId -> pair<currentResetSurfaceIndex, map(resetSurfaceIndex -> buffer)>
    std::map<NodeId, std::pair<uint32_t, BufferMap>> pendingBufferMap_;
    std::mutex pendingBufferMutex_; // Protects pendingBufferMap_
};
} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_OHOS && RS_ENABLE_VK
#endif // RENDER_SERVICE_CORE_MEMORY_RS_CANVAS_DMA_BUFFER_CACHE_H
