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
#include <limits>
#include <test_header.h>

#include "hgm_core.h"
#include "hgm_frame_rate_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    constexpr int32_t nodeIdOffset = 32;
    const std::string settingStrategyName = "99";
    const std::string otherPkgName = "com.pkg.other";
    const std::string defaultPidStr = "0";

    constexpr int32_t fps0 = 10;
    constexpr int32_t downFps0 = 90;
    constexpr int32_t pid0 = 10010;
    const std::string strategyName0 = "110";
    const std::string pkgName0 = "com.app10";
    const std::string appTypeName0 = "appType0";

    constexpr int32_t fps1 = 15;
    constexpr int32_t downFps1 = 120;
    constexpr int32_t pid1 = 10015;
    const std::string strategyName1 = "115";
    const std::string pkgName1 = "com.app15";
    const std::string appTypeName1 = "appType1";
}

struct PkgParam {
    std::string pkgName;
    int32_t fps;
    pid_t pid;
    std::shared_ptr<RSRenderFrameRateLinker> linker;
};

class HgmMultiAppStrategyTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp();
    void TearDown() {}
    void SetMultiAppStrategy(MultiAppStrategyType multiAppStrategyType, const std::string& strategyName = "");
    std::vector<std::string> CreateVotePkgs();

    std::vector<PkgParam> pkgParams_;
    std::shared_ptr<HgmMultiAppStrategy> multiAppStrategy_;
};

void HgmMultiAppStrategyTest::SetUp()
{
    multiAppStrategy_ = std::make_shared<HgmMultiAppStrategy>();
    // init config
    // set app config
    auto &strategyConfigs = multiAppStrategy_->GetStrategyConfigs();
    auto &screenSetting = multiAppStrategy_->GetScreenSetting();
    auto &appTypes = screenSetting.appTypes;

    strategyConfigs[settingStrategyName] = { .min = OLED_NULL_HZ, .max = OLED_120_HZ,
        .dynamicMode = DynamicModeType::TOUCH_ENABLED };
    screenSetting.strategy = settingStrategyName;

    strategyConfigs[strategyName0] = { .min = fps0, .max = fps0, .dynamicMode = DynamicModeType::TOUCH_ENABLED,
        .drawMin = OLED_NULL_HZ, .drawMax = OLED_NULL_HZ, .down = downFps0 };
    screenSetting.appList[pkgName0] = strategyName0;
    pkgParams_.push_back({ .pkgName = pkgName0, .fps = fps0, .pid = pid0,
        .linker = std::make_shared<RSRenderFrameRateLinker>(((NodeId)pid0) << nodeIdOffset) });

    strategyConfigs[strategyName1] = { .min = fps1, .max = fps1, .dynamicMode = DynamicModeType::TOUCH_ENABLED,
        .drawMin = OLED_NULL_HZ, .drawMax = OLED_NULL_HZ, .down = downFps1 };
    screenSetting.appList[pkgName1] = strategyName1;
    pkgParams_.push_back({ .pkgName = pkgName1, .fps = fps1, .pid = pid1,
        .linker = std::make_shared<RSRenderFrameRateLinker>(((NodeId)pid1) << nodeIdOffset) });

    for (auto &pkgParam : pkgParams_) {
        pkgParam.linker->SetExpectedRange(FrameRateRange(OLED_NULL_HZ, RANGE_MAX_REFRESHRATE, OLED_NULL_HZ));
    }

    appTypes[appTypeName0] = strategyName0;
    appTypes[appTypeName1] = strategyName1;
}

void HgmMultiAppStrategyTest::SetMultiAppStrategy(
    MultiAppStrategyType multiAppStrategyType, const std::string& strategyName)
{
    auto &screenSetting = multiAppStrategy_->GetScreenSetting();
    screenSetting.multiAppStrategyType = multiAppStrategyType;
    screenSetting.multiAppStrategyName = strategyName;
}

std::vector<std::string> HgmMultiAppStrategyTest::CreateVotePkgs()
{
    std::vector<std::string> pkgs;
    for (auto &pkgParam : pkgParams_) {
        pkgs.push_back(pkgParam.pkgName + ":" + std::to_string(pkgParam.pid));
    }
    return pkgs;
}

