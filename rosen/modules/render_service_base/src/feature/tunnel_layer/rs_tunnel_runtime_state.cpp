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

#include "feature/tunnel_layer/rs_tunnel_runtime_state.h"

#include "platform/common/rs_log.h"
#include "common/rs_optional_trace.h"

namespace OHOS {
namespace Rosen {
void RSTunnelRuntimeState::SetLayerInfo(uint64_t tunnelLayerId, uint32_t property)
{
    tunnelLayerId_.store(tunnelLayerId, std::memory_order_release);
    tunnelLayerProperty_.store(property, std::memory_order_release);
}

void RSTunnelRuntimeState::GetLayerInfo(uint64_t& tunnelLayerId, uint32_t& property) const
{
    tunnelLayerId = tunnelLayerId_.load(std::memory_order_acquire);
    property = tunnelLayerProperty_.load(std::memory_order_acquire);
}

RSTunnelRuntimeState::TunnelState RSTunnelRuntimeState::GetTunnelState() const
{
    return tunnelState_.load(std::memory_order_acquire);
}

void RSTunnelRuntimeState::SetBuilding()
{
    phase_.store(Phase::TUNNEL_IDLE, std::memory_order_release);
    pendingParam_.store(false, std::memory_order_release);
    tunnelState_.store(TunnelState::BUILDING, std::memory_order_release);
}

void RSTunnelRuntimeState::IncrementTunnelLayerGeneration()
{
    tunnelLayerGeneration_.fetch_add(1, std::memory_order_acq_rel);
}

void RSTunnelRuntimeState::SetNotTunnel()
{
    tunnelLayerId_.store(0, std::memory_order_release);
    tunnelLayerProperty_.store(TUNNEL_PROP_INVALID, std::memory_order_release);
    phase_.store(Phase::TUNNEL_IDLE, std::memory_order_release);
    pendingParam_.store(false, std::memory_order_release);
    tunnelState_.store(TunnelState::NOT_TUNNEL, std::memory_order_release);
}

bool RSTunnelRuntimeState::SetActiveFromTunnelLayerAvailable(uint64_t tunnelLayerGeneration)
{
    if (!IsCurrentTunnelLayerGeneration(tunnelLayerGeneration)) {
        RS_LOGD_IF(DEBUG_TUNNEL, "TUNNEL_DEBUG %{public}s rejected, "
            "reason:generation mismatch, "
            "incoming:%{public}" PRIu64 ", current:%{public}" PRIu64,
            __func__, tunnelLayerGeneration,
            tunnelLayerGeneration_.load(std::memory_order_acquire));
        RS_OPTIONAL_TRACE_NAME_FMT("TUNNEL_DEBUG %s rejected, reason:generation mismatch, "
            "incoming:%" PRIu64 ", current:%" PRIu64,
            __func__, tunnelLayerGeneration,
            tunnelLayerGeneration_.load(std::memory_order_acquire));
        return false;
    }
    TunnelState expected = TunnelState::BUILDING;
    if (tunnelState_.compare_exchange_strong(
        expected, TunnelState::ACTIVE, std::memory_order_acq_rel, std::memory_order_acquire)) {
        RS_TRACE_NAME_FMT("TUNNEL_DEBUG %s BUILDING -> ACTIVE, generation:%" PRIu64,
            __func__, tunnelLayerGeneration);
        RS_LOGI("TUNNEL_DEBUG %{public}s BUILDING -> ACTIVE, generation:%{public}" PRIu64,
            __func__, tunnelLayerGeneration);
        return true;
    }
    return false;
}

bool RSTunnelRuntimeState::IsTunnelDirectAllowed() const
{
    return GetTunnelState() == TunnelState::ACTIVE;
}

uint64_t RSTunnelRuntimeState::GetTunnelLayerGeneration() const
{
    return tunnelLayerGeneration_.load(std::memory_order_acquire);
}

bool RSTunnelRuntimeState::IsCurrentTunnelLayerGeneration(uint64_t tunnelLayerGeneration) const
{
    return tunnelLayerGeneration != 0 &&
        tunnelLayerGeneration == tunnelLayerGeneration_.load(std::memory_order_acquire);
}

void RSTunnelRuntimeState::DisableTunnelLayer()
{
    RS_LOGD_IF(DEBUG_TUNNEL, "TUNNEL_DEBUG %{public}s disable tunnelId:%{public}" PRIu64
        " tunnelLayerProperty:%{public}u", __func__, tunnelLayerId_.load(std::memory_order_acquire),
        tunnelLayerProperty_.load(std::memory_order_acquire));
    RS_TRACE_NAME_FMT("TUNNEL_DEBUG %s disable tunnelId:%" PRIu64 "tunnelLayerProperty:%u",
         __func__, tunnelLayerId_.load(std::memory_order_acquire),
        tunnelLayerProperty_.load(std::memory_order_acquire));
    SetNotTunnel();
    ClearCommittedTunnelBuffer();
}

void RSTunnelRuntimeState::ResetStateKeepLayerInfo()
{
    phase_.store(Phase::TUNNEL_IDLE, std::memory_order_release);
    pendingParam_.store(false, std::memory_order_release);
    tunnelState_.store(TunnelState::NOT_TUNNEL, std::memory_order_release);
    ClearCommittedTunnelBuffer();
}

void RSTunnelRuntimeState::SetCommittedTunnelBufferId(uint64_t bufferId)
{
    committedTunnelBufferId_.store(bufferId, std::memory_order_release);
}

bool RSTunnelRuntimeState::IsCommittedTunnelBuffer() const
{
    return committedTunnelBufferId_.load(std::memory_order_acquire) != 0;
}

void RSTunnelRuntimeState::ClearCommittedTunnelBuffer()
{
    committedTunnelBufferId_.store(0, std::memory_order_release);
}

RSTunnelRuntimeState::Phase RSTunnelRuntimeState::GetPhase() const
{
    return phase_.load(std::memory_order_acquire);
}

bool RSTunnelRuntimeState::IsPendingParam() const
{
    return pendingParam_.load(std::memory_order_acquire);
}

bool RSTunnelRuntimeState::TryClaimByListener()
{
    Phase claimedFrom = Phase::TUNNEL_IDLE;
    return TryClaimByListener(claimedFrom);
}

bool RSTunnelRuntimeState::TryClaimByListener(Phase& claimedFrom)
{
    if (pendingParam_.load(std::memory_order_acquire)) {
        return false;
    }
    Phase expected = phase_.load(std::memory_order_acquire);
    while (expected == Phase::TUNNEL_IDLE || expected == Phase::NORMAL_COMMITTED) {
        auto from = expected;
        if (phase_.compare_exchange_weak(expected, Phase::TUNNEL_INFLIGHT,
            std::memory_order_acq_rel, std::memory_order_acquire)) {
            claimedFrom = from;
            return true;
        }
    }
    return false;
}

bool RSTunnelRuntimeState::ReleaseByListener()
{
    phase_.store(Phase::TUNNEL_IDLE, std::memory_order_release);
    return pendingParam_.load(std::memory_order_acquire);
}

RSTunnelRuntimeState::ClaimResult RSTunnelRuntimeState::TryClaimByMain(bool needNormal)
{
    if (!needNormal) {
        Phase expected = Phase::NORMAL_COMMITTED;
        phase_.compare_exchange_strong(expected, Phase::TUNNEL_IDLE,
            std::memory_order_acq_rel, std::memory_order_acquire);
        return ClaimResult::KEEP_DIRECT;
    }
    Phase expected = phase_.load(std::memory_order_acquire);
    while (expected == Phase::TUNNEL_IDLE || expected == Phase::NORMAL_COMMITTED) {
        if (phase_.compare_exchange_weak(expected, Phase::NORMAL_PREPARING,
            std::memory_order_acq_rel, std::memory_order_acquire)) {
            pendingParam_.store(false, std::memory_order_release);
            ClearCommittedTunnelBuffer();
            return ClaimResult::GO_NORMAL;
        }
    }
    if (expected == Phase::TUNNEL_INFLIGHT) {
        pendingParam_.store(true, std::memory_order_release);
        return ClaimResult::KEEP_DIRECT;
    }
    if (expected == Phase::NORMAL_PREPARING) {
        pendingParam_.store(false, std::memory_order_release);
        ClearCommittedTunnelBuffer();
        return ClaimResult::GO_NORMAL;
    }
    return ClaimResult::KEEP_DIRECT;
}

void RSTunnelRuntimeState::OnRenderCommitDone()
{
    Phase expected = Phase::NORMAL_PREPARING;
    phase_.compare_exchange_strong(expected, Phase::NORMAL_COMMITTED,
        std::memory_order_acq_rel, std::memory_order_acquire);
}

void RSTunnelRuntimeState::AbandonNormalClaim()
{
    Phase expected = Phase::NORMAL_PREPARING;
    phase_.compare_exchange_strong(expected, Phase::TUNNEL_IDLE,
        std::memory_order_acq_rel, std::memory_order_acquire);
    pendingParam_.store(false, std::memory_order_release);
}

const RSTunnelRuntimeState::LastFrameRouteSnapshot& RSTunnelRuntimeState::GetLastFrameRouteSnapshot() const
{
    return lastFrameRouteSnapshot_;
}

void RSTunnelRuntimeState::UpdateLastFrameRouteSnapshot(const LastFrameRouteSnapshot& snapshot)
{
    lastFrameRouteSnapshot_ = snapshot;
}
} // namespace Rosen
} // namespace OHOS
