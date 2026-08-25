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

#include "common/rs_vector2.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class Vector2Test : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void Vector2Test::SetUpTestCase() {}
void Vector2Test::TearDownTestCase() {}
void Vector2Test::SetUp() {}
void Vector2Test::TearDown() {}

/**
 * @tc.name: IsAbsNearEqual001
 * @tc.desc: test results of Vector2<float>::IsAbsNearEqual
 * @tc.type: FUNC
 */
HWTEST_F(Vector2Test, IsAbsNearEqual001, TestSize.Level1)
{
    Vector2f value(10.0f, 20.0f);
    Vector2f target(12.0f, 18.0f);
    // |10-12|=2 <= 5, |20-18|=2 <= 5 -> true
    Vector2f threshold(5.0f, 5.0f);
    EXPECT_TRUE(value.IsAbsNearEqual(target, threshold));

    // |10-12|=2 > 1 -> false
    Vector2f smallThreshold(1.0f, 1.0f);
    EXPECT_FALSE(value.IsAbsNearEqual(target, smallThreshold));

    // only one component outside threshold -> false
    Vector2f mixedThreshold(5.0f, 1.0f);
    EXPECT_FALSE(value.IsAbsNearEqual(target, mixedThreshold));
}

/**
 * @tc.name: IsAbsNearEqual002
 * @tc.desc: test Vector2<float>::IsAbsNearEqual with exact match and negative threshold
 * @tc.type: FUNC
 */
HWTEST_F(Vector2Test, IsAbsNearEqual002, TestSize.Level1)
{
    // exact match with zero threshold -> true
    Vector2f value(10.0f, 20.0f);
    Vector2f zeroThreshold(0.0f, 0.0f);
    EXPECT_TRUE(value.IsAbsNearEqual(value, zeroThreshold));

    // negative threshold: abs is used, so -5 behaves like 5
    Vector2f target(12.0f, 18.0f);
    Vector2f negThreshold(-5.0f, -5.0f);
    EXPECT_TRUE(value.IsAbsNearEqual(target, negThreshold));
}

/**
 * @tc.name: TakeAbsMaxFrom001
 * @tc.desc: test results of Vector2<float>::TakeAbsMaxFrom
 * @tc.type: FUNC
 */
HWTEST_F(Vector2Test, TakeAbsMaxFrom001, TestSize.Level1)
{
    Vector2f value(10.0f, 20.0f);
    Vector2f target(50.0f, 5.0f);
    value.TakeAbsMaxFrom(target);
    // x: |10| < |50| -> 50; y: |20| > |5| -> stays 20
    EXPECT_FLOAT_EQ(value.x_, 50.0f);
    EXPECT_FLOAT_EQ(value.y_, 20.0f);
}

/**
 * @tc.name: TakeAbsMaxFrom002
 * @tc.desc: test Vector2<float>::TakeAbsMaxFrom with all smaller target (no change)
 * @tc.type: FUNC
 */
HWTEST_F(Vector2Test, TakeAbsMaxFrom002, TestSize.Level1)
{
    Vector2f value(100.0f, 100.0f);
    Vector2f target(10.0f, 20.0f);
    value.TakeAbsMaxFrom(target);
    // all target abs values are smaller -> no change
    EXPECT_FLOAT_EQ(value.x_, 100.0f);
    EXPECT_FLOAT_EQ(value.y_, 100.0f);
}

/**
 * @tc.name: TakeAbsMaxFrom003
 * @tc.desc: test Vector2<float>::TakeAbsMaxFrom with negative values (sign preserved)
 * @tc.type: FUNC
 */
HWTEST_F(Vector2Test, TakeAbsMaxFrom003, TestSize.Level1)
{
    Vector2f value(10.0f, 10.0f);
    Vector2f target(-50.0f, -5.0f);
    value.TakeAbsMaxFrom(target);
    // x: |10| < |−50|=50 -> -50; y: |10| > |−5|=5 -> stays 10
    EXPECT_FLOAT_EQ(value.x_, -50.0f);
    EXPECT_FLOAT_EQ(value.y_, 10.0f);
}

/**
 * @tc.name: TakeAbsMaxFrom004
 * @tc.desc: test Vector2<float>::TakeAbsMaxFrom with equal magnitude (strict <, no change)
 * @tc.type: FUNC
 */
HWTEST_F(Vector2Test, TakeAbsMaxFrom004, TestSize.Level1)
{
    Vector2f value(50.0f, -50.0f);
    Vector2f target(-50.0f, 50.0f);
    value.TakeAbsMaxFrom(target);
    // |50| == |−50|, strict < means no change
    EXPECT_FLOAT_EQ(value.x_, 50.0f);
    EXPECT_FLOAT_EQ(value.y_, -50.0f);
}
} // namespace OHOS::Rosen
