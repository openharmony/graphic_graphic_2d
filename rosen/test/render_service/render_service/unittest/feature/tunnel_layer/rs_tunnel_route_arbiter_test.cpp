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

#include "event_handler.h"
#include "feature/tunnel_layer/rs_tunnel_route_arbiter.h"
#include "gtest/gtest.h"
#include "params/rs_surface_render_params.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_test_util.h"
#include "feature/tunnel_layer/rs_tunnel_runtime_state.h"
#include "rs_tunnel_test_utils.h"
#include "screen_manager/rs_screen_property.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
using TunnelTest::CreateTunnelTestContext;
using TunnelTest::ScopedNewTunnelSwitch;
namespace {
constexpr uint64_t TEST_TUNNEL_LAYER_ID = 1001;
constexpr ScreenId CLAIMED_SCREEN_ID = 7;
constexpr ScreenId MIGRATED_SCREEN_ID = 9;
constexpr ScreenId UNRELATED_SCREEN_ID = 5;
constexpr ScreenId TEST_PHYSICAL_SCREEN_ID = 1001;
constexpr ScreenId TEST_VIRTUAL_SCREEN_ID = 1002;

void ActivateTunnelRuntime(RSTunnelRuntimeState& tunnelRuntime)
{
    tunnelRuntime.SetBuilding();
    ASSERT_TRUE(tunnelRuntime.SetActiveFromTunnelLayerAvailable(tunnelRuntime.GetTunnelLayerGeneration()));
}

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

std::shared_ptr<RSScreenRenderNode> CreateScreenNodeForTest(NodeId nodeId, ScreenId screenId, bool isVirtual)
{
    auto mainThread = RSMainThread::Instance();
    if (mainThread == nullptr) {
        return nullptr;
    }
    auto screenNode = std::make_shared<RSScreenRenderNode>(nodeId, screenId, mainThread->context_);
    if (screenNode == nullptr) {
        return nullptr;
    }
    RSScreenProperty screenProperty;
    screenProperty.Set<ScreenPropertyType::ID>(screenId);
    screenProperty.Set<ScreenPropertyType::IS_VIRTUAL>(isVirtual);
    screenNode->SetScreenProperty(screenProperty);
    return screenNode;
}

class ScopedRegisteredScreenNode {
public:
    explicit ScopedRegisteredScreenNode(const std::shared_ptr<RSScreenRenderNode>& screenNode) : screenNode_(screenNode)
    {
        auto mainThread = RSMainThread::Instance();
        if (mainThread == nullptr || screenNode_ == nullptr) {
            return;
        }
        registered_ = mainThread->context_->GetMutableNodeMap().RegisterRenderNode(screenNode_);
    }

    ~ScopedRegisteredScreenNode()
    {
        auto mainThread = RSMainThread::Instance();
        if (mainThread == nullptr || screenNode_ == nullptr || !registered_) {
            return;
        }
        mainThread->context_->GetMutableNodeMap().UnregisterRenderNode(screenNode_->GetId());
    }

    bool IsRegistered() const
    {
        return registered_;
    }

private:
    std::shared_ptr<RSScreenRenderNode> screenNode_;
    bool registered_ = false;
};

class ScopedMainThreadHandler {
public:
    explicit ScopedMainThreadHandler(const std::shared_ptr<AppExecFwk::EventHandler>& handler)
        : mainThread_(RSMainThread::Instance()), oldHandler_(mainThread_ == nullptr ? nullptr : mainThread_->handler_)
    {
        if (mainThread_ != nullptr) {
            mainThread_->handler_ = handler;
        }
    }

    ~ScopedMainThreadHandler()
    {
        if (mainThread_ != nullptr) {
            mainThread_->handler_ = oldHandler_;
        }
    }

private:
    RSMainThread* mainThread_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> oldHandler_;
};

enum class RouteMutationType {
    NONE,
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
} // namespace

class RSTunnelRouteArbiterTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        RSTestUtil::InitRenderNodeGC();
    }

    void TearDown() override
    {
        RSTunnelRouteArbiter::SetTunnelSolePresentLayer(false);
    }
};

