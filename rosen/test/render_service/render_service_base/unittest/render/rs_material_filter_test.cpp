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

#include "draw/surface.h"
#include "pipeline/rs_paint_filter_canvas.h"
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
 * @tc.name: RadiusVp2SigmaTest001
 * @tc.desc: Verify function RadiusVp2Sigma
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, RadiusVp2SigmaTest001, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::DEFAULT);
    EXPECT_EQ(rsMaterialFilter->RadiusVp2Sigma(0.f, 0.f), 0.0f);
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
    EXPECT_EQ(rsMaterialFilter->GetDescription(),
        "RSMaterialFilter blur radius is " + std::to_string(rsMaterialFilter->radius_) + " sigma");
}

/**
 * @tc.name: ComposeTest001
 * @tc.desc: Verify function Compose
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, ComposeTest001, TestSize.Level1)
{
    MaterialParam materialParam;
    std::shared_ptr<RSDrawingFilter> other = nullptr;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::DEFAULT);
    EXPECT_EQ(rsMaterialFilter->Compose(other), nullptr);
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
    EXPECT_NE(rsMaterialFilter->GetColorFilter(1.f, 1.f), nullptr);
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
    EXPECT_NE(rsMaterialFilter->CreateMaterialFilter(1.f, 1.f, 1.f), nullptr);
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
    MaterialParam materialParamf;
    auto imageSnapshot = std::make_shared<Drawing::Image>();
    auto rsMaterialFilterf = std::make_shared<RSMaterialFilter>(materialParamf, BLUR_COLOR_MODE::AVERAGE);
    rsMaterialFilterf->PreProcess(imageSnapshot);
    EXPECT_EQ(rsMaterialFilterf->colorMode_, BLUR_COLOR_MODE::AVERAGE);
    MaterialParam materialParams;
    auto imageSnapshots = std::make_shared<Drawing::Image>();
    auto rsMaterialFilters = std::make_shared<RSMaterialFilter>(materialParams, BLUR_COLOR_MODE::FASTAVERAGE);
    rsMaterialFilters->PreProcess(imageSnapshots);
    EXPECT_EQ(rsMaterialFilters->colorMode_, BLUR_COLOR_MODE::FASTAVERAGE);
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
 * @tc.name: AddTest001
 * @tc.desc: Verify function Add
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, AddTest001, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rhs = std::make_shared<RSFilter>();
    rhs->type_ = RSDrawingFilter::FilterType::NONE;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
    rhs->type_ = RSDrawingFilter::FilterType::MATERIAL;
    EXPECT_NE(rsMaterialFilter->Add(rhs), nullptr);
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
    rhs->type_ = RSDrawingFilter::FilterType::NONE;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    EXPECT_NE(rsMaterialFilter->Sub(rhs), nullptr);
    rhs->type_ = RSDrawingFilter::FilterType::MATERIAL;
    EXPECT_NE(rsMaterialFilter->Sub(rhs), nullptr);
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
 * @tc.name: GetColorPickerCacheTaskFilterTest001
 * @tc.desc: Verify function GetColorPickerCacheTask
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, GetColorPickerCacheTaskFilterTest001, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::FASTAVERAGE);
    EXPECT_NE(rsMaterialFilter->GetColorPickerCacheTask(), nullptr);
}

/**
 * @tc.name: ReleaseColorPickerFilterTest001
 * @tc.desc: Verify function ReleaseColorPickerFilter
 * @tc.type:FUNC
 */
HWTEST_F(RSMaterialFilterTest, ReleaseColorPickerFilterTest001, TestSize.Level1)
{
    MaterialParam materialParam;
    auto rsMaterialFilter = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::FASTAVERAGE);
    rsMaterialFilter->ReleaseColorPickerFilter();
    auto rsMaterialFilterT = std::make_shared<RSMaterialFilter>(materialParam, BLUR_COLOR_MODE::AVERAGE);
    rsMaterialFilterT->ReleaseColorPickerFilter();
    EXPECT_EQ(rsMaterialFilterT->colorPickerTask_, nullptr);
}
} // namespace OHOS::Rosen
