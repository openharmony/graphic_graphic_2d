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
#include <test_header.h>

#include "hgm_test_base.h"
#include "hgm_one_shot_timer.cpp"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class HgmSimpleTimerTest : public HgmTestBase {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp();
    void TearDown() {}
};

void HgmSimpleTimerTest::SetUp()
{
    if (!HgmTaskHandleThread::Instance().queue_) {
        HgmTaskHandleThread::Instance().queue_ =
            std::make_shared<ffrt::queue>("HgmTaskHandleThread", ffrt::queue_attr().qos(ffrt::qos_user_interactive));
    }
}

/**
 * @tc.name  : HgmSimpleTimerTest
 * @tc.number: Start_001
 * @tc.desc  : Test Start
 */
HWTEST_F(HgmSimpleTimerTest, Start_001, Function | SmallTest | Level1)
{
    HgmTaskHandleThread& instance = HgmTaskHandleThread::Instance();
    int32_t interval = 200;
    HgmSimpleTimer timer("test_timer", std::chrono::milliseconds(interval), nullptr, nullptr);

    EXPECT_NE(instance.queue_, nullptr);
    auto taskCount = instance.queue_->get_task_cnt();
    timer.Start();
    EXPECT_EQ(instance.queue_->get_task_cnt(), taskCount + 1);
    timer.Stop();
    EXPECT_EQ(instance.queue_->get_task_cnt(), taskCount);

    instance.queue_ = nullptr;
    timer.Start();
}

/**
 * @tc.name  : HgmSimpleTimerTest
 * @tc.number: Stop_001
 * @tc.desc  : Test Stop
 */
HWTEST_F(HgmSimpleTimerTest, Stop_001, Function | SmallTest | Level1)
{
    HgmTaskHandleThread& instance = HgmTaskHandleThread::Instance();
    int32_t interval = 200;
    HgmSimpleTimer timer("test_timer", std::chrono::milliseconds(interval), nullptr, nullptr);

    EXPECT_NE(instance.queue_, nullptr);
    auto taskCount = instance.queue_->get_task_cnt();
    timer.running_ = false;
    timer.Start();
    EXPECT_EQ(instance.queue_->get_task_cnt(), taskCount + 1);
    timer.running_ = false;
    timer.Stop();
    EXPECT_EQ(instance.queue_->get_task_cnt(), taskCount + 1);

    timer.running_ = true;
    timer.Stop();
    EXPECT_EQ(instance.queue_->get_task_cnt(), taskCount);

    instance.queue_ = nullptr;
    timer.running_ = false;
    timer.Stop();

    timer.running_ = true;
    timer.Stop();
}

/**
 * @tc.name  : HgmSimpleTimerTest
 * @tc.number: Loop_001
 * @tc.desc  : Test Loop
 */
HWTEST_F(HgmSimpleTimerTest, Loop_001, Function | SmallTest | Level1)
{
    HgmTaskHandleThread& instance = HgmTaskHandleThread::Instance();
    int32_t interval = 200;
    HgmSimpleTimer timer("test_timer", std::chrono::milliseconds(interval), nullptr, nullptr);

    timer.running_ = true;
    timer.Reset();
    timer.Loop();
    EXPECT_TRUE(timer.running_);

    std::this_thread::sleep_for(std::chrono::milliseconds(interval * 2));
    timer.Loop();
    EXPECT_FALSE(timer.running_);
}

/**
 * @tc.name  : HgmSimpleTimerTest
 * @tc.number: Loop_002
 * @tc.desc  : Test Loop
 */
HWTEST_F(HgmSimpleTimerTest, Loop_002, Function | SmallTest | Level1)
{
    HgmTaskHandleThread& instance = HgmTaskHandleThread::Instance();
    int32_t interval = 200;
    HgmSimpleTimer timer("test_timer", std::chrono::milliseconds(interval), nullptr, nullptr);

    EXPECT_NE(instance.queue_, nullptr);
    auto taskCount = instance.queue_->get_task_cnt();
    timer.running_ = true;
    timer.Reset();
    timer.Loop();
    EXPECT_EQ(instance.queue_->get_task_cnt(), taskCount + 1);

    timer.running_ = false;
    timer.Loop();
    EXPECT_EQ(instance.queue_->get_task_cnt(), taskCount + 1);
    timer.Stop();

    instance.queue_ = nullptr;
    timer.running_ = true;
    timer.Reset();
    timer.Loop();

    timer.running_ = false;
    timer.Loop();
}
} // namespace Rosen
} // namespace OHOS