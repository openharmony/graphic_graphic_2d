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
#include "modifier_render_thread/rs_modifiers_draw.h"
#include "modifier_render_thread/rs_canvas_modifiers_draw_agent.h"
#include "transaction/rs_render_pipeline_client.h"
#include "transaction/rs_transaction_data.h"
#include "transaction/rs_interfaces.h"
#include "transaction/rs_render_interface.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSModifiersDrawTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSModifiersDrawTest::SetUpTestCase() {}
void RSModifiersDrawTest::TearDownTestCase() {}
void RSModifiersDrawTest::SetUp() {}
void RSModifiersDrawTest::TearDown() {}

/**
 * @tc.name: Constructor_DefaultConstructor001
 * @tc.desc: Test RSModifiersDraw constructor starts thread automatically
 * @tc.type: FUNC
 */
HWTEST_F(RSModifiersDrawTest, Constructor_DefaultConstructor001, TestSize.Level1)
{
    RSModifiersDraw modifiersDraw;
    EXPECT_TRUE(modifiersDraw.started_.load());
    EXPECT_FALSE(modifiersDraw.destroyed_.load());
}

/**
 * @tc.name: Destroy_AfterStart001
 * @tc.desc: Test Destroy stops thread and sets destroyed_ flag
 * @tc.type: FUNC
 */
HWTEST_F(RSModifiersDrawTest, Destroy_AfterStart001, TestSize.Level1)
{
    RSModifiersDraw modifiersDraw;
    modifiersDraw.Destroy();
    EXPECT_TRUE(modifiersDraw.destroyed_.load());
    EXPECT_FALSE(modifiersDraw.started_.load());
}

/**
 * @tc.name: Destroy_BeforeStart001
 * @tc.desc: Test Destroy when started_ is false (early return, handler_/runner_ untouched)
 * @tc.type: FUNC
 */
HWTEST_F(RSModifiersDrawTest, Destroy_BeforeStart001, TestSize.Level1)
{
    RSModifiersDraw modifiersDraw;
    modifiersDraw.started_ = false;
    auto savedHandler = modifiersDraw.handler_;
    auto savedRunner = modifiersDraw.runner_;
    modifiersDraw.Destroy();
    EXPECT_TRUE(modifiersDraw.destroyed_.load());
    EXPECT_FALSE(modifiersDraw.started_.load());
}

/**
 * @tc.name: Destroy_NullHandlerAndRunnerWithStarted001
 * @tc.desc: Test Destroy when started_ is true but handler_/runner_ are nullptr (null-safety guards)
 * @tc.type: FUNC
 */
HWTEST_F(RSModifiersDrawTest, Destroy_NullHandlerAndRunnerWithStarted001, TestSize.Level1)
{
    RSModifiersDraw modifiersDraw;
    ASSERT_TRUE(modifiersDraw.started_.load());
    modifiersDraw.handler_ = nullptr;
    modifiersDraw.runner_ = nullptr;
    modifiersDraw.Destroy();
    EXPECT_TRUE(modifiersDraw.destroyed_.load());
    EXPECT_FALSE(modifiersDraw.started_.load());
    EXPECT_EQ(modifiersDraw.handler_, nullptr);
    EXPECT_EQ(modifiersDraw.runner_, nullptr);
}

/**
 * @tc.name: PostTask_ReturnsEarlyAfterDestroy001
 * @tc.desc: Test PostTask returns early when started_ is false
 * @tc.type: FUNC
 */
HWTEST_F(RSModifiersDrawTest, PostTask_ReturnsEarlyAfterDestroy001, TestSize.Level1)
{
    RSModifiersDraw modifiersDraw;
    modifiersDraw.Destroy();
    bool taskExecuted = false;
    modifiersDraw.PostTask([&taskExecuted]() { taskExecuted = true; }, "TestTask", 0);
    EXPECT_FALSE(taskExecuted);
}

/**
 * @tc.name: PostSyncTask_ReturnsEarlyAfterDestroy001
 * @tc.desc: Test PostSyncTask returns early when started_ is false
 * @tc.type: FUNC
 */
HWTEST_F(RSModifiersDrawTest, PostSyncTask_ReturnsEarlyAfterDestroy001, TestSize.Level1)
{
    RSModifiersDraw modifiersDraw;
    modifiersDraw.Destroy();
    bool taskExecuted = false;
    modifiersDraw.PostSyncTask([&taskExecuted]() { taskExecuted = true; });
    EXPECT_FALSE(taskExecuted);
}

/**
 * @tc.name: StartThread_ReturnsEarlyAfterDestroy001
 * @tc.desc: Test StartThread returns early when destroyed_ is true
 * @tc.type: FUNC
 */
HWTEST_F(RSModifiersDrawTest, StartThread_ReturnsEarlyAfterDestroy001, TestSize.Level1)
{
    RSModifiersDraw modifiersDraw;
    modifiersDraw.Destroy();
    modifiersDraw.started_ = false;
    modifiersDraw.StartThread();
    EXPECT_TRUE(modifiersDraw.destroyed_.load());
    EXPECT_FALSE(modifiersDraw.started_.load());
}

/**
 * @tc.name: StartThread_AlreadyStarted001
 * @tc.desc: Test StartThread returns early when started_ is already true
 * @tc.type: FUNC
 */
HWTEST_F(RSModifiersDrawTest, StartThread_AlreadyStarted001, TestSize.Level1)
{
    RSModifiersDraw modifiersDraw;
    ASSERT_TRUE(modifiersDraw.started_.load());
    auto savedHandler = modifiersDraw.handler_;
    auto savedRunner = modifiersDraw.runner_;
    modifiersDraw.StartThread();
    usleep(100000);
    EXPECT_TRUE(modifiersDraw.started_.load());
}

