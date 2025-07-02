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

#include "ipc_callbacks/rs_transaction_data_callback_stub.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSTransactionDataCallbackStubMock : public RSTransactionDataCallbackStub {
public:
    RSTransactionDataCallbackStubMock() = default;
    virtual ~RSTransactionDataCallbackStubMock() = default;
    void OnAfterProcess(uint64_t token, uint64_t timeStamp) override {};
};

class RSTransactionDataCallbackStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSTransactionDataCallbackStubTest::SetUpTestCase() {}
void RSTransactionDataCallbackStubTest::TearDownTestCase() {}
void RSTransactionDataCallbackStubTest::SetUp() {}
void RSTransactionDataCallbackStubTest::TearDown() {}

/**
 * @tc.name: OnRemoteRequest001
 * @tc.desc: Verify function OnRemoteRequest
 * @tc.type:FUNC
 * @tc.require: issueIC9LLT
 */
HWTEST_F(RSTransactionDataCallbackStubTest, OnRemoteRequest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto RSTransactionDataCallbackStub = std::make_shared<RSTransactionDataCallbackStubMock>();
    ASSERT_NE(RSTransactionDataCallbackStub, nullptr);
    auto code = static_cast<uint32_t>(RSITransactionDataCallbackInterfaceCode::ON_AFTER_PROCESS);
    data.WriteInterfaceToken(RSITransactionDataCallback::GetDescriptor());
    int res = RSTransactionDataCallbackStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_TRUE(res == ERR_NONE);
}

/**
 * @tc.name: OnRemoteRequest002
 * @tc.desc: Verify function OnRemoteRequest
 * @tc.type:FUNC
 * @tc.require: issueIC9LLT
 */
HWTEST_F(RSTransactionDataCallbackStubTest, OnRemoteRequest002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto RSTransactionDataCallbackStub = std::make_shared<RSTransactionDataCallbackStubMock>();
    ASSERT_NE(RSTransactionDataCallbackStub, nullptr);
    uint32_t code = std::numeric_limits<uint32_t>::max();
    int res = RSTransactionDataCallbackStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_TRUE(res == ERR_INVALID_STATE);
}

/**
 * @tc.name: OnRemoteRequest003
 * @tc.desc: Verify function OnRemoteRequest
 * @tc.type:FUNC
 * @tc.require: issueIC9LLT
 */
HWTEST_F(RSTransactionDataCallbackStubTest, OnRemoteRequest003, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto RSTransactionDataCallbackStub = std::make_shared<RSTransactionDataCallbackStubMock>();
    ASSERT_NE(RSTransactionDataCallbackStub, nullptr);
    uint32_t code = static_cast<uint32_t>(RSITransactionDataCallbackInterfaceCode::ON_AFTER_PROCESS);
    int res = RSTransactionDataCallbackStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_TRUE(res == ERR_INVALID_STATE);
}

/**
 * @tc.name: OnRemoteRequest004
 * @tc.desc: Verify function OnRemoteRequest
 * @tc.type:FUNC
 * @tc.require: issueIC9LLT
 */
HWTEST_F(RSTransactionDataCallbackStubTest, OnRemoteRequest004, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto RSTransactionDataCallbackStub = std::make_shared<RSTransactionDataCallbackStubMock>();
    ASSERT_NE(RSTransactionDataCallbackStub, nullptr);
    uint32_t code = std::numeric_limits<uint32_t>::max();
    data.WriteInterfaceToken(RSITransactionDataCallback::GetDescriptor());
    int res = RSTransactionDataCallbackStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_TRUE(res != ERR_NONE);
}
} // namespace OHOS::Rosen