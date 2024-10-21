/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "command/rs_message_processor.h"
#include "include/command/rs_command_templates.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class RSMessageProcessorUniTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSMessageProcessorUniTest::SetUpTestCase() {}
void RSMessageProcessorUniTest::TearDownTestCase() {}
void RSMessageProcessorUniTest::SetUp() {}
void RSMessageProcessorUniTest::TearDown() {}

/**
 * @tc.name: ReInitializeMovedMapUniTest001
 * @tc.desc: test results of ReInitializeMovedMap
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSMessageProcessorUniTest, ReInitializeMovedMapUniTest001, TestSize.Level1)
{
    EXPECT_EQ(false, RSMessageProcessor::Instance().HasTransaction());
}

/**
 * @tc.name: AddUIMessageUniTest001
 * @tc.desc: test results of AddUIMessage
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSMessageProcessorUniTest, AddUIMessageUniTest001, TestSize.Level1)
{
    uint32_t pid = 1;
    RSMessageProcessor::Instance().AddUIMessage(pid, nullptr);
    EXPECT_EQ(true, RSMessageProcessor::Instance().HasTransaction());
    EXPECT_EQ(nullptr, RSMessageProcessor::Instance().GetTransaction(0));

    pid = 0;
    auto transactionData = std::make_shared<RSTransactionData>();
    RSMessageProcessor::Instance().transactionMap_[pid] = transactionData;
    RSMessageProcessor::Instance().AddUIMessage(pid, nullptr);
    EXPECT_EQ(true, RSMessageProcessor::Instance().HasTransaction());
}

/**
 * @tc.name: uniTesting
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSMessageProcessorUniTest, uniTesting, TestSize.Level1)
{
    uint32_t pid = 1;
    RSMessageProcessor::Instance().AddUIMessage(pid, nullptr);
    RSMessageProcessor::Instance().AddUIMessage(pid, nullptr);
    EXPECT_EQ(true, RSMessageProcessor::Instance().HasTransaction());

    RSMessageProcessor::Instance().HasTransaction(pid);
    EXPECT_NE(nullptr, RSMessageProcessor::Instance().GetTransaction(pid));
    RSMessageProcessor::Instance().GetAllTransactions();

    RSMessageProcessor::Instance().GetTransaction(0);
    EXPECT_EQ(nullptr, RSMessageProcessor::Instance().GetTransaction(0));
}

/**
 * @tc.name: AddUIMessageUniTest002
 * @tc.desc: test results of AddUIMessage
 * @tc.type: FUNC
 * @tc.require: issueI9SBEZ
 */
HWTEST_F(RSMessageProcessorUniTest, AddUIMessageUniTest002, TestSize.Level1)
{
    uint32_t pid = 1;
    std::unique_ptr<RSCommand> command = nullptr;
    RSMessageProcessor::Instance().AddUIMessage(pid, std::move(command));
    EXPECT_EQ(true, RSMessageProcessor::Instance().HasTransaction());

    pid = 0;
    auto transactionData = std::make_shared<RSTransactionData>();
    RSMessageProcessor::Instance().transactionMap_[pid] = transactionData;
    std::unique_ptr<RSCommand> rsCommand = nullptr;
    RSMessageProcessor::Instance().AddUIMessage(pid, std::move(rsCommand));
    EXPECT_EQ(true, RSMessageProcessor::Instance().HasTransaction());
    EXPECT_EQ(false, RSMessageProcessor::Instance().HasTransaction(pid));
}

/**
 * @tc.name: AddUIMessageLvalueReferenceTest
 * @tc.desc: test results of AddUIMessage lvalue reference
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSMessageProcessorUniTest, AddUIMessageLvalueReferenceTest, TestSize.Level1)
{
    uint32_t pid = 1;
    std::unique_ptr<RSCommand> command = nullptr;
    RSMessageProcessor::Instance().transactionMap_.clear();
    RSMessageProcessor::Instance().AddUIMessage(pid, command);
    EXPECT_EQ(true, RSMessageProcessor::Instance().HasTransaction());
    EXPECT_EQ(nullptr, RSMessageProcessor::Instance().GetTransaction(0));

    pid = 0;
    auto transactionData = std::make_shared<RSTransactionData>();
    RSMessageProcessor::Instance().transactionMap_[pid] = transactionData;
    RSMessageProcessor::Instance().AddUIMessage(pid, command);
    EXPECT_EQ(true, RSMessageProcessor::Instance().HasTransaction());
}

/**
 * @tc.name: GetTransactionUniTest001
 * @tc.desc: test results of GetTransaction
 * @tc.type: FUNC
 * @tc.require: issueI9SBEZ
 */
HWTEST_F(RSMessageProcessorUniTest, GetTransactionUniTest001, TestSize.Level1)
{
    uint32_t pid = 10;
    EXPECT_EQ(nullptr, RSMessageProcessor::Instance().GetTransaction(pid));

    pid = 0;
    auto transactionData = std::make_shared<RSTransactionData>();
    RSMessageProcessor::Instance().transactionMap_[pid] = transactionData;
    EXPECT_NE(nullptr, RSMessageProcessor::Instance().GetTransaction(pid));
    EXPECT_EQ(RSMessageProcessor::Instance().transactionMap_, RSMessageProcessor::Instance().GetAllTransactions());
}
} // namespace Rosen
} // namespace OHOS