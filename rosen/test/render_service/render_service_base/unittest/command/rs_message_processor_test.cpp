/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

class RSMessageProcessorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSMessageProcessorTest::SetUpTestCase() {}
void RSMessageProcessorTest::TearDownTestCase() {}
void RSMessageProcessorTest::SetUp() {}
void RSMessageProcessorTest::TearDown() {}

/**
 * @tc.name: testing
 * @tc.desc:
 * @tc.type: FUNC
 */
HWTEST_F(RSMessageProcessorTest, testing, TestSize.Level1)
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
 * @tc.name: ReInitializeMovedMap001
 * @tc.desc: test results of ReInitializeMovedMap
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSMessageProcessorTest, ReInitializeMovedMap001, TestSize.Level1)
{
    RSMessageProcessor::Instance().ReInitializeMovedMap();
    EXPECT_EQ(false, RSMessageProcessor::Instance().HasTransaction());
}

/**
 * @tc.name: AddUIMessage001
 * @tc.desc: test results of AddUIMessage
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSMessageProcessorTest, AddUIMessage001, TestSize.Level1)
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
 * @tc.name: AddUIMessage002
 * @tc.desc: test results of AddUIMessage
 * @tc.type: FUNC
 * @tc.require: issueI9SBEZ
 */
HWTEST_F(RSMessageProcessorTest, AddUIMessage002, TestSize.Level1)
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
 * @tc.name: GetTransaction001
 * @tc.desc: test results of GetTransaction
 * @tc.type: FUNC
 * @tc.require: issueI9SBEZ
 */
HWTEST_F(RSMessageProcessorTest, GetTransaction001, TestSize.Level1)
{
    uint32_t pid = 10;
    EXPECT_EQ(nullptr, RSMessageProcessor::Instance().GetTransaction(pid));

    pid = 0;
    auto transactionData = std::make_shared<RSTransactionData>();
    RSMessageProcessor::Instance().transactionMap_[pid] = transactionData;
    EXPECT_NE(nullptr, RSMessageProcessor::Instance().GetTransaction(pid));
    EXPECT_EQ(RSMessageProcessor::Instance().transactionMap_, RSMessageProcessor::Instance().GetAllTransactions());
}

/**
 * @tc.name: AddUIMessageLvalueReferenceTest
 * @tc.desc: test results of AddUIMessage lvalue reference
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSMessageProcessorTest, AddUIMessageLvalueReferenceTest, TestSize.Level1)
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
 * @tc.name: GinstanceValidIsfalseTest
 * @tc.desc: test results of g_instanceValid is false
 * @tc.type: FUNC
 * @tc.require: issueIA61E9
 */
HWTEST_F(RSMessageProcessorTest, GinstanceValidIsfalseTest, TestSize.Level1)
{
    uint32_t pid = 1;
    std::unique_ptr<RSCommand> command = nullptr;
    RSMessageProcessor* messageProcessorTest1 = new RSMessageProcessor();
    RSMessageProcessor* messageProcessorTest2 = new RSMessageProcessor();
    delete messageProcessorTest1;
    messageProcessorTest1 = nullptr;

    messageProcessorTest2->AddUIMessage(pid, command);
    std::unique_ptr<RSCommand> rsCommand = nullptr;
    messageProcessorTest2->AddUIMessage(pid, std::move(rsCommand));
    EXPECT_FALSE(messageProcessorTest2->HasTransaction());
    EXPECT_FALSE(messageProcessorTest2->HasTransaction(0));
    messageProcessorTest2->ReInitializeMovedMap();
    EXPECT_EQ(messageProcessorTest2->GetTransaction(0), nullptr);
    messageProcessorTest2->GetAllTransactions();

    delete messageProcessorTest2;
    messageProcessorTest2 = nullptr;
}
} // namespace Rosen
} // namespace OHOS