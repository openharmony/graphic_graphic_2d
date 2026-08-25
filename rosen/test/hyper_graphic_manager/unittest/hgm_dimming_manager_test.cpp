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

#include <gtest/gtest.h>

#include "hgm_dimming_manager.h"
#include "hgm_test_base.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class HgmDimmingManagerTest : public HgmTestBase {
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
 * @tc.name: SetDimmingTimeTest
 * @tc.desc: Test the dimming up and down timeout values are set correctly.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmDimmingManagerTest, SetDimmingTimeTest, TestSize.Level0)
{
    auto dimmingManager = HgmDimmingManager();
    dimmingManager.SetDimmingTimeoutConfig(nullptr);
    ASSERT_EQ(dimmingManager.dimmingUpTimeoutMs_, 0);
    ASSERT_EQ(dimmingManager.dimmingDownTimeoutMs_, 0);

    std::shared_ptr<PolicyConfigData> configData = std::make_shared<PolicyConfigData>();
    dimmingManager.SetDimmingTimeoutConfig(configData);
    ASSERT_EQ(dimmingManager.dimmingUpTimeoutMs_, 0);
    ASSERT_EQ(dimmingManager.dimmingDownTimeoutMs_, 0);

    configData->dimmingConfig_["dimming_up_timeout_ms"] = "error";
    configData->dimmingConfig_["dimming_down_timeout_ms"] = "error";
    dimmingManager.SetDimmingTimeoutConfig(configData);
    ASSERT_EQ(dimmingManager.dimmingUpTimeoutMs_, 0);
    ASSERT_EQ(dimmingManager.dimmingDownTimeoutMs_, 0);

    configData->dimmingConfig_["dimming_up_timeout_ms"] = "600";
    configData->dimmingConfig_["dimming_down_timeout_ms"] = "800";
    dimmingManager.SetDimmingTimeoutConfig(configData);
    ASSERT_EQ(dimmingManager.dimmingUpTimeoutMs_, 600);
    ASSERT_EQ(dimmingManager.dimmingDownTimeoutMs_, 800);
}

