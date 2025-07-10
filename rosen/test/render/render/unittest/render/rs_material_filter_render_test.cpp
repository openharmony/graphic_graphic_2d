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

#include "parameters.h"
#include "draw/surface.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "render/rs_material_filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSMaterialFilterRenderTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSMaterialFilterRenderTest::SetUpTestCase() {}
void RSMaterialFilterRenderTest::TearDownTestCase() {}
void RSMaterialFilterRenderTest::SetUp() {}
void RSMaterialFilterRenderTest::TearDown() {}

/**
 * @tc.name: TestCreateMaterialStyle001
 * @tc.desc: Verify function CreateMaterialStyle
 * @tc.type: FUNC
 */
HWTEST_F(RSMaterialFilterRenderTest, TestCreateMaterialStyle001, TestSize.Level1)
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
 * @tc.name: TestCreateMaterialStyle002
 * @tc.desc: Verify function CreateMaterialStyle
 * @tc.type: FUNC
 */
HWTEST_F(RSMaterialFilterRenderTest, TestCreateMaterialStyle002, TestSize.Level1)
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

    auto result = rsMaterialFilter.Compose(rsMaterialFilter2);
    EXPECT_TRUE(result != nullptr);
}

/**
 * @tc.name: TestCreateMaterialStyle003
 * @tc.desc: Verify function CreateMaterialStyle
 * @tc.type: FUNC
 */
HWTEST_F(RSMaterialFilterRenderTest, TestCreateMaterialStyle003, TestSize.Level1)
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

    auto result = rsMaterialFilter.Compose(rsMaterialFilter2);
    EXPECT_TRUE(result != nullptr);
}

/**
 * @tc.name: TestRadiusVp2Sigma001
 * @tc.desc: Verify function RadiusVp2Sigma
 * @tc.type: FUNC
 */
HWTEST_F(RSMaterialFilterRenderTest, TestRadiusVp2Sigma001, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::DEFAULT);
    EXPECT_EQ(rsMaterialFilter->RadiusVp2Sigma(0.f, 0.f), 0.0f);
}

/**
 * @tc.name: TestRadiusVp2Sigma002
 * @tc.desc: Verify function RadiusVp2Sigma
 * @tc.type: FUNC
 */
HWTEST_F(RSMaterialFilterRenderTest, TestRadiusVp2Sigma002, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::DEFAULT);
    EXPECT_EQ(rsMaterialFilter->RadiusVp2Sigma(1.f, 1.f), BLUR_SIGMA_SCALE + 0.5f);
}

/**
 * @tc.name: TestGetDescription001
 * @tc.desc: Verify function GetDescription
 * @tc.type: FUNC
 */
HWTEST_F(RSMaterialFilterRenderTest, TestGetDescription001, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::DEFAULT);
    EXPECT_EQ(rsMaterialFilter->GetDescription(),
        "RSMaterialFilter blur radius is " + std::to_string(rsMaterialFilter->radius_) + " sigma");
}

/**
 * @tc.name: TestGetDescription002
 * @tc.desc: Verify function GetDescription
 * @tc.type: FUNC
 */
HWTEST_F(RSMaterialFilterRenderTest, TestGetDescription002, TestSize.Level1)
{
    // float radius 1.0 ,float saturation 1.0 ,float brightness 1.0 ,RSColor maskColor 0xffffffff;
    MaterialParam materialParam { 1.0, 1.0, 1.0, RSColor(0xffffffff) };
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::DEFAULT);
    EXPECT_EQ(rsMaterialFilter->GetDescription(),
        "RSMaterialFilter blur radius is " + std::to_string(materialParam.radius) + " sigma");
}

/**
 * @tc.name: TestGetDescription003
 * @tc.desc: Verify function GetDescription
 * @tc.type: FUNC
 */
HWTEST_F(RSMaterialFilterRenderTest, TestGetDescription003, TestSize.Level1)
{
    // float radius 1.0 ,float saturation 1.0 ,float brightness 1.0 ,RSColor maskColor 0xffffffff;
    MaterialParam materialParam { 1.0, 1.0, 1.0, RSColor(0xffffffff) };
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::FASTAVERAGE);
    EXPECT_EQ(rsMaterialFilter->GetDescription(),
        "RSMaterialFilter blur radius is " + std::to_string(materialParam.radius) + " sigma");
}


/**
 * @tc.name: TestCompose001
 * @tc.desc: Verify function Compose
 * @tc.type: FUNC
 */
