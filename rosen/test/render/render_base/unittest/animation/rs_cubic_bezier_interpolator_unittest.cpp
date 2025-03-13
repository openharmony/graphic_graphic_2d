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

#include "include/animation/rs_cubic_bezier_interpolator.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSCubicBezierInterpolatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSCubicBezierInterpolatorTest::SetUpTestCase() {}
void RSCubicBezierInterpolatorTest::TearDownTestCase() {}
void RSCubicBezierInterpolatorTest::SetUp() {}
void RSCubicBezierInterpolatorTest::TearDown() {}

/**
 * @tc.name: InterpolateImplTest
 * @tc.desc: test results of InterpolateImpl
 * @tc.type: FUNC
 */
HWTEST_F(RSCubicBezierInterpolatorTest, InterpolateImplTest, TestSize.Level1)
{
    float ctlX1 = 0.f;
    float ctlY1 = 0.f;
    float ctlX2 = 0.f;
    float ctlY2 = 0.f;
    RSCubicBezierInterpolator rsCubicBezierInterpolator(ctlX1, ctlY1, ctlX2, ctlY2);
    float input = 1.0f;
    float res = rsCubicBezierInterpolator.InterpolateImpl(input);
    EXPECT_EQ(res, 1.0f);

    input = 0.0f;
    res = rsCubicBezierInterpolator.InterpolateImpl(input);
    EXPECT_NE(res, 1.0f);
}

/**
 * @tc.name: MarshallingTest
 * @tc.desc: test results of Marshalling
 * @tc.type: FUNC
 */
HWTEST_F(RSCubicBezierInterpolatorTest, MarshallingTest, TestSize.Level1)
{
    float ctlX1 = 0.f;
    float ctlY1 = 0.f;
    float ctlX2 = 0.f;
    float ctlY2 = 0.f;
    RSCubicBezierInterpolator rsCubicBezierInterpolator(ctlX1, ctlY1, ctlX2, ctlY2);
    Parcel parcel;
    bool res = rsCubicBezierInterpolator.Marshalling(parcel);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: UnmarshallingTest
 * @tc.desc: test results of Unmarshalling
 * @tc.type: FUNC
 */
HWTEST_F(RSCubicBezierInterpolatorTest, UnmarshallingTest, TestSize.Level1)
{
    float ctlX1 = 0.f;
    float ctlY1 = 0.f;
    float ctlX2 = 0.f;
    float ctlY2 = 0.f;
    RSCubicBezierInterpolator rsCubicBezierInterpolator(ctlX1, ctlY1, ctlX2, ctlY2);
    Parcel parcel;
    RSCubicBezierInterpolator* interpolator;
    interpolator = rsCubicBezierInterpolator.Unmarshalling(parcel);
    EXPECT_EQ(interpolator, nullptr);
}

/**
 * @tc.name: BinarySearchTest
 * @tc.desc: test results of BinarySearch
 * @tc.type: FUNC
 */
HWTEST_F(RSCubicBezierInterpolatorTest, BinarySearchTest, TestSize.Level1)
{
    float ctlX1 = 0.f;
    float ctlY1 = 0.f;
    float ctlX2 = 0.f;
    float ctlY2 = 0.f;
    RSCubicBezierInterpolator rsCubicBezierInterpolator(ctlX1, ctlY1, ctlX2, ctlY2);
    float key = 0.000000000125f;
    int res = rsCubicBezierInterpolator.BinarySearch(key);
    EXPECT_NE(res, 0);

    key = 0.1f;
    res = rsCubicBezierInterpolator.BinarySearch(key);
    EXPECT_NE(res, 0);

    key = 1e-6f;
    res = rsCubicBezierInterpolator.BinarySearch(key);
    EXPECT_NE(res, 0);
}
} // namespace OHOS::Rosen