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

#include "feature/tunnel_layer/rs_tunnel_layer_manager.h"

#include <algorithm>
#include <cinttypes>
#include <surface.h>

#include "common/rs_tunnel_layer_utils.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_render_node_map.h"
#include "pipeline/rs_surface_handler.h"
#include "pipeline/rs_surface_render_node.h"
#include "feature/tunnel_layer/rs_tunnel_runtime_state.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "feature/tunnel_layer/rs_tunnel_layer_helper.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
namespace {

bool HandleAvailableCallback(const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode,
    NodeId nodeId, RSTunnelRuntimeState::TunnelState tunnelState, uint64_t tunnelLayerGeneration)
{
    if (surfaceNode == nullptr || tunnelState != RSTunnelRuntimeState::TunnelState::BUILDING) {
        return false;
    }
    auto& tunnelRuntime = RSTunnelRuntimeStore::GetOrCreate(surfaceNode->GetId());
    if (tunnelRuntime.SetActiveFromTunnelLayerAvailable(tunnelLayerGeneration)) {
        RS_LOGD("%{public}s%{public}s BUILDING -> ACTIVE, reason:AVAILABLE callback, "
            "nodeId:%{public}" PRIu64 ", generation:%{public}" PRIu64,
            TUNNEL_DEBUG_PREFIX, __func__, nodeId, tunnelLayerGeneration);
        RS_TRACE_NAME_FMT("TUNNEL_DEBUG %s BUILDING -> ACTIVE, reason=AVAILABLE callback, "
            "nodeId=%" PRIu64 ", generation=%" PRIu64, __func__, nodeId, tunnelLayerGeneration);
        return true;
    }
    RS_LOGD("%{public}s%{public}s ignore AVAILABLE, nodeId:%{public}" PRIu64
        ", generation:%{public}" PRIu64,
        TUNNEL_DEBUG_PREFIX, __func__, nodeId, tunnelLayerGeneration);
    RS_TRACE_NAME_FMT("TUNNEL_DEBUG %s ignore AVAILABLE, nodeId=%" PRIu64 ", generation=%" PRIu64,
        __func__, nodeId, tunnelLayerGeneration);
    return false;
}
} // namespace

RSTunnelLayerManager::RSTunnelLayerManager(std::shared_ptr<RSContext> context)
    : context_(context)
{}

void RSTunnelLayerManager::TransferTunnelPendingBufferToNormalConsume(
    const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) const
{
    if (surfaceNode == nullptr) {
        return;
    }

    auto surfaceHandler = surfaceNode->GetMutableRSSurfaceHandler();
    if (surfaceHandler == nullptr) {
        return;
    }
    if (!Rosen::IsNewTunnelEnabled()) {
        return;
    }
    auto consumer = surfaceHandler->GetConsumer();
    if (consumer == nullptr) {
        return;
    }
    auto& tunnelRuntime = RSTunnelRuntimeStore::GetOrCreate(surfaceNode->GetId());

    if (surfaceHandler->GetHoldBuffer() != nullptr || surfaceHandler->GetHoldReturnValue() != nullptr) {
        if (tunnelRuntime.HasPendingBuffer()) {
            surfaceHandler->SetAvailableBufferCount(std::max(surfaceHandler->GetAvailableBufferCount(), 1));
        }
        return;
    }

    RSSurfaceHandler::SurfaceBufferEntry pendingBuffer;
    if (!tunnelRuntime.TakePendingBuffer(pendingBuffer)) {
        return;
    }

    surfaceHandler->SetHoldBuffer(std::make_shared<RSSurfaceHandler::SurfaceBufferEntry>(pendingBuffer));
    consumer->SetBufferHold(true);
    int32_t availableCount = static_cast<int32_t>(consumer->GetAvailableBufferCount());
    surfaceHandler->SetAvailableBufferCount(availableCount > 0 ? availableCount : 1);
    RS_TRACE_NAME_FMT("TUNNEL_DEBUG %s transfer pending to RS normal, bufferId=%" PRIu64,
        __func__, pendingBuffer.buffer->GetBufferId());
    RS_LOGD("%{public}s%{public}s transfer pending buffer to RS normal, bufferId:%{public}" PRIu64,
        TUNNEL_DEBUG_PREFIX, __func__, pendingBuffer.buffer->GetBufferId());
}

