/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "animation/rs_spring_interpolator.h"
#include "animation/rs_steps_interpolator.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
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

/**
 * @tc.name: RSStepInterpolatorTest001
 * @tc.desc: Verify the RSStepInterpolator
 * @tc.type:FUNC
 */
HWTEST_F(RSInterpolatorTest, RSStepInterpolatorTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInterpolatorTest RSStepInterpolatorTest001 start";

    Parcel parcel1;
    auto interpolator = RSStepsInterpolator::Unmarshalling(parcel1);
    EXPECT_EQ(interpolator, nullptr);

    Parcel parcel2;
    parcel2.WriteInt32(1);
    interpolator = RSStepsInterpolator::Unmarshalling(parcel2);
    EXPECT_EQ(interpolator, nullptr);

    Parcel parcel3;
    parcel3.WriteInt32(1);
    parcel3.WriteInt32(0);
    interpolator = RSStepsInterpolator::Unmarshalling(parcel3);
    EXPECT_TRUE(interpolator != nullptr);

    GTEST_LOG_(INFO) << "RSInterpolatorTest RSStepInterpolatorTest001 end";
}

/**
 * @tc.name: RSSpringInterpolatorTest001
 * @tc.desc: Verify the RSSpringInterpolator
 * @tc.type:FUNC
 */
HWTEST_F(RSInterpolatorTest, RSSpringInterpolatorTest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInterpolatorTest RSSpringInterpolatorTest001 start";

    Parcel parcel1;
    auto interpolator = RSSpringInterpolator::Unmarshalling(parcel1);
    EXPECT_EQ(interpolator, nullptr);

    Parcel parcel2;
    parcel2.WriteFloat(1.0f);
    interpolator = RSSpringInterpolator::Unmarshalling(parcel2);
    EXPECT_EQ(interpolator, nullptr);

    Parcel parcel3;
    parcel3.WriteFloat(1.0f);
    parcel3.WriteFloat(1.0f);
    interpolator = RSSpringInterpolator::Unmarshalling(parcel3);
    EXPECT_EQ(interpolator, nullptr);

    Parcel parcel4;
    parcel4.WriteFloat(1.0f);
    parcel4.WriteFloat(1.0f);
    parcel4.WriteFloat(1.0f);
    interpolator = RSSpringInterpolator::Unmarshalling(parcel4);
    EXPECT_TRUE(interpolator != nullptr);

    GTEST_LOG_(INFO) << "RSInterpolatorTest RSSpringInterpolatorTest001 end";
}

/**
 * @tc.name: RSSpringInterpolatorTest002
 * @tc.desc: Verify the RSSpringInterpolator
 * @tc.type:FUNC
 */
HWTEST_F(RSInterpolatorTest, RSSpringInterpolatorTest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSInterpolatorTest RSSpringInterpolatorTest002 start";

    RSSpringInterpolator interpolator(0.1f, 1.0f, 1.0f);
    auto result = interpolator.Interpolate(0.0f);
    EXPECT_EQ(result, 0.0f);

    GTEST_LOG_(INFO) << "RSInterpolatorTest RSSpringInterpolatorTest002 end";
}

} // namespace Rosen
} // namespace OHOS