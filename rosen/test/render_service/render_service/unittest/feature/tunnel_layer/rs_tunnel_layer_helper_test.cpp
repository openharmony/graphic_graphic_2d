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
#include "feature/tunnel_layer/rs_tunnel_layer_helper.h"
#include "feature/tunnel_layer/rs_tunnel_layer_manager.h"
#include "feature/tunnel_layer/rs_tunnel_route_arbiter.h"
#include "gtest/gtest.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/render_thread/rs_base_surface_util.h"
#include "pipeline/rs_test_util.h"
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

bool SetRuntimePendingBufferFromConsumer(const TunnelTestContext& context)
{
    if (!context.IsBaseReady()) {
        return false;
    }
    IConsumerSurface::AcquireBufferReturnValue returnValue;
    if (context.consumer->AcquireBuffer(returnValue, CONSUME_DIRECTLY, false) != SURFACE_ERROR_OK ||
        returnValue.buffer == nullptr) {
        return false;
    }
    RSSurfaceHandler::SurfaceBufferEntry entry;
    entry.buffer = returnValue.buffer;
    entry.acquireFence = returnValue.fence;
    entry.timestamp = returnValue.timestamp;
    entry.bufferOwnerCount_->bufferId_ = returnValue.buffer->GetBufferId();
    entry.RegisterReleaseBufferListener([](uint64_t) {});
    context.node->GetTunnelRuntimeState().SetPendingBuffer(entry);
    return true;
}

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
};