void RSTunnelLayerManager::MarkTunnelBufferConsumedForNormal(
    const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode,
    const std::shared_ptr<RSComposerClientManager>& clientManager) const
{
    if (surfaceNode == nullptr) {
        return;
    }
    auto& tunnelRuntime = RSTunnelRuntimeStore::GetOrCreate(surfaceNode->GetId());
    if (tunnelRuntime.GetTunnelState() != RSTunnelRuntimeState::TunnelState::ACTIVE) {
        return;
    }
    auto surfaceHandler = surfaceNode->GetMutableRSSurfaceHandler();
    if (surfaceHandler == nullptr) {
        return;
    }
    auto buffer = surfaceHandler->GetBuffer();
    auto bufferOwnerCount = surfaceHandler->GetBufferOwnerCount();
    auto consumer = surfaceHandler->GetConsumer();
    if (buffer == nullptr || bufferOwnerCount == nullptr || consumer == nullptr) {
        return;
    }
    if (!tunnelRuntime.IsCommittedTunnelBuffer()) {
        return;
    }
    if (clientManager) {
        clientManager->MarkTunnelSurfaceInvalid(surfaceNode->GetScreenId(), consumer->GetUniqueId());
    }
    RSUniRenderThread::Instance().ReplacePendingReleaseBufferFence(
        consumer, buffer, SyncFence::InvalidFence(), bufferOwnerCount);
    surfaceHandler->SetCurrentFrameBufferConsumed();
    tunnelRuntime.ClearCommittedTunnelBuffer();
}

void RSTunnelLayerManager::ClearRuntimeStateByPid(pid_t remotePid)
{
    auto context = GetContext();
    if (context == nullptr) {
        return;
    }
    context->GetMutableNodeMap().TraverseSurfaceNodes([this, remotePid](const auto& surfaceNode) {
        if (surfaceNode != nullptr && ExtractPid(surfaceNode->GetId()) == remotePid) {
            RSTunnelRuntimeStore::Clear(surfaceNode->GetId());
            lastNotifiedLayerStates_.erase(surfaceNode->GetId());
        }
    });
}

std::shared_ptr<RSContext> RSTunnelLayerManager::GetContext() const
{
    return context_.lock();
}

std::shared_ptr<RSSurfaceRenderNode> RSTunnelLayerManager::GetSurfaceNode(NodeId nodeId) const
{
    auto context = GetContext();
    if (context == nullptr) {
        return nullptr;
    }
    return context->GetNodeMap().GetRenderNode<RSSurfaceRenderNode>(nodeId);
}

void RSTunnelLayerManager::UpdateTunnelLayerState(
    NodeId nodeId, const std::shared_ptr<RSSurfaceHandler>& surfaceHandler)
{
    if (HandleLppTunnelLayerId(surfaceHandler, nodeId)) {
        return;
    }

    if (!Rosen::IsNewTunnelEnabled() && !surfaceHandler->IsCurrentFrameBufferConsumed()) {
        return;
    }
    uint64_t cachedTunnelLayerId = 0;
    uint32_t cachedProperty = TUNNEL_PROP_INVALID;
    if (!RSTunnelRuntimeStore::GetLayerInfoIfPresent(nodeId, cachedTunnelLayerId, cachedProperty)
        || cachedTunnelLayerId == 0) {
        return;
    }
    HandleNewTunnelLayerId(surfaceHandler, nodeId);
}

