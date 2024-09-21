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

#include "include/animation/rs_interpolator.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSInterpolatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSInterpolatorTest::SetUpTestCase() {}
void RSInterpolatorTest::TearDownTestCase() {}
void RSInterpolatorTest::SetUp() {}
void RSInterpolatorTest::TearDown() {}

class LinearInterpolatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void LinearInterpolatorTest::SetUpTestCase() {}
void LinearInterpolatorTest::TearDownTestCase() {}
void LinearInterpolatorTest::SetUp() {}
void LinearInterpolatorTest::TearDown() {}

class RSCustomInterpolatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSCustomInterpolatorTest::SetUpTestCase() {}
void RSCustomInterpolatorTest::TearDownTestCase() {}
void RSCustomInterpolatorTest::SetUp() {}
void RSCustomInterpolatorTest::TearDown() {}

/**
 * @tc.name: MarshallingTest
 * @tc.desc: test results of Marshalling
 * @tc.type: FUNC
 */
HWTEST_F(LinearInterpolatorTest, MarshallingTest, TestSize.Level1)
{
    uint64_t id = 0;
    LinearInterpolator linearInterpolator(id);
    Parcel parcel;
    bool res = linearInterpolator.Marshalling(parcel);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: UnmarshallingTest
 * @tc.desc: test results of Unmarshalling
 * @tc.type: FUNC
 */
HWTEST_F(LinearInterpolatorTest, UnmarshallingTest, TestSize.Level1)
{
    uint64_t id = 0;
    LinearInterpolator linearInterpolator(id);
    Parcel parcel;
    LinearInterpolator* linear;
    linear = linearInterpolator.Unmarshalling(parcel);
    EXPECT_EQ(linear, nullptr);
}

/**
 * @tc.name: MarshallingTest
 * @tc.desc: test results of Marshalling
 * @tc.type: FUNC
 */
HWTEST_F(RSCustomInterpolatorTest, MarshallingTest, TestSize.Level1)
{
    std::function<float(float)> func;
    int duration = 0;
    RSCustomInterpolator rsCustomInterpolator(func, duration);
    Parcel parcel;
    bool res = rsCustomInterpolator.Marshalling(parcel);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: UnmarshallingTest
 * @tc.desc: test results of Unmarshalling
 * @tc.type: FUNC
 */
HWTEST_F(RSCustomInterpolatorTest, UnmarshallingTest, TestSize.Level1)
{
    std::function<float(float)> func;
    int duration = 0;
    RSCustomInterpolator rsCustomInterpolator(func, duration);
    Parcel parcel;
    RSCustomInterpolator* rsCustom;
    rsCustom = rsCustomInterpolator.Unmarshalling(parcel);
    EXPECT_NE(rsCustom, nullptr);
}

/**
 * @tc.name: ConvertTest
 * @tc.desc: test results of Convert
 * @tc.type: FUNC
 */
HWTEST_F(RSCustomInterpolatorTest, ConvertTest, TestSize.Level1)
{
    std::function<float(float)> func;
    int duration = 0;
    RSCustomInterpolator rsCustomInterpolator(func, duration);
    rsCustomInterpolator.Convert(duration);
    EXPECT_EQ(duration, 0);

    auto f = [](float a) { return a; };
    func = f;
    RSCustomInterpolator rsCustomInterpolator1(func, duration);
    rsCustomInterpolator1.Convert(duration);
    EXPECT_NE(func, nullptr);

    duration = 1;
    rsCustomInterpolator1.Convert(duration);
    EXPECT_NE(func, nullptr);
}

/**
 * @tc.name: InterpolateImplTest
 * @tc.desc: test results of InterpolateImpl
 * @tc.type: FUNC
 */
HWTEST_F(RSCustomInterpolatorTest, InterpolateImplTest, TestSize.Level1)
{
    auto f = [](float a) { return a; };
    std::function<float(float)> func = f;
    int duration = 0;
    RSCustomInterpolator rsCustomInterpolator(func, duration);
    float input = 1e-6f;
    rsCustomInterpolator.Convert(1);
    float res = rsCustomInterpolator.InterpolateImpl(input);
    EXPECT_NE(res, 0.f);

    input = 300.0f;
    res = rsCustomInterpolator.InterpolateImpl(input);
    EXPECT_NE(res, 0.f);
}
} // namespace OHOS::Rosen