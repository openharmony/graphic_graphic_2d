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

#include "gtest/gtest.h"

#include "common/rs_vector3.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class Vector3Test : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void Vector3Test::SetUpTestCase() {}
void Vector3Test::TearDownTestCase() {}
void Vector3Test::SetUp() {}
void Vector3Test::TearDown() {}

/**
 * @tc.name: Normalized001
 * @tc.desc: test results of Normalized
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector3Test, Normalized001, TestSize.Level1)
{
    Vector3 vector(1.f, 2.f, 2.f); // for test
    vector.Normalized();
    EXPECT_TRUE(vector.data_[0] == 1.f);
}

/**
 * @tc.name: Dot001
 * @tc.desc: test results of Dot
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector3Test, Dot001, TestSize.Level1)
{
    Vector3 vector(1.f, 1.f, 1.f); // for test
    Vector3 vectorNew(1.f, 2.f, 2.f); // for test
    float sum = vector.Dot(vectorNew);
    EXPECT_TRUE(sum != 1.f);
}

/**
 * @tc.name: Cross001
 * @tc.desc: test results of Cross
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector3Test, Cross001, TestSize.Level1)
{
    Vector3 vector(1.f, 1.f, 1.f); // for test
    Vector3 vectorNew(1.f, 2.f, 2.f); // for test
    Vector3 rCross = vector.Cross(vectorNew);
    EXPECT_TRUE(rCross.data_[0] == 0.f);
}

/**
 * @tc.name: GetSqrLength001
 * @tc.desc: test results of GetSqrLength
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector3Test, GetSqrLength001, TestSize.Level1)
{
    Vector3 vector(1.f, 2.f, 2.f); // for test
    float res = vector.GetSqrLength();
    EXPECT_TRUE(res == 9.f);
}

/**
 * @tc.name: GetLength001
 * @tc.desc: test results of GetLength
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector3Test, GetLength001, TestSize.Level1)
{
    Vector3 vector(1.f, 2.f, 2.f); // for test
    float res = vector.GetLength();
    EXPECT_TRUE(res == 3.f);
}

/**
 * @tc.name: SetZero001
 * @tc.desc: test results of SetZero
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector3Test, SetZero001, TestSize.Level1)
{
    Vector3 vector(1.f, 2.f, 2.f); // for test
    vector.SetZero();
    EXPECT_TRUE(vector.data_[0] == 0.f);
}

/**
 * @tc.name: SetValues001
 * @tc.desc: test results of SetValues
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector3Test, SetValues001, TestSize.Level1)
{
    Vector3 vector(1.f, 2.f, 2.f); // for test
    vector.SetValues(2.f, 0.f, 0.f);
    EXPECT_TRUE(vector.data_[0] == 2.f);
}

/**
 * @tc.name: Normalize001
 * @tc.desc: test results of Normalize
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector3Test, Normalize001, TestSize.Level1)
{
    Vector3 vector(1.f, 2.f, 2.f); // for test
    float res = vector.Normalize();
    EXPECT_TRUE(res == 3.f);
}

/**
 * @tc.name: MultiplyEqual001
 * @tc.desc: test results of operator*=
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector3Test, MultiplyEqual001, TestSize.Level1)
{
    Vector3 vector(1.f, 1.f, 1.f); // for test
    Vector3 vectorNew(1.f, 2.f, 2.f); // for test
    vector *= vectorNew;
    EXPECT_TRUE(vector.data_[1] == 2.f);
}

/**
 * @tc.name: MultiplyEqual002
 * @tc.desc: test results of operator*=
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector3Test, MultiplyEqual002, TestSize.Level1)
{
    Vector3 vector(1.f, 1.f, 1.f); // for test
    vector *= 2.f;
    EXPECT_TRUE(vector.data_[1] == 2.f);
}

/**
 * @tc.name: Multiply001
 * @tc.desc: test results of operator*
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector3Test, Multiply001, TestSize.Level1)
{
    Vector3 vector(1.f, 1.f, 1.f); // for test
    Vector3 VectorNew = vector * 2.f;
    EXPECT_TRUE(VectorNew.data_[1] == 2.f);
}

/**
 * @tc.name: Add001
 * @tc.desc: test results of operator+
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector3Test, Add001, TestSize.Level1)
{
    Vector3 vector(1.f, 1.f, 1.f); // for test
    Vector3 vectorNew(1.f, 2.f, 2.f); // for test
    Vector3 VectorRes = vector + vectorNew;
    EXPECT_TRUE(VectorRes.data_[1] == 3.f);
}

/**
 * @tc.name: AddEqual001
 * @tc.desc: test results of operator+=
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector3Test, AddEqual001, TestSize.Level1)
{
    Vector3 vector(1.f, 1.f, 1.f); // for test
    Vector3 vectorNew(1.f, 2.f, 2.f); // for test
    vector += vectorNew;
    EXPECT_TRUE(vector.data_[0] == 2.f);
}

/**
 * @tc.name: Equal001
 * @tc.desc: test results of operator=
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector3Test, Equal001, TestSize.Level1)
{
    Vector3 vector(1.f, 1.f, 1.f); // for test
    Vector3 vectorNew(1.f, 2.f, 2.f); // for test
    vector = vectorNew;
    EXPECT_TRUE(vector.data_[1] == 2.f);
}

/**
 * @tc.name: Minus001
 * @tc.desc: test results of operator-
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector3Test, Minus001, TestSize.Level1)
{
    Vector3 vector(1.f, 1.f, 1.f); // for test
    Vector3 vectorNew(1.f, 2.f, 2.f); // for test
    Vector3 vectorRes = vector - vectorNew;
    EXPECT_TRUE(vectorRes.data_[0] == 0.f);
}

/**
 * @tc.name: Brackets001
 * @tc.desc: test results of operator[]
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector3Test, Brackets001, TestSize.Level1)
{
    Vector3 vector(1.f, 1.f, 1.f); // for test
    vector.data_[1] += 1.f;
    EXPECT_TRUE(vector.data_[1] == 2.f);
}

/**
 * @tc.name: Brackets002
 * @tc.desc: test results of operator[]
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector3Test, Brackets002, TestSize.Level1)
{
    const Vector3 vector(1.f, 1.f, 1.f); // for test
    EXPECT_TRUE(vector.data_[1] == 1.f);
}

/**
 * @tc.name: Equal002
 * @tc.desc: test results of operator==
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector3Test, Equal002, TestSize.Level1)
{
    Vector3 vector(1.f, 1.f, 1.f); // for test
    Vector3 vectorNew(1.f, 1.f, 1.f); // for test
    EXPECT_TRUE(vector == vectorNew);
}

/**
 * @tc.name: GetData001
 * @tc.desc: test results of GetData
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector3Test, GetData001, TestSize.Level1)
{
    Vector3 vector(1.f, 1.f, 1.f); // for test
    vector.GetData();
    EXPECT_TRUE(vector.data_[1] != 2.f);
}

/**
 * @tc.name: IsNearEqual001
 * @tc.desc: test results of IsNearEqual
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector3Test, IsNearEqual001, TestSize.Level1)
{
    Vector3 vector(1.f, 2.f, 2.f); // 1.f, 2.f, 2.f for test
    Vector3 vectorNew(1.f, 2.f, 2.f); // 1.f, 2.f, 2.f for test
    EXPECT_TRUE(vector.IsNearEqual(vectorNew, 3.f)); // 3.f for test
}

/**
 * @tc.name: OperatorMultiplyVector3001
 * @tc.desc: test results of operator*(const Vector3&) component-wise multiplication
 * @tc.type: FUNC
 */
