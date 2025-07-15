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
    ASSERT_FALSE(opincManager_.OpincGetNodeSupportFlag(*rsCanvasRenderNode));

    auto rsCanvasDrawingRenderNode = std::make_shared<RSCanvasDrawingRenderNode>(++id);
    ASSERT_NE(rsCanvasDrawingRenderNode, nullptr);
    ASSERT_FALSE(opincManager_.OpincGetNodeSupportFlag(*rsCanvasDrawingRenderNode));
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
    rsCanvasRenderNode->SetSharedTransitionParam(sharedTransitionParam);
    EXPECT_FALSE(opincManager_.OpincGetCanvasNodeSupportFlag(*rsCanvasRenderNode));
    rsCanvasRenderNode->SetSharedTransitionParam(nullptr);
    ASSERT_TRUE(opincManager_.OpincGetCanvasNodeSupportFlag(*rsCanvasRenderNode));

    auto& property = rsCanvasRenderNode->GetMutableRenderProperties();

    property.isSpherizeValid_ = true;
    EXPECT_FALSE(opincManager_.OpincGetCanvasNodeSupportFlag(*rsCanvasRenderNode));
    property.isSpherizeValid_ = false;
    ASSERT_TRUE(opincManager_.OpincGetCanvasNodeSupportFlag(*rsCanvasRenderNode));

    property.isAttractionValid_ = true;
    EXPECT_FALSE(opincManager_.OpincGetCanvasNodeSupportFlag(*rsCanvasRenderNode));
    property.isAttractionValid_ = false;
    ASSERT_TRUE(opincManager_.OpincGetCanvasNodeSupportFlag(*rsCanvasRenderNode));

    property.needFilter_ = true;
    EXPECT_FALSE(opincManager_.OpincGetCanvasNodeSupportFlag(*rsCanvasRenderNode));
    property.needFilter_ = false;
    ASSERT_TRUE(opincManager_.OpincGetCanvasNodeSupportFlag(*rsCanvasRenderNode));

    property.useEffect_ = true;
    EXPECT_FALSE(opincManager_.OpincGetCanvasNodeSupportFlag(*rsCanvasRenderNode));
    property.useEffect_ = false;
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
}