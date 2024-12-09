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

#include "ipc_callbacks/rs_occlusion_change_callback_stub.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSCallbackStubOcclusionChangeCallbackStubMock : public RSOcclusionChangeCallbackStub {
public:
    RSCallbackStubOcclusionChangeCallbackStubMock() = default;
    virtual ~RSCallbackStubOcclusionChangeCallbackStubMock() = default;
    void OnOcclusionVisibleChanged(std::shared_ptr<RSOcclusionData> occlusionData) override {};
};

class RSOcclusionChangeCallbackStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSOcclusionChangeCallbackStubTest::SetUpTestCase() {}
void RSOcclusionChangeCallbackStubTest::TearDownTestCase() {}
void RSOcclusionChangeCallbackStubTest::SetUp() {}
void RSOcclusionChangeCallbackStubTest::TearDown() {}

/**
 * @tc.name: OnRemoteRequest001
 * @tc.desc: Verify function OnRemoteRequest
 * @tc.type:FUNC
 * @tc.require: issueIAKP5Y
 */
HWTEST_F(RSOcclusionChangeCallbackStubTest, OnRemoteRequest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto rsOcclusionChangeCallbackStub = std::make_shared<RSCallbackStubOcclusionChangeCallbackStubMock>();
    uint32_t code = -1;
    int res = rsOcclusionChangeCallbackStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_TRUE(res == ERR_INVALID_STATE);

    res = rsOcclusionChangeCallbackStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_TRUE(res == ERR_INVALID_STATE);
    code = static_cast<uint32_t>(RSIOcclusionChangeCallbackInterfaceCode::ON_OCCLUSION_VISIBLE_CHANGED);
}

/**
 * @tc.name: OnRemoteRequest002
 * @tc.desc: Verify function OnRemoteRequest
 * @tc.type:FUNC
 */
HWTEST_F(RSOcclusionChangeCallbackStubTest, OnRemoteRequest003, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto rsOcclusionChangeCallbackStub = std::make_shared<RSCallbackStubOcclusionChangeCallbackStubMock>();
    uint32_t code = 1;
    data.WriteInterfaceToken(RSIOcclusionChangeCallback::GetDescriptor());
    int res = rsOcclusionChangeCallbackStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_TRUE(res != ERR_NONE);
}

/**
 * @tc.name: OnRemoteRequest003
 * @tc.desc: Verify function OnRemoteRequest
 * @tc.type:FUNC
 * @tc.require: issueIAKP5Y
 */
HWTEST_F(RSOcclusionChangeCallbackStubTest, OnRemoteRequest002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto rsOcclusionChangeCallbackStub = std::make_shared<RSCallbackStubOcclusionChangeCallbackStubMock>();
    auto code = static_cast<uint32_t>(RSIOcclusionChangeCallbackInterfaceCode::ON_OCCLUSION_VISIBLE_CHANGED);
    data.WriteInterfaceToken(RSIOcclusionChangeCallback::GetDescriptor());

    int res = rsOcclusionChangeCallbackStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_TRUE(res == ERR_NONE);
}
} // namespace OHOS::Rosen
