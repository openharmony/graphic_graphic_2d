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

#include "ipc_callbacks/rs_frame_stability_callback_proxy.h"
#include "mock_iremote_object.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSFrameStabilityCallbackProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSFrameStabilityCallbackProxyTest::SetUpTestCase() {}
void RSFrameStabilityCallbackProxyTest::TearDownTestCase() {}
void RSFrameStabilityCallbackProxyTest::SetUp() {}
void RSFrameStabilityCallbackProxyTest::TearDown() {}

/**
 * @tc.name: OnFrameStabilityChanged001
 * @tc.desc: Verify OnFrameStabilityChanged with valid remote object
 * @tc.type:FUNC
 * @tc.require: issues22734
 */
HWTEST_F(RSFrameStabilityCallbackProxyTest, OnFrameStabilityChanged001, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteMocker = new MockIRemoteObject();
    ASSERT_TRUE(remoteMocker != nullptr);
    auto frameStabilityCallbackProxy = std::make_shared<RSFrameStabilityCallbackProxy>(remoteMocker);
    ASSERT_TRUE(frameStabilityCallbackProxy != nullptr);
    frameStabilityCallbackProxy->OnFrameStabilityChanged(true);
    ASSERT_EQ(remoteMocker->receivedCode_,
              static_cast<uint32_t>(RSIFrameStabilityCallbackInterfaceCode::ON_FRAME_STABILITY_CHANGED));
}

/**
 * @tc.name: OnFrameStabilityChanged002
 * @tc.desc: Verify OnFrameStabilityChanged with SendRequest failure
 * @tc.type:FUNC
 * @tc.require: issues22734
 */
HWTEST_F(RSFrameStabilityCallbackProxyTest, OnFrameStabilityChanged002, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteMocker = new MockIRemoteObject();
    ASSERT_TRUE(remoteMocker != nullptr);
    remoteMocker->sendRequestResult_ = 1;
    auto frameStabilityCallbackProxy = std::make_shared<RSFrameStabilityCallbackProxy>(remoteMocker);
    ASSERT_TRUE(frameStabilityCallbackProxy != nullptr);
    frameStabilityCallbackProxy->OnFrameStabilityChanged(false);
    ASSERT_EQ(remoteMocker->receivedCode_,
              static_cast<uint32_t>(RSIFrameStabilityCallbackInterfaceCode::ON_FRAME_STABILITY_CHANGED));
}

/**
 * @tc.name: OnFrameStabilityChanged003
 * @tc.desc: Verify OnFrameStabilityChanged with null remote object
 * @tc.type:FUNC
 * @tc.require: issues22734
 */
HWTEST_F(RSFrameStabilityCallbackProxyTest, OnFrameStabilityChanged003, TestSize.Level1)
{
    auto frameStabilityCallbackProxy = std::make_shared<RSFrameStabilityCallbackProxy>(nullptr);
    ASSERT_TRUE(frameStabilityCallbackProxy != nullptr);
    frameStabilityCallbackProxy->OnFrameStabilityChanged(true);
}
} // namespace Rosen
} // namespace OHOS