bool RSTunnelLayerManager::HandleLppTunnelLayerId(
    const std::shared_ptr<RSSurfaceHandler>& surfaceHandler, NodeId nodeId) const
{
    if (surfaceHandler->GetSourceType() !=
        static_cast<uint32_t>(OHSurfaceSource::OH_SURFACE_SOURCE_LOWPOWERVIDEO)) {
        return false;
    }

    auto consumer = surfaceHandler->GetConsumer();
    if (consumer == nullptr) {
        RS_LOGD("%{public}s%{public}s consumer is null", TUNNEL_DEBUG_PREFIX, __func__);
        return true;
    }
    auto& tunnelRuntime = RSTunnelRuntimeStore::GetOrCreate(nodeId);
    uint64_t currentTunnelLayerId = 0;
    uint32_t currentProperty = TUNNEL_PROP_INVALID;
    tunnelRuntime.GetLayerInfo(currentTunnelLayerId, currentProperty);
    uint64_t newTunnelLayerId = consumer->GetUniqueId();
    if (currentTunnelLayerId == newTunnelLayerId) {
        return true;
    }
    tunnelRuntime.SetLayerInfo(
        newTunnelLayerId, TUNNEL_PROP_BUFFER_ADDR | TUNNEL_PROP_DEVICE_COMMIT);
    RS_LOGD("%{public}s%{public}s lpp surfaceid:%{public}" PRIu64,
        TUNNEL_DEBUG_PREFIX, __func__, newTunnelLayerId);
    RS_TRACE_NAME_FMT("TUNNEL_DEBUG %s lpp surfaceid=%" PRIu64, __func__, newTunnelLayerId);
    return true;
}

void RSTunnelLayerManager::ResetTunnelLayerState(const std::shared_ptr<RSSurfaceHandler>& surfaceHandler,
    const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode)
{
    uint64_t tunnelLayerId = 0;
    uint32_t tunnelLayerProperty = TUNNEL_PROP_INVALID;
    RSTunnelRuntimeStore::GetLayerInfoOrDefault(surfaceNode->GetId(), tunnelLayerId, tunnelLayerProperty);
    if (tunnelLayerId != 0) {
        auto tunnelLayerGeneration = RSTunnelRuntimeStore::GetTunnelLayerGeneration(surfaceNode->GetId());
        ProcessLayerStateChanged(surfaceHandler, surfaceNode->GetId(), LayerStateChange::UNAVAILABLE,
            tunnelLayerGeneration, surfaceHandler->GetConsumer());
        return;
    }
    RSTunnelLayerHelper::ResetTunnelState(surfaceNode);
}

void RSTunnelLayerManager::HandleNewTunnelLayerId(
    const std::shared_ptr<RSSurfaceHandler>& surfaceHandler, NodeId nodeId)
{
    auto consumer = surfaceHandler->GetConsumer();
    auto& tunnelRuntime = RSTunnelRuntimeStore::GetOrCreate(nodeId);
    uint64_t currentTunnelLayerId = 0;
    uint32_t currentProperty = TUNNEL_PROP_INVALID;
    tunnelRuntime.GetLayerInfo(currentTunnelLayerId, currentProperty);
    bool hasTunnelState = currentTunnelLayerId != 0;

    uint64_t newTunnelLayerId = 0;
    uint32_t newProperty = TUNNEL_PROP_INVALID;
    bool resolved = RSTunnelLayerHelper::ResolveTunnelLayerInfo(
        consumer, newTunnelLayerId, newProperty, nodeId);
    if (!resolved || !IsNewTunnelLayerValid(newTunnelLayerId, newProperty)) {
        if (!hasTunnelState) {
            return;
        }
        auto tunnelState = tunnelRuntime.GetTunnelState();
        RS_OPTIONAL_TRACE_NAME_FMT("TUNNEL_DEBUG %s invalid tunnel info, nodeId=%" PRIu64 ", resolved=%d, "
            "current=%" PRIu64 ", new=%" PRIu64 ", property=%u, state=%s", __func__, nodeId,
            resolved, currentTunnelLayerId, newTunnelLayerId, newProperty, ToTunnelStateName(tunnelState));
        auto tunnelLayerGeneration = tunnelRuntime.GetTunnelLayerGeneration();
        ProcessLayerStateChanged(
            surfaceHandler, nodeId, LayerStateChange::UNAVAILABLE, tunnelLayerGeneration, consumer);
        return;
    }

    auto tunnelState = tunnelRuntime.GetTunnelState();
    bool sameTunnelInfo = currentTunnelLayerId == newTunnelLayerId && currentProperty == newProperty;
    if (sameTunnelInfo && tunnelState == RSTunnelRuntimeState::TunnelState::BUILDING) {
        // Keep runtime buffers while waiting for the layer-created callback.
        RS_TRACE_NAME_FMT("TUNNEL_DEBUG %s wait tunnel activation, nodeId=%" PRIu64 ", tunnelLayerId=%" PRIu64
            ", property=%u, state=%s", __func__, nodeId, newTunnelLayerId, newProperty,
            ToTunnelStateName(tunnelState));
        RS_LOGD("%{public}s%{public}s wait tunnel activation, nodeId:%{public}" PRIu64
            ", tunnelLayerId:%{public}" PRIu64 ", property:%{public}u, state:%{public}s",
            TUNNEL_DEBUG_PREFIX, __func__, nodeId, newTunnelLayerId, newProperty,
            ToTunnelStateName(tunnelState));
        return;
    }
    if (sameTunnelInfo && tunnelState == RSTunnelRuntimeState::TunnelState::ACTIVE) {
        return;
    }
    RSTunnelLayerHelper::BeginTunnelBuilding(nodeId, newTunnelLayerId, newProperty);
    RS_LOGD("%{public}s%{public}s tunnel surfaceid:%{public}" PRIu64 ", property:%{public}u",
        TUNNEL_DEBUG_PREFIX, __func__, newTunnelLayerId, newProperty);
    RS_TRACE_NAME_FMT("TUNNEL_DEBUG %s tunnel surfaceid=%" PRIu64 ", property=%u",
        __func__, newTunnelLayerId, newProperty);
}

