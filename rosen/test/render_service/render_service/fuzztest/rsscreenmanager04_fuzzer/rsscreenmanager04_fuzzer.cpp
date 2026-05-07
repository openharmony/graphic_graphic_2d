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

#include "rsscreenmanager04_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>
#include <memory>

#include "screen_manager/rs_screen_manager.h"
#include "screen_manager/screen_types.h"
#include "screen_manager/rs_screen_mode_info.h"

namespace OHOS {
namespace Rosen {

sptr<RSScreenManager> g_screenManager;

namespace {
constexpr uint8_t DO_GET_SCREEN_TYPE = 0;
constexpr uint8_t DO_GET_SCREEN_ACTIVE_MODE = 1;
constexpr uint8_t DO_GET_SCREEN_SUPPORTED_MODES = 2;
constexpr uint8_t DO_GET_SCREEN_CAPABILITY = 3;
constexpr uint8_t DO_GET_SCREEN_POWER_STATUS = 4;
constexpr uint8_t DO_GET_SCREEN_BACKLIGHT = 5;
constexpr uint8_t DO_GET_SCREEN_ACTIVE_REFRESH_RATE = 6;
constexpr uint8_t DO_GET_SCREEN_CONNECTION_TYPE = 7;
constexpr uint8_t DO_GET_PANEL_POWER_STATUS = 8;
constexpr uint8_t DO_GET_SCREEN_DATA = 9;
constexpr uint8_t TARGET_SIZE = 10;

constexpr uint8_t SCREEN_SCREEN_TYPE_SIZE = 4;

void DoGetScreenType(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    RSScreenType type = static_cast<RSScreenType>(fdp.ConsumeIntegral<uint8_t>() % SCREEN_SCREEN_TYPE_SIZE);
    g_screenManager->GetScreenType(id, type);
}

void DoGetScreenActiveMode(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegralInRange<ScreenId>(0, INVALID_SCREEN_ID - 1);
    RSScreenModeInfo screenModeInfo;
    screenModeInfo.SetScreenWidth(fdp.ConsumeIntegral<int32_t>());
    screenModeInfo.SetScreenHeight(fdp.ConsumeIntegral<int32_t>());
    screenModeInfo.SetScreenRefreshRate(fdp.ConsumeIntegral<uint32_t>());
    screenModeInfo.SetScreenModeId(fdp.ConsumeIntegral<int32_t>());
    g_screenManager->GetScreenActiveMode(id, screenModeInfo);
}

void DoGetScreenSupportedModes(FuzzedDataProvider& fdp)
{
    fdp.ConsumeBool();
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    g_screenManager->GetScreenSupportedModes(id);
}

void DoGetScreenCapability(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    g_screenManager->GetScreenCapability(id);
}

void DoGetScreenPowerStatus(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegralInRange<ScreenId>(0, 255);
    g_screenManager->GetScreenPowerStatus(id);
}

void DoGetScreenBacklight(FuzzedDataProvider& fdp)
{
    fdp.ConsumeIntegral<uint32_t>();
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    g_screenManager->GetScreenBacklight(id);
}

void DoGetScreenActiveRefreshRate(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    fdp.ConsumeBool();
    g_screenManager->GetScreenActiveRefreshRate(id);
}

void DoGetScreenConnectionType(FuzzedDataProvider& fdp)
{
    fdp.ConsumeIntegral<uint8_t>();
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    g_screenManager->GetScreenConnectionType(id);
}

void DoGetPanelPowerStatus(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    fdp.ConsumeIntegral<uint32_t>();
    g_screenManager->GetPanelPowerStatus(id);
}

void DoGetScreenData(FuzzedDataProvider& fdp)
{
    fdp.ConsumeIntegralInRange<uint8_t>(0, DO_GET_SCREEN_CONNECTION_TYPE);
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    g_screenManager->GetScreenData(id);
}

} // namespace

} // namespace Rosen
} // namespace OHOS

extern "C" int LLVMFuzzerInitialize(int* argc, char*** argv)
{
    OHOS::Rosen::g_screenManager = OHOS::sptr<OHOS::Rosen::RSScreenManager>::MakeSptr();
    if (!OHOS::Rosen::g_screenManager) {
        return -1;
    }
    auto runner = OHOS::AppExecFwk::EventRunner::Create(false);
    auto handler = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner);
    OHOS::Rosen::g_screenManager->Init(handler);
    return 0;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return -1;
    }
    if (!OHOS::Rosen::g_screenManager) {
        return -1;
    }

    FuzzedDataProvider fdp(data, size);

    uint8_t tarPos = fdp.ConsumeIntegral<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_GET_SCREEN_TYPE:
            OHOS::Rosen::DoGetScreenType(fdp);
            break;
        case OHOS::Rosen::DO_GET_SCREEN_ACTIVE_MODE:
            OHOS::Rosen::DoGetScreenActiveMode(fdp);
            break;
        case OHOS::Rosen::DO_GET_SCREEN_SUPPORTED_MODES:
            OHOS::Rosen::DoGetScreenSupportedModes(fdp);
            break;
        case OHOS::Rosen::DO_GET_SCREEN_CAPABILITY:
            OHOS::Rosen::DoGetScreenCapability(fdp);
            break;
        case OHOS::Rosen::DO_GET_SCREEN_POWER_STATUS:
            OHOS::Rosen::DoGetScreenPowerStatus(fdp);
            break;
        case OHOS::Rosen::DO_GET_SCREEN_BACKLIGHT:
            OHOS::Rosen::DoGetScreenBacklight(fdp);
            break;
        case OHOS::Rosen::DO_GET_SCREEN_ACTIVE_REFRESH_RATE:
            OHOS::Rosen::DoGetScreenActiveRefreshRate(fdp);
            break;
        case OHOS::Rosen::DO_GET_SCREEN_CONNECTION_TYPE:
            OHOS::Rosen::DoGetScreenConnectionType(fdp);
            break;
        case OHOS::Rosen::DO_GET_PANEL_POWER_STATUS:
            OHOS::Rosen::DoGetPanelPowerStatus(fdp);
            break;
        case OHOS::Rosen::DO_GET_SCREEN_DATA:
            OHOS::Rosen::DoGetScreenData(fdp);
            break;
        default:
            return -1;
    }
    return 0;
}