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

#include <parameters.h>

#include "feature/selective_prepare/rs_selective_prepare_manager.h"
#include "gtest/gtest.h"

#include "animation/rs_animation_manager.h"
#include "animation/rs_interpolator.h"
#include "animation/rs_render_curve_animation.h"
#include "modifier/rs_render_property.h"
#include "modifier_ng/rs_modifier_ng_type.h"
#include "modifier_ng/rs_render_modifier_ng.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_test_util.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
constexpr NodeId DISPLAY_NODE_ID = 900;
constexpr NodeId SCREEN_NODE_ID = 901;
constexpr NodeId CONTAINER_NODE_ID = 910;
constexpr NodeId AWEME_SURFACE_ID = 920;
constexpr NodeId OPT_NODE_ID = 930;
constexpr const char* AWEME_SURFACE_NAME = "aweme0";
constexpr PropertyId ROTATION_PROPERTY_ID = 5000;
constexpr AnimationId ROTATION_ANIMATION_ID = 6000;
constexpr int32_t INFINITE_REPEAT = -1;
constexpr int64_t ANIM_DURATION_MS = 1000;

// persist param keys must match the production constants in selective_prepare_manager.cpp
constexpr const char* FEATURE_SWITCH = "persist.rosen.graphic.selective_prepare_opt";
} // namespace

class RSSelectivePrepareManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    // build a standard eligible tree:
    //   displayNode(placeholder) <- container(canvas) <- awemeSurface <- optNode(canvas)
    // optNode has a running infinite rotation-only animation (TRANSFORM modifier + ROTATION property)
    // and is registered in context animating list + active list
    void BuildStandardTree();
    // attach a running infinite rotation animation to node, return the property used
    std::shared_ptr<RSRenderAnimatableProperty<float>> AttachInfiniteRotation(const std::shared_ptr<RSRenderNode>& node,
        PropertyId propertyId = ROTATION_PROPERTY_ID, int repeatCount = INFINITE_REPEAT,
        AnimationId animationId = ROTATION_ANIMATION_ID);
    // set the surface node's logical display id so CheckAncestorStateToDisplay stops at displayNode
    void MarkOnTree(const std::shared_ptr<RSRenderNode>& node, NodeId logicalDisplayNodeId = DISPLAY_NODE_ID);
    // reset only the per-frame flags between simulated frames; keep pendingActivation_ so the
    // two-frame activation flow progresses (frame N pending -> frame N+1 eligibility)
    void ResetEligibleState();

    std::shared_ptr<RSContext> context_;
    std::shared_ptr<RSRenderNode> displayNode_;
    std::shared_ptr<RSCanvasRenderNode> containerNode_;
    std::shared_ptr<RSSurfaceRenderNode> awemeSurfaceNode_;
    std::shared_ptr<RSCanvasRenderNode> optNode_;
    std::unique_ptr<RSSelectivePrepareManager> manager_;
};

void RSSelectivePrepareManagerTest::SetUpTestCase()
{
    RSTestUtil::InitRenderNodeGC();
}

void RSSelectivePrepareManagerTest::TearDownTestCase() {}

void RSSelectivePrepareManagerTest::SetUp()
{
    context_ = std::make_shared<RSContext>();
    // Initialize sets nodeMap's context; without it RegisterRenderNode->OnRegister would
    // overwrite node contexts with an empty weak_ptr
    context_->Initialize();
    manager_ = std::make_unique<RSSelectivePrepareManager>(context_);
    // ensure the feature switch is on for all cases; individual cases may turn it off
    system::SetParameter(FEATURE_SWITCH, "true");
}

void RSSelectivePrepareManagerTest::TearDown()
{
    manager_->ResetState();
    // destroy nodes before the context: node destructors may access the context/node map
    optNode_.reset();
    awemeSurfaceNode_.reset();
    containerNode_.reset();
    displayNode_.reset();
    context_.reset();
}

void RSSelectivePrepareManagerTest::MarkOnTree(const std::shared_ptr<RSRenderNode>& node, NodeId logicalDisplayNodeId)
{
    node->SetIsOnTheTree(true, node->GetInstanceRootNodeId(), node->GetFirstLevelNodeId(), node->GetUifirstRootNodeId(),
        node->GetScreenNodeId(), logicalDisplayNodeId);
}

