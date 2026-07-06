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
    bool taskExecuted = false;
    thread->PostTask([&taskExecuted]() { taskExecuted = true; }, "TestTask", 0);
    usleep(10000);
    EXPECT_TRUE(taskExecuted);
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
    uint32_t transactionDataIndex = 0;
    thread->CommitTransaction(
        canvasModifiersDrawAgent, renderPipelineClient, std::move(transactionData), transactionDataIndex);
}

HWTEST_F(RSModifiersDrawThreadTest, ScheduleTask_Basic001, TestSize.Level1)
{
    auto thread = std::make_shared<RSModifiersDrawThread>();
    thread->Start();
    bool taskExecuted = false;
    auto future = thread->ScheduleTask([&taskExecuted]() {
        taskExecuted = true;
        return 0;
    });
    usleep(10000);
    EXPECT_TRUE(taskExecuted);
}
} // namespace Rosen
} // namespace OHOS
