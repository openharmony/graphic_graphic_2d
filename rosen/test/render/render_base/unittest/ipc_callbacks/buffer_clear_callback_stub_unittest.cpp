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

#include "ipc_callbacks/buffer_clear_callback_stub.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSBufferClearCallbackStubMock : public RSBufferClearCallbackStub {
public:
    RSBufferClearCallbackStubMock() = default;
    virtual ~RSBufferClearCallbackStubMock() = default;
    void OnBufferClear() override {};
};

class RSBufferClearCallbackStubUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSBufferClearCallbackStubUnitTest::SetUpTestCase() {}
void RSBufferClearCallbackStubUnitTest::TearDownTestCase() {}
void RSBufferClearCallbackStubUnitTest::SetUp() {}
void RSBufferClearCallbackStubUnitTest::TearDown() {}

/**
 * @tc.name: OnRemoteRequest001
 * @tc.desc: Verify function OnRemoteRequest
 * @tc.type:FUNC
 * @tc.require: issueIAKP5Y
 */
HWTEST_F(RSBufferClearCallbackStubUnitTest, OnRemoteRequest002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto rsBufferClearCallbackStub = std::make_shared<RSBufferClearCallbackStubMock>();
    auto code = static_cast<uint32_t>(RSIBufferClearCallbackInterfaceCode::ON_BUFFER_CLEAR);
    data.WriteInterfaceToken(RSIBufferClearCallback::GetDescriptor());

    int res = rsBufferClearCallbackStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_TRUE(res == ERR_NONE);
    res = rsBufferClearCallbackStub->OnRemoteRequest(-1, data, reply, option);
    EXPECT_TRUE(res == ERR_INVALID_STATE);
}

/**
 * @tc.name: OnRemoteRequest002
 * @tc.desc: Verify function OnRemoteRequest
 * @tc.type:FUNC
 * @tc.require: issueIAKP5Y
 */
HWTEST_F(RSBufferClearCallbackStubUnitTest, OnRemoteRequest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto rsBufferClearCallbackStub = std::make_shared<RSBufferClearCallbackStubMock>();
    uint32_t code = -1;
    int res = rsBufferClearCallbackStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_TRUE(res == ERR_INVALID_STATE);
    code = static_cast<uint32_t>(RSIBufferClearCallbackInterfaceCode::ON_BUFFER_CLEAR);

    res = rsBufferClearCallbackStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_TRUE(res == ERR_INVALID_STATE);
}

} // namespace OHOS::Rosen
