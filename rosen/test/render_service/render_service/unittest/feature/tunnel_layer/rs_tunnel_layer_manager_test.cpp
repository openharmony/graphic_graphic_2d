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

#include "feature/tunnel_layer/rs_tunnel_layer_manager.h"
#include "gtest/gtest.h"
#include "pipeline/render_thread/rs_base_surface_util.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_test_util.h"
#include "feature/tunnel_layer/rs_tunnel_runtime_state.h"
#include "rs_tunnel_test_utils.h"
#include "iostream"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
using namespace TunnelTest;

constexpr pid_t TEST_PID_ONE = 1001;
constexpr pid_t TEST_PID_TWO = 1002;
constexpr uint32_t TEST_NODE_UID_ONE = 11;
constexpr uint32_t TEST_NODE_UID_TWO = 12;
constexpr uint32_t TEST_NODE_UID_THREE = 13;
constexpr uint32_t TEST_UNKNOWN_LAYER_STATE = 99;

class RSTunnelLayerManagerTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() override
    {
        ClearRecordingComposerLayers();
    }
    void TearDown() override
    {
        ClearRecordingComposerLayers();
        ClearTrackedTunnelRuntimeStates();
    }
};

TunnelTestContext CreateTunnelContext(NodeId nodeId = 0)
{
    TunnelTestContext context;
    RSSurfaceRenderNodeConfig config;
    if (nodeId != 0) {
        config.id = nodeId;
        config.name = "TunnelLayerManagerNode" + std::to_string(nodeId);
    }
    context.node = RSTestUtil::CreateSurfaceNode(config);
    RSTestUtil::UnregisterConsumerListener();
    if (context.node == nullptr) {
        return context;
    }
    TrackTunnelTestNode(context.node->GetId());
    context.surfaceHandler = context.node->GetMutableRSSurfaceHandler();
    if (context.surfaceHandler == nullptr) {
        return context;
    }
    context.consumer = context.surfaceHandler->GetConsumer();
    if (context.consumer == nullptr) {
        return context;
    }
    context.consumerListener = new TunnelNoopConsumerListener();
    if (context.consumer->RegisterConsumerListener(context.consumerListener) != GSERROR_OK) {
        return context;
    }
    sptr<IBufferProducer> producerToken = context.consumer->GetProducer();
    context.producer = Surface::CreateSurfaceAsProducer(producerToken);
    return context;
}

bool RegisterSurfaceNode(const std::shared_ptr<RSContext>& context, const std::shared_ptr<RSSurfaceRenderNode>& node)
{
    if (context == nullptr || node == nullptr) {
        return false;
    }
    bool registered = context->GetMutableNodeMap().RegisterRenderNode(node);
    if (registered) {
        node->SetIsOnTheTree(true);
    }
    return registered;
}

void ExpectTunnelLayerInfo(const std::shared_ptr<RSSurfaceRenderNode>& node,
    uint64_t expectedTunnelLayerId, uint32_t expectedProperty)
{
    ASSERT_NE(node, nullptr);
    uint64_t tunnelLayerId = 0;
    uint32_t property = TUNNEL_PROP_INVALID;
    RSTunnelRuntimeStore::GetLayerInfoOrDefault(node->GetId(), tunnelLayerId, property);
    EXPECT_EQ(tunnelLayerId, expectedTunnelLayerId);
    EXPECT_EQ(property, expectedProperty);
}

void ActivateTunnelRuntime(const std::shared_ptr<RSSurfaceRenderNode>& node)
{
    ASSERT_NE(node, nullptr);
    auto& runtime = RSTunnelRuntimeStore::GetOrCreate(node->GetId());
    runtime.SetBuilding();
    ASSERT_TRUE(runtime.SetActiveFromTunnelLayerAvailable(runtime.GetTunnelLayerGeneration()));
}
} // namespace

