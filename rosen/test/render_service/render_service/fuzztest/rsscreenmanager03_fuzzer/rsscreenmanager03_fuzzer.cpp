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

#include "rsscreenmanager03_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>
#include <memory>

#include "screen_manager/rs_screen_manager.h"
#include "screen_manager/screen_types.h"

namespace OHOS {
namespace Rosen {

sptr<RSScreenManager> g_screenManager;

namespace {
constexpr uint8_t DO_SET_SCREEN_ACTIVE_RECT = 0;
constexpr uint8_t DO_SET_PHYSICAL_SCREEN_RESOLUTION = 1;
constexpr uint8_t DO_SET_ROG_SCREEN_RESOLUTION = 2;
constexpr uint8_t DO_SET_DUAL_SCREEN_STATE = 3;
constexpr uint8_t DO_SET_AS_MAIN_SCREEN = 4;
constexpr uint8_t DO_SET_SCREEN_SKIP_FRAME_INTERVAL = 5;
constexpr uint8_t DO_SET_SCREEN_COLOR_SPACE = 6;
constexpr uint8_t DO_SET_SCREEN_SWITCH_STATUS = 7;
constexpr uint8_t DO_SET_SCREEN_OFFSET = 8;
constexpr uint8_t DO_SET_SCREEN_FRAME_GRAVITY = 9;
constexpr uint8_t TARGET_SIZE = 10;

constexpr uint8_t DUAL_SCREEN_STATUS_SIZE = 3;
constexpr uint8_t GRAPHIC_CM_COLOR_SPACE_TYPE_SIZE = 10;

void DoSetScreenActiveRect(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    Rect activeRect;
    activeRect.x = fdp.ConsumeIntegral<int32_t>();
    activeRect.y = fdp.ConsumeIntegral<int32_t>();
    activeRect.w = fdp.ConsumeIntegral<int32_t>();
    activeRect.h = fdp.ConsumeIntegral<int32_t>();
    g_screenManager->SetScreenActiveRect(id, activeRect);
}

void DoSetPhysicalScreenResolution(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    uint32_t width = fdp.ConsumeIntegral<uint32_t>();
    uint32_t height = fdp.ConsumeIntegral<uint32_t>();
    g_screenManager->SetPhysicalScreenResolution(id, width, height);
}

void DoSetRogScreenResolution(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegralInRange<ScreenId>(0, 255);
    uint32_t width = fdp.ConsumeIntegral<uint32_t>();
    uint32_t height = fdp.ConsumeIntegral<uint32_t>();
    g_screenManager->SetRogScreenResolution(id, width, height);
}

void DoSetDualScreenState(FuzzedDataProvider& fdp)
{
    DualScreenStatus status = static_cast<DualScreenStatus>(
        fdp.ConsumeIntegral<uint8_t>() % DUAL_SCREEN_STATUS_SIZE);
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    g_screenManager->SetDualScreenState(id, status);
}

void DoSetAsMainScreen(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    bool isMainScreen = fdp.ConsumeBool();
    g_screenManager->SetAsMainScreen(id, isMainScreen);
}

void DoSetScreenSkipFrameInterval(FuzzedDataProvider& fdp)
{
    fdp.ConsumeBool();
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    uint32_t skipFrameInterval = fdp.ConsumeIntegral<uint32_t>();
    g_screenManager->SetScreenSkipFrameInterval(id, skipFrameInterval);
}

void DoSetScreenColorSpace(FuzzedDataProvider& fdp)
{
    GraphicCM_ColorSpaceType colorSpace = static_cast<GraphicCM_ColorSpaceType>(
        fdp.ConsumeIntegralInRange<uint8_t>(0, GRAPHIC_CM_COLOR_SPACE_TYPE_SIZE));
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    g_screenManager->SetScreenColorSpace(id, colorSpace);
}

void DoSetScreenSwitchStatus(FuzzedDataProvider& fdp)
{
    bool status = fdp.ConsumeBool();
    g_screenManager->SetScreenSwitchStatus(status);
}

void DoSetScreenOffset(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    int32_t offsetX = fdp.ConsumeIntegral<int32_t>();
    int32_t offsetY = fdp.ConsumeIntegral<int32_t>();
    g_screenManager->SetScreenOffset(id, offsetX, offsetY);
}

void DoSetScreenFrameGravity(FuzzedDataProvider& fdp)
{
    int32_t gravity = fdp.ConsumeIntegral<int32_t>();
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    g_screenManager->SetScreenFrameGravity(id, gravity);
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
        case OHOS::Rosen::DO_SET_SCREEN_ACTIVE_RECT:
            OHOS::Rosen::DoSetScreenActiveRect(fdp);
            break;
        case OHOS::Rosen::DO_SET_PHYSICAL_SCREEN_RESOLUTION:
            OHOS::Rosen::DoSetPhysicalScreenResolution(fdp);
            break;
        case OHOS::Rosen::DO_SET_ROG_SCREEN_RESOLUTION:
            OHOS::Rosen::DoSetRogScreenResolution(fdp);
            break;
        case OHOS::Rosen::DO_SET_DUAL_SCREEN_STATE:
            OHOS::Rosen::DoSetDualScreenState(fdp);
            break;
        case OHOS::Rosen::DO_SET_AS_MAIN_SCREEN:
            OHOS::Rosen::DoSetAsMainScreen(fdp);
            break;
        case OHOS::Rosen::DO_SET_SCREEN_SKIP_FRAME_INTERVAL:
            OHOS::Rosen::DoSetScreenSkipFrameInterval(fdp);
            break;
        case OHOS::Rosen::DO_SET_SCREEN_COLOR_SPACE:
            OHOS::Rosen::DoSetScreenColorSpace(fdp);
            break;
        case OHOS::Rosen::DO_SET_SCREEN_SWITCH_STATUS:
            OHOS::Rosen::DoSetScreenSwitchStatus(fdp);
            break;
        case OHOS::Rosen::DO_SET_SCREEN_OFFSET:
            OHOS::Rosen::DoSetScreenOffset(fdp);
            break;
        case OHOS::Rosen::DO_SET_SCREEN_FRAME_GRAVITY:
            OHOS::Rosen::DoSetScreenFrameGravity(fdp);
            break;
        default:
            return -1;
    }
    return 0;
}