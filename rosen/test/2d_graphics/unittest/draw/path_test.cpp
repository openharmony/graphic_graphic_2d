/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

#include "draw/path.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class PathTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void PathTest::SetUpTestCase() {}
void PathTest::TearDownTestCase() {}
void PathTest::SetUp() {}
void PathTest::TearDown() {}

/**
 * @tc.name: CreateAndDestroy001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathTest, CreateAndDestroy001, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
}

/**
 * @tc.name: BuildFromSVGString001
 * @tc.desc: Test for Parsing the SVG format string and sets the Path.
 * @tc.type: FUNC
 * @tc.require: I715J0
 */
HWTEST_F(PathTest, BuildFromSVGString001, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    std::string str;
    EXPECT_TRUE(path->BuildFromSVGString(str));
}

/**
 * @tc.name: BuildFromSVGString002
 * @tc.desc: Test for Parsing the SVG format string and sets the Path.
 * @tc.type: FUNC
 * @tc.require: I715J0
 */
HWTEST_F(PathTest, BuildFromSVGString002, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    std::string str = "string";
    EXPECT_FALSE(path->BuildFromSVGString(str));
}

/**
 * @tc.name: BuildFromSVGString003
 * @tc.desc: Test for Parsing the SVG format string and sets the Path.
 * @tc.type: FUNC
 * @tc.require: I715J0
 */
HWTEST_F(PathTest, BuildFromSVGString003, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    path->AddRect(1.0f, 4.0f, 3.0f, 2.0f);
    EXPECT_TRUE(path->BuildFromSVGString(path->ConvertToSVGString()));
}

/**
 * @tc.name: ConvertToSVGString001
 * @tc.desc: Test for Parsing into a string in SVG format that describes the Path.
 * @tc.type: FUNC
 * @tc.require: I715J0
 */
HWTEST_F(PathTest, ConvertToSVGString001, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    EXPECT_EQ(path->ConvertToSVGString(), "");
}

/**
 * @tc.name: ConvertToSVGString002
 * @tc.desc: Test for Parsing into a string in SVG format that describes the Path.
 * @tc.type: FUNC
 * @tc.require: I715J0
 */
HWTEST_F(PathTest, ConvertToSVGString002, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    path->AddRect(1.0f, 4.0f, 3.0f, 2.0f);
    EXPECT_TRUE(path->ConvertToSVGString() != "");
}

/**
 * @tc.name: MoveTo001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathTest, MoveTo001, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    path->MoveTo(5.0f, 4.5f);
}

/**
 * @tc.name: MoveTo002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathTest, MoveTo002, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    path->MoveTo(4.5f, 5.0f);
}

/**
 * @tc.name: LineTo001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathTest, LineTo001, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    path->LineTo(4.5f, 5.0f);
}

/**
 * @tc.name: LineTo002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathTest, LineTo002, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    path->LineTo(1.0f, 3.0f);
}

/**
 * @tc.name: ArcTo001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathTest, ArcTo001, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    path->ArcTo(1.0f, 3.0f, 2.2f, 2.3f, 0.0f, 5.0f);
}

/**
 * @tc.name: ArcTo002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathTest, ArcTo002, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    path->ArcTo(1.0f, 3.0f, 2.5f, 2.4f, 1.0f, 3.0f);
}

/**
 * @tc.name: ArcTo003
 * @tc.desc: Arc To Direction Test
 * @tc.type: FUNC
 * @tc.require: issuel#I6Q4ZH
 */
HWTEST_F(PathTest, ArcTo003, TestSize.Level2)
{
    Path path;
    path.ArcTo(1.0f, 3.0f, 2.5f, PathDirection::CCW_DIRECTION, 1.0f, 3.0f);
    ASSERT_TRUE(path.IsValid());
}

