/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <vector>
#include "gtest/gtest.h"
#include "draw/path.h"
#include "draw/path_iterator.h"
#include "utils/point.h"
#include "utils/utils_path.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {

class UtilsPathTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void UtilsPathTest::SetUpTestCase() {}
void UtilsPathTest::TearDownTestCase() {}
void UtilsPathTest::SetUp() {}
void UtilsPathTest::TearDown() {}

/**
 * @tc.name: UtilsPathDistance001
 * @tc.desc: Test Distance
 * @tc.type: FUNC
 * @tc.require: ICAWXU
 */
HWTEST_F(UtilsPathTest, UtilsPathDistance001, TestSize.Level1)
{
    Point p1(0, 0);
    Point p2(3, 4); // 3, 4 are x, y coordinates
    scalar dist = UtilsPath::Distance(p1, p2);
    EXPECT_FLOAT_EQ(dist, 5.0f); // 5.0f is the expected Euclidean distance
}

/**
 * @tc.name: UtilsPathAddMove001
 * @tc.desc: Test AddMove
 * @tc.type: FUNC
 * @tc.require: ICAWXU
 */
HWTEST_F(UtilsPathTest, UtilsPathAddMove001, TestSize.Level1)
{
    std::vector<Point> points;
    std::vector<float> lengths;
    Point p(1, 2); // 1, 2 are x, y coordinates
    UtilsPath::AddMove(p, points, lengths);
    EXPECT_EQ(points.size(), 1);
    EXPECT_EQ(lengths.size(), 1);
    EXPECT_EQ(points[0], p);
    EXPECT_FLOAT_EQ(lengths[0], 0.0f);
}

/**
 * @tc.name: UtilsPathAddLine001
 * @tc.desc: Test AddLine
 * @tc.type: FUNC
 * @tc.require: ICAWXU
 */
HWTEST_F(UtilsPathTest, UtilsPathAddLine001, TestSize.Level1)
{
    std::vector<Point> points;
    std::vector<float> lengths;
    Point p1(0, 0);
    Point p2(2, 0); // 2, 0 are x, y coordinates
    UtilsPath::AddMove(p1, points, lengths);
    UtilsPath::AddLine(p2, points, lengths);
    EXPECT_EQ(points.size(), 2);  // Should have two points now
    EXPECT_EQ(lengths.size(), 2); // Should have two lengths now
    EXPECT_EQ(points[1], p2);
    UtilsPath::AddLine(p2, points, lengths);
    EXPECT_EQ(points.size(), 2); // Should still have two points
}

/**
 * @tc.name: UtilsPathCalculateQuadraticBezier001
 * @tc.desc: Test CalculateQuadraticBezier
 * @tc.type: FUNC
 * @tc.require: ICAWXU
 */
HWTEST_F(UtilsPathTest, UtilsPathCalculateQuadraticBezier001, TestSize.Level1)
{
    Point pts[3] = { Point(0, 0), Point(1, 2), Point(2, 0) }; // 1, 2 are x, y coordinates
    Point p0 = UtilsPath::CalculateQuadraticBezier(0.0f, pts);
    EXPECT_EQ(p0, pts[0]);
    Point p1 = UtilsPath::CalculateQuadraticBezier(1.0f, pts);
    EXPECT_EQ(p1, pts[2]);
    Point pmid = UtilsPath::CalculateQuadraticBezier(0.5f, pts); // 0.5f should be the midpoint
    EXPECT_EQ(pmid, Point(1, 1));
}

/**
 * @tc.name: UtilsPathCalculateCubicBezier001
 * @tc.desc: Test CalculateCubicBezier
 * @tc.type: FUNC
 * @tc.require: ICAWXU
 */
HWTEST_F(UtilsPathTest, UtilsPathCalculateCubicBezier001, TestSize.Level1)
{
    Point pts[4] = { Point(0, 0), Point(1, 3), Point(3, 3), Point(4, 0) }; // 1, 3, 4 are x, y coordinates
    Point p0 = UtilsPath::CalculateCubicBezier(0.0f, pts);
    EXPECT_EQ(p0, pts[0]);
    Point p1 = UtilsPath::CalculateCubicBezier(1.0f, pts);
    EXPECT_EQ(p1, pts[3]);
    Point pmid = UtilsPath::CalculateCubicBezier(0.5f, pts); // 0.5f should be the midpoint
    EXPECT_EQ(pmid, Point(2.0f, 2.25f));                       // 2.0f, 2.25f are x, y coordinates
}

/**
 * @tc.name: UtilsPathAddBezier001
 * @tc.desc: Test AddBezier
 * @tc.type: FUNC
 * @tc.require: ICAWXU
 */
HWTEST_F(UtilsPathTest, UtilsPathAddBezier001, TestSize.Level1)
{
    Point pts1[3] = { Point(0, 0), Point(1, 2), Point(2, 0) }; // 1, 2 are x, y coordinates
    std::vector<Point> approxPoints1;
    std::vector<float> approxLengths1;
    // 100.0f for the test
    UtilsPath::AddBezier(pts1, UtilsPath::CalculateQuadraticBezier, approxPoints1, approxLengths1, 100.0f, false);
    EXPECT_EQ(approxPoints1.size(), 3); // 3 for the test
    Point pts2[3] = { Point(0, 0), Point(100, 200), Point(200, 0) }; // 100, 200 are x, y coordinates
    std::vector<Point> approxPoints2;
    std::vector<float> approxLengths2;
    UtilsPath::AddBezier(pts2, UtilsPath::CalculateQuadraticBezier, approxPoints2, approxLengths2, 1.0f, false);
    EXPECT_EQ(approxPoints2.size(), 18); // 18 for the test
    Point pts3[3] = { Point(0, 0), Point(1, 2), Point(2, 0) }; // 1, 2 are x, y coordinates
    std::vector<Point> approxPoints3;
    std::vector<float> approxLengths3;
    // 100.0f for the test
    UtilsPath::AddBezier(pts3, UtilsPath::CalculateQuadraticBezier, approxPoints3, approxLengths3, 100.0f, true);
    EXPECT_EQ(approxPoints3.size(), 3); // 3 for the test
    Point pts4[3] = { Point(0, 0), Point(100, 200), Point(200, 0) }; // 100, 200 are x, y coordinates
    std::vector<Point> approxPoints4;
    std::vector<float> approxLengths4;
    // 0.5f for the test
    UtilsPath::AddBezier(pts4, UtilsPath::CalculateQuadraticBezier, approxPoints4, approxLengths4, 0.5f, true);
    EXPECT_EQ(approxPoints4.size(), 18); // 18 for the test
}

/**
 * @tc.name: UtilsPathAddConic001
 * @tc.desc: Test AddConic
 * @tc.type: FUNC
 * @tc.require: ICAWXU
 */
HWTEST_F(UtilsPathTest, UtilsPathAddConic001, TestSize.Level1)
{
    Path path;
    path.MoveTo(0, 0);
    path.ConicTo(1, 2, 3, 0, 1.0f); // 1, 2, 3 are x, y coordinates, 1.0 is weight
    PathIter iter(path, false);
    PathVerb verb;
    Point pts[4] = {};
    do {
        verb = iter.Next(pts);
    } while (verb != PathVerb::CONIC && verb != PathVerb::DONE);
    if (verb == PathVerb::CONIC) {
        std::vector<Point> approxPoints;
        std::vector<float> approxLengths;
        UtilsPath::AddConic(iter, pts, approxPoints, approxLengths, 0.5f); // 0.5 is errorConic
        EXPECT_EQ(approxPoints.size(), 2); // 2 for the test
    }
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
