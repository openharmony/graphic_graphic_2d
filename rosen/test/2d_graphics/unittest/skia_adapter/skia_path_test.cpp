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

#include <cstddef>
#include "gtest/gtest.h"
#include "draw/path.h"
#include "skia_adapter/skia_path.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaPathTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SkiaPathTest::SetUpTestCase() {}
void SkiaPathTest::TearDownTestCase() {}
void SkiaPathTest::SetUp() {}
void SkiaPathTest::TearDown() {}

/**
 * @tc.name: SkiaPath001
 * @tc.desc: Test SkiaPath's funstions
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaPathTest, SkiaPath001, TestSize.Level1)
{
    SkiaPath skiaPath;
    skiaPath.MoveTo(0, 0);
    skiaPath.LineTo(100, 100); // 100: x, y
    skiaPath.LineTo(100, 0); // 100: x
    skiaPath.Close();
    skiaPath.Reset();
    skiaPath.ArcTo(0, 0, 100, 100, 90, 90); // 100: pt2X and pt2Y, 90: startAngle and sweepAngle
    skiaPath.ArcTo(0, 0, 90, PathDirection::CW_DIRECTION, 90, 90); // 90: angle, 90: endX and endY
    skiaPath.ArcTo(0, 0, 90, 90, 10); // 90: x2 and y2, 10: radius
    skiaPath.CubicTo(0, 0, 100, 100, 100, 200); // 100: angle, direction and endX, 200: endY
    skiaPath.QuadTo(0, 0, 100, 100); // 100: endX and endY
    skiaPath.RMoveTo(100, 100); // 100: dx and dy
    skiaPath.RLineTo(200, 200); // 200: dx and dy
    skiaPath.RCubicTo(0, 0, 100, 100, 100, 200); // 200: dx and dy
    skiaPath.RQuadTo(0, 0, 100, 100); // 100: dx2 and dy2
    skiaPath.AddOval(0, 0, 100, 100, PathDirection::CW_DIRECTION); // 100: right, bottom
    skiaPath.AddArc(0, 0, 100, 100, 90, 90); // 100: right, bottom, 90: startAngle, sweepAngle
    skiaPath.AddCircle(0, 0, 100, PathDirection::CW_DIRECTION); // 100: radius
    skiaPath.AddRoundRect(0, 0,
        100, 100, 100, 100, PathDirection::CW_DIRECTION); // 100: right, bottom, xRadius, yRadius
    Path path;
    skiaPath.AddPath(path, 200, 200, PathAddMode::APPEND_PATH_ADD_MODE); // 200: dx and dy
    skiaPath.AddPath(path, PathAddMode::APPEND_PATH_ADD_MODE);
    skiaPath.ReverseAddPath(path);
    Matrix matrix;
    skiaPath.AddPath(path, matrix, PathAddMode::APPEND_PATH_ADD_MODE);
    skiaPath.SetFillStyle(PathFillType::WINDING);
    Path path2;
    skiaPath.Interpolate(path, 2, path2); // 2: weight
    skiaPath.Transform(matrix);
    std::string svgString = skiaPath.ConvertToSVGString();
    SkiaPath skiaPath2;
    skiaPath2.InitWithSVGString(svgString);
    EXPECT_TRUE(skiaPath2.GetLength(true) >= 0);
    EXPECT_TRUE(skiaPath2.Contains(0, 0));
    EXPECT_TRUE(!skiaPath2.Deserialize(nullptr));
}

/**
 * @tc.name: SkiaPathGetPositionAndTangent002
 * @tc.desc: Test GetPositionAndTangent
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaPathTest, SkiaPathGetPositionAndTangent002, TestSize.Level1)
{
    Point position;
    Point tangent;
    bool ret = false;
    SkiaPath skiaPathTmp; // test no path
    ret = skiaPathTmp.GetPositionAndTangent(10, position, tangent, true); // 10: distance
    EXPECT_FALSE(ret);

    SkiaPath skiaPath; // test path add oval
    skiaPath.AddOval(0, 0, 100, 100, PathDirection::CW_DIRECTION); // 100: right, bottom
    ret = skiaPath.GetPositionAndTangent(0, position, tangent, true);
    EXPECT_TRUE(ret);
    ret = skiaPath.GetPositionAndTangent(std::nanf(""), position, tangent, true);
    EXPECT_FALSE(ret);
    ret = skiaPath.GetPositionAndTangent(10, position, tangent, true); // 10: distance
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: SkiaPathIsClosed003
 * @tc.desc: Test IsClosed
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaPathTest, SkiaPathIsClosed003, TestSize.Level1)
{
    SkiaPath skiaPath;
    bool ret = false;
    skiaPath.MoveTo(0, 0);
    skiaPath.LineTo(100, 100); // 100: x, y
    ret = skiaPath.IsClosed(false);
    EXPECT_FALSE(ret);
    ret = skiaPath.IsClosed(true);
    EXPECT_TRUE(ret);

    SkiaPath skiaPathOval;
    skiaPathOval.AddOval(0, 0, 100, 100, PathDirection::CW_DIRECTION); // 100: right, bottom
    ret = skiaPathOval.IsClosed(false);
    EXPECT_TRUE(ret);
    ret = skiaPathOval.IsClosed(true);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: SkiaPathGetMatrix004
 * @tc.desc: Test GetMatrix
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaPathTest, SkiaPathGetMatrix004, TestSize.Level1)
{
    SkiaPath skiaPath;
    Matrix matrix;
    bool ret = false;
    ret = skiaPath.GetMatrix(false, 0, &matrix, PathMeasureMatrixFlags::GET_POSITION_MATRIX);
    EXPECT_FALSE(ret);
    ret = skiaPath.GetMatrix(false, -10, &matrix, PathMeasureMatrixFlags::GET_POSITION_MATRIX); // -10: distance
    EXPECT_FALSE(ret);
    ret = skiaPath.GetMatrix(false, 10, nullptr, PathMeasureMatrixFlags::GET_POSITION_MATRIX); // 10: distance
    EXPECT_FALSE(ret);
    skiaPath.AddOval(0, 0, 100, 100, PathDirection::CW_DIRECTION); // 100: right, bottom
    ret = skiaPath.GetMatrix(true, 10, &matrix, PathMeasureMatrixFlags::GET_POSITION_MATRIX); // 10: distance
    EXPECT_TRUE(ret);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS