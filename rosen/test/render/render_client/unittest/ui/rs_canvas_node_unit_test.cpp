/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"
#include "animation/rs_animation.h"
#include "core/transaction/rs_interfaces.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_ui_director.h"
#include "draw/paint.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
constexpr static float FLOAT_DATA_ZERO = 0.0f;
constexpr static float FLOAT_DATA_POSITIVE = 485.44f;
constexpr static float FLOAT_DATA_NEGATIVE = -34.4f;
constexpr static float FLOAT_DATA_MAX = std::numeric_limits<float>::max();
constexpr static float FLOAT_DATA_MIN = std::numeric_limits<float>::min();

class RsCanvasNodesTest : public testing::Test {
public:
    constexpr static float floatData[] = {
        FLOAT_DATA_ZERO, FLOAT_DATA_POSITIVE, FLOAT_DATA_NEGATIVE,
        FLOAT_DATA_MAX, FLOAT_DATA_MIN,
        };
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    Vector4f createV4fWithValue(float value) const
    {
        return {value, value, value, value};
    }
    void SetBorderDashParamsAndTest(float value) const
    {
        SetBorderDashParamsAndTest(createV4fWithValue(value));
    }
    void SetOutlineDashParamsAndTest(float value) const
    {
        SetOutlineDashParamsAndTest(createV4fWithValue(value));
    }
    void SetBorderDashParamsAndTest(const Vector4f& params) const
    {
        auto rsNode = RSCanvasNode::Create();
        rsNode->SetBorderDashWidth(params);
        rsNode->SetBorderDashGap(params);
        auto borderDashWidth = rsNode->GetStagingProperties().GetBorderDashWidth();
        auto borderDashGap = rsNode->GetStagingProperties().GetBorderDashGap();
        EXPECT_TRUE(borderDashWidth.IsNearEqual(params));
        EXPECT_TRUE(borderDashGap.IsNearEqual(params));
    }
    void SetOutlineDashParamsAndTest(const Vector4f& params) const
    {
        auto rsNode = RSCanvasNode::Create();
        rsNode->SetOutlineDashWidth(params);
        rsNode->SetOutlineDashGap(params);
        auto borderOutlineWidth = rsNode->GetStagingProperties().GetOutlineDashWidth();
        auto borderOutlineGap = rsNode->GetStagingProperties().GetOutlineDashGap();
        EXPECT_TRUE(borderOutlineWidth.IsNearEqual(params));
        EXPECT_TRUE(borderOutlineGap.IsNearEqual(params));
    }
};

void RsCanvasNodesTest::SetUpTestCase() {}
void RsCanvasNodesTest::TearDownTestCase() {}
void RsCanvasNodesTest::SetUp() {}
void RsCanvasNodesTest::TearDown() {}

/**
 * @tc.name: Create01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, Create01, TestSize.Level2)
{
	// return shared_ptr
    RSCanvasNode::SharedPtr rootNode = RSCanvasNode::Create();
    ASSERT_TRUE(rootNode != nullptr);
}

/**
 * @tc.name: Create02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, Create02, TestSize.Level2)
{
	// return shared_ptr
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create(true);
    ASSERT_TRUE(canvasnode != nullptr);
}

/**
 * @tc.name: Create03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, Create03, TestSize.Level2)
{
	// return shared_ptr
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create(false);
    ASSERT_TRUE(canvasnode != nullptr);
}

/**
 * @tc.name: LifeCycle01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, LifeCycle01, TestSize.Level2)
{
    /**
     * @tc.steps: step1. create canvasnode and add child
     */
    RSCanvasNode::SharedPtr rootNode = RSCanvasNode::Create();
    ASSERT_TRUE(rootNode != nullptr);

    RSCanvasNode::SharedPtr child1 = RSCanvasNode::Create();
    RSCanvasNode::SharedPtr child2 = RSCanvasNode::Create();
    RSCanvasNode::SharedPtr child3 = RSCanvasNode::Create();
    rootNode->AddChild(child1, -1);
    rootNode->AddChild(child2, 0);
    child1->AddChild(child3, 1);

    /**
     * @tc.steps: step2. remove child
     */
    rootNode->RemoveChild(child2);
}

/**
 * @tc.name: Recording01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, Recording01, TestSize.Level2)
{
    /**
     * @tc.steps: step1. create canvasnode and RSUIDirector
     */
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();

    /**
     * @tc.steps: step2. begin recording
     */
    EXPECT_FALSE(canvasnode->IsRecording());
    canvasnode->BeginRecording(50, 40);
    EXPECT_TRUE(canvasnode->IsRecording());
}

/**
 * @tc.name: Recording02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, Recording02, TestSize.Level2)
{
    /**
     * @tc.steps: step1. create canvasnode and RSUIDirector
     */
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();

    /**
     * @tc.steps: step2. begin recording
     */
    EXPECT_FALSE(canvasnode->IsRecording());
    canvasnode->FinishRecording();
    canvasnode->BeginRecording(50, 40);
    EXPECT_TRUE(canvasnode->IsRecording());
    canvasnode->FinishRecording();
}

/**
 * @tc.name: SetPaintOrder01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetPaintOrder01, TestSize.Level2)
{
    /**
     * @tc.steps: step1. create canvasnode and RSUIDirector
     */
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetPaintOrder(true);
}

/**
 * @tc.name: SetandGetBounds01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBounds01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetBounds(floatData[0], floatData[1], floatData[2], floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBounds()[0], floatData[0]));
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBounds()[1], floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBounds()[2], floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBounds()[3], floatData[3]));
}

/**
 * @tc.name: SetandGetBounds02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBounds02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetBounds(floatData[3], floatData[1], floatData[2], floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBounds()[0], floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBounds()[1], floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBounds()[2], floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBounds()[3], floatData[0]));
}

/**
 * @tc.name: SetandGetBounds03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBounds03, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetBounds(floatData[3], floatData[2], floatData[1], floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBounds()[0], floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBounds()[1], floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBounds()[2], floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBounds()[3], floatData[0]));
}

/**
 * @tc.name: LifeCycle02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, LifeCycle02, TestSize.Level2)
{
    /**
     * @tc.steps: step1. create canvasnode and add child
     */
    RSCanvasNode::SharedPtr rootNode = RSCanvasNode::Create();
    ASSERT_TRUE(rootNode != nullptr);

    RSCanvasNode::SharedPtr child1 = RSCanvasNode::Create();
    RSCanvasNode::SharedPtr child2 = RSCanvasNode::Create();
    RSCanvasNode::SharedPtr child3 = RSCanvasNode::Create();
    rootNode->AddChild(child1, -1);
    rootNode->AddChild(child2, 0);
    child1->AddChild(child3, 1);
    /**
     * @tc.steps: step2. remove child
     */
    rootNode->RemoveFromTree();
}

/**
 * @tc.name: LifeCycle03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, LifeCycle03, TestSize.Level2)
{
    /**
     * @tc.steps: step1. create canvasnode and add child
     */
    RSCanvasNode::SharedPtr rootNode = RSCanvasNode::Create();
    ASSERT_TRUE(rootNode != nullptr);

    RSCanvasNode::SharedPtr child1 = RSCanvasNode::Create();
    RSCanvasNode::SharedPtr child2 = RSCanvasNode::Create();
    RSCanvasNode::SharedPtr child3 = RSCanvasNode::Create();
    rootNode->AddChild(child1, -1);
    rootNode->AddChild(child2, 0);
    child1->AddChild(child3, 1);
    /**
     * @tc.steps: step2. remove child
     */
    rootNode->ClearChildren();
}

/**
 * @tc.name: LifeCycle04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, LifeCycle04, TestSize.Level2)
{
    /**
     * @tc.steps: step1. create canvasnode and add child
     */
    RSCanvasNode::SharedPtr rootNode = RSCanvasNode::Create();
    ASSERT_TRUE(rootNode != nullptr);

    RSCanvasNode::SharedPtr child1 = RSCanvasNode::Create();
    RSCanvasNode::SharedPtr child2 = RSCanvasNode::Create();
    RSCanvasNode::SharedPtr child3 = RSCanvasNode::Create();
    rootNode->AddChild(child1, -1);
    rootNode->AddChild(child2, 0);
    rootNode->DumpNode(1);
    child1->AddChild(child3, 1);
    child1->MoveChild(child2, 0);
    child1->MoveChild(child3, 1);
    child1->AddChild(child3, 1);
    child1->MoveChild(child3, -1);
    child1->AddChild(child2, -1);
    child1->MoveChild(child2, 3);
    /**
     * @tc.steps: step2. remove child
     */
    rootNode->RemoveChild(child3);
}

