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
#include "ipc_callbacks/hgm_config_change_callback_proxy.h"
#include "mock_iremoter_objects.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class CallbackProxyHGMConfigChangeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void CallbackProxyHGMConfigChangeTest::SetUpTestCase() {}
void CallbackProxyHGMConfigChangeTest::TearDownTestCase() {}
void CallbackProxyHGMConfigChangeTest::SetUp() {}
void CallbackProxyHGMConfigChangeTest::TearDown() {}

/**
 * @tc.name: OnHgmConfigChanged001
 * @tc.desc: Verify function OnHgmConfigChanged
 * @tc.type: FUNC
 */
HWTEST_F(CallbackProxyHGMConfigChangeTest, OnHgmConfigChanged001, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteObject = new MockIRemoteObject();
    ASSERT_EQ(static_cast<uint32_t>(RSIHgmConfigChangeCallbackInterfaceCode::ON_HGM_CONFIG_CHANGED),
        remoteObject->receivedCode_);
    ASSERT_NE(remoteObject, nullptr);
    auto rsHgmConfigChangeCallbackProxy = std::make_shared<RSHgmConfigChangeCallbackProxy>(remoteObject);
    ASSERT_NE(rsHgmConfigChangeCallbackProxy, nullptr);
    rsHgmConfigChangeCallbackProxy->OnHgmConfigChanged(std::make_shared<RSHgmConfigData>());
}

/**
 * @tc.name: OnHgmConfigChanged002
 * @tc.desc: Verify function OnHgmConfigChanged
 * @tc.type: FUNC
 */
HWTEST_F(CallbackProxyHGMConfigChangeTest, OnHgmConfigChanged002, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteObject = new MockIRemoteObject();
    ASSERT_NE(remoteObject, nullptr);
    remoteObject->sendRequestResult_ = 1;
    auto rsHgmConfigChangeCallbackProxy = std::make_shared<RSHgmConfigChangeCallbackProxy>(remoteObject);
    ASSERT_NE(rsHgmConfigChangeCallbackProxy, nullptr);
    rsHgmConfigChangeCallbackProxy->OnHgmConfigChanged(std::make_shared<RSHgmConfigData>());
    ASSERT_EQ(static_cast<uint32_t>(RSIHgmConfigChangeCallbackInterfaceCode::ON_HGM_CONFIG_CHANGED),
        remoteObject->receivedCode_);
}

/**
 * @tc.name: OnHgmConfigChanged003
 * @tc.desc: Verify function OnHgmConfigChanged
 * @tc.type: FUNC
 */
HWTEST_F(CallbackProxyHGMConfigChangeTest, OnHgmConfigChanged003, TestSize.Level1)
{
    auto rsHgmConfigChangeCallbackProxy = std::make_shared<RSHgmConfigChangeCallbackProxy>(nullptr);
    ASSERT_NE(rsHgmConfigChangeCallbackProxy, nullptr);
    rsHgmConfigChangeCallbackProxy->OnHgmConfigChanged(std::make_shared<RSHgmConfigData>());
}

/**
 * @tc.name: OnHgmRefreshRateModeChanged001
 * @tc.desc: Verify function OnHgmRefreshRateModeChanged
 * @tc.type: FUNC
 */
HWTEST_F(CallbackProxyHGMConfigChangeTest, OnHgmRefreshRateModeChanged001, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteObject = new MockIRemoteObject();
    ASSERT_NE(remoteObject, nullptr);
    auto rsHgmConfigChangeCallbackProxy = std::make_shared<RSHgmConfigChangeCallbackProxy>(remoteObject);
    ASSERT_NE(rsHgmConfigChangeCallbackProxy, nullptr);
    int32_t refreshRateMode = 60;
    rsHgmConfigChangeCallbackProxy->OnHgmRefreshRateModeChanged(refreshRateMode);
    ASSERT_EQ(static_cast<uint32_t>(RSIHgmConfigChangeCallbackInterfaceCode::ON_HGM_REFRESH_RATE_MODE_CHANGED),
        remoteObject->receivedCode_);
}

/**
 * @tc.name: OnHgmRefreshRateModeChanged002
 * @tc.desc: Verify function OnHgmRefreshRateModeChanged
 * @tc.type: FUNC
 */
