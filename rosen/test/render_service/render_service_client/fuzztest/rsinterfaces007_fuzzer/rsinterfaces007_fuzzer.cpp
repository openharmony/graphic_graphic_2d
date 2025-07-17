/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "rsinterfaces007_fuzzer.h"

#include <securec.h>

#include "transaction/rs_interfaces.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t DO_SET_SCREEN_GAMUT = 0;
const uint8_t DO_SET_SCREEN_GAMUT_MAP = 1;
const uint8_t DO_SET_SCREEN_CORRECTION = 2;
const uint8_t DO_SET_VIRTUAL_MIRROR_SCREEN_CANVAS_ROTATION = 3;
const uint8_t DO_SET_VIRTUAL_MIRROR_SCREEN_SCALE_MODE = 4;
const uint8_t DO_SET_GLOBAL_DARK_COLOR_MODE = 5;
const uint8_t TARGET_SIZE = 6;

const uint8_t* DATA = nullptr;
size_t g_size = 0;
size_t g_pos;
constexpr uint8_t SCREEN_ROTATION_SIZE = 5;
constexpr uint8_t SCREEN_SCALE_MODE_SIZE = 3;
constexpr uint8_t SCREEN_GAMUT_MAP_SIZE = 4;

template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (DATA == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, DATA + g_pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_pos += objectSize;
    return object;
}

bool Init(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    DATA = data;
    g_size = size;
    g_pos = 0;
    return true;
}
} // namespace

void DoSetScreenColorGamut()
{
    ScreenId id = GetData<ScreenId>();
    int32_t modeIdx = GetData<int32_t>();
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetScreenColorGamut(id, modeIdx);
}

void DoSetScreenGamutMap()
{
    ScreenId id = GetData<ScreenId>();
    ScreenGamutMap mode = static_cast<ScreenGamutMap>(GetData<uint8_t>() % SCREEN_GAMUT_MAP_SIZE);
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetScreenGamutMap(id, mode);
}

void DoSetScreenCorrection()
{
    ScreenId id = GetData<ScreenId>();
    ScreenGamutMap mode = static_cast<ScreenGamutMap>(GetData<uint8_t>() % SCREEN_ROTATION_SIZE);
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetScreenCorrection(id, mode);
}

void DoSetVirtualMirrorScreenCanvasRotation()
{
    ScreenId id = GetData<ScreenId>();
    bool canvasRotation = GetData<bool>();
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetVirtualMirrorScreenCanvasRotation(id, canvasRotation);
}

void DoSetVirtualMirrorScreenScaleMode()
{
    ScreenId id = GetData<ScreenId>();
    ScreenScaleMode mode = static_cast<ScreenScaleMode>(GetData<uint8_t>() % SCREEN_SCALE_MODE_SIZE);
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetVirtualMirrorScreenScaleMode(id, mode);
}

void DoSetGlobalDarkColorMode()
{
    bool isDark = GetData<bool>();
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetGlobalDarkColorMode(isDark);
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (!OHOS::Rosen::Init(data, size)) {
        return -1;
    }
    /* Run your code on data */
    uint8_t tarPos = OHOS::Rosen::GetData<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_SET_SCREEN_GAMUT:
            OHOS::Rosen::DoSetScreenColorGamut();
            break;
        case OHOS::Rosen::DO_SET_SCREEN_GAMUT_MAP:
            OHOS::Rosen::DoSetScreenGamutMap();
            break;
        case OHOS::Rosen::DO_SET_SCREEN_CORRECTION:
            OHOS::Rosen::DoSetScreenCorrection();
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_MIRROR_SCREEN_CANVAS_ROTATION:
            OHOS::Rosen::DoSetVirtualMirrorScreenCanvasRotation();
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_MIRROR_SCREEN_SCALE_MODE:
            OHOS::Rosen::DoSetVirtualMirrorScreenScaleMode();
            break;
        case OHOS::Rosen::DO_SET_GLOBAL_DARK_COLOR_MODE:
            OHOS::Rosen::DoSetGlobalDarkColorMode();
            break;
        default:
            return -1;
    }
    return 0;
}
