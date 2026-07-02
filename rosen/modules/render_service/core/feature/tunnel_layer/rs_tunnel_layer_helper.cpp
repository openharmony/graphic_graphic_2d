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

#include "common/rs_tunnel_layer_utils.h"
#include "feature/tunnel_layer/rs_tunnel_route_arbiter.h"
#include "params/rs_surface_render_params.h"
#include "pipeline/rs_surface_handler.h"
#include "pipeline/rs_surface_render_node.h"
#include "feature/tunnel_layer/rs_tunnel_runtime_state.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "platform/common/rs_log.h"
#include "rs_composer_client_manager.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr int64_t DIRECT_CONSUME_TIMESTAMP = 0;
constexpr int32_t NORMAL_TO_TUNNEL_RELEASE_REF_COUNT = 2;

// GetHDRPresent is the single source of truth for HDR-on (RSHdrUtil::SetHDRPresent is the
// only setter across the codebase). displayNit/brightnessRatio are brightness-tuning fields
// shared with SDR (displayNit defaults to 500.0f, brightnessRatio is the SDR dimming factor)
// and must not be used as HDR proxies.
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

bool RejectDirectCommit(NodeId nodeId, const char* reason, uint64_t tunnelLayerId = 0,
    uint32_t property = TUNNEL_PROP_INVALID)
{
    RS_TRACE_NAME_FMT("TUNNEL_DEBUG %s nodeId=%" PRIu64 ", reason=%s, tunnelLayerId=%" PRIu64 ", property=%u",
        __func__, nodeId, reason, tunnelLayerId, property);
    RS_LOGD("%{public}s%{public}s nodeId:%{public}" PRIu64 ", reason:%{public}s, "
        "tunnelLayerId:%{public}" PRIu64 ", property:%{public}u",
        TUNNEL_DEBUG_PREFIX, __func__, nodeId, reason, tunnelLayerId, property);
    return false;
}

