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
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
