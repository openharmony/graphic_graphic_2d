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

#include "feature/tunnel_layer/rs_tunnel_route_arbiter.h"

#include <algorithm>
#include <cinttypes>
#include <cmath>

#include "feature/tunnel_layer/rs_tunnel_layer_utils.h"
#include "params/rs_surface_render_params.h"
#include "platform/common/rs_log.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "feature/tunnel_layer/rs_tunnel_runtime_state.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
namespace {
struct RouteDecision {
    bool needNormal = false;
    const char* cause = nullptr;
};

constexpr float ALPHA_EPS = 1e-3f;

const RSSurfaceRenderParams* GetSurfaceParams(const RSSurfaceRenderNode& node)
{
#ifdef RS_ENABLE_GPU
    // GetStagingRenderParams is non-const; same const_cast pattern as IsNodeHdrOn in helper.
    auto& stagingParams = const_cast<RSSurfaceRenderNode&>(node).GetStagingRenderParams();
    return static_cast<const RSSurfaceRenderParams*>(stagingParams.get());
#else
    (void)node;
    return nullptr;
#endif
}

RSTunnelRuntimeState::LastFrameRouteSnapshot BuildSnapshot(RSSurfaceRenderNode& node,
    RSTunnelRuntimeState& tunnelRuntime)
{
    RSTunnelRuntimeState::LastFrameRouteSnapshot snapshot;
    auto* params = GetSurfaceParams(node);
    if (params != nullptr) {
        const auto& layerInfo = params->GetLayerInfo();
        snapshot.dstRect = layerInfo.dstRect;
        snapshot.srcRect = layerInfo.srcRect;
        snapshot.transformType = layerInfo.transformType;
        snapshot.zOrder = layerInfo.zOrder;
        snapshot.alpha = layerInfo.alpha;
        snapshot.blendType = layerInfo.blendType;
        auto buffer = params->GetBuffer();
        if (buffer != nullptr) {
            snapshot.bufferWidth = buffer->GetWidth();
            snapshot.bufferHeight = buffer->GetHeight();
            snapshot.bufferFormat = buffer->GetFormat();
        }
    }
    snapshot.hwcEnabled = !node.IsHardwareForcedDisabled();
    tunnelRuntime.GetLayerInfo(snapshot.tunnelLayerId, snapshot.tunnelProperty);
    return snapshot;
}

// Detect any non-mirror virtual screen (mirrors stay covered by the MIRROR_ACTIVE probe).
bool HasNonMirrorVirtualScreen(RSMainThread* mainThread)
{
    if (mainThread == nullptr) {
        return false;
    }
    bool found = false;
    const auto& nodeMap = mainThread->GetContext().GetNodeMap();
    nodeMap.TraverseScreenNodes([&found](const std::shared_ptr<RSScreenRenderNode>& screenNode) {
        if (found || screenNode == nullptr) {
            return;
        }
        if (screenNode->GetMirrorSource().lock()) {
            return;
        }
        if (screenNode->GetScreenProperty().IsVirtual()) {
            found = true;
        }
    });
    return found;
}

RouteDecision CheckGlobalTriggers()
{
    if (auto* cause = RSTunnelRouteArbiter::ComputeGlobalForbiddenCause(RSMainThread::Instance())) {
        RS_LOGD_IF(DEBUG_TUNNEL,  "%{public}s %{public}s ComputeClobalForbiddenCause cause %{public}s",
            TUNNEL_DEBUG_PREFIX, __func__, cause);
        RS_TRACE_NAME_FMT("TUNNEL_DEBUG %s ComputeGlobalForbiddenCause cause %s", __func__, cause);
        return {true, cause};
    }
    return {};
}

bool ComputeGlobalRouteForcedNormal()
{
    return RSTunnelRouteArbiter::ComputeGlobalForbiddenCause(RSMainThread::Instance()) != nullptr;
}

RouteDecision CheckSurfaceConstantTriggers(const RSSurfaceRenderNode& node,
    const RSSurfaceRenderParams* params, RSTunnelRuntimeState& tunnelRuntime)
{
    if (params != nullptr && params->IsNodeToBeCaptured()) {
        return {true, "CAPTURE_ACTIVE"};
    }
    if (node.GetSpecialLayerMgr().Find(SpecialLayerType::PROTECTED)) {
        return {true, "PROTECTED_LAYER"};
    }
    if (node.IsHardwareForcedDisabled()) {
        return {true, "HWC_DISABLED"};
    }
    uint64_t curId = 0;
    uint32_t curProp = TUNNEL_PROP_INVALID;
    tunnelRuntime.GetLayerInfo(curId, curProp);
    const auto& last = tunnelRuntime.GetLastFrameRouteSnapshot();
    if (curId != last.tunnelLayerId || curProp != last.tunnelProperty) {
        return {true, "TUNNEL_INFO_CHANGED"};
    }
    return {};
}

RouteDecision CheckLayerInfoDiff(const RSLayerInfo& cur,
    const RSTunnelRuntimeState::LastFrameRouteSnapshot& last)
{
    if (!(cur.dstRect == last.dstRect)) {
        return {true, "GEOMETRY_CHANGED"};
    }
    if (!(cur.srcRect == last.srcRect)) {
        return {true, "GEOMETRY_CHANGED"};
    }
    if (cur.transformType != last.transformType) {
        return {true, "TRANSFORM_CHANGED"};
    }
    if (cur.zOrder != last.zOrder) {
        return {true, "ZORDER_CHANGED"};
    }
    if (std::fabs(cur.alpha - last.alpha) > ALPHA_EPS) {
        return {true, "ALPHA_CHANGED"};
    }
    if (cur.blendType != last.blendType) {
        return {true, "BLEND_CHANGED"};
    }
    return {};
}

RouteDecision CheckHwcDiff(const RSSurfaceRenderNode& node,
    const RSTunnelRuntimeState::LastFrameRouteSnapshot& last)
{
    bool curHwcEnabled = !node.IsHardwareForcedDisabled();
    if (curHwcEnabled != last.hwcEnabled) {
        return {true, "HWC_FALLBACK"};
    }
    return {};
}

RouteDecision CheckBufferDiff(const RSSurfaceRenderParams* params,
    const RSTunnelRuntimeState::LastFrameRouteSnapshot& last)
{
    if (params == nullptr) {
        return {};
    }
    auto buffer = params->GetBuffer();
    if (buffer == nullptr) {
        return {};
    }
    int32_t w = buffer->GetWidth();
    int32_t h = buffer->GetHeight();
    int32_t fmt = buffer->GetFormat();
    if (w != last.bufferWidth || h != last.bufferHeight) {
        return {true, "BUFFER_GEOMETRY_CHANGED"};
    }
    if (fmt != last.bufferFormat) {
        return {true, "BUFFER_FORMAT_CHANGED"};
    }
    return {};
}

RouteDecision ShouldGoNormalThisFrame(const RSSurfaceRenderNode& node,
    RSTunnelRuntimeState& tunnelRuntime)
{
    // NOT_TUNNEL: not eligible for GO_NORMAL evaluation; only ACTIVE surfaces are meaningful here.
    if (tunnelRuntime.GetTunnelState() == RSTunnelRuntimeState::TunnelState::NOT_TUNNEL) {
        return {};
    }
    // Drain buffers stranded by listener reject paths; GO_NORMAL lets the pipeline consume them.
    if (auto handler = node.GetRSSurfaceHandler();
        handler != nullptr && handler->GetAvailableBufferCount() > 0) {
        return {true, "CONSUMER_QUEUE_STALE"};
    }
    auto global = CheckGlobalTriggers();
    if (global.needNormal) {
        return global;
    }
    auto* params = GetSurfaceParams(node);
    auto constant = CheckSurfaceConstantTriggers(node, params, tunnelRuntime);
    if (constant.needNormal) {
        return constant;
    }
    if (params != nullptr) {
        auto layerDiff = CheckLayerInfoDiff(params->GetLayerInfo(),
            tunnelRuntime.GetLastFrameRouteSnapshot());
        if (layerDiff.needNormal) {
            return layerDiff;
        }
    }
    auto hwcDiff = CheckHwcDiff(node, tunnelRuntime.GetLastFrameRouteSnapshot());
    if (hwcDiff.needNormal) {
        return hwcDiff;
    }
    auto bufferDiff = CheckBufferDiff(params,
        tunnelRuntime.GetLastFrameRouteSnapshot());
    if (bufferDiff.needNormal) {
        return bufferDiff;
    }
    return {};
}

void CaptureRouteSnapshot(RSSurfaceRenderNode& node, RSTunnelRuntimeState& tunnelRuntime)
{
    if (!IsNewTunnelEnabled()) {
        return;
    }
    auto* params = GetSurfaceParams(node);
    if (params == nullptr || params->GetBuffer() == nullptr) {
        return;
    }
    tunnelRuntime.UpdateLastFrameRouteSnapshot(BuildSnapshot(node, tunnelRuntime));
}
} // namespace