/**
 * @tc.name: ArcToWith6001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathTest, ArcToWith6001, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    Point point1;
    Point point2;
    path->ArcTo(point1, point2, 2.5f, 2.4f);
}

/**
 * @tc.name: ArcToWith6002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathTest, ArcToWith6002, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    Point point1;
    Point point2;
    path->ArcTo(point1, point2, 2.5f, 2.0f);
}

/**
 * @tc.name: CubicTo001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathTest, CubicTo001, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    path->CubicTo(1.0f, 2.3f, 2.5f, 2.0f, 3.5f, 3.0f);
}

/**
 * @tc.name: CubicTo002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathTest, CubicTo002, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    path->CubicTo(1.0f, 2.3f, 1.4f, 2.0f, 1.5f, 3.0f);
}

/**
 * @tc.name: CubicTo2001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathTest, CubicTo2001, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    Point point1;
    Point point2;
    Point endPoint(2.3f, 1.5f);
    path->CubicTo(point1, point2, endPoint);
}

/**
 * @tc.name: CubicTo2002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathTest, CubicTo2002, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    Point point1(1.2f, 0.0f);
    Point point2(1.3f, 1.0f);
    Point endPoint(2.3f, 1.5f);
    path->CubicTo(point1, point2, endPoint);
}

/**
 * @tc.name: QuadTo2001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathTest, QuadTo2001, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    Point point1(1.2f, 0.0f);
    Point endPoint(2.3f, 1.5f);
    path->QuadTo(point1, endPoint);
}

/**
 * @tc.name: QuadTo2002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathTest, QuadTo2002, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    Point point1(0.5f, 0.3f);
    Point endPoint(3.5f, 3.3f);
    path->QuadTo(point1, endPoint);
}

/**
 * @tc.name: QuadTo4001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathTest, QuadTo4001, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    path->QuadTo(1.0f, 1.5f, 3.3f, 4.5f);
}

/**
 * @tc.name: QuadTo4002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathTest, QuadTo4002, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    path->QuadTo(1.0f, 1.2f, 3.0f, 4.0f);
}

/**
 * @tc.name: AddRect2001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathTest, AddRect2001, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    Rect rect;
    path->AddRect(rect);
}

/**
 * @tc.name: AddRect2002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathTest, AddRect2002, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    Rect rect;
    path->AddRect(rect, PathDirection::CCW_DIRECTION);
}

/**
 * @tc.name: AddRect5001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathTest, AddRect5001, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    path->AddRect(1.0f, 4.0f, 3.0f, 2.0f, PathDirection::CCW_DIRECTION);
}

/**
 * @tc.name: AddRect5002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathTest, AddRect5002, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    path->AddRect(1.0f, 4.0f, 3.0f, 2.0f);
}

/**
 * @tc.name: AddOval001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathTest, AddOval001, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    Rect oval;
    path->AddOval(oval, PathDirection::CCW_DIRECTION);
}

/**
 * @tc.name: AddOval002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathTest, AddOval002, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    Rect oval;
    path->AddOval(oval);
}

/**
 * @tc.name: AddArc001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathTest, AddArc001, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    Rect rect;
    path->AddArc(rect, 1.0f, 2.0f);
}

/**
 * @tc.name: AddArc002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathTest, AddArc002, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    Rect rect;
    path->AddArc(rect, 2.0f, 1.0f);
}

/**
 * @tc.name: AddPoly001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathTest, AddPoly001, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    std::vector<Point> points;
    Point point1;
    points.push_back(point1);
    int size = points.size();
    path->AddPoly(points, size, false);
}

/**
 * @tc.name: AddPoly002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathTest, AddPoly002, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    std::vector<Point> points;
    Point point1;
    Point point2;
    points.push_back(point1);
    points.push_back(point2);
    int size = points.size();
    path->AddPoly(points, size, true);
}

/**
 * @tc.name: AddCircle001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathTest, AddCircle001, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    path->AddCircle(1.0f, 0.5f, 0.5f);
}

/**
 * @tc.name: AddCircle002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathTest, AddCircle002, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    path->AddCircle(1.0f, 0.5f, 0.5f, PathDirection::CCW_DIRECTION);
}

/**
 * @tc.name: AddRoundRect001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathTest, AddRoundRect001, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    Rect rect;
    path->AddRoundRect(rect, 0.5f, 0.5f, PathDirection::CCW_DIRECTION);
}

/**
 * @tc.name: AddRoundRect002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathTest, AddRoundRect002, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    Rect rect;
    path->AddRoundRect(rect, 0.5f, 0.5f);
}

/**
 * @tc.name: AddRoundRect003
 * @tc.desc: Test for adding the circle rectangle to the Path.
 * @tc.type: FUNC
 * @tc.require: I715J0
 */
