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

#include "screenHdr_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "display_engine/rs_luminance_control.h"

#include "transaction/rs_interfaces.h"

namespace OHOS {
namespace Rosen {

RSInterfaces* g_rsInterfaces = nullptr;

namespace {
const uint8_t DO_GET_SCREEN_HDR_STATUS = 0;
const uint8_t DO_GET_SCREEN_HDR_CAPABILITY = 1;
const uint8_t DO_GET_SCREEN_SUPPORTED_HDR_FORMATS = 2;
const uint8_t DO_GET_SCREEN_HDR_FORMAT = 3;
const uint8_t DO_SET_SCREEN_HDR_FORMAT = 4;
const uint8_t TARGET_SIZE = 5;

void DoGetScreenHdrStatus(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    HdrStatus hdrStatus;
    g_rsInterfaces->GetScreenHDRStatus(id, hdrStatus);
}

void DoGetScreenHdrCapability(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    RSScreenHDRCapability screenHdrCapability;
    g_rsInterfaces->GetScreenHDRCapability(id, screenHdrCapability);
}

void DoGetScreenSupportedHdrFormats(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    std::vector<ScreenHDRFormat> hdrFormats;
    g_rsInterfaces->GetScreenSupportedHDRFormats(id, hdrFormats);
}

void DoGetScreenHdrFormat(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    ScreenHDRFormat hdrFormat;
    g_rsInterfaces->GetScreenHDRFormat(id, hdrFormat);
}

void DoSetScreenHdrFormat(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    int32_t modeIdx = fdp.ConsumeIntegral<int32_t>();
    g_rsInterfaces->SetScreenHDRFormat(id, modeIdx);
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
        case OHOS::Rosen::DO_GET_SCREEN_HDR_STATUS:
            OHOS::Rosen::DoGetScreenHdrStatus(fdp);
            break;
        case OHOS::Rosen::DO_GET_SCREEN_HDR_CAPABILITY:
            OHOS::Rosen::DoGetScreenHdrCapability(fdp);
            break;
        case OHOS::Rosen::DO_GET_SCREEN_SUPPORTED_HDR_FORMATS:
            OHOS::Rosen::DoGetScreenSupportedHdrFormats(fdp);
            break;
        case OHOS::Rosen::DO_GET_SCREEN_HDR_FORMAT:
            OHOS::Rosen::DoGetScreenHdrFormat(fdp);
            break;
        case OHOS::Rosen::DO_SET_SCREEN_HDR_FORMAT:
            OHOS::Rosen::DoSetScreenHdrFormat(fdp);
            break;
        default:
            return -1;
    }
    return 0;
}
