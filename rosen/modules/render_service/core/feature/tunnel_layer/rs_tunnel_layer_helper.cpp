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

#include "feature/tunnel_layer/rs_tunnel_layer_helper.h"

#include <algorithm>
#include <cinttypes>

#include "feature/tunnel_layer/rs_tunnel_layer_utils.h"
#include "feature/tunnel_layer/rs_tunnel_route_arbiter.h"
#include "params/rs_surface_render_params.h"
#include "pipeline/rs_surface_handler.h"
#include "pipeline/rs_surface_render_node.h"
#include "feature/tunnel_layer/rs_tunnel_runtime_state.h"
#include "platform/common/rs_log.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "platform/common/rs_log.h"
#include "rs_composer_client_manager.h"
#include "rs_trace.h"
#include "common/rs_optional_trace.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr int64_t DIRECT_CONSUME_TIMESTAMP = 0;
constexpr int32_t NORMAL_TO_TUNNEL_RELEASE_REF_COUNT = 2;

// GetHDRPresent is the sole HDR-on source of truth; displayNit/brightnessRatio are SDR tuning fields.
bool IsNodeHdrOn(const RSSurfaceRenderNode& node)
{
#ifdef RS_ENABLE_GPU
    auto& stagingParams = const_cast<RSSurfaceRenderNode&>(node).GetStagingRenderParams();
    auto* params = static_cast<const RSSurfaceRenderParams*>(stagingParams.get());
    return params != nullptr && params->GetHDRPresent();
#else
    (void)node;
    return false;
#endif
}

RSSurfaceHandler::SurfaceBufferEntry CreateTunnelPendingBuffer(
    const IConsumerSurface::AcquireBufferReturnValue& returnValue)
{
    RSSurfaceHandler::SurfaceBufferEntry entry;
    entry.buffer = returnValue.buffer;
    entry.acquireFence = returnValue.fence;
    entry.timestamp = returnValue.timestamp;
    entry.damageRect = {
        .x = 0,
        .y = 0,
        .w = entry.buffer->GetSurfaceBufferWidth(),
        .h = entry.buffer->GetSurfaceBufferHeight(),
    };
    entry.bufferOwnerCount_->bufferId_ = entry.buffer->GetBufferId();
    entry.RegisterReleaseBufferListener([](uint64_t bufferId) {
        RSUniRenderThread::Instance().ReleaseBufferById(bufferId);
    });
    return entry;
}

// All direct-commit reject paths funnel here; kept as a named helper so reject sites stay greppable.
bool RejectDirectCommit(NodeId nodeId, const char* reason, uint64_t tunnelLayerId = 0,
    uint32_t property = TUNNEL_PROP_INVALID)
{
    RS_TRACE_NAME_FMT("TUNNEL_DEBUG %s nodeId=%" PRIu64 " , reson=%s, tunnelLayerId=%" PRIu64 ", property=%u",
        __func__, nodeId, reason, tunnelLayerId, property);
    RS_LOGD_IF(DEBUG_TUNNEL, "%s %s nodeId=%{public}" PRIu64 " , reson=%{public}s,"
        "tunnelLayerId=%{public}" PRIu64 ", property=%{public}u",
        TUNNEL_DEBUG_PREFIX, __func__, nodeId, reason, tunnelLayerId, property);
    return false;
}

