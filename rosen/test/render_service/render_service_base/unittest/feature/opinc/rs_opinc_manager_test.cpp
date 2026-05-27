/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "feature/opinc/rs_opinc_manager.h"
#include "gtest/gtest.h"
#include "params/rs_render_params.h"
#include "pipeline/rs_canvas_render_node.h"
#include "pipeline/rs_canvas_drawing_render_node.h"
#include "pipeline/rs_surface_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
constexpr int64_t CACHE_MEM = 100;
class RSOpincManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

    static inline RSOpincManager& opincManager_ = RSOpincManager::Instance();
};

void RSOpincManagerTest::SetUpTestCase() {}
void RSOpincManagerTest::TearDownTestCase() {}
void RSOpincManagerTest::SetUp() {}
void RSOpincManagerTest::TearDown() {}

/**
 * @tc.name: SetOPIncSwitch
 * @tc.desc: Verify the SetOPIncSwitch function
 * @tc.type: FUNC
 * @tc.require: #IBQETW
 */
HWTEST_F(RSOpincManagerTest, SetOPIncEnable, Function | SmallTest | Level1)
{
    opincManager_.SetOPIncSwitch(true);
    ASSERT_EQ(opincManager_.GetOPIncSwitch(), true);
}

/**
 * @tc.name: OpincGetNodeSupportFlag
 * @tc.desc: Verify the OpincGetNodeSupportFlag function
 * @tc.type: FUNC
 * @tc.require: #IBQETW
 */
HWTEST_F(RSOpincManagerTest, OpincGetNodeSupportFlag, Function | SmallTest | Level1)
{
    NodeId id = 0;
    auto rsSurfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(id);
    ASSERT_NE(rsSurfaceRenderNode, nullptr);
    ASSERT_FALSE(opincManager_.OpincGetNodeSupportFlag(*rsSurfaceRenderNode));

    auto rsCanvasRenderNode = std::make_shared<RSCanvasRenderNode>(++id);
    ASSERT_NE(rsCanvasRenderNode, nullptr);
    ASSERT_TRUE(opincManager_.OpincGetNodeSupportFlag(*rsCanvasRenderNode));

    auto rsCanvasDrawingRenderNode = std::make_shared<RSCanvasDrawingRenderNode>(++id);
    ASSERT_NE(rsCanvasDrawingRenderNode, nullptr);
    ASSERT_TRUE(opincManager_.OpincGetNodeSupportFlag(*rsCanvasDrawingRenderNode));
}

/**
 * @tc.name: OpincGetNodeSupportFlagCanvasDrawingSuggestNode
 * @tc.desc: Verify canvas-drawing branch disables support when node is already suggested opinc node
 * @tc.type: FUNC
 * @tc.require: issueRootCacheBranch
 */
HWTEST_F(RSOpincManagerTest, OpincGetNodeSupportFlagCanvasDrawingSuggestNode, Function | SmallTest | Level1)
{
    NodeId id = 100;
    auto node = std::make_shared<RSCanvasDrawingRenderNode>(id);
    ASSERT_NE(node, nullptr);

    node->MarkSuggestOpincNode(true, true);
    ASSERT_FALSE(opincManager_.OpincGetNodeSupportFlag(*node));
    ASSERT_FALSE(node->GetOpincCache().GetCurNodeTreeSupportFlag());

    node->MarkSuggestOpincNode(false, true);
    ASSERT_TRUE(opincManager_.OpincGetNodeSupportFlag(*node));
    ASSERT_TRUE(node->GetOpincCache().GetCurNodeTreeSupportFlag());
}

/**
 * @tc.name: OpincGetCanvasNodeSupportFlag
 * @tc.desc: Verify the OpincGetCanvasNodeSupportFlag function
 * @tc.type: FUNC
 * @tc.require: #IBQETW
 */
