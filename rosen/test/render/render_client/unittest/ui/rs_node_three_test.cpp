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

/**
 * @tc.name: SetandGetClipBounds01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetClipBounds01, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
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
    EXPECT_EQ(rsnode->GetStagingProperties().GetShadowIsFilled(), true);
}

/**
 * @tc.name: SetandGetShadowIsFilled02
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetandGetShadowIsFilled02, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    rsnode->SetShadowIsFilled(false);
    EXPECT_EQ(rsnode->GetStagingProperties().GetShadowIsFilled(), false);
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
 * @tc.name: SetProperty01
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, SetProperty01, TestSize.Level1)
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
        RSModifierType::FOREGROUND_COLOR, Color(), Color(0xFF0FF0));
    SetPropertyTest<RSBackgroundColorModifier, RSAnimatableProperty<Color>, Color>(
        RSModifierType::BACKGROUND_COLOR, Color(), Color(0xFF0FF0));
    SetPropertyTest<RSBgImageModifier, RSProperty<std::shared_ptr<RSImage>>, std::shared_ptr<RSImage>>(
        RSModifierType::BG_IMAGE, std::make_shared<RSImage>(), nullptr);
    SetPropertyTest<RSBgImageWidthModifier, RSAnimatableProperty<float>, float>(
        RSModifierType::BG_IMAGE_WIDTH, 45.f, 90.f);
    SetPropertyTest<RSBgImageHeightModifier, RSAnimatableProperty<float>, float>(
        RSModifierType::BG_IMAGE_HEIGHT, 45.f, 90.f);
    SetPropertyTest<RSBorderColorModifier, RSAnimatableProperty<Vector4<Color>>, Vector4<Color>>(
        RSModifierType::BORDER_COLOR, Vector4<Color>(), Vector4<Color>(Color(0xFF0FF0)));
    SetPropertyTest<RSBorderWidthModifier, RSAnimatableProperty<Vector4f>, Vector4f>(
        RSModifierType::BORDER_WIDTH, Vector4f(), Vector4f(1.f));
    SetPropertyTest<RSBorderStyleModifier, RSProperty<Vector4<uint32_t>>, Vector4<uint32_t>>(
        RSModifierType::BORDER_STYLE, Vector4<uint32_t>(), Vector4<uint32_t>(1));
    SetPropertyTest<RSShadowColorModifier, RSAnimatableProperty<Color>, Color>(
        RSModifierType::SHADOW_COLOR, Color(), Color(0xFF0FF0));
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
        auto value = Vector2f(20.f, 30.f);
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
 * @tc.name: CloseImplicitAnimationTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSNodeOneTest, CloseImplicitAnimationTest, TestSize.Level1)
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
HWTEST_F(RSNodeOneTest, AnimateTest, TestSize.Level1)
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
HWTEST_F(RSNodeOneTest, AnimateWithCurrentOptionsTest, TestSize.Level1)
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
HWTEST_F(RSNodeOneTest, AnimateWithCurrentCallbackTest, TestSize.Level1)
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
HWTEST_F(RSNodeOneTest, SetColorBlendMode, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
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
    EXPECT_EQ(RSTransactionProxy::GetInstance(), nullptr);
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
    std::string tag = "tag"; // for test
    rsnode->SetFrameNodeInfo(0, tag);
    EXPECT_EQ(rsnode->frameNodeId_, 0);
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
    std::string tag = "tag"; // for test
    rsnode->SetFrameNodeInfo(0, tag);
    int32_t id = rsnode->GetFrameNodeId();
    EXPECT_EQ(id, 0);
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
    std::string tag = "tag"; // for test
    rsnode->SetFrameNodeInfo(0, tag);
    std::string strResult = rsnode->GetFrameNodeTag();
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
HWTEST_F(RSNodeOneTest, AddKeyFrame, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
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
    bool res = rsnode->IsImplicitAnimationOpen();
    EXPECT_EQ(res, false);
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
    PropertyCallback callback = nullptr;
    std::shared_ptr<RSImplicitAnimator> implicitAnimator = nullptr;
    rsnode->ExecuteWithoutAnimation(callback, implicitAnimator);
    EXPECT_EQ(callback, nullptr);

    callback = []() {};
    rsnode->ExecuteWithoutAnimation(callback, implicitAnimator);
    EXPECT_EQ(implicitAnimator, nullptr);

    implicitAnimator = std::make_shared<RSImplicitAnimator>();
    rsnode->ExecuteWithoutAnimation(callback, implicitAnimator);
    EXPECT_NE(implicitAnimator, nullptr);
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
    auto target = RSNodeMap::Instance().GetAnimationFallbackNode();
    target = nullptr;
    rsnode->FallbackAnimationsToRoot();
    EXPECT_EQ(rsnode->motionPathOption_, nullptr);

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
    EXPECT_TRUE(animation->repeatCount_ == -1);
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
    auto animation = std::make_shared<RSAnimation>();
    rsnode->AddAnimationInner(animation);
    rsnode->RemoveAnimationInner(animation);
    EXPECT_NE(animation, nullptr);

    PropertyId id = animation->GetPropertyId();
    uint32_t num = 2;
    rsnode->animatingPropertyNum_.insert({ id, num });
    rsnode->RemoveAnimationInner(animation);
    EXPECT_NE(animation, nullptr);

    rsnode->animatingPropertyNum_.clear();
    num = 1;
    rsnode->animatingPropertyNum_.insert({ id, num });
    rsnode->RemoveAnimationInner(animation);
    EXPECT_NE(animation, nullptr);
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
    PropertyId id = 0; // for test
    rsnode->animatingPropertyNum_.insert({ id, 1 });
    delete RSTransactionProxy::instance_;
    RSTransactionProxy::instance_ = nullptr;
    rsnode->CancelAnimationByProperty(id, true);
    EXPECT_EQ(RSTransactionProxy::GetInstance(), nullptr);

    RSTransactionProxy::instance_ = new RSTransactionProxy();
    rsnode->CancelAnimationByProperty(id, true);
    EXPECT_NE(RSTransactionProxy::GetInstance(), nullptr);
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
    rsnode->GetShowingProperties();
}

/**
 * @tc.name: AddAnimation
 * @tc.desc: test results of AddAnimation
 * @tc.type: FUNC
 * @tc.require: issueI9KAZH
 */
