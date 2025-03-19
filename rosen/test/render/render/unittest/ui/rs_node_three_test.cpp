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
    ASSERT_TRUE(rsnode != nullptr);
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
    ASSERT_TRUE(rsnode != nullptr);
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

/**
 * @tc.name: SetandGetClipBounds01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetClipBounds01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    ASSERT_TRUE(rsnode != nullptr);
    std::shared_ptr<RSPath> clipPath = RSPath::CreateRSPath();
    rsnode->SetClipBounds(clipPath);
    EXPECT_TRUE(rsnode->GetStagingProperties().GetClipBounds() == clipPath);
}

/**
 * @tc.name: GetId01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, GetId01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    ASSERT_TRUE(rsnode != nullptr);
    rsnode->GetId();
}

/**
 * @tc.name: GetChildren01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, GetChildren01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    ASSERT_TRUE(rsnode != nullptr);
    auto c = rsnode->GetChildren();
}

/**
 * @tc.name: GetChildren02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, GetChildren02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    ASSERT_TRUE(rsnode != nullptr);
    const auto c = rsnode->GetChildren();
}

/**
 * @tc.name: GetStagingProperties01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, GetStagingProperties01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    ASSERT_TRUE(rsnode != nullptr);
    rsnode->GetStagingProperties();
}

/**
 * @tc.name: GetMotionPathOption01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, GetMotionPathOption02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    ASSERT_TRUE(rsnode != nullptr);
    rsnode->GetMotionPathOption();
}

/**
 * @tc.name: SetBgImage01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetBgImage01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    ASSERT_TRUE(rsnode != nullptr);
    auto image = std::make_shared<RSImage>();
    rsnode->SetBgImage(image);
}

/**
 * @tc.name: SetBackgroundShader01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetBackgroundShader01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    ASSERT_TRUE(rsnode != nullptr);
    auto shader = RSShader::CreateRSShader();
    rsnode->SetBackgroundShader(shader);
}

/**
 * @tc.name: SetandGetGreyCoef01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetGreyCoef01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    ASSERT_TRUE(rsnode != nullptr);
    Vector2f greyCoef = { 0.5, 0.5 };
    rsnode->SetGreyCoef(greyCoef);
}

/**
 * @tc.name: SetandGetAiInvertTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetAiInvertTest, TestSize.Level1)
{
    auto rootNode = RSCanvasNode::Create();
    ASSERT_TRUE(rsnode != nullptr);
    auto value = Vector4f(10.f);
    rootNode->SetAiInvert(value);
}

/**
 * @tc.name: SetandGetSpherizeDegree01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetSpherizeDegree01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    float spherizeDegree = 1.0f;
    rsnode->SetSpherizeDegree(spherizeDegree);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetSpherizeDegree(), spherizeDegree));
}

/**
 * @tc.name: SetAttractionEffect
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetAttractionEffect, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    float attractionFraction = 1.0f;
    Vector2f attractionDstPoint = { 10.0, 10.0 };
    rsnode->SetAttractionEffect(attractionFraction, attractionDstPoint);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetAttractionFractionValue(), attractionFraction));
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetAttractionDstPointValue(), attractionDstPoint));
}

/**
 * @tc.name: SetandGetSetAttractionEffectDstPoint01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetSetAttractionEffectDstPoint01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    Vector2f attractionDstPoint = { 10.0, 10.0 };
    rsnode->SetAttractionEffectDstPoint(attractionDstPoint);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetAttractionDstPointValue(), attractionDstPoint));
}

/**
 * @tc.name: SetandGetLightUpEffectDegree01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetLightUpEffectDegree01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    float lightUpEffectDegree = 1.0f;
    rsnode->SetLightUpEffectDegree(lightUpEffectDegree);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetLightUpEffectDegree(), lightUpEffectDegree));
}

/**
 * @tc.name: SetandGetShadowIsFilled01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetShadowIsFilled01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetShadowIsFilled(true);
    ASSERT_TRUE(rsnode != nullptr);
}

/**
 * @tc.name: SetandGetShadowIsFilled02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetShadowIsFilled02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    ASSERT_TRUE(rsnode != nullptr);
    rsnode->SetShadowIsFilled(false);
}

/**
 * @tc.name: SetandGetForegroundEffectRadius01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetForegroundEffectRadius01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    float radius = 10.0f;
    rsnode->SetForegroundEffectRadius(radius);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetForegroundEffectRadius(), radius));
}

/**
 * @tc.name: SetCompositingFilter01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetCompositingFilter01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    ASSERT_TRUE(rsnode != nullptr);
    auto compositingFilter = RSFilter::CreateBlurFilter(0.0f, 0.0f);
    rsnode->SetCompositingFilter(compositingFilter);
}

/**
 * @tc.name: SetShadowPath01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetShadowPath01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    ASSERT_TRUE(rsnode != nullptr);
    auto shadowpath = RSPath::CreateRSPath();
    rsnode->SetShadowPath(shadowpath);
}

/**
 * @tc.name: SetFreeze01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetFreeze01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetFreeze(true);
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
};

/**
 * @tc.name: SetModifier01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetModifier01, TestSize.Level1)
{
    RSSurfaceNodeConfig surfaceNodeConfig;
    auto node = RSSurfaceNode::Create(surfaceNodeConfig, false);
    auto value = Vector4f(10.f);
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
 * @tc.name: SetModifier02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetModifier02, TestSize.Level1)
{
    auto node = RSCanvasNode::Create();
    node->SetMotionPathOption(std::make_shared<RSMotionPathOption>(""));
    {
        auto value = Vector4f(10.f);
        auto prop = std::make_shared<RSAnimatableProperty<Vector4f>>(value);
        auto modifier = std::make_shared<RSBoundsModifier>(prop);
        node->AddModifier(modifier);
        ASSERT_TRUE(node != nullptr);
        ASSERT_EQ(node->GetStagingProperties().GetBounds(), value);
        node->RemoveModifier(modifier);
    }
    {
        auto value = Vector4f(10.f);
        auto prop = std::make_shared<RSAnimatableProperty<Vector4f>>(value);
        auto modifier = std::make_shared<RSFrameModifier>(prop);
        node->AddModifier(modifier);
        ASSERT_TRUE(node != nullptr);
        ASSERT_EQ(node->GetStagingProperties().GetFrame(), value);
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
    animation->SetDuration(10);
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
 * @tc.name: OpenImplicitAnimationTest01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, OpenImplicitAnimationTest01, TestSize.Level1)
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
 * @tc.name: AnimateTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, AnimateTest, TestSize.Level1)
{
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    PropertyCallback propertyCallback = nullptr;
    std::function<void()> finishCallback = nullptr;
    std::function<void()> repeatCallback = nullptr;
    auto animate = RSNode::Animate(timingProtocol, timingCurve, propertyCallback, finishCallback);
    std::vector<std::shared_ptr<RSAnimation>> vec;

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
HWTEST_F(RSNodeOneTest, AnimateWithCurrentOptionsTest, TestSize.Level1)
{
    PropertyCallback propertyCallback = nullptr;
    std::function<void()> finishCallback = nullptr;
    bool timingSensitive = true;
    auto animateWithCurrentOptions =
        RSNode::AnimateWithCurrentOptions(propertyCallback, finishCallback, timingSensitive);
    std::vector<std::shared_ptr<RSAnimation>> vec;

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
HWTEST_F(RSNodeOneTest, AnimateWithCurrentCallbackTest, TestSize.Level1)
{
    RSAnimationTimingProtocol timingProtocol;
    RSAnimationTimingCurve timingCurve;
    PropertyCallback propertyCallback = nullptr;
    auto Callback = RSNode::AnimateWithCurrentCallback(timingProtocol, timingCurve, propertyCallback);
    std::vector<std::shared_ptr<RSAnimation>> vec;

    propertyCallback = []() {};
    Callback = RSNode::AnimateWithCurrentCallback(timingProtocol, timingCurve, propertyCallback);
    EXPECT_TRUE(propertyCallback != nullptr);
}

/**
 * @tc.name: SetColorBlendMode
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetColorBlendMode, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    ASSERT_TRUE(rsnode != nullptr);
    RSColorBlendMode blendModeType = RSColorBlendMode::NONE;
    rsnode->SetColorBlendMode(blendModeType);
    blendModeType = RSColorBlendMode::DST_IN;
    rsnode->SetColorBlendMode(blendModeType);
    blendModeType = RSColorBlendMode::SRC_IN;
    rsnode->SetColorBlendMode(blendModeType);
}

/**
 * @tc.name: SetTextureExport
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetTextureExport, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetTextureExport(true);
    ASSERT_TRUE(rsnode->IsTextureExportNode());

    rsnode->SetTextureExport(false);
    ASSERT_EQ(rsnode->IsTextureExportNode(), false);

    rsnode->isTextureExportNode_ = true;
    rsnode->SetTextureExport(false);
    EXPECT_TRUE(rsnode->children_.empty());
}

/**
 * @tc.name: SyncTextureExport
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SyncTextureExport, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SyncTextureExport(false);

    rsnode->SyncTextureExport(true);
    EXPECT_TRUE(rsnode->children_.empty());

    rsnode->children_.push_back(0);
    rsnode->children_.push_back(1);
    rsnode->SyncTextureExport(true);
    EXPECT_TRUE(!rsnode->children_.empty());

    rsnode->isTextureExportNode_ = true;
    rsnode->SyncTextureExport(false);
    EXPECT_TRUE(!rsnode->children_.empty());
}

/**
 * @tc.name: CalcExpectedFrameRate
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, CalcExpectedFrameRate, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    auto canvasNode = RSCanvasNode::Create();
    rsnode->AddChild(canvasNode, -1);
    auto scene = "test_scene";
    float speed = 0;
    ASSERT_EQ(rsnode->CalcExpectedFrameRate(scene, speed), 0);
}

/**
 * @tc.name: RemoveChildByNodeIdTest Test
 * @tc.desc: test results of RemoveChildByNodeId
 * @tc.type: FUNC
 * @tc.require:issueI9MWJR
 */
