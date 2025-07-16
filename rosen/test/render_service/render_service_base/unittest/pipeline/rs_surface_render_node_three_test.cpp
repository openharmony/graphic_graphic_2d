/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>

#include "pipeline/rs_context.h"
#include "params/rs_surface_render_params.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_surface_handler.h"
#include "visitor/rs_node_visitor.h"
#include "common/rs_common_hook.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSSurfaceRenderNodeThreeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline NodeId id;
    static constexpr float outerRadius = 30.4f;
    RRect rrect = RRect({0, 0, 0, 0}, outerRadius, outerRadius);
};

void RSSurfaceRenderNodeThreeTest::SetUpTestCase() {}
void RSSurfaceRenderNodeThreeTest::TearDownTestCase() {}
void RSSurfaceRenderNodeThreeTest::SetUp() {}
void RSSurfaceRenderNodeThreeTest::TearDown() {}

class RSTestVisitor : public RSNodeVisitor {
public:
    RSTestVisitor() = default;
    ~RSTestVisitor() override {}

    void QuickPrepareScreenRenderNode(RSScreenRenderNode& node) override {}
    void QuickPrepareLogicalDisplayRenderNode(RSLogicalDisplayRenderNode& node) override {}
    void QuickPrepareSurfaceRenderNode(RSSurfaceRenderNode& node) override {}
    void QuickPrepareCanvasRenderNode(RSCanvasRenderNode& node) override {}
    void QuickPrepareEffectRenderNode(RSEffectRenderNode& node) override {}
    void QuickPrepareChildren(RSRenderNode& node) override {}

    void PrepareChildren(RSRenderNode& node) override {}
    void PrepareCanvasRenderNode(RSCanvasRenderNode& node) override {}
    void PrepareScreenRenderNode(RSScreenRenderNode& node) override {}
    void PrepareProxyRenderNode(RSProxyRenderNode& node) override {}
    void PrepareRootRenderNode(RSRootRenderNode& node) override {}
    void PrepareSurfaceRenderNode(RSSurfaceRenderNode& node) override {}
    void PrepareEffectRenderNode(RSEffectRenderNode& node) override {}
    void PrepareLogicalDisplayRenderNode(RSLogicalDisplayRenderNode& node) override {}

    void ProcessChildren(RSRenderNode& node) override {}
    void ProcessCanvasRenderNode(RSCanvasRenderNode& node) override {}
    void ProcessScreenRenderNode(RSScreenRenderNode& node) override {}
    void ProcessProxyRenderNode(RSProxyRenderNode& node) override {}
    void ProcessRootRenderNode(RSRootRenderNode& node) override {}
    void ProcessSurfaceRenderNode(RSSurfaceRenderNode& node) override {}
    void ProcessEffectRenderNode(RSEffectRenderNode& node) override {}
    void ProcessLogicalDisplayRenderNode(RSLogicalDisplayRenderNode& node) override {}
};

/**
 * @tc.name: UpdateAbilityNodeIds
 * @tc.desc: test results of UpdateAbilityNodeIds
 * @tc.type: FUNC
 * @tc.require: issueI9L0VL
 */
HWTEST_F(RSSurfaceRenderNodeThreeTest, UpdateAbilityNodeIds, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    node->UpdateAbilityNodeIds(1, true);
    EXPECT_TRUE(node->GetAbilityNodeIds().count(1) == 1);
    std::unordered_set<NodeId> nodeIds;
    node->AddAbilityComponentNodeIds(nodeIds);
    node->ResetAbilityNodeIds();
    node->UpdateAbilityNodeIds(1, false);
    EXPECT_TRUE(node->GetAbilityNodeIds().empty());
}

/**
 * @tc.name: UpdateChildHardwareEnabledNode
 * @tc.desc: test results of UpdateChildHardwareEnabledNode
 * @tc.type: FUNC
 * @tc.require: issueI9L0VL
 */
HWTEST_F(RSSurfaceRenderNodeThreeTest, UpdateChildHardwareEnabledNode, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    node->UpdateChildHardwareEnabledNode(1, false);
    node->UpdateChildHardwareEnabledNode(1, true);
    ASSERT_TRUE(node->GetNeedCollectHwcNode());
}

/**
 * @tc.name: SetHwcChildrenDisabledState
 * @tc.desc: test results of SetHwcChildrenDisabledState
 * @tc.type: FUNC
 * @tc.require: issueI9L0VL
 */
HWTEST_F(RSSurfaceRenderNodeThreeTest, SetHwcChildrenDisabledState, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    node->SetHwcChildrenDisabledState();
    ASSERT_TRUE(node->childHardwareEnabledNodes_.size() == 0);
}

/**
 * @tc.name: OnApplyModifiers
 * @tc.desc: test results of OnApplyModifiers
 * @tc.type: FUNC
 * @tc.require: issueI9L0VL
 */
HWTEST_F(RSSurfaceRenderNodeThreeTest, OnApplyModifiers, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    node->OnApplyModifiers();
    auto& properties = node->GetMutableRenderProperties();
    ASSERT_EQ(properties.GetAlpha(), properties.GetAlpha() * node->contextAlpha_);
    node->shouldPaint_ = false;
    node->OnApplyModifiers();
    ASSERT_EQ(properties.GetAlpha(), properties.GetAlpha() * node->contextAlpha_);
    node->shouldPaint_ = true;
    node->OnApplyModifiers();
    ASSERT_EQ(properties.GetAlpha(), properties.GetAlpha() * node->contextAlpha_);
}