HWTEST_F(Vector3Test, OperatorMultiplyVector3001, TestSize.Level1)
{
    Vector3f a(2.0f, 3.0f, 4.0f);
    Vector3f b(5.0f, 6.0f, 7.0f);
    Vector3f result = a * b;
    EXPECT_FLOAT_EQ(result.x_, 10.0f);
    EXPECT_FLOAT_EQ(result.y_, 18.0f);
    EXPECT_FLOAT_EQ(result.z_, 28.0f);
}

/**
 * @tc.name: OperatorMultiplyVector3002
 * @tc.desc: test operator*(const Vector3&) with zeros and negatives
 * @tc.type: FUNC
 */
HWTEST_F(Vector3Test, OperatorMultiplyVector3002, TestSize.Level1)
{
    Vector3f zero(0.0f, 0.0f, 0.0f);
    Vector3f result = zero * zero;
    EXPECT_FLOAT_EQ(result.x_, 0.0f);
    EXPECT_FLOAT_EQ(result.y_, 0.0f);
    EXPECT_FLOAT_EQ(result.z_, 0.0f);

    Vector3f neg(-2.0f, 3.0f, -4.0f);
    Vector3f other(3.0f, -2.0f, 5.0f);
    result = neg * other;
    EXPECT_FLOAT_EQ(result.x_, -6.0f);
    EXPECT_FLOAT_EQ(result.y_, -6.0f);
    EXPECT_FLOAT_EQ(result.z_, -20.0f);
}

/**
 * @tc.name: IsAbsNearEqual001
 * @tc.desc: test results of Vector3<float>::IsAbsNearEqual
 * @tc.type: FUNC
 */