/**
 * @tc.name: SetandGetBoundsWidth01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBoundsWidth01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetBoundsWidth(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBounds()[2], floatData[1]));
}

/**
 * @tc.name: SetandGetBoundsWidth02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBoundsWidth02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetBoundsWidth(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBounds()[2], floatData[2]));
}

/**
 * @tc.name: SetandGetBoundsWidth03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBoundsWidth03, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetBoundsWidth(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBounds()[2], floatData[3]));
}

/**
 * @tc.name: SetandGetBoundsWidth04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBoundsWidth04, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetBoundsWidth(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBounds()[2], floatData[4]));
}

/**
 * @tc.name: SetandGetBoundsWidth05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBoundsWidth05, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetBoundsWidth(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBounds()[2], floatData[0]));
}

/**
 * @tc.name: SetandGetBoundsHeight01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBoundsHeight01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetBoundsHeight(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBounds()[3], floatData[1]));
}

/**
 * @tc.name: SetandGetBoundsHeight02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBoundsHeight02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetBoundsHeight(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBounds()[3], floatData[2]));
}

/**
 * @tc.name: SetandGetBoundsHeight03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBoundsHeight03, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetBoundsHeight(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBounds()[3], floatData[3]));
}

/**
 * @tc.name: SetandGetBoundsHeight04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBoundsHeight04, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetBoundsHeight(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBounds()[3], floatData[4]));
}

/**
 * @tc.name: SetandGetBoundsHeight05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBoundsHeight05, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetBoundsHeight(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBounds()[3], floatData[0]));
}

/**
 * @tc.name: SetandGetFrame01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetFrame01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetFrame(floatData[0], floatData[1], floatData[2], floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetFrame()[0], floatData[0]));
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetFrame()[1], floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetFrame()[2], floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetFrame()[3], floatData[3]));
}

/**
 * @tc.name: SetandGetFrame02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetFrame02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetFrame(floatData[3], floatData[1], floatData[2], floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetFrame()[0], floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetFrame()[1], floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetFrame()[2], floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetFrame()[3], floatData[0]));
}

/**
 * @tc.name: SetandGetFrame03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetFrame03, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetFrame(floatData[3], floatData[2], floatData[1], floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetFrame()[0], floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetFrame()[1], floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetFrame()[2], floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetFrame()[3], floatData[0]));
}

/**
 * @tc.name: SetandGetFramePositionX01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetFramePositionX01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetFramePositionX(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetFrame()[0], floatData[1]));
}

/**
 * @tc.name: SetandGetFramePositionX02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetFramePositionX02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetFramePositionX(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetFrame()[0], floatData[2]));
}

/**
 * @tc.name: SetandGetFramePositionX03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetFramePositionX03, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetFramePositionX(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetFrame()[0], floatData[3]));
}

/**
 * @tc.name: SetandGetFramePositionX04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetFramePositionX04, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetFramePositionX(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetFrame()[0], floatData[4]));
}

/**
 * @tc.name: SetandGetFramePositionX05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetFramePositionX05, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetFramePositionX(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetFrame()[0], floatData[0]));
}

/**
 * @tc.name: SetandGetFramePositionY01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetFramePositionY01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetFramePositionY(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetFrame()[1], floatData[1]));
}

/**
 * @tc.name: SetandGetFramePositionY02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetFramePositionY02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetFramePositionY(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetFrame()[1], floatData[2]));
}

/**
 * @tc.name: SetandGetFramePositionY03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetFramePositionY03, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetFramePositionY(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetFrame()[1], floatData[3]));
}

/**
 * @tc.name: SetandGetFramePositionY04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetFramePositionY04, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetFramePositionY(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetFrame()[1], floatData[4]));
}

/**
 * @tc.name: SetandGetFramePositionY05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetFramePositionY05, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetFramePositionY(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetFrame()[1], floatData[0]));
}

/**
 * @tc.name: SetandGetPositionZ01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetPositionZ01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetPositionZ(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetPositionZ(), floatData[1]));
}

/**
 * @tc.name: SetandGetPositionZ02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetPositionZ02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetPositionZ(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetPositionZ(), floatData[2]));
}

/**
 * @tc.name: SetandGetPositionZ03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetPositionZ03, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetPositionZ(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetPositionZ(), floatData[3]));
}

/**
 * @tc.name: SetandGetPositionZ04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetPositionZ04, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetPositionZ(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetPositionZ(), floatData[4]));
}

/**
 * @tc.name: SetandGetPositionZ05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetPositionZ05, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetPositionZ(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetPositionZ(), floatData[0]));
}

/**
 * @tc.name: SetandGetCornerRadius01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetCornerRadius01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetCornerRadius(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetCornerRadius().x_, floatData[1]));
}

/**
 * @tc.name: SetandGetCornerRadius02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetCornerRadius02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetCornerRadius(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetCornerRadius().y_, floatData[2]));
}

/**
 * @tc.name: SetandGetCornerRadius03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetCornerRadius03, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetCornerRadius(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetCornerRadius().z_, floatData[3]));
}

/**
 * @tc.name: SetandGetCornerRadius04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetCornerRadius04, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetCornerRadius(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetCornerRadius().w_, floatData[4]));
}

/**
 * @tc.name: SetandGetCornerRadius05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetCornerRadius05, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetCornerRadius(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetCornerRadius().x_, floatData[0]));
}

/**
 * @tc.name: SetandGetRotationThree01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetRotationThree01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetRotation(floatData[1], floatData[2], floatData[3]);
}

/**
 * @tc.name: SetandGetRotation01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetRotation01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetRotation(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetRotation(), floatData[1]));
}

/**
 * @tc.name: SetandGetRotation02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetRotation02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetRotation(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetRotation(), floatData[2]));
}

/**
 * @tc.name: SetandGetRotation03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetRotation03, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetRotation(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetRotation(), floatData[3]));
}

/**
 * @tc.name: SetandGetRotation04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetRotation04, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetRotation(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetRotation(), floatData[4]));
}

/**
 * @tc.name: SetandGetRotation05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetRotation05, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetRotation(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetRotation(), floatData[0]));
}

/**
 * @tc.name: SetandGetRotationX01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetRotationX01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetRotationX(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetRotationX(), floatData[1]));
}

/**
 * @tc.name: SetandGetRotationX02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetRotationX02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetRotationX(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetRotationX(), floatData[2]));
}

/**
 * @tc.name: SetandGetRotationX03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetRotationX03, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetRotationX(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetRotationX(), floatData[3]));
}

/**
 * @tc.name: SetandGetRotationX04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetRotationX04, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetRotationX(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetRotationX(), floatData[4]));
}

/**
 * @tc.name: SetandGetRotationX05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetRotationX05, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetRotationX(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetRotationX(), floatData[0]));
}

/**
 * @tc.name: SetandGetRotationY01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetRotationY01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetRotationY(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetRotationY(), floatData[1]));
}

/**
 * @tc.name: SetandGetRotationY02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetRotationY02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetRotationY(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetRotationY(), floatData[2]));
}

/**
 * @tc.name: SetandGetRotationY03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetRotationY03, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetRotationY(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetRotationY(), floatData[3]));
}

/**
 * @tc.name: SetandGetRotationY04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetRotationY04, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetRotationY(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetRotationY(), floatData[4]));
}

/**
 * @tc.name: SetandGetRotationY05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetRotationY05, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetRotationY(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetRotationY(), floatData[0]));
}


/**
 * @tc.name: SetandGetScaleX01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetScaleX01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetScaleX(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetScale().x_, floatData[1]));
}

/**
 * @tc.name: SetandGetScaleX02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetScaleX02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetScaleX(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetScale().x_, floatData[2]));
}

/**
 * @tc.name: SetandGetScaleX03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetScaleX03, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetScaleX(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetScale().x_, floatData[3]));
}

/**
 * @tc.name: SetandGetScaleX04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetScaleX04, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetScaleX(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetScale().x_, floatData[4]));
}

/**
 * @tc.name: SetandGetScaleX05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetScaleX05, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetScaleX(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetScale().x_, floatData[0]));
}

/**
 * @tc.name: SetandGetScale01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetScale01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetScale(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetScale().x_, floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetScale().y_, floatData[1]));
}

/**
 * @tc.name: SetandGetScale02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetScale02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetScale(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetScale().x_, floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetScale().y_, floatData[2]));
}

/**
 * @tc.name: SetandGetScale03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetScale03, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetScale(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetScale().x_, floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetScale().y_, floatData[3]));
}

/**
 * @tc.name: SetandGetScale04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetScale04, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetScale(floatData[3], floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetScale().x_, floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetScale().y_, floatData[0]));
}

/**
 * @tc.name: SetandGetScale005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetScale005, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetScale(floatData[2], floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetScale().x_, floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetScale().y_, floatData[1]));
}

/**
 * @tc.name: SetandGetScaleY01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetScaleY01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetScaleY(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetScale().y_, floatData[1]));
}

/**
 * @tc.name: SetandGetScaleY02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetScaleY02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetScaleY(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetScale().y_, floatData[2]));
}

/**
 * @tc.name: SetandGetScaleY03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetScaleY03, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetScaleY(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetScale().y_, floatData[3]));
}

/**
 * @tc.name: SetandGetScaleY04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetScaleY04, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetScaleY(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetScale().y_, floatData[4]));
}

/**
 * @tc.name: SetandGetScaleY05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetScaleY05, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetScaleY(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetScale().y_, floatData[0]));
}

/**
 * @tc.name: SetandGetSkew01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetSkew01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetSkew(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetSkew().x_, floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetSkew().y_, floatData[1]));
}

/**
 * @tc.name: SetandGetSkew02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetSkew02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetSkew(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetSkew().x_, floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetSkew().y_, floatData[2]));
}

/**
 * @tc.name: SetandGetSkew03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetSkew03, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetSkew(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetSkew().x_, floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetSkew().y_, floatData[3]));
}

/**
 * @tc.name: SetandGetSkew04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetSkew04, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetSkew(floatData[3], floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetSkew().x_, floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetSkew().y_, floatData[0]));
}

/**
 * @tc.name: SetandGetSkew005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetSkew005, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetSkew(floatData[2], floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetSkew().x_, floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetSkew().y_, floatData[1]));
}

/**
 * @tc.name: SetandGetSkewX01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetSkewX01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetSkewX(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetSkew().x_, floatData[1]));
}

/**
 * @tc.name: SetandGetSkewX02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetSkewX02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetSkewX(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetSkew().x_, floatData[2]));
}

/**
 * @tc.name: SetandGetSkewX03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetSkewX03, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetSkewX(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetSkew().x_, floatData[3]));
}

/**
 * @tc.name: SetandGetSkewX04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetSkewX04, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetSkewX(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetSkew().x_, floatData[4]));
}

/**
 * @tc.name: SetandGetSkewX05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetSkewX05, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetSkewX(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetSkew().x_, floatData[0]));
}

/**
 * @tc.name: SetandGetSkewY01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetSkewY01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetSkewY(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetSkew().y_, floatData[1]));
}

/**
 * @tc.name: SetandGetSkewY02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetSkewY02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetSkewY(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetSkew().y_, floatData[2]));
}

/**
 * @tc.name: SetandGetSkewY03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetSkewY03, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetSkewY(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetSkew().y_, floatData[3]));
}

/**
 * @tc.name: SetandGetSkewY04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetSkewY04, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetSkewY(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetSkew().y_, floatData[4]));
}

/**
 * @tc.name: SetandGetSkewY05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetSkewY05, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetSkewY(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetSkew().y_, floatData[0]));
}

/**
 * @tc.name: SetandGetPersp01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetPersp01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetPersp(FLOAT_DATA_POSITIVE);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetPersp().x_, FLOAT_DATA_POSITIVE));
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetPersp().y_, FLOAT_DATA_POSITIVE));
}

/**
 * @tc.name: SetandGetPersp02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetPersp02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetPersp(FLOAT_DATA_NEGATIVE);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetPersp().x_, FLOAT_DATA_NEGATIVE));
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetPersp().y_, FLOAT_DATA_NEGATIVE));
}

/**
 * @tc.name: SetandGetPersp03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetPersp03, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetPersp(FLOAT_DATA_MAX);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetPersp().x_, FLOAT_DATA_MAX));
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetPersp().y_, FLOAT_DATA_MAX));
}

/**
 * @tc.name: SetandGetPersp04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetPersp04, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetPersp(FLOAT_DATA_MAX, FLOAT_DATA_ZERO);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetPersp().x_, FLOAT_DATA_MAX));
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetPersp().y_, FLOAT_DATA_ZERO));
}

/**
 * @tc.name: SetandGetPersp005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetPersp005, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetPersp(FLOAT_DATA_NEGATIVE, FLOAT_DATA_POSITIVE);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetPersp().x_, FLOAT_DATA_NEGATIVE));
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetPersp().y_, FLOAT_DATA_POSITIVE));
}

/**
 * @tc.name: SetandGetPerspX01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetPerspX01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetPerspX(FLOAT_DATA_POSITIVE);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetPersp().x_, FLOAT_DATA_POSITIVE));
}

/**
 * @tc.name: SetandGetPerspX02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetPerspX02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetPerspX(FLOAT_DATA_NEGATIVE);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetPersp().x_, FLOAT_DATA_NEGATIVE));
}

/**
 * @tc.name: SetandGetPerspX03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetPerspX03, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetPerspX(FLOAT_DATA_MAX);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetPersp().x_, FLOAT_DATA_MAX));
}

/**
 * @tc.name: SetandGetPerspX04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetPerspX04, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetPerspX(FLOAT_DATA_MIN);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetPersp().x_, FLOAT_DATA_MIN));
}

/**
 * @tc.name: SetandGetPerspX05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetPerspX05, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetPerspX(FLOAT_DATA_ZERO);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetPersp().x_, FLOAT_DATA_ZERO));
}

/**
 * @tc.name: SetandGetPerspY01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetPerspY01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetPerspY(FLOAT_DATA_POSITIVE);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetPersp().y_, FLOAT_DATA_POSITIVE));
}

/**
 * @tc.name: SetandGetPerspY02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetPerspY02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetPerspY(FLOAT_DATA_NEGATIVE);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetPersp().y_, FLOAT_DATA_NEGATIVE));
}

/**
 * @tc.name: SetandGetPerspY03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetPerspY03, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetPerspY(FLOAT_DATA_MAX);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetPersp().y_, FLOAT_DATA_MAX));
}

/**
 * @tc.name: SetandGetPerspY04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetPerspY04, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetPerspY(FLOAT_DATA_MIN);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetPersp().y_, FLOAT_DATA_MIN));
}

/**
 * @tc.name: SetandGetPerspY05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetPerspY05, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetPerspY(FLOAT_DATA_ZERO);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetPersp().y_, FLOAT_DATA_ZERO));
}

/**
 * @tc.name: SetandGetAlpha01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetAlpha01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetAlpha(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetAlpha(), floatData[1]));
}

/**
 * @tc.name: SetandGetAlpha02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetAlpha02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetAlpha(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetAlpha(), floatData[2]));
}

/**
 * @tc.name: SetandGetAlpha03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetAlpha03, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetAlpha(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetAlpha(), floatData[3]));
}

/**
 * @tc.name: SetandGetAlpha04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetAlpha04, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetAlpha(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetAlpha(), floatData[4]));
}

/**
 * @tc.name: SetandGetAlpha05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetAlpha05, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetAlpha(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetAlpha(), floatData[0]));
}

/**
 * @tc.name: SetandGetBgImageSize01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBgImageSize01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetBgImageSize(floatData[0], floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBgImageWidth(), floatData[0]));
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBgImageHeight(), floatData[1]));
}

/**
 * @tc.name: SetandGetBgImageSize02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBgImageSize02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetBgImageSize(floatData[3], floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBgImageWidth(), floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBgImageHeight(), floatData[1]));
}

/**
 * @tc.name: SetandGetBgImageSize03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBgImageSize03, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetBgImageSize(floatData[3], floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBgImageWidth(), floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBgImageHeight(), floatData[2]));
}

/**
 * @tc.name: SetandGetBgImageWidth01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBgImageWidth01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetBgImageWidth(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBgImageWidth(), floatData[1]));
}

/**
 * @tc.name: SetandGetBgImageWidth02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBgImageWidth02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetBgImageWidth(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBgImageWidth(), floatData[2]));
}

/**
 * @tc.name: SetandGetBgImageWidth03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBgImageWidth03, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetBgImageWidth(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBgImageWidth(), floatData[3]));
}

/**
 * @tc.name: SetandGetBgImageWidth04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBgImageWidth04, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetBgImageWidth(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBgImageWidth(), floatData[4]));
}

/**
 * @tc.name: SetandGetBgImageWidth05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBgImageWidth05, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetBgImageWidth(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBgImageWidth(), floatData[0]));
}

/**
 * @tc.name: SetandGetBgImageHeight01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBgImageHeight01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetBgImageHeight(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBgImageHeight(), floatData[1]));
}

/**
 * @tc.name: SetandGetBgImageHeight02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBgImageHeight02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetBgImageHeight(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBgImageHeight(), floatData[2]));
}

/**
 * @tc.name: SetandGetBgImageHeight03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBgImageHeight03, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetBgImageHeight(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBgImageHeight(), floatData[3]));
}

/**
 * @tc.name: SetandGetBgImageHeight04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBgImageHeight04, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetBgImageHeight(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBgImageHeight(), floatData[4]));
}

/**
 * @tc.name: SetandGetBgImageHeight05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBgImageHeight05, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetBgImageHeight(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBgImageHeight(), floatData[0]));
}

/**
 * @tc.name: SetandSetBgImagePosition01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandSetBgImagePosition01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetBgImagePosition(floatData[2], floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBgImagePositionX(), floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBgImagePositionY(), floatData[3]));
}

/**
 * @tc.name: SetandSetBgImagePosition02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandSetBgImagePosition02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetBgImagePosition(floatData[2], floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBgImagePositionX(), floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBgImagePositionY(), floatData[0]));
}

/**
 * @tc.name: SetandSetBgImagePosition03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandSetBgImagePosition03, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetBgImagePosition(floatData[1], floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBgImagePositionX(), floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBgImagePositionY(), floatData[3]));
}

/**
 * @tc.name: SetandGetBgImagePositionX01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBgImagePositionX01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetBgImagePositionX(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBgImagePositionX(), floatData[1]));
}

/**
 * @tc.name: SetandGetBgImagePositionX02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBgImagePositionX02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetBgImagePositionX(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBgImagePositionX(), floatData[2]));
}

/**
 * @tc.name: SetandGetBgImagePositionX03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBgImagePositionX03, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetBgImagePositionX(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBgImagePositionX(), floatData[3]));
}

/**
 * @tc.name: SetandGetBgImagePositionX04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBgImagePositionX04, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetBgImagePositionX(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBgImagePositionX(), floatData[4]));
}

/**
 * @tc.name: SetandGetBgImagePositionX05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBgImagePositionX05, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetBgImagePositionX(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBgImagePositionX(), floatData[0]));
}

/**
 * @tc.name: SetandGetBgImagePositionY01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBgImagePositionY01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetBgImagePositionY(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBgImagePositionY(), floatData[1]));
}

/**
 * @tc.name: SetandGetBgImagePositionY02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBgImagePositionY02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetBgImagePositionY(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBgImagePositionY(), floatData[2]));
}

/**
 * @tc.name: SetandGetBgImagePositionY03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBgImagePositionY03, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetBgImagePositionY(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBgImagePositionY(), floatData[3]));
}

/**
 * @tc.name: SetandGetBgImagePositionY04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBgImagePositionY04, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetBgImagePositionY(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBgImagePositionY(), floatData[4]));
}

/**
 * @tc.name: SetandGetBgImagePositionY05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBgImagePositionY05, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetBgImagePositionY(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBgImagePositionY(), floatData[0]));
}

/**
 * @tc.name: SetandGetBorderWidth01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBorderWidth01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetBorderWidth(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBorderWidth().x_, floatData[1]));
}

/**
 * @tc.name: SetandGetBorderWidth02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBorderWidth02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetBorderWidth(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBorderWidth().y_, floatData[2]));
}

/**
 * @tc.name: SetandGetBorderWidth03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBorderWidth03, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetBorderWidth(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBorderWidth().z_, floatData[3]));
}

/**
 * @tc.name: SetandGetBorderWidth04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBorderWidth04, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetBorderWidth(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBorderWidth().w_, floatData[4]));
}

/**
 * @tc.name: SetandGetBorderWidth05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBorderWidth05, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetBorderWidth(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetBorderWidth().x_, floatData[0]));
}

/**
 * @tc.name: SetandGetBorderDashParams01
 * @tc.desc: Check for zero values
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBorderDashParams01, TestSize.Level2)
{
    SetBorderDashParamsAndTest(FLOAT_DATA_ZERO);
}

/**
 * @tc.name: SetandGetBorderDashParams02
 * @tc.desc: Check for positive values
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBorderDashParams02, TestSize.Level2)
{
    SetBorderDashParamsAndTest(FLOAT_DATA_POSITIVE);
}

/**
 * @tc.name: SetandGetBorderDashParams03
 * @tc.desc: Check for negative values
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBorderDashParams03, TestSize.Level2)
{
    SetBorderDashParamsAndTest(FLOAT_DATA_NEGATIVE);
}

/**
 * @tc.name: SetandGetBorderDashParams04
 * @tc.desc: Check for max values
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBorderDashParams04, TestSize.Level2)
{
    SetBorderDashParamsAndTest(FLOAT_DATA_MAX);
}

/**
 * @tc.name: SetandGetBorderDashParams05
 * @tc.desc: Check for min values
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBorderDashParams05, TestSize.Level2)
{
    SetBorderDashParamsAndTest(FLOAT_DATA_MIN);
}

/**
 * @tc.name: SetandGetOutlineDashParams01
 * @tc.desc: Check for zero values
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetOutlineDashParams01, TestSize.Level2)
{
    SetOutlineDashParamsAndTest(FLOAT_DATA_ZERO);
}

/**
 * @tc.name: SetandGetOutlineDashParams02
 * @tc.desc: Check for positive values
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetOutlineDashParams02, TestSize.Level2)
{
    SetOutlineDashParamsAndTest(FLOAT_DATA_POSITIVE);
}

/**
 * @tc.name: SetandGetOutlineDashParams03
 * @tc.desc: Check for negative values
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetOutlineDashParams03, TestSize.Level2)
{
    SetOutlineDashParamsAndTest(FLOAT_DATA_NEGATIVE);
}

/**
 * @tc.name: SetandGetOutlineDashParams04
 * @tc.desc: Check for max values
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetOutlineDashParams04, TestSize.Level2)
{
    SetOutlineDashParamsAndTest(FLOAT_DATA_MAX);
}

/**
 * @tc.name: SetandGetOutlineDashParams05
 * @tc.desc: Check for min values
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetOutlineDashParams05, TestSize.Level2)
{
    SetOutlineDashParamsAndTest(FLOAT_DATA_MIN);
}

/**
 * @tc.name: SetandGetPivot01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetPivot01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetPivot(floatData[0], floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetPivot().x_, floatData[0]));
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetPivot().y_, floatData[1]));
}

/**
 * @tc.name: SetandGetPivot02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetPivot02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetPivot(floatData[3], floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetPivot().x_, floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetPivot().y_, floatData[2]));
}

/**
 * @tc.name: SetandGetPivot03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetPivot03, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetPivot(floatData[1], floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetPivot().x_, floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetPivot().y_, floatData[3]));
}

/**
 * @tc.name: SetandGetPivotX01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetPivotX01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetPivotX(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetPivot().x_, floatData[1]));
}

/**
 * @tc.name: SetandGetPivotX02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetPivotX02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetPivotX(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetPivot().x_, floatData[2]));
}

/**
 * @tc.name: SetandGetPivotX03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetPivotX03, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetPivotX(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetPivot().x_, floatData[3]));
}

/**
 * @tc.name: SetandGetPivotX04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetPivotX04, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetPivotX(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetPivot().x_, floatData[4]));
}

/**
 * @tc.name: SetandGetPivotX05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetPivotX05, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetPivotX(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetPivot().x_, floatData[0]));
}

/**
 * @tc.name: SetandGetPivotY01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetPivotY01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetPivotY(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetPivot().y_, floatData[1]));
}

/**
 * @tc.name: SetandGetPivotY02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetPivotY02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetPivotY(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetPivot().y_, floatData[2]));
}

/**
 * @tc.name: SetandGetPivotY03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetPivotY03, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetPivotY(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetPivot().y_, floatData[3]));
}

/**
 * @tc.name: SetandGetPivotY04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetPivotY04, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetPivotY(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetPivot().y_, floatData[4]));
}

/**
 * @tc.name: SetandGetPivotY05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetPivotY05, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetPivotY(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetPivot().y_, floatData[0]));
}

/**
 * @tc.name: SetandGetShadowOffset01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetShadowOffset01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetShadowOffset(floatData[2], floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetShadowOffsetX(), floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetShadowOffsetY(), floatData[3]));
}

/**
 * @tc.name: SetandGetShadowOffset02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetShadowOffset02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetShadowOffset(floatData[2], floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetShadowOffsetX(), floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetShadowOffsetY(), floatData[0]));
}

/**
 * @tc.name: SetandGetShadowOffset03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetShadowOffset03, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetShadowOffset(floatData[1], floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetShadowOffsetX(), floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetShadowOffsetY(), floatData[3]));
}
/**
 * @tc.name: SetandGetShadowOffsetX01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetShadowOffsetX01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetShadowOffsetX(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetShadowOffsetX(), floatData[1]));
}

/**
 * @tc.name: SetandGetShadowOffsetX02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetShadowOffsetX02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetShadowOffsetX(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetShadowOffsetX(), floatData[2]));
}

/**
 * @tc.name: SetandGetShadowOffsetX03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetShadowOffsetX03, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetShadowOffsetX(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetShadowOffsetX(), floatData[3]));
}

/**
 * @tc.name: SetandGetShadowOffsetX04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetShadowOffsetX04, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetShadowOffsetX(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetShadowOffsetX(), floatData[4]));
}

/**
 * @tc.name: SetandGetShadowOffsetX05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetShadowOffsetX05, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetShadowOffsetX(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetShadowOffsetX(), floatData[0]));
}

/**
 * @tc.name: SetandGetShadowOffsetY01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetShadowOffsetY01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetShadowOffsetY(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetShadowOffsetY(), floatData[1]));
}

/**
 * @tc.name: SetandGetShadowOffsetY02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetShadowOffsetY02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetShadowOffsetY(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetShadowOffsetY(), floatData[2]));
}

/**
 * @tc.name: SetandGetShadowOffsetY03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetShadowOffsetY03, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetShadowOffsetY(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetShadowOffsetY(), floatData[3]));
}

/**
 * @tc.name: SetandGetShadowOffsetY04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetShadowOffsetY04, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetShadowOffsetY(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetShadowOffsetY(), floatData[4]));
}

/**
 * @tc.name: SetandGetShadowOffsetY05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetShadowOffsetY05, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetShadowOffsetY(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetShadowOffsetY(), floatData[0]));
}

/**
 * @tc.name: SetandGetShadowAlpha01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetShadowAlpha01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetShadowAlpha(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetShadowAlpha(),
        floatData[1], 0.02f));
}

/**
 * @tc.name: SetandGetShadowAlpha02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetShadowAlpha02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetShadowAlpha(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetShadowAlpha(),
        floatData[2], 0.02f));
}

/**
 * @tc.name: SetandGetShadowAlpha03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetShadowAlpha03, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetShadowAlpha(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetShadowAlpha(),
        floatData[3], 0.02f));
}

/**
 * @tc.name: SetandGetShadowAlpha04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetShadowAlpha04, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetShadowAlpha(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetShadowAlpha(),
        floatData[4], 0.02f));
}

/**
 * @tc.name: SetandGetShadowAlpha05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetShadowAlpha05, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetShadowAlpha(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetShadowAlpha(),
        floatData[0], 0.02f));
}

/**
 * @tc.name: SetandGetShadowElevation01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetShadowElevation01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetShadowElevation(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetShadowElevation(), floatData[1]));
}

/**
 * @tc.name: SetandGetShadowElevation02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetShadowElevation02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetShadowElevation(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetShadowElevation(), floatData[2]));
}

/**
 * @tc.name: SetandGetShadowElevation03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetShadowElevation03, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetShadowElevation(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetShadowElevation(), floatData[3]));
}

/**
 * @tc.name: SetandGetShadowElevation04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetShadowElevation04, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetShadowElevation(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetShadowElevation(), floatData[4]));
}

/**
 * @tc.name: SetandGetShadowElevation05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetShadowElevation05, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetShadowElevation(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetShadowElevation(), floatData[0]));
}

/**
 * @tc.name: SetandGetShadowRadius01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetShadowRadius01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetShadowRadius(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetShadowRadius(), floatData[1]));
}

/**
 * @tc.name: SetandGetShadowRadius02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetShadowRadius02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetShadowRadius(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetShadowRadius(), floatData[2]));
}

/**
 * @tc.name: SetandGetShadowRadius03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetShadowRadius03, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetShadowRadius(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetShadowRadius(), floatData[3]));
}

/**
 * @tc.name: SetandGetShadowRadius04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetShadowRadius04, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetShadowRadius(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetShadowRadius(), floatData[4]));
}

/**
 * @tc.name: SetandGetShadowRadius05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetShadowRadius05, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetShadowRadius(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetShadowRadius(), floatData[0]));
}

/**
 * @tc.name: SetandGetShadowColor01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetShadowColor01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = 0x034123;
    canvasnode->SetShadowColor(colorValue);
    EXPECT_TRUE(canvasnode->GetStagingProperties().GetShadowColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetShadowColor02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetShadowColor02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = 0x45ba87;
    canvasnode->SetShadowColor(colorValue);
    EXPECT_TRUE(canvasnode->GetStagingProperties().GetShadowColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetShadowColor03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetShadowColor03, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = 0x32aadd;
    canvasnode->SetShadowColor(colorValue);
    EXPECT_TRUE(canvasnode->GetStagingProperties().GetShadowColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetShadowColor04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetShadowColor04, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::max();
    canvasnode->SetShadowColor(colorValue);
    EXPECT_TRUE(canvasnode->GetStagingProperties().GetShadowColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetShadowColor05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetShadowColor05, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::min();
    canvasnode->SetShadowColor(colorValue);
    EXPECT_TRUE(canvasnode->GetStagingProperties().GetShadowColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetTranslateThree01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetTranslateThree01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetTranslate(floatData[1], floatData[2], floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetTranslate().x_, floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetTranslate().y_, floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetTranslateZ(), floatData[3]));
}

/**
 * @tc.name: SetandGetTranslateThree02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetTranslateThree02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetTranslate(floatData[0], floatData[0], floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetTranslate().x_, floatData[0]));
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetTranslate().y_, floatData[0]));
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetTranslateZ(), floatData[2]));
}

/**
 * @tc.name: SetandGetTranslateThree03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetTranslateThree03, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetTranslate(floatData[1], floatData[3], floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetTranslate().x_, floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetTranslate().y_, floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetTranslateZ(), floatData[1]));
}

/**
 * @tc.name: SetandGetTranslateX01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetTranslateX01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetTranslateX(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetTranslate().x_, floatData[1]));
}

/**
 * @tc.name: SetandGetTranslateX02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetTranslateX02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetTranslateX(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetTranslate().x_, floatData[2]));
}

/**
 * @tc.name: SetandGetTranslateX03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetTranslateX03, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetTranslateX(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetTranslate().x_, floatData[3]));
}

/**
 * @tc.name: SetandGetTranslateX04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetTranslateX04, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetTranslateX(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetTranslate().x_, floatData[4]));
}

/**
 * @tc.name: SetandGetTranslateX05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetTranslateX05, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetTranslateX(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetTranslate().x_, floatData[0]));
}

/**
 * @tc.name: SetandGetTranslateY01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetTranslateY01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetTranslateY(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetTranslate().y_, floatData[1]));
}

/**
 * @tc.name: SetandGetTranslateY02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetTranslateY02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetTranslateY(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetTranslate().y_, floatData[2]));
}

/**
 * @tc.name: SetandGetTranslateY03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetTranslateY03, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetTranslateY(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetTranslate().y_, floatData[3]));
}

/**
 * @tc.name: SetandGetTranslateY04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetTranslateY04, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetTranslateY(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetTranslate().y_, floatData[4]));
}

/**
 * @tc.name: SetandGetTranslateY05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetTranslateY05, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetTranslateY(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetTranslate().y_, floatData[0]));
}

/**
 * @tc.name: SetandGetTranslateZ01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetTranslateZ01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetTranslateZ(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetTranslateZ(), floatData[1]));
}

/**
 * @tc.name: SetandGetTranslateZ02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetTranslateZ02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetTranslateZ(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetTranslateZ(), floatData[2]));
}

/**
 * @tc.name: SetandGetTranslateZ03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetTranslateZ03, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetTranslateZ(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetTranslateZ(), floatData[3]));
}

/**
 * @tc.name: SetandGetTranslateZ04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetTranslateZ04, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetTranslateZ(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetTranslateZ(), floatData[4]));
}

/**
 * @tc.name: SetandGetTranslateZ05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetTranslateZ05, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetTranslateZ(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(canvasnode->GetStagingProperties().GetTranslateZ(), floatData[0]));
}

/**
 * @tc.name: SetandGetClipToBounds01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetClipToBounds01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetClipToBounds(true);
    ASSERT_TRUE(canvasnode != nullptr);
}

/**
 * @tc.name: SetandGetClipToBounds02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetClipToBounds02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetClipToBounds(false);
    ASSERT_TRUE(canvasnode != nullptr);
}

/**
 * @tc.name: SetandGetClipToFrame01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetClipToFrame01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetClipToFrame(true);
    ASSERT_TRUE(canvasnode != nullptr);
}

/**
 * @tc.name: SetandGetClipToFrame02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetClipToFrame02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetClipToFrame(false);
    ASSERT_TRUE(canvasnode != nullptr);
}

/**
 * @tc.name: SetandGetVisible01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetVisible01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetVisible(true);
    ASSERT_TRUE(canvasnode != nullptr);
}

/**
 * @tc.name: SetandGetVisible02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetVisible02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetVisible(false);
    ASSERT_TRUE(canvasnode != nullptr);
}

/**
 * @tc.name: SetandGetBorderStyle01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBorderStyle01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    auto borderStyle = static_cast<uint32_t>(BorderStyle::SOLID);
    canvasnode->SetBorderStyle(static_cast<uint32_t>(borderStyle));
    EXPECT_TRUE(canvasnode->GetStagingProperties().GetBorderStyle() == borderStyle);
}

/**
 * @tc.name: SetandGetBorderStyle02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBorderStyle02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    auto borderStyle = static_cast<uint32_t>(BorderStyle::DASHED);
    canvasnode->SetBorderStyle(static_cast<uint32_t>(borderStyle));
    EXPECT_TRUE(canvasnode->GetStagingProperties().GetBorderStyle() == borderStyle);
}

/**
 * @tc.name: SetandGetBorderStyle03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBorderStyle03, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    auto borderStyle = static_cast<uint32_t>(BorderStyle::DOTTED);
    canvasnode->SetBorderStyle(static_cast<uint32_t>(borderStyle));
    EXPECT_TRUE(canvasnode->GetStagingProperties().GetBorderStyle() == borderStyle);
}

/**
 * @tc.name: SetandGetBorderStyle04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBorderStyle04, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    auto borderStyle = static_cast<uint32_t>(BorderStyle::NONE);
    canvasnode->SetBorderStyle(static_cast<uint32_t>(borderStyle));
    EXPECT_TRUE(canvasnode->GetStagingProperties().GetBorderStyle() == borderStyle);
}

/**
 * @tc.name: SetandGetFrameGravity01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetFrameGravity01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    Gravity gravity = Gravity::CENTER;
    canvasnode->SetFrameGravity(gravity);
    EXPECT_TRUE(canvasnode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetFrameGravity02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    Gravity gravity = Gravity::TOP;
    canvasnode->SetFrameGravity(gravity);
    EXPECT_TRUE(canvasnode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetFrameGravity03, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    Gravity gravity = Gravity::BOTTOM;
    canvasnode->SetFrameGravity(gravity);
    EXPECT_TRUE(canvasnode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetFrameGravity04, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    Gravity gravity = Gravity::LEFT;
    canvasnode->SetFrameGravity(gravity);
    EXPECT_TRUE(canvasnode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetFrameGravity05, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    Gravity gravity = Gravity::RIGHT;
    canvasnode->SetFrameGravity(gravity);
    EXPECT_TRUE(canvasnode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity06
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetFrameGravity06, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    Gravity gravity = Gravity::TOP_LEFT;
    canvasnode->SetFrameGravity(gravity);
    EXPECT_TRUE(canvasnode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity07
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetFrameGravity07, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    Gravity gravity = Gravity::TOP_RIGHT;
    canvasnode->SetFrameGravity(gravity);
    EXPECT_TRUE(canvasnode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity08
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetFrameGravity08, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    Gravity gravity = Gravity::BOTTOM_LEFT;
    canvasnode->SetFrameGravity(gravity);
    EXPECT_TRUE(canvasnode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity09
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetFrameGravity09, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    Gravity gravity = Gravity::BOTTOM_RIGHT;
    canvasnode->SetFrameGravity(gravity);
    EXPECT_TRUE(canvasnode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity010
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetFrameGravity010, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    Gravity gravity = Gravity::RESIZE;
    canvasnode->SetFrameGravity(gravity);
    EXPECT_TRUE(canvasnode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity011
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetFrameGravity011, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    Gravity gravity = Gravity::RESIZE_ASPECT;
    canvasnode->SetFrameGravity(gravity);
    EXPECT_TRUE(canvasnode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity012
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetFrameGravity012, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    Gravity gravity = Gravity::RESIZE_ASPECT_FILL;
    canvasnode->SetFrameGravity(gravity);
    EXPECT_TRUE(canvasnode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: NotifyTransition01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, NotifyTransition01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->NotifyTransition(RSTransitionEffect::Create(), true);
    ASSERT_TRUE(canvasnode != nullptr);
}

/**
 * @tc.name: NotifyTransition02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, NotifyTransition02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->NotifyTransition(RSTransitionEffect::Create(), false);
    ASSERT_TRUE(canvasnode != nullptr);
}

/**
 * @tc.name: SetandGetForegroundColor01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetForegroundColor01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = 0x034123;
    canvasnode->SetForegroundColor(colorValue);
    EXPECT_TRUE(canvasnode->GetStagingProperties().GetForegroundColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetForegroundColor02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetForegroundColor02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::max();
    canvasnode->SetForegroundColor(colorValue);
    EXPECT_TRUE(canvasnode->GetStagingProperties().GetForegroundColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetForegroundColor03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetForegroundColor03, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::min();
    canvasnode->SetForegroundColor(colorValue);
    EXPECT_TRUE(canvasnode->GetStagingProperties().GetForegroundColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetBackgroundColor01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBackgroundColor01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = 0x034123;
    canvasnode->SetBackgroundColor(colorValue);
    EXPECT_TRUE(canvasnode->GetStagingProperties().GetBackgroundColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetBackgroundColor02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBackgroundColor02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::max();
    canvasnode->SetBackgroundColor(colorValue);
    EXPECT_TRUE(canvasnode->GetStagingProperties().GetBackgroundColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetBackgroundColor03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBackgroundColor03, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::min();
    canvasnode->SetBackgroundColor(colorValue);
    EXPECT_TRUE(canvasnode->GetStagingProperties().GetBackgroundColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetBorderColor01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBorderColor01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = 0x034123;
    canvasnode->SetBorderColor(colorValue);
    EXPECT_TRUE(canvasnode->GetStagingProperties().GetBorderColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetBorderColor02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBorderColor02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::max();
    canvasnode->SetBorderColor(colorValue);
    EXPECT_TRUE(canvasnode->GetStagingProperties().GetBorderColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetBorderColor03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetBorderColor03, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::min();
    canvasnode->SetBorderColor(colorValue);
    EXPECT_TRUE(canvasnode->GetStagingProperties().GetBorderColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetRotationVector01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetRotationVector01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    Vector4f quaternion(std::numeric_limits<int>::min(), 2.f, 3.f, 4.f);
    canvasnode->SetRotation(quaternion);
    ASSERT_TRUE(canvasnode != nullptr);
}

/**
 * @tc.name: SetandGetTranslateVector01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetTranslateVector01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    Vector2f quaternion(std::numeric_limits<int>::max(), 2.f);
    canvasnode->SetTranslate(quaternion);
    ASSERT_TRUE(canvasnode != nullptr);
}

/**
 * @tc.name: CreateBlurFilter01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, CreateBlurFilter01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> backgroundFilter = RSFilter::CreateBlurFilter(floatData[0], floatData[1]);
    canvasnode->SetBackgroundFilter(backgroundFilter);
    EXPECT_TRUE(canvasnode->GetStagingProperties().GetBackgroundBlurRadiusX() == floatData[0]);
    EXPECT_TRUE(canvasnode->GetStagingProperties().GetBackgroundBlurRadiusY() == floatData[1]);
}

/**
 * @tc.name: CreateBlurFilter02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, CreateBlurFilter02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> backgroundFilter = RSFilter::CreateBlurFilter(floatData[1], floatData[2]);
    canvasnode->SetBackgroundFilter(backgroundFilter);
    EXPECT_TRUE(canvasnode->GetStagingProperties().GetBackgroundBlurRadiusX() == floatData[1]);
    EXPECT_TRUE(canvasnode->GetStagingProperties().GetBackgroundBlurRadiusY() == floatData[2]);
}

/**
 * @tc.name: CreateBlurFilter03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, CreateBlurFilter03, TestSize.Level3)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> backgroundFilter = RSFilter::CreateBlurFilter(floatData[2], floatData[3]);
    canvasnode->SetBackgroundFilter(backgroundFilter);
    EXPECT_TRUE(canvasnode->GetStagingProperties().GetBackgroundBlurRadiusX() == floatData[2]);
    EXPECT_TRUE(canvasnode->GetStagingProperties().GetBackgroundBlurRadiusY() == floatData[3]);
}

/**
 * @tc.name: CreateBlurFilter04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, CreateBlurFilter04, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> backgroundFilter = RSFilter::CreateBlurFilter(floatData[3], floatData[4]);
    canvasnode->SetBackgroundFilter(backgroundFilter);
    EXPECT_TRUE(canvasnode->GetStagingProperties().GetBackgroundBlurRadiusX() == floatData[3]);
    EXPECT_TRUE(canvasnode->GetStagingProperties().GetBackgroundBlurRadiusY() == floatData[4]);
}

/**
 * @tc.name: CreateBlurFilter05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, CreateBlurFilter05, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> backgroundFilter = RSFilter::CreateBlurFilter(floatData[4], floatData[0]);
    canvasnode->SetBackgroundFilter(backgroundFilter);
    EXPECT_TRUE(canvasnode->GetStagingProperties().GetBackgroundBlurRadiusX() == floatData[4]);
    EXPECT_TRUE(canvasnode->GetStagingProperties().GetBackgroundBlurRadiusY() == floatData[0]);
}

/**
 * @tc.name: CreateNormalFilter01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, CreateNormalFilter01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(floatData[0], floatData[1]);
    canvasnode->SetFilter(filter);
    EXPECT_TRUE(canvasnode->GetStagingProperties().GetForegroundBlurRadiusX() == floatData[0]);
    EXPECT_TRUE(canvasnode->GetStagingProperties().GetForegroundBlurRadiusY() == floatData[1]);
}

/**
 * @tc.name: CreateNormalFilter02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, CreateNormalFilter02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(floatData[1], floatData[2]);
    canvasnode->SetFilter(filter);
    EXPECT_TRUE(canvasnode->GetStagingProperties().GetForegroundBlurRadiusX() == floatData[1]);
    EXPECT_TRUE(canvasnode->GetStagingProperties().GetForegroundBlurRadiusY() == floatData[2]);
}

/**
 * @tc.name: CreateNormalFilter03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, CreateNormalFilter03, TestSize.Level3)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(floatData[2], floatData[3]);
    canvasnode->SetFilter(filter);
    EXPECT_TRUE(canvasnode->GetStagingProperties().GetForegroundBlurRadiusX() == floatData[2]);
    EXPECT_TRUE(canvasnode->GetStagingProperties().GetForegroundBlurRadiusY() == floatData[3]);
}

/**
 * @tc.name: CreateNormalFilter04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, CreateNormalFilter04, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(floatData[3], floatData[4]);
    canvasnode->SetFilter(filter);
    EXPECT_TRUE(canvasnode->GetStagingProperties().GetForegroundBlurRadiusX() == floatData[3]);
    EXPECT_TRUE(canvasnode->GetStagingProperties().GetForegroundBlurRadiusY() == floatData[4]);
}

/**
 * @tc.name: CreateNormalFilter05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, CreateNormalFilter05, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(floatData[4], floatData[0]);
    canvasnode->SetFilter(filter);
    EXPECT_TRUE(canvasnode->GetStagingProperties().GetForegroundBlurRadiusX() == floatData[4]);
    EXPECT_TRUE(canvasnode->GetStagingProperties().GetForegroundBlurRadiusY() == floatData[0]);
}

/**
 * @tc.name: SetandGetClipBounds01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetandGetClipBounds01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    std::shared_ptr<RSPath> clipPath = RSPath::CreateRSPath();
    canvasnode->SetClipBounds(clipPath);
    EXPECT_TRUE(canvasnode->GetStagingProperties().GetClipBounds() == clipPath);
}

/**
 * @tc.name: GetId01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, GetId01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->GetId();
    ASSERT_TRUE(canvasnode != nullptr);
}

/**
 * @tc.name: GetStagingProperties01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, GetStagingProperties01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->GetStagingProperties();
    ASSERT_TRUE(canvasnode != nullptr);
}

/**
 * @tc.name: GetMotionPathOption01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, GetMotionPathOption02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->GetMotionPathOption();
    ASSERT_TRUE(canvasnode != nullptr);
}

/**
 * @tc.name: SetBgImage01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetBgImage01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    auto image = std::make_shared<RSImage>();
    canvasnode->SetBgImage(image);
    ASSERT_TRUE(canvasnode != nullptr);
}

/**
 * @tc.name: SetBackgroundShader01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetBackgroundShader01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    auto shader = RSShader::CreateRSShader();
    canvasnode->SetBackgroundShader(shader);
    ASSERT_TRUE(canvasnode != nullptr);
}

/**
 * @tc.name: SetCompositingFilter01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetCompositingFilter01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    auto compositingFilter = RSFilter::CreateBlurFilter(0.0f, 0.0f);
    canvasnode->SetCompositingFilter(compositingFilter);
    ASSERT_TRUE(canvasnode != nullptr);
}

/**
 * @tc.name: SetShadowPath01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, SetShadowPath01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    auto shadowpath = RSPath::CreateRSPath();
    canvasnode->SetShadowPath(shadowpath);
    ASSERT_TRUE(canvasnode != nullptr);
}

/**
 * @tc.name: GetType01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, GetType01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    ASSERT_TRUE(canvasnode != nullptr);
    ASSERT_TRUE(canvasnode->GetType() == RSUINodeType::CANVAS_NODE);
}

/**
 * @tc.name: GetType02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, GetType02, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create(true);
    ASSERT_TRUE(canvasnode != nullptr);
    ASSERT_TRUE(canvasnode->GetType() == RSUINodeType::CANVAS_NODE);
}

/**
 * @tc.name: GetType03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RsCanvasNodesTest, GetType03, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create(false);
    ASSERT_TRUE(canvasnode != nullptr);
    ASSERT_TRUE(canvasnode->GetType() == RSUINodeType::CANVAS_NODE);
}

/**
 * @tc.name: SetBoundsChangedCallbackTest
 * @tc.desc: test results of SetBoundsChangedCallback
 * @tc.type:FUNC
 * @tc.require:issueI9MWJR
 */
