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

#include <memory>
#include "gtest/gtest.h"
#include "animation/rs_animation.h"
#include "animation/rs_transition.h"
#include "modifier/rs_property_modifier.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_surface_node.h"
#include "ui_effect/effect/include/brightness_blender.h"
#include "animation/rs_animation_callback.h"
#include "animation/rs_implicit_animator_map.h"
#include "animation/rs_implicit_animator.h"
#include "animation/rs_implicit_animation_param.h"
#include "modifier/rs_modifier.h"
#include "common/rs_vector4.h"

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

class RSCanvasNodeTest002 : public testing::Test {
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

void RSCanvasNodeTest002::SetUpTestCase() {}
void RSCanvasNodeTest002::TearDownTestCase() {}
void RSCanvasNodeTest002::SetUp() {}
void RSCanvasNodeTest002::TearDown() {}

/**
 * @tc.name: LifeCycle001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasNodeTest002, LifeCycle001, TestSize.Level1)
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
 * @tc.name: LifeCycle002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasNodeTest002, LifeCycle002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, LifeCycle003, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, LifeCycle004, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, LifeCycle005, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, Recording001, TestSize.Level1)
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
 * @tc.name: SetandGetBounds001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasNodeTest002, SetandGetBounds001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetBounds002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetBounds003, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetBoundsWidth001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetBoundsWidth002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetBoundsWidth003, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetIlluminatedBorderWidthTest, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetBoundsWidth004, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetBoundsWidth005, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetBoundsHeight001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetBoundsHeight002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetBoundsHeight003, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetBoundsHeight004, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetBoundsHeight005, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetFrame001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetFrame002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetFrame003, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetFramePositionX001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetFramePositionX002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetFramePositionX003, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetFramePositionX004, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetFramePositionX005, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetFramePositionY001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetFramePositionY002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetFramePositionY003, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetFramePositionY004, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetFramePositionY005, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetPositionZ001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetPositionZ002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetPositionZ003, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetPositionZ004, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetPositionZ005, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetCornerRadius001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetCornerRadius002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetCornerRadius003, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetCornerRadius004, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetCornerRadius005, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetOutlineRadius001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetOutlineRadius002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetOutlineRadius003, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetOutlineRadius004, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetOutlineRadius005, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetOutlineRadius(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetOutlineRadius().x_, floatData[0]));
}

/**
 * @tc.name: SetandGetRotation001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasNodeTest002, SetandGetRotation001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetRotation002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetRotation003, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetRotation004, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetRotation005, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetRotationX001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetRotationX002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetRotationX003, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetRotationX004, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetRotationX005, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetRotationY001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetRotationY002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetRotationY003, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetRotationY004, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetRotationY005, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetScaleX001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetScaleX002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetScaleX003, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetScaleX004, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetScaleX005, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetScale001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetScale002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetScale003, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetScale004, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetScale(floatData[3], floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScale().x_, floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScale().y_, floatData[0]));
}

/**
 * @tc.name: SetandGetSkew001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasNodeTest002, SetandGetSkew001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetSkew002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetSkew003, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetSkew004, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetSkew0005, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetSkewX001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetSkewX002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetSkewX003, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetSkewX004, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetSkewX005, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetSkewY001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetSkewY002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetSkewY003, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetSkewY004, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetSkewY005, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetPersp001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetPersp002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetPersp003, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetPersp004, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetPersp0005, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetPerspX001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetPerspX002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetPerspX003, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetPerspX004, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetPerspX005, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetPerspY001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetPerspY002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetPerspY003, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetPerspY004, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetPerspY005, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetAlphaOffscreen001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetAlphaOffscreen002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetAlpha001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetAlpha002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetAlpha003, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetAlpha004, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetAlpha005, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetBgImageSize001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetBgImageSize002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetBgImageSize003, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetBgImageWidth001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetBgImageWidth002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetBgImageWidth003, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetBgImageWidth004, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetBgImageWidth005, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetBgImageHeight001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetBgImageHeight002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetBgImageHeight003, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetBgImageHeight004, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetBgImageHeight005, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandSetBgImagePosition001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandSetBgImagePosition002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandSetBgImagePosition003, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetBgImagePositionX001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetBgImagePositionX002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetBgImagePositionX003, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetBgImagePositionX004, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetBgImagePositionX005, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetBgImagePositionY001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetBgImagePositionY002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetBgImagePositionY003, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetBgImagePositionY004, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetBgImagePositionY005, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetBorderWidth001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetBorderWidth002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetBorderWidth003, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetBorderWidth004, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetBorderWidth005, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBorderWidth(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBorderWidth().x_, floatData[0]));
}

/**
 * @tc.name: SetandGetBorderDashParams001
 * @tc.desc: Check for zero values
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasNodeTest002, SetandGetBorderDashParams001, TestSize.Level1)
{
    SetBorderDashParamsAndTest(FLOAT_DATA_ZERO);
}

/**
 * @tc.name: SetandGetBorderDashParams002
 * @tc.desc: Check for positive values
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasNodeTest002, SetandGetBorderDashParams002, TestSize.Level1)
{
    SetBorderDashParamsAndTest(FLOAT_DATA_POSITIVE);
}

/**
 * @tc.name: SetandGetBorderDashParams003
 * @tc.desc: Check for negative values
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasNodeTest002, SetandGetBorderDashParams003, TestSize.Level1)
{
    SetBorderDashParamsAndTest(FLOAT_DATA_NEGATIVE);
}

/**
 * @tc.name: SetandGetBorderDashParams004
 * @tc.desc: Check for max values
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasNodeTest002, SetandGetBorderDashParams004, TestSize.Level1)
{
    SetBorderDashParamsAndTest(FLOAT_DATA_MAX);
}

/**
 * @tc.name: SetandGetBorderDashParams005
 * @tc.desc: Check for min values
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasNodeTest002, SetandGetBorderDashParams005, TestSize.Level1)
{
    SetBorderDashParamsAndTest(FLOAT_DATA_MIN);
}

/**
 * @tc.name: SetandGetOutlineWidth001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasNodeTest002, SetandGetOutlineWidth001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetOutlineWidth002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetOutlineWidth003, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetOutlineWidth004, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetOutlineWidth(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetOutlineWidth().x_, floatData[4]));
}

/**
 * @tc.name: SetandGetOutlineDashParams001
 * @tc.desc: Check for zero values
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasNodeTest002, SetandGetOutlineDashParams001, TestSize.Level1)
{
    SetOutlineDashParamsAndTest(FLOAT_DATA_ZERO);
}

/**
 * @tc.name: SetandGetOutlineDashParams002
 * @tc.desc: Check for positive values
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasNodeTest002, SetandGetOutlineDashParams002, TestSize.Level1)
{
    SetOutlineDashParamsAndTest(FLOAT_DATA_POSITIVE);
}

/**
 * @tc.name: SetandGetOutlineDashParams003
 * @tc.desc: Check for negative values
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasNodeTest002, SetandGetOutlineDashParams003, TestSize.Level1)
{
    SetOutlineDashParamsAndTest(FLOAT_DATA_NEGATIVE);
}

/**
 * @tc.name: SetandGetOutlineDashParams004
 * @tc.desc: Check for max values
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasNodeTest002, SetandGetOutlineDashParams004, TestSize.Level1)
{
    SetOutlineDashParamsAndTest(FLOAT_DATA_MAX);
}

/**
 * @tc.name: SetandGetOutlineDashParams005
 * @tc.desc: Check for min values
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasNodeTest002, SetandGetOutlineDashParams005, TestSize.Level1)
{
    SetOutlineDashParamsAndTest(FLOAT_DATA_MIN);
}

/**
 * @tc.name: SetandGetPivot001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasNodeTest002, SetandGetPivot001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetPivot002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetPivot003, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetPivotX001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetPivotX002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetPivotX003, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetPivotX004, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetPivotX005, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetPivotY001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetPivotY002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetPivotY003, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetPivotY004, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetPivotY005, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetShadowOffset001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetShadowOffset002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetShadowOffset003, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetShadowOffsetX001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetShadowOffsetX002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetShadowOffsetX003, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetShadowOffsetX004, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetShadowOffsetX005, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetShadowOffsetY001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetShadowOffsetY002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetShadowOffsetY003, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetShadowOffsetY004, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetShadowOffsetY005, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetShadowAlpha001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetShadowAlpha002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetShadowAlpha003, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetShadowAlpha004, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetShadowAlpha005, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetShadowElevation001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetShadowElevation002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetShadowElevation003, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetShadowElevation004, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetShadowElevation005, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetShadowRadius001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetShadowRadius002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetShadowRadius003, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetShadowRadius004, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetShadowRadius005, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetShadowColor001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetShadowColor002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetShadowColor003, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetShadowColor004, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetShadowColor005, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetShadowColorStrategy001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    int shadowColorStrategy = SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_NONE;
    rsNode->SetShadowColorStrategy(shadowColorStrategy);
    ASSERT_EQ(rsNode->GetStagingProperties().GetShadowColorStrategy(), shadowColorStrategy);
}

/**
 * @tc.name: SetandGetShadowColorStrategy002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasNodeTest002, SetandGetShadowColorStrategy002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetShadowColorStrategy003, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetTranslateThree001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetTranslateThree002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetTranslateThree003, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetTranslateX001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetTranslateX002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetTranslateX003, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetTranslateX004, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetTranslateX005, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetTranslateY001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetTranslateY002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetTranslateY003, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetTranslateY004, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetTranslateY005, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetTranslateZ001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetTranslateZ002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetTranslateZ003, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetTranslateZ004, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetTranslateZ005, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetClipToBounds001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetClipToBounds002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetClipToFrame001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest002, SetandGetClipToFrame002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetClipToFrame(false);
    EXPECT_EQ(rsNode->GetStagingProperties().GetClipToFrame(), false);
}

/**
 * @tc.name: SetBoundsWidth
 * @tc.desc: test results of SetBoundsWidth
 * @tc.type: FUNC
 * @tc.require: issueI9RLG7
 */
HWTEST_F(RSCanvasNodeTest, SetBoundsWidth, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBoundsWidth(1.f);
    EXPECT_TRUE(!rsNode->propertyModifiers_.empty());

    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSPropertyBase>();
    RSModifierType modifierType = RSModifierType::BOUNDS;
    std::shared_ptr<RSModifier> modifier = std::make_shared<RSBackgroundShaderModifier>(property);
    modifier->property_ = nullptr;
    rsNode->propertyModifiers_[modifierType] = modifier;
    rsNode->SetBoundsWidth(1.f);
    EXPECT_TRUE(!rsNode->propertyModifiers_.empty());
}

/**
 * @tc.name: SetBoundsHeight
 * @tc.desc: test results of SetBoundsHeight
 * @tc.type: FUNC
 * @tc.require: issueI9RLG7
 */
HWTEST_F(RSCanvasNodeTest, SetBoundsHeight, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBoundsHeight(1.f);
    EXPECT_TRUE(!rsNode->propertyModifiers_.empty());

    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSPropertyBase>();
    RSModifierType modifierType = RSModifierType::BOUNDS;
    std::shared_ptr<RSModifier> modifier = std::make_shared<RSBackgroundShaderModifier>(property);
    modifier->property_ = nullptr;
    rsNode->propertyModifiers_[modifierType] = modifier;
    rsNode->SetBoundsHeight(1.f);
    EXPECT_TRUE(!rsNode->propertyModifiers_.empty());
}

/**
 * @tc.name: SetFramePositionX
 * @tc.desc: test results of SetFramePositionX
 * @tc.type: FUNC
 * @tc.require: issueI9RLG7
 */
HWTEST_F(RSCanvasNodeTest, SetFramePositionX, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetFramePositionX(1.f);
    EXPECT_TRUE(!rsNode->propertyModifiers_.empty());

    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSPropertyBase>();
    RSModifierType modifierType = RSModifierType::FRAME;
    std::shared_ptr<RSModifier> modifier = std::make_shared<RSBackgroundShaderModifier>(property);
    modifier->property_ = nullptr;
    rsNode->propertyModifiers_[modifierType] = modifier;
    rsNode->SetFramePositionX(1.f);
    EXPECT_TRUE(!rsNode->propertyModifiers_.empty());
}

/**
 * @tc.name: SetFramePositionY
 * @tc.desc: test results of SetFramePositionY
 * @tc.type: FUNC
 * @tc.require: issueI9RLG7
 */
HWTEST_F(RSCanvasNodeTest, SetFramePositionY, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetFramePositionY(1.f);
    EXPECT_TRUE(!rsNode->propertyModifiers_.empty());

    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSPropertyBase>();
    RSModifierType modifierType = RSModifierType::FRAME;
    std::shared_ptr<RSModifier> modifier = std::make_shared<RSBackgroundShaderModifier>(property);
    modifier->property_ = nullptr;
    rsNode->propertyModifiers_[modifierType] = modifier;
    rsNode->SetFramePositionY(1.f);
    EXPECT_TRUE(!rsNode->propertyModifiers_.empty());
}

/**
 * @tc.name: SetPivotX
 * @tc.desc: test results of SetPivotX
 * @tc.type: FUNC
 * @tc.require: issueI9RLG7
 */
HWTEST_F(RSCanvasNodeTest, SetPivotX, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetPivotX(1.f);
    EXPECT_TRUE(!rsNode->propertyModifiers_.empty());

    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSPropertyBase>();
    RSModifierType modifierType = RSModifierType::PIVOT;
    std::shared_ptr<RSModifier> modifier = std::make_shared<RSBackgroundShaderModifier>(property);
    modifier->property_ = nullptr;
    rsNode->propertyModifiers_[modifierType] = modifier;
    rsNode->SetPivotX(1.f);
    EXPECT_TRUE(!rsNode->propertyModifiers_.empty());
}

/**
 * @tc.name: SetPivotY
 * @tc.desc: test results of SetPivotY
 * @tc.type: FUNC
 * @tc.require: issueI9RLG7
 */
HWTEST_F(RSCanvasNodeTest, SetPivotY, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetPivotY(1.f);
    EXPECT_TRUE(!rsNode->propertyModifiers_.empty());

    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSPropertyBase>();
    RSModifierType modifierType = RSModifierType::PIVOT;
    std::shared_ptr<RSModifier> modifier = std::make_shared<RSBackgroundShaderModifier>(property);
    modifier->property_ = nullptr;
    rsNode->propertyModifiers_[modifierType] = modifier;
    rsNode->SetPivotY(1.f);
    EXPECT_TRUE(!rsNode->propertyModifiers_.empty());
}

/**
 * @tc.name: SetTranslateX
 * @tc.desc: test results of SetTranslateX
 * @tc.type: FUNC
 * @tc.require: issueI9RLG7
 */
HWTEST_F(RSCanvasNodeTest, SetTranslateX, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetTranslateX(1.f);
    EXPECT_TRUE(!rsNode->propertyModifiers_.empty());

    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSPropertyBase>();
    RSModifierType modifierType = RSModifierType::TRANSLATE;
    std::shared_ptr<RSModifier> modifier = std::make_shared<RSBackgroundShaderModifier>(property);
    modifier->property_ = nullptr;
    rsNode->propertyModifiers_[modifierType] = modifier;
    rsNode->SetTranslateX(1.f);
    EXPECT_TRUE(!rsNode->propertyModifiers_.empty());
}

/**
 * @tc.name: SetTranslateY
 * @tc.desc: test results of SetTranslateY
 * @tc.type: FUNC
 * @tc.require: issueI9RLG7
 */
HWTEST_F(RSCanvasNodeTest, SetTranslateY, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetTranslateY(1.f);
    EXPECT_TRUE(!rsNode->propertyModifiers_.empty());

    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSPropertyBase>();
    RSModifierType modifierType = RSModifierType::TRANSLATE;
    std::shared_ptr<RSModifier> modifier = std::make_shared<RSBackgroundShaderModifier>(property);
    modifier->property_ = nullptr;
    rsNode->propertyModifiers_[modifierType] = modifier;
    rsNode->SetTranslateY(1.f);
    EXPECT_TRUE(!rsNode->propertyModifiers_.empty());
}

/**
 * @tc.name: SetScaleX
 * @tc.desc: test results of SetScaleX
 * @tc.type: FUNC
 * @tc.require: issueI9RLG7
 */
HWTEST_F(RSCanvasNodeTest, SetScaleX, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetScaleX(1.f);
    EXPECT_TRUE(!rsNode->propertyModifiers_.empty());

    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSPropertyBase>();
    RSModifierType modifierType = RSModifierType::SCALE;
    std::shared_ptr<RSModifier> modifier = std::make_shared<RSBackgroundShaderModifier>(property);
    modifier->property_ = nullptr;
    rsNode->propertyModifiers_[modifierType] = modifier;
    rsNode->SetScaleX(1.f);
    EXPECT_TRUE(!rsNode->propertyModifiers_.empty());
}

/**
 * @tc.name: SetScaleY
 * @tc.desc: test results of SetScaleY
 * @tc.type: FUNC
 * @tc.require: issueI9RLG7
 */
HWTEST_F(RSCanvasNodeTest, SetScaleY, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetScaleY(1.f);
    EXPECT_TRUE(!rsNode->propertyModifiers_.empty());

    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSPropertyBase>();
    RSModifierType modifierType = RSModifierType::SCALE;
    std::shared_ptr<RSModifier> modifier = std::make_shared<RSBackgroundShaderModifier>(property);
    modifier->property_ = nullptr;
    rsNode->propertyModifiers_[modifierType] = modifier;
    rsNode->SetScaleY(1.f);
    EXPECT_TRUE(!rsNode->propertyModifiers_.empty());
}

/**
 * @tc.name: SetSkewX
 * @tc.desc: test results of SetSkewX
 * @tc.type: FUNC
 * @tc.require: issueI9RLG7
 */
HWTEST_F(RSCanvasNodeTest, SetSkewX, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetSkewX(1.f);
    EXPECT_TRUE(!rsNode->propertyModifiers_.empty());

    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSPropertyBase>();
    RSModifierType modifierType = RSModifierType::SKEW;
    std::shared_ptr<RSModifier> modifier = std::make_shared<RSBackgroundShaderModifier>(property);
    modifier->property_ = nullptr;
    rsNode->propertyModifiers_[modifierType] = modifier;
    rsNode->SetSkewX(1.f);
    EXPECT_TRUE(!rsNode->propertyModifiers_.empty());
}

/**
 * @tc.name: SetSkewY
 * @tc.desc: test results of SetSkewY
 * @tc.type: FUNC
 * @tc.require: issueI9RLG7
 */
HWTEST_F(RSCanvasNodeTest, SetSkewY, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetSkewY(1.f);
    EXPECT_TRUE(!rsNode->propertyModifiers_.empty());

    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSPropertyBase>();
    RSModifierType modifierType = RSModifierType::SKEW;
    std::shared_ptr<RSModifier> modifier = std::make_shared<RSBackgroundShaderModifier>(property);
    modifier->property_ = nullptr;
    rsNode->propertyModifiers_[modifierType] = modifier;
    rsNode->SetSkewY(1.f);
    EXPECT_TRUE(!rsNode->propertyModifiers_.empty());
}

/**
 * @tc.name: SetPerspX
 * @tc.desc: test results of SetPerspX
 * @tc.type: FUNC
 * @tc.require: issueI9RLG7
 */
HWTEST_F(RSCanvasNodeTest, SetPerspX, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetPerspX(1.f);
    EXPECT_TRUE(!rsNode->propertyModifiers_.empty());

    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSPropertyBase>();
    RSModifierType modifierType = RSModifierType::PERSP;
    std::shared_ptr<RSModifier> modifier = std::make_shared<RSBackgroundShaderModifier>(property);
    modifier->property_ = nullptr;
    rsNode->propertyModifiers_[modifierType] = modifier;
    rsNode->SetPerspX(1.f);
    EXPECT_TRUE(!rsNode->propertyModifiers_.empty());
}

/**
 * @tc.name: SetPerspY
 * @tc.desc: test results of SetPerspY
 * @tc.type: FUNC
 * @tc.require: issueI9RLG7
 */
HWTEST_F(RSCanvasNodeTest, SetPerspY, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetPerspY(1.f);
    EXPECT_TRUE(!rsNode->propertyModifiers_.empty());

    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSPropertyBase>();
    RSModifierType modifierType = RSModifierType::PERSP;
    std::shared_ptr<RSModifier> modifier = std::make_shared<RSBackgroundShaderModifier>(property);
    modifier->property_ = nullptr;
    rsNode->propertyModifiers_[modifierType] = modifier;
    rsNode->SetPerspY(1.f);
    EXPECT_TRUE(!rsNode->propertyModifiers_.empty());
}

/**
 * @tc.name: SetandGetVisible001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasNodeTest, SetandGetVisible001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetVisible(true);
    EXPECT_EQ(rsNode->GetStagingProperties().GetVisible(), true);

    rsNode->transitionEffect_ = std::make_shared<RSTransitionEffect>();
    rsNode->SetVisible(false);
    EXPECT_EQ(rsNode->GetStagingProperties().GetVisible(), false);

    rsNode->SetVisible(true);
    EXPECT_EQ(rsNode->GetStagingProperties().GetVisible(), true);
}

/**
 * @tc.name: SetandGetVisible002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasNodeTest, SetandGetVisible002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest, SetandGetBorderStyle001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest, SetandGetBorderStyle002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest, SetandGetBorderStyle003, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest, SetandGetBorderStyle004, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest, SetandGetOutlineStyle001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest, SetandGetOutlineStyle002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest, SetandGetOutlineStyle003, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest, SetandGetOutlineStyle004, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest, SetandGetFrameGravity001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest, SetandGetFrameGravity002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest, SetandGetFrameGravity003, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest, SetandGetFrameGravity004, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest, SetandGetFrameGravity005, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest, SetandGetFrameGravity006, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest, SetandGetFrameGravity007, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest, SetandGetFrameGravity008, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest, SetandGetFrameGravity009, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest, SetandGetFrameGravity010, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest, SetandGetFrameGravity011, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest, SetandGetFrameGravity012, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest, SetandGetForegroundColor001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest, SetandGetForegroundColor002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest, SetandGetForegroundColor003, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest, SetandGetBackgroundColor001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest, SetandGetBackgroundColor002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest, SetandGetBackgroundColor003, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest, SetandGetBorderColor001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest, SetandGetBorderColor002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest, SetandGetBorderColor003, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest, SetandGetOutlineColor001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest, SetandGetOutlineColor002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest, SetandGetOutlineColor003, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest, SetandGetRotationVector001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    Vector4f quaternion(std::numeric_limits<float>::min(), 2.f, 3.f, 4.f);
    rsNode->SetRotation(quaternion);
}

/**
 * @tc.name: SetUIBackgroundFilter
 * @tc.desc: test results of SetUIBackgroundFilter
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasNodeTest, SetUIBackgroundFilter, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    Filter* filterObj = new(std::nothrow) Filter();
    std::shared_ptr<FilterBlurPara> para = std::make_shared<FilterBlurPara>();
    para->SetRadius(floatData[1]);
    filterObj->AddPara(para);
    rsNode->SetUIBackgroundFilter(filterObj);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBackgroundBlurRadiusX() == floatData[1]);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBackgroundBlurRadiusY() == floatData[1]);
    if (filterObj != nullptr) {
        delete filterObj;
        filterObj = nullptr;
    }
}

/**
 * @tc.name: SetUICompositingFilter
 * @tc.desc: test results of SetUICompositingFilter
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasNodeTest, SetUICompositingFilter, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    Filter* filterObj = new(std::nothrow) Filter();
    std::shared_ptr<FilterBlurPara> para = std::make_shared<FilterBlurPara>();
    para->SetRadius(floatData[1]);
    filterObj->AddPara(para);
    rsNode->SetUICompositingFilter(filterObj);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetForegroundBlurRadiusX() == floatData[1]);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetForegroundBlurRadiusY() == floatData[1]);
    if (filterObj != nullptr) {
        delete filterObj;
        filterObj = nullptr;
    }
}

/**
 * @tc.name: SetUIForegroundFilter
 * @tc.desc: test results of SetUIForegroundFilter
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasNodeTest, SetUIForegroundFilter, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    Filter* filterObj = new(std::nothrow) Filter();
    std::shared_ptr<FilterBlurPara> para = std::make_shared<FilterBlurPara>();
    para->SetRadius(floatData[1]);
    filterObj->AddPara(para);
    rsNode->SetUIForegroundFilter(filterObj);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetForegroundEffectRadius() == floatData[1]);
    if (filterObj != nullptr) {
        delete filterObj;
        filterObj = nullptr;
    }
}

/**
 * @tc.name: SetVisualEffect
 * @tc.desc: test results of SetVisualEffect
 * @tc.type: FUNC
 */
HWTEST_F(RSCanvasNodeTest, SetVisualEffect, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    VisualEffect* effectObj = new(std::nothrow) VisualEffect();
    std::shared_ptr<BrightnessBlender> blender = std::make_shared<BrightnessBlender>();
    blender->SetFraction(floatData[0]);
    std::shared_ptr<BackgroundColorEffectPara> para = std::make_shared<BackgroundColorEffectPara>();
    para->SetBlender(blender);
    effectObj->AddPara(para);
    rsNode->SetVisualEffect(effectObj);
    EXPECT_NE(floatData[0], 1.f);
    if (effectObj != nullptr) {
        delete effectObj;
        effectObj = nullptr;
    }
}

/**
 * @tc.name: SetandGetTranslateVector001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasNodeTest, SetandGetTranslateVector001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest, CreateBlurFilter001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest, CreateBlurFilter002, TestSize.Level2)
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
HWTEST_F(RSCanvasNodeTest, CreateBlurFilter003, TestSize.Level3)
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
HWTEST_F(RSCanvasNodeTest, CreateBlurFilter004, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest, CreateBlurFilter005, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest, CreateNormalFilter001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest, CreateNormalFilter002, TestSize.Level2)
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
HWTEST_F(RSCanvasNodeTest, CreateNormalFilter003, TestSize.Level3)
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
HWTEST_F(RSCanvasNodeTest, CreateNormalFilter004, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest, CreateNormalFilter005, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(floatData[4], floatData[0]);
    rsNode->SetFilter(filter);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetForegroundBlurRadiusX() == floatData[4]);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetForegroundBlurRadiusY() == floatData[0]);
}

/**
 * @tc.name: SetBackgroundFilter
 * @tc.desc: test results of SetBackgroundFilter
 * @tc.type: FUNC
 * @tc.require: issueI9RLG7
 */
HWTEST_F(RSCanvasNodeTest, SetBackgroundFilter, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> backgroundFilter = nullptr;
    rsNode->SetBackgroundFilter(backgroundFilter);
    EXPECT_TRUE(backgroundFilter == nullptr);

    backgroundFilter = std::make_shared<RSFilter>();
    backgroundFilter->type_ = RSFilter::MATERIAL;
    rsNode->SetBackgroundFilter(backgroundFilter);
    EXPECT_TRUE(backgroundFilter != nullptr);

    backgroundFilter->type_ = RSFilter::BLUR;
    rsNode->SetBackgroundFilter(backgroundFilter);
    EXPECT_TRUE(backgroundFilter != nullptr);
}

/**
 * @tc.name: SetFilter
 * @tc.desc: test results of SetFilter
 * @tc.type: FUNC
 * @tc.require: issueI9RLG7
 */
HWTEST_F(RSCanvasNodeTest, SetFilter, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> filter = nullptr;
    rsNode->SetFilter(filter);
    EXPECT_TRUE(filter == nullptr);

    filter = std::make_shared<RSFilter>();
    filter->type_ = RSFilter::MATERIAL;
    rsNode->SetFilter(filter);
    EXPECT_TRUE(filter != nullptr);

    filter->type_ = RSFilter::BLUR;
    rsNode->SetFilter(filter);
    EXPECT_TRUE(filter != nullptr);
}

/**
 * @tc.name: SetandGetClipBounds001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasNodeTest, SetandGetClipBounds001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest, GetId001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    ASSERT_NE(rsNode, nullptr);
    rsNode->GetId();
}

/**
 * @tc.name: GetChildren001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasNodeTest, GetChildren001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    ASSERT_NE(rsNode, nullptr);
    auto c = rsNode->GetChildren();
}

/**
 * @tc.name: GetChildren002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasNodeTest, GetChildren002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    ASSERT_NE(rsNode, nullptr);
    const auto c = rsNode->GetChildren();
}

/**
 * @tc.name: GetStagingProperties001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasNodeTest, GetStagingProperties001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    ASSERT_NE(rsNode, nullptr);
    rsNode->GetStagingProperties();
}

/**
 * @tc.name: GetMotionPathOption001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasNodeTest, GetMotionPathOption002, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    ASSERT_NE(rsNode, nullptr);
    rsNode->GetMotionPathOption();
}

/**
 * @tc.name: SetBgImage001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasNodeTest, SetBgImage001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    ASSERT_NE(rsNode, nullptr);
    auto image = std::make_shared<RSImage>();
    rsNode->SetBgImage(image);
}

/**
 * @tc.name: SetBackgroundShader001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasNodeTest, SetBackgroundShader001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    ASSERT_NE(rsNode, nullptr);
    auto shader = RSShader::CreateRSShader();
    rsNode->SetBackgroundShader(shader);
}

/**
 * @tc.name: SetandGetGreyCoef001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasNodeTest, SetGreyCoef001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    ASSERT_NE(rsNode, nullptr);
    Vector2f greyCoef = { 0.5, 0.5 };
    rsNode->SetGreyCoef(greyCoef);
}

/**
 * @tc.name: SetandGetAiInvertTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasNodeTest, SetAiInvertTest, TestSize.Level1)
{
    auto rootNode = RSCanvasNode::Create();
    ASSERT_NE(rootNode, nullptr);
    auto value = Vector4f(10.f);
    rootNode->SetAiInvert(value);
}

/**
 * @tc.name: SetandGetSpherizeDegree001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasNodeTest, SetandGetSpherizeDegree001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    float spherizeDegree = 1.0f;
    rsNode->SetSpherizeDegree(spherizeDegree);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetSpherizeDegree(), spherizeDegree));
}

/**
 * @tc.name: SetAttractionEffect
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasNodeTest, SetAttractionEffect, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    float attractionFraction = 1.0f;
    Vector2f attractionDstPoint = { 100.0, 100.0 };
    rsNode->SetAttractionEffect(attractionFraction, attractionDstPoint);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetAttractionFractionValue(), attractionFraction));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetAttractionDstPointValue(), attractionDstPoint));
}

/**
 * @tc.name: SetandGetSetAttractionEffectDstPoint001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasNodeTest, SetandGetSetAttractionEffectDstPoint001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    Vector2f attractionDstPoint = { 100.0, 100.0 };
    rsNode->SetAttractionEffectDstPoint(attractionDstPoint);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetAttractionDstPointValue(), attractionDstPoint));
}

/**
 * @tc.name: SetandGetLightUpEffectDegree001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasNodeTest, SetandGetLightUpEffectDegree001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest, SetandGetShadowIsFilled001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest, SetandGetShadowIsFilled002, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest, SetandGetForegroundEffectRadius001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest, SetCompositingFilter001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    ASSERT_NE(rsNode, nullptr);
    auto compositingFilter = RSFilter::CreateBlurFilter(0.0f, 0.0f);
    rsNode->SetCompositingFilter(compositingFilter);
}

/**
 * @tc.name: SetShadowPath001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasNodeTest, SetShadowPath001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    ASSERT_NE(rsNode, nullptr);
    auto shadowpath = RSPath::CreateRSPath();
    rsNode->SetShadowPath(shadowpath);
}

/**
 * @tc.name: SetFreeze001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasNodeTest, SetFreeze001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetFreeze(true);
    std::shared_ptr<RSNode> child = std::make_shared<RSNode>(0);
    child->SetFreeze(true);
    EXPECT_TRUE(child != nullptr);
}

template<typename ModifierName, typename PropertyName, typename T>
void SetPropertyTest(RSModifierType modifierType, T value1, T value2)
{
    auto node = RSCanvasNode::Create();
    node->SetProperty<ModifierName, PropertyName, T>(modifierType, value1);
    node->SetProperty<ModifierName, PropertyName, T>(modifierType, value1);
    node->SetProperty<ModifierName, PropertyName, T>(modifierType, value2);

    auto iter = node->propertyModifiers_.find(modifierType);
    ASSERT_TRUE(iter != node->propertyModifiers_.end());
};

/**
 * @tc.name: SetProperty001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasNodeTest, SetProperty001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest, SetModifier001, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest, SetModifier002, TestSize.Level1)
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
    node1->children_.push_back(1);
    node1->DumpNode(0);
    ASSERT_TRUE(node1->GetModifierIds().size() == 1);
}

/**
 * @tc.name: OpenImplicitAnimationTest001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasNodeTest, OpenImplicitAnimationTest001, TestSize.Level1)
{
    std::function<void()> finishCallback = nullptr;
    RSAnimationTimingProtocol timingProtocal;
    RSAnimationTimingCurve timingCurve;
    RSNode::OpenImplicitAnimation(timingProtocal, timingCurve, finishCallback);
    EXPECT_TRUE(finishCallback == nullptr);

    finishCallback = []() {};
    RSNode::OpenImplicitAnimation(timingProtocal, timingCurve, finishCallback);
    EXPECT_TRUE(finishCallback != nullptr);
}
/**
 * @tc.name: CloseImplicitAnimationTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasNodeTest, CloseImplicitAnimationTest, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest, AnimateTest, TestSize.Level1)
{
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    PropertyCallback propertyCallback = nullptr;
    std::function<void()> finishCallback = nullptr;
    std::function<void()> repeatCallback = nullptr;
    auto animate = RSNode::Animate(timingProtocol, timingCurve, propertyCallback, finishCallback);
    std::vector<std::shared_ptr<RSAnimation>> vec;
    EXPECT_EQ(vec, animate);

    propertyCallback = []() {};
    animate = RSNode::Animate(timingProtocol, timingCurve, propertyCallback, finishCallback);
    EXPECT_TRUE(propertyCallback != nullptr);

    finishCallback = []() {};
    animate = RSNode::Animate(timingProtocol, timingCurve, propertyCallback, finishCallback);
    EXPECT_TRUE(finishCallback != nullptr);

    repeatCallback = []() {};
    animate = RSNode::Animate(timingProtocol, timingCurve, propertyCallback, finishCallback, repeatCallback);
    EXPECT_TRUE(repeatCallback != nullptr);
}
/**
 * @tc.name: AnimateWithCurrentOptionsTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasNodeTest, AnimateWithCurrentOptionsTest, TestSize.Level1)
{
    PropertyCallback propertyCallback = nullptr;
    std::function<void()> finishCallback = nullptr;
    bool timingSensitive = true;
    auto animateWithCurrentOptions =
        RSNode::AnimateWithCurrentOptions(propertyCallback, finishCallback, timingSensitive);
    std::vector<std::shared_ptr<RSAnimation>> vec;
    EXPECT_EQ(vec, animateWithCurrentOptions);

    propertyCallback = []() {};
    animateWithCurrentOptions = RSNode::AnimateWithCurrentOptions(propertyCallback, finishCallback, timingSensitive);
    EXPECT_TRUE(propertyCallback != nullptr);

    finishCallback = []() {};
    animateWithCurrentOptions = RSNode::AnimateWithCurrentOptions(propertyCallback, finishCallback, timingSensitive);
    EXPECT_TRUE(finishCallback != nullptr);
}
/**
 * @tc.name: AnimateWithCurrentCallbackTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasNodeTest, AnimateWithCurrentCallbackTest, TestSize.Level1)
{
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    PropertyCallback propertyCallback = nullptr;
    auto Callback = RSNode::AnimateWithCurrentCallback(timingProtocol, timingCurve, propertyCallback);
    std::vector<std::shared_ptr<RSAnimation>> vec;
    EXPECT_EQ(vec, Callback);

    propertyCallback = []() {};
    Callback = RSNode::AnimateWithCurrentCallback(timingProtocol, timingCurve, propertyCallback);
    EXPECT_TRUE(propertyCallback != nullptr);
}

/**
 * @tc.name: SetColorBlendMode
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasNodeTest, SetColorBlendMode, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    ASSERT_NE(rsNode, nullptr);
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
HWTEST_F(RSCanvasNodeTest, SetTextureExport, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetTextureExport(true);
    ASSERT_TRUE(rsNode->IsTextureExportNode());

    rsNode->SetTextureExport(false);
    ASSERT_EQ(rsNode->IsTextureExportNode(), false);

    rsNode->isTextureExportNode_ = true;
    rsNode->SetTextureExport(false);
    EXPECT_TRUE(rsNode->children_.empty());
}

/**
 * @tc.name: SyncTextureExport
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasNodeTest, SyncTextureExport, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SyncTextureExport(false);

    rsNode->SyncTextureExport(true);
    EXPECT_TRUE(rsNode->children_.empty());

    rsNode->children_.push_back(0);
    rsNode->children_.push_back(1);
    rsNode->SyncTextureExport(true);
    EXPECT_TRUE(!rsNode->children_.empty());

    rsNode->isTextureExportNode_ = true;
    rsNode->SyncTextureExport(false);
    EXPECT_TRUE(!rsNode->children_.empty());
}

/**
 * @tc.name: CalcExpectedFrameRate
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSCanvasNodeTest, CalcExpectedFrameRate, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    auto canvasNode = RSCanvasNode::Create();
    rsNode->AddChild(canvasNode, -1);
    auto scene = "test_scene";
    float speed = 0;
    ASSERT_EQ(rsNode->CalcExpectedFrameRate(scene, speed), 0);
}

/**
 * @tc.name: RemoveChildByNodeIdTest Test
 * @tc.desc: test results of RemoveChildByNodeId
 * @tc.type: FUNC
 * @tc.require:issueI9MWJR
 */
HWTEST_F(RSCanvasNodeTest, RemoveChildByNodeIdTest, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    NodeId childId = 1;
    rsNode->RemoveChildByNodeId(childId);
    auto nodePtr = RSCanvasNode::Create();
    nodePtr->SetParent(2);
    RSNodeMap::MutableInstance().nodeMap_.emplace(nodePtr->GetId(), nodePtr);
    rsNode->RemoveChildByNodeId(nodePtr->GetId());
    auto list = rsNode->children_;
    ASSERT_EQ(std::find(list.begin(), list.end(), nodePtr->GetId()), list.end());
}

/**
 * @tc.name: RemoveChild
 * @tc.desc: test results of RemoveChild
 * @tc.type: FUNC
 * @tc.require: issueI9RLG7
 */
HWTEST_F(RSCanvasNodeTest, RemoveChild, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    std::shared_ptr<RSNode> child = nullptr;
    rsNode->RemoveChild(child);
    EXPECT_TRUE(child == nullptr);

    child = std::make_shared<RSNode>(0);
    child->parent_ = 2;
    rsNode->RemoveChild(child);
    EXPECT_TRUE(child != nullptr);

    child->parent_ = rsNode->id_;
    rsNode->RemoveChild(child);
    EXPECT_TRUE(child->parent_ != rsNode->id_);

    delete RSTransactionProxy::instance_;
    RSTransactionProxy::instance_ = nullptr;
    rsNode->RemoveChild(child);
    EXPECT_EQ(RSTransactionProxy::GetInstance(), nullptr);
    RSTransactionProxy::instance_ = new RSTransactionProxy();
}

/**
 * @tc.name: RemoveChildByNodeId001
 * @tc.desc: test results of RemoveChildByNodeId
 * @tc.type: FUNC
 * @tc.require: issueI9RLG7
 */
HWTEST_F(RSCanvasNodeTest, RemoveChildByNodeId001, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    NodeId childId = 0;
    rsNode->RemoveChildByNodeId(childId);
    EXPECT_TRUE(!childId);

    rsNode->RemoveChildByNodeId(1);
    EXPECT_TRUE(!childId);
}

/**
 * @tc.name: DrawOnNode Test
 * @tc.desc: DrawOnNode and SetFreeze
 * @tc.type: FUNC
 * @tc.require:issueI9MWJR
 */
HWTEST_F(RSCanvasNodeTest, DrawOnNode, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->DrawOnNode(RSModifierType::BOUNDS, [](std::shared_ptr<Drawing::Canvas> canvasPtr) {});
    ASSERT_FALSE(rsNode->recordingUpdated_);
    rsNode->SetFreeze(true);
    rsNode->InitUniRenderEnabled();
    rsNode->SetFreeze(true);
    ASSERT_NE(RSTransactionProxy::GetInstance()->implicitRemoteTransactionData_, nullptr);
}

/**
 * @tc.name: MarkDrivenRender Test
 * @tc.desc: MarkDrivenRender and MarkDrivenRenderItemIndex and MarkDrivenRenderFramePaintState and MarkContentChanged
 * @tc.type: FUNC
 * @tc.require:issueI9MWJR
 */
HWTEST_F(RSCanvasNodeTest, MarkDrivenRender, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->MarkDrivenRender(true);
    rsNode->MarkDrivenRenderItemIndex(1);
    rsNode->MarkDrivenRenderFramePaintState(true);
    rsNode->MarkContentChanged(true);
    ASSERT_FALSE(rsNode->isNodeGroup_);
}

/**
 * @tc.name: SetFrameNodeInfo
 * @tc.desc: test results of SetFrameNodeInfo
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSCanvasNodeTest, SetFrameNodeInfo, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest, GetFrameNodeId, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest, GetFrameNodeTag, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest, AddKeyFrame, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    ASSERT_NE(rsNode, nullptr);
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
HWTEST_F(RSCanvasNodeTest, AddDurationKeyFrame, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    ASSERT_NE(rsNode, nullptr);
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
HWTEST_F(RSCanvasNodeTest, IsImplicitAnimationOpen, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest, ExecuteWithoutAnimation, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    PropertyCallback callback = nullptr;
    std::shared_ptr<RSImplicitAnimator> implicitAnimator = nullptr;
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
HWTEST_F(RSCanvasNodeTest, FallbackAnimationsToRoot, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    auto target = RSNodeMap::Instance().GetAnimationFallbackNode();
    target = nullptr;
    rsNode->FallbackAnimationsToRoot();
    EXPECT_EQ(rsNode->motionPathOption_, nullptr);

    bool isRenderServiceNode = true;
    target = std::make_shared<RSNode>(isRenderServiceNode);
    AnimationId id = 1;
    std::shared_ptr<RSAnimation> animation = nullptr;
    rsNode->FallbackAnimationsToRoot();
    EXPECT_NE(RSNodeMap::Instance().animationFallbackNode_, nullptr);

    animation = std::make_shared<RSAnimation>();
    animation->repeatCount_ = 1;
    rsNode->animations_.insert({ id, animation });
    rsNode->FallbackAnimationsToRoot();
    EXPECT_TRUE(animation->repeatCount_);

    rsNode->animations_.clear();
    animation->repeatCount_ = -1;
    rsNode->animations_.insert({ id, animation });
    rsNode->FallbackAnimationsToRoot();
    EXPECT_TRUE(animation->repeatCount_ == -1);
}

/**
 * @tc.name: AddAnimationInner
 * @tc.desc: test results of AddAnimationInner
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSCanvasNodeTest, AddAnimationInner, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest, RemoveAnimationInner, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    auto animation = std::make_shared<RSAnimation>();
    rsNode->AddAnimationInner(animation);
    rsNode->RemoveAnimationInner(animation);
    EXPECT_NE(animation, nullptr);

    PropertyId id = animation->GetPropertyId();
    uint32_t num = 2;
    rsNode->animatingPropertyNum_.insert({ id, num });
    rsNode->RemoveAnimationInner(animation);
    EXPECT_NE(animation, nullptr);

    rsNode->animatingPropertyNum_.clear();
    num = 1;
    rsNode->animatingPropertyNum_.insert({ id, num });
    rsNode->RemoveAnimationInner(animation);
    EXPECT_NE(animation, nullptr);
}

/**
 * @tc.name: FinishAnimationByProperty
 * @tc.desc: test results of FinishAnimationByProperty
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSCanvasNodeTest, FinishAnimationByProperty, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    PropertyId id = 0; // for test
    auto animation = std::make_shared<RSAnimation>();
    rsNode->AddAnimationInner(animation);
    rsNode->FinishAnimationByProperty(id);
    EXPECT_NE(animation, nullptr);

    id = animation->GetPropertyId();
    rsNode->animations_.insert({ id, animation });
    rsNode->FinishAnimationByProperty(id);
    EXPECT_NE(animation, nullptr);
}

/**
 * @tc.name: CancelAnimationByProperty
 * @tc.desc: test results of CancelAnimationByProperty
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSCanvasNodeTest, CancelAnimationByProperty, TestSize.Level1)
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
HWTEST_F(RSCanvasNodeTest, GetShowingProperties, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    ASSERT_NE(rsNode, nullptr);
    rsNode->GetShowingProperties();
}

/**
 * @tc.name: AddAnimation
 * @tc.desc: test results of AddAnimation
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSCanvasNodeTest, AddAnimation, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    std::shared_ptr<RSAnimation> animation = nullptr;
    rsNode->AddAnimation(animation);
    EXPECT_EQ(animation, nullptr);

    animation = std::make_shared<RSAnimation>();
    rsNode->AddAnimation(animation);
    EXPECT_TRUE(animation != nullptr);

    rsNode->id_ = 0;
    rsNode->AddAnimation(animation);
    EXPECT_TRUE(animation != nullptr);

    animation->duration_ = 0;
    rsNode->AddAnimation(animation);
    EXPECT_TRUE(!animation->duration_);

    rsNode->id_ = 1;
    rsNode->AddAnimation(animation);
    EXPECT_TRUE(rsNode->id_ = 1);

    AnimationId id = animation->GetId();
    rsNode->animations_.insert({ id, animation });
    rsNode->AddAnimation(animation);
    EXPECT_TRUE(!rsNode->animations_.empty());
}

/**
 * @tc.name: RemoveAllAnimations
 * @tc.desc: test results of RemoveAllAnimations
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSCanvasNodeTest, RemoveAllAnimations, TestSize.Level1)
{
    auto rsNode = RSCanvasNode::Create();
    AnimationId id = 1;
    auto animation = std::make_shared<RSAnimation>();
    rsNode->animations_.insert({ id, animation });
    rsNode->RemoveAllAnimations();
    EXPECT_NE(animation, nullptr);
}
} // namespace OHOS::Rosen