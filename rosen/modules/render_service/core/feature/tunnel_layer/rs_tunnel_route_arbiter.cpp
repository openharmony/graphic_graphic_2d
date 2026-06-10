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

#include "common/rs_tunnel_layer_utils.h"
#include "params/rs_surface_render_params.h"
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

#ifndef ROSEN_CROSS_PLATFORM
constexpr float ALPHA_EPS = 1e-3f;

const RSSurfaceRenderParams* GetSurfaceParams(const RSSurfaceRenderNode& node)
{
#ifdef RS_ENABLE_GPU
    auto& stagingParams = const_cast<RSSurfaceRenderNode&>(node).GetStagingRenderParams();
    return static_cast<const RSSurfaceRenderParams*>(stagingParams.get());
#else
    (void)node;
    return nullptr;
#endif
}

const RSSurfaceRenderParams* GetSurfaceParams(RSSurfaceRenderNode& node)
{
#ifdef RS_ENABLE_GPU
    auto& stagingParams = node.GetStagingRenderParams();
    return static_cast<const RSSurfaceRenderParams*>(stagingParams.get());
#else
    (void)node;
    return nullptr;
#endif
}

RSTunnelRuntimeState::LastFrameRouteSnapshot BuildSnapshot(RSSurfaceRenderNode& node)
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
    uint64_t tunnelLayerId = 0;
    uint32_t tunnelProperty = TUNNEL_PROP_INVALID;
    RSTunnelRuntimeStore::GetLayerInfoOrDefault(node.GetId(), tunnelLayerId, tunnelProperty);
    snapshot.tunnelLayerId = tunnelLayerId;
    snapshot.tunnelProperty = tunnelProperty;
    return snapshot;
}

// Iterates screen nodes once to detect any non-mirror virtual screen on the tree, which
// covers extended-desktop and virtual-recording outputs. Mirror virtual screens are filtered
// out so the caller's earlier MIRROR_ACTIVE check stays the canonical mirror probe.
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
        return {true, cause};
    }
    return {};
}

bool ComputeGlobalRouteForcedNormal()
{
    return RSTunnelRouteArbiter::ComputeGlobalForbiddenCause(RSMainThread::Instance()) != nullptr;
}

RouteDecision CheckSurfaceConstantTriggers(const RSSurfaceRenderNode& node, const RSSurfaceRenderParams* params)
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
    auto& tunnelRuntime = RSTunnelRuntimeStore::GetOrCreate(node.GetId());
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

RouteDecision ShouldGoNormalThisFrame(const RSSurfaceRenderNode& node)
{
    // Kill-switch: if the runtime flag flips off while tunnelState is still ACTIVE, the
    // listener path is already blocked (IsTunnelDirectAllowed gates on the same flag) and
    // returning KEEP_DIRECT here would strand buffers in the consumer queue. Forcing
    // GO_NORMAL lets ConsumeAndUpdateBuffer drain them; UpdateTunnelLayerState then resets
    // tunnelState afterwards so the state machine converges.
    if (!IsNewTunnelEnabled()) {
        return {true, "NEW_TUNNEL_DISABLED"};
    }
    auto& tunnelRuntime = RSTunnelRuntimeStore::GetOrCreate(node.GetId());
    if (tunnelRuntime.HasPendingBuffer()) {
        return {true, "PENDING_BUFFER_STALE"};
    }
    // Drain buffers stranded in the consumer queue by listener reject paths that never reached
    // AcquirePendingBuffer (state/global/phase/CanCommit/AcquireBuffer early-exits). Those paths
    // leave availableBufferCount > 0 without filling pendingBuffer_, so HasPendingBuffer cannot
    // observe them. Routing to GO_NORMAL lets the RS pipeline consume the stale entry. The
    // steady-state direct path is unaffected because AcquirePendingBuffer resyncs the handler
    // count to the consumer's true count after a successful listener commit.
    if (auto handler = node.GetRSSurfaceHandler();
        handler != nullptr && handler->GetAvailableBufferCount() > 0) {
        return {true, "CONSUMER_QUEUE_STALE"};
    }
    auto global = CheckGlobalTriggers();
    if (global.needNormal) {
        return global;
    }
    auto* params = GetSurfaceParams(node);
    auto constant = CheckSurfaceConstantTriggers(node, params);
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

void CaptureRouteSnapshot(RSSurfaceRenderNode& node)
{
    if (!IsNewTunnelEnabled()) {
        return;
    }
    auto* params = GetSurfaceParams(node);
    if (params == nullptr || params->GetBuffer() == nullptr) {
        RS_TRACE_NAME_FMT("TUNNEL_DEBUG %s skip, nodeId=%" PRIu64 ", reason=%s",
            __func__, node.GetId(), params == nullptr ? "no_params" : "no_buffer");
        return;
    }
    RSTunnelRuntimeStore::GetOrCreate(node.GetId()).UpdateLastFrameRouteSnapshot(BuildSnapshot(node));
}
#else  // ROSEN_CROSS_PLATFORM
RouteDecision ShouldGoNormalThisFrame(const RSSurfaceRenderNode& /* node */)
{
    return {};
}

void CaptureRouteSnapshot(RSSurfaceRenderNode& /* node */)
{
}
#endif // ROSEN_CROSS_PLATFORM
} // namespace

