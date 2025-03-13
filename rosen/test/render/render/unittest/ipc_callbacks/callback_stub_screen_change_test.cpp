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

#include "ipc_callbacks/screen_change_callback_stub.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSScreenCallbackStubChangeMock : public RSScreenChangeCallbackStub {
public:
    RSScreenCallbackStubChangeMock() = default;
    virtual ~RSScreenCallbackStubChangeMock() = default;
    void OnScreenChanged(ScreenId id, ScreenEvent event,
        ScreenChangeReason reason) override {};
};

class RSScreenChangeCallbackStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSScreenChangeCallbackStubTest::SetUpTestCase() {}
void RSScreenChangeCallbackStubTest::TearDownTestCase() {}
void RSScreenChangeCallbackStubTest::SetUp() {}
void RSScreenChangeCallbackStubTest::TearDown() {}

/**
 * @tc.name: OnRemoteRequest001
 * @tc.desc: Verify function OnRemoteRequest
 * @tc.type:FUNC
 */
HWTEST_F(RSScreenChangeCallbackStubTest, OnRemoteRequest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto rsScreenChangeCallbackStub = std::make_shared<RSScreenCallbackStubChangeMock>();
    ASSERT_NE(rsScreenChangeCallbackStub, nullptr);
    auto code = static_cast<uint32_t>(RSIScreenChangeCallbackInterfaceCode::ON_SCREEN_CHANGED);
    data.WriteInterfaceToken(RSIScreenChangeCallback::GetDescriptor());
    int res = rsScreenChangeCallbackStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_TRUE(res == ERR_NONE);
}

/**
 * @tc.name: OnRemoteRequest002
 * @tc.desc: Verify function OnRemoteRequest
 * @tc.type:FUNC
 */
HWTEST_F(RSScreenChangeCallbackStubTest, OnRemoteRequest002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto rsScreenChangeCallbackStub = std::make_shared<RSScreenCallbackStubChangeMock>();
    ASSERT_NE(rsScreenChangeCallbackStub, nullptr);
    uint32_t code = std::numeric_limits<uint32_t>::max();
    int res = rsScreenChangeCallbackStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_TRUE(res == ERR_INVALID_STATE);
}

/**
 * @tc.name: OnRemoteRequest003
 * @tc.desc: Verify function OnRemoteRequest
 * @tc.type:FUNC
 */
HWTEST_F(RSScreenChangeCallbackStubTest, OnRemoteRequest003, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto rsScreenChangeCallbackStub = std::make_shared<RSScreenCallbackStubChangeMock>();
    ASSERT_NE(rsScreenChangeCallbackStub, nullptr);
    uint32_t code = static_cast<uint32_t>(RSIScreenChangeCallbackInterfaceCode::ON_SCREEN_CHANGED);
    int res = rsScreenChangeCallbackStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_TRUE(res == ERR_INVALID_STATE);
}

/**
 * @tc.name: OnRemoteRequest004
 * @tc.desc: Verify function OnRemoteRequest
 * @tc.type:FUNC
 */
HWTEST_F(RSScreenChangeCallbackStubTest, OnRemoteRequest004, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto rsScreenChangeCallbackStub = std::make_shared<RSScreenCallbackStubChangeMock>();
    ASSERT_NE(rsScreenChangeCallbackStub, nullptr);
    uint32_t code = std::numeric_limits<uint32_t>::max();
    data.WriteInterfaceToken(RSIScreenChangeCallback::GetDescriptor());
    int res = rsScreenChangeCallbackStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_TRUE(res != ERR_NONE);
}

} // namespace OHOS::Rosen
