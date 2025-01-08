/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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
#include "include/render/rs_border.h"

#include "property/rs_properties.h"
#include "property/rs_properties_painter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSBorderTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSBorderTest::SetUpTestCase() {}
void RSBorderTest::TearDownTestCase() {}
void RSBorderTest::SetUp() {}
void RSBorderTest::TearDown() {}

/**
 * @tc.name: LifeCycle001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSBorderTest, LifeCycle001, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSMask by Gradient
     */
    RSColor color(1, 1, 1);
    std::shared_ptr<RSBorder> border = std::make_shared<RSBorder>();
    ASSERT_TRUE(border->GetColor(0) == RgbPalette::Transparent());
    ASSERT_TRUE(border->GetStyle(0) == BorderStyle::NONE);
    border->SetColor(color);
    border->GetColor(0);
    RSColor color2(1, 1, 1);
    border->SetColor(color2);
    border->GetColor(1);
    border->SetWidth(1.f);
    border->SetWidth(2.f);
    ASSERT_TRUE(border->GetWidth(1) == 2.f);
    border->SetStyle(BorderStyle::DOTTED);
    border->SetStyle(BorderStyle::DOTTED);
    ASSERT_TRUE(border->GetStyle(1) == BorderStyle::DOTTED);
}

/**
 * @tc.name: LifeCycle002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSBorderTest, LifeCycle002, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSMask by Gradient
     */
    Color color1(1, 1, 1);
    Color color2(2, 2, 2);
    Color color3(3, 3, 3);
    Color color4(4, 4, 4);
    Vector4<Color> vectorColor(color1, color2, color3, color4);
    std::shared_ptr<RSBorder> border = std::make_shared<RSBorder>();
    border->GetColorFour();
    border->SetColorFour(vectorColor);
    border->GetColorFour();
    Vector4f width(1, 1, 1, 1);
    border->SetWidthFour(width);
    Vector4f width2(1, 2, 3, 4);
    border->SetWidthFour(width2);
    border->GetWidthFour();
    Vector4<uint32_t> vectorStyle(1, 2, 3, 4);
    border->GetStyleFour();
    border->SetStyleFour(vectorStyle);
    border->GetStyleFour();
    border->SetStyle(BorderStyle::DOTTED);
    EXPECT_EQ(
        border->ToString(), "colors: 4278255873, 4278321666, 4278387459, 4278453252, widths: 1, 2, 3, 4, styles: 2, ");
}

/**
 * @tc.name: LifeCycle003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSBorderTest, LifeCycle003, TestSize.Level1)
{
    /**
     * @tc.steps: step1. create RSMask by Gradient
     */
    Color color1(1, 1, 1);
    Color color2(1, 1, 1);
    Color color3(1, 1, 1);
    Color color4(1, 1, 1);
    Vector4<Color> vectorColor(color1, color2, color3, color4);
    RSProperties properties;
    Drawing::Pen pen;
    pen.SetAntiAlias(true);
    properties.SetBorderColor(vectorColor);
    Vector4<uint32_t> style(0, 0, 0, 0);
    properties.SetBorderStyle(style);
    Vector4f cornerRadius(0.f, 0.f, 0.f, 0.f);
    properties.SetCornerRadius(cornerRadius);
    Vector4f width(1.f, 1.f, 1.f, 1.f);
    properties.SetBorderWidth(width);
    properties.SetBorderDashWidth(width);
    properties.SetBorderDashGap(width);
    properties.SetOutlineDashWidth(width);
    properties.SetOutlineDashGap(width);
    ASSERT_TRUE(properties.GetCornerRadius().IsZero());
    Drawing::Canvas canvas;
    RSPropertiesPainter::DrawBorder(properties, canvas);
    Vector4f cornerRadius2(1.f, 2.f, 0.f, 0.f);
    properties.SetCornerRadius(cornerRadius2);
    RSPropertiesPainter::DrawBorder(properties, canvas);
    Vector4<uint32_t> style2(1, 1, 1, 1);
    properties.SetBorderStyle(style2);
    RSPropertiesPainter::DrawBorder(properties, canvas);
    Color color5(2, 1, 1);
    Vector4<Color> Color(color1, color2, color3, color5);
    properties.SetBorderColor(Color);
    RSPropertiesPainter::DrawBorder(properties, canvas);
}

