/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "hgm_core.h"
#include "hgm_frame_rate_manager.h"
#include "hgm_config_callback_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    int32_t width = 720;
    int32_t height = 1080;
    int32_t phyWidth = 685;
    int32_t phyHeight = 1218;
    ScreenSize screenSize = {width, height, phyWidth, phyHeight};
}
class HgmFrameRateMgrTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HgmFrameRateMgrTest::SetUpTestCase() {}
void HgmFrameRateMgrTest::TearDownTestCase() {}
void HgmFrameRateMgrTest::SetUp() {}
void HgmFrameRateMgrTest::TearDown() {}

/**
 * @tc.name: UniProcessDataForLtpo
 * @tc.desc: Verify the result of UniProcessDataForLtpo function
 * @tc.type: FUNC
 * @tc.require: I7DMS1
 */
HWTEST_F(HgmFrameRateMgrTest, UniProcessDataForLtpo, Function | SmallTest | Level1)
{
    auto &instance = HgmCore::Instance();
    ScreenId id = 8;
    sptr<HgmScreen> screen = nullptr;
    int32_t width = 1344;
    int32_t height = 2772;
    uint32_t rate = 120;
    uint32_t rate2 = 60;
    uint32_t rate3 = 90;
    int32_t mode = 1;
    int32_t mode2 = 2;
    int32_t mode3 = 3;
    instance.AddScreen(id, 1, screenSize);
    instance.AddScreenInfo(id, width, height, rate, mode);
    instance.AddScreenInfo(id, width, height, rate2, mode2);
    instance.AddScreenInfo(id, width, height, rate3, mode3);
    instance.SetActiveScreenId(id);
    std::shared_ptr<RSRenderFrameRateLinker> rsFrameRateLinker = std::make_shared<RSRenderFrameRateLinker>();
    ASSERT_NE(rsFrameRateLinker, nullptr);
    rsFrameRateLinker->SetExpectedRange({0, 120, 60});

    std::shared_ptr<RSRenderFrameRateLinker> appFrameLinker1 = std::make_shared<RSRenderFrameRateLinker>();
    ASSERT_NE(appFrameLinker1, nullptr);
    appFrameLinker1->SetExpectedRange({0, 120, 90});
    std::shared_ptr<RSRenderFrameRateLinker> appFrameLinker2 = std::make_shared<RSRenderFrameRateLinker>();
    ASSERT_NE(appFrameLinker2, nullptr);
    appFrameLinker2->SetExpectedRange({0, 120, 120});
    std::unordered_map<FrameRateLinkerId, std::shared_ptr<RSRenderFrameRateLinker>> appFrameLinkers =
        {{1, appFrameLinker1}, {2, appFrameLinker2}};

    uint64_t timestamp = 10000000;
    bool flag = false;
    pid_t pid = 0;
    sptr<VSyncGenerator> vsyncGenerator = CreateVSyncGenerator();
    sptr<VSyncController> rsController = new VSyncController(vsyncGenerator, 0);
    sptr<VSyncController> appController = new VSyncController(vsyncGenerator, 0);
    std::shared_ptr<HgmVSyncGeneratorController> controller =
        std::make_shared<HgmVSyncGeneratorController>(rsController, appController, vsyncGenerator);
    ASSERT_NE(controller, nullptr);
    std::unique_ptr<HgmFrameRateManager> frameRateMgr = std::make_unique<HgmFrameRateManager>();
    PART("CaseDescription") {
        STEP("1. get a HgmFrameRateManager and check the result of UniProcessDataForLtpo") {
            ASSERT_NE(frameRateMgr, nullptr);
            HgmConfigCallbackManager::GetInstance()->RegisterHgmRefreshRateModeChangeCallback(pid, nullptr);
            frameRateMgr->Init(rsController, appController, vsyncGenerator);
            frameRateMgr->SetForceUpdateCallback([](bool idleTimerExpired, bool forceUpdate) {});
            frameRateMgr->touchMgr_->SetRSIdleUpdateCallback([](bool rsIdleTimerExpired) {});
            frameRateMgr->UniProcessDataForLtpo(timestamp, rsFrameRateLinker, appFrameLinkers, flag, false);
        }
    }
}

/**
 * @tc.name: UniProcessDataForLtps
 * @tc.desc: Verify the result of UniProcessDataForLtps function
 * @tc.type: FUNC
 * @tc.require: I7DMS1
 */
HWTEST_F(HgmFrameRateMgrTest, UniProcessDataForLtps, Function | SmallTest | Level1)
{
    bool flag = false;
    std::unique_ptr<HgmFrameRateManager> frameRateMgr = std::make_unique<HgmFrameRateManager>();
    PART("CaseDescription") {
        STEP("1. check the result of UniProcessDataForLtps") {
            frameRateMgr->UniProcessDataForLtps(flag);
        }
    }
}

/**
 * @tc.name: HgmOneShotTimerTest
 * @tc.desc: Verify the result of HgmOneShotTimerTest
 * @tc.type: FUNC
 * @tc.require: I7DMS1
 */
HWTEST_F(HgmFrameRateMgrTest, HgmOneShotTimerTest, Function | SmallTest | Level2)
{
    std::unique_ptr<HgmFrameRateManager> mgr = std::make_unique<HgmFrameRateManager>();
    ScreenId id = 1;
    int32_t interval = 200; // 200ms means waiting time

    PART("CaseDescription") {
        STEP("1. set force update callback") {
            mgr->SetForceUpdateCallback([](bool idleTimerExpired, bool forceUpdate) {});
        }
        STEP("2. insert and start screenTimer") {
            mgr->StartScreenTimer(id, interval, nullptr, nullptr);
            auto timer = mgr->GetScreenTimer(id);
            STEP_ASSERT_NE(timer, nullptr);
        }
        STEP("3. reset screenTimer") {
            mgr->ResetScreenTimer(id);
            auto timer = mgr->GetScreenTimer(id);
            STEP_ASSERT_NE(timer, nullptr);
        }
    }
}
} // namespace Rosen
} // namespace OHOS