HWTEST_F(RSMaterialFilterRenderTest, TestCompose001, TestSize.Level1)
{
    MaterialParam materialParam;
    std::shared_ptr<RSDrawingFilterOriginal> other = nullptr;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::DEFAULT);
    EXPECT_EQ(rsMaterialFilter->Compose(other), nullptr);
}

/**
 * @tc.name: TestCompose002
 * @tc.desc: Verify function Compose
 * @tc.type: FUNC
 */
HWTEST_F(RSMaterialFilterRenderTest, TestCompose002, TestSize.Level1)
{
    // float radius 1.0 ,float saturation 1.0 ,float brightness 1.0 ,RSColor maskColor 0xffffffff;
    MaterialParam materialParam { 1.0, 1.0, 1.0, RSColor(0xffffffff) };
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::DEFAULT);

    MaterialParam materialParam2 { 2.0, 1.0, 1.0, RSColor(0xff0000ff) };
    auto rsMaterialFilter2 = std::make_shared<RSMaterialFilter>(materialParam2, BLUR_COLOR_MODE::DEFAULT);

    EXPECT_NE(rsMaterialFilter->Compose(rsMaterialFilter2), rsMaterialFilter);
}


/**
 * @tc.name: TestGetColorFilter001
 * @tc.desc: Verify function GetColorFilter
 * @tc.type: FUNC
 */
HWTEST_F(RSMaterialFilterRenderTest, TestGetColorFilter001, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::DEFAULT);
    EXPECT_NE(rsMaterialFilter->GetColorFilter(1.f, 1.f), nullptr);
}

/**
 * @tc.name: TestCreateMaterialFilter001
 * @tc.desc: Verify function CreateMaterialFilter
 * @tc.type: FUNC
 */
HWTEST_F(RSMaterialFilterRenderTest, TestCreateMaterialFilter001, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::DEFAULT);
    EXPECT_NE(rsMaterialFilter->CreateMaterialFilter(1.f, 1.f, 1.f), nullptr);
}

/**
 * @tc.name: TestCreateMaterialStyle001
 * @tc.desc: Verify function CreateMaterialStyle
 * @tc.type: FUNC
 */
HWTEST_F(RSMaterialFilterRenderTest, TestCreateMaterialStyle001, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::DEFAULT);
    EXPECT_NE(rsMaterialFilter->CreateMaterialStyle(MATERIAL_BLUR_STYLE::STYLE_CARD_THIN_LIGHT, 1.f, 1.f), nullptr);
}

/**
 * @tc.name: TestPreProcess001
 * @tc.desc: Verify function PreProcess
 * @tc.type: FUNC
 */
HWTEST_F(RSMaterialFilterRenderTest, TestPreProcess001, TestSize.Level1)
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
 * @tc.name: TestPostProcess
 * @tc.desc: Verify function PostProcess
 * @tc.type: FUNC
 */
HWTEST_F(RSMaterialFilterRenderTest, TestPostProcess, TestSize.Level1)
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
 * @tc.name: TestTransformFilter001
 * @tc.desc: Verify function TransformFilter
 * @tc.type: FUNC
 */
HWTEST_F(RSMaterialFilterRenderTest, TestTransformFilter001, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    EXPECT_NE(rsMaterialFilter->TransformFilter(1.0f), nullptr);
}

/**
 * @tc.name: TestIsValid001
 * @tc.desc: Verify function IsValid
 * @tc.type: FUNC
 */
HWTEST_F(RSMaterialFilterRenderTest, TestIsValid001, TestSize.Level1)
{
    MaterialParam materialParam;
    materialParam.radius = 1.0f;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    EXPECT_EQ(rsMaterialFilter->IsValid(), true);
}

/**
 * @tc.name: TestIsValid002
 * @tc.desc: Verify function IsValid
 * @tc.type: FUNC
 */
HWTEST_F(RSMaterialFilterRenderTest, TestIsValid002, TestSize.Level1)
{
    MaterialParam materialParam;
    materialParam.radius = 0.1f;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    EXPECT_EQ(rsMaterialFilter->IsValid(), false);
}

/**
 * @tc.name: TestAdd001
 * @tc.desc: Verify function Add
 * @tc.type: FUNC
 */
HWTEST_F(RSMaterialFilterRenderTest, TestAdd001, TestSize.Level1)
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
 * @tc.name: TestSub001
 * @tc.desc: Verify function Sub
 * @tc.type: FUNC
 */
