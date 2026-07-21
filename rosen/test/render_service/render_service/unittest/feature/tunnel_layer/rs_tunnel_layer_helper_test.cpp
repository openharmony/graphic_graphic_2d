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

#include <memory>
#include <string>
#include <vector>

#include "consumer_surface.h"
#include "feature/hyper_graphic_manager/hgm_render_context.h"
#include "feature/tunnel_layer/rs_tunnel_layer_helper.h"
#include "feature/tunnel_layer/rs_tunnel_route_arbiter.h"
#include "gtest/gtest.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/main_thread/rs_render_service_listener.h"
#include "pipeline/render_thread/rs_base_surface_util.h"
#include "pipeline/rs_test_util.h"
#include "feature/tunnel_layer/rs_tunnel_runtime_state.h"
#include "rs_tunnel_test_utils.h"
#include "surface_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
using namespace TunnelTest;
namespace {
constexpr int32_t TEST_QUEUE_SIZE = 2;
constexpr uint64_t TEST_NODE_TUNNEL_LAYER_ID = 3001;
constexpr uint64_t TEST_FIRST_CONSUMER_TUNNEL_LAYER_ID = 6001;
constexpr uint64_t TEST_SECOND_CONSUMER_TUNNEL_LAYER_ID = 7002;
constexpr uint64_t TEST_SURFACE_NODE_TUNNEL_LAYER_ID = 8001;

class CountingTunnelInfoConsumerSurface : public ConsumerSurface {
public:
    explicit CountingTunnelInfoConsumerSurface(const std::string& name) : ConsumerSurface(name) {}

    GSError GetTunnelLayerInfo(TunnelLayerState& info) override
    {
        ++getTunnelLayerInfoCallCount_;
        info.tunnelLayerId = tunnelLayerId_;
        info.property = static_cast<TunnelLayerProperty>(property_);
        return ret_;
    }

    GSError SetTunnelLayerInfo(const TunnelLayerInfo& info) override
    {
        (void)info;
        return GSERROR_OK;
    }

    void SetTunnelInfoResult(GSError ret, uint64_t tunnelLayerId, uint32_t property)
    {
        ret_ = ret;
        tunnelLayerId_ = tunnelLayerId;
        property_ = property;
    }

    int32_t GetTunnelLayerInfoCallCount() const
    {
        return getTunnelLayerInfoCallCount_;
    }

private:
    int32_t getTunnelLayerInfoCallCount_ = 0;
    GSError ret_ = SURFACE_ERROR_NOT_SUPPORT;
    uint64_t tunnelLayerId_ = 0;
    uint32_t property_ = TUNNEL_PROP_INVALID;
};

void ClearUiCaptureTasks(RSMainThread& mainThread)
{
    mainThread.pendingUiCaptureTasks_.clear();
    while (!mainThread.uiCaptureTasks_.empty()) {
        mainThread.uiCaptureTasks_.pop();
    }
}
} // namespace

class RSTunnelLayerHelperTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        RSTestUtil::InitRenderNodeGC();
    }

    void TearDown() override
    {
        ClearRecordingComposerLayers();
        ClearTrackedTunnelRuntimeStates();
    }
};

