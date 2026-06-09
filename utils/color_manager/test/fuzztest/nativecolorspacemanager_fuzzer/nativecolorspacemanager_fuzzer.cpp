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

#include "native_color_space_manager.h"

namespace OHOS {
namespace ColorSpaceManager {
namespace {
constexpr uint8_t DO_CREATE_FROM_PRIMARIES = 0;
constexpr uint8_t DO_GET_NAME = 1;
constexpr uint8_t DO_GET_GAMMA = 2;
constexpr uint8_t DO_GET_WHITE_POINT = 3;
constexpr uint8_t DO_DESTROY = 4;
constexpr uint8_t TARGET_SIZE = 5;
constexpr uint8_t COLOR_SPACE_NAME_RANGE = 29;
constexpr uint8_t COLOR_SPACE_NAME_SKIPPED = 27;

void DestroyColorSpaceManager(OH_NativeColorSpaceManager* mgr)
{
    if (mgr != nullptr) {
        OH_NativeColorSpaceManager_Destroy(mgr);
    }
}

ColorSpaceName ConsumeColorSpaceName(FuzzedDataProvider& fdp)
{
    uint8_t rawValue = fdp.ConsumeIntegral<uint8_t>() % COLOR_SPACE_NAME_RANGE;
    if (rawValue == COLOR_SPACE_NAME_SKIPPED) {
        rawValue = COLOR_SPACE_NAME_SKIPPED + 1;
    }
    return static_cast<ColorSpaceName>(rawValue);
}
}

void DoCreateFromPrimariesAndGamma(FuzzedDataProvider& fdp)
{
    ColorSpacePrimaries primaries = {
        fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>(),
        fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>(),
        fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>(),
        fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>()};
    float gamma = fdp.ConsumeFloatingPoint<float>();
    OH_NativeColorSpaceManager* mgr = OH_NativeColorSpaceManager_CreateFromPrimariesAndGamma(primaries, gamma);
    DestroyColorSpaceManager(mgr);
}

void DoGetColorSpaceName(FuzzedDataProvider& fdp)
{
    ColorSpaceName colorSpaceName = ConsumeColorSpaceName(fdp);
    OH_NativeColorSpaceManager* mgr = OH_NativeColorSpaceManager_CreateFromName(colorSpaceName);
    (void)OH_NativeColorSpaceManager_GetColorSpaceName(mgr);
    DestroyColorSpaceManager(mgr);
}

void DoGetGamma(FuzzedDataProvider& fdp)
{
    ColorSpaceName colorSpaceName = ConsumeColorSpaceName(fdp);
    OH_NativeColorSpaceManager* mgr = OH_NativeColorSpaceManager_CreateFromName(colorSpaceName);
    (void)OH_NativeColorSpaceManager_GetGamma(mgr);
    DestroyColorSpaceManager(mgr);
}

void DoGetWhitePoint(FuzzedDataProvider& fdp)
{
    ColorSpaceName colorSpaceName = ConsumeColorSpaceName(fdp);
    OH_NativeColorSpaceManager* mgr = OH_NativeColorSpaceManager_CreateFromName(colorSpaceName);
    (void)OH_NativeColorSpaceManager_GetWhitePoint(mgr);
    DestroyColorSpaceManager(mgr);
}

void DoDestroy(FuzzedDataProvider& fdp)
{
    OH_NativeColorSpaceManager_Destroy(nullptr);
    ColorSpaceName colorSpaceName = ConsumeColorSpaceName(fdp);
    OH_NativeColorSpaceManager* mgr = OH_NativeColorSpaceManager_CreateFromName(colorSpaceName);
    OH_NativeColorSpaceManager_Destroy(mgr);
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
        case OHOS::ColorSpaceManager::DO_CREATE_FROM_PRIMARIES:
            OHOS::ColorSpaceManager::DoCreateFromPrimariesAndGamma(fdp);
            break;
        case OHOS::ColorSpaceManager::DO_GET_NAME:
            OHOS::ColorSpaceManager::DoGetColorSpaceName(fdp);
            break;
        case OHOS::ColorSpaceManager::DO_GET_GAMMA:
            OHOS::ColorSpaceManager::DoGetGamma(fdp);
            break;
        case OHOS::ColorSpaceManager::DO_GET_WHITE_POINT:
            OHOS::ColorSpaceManager::DoGetWhitePoint(fdp);
            break;
        case OHOS::ColorSpaceManager::DO_DESTROY:
            OHOS::ColorSpaceManager::DoDestroy(fdp);
            break;
        default:
            break;
    }
    return 0;
}