HWTEST_F(PathTest, AddRoundRect003, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    RoundRect roundRect;
    path->AddRoundRect(roundRect, PathDirection::CCW_DIRECTION);
}

/**
 * @tc.name: AddRoundRect004
 * @tc.desc: Test for adding the circle rectangle to the Path.
 * @tc.type: FUNC
 * @tc.require: I715J0
 */
HWTEST_F(PathTest, AddRoundRect004, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    RoundRect roundRect;
    path->AddRoundRect(roundRect);
}

/**
 * @tc.name: AddRoundRect005
 * @tc.desc: Test for adding the circle rectangle to the Path.
 * @tc.type: FUNC
 * @tc.require: I715J0
 */
HWTEST_F(PathTest, AddRoundRect005, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    Rect rect;
    RoundRect roundRect(rect, 12.6f, 77.4f);
    path->AddRoundRect(roundRect);
}

/**
 * @tc.name: AddPath3001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathTest, AddPath3001, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    Path sourcePath;
    path->AddPath(sourcePath, 0.5f, 0.5f);
}

/**
 * @tc.name: AddPath3002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathTest, AddPath3002, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    Path sourcePath;
    path->AddPath(sourcePath, 1.0f, 1.0f);
}

/**
 * @tc.name: AddPath1001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathTest, AddPath1001, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    Path sourcePath;
    path->AddPath(sourcePath);
}

/**
 * @tc.name: AddPath2001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathTest, AddPath2001, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    Path path1;
    Matrix matrix;
    path->AddPath(path1, matrix);
}

/**
 * @tc.name: AddPath2002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathTest, AddPath2002, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    Path path1;
    Matrix matrix;
    path->AddPath(path1, matrix);
}

/**
 * @tc.name: ReverseAddPath001
 * @tc.desc: Test for adding the src from back forward to the Path.
 * @tc.type: FUNC
 * @tc.require: I715J0
 */
HWTEST_F(PathTest, ReverseAddPath001, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    Path path1;
    path->ReverseAddPath(path1);
}

/**
 * @tc.name: ReverseAddPath002
 * @tc.desc: Test for adding the src from back forward to the Path.
 * @tc.type: FUNC
 * @tc.require: I715J0
 */
HWTEST_F(PathTest, ReverseAddPath002, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    Path path2;
    path2.AddRect(1.0f, 4.0f, 3.0f, 2.0f);
    path->ReverseAddPath(path2);
}

/**
 * @tc.name: GetBounds001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathTest, GetBounds001, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    auto rect = path->GetBounds();
}

/**
 * @tc.name: SetFillStyle001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathTest, SetFillStyle001, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    path->SetFillStyle(PathFillType::WINDING);
}

/**
 * @tc.name: SetFillStyle002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathTest, SetFillStyle002, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    path->SetFillStyle(PathFillType::INVERSE_WINDING);
}

/**
 * @tc.name: GetFillStyle001
 * @tc.desc: Test Path's GetFillStyle
 * @tc.type: FUNC
 * @tc.require: IB742Z
 */
HWTEST_F(PathTest, GetFillStyle001, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    path->SetFillStyle(PathFillType::WINDING);
    EXPECT_TRUE(path->GetFillStyle() == PathFillType::WINDING);
    path->SetFillStyle(PathFillType::INVERSE_WINDING);
    EXPECT_TRUE(path->GetFillStyle() == PathFillType::INVERSE_WINDING);
}