HWTEST_F(RsCanvasNodesTest, SetBoundsChangedCallbackTest, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create(false);
    ASSERT_TRUE(canvasnode != nullptr);
    canvasnode->DrawOnNode(RSModifierType::BOUNDS, [](std::shared_ptr<Drawing::Canvas>) {});
    ASSERT_FALSE(canvasnode->recordingUpdated_);
    canvasnode->SetHDRPresent(true);
    canvasnode->SetBoundsChangedCallback([](const Rosen::Vector4f& vector4f) {});
    ASSERT_NE(canvasnode->boundsChangedCallback_, nullptr);
}

/**
 * @tc.name: CreateRenderNodeForTextureExportSwitch
 * @tc.desc: test results of CreateRenderNodeForTextureExportSwitch
 * @tc.type: FUNC
 * @tc.require: issueI9KDPI
 */
HWTEST_F(RsCanvasNodesTest, CreateRenderNodeForTextureExportSwitch, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->CreateRenderNodeForTextureExportSwitch();
    ASSERT_TRUE(RSTransactionProxy::instance_ != nullptr);
}

/**
 * @tc.name: DrawOnNode
 * @tc.desc: test results of DrawOnNode
 * @tc.type: FUNC
 * @tc.require: issueI9KDPI
 */
HWTEST_F(RsCanvasNodesTest, DrawOnNode, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    RSModifierType type = RSModifierType::INVALID;
    DrawFunc func = [&](std::shared_ptr<Drawing::Canvas>) {};
    delete RSTransactionProxy::instance_;
    RSTransactionProxy::instance_ = nullptr;
    canvasnode->DrawOnNode(type, func);
    EXPECT_TRUE(RSTransactionProxy::instance_ == nullptr);

    RSTransactionProxy::instance_ = new RSTransactionProxy();
    canvasnode->DrawOnNode(type, func);
    EXPECT_TRUE(RSTransactionProxy::instance_ != nullptr);
}