std::atomic<bool> RSTunnelRouteArbiter::globalRouteForcedNormal_{false};
std::atomic<uint32_t> RSTunnelRouteArbiter::dynamicBuildingThreshold_{
    TUNNEL_NOT_TUNNEL_TO_BUILDING_THRESHOLD};
std::atomic<uint32_t> RSTunnelRouteArbiter::consecutiveSolePresentCount_{0};

uint32_t RSTunnelRouteArbiter::GetBuildingThreshold()
{
    return dynamicBuildingThreshold_.load(std::memory_order_acquire);
}

uint32_t RSTunnelRouteArbiter::GetConsecutiveSolePresentCount()
{
    return consecutiveSolePresentCount_.load(std::memory_order_acquire);
}

void RSTunnelRouteArbiter::ResetConsecutiveSolePresentCount()
{
    consecutiveSolePresentCount_.store(0, std::memory_order_release);
}

void RSTunnelRouteArbiter::OnTunnelRollbackDetected()
{
    (void)dynamicBuildingThreshold_.fetch_add(1, std::memory_order_acq_rel);
    // The rollback frame is unstable: restart the BUILDING stability gate.
    ResetConsecutiveSolePresentCount();
}

void RSTunnelRouteArbiter::OnDirectCompositionStable(uint32_t consecCount)
{
    if (consecCount > TUNNEL_STABLE_THRESHOLD) {
        (void)dynamicBuildingThreshold_.exchange(
            TUNNEL_NOT_TUNNEL_TO_BUILDING_THRESHOLD, std::memory_order_acq_rel);
    }
}