/**
 * @tc.name: Interpolate001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathTest, Interpolate001, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    Path ending;
    Path out;
    path->Interpolate(ending, 0.5f, out);
}

/**
 * @tc.name: Interpolate002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathTest, Interpolate002, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    Path ending;
    Path out;
    path->Interpolate(ending, 0.2f, out);
}

/**
 * @tc.name: Transform001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathTest, Transform001, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    Matrix matrix;
    path->Transform(matrix);
}

/**
 * @tc.name: Offset001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathTest, Offset001, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    path->Offset(1.0f, 2.3f);
}

/**
 * @tc.name: Offset002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathTest, Offset002, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    path->Offset(2.3f, 1.0f);
}

/**
 * @tc.name: Op001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathTest, Op001, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    Path path1;
    Path path2;
    path->Op(path1, path2, PathOp::INTERSECT);
}

/**
 * @tc.name: Op002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathTest, Op002, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    Path path1;
    Path path2;
    path->Op(path1, path2, PathOp::UNION);
}

/**
 * @tc.name: IsValid001
 * @tc.desc: Test for Checking whether the Path is valid.
 * @tc.type: FUNC
 * @tc.require: I715J0
 */
HWTEST_F(PathTest, IsValid001, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    EXPECT_FALSE(path->IsValid());
}

/**
 * @tc.name: IsValid002
 * @tc.desc: Test for Checking whether the Path is valid.
 * @tc.type: FUNC
 * @tc.require: I715J0
 */
HWTEST_F(PathTest, IsValid002, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    path->AddRect(1.0f, 4.0f, 3.0f, 2.0f);
    EXPECT_TRUE(path->IsValid());
}

/**
 * @tc.name: Reset001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathTest, Reset001, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    path->Reset();
}

/**
 * @tc.name: Close001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require: AR000GGNV3
 * @tc.author:
 */
HWTEST_F(PathTest, Close001, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    path->Close();
}

/**
 * @tc.name: GetLength001
 * @tc.desc: Test for geting the length of the current path object.
 * @tc.type: FUNC
 * @tc.require: I715J0
 */
HWTEST_F(PathTest, GetLength001, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    EXPECT_EQ(path->GetLength(false), 0);
}

/**
 * @tc.name: GetLength002
 * @tc.desc: Test for geting the length of the current path object.
 * @tc.type: FUNC
 * @tc.require: I715J0
 */
HWTEST_F(PathTest, GetLength002, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    EXPECT_EQ(path->GetLength(true), 0);
}

/**
 * @tc.name: GetLength003
 * @tc.desc: Test for geting the length of the current path object.
 * @tc.type: FUNC
 * @tc.require: I715J0
 */
HWTEST_F(PathTest, GetLength003, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    path->AddRect(1.0f, 4.0f, 3.0f, 2.0f);
    EXPECT_EQ(path->GetLength(true), 8);
}

/**
 * @tc.name: GetPositionAndTangent001
 * @tc.desc: Test for geting the position and tangent of the distance from the starting position of the Path.
 * @tc.type: FUNC
 * @tc.require: I715J0
 */
HWTEST_F(PathTest, GetPositionAndTangent001, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    Point point1;
    Point point2;
    EXPECT_FALSE(path->GetPositionAndTangent(0, point1, point2, false));
}

/**
 * @tc.name: GetPositionAndTangent002
 * @tc.desc: Test for geting the position and tangent of the distance from the starting position of the Path.
 * @tc.type: FUNC
 * @tc.require: I715J0
 */
HWTEST_F(PathTest, GetPositionAndTangent002, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    Point point1;
    Point point2;
    path->AddRect(1.0f, 4.0f, 3.0f, 2.0f);
    EXPECT_TRUE(path->GetPositionAndTangent(10, point1, point2, true));
}

/**
 * @tc.name: GetPositionAndTangent003
 * @tc.desc: Test for geting the position and tangent of the distance from the starting position of the Path.
 * @tc.type: FUNC
 * @tc.require: I715J0
 */
HWTEST_F(PathTest, GetPositionAndTangent003, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    ASSERT_TRUE(path != nullptr);
    Point point1(0.5f, 0.3f);
    Point point2(3.5f, 3.3f);
    path->AddRect(1.0f, 4.0f, 3.0f, 2.0f);
    EXPECT_TRUE(path->GetPositionAndTangent(0.1f, point1, point2, false));
}