/**
 * @tc.name: LeashWindowRelatedAppWindowOccluded
 * @tc.desc: test results of LeashWindowRelatedAppWindowOccluded
 * @tc.type: FUNC
 * @tc.require: issueI9L0VL
 */
HWTEST_F(RSSurfaceRenderNodeThreeTest, LeashWindowRelatedAppWindowOccluded, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    std::vector<std::shared_ptr<RSSurfaceRenderNode>> appNode;
    ASSERT_FALSE(node->LeashWindowRelatedAppWindowOccluded(appNode));
}

/**
 * @tc.name: GetLeashWindowNestedSurfaces
 * @tc.desc: test results of GetLeashWindowNestedSurfaces
 * @tc.type: FUNC
 * @tc.require: issueI9L0VL
 */
HWTEST_F(RSSurfaceRenderNodeThreeTest, GetLeashWindowNestedSurfaces, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    ASSERT_EQ(node->GetLeashWindowNestedSurfaces().size(), 0);
    node->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    ASSERT_EQ(node->GetLeashWindowNestedSurfaces().size(), 0);
    auto child1 = std::make_shared<RSSurfaceRenderNode>(id + 1);
    auto child2 = std::make_shared<RSRenderNode>(id + 2);
    std::vector<std::shared_ptr<RSRenderNode>> children;
    children.push_back(child1);
    children.push_back(child2);
    node->fullChildrenList_ = std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>(children);
    ASSERT_EQ(node->GetLeashWindowNestedSurfaces().size(), 1);
}

/**
 * @tc.name: IsUIFirstSelfDrawCheck
 * @tc.desc: test results of IsUIFirstSelfDrawCheck
 * @tc.type: FUNC
 * @tc.require: issueI9L0VL
 */
HWTEST_F(RSSurfaceRenderNodeThreeTest, IsUIFirstSelfDrawCheck, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    ASSERT_TRUE(node->IsUIFirstSelfDrawCheck());
}

/**
 * @tc.name: UpdateCacheSurfaceDirtyManager
 * @tc.desc: test results of UpdateCacheSurfaceDirtyManager
 * @tc.type: FUNC
 * @tc.require: issueI9L0VL
 */
HWTEST_F(RSSurfaceRenderNodeThreeTest, UpdateCacheSurfaceDirtyManager, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    int bufferAge = 1;
    node->UpdateCacheSurfaceDirtyManager(bufferAge);
    ASSERT_NE(node->cacheSurfaceDirtyManager_, nullptr);
}

/**
 * @tc.name: SetIsOnTheTree
 * @tc.desc: test results of SetIsOnTheTree
 * @tc.type: FUNC
 * @tc.require: issueI9L0VL
 */
HWTEST_F(RSSurfaceRenderNodeThreeTest, SetIsOnTheTree, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    NodeId instanceRootNodeId = 1;
    NodeId firstLevelNodeId = 1;
    NodeId cacheNodeId = 1;
    bool flag = false;
    node->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    node->SetIsOnTheTree(flag, instanceRootNodeId, firstLevelNodeId, cacheNodeId);
    ASSERT_TRUE(node->IsLeashWindow());
    node->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    node->SetIsOnTheTree(flag, instanceRootNodeId, firstLevelNodeId, cacheNodeId);
    ASSERT_TRUE(node->IsAppWindow());
    auto parent = std::make_shared<RSSurfaceRenderNode>(id + 1);
    node->SetParent(parent);
    parent->firstLevelNodeId_ = id + 1;
    node->SetIsOnTheTree(flag, instanceRootNodeId, firstLevelNodeId, cacheNodeId);
    ASSERT_NE(parent->GetFirstLevelNodeId(), INVALID_NODEID);
    parent->firstLevelNodeId_ = 0;
    node->SetIsOnTheTree(flag, instanceRootNodeId, firstLevelNodeId, cacheNodeId);
    ASSERT_EQ(parent->GetFirstLevelNodeId(), INVALID_NODEID);
    node->nodeType_ = RSSurfaceNodeType::UI_EXTENSION_COMMON_NODE;
    flag = true;
    node->SetIsOnTheTree(flag, instanceRootNodeId, firstLevelNodeId, cacheNodeId);
    ASSERT_TRUE(node->IsUIExtension());
    flag = false;
    parent = nullptr;
    node->SetParent(parent);
    node->SetIsOnTheTree(flag, instanceRootNodeId, firstLevelNodeId, cacheNodeId);
    ASSERT_TRUE(node->IsUIExtension());
    node->nodeType_ = RSSurfaceNodeType::DEFAULT;
    node->SetIsOnTheTree(flag, instanceRootNodeId, firstLevelNodeId, cacheNodeId);
    ASSERT_FALSE(node->IsUIExtension());
}

/**
 * @tc.name: HasOnlyOneRootNode
 * @tc.desc: test results of HasOnlyOneRootNode
 * @tc.type: FUNC
 * @tc.require: issueI9L0VL
 */
