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

#include "getscreeninfo_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "transaction/rs_interfaces.h"

namespace OHOS {
namespace Rosen {

// Global RSInterfaces instance, initialized in LLVMFuzzerInitialize
RSInterfaces* g_rsInterfaces = nullptr;

namespace {
const uint8_t DO_GET_SCREEN_ACTIVE_MODE = 0;
const uint8_t DO_GET_SCREEN_SUPPORTED_MODES = 1;
const uint8_t DO_GET_SCREEN_CAPABILITY = 2;
const uint8_t DO_GET_SCREEN_POWER_STATUS = 3;
const uint8_t DO_GET_SCREEN_BACK_LIGHT = 4;
const uint8_t DO_GET_SCREEN_DATA = 5;
const uint8_t DO_GET_SCREEN_SUPPORTED_METADATAKEYS = 6;
const uint8_t DO_GET_SCREEN_TYPE = 7;
const uint8_t DO_GET_DISPLAY_IDENTIFICATION_DATA = 8;
const uint8_t DO_GET_PANEL_POWER_STATUS = 9;
const uint8_t DO_GET_ROG_SCREEN_RESOLUTION = 10;
const uint8_t TARGET_SIZE = 11;

void DoGetScreenActiveMode(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    g_rsInterfaces->GetScreenActiveMode(id);
}

void DoGetScreenSupportedModes(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    g_rsInterfaces->GetScreenSupportedModes(id);
}

void DoGetScreenCapability(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    g_rsInterfaces->GetScreenCapability(id);
}

void DoGetScreenPowerStatus(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    g_rsInterfaces->GetScreenPowerStatus(id);
}

void DoGetScreenBackLight(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    g_rsInterfaces->GetScreenBacklight(id);
}

void DoGetScreenData(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    g_rsInterfaces->GetScreenData(id);
}

void DoGetScreenSupportedMetaDataKeys(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    uint8_t listSize = fdp.ConsumeIntegral<uint8_t>();
    std::vector<ScreenHDRMetadataKey> keys;
    for (auto i = 0; i < listSize; i++) {
        keys.push_back(static_cast<ScreenHDRMetadataKey>(fdp.ConsumeIntegral<uint32_t>()));
    }
    g_rsInterfaces->GetScreenSupportedMetaDataKeys(id, keys);
}

void DoGetScreenType(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    RSScreenType screenType = static_cast<RSScreenType>(fdp.ConsumeIntegral<uint32_t>());
    g_rsInterfaces->GetScreenType(id, screenType);
}

void DoGetDisplayIdentificationData(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    uint8_t outPort = fdp.ConsumeIntegral<uint8_t>();
    uint8_t listSize = fdp.ConsumeIntegral<uint8_t>();
    std::vector<uint8_t> edidData;
    for (auto i = 0; i < listSize; i++) {
        edidData.push_back(fdp.ConsumeIntegral<uint8_t>());
    }
    g_rsInterfaces->GetDisplayIdentificationData(id, outPort, edidData);
}

void DoGetPanelPowerStatus(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    g_rsInterfaces->GetPanelPowerStatus(id);
}

void DoGetRogScreenResolution(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    uint32_t width = fdp.ConsumeIntegral<uint32_t>();
    uint32_t height = fdp.ConsumeIntegral<uint32_t>();
    g_rsInterfaces->GetRogScreenResolution(id, width, height);
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
        case OHOS::Rosen::DO_GET_SCREEN_BACK_LIGHT:
            OHOS::Rosen::DoGetScreenBackLight(fdp);
            break;
        case OHOS::Rosen::DO_GET_SCREEN_DATA:
            OHOS::Rosen::DoGetScreenData(fdp);
            break;
        case OHOS::Rosen::DO_GET_SCREEN_SUPPORTED_METADATAKEYS:
            OHOS::Rosen::DoGetScreenSupportedMetaDataKeys(fdp);
            break;
        case OHOS::Rosen::DO_GET_SCREEN_TYPE:
            OHOS::Rosen::DoGetScreenType(fdp);
            break;
        case OHOS::Rosen::DO_GET_DISPLAY_IDENTIFICATION_DATA:
            OHOS::Rosen::DoGetDisplayIdentificationData(fdp);
            break;
        case OHOS::Rosen::DO_GET_PANEL_POWER_STATUS:
            OHOS::Rosen::DoGetPanelPowerStatus(fdp);
            break;
        case OHOS::Rosen::DO_GET_ROG_SCREEN_RESOLUTION:
            OHOS::Rosen::DoGetRogScreenResolution(fdp);
            break;
        default:
            return -1;
    }
    return 0;
}