void RSTunnelRouteArbiter::RefreshGlobalTriggerSnapshot()
{
    // Kill-switch: no tunnel consumers read the snapshot when disabled; skip the screen-node scan.
    if (!IsNewTunnelEnabled()) {
        return;
    }
    globalRouteForcedNormal_.store(ComputeGlobalRouteForcedNormal(), std::memory_order_release);
}

bool RSTunnelRouteArbiter::IsGlobalRouteForcedNormal()
{
    return globalRouteForcedNormal_.load(std::memory_order_acquire);
}

const char* RSTunnelRouteArbiter::ComputeGlobalForbiddenCause(RSMainThread* mainThread)
{
    if (mainThread == nullptr) {
        return nullptr;
    }
    if (mainThread->HasMirrorDisplay()) {
        return "MIRROR_ACTIVE";
    }
    if (mainThread->IsSnapshotPendingThisFrame()) {
        return "SCREENSHOT_ACTIVE";
    }
    if (HasNonMirrorVirtualScreen(mainThread)) {
        return "VIRTUAL_SCREEN_ACTIVE";
    }
    if (mainThread->IsLastFrameGpuComposition()) {
        return "GPU_COMPOSITION";
    }
    return nullptr;
}

RSTunnelRouteArbiter::MainThreadOutcome RSTunnelRouteArbiter::ArbitrateAndClaim(
    const std::shared_ptr<RSSurfaceRenderNode>& node)
{
    if (node == nullptr) {
        return MainThreadOutcome::NOT_TUNNEL_ACTIVE;
    }
    auto tunnelRuntimePtr = node->TryGetTunnelRuntimeState();
    if (tunnelRuntimePtr == nullptr) {
        return MainThreadOutcome::NOT_TUNNEL_ACTIVE;
    }
    auto& tunnelRuntime = *tunnelRuntimePtr;
    auto state = tunnelRuntime.GetTunnelState();
    if (state != RSTunnelRuntimeState::TunnelState::ACTIVE) {
        // NOT_TUNNEL / BUILDING / others: not participating in arbitration this frame.
        return MainThreadOutcome::NOT_TUNNEL_ACTIVE;
    }
    auto decision = ShouldGoNormalThisFrame(*node, tunnelRuntime);
    if (decision.needNormal) {
        RS_TRACE_NAME_FMT("TUNNEL_DEBUG %s GO_NORMAL, cause:%s, nodeId=%" PRIu64,
            __func__, decision.cause, node->GetId());
        RS_LOGI_IF(DEBUG_TUNNEL, "%{public}s %{public}s GO_NORMAL, cause:%{public}s, "
            "nodeId:%{public}" PRIu64, TUNNEL_DEBUG_PREFIX, __func__, decision.cause, node->GetId());
    }
    auto claim = tunnelRuntime.TryClaimByMain(decision.needNormal);
    if (claim == RSTunnelRuntimeState::ClaimResult::KEEP_DIRECT) {
        return MainThreadOutcome::KEEP_DIRECT;
    }
    CaptureRouteSnapshot(*node, tunnelRuntime);
    // Snapshot the screen at claim time so a migration before commit-done cannot stall the phase.
    normalRouteNodes_[node->GetId()] = std::make_pair(node, node->GetScreenId());
    return MainThreadOutcome::GO_NORMAL;
}

