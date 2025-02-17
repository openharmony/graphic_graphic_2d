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
class RSBufferCallbackStubClearMock : public RSBufferClearCallbackStub {
public:
    RSBufferCallbackStubClearMock() = default;
    virtual ~RSBufferCallbackStubClearMock() = default;
    void OnBufferClear() override {};
};

class RSCallbackStubBufferClearTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSCallbackStubBufferClearTest::SetUpTestCase() {}
void RSCallbackStubBufferClearTest::TearDownTestCase() {}
void RSCallbackStubBufferClearTest::SetUp() {}
void RSCallbackStubBufferClearTest::TearDown() {}

/**
 * @tc.name: OnRemoteRequest001
 * @tc.desc: Verify function OnRemoteRequest
 * @tc.type:FUNC
 * @tc.require: issueIAKP5Y
 */
HWTEST_F(RSCallbackStubBufferClearTest, OnRemoteRequest002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int res = rsBufferClearCallbackStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_TRUE(res == ERR_NONE);
    res = rsBufferClearCallbackStub->OnRemoteRequest(-1, data, reply, option);
    EXPECT_TRUE(res == ERR_INVALID_STATE);
    auto rsBufferClearCallbackStub = std::make_shared<RSBufferCallbackStubClearMock>();
    auto code = static_cast<uint32_t>(RSIBufferClearCallbackInterfaceCode::ON_BUFFER_CLEAR);
    data.WriteInterfaceToken(RSIBufferClearCallback::GetDescriptor());
}

/**
 * @tc.name: OnRemoteRequest002
 * @tc.desc: Verify function OnRemoteRequest
 * @tc.type:FUNC
 * @tc.require: issueIAKP5Y
 */
HWTEST_F(RSCallbackStubBufferClearTest, OnRemoteRequest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto rsBufferClearCallbackStub = std::make_shared<RSBufferCallbackStubClearMock>();
    uint32_t code = -1;
    int res = rsBufferClearCallbackStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_TRUE(res == ERR_INVALID_STATE);
    res = rsBufferClearCallbackStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_TRUE(res == ERR_INVALID_STATE);
    code = static_cast<uint32_t>(RSIBufferClearCallbackInterfaceCode::ON_BUFFER_CLEAR);
}


} // namespace OHOS::Rosen
