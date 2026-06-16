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
        ASSERT_TRUE(SetTunnelInfoForConsumer(context.consumer, expectedState, testCase.consumerTunnelType));

        uint64_t tunnelLayerId = 0;
        uint32_t property = TUNNEL_PROP_INVALID;
        EXPECT_TRUE(RSTunnelLayerHelper::ResolveTunnelLayerInfo(context.consumer, tunnelLayerId, property));
        EXPECT_EQ(tunnelLayerId, expectedState.tunnelLayerId);
        EXPECT_EQ(property, expectedState.property);
        EXPECT_EQ(testCase.expectNewTunnel, tunnelLayerId != 0 && IsNewTunnelProperty(property));
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
        EXPECT_EQ(tunnelLayerId, testCase.consumerTunnelLayerId);
        EXPECT_EQ(property, testCase.consumerProperty);
        EXPECT_EQ(testCase.expectNewTunnel, tunnelLayerId != 0 && IsNewTunnelProperty(property));
        EXPECT_EQ(consumer->GetTunnelLayerInfoCallCount(), 1);
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
    EXPECT_FALSE(RSTunnelLayerHelper::ResolveTunnelLayerInfo(consumer, tunnelLayerId, property));
    EXPECT_EQ(consumer->GetTunnelLayerInfoCallCount(), 1);
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
    ASSERT_TRUE(SetRuntimePendingBufferForTest(context));
    ASSERT_TRUE(tunnelRuntimeState.HasPendingBuffer());

    auto nextConsumer = IConsumerSurface::Create("TunnelConsumer2");
    ASSERT_NE(nextConsumer, nullptr);
    context.node->SetConsumer(nextConsumer);
    EXPECT_FALSE(tunnelRuntimeState.HasPendingBuffer());
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
        bool consumePendingBuffer;
        bool setupPendingBuffer;
        bool onTree;
        bool hardwareDisabled;
        bool keepAvailableBufferCount;
    };
    const std::vector<TestCase> testCases = {
        { "invalid_consumer_tunnel_info", TUNNEL_TYPE_NONE, RSTunnelRuntimeState::TunnelState::ACTIVE,
            false, true, true, false, false },
        { "building_state_blocks_direct_commit", TUNNEL_TYPE_STYLUS,
            RSTunnelRuntimeState::TunnelState::BUILDING, true, false, true, false, false },
        { "not_on_tree_blocks_direct_commit", TUNNEL_TYPE_STYLUS,
            RSTunnelRuntimeState::TunnelState::ACTIVE, true, false, false, false, true },
        { "hardware_disabled_blocks_direct_commit", TUNNEL_TYPE_STYLUS,
            RSTunnelRuntimeState::TunnelState::ACTIVE, true, false, true, true, true },
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
        if (testCase.setupPendingBuffer) {
            ASSERT_TRUE(SetRuntimePendingBufferForTest(context));
            ASSERT_TRUE(RSTunnelRuntimeStore::GetOrCreate(context.node->GetId()).HasPendingBuffer());
        }

        auto connection = sptr<RecordingRenderToComposerConnection>::MakeSptr();
        auto composerClientManager = CreateRecordingComposerManager(context.node->GetId(), connection);
        ASSERT_NE(composerClientManager, nullptr);
        EXPECT_FALSE(RSTunnelLayerHelper::TryCommitBufferDirect(
            context.node, composerClientManager, testCase.consumePendingBuffer));
        EXPECT_FALSE(connection->commitTunnelCalled);
        if (testCase.keepAvailableBufferCount) {
            EXPECT_FALSE(tunnelRuntime.HasPendingBuffer());
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
    EXPECT_FALSE(RSTunnelLayerHelper::TryCommitBufferDirect(context.node, composerClientManager, true));
    EXPECT_FALSE(connection->commitTunnelCalled);
    EXPECT_FALSE(tunnelRuntime.HasPendingBuffer());
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
    EXPECT_FALSE(RSTunnelLayerHelper::AcquirePendingBuffer(nullptr));

    auto context = CreateTunnelTestContext(true);
    ASSERT_TRUE(context.IsProducerReady());
    EXPECT_FALSE(RSTunnelLayerHelper::AcquirePendingBuffer(context.node));
    EXPECT_FALSE(RSTunnelRuntimeStore::GetOrCreate(context.node->GetId()).HasPendingBuffer());

    auto& tunnelRuntime = RSTunnelRuntimeStore::GetOrCreate(context.node->GetId());
    tunnelRuntime.SetBuilding();
    ASSERT_TRUE(tunnelRuntime.SetActiveFromTunnelLayerAvailable(tunnelRuntime.GetTunnelLayerGeneration()));
    context.surfaceHandler->SetAvailableBufferCount(0);
    EXPECT_FALSE(RSTunnelLayerHelper::AcquirePendingBuffer(context.node));
    EXPECT_FALSE(tunnelRuntime.HasPendingBuffer());

    context.surfaceHandler->SetAvailableBufferCount(1);
    EXPECT_FALSE(RSTunnelLayerHelper::AcquirePendingBuffer(context.node));
    EXPECT_FALSE(tunnelRuntime.HasPendingBuffer());
    EXPECT_EQ(context.surfaceHandler->GetAvailableBufferCount(), 0);
}

/**
 * @tc.name: AcquirePendingBuffer_CreatesRuntimePendingEntry
 * @tc.desc: Test a successful direct acquire stores the pending entry fields created from the consumer return value.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerHelperTest, AcquirePendingBuffer_CreatesRuntimePendingEntry, TestSize.Level1)
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

    ASSERT_TRUE(RSTunnelLayerHelper::AcquirePendingBuffer(context.node));
    ASSERT_TRUE(tunnelRuntime.HasPendingBuffer());

    RSSurfaceHandler::SurfaceBufferEntry pendingBuffer;
    ASSERT_TRUE(tunnelRuntime.TakePendingBuffer(pendingBuffer));
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
    ASSERT_TRUE(SetTunnelInfoForConsumer(context.consumer));
    RSTunnelRuntimeStore::GetOrCreate(context.node->GetId()).SetBuilding();
    RSTunnelRuntimeStore::GetOrCreate(context.node->GetId()).SetActiveFromTunnelLayerAvailable(
        RSTunnelRuntimeStore::GetOrCreate(context.node->GetId()).GetTunnelLayerGeneration());
    ScopedRegisteredSurfaceNode registeredNode(context.node);
    ASSERT_TRUE(registeredNode.IsRegistered());
    ASSERT_TRUE(SetRuntimePendingBufferForTest(context));
    context.surfaceHandler->SetAvailableBufferCount(1);

    auto connection = sptr<RecordingRenderToComposerConnection>::MakeSptr();
    connection->commitTunnelResult = GRAPHIC_DISPLAY_FAILURE;
    auto composerClientManager = CreateRecordingComposerManager(context.node->GetId(), connection);
    ASSERT_NE(composerClientManager, nullptr);
    RSMainThread::Instance()->directComposeHelper_.consecutiveDoCompSuccessCount_.store(TUNNEL_STABLE_THRESHOLD);
    RSTunnelRouteArbiter::RefreshGlobalTriggerSnapshot();
    EXPECT_FALSE(RSTunnelLayerHelper::TryCommitBufferDirect(context.node, composerClientManager, true));
    EXPECT_TRUE(connection->commitTunnelCalled);
    EXPECT_TRUE(RSTunnelRuntimeStore::GetOrCreate(context.node->GetId()).HasPendingBuffer());
    uint64_t actualTunnelLayerId = 0;
    uint32_t actualProperty = TUNNEL_PROP_INVALID;
    RSTunnelRuntimeStore::GetLayerInfoOrDefault(context.node->GetId(), actualTunnelLayerId, actualProperty);
    EXPECT_EQ(actualTunnelLayerId, 0u);
    EXPECT_EQ(RSTunnelRuntimeStore::GetOrCreate(context.node->GetId()).GetTunnelState(),
        RSTunnelRuntimeState::TunnelState::BUILDING);
    EXPECT_EQ(context.surfaceHandler->GetAvailableBufferCount(), 1);

    RSSurfaceHandler::SurfaceBufferEntry pendingBuffer;
    ASSERT_TRUE(RSTunnelRuntimeStore::GetOrCreate(context.node->GetId()).TakePendingBuffer(pendingBuffer));
    ASSERT_NE(pendingBuffer.bufferOwnerCount_, nullptr);
    EXPECT_EQ(pendingBuffer.bufferOwnerCount_->refCount_.load(), 1);
    RSTunnelRuntimeStore::GetOrCreate(context.node->GetId()).SetPendingBuffer(pendingBuffer);

    ASSERT_TRUE(MoveRuntimePendingToNormalHold(context));
    auto holdBuffer = context.surfaceHandler->GetHoldBuffer();
    ASSERT_NE(holdBuffer, nullptr);
    ASSERT_NE(holdBuffer->buffer, nullptr);
    auto fallbackBufferId = holdBuffer->buffer->GetBufferId();
    EXPECT_TRUE(RSBaseSurfaceUtil::ConsumeAndUpdateBuffer(*context.surfaceHandler));
    ASSERT_NE(context.surfaceHandler->GetBuffer(), nullptr);
    EXPECT_EQ(context.surfaceHandler->GetBuffer()->GetBufferId(), fallbackBufferId);
    EXPECT_TRUE(context.surfaceHandler->IsCurrentFrameBufferConsumed());
    EXPECT_EQ(context.surfaceHandler->GetHoldReturnValue(), nullptr);
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

    TunnelLayerState state;
    ASSERT_TRUE(SetTunnelInfoForConsumer(context.consumer, state));
    RSTunnelRuntimeStore::SetLayerInfo(context.node->GetId(), state.tunnelLayerId, state.property);
    auto& tunnelRuntime = RSTunnelRuntimeStore::GetOrCreate(context.node->GetId());
    tunnelRuntime.SetBuilding();
    ASSERT_TRUE(tunnelRuntime.SetActiveFromTunnelLayerAvailable(tunnelRuntime.GetTunnelLayerGeneration()));
    ScopedRegisteredSurfaceNode registeredNode(context.node);
    ASSERT_TRUE(registeredNode.IsRegistered());
    auto connection = sptr<RecordingRenderToComposerConnection>::MakeSptr();
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
    ASSERT_TRUE(SetRuntimePendingBufferForTest(context));
    context.surfaceHandler->SetAvailableBufferCount(1);
    RSMainThread::Instance()->directComposeHelper_.consecutiveDoCompSuccessCount_.store(TUNNEL_STABLE_THRESHOLD);
    RSTunnelRouteArbiter::RefreshGlobalTriggerSnapshot();
    ASSERT_TRUE(RSTunnelLayerHelper::TryCommitBufferDirect(context.node, composerClientManager, true));
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
    ASSERT_TRUE(SetRuntimePendingBufferForTest(context));
    context.surfaceHandler->SetAvailableBufferCount(1);

    auto composerClientManager = std::make_shared<RSComposerClientManager>();
    EXPECT_FALSE(RSTunnelLayerHelper::TryCommitBufferDirect(context.node, composerClientManager, true));
    ASSERT_NE(context.surfaceHandler->GetHoldReturnValue(), nullptr);
    ASSERT_NE(context.surfaceHandler->GetHoldReturnValue()->buffer, nullptr);
    EXPECT_EQ(context.surfaceHandler->GetHoldReturnValue()->buffer->GetBufferId(),
        existingReturnValue.buffer->GetBufferId());
    EXPECT_TRUE(RSTunnelRuntimeStore::GetOrCreate(context.node->GetId()).HasPendingBuffer());

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

    auto rsListener = std::make_shared<RSRenderServiceListener>(context.node, nullptr);
    rsListener->OnBufferAvailable();

    EXPECT_EQ(context.surfaceHandler->GetBuffer(), normalBuffer);
    EXPECT_EQ(context.surfaceHandler->GetBufferOwnerCount(), normalBufferOwnerCount);
    EXPECT_EQ(context.surfaceHandler->GetHoldReturnValue(), nullptr);
    EXPECT_EQ(context.surfaceHandler->GetAvailableBufferCount(), 1);
    EXPECT_FALSE(RSTunnelRuntimeStore::GetOrCreate(context.node->GetId()).HasPendingBuffer());
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
    auto firstTunnelBufferEntry = CreateTestBufferEntry();
    ASSERT_NE(firstTunnelBufferEntry.buffer, nullptr);
    auto firstBufferId = firstTunnelBufferEntry.buffer->GetBufferId();
    RSMainThread::Instance()->directComposeHelper_.consecutiveDoCompSuccessCount_.store(TUNNEL_STABLE_THRESHOLD);
    RSTunnelRouteArbiter::RefreshGlobalTriggerSnapshot();
    RSTunnelRuntimeStore::GetOrCreate(context.node->GetId()).SetPendingBuffer(firstTunnelBufferEntry);
    context.surfaceHandler->SetAvailableBufferCount(0);

    auto rsListener = std::make_shared<RSRenderServiceListener>(context.node, composerManager);
    rsListener->OnBufferAvailable();

    EXPECT_TRUE(connection->commitTunnelCalled);
    EXPECT_EQ(connection->commitTunnelCallCount, 1u);
    EXPECT_EQ(connection->lastSurfaceId, context.consumer->GetUniqueId());
    EXPECT_EQ(connection->lastTunnelLayerId, state.tunnelLayerId);
    EXPECT_NE(connection->lastBufferId, 0u);
    EXPECT_FALSE(RSTunnelRuntimeStore::GetOrCreate(context.node->GetId()).HasPendingBuffer());
    ASSERT_NE(context.surfaceHandler->GetBuffer(), nullptr);
    EXPECT_EQ(context.surfaceHandler->GetBuffer()->GetBufferId(), firstBufferId);
    EXPECT_TRUE(RSTunnelRuntimeStore::GetOrCreate(context.node->GetId()).IsCommittedTunnelBuffer(firstBufferId));
    EXPECT_EQ(normalBufferOwnerCount->refCount_.load(), 0);
    auto firstTunnelBufferOwnerCount = context.surfaceHandler->GetBufferOwnerCount();
    ASSERT_NE(firstTunnelBufferOwnerCount, nullptr);
    EXPECT_EQ(firstTunnelBufferOwnerCount->refCount_.load(), 1);
    EXPECT_TRUE(context.surfaceHandler->IsCurrentFrameBufferConsumed());
    EXPECT_EQ(context.surfaceHandler->GetPreBuffer(), nullptr);
    EXPECT_EQ(RSTunnelRuntimeStore::GetOrCreate(context.node->GetId()).GetTunnelState(),
        RSTunnelRuntimeState::TunnelState::ACTIVE);

    constexpr uint32_t COMMIT_COUNT_AFTER_SECOND_BUFFER = 2;
    auto secondTunnelBufferEntry = CreateTestBufferEntry();
    ASSERT_NE(secondTunnelBufferEntry.buffer, nullptr);
    auto secondBufferId = secondTunnelBufferEntry.buffer->GetBufferId();
    RSTunnelRuntimeStore::GetOrCreate(context.node->GetId()).SetPendingBuffer(secondTunnelBufferEntry);
    rsListener->OnBufferAvailable();
    EXPECT_TRUE(connection->commitTunnelCalled);
    EXPECT_EQ(connection->commitTunnelCallCount, COMMIT_COUNT_AFTER_SECOND_BUFFER);
    EXPECT_NE(connection->lastBufferId, 0u);
    ASSERT_NE(context.surfaceHandler->GetBuffer(), nullptr);
    EXPECT_EQ(context.surfaceHandler->GetBuffer()->GetBufferId(), secondBufferId);
    EXPECT_TRUE(RSTunnelRuntimeStore::GetOrCreate(context.node->GetId()).IsCommittedTunnelBuffer(secondBufferId));
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

    auto rsListener = std::make_shared<RSRenderServiceListener>(context.node, composerManager);
    rsListener->OnBufferAvailable();

    EXPECT_FALSE(connection->commitTunnelCalled);
    EXPECT_FALSE(RSTunnelRuntimeStore::GetOrCreate(context.node->GetId()).HasPendingBuffer());
    EXPECT_EQ(RSTunnelRuntimeStore::GetOrCreate(context.node->GetId()).GetTunnelState(),
        RSTunnelRuntimeState::TunnelState::BUILDING);
    EXPECT_EQ(context.surfaceHandler->GetAvailableBufferCount(), 1);
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

    std::vector<LayerStateChange> results;
    ASSERT_EQ(producer->RegisterLayerStateChangedListener(
                  [&results](LayerStateChange state) { results.emplace_back(state); }),
        GSERROR_OK);
    TunnelLayerState state;
    ASSERT_TRUE(SetTunnelInfoForConsumer(consumer, state));
    RSTunnelRuntimeStore::SetLayerInfo(node->GetId(), state.tunnelLayerId, state.property);
    RSTunnelRuntimeStore::GetOrCreate(node->GetId()).SetBuilding();
    RSTunnelRuntimeStore::GetOrCreate(node->GetId()).SetActiveFromTunnelLayerAvailable(
        RSTunnelRuntimeStore::GetOrCreate(node->GetId()).GetTunnelLayerGeneration());
    ScopedRegisteredSurfaceNode registeredNode(node);
    ASSERT_TRUE(registeredNode.IsRegistered());

    auto listener = std::make_shared<RSRenderServiceListener>(node, std::make_shared<RSComposerClientManager>());
    auto pendingBufferEntry = CreateTestBufferEntry();
    ASSERT_NE(pendingBufferEntry.buffer, nullptr);
    RSTunnelRuntimeStore::GetOrCreate(node->GetId()).SetPendingBuffer(pendingBufferEntry);
    surfaceHandler->SetAvailableBufferCount(0);
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
    EXPECT_TRUE(RSTunnelRuntimeStore::GetOrCreate(node->GetId()).HasPendingBuffer());
    EXPECT_EQ(surfaceHandler->GetHoldReturnValue(), nullptr);
    EXPECT_EQ(surfaceHandler->GetAvailableBufferCount(), 1);

    ASSERT_TRUE(MoveRuntimePendingToNormalHold(node, surfaceHandler, consumer));
    EXPECT_TRUE(RSBaseSurfaceUtil::ConsumeAndUpdateBuffer(*surfaceHandler));
    ASSERT_NE(surfaceHandler->GetBuffer(), nullptr);
    EXPECT_TRUE(surfaceHandler->IsCurrentFrameBufferConsumed());
    EXPECT_EQ(surfaceHandler->GetHoldReturnValue(), nullptr);
    RSTestUtil::UnregisterConsumerListener();
}
} // namespace OHOS::Rosen
