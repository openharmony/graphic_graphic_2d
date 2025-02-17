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
#include <parameters.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr const char* VDEC_HFBC_SWITCH = "vendor.vdec.hfbc.disable.fromuser";
}

class HgmHfbcConfigTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

/**
 * @tc.name: HandleHfbcConfig
 * @tc.desc: Verify the execution process of HandleHfbcConfig function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmHfbcConfigTest, HandleHfbcConfig01, Function | SmallTest | Level1)
{
    std::vector<std::string> packageList;
    packageList.emplace_back("com.test.banapp");
    packageList.emplace_back("com.test.allowapp");
    ASSERT_NO_FATAL_FAILURE({HgmHfbcConfig::HandleHfbcConfig(packageList);});
    std::string hfbcStatus = system::GetParameter(VDEC_HFBC_SWITCH, "0");
    bool ret = hfbcStatus == "-1";
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: HandleHfbcConfig
 * @tc.desc: Verify the execution process of HandleHfbcConfig function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmHfbcConfigTest, HandleHfbcConfig02, Function | SmallTest | Level1)
{
    std::vector<std::string> packageList;
    packageList.emplace_back("com.test.allowapp");
    ASSERT_NO_FATAL_FAILURE({HgmHfbcConfig::HandleHfbcConfig(packageList);});
    // hfbc can be enabled now
    std::string hfbcStatus = system::GetParameter(VDEC_HFBC_SWITCH, "0");
    bool ret = hfbcStatus == "0";
    ASSERT_EQ(ret, 1);
}

} // namespace Rosen
} // namespace OHOS
