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

#ifndef RENDER_SERVICE_BASE_FEATURE_TUNNEL_LAYER_RS_TUNNEL_RUNTIME_STATE_H
#define RENDER_SERVICE_BASE_FEATURE_TUNNEL_LAYER_RS_TUNNEL_RUNTIME_STATE_H

#include <atomic>
#include <cstdint>
#include <memory>
#include <mutex>

#include <surface_type.h>

#include "common/rs_common_def.h"
#include "common/rs_macros.h"
#include "screen_manager/screen_types.h"

namespace OHOS {
namespace Rosen {
class RSLayer;
class RSB_EXPORT RSTunnelRuntimeState {
public:
    enum class TunnelState : uint32_t {
        NOT_TUNNEL,
        BUILDING,
        ACTIVE,
    };

    enum class Phase : uint32_t {
        TUNNEL_IDLE,
        NORMAL_PREPARING,
        NORMAL_COMMITTED,
        TUNNEL_INFLIGHT,
    };

    enum class ClaimResult : uint32_t {
        GO_NORMAL,
        KEEP_DIRECT,
    };

    RSTunnelRuntimeState() = default;
    ~RSTunnelRuntimeState() noexcept = default;

    struct LastFrameRouteSnapshot {
        GraphicIRect dstRect = {0};
        GraphicIRect srcRect = {0};
        GraphicTransformType transformType = GraphicTransformType::GRAPHIC_ROTATE_NONE;
        int32_t zOrder = 0;
        float alpha = 1.0f;
        GraphicBlendType blendType = GraphicBlendType::GRAPHIC_BLEND_NONE;
        bool hwcEnabled = false;
        int32_t bufferWidth = 0;
        int32_t bufferHeight = 0;
        int32_t bufferFormat = 0;
        uint64_t tunnelLayerId = 0;
        uint32_t tunnelProperty = TUNNEL_PROP_INVALID;
    };

    void SetLayerInfo(uint64_t tunnelLayerId, uint32_t property);
    void GetLayerInfo(uint64_t& tunnelLayerId, uint32_t& property) const;
    // Bind the node to its screen's RSComposerClient; set in the normal path so the
    // tunnel commit path can route by screenId instead of guessing from nodeId.
    void SetScreenId(ScreenId screenId) { screenId_.store(screenId, std::memory_order_release); }
    ScreenId GetScreenId() const { return screenId_.load(std::memory_order_acquire); }
    // Set only by the LPP path (sourceType == OH_SURFACE_SOURCE_LOWPOWERVIDEO); consumers must not clear it.
    void SetLppTunnel(bool isLpp) { isLppTunnel_.store(isLpp, std::memory_order_release); }
    bool IsLppTunnel() const { return isLppTunnel_.load(std::memory_order_acquire); }
    TunnelState GetTunnelState() const;
    void SetBuilding();
    void IncrementTunnelLayerGeneration();
    void SetNotTunnel();
    bool SetActiveFromTunnelLayerAvailable(uint64_t tunnelLayerGeneration);
    bool IsTunnelDirectAllowed() const;

    uint64_t GetTunnelLayerGeneration() const;
    bool IsCurrentTunnelLayerGeneration(uint64_t tunnelLayerGeneration) const;
    void DisableTunnelLayer();
    // Reset state/phase/pending back to NOT_TUNNEL but keep layerId/property so
    // arbiter's NOT_TUNNEL -> BUILDING gate (layerId != 0) stays satisfiable.
    // Use this when a new layerId arrives and we want to re-gate cleanly without
    // wiping the (about-to-be-overwritten) layer info.
    void ResetStateKeepLayerInfo();

    void SetCommittedTunnelBufferId(uint64_t bufferId);
    bool IsCommittedTunnelBuffer() const;
    void ClearCommittedTunnelBuffer();

    Phase GetPhase() const;
    bool TryClaimByListener();
    bool TryClaimByListener(Phase& claimedFrom);
    bool ReleaseByListener();
    ClaimResult TryClaimByMain(bool needNormal);
    void OnRenderCommitDone();
    void AbandonNormalClaim();
    bool IsPendingParam() const;

    const LastFrameRouteSnapshot& GetLastFrameRouteSnapshot() const;
    void UpdateLastFrameRouteSnapshot(const LastFrameRouteSnapshot& snapshot);
    void SetTunnelLayer(std::shared_ptr<RSLayer> tunnelLayer)
    {
        tunnelLayer_ = tunnelLayer;
    }
    std::shared_ptr<RSLayer> GetTunnelLayer()
    {
        return tunnelLayer_;
    }

private:
    std::atomic<uint64_t> tunnelLayerId_ { 0 };
    std::atomic<uint32_t> tunnelLayerProperty_ { TUNNEL_PROP_INVALID };
    std::atomic<TunnelState> tunnelState_ { TunnelState::NOT_TUNNEL };
    std::atomic<uint64_t> tunnelLayerGeneration_ { 1 };
    std::atomic<Phase> phase_ { Phase::TUNNEL_IDLE };
    std::atomic_bool pendingParam_ { false };
    std::atomic<uint64_t> committedTunnelBufferId_ { 0 };
    std::atomic_bool isLppTunnel_ { false };
    std::shared_ptr<RSLayer> tunnelLayer_ { nullptr };
    std::atomic<ScreenId> screenId_ { 0 };

    LastFrameRouteSnapshot lastFrameRouteSnapshot_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_BASE_FEATURE_TUNNEL_LAYER_RS_TUNNEL_RUNTIME_STATE_H
