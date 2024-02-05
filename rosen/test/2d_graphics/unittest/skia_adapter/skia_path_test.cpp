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
    skiaPath.AddPath(path, 200, 200); // 200: dx and dy
    skiaPath.AddPath(path);
    skiaPath.ReverseAddPath(path);
    Matrix matrix;
    skiaPath.AddPathWithMatrix(path, matrix);
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
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS