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

class RSNodeTwoTest : public testing::Test {
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

void RSNodeTwoTest::SetUpTestCase() {}
void RSNodeTwoTest::TearDownTestCase() {}
void RSNodeTwoTest::SetUp() {}
void RSNodeTwoTest::TearDown() {}

/**
 * @tc.name: SetandGetBorderWidth01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetBorderWidth01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetBorderWidth(floatDatas[1]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBorderWidth().x_, floatDatas[1]));
}

/**
 * @tc.name: SetandGetBorderWidth02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetBorderWidth02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetBorderWidth(floatDatas[2]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBorderWidth().x_, floatDatas[2]));
}

/**
 * @tc.name: SetandGetBorderWidth03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetBorderWidth03, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetBorderWidth(floatDatas[3]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBorderWidth().x_, floatDatas[3]));
}

/**
 * @tc.name: SetandGetBorderWidth04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetBorderWidth04, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetBorderWidth(floatDatas[4]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBorderWidth().x_, floatDatas[4]));
}

/**
 * @tc.name: SetandGetBorderWidth05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetBorderWidth05, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetBorderWidth(floatDatas[0]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBorderWidth().x_, floatDatas[0]));
}

/**
 * @tc.name: SetandGetBorderDashParams01
 * @tc.desc: Check for zero values
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetBorderDashParams01, TestSize.Level1)
{
    SetBorderDashParamsAndTest(FLOAT_DATA_ZERO);
}

/**
 * @tc.name: SetandGetBorderDashParams02
 * @tc.desc: Check for positive values
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetBorderDashParams02, TestSize.Level1)
{
    SetBorderDashParamsAndTest(FLOAT_DATA_POSITIVE);
}

/**
 * @tc.name: SetandGetBorderDashParams03
 * @tc.desc: Check for negative values
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetBorderDashParams03, TestSize.Level1)
{
    SetBorderDashParamsAndTest(FLOAT_DATA_NEGATIVE);
}

/**
 * @tc.name: SetandGetBorderDashParams04
 * @tc.desc: Check for max values
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetBorderDashParams04, TestSize.Level1)
{
    SetBorderDashParamsAndTest(FLOAT_DATA_MAX);
}

/**
 * @tc.name: SetandGetBorderDashParams05
 * @tc.desc: Check for min values
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetBorderDashParams05, TestSize.Level1)
{
    SetBorderDashParamsAndTest(FLOAT_DATA_MIN);
}

/**
 * @tc.name: SetandGetOutlineWidth01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetOutlineWidth01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetOutlineWidth(floatDatas[1]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetOutlineWidth().x_, floatDatas[1]));
}

/**
 * @tc.name: SetandGetOutlineWidth02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetOutlineWidth02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetOutlineWidth(floatDatas[2]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetOutlineWidth().x_, floatDatas[2]));
}

/**
 * @tc.name: SetandGetOutlineWidth03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetOutlineWidth03, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetOutlineWidth(floatDatas[3]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetOutlineWidth().x_, floatDatas[3]));
}

/**
 * @tc.name: SetandGetOutlineWidth04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetOutlineWidth04, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetOutlineWidth(floatDatas[4]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetOutlineWidth().x_, floatDatas[4]));
}

/**
 * @tc.name: SetandGetOutlineWidth05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetOutlineWidth05, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetOutlineWidth(floatDatas[0]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetOutlineWidth().x_, floatDatas[0]));
}

/**
 * @tc.name: SetandGetOutlineDashParams01
 * @tc.desc: Check for zero values
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetOutlineDashParams01, TestSize.Level1)
{
    SetOutlineDashParamsAndTest(FLOAT_DATA_ZERO);
}

/**
 * @tc.name: SetandGetOutlineDashParams02
 * @tc.desc: Check for positive values
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetOutlineDashParams02, TestSize.Level1)
{
    SetOutlineDashParamsAndTest(FLOAT_DATA_POSITIVE);
}

/**
 * @tc.name: SetandGetOutlineDashParams03
 * @tc.desc: Check for negative values
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetOutlineDashParams03, TestSize.Level1)
{
    SetOutlineDashParamsAndTest(FLOAT_DATA_NEGATIVE);
}

/**
 * @tc.name: SetandGetOutlineDashParams04
 * @tc.desc: Check for max values
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetOutlineDashParams04, TestSize.Level1)
{
    SetOutlineDashParamsAndTest(FLOAT_DATA_MAX);
}

/**
 * @tc.name: SetandGetOutlineDashParams05
 * @tc.desc: Check for min values
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetOutlineDashParams05, TestSize.Level1)
{
    SetOutlineDashParamsAndTest(FLOAT_DATA_MIN);
}

/**
 * @tc.name: SetandGetPivot01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetPivot01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetPivot(floatDatas[0], floatDatas[1]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetPivot().x_, floatDatas[0]));
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetPivot().y_, floatDatas[1]));
}

/**
 * @tc.name: SetandGetPivot02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetPivot02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetPivot(floatDatas[3], floatDatas[2]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetPivot().x_, floatDatas[3]));
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetPivot().y_, floatDatas[2]));
}

/**
 * @tc.name: SetandGetPivot03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetPivot03, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetPivot(floatDatas[1], floatDatas[3]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetPivot().x_, floatDatas[1]));
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetPivot().y_, floatDatas[3]));
}

/**
 * @tc.name: SetandGetPivotX01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetPivotX01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetPivotX(floatDatas[1]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetPivot().x_, floatDatas[1]));
}

/**
 * @tc.name: SetandGetPivotX02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetPivotX02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetPivotX(floatDatas[2]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetPivot().x_, floatDatas[2]));
}

/**
 * @tc.name: SetandGetPivotX03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetPivotX03, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetPivotX(floatDatas[3]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetPivot().x_, floatDatas[3]));
}

/**
 * @tc.name: SetandGetPivotX04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetPivotX04, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetPivotX(floatDatas[4]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetPivot().x_, floatDatas[4]));
}

/**
 * @tc.name: SetandGetPivotX05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetPivotX05, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetPivotX(floatDatas[0]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetPivot().x_, floatDatas[0]));
}

/**
 * @tc.name: SetandGetPivotY01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetPivotY01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetPivotY(floatDatas[1]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetPivot().y_, floatDatas[1]));
}

/**
 * @tc.name: SetandGetPivotY02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetPivotY02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetPivotY(floatDatas[2]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetPivot().y_, floatDatas[2]));
}

/**
 * @tc.name: SetandGetPivotY03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetPivotY03, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetPivotY(floatDatas[3]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetPivot().y_, floatDatas[3]));
}

/**
 * @tc.name: SetandGetPivotY04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetPivotY04, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetPivotY(floatDatas[4]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetPivot().y_, floatDatas[4]));
}

/**
 * @tc.name: SetandGetPivotY05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetPivotY05, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetPivotY(floatDatas[0]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetPivot().y_, floatDatas[0]));
}

/**
 * @tc.name: SetandGetShadowOffset01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetShadowOffset01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetShadowOffset(floatDatas[2], floatDatas[3]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetShadowOffsetX(), floatDatas[2]));
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetShadowOffsetY(), floatDatas[3]));
}

/**
 * @tc.name: SetandGetShadowOffset02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetShadowOffset02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetShadowOffset(floatDatas[2], floatDatas[0]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetShadowOffsetX(), floatDatas[2]));
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetShadowOffsetY(), floatDatas[0]));
}

/**
 * @tc.name: SetandGetShadowOffset03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetShadowOffset03, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetShadowOffset(floatDatas[1], floatDatas[3]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetShadowOffsetX(), floatDatas[1]));
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetShadowOffsetY(), floatDatas[3]));
}
/**
 * @tc.name: SetandGetShadowOffsetX01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetShadowOffsetX01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetShadowOffsetX(floatDatas[1]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetShadowOffsetX(), floatDatas[1]));
}

/**
 * @tc.name: SetandGetShadowOffsetX02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetShadowOffsetX02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetShadowOffsetX(floatDatas[2]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetShadowOffsetX(), floatDatas[2]));
}

/**
 * @tc.name: SetandGetShadowOffsetX03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetShadowOffsetX03, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetShadowOffsetX(floatDatas[3]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetShadowOffsetX(), floatDatas[3]));
}

/**
 * @tc.name: SetandGetShadowOffsetX04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetShadowOffsetX04, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetShadowOffsetX(floatDatas[4]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetShadowOffsetX(), floatDatas[4]));
}

/**
 * @tc.name: SetandGetShadowOffsetX05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetShadowOffsetX05, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetShadowOffsetX(floatDatas[0]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetShadowOffsetX(), floatDatas[0]));
}

/**
 * @tc.name: SetandGetShadowOffsetY01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetShadowOffsetY01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetShadowOffsetY(floatDatas[1]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetShadowOffsetY(), floatDatas[1]));
}

/**
 * @tc.name: SetandGetShadowOffsetY02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetShadowOffsetY02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetShadowOffsetY(floatDatas[2]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetShadowOffsetY(), floatDatas[2]));
}

/**
 * @tc.name: SetandGetShadowOffsetY03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetShadowOffsetY03, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetShadowOffsetY(floatDatas[3]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetShadowOffsetY(), floatDatas[3]));
}

/**
 * @tc.name: SetandGetShadowOffsetY04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetShadowOffsetY04, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetShadowOffsetY(floatDatas[4]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetShadowOffsetY(), floatDatas[4]));
}

/**
 * @tc.name: SetandGetShadowOffsetY05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetShadowOffsetY05, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetShadowOffsetY(floatDatas[0]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetShadowOffsetY(), floatDatas[0]));
}

/**
 * @tc.name: SetandGetShadowAlpha01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetShadowAlpha01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetShadowAlpha(floatDatas[1]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetShadowAlpha(), floatDatas[1], 0.02f));
}

/**
 * @tc.name: SetandGetShadowAlpha02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetShadowAlpha02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetShadowAlpha(floatDatas[2]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetShadowAlpha(), floatDatas[2], 0.02f));
}

/**
 * @tc.name: SetandGetShadowAlpha03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetShadowAlpha03, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetShadowAlpha(floatDatas[3]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetShadowAlpha(), floatDatas[3], 0.02f));
}

/**
 * @tc.name: SetandGetShadowAlpha04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetShadowAlpha04, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetShadowAlpha(floatDatas[4]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetShadowAlpha(), floatDatas[4], 0.02f));
}

/**
 * @tc.name: SetandGetShadowAlpha05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetShadowAlpha05, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetShadowAlpha(floatDatas[0]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetShadowAlpha(),
        std::clamp(floatDatas[0], 0.0f, 1.0f), 0.02f));
}

/**
 * @tc.name: SetandGetShadowElevation01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetShadowElevation01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetShadowElevation(floatDatas[1]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetShadowElevation(), floatDatas[1]));
}

/**
 * @tc.name: SetandGetShadowElevation02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetShadowElevation02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetShadowElevation(floatDatas[2]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetShadowElevation(), floatDatas[2]));
}

/**
 * @tc.name: SetandGetShadowElevation03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetShadowElevation03, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetShadowElevation(floatDatas[3]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetShadowElevation(), floatDatas[3]));
}

/**
 * @tc.name: SetandGetShadowElevation04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetShadowElevation04, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetShadowElevation(floatDatas[4]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetShadowElevation(), floatDatas[4]));
}

/**
 * @tc.name: SetandGetShadowElevation05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetShadowElevation05, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetShadowElevation(floatDatas[0]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetShadowElevation(), floatDatas[0]));
}

/**
 * @tc.name: SetandGetShadowRadius01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetShadowRadius01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetShadowRadius(floatDatas[1]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetShadowRadius(), floatDatas[1]));
}

/**
 * @tc.name: SetandGetShadowRadius02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetShadowRadius02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetShadowRadius(floatDatas[2]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetShadowRadius(), floatDatas[2]));
}

/**
 * @tc.name: SetandGetShadowRadius03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetShadowRadius03, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetShadowRadius(floatDatas[3]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetShadowRadius(), floatDatas[3]));
}

/**
 * @tc.name: SetandGetShadowRadius04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetShadowRadius04, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetShadowRadius(floatDatas[4]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetShadowRadius(), floatDatas[4]));
}

/**
 * @tc.name: SetandGetShadowRadius05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetShadowRadius05, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetShadowRadius(floatDatas[0]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetShadowRadius(), floatDatas[0]));
}

/**
 * @tc.name: SetandGetShadowColor01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetShadowColor01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = 0x034123;
    rsnode->SetShadowColor(colorValue);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetShadowColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetShadowColor02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetShadowColor02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = 0x45ba87;
    rsnode->SetShadowColor(colorValue);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetShadowColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetShadowColor03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetShadowColor03, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = 0x32aadd;
    rsnode->SetShadowColor(colorValue);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetShadowColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetShadowColor04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetShadowColor04, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::max();
    rsnode->SetShadowColor(colorValue);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetShadowColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetShadowColor05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetShadowColor05, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::min();
    rsnode->SetShadowColor(colorValue);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetShadowColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetShadowColorStrategy01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetShadowColorStrategy01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    int shadowColorStrategy = SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_NONE;
    rsnode->SetShadowColorStrategy(shadowColorStrategy);
    ASSERT_EQ(rsnode->GetStagingProperties().GetShadowColorStrategy(), shadowColorStrategy);
}

/**
 * @tc.name: SetandGetShadowColorStrategy02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetShadowColorStrategy02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    int shadowColorStrategy = SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_AVERAGE;
    rsnode->SetShadowColorStrategy(shadowColorStrategy);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetShadowColorStrategy(), shadowColorStrategy));
}

/**
 * @tc.name: SetandGetShadowColorStrategy03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetShadowColorStrategy03, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    int shadowColorStrategy = SHADOW_COLOR_STRATEGY::COLOR_STRATEGY_MAIN;
    rsnode->SetShadowColorStrategy(shadowColorStrategy);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetShadowColorStrategy(), shadowColorStrategy));
}

/**
 * @tc.name: SetandGetTranslateThree01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetTranslateThree01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetTranslate(floatDatas[1], floatDatas[2], floatDatas[3]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetTranslate().x_, floatDatas[1]));
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetTranslate().y_, floatDatas[2]));
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetTranslateZ(), floatDatas[3]));
}

/**
 * @tc.name: SetandGetTranslateThree02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetTranslateThree02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetTranslate(floatDatas[0], floatDatas[0], floatDatas[2]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetTranslate().x_, floatDatas[0]));
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetTranslate().y_, floatDatas[0]));
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetTranslateZ(), floatDatas[2]));
}

/**
 * @tc.name: SetandGetTranslateThree03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetTranslateThree03, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetTranslate(floatDatas[1], floatDatas[3], floatDatas[1]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetTranslate().x_, floatDatas[1]));
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetTranslate().y_, floatDatas[3]));
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetTranslateZ(), floatDatas[1]));
}

/**
 * @tc.name: SetandGetTranslateX01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetTranslateX01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetTranslateX(floatDatas[1]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetTranslate().x_, floatDatas[1]));
}

/**
 * @tc.name: SetandGetTranslateX02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetTranslateX02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetTranslateX(floatDatas[2]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetTranslate().x_, floatDatas[2]));
}

/**
 * @tc.name: SetandGetTranslateX03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetTranslateX03, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetTranslateX(floatDatas[3]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetTranslate().x_, floatDatas[3]));
}

/**
 * @tc.name: SetandGetTranslateX04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetTranslateX04, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetTranslateX(floatDatas[4]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetTranslate().x_, floatDatas[4]));
}

/**
 * @tc.name: SetandGetTranslateX05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetTranslateX05, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetTranslateX(floatDatas[0]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetTranslate().x_, floatDatas[0]));
}

/**
 * @tc.name: SetandGetTranslateY01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetTranslateY01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetTranslateY(floatDatas[1]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetTranslate().y_, floatDatas[1]));
}

/**
 * @tc.name: SetandGetTranslateY02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetTranslateY02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetTranslateY(floatDatas[2]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetTranslate().y_, floatDatas[2]));
}

/**
 * @tc.name: SetandGetTranslateY03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetTranslateY03, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetTranslateY(floatDatas[3]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetTranslate().y_, floatDatas[3]));
}

/**
 * @tc.name: SetandGetTranslateY04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetTranslateY04, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetTranslateY(floatDatas[4]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetTranslate().y_, floatDatas[4]));
}

/**
 * @tc.name: SetandGetTranslateY05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetTranslateY05, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetTranslateY(floatDatas[0]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetTranslate().y_, floatDatas[0]));
}

/**
 * @tc.name: SetandGetTranslateZ01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetTranslateZ01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetTranslateZ(floatDatas[1]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetTranslateZ(), floatDatas[1]));
}

/**
 * @tc.name: SetandGetTranslateZ02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetTranslateZ02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetTranslateZ(floatDatas[2]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetTranslateZ(), floatDatas[2]));
}

/**
 * @tc.name: SetandGetTranslateZ03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetTranslateZ03, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetTranslateZ(floatDatas[3]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetTranslateZ(), floatDatas[3]));
}

/**
 * @tc.name: SetandGetTranslateZ04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetTranslateZ04, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetTranslateZ(floatDatas[4]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetTranslateZ(), floatDatas[4]));
}

/**
 * @tc.name: SetandGetTranslateZ05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetTranslateZ05, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetTranslateZ(floatDatas[0]);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetTranslateZ(), floatDatas[0]));
}

/**
 * @tc.name: SetandGetClipToBounds01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetClipToBounds01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetClipToBounds(true);
    ASSERT_TRUE(rsnode != nullptr);
}

/**
 * @tc.name: SetandGetClipToBounds02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetClipToBounds02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetClipToBounds(false);
    ASSERT_TRUE(rsnode != nullptr);
}

/**
 * @tc.name: SetandGetClipToFrame01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetClipToFrame01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetClipToFrame(true);
    ASSERT_TRUE(rsnode != nullptr);
}

/**
 * @tc.name: SetandGetClipToFrame02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetClipToFrame02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetClipToFrame(false);
    ASSERT_TRUE(rsnode != nullptr);
}

/**
 * @tc.name: SetBoundsWidth
 * @tc.desc: test results of SetBoundsWidth
 * @tc.type: FUNC
 * @tc.require: issueI9RLG7
 */
HWTEST_F(RSNodeTwoTest, SetBoundsWidth, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetBoundsWidth(1.f);
    EXPECT_TRUE(!rsnode->propertyModifiers_.empty());

    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSPropertyBase>();
    RSModifierType modifierType = RSModifierType::BOUNDS;
    std::shared_ptr<RSModifier> modifier = std::make_shared<RSBackgroundShaderModifier>(property);
    modifier->property_ = nullptr;
    rsnode->propertyModifiers_[modifierType] = modifier;
    rsnode->SetBoundsWidth(1.f);
    EXPECT_TRUE(!rsnode->propertyModifiers_.empty());
}

/**
 * @tc.name: SetBoundsHeight
 * @tc.desc: test results of SetBoundsHeight
 * @tc.type: FUNC
 * @tc.require: issueI9RLG7
 */
HWTEST_F(RSNodeTwoTest, SetBoundsHeight, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetBoundsHeight(1.f);
    EXPECT_TRUE(!rsnode->propertyModifiers_.empty());

    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSPropertyBase>();
    RSModifierType modifierType = RSModifierType::BOUNDS;
    std::shared_ptr<RSModifier> modifier = std::make_shared<RSBackgroundShaderModifier>(property);
    modifier->property_ = nullptr;
    rsnode->propertyModifiers_[modifierType] = modifier;
    rsnode->SetBoundsHeight(1.f);
    EXPECT_TRUE(!rsnode->propertyModifiers_.empty());
}

/**
 * @tc.name: SetFramePositionX
 * @tc.desc: test results of SetFramePositionX
 * @tc.type: FUNC
 * @tc.require: issueI9RLG7
 */
HWTEST_F(RSNodeTwoTest, SetFramePositionX, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetFramePositionX(1.f);
    EXPECT_TRUE(!rsnode->propertyModifiers_.empty());

    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSPropertyBase>();
    RSModifierType modifierType = RSModifierType::FRAME;
    std::shared_ptr<RSModifier> modifier = std::make_shared<RSBackgroundShaderModifier>(property);
    modifier->property_ = nullptr;
    rsnode->propertyModifiers_[modifierType] = modifier;
    rsnode->SetFramePositionX(1.f);
    EXPECT_TRUE(!rsnode->propertyModifiers_.empty());
}

/**
 * @tc.name: SetFramePositionY
 * @tc.desc: test results of SetFramePositionY
 * @tc.type: FUNC
 * @tc.require: issueI9RLG7
 */
HWTEST_F(RSNodeTwoTest, SetFramePositionY, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetFramePositionY(1.f);
    EXPECT_TRUE(!rsnode->propertyModifiers_.empty());

    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSPropertyBase>();
    RSModifierType modifierType = RSModifierType::FRAME;
    std::shared_ptr<RSModifier> modifier = std::make_shared<RSBackgroundShaderModifier>(property);
    modifier->property_ = nullptr;
    rsnode->propertyModifiers_[modifierType] = modifier;
    rsnode->SetFramePositionY(1.f);
    EXPECT_TRUE(!rsnode->propertyModifiers_.empty());
}

/**
 * @tc.name: SetPivotX
 * @tc.desc: test results of SetPivotX
 * @tc.type: FUNC
 * @tc.require: issueI9RLG7
 */
HWTEST_F(RSNodeTwoTest, SetPivotX, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetPivotX(1.f);
    EXPECT_TRUE(!rsnode->propertyModifiers_.empty());

    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSPropertyBase>();
    RSModifierType modifierType = RSModifierType::PIVOT;
    std::shared_ptr<RSModifier> modifier = std::make_shared<RSBackgroundShaderModifier>(property);
    modifier->property_ = nullptr;
    rsnode->propertyModifiers_[modifierType] = modifier;
    rsnode->SetPivotX(1.f);
    EXPECT_TRUE(!rsnode->propertyModifiers_.empty());
}

/**
 * @tc.name: SetPivotY
 * @tc.desc: test results of SetPivotY
 * @tc.type: FUNC
 * @tc.require: issueI9RLG7
 */
HWTEST_F(RSNodeTwoTest, SetPivotY, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetPivotY(1.f);
    EXPECT_TRUE(!rsnode->propertyModifiers_.empty());

    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSPropertyBase>();
    RSModifierType modifierType = RSModifierType::PIVOT;
    std::shared_ptr<RSModifier> modifier = std::make_shared<RSBackgroundShaderModifier>(property);
    modifier->property_ = nullptr;
    rsnode->propertyModifiers_[modifierType] = modifier;
    rsnode->SetPivotY(1.f);
    EXPECT_TRUE(!rsnode->propertyModifiers_.empty());
}

/**
 * @tc.name: SetTranslateX
 * @tc.desc: test results of SetTranslateX
 * @tc.type: FUNC
 * @tc.require: issueI9RLG7
 */
HWTEST_F(RSNodeTwoTest, SetTranslateX, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetTranslateX(1.f);
    EXPECT_TRUE(!rsnode->propertyModifiers_.empty());

    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSPropertyBase>();
    RSModifierType modifierType = RSModifierType::TRANSLATE;
    std::shared_ptr<RSModifier> modifier = std::make_shared<RSBackgroundShaderModifier>(property);
    modifier->property_ = nullptr;
    rsnode->propertyModifiers_[modifierType] = modifier;
    rsnode->SetTranslateX(1.f);
    EXPECT_TRUE(!rsnode->propertyModifiers_.empty());
}

/**
 * @tc.name: SetTranslateY
 * @tc.desc: test results of SetTranslateY
 * @tc.type: FUNC
 * @tc.require: issueI9RLG7
 */
HWTEST_F(RSNodeTwoTest, SetTranslateY, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetTranslateY(1.f);
    EXPECT_TRUE(!rsnode->propertyModifiers_.empty());

    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSPropertyBase>();
    RSModifierType modifierType = RSModifierType::TRANSLATE;
    std::shared_ptr<RSModifier> modifier = std::make_shared<RSBackgroundShaderModifier>(property);
    modifier->property_ = nullptr;
    rsnode->propertyModifiers_[modifierType] = modifier;
    rsnode->SetTranslateY(1.f);
    EXPECT_TRUE(!rsnode->propertyModifiers_.empty());
}

/**
 * @tc.name: SetScaleX
 * @tc.desc: test results of SetScaleX
 * @tc.type: FUNC
 * @tc.require: issueI9RLG7
 */
HWTEST_F(RSNodeTwoTest, SetScaleX, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetScaleX(1.f);
    EXPECT_TRUE(!rsnode->propertyModifiers_.empty());

    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSPropertyBase>();
    RSModifierType modifierType = RSModifierType::SCALE;
    std::shared_ptr<RSModifier> modifier = std::make_shared<RSBackgroundShaderModifier>(property);
    modifier->property_ = nullptr;
    rsnode->propertyModifiers_[modifierType] = modifier;
    rsnode->SetScaleX(1.f);
    EXPECT_TRUE(!rsnode->propertyModifiers_.empty());
}

/**
 * @tc.name: SetScaleY
 * @tc.desc: test results of SetScaleY
 * @tc.type: FUNC
 * @tc.require: issueI9RLG7
 */
HWTEST_F(RSNodeTwoTest, SetScaleY, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetScaleY(1.f);
    EXPECT_TRUE(!rsnode->propertyModifiers_.empty());

    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSPropertyBase>();
    RSModifierType modifierType = RSModifierType::SCALE;
    std::shared_ptr<RSModifier> modifier = std::make_shared<RSBackgroundShaderModifier>(property);
    modifier->property_ = nullptr;
    rsnode->propertyModifiers_[modifierType] = modifier;
    rsnode->SetScaleY(1.f);
    EXPECT_TRUE(!rsnode->propertyModifiers_.empty());
}

/**
 * @tc.name: SetSkewX
 * @tc.desc: test results of SetSkewX
 * @tc.type: FUNC
 * @tc.require: issueI9RLG7
 */
HWTEST_F(RSNodeTwoTest, SetSkewX, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetSkewX(1.f);
    EXPECT_TRUE(!rsnode->propertyModifiers_.empty());

    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSPropertyBase>();
    RSModifierType modifierType = RSModifierType::SKEW;
    std::shared_ptr<RSModifier> modifier = std::make_shared<RSBackgroundShaderModifier>(property);
    modifier->property_ = nullptr;
    rsnode->propertyModifiers_[modifierType] = modifier;
    rsnode->SetSkewX(1.f);
    EXPECT_TRUE(!rsnode->propertyModifiers_.empty());
}

/**
 * @tc.name: SetSkewY
 * @tc.desc: test results of SetSkewY
 * @tc.type: FUNC
 * @tc.require: issueI9RLG7
 */
HWTEST_F(RSNodeTwoTest, SetSkewY, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetSkewY(1.f);
    EXPECT_TRUE(!rsnode->propertyModifiers_.empty());

    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSPropertyBase>();
    RSModifierType modifierType = RSModifierType::SKEW;
    std::shared_ptr<RSModifier> modifier = std::make_shared<RSBackgroundShaderModifier>(property);
    modifier->property_ = nullptr;
    rsnode->propertyModifiers_[modifierType] = modifier;
    rsnode->SetSkewY(1.f);
    EXPECT_TRUE(!rsnode->propertyModifiers_.empty());
}

/**
 * @tc.name: SetPerspX
 * @tc.desc: test results of SetPerspX
 * @tc.type: FUNC
 * @tc.require: issueI9RLG7
 */
HWTEST_F(RSNodeTwoTest, SetPerspX, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetPerspX(1.f);
    EXPECT_TRUE(!rsnode->propertyModifiers_.empty());

    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSPropertyBase>();
    RSModifierType modifierType = RSModifierType::PERSP;
    std::shared_ptr<RSModifier> modifier = std::make_shared<RSBackgroundShaderModifier>(property);
    modifier->property_ = nullptr;
    rsnode->propertyModifiers_[modifierType] = modifier;
    rsnode->SetPerspX(1.f);
    EXPECT_TRUE(!rsnode->propertyModifiers_.empty());
}

/**
 * @tc.name: SetPerspY
 * @tc.desc: test results of SetPerspY
 * @tc.type: FUNC
 * @tc.require: issueI9RLG7
 */
HWTEST_F(RSNodeTwoTest, SetPerspY, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetPerspY(1.f);
    EXPECT_TRUE(!rsnode->propertyModifiers_.empty());

    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSPropertyBase>();
    RSModifierType modifierType = RSModifierType::PERSP;
    std::shared_ptr<RSModifier> modifier = std::make_shared<RSBackgroundShaderModifier>(property);
    modifier->property_ = nullptr;
    rsnode->propertyModifiers_[modifierType] = modifier;
    rsnode->SetPerspY(1.f);
    EXPECT_TRUE(!rsnode->propertyModifiers_.empty());
}

/**
 * @tc.name: SetandGetVisible01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetVisible01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetVisible(true);

    rsnode->transitionEffect_ = std::make_shared<RSTransitionEffect>();
    rsnode->SetVisible(false);

    rsnode->SetVisible(true);
    ASSERT_TRUE(rsnode != nullptr);
}

/**
 * @tc.name: SetandGetVisible02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetVisible02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetVisible(false);
    ASSERT_TRUE(rsnode != nullptr);
}

/**
 * @tc.name: SetandGetBorderStyle01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetBorderStyle01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    uint32_t borderStyle = static_cast<uint32_t>(BorderStyle::SOLID);
    rsnode->SetBorderStyle(borderStyle);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetBorderStyle().x_ == borderStyle);
}

/**
 * @tc.name: SetandGetBorderStyle02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetBorderStyle02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    uint32_t borderStyle = static_cast<uint32_t>(BorderStyle::DASHED);
    rsnode->SetBorderStyle(borderStyle);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetBorderStyle().x_ == borderStyle);
}

/**
 * @tc.name: SetandGetBorderStyle03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetBorderStyle03, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    uint32_t borderStyle = static_cast<uint32_t>(BorderStyle::DOTTED);
    rsnode->SetBorderStyle(borderStyle);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetBorderStyle().x_ == borderStyle);
}

/**
 * @tc.name: SetandGetBorderStyle04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetBorderStyle04, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    uint32_t borderStyle = static_cast<uint32_t>(BorderStyle::NONE);
    rsnode->SetBorderStyle(borderStyle);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetBorderStyle().x_ == borderStyle);
}

/**
 * @tc.name: SetandGetOutlineStyle01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetOutlineStyle01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    uint32_t borderStyle = static_cast<uint32_t>(BorderStyle::SOLID);
    rsnode->SetOutlineStyle(BorderStyle::SOLID);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetOutlineStyle().x_ == borderStyle);
}

/**
 * @tc.name: SetandGetOutlineStyle02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetOutlineStyle02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    uint32_t borderStyle = static_cast<uint32_t>(BorderStyle::DASHED);
    rsnode->SetOutlineStyle(BorderStyle::DASHED);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetOutlineStyle().x_ == borderStyle);
}

/**
 * @tc.name: SetandGetOutlineStyle03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetOutlineStyle03, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    uint32_t borderStyle = static_cast<uint32_t>(BorderStyle::DOTTED);
    rsnode->SetOutlineStyle(BorderStyle::DOTTED);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetOutlineStyle().x_ == borderStyle);
}

/**
 * @tc.name: SetandGetOutlineStyle04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetOutlineStyle04, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    uint32_t borderStyle = static_cast<uint32_t>(BorderStyle::NONE);
    rsnode->SetOutlineStyle(BorderStyle::NONE);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetOutlineStyle().x_ == borderStyle);
}

/**
 * @tc.name: SetandGetFrameGravity01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetFrameGravity01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    Gravity gravity = Gravity::CENTER;
    rsnode->SetFrameGravity(gravity);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetFrameGravity02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    Gravity gravity = Gravity::TOP;
    rsnode->SetFrameGravity(gravity);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetFrameGravity03, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    Gravity gravity = Gravity::BOTTOM;
    rsnode->SetFrameGravity(gravity);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetFrameGravity04, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    Gravity gravity = Gravity::LEFT;
    rsnode->SetFrameGravity(gravity);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetFrameGravity05, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    Gravity gravity = Gravity::RIGHT;
    rsnode->SetFrameGravity(gravity);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity06
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetFrameGravity06, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    Gravity gravity = Gravity::TOP_LEFT;
    rsnode->SetFrameGravity(gravity);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity07
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetFrameGravity07, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    Gravity gravity = Gravity::TOP_RIGHT;
    rsnode->SetFrameGravity(gravity);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity08
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetFrameGravity08, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    Gravity gravity = Gravity::BOTTOM_LEFT;
    rsnode->SetFrameGravity(gravity);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity09
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetFrameGravity09, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    Gravity gravity = Gravity::BOTTOM_RIGHT;
    rsnode->SetFrameGravity(gravity);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity010
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetFrameGravity010, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    Gravity gravity = Gravity::RESIZE;
    rsnode->SetFrameGravity(gravity);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity011
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetFrameGravity011, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    Gravity gravity = Gravity::RESIZE_ASPECT;
    rsnode->SetFrameGravity(gravity);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity012
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetFrameGravity012, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    Gravity gravity = Gravity::RESIZE_ASPECT_FILL;
    rsnode->SetFrameGravity(gravity);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetForegroundColor01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetForegroundColor01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = 0x034123;
    rsnode->SetForegroundColor(colorValue);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetForegroundColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetForegroundColor02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetForegroundColor02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::max();
    rsnode->SetForegroundColor(colorValue);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetForegroundColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetForegroundColor03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetForegroundColor03, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::min();
    rsnode->SetForegroundColor(colorValue);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetForegroundColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetBackgroundColor01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetBackgroundColor01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = 0x034123;
    rsnode->SetBackgroundColor(colorValue);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetBackgroundColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetBackgroundColor02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetBackgroundColor02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::max();
    rsnode->SetBackgroundColor(colorValue);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetBackgroundColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetBackgroundColor03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetBackgroundColor03, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::min();
    rsnode->SetBackgroundColor(colorValue);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetBackgroundColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetBorderColor01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetBorderColor01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = 0x034123;
    rsnode->SetBorderColor(colorValue);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetBorderColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetBorderColor02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetBorderColor02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::max();
    rsnode->SetBorderColor(colorValue);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetBorderColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetBorderColor03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetBorderColor03, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::min();
    rsnode->SetBorderColor(colorValue);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetBorderColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetOutlineColor01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetOutlineColor01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = 0x034123;
    rsnode->SetOutlineColor(Color::FromArgbInt(colorValue));
    EXPECT_TRUE(rsnode->GetStagingProperties().GetOutlineColor().x_ == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetOutlineColor02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetOutlineColor02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::max();
    rsnode->SetOutlineColor(Color::FromArgbInt(colorValue));
    EXPECT_TRUE(rsnode->GetStagingProperties().GetOutlineColor().x_ == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetOutlineColor03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeTwoTest, SetandGetOutlineColor03, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::min();
    rsnode->SetOutlineColor(Color::FromArgbInt(colorValue));
    EXPECT_TRUE(rsnode->GetStagingProperties().GetOutlineColor().x_ == Color::FromArgbInt(colorValue));
}
} // namespace OHOS::Rosen