void RSTunnelLayerManager::HandleLayerStateChanged(
    NodeId nodeId, LayerStateChange state, uint64_t tunnelLayerGeneration)
{
    auto surfaceNode = GetSurfaceNode(nodeId);
    auto surfaceHandler = surfaceNode == nullptr ? nullptr : surfaceNode->GetMutableRSSurfaceHandler();
    ProcessLayerStateChanged(surfaceHandler, nodeId, state, tunnelLayerGeneration, nullptr);
}

void RSTunnelLayerManager::ProcessLayerStateChanged(const std::shared_ptr<RSSurfaceHandler>& surfaceHandler,
    NodeId nodeId, LayerStateChange state, uint64_t tunnelLayerGeneration, const sptr<IConsumerSurface>& consumer)
{
    auto callbackConsumer = consumer;
    if (callbackConsumer == nullptr && surfaceHandler != nullptr) {
        callbackConsumer = surfaceHandler->GetConsumer();
    }
    auto surfaceNode = GetSurfaceNode(nodeId);
    uint64_t trackedTunnelLayerId = 0;
    uint32_t trackedProperty = TUNNEL_PROP_INVALID;
    auto& tunnelRuntime = RSTunnelRuntimeStore::GetOrCreate(nodeId);
    if (surfaceNode != nullptr) {
        tunnelRuntime.GetLayerInfo(trackedTunnelLayerId, trackedProperty);
    }
    bool isTunnelStateTracked = surfaceNode != nullptr && trackedTunnelLayerId != 0;
    bool hasVersionedCallback = tunnelLayerGeneration != 0;
    bool isCurrentGeneration = surfaceNode != nullptr &&
        tunnelRuntime.IsCurrentTunnelLayerGeneration(tunnelLayerGeneration);
    if (hasVersionedCallback && !isCurrentGeneration) {
        RS_LOGD("%{public}s%{public}s ignore stale callback, nodeId:%{public}" PRIu64
            ", input:%{public}s, generation:%{public}" PRIu64,
            TUNNEL_DEBUG_PREFIX, __func__, nodeId, ToLayerStateChangeName(state), tunnelLayerGeneration);
        RS_TRACE_NAME_FMT("TUNNEL_DEBUG %s ignore stale callback, nodeId=%" PRIu64
            ", input=%s, generation=%" PRIu64,
            __func__, nodeId, ToLayerStateChangeName(state), tunnelLayerGeneration);
        return;
    }
    LayerStateChange callbackState = isTunnelStateTracked && state != LayerStateChange::AVAILABLE ?
        LayerStateChange::UNAVAILABLE : state;
    auto tunnelState = surfaceNode == nullptr ? RSTunnelRuntimeState::TunnelState::BUILDING :
        tunnelRuntime.GetTunnelState();
    RS_TRACE_NAME_FMT("TUNNEL_DEBUG %s nodeId=%" PRIu64
        ", input=%s, callback=%s, tracked=%d, activation=%s, generation=%" PRIu64,
        __func__, nodeId, ToLayerStateChangeName(state), ToLayerStateChangeName(callbackState),
        isTunnelStateTracked, ToTunnelStateName(tunnelState), tunnelLayerGeneration);
    RS_LOGD("%{public}s%{public}s nodeId:%{public}" PRIu64
        ", input:%{public}s, callback:%{public}s, tracked:%{public}d, activation:%{public}s"
        ", generation:%{public}" PRIu64,
        TUNNEL_DEBUG_PREFIX, __func__, nodeId, ToLayerStateChangeName(state),
        ToLayerStateChangeName(callbackState), isTunnelStateTracked, ToTunnelStateName(tunnelState),
        tunnelLayerGeneration);

    bool shouldDispatch = callbackState != LayerStateChange::AVAILABLE ||
        (isTunnelStateTracked &&
            HandleAvailableCallback(surfaceNode, nodeId, tunnelState, tunnelLayerGeneration));
    if (callbackState == LayerStateChange::UNAVAILABLE) {
        RSTunnelLayerHelper::ResetTunnelState(surfaceNode);
        RS_TRACE_NAME_FMT("TUNNEL_DEBUG %s tunnel reset, nodeId=%" PRIu64, __func__, nodeId);
        RS_LOGD("%{public}s%{public}s tunnel reset, nodeId:%{public}" PRIu64,
            TUNNEL_DEBUG_PREFIX, __func__, nodeId);
    }
    if (callbackConsumer != nullptr && shouldDispatch) {
        UpdateLayerStateLog(nodeId, callbackState, state, isTunnelStateTracked);
    }
    if (shouldDispatch) {
        DispatchLayerStateChanged(nodeId, callbackState, callbackConsumer);
    }
}