/**
 * @tc.name: HasBorderTest
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSBorderTest, HasBorderTest, TestSize.Level1)
{
    Color blackColor(0, 0, 0);
    std::shared_ptr<RSBorder> border = std::make_shared<RSBorder>();
    border->SetColor(blackColor);
    border->GetColor(1);
    border->SetStyle(BorderStyle::DOTTED);
    border->SetWidth(2.f);
    bool hasBorder = border->HasBorder();
    ASSERT_TRUE(hasBorder);
}

/**
 * @tc.name: SetColorTest
 * @tc.desc: Verify function SetColor
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBorderTest, SetColorTest, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>();
    border->SetColor(Color(0, 0, 0));
    EXPECT_EQ(border->GetColor(0), Color(0, 0, 0));
}

/**
 * @tc.name: SetWidthTest
 * @tc.desc: Verify function SetWidth
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBorderTest, SetWidthTest, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>();
    border->SetWidth(1.0f);
    EXPECT_EQ(border->GetWidth(0), 1.0f);
}

/**
 * @tc.name: SetStyleTest
 * @tc.desc: Verify function SetStyle
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBorderTest, SetStyleTest, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>();
    border->SetStyle(BorderStyle::SOLID);
    EXPECT_TRUE(border->GetStyle(0) == BorderStyle::SOLID);
}

/**
 * @tc.name: GetColorTest
 * @tc.desc: Verify function GetColor
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBorderTest, GetColorTest, TestSize.Level1)
{
    // for test
    Color redColor(255, 0, 0);
    Color blackColor(0, 0, 0);
    Color whiteColor(255, 255, 255);
    Color greenColor(0, 255, 0);
    auto border = std::make_shared<RSBorder>();
    RSColor val { 0, 0, 0, 0 };
    EXPECT_TRUE(border->GetColor(0) == val);

    border->SetColor(redColor);
    EXPECT_TRUE(border->GetColor(0) == redColor);

    Vector4<Color> vectorColor(redColor, blackColor, whiteColor, greenColor);
    border->SetColorFour(vectorColor);
    EXPECT_TRUE(border->GetColor(1) == blackColor);
}

/**
 * @tc.name: GetWidthTest
 * @tc.desc: Verify function GetWidth
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBorderTest, GetWidthTest, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>();
    EXPECT_EQ(border->GetWidth(0), 0.f);

    border->SetWidth(1.f);
    EXPECT_EQ(border->GetWidth(0), 1.f);
    // for test
    Vector4f widths(5.0f, 10.0f, 5.0f, 10.0f);
    border->SetWidthFour(widths);
    EXPECT_EQ(border->GetWidth(1), 10.0f);
}

/**
 * @tc.name: GetStyleTest
 * @tc.desc: Verify function GetStyle
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBorderTest, GetStyleTest, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>();
    EXPECT_TRUE(border->GetStyle(0) == BorderStyle::NONE);

    border->SetStyle(BorderStyle::SOLID);
    EXPECT_TRUE(border->GetStyle(0) == BorderStyle::SOLID);
    // for test
    Vector4<uint32_t> vectorStyle(0, 1, 2, 3);
    border->SetStyleFour(vectorStyle);
    EXPECT_TRUE(border->GetStyle(0) == BorderStyle::SOLID);
}

/**
 * @tc.name: SetColorFourTest
 * @tc.desc: Verify function SetColorFour
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBorderTest, SetColorFourTest, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>();
    // for test
    Color redColor(255, 0, 0);
    Color blackColor(0, 0, 0);
    Color whiteColor(255, 255, 255);
    Color greenColor(0, 255, 0);
    Vector4<Color> vectorColor1(redColor, blackColor, whiteColor, greenColor);
    border->SetColorFour(vectorColor1);
    EXPECT_TRUE(border->GetColorFour() == vectorColor1);
    Vector4<Color> vectorColor2(redColor, redColor, redColor, redColor);
    border->SetColorFour(vectorColor2);
    EXPECT_TRUE(border->GetColor() == redColor);
}

/**
 * @tc.name: SetWidthFourTest
 * @tc.desc: Verify function SetWidthFour
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBorderTest, SetWidthFourTest, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>();
    // for test
    Vector4f initialBorderWidths(5.0f, 10.0f, 5.0f, 10.0f);
    border->SetWidthFour(initialBorderWidths);
    EXPECT_EQ(border->GetWidthFour(), initialBorderWidths);
    // for test
    Vector4f uniformBorderWidth(8.0f, 8.0f, 8.0f, 8.0f);
    border->SetWidthFour(uniformBorderWidth);
    EXPECT_EQ(border->GetWidthFour(), 8.0f);
}

/**
 * @tc.name: SetStyleFourTest
 * @tc.desc: Verify function SetStyleFour
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBorderTest, SetStyleFourTest, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>();
    // for test
    Vector4<uint32_t> vectorStyle(0, 1, 2, 3);
    border->SetStyleFour(vectorStyle);
    EXPECT_TRUE(border->GetStyleFour() == vectorStyle);

    Vector4<uint32_t> vectorStyle1(0, 0, 0, 0);
    border->SetStyleFour(vectorStyle1);
    EXPECT_TRUE(border->GetStyle() == BorderStyle::SOLID);
}

/**
 * @tc.name: SetRadiusFourTest
 * @tc.desc: Verify function SetRadiusFour
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBorderTest, SetRadiusFourTest, TestSize.Level1)
{
    // for test
    Vector4f radius(5.0f, 10.0f, 5.0f, 10.0f);
    auto border = std::make_shared<RSBorder>();
    border->SetRadiusFour(radius);
    EXPECT_TRUE(border->GetRadiusFour() == radius);
}

/**
 * @tc.name: GetColorFourTest
 * @tc.desc: Verify function GetColorFour
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBorderTest, GetColorFourTest, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>();
    // for test
    Color redColor(255, 0, 0);
    Color blackColor(0, 0, 0);
    Color whiteColor(255, 255, 255);
    Color greenColor(0, 255, 0);
    Vector4<Color> vectorColor1(redColor, blackColor, whiteColor, greenColor);
    border->SetColorFour(vectorColor1);
    EXPECT_TRUE(border->GetColorFour() == vectorColor1);

    Vector4<Color> vectorColor2(redColor, redColor, redColor, redColor);
    border->SetColorFour(vectorColor2);
    EXPECT_TRUE(border->GetColor() == redColor);
}

/**
 * @tc.name: GetWidthFourTest
 * @tc.desc: Verify function GetWidthFour
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBorderTest, GetWidthFourTest, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>();
    // for test
    Vector4f initialBorderWidths(5.0f, 10.0f, 5.0f, 10.0f);
    border->SetWidthFour(initialBorderWidths);
    EXPECT_EQ(border->GetWidthFour(), initialBorderWidths);
    // for test
    Vector4f uniformBorderWidth(8.0f, 8.0f, 8.0f, 8.0f);
    border->SetWidthFour(uniformBorderWidth);
    EXPECT_EQ(border->GetWidthFour(), 8.0f);
}

/**
 * @tc.name: GetStyleFourTest
 * @tc.desc: Verify function GetStyleFour
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBorderTest, GetStyleFourTest, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>();
    // for test
    Vector4<uint32_t> vectorStyle(0, 1, 2, 3);
    border->SetStyleFour(vectorStyle);
    EXPECT_TRUE(border->GetStyleFour() == vectorStyle);

    Vector4<uint32_t> vectorStyle1(0, 0, 0, 0);
    border->SetStyleFour(vectorStyle1);
    EXPECT_TRUE(border->GetStyleFour() == 0);
}

/**
 * @tc.name: GetRadiusFourTest
 * @tc.desc: Verify function GetRadiusFour
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBorderTest, GetRadiusFourTest, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>();
    auto radius = border->GetRadiusFour();
    EXPECT_TRUE(radius[0] == 0.0f);
}

/**
 * @tc.name: ApplyFillStyleTest
 * @tc.desc: Verify function ApplyFillStyle
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBorderTest, ApplyFillStyleTest, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>();
    Drawing::Brush brush;
    EXPECT_FALSE(border->ApplyFillStyle(brush));
    border->SetColor(Color(0, 0, 0));
    border->SetStyle(BorderStyle::NONE);
    EXPECT_FALSE(border->ApplyFillStyle(brush));
    // for test
    Vector4<uint32_t> vectorStyle(0, 1, 2, 3);
    border->SetStyleFour(vectorStyle);
    EXPECT_FALSE(border->ApplyFillStyle(brush));
    border->SetColor(Color(0, 0, 0));
    border->SetStyle(BorderStyle::SOLID);
    border->SetWidth(0.f);
    EXPECT_FALSE(border->ApplyFillStyle(brush));
    border->SetWidth(1.f);
    EXPECT_TRUE(border->ApplyFillStyle(brush));
}

/**
 * @tc.name: ApplyPathStyleTest
 * @tc.desc: Verify function ApplyPathStyle
 * @tc.type:FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSBorderTest, ApplyPathStyleTest, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>();
    Drawing::Pen pen;
    EXPECT_FALSE(border->ApplyPathStyle(pen));
    border->SetColor(Color(0, 0, 0));
    EXPECT_FALSE(border->ApplyPathStyle(pen));
    border->SetWidth(0.f);
    EXPECT_FALSE(border->ApplyPathStyle(pen));
    border->SetStyle(BorderStyle::SOLID);
    EXPECT_FALSE(border->ApplyPathStyle(pen));
    border->SetStyle(BorderStyle::DOTTED);
    EXPECT_FALSE(border->ApplyPathStyle(pen));
}

/**
 * @tc.name: ApplyFourLineTest
 * @tc.desc: Verify function ApplyFourLine
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBorderTest, ApplyFourLineTest, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>();
    Drawing::Pen pen;
    EXPECT_FALSE(border->ApplyFourLine(pen));
    border->SetColor(Color(0, 0, 0));
    EXPECT_FALSE(border->ApplyFourLine(pen));
    border->SetStyle(BorderStyle::SOLID);
    EXPECT_TRUE(border->ApplyFourLine(pen));
}

/**
 * @tc.name: ApplyLineStyleTest
 * @tc.desc: Verify function ApplyLineStyle
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBorderTest, ApplyLineStyleTest, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>();
    Drawing::Pen pen;
    EXPECT_FALSE(border->ApplyLineStyle(pen, 0, 1.f));
    border->SetWidth(1.f);
    border->SetStyle(BorderStyle::DOTTED);
    EXPECT_FALSE(border->ApplyLineStyle(pen, 0, 1.f));
    border->SetStyle(BorderStyle::SOLID);
    border->SetWidth(1.0f);
    EXPECT_TRUE(border->ApplyLineStyle(pen, 0, 1.f));
}

/**
 * @tc.name: ApplySimpleBorderTest
 * @tc.desc: Verify function ApplySimpleBorder
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSBorderTest, ApplySimpleBorderTest, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>();
    border->colors_.clear();
    border->widths_.clear();
    border->styles_.clear();
    RRect rect;
    EXPECT_FALSE(border->ApplySimpleBorder(rect));
    {
        Color color = { 0, 0, 0, 0 };
        border->colors_.push_back(color);
        EXPECT_FALSE(border->ApplySimpleBorder(rect));
        border->widths_.push_back(1);
        EXPECT_FALSE(border->ApplySimpleBorder(rect));
        border->styles_.push_back(BorderStyle::SOLID);
        EXPECT_TRUE(border->ApplySimpleBorder(rect));
    }

    {
        border->styles_.push_back(BorderStyle::DASHED);
        EXPECT_FALSE(border->ApplySimpleBorder(rect));
        Color color = { 0, 0, 0, 0 };
        border->colors_.push_back(color);
        EXPECT_FALSE(border->ApplySimpleBorder(rect));
    }

    {
        border->widths_.push_back(1);
        EXPECT_FALSE(border->ApplySimpleBorder(rect));
        border->styles_.push_back(BorderStyle::SOLID);
        EXPECT_FALSE(border->ApplySimpleBorder(rect));
    }
}

/**
 * @tc.name: PaintFourLineTest
 * @tc.desc: Verify function PaintFourLine
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBorderTest, PaintFourLineTest, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>();
    Drawing::Canvas canvas;
    Drawing::Pen pen;
    RectF rect(1, 1, 1, 1);
    Vector4<uint32_t> style = { 1, 1, 1, 1 };
    border->SetStyleFour(style);
    border->SetWidth(1);
    border->PaintFourLine(canvas, pen, rect);
    EXPECT_TRUE(border->colors_.empty());
}

/**
 * @tc.name: ToStringTest
 * @tc.desc: Verify function ToString
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBorderTest, ToStringTest, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>();
    EXPECT_TRUE(border->ToString().empty());
    border->SetColor(Color(1, 1, 1));
    border->SetWidth(1.f);
    border->SetStyle(BorderStyle::NONE);
    EXPECT_FALSE(border->ToString().empty());
}

/**
 * @tc.name: GetIPTest
 * @tc.desc: Verify function GetTRIP and GetTLIP and GetBLIP and GetBRIP
 * @tc.type:FUNC
 * @tc.require: issueI9TOXM
 */
HWTEST_F(RSBorderTest, GetIPTest, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>();
    Drawing::RoundRect rrect;
    Drawing::Point innerRectCenter;
    innerRectCenter.SetX(1.f);
    innerRectCenter.SetY(1.f);
    rrect.rect_.SetRight(10.f);
    rrect.rect_.SetBottom(10.f);
    rrect.rect_.SetLeft(1.f);
    rrect.rect_.SetTop(1.f);
    EXPECT_EQ(border->GetTRIP(rrect, innerRectCenter).GetX(), 10);
    EXPECT_EQ(border->GetTLIP(rrect, innerRectCenter).GetX(), 1);
    EXPECT_EQ(border->GetBLIP(rrect, innerRectCenter).GetX(), 1);
    EXPECT_EQ(border->GetBRIP(rrect, innerRectCenter).GetX(), 10);
}

/**
 * @tc.name: GetDashWidthTest
 * @tc.desc: Verify function GetDashWidth
 * @tc.type:FUNC
 */
HWTEST_F(RSBorderTest, GetDashWidthTest, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>(false);
    border->SetDashWidth(0.1f);
    border->dashWidth_.push_back(0.2f);
    EXPECT_EQ(border->GetDashWidth(1), 0.2f);
}

/**
 * @tc.name: GetDashGapTest
 * @tc.desc: Verify function GetDashGap
 * @tc.type:FUNC
 */
HWTEST_F(RSBorderTest, GetDashGapTest, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>(false);
    border->SetDashGap(0.1f);
    border->dashGap_.push_back(0.2f);
    EXPECT_EQ(border->GetDashGap(1), 0.2f);
}

