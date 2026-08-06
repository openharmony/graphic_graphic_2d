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
#include "display_engine/ipc_callbacks/rs_ide_status_change_callback_stub.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSDEStatusChangeCallbackStubMock : public RSDEStatusChangeCallbackStub {
public:
    RSDEStatusChangeCallbackStubMock() = default;
    virtual ~RSDEStatusChangeCallbackStubMock() = default;

    void OnNotifyDEStatusChangeDone(const uint32_t sceneKey, const std::vector<uint8_t>& result) override {}
};

class RSDEStatusChangeCallbackStubTest : public testing::Test {
public:
    static sptr<RSDEStatusChangeCallbackStubMock> stub;

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

sptr<RSDEStatusChangeCallbackStubMock> RSDEStatusChangeCallbackStubTest::stub = nullptr;

void RSDEStatusChangeCallbackStubTest::SetUpTestCase()
{
    stub = new RSDEStatusChangeCallbackStubMock();
}
void RSDEStatusChangeCallbackStubTest::TearDownTestCase()
{
    stub = nullptr;
}
void RSDEStatusChangeCallbackStubTest::SetUp() {}
void RSDEStatusChangeCallbackStubTest::TearDown() {}

/**
 * @tc.name: OnRemoteRequest001
 * @tc.desc: Verify function OnRemoteRequest incorrect code and no data
 * @tc.type: FUNC
 * @tc.require: issueIABHAX
 */
HWTEST_F(RSDEStatusChangeCallbackStubTest, OnRemoteRequest001, TestSize.Level1)
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
 * @tc.require: issueIABHAX
 */
HWTEST_F(RSDEStatusChangeCallbackStubTest, OnRemoteRequest002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto code = static_cast<uint32_t>(RSIDEStatusChangeCallbackInterfaceCode::ON_DE_STATUS_CHANGE_DONE);
    data.WriteInterfaceToken(u"ohos.rosen.TestDescriptor");
 
    int res = stub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ERR_INVALID_STATE, res);
}

/**
 * @tc.name: OnRemoteRequest003
 * @tc.desc: Verify function OnRemoteRequest incorrect code and data present
 * @tc.type: FUNC
 * @tc.require: issueIABHAX
 */
HWTEST_F(RSDEStatusChangeCallbackStubTest, OnRemoteRequest003, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto code = -1;
    data.WriteInterfaceToken(RSIDEStatusChangeCallback::GetDescriptor());
 
    int res = stub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(IPC_STUB_UNKNOW_TRANS_ERR, res);
}

/**
 * @tc.name: OnRemoteRequest004
 * @tc.desc: Verify function OnRemoteRequest ON_DE_STATUS_CHANGE_DONE with valid data
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSDEStatusChangeCallbackStubTest, OnRemoteRequest004, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto code = static_cast<uint32_t>(RSIDEStatusChangeCallbackInterfaceCode::ON_DE_STATUS_CHANGE_DONE);
    data.WriteInterfaceToken(RSIDEStatusChangeCallback::GetDescriptor());
    data.WriteUint32(123);
    std::vector<uint8_t> values = {1, 2, 3};
    data.WriteUInt8Vector(values);
 
    int res = stub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ERR_NONE, res);
}
} // namespace OHOS::Rosen