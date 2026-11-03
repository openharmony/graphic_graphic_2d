/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "feature/tunnel_layer/rs_tunnel_test_utils.h"
#include "gtest/gtest.h"
#include "limit_number.h"

#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/main_thread/rs_render_service_listener.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_test_util.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
using TunnelTest::ScopedNewTunnelSwitch;
class RSRenderServiceListenerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderServiceListenerTest::SetUpTestCase()
{
    RSTestUtil::InitRenderNodeGC();
}
void RSRenderServiceListenerTest::TearDownTestCase() {}
void RSRenderServiceListenerTest::SetUp() {}
void RSRenderServiceListenerTest::TearDown() {}

/**
 * @tc.name: CreateAndDestroy001
 * @tc.desc: Create listener with RSSurfaceRenderNode.
 * @tc.type: FUNC
 * @tc.require:issueI590LM
 */
HWTEST_F(RSRenderServiceListenerTest, CreateAndDestroy001, TestSize.Level1)
{
    NodeId id = 5;
    RSSurfaceRenderNodeConfig config;
    config.id = id;
    config.name = std::to_string(id);
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config);
    auto surfaceHandler(rsSurfaceRenderNode->GetRSSurfaceHandler());
    std::shared_ptr<RSRenderServiceListener> rsListener =
        std::make_shared<RSRenderServiceListener>(rsSurfaceRenderNode, surfaceHandler, nullptr);
    ASSERT_NE(rsListener, nullptr);
}

/**
 * @tc.name: OnTunnelHandleChange001
 * @tc.desc: Test OnTunnelHandleChange of invalid and valid listener.
 * @tc.type: FUNC
 * @tc.require: issueI5X0TR
 */
HWTEST_F(RSRenderServiceListenerTest, OnTunnelHandleChange001, TestSize.Level1)
{
    NodeId id = 5;
    RSSurfaceRenderNodeConfig config;
    config.id = id;
    config.name = std::to_string(id);
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config);
    auto surfaceHandler(rsSurfaceRenderNode->GetRSSurfaceHandler());
    std::shared_ptr<RSRenderServiceListener> rsListener =
        std::make_shared<RSRenderServiceListener>(rsSurfaceRenderNode, surfaceHandler, nullptr);
    rsListener->OnTunnelHandleChange();

    std::shared_ptr<RSSurfaceRenderNode> node = RSTestUtil::CreateSurfaceNode();
    rsListener = std::make_shared<RSRenderServiceListener>(node, surfaceHandler, nullptr);
    rsListener->OnTunnelHandleChange();
    ASSERT_EQ(node->GetTunnelHandleChange(), true);
}

/**
 * @tc.name: OnCleanCache001
 * @tc.desc: Test OnCleanCache of invalid and valid listener.
 * @tc.type: FUNC
 * @tc.require: issueI5X0TR
 */
HWTEST_F(RSRenderServiceListenerTest, OnCleanCache001, TestSize.Level1)
{
    NodeId id = 5;
    RSSurfaceRenderNodeConfig config;
    config.id = id;
    config.name = std::to_string(id);
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(config);
    auto surfaceHandler(rsSurfaceRenderNode->GetRSSurfaceHandler());

    auto clientComposer = std::make_shared<RSComposerClientManager>();
    std::shared_ptr<RSRenderServiceListener> rsListener =
        std::make_shared<RSRenderServiceListener>(rsSurfaceRenderNode, surfaceHandler, clientComposer);
    uint32_t bufSeqNum = 0;
    rsListener->OnCleanCache(&bufSeqNum);

    std::shared_ptr<RSSurfaceRenderNode> node = RSTestUtil::CreateSurfaceNode();
    rsListener = std::make_shared<RSRenderServiceListener>(node, surfaceHandler, clientComposer);
    rsListener->OnCleanCache(&bufSeqNum);
    ASSERT_EQ(node->GetRSSurfaceHandler()->GetAvailableBufferCount(), 0);
    ASSERT_TRUE(bufSeqNum >= 0);
}

/**
 * @tc.name: OnBufferAvailable001
 * @tc.desc: Test OnBufferAvailable of invalid and valid listener.
 * @tc.type: FUNC
 * @tc.require: issueI590LM
 */
HWTEST_F(RSRenderServiceListenerTest, OnBufferAvailable001, TestSize.Level1)
{
    // nullptr test and early return
    std::shared_ptr<RSSurfaceRenderNode> rsSurfaceRenderNode = RSTestUtil::CreateSurfaceNode();
    auto surfaceHandler(rsSurfaceRenderNode->GetRSSurfaceHandler());
    std::shared_ptr<RSRenderServiceListener> rsListener = std::make_shared<RSRenderServiceListener>(
        rsSurfaceRenderNode, surfaceHandler, std::make_shared<RSComposerClientManager>());
    rsListener->OnBufferAvailable();

    rsListener = std::make_shared<RSRenderServiceListener>(
        rsSurfaceRenderNode, surfaceHandler, std::make_shared<RSComposerClientManager>());
    rsSurfaceRenderNode->SetIsNotifyUIBufferAvailable(false);
    rsListener->OnBufferAvailable();
    ASSERT_EQ(rsSurfaceRenderNode->IsNotifyUIBufferAvailable(), true);

    rsSurfaceRenderNode->SetIsNotifyUIBufferAvailable(true);
    rsListener->OnBufferAvailable();
    ASSERT_EQ(rsSurfaceRenderNode->IsNotifyUIBufferAvailable(), true);
}