/**
 * @tc.name: DrawBordersTest001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSBorderTest, DrawBordersTest001, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>(false);

    // set test para
    RectF rect(-65.0f, -97.5f, 821.0f, 747.5f);
    Vector4f width(60.0f, 61.0f, 62.0f, 63.0f);
    Vector4f radius(65.0f, 65.0f, 65.0f, 65.0f);
    border->SetStyle(BorderStyle::DOTTED);
    Color color1(255, 0, 0); // red
    Color color2(0, 0, 0);   // black
    Color color3(0, 0, 255); // blue
    Color color4(0, 255, 0); // green

    // init border object
    border->SetDashWidth(1.0f);
    border->SetDashGap(1.0f);
    border->SetWidthFour(width);
    border->SetRadiusFour(radius);

    // init borderGeo
    RSBorderGeo borderGeo;
    borderGeo.rrect.rect_.SetLeft(rect.GetLeft());
    borderGeo.rrect.rect_.SetTop(rect.GetTop());
    borderGeo.rrect.rect_.SetRight(rect.GetRight());
    borderGeo.rrect.rect_.SetBottom(rect.GetBottom());
    borderGeo.innerRRect.rect_.SetLeft(rect.GetLeft() + width[RSBorder::LEFT]);
    borderGeo.innerRRect.rect_.SetTop(rect.GetTop() + width[RSBorder::TOP]);
    borderGeo.innerRRect.rect_.SetRight(rect.GetRight() + width[RSBorder::RIGHT]);
    borderGeo.innerRRect.rect_.SetBottom(rect.GetBottom() + width[RSBorder::BOTTOM]);
    borderGeo.center = { (borderGeo.innerRRect.rect_.GetLeft() + borderGeo.innerRRect.rect_.GetRight()) * 0.5,
        (borderGeo.innerRRect.rect_.GetTop() + borderGeo.innerRRect.rect_.GetBottom()) * 0.5 };

    Drawing::Canvas canvas;
    Drawing::Pen pen;
    pen.SetAntiAlias(true);
    Vector4<Color> vectorColor1(color1, color2, color3, color4);
    border->SetColorFour(vectorColor1);
    border->DrawBorders(canvas, pen, borderGeo);

    Vector4<Color> vectorColor2(color1, color1, color3, color4);
    border->SetColorFour(vectorColor2);
    border->DrawBorders(canvas, pen, borderGeo);

    Vector4<Color> vectorColor3(color1, color1, color1, color4);
    border->SetColorFour(vectorColor3);
    border->DrawBorders(canvas, pen, borderGeo);

    Vector4<Color> vectorColor4(color4, color2, color3, color4);
    border->SetColorFour(vectorColor4);
    border->DrawBorders(canvas, pen, borderGeo);

    Vector4<Color> vectorColor5(color1, color1, color1, color1);
    border->SetColorFour(vectorColor5);
    border->DrawBorders(canvas, pen, borderGeo);
    EXPECT_NE(&pen, nullptr);
}

/**
 * @tc.name: DrawBordersTest002
 * @tc.desc: Verify function
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBorderTest, DrawBordersTest002, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>(false);

    // set test para
    RectF rect(-65.0f, -97.5f, 821.0f, 747.5f);
    Vector4f width(60.0f, 61.0f, 62.0f, 63.0f);
    Vector4f radius(65.0f, 65.0f, 65.0f, 65.0f);
    border->SetStyle(BorderStyle::SOLID);
    Color color1(255, 0, 0); // red
    Color color2(0, 0, 0);   // black
    Color color3(0, 0, 255); // blue
    Color color4(0, 255, 0); // green

    // init border object
    border->SetDashWidth(1.0f);
    border->SetDashGap(1.0f);
    border->SetWidthFour(width);
    border->SetRadiusFour(radius);

    // init borderGeo
    RSBorderGeo borderGeo;
    borderGeo.rrect.rect_.SetLeft(rect.GetLeft());
    borderGeo.rrect.rect_.SetTop(rect.GetTop());
    borderGeo.rrect.rect_.SetRight(rect.GetRight());
    borderGeo.rrect.rect_.SetBottom(rect.GetBottom());
    borderGeo.innerRRect.rect_.SetLeft(rect.GetLeft() + width[RSBorder::LEFT]);
    borderGeo.innerRRect.rect_.SetTop(rect.GetTop() + width[RSBorder::TOP]);
    borderGeo.innerRRect.rect_.SetRight(rect.GetRight() + width[RSBorder::RIGHT]);
    borderGeo.innerRRect.rect_.SetBottom(rect.GetBottom() + width[RSBorder::BOTTOM]);
    borderGeo.center = { (borderGeo.innerRRect.rect_.GetLeft() + borderGeo.innerRRect.rect_.GetRight()) * 0.5,
        (borderGeo.innerRRect.rect_.GetTop() + borderGeo.innerRRect.rect_.GetBottom()) * 0.5 };

    Drawing::Canvas canvas;
    Drawing::Pen pen;
    pen.SetAntiAlias(true);
    Vector4<Color> vectorColor1(color1, color2, color3, color4);
    border->SetColorFour(vectorColor1);
    border->DrawBorders(canvas, pen, borderGeo);

    Vector4<Color> vectorColor2(color1, color1, color3, color4);
    border->SetColorFour(vectorColor2);
    border->DrawBorders(canvas, pen, borderGeo);

    Vector4<Color> vectorColor3(color1, color1, color1, color4);
    border->SetColorFour(vectorColor3);
    border->DrawBorders(canvas, pen, borderGeo);

    Vector4<Color> vectorColor4(color4, color2, color3, color4);
    border->SetColorFour(vectorColor4);
    border->DrawBorders(canvas, pen, borderGeo);

    Vector4<Color> vectorColor5(color1, color1, color1, color1);
    border->SetColorFour(vectorColor5);
    border->DrawBorders(canvas, pen, borderGeo);
    EXPECT_NE(&pen, nullptr);
}

/**
 * @tc.name: DrawBordersTest003
 * @tc.desc: Verify function
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBorderTest, DrawBordersTest003, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>(false);

    // set test para
    RectF rect(-65.0f, -97.5f, 821.0f, 747.5f);
    Vector4f width(60.0f, 61.0f, 62.0f, 63.0f);
    Vector4f radius(65.0f, 65.0f, 65.0f, 65.0f);
    border->SetStyle(BorderStyle::DASHED);
    Color color1(255, 0, 0); // red
    Color color2(0, 0, 0);   // black
    Color color3(0, 0, 255); // blue
    Color color4(0, 255, 0); // green

    // init border object
    border->SetDashWidth(1.0f);
    border->SetDashGap(1.0f);
    border->SetWidthFour(width);
    border->SetRadiusFour(radius);

    // init borderGeo
    RSBorderGeo borderGeo;
    borderGeo.rrect.rect_.SetLeft(rect.GetLeft());
    borderGeo.rrect.rect_.SetTop(rect.GetTop());
    borderGeo.rrect.rect_.SetRight(rect.GetRight());
    borderGeo.rrect.rect_.SetBottom(rect.GetBottom());
    borderGeo.innerRRect.rect_.SetLeft(rect.GetLeft() + width[RSBorder::LEFT]);
    borderGeo.innerRRect.rect_.SetTop(rect.GetTop() + width[RSBorder::TOP]);
    borderGeo.innerRRect.rect_.SetRight(rect.GetRight() + width[RSBorder::RIGHT]);
    borderGeo.innerRRect.rect_.SetBottom(rect.GetBottom() + width[RSBorder::BOTTOM]);
    borderGeo.center = { (borderGeo.innerRRect.rect_.GetLeft() + borderGeo.innerRRect.rect_.GetRight()) * 0.5,
        (borderGeo.innerRRect.rect_.GetTop() + borderGeo.innerRRect.rect_.GetBottom()) * 0.5 };

    Drawing::Canvas canvas;
    Drawing::Pen pen;
    pen.SetAntiAlias(true);
    Vector4<Color> vectorColor1(color1, color2, color3, color4);
    border->SetColorFour(vectorColor1);
    border->DrawBorders(canvas, pen, borderGeo);

    Vector4<Color> vectorColor2(color1, color1, color3, color4);
    border->SetColorFour(vectorColor2);
    border->DrawBorders(canvas, pen, borderGeo);

    Vector4<Color> vectorColor3(color1, color1, color1, color4);
    border->SetColorFour(vectorColor3);
    border->DrawBorders(canvas, pen, borderGeo);

    Vector4<Color> vectorColor4(color4, color2, color3, color4);
    border->SetColorFour(vectorColor4);
    border->DrawBorders(canvas, pen, borderGeo);

    Vector4<Color> vectorColor5(color1, color1, color1, color1);
    border->SetColorFour(vectorColor5);
    border->DrawBorders(canvas, pen, borderGeo);

    EXPECT_FALSE(border->styles_.empty());
}

/**
 * @tc.name: DrawBordersTest004
 * @tc.desc: Verify function
 * @tc.type:FUNC
 */
HWTEST_F(RSBorderTest, DrawBordersTest004, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>(false);

    // set test para
    RectF rect(-65.0f, -97.5f, 821.0f, 747.5f);
    Vector4f width(60.0f, 61.0f, 62.0f, 63.0f);
    Vector4f radius(65.0f, 65.0f, 65.0f, 65.0f);
    border->SetStyleFour({ 1, 0, 1, 0 });
    Color color1(255, 0, 0); // red
    Color color2(0, 0, 0);   // black
    Color color3(0, 0, 255); // blue
    Color color4(0, 255, 0); // green

    // init border object
    border->SetDashWidth(1.0f);
    border->SetDashGap(1.0f);
    border->SetWidthFour(width);
    border->SetRadiusFour(radius);

    // init borderGeo
    RSBorderGeo borderGeo;
    borderGeo.rrect.rect_.SetLeft(rect.GetLeft());
    borderGeo.rrect.rect_.SetTop(rect.GetTop());
    borderGeo.rrect.rect_.SetRight(rect.GetRight());
    borderGeo.rrect.rect_.SetBottom(rect.GetBottom());
    borderGeo.innerRRect.rect_.SetLeft(rect.GetLeft() + width[RSBorder::LEFT]);
    borderGeo.innerRRect.rect_.SetTop(rect.GetTop() + width[RSBorder::TOP]);
    borderGeo.innerRRect.rect_.SetRight(rect.GetRight() + width[RSBorder::RIGHT]);
    borderGeo.innerRRect.rect_.SetBottom(rect.GetBottom() + width[RSBorder::BOTTOM]);
    borderGeo.center = { (borderGeo.innerRRect.rect_.GetLeft() + borderGeo.innerRRect.rect_.GetRight()) * 0.5,
        (borderGeo.innerRRect.rect_.GetTop() + borderGeo.innerRRect.rect_.GetBottom()) * 0.5 };

    Drawing::Canvas canvas;
    Drawing::Pen pen;
    pen.SetAntiAlias(true);
    Vector4<Color> vectorColor1(color1, color2, color3, color4);
    border->SetColorFour(vectorColor1);
    border->DrawBorders(canvas, pen, borderGeo);

    Vector4<Color> vectorColor2(color1, color1, color3, color4);
    border->SetColorFour(vectorColor2);
    border->DrawBorders(canvas, pen, borderGeo);

    Vector4<Color> vectorColor3(color1, color1, color1, color4);
    border->SetColorFour(vectorColor3);
    border->DrawBorders(canvas, pen, borderGeo);

    Vector4<Color> vectorColor4(color4, color2, color3, color4);
    border->SetColorFour(vectorColor4);
    border->DrawBorders(canvas, pen, borderGeo);

    Vector4<Color> vectorColor5(color1, color1, color1, color1);
    border->SetColorFour(vectorColor5);
    border->DrawBorders(canvas, pen, borderGeo);

    EXPECT_FALSE(border->styles_.empty());
}

