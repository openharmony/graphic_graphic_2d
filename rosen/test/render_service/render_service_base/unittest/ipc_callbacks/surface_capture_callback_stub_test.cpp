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

#include "ipc_callbacks/surface_capture_callback_stub.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSSurfaceCaptureCallbackStubMock : public RSSurfaceCaptureCallbackStub {
public:
    RSSurfaceCaptureCallbackStubMock() = default;
    virtual ~RSSurfaceCaptureCallbackStubMock() = default;
    void OnSurfaceCapture(NodeId id, const RSSurfaceCaptureConfig& captureConfig,
        Media::PixelMap* pixelmap, Media::PixelMap* pixelmapHDR = nullptr) override {};
};

class RSSurfaceCaptureCallbackStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

private:
    static sptr<RSSurfaceCaptureCallbackStubMock> stub;
};

sptr<RSSurfaceCaptureCallbackStubMock> RSSurfaceCaptureCallbackStubTest::stub =
    new RSSurfaceCaptureCallbackStubMock();

void RSSurfaceCaptureCallbackStubTest::SetUpTestCase() {}
void RSSurfaceCaptureCallbackStubTest::TearDownTestCase()
{
    stub = nullptr;
}
void RSSurfaceCaptureCallbackStubTest::SetUp() {}
void RSSurfaceCaptureCallbackStubTest::TearDown() {}

/**
 * @tc.name: OnRemoteRequest001
 * @tc.desc: Verify function OnRemoteRequest if code exist and data has no content
 * @tc.type:FUNC
 * @tc.require: issueIAKP5Y
 */
HWTEST_F(RSSurfaceCaptureCallbackStubTest, OnRemoteRequest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    uint32_t code = static_cast<uint32_t>(RSISurfaceCaptureCallbackInterfaceCode::ON_SURFACE_CAPTURE);

    int res = stub->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, ERR_INVALID_STATE);
}

/**
 * @tc.name: OnRemoteRequest002
 * @tc.desc: Verify function OnRemoteRequest if code does not exist and data has content
 * @tc.type:FUNC
 * @tc.require: issueIAKP5Y
 */
HWTEST_F(RSSurfaceCaptureCallbackStubTest, OnRemoteRequest002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    uint32_t code = -1;
    data.WriteInterfaceToken(RSISurfaceCaptureCallback::GetDescriptor());

    int res = stub->OnRemoteRequest(code, data, reply, option);
    ASSERT_EQ(res, IPC_STUB_UNKNOW_TRANS_ERR);
}

/**
 * @tc.name: ReadSurfaceCaptureConfig
 * @tc.desc: Verify the ReadSurfaceCaptureConfig error case
 * @tc.type:FUNC
 * @tc.require: issueIBOVQL
 */
HWTEST_F(RSSurfaceCaptureCallbackStubTest, ReadSurfaceCaptureConfig, TestSize.Level1)
{
    MessageParcel data;
    RSSurfaceCaptureConfig captureConfig;
    int res = stub->ReadSurfaceCaptureConfig(captureConfig, data);
    ASSERT_EQ(res, false);
}

} // namespace OHOS::Rosen
