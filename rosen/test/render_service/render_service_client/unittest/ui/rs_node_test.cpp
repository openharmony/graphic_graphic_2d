/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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
#include "gtest/gtest.h"
#include "animation/rs_animation.h"
#include "animation/rs_transition.h"
#include "modifier/rs_property_modifier.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_surface_node.h"
#include "animation/rs_animation_callback.h"
#include "animation/rs_implicit_animator_map.h"
#include "animation/rs_implicit_animator.h"
#include "animation/rs_implicit_animation_param.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
constexpr static float FLOAT_DATA_ZERO = 0.0f;
constexpr static float FLOAT_DATA_POSITIVE = 485.44f;
constexpr static float FLOAT_DATA_NEGATIVE = -34.4f;
constexpr static float FLOAT_DATA_MAX = std::numeric_limits<float>::max();
constexpr static float FLOAT_DATA_MIN = std::numeric_limits<float>::min();
constexpr static float FLOAT_DATA_INIT = 0.5f;
constexpr static float FLOAT_DATA_UPDATE = 1.0f;

class RSNodeTest : public testing::Test {
public:
    constexpr static float floatData[] = {
        0.0f, 485.44f, -34.4f,
        std::numeric_limits<float>::max(), std::numeric_limits<float>::min(),
        };
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSNodeTest::SetUpTestCase() {}
void RSNodeTest::TearDownTestCase() {}
void RSNodeTest::SetUp() {}
void RSNodeTest::TearDown() {}

/**
 * @tc.name: LifeCycle001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, LifeCycle001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSNode and add child
     */
    auto rootNode = RSCanvasNode::Create();
    ASSERT_TRUE(rootNode != nullptr);

    auto child1 = RSCanvasNode::Create();
    auto child2 = RSCanvasNode::Create();
    auto child3 = RSCanvasNode::Create();
    rootNode->AddChild(child1, -1);
    rootNode->AddChild(child2, 0);
    child1->AddChild(child3, 1);

    EXPECT_EQ(rootNode->GetId() + 1, child1->GetId());

    /**
     * @tc.steps: step2. remove child
     */
    rootNode->RemoveChild(child2);
}

/**
 * @tc.name: LifeCycle002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, LifeCycle002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSNode and add child
     */
    auto rootNode = RSCanvasNode::Create();
    ASSERT_TRUE(rootNode != nullptr);

    auto child1 = RSCanvasNode::Create();
    auto child2 = RSCanvasNode::Create();
    auto child3 = RSCanvasNode::Create();
    rootNode->AddChild(child1, -1);
    rootNode->AddChild(child2, 0);
    child1->AddChild(child3, 1);
    /**
     * @tc.steps: step2. remove child
     */
    rootNode->RemoveChild(child2);
}

/**
 * @tc.name: LifeCycle003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, LifeCycle003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSNode and add child
     */
    auto rootNode = RSCanvasNode::Create();
    ASSERT_TRUE(rootNode != nullptr);

    auto child1 = RSCanvasNode::Create();
    auto child2 = RSCanvasNode::Create();
    auto child3 = RSCanvasNode::Create();
    rootNode->AddChild(child1, -1);
    rootNode->AddChild(child2, 0);
    child1->AddChild(child3, 1);
    /**
     * @tc.steps: step2. remove child
     */
    rootNode->RemoveFromTree();
}

/**
 * @tc.name: LifeCycle004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, LifeCycle004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSNode and add child
     */
    auto rootNode = RSCanvasNode::Create();
    ASSERT_TRUE(rootNode != nullptr);

    auto child1 = RSCanvasNode::Create();
    auto child2 = RSCanvasNode::Create();
    auto child3 = RSCanvasNode::Create();
    rootNode->AddChild(child1, -1);
    rootNode->AddChild(child2, 0);
    child1->AddChild(child3, 1);
    /**
     * @tc.steps: step2. remove child
     */
    rootNode->ClearChildren();
}

/**
 * @tc.name: LifeCycle005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, LifeCycle005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSNode and add child
     */
    auto rootNode = RSCanvasNode::Create();
    ASSERT_TRUE(rootNode != nullptr);

    auto child1 = RSCanvasNode::Create();
    auto child2 = RSCanvasNode::Create();
    auto child3 = RSCanvasNode::Create();
    rootNode->AddChild(child1, -1);
    rootNode->AddChild(child2, 0);
    child1->AddChild(child3, 1);
    /**
     * @tc.steps: step2. remove child
     */
    rootNode->RemoveChild(child3);
}

/**
 * @tc.name: Recording001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, Recording001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSNode and RSUIDirector
     */
    auto rsNode = RSCanvasNode::Create();

    /**
     * @tc.steps: step2. begin recording
     */
    EXPECT_FALSE(rsNode->IsRecording());
    rsNode->BeginRecording(500, 400);
    EXPECT_TRUE(rsNode->IsRecording());
}

/**
 * @tc.name: SetPaintOrder001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetPaintOrder001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSNode and RSUIDirector
     */
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetPaintOrder(true);
}

