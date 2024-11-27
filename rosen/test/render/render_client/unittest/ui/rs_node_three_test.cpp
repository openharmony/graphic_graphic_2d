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
 * @tc.name: SetandGetRotationVector01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetRotationVector01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    Vector4f quaternion(std::numeric_limits<float>::min(), 2.f, 3.f, 4.f);
    rsnode->SetRotation(quaternion);
}

/**
 * @tc.name: SetUIBackgroundFilter
 * @tc.desc: test results of SetUIBackgroundFilter
 * @tc.type: FUNC
 */
HWTEST_F(RSNodeOneTest, SetUIBackgroundFilter, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    Filter* filterObj = new(std::nothrow) Filter();
    std::shared_ptr<FilterBlurPara> para = std::make_shared<FilterBlurPara>();
    para->SetRadius(floatDatas[1]);
    filterObj->AddPara(para);
    rsnode->SetUIBackgroundFilter(filterObj);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetBackgroundBlurRadiusX() == floatDatas[1]);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetBackgroundBlurRadiusY() == floatDatas[1]);
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
HWTEST_F(RSNodeOneTest, SetUICompositingFilter, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    Filter* filterObj = new(std::nothrow) Filter();
    std::shared_ptr<FilterBlurPara> para = std::make_shared<FilterBlurPara>();
    para->SetRadius(floatDatas[1]);
    filterObj->AddPara(para);
    rsnode->SetUICompositingFilter(filterObj);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetForegroundBlurRadiusX() == floatDatas[1]);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetForegroundBlurRadiusY() == floatDatas[1]);
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
HWTEST_F(RSNodeOneTest, SetUIForegroundFilter, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    Filter* filterObj = new(std::nothrow) Filter();
    std::shared_ptr<FilterBlurPara> para = std::make_shared<FilterBlurPara>();
    para->SetRadius(floatDatas[1]);
    filterObj->AddPara(para);
    rsnode->SetUIForegroundFilter(filterObj);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetForegroundEffectRadius() == floatDatas[1]);
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
HWTEST_F(RSNodeOneTest, SetVisualEffect, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    VisualEffect* effectObj = new(std::nothrow) VisualEffect();
    std::shared_ptr<BrightnessBlender> blender = std::make_shared<BrightnessBlender>();
    blender->SetFraction(floatDatas[0]);
    std::shared_ptr<BackgroundColorEffectPara> para = std::make_shared<BackgroundColorEffectPara>();
    para->SetBlender(blender);
    effectObj->AddPara(para);
    rsnode->SetVisualEffect(effectObj);
    EXPECT_NE(floatDatas[0], 1.f);
    if (effectObj != nullptr) {
        delete effectObj;
        effectObj = nullptr;
    }
}

/**
 * @tc.name: SetandGetTranslateVector01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetTranslateVector01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    Vector2f quaternion(std::numeric_limits<float>::max(), 2.f);
    rsnode->SetTranslate(quaternion);
}

/**
 * @tc.name: CreateBlurFilter01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, CreateBlurFilter01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> backgroundFilter = RSFilter::CreateBlurFilter(floatDatas[0], floatDatas[1]);
    rsnode->SetBackgroundFilter(backgroundFilter);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetBackgroundBlurRadiusX() == floatDatas[0]);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetBackgroundBlurRadiusY() == floatDatas[1]);
}

/**
 * @tc.name: CreateBlurFilter02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, CreateBlurFilter02, TestSize.Level2)
{
    auto rsnode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> backgroundFilter = RSFilter::CreateBlurFilter(floatDatas[1], floatDatas[2]);
    rsnode->SetBackgroundFilter(backgroundFilter);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetBackgroundBlurRadiusX() == floatDatas[1]);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetBackgroundBlurRadiusY() == floatDatas[2]);
}

/**
 * @tc.name: CreateBlurFilter03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, CreateBlurFilter03, TestSize.Level3)
{
    auto rsnode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> backgroundFilter = RSFilter::CreateBlurFilter(floatDatas[2], floatDatas[3]);
    rsnode->SetBackgroundFilter(backgroundFilter);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetBackgroundBlurRadiusX() == floatDatas[2]);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetBackgroundBlurRadiusY() == floatDatas[3]);
}

/**
 * @tc.name: CreateBlurFilter04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, CreateBlurFilter04, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> backgroundFilter = RSFilter::CreateBlurFilter(floatDatas[3], floatDatas[4]);
    rsnode->SetBackgroundFilter(backgroundFilter);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetBackgroundBlurRadiusX() == floatDatas[3]);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetBackgroundBlurRadiusY() == floatDatas[4]);
}

/**
 * @tc.name: CreateBlurFilter05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, CreateBlurFilter05, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> backgroundFilter = RSFilter::CreateBlurFilter(floatDatas[4], floatDatas[0]);
    rsnode->SetBackgroundFilter(backgroundFilter);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetBackgroundBlurRadiusX() == floatDatas[4]);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetBackgroundBlurRadiusY() == floatDatas[0]);
}

/**
 * @tc.name: CreateNormalFilter01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, CreateNormalFilter01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(floatDatas[0], floatDatas[1]);
    rsnode->SetFilter(filter);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetForegroundBlurRadiusX() == floatDatas[0]);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetForegroundBlurRadiusY() == floatDatas[1]);
}

/**
 * @tc.name: CreateNormalFilter02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, CreateNormalFilter02, TestSize.Level2)
{
    auto rsnode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(floatDatas[1], floatDatas[2]);
    rsnode->SetFilter(filter);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetForegroundBlurRadiusX() == floatDatas[1]);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetForegroundBlurRadiusY() == floatDatas[2]);
}

/**
 * @tc.name: CreateNormalFilter03
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, CreateNormalFilter03, TestSize.Level3)
{
    auto rsnode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(floatDatas[2], floatDatas[3]);
    rsnode->SetFilter(filter);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetForegroundBlurRadiusX() == floatDatas[2]);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetForegroundBlurRadiusY() == floatDatas[3]);
}

/**
 * @tc.name: CreateNormalFilter04
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, CreateNormalFilter04, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(floatDatas[3], floatDatas[4]);
    rsnode->SetFilter(filter);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetForegroundBlurRadiusX() == floatDatas[3]);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetForegroundBlurRadiusY() == floatDatas[4]);
}

/**
 * @tc.name: CreateNormalFilter05
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, CreateNormalFilter05, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(floatDatas[4], floatDatas[0]);
    rsnode->SetFilter(filter);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetForegroundBlurRadiusX() == floatDatas[4]);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetForegroundBlurRadiusY() == floatDatas[0]);
}

/**
 * @tc.name: SetBackgroundFilter
 * @tc.desc: test results of SetBackgroundFilter
 * @tc.type: FUNC
 * @tc.require: issueI9RLG7
 */
HWTEST_F(RSNodeOneTest, SetBackgroundFilter, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> backgroundFilter = nullptr;
    rsnode->SetBackgroundFilter(backgroundFilter);
    EXPECT_TRUE(backgroundFilter == nullptr);

    backgroundFilter = std::make_shared<RSFilter>();
    backgroundFilter->type_ = RSFilter::MATERIAL;
    rsnode->SetBackgroundFilter(backgroundFilter);
    EXPECT_TRUE(backgroundFilter != nullptr);

    backgroundFilter->type_ = RSFilter::BLUR;
    rsnode->SetBackgroundFilter(backgroundFilter);
    EXPECT_TRUE(backgroundFilter != nullptr);
}

/**
 * @tc.name: SetFilter
 * @tc.desc: test results of SetFilter
 * @tc.type: FUNC
 * @tc.require: issueI9RLG7
 */
HWTEST_F(RSNodeOneTest, SetFilter, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> filter = nullptr;
    rsnode->SetFilter(filter);
    EXPECT_TRUE(filter == nullptr);

    filter = std::make_shared<RSFilter>();
    filter->type_ = RSFilter::MATERIAL;
    rsnode->SetFilter(filter);
    EXPECT_TRUE(filter != nullptr);

    filter->type_ = RSFilter::BLUR;
    rsnode->SetFilter(filter);
    EXPECT_TRUE(filter != nullptr);
}

} // namespace OHOS::Rosen