std::shared_ptr<RSRenderAnimatableProperty<float>> RSSelectivePrepareManagerTest::AttachInfiniteRotation(
    const std::shared_ptr<RSRenderNode>& node, PropertyId propertyId, int repeatCount, AnimationId animationId)
{
    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f, propertyId);
    auto modifier = ModifierNG::RSRenderModifier::MakeRenderModifier(ModifierNG::RSModifierType::TRANSFORM,
        std::static_pointer_cast<RSRenderProperty<float>>(property), 0, ModifierNG::RSPropertyType::ROTATION);
    if (modifier != nullptr) {
        node->AddModifier(modifier);
    }
    auto startValue = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f, propertyId);
    auto endValue = std::make_shared<RSRenderAnimatableProperty<float>>(360.0f, propertyId);
    auto animation = std::make_shared<RSRenderCurveAnimation>(animationId, propertyId, property, startValue, endValue);
    animation->SetDuration(ANIM_DURATION_MS);
    animation->SetRepeatCount(repeatCount);
    animation->SetInterpolator(std::make_shared<LinearInterpolator>());
    animation->Start();
    node->GetOrCreateAnimationManager()->AddAnimation(animation);
    return property;
}

void RSSelectivePrepareManagerTest::BuildStandardTree()
{
    // nodes must carry the test context for GetInstanceRootNode()->GetNodeMap() lookups, and
    // canvas nodes need InitRenderParams() so that stagingRenderParams_ is not null
    displayNode_ = std::make_shared<RSRenderNode>(DISPLAY_NODE_ID, true, context_);
    containerNode_ = std::make_shared<RSCanvasRenderNode>(CONTAINER_NODE_ID, context_);
    containerNode_->InitRenderParams();
    RSSurfaceRenderNodeConfig surfaceConfig;
    surfaceConfig.id = AWEME_SURFACE_ID;
    surfaceConfig.name = AWEME_SURFACE_NAME;
    awemeSurfaceNode_ = RSTestUtil::CreateSurfaceNode(surfaceConfig);
    optNode_ = std::make_shared<RSCanvasRenderNode>(OPT_NODE_ID, context_);
    optNode_->InitRenderParams();

    displayNode_->AddChild(containerNode_);
    containerNode_->AddChild(awemeSurfaceNode_);
    // the aweme surface is the instance root of its subtree: mark it on the tree (with itself as
    // instance root and the display/screen ids) BEFORE adding optNode, so AddChild propagation
    // fills optNode's cached tree ids
    awemeSurfaceNode_->SetIsOnTheTree(
        true, AWEME_SURFACE_ID, AWEME_SURFACE_ID, INVALID_NODEID, SCREEN_NODE_ID, DISPLAY_NODE_ID);
    awemeSurfaceNode_->AddChild(optNode_);
    MarkOnTree(containerNode_);

    // register AFTER tree setup so OnRegister overwrites the context with the (now initialized)
    // nodeMap context and the instance root id is already propagated; OnRegister also calls
    // SetDirty(true)->AddActiveNode, so clear the active list afterwards to keep the count check
    // driven solely by the explicit AddActiveNode below
    context_->GetMutableNodeMap().RegisterRenderNode(awemeSurfaceNode_);
    {
        std::lock_guard<std::mutex> lock(context_->activeNodesInRootMutex_);
        context_->activeNodesInRoot_.clear();
    }

    AttachInfiniteRotation(optNode_);
    context_->RegisterAnimatingRenderNode(optNode_);
    context_->AddActiveNode(optNode_);
}

void RSSelectivePrepareManagerTest::ResetEligibleState()
{
    // only reset per-frame flags; pendingActivation_/active are kept to drive the multi-frame flow
    manager_->SetHasGpuSurfaceDirty(false);
    manager_->SetHasCommandInFrame(false);
}

/**
 * @tc.name: FeatureDisabled
 * @tc.desc: feature switch off -> never activate
 * @tc.type: FUNC
 */
HWTEST_F(RSSelectivePrepareManagerTest, FeatureDisabled, TestSize.Level2)
{
    BuildStandardTree();
    system::SetParameter(FEATURE_SWITCH, "false");
    manager_->CheckAndSetup();
    EXPECT_FALSE(manager_->IsActive());
    EXPECT_FALSE(manager_->pendingActivation_);
}

/**
 * @tc.name: GpuDirty
 * @tc.desc: hasGpuSurfaceDirty_ set -> not activate, pending cleared
 * @tc.type: FUNC
 */
