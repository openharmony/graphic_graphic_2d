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

#include "common/rs_vector4.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class Vector4Test : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void Vector4Test::SetUpTestCase() {}
void Vector4Test::TearDownTestCase() {}
void Vector4Test::SetUp() {}
void Vector4Test::TearDown() {}

/**
 * @tc.name: Normalized001
 * @tc.desc: test results of Normalized
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector4Test, Normalized001, TestSize.Level1)
{
    Vector4 vector(1.f, 2.f, 2.f, 1.f); // for test
    vector.Normalized();
    EXPECT_TRUE(vector.data_[0] == 1.f);
}

/**
 * @tc.name: Dot001
 * @tc.desc: test results of Dot
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector4Test, Dot001, TestSize.Level1)
{
    Vector4 vector(1.f, 1.f, 1.f, 1.f); // for test
    Vector4 vectorNew(1.f, 2.f, 2.f, 1.f); // for test
    float sum = vector.Dot(vectorNew);
    EXPECT_TRUE(sum != 1.f);
}

/**
 * @tc.name: GetSqrLength001
 * @tc.desc: test results of GetSqrLength
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector4Test, GetSqrLength001, TestSize.Level1)
{
    Vector4 vector(1.f, 1.f, 1.f, 1.f); // for test
    float sum = vector.GetSqrLength();
    EXPECT_TRUE(sum == 4.f);
}

/**
 * @tc.name: GetLength001
 * @tc.desc: test results of GetLength
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector4Test, GetLength001, TestSize.Level1)
{
    Vector4 vector(1.f, 1.f, 1.f, 1.f); // for test
    float res = vector.GetLength();
    EXPECT_TRUE(res == 2.f);
}

/**
 * @tc.name: Normalize001
 * @tc.desc: test results of Normalize
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector4Test, Normalize001, TestSize.Level1)
{
    Vector4 vector(1.f, 1.f, 1.f, 1.f); // for test
    float res = vector.Normalize();
    EXPECT_TRUE(res == 2.f);
}

/**
 * @tc.name: Identity001
 * @tc.desc: test results of Identity
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector4Test, Identity001, TestSize.Level1)
{
    Vector4 vector(1.f, 1.f, 1.f, 1.f); // for test
    vector.Identity();
    EXPECT_TRUE(vector.data_[0] == 0.f);
}

/**
 * @tc.name: IsInfinite001
 * @tc.desc: test results of IsInfinite
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector4Test, IsInfinite001, TestSize.Level1)
{
    double num = 3.1415926; // for test
    Vector4d vector(num, num, num, num); // for test
    vector.data_[0] = std::numeric_limits<double>::infinity();
    EXPECT_TRUE(vector.IsInfinite());
}

/**
 * @tc.name: IsIdentity001
 * @tc.desc: test results of IsIdentity
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector4Test, IsIdentity001, TestSize.Level1)
{
    Vector4 vector(0.f, 0.f, 0.f, 1.f); // for test
    EXPECT_TRUE(vector.IsIdentity());
}

/**
 * @tc.name: IsZero001
 * @tc.desc: test results of IsZero
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector4Test, IsZero001, TestSize.Level1)
{
    Vector4 vector(0.f, 0.f, 0.f, 0.f); // for test
    EXPECT_TRUE(vector.IsZero());
}

/**
 * @tc.name: SetValues001
 * @tc.desc: test results of SetValues
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector4Test, SetValues001, TestSize.Level1)
{
    Vector4 vector(0.f, 0.f, 0.f, 0.f); // for test
    vector.SetValues(1.f, 1.f, 1.f, 1.f);
    EXPECT_TRUE(vector.data_[0] == 1.f);
}

/**
 * @tc.name: SetZero001
 * @tc.desc: test results of SetZero
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector4Test, SetZero001, TestSize.Level1)
{
    Vector4 vector(1.f, 0.f, 0.f, 0.f); // for test
    vector.SetZero();
    EXPECT_TRUE(vector.data_[0] == 0.f);
}

/**
 * @tc.name: Minus001
 * @tc.desc: test results of operator-
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector4Test, Minus001, TestSize.Level1)
{
    Vector4 vector(1.f, 0.f, 0.f, 0.f); // for test
    Vector4 vectorNew(1.f, 2.f, 2.f, 1.f); // for test
    Vector4 vectorRes = vector - vectorNew;
    EXPECT_TRUE(vectorRes.data_[0] == 0.f);
}

/**
 * @tc.name: Minus002
 * @tc.desc: test results of operator-
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector4Test, Minus002, TestSize.Level1)
{
    Vector4 vector(1.f, 0.f, 0.f, 0.f); // for test
    EXPECT_TRUE((-vector).data_[0] == -1.f);
}

/**
 * @tc.name: Add001
 * @tc.desc: test results of operator+
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector4Test, Add001, TestSize.Level1)
{
    Vector4 vector(1.f, 0.f, 0.f, 0.f); // for test
    Vector4 vectorNew(1.f, 2.f, 2.f, 1.f); // for test
    Vector4 vectorRes = vector + vectorNew;
    EXPECT_TRUE(vectorRes.data_[0] == 2.f);
}

/**
 * @tc.name: Divide001
 * @tc.desc: test results of operator/
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector4Test, Divide001, TestSize.Level1)
{
    Vector4 vector(4.f, 2.f, 0.f, 0.f); // for test
    Vector4 vectorRes = vector / 2.f;
    EXPECT_TRUE(vectorRes.data_[0] == 2.f);
}

/**
 * @tc.name: Multiply001
 * @tc.desc: test results of operator*
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector4Test, Multiply001, TestSize.Level1)
{
    Vector4 vector(4.f, 2.f, 0.f, 0.f); // for test
    Vector4 vectorRes = vector * 2.f;
    EXPECT_TRUE(vectorRes.data_[0] == 8.f);
}

/**
 * @tc.name: Multiply002
 * @tc.desc: test results of operator*
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector4Test, Multiply002, TestSize.Level1)
{
    Vector4 vector(4.f, 2.f, 0.f, 0.f); // for test
    Vector4 vectorNew(1.f, 2.f, 2.f, 1.f); // for test
    Vector4 vectorRes = vector * vectorNew;
    EXPECT_TRUE(vectorRes.data_[0] == 4.f);
}

/**
 * @tc.name: MultiplyEqual001
 * @tc.desc: test results of operator*=
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector4Test, MultiplyEqual001, TestSize.Level1)
{
    Vector4 vector(4.f, 2.f, 0.f, 0.f); // for test
    Vector4 vectorNew(1.f, 2.f, 2.f, 1.f); // for test
    vector *= vectorNew;
    EXPECT_TRUE(vector.data_[0] == 4.f);
}

/**
 * @tc.name: Equal001
 * @tc.desc: test results of operator=
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector4Test, Equal001, TestSize.Level1)
{
    Vector4 vector(4.f, 2.f, 0.f, 0.f); // for test
    Vector4 vectorNew(1.f, 2.f, 2.f, 1.f); // for test
    vector = vectorNew;
    EXPECT_TRUE(vector.data_[0] == 1.f);
}

/**
 * @tc.name: Equal002
 * @tc.desc: test results of operator==
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector4Test, Equal002, TestSize.Level1)
{
    Vector4 vector(1.f, 2.f, 2.f, 1.f); // for test
    Vector4 vectorNew(1.f, 2.f, 2.f, 1.f); // for test
    EXPECT_TRUE(vector == vectorNew);
}

/**
 * @tc.name: UnEqual001
 * @tc.desc: test results of operator!=
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector4Test, UnEqual001, TestSize.Level1)
{
    Vector4 vector(1.f, 4.f, 2.f, 1.f); // for test
    Vector4 vectorNew(1.f, 2.f, 2.f, 1.f); // for test
    EXPECT_TRUE(vector != vectorNew);
}

/**
 * @tc.name: IsNearEqual001
 * @tc.desc: test results of IsNearEqual
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector4Test, IsNearEqual001, TestSize.Level1)
{
    Vector4 vector(1.f, 4.f, 2.f, 1.f); // for test
    Vector4 vectorNew(1.f, 2.f, 2.f, 1.f); // for test
    EXPECT_TRUE(vector.IsNearEqual(vectorNew, 3.f));
}

/**
 * @tc.name: Brackets001
 * @tc.desc: test results of operator[]
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector4Test, Brackets001, TestSize.Level1)
{
    Vector4 vector(1.f, 4.f, 2.f, 1.f); // for test
    vector.data_[1] += 1.f;
    EXPECT_TRUE(vector.data_[1] == 5.f);
}

/**
 * @tc.name: Brackets002
 * @tc.desc: test results of operator[]
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector4Test, Brackets002, TestSize.Level1)
{
    const Vector4 vector(1.f, 4.f, 2.f, 1.f); // for test
    EXPECT_TRUE(vector.data_[0] == 1.f);
}

/**
 * @tc.name: GetData001
 * @tc.desc: test results of GetData
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector4Test, GetData001, TestSize.Level1)
{
    Vector4 vector(1.f, 4.f, 2.f, 1.f); // for test
    vector.GetData();
    EXPECT_TRUE(vector.data_[1] != 2.f);
}

/**
 * @tc.name: Scale001
 * @tc.desc: test results of Scale
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector4Test, Scale001, TestSize.Level1)
{
    Vector4 vector(1.f, 4.f, 2.f, 1.f); // for test
    vector.Scale(2.f);
    EXPECT_TRUE(vector.data_[0] == 2.f);
}

/**
 * @tc.name: Sub001
 * @tc.desc: test results of Sub
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector4Test, Sub001, TestSize.Level1)
{
    Vector4 vector(1.f, 4.f, 2.f, 1.f); // for test
    Vector4 vectorNew(1.f, 2.f, 2.f, 1.f); // for test
    vector.Sub(vectorNew);
    EXPECT_TRUE(vector.data_[0] == 0.f);
}

/**
 * @tc.name: Add002
 * @tc.desc: test results of Add
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector4Test, Add002, TestSize.Level1)
{
    Vector4 vector(1.f, 4.f, 2.f, 1.f); // for test
    Vector4 vectorNew(1.f, 2.f, 2.f, 1.f); // for test
    vector.Add(vectorNew);
    EXPECT_TRUE(vector.data_[0] == 2.f);
}

/**
 * @tc.name: Multiply003
 * @tc.desc: test results of Multiply
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector4Test, Multiply003, TestSize.Level1)
{
    Vector4 vector(1.f, 4.f, 2.f, 1.f); // for test
    Vector4 vectorNew(1.f, 2.f, 2.f, 1.f); // for test
    vector.Multiply(vectorNew);
    EXPECT_TRUE(vector.data_[0] == 1.f);
}

/**
 * @tc.name: Div001
 * @tc.desc: test results of Div
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector4Test, Div001, TestSize.Level1)
{
    Vector4 vector(1.f, 4.f, 2.f, 1.f); // for test
    Vector4 vectorNew(1.f, 2.f, 2.f, 1.f); // for test
    vector.Div(vectorNew);
    EXPECT_TRUE(vector.data_[1] == 2.f);
}

/**
 * @tc.name: Negate001
 * @tc.desc: test results of Negate
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector4Test, Negate001, TestSize.Level1)
{
    Vector4 vector(1.f, 4.f, 2.f, 1.f); // for test
    vector.Negate();
    EXPECT_TRUE(vector.data_[0] == -1.f);
}

/**
 * @tc.name: Absolute001
 * @tc.desc: test results of Absolute
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector4Test, Absolute001, TestSize.Level1)
{
    Vector4 vector(1.f, -4.f, 2.f, 1.f); // for test
    vector.Absolute();
    EXPECT_TRUE(vector.data_[1] == 4.f);
}

/**
 * @tc.name: Min001
 * @tc.desc: test results of Min
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector4Test, Min001, TestSize.Level1)
{
    Vector4 a(1.f, 2.f, 2.f, 1.f); // for test
    Vector4 b(1.f, 1.f, 1.f, 1.f); // for test
    Vector4 result(2.f, 1.f, 2.f, 1.f); // for test
    Vector4<float>::Min(a, b, result);
    EXPECT_TRUE(result.data_[1] == 1.f);
}

/**
 * @tc.name: Max001
 * @tc.desc: test results of Max
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector4Test, Max001, TestSize.Level1)
{
    Vector4 a(1.f, 2.f, 2.f, 1.f); // for test
    Vector4 b(1.f, 1.f, 1.f, 1.f); // for test
    Vector4 result(2.f, 1.f, 2.f, 1.f); // for test
    Vector4<float>::Max(a, b, result);
    EXPECT_TRUE(result.data_[1] == 2.f);
}

/**
 * @tc.name: Mix001
 * @tc.desc: test results of Mix
 * @tc.type: FUNC
 * @tc.require: issueI9MO8D
 */
HWTEST_F(Vector4Test, Mix001, TestSize.Level1)
{
    Vector4 min(1.f, 1.f, 2.f, 1.f); // for test
    Vector4 max(1.f, 2.f, 1.f, 1.f); // for test
    Vector4 result(2.f, 1.f, 2.f, 1.f); // for test
    Vector4<float>::Mix(min, max, 2.f, result);
    EXPECT_TRUE(result.data_[1] == 3.f);
}
} // namespace OHOS::Rosen