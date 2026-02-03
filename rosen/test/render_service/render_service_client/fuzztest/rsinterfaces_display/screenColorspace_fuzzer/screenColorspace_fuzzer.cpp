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

#include "screenColorspace_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "transaction/rs_interfaces.h"
#include "color_manager/color_space.h"

namespace OHOS {
namespace Rosen {

RSInterfaces* g_rsInterfaces = nullptr;

namespace {
const uint8_t DO_GET_SCREEN_SUPPORTED_COLORSPACES = 0;
const uint8_t DO_GET_SCREEN_COLORSPACE = 1;
const uint8_t DO_SET_SCREEN_COLORSPACE = 2;
const uint8_t DO_SET_COLOR_FOLLOW = 3;
const uint8_t TARGET_SIZE = 4;
constexpr size_t STR_LEN = 10;

void DoGetScreenSupportedColorSpaces(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    std::vector<GraphicCM_ColorSpaceType> colorSpaces;
    g_rsInterfaces->GetScreenSupportedColorSpaces(id, colorSpaces);
}

void DoGetScreenColorSpace(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    GraphicCM_ColorSpaceType colorSpace;
    g_rsInterfaces->GetScreenColorSpace(id, colorSpace);
}

void DoSetScreenColorSpace(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    GraphicCM_ColorSpaceType colorSpace = static_cast<GraphicCM_ColorSpaceType>(fdp.ConsumeIntegral<uint32_t>());
    g_rsInterfaces->SetScreenColorSpace(id, colorSpace);
}

void DoSetColorFollow(FuzzedDataProvider& fdp)
{
    std::string nodeIdStr = fdp.ConsumeRandomLengthString(STR_LEN);
    bool isFollow = fdp.ConsumeBool();
    g_rsInterfaces->SetColorFollow(nodeIdStr, isFollow);
}

} // namespace

} // namespace Rosen
} // namespace OHOS

/* Fuzzer environment initialization */
extern "C" int LLVMFuzzerInitialize(int* argc, char*** argv)
{
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
    uint8_t tarPos = fdp.ConsumeIntegral<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_GET_SCREEN_SUPPORTED_COLORSPACES:
            OHOS::Rosen::DoGetScreenSupportedColorSpaces(fdp);
            break;
        case OHOS::Rosen::DO_GET_SCREEN_COLORSPACE:
            OHOS::Rosen::DoGetScreenColorSpace(fdp);
            break;
        case OHOS::Rosen::DO_SET_SCREEN_COLORSPACE:
            OHOS::Rosen::DoSetScreenColorSpace(fdp);
            break;
        case OHOS::Rosen::DO_SET_COLOR_FOLLOW:
            OHOS::Rosen::DoSetColorFollow(fdp);
            break;
        default:
            return -1;
    }
    return 0;
}
