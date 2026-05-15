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
#include <vector>

#include "feature/hyper_graphic_manager/hgm_render_context.h"
#include "gtest/gtest.h"
#include "limit_number.h"

#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/main_thread/rs_render_service_listener.h"
#include "pipeline/render_thread/rs_base_surface_util.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_test_util.h"
#include "feature/tunnel_layer/rs_tunnel_test_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
using namespace TunnelTest;

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
void RSRenderServiceListenerTest::SetUp()
{
}
void RSRenderServiceListenerTest::TearDown() {}

/**
 * @tc.name: CreateAndDestroy001
 * @tc.desc: Create listener with RSSurfaceRenderNode.
 * @tc.type: FUNC
 * @tc.require:issueI590LM
 */
HWTEST_F(RSRenderServiceListenerTest, CreateAndDestroy001, TestSize.Level1)
{
    // nullptr test
    std::weak_ptr<RSSurfaceRenderNode> wp;
    std::shared_ptr<RSRenderServiceListener> rsListener = std::make_shared<RSRenderServiceListener>(wp, nullptr);
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
    // nullptr test and early return
    std::weak_ptr<RSSurfaceRenderNode> wp;
    std::shared_ptr<RSRenderServiceListener> rsListener = std::make_shared<RSRenderServiceListener>(wp, nullptr);
    rsListener->OnTunnelHandleChange();

    std::shared_ptr<RSSurfaceRenderNode> node = RSTestUtil::CreateSurfaceNode();
    rsListener = std::make_shared<RSRenderServiceListener>(node, nullptr);
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
    // nullptr test and early return
    std::weak_ptr<RSSurfaceRenderNode> wp;
    auto clientComposer = std::make_shared<RSComposerClientManager>();
    std::shared_ptr<RSRenderServiceListener> rsListener =
        std::make_shared<RSRenderServiceListener>(wp, clientComposer);
    uint32_t bufSeqNum = 0;
    rsListener->OnCleanCache(&bufSeqNum);

    std::shared_ptr<RSSurfaceRenderNode> node = RSTestUtil::CreateSurfaceNode();
    rsListener = std::make_shared<RSRenderServiceListener>(node, clientComposer);
    rsListener->OnCleanCache(&bufSeqNum);
    ASSERT_EQ(node->GetRSSurfaceHandler()->GetAvailableBufferCount(), 0);
    ASSERT_TRUE(bufSeqNum >= 0);
}

