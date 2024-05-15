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
} // namespace OHOS::Rosen