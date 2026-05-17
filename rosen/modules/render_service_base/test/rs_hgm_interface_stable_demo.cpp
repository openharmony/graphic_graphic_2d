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

/**
 * @file rs_hgm_interface_stable_demo.cpp
 * @brief Comprehensive stability test for HGM-related RSRenderServiceClient APIs
 * @description Tests HGM interfaces with various parameter combinations:
 * - Normal values
 * - Boundary values
 * - Edge cases
 * Uses RSRenderServiceClient directly (ENABLE_RS_PROXY path)
 */

#include <iostream>
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>

#include "transaction/rs_render_service_client.h"
#include "animation/rs_frame_rate_range.h"
#include "variable_frame_rate/rs_variable_frame_rate.h"
#include "transaction/rs_hgm_config_data.h"

using namespace OHOS;
using namespace OHOS::Rosen;

namespace OHOS {
namespace Rosen {

static constexpr int32_t LOOP_COUNT = 100;

static void TestRefreshRateControl()
{
    auto rsClient = RSRenderServiceClient::CreateRenderServiceClient();
    if (!rsClient) {
        std::cout << "Failed to create RSRenderServiceClient" << std::endl;
        return;
    }

    for (int32_t i = 0; i < LOOP_COUNT; i++) {
        rsClient->SetScreenRefreshRate(0, 0, 60);
        rsClient->SetScreenRefreshRate(0, 1, 120);
        rsClient->SetRefreshRateMode(0);
        rsClient->SetRefreshRateMode(1);
        uint32_t currentRate = rsClient->GetScreenCurrentRefreshRate(0);
        (void)currentRate;
        int32_t mode = rsClient->GetCurrentRefreshRateMode();
        (void)mode;
        auto supportedRates = rsClient->GetScreenSupportedRefreshRates(0);
        (void)supportedRates;
        bool showEnabled = rsClient->GetShowRefreshRateEnabled();
        (void)showEnabled;
        rsClient->SetShowRefreshRateEnabled(true, 1);
        rsClient->SetShowRefreshRateEnabled(false, 0);
        uint32_t realtimeRate = rsClient->GetRealtimeRefreshRate(0);
        (void)realtimeRate;
        rsClient->SetScreenRefreshRate(0, 0, 60);
        rsClient->SetRefreshRateMode(-1);
        rsClient->SetShowRefreshRateEnabled(true, -1);
        rsClient->SetShowRefreshRateEnabled(true, INT32_MAX);
    }
}

static void TestFrameRateLinker()
{
    auto rsClient = RSRenderServiceClient::CreateRenderServiceClient();
    if (!rsClient) {
        std::cout << "Failed to create RSRenderServiceClient" << std::endl;
        return;
    }

    for (int32_t i = 0; i < LOOP_COUNT; i++) {
        FrameRateRange range(30, 120, 60);
        rsClient->SyncFrameRateRange(0, range, 60);
        FrameRateRange range2(1, 240, 120);
        rsClient->SyncFrameRateRange(1, range2, 120);
        rsClient->UnregisterFrameRateLinker(0);
        rsClient->UnregisterFrameRateLinker(1);
        rsClient->SyncFrameRateRange(UINT64_MAX, FrameRateRange(0, 0, 0), 0);
        rsClient->UnregisterFrameRateLinker(UINT64_MAX);
    }
}

static void TestHgmCallbacks()
{
    auto rsClient = RSRenderServiceClient::CreateRenderServiceClient();
    if (!rsClient) {
        std::cout << "Failed to create RSRenderServiceClient" << std::endl;
        return;
    }

    for (int32_t i = 0; i < LOOP_COUNT; i++) {
        HgmConfigChangeCallback configCallback = [](std::shared_ptr<RSHgmConfigData> data) {
            (void)data;
        };
        rsClient->RegisterHgmConfigChangeCallback(configCallback);
        HgmRefreshRateModeChangeCallback modeCallback = [](int32_t mode) {
            (void)mode;
        };
        rsClient->RegisterHgmRefreshRateModeChangeCallback(modeCallback);
        HgmRefreshRateUpdateCallback updateCallback = [](int32_t rate) {
            (void)rate;
        };
        rsClient->RegisterHgmRefreshRateUpdateCallback(updateCallback);
        rsClient->RegisterHgmConfigChangeCallback(nullptr);
        rsClient->RegisterHgmRefreshRateModeChangeCallback(nullptr);
        rsClient->RegisterHgmRefreshRateUpdateCallback(nullptr);
    }
}

static void TestFrameRateLinkerCallbacks()
{
    auto rsClient = RSRenderServiceClient::CreateRenderServiceClient();
    if (!rsClient) {
        std::cout << "Failed to create RSRenderServiceClient" << std::endl;
        return;
    }

    for (int32_t i = 0; i < LOOP_COUNT; i++) {
        FrameRateLinkerExpectedFpsUpdateCallback callback = [](int32_t fps,
            const std::string& name, int32_t dstPid) {
            (void)fps;
            (void)name;
            (void)dstPid;
        };
        rsClient->RegisterFrameRateLinkerExpectedFpsUpdateCallback(0, callback);
        rsClient->RegisterFrameRateLinkerExpectedFpsUpdateCallback(1, nullptr);
    }
}

static void TestWindowExpectedRefreshRate()
{
    auto rsClient = RSRenderServiceClient::CreateRenderServiceClient();
    if (!rsClient) {
        std::cout << "Failed to create RSRenderServiceClient" << std::endl;
        return;
    }

    for (int32_t i = 0; i < LOOP_COUNT; i++) {
        std::unordered_map<uint64_t, EventInfo> eventInfos;
        EventInfo info;
        info.eventName = "test";
        info.eventStatus = true;
        info.minRefreshRate = 60;
        info.maxRefreshRate = 120;
        info.description = "test event";
        eventInfos[0] = info;
        eventInfos[1] = info;
        rsClient->SetWindowExpectedRefreshRate(eventInfos);
        std::unordered_map<std::string, EventInfo> eventInfoStrs;
        eventInfoStrs["window1"] = info;
        eventInfoStrs["window2"] = info;
        rsClient->SetWindowExpectedRefreshRate(eventInfoStrs);
        rsClient->SetWindowExpectedRefreshRate(std::unordered_map<uint64_t, EventInfo>());
        rsClient->SetWindowExpectedRefreshRate(std::unordered_map<std::string, EventInfo>());
    }
}

static void TestNotifyEvents()
{
    auto rsClient = RSRenderServiceClient::CreateRenderServiceClient();
    if (!rsClient) {
        std::cout << "Failed to create RSRenderServiceClient" << std::endl;
        return;
    }

    std::string pkgName = "com.example.app";
    std::vector<std::pair<std::string, std::string>> config = {{"key1", "value1"}, {"key2", "value2"}};
    std::vector<std::string> packageList = {"com.example.app1", "com.example.app2"};
    EventInfo eventInfo;
    eventInfo.eventName = "test";
    eventInfo.eventStatus = true;
    eventInfo.minRefreshRate = 60;
    eventInfo.maxRefreshRate = 120;
    eventInfo.description = "test";

    for (int32_t i = 0; i < LOOP_COUNT; i++) {
        rsClient->NotifyAppStrategyConfigChangeEvent(pkgName, 2, config);
        rsClient->NotifySoftVsyncRateDiscountEvent(0, "test", 60);
        rsClient->NotifyTouchEvent(0, 1, 0);
        rsClient->NotifyLightFactorStatus(0);
        rsClient->NotifyLightFactorStatus(1);
        rsClient->NotifyPackageEvent(2, packageList);
        rsClient->NotifyRefreshRateEvent(eventInfo);
        rsClient->NotifyDynamicModeEvent(true);
        rsClient->NotifyDynamicModeEvent(false);
        rsClient->NotifyAppStrategyConfigChangeEvent("", 0, {});
        rsClient->NotifySoftVsyncRateDiscountEvent(0, "", 0);
        rsClient->NotifyTouchEvent(-1, 0, -1);
        rsClient->NotifyLightFactorStatus(-1);
        rsClient->NotifyPackageEvent(0, {});
        rsClient->NotifyPackageEvent(UINT32_MAX, packageList);
        rsClient->NotifyRefreshRateEvent(EventInfo());
        rsClient->NotifyDynamicModeEvent(true);
    }
}

} // namespace Rosen
} // namespace OHOS

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    std::cout << "========== HGM Interface Stability Test Start ==========" << std::endl;
    std::cout << "Loop count: " << OHOS::Rosen::LOOP_COUNT << " times" << std::endl;

    OHOS::Rosen::TestRefreshRateControl();
    std::cout << "[PASS] TestRefreshRateControl" << std::endl;

    OHOS::Rosen::TestFrameRateLinker();
    std::cout << "[PASS] TestFrameRateLinker" << std::endl;

    OHOS::Rosen::TestHgmCallbacks();
    std::cout << "[PASS] TestHgmCallbacks" << std::endl;

    OHOS::Rosen::TestFrameRateLinkerCallbacks();
    std::cout << "[PASS] TestFrameRateLinkerCallbacks" << std::endl;

    OHOS::Rosen::TestWindowExpectedRefreshRate();
    std::cout << "[PASS] TestWindowExpectedRefreshRate" << std::endl;

    OHOS::Rosen::TestNotifyEvents();
    std::cout << "[PASS] TestNotifyEvents" << std::endl;

    std::cout << "========== HGM Interface Stability Test End ==========" << std::endl;
    return EXIT_SUCCESS;
}
