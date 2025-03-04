/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include <test_header.h>

#include "socperf_param.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class SOCPerfParamTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SOCPerfParamTest::SetUpTestCase() {}
void SOCPerfParamTest::TearDownTestCase() {}
void SOCPerfParamTest::SetUp() {}
void SOCPerfParamTest::TearDown() {}

/**
 * @tc.name: SetMultilayersSOCPerfEnable
 * @tc.desc: Verify the SetMultilayersSOCPerfEnable function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SOCPerfParamTest, SetMultilayersSOCPerfEnable, Function | SmallTest | Level1)
{
    SOCPerfParam socPerfParam;
    socPerfParam.SetMultilayersSOCPerfEnable(true);
    EXPECT_EQ(socPerfParam.isMultilayersSOCPerfEnable_, true);
    socPerfParam.SetMultilayersSOCPerfEnable(false);
    EXPECT_EQ(socPerfParam.isMultilayersSOCPerfEnable_, false);
}

/**
 * @tc.name: SetUnlockSOCPerfEnable
 * @tc.desc: Verify the SetUnlockSOCPerfEnable function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SOCPerfParamTest, SetUnlockSOCPerfEnable, Function | SmallTest | Level1)
{
    SOCPerfParam socPerfParam;
    socPerfParam.SetUnlockSOCPerfEnable(true);
    EXPECT_EQ(socPerfParam.isUnlockSOCPerfEnable_, true);
    socPerfParam.SetUnlockSOCPerfEnable(false);
    EXPECT_EQ(socPerfParam.isUnlockSOCPerfEnable_, false);
}

/**
 * @tc.name: SetBlurSOCPerfEnable
 * @tc.desc: Verify the SetBlurSOCPerfEnable function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SOCPerfParamTest, SetBlurSOCPerfEnable, Function | SmallTest | Level1)
{
    SOCPerfParam socPerfParam;
    socPerfParam.SetBlurSOCPerfEnable(true);
    EXPECT_EQ(socPerfParam.isBlurSOCPerfEnable_, true);
    socPerfParam.SetBlurSOCPerfEnable(false);
    EXPECT_EQ(socPerfParam.isBlurSOCPerfEnable_, false);
}

/**
 * @tc.name: IsMultilayersSOCPerfEnable
 * @tc.desc: Verify the result of IsMultilayersSOCPerfEnable function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SOCPerfParamTest, IsMultilayersSOCPerfEnable, Function | SmallTest | Level1)
{
    SOCPerfParam socPerfParam;
    socPerfParam.isMultilayersSOCPerfEnable_ = true;
    EXPECT_TRUE(socPerfParam.IsMultilayersSOCPerfEnable());
    socPerfParam.isMultilayersSOCPerfEnable_ = false;
    EXPECT_FALSE(socPerfParam.IsMultilayersSOCPerfEnable());
}

/**
 * @tc.name: IsUnlockSOCPerfEnable
 * @tc.desc: Verify the result of IsUnlockSOCPerfEnable function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SOCPerfParamTest, IsUnlockSOCPerfEnable, Function | SmallTest | Level1)
{
    SOCPerfParam socPerfParam;
    socPerfParam.isUnlockSOCPerfEnable_ = true;
    EXPECT_TRUE(socPerfParam.IsUnlockSOCPerfEnable());
    socPerfParam.isUnlockSOCPerfEnable_ = false;
    EXPECT_FALSE(socPerfParam.IsUnlockSOCPerfEnable());
}

/**
 * @tc.name: IsBlurSOCPerfEnable
 * @tc.desc: Verify the result of IsBlurSOCPerfEnable function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SOCPerfParamTest, IsBlurSOCPerfEnable, Function | SmallTest | Level1)
{
    SOCPerfParam socPerfParam;
    socPerfParam.isBlurSOCPerfEnable_ = true;
    EXPECT_TRUE(socPerfParam.IsBlurSOCPerfEnable());
    socPerfParam.isBlurSOCPerfEnable_ = false;
    EXPECT_FALSE(socPerfParam.IsBlurSOCPerfEnable());
}

} // namespace Rosen
} // namespace OHOS