bool CanCommitBufferDirect(const std::shared_ptr<RSSurfaceRenderNode>& node,
    uint64_t& tunnelLayerId, uint32_t& property)
{
    if (node == nullptr) {
        return RejectDirectCommit(0, "node_null");
    }
    NodeId nodeId = node->GetId();
    auto surfaceHandler = node->GetMutableRSSurfaceHandler();
    if (!Rosen::IsNewTunnelEnabled() || surfaceHandler == nullptr) {
        return RejectDirectCommit(nodeId, "disabled_or_handler_null");
    }
    auto consumer = surfaceHandler->GetConsumer();
    if (consumer == nullptr) {
        return RejectDirectCommit(nodeId, "consumer_null");
    }
    if (!RSTunnelRuntimeStore::GetOrCreate(nodeId).IsTunnelDirectAllowed()) {
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
    // HDR is a hard capability gate: tunnel direct cannot perform tone mapping. Listener-side
    // absolute check closes the race where producer flushes the first HDR buffer before the
    // arbiter has run for the new frame.
    if (IsNodeHdrOn(*node)) {
        return RejectDirectCommit(nodeId, "hdr_on");
    }
    if (!RSTunnelLayerHelper::ResolveTunnelLayerInfo(consumer, tunnelLayerId, property, nodeId)) {
        return RejectDirectCommit(nodeId, "resolve_layer_info_failed");
    }
    if (tunnelLayerId == 0 || !IsNewTunnelProperty(property)) {
        return RejectDirectCommit(nodeId, "invalid_layer_info", tunnelLayerId, property);
    }
    return true;
}

void PreparePendingTunnelBufferForFallback(const std::shared_ptr<RSSurfaceHandler>& surfaceHandler,
    const RSSurfaceHandler::SurfaceBufferEntry& pendingBuffer)
{
    uint64_t bufferId = pendingBuffer.buffer->GetBufferId();
    if (surfaceHandler->GetHoldReturnValue() != nullptr) {
        surfaceHandler->SetAvailableBufferCount(std::max(surfaceHandler->GetAvailableBufferCount(), 1));
        RS_TRACE_NAME_FMT("TUNNEL_DEBUG %s holdReturnValue occupied, keep tunnel pending bufferId=%" PRIu64,
            __func__, bufferId);
        RS_LOGD("%{public}s%{public}s holdReturnValue occupied, keep tunnel pending bufferId:%{public}" PRIu64,
            TUNNEL_DEBUG_PREFIX, __func__, bufferId);
        return;
    }
    auto consumer = surfaceHandler->GetConsumer();
    int32_t availableBufferCount = consumer == nullptr ? 0 : static_cast<int32_t>(consumer->GetAvailableBufferCount());
    surfaceHandler->SetAvailableBufferCount(availableBufferCount > 0 ? availableBufferCount : 1);
    RS_TRACE_NAME_FMT("TUNNEL_DEBUG %s bufferId=%" PRIu64 ", available=%d", __func__,
        bufferId, surfaceHandler->GetAvailableBufferCount());
    RS_LOGD("%{public}s%{public}s keep pending buffer for fallback, bufferId:%{public}" PRIu64
        ", available:%{public}d",
        TUNNEL_DEBUG_PREFIX, __func__, bufferId, surfaceHandler->GetAvailableBufferCount());
}

bool CommitBuffer(const TunnelLayerCommitInfo& commitInfo,
    const std::shared_ptr<RSComposerClientManager>& composerClientManager, sptr<SyncFence>& releaseFence)
{
    if (!Rosen::IsNewTunnelEnabled()) {
        return false;
    }

    releaseFence = SyncFence::InvalidFence();
    int32_t ret = composerClientManager->CommitTunnelLayerBySurfaceId(commitInfo, releaseFence);
    if (ret != GRAPHIC_DISPLAY_SUCCESS) {
        RS_LOGD("%{public}s%{public}s commit tunnel layer skipped, surfaceId:%{public}" PRIu64
            ", nodeId:%{public}" PRIu64 ", tunnelLayerId:%{public}" PRIu64
            ", bufferId:%{public}" PRIu64 ", acquireFence:%{public}d, ret:%{public}d",
            TUNNEL_DEBUG_PREFIX, __func__, commitInfo.surfaceId, commitInfo.nodeId,
            commitInfo.tunnelLayerId, commitInfo.buffer->GetBufferId(),
            commitInfo.acquireFence ? commitInfo.acquireFence->Get() : -1, ret);
        return false;
    }
    RS_TRACE_NAME_FMT("TUNNEL_DEBUG %s success, surfaceId=%" PRIu64 ", nodeId=%" PRIu64
        ", tunnelLayerId=%" PRIu64 ", bufferId=%" PRIu64 ", acquireFence=%d, releaseFence=%d",
        __func__, commitInfo.surfaceId, commitInfo.nodeId, commitInfo.tunnelLayerId,
        commitInfo.buffer->GetBufferId(), commitInfo.acquireFence ? commitInfo.acquireFence->Get() : -1,
        releaseFence ? releaseFence->Get() : -1);
    RS_LOGD("%{public}s%{public}s success, surfaceId:%{public}" PRIu64 ", nodeId:%{public}" PRIu64
        ", tunnelLayerId:%{public}" PRIu64 ", bufferId:%{public}" PRIu64
        ", acquireFence:%{public}d, releaseFence:%{public}d",
        TUNNEL_DEBUG_PREFIX, __func__, commitInfo.surfaceId, commitInfo.nodeId,
        commitInfo.tunnelLayerId, commitInfo.buffer->GetBufferId(),
        commitInfo.acquireFence ? commitInfo.acquireFence->Get() : -1, releaseFence ? releaseFence->Get() : -1);
    return true;
}

void ReleasePreviousNormalBuffer(const sptr<IConsumerSurface>& consumer,
    const sptr<SurfaceBuffer>& buffer, const sptr<SyncFence>& releaseFence,
    const std::shared_ptr<RSSurfaceHandler::BufferOwnerCount>& bufferOwnerCount)
{
    if (consumer == nullptr || buffer == nullptr || bufferOwnerCount == nullptr) {
        return;
    }
    RSUniRenderThread::Instance().ReplacePendingReleaseBufferFence(
        consumer, buffer, releaseFence, bufferOwnerCount);
    for (int32_t i = 0; i < NORMAL_TO_TUNNEL_RELEASE_REF_COUNT && bufferOwnerCount->refCount_.load() > 0; ++i) {
        bufferOwnerCount->OnBufferReleased();
    }
    RS_TRACE_NAME_FMT("TUNNEL_DEBUG %s bufferId=%" PRIu64 ", fence=%d, refCount=%d",
        __func__, buffer->GetBufferId(), releaseFence ? releaseFence->Get() : -1, bufferOwnerCount->refCount_.load());
    RS_LOGD("%{public}s%{public}s bufferId:%{public}" PRIu64 ", fence:%{public}d, refCount:%{public}d",
        TUNNEL_DEBUG_PREFIX, __func__, buffer->GetBufferId(), releaseFence ? releaseFence->Get() : -1,
        bufferOwnerCount->refCount_.load());
}
}

