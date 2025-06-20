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

#include "ipc_callbacks/rs_transaction_callback_proxy.h"
#include "mock_iremote_object.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSTransactionCallbackProxyStub : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSTransactionCallbackProxyStub::SetUpTestCase() {}
void RSTransactionCallbackProxyStub::TearDownTestCase() {}
void RSTransactionCallbackProxyStub::SetUp() {}
void RSTransactionCallbackProxyStub::TearDown() {}

/**
 * @tc.name: RSTransactionCallbackProxy001
 * @tc.desc: Verify the RSTransactionCallbackProxy001
 * @tc.type:FUNC
 * @tc.require: issueIC9LLT
 */
HWTEST_F(RSTransactionCallbackProxyStub, RSTransactionCallbackProxy001, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteMocker = new MockIRemoteObject();
    ASSERT_TRUE(remoteMocker != nullptr);
    auto transactionDataCallbackProxy = std::make_shared<RSTransactionCallbackProxy>(remoteMocker);
    ASSERT_TRUE(transactionDataCallbackProxy != nullptr);
}

/**
 * @tc.name: OnCompleted001
 * @tc.desc: Verify the OnCompleted
 * @tc.type:FUNC
 * @tc.require: issueIC9LLT
 */
HWTEST_F(RSTransactionCallbackProxyStub, OnCompleted001, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteMocker = new MockIRemoteObject();
    ASSERT_TRUE(remoteMocker != nullptr);
    auto transactionDataCallbackProxy = std::make_shared<RSTransactionCallbackProxy>(remoteMocker);
    ASSERT_TRUE(transactionDataCallbackProxy != nullptr);

    // onCompletedCallback_ is nullptr
    OnCompletedRet ret = {
        .queueId = 1,
        .releaseFence = SyncFence::INVALID_FENCE,
        .bufferSeqNum = 3,
        .isActiveGame = true,
    };
    GSError retVal = transactionDataCallbackProxy->OnCompleted(ret);
    ASSERT_EQ(retVal, GSERROR_OK);
}

/**
 * @tc.name: GoBackground001
 * @tc.desc: Verify the GoBackground001
 * @tc.type:FUNC
 * @tc.require: issueIC9LLT
 */
HWTEST_F(RSTransactionCallbackProxyStub, GoBackground001, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteMocker = new MockIRemoteObject();
    ASSERT_TRUE(remoteMocker != nullptr);
    auto transactionDataCallbackProxy = std::make_shared<RSTransactionCallbackProxy>(remoteMocker);
    ASSERT_TRUE(transactionDataCallbackProxy != nullptr);

    uint64_t queueId = 1;
    GSError retVal = transactionDataCallbackProxy->GoBackground(queueId);
    ASSERT_EQ(retVal, GSERROR_OK);
}

/**
 * @tc.name: OnDropBuffers001
 * @tc.desc: Verify the GoBackground001
 * @tc.type:FUNC
 * @tc.require: issueIC9LLT
 */
HWTEST_F(RSTransactionCallbackProxyStub, OnDropBuffers001, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteMocker = new MockIRemoteObject();
    ASSERT_TRUE(remoteMocker != nullptr);
    auto transactionDataCallbackProxy = std::make_shared<RSTransactionCallbackProxy>(remoteMocker);
    ASSERT_TRUE(transactionDataCallbackProxy != nullptr);

    OnDropBuffersRet ret;
    ret.bufferSeqNums = { 1, 2 };
    GSError retVal = transactionDataCallbackProxy->OnDropBuffers(ret);
    ASSERT_EQ(retVal, GSERROR_OK);
}

/**
 * @tc.name: SetDefaultWidthAndHeight001
 * @tc.desc: Verify the SetDefaultWidthAndHeight001
 * @tc.type:FUNC
 * @tc.require: issueIC9LLT
 */
HWTEST_F(RSTransactionCallbackProxyStub, SetDefaultWidthAndHeight001, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteMocker = new MockIRemoteObject();
    ASSERT_TRUE(remoteMocker != nullptr);
    auto transactionDataCallbackProxy = std::make_shared<RSTransactionCallbackProxy>(remoteMocker);
    ASSERT_TRUE(transactionDataCallbackProxy != nullptr);

    OnSetDefaultWidthAndHeightRet ret = {.queueId = 2, .height = 1, .width = 3};
    GSError retVal = transactionDataCallbackProxy->SetDefaultWidthAndHeight(ret);
    ASSERT_EQ(retVal, GSERROR_OK);
}

/**
 * @tc.name: OnSurfaceDump001
 * @tc.desc: Verify the OnSurfaceDump001
 * @tc.type:FUNC
 * @tc.require: issueIC9LLT
 */
HWTEST_F(RSTransactionCallbackProxyStub, OnSurfaceDump001, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteMocker = new MockIRemoteObject();
    ASSERT_TRUE(remoteMocker != nullptr);
    auto transactionDataCallbackProxy = std::make_shared<RSTransactionCallbackProxy>(remoteMocker);
    ASSERT_TRUE(transactionDataCallbackProxy != nullptr);

    OnSurfaceDumpRet ret;
    GSError retVal = transactionDataCallbackProxy->OnSurfaceDump(ret);
    ASSERT_EQ(retVal, GSERROR_BINDER);
}
} // namespace Rosen
} // namespace OHOS