/**
 * @tc.name: PostTask_HandlerNullWithStarted001
 * @tc.desc: Test PostTask returns early when started_ is true but handler_ is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSModifiersDrawTest, PostTask_HandlerNullWithStarted001, TestSize.Level1)
{
    RSModifiersDraw modifiersDraw;
    ASSERT_TRUE(modifiersDraw.started_.load());
    modifiersDraw.handler_ = nullptr;
    bool taskExecuted = false;
    modifiersDraw.PostTask([&taskExecuted]() { taskExecuted = true; }, "TestTask", 0);
    EXPECT_FALSE(taskExecuted);
}

/**
 * @tc.name: PostSyncTask_HandlerNullWithStarted001
 * @tc.desc: Test PostSyncTask returns early when started_ is true but handler_ is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSModifiersDrawTest, PostSyncTask_HandlerNullWithStarted001, TestSize.Level1)
{
    RSModifiersDraw modifiersDraw;
    ASSERT_TRUE(modifiersDraw.started_.load());
    modifiersDraw.handler_ = nullptr;
    bool taskExecuted = false;
    modifiersDraw.PostSyncTask([&taskExecuted]() { taskExecuted = true; });
    EXPECT_FALSE(taskExecuted);
}

/**
 * @tc.name: CommitTransaction_NullCanvasModifiersDrawAgent001
 * @tc.desc: Test CommitTransaction with null canvasModifiersDrawAgent
 * @tc.type: FUNC
 */
HWTEST_F(RSModifiersDrawTest, CommitTransaction_NullCanvasModifiersDrawAgent001, TestSize.Level1)
{
    RSModifiersDraw modifiersDraw;
    auto renderPipelineClient = std::make_shared<RSRenderPipelineClient>();
    auto transactionData = std::make_unique<RSTransactionData>();
    std::atomic<uint32_t> transactionDataIndex = 0;
    modifiersDraw.CommitTransaction(nullptr, renderPipelineClient, transactionData, transactionDataIndex);
    EXPECT_EQ(transactionDataIndex.load(), transactionData->GetIndex());
}

/**
 * @tc.name: CommitTransaction_WithCanvasModifiersDrawAgent001
 * @tc.desc: Test CommitTransaction with non-null canvasModifiersDrawAgent and empty transactionConfigList
 * @tc.type: FUNC
 */
HWTEST_F(RSModifiersDrawTest, CommitTransaction_WithCanvasModifiersDrawAgent001, TestSize.Level1)
{
    RSModifiersDraw modifiersDraw;
    auto screenId = RSInterfaces::GetInstance().GetDefaultScreenId();
    auto connectToRender = RSInterfaces::GetInstance().GetConnectToRenderToken(screenId);
    ASSERT_NE(connectToRender, nullptr);
    auto renderInterface = std::make_shared<RSRenderInterface>(connectToRender);
    auto canvasModifiersDrawAgent = std::make_shared<RSCanvasModifiersDrawAgent>(renderInterface);
    ASSERT_NE(canvasModifiersDrawAgent, nullptr);
    auto renderPipelineClient = std::make_shared<RSRenderPipelineClient>();
    auto transactionData = std::make_unique<RSTransactionData>();
    std::atomic<uint32_t> transactionDataIndex = 0;
    modifiersDraw.CommitTransaction(canvasModifiersDrawAgent, renderPipelineClient, transactionData,
        transactionDataIndex);
    EXPECT_EQ(transactionDataIndex.load(), transactionData->GetIndex());
}

/**
 * @tc.name: CommitTransaction_WithNonEmptyConfigList001
 * @tc.desc: Test CommitTransaction with non-empty transactionConfigList so SyncAndWait returns non-null command
 * @tc.type: FUNC
 */
HWTEST_F(RSModifiersDrawTest, CommitTransaction_WithNonEmptyConfigList001, TestSize.Level1)
{
    RSModifiersDraw modifiersDraw;
    auto screenId = RSInterfaces::GetInstance().GetDefaultScreenId();
    auto connectToRender = RSInterfaces::GetInstance().GetConnectToRenderToken(screenId);
    ASSERT_NE(connectToRender, nullptr);
    auto renderInterface = std::make_shared<RSRenderInterface>(connectToRender);
    auto canvasModifiersDrawAgent = std::make_shared<RSCanvasModifiersDrawAgent>(renderInterface);
    ASSERT_NE(canvasModifiersDrawAgent, nullptr);
    ASSERT_NE(canvasModifiersDrawAgent->canvasModifiersDraw_, nullptr);
    canvasModifiersDrawAgent->canvasModifiersDraw_->StartThread();
    // Push a config so SyncAndWait returns a non-null TransactionBufferCommand
    RSTransactionConfig config;
    config.nodeId = 12345;
    config.hasBuffer = false;
    canvasModifiersDrawAgent->canvasModifiersDraw_->transactionConfigList_.push_back(config);
    auto renderPipelineClient = std::make_shared<RSRenderPipelineClient>();
    auto transactionData = std::make_unique<RSTransactionData>();
    std::atomic<uint32_t> transactionDataIndex = 0;
    modifiersDraw.CommitTransaction(canvasModifiersDrawAgent, renderPipelineClient, transactionData,
        transactionDataIndex);
    EXPECT_EQ(transactionDataIndex.load(), transactionData->GetIndex());
    canvasModifiersDrawAgent->canvasModifiersDraw_->Destroy();
}
} // namespace Rosen
} // namespace OHOS
