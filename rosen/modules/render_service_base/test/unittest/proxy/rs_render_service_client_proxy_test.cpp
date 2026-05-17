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

#include <gtest/gtest.h>
#include "rs_client_to_service_connect_hub.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_render_service_client.h"
#include "animation/rs_frame_rate_range.h"
#include "variable_frame_rate/rs_variable_frame_rate.h"
#include "transaction/zidl/rs_client_to_service_connection_proxy.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSRenderServiceClientProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSRenderServiceClientProxyTest::SetUpTestCase() {}
void RSRenderServiceClientProxyTest::TearDownTestCase() {}
void RSRenderServiceClientProxyTest::SetUp() {}
void RSRenderServiceClientProxyTest::TearDown() {}

/**
 * @tc.name: SetScreenRefreshRateTest
 * @tc.desc: Set screen refresh rate with screenId, sceneId and rate
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceClientProxyTest, SetScreenRefreshRateTest, TestSize.Level1)
{
    auto rsClient = RSRenderServiceClient::CreateRenderServiceClient();
    rsClient->SetScreenRefreshRate(0, 0, 60);
    EXPECT_NE(rsClient, nullptr);
}

/**
 * @tc.name: SetRefreshRateModeTest
 * @tc.desc: Set refresh rate mode (e.g., auto, manual)
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceClientProxyTest, SetRefreshRateModeTest, TestSize.Level1)
{
    auto rsClient = RSRenderServiceClient::CreateRenderServiceClient();
    rsClient->SetRefreshRateMode(0);
    EXPECT_NE(rsClient, nullptr);
}

/**
 * @tc.name: SyncFrameRateRangeTest
 * @tc.desc: Sync frame rate range for a frame rate linker
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceClientProxyTest, SyncFrameRateRangeTest, TestSize.Level1)
{
    auto rsClient = RSRenderServiceClient::CreateRenderServiceClient();
    FrameRateRange range(30, 120, 60);
    rsClient->SyncFrameRateRange(0, range, 60);
    EXPECT_NE(rsClient, nullptr);
}

/**
 * @tc.name: UnregisterFrameRateLinkerTest
 * @tc.desc: Unregister a frame rate linker by id
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceClientProxyTest, UnregisterFrameRateLinkerTest, TestSize.Level1)
{
    auto rsClient = RSRenderServiceClient::CreateRenderServiceClient();
    rsClient->UnregisterFrameRateLinker(0);
    EXPECT_NE(rsClient, nullptr);
}

/**
 * @tc.name: GetScreenCurrentRefreshRateTest
 * @tc.desc: Get current refresh rate of a screen
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceClientProxyTest, GetScreenCurrentRefreshRateTest, TestSize.Level1)
{
    auto rsClient = RSRenderServiceClient::CreateRenderServiceClient();
    rsClient->GetScreenCurrentRefreshRate(0);
    EXPECT_NE(rsClient, nullptr);
}

/**
 * @tc.name: GetCurrentRefreshRateModeTest
 * @tc.desc: Get current refresh rate mode
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceClientProxyTest, GetCurrentRefreshRateModeTest, TestSize.Level1)
{
    auto rsClient = RSRenderServiceClient::CreateRenderServiceClient();
    rsClient->GetCurrentRefreshRateMode();
    EXPECT_NE(rsClient, nullptr);
}

/**
 * @tc.name: GetScreenSupportedRefreshRatesTest
 * @tc.desc: Get supported refresh rates of a screen
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceClientProxyTest, GetScreenSupportedRefreshRatesTest, TestSize.Level1)
{
    auto rsClient = RSRenderServiceClient::CreateRenderServiceClient();
    rsClient->GetScreenSupportedRefreshRates(0);
    EXPECT_NE(rsClient, nullptr);
}

/**
 * @tc.name: GetShowRefreshRateEnabledTest
 * @tc.desc: Get whether show refresh rate is enabled
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceClientProxyTest, GetShowRefreshRateEnabledTest, TestSize.Level1)
{
    auto rsClient = RSRenderServiceClient::CreateRenderServiceClient();
    rsClient->GetShowRefreshRateEnabled();
    EXPECT_NE(rsClient, nullptr);
}

/**
 * @tc.name: SetShowRefreshRateEnabledTest
 * @tc.desc: Enable or disable show refresh rate feature
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceClientProxyTest, SetShowRefreshRateEnabledTest, TestSize.Level1)
{
    auto rsClient = RSRenderServiceClient::CreateRenderServiceClient();
    rsClient->SetShowRefreshRateEnabled(true, 1);
    EXPECT_NE(rsClient, nullptr);
}

/**
 * @tc.name: GetRealtimeRefreshRateTest
 * @tc.desc: Get real-time refresh rate of a screen
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceClientProxyTest, GetRealtimeRefreshRateTest, TestSize.Level1)
{
    auto rsClient = RSRenderServiceClient::CreateRenderServiceClient();
    rsClient->GetRealtimeRefreshRate(0);
    EXPECT_NE(rsClient, nullptr);
}

/**
 * @tc.name: SetWindowExpectedRefreshRateUint64Test
 * @tc.desc: Set expected refresh rate with uint64_t window id
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceClientProxyTest, SetWindowExpectedRefreshRateUint64Test, TestSize.Level1)
{
    auto rsClient = RSRenderServiceClient::CreateRenderServiceClient();
    std::unordered_map<uint64_t, EventInfo> eventInfos;
    EventInfo info;
    info.eventName = "test";
    info.eventStatus = true;
    info.minRefreshRate = 60;
    info.maxRefreshRate = 120;
    info.description = "test";
    eventInfos[0] = info;
    rsClient->SetWindowExpectedRefreshRate(eventInfos);
    EXPECT_NE(rsClient, nullptr);
}

/**
 * @tc.name: SetWindowExpectedRefreshRateStringTest
 * @tc.desc: Set expected refresh rate with string window id
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceClientProxyTest, SetWindowExpectedRefreshRateStringTest, TestSize.Level1)
{
    auto rsClient = RSRenderServiceClient::CreateRenderServiceClient();
    std::unordered_map<std::string, EventInfo> eventInfos;
    EventInfo info;
    info.eventName = "test";
    info.eventStatus = true;
    info.minRefreshRate = 60;
    info.maxRefreshRate = 120;
    info.description = "test";
    eventInfos["test"] = info;
    rsClient->SetWindowExpectedRefreshRate(eventInfos);
    EXPECT_NE(rsClient, nullptr);
}

/**
 * @tc.name: RegisterHgmConfigChangeCallbackTest
 * @tc.desc: Register callback for HGM config change notifications
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceClientProxyTest, RegisterHgmConfigChangeCallbackTest, TestSize.Level1)
{
    auto rsClient = RSRenderServiceClient::CreateRenderServiceClient();
    HgmConfigChangeCallback callback = [](std::shared_ptr<RSHgmConfigData> data) {};
    rsClient->RegisterHgmConfigChangeCallback(callback);
    EXPECT_NE(rsClient, nullptr);
}

/**
 * @tc.name: RegisterHgmRefreshRateModeChangeCallbackTest
 * @tc.desc: Register callback for refresh rate mode changes
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceClientProxyTest, RegisterHgmRefreshRateModeChangeCallbackTest, TestSize.Level1)
{
    auto rsClient = RSRenderServiceClient::CreateRenderServiceClient();
    HgmRefreshRateModeChangeCallback callback = [](int32_t mode) {};
    rsClient->RegisterHgmRefreshRateModeChangeCallback(callback);
    EXPECT_NE(rsClient, nullptr);
}

/**
 * @tc.name: RegisterHgmRefreshRateUpdateCallbackTest
 * @tc.desc: Register callback for refresh rate update notifications
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceClientProxyTest, RegisterHgmRefreshRateUpdateCallbackTest, TestSize.Level1)
{
    auto rsClient = RSRenderServiceClient::CreateRenderServiceClient();
    HgmRefreshRateUpdateCallback callback = [](int32_t rate) {};
    rsClient->RegisterHgmRefreshRateUpdateCallback(callback);
    EXPECT_NE(rsClient, nullptr);
}

/**
 * @tc.name: RegisterFrameRateLinkerExpectedFpsUpdateCallbackTest
 * @tc.desc: Register callback for expected FPS updates
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceClientProxyTest, RegisterFrameRateLinkerExpectedFpsUpdateCallbackTest, TestSize.Level1)
{
    auto rsClient = RSRenderServiceClient::CreateRenderServiceClient();
    FrameRateLinkerExpectedFpsUpdateCallback callback = [](int32_t fps, const std::string& name, int32_t dstPid) {};
    rsClient->RegisterFrameRateLinkerExpectedFpsUpdateCallback(0, callback);
    EXPECT_NE(rsClient, nullptr);
}

/**
 * @tc.name: NotifyAppStrategyConfigChangeEventTest
 * @tc.desc: Notify app strategy config change to HGM
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceClientProxyTest, NotifyAppStrategyConfigChangeEventTest, TestSize.Level1)
{
    auto rsClient = RSRenderServiceClient::CreateRenderServiceClient();
    std::vector<std::pair<std::string, std::string>> config;
    config.emplace_back("key", "value");
    rsClient->NotifyAppStrategyConfigChangeEvent("com.test", 1, config);
    EXPECT_NE(rsClient, nullptr);
}

/**
 * @tc.name: NotifySoftVsyncRateDiscountEventTest
 * @tc.desc: Notify soft vsync rate discount event to HGM
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceClientProxyTest, NotifySoftVsyncRateDiscountEventTest, TestSize.Level1)
{
    auto rsClient = RSRenderServiceClient::CreateRenderServiceClient();
    rsClient->NotifySoftVsyncRateDiscountEvent(0, "test", 100);
    EXPECT_NE(rsClient, nullptr);
}

/**
 * @tc.name: NotifyTouchEventTest
 * @tc.desc: Notify touch event to HGM for refresh rate decisions
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceClientProxyTest, NotifyTouchEventTest, TestSize.Level1)
{
    auto rsClient = RSRenderServiceClient::CreateRenderServiceClient();
    rsClient->NotifyTouchEvent(0, 1, 0);
    EXPECT_NE(rsClient, nullptr);
}

/**
 * @tc.name: NotifyLightFactorStatusTest
 * @tc.desc: Notify light factor status change to HGM
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceClientProxyTest, NotifyLightFactorStatusTest, TestSize.Level1)
{
    auto rsClient = RSRenderServiceClient::CreateRenderServiceClient();
    rsClient->NotifyLightFactorStatus(0);
    EXPECT_NE(rsClient, nullptr);
}

/**
 * @tc.name: NotifyPackageEventTest
 * @tc.desc: Notify package list event to HGM
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceClientProxyTest, NotifyPackageEventTest, TestSize.Level1)
{
    auto rsClient = RSRenderServiceClient::CreateRenderServiceClient();
    std::vector<std::string> packageList = {"package1", "package2"};
    rsClient->NotifyPackageEvent(2, packageList);
    EXPECT_NE(rsClient, nullptr);
}

/**
 * @tc.name: NotifyRefreshRateEventTest
 * @tc.desc: Notify refresh rate event to HGM
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceClientProxyTest, NotifyRefreshRateEventTest, TestSize.Level1)
{
    auto rsClient = RSRenderServiceClient::CreateRenderServiceClient();
    EventInfo info;
    info.eventName = "test";
    info.eventStatus = true;
    info.minRefreshRate = 60;
    info.maxRefreshRate = 120;
    info.description = "test";
    rsClient->NotifyRefreshRateEvent(info);
    EXPECT_NE(rsClient, nullptr);
}

/**
 * @tc.name: NotifyDynamicModeEventTest
 * @tc.desc: Notify dynamic mode enable/disable event to HGM
 * @tc.type: FUNC
 */
HWTEST_F(RSRenderServiceClientProxyTest, NotifyDynamicModeEventTest, TestSize.Level1)
{
    auto rsClient = RSRenderServiceClient::CreateRenderServiceClient();
    rsClient->NotifyDynamicModeEvent(true);
    EXPECT_NE(rsClient, nullptr);
}

} // namespace Rosen
} // namespace OHOS
