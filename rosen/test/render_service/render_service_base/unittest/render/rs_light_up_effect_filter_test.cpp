/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
class RSLightUpEffectFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSLightUpEffectFilterTest::SetUpTestCase() {}
void RSLightUpEffectFilterTest::TearDownTestCase() {}
void RSLightUpEffectFilterTest::SetUp() {}
void RSLightUpEffectFilterTest::TearDown() {}

/**
 * @tc.name: RSLightUpEffectFilterTest001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, RSLightUpEffectFilterTest001, TestSize.Level1)
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
 * @tc.name: RSLightUpEffectFilterTestFLT_MAX
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, RSLightUpEffectFilterTestFLT_MAX, TestSize.Level1)
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
 * @tc.name: RSLightUpEffectFilterTestFLT_MIN
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, RSLightUpEffectFilterTestFLT_MIN, TestSize.Level1)
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
 * @tc.name: RSLightUpEffectFilterTest002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, RSLightUpEffectFilterTest002, TestSize.Level1)
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
 * @tc.name: CreateLightUpEffectFilterTest001
 * @tc.desc: Verify function CreateLightUpEffectFilter
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, CreateLightUpEffectFilterTest001, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    EXPECT_NE(filter->CreateLightUpEffectFilter(lightUpDegree), nullptr);
}

/**
 * @tc.name: GetLightUpDegreeTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, GetLightUpDegreeTest, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    EXPECT_TRUE(filter != nullptr);
    filter->Add(filter);
    EXPECT_NE(filter->GetLightUpDegree(), 0.f);
}

/**
 * @tc.name: GetDescriptionTest001
 * @tc.desc: Verify function GetDescription
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, GetDescriptionTest001, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    EXPECT_EQ(filter->GetDescription(), "RSLightUpEffectFilter light up degree is " + std::to_string(lightUpDegree));
}

/**
 * @tc.name: ComposeTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, ComposeTest, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto filter_ = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    EXPECT_NE(filter->Compose(filter_), nullptr);
}

/**
 * @tc.name: AddTest001
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, GetDescriptAddTest001ionTest001, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::LIGHT_UP_EFFECT;
    EXPECT_NE(filter->Add(rhs), nullptr);
}

/**
 * @tc.name: GetDescriptAddTestNONE
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, GetDescriptAddTestNONE, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::NONE;
    EXPECT_NE(filter->Add(rhs), nullptr);
}

/**
 * @tc.name: GetDescriptAddTestBLUR
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, GetDescriptAddTestBLUR, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::BLUR;
    EXPECT_NE(filter->Add(rhs), nullptr);
}

/**
 * @tc.name: GetDescriptAddTestMATERIAL
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, GetDescriptAddTestMATERIAL, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::MATERIAL;
    EXPECT_NE(filter->Add(rhs), nullptr);
}

/**
 * @tc.name: GetDescriptAddTestAIBAR
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, GetDescriptAddTestAIBAR, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::AIBAR;
    EXPECT_NE(filter->Add(rhs), nullptr);
}

/**
 * @tc.name: GetDescriptAddTestLINEAR_GRADIENT_BLUR
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, GetDescriptAddTestLINEAR_GRADIENT_BLUR, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::LINEAR_GRADIENT_BLUR;
    EXPECT_NE(filter->Add(rhs), nullptr);
}

/**
 * @tc.name: GetDescriptAddTestFOREGROUND_EFFECT
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, GetDescriptAddTestFOREGROUND_EFFECT, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::FOREGROUND_EFFECT;
    EXPECT_NE(filter->Add(rhs), nullptr);
}

/**
 * @tc.name: GetDescriptAddTestMOTION_BLUR
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, GetDescriptAddTestMOTION_BLUR, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::MOTION_BLUR;
    EXPECT_NE(filter->Add(rhs), nullptr);
}

/**
 * @tc.name: GetDescriptAddTestSPHERIZE_EFFECT
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, GetDescriptAddTestSPHERIZE_EFFECT, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::SPHERIZE_EFFECT;
    EXPECT_NE(filter->Add(rhs), nullptr);
}

/**
 * @tc.name: GetDescriptAddTestCOLORFUL_SHADOW
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, GetDescriptAddTestCOLORFUL_SHADOW, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::COLORFUL_SHADOW;
    EXPECT_NE(filter->Add(rhs), nullptr);
}

/**
 * @tc.name: GetDescriptAddTestATTRACTION_EFFECT
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, GetDescriptAddTestATTRACTION_EFFECT, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::ATTRACTION_EFFECT;
    EXPECT_NE(filter->Add(rhs), nullptr);
}

/**
 * @tc.name: GetDescriptAddTestWATER_RIPPLE
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, GetDescriptAddTestWATER_RIPPLE, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::WATER_RIPPLE;
    EXPECT_NE(filter->Add(rhs), nullptr);
}

/**
 * @tc.name: GetDescriptAddTestCOMPOUND_EFFECT
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, GetDescriptAddTestCOMPOUND_EFFECT, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::COMPOUND_EFFECT;
    EXPECT_NE(filter->Add(rhs), nullptr);
}

/**
 * @tc.name: GetDescriptAddTestMAGNIFIER
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, GetDescriptAddTestMAGNIFIER, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::MAGNIFIER;
    EXPECT_NE(filter->Add(rhs), nullptr);
}

/**
 * @tc.name: GetDescriptAddTestFLY_OUT
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, GetDescriptAddTestFLY_OUT, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::FLY_OUT;
    EXPECT_NE(filter->Add(rhs), nullptr);
}

/**
 * @tc.name: GetDescriptAddTestDISTORT
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, GetDescriptAddTestDISTORT, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::DISTORT;
    EXPECT_NE(filter->Add(rhs), nullptr);
}

/**
 * @tc.name: AddTest002
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, AddTest002, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    EXPECT_EQ(filter->Add(nullptr), filter);
}

/**
 * @tc.name: SubTest001
 * @tc.desc: Verify function Sub
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, SubTest001, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Sub(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::LIGHT_UP_EFFECT;
    EXPECT_NE(filter->Sub(rhs), nullptr);
}

/**
 * @tc.name: SubTestNONE
 * @tc.desc: Verify function Sub
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, SubTestNONE, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Sub(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::NONE;
    EXPECT_NE(filter->Sub(rhs), nullptr);
}

/**
 * @tc.name: SubTestBLUR
 * @tc.desc: Verify function Sub
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, SubTestBLUR, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Sub(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::BLUR;
    EXPECT_NE(filter->Sub(rhs), nullptr);
}

/**
 * @tc.name: SubTestMATERIAL
 * @tc.desc: Verify function Sub
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, SubTestMATERIAL, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Sub(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::MATERIAL;
    EXPECT_NE(filter->Sub(rhs), nullptr);
}

/**
 * @tc.name: SubTestAIBAR
 * @tc.desc: Verify function Sub
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, SubTestAIBAR, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Sub(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::AIBAR;
    EXPECT_NE(filter->Sub(rhs), nullptr);
}

/**
 * @tc.name: SubTestLINEAR_GRADIENT_BLUR
 * @tc.desc: Verify function Sub
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, SubTestLINEAR_GRADIENT_BLUR, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Sub(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::LINEAR_GRADIENT_BLUR;
    EXPECT_NE(filter->Sub(rhs), nullptr);
}

/**
 * @tc.name: SubTestFOREGROUND_EFFECT
 * @tc.desc: Verify function Sub
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, SubTestFOREGROUND_EFFECT, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Sub(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::FOREGROUND_EFFECT;
    EXPECT_NE(filter->Sub(rhs), nullptr);
}

/**
 * @tc.name: SubTestMOTION_BLUR
 * @tc.desc: Verify function Sub
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, SubTestMOTION_BLUR, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Sub(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::MOTION_BLUR;
    EXPECT_NE(filter->Sub(rhs), nullptr);
}

/**
 * @tc.name: SubTestSPHERIZE_EFFECT
 * @tc.desc: Verify function Sub
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, SubTestSPHERIZE_EFFECT, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Sub(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::SPHERIZE_EFFECT;
    EXPECT_NE(filter->Sub(rhs), nullptr);
}

/**
 * @tc.name: SubTestCOLORFUL_SHADOW
 * @tc.desc: Verify function Sub
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, SubTestCOLORFUL_SHADOW, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Sub(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::COLORFUL_SHADOW;
    EXPECT_NE(filter->Sub(rhs), nullptr);
}

/**
 * @tc.name: SubTestATTRACTION_EFFECT
 * @tc.desc: Verify function Sub
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, SubTestATTRACTION_EFFECT, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Sub(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::ATTRACTION_EFFECT;
    EXPECT_NE(filter->Sub(rhs), nullptr);
}

/**
 * @tc.name: SubTestWATER_RIPPLE
 * @tc.desc: Verify function Sub
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, SubTestWATER_RIPPLE, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Sub(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::WATER_RIPPLE;
    EXPECT_NE(filter->Sub(rhs), nullptr);
}

/**
 * @tc.name: SubTestCOMPOUND_EFFECT
 * @tc.desc: Verify function Sub
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, SubTestCOMPOUND_EFFECT, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Sub(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::COMPOUND_EFFECT;
    EXPECT_NE(filter->Sub(rhs), nullptr);
}

/**
 * @tc.name: SubTestMAGNIFIER
 * @tc.desc: Verify function Sub
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, SubTestMAGNIFIER, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Sub(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::MAGNIFIER;
    EXPECT_NE(filter->Sub(rhs), nullptr);
}

/**
 * @tc.name: SubTestFLY_OUT
 * @tc.desc: Verify function Sub
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, SubTestFLY_OUT, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Sub(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::FLY_OUT;
    EXPECT_NE(filter->Sub(rhs), nullptr);
}

/**
 * @tc.name: SubTestDISTORT
 * @tc.desc: Verify function Sub
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, SubTestDISTORT, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto rhs = std::make_shared<RSFilter>();
    EXPECT_NE(filter->Sub(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::DISTORT;
    EXPECT_NE(filter->Sub(rhs), nullptr);
}

/**
 * @tc.name: SubTest002
 * @tc.desc: Verify function Sub
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, SubTest002, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    EXPECT_EQ(filter->Sub(nullptr), filter);
}

/**
 * @tc.name: IsEqualTest001
 * @tc.desc: Verify function Sub
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, IsEqualTest001, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    auto otherFilter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    EXPECT_TRUE(filter->IsEqual(otherFilter));
}

/**
 * @tc.name: MultiplyTest001
 * @tc.desc: Verify function Multiply
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, MultiplyTest001, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    EXPECT_NE(filter->Multiply(1.0f), nullptr);
}

/**
 * @tc.name: NegateTest001
 * @tc.desc: Verify function Negate
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, NegateTest001, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    EXPECT_NE(filter->Negate(), nullptr);
}

/**
 * @tc.name: IsNearEqual001
 * @tc.desc: Verify function IsNearEqual
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, IsNearEqual001, TestSize.Level1)
{
    float lightUpDegree = 1.0f;
    float lightUpDegree1 = 1.2f;
    float threshold = 0.5f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);

    std::shared_ptr<RSFilter> other1 = std::make_shared<RSLightUpEffectFilter>(lightUpDegree1);
    EXPECT_TRUE(filter->IsNearEqual(other1, threshold));
}

/**
 * @tc.name: IsNearZero001
 * @tc.desc: Verify function IsNearZero
 * @tc.type:FUNC
 */
HWTEST_F(RSLightUpEffectFilterTest, IsNearZero001, TestSize.Level1)
{
    float lightUpDegree = 0.2f;
    float threshold = 0.5f;
    auto filter = std::make_shared<RSLightUpEffectFilter>(lightUpDegree);
    EXPECT_TRUE(filter->IsNearZero(threshold));
}
} // namespace OHOS::Rosen