/**
 * @tc.name: ForceRefresh001
 * @tc.desc: Test ForceRefresh
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSRenderServiceListenerTest, ForceRefresh001, TestSize.Level1)
{
    RSMainThread::Instance()->composerClientManager_ = std::make_shared<RSComposerClientManager>();
    std::shared_ptr<RSSurfaceRenderNode> node = RSTestUtil::CreateSurfaceNode();
    std::shared_ptr<RSRenderServiceListener> rsListener =
        std::make_shared<RSRenderServiceListener>(node, std::make_shared<RSComposerClientManager>());
    rsListener = std::make_shared<RSRenderServiceListener>(node, std::make_shared<RSComposerClientManager>());

    ASSERT_FALSE(node->IsLayerTop());
    ASSERT_FALSE(node->IsTopLayerForceRefresh());
    rsListener->ForceRefresh(node);

    node->SetLayerTop(true);
    ASSERT_TRUE(node->IsLayerTop());
    ASSERT_TRUE(node->IsTopLayerForceRefresh());
    rsListener->ForceRefresh(node);

    node->SetLayerTop(true, false);
    ASSERT_TRUE(node->IsLayerTop());
    ASSERT_FALSE(node->IsTopLayerForceRefresh());
    rsListener->ForceRefresh(node);

    node->SetLayerTop(false, true);
    ASSERT_FALSE(node->IsLayerTop());
    ASSERT_TRUE(node->IsTopLayerForceRefresh());
    rsListener->ForceRefresh(node);
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
    std::weak_ptr<RSSurfaceRenderNode> wp;
    std::shared_ptr<RSRenderServiceListener> rsListener = std::make_shared<RSRenderServiceListener>(wp, nullptr);
    rsListener->OnBufferAvailable();

    std::shared_ptr<RSSurfaceRenderNode> node = RSTestUtil::CreateSurfaceNode();
    rsListener = std::make_shared<RSRenderServiceListener>(node, nullptr);
    node->SetIsNotifyUIBufferAvailable(false);
    rsListener->OnBufferAvailable();
    ASSERT_EQ(node->IsNotifyUIBufferAvailable(), true);

    node->SetIsNotifyUIBufferAvailable(true);
    rsListener->OnBufferAvailable();
    ASSERT_EQ(node->IsNotifyUIBufferAvailable(), true);
}

/**
 * @tc.name: OnBufferAvailable002
 * @tc.desc: Test tunnel callback-thread path lets normal consume handle buffer when composer is unavailable.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceListenerTest, OnBufferAvailable002, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    auto context = CreateTunnelTestContext(true);
    ASSERT_TRUE(context.IsProducerReady());

    ASSERT_EQ(context.consumer->SetSurfaceSourceType(OHSurfaceSource::OH_SURFACE_SOURCE_DEFAULT), GSERROR_OK);
    ASSERT_TRUE(FlushOneBuffer(context.producer));
    context.surfaceHandler->SetAvailableBufferCount(1);
    ASSERT_TRUE(RSBaseSurfaceUtil::ConsumeAndUpdateBuffer(*context.surfaceHandler));
    auto normalBuffer = context.surfaceHandler->GetBuffer();
    ASSERT_NE(normalBuffer, nullptr);
    auto normalBufferOwnerCount = context.surfaceHandler->GetBufferOwnerCount();
    ASSERT_NE(normalBufferOwnerCount, nullptr);
    EXPECT_EQ(normalBufferOwnerCount->refCount_.load(), 1);

    TunnelLayerState state;
    ASSERT_TRUE(SetTunnelInfoForConsumer(context.consumer, state));
    context.node->SetTunnelLayerInfo(state.tunnelLayerId, state.property);
    context.node->GetTunnelRuntimeState().SetBuilding();
    context.node->GetTunnelRuntimeState().SetActiveFromTunnelLayerAvailable(
        context.node->GetTunnelRuntimeState().GetTunnelLayerGeneration());
    RSMainThread::Instance()->hgmRenderContext_ = std::make_shared<HgmRenderContext>(nullptr);
    ASSERT_TRUE(FlushOneBuffer(context.producer));

    auto rsListener = std::make_shared<RSRenderServiceListener>(context.node, nullptr);
    rsListener->OnBufferAvailable();

    EXPECT_EQ(context.surfaceHandler->GetBuffer(), normalBuffer);
    EXPECT_EQ(context.surfaceHandler->GetBufferOwnerCount(), normalBufferOwnerCount);
    EXPECT_EQ(context.surfaceHandler->GetHoldReturnValue(), nullptr);
    EXPECT_EQ(context.surfaceHandler->GetAvailableBufferCount(), 1);
    EXPECT_FALSE(context.node->GetTunnelRuntimeState().HasPendingBuffer());
}

/**
 * @tc.name: OnBufferAvailable003
 * @tc.desc: Test ACTIVE tunnel callback-thread path acquires and commits a buffer directly.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceListenerTest, OnBufferAvailable003, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    auto context = CreateTunnelTestContext(true);
    ASSERT_TRUE(context.IsProducerReady());

    ASSERT_EQ(context.consumer->SetSurfaceSourceType(OHSurfaceSource::OH_SURFACE_SOURCE_DEFAULT), GSERROR_OK);
    ASSERT_TRUE(FlushOneBuffer(context.producer));
    context.surfaceHandler->SetAvailableBufferCount(1);
    ASSERT_TRUE(RSBaseSurfaceUtil::ConsumeAndUpdateBuffer(*context.surfaceHandler));
    auto normalBufferOwnerCount = context.surfaceHandler->GetBufferOwnerCount();
    ASSERT_NE(normalBufferOwnerCount, nullptr);
    EXPECT_EQ(normalBufferOwnerCount->refCount_.load(), 1);

    TunnelLayerState state;
    ASSERT_TRUE(SetTunnelInfoForConsumer(context.consumer, state));
    context.node->SetTunnelLayerInfo(state.tunnelLayerId, state.property);
    context.node->GetTunnelRuntimeState().SetBuilding();
    context.node->GetTunnelRuntimeState().SetActiveFromTunnelLayerAvailable(
        context.node->GetTunnelRuntimeState().GetTunnelLayerGeneration());
    auto& tunnelRuntime = context.node->GetTunnelRuntimeState();
    ASSERT_EQ(tunnelRuntime.TryClaimByMain(true), RSTunnelRuntimeState::ClaimResult::GO_NORMAL);
    tunnelRuntime.OnRenderCommitDone();
    ASSERT_EQ(tunnelRuntime.GetPhase(), RSTunnelRuntimeState::Phase::NORMAL_COMMITTED);
    ScopedRegisteredSurfaceNode registeredNode(context.node);
    ASSERT_TRUE(registeredNode.IsRegistered());
    auto connection = sptr<RecordingRenderToComposerConnection>::MakeSptr();
    auto composerManager = CreateRecordingComposerManager(context.node->GetId(), connection);
    ASSERT_NE(composerManager, nullptr);
    ASSERT_TRUE(FlushOneBuffer(context.producer));

    auto rsListener = std::make_shared<RSRenderServiceListener>(context.node, composerManager);
    rsListener->OnBufferAvailable();

    EXPECT_TRUE(connection->commitTunnelCalled);
    EXPECT_EQ(connection->lastSurfaceId, context.consumer->GetUniqueId());
    EXPECT_EQ(connection->lastTunnelLayerId, state.tunnelLayerId);
    EXPECT_NE(connection->lastBufferId, 0u);
    EXPECT_FALSE(context.node->GetTunnelRuntimeState().HasPendingBuffer());
    ASSERT_NE(context.surfaceHandler->GetBuffer(), nullptr);
    EXPECT_EQ(context.surfaceHandler->GetBuffer()->GetBufferId(), connection->lastBufferId);
    EXPECT_EQ(normalBufferOwnerCount->refCount_.load(), 0);
    auto firstTunnelBufferOwnerCount = context.surfaceHandler->GetBufferOwnerCount();
    ASSERT_NE(firstTunnelBufferOwnerCount, nullptr);
    EXPECT_EQ(firstTunnelBufferOwnerCount->refCount_.load(), 1);
    EXPECT_TRUE(context.surfaceHandler->IsCurrentFrameBufferConsumed());
    EXPECT_EQ(context.surfaceHandler->GetPreBuffer(), nullptr);
    EXPECT_EQ(context.surfaceHandler->GetPreBufferOwnerCount(), nullptr);
    EXPECT_EQ(context.node->GetTunnelRuntimeState().GetTunnelState(),
        RSTunnelRuntimeState::TunnelState::ACTIVE);

    auto firstBufferId = connection->lastBufferId;
    ASSERT_TRUE(FlushOneBuffer(context.producer));
    rsListener->OnBufferAvailable();
    EXPECT_TRUE(connection->commitTunnelCalled);
    EXPECT_NE(connection->lastBufferId, firstBufferId);
    EXPECT_TRUE(context.surfaceHandler->IsCurrentFrameBufferConsumed());
    EXPECT_EQ(normalBufferOwnerCount->refCount_.load(), 0);
    EXPECT_EQ(firstTunnelBufferOwnerCount->refCount_.load(), 0);
    EXPECT_EQ(context.surfaceHandler->GetPreBuffer(), nullptr);
    EXPECT_EQ(context.surfaceHandler->GetPreBufferOwnerCount(), nullptr);
}

/**
 * @tc.name: OnBufferAvailable004
 * @tc.desc: Test BUILDING tunnel callback-thread path does not acquire or activate tunnel direct.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceListenerTest, OnBufferAvailable004, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    auto context = CreateTunnelTestContext(true);
    ASSERT_TRUE(context.IsProducerReady());

    ASSERT_EQ(context.consumer->SetSurfaceSourceType(OHSurfaceSource::OH_SURFACE_SOURCE_DEFAULT), GSERROR_OK);
    TunnelLayerState state;
    ASSERT_TRUE(SetTunnelInfoForConsumer(context.consumer, state));
    context.node->SetTunnelLayerInfo(state.tunnelLayerId, state.property);
    context.node->GetTunnelRuntimeState().SetBuilding();
    auto connection = sptr<RecordingRenderToComposerConnection>::MakeSptr();
    auto composerManager = CreateRecordingComposerManager(context.node->GetId(), connection);
    ASSERT_NE(composerManager, nullptr);
    ASSERT_TRUE(FlushOneBuffer(context.producer));

    auto rsListener = std::make_shared<RSRenderServiceListener>(context.node, composerManager);
    rsListener->OnBufferAvailable();

    EXPECT_FALSE(connection->commitTunnelCalled);
    EXPECT_FALSE(context.node->GetTunnelRuntimeState().HasPendingBuffer());
    EXPECT_EQ(context.node->GetTunnelRuntimeState().GetTunnelState(),
        RSTunnelRuntimeState::TunnelState::BUILDING);
    EXPECT_EQ(context.surfaceHandler->GetAvailableBufferCount(), 1);
}

/**
 * @tc.name: OnBufferAvailable006
 * @tc.desc: Test listener keeps tunnel buffer for normal consume fallback when direct IPC commit fails.
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceListenerTest, OnBufferAvailable006, TestSize.Level1)
{
    ScopedNewTunnelSwitch newTunnelSwitch(true);
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto node = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(node, nullptr);
    RSTestUtil::UnregisterConsumerListener();

    auto surfaceHandler = node->GetMutableRSSurfaceHandler();
    ASSERT_NE(surfaceHandler, nullptr);
    auto consumer = surfaceHandler->GetConsumer();
    ASSERT_NE(consumer, nullptr);
    sptr<IBufferProducer> producerToken = consumer->GetProducer();
    auto producer = Surface::CreateSurfaceAsProducer(producerToken);
    ASSERT_NE(producer, nullptr);

    std::vector<LayerStateChange> results;
    ASSERT_EQ(producer->RegisterLayerStateChangedListener(
                  [&results](LayerStateChange state) { results.emplace_back(state); }),
        GSERROR_OK);
    TunnelLayerState state;
    ASSERT_TRUE(SetTunnelInfoForConsumer(consumer, state));
    node->SetTunnelLayerInfo(state.tunnelLayerId, state.property);
    node->GetTunnelRuntimeState().SetBuilding();
    node->GetTunnelRuntimeState().SetActiveFromTunnelLayerAvailable(
        node->GetTunnelRuntimeState().GetTunnelLayerGeneration());
    ScopedRegisteredSurfaceNode registeredNode(node);
    ASSERT_TRUE(registeredNode.IsRegistered());

    auto listener = std::make_shared<RSRenderServiceListener>(node, std::make_shared<RSComposerClientManager>());
    ASSERT_TRUE(FlushOneBuffer(producer));
    listener->OnBufferAvailable();

    EXPECT_TRUE(results.empty());
    uint64_t actualTunnelLayerId = 0;
    uint32_t actualProperty = TUNNEL_PROP_INVALID;
    node->GetTunnelLayerInfo(actualTunnelLayerId, actualProperty);
    EXPECT_EQ(actualTunnelLayerId, 0u);
    EXPECT_EQ(actualProperty, TUNNEL_PROP_INVALID);
    EXPECT_EQ(node->GetTunnelRuntimeState().GetTunnelState(),
        RSTunnelRuntimeState::TunnelState::BUILDING);
    EXPECT_TRUE(node->GetTunnelRuntimeState().HasPendingBuffer());
    EXPECT_EQ(surfaceHandler->GetHoldReturnValue(), nullptr);
    EXPECT_EQ(surfaceHandler->GetAvailableBufferCount(), 1);

    ASSERT_TRUE(MoveRuntimePendingToNormalHold(node, surfaceHandler, consumer));
    EXPECT_TRUE(RSBaseSurfaceUtil::ConsumeAndUpdateBuffer(*surfaceHandler));
    ASSERT_NE(surfaceHandler->GetBuffer(), nullptr);
    EXPECT_TRUE(surfaceHandler->IsCurrentFrameBufferConsumed());
    EXPECT_EQ(surfaceHandler->GetHoldReturnValue(), nullptr);
    RSTestUtil::UnregisterConsumerListener();
}

/**
 * @tc.name: SetBufferInfoAndRequest001
 * @tc.desc: Test SetBufferInfoAndRequest
 * @tc.type: FUNC
 * @tc.require: issue20841
 */
HWTEST_F(RSRenderServiceListenerTest, SetBufferInfoAndRequest001, TestSize.Level1)
{
    std::weak_ptr<RSSurfaceRenderNode> wp;
    std::shared_ptr<RSRenderServiceListener> rsListener = std::make_shared<RSRenderServiceListener>(wp, nullptr);
    std::shared_ptr<RSSurfaceRenderNode> node = RSTestUtil::CreateSurfaceNode();
    NodeId id = 0;
    std::shared_ptr<RSSurfaceHandler> handler = std::make_shared<RSSurfaceHandler>(id);
    rsListener->SetBufferInfoAndRequest(node, handler, handler->GetConsumer(), true);
    ASSERT_EQ(node->GetAncoFlags(), 0);
    rsListener->SetBufferInfoAndRequest(node, handler, handler->GetConsumer(), false);
    ASSERT_EQ(node->GetAncoFlags(), 0);
}

} // namespace OHOS::Rosen
