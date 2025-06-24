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
    std::vector<Drawing::ColorQuad> colors = {0xFF00FF00, 0XFFFF0000}; // 0xFF00FF00 and 0XFFFF0000 is ARGB
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
    std::vector<Drawing::ColorQuad> colors = {0xFF00FF00, 0XFFFF0000}; // 0xFF00FF00 and 0XFFFF0000 is ARGB
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

    std::vector<Drawing::ColorQuad> colors1 = {0xFF00FFFF, 0XFFFF0000}; // 0xFF00FFFF and 0XFFFF0000 is ARGB
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
    std::vector<Drawing::ColorQuad> colors = {0XFF00FF00, 0XFFFF0000}; // 0XFF00FF00 and 0XFFFF0000 is ARGB
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
    std::vector<Drawing::ColorQuad> colors = {0xFF00FF00, 0XFFFF0000}; // 0xFF00FF00 and 0XFFFF0000 is ARGB
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
} // namespace Rosen
} // namespace OHOS