/**
 * @tc.name: DrawBordersTest005
 * @tc.desc: Verify function
 * @tc.type:FUNC
 */
HWTEST_F(RSBorderTest, DrawBordersTest005, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>(false);

    // set test para
    RectF rect(-65.0f, -97.5f, 821.0f, 747.5f);
    Vector4f width(60.0f, 61.0f, 62.0f, 63.0f);
    Vector4f radius(65.0f, 65.0f, 65.0f, 65.0f);
    border->SetStyleFour({ 0, 0, 1, 1 });
    Color color1(255, 0, 0); // red
    Color color2(0, 0, 0);   // black
    Color color3(0, 0, 255); // blue
    Color color4(0, 255, 0); // green

    // init border object
    border->SetDashWidth(1.0f);
    border->SetDashGap(1.0f);
    border->SetWidthFour(width);
    border->SetRadiusFour(radius);

    // init borderGeo
    RSBorderGeo borderGeo;
    borderGeo.rrect.rect_.SetLeft(rect.GetLeft());
    borderGeo.rrect.rect_.SetTop(rect.GetTop());
    borderGeo.rrect.rect_.SetRight(rect.GetRight());
    borderGeo.rrect.rect_.SetBottom(rect.GetBottom());
    borderGeo.innerRRect.rect_.SetLeft(rect.GetLeft() + width[RSBorder::LEFT]);
    borderGeo.innerRRect.rect_.SetTop(rect.GetTop() + width[RSBorder::TOP]);
    borderGeo.innerRRect.rect_.SetRight(rect.GetRight() + width[RSBorder::RIGHT]);
    borderGeo.innerRRect.rect_.SetBottom(rect.GetBottom() + width[RSBorder::BOTTOM]);
    borderGeo.center = { (borderGeo.innerRRect.rect_.GetLeft() + borderGeo.innerRRect.rect_.GetRight()) * 0.5,
        (borderGeo.innerRRect.rect_.GetTop() + borderGeo.innerRRect.rect_.GetBottom()) * 0.5 };

    Drawing::Canvas canvas;
    Drawing::Pen pen;
    pen.SetAntiAlias(true);
    Vector4<Color> vectorColor1(color1, color2, color3, color4);
    border->SetColorFour(vectorColor1);
    border->DrawBorders(canvas, pen, borderGeo);

    Vector4<Color> vectorColor2(color1, color1, color3, color4);
    border->SetColorFour(vectorColor2);
    border->DrawBorders(canvas, pen, borderGeo);

    Vector4<Color> vectorColor3(color1, color1, color1, color4);
    border->SetColorFour(vectorColor3);
    border->DrawBorders(canvas, pen, borderGeo);

    Vector4<Color> vectorColor4(color4, color2, color3, color4);
    border->SetColorFour(vectorColor4);
    border->DrawBorders(canvas, pen, borderGeo);

    Vector4<Color> vectorColor5(color1, color1, color1, color1);
    border->SetColorFour(vectorColor5);
    border->DrawBorders(canvas, pen, borderGeo);

    EXPECT_FALSE(border->styles_.empty());
}

/**
 * @tc.name: DrawBordersTest006
 * @tc.desc: Verify function
 * @tc.type:FUNC
 */
HWTEST_F(RSBorderTest, DrawBordersTest006, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>(false);

    // set test para
    RectF rect(-65.0f, -97.5f, 821.0f, 747.5f);
    Vector4f width(60.0f, 61.0f, 62.0f, 63.0f);
    Vector4f radius(65.0f, 65.0f, 65.0f, 65.0f);
    border->SetStyleFour({ 1, 1, 0, 0 });
    Color color1(255, 0, 0); // red
    Color color2(0, 0, 0);   // black
    Color color3(0, 0, 255); // blue
    Color color4(0, 255, 0); // green

    // init border object
    border->SetDashWidth(1.0f);
    border->SetDashGap(1.0f);
    border->SetWidthFour(width);
    border->SetRadiusFour(radius);

    // init borderGeo
    RSBorderGeo borderGeo;
    borderGeo.rrect.rect_.SetLeft(rect.GetLeft());
    borderGeo.rrect.rect_.SetTop(rect.GetTop());
    borderGeo.rrect.rect_.SetRight(rect.GetRight());
    borderGeo.rrect.rect_.SetBottom(rect.GetBottom());
    borderGeo.innerRRect.rect_.SetLeft(rect.GetLeft() + width[RSBorder::LEFT]);
    borderGeo.innerRRect.rect_.SetTop(rect.GetTop() + width[RSBorder::TOP]);
    borderGeo.innerRRect.rect_.SetRight(rect.GetRight() + width[RSBorder::RIGHT]);
    borderGeo.innerRRect.rect_.SetBottom(rect.GetBottom() + width[RSBorder::BOTTOM]);
    borderGeo.center = { (borderGeo.innerRRect.rect_.GetLeft() + borderGeo.innerRRect.rect_.GetRight()) * 0.5,
        (borderGeo.innerRRect.rect_.GetTop() + borderGeo.innerRRect.rect_.GetBottom()) * 0.5 };

    Drawing::Canvas canvas;
    Drawing::Pen pen;
    pen.SetAntiAlias(true);
    Vector4<Color> vectorColor1(color1, color2, color3, color4);
    border->SetColorFour(vectorColor1);
    border->DrawBorders(canvas, pen, borderGeo);

    Vector4<Color> vectorColor2(color1, color1, color3, color4);
    border->SetColorFour(vectorColor2);
    border->DrawBorders(canvas, pen, borderGeo);

    Vector4<Color> vectorColor3(color1, color1, color1, color4);
    border->SetColorFour(vectorColor3);
    border->DrawBorders(canvas, pen, borderGeo);

    Vector4<Color> vectorColor4(color4, color2, color3, color4);
    border->SetColorFour(vectorColor4);
    border->DrawBorders(canvas, pen, borderGeo);

    Vector4<Color> vectorColor5(color1, color1, color1, color1);
    border->SetColorFour(vectorColor5);
    border->DrawBorders(canvas, pen, borderGeo);

    EXPECT_FALSE(border->styles_.empty());
}


/**
 * @tc.name: DrawBordersTest007
 * @tc.desc: Verify function
 * @tc.type:FUNC
 */
