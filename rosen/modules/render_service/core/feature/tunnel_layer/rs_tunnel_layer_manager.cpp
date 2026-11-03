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

#include "feature/tunnel_layer/rs_tunnel_layer_utils.h"
#include "platform/common/rs_log.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_render_node_map.h"
#include "pipeline/rs_surface_handler.h"
#include "pipeline/rs_surface_render_node.h"
#include "params/rs_surface_render_params.h"
#include "feature/tunnel_layer/rs_tunnel_route_arbiter.h"
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
    auto tunnelRuntimePtr = surfaceNode->TryGetTunnelRuntimeState();
    if (tunnelRuntimePtr == nullptr) {
        return false;
    }
    if (tunnelRuntimePtr->SetActiveFromTunnelLayerAvailable(tunnelLayerGeneration)) {
        return true;
    }
    return false;
}
} // namespace

RSTunnelLayerManager::RSTunnelLayerManager(std::shared_ptr<RSContext> context)
    : context_(context), arbiter_(std::make_unique<RSTunnelRouteArbiter>())
{}

RSTunnelLayerManager::~RSTunnelLayerManager() = default;

void RSTunnelLayerManager::AttachArbiterToRenderThread() const
{
    if (arbiter_ != nullptr) {
        arbiter_->AttachToRenderThread();
    }
}

void RSTunnelLayerManager::ClearRuntimeStateByPid(pid_t remotePid)
{
    auto context = GetContext();
    if (context == nullptr) {
        return;
    }
    context->GetMutableNodeMap().TraverseSurfaceNodes([this, remotePid](const auto& surfaceNode) {
        if (surfaceNode != nullptr && ExtractPid(surfaceNode->GetId()) == remotePid) {
            // Clear node-owned tunnel state; next access re-creates fresh.
            auto statePtr = surfaceNode->TryGetTunnelRuntimeState();
            if (statePtr) {
                statePtr->DisableTunnelLayer();
            }
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

void RSTunnelLayerManager::UpdateTunnelStateMachine(
    const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode, bool consumeSucceeded)
{
    auto surfaceHandler = surfaceNode == nullptr ? nullptr : surfaceNode->GetMutableRSSurfaceHandler();
    if (surfaceHandler == nullptr) {
        return;
    }
    // LPP layer-id sync needs a real consume (sourceType refreshes only then); others run per frame.
    if (consumeSucceeded && HandleLppTunnelLayerId(surfaceNode)) {
        SyncTunnelStateToStaging(surfaceNode);
        return;
    }
    // Kill-switch: both consume paths are independently guarded, so early return never strands buffers.
    if (!Rosen::IsNewTunnelEnabled()) {
        return;
    }
    if (!surfaceHandler->HasReceivedTunnelLayerInfo()) {
        return;
    }
    // TryGet (no create): non-tunnel surfaces keep tunnelRuntimeState_ null (no per-frame mutex/entry).
    auto tunnelRuntime = surfaceNode->TryGetTunnelRuntimeState();
    if (tunnelRuntime == nullptr) {
        return;
    }
    // Per-frame generation bump: frame version sent to the composer for stale-callback rejection.
    tunnelRuntime->IncrementTunnelLayerGeneration();
    // Run ArbitrateAndClaim for ACTIVE claim / GO_NORMAL bookkeeping; outcome is discarded.
    if (arbiter_ != nullptr) {
        (void)arbiter_->ArbitrateAndClaim(surfaceNode);
    }
    // Main consume path: roll ACTIVE back to NOT_TUNNEL keeping layerId/property for re-gating.
    if (tunnelRuntime->GetTunnelState() == RSTunnelRuntimeState::TunnelState::ACTIVE) {
        RSTunnelRouteArbiter::OnTunnelRollbackDetected();
        tunnelRuntime->ResetStateKeepLayerInfo();
        RS_TRACE_NAME_FMT("TUNNEL_DEBUG %s ACTIVE -> NOT_TUNNEL, reason:main consume path, "
            "nodeId=%" PRIu64, __func__, surfaceNode->GetId());
        RS_LOGI("%{public}s %{public}s ACTIVE -> NOT_TUNNEL, reason:main consume path, "
            "nodeId:%{public}" PRIu64, TUNNEL_DEBUG_PREFIX, __func__, surfaceNode->GetId());
    }
    if (!surfaceHandler->IsCurrentFrameBufferConsumed()) {
        SyncTunnelStateToStaging(surfaceNode);
        RS_OPTIONAL_TRACE_NAME_FMT("TUNNEL_DEBUG %s end buffer not consumed, nodeId=%" PRIu64 ", state=%d",
            __func__, surfaceNode->GetId(), static_cast<int32_t>(tunnelRuntime->GetTunnelState()));
        return;
    }
    uint64_t cachedTunnelLayerId = 0;
    uint32_t cachedProperty = TUNNEL_PROP_INVALID;
    tunnelRuntime->GetLayerInfo(cachedTunnelLayerId, cachedProperty);
    if (cachedTunnelLayerId == 0) {
        SyncTunnelStateToStaging(surfaceNode);
        RS_OPTIONAL_TRACE_NAME_FMT("TUNNEL_DEBUG %s end no layer info, nodeId=%" PRIu64 ", state=%d",
            __func__, surfaceNode->GetId(), static_cast<int32_t>(tunnelRuntime->GetTunnelState()));
        return;
    }
    HandleNewTunnelLayerId(surfaceNode);
    SyncTunnelStateToStaging(surfaceNode);
    RS_TRACE_NAME_FMT("TUNNEL_DEBUG %s end, nodeId=%" PRIu64 ", state=%d",
        __func__, surfaceNode->GetId(), static_cast<int32_t>(tunnelRuntime->GetTunnelState()));
}

void RSTunnelLayerManager::SyncTunnelStateToStaging(const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode)
{
    auto tunnelRuntimePtr = surfaceNode == nullptr ? nullptr : surfaceNode->TryGetTunnelRuntimeState();
    if (tunnelRuntimePtr == nullptr) {
        return;
    }
    auto* staging = static_cast<RSSurfaceRenderParams*>(surfaceNode->GetStagingRenderParams().get());
    if (staging == nullptr) {
        return;
    }
    staging->SetTunnelState(tunnelRuntimePtr->GetTunnelState());
    uint64_t tunnelLayerId = 0;
    uint32_t tunnelLayerProperty = TUNNEL_PROP_INVALID;
    tunnelRuntimePtr->GetLayerInfo(tunnelLayerId, tunnelLayerProperty);
    staging->SetTunnelLayerId(tunnelLayerId);
    staging->SetTunnelLayerProperty(tunnelLayerProperty);
    staging->SetTunnelLayerGeneration(tunnelRuntimePtr->GetTunnelLayerGeneration());
    staging->SetIsLppTunnel(tunnelRuntimePtr->IsLppTunnel());
}

bool RSTunnelLayerManager::HandleLppTunnelLayerId(
    const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode) const
{
    auto surfaceHandler = surfaceNode == nullptr ? nullptr : surfaceNode->GetMutableRSSurfaceHandler();
    if (surfaceHandler == nullptr) {
        return false;
    }
    if (surfaceHandler->GetSourceType() !=
        static_cast<uint32_t>(OHSurfaceSource::OH_SURFACE_SOURCE_LOWPOWERVIDEO)) {
        return false;
    }

    auto consumer = surfaceHandler->GetConsumer();
    if (consumer == nullptr) {
        return true;
    }
    // GetOrCreate + LPP flag: LPP bypasses the state machine, nothing else creates the state for LPP.
    auto tunnelRuntime = surfaceNode->GetTunnelRuntimeState();
    tunnelRuntime->SetLppTunnel(true);
    tunnelRuntime->SetScreenId(surfaceNode->GetScreenId());
    uint64_t currentTunnelLayerId = 0;
    uint32_t currentProperty = TUNNEL_PROP_INVALID;
    tunnelRuntime->GetLayerInfo(currentTunnelLayerId, currentProperty);
    uint64_t newTunnelLayerId = consumer->GetUniqueId();
    if (currentTunnelLayerId == newTunnelLayerId) {
        return true;
    }
    tunnelRuntime->SetLayerInfo(
        newTunnelLayerId, TUNNEL_PROP_BUFFER_ADDR | TUNNEL_PROP_DEVICE_COMMIT);
    return true;
}

void RSTunnelLayerManager::HandleNewTunnelLayerId(
    const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode)
{
    auto surfaceHandler = surfaceNode == nullptr ? nullptr : surfaceNode->GetMutableRSSurfaceHandler();
    if (surfaceHandler == nullptr) {
        return;
    }
    auto consumer = surfaceHandler->GetConsumer();
    auto tunnelRuntimePtr = surfaceNode->TryGetTunnelRuntimeState();
    if (tunnelRuntimePtr == nullptr) {
        return;
    }
    auto& tunnelRuntime = *tunnelRuntimePtr;
    uint64_t currentTunnelLayerId = 0;
    uint32_t currentProperty = TUNNEL_PROP_INVALID;
    tunnelRuntime.GetLayerInfo(currentTunnelLayerId, currentProperty);
    bool hasTunnelState = currentTunnelLayerId != 0;

    uint64_t newTunnelLayerId = 0;
    uint32_t newProperty = TUNNEL_PROP_INVALID;
    bool resolved = RSTunnelLayerHelper::ResolveTunnelLayerInfo(
        surfaceNode, newTunnelLayerId, newProperty);
    if (!resolved || !IsNewTunnelLayerValid(newTunnelLayerId, newProperty)) {
        if (!hasTunnelState) {
            return;
        }
        auto tunnelLayerGeneration = tunnelRuntime.GetTunnelLayerGeneration();
        ProcessLayerStateChanged(
            surfaceNode, surfaceNode->GetId(), LayerStateChange::UNAVAILABLE, tunnelLayerGeneration, consumer);
        return;
    }

    auto tunnelState = tunnelRuntime.GetTunnelState();
    bool sameTunnelInfo = currentTunnelLayerId == newTunnelLayerId && currentProperty == newProperty;
    if (sameTunnelInfo && tunnelState == RSTunnelRuntimeState::TunnelState::BUILDING) {
        // Keep runtime buffers while waiting for the layer-created callback.
        return;
    }
    if (sameTunnelInfo && tunnelState == RSTunnelRuntimeState::TunnelState::ACTIVE) {
        return;
    }
    RSTunnelLayerHelper::BeginTunnelBuilding(*surfaceNode, newTunnelLayerId, newProperty);
}

void RSTunnelLayerManager::HandleLayerStateChanged(
    NodeId nodeId, LayerStateChange state, uint64_t tunnelLayerGeneration)
{
    auto surfaceNode = GetSurfaceNode(nodeId);
    ProcessLayerStateChanged(surfaceNode, nodeId, state, tunnelLayerGeneration, nullptr);
}

void RSTunnelLayerManager::ProcessLayerStateChanged(
    const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode, NodeId nodeId,
    LayerStateChange state, uint64_t tunnelLayerGeneration, const sptr<IConsumerSurface>& consumer)
{
    if (surfaceNode == nullptr) {
        return;
    }
    auto tunnelRuntimePtr = surfaceNode->TryGetTunnelRuntimeState();
    if (tunnelRuntimePtr == nullptr) {
        return;
    }
    uint64_t trackedTunnelLayerId = 0;
    uint32_t trackedProperty = TUNNEL_PROP_INVALID;
    tunnelRuntimePtr->GetLayerInfo(trackedTunnelLayerId, trackedProperty);
    bool isTunnelStateTracked = trackedTunnelLayerId != 0;
    LayerStateChange callbackState = isTunnelStateTracked && state != LayerStateChange::AVAILABLE ?
        LayerStateChange::UNAVAILABLE : state;
    auto tunnelState = tunnelRuntimePtr->GetTunnelState();

    bool shouldDispatch = callbackState != LayerStateChange::AVAILABLE ||
        (isTunnelStateTracked &&
            HandleAvailableCallback(surfaceNode, nodeId, tunnelState, tunnelLayerGeneration));
    if (callbackState == LayerStateChange::UNAVAILABLE) {
        RSTunnelLayerHelper::ResetTunnelState(surfaceNode);
        RS_TRACE_NAME_FMT("TUNNEL_DEBUG %s tunnel reset, nodeId=%" PRIu64, __func__, nodeId);
        RS_LOGI_IF(DEBUG_TUNNEL, "%{public}s %{public}s tunnel reset, nodeId:%{public}" PRIu64,
            TUNNEL_DEBUG_PREFIX, __func__, nodeId);
    }
    if (shouldDispatch) {
        auto surfaceHandler = surfaceNode->GetMutableRSSurfaceHandler();
        auto callbackConsumer = consumer;
        if (callbackConsumer == nullptr && surfaceHandler != nullptr) {
            callbackConsumer = surfaceHandler->GetConsumer();
        }
        DispatchLayerStateChanged(nodeId, callbackState, callbackConsumer);
    }
}

void RSTunnelLayerManager::DispatchLayerStateChanged(
    NodeId nodeId, LayerStateChange state, const sptr<IConsumerSurface>& callbackConsumer) const
{
    if (callbackConsumer == nullptr) {
        return;
    }

    GSError ret = callbackConsumer->NotifyLayerStateChanged(state);
    if (ret != GSERROR_OK) {
        RS_LOGE("%{public}s %{public}s NotifyLayerStateChanged failed, nodeId:%{public}" PRIu64
            ", state:%{public}u, ret:%{public}d",
            TUNNEL_DEBUG_PREFIX, __func__, nodeId, static_cast<uint32_t>(state), ret);
    }
}
} // namespace Rosen
} // namespace OHOS
