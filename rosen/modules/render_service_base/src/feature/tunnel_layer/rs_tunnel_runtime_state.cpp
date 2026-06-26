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

namespace OHOS {
namespace Rosen {
std::mutex RSTunnelRuntimeStore::mutex_;
std::unordered_map<NodeId, std::unique_ptr<RSTunnelRuntimeState>> RSTunnelRuntimeStore::tunnelRuntimeStates_;

RSTunnelRuntimeState::~RSTunnelRuntimeState() noexcept
{
    Clear();
}

void RSTunnelRuntimeState::SetLayerInfo(uint64_t tunnelLayerId, uint32_t property)
{
    std::lock_guard<std::mutex> lock(mutex_);
    tunnelLayerId_ = tunnelLayerId;
    tunnelLayerProperty_ = property;
}

void RSTunnelRuntimeState::GetLayerInfo(uint64_t& tunnelLayerId, uint32_t& property) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    tunnelLayerId = tunnelLayerId_;
    property = tunnelLayerProperty_;
}

RSTunnelRuntimeState::TunnelState RSTunnelRuntimeState::GetTunnelState() const
{
    return tunnelState_.load(std::memory_order_acquire);
}

void RSTunnelRuntimeState::SetBuilding()
{
    tunnelLayerGeneration_.fetch_add(1, std::memory_order_acq_rel);
    phase_.store(Phase::TUNNEL_IDLE, std::memory_order_release);
    pendingParam_.store(false, std::memory_order_release);
    tunnelState_.store(TunnelState::BUILDING, std::memory_order_release);
}

bool RSTunnelRuntimeState::SetActiveFromTunnelLayerAvailable(uint64_t tunnelLayerGeneration)
{
    if (!IsCurrentTunnelLayerGeneration(tunnelLayerGeneration)) {
        return false;
    }
    TunnelState expected = TunnelState::BUILDING;
    return tunnelState_.compare_exchange_strong(
        expected, TunnelState::ACTIVE, std::memory_order_acq_rel, std::memory_order_acquire);
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

void RSTunnelRuntimeState::Clear()
{
#ifndef ROSEN_CROSS_PLATFORM
    RSSurfaceHandler::SurfaceBufferEntry pendingBuffer;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        pendingBuffer = pendingBuffer_;
        pendingBuffer_ = RSSurfaceHandler::SurfaceBufferEntry();
        tunnelLayerId_ = 0;
        tunnelLayerProperty_ = TUNNEL_PROP_INVALID;
    }
    SetBuilding();
    ReleasePendingBuffer(pendingBuffer);
#else
    std::lock_guard<std::mutex> lock(mutex_);
    tunnelLayerId_ = 0;
    tunnelLayerProperty_ = TUNNEL_PROP_INVALID;
    SetBuilding();
#endif
    ClearCommittedTunnelBuffer();
}

#ifndef ROSEN_CROSS_PLATFORM
bool RSTunnelRuntimeState::IsPendingBufferValid(const RSSurfaceHandler::SurfaceBufferEntry& buffer)
{
    return buffer.buffer != nullptr && buffer.bufferOwnerCount_ != nullptr &&
        buffer.bufferOwnerCount_->bufferId_ != 0;
}

void RSTunnelRuntimeState::ReleasePendingBuffer(RSSurfaceHandler::SurfaceBufferEntry& buffer)
{
    if (!IsPendingBufferValid(buffer)) {
        return;
    }
    buffer.bufferOwnerCount_->DecRef();
    buffer = RSSurfaceHandler::SurfaceBufferEntry();
}

bool RSTunnelRuntimeState::HasPendingBuffer() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return IsPendingBufferValid(pendingBuffer_);
}

void RSTunnelRuntimeState::SetPendingBuffer(RSSurfaceHandler::SurfaceBufferEntry buffer)
{
    if (buffer.buffer != nullptr && buffer.bufferOwnerCount_ != nullptr) {
        buffer.bufferOwnerCount_->bufferId_ = buffer.buffer->GetBufferId();
    }
    if (!IsPendingBufferValid(buffer)) {
        return;
    }

    RSSurfaceHandler::SurfaceBufferEntry oldBuffer;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (IsPendingBufferValid(pendingBuffer_) &&
            pendingBuffer_.bufferOwnerCount_ != buffer.bufferOwnerCount_) {
            oldBuffer = pendingBuffer_;
        }
        pendingBuffer_ = buffer;
    }
    ReleasePendingBuffer(oldBuffer);
}

