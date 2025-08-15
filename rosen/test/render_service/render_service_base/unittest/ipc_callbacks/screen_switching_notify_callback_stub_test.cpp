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

#include "ipc_callbacks/screen_switching_notify_callback_stub.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSScreenSwitchingNotifyCallbackStubMock : public RSScreenSwitchingNotifyCallbackStub {
public:
    RSScreenSwitchingNotifyCallbackStubMock() = default;
    virtual ~RSScreenSwitchingNotifyCallbackStubMock() = default;
    void OnScreenSwitchingNotify(bool status) override {};
};

class RSScreenSwitchingNotifyCallbackStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSScreenSwitchingNotifyCallbackStubTest::SetUpTestCase() {}
void RSScreenSwitchingNotifyCallbackStubTest::TearDownTestCase() {}
void RSScreenSwitchingNotifyCallbackStubTest::SetUp() {}
void RSScreenSwitchingNotifyCallbackStubTest::TearDown() {}

/**
 * @tc.name: OnRemoteRequest001
 * @tc.desc: Verify function OnRemoteRequest
 * @tc.type:FUNC
 */
HWTEST_F(RSScreenSwitchingNotifyCallbackStubTest, OnRemoteRequest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto rsScreenSwitchingNotifyCallbackStub = std::make_shared<RSScreenSwitchingNotifyCallbackStubMock>();
    ASSERT_NE(rsScreenSwitchingNotifyCallbackStub, nullptr);
    auto code = static_cast<uint32_t>(RSIScreenSwitchingNotifyCallbackInterfaceCode::ON_SCREEN_SWITCHING_NOTIFY);
    data.WriteInterfaceToken(RSIScreenSwitchingNotifyCallback::GetDescriptor());
    int res = rsScreenSwitchingNotifyCallbackStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_TRUE(res == ERR_INVALID_DATA);
}

/**
 * @tc.name: OnRemoteRequest002
 * @tc.desc: Verify function OnRemoteRequest
 * @tc.type:FUNC
 */
HWTEST_F(RSScreenSwitchingNotifyCallbackStubTest, OnRemoteRequest002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto rsScreenSwitchingNotifyCallbackStub = std::make_shared<RSScreenSwitchingNotifyCallbackStubMock>();
    ASSERT_NE(rsScreenSwitchingNotifyCallbackStub, nullptr);
    uint32_t code = std::numeric_limits<uint32_t>::max();
    int res = rsScreenSwitchingNotifyCallbackStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_TRUE(res == ERR_INVALID_STATE);
}

/**
 * @tc.name: OnRemoteRequest003
 * @tc.desc: Verify function OnRemoteRequest
 * @tc.type:FUNC
 */
HWTEST_F(RSScreenSwitchingNotifyCallbackStubTest, OnRemoteRequest003, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto rsScreenSwitchingNotifyCallbackStub = std::make_shared<RSScreenSwitchingNotifyCallbackStubMock>();
    ASSERT_NE(rsScreenSwitchingNotifyCallbackStub, nullptr);
    uint32_t code = static_cast<uint32_t>(RSIScreenSwitchingNotifyCallbackInterfaceCode::ON_SCREEN_SWITCHING_NOTIFY);
    int res = rsScreenSwitchingNotifyCallbackStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_TRUE(res == ERR_INVALID_STATE);
}

/**
 * @tc.name: OnRemoteRequest004
 * @tc.desc: Verify function OnRemoteRequest
 * @tc.type:FUNC
 */
HWTEST_F(RSScreenSwitchingNotifyCallbackStubTest, OnRemoteRequest004, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto rsScreenSwitchingNotifyCallbackStub = std::make_shared<RSScreenSwitchingNotifyCallbackStubMock>();
    ASSERT_NE(rsScreenSwitchingNotifyCallbackStub, nullptr);
    uint32_t code = std::numeric_limits<uint32_t>::max();
    data.WriteInterfaceToken(RSIScreenSwitchingNotifyCallback::GetDescriptor());
    int res = rsScreenSwitchingNotifyCallbackStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_TRUE(res != ERR_NONE);
}

} // namespace OHOS::Rosen
