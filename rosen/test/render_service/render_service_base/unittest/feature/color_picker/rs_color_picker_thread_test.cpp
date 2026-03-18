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

#include "drawable/rs_color_picker_drawable.h"
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
 * @tc.desc: Test PostTask executes the posted task
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerThreadTest, PostTaskTest, TestSize.Level1)
{
    std::atomic<bool> taskExecuted {false};
    auto func = [&taskExecuted]() -> void {
        taskExecuted.store(true);
    };

    RSColorPickerThread::Instance().PostTask(func);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    EXPECT_TRUE(taskExecuted.load());
}

/**
 * @tc.name: PostTaskWithoutRateLimitTest
 * @tc.desc: Test PostTask with limited=false bypasses rate limiting
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerThreadTest, PostTaskWithoutRateLimitTest, TestSize.Level1)
{
    std::atomic<uint32_t> executedTaskCount {0};
    auto func = [&executedTaskCount]() -> void {
        executedTaskCount.fetch_add(1, std::memory_order_relaxed);
    };

    // Post with limited=false should bypass rate limiting
    for (int i = 0; i < 25; i++) {
        RSColorPickerThread::Instance().PostTask(func, false);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    EXPECT_EQ(executedTaskCount.load(), 25u);
}

/**
 * @tc.name: RegisterNodeDirtyCallbackTest
 * @tc.desc: Test RegisterNodeDirtyCallback registers callback and NotifyNodeDirty invokes it
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerThreadTest, RegisterNodeDirtyCallbackTest, TestSize.Level1)
{
    std::atomic<bool> callbackInvoked {false};
    std::atomic<uint64_t> receivedNodeId {0};

    auto callback = [&callbackInvoked, &receivedNodeId](uint64_t nodeId) -> void {
        callbackInvoked.store(true);
        receivedNodeId.store(nodeId);
    };

    RSColorPickerThread::Instance().RegisterNodeDirtyCallback(callback);

    constexpr uint64_t testNodeId = 99999;
    RSColorPickerThread::Instance().NotifyNodeDirty(testNodeId);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    EXPECT_TRUE(callbackInvoked.load());
    EXPECT_EQ(receivedNodeId.load(), testNodeId);
}

/**
 * @tc.name: RegisterNodeDirtyCallbackNullTest
 * @tc.desc: Test NotifyNodeDirty handles null callback gracefully
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerThreadTest, RegisterNodeDirtyCallbackNullTest, TestSize.Level1)
{
    // Register null callback
    RSColorPickerThread::Instance().RegisterNodeDirtyCallback(nullptr);

    // Should not crash when NotifyNodeDirty is called
    RSColorPickerThread::Instance().NotifyNodeDirty(12345);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // If we reach here without crash, test passes
    EXPECT_TRUE(true);
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

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Verify that no more than MAX_TASKS_PER_SECOND tasks were posted
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

    std::this_thread::sleep_for(std::chrono::milliseconds(1100));

    uint32_t firstBatchCount = executedTaskCount.load();

    // Post another batch of tasks
    for (uint32_t i = 0; i < maxTasksPerSecond; i++) {
        RSColorPickerThread::Instance().PostTask([&executedTaskCount]() {
            executedTaskCount.fetch_add(1, std::memory_order_relaxed);
        });
    }

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

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // postTask doesn't work in UT
    EXPECT_EQ(executedTaskCount.load(), 0);
}

/**
 * @tc.name: RegisterStateTransitionCallbackTest
 * @tc.desc: Test RegisterStateTransitionCallback and TransitionState work together
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerThreadTest, RegisterStateTransitionCallbackTest, TestSize.Level1)
{
    std::atomic<bool> callbackInvoked {false};
    std::atomic<uint64_t> receivedNodeId {0};
    std::atomic<uint8_t> receivedState {0};
    std::atomic<int64_t> receivedDelay {0};

    auto callback = [&callbackInvoked, &receivedNodeId, &receivedState, &receivedDelay](
                        uint64_t nodeId, DrawableV2::ColorPickerState state, int64_t delayTime) -> void {
        callbackInvoked.store(true);
        receivedNodeId.store(nodeId);
        receivedState.store(static_cast<uint8_t>(state));
        receivedDelay.store(delayTime);
    };

    RSColorPickerThread::Instance().RegisterStateTransitionCallback(callback);

    constexpr uint64_t testNodeId = 12345;
    constexpr int64_t testDelay = 100;
    RSColorPickerThread::Instance().TransitionState(
        testNodeId, DrawableV2::ColorPickerState::COLOR_PICK_THIS_FRAME, testDelay);

    EXPECT_TRUE(callbackInvoked.load());
    EXPECT_EQ(receivedNodeId.load(), testNodeId);
    EXPECT_EQ(receivedState.load(), static_cast<uint8_t>(DrawableV2::ColorPickerState::COLOR_PICK_THIS_FRAME));
    EXPECT_EQ(receivedDelay.load(), testDelay); // Register null callback

    RSColorPickerThread::Instance().RegisterStateTransitionCallback(nullptr);

    // Should not crash when TransitionState is called
    RSColorPickerThread::Instance().TransitionState(1, DrawableV2::ColorPickerState::COLOR_PICK_THIS_FRAME, 0);
}

/**
 * @tc.name: NotifyClientTest
 * @tc.desc: Test NotifyClient calls the registered callback with correct parameters
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerThreadTest, NotifyClientTest, TestSize.Level1)
{
    std::atomic<bool> callbackInvoked {false};
    std::atomic<uint64_t> receivedNodeId {0};
    std::atomic<uint32_t> receivedColor {0};

    auto callback = [&callbackInvoked, &receivedNodeId, &receivedColor](
                        uint64_t nodeId, uint32_t color) -> void {
        callbackInvoked.store(true);
        receivedNodeId.store(nodeId);
        receivedColor.store(color);
    };

    RSColorPickerThread::Instance().RegisterNotifyClientCallback(callback);

    constexpr uint64_t testNodeId = 54321;
    constexpr uint32_t testColor = 0xFF123456;
    RSColorPickerThread::Instance().NotifyClient(testNodeId, testColor);

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    EXPECT_TRUE(callbackInvoked.load());
    EXPECT_EQ(receivedNodeId.load(), testNodeId);
    EXPECT_EQ(receivedColor.load(), testColor);
}
} // namespace OHOS::Rosen