bool RSTunnelLayerHelper::ResolveTunnelLayerInfo(
    const sptr<IConsumerSurface>& consumer, uint64_t& tunnelLayerId, uint32_t& property, NodeId nodeId)
{
    if (!Rosen::IsNewTunnelEnabled() || consumer == nullptr) {
        return false;
    }

    tunnelLayerId = 0;
    property = TUNNEL_PROP_INVALID;
    if (nodeId != 0 && RSTunnelRuntimeStore::GetLayerInfoIfPresent(nodeId, tunnelLayerId, property)) {
        if (tunnelLayerId != 0) {
            return true;
        }
    }
    return true;
}

bool RSTunnelLayerHelper::TryCommitBufferDirect(const std::shared_ptr<RSSurfaceRenderNode>& node,
    const std::shared_ptr<RSComposerClientManager>& composerClientManager, bool consumePendingBuffer,
    bool previousFrameWasRs)
{
    if (node == nullptr) {
        return false;
    }
    auto& tunnelRuntime = RSTunnelRuntimeStore::GetOrCreate(node->GetId());
    if (!tunnelRuntime.IsTunnelDirectAllowed()) {
        RS_TRACE_NAME_FMT("TUNNEL_DEBUG %s reject, nodeId=%" PRIu64 ", state=%s",
            __func__, node->GetId(), ToTunnelStateName(tunnelRuntime.GetTunnelState()));
        RS_LOGD("%{public}s%{public}s reject, nodeId:%{public}" PRIu64 ", state:%{public}s",
            TUNNEL_DEBUG_PREFIX, __func__, node->GetId(), ToTunnelStateName(tunnelRuntime.GetTunnelState()));
        return false;
    }
    // The tunnel-direct check must happen before AcquirePendingBuffer. If RS normal is consuming
    // this surface, tunnel direct must leave the producer queue untouched.
    if (consumePendingBuffer && !tunnelRuntime.HasPendingBuffer()) {
        uint64_t tunnelLayerId = 0;
        uint32_t property = TUNNEL_PROP_INVALID;
        if (!CanCommitBufferDirect(node, tunnelLayerId, property)) {
            return false;
        }
        if (!AcquirePendingBuffer(node) && !tunnelRuntime.HasPendingBuffer()) {
            return false;
        }
    }
    return TryCommitPendingBuffer(node, composerClientManager, consumePendingBuffer, previousFrameWasRs);
}

