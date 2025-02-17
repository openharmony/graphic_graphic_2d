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

#include "hfbc_param.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class HfbcParamTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HfbcParamTest::SetUpTestCase() {}
void HfbcParamTest::TearDownTestCase() {}
void HfbcParamTest::SetUp() {}
void HfbcParamTest::TearDown() {}

/**
 * @tc.name: SetHfbcConfigForApp
 * @tc.desc: Verify the SetHfbcConfigForApp function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HfbcParamTest, SetHfbcConfigForApp, Function | SmallTest | Level1)
{
    HfbcParam hfbcParam;
    std::string appName = "com.test.banapp";
    const std::unordered_map<std::string, std::string>& hfbcConfig = hfbcParam.GetHfbcConfigMap();

    EXPECT_EQ(hfbcConfig.find(appName) != hfbcConfig.end(), false);

    hfbcParam.SetHfbcConfigForApp(appName, "1");
    EXPECT_EQ(hfbcConfig.find(appName) != hfbcConfig.end(), true);
}

} // namespace Rosen
} // namespace OHOS