std::atomic<bool> RSTunnelRouteArbiter::tunnelSolePresentLayer_{false};

bool RSTunnelRouteArbiter::GetTunnelSolePresentLayer()
{
    bool value = tunnelSolePresentLayer_.load(std::memory_order_acquire);
    RS_TRACE_NAME_FMT("TUNNEL_DEBUG %s value %d", __func__, value);
    return value;
}

void RSTunnelRouteArbiter::SetTunnelSolePresentLayer(bool isSole)
{
    RS_TRACE_NAME_FMT("TUNNEL_DEBUG %s isSole %d", __func__, isSole);
    tunnelSolePresentLayer_.store(isSole, std::memory_order_release);
    if (!isSole) {
        // A not-sole frame breaks the consecutive sole-tunnel streak.
        ResetConsecutiveSolePresentCount();
        return;
    }
    uint32_t consecCount = consecutiveSolePresentCount_.fetch_add(1, std::memory_order_acq_rel) + 1;
    // Relax the dynamic BUILDING threshold once sole-tunnel DoComp stays stable.
    OnDirectCompositionStable(consecCount);
}

void RSTunnelRouteArbiter::GetAllLayerAndTunnelLayerCount(
    const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode, uint32_t& presentCount, uint32_t& tunnelCount)
{
    if (IsNewTunnelEnabled()) {
        auto surfaceHandler = surfaceNode->GetRSSurfaceHandler();
        bool consumed = surfaceHandler != nullptr && surfaceHandler->IsCurrentFrameBufferConsumed();
        RS_TRACE_NAME_FMT("TUNNEL_DEBUG %s id:%" PRIu64 "consumed:%d", __func__, surfaceNode->GetId(), consumed);
        if (!consumed) {
            return;
        }
        ++presentCount;
        uint64_t tunnelLayerId = 0;
        uint32_t property = TUNNEL_PROP_INVALID;
        bool isTunnel = false;
        auto statePtr = surfaceNode->TryGetTunnelRuntimeState();
        if (statePtr != nullptr) {
            statePtr->GetLayerInfo(tunnelLayerId, property);
            isTunnel = IsNewTunnelProperty(property);
        }
        RS_TRACE_NAME_FMT("TUNNEL_DEBUG %s id:%" PRIu64 "presentCount:%u isTunnel:%d tunnelLayerId:%" PRIu64
                          " property:%u",
            __func__, surfaceNode->GetId(), presentCount, isTunnel, tunnelLayerId, property);
        if (isTunnel) {
            ++tunnelCount;
        }
    }
}

