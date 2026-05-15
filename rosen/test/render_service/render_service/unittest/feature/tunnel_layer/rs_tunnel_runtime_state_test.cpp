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

#include "gtest/gtest.h"

#ifndef ROSEN_CROSS_PLATFORM
#include <iconsumer_surface.h>
#include <surface.h>
#endif

#include "pipeline/rs_tunnel_runtime_state.h"
#include "surface_buffer_impl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr int32_t TEST_BUFFER_SIZE = 0x100;
constexpr int32_t TEST_STRIDE_ALIGNMENT = 0x8;
constexpr int64_t FIRST_TIMESTAMP = 100;
constexpr int64_t SECOND_TIMESTAMP = 200;
constexpr uint64_t TEST_TUNNEL_LAYER_ID = 1001;
constexpr uint32_t TEST_TUNNEL_PROPERTY = TUNNEL_PROP_BUFFER_ADDR;
constexpr int64_t CONSUME_TIMESTAMP = 0;
constexpr uint64_t FIRST_BUFFER_ID = 11;
constexpr uint64_t SECOND_BUFFER_ID = 12;
} // namespace

class RSTunnelRuntimeStateTest : public testing::Test {
public:
    struct BufferEntry {
        sptr<SurfaceBuffer> buffer = nullptr;
        sptr<SyncFence> fence = SyncFence::InvalidFence();
    };

    BufferEntry RequestAndFlushBuffer();
#ifndef ROSEN_CROSS_PLATFORM
    RSSurfaceHandler::SurfaceBufferEntry AcquirePendingEntry(int64_t timestamp);
#endif

    void SetUp() override;
    void TearDown() override {}

private:
#ifndef ROSEN_CROSS_PLATFORM
    sptr<IConsumerSurface> consumerSurface_;
    sptr<IBufferProducer> bufferProducer_;
    sptr<Surface> surface_;
    static inline BufferRequestConfig requestConfig_ = {
        .width = TEST_BUFFER_SIZE,
        .height = TEST_BUFFER_SIZE,
        .strideAlignment = TEST_STRIDE_ALIGNMENT,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };
    static inline BufferFlushConfig flushConfig_ = {
        .damage = { .w = TEST_BUFFER_SIZE, .h = TEST_BUFFER_SIZE, },
    };
#endif
};

void RSTunnelRuntimeStateTest::SetUp()
{
#ifndef ROSEN_CROSS_PLATFORM
    consumerSurface_ = IConsumerSurface::Create();
    ASSERT_NE(consumerSurface_, nullptr);
    bufferProducer_ = consumerSurface_->GetProducer();
    ASSERT_NE(bufferProducer_, nullptr);
    surface_ = Surface::CreateSurfaceAsProducer(bufferProducer_);
    ASSERT_NE(surface_, nullptr);
#endif
}

RSTunnelRuntimeStateTest::BufferEntry RSTunnelRuntimeStateTest::RequestAndFlushBuffer()
{
    BufferEntry entry;
#ifndef ROSEN_CROSS_PLATFORM
    if (surface_ == nullptr) {
        return entry;
    }

    sptr<SyncFence> requestFence = SyncFence::InvalidFence();
    auto ret = surface_->RequestBuffer(entry.buffer, requestFence, requestConfig_);
    EXPECT_EQ(ret, GSERROR_OK);
    if (ret != GSERROR_OK || entry.buffer == nullptr) {
        entry.buffer = nullptr;
        return entry;
    }
    sptr<SyncFence> flushFence = SyncFence::InvalidFence();
    ret = surface_->FlushBuffer(entry.buffer, flushFence, flushConfig_);
    EXPECT_EQ(ret, GSERROR_OK);
    entry.fence = flushFence;
#endif
    return entry;
}

#ifndef ROSEN_CROSS_PLATFORM
RSSurfaceHandler::SurfaceBufferEntry RSTunnelRuntimeStateTest::AcquirePendingEntry(int64_t timestamp)
{
    RSSurfaceHandler::SurfaceBufferEntry entry;
    RequestAndFlushBuffer();
    IConsumerSurface::AcquireBufferReturnValue returnValue;
    if (consumerSurface_->AcquireBuffer(returnValue, CONSUME_TIMESTAMP, false) != SURFACE_ERROR_OK ||
        returnValue.buffer == nullptr) {
        return entry;
    }
    entry.buffer = returnValue.buffer;
    entry.acquireFence = returnValue.fence;
    entry.timestamp = timestamp;
    entry.damageRect = { 0, 0, TEST_BUFFER_SIZE, TEST_BUFFER_SIZE };
    entry.bufferOwnerCount_->bufferId_ = entry.buffer->GetBufferId();
    entry.RegisterReleaseBufferListener([consumer = consumerSurface_, buffer = entry.buffer,
        fence = entry.acquireFence](uint64_t) {
        if (consumer != nullptr) {
            auto releaseBuffer = buffer;
            consumer->ReleaseBuffer(releaseBuffer, fence);
        }
    });
    return entry;
}

