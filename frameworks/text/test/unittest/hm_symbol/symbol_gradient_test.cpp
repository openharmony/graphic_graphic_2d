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
#include "symbol_gradient.h"
#include "convert.h"


using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class OHHmSymbolGradientTest : public testing::Test {};

/*
 * @tc.name: SymbolGradientTest001
 * @tc.desc: test SymbolGradient
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolGradientTest, SymbolGradientTest001, TestSize.Level0)
{
    // init data
    SymbolGradient gradient = SymbolGradient();
    // 0xFF00FF00 and 0XFFFF0000 is ARGB
    std::vector<Drawing::Color> colors = {Drawing::Color(0xFF00FF00), Drawing::Color(0XFFFF0000)};
    std::vector<float> positions = {0.2f, 0.9f}; // 0.2f and 0.9f is position of colors

    // test setColors
    gradient.SetColors(colors);
    EXPECT_FALSE(gradient.GetColors().empty());
    EXPECT_TRUE(gradient.GetGradientType() == GradientType::NONE_GRADIENT);

    // test SetPositions
    gradient.SetPositions(positions);
    EXPECT_FALSE(gradient.GetPositions().empty());

    // test SetTileMode
    gradient.SetTileMode(Drawing::TileMode::REPEAT);
    EXPECT_TRUE(gradient.GetTileMode() == Drawing::TileMode::REPEAT);

    // 0.0f left, 0.0f top, 100.0f right, 100.0f bottom
    Drawing::Rect bounds = Drawing::Rect(0.0f, 0.0f, 100.0f, 100.0f);
    Drawing::Point offset = Drawing::Point(0.5f, 0.6f); // 0.5f: x, 0.6f: y
    gradient.Make(bounds);
    auto brush = gradient.CreateGradientBrush();
    auto pen = gradient.CreateGradientPen();
    auto shader =  gradient.CreateGradientShader(offset);
    EXPECT_EQ(shader, nullptr);
}

/*
 * @tc.name: IsNearlyEqual001
 * @tc.desc: test IsNearlyEqual
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolGradientTest, IsNearlyEqual001, TestSize.Level0)
{
    // init data
    auto gradient1 = std::make_shared<SymbolGradient>();
    auto gradient2 = std::make_shared<SymbolGradient>();
    // 0xFF00FF00 and 0XFFFF0000 is ARGB
    std::vector<Drawing::Color> colors = {Drawing::Color(0xFF00FF00), Drawing::Color(0XFFFF0000)};
    std::vector<float> positions = {0.2f, 0.9f}; // 0.2f and 0.9f is position of colors

    // test colors is empty
    auto brush = gradient1->CreateGradientBrush();
    auto pen = gradient1->CreateGradientPen();
    EXPECT_TRUE(gradient1->IsNearlyEqual(gradient2));

    // test gradientType
    auto gradient3 = std::make_shared<SymbolLineGradient>(45.0f); // 45.0f is angle of linegradient
    EXPECT_FALSE(gradient1->IsNearlyEqual(gradient3));

    // test colors
    gradient1->SetColors(colors);
    EXPECT_FALSE(gradient1->IsNearlyEqual(gradient2));

    // 0xFF00FFFF and 0XFFFF0000 is ARGB
    std::vector<Drawing::Color> colors1 = {Drawing::Color(0xFF00FFFF), Drawing::Color(0XFFFF0000)};
    gradient2->SetColors(colors1);
    EXPECT_FALSE(gradient1->IsNearlyEqual(gradient2));

    gradient2->SetColors(colors);
    EXPECT_TRUE(gradient1->IsNearlyEqual(gradient2));

    // test positions
    gradient2->SetPositions(positions);
    EXPECT_FALSE(gradient1->IsNearlyEqual(gradient2));

    std::vector<float> positions1 = {0.3f, 0.9f}; // 0.3f and 0.9f is position of colors
    gradient1->SetPositions(positions1);
    EXPECT_FALSE(gradient1->IsNearlyEqual(gradient2));

    gradient1->SetPositions(positions);
    EXPECT_TRUE(gradient1->IsNearlyEqual(gradient2));

    // test nullptr
    EXPECT_FALSE(gradient1->IsNearlyEqual(nullptr));
}


/*
 * @tc.name: SymbolLineGradient001
 * @tc.desc: test SymbolLineGradient
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolGradientTest, SymbolLineGradient001, TestSize.Level0)
{
    // init data
    SymbolLineGradient gradient = SymbolLineGradient(45.0f); // 45.0f is angle of linegradient
    // 0XFF00FF00 and 0XFFFF0000 is ARGB
    std::vector<Drawing::Color> colors = {Drawing::Color(0XFF00FF00), Drawing::Color(0XFFFF0000)};
    std::vector<float> positions = {0.2f, 0.9f}; // 0.2f and 0.9f is position of colors
    // 0.0f left, 0.0f top, 100.0f right, 100.0f bottom
    Drawing::Rect bounds = Drawing::Rect(0.0f, 0.0f, 100.0f, 100.0f);
    Drawing::Point offset = Drawing::Point(0.5f, 0.6f); // 0.5f: x, 0.6f: y

    // test colors is empty
    auto brush = gradient.CreateGradientBrush(offset);
    auto pen = gradient.CreateGradientPen(offset);
    EXPECT_EQ(brush.GetShaderEffect(), nullptr);

    // test colors is not empty
    gradient.SetColors(colors);
    gradient.SetPositions(positions);
    gradient.SetPositions(positions);
    gradient.Make(bounds);
    brush = gradient.CreateGradientBrush(offset);
    pen = gradient.CreateGradientPen(offset);
    EXPECT_NE(brush.GetShaderEffect(), nullptr);
}

/*
 * @tc.name: PointsFromAngle001
 * @tc.desc: test PointsFromAngle
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolGradientTest, PointsFromAngle001, TestSize.Level0)
{
    // init data
    // 0.0f left, 0.0f top, 100.0f right, 100.0f bottom
    Drawing::Rect bounds = Drawing::Rect(0.0f, 0.0f, 100.0f, 100.0f);
    Drawing::Point firstPoint;
    Drawing::Point secondPoint;
    SymbolLineGradient gradient = SymbolLineGradient(-45.0f); // -45.0f is angle
    gradient.Make(bounds);

    // test angle is 0.0f
    gradient.PointsFromAngle(0.0f, bounds, firstPoint, secondPoint);
    EXPECT_EQ(firstPoint.GetY(), bounds.GetHeight());

    // test angle is 90.0f
    gradient.PointsFromAngle(90.0f, bounds, firstPoint, secondPoint);
    EXPECT_EQ(secondPoint.GetX(), bounds.GetWidth());

    // test angle is 180.0f
    gradient.PointsFromAngle(180.0f, bounds, firstPoint, secondPoint);
    EXPECT_EQ(secondPoint.GetY(), bounds.GetHeight());

    // test angle is 270.0f
    gradient.PointsFromAngle(270.0f, bounds, firstPoint, secondPoint);
    EXPECT_EQ(firstPoint.GetX(), bounds.GetWidth());

    auto half = 50.0f; // 50.0f is half of widht
    // test angle is 45.0f < 90.0f
    gradient.PointsFromAngle(45.0f, bounds, firstPoint, secondPoint);
    EXPECT_TRUE(secondPoint.GetY() < half);
    EXPECT_TRUE(secondPoint.GetX() > half);

    // test angle is 135.0f <180.0f
    gradient.PointsFromAngle(135.0f, bounds, firstPoint, secondPoint);
    EXPECT_TRUE(secondPoint.GetY() > half);
    EXPECT_TRUE(secondPoint.GetX() > half);

    // test angle is 200.0f < 270.0f
    gradient.PointsFromAngle(200.0f, bounds, firstPoint, secondPoint);
    EXPECT_TRUE(secondPoint.GetY() > half);
    EXPECT_TRUE(secondPoint.GetX() < half);

    // test angle is 300.0f < 360.0f
    gradient.PointsFromAngle(300.0f, bounds, firstPoint, secondPoint);
    EXPECT_TRUE(secondPoint.GetY() < half);
    EXPECT_TRUE(secondPoint.GetX() < half);
}

/*
 * @tc.name: SymbolRadialGradient001
 * @tc.desc: test PointsFromAngle
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolGradientTest, SymbolRadialGradient001, TestSize.Level0)
{
    // init data
    Drawing::Point centerPt = Drawing::Point(0.5f, 0.5f); // 0.5f: x, 0.5f: y
    float radiusRatio = 0.6f; // 0.6f is radius
    SymbolRadialGradient gradient = SymbolRadialGradient(centerPt, radiusRatio);
    // 0xFF00FF00 and 0XFFFF0000 is ARGB
    std::vector<Drawing::Color> colors = {Drawing::Color(0xFF00FF00), Drawing::Color(0XFFFF0000)};
    std::vector<float> positions = {0.2f, 0.9f}; // 0.2f and 0.9f is position of colors
    // 0.0f left, 0.0f top, 100.0f right, 100.0f bottom
    Drawing::Rect bounds = Drawing::Rect(0.0f, 0.0f, 100.0f, 100.0f);
    Drawing::Point offset = Drawing::Point(0.5f, 0.6f); // 0.5f: x, 0.6f: y

    // test colors is empty
    auto brush = gradient.CreateGradientBrush(offset);
    auto pen = gradient.CreateGradientPen(offset);
    EXPECT_EQ(brush.GetShaderEffect(), nullptr);

    // test colors is not empty
    gradient.SetColors(colors);
    gradient.SetPositions(positions);
    gradient.SetPositions(positions);
    gradient.SetCenterPoint(centerPt);
    gradient.SetRadiusRatio(radiusRatio);
    gradient.Make(bounds);
    brush = gradient.CreateGradientBrush(offset);
    pen = gradient.CreateGradientPen(offset);
    EXPECT_NE(brush.GetShaderEffect(), nullptr);

    // test SetRadius
    gradient.SetRadius(10.0f); // 10.0f is radius of radialGradient
    EXPECT_EQ(gradient.GetRadius(), 10.0f); // 10.0f is radius
    gradient.Make(bounds);
    EXPECT_FALSE(gradient.isRadiusRatio_);
}

/*
 * @tc.name: SymbolRadialGradient_SetRadius
 * @tc.desc: test SetRadius of SymbolRadialGradient by radius < 0
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolGradientTest, SymbolRadialGradient_SetRadius, TestSize.Level0)
{
    Drawing::Point centerPt = Drawing::Point(0.5f, 0.5f); // 0.5f: x, 0.5f: y
    float radiusRatio = -0.6f; // -0.6f test radiusRatio < 0
    SymbolRadialGradient gradient = SymbolRadialGradient(centerPt, radiusRatio);
    EXPECT_EQ(gradient.GetRadiusRatio(), 0.0f);

    gradient.SetRadius(-10.5f); // -10.5f is radius < 0
    EXPECT_EQ(gradient.GetRadius(), 0.0f);
    gradient.SetRadiusRatio(-0.5f); // -0.5f is RadiusRatios < 0
    EXPECT_EQ(gradient.GetRadiusRatio(), 0.0f);
}

/*
 * @tc.name: UIColor_SetGet001
 * @tc.desc: test SetUIColors and GetUIColors and HasUIColor
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolGradientTest, UIColor_SetGet001, TestSize.Level0)
{
    SymbolGradient gradient;
    // test HasUIColor returns false by default
    EXPECT_FALSE(gradient.HasUIColor());

    // test SetUIColors and GetUIColors
    Drawing::UIColor uiColor(1.0f, 0.5f, 0.3f, 1.0f); // RGBA
    gradient.SetUIColors({ uiColor }, SymbolColorSpace::SRGB);
    EXPECT_TRUE(gradient.HasUIColor());

    auto uiColors = gradient.GetUIColors();
    ASSERT_EQ(uiColors.size(), 1); // 1 is the size of uiColors
    EXPECT_FLOAT_EQ(uiColors[0].GetRed(), 1.0f);
    EXPECT_FLOAT_EQ(uiColors[0].GetGreen(), 0.5f);
    EXPECT_FLOAT_EQ(uiColors[0].GetBlue(), 0.3f);
    EXPECT_FLOAT_EQ(uiColors[0].GetAlpha(), 1.0f);
}

/*
 * @tc.name: UIColor_ColorSpace001
 * @tc.desc: test GetColorSpace with different color spaces
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolGradientTest, UIColor_ColorSpace001, TestSize.Level0)
{
    SymbolGradient gradient;
    // default color space is SRGB
    EXPECT_EQ(gradient.GetColorSpace(), SymbolColorSpace::SRGB);

    Drawing::UIColor uiColor(1.0f, 0.0f, 0.0f, 1.0f);
    gradient.SetUIColors({ uiColor }, SymbolColorSpace::DISPLAY_P3);
    EXPECT_EQ(gradient.GetColorSpace(), SymbolColorSpace::DISPLAY_P3);

    gradient.SetUIColors({ uiColor }, SymbolColorSpace::BT2020);
    EXPECT_EQ(gradient.GetColorSpace(), SymbolColorSpace::BT2020);

    gradient.SetUIColors({ uiColor }, SymbolColorSpace::SRGB);
    EXPECT_EQ(gradient.GetColorSpace(), SymbolColorSpace::SRGB);
}

/*
 * @tc.name: IsNearlyEqual_UIColor001
 * @tc.desc: test IsNearlyEqual with UIColor comparison
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolGradientTest, IsNearlyEqual_UIColor001, TestSize.Level0)
{
    auto gradient1 = std::make_shared<SymbolGradient>();
    auto gradient2 = std::make_shared<SymbolGradient>();
    // 0xFF00FF00 and 0XFFFF0000 is ARGB
    std::vector<Drawing::Color> colors = {Drawing::Color(0xFF00FF00), Drawing::Color(0XFFFF0000)};
    gradient1->SetColors(colors);
    gradient2->SetColors(colors);
    EXPECT_TRUE(gradient1->IsNearlyEqual(gradient2));

    // test one has UIColor, the other doesn't
    Drawing::UIColor uiColor(1.0f, 0.0f, 0.0f, 1.0f);
    gradient1->SetUIColors({ uiColor }, SymbolColorSpace::SRGB);
    EXPECT_FALSE(gradient1->IsNearlyEqual(gradient2));

    // test same UIColor
    gradient2->SetUIColors({ uiColor }, SymbolColorSpace::SRGB);
    EXPECT_TRUE(gradient1->IsNearlyEqual(gradient2));

    // test different UIColor
    Drawing::UIColor uiColor2(0.0f, 1.0f, 0.0f, 1.0f);
    gradient1->SetUIColors({ uiColor2 }, SymbolColorSpace::SRGB);
    EXPECT_FALSE(gradient1->IsNearlyEqual(gradient2));
}

/*
 * @tc.name: CreateGradientBrush_UIColor001
 * @tc.desc: test CreateGradientBrush with UIColor for SymbolGradient
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolGradientTest, CreateGradientBrush_UIColor001, TestSize.Level0)
{
    SymbolGradient gradient;
    Drawing::UIColor uiColor(1.0f, 0.5f, 0.3f, 1.0f);
    gradient.SetUIColors({ uiColor }, SymbolColorSpace::SRGB);
    // 0.0f left, 0.0f top, 100.0f right, 100.0f bottom
    Drawing::Rect bounds = Drawing::Rect(0.0f, 0.0f, 100.0f, 100.0f);
    gradient.Make(bounds);
    auto brush = gradient.CreateGradientBrush();
    EXPECT_TRUE(brush.IsAntiAlias());
}

/*
 * @tc.name: CreateGradientPen_UIColor001
 * @tc.desc: test CreateGradientPen with UIColor for SymbolGradient
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolGradientTest, CreateGradientPen_UIColor001, TestSize.Level0)
{
    SymbolGradient gradient;
    Drawing::UIColor uiColor(1.0f, 0.5f, 0.3f, 1.0f);
    gradient.SetUIColors({ uiColor }, SymbolColorSpace::DISPLAY_P3);
    // 0.0f left, 0.0f top, 100.0f right, 100.0f bottom
    Drawing::Rect bounds = Drawing::Rect(0.0f, 0.0f, 100.0f, 100.0f);
    gradient.Make(bounds);
    auto pen = gradient.CreateGradientPen();
    EXPECT_TRUE(pen.IsAntiAlias());
}

/*
 * @tc.name: SymbolLineGradient_UIColor001
 * @tc.desc: test SymbolLineGradient with UIColor
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolGradientTest, SymbolLineGradient_UIColor001, TestSize.Level0)
{
    SymbolLineGradient gradient(45.0f); // 45.0f is angle
    // 0XFF00FF00 and 0XFFFF0000 is ARGB
    std::vector<Drawing::Color> colors = {Drawing::Color(0XFF00FF00), Drawing::Color(0XFFFF0000)};
    std::vector<float> positions = {0.2f, 0.9f}; // 0.2f and 0.9f is position of colors
    Drawing::UIColor uiColor(1.0f, 0.5f, 0.3f, 1.0f);
    gradient.SetUIColors({ uiColor }, SymbolColorSpace::SRGB);
    EXPECT_TRUE(gradient.HasUIColor());

    // 0.0f left, 0.0f top, 100.0f right, 100.0f bottom
    Drawing::Rect bounds = Drawing::Rect(0.0f, 0.0f, 100.0f, 100.0f);
    Drawing::Point offset = Drawing::Point(0.5f, 0.6f); // 0.5f: x, 0.6f: y
    gradient.SetColors(colors);
    gradient.SetPositions(positions);
    gradient.Make(bounds);
    auto brush = gradient.CreateGradientBrush(offset);
    auto pen = gradient.CreateGradientPen(offset);
    EXPECT_NE(brush.GetShaderEffect(), nullptr);
    EXPECT_TRUE(brush.IsAntiAlias());
    EXPECT_TRUE(pen.IsAntiAlias());
}

/*
 * @tc.name: SymbolRadialGradient_UIColor001
 * @tc.desc: test SymbolRadialGradient with UIColor
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolGradientTest, SymbolRadialGradient_UIColor001, TestSize.Level0)
{
    Drawing::Point centerPt = Drawing::Point(0.5f, 0.5f); // 0.5f: x, 0.5f: y
    float radiusRatio = 0.6f; // 0.6f is radius ratio
    SymbolRadialGradient gradient(centerPt, radiusRatio);
    // 0XFF00FF00 and 0XFFFF0000 is ARGB
    std::vector<Drawing::Color> colors = {Drawing::Color(0XFF00FF00), Drawing::Color(0XFFFF0000)};
    std::vector<float> positions = {0.2f, 0.9f}; // 0.2f and 0.9f is position of colors
    Drawing::UIColor uiColor(1.0f, 0.5f, 0.3f, 1.0f);
    gradient.SetUIColors({ uiColor }, SymbolColorSpace::BT2020);
    EXPECT_TRUE(gradient.HasUIColor());
    EXPECT_EQ(gradient.GetColorSpace(), SymbolColorSpace::BT2020);

    // 0.0f left, 0.0f top, 100.0f right, 100.0f bottom
    Drawing::Rect bounds = Drawing::Rect(0.0f, 0.0f, 100.0f, 100.0f);
    Drawing::Point offset = Drawing::Point(0.5f, 0.6f); // 0.5f: x, 0.6f: y
    gradient.SetColors(colors);
    gradient.SetPositions(positions);
    gradient.Make(bounds);
    auto brush = gradient.CreateGradientBrush(offset);
    auto pen = gradient.CreateGradientPen(offset);
    EXPECT_NE(brush.GetShaderEffect(), nullptr);
    EXPECT_TRUE(brush.IsAntiAlias());
    EXPECT_TRUE(pen.IsAntiAlias());
}

/*
 * @tc.name: UIColor_BothColorAndUIColor001
 * @tc.desc: test SymbolGradient with both Color and UIColor set, UIColor takes priority
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolGradientTest, UIColor_BothColorAndUIColor001, TestSize.Level0)
{
    SymbolGradient gradient;
    // Set Drawing::Color first
    gradient.SetColors({Drawing::Color(0xFF00FF00)}); // 0xFF00FF00 is ARGB
    EXPECT_FALSE(gradient.GetColors().empty());
    EXPECT_FALSE(gradient.HasUIColor());

    // Set UIColor
    Drawing::UIColor uiColor(1.0f, 0.5f, 0.3f, 1.0f);
    gradient.SetUIColors({ uiColor }, SymbolColorSpace::DISPLAY_P3);

    // Both are stored, UIColor takes priority in CreateGradientBrush/Pen
    EXPECT_FALSE(gradient.GetColors().empty());
    EXPECT_TRUE(gradient.HasUIColor());
    EXPECT_EQ(gradient.GetColorSpace(), SymbolColorSpace::DISPLAY_P3);

    Drawing::Rect bounds = Drawing::Rect(0.0f, 0.0f, 100.0f, 100.0f);
    gradient.Make(bounds);
    auto brush = gradient.CreateGradientBrush();
    auto pen = gradient.CreateGradientPen();
    EXPECT_TRUE(brush.IsAntiAlias());
    EXPECT_TRUE(pen.IsAntiAlias());
}

/*
 * @tc.name: SymbolLineGradient_BothColorAndUIColor001
 * @tc.desc: test SymbolLineGradient with both Color and UIColor set
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolGradientTest, SymbolLineGradient_BothColorAndUIColor001, TestSize.Level0)
{
    SymbolLineGradient gradient(90.0f); // 90.0f is angle
    // 0XFF00FF00 and 0XFFFF0000 is ARGB
    std::vector<Drawing::Color> colors = {Drawing::Color(0XFF00FF00), Drawing::Color(0XFFFF0000)};
    std::vector<float> positions = {0.2f, 0.9f}; // 0.2f and 0.9f is position of colors
    gradient.SetColors(colors);
    gradient.SetPositions(positions);

    Drawing::UIColor uiColor(1.0f, 0.5f, 0.3f, 1.0f);
    gradient.SetUIColors({ uiColor }, SymbolColorSpace::BT2020);

    EXPECT_FALSE(gradient.GetColors().empty());
    EXPECT_TRUE(gradient.HasUIColor());
    EXPECT_EQ(gradient.GetColorSpace(), SymbolColorSpace::BT2020);

    Drawing::Rect bounds = Drawing::Rect(0.0f, 0.0f, 100.0f, 100.0f);
    Drawing::Point offset = Drawing::Point(0.5f, 0.6f); // 0.5f: x, 0.6f: y
    gradient.Make(bounds);
    auto brush = gradient.CreateGradientBrush(offset);
    auto pen = gradient.CreateGradientPen(offset);
    EXPECT_NE(brush.GetShaderEffect(), nullptr);
    EXPECT_TRUE(brush.IsAntiAlias());
    EXPECT_TRUE(pen.IsAntiAlias());
}

/*
 * @tc.name: SymbolRadialGradient_BothColorAndUIColor001
 * @tc.desc: test SymbolRadialGradient with both Color and UIColor set
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolGradientTest, SymbolRadialGradient_BothColorAndUIColor001, TestSize.Level0)
{
    Drawing::Point centerPt = Drawing::Point(0.5f, 0.5f); // 0.5f: x, 0.5f: y
    float radiusRatio = 0.6f; // 0.6f is radius ratio
    SymbolRadialGradient gradient(centerPt, radiusRatio);
    // 0XFF00FF00 and 0XFFFF0000 is ARGB
    std::vector<Drawing::Color> colors = {Drawing::Color(0XFF00FF00), Drawing::Color(0XFFFF0000)};
    std::vector<float> positions = {0.2f, 0.9f}; // 0.2f and 0.9f is position of colors
    gradient.SetColors(colors);
    gradient.SetPositions(positions);

    Drawing::UIColor uiColor(1.0f, 0.5f, 0.3f, 1.0f);
    gradient.SetUIColors({ uiColor }, SymbolColorSpace::SRGB);

    EXPECT_FALSE(gradient.GetColors().empty());
    EXPECT_TRUE(gradient.HasUIColor());

    Drawing::Rect bounds = Drawing::Rect(0.0f, 0.0f, 100.0f, 100.0f);
    Drawing::Point offset = Drawing::Point(0.5f, 0.6f); // 0.5f: x, 0.6f: y
    gradient.Make(bounds);
    auto brush = gradient.CreateGradientBrush(offset);
    auto pen = gradient.CreateGradientPen(offset);
    EXPECT_NE(brush.GetShaderEffect(), nullptr);
    EXPECT_TRUE(brush.IsAntiAlias());
    EXPECT_TRUE(pen.IsAntiAlias());
}

/*
 * @tc.name: IsNearlyEqual_BothColorAndUIColor001
 * @tc.desc: test IsNearlyEqual with both Color and UIColor set
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolGradientTest, IsNearlyEqual_BothColorAndUIColor001, TestSize.Level0)
{
    auto gradient1 = std::make_shared<SymbolGradient>();
    auto gradient2 = std::make_shared<SymbolGradient>();
    // 0xFF00FF00 is ARGB
    std::vector<Drawing::Color> colors = {Drawing::Color(0xFF00FF00)};
    Drawing::UIColor uiColor(1.0f, 0.5f, 0.3f, 1.0f);

    // Both have same Color + same UIColor
    gradient1->SetColors(colors);
    gradient1->SetUIColors({ uiColor }, SymbolColorSpace::SRGB);
    gradient2->SetColors(colors);
    gradient2->SetUIColors({ uiColor }, SymbolColorSpace::SRGB);
    EXPECT_TRUE(gradient1->IsNearlyEqual(gradient2));

    // Same Color, different UIColor
    Drawing::UIColor uiColor2(0.0f, 1.0f, 0.0f, 1.0f);
    gradient2->SetUIColors({ uiColor2 }, SymbolColorSpace::SRGB);
    EXPECT_FALSE(gradient1->IsNearlyEqual(gradient2));
}

/*
 * @tc.name: UIColor_Headroom001
 * @tc.desc: test UIColor headroom set and get with HDR values
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolGradientTest, UIColor_Headroom001, TestSize.Level0)
{
    SymbolGradient gradient;
    // default headroom is 1.0
    Drawing::UIColor uiColorDefault(1.0f, 0.5f, 0.3f, 1.0f);
    EXPECT_FLOAT_EQ(uiColorDefault.GetHeadroom(), 1.0f);

    // SetHeadroom sets HDR headroom > 1.0 (brighter)
    Drawing::UIColor uiColorBright(1.0f, 0.5f, 0.3f, 1.0f);
    uiColorBright.SetHeadroom(2.0f);
    EXPECT_FLOAT_EQ(uiColorBright.GetHeadroom(), 2.0f);

    // SetHeadroom sets HDR headroom = 4.0 (maximum)
    Drawing::UIColor uiColor(1.0f, 0.5f, 0.3f, 1.0f);
    uiColor.SetHeadroom(4.0f);
    EXPECT_FLOAT_EQ(uiColor.GetHeadroom(), 4.0f);

    // Verify UIColor with headroom stored in gradient
    gradient.SetUIColors({ uiColor }, SymbolColorSpace::BT2020);
    auto uiColors = gradient.GetUIColors();
    ASSERT_EQ(uiColors.size(), 1);
    EXPECT_FLOAT_EQ(uiColors[0].GetHeadroom(), 4.0f);
    EXPECT_FLOAT_EQ(uiColors[0].GetRed(), 1.0f);
    EXPECT_FLOAT_EQ(uiColors[0].GetGreen(), 0.5f);
    EXPECT_FLOAT_EQ(uiColors[0].GetBlue(), 0.3f);
}

/*
 * @tc.name: UIColor_ValuesAboveOne001
 * @tc.desc: test UIColor with RGBA values > 1.0 (HDR range)
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolGradientTest, UIColor_ValuesAboveOne001, TestSize.Level0)
{
    // HDR color with RGBA > 1.0, headroom set via SetHeadroom
    Drawing::UIColor uiColor(2.0f, 3.5f, 1.5f, 1.0f);
    uiColor.SetHeadroom(4.0f);
    EXPECT_FLOAT_EQ(uiColor.GetRed(), 2.0f);
    EXPECT_FLOAT_EQ(uiColor.GetGreen(), 3.5f);
    EXPECT_FLOAT_EQ(uiColor.GetBlue(), 1.5f);
    EXPECT_FLOAT_EQ(uiColor.GetAlpha(), 1.0f);
    EXPECT_FLOAT_EQ(uiColor.GetHeadroom(), 4.0f);

    SymbolGradient gradient;
    gradient.SetUIColors({ uiColor }, SymbolColorSpace::DISPLAY_P3);
    EXPECT_TRUE(gradient.HasUIColor());

    auto uiColors = gradient.GetUIColors();
    ASSERT_EQ(uiColors.size(), 1);
    EXPECT_FLOAT_EQ(uiColors[0].GetRed(), 2.0f);
    EXPECT_FLOAT_EQ(uiColors[0].GetGreen(), 3.5f);
    EXPECT_FLOAT_EQ(uiColors[0].GetBlue(), 1.5f);
    EXPECT_FLOAT_EQ(uiColors[0].GetHeadroom(), 4.0f);
}

/*
 * @tc.name: UIColor_ValuesBelowOne001
 * @tc.desc: test UIColor with RGBA values < 1.0 (dim HDR)
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolGradientTest, UIColor_ValuesBelowOne001, TestSize.Level0)
{
    // Dim color with RGBA < 1.0, headroom default 1.0
    Drawing::UIColor uiColor(0.1f, 0.2f, 0.05f, 0.3f);
    EXPECT_FLOAT_EQ(uiColor.GetRed(), 0.1f);
    EXPECT_FLOAT_EQ(uiColor.GetGreen(), 0.2f);
    EXPECT_FLOAT_EQ(uiColor.GetBlue(), 0.05f);
    EXPECT_FLOAT_EQ(uiColor.GetAlpha(), 0.3f);
    EXPECT_FLOAT_EQ(uiColor.GetHeadroom(), 1.0f);

    SymbolGradient gradient;
    gradient.SetUIColors({ uiColor }, SymbolColorSpace::SRGB);

    auto uiColors = gradient.GetUIColors();
    ASSERT_EQ(uiColors.size(), 1);
    EXPECT_FLOAT_EQ(uiColors[0].GetRed(), 0.1f);
    EXPECT_FLOAT_EQ(uiColors[0].GetGreen(), 0.2f);
    EXPECT_FLOAT_EQ(uiColors[0].GetBlue(), 0.05f);
    EXPECT_FLOAT_EQ(uiColors[0].GetAlpha(), 0.3f);
    EXPECT_FLOAT_EQ(uiColors[0].GetHeadroom(), 1.0f);
}

/*
 * @tc.name: UIColor_AlphaClamp001
 * @tc.desc: test UIColor alpha is clamped to [0.0, 1.0] by Drawing layer
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolGradientTest, UIColor_AlphaClamp001, TestSize.Level0)
{
    // alpha > 1.0 clamped to 1.0
    Drawing::UIColor uiColorOver(1.0f, 0.5f, 0.3f, 2.0f);
    EXPECT_FLOAT_EQ(uiColorOver.GetAlpha(), 1.0f);

    // alpha = 0.0 is valid
    Drawing::UIColor uiColorZero(1.0f, 0.5f, 0.3f, 0.0f);
    EXPECT_FLOAT_EQ(uiColorZero.GetAlpha(), 0.0f);
    EXPECT_TRUE(uiColorZero.GetRed() > 0.0f);

    // alpha < 0.0 clamped to 0.0
    Drawing::UIColor uiColorNeg(1.0f, 0.5f, 0.3f, -1.0f);
    EXPECT_FLOAT_EQ(uiColorNeg.GetAlpha(), 0.0f);
}

/*
 * @tc.name: UIColor_RGBBelowZero001
 * @tc.desc: test UIColor negative RGB values are clamped to 0.0 by Drawing layer
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolGradientTest, UIColor_RGBBelowZero001, TestSize.Level0)
{
    // Negative RGB values clamped to 0.0
    Drawing::UIColor uiColor(-1.0f, -0.5f, -0.3f, 1.0f);
    EXPECT_FLOAT_EQ(uiColor.GetRed(), 0.0f);
    EXPECT_FLOAT_EQ(uiColor.GetGreen(), 0.0f);
    EXPECT_FLOAT_EQ(uiColor.GetBlue(), 0.0f);
}

/*
 * @tc.name: IsNearlyEqual_UIColor_Headroom001
 * @tc.desc: test IsNearlyEqual with different headroom values
 * @tc.type: FUNC
 */