HWTEST_F(Vector3Test, IsAbsNearEqual001, TestSize.Level1)
{
    Vector3f value(10.0f, 20.0f, 30.0f);
    Vector3f target(12.0f, 18.0f, 33.0f);
    // |10-12|=2 <= 5, |20-18|=2 <= 5, |30-33|=3 <= 5 -> true
    Vector3f threshold(5.0f, 5.0f, 5.0f);
    EXPECT_TRUE(value.IsAbsNearEqual(target, threshold));

    // |10-12|=2 > 1 -> false
    Vector3f smallThreshold(1.0f, 1.0f, 1.0f);
    EXPECT_FALSE(value.IsAbsNearEqual(target, smallThreshold));

    // only z outside threshold -> false
    Vector3f mixedThreshold(5.0f, 5.0f, 1.0f);
    EXPECT_FALSE(value.IsAbsNearEqual(target, mixedThreshold));
}

/**
 * @tc.name: IsAbsNearEqual002
 * @tc.desc: test Vector3<float>::IsAbsNearEqual with exact match and negative threshold
 * @tc.type: FUNC
 */
HWTEST_F(Vector3Test, IsAbsNearEqual002, TestSize.Level1)
{
    Vector3f value(10.0f, 20.0f, 30.0f);
    // exact match with zero threshold -> true
    Vector3f zeroThreshold(0.0f, 0.0f, 0.0f);
    EXPECT_TRUE(value.IsAbsNearEqual(value, zeroThreshold));

    // negative threshold: abs is used, so -5 behaves like 5
    Vector3f target(12.0f, 18.0f, 33.0f);
    Vector3f negThreshold(-5.0f, -5.0f, -5.0f);
    EXPECT_TRUE(value.IsAbsNearEqual(target, negThreshold));
}

/**
 * @tc.name: TakeAbsMaxFrom001
 * @tc.desc: test results of Vector3<float>::TakeAbsMaxFrom with mixed values
 * @tc.type: FUNC
 */
HWTEST_F(Vector3Test, TakeAbsMaxFrom001, TestSize.Level1)
{
    Vector3f value(10.0f, 20.0f, 30.0f);
    Vector3f target(50.0f, 5.0f, 60.0f);
    value.TakeAbsMaxFrom(target);
    // x: |10| < |50| -> 50; y: |20| > |5| -> stays 20; z: |30| < |60| -> 60
    EXPECT_FLOAT_EQ(value.x_, 50.0f);
    EXPECT_FLOAT_EQ(value.y_, 20.0f);
    EXPECT_FLOAT_EQ(value.z_, 60.0f);
}

/**
 * @tc.name: TakeAbsMaxFrom002
 * @tc.desc: test Vector3<float>::TakeAbsMaxFrom with all smaller target (no change)
 * @tc.type: FUNC
 */
HWTEST_F(Vector3Test, TakeAbsMaxFrom002, TestSize.Level1)
{
    Vector3f value(100.0f, 100.0f, 100.0f);
    Vector3f target(10.0f, 20.0f, 30.0f);
    value.TakeAbsMaxFrom(target);
    EXPECT_FLOAT_EQ(value.x_, 100.0f);
    EXPECT_FLOAT_EQ(value.y_, 100.0f);
    EXPECT_FLOAT_EQ(value.z_, 100.0f);
}

/**
 * @tc.name: TakeAbsMaxFrom003
 * @tc.desc: test Vector3<float>::TakeAbsMaxFrom with negative values (sign preserved)
 * @tc.type: FUNC
 */
HWTEST_F(Vector3Test, TakeAbsMaxFrom003, TestSize.Level1)
{
    Vector3f value(10.0f, 10.0f, 10.0f);
    Vector3f target(-50.0f, -5.0f, -60.0f);
    value.TakeAbsMaxFrom(target);
    // x: |10| < |−50|=50 -> -50; y: |10| > |−5|=5 -> stays 10; z: |10| < |−60|=60 -> -60
    EXPECT_FLOAT_EQ(value.x_, -50.0f);
    EXPECT_FLOAT_EQ(value.y_, 10.0f);
    EXPECT_FLOAT_EQ(value.z_, -60.0f);
}

/**
 * @tc.name: TakeAbsMaxFrom004
 * @tc.desc: test Vector3<float>::TakeAbsMaxFrom with equal magnitude (strict <, no change)
 * @tc.type: FUNC
 */
HWTEST_F(Vector3Test, TakeAbsMaxFrom004, TestSize.Level1)
{
    Vector3f value(50.0f, -50.0f, 50.0f);
    Vector3f target(-50.0f, 50.0f, -50.0f);
    value.TakeAbsMaxFrom(target);
    // all |50| == |−50|, strict < means no change
    EXPECT_FLOAT_EQ(value.x_, 50.0f);
    EXPECT_FLOAT_EQ(value.y_, -50.0f);
    EXPECT_FLOAT_EQ(value.z_, 50.0f);
}
} // namespace OHOS::Rosen