bool CanCommitBufferDirect(const std::shared_ptr<RSSurfaceRenderNode>& node,
    RSTunnelRuntimeState& tunnelRuntime, uint64_t& tunnelLayerId, uint32_t& property)
{
    if (node == nullptr) {
        return RejectDirectCommit(0, "node_null");
    }
    NodeId nodeId = node->GetId();
    auto surfaceHandler = node->GetMutableRSSurfaceHandler();
    if (surfaceHandler == nullptr) {
        return RejectDirectCommit(nodeId, "handler_null");
    }
    auto consumer = surfaceHandler->GetConsumer();
    if (consumer == nullptr) {
        return RejectDirectCommit(nodeId, "consumer_null");
    }
    if (!tunnelRuntime.IsTunnelDirectAllowed()) {
        return RejectDirectCommit(nodeId, "runtime_not_active");
    }
    if (!node->IsOnTheTree()) {
        return RejectDirectCommit(nodeId, "not_on_tree");
    }
    if (RSTunnelRouteArbiter::IsGlobalRouteForcedNormal()) {
        return RejectDirectCommit(nodeId, "global_trigger");
    }
    if (node->IsHardwareForcedDisabled()) {
        return RejectDirectCommit(nodeId, "hwc_disabled");
    }
    // HDR is a hard gate (tunnel cannot tone map); listener-side check closes the first-HDR race.
    if (IsNodeHdrOn(*node)) {
        return RejectDirectCommit(nodeId, "hdr_on");
    }
    if (!RSTunnelLayerHelper::ResolveTunnelLayerInfo(
        node, tunnelLayerId, property)) {
        return RejectDirectCommit(nodeId, "resolve_layer_info_failed");
    }
    if (tunnelLayerId == 0 || !IsNewTunnelProperty(property)) {
        return RejectDirectCommit(nodeId, "invalid_layer_info", tunnelLayerId, property);
    }
    /**
     * The tunnel can be directly output only when the tunnel layer is sent to display (thrad-safe reading of bool).
     * The main thread compares the data and writes it after ConsumeAndUpdateAllNodes is complete,
     * The listener thread reads the data atomically.
     */
    if  (!RSTunnelRouteArbiter::GetTunnelSolePresentLayer()) {
        return RejectDirectCommit(nodeId, "not_sole_tunnel_layer");
    }
    return true;
}

bool CommitBuffer(const TunnelLayerCommitInfo& commitInfo,
    const std::shared_ptr<RSComposerClientManager>& composerClientManager, sptr<SyncFence>& releaseFence)
{
    releaseFence = SyncFence::InvalidFence();
    int32_t ret = composerClientManager->CommitTunnelLayerBySurfaceId(commitInfo, releaseFence);
    if (ret != GRAPHIC_DISPLAY_SUCCESS) {
        return false;
    }
    return true;
}

void ReleasePreviousNormalBuffer(const sptr<IConsumerSurface>& consumer,
    const sptr<SurfaceBuffer>& buffer, const sptr<SyncFence>& releaseFence,
    const std::shared_ptr<RSSurfaceHandler::BufferOwnerCount>& bufferOwnerCount)
{
    if (consumer == nullptr || buffer == nullptr || bufferOwnerCount == nullptr) {
        return;
    }
    // Tunnel fence merges into the pending entry so the GPU fence from the normal path is preserved.
    RSUniRenderThread::Instance().AddPendingReleaseBuffer(
        consumer, buffer, releaseFence, bufferOwnerCount);
    for (int32_t i = 0; i < NORMAL_TO_TUNNEL_RELEASE_REF_COUNT && bufferOwnerCount->refCount_.load() > 0; ++i) {
        bufferOwnerCount->OnBufferReleased();
    }
}
}

bool RSTunnelLayerHelper::ResolveTunnelLayerInfo(
    const std::shared_ptr<RSSurfaceRenderNode>& node,
    uint64_t& tunnelLayerId, uint32_t& property)
{
    if (node == nullptr) {
        return false;
    }
    tunnelLayerId = 0;
    property = TUNNEL_PROP_INVALID;
    auto statePtr = node->TryGetTunnelRuntimeState();
    if (statePtr) {
        statePtr->GetLayerInfo(tunnelLayerId, property);
    }
    return true;
}

bool RSTunnelLayerHelper::TryCommitBufferDirect(const std::shared_ptr<RSSurfaceRenderNode>& node,
    const std::shared_ptr<RSComposerClientManager>& composerClientManager,
    bool preBufferFromTunnel, RSTunnelRuntimeState& tunnelRuntime)
{
    if (node == nullptr) {
        return false;
    }
    if (!tunnelRuntime.IsTunnelDirectAllowed()) {
        return false;
    }
    uint64_t tunnelLayerId = 0;
    uint32_t property = TUNNEL_PROP_INVALID;
    if (!CanCommitBufferDirect(node, tunnelRuntime, tunnelLayerId, property)) {
        return false;
    }
    auto surfaceHandler = node->GetMutableRSSurfaceHandler();
    if (surfaceHandler == nullptr) {
        return false;
    }
    std::lock_guard<std::mutex> lock(surfaceHandler->GetTunnelMutex());
    auto pendingBuffer = AcquirePendingBuffer(node, tunnelRuntime);
    if (pendingBuffer.buffer == nullptr) {
        return false;
    }
    return TryCommitPendingBuffer(node, composerClientManager, preBufferFromTunnel, tunnelRuntime, pendingBuffer);
}