/**
 * @tc.name: GetSegment001
 * @tc.desc: Test for Gets the path between the start and end points.
 * @tc.type: FUNC
 * @tc.require: I715J0
 */
HWTEST_F(PathTest, GetSegment001, TestSize.Level1)
{
    Path path;
    path.MoveTo(100, 100);
    path.LineTo(100, 200);
    path.LineTo(200, 200);
    Path newPath;
    EXPECT_EQ(path.GetSegment(120, 180, &newPath, false, false), true);
    EXPECT_EQ(path.GetSegment(120, 280, &newPath, false, true), true);
    EXPECT_EQ(path.GetSegment(-50, 999, &newPath, false, true), true);
    EXPECT_EQ(path.GetSegment(120, 180, &newPath, true, false), true);
    EXPECT_EQ(path.GetSegment(120, 280, &newPath, true, true), true);
    EXPECT_EQ(path.GetSegment(-50, 999, &newPath, true, true), true);
    EXPECT_EQ(path.GetSegment(120, 120, &newPath, false, true), true);
    EXPECT_EQ(path.GetSegment(130, 120, &newPath, false, true), false);
    EXPECT_EQ(path.GetSegment(130, 120, nullptr, false, true), false);
}

/**
 * @tc.name: CopyConstruction001
 * @tc.desc: Bounds should be same by using copy construction
 * @tc.type: FUNC
 * @tc.require: issuelI6M9U9
 */
HWTEST_F(PathTest, CopyConstruction001, TestSize.Level1)
{
    Path path1;
    path1.MoveTo(1.0f, 2.0f);
    path1.LineTo(3.0f, 4.0f);
    Path path2 = path1;
    ASSERT_TRUE(path1.GetBounds() == path2.GetBounds());
}

/**
 * @tc.name: CopyConstruction002
 * @tc.desc: Deep clone by the copy construction should not modify the original object
 * @tc.type: FUNC
 * @tc.require: issuelI6M9U9
 */
HWTEST_F(PathTest, CopyConstruction002, TestSize.Level1)
{
    Path path1;
    path1.MoveTo(1.0f, 2.0f);
    path1.LineTo(3.0f, 4.0f);
    Path path2 = path1;
    path2.LineTo(10.0f, 10.0f);
    ASSERT_TRUE(path1.GetBounds() != path2.GetBounds());
}

/**
 * @tc.name: Assignment001
 * @tc.desc: Bounds should be same by using assignment method
 * @tc.type: FUNC
 * @tc.require: issuelI6M9U9
 */
HWTEST_F(PathTest, Assignment001, TestSize.Level1)
{
    Path path1;
    path1.MoveTo(1.0f, 2.0f);
    path1.LineTo(3.0f, 4.0f);
    Path path2;
    path2 = path1;
    ASSERT_TRUE(path1.GetBounds() == path2.GetBounds());
}

/**
 * @tc.name: Assignment002
 * @tc.desc: Deep clone by the assignment method should not modify the original object
 * @tc.type: FUNC
 * @tc.require: issuelI6M9U9
 */
HWTEST_F(PathTest, Assignment002, TestSize.Level1)
{
    Path path1;
    path1.MoveTo(1.0f, 2.0f);
    path1.LineTo(3.0f, 4.0f);
    Path path2;
    path2 = path1;
    path2.LineTo(10.0f, 10.0f);
    ASSERT_TRUE(path1.GetBounds() != path2.GetBounds());
}

