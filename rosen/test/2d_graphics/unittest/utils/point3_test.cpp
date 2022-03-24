/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"
#include "utils/point3.h"
#include "utils/scalar.h"
 
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class Point3Test : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void Point3Test::SetUpTestCase() {}
void Point3Test::TearDownTestCase() {}
void Point3Test::SetUp() {}
void Point3Test::TearDown() {}

/**
 * @tc.name: Point3CreateAndDestroy001
 * @tc.desc: Point3 Point3 Function SetandGet XYZ Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, Point3CreateAndDestroy001, TestSize.Level1)
{
    std::unique_ptr<Point3> point3 = std::make_unique<Point3>();
    EXPECT_EQ(0.0f, point3->GetX());
    EXPECT_EQ(0.0f, point3->GetY());
    EXPECT_EQ(0.0f, point3->GetZ());
}

/**
 * @tc.name: Point3CreateAndDestroy002
 * @tc.desc: Point3 Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, Point3CreateAndDestroy002, TestSize.Level1)
{
    Point3 point1;
    point1.SetX(2.0f);
    Point3 point2(point1);
    EXPECT_TRUE(point1 == point2);
}

/**
 * @tc.name: Point3CreateAndDestroy003
 * @tc.desc: Point3 Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, Point3CreateAndDestroy003, TestSize.Level1)
{
    Point3 point1;
    point1.SetX(3.0f);
    Point3 point2(point1);
    EXPECT_TRUE(point1 == point2);
}

/**
 * @tc.name: Point3CreateAndDestroy004
 * @tc.desc: Point3 Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, Point3CreateAndDestroy004, TestSize.Level1)
{
    std::unique_ptr<Point3> point3 = std::make_unique<Point3>(1.0f, 2.0f, 3.0f);
    EXPECT_EQ(1.0f, point3->GetX());
    EXPECT_EQ(2.0f, point3->GetY());
    EXPECT_EQ(3.0f, point3->GetZ());
}

/**
 * @tc.name: Point3CreateAndDestroy005
 * @tc.desc: Point3 Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, Point3CreateAndDestroy005, TestSize.Level1)
{
    std::unique_ptr<Point3> point3 = std::make_unique<Point3>(4.0f, 5.0f, 6.0f);
    EXPECT_EQ(4.0f, point3->GetX());
    EXPECT_EQ(5.0f, point3->GetY());
    EXPECT_EQ(6.0f, point3->GetZ());
}

/**
 * @tc.name: Point3SetAndGetXTest001
 * @tc.desc: Point3 Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, Point3SetAndGetXTest001, TestSize.Level1)
{
    std::unique_ptr<Point3> point3 = std::make_unique<Point3>();
    point3->SetX(1.0f);
    EXPECT_EQ(1.0f, point3->GetX());
}

/**
 * @tc.name: Point3SetAndGetXTest002
 * @tc.desc: Point3 Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, Point3SetAndGetXTest002, TestSize.Level1)
{
    std::unique_ptr<Point3> point3 = std::make_unique<Point3>();
    point3->SetX(2.0f);
    EXPECT_EQ(2.0f, point3->GetX());
}

/**
 * @tc.name: Point3SetAndGetYTest001
 * @tc.desc: Point3 Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, Point3SetAndGetYTest001, TestSize.Level1)
{
    std::unique_ptr<Point3> point3 = std::make_unique<Point3>();
    point3->SetY(1.0f);
    EXPECT_EQ(1.0f, point3->GetY());
}

/**
 * @tc.name: Point3SetAndGetYTest002
 * @tc.desc: Point3 Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, Point3SetAndGetYTest002, TestSize.Level1)
{
    std::unique_ptr<Point3> point3 = std::make_unique<Point3>();
    point3->SetY(2.0f);
    EXPECT_EQ(2.0f, point3->GetY());
}

/**
 * @tc.name: Point3SetAndGetZTest001
 * @tc.desc: Point3 Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, Point3SetAndGetZTest001, TestSize.Level1)
{
    std::unique_ptr<Point3> point3 = std::make_unique<Point3>();
    point3->SetZ(1.0f);
    EXPECT_EQ(1.0f, point3->GetZ());
}

/**
 * @tc.name: Point3SetAndGetZTest002
 * @tc.desc: Point3 Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, Point3SetAndGetZTest002, TestSize.Level1)
{
    std::unique_ptr<Point3> point3 = std::make_unique<Point3>();
    point3->SetZ(2.0f);
    EXPECT_EQ(2.0f, point3->GetZ());
}

/**
 * @tc.name: Point3AddEqualTest001
 * @tc.desc: Point3 Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, Point3AddEqualTest001, TestSize.Level1)
{
    Point3 point1;
    Point3 point2(1.0f, 2.0f, 3.0f);
    point1 += point2;
    EXPECT_EQ(1.0f, point1.GetX());
    EXPECT_EQ(2.0f, point1.GetY());
    EXPECT_EQ(3.0f, point1.GetZ());
}

/**
 * @tc.name: Point3AddEqualTest002
 * @tc.desc: Point3 Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, Point3AddEqualTest002, TestSize.Level1)
{
    Point3 point1;
    Point3 point2(3.0f, 2.0f, 1.0f);
    point1 += point2;
    EXPECT_EQ(3.0f, point1.GetX());
    EXPECT_EQ(2.0f, point1.GetY());
    EXPECT_EQ(1.0f, point1.GetZ());
}

/**
 * @tc.name: Point3MinusEqualTest001
 * @tc.desc: Point3 Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, Point3MinusEqualTest001, TestSize.Level1)
{
    Point3 point1;
    Point3 point2(1.0f, 2.0f, 3.0f);
    point1 -= point2;
    EXPECT_EQ(-1.0f, point1.GetX());
    EXPECT_EQ(-2.0f, point1.GetY());
    EXPECT_EQ(-3.0f, point1.GetZ());
}

/**
 * @tc.name: Point3MinusEqualTest002
 * @tc.desc: Point3 Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, Point3MinusEqualTest002, TestSize.Level1)
{
    Point3 point1;
    Point3 point2(3.0f, 2.0f, 1.0f);
    point1 -= point2;
    EXPECT_EQ(-3.0f, point1.GetX());
    EXPECT_EQ(-2.0f, point1.GetY());
    EXPECT_EQ(-1.0f, point1.GetZ());
}

/**
 * @tc.name: Point3MultiplyEqualTest001
 * @tc.desc: Point3 Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, Point3MultiplyEqualTest001, TestSize.Level1)
{
    Point3 point3(1.0f, 2.0f, 3.0f);
    point3 *= 2;
    EXPECT_EQ(2.0f, point3.GetX());
    EXPECT_EQ(4.0f, point3.GetY());
    EXPECT_EQ(6.0f, point3.GetZ());
}

/**
 * @tc.name: Point3MultiplyEqualTest002
 * @tc.desc: Point3 Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, Point3MultiplyEqualTest002, TestSize.Level1)
{
    Point3 point3(3.0f, 2.0f, 1.0f);
    point3 *= 2;
    EXPECT_EQ(6.0f, point3.GetX());
    EXPECT_EQ(4.0f, point3.GetY());
    EXPECT_EQ(2.0f, point3.GetZ());
}

/**
 * @tc.name: Point3DivideEqualTest001
 * @tc.desc: Point3 Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, Point3DivideEqualTest001, TestSize.Level1)
{
    Point3 point3(2.0f, 4.0f, 6.0f);
    point3 /= 2.0f;
    EXPECT_EQ(1.0f, point3.GetX());
    EXPECT_EQ(2.0f, point3.GetY());
    EXPECT_EQ(3.0f, point3.GetZ());
}

/**
 * @tc.name: Point3DivideEqualTest002
 * @tc.desc: Point3 Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, Point3DivideEqualTest002, TestSize.Level1)
{
    Point3 point3(4.0f, 8.0f, 10.0f);
    point3 /= 2.0f;
    EXPECT_EQ(2.0f, point3.GetX());
    EXPECT_EQ(4.0f, point3.GetY());
    EXPECT_EQ(5.0f, point3.GetZ());
}

/**
 * @tc.name: Point3AddTest001
 * @tc.desc: Point3 Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, Point3AddTest001, TestSize.Level1)
{
    Point3 point1(1.0f, 2.0f, 3.0f);
    Point3 point2(0.0f, 0.0f, 0.0f);
    Point3 point3 = point1 + point2;
    EXPECT_EQ(3.0f, point3.GetX());
    EXPECT_EQ(0.0f, point3.GetY());
    EXPECT_EQ(3.0f, point3.GetZ());
}

/**
 * @tc.name: Point3AddTest002
 * @tc.desc: Point3 Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, Point3AddTest002, TestSize.Level1)
{
    Point3 point1(3.0f, 2.0f, 1.0f);
    Point3 point2(0.0f, 0.0f, 0.0f);
    Point3 point3 = point1 + point2;
    EXPECT_EQ(5.0f, point3.GetX());
    EXPECT_EQ(0.0f, point3.GetY());
    EXPECT_EQ(1.0f, point3.GetZ());
}

/**
 * @tc.name: Point3MinusTest001
 * @tc.desc: Point3 Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, Point3MinusTest001, TestSize.Level1)
{
    Point3 point1(1.0f, 2.0f, 3.0f);
    Point3 point2;
    Point3 point3 = point1 - point2;
    EXPECT_EQ(1.0f, point3.GetX());
    EXPECT_EQ(2.0f, point3.GetY());
    EXPECT_EQ(3.0f, point3.GetZ());
}

/**
 * @tc.name: Point3MinusTest002
 * @tc.desc: Point3 Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, Point3MinusTest002, TestSize.Level1)
{
    Point3 point1(3.0f, 2.0f, 1.0f);
    Point3 point2;
    Point3 point3 = point1 - point2;
    EXPECT_EQ(3.0f, point3.GetX());
    EXPECT_EQ(2.0f, point3.GetY());
    EXPECT_EQ(1.0f, point3.GetZ());
}

/**
 * @tc.name: Point3MultiplyTest001
 * @tc.desc: Point3 Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, Point3MultiplyTest001, TestSize.Level1)
{
    Point3 point1(1.0f, 2.0f, 3.0f);
    Point3 point2 = 2.0f * point1;
    EXPECT_EQ(2.0f, point2.GetX());
    EXPECT_EQ(4.0f, point2.GetY());
    EXPECT_EQ(6.0f, point2.GetZ());
}

/**
 * @tc.name: Point3MultiplyTest002
 * @tc.desc: Point3 Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, Point3MultiplyTest002, TestSize.Level1)
{
    Point3 point1(1.0f, 2.0f, 3.0f);
    Point3 point2 = 3.0f * point1;
    EXPECT_EQ(3.0f, point2.GetX());
    EXPECT_EQ(6.0f, point2.GetY());
    EXPECT_EQ(9.0f, point2.GetZ());
}

/**
 * @tc.name: Point3MultiplyTest003
 * @tc.desc: Point3 Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, Point3MultiplyTest003, TestSize.Level1)
{
    Point3 point1(1.0f, 2.0f, 3.0f);
    Point3 point2 = point1 * 2.0f;
    EXPECT_EQ(2.0f, point2.GetX());
    EXPECT_EQ(4.0f, point2.GetY());
    EXPECT_EQ(6.0f, point2.GetZ());
}

/**
 * @tc.name: Point3MultiplyTest004
 * @tc.desc: Point3 Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, Point3MultiplyTest004, TestSize.Level1)
{
    Point3 point1(1.0f, 2.0f, 3.0f);
    Point3 point2 = point1 * 3.0f;
    EXPECT_EQ(3.0f, point2.GetX());
    EXPECT_EQ(6.0f, point2.GetY());
    EXPECT_EQ(9.0f, point2.GetZ());
}

/**
 * @tc.name: Point3DivideTest001
 * @tc.desc: Point3 Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, Point3DivideTest001, TestSize.Level1)
{
    Point3 point1(2.0f, 4.0f, 6.0f);
    Point3 point2 = point1 / 2.0f;
    EXPECT_EQ(1.0f, point2.GetX());
    EXPECT_EQ(2.0f, point2.GetY());
    EXPECT_EQ(3.0f, point2.GetZ());
}

/**
 * @tc.name: Point3DivideTest002
 * @tc.desc: Point3 Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, Point3DivideTest002, TestSize.Level1)
{
    Point3 point1(3.0f, 6.0f, 9.0f);
    Point3 point2 = point1 / 3.0f;
    EXPECT_EQ(1.0f, point2.GetX());
    EXPECT_EQ(2.0f, point2.GetY());
    EXPECT_EQ(3.0f, point2.GetZ());
}

/**
 * @tc.name: Point3AddTest003
 * @tc.desc: Point3 Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, Point3AddTest003, TestSize.Level1)
{
    Point3 point1(1.0f, 2.0f, 3.0f);
    Point3 point2 = +point1;
    EXPECT_EQ(1.0f, point2.GetX());
    EXPECT_EQ(2.0f, point2.GetY());
    EXPECT_EQ(3.0f, point2.GetZ());
}

/**
 * @tc.name: Point3AddTest004
 * @tc.desc: Point3 Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, Point3AddTest005, TestSize.Level1)
{
    Point3 point1(3.0f, 2.0f, 1.0f);
    Point3 point2 = +point1;
    EXPECT_EQ(3.0f, point2.GetX());
    EXPECT_EQ(2.0f, point2.GetY());
    EXPECT_EQ(1.0f, point2.GetZ());
}

/**
 * @tc.name: Point3MinusTest003
 * @tc.desc: Point3 Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, Point3MinusTest003, TestSize.Level1)
{
    Point3 point1(1.0f, 2.0f, 3.0f);
    Point3 point2 = -point1;
    EXPECT_EQ(-1.0f, point2.GetX());
    EXPECT_EQ(-2.0f, point2.GetY());
    EXPECT_EQ(-3.0f, point2.GetZ());
}

/**
 * @tc.name: Point3MinusTest004
 * @tc.desc: Point3 Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, Point3MinusTest004, TestSize.Level1)
{
    Point3 point1(3.0f, 2.0f, 1.0f);
    Point3 point2 = -point1;
    EXPECT_EQ(-3.0f, point2.GetX());
    EXPECT_EQ(-2.0f, point2.GetY());
    EXPECT_EQ(-1.0f, point2.GetZ());
}

/**
 * @tc.name: Point3EqualTest001
 * @tc.desc: Point3 Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, Point3EqualTest001, TestSize.Level1)
{
    Point3 point1;
    Point3 point2;
    EXPECT_TRUE(point1 == point2);
}

/**
 * @tc.name: Point3EqualTest002
 * @tc.desc: Point3 Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, Point3EqualTest002, TestSize.Level1)
{
    Point3 point1(1.0f, 2.0f, 3.0f);
    Point3 point2;
    EXPECT_FALSE(point1 == point2);
}

/**
 * @tc.name: Point3NotEqualTest001
 * @tc.desc: Point3 Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, Point3NotEqualTest001, TestSize.Level1)
{
    Point3 point1(1.0f, 2.0f, 3.0f);
    Point3 point2;
    EXPECT_TRUE(point1 != point2);
}

/**
 * @tc.name: Point3NotEqualTest002
 * @tc.desc: Point3 Point3 Function Test
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(Point3Test, Point3NotEqualTest002, TestSize.Level1)
{
    Point3 point1;
    Point3 point2;
    EXPECT_FALSE(point1 != point2);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS