/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>

#include "platform/common/rs_hisysevent.h"
#include <thread>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSHiSysEventTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSHiSysEventTest::SetUpTestCase() {}
void RSHiSysEventTest::TearDownTestCase() {}
void RSHiSysEventTest::SetUp() {}
void RSHiSysEventTest::TearDown() {}

/**
 * @tc.name: RSHiSysEventTest001
 * @tc.desc: Verify function EventWrite
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSHiSysEventTest, RSHiSysEventTest001, TestSize.Level1)
{
    string eventName = "RSHISYSEVENT_TEST";
    string msg = "RSHiSysEvent test 1";
    int id = 1;
    int ret = RSHiSysEvent::EventWrite(eventName, RSEventType::RS_STATISTIC, "MSG", msg, "ID", id);
    ASSERT_EQ(ret, 0);
}

/**
 * @tc.name: RSHiSysEventTest002
 * @tc.desc: Verify function EventWrite
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSHiSysEventTest, RSHiSysEventTest002, TestSize.Level1)
{
    string eventName = "RSHISYSEVENT_TEST";
    string msg1 = "RSHiSysEvent test msg1";
    string msg2 = "RSHiSysEvent test msg2";
    int id = 1;

    int ret1;
    int ret2;
    std::thread t1([&ret1, eventName, msg1, id]() {
        ret1 = RSHiSysEvent::EventWrite(eventName, RSEventType::RS_STATISTIC, "MSG", msg1, "ID", id);
    });
    std::thread t2([&ret2, eventName, msg2, id]() {
        ret2 = RSHiSysEvent::EventWrite(eventName, RSEventType::RS_STATISTIC, "MSG", msg2, "ID", id);
    });
    t1.join();
    t2.join();
    ASSERT_EQ(ret1, 0);
    ASSERT_EQ(ret2, 0);
}

/**
 * @tc.name: RSHiSysEventTest003
 * @tc.desc: Verify function EventWrite
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSHiSysEventTest, RSHiSysEventTest003, TestSize.Level1)
{
    string eventName = "0_RSHISYSEVENT_TEST"; // invalid
    string msg = "RSHiSysEvent test 3";
    int id = 1;
    int ret = RSHiSysEvent::EventWrite(eventName, RSEventType::RS_STATISTIC, "MSG", msg, "ID", id);
    ASSERT_EQ(ret, -2);
}
} // namespace Rosen
} // namespace OHOS