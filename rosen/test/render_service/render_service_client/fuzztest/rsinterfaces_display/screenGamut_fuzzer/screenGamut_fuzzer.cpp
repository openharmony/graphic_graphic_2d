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

#include "screenGamut_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "transaction/rs_interfaces.h"

namespace OHOS {
namespace Rosen {

RSInterfaces* g_rsInterfaces = nullptr;

namespace {
const uint8_t DO_GET_SCREEN_SUPPORTED_GAMUTS = 0;
const uint8_t DO_GET_SCREEN_GAMUT = 1;
const uint8_t DO_SET_SCREEN_GAMUT = 2;
const uint8_t DO_GET_SCREEN_GAMUT_MAP = 3;
const uint8_t DO_SET_SCREEN_GAMUT_MAP = 4;
const uint8_t TARGET_SIZE = 5;

void DoGetScreenSupportedGamuts(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    std::vector<ScreenColorGamut> gamuts;
    g_rsInterfaces->GetScreenSupportedColorGamuts(id, gamuts);
}

void DoGetScreenGamut(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    g_rsInterfaces->GetScreenGamut(id);
}

void DoSetScreenGamut(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    ScreenColorGamut screenGamut = static_cast<ScreenColorGamut>(fdp.ConsumeIntegral<uint32_t>());
    g_rsInterfaces->SetScreenGamut(id, screenGamut);
}

void DoGetScreenGamutMap(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    ScreenGamutMap screenGamutMap;
    g_rsInterfaces->GetScreenGamutMap(id, screenGamutMap);
}

void DoSetScreenGamutMap(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    ScreenGamutMap screenGamutMap = static_cast<ScreenGamutMap>(fdp.ConsumeIntegral<uint32_t>());
    g_rsInterfaces->SetScreenGamutMap(id, screenGamutMap);
}

} // namespace

} // namespace Rosen
} // namespace OHOS

extern "C" int LLVMFuzzerInitialize(int* argc, char*** argv)
{
    OHOS::Rosen::g_rsInterfaces = &OHOS::Rosen::RSInterfaces::GetInstance();
    return 0;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (OHOS::Rosen::g_rsInterfaces == nullptr || data == nullptr) {
        return -1;
    }

    FuzzedDataProvider fdp(data, size);
    uint8_t tarPos = fdp.ConsumeIntegral<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_GET_SCREEN_SUPPORTED_GAMUTS:
            OHOS::Rosen::DoGetScreenSupportedGamuts(fdp);
            break;
        case OHOS::Rosen::DO_GET_SCREEN_GAMUT:
            OHOS::Rosen::DoGetScreenGamut(fdp);
            break;
        case OHOS::Rosen::DO_SET_SCREEN_GAMUT:
            OHOS::Rosen::DoSetScreenGamut(fdp);
            break;
        case OHOS::Rosen::DO_GET_SCREEN_GAMUT_MAP:
            OHOS::Rosen::DoGetScreenGamutMap(fdp);
            break;
        case OHOS::Rosen::DO_SET_SCREEN_GAMUT_MAP:
            OHOS::Rosen::DoSetScreenGamutMap(fdp);
            break;
        default:
            return -1;
    }
    return 0;
}
