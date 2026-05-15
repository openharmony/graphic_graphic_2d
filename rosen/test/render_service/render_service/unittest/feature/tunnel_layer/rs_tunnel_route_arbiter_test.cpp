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

#include <vector>

#include "feature/tunnel_layer/rs_tunnel_route_arbiter.h"
#include "gtest/gtest.h"
#include "params/rs_surface_render_params.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_test_util.h"
#include "rs_tunnel_test_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
using TunnelTest::CreateTunnelTestContext;
using TunnelTest::ScopedNewTunnelSwitch;
namespace {
constexpr uint64_t TEST_TUNNEL_LAYER_ID = 1001;
constexpr uint32_t TEST_TUNNEL_LAYER_PROPERTY = TUNNEL_PROP_BUFFER_ADDR;
constexpr ScreenId CLAIMED_SCREEN_ID = 7;
constexpr ScreenId MIGRATED_SCREEN_ID = 9;
constexpr ScreenId UNRELATED_SCREEN_ID = 5;
constexpr uint32_t TEST_ROUTE_BUFFER_SIZE = 0x100;
constexpr uint32_t TEST_ROUTE_BUFFER_SIZE_CHANGED = 0x200;
constexpr uint32_t TEST_ROUTE_STRIDE_ALIGNMENT = 0x8;
constexpr int32_t TEST_ROUTE_OFFSET = 10;
constexpr int32_t TEST_ROUTE_ZORDER = 10;
constexpr int32_t TEST_ROUTE_ZORDER_CHANGED = 11;
constexpr float TEST_ROUTE_ALPHA_CHANGED = 0.5f;

void ActivateTunnelRuntime(RSTunnelRuntimeState& tunnelRuntime)
{
    tunnelRuntime.SetBuilding();
    ASSERT_TRUE(tunnelRuntime.SetActiveFromTunnelLayerAvailable(tunnelRuntime.GetTunnelLayerGeneration()));
}

void ClearUiCaptureTasks(RSMainThread& mainThread)
{
    mainThread.pendingUiCaptureTasks_.clear();
    while (!mainThread.uiCaptureTasks_.empty()) {
        mainThread.uiCaptureTasks_.pop();
    }
}

sptr<SurfaceBuffer> CreateRouteBuffer(uint32_t size = TEST_ROUTE_BUFFER_SIZE,
    GraphicPixelFormat format = GRAPHIC_PIXEL_FMT_RGBA_8888)
{
    sptr<SurfaceBuffer> buffer = SurfaceBuffer::Create();
    if (buffer == nullptr) {
        return nullptr;
    }
    BufferRequestConfig config = {
        .width = static_cast<int32_t>(size),
        .height = static_cast<int32_t>(size),
        .strideAlignment = TEST_ROUTE_STRIDE_ALIGNMENT,
        .format = format,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };
    return buffer->Alloc(config) == GSERROR_OK ? buffer : nullptr;
}

RSLayerInfo CreateBaseRouteLayerInfo()
{
    RSLayerInfo layerInfo;
    layerInfo.srcRect = { 0, 0, TEST_ROUTE_BUFFER_SIZE, TEST_ROUTE_BUFFER_SIZE };
    layerInfo.dstRect = { 0, 0, TEST_ROUTE_BUFFER_SIZE, TEST_ROUTE_BUFFER_SIZE };
    layerInfo.zOrder = TEST_ROUTE_ZORDER;
    layerInfo.alpha = 1.0f;
    layerInfo.blendType = GRAPHIC_BLEND_NONE;
    layerInfo.transformType = GRAPHIC_ROTATE_NONE;
    return layerInfo;
}

void PrepareRouteBaseline(const TunnelTest::TunnelTestContext& context)
{
    auto* params = static_cast<RSSurfaceRenderParams*>(context.node->GetStagingRenderParams().get());
    ASSERT_NE(params, nullptr);
    auto buffer = CreateRouteBuffer();
    ASSERT_NE(buffer, nullptr);
    auto layerInfo = CreateBaseRouteLayerInfo();
    params->SetLayerInfo(layerInfo);
    params->SetBuffer(buffer, nullptr, Rect());
    context.node->SetTunnelLayerInfo(TEST_TUNNEL_LAYER_ID, TEST_TUNNEL_LAYER_PROPERTY);

    auto& tunnelRuntime = context.node->GetTunnelRuntimeState();
    tunnelRuntime.SetLayerInfo(TEST_TUNNEL_LAYER_ID, TEST_TUNNEL_LAYER_PROPERTY);
    RSTunnelRuntimeState::LastFrameRouteSnapshot snapshot;
    snapshot.dstRect = layerInfo.dstRect;
    snapshot.srcRect = layerInfo.srcRect;
    snapshot.transformType = layerInfo.transformType;
    snapshot.zOrder = layerInfo.zOrder;
    snapshot.alpha = layerInfo.alpha;
    snapshot.blendType = layerInfo.blendType;
    snapshot.hwcEnabled = !context.node->IsHardwareForcedDisabled();
    snapshot.bufferWidth = buffer->GetWidth();
    snapshot.bufferHeight = buffer->GetHeight();
    snapshot.bufferFormat = buffer->GetFormat();
    snapshot.tunnelLayerId = TEST_TUNNEL_LAYER_ID;
    snapshot.tunnelProperty = TEST_TUNNEL_LAYER_PROPERTY;
    tunnelRuntime.UpdateLastFrameRouteSnapshot(snapshot);
    ActivateTunnelRuntime(tunnelRuntime);
}

enum class RouteMutationType {
    NONE,
    PENDING_BUFFER,
    CAPTURE,
    PROTECTED,
    TUNNEL_INFO,
    DST_RECT,
    TRANSFORM,
    ZORDER,
    ALPHA,
    BLEND,
    BUFFER_SIZE,
    BUFFER_FORMAT,
};

struct RouteMutationCase {
    const char* name;
    bool newTunnelEnabled;
    RouteMutationType type;
};

enum class ApplyMutationResult {
    APPLIED,
    NOT_MATCHED,
    FAILED,
};

ApplyMutationResult ApplyNonLayerRouteMutation(RouteMutationType type, const TunnelTest::TunnelTestContext& context,
    RSSurfaceRenderParams& params)
{
    switch (type) {
        case RouteMutationType::NONE:
            return ApplyMutationResult::APPLIED;
        case RouteMutationType::PENDING_BUFFER: {
            RSSurfaceHandler::SurfaceBufferEntry pendingBuffer;
            pendingBuffer.buffer = CreateRouteBuffer();
            if (pendingBuffer.buffer == nullptr) {
                return ApplyMutationResult::FAILED;
            }
            pendingBuffer.bufferOwnerCount_->bufferId_ = pendingBuffer.buffer->GetBufferId();
            context.node->GetTunnelRuntimeState().SetPendingBuffer(pendingBuffer);
            return ApplyMutationResult::APPLIED;
        }
        case RouteMutationType::CAPTURE:
            params.SetIsNodeToBeCaptured(true);
            return ApplyMutationResult::APPLIED;
        case RouteMutationType::PROTECTED:
            context.node->GetMultableSpecialLayerMgr().Set(SpecialLayerType::PROTECTED, true);
            return ApplyMutationResult::APPLIED;
        case RouteMutationType::TUNNEL_INFO:
            context.node->GetTunnelRuntimeState().SetLayerInfo(TEST_TUNNEL_LAYER_ID + 1,
                TEST_TUNNEL_LAYER_PROPERTY);
            return ApplyMutationResult::APPLIED;
        case RouteMutationType::BUFFER_SIZE: {
            auto buffer = CreateRouteBuffer(TEST_ROUTE_BUFFER_SIZE_CHANGED);
            if (buffer == nullptr) {
                return ApplyMutationResult::FAILED;
            }
            params.SetBuffer(buffer, nullptr, Rect());
            return ApplyMutationResult::APPLIED;
        }
        case RouteMutationType::BUFFER_FORMAT: {
            auto buffer = CreateRouteBuffer(TEST_ROUTE_BUFFER_SIZE, GRAPHIC_PIXEL_FMT_RGB_565);
            if (buffer == nullptr) {
                return ApplyMutationResult::FAILED;
            }
            params.SetBuffer(buffer, nullptr, Rect());
            return ApplyMutationResult::APPLIED;
        }
        default:
            return ApplyMutationResult::NOT_MATCHED;
    }
}

bool ApplyLayerRouteMutation(RouteMutationType type, RSSurfaceRenderParams& params)
{
    auto layerInfo = params.GetLayerInfo();
    switch (type) {
        case RouteMutationType::DST_RECT:
            layerInfo.dstRect.x = TEST_ROUTE_OFFSET;
            break;
        case RouteMutationType::TRANSFORM:
            layerInfo.transformType = GRAPHIC_ROTATE_90;
            break;
        case RouteMutationType::ZORDER:
            layerInfo.zOrder = TEST_ROUTE_ZORDER_CHANGED;
            break;
        case RouteMutationType::ALPHA:
            layerInfo.alpha = TEST_ROUTE_ALPHA_CHANGED;
            break;
        case RouteMutationType::BLEND:
            layerInfo.blendType = GRAPHIC_BLEND_SRCOVER;
            break;
        default:
            return false;
    }
    params.SetLayerInfo(layerInfo);
    return true;
}

bool ApplyRouteMutation(RouteMutationType type, const TunnelTest::TunnelTestContext& context,
    RSSurfaceRenderParams& params)
{
    auto result = ApplyNonLayerRouteMutation(type, context, params);
    if (result == ApplyMutationResult::FAILED) {
        return false;
    }
    if (result == ApplyMutationResult::APPLIED) {
        return true;
    }
    return ApplyLayerRouteMutation(type, params);
}

void ExpectRouteClaimGoesNormal(RSTunnelRouteArbiter& arbiter,
    const TunnelTest::TunnelTestContext& context)
{
    EXPECT_EQ(arbiter.ArbitrateAndClaim(context.node), RSTunnelRouteArbiter::MainThreadOutcome::GO_NORMAL);
    EXPECT_EQ(context.node->GetTunnelRuntimeState().GetPhase(), RSTunnelRuntimeState::Phase::NORMAL_PREPARING);
}

void ExpectStaleQueueGoesNormal(RSTunnelRouteArbiter& arbiter)
{
    auto context = CreateTunnelTestContext(false);
    ASSERT_TRUE(context.IsBaseReady());
    auto& tunnelRuntime = context.node->GetTunnelRuntimeState();
    ActivateTunnelRuntime(tunnelRuntime);
    ASSERT_FALSE(tunnelRuntime.HasPendingBuffer());
    ASSERT_EQ(tunnelRuntime.GetPhase(), RSTunnelRuntimeState::Phase::TUNNEL_IDLE);
    context.surfaceHandler->SetAvailableBufferCount(1);
    ExpectRouteClaimGoesNormal(arbiter, context);
}

void ExpectHardwareDisabledGoesNormal(RSTunnelRouteArbiter& arbiter)
{
    auto context = CreateTunnelTestContext(false);
    ASSERT_TRUE(context.IsBaseReady());
    ActivateTunnelRuntime(context.node->GetTunnelRuntimeState());
    context.node->SetHardwareForcedDisabledState(true);
    ExpectRouteClaimGoesNormal(arbiter, context);
}

void ExpectMissingBufferKeepsEmptySnapshot(RSTunnelRouteArbiter& arbiter)
{
    auto context = CreateTunnelTestContext(false);
    ASSERT_TRUE(context.IsBaseReady());
    context.node->SetTunnelLayerInfo(TEST_TUNNEL_LAYER_ID, TEST_TUNNEL_LAYER_PROPERTY);
    auto& tunnelRuntime = context.node->GetTunnelRuntimeState();
    tunnelRuntime.SetLayerInfo(TEST_TUNNEL_LAYER_ID, TEST_TUNNEL_LAYER_PROPERTY);
    ActivateTunnelRuntime(tunnelRuntime);

    auto* params = static_cast<RSSurfaceRenderParams*>(context.node->GetStagingRenderParams().get());
    ASSERT_NE(params, nullptr);
    ASSERT_EQ(params->GetBuffer(), nullptr);
    ExpectRouteClaimGoesNormal(arbiter, context);

    const auto& snapshot = tunnelRuntime.GetLastFrameRouteSnapshot();
    EXPECT_EQ(snapshot.tunnelLayerId, 0u);
    EXPECT_EQ(snapshot.tunnelProperty, static_cast<uint32_t>(TUNNEL_PROP_INVALID));
    EXPECT_EQ(snapshot.bufferWidth, 0);
    EXPECT_EQ(snapshot.bufferHeight, 0);
    EXPECT_EQ(snapshot.bufferFormat, 0);
}

void RunRouteGoNormalMatrix()
{
    const std::vector<RouteMutationCase> testCases = {
        { "new_tunnel_disabled", false, RouteMutationType::NONE },
        { "pending_buffer", true, RouteMutationType::PENDING_BUFFER },
        { "capture", true, RouteMutationType::CAPTURE },
        { "protected", true, RouteMutationType::PROTECTED },
        { "tunnel_info", true, RouteMutationType::TUNNEL_INFO },
        { "dst_rect", true, RouteMutationType::DST_RECT },
        { "transform", true, RouteMutationType::TRANSFORM },
        { "zorder", true, RouteMutationType::ZORDER },
        { "alpha", true, RouteMutationType::ALPHA },
        { "blend", true, RouteMutationType::BLEND },
        { "buffer_size", true, RouteMutationType::BUFFER_SIZE },
        { "buffer_format", true, RouteMutationType::BUFFER_FORMAT },
    };

    for (const auto& testCase : testCases) {
        SCOPED_TRACE(testing::Message() << "case=" << testCase.name);
        ScopedNewTunnelSwitch scopedSwitch(testCase.newTunnelEnabled);
        auto context = CreateTunnelTestContext(false);
        ASSERT_TRUE(context.IsBaseReady());
        PrepareRouteBaseline(context);
        auto* params = static_cast<RSSurfaceRenderParams*>(context.node->GetStagingRenderParams().get());
        ASSERT_NE(params, nullptr);
        ASSERT_TRUE(ApplyRouteMutation(testCase.type, context, *params));

        RSTunnelRouteArbiter arbiter;
        ExpectRouteClaimGoesNormal(arbiter, context);
    }
}

} // namespace

class RSTunnelRouteArbiterTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        RSTestUtil::InitRenderNodeGC();
    }
};

/**
 * @tc.name: ArbitrateAndClaim_GoNormalSurfaceCases
 * @tc.desc: Surface-level fallback causes must route GO_NORMAL so RS normal consume can own the frame.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelRouteArbiterTest,
    RSTunnelRouteArbiter_ArbitrateAndClaim_GoNormalSurfaceCases,
    TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    RSTunnelRouteArbiter arbiter;
    ExpectStaleQueueGoesNormal(arbiter);
    ExpectHardwareDisabledGoesNormal(arbiter);
    ExpectMissingBufferKeepsEmptySnapshot(arbiter);
    RunRouteGoNormalMatrix();
}

/**
 * @tc.name: OnRenderCommitDone_AdvancesByClaimedScreenId_NotCurrentScreenId
 * @tc.desc: A surface that migrates to a new screen between claim and commit-done must still be
 *           advanced by the original screen's commit-done so NORMAL_PREPARING cannot stall.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelRouteArbiterTest,
    RSTunnelRouteArbiter_OnRenderCommitDone_AdvancesByClaimedScreenId_NotCurrentScreenId, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    auto context = CreateTunnelTestContext(false);
    ASSERT_TRUE(context.IsBaseReady());

    auto& tunnelRuntime = context.node->GetTunnelRuntimeState();
    ActivateTunnelRuntime(tunnelRuntime);
    context.surfaceHandler->SetAvailableBufferCount(1);
    context.node->screenId_ = CLAIMED_SCREEN_ID;

    RSTunnelRouteArbiter arbiter;
    auto outcome = arbiter.ArbitrateAndClaim(context.node);
    ASSERT_EQ(outcome, RSTunnelRouteArbiter::MainThreadOutcome::GO_NORMAL);
    ASSERT_EQ(tunnelRuntime.GetPhase(), RSTunnelRuntimeState::Phase::NORMAL_PREPARING);

    context.node->screenId_ = MIGRATED_SCREEN_ID;

    arbiter.OnRenderCommitDone(UNRELATED_SCREEN_ID);
    EXPECT_EQ(tunnelRuntime.GetPhase(), RSTunnelRuntimeState::Phase::NORMAL_PREPARING);

    arbiter.OnRenderCommitDone(CLAIMED_SCREEN_ID);
    EXPECT_EQ(tunnelRuntime.GetPhase(), RSTunnelRuntimeState::Phase::NORMAL_COMMITTED);
}

/**
 * @tc.name: ComputeGlobalForbiddenCause_NullMainThread_ReturnsNullptr
 * @tc.desc: Defensive contract: a null main-thread pointer yields nullptr so callers can
 *           short-circuit without crashing.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelRouteArbiterTest,
    RSTunnelRouteArbiter_ComputeGlobalForbiddenCause_NullMainThread_ReturnsNullptr, TestSize.Level1)
{
    EXPECT_EQ(RSTunnelRouteArbiter::ComputeGlobalForbiddenCause(nullptr), nullptr);
}

/**
 * @tc.name: ComputeGlobalForbiddenCause_ReturnsScreenshotActive_WhenUiCapturePending
 * @tc.desc: When a ui-capture task is pending on the main thread, IsSnapshotPendingThisFrame
 *           becomes true; the unified probe must report cause SCREENSHOT_ACTIVE.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelRouteArbiterTest,
    RSTunnelRouteArbiter_ComputeGlobalForbiddenCause_ReturnsScreenshotActive_WhenUiCapturePending,
    TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);

    ClearUiCaptureTasks(*mainThread);
    ASSERT_FALSE(mainThread->IsSnapshotPendingThisFrame());

    auto node = RSTestUtil::CreateSurfaceNode();
    ASSERT_NE(node, nullptr);
    mainThread->context_->GetMutableNodeMap().RegisterRenderNode(node);
    mainThread->AddUiCaptureTask(node->GetId(), []() {});
    ASSERT_TRUE(mainThread->IsSnapshotPendingThisFrame());

    const char* cause = RSTunnelRouteArbiter::ComputeGlobalForbiddenCause(mainThread);
    ASSERT_NE(cause, nullptr);
    EXPECT_STREQ(cause, "SCREENSHOT_ACTIVE");

    ClearUiCaptureTasks(*mainThread);
    mainThread->context_->GetMutableNodeMap().UnregisterRenderNode(node->GetId());
}

/**
 * @tc.name: ComputeGlobalForbiddenCause_ReturnsNullptr_WhenCleanState
 * @tc.desc: With no mirror display, no pending ui-capture task, and no non-mirror virtual
 *           screen registered, the unified probe must return nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelRouteArbiterTest,
    RSTunnelRouteArbiter_ComputeGlobalForbiddenCause_ReturnsNullptr_WhenCleanState, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);

    ClearUiCaptureTasks(*mainThread);
    ASSERT_FALSE(mainThread->IsSnapshotPendingThisFrame());

    EXPECT_EQ(RSTunnelRouteArbiter::ComputeGlobalForbiddenCause(mainThread), nullptr);
}
} // namespace OHOS::Rosen
