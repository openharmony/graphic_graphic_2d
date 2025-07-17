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

#include "rsinterfaces006_fuzzer.h"

#include <securec.h>

#include "transaction/rs_interfaces.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t DO_GET_SCREEN_ACTIVE_MODE = 0;
const uint8_t DO_GET_SCREEN_SUPPORTED_MODES = 1;
const uint8_t DO_GET_SCREEN_CAPABILITY = 2;
const uint8_t DO_GET_SCREEN_POWER_STATUS = 3;
const uint8_t DO_GET_SCREEN_BACK_LIGHT = 4;
const uint8_t DO_GET_SCREEN_DATA = 5;
const uint8_t DO_GET_SCREEN_SUPPORTED_GAMUTS = 6;
const uint8_t DO_GET_SCREEN_SUPPORTED_METADATAKEYS = 7;
const uint8_t DO_GET_SCREEN_GAMUT = 8;
const uint8_t DO_GET_SCREEN_GAMUT_MAP = 9;
const uint8_t TARGET_SIZE = 10;

const uint8_t* DATA = nullptr;
size_t g_size = 0;
size_t g_pos;
constexpr uint8_t SCREEN_COLOR_GAMUT_SIZE = 12;
constexpr uint8_t SCREEN_HDR_MATES_DATA_KEYS_SIZE = 12;
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

namespace Mock {

} // namespace Mock

void DoGetScreenActiveMode()
{
#ifndef ROSEN_ARKUI_X
    ScreenId id = GetData<ScreenId>();
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.GetScreenActiveMode(id);
#endif
}

void DoGetScreenSupportedModes()
{
#ifndef ROSEN_ARKUI_X
    ScreenId id = GetData<ScreenId>();
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.GetScreenSupportedModes(id);
#endif
}

void DoGetScreenCapability()
{
#ifndef ROSEN_ARKUI_X
    ScreenId id = GetData<ScreenId>();
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.GetScreenCapability(id);
#endif
}

void DoGetScreenPowerStatus()
{
#ifndef ROSEN_ARKUI_X
    ScreenId id = GetData<ScreenId>();
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.GetScreenPowerStatus(id);
#endif
}

void DoGetScreenBacklight()
{
#ifndef ROSEN_ARKUI_X
    ScreenId id = GetData<ScreenId>();
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.GetScreenBacklight(id);
#endif
}

void DoGetScreenData()
{
#ifndef ROSEN_ARKUI_X
    ScreenId id = GetData<ScreenId>();
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.GetScreenData(id);
#endif
}

void DoGetScreenSupportedColorGamuts()
{
    ScreenId id = GetData<ScreenId>();
    ScreenColorGamut type = static_cast<ScreenColorGamut>(GetData<uint8_t>() % SCREEN_COLOR_GAMUT_SIZE);
    std::vector<ScreenColorGamut> mode = { type };
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.GetScreenSupportedColorGamuts(id, mode);
}

void DoGetScreenSupportedMetaDataKeys()
{
    ScreenId id = GetData<ScreenId>();
    ScreenHDRMetadataKey type = static_cast<ScreenHDRMetadataKey>(GetData<uint8_t>() % SCREEN_HDR_MATES_DATA_KEYS_SIZE);
    std::vector<ScreenHDRMetadataKey> keys = { type };
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.GetScreenSupportedMetaDataKeys(id, keys);
}

void DoGetScreenColorGamut()
{
    ScreenId id = GetData<ScreenId>();
    ScreenColorGamut mode = static_cast<ScreenColorGamut>(GetData<uint8_t>() % SCREEN_COLOR_GAMUT_SIZE);
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.GetScreenColorGamut(id, mode);
}

void DoGetScreenGamutMap()
{
    ScreenId id = GetData<ScreenId>();
    ScreenColorGamut mode = static_cast<ScreenColorGamut>(GetData<uint8_t>() % SCREEN_GAMUT_MAP_SIZE);
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.GetScreenGamutMap(id, mode);
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
        case OHOS::Rosen::DO_GET_SCREEN_ACTIVE_MODE:
            OHOS::Rosen::DoGetScreenActiveMode();
            break;
        case OHOS::Rosen::DO_GET_SCREEN_SUPPORTED_MODES:
            OHOS::Rosen::DoGetScreenSupportedModes();
            break;
        case OHOS::Rosen::DO_GET_SCREEN_CAPABILITY:
            OHOS::Rosen::DoGetScreenCapability();
            break;
        case OHOS::Rosen::DO_GET_SCREEN_POWER_STATUS:
            OHOS::Rosen::DoGetScreenPowerStatus();
            break;
        case OHOS::Rosen::DO_GET_SCREEN_BACK_LIGHT:
            OHOS::Rosen::DoGetScreenBacklight();
            break;
        case OHOS::Rosen::DO_GET_SCREEN_DATA:
            OHOS::Rosen::DoGetScreenData();
            break;
        case OHOS::Rosen::DO_GET_SCREEN_SUPPORTED_GAMUTS:
            OHOS::Rosen::DoGetScreenSupportedColorGamuts();
            break;
        case OHOS::Rosen::DO_GET_SCREEN_SUPPORTED_METADATAKEYS:
            OHOS::Rosen::DoGetScreenSupportedMetaDataKeys();
            break;
        case OHOS::Rosen::DO_GET_SCREEN_GAMUT:
            OHOS::Rosen::DoGetScreenColorGamut();
            break;
        case OHOS::Rosen::DO_GET_SCREEN_GAMUT_MAP:
            OHOS::Rosen::DoGetScreenGamutMap();
            break;
        default:
            return -1;
    }
    return 0;
}