HWTEST_F(RSNodeOneTest, AddAnimation, TestSize.Level1)
{
    auto rsnode = RSCanvasNode::Create();
    std::shared_ptr<RSAnimation> animation = nullptr;
    rsnode->AddAnimation(animation);
    EXPECT_EQ(animation, nullptr);

    animation = std::make_shared<RSAnimation>();
    rsnode->AddAnimation(animation);
    EXPECT_TRUE(animation != nullptr);

    rsnode->id_ = 0;
    rsnode->AddAnimation(animation);
    EXPECT_TRUE(animation != nullptr);

    animation->duration_ = 0;
    rsnode->AddAnimation(animation);
    EXPECT_TRUE(!animation->duration_);

    rsnode->id_ = 1;
    rsnode->AddAnimation(animation);
    EXPECT_TRUE(rsnode->id_ = 1);

    AnimationId id = animation->GetId();
    rsnode->animations_.insert({ id, animation });
    rsnode->AddAnimation(animation);
    EXPECT_TRUE(!rsnode->animations_.empty());
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
    std::shared_ptr<RSAnimation> animation = nullptr;
    rsnode->RemoveAnimation(animation);
    EXPECT_EQ(animation, nullptr);

    animation = std::make_shared<RSAnimation>();
    rsnode->RemoveAnimation(animation);
    EXPECT_NE(animation, nullptr);

    AnimationId id = animation->GetId();
    rsnode->animations_.insert({ id, animation });
    rsnode->RemoveAnimation(animation);
    EXPECT_NE(animation, nullptr);
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

} // namespace OHOS::Rosen
