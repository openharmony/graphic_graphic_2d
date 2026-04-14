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

#include "ipc_callbacks/rs_exposed_event_callback_stub.h"
#include "ipc_callbacks/rs_iexposed_event_callback_ipc_interface_code.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSExposedEventCallbackStubMock : public RSExposedEventCallbackStub {
public:
    RSExposedEventCallbackStubMock() = default;
    virtual ~RSExposedEventCallbackStubMock() = default;
    void OnDisplayEvent(const std::shared_ptr<RSExposedEventDataBase> data) override {};
};

class RSExposedEventCallbackStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSExposedEventCallbackStubTest::SetUpTestCase() {}
void RSExposedEventCallbackStubTest::TearDownTestCase() {}
void RSExposedEventCallbackStubTest::SetUp() {}
void RSExposedEventCallbackStubTest::TearDown() {}

/**
 * @tc.name: OnRemoteRequest001
 * @tc.desc: Verify OnRemoteRequest with valid ON_EXPOSED_EVENT code
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSExposedEventCallbackStubTest, OnRemoteRequest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto stub = std::make_shared<RSExposedEventCallbackStubMock>();
    ASSERT_NE(stub, nullptr);
    auto code = static_cast<uint32_t>(RSIExposedEventCallbackInterfaceCode::ON_EXPOSED_EVENT);
    data.WriteInterfaceToken(RSIExposedEventCallback::GetDescriptor());
    int res = stub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_NONE);
}

/**
 * @tc.name: OnRemoteRequest002
 * @tc.desc: Verify OnRemoteRequest with invalid interface token
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSExposedEventCallbackStubTest, OnRemoteRequest002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto stub = std::make_shared<RSExposedEventCallbackStubMock>();
    ASSERT_NE(stub, nullptr);
    auto code = static_cast<uint32_t>(RSIExposedEventCallbackInterfaceCode::ON_EXPOSED_EVENT);
    data.WriteInterfaceToken(u"invalid.token");
    int res = stub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_STATE);
}

/**
 * @tc.name: OnRemoteRequest003
 * @tc.desc: Verify OnRemoteRequest with unknown code
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSExposedEventCallbackStubTest, OnRemoteRequest003, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto stub = std::make_shared<RSExposedEventCallbackStubMock>();
    ASSERT_NE(stub, nullptr);
    uint32_t code = std::numeric_limits<uint32_t>::max();
    data.WriteInterfaceToken(RSIExposedEventCallback::GetDescriptor());
    int res = stub->OnRemoteRequest(code, data, reply, option);
    EXPECT_NE(res, ERR_NONE);
}

/**
 * @tc.name: OnRemoteRequest004
 * @tc.desc: Verify OnRemoteRequest without interface token
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSExposedEventCallbackStubTest, OnRemoteRequest004, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto stub = std::make_shared<RSExposedEventCallbackStubMock>();
    ASSERT_NE(stub, nullptr);
    auto code = static_cast<uint32_t>(RSIExposedEventCallbackInterfaceCode::ON_EXPOSED_EVENT);
    int res = stub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, ERR_INVALID_STATE);
}

} // namespace OHOS::Rosen
