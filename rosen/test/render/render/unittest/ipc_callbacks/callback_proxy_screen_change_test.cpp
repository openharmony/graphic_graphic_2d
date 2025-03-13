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

#include "ipc_callbacks/screen_change_callback_proxy.h"
#include "mock_iremoter_objects.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSScreenCallbackProxyChangeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSScreenCallbackProxyChangeTest::SetUpTestCase() {}
void RSScreenCallbackProxyChangeTest::TearDownTestCase() {}
void RSScreenCallbackProxyChangeTest::SetUp() {}
void RSScreenCallbackProxyChangeTest::TearDown() {}

/**
 * @tc.name: OnScreenChanged001
 * @tc.desc: Verify the OnScreenChanged
 * @tc.type:FUNC
 */
HWTEST_F(RSScreenCallbackProxyChangeTest, OnScreenChanged001, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteMocker = new MockIRemoteObject();
    ASSERT_TRUE(remoteMocker != nullptr);
    auto screenChangeCallbackProxy = std::make_shared<RSScreenChangeCallbackProxy>(remoteMocker);
    ASSERT_TRUE(screenChangeCallbackProxy != nullptr);
    screenChangeCallbackProxy->OnScreenChanged(INVALID_SCREEN_ID, ScreenEvent::UNKNOWN);
    ASSERT_EQ(remoteMocker->receivedCode_,
              static_cast<uint32_t>(RSIScreenChangeCallbackInterfaceCode::ON_SCREEN_CHANGED));
}

/**
 * @tc.name: OnScreenChanged002
 * @tc.desc: Verify the OnScreenChanged
 * @tc.type:FUNC
 */
HWTEST_F(RSScreenCallbackProxyChangeTest, OnScreenChanged002, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteMocker = new MockIRemoteObject();
    ASSERT_TRUE(remoteMocker != nullptr);
    remoteMocker->SetErrorStatus(UNKNOWN_ERROR);
    auto screenChangeCallbackProxy = std::make_shared<RSScreenChangeCallbackProxy>(remoteMocker);
    ASSERT_TRUE(screenChangeCallbackProxy != nullptr);
    screenChangeCallbackProxy->OnScreenChanged(INVALID_SCREEN_ID, ScreenEvent::CONNECTED);
    ASSERT_EQ(remoteMocker->receivedCode_,
              static_cast<uint32_t>(RSIScreenChangeCallbackInterfaceCode::ON_SCREEN_CHANGED));
}

/**
 * @tc.name: OnScreenChanged003
 * @tc.desc: Verify the OnScreenChanged
 * @tc.type:FUNC
 */
HWTEST_F(RSScreenCallbackProxyChangeTest, OnScreenChanged003, TestSize.Level1)
{
    auto screenChangeCallbackProxy = std::make_shared<RSScreenChangeCallbackProxy>(nullptr);
    ASSERT_TRUE(screenChangeCallbackProxy != nullptr);
    screenChangeCallbackProxy->OnScreenChanged(INVALID_SCREEN_ID, ScreenEvent::CONNECTED);
}

} // namespace Rosen
} // namespace OHOS