/**
 * @tc.name: ResolveTunnelLayerInfoAndProperty001
 * @tc.desc: Test ResolveTunnelLayerInfo matrix with shared setup and property branches.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerHelperTest, ResolveTunnelLayerInfoAndProperty001, TestSize.Level1)
{
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
            context.node->SetTunnelLayerInfo(testCase.nodeTunnelLayerId, testCase.nodeProperty);
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
    context.node->SetTunnelLayerInfo(TEST_SURFACE_NODE_TUNNEL_LAYER_ID, property);

    uint64_t actualTunnelLayerId = 0;
    uint32_t actualProperty = TUNNEL_PROP_INVALID;
    context.node->GetTunnelLayerInfo(actualTunnelLayerId, actualProperty);
    EXPECT_EQ(actualTunnelLayerId, TEST_SURFACE_NODE_TUNNEL_LAYER_ID);
    EXPECT_EQ(actualProperty, property);

    uint64_t runtimeTunnelLayerId = 0;
    uint32_t runtimeProperty = TUNNEL_PROP_INVALID;
    context.node->GetTunnelRuntimeState().GetLayerInfo(runtimeTunnelLayerId, runtimeProperty);
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
    ASSERT_TRUE(FlushOneBuffer(context.producer));

    constexpr uint32_t property = TUNNEL_PROP_BUFFER_ADDR | TUNNEL_PROP_DEVICE_COMMIT;
    context.node->SetTunnelLayerInfo(TEST_SURFACE_NODE_TUNNEL_LAYER_ID, property);
    auto& tunnelRuntimeState = context.node->GetTunnelRuntimeState();
    tunnelRuntimeState.SetBuilding();
    ASSERT_TRUE(tunnelRuntimeState.SetActiveFromTunnelLayerAvailable(tunnelRuntimeState.GetTunnelLayerGeneration()));
    ASSERT_TRUE(SetRuntimePendingBufferFromConsumer(context));
    ASSERT_TRUE(tunnelRuntimeState.HasPendingBuffer());

    auto nextConsumer = IConsumerSurface::Create("TunnelConsumer2");
    ASSERT_NE(nextConsumer, nullptr);
    context.node->SetConsumer(nextConsumer);
    EXPECT_FALSE(tunnelRuntimeState.HasPendingBuffer());
    EXPECT_EQ(tunnelRuntimeState.GetTunnelState(), RSTunnelRuntimeState::TunnelState::BUILDING);
    uint64_t actualTunnelLayerId = TEST_SURFACE_NODE_TUNNEL_LAYER_ID;
    uint32_t actualProperty = property;
    context.node->GetTunnelLayerInfo(actualTunnelLayerId, actualProperty);
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
        auto& tunnelRuntime = context.node->GetTunnelRuntimeState();
        tunnelRuntime.SetBuilding();
        if (testCase.activationState == RSTunnelRuntimeState::TunnelState::ACTIVE) {
            ASSERT_TRUE(tunnelRuntime.SetActiveFromTunnelLayerAvailable(tunnelRuntime.GetTunnelLayerGeneration()));
        }
        context.node->SetIsOnTheTree(testCase.onTree);
        context.node->SetHardwareForcedDisabledState(testCase.hardwareDisabled);
        ASSERT_TRUE(FlushOneBuffer(context.producer));
        if (testCase.keepAvailableBufferCount) {
            context.surfaceHandler->SetAvailableBufferCount(1);
        }
        if (testCase.setupPendingBuffer) {
            ASSERT_TRUE(SetRuntimePendingBufferFromConsumer(context));
            ASSERT_TRUE(context.node->GetTunnelRuntimeState().HasPendingBuffer());
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
        context.node->GetTunnelRuntimeState().Clear();
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
    auto& tunnelRuntime = context.node->GetTunnelRuntimeState();
    tunnelRuntime.SetBuilding();
    ASSERT_TRUE(tunnelRuntime.SetActiveFromTunnelLayerAvailable(tunnelRuntime.GetTunnelLayerGeneration()));
    ASSERT_TRUE(FlushOneBuffer(context.producer));
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
    context.node->GetTunnelRuntimeState().SetBuilding();
    context.node->GetTunnelRuntimeState().SetActiveFromTunnelLayerAvailable(
        context.node->GetTunnelRuntimeState().GetTunnelLayerGeneration());
    ScopedRegisteredSurfaceNode registeredNode(context.node);
    ASSERT_TRUE(registeredNode.IsRegistered());
    ASSERT_TRUE(FlushOneBuffer(context.producer));
    context.surfaceHandler->SetAvailableBufferCount(1);

    auto connection = sptr<RecordingRenderToComposerConnection>::MakeSptr();
    connection->commitTunnelResult = GRAPHIC_DISPLAY_FAILURE;
    auto composerClientManager = CreateRecordingComposerManager(context.node->GetId(), connection);
    ASSERT_NE(composerClientManager, nullptr);
    EXPECT_FALSE(RSTunnelLayerHelper::TryCommitBufferDirect(context.node, composerClientManager, true));
    EXPECT_TRUE(connection->commitTunnelCalled);
    EXPECT_TRUE(context.node->GetTunnelRuntimeState().HasPendingBuffer());
    uint64_t actualTunnelLayerId = 0;
    uint32_t actualProperty = TUNNEL_PROP_INVALID;
    context.node->GetTunnelLayerInfo(actualTunnelLayerId, actualProperty);
    EXPECT_EQ(actualTunnelLayerId, 0u);
    EXPECT_EQ(context.node->GetTunnelRuntimeState().GetTunnelState(),
        RSTunnelRuntimeState::TunnelState::BUILDING);
    EXPECT_EQ(context.surfaceHandler->GetAvailableBufferCount(), 1);

    RSSurfaceHandler::SurfaceBufferEntry pendingBuffer;
    ASSERT_TRUE(context.node->GetTunnelRuntimeState().TakePendingBuffer(pendingBuffer));
    ASSERT_NE(pendingBuffer.bufferOwnerCount_, nullptr);
    EXPECT_EQ(pendingBuffer.bufferOwnerCount_->refCount_.load(), 1);
    context.node->GetTunnelRuntimeState().SetPendingBuffer(pendingBuffer);

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
    context.node->SetTunnelLayerInfo(state.tunnelLayerId, state.property);
    auto& tunnelRuntime = context.node->GetTunnelRuntimeState();
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
    bool oldPreBufferReleased = false;
    oldPreBufferOwnerCount->bufferReleaseCb_ = [&oldPreBufferReleased](uint64_t) {
        oldPreBufferReleased = true;
    };
    currentBufferOwnerCount->bufferReleaseCb_ = [](uint64_t) {};
    context.surfaceHandler->SetBuffer(oldPreBuffer, SyncFence::InvalidFence(), Rect(), 0, oldPreBufferOwnerCount);
    context.surfaceHandler->SetBuffer(currentBuffer, SyncFence::InvalidFence(), Rect(), 0, currentBufferOwnerCount);
    ASSERT_EQ(oldPreBufferOwnerCount->bufferId_, oldPreBuffer->GetBufferId());
    ASSERT_EQ(currentBufferOwnerCount->bufferId_, currentBuffer->GetBufferId());
    ASSERT_EQ(context.surfaceHandler->GetPreBuffer(), oldPreBuffer);
    ASSERT_TRUE(FlushOneBuffer(context.producer));
    context.surfaceHandler->SetAvailableBufferCount(1);
    ASSERT_TRUE(RSTunnelLayerHelper::TryCommitBufferDirect(context.node, composerClientManager, true));
    EXPECT_TRUE(oldPreBufferReleased);
    EXPECT_EQ(oldPreBufferOwnerCount->refCount_.load(), 0);
    auto tunnelBufferOwnerCount = context.surfaceHandler->GetBufferOwnerCount();
    ASSERT_NE(tunnelBufferOwnerCount, nullptr);

    context.node->SetTunnelLayerInfo(0, TUNNEL_PROP_INVALID);
    tunnelRuntime.SetLayerInfo(0, TUNNEL_PROP_INVALID);
    tunnelRuntime.SetBuilding();

    ASSERT_TRUE(FlushOneBuffer(context.producer));
    context.surfaceHandler->SetAvailableBufferCount(1);
    ASSERT_TRUE(RSBaseSurfaceUtil::ConsumeAndUpdateBuffer(*context.surfaceHandler));
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
 * @tc.name: TransferTunnelPendingBufferToNormalConsume001
 * @tc.desc: Test tunnel to RS normal transition keeps callback available count without pending buffer.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerHelperTest, TransferTunnelPendingBufferToNormalConsume001, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    auto context = CreateTunnelTestContext(true);
    ASSERT_TRUE(context.IsProducerReady());

    ASSERT_TRUE(FlushOneBuffer(context.producer));
    context.surfaceHandler->SetAvailableBufferCount(1);
    RSTunnelLayerManager tunnelLayerManager(nullptr);
    tunnelLayerManager.TransferTunnelPendingBufferToNormalConsume(context.node);
    EXPECT_EQ(context.surfaceHandler->GetAvailableBufferCount(), 1);
    EXPECT_EQ(context.surfaceHandler->GetPreBuffer(), nullptr);
    ASSERT_TRUE(RSBaseSurfaceUtil::ConsumeAndUpdateBuffer(*context.surfaceHandler));
    EXPECT_TRUE(context.surfaceHandler->IsCurrentFrameBufferConsumed());
}

/**
 * @tc.name: TransferTunnelPendingBufferToNormalConsume002
 * @tc.desc: Test pending tunnel buffer is moved into normal hold buffer for RS consume.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelLayerHelperTest, TransferTunnelPendingBufferToNormalConsume002, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    auto context = CreateTunnelTestContext(true);
    ASSERT_TRUE(context.IsProducerReady());

    ASSERT_TRUE(FlushOneBuffer(context.producer));
    ASSERT_TRUE(SetRuntimePendingBufferFromConsumer(context));
    ASSERT_TRUE(context.node->GetTunnelRuntimeState().HasPendingBuffer());

    RSTunnelLayerManager tunnelLayerManager(nullptr);
    tunnelLayerManager.TransferTunnelPendingBufferToNormalConsume(context.node);
    EXPECT_FALSE(context.node->GetTunnelRuntimeState().HasPendingBuffer());
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
    ASSERT_TRUE(FlushOneBuffer(context.producer));
    IConsumerSurface::AcquireBufferReturnValue existingReturnValue;
    ASSERT_EQ(context.consumer->AcquireBuffer(existingReturnValue, CONSUME_DIRECTLY, false), SURFACE_ERROR_OK);
    ASSERT_NE(existingReturnValue.buffer, nullptr);
    context.surfaceHandler->SetHoldReturnValue(existingReturnValue);

    context.node->GetTunnelRuntimeState().SetBuilding();
    context.node->GetTunnelRuntimeState().SetActiveFromTunnelLayerAvailable(
        context.node->GetTunnelRuntimeState().GetTunnelLayerGeneration());
    ScopedRegisteredSurfaceNode registeredNode(context.node);
    ASSERT_TRUE(registeredNode.IsRegistered());
    ASSERT_TRUE(FlushOneBuffer(context.producer));
    context.surfaceHandler->SetAvailableBufferCount(1);

    auto composerClientManager = std::make_shared<RSComposerClientManager>();
    EXPECT_FALSE(RSTunnelLayerHelper::TryCommitBufferDirect(context.node, composerClientManager, true));
    ASSERT_NE(context.surfaceHandler->GetHoldReturnValue(), nullptr);
    ASSERT_NE(context.surfaceHandler->GetHoldReturnValue()->buffer, nullptr);
    EXPECT_EQ(context.surfaceHandler->GetHoldReturnValue()->buffer->GetBufferId(),
        existingReturnValue.buffer->GetBufferId());
    EXPECT_TRUE(context.node->GetTunnelRuntimeState().HasPendingBuffer());

    EXPECT_TRUE(RSBaseSurfaceUtil::ConsumeAndUpdateBuffer(*context.surfaceHandler));
    ASSERT_NE(context.surfaceHandler->GetBuffer(), nullptr);
    EXPECT_EQ(context.surfaceHandler->GetBuffer()->GetBufferId(), existingReturnValue.buffer->GetBufferId());
    EXPECT_EQ(context.surfaceHandler->GetHoldReturnValue(), nullptr);
}
} // namespace OHOS::Rosen
