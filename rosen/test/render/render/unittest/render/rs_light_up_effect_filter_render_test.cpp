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

#include "gtest/gtest.h"

#include "render/rs_light_up_effect_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSLightUpEffectFilterRenderTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSLightUpEffectFilterRenderTest::SetUpTestCase() {}
void RSLightUpEffectFilterRenderTest::TearDownTestCase() {}
void RSLightUpEffectFilterRenderTest::SetUp() {}
void RSLightUpEffectFilterRenderTest::TearDown() {}

/**
 * @tc.name: TestRSLightUpEffectFilterRender001
 * @tc.desc: Verify function TestRSLightUpEffectFilterRender001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterRenderTest, TestRSLightUpEffectFilterRender001, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    EXPECT_TRUE(filter != nullptr);

    EXPECT_TRUE(filter->GetLightUpDegree() == 1.0f);
    filter->GetDescription();

    auto filter2 = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto result = filter->Compose(filter2);
    EXPECT_TRUE(result != nullptr);

    RSLightUpEffectFilter rsLightUpEffectFilter = RSLightUpEffectFilter(lightUpDegree);
    rsLightUpEffectFilter.GetDescription();
}

/**
 * @tc.name: TestRSLightUpEffectFilterRenderFLT_MAX
 * @tc.desc: Verify function TestRSLightUpEffectFilterRenderFLT_MAX
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterRenderTest, TestRSLightUpEffectFilterRenderFLT_MAX, TestSize.Level1)
{
    float lightUpDegree = FLT_MAX;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    EXPECT_TRUE(filter != nullptr);

    EXPECT_TRUE(filter->GetLightUpDegree() == FLT_MAX);
    filter->GetDescription();

    auto filter2 = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto result = filter->Compose(filter2);
    EXPECT_TRUE(result != nullptr);

    RSLightUpEffectFilter rsLightUpEffectFilter = RSLightUpEffectFilter(lightUpDegree);
    rsLightUpEffectFilter.GetDescription();
}

/**
 * @tc.name: TestRSLightUpEffectFilterRenderFLT_MIN
 * @tc.desc: Verify function TestRSLightUpEffectFilterRenderFLT_MIN
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterRenderTest, TestRSLightUpEffectFilterRenderFLT_MIN, TestSize.Level1)
{
    float lightUpDegree = FLT_MIN;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    EXPECT_TRUE(filter != nullptr);

    EXPECT_TRUE(filter->GetLightUpDegree() == FLT_MIN);
    filter->GetDescription();

    auto filter2 = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto result = filter->Compose(filter2);
    EXPECT_TRUE(result != nullptr);

    RSLightUpEffectFilter rsLightUpEffectFilter = RSLightUpEffectFilter(lightUpDegree);
    rsLightUpEffectFilter.GetDescription();
}

/**
 * @tc.name: TestRSLightUpEffectFilterRender002
 * @tc.desc: Verify function TestRSLightUpEffectFilterRender002
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterRenderTest, TestRSLightUpEffectFilterRender002, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    EXPECT_TRUE(filter != nullptr);

    auto filter2 = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    EXPECT_TRUE(filter2 != nullptr);

    auto result = filter->Add(filter2);
    EXPECT_TRUE(result != nullptr);

    result = filter->Sub(filter2);
    EXPECT_TRUE(result != nullptr);

    result = filter->Multiply(1.0f);
    EXPECT_TRUE(result != nullptr);

    result = filter->Negate();
    EXPECT_TRUE(result != nullptr);
}

/**
 * @tc.name: TestCreateLightUpEffectFilter001
 * @tc.desc: Verify function TestCreateLightUpEffectFilter001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterRenderTest, TestCreateLightUpEffectFilter001, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    EXPECT_NE(filter->CreateLightUpEffectFilter(lightUpDegree), nullptr);
}

/**
 * @tc.name: TestGetLightUpDegree
 * @tc.desc: Verify function TestGetLightUpDegree
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterRenderTest, TestGetLightUpDegree, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    EXPECT_TRUE(filter != nullptr);
    filter->Add(filter);
    EXPECT_NE(filter->GetLightUpDegree(), 0.f);
}

/**
 * @tc.name: TestGetDescription001
 * @tc.desc: Verify function TestGetDescription001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterRenderTest, TestGetDescription001, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    EXPECT_EQ(filter->GetDescription(), "RSLightUpEffectFilter light up degree is " + std::to_string(lightUpDegree));
}

/**
 * @tc.name: TestCompose
 * @tc.desc: Verify function TestCompose
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterRenderTest, TestCompose, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto filter_ = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    EXPECT_NE(filter->Compose(filter_), nullptr);
}

/**
 * @tc.name: TestGetDescriptAddTest001ion001
 * @tc.desc: Verify function Add TestGetDescriptAddTest001ion001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterRenderTest, TestGetDescriptAddTest001ion001, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::LIGHT_UP_EFFECT;
    EXPECT_NE(filter->Add(rhs), nullptr);
}

/**
 * @tc.name: TestGetDescriptAddNONE
 * @tc.desc: Verify function Add TestGetDescriptAddNONE
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterRenderTest, TestGetDescriptAddNONE, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::NONE;
    EXPECT_NE(filter->Add(rhs), nullptr);
}

/**
 * @tc.name: TestGetDescriptAddBLUR
 * @tc.desc: Verify function Add TestGetDescriptAddBLUR
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterRenderTest, TestGetDescriptAddBLUR, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::BLUR;
    EXPECT_NE(filter->Add(rhs), nullptr);
}

/**
 * @tc.name: TestGetDescriptAddMATERIAL
 * @tc.desc: Verify function Add TestGetDescriptAddMATERIAL
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterRenderTest, TestGetDescriptAddMATERIAL, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::MATERIAL;
    EXPECT_NE(filter->Add(rhs), nullptr);
}

/**
 * @tc.name: TestGetDescriptAddAIBAR
 * @tc.desc: Verify function Add TestGetDescriptAddAIBAR
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterRenderTest, TestGetDescriptAddAIBAR, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::AIBAR;
    EXPECT_NE(filter->Add(rhs), nullptr);
}

/**
 * @tc.name: TestGetDescriptAddLINEAR_GRADIENT_BLUR
 * @tc.desc: Verify function Add TestGetDescriptAddLINEAR_GRADIENT_BLUR
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterRenderTest, TestGetDescriptAddLINEAR_GRADIENT_BLUR, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::LINEAR_GRADIENT_BLUR;
    EXPECT_NE(filter->Add(rhs), nullptr);
}

/**
 * @tc.name: TestGetDescriptAddFOREGROUND_EFFECT
 * @tc.desc: Verify function Add TestGetDescriptAddFOREGROUND_EFFECT
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterRenderTest, TestGetDescriptAddFOREGROUND_EFFECT, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::FOREGROUND_EFFECT;
    EXPECT_NE(filter->Add(rhs), nullptr);
}

/**
 * @tc.name: TestGetDescriptAddMOTION_BLUR
 * @tc.desc: Verify function Add TestGetDescriptAddMOTION_BLUR
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterRenderTest, TestGetDescriptAddMOTION_BLUR, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::MOTION_BLUR;
    EXPECT_NE(filter->Add(rhs), nullptr);
}

/**
 * @tc.name: TestGetDescriptAddSPHERIZE_EFFECT
 * @tc.desc: Verify function Add TestGetDescriptAddSPHERIZE_EFFECT
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterRenderTest, TestGetDescriptAddSPHERIZE_EFFECT, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::SPHERIZE_EFFECT;
    EXPECT_NE(filter->Add(rhs), nullptr);
}

/**
 * @tc.name: TestGetDescriptAddCOLORFUL_SHADOW
 * @tc.desc: Verify function Add TestGetDescriptAddCOLORFUL_SHADOW
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterRenderTest, TestGetDescriptAddCOLORFUL_SHADOW, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::COLORFUL_SHADOW;
    EXPECT_NE(filter->Add(rhs), nullptr);
}

/**
 * @tc.name: TestGetDescriptAddATTRACTION_EFFECT
 * @tc.desc: Verify function Add TestGetDescriptAddATTRACTION_EFFECT
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterRenderTest, TestGetDescriptAddATTRACTION_EFFECT, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::ATTRACTION_EFFECT;
    EXPECT_NE(filter->Add(rhs), nullptr);
}

/**
 * @tc.name: TestGetDescriptAddWATER_RIPPLE
 * @tc.desc: Verify function Add TestGetDescriptAddWATER_RIPPLE
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterRenderTest, TestGetDescriptAddWATER_RIPPLE, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::WATER_RIPPLE;
    EXPECT_NE(filter->Add(rhs), nullptr);
}

/**
 * @tc.name: TestGetDescriptAddCOMPOUND_EFFECT
 * @tc.desc: Verify function Add TestGetDescriptAddCOMPOUND_EFFECT
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterRenderTest, TestGetDescriptAddCOMPOUND_EFFECT, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::COMPOUND_EFFECT;
    EXPECT_NE(filter->Add(rhs), nullptr);
}

/**
 * @tc.name: TestGetDescriptAddMAGNIFIER
 * @tc.desc: Verify function Add TestGetDescriptAddMAGNIFIER
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterRenderTest, TestGetDescriptAddMAGNIFIER, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::MAGNIFIER;
    EXPECT_NE(filter->Add(rhs), nullptr);
}

/**
 * @tc.name: TestGetDescriptAddFLY_OUT
 * @tc.desc: Verify function Add TestGetDescriptAddFLY_OUT
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterRenderTest, TestGetDescriptAddFLY_OUT, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::FLY_OUT;
    EXPECT_NE(filter->Add(rhs), nullptr);
}

/**
 * @tc.name: TestGetDescriptAddDISTORT
 * @tc.desc: Verify function Add TestGetDescriptAddDISTORT
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterRenderTest, TestGetDescriptAddDISTORT, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::DISTORT;
    EXPECT_NE(filter->Add(rhs), nullptr);
}

/**
 * @tc.name: TestAdd002
 * @tc.desc: Verify function TestAdd002
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterRenderTest, TestAdd002, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    EXPECT_EQ(filter->Add(nullptr), filter);
}

/**
 * @tc.name: TestSub001
 * @tc.desc: Verify function TestSub001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterRenderTest, TestSub001, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Sub(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::LIGHT_UP_EFFECT;
    EXPECT_NE(filter->Sub(rhs), nullptr);
}

/**
 * @tc.name: TestSubNONE
 * @tc.desc: Verify function TestSubNONE
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterRenderTest, TestSubNONE, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Sub(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::NONE;
    EXPECT_NE(filter->Sub(rhs), nullptr);
}

/**
 * @tc.name: TestSubBLUR
 * @tc.desc: Verify function TestSubBLUR
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterRenderTest, TestSubBLUR, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Sub(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::BLUR;
    EXPECT_NE(filter->Sub(rhs), nullptr);
}

/**
 * @tc.name: TestSubMATERIAL
 * @tc.desc: Verify function TestSubMATERIAL
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterRenderTest, TestSubMATERIAL, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Sub(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::MATERIAL;
    EXPECT_NE(filter->Sub(rhs), nullptr);
}

/**
 * @tc.name: TestSubAIBAR
 * @tc.desc: Verify function TestSubAIBAR
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterRenderTest, TestSubAIBAR, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Sub(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::AIBAR;
    EXPECT_NE(filter->Sub(rhs), nullptr);
}

/**
 * @tc.name: TestSubLINEAR_GRADIENT_BLUR
 * @tc.desc: Verify function TestSubLINEAR_GRADIENT_BLUR
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterRenderTest, TestSubLINEAR_GRADIENT_BLUR, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Sub(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::LINEAR_GRADIENT_BLUR;
    EXPECT_NE(filter->Sub(rhs), nullptr);
}

/**
 * @tc.name: TestSubFOREGROUND_EFFECT
 * @tc.desc: Verify function TestSubFOREGROUND_EFFECT
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterRenderTest, TestSubFOREGROUND_EFFECT, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Sub(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::FOREGROUND_EFFECT;
    EXPECT_NE(filter->Sub(rhs), nullptr);
}

/**
 * @tc.name: TestSubMOTION_BLUR
 * @tc.desc: Verify function TestSubMOTION_BLUR
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterRenderTest, TestSubMOTION_BLUR, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Sub(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::MOTION_BLUR;
    EXPECT_NE(filter->Sub(rhs), nullptr);
}

/**
 * @tc.name: TestSubSPHERIZE_EFFECT
 * @tc.desc: Verify function TestSubSPHERIZE_EFFECT
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterRenderTest, TestSubSPHERIZE_EFFECT, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Sub(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::SPHERIZE_EFFECT;
    EXPECT_NE(filter->Sub(rhs), nullptr);
}

/**
 * @tc.name: TestSubCOLORFUL_SHADOW
 * @tc.desc: Verify function TestSubCOLORFUL_SHADOW
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterRenderTest, TestSubCOLORFUL_SHADOW, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Sub(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::COLORFUL_SHADOW;
    EXPECT_NE(filter->Sub(rhs), nullptr);
}

/**
 * @tc.name: TestSubATTRACTION_EFFECT
 * @tc.desc: Verify function TestSubATTRACTION_EFFECT
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterRenderTest, TestSubATTRACTION_EFFECT, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Sub(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::ATTRACTION_EFFECT;
    EXPECT_NE(filter->Sub(rhs), nullptr);
}

/**
 * @tc.name: TestSubWATER_RIPPLE
 * @tc.desc: Verify function TestSubWATER_RIPPLE
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterRenderTest, TestSubWATER_RIPPLE, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Sub(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::WATER_RIPPLE;
    EXPECT_NE(filter->Sub(rhs), nullptr);
}

/**
 * @tc.name: TestSubCOMPOUND_EFFECT
 * @tc.desc: Verify function TestSubCOMPOUND_EFFECT
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterRenderTest, TestSubCOMPOUND_EFFECT, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Sub(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::COMPOUND_EFFECT;
    EXPECT_NE(filter->Sub(rhs), nullptr);
}

/**
 * @tc.name: TestSubMAGNIFIER
 * @tc.desc: Verify function TestSubMAGNIFIER
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterRenderTest, TestSubMAGNIFIER, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Sub(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::MAGNIFIER;
    EXPECT_NE(filter->Sub(rhs), nullptr);
}

/**
 * @tc.name: TestSubFLY_OUT
 * @tc.desc: Verify function TestSubFLY_OUT
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterRenderTest, TestSubFLY_OUT, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Sub(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::FLY_OUT;
    EXPECT_NE(filter->Sub(rhs), nullptr);
}

/**
 * @tc.name: TestSubDISTORT
 * @tc.desc: Verify function TestSubDISTORT
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterRenderTest, TestSubDISTORT, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Sub(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::DISTORT;
    EXPECT_NE(filter->Sub(rhs), nullptr);
}

/**
 * @tc.name: TestSub002
 * @tc.desc: Verify function TestSub002
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterRenderTest, TestSub002, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    EXPECT_EQ(filter->Sub(nullptr), filter);
}

/**
 * @tc.name: TestIsEqual001
 * @tc.desc: Verify function TestIsEqual001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterRenderTest, TestIsEqual001, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto otherFilter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    EXPECT_TRUE(filter->IsEqual(otherFilter));
}

/**
 * @tc.name: TestMultiply001
 * @tc.desc: Verify function TestMultiply001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterRenderTest, TestMultiply001, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    EXPECT_NE(filter->Multiply(1.0f), nullptr);
}

/**
 * @tc.name: TestNegate001
 * @tc.desc: Verify function TestNegate001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterRenderTest, TestNegate001, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    EXPECT_NE(filter->Negate(), nullptr);
}

/**
 * @tc.name: TestIsNearEqual001
 * @tc.desc: Verify function TestIsNearEqual001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterRenderTest, TestIsNearEqual001, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    float lightUpDegree1 = 1.2f;
    float threshold = 0.5f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);

    std::shared_ptr<RSFilter> other1 = std::make_shared<RSLightUpEffectFilter>(lightUpDegree1);
    EXPECT_TRUE(filter->IsNearEqual(other1, threshold));
}

/**
 * @tc.name: TestIsNearZero001
 * @tc.desc: Verify function TestIsNearZero001
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSLightUpEffectFilterRenderTest, TestIsNearZero001, TestSize.Level1)
{
    float lightUpDegree = 0.2f;
    float threshold = 0.5f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    EXPECT_TRUE(filter->IsNearZero(threshold));
}
} // namespace OHOS::Rosen
