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

void RSContext::AddPendingBuffer(NodeId nodeId, const sptr<SurfaceBuffer>& buffer, uint32_t resetSurfaceIndex)
{
    auto node = GetNodeMap().GetRenderNode<RSCanvasDrawingRenderNode>(nodeId);
    if (node != nullptr && resetSurfaceIndex < node->GetResetSurfaceIndex()) {
        RS_LOGW("RSContext::AddPendingBuffer nodeId=%{public}" PRIu64 " ignored (old resetSurfaceIndex)", nodeId);
        return;
    }

    std::lock_guard<std::mutex> lock(pendingBufferMutex_);
    auto it = pendingBufferMap_.find(nodeId);
    if (it == pendingBufferMap_.end()) {
        pendingBufferMap_[nodeId] = { { resetSurfaceIndex, buffer } };
        return;
    }
    auto& nodeBufferMap = it->second; // resetSurfaceIndex -> SurfaceBuffer
    auto bufferIt = nodeBufferMap.find(resetSurfaceIndex);
    if (bufferIt == nodeBufferMap.end()) {
        nodeBufferMap.emplace(resetSurfaceIndex, buffer);
        if (nodeBufferMap.size() > CACHED_SURFACE_BUFFER_LIMIT_PER_NODE) {
            RS_LOGW("RSContext::AddPendingBuffer, Cached surfaceBuffer out of limit, nodeId=%{public}" PRIu64, nodeId);
            nodeBufferMap.erase(nodeBufferMap.begin());
        }
    }
}

sptr<SurfaceBuffer> RSContext::GetPendingBuffer(NodeId nodeId, uint32_t resetSurfaceIndex, bool placeholder)
{
    std::lock_guard<std::mutex> lock(pendingBufferMutex_);
    auto it = pendingBufferMap_.find(nodeId);
    if (it == pendingBufferMap_.end()) {
        if (placeholder) {
            pendingBufferMap_[nodeId] = { { resetSurfaceIndex, nullptr } };
        }
        RS_LOGW("RSContext::GetPendingBuffer, no cache, nodeId=%{public}" PRIu64, nodeId);
        return nullptr;
    }
    auto& nodeBufferMap = it->second;
    auto bufferIt = nodeBufferMap.find(resetSurfaceIndex);
    if (bufferIt == nodeBufferMap.end()) {
        if (placeholder) {
            nodeBufferMap.emplace(resetSurfaceIndex, nullptr);
        }
        RS_LOGW("RSContext::GetPendingBuffer, failed to hit cache, nodeId=%{public}" PRIu64, nodeId);
        return nullptr;
    }
    return bufferIt->second;
}

void RSContext::CleanupUnconsumedPendingBuffers()
{
    std::lock_guard<std::mutex> lock(pendingBufferMutex_);
    auto it = pendingBufferMap_.end();
    while (it != pendingBufferMap_.begin()) {
        it--;
        auto nodeId = it->first;
        auto node = GetNodeMap().GetRenderNode<RSCanvasDrawingRenderNode>(nodeId);
        if (node == nullptr) {
            it = pendingBufferMap_.erase(it);
            continue;
        }
        auto& nodeBufferMap = it->second;
        auto bufferIt = nodeBufferMap.end();
        while (bufferIt != nodeBufferMap.begin()) {
            bufferIt--;
            if (bufferIt->first < node->GetResetSurfaceIndex()) {
                bufferIt = nodeBufferMap.erase(bufferIt);
            }
        }
        if (it->second.empty()) {
            it = pendingBufferMap_.erase(it);
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
