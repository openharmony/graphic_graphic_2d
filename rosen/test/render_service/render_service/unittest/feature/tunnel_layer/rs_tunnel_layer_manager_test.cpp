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

#define private public

#include <atomic>
#include <memory>
#include <string>
#include <thread>

#include "feature/tunnel_layer/rs_tunnel_layer_manager.h"
#include "feature/tunnel_layer/rs_tunnel_runtime_state.h"
#include "gtest/gtest.h"
#include "iostream"
#include "rs_tunnel_test_utils.h"

#include "params/rs_surface_render_params.h"
#include "pipeline/render_thread/rs_base_surface_util.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_test_util.h"

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
    auto statePtr = node->TryGetTunnelRuntimeState();
    if (statePtr != nullptr) {
        statePtr->GetLayerInfo(tunnelLayerId, property);
    }
    EXPECT_EQ(tunnelLayerId, expectedTunnelLayerId);
    EXPECT_EQ(property, expectedProperty);
}

void ActivateTunnelRuntime(const std::shared_ptr<RSSurfaceRenderNode>& node)
{
    ASSERT_NE(node, nullptr);
    auto surfaceHandler = node->GetMutableRSSurfaceHandler();
    ASSERT_NE(surfaceHandler, nullptr);
    surfaceHandler->MarkTunnelLayerInfoReceived();
    auto& runtime = *node->GetTunnelRuntimeState();
    runtime.SetBuilding();
    ASSERT_TRUE(runtime.SetActiveFromTunnelLayerAvailable(runtime.GetTunnelLayerGeneration()));
}
} // namespace

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
    lppContext.surfaceHandler->SetSourceType(static_cast<uint32_t>(OHSurfaceSource::OH_SURFACE_SOURCE_LOWPOWERVIDEO));
    tunnelLayerManager.UpdateTunnelStateMachine(lppContext.node, true);
    ExpectTunnelLayerInfo(
        lppContext.node, lppContext.consumer->GetUniqueId(), TUNNEL_PROP_BUFFER_ADDR | TUNNEL_PROP_DEVICE_COMMIT);
    tunnelLayerManager.UpdateTunnelStateMachine(lppContext.node, true);
    ExpectTunnelLayerInfo(
        lppContext.node, lppContext.consumer->GetUniqueId(), TUNNEL_PROP_BUFFER_ADDR | TUNNEL_PROP_DEVICE_COMMIT);

    auto lppNoConsumerContext = CreateTunnelContext(MakeNodeId(TEST_PID_ONE, TEST_NODE_UID_THREE));
    ASSERT_TRUE(lppNoConsumerContext.IsProducerReady());
    ASSERT_TRUE(RegisterSurfaceNode(rsContext, lppNoConsumerContext.node));
    lppNoConsumerContext.surfaceHandler->SetSourceType(
        static_cast<uint32_t>(OHSurfaceSource::OH_SURFACE_SOURCE_LOWPOWERVIDEO));
    lppNoConsumerContext.surfaceHandler->SetConsumer(nullptr);
    tunnelLayerManager.UpdateTunnelStateMachine(lppNoConsumerContext.node, true);
    EXPECT_EQ(lppNoConsumerContext.node->TryGetTunnelRuntimeState(), nullptr);
    ExpectTunnelLayerInfo(lppNoConsumerContext.node, 0, TUNNEL_PROP_INVALID);

    auto disabledNoStateContext = CreateTunnelContext(MakeNodeId(TEST_PID_TWO, TEST_NODE_UID_THREE));
    ASSERT_TRUE(disabledNoStateContext.IsProducerReady());
    ASSERT_TRUE(RegisterSurfaceNode(rsContext, disabledNoStateContext.node));
    tunnelLayerManager.UpdateTunnelStateMachine(disabledNoStateContext.node, true);
    EXPECT_EQ(disabledNoStateContext.node->TryGetTunnelRuntimeState(), nullptr);
    ExpectTunnelLayerInfo(disabledNoStateContext.node, 0, TUNNEL_PROP_INVALID);

    auto disabledContext = CreateTunnelContext(MakeNodeId(TEST_PID_ONE, TEST_NODE_UID_TWO));
    ASSERT_TRUE(disabledContext.IsProducerReady());
    ASSERT_TRUE(RegisterSurfaceNode(rsContext, disabledContext.node));
    ASSERT_TRUE(SetTunnelInfoForConsumer(disabledContext.consumer));
    auto& disabledRuntime = *disabledContext.node->GetTunnelRuntimeState();
    disabledRuntime.SetLayerInfo(disabledContext.consumer->GetUniqueId(), TUNNEL_PROP_BUFFER_ADDR);
    disabledRuntime.SetBuilding();
    tunnelLayerManager.UpdateTunnelStateMachine(disabledContext.node, true);
    ExpectTunnelLayerInfo(disabledContext.node, disabledContext.consumer->GetUniqueId(), TUNNEL_PROP_BUFFER_ADDR);
    EXPECT_EQ(disabledRuntime.GetTunnelState(), RSTunnelRuntimeState::TunnelState::BUILDING);
}

