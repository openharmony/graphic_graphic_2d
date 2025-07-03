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

#include "hgm_frame_rate_manager.h"
#include "hgm_test_base.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr int32_t waitTaskFinishNs = 100000;
}

class HgmTouchManagerTest : public HgmTestBase {
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
 * @tc.name: QuickTouch
 * @tc.desc: Verify the result of QuickTouch function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmTouchManagerTest, QuickTouch, Function | SmallTest | Level0)
{
    int32_t clickNum = 100;
    auto touchManager = HgmTouchManager();
    for (int i = 0; i < clickNum; i++) {
        touchManager.ChangeState(TouchState::DOWN_STATE);
        touchManager.ChangeState(TouchState::UP_STATE);
    }
    touchManager.ChangeState(TouchState::IDLE_STATE);
    sleep(1); // wait for 1s for the async task to complete
    ASSERT_EQ(touchManager.GetState(), TouchState::IDLE_STATE);
}

/**
 * @tc.name: ChangeState
 * @tc.desc: Verify the result of ChangeState function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmTouchManagerTest, ChangeState, Function | SmallTest | Level0)
{
    PART("CaseDescription") {
        auto touchManager = HgmTouchManager();
        ASSERT_EQ(touchManager.GetState(), TouchState::IDLE_STATE);

        touchManager.ChangeState(TouchState::UP_STATE);
        ASSERT_EQ(touchManager.GetState(), TouchState::IDLE_STATE);

        touchManager.ChangeState(TouchState::DOWN_STATE);
        usleep(waitTaskFinishNs);
        ASSERT_EQ(touchManager.GetState(), TouchState::DOWN_STATE);

        touchManager.ChangeState(TouchState::IDLE_STATE);
        ASSERT_EQ(touchManager.GetState(), TouchState::DOWN_STATE);

        touchManager.ChangeState(TouchState::UP_STATE);
        usleep(waitTaskFinishNs);
        ASSERT_EQ(touchManager.GetState(), TouchState::UP_STATE);

        touchManager.ChangeState(TouchState::DOWN_STATE);
        usleep(waitTaskFinishNs);
        ASSERT_EQ(touchManager.GetState(), TouchState::DOWN_STATE);

        touchManager.ChangeState(TouchState::UP_STATE);
        usleep(waitTaskFinishNs);
        ASSERT_EQ(touchManager.GetState(), TouchState::UP_STATE);

        touchManager.ChangeState(TouchState::IDLE_STATE);
        usleep(waitTaskFinishNs);
        ASSERT_EQ(touchManager.GetState(), TouchState::IDLE_STATE);
    }
}

/**
 * @tc.name: Up2IdleState
 * @tc.desc: Verify the result of Up2IdleState function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmTouchManagerTest, Up2IdleState, Function | SmallTest | Level0)
{
    auto touchManager = HgmTouchManager();
    const int32_t rsTimeoutUs = 610000;
    const int32_t handleRsFrameTimeUs = 150000;
    const int32_t handleRsFrameNum = 20;
    const int32_t skipTimeUs = 2600000;
    const int32_t upOffsetTimeUs = 400000;

    // 1. 600ms timeout
    touchManager.ChangeState(TouchState::DOWN_STATE);
    touchManager.ChangeState(TouchState::UP_STATE);
    usleep(waitTaskFinishNs);
    ASSERT_EQ(touchManager.GetState(), TouchState::UP_STATE);

    usleep(rsTimeoutUs);
    ASSERT_EQ(touchManager.GetState(), TouchState::IDLE_STATE);

    // 2. 3s timeout
    touchManager.ChangeState(TouchState::DOWN_STATE);
    touchManager.ChangeState(TouchState::UP_STATE);
    auto start  = std::chrono::steady_clock::now();
    usleep(waitTaskFinishNs);
    ASSERT_EQ(touchManager.GetState(), TouchState::UP_STATE);

    for (int i = 0; i < handleRsFrameNum; i++) {
        touchManager.HandleRsFrame();
        usleep(handleRsFrameTimeUs);
        auto end  = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() > skipTimeUs) {
            break;
        }
        ASSERT_EQ(touchManager.GetState(), TouchState::UP_STATE);
    }
    touchManager.HandleRsFrame();
    usleep(upOffsetTimeUs);
    touchManager.HandleRsFrame();
    usleep(upOffsetTimeUs);
    ASSERT_EQ(touchManager.GetState(), TouchState::IDLE_STATE);
}

/**
 * @tc.name: Up2IdleState001
 * @tc.desc: Verify the result of Up2IdleState001 function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmTouchManagerTest, Up2IdleState001, Function | SmallTest | Level0)
{
    PART("CaseDescription") {
        auto touchManager = HgmTouchManager();
        const int32_t rsTimeoutUs = 610000;
        const int32_t handleRsFrameTimeUs = 510000;
        const int32_t handleRsFrameNum = 5;
        const TouchState undefinedState = static_cast<TouchState>(100);

        STEP("3s timeout") {
            touchManager.ChangeState(TouchState::DOWN_STATE);
            touchManager.ChangeState(TouchState::UP_STATE);
            usleep(waitTaskFinishNs);
            ASSERT_EQ(touchManager.GetState(), TouchState::UP_STATE);

            touchManager.OnEvent(TouchEvent::UP_TIMEOUT_EVENT);
            usleep(rsTimeoutUs);
            ASSERT_EQ(touchManager.GetState(), TouchState::IDLE_STATE);
            touchManager.OnEvent(TouchEvent::RS_IDLE_TIMEOUT_EVENT);
            usleep(rsTimeoutUs);
            ASSERT_EQ(touchManager.GetState(), TouchState::IDLE_STATE);
        }
        STEP("State2String") {
            touchManager.State2String(undefinedState);
        }
        STEP("CheckChangeStateValid") {
            touchManager.CheckChangeStateValid(TouchState::IDLE_STATE, TouchState::UP_STATE);
            touchManager.CheckChangeStateValid(TouchState::IDLE_STATE, undefinedState);
            touchManager.CheckChangeStateValid(undefinedState, TouchState::IDLE_STATE);
        }
        STEP("ExecuteCallback") {
            touchManager.ExecuteCallback(nullptr);
            touchManager.ExecuteCallback([]() { usleep(1); });
            touchManager.ExecuteCallback(nullptr);
        }
    }
    sleep(1); // wait for task finished
}
} // namespace Rosen
} // namespace OHOS