HWTEST_F(RSOpincManagerTest, OpincGetCanvasNodeSupportFlag, Function | SmallTest | Level1)
{
    NodeId id = 0;
    auto rsCanvasRenderNode = std::make_shared<RSCanvasRenderNode>(id);
    ASSERT_NE(rsCanvasRenderNode, nullptr);

    std::shared_ptr<RSRenderNode> inNode = std::make_shared<RSBaseRenderNode>(id + 1);
    std::shared_ptr<RSRenderNode> outNode = std::make_shared<RSBaseRenderNode>(id + 2);
    auto sharedTransitionParam = std::make_shared<SharedTransitionParam>(inNode, outNode, true);

    rsCanvasRenderNode->hasHdrPresent_ = true;
    ASSERT_TRUE(opincManager_.OpincGetCanvasNodeSupportFlag(*rsCanvasRenderNode));
    rsCanvasRenderNode->hasHdrPresent_ = false;
    ASSERT_TRUE(opincManager_.OpincGetCanvasNodeSupportFlag(*rsCanvasRenderNode));

    rsCanvasRenderNode->SetSharedTransitionParam(sharedTransitionParam);
    EXPECT_FALSE(opincManager_.OpincGetCanvasNodeSupportFlag(*rsCanvasRenderNode));
    rsCanvasRenderNode->SetSharedTransitionParam(nullptr);
    ASSERT_TRUE(opincManager_.OpincGetCanvasNodeSupportFlag(*rsCanvasRenderNode));

    auto& property = rsCanvasRenderNode->GetMutableRenderProperties();

    property.SetSpherize(1.0f);
    EXPECT_FALSE(opincManager_.OpincGetCanvasNodeSupportFlag(*rsCanvasRenderNode));
    property.SetSpherize(0.0f);
    ASSERT_TRUE(opincManager_.OpincGetCanvasNodeSupportFlag(*rsCanvasRenderNode));

    property.GetEffect().isAttractionValid_ = true;
    EXPECT_FALSE(opincManager_.OpincGetCanvasNodeSupportFlag(*rsCanvasRenderNode));
    property.GetEffect().isAttractionValid_ = false;
    ASSERT_TRUE(opincManager_.OpincGetCanvasNodeSupportFlag(*rsCanvasRenderNode));

    property.needFilter_ = true;
    EXPECT_FALSE(opincManager_.OpincGetCanvasNodeSupportFlag(*rsCanvasRenderNode));
    property.needFilter_ = false;
    ASSERT_TRUE(opincManager_.OpincGetCanvasNodeSupportFlag(*rsCanvasRenderNode));

    property.GetEffect().useEffect_ = true;
    EXPECT_FALSE(opincManager_.OpincGetCanvasNodeSupportFlag(*rsCanvasRenderNode));
    property.GetEffect().useEffect_ = false;
    ASSERT_TRUE(opincManager_.OpincGetCanvasNodeSupportFlag(*rsCanvasRenderNode));

    Color color(255, 0, 0);
    std::optional<Color> colorBlend = color;
    property.SetColorBlend(colorBlend);
    EXPECT_FALSE(opincManager_.OpincGetCanvasNodeSupportFlag(*rsCanvasRenderNode));
    std::optional<Color> colorBlendEmpty;
    property.SetColorBlend(colorBlendEmpty);
    ASSERT_TRUE(opincManager_.OpincGetCanvasNodeSupportFlag(*rsCanvasRenderNode));

    rsCanvasRenderNode->childHasVisibleFilter_ = true;
    EXPECT_FALSE(opincManager_.OpincGetCanvasNodeSupportFlag(*rsCanvasRenderNode));
    rsCanvasRenderNode->childHasVisibleFilter_ = false;
    ASSERT_TRUE(opincManager_.OpincGetCanvasNodeSupportFlag(*rsCanvasRenderNode));

    rsCanvasRenderNode->childHasVisibleEffect_ = true;
    EXPECT_FALSE(opincManager_.OpincGetCanvasNodeSupportFlag(*rsCanvasRenderNode));
    rsCanvasRenderNode->childHasVisibleEffect_ = false;
    ASSERT_TRUE(opincManager_.OpincGetCanvasNodeSupportFlag(*rsCanvasRenderNode));
}

/**
 * @tc.name: OpincGetCanvasNodeSupportFlag1
 * @tc.desc: Verify the OpincGetCanvasNodeSupportFlag function
 * @tc.type: FUNC
 * @tc.require: #IBQETW
 */
HWTEST_F(RSOpincManagerTest, OpincGetCanvasNodeSupportFlag1, Function | SmallTest | Level1)
{
    NodeId id = 0;
    auto rsCanvasRenderNode = std::make_shared<RSCanvasRenderNode>(id);
    ASSERT_NE(rsCanvasRenderNode, nullptr);

    auto& property = rsCanvasRenderNode->GetMutableRenderProperties();

    property.hasHarmonium_ = true;
    EXPECT_FALSE(opincManager_.OpincGetCanvasNodeSupportFlag(*rsCanvasRenderNode));
    property.hasHarmonium_ = false;
    ASSERT_TRUE(opincManager_.OpincGetCanvasNodeSupportFlag(*rsCanvasRenderNode));
}

/**
 * @tc.name: OpincGetCanvasNodeSupportFlagSubTreeDisabled
 * @tc.desc: Verify canvas support follows subtree support flag when no other blocker exists
 * @tc.type: FUNC
 * @tc.require: issueRootCacheBranch
 */