/**
 * @tc.name: CalcDimmingRefreshRateTest1
 * @tc.desc: Test the dimming refresh rate calculation under various conditions.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmDimmingManagerTest, CalcDimmingRefreshRateTest1, TestSize.Level0)
{
    auto dimmingManager = HgmDimmingManager();
    // timeout is 0, should return voteFps directly
    std::shared_ptr<PolicyConfigData> configData = std::make_shared<PolicyConfigData>();
    configData->dimmingConfig_["dimming_up_timeout_ms"] = "0";
    configData->dimmingConfig_["dimming_down_timeout_ms"] = "0";
    dimmingManager.SetDimmingTimeoutConfig(configData);
    uint32_t refreshRate = dimmingManager.CalcDimmingRefreshRate(OLED_60_HZ);
    ASSERT_EQ(refreshRate, OLED_60_HZ);
    configData->dimmingConfig_["dimming_up_timeout_ms"] = "0";
    configData->dimmingConfig_["dimming_down_timeout_ms"] = "100";
    dimmingManager.SetDimmingTimeoutConfig(configData);
    refreshRate = dimmingManager.CalcDimmingRefreshRate(OLED_120_HZ);
    ASSERT_EQ(refreshRate, OLED_120_HZ);
    configData->dimmingConfig_["dimming_up_timeout_ms"] = "100";
    configData->dimmingConfig_["dimming_down_timeout_ms"] = "0";
    dimmingManager.SetDimmingTimeoutConfig(configData);
    refreshRate = dimmingManager.CalcDimmingRefreshRate(OLED_60_HZ);
    ASSERT_EQ(refreshRate, OLED_60_HZ);
    // lightFactorStatus is NORMAL_HIGH or HIGH_LEVEL, should return voteFps directly
    configData->dimmingConfig_["dimming_up_timeout_ms"] = "300";
    configData->dimmingConfig_["dimming_down_timeout_ms"] = "400";
    dimmingManager.SetDimmingTimeoutConfig(configData);
    dimmingManager.SetLightFactorStatus(LightFactorStatus::NORMAL_HIGH);
    refreshRate = dimmingManager.CalcDimmingRefreshRate(OLED_120_HZ);
    ASSERT_EQ(refreshRate, OLED_120_HZ);
    dimmingManager.SetLightFactorStatus(LightFactorStatus::HIGH_LEVEL);
    refreshRate = dimmingManager.CalcDimmingRefreshRate(OLED_60_HZ);
    ASSERT_EQ(refreshRate, OLED_60_HZ);
    // refreshRate not in refreshRateVec_, should return voteFps directly
    dimmingManager.SetLightFactorStatus(LightFactorStatus::NORMAL_LOW);
    dimmingManager.SetRefreshRateVec({OLED_60_HZ, OLED_90_HZ, OLED_120_HZ});
    refreshRate = dimmingManager.CalcDimmingRefreshRate(OLED_144_HZ);
    ASSERT_EQ(refreshRate, OLED_144_HZ);
    refreshRate = dimmingManager.CalcDimmingRefreshRate(OLED_120_HZ);
    ASSERT_EQ(refreshRate, OLED_120_HZ);
    refreshRate = dimmingManager.CalcDimmingRefreshRate(OLED_30_HZ);
    ASSERT_EQ(refreshRate, OLED_30_HZ);
    refreshRate = dimmingManager.CalcDimmingRefreshRate(OLED_120_HZ);
    ASSERT_EQ(refreshRate, OLED_120_HZ);
}

/**
 * @tc.name: CalcDimmingRefreshRateTest2
 * @tc.desc: Test the dimming refresh rate calculation based on the current and vote refresh rates.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmDimmingManagerTest, CalcDimmingRefreshRateTest2, TestSize.Level0)
{
    auto dimmingManager = HgmDimmingManager();
    // 60->60, should return voteFps directly
    dimmingManager.SetLightFactorStatus(LightFactorStatus::NORMAL_LOW);
    dimmingManager.SetRefreshRateVec({OLED_60_HZ, OLED_90_HZ, OLED_120_HZ});
    std::shared_ptr<PolicyConfigData> configData = std::make_shared<PolicyConfigData>();
    configData->dimmingConfig_["dimming_up_timeout_ms"] = "300";
    configData->dimmingConfig_["dimming_down_timeout_ms"] = "400";
    dimmingManager.SetDimmingTimeoutConfig(configData);
    dimmingManager.currRefreshRate_ = OLED_60_HZ;
    uint32_t refreshRate = dimmingManager.CalcDimmingRefreshRate(OLED_60_HZ);
    ASSERT_EQ(refreshRate, OLED_60_HZ);
    // 60->120, should return 90 at first, then return 120 after dimmingUpTimeoutMs_
    refreshRate = dimmingManager.CalcDimmingRefreshRate(OLED_120_HZ);
    ASSERT_EQ(refreshRate, OLED_90_HZ);
    refreshRate = dimmingManager.CalcDimmingRefreshRate(OLED_120_HZ);
    ASSERT_EQ(refreshRate, OLED_90_HZ);
    usleep(300 * 1000); // sleep for 300ms
    refreshRate = dimmingManager.CalcDimmingRefreshRate(OLED_120_HZ);
    ASSERT_EQ(refreshRate, OLED_120_HZ);
    refreshRate = dimmingManager.CalcDimmingRefreshRate(OLED_120_HZ);
    ASSERT_EQ(refreshRate, OLED_120_HZ);
    usleep(300 * 1000); // sleep for 300ms
    refreshRate = dimmingManager.CalcDimmingRefreshRate(OLED_120_HZ);
    ASSERT_EQ(refreshRate, OLED_120_HZ);
    // 120->60, should return 90 at first, then return 60 after dimmingDownTimeoutMs_
    refreshRate = dimmingManager.CalcDimmingRefreshRate(OLED_60_HZ);
    ASSERT_EQ(refreshRate, OLED_90_HZ);
    refreshRate = dimmingManager.CalcDimmingRefreshRate(OLED_60_HZ);
    ASSERT_EQ(refreshRate, OLED_90_HZ);
    usleep(400 * 1000); // sleep for 400ms
    refreshRate = dimmingManager.CalcDimmingRefreshRate(OLED_60_HZ);
    ASSERT_EQ(refreshRate, OLED_60_HZ);
    refreshRate = dimmingManager.CalcDimmingRefreshRate(OLED_60_HZ);
    ASSERT_EQ(refreshRate, OLED_60_HZ);
    usleep(400 * 1000); // sleep for 400ms
    refreshRate = dimmingManager.CalcDimmingRefreshRate(OLED_60_HZ);
    ASSERT_EQ(refreshRate, OLED_60_HZ);
    // 60->120->60, interrupt, trigger callback
    dimmingManager.RegisterDimmingEventCallback([](int32_t dimmingTimeoutMs) {});
    refreshRate = dimmingManager.CalcDimmingRefreshRate(OLED_120_HZ);
    ASSERT_EQ(refreshRate, OLED_90_HZ);
    refreshRate = dimmingManager.CalcDimmingRefreshRate(OLED_60_HZ);
    ASSERT_EQ(refreshRate, OLED_60_HZ);
    usleep(400 * 1000); // sleep for 400ms
}
} // namespace Rosen
} // namespace OHOS