/**
 * @tc.name: GetPaintWidth
 * @tc.desc: test results of GetPaintWidth
 * @tc.type: FUNC
 * @tc.require: issueI9KDPI
 */
HWTEST_F(RsCanvasNodesTest, GetPaintWidth, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    float res = canvasnode->GetPaintWidth();
    EXPECT_FALSE(res == 1.f);
}

/**
 * @tc.name: GetPaintHeight
 * @tc.desc: test results of GetPaintHeight
 * @tc.type: FUNC
 * @tc.require: issueI9KDPI
 */
HWTEST_F(RsCanvasNodesTest, GetPaintHeight, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    float res = canvasnode->GetPaintHeight();
    EXPECT_FALSE(res == 1.f);
}

/**
 * @tc.name: SetFreeze
 * @tc.desc: test results of SetFreeze
 * @tc.type: FUNC
 * @tc.require: issueI9KDPI
 */
HWTEST_F(RsCanvasNodesTest, SetFreeze, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetFreeze(true);
    EXPECT_TRUE(RSTransactionProxy::instance_ != nullptr);
}

/**
 * @tc.name: OnBoundsSizeChanged
 * @tc.desc: test results of OnBoundsSizeChanged
 * @tc.type: FUNC
 * @tc.require: issueI9KDPI
 */
