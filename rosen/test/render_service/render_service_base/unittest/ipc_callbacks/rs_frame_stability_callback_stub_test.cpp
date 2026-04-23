/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with * License.
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

#include "ipc_callbacks/rs_frame_stability_callback_stub.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSFrameStabilityCallbackStubMock : public RSFrameStabilityCallbackStub {
public:
    RSFrameStabilityCallbackStubMock() = default;
    ~RSFrameStabilityCallbackStubMock() = default;
    void OnFrameStabilityChanged(bool isStable) override {};
};

class RSFrameStabilityCallbackStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSFrameStabilityCallbackStubTest::SetUpTestCase() {}
void RSFrameStabilityCallbackStubTest::TearDownTestCase() {}
void RSFrameStabilityCallbackStubTest::SetUp() {}
void RSFrameStabilityCallbackStubTest::TearDown() {}

/**
 * @tc.name: OnRemoteRequest001
 * @tc.desc: Verify function OnRemoteRequest with invalid code
 * @tc.type:FUNC
 * @tc.require: issues22734
 */
HWTEST_F(RSFrameStabilityCallbackStubTest, OnRemoteRequest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto rsFrameStabilityCallbackStub = std::make_shared<RSFrameStabilityCallbackStubMock>();
    uint32_t code = -1;
    int res = rsFrameStabilityCallbackStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_TRUE(res == ERR_INVALID_STATE);
    code = static_cast<uint32_t>(RSIFrameStabilityCallbackInterfaceCode::ON_FRAME_STABILITY_CHANGED);

    res = rsFrameStabilityCallbackStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_TRUE(res == ERR_INVALID_STATE);
}

/**
 * @tc.name: OnRemoteRequest002
 * @tc.desc: Verify function OnRemoteRequest with valid code and token
 * @tc.type:FUNC
 * @tc.require: issues22734
 */
HWTEST_F(RSFrameStabilityCallbackStubTest, OnRemoteRequest002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto rsFrameStabilityCallbackStub = std::make_shared<RSFrameStabilityCallbackStubMock>();
    auto code = static_cast<uint32_t>(RSIFrameStabilityCallbackInterfaceCode::ON_FRAME_STABILITY_CHANGED);
    data.WriteInterfaceToken(RSIFrameStabilityCallback::GetDescriptor());

    int res = rsFrameStabilityCallbackStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_TRUE(res == ERR_INVALID_DATA);

    MessageParcel dataHasBool;
    dataHasBool.WriteInterfaceToken(RSIFrameStabilityCallback::GetDescriptor());
    dataHasBool.WriteBool(true);
    res = rsFrameStabilityCallbackStub->OnRemoteRequest(code, dataHasBool, reply, option);
    EXPECT_TRUE(res == ERR_NONE);
}

/**
 * @tc.name: OnRemoteRequest003
 * @tc.desc: Verify function OnRemoteRequest with unknown code
 * @tc.type:FUNC
 * @tc.require: issues22734
 */
HWTEST_F(RSFrameStabilityCallbackStubTest, OnRemoteRequest003, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto rsFrameStabilityCallbackStub = std::make_shared<RSFrameStabilityCallbackStubMock>();
    uint32_t code = 1;
    data.WriteInterfaceToken(RSIFrameStabilityCallback::GetDescriptor());
    int res = rsFrameStabilityCallbackStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_TRUE(res != ERR_NONE);
}
} // namespace OHOS::Rosen
