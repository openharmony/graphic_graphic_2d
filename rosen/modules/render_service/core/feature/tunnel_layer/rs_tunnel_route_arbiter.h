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

#ifndef RS_TUNNEL_ROUTE_ARBITER_H
#define RS_TUNNEL_ROUTE_ARBITER_H

#include <atomic>
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#include "common/rs_common_def.h"
#include "screen_manager/screen_types.h"

namespace OHOS {
namespace Rosen {
class RSMainThread;
class RSSurfaceRenderNode;
class RSTunnelRuntimeState;

class RSTunnelRouteArbiter {
public:
    enum class MainThreadOutcome : uint32_t {
        NOT_TUNNEL_ACTIVE,
        GO_NORMAL,
        KEEP_DIRECT,
    };

    RSTunnelRouteArbiter() = default;
    ~RSTunnelRouteArbiter() = default;
    RSTunnelRouteArbiter(const RSTunnelRouteArbiter&) = delete;
    RSTunnelRouteArbiter& operator=(const RSTunnelRouteArbiter&) = delete;

    MainThreadOutcome ArbitrateAndClaim(const std::shared_ptr<RSSurfaceRenderNode>& node);
    void AbandonNormalClaim(const std::shared_ptr<RSSurfaceRenderNode>& node);
    void OnRenderCommitDone(ScreenId screenId);
    void AttachToRenderThread();

    static bool GetTunnelSolePresentLayer();
    static void SetTunnelSolePresentLayer(bool isSole);
    static void GetAllLayerAndTunnelLayerCount(
        const std::shared_ptr<RSSurfaceRenderNode>& surfaceNode, uint32_t& presentCount, uint32_t& tunnelCount);

    // NOT_TUNNEL -> BUILDING gate (bind path, dynamic threshold); below threshold, re-check next entry.
    static void BeginTunnelBuilding(
        RSTunnelRuntimeState& tunnelRuntime, NodeId nodeId, uint64_t tunnelLayerId);

    // Dynamic threshold state: TunnelRollbackDetected +1 (no cap); DirectCompositionStable resets to base.
    static uint32_t GetBuildingThreshold();
    static void OnTunnelRollbackDetected();
    static void OnDirectCompositionStable(uint32_t consecCount);

    // Consecutive sole-tunnel present frames; resets to 0 on rollback / listener commit / not-sole frames.
    static uint32_t GetConsecutiveSolePresentCount();
    static void ResetConsecutiveSolePresentCount();

    static void RefreshGlobalTriggerSnapshot();
    static bool IsGlobalRouteForcedNormal();

    static const char* ComputeGlobalForbiddenCause(RSMainThread* mainThread);

private:
    // Pair the weak node with claim-time screenId so migration cannot stall the NORMAL phase.
    std::unordered_map<NodeId, std::pair<std::weak_ptr<RSSurfaceRenderNode>, ScreenId>> normalRouteNodes_;
    static std::atomic<bool> tunnelSolePresentLayer_;
    static std::atomic<bool> globalRouteForcedNormal_;
    // Dynamic NOT_TUNNEL -> BUILDING threshold; atomic (main writes, listener reads).
    static std::atomic<uint32_t> dynamicBuildingThreshold_;
    // Consecutive sole-tunnel present frames; atomic (render thread writes, main/listener read).
    static std::atomic<uint32_t> consecutiveSolePresentCount_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_TUNNEL_ROUTE_ARBITER_H