HWTEST_F(RsCanvasNodesTest, OnBoundsSizeChanged, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->OnBoundsSizeChanged();
    EXPECT_TRUE(RSTransactionProxy::instance_ != nullptr);
}

/**
 * @tc.name: SetBoundsChangedCallback
 * @tc.desc: test results of SetBoundsChangedCallback
 * @tc.type: FUNC
 * @tc.require: issueI9KDPI
 */
HWTEST_F(RsCanvasNodesTest, SetBoundsChangedCallback, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->boundsChangedCallback_ = [](const Rosen::Vector4f& bounds) {};
    canvasnode->SetBoundsChangedCallback(canvasnode->boundsChangedCallback_);
    EXPECT_TRUE(RSTransactionProxy::instance_ != nullptr);
}

/**
 * @tc.name: BeginRecording01
 * @tc.desc: test results of BeginRecording
 * @tc.type: FUNC
 * @tc.require: issueI9R0EY
 */
HWTEST_F(RsCanvasNodesTest, BeginRecording01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->BeginRecording(20, 30);
    EXPECT_TRUE(RSTransactionProxy::instance_ != nullptr);

    canvasnode->recordingUpdated_ = true;
    canvasnode->BeginRecording(20, 30);
    EXPECT_TRUE(!canvasnode->recordingUpdated_);

    delete RSTransactionProxy::instance_;
    RSTransactionProxy::instance_ = nullptr;
    canvasnode->BeginRecording(20, 30);
    EXPECT_TRUE(RSTransactionProxy::instance_ == nullptr);
    RSTransactionProxy::instance_ = new RSTransactionProxy();
    EXPECT_TRUE(RSTransactionProxy::instance_ != nullptr);
}

