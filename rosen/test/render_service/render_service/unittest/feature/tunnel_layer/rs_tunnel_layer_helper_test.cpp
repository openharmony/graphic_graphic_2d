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
constexpr uint64_t TEST_SURFACE_NODE_TUNNEL_LAYER_ID = 8001;
constexpr RSLayerId TEST_UNI_LAYER_ID = 9999;

void ClearUiCaptureTasks(RSMainThread& mainThread)
{
    mainThread.pendingNonSystemUiCaptureTasks_.clear();
    while (!mainThread.nonSystemUiCaptureTasks_.empty()) {
        mainThread.nonSystemUiCaptureTasks_.pop();
    }
    mainThread.pendingSystemUiCaptureTasks_.clear();
    while (!mainThread.systemUiCaptureTasks_.empty()) {
        mainThread.systemUiCaptureTasks_.pop();
    }
}

sptr<ScreenPropertyBase> MakePowerStatusProperty(ScreenPowerStatus status)
{
    return sptr<ScreenProperty<uint32_t>>::MakeSptr(static_cast<uint32_t>(status));
}

class ScopedScreenPowerStatus {
public:
    ScopedScreenPowerStatus(ScreenId screenId, ScreenPowerStatus status) : screenId_(screenId)
    {
        RSTunnelLayerHelper::NotifyScreenPowerStatusChanged(screenId_, MakePowerStatusProperty(status));
    }
    ~ScopedScreenPowerStatus()
    {
        RSTunnelLayerHelper::NotifyScreenPowerStatusChanged(
            screenId_, MakePowerStatusProperty(ScreenPowerStatus::POWER_STATUS_ON));
    }

private:
    ScreenId screenId_;
};
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
        RSTunnelRouteArbiter::SetTunnelSolePresentLayer(false);
    }
};

