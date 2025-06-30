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

#include "hgm_frame_rate_manager.h"
#include "hgm_test_base.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr int32_t waitTaskFinishNs = 100000;
constexpr pid_t appPid = 0;
constexpr uint32_t touchCount = 1;
constexpr uint32_t delay_60Ms = 60;
constexpr uint32_t delay_110Ms = 110;
}

class HgmPointerManagerTest : public HgmTestBase {
public:
    static void SetUpTestCase()
    {
        HgmTestBase::SetUpTestCase();
    }
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
    HgmErrCode getPointerVoteInfo(VoteInfo& pointerVoteInfo);
};

/**
 * @tc.name: QuickClick
 * @tc.desc: Verify the result of QuickClick function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmPointerManagerTest, QuickClick, Function | SmallTest | Level1)
{
    int32_t clickNum = 100;
    auto pointerManager = HgmPointerManager();
    for (int i = 0; i < clickNum; i++) {
        pointerManager.ChangeState(PointerState::POINTER_ACTIVE_STATE);
        pointerManager.ChangeState(PointerState::POINTER_IDLE_STATE);
    }
    pointerManager.ChangeState(PointerState::POINTER_IDLE_STATE);
    sleep(1); // wait for 1s for the async task to complete
    ASSERT_EQ(pointerManager.GetState(), PointerState::POINTER_IDLE_STATE);
}

/**
 * @tc.name: ChangeState
 * @tc.desc: Verify the result of ChangeState function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmPointerManagerTest, ChangeState, Function | SmallTest | Level1)
{
    PART("CaseDescription") {
        auto pointerManager = HgmPointerManager();
        ASSERT_EQ(pointerManager.GetState(), PointerState::POINTER_IDLE_STATE);

        pointerManager.ChangeState(PointerState::POINTER_ACTIVE_STATE);
        usleep(waitTaskFinishNs);
        ASSERT_EQ(pointerManager.GetState(), PointerState::POINTER_ACTIVE_STATE);

        pointerManager.ChangeState(PointerState::POINTER_IDLE_STATE);
        usleep(waitTaskFinishNs);
        ASSERT_EQ(pointerManager.GetState(), PointerState::POINTER_IDLE_STATE);
    }
}

/**
 * @tc.name: Active2IdleState
 * @tc.desc: Verify the result of Active2IdleState function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmPointerManagerTest, Active2IdleState, Function | SmallTest | Level1)
{
    PART("CaseDescription") {
        auto pointerManager = HgmPointerManager();
        const int32_t handleRsFrameUs = 1100000;
        const int32_t handleRsFrameNum = 2;
        const PointerState undefinedState = static_cast<PointerState>(100);

        STEP("1. 3000ms timeout") {
            pointerManager.ChangeState(PointerState::POINTER_ACTIVE_STATE);
            usleep(waitTaskFinishNs);
            ASSERT_EQ(pointerManager.GetState(), PointerState::POINTER_ACTIVE_STATE);

            for (int i = 0; i < handleRsFrameNum; i++) {
                pointerManager.HandleRsFrame();
                usleep(handleRsFrameUs);
                ASSERT_EQ(pointerManager.GetState(), PointerState::POINTER_ACTIVE_STATE);
            }
            usleep(handleRsFrameUs);
            ASSERT_EQ(pointerManager.GetState(), PointerState::POINTER_IDLE_STATE);
        }
        STEP("State2String") {
            pointerManager.State2String(undefinedState);
        }
        STEP("CheckChangeStateValid") {
            pointerManager.CheckChangeStateValid(PointerState::POINTER_IDLE_STATE, undefinedState);
            pointerManager.CheckChangeStateValid(undefinedState, PointerState::POINTER_IDLE_STATE);
        }
        STEP("ExecuteCallback") {
            pointerManager.ExecuteCallback(nullptr);
            pointerManager.ExecuteCallback([]() { usleep(1); });
            pointerManager.ExecuteCallback(nullptr);
        }
    }
}

/**
 * @tc.name: HandleTimerResetEvent
 * @tc.desc: Verify the result of HandleTimerResetEvent function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmPointerManagerTest, HandleTimerResetEvent, Function | SmallTest | Level1)
{
    PART("CaseDescription") {
        auto pointerManager = HgmPointerManager();
        const int32_t activeTimeoutUs = 1300000;
        const int32_t handleTimerResetTime = 1000000;

        STEP("HandleMoveTimeOut") {
            pointerManager.ChangeState(PointerState::POINTER_ACTIVE_STATE);
            usleep(waitTaskFinishNs);
            ASSERT_EQ(pointerManager.GetState(), PointerState::POINTER_ACTIVE_STATE);
            usleep(handleTimerResetTime - waitTaskFinishNs);
            pointerManager.HandleTimerReset();
            usleep(handleTimerResetTime);
            ASSERT_EQ(pointerManager.GetState(), PointerState::POINTER_ACTIVE_STATE);
            pointerManager.HandleTimerReset();
            usleep(handleTimerResetTime);
            ASSERT_EQ(pointerManager.GetState(), PointerState::POINTER_ACTIVE_STATE);
            pointerManager.HandleTimerReset();
            usleep(activeTimeoutUs);
            ASSERT_EQ(pointerManager.GetState(), PointerState::POINTER_IDLE_STATE);
        }
    }
}

/**
 * @tc.name: HgmSetPointerActiveFPS
 * @tc.desc: Verify the result of HgmSetPointerActiveFPS function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmPointerManagerTest, HgmSetPointerActiveFPS, Function | SmallTest | Level1)
{
    HgmFrameRateManager frameRateMgr;
    constexpr uint32_t delay_1100Ms = 1100;
    constexpr uint32_t delay_1300Ms = 1300;
    frameRateMgr.Init(nullptr, nullptr, nullptr, nullptr);
    frameRateMgr.HandleTouchEvent(appPid, TouchStatus::TOUCH_BUTTON_DOWN, touchCount);
    std::this_thread::sleep_for(std::chrono::milliseconds(delay_110Ms));
    ASSERT_EQ(frameRateMgr.pointerManager_.GetState(), PointerState::POINTER_ACTIVE_STATE);

    frameRateMgr.HandleTouchEvent(appPid, TouchStatus::TOUCH_MOVE, touchCount);
    std::this_thread::sleep_for(std::chrono::milliseconds(delay_110Ms));
    ASSERT_EQ(frameRateMgr.pointerManager_.GetState(), PointerState::POINTER_ACTIVE_STATE);

    frameRateMgr.HandleTouchEvent(appPid, TouchStatus::TOUCH_BUTTON_UP, touchCount);
    std::this_thread::sleep_for(std::chrono::milliseconds(delay_110Ms));
    ASSERT_EQ(frameRateMgr.pointerManager_.GetState(), PointerState::POINTER_ACTIVE_STATE);

    frameRateMgr.HandleTouchEvent(appPid, TouchStatus::TOUCH_MOVE, touchCount);
    std::this_thread::sleep_for(std::chrono::milliseconds(delay_1100Ms));
    ASSERT_EQ(frameRateMgr.pointerManager_.GetState(), PointerState::POINTER_ACTIVE_STATE);

    std::this_thread::sleep_for(std::chrono::milliseconds(delay_1300Ms));
    ASSERT_EQ(frameRateMgr.pointerManager_.GetState(), PointerState::POINTER_IDLE_STATE);

    frameRateMgr.pointerManager_.ChangeState(PointerState::POINTER_IDLE_STATE);
    sleep(1); // wait for handler task finished
}

/**
 * @tc.name: HgmSetAxisActiveFPS
 * @tc.desc: Verify the result of HgmSetAxisActiveFPS function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmPointerManagerTest, HgmSetAxisActiveFPS, Function | SmallTest | Level1)
{
    HgmFrameRateManager frameRateMgr;
    constexpr uint32_t delay_1100Ms = 1100;
    constexpr uint32_t delay_200Ms = 200;
    frameRateMgr.Init(nullptr, nullptr, nullptr, nullptr);
    frameRateMgr.HandleTouchEvent(appPid, TouchStatus::AXIS_BEGIN, touchCount);
    std::this_thread::sleep_for(std::chrono::milliseconds(delay_110Ms));
    ASSERT_EQ(frameRateMgr.pointerManager_.GetState(), PointerState::POINTER_ACTIVE_STATE);
 
    frameRateMgr.HandleTouchEvent(appPid, TouchStatus::AXIS_UPDATE, touchCount);
    std::this_thread::sleep_for(std::chrono::milliseconds(delay_110Ms));
    ASSERT_EQ(frameRateMgr.pointerManager_.GetState(), PointerState::POINTER_ACTIVE_STATE);
 
    frameRateMgr.HandleTouchEvent(appPid, TouchStatus::AXIS_END, touchCount);
    std::this_thread::sleep_for(std::chrono::milliseconds(delay_110Ms));
    ASSERT_EQ(frameRateMgr.pointerManager_.GetState(), PointerState::POINTER_ACTIVE_STATE);
 
    frameRateMgr.HandleTouchEvent(appPid, TouchStatus::AXIS_UPDATE, touchCount);
    std::this_thread::sleep_for(std::chrono::milliseconds(delay_1100Ms));
    ASSERT_EQ(frameRateMgr.pointerManager_.GetState(), PointerState::POINTER_ACTIVE_STATE);
 
    std::this_thread::sleep_for(std::chrono::milliseconds(delay_200Ms));
    ASSERT_EQ(frameRateMgr.pointerManager_.GetState(), PointerState::POINTER_IDLE_STATE);
 
    frameRateMgr.pointerManager_.ChangeState(PointerState::POINTER_IDLE_STATE);
    sleep(1); // wait for handler task finished
}

} // namespace Rosen
} // namespace OHOS