/**
 * @tc.name: CreateTextureExportRenderNodeInRT01
 * @tc.desc: test results of CreateRenderNodeForTextureExportSwitch
 * @tc.type: FUNC
 * @tc.require: issueI9R0EY
 */
HWTEST_F(RsCanvasNodesTest, CreateTextureExportRenderNodeInRT01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->CreateRenderNodeForTextureExportSwitch();
    EXPECT_TRUE(RSTransactionProxy::instance_ != nullptr);
}

/**
 * @tc.name: FinishRecording01
 * @tc.desc: test results of FinishRecording
 * @tc.type: FUNC
 * @tc.require: issueI9R0EY
 */
HWTEST_F(RsCanvasNodesTest, FinishRecording01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->FinishRecording();
    EXPECT_TRUE(RSTransactionProxy::instance_ != nullptr);

    canvasnode->BeginRecording(20, 30);
    canvasnode->recordingCanvas_->cmdList_ =
        std::make_shared<Drawing::DrawCmdList>(Drawing::DrawCmdList::UnmarshalMode::DEFERRED);
    canvasnode->FinishRecording();
    EXPECT_TRUE(RSTransactionProxy::instance_ != nullptr);

    canvasnode->BeginRecording(20, 30);
    canvasnode->recordingCanvas_->cmdList_ =
        std::make_shared<Drawing::DrawCmdList>(Drawing::DrawCmdList::UnmarshalMode::IMMEDIATE);
    Drawing::Paint paint;
    auto item = std::make_shared<Drawing::DrawWithPaintOpItem>(paint, 0);
    canvasnode->recordingCanvas_->cmdList_->drawOpItems_.push_back(item);
    canvasnode->FinishRecording();
    EXPECT_TRUE(canvasnode->recordingUpdated_);

    canvasnode->BeginRecording(20, 30);
    delete RSTransactionProxy::instance_;
    RSTransactionProxy::instance_ = nullptr;
    canvasnode->FinishRecording();
    EXPECT_TRUE(RSTransactionProxy::instance_ == nullptr);
    RSTransactionProxy::instance_ = new RSTransactionProxy();
    EXPECT_TRUE(RSTransactionProxy::instance_ != nullptr);

    canvasnode->BeginRecording(20, 30);
    canvasnode->recordingCanvas_->cmdList_ = nullptr;
    canvasnode->FinishRecording();
    EXPECT_TRUE(RSTransactionProxy::instance_ != nullptr);
}