void RSTunnelRouteArbiter::BeginTunnelBuilding(
    RSTunnelRuntimeState& tunnelRuntime, NodeId nodeId, uint64_t tunnelLayerId)
{
    uint32_t consecCount = GetConsecutiveSolePresentCount();
    uint32_t threshold = GetBuildingThreshold();
    if (consecCount < threshold) {
        return;
    }
    tunnelRuntime.SetBuilding();
    RS_TRACE_NAME_FMT("TUNNEL_DEBUG %s NOT_TUNNEL->BUILDING, nodeId=%" PRIu64
        ", tunnelLayerId=%" PRIu64 ", consecCount=%u, threshold=%u",
        __func__, nodeId, tunnelLayerId, consecCount, threshold);
    RS_LOGI("%{public}s%{public}s NOT_TUNNEL->BUILDING, nodeId:%{public}" PRIu64
        ", tunnelLayerId:%{public}" PRIu64 ", consecCount:%{public}u, threshold:%{public}u",
        TUNNEL_DEBUG_PREFIX, __func__, nodeId, tunnelLayerId, consecCount, threshold);
}

void RSTunnelRouteArbiter::AbandonNormalClaim(const std::shared_ptr<RSSurfaceRenderNode>& node)
{
    if (node == nullptr) {
        return;
    }
    auto tunnelRuntimePtr = node->TryGetTunnelRuntimeState();
    if (tunnelRuntimePtr == nullptr) {
        return;
    }
    tunnelRuntimePtr->AbandonNormalClaim();
    // Self-reschedule: the failed claim leaves a fresh buffer or stranded pending entry to drain.
    if (auto* mainThread = RSMainThread::Instance(); mainThread != nullptr) {
        mainThread->RequestNextVSync("tunnel-abandon-claim");
    }
}

void RSTunnelRouteArbiter::OnRenderCommitDone(ScreenId screenId)
{
    for (auto iter = normalRouteNodes_.begin(); iter != normalRouteNodes_.end();) {
        auto& [nodeId, entry] = *iter;
        auto node = entry.first.lock();
        if (node == nullptr) {
            iter = normalRouteNodes_.erase(iter);
            continue;
        }
        if (entry.second != screenId) {
            ++iter;
            continue;
        }
        auto tunnelRuntimePtr = node->TryGetTunnelRuntimeState();
        if (tunnelRuntimePtr != nullptr) {
            tunnelRuntimePtr->OnRenderCommitDone();
        }
        iter = normalRouteNodes_.erase(iter);
    }
}

void RSTunnelRouteArbiter::AttachToRenderThread()
{
    RSUniRenderThread::Instance().SetCommitDoneCallback([this](ScreenId screenId) {
        auto* mainThread = RSMainThread::Instance();
        if (mainThread == nullptr) {
            return;
        }
        mainThread->PostTask([this, screenId]() {
            OnRenderCommitDone(screenId);
        });
    });
}
} // namespace Rosen
} // namespace OHOS
