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
    uiContext->DumpNodeTreeProcessor(out, canvasNode->GetId(), 0, 0);
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
    uiContext->rsTransactionHandler_ = nullptr;
    auto transaction = uiContext->GetRSTransaction();
    ASSERT_EQ(transaction, nullptr);
    std::string out = "";
    uiContext->DumpNodeTreeProcessor(out, 0, 0, 0);
    ASSERT_TRUE(out.find("transactionFlags") == std::string::npos);
    ASSERT_TRUE(out.find("UIContext") == std::string::npos);
}
} // namespace OHOS::Rosen