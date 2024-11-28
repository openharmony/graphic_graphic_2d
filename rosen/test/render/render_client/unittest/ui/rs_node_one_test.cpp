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

class RSNodeOneTest : public testing::Test {
public:
    constexpr static float floatDatas[] = {
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
    void SetBorderDashParamsAndTest(const Vector4f& param) const
    {
        auto rsnode = RSCanvasNode::Create();
        rsnode->SetBorderDashWidth(param);
        rsnode->SetBorderDashGap(param);
        auto borderDashWidth = rsnode->GetStagingProperties().GetBorderDashWidth();
        auto borderDashGap = rsnode->GetStagingProperties().GetBorderDashGap();
        EXPECT_TRUE(borderDashWidth.IsNearEqual(param));
        EXPECT_TRUE(borderDashGap.IsNearEqual(param));
    }
    void SetOutlineDashParamsAndTest(const Vector4f& param) const
    {
        auto rsnode = RSCanvasNode::Create();
        rsnode->SetOutlineDashWidth(param);
        rsnode->SetOutlineDashGap(param);
        auto borderOutlineWidth = rsnode->GetStagingProperties().GetOutlineDashWidth();
        auto borderOutlineGap = rsnode->GetStagingProperties().GetOutlineDashGap();
        EXPECT_TRUE(borderOutlineWidth.IsNearEqual(param));
        EXPECT_TRUE(borderOutlineGap.IsNearEqual(param));
    }
};

void RSNodeOneTest::SetUpTestCase() {}
void RSNodeOneTest::TearDownTestCase() {}
void RSNodeOneTest::SetUp() {}
void RSNodeOneTest::TearDown() {}

/**
 * @tc.name: Recording01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, Recording01, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSNode and RSUIDirector
     */
    auto rsnode = RSCanvasNode::Create();

    /**
     * @tc.steps: step2. begin recording
     */
    EXPECT_FALSE(rsnode->IsRecording());
    rsnode->BeginRecording(50, 40);
    EXPECT_TRUE(rsnode->IsRecording());
}

/**
 * @tc.name: SetPaintOrder01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetPaintOrder01, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSNode and RSUIDirector
     */
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetPaintOrder(true);
}

/**
 * @tc.name: SetandGetBounds01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetBounds01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetBounds(floatDatas[0], floatDatas[1], floatDatas[2], floatDatas[3]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBounds().x_, floatDatas[0]));
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBounds().y_, floatDatas[1]));
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBounds().z_, floatDatas[2]));
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBounds().w_, floatDatas[3]));
}

/**
 * @tc.name: SetandGetBounds02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetBounds02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetBounds(floatDatas[3], floatDatas[1], floatDatas[2], floatDatas[0]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBounds().x_, floatDatas[3]));
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBounds().y_, floatDatas[1]));
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBounds().z_, floatDatas[2]));
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBounds().w_, floatDatas[0]));
}

/**
 * @tc.name: SetandGetBounds03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetBounds03, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetBounds(floatDatas[3], floatDatas[2], floatDatas[1], floatDatas[0]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBounds().x_, floatDatas[3]));
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBounds().y_, floatDatas[2]));
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBounds().z_, floatDatas[1]));
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBounds().w_, floatDatas[0]));
}

/**
 * @tc.name: LifeCycle01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, LifeCycle01, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSNode and add child
     */
    auto rootNode = RSCanvasNode::Create();
    ASSERT_TRUE(rootNode != nullptr);

    auto chld1 = RSCanvasNode::Create();
    auto chld2 = RSCanvasNode::Create();
    auto chld3 = RSCanvasNode::Create();
    rootNode->AddChild(chld1, -1);
    rootNode->AddChild(chld2, 0);
    chld1->AddChild(chld3, 1);

    /**
     * @tc.steps: step2. remove child
     */
    rootNode->RemoveChild(chld2);
}