HWTEST_F(RSOpincManagerTest, OpincGetCanvasNodeSupportFlagSubTreeDisabled, Function | SmallTest | Level1)
{
    NodeId id = 101;
    auto node = std::make_shared<RSCanvasRenderNode>(id);
    ASSERT_NE(node, nullptr);

    node->GetOpincCache().SetSubTreeSupportFlag(false);
    ASSERT_FALSE(opincManager_.OpincGetCanvasNodeSupportFlag(*node));
    node->GetOpincCache().SetSubTreeSupportFlag(true);
    ASSERT_TRUE(opincManager_.OpincGetCanvasNodeSupportFlag(*node));
}

 /**
 * @tc.name: IsOpincSubTreeDirty
 * @tc.desc: Verify the IsOpincSubTreeDirty function
 * @tc.type: FUNC
 * @tc.require: #IBQETW
 */
HWTEST_F(RSOpincManagerTest, IsOpincSubTreeDirty, Function | SmallTest | Level1)
{
    NodeId id = 0;
    auto rsCanvasRenderNode = std::make_shared<RSCanvasRenderNode>(id);
    ASSERT_NE(rsCanvasRenderNode, nullptr);
    rsCanvasRenderNode->MarkSuggestOpincNode(true, true);
    ASSERT_TRUE(opincManager_.IsOpincSubTreeDirty(*rsCanvasRenderNode, true));

    rsCanvasRenderNode->MarkSuggestOpincNode(false, true);
    ASSERT_FALSE(opincManager_.IsOpincSubTreeDirty(*rsCanvasRenderNode, true));
}

/**
 * @tc.name: QuickMarkStableNode
 * @tc.desc: Verify the QuickMarkStableNode function
 * @tc.type: FUNC
 * @tc.require: #ICTPO0
 */
HWTEST_F(RSOpincManagerTest, QuickMarkStableNode, Function | SmallTest | Level1)
{
    NodeId id = 0;
    auto rsCanvasRenderNode = std::make_shared<RSCanvasRenderNode>(id);
    ASSERT_NE(rsCanvasRenderNode, nullptr);
    auto& opincRootCache = rsCanvasRenderNode->GetOpincRootCache();
    bool unchangeMarkInApp = true;
    bool unchangeMarkEnable = false;
    opincRootCache.isSuggestOpincNode_ = true;

    opincManager_.QuickMarkStableNode(*rsCanvasRenderNode, unchangeMarkInApp, unchangeMarkEnable, false);
    ASSERT_TRUE(unchangeMarkEnable);

    unchangeMarkInApp = false;
    unchangeMarkEnable = false;
    opincManager_.QuickMarkStableNode(*rsCanvasRenderNode, unchangeMarkInApp, unchangeMarkEnable, false);
    ASSERT_FALSE(unchangeMarkEnable);

    unchangeMarkInApp = true;
    rsCanvasRenderNode->stagingRenderParams_ = std::make_unique<RSRenderParams>(id);
    opincManager_.QuickMarkStableNode(*rsCanvasRenderNode, unchangeMarkInApp, unchangeMarkEnable, false);
    ASSERT_TRUE(unchangeMarkEnable);

    unchangeMarkEnable = false;
    opincManager_.QuickMarkStableNode(*rsCanvasRenderNode, unchangeMarkInApp, unchangeMarkEnable, false);
    ASSERT_TRUE(unchangeMarkEnable);
}

/**
 * @tc.name: UpdateRootFlag
 * @tc.desc: Verify the UpdateRootFlag function
 * @tc.type: FUNC
 * @tc.require: #ICTPO0
 */
HWTEST_F(RSOpincManagerTest, UpdateRootFlag, Function | SmallTest | Level1)
{
    NodeId id = 0;
    auto rsCanvasRenderNode = std::make_shared<RSCanvasRenderNode>(id);
    ASSERT_NE(rsCanvasRenderNode, nullptr);
    auto& opincRootCache = rsCanvasRenderNode->GetOpincRootCache();
    bool unchangeMarkEnable = false;
    opincRootCache.isSuggestOpincNode_ = true;

    opincRootCache.isOpincRootFlag_ = false;
    opincManager_.UpdateRootFlag(*rsCanvasRenderNode, unchangeMarkEnable);
    ASSERT_FALSE(opincRootCache.isUnchangeMarkEnable_);

    opincRootCache.isOpincRootFlag_ = true;
    opincManager_.UpdateRootFlag(*rsCanvasRenderNode, unchangeMarkEnable);
    ASSERT_FALSE(opincRootCache.isUnchangeMarkEnable_);

    rsCanvasRenderNode->stagingRenderParams_ = std::make_unique<RSRenderParams>(id);
    opincRootCache.isOpincRootFlag_ = false;
    opincManager_.UpdateRootFlag(*rsCanvasRenderNode, unchangeMarkEnable);
    ASSERT_FALSE(rsCanvasRenderNode->stagingRenderParams_->isOpincRootFlag_);

    opincRootCache.isOpincRootFlag_ = true;
    opincManager_.UpdateRootFlag(*rsCanvasRenderNode, unchangeMarkEnable);
    ASSERT_TRUE(rsCanvasRenderNode->stagingRenderParams_->isOpincRootFlag_);
}

