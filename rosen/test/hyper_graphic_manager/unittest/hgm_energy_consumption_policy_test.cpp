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

#include <gtest/gtest.h>

#include "hgm_energy_consumption_policy.h"

#include "common/rs_common_hook.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
constexpr int DEFAULT_MAX_FPS = 120;
constexpr int IDLE_FPS = 60;

class HgmEnergyConsumptionPolicyTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
    void SetConfigEnable(std::string isEnable);
    void SetIdleStateEnable(bool isIdle);
};

void HgmEnergyConsumptionPolicyTest::SetConfigEnable(std::string isEnable)
{
    std::unordered_map<std::string, std::string> animationPowerConfig = { { "animation_ltpo_power_enable", isEnable },
        { "animation_idle_fps", "60" }, { "animation_idle_duration", "2000" } };
    HgmEnergyConsumptionPolicy::Instance().SetEnergyConsumptionConfig(animationPowerConfig);
}

void HgmEnergyConsumptionPolicyTest::SetIdleStateEnable(bool isIdle)
{
    HgmEnergyConsumptionPolicy::Instance().SetEnergyConsumptionAssuranceMode(isIdle);
}

/**
 * @tc.name: SetEnergyConsumptionConfigTest1
 * @tc.desc: test results of SetEnergyConsumptionConfigTest1
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, SetEnergyConsumptionConfigTest1, TestSize.Level1)
{
    SetConfigEnable("true");
}

/**
 * @tc.name: SetEnergyConsumptionConfigTest2
 * @tc.desc: test results of SetEnergyConsumptionConfigTest2
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, SetEnergyConsumptionConfigTest2, TestSize.Level1)
{
    SetConfigEnable("false");
}

/**
 * @tc.name: SetEnergyConsumptionAssuranceModeTest1
 * @tc.desc: test results of SetEnergyConsumptionAssuranceModeTest1
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, SetEnergyConsumptionAssuranceModeTest1, TestSize.Level1)
{
    SetConfigEnable("true");
    SetIdleStateEnable(true);
    SetIdleStateEnable(false);
}

/**
 * @tc.name: SetEnergyConsumptionAssuranceModeTest2
 * @tc.desc: test results of SetEnergyConsumptionAssuranceModeTest2
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, SetEnergyConsumptionAssuranceModeTest2, TestSize.Level1)
{
    SetConfigEnable("true");
    SetIdleStateEnable(true);
    SetIdleStateEnable(true);
}

/**
 * @tc.name: SetEnergyConsumptionAssuranceModeTest3
 * @tc.desc: test results of SetEnergyConsumptionAssuranceModeTest3
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, SetEnergyConsumptionAssuranceModeTest3, TestSize.Level1)
{
    SetConfigEnable("false");
    SetIdleStateEnable(false);
    SetIdleStateEnable(true);
}

/**
 * @tc.name: StatisticAnimationTimeTest1
 * @tc.desc: test results of StatisticAnimationTimeTest1
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, StatisticAnimationTimeTest1, TestSize.Level1)
{
    SetConfigEnable("false");
    SetIdleStateEnable(false);
    uint64_t currentTime = 1719544264071;
    HgmEnergyConsumptionPolicy::Instance().StatisticAnimationTime(currentTime);
}

/**
 * @tc.name: StatisticAnimationTimeTest2
 * @tc.desc: test results of StatisticAnimationTimeTest2
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, StatisticAnimationTimeTest2, TestSize.Level1)
{
    SetConfigEnable("true");
    SetIdleStateEnable(false);
    uint64_t currentTime = 1719544264071;
    HgmEnergyConsumptionPolicy::Instance().StatisticAnimationTime(currentTime);
}

/**
 * @tc.name: StatisticAnimationTimeTest3
 * @tc.desc: test results of StatisticAnimationTimeTest3
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, StatisticAnimationTimeTest3, TestSize.Level1)
{
    SetConfigEnable("true");
    SetIdleStateEnable(true);
    uint64_t currentTime = 1719544264071;
    HgmEnergyConsumptionPolicy::Instance().StatisticAnimationTime(currentTime);
}

/**
 * @tc.name: StartNewAnimationTest1
 * @tc.desc: test results of StartNewAnimationTest1
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, StartNewAnimationTest1, TestSize.Level1)
{
    SetConfigEnable("true");
    HgmEnergyConsumptionPolicy::Instance().StartNewAnimation();
}

/**
 * @tc.name: StartNewAnimationTest2
 * @tc.desc: test results of StartNewAnimationTest2
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, StartNewAnimationTest2, TestSize.Level1)
{
    SetConfigEnable("true");
    RsCommonHook::Instance().OnStartNewAnimation();
}

/**
 * @tc.name: GetAnimationIdleFpsTest1
 * @tc.desc: test results of GetAnimationIdleFpsTest1
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, GetAnimationIdleFpsTest1, TestSize.Level1)
{
    SetConfigEnable("false");
    FrameRateRange rsRange = { DEFAULT_MAX_FPS, DEFAULT_MAX_FPS, DEFAULT_MAX_FPS };
    HgmEnergyConsumptionPolicy::Instance().GetAnimationIdleFps(rsRange);
    ASSERT_EQ(rsRange.max_, DEFAULT_MAX_FPS);
    ASSERT_EQ(rsRange.min_, DEFAULT_MAX_FPS);
    ASSERT_EQ(rsRange.preferred_, DEFAULT_MAX_FPS);
}

/**
 * @tc.name: GetAnimationIdleFpsTest2
 * @tc.desc: test results of GetAnimationIdleFpsTest2
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, GetAnimationIdleFpsTest2, TestSize.Level1)
{
    SetConfigEnable("true");
    HgmEnergyConsumptionPolicy::Instance().lastAnimationTimestamp_ = 1719544264071;
    HgmEnergyConsumptionPolicy::Instance().firstAnimationTimestamp_ = 1719544263071;
    FrameRateRange rsRange = { DEFAULT_MAX_FPS, DEFAULT_MAX_FPS, DEFAULT_MAX_FPS };
    HgmEnergyConsumptionPolicy::Instance().GetAnimationIdleFps(rsRange);
    ASSERT_EQ(rsRange.max_, DEFAULT_MAX_FPS);
    ASSERT_EQ(rsRange.min_, DEFAULT_MAX_FPS);
    ASSERT_EQ(rsRange.preferred_, DEFAULT_MAX_FPS);
}

/**
 * @tc.name: GetAnimationIdleFpsTest3
 * @tc.desc: test results of GetAnimationIdleFpsTest3
 * @tc.type: FUNC
 * @tc.require: issuesIA96Q3
 */
HWTEST_F(HgmEnergyConsumptionPolicyTest, GetAnimationIdleFpsTest3, TestSize.Level1)
{
    SetConfigEnable("true");
    HgmEnergyConsumptionPolicy::Instance().lastAnimationTimestamp_ = 1719544264071;
    HgmEnergyConsumptionPolicy::Instance().firstAnimationTimestamp_ = 1719544261071;
    FrameRateRange rsRange = { DEFAULT_MAX_FPS, DEFAULT_MAX_FPS, DEFAULT_MAX_FPS };
    HgmEnergyConsumptionPolicy::Instance().GetAnimationIdleFps(rsRange);
    ASSERT_EQ(rsRange.max_, IDLE_FPS);
    ASSERT_EQ(rsRange.min_, IDLE_FPS);
    ASSERT_EQ(rsRange.preferred_, IDLE_FPS);
}

} // namespace Rosen
} // namespace OHOS