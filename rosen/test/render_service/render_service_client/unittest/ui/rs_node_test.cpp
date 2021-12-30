/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "ui/rs_node.h"
#include "ui/rs_ui_director.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
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
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, LifeCycle001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSNode and add child
     */
    RSNode::SharedPtr rootNode = RSNode::Create();
    ASSERT_TRUE(rootNode != nullptr);

    RSNode::SharedPtr child1 = RSNode::Create();
    RSNode::SharedPtr child2 = RSNode::Create();
    RSNode::SharedPtr child3 = RSNode::Create();
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
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, LifeCycle002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSNode and add child
     */
    RSNode::SharedPtr rootNode = RSNode::Create();
    ASSERT_TRUE(rootNode != nullptr);

    RSNode::SharedPtr child1 = RSNode::Create();
    RSNode::SharedPtr child2 = RSNode::Create();
    RSNode::SharedPtr child3 = RSNode::Create();
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
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, LifeCycle003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSNode and add child
     */
    RSNode::SharedPtr rootNode = RSNode::Create();
    ASSERT_TRUE(rootNode != nullptr);

    RSNode::SharedPtr child1 = RSNode::Create();
    RSNode::SharedPtr child2 = RSNode::Create();
    RSNode::SharedPtr child3 = RSNode::Create();
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
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, LifeCycle004, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSNode and add child
     */
    RSNode::SharedPtr rootNode = RSNode::Create();
    ASSERT_TRUE(rootNode != nullptr);

    RSNode::SharedPtr child1 = RSNode::Create();
    RSNode::SharedPtr child2 = RSNode::Create();
    RSNode::SharedPtr child3 = RSNode::Create();
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
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, LifeCycle005, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSNode and add child
     */
    RSNode::SharedPtr rootNode = RSNode::Create();
    ASSERT_TRUE(rootNode != nullptr);

    RSNode::SharedPtr child1 = RSNode::Create();
    RSNode::SharedPtr child2 = RSNode::Create();
    RSNode::SharedPtr child3 = RSNode::Create();
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
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, Recording001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSNode and RSUIDirector
     */
    RSNode::SharedPtr rsNode = RSNode::Create();

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
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetPaintOrder001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSNode and RSUIDirector
     */
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetPaintOrder(true);
}