bool RSTunnelLayerHelper::TryCommitPendingBuffer(const std::shared_ptr<RSSurfaceRenderNode>& node,
    const std::shared_ptr<RSComposerClientManager>& composerClientManager,
    bool preBufferFromTunnel, RSTunnelRuntimeState& tunnelRuntime,
    RSSurfaceHandler::SurfaceBufferEntry pendingBuffer)
{
    if (node == nullptr || pendingBuffer.buffer == nullptr) {
        return false;
    }
    auto surfaceHandler = node->GetMutableRSSurfaceHandler();
    if (surfaceHandler == nullptr) {
        return false;
    }

    auto existingBuffer = surfaceHandler->GetBuffer();
    if (existingBuffer != nullptr) {
        if (existingBuffer->GetWidth() != pendingBuffer.buffer->GetWidth() ||
            existingBuffer->GetHeight() != pendingBuffer.buffer->GetHeight()) {
            pendingBuffer.bufferOwnerCount_->OnBufferReleased();
            return false;
        }
    }

    uint64_t tunnelLayerId = 0;
    uint32_t property = TUNNEL_PROP_INVALID;
    auto consumer = surfaceHandler->GetConsumer();
    bool canCommit = CanCommitBufferDirect(node, tunnelRuntime, tunnelLayerId, property);
    if (!canCommit || consumer == nullptr || composerClientManager == nullptr) {
        tunnelRuntime.DisableTunnelLayer();
        pendingBuffer.bufferOwnerCount_->OnBufferReleased();
        return false;
    }

    sptr<SyncFence> releaseFence = SyncFence::InvalidFence();
    TunnelLayerCommitInfo commitInfo;
    commitInfo.surfaceId = consumer->GetUniqueId();
    commitInfo.nodeId = node->GetId();
    commitInfo.tunnelLayerId = tunnelLayerId;
    commitInfo.screenId = tunnelRuntime.GetScreenId();
    commitInfo.buffer = pendingBuffer.buffer;
    commitInfo.acquireFence = pendingBuffer.acquireFence;
    if (!CommitBuffer(commitInfo, composerClientManager, releaseFence)) {
        tunnelRuntime.DisableTunnelLayer();
        pendingBuffer.bufferOwnerCount_->OnBufferReleased();
        return false;
    }

    auto pendingReleaseFence = releaseFence == nullptr ? SyncFence::InvalidFence() : releaseFence;
    RSUniRenderThread::Instance().AddPendingReleaseBuffer(
        consumer, pendingBuffer.buffer, pendingReleaseFence, pendingBuffer.bufferOwnerCount_);
    tunnelRuntime.SetCommittedTunnelBufferId(pendingBuffer.buffer->GetBufferId());
    surfaceHandler->ConsumeAndUpdateBuffer(pendingBuffer);
    auto preBuffer = surfaceHandler->GetPreBuffer();
    auto preBufferOwnerCount = surfaceHandler->GetPreBufferOwnerCount();
    node->UpdateTunnelBufferInfo(surfaceHandler->GetBuffer(), surfaceHandler->GetBufferOwnerCount(),
        surfaceHandler->GetDamageRegion(), surfaceHandler->GetAcquireFence(), preBuffer, preBufferOwnerCount);
    if (preBuffer != nullptr && preBufferOwnerCount != nullptr) {
        ReleaseTunnelLayer(tunnelRuntime.GetTunnelLayer(), preBuffer->GetBufferId());
        if (preBufferFromTunnel) {
            preBufferOwnerCount->OnBufferReleased();
            surfaceHandler->ResetPreBuffer(false);
        } else {
            RS_TRACE_NAME_FMT("TUNNEL_DEBUG %s previous normal buffer count %d",
                __func__, preBufferOwnerCount->refCount_.load());
            ReleasePreviousNormalBuffer(consumer, preBuffer, pendingReleaseFence, preBufferOwnerCount);
            surfaceHandler->ResetPreBuffer(false);
        }
    }
    tunnelRuntime.SetTunnelLayer(CreateTunnelLayer(node, composerClientManager, pendingBuffer));
    RS_TRACE_NAME_FMT("TUNNEL_DEBUG %s success, nodeId=%" PRIu64 ", tunnelLayerId=%" PRIu64
        ", bufferId=%" PRIu64 ", releaseFence=%d",
        __func__, node->GetId(), tunnelLayerId, pendingBuffer.buffer->GetBufferId(),
        releaseFence ? releaseFence->Get() : -1);
    RS_LOGI_IF(DEBUG_TUNNEL, "%{public}s %{public}s success, nodeId:%{public}" PRIu64
        ", tunnelLayerId:%{public}" PRIu64 ", bufferId:%{public}" PRIu64 ", releaseFence:%{public}d",
        TUNNEL_DEBUG_PREFIX, __func__, node->GetId(), tunnelLayerId, pendingBuffer.buffer->GetBufferId(),
        releaseFence ? releaseFence->Get() : -1);
    return true;
}

