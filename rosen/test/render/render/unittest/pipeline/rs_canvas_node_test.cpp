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
} // namespace OHOS::Rosen