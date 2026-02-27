/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "virtualScreen_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "transaction/rs_interfaces.h"

namespace OHOS {
namespace Rosen {

// Global RSInterfaces instance, initialized in LLVMFuzzerInitialize
RSInterfaces* g_rsInterfaces = nullptr;

namespace {
const uint8_t DO_CREATE_VIRTUAL_SCREEN = 0;
const uint8_t DO_REMOVE_VIRTUAL_SCREEN = 1;
const uint8_t DO_RESIZE_VIRTUAL_SCREEN = 2;
const uint8_t DO_SET_VIRTUAL_SCREEN_RESOLUTION = 3;
const uint8_t DO_SET_VIRTUAL_SCREEN_SURFACE = 4;
const uint8_t DO_SET_VIRTUAL_SCREEN_BLACKLIST = 5;
const uint8_t DO_GET_VIRTUAL_SCREEN_RESOLUTION = 6;
const uint8_t DO_SET_VIRTUAL_SCREEN_REFRESH_RATE = 7;
const uint8_t DO_SET_VIRTUAL_SCREEN_AUTO_ROTATION = 8;
const uint8_t DO_SET_VIRTUAL_SCREEN_USING_STATUS = 9;
const uint8_t DO_SET_VIRTUAL_MIRROR_SCREEN_CANVAS_ROTATION = 10;
const uint8_t DO_SET_VIRTUAL_MIRROR_SCREEN_SCALE_MODE = 11;
const uint8_t TARGET_SIZE = 12;
constexpr size_t STR_LEN = 10;

void DoCreateVirtualScreen(FuzzedDataProvider& fdp)
{
    std::string name = fdp.ConsumeRandomLengthString(STR_LEN);
    uint32_t width = fdp.ConsumeIntegral<uint32_t>();
    uint32_t height = fdp.ConsumeIntegral<uint32_t>();
    auto csurface = IConsumerSurface::Create("DisplayNode");
    if (csurface == nullptr) {
        return;
    }
    auto producer = csurface->GetProducer();
    if (producer == nullptr) {
        return;
    }
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    if (psurface == nullptr) {
        return;
    }
    ScreenId associatedScreenId = fdp.ConsumeIntegral<uint64_t>();
    int32_t flags = fdp.ConsumeIntegral<int32_t>();
    uint8_t listSize = fdp.ConsumeIntegral<uint8_t>();
    std::vector<NodeId> whiteList;
    for (auto i = 0; i < listSize; i++) {
        whiteList.push_back(fdp.ConsumeIntegral<uint64_t>());
    }
    g_rsInterfaces->CreateVirtualScreen(name, width, height, psurface, associatedScreenId, flags, whiteList);
}

void DoRemoveVirtualScreen(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    g_rsInterfaces->RemoveVirtualScreen(id);
}

void DoResizeVirtualScreen(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    uint32_t width = fdp.ConsumeIntegral<uint32_t>();
    uint32_t height = fdp.ConsumeIntegral<uint32_t>();
    g_rsInterfaces->ResizeVirtualScreen(id, width, height);
}

void DoSetVirtualScreenResolution(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    uint32_t width = fdp.ConsumeIntegral<uint32_t>();
    uint32_t height = fdp.ConsumeIntegral<uint32_t>();
    g_rsInterfaces->SetVirtualScreenResolution(id, width, height);
}

void DoSetVirtualScreenSurface(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    auto csurface = IConsumerSurface::Create("DisplayNode");
    if (csurface == nullptr) {
        return;
    }
    auto producer = csurface->GetProducer();
    if (producer == nullptr) {
        return;
    }
    auto psurface = Surface::CreateSurfaceAsProducer(producer);
    if (psurface == nullptr) {
        return;
    }
    g_rsInterfaces->SetVirtualScreenSurface(id, psurface);
}

void DoSetVirtualScreenBlackList(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    uint8_t listSize = fdp.ConsumeIntegral<uint8_t>();
    std::vector<NodeId> blackList;
    for (auto i = 0; i < listSize; i++) {
        blackList.push_back(fdp.ConsumeIntegral<uint64_t>());
    }
    g_rsInterfaces->SetVirtualScreenBlackList(id, blackList);
}

void DoGetVirtualScreenResolution(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    g_rsInterfaces->GetVirtualScreenResolution(id);
}

void DoSetVirtualScreenRefreshRate(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    uint32_t maxRefreshRate = fdp.ConsumeIntegral<uint32_t>();
    uint32_t actualRefreshRate = fdp.ConsumeIntegral<uint32_t>();
    g_rsInterfaces->SetVirtualScreenRefreshRate(id, maxRefreshRate, actualRefreshRate);
}

void DoSetVirtualScreenAutoRotation(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    bool isAutoRotation = fdp.ConsumeBool();
    g_rsInterfaces->SetVirtualScreenAutoRotation(id, isAutoRotation);
}

void DoSetVirtualScreenUsingStatus(FuzzedDataProvider& fdp)
{
    bool isVirtualScreenUsingStatus = fdp.ConsumeBool();
    g_rsInterfaces->SetVirtualScreenUsingStatus(isVirtualScreenUsingStatus);
}

void DoSetVirtualMirrorScreenCanvasRotation(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    bool canvasRotation = fdp.ConsumeBool();
    g_rsInterfaces->SetVirtualMirrorScreenCanvasRotation(id, canvasRotation);
}

void DoSetVirtualMirrorScreenScaleMode(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    ScreenScaleMode scaleMode = static_cast<ScreenScaleMode>(fdp.ConsumeIntegral<uint32_t>());
    g_rsInterfaces->SetVirtualMirrorScreenScaleMode(id, scaleMode);
}

} // namespace

} // namespace Rosen
} // namespace OHOS

/* Fuzzer environment initialization */
extern "C" int LLVMFuzzerInitialize(int* argc, char*** argv)
{
    // Pre-initialize RSInterfaces singleton to avoid runtime initialization overhead
    OHOS::Rosen::g_rsInterfaces = &OHOS::Rosen::RSInterfaces::GetInstance();
    return 0;
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (OHOS::Rosen::g_rsInterfaces == nullptr || data == nullptr) {
        return -1;
    }

    FuzzedDataProvider fdp(data, size);

    /* Run your code on data */
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
        case OHOS::Rosen::DO_SET_VIRTUAL_SCREEN_RESOLUTION:
            OHOS::Rosen::DoSetVirtualScreenResolution(fdp);
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_SCREEN_SURFACE:
            OHOS::Rosen::DoSetVirtualScreenSurface(fdp);
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_SCREEN_BLACKLIST:
            OHOS::Rosen::DoSetVirtualScreenBlackList(fdp);
            break;
        case OHOS::Rosen::DO_GET_VIRTUAL_SCREEN_RESOLUTION:
            OHOS::Rosen::DoGetVirtualScreenResolution(fdp);
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_SCREEN_REFRESH_RATE:
            OHOS::Rosen::DoSetVirtualScreenRefreshRate(fdp);
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_SCREEN_AUTO_ROTATION:
            OHOS::Rosen::DoSetVirtualScreenAutoRotation(fdp);
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_SCREEN_USING_STATUS:
            OHOS::Rosen::DoSetVirtualScreenUsingStatus(fdp);
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_MIRROR_SCREEN_CANVAS_ROTATION:
            OHOS::Rosen::DoSetVirtualMirrorScreenCanvasRotation(fdp);
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_MIRROR_SCREEN_SCALE_MODE:
            OHOS::Rosen::DoSetVirtualMirrorScreenScaleMode(fdp);
            break;
        default:
            return -1;
    }
    return 0;
}