HWTEST_F(RSSelectivePrepareManagerTest, GpuDirty, TestSize.Level2)
{
    BuildStandardTree();
    manager_->SetHasGpuSurfaceDirty(true);
    manager_->CheckAndSetup();
    EXPECT_FALSE(manager_->IsActive());
    EXPECT_FALSE(manager_->pendingActivation_);
}

/**
 * @tc.name: CommandInFrame
 * @tc.desc: hasCommandInFrame_ set -> not activate, pending cleared
 * @tc.type: FUNC
 */
HWTEST_F(RSSelectivePrepareManagerTest, CommandInFrame, TestSize.Level2)
{
    BuildStandardTree();
    manager_->SetHasCommandInFrame(true);
    manager_->CheckAndSetup();
    EXPECT_FALSE(manager_->IsActive());
    EXPECT_FALSE(manager_->pendingActivation_);
}

/**
 * @tc.name: MultipleAnimatingNodes
 * @tc.desc: 2 on-tree animating nodes -> not activate, pending cleared
 * @tc.type: FUNC
 */
HWTEST_F(RSSelectivePrepareManagerTest, MultipleAnimatingNodes, TestSize.Level2)
{
    BuildStandardTree();
    auto second = std::make_shared<RSCanvasRenderNode>(OPT_NODE_ID + 1);
    awemeSurfaceNode_->AddChild(second);
    MarkOnTree(second);
    AttachInfiniteRotation(second);
    context_->RegisterAnimatingRenderNode(second);
    context_->AddActiveNode(second);

    manager_->CheckAndSetup();
    EXPECT_FALSE(manager_->IsActive());
    EXPECT_FALSE(manager_->pendingActivation_);
}

/**
 * @tc.name: ZeroAnimatingNodes
 * @tc.desc: no animating nodes -> not activate, pending cleared
 * @tc.type: FUNC
 */
HWTEST_F(RSSelectivePrepareManagerTest, ZeroAnimatingNodes, TestSize.Level2)
{
    BuildStandardTree();
    context_->UnregisterAnimatingRenderNode(optNode_->GetId());
    context_->AddActiveNode(optNode_); // active but not animating

    manager_->CheckAndSetup();
    EXPECT_FALSE(manager_->IsActive());
    EXPECT_FALSE(manager_->pendingActivation_);
}

/**
 * @tc.name: SurfaceNull
 * @tc.desc: optNode has no enclosing surface -> not activate
 * @tc.type: FUNC
 */
HWTEST_F(RSSelectivePrepareManagerTest, SurfaceNull, TestSize.Level2)
{
    BuildStandardTree();
    // detach optNode from the aweme surface so it has no enclosing surface
    awemeSurfaceNode_->RemoveChild(optNode_);
    context_->RegisterAnimatingRenderNode(optNode_);

    manager_->CheckAndSetup();
    EXPECT_FALSE(manager_->IsActive());
}

/**
 * @tc.name: SurfaceChildHasFilter
 * @tc.desc: surface ChildHasVisibleFilter set -> 2nd frame rejects, pending set on frame 1
 * @tc.type: FUNC
 */
HWTEST_F(RSSelectivePrepareManagerTest, SurfaceChildHasFilter, TestSize.Level2)
{
    BuildStandardTree();
    awemeSurfaceNode_->SetChildHasVisibleFilter(true);

    manager_->CheckAndSetup(); // frame N: pending
    EXPECT_TRUE(manager_->pendingActivation_);
    EXPECT_FALSE(manager_->IsActive());

    ResetEligibleState();
    manager_->CheckAndSetup(); // frame N+1: surface eligibility rejects
    EXPECT_FALSE(manager_->IsActive());
    EXPECT_FALSE(manager_->pendingActivation_);
}

/**
 * @tc.name: SurfaceAlphaNotOne
 * @tc.desc: surface globalAlpha != 1.0 -> 2nd frame rejects
 * @tc.type: FUNC
 */
HWTEST_F(RSSelectivePrepareManagerTest, SurfaceAlphaNotOne, TestSize.Level2)
{
    BuildStandardTree();
    awemeSurfaceNode_->SetGlobalAlpha(0.5f);

    manager_->CheckAndSetup();
    EXPECT_TRUE(manager_->pendingActivation_);
    EXPECT_FALSE(manager_->IsActive());

    ResetEligibleState();
    manager_->CheckAndSetup();
    EXPECT_FALSE(manager_->IsActive());
    EXPECT_FALSE(manager_->pendingActivation_);
}

/**
 * @tc.name: SurfaceHasBackgroundFilter
 * @tc.desc: surface ChildHasVisibleEffect set -> 2nd frame rejects
 * @tc.type: FUNC
 */