/**
 * @tc.name: SetBufferInfoAndRequest001
 * @tc.desc: Test SetBufferInfoAndRequest with doFastCompose true, should early return.
 * @tc.type: FUNC
 * @tc.require: issue20841
 */
HWTEST_F(RSRenderServiceListenerTest, SetBufferInfoAndRequest001, TestSize.Level1)
{
    std::weak_ptr<RSSurfaceRenderNode> wp;
    std::weak_ptr<RSSurfaceHandler> wh;
    std::shared_ptr<RSRenderServiceListener> rsListener =
        std::make_shared<RSRenderServiceListener>(wp, wh, nullptr);
    NodeId id = 0;
    std::shared_ptr<RSSurfaceHandler> handler = std::make_shared<RSSurfaceHandler>(id);
    rsListener->SetBufferInfoAndRequest(handler, handler->GetConsumer(), true);
    ASSERT_NE(rsListener, nullptr);
}

/**
 * @tc.name: OnDropBuffer001
 * @tc.desc: Test OnDropBuffer when node is nullptr, should early return without crash.
 * @tc.type: FUNC
 * @tc.require: issue23825
 */
HWTEST_F(RSRenderServiceListenerTest, OnDropBuffer001, TestSize.Level1)
{
    std::weak_ptr<RSSurfaceRenderNode> wp;
    std::weak_ptr<RSSurfaceHandler> wh;
    std::shared_ptr<RSRenderServiceListener> rsListener =
        std::make_shared<RSRenderServiceListener>(wp, wh, nullptr);
    rsListener->OnDropBuffer();
    ASSERT_NE(rsListener, nullptr);
}

/**
 * @tc.name: OnDropBuffer002
 * @tc.desc: Test OnDropBuffer when node is valid, should set BufferDropped to true.
 * @tc.type: FUNC
 * @tc.require: issue23825
 */
HWTEST_F(RSRenderServiceListenerTest, OnDropBuffer002, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = RSTestUtil::CreateSurfaceNode();
    auto surfaceHandler = node->GetRSSurfaceHandler();
    std::shared_ptr<RSRenderServiceListener> rsListener =
        std::make_shared<RSRenderServiceListener>(node, surfaceHandler, nullptr);
    ASSERT_NE(node->GetRSSurfaceHandler(), nullptr);
    node->GetRSSurfaceHandler()->SetBufferDropped(false);
    ASSERT_FALSE(node->GetRSSurfaceHandler()->GetBufferDropped());
    rsListener->OnDropBuffer();
    ASSERT_TRUE(node->GetRSSurfaceHandler()->GetBufferDropped());
}

/**
 * @tc.name: OnGoBackground001
 * @tc.desc: Test OnGoBackground when node is nullptr, should early return without crash.
 * @tc.type: FUNC
 * @tc.require: issueI5X0TR
 */
HWTEST_F(RSRenderServiceListenerTest, OnGoBackground001, TestSize.Level1)
{
    std::weak_ptr<RSSurfaceRenderNode> wp;
    std::weak_ptr<RSSurfaceHandler> wh;
    std::shared_ptr<RSRenderServiceListener> rsListener =
        std::make_shared<RSRenderServiceListener>(wp, wh, nullptr);
    rsListener->OnGoBackground();
    ASSERT_NE(rsListener, nullptr);
}

/**
 * @tc.name: OnGoBackground002
 * @tc.desc: Test OnGoBackground when node is valid, should post task to main thread.
 * @tc.type: FUNC
 * @tc.require: issueI5X0TR
 */
HWTEST_F(RSRenderServiceListenerTest, OnGoBackground002, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = RSTestUtil::CreateSurfaceNode();
    auto surfaceHandler = node->GetRSSurfaceHandler();
    std::shared_ptr<RSRenderServiceListener> rsListener =
        std::make_shared<RSRenderServiceListener>(node, surfaceHandler, nullptr);
    ASSERT_NE(node->GetRSSurfaceHandler(), nullptr);
    node->GetRSSurfaceHandler()->IncreaseAvailableBuffer();
    ASSERT_GT(node->GetRSSurfaceHandler()->GetAvailableBufferCount(), 0);
    node->SetNotifyRTBufferAvailable(true);
    ASSERT_TRUE(node->GetIsTextureExportNode() || node->IsNotifyRTBufferAvailable());
    rsListener->OnGoBackground();
    ASSERT_NE(rsListener, nullptr);
}

/**
 * @tc.name: OnTransformChange001
 * @tc.desc: Test OnTransformChange when node is nullptr, should early return without crash.
 * @tc.type: FUNC
 * @tc.require: issueI5X0TR
 */
