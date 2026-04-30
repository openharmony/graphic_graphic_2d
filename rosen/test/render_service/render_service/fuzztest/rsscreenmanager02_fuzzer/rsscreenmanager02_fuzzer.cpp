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

#include "rsscreenmanager02_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>
#include <memory>

#include "screen_manager/rs_screen_manager.h"
#include "screen_manager/screen_types.h"

namespace OHOS {
namespace Rosen {

sptr<RSScreenManager> g_screenManager;

namespace {
constexpr uint8_t DO_CREATE_VIRTUAL_SCREEN = 0;
constexpr uint8_t DO_REMOVE_VIRTUAL_SCREEN = 1;
constexpr uint8_t DO_RESIZE_VIRTUAL_SCREEN = 2;
constexpr uint8_t DO_SET_VIRTUAL_MIRROR_SCREEN_CANVAS_ROTATION = 3;
constexpr uint8_t DO_SET_VIRTUAL_SCREEN_AUTO_ROTATION = 4;
constexpr uint8_t DO_SET_VIRTUAL_MIRROR_SCREEN_SCALE_MODE = 5;
constexpr uint8_t DO_SET_VIRTUAL_SCREEN_STATUS = 6;
constexpr uint8_t DO_SET_VIRTUAL_SCREEN_BLACK_LIST = 7;
constexpr uint8_t DO_ADD_VIRTUAL_SCREEN_WHITE_LIST = 8;
constexpr uint8_t DO_SET_MIRROR_SCREEN_VISIBLE_RECT = 9;
constexpr uint8_t TARGET_SIZE = 10;

constexpr uint8_t SCREEN_SCALE_MODE_SIZE = 3;
constexpr uint8_t VIRTUAL_SCREEN_STATUS_SIZE = 3;
constexpr uint8_t MAX_FUZZ_LIST_SIZE = 8;

void DoCreateVirtualScreen(FuzzedDataProvider& fdp)
{
    std::string name = fdp.ConsumeRandomLengthString(32);
    uint32_t width = fdp.ConsumeIntegral<uint32_t>();
    uint32_t height = fdp.ConsumeIntegral<uint32_t>();
    sptr<Surface> surface;
    ScreenId associatedScreenId = fdp.ConsumeIntegral<ScreenId>();
    int32_t flags = fdp.ConsumeIntegral<int32_t>();
    uint8_t count = fdp.ConsumeIntegral<uint8_t>() % MAX_FUZZ_LIST_SIZE;
    std::vector<uint64_t> whiteList;
    for (uint8_t i = 0; i < count; i++) {
        whiteList.push_back(fdp.ConsumeIntegral<uint64_t>());
    }
    g_screenManager->CreateVirtualScreen(name, width, height, surface, associatedScreenId, flags, whiteList);
}

void DoRemoveVirtualScreen(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    g_screenManager->RemoveVirtualScreen(id);
}

void DoResizeVirtualScreen(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegralInRange<ScreenId>(0, 255);
    uint32_t width = fdp.ConsumeIntegral<uint32_t>();
    uint32_t height = fdp.ConsumeIntegral<uint32_t>();
    g_screenManager->ResizeVirtualScreen(id, width, height);
}

void DoSetVirtualMirrorScreenCanvasRotation(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    bool canvasRotation = fdp.ConsumeBool();
    g_screenManager->SetVirtualMirrorScreenCanvasRotation(id, canvasRotation);
}

void DoSetVirtualScreenAutoRotation(FuzzedDataProvider& fdp)
{
    bool isAutoRotation = fdp.ConsumeBool();
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    g_screenManager->SetVirtualScreenAutoRotation(id, isAutoRotation);
}

void DoSetVirtualMirrorScreenScaleMode(FuzzedDataProvider& fdp)
{
    ScreenScaleMode scaleMode = static_cast<ScreenScaleMode>(
        fdp.ConsumeIntegral<uint8_t>() % SCREEN_SCALE_MODE_SIZE);
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    g_screenManager->SetVirtualMirrorScreenScaleMode(id, scaleMode);
}

void DoSetVirtualScreenStatus(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    VirtualScreenStatus screenStatus = static_cast<VirtualScreenStatus>(
        fdp.ConsumeIntegralInRange<uint8_t>(0, VIRTUAL_SCREEN_STATUS_SIZE));
    g_screenManager->SetVirtualScreenStatus(id, screenStatus);
}

void DoSetVirtualScreenBlackList(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    uint8_t count = fdp.ConsumeIntegral<uint8_t>() % MAX_FUZZ_LIST_SIZE;
    std::vector<uint64_t> bList;
    for (uint8_t i = 0; i < count; i++) {
        bList.push_back(fdp.ConsumeIntegral<uint64_t>());
    }
    g_screenManager->SetVirtualScreenBlackList(id, bList);
}

void DoAddVirtualScreenWhiteList(FuzzedDataProvider& fdp)
{
    fdp.ConsumeBool();
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    uint8_t count = fdp.ConsumeIntegral<uint8_t>() % MAX_FUZZ_LIST_SIZE;
    std::vector<NodeId> whiteList;
    for (uint8_t i = 0; i < count; i++) {
        whiteList.push_back(fdp.ConsumeIntegral<NodeId>());
    }
    g_screenManager->AddVirtualScreenWhiteList(id, whiteList);
}

void DoSetMirrorScreenVisibleRect(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    Rect mainScreenRect;
    mainScreenRect.x = fdp.ConsumeIntegral<int32_t>();
    mainScreenRect.y = fdp.ConsumeIntegral<int32_t>();
    mainScreenRect.w = fdp.ConsumeIntegral<int32_t>();
    mainScreenRect.h = fdp.ConsumeIntegral<int32_t>();
    bool supportRotation = fdp.ConsumeBool();
    g_screenManager->SetMirrorScreenVisibleRect(id, mainScreenRect, supportRotation);
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
        case OHOS::Rosen::DO_CREATE_VIRTUAL_SCREEN:
            OHOS::Rosen::DoCreateVirtualScreen(fdp);
            break;
        case OHOS::Rosen::DO_REMOVE_VIRTUAL_SCREEN:
            OHOS::Rosen::DoRemoveVirtualScreen(fdp);
            break;
        case OHOS::Rosen::DO_RESIZE_VIRTUAL_SCREEN:
            OHOS::Rosen::DoResizeVirtualScreen(fdp);
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_MIRROR_SCREEN_CANVAS_ROTATION:
            OHOS::Rosen::DoSetVirtualMirrorScreenCanvasRotation(fdp);
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_SCREEN_AUTO_ROTATION:
            OHOS::Rosen::DoSetVirtualScreenAutoRotation(fdp);
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_MIRROR_SCREEN_SCALE_MODE:
            OHOS::Rosen::DoSetVirtualMirrorScreenScaleMode(fdp);
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_SCREEN_STATUS:
            OHOS::Rosen::DoSetVirtualScreenStatus(fdp);
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_SCREEN_BLACK_LIST:
            OHOS::Rosen::DoSetVirtualScreenBlackList(fdp);
            break;
        case OHOS::Rosen::DO_ADD_VIRTUAL_SCREEN_WHITE_LIST:
            OHOS::Rosen::DoAddVirtualScreenWhiteList(fdp);
            break;
        case OHOS::Rosen::DO_SET_MIRROR_SCREEN_VISIBLE_RECT:
            OHOS::Rosen::DoSetMirrorScreenVisibleRect(fdp);
            break;
        default:
            return -1;
    }
    return 0;
}