bool RSTunnelLayerHelper::TryCommitPendingBuffer(const std::shared_ptr<RSSurfaceRenderNode>& node,
    const std::shared_ptr<RSComposerClientManager>& composerClientManager, bool fallbackOnFailure,
    bool previousFrameWasRs)
{
    if (node == nullptr) {
        return false;
    }
    auto surfaceHandler = node->GetMutableRSSurfaceHandler();
    if (surfaceHandler == nullptr) {
        return false;
    }
    auto& tunnelRuntime = RSTunnelRuntimeStore::GetOrCreate(node->GetId());

    if (surfaceHandler->GetBuffer() &&
        tunnelRuntime.IsBufferSizeChanged(surfaceHandler->GetBuffer()->GetSize())) {
        return false;
    }

    RSSurfaceHandler::SurfaceBufferEntry pendingBuffer;
    if (!tunnelRuntime.TakePendingBuffer(pendingBuffer)) {
        return false;
    }

    uint64_t tunnelLayerId = 0;
    uint32_t property = TUNNEL_PROP_INVALID;
    auto consumer = surfaceHandler->GetConsumer();
    bool canCommit = CanCommitBufferDirect(node, tunnelLayerId, property);
    if (!canCommit || consumer == nullptr || composerClientManager == nullptr) {
        if (fallbackOnFailure) {
            PreparePendingTunnelBufferForFallback(surfaceHandler, pendingBuffer);
        }
        tunnelRuntime.SetPendingBuffer(pendingBuffer);
        return false;
    }

    sptr<SyncFence> releaseFence = SyncFence::InvalidFence();
    TunnelLayerCommitInfo commitInfo;
    commitInfo.surfaceId = consumer->GetUniqueId();
    commitInfo.nodeId = node->GetId();
    commitInfo.tunnelLayerId = tunnelLayerId;
    commitInfo.buffer = pendingBuffer.buffer;
    commitInfo.acquireFence = pendingBuffer.acquireFence;
    if (!CommitBuffer(commitInfo, composerClientManager, releaseFence)) {
        tunnelRuntime.SetLayerInfo(0, TUNNEL_PROP_INVALID);
        tunnelRuntime.SetBuilding();
        if (fallbackOnFailure) {
            PreparePendingTunnelBufferForFallback(surfaceHandler, pendingBuffer);
        }
        tunnelRuntime.SetPendingBuffer(pendingBuffer);
        return false;
    }

    auto pendingReleaseFence = releaseFence == nullptr ? SyncFence::InvalidFence() : releaseFence;
    RSUniRenderThread::Instance().AddPendingReleaseBuffer(
        consumer, pendingBuffer.buffer, pendingReleaseFence, pendingBuffer.bufferOwnerCount_);
    tunnelRuntime.SetCommittedTunnelBufferId(pendingBuffer.buffer->GetBufferId());
    surfaceHandler->ConsumeAndUpdateBuffer(pendingBuffer);
    auto preBuffer = surfaceHandler->GetPreBuffer();
    auto preBufferOwnerCount = surfaceHandler->GetPreBufferOwnerCount();
    if (preBuffer != nullptr && preBufferOwnerCount != nullptr) {
        ReleaseTunnelLayer(tunnelRuntime.GetTunnelLayer(), preBuffer->GetBufferId());
        if (previousFrameWasRs) {
            ReleasePreviousNormalBuffer(consumer, preBuffer, pendingReleaseFence, preBufferOwnerCount);
            surfaceHandler->ResetPreBuffer(false);
        } else {
            preBufferOwnerCount->OnBufferReleased();
            surfaceHandler->ResetPreBuffer(false);
        }
    }
    tunnelRuntime.SetTunnelLayer(CreateTunnelLayer(node, composerClientManager, pendingBuffer));
    RS_TRACE_NAME_FMT("TUNNEL_DEBUG %s success, nodeId=%" PRIu64 ", tunnelLayerId=%" PRIu64
        ", bufferId=%" PRIu64 ", releaseFence=%d",
        __func__, node->GetId(), tunnelLayerId, pendingBuffer.buffer->GetBufferId(),
        releaseFence ? releaseFence->Get() : -1);
    RS_LOGD("%{public}s%{public}s success, nodeId:%{public}" PRIu64 ", tunnelLayerId:%{public}" PRIu64
        ", bufferId:%{public}" PRIu64 ", releaseFence:%{public}d",
        TUNNEL_DEBUG_PREFIX, __func__, node->GetId(), tunnelLayerId, pendingBuffer.buffer->GetBufferId(),
        releaseFence ? releaseFence->Get() : -1);
    return true;
}

void RSTunnelLayerHelper::BeginTunnelBuilding(NodeId nodeId, uint64_t tunnelLayerId, uint32_t property)
{
    auto& tunnelRuntime = RSTunnelRuntimeStore::GetOrCreate(nodeId);
    uint64_t currentTunnelLayerId = 0;
    uint32_t currentProperty = TUNNEL_PROP_INVALID;
    tunnelRuntime.GetLayerInfo(currentTunnelLayerId, currentProperty);
    if (currentTunnelLayerId != tunnelLayerId || currentProperty != property) {
        tunnelRuntime.Clear();
    }
    tunnelRuntime.SetLayerInfo(tunnelLayerId, property);
    tunnelRuntime.SetBuilding();
    RS_TRACE_NAME_FMT("TUNNEL_DEBUG %s BUILDING, nodeId=%" PRIu64 ", tunnelLayerId=%" PRIu64
        ", property=%u", __func__, nodeId, tunnelLayerId, property);
    RS_LOGD("%{public}s%{public}s BUILDING, nodeId:%{public}" PRIu64 ", tunnelLayerId:%{public}" PRIu64
        ", property:%{public}u",
        TUNNEL_DEBUG_PREFIX, __func__, nodeId, tunnelLayerId, property);
}