/**
 * @tc.name: LifeCycle02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, LifeCycle02, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSNode and add child
     */
    auto rootNode = RSCanvasNode::Create();
    ASSERT_TRUE(rootNode != nullptr);

    auto chld1 = RSCanvasNode::Create();
    auto chld2 = RSCanvasNode::Create();
    auto chld3 = RSCanvasNode::Create();
    rootNode->AddChild(chld1, -1);
    rootNode->AddChild(chld2, 0);
    chld1->AddChild(chld3, 1);
    /**
     * @tc.steps: step2. remove child
     */
    rootNode->RemoveChild(chld2);
}

/**
 * @tc.name: LifeCycle03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, LifeCycle03, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSNode and add child
     */
    auto rootNode = RSCanvasNode::Create();
    ASSERT_TRUE(rootNode != nullptr);

    auto chld1 = RSCanvasNode::Create();
    auto chld2 = RSCanvasNode::Create();
    auto chld3 = RSCanvasNode::Create();
    rootNode->AddChild(chld1, -1);
    rootNode->AddChild(chld2, 0);
    chld1->AddChild(chld3, 1);
    /**
     * @tc.steps: step2. remove child
     */
    rootNode->RemoveFromTree();
}

/**
 * @tc.name: LifeCycle04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, LifeCycle04, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSNode and add child
     */
    auto rootNode = RSCanvasNode::Create();
    ASSERT_TRUE(rootNode != nullptr);

    auto chld1 = RSCanvasNode::Create();
    auto chld2 = RSCanvasNode::Create();
    auto chld3 = RSCanvasNode::Create();
    rootNode->AddChild(chld1, -1);
    rootNode->AddChild(chld2, 0);
    chld1->AddChild(chld3, 1);
    /**
     * @tc.steps: step2. remove child
     */
    rootNode->ClearChildren();
}

/**
 * @tc.name: LifeCycle05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, LifeCycle05, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSNode and add child
     */
    auto rootNode = RSCanvasNode::Create();
    ASSERT_TRUE(rootNode != nullptr);

    auto chld1 = RSCanvasNode::Create();
    auto chld2 = RSCanvasNode::Create();
    auto chld3 = RSCanvasNode::Create();
    rootNode->AddChild(chld1, -1);
    rootNode->AddChild(chld2, 0);
    chld1->AddChild(chld3, 1);
    /**
     * @tc.steps: step2. remove child
     */
    rootNode->RemoveChild(chld3);
}

