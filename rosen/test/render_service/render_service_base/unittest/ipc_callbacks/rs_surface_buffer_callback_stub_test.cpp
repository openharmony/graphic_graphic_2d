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

#include "ipc_callbacks/rs_surface_buffer_callback_stub.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSSurfaceBufferCallbackStubMock : public RSSurfaceBufferCallbackStub {
public:
    RSSurfaceBufferCallbackStubMock() = default;
    virtual ~RSSurfaceBufferCallbackStubMock() = default;
    void OnFinish(const FinishCallbackRet& ret) override {}
    void OnAfterAcquireBuffer(const AfterAcquireBufferRet& ret) override {}
};

class RSSurfaceBufferCallbackStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSurfaceBufferCallbackStubTest::SetUpTestCase() {}
void RSSurfaceBufferCallbackStubTest::TearDownTestCase() {}
void RSSurfaceBufferCallbackStubTest::SetUp() {}
void RSSurfaceBufferCallbackStubTest::TearDown() {}

/**
 * @tc.name: OnRemoteRequest001
 * @tc.desc: Verify function OnRemoteRequest
 * @tc.type:FUNC
 * @tc.require: issueIB2AHG
 */
HWTEST_F(RSSurfaceBufferCallbackStubTest, OnRemoteRequest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto rsSurfaceBufferCallbackStubMock = std::make_shared<RSSurfaceBufferCallbackStubMock>();
    uint32_t code = -1;
    int res = rsSurfaceBufferCallbackStubMock->OnRemoteRequest(code, data, reply, option);
    EXPECT_TRUE(res == ERR_INVALID_STATE);
    code = static_cast<uint32_t>(RSISurfaceBufferCallbackInterfaceCode::ON_FINISH);

    res = rsSurfaceBufferCallbackStubMock->OnRemoteRequest(code, data, reply, option);
    EXPECT_TRUE(res == ERR_INVALID_STATE);
}

/**
 * @tc.name: OnRemoteRequest002
 * @tc.desc: Verify function OnRemoteRequest
 * @tc.type:FUNC
 * @tc.require: issueIB2AHG
 */
HWTEST_F(RSSurfaceBufferCallbackStubTest, OnRemoteRequest002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto rsSurfaceBufferCallbackStubMock = std::make_shared<RSSurfaceBufferCallbackStubMock>();
    auto code = static_cast<uint32_t>(RSISurfaceBufferCallbackInterfaceCode::ON_FINISH);
    data.WriteInterfaceToken(RSISurfaceBufferCallback::GetDescriptor());

    int res = rsSurfaceBufferCallbackStubMock->OnRemoteRequest(code, data, reply, option);
    EXPECT_TRUE(res == ERR_NONE);
}

/**
 * @tc.name: OnRemoteRequest003
 * @tc.desc: Verify function OnRemoteRequest
 * @tc.type:FUNC
 * @tc.require: issueIB2AHG
 */
HWTEST_F(RSSurfaceBufferCallbackStubTest, OnRemoteRequest003, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto rsSurfaceBufferCallbackStubMock = std::make_shared<RSSurfaceBufferCallbackStubMock>();
    uint32_t code = -1;
    data.WriteInterfaceToken(RSISurfaceBufferCallback::GetDescriptor());

    int res = rsSurfaceBufferCallbackStubMock->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(res, IPC_STUB_UNKNOW_TRANS_ERR);
}
} // namespace OHOS::Rosen
