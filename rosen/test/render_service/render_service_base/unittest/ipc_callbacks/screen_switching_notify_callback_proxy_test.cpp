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

#include "ipc_callbacks/screen_switching_notify_callback_proxy.h"
#include "mock_iremote_object.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSScreenSwitchingNotifyCallbackProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSScreenSwitchingNotifyCallbackProxyTest::SetUpTestCase() {}
void RSScreenSwitchingNotifyCallbackProxyTest::TearDownTestCase() {}
void RSScreenSwitchingNotifyCallbackProxyTest::SetUp() {}
void RSScreenSwitchingNotifyCallbackProxyTest::TearDown() {}
 
/**
 * @tc.name: OnScreenSwitchingNotify001
 * @tc.desc: Verify the OnScreenSwitchingNotify
 * @tc.type:FUNC
 */
HWTEST_F(RSScreenSwitchingNotifyCallbackProxyTest, OnScreenSwitchingNotify001, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteMocker = new MockIRemoteObject();
    ASSERT_TRUE(remoteMocker != nullptr);
    auto screenSwitchingNotifyCallbackProxy = std::make_shared<RSScreenSwitchingNotifyCallbackProxy>(remoteMocker);
    ASSERT_TRUE(screenSwitchingNotifyCallbackProxy != nullptr);
    screenSwitchingNotifyCallbackProxy->OnScreenSwitchingNotify(false);
    ASSERT_EQ(remoteMocker->receivedCode_,
            static_cast<uint32_t>(RSIScreenSwitchingNotifyCallbackInterfaceCode::ON_SCREEN_SWITCHING_NOTIFY));
}
 
/**
 * @tc.name: OnScreenSwitchingNotify002
 * @tc.desc: Verify the OnScreenSwitchingNotify
 * @tc.type:FUNC
 */
HWTEST_F(RSScreenSwitchingNotifyCallbackProxyTest, OnScreenSwitchingNotify002, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteMocker = new MockIRemoteObject();
    ASSERT_TRUE(remoteMocker != nullptr);
    remoteMocker->SetErrorStatus(UNKNOWN_ERROR);
    auto screenSwitchingNotifyCallbackProxy = std::make_shared<RSScreenSwitchingNotifyCallbackProxy>(remoteMocker);
    ASSERT_TRUE(screenSwitchingNotifyCallbackProxy != nullptr);
    screenSwitchingNotifyCallbackProxy->OnScreenSwitchingNotify(false);
    ASSERT_EQ(remoteMocker->receivedCode_,
            static_cast<uint32_t>(RSIScreenSwitchingNotifyCallbackInterfaceCode::ON_SCREEN_SWITCHING_NOTIFY));
}

/**
 * @tc.name: OnScreenSwitchingNotify003
 * @tc.desc: Verify the OnScreenSwitchingNotify
 * @tc.type:FUNC
 */
HWTEST_F(RSScreenSwitchingNotifyCallbackProxyTest, OnScreenSwitchingNotify003, TestSize.Level1)
{
    auto screenSwitchingNotifyCallbackProxy = std::make_shared<RSScreenSwitchingNotifyCallbackProxy>(nullptr);
    ASSERT_TRUE(screenSwitchingNotifyCallbackProxy != nullptr);
    screenSwitchingNotifyCallbackProxy->OnScreenSwitchingNotify(false);
}

} // namespace Rosen
} // namespace OHOS
