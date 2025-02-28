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

#include "ipc_callbacks/surface_capture_callback_proxy.h"
#include "mock_iremote_object.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSSurfaceCaptureCallbackProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSurfaceCaptureCallbackProxyTest::SetUpTestCase() {}
void RSSurfaceCaptureCallbackProxyTest::TearDownTestCase() {}
void RSSurfaceCaptureCallbackProxyTest::SetUp() {}
void RSSurfaceCaptureCallbackProxyTest::TearDown() {}

/**
 * @tc.name: OnSurfaceCapture001
 * @tc.desc: Verify the OnSurfaceCapture
 * @tc.type:FUNC
 * @tc.require: issueIB2AHG
 */
HWTEST_F(RSSurfaceCaptureCallbackProxyTest, OnSurfaceCapture001, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteMocker = new MockIRemoteObject();
    ASSERT_TRUE(remoteMocker != nullptr);
    auto rsSurfaceCaptureCallbackProxy = std::make_shared<RSSurfaceCaptureCallbackProxy>(remoteMocker);
    ASSERT_TRUE(rsSurfaceCaptureCallbackProxy != nullptr);
    RSSurfaceCaptureConfig captureConfig;
    std::shared_ptr<Media::PixelMap> pixelMap = std::make_shared<Media::PixelMap>();
    rsSurfaceCaptureCallbackProxy->OnSurfaceCapture(1, captureConfig, pixelMap.get());
    ASSERT_EQ(
        remoteMocker->receivedCode_, static_cast<uint32_t>(RSISurfaceCaptureCallbackInterfaceCode::ON_SURFACE_CAPTURE));
}

/**
 * @tc.name: OnSurfaceCapture002
 * @tc.desc: Verify the OnSurfaceCapture error case
 * @tc.type:FUNC
 * @tc.require: issueIB2AHG
 */
HWTEST_F(RSSurfaceCaptureCallbackProxyTest, OnSurfaceCapture002, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteMocker = new MockIRemoteObject();
    ASSERT_TRUE(remoteMocker != nullptr);
    remoteMocker->sendRequestResult_ = 1;
    auto rsSurfaceCaptureCallbackProxy = std::make_shared<RSSurfaceCaptureCallbackProxy>(remoteMocker);
    ASSERT_TRUE(rsSurfaceCaptureCallbackProxy != nullptr);
    RSSurfaceCaptureConfig captureConfig;
    std::shared_ptr<Media::PixelMap> pixelMap = std::make_shared<Media::PixelMap>();
    rsSurfaceCaptureCallbackProxy->OnSurfaceCapture(1, captureConfig, pixelMap.get());
    ASSERT_EQ(
        remoteMocker->receivedCode_, static_cast<uint32_t>(RSISurfaceCaptureCallbackInterfaceCode::ON_SURFACE_CAPTURE));
}

/**
 * @tc.name: WriteSurfaceCaptureConfig
 * @tc.desc: Verify the WriteSurfaceCaptureConfig normal case
 * @tc.type:FUNC
 * @tc.require: issueIBOVQL
 */
HWTEST_F(RSSurfaceCaptureCallbackProxyTest, WriteSurfaceCaptureConfig, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteMocker = new MockIRemoteObject();
    ASSERT_TRUE(remoteMocker != nullptr);
    remoteMocker->sendRequestResult_ = 1;
    auto rsSurfaceCaptureCallbackProxy = std::make_shared<RSSurfaceCaptureCallbackProxy>(remoteMocker);
    ASSERT_TRUE(rsSurfaceCaptureCallbackProxy != nullptr);
    MessageParcel data;
    RSSurfaceCaptureConfig captureConfig;
    ASSERT_EQ(rsSurfaceCaptureCallbackProxy->WriteSurfaceCaptureConfig(captureConfig, data), true);
}
} // namespace Rosen
} // namespace OHOS
