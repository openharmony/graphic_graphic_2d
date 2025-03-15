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
#include "ipc_callbacks/rs_first_frame_callback_stub.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSFirstFrameCallbackStubMock : public RSFirstFrameCallbackStub {
public:
    RSFirstFrameCallbackStubMock() = default;
    virtual ~RSFirstFrameCallbackStubMock() = default;
    void OnPowerOnFirstFrame(uint32_t screenId, int64_t timestamp) override {};
};

class RSFirstFrameCallbackStubTest : public testing::Test {
public:
    static sptr<RSFirstFrameCallbackStubMock> stub;

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

sptr<RSFirstFrameCallbackStubMock> RSFirstFrameCallbackStubTest::stub = nullptr;

void RSFirstFrameCallbackStubTest::SetUpTestCase()
{
    stub = new RSFirstFrameCallbackStubMock();
}
void RSFirstFrameCallbackStubTest::TearDownTestCase()
{
    stub = nullptr;
}
void RSFirstFrameCallbackStubTest::SetUp() {}
void RSFirstFrameCallbackStubTest::TearDown() {}

/**
 * @tc.name: OnRemoteRequest001
 * @tc.desc: Verify function OnRemoteRequest incorrect code and no data
 * @tc.type: FUNC
 */
HWTEST_F(RSFirstFrameCallbackStubTest, OnRemoteRequest001, TestSize.Level1)
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
 */
HWTEST_F(RSFirstFrameCallbackStubTest, OnRemoteRequest002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto code = static_cast<uint32_t>(RSIFirstFrameCallbackInterfaceCode::ON_POWER_ON);
    data.WriteInterfaceToken(u"ohos.rosen.TestDescriptor");

    int res = stub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ERR_INVALID_STATE, res);
}

/**
 * @tc.name: OnRemoteRequest003
 * @tc.desc: Verify function OnRemoteRequest incorrect code and data present
 * @tc.type: FUNC
 */
HWTEST_F(RSFirstFrameCallbackStubTest, OnRemoteRequest003, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto code = -1;
    data.WriteInterfaceToken(RSIFirstFrameCallback::GetDescriptor());
    
    int res = stub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(IPC_STUB_UNKNOW_TRANS_ERR, res);
}

/**
 * @tc.name: OnRemoteRequest004
 * @tc.desc: Verify function OnRemoteRequest code:ON_POWER_ON
 * @tc.type: FUNC
 */
HWTEST_F(RSFirstFrameCallbackStubTest, OnRemoteRequest004, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto code = static_cast<uint32_t>(RSIFirstFrameCallbackInterfaceCode::ON_POWER_ON);
    data.WriteInterfaceToken(RSIFirstFrameCallback::GetDescriptor());

    int res = stub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ERR_NONE, res);
}
} // namespace Rosen
} // namespace OHOS