/**
 * @tc.name: RSTunnelRouteArbiter_TunnelSolePresentLayer_DefaultFalse_ThenSetTrue_ThenSetFalse
 * @tc.desc: Cover the true and false branches of the load/store paths in
 *          GetTunnelSolePresentLayer/SetTunnelSolePresentLayer so the static atomic gate
 *          reflects every write and reads back the latest value.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelRouteArbiterTest,
    RSTunnelRouteArbiter_TunnelSolePresentLayer_DefaultFalse_ThenSetTrue_ThenSetFalse, TestSize.Level1)
{
    RSTunnelRouteArbiter::SetTunnelSolePresentLayer(false);
    EXPECT_FALSE(RSTunnelRouteArbiter::GetTunnelSolePresentLayer());

    RSTunnelRouteArbiter::SetTunnelSolePresentLayer(true);
    EXPECT_TRUE(RSTunnelRouteArbiter::GetTunnelSolePresentLayer());

    RSTunnelRouteArbiter::SetTunnelSolePresentLayer(false);
    EXPECT_FALSE(RSTunnelRouteArbiter::GetTunnelSolePresentLayer());
}

/**
 * @tc.name: RSTunnelRouteArbiter_NewStaticFunctions
 * @tc.desc: Cover the new building-threshold and consecutive-sole-present static functions:
 *          GetBuildingThreshold, GetConsecutiveSolePresentCount, ResetConsecutiveSolePresentCount,
 *          OnTunnelRollbackDetected, OnDirectCompositionStable, and BeginTunnelBuilding.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelRouteArbiterTest,
    RSTunnelRouteArbiter_NewStaticFunctions, TestSize.Level1)
{
    RSTunnelRouteArbiter::OnDirectCompositionStable(TUNNEL_STABLE_THRESHOLD + 1);
    RSTunnelRouteArbiter::ResetConsecutiveSolePresentCount();
    EXPECT_EQ(RSTunnelRouteArbiter::GetBuildingThreshold(), TUNNEL_NOT_TUNNEL_TO_BUILDING_THRESHOLD);
    EXPECT_EQ(RSTunnelRouteArbiter::GetConsecutiveSolePresentCount(), 0u);

    RSTunnelRouteArbiter::consecutiveSolePresentCount_.store(TUNNEL_STABLE_THRESHOLD, std::memory_order_release);
    RSTunnelRouteArbiter::OnTunnelRollbackDetected();
    EXPECT_EQ(RSTunnelRouteArbiter::GetBuildingThreshold(), TUNNEL_NOT_TUNNEL_TO_BUILDING_THRESHOLD + 1);
    EXPECT_EQ(RSTunnelRouteArbiter::GetConsecutiveSolePresentCount(), 0u);

    RSTunnelRouteArbiter::OnDirectCompositionStable(TUNNEL_STABLE_THRESHOLD);
    EXPECT_EQ(RSTunnelRouteArbiter::GetBuildingThreshold(), TUNNEL_NOT_TUNNEL_TO_BUILDING_THRESHOLD + 1);
    RSTunnelRouteArbiter::OnDirectCompositionStable(TUNNEL_STABLE_THRESHOLD + 1);
    EXPECT_EQ(RSTunnelRouteArbiter::GetBuildingThreshold(), TUNNEL_NOT_TUNNEL_TO_BUILDING_THRESHOLD);

    RSTunnelRuntimeState tunnelRuntime;
    RSTunnelRouteArbiter::BeginTunnelBuilding(tunnelRuntime, 1, TEST_TUNNEL_LAYER_ID);
    EXPECT_EQ(tunnelRuntime.GetTunnelState(), RSTunnelRuntimeState::TunnelState::NOT_TUNNEL);

    RSTunnelRouteArbiter::consecutiveSolePresentCount_.store(
        TUNNEL_NOT_TUNNEL_TO_BUILDING_THRESHOLD, std::memory_order_release);
    RSTunnelRouteArbiter::BeginTunnelBuilding(tunnelRuntime, 1, TEST_TUNNEL_LAYER_ID);
    EXPECT_EQ(tunnelRuntime.GetTunnelState(), RSTunnelRuntimeState::TunnelState::BUILDING);

    RSTunnelRouteArbiter::OnDirectCompositionStable(TUNNEL_STABLE_THRESHOLD + 1);
    RSTunnelRouteArbiter::ResetConsecutiveSolePresentCount();
}

/**
 * @tc.name: ArbitrateAndClaim_RejectsNullAndInactiveNode
 * @tc.desc: Defensive branches must report NOT_TUNNEL_ACTIVE without mutating runtime phase.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelRouteArbiterTest,
    RSTunnelRouteArbiter_ArbitrateAndClaim_RejectsNullAndInactiveNode, TestSize.Level1)
{
    RSTunnelRouteArbiter arbiter;
    EXPECT_EQ(arbiter.ArbitrateAndClaim(nullptr), RSTunnelRouteArbiter::MainThreadOutcome::NOT_TUNNEL_ACTIVE);

    auto context = CreateTunnelTestContext(false);
    ASSERT_TRUE(context.IsBaseReady());
    EXPECT_EQ(context.node->GetTunnelRuntimeState()->GetTunnelState(),
        RSTunnelRuntimeState::TunnelState::NOT_TUNNEL);
    EXPECT_EQ(arbiter.ArbitrateAndClaim(context.node), RSTunnelRouteArbiter::MainThreadOutcome::NOT_TUNNEL_ACTIVE);
    EXPECT_EQ(context.node->GetTunnelRuntimeState()->GetPhase(),
        RSTunnelRuntimeState::Phase::TUNNEL_IDLE);

    arbiter.AbandonNormalClaim(nullptr);
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

    auto& tunnelRuntime = *context.node->GetTunnelRuntimeState();
    ActivateTunnelRuntime(tunnelRuntime);
    context.surfaceHandler->SetAvailableBufferCount(1);
    auto claimedScreenNode = std::make_shared<RSScreenRenderNode>(0, CLAIMED_SCREEN_ID);
    context.node->SetAncestorScreenNode(claimedScreenNode);

    RSTunnelRouteArbiter arbiter;
    auto outcome = arbiter.ArbitrateAndClaim(context.node);
    ASSERT_EQ(outcome, RSTunnelRouteArbiter::MainThreadOutcome::GO_NORMAL);
    ASSERT_EQ(tunnelRuntime.GetPhase(), RSTunnelRuntimeState::Phase::NORMAL_PREPARING);

    auto migratedScreenNode = std::make_shared<RSScreenRenderNode>(0, MIGRATED_SCREEN_ID);
    context.node->SetAncestorScreenNode(migratedScreenNode);

    arbiter.OnRenderCommitDone(UNRELATED_SCREEN_ID);
    EXPECT_EQ(tunnelRuntime.GetPhase(), RSTunnelRuntimeState::Phase::NORMAL_PREPARING);

    arbiter.OnRenderCommitDone(CLAIMED_SCREEN_ID);
    EXPECT_EQ(tunnelRuntime.GetPhase(), RSTunnelRuntimeState::Phase::NORMAL_COMMITTED);
}

/**
 * @tc.name: OnRenderCommitDone_DropsExpiredNormalRouteNode
 * @tc.desc: An expired weak node in the normal-route list must be removed without dereferencing it.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelRouteArbiterTest,
    RSTunnelRouteArbiter_OnRenderCommitDone_DropsExpiredNormalRouteNode, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    RSTunnelRouteArbiter arbiter;
    std::weak_ptr<RSSurfaceRenderNode> weakNode;
    {
        auto context = CreateTunnelTestContext(false);
        ASSERT_TRUE(context.IsBaseReady());
        weakNode = context.node;

        ActivateTunnelRuntime(*context.node->GetTunnelRuntimeState());
        context.surfaceHandler->SetAvailableBufferCount(1);
        auto claimedScreenNode = std::make_shared<RSScreenRenderNode>(0, CLAIMED_SCREEN_ID);
        context.node->SetAncestorScreenNode(claimedScreenNode);
        ASSERT_EQ(arbiter.ArbitrateAndClaim(context.node), RSTunnelRouteArbiter::MainThreadOutcome::GO_NORMAL);
    }
    ASSERT_EQ(weakNode.lock(), nullptr);

    arbiter.OnRenderCommitDone(CLAIMED_SCREEN_ID);
    arbiter.OnRenderCommitDone(CLAIMED_SCREEN_ID);
}

/**
 * @tc.name: AttachToRenderThread_CommitDoneCallbackAdvancesClaimedNode
 * @tc.desc: The render-thread commit-done callback posts back to main and advances the claimed screen node.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelRouteArbiterTest,
    RSTunnelRouteArbiter_AttachToRenderThread_CommitDoneCallbackAdvancesClaimedNode, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    auto runner = AppExecFwk::EventRunner::Create(false);
    ASSERT_NE(runner, nullptr);
    ScopedMainThreadHandler scopedHandler(std::make_shared<AppExecFwk::EventHandler>(runner));
    ASSERT_NE(mainThread->handler_, nullptr);

    auto context = CreateTunnelTestContext(false);
    ASSERT_TRUE(context.IsBaseReady());
    auto& tunnelRuntime = *context.node->GetTunnelRuntimeState();
    ActivateTunnelRuntime(tunnelRuntime);
    context.surfaceHandler->SetAvailableBufferCount(1);
    auto claimedScreenNode = std::make_shared<RSScreenRenderNode>(0, CLAIMED_SCREEN_ID);
    context.node->SetAncestorScreenNode(claimedScreenNode);

    RSTunnelRouteArbiter arbiter;
    ASSERT_EQ(arbiter.ArbitrateAndClaim(context.node), RSTunnelRouteArbiter::MainThreadOutcome::GO_NORMAL);
    ASSERT_EQ(tunnelRuntime.GetPhase(), RSTunnelRuntimeState::Phase::NORMAL_PREPARING);

    arbiter.AttachToRenderThread();
    RSUniRenderThread::Instance().NotifyCommitDone(CLAIMED_SCREEN_ID);
    mainThread->handler_->PostTask([runner]() { runner->Stop(); });
    runner->Run();

    EXPECT_EQ(tunnelRuntime.GetPhase(), RSTunnelRuntimeState::Phase::NORMAL_COMMITTED);
    RSUniRenderThread::Instance().SetCommitDoneCallback(nullptr);
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
    mainThread->AddUiCaptureTask(node->GetId(), false, []() {});
    ASSERT_TRUE(mainThread->IsSnapshotPendingThisFrame());

    const char* cause = RSTunnelRouteArbiter::ComputeGlobalForbiddenCause(mainThread);
    ASSERT_NE(cause, nullptr);
    EXPECT_STREQ(cause, "SCREENSHOT_ACTIVE");

    ClearUiCaptureTasks(*mainThread);
    mainThread->context_->GetMutableNodeMap().UnregisterRenderNode(node->GetId());
}

/**
 * @tc.name: IsSnapshotPendingThisFrame_WindowCapTask_Pending
 * @tc.desc: When a windowCap task is pending, IsSnapshotPendingThisFrame becomes true
 *           and ComputeGlobalForbiddenCause reports SCREENSHOT_ACTIVE.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelRouteArbiterTest,
    IsSnapshotPendingThisFrame_WindowCapTask_Pending, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    ClearUiCaptureTasks(*mainThread);
    mainThread->pendingWindowCapTasks_.clear();
    mainThread->windowCapTasks_ = std::queue<std::tuple<NodeId, std::function<void()>>>();
    ASSERT_FALSE(mainThread->IsSnapshotPendingThisFrame());

    std::function<void()> task = []() {};
    mainThread->pendingWindowCapTasks_.emplace_back(1, task, 0, 0, false);
    ASSERT_TRUE(mainThread->IsSnapshotPendingThisFrame());

    const char* cause = RSTunnelRouteArbiter::ComputeGlobalForbiddenCause(mainThread);
    ASSERT_NE(cause, nullptr);
    EXPECT_STREQ(cause, "SCREENSHOT_ACTIVE");

    mainThread->pendingWindowCapTasks_.clear();
    mainThread->windowCapTasks_ = std::queue<std::tuple<NodeId, std::function<void()>>>();
}

/**
 * @tc.name: ComputeGlobalForbiddenCause_ReturnsVirtualScreenActive_ForNonMirrorVirtualScreen
 * @tc.desc: A non-mirror virtual screen is a global normal-route trigger.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelRouteArbiterTest,
    RSTunnelRouteArbiter_ComputeGlobalForbiddenCause_ReturnsVirtualScreenActive_ForNonMirrorVirtualScreen,
    TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    RSTunnelRouteArbiter::consecutiveSolePresentCount_.store(TUNNEL_STABLE_THRESHOLD, std::memory_order_release);
    RSTunnelRouteArbiter::RefreshGlobalTriggerSnapshot();
    ClearUiCaptureTasks(*mainThread);

    auto virtualScreen = CreateScreenNodeForTest(GenerateUniqueNodeIdForRS(), TEST_VIRTUAL_SCREEN_ID, true);
    ASSERT_NE(virtualScreen, nullptr);
    ScopedRegisteredScreenNode registeredVirtualScreen(virtualScreen);
    ASSERT_TRUE(registeredVirtualScreen.IsRegistered());

    const char* cause = RSTunnelRouteArbiter::ComputeGlobalForbiddenCause(mainThread);
    ASSERT_NE(cause, nullptr);
    EXPECT_STREQ(cause, "VIRTUAL_SCREEN_ACTIVE");
}

/**
 * @tc.name: ComputeGlobalForbiddenCause_IgnoresMirrorVirtualScreen
 * @tc.desc: A mirror virtual screen is filtered by HasNonMirrorVirtualScreen and does not report virtual-screen cause.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelRouteArbiterTest,
    RSTunnelRouteArbiter_ComputeGlobalForbiddenCause_IgnoresMirrorVirtualScreen, TestSize.Level1)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    ClearUiCaptureTasks(*mainThread);

    auto physicalScreen = CreateScreenNodeForTest(GenerateUniqueNodeIdForRS(), TEST_PHYSICAL_SCREEN_ID, false);
    auto virtualScreen = CreateScreenNodeForTest(GenerateUniqueNodeIdForRS(), TEST_VIRTUAL_SCREEN_ID, true);
    ASSERT_NE(physicalScreen, nullptr);
    ASSERT_NE(virtualScreen, nullptr);
    virtualScreen->SetIsMirrorScreen(true);
    virtualScreen->SetMirrorSource(physicalScreen);

    ScopedRegisteredScreenNode registeredPhysicalScreen(physicalScreen);
    ScopedRegisteredScreenNode registeredVirtualScreen(virtualScreen);
    ASSERT_TRUE(registeredPhysicalScreen.IsRegistered());
    ASSERT_TRUE(registeredVirtualScreen.IsRegistered());

    EXPECT_EQ(RSTunnelRouteArbiter::ComputeGlobalForbiddenCause(mainThread), nullptr);
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
    RSTunnelRouteArbiter::consecutiveSolePresentCount_.store(TUNNEL_STABLE_THRESHOLD, std::memory_order_release);
    RSTunnelRouteArbiter::RefreshGlobalTriggerSnapshot();
    ClearUiCaptureTasks(*mainThread);
    ASSERT_FALSE(mainThread->IsSnapshotPendingThisFrame());

EXPECT_EQ(RSTunnelRouteArbiter::ComputeGlobalForbiddenCause(mainThread), nullptr);
}

/**
 * @tc.name: ArbitrateAndClaim_NoRuntimeState_ReturnsNotTunnelActive
 * @tc.desc: Cover null tunnelRuntime early return in ArbitrateAndClaim.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelRouteArbiterTest, ArbitrateAndClaim_NoRuntimeState_ReturnsNotTunnelActive, TestSize.Level1)
{
    auto context = CreateTunnelTestContext(false);
    ASSERT_TRUE(context.IsBaseReady());
    EXPECT_EQ(context.node->TryGetTunnelRuntimeState(), nullptr);
    RSTunnelRouteArbiter arbiter;
    EXPECT_EQ(arbiter.ArbitrateAndClaim(context.node),
        RSTunnelRouteArbiter::MainThreadOutcome::NOT_TUNNEL_ACTIVE);
}

/**
 * @tc.name: AbandonNormalClaim_NoRuntimeState_NoOp
 * @tc.desc: Cover null tunnelRuntime and mainThread-not-null branches in AbandonNormalClaim.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelRouteArbiterTest, AbandonNormalClaim_NoRuntimeState_NoOp, TestSize.Level1)
{
    auto context = CreateTunnelTestContext(false);
    ASSERT_TRUE(context.IsBaseReady());
    RSTunnelRouteArbiter arbiter;
    arbiter.AbandonNormalClaim(context.node);
    EXPECT_EQ(context.node->GetTunnelRuntimeState()->GetPhase(),
        RSTunnelRuntimeState::Phase::TUNNEL_IDLE);
}

/**
 * @tc.name: GetAllLayerAndTunnelLayerCount_CoversRuntimeStateBranches
 * @tc.desc: Cover both statePtr==nullptr and statePtr!=nullptr in GetAllLayerAndTunnelLayerCount.
 * @tc.type: FUNC
 */
