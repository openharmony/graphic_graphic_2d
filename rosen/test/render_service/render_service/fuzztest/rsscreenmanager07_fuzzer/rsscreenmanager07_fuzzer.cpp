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

#include "rsscreenmanager07_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>
#include <memory>
#include <string>

#include "screen_manager/rs_screen_manager.h"
#include "screen_manager/screen_types.h"
#include "screen_manager/rs_screen_mode_info.h"
#include "screen_manager/rs_virtual_screen_resolution.h"

namespace OHOS {
namespace Rosen {

sptr<RSScreenManager> g_screenManager;

namespace {
constexpr uint8_t DO_GET_DEFAULT_SCREEN_ACTIVE_MODE = 0;
constexpr uint8_t DO_GET_DISPLAY_IDENTIFICATION_DATA = 1;
constexpr uint8_t DO_GET_ROG_SCREEN_RESOLUTION = 2;
constexpr uint8_t DO_GET_VIRTUAL_SCREEN_RESOLUTION = 3;
constexpr uint8_t DO_GET_CANVAS_ROTATION = 4;
constexpr uint8_t DO_GET_VIRTUAL_SCREEN_AUTO_ROTATION = 5;
constexpr uint8_t DO_UPDATE_FOLD_SCREEN_CONNECT_STATUS_LOCKED = 6;
constexpr uint8_t DO_SET_SCREEN_VSYNC_ENABLE_BY_ID = 7;
constexpr uint8_t DO_GET_SCREEN_VSYNC_ENABLE_BY_ID = 8;
constexpr uint8_t DO_DISPLAY_DUMP = 9;
constexpr uint8_t TARGET_SIZE = 10;

void DoGetDefaultScreenActiveMode(FuzzedDataProvider& fdp)
{
    RSScreenModeInfo screenModeInfo;
    screenModeInfo.SetScreenHeight(fdp.ConsumeIntegral<int32_t>());
    screenModeInfo.SetScreenRefreshRate(fdp.ConsumeIntegral<uint32_t>());
    int32_t maxModeId = 31;
    screenModeInfo.SetScreenModeId(fdp.ConsumeIntegralInRange<int32_t>(0, maxModeId));
    g_screenManager->GetDefaultScreenActiveMode(screenModeInfo);
}

void DoGetDisplayIdentificationData(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegralInRange<ScreenId>(0, 255);
    uint8_t outPort = fdp.ConsumeIntegral<uint8_t>();
    std::vector<uint8_t> edidData;
    g_screenManager->GetDisplayIdentificationData(id, outPort, edidData);
}

void DoGetRogScreenResolution(FuzzedDataProvider& fdp)
{
    uint32_t width = fdp.ConsumeIntegral<uint32_t>();
    uint32_t height = fdp.ConsumeIntegral<uint32_t>();
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    g_screenManager->GetRogScreenResolution(id, width, height);
}

void DoGetVirtualScreenResolution(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    fdp.ConsumeBool();
    RSVirtualScreenResolution virtualScreenResolution;
    virtualScreenResolution.SetVirtualScreenWidth(fdp.ConsumeIntegral<uint32_t>());
    virtualScreenResolution.SetVirtualScreenHeight(fdp.ConsumeIntegral<uint32_t>());
    g_screenManager->GetVirtualScreenResolution(id, virtualScreenResolution);
}

void DoGetCanvasRotation(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegralInRange<ScreenId>(0, INVALID_SCREEN_ID - 1);
    g_screenManager->GetCanvasRotation(id);
}

void DoGetVirtualScreenAutoRotation(FuzzedDataProvider& fdp)
{
    fdp.ConsumeBool();
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    g_screenManager->GetVirtualScreenAutoRotation(id);
}

void DoUpdateFoldScreenConnectStatusLocked(FuzzedDataProvider& fdp)
{
    ScreenId screenId = fdp.ConsumeIntegral<ScreenId>();
    bool connected = fdp.ConsumeBool();
    g_screenManager->UpdateFoldScreenConnectStatusLocked(screenId, connected);
}

void DoSetScreenVsyncEnableById(FuzzedDataProvider& fdp)
{
    ScreenId vsyncEnabledScreenId = fdp.ConsumeIntegralInRange<ScreenId>(0, 255);
    ScreenId screenId = fdp.ConsumeIntegral<ScreenId>();
    bool enabled = fdp.ConsumeBool();
    g_screenManager->SetScreenVsyncEnableById(vsyncEnabledScreenId, screenId, enabled);
}

void DoGetScreenVsyncEnableById(FuzzedDataProvider& fdp)
{
    fdp.ConsumeIntegral<uint8_t>();
    ScreenId vsyncEnabledScreenId = fdp.ConsumeIntegralInRange<ScreenId>(0, 255);
    g_screenManager->GetScreenVsyncEnableById(vsyncEnabledScreenId);
}

void DoDisplayDump(FuzzedDataProvider& fdp)
{
    std::string dumpString = fdp.ConsumeRandomLengthString(64);
    g_screenManager->DisplayDump(dumpString);
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
        case OHOS::Rosen::DO_GET_DEFAULT_SCREEN_ACTIVE_MODE:
            OHOS::Rosen::DoGetDefaultScreenActiveMode(fdp);
            break;
        case OHOS::Rosen::DO_GET_DISPLAY_IDENTIFICATION_DATA:
            OHOS::Rosen::DoGetDisplayIdentificationData(fdp);
            break;
        case OHOS::Rosen::DO_GET_ROG_SCREEN_RESOLUTION:
            OHOS::Rosen::DoGetRogScreenResolution(fdp);
            break;
        case OHOS::Rosen::DO_GET_VIRTUAL_SCREEN_RESOLUTION:
            OHOS::Rosen::DoGetVirtualScreenResolution(fdp);
            break;
        case OHOS::Rosen::DO_GET_CANVAS_ROTATION:
            OHOS::Rosen::DoGetCanvasRotation(fdp);
            break;
        case OHOS::Rosen::DO_GET_VIRTUAL_SCREEN_AUTO_ROTATION:
            OHOS::Rosen::DoGetVirtualScreenAutoRotation(fdp);
            break;
        case OHOS::Rosen::DO_UPDATE_FOLD_SCREEN_CONNECT_STATUS_LOCKED:
            OHOS::Rosen::DoUpdateFoldScreenConnectStatusLocked(fdp);
            break;
        case OHOS::Rosen::DO_SET_SCREEN_VSYNC_ENABLE_BY_ID:
            OHOS::Rosen::DoSetScreenVsyncEnableById(fdp);
            break;
        case OHOS::Rosen::DO_GET_SCREEN_VSYNC_ENABLE_BY_ID:
            OHOS::Rosen::DoGetScreenVsyncEnableById(fdp);
            break;
        case OHOS::Rosen::DO_DISPLAY_DUMP:
            OHOS::Rosen::DoDisplayDump(fdp);
            break;
        default:
            return -1;
    }
    return 0;
}