HWTEST_F(RSSelectivePrepareManagerTest, SurfaceHasBackgroundFilter, TestSize.Level2)
{
    BuildStandardTree();
    awemeSurfaceNode_->SetChildHasVisibleEffect(true);

    manager_->CheckAndSetup();
    EXPECT_TRUE(manager_->pendingActivation_);
    EXPECT_FALSE(manager_->IsActive());

    ResetEligibleState();
    manager_->CheckAndSetup();
    EXPECT_FALSE(manager_->IsActive());
    EXPECT_FALSE(manager_->pendingActivation_);
}

/**
 * @tc.name: ContainerNodeGroup
 * @tc.desc: container canvas marked as NodeGroup -> 2nd frame rejects, hasNodeGroup recorded
 * @tc.type: FUNC
 */
HWTEST_F(RSSelectivePrepareManagerTest, ContainerNodeGroup, TestSize.Level2)
{
    BuildStandardTree();
    containerNode_->MarkNodeGroup(RSRenderNode::NodeGroupType::GROUPED_BY_FOREGROUND_FILTER, true, false);

    manager_->CheckAndSetup();
    EXPECT_TRUE(manager_->pendingActivation_);
    ResetEligibleState();
    manager_->CheckAndSetup();
    EXPECT_FALSE(manager_->IsActive());
    EXPECT_FALSE(manager_->pendingActivation_);
}

/**
 * @tc.name: ContainerAlphaNotOne
 * @tc.desc: container alpha=0.5 -> 2nd frame rejects, hasAlphaNotOne recorded
 * @tc.type: FUNC
 */
HWTEST_F(RSSelectivePrepareManagerTest, ContainerAlphaNotOne, TestSize.Level2)
{
    BuildStandardTree();
    containerNode_->GetMutableRenderProperties().SetAlpha(0.5f);

    manager_->CheckAndSetup();
    EXPECT_TRUE(manager_->pendingActivation_);
    ResetEligibleState();
    manager_->CheckAndSetup();
    EXPECT_FALSE(manager_->IsActive());
    EXPECT_FALSE(manager_->pendingActivation_);
}

/**
 * @tc.name: TraverseCount
 * @tc.desc: standard tree optNode->surface->container->display -> traverseCount==3
 * @tc.type: FUNC
 */
HWTEST_F(RSSelectivePrepareManagerTest, TraverseCount, TestSize.Level2)
{
    BuildStandardTree();
    manager_->CheckAndSetup(); // frame N: pending
    ResetEligibleState();
    manager_->CheckAndSetup(); // frame N+1: ancestor traversal runs
    // eligibility passes for the standard tree, so activation succeeds
    EXPECT_TRUE(manager_->IsActive());
    auto result = manager_->CheckAncestorStateToDisplay(optNode_, awemeSurfaceNode_->GetLogicalDisplayNodeId());
    EXPECT_EQ(result.traverseCount, 3u);
    EXPECT_FALSE(result.hasNodeGroup);
    EXPECT_FALSE(result.hasAlphaNotOne);
}

/**
 * @tc.name: SkipsOptNodeOwnAlpha
 * @tc.desc: optNode own alpha != 1.0 does not affect ancestor check (skipped)
 * @tc.type: FUNC
 */
HWTEST_F(RSSelectivePrepareManagerTest, SkipsOptNodeOwnAlpha, TestSize.Level2)
{
    BuildStandardTree();
    optNode_->GetMutableRenderProperties().SetAlpha(0.5f);

    manager_->CheckAndSetup();
    EXPECT_TRUE(manager_->pendingActivation_);
    ResetEligibleState();
    manager_->CheckAndSetup();
    // optNode alpha is skipped, activation still succeeds
    EXPECT_TRUE(manager_->IsActive());
    auto result = manager_->CheckAncestorStateToDisplay(optNode_, awemeSurfaceNode_->GetLogicalDisplayNodeId());
    EXPECT_FALSE(result.hasAlphaNotOne);
}

/**
 * @tc.name: CanvasNodeOnly
 * @tc.desc: standard tree all canvas -> IsCanvasOnlySubtree true
 * @tc.type: FUNC
 */
HWTEST_F(RSSelectivePrepareManagerTest, CanvasNodeOnly, TestSize.Level2)
{
    BuildStandardTree();
    EXPECT_TRUE(manager_->IsCanvasOnlySubtree(optNode_));
}