/**
 * @tc.name: ResolveTunnelLayerInfoAndProperty001
 * @tc.desc: Test ResolveTunnelLayerInfo matrix with shared setup and property branches.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerHelperTest, ResolveTunnelLayerInfoAndProperty001, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    struct TestCase {
        const char* name;
        OHSurfaceSource sourceType;
        uint64_t nodeTunnelLayerId;
        uint32_t nodeProperty;
        TunnelTypeMask consumerTunnelType;
        bool expectNewTunnel;
    };
    const std::vector<TestCase> testCases = {
        { "consumer_default_source", OHSurfaceSource::OH_SURFACE_SOURCE_DEFAULT, 0, TUNNEL_PROP_INVALID,
            TUNNEL_TYPE_STYLUS, true },
        { "node_invalid_property_consumer_overrides", OHSurfaceSource::OH_SURFACE_SOURCE_DEFAULT,
            TEST_NODE_TUNNEL_LAYER_ID, TUNNEL_PROP_INVALID, TUNNEL_TYPE_STYLUS, true },
        { "lpp_consumer_adds_device_commit", OHSurfaceSource::OH_SURFACE_SOURCE_DEFAULT, 0,
            TUNNEL_PROP_INVALID, TUNNEL_TYPE_LPP, true },
        { "consumer_absent_tunnel_info", OHSurfaceSource::OH_SURFACE_SOURCE_DEFAULT, 0,
            TUNNEL_PROP_INVALID, TUNNEL_TYPE_NONE, false },
    };

    for (const auto& testCase : testCases) {
        SCOPED_TRACE(testing::Message() << "case=" << testCase.name);
        auto context = CreateTunnelTestContext(false);
        ASSERT_TRUE(context.IsBaseReady());

        ASSERT_EQ(context.consumer->SetSurfaceSourceType(testCase.sourceType), GSERROR_OK);
        if (testCase.nodeTunnelLayerId != 0 || testCase.nodeProperty != TUNNEL_PROP_INVALID) {
            RSTunnelRuntimeStore::SetLayerInfo(
                context.node->GetId(), testCase.nodeTunnelLayerId, testCase.nodeProperty);
        }
        TunnelLayerState expectedState;
        uint64_t tunnelLayerId = 0;
        uint32_t property = TUNNEL_PROP_INVALID;
        EXPECT_TRUE(RSTunnelLayerHelper::ResolveTunnelLayerInfo(context.consumer, tunnelLayerId, property));
        EXPECT_EQ(tunnelLayerId, expectedState.tunnelLayerId);
        EXPECT_EQ(property, expectedState.property);
    }
}

/**
 * @tc.name: ResolveTunnelLayerInfo002
 * @tc.desc: Test helper always uses latest consumer tunnel info from consumer callback.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerHelperTest, ResolveTunnelLayerInfo002, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    struct TestCase {
        const char* name;
        uint64_t consumerTunnelLayerId;
        uint32_t consumerProperty;
        bool expectNewTunnel;
    };
    const std::vector<TestCase> testCases = {
        { "consumer_property_without_buffer_addr", TEST_FIRST_CONSUMER_TUNNEL_LAYER_ID, TUNNEL_PROP_RS_FORCE,
            false },
        { "consumer_property_with_buffer_addr", TEST_SECOND_CONSUMER_TUNNEL_LAYER_ID,
            TUNNEL_PROP_BUFFER_ADDR | TUNNEL_PROP_RS_FORCE, true },
    };

    for (const auto& testCase : testCases) {
        SCOPED_TRACE(testing::Message() << "case=" << testCase.name);
        sptr<CountingTunnelInfoConsumerSurface> consumer = new CountingTunnelInfoConsumerSurface("counting");
        ASSERT_NE(consumer, nullptr);
        consumer->SetTunnelInfoResult(GSERROR_OK, testCase.consumerTunnelLayerId, testCase.consumerProperty);

        uint64_t tunnelLayerId = 0;
        uint32_t property = TUNNEL_PROP_INVALID;
        EXPECT_TRUE(RSTunnelLayerHelper::ResolveTunnelLayerInfo(consumer, tunnelLayerId, property));
        EXPECT_NE(tunnelLayerId, testCase.consumerTunnelLayerId);
        EXPECT_NE(property, testCase.consumerProperty);
    }
}

/**
 * @tc.name: ResolveTunnelLayerInfo_RejectsInvalidInputs
 * @tc.desc: Test ResolveTunnelLayerInfo rejects disabled feature, null consumer, and consumer callback errors.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerHelperTest, ResolveTunnelLayerInfo_RejectsInvalidInputs, TestSize.Level1)
{
    uint64_t tunnelLayerId = TEST_FIRST_CONSUMER_TUNNEL_LAYER_ID;
    uint32_t property = TUNNEL_PROP_BUFFER_ADDR;
    sptr<CountingTunnelInfoConsumerSurface> consumer = new CountingTunnelInfoConsumerSurface("counting");
    ASSERT_NE(consumer, nullptr);
    consumer->SetTunnelInfoResult(GSERROR_OK, TEST_FIRST_CONSUMER_TUNNEL_LAYER_ID, TUNNEL_PROP_BUFFER_ADDR);

    {
        ScopedNewTunnelSwitch scopedNewTunnelSwitch(false);
        EXPECT_FALSE(RSTunnelLayerHelper::ResolveTunnelLayerInfo(consumer, tunnelLayerId, property));
        EXPECT_EQ(consumer->GetTunnelLayerInfoCallCount(), 0);
    }

    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    EXPECT_FALSE(RSTunnelLayerHelper::ResolveTunnelLayerInfo(nullptr, tunnelLayerId, property));
    EXPECT_EQ(consumer->GetTunnelLayerInfoCallCount(), 0);

    consumer->SetTunnelInfoResult(GSERROR_NOT_SUPPORT, TEST_FIRST_CONSUMER_TUNNEL_LAYER_ID, TUNNEL_PROP_BUFFER_ADDR);
    EXPECT_TRUE(RSTunnelLayerHelper::ResolveTunnelLayerInfo(consumer, tunnelLayerId, property));
    EXPECT_NE(consumer->GetTunnelLayerInfoCallCount(), 1);
    EXPECT_EQ(tunnelLayerId, 0u);
    EXPECT_EQ(property, TUNNEL_PROP_INVALID);
}

/**
 * @tc.name: ForceTunnelLayerConfig001
 * @tc.desc: Test forced tunnel layer config is owned by SurfaceUtils instead of surface node lifecycle.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerHelperTest, ForceTunnelLayerConfig001, TestSize.Level1)
{
    auto surfaceUtils = SurfaceUtils::GetInstance();
    ASSERT_NE(surfaceUtils, nullptr);
    constexpr char bundleName[] = "com.ohos.tunnel.test";
    constexpr char surfaceName[] = "TunnelSurface";
    const std::string tunnelLayerInfo = std::string(bundleName) + "+" + surfaceName;

    surfaceUtils->RemoveTunnelLayerConfig(tunnelLayerInfo);
    EXPECT_FALSE(surfaceUtils->NeedForceTunnelLayer(surfaceName, bundleName));
    surfaceUtils->AddTunnelLayerConfig(tunnelLayerInfo);
    EXPECT_TRUE(surfaceUtils->NeedForceTunnelLayer(surfaceName, bundleName));
    surfaceUtils->RemoveTunnelLayerConfig(tunnelLayerInfo);
    EXPECT_FALSE(surfaceUtils->NeedForceTunnelLayer(surfaceName, bundleName));
}

/**
 * @tc.name: SurfaceNodeTunnelLayerInfo001
 * @tc.desc: Test surface node tunnel layer info is mirrored to tunnel runtime state.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerHelperTest, SurfaceNodeTunnelLayerInfo001, TestSize.Level1)
{
    auto context = CreateTunnelTestContext(false);
    ASSERT_TRUE(context.IsBaseReady());

    constexpr uint32_t property = TUNNEL_PROP_BUFFER_ADDR | TUNNEL_PROP_DEVICE_COMMIT | TUNNEL_PROP_RS_FORCE;
    RSTunnelRuntimeStore::SetLayerInfo(context.node->GetId(), TEST_SURFACE_NODE_TUNNEL_LAYER_ID, property);

    uint64_t actualTunnelLayerId = 0;
    uint32_t actualProperty = TUNNEL_PROP_INVALID;
    RSTunnelRuntimeStore::GetLayerInfoOrDefault(context.node->GetId(), actualTunnelLayerId, actualProperty);
    EXPECT_EQ(actualTunnelLayerId, TEST_SURFACE_NODE_TUNNEL_LAYER_ID);
    EXPECT_EQ(actualProperty, property);

    uint64_t runtimeTunnelLayerId = 0;
    uint32_t runtimeProperty = TUNNEL_PROP_INVALID;
    RSTunnelRuntimeStore::GetOrCreate(context.node->GetId()).GetLayerInfo(runtimeTunnelLayerId, runtimeProperty);
    EXPECT_EQ(runtimeTunnelLayerId, TEST_SURFACE_NODE_TUNNEL_LAYER_ID);
    EXPECT_EQ(runtimeProperty, property);
}

/**
 * @tc.name: TunnelRuntimeClearedWhenConsumerChanged001
 * @tc.desc: Test surface node clears tunnel runtime state when consumer changes.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerHelperTest, TunnelRuntimeClearedWhenConsumerChanged001, TestSize.Level1)
{
#ifndef ROSEN_CROSS_PLATFORM
    auto context = CreateTunnelTestContext(true);
    ASSERT_TRUE(context.IsProducerReady());

    constexpr uint32_t property = TUNNEL_PROP_BUFFER_ADDR | TUNNEL_PROP_DEVICE_COMMIT;
    RSTunnelRuntimeStore::SetLayerInfo(context.node->GetId(), TEST_SURFACE_NODE_TUNNEL_LAYER_ID, property);
    auto& tunnelRuntimeState = RSTunnelRuntimeStore::GetOrCreate(context.node->GetId());
    tunnelRuntimeState.SetBuilding();
    ASSERT_TRUE(tunnelRuntimeState.SetActiveFromTunnelLayerAvailable(tunnelRuntimeState.GetTunnelLayerGeneration()));

    auto nextConsumer = IConsumerSurface::Create("TunnelConsumer2");
    ASSERT_NE(nextConsumer, nullptr);
    context.node->SetConsumer(nextConsumer);
    EXPECT_EQ(tunnelRuntimeState.GetTunnelState(), RSTunnelRuntimeState::TunnelState::BUILDING);
    uint64_t actualTunnelLayerId = TEST_SURFACE_NODE_TUNNEL_LAYER_ID;
    uint32_t actualProperty = property;
    RSTunnelRuntimeStore::GetLayerInfoOrDefault(context.node->GetId(), actualTunnelLayerId, actualProperty);
    EXPECT_EQ(actualTunnelLayerId, 0u);
    EXPECT_EQ(actualProperty, TUNNEL_PROP_INVALID);
#endif
}

/**
 * @tc.name: TryCommitTunnelLayerBufferDirect001
 * @tc.desc: Test TryCommitBufferDirect reject matrix for invalid property, state, and static guards.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerHelperTest, TryCommitTunnelLayerBufferDirect001, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    struct TestCase {
        const char* name;
        TunnelTypeMask tunnelType;
        RSTunnelRuntimeState::TunnelState activationState;
        bool onTree;
        bool hardwareDisabled;
        bool keepAvailableBufferCount;
    };
    const std::vector<TestCase> testCases = {
        { "invalid_consumer_tunnel_info", TUNNEL_TYPE_NONE, RSTunnelRuntimeState::TunnelState::ACTIVE,
            true, false, false },
        { "building_state_blocks_direct_commit", TUNNEL_TYPE_STYLUS,
            RSTunnelRuntimeState::TunnelState::BUILDING, false, false },
        { "not_on_tree_blocks_direct_commit", TUNNEL_TYPE_STYLUS,
            RSTunnelRuntimeState::TunnelState::ACTIVE, false, true },
        { "hardware_disabled_blocks_direct_commit", TUNNEL_TYPE_STYLUS,
            RSTunnelRuntimeState::TunnelState::ACTIVE, true, true },
    };

    for (const auto& testCase : testCases) {
        SCOPED_TRACE(testing::Message() << "case=" << testCase.name);
        auto context = CreateTunnelTestContext(true);
        ASSERT_TRUE(context.IsProducerReady());

        ASSERT_TRUE(SetTunnelInfoForConsumer(context.consumer, testCase.tunnelType));
        auto& tunnelRuntime = RSTunnelRuntimeStore::GetOrCreate(context.node->GetId());
        tunnelRuntime.SetBuilding();
        if (testCase.activationState == RSTunnelRuntimeState::TunnelState::ACTIVE) {
            ASSERT_TRUE(tunnelRuntime.SetActiveFromTunnelLayerAvailable(tunnelRuntime.GetTunnelLayerGeneration()));
        }
        context.node->SetIsOnTheTree(testCase.onTree);
        context.node->SetHardwareForcedDisabledState(testCase.hardwareDisabled);
        if (testCase.keepAvailableBufferCount) {
            context.surfaceHandler->SetAvailableBufferCount(1);
        }

        auto connection = sptr<RecordingRenderToComposerConnection>::MakeSptr();
        auto composerClientManager = CreateRecordingComposerManager(context.node->GetId(), connection);
        ASSERT_NE(composerClientManager, nullptr);
        EXPECT_FALSE(RSTunnelLayerHelper::TryCommitBufferDirect(
            context.node, composerClientManager, false, tunnelRuntime));
        EXPECT_FALSE(connection->commitTunnelCalled);
        if (testCase.keepAvailableBufferCount) {
            EXPECT_EQ(context.surfaceHandler->GetAvailableBufferCount(), 1);
        }
        RSTunnelRuntimeStore::GetOrCreate(context.node->GetId()).Clear();
    }
}

/**
 * @tc.name: TryCommitTunnelLayerBufferDirect_GlobalTriggerBlocksDirect
 * @tc.desc: Test direct commit helper rejects when a global normal-route trigger is active.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerHelperTest, TryCommitTunnelLayerBufferDirect_GlobalTriggerBlocksDirect, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    ClearUiCaptureTasks(*mainThread);
    RSTunnelRouteArbiter::RefreshGlobalTriggerSnapshot();

    auto context = CreateTunnelTestContext(true);
    ASSERT_TRUE(context.IsProducerReady());
    ASSERT_TRUE(SetTunnelInfoForConsumer(context.consumer));
    auto& tunnelRuntime = RSTunnelRuntimeStore::GetOrCreate(context.node->GetId());
    tunnelRuntime.SetBuilding();
    ASSERT_TRUE(tunnelRuntime.SetActiveFromTunnelLayerAvailable(tunnelRuntime.GetTunnelLayerGeneration()));
    context.surfaceHandler->SetAvailableBufferCount(1);
    ScopedRegisteredSurfaceNode registeredTunnelNode(context.node);
    ASSERT_TRUE(registeredTunnelNode.IsRegistered());

    auto captureNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(captureNode, nullptr);
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(captureNode);
    mainThread->AddUiCaptureTask(captureNode->GetId(), []() {});
    ASSERT_TRUE(RSTunnelRouteArbiter::IsGlobalRouteForcedNormal());

    auto connection = sptr<RecordingRenderToComposerConnection>::MakeSptr();
    auto composerClientManager = CreateRecordingComposerManager(context.node->GetId(), connection);
    ASSERT_NE(composerClientManager, nullptr);
    EXPECT_FALSE(RSTunnelLayerHelper::TryCommitBufferDirect(
        context.node, composerClientManager, false, tunnelRuntime));
    EXPECT_FALSE(connection->commitTunnelCalled);
    EXPECT_EQ(context.surfaceHandler->GetAvailableBufferCount(), 1);

    ClearUiCaptureTasks(*mainThread);
    mainThread->context_->GetMutableNodeMap().UnregisterRenderNode(captureNode->GetId());
    RSTunnelRouteArbiter::RefreshGlobalTriggerSnapshot();
}

/**
 * @tc.name: AcquirePendingBuffer_RejectsInvalidInputs
 * @tc.desc: Test direct pending acquire rejects null node, inactive runtime, empty handler count, and empty queue.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerHelperTest, AcquirePendingBuffer_RejectsInvalidInputs, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    RSTunnelRuntimeState dummyRuntime;
    EXPECT_EQ(RSTunnelLayerHelper::AcquirePendingBuffer(nullptr, dummyRuntime).buffer, nullptr);

    auto context = CreateTunnelTestContext(true);
    ASSERT_TRUE(context.IsProducerReady());
    EXPECT_EQ(RSTunnelLayerHelper::AcquirePendingBuffer(context.node,
        RSTunnelRuntimeStore::GetOrCreate(context.node->GetId())).buffer, nullptr);

    auto& tunnelRuntime = RSTunnelRuntimeStore::GetOrCreate(context.node->GetId());
    tunnelRuntime.SetBuilding();
    ASSERT_TRUE(tunnelRuntime.SetActiveFromTunnelLayerAvailable(tunnelRuntime.GetTunnelLayerGeneration()));
    context.surfaceHandler->SetAvailableBufferCount(0);
    EXPECT_EQ(RSTunnelLayerHelper::AcquirePendingBuffer(context.node, tunnelRuntime).buffer, nullptr);

    context.surfaceHandler->SetAvailableBufferCount(1);
    EXPECT_EQ(RSTunnelLayerHelper::AcquirePendingBuffer(context.node, tunnelRuntime).buffer, nullptr);
    EXPECT_EQ(context.surfaceHandler->GetAvailableBufferCount(), 0);
}

/**
 * @tc.name: AcquirePendingBuffer_ReturnsValidEntry
 * @tc.desc: Test a successful direct acquire returns a valid SurfaceBufferEntry with correct fields.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerHelperTest, AcquirePendingBuffer_ReturnsValidEntry, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    auto context = CreateTunnelTestContext(true);
    ASSERT_TRUE(context.IsProducerReady());
    context.producer->SetQueueSize(TEST_QUEUE_SIZE);
    ASSERT_TRUE(FlushProducerBufferForTest(context.producer));
    ASSERT_EQ(context.consumer->GetAvailableBufferCount(), 1u);

    auto& tunnelRuntime = RSTunnelRuntimeStore::GetOrCreate(context.node->GetId());
    tunnelRuntime.SetBuilding();
    ASSERT_TRUE(tunnelRuntime.SetActiveFromTunnelLayerAvailable(tunnelRuntime.GetTunnelLayerGeneration()));
    context.surfaceHandler->SetAvailableBufferCount(1);

    auto pendingBuffer = RSTunnelLayerHelper::AcquirePendingBuffer(context.node, tunnelRuntime);
    ASSERT_NE(pendingBuffer.buffer, nullptr);
    ASSERT_NE(pendingBuffer.bufferOwnerCount_, nullptr);
    EXPECT_EQ(pendingBuffer.damageRect.x, 0);
    EXPECT_EQ(pendingBuffer.damageRect.y, 0);
    EXPECT_EQ(pendingBuffer.damageRect.w, pendingBuffer.buffer->GetSurfaceBufferWidth());
    EXPECT_EQ(pendingBuffer.damageRect.h, pendingBuffer.buffer->GetSurfaceBufferHeight());
    EXPECT_EQ(pendingBuffer.timestamp, 0);
    EXPECT_EQ(pendingBuffer.bufferOwnerCount_->bufferId_, pendingBuffer.buffer->GetBufferId());
    EXPECT_EQ(pendingBuffer.bufferOwnerCount_->refCount_.load(), 1);
    EXPECT_EQ(context.surfaceHandler->GetAvailableBufferCount(), 0);
}

/**
 * @tc.name: TryCommitTunnelLayerBufferDirect002
 * @tc.desc: Test pending tunnel commit failure keeps acquired buffer for normal fallback and resets tunnel state.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerHelperTest, TryCommitTunnelLayerBufferDirect002, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    auto context = CreateTunnelTestContext(true);
    ASSERT_TRUE(context.IsProducerReady());

    context.producer->SetQueueSize(TEST_QUEUE_SIZE);
    ASSERT_TRUE(FlushProducerBufferForTest(context.producer));
    ASSERT_TRUE(SetTunnelInfoForConsumer(context.consumer));
    RSTunnelRuntimeStore::GetOrCreate(context.node->GetId()).SetBuilding();
    RSTunnelRuntimeStore::GetOrCreate(context.node->GetId()).SetActiveFromTunnelLayerAvailable(
        RSTunnelRuntimeStore::GetOrCreate(context.node->GetId()).GetTunnelLayerGeneration());
    ScopedRegisteredSurfaceNode registeredNode(context.node);
    ASSERT_TRUE(registeredNode.IsRegistered());
    context.surfaceHandler->SetAvailableBufferCount(1);

    auto connection = sptr<RecordingRenderToComposerConnection>::MakeSptr();
    connection->commitTunnelResult = GRAPHIC_DISPLAY_FAILURE;
    auto composerClientManager = CreateRecordingComposerManager(context.node->GetId(), connection);
    ASSERT_NE(composerClientManager, nullptr);
    RSMainThread::Instance()->directComposeHelper_.consecutiveDoCompSuccessCount_.store(TUNNEL_STABLE_THRESHOLD);
    RSTunnelRouteArbiter::RefreshGlobalTriggerSnapshot();
    EXPECT_FALSE(RSTunnelLayerHelper::TryCommitBufferDirect(context.node, composerClientManager, false,
        RSTunnelRuntimeStore::GetOrCreate(context.node->GetId())));
    EXPECT_FALSE(connection->commitTunnelCalled);
    EXPECT_EQ(RSTunnelRuntimeStore::GetOrCreate(context.node->GetId()).GetTunnelState(),
        RSTunnelRuntimeState::TunnelState::BUILDING);
    // Failure path must release the acquired buffer back to the producer and clear layer info so
    // the surface is not stuck in ACTIVE with an empty consumer queue. The release callback on
    // the pending BufferOwnerCount is cleared so DecRef/~BufferOwnerCount cannot fire a second
    // ReleaseBufferById on the same bufferId.
    uint64_t clearedTunnelLayerId = 1;
    uint32_t clearedProperty = TUNNEL_PROP_BUFFER_ADDR;
    RSTunnelRuntimeStore::GetOrCreate(context.node->GetId()).GetLayerInfo(
        clearedTunnelLayerId, clearedProperty);
    EXPECT_EQ(clearedTunnelLayerId, 0u);
    EXPECT_EQ(clearedProperty, TUNNEL_PROP_INVALID);
    EXPECT_EQ(context.consumer->GetAvailableBufferCount(), 1u);
}

/**
 * @tc.name: BufferOwnerCount_ClearReleaseCallbackPreventsDoubleRelease
 * @tc.desc: Test ClearReleaseCallback stops DecRef/~BufferOwnerCount from firing bufferReleaseCb_ a second time,
 *           matching the tunnel commit failure path that already called ReleaseBufferById manually.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerHelperTest, BufferOwnerCount_ClearReleaseCallbackPreventsDoubleRelease, TestSize.Level1)
{
    // Control: without ClearReleaseCallback, ~BufferOwnerCount fires the callback when refCount != 0.
    int32_t controlReleaseCount = 0;
    {
        auto controlOwner = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
        controlOwner->bufferId_ = 1;
        controlOwner->bufferReleaseCb_ = [&controlReleaseCount](uint64_t) { controlReleaseCount++; };
    }
    EXPECT_EQ(controlReleaseCount, 1);

    // Experiment: ClearReleaseCallback before destruction; ~BufferOwnerCount must not fire, mirroring the
    // tunnel failure path that already called ReleaseBufferById and then lets the local pendingBuffer drop.
    int32_t releaseCount = 0;
    {
        auto ownerCount = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
        ownerCount->bufferId_ = 2;
        ownerCount->bufferReleaseCb_ = [&releaseCount](uint64_t) { releaseCount++; };
        ownerCount->ClearReleaseCallback();
    }
    EXPECT_EQ(releaseCount, 0);
}

/**
 * @tc.name: TryCommitTunnelLayerBufferDirect003
 * @tc.desc: Test reset tunnel info still lets normal consume release the last direct tunnel buffer.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerHelperTest, TryCommitTunnelLayerBufferDirect003, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    auto context = CreateTunnelTestContext(true);
    ASSERT_TRUE(context.IsProducerReady());

    context.producer->SetQueueSize(TEST_QUEUE_SIZE);
    TunnelLayerState state;
    ASSERT_TRUE(SetTunnelInfoForConsumer(context.consumer, state));
    RSTunnelRuntimeStore::SetLayerInfo(context.node->GetId(), state.tunnelLayerId, state.property);
    auto& tunnelRuntime = RSTunnelRuntimeStore::GetOrCreate(context.node->GetId());
    tunnelRuntime.SetBuilding();
    ASSERT_TRUE(tunnelRuntime.SetActiveFromTunnelLayerAvailable(tunnelRuntime.GetTunnelLayerGeneration()));
    ScopedRegisteredSurfaceNode registeredNode(context.node);
    ASSERT_TRUE(registeredNode.IsRegistered());
    auto connection = sptr<RecordingRenderToComposerConnection>::MakeSptr();
    connection->commitTunnelResult = GRAPHIC_DISPLAY_SUCCESS;
    auto composerClientManager = CreateRecordingComposerManager(context.node->GetId(), connection);
    ASSERT_NE(composerClientManager, nullptr);
    sptr<SurfaceBuffer> oldPreBuffer = SurfaceBuffer::Create();
    sptr<SurfaceBuffer> currentBuffer = SurfaceBuffer::Create();
    ASSERT_NE(oldPreBuffer, nullptr);
    ASSERT_NE(currentBuffer, nullptr);
    BufferRequestConfig requestConfig = {
        .width = TEST_BUFFER_SIZE,
        .height = TEST_BUFFER_SIZE,
        .strideAlignment = TEST_STRIDE_ALIGNMENT,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };
    ASSERT_EQ(oldPreBuffer->Alloc(requestConfig), GSERROR_OK);
    ASSERT_EQ(currentBuffer->Alloc(requestConfig), GSERROR_OK);
    auto oldPreBufferOwnerCount = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    auto currentBufferOwnerCount = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    bool currentBufferReleased = false;
    currentBufferOwnerCount->bufferReleaseCb_ = [&currentBufferReleased](uint64_t) {
        currentBufferReleased = true;
    };
    context.surfaceHandler->SetBuffer(oldPreBuffer, SyncFence::InvalidFence(), Rect(), 0, oldPreBufferOwnerCount);
    context.surfaceHandler->SetBuffer(currentBuffer, SyncFence::InvalidFence(), Rect(), 0, currentBufferOwnerCount);
    ASSERT_EQ(oldPreBufferOwnerCount->bufferId_, oldPreBuffer->GetBufferId());
    ASSERT_EQ(currentBufferOwnerCount->bufferId_, currentBuffer->GetBufferId());
    ASSERT_EQ(context.surfaceHandler->GetPreBuffer(), oldPreBuffer);
    ASSERT_TRUE(FlushProducerBufferForTest(context.producer));
    context.surfaceHandler->SetAvailableBufferCount(1);
    RSMainThread::Instance()->directComposeHelper_.consecutiveDoCompSuccessCount_.store(TUNNEL_STABLE_THRESHOLD);
    RSTunnelRouteArbiter::RefreshGlobalTriggerSnapshot();
    ASSERT_TRUE(RSTunnelLayerHelper::TryCommitBufferDirect(
        context.node, composerClientManager, false, tunnelRuntime));
    EXPECT_TRUE(currentBufferReleased);
    EXPECT_EQ(currentBufferOwnerCount->refCount_.load(), 0);
    auto tunnelBufferOwnerCount = context.surfaceHandler->GetBufferOwnerCount();
    ASSERT_NE(tunnelBufferOwnerCount, nullptr);

    RSTunnelRuntimeStore::SetLayerInfo(context.node->GetId(), 0, TUNNEL_PROP_INVALID);
    tunnelRuntime.SetLayerInfo(0, TUNNEL_PROP_INVALID);
    tunnelRuntime.SetBuilding();

    auto normalBufferEntry = CreateTestBufferEntry();
    ASSERT_NE(normalBufferEntry.buffer, nullptr);
    context.surfaceHandler->ConsumeAndUpdateBuffer(normalBufferEntry);
    context.surfaceHandler->ReleaseAndResetPreBuffer();
    EXPECT_EQ(tunnelBufferOwnerCount->refCount_.load(), 0);
}

/**
 * @tc.name: ReleaseAndResetPreBuffer001
 * @tc.desc: Test preBuffer stays alive while ReleaseAndResetPreBuffer runs release callback.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerHelperTest, ReleaseAndResetPreBuffer001, TestSize.Level1)
{
    auto surfaceHandler = std::make_shared<RSSurfaceHandler>(1);
    sptr<SurfaceBuffer> firstBuffer = SurfaceBuffer::Create();
    sptr<SurfaceBuffer> secondBuffer = SurfaceBuffer::Create();
    ASSERT_NE(firstBuffer, nullptr);
    ASSERT_NE(secondBuffer, nullptr);
    BufferRequestConfig requestConfig = {
        .width = TEST_BUFFER_SIZE,
        .height = TEST_BUFFER_SIZE,
        .strideAlignment = TEST_STRIDE_ALIGNMENT,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };
    ASSERT_EQ(firstBuffer->Alloc(requestConfig), GSERROR_OK);
    ASSERT_EQ(secondBuffer->Alloc(requestConfig), GSERROR_OK);

    auto firstOwnerCount = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    surfaceHandler->SetBuffer(firstBuffer, SyncFence::InvalidFence(), Rect(), 0, firstOwnerCount);
    ASSERT_NE(firstOwnerCount->bufferId_, 0u);
    wptr<SurfaceBuffer> weakFirstBuffer = firstBuffer;
    bool callbackSawBuffer = false;
    firstOwnerCount->bufferReleaseCb_ = [&callbackSawBuffer, weakFirstBuffer](uint64_t) {
        callbackSawBuffer = weakFirstBuffer.promote() != nullptr;
    };

    auto secondOwnerCount = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    surfaceHandler->SetBuffer(secondBuffer, SyncFence::InvalidFence(), Rect(), 0, secondOwnerCount);
    firstBuffer = nullptr;

    ASSERT_NE(surfaceHandler->GetPreBuffer(), nullptr);
    EXPECT_TRUE(surfaceHandler->ReleaseAndResetPreBuffer());
    EXPECT_TRUE(callbackSawBuffer);
    EXPECT_EQ(firstOwnerCount->refCount_.load(), 0);
    EXPECT_EQ(surfaceHandler->GetPreBuffer(), nullptr);
}

/**
 * @tc.name: TryCommitTunnelLayerBufferDirect005
 * @tc.desc: Test fallback never overwrites an existing holdReturnValue with a newly failed tunnel buffer.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerHelperTest, TryCommitTunnelLayerBufferDirect005, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    auto context = CreateTunnelTestContext(true);
    ASSERT_TRUE(context.IsProducerReady());

    context.producer->SetQueueSize(TEST_QUEUE_SIZE);
    ASSERT_TRUE(SetTunnelInfoForConsumer(context.consumer));
    IConsumerSurface::AcquireBufferReturnValue existingReturnValue = CreateTestAcquireBufferReturnValue();
    ASSERT_NE(existingReturnValue.buffer, nullptr);
    context.surfaceHandler->SetHoldReturnValue(existingReturnValue);

    RSTunnelRuntimeStore::GetOrCreate(context.node->GetId()).SetBuilding();
    RSTunnelRuntimeStore::GetOrCreate(context.node->GetId()).SetActiveFromTunnelLayerAvailable(
        RSTunnelRuntimeStore::GetOrCreate(context.node->GetId()).GetTunnelLayerGeneration());
    ScopedRegisteredSurfaceNode registeredNode(context.node);
    ASSERT_TRUE(registeredNode.IsRegistered());
    context.surfaceHandler->SetAvailableBufferCount(1);

    auto composerClientManager = std::make_shared<RSComposerClientManager>();
    EXPECT_FALSE(RSTunnelLayerHelper::TryCommitBufferDirect(context.node, composerClientManager, false,
        RSTunnelRuntimeStore::GetOrCreate(context.node->GetId())));
    ASSERT_NE(context.surfaceHandler->GetHoldReturnValue(), nullptr);
    ASSERT_NE(context.surfaceHandler->GetHoldReturnValue()->buffer, nullptr);
    EXPECT_EQ(context.surfaceHandler->GetHoldReturnValue()->buffer->GetBufferId(),
        existingReturnValue.buffer->GetBufferId());

    EXPECT_TRUE(RSBaseSurfaceUtil::ConsumeAndUpdateBuffer(*context.surfaceHandler));
    ASSERT_NE(context.surfaceHandler->GetBuffer(), nullptr);
    EXPECT_EQ(context.surfaceHandler->GetBuffer()->GetBufferId(), existingReturnValue.buffer->GetBufferId());
    EXPECT_EQ(context.surfaceHandler->GetHoldReturnValue(), nullptr);
}

/**
 * @tc.name: OnBufferAvailable001
 * @tc.desc: Test tunnel callback-thread path lets normal consume handle buffer when composer is unavailable.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerHelperTest, OnBufferAvailable001, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    auto context = CreateTunnelTestContext(true);
    ASSERT_TRUE(context.IsProducerReady());

    ASSERT_EQ(context.consumer->SetSurfaceSourceType(OHSurfaceSource::OH_SURFACE_SOURCE_DEFAULT), GSERROR_OK);
    auto normalBufferEntry = CreateTestBufferEntry();
    ASSERT_NE(normalBufferEntry.buffer, nullptr);
    context.surfaceHandler->ConsumeAndUpdateBuffer(normalBufferEntry);
    auto normalBuffer = context.surfaceHandler->GetBuffer();
    ASSERT_NE(normalBuffer, nullptr);
    auto normalBufferOwnerCount = context.surfaceHandler->GetBufferOwnerCount();
    ASSERT_NE(normalBufferOwnerCount, nullptr);
    EXPECT_EQ(normalBufferOwnerCount->refCount_.load(), 1);

    TunnelLayerState state;
    ASSERT_TRUE(SetTunnelInfoForConsumer(context.consumer, state));
    RSTunnelRuntimeStore::SetLayerInfo(context.node->GetId(), state.tunnelLayerId, state.property);
    RSTunnelRuntimeStore::GetOrCreate(context.node->GetId()).SetBuilding();
    RSTunnelRuntimeStore::GetOrCreate(context.node->GetId()).SetActiveFromTunnelLayerAvailable(
        RSTunnelRuntimeStore::GetOrCreate(context.node->GetId()).GetTunnelLayerGeneration());
    RSMainThread::Instance()->hgmRenderContext_ = std::make_shared<HgmRenderContext>(nullptr);
    context.surfaceHandler->SetAvailableBufferCount(0);

    auto surfaceHandler(context.node->GetRSSurfaceHandler());
    auto rsListener = std::make_shared<RSRenderServiceListener>(context.node, surfaceHandler, nullptr);
    rsListener->OnBufferAvailable();

    EXPECT_EQ(context.surfaceHandler->GetBuffer(), normalBuffer);
    EXPECT_EQ(context.surfaceHandler->GetBufferOwnerCount(), normalBufferOwnerCount);
    EXPECT_EQ(context.surfaceHandler->GetHoldReturnValue(), nullptr);
    EXPECT_EQ(context.surfaceHandler->GetAvailableBufferCount(), 1);
}

/**
 * @tc.name: OnBufferAvailable002
 * @tc.desc: Test ACTIVE tunnel callback-thread path acquires and commits a buffer directly.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerHelperTest, OnBufferAvailable002, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    auto context = CreateTunnelTestContext(true);
    ASSERT_TRUE(context.IsProducerReady());

    ASSERT_EQ(context.consumer->SetSurfaceSourceType(OHSurfaceSource::OH_SURFACE_SOURCE_DEFAULT), GSERROR_OK);
    auto normalBufferEntry = CreateTestBufferEntry();
    ASSERT_NE(normalBufferEntry.buffer, nullptr);
    context.surfaceHandler->ConsumeAndUpdateBuffer(normalBufferEntry);
    auto normalBufferOwnerCount = context.surfaceHandler->GetBufferOwnerCount();
    ASSERT_NE(normalBufferOwnerCount, nullptr);
    EXPECT_EQ(normalBufferOwnerCount->refCount_.load(), 1);

    context.producer->SetQueueSize(TEST_QUEUE_SIZE);
    TunnelLayerState state;
    ASSERT_TRUE(SetTunnelInfoForConsumer(context.consumer, state));
    RSTunnelRuntimeStore::SetLayerInfo(context.node->GetId(), state.tunnelLayerId, state.property);
    RSTunnelRuntimeStore::GetOrCreate(context.node->GetId()).SetBuilding();
    RSTunnelRuntimeStore::GetOrCreate(context.node->GetId()).SetActiveFromTunnelLayerAvailable(
        RSTunnelRuntimeStore::GetOrCreate(context.node->GetId()).GetTunnelLayerGeneration());
    auto& tunnelRuntime = RSTunnelRuntimeStore::GetOrCreate(context.node->GetId());
    ASSERT_EQ(tunnelRuntime.TryClaimByMain(true), RSTunnelRuntimeState::ClaimResult::GO_NORMAL);
    tunnelRuntime.OnRenderCommitDone();
    ASSERT_EQ(tunnelRuntime.GetPhase(), RSTunnelRuntimeState::Phase::NORMAL_COMMITTED);
    ScopedRegisteredSurfaceNode registeredNode(context.node);
    ASSERT_TRUE(registeredNode.IsRegistered());
    auto connection = sptr<RecordingRenderToComposerConnection>::MakeSptr();
    auto composerManager = CreateRecordingComposerManager(context.node->GetId(), connection);
    ASSERT_NE(composerManager, nullptr);
    RSMainThread::Instance()->directComposeHelper_.consecutiveDoCompSuccessCount_.store(TUNNEL_STABLE_THRESHOLD);
    RSTunnelRouteArbiter::RefreshGlobalTriggerSnapshot();
    ASSERT_TRUE(FlushProducerBufferForTest(context.producer));
    context.surfaceHandler->SetAvailableBufferCount(1);

    auto surfaceHandler(context.node->GetRSSurfaceHandler());
    auto rsListener = std::make_shared<RSRenderServiceListener>(context.node, surfaceHandler, composerManager);
    rsListener->OnBufferAvailable();

    EXPECT_TRUE(connection->commitTunnelCalled);
    EXPECT_EQ(connection->commitTunnelCallCount, 1u);
    EXPECT_EQ(connection->lastSurfaceId, context.consumer->GetUniqueId());
    EXPECT_EQ(connection->lastTunnelLayerId, state.tunnelLayerId);
    EXPECT_NE(connection->lastBufferId, 0u);
    ASSERT_NE(context.surfaceHandler->GetBuffer(), nullptr);
    EXPECT_EQ(context.surfaceHandler->GetBuffer()->GetBufferId(), connection->lastBufferId);
    EXPECT_TRUE(RSTunnelRuntimeStore::GetOrCreate(context.node->GetId()).IsCommittedTunnelBuffer());
    EXPECT_EQ(normalBufferOwnerCount->refCount_.load(), 0);
    auto firstTunnelBufferOwnerCount = context.surfaceHandler->GetBufferOwnerCount();
    ASSERT_NE(firstTunnelBufferOwnerCount, nullptr);
    EXPECT_EQ(firstTunnelBufferOwnerCount->refCount_.load(), 1);
    EXPECT_TRUE(context.surfaceHandler->IsCurrentFrameBufferConsumed());
    EXPECT_EQ(context.surfaceHandler->GetPreBuffer(), nullptr);
    EXPECT_EQ(RSTunnelRuntimeStore::GetOrCreate(context.node->GetId()).GetTunnelState(),
        RSTunnelRuntimeState::TunnelState::ACTIVE);

    constexpr uint32_t COMMIT_COUNT_AFTER_SECOND_BUFFER = 2;
    ASSERT_TRUE(FlushProducerBufferForTest(context.producer));
    context.surfaceHandler->SetAvailableBufferCount(1);
    rsListener->OnBufferAvailable();
    EXPECT_TRUE(connection->commitTunnelCalled);
    EXPECT_EQ(connection->commitTunnelCallCount, COMMIT_COUNT_AFTER_SECOND_BUFFER);
    EXPECT_NE(connection->lastBufferId, 0u);
    ASSERT_NE(context.surfaceHandler->GetBuffer(), nullptr);
    EXPECT_EQ(context.surfaceHandler->GetBuffer()->GetBufferId(), connection->lastBufferId);
    EXPECT_TRUE(RSTunnelRuntimeStore::GetOrCreate(context.node->GetId()).IsCommittedTunnelBuffer());
    EXPECT_TRUE(context.surfaceHandler->IsCurrentFrameBufferConsumed());
    EXPECT_EQ(normalBufferOwnerCount->refCount_.load(), 0);
    EXPECT_EQ(firstTunnelBufferOwnerCount->refCount_.load(), 0);
    EXPECT_EQ(context.surfaceHandler->GetPreBuffer(), nullptr);
}

/**
 * @tc.name: OnBufferAvailable003
 * @tc.desc: Test BUILDING tunnel callback-thread path does not acquire or activate tunnel direct.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerHelperTest, OnBufferAvailable003, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    auto context = CreateTunnelTestContext(true);
    ASSERT_TRUE(context.IsProducerReady());

    ASSERT_EQ(context.consumer->SetSurfaceSourceType(OHSurfaceSource::OH_SURFACE_SOURCE_DEFAULT), GSERROR_OK);
    TunnelLayerState state;
    ASSERT_TRUE(SetTunnelInfoForConsumer(context.consumer, state));
    RSTunnelRuntimeStore::SetLayerInfo(context.node->GetId(), state.tunnelLayerId, state.property);
    RSTunnelRuntimeStore::GetOrCreate(context.node->GetId()).SetBuilding();
    auto connection = sptr<RecordingRenderToComposerConnection>::MakeSptr();
    auto composerManager = CreateRecordingComposerManager(context.node->GetId(), connection);
    ASSERT_NE(composerManager, nullptr);
    context.surfaceHandler->SetAvailableBufferCount(0);

    auto surfaceHandler(context.node->GetRSSurfaceHandler());
    auto rsListener = std::make_shared<RSRenderServiceListener>(context.node, surfaceHandler, composerManager);
    rsListener->OnBufferAvailable();

    EXPECT_FALSE(connection->commitTunnelCalled);
    EXPECT_EQ(RSTunnelRuntimeStore::GetOrCreate(context.node->GetId()).GetTunnelState(),
        RSTunnelRuntimeState::TunnelState::BUILDING);
    EXPECT_EQ(context.surfaceHandler->GetAvailableBufferCount(), 1);
}

/**
 * @tc.name: ResolveTunnelLayerInfo_NodeIdFallback001
 * @tc.desc: Test ResolveTunnelLayerInfo uses nodeId fallback when consumer returns zero tunnelLayerId.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerHelperTest, ResolveTunnelLayerInfo_NodeIdFallback001, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    auto context = CreateTunnelTestContext(false);
    ASSERT_TRUE(context.IsBaseReady());
 
    constexpr uint64_t expectedTunnelLayerId = 9001;
    constexpr uint32_t expectedProperty = TUNNEL_PROP_BUFFER_ADDR | TUNNEL_PROP_DEVICE_COMMIT;
    RSTunnelRuntimeStore::SetLayerInfo(context.node->GetId(), expectedTunnelLayerId, expectedProperty);
 
    auto countingConsumer = new CountingTunnelInfoConsumerSurface("fallback_test");
    ASSERT_NE(countingConsumer, nullptr);
    countingConsumer->SetTunnelInfoResult(GSERROR_OK, 0, TUNNEL_PROP_INVALID);
 
    uint64_t tunnelLayerId = 0;
    uint32_t property = TUNNEL_PROP_INVALID;
    EXPECT_TRUE(RSTunnelLayerHelper::ResolveTunnelLayerInfo(
        countingConsumer, tunnelLayerId, property, context.node->GetId()));
    EXPECT_EQ(tunnelLayerId, expectedTunnelLayerId);
    EXPECT_EQ(property, expectedProperty);
    EXPECT_EQ(countingConsumer->GetTunnelLayerInfoCallCount(), 0);
}
 
/**
 * @tc.name: ResolveTunnelLayerInfo_NodeIdFallbackFallsThrough002
 * @tc.desc: Test ResolveTunnelLayerInfo falls through to consumer when nodeId store has zero tunnelLayerId.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerHelperTest, ResolveTunnelLayerInfo_NodeIdFallbackFallsThrough002, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    auto context = CreateTunnelTestContext(false);
    ASSERT_TRUE(context.IsBaseReady());
 
    RSTunnelRuntimeStore::SetLayerInfo(context.node->GetId(), 0, TUNNEL_PROP_INVALID);
 
    constexpr uint64_t consumerTunnelLayerId = 9002;
    constexpr uint32_t consumerProperty = TUNNEL_PROP_BUFFER_ADDR;
    auto countingConsumer = new CountingTunnelInfoConsumerSurface("fallback_through_test");
    ASSERT_NE(countingConsumer, nullptr);
    countingConsumer->SetTunnelInfoResult(GSERROR_OK, consumerTunnelLayerId, consumerProperty);
 
    uint64_t tunnelLayerId = 0;
    uint32_t property = TUNNEL_PROP_INVALID;
    EXPECT_TRUE(RSTunnelLayerHelper::ResolveTunnelLayerInfo(
        countingConsumer, tunnelLayerId, property, context.node->GetId()));
    EXPECT_NE(tunnelLayerId, consumerTunnelLayerId);
    EXPECT_NE(property, consumerProperty);
    EXPECT_NE(countingConsumer->GetTunnelLayerInfoCallCount(), 1);
}
 
/**
 * @tc.name: ResolveTunnelLayerInfo_NodeIdZeroSkipsFallback003
 * @tc.desc: Test ResolveTunnelLayerInfo skips nodeId fallback when nodeId is zero.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerHelperTest, ResolveTunnelLayerInfo_NodeIdZeroSkipsFallback003, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    auto context = CreateTunnelTestContext(false);
    ASSERT_TRUE(context.IsBaseReady());
 
    constexpr uint64_t storedTunnelLayerId = 9003;
    constexpr uint32_t storedProperty = TUNNEL_PROP_BUFFER_ADDR;
    RSTunnelRuntimeStore::SetLayerInfo(context.node->GetId(), storedTunnelLayerId, storedProperty);
 
    constexpr uint64_t consumerTunnelLayerId = 9004;
    constexpr uint32_t consumerProperty = TUNNEL_PROP_BUFFER_ADDR | TUNNEL_PROP_RS_FORCE;
    auto countingConsumer = new CountingTunnelInfoConsumerSurface("skip_fallback_test");
    ASSERT_NE(countingConsumer, nullptr);
    countingConsumer->SetTunnelInfoResult(GSERROR_OK, consumerTunnelLayerId, consumerProperty);
 
    uint64_t tunnelLayerId = 0;
    uint32_t property = TUNNEL_PROP_INVALID;
    EXPECT_TRUE(RSTunnelLayerHelper::ResolveTunnelLayerInfo(
        countingConsumer, tunnelLayerId, property, 0));
    EXPECT_NE(tunnelLayerId, consumerTunnelLayerId);
    EXPECT_NE(property, consumerProperty);
    EXPECT_NE(countingConsumer->GetTunnelLayerInfoCallCount(), 1);
}

/**
 * @tc.name: OnBufferAvailable004
 * @tc.desc: Test listener keeps tunnel buffer for normal consume fallback when direct IPC commit fails.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerHelperTest, OnBufferAvailable004, TestSize.Level1)
{
    ScopedNewTunnelSwitch newTunnelSwitch(true);
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto node = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(node, nullptr);
    RSTestUtil::UnregisterConsumerListener();

    auto surfaceHandler = node->GetMutableRSSurfaceHandler();
    auto consumer = surfaceHandler->GetConsumer();
    sptr<IBufferProducer> producerToken = consumer->GetProducer();
    auto producer = Surface::CreateSurfaceAsProducer(producerToken);
    producer->SetQueueSize(TEST_QUEUE_SIZE);

    std::vector<LayerStateChange> results;
    ASSERT_EQ(producer->RegisterLayerStateChangedListener(
                  [&results](LayerStateChange state) { results.emplace_back(state); }),
        GSERROR_OK);
    TunnelLayerState state;
    ASSERT_FALSE(SetTunnelInfoForConsumer(consumer, state));
    RSTunnelRuntimeStore::SetLayerInfo(node->GetId(), state.tunnelLayerId, state.property);
    RSTunnelRuntimeStore::GetOrCreate(node->GetId()).SetBuilding();
    RSTunnelRuntimeStore::GetOrCreate(node->GetId()).SetActiveFromTunnelLayerAvailable(
        RSTunnelRuntimeStore::GetOrCreate(node->GetId()).GetTunnelLayerGeneration());
    ScopedRegisteredSurfaceNode registeredNode(node);
    ASSERT_TRUE(registeredNode.IsRegistered());

    auto listener =
        std::make_shared<RSRenderServiceListener>(node, surfaceHandler, std::make_shared<RSComposerClientManager>());
    ASSERT_TRUE(FlushProducerBufferForTest(producer));
    surfaceHandler->SetAvailableBufferCount(1);
    RSMainThread::Instance()->directComposeHelper_.consecutiveDoCompSuccessCount_.store(TUNNEL_STABLE_THRESHOLD);
    RSTunnelRouteArbiter::RefreshGlobalTriggerSnapshot();
    listener->OnBufferAvailable();

    EXPECT_TRUE(results.empty());
    uint64_t actualTunnelLayerId = 0;
    uint32_t actualProperty = TUNNEL_PROP_INVALID;
    RSTunnelRuntimeStore::GetLayerInfoOrDefault(node->GetId(), actualTunnelLayerId, actualProperty);
    EXPECT_EQ(actualTunnelLayerId, 0u);
    EXPECT_EQ(actualProperty, TUNNEL_PROP_INVALID);
    EXPECT_EQ(RSTunnelRuntimeStore::GetOrCreate(node->GetId()).GetTunnelState(),
        RSTunnelRuntimeState::TunnelState::BUILDING);
    RSTestUtil::UnregisterConsumerListener();
}

/**
 * @tc.name: TryCommitTunnelLayerBufferDirect006
 * @tc.desc: Test TryCommitPendingBuffer decrements bufferOwnerCount at entry when exists
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerHelperTest, TryCommitTunnelLayerBufferDirect006, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    auto context = CreateTunnelTestContext(true);
    ASSERT_TRUE(context.IsProducerReady());

    context.producer->SetQueueSize(TEST_QUEUE_SIZE);
    TunnelLayerState state;
    ASSERT_TRUE(SetTunnelInfoForConsumer(context.consumer, state));
    RSTunnelRuntimeStore::SetLayerInfo(context.node->GetId(), state.tunnelLayerId, state.property);
    auto& tunnelRuntime = RSTunnelRuntimeStore::GetOrCreate(context.node->GetId());
    tunnelRuntime.SetBuilding();
    ASSERT_TRUE(tunnelRuntime.SetActiveFromTunnelLayerAvailable(tunnelRuntime.GetTunnelLayerGeneration()));
    ScopedRegisteredSurfaceNode registeredNode(context.node);
    ASSERT_TRUE(registeredNode.IsRegistered());

    auto connection = sptr<RecordingRenderToComposerConnection>::MakeSptr();
    connection->commitTunnelResult = GRAPHIC_DISPLAY_SUCCESS;
    auto composerClientManager = CreateRecordingComposerManager(context.node->GetId(), connection);
    ASSERT_NE(composerClientManager, nullptr);

    sptr<SurfaceBuffer> currentBuffer = SurfaceBuffer::Create();
    ASSERT_NE(currentBuffer, nullptr);
    BufferRequestConfig requestConfig = {
        .width = TEST_BUFFER_SIZE,
        .height = TEST_BUFFER_SIZE,
        .strideAlignment = TEST_STRIDE_ALIGNMENT,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };
    ASSERT_EQ(currentBuffer->Alloc(requestConfig), GSERROR_OK);

    auto currentBufferOwnerCount = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    currentBufferOwnerCount->bufferId_ = currentBuffer->GetBufferId();
    currentBufferOwnerCount->refCount_.store(2);
    context.surfaceHandler->SetBuffer(currentBuffer, SyncFence::InvalidFence(), Rect(), 0, currentBufferOwnerCount);

    ASSERT_TRUE(FlushProducerBufferForTest(context.producer));
    context.surfaceHandler->SetAvailableBufferCount(1);
    RSMainThread::Instance()->directComposeHelper_.consecutiveDoCompSuccessCount_.store(TUNNEL_STABLE_THRESHOLD);
    RSTunnelRouteArbiter::RefreshGlobalTriggerSnapshot();

    EXPECT_TRUE(RSTunnelLayerHelper::TryCommitBufferDirect(
        context.node, composerClientManager, false, tunnelRuntime));
    EXPECT_EQ(currentBufferOwnerCount->refCount_.load(), 1);
}
 
/**
 * @tc.name: LastBufferStatus_TransitionsToTunnelOnSuccess
 * @tc.desc: Test tunnelBufferStatus_ transitions to TUNNEL_STATUS on successful tunnel commit
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerHelperTest, LastBufferStatus_TransitionsToTunnelOnSuccess, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    auto context = CreateTunnelTestContext(true);
    ASSERT_TRUE(context.IsProducerReady());

    context.producer->SetQueueSize(TEST_QUEUE_SIZE);
    TunnelLayerState state;
    ASSERT_TRUE(SetTunnelInfoForConsumer(context.consumer, state));
    RSTunnelRuntimeStore::SetLayerInfo(context.node->GetId(), state.tunnelLayerId, state.property);
    auto& tunnelRuntime = RSTunnelRuntimeStore::GetOrCreate(context.node->GetId());
    tunnelRuntime.SetBuilding();
    ASSERT_TRUE(tunnelRuntime.SetActiveFromTunnelLayerAvailable(tunnelRuntime.GetTunnelLayerGeneration()));
    ScopedRegisteredSurfaceNode registeredNode(context.node);
    ASSERT_TRUE(registeredNode.IsRegistered());

    auto connection = sptr<RecordingRenderToComposerConnection>::MakeSptr();
    connection->commitTunnelResult = GRAPHIC_DISPLAY_SUCCESS;
    auto composerClientManager = CreateRecordingComposerManager(context.node->GetId(), connection);
    ASSERT_NE(composerClientManager, nullptr);

    ASSERT_TRUE(FlushProducerBufferForTest(context.producer));
    context.surfaceHandler->SetAvailableBufferCount(1);
    RSMainThread::Instance()->directComposeHelper_.consecutiveDoCompSuccessCount_.store(TUNNEL_STABLE_THRESHOLD);
    RSTunnelRouteArbiter::RefreshGlobalTriggerSnapshot();

    EXPECT_TRUE(RSTunnelLayerHelper::TryCommitBufferDirect(
        context.node, composerClientManager, false, tunnelRuntime));
}

/**
 * @tc.name: TryCommitBufferDirect_NullNode_ReturnsFalse
 * @tc.desc: Test TryCommitBufferDirect returns false immediately when node is nullptr, covering the
 *          null-node guard at entry so subsequent dereferences are not reached.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerHelperTest, TryCommitBufferDirect_NullNode_ReturnsFalse, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    RSTunnelRuntimeState dummyRuntime;
    auto connection = sptr<RecordingRenderToComposerConnection>::MakeSptr();
    auto composerClientManager = CreateRecordingComposerManager(0, connection);
    ASSERT_NE(composerClientManager, nullptr);
    EXPECT_FALSE(RSTunnelLayerHelper::TryCommitBufferDirect(nullptr, composerClientManager, false, dummyRuntime));
    EXPECT_FALSE(connection->commitTunnelCalled);
}

/**
 * @tc.name: TryCommitPendingBuffer_SizeMismatch_ReleasesBuffer
 * @tc.desc: Test TryCommitPendingBuffer releases the acquired buffer and clears the release callback
 *          when the surface handler already holds a buffer with a different size, covering the
 *          size-mismatch failure path.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerHelperTest, TryCommitPendingBuffer_SizeMismatch_ReleasesBuffer, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    auto context = CreateTunnelTestContext(true);
    ASSERT_TRUE(context.IsProducerReady());

    context.producer->SetQueueSize(TEST_QUEUE_SIZE);
    TunnelLayerState state;
    ASSERT_TRUE(SetTunnelInfoForConsumer(context.consumer, state));
    RSTunnelRuntimeStore::SetLayerInfo(context.node->GetId(), state.tunnelLayerId, state.property);
    auto& tunnelRuntime = RSTunnelRuntimeStore::GetOrCreate(context.node->GetId());
    tunnelRuntime.SetBuilding();
    ASSERT_TRUE(tunnelRuntime.SetActiveFromTunnelLayerAvailable(tunnelRuntime.GetTunnelLayerGeneration()));
    ScopedRegisteredSurfaceNode registeredNode(context.node);
    ASSERT_TRUE(registeredNode.IsRegistered());

    auto connection = sptr<RecordingRenderToComposerConnection>::MakeSptr();
    connection->commitTunnelResult = GRAPHIC_DISPLAY_SUCCESS;
    auto composerClientManager = CreateRecordingComposerManager(context.node->GetId(), connection);
    ASSERT_NE(composerClientManager, nullptr);

    constexpr uint32_t EXISTING_BUFFER_SIZE = TEST_BUFFER_SIZE / 2;
    sptr<SurfaceBuffer> existingBuffer = SurfaceBuffer::Create();
    ASSERT_NE(existingBuffer, nullptr);
    BufferRequestConfig existingRequestConfig = {
        .width = EXISTING_BUFFER_SIZE,
        .height = EXISTING_BUFFER_SIZE,
        .strideAlignment = TEST_STRIDE_ALIGNMENT,
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };
    ASSERT_EQ(existingBuffer->Alloc(existingRequestConfig), GSERROR_OK);
    auto existingBufferOwnerCount = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    context.surfaceHandler->SetBuffer(
        existingBuffer, SyncFence::InvalidFence(), Rect(), 0, existingBufferOwnerCount);

    ASSERT_TRUE(FlushProducerBufferForTest(context.producer));
    context.surfaceHandler->SetAvailableBufferCount(1);
    RSMainThread::Instance()->directComposeHelper_.consecutiveDoCompSuccessCount_.store(TUNNEL_STABLE_THRESHOLD);
    RSTunnelRouteArbiter::RefreshGlobalTriggerSnapshot();

    EXPECT_FALSE(RSTunnelLayerHelper::TryCommitBufferDirect(
        context.node, composerClientManager, false, tunnelRuntime));
    EXPECT_FALSE(connection->commitTunnelCalled);
    EXPECT_EQ(tunnelRuntime.GetTunnelState(), RSTunnelRuntimeState::TunnelState::ACTIVE);
    EXPECT_EQ(context.consumer->GetAvailableBufferCount(), 1u);
}

/**
 * @tc.name: TryCommitPendingBuffer_NullComposerManager_ReleasesBuffer
 * @tc.desc: Test TryCommitPendingBuffer releases the acquired buffer and clears the release callback
 *          when composerClientManager is nullptr, covering the null-manager failure path.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerHelperTest, TryCommitPendingBuffer_NullComposerManager_ReleasesBuffer, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    auto context = CreateTunnelTestContext(true);
    ASSERT_TRUE(context.IsProducerReady());

    context.producer->SetQueueSize(TEST_QUEUE_SIZE);
    TunnelLayerState state;
    ASSERT_TRUE(SetTunnelInfoForConsumer(context.consumer, state));
    RSTunnelRuntimeStore::SetLayerInfo(context.node->GetId(), state.tunnelLayerId, state.property);
    auto& tunnelRuntime = RSTunnelRuntimeStore::GetOrCreate(context.node->GetId());
    tunnelRuntime.SetBuilding();
    ASSERT_TRUE(tunnelRuntime.SetActiveFromTunnelLayerAvailable(tunnelRuntime.GetTunnelLayerGeneration()));
    ScopedRegisteredSurfaceNode registeredNode(context.node);
    ASSERT_TRUE(registeredNode.IsRegistered());

    ASSERT_TRUE(FlushProducerBufferForTest(context.producer));
    context.surfaceHandler->SetAvailableBufferCount(1);
    RSMainThread::Instance()->directComposeHelper_.consecutiveDoCompSuccessCount_.store(TUNNEL_STABLE_THRESHOLD);
    RSTunnelRouteArbiter::RefreshGlobalTriggerSnapshot();

    EXPECT_FALSE(RSTunnelLayerHelper::TryCommitBufferDirect(
        context.node, nullptr, false, tunnelRuntime));
    uint64_t clearedTunnelLayerId = 1;
    uint32_t clearedProperty = state.property;
    tunnelRuntime.GetLayerInfo(clearedTunnelLayerId, clearedProperty);
    EXPECT_EQ(clearedTunnelLayerId, 0u);
    EXPECT_EQ(clearedProperty, TUNNEL_PROP_INVALID);
    EXPECT_EQ(tunnelRuntime.GetTunnelState(), RSTunnelRuntimeState::TunnelState::BUILDING);
    EXPECT_EQ(context.consumer->GetAvailableBufferCount(), 1u);
}
} // namespace OHOS::Rosen