HWTEST_F(RSBorderTest, DrawBordersTest007, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>(false);

    // set test para
    RectF rect(-65.0f, -97.5f, 821.0f, 747.5f);
    Vector4f width(60.0f, 61.0f, 62.0f, 63.0f);
    Vector4f radius(65.0f, 65.0f, 65.0f, 65.0f);
    border->SetStyleFour({ 1, 0, 0, 1 });
    Color color1(255, 0, 0); // red
    Color color2(0, 0, 0);   // black
    Color color3(0, 0, 255); // blue
    Color color4(0, 255, 0); // green

    // init border object
    border->SetDashWidth(1.0f);
    border->SetDashGap(1.0f);
    border->SetWidthFour(width);
    border->SetRadiusFour(radius);

    // init borderGeo
    RSBorderGeo borderGeo;
    borderGeo.rrect.rect_.SetLeft(rect.GetLeft());
    borderGeo.rrect.rect_.SetTop(rect.GetTop());
    borderGeo.rrect.rect_.SetRight(rect.GetRight());
    borderGeo.rrect.rect_.SetBottom(rect.GetBottom());
    borderGeo.innerRRect.rect_.SetLeft(rect.GetLeft() + width[RSBorder::LEFT]);
    borderGeo.innerRRect.rect_.SetTop(rect.GetTop() + width[RSBorder::TOP]);
    borderGeo.innerRRect.rect_.SetRight(rect.GetRight() + width[RSBorder::RIGHT]);
    borderGeo.innerRRect.rect_.SetBottom(rect.GetBottom() + width[RSBorder::BOTTOM]);
    borderGeo.center = { (borderGeo.innerRRect.rect_.GetLeft() + borderGeo.innerRRect.rect_.GetRight()) * 0.5,
        (borderGeo.innerRRect.rect_.GetTop() + borderGeo.innerRRect.rect_.GetBottom()) * 0.5 };

    Drawing::Canvas canvas;
    Drawing::Pen pen;
    pen.SetAntiAlias(true);
    Vector4<Color> vectorColor1(color1, color2, color3, color4);
    border->SetColorFour(vectorColor1);
    border->DrawBorders(canvas, pen, borderGeo);

    Vector4<Color> vectorColor2(color1, color1, color3, color4);
    border->SetColorFour(vectorColor2);
    border->DrawBorders(canvas, pen, borderGeo);

    Vector4<Color> vectorColor3(color1, color1, color1, color4);
    border->SetColorFour(vectorColor3);
    border->DrawBorders(canvas, pen, borderGeo);

    Vector4<Color> vectorColor4(color4, color2, color3, color4);
    border->SetColorFour(vectorColor4);
    border->DrawBorders(canvas, pen, borderGeo);

    Vector4<Color> vectorColor5(color1, color1, color1, color1);
    border->SetColorFour(vectorColor5);
    border->DrawBorders(canvas, pen, borderGeo);

    EXPECT_FALSE(border->styles_.empty());
}

