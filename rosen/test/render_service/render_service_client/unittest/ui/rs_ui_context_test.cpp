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

#include "gtest/gtest.h"

#include "transaction/rs_transaction.h"

#include "ui/rs_canvas_node.h"
#include "ui/rs_ui_context.h"
#include "ui/rs_ui_context_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSUIContextTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUIContextTest::SetUpTestCase() {}
void RSUIContextTest::TearDownTestCase() {}
void RSUIContextTest::SetUp() {}
void RSUIContextTest::TearDown() {}

/**
 * @tc.name: PostDelayTaskTest001
 * @tc.desc: Test the case where the task runner is empty
 * @tc.type:FUNC
 */
HWTEST_F(RSUIContextTest, PostDelayTaskTest001, TestSize.Level1)
{
    auto uiContext1 = RSUIContextManager::MutableInstance().CreateRSUIContext();
    auto uiContext2 = RSUIContextManager::MutableInstance().CreateRSUIContext();
    ASSERT_TRUE(RSUIContextManager::Instance().rsUIContextMap_.size() > 1);
    bool flag = false;
    const std::function<void()>& task = [&flag]() { flag = true; };
    uint32_t delay = 0;
    uiContext1->PostDelayTask(task, delay);
    ASSERT_FALSE(flag);
}

/**
 * @tc.name: PostDelayTaskTest002
 * @tc.desc: Test the case where the random task runner isn't empty
 * @tc.type:FUNC
 */
HWTEST_F(RSUIContextTest, PostDelayTaskTest002, TestSize.Level1)
{
    auto uiContext1 = RSUIContextManager::MutableInstance().CreateRSUIContext();
    auto uiContext2 = RSUIContextManager::MutableInstance().CreateRSUIContext();
    ASSERT_TRUE(RSUIContextManager::Instance().rsUIContextMap_.size() > 1);
    uiContext2->SetUITaskRunner([](const std::function<void()>& task, uint32_t delay) { task(); });
    ASSERT_FALSE(uiContext1->HasTaskRunner());
    ASSERT_TRUE(uiContext2->HasTaskRunner());
    bool flag = false;
    const std::function<void()>& task = [&flag]() { flag = true; };
    uint32_t delay = 0;
    uiContext1->PostDelayTask(task, delay);
    ASSERT_TRUE(flag);
    uiContext2->rsTransactionHandler_ = nullptr;
    uiContext2->SetUITaskRunner([](const std::function<void()>& task, uint32_t delay) { task(); });
}

