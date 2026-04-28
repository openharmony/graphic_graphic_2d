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

#include "nativecolorspacemanager_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "color.h"
#include "color_space.h"
#include "color_space_convertor.h"
#include "native_color_space_manager.h"

namespace OHOS {
namespace ColorSpaceManager {
namespace {
const uint8_t DO_COLOR = 0;
const uint8_t DO_CREATE_FROM_PRIMARIES = 1;
const uint8_t DO_GET_NAME = 2;
const uint8_t DO_GET_GAMMA = 3;
const uint8_t TARGET_SIZE = 4;
}

void DoColorFuzzTest(FuzzedDataProvider& fdp)
{
    Color randomColor = Color(fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>(),
        fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>());
    ColorSpace csObject = ColorSpace(fdp.ConsumeIntegral<ColorSpaceName>());
    ColorSpaceConvertor convertor = ColorSpaceConvertor(fdp.ConsumeIntegral<ColorSpaceName>(),
        fdp.ConsumeIntegral<ColorSpaceName>(), fdp.ConsumeIntegral<GamutMappingMode>());
    (void)randomColor.Convert(fdp.ConsumeIntegral<ColorSpaceName>());
    (void)randomColor.Convert(csObject);
    Color convertColor = randomColor.Convert(convertor);
    (void)randomColor.ColorEqual(convertColor);
}

void DoCreateFromPrimariesAndGamma(FuzzedDataProvider& fdp)
{
    OH_NativeColorSpaceManager* nativeColorSpaceManager = nullptr;
    ColorSpaceName colorSpaceName = ColorSpaceName::NONE;
    nativeColorSpaceManager = OH_NativeColorSpaceManager_CreateFromName(colorSpaceName);
    ColorSpacePrimaries primaries = {0.640f, 0.330f, 0.210f, 0.710f, 0.150f, 0.060f, 0.3127f, 0.3290f};
    float gamma = fdp.ConsumeFloatingPoint<float>();
    nativeColorSpaceManager->OH_NativeColorSpaceManager_CreateFromPrimariesAndGamma(primaries, gamma);
}

void DoGetColorSpaceName(FuzzedDataProvider& fdp)
{
    ColorSpaceName colorSpaceName = fdp.ConsumeIntegral<ColorSpaceName>();
    OH_NativeColorSpaceManager* nativeColorSpaceManager =
        OH_NativeColorSpaceManager_CreateFromName(colorSpaceName);
    (void)static_cast<ColorSpaceName>(
        OH_NativeColorSpaceManager_GetColorSpaceName(nativeColorSpaceManager));
}

void DoGetGamma(FuzzedDataProvider& fdp)
{
    ColorSpaceName colorSpaceName = fdp.ConsumeIntegral<ColorSpaceName>();
    OH_NativeColorSpaceManager* nativeColorSpaceManager =
        OH_NativeColorSpaceManager_CreateFromName(colorSpaceName);
    (void)OH_NativeColorSpaceManager_GetGamma(nativeColorSpaceManager);
}

} // namespace ColorSpaceManager
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return -1;
    }

    FuzzedDataProvider fdp(data, size);
    uint8_t tarPos = fdp.ConsumeIntegral<uint8_t>() % OHOS::ColorSpaceManager::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::ColorSpaceManager::DO_COLOR:
            OHOS::ColorSpaceManager::DoColorFuzzTest(fdp);
            break;
        case OHOS::ColorSpaceManager::DO_CREATE_FROM_PRIMARIES:
            OHOS::ColorSpaceManager::DoCreateFromPrimariesAndGamma(fdp);
            break;
        case OHOS::ColorSpaceManager::DO_GET_NAME:
            OHOS::ColorSpaceManager::DoGetColorSpaceName(fdp);
            break;
        case OHOS::ColorSpaceManager::DO_GET_GAMMA:
            OHOS::ColorSpaceManager::DoGetGamma(fdp);
            break;
        default:
            break;
    }
    return 0;
}