HWTEST_F(RSNodeOneTest, RemoveChildByNodeIdTest, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    NodeId childId = 1;
    rsnode->RemoveChildByNodeId(childId);
    auto nodePtr = RSCanvasNode::Create();
    nodePtr->SetParent(2);
    RSNodeMap::MutableInstance().nodeMap_.emplace(nodePtr->GetId(), nodePtr);
    rsnode->RemoveChildByNodeId(nodePtr->GetId());
    auto list = rsnode->children_;
    ASSERT_EQ(std::find(list.begin(), list.end(), nodePtr->GetId()), list.end());
}

/**
 * @tc.name: RemoveChild
 * @tc.desc: test results of RemoveChild
 * @tc.type: FUNC
 * @tc.require: issueI9RLG7
 */
HWTEST_F(RSNodeOneTest, RemoveChild, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    std::shared_ptr<RSNode> child = nullptr;
    rsnode->RemoveChild(child);
    EXPECT_TRUE(child == nullptr);

    child = std::make_shared<RSNode>(0);
    child->parent_ = 2;
    rsnode->RemoveChild(child);
    EXPECT_TRUE(child != nullptr);

    child->parent_ = rsnode->id_;
    rsnode->RemoveChild(child);
    EXPECT_TRUE(child->parent_ != rsnode->id_);

    delete RSTransactionProxy::instance_;
    RSTransactionProxy::instance_ = nullptr;
    rsnode->RemoveChild(child);
    RSTransactionProxy::instance_ = new RSTransactionProxy();
}