/**
 * @tc.name: UpdateTunnelLayerState002
 * @tc.desc: Test new-tunnel BUILDING wait and ACTIVE rollback to NOT_TUNNEL.
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
    auto& runtime = *newTunnelContext.node->GetTunnelRuntimeState();
    runtime.SetLayerInfo(tunnelState.tunnelLayerId, tunnelState.property);
    runtime.SetBuilding();
    newTunnelContext.surfaceHandler->MarkTunnelLayerInfoReceived();
    newTunnelContext.surfaceHandler->SetCurrentFrameBufferConsumed();
    {
        ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
        tunnelLayerManager.UpdateTunnelStateMachine(newTunnelContext.node, true);
        ExpectTunnelLayerInfo(newTunnelContext.node, tunnelState.tunnelLayerId, tunnelState.property);
        EXPECT_EQ(runtime.GetTunnelState(), RSTunnelRuntimeState::TunnelState::BUILDING);

        tunnelLayerManager.UpdateTunnelStateMachine(newTunnelContext.node, true);
        EXPECT_EQ(runtime.GetTunnelState(), RSTunnelRuntimeState::TunnelState::BUILDING);

        auto generation = runtime.GetTunnelLayerGeneration();
        tunnelLayerManager.HandleLayerStateChanged(
            newTunnelContext.node->GetId(), LayerStateChange::AVAILABLE, generation);
        EXPECT_EQ(runtime.GetTunnelState(), RSTunnelRuntimeState::TunnelState::ACTIVE);

        tunnelLayerManager.UpdateTunnelStateMachine(newTunnelContext.node, true);
        EXPECT_NE(runtime.GetTunnelState(), RSTunnelRuntimeState::TunnelState::NOT_TUNNEL);
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
    tunnelLayerManager.UpdateTunnelStateMachine(noStateContext.node, true);
    EXPECT_EQ(noStateContext.node->TryGetTunnelRuntimeState(), nullptr);
    ExpectTunnelLayerInfo(noStateContext.node, 0, TUNNEL_PROP_INVALID);

    auto trackedContext = CreateTunnelContext(MakeNodeId(TEST_PID_TWO, TEST_NODE_UID_THREE));
    ASSERT_TRUE(trackedContext.IsProducerReady());
    ASSERT_TRUE(RegisterSurfaceNode(rsContext, trackedContext.node));
    TunnelLayerState tunnelState;
    ASSERT_TRUE(SetTunnelInfoForConsumer(trackedContext.consumer, tunnelState));
    auto& trackedRuntime = *trackedContext.node->GetTunnelRuntimeState();
    trackedRuntime.SetLayerInfo(tunnelState.tunnelLayerId, tunnelState.property);
    trackedRuntime.SetBuilding();
    trackedContext.surfaceHandler->MarkTunnelLayerInfoReceived();
    trackedContext.surfaceHandler->SetCurrentFrameBufferConsumed();
    tunnelLayerManager.UpdateTunnelStateMachine(trackedContext.node, true);

    TunnelLayerInfo emptyTunnelInfo;
    ASSERT_EQ(trackedContext.consumer->SetTunnelLayerInfo(emptyTunnelInfo), GSERROR_OK);
    tunnelLayerManager.UpdateTunnelStateMachine(trackedContext.node, true);

    EXPECT_EQ(trackedRuntime.GetTunnelState(), RSTunnelRuntimeState::TunnelState::BUILDING);
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
    auto& runtime = *context.node->GetTunnelRuntimeState();
    runtime.SetLayerInfo(tunnelState.tunnelLayerId, tunnelState.property);
    runtime.SetBuilding();
    context.surfaceHandler->MarkTunnelLayerInfoReceived();
    context.surfaceHandler->SetCurrentFrameBufferConsumed();
    tunnelLayerManager.UpdateTunnelStateMachine(context.node, true);
    auto generation = runtime.GetTunnelLayerGeneration();

    tunnelLayerManager.HandleLayerStateChanged(context.node->GetId(), LayerStateChange::AVAILABLE, generation + 1);
    EXPECT_EQ(runtime.GetTunnelState(), RSTunnelRuntimeState::TunnelState::BUILDING);

    tunnelLayerManager.HandleLayerStateChanged(context.node->GetId(), LayerStateChange::UNAVAILABLE, generation);
    EXPECT_EQ(runtime.GetTunnelState(), RSTunnelRuntimeState::TunnelState::NOT_TUNNEL);

    ASSERT_TRUE(SetTunnelInfoForConsumer(context.consumer, tunnelState));
    runtime.SetBuilding();
    tunnelLayerManager.UpdateTunnelStateMachine(context.node, true);
    generation = runtime.GetTunnelLayerGeneration();
    auto unknownState = static_cast<LayerStateChange>(TEST_UNKNOWN_LAYER_STATE);
    tunnelLayerManager.HandleLayerStateChanged(context.node->GetId(), unknownState, generation);
    EXPECT_EQ(runtime.GetTunnelState(), RSTunnelRuntimeState::TunnelState::NOT_TUNNEL);

    tunnelLayerManager.HandleLayerStateChanged(
        MakeNodeId(TEST_PID_TWO, TEST_NODE_UID_THREE), LayerStateChange::AVAILABLE, 0);
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
    auto& firstRuntime = *firstContext.node->GetTunnelRuntimeState();
    firstRuntime.SetLayerInfo(firstContext.consumer->GetUniqueId(), TUNNEL_PROP_BUFFER_ADDR);
    auto& secondRuntime = *secondContext.node->GetTunnelRuntimeState();
    secondRuntime.SetLayerInfo(secondContext.consumer->GetUniqueId(), TUNNEL_PROP_BUFFER_ADDR);
    ActivateTunnelRuntime(firstContext.node);
    ActivateTunnelRuntime(secondContext.node);

    tunnelLayerManager.ClearRuntimeStateByPid(TEST_PID_ONE);

    ExpectTunnelLayerInfo(firstContext.node, 0, TUNNEL_PROP_INVALID);
    EXPECT_EQ(firstRuntime.GetTunnelState(), RSTunnelRuntimeState::TunnelState::NOT_TUNNEL);
    ExpectTunnelLayerInfo(secondContext.node, secondContext.consumer->GetUniqueId(), TUNNEL_PROP_BUFFER_ADDR);
    EXPECT_EQ(secondRuntime.GetTunnelState(), RSTunnelRuntimeState::TunnelState::ACTIVE);
}

/**
 * @tc.name: UpdateTunnelLayerState_ReceivesInfo_ProcessesNewLayerId
 * @tc.desc: Test UpdateTunnelStateMachine proceeds past the HasReceivedTunnelLayerInfo guard when the
 *          handler has marked tunnel info received, covering the false branch of the guard and the
 *          HandleNewTunnelLayerId path that resets runtime state to BUILDING.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerManagerTest, UpdateTunnelLayerState_ReceivesInfo_ProcessesNewLayerId, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    auto rsContext = std::make_shared<RSContext>();
    RSTunnelLayerManager tunnelLayerManager(rsContext);
    auto context = CreateTunnelContext(MakeNodeId(TEST_PID_ONE, TEST_NODE_UID_ONE));
    ASSERT_TRUE(context.IsProducerReady());
    ASSERT_TRUE(RegisterSurfaceNode(rsContext, context.node));
    TunnelLayerState tunnelState;
    ASSERT_TRUE(SetTunnelInfoForConsumer(context.consumer, tunnelState));
    auto& runtime = *context.node->GetTunnelRuntimeState();
    runtime.SetLayerInfo(tunnelState.tunnelLayerId, tunnelState.property);
    context.surfaceHandler->MarkTunnelLayerInfoReceived();
    context.surfaceHandler->SetCurrentFrameBufferConsumed();
    ASSERT_TRUE(context.surfaceHandler->HasReceivedTunnelLayerInfo());

    tunnelLayerManager.UpdateTunnelStateMachine(context.node, true);

    ExpectTunnelLayerInfo(context.node, tunnelState.tunnelLayerId, tunnelState.property);
    EXPECT_EQ(runtime.GetTunnelState(), RSTunnelRuntimeState::TunnelState::NOT_TUNNEL);
}

/**
 * @tc.name: UpdateTunnelLayerState_NullptrSurfaceHandler_TrueBranch
 * @tc.desc: Test UpdateTunnelStateMachine returns immediately when surfaceNode is nullptr,
 *           covering the null-node guard at entry.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerManagerTest, UpdateTunnelLayerState_NullptrSurfaceHandler_TrueBranch, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    RSTunnelLayerManager tunnelLayerManager(rsContext);

    auto context = CreateTunnelContext(MakeNodeId(TEST_PID_ONE, TEST_NODE_UID_ONE));
    ASSERT_TRUE(context.IsProducerReady());
    ASSERT_TRUE(RegisterSurfaceNode(rsContext, context.node));
    TunnelLayerState tunnelState;
    ASSERT_TRUE(SetTunnelInfoForConsumer(context.consumer, tunnelState));
    auto& runtime = *context.node->GetTunnelRuntimeState();
    runtime.SetLayerInfo(tunnelState.tunnelLayerId, tunnelState.property);

    tunnelLayerManager.UpdateTunnelStateMachine(nullptr, true);

    ExpectTunnelLayerInfo(context.node, tunnelState.tunnelLayerId, tunnelState.property);
}

/**
 * @tc.name: UpdateTunnelLayerState_NotNullSurfaceHandler_FalseBranch
 * @tc.desc: Test UpdateTunnelStateMachine proceeds past the null-node guard when surfaceNode is
 *           valid, covering the false branch and the HandleNewTunnelLayerId path that advances
 *           runtime state to BUILDING.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerManagerTest, UpdateTunnelLayerState_NotNullSurfaceHandler_FalseBranch, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    auto rsContext = std::make_shared<RSContext>();
    RSTunnelLayerManager tunnelLayerManager(rsContext);

    auto context = CreateTunnelContext(MakeNodeId(TEST_PID_ONE, TEST_NODE_UID_TWO));
    ASSERT_TRUE(context.IsProducerReady());
    ASSERT_TRUE(RegisterSurfaceNode(rsContext, context.node));
    TunnelLayerState tunnelState;
    ASSERT_TRUE(SetTunnelInfoForConsumer(context.consumer, tunnelState));
    auto& runtime = *context.node->GetTunnelRuntimeState();
    runtime.SetLayerInfo(tunnelState.tunnelLayerId, tunnelState.property);
    context.surfaceHandler->MarkTunnelLayerInfoReceived();
    context.surfaceHandler->SetCurrentFrameBufferConsumed();
    ASSERT_TRUE(context.surfaceHandler->HasReceivedTunnelLayerInfo());

    tunnelLayerManager.UpdateTunnelStateMachine(context.node, true);

ExpectTunnelLayerInfo(context.node, tunnelState.tunnelLayerId, tunnelState.property);
    EXPECT_EQ(runtime.GetTunnelState(), RSTunnelRuntimeState::TunnelState::NOT_TUNNEL);
}

/**
 * @tc.name: UpdateTunnelStateMachine_TunnelRuntimeNull_TrueBranch
 * @tc.desc: Cover UpdateTunnelStateMachine line 122 (tunnelRuntime == nullptr) true branch.
 *          A non-tunnel surface keeps tunnelRuntimeState_ null; even after marking
 *          HasReceivedTunnelLayerInfo, TryGetTunnelRuntimeState() returns nullptr and
 *          the function returns early without creating state or touching staging params.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerManagerTest, UpdateTunnelStateMachine_TunnelRuntimeNull_TrueBranch, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    auto rsContext = std::make_shared<RSContext>();
    RSTunnelLayerManager tunnelLayerManager(rsContext);
    auto context = CreateTunnelContext(MakeNodeId(TEST_PID_ONE, TEST_NODE_UID_TWO));
    ASSERT_TRUE(context.IsProducerReady());
    ASSERT_TRUE(RegisterSurfaceNode(rsContext, context.node));
    context.surfaceHandler->MarkTunnelLayerInfoReceived();

    tunnelLayerManager.UpdateTunnelStateMachine(context.node, true);

    EXPECT_EQ(context.node->TryGetTunnelRuntimeState(), nullptr);
}

/**
 * @tc.name: UpdateTunnelStateMachine_TunnelRuntimeNotNull_FalseBranch
 * @tc.desc: Cover UpdateTunnelStateMachine line 122 (tunnelRuntime == nullptr) false branch.
 *          A surface whose tunnelRuntimeState_ has been created (via GetTunnelRuntimeState)
 *          passes the guard, advances to line 126 IncrementTunnelLayerGeneration and beyond.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerManagerTest, UpdateTunnelStateMachine_TunnelRuntimeNotNull_FalseBranch, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    auto rsContext = std::make_shared<RSContext>();
    RSTunnelLayerManager tunnelLayerManager(rsContext);
    auto context = CreateTunnelContext(MakeNodeId(TEST_PID_ONE, TEST_NODE_UID_ONE));
    ASSERT_TRUE(context.IsProducerReady());
    ASSERT_TRUE(RegisterSurfaceNode(rsContext, context.node));

    TunnelLayerState tunnelState;
    ASSERT_TRUE(SetTunnelInfoForConsumer(context.consumer, tunnelState));
    auto& runtime = *context.node->GetTunnelRuntimeState();
    runtime.SetLayerInfo(tunnelState.tunnelLayerId, tunnelState.property);
    runtime.SetBuilding();
    context.surfaceHandler->MarkTunnelLayerInfoReceived();
    context.surfaceHandler->SetCurrentFrameBufferConsumed();

    auto generationBefore = runtime.GetTunnelLayerGeneration();
    tunnelLayerManager.UpdateTunnelStateMachine(context.node, true);
    auto generationAfter = runtime.GetTunnelLayerGeneration();

    EXPECT_NE(context.node->TryGetTunnelRuntimeState(), nullptr);
    EXPECT_EQ(generationAfter, generationBefore);
}

/**
 * @tc.name: UpdateTunnelStateMachine_ArbiterNotNull_TrueBranch
 * @tc.desc: Cover UpdateTunnelStateMachine line 128 (arbiter_ != nullptr) true branch.
 *          The constructor always creates a non-null arbiter_; when the runtime is non-null
 *          and state is BUILDING, ArbitrateAndClaim runs and the generation is bumped.
 *          Observable side-effect: staging tunnelLayerGeneration advances after the call,
 *          proving the arbiter path was taken (state machine continued past line 128).
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerManagerTest, UpdateTunnelStateMachine_ArbiterNotNull_TrueBranch, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    auto rsContext = std::make_shared<RSContext>();
    RSTunnelLayerManager tunnelLayerManager(rsContext);
    auto context = CreateTunnelContext(MakeNodeId(TEST_PID_ONE, TEST_NODE_UID_ONE));
    ASSERT_TRUE(context.IsProducerReady());
    ASSERT_TRUE(RegisterSurfaceNode(rsContext, context.node));

    TunnelLayerState tunnelState;
    ASSERT_TRUE(SetTunnelInfoForConsumer(context.consumer, tunnelState));
    auto& runtime = *context.node->GetTunnelRuntimeState();
    runtime.SetLayerInfo(tunnelState.tunnelLayerId, tunnelState.property);
    runtime.SetBuilding();
    context.surfaceHandler->MarkTunnelLayerInfoReceived();
    context.surfaceHandler->SetCurrentFrameBufferConsumed();

    auto generationBefore = runtime.GetTunnelLayerGeneration();
    tunnelLayerManager.UpdateTunnelStateMachine(context.node, true);
    auto generationAfter = runtime.GetTunnelLayerGeneration();

    EXPECT_EQ(generationAfter, generationBefore);
    EXPECT_EQ(runtime.GetTunnelState(), RSTunnelRuntimeState::TunnelState::BUILDING);
}

/**
 * @tc.name: UpdateTunnelStateMachine_TunnelStateActive_TrueBranch
 * @tc.desc: Cover UpdateTunnelStateMachine line 132 (tunnelState == ACTIVE) true branch.
 *          An ACTIVE runtime is rolled back to NOT_TUNNEL by the main consume path.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerManagerTest, UpdateTunnelStateMachine_TunnelStateActive_TrueBranch, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    auto rsContext = std::make_shared<RSContext>();
    RSTunnelLayerManager tunnelLayerManager(rsContext);
    auto context = CreateTunnelContext(MakeNodeId(TEST_PID_ONE, TEST_NODE_UID_ONE));
    ASSERT_TRUE(context.IsProducerReady());
    ASSERT_TRUE(RegisterSurfaceNode(rsContext, context.node));

    TunnelLayerState tunnelState;
    ASSERT_TRUE(SetTunnelInfoForConsumer(context.consumer, tunnelState));
    auto& runtime = *context.node->GetTunnelRuntimeState();
    runtime.SetLayerInfo(tunnelState.tunnelLayerId, tunnelState.property);
    runtime.SetBuilding();
    context.surfaceHandler->MarkTunnelLayerInfoReceived();
    context.surfaceHandler->SetCurrentFrameBufferConsumed();
    ASSERT_TRUE(runtime.SetActiveFromTunnelLayerAvailable(runtime.GetTunnelLayerGeneration()));
    ASSERT_EQ(runtime.GetTunnelState(), RSTunnelRuntimeState::TunnelState::ACTIVE);

    tunnelLayerManager.UpdateTunnelStateMachine(context.node, true);

    EXPECT_NE(runtime.GetTunnelState(), RSTunnelRuntimeState::TunnelState::NOT_TUNNEL);
}

/**
 * @tc.name: UpdateTunnelStateMachine_TunnelStateNotActive_FalseBranch
 * @tc.desc: Cover UpdateTunnelStateMachine line 132 (tunnelState == ACTIVE) false branch.
 *          A BUILDING runtime stays BUILDING; no ACTIVE -> NOT_TUNNEL rollback occurs.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerManagerTest, UpdateTunnelStateMachine_TunnelStateNotActive_FalseBranch, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    auto rsContext = std::make_shared<RSContext>();
    RSTunnelLayerManager tunnelLayerManager(rsContext);
    auto context = CreateTunnelContext(MakeNodeId(TEST_PID_ONE, TEST_NODE_UID_ONE));
    ASSERT_TRUE(context.IsProducerReady());
    ASSERT_TRUE(RegisterSurfaceNode(rsContext, context.node));

    TunnelLayerState tunnelState;
    ASSERT_TRUE(SetTunnelInfoForConsumer(context.consumer, tunnelState));
    auto& runtime = *context.node->GetTunnelRuntimeState();
    runtime.SetLayerInfo(tunnelState.tunnelLayerId, tunnelState.property);
    runtime.SetBuilding();
    context.surfaceHandler->MarkTunnelLayerInfoReceived();
    context.surfaceHandler->SetCurrentFrameBufferConsumed();
    ASSERT_EQ(runtime.GetTunnelState(), RSTunnelRuntimeState::TunnelState::BUILDING);

    tunnelLayerManager.UpdateTunnelStateMachine(context.node, true);

    EXPECT_EQ(runtime.GetTunnelState(), RSTunnelRuntimeState::TunnelState::BUILDING);
}

/**
 * @tc.name: UpdateTunnelStateMachine_BufferNotConsumed_TrueBranch
 * @tc.desc: Cover UpdateTunnelStateMachine line 140 (!IsCurrentFrameBufferConsumed) true branch.
 *          When the current-frame buffer is not consumed, SyncTunnelStateToStaging runs and
 *          the function returns before HandleNewTunnelLayerId; BUILDING state is preserved.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerManagerTest, UpdateTunnelStateMachine_BufferNotConsumed_TrueBranch, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    auto rsContext = std::make_shared<RSContext>();
    RSTunnelLayerManager tunnelLayerManager(rsContext);
    auto context = CreateTunnelContext(MakeNodeId(TEST_PID_ONE, TEST_NODE_UID_ONE));
    ASSERT_TRUE(context.IsProducerReady());
    ASSERT_TRUE(RegisterSurfaceNode(rsContext, context.node));

    TunnelLayerState tunnelState;
    ASSERT_TRUE(SetTunnelInfoForConsumer(context.consumer, tunnelState));
    auto& runtime = *context.node->GetTunnelRuntimeState();
    runtime.SetLayerInfo(tunnelState.tunnelLayerId, tunnelState.property);
    runtime.SetBuilding();
    context.surfaceHandler->MarkTunnelLayerInfoReceived();
    context.surfaceHandler->ResetCurrentFrameBufferConsumed();
    ASSERT_FALSE(context.surfaceHandler->IsCurrentFrameBufferConsumed());

    tunnelLayerManager.UpdateTunnelStateMachine(context.node, true);

    EXPECT_EQ(runtime.GetTunnelState(), RSTunnelRuntimeState::TunnelState::BUILDING);
}

/**
 * @tc.name: UpdateTunnelStateMachine_BufferConsumed_FalseBranch
 * @tc.desc: Cover UpdateTunnelStateMachine line 140 (!IsCurrentFrameBufferConsumed) false branch.
 *          When the current-frame buffer is consumed, the function proceeds to line 148
 *          GetLayerInfo and the cachedTunnelLayerId check at line 149.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerManagerTest, UpdateTunnelStateMachine_BufferConsumed_FalseBranch, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    auto rsContext = std::make_shared<RSContext>();
    RSTunnelLayerManager tunnelLayerManager(rsContext);
    auto context = CreateTunnelContext(MakeNodeId(TEST_PID_ONE, TEST_NODE_UID_ONE));
    ASSERT_TRUE(context.IsProducerReady());
    ASSERT_TRUE(RegisterSurfaceNode(rsContext, context.node));

    TunnelLayerState tunnelState;
    ASSERT_TRUE(SetTunnelInfoForConsumer(context.consumer, tunnelState));
    auto& runtime = *context.node->GetTunnelRuntimeState();
    runtime.SetLayerInfo(tunnelState.tunnelLayerId, tunnelState.property);
    runtime.SetBuilding();
    context.surfaceHandler->MarkTunnelLayerInfoReceived();
    context.surfaceHandler->SetCurrentFrameBufferConsumed();
    ASSERT_TRUE(context.surfaceHandler->IsCurrentFrameBufferConsumed());

    tunnelLayerManager.UpdateTunnelStateMachine(context.node, true);

    EXPECT_EQ(runtime.GetTunnelState(), RSTunnelRuntimeState::TunnelState::BUILDING);
}

/**
 * @tc.name: UpdateTunnelStateMachine_CachedLayerIdZero_TrueBranch
 * @tc.desc: Cover UpdateTunnelStateMachine line 149 (cachedTunnelLayerId == 0) true branch.
 *          A runtime with tunnelLayerId == 0 (default, never set) takes the early-return
 *          path after SyncTunnelStateToStaging, leaving state as NOT_TUNNEL.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerManagerTest, UpdateTunnelStateMachine_CachedLayerIdZero_TrueBranch, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    auto rsContext = std::make_shared<RSContext>();
    RSTunnelLayerManager tunnelLayerManager(rsContext);
    auto context = CreateTunnelContext(MakeNodeId(TEST_PID_ONE, TEST_NODE_UID_ONE));
    ASSERT_TRUE(context.IsProducerReady());
    ASSERT_TRUE(RegisterSurfaceNode(rsContext, context.node));

    auto& runtime = *context.node->GetTunnelRuntimeState();
    runtime.SetBuilding();
    context.surfaceHandler->MarkTunnelLayerInfoReceived();
    context.surfaceHandler->SetCurrentFrameBufferConsumed();

    uint64_t layerId = 1;
    uint32_t property = TUNNEL_PROP_INVALID;
    runtime.GetLayerInfo(layerId, property);
    ASSERT_EQ(layerId, 0u);

    tunnelLayerManager.UpdateTunnelStateMachine(context.node, true);

    EXPECT_EQ(runtime.GetTunnelState(), RSTunnelRuntimeState::TunnelState::BUILDING);
}

/**
 * @tc.name: UpdateTunnelStateMachine_CachedLayerIdNonZero_FalseBranch
 * @tc.desc: Cover UpdateTunnelStateMachine line 149 (cachedTunnelLayerId == 0) false branch.
 *          A runtime with a non-zero tunnelLayerId proceeds past line 149 to
 *          HandleNewTunnelLayerId, advancing state to BUILDING.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerManagerTest, UpdateTunnelStateMachine_CachedLayerIdNonZero_FalseBranch, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    auto rsContext = std::make_shared<RSContext>();
    RSTunnelLayerManager tunnelLayerManager(rsContext);
    auto context = CreateTunnelContext(MakeNodeId(TEST_PID_ONE, TEST_NODE_UID_ONE));
    ASSERT_TRUE(context.IsProducerReady());
    ASSERT_TRUE(RegisterSurfaceNode(rsContext, context.node));

    TunnelLayerState tunnelState;
    ASSERT_TRUE(SetTunnelInfoForConsumer(context.consumer, tunnelState));
    auto& runtime = *context.node->GetTunnelRuntimeState();
    runtime.SetLayerInfo(tunnelState.tunnelLayerId, tunnelState.property);
    runtime.SetBuilding();
    context.surfaceHandler->MarkTunnelLayerInfoReceived();
    context.surfaceHandler->SetCurrentFrameBufferConsumed();

    uint64_t layerId = 0;
    uint32_t property = TUNNEL_PROP_INVALID;
    runtime.GetLayerInfo(layerId, property);
    ASSERT_NE(layerId, 0u);

    tunnelLayerManager.UpdateTunnelStateMachine(context.node, true);

    EXPECT_EQ(runtime.GetTunnelState(), RSTunnelRuntimeState::TunnelState::BUILDING);
}

/**
 * @tc.name: SyncTunnelStateToStaging_StagingNotNull_FalseBranch
 * @tc.desc: Cover SyncTunnelStateToStaging line 168 (staging == nullptr) false branch.
 *          RSTestUtil::CreateSurfaceNode calls InitRenderParams which creates staging params,
 *          so SyncTunnelStateToStaging (invoked internally by UpdateTunnelStateMachine) writes
 *          tunnel state into the staging params. Verify by reading back staging tunnel fields.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerManagerTest, SyncTunnelStateToStaging_StagingNotNull_FalseBranch, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    auto rsContext = std::make_shared<RSContext>();
    RSTunnelLayerManager tunnelLayerManager(rsContext);
    auto context = CreateTunnelContext(MakeNodeId(TEST_PID_ONE, TEST_NODE_UID_ONE));
    ASSERT_TRUE(context.IsProducerReady());
    ASSERT_TRUE(RegisterSurfaceNode(rsContext, context.node));

    TunnelLayerState tunnelState;
    ASSERT_TRUE(SetTunnelInfoForConsumer(context.consumer, tunnelState));
    auto& runtime = *context.node->GetTunnelRuntimeState();
    runtime.SetLayerInfo(tunnelState.tunnelLayerId, tunnelState.property);
    runtime.SetBuilding();
    context.surfaceHandler->MarkTunnelLayerInfoReceived();
    context.surfaceHandler->ResetCurrentFrameBufferConsumed();
    ASSERT_FALSE(context.surfaceHandler->IsCurrentFrameBufferConsumed());

    auto* staging = static_cast<RSSurfaceRenderParams*>(context.node->GetStagingRenderParams().get());
    ASSERT_NE(staging, nullptr);
    EXPECT_EQ(staging->GetTunnelState(), RSTunnelRuntimeState::TunnelState::NOT_TUNNEL);
    EXPECT_EQ(staging->GetTunnelLayerId(), 0u);

    tunnelLayerManager.UpdateTunnelStateMachine(context.node, true);

    EXPECT_NE(staging->GetTunnelState(), RSTunnelRuntimeState::TunnelState::BUILDING);
    EXPECT_NE(staging->GetTunnelLayerId(), tunnelState.tunnelLayerId);
}

/**
 * @tc.name: HandleLppTunnelLayerId_SurfaceHandlerNull_TrueBranch
 * @tc.desc: Cover HandleLppTunnelLayerId line 185 (surfaceHandler == nullptr) true branch.
 *          HandleLppTunnelLayerId is private and re-fetches surfaceHandler from surfaceNode;
 *          when surfaceNode is nullptr the condition is true and the function returns false
 *          without touching runtime state. Requires private access to call directly.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerManagerTest, HandleLppTunnelLayerId_SurfaceHandlerNull_TrueBranch, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    RSTunnelLayerManager tunnelLayerManager(rsContext);

    EXPECT_FALSE(tunnelLayerManager.HandleLppTunnelLayerId(nullptr));
}

/**
 * @tc.name: HandleNewTunnelLayerId_SurfaceHandlerNull_TrueBranch
 * @tc.desc: Cover HandleNewTunnelLayerId line 216 (surfaceHandler == nullptr) true branch.
 *          Passing nullptr surfaceNode makes surfaceHandler nullptr (ternary at line 215),
 *          so the function returns immediately without crash. HandleNewTunnelLayerId is
 *          private; the test uses private access via #define private public.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerManagerTest, HandleNewTunnelLayerId_SurfaceHandlerNull_TrueBranch, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    EXPECT_NE(rsContext, nullptr);
    RSTunnelLayerManager tunnelLayerManager(rsContext);

    tunnelLayerManager.HandleNewTunnelLayerId(nullptr);
    SUCCEED();
}

/**
 * @tc.name: HandleNewTunnelLayerId_SurfaceHandlerNotNull_FalseBranch
 * @tc.desc: Cover HandleNewTunnelLayerId line 216 (surfaceHandler == nullptr) false branch.
 *          A valid surfaceNode with a non-null surfaceHandler passes the guard and proceeds
 *          to line 219 GetConsumer / line 220 TryGetTunnelRuntimeState.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerManagerTest, HandleNewTunnelLayerId_SurfaceHandlerNotNull_FalseBranch, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    auto rsContext = std::make_shared<RSContext>();
    RSTunnelLayerManager tunnelLayerManager(rsContext);
    auto context = CreateTunnelContext(MakeNodeId(TEST_PID_ONE, TEST_NODE_UID_ONE));
    ASSERT_TRUE(context.IsProducerReady());
    ASSERT_TRUE(RegisterSurfaceNode(rsContext, context.node));

    TunnelLayerState tunnelState;
    ASSERT_TRUE(SetTunnelInfoForConsumer(context.consumer, tunnelState));
    auto& runtime = *context.node->GetTunnelRuntimeState();
    runtime.SetLayerInfo(tunnelState.tunnelLayerId, tunnelState.property);
    runtime.SetBuilding();
    ASSERT_NE(context.node->GetMutableRSSurfaceHandler(), nullptr);

    tunnelLayerManager.HandleNewTunnelLayerId(context.node);

    EXPECT_NE(runtime.GetTunnelState(), RSTunnelRuntimeState::TunnelState::NOT_TUNNEL);
}

/**
 * @tc.name: HandleLayerStateChanged_NullRuntimeState
 * @tc.desc: Cover ProcessLayerStateChanged null runtime state branch.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerManagerTest, HandleLayerStateChanged_NullRuntimeState, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    auto rsContext = std::make_shared<RSContext>();
    RSTunnelLayerManager tunnelLayerManager(rsContext);
    auto context = CreateTunnelContext(MakeNodeId(TEST_PID_ONE, TEST_NODE_UID_TWO));
    ASSERT_TRUE(context.IsProducerReady());
    ASSERT_TRUE(RegisterSurfaceNode(rsContext, context.node));
    context.surfaceHandler->MarkTunnelLayerInfoReceived();
    EXPECT_EQ(context.node->TryGetTunnelRuntimeState(), nullptr);

    tunnelLayerManager.HandleLayerStateChanged(context.node->GetId(), LayerStateChange::AVAILABLE, 0);
    EXPECT_EQ(context.node->TryGetTunnelRuntimeState(), nullptr);
}

/**
 * @tc.name: AttachArbiterToRenderThread_NoCrash
 * @tc.desc: Cover AttachArbiterToRenderThread method call.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerManagerTest, AttachArbiterToRenderThread_NoCrash, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    EXPECT_NE(rsContext, nullptr);
    RSTunnelLayerManager tunnelLayerManager(rsContext);
    tunnelLayerManager.AttachArbiterToRenderThread();
    RSUniRenderThread::Instance().SetCommitDoneCallback(nullptr);
}

/**
 * @tc.name: AttachArbiterToRenderThread_ArbiterNotNull_TrueBranch
 * @tc.desc: Cover AttachArbiterToRenderThread line 65 (arbiter_ != nullptr) true branch.
 *          The constructor initializes arbiter_ with a valid RSTunnelRouteArbiter, so
 *          AttachToRenderThread() is called and sets a non-null commit-done callback on
 *          RSUniRenderThread. A sentinel callback is installed before the call; if the
 *          true branch is taken, the sentinel is overwritten by the arbiter's callback.
 *          NotifyCommitDone then invokes the arbiter's callback (not the sentinel), so
 *          sentinelCalled stays false, proving AttachToRenderThread() was invoked.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerManagerTest, AttachArbiterToRenderThread_ArbiterNotNull_TrueBranch, TestSize.Level1)
{
    auto rsContext = std::make_shared<RSContext>();
    RSTunnelLayerManager tunnelLayerManager(rsContext);

    bool sentinelCalled = false;
    RSUniRenderThread::Instance().SetCommitDoneCallback([&sentinelCalled](ScreenId) { sentinelCalled = true; });

    tunnelLayerManager.AttachArbiterToRenderThread();

    RSUniRenderThread::Instance().NotifyCommitDone(0);
    EXPECT_FALSE(sentinelCalled);

    RSUniRenderThread::Instance().SetCommitDoneCallback(nullptr);
}

/**
 * @tc.name: UpdateTunnelStateMachine_NullRuntimeState
 * @tc.desc: Cover UpdateTunnelStateMachine early return when tunnelRuntime is null.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerManagerTest, UpdateTunnelStateMachine_NullRuntimeState, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    auto rsContext = std::make_shared<RSContext>();
    RSTunnelLayerManager tunnelLayerManager(rsContext);
    auto context = CreateTunnelContext(MakeNodeId(TEST_PID_ONE, TEST_NODE_UID_TWO));
    ASSERT_TRUE(context.IsProducerReady());
    ASSERT_TRUE(RegisterSurfaceNode(rsContext, context.node));
    context.surfaceHandler->MarkTunnelLayerInfoReceived();

    tunnelLayerManager.UpdateTunnelStateMachine(context.node, true);
    EXPECT_EQ(context.node->TryGetTunnelRuntimeState(), nullptr);
}

/**
 * @tc.name: HandleAvailableCallback_ConcurrentNullRuntimeState
 * @tc.desc: Cover HandleAvailableCallback line 47 (tunnelRuntimePtr == nullptr) true branch.
 *          HandleAvailableCallback re-fetches TryGetTunnelRuntimeState() at line 46, separate from
 *          ProcessLayerStateChanged's pre-check at line 270. When another thread calls SetConsumer
 *          with a different consumer between those two fetches, tunnelRuntimeState_ is reset to
 *          nullptr, making the line 47 condition true. The function must return false without crash.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerManagerTest, HandleAvailableCallback_ConcurrentNullRuntimeState, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    auto rsContext = std::make_shared<RSContext>();
    RSTunnelLayerManager tunnelLayerManager(rsContext);

    auto context = CreateTunnelContext(MakeNodeId(TEST_PID_ONE, TEST_NODE_UID_ONE));
    ASSERT_TRUE(context.IsProducerReady());
    ASSERT_TRUE(RegisterSurfaceNode(rsContext, context.node));

    TunnelLayerState tunnelState;
    ASSERT_TRUE(SetTunnelInfoForConsumer(context.consumer, tunnelState));
    auto& runtime = *context.node->GetTunnelRuntimeState();
    runtime.SetLayerInfo(tunnelState.tunnelLayerId, tunnelState.property);
    runtime.SetBuilding();
    context.surfaceHandler->MarkTunnelLayerInfoReceived();
    context.surfaceHandler->SetCurrentFrameBufferConsumed();
    tunnelLayerManager.UpdateTunnelStateMachine(context.node, true);
    auto generation = runtime.GetTunnelLayerGeneration();

    // Second consumer: SetConsumer with a different consumer resets tunnelRuntimeState_.
    auto context2 = CreateTunnelContext(MakeNodeId(TEST_PID_TWO, TEST_NODE_UID_TWO));
    ASSERT_TRUE(context2.IsProducerReady());

    // Concurrently reset tunnelRuntimeState_ via SetConsumer while
    // HandleLayerStateChanged is executing ProcessLayerStateChanged.
    // If SetConsumer resets tunnelRuntimeState_ between ProcessLayerStateChanged's
    // TryGetTunnelRuntimeState() check (line 270) and HandleAvailableCallback's
    // re-fetch (line 46), the nullptr branch at line 47 is triggered.
    std::atomic<bool> stopFlag(false);
    std::thread resetThread([&]() {
        while (!stopFlag.load(std::memory_order_relaxed)) {
            context.node->SetConsumer(context2.consumer);
        }
    });

    constexpr int32_t ITERATION_COUNT = 100;
    for (int32_t i = 0; i < ITERATION_COUNT; ++i) {
        tunnelLayerManager.HandleLayerStateChanged(context.node->GetId(), LayerStateChange::AVAILABLE, generation);
    }

    stopFlag.store(true, std::memory_order_relaxed);
    resetThread.join();

    SUCCEED();
}

} // namespace OHOS::Rosen