HWTEST_F(CallbackProxyHGMConfigChangeTest, OnHgmRefreshRateModeChanged002, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteObject = new MockIRemoteObject();
    ASSERT_NE(remoteObject, nullptr);
    remoteObject->sendRequestResult_ = 1;
    auto rsHgmConfigChangeCallbackProxy = std::make_shared<RSHgmConfigChangeCallbackProxy>(remoteObject);
    ASSERT_NE(rsHgmConfigChangeCallbackProxy, nullptr);
    int32_t refreshRateMode = 60;
    rsHgmConfigChangeCallbackProxy->OnHgmRefreshRateModeChanged(refreshRateMode);
    ASSERT_EQ(static_cast<uint32_t>(RSIHgmConfigChangeCallbackInterfaceCode::ON_HGM_REFRESH_RATE_MODE_CHANGED),
        remoteObject->receivedCode_);
}

/**
 * @tc.name: OnHgmRefreshRateModeChanged003
 * @tc.desc: Verify function OnHgmRefreshRateModeChanged
 * @tc.type: FUNC
 */
HWTEST_F(CallbackProxyHGMConfigChangeTest, OnHgmRefreshRateModeChanged003, TestSize.Level1)
{
    auto rsHgmConfigChangeCallbackProxy = std::make_shared<RSHgmConfigChangeCallbackProxy>(nullptr);
    ASSERT_NE(rsHgmConfigChangeCallbackProxy, nullptr);
    int32_t refreshRateMode = 60;
    rsHgmConfigChangeCallbackProxy->OnHgmRefreshRateModeChanged(refreshRateMode);
}

/**
 * @tc.name: OnHgmRefreshRateUpdate001
 * @tc.desc: Verify function OnHgmRefreshRateUpdate
 * @tc.type: FUNC
 */
HWTEST_F(CallbackProxyHGMConfigChangeTest, OnHgmRefreshRateUpdate001, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteObject = new MockIRemoteObject();
    ASSERT_NE(remoteObject, nullptr);
    auto rsHgmConfigChangeCallbackProxy = std::make_shared<RSHgmConfigChangeCallbackProxy>(remoteObject);
    ASSERT_NE(rsHgmConfigChangeCallbackProxy, nullptr);
    int32_t refreshRate = 32;
    rsHgmConfigChangeCallbackProxy->OnHgmRefreshRateUpdate(refreshRate);
    ASSERT_EQ(static_cast<uint32_t>(RSIHgmConfigChangeCallbackInterfaceCode::ON_HGM_REFRESH_RATE_CHANGED),
        remoteObject->receivedCode_);
}

/**
 * @tc.name: OnHgmRefreshRateUpdate002
 * @tc.desc: Verify function OnHgmRefreshRateUpdate
 * @tc.type: FUNC
 */
HWTEST_F(CallbackProxyHGMConfigChangeTest, OnHgmRefreshRateUpdate002, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteObject = new MockIRemoteObject();
    ASSERT_NE(remoteObject, nullptr);
    remoteObject->sendRequestResult_ = 1;
    auto rsHgmConfigChangeCallbackProxy = std::make_shared<RSHgmConfigChangeCallbackProxy>(remoteObject);
    ASSERT_NE(rsHgmConfigChangeCallbackProxy, nullptr);
    int32_t refreshRate = 32;
    rsHgmConfigChangeCallbackProxy->OnHgmRefreshRateUpdate(refreshRate);
    ASSERT_EQ(static_cast<uint32_t>(RSIHgmConfigChangeCallbackInterfaceCode::ON_HGM_REFRESH_RATE_CHANGED),
        remoteObject->receivedCode_);
}

/**
 * @tc.name: OnHgmRefreshRateUpdate003
 * @tc.desc: Verify function OnHgmRefreshRateUpdate
 * @tc.type: FUNC
 */
HWTEST_F(CallbackProxyHGMConfigChangeTest, OnHgmRefreshRateUpdate003, TestSize.Level1)
{
    auto rsHgmConfigChangeCallbackProxy = std::make_shared<RSHgmConfigChangeCallbackProxy>(nullptr);
    ASSERT_NE(rsHgmConfigChangeCallbackProxy, nullptr);
    int32_t refreshRate = 32;
    rsHgmConfigChangeCallbackProxy->OnHgmRefreshRateUpdate(refreshRate);
}

} // namespace OHOS::Rosen
