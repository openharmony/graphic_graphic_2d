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
    constexpr int32_t waitTaskFinishNs = 100000;
}
class HgmTouchManagerTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

/**
 * @tc.name: ChangeState
 * @tc.desc: Verify the result of ChangeState function
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmTouchManagerTest, ChangeState, Function | SmallTest | Level1)
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
HWTEST_F(HgmTouchManagerTest, Up2IdleState, Function | SmallTest | Level1)
{
    PART("CaseDescription") {
        auto touchManager = HgmTouchManager();
        const int32_t rsTimeoutUs = 610000;
        const int32_t handleRsFrameTimeUs = 510000;
        const int32_t handleRsFrameNum = 5;

        STEP("1. 600ms timeout") {
            touchManager.ChangeState(TouchState::DOWN_STATE);
            touchManager.ChangeState(TouchState::UP_STATE);
            usleep(waitTaskFinishNs);
            ASSERT_EQ(touchManager.GetState(), TouchState::UP_STATE);

            usleep(rsTimeoutUs);
            ASSERT_EQ(touchManager.GetState(), TouchState::IDLE_STATE);
        }
        STEP("2. 3s timeout") {
            touchManager.ChangeState(TouchState::DOWN_STATE);
            touchManager.ChangeState(TouchState::UP_STATE);
            usleep(waitTaskFinishNs);
            ASSERT_EQ(touchManager.GetState(), TouchState::UP_STATE);

            for (int i = 0; i < handleRsFrameNum; i++) {
                touchManager.HandleRsFrame();
                usleep(handleRsFrameTimeUs);
                ASSERT_EQ(touchManager.GetState(), TouchState::UP_STATE);
            }
            usleep(handleRsFrameTimeUs);
            ASSERT_EQ(touchManager.GetState(), TouchState::IDLE_STATE);
        }
    }
}
} // namespace Rosen
} // namespace OHOS