/**
 * @tc.name: DrawOnNode01
 * @tc.desc: test results of DrawOnNode
 * @tc.type: FUNC
 * @tc.require: issueI9R0EY
 */
HWTEST_F(RsCanvasNodesTest, DrawOnNode01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    DrawFunc func = [&](std::shared_ptr<Drawing::Canvas>) {};
    canvasnode->DrawOnNode(RSModifierType::INVALID, func);
    EXPECT_TRUE(RSTransactionProxy::instance_ != nullptr);

    canvasnode->BeginRecording(20, 30);
    delete RSTransactionProxy::instance_;
    RSTransactionProxy::instance_ = nullptr;
    canvasnode->DrawOnNode(RSModifierType::INVALID, func);
    EXPECT_TRUE(RSTransactionProxy::instance_ == nullptr);
    RSTransactionProxy::instance_ = new RSTransactionProxy();
    EXPECT_TRUE(RSTransactionProxy::instance_ != nullptr);
}

/**
 * @tc.name: SetFreeze01
 * @tc.desc: test results of SetFreeze
 * @tc.type: FUNC
 * @tc.require: issueI9R0EY
 */
HWTEST_F(RsCanvasNodesTest, SetFreeze01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetFreeze(true);
    EXPECT_TRUE(RSTransactionProxy::instance_ != nullptr);

    RSSystemProperties::GetUniRenderEnabled();
    RSSystemProperties::isUniRenderEnabled_ = true;
    canvasnode->SetFreeze(true);
    ASSERT_TRUE(RSSystemProperties::isUniRenderEnabled_);

    delete RSTransactionProxy::instance_;
    RSTransactionProxy::instance_ = nullptr;
    canvasnode->SetFreeze(true);
    ASSERT_TRUE(RSSystemProperties::isUniRenderEnabled_);
    RSTransactionProxy::instance_ = new RSTransactionProxy();
}