/**
 * @tc.name: SetandGetBounds001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetBounds001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBounds(floatData[0], floatData[1], floatData[2], floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBounds().x_, floatData[0]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBounds().y_, floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBounds().z_, floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBounds().w_, floatData[3]));
}

/**
 * @tc.name: SetandGetBounds002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetBounds002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBounds(floatData[3], floatData[1], floatData[2], floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBounds().x_, floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBounds().y_, floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBounds().z_, floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBounds().w_, floatData[0]));
}

/**
 * @tc.name: SetandGetBounds003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetBounds003, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBounds(floatData[3], floatData[2], floatData[1], floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBounds().x_, floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBounds().y_, floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBounds().z_, floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBounds().w_, floatData[0]));
}

/**
 * @tc.name: SetandGetBoundsWidth001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetBoundsWidth001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBoundsWidth(floatData[1]);
    rsNode->SetBoundsWidth(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBounds().z_, floatData[1]));
}

/**
 * @tc.name: SetandGetBoundsWidth002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetBoundsWidth002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBoundsWidth(floatData[2]);
    rsNode->SetBoundsWidth(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBounds().z_, floatData[2]));
}

/**
 * @tc.name: SetandGetBoundsWidth003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetBoundsWidth003, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBoundsWidth(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBounds().z_, floatData[3]));
}

/**
 * @tc.name: SetIlluminatedBorderWidthTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetIlluminatedBorderWidthTest, TestSize.Level1)
{
    auto rootNode = RSCanvasNode::Create();
    rootNode->SetIlluminatedBorderWidth(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rootNode->GetStagingProperties().GetIlluminatedBorderWidth(), floatData[1]));
}

/**
 * @tc.name: SetandGetBoundsWidth004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetBoundsWidth004, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBoundsWidth(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBounds().z_, floatData[4]));
}

/**
 * @tc.name: SetandGetBoundsWidth005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetBoundsWidth005, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBoundsWidth(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBounds().z_, floatData[0]));
}

/**
 * @tc.name: SetandGetBoundsHeight001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetBoundsHeight001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBoundsHeight(floatData[1]);
    rsNode->SetBoundsHeight(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBounds().w_, floatData[1]));
}

/**
 * @tc.name: SetandGetBoundsHeight002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetBoundsHeight002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBoundsHeight(floatData[2]);
    rsNode->SetBoundsHeight(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBounds().w_, floatData[2]));
}

/**
 * @tc.name: SetandGetBoundsHeight003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetBoundsHeight003, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBoundsHeight(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBounds().w_, floatData[3]));
}

/**
 * @tc.name: SetandGetBoundsHeight004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetBoundsHeight004, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBoundsHeight(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBounds().w_, floatData[4]));
}

/**
 * @tc.name: SetandGetBoundsHeight005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetBoundsHeight005, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBoundsHeight(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBounds().w_, floatData[0]));
}

/**
 * @tc.name: SetandGetFrame001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetFrame001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetFrame(floatData[0], floatData[1], floatData[2], floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrame().x_, floatData[0]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrame().y_, floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrame().z_, floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrame().w_, floatData[3]));
}

/**
 * @tc.name: SetandGetFrame002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetFrame002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetFrame(floatData[3], floatData[1], floatData[2], floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrame().x_, floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrame().y_, floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrame().z_, floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrame().w_, floatData[0]));
}

/**
 * @tc.name: SetandGetFrame003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetFrame003, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetFrame(floatData[3], floatData[2], floatData[1], floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrame().x_, floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrame().y_, floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrame().z_, floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrame().w_, floatData[0]));
}

/**
 * @tc.name: SetandGetFramePositionX001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetFramePositionX001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetFramePositionX(floatData[1]);
    rsNode->SetFramePositionX(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrame().x_, floatData[1]));
}

/**
 * @tc.name: SetandGetFramePositionX002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetFramePositionX002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetFramePositionX(floatData[2]);
    rsNode->SetFramePositionX(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrame().x_, floatData[2]));
}

/**
 * @tc.name: SetandGetFramePositionX003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetFramePositionX003, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetFramePositionX(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrame().x_, floatData[3]));
}

/**
 * @tc.name: SetandGetFramePositionX004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetFramePositionX004, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetFramePositionX(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrame().x_, floatData[4]));
}

/**
 * @tc.name: SetandGetFramePositionX005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetFramePositionX005, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetFramePositionX(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrame().x_, floatData[0]));
}

/**
 * @tc.name: SetandGetFramePositionY001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetFramePositionY001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetFramePositionY(floatData[1]);
    rsNode->SetFramePositionY(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrame().y_, floatData[1]));
}

/**
 * @tc.name: SetandGetFramePositionY002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetFramePositionY002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetFramePositionY(floatData[2]);
    rsNode->SetFramePositionY(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrame().y_, floatData[2]));
}

/**
 * @tc.name: SetandGetFramePositionY003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetFramePositionY003, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetFramePositionY(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrame().y_, floatData[3]));
}

/**
 * @tc.name: SetandGetFramePositionY004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetFramePositionY004, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetFramePositionY(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrame().y_, floatData[4]));
}

/**
 * @tc.name: SetandGetFramePositionY005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetFramePositionY005, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetFramePositionY(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrame().y_, floatData[0]));
}

/**
 * @tc.name: SetandGetPositionZ001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetPositionZ001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetPositionZ(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPositionZ(), floatData[1]));
}

/**
 * @tc.name: SetandGetPositionZ002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetPositionZ002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetPositionZ(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPositionZ(), floatData[2]));
}

/**
 * @tc.name: SetandGetPositionZ003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetPositionZ003, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetPositionZ(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPositionZ(), floatData[3]));
}

/**
 * @tc.name: SetandGetPositionZ004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetPositionZ004, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetPositionZ(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPositionZ(), floatData[4]));
}

/**
 * @tc.name: SetandGetPositionZ005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetPositionZ005, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetPositionZ(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPositionZ(), floatData[0]));
}

/**
 * @tc.name: SetandGetCornerRadius001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetCornerRadius001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetCornerRadius(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetCornerRadius().x_, floatData[1]));
}

/**
 * @tc.name: SetandGetCornerRadius002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetCornerRadius002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetCornerRadius(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetCornerRadius().x_, floatData[2]));
}

/**
 * @tc.name: SetandGetCornerRadius003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetCornerRadius003, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetCornerRadius(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetCornerRadius().x_, floatData[3]));
}

/**
 * @tc.name: SetandGetCornerRadius004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetCornerRadius004, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetCornerRadius(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetCornerRadius().x_, floatData[4]));
}

/**
 * @tc.name: SetandGetCornerRadius005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetCornerRadius005, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetCornerRadius(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetCornerRadius().x_, floatData[0]));
}

/**
 * @tc.name: SetandGetOutlineRadius001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetOutlineRadius001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetOutlineRadius(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetOutlineRadius().x_, floatData[1]));
}

/**
 * @tc.name: SetandGetOutlineRadius002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetOutlineRadius002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetOutlineRadius(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetOutlineRadius().x_, floatData[2]));
}

/**
 * @tc.name: SetandGetOutlineRadius003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetOutlineRadius003, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetOutlineRadius(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetOutlineRadius().x_, floatData[3]));
}

/**
 * @tc.name: SetandGetOutlineRadius004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetOutlineRadius004, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetOutlineRadius(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetOutlineRadius().x_, floatData[4]));
}

/**
 * @tc.name: SetandGetOutlineRadius005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetOutlineRadius005, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetOutlineRadius(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetOutlineRadius().x_, floatData[0]));
}

/**
 * @tc.name: SetandGetRotationThree001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetRotationThree001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetRotation(floatData[1], floatData[2], floatData[3]);
}

/**
 * @tc.name: SetandGetRotation001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetRotation001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetRotation(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetRotation(), floatData[1]));
}

/**
 * @tc.name: SetandGetRotation002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetRotation002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetRotation(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetRotation(), floatData[2]));
}

/**
 * @tc.name: SetandGetRotation003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetRotation003, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetRotation(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetRotation(), floatData[3]));
}

/**
 * @tc.name: SetandGetRotation004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetRotation004, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetRotation(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetRotation(), floatData[4]));
}

/**
 * @tc.name: SetandGetRotation005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetRotation005, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetRotation(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetRotation(), floatData[0]));
}

/**
 * @tc.name: SetandGetRotationX001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetRotationX001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetRotationX(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetRotationX(), floatData[1]));
}

/**
 * @tc.name: SetandGetRotationX002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetRotationX002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetRotationX(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetRotationX(), floatData[2]));
}

/**
 * @tc.name: SetandGetRotationX003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetRotationX003, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetRotationX(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetRotationX(), floatData[3]));
}

/**
 * @tc.name: SetandGetRotationX004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetRotationX004, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetRotationX(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetRotationX(), floatData[4]));
}

/**
 * @tc.name: SetandGetRotationX005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetRotationX005, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetRotationX(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetRotationX(), floatData[0]));
}

/**
 * @tc.name: SetandGetRotationY001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetRotationY001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetRotationY(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetRotationY(), floatData[1]));
}

/**
 * @tc.name: SetandGetRotationY002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetRotationY002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetRotationY(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetRotationY(), floatData[2]));
}

/**
 * @tc.name: SetandGetRotationY003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetRotationY003, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetRotationY(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetRotationY(), floatData[3]));
}

/**
 * @tc.name: SetandGetRotationY004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetRotationY004, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetRotationY(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetRotationY(), floatData[4]));
}

/**
 * @tc.name: SetandGetRotationY005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetRotationY005, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetRotationY(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetRotationY(), floatData[0]));
}


/**
 * @tc.name: SetandGetScaleX001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetScaleX001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetScaleX(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScale().x_, floatData[1]));
}

/**
 * @tc.name: SetandGetScaleX002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetScaleX002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetScaleX(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScale().x_, floatData[2]));
}

/**
 * @tc.name: SetandGetScaleX003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetScaleX003, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetScaleX(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScale().x_, floatData[3]));
}

/**
 * @tc.name: SetandGetScaleX004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetScaleX004, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetScaleX(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScale().x_, floatData[4]));
}

/**
 * @tc.name: SetandGetScaleX005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetScaleX005, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetScaleX(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScale().x_, floatData[0]));
}

/**
 * @tc.name: SetandGetScale001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetScale001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetScale(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScale().x_, floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScale().y_, floatData[1]));
}

/**
 * @tc.name: SetandGetScale002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetScale002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetScale(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScale().x_, floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScale().y_, floatData[2]));
}

/**
 * @tc.name: SetandGetScale003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetScale003, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetScale(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScale().x_, floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScale().y_, floatData[3]));
}

/**
 * @tc.name: SetandGetScale004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetScale004, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetScale(floatData[3], floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScale().x_, floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScale().y_, floatData[0]));
}

/**
 * @tc.name: SetandGetScale0005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetScale0005, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetScale(floatData[2], floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScale().x_, floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScale().y_, floatData[1]));
}

/**
 * @tc.name: SetandGetScaleY001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetScaleY001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetScaleY(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScale().y_, floatData[1]));
}

/**
 * @tc.name: SetandGetScaleY002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetScaleY002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetScaleY(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScale().y_, floatData[2]));
}

/**
 * @tc.name: SetandGetScaleY003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetScaleY003, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetScaleY(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScale().y_, floatData[3]));
}

/**
 * @tc.name: SetandGetScaleY004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetScaleY004, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetScaleY(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScale().y_, floatData[4]));
}

/**
 * @tc.name: SetandGetScaleY005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetScaleY005, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetScaleY(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScale().y_, floatData[0]));
}

/**
 * @tc.name: SetandGetSkew001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetSkew001, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetSkew(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetSkew().x_, floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetSkew().y_, floatData[1]));
}

/**
 * @tc.name: SetandGetSkew002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetSkew002, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetSkew(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetSkew().x_, floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetSkew().y_, floatData[2]));
}

/**
 * @tc.name: SetandGetSkew003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetSkew003, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetSkew(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetSkew().x_, floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetSkew().y_, floatData[3]));
}

/**
 * @tc.name: SetandGetSkew004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetSkew004, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetSkew(floatData[3], floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetSkew().x_, floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetSkew().y_, floatData[0]));
}

/**
 * @tc.name: SetandGetSkew0005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetSkew0005, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetSkew(floatData[2], floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetSkew().x_, floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetSkew().y_, floatData[1]));
}

/**
 * @tc.name: SetandGetSkewX001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetSkewX001, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetSkewX(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetSkew().x_, floatData[1]));
}

/**
 * @tc.name: SetandGetSkewX002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetSkewX002, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetSkewX(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetSkew().x_, floatData[2]));
}

/**
 * @tc.name: SetandGetSkewX003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetSkewX003, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetSkewX(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetSkew().x_, floatData[3]));
}

/**
 * @tc.name: SetandGetSkewX004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetSkewX004, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetSkewX(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetSkew().x_, floatData[4]));
}

/**
 * @tc.name: SetandGetSkewX005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetSkewX005, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetSkewX(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetSkew().x_, floatData[0]));
}

/**
 * @tc.name: SetandGetSkewY001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetSkewY001, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetSkewY(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetSkew().y_, floatData[1]));
}

/**
 * @tc.name: SetandGetSkewY002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetSkewY002, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetSkewY(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetSkew().y_, floatData[2]));
}

/**
 * @tc.name: SetandGetSkewY003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetSkewY003, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetSkewY(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetSkew().y_, floatData[3]));
}

/**
 * @tc.name: SetandGetSkewY004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetSkewY004, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetSkewY(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetSkew().y_, floatData[4]));
}

/**
 * @tc.name: SetandGetSkewY005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetSkewY005, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetSkewY(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetSkew().y_, floatData[0]));
}

/**
 * @tc.name: SetandGetPersp001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetPersp001, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetPersp(FLOAT_DATA_POSITIVE);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetPersp().x_, FLOAT_DATA_POSITIVE));
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetPersp().y_, FLOAT_DATA_POSITIVE));
}

/**
 * @tc.name: SetandGetPersp002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetPersp002, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetPersp(FLOAT_DATA_NEGATIVE);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetPersp().x_, FLOAT_DATA_NEGATIVE));
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetPersp().y_, FLOAT_DATA_NEGATIVE));
}

/**
 * @tc.name: SetandGetPersp003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetPersp003, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetPersp(FLOAT_DATA_MAX);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetPersp().x_, FLOAT_DATA_MAX));
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetPersp().y_, FLOAT_DATA_MAX));
}

/**
 * @tc.name: SetandGetPersp004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetPersp004, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetPersp(FLOAT_DATA_MAX, FLOAT_DATA_ZERO);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetPersp().x_, FLOAT_DATA_MAX));
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetPersp().y_, FLOAT_DATA_ZERO));
}

/**
 * @tc.name: SetandGetPersp0005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetPersp0005, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetPersp(FLOAT_DATA_NEGATIVE, FLOAT_DATA_POSITIVE);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetPersp().x_, FLOAT_DATA_NEGATIVE));
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetPersp().y_, FLOAT_DATA_POSITIVE));
}

/**
 * @tc.name: SetandGetPerspX001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetPerspX001, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetPerspX(FLOAT_DATA_POSITIVE);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetPersp().x_, FLOAT_DATA_POSITIVE));
}

/**
 * @tc.name: SetandGetPerspX002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetPerspX002, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetPerspX(FLOAT_DATA_NEGATIVE);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetPersp().x_, FLOAT_DATA_NEGATIVE));
}

/**
 * @tc.name: SetandGetPerspX003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetPerspX003, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetPerspX(FLOAT_DATA_MAX);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetPersp().x_, FLOAT_DATA_MAX));
}

/**
 * @tc.name: SetandGetPerspX004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetPerspX004, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetPerspX(FLOAT_DATA_MIN);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetPersp().x_, FLOAT_DATA_MIN));
}

/**
 * @tc.name: SetandGetPerspX005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetPerspX005, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetPerspX(FLOAT_DATA_ZERO);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetPersp().x_, FLOAT_DATA_ZERO));
}

/**
 * @tc.name: SetandGetPerspY001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetPerspY001, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetPerspY(FLOAT_DATA_POSITIVE);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetPersp().y_, FLOAT_DATA_POSITIVE));
}

/**
 * @tc.name: SetandGetPerspY002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetPerspY002, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetPerspY(FLOAT_DATA_NEGATIVE);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetPersp().y_, FLOAT_DATA_NEGATIVE));
}

/**
 * @tc.name: SetandGetPerspY003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetPerspY003, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetPerspY(FLOAT_DATA_MAX);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetPersp().y_, FLOAT_DATA_MAX));
}

/**
 * @tc.name: SetandGetPerspY004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetPerspY004, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetPerspY(FLOAT_DATA_MIN);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetPersp().y_, FLOAT_DATA_MIN));
}

/**
 * @tc.name: SetandGetPerspY005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetPerspY005, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetPerspY(FLOAT_DATA_ZERO);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetPersp().y_, FLOAT_DATA_ZERO));
}

/**
 * @tc.name: SetandGetAlphaOffscreen001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetAlphaOffscreen001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetAlphaOffscreen(true);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetAlphaOffscreen(), true));
}

/**
 * @tc.name: SetandGetAlphaOffscreen002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetAlphaOffscreen002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetAlphaOffscreen(false);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetAlphaOffscreen(), false));
}

/**
 * @tc.name: SetandGetAlpha001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetAlpha001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetAlpha(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetAlpha(), floatData[1]));
}

/**
 * @tc.name: SetandGetAlpha002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetAlpha002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetAlpha(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetAlpha(), floatData[2]));
}

/**
 * @tc.name: SetandGetAlpha003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetAlpha003, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetAlpha(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetAlpha(), floatData[3]));
}

/**
 * @tc.name: SetandGetAlpha004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetAlpha004, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetAlpha(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetAlpha(), floatData[4]));
}

/**
 * @tc.name: SetandGetAlpha005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetAlpha005, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetAlpha(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetAlpha(), floatData[0]));
}

/**
 * @tc.name: SetandGetBgImageSize001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetBgImageSize001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBgImageSize(floatData[0], floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImageWidth(), floatData[0]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImageHeight(), floatData[1]));
}

/**
 * @tc.name: SetandGetBgImageSize002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetBgImageSize002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBgImageSize(floatData[3], floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImageWidth(), floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImageHeight(), floatData[1]));
}

/**
 * @tc.name: SetandGetBgImageSize003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetBgImageSize003, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBgImageSize(floatData[3], floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImageWidth(), floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImageHeight(), floatData[2]));
}

/**
 * @tc.name: SetandGetBgImageWidth001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetBgImageWidth001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBgImageWidth(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImageWidth(), floatData[1]));
}

/**
 * @tc.name: SetandGetBgImageWidth002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetBgImageWidth002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBgImageWidth(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImageWidth(), floatData[2]));
}

/**
 * @tc.name: SetandGetBgImageWidth003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetBgImageWidth003, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBgImageWidth(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImageWidth(), floatData[3]));
}

/**
 * @tc.name: SetandGetBgImageWidth004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetBgImageWidth004, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBgImageWidth(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImageWidth(), floatData[4]));
}

/**
 * @tc.name: SetandGetBgImageWidth005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetBgImageWidth005, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBgImageWidth(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImageWidth(), floatData[0]));
}

/**
 * @tc.name: SetandGetBgImageHeight001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetBgImageHeight001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBgImageHeight(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImageHeight(), floatData[1]));
}

/**
 * @tc.name: SetandGetBgImageHeight002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetBgImageHeight002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBgImageHeight(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImageHeight(), floatData[2]));
}

/**
 * @tc.name: SetandGetBgImageHeight003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetBgImageHeight003, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBgImageHeight(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImageHeight(), floatData[3]));
}

/**
 * @tc.name: SetandGetBgImageHeight004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetBgImageHeight004, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBgImageHeight(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImageHeight(), floatData[4]));
}

/**
 * @tc.name: SetandGetBgImageHeight005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetBgImageHeight005, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBgImageHeight(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImageHeight(), floatData[0]));
}

/**
 * @tc.name: SetandSetBgImagePosition001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandSetBgImagePosition001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBgImagePosition(floatData[2], floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImagePositionX(), floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImagePositionY(), floatData[3]));
}

/**
 * @tc.name: SetandSetBgImagePosition002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandSetBgImagePosition002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBgImagePosition(floatData[2], floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImagePositionX(), floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImagePositionY(), floatData[0]));
}

/**
 * @tc.name: SetandSetBgImagePosition003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandSetBgImagePosition003, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBgImagePosition(floatData[1], floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImagePositionX(), floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImagePositionY(), floatData[3]));
}

/**
 * @tc.name: SetandGetBgImagePositionX001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetBgImagePositionX001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBgImagePositionX(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImagePositionX(), floatData[1]));
}

/**
 * @tc.name: SetandGetBgImagePositionX002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetBgImagePositionX002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBgImagePositionX(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImagePositionX(), floatData[2]));
}

/**
 * @tc.name: SetandGetBgImagePositionX003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetBgImagePositionX003, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBgImagePositionX(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImagePositionX(), floatData[3]));
}

/**
 * @tc.name: SetandGetBgImagePositionX004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetBgImagePositionX004, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBgImagePositionX(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImagePositionX(), floatData[4]));
}

/**
 * @tc.name: SetandGetBgImagePositionX005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetBgImagePositionX005, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBgImagePositionX(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImagePositionX(), floatData[0]));
}

/**
 * @tc.name: SetandGetBgImagePositionY001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetBgImagePositionY001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBgImagePositionY(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImagePositionY(), floatData[1]));
}

/**
 * @tc.name: SetandGetBgImagePositionY002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetBgImagePositionY002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBgImagePositionY(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImagePositionY(), floatData[2]));
}

/**
 * @tc.name: SetandGetBgImagePositionY003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetBgImagePositionY003, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBgImagePositionY(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImagePositionY(), floatData[3]));
}

/**
 * @tc.name: SetandGetBgImagePositionY004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetBgImagePositionY004, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBgImagePositionY(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImagePositionY(), floatData[4]));
}

/**
 * @tc.name: SetandGetBgImagePositionY005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetBgImagePositionY005, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBgImagePositionY(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImagePositionY(), floatData[0]));
}

/**
 * @tc.name: SetandGetBorderWidth001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetBorderWidth001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBorderWidth(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBorderWidth().x_, floatData[1]));
}

/**
 * @tc.name: SetandGetBorderWidth002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetBorderWidth002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBorderWidth(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBorderWidth().x_, floatData[2]));
}

/**
 * @tc.name: SetandGetBorderWidth003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetBorderWidth003, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBorderWidth(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBorderWidth().x_, floatData[3]));
}

/**
 * @tc.name: SetandGetBorderWidth004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetBorderWidth004, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBorderWidth(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBorderWidth().x_, floatData[4]));
}

/**
 * @tc.name: SetandGetBorderWidth005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetBorderWidth005, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBorderWidth(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBorderWidth().x_, floatData[0]));
}

/**
 * @tc.name: SetandGetOutlineWidth001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetOutlineWidth001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetOutlineWidth(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetOutlineWidth().x_, floatData[1]));
}

/**
 * @tc.name: SetandGetOutlineWidth002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetOutlineWidth002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetOutlineWidth(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetOutlineWidth().x_, floatData[2]));
}

/**
 * @tc.name: SetandGetOutlineWidth003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetOutlineWidth003, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetOutlineWidth(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetOutlineWidth().x_, floatData[3]));
}

/**
 * @tc.name: SetandGetOutlineWidth004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetOutlineWidth004, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetOutlineWidth(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetOutlineWidth().x_, floatData[4]));
}

/**
 * @tc.name: SetandGetOutlineWidth005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetOutlineWidth005, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetOutlineWidth(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetOutlineWidth().x_, floatData[0]));
}

/**
 * @tc.name: SetandGetPivot001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetPivot001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetPivot(floatData[0], floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPivot().x_, floatData[0]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPivot().y_, floatData[1]));
}

/**
 * @tc.name: SetandGetPivot002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetPivot002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetPivot(floatData[3], floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPivot().x_, floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPivot().y_, floatData[2]));
}

/**
 * @tc.name: SetandGetPivot003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetPivot003, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetPivot(floatData[1], floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPivot().x_, floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPivot().y_, floatData[3]));
}

/**
 * @tc.name: SetandGetPivotX001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetPivotX001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetPivotX(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPivot().x_, floatData[1]));
}

/**
 * @tc.name: SetandGetPivotX002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetPivotX002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetPivotX(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPivot().x_, floatData[2]));
}

/**
 * @tc.name: SetandGetPivotX003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetPivotX003, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetPivotX(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPivot().x_, floatData[3]));
}

/**
 * @tc.name: SetandGetPivotX004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetPivotX004, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetPivotX(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPivot().x_, floatData[4]));
}

/**
 * @tc.name: SetandGetPivotX005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetPivotX005, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetPivotX(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPivot().x_, floatData[0]));
}

/**
 * @tc.name: SetandGetPivotY001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetPivotY001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetPivotY(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPivot().y_, floatData[1]));
}

/**
 * @tc.name: SetandGetPivotY002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetPivotY002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetPivotY(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPivot().y_, floatData[2]));
}

/**
 * @tc.name: SetandGetPivotY003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetPivotY003, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetPivotY(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPivot().y_, floatData[3]));
}

/**
 * @tc.name: SetandGetPivotY004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetPivotY004, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetPivotY(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPivot().y_, floatData[4]));
}

/**
 * @tc.name: SetandGetPivotY005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetPivotY005, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetPivotY(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPivot().y_, floatData[0]));
}

/**
 * @tc.name: SetandGetShadowOffset001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetShadowOffset001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowOffset(floatData[2], floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowOffsetX(), floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowOffsetY(), floatData[3]));
}

/**
 * @tc.name: SetandGetShadowOffset002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetShadowOffset002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowOffset(floatData[2], floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowOffsetX(), floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowOffsetY(), floatData[0]));
}

/**
 * @tc.name: SetandGetShadowOffset003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetShadowOffset003, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowOffset(floatData[1], floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowOffsetX(), floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowOffsetY(), floatData[3]));
}
/**
 * @tc.name: SetandGetShadowOffsetX001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetShadowOffsetX001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowOffsetX(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowOffsetX(), floatData[1]));
}

/**
 * @tc.name: SetandGetShadowOffsetX002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetShadowOffsetX002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowOffsetX(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowOffsetX(), floatData[2]));
}

/**
 * @tc.name: SetandGetShadowOffsetX003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetShadowOffsetX003, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowOffsetX(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowOffsetX(), floatData[3]));
}

/**
 * @tc.name: SetandGetShadowOffsetX004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetShadowOffsetX004, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowOffsetX(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowOffsetX(), floatData[4]));
}

/**
 * @tc.name: SetandGetShadowOffsetX005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetShadowOffsetX005, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowOffsetX(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowOffsetX(), floatData[0]));
}

/**
 * @tc.name: SetandGetShadowOffsetY001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetShadowOffsetY001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowOffsetY(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowOffsetY(), floatData[1]));
}

/**
 * @tc.name: SetandGetShadowOffsetY002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetShadowOffsetY002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowOffsetY(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowOffsetY(), floatData[2]));
}

/**
 * @tc.name: SetandGetShadowOffsetY003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetShadowOffsetY003, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowOffsetY(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowOffsetY(), floatData[3]));
}

/**
 * @tc.name: SetandGetShadowOffsetY004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetShadowOffsetY004, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowOffsetY(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowOffsetY(), floatData[4]));
}

/**
 * @tc.name: SetandGetShadowOffsetY005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetShadowOffsetY005, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowOffsetY(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowOffsetY(), floatData[0]));
}

/**
 * @tc.name: SetandGetShadowAlpha001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetShadowAlpha001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowAlpha(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowAlpha(), floatData[1], 0.02f));
}

/**
 * @tc.name: SetandGetShadowAlpha002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetShadowAlpha002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowAlpha(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowAlpha(), floatData[2], 0.02f));
}

/**
 * @tc.name: SetandGetShadowAlpha003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetShadowAlpha003, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowAlpha(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowAlpha(), floatData[3], 0.02f));
}

/**
 * @tc.name: SetandGetShadowAlpha004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetShadowAlpha004, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowAlpha(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowAlpha(), floatData[4], 0.02f));
}

/**
 * @tc.name: SetandGetShadowAlpha005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetShadowAlpha005, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowAlpha(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowAlpha(),
        std::clamp(floatData[0], 0.0f, 1.0f), 0.02f));
}

/**
 * @tc.name: SetandGetShadowElevation001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetShadowElevation001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowElevation(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowElevation(), floatData[1]));
}

/**
 * @tc.name: SetandGetShadowElevation002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetShadowElevation002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowElevation(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowElevation(), floatData[2]));
}

/**
 * @tc.name: SetandGetShadowElevation003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetShadowElevation003, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowElevation(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowElevation(), floatData[3]));
}

/**
 * @tc.name: SetandGetShadowElevation004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetShadowElevation004, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowElevation(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowElevation(), floatData[4]));
}

/**
 * @tc.name: SetandGetShadowElevation005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetShadowElevation005, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowElevation(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowElevation(), floatData[0]));
}

/**
 * @tc.name: SetandGetShadowRadius001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetShadowRadius001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowRadius(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowRadius(), floatData[1]));
}

/**
 * @tc.name: SetandGetShadowRadius002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetShadowRadius002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowRadius(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowRadius(), floatData[2]));
}

/**
 * @tc.name: SetandGetShadowRadius003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetShadowRadius003, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowRadius(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowRadius(), floatData[3]));
}

/**
 * @tc.name: SetandGetShadowRadius004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetShadowRadius004, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowRadius(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowRadius(), floatData[4]));
}

/**
 * @tc.name: SetandGetShadowRadius005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetShadowRadius005, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowRadius(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowRadius(), floatData[0]));
}

/**
 * @tc.name: SetandGetShadowColor001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetShadowColor001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = 0x034123;
    rsNode->SetShadowColor(colorValue);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetShadowColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetShadowColor002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetShadowColor002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = 0x45ba87;
    rsNode->SetShadowColor(colorValue);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetShadowColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetShadowColor003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetShadowColor003, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = 0x32aadd;
    rsNode->SetShadowColor(colorValue);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetShadowColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetShadowColor004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetShadowColor004, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::max();
    rsNode->SetShadowColor(colorValue);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetShadowColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetShadowColor005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetShadowColor005, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::min();
    rsNode->SetShadowColor(colorValue);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetShadowColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetShadowColorStrategy001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetShadowColorStrategy001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    int shadowColorStrategy = SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_NONE;
    rsNode->SetShadowColorStrategy(shadowColorStrategy);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowColorStrategy(), shadowColorStrategy));
}

/**
 * @tc.name: SetandGetShadowColorStrategy002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetShadowColorStrategy002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    int shadowColorStrategy = SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_AVERAGE;
    rsNode->SetShadowColorStrategy(shadowColorStrategy);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowColorStrategy(), shadowColorStrategy));
}

/**
 * @tc.name: SetandGetShadowColorStrategy003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetShadowColorStrategy003, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    int shadowColorStrategy = SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_MAIN;
    rsNode->SetShadowColorStrategy(shadowColorStrategy);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowColorStrategy(), shadowColorStrategy));
}

/**
 * @tc.name: SetandGetTranslateThree001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetTranslateThree001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetTranslate(floatData[1], floatData[2], floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslate().x_, floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslate().y_, floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslateZ(), floatData[3]));
}

/**
 * @tc.name: SetandGetTranslateThree002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetTranslateThree002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetTranslate(floatData[0], floatData[0], floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslate().x_, floatData[0]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslate().y_, floatData[0]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslateZ(), floatData[2]));
}

/**
 * @tc.name: SetandGetTranslateThree003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetTranslateThree003, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetTranslate(floatData[1], floatData[3], floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslate().x_, floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslate().y_, floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslateZ(), floatData[1]));
}

/**
 * @tc.name: SetandGetTranslateX001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetTranslateX001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetTranslateX(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslate().x_, floatData[1]));
}

/**
 * @tc.name: SetandGetTranslateX002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetTranslateX002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetTranslateX(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslate().x_, floatData[2]));
}

/**
 * @tc.name: SetandGetTranslateX003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetTranslateX003, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetTranslateX(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslate().x_, floatData[3]));
}

/**
 * @tc.name: SetandGetTranslateX004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetTranslateX004, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetTranslateX(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslate().x_, floatData[4]));
}

/**
 * @tc.name: SetandGetTranslateX005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetTranslateX005, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetTranslateX(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslate().x_, floatData[0]));
}

/**
 * @tc.name: SetandGetTranslateY001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetTranslateY001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetTranslateY(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslate().y_, floatData[1]));
}

/**
 * @tc.name: SetandGetTranslateY002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetTranslateY002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetTranslateY(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslate().y_, floatData[2]));
}

/**
 * @tc.name: SetandGetTranslateY003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetTranslateY003, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetTranslateY(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslate().y_, floatData[3]));
}

/**
 * @tc.name: SetandGetTranslateY004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetTranslateY004, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetTranslateY(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslate().y_, floatData[4]));
}

/**
 * @tc.name: SetandGetTranslateY005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetTranslateY005, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetTranslateY(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslate().y_, floatData[0]));
}

/**
 * @tc.name: SetandGetTranslateZ001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetTranslateZ001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetTranslateZ(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslateZ(), floatData[1]));
}

/**
 * @tc.name: SetandGetTranslateZ002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetTranslateZ002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetTranslateZ(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslateZ(), floatData[2]));
}

/**
 * @tc.name: SetandGetTranslateZ003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetTranslateZ003, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetTranslateZ(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslateZ(), floatData[3]));
}

/**
 * @tc.name: SetandGetTranslateZ004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetTranslateZ004, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetTranslateZ(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslateZ(), floatData[4]));
}

/**
 * @tc.name: SetandGetTranslateZ005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetTranslateZ005, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetTranslateZ(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslateZ(), floatData[0]));
}

/**
 * @tc.name: SetandGetClipToBounds001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetClipToBounds001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetClipToBounds(true);
    EXPECT_EQ(rsNode->GetStagingProperties().GetClipToBounds(), true);
}

/**
 * @tc.name: SetandGetClipToBounds002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetClipToBounds002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetClipToBounds(false);
    EXPECT_EQ(rsNode->GetStagingProperties().GetClipToBounds(), false);
}

/**
 * @tc.name: SetandGetClipToFrame001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetClipToFrame001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetClipToFrame(true);
    EXPECT_EQ(rsNode->GetStagingProperties().GetClipToFrame(), true);
}

/**
 * @tc.name: SetandGetClipToFrame002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetClipToFrame002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetClipToFrame(false);
    EXPECT_EQ(rsNode->GetStagingProperties().GetClipToFrame(), false);
}

/**
 * @tc.name: SetandGetVisible001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetVisible001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetVisible(true);
    EXPECT_EQ(rsNode->GetStagingProperties().GetVisible(), true);
}

/**
 * @tc.name: SetandGetVisible002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetVisible002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetVisible(false);
    EXPECT_EQ(rsNode->GetStagingProperties().GetVisible(), false);
}

/**
 * @tc.name: SetandGetBorderStyle001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetBorderStyle001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    uint32_t borderStyle = static_cast<uint32_t>(BorderStyle::SOLID);
    rsNode->SetBorderStyle(borderStyle);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBorderStyle().x_ == borderStyle);
}

/**
 * @tc.name: SetandGetBorderStyle002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetBorderStyle002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    uint32_t borderStyle = static_cast<uint32_t>(BorderStyle::DASHED);
    rsNode->SetBorderStyle(borderStyle);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBorderStyle().x_ == borderStyle);
}

/**
 * @tc.name: SetandGetBorderStyle003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetBorderStyle003, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    uint32_t borderStyle = static_cast<uint32_t>(BorderStyle::DOTTED);
    rsNode->SetBorderStyle(borderStyle);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBorderStyle().x_ == borderStyle);
}

/**
 * @tc.name: SetandGetBorderStyle004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetBorderStyle004, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    uint32_t borderStyle = static_cast<uint32_t>(BorderStyle::NONE);
    rsNode->SetBorderStyle(borderStyle);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBorderStyle().x_ == borderStyle);
}

/**
 * @tc.name: SetandGetOutlineStyle001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetOutlineStyle001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    uint32_t borderStyle = static_cast<uint32_t>(BorderStyle::SOLID);
    rsNode->SetOutlineStyle(BorderStyle::SOLID);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetOutlineStyle().x_ == borderStyle);
}

/**
 * @tc.name: SetandGetOutlineStyle002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetOutlineStyle002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    uint32_t borderStyle = static_cast<uint32_t>(BorderStyle::DASHED);
    rsNode->SetOutlineStyle(BorderStyle::DASHED);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetOutlineStyle().x_ == borderStyle);
}

/**
 * @tc.name: SetandGetOutlineStyle003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetOutlineStyle003, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    uint32_t borderStyle = static_cast<uint32_t>(BorderStyle::DOTTED);
    rsNode->SetOutlineStyle(BorderStyle::DOTTED);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetOutlineStyle().x_ == borderStyle);
}

/**
 * @tc.name: SetandGetOutlineStyle004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetOutlineStyle004, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    uint32_t borderStyle = static_cast<uint32_t>(BorderStyle::NONE);
    rsNode->SetOutlineStyle(BorderStyle::NONE);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetOutlineStyle().x_ == borderStyle);
}

/**
 * @tc.name: SetandGetFrameGravity001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetFrameGravity001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    Gravity gravity = Gravity::CENTER;
    rsNode->SetFrameGravity(gravity);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetFrameGravity002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    Gravity gravity = Gravity::TOP;
    rsNode->SetFrameGravity(gravity);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetFrameGravity003, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    Gravity gravity = Gravity::BOTTOM;
    rsNode->SetFrameGravity(gravity);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetFrameGravity004, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    Gravity gravity = Gravity::LEFT;
    rsNode->SetFrameGravity(gravity);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetFrameGravity005, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    Gravity gravity = Gravity::RIGHT;
    rsNode->SetFrameGravity(gravity);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity006
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetFrameGravity006, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    Gravity gravity = Gravity::TOP_LEFT;
    rsNode->SetFrameGravity(gravity);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity007
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetFrameGravity007, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    Gravity gravity = Gravity::TOP_RIGHT;
    rsNode->SetFrameGravity(gravity);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity008
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetFrameGravity008, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    Gravity gravity = Gravity::BOTTOM_LEFT;
    rsNode->SetFrameGravity(gravity);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity009
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetFrameGravity009, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    Gravity gravity = Gravity::BOTTOM_RIGHT;
    rsNode->SetFrameGravity(gravity);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity010
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetFrameGravity010, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    Gravity gravity = Gravity::RESIZE;
    rsNode->SetFrameGravity(gravity);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity011
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetFrameGravity011, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    Gravity gravity = Gravity::RESIZE_ASPECT;
    rsNode->SetFrameGravity(gravity);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity012
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetFrameGravity012, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    Gravity gravity = Gravity::RESIZE_ASPECT_FILL;
    rsNode->SetFrameGravity(gravity);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetForegroundColor001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetForegroundColor001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = 0x034123;
    rsNode->SetForegroundColor(colorValue);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetForegroundColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetForegroundColor002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetForegroundColor002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::max();
    rsNode->SetForegroundColor(colorValue);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetForegroundColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetForegroundColor003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetForegroundColor003, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::min();
    rsNode->SetForegroundColor(colorValue);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetForegroundColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetBackgroundColor001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetBackgroundColor001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = 0x034123;
    rsNode->SetBackgroundColor(colorValue);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBackgroundColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetBackgroundColor002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetBackgroundColor002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::max();
    rsNode->SetBackgroundColor(colorValue);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBackgroundColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetBackgroundColor003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetBackgroundColor003, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::min();
    rsNode->SetBackgroundColor(colorValue);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBackgroundColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetBorderColor001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetBorderColor001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = 0x034123;
    rsNode->SetBorderColor(colorValue);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBorderColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetBorderColor002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetBorderColor002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::max();
    rsNode->SetBorderColor(colorValue);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBorderColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetBorderColor003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetBorderColor003, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::min();
    rsNode->SetBorderColor(colorValue);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBorderColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetOutlineColor001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetOutlineColor001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = 0x034123;
    rsNode->SetOutlineColor(Color::FromArgbInt(colorValue));
    EXPECT_TRUE(rsNode->GetStagingProperties().GetOutlineColor().x_ == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetOutlineColor002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetOutlineColor002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::max();
    rsNode->SetOutlineColor(Color::FromArgbInt(colorValue));
    EXPECT_TRUE(rsNode->GetStagingProperties().GetOutlineColor().x_ == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetOutlineColor003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetOutlineColor003, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::min();
    rsNode->SetOutlineColor(Color::FromArgbInt(colorValue));
    EXPECT_TRUE(rsNode->GetStagingProperties().GetOutlineColor().x_ == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetRotationVector001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetRotationVector001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    Vector4f quaternion(std::numeric_limits<float>::min(), 2.f, 3.f, 4.f);
    rsNode->SetRotation(quaternion);
}

/**
 * @tc.name: SetandGetTranslateVector001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetTranslateVector001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    Vector2f quaternion(std::numeric_limits<float>::max(), 2.f);
    rsNode->SetTranslate(quaternion);
}

/**
 * @tc.name: CreateBlurFilter001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, CreateBlurFilter001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> backgroundFilter = RSFilter::CreateBlurFilter(floatData[0], floatData[1]);
    rsNode->SetBackgroundFilter(backgroundFilter);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBackgroundBlurRadiusX() == floatData[0]);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBackgroundBlurRadiusY() == floatData[1]);
}

/**
 * @tc.name: CreateBlurFilter002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, CreateBlurFilter002, TestSize.Level2)
{
    auto rsNode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> backgroundFilter = RSFilter::CreateBlurFilter(floatData[1], floatData[2]);
    rsNode->SetBackgroundFilter(backgroundFilter);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBackgroundBlurRadiusX() == floatData[1]);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBackgroundBlurRadiusY() == floatData[2]);
}

/**
 * @tc.name: CreateBlurFilter003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, CreateBlurFilter003, TestSize.Level3)
{
    auto rsNode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> backgroundFilter = RSFilter::CreateBlurFilter(floatData[2], floatData[3]);
    rsNode->SetBackgroundFilter(backgroundFilter);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBackgroundBlurRadiusX() == floatData[2]);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBackgroundBlurRadiusY() == floatData[3]);
}

/**
 * @tc.name: CreateBlurFilter004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, CreateBlurFilter004, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> backgroundFilter = RSFilter::CreateBlurFilter(floatData[3], floatData[4]);
    rsNode->SetBackgroundFilter(backgroundFilter);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBackgroundBlurRadiusX() == floatData[3]);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBackgroundBlurRadiusY() == floatData[4]);
}

/**
 * @tc.name: CreateBlurFilter005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, CreateBlurFilter005, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> backgroundFilter = RSFilter::CreateBlurFilter(floatData[4], floatData[0]);
    rsNode->SetBackgroundFilter(backgroundFilter);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBackgroundBlurRadiusX() == floatData[4]);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBackgroundBlurRadiusY() == floatData[0]);
}

/**
 * @tc.name: CreateNormalFilter001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, CreateNormalFilter001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(floatData[0], floatData[1]);
    rsNode->SetFilter(filter);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetForegroundBlurRadiusX() == floatData[0]);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetForegroundBlurRadiusY() == floatData[1]);
}

/**
 * @tc.name: CreateNormalFilter002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, CreateNormalFilter002, TestSize.Level2)
{
    auto rsNode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(floatData[1], floatData[2]);
    rsNode->SetFilter(filter);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetForegroundBlurRadiusX() == floatData[1]);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetForegroundBlurRadiusY() == floatData[2]);
}

/**
 * @tc.name: CreateNormalFilter003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, CreateNormalFilter003, TestSize.Level3)
{
    auto rsNode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(floatData[2], floatData[3]);
    rsNode->SetFilter(filter);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetForegroundBlurRadiusX() == floatData[2]);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetForegroundBlurRadiusY() == floatData[3]);
}

/**
 * @tc.name: CreateNormalFilter004
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, CreateNormalFilter004, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(floatData[3], floatData[4]);
    rsNode->SetFilter(filter);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetForegroundBlurRadiusX() == floatData[3]);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetForegroundBlurRadiusY() == floatData[4]);
}

/**
 * @tc.name: CreateNormalFilter005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, CreateNormalFilter005, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(floatData[4], floatData[0]);
    rsNode->SetFilter(filter);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetForegroundBlurRadiusX() == floatData[4]);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetForegroundBlurRadiusY() == floatData[0]);
}

/**
 * @tc.name: SetandGetClipBounds001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetClipBounds001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    std::shared_ptr<RSPath> clipPath = RSPath::CreateRSPath();
    rsNode->SetClipBounds(clipPath);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetClipBounds() == clipPath);
}

/**
 * @tc.name: GetId001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, GetId001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->GetId();
}

/**
 * @tc.name: GetChildren001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, GetChildren001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    auto c = rsNode->GetChildren();
}

/**
 * @tc.name: GetChildren002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, GetChildren002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    const auto c = rsNode->GetChildren();
}

/**
 * @tc.name: GetStagingProperties001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, GetStagingProperties001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->GetStagingProperties();
}

/**
 * @tc.name: GetMotionPathOption001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, GetMotionPathOption002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->GetMotionPathOption();
}

/**
 * @tc.name: SetBgImage001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetBgImage001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    auto image = std::make_shared<RSImage>();
    rsNode->SetBgImage(image);
}

/**
 * @tc.name: SetBackgroundShader001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetBackgroundShader001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    auto shader = RSShader::CreateRSShader();
    rsNode->SetBackgroundShader(shader);
}

/**
 * @tc.name: SetandGetGreyCoef001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetGreyCoef001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    Vector2f greyCoef = { 0.5, 0.5 };
    rsNode->SetGreyCoef(greyCoef);
}

/**
 * @tc.name: SetandGetAiInvertTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetAiInvertTest, TestSize.Level1)
{
    auto rootNode = RSCanvasNode::Create();
    auto value = Vector4f(10.f);
    rootNode->SetAiInvert(value);
}

/**
 * @tc.name: SetandGetSpherizeDegree001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetSpherizeDegree001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    float spherizeDegree = 1.0f;
    rsNode->SetSpherizeDegree(spherizeDegree);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetSpherizeDegree(), spherizeDegree));
}

/**
 * @tc.name: SetandGetLightUpEffectDegree001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetLightUpEffectDegree001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    float lightUpEffectDegree = 1.0f;
    rsNode->SetLightUpEffectDegree(lightUpEffectDegree);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetLightUpEffectDegree(), lightUpEffectDegree));
}

/**
 * @tc.name: SetandGetShadowIsFilled001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetShadowIsFilled001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowIsFilled(true);
    EXPECT_EQ(rsNode->GetStagingProperties().GetShadowIsFilled(), true);
}

/**
 * @tc.name: SetandGetShadowIsFilled002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetShadowIsFilled002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowIsFilled(false);
    EXPECT_EQ(rsNode->GetStagingProperties().GetShadowIsFilled(), false);
}

/**
 * @tc.name: SetandGetForegroundEffectRadius001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetandGetForegroundEffectRadius001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    float radius = 10.0f;
    rsNode->SetForegroundEffectRadius(radius);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetForegroundEffectRadius(), radius));
}

/**
 * @tc.name: SetCompositingFilter001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetCompositingFilter001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    auto compositingFilter = RSFilter::CreateBlurFilter(0.0f, 0.0f);
    rsNode->SetCompositingFilter(compositingFilter);
}

/**
 * @tc.name: SetShadowPath001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetShadowPath001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    auto shadowpath = RSPath::CreateRSPath();
    rsNode->SetShadowPath(shadowpath);
}

/**
 * @tc.name: SetFreeze001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetFreeze001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetFreeze(true);
}

template<typename ModifierName, typename PropertyName, typename T>
void SetPropertyTest(RSModifierType modifierType, T value1, T value2)
{
    auto node = RSCanvasNode::Create();
    node->SetProperty<ModifierName, PropertyName, T>(modifierType, value1);
    node->SetProperty<ModifierName, PropertyName, T>(modifierType, value1);
    node->SetProperty<ModifierName, PropertyName, T>(modifierType, value2);
};

/**
 * @tc.name: SetProperty001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetProperty001, TestSize.Level1)
{
    SetPropertyTest<RSAlphaModifier, RSAnimatableProperty<float>, float>(
        RSModifierType::ALPHA, 0.0f, 0.5f);
    SetPropertyTest<RSAlphaOffscreenModifier, RSProperty<bool>, bool>(
        RSModifierType::ALPHA_OFFSCREEN, false, true);
    SetPropertyTest<RSBoundsModifier, RSAnimatableProperty<Vector4f>, Vector4f>(
        RSModifierType::BOUNDS, Vector4f(), Vector4f(25.f));
    SetPropertyTest<RSFrameModifier, RSAnimatableProperty<Vector4f>, Vector4f>(
        RSModifierType::FRAME, Vector4f(), Vector4f(25.f));
    SetPropertyTest<RSPositionZModifier, RSAnimatableProperty<float>, float>(
        RSModifierType::POSITION_Z, 1.f, 2.f);
    SetPropertyTest<RSPivotModifier, RSAnimatableProperty<Vector2f>, Vector2f>(
        RSModifierType::PIVOT, Vector2f(0.f, 0.f), Vector2f(1.f, 1.f));
    SetPropertyTest<RSCornerRadiusModifier, RSAnimatableProperty<Vector4f>, Vector4f>(
        RSModifierType::CORNER_RADIUS, Vector4f(), Vector4f(1.f));
    SetPropertyTest<RSQuaternionModifier, RSAnimatableProperty<Quaternion>, Quaternion>(
        RSModifierType::QUATERNION, Quaternion(), Quaternion(0.f, 1.f, 0.f, 0.f));
    SetPropertyTest<RSRotationModifier, RSAnimatableProperty<float>, float>(
        RSModifierType::ROTATION, 45.f, 90.f);
    SetPropertyTest<RSRotationXModifier, RSAnimatableProperty<float>, float>(
        RSModifierType::ROTATION_X, 45.f, 90.f);
    SetPropertyTest<RSRotationYModifier, RSAnimatableProperty<float>, float>(
        RSModifierType::ROTATION_Y, 45.f, 90.f);
    SetPropertyTest<RSTranslateModifier, RSAnimatableProperty<Vector2f>, Vector2f>(
        RSModifierType::TRANSLATE, Vector2f(10.f, 10.f), Vector2f(1.f, 1.f));
    SetPropertyTest<RSTranslateZModifier, RSAnimatableProperty<float>, float>(
        RSModifierType::TRANSLATE_Z, 45.f, 90.f);
    SetPropertyTest<RSScaleModifier, RSAnimatableProperty<Vector2f>, Vector2f>(
        RSModifierType::SCALE, Vector2f(0.f, 0.f), Vector2f(1.f, 1.f));
    SetPropertyTest<RSSkewModifier, RSAnimatableProperty<Vector2f>, Vector2f>(
        RSModifierType::SKEW, Vector2f(1.f, 1.f), Vector2f(-1.f, -1.f));
    SetPropertyTest<RSPerspModifier, RSAnimatableProperty<Vector2f>, Vector2f>(
        RSModifierType::PERSP, Vector2f(FLOAT_DATA_INIT, FLOAT_DATA_INIT),
        Vector2f(FLOAT_DATA_UPDATE, FLOAT_DATA_UPDATE));
    SetPropertyTest<RSForegroundColorModifier, RSAnimatableProperty<Color>, Color>(
        RSModifierType::FOREGROUND_COLOR, Color(), Color(0xFF00FF00));
    SetPropertyTest<RSBackgroundColorModifier, RSAnimatableProperty<Color>, Color>(
        RSModifierType::BACKGROUND_COLOR, Color(), Color(0xFF00FF00));
    SetPropertyTest<RSBgImageModifier, RSProperty<std::shared_ptr<RSImage>>, std::shared_ptr<RSImage>>(
        RSModifierType::BG_IMAGE, std::make_shared<RSImage>(), nullptr);
    SetPropertyTest<RSBgImageWidthModifier, RSAnimatableProperty<float>, float>(
        RSModifierType::BG_IMAGE_WIDTH, 45.f, 90.f);
    SetPropertyTest<RSBgImageHeightModifier, RSAnimatableProperty<float>, float>(
        RSModifierType::BG_IMAGE_HEIGHT, 45.f, 90.f);
    SetPropertyTest<RSBorderColorModifier, RSAnimatableProperty<Vector4<Color>>, Vector4<Color>>(
        RSModifierType::BORDER_COLOR, Vector4<Color>(), Vector4<Color>(Color(0xFF00FF00)));
    SetPropertyTest<RSBorderWidthModifier, RSAnimatableProperty<Vector4f>, Vector4f>(
        RSModifierType::BORDER_WIDTH, Vector4f(), Vector4f(1.f));
    SetPropertyTest<RSBorderStyleModifier, RSProperty<Vector4<uint32_t>>, Vector4<uint32_t>>(
        RSModifierType::BORDER_STYLE, Vector4<uint32_t>(), Vector4<uint32_t>(1));
    SetPropertyTest<RSShadowColorModifier, RSAnimatableProperty<Color>, Color>(
        RSModifierType::SHADOW_COLOR, Color(), Color(0xFF00FF00));
    SetPropertyTest<RSShadowOffsetXModifier, RSAnimatableProperty<float>, float>(
        RSModifierType::SHADOW_OFFSET_X, 1.f, 2.f);
    SetPropertyTest<RSShadowOffsetYModifier, RSAnimatableProperty<float>, float>(
        RSModifierType::SHADOW_OFFSET_Y, 1.f, 2.f);
    SetPropertyTest<RSShadowAlphaModifier, RSAnimatableProperty<float>, float>(
        RSModifierType::SHADOW_ALPHA, 0.2f, 0.5f);
    SetPropertyTest<RSShadowElevationModifier, RSAnimatableProperty<float>, float>(
        RSModifierType::SHADOW_ELEVATION, 1.f, 2.f);
    SetPropertyTest<RSShadowRadiusModifier, RSAnimatableProperty<float>, float>(
        RSModifierType::SHADOW_RADIUS, 1.f, 2.f);
    SetPropertyTest<RSShadowPathModifier, RSProperty<std::shared_ptr<RSPath>>, std::shared_ptr<RSPath>>(
        RSModifierType::SHADOW_PATH, RSPath::CreateRSPath(), nullptr);
    SetPropertyTest<RSFrameGravityModifier, RSProperty<Gravity>, Gravity>(
        RSModifierType::FRAME_GRAVITY, Gravity::TOP_RIGHT, Gravity::RESIZE);
    SetPropertyTest<RSClipBoundsModifier, RSProperty<std::shared_ptr<RSPath>>, std::shared_ptr<RSPath>>(
        RSModifierType::CLIP_BOUNDS, RSPath::CreateRSPath(), nullptr);
    SetPropertyTest<RSClipToBoundsModifier, RSProperty<bool>, bool>(
        RSModifierType::CLIP_TO_BOUNDS, false, true);
    SetPropertyTest<RSClipToFrameModifier, RSProperty<bool>, bool>(
        RSModifierType::CLIP_TO_FRAME, false, true);
    SetPropertyTest<RSVisibleModifier, RSProperty<bool>, bool>(
        RSModifierType::VISIBLE, false, true);
    SetPropertyTest<RSMaskModifier, RSProperty<std::shared_ptr<RSMask>>, std::shared_ptr<RSMask>>(
        RSModifierType::MASK, std::make_shared<RSMask>(), nullptr);
}

/**
 * @tc.name: SetModifier001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetModifier001, TestSize.Level1)
{
    RSSurfaceNodeConfig surfaceNodeConfig;
    auto node = RSSurfaceNode::Create(surfaceNodeConfig, false);
    auto value = Vector4f(100.f);
    auto prop = std::make_shared<RSAnimatableProperty<Vector4f>>(value);
    auto modifier = std::make_shared<RSBoundsModifier>(prop);

    node->AddModifier(nullptr);
    node->RemoveModifier(modifier);
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetBounds(), value);
    node->AddModifier(modifier);
    node->RemoveModifier(modifier);
}


class RSC_EXPORT MockRSNode : public RSNode {
public:
    MockRSNode() : RSNode(false) {}
    virtual ~MockRSNode() = default;
    std::vector<PropertyId> GetModifierIds() const {
        return RSNode::GetModifierIds();
    }
};

/**
 * @tc.name: SetModifier002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetModifier002, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    node->SetMotionPathOption(std::make_shared<RSMotionPathOption>(""));
    {
        auto value = Vector4f(100.f);
        auto prop = std::make_shared<RSAnimatableProperty<Vector4f>>(value);
        auto modifier = std::make_shared<RSBoundsModifier>(prop);
        node->AddModifier(modifier);
        ASSERT_TRUE(node != nullptr);
        ASSERT_EQ(node->GetStagingProperties().GetBounds(), value);
        node->RemoveModifier(modifier);
    }
    {
        auto value = Vector4f(100.f);
        auto prop = std::make_shared<RSAnimatableProperty<Vector4f>>(value);
        auto modifier = std::make_shared<RSFrameModifier>(prop);
        node->AddModifier(modifier);
        ASSERT_TRUE(node != nullptr);
        ASSERT_EQ(node->GetStagingProperties().GetFrame(), value);
        node->RemoveModifier(modifier);
    }
    {
        auto value = Vector2f(200.f, 300.f);
        auto prop = std::make_shared<RSAnimatableProperty<Vector2f>>(value);
        auto modifier = std::make_shared<RSTranslateModifier>(prop);
        node->AddModifier(modifier);
        ASSERT_TRUE(node != nullptr);
        ASSERT_EQ(node->GetStagingProperties().GetTranslate(), value);
        node->RemoveModifier(modifier);
    }
    {
        auto value = Vector2f(2.f, 2.f);
        auto prop = std::make_shared<RSAnimatableProperty<Vector2f>>(value);
        auto modifier = std::make_shared<RSScaleModifier>(prop);
        node->AddModifier(modifier);
        ASSERT_TRUE(node != nullptr);
        ASSERT_EQ(node->GetStagingProperties().GetScale(), value);
        node->RemoveModifier(modifier);
    }
    {
        auto value = Vector2f(2.f, 2.f);
        auto prop = std::make_shared<RSAnimatableProperty<Vector2f>>(value);
        auto modifier = std::make_shared<RSSkewModifier>(prop);
        node->AddModifier(modifier);
        ASSERT_TRUE(node != nullptr);
        ASSERT_EQ(node->GetStagingProperties().GetSkew(), value);
        node->RemoveModifier(modifier);
    }
    {
        auto value = Vector2f(FLOAT_DATA_INIT, FLOAT_DATA_INIT);
        auto prop = std::make_shared<RSAnimatableProperty<Vector2f>>(value);
        auto modifier = std::make_shared<RSPerspModifier>(prop);
        node->AddModifier(modifier);
        ASSERT_TRUE(node != nullptr);
        ASSERT_EQ(node->GetStagingProperties().GetPersp(), value);
        node->RemoveModifier(modifier);
    }

    auto node1 = std::make_shared<MockRSNode>();
    node1->DumpNode(0);
    auto animation = std::make_shared<RSTransition>(RSTransitionEffect::OPACITY, true);
    animation->SetDuration(100);
    animation->SetTimingCurve(RSAnimationTimingCurve::EASE_IN_OUT);
    animation->SetFinishCallback([]() {});
    node1->AddAnimation(animation);
    node1->DumpNode(0);
    ASSERT_TRUE(node1->GetModifierIds().size() == 0);
    auto value = Vector2f(2.f, 2.f);
    auto prop = std::make_shared<RSAnimatableProperty<Vector2f>>(value);
    auto modifier = std::make_shared<RSScaleModifier>(prop);
    node1->AddModifier(modifier);
    node1->DumpNode(0);
    ASSERT_TRUE(node1->GetModifierIds().size() == 1);
}

/**
 * @tc.name: OpenImplicitAnimationTest001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, OpenImplicitAnimationTest001, TestSize.Level1)
{
    std::function<void()> finishCallback;
    RSAnimationTimingProtocol timingProtocal;
    RSAnimationTimingCurve timingCurve;
    RSNode::OpenImplicitAnimation(timingProtocal, timingCurve, finishCallback);
}
/**
 * @tc.name: CloseImplicitAnimationTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, CloseImplicitAnimationTest, TestSize.Level1)
{
    RSNode::CloseImplicitAnimation();
    std::vector<std::shared_ptr<RSAnimation>> vec;
    EXPECT_EQ(vec, RSNode::CloseImplicitAnimation());
}

/**
 * @tc.name: AnimateTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, AnimateTest, TestSize.Level1)
{
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    PropertyCallback propertyCallback = nullptr;
    std::function<void()> finishCallback;
    auto animate = RSNode::Animate(timingProtocol, timingCurve, propertyCallback, finishCallback);
    std::vector<std::shared_ptr<RSAnimation>> vec;
    EXPECT_EQ(vec, animate);
}
/**
 * @tc.name: AnimateWithCurrentOptionsTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, AnimateWithCurrentOptionsTest, TestSize.Level1)
{
    PropertyCallback propertyCallback = nullptr;
    std::function<void()> finishCallback = nullptr;
    bool timingSensitive = true;
    auto animateWithCurrentOptions =
        RSNode::AnimateWithCurrentOptions(propertyCallback, finishCallback, timingSensitive);
    std::vector<std::shared_ptr<RSAnimation>> vec;
    EXPECT_EQ(vec, animateWithCurrentOptions);
}
/**
 * @tc.name: AnimateWithCurrentCallbackTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, AnimateWithCurrentCallbackTest, TestSize.Level1)
{
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    PropertyCallback propertyCallback = nullptr;
    auto Callback = RSNode::AnimateWithCurrentCallback(timingProtocol, timingCurve, propertyCallback);
    std::vector<std::shared_ptr<RSAnimation>> vec;
    EXPECT_EQ(vec, Callback);
}

/**
 * @tc.name: SetColorBlendMode
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetColorBlendMode, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    RSColorBlendMode blendModeType = RSColorBlendMode::NONE;
    rsNode->SetColorBlendMode(blendModeType);
    blendModeType = RSColorBlendMode::DST_IN;
    rsNode->SetColorBlendMode(blendModeType);
    blendModeType = RSColorBlendMode::SRC_IN;
    rsNode->SetColorBlendMode(blendModeType);
}

/**
 * @tc.name: SetTextureExport
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SetTextureExport, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetTextureExport(true);
    ASSERT_TRUE(rsNode->IsTextureExportNode());
    rsNode->SetTextureExport(false);
    ASSERT_EQ(rsNode->IsTextureExportNode(), false);
}

/**
 * @tc.name: SyncTextureExport
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, SyncTextureExport, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    auto canvasNode = RSCanvasNode::Create();
    rsNode->AddChild(canvasNode, -1);
    rsNode->SyncTextureExport(true);
}

/**
 * @tc.name: CalcExpectedFrameRate
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTest, CalcExpectedFrameRate, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    auto canvasNode = RSCanvasNode::Create();
    rsNode->AddChild(canvasNode, -1);
    auto scene = "test_scene";
    float speed = 0;
    ASSERT_EQ(rsNode->CalcExpectedFrameRate(scene, speed), 0);
}

/**
 * @tc.name: SetFrameNodeInfo
 * @tc.desc: test results of SetFrameNodeInfo
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, SetFrameNodeInfo, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    std::string tag = "tag"; // for test
    rsNode->SetFrameNodeInfo(0, tag);
    EXPECT_EQ(rsNode->frameNodeId_, 0);
}

/**
 * @tc.name: GetFrameNodeId
 * @tc.desc: test results of GetFrameNodeId
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, GetFrameNodeId, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    std::string tag = "tag"; // for test
    rsNode->SetFrameNodeInfo(0, tag);
    int32_t id = rsNode->GetFrameNodeId();
    EXPECT_EQ(id, 0);
}

/**
 * @tc.name: GetFrameNodeTag
 * @tc.desc: test results of GetFrameNodeTag
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, GetFrameNodeTag, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    std::string tag = "tag"; // for test
    rsNode->SetFrameNodeInfo(0, tag);
    std::string strResult = rsNode->GetFrameNodeTag();
    const char* cTag = tag.c_str();
    const char* cResult = strResult.c_str();
    int result = strcmp(cTag, cResult);
    EXPECT_EQ(result == 0, true);
}

/**
 * @tc.name: AddKeyFrame
 * @tc.desc: test results of AddKeyFrame
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, AddKeyFrame, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    RSAnimationTimingCurve timingCurve;
    PropertyCallback propertyCallback = []() {};
    rsNode->AddKeyFrame(1.f, timingCurve, propertyCallback);
    rsNode->AddKeyFrame(1.f, propertyCallback);
}

/**
 * @tc.name: AddDurationKeyFrame
 * @tc.desc: test results of AddDurationKeyFrame
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, AddDurationKeyFrame, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    RSAnimationTimingCurve timingCurve;
    PropertyCallback propertyCallback = []() {};
    rsNode->AddDurationKeyFrame(1, timingCurve, propertyCallback);
}

/**
 * @tc.name: IsImplicitAnimationOpen
 * @tc.desc: test results of IsImplicitAnimationOpen
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, IsImplicitAnimationOpen, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    bool res = rsNode->IsImplicitAnimationOpen();
    EXPECT_EQ(res, false);
}

/**
 * @tc.name: ExecuteWithoutAnimation
 * @tc.desc: test results of ExecuteWithoutAnimation
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, ExecuteWithoutAnimation, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    PropertyCallback callback;
    std::shared_ptr<RSImplicitAnimator> implicitAnimator;
    rsNode->ExecuteWithoutAnimation(callback, implicitAnimator);
    EXPECT_EQ(callback, nullptr);

    callback = []() {};
    rsNode->ExecuteWithoutAnimation(callback, implicitAnimator);
    EXPECT_EQ(implicitAnimator, nullptr);

    implicitAnimator = std::make_shared<RSImplicitAnimator>();
    rsNode->ExecuteWithoutAnimation(callback, implicitAnimator);
    EXPECT_NE(implicitAnimator, nullptr);
}

/**
 * @tc.name: FallbackAnimationsToRoot
 * @tc.desc: test results of FallbackAnimationsToRoot
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, FallbackAnimationsToRoot, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->FallbackAnimationsToRoot();
    EXPECT_EQ(rsNode->motionPathOption_, nullptr);

    bool isRenderServiceNode = true;
    auto target = RSNodeMap::Instance().GetAnimationFallbackNode();
    target = std::make_shared<RSNode>(isRenderServiceNode);
    AnimationId id = 1;
    auto animation = std::make_shared<RSAnimation>();
    target->animations_.insert({ id, animation });
    rsNode->FallbackAnimationsToRoot();
    EXPECT_NE(target, nullptr);
}

/**
 * @tc.name: AddAnimationInner
 * @tc.desc: test results of AddAnimationInner
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, AddAnimationInner, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    std::shared_ptr<RSAnimation> animation = std::make_shared<RSAnimation>();
    rsNode->AddAnimationInner(animation);
    EXPECT_NE(animation, nullptr);
}

/**
 * @tc.name: RemoveAnimationInner
 * @tc.desc: test results of RemoveAnimationInner
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, RemoveAnimationInner, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    auto animation = std::make_shared<RSAnimation>();
    rsNode->AddAnimationInner(animation);
    rsNode->RemoveAnimationInner(animation);
    EXPECT_NE(animation, nullptr);
}

/**
 * @tc.name: FinishAnimationByProperty
 * @tc.desc: test results of FinishAnimationByProperty
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, FinishAnimationByProperty, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    PropertyId id = 0; // for test
    auto animation = std::make_shared<RSAnimation>();
    rsNode->AddAnimationInner(animation);
    rsNode->FinishAnimationByProperty(id);
    EXPECT_NE(animation, nullptr);
}

/**
 * @tc.name: CancelAnimationByProperty
 * @tc.desc: test results of CancelAnimationByProperty
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, CancelAnimationByProperty, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    PropertyId id = 0; // for test
    rsNode->animatingPropertyNum_.insert({ id, 1 });
    delete RSTransactionProxy::instance_;
    RSTransactionProxy::instance_ = nullptr;
    rsNode->CancelAnimationByProperty(id, true);
    EXPECT_EQ(RSTransactionProxy::GetInstance(), nullptr);

    RSTransactionProxy::instance_ = new RSTransactionProxy();
    rsNode->CancelAnimationByProperty(id, true);
    EXPECT_NE(RSTransactionProxy::GetInstance(), nullptr);
}

/**
 * @tc.name: GetShowingProperties
 * @tc.desc: test results of GetShowingProperties
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, GetShowingProperties, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->GetShowingProperties();
}

/**
 * @tc.name: AddAnimation
 * @tc.desc: test results of AddAnimation
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, AddAnimation, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    std::shared_ptr<RSAnimation> animation;
    rsNode->AddAnimation(animation);
    EXPECT_EQ(animation, nullptr);
}

/**
 * @tc.name: RemoveAllAnimations
 * @tc.desc: test results of RemoveAllAnimations
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, RemoveAllAnimations, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    AnimationId id = 1;
    auto animation = std::make_shared<RSAnimation>();
    rsNode->animations_.insert({ id, animation });
    rsNode->RemoveAllAnimations();
    EXPECT_NE(animation, nullptr);
}

/**
 * @tc.name: RemoveAnimation
 * @tc.desc: test results of RemoveAnimation
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, RemoveAnimation, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    std::shared_ptr<RSAnimation> animation;
    rsNode->RemoveAnimation(animation);
    EXPECT_EQ(animation, nullptr);

    AnimationId id = 1;
    animation = std::make_shared<RSAnimation>();
    rsNode->RemoveAnimation(animation);
    EXPECT_NE(animation, nullptr);

    rsNode->animations_.insert({ id, animation });
    rsNode->RemoveAnimation(animation);
    EXPECT_NE(animation, nullptr);
}

/**
 * @tc.name: SetMotionPathOption
 * @tc.desc: test results of SetMotionPathOption
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, SetMotionPathOption, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    std::string path = "";
    auto motionPathOption = std::make_shared<RSMotionPathOption>(path);
    rsNode->SetMotionPathOption(motionPathOption);
    EXPECT_NE(motionPathOption, nullptr);
}

/**
 * @tc.name: GetMotionPathOption
 * @tc.desc: test results of GetMotionPathOption
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, GetMotionPathOption, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    std::string path = "";
    auto motionPathOption = std::make_shared<RSMotionPathOption>(path);
    rsNode->SetMotionPathOption(motionPathOption);
    rsNode->GetMotionPathOption();
    EXPECT_NE(motionPathOption, nullptr);
}

/**
 * @tc.name: HasPropertyAnimation
 * @tc.desc: test results of HasPropertyAnimation
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, HasPropertyAnimation, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    PropertyId id = 1;
    bool res = rsNode->HasPropertyAnimation(id);
    EXPECT_EQ(res, false);
}

/**
 * @tc.name: GetAnimationByPropertyId
 * @tc.desc: test results of GetAnimationByPropertyId
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, GetAnimationByPropertyId, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    PropertyId id = 1;
    AnimationId animationId = 1;
    auto animation = std::make_shared<RSAnimation>();
    rsNode->animations_.insert({ animationId, animation });
    rsNode->GetAnimationByPropertyId(id);
    EXPECT_NE(animation, nullptr);
}

/**
 * @tc.name: SetProperty
 * @tc.desc: test results of SetProperty
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, SetProperty, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetProperty<RSAlphaModifier, RSAnimatableProperty<float>>(RSModifierType::ALPHA, 1.f);
}

/**
 * @tc.name: SetBounds001
 * @tc.desc: test results of SetBounds
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, SetBounds001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    Vector4f bounds = { 1.f, 1.f, 1.f, 1.f }; // for test
    rsNode->SetBounds(bounds);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBounds().x_, 1.f));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBounds().y_, 1.f));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBounds().z_, 1.f));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBounds().w_, 1.f));
}

/**
 * @tc.name: SetSandBox
 * @tc.desc: test results of SetSandBox
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, SetSandBox, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    std::optional<Vector2f> parentPosition;
    rsNode->SetSandBox(parentPosition);

    Vector2f newPosition(1.0f, 2.0f);
    parentPosition = newPosition;
    rsNode->SetSandBox(parentPosition);
    EXPECT_EQ(parentPosition, newPosition);
}

/**
 * @tc.name: SetPivotZ
 * @tc.desc: test results of SetPivotZ
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, SetPivotZ, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    float pivotZ = 1.f; // for test
    rsNode->SetPivotZ(pivotZ);
    EXPECT_EQ(pivotZ, 1.f);
}

/**
 * @tc.name: SetCornerRadius
 * @tc.desc: test results of SetCornerRadius
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, SetCornerRadius, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    Vector4f cornerRadius = { 1.f, 1.f, 1.f, 1.f }; // for test
    rsNode->SetCornerRadius(cornerRadius);
}

/**
 * @tc.name: SetCameraDistance
 * @tc.desc: test results of SetCameraDistance
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, SetCameraDistance, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    float cameraDistance = 1.f; // for test
    rsNode->SetCameraDistance(cameraDistance);
    EXPECT_EQ(cameraDistance, 1.f);
}

/**
 * @tc.name: SetEnvForegroundColor
 * @tc.desc: test results of SetEnvForegroundColor
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, SetEnvForegroundColor, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    uint32_t colorValue = 1; // for test
    rsNode->SetEnvForegroundColor(colorValue);
    EXPECT_EQ(colorValue, 1);
}

/**
 * @tc.name: SetEnvForegroundColorStrategy
 * @tc.desc: test results of SetEnvForegroundColorStrategy
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, SetEnvForegroundColorStrategy, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    ForegroundColorStrategyType strategyType = ForegroundColorStrategyType::INVERT_BACKGROUNDCOLOR;
    rsNode->SetEnvForegroundColorStrategy(strategyType);
    EXPECT_EQ(strategyType, ForegroundColorStrategyType::INVERT_BACKGROUNDCOLOR);
}

/**
 * @tc.name: SetParticleParams
 * @tc.desc: test results of SetParticleParams
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, SetParticleParams, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    ParticleParams params;
    std::vector<ParticleParams> particleParams;
    particleParams.push_back(params);
    std::function<void()> finishCallback = []() {};
    rsNode->SetParticleParams(particleParams, finishCallback);
    EXPECT_EQ(particleParams.empty(), false);
}

/**
 * @tc.name: SetParticleDrawRegion
 * @tc.desc: test results of SetParticleDrawRegion
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, SetParticleDrawRegion, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    EmitterConfig emitterConfig;
    std::shared_ptr<RSImage> image;
    ParticleParams params;
    params.emitterConfig_ = emitterConfig;
    std::vector<ParticleParams> particleParams;
    particleParams.push_back(params);
    rsNode->SetParticleDrawRegion(particleParams);
    EXPECT_EQ(particleParams.empty(), false);

    params.emitterConfig_.type_ = ParticleType::IMAGES;
    rsNode->SetParticleDrawRegion(particleParams);
    EXPECT_EQ(particleParams.empty(), false);

    image = std::make_shared<RSImage>();
    emitterConfig.image_ = image;
    rsNode->SetParticleDrawRegion(particleParams);
    EXPECT_EQ(particleParams.empty(), false);
}

/**
 * @tc.name: SetEmitterUpdater
 * @tc.desc: test results of SetEmitterUpdater
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, SetEmitterUpdater, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    std::vector<std::shared_ptr<EmitterUpdater>> para;
    rsNode->SetEmitterUpdater(para);
    EXPECT_EQ(para.empty(), true);
}

/**
 * @tc.name: SetParticleNoiseFields
 * @tc.desc: test results of SetParticleNoiseFields
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, SetParticleNoiseFields, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    std::shared_ptr<ParticleNoiseFields> para = std::make_shared<ParticleNoiseFields>();
    rsNode->SetParticleNoiseFields(para);
    EXPECT_NE(para, nullptr);
}

/**
 * @tc.name: SetBgImageInnerRect
 * @tc.desc: test results of SetBgImageInnerRect
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, SetBgImageInnerRect, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    Vector4f rect = { 1.f, 1.f, 1.f, 1.f }; // for test
    rsNode->SetBgImageInnerRect(rect);
    EXPECT_NE(rsNode, nullptr);
}

/**
 * @tc.name: SetOuterBorderColor
 * @tc.desc: test results of SetOuterBorderColor
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, SetOuterBorderColor, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    Color example(255, 0, 0, 255); // for test
    Vector4<Color> color = { example, example, example, example };
    rsNode->SetOuterBorderColor(color);
    EXPECT_NE(rsNode, nullptr);
}

/**
 * @tc.name: SetOuterBorderWidth
 * @tc.desc: test results of SetOuterBorderWidth
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, SetOuterBorderWidth, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    Vector4f width = { 1.f, 1.f, 1.f, 1.f }; // for test
    rsNode->SetOuterBorderWidth(width);
    EXPECT_NE(rsNode, nullptr);
}

/**
 * @tc.name: SetOuterBorderStyle
 * @tc.desc: test results of SetOuterBorderStyle
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, SetOuterBorderStyle, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    Vector4<BorderStyle> style = { BorderStyle::SOLID, BorderStyle::DASHED, BorderStyle::DOTTED,
        BorderStyle::NONE }; // for test
    rsNode->SetOuterBorderStyle(style);
    EXPECT_NE(rsNode, nullptr);
}

/**
 * @tc.name: SetOuterBorderRadius
 * @tc.desc: test results of SetOuterBorderRadius
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, SetOuterBorderRadius, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    Vector4f radius = { 1.f, 1.f, 1.f, 1.f }; // for test
    rsNode->SetOuterBorderRadius(radius);
    EXPECT_NE(rsNode, nullptr);
}

/**
 * @tc.name: SetForegroundEffectRadius
 * @tc.desc: test results of SetForegroundEffectRadius
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, SetForegroundEffectRadius, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetForegroundEffectRadius(1.f);
    EXPECT_NE(rsNode, nullptr);
}

/**
 * @tc.name: SetLinearGradientBlurPara
 * @tc.desc: test results of SetLinearGradientBlurPara
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, SetLinearGradientBlurPara, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    std::vector<std::pair<float, float>> fractionStops;
    GradientDirection direction = GradientDirection::LEFT;
    auto para = std::make_shared<RSLinearGradientBlurPara>(1.f, fractionStops, direction);
    rsNode->SetLinearGradientBlurPara(para);
    EXPECT_NE(fractionStops.empty(), false);
}

/**
 * @tc.name: SetMotionBlurPara
 * @tc.desc: test results of SetMotionBlurPara
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, SetMotionBlurPara, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    Vector2f anchor = { 1.f, 1.f }; // for test
    rsNode->SetMotionBlurPara(1.f, anchor);
    EXPECT_NE(anchor[0], 0.f);
}

/**
 * @tc.name: SetDynamicLightUpRate
 * @tc.desc: test results of SetDynamicLightUpRate
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, SetDynamicLightUpRate, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    float rate = 1.f; // for test
    rsNode->SetDynamicLightUpRate(rate);
    EXPECT_NE(rate, 0.f);
}

/**
 * @tc.name: SetDynamicLightUpDegree
 * @tc.desc: test results of SetDynamicLightUpDegree
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, SetDynamicLightUpDegree, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    float lightUpDegree = 1.f; // for test
    rsNode->SetDynamicLightUpDegree(lightUpDegree);
    EXPECT_NE(lightUpDegree, 0.f);
}

/**
 * @tc.name: SetFgBrightnessParams
 * @tc.desc: test results of SetFgBrightnessParams
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, SetFgBrightnessParams, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    RSDynamicBrightnessPara params;
    rsNode->SetFgBrightnessParams(params);
    EXPECT_NE(params.rate_, 0.f);
}

/**
 * @tc.name: SetFgBrightnessFract
 * @tc.desc: test results of SetFgBrightnessFract
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, SetFgBrightnessFract, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    float fract = 1.f; // for test
    rsNode->SetFgBrightnessFract(fract);
    EXPECT_NE(fract, 0.f);
}

/**
 * @tc.name: SetBgBrightnessParams
 * @tc.desc: test results of SetBgBrightnessParams
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, SetBgBrightnessParams, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    RSDynamicBrightnessPara params;
    rsNode->SetBgBrightnessParams(params);
    EXPECT_NE(params.rate_, 0.f);
}

/**
 * @tc.name: SetBgBrightnessFract
 * @tc.desc: test results of SetBgBrightnessFract
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, SetBgBrightnessFract, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    float fract = 1.f; // for test
    rsNode->SetBgBrightnessFract(fract);
    EXPECT_NE(fract, 0.f);
}

/**
 * @tc.name: SetDynamicDimDegree
 * @tc.desc: test results of SetDynamicDimDegree
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, SetDynamicDimDegree, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    float dimDegree = 1.f; // for test
    rsNode->SetDynamicDimDegree(dimDegree);
    EXPECT_NE(dimDegree, 0.f);
}

/**
 * @tc.name: SetGreyCoef
 * @tc.desc: test results of SetGreyCoef
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, SetGreyCoef, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    Vector2f greyCoef = { 1.f, 1.f }; // for test
    rsNode->SetGreyCoef(greyCoef);
    EXPECT_NE(greyCoef[0], 0.f);
}

/**
 * @tc.name: SetShadowMask
 * @tc.desc: test results of SetShadowMask
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, SetShadowMask, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    bool shadowMask = true;
    rsNode->SetShadowMask(shadowMask);
    EXPECT_EQ(shadowMask, true);
}

/**
 * @tc.name: SetShadowIsFilled
 * @tc.desc: test results of SetShadowIsFilled
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, SetShadowIsFilled, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    bool shadowIsFilled = true;
    rsNode->SetShadowIsFilled(shadowIsFilled);
    EXPECT_EQ(shadowIsFilled, true);
}

/**
 * @tc.name: SetShadowColorStrategy
 * @tc.desc: test results of SetShadowColorStrategy
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, SetShadowColorStrategy, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    int shadowColorStrategy = 1; // for test
    rsNode->SetShadowColorStrategy(shadowColorStrategy);
    EXPECT_EQ(shadowColorStrategy, 1);
}

/**
 * @tc.name: SetClipRRect
 * @tc.desc: test results of SetClipRRect
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, SetClipRRect, TestSize.Level1)
{
    Vector4f clipRect = { 1.f, 1.f, 1.f, 1.f }; // for test
    auto rsNode = RSCanvasNode::Create();
    Vector4f clipRadius = { 0.f, 1.f, 0.f, 1.f }; // for test
    rsNode->SetClipRRect(clipRect, clipRadius);
    EXPECT_EQ(clipRect[0], 1.f);
}

/**
 * @tc.name: SetMask
 * @tc.desc: test results of SetMask
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, SetMask, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    std::shared_ptr<RSMask> mask = std::make_shared<RSMask>();
    rsNode->SetMask(mask);
    EXPECT_NE(mask, nullptr);
}

/**
 * @tc.name: SetUseEffect
 * @tc.desc: test results of SetUseEffect
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, SetUseEffect, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    bool useEffect = true;
    rsNode->SetUseEffect(useEffect);
    EXPECT_EQ(useEffect, true);
}

/**
 * @tc.name: SetUseShadowBatching
 * @tc.desc: test results of SetUseShadowBatching
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, SetUseShadowBatching, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    bool useShadowBatching = true;
    rsNode->SetUseShadowBatching(useShadowBatching);
    EXPECT_EQ(useShadowBatching, true);
}

/**
 * @tc.name: SetColorBlendApplyType
 * @tc.desc: test results of SetColorBlendApplyType
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, SetColorBlendApplyType, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    RSColorBlendApplyType colorBlendApplyType = RSColorBlendApplyType::FAST;
    rsNode->SetColorBlendApplyType(colorBlendApplyType);
    colorBlendApplyType = RSColorBlendApplyType::SAVE_LAYER;
    rsNode->SetColorBlendApplyType(colorBlendApplyType);
    colorBlendApplyType = RSColorBlendApplyType::MAX;
    rsNode->SetColorBlendApplyType(colorBlendApplyType);
}

/**
 * @tc.name: SetPixelStretch
 * @tc.desc: test results of SetPixelStretch
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, SetPixelStretch, TestSize.Level1)
{
    Vector4f stretchSize = { 1.f, 1.f, 1.f, 1.f }; // for test
    auto rsNode = RSCanvasNode::Create();
    Drawing::TileMode stretchTileMode = Drawing::TileMode::CLAMP;
    rsNode->SetPixelStretch(stretchSize, stretchTileMode);
    stretchTileMode = Drawing::TileMode::REPEAT;
    rsNode->SetPixelStretch(stretchSize, stretchTileMode);
    stretchTileMode = Drawing::TileMode::MIRROR;
    rsNode->SetPixelStretch(stretchSize, stretchTileMode);
    stretchTileMode = Drawing::TileMode::DECAL;
    rsNode->SetPixelStretch(stretchSize, stretchTileMode);
    EXPECT_EQ(stretchSize[0], 1.f);
}

/**
 * @tc.name: SetPixelStretchPercent
 * @tc.desc: test results of SetPixelStretchPercent
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, SetPixelStretchPercent, TestSize.Level1)
{
    Vector4f stretchPercent = { 1.f, 1.f, 1.f, 1.f }; // for test
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetPixelStretchPercent(stretchPercent);
    EXPECT_EQ(stretchPercent[0], 1.f);
}

/**
 * @tc.name: SetNodeName
 * @tc.desc: test results of SetNodeName
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeTest, SetNodeName, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    std::string nodeName = "1"; // for test
    rsNode->SetNodeName(nodeName);
    EXPECT_NE(RSTransactionProxy::instance_, nullptr);
}

/**
 * @tc.name: SetTakeSurfaceForUIFlag
 * @tc.desc: test results of SetTakeSurfaceForUIFlag
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, SetTakeSurfaceForUIFlag, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetTakeSurfaceForUIFlag();
    EXPECT_NE(RSTransactionProxy::instance_, nullptr);
}

/**
 * @tc.name: SetSpherizeDegree
 * @tc.desc: test results of SetSpherizeDegree
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, SetSpherizeDegree, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    float spherizeDegree = 1.f; // for test
    rsNode->SetSpherizeDegree(spherizeDegree);
    EXPECT_NE(spherizeDegree, 0.f);
}

/**
 * @tc.name: SetLightUpEffectDegree
 * @tc.desc: test results of SetLightUpEffectDegree
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, SetLightUpEffectDegree, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    float degree = 1.f; // for test
    rsNode->SetLightUpEffectDegree(degree);
    EXPECT_NE(degree, 0.f);
}

/**
 * @tc.name: NotifyTransition
 * @tc.desc: test results of NotifyTransition
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, NotifyTransition, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    bool isTransitionIn = true;
    std::shared_ptr<const RSTransitionEffect> effect;
    rsNode->NotifyTransition(effect, isTransitionIn);
    EXPECT_NE(isTransitionIn, false);

    rsNode->implicitAnimator_ = std::make_shared<RSImplicitAnimator>();
    rsNode->NotifyTransition(effect, isTransitionIn);
    EXPECT_NE(rsNode->implicitAnimator_, nullptr);
}

/**
 * @tc.name: OnAddChildren
 * @tc.desc: test results of OnAddChildren
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, OnAddChildren, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->transitionEffect_ = std::make_shared<const RSTransitionEffect>();
    rsNode->OnAddChildren();
    EXPECT_NE(rsNode->transitionEffect_, nullptr);
}

/**
 * @tc.name: OnRemoveChildren
 * @tc.desc: test results of OnRemoveChildren
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, OnRemoveChildren, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->transitionEffect_ = std::make_shared<const RSTransitionEffect>();
    rsNode->OnRemoveChildren();
    EXPECT_NE(rsNode->transitionEffect_, nullptr);
}

/**
 * @tc.name: SetBackgroundBlurRadius
 * @tc.desc: test results of SetBackgroundBlurRadius
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, SetBackgroundBlurRadius, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    float radius = 1.f; // for test
    rsNode->SetBackgroundBlurRadius(radius);
    EXPECT_NE(radius, 0.f);
}

/**
 * @tc.name: SetBackgroundBlurSaturation
 * @tc.desc: test results of SetBackgroundBlurSaturation
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, SetBackgroundBlurSaturation, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    float saturation = 1.f; // for test
    rsNode->SetBackgroundBlurSaturation(saturation);
    EXPECT_NE(saturation, 0.f);
}

/**
 * @tc.name: SetBackgroundBlurBrightness
 * @tc.desc: test results of SetBackgroundBlurBrightness
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, SetBackgroundBlurBrightness, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    float brightness = 1.f; // for test
    rsNode->SetBackgroundBlurBrightness(brightness);
    EXPECT_NE(brightness, 0.f);
}

/**
 * @tc.name: SetBackgroundBlurMaskColor
 * @tc.desc: test results of SetBackgroundBlurMaskColor
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, SetBackgroundBlurMaskColor, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    Color maskColor(0, 255, 255, 0); // for test
    rsNode->SetBackgroundBlurMaskColor(maskColor);
    EXPECT_EQ(maskColor.GetAlpha(), 0);
}

/**
 * @tc.name: SetBackgroundBlurColorMode
 * @tc.desc: test results of SetBackgroundBlurColorMode
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, SetBackgroundBlurColorMode, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    int colorMode = 1; // for test
    rsNode->SetBackgroundBlurColorMode(colorMode);
    EXPECT_NE(colorMode, 0);
}

/**
 * @tc.name: SetBackgroundBlurRadiusX
 * @tc.desc: test results of SetBackgroundBlurRadiusX
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, SetBackgroundBlurRadiusX, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    float blurRadiusX = 1.f; // for test
    rsNode->SetBackgroundBlurRadiusX(blurRadiusX);
    EXPECT_NE(blurRadiusX, 0.f);
}

/**
 * @tc.name: SetBackgroundBlurRadiusY
 * @tc.desc: test results of SetBackgroundBlurRadiusY
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, SetBackgroundBlurRadiusY, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    float blurRadiusY = 1.f; // for test
    rsNode->SetBackgroundBlurRadiusY(blurRadiusY);
    EXPECT_NE(blurRadiusY, 0.f);
}

/**
 * @tc.name: SetForegroundBlurRadius
 * @tc.desc: test results of SetForegroundBlurRadius
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, SetForegroundBlurRadius, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    float radius = 1.f; // for test
    rsNode->SetForegroundBlurRadius(radius);
    EXPECT_NE(radius, 0.f);
}

/**
 * @tc.name: SetForegroundBlurSaturation
 * @tc.desc: test results of SetForegroundBlurSaturation
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, SetForegroundBlurSaturation, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    float saturation = 1.f; // for test
    rsNode->SetForegroundBlurSaturation(saturation);
    EXPECT_NE(saturation, 0.f);
}

/**
 * @tc.name: SetForegroundBlurBrightness
 * @tc.desc: test results of SetForegroundBlurBrightness
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, SetForegroundBlurBrightness, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    float brightness = 1.f; // for test
    rsNode->SetForegroundBlurBrightness(brightness);
    EXPECT_NE(brightness, 0.f);
}

/**
 * @tc.name: SetForegroundBlurMaskColor
 * @tc.desc: test results of SetForegroundBlurMaskColor
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, SetForegroundBlurMaskColor, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    Color maskColor(0, 255, 255, 0); // for test
    rsNode->SetForegroundBlurMaskColor(maskColor);
    EXPECT_EQ(maskColor.GetAlpha(), 0);
}

/**
 * @tc.name: SetForegroundBlurColorMode
 * @tc.desc: test results of SetForegroundBlurColorMode
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, SetForegroundBlurColorMode, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    int colorMode = 1; // for test
    rsNode->SetForegroundBlurColorMode(colorMode);
    EXPECT_NE(colorMode, 0);
}

/**
 * @tc.name: SetForegroundBlurRadiusX
 * @tc.desc: test results of SetForegroundBlurRadiusX
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, SetForegroundBlurRadiusX, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    float blurRadiusX = 1.f; // for test
    rsNode->SetForegroundBlurRadiusX(blurRadiusX);
    EXPECT_NE(blurRadiusX, 0.f);
}

/**
 * @tc.name: SetForegroundBlurRadiusY
 * @tc.desc: test results of SetForegroundBlurRadiusY
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, SetForegroundBlurRadiusY, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    float blurRadiusY = 1.f; // for test
    rsNode->SetForegroundBlurRadiusY(blurRadiusY);
    EXPECT_NE(blurRadiusY, 0.f);
}

/**
 * @tc.name: AnimationCallback
 * @tc.desc: test results of AnimationCallback
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, AnimationCallback, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    AnimationId animationId = 1;
    AnimationCallbackEvent event = FINISHED;
    bool res = rsNode->AnimationCallback(animationId, event);
    EXPECT_EQ(res, false);

    auto animation = std::make_shared<RSAnimation>();
    rsNode->animations_.insert({ animationId, animation });
    res = rsNode->AnimationCallback(animationId, event);
    EXPECT_EQ(res, true);

    event = REPEAT_FINISHED;
    res = rsNode->AnimationCallback(animationId, event);
    EXPECT_EQ(res, true);

    event = LOGICALLY_FINISHED;
    res = rsNode->AnimationCallback(animationId, event);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: ClearAllModifiers
 * @tc.desc: test results of ClearAllModifiers
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, ClearAllModifiers, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    PropertyId id = 1;
    std::shared_ptr<RSPropertyBase> property;
    auto value = Vector4f(100.f);
    auto prop = std::make_shared<RSAnimatableProperty<Vector4f>>(value);
    auto modifier = std::make_shared<RSBoundsModifier>(prop);
    rsNode->modifiers_[id] = modifier;
    rsNode->ClearAllModifiers();
    EXPECT_NE(modifier, nullptr);
}

/**
 * @tc.name: DoFlushModifier
 * @tc.desc: test results of DoFlushModifier
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, DoFlushModifier, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->DoFlushModifier();
    EXPECT_EQ(rsNode->modifiers_.empty(), true);

    PropertyId id = 1;
    auto value = Vector4f(100.f);
    auto prop = std::make_shared<RSAnimatableProperty<Vector4f>>(value);
    auto modifier = std::make_shared<RSBoundsModifier>(prop);
    rsNode->modifiers_[id] = modifier;
    rsNode->DoFlushModifier();
    EXPECT_NE(RSTransactionProxy::instance_, nullptr);
}

/**
 * @tc.name: GetModifier
 * @tc.desc: test results of GetModifier
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, GetModifier, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    PropertyId propertyId = 1;
    rsNode->GetModifier(propertyId);
    EXPECT_EQ(rsNode->modifiers_.empty(), true);

    auto value = Vector4f(100.f);
    auto prop = std::make_shared<RSAnimatableProperty<Vector4f>>(value);
    auto modifier = std::make_shared<RSBoundsModifier>(prop);
    rsNode->modifiers_[propertyId] = modifier;
    rsNode->GetModifier(propertyId);
    EXPECT_EQ(rsNode->modifiers_.empty(), false);
}

/**
 * @tc.name: UpdateModifierMotionPathOption
 * @tc.desc: test results of UpdateModifierMotionPathOption
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, UpdateModifierMotionPathOption, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    RSModifierType modifierType = RSModifierType::BOUNDS;
    PropertyId propertyId = 1;
    auto value = Vector4f(100.f);
    auto prop = std::make_shared<RSAnimatableProperty<Vector4f>>(value);
    auto modifier = std::make_shared<RSBoundsModifier>(prop);
    rsNode->modifiers_[propertyId] = modifier;
    rsNode->propertyModifiers_[modifierType] = modifier;
    rsNode->UpdateModifierMotionPathOption();
    EXPECT_EQ(rsNode->modifiers_.empty(), false);
    EXPECT_EQ(rsNode->propertyModifiers_.empty(), false);
}

/**
 * @tc.name: UpdateImplicitAnimator
 * @tc.desc: test results of UpdateImplicitAnimator
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, UpdateImplicitAnimator, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->UpdateImplicitAnimator();
}

/**
 * @tc.name: GetModifierIds
 * @tc.desc: test results of GetModifierIds
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, GetModifierIds, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    PropertyId propertyId = 1;
    auto value = Vector4f(100.f);
    auto prop = std::make_shared<RSAnimatableProperty<Vector4f>>(value);
    auto modifier = std::make_shared<RSBoundsModifier>(prop);
    rsNode->modifiers_[propertyId] = modifier;
    rsNode->GetModifierIds();
    EXPECT_EQ(rsNode->modifiers_.empty(), false);
}

/**
 * @tc.name: MarkAllExtendModifierDirty
 * @tc.desc: test results of MarkAllExtendModifierDirty
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, MarkAllExtendModifierDirty, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    PropertyId propertyId = 1;
    auto value = Vector4f(100.f);
    auto prop = std::make_shared<RSAnimatableProperty<Vector4f>>(value);
    auto modifier = std::make_shared<RSBoundsModifier>(prop);
    rsNode->modifiers_[propertyId] = modifier;
    rsNode->MarkAllExtendModifierDirty();
    EXPECT_EQ(rsNode->modifiers_.empty(), false);

    rsNode->MarkAllExtendModifierDirty();
    EXPECT_EQ(rsNode->extendModifierIsDirty_, true);
}

/**
 * @tc.name: ResetExtendModifierDirty
 * @tc.desc: test results of ResetExtendModifierDirty
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, ResetExtendModifierDirty, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->ResetExtendModifierDirty();
    EXPECT_EQ(rsNode->extendModifierIsDirty_, false);
}

/**
 * @tc.name: SetIsCustomTextType
 * @tc.desc: test results of SetIsCustomTextType
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, SetIsCustomTextType, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetIsCustomTextType(true);
    EXPECT_EQ(rsNode->GetIsCustomTextType(), true);
}

/**
 * @tc.name: GetIsCustomTextType
 * @tc.desc: test results of GetIsCustomTextType
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, GetIsCustomTextType, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetIsCustomTextType(true);
    bool res = rsNode->GetIsCustomTextType();
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: SetDrawRegion
 * @tc.desc: test results of SetDrawRegion
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, SetDrawRegion, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    std::shared_ptr<RectF> rect = std::make_shared<RectF>();
    rsNode->SetDrawRegion(rect);
    EXPECT_NE(RSTransactionProxy::instance_, nullptr);
}

/**
 * @tc.name: RegisterTransitionPair
 * @tc.desc: test results of RegisterTransitionPair
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, RegisterTransitionPair, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    NodeId inNodeId = 1;
    NodeId outNodeId = 1;
    rsNode->RegisterTransitionPair(inNodeId, outNodeId);
    EXPECT_NE(RSTransactionProxy::instance_, nullptr);
}

/**
 * @tc.name: UnregisterTransitionPair
 * @tc.desc: test results of UnregisterTransitionPair
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, UnregisterTransitionPair, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    NodeId inNodeId = 1;
    NodeId outNodeId = 1;
    rsNode->UnregisterTransitionPair(inNodeId, outNodeId);
    EXPECT_NE(RSTransactionProxy::instance_, nullptr);
}

/**
 * @tc.name: MarkNodeGroup
 * @tc.desc: test results of MarkNodeGroup
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, MarkNodeGroup, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->MarkNodeGroup(true, true, true);
    EXPECT_NE(RSTransactionProxy::instance_, nullptr);
    delete RSTransactionProxy::instance_;
    RSTransactionProxy::instance_ = nullptr;

    rsNode->MarkNodeGroup(false, true, true);
    EXPECT_EQ(RSTransactionProxy::instance_, nullptr);
    RSTransactionProxy::instance_ = new RSTransactionProxy();
}

/**
 * @tc.name: MarkNodeSingleFrameComposer
 * @tc.desc: test results of MarkNodeSingleFrameComposer
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, MarkNodeSingleFrameComposer, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->MarkNodeSingleFrameComposer(true);
    EXPECT_NE(RSTransactionProxy::instance_, nullptr);
}

/**
 * @tc.name: SetGrayScale
 * @tc.desc: test results of SetGrayScale
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, SetGrayScale, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    float grayScale = 1.f; // for test
    rsNode->SetGrayScale(grayScale);
    EXPECT_NE(grayScale, 0.f);
}

/**
 * @tc.name: SetLightIntensity
 * @tc.desc: test results of SetLightIntensity
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, SetLightIntensity, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    float lightIntensity = 1.f; // for test
    rsNode->SetLightIntensity(lightIntensity);
    EXPECT_NE(lightIntensity, 0.f);
}

/**
 * @tc.name: SetLightColor
 * @tc.desc: test results of SetLightColor
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, SetLightColor, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    uint32_t lightColorValue = 1; // for test
    rsNode->SetLightColor(lightColorValue);
    EXPECT_NE(lightColorValue, 0);
}

/**
 * @tc.name: SetLightPosition
 * @tc.desc: test results of SetLightPosition
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, SetLightPosition, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    float positionX = 1.f; // for test
    float positionY = 2.f; // for test
    float positionZ = 1.f; // for test
    rsNode->SetLightPosition(positionX, positionY, positionZ);
    Vector4f lightPosition = { 0.f, 1.f, 3.f, 0.f }; // for test
    rsNode->SetLightPosition(lightPosition);
    EXPECT_NE(lightPosition[0], 1.f);
}

/**
 * @tc.name: SetIlluminatedType
 * @tc.desc: test results of SetIlluminatedType
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, SetIlluminatedType, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    uint32_t illuminatedType = 1; // for test
    rsNode->SetIlluminatedType(illuminatedType);
    EXPECT_NE(illuminatedType, 0);
}

/**
 * @tc.name: SetFunTest
 * @tc.desc: test results of SetFunTest
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, SetFunTest, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    float position = 1.f; // for test
    rsNode->SetBloom(position);
    rsNode->SetBrightness(position);
    rsNode->SetContrast(position);
    rsNode->SetSaturate(position);
    rsNode->SetSepia(position);
    rsNode->SetInvert(position);
    rsNode->SetHueRotate(position);
    EXPECT_NE(position, 0.f);
}

/**
 * @tc.name: SetAiInvert
 * @tc.desc: test results of SetAiInvert
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, SetAiInvert, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    Vector4f aiInvert = { 1.f, 1.f, 1.f, 1.f }; // for test
    rsNode->SetAiInvert(aiInvert);
    EXPECT_NE(aiInvert[0], 0.f);
}

/**
 * @tc.name: SetSystemBarEffect
 * @tc.desc: test results of SetSystemBarEffect
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, SetSystemBarEffect, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetSystemBarEffect();
}

/**
 * @tc.name: SetColorBlend
 * @tc.desc: test results of SetColorBlend
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, SetColorBlend, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    uint32_t colorValue = 1; // for test
    rsNode->SetColorBlend(colorValue);
    EXPECT_NE(colorValue, 0);
}

/**
 * @tc.name: SetOutOfParent
 * @tc.desc: test results of SetOutOfParent
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, SetOutOfParent, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    OutOfParentType outOfParent = OutOfParentType::OUTSIDE;
    rsNode->SetOutOfParent(outOfParent);
    EXPECT_NE(RSTransactionProxy::instance_, nullptr);
}

/**
 * @tc.name: GenerateId
 * @tc.desc: test results of GenerateId
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, GenerateId, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    NodeId id = rsNode->GenerateId();
    EXPECT_NE(id, 1);
}

/**
 * @tc.name: InitUniRenderEnabled
 * @tc.desc: test results of InitUniRenderEnabled
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, InitUniRenderEnabled, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->InitUniRenderEnabled();
}

/**
 * @tc.name: IsUniRenderEnabled
 * @tc.desc: test results of IsUniRenderEnabled
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, IsUniRenderEnabled, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    bool res = rsNode->IsUniRenderEnabled();
    EXPECT_NE(res, true);
}

/**
 * @tc.name: IsRenderServiceNode
 * @tc.desc: test results of IsRenderServiceNode
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, IsRenderServiceNode, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->isRenderServiceNode_ = true;
    bool res = rsNode->IsRenderServiceNode();
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: MoveChild
 * @tc.desc: test results of MoveChild
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, MoveChild, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    std::shared_ptr<RSNode> child;
    rsNode->MoveChild(child, 1);
    EXPECT_EQ(child, nullptr);
}

/**
 * @tc.name: RemoveChildByNodeId
 * @tc.desc: test results of RemoveChildByNodeId
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, RemoveChildByNodeId, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    NodeId childId = 0;
    rsNode->RemoveChildByNodeId(childId);
}

/**
 * @tc.name: AddCrossParentChild
 * @tc.desc: test results of AddCrossParentChild
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, AddCrossParentChild, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    std::shared_ptr<RSNode> child;
    rsNode->AddCrossParentChild(child, 1);
    EXPECT_EQ(child, nullptr);
}

/**
 * @tc.name: RemoveCrossParentChild
 * @tc.desc: test results of RemoveCrossParentChild
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, RemoveCrossParentChild, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    std::shared_ptr<RSNode> child;
    rsNode->RemoveCrossParentChild(child, 1);
    EXPECT_EQ(child, nullptr);
}

/**
 * @tc.name: RemoveChildById
 * @tc.desc: test results of RemoveChildById
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, RemoveChildById, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    NodeId childId = 1;
    rsNode->children_.push_back(childId);
    rsNode->RemoveChildById(childId);
    EXPECT_EQ(rsNode->children_.empty(), true);
}

/**
 * @tc.name: GetChildIdByIndex
 * @tc.desc: test results of GetChildIdByIndex
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, GetChildIdByIndex, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    NodeId childId = 1;
    rsNode->children_.push_back(childId);
    rsNode->GetChildIdByIndex(-1);
    EXPECT_EQ(rsNode->children_.empty(), false);

    EXPECT_EQ(rsNode->GetChildIdByIndex(-2), std::nullopt);
}

/**
 * @tc.name: SetandGetParent
 * @tc.desc: test results of SetandGetParent
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, SetandGetParent, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetParent(1);
    EXPECT_EQ(rsNode->GetParent(), nullptr);
}

/**
 * @tc.name: IsInstanceOf
 * @tc.desc: test results of IsInstanceOf
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, IsInstanceOf, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    bool res = rsNode->IsInstanceOf(RSUINodeType::CANVAS_NODE);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: SetInstanceId
 * @tc.desc: test results of SetInstanceId
 * @tc.type: FUNC
 * @tc.require: issueI9KQ6R
 */
HWTEST_F(RSNodeTest, SetInstanceId, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetInstanceId(1);
}
} // namespace OHOS::Rosen
