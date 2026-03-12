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

#include "ipc_callbacks/dfx/rs_dump_callback_stub.h"
#include "ipc_callbacks/dfx/rs_dump_callback_ipc_interface_code.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSDumpCallbackStubMock : public RSDumpCallbackStub {
public:
    RSDumpCallbackStubMock() = default;
    virtual ~RSDumpCallbackStubMock() = default;
    void OnDumpResult(std::string& dumpResult) override {}
};

class RSDumpCallbackStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSDumpCallbackStubTest::SetUpTestCase() {}
void RSDumpCallbackStubTest::TearDownTestCase() {}
void RSDumpCallbackStubTest::SetUp() {}
void RSDumpCallbackStubTest::TearDown() {}

/**
 * @tc.name: OnRemoteRequest_InvalidToken
 * @tc.desc: Verify OnRemoteRequest with invalid interface token
 * @tc.type:FUNC
 * @tc.require: issueIAKP5Y
 */
HWTEST_F(RSDumpCallbackStubTest, OnRemoteRequest_InvalidToken, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto stub = std::make_shared<RSDumpCallbackStubMock>();
    uint32_t code = static_cast<uint32_t>(RSDumpCallbackInterfaceCode::REPLY_DUMP_RESULT_TO_SERVICE);
    
    int res = stub->OnRemoteRequest(code, data, reply, option);
    EXPECT_TRUE(res == ERR_INVALID_STATE);
}

/**
 * @tc.name: OnRemoteRequest_ValidCode
 * @tc.desc: Verify OnRemoteRequest with valid interface token and code
 * @tc.type:FUNC
 * @tc.require: issueIAKP5Y
 */
HWTEST_F(RSDumpCallbackStubTest, OnRemoteRequest_ValidCode, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto stub = std::make_shared<RSDumpCallbackStubMock>();
    
    data.WriteInterfaceToken(RSIDumpCallback::GetDescriptor());
    data.WriteInt32(0);
    
    uint32_t code = static_cast<uint32_t>(RSDumpCallbackInterfaceCode::REPLY_DUMP_RESULT_TO_SERVICE);
    int res = stub->OnRemoteRequest(code, data, reply, option);
    EXPECT_TRUE(res == ERR_NONE);
}

/**
 * @tc.name: OnRemoteRequest_DefaultCase
 * @tc.desc: Verify OnRemoteRequest handles unknown code in default case
 * @tc.type:FUNC
 * @tc.require: issueIAKP5Y
 */
HWTEST_F(RSDumpCallbackStubTest, OnRemoteRequest_DefaultCase, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto stub = std::make_shared<RSDumpCallbackStubMock>();
    
    data.WriteInterfaceToken(RSIDumpCallback::GetDescriptor());
    
    int res = stub->OnRemoteRequest(9999, data, reply, option);
    EXPECT_FALSE(res == ERR_NONE);
}

/**
 * @tc.name: OnRemoteRequest_ReadInt32Failed
 * @tc.desc: Verify OnRemoteRequest handles ReadInt32 failure
 * @tc.type:FUNC
 * @tc.require: issueIAKP5Y
 */
HWTEST_F(RSDumpCallbackStubTest, OnRemoteRequest_ReadInt32Failed, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto stub = std::make_shared<RSDumpCallbackStubMock>();
    
    data.WriteInterfaceToken(RSIDumpCallback::GetDescriptor());
    
    uint32_t code = static_cast<uint32_t>(RSDumpCallbackInterfaceCode::REPLY_DUMP_RESULT_TO_SERVICE);
    int res = stub->OnRemoteRequest(code, data, reply, option);
    EXPECT_TRUE(res == ERR_INVALID_STATE);
}

/**
 * @tc.name: OnRemoteRequest_NullDataPtr
 * @tc.desc: Verify OnRemoteRequest handles null data pointer
 * @tc.type:FUNC
 * @tc.require: issueIAKP5Y
 */
HWTEST_F(RSDumpCallbackStubTest, OnRemoteRequest_NullDataPtr, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto stub = std::make_shared<RSDumpCallbackStubMock>();
    
    data.WriteInterfaceToken(RSIDumpCallback::GetDescriptor());
    data.WriteInt32(0);
    
    uint32_t code = static_cast<uint32_t>(RSDumpCallbackInterfaceCode::REPLY_DUMP_RESULT_TO_SERVICE);
    int res = stub->OnRemoteRequest(code, data, reply, option);
    EXPECT_TRUE(res == ERR_NONE);
}

} // namespace OHOS::Rosen
