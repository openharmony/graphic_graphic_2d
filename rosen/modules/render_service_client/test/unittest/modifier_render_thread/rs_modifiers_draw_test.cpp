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
#include "command/rs_delegate_composite_command.h"
#include "transaction/rs_transaction_data.h"

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

HWTEST_F(RSModifiersDrawTest, ConvertTransaction_EmptyTransactionConfigList001, TestSize.Level1)
{
    RSModifiersDraw modifiersDraw;
    auto transactionData = std::make_unique<RSTransactionData>();
    std::vector<RSTransactionConfig> transactionConfigList;
    modifiersDraw.ConvertTransaction(transactionData, transactionConfigList);
    EXPECT_EQ(transactionConfigList.size(), 0);
}

HWTEST_F(RSModifiersDrawTest, ConvertTransaction_NonTransactionBufferCommand001, TestSize.Level1)
{
    RSModifiersDraw modifiersDraw;
    auto transactionData = std::make_unique<RSTransactionData>();
    std::vector<RSTransactionConfig> transactionConfigList;
    RSTransactionConfig config;
    config.nodeId = 12345;
    transactionConfigList.push_back(config);
    auto& payload = transactionData->GetPayload();
    auto command = std::make_unique<TransactionBufferCommand>();
    payload.emplace_back(12345, FollowType::FOLLOW_TO_PARENT, std::move(command));
    modifiersDraw.ConvertTransaction(transactionData, transactionConfigList);
    EXPECT_EQ(transactionConfigList.size(), 0);
}

HWTEST_F(RSModifiersDrawTest, ConvertTransaction_TransactionBufferCommand001, TestSize.Level1)
{
    RSModifiersDraw modifiersDraw;
    auto transactionData = std::make_unique<RSTransactionData>();
    std::vector<RSTransactionConfig> transactionConfigList;
    RSTransactionConfig config;
    config.nodeId = 12345;
    transactionConfigList.push_back(config);
    auto& payload = transactionData->GetPayload();
    auto command = std::make_unique<TransactionBufferCommand>();
    payload.emplace_back(12345, FollowType::FOLLOW_TO_PARENT, std::move(command));
    modifiersDraw.ConvertTransaction(transactionData, transactionConfigList);
    EXPECT_EQ(payload.size(), 1);
}

HWTEST_F(RSModifiersDrawTest, ConvertTransaction_NullCommandInPayload001, TestSize.Level1)
{
    RSModifiersDraw modifiersDraw;
    auto transactionData = std::make_unique<RSTransactionData>();
    std::vector<RSTransactionConfig> transactionConfigList;
    RSTransactionConfig config;
    config.nodeId = 12345;
    transactionConfigList.push_back(config);
    auto& payload = transactionData->GetPayload();
    std::unique_ptr<RSCommand> nullCommand;
    payload.emplace_back(12345, FollowType::FOLLOW_TO_PARENT, std::move(nullCommand));
    modifiersDraw.ConvertTransaction(transactionData, transactionConfigList);
    EXPECT_EQ(transactionConfigList.size(), 1);
}
} // namespace Rosen
} // namespace OHOS
