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
} // namespace OHOS::Rosen