void RSTunnelLayerHelper::BeginTunnelBuilding(RSSurfaceRenderNode& node,
    uint64_t tunnelLayerId, uint32_t property)
{
    auto tunnelRuntimePtr = node.TryGetTunnelRuntimeState();
    if (tunnelRuntimePtr == nullptr) {
        return;
    }
    // tunnelLayerId == 0: unbind (full Clear); != 0: rebind — reset state, keep layerId/property.
    if (tunnelLayerId == 0) {
        tunnelRuntimePtr->DisableTunnelLayer();
    } else {
        tunnelRuntimePtr->ResetStateKeepLayerInfo();
    }
    tunnelRuntimePtr->SetLayerInfo(tunnelLayerId, property);
    // NOT_TUNNEL -> BUILDING gate: arbiter BeginTunnelBuilding, bind path + dynamic threshold.
    if (tunnelLayerId != 0) {
        RSTunnelRouteArbiter::BeginTunnelBuilding(*tunnelRuntimePtr, node.GetId(), tunnelLayerId);
    }
}

void RSTunnelLayerHelper::ResetTunnelState(const std::shared_ptr<RSSurfaceRenderNode>& node)
{
    if (node != nullptr) {
        // UNAVAILABLE callback: reset state keeping layerId/property for later re-gating.
        auto statePtr = node->TryGetTunnelRuntimeState();
        if (statePtr) {
            statePtr->ResetStateKeepLayerInfo();
        }
    }
}

RSSurfaceHandler::SurfaceBufferEntry RSTunnelLayerHelper::AcquirePendingBuffer(
    const std::shared_ptr<RSSurfaceRenderNode>& node, RSTunnelRuntimeState& tunnelRuntime)
{
    if (node == nullptr) {
        return {};
    }
    if (!tunnelRuntime.IsTunnelDirectAllowed()) {
        return {};
    }
    auto surfaceHandler = node->GetMutableRSSurfaceHandler();
    if (surfaceHandler == nullptr || surfaceHandler->GetAvailableBufferCount() <= 0) {
        return {};
    }
    auto consumer = surfaceHandler->GetConsumer();
    if (consumer == nullptr) {
        return {};
    }

    IConsumerSurface::AcquireBufferReturnValue returnValue;
    int32_t ret = consumer->AcquireBuffer(returnValue, DIRECT_CONSUME_TIMESTAMP, false);
    surfaceHandler->SetAvailableBufferCount(static_cast<int32_t>(consumer->GetAvailableBufferCount()));
    if (ret != SURFACE_ERROR_OK || returnValue.buffer == nullptr) {
        return {};
    }

    auto pendingBuffer = CreateTunnelPendingBuffer(returnValue);
    RSUniRenderThread::Instance().AddPendingReleaseBuffer(
        consumer, pendingBuffer.buffer, SyncFence::InvalidFence(), pendingBuffer.bufferOwnerCount_);
    RS_TRACE_NAME_FMT("TUNNEL_DEBUG %s success, nodeId=%" PRIu64 ", bufferId=%" PRIu64 ", available=%u",
        __func__, surfaceHandler->GetNodeId(), returnValue.buffer->GetBufferId(),
        consumer->GetAvailableBufferCount());
    RS_LOGI_IF(DEBUG_TUNNEL, "%{public}s %{public}s success, nodeId:%{public}" PRIu64
        ", bufferId:%{public}" PRIu64 ", available:%{public}u",
        TUNNEL_DEBUG_PREFIX, __func__, surfaceHandler->GetNodeId(), returnValue.buffer->GetBufferId(),
        consumer->GetAvailableBufferCount());
    return pendingBuffer;
}

