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
#include "utils/sampling_options.h"
#include "utils/scalar.h"
 
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SamplingOptionsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SamplingOptionsTest::SetUpTestCase() {}
void SamplingOptionsTest::TearDownTestCase() {}
void SamplingOptionsTest::SetUp() {}
void SamplingOptionsTest::TearDown() {}

/**
 * @tc.name: SamplingOptionsCreateAndDestroy001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(SamplingOptionsTest, SamplingOptionsCreateAndDestroy001, TestSize.Level1)
{
    // The best way to create SamplingOptions.
    std::unique_ptr<SamplingOptions> sampling = std::make_unique<SamplingOptions>();
    ASSERT_TRUE(sampling != nullptr);
}

/**
 * @tc.name: SamplingOptionsCreateAndDestroy002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(SamplingOptionsTest, SamplingOptionsCreateAndDestroy002, TestSize.Level1)
{
    // The best way to create SamplingOptions.
    CubicResampler cubicResampler;
    std::unique_ptr<SamplingOptions> sampling = std::make_unique<SamplingOptions>(cubicResampler);
    ASSERT_TRUE(sampling != nullptr);
}

/**
 * @tc.name: SamplingOptionsGetUseCubic001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(SamplingOptionsTest, SamplingOptionsGetUseCubic001, TestSize.Level1)
{
    std::unique_ptr<SamplingOptions> sampling = std::make_unique<SamplingOptions>();
    ASSERT_TRUE(sampling != nullptr);
    ASSERT_EQ(false, sampling->GetUseCubic());
}

/**
 * @tc.name: SamplingOptionsGetFilterMode001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(SamplingOptionsTest, SamplingOptionsGetFilterMode001, TestSize.Level1)
{
    std::unique_ptr<SamplingOptions> sampling = std::make_unique<SamplingOptions>();
    ASSERT_TRUE(sampling != nullptr);
    ASSERT_EQ(FilterMode::NEAREST, sampling->GetFilterMode());
}

/**
 * @tc.name: SamplingOptionsGetMipmapMode001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(SamplingOptionsTest, SamplingOptionsGetMipmapMode001, TestSize.Level1)
{
    std::unique_ptr<SamplingOptions> sampling = std::make_unique<SamplingOptions>();
    ASSERT_TRUE(sampling != nullptr);
    ASSERT_EQ(MipmapMode::NONE, sampling->GetMipmapMode());
}

/**
 * @tc.name: SamplingOptionsGetCubicCoffB001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(SamplingOptionsTest, SamplingOptionsGetCubicCoffB001, TestSize.Level1)
{
    std::unique_ptr<SamplingOptions> sampling = std::make_unique<SamplingOptions>();
    ASSERT_TRUE(sampling != nullptr);
    ASSERT_EQ(0, sampling->GetCubicCoffB());
}

/**
 * @tc.name: SamplingOptionsGetCubicCoffB002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(SamplingOptionsTest, SamplingOptionsGetCubicCoffB002, TestSize.Level1)
{
    CubicResampler cubicResampler;
    std::unique_ptr<SamplingOptions> sampling = std::make_unique<SamplingOptions>(cubicResampler);
    ASSERT_TRUE(sampling != nullptr);
    ASSERT_EQ(0, sampling->GetCubicCoffB());
}

/**
 * @tc.name: SamplingOptionsGetCubicCoffC001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(SamplingOptionsTest, SamplingOptionsGetCubicCoffC001, TestSize.Level1)
{
    std::unique_ptr<SamplingOptions> sampling = std::make_unique<SamplingOptions>();
    ASSERT_TRUE(sampling != nullptr);
    ASSERT_EQ(0, sampling->GetCubicCoffC());
}

/**
 * @tc.name: SamplingOptionsGetCubicCoffC002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(SamplingOptionsTest, SamplingOptionsGetCubicCoffC002, TestSize.Level1)
{
    CubicResampler cubicResampler;
    std::unique_ptr<SamplingOptions> sampling = std::make_unique<SamplingOptions>(cubicResampler);
    ASSERT_TRUE(sampling != nullptr);
    ASSERT_EQ(0, sampling->GetCubicCoffC());
}

/**
 * @tc.name: SamplingOptionsEqual001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SamplingOptionsTest, SamplingOptionsEqual001, TestSize.Level1)
{
    SamplingOptions sampling1;
    ASSERT_TRUE(sampling1 == sampling1);
}

/**
 * @tc.name: SamplingOptionsEqual002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SamplingOptionsTest, SamplingOptionsEqual002, TestSize.Level1)
{
    SamplingOptions sampling1;
    SamplingOptions sampling2 = sampling1;
    EXPECT_FALSE(sampling2 != sampling1);
}

/**
 * @tc.name: SamplingOptionsEqual003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(SamplingOptionsTest, SamplingOptionsEqual003, TestSize.Level1)
{
    SamplingOptions sampling1;
    SamplingOptions sampling2;
    EXPECT_FALSE(sampling1 != sampling2);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS