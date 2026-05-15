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

#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_TUNNEL_RUNTIME_STATE_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_TUNNEL_RUNTIME_STATE_H

#include <atomic>
#include <cstdint>
#include <mutex>

#include <surface_type.h>

#include "common/rs_common_def.h"
#include "common/rs_macros.h"
#include "pipeline/rs_surface_handler.h"

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSTunnelRuntimeState {
public:
    enum class TunnelState : uint32_t {
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
    ~RSTunnelRuntimeState() noexcept;

#ifndef ROSEN_CROSS_PLATFORM
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
#endif

    void SetLayerInfo(uint64_t tunnelLayerId, uint32_t property);
    void GetLayerInfo(uint64_t& tunnelLayerId, uint32_t& property) const;
    TunnelState GetTunnelState() const;
    void SetBuilding();
    bool SetActiveFromTunnelLayerAvailable(uint64_t tunnelLayerGeneration);
    bool IsTunnelDirectAllowed() const;

    uint64_t GetTunnelLayerGeneration() const;
    bool IsCurrentTunnelLayerGeneration(uint64_t tunnelLayerGeneration) const;
    void Clear();

#ifndef ROSEN_CROSS_PLATFORM
    bool HasPendingBuffer() const;
    void SetPendingBuffer(RSSurfaceHandler::SurfaceBufferEntry buffer);
    bool TakePendingBuffer(RSSurfaceHandler::SurfaceBufferEntry& buffer);
#endif

    void SetCommittedTunnelBufferId(uint64_t bufferId);
    bool IsCommittedTunnelBuffer(uint64_t bufferId) const;
    void ClearCommittedTunnelBuffer();

    Phase GetPhase() const;
    bool TryClaimByListener();
    bool TryClaimByListener(Phase& claimedFrom);
    bool ReleaseByListener();
    ClaimResult TryClaimByMain(bool needNormal);
    void OnRenderCommitDone();
    void AbandonNormalClaim();
    bool IsPendingParam() const;

#ifndef ROSEN_CROSS_PLATFORM
    const LastFrameRouteSnapshot& GetLastFrameRouteSnapshot() const;
    void UpdateLastFrameRouteSnapshot(const LastFrameRouteSnapshot& snapshot);
#endif

private:
#ifndef ROSEN_CROSS_PLATFORM
    static bool IsPendingBufferValid(const RSSurfaceHandler::SurfaceBufferEntry& buffer);
    static void ReleasePendingBuffer(RSSurfaceHandler::SurfaceBufferEntry& buffer);
#endif

    mutable std::mutex mutex_;
    uint64_t tunnelLayerId_ = 0;
    uint32_t tunnelLayerProperty_ = TUNNEL_PROP_INVALID;
    std::atomic<TunnelState> tunnelState_ { TunnelState::BUILDING };
    std::atomic<uint64_t> tunnelLayerGeneration_ { 1 };
    std::atomic<Phase> phase_ { Phase::TUNNEL_IDLE };
    std::atomic_bool pendingParam_ { false };
    std::atomic<uint64_t> committedTunnelBufferId_ { 0 };

#ifndef ROSEN_CROSS_PLATFORM
    RSSurfaceHandler::SurfaceBufferEntry pendingBuffer_;
    LastFrameRouteSnapshot lastFrameRouteSnapshot_;
#endif
};

inline const char* ToTunnelStateName(RSTunnelRuntimeState::TunnelState state)
{
    switch (state) {
        case RSTunnelRuntimeState::TunnelState::BUILDING:
            return "BUILDING";
        case RSTunnelRuntimeState::TunnelState::ACTIVE:
            return "ACTIVE";
        default:
            return "UNKNOWN";
    }
}

inline const char* ToPhaseName(RSTunnelRuntimeState::Phase phase)
{
    switch (phase) {
        case RSTunnelRuntimeState::Phase::TUNNEL_IDLE:
            return "TUNNEL_IDLE";
        case RSTunnelRuntimeState::Phase::NORMAL_PREPARING:
            return "NORMAL_PREPARING";
        case RSTunnelRuntimeState::Phase::NORMAL_COMMITTED:
            return "NORMAL_COMMITTED";
        case RSTunnelRuntimeState::Phase::TUNNEL_INFLIGHT:
            return "TUNNEL_INFLIGHT";
        default:
            return "UNKNOWN";
    }
}

inline const char* ToClaimResultName(RSTunnelRuntimeState::ClaimResult result)
{
    switch (result) {
        case RSTunnelRuntimeState::ClaimResult::GO_NORMAL:
            return "GO_NORMAL";
        case RSTunnelRuntimeState::ClaimResult::KEEP_DIRECT:
            return "KEEP_DIRECT";
        default:
            return "UNKNOWN";
    }
}
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_TUNNEL_RUNTIME_STATE_H
