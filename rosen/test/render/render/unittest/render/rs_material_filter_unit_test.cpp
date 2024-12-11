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

#include "gtest/gtest.h"

#include "parameters.h"
#include "draw/surface.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "render/rs_material_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSMaterialFilterUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSMaterialFilterUnitTest::SetUpTestCase() {}
void RSMaterialFilterUnitTest::TearDownTestCase() {}
void RSMaterialFilterUnitTest::SetUp() {}
void RSMaterialFilterUnitTest::TearDown() {}

/**
 * @tc.name: TestCreateMaterialStyle01
 * @tc.desc: Verify function CreateMaterialStyle
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMaterialFilterUnitTest, TestCreateMaterialStyle01, TestSize.Level1)
{
    float dipScale = 1.0;
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::DEFAULT;
    MATERIAL_BLUR_STYLE style = static_cast<MATERIAL_BLUR_STYLE>(0);
    float ratio = 1.0;
    RSMaterialFilter testFilter(style, dipScale, mode, ratio);
    EXPECT_EQ(testFilter.GetImageFilter(), nullptr);

    style = MATERIAL_BLUR_STYLE::STYLE_CARD_DARK;
    auto testFilter2 = RSMaterialFilter(style, dipScale, mode, ratio);
    EXPECT_NE(testFilter2.GetImageFilter(), nullptr);
}

/**
 * @tc.name: TestCreateMaterialStyle02
 * @tc.desc: Verify function CreateMaterialStyle
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMaterialFilterUnitTest, TestCreateMaterialStyle02, TestSize.Level1)
{
    float dipScale = 1.0f;
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::DEFAULT;
    float ratio = 1.0f;
    MATERIAL_BLUR_STYLE style = MATERIAL_BLUR_STYLE::STYLE_CARD_DARK;

    RSMaterialFilter testFilter = RSMaterialFilter(style, dipScale, mode, ratio);
    EXPECT_NE(testFilter.GetImageFilter(), nullptr);

    testFilter.GetDescription();
    testFilter.PreProcess(nullptr);

    auto testFilter2 = std::make_shared<RSMaterialFilter>(style, dipScale, mode, ratio);
    EXPECT_NE(testFilter2->GetImageFilter(), nullptr);
    auto filter = testFilter.Add(testFilter2);
    EXPECT_TRUE(filter != nullptr);

    filter = testFilter.Sub(testFilter2);
    EXPECT_TRUE(filter != nullptr);

    filter = testFilter.Multiply(1.0f);
    EXPECT_TRUE(filter != nullptr);

    filter = testFilter.Negate();
    EXPECT_TRUE(filter != nullptr);

    auto result = testFilter.Compose(rsMaterialFilter2);
    EXPECT_TRUE(result != nullptr);
}

/**
 * @tc.name: TestRadiusVp2Sigma01
 * @tc.desc: Verify function RadiusVp2Sigma
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMaterialFilterUnitTest, TestRadiusVp2Sigma01, TestSize.Level1)
{
    MaterialParam materialParam;
    auto testFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::DEFAULT);
    EXPECT_EQ(testFilter->RadiusVp2Sigma(0.f, 0.f), 0.0f);
}

/**
 * @tc.name: TestGetDescription01
 * @tc.desc: Verify function GetDescription
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMaterialFilterUnitTest, TestGetDescription01, TestSize.Level1)
{
    MaterialParam materialParam;
    auto testFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::DEFAULT);
    EXPECT_EQ(testFilter->GetDescription(),
        "RSMaterialFilter blur radius is " + std::to_string(testFilter->radius_) + " sigma");
}

/**
 * @tc.name: TestCompose01
 * @tc.desc: Verify function Compose
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMaterialFilterUnitTest, TestCompose01, TestSize.Level1)
{
    MaterialParam materialParam;
    std::shared_ptr<RSDrawingFilterOriginal> other = nullptr;
    auto testFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::DEFAULT);
    EXPECT_EQ(testFilter->Compose(other), nullptr);
}

/**
 * @tc.name: TestGetColorFilter01
 * @tc.desc: Verify function GetColorFilter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMaterialFilterUnitTest, TestGetColorFilter01, TestSize.Level1)
{
    MaterialParam materialParam;
    auto testFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::DEFAULT);
    EXPECT_NE(testFilter->GetColorFilter(1.f, 1.f), nullptr);
}

/**
 * @tc.name: TestCreateMaterialFilter01
 * @tc.desc: Verify function CreateMaterialFilter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMaterialFilterUnitTest, TestCreateMaterialFilter01, TestSize.Level1)
{
    MaterialParam materialParam;
    auto testFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::DEFAULT);
    EXPECT_NE(testFilter->CreateMaterialFilter(1.f, 1.f, 1.f), nullptr);
}

/**
 * @tc.name: TestCreateMaterialStyle01
 * @tc.desc: Verify function CreateMaterialStyle
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMaterialFilterUnitTest, TestCreateMaterialStyle01, TestSize.Level1)
{
    MaterialParam materialParam;
    auto testFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::DEFAULT);
    EXPECT_NE(testFilter->CreateMaterialStyle(MATERIAL_BLUR_STYLE::STYLE_CARD_THIN_LIGHT, 1.f, 1.f), nullptr);
}

/**
 * @tc.name: PreProcessTest001
 * @tc.desc: Verify function PreProcess
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMaterialFilterUnitTest, TestPreProcess01, TestSize.Level1)
{
    MaterialParam materialParam;
    auto imageSnapshot = std::make_shared<Drawing::Image>();
    RSUniRenderJudgement::uniRenderEnabledType_ = UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL;
    auto testFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::FASTAVERAGE);
    testFilter->PreProcess(imageSnapshot);
    EXPECT_EQ(testFilter->colorMode_, BLUR_COLOR_MODE::AVERAGE);
    testFilter->colorMode_ = BLUR_COLOR_MODE::AVERAGE;
    testFilter->PreProcess(imageSnapshot);
    EXPECT_EQ(testFilter->colorMode_, BLUR_COLOR_MODE::AVERAGE);
}

/**
 * @tc.name: TestPostProcess01
 * @tc.desc: Verify function PostProcess
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMaterialFilterUnitTest, TestPostProcess01, TestSize.Level1)
{
    std::unique_ptr<Drawing::Canvas> drawingCanvas = std::make_unique<Drawing::Canvas>(10, 10);
    std::shared_ptr<RSPaintFilterCanvas> canvas = std::make_shared<RSPaintFilterCanvas>(drawingCanvas.get());
    float dipScale = 1.0f;
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::DEFAULT;
    float ratio = 1.0f;
    MATERIAL_BLUR_STYLE style = MATERIAL_BLUR_STYLE::STYLE_CARD_DARK;
    RSMaterialFilter testFilter = RSMaterialFilter(style, dipScale, mode, ratio);
    testFilter.PostProcess(*canvas);
    EXPECT_NE(canvas, nullptr);
}

/**
 * @tc.name: TestTransformFilter01
 * @tc.desc: Verify function TransformFilter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMaterialFilterUnitTest, TestTransformFilter01, TestSize.Level1)
{
    MaterialParam materialParam;
    auto testFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    EXPECT_NE(testFilter->TransformFilter(1.0f), nullptr);
}

/**
 * @tc.name: TestIsValid01
 * @tc.desc: Verify function IsValid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMaterialFilterUnitTest, TestIsValid01, TestSize.Level1)
{
    MaterialParam materialParam;
    materialParam.radius = 1.0f;
    auto testFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    EXPECT_EQ(testFilter->IsValid(), true);
}

/**
 * @tc.name: TestAdd01
 * @tc.desc: Verify function Add
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMaterialFilterUnitTest, TestAdd01, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rhs = std::make_shared<RSFilter>();
    rhs->type_ = RSDrawingFilterOriginal::FilterType::NONE;
    auto testFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    EXPECT_NE(testFilter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::MATERIAL;
    EXPECT_NE(testFilter->Add(rhs), nullptr);
    EXPECT_EQ(testFilter->Add(nullptr), testFilter);
}

/**
 * @tc.name: TestSub01
 * @tc.desc: Verify function Sub
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMaterialFilterUnitTest, TestSub01, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rhs = std::make_shared<RSFilter>();
    rhs->type_ = RSDrawingFilterOriginal::FilterType::NONE;
    auto testFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    EXPECT_NE(testFilter->Sub(rhs), nullptr);
    rhs->type_ = RSDrawingFilterOriginal::FilterType::MATERIAL;
    EXPECT_NE(testFilter->Sub(rhs), nullptr);
    EXPECT_EQ(testFilter->Sub(nullptr), testFilter);
}

/**
 * @tc.name: TestMultiply01
 * @tc.desc: Verify function Multiply
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMaterialFilterUnitTest, TestMultiply01, TestSize.Level1)
{
    MaterialParam materialParam;
    auto testFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    EXPECT_NE(testFilter->Multiply(1.0f), nullptr);
}

/**
 * @tc.name: TestNegate01
 * @tc.desc: Verify function Negate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMaterialFilterUnitTest, TestNegate01, TestSize.Level1)
{
    MaterialParam materialParam;
    auto testFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    EXPECT_NE(testFilter->Negate(), nullptr);
}

/**
 * @tc.name: TestDrawImageRect01
 * @tc.desc: Verify function DrawImageRect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMaterialFilterUnitTest, TestDrawImageRect01, TestSize.Level1)
{
    MaterialParam materialParam;
    auto testFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    Drawing::Canvas canvas(10, 10);
    std::shared_ptr<Drawing::Image> image = nullptr;
    auto imageS = std::make_shared<Drawing::Image>();
    Drawing::Rect src(1.0f, 1.0f, 1.0f, 1.0f);
    Drawing::Rect dst(1.0f, 1.0f, 1.0f, 1.0f);
    Vector2 value(1.f, 1.f);
    std::optional<Vector2f> greyCoef(value);
    testFilter->SetGreyCoef(greyCoef);
    testFilter->DrawImageRect(canvas, imageS, src, dst);
    EXPECT_TRUE(testFilter->greyCoef_.has_value());
}

/**
 * @tc.name: TestSetGreyCoef01
 * @tc.desc: Verify function SetGreyCoef
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMaterialFilterUnitTest, TestSetGreyCoef01, TestSize.Level1)
{
    MaterialParam materialParam;
    auto testFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    Vector2 value(1.f, 1.f);
    std::optional<Vector2f> greyCoef(value);
    testFilter->SetGreyCoef(greyCoef);
    EXPECT_TRUE(testFilter->greyCoef_.has_value());
}

/**
 * @tc.name: TestGetRadiusf01
 * @tc.desc: Verify function GetRadius
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMaterialFilterUnitTest, TestGetRadiusf01, TestSize.Level1)
{
    MaterialParam materialParam;
    materialParam.radius = 1.0f;
    auto testFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    EXPECT_EQ(testFilter->GetRadius(), 1.0f);
}

/**
 * @tc.name: TestCanSkipFrame01
 * @tc.desc: Verify function CanSkipFrame
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMaterialFilterUnitTest, TestCanSkipFrame01, TestSize.Level1)
{
    float dipScale = 1.0f;
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::DEFAULT;
    float ratio = 1.0f;
    MATERIAL_BLUR_STYLE style = MATERIAL_BLUR_STYLE::STYLE_CARD_DARK;
    RSMaterialFilter testFilter = RSMaterialFilter(style, dipScale, mode, ratio);
    auto res = testFilter.CanSkipFrame();
    EXPECT_TRUE(res);
}

/**
 * @tc.name: TestIsNearEqual01
 * @tc.desc: Verify function IsNearEqual
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMaterialFilterUnitTest, TestIsNearEqual01, TestSize.Level1)
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
    std::shared_ptr<RSFilter> testFilter = std::make_shared<RSMaterialFilter>(materialParam, mode);
    float saturation1 = 1.2f;
    struct MaterialParam materialParam1 = { radius, saturation1, brightness, color };
    float threshold = 1.0f;
    std::shared_ptr<RSFilter> testFilte2 = std::make_shared<RSMaterialFilter>(materialParam1, mode);
    EXPECT_TRUE(testFilter->IsNearEqual(testFilte2, threshold));
    EXPECT_TRUE(testFilter->IsNearEqual(nullptr, threshold));
}

/**
 * @tc.name: TestIsNearZero01
 * @tc.desc: Verify function IsNearZero
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMaterialFilterUnitTest, TestIsNearZero01, TestSize.Level1)
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
    std::shared_ptr<RSFilter> testFilter = std::make_shared<RSMaterialFilter>(materialParam, mode);
    float threshold = 1.0f;
    EXPECT_TRUE(testFilter->IsNearZero(threshold));
}

/**
 * @tc.name: TestGetDetailedDescription01
 * @tc.desc: Verify function GetDetailedDescription
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMaterialFilterUnitTest, TestGetDetailedDescription01, TestSize.Level1)
{
    float dipScale = 1.0f;
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::DEFAULT;
    float ratio = 1.0f;
    MATERIAL_BLUR_STYLE style = MATERIAL_BLUR_STYLE::STYLE_CARD_DARK;
    RSMaterialFilter testFilter = RSMaterialFilter(style, dipScale, mode, ratio);

    ASSERT_EQ(testFilter.colorMode_, mode);
    ASSERT_EQ(testFilter.type_, RSDrawingFilterOriginal::FilterType::MATERIAL);
}

/**
 * @tc.name: TestIsEqual01
 * @tc.desc: Verify function IsEqual
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMaterialFilterUnitTest, TestIsEqual01, TestSize.Level1)
{
    float radius = 1.0f;
    float saturation = 1.0f;
    float brightness = 1.0f;
    struct MaterialParam materialParam = { radius, saturation, brightness, RSColor() };
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::DEFAULT;
    std::shared_ptr<RSFilter> testFilter = std::make_shared<RSMaterialFilter>(materialParam, mode);
    std::shared_ptr<RSFilter> testFilter2 = std::make_shared<RSMaterialFilter>(materialParam, mode);
    EXPECT_TRUE(testFilter->IsEqual(testFilter2));
}

/**
 * @tc.name: TestIsEqual02
 * @tc.desc: Verify function IsEqual
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMaterialFilterUnitTest, TestIsEqual02, TestSize.Level1)
{
    float radius = 1.0f;
    float saturation = 1.0f;
    float brightness = 1.0f;
    struct MaterialParam materialParam = { radius, saturation, brightness, RSColor() };
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::DEFAULT;
    std::shared_ptr<RSFilter> testFilter = std::make_shared<RSMaterialFilter>(materialParam, mode);
    EXPECT_TRUE(testFilter->IsEqual(nullptr));
}

/**
 * @tc.name: TestGetDetailedDescription02
 * @tc.desc: Verify function GetDetailedDescription
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSMaterialFilterUnitTest, TestGetDetailedDescription02, TestSize.Level1)
{
    float radius = 1.0f;
    float saturation = 1.0f;
    float brightness = 1.0f;
    struct MaterialParam materialParam = { radius, saturation, brightness, RSColor(255) };
    BLUR_COLOR_MODE mode = BLUR_COLOR_MODE::DEFAULT;
    std::shared_ptr<RSFilter> testFilter = std::make_shared<RSMaterialFilter>(materialParam, mode);
    EXPECT_TRUE(testFilter->GetDetailedDescription().find("RSMaterialFilterBlur, radius: ")!=std::string::npos);
}


} // namespace OHOS::Rosen
