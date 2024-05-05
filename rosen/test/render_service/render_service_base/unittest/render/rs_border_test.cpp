/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
    border->ToString();
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
 * @tc.require: issueI9I98H
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
    EXPECT_TRUE(border->ApplyPathStyle(pen));
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
    EXPECT_TRUE(border->ApplyLineStyle(pen, 0, 1.f));
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
    RectF rect;
    border->PaintFourLine(canvas, pen, rect);
    EXPECT_TRUE(border->colors_.empty());
}

/**
 * @tc.name: PaintTopPathTest
 * @tc.desc: Verify function PaintTopPath
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBorderTest, PaintTopPathTest, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>();
    Drawing::Canvas canvas;
    Drawing::Pen pen;
    Drawing::RoundRect rrect;
    Drawing::Point innerRectCenter;
    border->SetWidth(1.f);
    border->SetStyle(BorderStyle::SOLID);
    border->PaintTopPath(canvas, pen, rrect, innerRectCenter);
    border->SetStyle(BorderStyle::NONE);
    border->PaintTopPath(canvas, pen, rrect, innerRectCenter);
    EXPECT_FALSE(border->styles_.empty());
}

/**
 * @tc.name: PaintRightPathTest
 * @tc.desc: Verify function PaintRightPath
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBorderTest, PaintRightPathTest, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>();
    Drawing::Canvas canvas;
    Drawing::Pen pen;
    Drawing::RoundRect rrect;
    Drawing::Point innerRectCenter;
    border->SetWidth(1.f);
    border->SetStyle(BorderStyle::SOLID);
    border->PaintRightPath(canvas, pen, rrect, innerRectCenter);
    border->SetStyle(BorderStyle::NONE);
    border->PaintRightPath(canvas, pen, rrect, innerRectCenter);
    EXPECT_FALSE(border->styles_.empty());
}

/**
 * @tc.name: PaintBottomPathTest
 * @tc.desc: Verify function PaintBottomPath
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBorderTest, PaintBottomPathTest, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>();
    Drawing::Canvas canvas;
    Drawing::Pen pen;
    Drawing::RoundRect rrect;
    Drawing::Point innerRectCenter;
    border->SetWidth(1.f);
    border->SetStyle(BorderStyle::SOLID);
    border->PaintBottomPath(canvas, pen, rrect, innerRectCenter);
    border->SetStyle(BorderStyle::NONE);
    border->PaintBottomPath(canvas, pen, rrect, innerRectCenter);
    EXPECT_FALSE(border->styles_.empty());
}

/**
 * @tc.name: PaintLeftPathTest
 * @tc.desc: Verify function PaintLeftPath
 * @tc.type:FUNC
 * @tc.require: issueI9I98H
 */
HWTEST_F(RSBorderTest, PaintLeftPathTest, TestSize.Level1)
{
    auto border = std::make_shared<RSBorder>();
    Drawing::Canvas canvas;
    Drawing::Pen pen;
    Drawing::RoundRect rrect;
    Drawing::Point innerRectCenter;
    border->SetWidth(1.f);
    border->SetStyle(BorderStyle::SOLID);
    border->PaintLeftPath(canvas, pen, rrect, innerRectCenter);
    border->SetStyle(BorderStyle::NONE);
    border->PaintLeftPath(canvas, pen, rrect, innerRectCenter);
    EXPECT_FALSE(border->styles_.empty());
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
} // namespace OHOS::Rosen