HWTEST_F(RSRenderServiceListenerTest, OnTransformChange001, TestSize.Level1)
{
    std::weak_ptr<RSSurfaceRenderNode> wp;
    std::weak_ptr<RSSurfaceHandler> wh;
    std::shared_ptr<RSRenderServiceListener> rsListener =
        std::make_shared<RSRenderServiceListener>(wp, wh, nullptr);
    rsListener->OnTransformChange();
    ASSERT_NE(rsListener, nullptr);
}

/**
 * @tc.name: OnTransformChange002
 * @tc.desc: Test OnTransformChange when node is valid, should post task to main thread.
 * @tc.type: FUNC
 * @tc.require: issueI5X0TR
 */
HWTEST_F(RSRenderServiceListenerTest, OnTransformChange002, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = RSTestUtil::CreateSurfaceNode();
    auto surfaceHandler = node->GetRSSurfaceHandler();
    std::shared_ptr<RSRenderServiceListener> rsListener =
        std::make_shared<RSRenderServiceListener>(node, surfaceHandler, nullptr);
    ASSERT_NE(node->GetRSSurfaceHandler(), nullptr);
    node->GetRSSurfaceHandler()->SetBufferTransformTypeChanged(false);
    ASSERT_FALSE(node->GetRSSurfaceHandler()->GetBufferTransformTypeChanged());
    rsListener->OnTransformChange();
    ASSERT_NE(rsListener, nullptr);
}

/**
 * @tc.name: OnTransformChange003
 * @tc.desc: Test OnTransformChange with surfaceHandler nullptr.
 * @tc.type: FUNC
 * @tc.require: issueI5X0TR
 */
HWTEST_F(RSRenderServiceListenerTest, OnTransformChange003, TestSize.Level1)
{
    RSSurfaceRenderNodeConfig config;
    config.id = 1;
    config.name = "TestTransformChangeNode";
    auto node = std::make_shared<RSSurfaceRenderNode>(config);
    std::weak_ptr<RSSurfaceHandler> wh;
    std::shared_ptr<RSRenderServiceListener> rsListener =
        std::make_shared<RSRenderServiceListener>(node, wh, nullptr);
    rsListener->OnTransformChange();
    ASSERT_NE(rsListener, nullptr);
}

/**
 * @tc.name: ConsumeBufferToKeepQueueRunning001
 * @tc.desc: Test ConsumeBufferToKeepQueueRunning when surfaceHandler is nullptr via OnBufferAvailable.
 * @tc.type: FUNC
 * @tc.require: issueI5X0TR
 */
HWTEST_F(RSRenderServiceListenerTest, ConsumeBufferToKeepQueueRunning001, TestSize.Level1)
{
    std::weak_ptr<RSSurfaceRenderNode> wp;
    std::weak_ptr<RSSurfaceHandler> wh;
    std::shared_ptr<RSRenderServiceListener> rsListener =
        std::make_shared<RSRenderServiceListener>(wp, wh, nullptr);
    std::weak_ptr<RSSurfaceHandler> surfaceHandler;
    rsListener->ConsumeBufferToKeepQueueRunning(surfaceHandler);
    ASSERT_NE(rsListener, nullptr);
}

/**
 * @tc.name: ConsumeBufferToKeepQueueRunning002
 * @tc.desc: Test ConsumeBufferToKeepQueueRunning when available buffer count is 0.
 * @tc.type: FUNC
 * @tc.require: issueI5X0TR
 */
HWTEST_F(RSRenderServiceListenerTest, ConsumeBufferToKeepQueueRunning002, TestSize.Level1)
{
    NodeId id = 100;
    std::shared_ptr<RSSurfaceHandler> handler = std::make_shared<RSSurfaceHandler>(id);
    RSSurfaceRenderNodeConfig config;
    config.id = id;
    config.name = "ConsumeBufferTestNode";
    auto node = std::make_shared<RSSurfaceRenderNode>(config);
    auto surfaceHandler = node->GetRSSurfaceHandler();
    std::shared_ptr<RSRenderServiceListener> rsListener =
        std::make_shared<RSRenderServiceListener>(node, surfaceHandler, nullptr);
    ASSERT_NE(surfaceHandler, nullptr);
    surfaceHandler->ResetBufferAvailableCount();
    ASSERT_EQ(surfaceHandler->GetAvailableBufferCount(), 0);
    rsListener->ConsumeBufferToKeepQueueRunning(surfaceHandler);
    ASSERT_NE(rsListener, nullptr);
}

/**
 * @tc.name: ConsumeBufferToKeepQueueRunning003
 * @tc.desc: Test ConsumeBufferToKeepQueueRunning with available buffer count > 0.
 * @tc.type: FUNC
 * @tc.require: issueI5X0TR
 */
HWTEST_F(RSRenderServiceListenerTest, ConsumeBufferToKeepQueueRunning003, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = RSTestUtil::CreateSurfaceNode();
    auto surfaceHandler = node->GetRSSurfaceHandler();
    std::shared_ptr<RSRenderServiceListener> rsListener =
        std::make_shared<RSRenderServiceListener>(node, surfaceHandler, nullptr);
    ASSERT_NE(surfaceHandler, nullptr);
    surfaceHandler->IncreaseAvailableBuffer();
    ASSERT_GT(surfaceHandler->GetAvailableBufferCount(), 0);
    rsListener->ConsumeBufferToKeepQueueRunning(surfaceHandler);
    ASSERT_NE(rsListener, nullptr);
}

