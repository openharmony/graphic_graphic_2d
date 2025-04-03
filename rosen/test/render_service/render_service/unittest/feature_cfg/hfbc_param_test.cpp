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
namespace {
bool g_blackListMode = true;

bool CheckHfbcStatus(const std::vector<std::string>& packageList)
{
    const std::unordered_map<std::string, std::string>& hfbcConfig = HFBCParam::GetHfbcConfigMap();

    bool isHfbcDisabled = false;
    // disable the HFBC, when the package name exists in the configList
    for (auto& pkg : packageList) {
        std::string pkgNameForCheck = pkg.substr(0, pkg.find(':'));
        bool isFind = hfbcConfig.find(pkgNameForCheck) != hfbcConfig.end();
        if (g_blackListMode && isFind) {
            isHfbcDisabled = true;
            break;
        } else if ((!g_blackListMode) && (!isFind)) {
            isHfbcDisabled = true;
            break;
        }
    }
    return isHfbcDisabled;
}
}

/**
 * @tc.name: SetHfbcConfigForApp
 * @tc.desc: Verify the SetHfbcConfigForApp function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HfbcParamTest, SetHfbcConfigForApp, Function | SmallTest | Level1)
{
    EXPECT_EQ(HFBCParam::GetHfbcConfigMap().size(), 0);

    std::string appName = "com.test.banapp";
    std::vector<std::string> packages = { appName };

    EXPECT_EQ(CheckHfbcStatus(packages), false);

    HFBCParam::SetHfbcConfigForApp(appName, "1");

    EXPECT_EQ(CheckHfbcStatus(packages), true);

    packages = { { "com.test.banapp2", "1"} };
    EXPECT_EQ(CheckHfbcStatus(packages), false);
}

/**
 * @tc.name: SetHfbcConfigForApp2
 * @tc.desc: Verify the SetHfbcConfigForApp function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HfbcParamTest, SetHfbcConfigForApp2, Function | SmallTest | Level1)
{
    // disable list mode
    g_blackListMode = false;
    EXPECT_EQ(HFBCParam::GetHfbcConfigMap().size(), 0);
    std::string appName = "com.test.banapp";
    std::vector<std::string> packages = { appName };

    EXPECT_EQ(CheckHfbcStatus(packages), true);

    HFBCParam::SetHfbcConfigForApp(appName, "1");

    EXPECT_EQ(CheckHfbcStatus(packages), false);

    packages = { { "com.test.banapp2", "1"} };
    EXPECT_EQ(CheckHfbcStatus(packages), true);
}
} // namespace Rosen
} // namespace OHOS