/**
 * @tc.name: RemoveChildByNodeId01
 * @tc.desc: test results of RemoveChildByNodeId
 * @tc.type: FUNC
 * @tc.require: issueI9RLG7
 */
HWTEST_F(RSNodeOneTest, RemoveChildByNodeId01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    NodeId childId = 0;
    rsnode->RemoveChildByNodeId(childId);
    EXPECT_TRUE(!childId);

    rsnode->RemoveChildByNodeId(1);
    EXPECT_TRUE(!childId);
}

/**
 * @tc.name: DrawOnNode Test
 * @tc.desc: DrawOnNode and SetFreeze
 * @tc.type: FUNC
 * @tc.require:issueI9MWJR
 */
HWTEST_F(RSNodeOneTest, DrawOnNode, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->DrawOnNode(RSModifierType::BOUNDS, [](std::shared_ptr<Drawing::Canvas> canvasPtr) {});
    ASSERT_FALSE(rsnode->recordingUpdated_);
    rsnode->SetFreeze(true);
    rsnode->InitUniRenderEnabled();
    rsnode->SetFreeze(true);
    ASSERT_NE(RSTransactionProxy::GetInstance()->implicitRemoteTransactionData_, nullptr);
}

/**
 * @tc.name: MarkDrivenRender Test
 * @tc.desc: MarkDrivenRender and MarkDrivenRenderItemIndex and MarkDrivenRenderFramePaintState and MarkContentChanged
 * @tc.type: FUNC
 * @tc.require:issueI9MWJR
 */
