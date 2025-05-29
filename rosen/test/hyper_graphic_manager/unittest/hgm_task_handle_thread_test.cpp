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
void HgmTaskHandleThreadTest::TearDownTestCase() {}
void HgmTaskHandleThreadTest::SetUp() {}
void HgmTaskHandleThreadTest::TearDown() {}


/*
 * @tc.name: Instance
 * @tc.desc: Test Instance
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmTaskHandleThreadTest, Instance, TestSize.Level1)
{
    HgmTaskHandleThread& instance1 = HgmTaskHandleThread::Instance();
    HgmTaskHandleThread& instance2 = HgmTaskHandleThread::Instance();
    EXPECT_EQ(&instance1, &instance2);
}

/*
 * @tc.name: PostTask001
 * @tc.desc: Test PostTask
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmTaskHandleThreadTest, PostTask001, TestSize.Level1)
{
    std::function<void()> func = []() -> void {};
    HgmTaskHandleThread::Instance().PostTask(func);
    int64_t delayTime = 1;
    HgmTaskHandleThread::Instance().PostTask(func, delayTime);
    EXPECT_TRUE(HgmTaskHandleThread::Instance().handler_);
}

/*
 * @tc.name: PostTask002
 * @tc.desc: Test PostTask
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmTaskHandleThreadTest, PostTask002, TestSize.Level1)
{
    HgmTaskHandleThread& instance = HgmTaskHandleThread::Instance();
    int count = 0;
    instance.PostTask([&count](){ count++; }, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_EQ(count, 1);
}

/*
 * @tc.name: PostTask003
 * @tc.desc: Test PostTask
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(HgmTaskHandleThreadTest, PostTask003, TestSize.Level1)
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

/*
 * @tc.name: DetectMultiThreadingCalls
 * @tc.desc: Test DetectMultiThreadingCalls
 * @tc.type: FUNC
 * @tc.require:IB3MVN
 */
HWTEST_F(HgmTaskHandleThreadTest, DetectMultiThreadingCalls, TestSize.Level1)
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
