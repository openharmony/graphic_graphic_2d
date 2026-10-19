/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
 
#include "common/rs_rect_shape.h"
#include "common/rs_vector4.h"
 
using namespace testing;
using namespace testing::ext;
 
namespace OHOS::Rosen {
namespace {
constexpr float WIDTH_A = 10.0f;
constexpr float HEIGHT_A = 20.0f;
constexpr float WIDTH_B = 5.0f;
constexpr float HEIGHT_B = 15.0f;
constexpr float SCALE_FACTOR = 2.0f;
constexpr float CORNER_LT = 1.0f;
constexpr float CORNER_RT = 2.0f;
constexpr float CORNER_RB = 3.0f;
constexpr float CORNER_LB = 4.0f;
} // namespace
 
class RectShapeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};
 
void RectShapeTest::SetUpTestCase() {}
void RectShapeTest::TearDownTestCase() {}
void RectShapeTest::SetUp() {}
void RectShapeTest::TearDown() {}
 
/**
 * @tc.name: DefaultConstruction001
 * @tc.desc: test results of RectShape default construction
 * @tc.type: FUNC
 * @tc.require: SR20260808951364
 */
HWTEST_F(RectShapeTest, DefaultConstruction001, TestSize.Level1)
{
    RectShape shape;
    EXPECT_EQ(shape.GetWidth(), 0.0f);
    EXPECT_EQ(shape.GetHeight(), 0.0f);
    EXPECT_EQ(shape.GetCornerRadius(), Vector4f(0.0f));
}
 
/**
 * @tc.name: ParameterizedConstruction001
 * @tc.desc: test results of RectShape parameterized construction
 * @tc.type: FUNC
 * @tc.require: SR20260808951364
 */
HWTEST_F(RectShapeTest, ParameterizedConstruction001, TestSize.Level1)
{
    Vector4f radius(CORNER_LT, CORNER_RT, CORNER_RB, CORNER_LB); // for test
    RectShape shape(WIDTH_A, HEIGHT_A, radius);
    EXPECT_EQ(shape.GetWidth(), WIDTH_A);
    EXPECT_EQ(shape.GetHeight(), HEIGHT_A);
    EXPECT_EQ(shape.GetCornerRadius(), radius);
}
 
/**
 * @tc.name: OperatorPlus001
 * @tc.desc: test results of RectShape operator+
 * @tc.type: FUNC
 * @tc.require: SR20260808951364
 */
HWTEST_F(RectShapeTest, OperatorPlus001, TestSize.Level1)
{
    Vector4f radiusA(CORNER_LT, CORNER_RT, CORNER_RB, CORNER_LB); // for test
    Vector4f radiusB(CORNER_LB, CORNER_RB, CORNER_RT, CORNER_LT); // for test
    RectShape shapeA(WIDTH_A, HEIGHT_A, radiusA);
    RectShape shapeB(WIDTH_B, HEIGHT_B, radiusB);
    RectShape result = shapeA + shapeB;
    EXPECT_EQ(result.GetWidth(), WIDTH_A + WIDTH_B);
    EXPECT_EQ(result.GetHeight(), HEIGHT_A + HEIGHT_B);
    EXPECT_EQ(result.GetCornerRadius(), radiusA + radiusB);
}
 
/**
 * @tc.name: OperatorMinus001
 * @tc.desc: test results of RectShape operator-
 * @tc.type: FUNC
 * @tc.require: SR20260808951364
 */
HWTEST_F(RectShapeTest, OperatorMinus001, TestSize.Level1)
{
    Vector4f radiusA(CORNER_LT, CORNER_RT, CORNER_RB, CORNER_LB); // for test
    Vector4f radiusB(CORNER_LB, CORNER_RB, CORNER_RT, CORNER_LT); // for test
    RectShape shapeA(WIDTH_A, HEIGHT_A, radiusA);
    RectShape shapeB(WIDTH_B, HEIGHT_B, radiusB);
    RectShape result = shapeA - shapeB;
    EXPECT_EQ(result.GetWidth(), WIDTH_A - WIDTH_B);
    EXPECT_EQ(result.GetHeight(), HEIGHT_A - HEIGHT_B);
    EXPECT_EQ(result.GetCornerRadius(), radiusA - radiusB);
}
 
/**
 * @tc.name: OperatorMultiply001
 * @tc.desc: test results of RectShape operator* with float scale
 * @tc.type: FUNC
 * @tc.require: SR20260808951364
 */
HWTEST_F(RectShapeTest, OperatorMultiply001, TestSize.Level1)
{
    Vector4f radius(CORNER_LT, CORNER_RT, CORNER_RB, CORNER_LB); // for test
    RectShape shape(WIDTH_A, HEIGHT_A, radius);
    RectShape result = shape * SCALE_FACTOR;
    EXPECT_EQ(result.GetWidth(), WIDTH_A * SCALE_FACTOR);
    EXPECT_EQ(result.GetHeight(), HEIGHT_A * SCALE_FACTOR);
    EXPECT_EQ(result.GetCornerRadius(), radius * SCALE_FACTOR);
}
 
/**
 * @tc.name: OperatorEqual001
 * @tc.desc: test results of RectShape operator== when all components equal
 * @tc.type: FUNC
 * @tc.require: SR20260808951364
 */
HWTEST_F(RectShapeTest, OperatorEqual001, TestSize.Level1)
{
    Vector4f radius(CORNER_LT, CORNER_RT, CORNER_RB, CORNER_LB); // for test
    RectShape shapeA(WIDTH_A, HEIGHT_A, radius);
    RectShape shapeB(WIDTH_A, HEIGHT_A, radius);
    EXPECT_TRUE(shapeA == shapeB);
}
 
/**
 * @tc.name: OperatorEqual002
 * @tc.desc: test results of RectShape operator== when components differ
 * @tc.type: FUNC
 * @tc.require: SR20260808951364
 */
HWTEST_F(RectShapeTest, OperatorEqual002, TestSize.Level1)
{
    Vector4f radius(CORNER_LT, CORNER_RT, CORNER_RB, CORNER_LB); // for test
    RectShape shapeA(WIDTH_A, HEIGHT_A, radius);
    RectShape shapeB(WIDTH_B, HEIGHT_A, radius);
    EXPECT_FALSE(shapeA == shapeB);
}
 
/**
 * @tc.name: OperatorEqual003
 * @tc.desc: test results of RectShape operator== when corner radius differs
 * @tc.type: FUNC
 * @tc.require: SR20260808951364
 */
HWTEST_F(RectShapeTest, OperatorEqual003, TestSize.Level1)
{
    Vector4f radiusA(CORNER_LT, CORNER_RT, CORNER_RB, CORNER_LB); // for test
    Vector4f radiusB(CORNER_LT, CORNER_RT, CORNER_RB, CORNER_LT); // for test
    RectShape shapeA(WIDTH_A, HEIGHT_A, radiusA);
    RectShape shapeB(WIDTH_A, HEIGHT_A, radiusB);
    EXPECT_FALSE(shapeA == shapeB);
}
} // namespace OHOS::Rosen