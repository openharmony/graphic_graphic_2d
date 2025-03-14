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
#include "ipc_callbacks/rs_first_frame_callback_proxy.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSFirstFrameCallbackProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSFirstFrameCallbackProxyTest::SetUpTestCase() {}
void RSFirstFrameCallbackProxyTest::TearDownTestCase() {}
void RSFirstFrameCallbackProxyTest::SetUp() {}
void RSFirstFrameCallbackProxyTest::TearDown() {}

/**
 * @tc.name: OnPowerOnFirstFrame001
 * @tc.desc: Verify function OnPowerOnFirstFrame
 * @tc.type: FUNC
 */
HWTEST_F(RSFirstFrameCallbackProxyTest, OnPowerOnFirstFrame001, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteObject = new MockIRemoteObject();
    ASSERT_NE(remoteObject, nullptr);
    auto rsFirstFrameCallbackProxy = std::make_shared<RSFirstFrameCallbackProxy>(remoteObject);
    ASSERT_NE(rsFirstFrameCallbackProxy, nullptr);
    ScreenId screenId = 0;
    int64_t timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    rsFirstFrameCallbackProxy->OnPowerOnFirstFrame(screenId, timestamp);
    ASSERT_EQ(static_cast<uint32_t>(RSIFirstFrameCallbackInterfaceCode::ON_POWER_ON),
        remoteObject->receivedCode_);
}

/**
 * @tc.name: OnPowerOnFirstFrame002
 * @tc.desc: Verify function OnPowerOnFirstFrame
 * @tc.type: FUNC
 */
HWTEST_F(RSFirstFrameCallbackProxyTest, OnPowerOnFirstFrame002, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteObject = new MockIRemoteObject();
    ASSERT_NE(remoteObject, nullptr);
    remoteObject->sendRequestResult_ = 1;
    auto rsFirstFrameCallbackProxy = std::make_shared<RSFirstFrameCallbackProxy>(remoteObject);
    ASSERT_NE(rsFirstFrameCallbackProxy, nullptr);
    ScreenId screenId = 0;
    int64_t timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    rsFirstFrameCallbackProxy->OnPowerOnFirstFrame(screenId, timestamp);
    ASSERT_EQ(static_cast<uint32_t>(RSIFirstFrameCallbackInterfaceCode::ON_POWER_ON),
        remoteObject->receivedCode_);
}

/**
 * @tc.name: OnPowerOnFirstFrame003
 * @tc.desc: Verify function OnPowerOnFirstFrame
 * @tc.type: FUNC
 */
HWTEST_F(RSFirstFrameCallbackProxyTest, OnPowerOnFirstFrame003, TestSize.Level1)
{
    auto rsFirstFrameCallbackProxy = std::make_shared<RSFirstFrameCallbackProxy>(nullptr);
    ASSERT_NE(rsFirstFrameCallbackProxy, nullptr);
    ScreenId screenId = 0;
    int64_t timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    rsFirstFrameCallbackProxy->OnPowerOnFirstFrame(screenId, timestamp);
}

} // namespace OHOS::Rosen
