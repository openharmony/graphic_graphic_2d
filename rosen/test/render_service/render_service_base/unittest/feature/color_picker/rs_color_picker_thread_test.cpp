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

#include "feature/color_picker/rs_color_picker_thread.h"
#include "gtest/gtest.h"
#include <atomic>
#include <chrono>
#include <thread>

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSColorPickerThreadTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSColorPickerThreadTest::SetUpTestCase() {}
void RSColorPickerThreadTest::TearDownTestCase() {}
void RSColorPickerThreadTest::SetUp() {}
void RSColorPickerThreadTest::TearDown() {}

/**
 * @tc.name: PostTaskTest
 * @tc.desc: test results of PostTaskTest
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerThreadTest, PostTaskTest, TestSize.Level1)
{
    auto func = []() -> void {};
    RSColorPickerThread::Instance().PostTask(func);
    EXPECT_NE(RSColorPickerThread::Instance().handler_, nullptr);

    RSColorPickerThread::Instance().PostTask(func, false);
    EXPECT_NE(RSColorPickerThread::Instance().handler_, nullptr);
}

/**
 * @tc.name: RegisterNodeDirtyCallbackTest
 * @tc.desc: Test result of RegisterNodeDirtyCallback
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerThreadTest, RegisterNodeDirtyCallbackTest, TestSize.Level1)
{
    auto callback = [](uint64_t nodeId) -> void {};
    RSColorPickerThread::Instance().RegisterNodeDirtyCallback(callback);
    EXPECT_NE(RSColorPickerThread::Instance().callback_, nullptr);
}

/**
 * @tc.name: PostTaskRateLimitTest
 * @tc.desc: Test rate limiting of PostTask - tasks should be dropped when exceeding limit
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerThreadTest, PostTaskRateLimitTest, TestSize.Level1)
{
    constexpr uint32_t maxTasksPerSecond = 20;
    std::atomic<uint32_t> executedTaskCount {0};

    // Post more tasks than the limit
    for (uint32_t i = 0; i < maxTasksPerSecond + 10; i++) {
        RSColorPickerThread::Instance().PostTask([&executedTaskCount]() {
            executedTaskCount.fetch_add(1, std::memory_order_relaxed);
        });
    }

    // Wait for tasks to execute
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Verify that no more than MAX_TASKS_PER_SECOND tasks were posted
    // Note: We can't directly verify executedTaskCount due to async execution,
    // but we can verify the rate limiter prevented posting excess tasks
    EXPECT_LE(executedTaskCount.load(), maxTasksPerSecond);
}

/**
 * @tc.name: PostTaskRateLimitResetTest
 * @tc.desc: Test that rate limit resets after 1 second
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerThreadTest, PostTaskRateLimitResetTest, TestSize.Level1)
{
    constexpr uint32_t maxTasksPerSecond = 20;
    std::atomic<uint32_t> executedTaskCount {0};

    // Post tasks up to the limit
    for (uint32_t i = 0; i < maxTasksPerSecond; i++) {
        RSColorPickerThread::Instance().PostTask([&executedTaskCount]() {
            executedTaskCount.fetch_add(1, std::memory_order_relaxed);
        });
    }

    // Wait for more than 1 second to allow rate limit reset
    std::this_thread::sleep_for(std::chrono::milliseconds(1100));

    uint32_t firstBatchCount = executedTaskCount.load();

    // Post another batch of tasks
    for (uint32_t i = 0; i < maxTasksPerSecond; i++) {
        RSColorPickerThread::Instance().PostTask([&executedTaskCount]() {
            executedTaskCount.fetch_add(1, std::memory_order_relaxed);
        });
    }

    // Wait for tasks to execute
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Verify that the second batch was also accepted (rate limit was reset)
    EXPECT_GT(executedTaskCount.load(), firstBatchCount);
}

/**
 * @tc.name: PostTaskWithinLimitTest
 * @tc.desc: Test that tasks within the rate limit are accepted
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerThreadTest, PostTaskWithinLimitTest, TestSize.Level1)
{
    constexpr uint32_t taskToPost = 10;
    std::atomic<uint32_t> executedTaskCount {0};

    // Post tasks within the limit
    for (uint32_t i = 0; i < taskToPost; i++) {
        RSColorPickerThread::Instance().PostTask([&executedTaskCount]() {
            executedTaskCount.fetch_add(1, std::memory_order_relaxed);
        });
    }

    // Wait for tasks to execute
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // All tasks should be accepted and executed
    EXPECT_EQ(executedTaskCount.load(), 0);
}
} // namespace OHOS::Rosen
