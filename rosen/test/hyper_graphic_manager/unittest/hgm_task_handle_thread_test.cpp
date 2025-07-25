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
#include <thread>

#include "ffrt_inner.h"
#include "hgm_task_handle_thread.h"
#include "hgm_test_base.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class HgmTaskHandleThreadTest : public HgmTestBase {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HgmTaskHandleThreadTest::SetUpTestCase()
{
    HgmTestBase::SetUpTestCase();
}

void HgmTaskHandleThreadTest::TearDownTestCase()
{
    HgmTaskHandleThread::Instance().queue_ = std::make_shared<ffrt::queue>(
        static_cast<ffrt::queue_type>(ffrt_inner_queue_type_t::ffrt_queue_eventhandler_adapter), "HgmTaskHandleThread",
        ffrt::queue_attr().qos(ffrt::qos_user_interactive));
}

void HgmTaskHandleThreadTest::SetUp()
{
    if (!HgmTaskHandleThread::Instance().queue_) {
        HgmTaskHandleThread::Instance().queue_ = std::make_shared<ffrt::queue>(
            static_cast<ffrt::queue_type>(ffrt_inner_queue_type_t::ffrt_queue_eventhandler_adapter),
            "HgmTaskHandleThread", ffrt::queue_attr().qos(ffrt::qos_user_interactive));
    }
}

void HgmTaskHandleThreadTest::TearDown() {}

/**
 * @tc.name: Instance
 * @tc.desc: Test Instance
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmTaskHandleThreadTest, Instance, TestSize.Level0)
{
    HgmTaskHandleThread& instance1 = HgmTaskHandleThread::Instance();
    HgmTaskHandleThread& instance2 = HgmTaskHandleThread::Instance();
    EXPECT_EQ(&instance1, &instance2);
}

/**
 * @tc.name: PostTask001
 * @tc.desc: Test PostTask
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmTaskHandleThreadTest, PostTask001, TestSize.Level0)
{
    HgmTaskHandleThread& instance = HgmTaskHandleThread::Instance();
    std::function<void()> func = []() -> void {};
    int64_t delayTime = 100;

    EXPECT_NE(instance.queue_, nullptr);
    auto taskCount = instance.queue_->get_task_cnt();
    instance.PostTask(func, delayTime);
    EXPECT_EQ(instance.queue_->get_task_cnt(), taskCount + 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(delayTime));

    delayTime = -1;
    instance.PostTask(func, delayTime);

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    taskCount = instance.queue_->get_task_cnt();
    delayTime = UINT64_MAX / 100;
    instance.PostTask(func, delayTime);
    EXPECT_EQ(instance.queue_->get_task_cnt(), taskCount + 1);

    instance.queue_ = nullptr;
    instance.PostTask(func, delayTime);
}

/**
 * @tc.name: PostTask002
 * @tc.desc: Test PostTask
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmTaskHandleThreadTest, PostTask002, TestSize.Level0)
{
    HgmTaskHandleThread& instance = HgmTaskHandleThread::Instance();
    int count = 0;
    instance.PostTask([&count](){ count++; }, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_EQ(count, 1);
}

/**
 * @tc.name: PostTask003
 * @tc.desc: Test PostTask
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmTaskHandleThreadTest, PostTask003, TestSize.Level0)
{
    HgmTaskHandleThread& instance = HgmTaskHandleThread::Instance();
    int count = 0;
    const int count1 = 1;
    const int time = 500;
    const int time1 = 1000;
    auto start_time = std::chrono::high_resolution_clock::now();
    instance.PostTask([&count](){ count++; }, time1);
    std::this_thread::sleep_for(std::chrono::milliseconds(time));
    std::this_thread::sleep_for(std::chrono::milliseconds(time1));
    auto end_time = std::chrono::high_resolution_clock::now();
    EXPECT_EQ(count, count1);
    EXPECT_GE(std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count(), time1);
}

/**
 * @tc.name: PostSyncTask001
 * @tc.desc: Test PostSyncTask
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmTaskHandleThreadTest, PostSyncTask001, TestSize.Level0)
{
    HgmTaskHandleThread& instance = HgmTaskHandleThread::Instance();
    std::function<void()> func = []() -> void {};

    EXPECT_NE(instance.queue_, nullptr);
    bool res = instance.PostSyncTask(func);
    EXPECT_TRUE(res);

    instance.queue_ = nullptr;
    res = instance.PostSyncTask(func);
    EXPECT_FALSE(res);
}

/**
 * @tc.name: PostEvent001
 * @tc.desc: Test PostEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmTaskHandleThreadTest, PostEvent001, TestSize.Level0)
{
    HgmTaskHandleThread& instance = HgmTaskHandleThread::Instance();
    std::function<void()> func = []() -> void {};
    int64_t delayTime = 1000;
    std::string eventId = "eventId";

    EXPECT_NE(instance.queue_, nullptr);
    auto taskCount = instance.queue_->get_task_cnt();
    instance.PostEvent(eventId, func, delayTime);
    EXPECT_EQ(instance.queue_->get_task_cnt(), taskCount + 1);

    delayTime = -1;
    instance.PostEvent(eventId, func, delayTime);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    EXPECT_EQ(instance.queue_->get_task_cnt(), taskCount + 1);

    delayTime = UINT64_MAX / 100;
    instance.PostEvent(eventId, func, delayTime);
    EXPECT_EQ(instance.queue_->get_task_cnt(), taskCount + 2);

    instance.RemoveEvent(eventId);
    EXPECT_EQ(instance.queue_->get_task_cnt(), taskCount);

    instance.queue_ = nullptr;
    instance.PostEvent(eventId, func, delayTime);
}

/**
 * @tc.name: RemoveEvent001
 * @tc.desc: Test RemoveEvent
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmTaskHandleThreadTest, RemoveEvent001, TestSize.Level0)
{
    HgmTaskHandleThread& instance = HgmTaskHandleThread::Instance();
    std::function<void()> func = []() -> void {};
    int64_t delayTime = 5000;
    std::string eventId = "eventId";

    EXPECT_NE(instance.queue_, nullptr);
    auto taskCount = instance.queue_->get_task_cnt();
    instance.PostEvent(eventId, func, delayTime);
    EXPECT_EQ(instance.queue_->get_task_cnt(), taskCount + 1);
    instance.RemoveEvent(eventId);
    EXPECT_EQ(instance.queue_->get_task_cnt(), taskCount);

    instance.queue_ = nullptr;
    instance.RemoveEvent(eventId);
}

/**
 * @tc.name: DetectMultiThreadingCalls
 * @tc.desc: Test DetectMultiThreadingCalls
 * @tc.type: FUNC
 * @tc.require:IB3MVN
 */
HWTEST_F(HgmTaskHandleThreadTest, DetectMultiThreadingCalls, TestSize.Level0)
{
    HgmTaskHandleThread& instance = HgmTaskHandleThread::Instance();
    instance.DetectMultiThreadingCalls();
    int32_t curThreadId = instance.curThreadId_;
    instance.DetectMultiThreadingCalls();
    EXPECT_EQ(curThreadId, instance.curThreadId_);
    std::thread([&instance]() { instance.DetectMultiThreadingCalls(); }).join();
    EXPECT_NE(curThreadId, instance.curThreadId_);
}
} // namespace Rosen
} // namespace OHOS