/**
 * @tc.name: DumpNodeTreeProcessorTest001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIContextTest, DumpNodeTreeProcessorTest001, TestSize.Level1)
{
    auto uiContext = RSUIContextManager::MutableInstance().CreateRSUIContext();
    auto transaction = uiContext->GetRSTransaction();
    ASSERT_NE(transaction, nullptr);
    auto canvasNode = RSCanvasNode::Create(false, false, uiContext);
    std::string out = "";
    uiContext->DumpNodeTreeProcessor(canvasNode->GetId(), 0, 0, out);
    ASSERT_TRUE(out.find("transactionFlags") != std::string::npos);
    ASSERT_TRUE(out.find("UIContext") != std::string::npos);
}

/**
 * @tc.name: DumpNodeTreeProcessorTest002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIContextTest, DumpNodeTreeProcessorTest002, TestSize.Level1)
{
    auto uiContext = RSUIContextManager::MutableInstance().CreateRSUIContext();
    auto transaction = uiContext->GetRSTransaction();
    ASSERT_NE(transaction, nullptr);
    std::string out = "";
    uiContext->DumpNodeTreeProcessor(0, 0, 0, out);
    ASSERT_TRUE(out.find("transactionFlags") == std::string::npos);
    ASSERT_TRUE(out.find("UIContext") == std::string::npos);
}

/**
 * @tc.name: DumpNodeTreeProcessorTest003
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIContextTest, DumpNodeTreeProcessorTest003, TestSize.Level1)
{
    bool enable = RSSystemProperties::GetRSClientMultiInstanceEnabled();
    if (enable) {
        auto uiContext = RSUIContextManager::MutableInstance().CreateRSUIContext();
        uiContext->rsTransactionHandler_ = nullptr;
        auto transaction = uiContext->GetRSTransaction();
        ASSERT_EQ(transaction, nullptr);
        auto canvasNode = RSCanvasNode::Create(false, false, uiContext);
        std::string out = "";
        uiContext->DumpNodeTreeProcessor(canvasNode->GetId(), 0, 0, out);
        ASSERT_TRUE(out.find("transactionFlags") == std::string::npos);
        ASSERT_TRUE(out.find("UIContext") == std::string::npos);
    }
}

/**
 * @tc.name: StartCloseSyncTransactionFallbackTaskTest001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIContextTest, StartCloseSyncTransactionFallbackTaskTest001, TestSize.Level1)
{
    const uint64_t syncId = 123;
    std::shared_ptr<OHOS::AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create("runner");
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> handler = std::make_shared<AppExecFwk::EventHandler>(runner);
    RSUIContextManager::MutableInstance().handler_ = handler;

    ASSERT_TRUE(RSUIContextManager::MutableInstance().taskNames_.empty());
    RSUIContextManager::MutableInstance().StartCloseSyncTransactionFallbackTask(true, syncId);
    ASSERT_FALSE(RSUIContextManager::MutableInstance().taskNames_.empty());
    RSUIContextManager::MutableInstance().StartCloseSyncTransactionFallbackTask(false, syncId);
    ASSERT_TRUE(RSUIContextManager::MutableInstance().taskNames_.empty());

    ASSERT_TRUE(RSUIContextManager::MutableInstance().taskNames_.empty());
    RSUIContextManager::MutableInstance().StartCloseSyncTransactionFallbackTask(true, syncId);
    ASSERT_FALSE(RSUIContextManager::MutableInstance().taskNames_.empty());
    std::queue<std::string>().swap(RSUIContextManager::MutableInstance().taskNames_);
    RSUIContextManager::MutableInstance().StartCloseSyncTransactionFallbackTask(false, syncId);

    RSUIContextManager::MutableInstance().StartCloseSyncTransactionFallbackTask(true, syncId);
    ASSERT_FALSE(RSUIContextManager::MutableInstance().taskNames_.empty());
    sleep(8);

    RSUIContextManager::MutableInstance().StartCloseSyncTransactionFallbackTask(true, syncId);
    std::queue<std::string>().swap(RSUIContextManager::MutableInstance().taskNames_);
    ASSERT_TRUE(RSUIContextManager::MutableInstance().taskNames_.empty());
    sleep(8);
}

/**
 * @tc.name: CloseAllSyncTransactionTest001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSUIContextTest, CloseAllSyncTransactionTest001, TestSize.Level1)
{
    const uint64_t syncId = 123;

    RSUIContextManager::MutableInstance().isMultiInstanceOpen_ = false;
    RSUIContextManager::MutableInstance().CloseAllSyncTransaction(syncId);

    RSUIContextManager::MutableInstance().isMultiInstanceOpen_ = true;
    RSUIContextManager::MutableInstance().rsUIContextMap_.clear();
    ASSERT_TRUE(RSUIContextManager::MutableInstance().rsUIContextMap_.empty());
    RSUIContextManager::MutableInstance().CloseAllSyncTransaction(syncId);

    RSUIContextManager::MutableInstance().isMultiInstanceOpen_ = true;
    RSUIContextManager::MutableInstance().CreateRSUIContext();
    ASSERT_FALSE(RSUIContextManager::MutableInstance().rsUIContextMap_.empty());
    RSUIContextManager::MutableInstance().CloseAllSyncTransaction(syncId);

    ASSERT_FALSE(RSUIContextManager::MutableInstance().rsUIContextMap_.empty());
    auto it = RSUIContextManager::MutableInstance().rsUIContextMap_.begin();
    it->second->rsSyncTransactionHandler_ = nullptr;
    RSUIContextManager::MutableInstance().CloseAllSyncTransaction(syncId);

    RSUIContextManager::MutableInstance().rsUIContextMap_.clear();
    RSUIContextManager::MutableInstance().CreateRSUIContext();
    ASSERT_FALSE(RSUIContextManager::MutableInstance().rsUIContextMap_.empty());
    auto syncHandler =
        RSUIContextManager::MutableInstance().rsUIContextMap_.begin()->second->GetSyncTransactionHandler();
    ASSERT_FALSE(syncHandler == nullptr);
    syncHandler->rsTransaction_ = nullptr;
    RSUIContextManager::MutableInstance().CloseAllSyncTransaction(syncId);

    RSUIContextManager::MutableInstance().rsUIContextMap_.clear();
    RSUIContextManager::MutableInstance().CreateRSUIContext();
    ASSERT_FALSE(RSUIContextManager::MutableInstance().rsUIContextMap_.empty());
    auto syncHandler1 =
        RSUIContextManager::MutableInstance().rsUIContextMap_.begin()->second->GetSyncTransactionHandler();
    ASSERT_FALSE(syncHandler1 == nullptr);
    auto transaction = syncHandler1->GetCommonRSTransaction();
    ASSERT_FALSE(transaction == nullptr);
    transaction->rsTransactionHandler_ = nullptr;
    RSUIContextManager::MutableInstance().CloseAllSyncTransaction(syncId);
}
} // namespace OHOS::Rosen
