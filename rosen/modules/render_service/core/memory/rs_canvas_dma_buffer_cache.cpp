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

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
#include "rs_canvas_dma_buffer_cache.h"

#include "pipeline/rs_canvas_drawing_render_node.h"
#include "platform/common/rs_log.h"

namespace OHOS::Rosen {
namespace {
constexpr int CACHED_SURFACE_BUFFER_LIMIT_PER_NODE = 3;
constexpr int CACHED_SURFACE_BUFFER_LIMIT_PER_PID = 512;
}

RSCanvasDmaBufferCache& RSCanvasDmaBufferCache::GetInstance()
{
    static RSCanvasDmaBufferCache instance;
    return instance;
}

void RSCanvasDmaBufferCache::RegisterCanvasCallback(
    pid_t remotePid, const sptr<RSICanvasSurfaceBufferCallback>& callback)
{
    std::lock_guard<std::mutex> lock(canvasCallbackMutex_);
    if (callback != nullptr) {
        canvasSurfaceBufferCallbackMap_[remotePid] = callback;
    } else {
        canvasSurfaceBufferCallbackMap_.erase(remotePid);
    }
    RS_LOGD("RSCanvasDmaBufferCache::RegisterCanvasCallback pid=%{public}d, callback=%{public}s",
        remotePid, callback ? "registered" : "unregistered");
}

void RSCanvasDmaBufferCache::NotifyCanvasSurfaceBufferChanged(
    NodeId nodeId, const sptr<SurfaceBuffer>& buffer, uint32_t resetSurfaceIndex) const
{
    auto pid = ExtractPid(nodeId);
    sptr<RSICanvasSurfaceBufferCallback> callback = nullptr;
    {
        std::lock_guard<std::mutex> lock(canvasCallbackMutex_);
        auto it = canvasSurfaceBufferCallbackMap_.find(pid);
        if (it == canvasSurfaceBufferCallbackMap_.end()) {
            RS_LOGE(
                "RSCanvasDmaBufferCache::NotifyCanvasSurfaceBufferChanged callback not found for pid=%{public}d", pid);
            return;
        }
        callback = it->second;
    }
    if (callback != nullptr) {
        callback->OnCanvasSurfaceBufferChanged(nodeId, buffer, resetSurfaceIndex);
    }
}

bool RSCanvasDmaBufferCache::AddPendingBuffer(
    NodeId nodeId, const sptr<SurfaceBuffer>& buffer, uint32_t resetSurfaceIndex)
{
    if (buffer == nullptr && resetSurfaceIndex == 0) {
        ClearPendingBufferByNodeId(nodeId);
        return true;
    }

    std::lock_guard<std::mutex> lock(pendingBufferMutex_);
    pid_t pid = ExtractPid(nodeId);
    auto pidIt = pendingBufferMap_.find(pid);
    if (pidIt == pendingBufferMap_.end()) {
        pendingBufferMap_[pid] = {};
        pidIt = pendingBufferMap_.find(pid);
    }
    auto& nodeMap = pidIt->second;
    size_t pidBufferCount = 0;
    for (const auto& [id, cache] : nodeMap) {
        pidBufferCount += cache.second.size();
    }
    if (pidBufferCount >= CACHED_SURFACE_BUFFER_LIMIT_PER_PID) {
        RS_LOGW("RSCanvasDmaBufferCache::AddPendingBuffer pid=%{public}d buffer count %{public}zu exceeds limit",
            pid, pidBufferCount);
        return false;
    }
    auto it = nodeMap.find(nodeId);
    // Cache first buffer for the node
    if (it == nodeMap.end()) {
        nodeMap[nodeId] = { std::make_pair(0, BufferMap { { resetSurfaceIndex, buffer } }) };
        return true;
    }

    auto& cache = it->second; // std::pair<currentResetSurfaceIndex, BufferMap>
    auto currentResetSurfaceIndex = cache.first;
    auto& nodeBufferMap = cache.second; // resetSurfaceIndex -> SurfaceBuffer
    bool willAdd = false;
    if (resetSurfaceIndex > currentResetSurfaceIndex) {
        willAdd = nodeBufferMap.find(resetSurfaceIndex) == nodeBufferMap.end();
    } else {
        RS_LOGW("RSCanvasDmaBufferCache::AddPendingBuffer nodeId=%{public}" PRIu64 " ignored (old resetSurfaceIndex)",
            nodeId);
    }
    if (willAdd) {
        nodeBufferMap.emplace(resetSurfaceIndex, buffer);
    }
    RemovePendingBuffer(nodeBufferMap, currentResetSurfaceIndex, nodeId);
    return willAdd;
}

sptr<SurfaceBuffer> RSCanvasDmaBufferCache::AcquirePendingBuffer(NodeId nodeId, uint32_t resetSurfaceIndex)
{
    std::lock_guard<std::mutex> lock(pendingBufferMutex_);
    pid_t pid = ExtractPid(nodeId);
    auto pidIt = pendingBufferMap_.find(pid);
    if (pidIt == pendingBufferMap_.end()) {
        pendingBufferMap_[pid] = {};
        pidIt = pendingBufferMap_.find(pid);
    }
    auto& nodeMap = pidIt->second;
    auto it = nodeMap.find(nodeId);
    // Node has no cached buffers, add placeholder
    if (it == nodeMap.end()) {
        nodeMap[nodeId] = { std::make_pair(resetSurfaceIndex, BufferMap {}) };
        RS_LOGW("RSCanvasDmaBufferCache::AcquirePendingBuffer, no cache, nodeId=%{public}" PRIu64, nodeId);
        return nullptr;
    }

    auto& cache = it->second; // std::pair<currentResetSurfaceIndex, BufferMap>
    auto& nodeBufferMap = cache.second;
    cache.first = resetSurfaceIndex; // Update current resetSurfaceIndex, it will used in function AddPendingBuffer
    auto bufferIt = nodeBufferMap.find(resetSurfaceIndex);
    sptr<SurfaceBuffer> buffer = nullptr;
    if (bufferIt != nodeBufferMap.end()) {
        buffer = bufferIt->second; // Hit the cache
    } else {
        RS_LOGW("RSCanvasDmaBufferCache::AcquirePendingBuffer, failed to hit cache, nodeId=%{public}" PRIu64, nodeId);
    }

    RemovePendingBuffer(nodeBufferMap, resetSurfaceIndex, nodeId);
    return buffer;
}

void RSCanvasDmaBufferCache::RemovePendingBuffer(BufferMap& nodeBufferMap, uint32_t resetSurfaceIndex, NodeId nodeId)
{
    auto bufferIt = nodeBufferMap.begin();
    while (bufferIt != nodeBufferMap.end()) {
        if (bufferIt->first <= resetSurfaceIndex) {
            bufferIt = nodeBufferMap.erase(bufferIt);
        } else if (nodeBufferMap.size() > CACHED_SURFACE_BUFFER_LIMIT_PER_NODE) {
            RS_LOGW("RSCanvasDmaBufferCache::RemovePendingBuffer, Cached surfaceBuffer out of limit, "
                    "nodeId=%{public}" PRIu64, nodeId);
            bufferIt = nodeBufferMap.erase(bufferIt);
        } else {
            break;
        }
    }
}

void RSCanvasDmaBufferCache::RemovePendingBuffer(NodeId nodeId, uint32_t resetSurfaceIndex)
{
    std::lock_guard<std::mutex> lock(pendingBufferMutex_);
    pid_t pid = ExtractPid(nodeId);
    auto pidIt = pendingBufferMap_.find(pid);
    if (pidIt == pendingBufferMap_.end()) {
        return;
    }
    auto it = pidIt->second.find(nodeId);
    if (it == pidIt->second.end()) {
        return;
    }
    RemovePendingBuffer(it->second.second, resetSurfaceIndex, nodeId);
}

void RSCanvasDmaBufferCache::ClearPendingBufferByNodeId(NodeId nodeId)
{
    std::lock_guard<std::mutex> lock(pendingBufferMutex_);
    pid_t pid = ExtractPid(nodeId);
    auto pidIt = pendingBufferMap_.find(pid);
    if (pidIt == pendingBufferMap_.end()) {
        return;
    }
    auto it = pidIt->second.find(nodeId);
    if (it != pidIt->second.end()) {
        it->second.second.clear();
        pidIt->second.erase(it);
    }
    if (pidIt->second.empty()) {
        pendingBufferMap_.erase(pidIt);
    }
}

void RSCanvasDmaBufferCache::ClearPendingBufferByPid(pid_t pid)
{
    std::lock_guard<std::mutex> lock(pendingBufferMutex_);
    auto pidIt = pendingBufferMap_.find(pid);
    if (pidIt != pendingBufferMap_.end()) {
        for (auto& [nodeId, cache] : pidIt->second) {
            cache.second.clear();
        }
        pendingBufferMap_.erase(pidIt);
    }
}
} // namespace OHOS::Rosen
#endif // ROSEN_OHOS && RS_ENABLE_VK