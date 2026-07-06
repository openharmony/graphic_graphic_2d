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
#include "gtest/gtest.h"
#include "limit_number.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/main_thread/rs_render_service_listener.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_test_util.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
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
 * @tc.desc: Test OnTunnelLayerInfoChanged stores layer info in RSTunnelRuntimeStore and requests VSync.
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
    RSTunnelRuntimeStore::GetLayerInfoOrDefault(node->GetId(), storedTunnelLayerId, storedProperty);
    EXPECT_EQ(storedTunnelLayerId, testTunnelLayerId);
    EXPECT_EQ(storedProperty, testProperty);
 
    RSTunnelRuntimeStore::Erase(node->GetId());
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
    RSTunnelRuntimeStore::SetLayerInfo(node->GetId(), initialTunnelLayerId, initialProperty);
 
    TunnelLayerState state;
    state.tunnelLayerId = 0;
    state.property = TUNNEL_PROP_INVALID;
    rsListener->OnTunnelLayerInfoChanged(state);
 
    uint64_t storedTunnelLayerId = 0;
    uint32_t storedProperty = TUNNEL_PROP_INVALID;
    RSTunnelRuntimeStore::GetLayerInfoOrDefault(node->GetId(), storedTunnelLayerId, storedProperty);
    EXPECT_EQ(storedTunnelLayerId, 0u);
    EXPECT_EQ(storedProperty, TUNNEL_PROP_INVALID);
 
    RSTunnelRuntimeStore::Erase(node->GetId());
}
} // namespace OHOS::Rosen