void RSTunnelLayerHelper::ResetTunnelState(const std::shared_ptr<RSSurfaceRenderNode>& node)
{
    if (node != nullptr) {
        RSTunnelRuntimeStore::Clear(node->GetId());
    }
}

bool RSTunnelLayerHelper::AcquirePendingBuffer(const std::shared_ptr<RSSurfaceRenderNode>& node)
{
    if (node == nullptr) {
        return false;
    }
    auto& tunnelRuntime = RSTunnelRuntimeStore::GetOrCreate(node->GetId());
    auto tunnelState = tunnelRuntime.GetTunnelState();
    if (!tunnelRuntime.IsTunnelDirectAllowed()) {
        RS_TRACE_NAME_FMT("TUNNEL_DEBUG %s reject, nodeId=%" PRIu64 ", state=%s",
            __func__, node->GetId(), ToTunnelStateName(tunnelState));
        RS_LOGD("%{public}s%{public}s reject, nodeId:%{public}" PRIu64 ", state:%{public}s",
            TUNNEL_DEBUG_PREFIX, __func__, node->GetId(), ToTunnelStateName(tunnelState));
        return false;
    }
    auto surfaceHandler = node->GetMutableRSSurfaceHandler();
    if (surfaceHandler == nullptr || surfaceHandler->GetAvailableBufferCount() <= 0) {
        return false;
    }
    auto consumer = surfaceHandler->GetConsumer();
    if (consumer == nullptr) {
        return false;
    }

    IConsumerSurface::AcquireBufferReturnValue returnValue;
    int32_t ret = consumer->AcquireBuffer(returnValue, DIRECT_CONSUME_TIMESTAMP, false);
    surfaceHandler->SetAvailableBufferCount(static_cast<int32_t>(consumer->GetAvailableBufferCount()));
    if (ret != SURFACE_ERROR_OK || returnValue.buffer == nullptr) {
        RS_LOGD_IF(DEBUG_PIPELINE, "%{public}sRSTunnelLayerHelper::AcquirePendingBuffer failed, "
            "nodeId:%{public}" PRIu64 ", ret:%{public}d", TUNNEL_DEBUG_PREFIX, surfaceHandler->GetNodeId(), ret);
        return false;
    }

    auto pendingBuffer = CreateTunnelPendingBuffer(returnValue);
    RSUniRenderThread::Instance().AddPendingReleaseBuffer(
        consumer, pendingBuffer.buffer, SyncFence::InvalidFence(), pendingBuffer.bufferOwnerCount_);
    tunnelRuntime.SetPendingBuffer(pendingBuffer);
    RS_TRACE_NAME_FMT("TUNNEL_DEBUG %s success, nodeId=%" PRIu64 ", bufferId=%" PRIu64 ", available=%u",
        __func__, surfaceHandler->GetNodeId(), returnValue.buffer->GetBufferId(),
        consumer->GetAvailableBufferCount());
    RS_LOGD("%{public}s%{public}s success, nodeId:%{public}" PRIu64 ", bufferId:%{public}" PRIu64
        ", available:%{public}u",
        TUNNEL_DEBUG_PREFIX, __func__, surfaceHandler->GetNodeId(), returnValue.buffer->GetBufferId(),
        consumer->GetAvailableBufferCount());
    return true;
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
    if (uniBufferCount && layer->GetBuffer()) {
        uniBufferCount->InsertUniOnDrawSet(layer->GetRSLayerId(), layer->GetBuffer()->GetBufferId());
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
    auto& tunnelRuntime = RSTunnelRuntimeStore::GetOrCreate(node->GetId());
    auto tunnelState = tunnelRuntime.GetTunnelState();
    if (composerClientManager == nullptr) {
        RS_TRACE_NAME_FMT("TUNNEL_DEBUG %s rejected, nodeId=%" PRIu64 ", reason=composer_null, state=%s",
            __func__, node->GetId(), ToTunnelStateName(tunnelState));
        RS_LOGD("TUNNEL_DEBUG %{public}s rejected, nodeId:%{public}" PRIu64
            ", reason:composer_null, state:%{public}s",
            __func__, node->GetId(), ToTunnelStateName(tunnelState));
        return result;
    }
    if (tunnelState != RSTunnelRuntimeState::TunnelState::ACTIVE) {
        RS_TRACE_NAME_FMT("TUNNEL_DEBUG %s rejected, nodeId=%" PRIu64 ", reason=state, state=%s",
            __func__, node->GetId(), ToTunnelStateName(tunnelState));
        RS_LOGD("TUNNEL_DEBUG %{public}s rejected, nodeId:%{public}" PRIu64
            ", reason:state, state:%{public}s",
            __func__, node->GetId(), ToTunnelStateName(tunnelState));
        return result;
    }
    // Mirror / snapshot / capture need this surface in the RS pipeline, not on the DSS direct
    // path. Skip the listener claim so the buffer stays in the consumer queue and main's
    // ArbitrateAndClaim can route GO_NORMAL on the next vsync. Falling through with empty
    // result lets the caller drive SetBufferInfoAndRequest, which already requests the vsync.
    if (RSTunnelRouteArbiter::IsGlobalRouteForcedNormal()) {
        RS_TRACE_NAME_FMT("TUNNEL_DEBUG %s rejected, nodeId=%" PRIu64 ", reason=global_trigger",
            __func__, node->GetId());
        RS_LOGD("TUNNEL_DEBUG %{public}s rejected, nodeId:%{public}" PRIu64 ", reason:global_trigger",
            __func__, node->GetId());
        return result;
    }
    auto claimedFrom = RSTunnelRuntimeState::Phase::TUNNEL_IDLE;
    if (!tunnelRuntime.TryClaimByListener(claimedFrom)) {
        RS_TRACE_NAME_FMT("TUNNEL_DEBUG %s rejected, nodeId=%" PRIu64 ", reason=phase,"
            "phase=%s, pending=%d, claimedFrom:%d",
            __func__, node->GetId(), ToPhaseName(tunnelRuntime.GetPhase()),
            tunnelRuntime.IsPendingParam(), claimedFrom);
        RS_LOGD("TUNNEL_DEBUG %{public}s rejected, nodeId:%{public}" PRIu64
            ", reason:phase, phase:%{public}s, pending:%{public}d",
            __func__, node->GetId(), ToPhaseName(tunnelRuntime.GetPhase()), tunnelRuntime.IsPendingParam());
        return result;
    }
    RS_OPTIONAL_TRACE_NAME_FMT("HandleListenerBuffer claimedFrom:%d", claimedFrom);
    bool previousFrameWasRs = claimedFrom == RSTunnelRuntimeState::Phase::NORMAL_COMMITTED ||
        claimedFrom == RSTunnelRuntimeState::Phase::TUNNEL_IDLE;
    bool hasCommitCandidateBuffer = surfaceHandler->GetAvailableBufferCount() > 0 ||
        tunnelRuntime.HasPendingBuffer();
    bool directCommitted = hasCommitCandidateBuffer &&
        TryCommitBufferDirect(node, composerClientManager, true, previousFrameWasRs);
    if (directCommitted) {
        RSMainThread::Instance()->ResetConsecutiveDoCompSuccessCount();
    }
    bool needRequestVsync = tunnelRuntime.ReleaseByListener();
    RS_TRACE_NAME_FMT("TUNNEL_DEBUG %s nodeId=%" PRIu64
        ", candidate=%d, committed=%d, available=%d, pending=%d",
        __func__, node->GetId(), hasCommitCandidateBuffer, directCommitted,
        surfaceHandler->GetAvailableBufferCount(), needRequestVsync);
    RS_LOGD("TUNNEL_DEBUG %s nodeId:%{public}" PRIu64
        ", candidate:%{public}d, committed:%{public}d, available:%{public}d, pending:%{public}d",
        __func__, node->GetId(), hasCommitCandidateBuffer, directCommitted,
        surfaceHandler->GetAvailableBufferCount(), needRequestVsync);
    result.committed = directCommitted;
    result.needRequestVsync = needRequestVsync;
    return result;
}

} // namespace Rosen
} // namespace OHOS