/**
 * @tc.name: QuickCheckOpincStable001
 * @tc.desc: Verify QuickCheckOpincStable
 * @tc.type: FUNC
 * @tc.require: issue23124
 */
HWTEST_F(RSOpincManagerTest, QuickCheckOpincStable001, TestSize.Level1)
{
    NodeId id = 1;
    auto canvasNode = std::make_shared<RSCanvasRenderNode>(id);
    ASSERT_NE(canvasNode, nullptr);
    auto preOPIncSwitch = opincManager_.GetOPIncSwitch();
    bool unchangeMarkInApp = true;
    bool unchangeMarkEnable = false;
    bool hasUnstableOpincNode = false;

    // GetOPIncSwitch() is false
    opincManager_.SetOPIncSwitch(false);
    EXPECT_FALSE(opincManager_.GetOPIncSwitch());
    opincManager_.QuickCheckOpincStable(*canvasNode, unchangeMarkInApp, unchangeMarkEnable, hasUnstableOpincNode);

    // GetOPIncSwitch() is true and HasUnstableOpincNode() is false
    opincManager_.SetOPIncSwitch(true);
    EXPECT_TRUE(opincManager_.GetOPIncSwitch());
    EXPECT_FALSE(canvasNode->GetOpincCache().HasUnstableOpincNode());
    opincManager_.QuickCheckOpincStable(*canvasNode, unchangeMarkInApp, unchangeMarkEnable, hasUnstableOpincNode);

    // GetOPIncSwitch() is true and HasUnstableOpincNode() is true
    canvasNode->GetOpincCache().SetHasUnstableOpincNode(true);
    EXPECT_TRUE(canvasNode->GetOpincCache().HasUnstableOpincNode());
    opincManager_.QuickCheckOpincStable(*canvasNode, unchangeMarkInApp, unchangeMarkEnable, hasUnstableOpincNode);
    EXPECT_EQ(canvasNode->TryGetOpincRootCachePtr(), nullptr);

    opincManager_.SetOPIncSwitch(preOPIncSwitch);
}

/**
 * @tc.name: QuickCheckOpincStable002
 * @tc.desc: Verify QuickCheckOpincStable
 * @tc.type: FUNC
 * @tc.require: issue23124
 */
HWTEST_F(RSOpincManagerTest, QuickCheckOpincStable002, TestSize.Level1)
{
    NodeId id = 1;
    auto parentNode = std::make_shared<RSSurfaceRenderNode>(id);
    auto childNode = std::make_shared<RSCanvasRenderNode>(id + 1);
    ASSERT_NE(parentNode, nullptr);
    ASSERT_NE(childNode, nullptr);
    auto preOPIncSwitch = opincManager_.GetOPIncSwitch();
    opincManager_.SetOPIncSwitch(true);

    auto fullChildrenList = std::make_shared<std::vector<std::shared_ptr<RSRenderNode>>>();
    fullChildrenList->push_back(childNode);
    parentNode->fullChildrenList_ = fullChildrenList;
    EXPECT_FALSE(parentNode->GetSortedChildren()->empty());

    bool unchangeMarkInApp = true;
    bool unchangeMarkEnable = false;
    bool hasUnstableOpincNode = false;
    parentNode->GetOpincCache().SetHasUnstableOpincNode(true);
    childNode->GetOpincCache().SetHasUnstableOpincNode(true);

    // node is not opinc node
    opincManager_.QuickCheckOpincStable(*parentNode, unchangeMarkInApp, unchangeMarkEnable, hasUnstableOpincNode);
    EXPECT_FALSE(parentNode->GetOpincCache().HasUnstableOpincNode());
    EXPECT_FALSE(childNode->GetOpincCache().HasUnstableOpincNode());

    // node is opinc node and unchanged
    parentNode->GetOpincCache().SetHasUnstableOpincNode(true);
    childNode->GetOpincCache().SetHasUnstableOpincNode(true);
    childNode->MarkSuggestOpincNode(true, true);
    childNode->GetOpincRootCache().nodeCacheState_ = NodeCacheState::STATE_UNCHANGE;
    opincManager_.QuickCheckOpincStable(*parentNode, unchangeMarkInApp, unchangeMarkEnable, hasUnstableOpincNode);
    EXPECT_FALSE(parentNode->GetOpincCache().HasUnstableOpincNode());
    EXPECT_FALSE(childNode->GetOpincCache().HasUnstableOpincNode());

    // node is opinc and changed
    parentNode->GetOpincCache().SetHasUnstableOpincNode(true);
    childNode->GetOpincCache().SetHasUnstableOpincNode(true);
    childNode->GetOpincRootCache().nodeCacheState_ = NodeCacheState::STATE_CHANGE;
    EXPECT_EQ(childNode->GetOpincRootCache().unchangeCount_, 0);
    opincManager_.QuickCheckOpincStable(*parentNode, unchangeMarkInApp, unchangeMarkEnable, hasUnstableOpincNode);
    EXPECT_EQ(childNode->GetOpincRootCache().unchangeCount_, 1);

    opincManager_.SetOPIncSwitch(preOPIncSwitch);
}

