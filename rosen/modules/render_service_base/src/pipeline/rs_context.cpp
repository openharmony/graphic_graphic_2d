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

#include "pipeline/rs_context.h"

#include "pipeline/rs_render_node.h"
#include "platform/common/rs_log.h"
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
#include "surface_buffer.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#endif

namespace OHOS::Rosen {
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
namespace {
constexpr int CACHED_SURFACE_BUFFER_LIMIT_PER_NODE = 3;
}
#endif

void RSContext::RegisterAnimatingRenderNode(const std::shared_ptr<RSRenderNode>& nodePtr)
{
    NodeId id = nodePtr->GetId();
    animatingNodeList_.emplace(id, nodePtr);
    nodePtr->ActivateDisplaySync();
    ROSEN_LOGD("RSContext::RegisterAnimatingRenderNode, register node id: %{public}" PRIu64, id);
}

void RSContext::UnregisterAnimatingRenderNode(NodeId id)
{
    animatingNodeList_.erase(id);
    ROSEN_LOGD("RSContext::UnregisterAnimatingRenderNode, unregister node id: %{public}" PRIu64, id);
}

void RSContext::AddActiveNode(const std::shared_ptr<RSRenderNode>& node)
{
    if (node == nullptr || node->GetId() == INVALID_NODEID) {
        return;
    }
    auto rootNodeId = node->GetInstanceRootNodeId();
    std::lock_guard<std::mutex> lock(activeNodesInRootMutex_);
    activeNodesInRoot_[rootNodeId].emplace(node->GetId(), node);
}

// replication method takes long
std::unordered_map<NodeId, std::unordered_map<NodeId, std::weak_ptr<RSRenderNode>>> RSContext::GetActiveNodes()
{
    std::lock_guard<std::mutex> lock(activeNodesInRootMutex_);
    return activeNodesInRoot_;
}

bool RSContext::HasActiveNode(const std::shared_ptr<RSRenderNode>& node)
{
    if (node == nullptr || node->GetId() == INVALID_NODEID) {
        return false;
    }
    auto rootNodeId = node->GetInstanceRootNodeId();
    std::lock_guard<std::mutex> lock(activeNodesInRootMutex_);
    return activeNodesInRoot_[rootNodeId].count(node->GetId()) > 0;
}

int32_t RSContext::SetBrightnessInfoChangeCallback(pid_t remotePid,
    const sptr<RSIBrightnessInfoChangeCallback>& callback)
{
    if (callback == nullptr) {
        brightnessInfoChangeCallbackMap_.erase(remotePid);
        return SUCCESS;
    }
    brightnessInfoChangeCallbackMap_[remotePid] = callback;
    return SUCCESS;
}

void RSContext::NotifyBrightnessInfoChangeCallback(ScreenId screenId, const BrightnessInfo& brightnessInfo) const
{
    for (auto& [_, callback] : brightnessInfoChangeCallbackMap_) {
        if (callback == nullptr) {
            continue;
        }
        callback->OnBrightnessInfoChange(screenId, brightnessInfo);
    }
}

bool RSContext::IsBrightnessInfoChangeCallbackMapEmpty() const
{
    return brightnessInfoChangeCallbackMap_.empty();
}

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
void RSContext::RegisterCanvasCallback(pid_t remotePid, const sptr<RSICanvasSurfaceBufferCallback>& callback)
{
    std::lock_guard<std::mutex> lock(canvasCallbackMutex_);
    if (callback != nullptr) {
        canvasSurfaceBufferCallbackMap_[remotePid] = callback;
    } else {
        canvasSurfaceBufferCallbackMap_.erase(remotePid);
    }
    RS_LOGD("RSContext::RegisterCanvasCallback pid=%{public}d, callback=%{public}s",
        remotePid, callback ? "registered" : "unregistered");
}

void RSContext::NotifyCanvasSurfaceBufferChanged(
    NodeId nodeId, const sptr<SurfaceBuffer>& buffer, uint32_t resetSurfaceIndex) const
{
    auto pid = ExtractPid(nodeId);
    sptr<RSICanvasSurfaceBufferCallback> callback = nullptr;
    {
        std::lock_guard<std::mutex> lock(canvasCallbackMutex_);
        auto it = canvasSurfaceBufferCallbackMap_.find(pid);
        if (it == canvasSurfaceBufferCallbackMap_.end()) {
            RS_LOGE("RSContext::NotifyCanvasSurfaceBufferChanged callback not found for pid=%{public}d", pid);
            return;
        }
        callback = it->second;
    }
    if (callback != nullptr) {
        callback->OnCanvasSurfaceBufferChanged(nodeId, buffer, resetSurfaceIndex);
    }
}

bool RSContext::AddPendingBuffer(NodeId nodeId, const sptr<SurfaceBuffer>& buffer, uint32_t resetSurfaceIndex)
{
    std::lock_guard<std::mutex> lock(pendingBufferMutex_);
    auto it = pendingBufferMap_.find(nodeId);
    // Cache first buffer for the node
    if (it == pendingBufferMap_.end()) {
        pendingBufferMap_[nodeId] = { std::make_pair(0, BufferMap { { resetSurfaceIndex, buffer } }) };
        return true;
    }

    auto& cache = it->second; // std::pair<currentResetSurfaceIndex, BufferMap>
    auto currentResetSurfaceIndex = cache.first;
    if (resetSurfaceIndex <= currentResetSurfaceIndex) {
        RS_LOGW("RSContext::AddPendingBuffer nodeId=%{public}" PRIu64 " ignored (old resetSurfaceIndex)", nodeId);
        return false;
    }

    auto& nodeBufferMap = cache.second; // resetSurfaceIndex -> SurfaceBuffer
    auto bufferIt = nodeBufferMap.find(resetSurfaceIndex);
    if (bufferIt == nodeBufferMap.end()) {
        nodeBufferMap.emplace(resetSurfaceIndex, buffer);
        if (nodeBufferMap.size() > CACHED_SURFACE_BUFFER_LIMIT_PER_NODE) {
            RS_LOGW("RSContext::AddPendingBuffer, Cached surfaceBuffer out of limit, nodeId=%{public}" PRIu64, nodeId);
            nodeBufferMap.erase(nodeBufferMap.begin());
        }
        return true;
    }
    return false;
}

sptr<SurfaceBuffer> RSContext::AcquirePendingBuffer(NodeId nodeId, uint32_t resetSurfaceIndex)
{
    std::lock_guard<std::mutex> lock(pendingBufferMutex_);
    auto it = pendingBufferMap_.find(nodeId);
    // Node has no cached buffers, add placeholder
    if (it == pendingBufferMap_.end()) {
        pendingBufferMap_[nodeId] = { std::make_pair(resetSurfaceIndex, BufferMap {}) };
        RS_LOGW("RSContext::AcquirePendingBuffer, no cache, nodeId=%{public}" PRIu64, nodeId);
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
        RS_LOGW("RSContext::AcquirePendingBuffer, failed to hit cache, nodeId=%{public}" PRIu64, nodeId);
    }

    bufferIt = nodeBufferMap.begin();
    while (bufferIt != nodeBufferMap.end()) {
        if (bufferIt->first <= resetSurfaceIndex) {
            bufferIt = nodeBufferMap.erase(bufferIt);
            continue;
        } else {
            break;
        }
    }

    return buffer;
}

void RSContext::RemovePendingBuffer(NodeId nodeId, uint32_t resetSurfaceIndex)
{
    std::lock_guard<std::mutex> lock(pendingBufferMutex_);
    auto it = pendingBufferMap_.find(nodeId);
    if (it == pendingBufferMap_.end()) {
        return;
    }

    auto& nodeBufferMap = it->second.second;
    auto bufferIt = nodeBufferMap.find(resetSurfaceIndex);
    if (bufferIt != nodeBufferMap.end()) {
        nodeBufferMap.erase(bufferIt);
    }
}

void RSContext::ClearPendingBufferByNodeId(NodeId nodeId)
{
    std::lock_guard<std::mutex> lock(pendingBufferMutex_);
    auto it = pendingBufferMap_.find(nodeId);
    if (it != pendingBufferMap_.end()) {
        it->second.second.clear();
        pendingBufferMap_.erase(it);
    }
}

void RSContext::ClearPendingBufferByPid(pid_t pid)
{
    std::lock_guard<std::mutex> lock(pendingBufferMutex_);
    auto it = pendingBufferMap_.begin();
    while (it != pendingBufferMap_.end()) {
        if (ExtractPid(it->first) == pid) {
            it->second.second.clear();
            it = pendingBufferMap_.erase(it);
        } else {
            ++it;
        }
    }
}
#endif

void RSContext::AddPendingSyncNode(const std::shared_ptr<RSRenderNode> node)
{
    if (node == nullptr || node->GetId() == INVALID_NODEID) {
        return;
    }
    pendingSyncNodes_.emplace(node->GetId(), node);
}

void RSContext::MarkNeedPurge(ClearMemoryMoment moment, PurgeType purgeType)
{
    clearMoment_ = moment;
    purgeType_ = purgeType;
}

void RSContext::SetClearMoment(ClearMemoryMoment moment)
{
    clearMoment_ = moment;
}

ClearMemoryMoment RSContext::GetClearMoment() const
{
    return clearMoment_;
}

void RSContext::Initialize()
{
    nodeMap.Initialize(weak_from_this());
    globalRootRenderNode_->OnRegister(weak_from_this());
}

void RSContext::AddSyncFinishAnimationList(NodeId nodeId, AnimationId animationId, uint64_t token)
{
    needSyncFinishAnimationList_.push_back({nodeId, animationId, token});
}
} // namespace OHOS::Rosen
