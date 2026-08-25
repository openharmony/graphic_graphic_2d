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

#include "common/rs_matrix3.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class Matrix3Test : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void Matrix3Test::SetUpTestCase() {}
void Matrix3Test::TearDownTestCase() {}
void Matrix3Test::SetUp() {}
void Matrix3Test::TearDown() {}

/**
 * @tc.name: IsAbsNearEqual001
 * @tc.desc: test results of Matrix3<float>::IsAbsNearEqual
 * @tc.type: FUNC
 */
HWTEST_F(Matrix3Test, IsAbsNearEqual001, TestSize.Level1)
{
    Matrix3f value(10.0f, 20.0f, 30.0f, 40.0f, 50.0f, 60.0f, 70.0f, 80.0f, 90.0f);
    Matrix3f target(12.0f, 18.0f, 33.0f, 38.0f, 48.0f, 62.0f, 68.0f, 78.0f, 88.0f);
    // all diffs are <= 5 -> true
    Matrix3f threshold(5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f, 5.0f);
    EXPECT_TRUE(value.IsAbsNearEqual(target, threshold));

    // all diffs are > 1 -> false
    Matrix3f smallThreshold(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    EXPECT_FALSE(value.IsAbsNearEqual(target, smallThreshold));
}

/**
 * @tc.name: IsAbsNearEqual002
 * @tc.desc: test Matrix3<float>::IsAbsNearEqual with exact match and negative threshold
 * @tc.type: FUNC
 */
HWTEST_F(Matrix3Test, IsAbsNearEqual002, TestSize.Level1)
{
    // exact match with zero threshold -> true
    Matrix3f value(10.0f, 20.0f, 30.0f, 40.0f, 50.0f, 60.0f, 70.0f, 80.0f, 90.0f);
    Matrix3f zeroThreshold(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    EXPECT_TRUE(value.IsAbsNearEqual(value, zeroThreshold));

    // negative threshold: abs is used, so -5 behaves like 5
    Matrix3f target(12.0f, 18.0f, 33.0f, 38.0f, 48.0f, 62.0f, 68.0f, 78.0f, 88.0f);
    Matrix3f negThreshold(-5.0f, -5.0f, -5.0f, -5.0f, -5.0f, -5.0f, -5.0f, -5.0f, -5.0f);
    EXPECT_TRUE(value.IsAbsNearEqual(target, negThreshold));
}

/**
 * @tc.name: TakeAbsMaxFrom001
 * @tc.desc: test results of Matrix3<float>::TakeAbsMaxFrom with mixed values
 * @tc.type: FUNC
 */
HWTEST_F(Matrix3Test, TakeAbsMaxFrom001, TestSize.Level1)
{
    Matrix3f value(10.0f, 20.0f, 30.0f, 40.0f, 50.0f, 60.0f, 70.0f, 80.0f, 90.0f);
    Matrix3f target(50.0f, 5.0f, 60.0f, 3.0f, 100.0f, 5.0f, 110.0f, 5.0f, 120.0f);
    value.TakeAbsMaxFrom(target);
    const float* data = value.GetConstData();
    EXPECT_FLOAT_EQ(data[0], 50.0f);  // |10| < |50| -> 50
    EXPECT_FLOAT_EQ(data[1], 20.0f);  // |20| > |5| -> stays 20
    EXPECT_FLOAT_EQ(data[2], 60.0f);  // |30| < |60| -> 60
    EXPECT_FLOAT_EQ(data[3], 40.0f);  // |40| > |3| -> stays 40
    EXPECT_FLOAT_EQ(data[4], 100.0f); // |50| < |100| -> 100
    EXPECT_FLOAT_EQ(data[5], 60.0f);  // |60| > |5| -> stays 60
    EXPECT_FLOAT_EQ(data[6], 110.0f); // |70| < |110| -> 110
    EXPECT_FLOAT_EQ(data[7], 80.0f);  // |80| > |5| -> stays 80
    EXPECT_FLOAT_EQ(data[8], 120.0f); // |90| < |120| -> 120
}

/**
 * @tc.name: TakeAbsMaxFrom002
 * @tc.desc: test Matrix3<float>::TakeAbsMaxFrom with all smaller target (no change)
 * @tc.type: FUNC
 */
HWTEST_F(Matrix3Test, TakeAbsMaxFrom002, TestSize.Level1)
{
    Matrix3f value(100.0f, 100.0f, 100.0f, 100.0f, 100.0f, 100.0f, 100.0f, 100.0f, 100.0f);
    Matrix3f target(10.0f, 20.0f, 30.0f, 40.0f, 50.0f, 60.0f, 70.0f, 80.0f, 90.0f);
    value.TakeAbsMaxFrom(target);
    const float* data = value.GetConstData();
    for (int i = 0; i < Matrix3f::MATRIX3_SIZE; i++) {
        EXPECT_FLOAT_EQ(data[i], 100.0f);
    }
}

/**
 * @tc.name: TakeAbsMaxFrom003
 * @tc.desc: test Matrix3<float>::TakeAbsMaxFrom with negative values (sign preserved)
 * @tc.type: FUNC
 */
HWTEST_F(Matrix3Test, TakeAbsMaxFrom003, TestSize.Level1)
{
    Matrix3f value(10.0f, 10.0f, 10.0f, 10.0f, 10.0f, 10.0f, 10.0f, 10.0f, 10.0f);
    Matrix3f target(-50.0f, -5.0f, -60.0f, -3.0f, -100.0f, -5.0f, -110.0f, -5.0f, -120.0f);
    value.TakeAbsMaxFrom(target);
    const float* data = value.GetConstData();
    EXPECT_FLOAT_EQ(data[0], -50.0f);  // |10| < |−50|=50 -> -50
    EXPECT_FLOAT_EQ(data[1], 10.0f);   // |10| > |−5|=5 -> stays 10
    EXPECT_FLOAT_EQ(data[2], -60.0f);  // |10| < |−60|=60 -> -60
    EXPECT_FLOAT_EQ(data[3], 10.0f);   // |10| > |−3|=3 -> stays 10
    EXPECT_FLOAT_EQ(data[4], -100.0f); // |10| < |−100|=100 -> -100
    EXPECT_FLOAT_EQ(data[5], 10.0f);   // |10| > |−5|=5 -> stays 10
    EXPECT_FLOAT_EQ(data[6], -110.0f); // |10| < |−110|=110 -> -110
    EXPECT_FLOAT_EQ(data[7], 10.0f);   // |10| > |−5|=5 -> stays 10
    EXPECT_FLOAT_EQ(data[8], -120.0f); // |10| < |−120|=120 -> -120
}

/**
 * @tc.name: TakeAbsMaxFrom004
 * @tc.desc: test Matrix3<float>::TakeAbsMaxFrom with equal magnitude (strict <, no change)
 * @tc.type: FUNC
 */
HWTEST_F(Matrix3Test, TakeAbsMaxFrom004, TestSize.Level1)
{
    Matrix3f value(50.0f, -50.0f, 50.0f, -50.0f, 50.0f, -50.0f, 50.0f, -50.0f, 50.0f);
    Matrix3f target(-50.0f, 50.0f, -50.0f, 50.0f, -50.0f, 50.0f, -50.0f, 50.0f, -50.0f);
    value.TakeAbsMaxFrom(target);
    const float* data = value.GetConstData();
    // all |50| == |−50|, strict < means no change
    for (int i = 0; i < Matrix3f::MATRIX3_SIZE; i++) {
        EXPECT_FLOAT_EQ(data[i], ((i % 2) == 0) ? 50.0f : -50.0f);
    }
}
} // namespace OHOS::Rosen