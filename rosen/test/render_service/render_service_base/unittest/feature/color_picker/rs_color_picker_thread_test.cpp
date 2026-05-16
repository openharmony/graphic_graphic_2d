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

    RSColorPickerThread::Instance().PostTask(func, 0);
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
        RSColorPickerThread::Instance().PostTask(func, 0);
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