/**
 * @tc.name: DrawBorderImplTest001
 * @tc.desc: Verify function
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBorderTest, DrawBorderImplTest001, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>(false);

    // set test para
    RectF rect(-65.0f, -97.5f, 821.0f, 747.5f);
    Vector4f width(60.0f, 61.0f, 62.0f, 63.0f);
    Vector4f radius(65.0f, 65.0f, 65.0f, 65.0f);
    border->SetStyle(BorderStyle::DASHED);
    Color color1(255, 0, 0); // red
    Color color2(0, 0, 0);   // black
    Color color3(0, 0, 255); // blue
    Color color4(0, 255, 0); // green

    // init border object
    border->SetDashWidth(1.0f);
    border->SetDashGap(1.0f);
    border->SetWidthFour(width);
    border->SetRadiusFour(radius);
    Vector4<Color> vectorColor(color1, color2, color3, color4);
    border->SetColorFour(vectorColor);

    // init borderGeo
    RSBorderGeo borderGeo;
    borderGeo.rrect.rect_.SetLeft(rect.GetLeft());
    borderGeo.rrect.rect_.SetTop(rect.GetTop());
    borderGeo.rrect.rect_.SetRight(rect.GetRight());
    borderGeo.rrect.rect_.SetBottom(rect.GetBottom());
    borderGeo.innerRRect.rect_.SetLeft(rect.GetLeft() + width[RSBorder::LEFT]);
    borderGeo.innerRRect.rect_.SetTop(rect.GetTop() + width[RSBorder::TOP]);
    borderGeo.innerRRect.rect_.SetRight(rect.GetRight() + width[RSBorder::RIGHT]);
    borderGeo.innerRRect.rect_.SetBottom(rect.GetBottom() + width[RSBorder::BOTTOM]);
    borderGeo.center = { (borderGeo.innerRRect.rect_.GetLeft() + borderGeo.innerRRect.rect_.GetRight()) * 0.5,
        (borderGeo.innerRRect.rect_.GetTop() + borderGeo.innerRRect.rect_.GetBottom()) * 0.5 };

    Drawing::Canvas canvas;
    Drawing::Pen pen;
    pen.SetAntiAlias(true);

    border->CalcBorderPath(borderGeo);
    border->DrawBorderImpl(canvas, pen, borderGeo, RSBorder::LEFT);
    border->DrawBorderImpl(canvas, pen, borderGeo, RSBorder::TOP);
    border->DrawBorderImpl(canvas, pen, borderGeo, RSBorder::RIGHT);
    border->DrawBorderImpl(canvas, pen, borderGeo, RSBorder::BOTTOM);
    EXPECT_FALSE(border->styles_.empty());
}

/**
 * @tc.name: DrawBorderImplTest002
 * @tc.desc: Verify function
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBorderTest, DrawBorderImplTest002, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>(false);

    // set test para
    RectF rect(-65.0f, -97.5f, 821.0f, 747.5f);
    Vector4f width(60.0f, 61.0f, 62.0f, 63.0f);
    Vector4f radius(65.0f, 65.0f, 65.0f, 65.0f);
    border->SetStyle(BorderStyle::SOLID);
    Color color1(255, 0, 0); // red
    Color color2(0, 0, 0);   // black
    Color color3(0, 0, 255); // blue
    Color color4(0, 255, 0); // green

    // init border object
    border->SetDashWidth(1.0f);
    border->SetDashGap(1.0f);
    border->SetWidthFour(width);
    border->SetRadiusFour(radius);

    // init borderGeo
    RSBorderGeo borderGeo;
    borderGeo.rrect.rect_.SetLeft(rect.GetLeft());
    borderGeo.rrect.rect_.SetTop(rect.GetTop());
    borderGeo.rrect.rect_.SetRight(rect.GetRight());
    borderGeo.rrect.rect_.SetBottom(rect.GetBottom());
    borderGeo.innerRRect.rect_.SetLeft(rect.GetLeft() + width[RSBorder::LEFT]);
    borderGeo.innerRRect.rect_.SetTop(rect.GetTop() + width[RSBorder::TOP]);
    borderGeo.innerRRect.rect_.SetRight(rect.GetRight() + width[RSBorder::RIGHT]);
    borderGeo.innerRRect.rect_.SetBottom(rect.GetBottom() + width[RSBorder::BOTTOM]);
    borderGeo.center = { (borderGeo.innerRRect.rect_.GetLeft() + borderGeo.innerRRect.rect_.GetRight()) * 0.5,
        (borderGeo.innerRRect.rect_.GetTop() + borderGeo.innerRRect.rect_.GetBottom()) * 0.5 };

    Drawing::Canvas canvas;
    Drawing::Pen pen;
    pen.SetAntiAlias(true);

    border->CalcBorderPath(borderGeo);

    Vector4<Color> vectorColor1(color1, color1, color3, color3);
    border->SetColorFour(vectorColor1);
    border->DrawBorderImpl(canvas, pen, borderGeo, RSBorder::LEFT, RSBorder::TOP);
    border->DrawBorderImpl(canvas, pen, borderGeo, RSBorder::RIGHT, RSBorder::BOTTOM);

    Vector4<Color> vectorColor2(color4, color2, color2, color4);
    border->SetColorFour(vectorColor2);
    border->DrawBorderImpl(canvas, pen, borderGeo, RSBorder::TOP, RSBorder::RIGHT);
    border->DrawBorderImpl(canvas, pen, borderGeo, RSBorder::BOTTOM, RSBorder::LEFT);
    EXPECT_FALSE(border->styles_.empty());
}

/**
 * @tc.name: DrawBorderImplTest003
 * @tc.desc: Verify function
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBorderTest, DrawBorderImplTest003, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>(false);

    // set test para
    RectF rect(-65.0f, -97.5f, 821.0f, 747.5f);
    Vector4f width(60.0f, 61.0f, 62.0f, 63.0f);
    Vector4f radius(65.0f, 65.0f, 65.0f, 65.0f);
    border->SetStyle(BorderStyle::SOLID);
    Color color1(255, 0, 0); // red
    Color color2(0, 0, 0);   // black
    Color color3(0, 0, 255); // blue
    Color color4(0, 255, 0); // green

    // init border object
    border->SetDashWidth(1.0f);
    border->SetDashGap(1.0f);
    border->SetWidthFour(width);
    border->SetRadiusFour(radius);

    // init borderGeo
    RSBorderGeo borderGeo;
    borderGeo.rrect.rect_.SetLeft(rect.GetLeft());
    borderGeo.rrect.rect_.SetTop(rect.GetTop());
    borderGeo.rrect.rect_.SetRight(rect.GetRight());
    borderGeo.rrect.rect_.SetBottom(rect.GetBottom());
    borderGeo.innerRRect.rect_.SetLeft(rect.GetLeft() + width[RSBorder::LEFT]);
    borderGeo.innerRRect.rect_.SetTop(rect.GetTop() + width[RSBorder::TOP]);
    borderGeo.innerRRect.rect_.SetRight(rect.GetRight() + width[RSBorder::RIGHT]);
    borderGeo.innerRRect.rect_.SetBottom(rect.GetBottom() + width[RSBorder::BOTTOM]);
    borderGeo.center = { (borderGeo.innerRRect.rect_.GetLeft() + borderGeo.innerRRect.rect_.GetRight()) * 0.5,
        (borderGeo.innerRRect.rect_.GetTop() + borderGeo.innerRRect.rect_.GetBottom()) * 0.5 };

    Drawing::Canvas canvas;
    Drawing::Pen pen;
    pen.SetAntiAlias(true);

    border->CalcBorderPath(borderGeo);
    Vector4<Color> vectorColor1(color1, color1, color1, color3);
    border->SetColorFour(vectorColor1);
    border->DrawBorderImpl(canvas, pen, borderGeo, RSBorder::LEFT, RSBorder::TOP, RSBorder::RIGHT);

    Vector4<Color> vectorColor2(color1, color2, color2, color2);
    border->SetColorFour(vectorColor2);
    border->DrawBorderImpl(canvas, pen, borderGeo, RSBorder::TOP, RSBorder::RIGHT, RSBorder::BOTTOM);

    Vector4<Color> vectorColor3(color3, color2, color3, color3);
    border->SetColorFour(vectorColor3);
    border->DrawBorderImpl(canvas, pen, borderGeo, RSBorder::RIGHT, RSBorder::BOTTOM, RSBorder::LEFT);

    Vector4<Color> vectorColor4(color4, color4, color3, color4);
    border->SetColorFour(vectorColor4);
    border->DrawBorderImpl(canvas, pen, borderGeo, RSBorder::BOTTOM, RSBorder::LEFT, RSBorder::TOP);
    EXPECT_FALSE(border->styles_.empty());
}

/**
 * @tc.name: CalcBorderPathTest
 * @tc.desc: Verify function
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBorderTest, CalcBorderPathTest, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>(false);

    // set test para
    RectF rect(-65.0f, -97.5f, 821.0f, 747.5f);
    Vector4f width(60.0f, 61.0f, 62.0f, 63.0f);
    Vector4f radius(65.0f, 65.0f, 65.0f, 65.0f);
    border->SetStyle(BorderStyle::SOLID);
    Color color1(255, 0, 0); // red
    Color color2(0, 0, 0);   // black
    Color color3(0, 0, 255); // blue
    Color color4(0, 255, 0); // green

    // init border object
    border->SetDashWidth(1.0f);
    border->SetDashGap(1.0f);
    border->SetWidthFour(width);
    border->SetRadiusFour(radius);
    Vector4<Color> vectorColor(color1, color1, color1, color3);
    border->SetColorFour(vectorColor);

    // init borderGeo
    RSBorderGeo borderGeo;
    borderGeo.rrect.rect_.SetLeft(rect.GetLeft());
    borderGeo.rrect.rect_.SetTop(rect.GetTop());
    borderGeo.rrect.rect_.SetRight(rect.GetRight());
    borderGeo.rrect.rect_.SetBottom(rect.GetBottom());
    borderGeo.innerRRect.rect_.SetLeft(rect.GetLeft() + width[RSBorder::LEFT]);
    borderGeo.innerRRect.rect_.SetTop(rect.GetTop() + width[RSBorder::TOP]);
    borderGeo.innerRRect.rect_.SetRight(rect.GetRight() + width[RSBorder::RIGHT]);
    borderGeo.innerRRect.rect_.SetBottom(rect.GetBottom() + width[RSBorder::BOTTOM]);
    borderGeo.center = { (borderGeo.innerRRect.rect_.GetLeft() + borderGeo.innerRRect.rect_.GetRight()) * 0.5,
        (borderGeo.innerRRect.rect_.GetTop() + borderGeo.innerRRect.rect_.GetBottom()) * 0.5 };

    Drawing::Canvas canvas;
    Drawing::Pen pen;
    pen.SetAntiAlias(true);

    border->CalcBorderPath(borderGeo);
    EXPECT_FALSE(border->styles_.empty());
}

/**
 * @tc.name: DrawTopBorderTest
 * @tc.desc: Verify function PaintTopPath
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBorderTest, DrawTopBorderTest, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>(false);

    // set test para
    RectF rect(-65.0f, -97.5f, 821.0f, 747.5f);
    Vector4f width(60.0f, 61.0f, 62.0f, 63.0f);
    Vector4f radius(65.0f, 65.0f, 65.0f, 65.0f);
    Color color1(255, 0, 0); // red
    Color color2(0, 0, 0);   // black
    Color color3(0, 0, 255); // blue
    Color color4(0, 255, 0); // green
    Vector4<Color> vectorColor(color1, color2, color3, color4);

    // init border object
    border->SetDashWidth(1.0f);
    border->SetDashGap(1.0f);
    border->SetWidthFour(width);
    border->SetRadiusFour(radius);
    border->SetColorFour(vectorColor);

    // init borderGeo
    RSBorderGeo borderGeo;
    borderGeo.rrect.rect_.SetLeft(rect.GetLeft());
    borderGeo.rrect.rect_.SetTop(rect.GetTop());
    borderGeo.rrect.rect_.SetRight(rect.GetRight());
    borderGeo.rrect.rect_.SetBottom(rect.GetBottom());
    borderGeo.innerRRect.rect_.SetLeft(rect.GetLeft() + width[RSBorder::LEFT]);
    borderGeo.innerRRect.rect_.SetTop(rect.GetTop() + width[RSBorder::TOP]);
    borderGeo.innerRRect.rect_.SetRight(rect.GetRight() + width[RSBorder::RIGHT]);
    borderGeo.innerRRect.rect_.SetBottom(rect.GetBottom() + width[RSBorder::BOTTOM]);
    borderGeo.center = { (borderGeo.innerRRect.rect_.GetLeft() + borderGeo.innerRRect.rect_.GetRight()) * 0.5,
        (borderGeo.innerRRect.rect_.GetTop() + borderGeo.innerRRect.rect_.GetBottom()) * 0.5 };

    Drawing::Canvas canvas;
    Drawing::Pen pen;
    pen.SetAntiAlias(true);
    border->SetStyle(BorderStyle::SOLID);
    border->DrawTopBorder(canvas, pen, borderGeo);
    border->SetStyle(BorderStyle::DASHED);
    border->DrawTopBorder(canvas, pen, borderGeo);
    EXPECT_FALSE(border->styles_.empty());
}

/**
 * @tc.name: DrawRightBorderTest
 * @tc.desc: Verify function PaintRightPath
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBorderTest, DrawRightBorderTest, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>(false);

    // set test para
    RectF rect(-65.0f, -97.5f, 821.0f, 747.5f);
    Vector4f width(60.0f, 61.0f, 62.0f, 63.0f);
    Vector4f radius(65.0f, 65.0f, 65.0f, 65.0f);
    Color color1(255, 0, 0); // red
    Color color2(0, 0, 0);   // black
    Color color3(0, 0, 255); // blue
    Color color4(0, 255, 0); // green
    Vector4<Color> vectorColor(color1, color2, color3, color4);

    // init border object
    border->SetDashWidth(1.0f);
    border->SetDashGap(1.0f);
    border->SetWidthFour(width);
    border->SetRadiusFour(radius);
    border->SetColorFour(vectorColor);

    // init borderGeo
    RSBorderGeo borderGeo;
    borderGeo.rrect.rect_.SetLeft(rect.GetLeft());
    borderGeo.rrect.rect_.SetTop(rect.GetTop());
    borderGeo.rrect.rect_.SetRight(rect.GetRight());
    borderGeo.rrect.rect_.SetBottom(rect.GetBottom());
    borderGeo.innerRRect.rect_.SetLeft(rect.GetLeft() + width[RSBorder::LEFT]);
    borderGeo.innerRRect.rect_.SetTop(rect.GetTop() + width[RSBorder::TOP]);
    borderGeo.innerRRect.rect_.SetRight(rect.GetRight() + width[RSBorder::RIGHT]);
    borderGeo.innerRRect.rect_.SetBottom(rect.GetBottom() + width[RSBorder::BOTTOM]);
    borderGeo.center = { (borderGeo.innerRRect.rect_.GetLeft() + borderGeo.innerRRect.rect_.GetRight()) * 0.5,
        (borderGeo.innerRRect.rect_.GetTop() + borderGeo.innerRRect.rect_.GetBottom()) * 0.5 };

    Drawing::Canvas canvas;
    Drawing::Pen pen;
    pen.SetAntiAlias(true);
    border->SetStyle(BorderStyle::SOLID);
    border->DrawRightBorder(canvas, pen, borderGeo);
    border->SetStyle(BorderStyle::DOTTED);
    border->DrawRightBorder(canvas, pen, borderGeo);
    EXPECT_FALSE(border->styles_.empty());
}

/**
 * @tc.name: DrawBottomBorderTest
 * @tc.desc: Verify function PaintBottomPath
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBorderTest, DrawBottomBorderTest, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>(false);

    // set test para
    RectF rect(-65.0f, -97.5f, 821.0f, 747.5f);
    Vector4f width(60.0f, 61.0f, 62.0f, 63.0f);
    Vector4f radius(65.0f, 65.0f, 65.0f, 65.0f);
    Color color1(255, 0, 0); // red
    Color color2(0, 0, 0);   // black
    Color color3(0, 0, 255); // blue
    Color color4(0, 255, 0); // green
    Vector4<Color> vectorColor(color1, color2, color3, color4);

    // init border object
    border->SetDashWidth(1.0f);
    border->SetDashGap(1.0f);
    border->SetWidthFour(width);
    border->SetRadiusFour(radius);
    border->SetColorFour(vectorColor);

    // init borderGeo
    RSBorderGeo borderGeo;
    borderGeo.rrect.rect_.SetLeft(rect.GetLeft());
    borderGeo.rrect.rect_.SetTop(rect.GetTop());
    borderGeo.rrect.rect_.SetRight(rect.GetRight());
    borderGeo.rrect.rect_.SetBottom(rect.GetBottom());
    borderGeo.innerRRect.rect_.SetLeft(rect.GetLeft() + width[RSBorder::LEFT]);
    borderGeo.innerRRect.rect_.SetTop(rect.GetTop() + width[RSBorder::TOP]);
    borderGeo.innerRRect.rect_.SetRight(rect.GetRight() + width[RSBorder::RIGHT]);
    borderGeo.innerRRect.rect_.SetBottom(rect.GetBottom() + width[RSBorder::BOTTOM]);
    borderGeo.center = { (borderGeo.innerRRect.rect_.GetLeft() + borderGeo.innerRRect.rect_.GetRight()) * 0.5,
        (borderGeo.innerRRect.rect_.GetTop() + borderGeo.innerRRect.rect_.GetBottom()) * 0.5 };

    Drawing::Canvas canvas;
    Drawing::Pen pen;
    pen.SetAntiAlias(true);
    border->SetStyle(BorderStyle::SOLID);
    border->DrawBottomBorder(canvas, pen, borderGeo);
    border->SetStyle(BorderStyle::DOTTED);
    border->DrawBottomBorder(canvas, pen, borderGeo);
    EXPECT_FALSE(border->styles_.empty());
}

/**
 * @tc.name: DrawLeftBorderTest
 * @tc.desc: Verify function PaintLeftPath
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBorderTest, DrawLeftBorderTest, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>(false);

    // set test para
    RectF rect(-65.0f, -97.5f, 821.0f, 747.5f);
    Vector4f width(60.0f, 61.0f, 62.0f, 63.0f);
    Vector4f radius(65.0f, 65.0f, 65.0f, 65.0f);
    Color color1(255, 0, 0); // red
    Color color2(0, 0, 0);   // black
    Color color3(0, 0, 255); // blue
    Color color4(0, 255, 0); // green
    Vector4<Color> vectorColor(color1, color2, color3, color4);

    // init border object
    border->SetDashWidth(1.0f);
    border->SetDashGap(1.0f);
    border->SetWidthFour(width);
    border->SetRadiusFour(radius);
    border->SetColorFour(vectorColor);

    // init borderGeo
    RSBorderGeo borderGeo;
    borderGeo.rrect.rect_.SetLeft(rect.GetLeft());
    borderGeo.rrect.rect_.SetTop(rect.GetTop());
    borderGeo.rrect.rect_.SetRight(rect.GetRight());
    borderGeo.rrect.rect_.SetBottom(rect.GetBottom());
    borderGeo.innerRRect.rect_.SetLeft(rect.GetLeft() + width[RSBorder::LEFT]);
    borderGeo.innerRRect.rect_.SetTop(rect.GetTop() + width[RSBorder::TOP]);
    borderGeo.innerRRect.rect_.SetRight(rect.GetRight() + width[RSBorder::RIGHT]);
    borderGeo.innerRRect.rect_.SetBottom(rect.GetBottom() + width[RSBorder::BOTTOM]);
    borderGeo.center = { (borderGeo.innerRRect.rect_.GetLeft() + borderGeo.innerRRect.rect_.GetRight()) * 0.5,
        (borderGeo.innerRRect.rect_.GetTop() + borderGeo.innerRRect.rect_.GetBottom()) * 0.5 };

    Drawing::Canvas canvas;
    Drawing::Pen pen;
    pen.SetAntiAlias(true);
    border->SetStyle(BorderStyle::SOLID);
    border->DrawLeftBorder(canvas, pen, borderGeo);
    border->SetStyle(BorderStyle::DASHED);
    border->DrawLeftBorder(canvas, pen, borderGeo);
    EXPECT_FALSE(border->styles_.empty());
}
} // namespace OHOS::Rosen
