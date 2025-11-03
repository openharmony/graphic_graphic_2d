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
    EXPECT_FALSE(opincManager_.OpincGetCanvasNodeSupportFlag(*rsCanvasRenderNode));
    rsCanvasRenderNode->hasHdrPresent_ = false;
    ASSERT_TRUE(opincManager_.OpincGetCanvasNodeSupportFlag(*rsCanvasRenderNode));

    rsCanvasRenderNode->SetSharedTransitionParam(sharedTransitionParam);
    EXPECT_FALSE(opincManager_.OpincGetCanvasNodeSupportFlag(*rsCanvasRenderNode));
    rsCanvasRenderNode->SetSharedTransitionParam(nullptr);
    ASSERT_TRUE(opincManager_.OpincGetCanvasNodeSupportFlag(*rsCanvasRenderNode));

    auto& property = rsCanvasRenderNode->GetMutableRenderProperties();

    property.GetEffect().isSpherizeValid_ = true;
    EXPECT_FALSE(opincManager_.OpincGetCanvasNodeSupportFlag(*rsCanvasRenderNode));
    property.GetEffect().isSpherizeValid_ = false;
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
    rsCanvasRenderNode->GetOpincCache().isSuggestOpincNode_ = true;
    ASSERT_TRUE(opincManager_.IsOpincSubTreeDirty(*rsCanvasRenderNode, true));

    rsCanvasRenderNode->GetOpincCache().isSuggestOpincNode_ = false;
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
    auto& opincCache = rsCanvasRenderNode->GetOpincCache();
    bool unchangeMarkInApp = true;
    bool unchangeMarkEnable = false;
    opincCache.isSuggestOpincNode_ = true;

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
    auto& opincCache = rsCanvasRenderNode->GetOpincCache();
    bool unchangeMarkEnable = false;
    opincCache.isSuggestOpincNode_ = true;

    opincCache.isOpincRootFlag_ = false;
    opincManager_.UpdateRootFlag(*rsCanvasRenderNode, unchangeMarkEnable);
    ASSERT_FALSE(opincCache.isUnchangeMarkEnable_);

    opincCache.isOpincRootFlag_ = true;
    opincManager_.UpdateRootFlag(*rsCanvasRenderNode, unchangeMarkEnable);
    ASSERT_FALSE(opincCache.isUnchangeMarkEnable_);

    rsCanvasRenderNode->stagingRenderParams_ = std::make_unique<RSRenderParams>(id);
    opincCache.isOpincRootFlag_ = false;
    opincManager_.UpdateRootFlag(*rsCanvasRenderNode, unchangeMarkEnable);
    ASSERT_FALSE(rsCanvasRenderNode->stagingRenderParams_->isOpincRootFlag_);

    opincCache.isOpincRootFlag_ = true;
    opincManager_.UpdateRootFlag(*rsCanvasRenderNode, unchangeMarkEnable);
    ASSERT_TRUE(rsCanvasRenderNode->stagingRenderParams_->isOpincRootFlag_);
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

    property.GetEffect().isSpherizeValid_ = true;
    ASSERT_EQ(opincManager_.GetUnsupportReason(*rsCanvasRenderNode), OpincUnsupportType::SPHERIZE);
    property.GetEffect().isSpherizeValid_ = false;

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
}