HWTEST_F(RSSurfaceRenderNodeThreeTest, HasOnlyOneRootNode, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    ASSERT_FALSE(node->HasOnlyOneRootNode());
    auto child = std::make_shared<RSSurfaceRenderNode>(id + 1);
    node->children_.emplace_back(child);
    ASSERT_FALSE(node->HasOnlyOneRootNode());
}

/**
 * @tc.name: GetNodeIsSingleFrameComposer
 * @tc.desc: test results of GetNodeIsSingleFrameComposer
 * @tc.type: FUNC
 * @tc.require: issueI9L0VL
 */
HWTEST_F(RSSurfaceRenderNodeThreeTest, GetNodeIsSingleFrameComposer, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    bool res = node->GetNodeIsSingleFrameComposer();
    ASSERT_FALSE(res);
}

/**
 * @tc.name: QuerySubAssignable
 * @tc.desc: test results of QuerySubAssignable
 * @tc.type: FUNC
 * @tc.require: issueI9L0VL
 */
HWTEST_F(RSSurfaceRenderNodeThreeTest, QuerySubAssignable, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    bool isRotation = true;
    bool res = node->QuerySubAssignable(isRotation);
    ASSERT_EQ(res, RSSystemProperties::GetCacheOptimizeRotateEnable());
}

/**
 * @tc.name: GetGravityTranslate
 * @tc.desc: test results of GetGravityTranslate
 * @tc.type: FUNC
 * @tc.require: issueI9L0VL
 */
HWTEST_F(RSSurfaceRenderNodeThreeTest, GetGravityTranslate, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    float imgWidth = 1.0f;
    float imgHeight = 1.0f;
    node->GetGravityTranslate(imgWidth, imgHeight);
    ASSERT_FALSE(node->IsLeashWindow());
}

/**
 * @tc.name: SetOcclusionVisible
 * @tc.desc: test results of SetOcclusionVisible
 * @tc.type: FUNC
 * @tc.require: issueI9L0VL
 */
HWTEST_F(RSSurfaceRenderNodeThreeTest, SetOcclusionVisible, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    bool visible = true;
    node->SetOcclusionVisible(visible);
    node->InitRenderParams();
    node->addedToPendingSyncList_ = true;
    node->SetOcclusionVisible(visible);
    ASSERT_TRUE(node->isOcclusionVisible_);
}

/**
 * @tc.name: UpdatePartialRenderParams
 * @tc.desc: test results of UpdatePartialRenderParams
 * @tc.type: FUNC
 * @tc.require: issueI9L0VL
 */