HWTEST_F(RSNodeOneTest, MarkDrivenRender, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->MarkDrivenRender(true);
    rsnode->MarkDrivenRenderItemIndex(1);
    rsnode->MarkDrivenRenderFramePaintState(true);
    rsnode->MarkContentChanged(true);
    ASSERT_FALSE(rsnode->isNodeGroup_);
}

/**
 * @tc.name: SetFrameNodeInfo
 * @tc.desc: test results of SetFrameNodeInfo
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeOneTest, SetFrameNodeInfo, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    ASSERT_TRUE(rsnode != nullptr);
    std::string tag = "tag"; // for test
    rsnode->SetFrameNodeInfo(0, tag);
}

/**
 * @tc.name: GetFrameNodeId
 * @tc.desc: test results of GetFrameNodeId
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeOneTest, GetFrameNodeId, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    ASSERT_TRUE(rsnode != nullptr);
    std::string tag = "tag"; // for test
    rsnode->SetFrameNodeInfo(0, tag);
    int32_t id = rsnode->GetFrameNodeId();
}

/**
 * @tc.name: GetFrameNodeTag
 * @tc.desc: test results of GetFrameNodeTag
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeOneTest, GetFrameNodeTag, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    ASSERT_TRUE(rsnode != nullptr);
    std::string tag = "tag"; // for test
    rsnode->SetFrameNodeInfo(0, tag);
    std::string strResult = rsnode->GetFrameNodeTag();
    const char* cTag = tag.c_str();
    const char* cResult = strResult.c_str();
    int result = strcmp(cTag, cResult);
}

/**
 * @tc.name: AddKeyFrame
 * @tc.desc: test results of AddKeyFrame
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeOneTest, AddKeyFrame, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    ASSERT_TRUE(rsnode != nullptr);
    RSAnimationTimingCurve timingCurve;
    PropertyCallback propertyCallback = []() {};
    rsnode->AddKeyFrame(1.f, timingCurve, propertyCallback);
    rsnode->AddKeyFrame(1.f, propertyCallback);
}

/**
 * @tc.name: AddDurationKeyFrame
 * @tc.desc: test results of AddDurationKeyFrame
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeOneTest, AddDurationKeyFrame, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    ASSERT_TRUE(rsnode != nullptr);
    RSAnimationTimingCurve timingCurve;
    PropertyCallback propertyCallback = []() {};
    rsnode->AddDurationKeyFrame(1, timingCurve, propertyCallback);
}

/**
 * @tc.name: IsImplicitAnimationOpen
 * @tc.desc: test results of IsImplicitAnimationOpen
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeOneTest, IsImplicitAnimationOpen, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    ASSERT_TRUE(rsnode != nullptr);
    bool res = rsnode->IsImplicitAnimationOpen();
}

/**
 * @tc.name: ExecuteWithoutAnimation
 * @tc.desc: test results of ExecuteWithoutAnimation
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeOneTest, ExecuteWithoutAnimation, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    ASSERT_TRUE(rsnode != nullptr);
    PropertyCallback callback = nullptr;
    std::shared_ptr<RSImplicitAnimator> implicitAnimator = nullptr;
    rsnode->ExecuteWithoutAnimation(callback, implicitAnimator);

    callback = []() {};
    rsnode->ExecuteWithoutAnimation(callback, implicitAnimator);

    implicitAnimator = std::make_shared<RSImplicitAnimator>();
    rsnode->ExecuteWithoutAnimation(callback, implicitAnimator);
}

/**
 * @tc.name: FallbackAnimationsToRoot
 * @tc.desc: test results of FallbackAnimationsToRoot
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeOneTest, FallbackAnimationsToRoot, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    ASSERT_TRUE(rsnode != nullptr);
    auto target = RSNodeMap::Instance().GetAnimationFallbackNode();
    target = nullptr;
    rsnode->FallbackAnimationsToRoot();

    bool isRenderServiceNode = true;
    target = std::make_shared<RSNode>(isRenderServiceNode);
    AnimationId id = 1;
    std::shared_ptr<RSAnimation> animation = nullptr;
    rsnode->FallbackAnimationsToRoot();
    EXPECT_NE(RSNodeMap::Instance().animationFallbackNode_, nullptr);

    animation = std::make_shared<RSAnimation>();
    animation->repeatCount_ = 1;
    rsnode->animations_.insert({ id, animation });
    rsnode->FallbackAnimationsToRoot();
    EXPECT_TRUE(animation->repeatCount_);

    rsnode->animations_.clear();
    animation->repeatCount_ = -1;
    rsnode->animations_.insert({ id, animation });
    rsnode->FallbackAnimationsToRoot();
}

/**
 * @tc.name: AddAnimationInner
 * @tc.desc: test results of AddAnimationInner
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeOneTest, AddAnimationInner, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    std::shared_ptr<RSAnimation> animation = std::make_shared<RSAnimation>();
    rsnode->AddAnimationInner(animation);
    EXPECT_NE(animation, nullptr);
}

/**
 * @tc.name: RemoveAnimationInner
 * @tc.desc: test results of RemoveAnimationInner
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeOneTest, RemoveAnimationInner, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    ASSERT_TRUE(rsnode != nullptr);
    auto animation = std::make_shared<RSAnimation>();
    rsnode->AddAnimationInner(animation);
    rsnode->RemoveAnimationInner(animation);

    PropertyId id = animation->GetPropertyId();
    uint32_t num = 2;
    rsnode->animatingPropertyNum_.insert({ id, num });
    rsnode->RemoveAnimationInner(animation);

    rsnode->animatingPropertyNum_.clear();
    num = 1;
    rsnode->animatingPropertyNum_.insert({ id, num });
    rsnode->RemoveAnimationInner(animation);
}

/**
 * @tc.name: FinishAnimationByProperty
 * @tc.desc: test results of FinishAnimationByProperty
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeOneTest, FinishAnimationByProperty, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    PropertyId id = 0; // for test
    auto animation = std::make_shared<RSAnimation>();
    rsnode->AddAnimationInner(animation);
    rsnode->FinishAnimationByProperty(id);
    EXPECT_NE(animation, nullptr);

    id = animation->GetPropertyId();
    rsnode->animations_.insert({ id, animation });
    rsnode->FinishAnimationByProperty(id);
    EXPECT_NE(animation, nullptr);
}

/**
 * @tc.name: CancelAnimationByProperty
 * @tc.desc: test results of CancelAnimationByProperty
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeOneTest, CancelAnimationByProperty, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    ASSERT_TRUE(rsnode != nullptr);
    PropertyId id = 0; // for test
    rsnode->animatingPropertyNum_.insert({ id, 1 });
    delete RSTransactionProxy::instance_;
    RSTransactionProxy::instance_ = nullptr;
    rsnode->CancelAnimationByProperty(id, true);

    RSTransactionProxy::instance_ = new RSTransactionProxy();
    rsnode->CancelAnimationByProperty(id, true);
}

/**
 * @tc.name: GetShowingProperties
 * @tc.desc: test results of GetShowingProperties
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeOneTest, GetShowingProperties, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    ASSERT_TRUE(rsnode != nullptr);
    rsnode->GetShowingProperties();
}

/**
 * @tc.name: RemoveAllAnimations
 * @tc.desc: test results of RemoveAllAnimations
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeOneTest, RemoveAllAnimations, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    AnimationId id = 1;
    auto animation = std::make_shared<RSAnimation>();
    rsnode->animations_.insert({ id, animation });
    rsnode->RemoveAllAnimations();
    EXPECT_NE(animation, nullptr);
}

/**
 * @tc.name: RemoveAnimation
 * @tc.desc: test results of RemoveAnimation
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeOneTest, RemoveAnimation, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    ASSERT_TRUE(rsnode != nullptr);
    std::shared_ptr<RSAnimation> animation = nullptr;
    rsnode->RemoveAnimation(animation);

    animation = std::make_shared<RSAnimation>();
    rsnode->RemoveAnimation(animation);

    AnimationId id = animation->GetId();
    rsnode->animations_.insert({ id, animation });
    rsnode->RemoveAnimation(animation);
}

/**
 * @tc.name: SetMotionPathOption
 * @tc.desc: test results of SetMotionPathOption
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeOneTest, SetMotionPathOption, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    std::string path = "";
    auto motionPathOption = std::make_shared<RSMotionPathOption>(path);
    rsnode->SetMotionPathOption(motionPathOption);
    EXPECT_NE(motionPathOption, nullptr);
}

/**
 * @tc.name: GetMotionPathOption
 * @tc.desc: test results of GetMotionPathOption
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeOneTest, GetMotionPathOption, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    std::string path = "";
    auto motionPathOption = std::make_shared<RSMotionPathOption>(path);
    rsnode->SetMotionPathOption(motionPathOption);
    rsnode->GetMotionPathOption();
    EXPECT_NE(motionPathOption, nullptr);
}

/**
 * @tc.name: HasPropertyAnimation
 * @tc.desc: test results of HasPropertyAnimation
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeOneTest, HasPropertyAnimation, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    ASSERT_TRUE(rsnode != nullptr);
    PropertyId id = 1;
    bool res = rsnode->HasPropertyAnimation(id);
}

/**
 * @tc.name: GetAnimationByPropertyId
 * @tc.desc: test results of GetAnimationByPropertyId
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeOneTest, GetAnimationByPropertyId, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    PropertyId id = 0;
    AnimationId animationId = 1;
    auto animation = std::make_shared<RSAnimation>();
    rsnode->animations_.insert({ animationId, animation });
    rsnode->GetAnimationByPropertyId(id);
    EXPECT_NE(animation, nullptr);
}

/**
 * @tc.name: SetProperty
 * @tc.desc: test results of SetProperty
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeOneTest, SetProperty, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetProperty<RSAlphaModifier, RSAnimatableProperty<float>>(RSModifierType::ALPHA, 1.f);

    std::shared_ptr<RSPropertyBase> property = std::make_shared<RSPropertyBase>();
    RSModifierType modifierType = RSModifierType::ALPHA;
    std::shared_ptr<RSModifier> modifier = std::make_shared<RSBackgroundShaderModifier>(property);
    modifier->property_ = nullptr;
    rsnode->propertyModifiers_[modifierType] = modifier;
    rsnode->SetProperty<RSAlphaModifier, RSAnimatableProperty<float>>(RSModifierType::ALPHA, 1.f);
    EXPECT_TRUE(!rsnode->propertyModifiers_.empty());
}

/**
 * @tc.name: SetBounds01
 * @tc.desc: test results of SetBounds
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeOneTest, SetBounds01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    Vector4f bounds = { 1.f, 1.f, 1.f, 1.f }; // for test
    rsnode->SetBounds(bounds);
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBounds().x_, 1.f));
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBounds().y_, 1.f));
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBounds().z_, 1.f));
    EXPECT_TRUE(ROSEN_EQ(rsnode->GetStagingProperties().GetBounds().w_, 1.f));
}

/**
 * @tc.name: SetPivotZ
 * @tc.desc: test results of SetPivotZ
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeOneTest, SetPivotZ, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    ASSERT_TRUE(rsnode != nullptr);
    float pivotZ = 1.f; // for test
    rsnode->SetPivotZ(pivotZ);
}

/**
 * @tc.name: SetCornerRadius
 * @tc.desc: test results of SetCornerRadius
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeOneTest, SetCornerRadius, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    ASSERT_TRUE(rsnode != nullptr);
    Vector4f cornerRadius = { 1.f, 1.f, 1.f, 1.f }; // for test
    rsnode->SetCornerRadius(cornerRadius);
}

/**
 * @tc.name: SetCameraDistance
 * @tc.desc: test results of SetCameraDistance
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeOneTest, SetCameraDistance, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    ASSERT_TRUE(rsnode != nullptr);
    float cameraDistance = 1.f; // for test
    rsnode->SetCameraDistance(cameraDistance);
}

/**
 * @tc.name: SetEnvForegroundColor
 * @tc.desc: test results of SetEnvForegroundColor
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeOneTest, SetEnvForegroundColor, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    uint32_t colorValue = 1; // for test
    rsnode->SetEnvForegroundColor(colorValue);
    ASSERT_EQ(rsnode->GetInstanceId(), -1);
}

/**
 * @tc.name: SetEnvForegroundColorStrategy
 * @tc.desc: test results of SetEnvForegroundColorStrategy
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeOneTest, SetEnvForegroundColorStrategy, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    ASSERT_TRUE(rsnode != nullptr);
    ForegroundColorStrategyType strategyType = ForegroundColorStrategyType::INVERT_BACKGROUNDCOLOR;
    rsnode->SetEnvForegroundColorStrategy(strategyType);
}

/**
 * @tc.name: SetCustomClipToFrame
 * @tc.desc: test results of SetCustomClipToFrame
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeOneTest, SetCustomClipToFrame, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    ASSERT_TRUE(rsnode != nullptr);
    Vector4f clipRect{1.0f, 1.0f, 1.0f, 1.0f}; // for test
    rsnode->SetCustomClipToFrame(clipRect);
}

/**
 * @tc.name: SetParticleParams
 * @tc.desc: test results of SetParticleParams
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeOneTest, SetParticleParams, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    ASSERT_TRUE(rsnode != nullptr);
    ParticleParams param;
    std::vector<ParticleParams> particleParams;
    particleParams.push_back(param);
    std::function<void()> finishCallback = []() {};
    rsnode->SetParticleParams(particleParams, finishCallback);

    finishCallback = nullptr;
    rsnode->SetParticleParams(particleParams, finishCallback);
}

/**
 * @tc.name: SetParticleDrawRegion
 * @tc.desc: test results of SetParticleDrawRegion
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeOneTest, SetParticleDrawRegion, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    ASSERT_TRUE(rsnode != nullptr);
    EmitterConfig emitterConfig;
    std::shared_ptr<RSImage> image;
    ParticleParams param;
    param.emitterConfig_ = emitterConfig;
    std::vector<ParticleParams> particleParams;
    particleParams.push_back(param);
    particleParams.push_back(param);
    particleParams.push_back(param);
    particleParams.push_back(param);
    rsnode->SetParticleDrawRegion(particleParams);

    param.emitterConfig_.type_ = ParticleType::IMAGES;
    rsnode->SetParticleDrawRegion(particleParams);

    image = std::make_shared<RSImage>();
    emitterConfig.image_ = image;
    rsnode->SetParticleDrawRegion(particleParams);

    param.emitterConfig_.type_ = ParticleType::POINTS;
    rsnode->SetParticleDrawRegion(particleParams);
}

/**
 * @tc.name: SetEmitterUpdater
 * @tc.desc: test results of SetEmitterUpdater
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeOneTest, SetEmitterUpdater, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    ASSERT_TRUE(rsnode != nullptr);
    std::vector<std::shared_ptr<EmitterUpdater>> para;
    rsnode->SetEmitterUpdater(para);
}

/**
 * @tc.name: SetParticleNoiseFields
 * @tc.desc: test results of SetParticleNoiseFields
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeOneTest, SetParticleNoiseFields, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    std::shared_ptr<ParticleNoiseFields> para = std::make_shared<ParticleNoiseFields>();
    rsnode->SetParticleNoiseFields(para);
    EXPECT_NE(para, nullptr);
}

/**
 * @tc.name: SetBgImageInnerRect
 * @tc.desc: test results of SetBgImageInnerRect
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeOneTest, SetBgImageInnerRect, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    Vector4f rect = { 1.f, 1.f, 1.f, 1.f }; // for test
    rsnode->SetBgImageInnerRect(rect);
    EXPECT_NE(rsnode, nullptr);
}

/**
 * @tc.name: SetOuterBorderColor
 * @tc.desc: test results of SetOuterBorderColor
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeOneTest, SetOuterBorderColor, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    Color example(255, 0, 0, 255); // for test
    Vector4<Color> color = { example, example, example, example };
    rsnode->SetOuterBorderColor(color);
    EXPECT_NE(rsnode, nullptr);
}

/**
 * @tc.name: SetOuterBorderWidth
 * @tc.desc: test results of SetOuterBorderWidth
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeOneTest, SetOuterBorderWidth, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    Vector4f width = { 1.f, 1.f, 1.f, 1.f }; // for test
    rsnode->SetOuterBorderWidth(width);
    EXPECT_NE(rsnode, nullptr);
}

/**
 * @tc.name: SetOuterBorderStyle
 * @tc.desc: test results of SetOuterBorderStyle
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeOneTest, SetOuterBorderStyle, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    Vector4<BorderStyle> style = { BorderStyle::SOLID, BorderStyle::DASHED, BorderStyle::DOTTED,
        BorderStyle::NONE }; // for test
    rsnode->SetOuterBorderStyle(style);
    EXPECT_NE(rsnode, nullptr);
}

/**
 * @tc.name: SetOuterBorderRadius
 * @tc.desc: test results of SetOuterBorderRadius
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeOneTest, SetOuterBorderRadius, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    Vector4f radius = { 1.f, 1.f, 1.f, 1.f }; // for test
    rsnode->SetOuterBorderRadius(radius);
    EXPECT_NE(rsnode, nullptr);
}

} // namespace OHOS::Rosen
