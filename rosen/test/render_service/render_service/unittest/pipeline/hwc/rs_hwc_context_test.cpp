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
const OHOS::Rosen::RECTI DEFAULT_RECT = {0, 80, 1000, 1000};
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
 * @tc.name: CheckPackageInConfigList
 * @tc.desc: Test CheckPackageInConfigList
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSHwcContextTest, CheckPackageInConfigList, TestSize.Level1)
{
    auto hwcContext = std::make_shared<RSHwcContext>();
    std::vector<std::string> pkgs = {};
    pkgs.push_back("com.youku.next");
    auto& rsCommonHook = RsCommonHook::Instance();
    hwcContext->sourceTuningConfig_["com.youku.next"] = "1";
    hwcContext->CheckPackageInConfigList(pkgs);
    auto result1 = rsCommonHook.GetVideoSurfaceFlag();
    ASSERT_TRUE(result1);
    hwcContext->sourceTuningConfig_["com.youku.next"] = "2";
    hwcContext->CheckPackageInConfigList(pkgs);
    auto result2 = rsCommonHook.GetHardwareEnabledByHwcnodeBelowSelfInAppFlag();
    ASSERT_TRUE(result2);
    hwcContext->solidLayerConfig_["com.youku.next"] = "1";
    hwcContext->CheckPackageInConfigList(pkgs);
    auto result3 = rsCommonHook.GetIsWhiteListForSolidColorLayerFlag();
    ASSERT_TRUE(result3);
    hwcContext->hwcSolidLayerConfig_["com.youku.next"] = "1";
    hwcContext->CheckPackageInConfigList(pkgs);
    auto result4 = rsCommonHook.GetIsVideoSurfaceFlag();
    ASSERT_TRUE(result4);
    hwcContext->hwcSourceTuningConfig_["com.youku.next"] = "2";
    hwcContext->CheckPackageInConfigList(pkgs);
    auto result5 = rsCommonHook.GetHardwareEnabledByHwcnodeBelowSelfInAppFlag();
    ASSERT_TRUE(result5);
    hwcContext->hwcSolidLayerConfig_["com.youku.next"] = "1";
    hwcContext->CheckPackageInConfigList(pkgs);
    auto result6 = rsCommonHook.GetIsWhiteListForSolidColorLayerFlag();
    ASSERT_TRUE(result6);
    pkgs.push_back("yylx.danmaku.bili");
    hwcContext->CheckPackageInConfigList(pkgs);
    auto result7 = rsCommonHook.GetVideoSurfaceFlag();
    ASSERT_FALSE(result7);
    auto result8 = rsCommonHook.GetHardwareEnabledByHwcnodeBelowSelfInAppFlag();
    ASSERT_FALSE(result8);
    auto result9 = rsCommonHook.GetIsWhiteListForSolidColorLayerFlag();
    ASSERT_FALSE(result9);
}
} // namespace OHOS::Rosen