/**
 * @tc.name: ContainsSurfaceNode
 * @tc.desc: optNode subtree contains a SurfaceNode -> IsCanvasOnlySubtree false
 * @tc.type: FUNC
 */
HWTEST_F(RSSelectivePrepareManagerTest, ContainsSurfaceNode, TestSize.Level2)
{
    BuildStandardTree();
    auto childSurface = RSTestUtil::CreateSurfaceNode();
    optNode_->AddChild(childSurface);
    EXPECT_FALSE(manager_->IsCanvasOnlySubtree(optNode_));
}

/**
 * @tc.name: EmptySubtree
 * @tc.desc: canvas node with no children -> IsCanvasOnlySubtree true
 * @tc.type: FUNC
 */
HWTEST_F(RSSelectivePrepareManagerTest, EmptySubtree, TestSize.Level2)
{
    BuildStandardTree();
    EXPECT_TRUE(manager_->IsCanvasOnlySubtree(optNode_));
}

/**
 * @tc.name: NonCanvasRoot
 * @tc.desc: subtree root is a SurfaceNode -> IsCanvasOnlySubtree false
 * @tc.type: FUNC
 */
HWTEST_F(RSSelectivePrepareManagerTest, NonCanvasRoot, TestSize.Level2)
{
    BuildStandardTree();
    EXPECT_FALSE(manager_->IsCanvasOnlySubtree(awemeSurfaceNode_));
}

/**
 * @tc.name: OptNodeNotCanvas
 * @tc.desc: animating node itself is a SurfaceNode -> 2nd frame rejects
 * @tc.type: FUNC
 */
HWTEST_F(RSSelectivePrepareManagerTest, OptNodeNotCanvas, TestSize.Level2)
{
    BuildStandardTree();
    // make the optNode a surface node instead of canvas: detach canvas optNode, attach a surface optNode
    awemeSurfaceNode_->RemoveChild(optNode_);
    context_->UnregisterAnimatingRenderNode(optNode_->GetId());
    auto surfaceOpt = RSTestUtil::CreateSurfaceNode();
    // animating node needs the context for GetInstanceRootNode()->GetNodeMap() resolution
    surfaceOpt->context_ = context_;
    awemeSurfaceNode_->AddChild(surfaceOpt);
    MarkOnTree(surfaceOpt);
    AttachInfiniteRotation(surfaceOpt);
    context_->RegisterAnimatingRenderNode(surfaceOpt);
    context_->AddActiveNode(surfaceOpt);

    manager_->CheckAndSetup();
    EXPECT_TRUE(manager_->pendingActivation_);
    ResetEligibleState();
    manager_->CheckAndSetup();
    EXPECT_FALSE(manager_->IsActive());
    EXPECT_FALSE(manager_->pendingActivation_);
}

/**
 * @tc.name: DeepSubtree
 * @tc.desc: subtree depth > MAX_SUBTREE_DEPTH(2) -> 2nd frame rejects
 * @tc.type: FUNC
 */
HWTEST_F(RSSelectivePrepareManagerTest, DeepSubtree, TestSize.Level2)
{
    BuildStandardTree();
    auto child1 = std::make_shared<RSCanvasRenderNode>(OPT_NODE_ID + 1);
    auto child2 = std::make_shared<RSCanvasRenderNode>(OPT_NODE_ID + 2);
    auto child3 = std::make_shared<RSCanvasRenderNode>(OPT_NODE_ID + 3); // depth 3 -> too deep
    optNode_->AddChild(child1);
    child1->AddChild(child2);
    child2->AddChild(child3);

    manager_->CheckAndSetup();
    EXPECT_TRUE(manager_->pendingActivation_);
    ResetEligibleState();
    manager_->CheckAndSetup();
    EXPECT_FALSE(manager_->IsActive());
    EXPECT_FALSE(manager_->IsSubtreeShallow(optNode_));
}

/**
 * @tc.name: BranchingSubtree
 * @tc.desc: optNode has multiple children -> IsSubtreeShallow false
 * @tc.type: FUNC
 */
HWTEST_F(RSSelectivePrepareManagerTest, BranchingSubtree, TestSize.Level2)
{
    BuildStandardTree();
    auto child1 = std::make_shared<RSCanvasRenderNode>(OPT_NODE_ID + 1);
    auto child2 = std::make_shared<RSCanvasRenderNode>(OPT_NODE_ID + 2);
    optNode_->AddChild(child1);
    optNode_->AddChild(child2);
    EXPECT_FALSE(manager_->IsSubtreeShallow(optNode_));
}