HWTEST_F(RSTunnelRouteArbiterTest, GetAllLayerAndTunnelLayerCount_CoversRuntimeStateBranches, TestSize.Level1)
{
    ScopedNewTunnelSwitch scopedNewTunnelSwitch(true);
    auto context = CreateTunnelTestContext(false);
    ASSERT_TRUE(context.IsBaseReady());
    context.surfaceHandler->SetCurrentFrameBufferConsumed();

    uint32_t presentCount = 0;
    uint32_t tunnelCount = 0;
    RSTunnelRouteArbiter::GetAllLayerAndTunnelLayerCount(context.node, presentCount, tunnelCount);
    EXPECT_EQ(context.node->TryGetTunnelRuntimeState(), nullptr);
    EXPECT_EQ(presentCount, 1u);
    EXPECT_EQ(tunnelCount, 0u);

    context.node->GetTunnelRuntimeState()->SetLayerInfo(
        TEST_TUNNEL_LAYER_ID, TUNNEL_PROP_BUFFER_ADDR);
    presentCount = 0;
    tunnelCount = 0;
    RSTunnelRouteArbiter::GetAllLayerAndTunnelLayerCount(context.node, presentCount, tunnelCount);
    EXPECT_EQ(presentCount, 1u);
    EXPECT_EQ(tunnelCount, 1u);
}

} // namespace OHOS::Rosen