/**
 * @tc.name: QuickCheckOpincStableRootExprTrue
 * @tc.desc: Verify line 200 composite expression is true when node itself is suggest-opinc and changed
 * @tc.type: FUNC
 * @tc.require: issueLine200
 */
HWTEST_F(RSOpincManagerTest, QuickCheckOpincStableRootExprTrue, TestSize.Level1)
{
    constexpr NodeId ROOT_NODE_ID = 200;
    auto rootNode = std::make_shared<RSCanvasRenderNode>(ROOT_NODE_ID);
    ASSERT_NE(rootNode, nullptr);

    auto preOPIncSwitch = opincManager_.GetOPIncSwitch();
    opincManager_.SetOPIncSwitch(true);

    rootNode->GetOpincCache().SetHasUnstableOpincNode(true);
    rootNode->MarkSuggestOpincNode(true, true);
    rootNode->GetOpincRootCache().nodeCacheState_ = NodeCacheState::STATE_CHANGE;

    bool unchangeMarkInApp = true;
    bool unchangeMarkEnable = false;
    bool hasUnstableOpincNode = false;
    opincManager_.QuickCheckOpincStable(*rootNode, unchangeMarkInApp, unchangeMarkEnable, hasUnstableOpincNode);

    ASSERT_TRUE(rootNode->GetOpincCache().HasUnstableOpincNode());
    ASSERT_TRUE(hasUnstableOpincNode);
    opincManager_.SetOPIncSwitch(preOPIncSwitch);
}

/**
 * @tc.name: GetUnsupportReason
 * @tc.desc: Verify the GetUnsupportReason function
 * @tc.type: FUNC
 * @tc.require: issueICP90U
 */