HWTEST_F(RSSurfaceRenderNodeThreeTest, UpdatePartialRenderParams, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    node->UpdatePartialRenderParams();
    node->UpdateRenderParams();
    node->UpdateAncestorScreenNodeInRenderParams();
    node->SetUifirstChildrenDirtyRectParam(RectI());
    node->SetUifirstNodeEnableParam(MultiThreadCacheType::NONE);
    node->SetIsParentUifirstNodeEnableParam(true);
    ASSERT_EQ(node->stagingRenderParams_.get(), nullptr);

    node->stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(id);
    node->UpdatePartialRenderParams();
    node->SetSurfaceNodeType(RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE);
    node->UpdatePartialRenderParams();
    node->SetSurfaceNodeType(RSSurfaceNodeType::SELF_DRAWING_NODE);
    node->UpdatePartialRenderParams();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: InitRenderParams
 * @tc.desc: test results of InitRenderParams
 * @tc.type: FUNC
 * @tc.require: issueI9L0VL
 */
HWTEST_F(RSSurfaceRenderNodeThreeTest, InitRenderParams, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    node->InitRenderParams();
    ASSERT_NE(node->stagingRenderParams_.get(), nullptr);
}

/**
 * @tc.name: GetHasTransparentSurface
 * @tc.desc: test results of GetHasTransparentSurface
 * @tc.type: FUNC
 * @tc.require: issueI9L0VL
 */
HWTEST_F(RSSurfaceRenderNodeThreeTest, GetHasTransparentSurface, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    node->SetHasSharedTransitionNode(true);
    ASSERT_TRUE(node->GetHasSharedTransitionNode());
    ASSERT_FALSE(node->GetHasTransparentSurface());
}

/**
 * @tc.name: GetCacheSurfaceProcessedStatus
 * @tc.desc: test results of GetCacheSurfaceProcessedStatus
 * @tc.type: FUNC
 * @tc.require: issueI9L0VL
 */
HWTEST_F(RSSurfaceRenderNodeThreeTest, GetCacheSurfaceProcessedStatus, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    CacheProcessStatus cacheProcessStatus = CacheProcessStatus::DOING;
    node->SetCacheSurfaceProcessedStatus(cacheProcessStatus);
    ASSERT_EQ(node->GetCacheSurfaceProcessedStatus(), cacheProcessStatus);
}

/**
 * @tc.name: GetLocalZOrder
 * @tc.desc: test results of GetLocalZOrder
 * @tc.type: FUNC
 * @tc.require: issueI9L0VL
 */
HWTEST_F(RSSurfaceRenderNodeThreeTest, GetLocalZOrder, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    float localZOrder = 1.0f;
    node->SetLocalZOrder(localZOrder);
    ASSERT_EQ(node->GetLocalZOrder(), localZOrder);
}

/**
 * @tc.name: GetChildHardwareEnabledNodes
 * @tc.desc: test results of GetChildHardwareEnabledNodes
 * @tc.type: FUNC
 * @tc.require: issueI9L0VL
 */
HWTEST_F(RSSurfaceRenderNodeThreeTest, GetChildHardwareEnabledNodes, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    std::weak_ptr<RSSurfaceRenderNode> childNode = std::make_shared<RSSurfaceRenderNode>(id + 1);
    node->AddChildHardwareEnabledNode(childNode);
    node->ResetChildHardwareEnabledNodes();
    ASSERT_EQ(node->GetChildHardwareEnabledNodes().size(), 0);
}

/**
 * @tc.name: UpdateSurfaceCacheContentStatic
 * @tc.desc: test results of UpdateSurfaceCacheContentStatic
 * @tc.type: FUNC
 * @tc.require: issueI9L0VL
 */
HWTEST_F(RSSurfaceRenderNodeThreeTest, UpdateSurfaceCacheContentStatic001, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    node->UpdateSurfaceCacheContentStatic();
    ASSERT_EQ(node->dirtyContentNodeNum_, 0);
}

/**
 * @tc.name: GetChildrenNeedFilterRectsCacheValid
 * @tc.desc: test results of GetChildrenNeedFilterRectsCacheValid
 * @tc.type: FUNC
 * @tc.require: issueI9L0VL
 */
HWTEST_F(RSSurfaceRenderNodeThreeTest, GetChildrenNeedFilterRectsCacheValid, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    ASSERT_EQ(node->GetChildrenFilterNodes().size(), 0);
    ASSERT_EQ(node->GetChildrenNeedFilterRectsCacheValid().size(), 0);
}

/**
 * @tc.name: CheckOpaqueRegionBaseInfo
 * @tc.desc: test results of CheckOpaqueRegionBaseInfo
 * @tc.type: FUNC
 * @tc.require: issueI9L0VL
 */
HWTEST_F(RSSurfaceRenderNodeThreeTest, CheckOpaqueRegionBaseInfo, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    node->InitRenderParams();
    RectI screeninfo;
    RectI absRect;
    ScreenRotation screenRotation = ScreenRotation::ROTATION_0;
    bool isFocusWindow = true;
    Vector4<int> cornerRadius;
    ASSERT_FALSE(node->CheckOpaqueRegionBaseInfo(screeninfo, absRect, screenRotation, isFocusWindow, cornerRadius));
    bool hasContainer = true;
    node->containerConfig_.Update(hasContainer, rrect);
    node->stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(id + 1);
    node->addedToPendingSyncList_ = true;
    node->isHardwareForcedDisabled_ = true;
    node->UpdateHardwareDisabledState(true);
    ASSERT_FALSE(node->opaqueRegionBaseInfo_.hasContainerWindow_);
    node->UpdateOccludedByFilterCache(false);
    ASSERT_FALSE(node->IsOccludedByFilterCache());
    ASSERT_FALSE(node->CheckIfOcclusionChanged());
}

/**
 * @tc.name: NeedSetCallbackForRenderThreadRefresh
 * @tc.desc: test results of NeedSetCallbackForRenderThreadRefresh
 * @tc.type: FUNC
 * @tc.require: issueI9L0VL
 */
HWTEST_F(RSSurfaceRenderNodeThreeTest, NeedSetCallbackForRenderThreadRefresh, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    node->SetStartAnimationFinished();
    ASSERT_TRUE(node->IsStartAnimationFinished());

    node->SetCallbackForRenderThreadRefresh(true);
    ASSERT_FALSE(node->NeedSetCallbackForRenderThreadRefresh());
}

/**
 * @tc.name: GetHasPrivacyContentLayer001
 * @tc.desc: Test GetHasPrivacyContentLayer when SetProtectedLayer is true.
 * @tc.type: FUNC
 * @tc.require: issueI7ZSC2
 */
HWTEST_F(RSSurfaceRenderNodeThreeTest, GetHasPrivacyContentLayer001, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSSurfaceRenderNode>(id, rsContext);
    ASSERT_NE(node, nullptr);

    node->SetHidePrivacyContent(true);
    bool result = node->GetHasPrivacyContentLayer();
    ASSERT_EQ(result, true);
}

/**
 * @tc.name: GetHasPrivacyContentLayer002
 * @tc.desc: Test GetHasPrivacyContentLayer when SetProtectedLayer is false.
 * @tc.type: FUNC
 * @tc.require: issueI7ZSC2
 */
HWTEST_F(RSSurfaceRenderNodeThreeTest, GetHasPrivacyContentLayer002, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSSurfaceRenderNode>(id, rsContext);
    ASSERT_NE(node, nullptr);

    node->SetHidePrivacyContent(false);
    bool result = node->GetHasPrivacyContentLayer();
    ASSERT_EQ(result, false);
}

/**
 * @tc.name: SetDoDirectComposition001
 * @tc.desc: Test SetDoDirectComposition
 * @tc.type: FUNC
 * @tc.require: issueI9Q8E9
 */
HWTEST_F(RSSurfaceRenderNodeThreeTest, SetDoDirectComposition001, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSSurfaceRenderNode>(id, rsContext);
    ASSERT_NE(node, nullptr);

    node->SetDoDirectComposition(false);
    ASSERT_EQ(node->GetDoDirectComposition(), false);

    node->SetDoDirectComposition(true);
    ASSERT_EQ(node->GetDoDirectComposition(), true);
}

/**
 * @tc.name: SetSkipDraw001
 * @tc.desc: Test function SetSkipDraw
 * @tc.type: FUNC
 * @tc.require: issueI9U6LX
 */
HWTEST_F(RSSurfaceRenderNodeThreeTest, SetSkipDraw001, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto node = std::make_shared<RSSurfaceRenderNode>(id, rsContext);
    ASSERT_NE(node, nullptr);

    node->SetSkipDraw(true);
    ASSERT_TRUE(node->GetSkipDraw());

    node->SetSkipDraw(false);
    ASSERT_FALSE(node->GetSkipDraw());
}

/**
 * @tc.name: SetSkipDraw001
 * @tc.desc: Test function SetSkipDraw
 * @tc.type: FUNC
 * @tc.require: issueI9U6LX
 */
HWTEST_F(RSSurfaceRenderNodeThreeTest, GetSkipDraw001, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto node = std::make_shared<RSSurfaceRenderNode>(id, rsContext);
    ASSERT_NE(node, nullptr);

    ASSERT_FALSE(node->GetSkipDraw());
}

/**
 * @tc.name: RotateCorner001
 * @tc.desc: test results of RotateCorner
 * @tc.type:FUNC
 * @tc.require:issueIAIAQ0
 */
HWTEST_F(RSSurfaceRenderNodeThreeTest, RotateCorner001, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSSurfaceRenderNode>(id, rsContext);
    constexpr int firstCornerRadius{1};
    constexpr int secondCornerRadius{2};
    constexpr int thirdCornerRadius{3};
    constexpr int fourthCornerRadius{4};

    Vector4<int> cornerRadius1{firstCornerRadius, secondCornerRadius, thirdCornerRadius, fourthCornerRadius};
    node->RotateCorner(RS_ROTATION_0, cornerRadius1);
    EXPECT_TRUE(cornerRadius1 == Vector4<int>(
        firstCornerRadius, secondCornerRadius, thirdCornerRadius, fourthCornerRadius));
    
    Vector4<int> cornerRadius2{firstCornerRadius, secondCornerRadius, thirdCornerRadius, fourthCornerRadius};
    node->RotateCorner(RS_ROTATION_90, cornerRadius2);
    EXPECT_TRUE(cornerRadius2 == Vector4<int>(
        fourthCornerRadius, firstCornerRadius, secondCornerRadius, thirdCornerRadius));
    
    Vector4<int> cornerRadius3{firstCornerRadius, secondCornerRadius, thirdCornerRadius, fourthCornerRadius};
    node->RotateCorner(RS_ROTATION_180, cornerRadius3);
    EXPECT_TRUE(cornerRadius3 == Vector4<int>(
        thirdCornerRadius, fourthCornerRadius, firstCornerRadius, secondCornerRadius));
    
    Vector4<int> cornerRadius4{firstCornerRadius, secondCornerRadius, thirdCornerRadius, fourthCornerRadius};
    node->RotateCorner(RS_ROTATION_270, cornerRadius4);
    EXPECT_TRUE(cornerRadius4 == Vector4<int>(
        secondCornerRadius, thirdCornerRadius, fourthCornerRadius, firstCornerRadius));
}

/**
 * @tc.name: GetAncoForceDoDirect001
 * @tc.desc: Test function GetAncoForceDoDirect
 * @tc.type: FUNC
 * @tc.require: issueIAIIEP
 */
HWTEST_F(RSSurfaceRenderNodeThreeTest, GetAncoForceDoDirect001, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    ASSERT_NE(rsContext, nullptr);
    auto node = std::make_shared<RSSurfaceRenderNode>(id, rsContext);
    ASSERT_NE(node, nullptr);
 
    RSSurfaceRenderNode::SetAncoForceDoDirect(false);
    node->SetAncoFlags(1);
    ASSERT_FALSE(node->GetAncoForceDoDirect());
    ASSERT_EQ(node->GetAncoFlags(), 1);
    node->SetAncoFlags(0);
    ASSERT_FALSE(node->GetAncoForceDoDirect());
    ASSERT_EQ(node->GetAncoFlags(), 0);
 
    RSSurfaceRenderNode::SetAncoForceDoDirect(true);
    node->SetAncoFlags(1);
    ASSERT_TRUE(node->GetAncoForceDoDirect());
    ASSERT_EQ(node->GetAncoFlags(), 1);
    node->SetAncoFlags(0);
    ASSERT_FALSE(node->GetAncoForceDoDirect());
    ASSERT_EQ(node->GetAncoFlags(), 0);
}

/**
 * @tc.name: SetNeedOffscreen
 * @tc.desc: test results of SetNeedOffscreen
 * @tc.type:FUNC SetNeedOffscreen
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeThreeTest, SetNeedOffscreen, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSSurfaceRenderNode>(id, rsContext);
    node->SetNeedOffscreen(true);
    ASSERT_TRUE(true);
    node->stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(id);
    node->SetNeedOffscreen(true);
    ASSERT_NE(node->stagingRenderParams_.get(), nullptr);
}

/**
 * @tc.name: ShouldPrepareSubnodes
 * @tc.desc: test results of ShouldPrepareSubnodes
 * @tc.type:FUNC ShouldPrepareSubnodes
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeThreeTest, ShouldPrepareSubnodes, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSSurfaceRenderNode>(id, rsContext);
    node->SetSurfaceNodeType(RSSurfaceNodeType::DEFAULT);
    ASSERT_TRUE(node->ShouldPrepareSubnodes());
    node->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    ASSERT_FALSE(node->ShouldPrepareSubnodes());
    node->SetSurfaceNodeType(RSSurfaceNodeType::ABILITY_COMPONENT_NODE);
    ASSERT_FALSE(node->ShouldPrepareSubnodes());
    node->SetDstRect({0, 0, 10, 10});
    node->SetSurfaceNodeType(RSSurfaceNodeType::DEFAULT);
    ASSERT_TRUE(node->ShouldPrepareSubnodes());
    node->SetSurfaceNodeType(RSSurfaceNodeType::APP_WINDOW_NODE);
    ASSERT_TRUE(node->ShouldPrepareSubnodes());
    node->SetSurfaceNodeType(RSSurfaceNodeType::ABILITY_COMPONENT_NODE);
    ASSERT_TRUE(node->ShouldPrepareSubnodes());
}

/**
 * @tc.name: DirtyRegionDump
 * @tc.desc: test results of DirtyRegionDump
 * @tc.type:FUNC DirtyRegionDump
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeThreeTest, DirtyRegionDump, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSSurfaceRenderNode>(id, rsContext);
    ASSERT_NE(node->DirtyRegionDump(), "");
    node->dirtyManager_ = std::make_shared<RSDirtyRegionManager>();
    ASSERT_NE(node->DirtyRegionDump(), "");
}

/**
 * @tc.name: OnSubSurfaceChanged
 * @tc.desc: test results of OnSubSurfaceChanged
 * @tc.type:FUNC OnSubSurfaceChanged
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeThreeTest, OnSubSurfaceChanged, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSSurfaceRenderNode>(id + 10, rsContext);
    auto parent = std::make_shared<RSRenderNode>(id);
    node->parent_ = parent;
    node->OnSubSurfaceChanged();
    auto parentTwo = std::make_shared<RSSurfaceRenderNode>(id + 1, rsContext);
    node->parent_ = parentTwo;
    parentTwo->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    node->OnSubSurfaceChanged();
    parentTwo->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE;
    node->OnSubSurfaceChanged();
    parentTwo->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    node->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    node->OnSubSurfaceChanged();
    ASSERT_TRUE(true);
}

/**
 * @tc.name: GetAllSubSurfaceNodes
 * @tc.desc: test results of GetAllSubSurfaceNodes
 * @tc.type:FUNC GetAllSubSurfaceNodes
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeThreeTest, GetAllSubSurfaceNodes, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSSurfaceRenderNode>(id, rsContext);
    auto subNode = std::make_shared<RSSurfaceRenderNode>(id + 1, rsContext);
    std::vector<std::pair<NodeId, RSSurfaceRenderNode::WeakPtr>> allSubNodes;
    node->GetAllSubSurfaceNodes(allSubNodes);
    ASSERT_TRUE(allSubNodes.empty());
    node->childSubSurfaceNodes_[id + 1] = subNode;
    node->GetAllSubSurfaceNodes(allSubNodes);
    ASSERT_FALSE(subNode->HasSubSurfaceNodes());
    ASSERT_FALSE(allSubNodes.empty());
    allSubNodes.clear();
    auto subNodeTwo = std::make_shared<RSSurfaceRenderNode>(id + 2, rsContext);
    subNode->childSubSurfaceNodes_[subNodeTwo->GetId()] = subNodeTwo;
    node->GetAllSubSurfaceNodes(allSubNodes);
    ASSERT_TRUE(subNode->HasSubSurfaceNodes());
    ASSERT_FALSE(allSubNodes.empty());
}

/**
 * @tc.name: OnResetParent
 * @tc.desc: test results of OnResetParent
 * @tc.type:FUNC OnResetParent
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeThreeTest, OnResetParent, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSSurfaceRenderNode>(id, rsContext);
    node->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    node->OnResetParent();
    node->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE;
    node->surfaceHandler_ = std::make_shared<RSSurfaceHandler>(id);
    node->OnResetParent();
    ASSERT_EQ(node->GetRSSurfaceHandler()->GetConsumer(), nullptr);
    node->surfaceHandler_->SetConsumer(IConsumerSurface::Create());
    node->OnResetParent();
    ASSERT_NE(node->GetRSSurfaceHandler()->GetConsumer(), nullptr);
    node->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_NODE;
    node->OnResetParent();
    ASSERT_NE(node->GetRSSurfaceHandler()->GetConsumer(), nullptr);
    ASSERT_TRUE(node->IsSelfDrawingType());
    ASSERT_FALSE(node->IsAbilityComponent());
    node->nodeType_ = RSSurfaceNodeType::ABILITY_COMPONENT_NODE;
    node->OnResetParent();
    ASSERT_NE(node->GetRSSurfaceHandler()->GetConsumer(), nullptr);
    ASSERT_FALSE(node->IsSelfDrawingType());
    ASSERT_TRUE(node->IsAbilityComponent());
    node->nodeType_ = RSSurfaceNodeType::DEFAULT;
    node->OnResetParent();
    ASSERT_NE(node->GetRSSurfaceHandler()->GetConsumer(), nullptr);
    ASSERT_FALSE(node->IsSelfDrawingType());
    ASSERT_FALSE(node->IsAbilityComponent());
}

/**
 * @tc.name: QuickPrepare
 * @tc.desc: test results of QuickPrepare
 * @tc.type:FUNC QuickPrepare
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeThreeTest, QuickPrepare, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSSurfaceRenderNode>(id, rsContext);
    std::shared_ptr<RSNodeVisitor> visitor = nullptr;
    node->QuickPrepare(visitor);
    visitor = std::make_shared<RSTestVisitor>();
    node->QuickPrepare(visitor);
    node->nodeType_ = RSSurfaceNodeType::APP_WINDOW_NODE;
    ASSERT_TRUE(node->IsAppWindow());
    node->QuickPrepare(visitor);
    node->isNotifyUIBufferAvailable_ = true;
    node->isWaitUifirstFirstFrame_ = true;
    ASSERT_TRUE(node->IsNotifyUIBufferAvailable());
    node->QuickPrepare(visitor);
    node->isNotifyUIBufferAvailable_ = false;
    ASSERT_FALSE(node->IsNotifyUIBufferAvailable());
    node->QuickPrepare(visitor);
    node->nodeType_ = RSSurfaceNodeType::SCB_SCREEN_NODE;
    ASSERT_TRUE(node->IsScbScreen());
    node->QuickPrepare(visitor);
    node->isNotifyUIBufferAvailable_ = true;
    ASSERT_TRUE(node->IsNotifyUIBufferAvailable());
    node->QuickPrepare(visitor);
    node->isNotifyUIBufferAvailable_ = false;
    ASSERT_FALSE(node->IsNotifyUIBufferAvailable());
    node->QuickPrepare(visitor);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: QuickPrepare001
 * @tc.desc: test results of QuickPrepare
 * @tc.type:FUNC QuickPrepare
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeThreeTest, QuickPrepare001, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSSurfaceRenderNode>(id, rsContext);
    std::shared_ptr<RSNodeVisitor> visitor = nullptr;
    node->QuickPrepare(visitor);
    visitor = std::make_shared<RSTestVisitor>();
    node->QuickPrepare(visitor);
    node->nodeType_ = RSSurfaceNodeType::UI_EXTENSION_COMMON_NODE;
    ASSERT_TRUE(node->IsUIExtension());
    node->QuickPrepare(visitor);
    node->isNotifyUIBufferAvailable_ = true;
    node->isWaitUifirstFirstFrame_ = true;
    ASSERT_TRUE(node->IsNotifyUIBufferAvailable());
    node->QuickPrepare(visitor);
    node->isNotifyUIBufferAvailable_ = false;
    ASSERT_FALSE(node->IsNotifyUIBufferAvailable());
    node->QuickPrepare(visitor);
    node->nodeType_ = RSSurfaceNodeType::UI_EXTENSION_SECURE_NODE;
    ASSERT_TRUE(node->IsUIExtension());
    node->QuickPrepare(visitor);
    node->isNotifyUIBufferAvailable_ = true;
    node->isWaitUifirstFirstFrame_ = true;
    ASSERT_TRUE(node->IsNotifyUIBufferAvailable());
    node->QuickPrepare(visitor);
    node->isNotifyUIBufferAvailable_ = false;
    ASSERT_FALSE(node->IsNotifyUIBufferAvailable());
    node->QuickPrepare(visitor);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: Prepare
 * @tc.desc: test results of Prepare
 * @tc.type:FUNC Prepare
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeThreeTest, Prepare, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSSurfaceRenderNode>(id, rsContext);
    std::shared_ptr<RSNodeVisitor> visitor = nullptr;
    node->Prepare(visitor);
    visitor = std::make_shared<RSTestVisitor>();
    node->Prepare(visitor);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: Process
 * @tc.desc: test results of Process
 * @tc.type:FUNC Process
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeThreeTest, Process, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSSurfaceRenderNode>(id, rsContext);
    std::shared_ptr<RSNodeVisitor> visitor = nullptr;
    node->Process(visitor);
    visitor = std::make_shared<RSTestVisitor>();
    node->Process(visitor);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: IsSubTreeNeedPrepare
 * @tc.desc: test results of IsSubTreeNeedPrepare
 * @tc.type:FUNC IsSubTreeNeedPrepare
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeThreeTest, IsSubTreeNeedPrepare, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSSurfaceRenderNode>(id, rsContext);
    node->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    ASSERT_TRUE(node->IsSubTreeNeedPrepare(true, true));
    node->nodeType_ = RSSurfaceNodeType::DEFAULT;
    ASSERT_FALSE(node->IsSubTreeNeedPrepare(true, true));
    auto parent = std::make_shared<RSSurfaceRenderNode>(id + 1, rsContext);
    node->parent_ = parent;
    ASSERT_FALSE(node->IsSubTreeNeedPrepare(true, true));
    parent->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    ASSERT_FALSE(node->IsSubTreeNeedPrepare(true, true));
    auto& properties = node->GetMutableRenderProperties();
    properties.curGeoDirty_ = true;
    ASSERT_TRUE(node->IsSubTreeNeedPrepare(true, true));
}

/**
 * @tc.name: ProcessAnimatePropertyBeforeChildren
 * @tc.desc: test results of ProcessAnimatePropertyBeforeChildren
 * @tc.type:FUNC ProcessAnimatePropertyBeforeChildren
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeThreeTest, ProcessAnimatePropertyBeforeChildren, TestSize.Level2)
{
    auto rsContext = std::make_shared<RSContext>();
    auto node = std::make_shared<RSSurfaceRenderNode>(id, rsContext);
    node->stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(id);
    Drawing::Canvas canvas;
    RSPaintFilterCanvas filterCanvas(&canvas);
    node->needDrawAnimateProperty_ = false;
    node->ProcessAnimatePropertyBeforeChildren(filterCanvas, true);
    node->needDrawAnimateProperty_ = true;
    node->ProcessAnimatePropertyBeforeChildren(filterCanvas, true);
    node->cacheType_ = CacheType::ANIMATE_PROPERTY;
    node->needDrawAnimateProperty_ = false;
    node->ProcessAnimatePropertyBeforeChildren(filterCanvas, true);
    node->needDrawAnimateProperty_ = true;
    node->ProcessAnimatePropertyBeforeChildren(filterCanvas, true);
    auto& properties = node->GetMutableRenderProperties();
    ASSERT_TRUE(properties.GetCornerRadius().IsZero());
    properties.SetCornerRadius({1.f, 1.f, 1.f, 1.f});
    node->ProcessAnimatePropertyBeforeChildren(filterCanvas, true);
    ASSERT_FALSE(properties.GetCornerRadius().IsZero());
}

/**
 * @tc.name: SetHardCursorStatus
 * @tc.desc: SetHardCursorStatus and GetHardCursorStatus and GetHardCursorLastStatus test
 * @tc.type:FUNC
 * @tc.require: issueIAX2SN
 */
HWTEST_F(RSSurfaceRenderNodeThreeTest, SetHardCursorStatusTest, TestSize.Level1)
{
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    node->SetHardCursorStatus(true);
    EXPECT_EQ(node->GetHardCursorStatus(), true);
    EXPECT_EQ(node->GetHardCursorLastStatus(), false);
    node->SetHardCursorStatus(false);
    EXPECT_EQ(node->GetHardCursorStatus(), false);
    EXPECT_EQ(node->GetHardCursorLastStatus(), true);
}

/**
 * @tc.name: IsHardwareForcedDisabledTest001
 * @tc.desc: IsHardwareForcedDisabled test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeThreeTest, IsHardwareForcedDisabledTest001, TestSize.Level1)
{
    const std::string testBundleName = "com.example.tvplayer";
    RsCommonHook::Instance().SetTvPlayerBundleName(testBundleName);

    RSSurfaceRenderNodeConfig config{};
    config.id = ++id;
    config.nodeType = RSSurfaceNodeType::SELF_DRAWING_NODE;
    config.bundleName = testBundleName;

    auto node = std::make_shared<RSSurfaceRenderNode>(config);
    node->SetIsOnTheTree(true);
    node->SetGlobalAlpha(0.05f);
    EXPECT_EQ(node->IsHardwareForcedDisabled(), true);
}

/**
 * @tc.name: IsHardwareForcedDisabledTest002
 * @tc.desc: IsHardwareForcedDisabled test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSSurfaceRenderNodeThreeTest, IsHardwareForcedDisabledTest002, TestSize.Level1)
{
    const std::string testBundleName = "com.example.tvplayer";
    RsCommonHook::Instance().SetTvPlayerBundleName(testBundleName);

    RSSurfaceRenderNodeConfig config{};
    config.id = ++id;
    config.nodeType = RSSurfaceNodeType::SELF_DRAWING_NODE;
    config.bundleName = "com.example.others";

    auto node = std::make_shared<RSSurfaceRenderNode>(config);
    node->SetIsOnTheTree(true);
    node->SetGlobalAlpha(0.05f);
    EXPECT_EQ(node->IsHardwareForcedDisabled(), false);
}

/**
 * @tc.name: IsFilterUnderSurfaceForcedHwcTest
 * @tc.desc: IsFilterUnderSurfaceForcedHwc test
 * @tc.type: FUNC
 * @tc.require: issuesICKNNB
 */
HWTEST_F(RSSurfaceRenderNodeThreeTest, IsFilterUnderSurfaceForcedHwcTest, TestSize.Level1)
{
    const std::string collaborationBundleName = "com.example.devicecollaboration";

    RSSurfaceRenderNodeConfig config{};
    config.id = ++id;
    config.nodeType = RSSurfaceNodeType::SELF_DRAWING_NODE;
    config.bundleName = collaborationBundleName;
    auto node = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_FALSE(node->IsFilterUnderSurfaceForcedHwc());

    RsCommonHook::Instance().SetFilterUnderHwcConfigByApp(collaborationBundleName, "1");
    ASSERT_TRUE(node->IsFilterUnderSurfaceForcedHwc());
}
} // namespace Rosen
} // namespace OHOS