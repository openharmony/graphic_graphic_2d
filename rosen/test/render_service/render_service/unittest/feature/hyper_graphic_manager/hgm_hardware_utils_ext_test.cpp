/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <parameter.h>
#include <parameters.h>

#include "gtest/gtest.h"

#include "hyper_graphic_manager/hgm_hardware_utils.h"
#include "hdi_output.h"
#include "hgm_core.h"
#include "params/rs_render_params.h"
#include "screen_manager/rs_screen.h"
#include "screen_manager/rs_screen_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::system {
bool GetBoolParameter(const std::string &key, bool def)
{
    return false;
}

std::string GetParameter(const std::string &key, const std::string &def)
{
    return "2,2,0,0";
}
} // namespace OHOS::system

namespace OHOS::Rosen {
namespace {
constexpr ScreenId SCREEN_ID = 12;
}

class HgmHardwareUtilsExtTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void HgmHardwareUtilsExtTest::SetUpTestCase() {}
void HgmHardwareUtilsExtTest::TearDownTestCase() {}
void HgmHardwareUtilsExtTest::SetUp() {}
void HgmHardwareUtilsExtTest::TearDown() {}

/**
 * @tc.name: ExecuteSwitchRefreshRateTest
 * @tc.desc: test HgmHardwareUtils.ExecuteSwitchRefreshRate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmHardwareUtilsExtTest, ExecuteSwitchRefreshRateTest, TestSize.Level1)
{
    auto hgmHardwareUtils = std::make_shared<HgmHardwareUtils>();
    ASSERT_NE(hgmHardwareUtils, nullptr);

    hgmHardwareUtils->ExecuteSwitchRefreshRate(SCREEN_ID);
}

/**
 * @tc.name: SwitchRefreshRateTest
 * @tc.desc: test HgmHardwareUtils.SwitchRefreshRate
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmHardwareUtilsExtTest, SwitchRefreshRateTest, TestSize.Level1)
{
    auto hgmHardwareUtils = std::make_shared<HgmHardwareUtils>();
    ASSERT_NE(hgmHardwareUtils, nullptr);
    OutputPtr output = HdiOutput::CreateHdiOutput(SCREEN_ID);
    ASSERT_NE(output, nullptr);
    auto screenManager = sptr<RSScreenManager>::MakeSptr();
    ASSERT_NE(screenManager, nullptr);
    auto& hgmCore = hgmHardwareUtils->hgmCore_;
    RSScreenManager* orgScmFromHgm = hgmCore.GetScreenManager();

    bool isSelfOwnedScreen = false;
    ScreenSize sSize = {720, 1080, 685, 1218};
    hgmCore.RemoveScreen(SCREEN_ID);
    hgmCore.AddScreen(SCREEN_ID, 0, sSize, isSelfOwnedScreen);
    auto screen = hgmCore.GetScreen(SCREEN_ID);
    screen->SetSelfOwnedScreenFlag(true);
    hgmCore.SetScreenManager(screenManager.GetRefPtr());

    PipelineParam pipelineParam =
        { .pendingScreenRefreshRate = 60, .frameTimestamp = 0, .pendingConstraintRelativeTime = 0, };
    hgmHardwareUtils->SwitchRefreshRate(output, 0, pipelineParam);

    auto rsScreen = std::make_shared<RSScreen>(SCREEN_ID);
    rsScreen->hdiScreen_ = HdiScreen::CreateHdiScreen(SCREEN_ID);
    rsScreen->property_.SetPowerStatus(ScreenPowerStatus::POWER_STATUS_ON);
    screenManager->screens_[SCREEN_ID] = rsScreen;
    hgmHardwareUtils->SwitchRefreshRate(output, 0, pipelineParam);
    
    rsScreen->property_.SetPowerStatus(ScreenPowerStatus::POWER_STATUS_OFF);
    hgmHardwareUtils->SwitchRefreshRate(output, 0, pipelineParam);

    rsScreen->property_.SetPowerStatus(ScreenPowerStatus::POWER_STATUS_SUSPEND);
    hgmHardwareUtils->SwitchRefreshRate(output, 0, pipelineParam);

    hgmCore.RemoveScreen(SCREEN_ID);
    hgmCore.SetScreenManager(orgScmFromHgm);
}
} // namespace OHOS::Rosen