std::atomic<bool> RSTunnelRouteArbiter::globalRouteForcedNormal_{false};

void RSTunnelRouteArbiter::RefreshGlobalTriggerSnapshot()
{
#ifndef ROSEN_CROSS_PLATFORM
    globalRouteForcedNormal_.store(ComputeGlobalRouteForcedNormal(), std::memory_order_release);
#endif
}

bool RSTunnelRouteArbiter::IsGlobalRouteForcedNormal()
{
    return globalRouteForcedNormal_.load(std::memory_order_acquire);
}

const char* RSTunnelRouteArbiter::ComputeGlobalForbiddenCause(RSMainThread* mainThread)
{
#ifndef ROSEN_CROSS_PLATFORM
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
    // Tunnel must not enter ACTIVE until the composition path has been proven
    // stable for consecutive frames. A low or zero consecutiveDoCompSuccessCount_
    // means DoComp hasn't run (or just recovered from failure / GPU composition),
    // and activating tunnel now risks an immediate GO_NORMAL forced by the next
    // unstable frame, creating a CommitTunnelLayerBySurfaceId vs
    // CommitAndGetReleaseFence conflict at the HdiOutput level.
    if (mainThread->GetConsecutiveDoCompSuccessCount() < TUNNEL_STABLE_THRESHOLD) {
        return "UNSTABLE_COMPOSITION";
    }
#endif
    return nullptr;
}

RSTunnelRouteArbiter::MainThreadOutcome RSTunnelRouteArbiter::ArbitrateAndClaim(
    const std::shared_ptr<RSSurfaceRenderNode>& node)
{
    if (node == nullptr) {
        return MainThreadOutcome::NOT_TUNNEL_ACTIVE;
    }
    if (!RSTunnelRuntimeStore::IsTunnelActive(node->GetId())) {
        return MainThreadOutcome::NOT_TUNNEL_ACTIVE;
    }
    auto& tunnelRuntime = RSTunnelRuntimeStore::GetOrCreate(node->GetId());
    auto decision = ShouldGoNormalThisFrame(*node);
    auto claim = tunnelRuntime.TryClaimByMain(decision.needNormal);
    RS_TRACE_NAME_FMT("TUNNEL_DEBUG %s arbitrate, nodeId=%" PRIu64
        ", needNormal=%d, cause=%s, claim=%s",
        __func__, node->GetId(), decision.needNormal,
        decision.cause ? decision.cause : "",
        ToClaimResultName(claim));
    if (claim == RSTunnelRuntimeState::ClaimResult::KEEP_DIRECT) {
        return MainThreadOutcome::KEEP_DIRECT;
    }
    CaptureRouteSnapshot(*node);
    // Snapshot the screen at claim time. If the surface migrates to another screen before the
    // commit-done callback fires, the original screen's commit still advances this entry, so
    // NORMAL_PREPARING cannot stall indefinitely.
    normalRouteNodes_.emplace_back(node, node->GetScreenId());
    return MainThreadOutcome::GO_NORMAL;
}

void RSTunnelRouteArbiter::AbandonNormalClaim(const std::shared_ptr<RSSurfaceRenderNode>& node)
{
    if (node == nullptr) {
        return;
    }
    RSTunnelRuntimeStore::GetOrCreate(node->GetId()).AbandonNormalClaim();
    // Self-reschedule so the next vsync re-arbitrates. The failed claim leaves either a fresh
    // buffer that will arrive momentarily (drained by CONSUMER_QUEUE_STALE) or a stranded
    // pending entry (drained by PENDING_BUFFER_STALE). Without this we would wait for an
    // external trigger, stretching the recovery to multiple vsync intervals.
    if (auto* mainThread = RSMainThread::Instance(); mainThread != nullptr) {
        mainThread->RequestNextVSync("tunnel-abandon-claim");
    }
    RS_TRACE_NAME_FMT("TUNNEL_DEBUG %s nodeId=%" PRIu64, __func__, node->GetId());
    RS_LOGD("TUNNEL_DEBUG %{public}s nodeId:%{public}" PRIu64, __func__, node->GetId());
}

void RSTunnelRouteArbiter::OnRenderCommitDone(ScreenId screenId)
{
    auto iter = std::remove_if(normalRouteNodes_.begin(), normalRouteNodes_.end(),
        [screenId](auto& entry) {
            auto node = entry.first.lock();
            if (node == nullptr) {
                return true;
            }
            if (entry.second != screenId) {
                return false;
            }
            RSTunnelRuntimeStore::GetOrCreate(node->GetId()).OnRenderCommitDone();
            return true;
        });
    normalRouteNodes_.erase(iter, normalRouteNodes_.end());
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