void RSTunnelLayerManager::UpdateLayerStateLog(NodeId nodeId, LayerStateChange callbackState,
    LayerStateChange inputState, bool isTunnelStateTracked)
{
    auto iter = lastNotifiedLayerStates_.find(nodeId);
    bool stateChanged = iter == lastNotifiedLayerStates_.end() || iter->second != callbackState;
    if (!stateChanged) {
        return;
    }
    const char* previousState = iter == lastNotifiedLayerStates_.end() ? "NONE" :
        ToLayerStateChangeName(iter->second);
    RS_TRACE_NAME_FMT("TUNNEL_DEBUG %s nodeId:%" PRIu64 ", state:%s -> %s, input:%s, tracked:%d",
        __func__, nodeId, previousState, ToLayerStateChangeName(callbackState),
        ToLayerStateChangeName(inputState), isTunnelStateTracked);
    RS_LOGD("%{public}s%{public}s nodeId:%{public}" PRIu64
        ", state:%{public}s -> %{public}s, input:%{public}s, tracked:%{public}d",
        TUNNEL_DEBUG_PREFIX, __func__, nodeId, previousState, ToLayerStateChangeName(callbackState),
        ToLayerStateChangeName(inputState), isTunnelStateTracked);
    lastNotifiedLayerStates_[nodeId] = callbackState;
}

void RSTunnelLayerManager::DispatchLayerStateChanged(
    NodeId nodeId, LayerStateChange state, const sptr<IConsumerSurface>& callbackConsumer) const
{
    if (callbackConsumer == nullptr) {
        return;
    }

    GSError ret = callbackConsumer->NotifyLayerStateChanged(state);
    if (ret != GSERROR_OK) {
        RS_LOGE("%{public}s%{public}s NotifyLayerStateChanged failed, nodeId:%{public}" PRIu64
            ", state:%{public}u, ret:%{public}d",
            TUNNEL_DEBUG_PREFIX, __func__, nodeId, static_cast<uint32_t>(state), ret);
    }
}

const char* RSTunnelLayerManager::ToLayerStateChangeName(LayerStateChange state)
{
    switch (state) {
        case LayerStateChange::AVAILABLE:
            return "AVAILABLE";
        case LayerStateChange::UNAVAILABLE:
            return "UNAVAILABLE";
        default:
            return "UNKNOWN";
    }
}
} // namespace Rosen
} // namespace OHOS