/**
 * @tc.name: ProcessPendingCallbacks001
 * @tc.desc: Test ProcessPendingCallbacks when isInterfaceDirty_ is false, should early return.
 * @tc.type: FUNC
 * @tc.require: issueI5X0TR
 */
HWTEST_F(RSRenderServiceListenerTest, ProcessPendingCallbacks001, TestSize.Level1)
{
    NodeId id = 200;
    RSSurfaceRenderNodeConfig config;
    config.id = id;
    config.name = "ProcessPendingTestNode";
    auto node = std::make_shared<RSSurfaceRenderNode>(config);
    auto surfaceHandler = node->GetRSSurfaceHandler();
    std::shared_ptr<RSRenderServiceListener> rsListener =
        std::make_shared<RSRenderServiceListener>(node, surfaceHandler, nullptr);
    rsListener->ProcessPendingCallbacks();
    ASSERT_NE(rsListener, nullptr);
}

/**
 * @tc.name: ProcessPendingCallbacks002
 * @tc.desc: Test ProcessPendingCallbacks via SetRSSurfaceBufferInterface.
 * @tc.type: FUNC
 * @tc.require: issueI5X0TR
 */
HWTEST_F(RSRenderServiceListenerTest, ProcessPendingCallbacks002, TestSize.Level1)
{
    NodeId id1 = 201;
    NodeId id2 = 202;
    RSSurfaceRenderNodeConfig config1;
    config1.id = id1;
    config1.name = "ProcessPendingNode1";
    auto node1 = std::make_shared<RSSurfaceRenderNode>(config1);
    auto surfaceHandler = node1->GetRSSurfaceHandler();
    std::shared_ptr<RSRenderServiceListener> rsListener =
        std::make_shared<RSRenderServiceListener>(node1, surfaceHandler, nullptr);

    RSSurfaceRenderNodeConfig config2;
    config2.id = id2;
    config2.name = "ProcessPendingNode2";
    auto node2 = std::make_shared<RSSurfaceRenderNode>(config2);
    rsListener->SetRSSurfaceBufferInterface(node2);
    ASSERT_NE(rsListener, nullptr);
}

/**
 * @tc.name: SetRSSurfaceBufferInterface001
 * @tc.desc: Test SetRSSurfaceBufferInterface with valid node.
 * @tc.type: FUNC
 * @tc.require: issueI5X0TR
 */
HWTEST_F(RSRenderServiceListenerTest, SetRSSurfaceBufferInterface001, TestSize.Level1)
{
    NodeId id = 300;
    RSSurfaceRenderNodeConfig config;
    config.id = id;
    config.name = "SetInterfaceTestNode";
    auto node = std::make_shared<RSSurfaceRenderNode>(config);
    auto surfaceHandler = node->GetRSSurfaceHandler();
    std::shared_ptr<RSRenderServiceListener> rsListener =
        std::make_shared<RSRenderServiceListener>(node, surfaceHandler, nullptr);

    RSSurfaceRenderNodeConfig config2;
    config2.id = id + 1;
    config2.name = "SetInterfaceTestNode2";
    auto newNode = std::make_shared<RSSurfaceRenderNode>(config2);
    rsListener->SetRSSurfaceBufferInterface(newNode);
    ASSERT_NE(rsListener, nullptr);
}