HWTEST_F(RSOpincManagerTest, GetUnsupportReason, Function | SmallTest | Level1)
{
    NodeId id = 0;
    auto rsCanvasRenderNode = std::make_shared<RSCanvasRenderNode>(id);
    ASSERT_NE(rsCanvasRenderNode, nullptr);

    rsCanvasRenderNode->opincCache_.subTreeSupportFlag_ = false;
    ASSERT_EQ(opincManager_.GetUnsupportReason(*rsCanvasRenderNode), OpincUnsupportType::CHILD_NOT_SUPPORT);
    rsCanvasRenderNode->opincCache_.subTreeSupportFlag_ = true;

    std::shared_ptr<RSRenderNode> inNode = std::make_shared<RSBaseRenderNode>(id + 1);
    std::shared_ptr<RSRenderNode> outNode = std::make_shared<RSBaseRenderNode>(id + 2);
    auto sharedTransitionParam = std::make_shared<SharedTransitionParam>(inNode, outNode, true);
    rsCanvasRenderNode->SetSharedTransitionParam(sharedTransitionParam);
    ASSERT_EQ(opincManager_.GetUnsupportReason(*rsCanvasRenderNode), OpincUnsupportType::SHARED_TRANSITION);
    rsCanvasRenderNode->SetSharedTransitionParam(nullptr);

    auto& property = rsCanvasRenderNode->GetMutableRenderProperties();

    property.SetSpherize(1.0f);
    ASSERT_EQ(opincManager_.GetUnsupportReason(*rsCanvasRenderNode), OpincUnsupportType::SPHERIZE);
    property.SetSpherize(0.0f);

    property.GetEffect().isAttractionValid_ = true;
    ASSERT_EQ(opincManager_.GetUnsupportReason(*rsCanvasRenderNode), OpincUnsupportType::ATTRACTION);
    property.GetEffect().isAttractionValid_ = false;

    property.needFilter_ = true;
    ASSERT_EQ(opincManager_.GetUnsupportReason(*rsCanvasRenderNode), OpincUnsupportType::HAS_FILTER);
    property.needFilter_ = false;

    property.GetEffect().useEffect_ = true;
    ASSERT_EQ(opincManager_.GetUnsupportReason(*rsCanvasRenderNode), OpincUnsupportType::USE_EFFECT);
    property.GetEffect().useEffect_ = false;

    Color color(255, 0, 0);
    std::optional<Color> colorBlend = color;
    property.SetColorBlend(colorBlend);
    ASSERT_EQ(opincManager_.GetUnsupportReason(*rsCanvasRenderNode), OpincUnsupportType::COLOR_BLEND);
    std::optional<Color> colorBlendEmpty;
    property.SetColorBlend(colorBlendEmpty);

    rsCanvasRenderNode->childHasVisibleFilter_ = true;
    ASSERT_EQ(opincManager_.GetUnsupportReason(*rsCanvasRenderNode), OpincUnsupportType::CHILD_HAS_FILTER);
    rsCanvasRenderNode->childHasVisibleFilter_ = false;

    rsCanvasRenderNode->childHasVisibleEffect_ = true;
    ASSERT_EQ(opincManager_.GetUnsupportReason(*rsCanvasRenderNode), OpincUnsupportType::CHILD_HAS_EFFECT);
    rsCanvasRenderNode->childHasVisibleEffect_ = false;

    ASSERT_EQ(opincManager_.GetUnsupportReason(*rsCanvasRenderNode), OpincUnsupportType::NONE);
}

/**
 * @tc.name: GetUnsupportReason1
 * @tc.desc: Verify the GetUnsupportReason function
 * @tc.type: FUNC
 * @tc.require: issueICP90U
 */
HWTEST_F(RSOpincManagerTest, GetUnsupportReason1, Function | SmallTest | Level1)
{
    NodeId id = 0;
    auto rsCanvasRenderNode = std::make_shared<RSCanvasRenderNode>(id);
    ASSERT_NE(rsCanvasRenderNode, nullptr);

    auto& property = rsCanvasRenderNode->GetMutableRenderProperties();

    property.hasHarmonium_ = true;
    ASSERT_EQ(opincManager_.GetUnsupportReason(*rsCanvasRenderNode), OpincUnsupportType::HAS_HARMONIUM);
    property.hasHarmonium_ = false;
}

/**
 * @tc.name: QuickGetNodeDebugInfo
 * @tc.desc: Verify the QuickGetNodeDebugInfo function
 * @tc.type: FUNC
 * @tc.require: issueICP90U
 */
HWTEST_F(RSOpincManagerTest, QuickGetNodeDebugInfo, Function | SmallTest | Level1)
{
    NodeId id = 0;
    auto rsCanvasRenderNode = std::make_shared<RSCanvasRenderNode>(id);
    ASSERT_NE(rsCanvasRenderNode, nullptr);
    ASSERT_NE(opincManager_.QuickGetNodeDebugInfo(*rsCanvasRenderNode), "");
}

/**
 * @tc.name: QuickGetNodeDebugInfoNoRootCache
 * @tc.desc: Verify debug info fallback values are used when root cache pointer is null
 * @tc.type: FUNC
 * @tc.require: issueRootCacheBranch
 */
HWTEST_F(RSOpincManagerTest, QuickGetNodeDebugInfoNoRootCache, Function | SmallTest | Level1)
{
    NodeId id = 102;
    auto node = std::make_shared<RSCanvasRenderNode>(id);
    ASSERT_NE(node, nullptr);
    ASSERT_EQ(node->TryGetOpincRootCachePtr(), nullptr);

    auto info = opincManager_.QuickGetNodeDebugInfo(*node);
    ASSERT_NE(info, "");
    ASSERT_NE(info.find("s:0"), std::string::npos);
    ASSERT_NE(info.find("uc:0"), std::string::npos);
    ASSERT_NE(info.find("suggest:0"), std::string::npos);
    ASSERT_NE(info.find("rootF:0"), std::string::npos);
}

/**
 * @tc.name: StaticOpincHelperNullRootCache
 * @tc.desc: Verify static helper wrappers still work when root cache pointer is null
 * @tc.type: FUNC
 * @tc.require: issueRootCacheBranch
 */