/**
 * @tc.name: SetandGetBounds001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBounds001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBounds(floatData[0], floatData[1], floatData[2], floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBoundsPositionX(), floatData[0]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBoundsPositionY(), floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBoundsWidth(), floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBoundsHeight(), floatData[3]));
}

/**
 * @tc.name: SetandGetBounds002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBounds002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBounds(floatData[3], floatData[1], floatData[2], floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBoundsPositionX(), floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBoundsPositionY(), floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBoundsWidth(), floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBoundsHeight(), floatData[0]));
}

/**
 * @tc.name: SetandGetBounds003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBounds003, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBounds(floatData[3], floatData[2], floatData[1], floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBoundsPositionX(), floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBoundsPositionY(), floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBoundsWidth(), floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBoundsHeight(), floatData[0]));
}

/**
 * @tc.name: SetandGetBoundsSize001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBoundsSize001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBoundsSize(floatData[0], floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBoundsWidth(), floatData[0]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBoundsHeight(), floatData[1]));
}

/**
 * @tc.name: SetandGetBoundsSize002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBoundsSize002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBoundsSize(floatData[3], floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBoundsWidth(), floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBoundsHeight(), floatData[1]));
}

/**
 * @tc.name: SetandGetBoundsSize003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBoundsSize003, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBoundsSize(floatData[3], floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBoundsWidth(), floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBoundsHeight(), floatData[2]));
}

/**
 * @tc.name: SetandGetBoundsPosition001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBoundsPosition001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    Vector2f quaternion(floatData[2], floatData[3]);
    rsNode->SetBoundsPosition(quaternion);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBoundsPositionX(), floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBoundsPositionY(), floatData[3]));
}

/**
 * @tc.name: SetandGetBoundsPosition002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBoundsPosition002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    Vector2f quaternion(floatData[2], floatData[0]);
    rsNode->SetBoundsPosition(quaternion);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBoundsPositionX(), floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBoundsPositionY(), floatData[0]));
}

/**
 * @tc.name: SetandGetBoundsPosition003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBoundsPosition003, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    Vector2f quaternion(floatData[1], floatData[3]);
    rsNode->SetBoundsPosition(quaternion);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBoundsPositionX(), floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBoundsPositionY(), floatData[3]));
}

/**
 * @tc.name: SetandGetBoundsWidth001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBoundsWidth001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBoundsWidth(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBoundsWidth(), floatData[1]));
}

/**
 * @tc.name: SetandGetBoundsWidth002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBoundsWidth002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBoundsWidth(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBoundsWidth(), floatData[2]));
}

/**
 * @tc.name: SetandGetBoundsWidth003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBoundsWidth003, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBoundsWidth(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBoundsWidth(), floatData[3]));
}

/**
 * @tc.name: SetandGetBoundsWidth004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBoundsWidth004, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBoundsWidth(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBoundsWidth(), floatData[4]));
}

/**
 * @tc.name: SetandGetBoundsWidth005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBoundsWidth005, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBoundsWidth(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBoundsWidth(), floatData[0]));
}

/**
 * @tc.name: SetandGetBoundsHeight001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBoundsHeight001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBoundsHeight(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBoundsHeight(), floatData[1]));
}

/**
 * @tc.name: SetandGetBoundsHeight002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBoundsHeight002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBoundsHeight(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBoundsHeight(), floatData[2]));
}

/**
 * @tc.name: SetandGetBoundsHeight003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBoundsHeight003, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBoundsHeight(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBoundsHeight(), floatData[3]));
}

/**
 * @tc.name: SetandGetBoundsHeight004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBoundsHeight004, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBoundsHeight(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBoundsHeight(), floatData[4]));
}

/**
 * @tc.name: SetandGetBoundsHeight005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBoundsHeight005, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBoundsHeight(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBoundsHeight(), floatData[0]));
}

/**
 * @tc.name: SetandGetBoundsPositionX001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBoundsPositionX001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBoundsPositionX(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBoundsPositionX(), floatData[1]));
}

/**
 * @tc.name: SetandGetBoundsPositionX002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBoundsPositionX002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBoundsPositionX(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBoundsPositionX(), floatData[2]));
}

/**
 * @tc.name: SetandGetBoundsPositionX003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBoundsPositionX003, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBoundsPositionX(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBoundsPositionX(), floatData[3]));
}

/**
 * @tc.name: SetandGetBoundsPositionX004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBoundsPositionX004, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBoundsPositionX(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBoundsPositionX(), floatData[4]));
}

/**
 * @tc.name: SetandGetBoundsPositionX005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBoundsPositionX005, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBoundsPositionX(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBoundsPositionX(), floatData[0]));
}

/**
 * @tc.name: SetandGetBoundsPositionY001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBoundsPositionY001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBoundsPositionY(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBoundsPositionY(), floatData[1]));
}

/**
 * @tc.name: SetandGetBoundsPositionY002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBoundsPositionY002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBoundsPositionY(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBoundsPositionY(), floatData[2]));
}

/**
 * @tc.name: SetandGetBoundsPositionY003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBoundsPositionY003, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBoundsPositionY(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBoundsPositionY(), floatData[3]));
}

/**
 * @tc.name: SetandGetBoundsPositionY004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBoundsPositionY004, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBoundsPositionY(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBoundsPositionY(), floatData[4]));
}

/**
 * @tc.name: SetandGetBoundsPositionY005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBoundsPositionY005, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBoundsPositionY(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBoundsPositionY(), floatData[0]));
}

/**
 * @tc.name: SetandGetFrame001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetFrame001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetFrame(floatData[0], floatData[1], floatData[2], floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFramePositionX(), floatData[0]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFramePositionY(), floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrameWidth(), floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrameHeight(), floatData[3]));
}

/**
 * @tc.name: SetandGetFrame002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetFrame002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetFrame(floatData[3], floatData[1], floatData[2], floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFramePositionX(), floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFramePositionY(), floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrameWidth(), floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrameHeight(), floatData[0]));
}

/**
 * @tc.name: SetandGetFrame003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetFrame003, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetFrame(floatData[3], floatData[2], floatData[1], floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFramePositionX(), floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFramePositionY(), floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrameWidth(), floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrameHeight(), floatData[0]));
}

/**
 * @tc.name: SetandGetFrameSize001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetFrameSize001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetFrameSize(floatData[0], floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrameWidth(), floatData[0]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrameHeight(), floatData[1]));
}

/**
 * @tc.name: SetandGetFrameSize002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetFrameSize002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetFrameSize(floatData[3], floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrameWidth(), floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrameHeight(), floatData[1]));
}

/**
 * @tc.name: SetandGetFrameSize003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetFrameSize003, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetFrameSize(floatData[3], floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrameWidth(), floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrameHeight(), floatData[2]));
}

/**
 * @tc.name: GetFramePosition001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, GetFramePosition001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    Vector2f quaternion(floatData[2], floatData[3]);
    rsNode->SetFramePosition(quaternion);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFramePositionX(), floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFramePositionY(), floatData[3]));
}

/**
 * @tc.name: GetFramePosition002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, GetFramePosition002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    Vector2f quaternion(floatData[2], floatData[0]);
    rsNode->SetFramePosition(quaternion);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFramePositionX(), floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFramePositionY(), floatData[0]));
}

/**
 * @tc.name: GetFramePosition003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, GetFramePosition003, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    Vector2f quaternion(floatData[1], floatData[3]);
    rsNode->SetFramePosition(quaternion);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFramePositionX(), floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFramePositionY(), floatData[3]));
}

/**
 * @tc.name: SetandGetFrameWidth001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetFrameWidth001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetFrameWidth(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrameWidth(), floatData[1]));
}

/**
 * @tc.name: SetandGetFrameWidth002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetFrameWidth002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetFrameWidth(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrameWidth(), floatData[2]));
}

/**
 * @tc.name: SetandGetFrameWidth003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetFrameWidth003, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetFrameWidth(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrameWidth(), floatData[3]));
}

/**
 * @tc.name: SetandGetFrameWidth004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetFrameWidth004, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetFrameWidth(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrameWidth(), floatData[4]));
}

/**
 * @tc.name: SetandGetFrameWidth005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetFrameWidth005, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetFrameWidth(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrameWidth(), floatData[0]));
}

/**
 * @tc.name: SetandGetFrameHeight001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetFrameHeight001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetFrameHeight(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrameHeight(), floatData[1]));
}

/**
 * @tc.name: SetandGetFrameHeight002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetFrameHeight002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetFrameHeight(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrameHeight(), floatData[2]));
}

/**
 * @tc.name: SetandGetFrameHeight003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetFrameHeight003, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetFrameHeight(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrameHeight(), floatData[3]));
}

/**
 * @tc.name: SetandGetFrameHeight004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetFrameHeight004, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetFrameHeight(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrameHeight(), floatData[4]));
}

/**
 * @tc.name: SetandGetFrameHeight005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetFrameHeight005, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetFrameHeight(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrameHeight(), floatData[0]));
}

/**
 * @tc.name: SetandGetFramePositionX001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetFramePositionX001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetFramePositionX(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFramePositionX(), floatData[1]));
}

/**
 * @tc.name: SetandGetFramePositionX002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetFramePositionX002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetFramePositionX(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFramePositionX(), floatData[2]));
}

/**
 * @tc.name: SetandGetFramePositionX003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetFramePositionX003, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetFramePositionX(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFramePositionX(), floatData[3]));
}

/**
 * @tc.name: SetandGetFramePositionX004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetFramePositionX004, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetFramePositionX(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFramePositionX(), floatData[4]));
}

/**
 * @tc.name: SetandGetFramePositionX005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetFramePositionX005, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetFramePositionX(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFramePositionX(), floatData[0]));
}

/**
 * @tc.name: SetandGetFramePositionY001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetFramePositionY001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetFramePositionY(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFramePositionY(), floatData[1]));
}

/**
 * @tc.name: SetandGetFramePositionY002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetFramePositionY002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetFramePositionY(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFramePositionY(), floatData[2]));
}

/**
 * @tc.name: SetandGetFramePositionY003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetFramePositionY003, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetFramePositionY(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFramePositionY(), floatData[3]));
}

/**
 * @tc.name: SetandGetFramePositionY004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetFramePositionY004, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetFramePositionY(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFramePositionY(), floatData[4]));
}

/**
 * @tc.name: SetandGetFramePositionY005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetFramePositionY005, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetFramePositionY(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFramePositionY(), floatData[0]));
}

/**
 * @tc.name: SetandGetPositionZ001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetPositionZ001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetPositionZ(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPositionZ(), floatData[1]));
}

/**
 * @tc.name: SetandGetPositionZ002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetPositionZ002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetPositionZ(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPositionZ(), floatData[2]));
}

/**
 * @tc.name: SetandGetPositionZ003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetPositionZ003, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetPositionZ(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPositionZ(), floatData[3]));
}

/**
 * @tc.name: SetandGetPositionZ004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetPositionZ004, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetPositionZ(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPositionZ(), floatData[4]));
}

/**
 * @tc.name: SetandGetPositionZ005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetPositionZ005, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetPositionZ(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPositionZ(), floatData[0]));
}

/**
 * @tc.name: SetandGetCornerRadius001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetCornerRadius001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetCornerRadius(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetCornerRadius(), floatData[1]));
}

/**
 * @tc.name: SetandGetCornerRadius002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetCornerRadius002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetCornerRadius(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetCornerRadius(), floatData[2]));
}

/**
 * @tc.name: SetandGetCornerRadius003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetCornerRadius003, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetCornerRadius(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetCornerRadius(), floatData[3]));
}

/**
 * @tc.name: SetandGetCornerRadius004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetCornerRadius004, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetCornerRadius(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetCornerRadius(), floatData[4]));
}

/**
 * @tc.name: SetandGetCornerRadius005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetCornerRadius005, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetCornerRadius(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetCornerRadius(), floatData[0]));
}

/**
 * @tc.name: SetandGetRotationThree001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetRotationThree001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetRotation(floatData[1], floatData[2], floatData[3]);
}

/**
 * @tc.name: SetandGetRotation001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetRotation001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetRotation(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetRotation(), floatData[1]));
}

/**
 * @tc.name: SetandGetRotation002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetRotation002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetRotation(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetRotation(), floatData[2]));
}

/**
 * @tc.name: SetandGetRotation003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetRotation003, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetRotation(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetRotation(), floatData[3]));
}

/**
 * @tc.name: SetandGetRotation004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetRotation004, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetRotation(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetRotation(), floatData[4]));
}

/**
 * @tc.name: SetandGetRotation005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetRotation005, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetRotation(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetRotation(), floatData[0]));
}

/**
 * @tc.name: SetandGetRotationX001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetRotationX001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetRotationX(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetRotationX(), floatData[1]));
}

/**
 * @tc.name: SetandGetRotationX002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetRotationX002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetRotationX(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetRotationX(), floatData[2]));
}

/**
 * @tc.name: SetandGetRotationX003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetRotationX003, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetRotationX(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetRotationX(), floatData[3]));
}

/**
 * @tc.name: SetandGetRotationX004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetRotationX004, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetRotationX(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetRotationX(), floatData[4]));
}

/**
 * @tc.name: SetandGetRotationX005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetRotationX005, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetRotationX(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetRotationX(), floatData[0]));
}

/**
 * @tc.name: SetandGetRotationY001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetRotationY001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetRotationY(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetRotationY(), floatData[1]));
}

/**
 * @tc.name: SetandGetRotationY002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetRotationY002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetRotationY(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetRotationY(), floatData[2]));
}

/**
 * @tc.name: SetandGetRotationY003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetRotationY003, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetRotationY(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetRotationY(), floatData[3]));
}

/**
 * @tc.name: SetandGetRotationY004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetRotationY004, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetRotationY(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetRotationY(), floatData[4]));
}

/**
 * @tc.name: SetandGetRotationY005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetRotationY005, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetRotationY(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetRotationY(), floatData[0]));
}


/**
 * @tc.name: SetandGetScaleX001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetScaleX001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetScaleX(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScaleX(), floatData[1]));
}

/**
 * @tc.name: SetandGetScaleX002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetScaleX002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetScaleX(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScaleX(), floatData[2]));
}

/**
 * @tc.name: SetandGetScaleX003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetScaleX003, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetScaleX(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScaleX(), floatData[3]));
}

/**
 * @tc.name: SetandGetScaleX004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetScaleX004, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetScaleX(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScaleX(), floatData[4]));
}

/**
 * @tc.name: SetandGetScaleX005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetScaleX005, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetScaleX(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScaleX(), floatData[0]));
}

/**
 * @tc.name: SetandGetScale001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetScale001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetScale(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScaleX(), floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScaleY(), floatData[1]));
}

/**
 * @tc.name: SetandGetScale002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetScale002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetScale(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScaleX(), floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScaleY(), floatData[2]));
}

/**
 * @tc.name: SetandGetScale003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetScale003, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetScale(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScaleX(), floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScaleY(), floatData[3]));
}

/**
 * @tc.name: SetandGetScale004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetScale004, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetScale(floatData[3], floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScaleX(), floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScaleY(), floatData[0]));
}

/**
 * @tc.name: SetandGetScale0005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetScale0005, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetScale(floatData[2], floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScaleX(), floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScaleY(), floatData[1]));
}

/**
 * @tc.name: SetandGetScaleY001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetScaleY001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetScaleY(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScaleY(), floatData[1]));
}

/**
 * @tc.name: SetandGetScaleY002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetScaleY002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetScaleY(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScaleY(), floatData[2]));
}

/**
 * @tc.name: SetandGetScaleY003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetScaleY003, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetScaleY(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScaleY(), floatData[3]));
}

/**
 * @tc.name: SetandGetScaleY004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetScaleY004, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetScaleY(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScaleY(), floatData[4]));
}

/**
 * @tc.name: SetandGetScaleY005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetScaleY005, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetScaleY(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScaleY(), floatData[0]));
}

/**
 * @tc.name: SetandGetAlpha001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetAlpha001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetAlpha(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetAlpha(), floatData[1]));
}

/**
 * @tc.name: SetandGetAlpha002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetAlpha002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetAlpha(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetAlpha(), floatData[2]));
}

/**
 * @tc.name: SetandGetAlpha003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetAlpha003, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetAlpha(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetAlpha(), floatData[3]));
}

/**
 * @tc.name: SetandGetAlpha004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetAlpha004, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetAlpha(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetAlpha(), floatData[4]));
}

/**
 * @tc.name: SetandGetAlpha005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetAlpha005, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetAlpha(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetAlpha(), floatData[0]));
}

/**
 * @tc.name: SetandGetBgImageSize001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBgImageSize001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBgImageSize(floatData[0], floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImageWidth(), floatData[0]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImageHeight(), floatData[1]));
}

/**
 * @tc.name: SetandGetBgImageSize002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBgImageSize002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBgImageSize(floatData[3], floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImageWidth(), floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImageHeight(), floatData[1]));
}

/**
 * @tc.name: SetandGetBgImageSize003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBgImageSize003, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBgImageSize(floatData[3], floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImageWidth(), floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImageHeight(), floatData[2]));
}

/**
 * @tc.name: SetandGetBgImageWidth001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBgImageWidth001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBgImageWidth(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImageWidth(), floatData[1]));
}

/**
 * @tc.name: SetandGetBgImageWidth002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBgImageWidth002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBgImageWidth(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImageWidth(), floatData[2]));
}

/**
 * @tc.name: SetandGetBgImageWidth003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBgImageWidth003, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBgImageWidth(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImageWidth(), floatData[3]));
}

/**
 * @tc.name: SetandGetBgImageWidth004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBgImageWidth004, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBgImageWidth(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImageWidth(), floatData[4]));
}

/**
 * @tc.name: SetandGetBgImageWidth005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBgImageWidth005, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBgImageWidth(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImageWidth(), floatData[0]));
}

/**
 * @tc.name: SetandGetBgImageHeight001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBgImageHeight001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBgImageHeight(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImageHeight(), floatData[1]));
}

/**
 * @tc.name: SetandGetBgImageHeight002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBgImageHeight002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBgImageHeight(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImageHeight(), floatData[2]));
}

/**
 * @tc.name: SetandGetBgImageHeight003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBgImageHeight003, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBgImageHeight(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImageHeight(), floatData[3]));
}

/**
 * @tc.name: SetandGetBgImageHeight004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBgImageHeight004, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBgImageHeight(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImageHeight(), floatData[4]));
}

/**
 * @tc.name: SetandGetBgImageHeight005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBgImageHeight005, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBgImageHeight(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImageHeight(), floatData[0]));
}

/**
 * @tc.name: SetandSetBgImagePosition001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandSetBgImagePosition001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBgImagePosition(floatData[2], floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImagePositionX(), floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImagePositionY(), floatData[3]));
}

/**
 * @tc.name: SetandSetBgImagePosition002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandSetBgImagePosition002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBgImagePosition(floatData[2], floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImagePositionX(), floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImagePositionY(), floatData[0]));
}

/**
 * @tc.name: SetandSetBgImagePosition003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandSetBgImagePosition003, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBgImagePosition(floatData[1], floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImagePositionX(), floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImagePositionY(), floatData[3]));
}

/**
 * @tc.name: SetandGetBgImagePositionX001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBgImagePositionX001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBgImagePositionX(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImagePositionX(), floatData[1]));
}

/**
 * @tc.name: SetandGetBgImagePositionX002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBgImagePositionX002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBgImagePositionX(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImagePositionX(), floatData[2]));
}

/**
 * @tc.name: SetandGetBgImagePositionX003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBgImagePositionX003, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBgImagePositionX(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImagePositionX(), floatData[3]));
}

/**
 * @tc.name: SetandGetBgImagePositionX004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBgImagePositionX004, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBgImagePositionX(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImagePositionX(), floatData[4]));
}

/**
 * @tc.name: SetandGetBgImagePositionX005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBgImagePositionX005, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBgImagePositionX(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImagePositionX(), floatData[0]));
}

/**
 * @tc.name: SetandGetBgImagePositionY001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBgImagePositionY001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBgImagePositionY(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImagePositionY(), floatData[1]));
}

/**
 * @tc.name: SetandGetBgImagePositionY002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBgImagePositionY002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBgImagePositionY(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImagePositionY(), floatData[2]));
}

/**
 * @tc.name: SetandGetBgImagePositionY003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBgImagePositionY003, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBgImagePositionY(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImagePositionY(), floatData[3]));
}

/**
 * @tc.name: SetandGetBgImagePositionY004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBgImagePositionY004, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBgImagePositionY(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImagePositionY(), floatData[4]));
}

/**
 * @tc.name: SetandGetBgImagePositionY005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBgImagePositionY005, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBgImagePositionY(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImagePositionY(), floatData[0]));
}

/**
 * @tc.name: SetandGetBorderWidth001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBorderWidth001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBorderWidth(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBorderWidth(), floatData[1]));
}

/**
 * @tc.name: SetandGetBorderWidth002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBorderWidth002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBorderWidth(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBorderWidth(), floatData[2]));
}

/**
 * @tc.name: SetandGetBorderWidth003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBorderWidth003, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBorderWidth(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBorderWidth(), floatData[3]));
}

/**
 * @tc.name: SetandGetBorderWidth004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBorderWidth004, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBorderWidth(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBorderWidth(), floatData[4]));
}

/**
 * @tc.name: SetandGetBorderWidth005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBorderWidth005, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetBorderWidth(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBorderWidth(), floatData[0]));
}

/**
 * @tc.name: SetandGetPivot001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetPivot001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetPivot(floatData[0], floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPivotX(), floatData[0]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPivotY(), floatData[1]));
}

/**
 * @tc.name: SetandGetPivot002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetPivot002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetPivot(floatData[3], floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPivotX(), floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPivotY(), floatData[2]));
}

/**
 * @tc.name: SetandGetPivot003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetPivot003, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetPivot(floatData[1], floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPivotX(), floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPivotY(), floatData[3]));
}

/**
 * @tc.name: SetandGetPivotX001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetPivotX001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetPivotX(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPivotX(), floatData[1]));
}

/**
 * @tc.name: SetandGetPivotX002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetPivotX002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetPivotX(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPivotX(), floatData[2]));
}

/**
 * @tc.name: SetandGetPivotX003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetPivotX003, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetPivotX(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPivotX(), floatData[3]));
}

/**
 * @tc.name: SetandGetPivotX004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetPivotX004, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetPivotX(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPivotX(), floatData[4]));
}

/**
 * @tc.name: SetandGetPivotX005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetPivotX005, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetPivotX(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPivotX(), floatData[0]));
}

/**
 * @tc.name: SetandGetPivotY001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetPivotY001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetPivotY(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPivotY(), floatData[1]));
}

/**
 * @tc.name: SetandGetPivotY002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetPivotY002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetPivotY(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPivotY(), floatData[2]));
}

/**
 * @tc.name: SetandGetPivotY003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetPivotY003, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetPivotY(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPivotY(), floatData[3]));
}

/**
 * @tc.name: SetandGetPivotY004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetPivotY004, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetPivotY(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPivotY(), floatData[4]));
}

/**
 * @tc.name: SetandGetPivotY005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetPivotY005, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetPivotY(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPivotY(), floatData[0]));
}

/**
 * @tc.name: SetandGetShadowOffset001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetShadowOffset001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetShadowOffset(floatData[2], floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowOffsetX(), floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowOffsetY(), floatData[3]));
}

/**
 * @tc.name: SetandGetShadowOffset002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetShadowOffset002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetShadowOffset(floatData[2], floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowOffsetX(), floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowOffsetY(), floatData[0]));
}

/**
 * @tc.name: SetandGetShadowOffset003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetShadowOffset003, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetShadowOffset(floatData[1], floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowOffsetX(), floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowOffsetY(), floatData[3]));
}
/**
 * @tc.name: SetandGetShadowOffsetX001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetShadowOffsetX001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetShadowOffsetX(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowOffsetX(), floatData[1]));
}

/**
 * @tc.name: SetandGetShadowOffsetX002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetShadowOffsetX002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetShadowOffsetX(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowOffsetX(), floatData[2]));
}

/**
 * @tc.name: SetandGetShadowOffsetX003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetShadowOffsetX003, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetShadowOffsetX(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowOffsetX(), floatData[3]));
}

/**
 * @tc.name: SetandGetShadowOffsetX004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetShadowOffsetX004, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetShadowOffsetX(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowOffsetX(), floatData[4]));
}

/**
 * @tc.name: SetandGetShadowOffsetX005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetShadowOffsetX005, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetShadowOffsetX(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowOffsetX(), floatData[0]));
}

/**
 * @tc.name: SetandGetShadowOffsetY001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetShadowOffsetY001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetShadowOffsetY(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowOffsetY(), floatData[1]));
}

/**
 * @tc.name: SetandGetShadowOffsetY002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetShadowOffsetY002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetShadowOffsetY(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowOffsetY(), floatData[2]));
}

/**
 * @tc.name: SetandGetShadowOffsetY003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetShadowOffsetY003, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetShadowOffsetY(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowOffsetY(), floatData[3]));
}

/**
 * @tc.name: SetandGetShadowOffsetY004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetShadowOffsetY004, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetShadowOffsetY(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowOffsetY(), floatData[4]));
}

/**
 * @tc.name: SetandGetShadowOffsetY005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetShadowOffsetY005, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetShadowOffsetY(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowOffsetY(), floatData[0]));
}

/**
 * @tc.name: SetandGetShadowAlpha001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetShadowAlpha001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetShadowAlpha(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowAlpha(),
        std::clamp(floatData[1], 0.0f, 1.0f), 0.02f));
}

/**
 * @tc.name: SetandGetShadowAlpha002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetShadowAlpha002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetShadowAlpha(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowAlpha(),
        std::clamp(floatData[2], 0.0f, 1.0f), 0.02f));
}

/**
 * @tc.name: SetandGetShadowAlpha003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetShadowAlpha003, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetShadowAlpha(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowAlpha(),
        std::clamp(floatData[3], 0.0f, 1.0f), 0.02f));
}

/**
 * @tc.name: SetandGetShadowAlpha004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetShadowAlpha004, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetShadowAlpha(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowAlpha(),
        std::clamp(floatData[4], 0.0f, 1.0f), 0.02f));
}

/**
 * @tc.name: SetandGetShadowAlpha005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetShadowAlpha005, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetShadowAlpha(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowAlpha(),
        std::clamp(floatData[0], 0.0f, 1.0f), 0.02f));
}

/**
 * @tc.name: SetandGetShadowElevation001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetShadowElevation001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetShadowElevation(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowElevation(), floatData[1]));
}

/**
 * @tc.name: SetandGetShadowElevation002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetShadowElevation002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetShadowElevation(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowElevation(), floatData[2]));
}

/**
 * @tc.name: SetandGetShadowElevation003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetShadowElevation003, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetShadowElevation(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowElevation(), floatData[3]));
}

/**
 * @tc.name: SetandGetShadowElevation004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetShadowElevation004, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetShadowElevation(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowElevation(), floatData[4]));
}

/**
 * @tc.name: SetandGetShadowElevation005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetShadowElevation005, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetShadowElevation(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowElevation(), floatData[0]));
}

/**
 * @tc.name: SetandGetShadowRadius001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetShadowRadius001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetShadowRadius(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowRadius(), floatData[1]));
}

/**
 * @tc.name: SetandGetShadowRadius002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetShadowRadius002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetShadowRadius(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowRadius(), floatData[2]));
}

/**
 * @tc.name: SetandGetShadowRadius003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetShadowRadius003, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetShadowRadius(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowRadius(), floatData[3]));
}

/**
 * @tc.name: SetandGetShadowRadius004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetShadowRadius004, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetShadowRadius(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowRadius(), floatData[4]));
}

/**
 * @tc.name: SetandGetShadowRadius005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetShadowRadius005, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetShadowRadius(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowRadius(), floatData[0]));
}

/**
 * @tc.name: SetandGetShadowColor001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetShadowColor001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    constexpr uint32_t colorValue = 0x034123;
    rsNode->SetShadowColor(colorValue);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetShadowColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetShadowColor002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetShadowColor002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    constexpr uint32_t colorValue = 0x45ba87;
    rsNode->SetShadowColor(colorValue);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetShadowColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetShadowColor003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetShadowColor003, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    constexpr uint32_t colorValue = 0x32aadd;
    rsNode->SetShadowColor(colorValue);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetShadowColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetShadowColor004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetShadowColor004, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::max();
    rsNode->SetShadowColor(colorValue);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetShadowColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetShadowColor005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetShadowColor005, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::min();
    rsNode->SetShadowColor(colorValue);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetShadowColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetTranslateThree001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetTranslateThree001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetTranslate(floatData[1], floatData[2], floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslateX(), floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslateY(), floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslateZ(), floatData[3]));
}

/**
 * @tc.name: SetandGetTranslateThree002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetTranslateThree002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetTranslate(floatData[0], floatData[0], floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslateX(), floatData[0]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslateY(), floatData[0]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslateZ(), floatData[2]));
}

/**
 * @tc.name: SetandGetTranslateThree003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetTranslateThree003, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetTranslate(floatData[1], floatData[3], floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslateX(), floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslateY(), floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslateZ(), floatData[1]));
}

/**
 * @tc.name: SetandGetTranslateX001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetTranslateX001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetTranslateX(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslateX(), floatData[1]));
}

/**
 * @tc.name: SetandGetTranslateX002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetTranslateX002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetTranslateX(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslateX(), floatData[2]));
}

/**
 * @tc.name: SetandGetTranslateX003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetTranslateX003, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetTranslateX(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslateX(), floatData[3]));
}

/**
 * @tc.name: SetandGetTranslateX004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetTranslateX004, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetTranslateX(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslateX(), floatData[4]));
}

/**
 * @tc.name: SetandGetTranslateX005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetTranslateX005, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetTranslateX(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslateX(), floatData[0]));
}

/**
 * @tc.name: SetandGetTranslateY001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetTranslateY001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetTranslateY(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslateY(), floatData[1]));
}

/**
 * @tc.name: SetandGetTranslateY002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetTranslateY002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetTranslateY(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslateY(), floatData[2]));
}

/**
 * @tc.name: SetandGetTranslateY003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetTranslateY003, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetTranslateY(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslateY(), floatData[3]));
}

/**
 * @tc.name: SetandGetTranslateY004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetTranslateY004, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetTranslateY(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslateY(), floatData[4]));
}

/**
 * @tc.name: SetandGetTranslateY005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetTranslateY005, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetTranslateY(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslateY(), floatData[0]));
}

/**
 * @tc.name: SetandGetTranslateZ001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetTranslateZ001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetTranslateZ(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslateZ(), floatData[1]));
}

/**
 * @tc.name: SetandGetTranslateZ002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetTranslateZ002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetTranslateZ(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslateZ(), floatData[2]));
}

/**
 * @tc.name: SetandGetTranslateZ003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetTranslateZ003, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetTranslateZ(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslateZ(), floatData[3]));
}

/**
 * @tc.name: SetandGetTranslateZ004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetTranslateZ004, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetTranslateZ(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslateZ(), floatData[4]));
}

/**
 * @tc.name: SetandGetTranslateZ005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetTranslateZ005, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetTranslateZ(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslateZ(), floatData[0]));
}

/**
 * @tc.name: SetandGetClipToBounds001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetClipToBounds001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetClipToBounds(true);
    EXPECT_EQ(rsNode->GetStagingProperties().GetClipToBounds(), true);
}

/**
 * @tc.name: SetandGetClipToBounds002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetClipToBounds002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetClipToBounds(false);
    EXPECT_EQ(rsNode->GetStagingProperties().GetClipToBounds(), false);
}

/**
 * @tc.name: SetandGetClipToFrame001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetClipToFrame001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetClipToFrame(true);
    EXPECT_EQ(rsNode->GetStagingProperties().GetClipToFrame(), true);
}

/**
 * @tc.name: SetandGetClipToFrame002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetClipToFrame002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetClipToFrame(false);
    EXPECT_EQ(rsNode->GetStagingProperties().GetClipToFrame(), false);
}

/**
 * @tc.name: SetandGetVisible001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetVisible001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetVisible(true);
    EXPECT_EQ(rsNode->GetStagingProperties().GetVisible(), true);
}

/**
 * @tc.name: SetandGetVisible002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetVisible002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->SetVisible(false);
    EXPECT_EQ(rsNode->GetStagingProperties().GetVisible(), false);
}

/**
 * @tc.name: SetandGetBorderStyle001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBorderStyle001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    BorderStyle borderStyle = BorderStyle::SOLID;
    rsNode->SetBorderStyle(static_cast<uint32_t>(borderStyle));
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBorderStyle() == borderStyle);
}

/**
 * @tc.name: SetandGetBorderStyle002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBorderStyle002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    BorderStyle borderStyle = BorderStyle::DASHED;
    rsNode->SetBorderStyle(static_cast<uint32_t>(borderStyle));
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBorderStyle() == borderStyle);
}

/**
 * @tc.name: SetandGetBorderStyle003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBorderStyle003, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    BorderStyle borderStyle = BorderStyle::DOTTED;
    rsNode->SetBorderStyle(static_cast<uint32_t>(borderStyle));
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBorderStyle() == borderStyle);
}

/**
 * @tc.name: SetandGetBorderStyle004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBorderStyle004, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    BorderStyle borderStyle = BorderStyle::NONE;
    rsNode->SetBorderStyle(static_cast<uint32_t>(borderStyle));
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBorderStyle() == borderStyle);
}

/**
 * @tc.name: SetandGetFrameGravity001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetFrameGravity001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    Gravity gravity = Gravity::CENTER;
    rsNode->SetFrameGravity(gravity);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetFrameGravity002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    Gravity gravity = Gravity::TOP;
    rsNode->SetFrameGravity(gravity);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetFrameGravity003, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    Gravity gravity = Gravity::BOTTOM;
    rsNode->SetFrameGravity(gravity);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetFrameGravity004, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    Gravity gravity = Gravity::LEFT;
    rsNode->SetFrameGravity(gravity);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetFrameGravity005, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    Gravity gravity = Gravity::RIGHT;
    rsNode->SetFrameGravity(gravity);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity006
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetFrameGravity006, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    Gravity gravity = Gravity::TOP_LEFT;
    rsNode->SetFrameGravity(gravity);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity007
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetFrameGravity007, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    Gravity gravity = Gravity::TOP_RIGHT;
    rsNode->SetFrameGravity(gravity);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity008
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetFrameGravity008, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    Gravity gravity = Gravity::BOTTOM_LEFT;
    rsNode->SetFrameGravity(gravity);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity009
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetFrameGravity009, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    Gravity gravity = Gravity::BOTTOM_RIGHT;
    rsNode->SetFrameGravity(gravity);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity010
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetFrameGravity010, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    Gravity gravity = Gravity::RESIZE;
    rsNode->SetFrameGravity(gravity);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity011
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetFrameGravity011, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    Gravity gravity = Gravity::RESIZE_ASPECT;
    rsNode->SetFrameGravity(gravity);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity012
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetFrameGravity012, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    Gravity gravity = Gravity::RESIZE_ASPECT_FILL;
    rsNode->SetFrameGravity(gravity);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: NotifyTransition001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, NotifyTransition001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->NotifyTransition({RSTransitionEffect(RSTransitionEffectType::FADE_IN)}, rsNode->GetId());
}

/**
 * @tc.name: NotifyTransition002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, NotifyTransition002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->NotifyTransition({RSTransitionEffect(RSTransitionEffectType::FADE_IN)}, rsNode->GetId());
}

/**
 * @tc.name: SetandGetForegroundColor001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetForegroundColor001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    constexpr uint32_t colorValue = 0x034123;
    rsNode->SetForegroundColor(colorValue);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetForegroundColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetForegroundColor002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetForegroundColor002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::max();
    rsNode->SetForegroundColor(colorValue);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetForegroundColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetForegroundColor003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetForegroundColor003, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::min();
    rsNode->SetForegroundColor(colorValue);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetForegroundColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetBackgroundColor001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBackgroundColor001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    constexpr uint32_t colorValue = 0x034123;
    rsNode->SetBackgroundColor(colorValue);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBackgroundColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetBackgroundColor002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBackgroundColor002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::max();
    rsNode->SetBackgroundColor(colorValue);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBackgroundColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetBackgroundColor003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBackgroundColor003, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::min();
    rsNode->SetBackgroundColor(colorValue);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBackgroundColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetBorderColor001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBorderColor001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    constexpr uint32_t colorValue = 0x034123;
    rsNode->SetBorderColor(colorValue);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBorderColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetBorderColor002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBorderColor002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::max();
    rsNode->SetBorderColor(colorValue);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBorderColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetBorderColor003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetBorderColor003, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::min();
    rsNode->SetBorderColor(colorValue);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBorderColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetRotationVector001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetRotationVector001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    Vector4f quaternion(std::numeric_limits<int>::min(), 2.f, 3.f, 4.f);
    rsNode->SetRotation(quaternion);
}

/**
 * @tc.name: SetandGetTranslateVector001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetTranslateVector001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    Vector2f quaternion(std::numeric_limits<int>::max(), 2.f);
    rsNode->SetTranslate(quaternion);
}

/**
 * @tc.name: SetandGetSublayerTransform001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetSublayerTransform001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    Matrix3f sublayerTransform = Matrix3f::IDENTITY;
    rsNode->SetSublayerTransform(sublayerTransform);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetSublayerTransform() == sublayerTransform);
}

/**
 * @tc.name: SetandGetSublayerTransform002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetSublayerTransform002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    Matrix3f sublayerTransform = Matrix3f::ZERO;
    rsNode->SetSublayerTransform(sublayerTransform);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetSublayerTransform() == sublayerTransform);
}

/**
 * @tc.name: CreateBlurFilter001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, CreateBlurFilter001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    std::shared_ptr<RSFilter> backgroundFilter = RSFilter::CreateBlurFilter(floatData[0], floatData[1]);
    rsNode->SetBackgroundFilter(backgroundFilter);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBackgroundFilter() == backgroundFilter);
}

/**
 * @tc.name: CreateBlurFilter002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, CreateBlurFilter002, TestSize.Level2)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    std::shared_ptr<RSFilter> backgroundFilter = RSFilter::CreateBlurFilter(floatData[1], floatData[2]);
    rsNode->SetBackgroundFilter(backgroundFilter);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBackgroundFilter() == backgroundFilter);
}

/**
 * @tc.name: CreateBlurFilter003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, CreateBlurFilter003, TestSize.Level3)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    std::shared_ptr<RSFilter> backgroundFilter = RSFilter::CreateBlurFilter(floatData[2], floatData[3]);
    rsNode->SetBackgroundFilter(backgroundFilter);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBackgroundFilter() == backgroundFilter);
}

/**
 * @tc.name: CreateBlurFilter004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, CreateBlurFilter004, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    std::shared_ptr<RSFilter> backgroundFilter = RSFilter::CreateBlurFilter(floatData[3], floatData[4]);
    rsNode->SetBackgroundFilter(backgroundFilter);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBackgroundFilter() == backgroundFilter);
}

/**
 * @tc.name: CreateBlurFilter005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, CreateBlurFilter005, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    std::shared_ptr<RSFilter> backgroundFilter = RSFilter::CreateBlurFilter(floatData[4], floatData[0]);
    rsNode->SetBackgroundFilter(backgroundFilter);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBackgroundFilter() == backgroundFilter);
}

/**
 * @tc.name: CreateNormalFilter001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, CreateNormalFilter001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(floatData[0], floatData[1]);
    rsNode->SetFilter(filter);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetFilter() == filter);
}

/**
 * @tc.name: CreateNormalFilter002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, CreateNormalFilter002, TestSize.Level2)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(floatData[1], floatData[2]);
    rsNode->SetFilter(filter);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetFilter() == filter);
}

/**
 * @tc.name: CreateNormalFilter003
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, CreateNormalFilter003, TestSize.Level3)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(floatData[2], floatData[3]);
    rsNode->SetFilter(filter);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetFilter() == filter);
}

/**
 * @tc.name: CreateNormalFilter004
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, CreateNormalFilter004, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(floatData[3], floatData[4]);
    rsNode->SetFilter(filter);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetFilter() == filter);
}

/**
 * @tc.name: CreateNormalFilter005
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, CreateNormalFilter005, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(floatData[4], floatData[0]);
    rsNode->SetFilter(filter);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetFilter() == filter);
}

/**
 * @tc.name: SetandGetClipBounds001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetandGetClipBounds001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    std::shared_ptr<RSPath> clipPath = RSPath::CreateRSPath();
    rsNode->SetClipBounds(clipPath);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetClipBounds() == clipPath);
}

/**
 * @tc.name: GetId001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, GetId001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->GetId();
}

/**
 * @tc.name: GetChildren001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, GetChildren001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    auto c = rsNode->GetChildren();
}

/**
 * @tc.name: GetChildren002
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, GetChildren002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    const auto c = rsNode->GetChildren();
}

/**
 * @tc.name: GetStagingProperties001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, GetStagingProperties001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->GetStagingProperties();
}

/**
 * @tc.name: GetMotionPathOption001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, GetMotionPathOption002, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->GetMotionPathOption();
}

/**
 * @tc.name: GetShowingProperties001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, GetShowingProperties001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    rsNode->GetShowingProperties();
}

/**
 * @tc.name: SetBgImage001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetBgImage001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    auto image = std::make_shared<RSImage>();
    rsNode->SetBgImage(image);
}

/**
 * @tc.name: SetBackgroundShader001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetBackgroundShader001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    auto shader = RSShader::CreateRSShader();
    rsNode->SetBackgroundShader(shader);
}

/**
 * @tc.name: SetCompositingFilter001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetCompositingFilter001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    auto compositingFilter = RSFilter::CreateBlurFilter(0.0f, 0.0f);
    rsNode->SetCompositingFilter(compositingFilter);
}

/**
 * @tc.name: SetShadowPath001
 * @tc.desc:
 * @tc.type:FUNC
 * @tc.require:AR000GGR40
 * @tc.author:
 */
HWTEST_F(RSNodeTest, SetShadowPath001, TestSize.Level1)
{
    RSNode::SharedPtr rsNode = RSNode::Create();
    auto shadowpath = RSPath::CreateRSPath();
    rsNode->SetShadowPath(shadowpath);
}
} // namespace OHOS::Rosen
