/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "src/ipc_callbacks/rs_sync_transaction_controller_stub.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSSyncTransactionControllerStubMock : public RSSyncTransactionControllerStub {
public:
    RSSyncTransactionControllerStubMock() = default;
    virtual ~RSSyncTransactionControllerStubMock() = default;

    void CreateTransactionFinished() override {}
};

class RSSyncTransactionControllerStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static sptr<RSSyncTransactionControllerStubMock> stub_;
};

sptr<RSSyncTransactionControllerStubMock> RSSyncTransactionControllerStubTest::stub_ = nullptr;

void RSSyncTransactionControllerStubTest::SetUpTestCase()
{
    stub_ = new RSSyncTransactionControllerStubMock();
}
void RSSyncTransactionControllerStubTest::TearDownTestCase()
{
    stub_ = nullptr;
}
void RSSyncTransactionControllerStubTest::SetUp() {}
void RSSyncTransactionControllerStubTest::TearDown() {}

/**
 * @tc.name: OnRemoteRequest001
 * @tc.desc: Verify the OnRemoteRequest
 * @tc.type:FUNC
 */
HWTEST_F(RSSyncTransactionControllerStubTest, OnRemoteRequest001, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSyncTransactionControllerStubTest OnRemoteRequest001 start";
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    int res = stub_->OnRemoteRequest(
        RSISyncTransactionController::CREATE_TRANSACTION_FINISHED, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_STATE);
    GTEST_LOG_(INFO) << "RSSyncTransactionControllerStubTest OnRemoteRequest001 end";
}

/**
 * @tc.name: OnRemoteRequest002
 * @tc.desc: Verify the OnRemoteRequest
 * @tc.type:FUNC
 */
HWTEST_F(RSSyncTransactionControllerStubTest, OnRemoteRequest002, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "RSSyncTransactionControllerStubTest OnRemoteRequest002 start";
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(RSISyncTransactionController::GetDescriptor());
    int res = stub_->OnRemoteRequest(-1, data, reply, option);
    ASSERT_EQ(res, IPC_STUB_UNKNOW_TRANS_ERR);
    GTEST_LOG_(INFO) << "RSSyncTransactionControllerStubTest OnRemoteRequest002 end";
}
} // namespace Rosen
} // namespace OHOS