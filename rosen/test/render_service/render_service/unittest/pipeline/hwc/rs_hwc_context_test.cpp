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

#include "common/rs_common_hook.h"
#include "gtest/gtest.h"
#include "hgm_core.h"
#include "limit_number.h"
#include "pipeline/rs_test_util.h"
#include "pipeline/hwc/rs_hwc_context.h"
#include "system/rs_system_parameters.h"

using namespace testing;
using namespace testing::ext;
namespace {
const OHOS::Rosen::RectI DEFAULT_RECT = {0, 80, 1000, 1000};
}
namespace OHOS::Rosen {
class RSHwcContextTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSHwcContextTest::SetUpTestCase()
{
    RSTestUtil::InitRenderNodeGC();
}
void RSHwcContextTest::TearDownTestCase() {}
void RSHwcContextTest::SetUp() {}
void RSHwcContextTest::TearDown() {}

/**
 * @tc.name: CheckPackageInConfigList001
 * @tc.desc: Test CheckPackageInConfigList
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHwcContextTest, CheckPackageInConfigList001, TestSize.Level1)
{
    std::unordered_map<std::string, std::string> sourceTuningConfig = {{"com.youku.next", "1"}};
    std::unordered_map<std::string, std::string> solidLayerConfig = {{"com.youku.next", "1"}};
    auto hwcContext = std::make_shared<RSHwcContext>(sourceTuningConfig, solidLayerConfig);
    std::vector<std::string> pkgs;
    pkgs.push_back("com.youku.next");
    auto& rsCommonHook = RsCommonHook::Instance();
    hwcContext->sourceTuningConfig_["com.youku.next"] = "1";
    hwcContext->solidLayerConfig_["com.youku.next"] = "1";
    hwcContext->CheckPackageInConfigList(pkgs);
    EXPECT_TRUE(rsCommonHook.GetVideoSurfaceFlag());
    EXPECT_FALSE(rsCommonHook.GetHardwareEnabledByHwcnodeBelowSelfInAppFlag());
    EXPECT_FALSE(rsCommonHook.GetHardwareEnabledByBackgroundAlphaFlag());
    EXPECT_TRUE(rsCommonHook.GetIsWhiteListForSolidColorLayerFlag());

    hwcContext->sourceTuningConfig_["com.youku.next"] = "2";
    hwcContext->solidLayerConfig_["com.youku.next"] = "2";
    hwcContext->CheckPackageInConfigList(pkgs);
    EXPECT_TRUE(rsCommonHook.GetVideoSurfaceFlag());
    EXPECT_FALSE(rsCommonHook.GetHardwareEnabledByHwcnodeBelowSelfInAppFlag());
    EXPECT_FALSE(rsCommonHook.GetHardwareEnabledByBackgroundAlphaFlag());
    EXPECT_TRUE(rsCommonHook.GetIsWhiteListForSolidColorLayerFlag());

    hwcContext->hwcSourceTuningConfig_["com.youku.next"] = "2";
    hwcContext->hwcSolidLayerConfig_["com.youku.next"] = "2";
    hwcContext->CheckPackageInConfigList(pkgs);
    EXPECT_FALSE(rsCommonHook.GetVideoSurfaceFlag());
    EXPECT_TRUE(rsCommonHook.GetHardwareEnabledByHwcnodeBelowSelfInAppFlag());
    EXPECT_TRUE(rsCommonHook.GetHardwareEnabledByBackgroundAlphaFlag());
    EXPECT_FALSE(rsCommonHook.GetIsWhiteListForSolidColorLayerFlag());

    hwcContext->sourceTuningConfig_["com.youku.next"] = "1";
    hwcContext->solidLayerConfig_["com.youku.next"] = "1";
    hwcContext->CheckPackageInConfigList(pkgs);
    EXPECT_TRUE(rsCommonHook.GetVideoSurfaceFlag());
    EXPECT_FALSE(rsCommonHook.GetHardwareEnabledByHwcnodeBelowSelfInAppFlag());
    EXPECT_FALSE(rsCommonHook.GetHardwareEnabledByBackgroundAlphaFlag());
    EXPECT_TRUE(rsCommonHook.GetIsWhiteListForSolidColorLayerFlag());
}

/**
 * @tc.name: CheckPackageInConfigList002
 * @tc.desc: Test CheckPackageInConfigList
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHwcContextTest, CheckPackageInConfigList002, TestSize.Level1)
{
    std::unordered_map<std::string, std::string> sourceTuningConfig = {{"com.youku.next", "1"}};
    std::unordered_map<std::string, std::string> solidLayerConfig = {{"com.youku.next", "1"}};
    auto hwcContext = std::make_shared<RSHwcContext>(sourceTuningConfig, solidLayerConfig);
    std::vector<std::string> pkgs = {"com.youku.next", "yylx.danmaku.bili"};
    auto& rsCommonHook = RsCommonHook::Instance();
    hwcContext->CheckPackageInConfigList(pkgs);
    EXPECT_FALSE(rsCommonHook.GetVideoSurfaceFlag());
    EXPECT_FALSE(rsCommonHook.GetHardwareEnabledByHwcnodeBelowSelfInAppFlag());
    EXPECT_FALSE(rsCommonHook.GetHardwareEnabledByBackgroundAlphaFlag());
    EXPECT_FALSE(rsCommonHook.GetIsWhiteListForSolidColorLayerFlag());
}

/**
 * @tc.name: CheckPackageInConfigList003
 * @tc.desc: Test CheckPackageInConfigList
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHwcContextTest, CheckPackageInConfigList003, TestSize.Level1)
{
    std::unordered_map<std::string, std::string> sourceTuningConfig = {{"com.youku.next", "1"}};
    std::unordered_map<std::string, std::string> solidLayerConfig = {{"com.youku.next", "1"}};
    auto hwcContext = std::make_shared<RSHwcContext>(sourceTuningConfig, solidLayerConfig);
    std::vector<std::string> pkgs;
    pkgs.push_back("com.youku.next");
    auto& rsCommonHook = RsCommonHook::Instance();
    hwcContext->sourceTuningConfig_["com.youku.next"] = "1";
    hwcContext->solidLayerConfig_["com.youku.next"] = "2";
    hwcContext->CheckPackageInConfigList(pkgs);
    EXPECT_TRUE(rsCommonHook.GetVideoSurfaceFlag());
    EXPECT_FALSE(rsCommonHook.GetHardwareEnabledByHwcnodeBelowSelfInAppFlag());
    EXPECT_FALSE(rsCommonHook.GetHardwareEnabledByBackgroundAlphaFlag());
    EXPECT_TRUE(rsCommonHook.GetIsWhiteListForSolidColorLayerFlag());

    hwcContext->sourceTuningConfig_["com.youku.next"] = "2";
    hwcContext->solidLayerConfig_["com.youku.next"] = "1";
    hwcContext->CheckPackageInConfigList(pkgs);
    EXPECT_TRUE(rsCommonHook.GetVideoSurfaceFlag());
    EXPECT_FALSE(rsCommonHook.GetHardwareEnabledByHwcnodeBelowSelfInAppFlag());
    EXPECT_FALSE(rsCommonHook.GetHardwareEnabledByBackgroundAlphaFlag());
    EXPECT_TRUE(rsCommonHook.GetIsWhiteListForSolidColorLayerFlag());

    hwcContext->hwcSourceTuningConfig_["com.youku.next"] = "2";
    hwcContext->hwcSolidLayerConfig_["com.youku.next"] = "2";
    hwcContext->CheckPackageInConfigList(pkgs);
    EXPECT_FALSE(rsCommonHook.GetVideoSurfaceFlag());
    EXPECT_TRUE(rsCommonHook.GetHardwareEnabledByHwcnodeBelowSelfInAppFlag());
    EXPECT_TRUE(rsCommonHook.GetHardwareEnabledByBackgroundAlphaFlag());
    EXPECT_TRUE(rsCommonHook.GetIsWhiteListForSolidColorLayerFlag());

    hwcContext->sourceTuningConfig_["com.youku.next"] = "1";
    hwcContext->solidLayerConfig_["com.youku.next"] = "2";
    hwcContext->CheckPackageInConfigList(pkgs);
    EXPECT_TRUE(rsCommonHook.GetVideoSurfaceFlag());
    EXPECT_FALSE(rsCommonHook.GetHardwareEnabledByHwcnodeBelowSelfInAppFlag());
    EXPECT_FALSE(rsCommonHook.GetHardwareEnabledByBackgroundAlphaFlag());
    EXPECT_FALSE(rsCommonHook.GetIsWhiteListForSolidColorLayerFlag());

    hwcContext->sourceTuningConfig_["com.youku.next"] = "3";
    hwcContext->CheckPackageInConfigList(pkgs);
    EXPECT_FALSE(rsCommonHook.GetVideoSurfaceFlag());
    EXPECT_TRUE(rsCommonHook.GetHardwareEnabledByHwcnodeBelowSelfInAppFlag());
    EXPECT_TRUE(rsCommonHook.GetHardwareEnabledByBackgroundAlphaFlag());
    EXPECT_FALSE(rsCommonHook.GetIsWhiteListForSolidColorLayerFlag());
}

/**
 * @tc.name: CheckPackageInConfigList004
 * @tc.desc: Test CheckPackageInConfigList
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHwcContextTest, CheckPackageInConfigList004, TestSize.Level1)
{
    std::unordered_map<std::string, std::string> sourceTuningConfig = {{"com.youku.next", "1"}};
    std::unordered_map<std::string, std::string> solidLayerConfig = {{"com.youku.next", "1"}};
    auto hwcContext = std::make_shared<RSHwcContext>(sourceTuningConfig, solidLayerConfig);
    std::vector<std::string> pkgs;
    pkgs.push_back("yylx.danmaku.bili");
    auto& rsCommonHook = RsCommonHook::Instance();
    hwcContext->sourceTuningConfig_["com.youku.next"] = "1";
    hwcContext->solidLayerConfig_["com.youku.next"] = "1";
    hwcContext->CheckPackageInConfigList(pkgs);
    EXPECT_FALSE(rsCommonHook.GetVideoSurfaceFlag());
    EXPECT_FALSE(rsCommonHook.GetHardwareEnabledByHwcnodeBelowSelfInAppFlag());
    EXPECT_FALSE(rsCommonHook.GetHardwareEnabledByBackgroundAlphaFlag());
    EXPECT_FALSE(rsCommonHook.GetIsWhiteListForSolidColorLayerFlag());
}
} // namespace OHOS::Rosen