HWTEST_F(RSOpincManagerTest, StaticOpincHelperNullRootCache, Function | SmallTest | Level1)
{
    NodeId id = 103;
    auto node = std::make_shared<RSCanvasRenderNode>(id);
    ASSERT_NE(node, nullptr);
    ASSERT_EQ(node->TryGetOpincRootCachePtr(), nullptr);

    bool unchangeMarkInApp = false;
    RSOpincManager::OpincSetInAppStateStart(*node, unchangeMarkInApp);
    ASSERT_TRUE(unchangeMarkInApp);

    RSOpincManager::OpincSetInAppStateEnd(*node, unchangeMarkInApp);
    ASSERT_FALSE(unchangeMarkInApp);

    ASSERT_FALSE(RSOpincManager::IsSuggestOpincNode(*node));
    ASSERT_FALSE(RSOpincManager::OpincGetRootFlag(*node));
    ASSERT_FALSE(RSOpincManager::IsOpincUnchangeState(*node));
}

/**
 * @tc.name: StaticOpincHelperNonNullRootCache
 * @tc.desc: Verify line 261 and 270 true branches when root cache exists
 * @tc.type: FUNC
 * @tc.require: issueLine261270
 */
HWTEST_F(RSOpincManagerTest, StaticOpincHelperNonNullRootCache, Function | SmallTest | Level1)
{
    constexpr NodeId NODE_ID = 201;
    auto node = std::make_shared<RSCanvasRenderNode>(NODE_ID);
    ASSERT_NE(node, nullptr);
    auto& rootCache = node->GetOpincRootCache();
    (void)rootCache;
    ASSERT_NE(node->TryGetOpincRootCachePtr(), nullptr);

    bool unchangeMarkInApp = false;
    RSOpincManager::OpincSetInAppStateStart(*node, unchangeMarkInApp);
    ASSERT_TRUE(unchangeMarkInApp);

    RSOpincManager::OpincSetInAppStateEnd(*node, unchangeMarkInApp);
    ASSERT_FALSE(unchangeMarkInApp);
}

/**
 * @tc.name: StaticOpincHelperStartEndCrossNode
 * @tc.desc: verify start/end pairing remains node-local through manager wrappers
 * @tc.type: FUNC
 * @tc.require: issueInAppFlagMove
 */
HWTEST_F(RSOpincManagerTest, StaticOpincHelperStartEndCrossNode, Function | SmallTest | Level1)
{
    auto nodeA = std::make_shared<RSCanvasRenderNode>(301);
    auto nodeB = std::make_shared<RSCanvasRenderNode>(302);
    ASSERT_NE(nodeA, nullptr);
    ASSERT_NE(nodeB, nullptr);

    bool unchangeMarkInApp = false;
    RSOpincManager::OpincSetInAppStateStart(*nodeA, unchangeMarkInApp);
    ASSERT_TRUE(unchangeMarkInApp);

    RSOpincManager::OpincSetInAppStateStart(*nodeB, unchangeMarkInApp);
    ASSERT_TRUE(unchangeMarkInApp);

    RSOpincManager::OpincSetInAppStateEnd(*nodeB, unchangeMarkInApp);
    ASSERT_TRUE(unchangeMarkInApp);

    RSOpincManager::OpincSetInAppStateEnd(*nodeA, unchangeMarkInApp);
    ASSERT_FALSE(unchangeMarkInApp);
}

/**
 * @tc.name: InitLayerPartRenderNodeNullCacheBranch
 * @tc.desc: Verify line 301 true branch when layerPartRenderCache is nullptr
 * @tc.type: FUNC
 * @tc.require: issueLine301
 */
HWTEST_F(RSOpincManagerTest, InitLayerPartRenderNodeNullCacheBranch, Function | SmallTest | Level1)
{
    constexpr NodeId NODE_ID = 202;
    auto node = std::make_shared<RSCanvasRenderNode>(NODE_ID);
    ASSERT_NE(node, nullptr);
    ASSERT_EQ(node->TryGetLayerPartRenderCachePtr(), nullptr);

    std::shared_ptr<RSDirtyRegionManager> layerPartRenderDirtyManager = std::make_shared<RSDirtyRegionManager>();
    ASSERT_NE(layerPartRenderDirtyManager, nullptr);

    opincManager_.InitLayerPartRenderNode(true, *node, layerPartRenderDirtyManager);
    ASSERT_NE(layerPartRenderDirtyManager, nullptr);
    ASSERT_EQ(node->TryGetLayerPartRenderCachePtr(), nullptr);
}

/**
 * @tc.name: CalculateAndUpdateLayerPartRenderDirtyRegionNotEnabled
 * @tc.desc: Verify line 417 true branch when IsLayerPartRender returns false
 * @tc.type: FUNC
 * @tc.require: issueLine417
 */