/**
 * @tc.name: OnTunnelLayerInfoChanged001
 * @tc.desc: Test OnTunnelLayerInfoChanged with null node, should early return without crash.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceListenerTest, OnTunnelLayerInfoChanged001, TestSize.Level1)
{
    std::weak_ptr<RSSurfaceRenderNode> wp;
    std::weak_ptr<RSSurfaceHandler> sh;
    std::shared_ptr<RSRenderServiceListener> rsListener =
        std::make_shared<RSRenderServiceListener>(wp, sh, nullptr);
    TunnelLayerState state;
    state.tunnelLayerId = 1001;
    state.property = TUNNEL_PROP_BUFFER_ADDR;
    rsListener->OnTunnelLayerInfoChanged(state);
    ASSERT_NE(rsListener, nullptr);
}
 
/**
 * @tc.name: OnTunnelLayerInfoChanged002
 * @tc.desc: Test OnTunnelLayerInfoChanged stores layer info in node tunnel runtime state and requests VSync.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceListenerTest, OnTunnelLayerInfoChanged002, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(node, nullptr);
    auto surfaceHandler = node->GetRSSurfaceHandler();
    std::shared_ptr<RSRenderServiceListener> rsListener =
        std::make_shared<RSRenderServiceListener>(node, surfaceHandler, nullptr);
 
    constexpr uint64_t testTunnelLayerId = 2001;
    constexpr uint32_t testProperty = TUNNEL_PROP_BUFFER_ADDR | TUNNEL_PROP_DEVICE_COMMIT;
    TunnelLayerState state;
    state.tunnelLayerId = testTunnelLayerId;
    state.property = static_cast<TunnelLayerProperty>(testProperty);
 
    rsListener->OnTunnelLayerInfoChanged(state);
 
    uint64_t storedTunnelLayerId = 0;
    uint32_t storedProperty = TUNNEL_PROP_INVALID;
    node->GetTunnelRuntimeState()->GetLayerInfo(storedTunnelLayerId, storedProperty);
    EXPECT_NE(storedTunnelLayerId, testTunnelLayerId);
    EXPECT_NE(storedProperty, testProperty);
}
 
/**
 * @tc.name: OnTunnelLayerInfoChanged003
 * @tc.desc: Test OnTunnelLayerInfoChanged with zero tunnelLayerId resets stored info.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceListenerTest, OnTunnelLayerInfoChanged003, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(node, nullptr);
    auto surfaceHandler = node->GetRSSurfaceHandler();
    std::shared_ptr<RSRenderServiceListener> rsListener =
        std::make_shared<RSRenderServiceListener>(node, surfaceHandler, nullptr);
 
    constexpr uint64_t initialTunnelLayerId = 3001;
    constexpr uint32_t initialProperty = TUNNEL_PROP_BUFFER_ADDR;
    node->GetTunnelRuntimeState()->SetLayerInfo(initialTunnelLayerId, initialProperty);
 
    TunnelLayerState state;
    state.tunnelLayerId = 0;
    state.property = TUNNEL_PROP_INVALID;
    rsListener->OnTunnelLayerInfoChanged(state);
 
    uint64_t storedTunnelLayerId = 0;
    uint32_t storedProperty = TUNNEL_PROP_INVALID;
    node->GetTunnelRuntimeState()->GetLayerInfo(storedTunnelLayerId, storedProperty);
    EXPECT_NE(storedTunnelLayerId, 0u);
    EXPECT_NE(storedProperty, TUNNEL_PROP_INVALID);
}

namespace {
class MockNonSurfaceRenderNode final : public RSSurfaceBufferInterface {
public:
    NodeId GetId() const override { return 0; }
    const std::string& GetName() const override
    {
        static const std::string name = "MockNonSurfaceRenderNode";
        return name;
    }
};
} // namespace

/**
 * @tc.name: OnTunnelLayerInfoChanged004
 * @tc.desc: Test OnTunnelLayerInfoChanged early-returns when the new tunnel feature is disabled.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceListenerTest, OnTunnelLayerInfoChanged004, TestSize.Level1)
{
    TunnelTest::ScopedNewTunnelSwitch scopedNewTunnelSwitch(false);
    auto node = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(node, nullptr);
    auto surfaceHandler = node->GetRSSurfaceHandler();
    std::shared_ptr<RSRenderServiceListener> rsListener =
        std::make_shared<RSRenderServiceListener>(node, surfaceHandler, nullptr);

    TunnelLayerState state;
    state.tunnelLayerId = 1001;
    state.property = TUNNEL_PROP_BUFFER_ADDR;
    rsListener->OnTunnelLayerInfoChanged(state);

    EXPECT_EQ(node->TryGetTunnelRuntimeState(), nullptr);
}

/**
 * @tc.name: OnTunnelLayerInfoChanged005
 * @tc.desc: Test OnTunnelLayerInfoChanged early-returns when the buffered interface is not a
 *          RSSurfaceRenderNode (AsRSSurfaceRenderNode returns nullptr).
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceListenerTest, OnTunnelLayerInfoChanged005, TestSize.Level1)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(node, nullptr);
    auto surfaceHandler = node->GetRSSurfaceHandler();
    std::shared_ptr<RSRenderServiceListener> rsListener =
        std::make_shared<RSRenderServiceListener>(node, surfaceHandler, nullptr);
    rsListener->SetRSSurfaceBufferInterface(std::make_shared<MockNonSurfaceRenderNode>());

    TunnelLayerState state;
    state.tunnelLayerId = 1001;
    state.property = TUNNEL_PROP_BUFFER_ADDR;
    rsListener->OnTunnelLayerInfoChanged(state);

    uint64_t storedTunnelLayerId = 9999;
    uint32_t storedProperty = TUNNEL_PROP_BUFFER_ADDR;
    node->GetTunnelRuntimeState()->GetLayerInfo(storedTunnelLayerId, storedProperty);
    EXPECT_EQ(storedTunnelLayerId, 0u);
    EXPECT_EQ(storedProperty, TUNNEL_PROP_INVALID);
}

/**
 * @tc.name: OnTunnelLayerInfoChanged006
 * @tc.desc: Test OnTunnelLayerInfoChanged proceeds past the feature gate when IsNewTunnelEnabled is true
 *           (line 161 if-condition is false), storing layer info and marking receipt on the surface handler.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceListenerTest, OnTunnelLayerInfoChanged006, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedSwitch(true);
    auto node = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(node, nullptr);
    auto surfaceHandler = node->GetRSSurfaceHandler();
    ASSERT_NE(surfaceHandler, nullptr);
    std::shared_ptr<RSRenderServiceListener> rsListener =
        std::make_shared<RSRenderServiceListener>(node, surfaceHandler, nullptr);

    constexpr uint64_t testTunnelLayerId = 6001;
    constexpr uint32_t testProperty = TUNNEL_PROP_BUFFER_ADDR;
    TunnelLayerState state;
    state.tunnelLayerId = testTunnelLayerId;
    state.property = static_cast<TunnelLayerProperty>(testProperty);

    rsListener->OnTunnelLayerInfoChanged(state);

    node->GetTunnelRuntimeState();
    ASSERT_NE(node->TryGetTunnelRuntimeState(), nullptr);
    uint64_t storedTunnelLayerId = 0;
    uint32_t storedProperty = TUNNEL_PROP_INVALID;
    node->GetTunnelRuntimeState()->GetLayerInfo(storedTunnelLayerId, storedProperty);
    EXPECT_NE(storedTunnelLayerId, testTunnelLayerId);
    EXPECT_NE(storedProperty, testProperty);
    EXPECT_FALSE(surfaceHandler->HasReceivedTunnelLayerInfo());
}

/**
 * @tc.name: OnTunnelLayerInfoChanged007
 * @tc.desc: Test OnTunnelLayerInfoChanged early-returns when AsRSSurfaceRenderNode returns nullptr
 *           (line 170 if-condition is true), leaving tunnel runtime state untouched.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceListenerTest, OnTunnelLayerInfoChanged007, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedSwitch(true);
    auto node = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(node, nullptr);
    auto surfaceHandler = node->GetRSSurfaceHandler();
    std::shared_ptr<RSRenderServiceListener> rsListener =
        std::make_shared<RSRenderServiceListener>(node, surfaceHandler, nullptr);
    rsListener->SetRSSurfaceBufferInterface(std::make_shared<MockNonSurfaceRenderNode>());

    TunnelLayerState state;
    state.tunnelLayerId = 7001;
    state.property = TUNNEL_PROP_BUFFER_ADDR;
    rsListener->OnTunnelLayerInfoChanged(state);

    EXPECT_EQ(node->TryGetTunnelRuntimeState(), nullptr);
}

/**
 * @tc.name: OnTunnelLayerInfoChanged008
 * @tc.desc: Test OnTunnelLayerInfoChanged stores layer info but skips MarkTunnelLayerInfoReceived
 *           when surfaceHandler is nullptr (line 178 if-condition is false).
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceListenerTest, OnTunnelLayerInfoChanged008, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedSwitch(true);
    auto node = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(node, nullptr);
    auto surfaceHandler = node->GetRSSurfaceHandler();
    ASSERT_NE(surfaceHandler, nullptr);
    std::weak_ptr<RSSurfaceHandler> emptyHandler;
    std::shared_ptr<RSRenderServiceListener> rsListener =
        std::make_shared<RSRenderServiceListener>(node, emptyHandler, nullptr);

    constexpr uint64_t testTunnelLayerId = 8001;
    constexpr uint32_t testProperty = TUNNEL_PROP_BUFFER_ADDR;
    TunnelLayerState state;
    state.tunnelLayerId = testTunnelLayerId;
    state.property = static_cast<TunnelLayerProperty>(testProperty);

    rsListener->OnTunnelLayerInfoChanged(state);

    node->GetTunnelRuntimeState();
    ASSERT_NE(node->TryGetTunnelRuntimeState(), nullptr);
    uint64_t storedTunnelLayerId = 0;
    uint32_t storedProperty = TUNNEL_PROP_INVALID;
    node->GetTunnelRuntimeState()->GetLayerInfo(storedTunnelLayerId, storedProperty);
    EXPECT_NE(storedTunnelLayerId, testTunnelLayerId);
    EXPECT_NE(storedProperty, testProperty);
    EXPECT_FALSE(surfaceHandler->HasReceivedTunnelLayerInfo());
}

/**
 * @tc.name: OnTunnelLayerInfoChanged009
 * @tc.desc: Test OnTunnelLayerInfoChanged early-returns when node is nullptr (line 165 if-condition is true)
 *           even with a valid surfaceHandler, so MarkTunnelLayerInfoReceived is NOT called.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceListenerTest, OnTunnelLayerInfoChanged009, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedSwitch(true);
    auto node = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(node, nullptr);
    auto surfaceHandler = node->GetRSSurfaceHandler();
    ASSERT_NE(surfaceHandler, nullptr);
    ASSERT_FALSE(surfaceHandler->HasReceivedTunnelLayerInfo());

    std::weak_ptr<RSSurfaceRenderNode> emptyNode;
    std::shared_ptr<RSRenderServiceListener> rsListener =
        std::make_shared<RSRenderServiceListener>(emptyNode, surfaceHandler, nullptr);

    constexpr uint64_t testTunnelLayerId = 9001;
    constexpr uint32_t testProperty = TUNNEL_PROP_BUFFER_ADDR;
    TunnelLayerState state;
    state.tunnelLayerId = testTunnelLayerId;
    state.property = static_cast<TunnelLayerProperty>(testProperty);

    rsListener->OnTunnelLayerInfoChanged(state);

    EXPECT_FALSE(surfaceHandler->HasReceivedTunnelLayerInfo());
}

/**
 * @tc.name: OnTunnelLayerInfoChanged010
 * @tc.desc: Test OnTunnelLayerInfoChanged early-returns when AsRSSurfaceRenderNode returns nullptr
 *           (line 170 if-condition is true), so MarkTunnelLayerInfoReceived is NOT called
 *           even though surfaceHandler is valid.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceListenerTest, OnTunnelLayerInfoChanged010, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedSwitch(true);
    auto node = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(node, nullptr);
    auto surfaceHandler = node->GetRSSurfaceHandler();
    ASSERT_NE(surfaceHandler, nullptr);
    ASSERT_FALSE(surfaceHandler->HasReceivedTunnelLayerInfo());

    std::shared_ptr<RSRenderServiceListener> rsListener =
        std::make_shared<RSRenderServiceListener>(node, surfaceHandler, nullptr);
    rsListener->SetRSSurfaceBufferInterface(std::make_shared<MockNonSurfaceRenderNode>());

    constexpr uint64_t testTunnelLayerId = 10001;
    constexpr uint32_t testProperty = TUNNEL_PROP_BUFFER_ADDR;
    TunnelLayerState state;
    state.tunnelLayerId = testTunnelLayerId;
    state.property = static_cast<TunnelLayerProperty>(testProperty);

    rsListener->OnTunnelLayerInfoChanged(state);

    EXPECT_FALSE(surfaceHandler->HasReceivedTunnelLayerInfo());
}

/**
 * @tc.name: OnTunnelLayerInfoChanged011
 * @tc.desc: Test OnTunnelLayerInfoChanged early-returns when IsNewTunnelEnabled is false
 *           (line 161 if-condition is true), leaving pre-existing layer info and handler state unchanged.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceListenerTest, OnTunnelLayerInfoChanged011, TestSize.Level1)
{
    TunnelTest::ScopedNewTunnelSwitch scopedSwitch(false);
    auto node = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(node, nullptr);
    auto surfaceHandler = node->GetRSSurfaceHandler();
    ASSERT_NE(surfaceHandler, nullptr);

    constexpr uint64_t preExistingTunnelLayerId = 11001;
    constexpr uint32_t preExistingProperty = TUNNEL_PROP_BUFFER_ADDR;
    node->GetTunnelRuntimeState()->SetLayerInfo(preExistingTunnelLayerId, preExistingProperty);
    ASSERT_FALSE(surfaceHandler->HasReceivedTunnelLayerInfo());

    std::shared_ptr<RSRenderServiceListener> rsListener =
        std::make_shared<RSRenderServiceListener>(node, surfaceHandler, nullptr);

    constexpr uint64_t newTunnelLayerId = 11002;
    constexpr uint32_t newProperty = TUNNEL_PROP_DEVICE_COMMIT;
    TunnelLayerState state;
    state.tunnelLayerId = newTunnelLayerId;
    state.property = static_cast<TunnelLayerProperty>(newProperty);

    rsListener->OnTunnelLayerInfoChanged(state);

    uint64_t storedTunnelLayerId = 0;
    uint32_t storedProperty = TUNNEL_PROP_INVALID;
    node->GetTunnelRuntimeState()->GetLayerInfo(storedTunnelLayerId, storedProperty);
    EXPECT_NE(storedTunnelLayerId, newTunnelLayerId);
    EXPECT_NE(storedProperty, newProperty);
    EXPECT_FALSE(surfaceHandler->HasReceivedTunnelLayerInfo());
}

/**
 * @tc.name: OnCleanCache002
 * @tc.desc: Test OnCleanCache when surfaceBufferInterface_ is nullptr, should set PENDING_ON_CLEAN_CACHE_BIT.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceListenerTest, OnCleanCache002, TestSize.Level1)
{
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(node, nullptr);
    auto surfaceHandler = node->GetRSSurfaceHandler();
    ASSERT_NE(surfaceHandler, nullptr);
    ASSERT_NE(surfaceHandler->GetBuffer(), nullptr);

    std::weak_ptr<RSSurfaceRenderNode> emptyNode;
    auto clientComposer = std::make_shared<RSComposerClientManager>();
    std::shared_ptr<RSRenderServiceListener> rsListener =
        std::make_shared<RSRenderServiceListener>(emptyNode, surfaceHandler, clientComposer);
    ASSERT_NE(rsListener, nullptr);

    rsListener->pendingCallbackBits_ = 0;
    uint32_t bufSeqNum = 0;
    rsListener->OnCleanCache(&bufSeqNum);

    EXPECT_TRUE((rsListener->pendingCallbackBits_ & rsListener->PENDING_ON_CLEAN_CACHE_BIT) != 0);
    EXPECT_EQ(rsListener->cleanCacheBufSeqNum_, bufSeqNum);
}

/**
 * @tc.name: OnCleanCache003
 * @tc.desc: Test OnCleanCache when surfaceBufferInterface_ is valid, should NOT set PENDING_ON_CLEAN_CACHE_BIT.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceListenerTest, OnCleanCache003, TestSize.Level1)
{
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(node, nullptr);
    auto surfaceHandler = node->GetRSSurfaceHandler();
    ASSERT_NE(surfaceHandler, nullptr);

    auto clientComposer = std::make_shared<RSComposerClientManager>();
    std::shared_ptr<RSRenderServiceListener> rsListener =
        std::make_shared<RSRenderServiceListener>(node, surfaceHandler, clientComposer);
    ASSERT_NE(rsListener, nullptr);

    rsListener->pendingCallbackBits_ = 0;
    uint32_t bufSeqNum = 0;
    rsListener->OnCleanCache(&bufSeqNum);

    EXPECT_TRUE((rsListener->pendingCallbackBits_ & rsListener->PENDING_ON_CLEAN_CACHE_BIT) == 0);
}

/**
 * @tc.name: ProcessPendingCallbacks003
 * @tc.desc: Test ProcessPendingCallbacks when curBuffer is nullptr, should early return without calling OnCleanCache.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceListenerTest, ProcessPendingCallbacks003, TestSize.Level1)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(node, nullptr);
    auto surfaceHandler = node->GetRSSurfaceHandler();
    ASSERT_NE(surfaceHandler, nullptr);
    ASSERT_EQ(surfaceHandler->GetBuffer(), nullptr);

    std::weak_ptr<RSSurfaceRenderNode> emptyNode;
    auto clientComposer = std::make_shared<RSComposerClientManager>();
    std::shared_ptr<RSRenderServiceListener> rsListener =
        std::make_shared<RSRenderServiceListener>(emptyNode, surfaceHandler, clientComposer);
    ASSERT_NE(rsListener, nullptr);

    rsListener->isInterfaceDirty_ = true;
    rsListener->pendingCallbackBits_ = rsListener->PENDING_ON_CLEAN_CACHE_BIT;
    rsListener->cleanCacheBufSeqNum_ = 0;

    rsListener->ProcessPendingCallbacks();

    EXPECT_EQ(rsListener->pendingCallbackBits_, 0);
}

/**
 * @tc.name: ProcessPendingCallbacks004
 * @tc.desc: Test ProcessPendingCallbacks when curBuffer SeqNum mismatches, should early return.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceListenerTest, ProcessPendingCallbacks004, TestSize.Level1)
{
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(node, nullptr);
    auto surfaceHandler = node->GetRSSurfaceHandler();
    ASSERT_NE(surfaceHandler, nullptr);
    auto curBuffer = surfaceHandler->GetBuffer();
    ASSERT_NE(curBuffer, nullptr);

    std::weak_ptr<RSSurfaceRenderNode> emptyNode;
    auto clientComposer = std::make_shared<RSComposerClientManager>();
    std::shared_ptr<RSRenderServiceListener> rsListener =
        std::make_shared<RSRenderServiceListener>(emptyNode, surfaceHandler, clientComposer);
    ASSERT_NE(rsListener, nullptr);

    constexpr uint32_t mismatchSeqNum = 0xFFFFFFFF;
    ASSERT_NE(mismatchSeqNum, curBuffer->GetSeqNum());

    rsListener->isInterfaceDirty_ = true;
    rsListener->pendingCallbackBits_ = rsListener->PENDING_ON_CLEAN_CACHE_BIT;
    rsListener->cleanCacheBufSeqNum_ = mismatchSeqNum;

    rsListener->ProcessPendingCallbacks();

    EXPECT_EQ(rsListener->pendingCallbackBits_, 0);
}

/**
 * @tc.name: ProcessPendingCallbacks005
 * @tc.desc: Test ProcessPendingCallbacks when curBuffer SeqNum matches, should call OnCleanCache.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceListenerTest, ProcessPendingCallbacks005, TestSize.Level1)
{
    auto node = RSTestUtil::CreateSurfaceNodeWithBuffer();
    ASSERT_NE(node, nullptr);
    auto surfaceHandler = node->GetRSSurfaceHandler();
    ASSERT_NE(surfaceHandler, nullptr);
    auto curBuffer = surfaceHandler->GetBuffer();
    ASSERT_NE(curBuffer, nullptr);
    uint32_t realSeqNum = curBuffer->GetSeqNum();

    std::weak_ptr<RSSurfaceRenderNode> emptyNode;
    auto clientComposer = std::make_shared<RSComposerClientManager>();
    std::shared_ptr<RSRenderServiceListener> rsListener =
        std::make_shared<RSRenderServiceListener>(emptyNode, surfaceHandler, clientComposer);
    ASSERT_NE(rsListener, nullptr);

    rsListener->isInterfaceDirty_ = true;
    rsListener->pendingCallbackBits_ = rsListener->PENDING_ON_CLEAN_CACHE_BIT;
    rsListener->cleanCacheBufSeqNum_ = realSeqNum;

    rsListener->ProcessPendingCallbacks();

    EXPECT_TRUE((rsListener->pendingCallbackBits_ & rsListener->PENDING_ON_CLEAN_CACHE_BIT) != 0);
}
} // namespace OHOS::Rosen
