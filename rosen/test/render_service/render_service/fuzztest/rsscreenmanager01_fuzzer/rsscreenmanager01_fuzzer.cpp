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

#include "rsscreenmanager01_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>
#include <memory>

#include "screen_manager/rs_screen_manager.h"
#include "screen_manager/screen_types.h"

namespace OHOS {
namespace Rosen {

sptr<RSScreenManager> g_screenManager;

namespace {
constexpr uint8_t DO_SET_SCREEN_ACTIVE_MODE = 0;
constexpr uint8_t DO_SET_SCREEN_POWER_STATUS = 1;
constexpr uint8_t DO_SET_SCREEN_BACKLIGHT = 2;
constexpr uint8_t DO_SET_SCREEN_COLOR_GAMUT = 3;
constexpr uint8_t DO_SET_SCREEN_GAMUT_MAP = 4;
constexpr uint8_t DO_SET_SCREEN_HDR_FORMAT = 5;
constexpr uint8_t DO_SET_SCREEN_CORRECTION = 6;
constexpr uint8_t DO_SET_PIXEL_FORMAT = 7;
constexpr uint8_t DO_SET_SCREEN_CONSTRAINT = 8;
constexpr uint8_t DO_SET_VIRTUAL_SCREEN_RESOLUTION = 9;
constexpr uint8_t TARGET_SIZE = 10;

constexpr uint8_t SCREEN_POWER_STATUS_SIZE = 11;
constexpr uint8_t SCREEN_ROTATION_SIZE = 5;
constexpr uint8_t SCREEN_GAMUT_MAP_SIZE = 4;
constexpr uint8_t SCREEN_CONSTRAINT_TYPE_SIZE = 4;
constexpr uint8_t GRAPHIC_PIXEL_FORMAT_SIZE = 8;

void DoSetScreenActiveMode(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    uint32_t modeId = fdp.ConsumeIntegral<uint32_t>();
    g_screenManager->SetScreenActiveMode(id, modeId);
}

void DoSetScreenPowerStatus(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    ScreenPowerStatus status = static_cast<ScreenPowerStatus>(
        fdp.ConsumeIntegral<uint8_t>() % SCREEN_POWER_STATUS_SIZE);
    g_screenManager->SetScreenPowerStatus(id, status);
}

void DoSetScreenBacklight(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    uint32_t level = fdp.ConsumeIntegralInRange<uint32_t>(0, 255);
    g_screenManager->SetScreenBacklight(id, level);
}

void DoSetScreenColorGamut(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    int32_t modeIdx = fdp.ConsumeIntegral<int32_t>();
    g_screenManager->SetScreenColorGamut(id, modeIdx);
}

void DoSetScreenGamutMap(FuzzedDataProvider& fdp)
{
    ScreenGamutMap mode = static_cast<ScreenGamutMap>(
        fdp.ConsumeIntegral<uint8_t>() % SCREEN_GAMUT_MAP_SIZE);
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    g_screenManager->SetScreenGamutMap(id, mode);
}

void DoSetScreenHDRFormat(FuzzedDataProvider& fdp)
{
    int32_t modeIdx = fdp.ConsumeIntegral<int32_t>();
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    g_screenManager->SetScreenHDRFormat(id, modeIdx);
}

void DoSetScreenCorrection(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegralInRange<ScreenId>(0, INVALID_SCREEN_ID - 1);
    ScreenRotation screenRotation = static_cast<ScreenRotation>(
        fdp.ConsumeIntegral<uint8_t>() % SCREEN_ROTATION_SIZE);
    g_screenManager->SetScreenCorrection(id, screenRotation);
}

void DoSetPixelFormat(FuzzedDataProvider& fdp)
{
    GraphicPixelFormat pixelFormat = static_cast<GraphicPixelFormat>(
        fdp.ConsumeIntegralInRange<uint8_t>(0, GRAPHIC_PIXEL_FORMAT_SIZE));
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    g_screenManager->SetPixelFormat(id, pixelFormat);
}

void DoSetScreenConstraint(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    uint64_t timestamp = fdp.ConsumeIntegral<uint64_t>();
    ScreenConstraintType type = static_cast<ScreenConstraintType>(
        fdp.ConsumeIntegral<uint8_t>() % SCREEN_CONSTRAINT_TYPE_SIZE);
    g_screenManager->SetScreenConstraint(id, timestamp, type);
}

void DoSetVirtualScreenResolution(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    uint32_t width = fdp.ConsumeIntegral<uint32_t>();
    uint32_t height = fdp.ConsumeIntegral<uint32_t>();
    g_screenManager->SetVirtualScreenResolution(id, width, height);
}

} // namespace

} // namespace Rosen
} // namespace OHOS

/* Fuzzer environment initialization */
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

/* Fuzzer entry point */
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
        case OHOS::Rosen::DO_SET_SCREEN_ACTIVE_MODE:
            OHOS::Rosen::DoSetScreenActiveMode(fdp);
            break;
        case OHOS::Rosen::DO_SET_SCREEN_POWER_STATUS:
            OHOS::Rosen::DoSetScreenPowerStatus(fdp);
            break;
        case OHOS::Rosen::DO_SET_SCREEN_BACKLIGHT:
            OHOS::Rosen::DoSetScreenBacklight(fdp);
            break;
        case OHOS::Rosen::DO_SET_SCREEN_COLOR_GAMUT:
            OHOS::Rosen::DoSetScreenColorGamut(fdp);
            break;
        case OHOS::Rosen::DO_SET_SCREEN_GAMUT_MAP:
            OHOS::Rosen::DoSetScreenGamutMap(fdp);
            break;
        case OHOS::Rosen::DO_SET_SCREEN_HDR_FORMAT:
            OHOS::Rosen::DoSetScreenHDRFormat(fdp);
            break;
        case OHOS::Rosen::DO_SET_SCREEN_CORRECTION:
            OHOS::Rosen::DoSetScreenCorrection(fdp);
            break;
        case OHOS::Rosen::DO_SET_PIXEL_FORMAT:
            OHOS::Rosen::DoSetPixelFormat(fdp);
            break;
        case OHOS::Rosen::DO_SET_SCREEN_CONSTRAINT:
            OHOS::Rosen::DoSetScreenConstraint(fdp);
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_SCREEN_RESOLUTION:
            OHOS::Rosen::DoSetVirtualScreenResolution(fdp);
            break;
        default:
            return -1;
    }
    return 0;
}