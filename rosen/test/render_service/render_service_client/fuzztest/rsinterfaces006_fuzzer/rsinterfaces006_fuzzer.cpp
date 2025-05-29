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

template<>
std::string GetData()
{
    size_t objectSize = GetData<uint8_t>();
    std::string object(objectSize, '\0');
    if (DATA == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    object.assign(reinterpret_cast<const char*>(DATA + g_pos), objectSize);
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
{}

void DoGetScreenSupportedModes()
{}

void DoGetScreenCapability()
{}

void DoGetScreenPowerStatus()
{}

void DoGetScreenBacklight()
{}

void DoGetScreenData()
{}

void DoGetScreenSupportedColorGamuts()
{}

void DoGetScreenSupportedMetaDataKeys()
{}

void DoGetScreenColorGamut()
{}

void DoGetScreenGamutMap()
{}
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