/**
 * @tc.name: Dump001
 * @tc.desc: Dump Path
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PathTest, Dump001, TestSize.Level1)
{
    Path path;
    path.MoveTo(1.0f, 2.0f);
    path.LineTo(3.0f, 4.0f);
    std::string out;
    EXPECT_TRUE(out.empty());
    path.Dump(out);
    EXPECT_FALSE(out.empty());
}

/**
 * @tc.name: ReWind001
 * @tc.desc: ReWind Path
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PathTest, ReWind001, TestSize.Level1)
{
    Path path;
    path.MoveTo(1.0f, 2.0f);
    path.LineTo(3.0f, 4.0f);
    path.ReWind();
    EXPECT_TRUE(path.IsEmpty());
}

/**
 * @tc.name: SetLastPoint001
 * @tc.desc: ReWind Path
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PathTest, SetLastPoint001, TestSize.Level1)
{
    Path path;
    path.MoveTo(1.0f, 2.0f);
    path.LineTo(3.0f, 4.0f);
    path.SetLastPoint(5.0f, 6.0f);
    EXPECT_FALSE(path.IsEmpty());
}

/**
 * @tc.name: Serialize001
 * @tc.desc: Data returned shouldn't be nullptr
 * @tc.type: FUNC
 * @tc.require: issuelI6M9U9
 */
HWTEST_F(PathTest, Serialize001, TestSize.Level1)
{
    Path path1;
    path1.MoveTo(1.0f, 2.0f);
    path1.LineTo(3.0f, 4.0f);
    auto data1 = path1.Serialize();
    ASSERT_TRUE(data1 != nullptr);
    Path path2;
    auto data2 = path2.Serialize();
    ASSERT_TRUE(data2 != nullptr);
}

/**
 * @tc.name: SetPath001
 * @tc.desc: Set path to another.
 * @tc.type: FUNC
 * @tc.require: issuelI6M9U9
 */
HWTEST_F(PathTest, SetPath001, TestSize.Level1)
{
    Path path1;
    path1.MoveTo(1.0f, 2.0f);
    path1.LineTo(3.0f, 4.0f);
    Path path2;
    ASSERT_TRUE(path2.IsEmpty());
    path2.SetPath(path1);
    ASSERT_FALSE(path2.IsEmpty());
}

/**
 * @tc.name: IsEmpty001
 * @tc.desc: return if path is empty.
 * @tc.type: FUNC
 * @tc.require: issuelI6M9U9
 */
HWTEST_F(PathTest, IsEmpty001, TestSize.Level1)
{
    Path path1;
    ASSERT_TRUE(path1.IsEmpty());
    path1.MoveTo(1.0f, 2.0f);
    path1.LineTo(3.0f, 4.0f);
    ASSERT_FALSE(path1.IsEmpty());
    Path path2;
    path2.AddRect(1.0f, 4.0f, 3.0f, 2.0f);
    ASSERT_FALSE(path2.IsEmpty());
}


/**
 * @tc.name: IsRect001
 * @tc.desc: return if path is empty.
 * @tc.type: FUNC
 * @tc.require: issuelI6M9U9
 */
HWTEST_F(PathTest, IsRect001, TestSize.Level1)
{
    Path path1;
    Rect rect;
    bool isClosed = false;
    PathDirection dir;
    path1.MoveTo(1.0f, 2.0f);
    path1.LineTo(2.0f, 2.0f);
    path1.LineTo(2.0f, 3.0f);
    path1.LineTo(1.0f, 3.0f);
    path1.LineTo(1.0f, 2.0f);
    ASSERT_TRUE(path1.IsRect(&rect));
    Path path2;
    path2.MoveTo(1.0f, 2.0f);
    path2.LineTo(3.0f, 3.0f);
    ASSERT_FALSE(path2.IsRect(&rect));
    ASSERT_FALSE(path2.IsRect(&rect, &isClosed));
    ASSERT_FALSE(path2.IsRect(&rect, &isClosed));
    ASSERT_FALSE(path2.IsRect(&rect, &isClosed, nullptr));
    ASSERT_FALSE(path2.IsRect(&rect, &isClosed, &dir));
}

/**
 * @tc.name: CountVerbs001
 * @tc.desc: Test for checking the count of verbs in the path.
 * @tc.type: FUNC
 * @tc.require: ICAWXU
 */
HWTEST_F(PathTest, CountVerbs001, TestSize.Level1)
{
    Path path1;
    path1.MoveTo(1.0f, 2.0f);         // Start point of the path.
    path1.LineTo(3.0f, 4.0f);         // End point of the path.
    path1.LineTo(5.0f, 6.0f);         // End point of the path.
    path1.LineTo(7.0f, 8.0f);         // End point of the path.
    ASSERT_EQ(path1.CountVerbs(), 4); // There are 4 verbs in the path.
}

