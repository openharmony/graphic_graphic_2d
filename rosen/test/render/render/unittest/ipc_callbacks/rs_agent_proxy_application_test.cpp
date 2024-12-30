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

#include "ipc_callbacks/rs_application_agent_proxy.h"
#include "mock_iremoter_objects.h"
#include "transaction/rs_transaction_data.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSAgentProxyApplicationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSAgentProxyApplicationTest::SetUpTestCase() {}
void RSAgentProxyApplicationTest::TearDownTestCase() {}
void RSAgentProxyApplicationTest::SetUp() {}
void RSAgentProxyApplicationTest::TearDown() {}

/**
 * @tc.name: OnTransaction001
 * @tc.desc: Verify the OnTransaction error case
 * @tc.type:FUNC
 * @tc.require: issueIB2AHG
 */
HWTEST_F(RSAgentProxyApplicationTest, OnTransaction002, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteMocker = new MockIRemoteObject();
    ASSERT_TRUE(remoteMocker != nullptr);
    remoteMocker->sendRequestResult_ = 1;
    auto rsApplicationAgentProxy = std::make_shared<RSApplicationAgentProxy>(remoteMocker);
    ASSERT_TRUE(rsApplicationAgentProxy != nullptr);
    std::shared_ptr<RSTransactionData> transactionData = std::make_shared<RSTransactionData>();
    rsApplicationAgentProxy->OnTransaction(transactionData);
    ASSERT_EQ(remoteMocker->receivedCode_, static_cast<uint32_t>(IApplicationAgentInterfaceCode::COMMIT_TRANSACTION));
}

/**
 * @tc.name: OnTransaction002
 * @tc.desc: Verify the OnTransaction
 * @tc.type:FUNC
 * @tc.require: issueIB2AHG
 */
HWTEST_F(RSAgentProxyApplicationTest, OnTransaction001, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteMocker = new MockIRemoteObject();
    ASSERT_TRUE(remoteMocker != nullptr);
    auto rsApplicationAgentProxy = std::make_shared<RSApplicationAgentProxy>(remoteMocker);
    ASSERT_TRUE(rsApplicationAgentProxy != nullptr);
    std::shared_ptr<RSTransactionData> transactionData = std::make_shared<RSTransactionData>();
    rsApplicationAgentProxy->OnTransaction(transactionData);
    ASSERT_EQ(remoteMocker->receivedCode_, static_cast<uint32_t>(IApplicationAgentInterfaceCode::COMMIT_TRANSACTION));
}
} // namespace Rosen
} // namespace OHOS