HWTEST_F(RSMaterialFilterRenderTest, TestSub001, TestSize.Level1)
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
 * @tc.name: TestMultiply001
 * @tc.desc: Verify function Multiply
 * @tc.type: FUNC
 */
HWTEST_F(RSMaterialFilterRenderTest, TestMultiply001, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    EXPECT_NE(rsMaterialFilter->Multiply(1.0f), nullptr);
}

/**
 * @tc.name: TestNegate001
 * @tc.desc: Verify function Negate
 * @tc.type: FUNC
 */
HWTEST_F(RSMaterialFilterRenderTest, TestNegate001, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    EXPECT_NE(rsMaterialFilter->Negate(), nullptr);
}

/**
 * @tc.name: TestDrawImageRect001
 * @tc.desc: Verify function DrawImageRect
 * @tc.type: FUNC
 */
HWTEST_F(RSMaterialFilterRenderTest, TestDrawImageRect001, TestSize.Level1)
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
 * @tc.name: TestSetGreyCoef001
 * @tc.desc: Verify function SetGreyCoef
 * @tc.type: FUNC
 */
HWTEST_F(RSMaterialFilterRenderTest, TestSetGreyCoef001, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    Vector2 value(1.f, 1.f);
    std::optional<Vector2f> greyCoef(value);
    rsMaterialFilter->SetGreyCoef(greyCoef);
    EXPECT_TRUE(rsMaterialFilter->greyCoef_.has_value());
}

/**
 * @tc.name: TestGetRadiusf001
 * @tc.desc: Verify function GetRadius
 * @tc.type: FUNC
 */
HWTEST_F(RSMaterialFilterRenderTest, TestGetRadiusf001, TestSize.Level1)
{
    MaterialParam materialParam;
    materialParam.radius = 1.0f;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    EXPECT_EQ(rsMaterialFilter->GetRadius(), 1.0f);
}

/**
 * @tc.name: TestCanSkipFrame
 * @tc.desc: Verify function CanSkipFrame
 * @tc.type: FUNC
 */
HWTEST_F(RSMaterialFilterRenderTest, TestCanSkipFrame, TestSize.Level1)
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
 * @tc.name: TestCanSkipFrame002
 * @tc.desc: Verify function CanSkipFrame
 * @tc.type: FUNC
 */
HWTEST_F(RSMaterialFilterRenderTest, TestCanSkipFrame002, TestSize.Level1)
{
    // float radius 1.0 ,float saturation 1.0 ,float brightness 1.0 ,RSColor maskColor 0xffffffff;
    MaterialParam materialParam { 1.0, 1.0, 1.0, RSColor(0xffffffff) };
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::DEFAULT);
    auto res = rsMaterialFilter->CanSkipFrame();
    EXPECT_FALSE(res);
}

/**
 * @tc.name: TestIsNearEqual001
 * @tc.desc: Verify function IsNearEqual
 * @tc.type: FUNC
 */
HWTEST_F(RSMaterialFilterRenderTest, TestIsNearEqual001, TestSize.Level1)
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
 * @tc.name: TestIsNearZero001
 * @tc.desc: Verify function IsNearZero
 * @tc.type: FUNC
 */
HWTEST_F(RSMaterialFilterRenderTest, TestIsNearZero001, TestSize.Level1)
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
 * @tc.name: TestGetDetailedDescription001
 * @tc.desc: Verify function GetDetailedDescription
 * @tc.type: FUNC
 */
HWTEST_F(RSMaterialFilterRenderTest, TestGetDetailedDescription001, TestSize.Level1)
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
 * @tc.name: TestIsEqual001
 * @tc.desc: Verify function IsEqual
 * @tc.type: FUNC
 */
HWTEST_F(RSMaterialFilterRenderTest, TestIsEqual001, TestSize.Level1)
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
 * @tc.name: TestIsEqual002
 * @tc.desc: Verify function IsEqual
 * @tc.type: FUNC
 */
HWTEST_F(RSMaterialFilterRenderTest, TestIsEqual002, TestSize.Level1)
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
 * @tc.name: TestGetDetailedDescription002
 * @tc.desc: Verify function GetDetailedDescription
 * @tc.type: FUNC
 */
HWTEST_F(RSMaterialFilterRenderTest, TestGetDetailedDescription002, TestSize.Level1)
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
 * @tc.name: TestGetDetailedDescription003
 * @tc.desc: Verify function GetDetailedDescription
 * @tc.type: FUNC
 */
HWTEST_F(RSMaterialFilterRenderTest, TestGetDetailedDescription003, TestSize.Level1)
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