bool RSTunnelRuntimeState::TakePendingBuffer(RSSurfaceHandler::SurfaceBufferEntry& buffer)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!IsPendingBufferValid(pendingBuffer_)) {
        return false;
    }
    buffer = pendingBuffer_;
    pendingBuffer_ = RSSurfaceHandler::SurfaceBufferEntry();
    return true;
}
#endif

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
            return ClaimResult::GO_NORMAL;
        }
    }
    // Listener owns the buffer this vsync; main must not consume. Mark intent-to-switch
    // via pendingParam_, listener's ReleaseByListener will RequestNextVSync on release.
    if (expected == Phase::TUNNEL_INFLIGHT) {
        pendingParam_.store(true, std::memory_order_release);
        return ClaimResult::KEEP_DIRECT;
    }
    // Already in NORMAL_PREPARING (previous frame's render commit not yet acked back to
    // main looper). Back-to-back normal prepare is safe: phase stays the same, listener
    // is still locked out, and an upcoming OnRenderCommitDone will advance to COMMITTED.
    if (expected == Phase::NORMAL_PREPARING) {
        pendingParam_.store(false, std::memory_order_release);
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

// Released by main when a GO_NORMAL claim cannot reach commit (e.g. AcquireBuffer failed
// because the consumer queue was drained by a prior tunnel direct path). Without this,
// phase would stay NORMAL_PREPARING; KEEP_DIRECT only relaxes NORMAL_COMMITTED, and the
// listener's TryClaimByListener rejects every non-IDLE/COMMITTED phase, deadlocking both
// consumption paths until the next commit-done that never arrives.
void RSTunnelRuntimeState::AbandonNormalClaim()
{
    Phase expected = Phase::NORMAL_PREPARING;
    phase_.compare_exchange_strong(expected, Phase::TUNNEL_IDLE,
        std::memory_order_acq_rel, std::memory_order_acquire);
    pendingParam_.store(false, std::memory_order_release);
}

#ifndef ROSEN_CROSS_PLATFORM
const RSTunnelRuntimeState::LastFrameRouteSnapshot& RSTunnelRuntimeState::GetLastFrameRouteSnapshot() const
{
    return lastFrameRouteSnapshot_;
}

void RSTunnelRuntimeState::UpdateLastFrameRouteSnapshot(const LastFrameRouteSnapshot& snapshot)
{
    lastFrameRouteSnapshot_ = snapshot;
}
#endif

RSTunnelRuntimeState& RSTunnelRuntimeStore::GetOrCreate(NodeId nodeId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto& tunnelRuntime = tunnelRuntimeStates_[nodeId];
    if (tunnelRuntime == nullptr) {
        tunnelRuntime = std::make_unique<RSTunnelRuntimeState>();
    }
    return *tunnelRuntime;
}

bool RSTunnelRuntimeState::IsBufferSizeChanged(const uint32_t bufferSize) const
{
#ifdef ROSEN_CROSS_PLATFORM
    return false;
#else
    std::lock_guard<std::mutex> lock(mutex_);
    if (!(pendingBuffer_.buffer)) {
        return false;
    }
    return bufferSize != pendingBuffer_.buffer->GetSize();
#endif
}

bool RSTunnelRuntimeStore::GetLayerInfoIfPresent(NodeId nodeId, uint64_t& tunnelLayerId, uint32_t& property)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto iter = tunnelRuntimeStates_.find(nodeId);
    if (iter == tunnelRuntimeStates_.end() || iter->second == nullptr) {
        tunnelLayerId = 0;
        property = TUNNEL_PROP_INVALID;
        return false;
    }
    iter->second->GetLayerInfo(tunnelLayerId, property);
    return true;
}

void RSTunnelRuntimeStore::GetLayerInfoOrDefault(NodeId nodeId, uint64_t& tunnelLayerId, uint32_t& property)
{
    (void)GetLayerInfoIfPresent(nodeId, tunnelLayerId, property);
}

void RSTunnelRuntimeStore::SetLayerInfo(NodeId nodeId, uint64_t tunnelLayerId, uint32_t property)
{
    GetOrCreate(nodeId).SetLayerInfo(tunnelLayerId, property);
}

uint64_t RSTunnelRuntimeStore::GetTunnelLayerGeneration(NodeId nodeId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto iter = tunnelRuntimeStates_.find(nodeId);
    if (iter == tunnelRuntimeStates_.end() || iter->second == nullptr) {
        return 0;
    }
    return iter->second->GetTunnelLayerGeneration();
}

bool RSTunnelRuntimeStore::HasPendingBuffer(NodeId nodeId)
{
#ifdef ROSEN_CROSS_PLATFORM
    return false;
#else
    std::lock_guard<std::mutex> lock(mutex_);
    auto iter = tunnelRuntimeStates_.find(nodeId);
    if (iter == tunnelRuntimeStates_.end() || iter->second == nullptr) {
        return false;
    }
    return iter->second->HasPendingBuffer();
#endif
}

bool RSTunnelRuntimeStore::IsTunnelActive(NodeId nodeId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto iter = tunnelRuntimeStates_.find(nodeId);
    if (iter == tunnelRuntimeStates_.end() || iter->second == nullptr) {
        return false;
    }
    return iter->second->GetTunnelState() == RSTunnelRuntimeState::TunnelState::ACTIVE;
}

void RSTunnelRuntimeStore::Clear(NodeId nodeId)
{
    RSTunnelRuntimeState* runtime = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto iter = tunnelRuntimeStates_.find(nodeId);
        if (iter == tunnelRuntimeStates_.end() || iter->second == nullptr) {
            return;
        }
        runtime = iter->second.get();
    }
    runtime->Clear();
}

void RSTunnelRuntimeStore::Erase(NodeId nodeId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    tunnelRuntimeStates_.erase(nodeId);
}
} // namespace Rosen
} // namespace OHOS
