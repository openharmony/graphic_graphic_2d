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
 * @tc.name: AddUIMessage001
 * @tc.desc: test results of AddUIMessage
 * @tc.type: FUNC
 * @tc.require: issueI9P2KH
 */
HWTEST_F(RSMessageProcessorTest, AddUIMessage01, TestSize.Level1)
{
    uint32_t pid = 1;
    RSMessageProcessor::Instance().AddUIMessage(pid, nullptr);

    pid = 0;
    auto transactionData = std::make_shared<RSTransactionData>();
    RSMessageProcessor::Instance().transactionMap_[pid] = transactionData;
    RSMessageProcessor::Instance().AddUIMessage(pid, nullptr);
    EXPECT_NE(transactionData, nullptr);
}

/**
 * @tc.name: GetTransaction001
 * @tc.desc: test results of GetTransaction
 * @tc.type: FUNC
 * @tc.require: issueI9SBEZ
 */
HWTEST_F(RSMessageProcessorTest, GetTransaction01, TestSize.Level1)
{
    uint32_t pid = 0;
    auto transactionData = std::make_shared<RSTransactionData>();
    RSMessageProcessor::Instance().transactionMap_[pid] = transactionData;
    EXPECT_NE(nullptr, RSMessageProcessor::Instance().GetTransaction(pid));
    EXPECT_FALSE(RSMessageProcessor::Instance().transactionMap_.empty());
    EXPECT_FALSE(RSMessageProcessor::Instance().GetAllTransactions().empty());
    EXPECT_TRUE(RSMessageProcessor::Instance().transactionMap_.empty());
}
} // namespace Rosen
} // namespace OHOS