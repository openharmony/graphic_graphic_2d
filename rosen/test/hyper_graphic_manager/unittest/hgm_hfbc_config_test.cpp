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
#include <limits>
#include <test_header.h>

#include "hgm_hfbc_config.h"
#include "hgm_test_base.h"
#include "parameters.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr const char* VDEC_HFBC_SWITCH = "vendor.vdec.hfbc.disable.fromuser";
HgmHfbcConfig g_hfbcConfig;
}

class HgmHfbcConfigTest : public HgmTestBase {
public:
    static void SetUpTestCase()
    {
        HgmTestBase::SetUpTestCase();
    }
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

/**
 * @tc.name: SetHfbcConfigMap
 * @tc.desc: Verify the result of SetHfbcConfigMap function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmHfbcConfigTest, SetHfbcConfigMap, Function | SmallTest | Level0)
{
    std::unordered_map<std::string, std::string> hfbcConfig = {
        { "com.test.allowapp", "1" }, { "com.test.allowapp2", "1" }
    };
    ASSERT_NO_FATAL_FAILURE({g_hfbcConfig.SetHfbcConfigMap(hfbcConfig);});
    EXPECT_EQ(g_hfbcConfig.hfbcConfig_.size(), hfbcConfig.size());
    for (const auto& pkg : hfbcConfig) {
        EXPECT_EQ(g_hfbcConfig.hfbcConfig_.find(pkg.first) != g_hfbcConfig.hfbcConfig_.end(), true);
    }
}

/**
 * @tc.name: SetHfbcControlMode
 * @tc.desc: Verify the result of SetHfbcControlMode function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmHfbcConfigTest, SetHfbcControlMode, Function | SmallTest | Level0)
{
    g_hfbcConfig.SetHfbcControlMode(true);
    EXPECT_EQ(g_hfbcConfig.isHfbcDisableListMode_, true);
    g_hfbcConfig.SetHfbcControlMode(false);
    EXPECT_EQ(g_hfbcConfig.isHfbcDisableListMode_, false);
}

/**
 * @tc.name: HandleHfbcConfig01
 * @tc.desc: Verify the execution process of HandleHfbcConfig function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmHfbcConfigTest, HandleHfbcConfig01, Function | SmallTest | Level0)
{
    std::unordered_map<std::string, std::string> hfbcConfig = {
        { "com.test.banapp", "1" }, { "com.test.banapp2", "1" }
    };
    ASSERT_NO_FATAL_FAILURE({ g_hfbcConfig.SetHfbcConfigMap(hfbcConfig); });
    EXPECT_EQ(g_hfbcConfig.hfbcConfig_.size(), hfbcConfig.size());

    g_hfbcConfig.SetHfbcControlMode(true);
    EXPECT_EQ(g_hfbcConfig.isHfbcDisableListMode_, true);

    std::vector<std::string> packageList;
    packageList.emplace_back("com.test.allowapp");
    packageList.emplace_back("com.test.allowapp2");
    ASSERT_NO_FATAL_FAILURE({ g_hfbcConfig.HandleHfbcConfig(packageList); });

    std::string hfbcStatus = system::GetParameter(VDEC_HFBC_SWITCH, "0");
    ASSERT_EQ(hfbcStatus == "0", true);
}

/**
 * @tc.name: HandleHfbcConfig02
 * @tc.desc: Verify the execution process of HandleHfbcConfig function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmHfbcConfigTest, HandleHfbcConfig02, Function | SmallTest | Level0)
{
    std::unordered_map<std::string, std::string> hfbcConfig = {
        { "com.test.allowapp", "1" }, { "com.test.allowapp2", "1" }
    };
    ASSERT_NO_FATAL_FAILURE({ g_hfbcConfig.SetHfbcConfigMap(hfbcConfig); });
    EXPECT_EQ(g_hfbcConfig.hfbcConfig_.size(), hfbcConfig.size());

    g_hfbcConfig.SetHfbcControlMode(false);
    EXPECT_EQ(g_hfbcConfig.isHfbcDisableListMode_, false);

    std::vector<std::string> packageList;
    packageList.emplace_back("com.test.allowapp");
    packageList.emplace_back("com.test.allowapp2");
    ASSERT_NO_FATAL_FAILURE({ g_hfbcConfig.HandleHfbcConfig(packageList); });

    std::string hfbcStatus = system::GetParameter(VDEC_HFBC_SWITCH, "0");
    ASSERT_EQ(hfbcStatus == "0", true);
}

} // namespace Rosen
} // namespace OHOS
