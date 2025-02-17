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
#include "mock_iremote_object.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class HGMConfigChangeCallbackProxyUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void HGMConfigChangeCallbackProxyUnitTest::SetUpTestCase() {}
void HGMConfigChangeCallbackProxyUnitTest::TearDownTestCase() {}
void HGMConfigChangeCallbackProxyUnitTest::SetUp() {}
void HGMConfigChangeCallbackProxyUnitTest::TearDown() {}

/**
 * @tc.name: OnHgmConfigChanged001
 * @tc.desc: Verify function OnHgmConfigChanged
 * @tc.type: FUNC
 */
HWTEST_F(HGMConfigChangeCallbackProxyUnitTest, OnHgmConfigChanged001, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteObj = new MockIRemoteObject();
    ASSERT_NE(remoteObj, nullptr);
    auto rsHgmConfigChangeCallbackProxy = std::make_shared<RSHgmConfigChangeCallbackProxy>(remoteObj);
    ASSERT_NE(rsHgmConfigChangeCallbackProxy, nullptr);
    rsHgmConfigChangeCallbackProxy->OnHgmConfigChanged(std::make_shared<RSHgmConfigData>());
    ASSERT_EQ(static_cast<uint32_t>(RSIHgmConfigChangeCallbackInterfaceCode::ON_HGM_CONFIG_CHANGED),
        remoteObj->receivedCode_);
}

/**
 * @tc.name: OnHgmConfigChanged002
 * @tc.desc: Verify function OnHgmConfigChanged
 * @tc.type: FUNC
 */
HWTEST_F(HGMConfigChangeCallbackProxyUnitTest, OnHgmConfigChanged002, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteObj = new MockIRemoteObject();
    ASSERT_NE(remoteObj, nullptr);
    remoteObj->sendRequestResult_ = 1;
    auto rsHgmConfigChangeCallbackProxy = std::make_shared<RSHgmConfigChangeCallbackProxy>(remoteObj);
    ASSERT_NE(rsHgmConfigChangeCallbackProxy, nullptr);
    rsHgmConfigChangeCallbackProxy->OnHgmConfigChanged(std::make_shared<RSHgmConfigData>());
    ASSERT_EQ(static_cast<uint32_t>(RSIHgmConfigChangeCallbackInterfaceCode::ON_HGM_CONFIG_CHANGED),
        remoteObj->receivedCode_);
}

/**
 * @tc.name: OnHgmConfigChanged003
 * @tc.desc: Verify function OnHgmConfigChanged
 * @tc.type: FUNC
 */
HWTEST_F(HGMConfigChangeCallbackProxyUnitTest, OnHgmConfigChanged003, TestSize.Level1)
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
HWTEST_F(HGMConfigChangeCallbackProxyUnitTest, OnHgmRefreshRateModeChanged001, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteObj = new MockIRemoteObject();
    ASSERT_NE(remoteObj, nullptr);
    auto rsHgmConfigChangeCallbackProxy = std::make_shared<RSHgmConfigChangeCallbackProxy>(remoteObj);
    ASSERT_NE(rsHgmConfigChangeCallbackProxy, nullptr);
    int32_t refreshRateMode = 60;
    rsHgmConfigChangeCallbackProxy->OnHgmRefreshRateModeChanged(refreshRateMode);
    ASSERT_EQ(static_cast<uint32_t>(RSIHgmConfigChangeCallbackInterfaceCode::ON_HGM_REFRESH_RATE_MODE_CHANGED),
        remoteObj->receivedCode_);
}

/**
 * @tc.name: OnHgmRefreshRateModeChanged002
 * @tc.desc: Verify function OnHgmRefreshRateModeChanged
 * @tc.type: FUNC
 */
HWTEST_F(HGMConfigChangeCallbackProxyUnitTest, OnHgmRefreshRateModeChanged002, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteObj = new MockIRemoteObject();
    ASSERT_NE(remoteObj, nullptr);
    remoteObj->sendRequestResult_ = 1;
    auto rsHgmConfigChangeCallbackProxy = std::make_shared<RSHgmConfigChangeCallbackProxy>(remoteObj);
    ASSERT_NE(rsHgmConfigChangeCallbackProxy, nullptr);
    int32_t refreshRateMode = 60;
    rsHgmConfigChangeCallbackProxy->OnHgmRefreshRateModeChanged(refreshRateMode);
    ASSERT_EQ(static_cast<uint32_t>(RSIHgmConfigChangeCallbackInterfaceCode::ON_HGM_REFRESH_RATE_MODE_CHANGED),
        remoteObj->receivedCode_);
}

/**
 * @tc.name: OnHgmRefreshRateModeChanged003
 * @tc.desc: Verify function OnHgmRefreshRateModeChanged
 * @tc.type: FUNC
 */
HWTEST_F(HGMConfigChangeCallbackProxyUnitTest, OnHgmRefreshRateModeChanged003, TestSize.Level1)
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
HWTEST_F(HGMConfigChangeCallbackProxyUnitTest, OnHgmRefreshRateUpdate001, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteObj = new MockIRemoteObject();
    ASSERT_NE(remoteObj, nullptr);
    auto rsHgmConfigChangeCallbackProxy = std::make_shared<RSHgmConfigChangeCallbackProxy>(remoteObj);
    ASSERT_NE(rsHgmConfigChangeCallbackProxy, nullptr);
    int32_t refreshRate = 32;
    rsHgmConfigChangeCallbackProxy->OnHgmRefreshRateUpdate(refreshRate);
    ASSERT_EQ(static_cast<uint32_t>(RSIHgmConfigChangeCallbackInterfaceCode::ON_HGM_REFRESH_RATE_CHANGED),
        remoteObj->receivedCode_);
}

/**
 * @tc.name: OnHgmRefreshRateUpdate002
 * @tc.desc: Verify function OnHgmRefreshRateUpdate
 * @tc.type: FUNC
 */
HWTEST_F(HGMConfigChangeCallbackProxyUnitTest, OnHgmRefreshRateUpdate002, TestSize.Level1)
{
    sptr<MockIRemoteObject> remoteObj = new MockIRemoteObject();
    ASSERT_NE(remoteObj, nullptr);
    remoteObj->sendRequestResult_ = 1;
    auto rsHgmConfigChangeCallbackProxy = std::make_shared<RSHgmConfigChangeCallbackProxy>(remoteObj);
    ASSERT_NE(rsHgmConfigChangeCallbackProxy, nullptr);
    int32_t refreshRate = 32;
    rsHgmConfigChangeCallbackProxy->OnHgmRefreshRateUpdate(refreshRate);
    ASSERT_EQ(static_cast<uint32_t>(RSIHgmConfigChangeCallbackInterfaceCode::ON_HGM_REFRESH_RATE_CHANGED),
        remoteObj->receivedCode_);
}

/**
 * @tc.name: OnHgmRefreshRateUpdate003
 * @tc.desc: Verify function OnHgmRefreshRateUpdate
 * @tc.type: FUNC
 */
HWTEST_F(HGMConfigChangeCallbackProxyUnitTest, OnHgmRefreshRateUpdate003, TestSize.Level1)
{
    auto rsHgmConfigChangeCallbackProxy = std::make_shared<RSHgmConfigChangeCallbackProxy>(nullptr);
    ASSERT_NE(rsHgmConfigChangeCallbackProxy, nullptr);
    int32_t refreshRate = 32;
    rsHgmConfigChangeCallbackProxy->OnHgmRefreshRateUpdate(refreshRate);
}

} // namespace OHOS::Rosen
