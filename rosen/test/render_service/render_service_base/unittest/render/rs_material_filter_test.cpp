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

#include "parameters.h"
#include "draw/surface.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "render/rs_material_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSMaterialFilterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSMaterialFilterTest::SetUpTestCase() {}
void RSMaterialFilterTest::TearDownTestCase() {}
void RSMaterialFilterTest::SetUp() {}
void RSMaterialFilterTest::TearDown() {}

/**
 * @tc.name: CreateMaterialStyle001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, CreateMaterialStyle001, TestSize.Level1)
{
    float dipScale = 1.0;
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::DEFAULT;
    MATERIAL_BLUR_STYLE style = static_cast<MATERIAL_BLUR_STYLE>(0);
    float ratio = 1.0;
    RSMaterialFilter rsMaterialFilter(style, dipScale, mode, ratio);
    EXPECT_EQ(rsMaterialFilter.GetImageFilter(), nullptr);

    style = MATERIAL_BLUR_STYLE::STYLE_CARD_DARK;
    auto rsMaterialFilter2 = RSMaterialFilter(style, dipScale, mode, ratio);
    EXPECT_NE(rsMaterialFilter2.GetImageFilter(), nullptr);
}

/**
 * @tc.name: CreateMaterialPRE_DEFINED
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, CreateMaterialPRE_DEFINED, TestSize.Level1)
{
    float dipScale = 1.0;
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::PRE_DEFINED;
    MATERIAL_BLUR_STYLE style = static_cast<MATERIAL_BLUR_STYLE>(0);
    float ratio = 1.0;
    RSMaterialFilter rsMaterialFilter(style, dipScale, mode, ratio);
    EXPECT_EQ(rsMaterialFilter.GetImageFilter(), nullptr);

    style = MATERIAL_BLUR_STYLE::STYLE_CARD_DARK;
    auto rsMaterialFilter2 = RSMaterialFilter(style, dipScale, mode, ratio);
    EXPECT_NE(rsMaterialFilter2.GetImageFilter(), nullptr);
}

/**
 * @tc.name: CreateMaterialAVERAGE
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, CreateMaterialAVERAGE, TestSize.Level1)
{
    float dipScale = 1.0;
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::AVERAGE;
    MATERIAL_BLUR_STYLE style = static_cast<MATERIAL_BLUR_STYLE>(0);
    float ratio = 1.0;
    RSMaterialFilter rsMaterialFilter(style, dipScale, mode, ratio);
    EXPECT_EQ(rsMaterialFilter.GetImageFilter(), nullptr);

    style = MATERIAL_BLUR_STYLE::STYLE_CARD_DARK;
    auto rsMaterialFilter2 = RSMaterialFilter(style, dipScale, mode, ratio);
    EXPECT_NE(rsMaterialFilter2.GetImageFilter(), nullptr);
}


/**
 * @tc.name: CreateMaterialFASTAVERAGE
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, CreateMaterialFASTAVERAGE, TestSize.Level1)
{
    float dipScale = 1.0;
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::FASTAVERAGE;
    MATERIAL_BLUR_STYLE style = static_cast<MATERIAL_BLUR_STYLE>(0);
    float ratio = 1.0;
    RSMaterialFilter rsMaterialFilter(style, dipScale, mode, ratio);
    EXPECT_EQ(rsMaterialFilter.GetImageFilter(), nullptr);

    style = MATERIAL_BLUR_STYLE::STYLE_CARD_DARK;
    auto rsMaterialFilter2 = RSMaterialFilter(style, dipScale, mode, ratio);
    EXPECT_NE(rsMaterialFilter2.GetImageFilter(), nullptr);
}

/**
 * @tc.name: CreateMaterialFLT_MAX
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, CreateMaterialFLT_MAX, TestSize.Level1)
{
    float dipScale = FLT_MAX;
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::FASTAVERAGE;
    MATERIAL_BLUR_STYLE style = static_cast<MATERIAL_BLUR_STYLE>(0);
    float ratio = FLT_MAX;
    RSMaterialFilter rsMaterialFilter(style, dipScale, mode, ratio);
    EXPECT_EQ(rsMaterialFilter.GetImageFilter(), nullptr);

    style = MATERIAL_BLUR_STYLE::STYLE_CARD_DARK;
    auto rsMaterialFilter2 = RSMaterialFilter(style, dipScale, mode, ratio);
    EXPECT_NE(rsMaterialFilter2.GetImageFilter(), nullptr);
}

/**
 * @tc.name: CreateMaterialFLT_MAX
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, CreateMaterialFLT_MIN, TestSize.Level1)
{
    float dipScale = FLT_MIN;
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::FASTAVERAGE;
    MATERIAL_BLUR_STYLE style = static_cast<MATERIAL_BLUR_STYLE>(0);
    float ratio = FLT_MIN;
    RSMaterialFilter rsMaterialFilter(style, dipScale, mode, ratio);
    EXPECT_EQ(rsMaterialFilter.GetImageFilter(), nullptr);

    style = MATERIAL_BLUR_STYLE::STYLE_CARD_DARK;
    auto rsMaterialFilter2 = RSMaterialFilter(style, dipScale, mode, ratio);
    EXPECT_NE(rsMaterialFilter2.GetImageFilter(), nullptr);
}

/**
 * @tc.name: CreateMaterialStyleSTYLE_CARD_THIN_LIGHT
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, CreateMaterialStyleSTYLE_CARD_THIN_LIGHT, TestSize.Level1)
{
    float dipScale = 1.0;
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::DEFAULT;
    MATERIAL_BLUR_STYLE style = static_cast<MATERIAL_BLUR_STYLE>(0);
    float ratio = 1.0;
    RSMaterialFilter rsMaterialFilter(style, dipScale, mode, ratio);
    EXPECT_EQ(rsMaterialFilter.GetImageFilter(), nullptr);

    style = MATERIAL_BLUR_STYLE::STYLE_CARD_THIN_LIGHT;
    auto rsMaterialFilter2 = RSMaterialFilter(style, dipScale, mode, ratio);
    EXPECT_NE(rsMaterialFilter2.GetImageFilter(), nullptr);
}

/**
 * @tc.name: CreateMaterialStyleSTYLE_CARD_LIGHT
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, CreateMaterialStyleSTYLE_CARD_LIGHT, TestSize.Level1)
{
    float dipScale = 1.0;
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::DEFAULT;
    MATERIAL_BLUR_STYLE style = static_cast<MATERIAL_BLUR_STYLE>(0);
    float ratio = 1.0;
    RSMaterialFilter rsMaterialFilter(style, dipScale, mode, ratio);
    EXPECT_EQ(rsMaterialFilter.GetImageFilter(), nullptr);

    style = MATERIAL_BLUR_STYLE::STYLE_CARD_LIGHT;
    auto rsMaterialFilter2 = RSMaterialFilter(style, dipScale, mode, ratio);
    EXPECT_NE(rsMaterialFilter2.GetImageFilter(), nullptr);
}

/**
 * @tc.name: CreateMaterialStyleSTYLE_CARD_THIN_DARK
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, CreateMaterialStyleSTYLE_CARD_THIN_DARK, TestSize.Level1)
{
    float dipScale = 1.0;
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::DEFAULT;
    MATERIAL_BLUR_STYLE style = static_cast<MATERIAL_BLUR_STYLE>(0);
    float ratio = 1.0;
    RSMaterialFilter rsMaterialFilter(style, dipScale, mode, ratio);
    EXPECT_EQ(rsMaterialFilter.GetImageFilter(), nullptr);

    style = MATERIAL_BLUR_STYLE::STYLE_CARD_THIN_DARK;
    auto rsMaterialFilter2 = RSMaterialFilter(style, dipScale, mode, ratio);
    EXPECT_NE(rsMaterialFilter2.GetImageFilter(), nullptr);
}

/**
 * @tc.name: CreateMaterialStyleSTYLE_CARD_THICK_DARK
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, CreateMaterialStyleSTYLE_CARD_THICK_DARK, TestSize.Level1)
{
    float dipScale = 1.0;
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::DEFAULT;
    MATERIAL_BLUR_STYLE style = static_cast<MATERIAL_BLUR_STYLE>(0);
    float ratio = 1.0;
    RSMaterialFilter rsMaterialFilter(style, dipScale, mode, ratio);
    EXPECT_EQ(rsMaterialFilter.GetImageFilter(), nullptr);

    style = MATERIAL_BLUR_STYLE::STYLE_CARD_THICK_DARK;
    auto rsMaterialFilter2 = RSMaterialFilter(style, dipScale, mode, ratio);
    EXPECT_NE(rsMaterialFilter2.GetImageFilter(), nullptr);
}

/**
 * @tc.name: CreateMaterialStyle002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, CreateMaterialStyle002, TestSize.Level1)
{
    float dipScale = 1.0f;
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::DEFAULT;
    float ratio = 1.0f;
    MATERIAL_BLUR_STYLE style = MATERIAL_BLUR_STYLE::STYLE_CARD_DARK;

    RSMaterialFilter rsMaterialFilter = RSMaterialFilter(style, dipScale, mode, ratio);
    EXPECT_NE(rsMaterialFilter.GetImageFilter(), nullptr);

    rsMaterialFilter.GetDescription();
    rsMaterialFilter.PreProcess(nullptr);

    auto rsMaterialFilter2 = std::make_shared<RSMaterialFilter>(style, dipScale, mode, ratio);
    EXPECT_NE(rsMaterialFilter2->GetImageFilter(), nullptr);
    auto filter = rsMaterialFilter.Add(rsMaterialFilter2);
    EXPECT_TRUE(filter != nullptr);

    filter = rsMaterialFilter.Sub(rsMaterialFilter2);
    EXPECT_TRUE(filter != nullptr);

    filter = rsMaterialFilter.Multiply(1.0f);
    EXPECT_TRUE(filter != nullptr);

    filter = rsMaterialFilter.Negate();
    EXPECT_TRUE(filter != nullptr);

    auto result = rsMaterialFilter.Compose(rsMaterialFilter2);
    EXPECT_TRUE(result != nullptr);
}

/**
 * @tc.name: CreateMaterialStyle003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, CreateMaterialStyle003, TestSize.Level1)
{
    float dipScale = 1.0f;
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::AVERAGE;
    float ratio = 1.0f;
    MATERIAL_BLUR_STYLE style = MATERIAL_BLUR_STYLE::STYLE_BACKGROUND_XLARGE_DARK;

    RSMaterialFilter rsMaterialFilter = RSMaterialFilter(style, dipScale, mode, ratio);
    EXPECT_NE(rsMaterialFilter.GetImageFilter(), nullptr);

    rsMaterialFilter.GetDescription();
    rsMaterialFilter.PreProcess(nullptr);

    auto rsMaterialFilter2 = std::make_shared<RSMaterialFilter>(style, dipScale, mode, ratio);
    EXPECT_NE(rsMaterialFilter2->GetImageFilter(), nullptr);
    auto filter = rsMaterialFilter.Add(rsMaterialFilter2);
    EXPECT_TRUE(filter != nullptr);

    filter = rsMaterialFilter.Sub(rsMaterialFilter2);
    EXPECT_TRUE(filter != nullptr);

    filter = rsMaterialFilter.Multiply(1.0f);
    EXPECT_TRUE(filter != nullptr);

    filter = rsMaterialFilter.Negate();
    EXPECT_TRUE(filter != nullptr);

    auto result = rsMaterialFilter.Compose(rsMaterialFilter2);
    EXPECT_TRUE(result != nullptr);
}

/**
 * @tc.name: CreateMaterialStyleSTYLE_BACKGROUND_SMALL_LIGHT
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, CreateMaterialStyleSTYLE_BACKGROUND_SMALL_LIGHT, TestSize.Level1)
{
    float dipScale = 1.0f;
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::AVERAGE;
    float ratio = 1.0f;
    MATERIAL_BLUR_STYLE style = MATERIAL_BLUR_STYLE::STYLE_BACKGROUND_SMALL_LIGHT;

    RSMaterialFilter rsMaterialFilter = RSMaterialFilter(style, dipScale, mode, ratio);
    EXPECT_NE(rsMaterialFilter.GetImageFilter(), nullptr);

    rsMaterialFilter.GetDescription();
    rsMaterialFilter.PreProcess(nullptr);

    auto rsMaterialFilter2 = std::make_shared<RSMaterialFilter>(style, dipScale, mode, ratio);
    EXPECT_NE(rsMaterialFilter2->GetImageFilter(), nullptr);
    auto filter = rsMaterialFilter.Add(rsMaterialFilter2);
    EXPECT_TRUE(filter != nullptr);

    filter = rsMaterialFilter.Sub(rsMaterialFilter2);
    EXPECT_TRUE(filter != nullptr);

    filter = rsMaterialFilter.Multiply(1.0f);
    EXPECT_TRUE(filter != nullptr);

    filter = rsMaterialFilter.Negate();
    EXPECT_TRUE(filter != nullptr);

    auto result = rsMaterialFilter.Compose(rsMaterialFilter2);
    EXPECT_TRUE(result != nullptr);
}

/**
 * @tc.name: CreateMaterialStyleSTYLE_BACKGROUND_MEDIUM_LIGHT
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, CreateMaterialStyleSTYLE_BACKGROUND_MEDIUM_LIGHT, TestSize.Level1)
{
    float dipScale = 1.0f;
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::AVERAGE;
    float ratio = 1.0f;
    MATERIAL_BLUR_STYLE style = MATERIAL_BLUR_STYLE::STYLE_BACKGROUND_MEDIUM_LIGHT;

    RSMaterialFilter rsMaterialFilter = RSMaterialFilter(style, dipScale, mode, ratio);
    EXPECT_NE(rsMaterialFilter.GetImageFilter(), nullptr);

    rsMaterialFilter.GetDescription();
    rsMaterialFilter.PreProcess(nullptr);

    auto rsMaterialFilter2 = std::make_shared<RSMaterialFilter>(style, dipScale, mode, ratio);
    EXPECT_NE(rsMaterialFilter2->GetImageFilter(), nullptr);
    auto filter = rsMaterialFilter.Add(rsMaterialFilter2);
    EXPECT_TRUE(filter != nullptr);

    filter = rsMaterialFilter.Sub(rsMaterialFilter2);
    EXPECT_TRUE(filter != nullptr);

    filter = rsMaterialFilter.Multiply(1.0f);
    EXPECT_TRUE(filter != nullptr);

    filter = rsMaterialFilter.Negate();
    EXPECT_TRUE(filter != nullptr);

    auto result = rsMaterialFilter.Compose(rsMaterialFilter2);
    EXPECT_TRUE(result != nullptr);
}

/**
 * @tc.name: CreateMaterialStyleSTYLE_BACKGROUND_LARGE_LIGHT
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, CreateMaterialStyleSTYLE_BACKGROUND_LARGE_LIGHT, TestSize.Level1)
{
    float dipScale = 1.0f;
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::AVERAGE;
    float ratio = 1.0f;
    MATERIAL_BLUR_STYLE style = MATERIAL_BLUR_STYLE::STYLE_BACKGROUND_LARGE_LIGHT;

    RSMaterialFilter rsMaterialFilter = RSMaterialFilter(style, dipScale, mode, ratio);
    EXPECT_NE(rsMaterialFilter.GetImageFilter(), nullptr);

    rsMaterialFilter.GetDescription();
    rsMaterialFilter.PreProcess(nullptr);

    auto rsMaterialFilter2 = std::make_shared<RSMaterialFilter>(style, dipScale, mode, ratio);
    EXPECT_NE(rsMaterialFilter2->GetImageFilter(), nullptr);
    auto filter = rsMaterialFilter.Add(rsMaterialFilter2);
    EXPECT_TRUE(filter != nullptr);

    filter = rsMaterialFilter.Sub(rsMaterialFilter2);
    EXPECT_TRUE(filter != nullptr);

    filter = rsMaterialFilter.Multiply(1.0f);
    EXPECT_TRUE(filter != nullptr);

    filter = rsMaterialFilter.Negate();
    EXPECT_TRUE(filter != nullptr);

    auto result = rsMaterialFilter.Compose(rsMaterialFilter2);
    EXPECT_TRUE(result != nullptr);
}

/**
 * @tc.name: CreateMaterialStyleSTYLE_BACKGROUND_XLARGE_LIGHT
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, CreateMaterialStyleSTYLE_BACKGROUND_XLARGE_LIGHT, TestSize.Level1)
{
    float dipScale = 1.0f;
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::AVERAGE;
    float ratio = 1.0f;
    MATERIAL_BLUR_STYLE style = MATERIAL_BLUR_STYLE::STYLE_BACKGROUND_XLARGE_LIGHT;

    RSMaterialFilter rsMaterialFilter = RSMaterialFilter(style, dipScale, mode, ratio);
    EXPECT_NE(rsMaterialFilter.GetImageFilter(), nullptr);

    rsMaterialFilter.GetDescription();
    rsMaterialFilter.PreProcess(nullptr);

    auto rsMaterialFilter2 = std::make_shared<RSMaterialFilter>(style, dipScale, mode, ratio);
    EXPECT_NE(rsMaterialFilter2->GetImageFilter(), nullptr);
    auto filter = rsMaterialFilter.Add(rsMaterialFilter2);
    EXPECT_TRUE(filter != nullptr);

    filter = rsMaterialFilter.Sub(rsMaterialFilter2);
    EXPECT_TRUE(filter != nullptr);

    filter = rsMaterialFilter.Multiply(1.0f);
    EXPECT_TRUE(filter != nullptr);

    filter = rsMaterialFilter.Negate();
    EXPECT_TRUE(filter != nullptr);

    auto result = rsMaterialFilter.Compose(rsMaterialFilter2);
    EXPECT_TRUE(result != nullptr);
}

/**
 * @tc.name: CreateMaterialStyleSTYLE_BACKGROUND_SMALL_DARK
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, CreateMaterialStyleSTYLE_BACKGROUND_SMALL_DARK, TestSize.Level1)
{
    float dipScale = 1.0f;
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::AVERAGE;
    float ratio = 1.0f;
    MATERIAL_BLUR_STYLE style = MATERIAL_BLUR_STYLE::STYLE_BACKGROUND_SMALL_DARK;

    RSMaterialFilter rsMaterialFilter = RSMaterialFilter(style, dipScale, mode, ratio);
    EXPECT_NE(rsMaterialFilter.GetImageFilter(), nullptr);

    rsMaterialFilter.GetDescription();
    rsMaterialFilter.PreProcess(nullptr);

    auto rsMaterialFilter2 = std::make_shared<RSMaterialFilter>(style, dipScale, mode, ratio);
    EXPECT_NE(rsMaterialFilter2->GetImageFilter(), nullptr);
    auto filter = rsMaterialFilter.Add(rsMaterialFilter2);
    EXPECT_TRUE(filter != nullptr);

    filter = rsMaterialFilter.Sub(rsMaterialFilter2);
    EXPECT_TRUE(filter != nullptr);

    filter = rsMaterialFilter.Multiply(1.0f);
    EXPECT_TRUE(filter != nullptr);

    filter = rsMaterialFilter.Negate();
    EXPECT_TRUE(filter != nullptr);

    auto result = rsMaterialFilter.Compose(rsMaterialFilter2);
    EXPECT_TRUE(result != nullptr);
}

/**
 * @tc.name: CreateMaterialStyleSTYLE_BACKGROUND_MEDIUM_DARK
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, CreateMaterialStyleSTYLE_BACKGROUND_MEDIUM_DARK, TestSize.Level1)
{
    float dipScale = 1.0f;
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::AVERAGE;
    float ratio = 1.0f;
    MATERIAL_BLUR_STYLE style = MATERIAL_BLUR_STYLE::STYLE_BACKGROUND_MEDIUM_DARK;

    RSMaterialFilter rsMaterialFilter = RSMaterialFilter(style, dipScale, mode, ratio);
    EXPECT_NE(rsMaterialFilter.GetImageFilter(), nullptr);

    rsMaterialFilter.GetDescription();
    rsMaterialFilter.PreProcess(nullptr);

    auto rsMaterialFilter2 = std::make_shared<RSMaterialFilter>(style, dipScale, mode, ratio);
    EXPECT_NE(rsMaterialFilter2->GetImageFilter(), nullptr);
    auto filter = rsMaterialFilter.Add(rsMaterialFilter2);
    EXPECT_TRUE(filter != nullptr);

    filter = rsMaterialFilter.Sub(rsMaterialFilter2);
    EXPECT_TRUE(filter != nullptr);

    filter = rsMaterialFilter.Multiply(1.0f);
    EXPECT_TRUE(filter != nullptr);

    filter = rsMaterialFilter.Negate();
    EXPECT_TRUE(filter != nullptr);

    auto result = rsMaterialFilter.Compose(rsMaterialFilter2);
    EXPECT_TRUE(result != nullptr);
}

/**
 * @tc.name: CreateMaterialStyleSTYLE_BACKGROUND_XLARGE_DARK
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, CreateMaterialStyleSTYLE_BACKGROUND_XLARGE_DARK, TestSize.Level1)
{
    float dipScale = 1.0f;
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::AVERAGE;
    float ratio = 1.0f;
    MATERIAL_BLUR_STYLE style = MATERIAL_BLUR_STYLE::STYLE_BACKGROUND_XLARGE_DARK;

    RSMaterialFilter rsMaterialFilter = RSMaterialFilter(style, dipScale, mode, ratio);
    EXPECT_NE(rsMaterialFilter.GetImageFilter(), nullptr);

    rsMaterialFilter.GetDescription();
    rsMaterialFilter.PreProcess(nullptr);

    auto rsMaterialFilter2 = std::make_shared<RSMaterialFilter>(style, dipScale, mode, ratio);
    EXPECT_NE(rsMaterialFilter2->GetImageFilter(), nullptr);
    auto filter = rsMaterialFilter.Add(rsMaterialFilter2);
    EXPECT_TRUE(filter != nullptr);

    filter = rsMaterialFilter.Sub(rsMaterialFilter2);
    EXPECT_TRUE(filter != nullptr);

    filter = rsMaterialFilter.Multiply(1.0f);
    EXPECT_TRUE(filter != nullptr);

    filter = rsMaterialFilter.Negate();
    EXPECT_TRUE(filter != nullptr);

    auto result = rsMaterialFilter.Compose(rsMaterialFilter2);
    EXPECT_TRUE(result != nullptr);
}

/**
 * @tc.name: RadiusVp2SigmaTest001
 * @tc.desc: Verify function RadiusVp2Sigma
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, RadiusVp2SigmaTest001, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::DEFAULT);
    ASSERT_NE(rsMaterialFilter, nullptr);
    EXPECT_EQ(rsMaterialFilter->RadiusVp2Sigma(0.f, 0.f), 0.0f);
}

/**
 * @tc.name: RadiusVp2SigmaTest002
 * @tc.desc: Verify function RadiusVp2Sigma
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, RadiusVp2SigmaTest002, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::DEFAULT);
    ASSERT_NE(rsMaterialFilter, nullptr);
    EXPECT_EQ(rsMaterialFilter->RadiusVp2Sigma(1.f, 1.f), BLUR_SIGMA_SCALE + 0.5f);
}

/**
 * @tc.name: RadiusVp2SigmaTestFLT_MAX
 * @tc.desc: Verify function RadiusVp2Sigma
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, RadiusVp2SigmaTestFLT_MAX, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::DEFAULT);
    ASSERT_NE(rsMaterialFilter, nullptr);
    EXPECT_NE(rsMaterialFilter->RadiusVp2Sigma(FLT_MAX, FLT_MAX), BLUR_SIGMA_SCALE + 0.5f);
}

/**
 * @tc.name: RadiusVp2SigmaTestFLT_MIN
 * @tc.desc: Verify function RadiusVp2Sigma
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, RadiusVp2SigmaTestFLT_MIN, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::DEFAULT);
    ASSERT_NE(rsMaterialFilter, nullptr);
    EXPECT_NE(rsMaterialFilter->RadiusVp2Sigma(FLT_MIN, FLT_MIN), BLUR_SIGMA_SCALE + 0.5f);
}

/**
 * @tc.name: GetDescriptionTest001
 * @tc.desc: Verify function GetDescription
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, GetDescriptionTest001, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::DEFAULT);
    ASSERT_NE(rsMaterialFilter, nullptr);
    EXPECT_EQ(rsMaterialFilter->GetDescription(),
        "RSMaterialFilter blur radius is " + std::to_string(rsMaterialFilter->radius_) + " sigma");
}

/**
 * @tc.name: GetDescriptionTest002
 * @tc.desc: Verify function GetDescription
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, GetDescriptionTest002, TestSize.Level1)
{
    // float radius 1.0 ,float saturation 1.0 ,float brightness 1.0 ,RSColor maskColor 0xffffffff;
    MaterialParam materialParam { 1.0, 1.0, 1.0, RSColor(0xffffffff) };
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::DEFAULT);
    ASSERT_NE(rsMaterialFilter, nullptr);
    EXPECT_EQ(rsMaterialFilter->GetDescription(),
        "RSMaterialFilter blur radius is " + std::to_string(materialParam.radius) + " sigma");
}

/**
 * @tc.name: GetDescriptionTest003
 * @tc.desc: Verify function GetDescription
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, GetDescriptionTest003, TestSize.Level1)
{
    // float radius 1.0 ,float saturation 1.0 ,float brightness 1.0 ,RSColor maskColor 0xffffffff;
    MaterialParam materialParam { 1.0, 1.0, 1.0, RSColor(0xffffffff) };
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::FASTAVERAGE);
    EXPECT_EQ(rsMaterialFilter->GetDescription(),
        "RSMaterialFilter blur radius is " + std::to_string(materialParam.radius) + " sigma");
}


/**
 * @tc.name: ComposeTest001
 * @tc.desc: Verify function Compose
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, ComposeTest001, TestSize.Level1)
{
    MaterialParam materialParam;
    std::shared_ptr<RSDrawingFilterOriginal> other = nullptr;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::DEFAULT);
    ASSERT_NE(rsMaterialFilter, nullptr);
    EXPECT_EQ(rsMaterialFilter->Compose(other), nullptr);
}

/**
 * @tc.name: ComposeTest002
 * @tc.desc: Verify function Compose
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, ComposeTest002, TestSize.Level1)
{
    // float radius 1.0 ,float saturation 1.0 ,float brightness 1.0 ,RSColor maskColor 0xffffffff;
    MaterialParam materialParam { 1.0, 1.0, 1.0, RSColor(0xffffffff) };
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::DEFAULT);
    ASSERT_NE(rsMaterialFilter, nullptr);

    MaterialParam materialParam2 { 2.0, 1.0, 1.0, RSColor(0xff0000ff) };
    auto rsMaterialFilter2 = std::make_shared<RSMaterialFilter>(materialParam2, BLUR_COLOR_MODE::DEFAULT);

    EXPECT_NE(rsMaterialFilter->Compose(rsMaterialFilter2), rsMaterialFilter);
}

/**
 * @tc.name: GetColorFilterTest001
 * @tc.desc: Verify function GetColorFilter
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, GetColorFilterTest001, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::DEFAULT);
    ASSERT_NE(rsMaterialFilter, nullptr);
    EXPECT_NE(rsMaterialFilter->GetColorFilter(1.f, 1.f), nullptr);
}

/**
 * @tc.name: GetColorFilterTestFLT_MAX
 * @tc.desc: Verify function GetColorFilter
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, GetColorFilterTestFLT_MAX, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::DEFAULT);
    ASSERT_NE(rsMaterialFilter, nullptr);
    EXPECT_NE(rsMaterialFilter->GetColorFilter(FLT_MAX, FLT_MAX), nullptr);
}

/**
 * @tc.name: GetColorFilterTestFLT_MIN
 * @tc.desc: Verify function GetColorFilter
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, GetColorFilterTestFLT_MIN, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::DEFAULT);
    ASSERT_NE(rsMaterialFilter, nullptr);
    EXPECT_NE(rsMaterialFilter->GetColorFilter(FLT_MIN, FLT_MIN), nullptr);
}

/**
 * @tc.name: CreateMaterialFilterTest001
 * @tc.desc: Verify function CreateMaterialFilter
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, CreateMaterialFilterTest001, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::DEFAULT);
    ASSERT_NE(rsMaterialFilter, nullptr);
    EXPECT_NE(rsMaterialFilter->CreateMaterialFilter(1.f, 1.f, 1.f), nullptr);
}

/**
 * @tc.name: CreateMaterialFilterTestFLT_MAX
 * @tc.desc: Verify function CreateMaterialFilter
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, CreateMaterialFilterTestFLT_MAX, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::DEFAULT);
    ASSERT_NE(rsMaterialFilter, nullptr);
    EXPECT_NE(rsMaterialFilter->CreateMaterialFilter(FLT_MAX, FLT_MAX, FLT_MAX), nullptr);
}

/**
 * @tc.name: CreateMaterialFilterTestFLT_MIN
 * @tc.desc: Verify function CreateMaterialFilter
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, CreateMaterialFilterTestFLT_MIN, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::DEFAULT);
    ASSERT_NE(rsMaterialFilter, nullptr);
    EXPECT_NE(rsMaterialFilter->CreateMaterialFilter(FLT_MIN, FLT_MIN, FLT_MIN), nullptr);
}

/**
 * @tc.name: CreateMaterialStyleTest001
 * @tc.desc: Verify function CreateMaterialStyle
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, CreateMaterialStyleTest001, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::DEFAULT);
    EXPECT_NE(rsMaterialFilter->CreateMaterialStyle(MATERIAL_BLUR_STYLE::STYLE_CARD_THIN_LIGHT, 1.f, 1.f), nullptr);
}

/**
 * @tc.name: PreProcessTest001
 * @tc.desc: Verify function PreProcess
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, PreProcessTest001, TestSize.Level1)
{
    MaterialParam materialParam;
    auto imageSnapshot = std::make_shared<Drawing::Image>();
    RSUniRenderJudgement::uniRenderEnabledType_ = UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::FASTAVERAGE);
    rsMaterialFilter->PreProcess(imageSnapshot);
    EXPECT_EQ(rsMaterialFilter->colorMode_, BLUR_COLOR_MODE::AVERAGE);
    rsMaterialFilter->colorMode_ = BLUR_COLOR_MODE::AVERAGE;
    rsMaterialFilter->PreProcess(imageSnapshot);
}

/**
 * @tc.name: PreProcessTestUNI_RENDER_DISABLED
 * @tc.desc: Verify function PreProcess
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, PreProcessTestUNI_RENDER_DISABLED, TestSize.Level1)
{
    MaterialParam materialParam;
    auto imageSnapshot = std::make_shared<Drawing::Image>();
    RSUniRenderJudgement::uniRenderEnabledType_ = UniRenderEnabledType::UNI_RENDER_DISABLED;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::FASTAVERAGE);
    ASSERT_NE(rsMaterialFilter, nullptr);
    rsMaterialFilter->PreProcess(imageSnapshot);
    EXPECT_EQ(rsMaterialFilter->colorMode_, BLUR_COLOR_MODE::AVERAGE);
    rsMaterialFilter->PreProcess(imageSnapshot);
}

/**
 * @tc.name: PostProcessTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, PostProcessTest, TestSize.Level1)
{
    std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    float dipScale = 1.0f;
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::DEFAULT;
    float ratio = 1.0f;
    MATERIAL_BLUR_STYLE style = MATERIAL_BLUR_STYLE::STYLE_CARD_DARK;
    RSMaterialFilter rsMaterialFilter = RSMaterialFilter(style, dipScale, mode, ratio);
    rsMaterialFilter.PostProcess(*canvas);
    EXPECT_NE(canvas, nullptr);
}

/**
 * @tc.name: TransformFilterTest001
 * @tc.desc: Verify function TransformFilter
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, TransformFilterTest001, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    EXPECT_NE(rsMaterialFilter->TransformFilter(1.0f), nullptr);
}

/**
 * @tc.name: IsValidTest001
 * @tc.desc: Verify function IsValid
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, IsValidTest001, TestSize.Level1)
{
    MaterialParam materialParam;
    materialParam.radius = 1.0f;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    EXPECT_EQ(rsMaterialFilter->IsValid(), true);
}

/**
 * @tc.name: IsValidTest002
 * @tc.desc: Verify function IsValid
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, IsValidTest002, TestSize.Level1)
{
    MaterialParam materialParam;
    materialParam.radius = 0.1f;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    EXPECT_EQ(rsMaterialFilter->IsValid(), false);
}

/**
 * @tc.name: AddTest001
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, AddTest001, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rhs = std::make_shared<RSFilter>();
    rhs->type_ = RSDrawingFilterOriginal::FilterType::NONE;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::MATERIAL;
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    EXPECT_EQ(rsMaterialFilter->Add(nullptr), rsMaterialFilter);
}

/**
 * @tc.name: AddTestBLUR
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, AddTestBLUR, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rhs = std::make_shared<RSFilter>();
    rhs->type_ = RSDrawingFilterOriginal::FilterType::BLUR;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::MATERIAL;
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    EXPECT_EQ(rsMaterialFilter->Add(nullptr), rsMaterialFilter);
}

/**
 * @tc.name: AddTestMATERIAL
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, AddTestMATERIAL, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rhs = std::make_shared<RSFilter>();
    rhs->type_ = RSDrawingFilterOriginal::FilterType::MATERIAL;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::MATERIAL;
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    EXPECT_EQ(rsMaterialFilter->Add(nullptr), rsMaterialFilter);
}

/**
 * @tc.name: AddTestLIGHT_UP_EFFECT
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, AddTestLIGHT_UP_EFFECT, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rhs = std::make_shared<RSFilter>();
    rhs->type_ = RSDrawingFilterOriginal::FilterType::LIGHT_UP_EFFECT;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::MATERIAL;
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    EXPECT_EQ(rsMaterialFilter->Add(nullptr), rsMaterialFilter);
}

/**
 * @tc.name: AddTestAIBAR
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, AddTestAIBAR, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rhs = std::make_shared<RSFilter>();
    rhs->type_ = RSDrawingFilterOriginal::FilterType::AIBAR;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::MATERIAL;
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    EXPECT_EQ(rsMaterialFilter->Add(nullptr), rsMaterialFilter);
}

/**
 * @tc.name: AddTestLINEAR_GRADIENT_BLUR
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, AddTestLINEAR_GRADIENT_BLUR, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rhs = std::make_shared<RSFilter>();
    rhs->type_ = RSDrawingFilterOriginal::FilterType::LINEAR_GRADIENT_BLUR;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::MATERIAL;
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    EXPECT_EQ(rsMaterialFilter->Add(nullptr), rsMaterialFilter);
}

/**
 * @tc.name: AddTestFOREGROUND_EFFECT
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, AddTestFOREGROUND_EFFECT, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rhs = std::make_shared<RSFilter>();
    rhs->type_ = RSDrawingFilterOriginal::FilterType::FOREGROUND_EFFECT;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::MATERIAL;
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    EXPECT_EQ(rsMaterialFilter->Add(nullptr), rsMaterialFilter);
}

/**
 * @tc.name: AddTestMOTION_BLUR
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, AddTestMOTION_BLUR, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rhs = std::make_shared<RSFilter>();
    rhs->type_ = RSDrawingFilterOriginal::FilterType::MOTION_BLUR;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::MATERIAL;
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    EXPECT_EQ(rsMaterialFilter->Add(nullptr), rsMaterialFilter);
}

/**
 * @tc.name: AddTestSPHERIZE_EFFECT
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, AddTestSPHERIZE_EFFECT, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rhs = std::make_shared<RSFilter>();
    rhs->type_ = RSDrawingFilterOriginal::FilterType::SPHERIZE_EFFECT;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::MATERIAL;
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    EXPECT_EQ(rsMaterialFilter->Add(nullptr), rsMaterialFilter);
}

/**
 * @tc.name: AddTestCOLORFUL_SHADOW
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, AddTestCOLORFUL_SHADOW, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rhs = std::make_shared<RSFilter>();
    rhs->type_ = RSDrawingFilterOriginal::FilterType::COLORFUL_SHADOW;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::MATERIAL;
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    EXPECT_EQ(rsMaterialFilter->Add(nullptr), rsMaterialFilter);
}

/**
 * @tc.name: AddTestATTRACTION_EFFECT
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, AddTestATTRACTION_EFFECT, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rhs = std::make_shared<RSFilter>();
    rhs->type_ = RSDrawingFilterOriginal::FilterType::ATTRACTION_EFFECT;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::MATERIAL;
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    EXPECT_EQ(rsMaterialFilter->Add(nullptr), rsMaterialFilter);
}

/**
 * @tc.name: AddTestWATER_RIPPLE
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, AddTestWATER_RIPPLE, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rhs = std::make_shared<RSFilter>();
    rhs->type_ = RSDrawingFilterOriginal::FilterType::WATER_RIPPLE;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::MATERIAL;
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    EXPECT_EQ(rsMaterialFilter->Add(nullptr), rsMaterialFilter);
}

/**
 * @tc.name: AddTestCOMPOUND_EFFECT
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, AddTestCOMPOUND_EFFECT, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rhs = std::make_shared<RSFilter>();
    rhs->type_ = RSDrawingFilterOriginal::FilterType::COMPOUND_EFFECT;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::MATERIAL;
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    EXPECT_EQ(rsMaterialFilter->Add(nullptr), rsMaterialFilter);
}

/**
 * @tc.name: AddTestMAGNIFIER
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, AddTestMAGNIFIER, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rhs = std::make_shared<RSFilter>();
    rhs->type_ = RSDrawingFilterOriginal::FilterType::MAGNIFIER;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::MATERIAL;
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    EXPECT_EQ(rsMaterialFilter->Add(nullptr), rsMaterialFilter);
}

/**
 * @tc.name: AddTestFLY_OUT
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, AddTestFLY_OUT, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rhs = std::make_shared<RSFilter>();
    rhs->type_ = RSDrawingFilterOriginal::FilterType::FLY_OUT;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::MATERIAL;
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    EXPECT_EQ(rsMaterialFilter->Add(nullptr), rsMaterialFilter);
}

/**
 * @tc.name: AddTestDISTORTDISTORT
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, AddTestDISTORTDISTORT, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rhs = std::make_shared<RSFilter>();
    rhs->type_ = RSDrawingFilterOriginal::FilterType::DISTORT;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::DISTORT;
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    EXPECT_EQ(rsMaterialFilter->Add(nullptr), rsMaterialFilter);
}


/**
 * @tc.name: AddTestBLURFLY_OUT
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, AddTestBLURFLY_OUT, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rhs = std::make_shared<RSFilter>();
    rhs->type_ = RSDrawingFilterOriginal::FilterType::BLUR;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::FLY_OUT;
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    EXPECT_EQ(rsMaterialFilter->Add(nullptr), rsMaterialFilter);
}

/**
 * @tc.name: AddTestLIGHT_UP_EFFECTCOMPOUND_EFFECT
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, AddTestLIGHT_UP_EFFECTCOMPOUND_EFFECT, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rhs = std::make_shared<RSFilter>();
    rhs->type_ = RSDrawingFilterOriginal::FilterType::LIGHT_UP_EFFECT;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::COMPOUND_EFFECT;
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    EXPECT_EQ(rsMaterialFilter->Add(nullptr), rsMaterialFilter);
}

/**
 * @tc.name: AddTestAIBARWATER_RIPPLE
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, AddTestAIBARWATER_RIPPLE, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rhs = std::make_shared<RSFilter>();
    rhs->type_ = RSDrawingFilterOriginal::FilterType::AIBAR;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::WATER_RIPPLE;
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    EXPECT_EQ(rsMaterialFilter->Add(nullptr), rsMaterialFilter);
}

/**
 * @tc.name: AddTestLINEAR_GRADIENT_BLURATTRACTION_EFFECT
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, AddTestLINEAR_GRADIENT_BLURATTRACTION_EFFECT, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rhs = std::make_shared<RSFilter>();
    rhs->type_ = RSDrawingFilterOriginal::FilterType::LINEAR_GRADIENT_BLUR;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::ATTRACTION_EFFECT;
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    EXPECT_EQ(rsMaterialFilter->Add(nullptr), rsMaterialFilter);
}

/**
 * @tc.name: AddTestFOREGROUND_EFFECTCOLORFUL_SHADOW
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, AddTestFOREGROUND_EFFECTCOLORFUL_SHADOW, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rhs = std::make_shared<RSFilter>();
    rhs->type_ = RSDrawingFilterOriginal::FilterType::FOREGROUND_EFFECT;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::COLORFUL_SHADOW;
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    EXPECT_EQ(rsMaterialFilter->Add(nullptr), rsMaterialFilter);
}

/**
 * @tc.name: AddTestMOTION_BLURSPHERIZE_EFFECT
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, AddTestMOTION_BLURSPHERIZE_EFFECT, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rhs = std::make_shared<RSFilter>();
    rhs->type_ = RSDrawingFilterOriginal::FilterType::MOTION_BLUR;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::SPHERIZE_EFFECT;
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    EXPECT_EQ(rsMaterialFilter->Add(nullptr), rsMaterialFilter);
}

/**
 * @tc.name: AddTestSPHERIZE_EFFECTMOTION_BLUR
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, AddTestSPHERIZE_EFFECTMOTION_BLUR, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rhs = std::make_shared<RSFilter>();
    rhs->type_ = RSDrawingFilterOriginal::FilterType::SPHERIZE_EFFECT;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::MOTION_BLUR;
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    EXPECT_EQ(rsMaterialFilter->Add(nullptr), rsMaterialFilter);
}

/**
 * @tc.name: AddTestCOLORFUL_SHADOWMOTION_BLUR
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, AddTestCOLORFUL_SHADOWMOTION_BLUR, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rhs = std::make_shared<RSFilter>();
    rhs->type_ = RSDrawingFilterOriginal::FilterType::COLORFUL_SHADOW;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::MOTION_BLUR;
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    EXPECT_EQ(rsMaterialFilter->Add(nullptr), rsMaterialFilter);
}

/**
 * @tc.name: AddTestATTRACTION_EFFECTFOREGROUND_EFFECT
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, AddTestATTRACTION_EFFECTFOREGROUND_EFFECT, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rhs = std::make_shared<RSFilter>();
    rhs->type_ = RSDrawingFilterOriginal::FilterType::ATTRACTION_EFFECT;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::FOREGROUND_EFFECT;
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    EXPECT_EQ(rsMaterialFilter->Add(nullptr), rsMaterialFilter);
}

/**
 * @tc.name: AddTestWATER_RIPPLELINEAR_GRADIENT_BLUR
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, AddTestWATER_RIPPLELINEAR_GRADIENT_BLUR, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rhs = std::make_shared<RSFilter>();
    rhs->type_ = RSDrawingFilterOriginal::FilterType::WATER_RIPPLE;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::LINEAR_GRADIENT_BLUR;
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    EXPECT_EQ(rsMaterialFilter->Add(nullptr), rsMaterialFilter);
}

/**
 * @tc.name: AddTestCOMPOUND_EFFECTAIBAR
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, AddTestCOMPOUND_EFFECTAIBAR, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rhs = std::make_shared<RSFilter>();
    rhs->type_ = RSDrawingFilterOriginal::FilterType::COMPOUND_EFFECT;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::AIBAR;
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    EXPECT_EQ(rsMaterialFilter->Add(nullptr), rsMaterialFilter);
}

/**
 * @tc.name: AddTestMAGNIFIERLIGHT_UP_EFFECT
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, AddTestMAGNIFIERLIGHT_UP_EFFECT, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rhs = std::make_shared<RSFilter>();
    rhs->type_ = RSDrawingFilterOriginal::FilterType::MAGNIFIER;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::LIGHT_UP_EFFECT;
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    EXPECT_EQ(rsMaterialFilter->Add(nullptr), rsMaterialFilter);
}

/**
 * @tc.name: AddTestFLY_OUTLIGHT_UP_EFFECT
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, AddTestFLY_OUTLIGHT_UP_EFFECT, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rhs = std::make_shared<RSFilter>();
    rhs->type_ = RSDrawingFilterOriginal::FilterType::FLY_OUT;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::LIGHT_UP_EFFECT;
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    EXPECT_EQ(rsMaterialFilter->Add(nullptr), rsMaterialFilter);
}

/**
 * @tc.name: AddTestDISTORTNONE
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, AddTestDISTORTNONE, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rhs = std::make_shared<RSFilter>();
    rhs->type_ = RSDrawingFilterOriginal::FilterType::DISTORT;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::NONE;
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    EXPECT_EQ(rsMaterialFilter->Add(nullptr), rsMaterialFilter);
}

/**
 * @tc.name: SubTest001
 * @tc.desc: Verify function Sub
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, SubTest001, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rhs = std::make_shared<RSFilter>();
    rhs->type_ = RSDrawingFilterOriginal::FilterType::NONE;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    EXPECT_NE(rsMaterialFilter->Sub(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::MATERIAL;
    EXPECT_NE(rsMaterialFilter->Sub(rhs), nullptr);
    EXPECT_EQ(rsMaterialFilter->Sub(nullptr), rsMaterialFilter);
}

/**
 * @tc.name: MultiplyTest001
 * @tc.desc: Verify function Multiply
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, MultiplyTest001, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    EXPECT_NE(rsMaterialFilter->Multiply(1.0f), nullptr);
}

/**
 * @tc.name: NegateTest001
 * @tc.desc: Verify function Negate
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, NegateTest001, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    EXPECT_NE(rsMaterialFilter->Negate(), nullptr);
}

/**
 * @tc.name: DrawImageRectTest001
 * @tc.desc: Verify function DrawImageRect
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, DrawImageRectTest001, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    Drawing::Canvas canvas(10, 10);
    std::shared_ptr<Drawing::Image> image = nullptr;
    auto imageS = std::make_shared<Drawing::Image>();
    Drawing::Rect src(1.0f, 1.0f, 1.0f, 1.0f);
    Drawing::Rect dst(1.0f, 1.0f, 1.0f, 1.0f);
    Vector2 value(1.f, 1.f);
    std::optional<Vector2f> greyCoef(value);
    rsMaterialFilter->SetGreyCoef(greyCoef);
    rsMaterialFilter->DrawImageRect(canvas, imageS, src, dst);
    EXPECT_TRUE(rsMaterialFilter->greyCoef_.has_value());
}

/**
 * @tc.name: SetGreyCoefTest001
 * @tc.desc: Verify function SetGreyCoef
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, SetGreyCoefTest001, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    Vector2 value(1.f, 1.f);
    std::optional<Vector2f> greyCoef(value);
    rsMaterialFilter->SetGreyCoef(greyCoef);
    EXPECT_TRUE(rsMaterialFilter->greyCoef_.has_value());
}

/**
 * @tc.name: GetRadiusfTest001
 * @tc.desc: Verify function GetRadius
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, GetRadiusfTest001, TestSize.Level1)
{
    MaterialParam materialParam;
    materialParam.radius = 1.0f;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    EXPECT_EQ(rsMaterialFilter->GetRadius(), 1.0f);
}

/**
 * @tc.name: CanSkipFrameTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, CanSkipFrameTest, TestSize.Level1)
{
    float dipScale = 1.0f;
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::DEFAULT;
    float ratio = 1.0f;
    MATERIAL_BLUR_STYLE style = MATERIAL_BLUR_STYLE::STYLE_CARD_DARK;
    RSMaterialFilter rsMaterialFilter = RSMaterialFilter(style, dipScale, mode, ratio);
    auto res = rsMaterialFilter.CanSkipFrame();
    EXPECT_TRUE(res);
}

/**
 * @tc.name: CanSkipFrameTest002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, CanSkipFrameTest002, TestSize.Level1)
{
    // float radius 1.0 ,float saturation 1.0 ,float brightness 1.0 ,RSColor maskColor 0xffffffff;
    MaterialParam materialParam { 1.0, 1.0, 1.0, RSColor(0xffffffff) };
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::DEFAULT);
    auto res = rsMaterialFilter->CanSkipFrame();
    EXPECT_FALSE(res);
}

/**
 * @tc.name: IsNearEqual001
 * @tc.desc: Verify function IsNearEqual
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, IsNearEqual001, TestSize.Level1)
{
    int16_t red = 10;
    int16_t green = 11;
    int16_t blue = 12;
    int16_t alpha = 13;
    RSColor color(red, green, blue, alpha);
    float radius = 0.5f;
    float saturation = 1.0f;
    float brightness = 1.0f;
    struct MaterialParam materialParam = { radius, saturation, brightness, color };
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::DEFAULT;
    std::shared_ptr<RSFilter> rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, mode);
    float saturation1 = 1.2f;
    struct MaterialParam materialParam1 = { radius, saturation1, brightness, color };
    float threshold = 1.0f;
    std::shared_ptr<RSFilter> rsMaterialFilter1 = std::make_shared<RSMaterialFilter>(materialParam1, mode);
    EXPECT_TRUE(rsMaterialFilter->IsNearEqual(rsMaterialFilter1, threshold));
    EXPECT_TRUE(rsMaterialFilter->IsNearEqual(nullptr, threshold));
}

/**
 * @tc.name: IsNearZero001
 * @tc.desc: Verify function IsNearZero
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, IsNearZero001, TestSize.Level1)
{
    int16_t red = 10;
    int16_t green = 11;
    int16_t blue = 12;
    int16_t alpha = 13;
    RSColor color(red, green, blue, alpha);
    float radius = 0.5f;
    float saturation = 1.0f;
    float brightness = 1.0f;
    struct MaterialParam materialParam = { radius, saturation, brightness, color };
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::DEFAULT;
    std::shared_ptr<RSFilter> rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, mode);
    float threshold = 1.0f;
    EXPECT_TRUE(rsMaterialFilter->IsNearZero(threshold));
}

/**
 * @tc.name: GetDetailedDescription001
 * @tc.desc: Verify function GetDetailedDescription
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, GetDetailedDescription001, TestSize.Level1)
{
    float dipScale = 1.0f;
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::DEFAULT;
    float ratio = 1.0f;
    MATERIAL_BLUR_STYLE style = MATERIAL_BLUR_STYLE::STYLE_CARD_DARK;
    RSMaterialFilter rsMaterialFilter = RSMaterialFilter(style, dipScale, mode, ratio);

    ASSERT_EQ(rsMaterialFilter.colorMode_, mode);
    ASSERT_EQ(rsMaterialFilter.type_, RSDrawingFilterOriginal::FilterType::MATERIAL);
}

/**
 * @tc.name: IsEqual001
 * @tc.desc: Verify function IsEqual
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, IsEqual001, TestSize.Level1)
{
    float radius = 1.0f;
    float saturation = 1.0f;
    float brightness = 1.0f;
    struct MaterialParam materialParam = { radius, saturation, brightness, RSColor() };
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::DEFAULT;
    std::shared_ptr<RSFilter> rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, mode);
    std::shared_ptr<RSFilter> rsMaterialFilterOhter = std::make_shared<RSMaterialFilter>(materialParam, mode);
    EXPECT_TRUE(rsMaterialFilter->IsEqual(rsMaterialFilterOhter));
}

/**
 * @tc.name: IsEqual002
 * @tc.desc: Verify function IsEqual
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, IsEqual002, TestSize.Level1)
{
    float radius = 1.0f;
    float saturation = 1.0f;
    float brightness = 1.0f;
    struct MaterialParam materialParam = { radius, saturation, brightness, RSColor() };
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::DEFAULT;
    std::shared_ptr<RSFilter> rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, mode);
    EXPECT_TRUE(rsMaterialFilter->IsEqual(nullptr));
}

/**
 * @tc.name: GetDetailedDescription002
 * @tc.desc: Verify function GetDetailedDescription
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, GetDetailedDescription002, TestSize.Level1)
{
    float radius = 1.0f;
    float saturation = 1.0f;
    float brightness = 1.0f;
    struct MaterialParam materialParam = { radius, saturation, brightness, RSColor(255) };
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::DEFAULT;
    std::shared_ptr<RSFilter> rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, mode);
    EXPECT_TRUE(rsMaterialFilter->GetDetailedDescription().find("RSMaterialFilterBlur, radius: ")!=std::string::npos);
}

/**
 * @tc.name: GetDetailedDescription003
 * @tc.desc: Verify function GetDetailedDescription
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, GetDetailedDescription003, TestSize.Level1)
{
    float dipScale = 1.0f;
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::AVERAGE;
    float ratio = 1.0f;
    MATERIAL_BLUR_STYLE style = MATERIAL_BLUR_STYLE::STYLE_CARD_DARK;
    RSMaterialFilter rsMaterialFilter = RSMaterialFilter(style, dipScale, mode, ratio);

    EXPECT_EQ(rsMaterialFilter.GetDetailedDescription(),
        "RSMaterialFilterBlur, radius: 29.367500 sigma, saturation: 2.150000, brightness: 1.000000, greyCoef1: "
        "0.000000, greyCoef2: 0.000000, color: D11F1F1F, colorMode: 1");
}


} // namespace OHOS::Rosen
