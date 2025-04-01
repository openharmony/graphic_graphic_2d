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
    SOCPerfParam::SetMultilayersSOCPerfEnable(true);
    EXPECT_EQ(SOCPerfParam::IsMultilayersSOCPerfEnable(), true);
    SOCPerfParam::SetMultilayersSOCPerfEnable(false);
    EXPECT_EQ(SOCPerfParam::IsMultilayersSOCPerfEnable(), false);
}

/**
 * @tc.name: SetBlurSOCPerfEnable
 * @tc.desc: Verify the SetBlurSOCPerfEnable function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(SOCPerfParamTest, SetBlurSOCPerfEnable, Function | SmallTest | Level1)
{
    SOCPerfParam::SetBlurSOCPerfEnable(true);
    EXPECT_EQ(SOCPerfParam::IsBlurSOCPerfEnable(), true);
    SOCPerfParam::SetBlurSOCPerfEnable(false);
    EXPECT_EQ(SOCPerfParam::IsBlurSOCPerfEnable(), false);
}
} // namespace Rosen
} // namespace OHOS