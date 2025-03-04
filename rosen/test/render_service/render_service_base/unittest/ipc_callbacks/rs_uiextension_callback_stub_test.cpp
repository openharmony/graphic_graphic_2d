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

#include "ipc_callbacks/rs_uiextension_callback_stub.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSUIExtensionCallbackStubMock : public RSUIExtensionCallbackStub {
public:
    RSUIExtensionCallbackStubMock() = default;
    virtual ~RSUIExtensionCallbackStubMock() = default;
    void OnUIExtension(std::shared_ptr<RSUIExtensionData> data, uint64_t userId) override {};
};

class RSUIExtensionCallbackStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSUIExtensionCallbackStubTest::SetUpTestCase() {}
void RSUIExtensionCallbackStubTest::TearDownTestCase() {}
void RSUIExtensionCallbackStubTest::SetUp() {}
void RSUIExtensionCallbackStubTest::TearDown() {}

/**
 * @tc.name: OnRemoteRequest001
 * @tc.desc: Verify function OnRemoteRequest
 * @tc.type:FUNC
 * @tc.require: issueIAKP5Y
 */
HWTEST_F(RSUIExtensionCallbackStubTest, OnRemoteRequest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto rsUiExtensionCallbackStub = std::make_shared<RSUIExtensionCallbackStubMock>();
    uint32_t code = -1;
    int res = rsUiExtensionCallbackStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_TRUE(res == ERR_INVALID_STATE);
    code = static_cast<uint32_t>(RSIUIExtensionCallbackInterfaceCode::ON_UIEXTENSION);

    res = rsUiExtensionCallbackStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_TRUE(res == ERR_INVALID_STATE);
}

/**
 * @tc.name: OnRemoteRequest002
 * @tc.desc: Verify function OnRemoteRequest
 * @tc.type:FUNC
 * @tc.require: issueIAKP5Y
 */
HWTEST_F(RSUIExtensionCallbackStubTest, OnRemoteRequest002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto rsUiExtensionCallbackStub = std::make_shared<RSUIExtensionCallbackStubMock>();
    auto code = static_cast<uint32_t>(RSIUIExtensionCallbackInterfaceCode::ON_UIEXTENSION);
    data.WriteInterfaceToken(RSIUIExtensionCallback::GetDescriptor());

    int res = rsUiExtensionCallbackStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_TRUE(res == ERR_INVALID_DATA);
    res = rsUiExtensionCallbackStub->OnRemoteRequest(-1, data, reply, option);
    EXPECT_TRUE(res == ERR_INVALID_STATE);
}

} // namespace OHOS::Rosen