/**
 * @tc.name: TransferTunnelPendingBufferToNormalConsume001
 * @tc.desc: Test pending tunnel buffer transfer and early-return branches for normal fallback.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerManagerTest, TransferTunnelPendingBufferToNormalConsume001, TestSize.Level1)
{
    RSTunnelLayerManager nullContextManager(nullptr);
    nullContextManager.TransferTunnelPendingBufferToNormalConsume(nullptr);

    auto disabledContext = CreateTunnelContext();
    ASSERT_TRUE(disabledContext.IsProducerReady());
    ASSERT_TRUE(SetRuntimePendingBufferForTest(disabledContext));
    {
        ScopedNewTunnelSwitch scopedNewTunnelSwitch(false);
        nullContextManager.TransferTunnelPendingBufferToNormalConsume(disabledContext.node);
    }
    EXPECT_TRUE(RSTunnelRuntimeStore::GetOrCreate(disabledContext.node->GetId()).HasPendingBuffer());
    EXPECT_EQ(disabledContext.surfaceHandler->GetHoldBuffer(), nullptr);

    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    auto noConsumerContext = CreateTunnelContext();
    ASSERT_TRUE(noConsumerContext.IsProducerReady());
    noConsumerContext.surfaceHandler->SetConsumer(nullptr);
    nullContextManager.TransferTunnelPendingBufferToNormalConsume(noConsumerContext.node);

    auto occupiedNoPendingContext = CreateTunnelContext();
    ASSERT_TRUE(occupiedNoPendingContext.IsProducerReady());
    occupiedNoPendingContext.surfaceHandler->SetHoldReturnValue(CreateTestAcquireBufferReturnValue());
    occupiedNoPendingContext.surfaceHandler->SetAvailableBufferCount(0);
    nullContextManager.TransferTunnelPendingBufferToNormalConsume(occupiedNoPendingContext.node);
    EXPECT_EQ(occupiedNoPendingContext.surfaceHandler->GetAvailableBufferCount(), 0);

    auto occupiedContext = CreateTunnelContext();
    ASSERT_TRUE(occupiedContext.IsProducerReady());
    ASSERT_TRUE(SetRuntimePendingBufferForTest(occupiedContext));
    occupiedContext.surfaceHandler->SetHoldReturnValue(CreateTestAcquireBufferReturnValue());
    occupiedContext.surfaceHandler->SetAvailableBufferCount(0);
    nullContextManager.TransferTunnelPendingBufferToNormalConsume(occupiedContext.node);
    EXPECT_TRUE(RSTunnelRuntimeStore::GetOrCreate(occupiedContext.node->GetId()).HasPendingBuffer());
    EXPECT_EQ(occupiedContext.surfaceHandler->GetAvailableBufferCount(), 1);

    auto context = CreateTunnelContext();
    ASSERT_TRUE(context.IsProducerReady());
    context.surfaceHandler->SetAvailableBufferCount(1);
    nullContextManager.TransferTunnelPendingBufferToNormalConsume(context.node);
    EXPECT_EQ(context.surfaceHandler->GetAvailableBufferCount(), 1);
    EXPECT_EQ(context.surfaceHandler->GetHoldBuffer(), nullptr);

    ASSERT_TRUE(SetRuntimePendingBufferForTest(context));
    nullContextManager.TransferTunnelPendingBufferToNormalConsume(context.node);
    EXPECT_FALSE(RSTunnelRuntimeStore::GetOrCreate(context.node->GetId()).HasPendingBuffer());
    auto holdBuffer = context.surfaceHandler->GetHoldBuffer();
    ASSERT_NE(holdBuffer, nullptr);
    ASSERT_NE(holdBuffer->buffer, nullptr);
    auto holdBufferId = holdBuffer->buffer->GetBufferId();
    EXPECT_EQ(context.surfaceHandler->GetAvailableBufferCount(), 1);

    ASSERT_TRUE(RSBaseSurfaceUtil::ConsumeAndUpdateBuffer(*context.surfaceHandler));
    ASSERT_NE(context.surfaceHandler->GetBuffer(), nullptr);
    EXPECT_EQ(context.surfaceHandler->GetBuffer()->GetBufferId(), holdBufferId);
    EXPECT_TRUE(context.surfaceHandler->IsCurrentFrameBufferConsumed());
}

/**
 * @tc.name: MarkTunnelBufferConsumedForNormal001
 * @tc.desc: Test committed tunnel buffer is marked consumed only for matching ACTIVE runtime.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerManagerTest, MarkTunnelBufferConsumedForNormal001, TestSize.Level1)
{
    RSTunnelLayerManager tunnelLayerManager(nullptr);
    tunnelLayerManager.MarkTunnelBufferConsumedForNormal(nullptr, nullptr);

    auto emptyBufferContext = CreateTunnelContext();
    ASSERT_TRUE(emptyBufferContext.IsProducerReady());
    ActivateTunnelRuntime(emptyBufferContext.node);
    tunnelLayerManager.MarkTunnelBufferConsumedForNormal(emptyBufferContext.node, nullptr);
    EXPECT_FALSE(emptyBufferContext.surfaceHandler->IsCurrentFrameBufferConsumed());

    auto consumedContext = CreateTunnelContext();
    ASSERT_TRUE(consumedContext.IsProducerReady());
    auto consumedEntry = CreateTestBufferEntry();
    ASSERT_NE(consumedEntry.buffer, nullptr);
    auto consumedBufferId = consumedEntry.buffer->GetBufferId();
    consumedContext.surfaceHandler->ConsumeAndUpdateBuffer(consumedEntry);
    ActivateTunnelRuntime(consumedContext.node);
    RSTunnelRuntimeStore::GetOrCreate(consumedContext.node->GetId()).SetCommittedTunnelBufferId(consumedBufferId);
    ASSERT_TRUE(consumedContext.surfaceHandler->IsCurrentFrameBufferConsumed());
    tunnelLayerManager.MarkTunnelBufferConsumedForNormal(consumedContext.node, nullptr);
    EXPECT_TRUE(consumedContext.surfaceHandler->IsCurrentFrameBufferConsumed());
    EXPECT_FALSE(RSTunnelRuntimeStore::GetOrCreate(consumedContext.node->GetId()).IsCommittedTunnelBuffer());

    auto context = CreateTunnelContext();
    ASSERT_TRUE(context.IsProducerReady());
    auto bufferEntry = CreateTestBufferEntry();
    ASSERT_NE(bufferEntry.buffer, nullptr);
    auto bufferId = bufferEntry.buffer->GetBufferId();
    context.surfaceHandler->ConsumeAndUpdateBuffer(bufferEntry);
    context.surfaceHandler->ResetCurrentFrameBufferConsumed();

    tunnelLayerManager.MarkTunnelBufferConsumedForNormal(context.node, nullptr);
    EXPECT_FALSE(context.surfaceHandler->IsCurrentFrameBufferConsumed());

    ActivateTunnelRuntime(context.node);
    tunnelLayerManager.MarkTunnelBufferConsumedForNormal(context.node, nullptr);
    EXPECT_FALSE(context.surfaceHandler->IsCurrentFrameBufferConsumed());

    RSTunnelRuntimeStore::GetOrCreate(context.node->GetId()).SetCommittedTunnelBufferId(bufferId + 1);
    tunnelLayerManager.MarkTunnelBufferConsumedForNormal(context.node, nullptr);
    EXPECT_FALSE(RSTunnelRuntimeStore::GetOrCreate(context.node->GetId()).IsCommittedTunnelBuffer());

    RSTunnelRuntimeStore::GetOrCreate(context.node->GetId()).SetCommittedTunnelBufferId(bufferId);
    tunnelLayerManager.MarkTunnelBufferConsumedForNormal(context.node, nullptr);
    EXPECT_TRUE(context.surfaceHandler->IsCurrentFrameBufferConsumed());
    EXPECT_FALSE(RSTunnelRuntimeStore::GetOrCreate(context.node->GetId()).IsCommittedTunnelBuffer());
}

/**
 * @tc.name: UpdateTunnelLayerState001
 * @tc.desc: Test LPP branch and disabled new-tunnel reset branch.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerManagerTest, UpdateTunnelLayerState001, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    RSTunnelLayerManager tunnelLayerManager(rsContext);

    auto lppContext = CreateTunnelContext(MakeNodeId(TEST_PID_ONE, TEST_NODE_UID_ONE));
    ASSERT_TRUE(lppContext.IsProducerReady());
    ASSERT_TRUE(RegisterSurfaceNode(rsContext, lppContext.node));
    lppContext.surfaceHandler->SetSourceType(
        static_cast<uint32_t>(OHSurfaceSource::OH_SURFACE_SOURCE_LOWPOWERVIDEO));
    tunnelLayerManager.UpdateTunnelLayerState(lppContext.node->GetId(), lppContext.surfaceHandler);
    ExpectTunnelLayerInfo(lppContext.node, lppContext.consumer->GetUniqueId(),
        TUNNEL_PROP_BUFFER_ADDR | TUNNEL_PROP_DEVICE_COMMIT);
    tunnelLayerManager.UpdateTunnelLayerState(lppContext.node->GetId(), lppContext.surfaceHandler);
    ExpectTunnelLayerInfo(lppContext.node, lppContext.consumer->GetUniqueId(),
        TUNNEL_PROP_BUFFER_ADDR | TUNNEL_PROP_DEVICE_COMMIT);

    auto lppNoConsumerContext = CreateTunnelContext(MakeNodeId(TEST_PID_ONE, TEST_NODE_UID_THREE));
    ASSERT_TRUE(lppNoConsumerContext.IsProducerReady());
    ASSERT_TRUE(RegisterSurfaceNode(rsContext, lppNoConsumerContext.node));
    lppNoConsumerContext.surfaceHandler->SetSourceType(
        static_cast<uint32_t>(OHSurfaceSource::OH_SURFACE_SOURCE_LOWPOWERVIDEO));
    lppNoConsumerContext.surfaceHandler->SetConsumer(nullptr);
    RSTunnelRuntimeStore::Erase(lppNoConsumerContext.node->GetId());
    tunnelLayerManager.UpdateTunnelLayerState(lppNoConsumerContext.node->GetId(),
        lppNoConsumerContext.surfaceHandler);
    ExpectTunnelLayerInfo(lppNoConsumerContext.node, 0, TUNNEL_PROP_INVALID);

    auto disabledNoStateContext = CreateTunnelContext(MakeNodeId(TEST_PID_TWO, TEST_NODE_UID_THREE));
    ASSERT_TRUE(disabledNoStateContext.IsProducerReady());
    ASSERT_TRUE(RegisterSurfaceNode(rsContext, disabledNoStateContext.node));
    {
        ScopedNewTunnelSwitch scopedNewTunnelSwitch(false);
        tunnelLayerManager.UpdateTunnelLayerState(
            disabledNoStateContext.node->GetId(), disabledNoStateContext.surfaceHandler);
    }
    ExpectTunnelLayerInfo(disabledNoStateContext.node, 0, TUNNEL_PROP_INVALID);

    auto disabledContext = CreateTunnelContext(MakeNodeId(TEST_PID_ONE, TEST_NODE_UID_TWO));
    ASSERT_TRUE(disabledContext.IsProducerReady());
    ASSERT_TRUE(RegisterSurfaceNode(rsContext, disabledContext.node));
    ASSERT_TRUE(SetTunnelInfoForConsumer(disabledContext.consumer));
    RSTunnelRuntimeStore::SetLayerInfo(
        disabledContext.node->GetId(), disabledContext.consumer->GetUniqueId(), TUNNEL_PROP_BUFFER_ADDR);
    RSTunnelRuntimeStore::GetOrCreate(disabledContext.node->GetId()).SetBuilding();
    {
        ScopedNewTunnelSwitch scopedNewTunnelSwitch(false);
        tunnelLayerManager.UpdateTunnelLayerState(disabledContext.node->GetId(),
            disabledContext.surfaceHandler);
    }
    RSTunnelRuntimeStore::Erase(disabledContext.node->GetId());
    ExpectTunnelLayerInfo(disabledContext.node, 0, TUNNEL_PROP_INVALID);
    EXPECT_EQ(RSTunnelRuntimeStore::GetOrCreate(disabledContext.node->GetId()).GetTunnelState(),
        RSTunnelRuntimeState::TunnelState::BUILDING);
}

/**
 * @tc.name: UpdateTunnelLayerState002
 * @tc.desc: Test new-tunnel BUILDING wait and ACTIVE no-op update branches.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerManagerTest, UpdateTunnelLayerState002, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    RSTunnelLayerManager tunnelLayerManager(rsContext);
    auto newTunnelContext = CreateTunnelContext(MakeNodeId(TEST_PID_TWO, TEST_NODE_UID_ONE));
    ASSERT_TRUE(newTunnelContext.IsProducerReady());
    ASSERT_TRUE(RegisterSurfaceNode(rsContext, newTunnelContext.node));
    TunnelLayerState tunnelState;
    ASSERT_TRUE(SetTunnelInfoForConsumer(newTunnelContext.consumer, tunnelState));
    RSTunnelRuntimeStore::SetLayerInfo(
        newTunnelContext.node->GetId(), tunnelState.tunnelLayerId, tunnelState.property);
    {
        ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
        tunnelLayerManager.UpdateTunnelLayerState(newTunnelContext.node->GetId(),
            newTunnelContext.surfaceHandler);
        ExpectTunnelLayerInfo(newTunnelContext.node, tunnelState.tunnelLayerId, tunnelState.property);
        EXPECT_EQ(RSTunnelRuntimeStore::GetOrCreate(newTunnelContext.node->GetId()).GetTunnelState(),
            RSTunnelRuntimeState::TunnelState::BUILDING);

        tunnelLayerManager.UpdateTunnelLayerState(newTunnelContext.node->GetId(),
            newTunnelContext.surfaceHandler);
        EXPECT_EQ(RSTunnelRuntimeStore::GetOrCreate(newTunnelContext.node->GetId()).GetTunnelState(),
            RSTunnelRuntimeState::TunnelState::BUILDING);

        auto generation = RSTunnelRuntimeStore::GetOrCreate(newTunnelContext.node->GetId()).GetTunnelLayerGeneration();
        tunnelLayerManager.HandleLayerStateChanged(
            newTunnelContext.node->GetId(), LayerStateChange::AVAILABLE, generation);
        EXPECT_EQ(RSTunnelRuntimeStore::GetOrCreate(newTunnelContext.node->GetId()).GetTunnelState(),
            RSTunnelRuntimeState::TunnelState::ACTIVE);

        tunnelLayerManager.UpdateTunnelLayerState(newTunnelContext.node->GetId(),
            newTunnelContext.surfaceHandler);
        EXPECT_EQ(RSTunnelRuntimeStore::GetOrCreate(newTunnelContext.node->GetId()).GetTunnelState(),
            RSTunnelRuntimeState::TunnelState::ACTIVE);
    }
}

/**
 * @tc.name: UpdateTunnelLayerState003
 * @tc.desc: Test invalid new-tunnel info with and without tracked tunnel state.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerManagerTest, UpdateTunnelLayerState003, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    auto rsContext = std::make_shared<RSContext>();
    RSTunnelLayerManager tunnelLayerManager(rsContext);
    auto noStateContext = CreateTunnelContext(MakeNodeId(TEST_PID_ONE, TEST_NODE_UID_THREE));
    ASSERT_TRUE(noStateContext.IsProducerReady());
    ASSERT_TRUE(RegisterSurfaceNode(rsContext, noStateContext.node));
    tunnelLayerManager.UpdateTunnelLayerState(noStateContext.node->GetId(),
        noStateContext.surfaceHandler);
    RSTunnelRuntimeStore::Erase(noStateContext.node->GetId());
    ExpectTunnelLayerInfo(noStateContext.node, 0, TUNNEL_PROP_INVALID);
    uint64_t tunnelLayerId = 0;
    uint32_t property = TUNNEL_PROP_INVALID;
    EXPECT_FALSE(RSTunnelRuntimeStore::GetLayerInfoIfPresent(
        noStateContext.node->GetId(), tunnelLayerId, property));

    auto trackedContext = CreateTunnelContext(MakeNodeId(TEST_PID_TWO, TEST_NODE_UID_THREE));
    ASSERT_TRUE(trackedContext.IsProducerReady());
    ASSERT_TRUE(RegisterSurfaceNode(rsContext, trackedContext.node));
    TunnelLayerState tunnelState;
    ASSERT_TRUE(SetTunnelInfoForConsumer(trackedContext.consumer, tunnelState));
    tunnelLayerManager.UpdateTunnelLayerState(trackedContext.node->GetId(),
        trackedContext.surfaceHandler);

    TunnelLayerInfo emptyTunnelInfo;
    ASSERT_EQ(trackedContext.consumer->SetTunnelLayerInfo(emptyTunnelInfo), GSERROR_OK);
    tunnelLayerManager.UpdateTunnelLayerState(trackedContext.node->GetId(),
        trackedContext.surfaceHandler);

    ExpectTunnelLayerInfo(trackedContext.node, 0, TUNNEL_PROP_INVALID);
    EXPECT_EQ(RSTunnelRuntimeStore::GetOrCreate(trackedContext.node->GetId()).GetTunnelState(),
        RSTunnelRuntimeState::TunnelState::BUILDING);
}

/**
 * @tc.name: HandleLayerStateChanged001
 * @tc.desc: Test stale, unavailable, and unknown state callbacks for tracked tunnel nodes.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerManagerTest, HandleLayerStateChanged001, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    auto rsContext = std::make_shared<RSContext>();
    RSTunnelLayerManager tunnelLayerManager(rsContext);

    auto context = CreateTunnelContext(MakeNodeId(TEST_PID_TWO, TEST_NODE_UID_TWO));
    ASSERT_TRUE(context.IsProducerReady());
    ASSERT_TRUE(RegisterSurfaceNode(rsContext, context.node));

    TunnelLayerState tunnelState;
    ASSERT_TRUE(SetTunnelInfoForConsumer(context.consumer, tunnelState));
    tunnelLayerManager.UpdateTunnelLayerState(context.node->GetId(), context.surfaceHandler);
    auto generation = RSTunnelRuntimeStore::GetOrCreate(context.node->GetId()).GetTunnelLayerGeneration();

    tunnelLayerManager.HandleLayerStateChanged(context.node->GetId(), LayerStateChange::AVAILABLE,
        generation + 1);
    EXPECT_EQ(RSTunnelRuntimeStore::GetOrCreate(context.node->GetId()).GetTunnelState(),
        RSTunnelRuntimeState::TunnelState::BUILDING);

    tunnelLayerManager.HandleLayerStateChanged(context.node->GetId(), LayerStateChange::UNAVAILABLE, generation);
    ExpectTunnelLayerInfo(context.node, 0, TUNNEL_PROP_INVALID);

    ASSERT_TRUE(SetTunnelInfoForConsumer(context.consumer, tunnelState));
    tunnelLayerManager.UpdateTunnelLayerState(context.node->GetId(), context.surfaceHandler);
    generation = RSTunnelRuntimeStore::GetOrCreate(context.node->GetId()).GetTunnelLayerGeneration();
    auto unknownState = static_cast<LayerStateChange>(TEST_UNKNOWN_LAYER_STATE);
    tunnelLayerManager.HandleLayerStateChanged(context.node->GetId(), unknownState, generation);
    ExpectTunnelLayerInfo(context.node, 0, TUNNEL_PROP_INVALID);

    tunnelLayerManager.HandleLayerStateChanged(MakeNodeId(TEST_PID_TWO, TEST_NODE_UID_THREE),
        LayerStateChange::AVAILABLE, 0);
}

/**
 * @tc.name: ClearRuntimeStateByPid001
 * @tc.desc: Test runtime clear only affects surface nodes from the requested pid.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerManagerTest, ClearRuntimeStateByPid001, TestSize.Level1)
{
    RSTunnelLayerManager nullContextManager(nullptr);
    nullContextManager.ClearRuntimeStateByPid(TEST_PID_ONE);

    auto rsContext = std::make_shared<RSContext>();
    RSTunnelLayerManager tunnelLayerManager(rsContext);
    auto firstContext = CreateTunnelContext(MakeNodeId(TEST_PID_ONE, TEST_NODE_UID_THREE));
    auto secondContext = CreateTunnelContext(MakeNodeId(TEST_PID_TWO, TEST_NODE_UID_THREE));
    ASSERT_TRUE(firstContext.IsProducerReady());
    ASSERT_TRUE(secondContext.IsProducerReady());
    ASSERT_TRUE(RegisterSurfaceNode(rsContext, firstContext.node));
    ASSERT_TRUE(RegisterSurfaceNode(rsContext, secondContext.node));
    RSTunnelRuntimeStore::SetLayerInfo(
        firstContext.node->GetId(), firstContext.consumer->GetUniqueId(), TUNNEL_PROP_BUFFER_ADDR);
    RSTunnelRuntimeStore::SetLayerInfo(
        secondContext.node->GetId(), secondContext.consumer->GetUniqueId(), TUNNEL_PROP_BUFFER_ADDR);
    ActivateTunnelRuntime(firstContext.node);
    ActivateTunnelRuntime(secondContext.node);

    tunnelLayerManager.ClearRuntimeStateByPid(TEST_PID_ONE);

    ExpectTunnelLayerInfo(firstContext.node, 0, TUNNEL_PROP_INVALID);
    EXPECT_EQ(RSTunnelRuntimeStore::GetOrCreate(firstContext.node->GetId()).GetTunnelState(),
        RSTunnelRuntimeState::TunnelState::BUILDING);
    ExpectTunnelLayerInfo(secondContext.node, secondContext.consumer->GetUniqueId(), TUNNEL_PROP_BUFFER_ADDR);
    EXPECT_EQ(RSTunnelRuntimeStore::GetOrCreate(secondContext.node->GetId()).GetTunnelState(),
        RSTunnelRuntimeState::TunnelState::ACTIVE);
}
} // namespace OHOS::Rosen