/**
 * @tc.name: PendingBufferEntryLifecycle
 * @tc.desc: test pending SurfaceBufferEntry can be set and taken
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSTunnelRuntimeStateTest, PendingBufferEntryLifecycle, TestSize.Level1)
{
    RSTunnelRuntimeState state;
    auto entry = AcquirePendingEntry(FIRST_TIMESTAMP);
    ASSERT_NE(entry.buffer, nullptr);
    auto bufferId = entry.buffer->GetBufferId();

    state.SetPendingBuffer(entry);
    EXPECT_TRUE(state.HasPendingBuffer());

    RSSurfaceHandler::SurfaceBufferEntry takenEntry;
    ASSERT_TRUE(state.TakePendingBuffer(takenEntry));
    ASSERT_NE(takenEntry.buffer, nullptr);
    EXPECT_EQ(takenEntry.buffer->GetBufferId(), bufferId);
    EXPECT_EQ(takenEntry.timestamp, FIRST_TIMESTAMP);
    EXPECT_FALSE(state.HasPendingBuffer());
}

/**
 * @tc.name: SetPendingBufferReplacesOlderPending
 * @tc.desc: test setting a newer pending entry replaces the older one
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSTunnelRuntimeStateTest, SetPendingBufferReplacesOlderPending, TestSize.Level1)
{
    RSTunnelRuntimeState state;
    auto firstEntry = AcquirePendingEntry(FIRST_TIMESTAMP);
    auto secondEntry = AcquirePendingEntry(SECOND_TIMESTAMP);
    ASSERT_NE(firstEntry.buffer, nullptr);
    ASSERT_NE(secondEntry.buffer, nullptr);

    state.SetPendingBuffer(firstEntry);
    state.SetPendingBuffer(secondEntry);

    RSSurfaceHandler::SurfaceBufferEntry latestEntry;
    ASSERT_TRUE(state.TakePendingBuffer(latestEntry));
    ASSERT_NE(latestEntry.buffer, nullptr);
    EXPECT_EQ(latestEntry.buffer->GetBufferId(), secondEntry.buffer->GetBufferId());
    EXPECT_EQ(latestEntry.timestamp, SECOND_TIMESTAMP);
}
#endif

/**
 * @tc.name: ClearResetsBuffersAndActivationState
 * @tc.desc: test Clear resets layer info and activation state
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSTunnelRuntimeStateTest, ClearResetsBuffersAndActivationState, TestSize.Level1)
{
    RSTunnelRuntimeState state;
    state.SetLayerInfo(TEST_TUNNEL_LAYER_ID, TEST_TUNNEL_PROPERTY);
    state.SetBuilding();
    state.SetActiveFromTunnelLayerAvailable(state.GetTunnelLayerGeneration());
#ifndef ROSEN_CROSS_PLATFORM
    auto entry = AcquirePendingEntry(FIRST_TIMESTAMP);
    ASSERT_NE(entry.buffer, nullptr);
    state.SetPendingBuffer(entry);
#endif
    state.Clear();

#ifndef ROSEN_CROSS_PLATFORM
    EXPECT_FALSE(state.HasPendingBuffer());
#endif
    EXPECT_EQ(state.GetTunnelState(), RSTunnelRuntimeState::TunnelState::BUILDING);
    uint64_t tunnelLayerId = TEST_TUNNEL_LAYER_ID;
    uint32_t property = TEST_TUNNEL_PROPERTY;
    state.GetLayerInfo(tunnelLayerId, property);
    EXPECT_EQ(tunnelLayerId, 0u);
    EXPECT_EQ(property, TUNNEL_PROP_INVALID);
}

/**
 * @tc.name: TunnelLayerAvailableActivatesDirect
 * @tc.desc: test TunnelLayerAvailable promotes BUILDING to ACTIVE and duplicate callback is ignored
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSTunnelRuntimeStateTest, TunnelLayerAvailableActivatesDirect, TestSize.Level1)
{
    RSTunnelRuntimeState state;

    EXPECT_EQ(state.GetTunnelState(), RSTunnelRuntimeState::TunnelState::BUILDING);
    EXPECT_FALSE(state.IsTunnelDirectAllowed());

    uint64_t tunnelLayerGeneration = state.GetTunnelLayerGeneration();
    EXPECT_TRUE(state.SetActiveFromTunnelLayerAvailable(tunnelLayerGeneration));
    EXPECT_EQ(state.GetTunnelState(), RSTunnelRuntimeState::TunnelState::ACTIVE);
    EXPECT_TRUE(state.IsTunnelDirectAllowed());
    EXPECT_FALSE(state.SetActiveFromTunnelLayerAvailable(tunnelLayerGeneration));
    EXPECT_EQ(state.GetTunnelState(), RSTunnelRuntimeState::TunnelState::ACTIVE);
}

/**
 * @tc.name: TunnelStateIsInstanceScoped
 * @tc.desc: test tunnel activation state is isolated between runtime state instances
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSTunnelRuntimeStateTest, TunnelStateIsInstanceScoped, TestSize.Level1)
{
    RSTunnelRuntimeState activeState;
    RSTunnelRuntimeState buildingState;

    EXPECT_TRUE(activeState.SetActiveFromTunnelLayerAvailable(activeState.GetTunnelLayerGeneration()));

    EXPECT_EQ(activeState.GetTunnelState(), RSTunnelRuntimeState::TunnelState::ACTIVE);
    EXPECT_TRUE(activeState.IsTunnelDirectAllowed());
    EXPECT_EQ(buildingState.GetTunnelState(), RSTunnelRuntimeState::TunnelState::BUILDING);
    EXPECT_FALSE(buildingState.IsTunnelDirectAllowed());
}

/**
 * @tc.name: StaleTunnelLayerAvailableIsIgnored
 * @tc.desc: test stale layer-created generation cannot promote BUILDING to ACTIVE
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSTunnelRuntimeStateTest, StaleTunnelLayerAvailableIsIgnored, TestSize.Level1)
{
    RSTunnelRuntimeState state;
    uint64_t staleGeneration = state.GetTunnelLayerGeneration();

    state.SetBuilding();

    EXPECT_FALSE(state.SetActiveFromTunnelLayerAvailable(staleGeneration));
    EXPECT_EQ(state.GetTunnelState(), RSTunnelRuntimeState::TunnelState::BUILDING);
    EXPECT_FALSE(state.IsTunnelDirectAllowed());
    EXPECT_TRUE(state.SetActiveFromTunnelLayerAvailable(state.GetTunnelLayerGeneration()));
    EXPECT_TRUE(state.IsTunnelDirectAllowed());
}

/**
 * @tc.name: SetBuildingMovesActiveToStableBuilding
 * @tc.desc: test SetBuilding always moves ACTIVE to BUILDING before normal consume
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSTunnelRuntimeStateTest, SetBuildingMovesActiveToStableBuilding, TestSize.Level1)
{
    RSTunnelRuntimeState state;
    state.SetBuilding();
    state.SetActiveFromTunnelLayerAvailable(state.GetTunnelLayerGeneration());

    state.SetBuilding();
    EXPECT_EQ(state.GetTunnelState(), RSTunnelRuntimeState::TunnelState::BUILDING);
    EXPECT_FALSE(state.IsTunnelDirectAllowed());

    state.SetBuilding();
    EXPECT_EQ(state.GetTunnelState(), RSTunnelRuntimeState::TunnelState::BUILDING);
    EXPECT_FALSE(state.IsTunnelDirectAllowed());
}

/**
 * @tc.name: TryClaimByMainMovesIdleToNormalPreparing
 * @tc.desc: test needNormal=true claims NORMAL_PREPARING from TUNNEL_IDLE
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSTunnelRuntimeStateTest, TryClaimByMainMovesIdleToNormalPreparing, TestSize.Level1)
{
    RSTunnelRuntimeState state;
    EXPECT_EQ(state.GetPhase(), RSTunnelRuntimeState::Phase::TUNNEL_IDLE);

    auto result = state.TryClaimByMain(true);
    EXPECT_EQ(result, RSTunnelRuntimeState::ClaimResult::GO_NORMAL);
    EXPECT_EQ(state.GetPhase(), RSTunnelRuntimeState::Phase::NORMAL_PREPARING);
    EXPECT_FALSE(state.IsPendingParam());
}

/**
 * @tc.name: TryClaimByMainKeepsNormalPreparingLocked
 * @tc.desc: test needNormal=false does not relax NORMAL_PREPARING before commit done
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSTunnelRuntimeStateTest, TryClaimByMainKeepsNormalPreparingLocked, TestSize.Level1)
{
    RSTunnelRuntimeState state;
    ASSERT_EQ(state.TryClaimByMain(true), RSTunnelRuntimeState::ClaimResult::GO_NORMAL);
    ASSERT_EQ(state.GetPhase(), RSTunnelRuntimeState::Phase::NORMAL_PREPARING);

    auto result = state.TryClaimByMain(false);
    EXPECT_EQ(result, RSTunnelRuntimeState::ClaimResult::KEEP_DIRECT);
    EXPECT_EQ(state.GetPhase(), RSTunnelRuntimeState::Phase::NORMAL_PREPARING);
}

/**
 * @tc.name: OnRenderCommitDoneAdvancesOnlyNormalPreparing
 * @tc.desc: test commit done only advances NORMAL_PREPARING to NORMAL_COMMITTED
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSTunnelRuntimeStateTest, OnRenderCommitDoneAdvancesOnlyNormalPreparing, TestSize.Level1)
{
    RSTunnelRuntimeState state;
    state.OnRenderCommitDone();
    EXPECT_EQ(state.GetPhase(), RSTunnelRuntimeState::Phase::TUNNEL_IDLE);

    ASSERT_EQ(state.TryClaimByMain(true), RSTunnelRuntimeState::ClaimResult::GO_NORMAL);
    ASSERT_EQ(state.GetPhase(), RSTunnelRuntimeState::Phase::NORMAL_PREPARING);

    state.OnRenderCommitDone();
    EXPECT_EQ(state.GetPhase(), RSTunnelRuntimeState::Phase::NORMAL_COMMITTED);
    state.OnRenderCommitDone();
    EXPECT_EQ(state.GetPhase(), RSTunnelRuntimeState::Phase::NORMAL_COMMITTED);
}

/**
 * @tc.name: TryClaimByMainReusesNormalCommitted
 * @tc.desc: test claims can restart from NORMAL_COMMITTED and clear on KEEP_DIRECT
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSTunnelRuntimeStateTest, TryClaimByMainReusesNormalCommitted, TestSize.Level1)
{
    RSTunnelRuntimeState state;
    ASSERT_EQ(state.TryClaimByMain(true), RSTunnelRuntimeState::ClaimResult::GO_NORMAL);
    state.OnRenderCommitDone();
    ASSERT_EQ(state.GetPhase(), RSTunnelRuntimeState::Phase::NORMAL_COMMITTED);

    auto committedClaim = state.TryClaimByMain(true);
    EXPECT_EQ(committedClaim, RSTunnelRuntimeState::ClaimResult::GO_NORMAL);
    EXPECT_EQ(state.GetPhase(), RSTunnelRuntimeState::Phase::NORMAL_PREPARING);

    state.OnRenderCommitDone();
    ASSERT_EQ(state.GetPhase(), RSTunnelRuntimeState::Phase::NORMAL_COMMITTED);
    auto keepDirect = state.TryClaimByMain(false);
    EXPECT_EQ(keepDirect, RSTunnelRuntimeState::ClaimResult::KEEP_DIRECT);
    EXPECT_EQ(state.GetPhase(), RSTunnelRuntimeState::Phase::TUNNEL_IDLE);
}

/**
 * @tc.name: TryClaimByListenerReturnsClaimedPhase
 * @tc.desc: test listener claim reports whether the previous phase was direct or normal committed
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSTunnelRuntimeStateTest, TryClaimByListenerReturnsClaimedPhase, TestSize.Level1)
{
    RSTunnelRuntimeState state;
    auto claimedFrom = RSTunnelRuntimeState::Phase::NORMAL_PREPARING;
    EXPECT_TRUE(state.TryClaimByListener(claimedFrom));
    EXPECT_EQ(claimedFrom, RSTunnelRuntimeState::Phase::TUNNEL_IDLE);
    state.ReleaseByListener();

    ASSERT_EQ(state.TryClaimByMain(true), RSTunnelRuntimeState::ClaimResult::GO_NORMAL);
    state.OnRenderCommitDone();
    ASSERT_EQ(state.GetPhase(), RSTunnelRuntimeState::Phase::NORMAL_COMMITTED);

    claimedFrom = RSTunnelRuntimeState::Phase::TUNNEL_IDLE;
    EXPECT_TRUE(state.TryClaimByListener(claimedFrom));
    EXPECT_EQ(claimedFrom, RSTunnelRuntimeState::Phase::NORMAL_COMMITTED);
}

/**
 * @tc.name: CommittedTunnelBufferIdLifecycle
 * @tc.desc: test committed tunnel buffer id is tracked and cleared explicitly
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSTunnelRuntimeStateTest, CommittedTunnelBufferIdLifecycle, TestSize.Level1)
{
    RSTunnelRuntimeState state;
    EXPECT_FALSE(state.IsCommittedTunnelBuffer(FIRST_BUFFER_ID));

    state.SetCommittedTunnelBufferId(FIRST_BUFFER_ID);
    EXPECT_TRUE(state.IsCommittedTunnelBuffer(FIRST_BUFFER_ID));
    EXPECT_FALSE(state.IsCommittedTunnelBuffer(SECOND_BUFFER_ID));

    state.ClearCommittedTunnelBuffer();
    EXPECT_FALSE(state.IsCommittedTunnelBuffer(FIRST_BUFFER_ID));

    state.SetCommittedTunnelBufferId(SECOND_BUFFER_ID);
    state.Clear();
    EXPECT_FALSE(state.IsCommittedTunnelBuffer(SECOND_BUFFER_ID));
}

/**
 * @tc.name: AbandonNormalClaimReleasesNormalPreparing
 * @tc.desc: test AbandonNormalClaim relaxes NORMAL_PREPARING back to TUNNEL_IDLE so the
 *           listener can resume after a GO_NORMAL claim that failed to acquire a buffer
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSTunnelRuntimeStateTest, AbandonNormalClaimReleasesNormalPreparing, TestSize.Level1)
{
    RSTunnelRuntimeState state;
    ASSERT_EQ(state.TryClaimByMain(true), RSTunnelRuntimeState::ClaimResult::GO_NORMAL);
    ASSERT_EQ(state.GetPhase(), RSTunnelRuntimeState::Phase::NORMAL_PREPARING);

    state.AbandonNormalClaim();
    EXPECT_EQ(state.GetPhase(), RSTunnelRuntimeState::Phase::TUNNEL_IDLE);
    EXPECT_FALSE(state.IsPendingParam());
    EXPECT_TRUE(state.TryClaimByListener());
    EXPECT_EQ(state.GetPhase(), RSTunnelRuntimeState::Phase::TUNNEL_INFLIGHT);
}

/**
 * @tc.name: AbandonNormalClaimIsNoOpOutsideNormalPreparing
 * @tc.desc: test AbandonNormalClaim does not perturb TUNNEL_INFLIGHT or NORMAL_COMMITTED
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSTunnelRuntimeStateTest, AbandonNormalClaimIsNoOpOutsideNormalPreparing, TestSize.Level1)
{
    RSTunnelRuntimeState state;
    ASSERT_TRUE(state.TryClaimByListener());
    ASSERT_EQ(state.GetPhase(), RSTunnelRuntimeState::Phase::TUNNEL_INFLIGHT);
    state.AbandonNormalClaim();
    EXPECT_EQ(state.GetPhase(), RSTunnelRuntimeState::Phase::TUNNEL_INFLIGHT);
    EXPECT_FALSE(state.IsPendingParam());

    state.ReleaseByListener();
    ASSERT_EQ(state.TryClaimByMain(true), RSTunnelRuntimeState::ClaimResult::GO_NORMAL);
    state.OnRenderCommitDone();
    ASSERT_EQ(state.GetPhase(), RSTunnelRuntimeState::Phase::NORMAL_COMMITTED);
    state.AbandonNormalClaim();
    EXPECT_EQ(state.GetPhase(), RSTunnelRuntimeState::Phase::NORMAL_COMMITTED);
}

/**
 * @tc.name: AbandonNormalClaimClearsPendingParam
 * @tc.desc: test AbandonNormalClaim clears pendingParam so a stale switch-intent does not
 *           keep blocking the listener after main has given up the claim
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSTunnelRuntimeStateTest, AbandonNormalClaimClearsPendingParam, TestSize.Level1)
{
    RSTunnelRuntimeState state;
    ASSERT_TRUE(state.TryClaimByListener());
    ASSERT_EQ(state.TryClaimByMain(true), RSTunnelRuntimeState::ClaimResult::KEEP_DIRECT);
    ASSERT_TRUE(state.IsPendingParam());

    state.ReleaseByListener();
    ASSERT_EQ(state.TryClaimByMain(true), RSTunnelRuntimeState::ClaimResult::GO_NORMAL);
    ASSERT_EQ(state.GetPhase(), RSTunnelRuntimeState::Phase::NORMAL_PREPARING);

    state.AbandonNormalClaim();
    EXPECT_EQ(state.GetPhase(), RSTunnelRuntimeState::Phase::TUNNEL_IDLE);
    EXPECT_FALSE(state.IsPendingParam());
}

} // namespace OHOS::Rosen