/**
 * @tc.name: SingleAppTouch001
 * @tc.desc: Verify the result of SingleAppTouch001 function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmMultiAppStrategyTest, SingleAppTouch001, Function | SmallTest | Level1)
{
    PART("CaseDescription") {
        auto &pkgParam = pkgParams_[0];
        std::vector<std::string> voteParam = { pkgParam.pkgName + ":" + std::to_string(pkgParam.pid), };

        PolicyConfigData::StrategyConfig strategyConfig;
        HgmErrCode res;
        SetMultiAppStrategy(MultiAppStrategyType::USE_MAX);

        STEP("1. pkg vote") {
            multiAppStrategy_->HandlePkgsEvent(voteParam);
            res = multiAppStrategy_->GetVoteRes(strategyConfig);
            ASSERT_EQ(res, EXEC_SUCCESS);
            ASSERT_EQ(strategyConfig.min, fps0);
            ASSERT_EQ(strategyConfig.max, fps0);
        }
        STEP("2. handle touch event") {
            multiAppStrategy_->HandleTouchInfo(pkgParam.pkgName, TouchState::DOWN_STATE);
            res = multiAppStrategy_->GetVoteRes(strategyConfig);
            ASSERT_EQ(res, EXEC_SUCCESS);
            ASSERT_EQ(strategyConfig.min, downFps0);
            ASSERT_EQ(strategyConfig.max, downFps0);

            multiAppStrategy_->HandleTouchInfo(pkgParam.pkgName, TouchState::UP_STATE);
            res = multiAppStrategy_->GetVoteRes(strategyConfig);
            ASSERT_EQ(res, EXEC_SUCCESS);
            ASSERT_EQ(strategyConfig.min, downFps0);
            ASSERT_EQ(strategyConfig.max, downFps0);

            multiAppStrategy_->HandleTouchInfo(pkgParam.pkgName, TouchState::IDLE_STATE);
            res = multiAppStrategy_->GetVoteRes(strategyConfig);
            ASSERT_EQ(res, EXEC_SUCCESS);
            ASSERT_EQ(strategyConfig.min, fps0);
            ASSERT_EQ(strategyConfig.max, fps0);
        }
    }
}

/**
 * @tc.name: SingleAppTouch002
 * @tc.desc: Verify the result of SingleAppTouch002 function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmMultiAppStrategyTest, SingleAppTouch002, Function | SmallTest | Level1)
{
    PART("CaseDescription") {
        std::string unConfigPkgName = "com.pkg.other";
        auto &pkgParam = pkgParams_[0];
        std::vector<std::string> voteParam = { pkgParam.pkgName + ":" + std::to_string(pkgParam.pid), };

        PolicyConfigData::StrategyConfig strategyConfig;
        HgmErrCode res;
        SetMultiAppStrategy(MultiAppStrategyType::USE_MAX);

        STEP("1. pkg vote") {
            multiAppStrategy_->HandlePkgsEvent(voteParam);
            res = multiAppStrategy_->GetVoteRes(strategyConfig);
            ASSERT_EQ(res, EXEC_SUCCESS);
            ASSERT_EQ(strategyConfig.min, fps0);
            ASSERT_EQ(strategyConfig.max, fps0);
        }
        STEP("2. click other pkg which hasn't config") {
            multiAppStrategy_->HandleTouchInfo(unConfigPkgName, TouchState::DOWN_STATE);
            res = multiAppStrategy_->GetVoteRes(strategyConfig);
            ASSERT_EQ(res, EXEC_SUCCESS);
            ASSERT_EQ(strategyConfig.min, OLED_120_HZ);
            ASSERT_EQ(strategyConfig.max, OLED_120_HZ);
            multiAppStrategy_->HandleTouchInfo(unConfigPkgName, TouchState::UP_STATE);
            multiAppStrategy_->HandleTouchInfo(unConfigPkgName, TouchState::IDLE_STATE);
            res = multiAppStrategy_->GetVoteRes(strategyConfig);
            ASSERT_EQ(res, EXEC_SUCCESS);
        }
        STEP("3. start other pkg which hasn't config") {
            res = multiAppStrategy_->HandlePkgsEvent({});
            ASSERT_EQ(res, EXEC_SUCCESS);
            multiAppStrategy_->HandlePkgsEvent({ unConfigPkgName, });
            ASSERT_EQ(res, EXEC_SUCCESS);
            res = multiAppStrategy_->GetVoteRes(strategyConfig);
            ASSERT_NE(res, EXEC_SUCCESS);
            ASSERT_EQ(strategyConfig.min, OLED_NULL_HZ);
            ASSERT_EQ(strategyConfig.max, OLED_120_HZ);

            multiAppStrategy_->HandleTouchInfo(unConfigPkgName, TouchState::DOWN_STATE);
            res = multiAppStrategy_->GetVoteRes(strategyConfig);
            ASSERT_NE(res, EXEC_SUCCESS);
            ASSERT_EQ(strategyConfig.min, OLED_120_HZ);
            ASSERT_EQ(strategyConfig.max, OLED_120_HZ);

            multiAppStrategy_->HandleTouchInfo(unConfigPkgName, TouchState::UP_STATE);
            multiAppStrategy_->HandleTouchInfo(unConfigPkgName, TouchState::IDLE_STATE);
            res = multiAppStrategy_->GetVoteRes(strategyConfig);
            ASSERT_NE(res, EXEC_SUCCESS);
            ASSERT_EQ(strategyConfig.min, OLED_NULL_HZ);
            ASSERT_EQ(strategyConfig.max, OLED_120_HZ);
        }
    }
}

/**
 * @tc.name: MultiAppTouch
 * @tc.desc: Verify the result of MultiAppTouch function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmMultiAppStrategyTest, MultiAppTouch, Function | SmallTest | Level1)
{
    PART("CaseDescription") {
        PolicyConfigData::StrategyConfig strategyConfig;
        SetMultiAppStrategy(MultiAppStrategyType::USE_MAX);
        STEP("1. pkg vote") {
            multiAppStrategy_->HandlePkgsEvent(CreateVotePkgs());
            multiAppStrategy_->GetVoteRes(strategyConfig);
            ASSERT_EQ(strategyConfig.min, fps1);
            ASSERT_EQ(strategyConfig.max, fps1);
        }
        STEP("2. handle pkg0 touch event") {
            multiAppStrategy_->HandleTouchInfo(pkgParams_[0].pkgName, TouchState::DOWN_STATE);
            multiAppStrategy_->GetVoteRes(strategyConfig);
            ASSERT_EQ(strategyConfig.min, downFps0);
            ASSERT_EQ(strategyConfig.max, downFps0);

            multiAppStrategy_->HandleTouchInfo(pkgParams_[0].pkgName, TouchState::UP_STATE);
            multiAppStrategy_->HandleTouchInfo(pkgParams_[0].pkgName, TouchState::IDLE_STATE);

            multiAppStrategy_->GetVoteRes(strategyConfig);
            ASSERT_EQ(strategyConfig.min, fps1);
            ASSERT_EQ(strategyConfig.max, fps1);
        }
        STEP("3. handle pkg1 touch event") {
            multiAppStrategy_->HandleTouchInfo(pkgParams_[1].pkgName, TouchState::DOWN_STATE);
            multiAppStrategy_->GetVoteRes(strategyConfig);
            ASSERT_EQ(strategyConfig.min, downFps1);
            ASSERT_EQ(strategyConfig.max, downFps1);

            multiAppStrategy_->HandleTouchInfo(pkgParams_[1].pkgName, TouchState::UP_STATE);
            multiAppStrategy_->HandleTouchInfo(pkgParams_[1].pkgName, TouchState::IDLE_STATE);

            multiAppStrategy_->GetVoteRes(strategyConfig);
            ASSERT_EQ(strategyConfig.min, fps1);
            ASSERT_EQ(strategyConfig.max, fps1);
        }
        STEP("4. handle empty pkg touch event") {
            multiAppStrategy_->HandleTouchInfo(otherPkgName, TouchState::DOWN_STATE);
            multiAppStrategy_->GetVoteRes(strategyConfig);
            ASSERT_EQ(strategyConfig.min, OLED_120_HZ);
            ASSERT_EQ(strategyConfig.max, OLED_120_HZ);

            multiAppStrategy_->HandleTouchInfo(otherPkgName, TouchState::UP_STATE);
            multiAppStrategy_->HandleTouchInfo(otherPkgName, TouchState::IDLE_STATE);

            multiAppStrategy_->GetVoteRes(strategyConfig);
            ASSERT_EQ(strategyConfig.min, fps1);
            ASSERT_EQ(strategyConfig.max, fps1);
        }
    }
}

/**
 * @tc.name: UseStrategyNum
 * @tc.desc: Verify the result of UseStrategyNum function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmMultiAppStrategyTest, UseStrategyNum, Function | SmallTest | Level1)
{
    PART("CaseDescription") {
        PolicyConfigData::StrategyConfig strategyConfig;
        SetMultiAppStrategy(MultiAppStrategyType::USE_STRATEGY_NUM, "110");
        STEP("1. handle 2 packages") {
            multiAppStrategy_->HandlePkgsEvent(CreateVotePkgs());
            multiAppStrategy_->GetVoteRes(strategyConfig);
            ASSERT_EQ(strategyConfig.min, fps0);
            ASSERT_EQ(strategyConfig.max, fps0);
        }
        STEP("2. empty pkgs") {
            multiAppStrategy_->HandlePkgsEvent({});
            multiAppStrategy_->GetVoteRes(strategyConfig);
            ASSERT_EQ(strategyConfig.min, fps0);
            ASSERT_EQ(strategyConfig.max, fps0);
        }
    }
}

/**
 * @tc.name: FollowFocus
 * @tc.desc: Verify the result of FollowFocus function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmMultiAppStrategyTest, FollowFocus, Function | SmallTest | Level1)
{
    PART("CaseDescription") {
        PolicyConfigData::StrategyConfig strategyConfig;
        SetMultiAppStrategy(MultiAppStrategyType::FOLLOW_FOCUS);
        STEP("1. handle 2 packages") {
            multiAppStrategy_->HandlePkgsEvent(CreateVotePkgs());
            multiAppStrategy_->GetVoteRes(strategyConfig);

            auto pkgParam = pkgParams_[0];
            ASSERT_EQ(strategyConfig.min, pkgParam.fps);
            ASSERT_EQ(strategyConfig.max, pkgParam.fps);
        }
        STEP("2. empty pkgs") {
            multiAppStrategy_->HandlePkgsEvent({});
            multiAppStrategy_->GetVoteRes(strategyConfig);
            ASSERT_EQ(strategyConfig.min, OledRefreshRate::OLED_NULL_HZ);
            ASSERT_EQ(strategyConfig.max, OledRefreshRate::OLED_120_HZ);
        }
    }
}

/**
 * @tc.name: UseMax
 * @tc.desc: Verify the result of UseMax function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmMultiAppStrategyTest, UseMax, Function | SmallTest | Level1)
{
    PART("CaseDescription") {
    PolicyConfigData::StrategyConfig strategyConfig;
    SetMultiAppStrategy(MultiAppStrategyType::USE_MAX);
        STEP("1. handle 2 packages") {
            multiAppStrategy_->HandlePkgsEvent(CreateVotePkgs());
            multiAppStrategy_->GetVoteRes(strategyConfig);
            ASSERT_EQ(strategyConfig.min, fps1);
            ASSERT_EQ(strategyConfig.max, fps1);
        }
        STEP("2. empty pkgs") {
            multiAppStrategy_->HandlePkgsEvent({});
            multiAppStrategy_->GetVoteRes(strategyConfig);
            ASSERT_EQ(strategyConfig.min, OledRefreshRate::OLED_NULL_HZ);
            ASSERT_EQ(strategyConfig.max, OledRefreshRate::OLED_120_HZ);
        }
    }
}

/**
 * @tc.name: AppType
 * @tc.desc: Verify the result of AppType
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmMultiAppStrategyTest, AppType, Function | SmallTest | Level1)
{
    PART("CaseDescription") {
        PolicyConfigData::StrategyConfig strategyConfig;
        multiAppStrategy_->HandlePkgsEvent({ otherPkgName + ":" + defaultPidStr + ":" + appTypeName0 });
        multiAppStrategy_->GetVoteRes(strategyConfig);
        ASSERT_EQ(strategyConfig.min, fps0);
        ASSERT_EQ(strategyConfig.max, fps0);
        
        multiAppStrategy_->HandlePkgsEvent({ otherPkgName + ":" + defaultPidStr + ":" + appTypeName1 });
        multiAppStrategy_->GetVoteRes(strategyConfig);
        ASSERT_EQ(strategyConfig.min, fps1);
        ASSERT_EQ(strategyConfig.max, fps1);
    }
}

/**
 * @tc.name: LightFactor
 * @tc.desc: Verify the result of LightFactor
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmMultiAppStrategyTest, LightFactor, Function | SmallTest | Level1)
{
    PART("CaseDescription") {
        PolicyConfigData::StrategyConfig strategyConfig;
        multiAppStrategy_->GetVoteRes(strategyConfig);
        ASSERT_EQ(strategyConfig.min, OledRefreshRate::OLED_NULL_HZ);
        ASSERT_EQ(strategyConfig.max, OledRefreshRate::OLED_120_HZ);

        multiAppStrategy_->HandleLightFactorStatus(true);
        multiAppStrategy_->GetVoteRes(strategyConfig);
        ASSERT_EQ(strategyConfig.min, OledRefreshRate::OLED_120_HZ);
        ASSERT_EQ(strategyConfig.max, OledRefreshRate::OLED_120_HZ);

        multiAppStrategy_->HandleLightFactorStatus(false);
        multiAppStrategy_->GetVoteRes(strategyConfig);
        ASSERT_EQ(strategyConfig.min, OledRefreshRate::OLED_NULL_HZ);
        ASSERT_EQ(strategyConfig.max, OledRefreshRate::OLED_120_HZ);
    }
}
} // namespace Rosen
} // namespace OHOS