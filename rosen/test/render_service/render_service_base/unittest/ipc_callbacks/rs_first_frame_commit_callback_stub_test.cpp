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
#include "ipc_callbacks/rs_first_frame_commit_callback_stub.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSFirstFrameCommitCallbackStubMock : public RSFirstFrameCommitCallbackStub {
public:
    RSFirstFrameCommitCallbackStubMock() = default;
    virtual ~RSFirstFrameCommitCallbackStubMock() = default;
    void OnFirstFrameCommit(uint32_t screenId, int64_t timestamp) override {};
};

class RSFirstFrameCommitCallbackStubTest : public testing::Test {
public:
    static sptr<RSFirstFrameCommitCallbackStubMock> stub;

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

sptr<RSFirstFrameCommitCallbackStubMock> RSFirstFrameCommitCallbackStubTest::stub = nullptr;

void RSFirstFrameCommitCallbackStubTest::SetUpTestCase()
{
    stub = new RSFirstFrameCommitCallbackStubMock();
}
void RSFirstFrameCommitCallbackStubTest::TearDownTestCase()
{
    stub = nullptr;
}
void RSFirstFrameCommitCallbackStubTest::SetUp() {}
void RSFirstFrameCommitCallbackStubTest::TearDown() {}

/**
 * @tc.name: OnRemoteRequest001
 * @tc.desc: Verify function OnRemoteRequest incorrect code and no data
 * @tc.type: FUNC
 * @tc.require: issuesIBTF2E
 */
HWTEST_F(RSFirstFrameCommitCallbackStubTest, OnRemoteRequest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto code = -1;

    int res = stub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ERR_INVALID_STATE, res);
}

/**
 * @tc.name: OnRemoteRequest002
 * @tc.desc: Verify function OnRemoteRequest incorrect Descriptor
 * @tc.type: FUNC
 * @tc.require: issuesIBTF2E
 */
HWTEST_F(RSFirstFrameCommitCallbackStubTest, OnRemoteRequest002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto code = static_cast<uint32_t>(RSIFirstFrameCommitCallbackInterfaceCode::ON_FIRST_FRAME_COMMIT);
    data.WriteInterfaceToken(u"ohos.rosen.TestDescriptor");

    int res = stub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ERR_INVALID_STATE, res);
}

/**
 * @tc.name: OnRemoteRequest003
 * @tc.desc: Verify function OnRemoteRequest incorrect code and data present
 * @tc.type: FUNC
 * @tc.require: issuesIBTF2E
 */
HWTEST_F(RSFirstFrameCommitCallbackStubTest, OnRemoteRequest003, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto code = -1;
    data.WriteInterfaceToken(RSIFirstFrameCommitCallback::GetDescriptor());
    
    int res = stub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(IPC_STUB_UNKNOW_TRANS_ERR, res);
}

/**
 * @tc.name: OnRemoteRequest004
 * @tc.desc: Verify function OnRemoteRequest code:ON_POWER_ON
 * @tc.type: FUNC
 * @tc.require: issuesIBTF2E
 */
HWTEST_F(RSFirstFrameCommitCallbackStubTest, OnRemoteRequest004, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto code = static_cast<uint32_t>(RSIFirstFrameCommitCallbackInterfaceCode::ON_FIRST_FRAME_COMMIT);
    data.WriteInterfaceToken(RSIFirstFrameCommitCallback::GetDescriptor());

    int res = stub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ERR_NONE, res);
}
} // namespace Rosen
} // namespace OHOS
