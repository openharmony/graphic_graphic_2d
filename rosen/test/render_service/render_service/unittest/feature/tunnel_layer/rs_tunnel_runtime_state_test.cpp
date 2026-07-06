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

#include "feature/tunnel_layer/rs_tunnel_runtime_state.h"

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
constexpr uint64_t FIRST_BUFFER_ID = 11;
constexpr uint64_t SECOND_BUFFER_ID = 12;
constexpr NodeId TEST_NODE_ID = TEST_TUNNEL_LAYER_ID + 1;
} // namespace

class RSTunnelRuntimeStateTest : public testing::Test {
public:
    struct BufferEntry {
        sptr<SurfaceBuffer> buffer = nullptr;
        sptr<SyncFence> fence = SyncFence::InvalidFence();
    };

    BufferEntry CreateBufferEntry();
#ifndef ROSEN_CROSS_PLATFORM
    RSSurfaceHandler::SurfaceBufferEntry AcquirePendingEntry(int64_t timestamp);
#endif
    void TearDown() override {}

private:
#ifndef ROSEN_CROSS_PLATFORM
    static inline BufferRequestConfig requestConfig_ = {
        .width = TEST_BUFFER_SIZE,
        .height = TEST_BUFFER_SIZE,
        .strideAlignment = TEST_STRIDE_ALIGNMENT,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };
#endif
};

RSTunnelRuntimeStateTest::BufferEntry RSTunnelRuntimeStateTest::CreateBufferEntry()
{
    BufferEntry entry;
#ifndef ROSEN_CROSS_PLATFORM
    entry.buffer = SurfaceBuffer::Create();
    if (entry.buffer == nullptr) {
        return entry;
    }

    auto ret = entry.buffer->Alloc(requestConfig_);
    EXPECT_EQ(ret, GSERROR_OK);
    if (ret != GSERROR_OK) {
        entry.buffer = nullptr;
        return entry;
    }
#endif
    return entry;
}