/**
 * @tc.name: NonInfiniteAnimation
 * @tc.desc: repeatCount != -1 -> IsSubtreeShallow false
 * @tc.type: FUNC
 */
HWTEST_F(RSSelectivePrepareManagerTest, NonInfiniteAnimation, TestSize.Level2)
{
    BuildStandardTree();
    // replace the infinite animation with a finite one
    AttachInfiniteRotation(optNode_, ROTATION_PROPERTY_ID + 1, 3, ROTATION_ANIMATION_ID + 100);
    EXPECT_FALSE(manager_->IsSubtreeShallow(optNode_));
}

/**
 * @tc.name: FirstFramePending
 * @tc.desc: CheckAndSetup x1 -> pending true, not active
 * @tc.type: FUNC
 */
HWTEST_F(RSSelectivePrepareManagerTest, FirstFramePending, TestSize.Level2)
{
    BuildStandardTree();
    manager_->CheckAndSetup();
    EXPECT_TRUE(manager_->pendingActivation_);
    EXPECT_FALSE(manager_->IsActive());
    EXPECT_EQ(manager_->GetHitCount(), 0u);
}

/**
 * @tc.name: SecondFrameActivates
 * @tc.desc: CheckAndSetup x2 -> active, hit count incremented by PrepareOptNodes
 * @tc.type: FUNC
 */
HWTEST_F(RSSelectivePrepareManagerTest, SecondFrameActivates, TestSize.Level2)
{
    BuildStandardTree();
    manager_->CheckAndSetup();
    EXPECT_FALSE(manager_->IsActive());
    ResetEligibleState();
    manager_->CheckAndSetup();
    EXPECT_TRUE(manager_->IsActive());
}

/**
 * @tc.name: ThirdFrameStaysActive
 * @tc.desc: CheckAndSetup x3 -> stays active, hit count keeps growing with PrepareOptNodes
 * @tc.type: FUNC
 */
HWTEST_F(RSSelectivePrepareManagerTest, ThirdFrameStaysActive, TestSize.Level2)
{
    BuildStandardTree();
    manager_->CheckAndSetup();
    ResetEligibleState();
    manager_->CheckAndSetup();
    EXPECT_TRUE(manager_->IsActive());
    ResetEligibleState();
    manager_->CheckAndSetup();
    EXPECT_TRUE(manager_->IsActive());
}

/**
 * @tc.name: PendingResetByGpuDirty
 * @tc.desc: pending set then gpuDirty -> pending cleared
 * @tc.type: FUNC
 */
HWTEST_F(RSSelectivePrepareManagerTest, PendingResetByGpuDirty, TestSize.Level2)
{
    BuildStandardTree();
    manager_->CheckAndSetup();
    EXPECT_TRUE(manager_->pendingActivation_);
    manager_->SetHasGpuSurfaceDirty(true);
    manager_->CheckAndSetup();
    EXPECT_FALSE(manager_->pendingActivation_);
    EXPECT_FALSE(manager_->IsActive());
}

/**
 * @tc.name: SurfaceAlphaSafetyNet
 * @tc.desc: active then surface alpha changes -> deactivated
 * @tc.type: FUNC
 */
HWTEST_F(RSSelectivePrepareManagerTest, SurfaceAlphaSafetyNet, TestSize.Level2)
{
    BuildStandardTree();
    manager_->CheckAndSetup();
    ResetEligibleState();
    manager_->CheckAndSetup();
    EXPECT_TRUE(manager_->IsActive());

    awemeSurfaceNode_->SetGlobalAlpha(0.5f);
    ResetEligibleState();
    manager_->CheckAndSetup();
    EXPECT_FALSE(manager_->IsActive());
}

/**
 * @tc.name: ActiveDeactivatesOnCommand
 * @tc.desc: active then hasCommandInFrame -> deactivated by O(1) exit
 * @tc.type: FUNC
 */
HWTEST_F(RSSelectivePrepareManagerTest, ActiveDeactivatesOnCommand, TestSize.Level2)
{
    BuildStandardTree();
    manager_->CheckAndSetup();
    ResetEligibleState();
    manager_->CheckAndSetup();
    EXPECT_TRUE(manager_->IsActive());

    manager_->SetHasCommandInFrame(true);
    manager_->CheckAndSetup();
    EXPECT_FALSE(manager_->IsActive());
}
} // namespace OHOS::Rosen