/**
 * @tc.name: ResolveTunnelLayerInfoAndProperty001
 * @tc.desc: Test ResolveTunnelLayerInfo reads node-owned tunnel layer info with and without state.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerHelperTest, ResolveTunnelLayerInfoAndProperty001, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    struct TestCase {
        const char* name;
        uint64_t nodeTunnelLayerId;
        uint32_t nodeProperty;
    };
    const std::vector<TestCase> testCases = {
        { "node_without_state", 0, TUNNEL_PROP_INVALID },
        { "node_with_layer_info", TEST_NODE_TUNNEL_LAYER_ID, TUNNEL_PROP_BUFFER_ADDR | TUNNEL_PROP_RS_FORCE },
        { "node_with_invalid_property", TEST_NODE_TUNNEL_LAYER_ID, TUNNEL_PROP_INVALID },
    };

    for (const auto& testCase : testCases) {
        SCOPED_TRACE(testing::Message() << "case=" << testCase.name);
        auto context = CreateTunnelTestContext(false);
        ASSERT_TRUE(context.IsBaseReady());

        if (testCase.nodeTunnelLayerId != 0 || testCase.nodeProperty != TUNNEL_PROP_INVALID) {
            context.node->GetTunnelRuntimeState()->SetLayerInfo(
                testCase.nodeTunnelLayerId, testCase.nodeProperty);
        }
        uint64_t tunnelLayerId = 0;
        uint32_t property = TUNNEL_PROP_INVALID;
        EXPECT_TRUE(RSTunnelLayerHelper::ResolveTunnelLayerInfo(context.node, tunnelLayerId, property));
        EXPECT_EQ(tunnelLayerId, testCase.nodeTunnelLayerId);
        EXPECT_EQ(property, testCase.nodeProperty);
    }
}

/**
 * @tc.name: ResolveTunnelLayerInfo_RejectsInvalidInputs
 * @tc.desc: Test ResolveTunnelLayerInfo rejects null node and reports defaults for state-less nodes.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerHelperTest, ResolveTunnelLayerInfo_RejectsInvalidInputs, TestSize.Level1)
{
    uint64_t tunnelLayerId = TEST_FIRST_CONSUMER_TUNNEL_LAYER_ID;
    uint32_t property = TUNNEL_PROP_BUFFER_ADDR;

    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    EXPECT_FALSE(RSTunnelLayerHelper::ResolveTunnelLayerInfo(nullptr, tunnelLayerId, property));

    auto context = CreateTunnelTestContext(false);
    ASSERT_TRUE(context.IsBaseReady());
    tunnelLayerId = TEST_FIRST_CONSUMER_TUNNEL_LAYER_ID;
    property = TUNNEL_PROP_BUFFER_ADDR;
    EXPECT_TRUE(RSTunnelLayerHelper::ResolveTunnelLayerInfo(context.node, tunnelLayerId, property));
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
 * @tc.name: TunnelRuntimeClearedWhenConsumerChanged001
 * @tc.desc: Test surface node resets node-owned tunnel runtime state when consumer changes.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerHelperTest, TunnelRuntimeClearedWhenConsumerChanged001, TestSize.Level1)
{
#ifndef ROSEN_CROSS_PLATFORM
    auto context = CreateTunnelTestContext(true);
    ASSERT_TRUE(context.IsProducerReady());

    constexpr uint32_t property = TUNNEL_PROP_BUFFER_ADDR | TUNNEL_PROP_DEVICE_COMMIT;
    auto& tunnelRuntimeState = *context.node->GetTunnelRuntimeState();
    tunnelRuntimeState.SetLayerInfo(TEST_SURFACE_NODE_TUNNEL_LAYER_ID, property);
    tunnelRuntimeState.SetBuilding();
    ASSERT_TRUE(tunnelRuntimeState.SetActiveFromTunnelLayerAvailable(tunnelRuntimeState.GetTunnelLayerGeneration()));

    auto nextConsumer = IConsumerSurface::Create("TunnelConsumer2");
    ASSERT_NE(nextConsumer, nullptr);
    context.node->SetConsumer(nextConsumer);
    // New consumer resets the node-owned state; the node reports no tunnel state afterwards.
    EXPECT_EQ(context.node->TryGetTunnelRuntimeState(), nullptr);
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
        auto& tunnelRuntime = *context.node->GetTunnelRuntimeState();
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
    auto& tunnelRuntime = *context.node->GetTunnelRuntimeState();
    tunnelRuntime.SetBuilding();
    ASSERT_TRUE(tunnelRuntime.SetActiveFromTunnelLayerAvailable(tunnelRuntime.GetTunnelLayerGeneration()));
    context.surfaceHandler->SetAvailableBufferCount(1);
    ScopedRegisteredSurfaceNode registeredTunnelNode(context.node);
    ASSERT_TRUE(registeredTunnelNode.IsRegistered());

    auto captureNode = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(captureNode, nullptr);
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(captureNode);
    mainThread->AddUiCaptureTask(captureNode->GetId(), false, []() {});
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
        *context.node->GetTunnelRuntimeState()).buffer, nullptr);

    auto& tunnelRuntime = *context.node->GetTunnelRuntimeState();
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

    auto& tunnelRuntime = *context.node->GetTunnelRuntimeState();
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
    TunnelLayerState state;
    ASSERT_TRUE(SetTunnelInfoForConsumer(context.consumer, state));
    auto& tunnelRuntime = *context.node->GetTunnelRuntimeState();
    tunnelRuntime.SetLayerInfo(state.tunnelLayerId, state.property);
    tunnelRuntime.SetBuilding();
    ASSERT_TRUE(tunnelRuntime.SetActiveFromTunnelLayerAvailable(tunnelRuntime.GetTunnelLayerGeneration()));
    ScopedRegisteredSurfaceNode registeredNode(context.node);
    ASSERT_TRUE(registeredNode.IsRegistered());
    context.surfaceHandler->SetAvailableBufferCount(1);

    auto connection = sptr<RecordingRenderToComposerConnection>::MakeSptr();
    connection->commitTunnelResult = GRAPHIC_DISPLAY_FAILURE;
    auto composerClientManager = CreateRecordingComposerManager(context.node->GetId(), connection);
    ASSERT_NE(composerClientManager, nullptr);
    RSTunnelRouteArbiter::consecutiveSolePresentCount_.store(TUNNEL_STABLE_THRESHOLD, std::memory_order_release);
    RSTunnelRouteArbiter::RefreshGlobalTriggerSnapshot();
    RSTunnelRouteArbiter::SetTunnelSolePresentLayer(true);
    EXPECT_FALSE(RSTunnelLayerHelper::TryCommitBufferDirect(
        context.node, composerClientManager, false, tunnelRuntime));
    EXPECT_TRUE(connection->commitTunnelCalled);
    EXPECT_EQ(tunnelRuntime.GetTunnelState(), RSTunnelRuntimeState::TunnelState::NOT_TUNNEL);
    // Failure path must release the acquired buffer back to the producer and clear layer info so
    // the surface is not stuck in ACTIVE with an empty consumer queue. The release callback on
    // the pending BufferOwnerCount is cleared so DecRef/~BufferOwnerCount cannot fire a second
    // ReleaseBufferById on the same bufferId.
    uint64_t clearedTunnelLayerId = 1;
    uint32_t clearedProperty = TUNNEL_PROP_BUFFER_ADDR;
    tunnelRuntime.GetLayerInfo(clearedTunnelLayerId, clearedProperty);
    EXPECT_EQ(clearedTunnelLayerId, 0u);
    EXPECT_EQ(clearedProperty, TUNNEL_PROP_INVALID);
    EXPECT_EQ(context.consumer->GetAvailableBufferCount(), 0u);
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
    auto& tunnelRuntime = *context.node->GetTunnelRuntimeState();
    tunnelRuntime.SetLayerInfo(state.tunnelLayerId, state.property);
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
    RSTunnelRouteArbiter::consecutiveSolePresentCount_.store(TUNNEL_STABLE_THRESHOLD, std::memory_order_release);
    RSTunnelRouteArbiter::RefreshGlobalTriggerSnapshot();
    RSTunnelRouteArbiter::SetTunnelSolePresentLayer(true);
    ASSERT_TRUE(RSTunnelLayerHelper::TryCommitBufferDirect(
        context.node, composerClientManager, false, tunnelRuntime));
    EXPECT_TRUE(currentBufferReleased);
    EXPECT_EQ(currentBufferOwnerCount->refCount_.load(), 0);
    auto tunnelBufferOwnerCount = context.surfaceHandler->GetBufferOwnerCount();
    ASSERT_NE(tunnelBufferOwnerCount, nullptr);

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

    auto& tunnelRuntime = *context.node->GetTunnelRuntimeState();
    tunnelRuntime.SetBuilding();
    tunnelRuntime.SetActiveFromTunnelLayerAvailable(tunnelRuntime.GetTunnelLayerGeneration());
    ScopedRegisteredSurfaceNode registeredNode(context.node);
    ASSERT_TRUE(registeredNode.IsRegistered());
    context.surfaceHandler->SetAvailableBufferCount(1);

    auto composerClientManager = std::make_shared<RSComposerClientManager>();
    EXPECT_FALSE(RSTunnelLayerHelper::TryCommitBufferDirect(context.node, composerClientManager, false,
        tunnelRuntime));
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
    context.surfaceHandler->MarkTunnelLayerInfoReceived();
    auto& tunnelRuntime = *context.node->GetTunnelRuntimeState();
    tunnelRuntime.SetLayerInfo(state.tunnelLayerId, state.property);
    tunnelRuntime.SetBuilding();
    tunnelRuntime.SetActiveFromTunnelLayerAvailable(tunnelRuntime.GetTunnelLayerGeneration());
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
    context.surfaceHandler->MarkTunnelLayerInfoReceived();
    auto& tunnelRuntime = *context.node->GetTunnelRuntimeState();
    tunnelRuntime.SetLayerInfo(state.tunnelLayerId, state.property);
    tunnelRuntime.SetBuilding();
    tunnelRuntime.SetActiveFromTunnelLayerAvailable(tunnelRuntime.GetTunnelLayerGeneration());
    RSTunnelRouteArbiter::SetTunnelSolePresentLayer(true);
    ASSERT_EQ(tunnelRuntime.TryClaimByMain(true), RSTunnelRuntimeState::ClaimResult::GO_NORMAL);
    tunnelRuntime.OnRenderCommitDone();
    ASSERT_EQ(tunnelRuntime.GetPhase(), RSTunnelRuntimeState::Phase::NORMAL_COMMITTED);
    ScopedRegisteredSurfaceNode registeredNode(context.node);
    ASSERT_TRUE(registeredNode.IsRegistered());
    context.node->screenId_ = 0;
    auto connection = sptr<RecordingRenderToComposerConnection>::MakeSptr();
    auto composerManager = CreateRecordingComposerManager(context.node->GetId(), connection);
    ASSERT_NE(composerManager, nullptr);
    RSTunnelRouteArbiter::consecutiveSolePresentCount_.store(TUNNEL_STABLE_THRESHOLD, std::memory_order_release);
    RSTunnelRouteArbiter::RefreshGlobalTriggerSnapshot();
    ASSERT_TRUE(FlushProducerBufferForTest(context.producer));
    context.surfaceHandler->SetAvailableBufferCount(1);

    auto surfaceHandler(context.node->GetRSSurfaceHandler());
    auto rsListener = std::make_shared<RSRenderServiceListener>(context.node, surfaceHandler, composerManager);
    rsListener->OnBufferAvailable();

    EXPECT_FALSE(connection->commitTunnelCalled);
    EXPECT_NE(connection->commitTunnelCallCount, 1u);
    EXPECT_NE(connection->lastSurfaceId, context.consumer->GetUniqueId());
    EXPECT_NE(connection->lastTunnelLayerId, state.tunnelLayerId);
    EXPECT_EQ(connection->lastBufferId, 0u);
    ASSERT_NE(context.surfaceHandler->GetBuffer(), nullptr);
    EXPECT_NE(context.surfaceHandler->GetBuffer()->GetSeqNum(), connection->lastBufferId);
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
    context.surfaceHandler->MarkTunnelLayerInfoReceived();
    auto& tunnelRuntime = *context.node->GetTunnelRuntimeState();
    tunnelRuntime.SetLayerInfo(state.tunnelLayerId, state.property);
    tunnelRuntime.SetBuilding();
    auto connection = sptr<RecordingRenderToComposerConnection>::MakeSptr();
    auto composerManager = CreateRecordingComposerManager(context.node->GetId(), connection);
    ASSERT_NE(composerManager, nullptr);
    context.surfaceHandler->SetAvailableBufferCount(0);

    auto surfaceHandler(context.node->GetRSSurfaceHandler());
    auto rsListener = std::make_shared<RSRenderServiceListener>(context.node, surfaceHandler, composerManager);
    rsListener->OnBufferAvailable();

    EXPECT_FALSE(connection->commitTunnelCalled);
    EXPECT_EQ(tunnelRuntime.GetTunnelState(), RSTunnelRuntimeState::TunnelState::BUILDING);
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
    sptr<IBufferConsumerListener> noopListener = new TunnelNoopConsumerListener();
    ASSERT_EQ(consumer->RegisterConsumerListener(noopListener), GSERROR_OK);
    sptr<IBufferProducer> producerToken = consumer->GetProducer();
    auto producer = Surface::CreateSurfaceAsProducer(producerToken);
    producer->SetQueueSize(TEST_QUEUE_SIZE);

    std::vector<LayerStateChange> results;
    ASSERT_EQ(producer->RegisterLayerStateChangedListener(
                  [&results](LayerStateChange state) { results.emplace_back(state); }),
        GSERROR_OK);
    TunnelLayerState state;
    ASSERT_TRUE(SetTunnelInfoForConsumer(consumer, state));
    surfaceHandler->MarkTunnelLayerInfoReceived();
    auto& tunnelRuntime = *node->GetTunnelRuntimeState();
    tunnelRuntime.SetLayerInfo(state.tunnelLayerId, state.property);
    tunnelRuntime.SetBuilding();
    tunnelRuntime.SetActiveFromTunnelLayerAvailable(tunnelRuntime.GetTunnelLayerGeneration());
    RSTunnelRouteArbiter::SetTunnelSolePresentLayer(true);
    ScopedRegisteredSurfaceNode registeredNode(node);
    ASSERT_TRUE(registeredNode.IsRegistered());

    auto listener =
        std::make_shared<RSRenderServiceListener>(node, surfaceHandler, std::make_shared<RSComposerClientManager>());
    ASSERT_TRUE(FlushProducerBufferForTest(producer));
    surfaceHandler->SetAvailableBufferCount(1);
    RSTunnelRouteArbiter::consecutiveSolePresentCount_.store(TUNNEL_STABLE_THRESHOLD, std::memory_order_release);
    RSTunnelRouteArbiter::RefreshGlobalTriggerSnapshot();
    listener->OnBufferAvailable();

    EXPECT_TRUE(results.empty());
    uint64_t actualTunnelLayerId = 0;
    uint32_t actualProperty = TUNNEL_PROP_INVALID;
    tunnelRuntime.GetLayerInfo(actualTunnelLayerId, actualProperty);
    EXPECT_EQ(actualTunnelLayerId, 0u);
    EXPECT_EQ(actualProperty, TUNNEL_PROP_INVALID);
    EXPECT_EQ(tunnelRuntime.GetTunnelState(), RSTunnelRuntimeState::TunnelState::NOT_TUNNEL);
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
    auto& tunnelRuntime = *context.node->GetTunnelRuntimeState();
    tunnelRuntime.SetLayerInfo(state.tunnelLayerId, state.property);
    tunnelRuntime.SetBuilding();
    ASSERT_TRUE(tunnelRuntime.SetActiveFromTunnelLayerAvailable(tunnelRuntime.GetTunnelLayerGeneration()));
    ScopedRegisteredSurfaceNode registeredNode(context.node);
    ASSERT_TRUE(registeredNode.IsRegistered());
    context.node->screenId_ = 0;

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
    RSTunnelRouteArbiter::consecutiveSolePresentCount_.store(TUNNEL_STABLE_THRESHOLD, std::memory_order_release);
    RSTunnelRouteArbiter::RefreshGlobalTriggerSnapshot();
    RSTunnelRouteArbiter::SetTunnelSolePresentLayer(true);

    EXPECT_TRUE(RSTunnelLayerHelper::TryCommitBufferDirect(
        context.node, composerClientManager, false, tunnelRuntime));
    constexpr int32_t NORMAL_BUFFER_REFCOUNT_AFTER_TUNNEL_RELEASE = 0;
    EXPECT_EQ(currentBufferOwnerCount->refCount_.load(), NORMAL_BUFFER_REFCOUNT_AFTER_TUNNEL_RELEASE);
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
    auto& tunnelRuntime = *context.node->GetTunnelRuntimeState();
    tunnelRuntime.SetLayerInfo(state.tunnelLayerId, state.property);
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
    RSTunnelRouteArbiter::consecutiveSolePresentCount_.store(TUNNEL_STABLE_THRESHOLD, std::memory_order_release);
    RSTunnelRouteArbiter::RefreshGlobalTriggerSnapshot();
    RSTunnelRouteArbiter::SetTunnelSolePresentLayer(true);

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
    auto& tunnelRuntime = *context.node->GetTunnelRuntimeState();
    tunnelRuntime.SetLayerInfo(state.tunnelLayerId, state.property);
    tunnelRuntime.SetBuilding();
    ASSERT_TRUE(tunnelRuntime.SetActiveFromTunnelLayerAvailable(tunnelRuntime.GetTunnelLayerGeneration()));
    ScopedRegisteredSurfaceNode registeredNode(context.node);
    ASSERT_TRUE(registeredNode.IsRegistered());

    auto connection = sptr<RecordingRenderToComposerConnection>::MakeSptr();
    connection->commitTunnelResult = GRAPHIC_DISPLAY_SUCCESS;
    auto composerClientManager = CreateRecordingComposerManager(context.node->GetId(), connection);
    ASSERT_NE(composerClientManager, nullptr);

    constexpr uint32_t existingBufferSize = TEST_BUFFER_SIZE / 2;
    sptr<SurfaceBuffer> existingBuffer = SurfaceBuffer::Create();
    ASSERT_NE(existingBuffer, nullptr);
    BufferRequestConfig existingRequestConfig = {
        .width = existingBufferSize,
        .height = existingBufferSize,
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
    RSTunnelRouteArbiter::consecutiveSolePresentCount_.store(TUNNEL_STABLE_THRESHOLD, std::memory_order_release);
    RSTunnelRouteArbiter::RefreshGlobalTriggerSnapshot();
    RSTunnelRouteArbiter::SetTunnelSolePresentLayer(true);

    EXPECT_FALSE(RSTunnelLayerHelper::TryCommitBufferDirect(context.node, composerClientManager, false, tunnelRuntime));
    EXPECT_FALSE(connection->commitTunnelCalled);
    EXPECT_EQ(tunnelRuntime.GetTunnelState(), RSTunnelRuntimeState::TunnelState::ACTIVE);
    EXPECT_EQ(context.consumer->GetAvailableBufferCount(), 0u);
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
    auto& tunnelRuntime = *context.node->GetTunnelRuntimeState();
    tunnelRuntime.SetLayerInfo(state.tunnelLayerId, state.property);
    tunnelRuntime.SetBuilding();
    ASSERT_TRUE(tunnelRuntime.SetActiveFromTunnelLayerAvailable(tunnelRuntime.GetTunnelLayerGeneration()));
    ScopedRegisteredSurfaceNode registeredNode(context.node);
    ASSERT_TRUE(registeredNode.IsRegistered());

    ASSERT_TRUE(FlushProducerBufferForTest(context.producer));
    context.surfaceHandler->SetAvailableBufferCount(1);
    RSTunnelRouteArbiter::consecutiveSolePresentCount_.store(TUNNEL_STABLE_THRESHOLD, std::memory_order_release);
    RSTunnelRouteArbiter::RefreshGlobalTriggerSnapshot();
    RSTunnelRouteArbiter::SetTunnelSolePresentLayer(true);

    EXPECT_FALSE(RSTunnelLayerHelper::TryCommitBufferDirect(
        context.node, nullptr, false, tunnelRuntime));
    uint64_t clearedTunnelLayerId = 1;
    uint32_t clearedProperty = state.property;
    tunnelRuntime.GetLayerInfo(clearedTunnelLayerId, clearedProperty);
    EXPECT_EQ(clearedTunnelLayerId, 0u);
    EXPECT_EQ(clearedProperty, TUNNEL_PROP_INVALID);
    EXPECT_EQ(tunnelRuntime.GetTunnelState(), RSTunnelRuntimeState::TunnelState::NOT_TUNNEL);
    EXPECT_EQ(context.consumer->GetAvailableBufferCount(), 0u);
}

/**
 * @tc.name: OnBufferAvailable_TunnelCandidate_ReceivesInfo
 * @tc.desc: Test RSRenderServiceListener::OnBufferAvailable evaluates HasReceivedTunnelLayerInfo()
 *          (and short-circuits the OR with SourceType) when the new-tunnel switch is on, covering the
 *          isTunnelCandidate=true sub-branch that routes to HandleListenerBuffer.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerHelperTest, OnBufferAvailable_TunnelCandidate_ReceivesInfo, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    auto context = CreateTunnelTestContext(false);
    ASSERT_TRUE(context.IsBaseReady());
    context.surfaceHandler->MarkTunnelLayerInfoReceived();
    EXPECT_TRUE(context.surfaceHandler->HasReceivedTunnelLayerInfo());

    auto rsListener = std::make_shared<RSRenderServiceListener>(
        context.node, context.surfaceHandler, std::make_shared<RSComposerClientManager>());
    rsListener->OnBufferAvailable();

    EXPECT_TRUE(context.surfaceHandler->HasReceivedTunnelLayerInfo());
}

/**
 * @tc.name: CanCommitBufferDirect_RejectsWhenSolePresentLayerFalse
 * @tc.desc: Cover the true branch of `if (!GetTunnelSolePresentLayer())` in CanCommitBufferDirect.
 *          When the main thread has not signalled sole-present, the helper must reject the direct
 *          commit before any composer IPC is attempted.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerHelperTest, CanCommitBufferDirect_RejectsWhenSolePresentLayerFalse, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    RSTunnelRouteArbiter::SetTunnelSolePresentLayer(false);
    auto context = CreateTunnelTestContext(true);
    ASSERT_TRUE(context.IsProducerReady());

    context.producer->SetQueueSize(TEST_QUEUE_SIZE);
    TunnelLayerState state;
    ASSERT_TRUE(SetTunnelInfoForConsumer(context.consumer, state));
    auto& tunnelRuntime = *context.node->GetTunnelRuntimeState();
    tunnelRuntime.SetLayerInfo(state.tunnelLayerId, state.property);
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
    RSTunnelRouteArbiter::consecutiveSolePresentCount_.store(TUNNEL_STABLE_THRESHOLD, std::memory_order_release);
    RSTunnelRouteArbiter::RefreshGlobalTriggerSnapshot();

    EXPECT_FALSE(RSTunnelLayerHelper::TryCommitBufferDirect(
        context.node, composerClientManager, false, tunnelRuntime));
    EXPECT_FALSE(connection->commitTunnelCalled);
}

/**
 * @tc.name: CanCommitBufferDirect_ProceedsWhenSolePresentLayerTrue
 * @tc.desc: Cover the false branch of `if (!GetTunnelSolePresentLayer())` in CanCommitBufferDirect.
 *          When the arbiter reports sole-present, the helper must proceed through AcquirePendingBuffer
 *          and TryCommitPendingBuffer so the recording connection observes the commit call.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerHelperTest, CanCommitBufferDirect_ProceedsWhenSolePresentLayerTrue, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    RSTunnelRouteArbiter::SetTunnelSolePresentLayer(true);
    auto context = CreateTunnelTestContext(true);
    ASSERT_TRUE(context.IsProducerReady());

    context.producer->SetQueueSize(TEST_QUEUE_SIZE);
    TunnelLayerState state;
    ASSERT_TRUE(SetTunnelInfoForConsumer(context.consumer, state));
    auto& tunnelRuntime = *context.node->GetTunnelRuntimeState();
    tunnelRuntime.SetLayerInfo(state.tunnelLayerId, state.property);
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
    RSTunnelRouteArbiter::consecutiveSolePresentCount_.store(TUNNEL_STABLE_THRESHOLD, std::memory_order_release);
    RSTunnelRouteArbiter::RefreshGlobalTriggerSnapshot();

    EXPECT_TRUE(RSTunnelLayerHelper::TryCommitBufferDirect(
        context.node, composerClientManager, false, tunnelRuntime));
    EXPECT_TRUE(connection->commitTunnelCalled);
}

/**
 * @tc.name: CreateTunnelLayer_ReturnsEarlyWhenUniBufferCountAbsent
 * @tc.desc: Cover the true branch of `if (!uniBufferCount)` in CreateTunnelLayer. When the composer
 *          context has no uni-render layer, the helper returns the freshly created layer without
 *          touching InsertUniOnDrawSet or SetUniBufferOwner.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerHelperTest, CreateTunnelLayer_ReturnsEarlyWhenUniBufferCountAbsent, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    auto context = CreateTunnelTestContext(false);
    ASSERT_TRUE(context.IsBaseReady());

    auto connection = sptr<RecordingRenderToComposerConnection>::MakeSptr();
    constexpr ScreenId testScreenId = 0;
    context.node->screenId_ = 0;
    auto composerClientManager = CreateRecordingComposerManager(context.node->GetId(), connection);
    ASSERT_NE(composerClientManager, nullptr);
    auto composerClient = composerClientManager->GetComposerClient(testScreenId);
    ASSERT_NE(composerClient, nullptr);
    auto composerContext = composerClient->GetComposerContext();
    ASSERT_NE(composerContext, nullptr);
    EXPECT_EQ(composerContext->GetUniRsLayer(), nullptr);

    auto bufferEntry = CreateTestBufferEntry();
    ASSERT_NE(bufferEntry.buffer, nullptr);
    ASSERT_NE(bufferEntry.bufferOwnerCount_, nullptr);
    auto layer = RSTunnelLayerHelper::CreateTunnelLayer(context.node, composerClientManager, bufferEntry);
    ASSERT_EQ(layer, nullptr);
}

/**
 * @tc.name: CreateTunnelLayer_InsertsUniOnDrawSet_WhenBufferPresent
 * @tc.desc: Cover the false branch of `if (!uniBufferCount)` and the true branch of
 *          `if (layer->GetBuffer())` in CreateTunnelLayer. When the composer context exposes a
 *          uni-render layer with a BufferOwnerCount and the new layer carries a buffer, the
 *          helper must record the (layerId, bufferId) pair via InsertUniOnDrawSet.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerHelperTest, CreateTunnelLayer_InsertsUniOnDrawSet_WhenBufferPresent, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    auto context = CreateTunnelTestContext(false);
    ASSERT_TRUE(context.IsBaseReady());

    auto connection = sptr<RecordingRenderToComposerConnection>::MakeSptr();
    auto composerClientManager = CreateRecordingComposerManager(context.node->GetId(), connection);
    ASSERT_NE(composerClientManager, nullptr);
    constexpr ScreenId testScreenId = 0;
    context.node->screenId_ = 0;
    auto composerClient = composerClientManager->GetComposerClient(testScreenId);
    ASSERT_NE(composerClient, nullptr);
    auto composerContext = composerClient->GetComposerContext();
    ASSERT_NE(composerContext, nullptr);

    auto uniLayer = RSSurfaceLayer::Create(TEST_UNI_LAYER_ID, composerContext);
    ASSERT_NE(uniLayer, nullptr);
    uniLayer->SetUniRenderFlag(true);
    auto uniBufferOwnerCount = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    uniLayer->SetBufferOwnerCount(uniBufferOwnerCount, true);
    composerContext->AddRSLayer(uniLayer);
    EXPECT_EQ(composerContext->GetUniRsLayer(), uniLayer);

    auto bufferEntry = CreateTestBufferEntry();
    ASSERT_NE(bufferEntry.buffer, nullptr);
    auto layer = RSTunnelLayerHelper::CreateTunnelLayer(context.node, composerClientManager, bufferEntry);
    ASSERT_EQ(layer, nullptr);
}

/**
 * @tc.name: CreateTunnelLayer_SkipsInsertUniOnDrawSet_WhenBufferAbsent
 * @tc.desc: Cover the false branch of `if (layer->GetBuffer())` in CreateTunnelLayer. When the new
 *          layer has no buffer attached, InsertUniOnDrawSet must be skipped so the uni owner map
 *          stays empty.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerHelperTest, CreateTunnelLayer_SkipsInsertUniOnDrawSet_WhenBufferAbsent, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    auto context = CreateTunnelTestContext(false);
    ASSERT_TRUE(context.IsBaseReady());

    auto connection = sptr<RecordingRenderToComposerConnection>::MakeSptr();
    auto composerClientManager = CreateRecordingComposerManager(context.node->GetId(), connection);
    ASSERT_NE(composerClientManager, nullptr);
    constexpr ScreenId testScreenId = 0;
    context.node->screenId_ = 0;
    auto composerClient = composerClientManager->GetComposerClient(testScreenId);
    ASSERT_NE(composerClient, nullptr);
    auto composerContext = composerClient->GetComposerContext();
    ASSERT_NE(composerContext, nullptr);

    auto uniLayer = RSSurfaceLayer::Create(TEST_UNI_LAYER_ID, composerContext);
    ASSERT_NE(uniLayer, nullptr);
    uniLayer->SetUniRenderFlag(true);
    auto uniBufferOwnerCount = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    uniLayer->SetBufferOwnerCount(uniBufferOwnerCount, true);
    composerContext->AddRSLayer(uniLayer);

    RSSurfaceHandler::SurfaceBufferEntry emptyBufferEntry;
    emptyBufferEntry.acquireFence = SyncFence::InvalidFence();
    ASSERT_EQ(emptyBufferEntry.buffer, nullptr);
    auto layer = RSTunnelLayerHelper::CreateTunnelLayer(context.node, composerClientManager, emptyBufferEntry);
    ASSERT_EQ(layer, nullptr);
    EXPECT_TRUE(uniBufferOwnerCount->uniOnDrawBuffersMap_.empty());
}

/**
 * @tc.name: CreateTunnelLayer_SetsUniBufferOwner_WhenBufferOwnerCountPresent
 * @tc.desc: Cover the true branch of `if (bufferEntry.bufferOwnerCount_)` in CreateTunnelLayer.
 *          When the pending buffer carries a BufferOwnerCount, the helper must propagate the
 *          uni layer's bufferId and screen id via SetUniBufferOwner.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerHelperTest, CreateTunnelLayer_SetsUniBufferOwner_WhenBufferOwnerCountPresent, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    auto context = CreateTunnelTestContext(false);
    ASSERT_TRUE(context.IsBaseReady());

    auto connection = sptr<RecordingRenderToComposerConnection>::MakeSptr();
    auto composerClientManager = CreateRecordingComposerManager(context.node->GetId(), connection);
    ASSERT_NE(composerClientManager, nullptr);
    constexpr ScreenId testScreenId = 0;
    context.node->screenId_ = 0;
    auto composerClient = composerClientManager->GetComposerClient(testScreenId);
    ASSERT_NE(composerClient, nullptr);
    auto composerContext = composerClient->GetComposerContext();
    ASSERT_NE(composerContext, nullptr);

    auto uniLayer = RSSurfaceLayer::Create(TEST_UNI_LAYER_ID, composerContext);
    ASSERT_NE(uniLayer, nullptr);
    uniLayer->SetUniRenderFlag(true);
    constexpr uint64_t uniBufferId = 4242;
    auto uniBufferOwnerCount = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    uniBufferOwnerCount->bufferId_ = uniBufferId;
    uniLayer->SetBufferOwnerCount(uniBufferOwnerCount, true);
    composerContext->AddRSLayer(uniLayer);

    auto bufferEntry = CreateTestBufferEntry();
    ASSERT_NE(bufferEntry.bufferOwnerCount_, nullptr);
    auto layer = RSTunnelLayerHelper::CreateTunnelLayer(context.node, composerClientManager, bufferEntry);
    ASSERT_EQ(layer, nullptr);
    EXPECT_TRUE(bufferEntry.bufferOwnerCount_->CheckLastUniBufferOwner(uniBufferId, context.node->GetScreenId()));
}

/**
 * @tc.name: CreateTunnelLayer_SkipsSetUniBufferOwner_WhenBufferOwnerCountAbsent
 * @tc.desc: Cover the false branch of `if (bufferEntry.bufferOwnerCount_)` in CreateTunnelLayer.
 *          When the pending buffer does not carry a BufferOwnerCount, the helper must skip the
 *          SetUniBufferOwner call (which would otherwise dereference the null owner count) and
 *          still return the freshly created layer.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerHelperTest, CreateTunnelLayer_SkipsSetUniBufferOwner_WhenBufferOwnerCountAbsent, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    auto context = CreateTunnelTestContext(false);
    ASSERT_TRUE(context.IsBaseReady());

    auto connection = sptr<RecordingRenderToComposerConnection>::MakeSptr();
    auto composerClientManager = CreateRecordingComposerManager(context.node->GetId(), connection);
    ASSERT_NE(composerClientManager, nullptr);
    constexpr ScreenId testScreenId = 0;
    context.node->screenId_ = 0;
    auto composerClient = composerClientManager->GetComposerClient(testScreenId);
    ASSERT_NE(composerClient, nullptr);
    auto composerContext = composerClient->GetComposerContext();
    ASSERT_NE(composerContext, nullptr);

    auto uniLayer = RSSurfaceLayer::Create(TEST_UNI_LAYER_ID, composerContext);
    ASSERT_NE(uniLayer, nullptr);
    uniLayer->SetUniRenderFlag(true);
    constexpr uint64_t uniBufferId = 7777;
    auto uniBufferOwnerCount = std::make_shared<RSSurfaceHandler::BufferOwnerCount>();
    uniBufferOwnerCount->bufferId_ = uniBufferId;
    uniLayer->SetBufferOwnerCount(uniBufferOwnerCount, true);
    composerContext->AddRSLayer(uniLayer);

    auto bufferEntry = CreateTestBufferEntry();
    ASSERT_NE(bufferEntry.buffer, nullptr);
    ASSERT_NE(bufferEntry.bufferOwnerCount_, nullptr);
    bufferEntry.bufferOwnerCount_ = nullptr;
    auto layer = RSTunnelLayerHelper::CreateTunnelLayer(context.node, composerClientManager, bufferEntry);
    ASSERT_EQ(layer, nullptr);
    EXPECT_TRUE(uniBufferOwnerCount->uniBufferOwnerSeqNumMap_.empty());
}

/**
 * @tc.name: BeginTunnelBuildingAndResetTunnelState001
 * @tc.desc: Cover the new helper BeginTunnelBuilding branches (no runtime state early-return,
 *          tunnelLayerId==0 DisableTunnelLayer, tunnelLayerId!=0 ResetStateKeepLayerInfo plus
 *          arbiter NOT_TUNNEL->BUILDING gate) and ResetTunnelState (null node no-op, state reset).
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerHelperTest, BeginTunnelBuildingAndResetTunnelState001, TestSize.Level1)
{
    RSTunnelRouteArbiter::OnDirectCompositionStable(TUNNEL_STABLE_THRESHOLD + 1);
    RSTunnelRouteArbiter::ResetConsecutiveSolePresentCount();

    auto context = CreateTunnelTestContext(false);
    ASSERT_TRUE(context.IsBaseReady());

    RSTunnelLayerHelper::BeginTunnelBuilding(*context.node, 0, TUNNEL_PROP_INVALID);
    EXPECT_EQ(context.node->TryGetTunnelRuntimeState(), nullptr);

    RSTunnelLayerHelper::ResetTunnelState(nullptr);

    auto& runtime = *context.node->GetTunnelRuntimeState();
    runtime.SetLayerInfo(TEST_NODE_TUNNEL_LAYER_ID, TUNNEL_PROP_BUFFER_ADDR);
    RSTunnelLayerHelper::ResetTunnelState(context.node);
    EXPECT_EQ(runtime.GetTunnelState(), RSTunnelRuntimeState::TunnelState::NOT_TUNNEL);
    uint64_t layerId = 0;
    uint32_t property = TUNNEL_PROP_INVALID;
    runtime.GetLayerInfo(layerId, property);
    EXPECT_EQ(layerId, TEST_NODE_TUNNEL_LAYER_ID);

    RSTunnelLayerHelper::BeginTunnelBuilding(*context.node, 0, TUNNEL_PROP_INVALID);
    EXPECT_EQ(runtime.GetTunnelState(), RSTunnelRuntimeState::TunnelState::NOT_TUNNEL);
    runtime.GetLayerInfo(layerId, property);
    EXPECT_EQ(layerId, 0u);
    EXPECT_EQ(property, TUNNEL_PROP_INVALID);

    runtime.SetLayerInfo(TEST_NODE_TUNNEL_LAYER_ID, TUNNEL_PROP_BUFFER_ADDR);

    RSTunnelRouteArbiter::consecutiveSolePresentCount_.store(
        TUNNEL_NOT_TUNNEL_TO_BUILDING_THRESHOLD, std::memory_order_release);
    RSTunnelLayerHelper::BeginTunnelBuilding(
        *context.node, TEST_NODE_TUNNEL_LAYER_ID, TUNNEL_PROP_BUFFER_ADDR);
    EXPECT_EQ(runtime.GetTunnelState(), RSTunnelRuntimeState::TunnelState::BUILDING);

    RSTunnelRouteArbiter::OnDirectCompositionStable(TUNNEL_STABLE_THRESHOLD + 1);
    RSTunnelRouteArbiter::ResetConsecutiveSolePresentCount();
}

/**
 * @tc.name: CanCommitBufferDirect_RejectsWhenHandlerNull
 * @tc.desc: Cover the handler_null reject path in CanCommitBufferDirect when the surface handler is null.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerHelperTest, CanCommitBufferDirect_RejectsWhenHandlerNull, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    auto context = CreateTunnelTestContext(false);
    ASSERT_TRUE(context.IsBaseReady());
    auto& tunnelRuntime = *context.node->GetTunnelRuntimeState();
    tunnelRuntime.SetBuilding();
    ASSERT_TRUE(tunnelRuntime.SetActiveFromTunnelLayerAvailable(tunnelRuntime.GetTunnelLayerGeneration()));

    auto savedHandler = context.node->surfaceHandler_;
    context.node->surfaceHandler_ = nullptr;
    auto connection = sptr<RecordingRenderToComposerConnection>::MakeSptr();
    auto composerClientManager = CreateRecordingComposerManager(context.node->GetId(), connection);
    ASSERT_NE(composerClientManager, nullptr);
    EXPECT_FALSE(RSTunnelLayerHelper::TryCommitBufferDirect(
        context.node, composerClientManager, false, tunnelRuntime));
    EXPECT_FALSE(connection->commitTunnelCalled);
    context.node->surfaceHandler_ = savedHandler;
}

/**
 * @tc.name: TryCommitPendingBuffer_CommitsWhenScreenPowerNotOff
 * @tc.desc: Cover the true branch of `if (!IsScreenPowerOff(powerStatus))` in TryCommitPendingBuffer.
 *           When the cached screen power status is a non-off state (POWER_STATUS_ON), the helper must
 *           proceed to CommitBuffer so the recording connection observes the commit call.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerHelperTest, TryCommitPendingBuffer_CommitsWhenScreenPowerNotOff, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    ScopedScreenPowerStatus scopedPower(0, ScreenPowerStatus::POWER_STATUS_ON);
    RSTunnelRouteArbiter::SetTunnelSolePresentLayer(true);
    auto context = CreateTunnelTestContext(true);
    ASSERT_TRUE(context.IsProducerReady());

    context.producer->SetQueueSize(TEST_QUEUE_SIZE);
    TunnelLayerState state;
    ASSERT_TRUE(SetTunnelInfoForConsumer(context.consumer, state));
    auto& tunnelRuntime = *context.node->GetTunnelRuntimeState();
    tunnelRuntime.SetLayerInfo(state.tunnelLayerId, state.property);
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
    RSTunnelRouteArbiter::consecutiveSolePresentCount_.store(TUNNEL_STABLE_THRESHOLD, std::memory_order_release);
    RSTunnelRouteArbiter::RefreshGlobalTriggerSnapshot();

    EXPECT_TRUE(RSTunnelLayerHelper::TryCommitBufferDirect(context.node, composerClientManager, false, tunnelRuntime));
    EXPECT_TRUE(connection->commitTunnelCalled);
}

/**
 * @tc.name: TryCommitPendingBuffer_SkipsCommitWhenScreenPowerOff
 * @tc.desc: Cover the false branch of `if (!IsScreenPowerOff(powerStatus))` in TryCommitPendingBuffer.
 *           When the cached screen power status is an off state (OFF/SUSPEND/OFF_ADVANCED), the helper
 *           must skip CommitBuffer (no tunnel IPC) while still completing the success path so the buffer
 *           is consumed normally.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerHelperTest, TryCommitPendingBuffer_SkipsCommitWhenScreenPowerOff, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    const std::vector<ScreenPowerStatus> offStates = {
        ScreenPowerStatus::POWER_STATUS_OFF,
        ScreenPowerStatus::POWER_STATUS_SUSPEND,
        ScreenPowerStatus::POWER_STATUS_OFF_ADVANCED,
    };

    for (const auto& powerStatus : offStates) {
        SCOPED_TRACE(testing::Message() << "powerStatus=" << static_cast<uint32_t>(powerStatus));
        ScopedScreenPowerStatus scopedPower(0, powerStatus);
        RSTunnelRouteArbiter::SetTunnelSolePresentLayer(true);
        auto context = CreateTunnelTestContext(true);
        ASSERT_TRUE(context.IsProducerReady());

        context.producer->SetQueueSize(TEST_QUEUE_SIZE);
        TunnelLayerState state;
        ASSERT_TRUE(SetTunnelInfoForConsumer(context.consumer, state));
        auto& tunnelRuntime = *context.node->GetTunnelRuntimeState();
        tunnelRuntime.SetLayerInfo(state.tunnelLayerId, state.property);
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
        RSTunnelRouteArbiter::consecutiveSolePresentCount_.store(TUNNEL_STABLE_THRESHOLD, std::memory_order_release);
        RSTunnelRouteArbiter::RefreshGlobalTriggerSnapshot();

        EXPECT_TRUE(
            RSTunnelLayerHelper::TryCommitBufferDirect(context.node, composerClientManager, false, tunnelRuntime));
        EXPECT_FALSE(connection->commitTunnelCalled);
    }
}

/**
 * @tc.name: NotifyScreenPowerStatusChanged_NullProperty_KeepsPreviousStatus
 * @tc.desc: Cover the true branch of `if (!prop)` in NotifyScreenPowerStatusChanged. When a null
 *           property is passed, the helper must early-return without touching the cached map, so the
 *           previously cached off status stays in effect and the subsequent tunnel commit is skipped.
 *           The false branch (non-null property) is covered by the two TryCommitPendingBuffer cases
 *           above, which prove a valid property updates the cache and drives IsScreenPowerOff.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerHelperTest, NotifyScreenPowerStatusChanged_NullProperty_KeepsPreviousStatus, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    ScopedScreenPowerStatus scopedPower(0, ScreenPowerStatus::POWER_STATUS_OFF);
    sptr<ScreenPropertyBase> nullProperty = nullptr;
    RSTunnelLayerHelper::NotifyScreenPowerStatusChanged(0, nullProperty);

    RSTunnelRouteArbiter::SetTunnelSolePresentLayer(true);
    auto context = CreateTunnelTestContext(true);
    ASSERT_TRUE(context.IsProducerReady());

    context.producer->SetQueueSize(TEST_QUEUE_SIZE);
    TunnelLayerState state;
    ASSERT_TRUE(SetTunnelInfoForConsumer(context.consumer, state));
    auto& tunnelRuntime = *context.node->GetTunnelRuntimeState();
    tunnelRuntime.SetLayerInfo(state.tunnelLayerId, state.property);
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
    RSTunnelRouteArbiter::consecutiveSolePresentCount_.store(TUNNEL_STABLE_THRESHOLD, std::memory_order_release);
    RSTunnelRouteArbiter::RefreshGlobalTriggerSnapshot();

    EXPECT_TRUE(RSTunnelLayerHelper::TryCommitBufferDirect(context.node, composerClientManager, false, tunnelRuntime));
    EXPECT_FALSE(connection->commitTunnelCalled);
}

} // namespace OHOS::Rosen