#ifndef ROSEN_CROSS_PLATFORM
RSSurfaceHandler::SurfaceBufferEntry RSTunnelRuntimeStateTest::AcquirePendingEntry(int64_t timestamp)
{
    RSSurfaceHandler::SurfaceBufferEntry entry;
    auto requestedEntry = CreateBufferEntry();
    if (requestedEntry.buffer == nullptr) {
        return entry;
    }
    entry.buffer = requestedEntry.buffer;
    entry.acquireFence = requestedEntry.fence;
    entry.timestamp = timestamp;
    entry.damageRect = { 0, 0, TEST_BUFFER_SIZE, TEST_BUFFER_SIZE };
    entry.bufferOwnerCount_->bufferId_ = entry.buffer->GetBufferId();
    entry.RegisterReleaseBufferListener([](uint64_t) {});
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
    EXPECT_FALSE(state.IsCommittedTunnelBuffer());

    state.SetCommittedTunnelBufferId(FIRST_BUFFER_ID);
    EXPECT_TRUE(state.IsCommittedTunnelBuffer());
    EXPECT_TRUE(state.IsCommittedTunnelBuffer());

    state.ClearCommittedTunnelBuffer();
    EXPECT_FALSE(state.IsCommittedTunnelBuffer());

    state.SetCommittedTunnelBufferId(SECOND_BUFFER_ID);
    state.Clear();
    EXPECT_FALSE(state.IsCommittedTunnelBuffer());
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

/**
 * @tc.name: RuntimeStoreNonCreatingQueries001
 * @tc.desc: test runtime store query APIs do not create default runtime entries
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSTunnelRuntimeStateTest, RuntimeStoreNonCreatingQueries001, TestSize.Level1)
{
    RSTunnelRuntimeStore::Erase(TEST_NODE_ID);

    uint64_t tunnelLayerId = 1;
    uint32_t property = TEST_TUNNEL_PROPERTY;
    RSTunnelRuntimeStore::GetLayerInfoOrDefault(TEST_NODE_ID, tunnelLayerId, property);
    EXPECT_EQ(tunnelLayerId, 0u);
    EXPECT_EQ(property, TUNNEL_PROP_INVALID);
    EXPECT_EQ(RSTunnelRuntimeStore::GetTunnelLayerGeneration(TEST_NODE_ID), 0u);
    EXPECT_FALSE(RSTunnelRuntimeStore::HasPendingBuffer(TEST_NODE_ID));
    EXPECT_FALSE(RSTunnelRuntimeStore::IsTunnelActive(TEST_NODE_ID));

    auto& runtime = RSTunnelRuntimeStore::GetOrCreate(TEST_NODE_ID);
    runtime.SetBuilding();
    ASSERT_TRUE(runtime.SetActiveFromTunnelLayerAvailable(runtime.GetTunnelLayerGeneration()));
    EXPECT_TRUE(RSTunnelRuntimeStore::IsTunnelActive(TEST_NODE_ID));

    RSTunnelRuntimeStore::Erase(TEST_NODE_ID);
}

/**
 * @tc.name: InstanceGetLayerInfoMatchesStore001
 * @tc.desc: test instance GetLayerInfo returns same values as SetLayerInfo
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelRuntimeStateTest, InstanceGetLayerInfoMatchesStore001, TestSize.Level1)
{
    RSTunnelRuntimeState state;
    constexpr uint64_t testTunnelLayerId = 6001;
    constexpr uint32_t testProperty = TUNNEL_PROP_BUFFER_ADDR | TUNNEL_PROP_DEVICE_COMMIT;
 
    state.SetLayerInfo(testTunnelLayerId, testProperty);
 
    uint64_t outTunnelLayerId = 0;
    uint32_t outProperty = TUNNEL_PROP_INVALID;
    state.GetLayerInfo(outTunnelLayerId, outProperty);
 
    EXPECT_EQ(outTunnelLayerId, testTunnelLayerId);
    EXPECT_EQ(outProperty, testProperty);
}
 
/**
 * @tc.name: InstanceGetLayerInfoAfterClear002
 * @tc.desc: test instance GetLayerInfo returns zeroed values after Clear
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelRuntimeStateTest, InstanceGetLayerInfoAfterClear002, TestSize.Level1)
{
    RSTunnelRuntimeState state;
    state.SetLayerInfo(TEST_TUNNEL_LAYER_ID, TEST_TUNNEL_PROPERTY);
    state.Clear();
 
    uint64_t outTunnelLayerId = TEST_TUNNEL_LAYER_ID;
    uint32_t outProperty = TEST_TUNNEL_PROPERTY;
    state.GetLayerInfo(outTunnelLayerId, outProperty);
 
    EXPECT_EQ(outTunnelLayerId, 0u);
    EXPECT_EQ(outProperty, TUNNEL_PROP_INVALID);
}
 
/**
 * @tc.name: InstanceGetTunnelLayerGeneration003
 * @tc.desc: test instance GetTunnelLayerGeneration returns consistent value
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelRuntimeStateTest, InstanceGetTunnelLayerGeneration003, TestSize.Level1)
{
    RSTunnelRuntimeState state;
    uint64_t gen1 = state.GetTunnelLayerGeneration();
    EXPECT_GE(gen1, 1u);
 
    state.SetBuilding();
    uint64_t gen2 = state.GetTunnelLayerGeneration();
    EXPECT_NE(gen1, gen2);
 
    uint64_t genAfterBuilding = state.GetTunnelLayerGeneration();
    state.SetActiveFromTunnelLayerAvailable(genAfterBuilding);
    uint64_t gen3 = state.GetTunnelLayerGeneration();
    EXPECT_EQ(genAfterBuilding, gen3);
}
 
/**
 * @tc.name: InstanceIsCurrentTunnelLayerGeneration004
 * @tc.desc: test instance IsCurrentTunnelLayerGeneration correctly identifies current vs stale generations
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelRuntimeStateTest, InstanceIsCurrentTunnelLayerGeneration004, TestSize.Level1)
{
    RSTunnelRuntimeState state;
    uint64_t currentGen = state.GetTunnelLayerGeneration();
 
    EXPECT_TRUE(state.IsCurrentTunnelLayerGeneration(currentGen));
    EXPECT_FALSE(state.IsCurrentTunnelLayerGeneration(currentGen + 1));
    EXPECT_FALSE(state.IsCurrentTunnelLayerGeneration(0));
}
 
/**
 * @tc.name: RuntimeStoreSetLayerInfoUsesInstanceMethod005
 * @tc.desc: test RSTunnelRuntimeStore::SetLayerInfo propagates to instance GetLayerInfo
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelRuntimeStateTest, RuntimeStoreSetLayerInfoUsesInstanceMethod005, TestSize.Level1)
{
    RSTunnelRuntimeStore::Erase(TEST_NODE_ID);
 
    constexpr uint64_t storeTunnelLayerId = 7001;
    constexpr uint32_t storeProperty = TUNNEL_PROP_BUFFER_ADDR | TUNNEL_PROP_RS_FORCE;
    RSTunnelRuntimeStore::SetLayerInfo(TEST_NODE_ID, storeTunnelLayerId, storeProperty);
 
    auto& runtime = RSTunnelRuntimeStore::GetOrCreate(TEST_NODE_ID);
    uint64_t outTunnelLayerId = 0;
    uint32_t outProperty = TUNNEL_PROP_INVALID;
    runtime.GetLayerInfo(outTunnelLayerId, outProperty);
 
    EXPECT_EQ(outTunnelLayerId, storeTunnelLayerId);
    EXPECT_EQ(outProperty, storeProperty);
 
    RSTunnelRuntimeStore::Erase(TEST_NODE_ID);
}

} // namespace OHOS::Rosen