RSLayerPtr RSTunnelLayerHelper::CreateTunnelLayer(const std::shared_ptr<RSSurfaceRenderNode>& node,
    const std::shared_ptr<RSComposerClientManager>& composerClientManager,
    const RSSurfaceHandler::SurfaceBufferEntry& bufferEntry)
{
    auto composerClient = composerClientManager->GetComposerClient(node->GetScreenId());
    if (!composerClient) {
        return nullptr;
    }
    auto composerContext = composerClient->GetComposerContext();
    if (!composerContext) {
        return nullptr;
    }
    RSLayerPtr layer = RSSurfaceLayer::Create(node->GetId(), composerContext);
    if (!layer) {
        return nullptr;
    }
    if (bufferEntry.buffer) {
        layer->SetBuffer(bufferEntry.buffer);
    }
    layer->SetBufferOwnerCount(bufferEntry.bufferOwnerCount_);
    auto uniRsLayer = composerContext->GetUniRsLayer();
    std::shared_ptr<RSSurfaceHandler::BufferOwnerCount> uniBufferCount = nullptr;
    if (uniRsLayer) {
        uniBufferCount = uniRsLayer->GetBufferOwnerCount();
    }
    if (!uniBufferCount) {
        return layer;
    }
    if (layer->GetBuffer()) {
        uniBufferCount->InsertUniOnDrawSet(layer->GetRSLayerId(), layer->GetBuffer()->GetBufferId());
    }
    // On tunnel-to-GPU switch, bind the self-drawn buffer to the unified rendering buffer for release.
    if (bufferEntry.bufferOwnerCount_) {
        bufferEntry.bufferOwnerCount_->SetUniBufferOwner(uniBufferCount->bufferId_, node->GetScreenId());
    }
    return layer;
}

void RSTunnelLayerHelper::ReleaseTunnelLayer(const RSLayerPtr& layer, uint64_t bufferId)
{
    if (!layer) {
        return;
    }
    auto bufferOwnerCount = layer->PopBufferOwnerCountById(bufferId);
    if (bufferOwnerCount) {
        bufferOwnerCount->OnBufferReleased();
    }
}

RSTunnelLayerHelper::ListenerHandleResult RSTunnelLayerHelper::HandleListenerBuffer(
    const std::shared_ptr<RSSurfaceRenderNode>& node,
    const std::shared_ptr<RSSurfaceHandler>& surfaceHandler,
    const std::shared_ptr<RSComposerClientManager>& composerClientManager)
{
    ListenerHandleResult result;
    if (node == nullptr || surfaceHandler == nullptr) {
        return result;
    }
    auto tunnelRuntimePtr = node->TryGetTunnelRuntimeState();
    if (tunnelRuntimePtr == nullptr) {
        return result;
    }
    auto& tunnelRuntime = *tunnelRuntimePtr;
    auto tunnelState = tunnelRuntime.GetTunnelState();
    if (composerClientManager == nullptr) {
        return result;
    }
    if (tunnelState != RSTunnelRuntimeState::TunnelState::ACTIVE) {
        return result;
    }
    // Mirror/snapshot/capture: skip listener claim so ArbitrateAndClaim routes GO_NORMAL next vsync.
    if (RSTunnelRouteArbiter::IsGlobalRouteForcedNormal()) {
        return result;
    }
    auto claimedFrom = RSTunnelRuntimeState::Phase::TUNNEL_IDLE;
    if (!tunnelRuntime.TryClaimByListener(claimedFrom)) {
        return result;
    }
    tunnelRuntime.SetScreenId(node->GetScreenId());
    // PreBuffer provenance: read IsCommittedTunnelBuffer before this frame's SetCommittedTunnelBufferId.
    bool preBufferFromTunnel = claimedFrom == RSTunnelRuntimeState::Phase::TUNNEL_IDLE &&
        tunnelRuntime.IsCommittedTunnelBuffer();
    bool hasCommitCandidateBuffer = surfaceHandler->GetAvailableBufferCount() > 0;
    bool directCommitted = hasCommitCandidateBuffer &&
        TryCommitBufferDirect(node, composerClientManager, preBufferFromTunnel, tunnelRuntime);
    if (directCommitted) {
        RSTunnelRouteArbiter::ResetConsecutiveSolePresentCount();
    }
    bool needRequestVsync = tunnelRuntime.ReleaseByListener();
    result.committed = directCommitted;
    result.needRequestVsync = needRequestVsync;
    return result;
}

} // namespace Rosen
} // namespace OHOS
