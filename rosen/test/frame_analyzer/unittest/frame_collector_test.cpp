/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include <test_header.h>

#include "frame_collector.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class FrameCollectorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void FrameCollectorTest::SetUpTestCase() {}
void FrameCollectorTest::TearDownTestCase() {}
void FrameCollectorTest::SetUp() {}
void FrameCollectorTest::TearDown() {}

/*
 * Function: Singleton is single
 * Type: Function
 * EnvConditions: N/A
 * CaseDescription: 1. call GetInstance twice
 *                  2. check instance should be same
 */
HWTEST_F(FrameCollectorTest, Singleton, Function | SmallTest | Level4)
{
    PART("CaseDescription") {
        STEP("1. call GetInstance twice") {
            auto &instance1 = FrameCollector::GetInstance();
            auto &instance2 = FrameCollector::GetInstance();
            STEP("2. check instance should be same") {
                STEP_ASSERT_EQ(&instance1, &instance2);
            }
        }
    }
}

/*
 * Function: IsEnabled/SetEnabled
 * Type: Function
 * EnvConditions: N/A
 * CaseDescription: 1. save IsEnabled as e
 *                  2. SetEnabled !e
 *                  3. check IsEnabled is !e
 */
HWTEST_F(FrameCollectorTest, Enable, Function | SmallTest | Level2)
{
    PART("CaseDescription") {
        bool e = false;
        STEP("1. save IsEnabled as e") {
            e = FrameCollector::GetInstance().IsEnabled();
        }

        STEP("2. SetEnabled !e") {
            FrameCollector::GetInstance().SetEnabled(!e);
        }

        STEP("3. save IsEnabled as e") {
            STEP_ASSERT_EQ(FrameCollector::GetInstance().IsEnabled(), !e);
        }
    }
}

/*
 * Function: MarkFrameEvent
 * Type: Function
 * EnvConditions: Enable and ClearEvents FrameCollector
 * CaseDescription: 1. check queue size 0
 *                  2. mark UIMarks
 *                  3. check queue size 0
 *                  4. mark WaitVsyncStart
 *                  5. check queue size 1
 *                  6. mark WaitVsyncStart
 *                  7. check queue size 2
 *                  8. clear events
 *                  9. check queue size 0
 */
HWTEST_F(FrameCollectorTest, MarkFrameEvent, Function | MediumTest | Level2)
{
    auto &collector = FrameCollector::GetInstance();
    PART("EnvConditions") {
        STEP("Enable and ClearEvents FrameCollector") {
            collector.SetEnabled(true);
            collector.ClearEvents();
        }
    }

    PART("CaseDescription") {
        STEP("1. check queue size 0") {
            STEP_ASSERT_EQ(collector.LockGetFrameQueue().GetSize(), 0);
            collector.UnlockFrameQueue();
        }

        STEP("2. mark UIMarks") {
            collector.MarkFrameEvent(FrameEventType::UploadStart);
            collector.MarkFrameEvent(FrameEventType::UploadEnd);
            collector.MarkFrameEvent(FrameEventType::AnimateStart);
            collector.MarkFrameEvent(FrameEventType::AnimateEnd);
            collector.MarkFrameEvent(FrameEventType::LayoutStart);
            collector.MarkFrameEvent(FrameEventType::LayoutEnd);
            collector.MarkFrameEvent(FrameEventType::DrawStart);
            collector.MarkFrameEvent(FrameEventType::DrawEnd);
        }

        STEP("3. check queue size 0") {
            STEP_ASSERT_EQ(collector.LockGetFrameQueue().GetSize(), 0);
            collector.UnlockFrameQueue();
        }

        STEP("4. mark WaitVsyncStart") {
            collector.MarkFrameEvent(FrameEventType::WaitVsyncStart);
        }

        STEP("5. check queue size 1") {
            STEP_ASSERT_EQ(collector.LockGetFrameQueue().GetSize(), 1);
            collector.UnlockFrameQueue();
        }

        STEP("6. mark WaitVsyncStart") {
            collector.MarkFrameEvent(FrameEventType::WaitVsyncStart);
        }

        STEP("7. check queue size 2") {
            STEP_ASSERT_EQ(collector.LockGetFrameQueue().GetSize(), 2);
            collector.UnlockFrameQueue();
        }

        STEP("8. clear events") {
            collector.ClearEvents();
        }

        STEP("9. check queue size 0") {
            STEP_ASSERT_EQ(collector.LockGetFrameQueue().GetSize(), 0);
            collector.UnlockFrameQueue();
        }
    }
}
} // namespace Rosen
} // namespace OHOS