HWTEST_F(RSOpincManagerTest, CalculateAndUpdateLayerPartRenderDirtyRegionNotEnabled,
    Function | SmallTest | Level1)
{
    constexpr NodeId NODE_ID = 203;
    auto node = std::make_shared<RSCanvasRenderNode>(NODE_ID);
    ASSERT_NE(node, nullptr);
    auto& layerPartRenderCache = node->GetLayerPartRenderCache();
    (void)layerPartRenderCache;
    ASSERT_NE(node->TryGetLayerPartRenderCachePtr(), nullptr);
    ASSERT_FALSE(node->GetLayerPartRenderCache().IsLayerPartRender());

    node->stagingRenderParams_ = std::make_unique<RSRenderParams>(NODE_ID);
    ASSERT_NE(node->GetStagingRenderParams(), nullptr);

    std::shared_ptr<RSDirtyRegionManager> layerPartRenderDirtyManager = std::make_shared<RSDirtyRegionManager>();
    ASSERT_NE(layerPartRenderDirtyManager, nullptr);

    const RectI visibleFilterRect = RectI(0, 0, 1, 1);
    opincManager_.CalculateAndUpdateLayerPartRenderDirtyRegion(*node, layerPartRenderDirtyManager,
        visibleFilterRect, false);

    ASSERT_NE(layerPartRenderDirtyManager, nullptr);
    ASSERT_FALSE(node->GetStagingRenderParams()->GetLayerPartRenderEnabled());
}

/**
 * @tc.name: QuickMarkStableNodeAccessibilityRootDirty
 * @tc.desc: Verify barrier-free access state change resets opinc state
 *           when node is opinc root
 * @tc.type: FUNC
 * @tc.require: issueRootCacheBranch
 */
HWTEST_F(RSOpincManagerTest, QuickMarkStableNodeAccessibilityRootDirty, Function | SmallTest | Level1)
{
    NodeId id = 104;
    auto node = std::make_shared<RSCanvasRenderNode>(id);
    ASSERT_NE(node, nullptr);
    ASSERT_EQ(node->TryGetOpincRootCachePtr(), nullptr);
    auto& rootCache = node->GetOpincRootCache();
    ASSERT_NE(node->TryGetOpincRootCachePtr(), nullptr);
    rootCache.nodeCacheState_ = NodeCacheState::STATE_UNCHANGE;
    rootCache.isSuggestOpincNode_ = true;
    rootCache.isOpincRootFlag_ = true;

    bool unchangeMarkInApp = true;
    bool unchangeMarkEnable = true;
    opincManager_.QuickMarkStableNode(*node, unchangeMarkInApp, unchangeMarkEnable, true);

    ASSERT_EQ(rootCache.GetNodeCacheState(), NodeCacheState::STATE_CHANGE);
}

/**
 * @tc.name: AddOpincCacheMem
 * @tc.desc: Verify the AddOpincCacheMem function
 * @tc.type: FUNC
 * @tc.require: issueICP90U
 */
HWTEST_F(RSOpincManagerTest, AddOpincCacheMem, Function | SmallTest | Level1)
{
    int64_t cacheMem = opincManager_.GetOpincCacheMem();
    int32_t cacheCount = opincManager_.GetOpincCacheCount();
    opincManager_.AddOpincCacheMem(CACHE_MEM);
    EXPECT_EQ(opincManager_.GetOpincCacheMem(), cacheMem + CACHE_MEM);
    EXPECT_EQ(opincManager_.GetOpincCacheCount(), cacheCount + 1);
}

/**
 * @tc.name: ReduceOpincCacheMem
 * @tc.desc: Verify the ReduceOpincCacheMem function
 * @tc.type: FUNC
 * @tc.require: issueICP90U
 */
HWTEST_F(RSOpincManagerTest, ReduceOpincCacheMem, Function | SmallTest | Level1)
{
    int64_t cacheMem = opincManager_.GetOpincCacheMem();
    int32_t cacheCount = opincManager_.GetOpincCacheCount();
    opincManager_.AddOpincCacheMem(CACHE_MEM);
    opincManager_.ReduceOpincCacheMem(CACHE_MEM);
    EXPECT_EQ(opincManager_.GetOpincCacheMem(), cacheMem);
    EXPECT_EQ(opincManager_.GetOpincCacheCount(), cacheCount);

    cacheMem = opincManager_.GetOpincCacheMem();
    opincManager_.ReduceOpincCacheMem(cacheMem + 1);
    EXPECT_EQ(opincManager_.GetOpincCacheMem(), 0);
}
}