/**
 * @tc.name: SetHDRPresent01
 * @tc.desc: test results of SetHDRPresent
 * @tc.type: FUNC
 * @tc.require: issueI9R0EY
 */
HWTEST_F(RsCanvasNodesTest, SetHDRPresent01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->SetHDRPresent(true);
    EXPECT_TRUE(RSTransactionProxy::instance_ != nullptr);

    canvasnode->SetHDRPresent(true);
    EXPECT_TRUE(RSTransactionProxy::instance_ != nullptr);

    canvasnode->SetHDRPresent(false);
    EXPECT_TRUE(RSTransactionProxy::instance_ != nullptr);

    canvasnode->SetHDRPresent(false);
    EXPECT_TRUE(RSTransactionProxy::instance_ != nullptr);
}

/**
 * @tc.name: OnBoundsSizeChanged01
 * @tc.desc: test results of OnBoundsSizeChanged
 * @tc.type: FUNC
 * @tc.require: issueI9R0EY
 */
HWTEST_F(RsCanvasNodesTest, OnBoundsSizeChanged01, TestSize.Level2)
{
    RSCanvasNode::SharedPtr canvasnode = RSCanvasNode::Create();
    canvasnode->OnBoundsSizeChanged();
    EXPECT_TRUE(RSTransactionProxy::instance_ != nullptr);

    canvasnode->boundsChangedCallback_ = [](const Rosen::Vector4f& bounds) {};
    canvasnode->OnBoundsSizeChanged();
    EXPECT_TRUE(RSTransactionProxy::instance_ != nullptr);
}
} // namespace OHOS::Rosen