/**
 * @tc.name: GetPoint001
 * @tc.desc: Test for getting the point of the path.
 * @tc.type: FUNC
 * @tc.require: ICAWXU
 */
HWTEST_F(PathTest, GetPoint001, TestSize.Level1)
{
    Path path1;
    path1.MoveTo(1.0f, 2.0f); // Start point of the path.
    path1.LineTo(3.0f, 4.0f); // End point of the path.
    Point point1 = path1.GetPoint(0);
    Point point2 = path1.GetPoint(1);
    Point point3 = path1.GetPoint(-1);
    ASSERT_EQ(point1, Point(1.0f, 2.0f)); // Start point of the path.
    ASSERT_EQ(point2, Point(3.0f, 4.0f)); // End point of the path.
    ASSERT_EQ(point3, Point(0.0f, 0.0f));
}

/**
 * @tc.name: IsInterpolate001
 * @tc.desc: Test for checking the interpolation of the Path.
 * @tc.type: FUNC
 * @tc.require: ICAWXU
 */
HWTEST_F(PathTest, IsInterpolate001, TestSize.Level1)
{
    Path path1;
    path1.MoveTo(1.0f, 2.0f); // Start point of the path.
    path1.LineTo(3.0f, 4.0f); // End point of the path.
    Path path2;
    path2.MoveTo(5.0f, 6.0f); // Start point of the path.
    path2.LineTo(7.0f, 8.0f); // End point of the path.
    ASSERT_TRUE(path1.IsInterpolate(path2));
}

/**
 * @tc.name: ApproximateTest001
 * @tc.desc: test for Approximate func.
 * @tc.type: FUNC
 * @tc.require: ICAWXU
 */
HWTEST_F(PathTest, ApproximateTest001, TestSize.Level1)
{
    auto path = std::make_unique<Path>();
    std::vector<scalar> approximatedPoints;
    path->Approximate(-1.0f, approximatedPoints); // -1.0f is the tolerance value.
    ASSERT_EQ(approximatedPoints.size(), 0);

    path->Reset();
    approximatedPoints.clear();
    path->MoveTo(10.0f, 10.0f);                  // 10.0f, 10.0f is the end point.
    path->Approximate(0.5f, approximatedPoints); // 0.5f is the tolerance value.
    ASSERT_EQ(approximatedPoints.size(), 6);     // 6 for the test.

    path->Reset();
    approximatedPoints.clear();
    path->MoveTo(0.0f, 0.0f);
    path->LineTo(5.0f, 5.0f);                   // 5.0f, 5.0f is the end point.
    path->QuadTo(10.0f, 50.0f, 100.0f, 100.0f); // 10.0f, 50.0f is the control point, 100.0f, 100.0f is the end point.
    // 150.0f, 200.0f is the control point, 250.0f, 350.0f is the end point, 0.5f is the weight.
    path->ConicTo(150.0f, 200.0f, 250.0f, 350.0f, 0.5f);
    // 350.0f, 450.0f, 450.0f, 550.0f is the control point, 550.0f, 650.0f is the end point.
    path->CubicTo(350.0f, 450.0f, 450.0f, 550.0f, 550.0f, 650.0f);
    path->Close();
    path->Approximate(-1.0f, approximatedPoints); // 1.0f is the tolerance value.
    ASSERT_EQ(approximatedPoints.size(), 0);
    approximatedPoints.clear();
    path->Approximate(0.5f, approximatedPoints); // 0.5f is the tolerance value.
    uint32_t count = approximatedPoints.size();
    ASSERT_EQ(count % 3, 0);                  // The count of the approximated points should be a multiple of 3.
    for (uint32_t i = 0; i < count; i += 3) { // Check each point.
        scalar fraction = approximatedPoints[i];
        ASSERT_GE(fraction, 0.0f);
        ASSERT_LE(fraction, 1.0f);
    }
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
