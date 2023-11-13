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

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
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
 * @tc.name: UniProcessData
 * @tc.desc: Verify the result of UniProcessData function
 * @tc.type: FUNC
 * @tc.require: I7DMS1
 */
HWTEST_F(HgmFrameRateMgrTest, UniProcessData, Function | SmallTest | Level1)
{
    std::unique_ptr<HgmFrameRateManager> mgr = std::make_unique<HgmFrameRateManager>();
    ScreenId id = 8;
    std::shared_ptr<RSRenderFrameRateLinker> rsFrameRateLinker;
    FrameRateLinkerMap appFrameLinkers;
    uint64_t timestamp = 10000000;
    bool flag = false;
    PART("CaseDescription") {
        STEP("1. get a HgmFrameRateManager") {
            STEP_ASSERT_NE(mgr, nullptr);
        }
        STEP("2. check the result of UniProcessData") {
            mgr->UniProcessData(id, timestamp, rsFrameRateLinker, appFrameLinkers, flag);
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
        STEP("1. insert and start screenTimer") {
            mgr->StartScreenTimer(id, interval, nullptr, nullptr);
            auto timer = mgr->GetScreenTimer(id);
            STEP_ASSERT_NE(timer, nullptr);
        }
        STEP("2. reset screenTimer") {
            mgr->ResetScreenTimer(id);
            auto timer = mgr->GetScreenTimer(id);
            STEP_ASSERT_NE(timer, nullptr);
        }
    }
}
} // namespace Rosen
} // namespace OHOS