HWTEST_F(OHHmSymbolGradientTest, IsNearlyEqual_UIColor_Headroom001, TestSize.Level0)
{
    auto gradient1 = std::make_shared<SymbolGradient>();
    auto gradient2 = std::make_shared<SymbolGradient>();

    // Same UIColor with same headroom via SetHeadroom
    Drawing::UIColor uiColor1(1.0f, 0.5f, 0.3f, 1.0f);
    uiColor1.SetHeadroom(2.0f);
    Drawing::UIColor uiColor2(1.0f, 0.5f, 0.3f, 1.0f);
    uiColor2.SetHeadroom(2.0f);
    gradient1->SetUIColors({ uiColor1 }, SymbolColorSpace::SRGB);
    gradient2->SetUIColors({ uiColor2 }, SymbolColorSpace::SRGB);
    EXPECT_TRUE(gradient1->IsNearlyEqual(gradient2));

    // Same RGBA, different headroom
    Drawing::UIColor uiColor3(1.0f, 0.5f, 0.3f, 1.0f);
    uiColor3.SetHeadroom(4.0f);
    gradient2->SetUIColors({ uiColor3 }, SymbolColorSpace::SRGB);
    EXPECT_FALSE(gradient1->IsNearlyEqual(gradient2));

    // Different uiColorSize
    gradient2->SetUIColors({ uiColor3, uiColor3}, SymbolColorSpace::SRGB);
    EXPECT_FALSE(gradient1->IsNearlyEqual(gradient2));
}
} // namespace Rosen
} // namespace OHOS