/**
 * @tc.name: SetandGetBoundsWidth01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetBoundsWidth01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetBoundsWidth(floatDatas[1]);
    rsnode->SetBoundsWidth(floatDatas[1]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBounds().z_, floatDatas[1]));
}

/**
 * @tc.name: SetandGetBoundsWidth02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetBoundsWidth02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetBoundsWidth(floatDatas[2]);
    rsnode->SetBoundsWidth(floatDatas[2]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBounds().z_, floatDatas[2]));
}

/**
 * @tc.name: SetandGetBoundsWidth03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetBoundsWidth03, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetBoundsWidth(floatDatas[3]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBounds().z_, floatDatas[3]));
}

/**
 * @tc.name: SetIlluminatedBorderWidthTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetIlluminatedBorderWidthTest, TestSize.Level1)
{
    auto rootNode = RSCanvasNode::Create();
    rootNode->SetIlluminatedBorderWidth(floatDatas[1]);
    EXPECT_TRUE(ROSEN_EQ(rootNode->GetStagingProperties().GetIlluminatedBorderWidth(), floatDatas[1]));
}

/**
 * @tc.name: SetandGetBoundsWidth04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetBoundsWidth04, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetBoundsWidth(floatDatas[4]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBounds().z_, floatDatas[4]));
}

/**
 * @tc.name: SetandGetBoundsWidth05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetBoundsWidth05, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetBoundsWidth(floatDatas[0]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBounds().z_, floatDatas[0]));
}

/**
 * @tc.name: SetandGetBoundsHeight01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetBoundsHeight01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetBoundsHeight(floatDatas[1]);
    rsnode->SetBoundsHeight(floatDatas[1]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBounds().w_, floatDatas[1]));
}

/**
 * @tc.name: SetandGetBoundsHeight02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetBoundsHeight02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetBoundsHeight(floatDatas[2]);
    rsnode->SetBoundsHeight(floatDatas[2]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBounds().w_, floatDatas[2]));
}

/**
 * @tc.name: SetandGetBoundsHeight03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetBoundsHeight03, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetBoundsHeight(floatDatas[3]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBounds().w_, floatDatas[3]));
}

/**
 * @tc.name: SetandGetBoundsHeight04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetBoundsHeight04, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetBoundsHeight(floatDatas[4]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBounds().w_, floatDatas[4]));
}

/**
 * @tc.name: SetandGetBoundsHeight05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetBoundsHeight05, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetBoundsHeight(floatDatas[0]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBounds().w_, floatDatas[0]));
}

/**
 * @tc.name: SetandGetFrame01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetFrame01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetFrame(floatDatas[0], floatDatas[1], floatDatas[2], floatDatas[3]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetFrame().x_, floatDatas[0]));
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetFrame().y_, floatDatas[1]));
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetFrame().z_, floatDatas[2]));
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetFrame().w_, floatDatas[3]));
}

/**
 * @tc.name: SetandGetFrame02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetFrame02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetFrame(floatDatas[3], floatDatas[1], floatDatas[2], floatDatas[0]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetFrame().x_, floatDatas[3]));
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetFrame().y_, floatDatas[1]));
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetFrame().z_, floatDatas[2]));
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetFrame().w_, floatDatas[0]));
}

/**
 * @tc.name: SetandGetFrame03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetFrame03, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetFrame(floatDatas[3], floatDatas[2], floatDatas[1], floatDatas[0]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetFrame().x_, floatDatas[3]));
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetFrame().y_, floatDatas[2]));
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetFrame().z_, floatDatas[1]));
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetFrame().w_, floatDatas[0]));
}

/**
 * @tc.name: SetandGetFramePositionX01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetFramePositionX01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetFramePositionX(floatDatas[1]);
    rsnode->SetFramePositionX(floatDatas[1]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetFrame().x_, floatDatas[1]));
}

/**
 * @tc.name: SetandGetFramePositionX02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetFramePositionX02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetFramePositionX(floatDatas[2]);
    rsnode->SetFramePositionX(floatDatas[2]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetFrame().x_, floatDatas[2]));
}

/**
 * @tc.name: SetandGetFramePositionX03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetFramePositionX03, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetFramePositionX(floatDatas[3]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetFrame().x_, floatDatas[3]));
}

/**
 * @tc.name: SetandGetFramePositionX04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetFramePositionX04, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetFramePositionX(floatDatas[4]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetFrame().x_, floatDatas[4]));
}

/**
 * @tc.name: SetandGetFramePositionX05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetFramePositionX05, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetFramePositionX(floatDatas[0]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetFrame().x_, floatDatas[0]));
}

/**
 * @tc.name: SetandGetFramePositionY01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetFramePositionY01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetFramePositionY(floatDatas[1]);
    rsnode->SetFramePositionY(floatDatas[1]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetFrame().y_, floatDatas[1]));
}

/**
 * @tc.name: SetandGetFramePositionY02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetFramePositionY02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetFramePositionY(floatDatas[2]);
    rsnode->SetFramePositionY(floatDatas[2]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetFrame().y_, floatDatas[2]));
}

/**
 * @tc.name: SetandGetFramePositionY03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetFramePositionY03, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetFramePositionY(floatDatas[3]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetFrame().y_, floatDatas[3]));
}

/**
 * @tc.name: SetandGetFramePositionY04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetFramePositionY04, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetFramePositionY(floatDatas[4]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetFrame().y_, floatDatas[4]));
}

/**
 * @tc.name: SetandGetFramePositionY05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetFramePositionY05, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetFramePositionY(floatDatas[0]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetFrame().y_, floatDatas[0]));
}

/**
 * @tc.name: SetandGetPositionZ01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetPositionZ01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetPositionZ(floatDatas[1]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetPositionZ(), floatDatas[1]));
}

/**
 * @tc.name: SetandGetPositionZ02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetPositionZ02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetPositionZ(floatDatas[2]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetPositionZ(), floatDatas[2]));
}

/**
 * @tc.name: SetandGetPositionZ03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetPositionZ03, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetPositionZ(floatDatas[3]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetPositionZ(), floatDatas[3]));
}

/**
 * @tc.name: SetandGetPositionZ04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetPositionZ04, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetPositionZ(floatDatas[4]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetPositionZ(), floatDatas[4]));
}

/**
 * @tc.name: SetandGetPositionZ05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetPositionZ05, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetPositionZ(floatDatas[0]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetPositionZ(), floatDatas[0]));
}

/**
 * @tc.name: SetandGetCornerRadius01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetCornerRadius01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetCornerRadius(floatDatas[1]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetCornerRadius().x_, floatDatas[1]));
}

/**
 * @tc.name: SetandGetCornerRadius02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetCornerRadius02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetCornerRadius(floatDatas[2]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetCornerRadius().x_, floatDatas[2]));
}

/**
 * @tc.name: SetandGetCornerRadius03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetCornerRadius03, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetCornerRadius(floatDatas[3]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetCornerRadius().x_, floatDatas[3]));
}

/**
 * @tc.name: SetandGetCornerRadius04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetCornerRadius04, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetCornerRadius(floatDatas[4]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetCornerRadius().x_, floatDatas[4]));
}

/**
 * @tc.name: SetandGetCornerRadius05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetCornerRadius05, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetCornerRadius(floatDatas[0]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetCornerRadius().x_, floatDatas[0]));
}

/**
 * @tc.name: SetandGetOutlineRadius01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetOutlineRadius01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetOutlineRadius(floatDatas[1]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetOutlineRadius().x_, floatDatas[1]));
}

/**
 * @tc.name: SetandGetOutlineRadius02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetOutlineRadius02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetOutlineRadius(floatDatas[2]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetOutlineRadius().x_, floatDatas[2]));
}

/**
 * @tc.name: SetandGetOutlineRadius03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetOutlineRadius03, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetOutlineRadius(floatDatas[3]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetOutlineRadius().x_, floatDatas[3]));
}

/**
 * @tc.name: SetandGetOutlineRadius04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetOutlineRadius04, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetOutlineRadius(floatDatas[4]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetOutlineRadius().x_, floatDatas[4]));
}

/**
 * @tc.name: SetandGetOutlineRadius05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetOutlineRadius05, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetOutlineRadius(floatDatas[0]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetOutlineRadius().x_, floatDatas[0]));
}

/**
 * @tc.name: SetandGetRotationThree01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetRotationThree01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetRotation(floatDatas[1], floatDatas[2], floatDatas[3]);
}

/**
 * @tc.name: SetandGetRotation01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetRotation01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetRotation(floatDatas[1]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetRotation(), floatDatas[1]));
}

/**
 * @tc.name: SetandGetRotation02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetRotation02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetRotation(floatDatas[2]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetRotation(), floatDatas[2]));
}

/**
 * @tc.name: SetandGetRotation03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetRotation03, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetRotation(floatDatas[3]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetRotation(), floatDatas[3]));
}

/**
 * @tc.name: SetandGetRotation04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetRotation04, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetRotation(floatDatas[4]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetRotation(), floatDatas[4]));
}

/**
 * @tc.name: SetandGetRotation05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetRotation05, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetRotation(floatDatas[0]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetRotation(), floatDatas[0]));
}

/**
 * @tc.name: SetandGetRotationX01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetRotationX01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetRotationX(floatDatas[1]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetRotationX(), floatDatas[1]));
}

/**
 * @tc.name: SetandGetRotationX02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetRotationX02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetRotationX(floatDatas[2]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetRotationX(), floatDatas[2]));
}

/**
 * @tc.name: SetandGetRotationX03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetRotationX03, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetRotationX(floatDatas[3]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetRotationX(), floatDatas[3]));
}

/**
 * @tc.name: SetandGetRotationX04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetRotationX04, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetRotationX(floatDatas[4]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetRotationX(), floatDatas[4]));
}

/**
 * @tc.name: SetandGetRotationX05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetRotationX05, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetRotationX(floatDatas[0]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetRotationX(), floatDatas[0]));
}

/**
 * @tc.name: SetandGetRotationY01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetRotationY01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetRotationY(floatDatas[1]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetRotationY(), floatDatas[1]));
}

/**
 * @tc.name: SetandGetRotationY02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetRotationY02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetRotationY(floatDatas[2]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetRotationY(), floatDatas[2]));
}

/**
 * @tc.name: SetandGetRotationY03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetRotationY03, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetRotationY(floatDatas[3]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetRotationY(), floatDatas[3]));
}

/**
 * @tc.name: SetandGetRotationY04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetRotationY04, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetRotationY(floatDatas[4]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetRotationY(), floatDatas[4]));
}

/**
 * @tc.name: SetandGetRotationY05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetRotationY05, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetRotationY(floatDatas[0]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetRotationY(), floatDatas[0]));
}


/**
 * @tc.name: SetandGetScaleX01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetScaleX01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetScaleX(floatDatas[1]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetScale().x_, floatDatas[1]));
}

/**
 * @tc.name: SetandGetScaleX02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetScaleX02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetScaleX(floatDatas[2]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetScale().x_, floatDatas[2]));
}

/**
 * @tc.name: SetandGetScaleX03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetScaleX03, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetScaleX(floatDatas[3]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetScale().x_, floatDatas[3]));
}

/**
 * @tc.name: SetandGetScaleX04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetScaleX04, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetScaleX(floatDatas[4]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetScale().x_, floatDatas[4]));
}

/**
 * @tc.name: SetandGetScaleX05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetScaleX05, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetScaleX(floatDatas[0]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetScale().x_, floatDatas[0]));
}

/**
 * @tc.name: SetandGetScale01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetScale01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetScale(floatDatas[1]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetScale().x_, floatDatas[1]));
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetScale().y_, floatDatas[1]));
}

/**
 * @tc.name: SetandGetScale02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetScale02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetScale(floatDatas[2]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetScale().x_, floatDatas[2]));
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetScale().y_, floatDatas[2]));
}

/**
 * @tc.name: SetandGetScale03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetScale03, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetScale(floatDatas[3]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetScale().x_, floatDatas[3]));
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetScale().y_, floatDatas[3]));
}

/**
 * @tc.name: SetandGetScale04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetScale04, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetScale(floatDatas[3], floatDatas[0]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetScale().x_, floatDatas[3]));
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetScale().y_, floatDatas[0]));
}

/**
 * @tc.name: SetandGetScale005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetScale005, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetScale(floatDatas[2], floatDatas[1]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetScale().x_, floatDatas[2]));
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetScale().y_, floatDatas[1]));
}

/**
 * @tc.name: SetandGetScaleY01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetScaleY01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetScaleY(floatDatas[1]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetScale().y_, floatDatas[1]));
}

/**
 * @tc.name: SetandGetScaleY02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetScaleY02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetScaleY(floatDatas[2]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetScale().y_, floatDatas[2]));
}

/**
 * @tc.name: SetandGetScaleY03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetScaleY03, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetScaleY(floatDatas[3]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetScale().y_, floatDatas[3]));
}

/**
 * @tc.name: SetandGetScaleY04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetScaleY04, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetScaleY(floatDatas[4]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetScale().y_, floatDatas[4]));
}

/**
 * @tc.name: SetandGetScaleY05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetScaleY05, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetScaleY(floatDatas[0]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetScale().y_, floatDatas[0]));
}

/**
 * @tc.name: SetandGetSkew01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetSkew01, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetSkew(floatDatas[1]);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetSkew().x_, floatDatas[1]));
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetSkew().y_, floatDatas[1]));
}

/**
 * @tc.name: SetandGetSkew02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetSkew02, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetSkew(floatDatas[2]);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetSkew().x_, floatDatas[2]));
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetSkew().y_, floatDatas[2]));
}

/**
 * @tc.name: SetandGetSkew03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetSkew03, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetSkew(floatDatas[3]);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetSkew().x_, floatDatas[3]));
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetSkew().y_, floatDatas[3]));
}

/**
 * @tc.name: SetandGetSkew04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetSkew04, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetSkew(floatDatas[3], floatDatas[0]);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetSkew().x_, floatDatas[3]));
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetSkew().y_, floatDatas[0]));
}

/**
 * @tc.name: SetandGetSkew005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetSkew005, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetSkew(floatDatas[2], floatDatas[1]);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetSkew().x_, floatDatas[2]));
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetSkew().y_, floatDatas[1]));
}

/**
 * @tc.name: SetandGetSkewX01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetSkewX01, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetSkewX(floatDatas[1]);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetSkew().x_, floatDatas[1]));
}

/**
 * @tc.name: SetandGetSkewX02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetSkewX02, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetSkewX(floatDatas[2]);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetSkew().x_, floatDatas[2]));
}

/**
 * @tc.name: SetandGetSkewX03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetSkewX03, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetSkewX(floatDatas[3]);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetSkew().x_, floatDatas[3]));
}

/**
 * @tc.name: SetandGetSkewX04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetSkewX04, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetSkewX(floatDatas[4]);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetSkew().x_, floatDatas[4]));
}

/**
 * @tc.name: SetandGetSkewX05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetSkewX05, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetSkewX(floatDatas[0]);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetSkew().x_, floatDatas[0]));
}

/**
 * @tc.name: SetandGetSkewY01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetSkewY01, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetSkewY(floatDatas[1]);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetSkew().y_, floatDatas[1]));
}

/**
 * @tc.name: SetandGetSkewY02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetSkewY02, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetSkewY(floatDatas[2]);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetSkew().y_, floatDatas[2]));
}

/**
 * @tc.name: SetandGetSkewY03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetSkewY03, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetSkewY(floatDatas[3]);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetSkew().y_, floatDatas[3]));
}

/**
 * @tc.name: SetandGetSkewY04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetSkewY04, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetSkewY(floatDatas[4]);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetSkew().y_, floatDatas[4]));
}

/**
 * @tc.name: SetandGetSkewY05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetSkewY05, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetSkewY(floatDatas[0]);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetSkew().y_, floatDatas[0]));
}

/**
 * @tc.name: SetandGetPersp01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetPersp01, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetPersp(FLOAT_DATA_POSITIVE);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetPersp().x_, FLOAT_DATA_POSITIVE));
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetPersp().y_, FLOAT_DATA_POSITIVE));
}

/**
 * @tc.name: SetandGetPersp02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetPersp02, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetPersp(FLOAT_DATA_NEGATIVE);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetPersp().x_, FLOAT_DATA_NEGATIVE));
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetPersp().y_, FLOAT_DATA_NEGATIVE));
}

/**
 * @tc.name: SetandGetPersp03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetPersp03, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetPersp(FLOAT_DATA_MAX);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetPersp().x_, FLOAT_DATA_MAX));
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetPersp().y_, FLOAT_DATA_MAX));
}

/**
 * @tc.name: SetandGetPersp04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetPersp04, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetPersp(FLOAT_DATA_MAX, FLOAT_DATA_ZERO);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetPersp().x_, FLOAT_DATA_MAX));
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetPersp().y_, FLOAT_DATA_ZERO));
}

/**
 * @tc.name: SetandGetPersp005
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetPersp005, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetPersp(FLOAT_DATA_NEGATIVE, FLOAT_DATA_POSITIVE);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetPersp().x_, FLOAT_DATA_NEGATIVE));
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetPersp().y_, FLOAT_DATA_POSITIVE));
}

/**
 * @tc.name: SetandGetPerspX01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetPerspX01, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetPerspX(FLOAT_DATA_POSITIVE);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetPersp().x_, FLOAT_DATA_POSITIVE));
}

/**
 * @tc.name: SetandGetPerspX02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetPerspX02, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetPerspX(FLOAT_DATA_NEGATIVE);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetPersp().x_, FLOAT_DATA_NEGATIVE));
}

/**
 * @tc.name: SetandGetPerspX03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetPerspX03, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetPerspX(FLOAT_DATA_MAX);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetPersp().x_, FLOAT_DATA_MAX));
}

/**
 * @tc.name: SetandGetPerspX04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetPerspX04, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetPerspX(FLOAT_DATA_MIN);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetPersp().x_, FLOAT_DATA_MIN));
}

/**
 * @tc.name: SetandGetPerspX05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetPerspX05, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetPerspX(FLOAT_DATA_ZERO);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetPersp().x_, FLOAT_DATA_ZERO));
}

/**
 * @tc.name: SetandGetPerspY01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetPerspY01, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetPerspY(FLOAT_DATA_POSITIVE);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetPersp().y_, FLOAT_DATA_POSITIVE));
}

/**
 * @tc.name: SetandGetPerspY02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetPerspY02, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetPerspY(FLOAT_DATA_NEGATIVE);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetPersp().y_, FLOAT_DATA_NEGATIVE));
}

/**
 * @tc.name: SetandGetPerspY03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetPerspY03, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetPerspY(FLOAT_DATA_MAX);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetPersp().y_, FLOAT_DATA_MAX));
}

/**
 * @tc.name: SetandGetPerspY04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetPerspY04, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetPerspY(FLOAT_DATA_MIN);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetPersp().y_, FLOAT_DATA_MIN));
}

/**
 * @tc.name: SetandGetPerspY05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetPerspY05, TestSize.Level1)
{
    RSCanvasNode::SharedPtr canvasNode = RSCanvasNode::Create();
    canvasNode->SetPerspY(FLOAT_DATA_ZERO);
    EXPECT_TRUE(ROSEN_EQ(canvasNode->GetStagingProperties().GetPersp().y_, FLOAT_DATA_ZERO));
}

/**
 * @tc.name: SetandGetAlphaOffscreen01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetAlphaOffscreen01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetAlphaOffscreen(true);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetAlphaOffscreen(), true));
}

/**
 * @tc.name: SetandGetAlphaOffscreen02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetAlphaOffscreen02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetAlphaOffscreen(false);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetAlphaOffscreen(), false));
}

/**
 * @tc.name: SetandGetAlpha01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetAlpha01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetAlpha(floatDatas[1]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetAlpha(), floatDatas[1]));
}

/**
 * @tc.name: SetandGetAlpha02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetAlpha02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetAlpha(floatDatas[2]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetAlpha(), floatDatas[2]));
}

/**
 * @tc.name: SetandGetAlpha03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetAlpha03, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetAlpha(floatDatas[3]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetAlpha(), floatDatas[3]));
}

/**
 * @tc.name: SetandGetAlpha04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetAlpha04, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetAlpha(floatDatas[4]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetAlpha(), floatDatas[4]));
}

/**
 * @tc.name: SetandGetAlpha05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetAlpha05, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetAlpha(floatDatas[0]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetAlpha(), floatDatas[0]));
}

/**
 * @tc.name: SetandGetBgImageSize01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetBgImageSize01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetBgImageSize(floatDatas[0], floatDatas[1]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBgImageWidth(), floatDatas[0]));
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBgImageHeight(), floatDatas[1]));
}

/**
 * @tc.name: SetandGetBgImageSize02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetBgImageSize02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetBgImageSize(floatDatas[3], floatDatas[1]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBgImageWidth(), floatDatas[3]));
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBgImageHeight(), floatDatas[1]));
}

/**
 * @tc.name: SetandGetBgImageSize03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetBgImageSize03, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetBgImageSize(floatDatas[3], floatDatas[2]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBgImageWidth(), floatDatas[3]));
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBgImageHeight(), floatDatas[2]));
}

/**
 * @tc.name: SetandGetBgImageWidth01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetBgImageWidth01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetBgImageWidth(floatDatas[1]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBgImageWidth(), floatDatas[1]));
}

/**
 * @tc.name: SetandGetBgImageWidth02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetBgImageWidth02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetBgImageWidth(floatDatas[2]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBgImageWidth(), floatDatas[2]));
}

/**
 * @tc.name: SetandGetBgImageWidth03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetBgImageWidth03, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetBgImageWidth(floatDatas[3]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBgImageWidth(), floatDatas[3]));
}

/**
 * @tc.name: SetandGetBgImageWidth04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetBgImageWidth04, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetBgImageWidth(floatDatas[4]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBgImageWidth(), floatDatas[4]));
}

/**
 * @tc.name: SetandGetBgImageWidth05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetBgImageWidth05, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetBgImageWidth(floatDatas[0]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBgImageWidth(), floatDatas[0]));
}

/**
 * @tc.name: SetandGetBgImageHeight01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetBgImageHeight01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetBgImageHeight(floatDatas[1]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBgImageHeight(), floatDatas[1]));
}

/**
 * @tc.name: SetandGetBgImageHeight02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetBgImageHeight02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetBgImageHeight(floatDatas[2]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBgImageHeight(), floatDatas[2]));
}

/**
 * @tc.name: SetandGetBgImageHeight03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetBgImageHeight03, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetBgImageHeight(floatDatas[3]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBgImageHeight(), floatDatas[3]));
}

/**
 * @tc.name: SetandGetBgImageHeight04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetBgImageHeight04, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetBgImageHeight(floatDatas[4]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBgImageHeight(), floatDatas[4]));
}

/**
 * @tc.name: SetandGetBgImageHeight05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetBgImageHeight05, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetBgImageHeight(floatDatas[0]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBgImageHeight(), floatDatas[0]));
}

/**
 * @tc.name: SetandSetBgImagePosition01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandSetBgImagePosition01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetBgImagePosition(floatDatas[2], floatDatas[3]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBgImagePositionX(), floatDatas[2]));
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBgImagePositionY(), floatDatas[3]));
}

/**
 * @tc.name: SetandSetBgImagePosition02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandSetBgImagePosition02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetBgImagePosition(floatDatas[2], floatDatas[0]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBgImagePositionX(), floatDatas[2]));
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBgImagePositionY(), floatDatas[0]));
}

/**
 * @tc.name: SetandSetBgImagePosition03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandSetBgImagePosition03, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetBgImagePosition(floatDatas[1], floatDatas[3]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBgImagePositionX(), floatDatas[1]));
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBgImagePositionY(), floatDatas[3]));
}

/**
 * @tc.name: SetandGetBgImagePositionX01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetBgImagePositionX01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetBgImagePositionX(floatDatas[1]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBgImagePositionX(), floatDatas[1]));
}

/**
 * @tc.name: SetandGetBgImagePositionX02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetBgImagePositionX02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetBgImagePositionX(floatDatas[2]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBgImagePositionX(), floatDatas[2]));
}

/**
 * @tc.name: SetandGetBgImagePositionX03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetBgImagePositionX03, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetBgImagePositionX(floatDatas[3]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBgImagePositionX(), floatDatas[3]));
}

/**
 * @tc.name: SetandGetBgImagePositionX04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetBgImagePositionX04, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetBgImagePositionX(floatDatas[4]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBgImagePositionX(), floatDatas[4]));
}

/**
 * @tc.name: SetandGetBgImagePositionX05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetBgImagePositionX05, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetBgImagePositionX(floatDatas[0]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBgImagePositionX(), floatDatas[0]));
}

/**
 * @tc.name: SetandGetBgImagePositionY01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetBgImagePositionY01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetBgImagePositionY(floatDatas[1]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBgImagePositionY(), floatDatas[1]));
}

/**
 * @tc.name: SetandGetBgImagePositionY02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetBgImagePositionY02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetBgImagePositionY(floatDatas[2]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBgImagePositionY(), floatDatas[2]));
}

/**
 * @tc.name: SetandGetBgImagePositionY03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetBgImagePositionY03, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetBgImagePositionY(floatDatas[3]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBgImagePositionY(), floatDatas[3]));
}

/**
 * @tc.name: SetandGetBgImagePositionY04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetBgImagePositionY04, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetBgImagePositionY(floatDatas[4]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBgImagePositionY(), floatDatas[4]));
}

/**
 * @tc.name: SetandGetBgImagePositionY05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetBgImagePositionY05, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetBgImagePositionY(floatDatas[0]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBgImagePositionY(), floatDatas[0]));
}

} // namespace OHOS::Rosen
