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

#include "ipc_callbacks/rs_transaction_data_callback_proxy.h"
#include "mock_iremote_object.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSTransactionDataCallbackProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSTransactionDataCallbackProxyTest::SetUpTestCase() {}
void RSTransactionDataCallbackProxyTest::TearDownTestCase() {}
void RSTransactionDataCallbackProxyTest::SetUp() {}
void RSTransactionDataCallbackProxyTest::TearDown() {}

/**
 * @tc.name: OnAfterProcess001
 * @tc.desc: Verify the OnAfterProcess
 * @tc.type:FUNC
 * @tc.require: issueIC9LLT
 */
HWTEST_F(RSTransactionDataCallbackProxyTest, OnAfterProcess001, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteMocker = new MockIRemoteObject();
    ASSERT_TRUE(remoteMocker != nullptr);
    auto TransactionDataCallbackProxy = std::make_shared<RSTransactionDataCallbackProxy>(remoteMocker);
    ASSERT_TRUE(TransactionDataCallbackProxy != nullptr);
    int32_t pid = 123;
    uint64_t timeStamp = 456;
    TransactionDataCallbackProxy->OnAfterProcess(pid, timeStamp);
    ASSERT_EQ(remoteMocker->receivedCode_,
        static_cast<uint32_t>(RSITransactionDataCallbackInterfaceCode::ON_AFTER_PROCESS));
}

/**
 * @tc.name: OnAfterProcess002
 * @tc.desc: Verify the OnAfterProcess error case
 * @tc.type:FUNC
 * @tc.require: issueIC9LLT
 */
HWTEST_F(RSTransactionDataCallbackProxyTest, OnAfterProcess002, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteMocker = new MockIRemoteObject();
    ASSERT_TRUE(remoteMocker != nullptr);
    remoteMocker->sendRequestResult_ = 1;
    auto TransactionDataCallbackProxy = std::make_shared<RSTransactionDataCallbackProxy>(remoteMocker);
    ASSERT_TRUE(TransactionDataCallbackProxy != nullptr);
    int32_t pid = 123;
    uint64_t timeStamp = 456;
    TransactionDataCallbackProxy->OnAfterProcess(pid, timeStamp);
    ASSERT_EQ(remoteMocker->receivedCode_,
        static_cast<uint32_t>(RSITransactionDataCallbackInterfaceCode::ON_AFTER_PROCESS));
}
} // namespace Rosen
} // namespace OHOS
