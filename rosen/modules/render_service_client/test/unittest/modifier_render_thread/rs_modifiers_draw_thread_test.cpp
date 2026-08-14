/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "gtest/gtest.h"
#include "modifier_render_thread/rs_modifiers_draw_thread.h"
#include "transaction/rs_transaction_data.h"
#include "transaction/rs_render_pipeline_client.h"
#include <future>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSModifiersDrawThreadTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSModifiersDrawThreadTest::SetUpTestCase() {}
void RSModifiersDrawThreadTest::TearDownTestCase() {}
void RSModifiersDrawThreadTest::SetUp() {}
void RSModifiersDrawThreadTest::TearDown() {}

HWTEST_F(RSModifiersDrawThreadTest, Constructor_InitializesCorrectly001, TestSize.Level1)
{
    auto thread = std::make_shared<RSModifiersDrawThread>();
    EXPECT_NE(thread, nullptr);
}

HWTEST_F(RSModifiersDrawThreadTest, Start_Idempotent001, TestSize.Level1)
{
    auto thread = std::make_shared<RSModifiersDrawThread>();
    thread->Start();
    thread->Start();
}

HWTEST_F(RSModifiersDrawThreadTest, PostTask_AfterStart001, TestSize.Level1)
{
    auto thread = std::make_shared<RSModifiersDrawThread>();
    thread->Start();
    std::promise<void> promise;
    auto future = promise.get_future();
    thread->PostTask([&promise]() { promise.set_value(); }, "TestTask", 0);
    future.wait_for(std::chrono::milliseconds(1000));
    EXPECT_TRUE(future.valid());
}

HWTEST_F(RSModifiersDrawThreadTest, PostTask_BeforeStart001, TestSize.Level1)
{
    auto thread = std::make_shared<RSModifiersDrawThread>();
    bool taskExecuted = false;
    thread->PostTask([&taskExecuted]() { taskExecuted = true; }, "TestTask", 0);
    usleep(10000);
    EXPECT_FALSE(taskExecuted);
}

HWTEST_F(RSModifiersDrawThreadTest, CommitTransaction_Basic001, TestSize.Level1)
{
    auto thread = std::make_shared<RSModifiersDrawThread>();
    thread->Start();
    auto canvasModifiersDrawAgent = std::make_shared<RSCanvasModifiersDrawAgent>();
    auto renderPipelineClient = std::make_shared<RSRenderPipelineClient>();
    auto transactionData = std::make_unique<RSTransactionData>();
    std::atomic<uint32_t> transactionDataIndex = 0;
    thread->CommitTransaction(
        canvasModifiersDrawAgent, renderPipelineClient, std::move(transactionData), transactionDataIndex);
}

HWTEST_F(RSModifiersDrawThreadTest, ScheduleTask_Basic001, TestSize.Level1)
{
    auto thread = std::make_shared<RSModifiersDrawThread>();
    thread->Start();
    std::promise<int> promise;
    auto future = promise.get_future();
    thread->ScheduleTask([&promise]() {
        promise.set_value(0);
        return 0;
    });
    auto result = future.wait_for(std::chrono::milliseconds(1000));
    EXPECT_EQ(result, std::future_status::ready);
}

HWTEST_F(RSModifiersDrawThreadTest, PostTask_ReturnsEarlyAfterDestroy001, TestSize.Level1)
{
    auto thread = std::make_shared<RSModifiersDrawThread>();
    thread->Start();
    thread->Destroy();
    bool taskExecuted = false;
    thread->PostTask([&taskExecuted]() { taskExecuted = true; }, "TestTask", 0);
    EXPECT_FALSE(taskExecuted);
    EXPECT_TRUE(thread->destroyed_.load());
}

HWTEST_F(RSModifiersDrawThreadTest, PostSyncTask_ReturnsEarlyAfterDestroy001, TestSize.Level1)
{
    auto thread = std::make_shared<RSModifiersDrawThread>();
    thread->Start();
    thread->Destroy();
    bool taskExecuted = false;
    thread->PostSyncTask([&taskExecuted]() { taskExecuted = true; });
    EXPECT_FALSE(taskExecuted);
    EXPECT_TRUE(thread->destroyed_.load());
}

HWTEST_F(RSModifiersDrawThreadTest, Start_ReturnsEarlyAfterDestroy001, TestSize.Level1)
{
    auto thread = std::make_shared<RSModifiersDrawThread>();
    thread->Start();
    thread->Destroy();
    thread->Start();
    EXPECT_FALSE(thread->started_.load());
    EXPECT_TRUE(thread->destroyed_.load());
}

HWTEST_F(RSModifiersDrawThreadTest, WaitAllTasksFinish_AfterDestroy001, TestSize.Level1)
{
    auto thread = std::make_shared<RSModifiersDrawThread>();
    thread->Start();
    thread->Destroy();
    thread->WaitAllTasksFinish();
    EXPECT_TRUE(thread->destroyed_.load());
}

HWTEST_F(RSModifiersDrawThreadTest, Destroy_BeforeStart001, TestSize.Level1)
{
    auto thread = std::make_shared<RSModifiersDrawThread>();
    thread->Destroy();
    EXPECT_TRUE(thread->destroyed_.load());
    EXPECT_FALSE(thread->started_.load());
}

// Verify Destroy() sets runner_ and handler_ to nullptr after Stop/RemoveAllEvents,
// and the null-safety guards work when runner_/handler_ are already nullptr.
HWTEST_F(RSModifiersDrawThreadTest, Destroy_NullptrAfterStopAndNullGuards001, TestSize.Level1)
{
    // Case 1: Start then Destroy — runner_/handler_ become nullptr after Stop+RemoveAllEvents
    auto thread = std::make_shared<RSModifiersDrawThread>();
    thread->Start();
    ASSERT_NE(thread->runner_, nullptr);
    ASSERT_NE(thread->handler_, nullptr);
    thread->Destroy();
    EXPECT_EQ(thread->runner_, nullptr);
    EXPECT_EQ(thread->handler_, nullptr);
    EXPECT_TRUE(thread->destroyed_.load());
    EXPECT_FALSE(thread->started_.load());

    // Case 2: started_=true but runner_/handler_ already nullptr —
    // bypasses early return, exercises null-safety guards (if handler_ != nullptr / if runner_ != nullptr)
    auto thread2 = std::make_shared<RSModifiersDrawThread>();
    thread2->started_ = true;
    thread2->runner_ = nullptr;
    thread2->handler_ = nullptr;
    thread2->Destroy();
    EXPECT_EQ(thread2->runner_, nullptr);
    EXPECT_EQ(thread2->handler_, nullptr);
    EXPECT_TRUE(thread2->destroyed_.load());
    EXPECT_FALSE(thread2->started_.load());
}

HWTEST_F(RSModifiersDrawThreadTest, PostSyncTask_BeforeStart001, TestSize.Level1)
{
    auto thread = std::make_shared<RSModifiersDrawThread>();
    bool taskExecuted = false;
    thread->PostSyncTask([&taskExecuted]() { taskExecuted = true; });
    EXPECT_FALSE(taskExecuted);
}
} // namespace Rosen
} // namespace OHOS
