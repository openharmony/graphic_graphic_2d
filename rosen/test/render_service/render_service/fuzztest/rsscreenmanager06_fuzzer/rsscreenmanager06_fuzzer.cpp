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

#include "rsscreenmanager06_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>
#include <memory>

#include "screen_manager/rs_screen_manager.h"
#include "screen_manager/screen_types.h"
#include "screen_manager/rs_screen_hdr_capability.h"

namespace OHOS {
namespace Rosen {

sptr<RSScreenManager> g_screenManager;

namespace {
constexpr uint8_t DO_GET_SCREEN_COLOR_GAMUT = 0;
constexpr uint8_t DO_GET_SCREEN_SUPPORTED_COLOR_GAMUTS = 1;
constexpr uint8_t DO_GET_SCREEN_GAMUT_MAP = 2;
constexpr uint8_t DO_GET_SCREEN_HDR_FORMAT = 3;
constexpr uint8_t DO_GET_SCREEN_SUPPORTED_HDR_FORMATS = 4;
constexpr uint8_t DO_GET_SCREEN_HDR_CAPABILITY = 5;
constexpr uint8_t DO_GET_SCREEN_SUPPORTED_META_DATA_KEYS = 6;
constexpr uint8_t DO_GET_SCREEN_COLOR_SPACE = 7;
constexpr uint8_t DO_GET_SCREEN_SUPPORTED_COLOR_SPACES = 8;
constexpr uint8_t DO_GET_PIXEL_FORMAT = 9;
constexpr uint8_t TARGET_SIZE = 10;

constexpr uint8_t SCREEN_COLOR_GAMUT_SIZE = 11;
constexpr uint8_t SCREEN_GAMUT_MAP_SIZE = 4;
constexpr uint8_t SCREEN_HDR_FORMAT_SIZE = 8;
constexpr uint8_t SCREEN_HDR_METADATA_KEY_SIZE = 14;
constexpr uint8_t GRAPHIC_CM_COLOR_SPACE_TYPE_SIZE = 10;
constexpr uint8_t GRAPHIC_PIXEL_FORMAT_SIZE = 8;
constexpr uint8_t MAX_FUZZ_ENUM_VECTOR_SIZE = 8;

void DoGetScreenColorGamut(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    ScreenColorGamut mode = static_cast<ScreenColorGamut>(fdp.ConsumeIntegral<uint8_t>() % SCREEN_COLOR_GAMUT_SIZE);
    g_screenManager->GetScreenColorGamut(id, mode);
}

void DoGetScreenSupportedColorGamuts(FuzzedDataProvider& fdp)
{
    fdp.ConsumeBool();
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    std::vector<ScreenColorGamut> mode;
    g_screenManager->GetScreenSupportedColorGamuts(id, mode);
}

void DoGetScreenGamutMap(FuzzedDataProvider& fdp)
{
    ScreenGamutMap mode = static_cast<ScreenGamutMap>(fdp.ConsumeIntegral<uint8_t>() % SCREEN_GAMUT_MAP_SIZE);
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    g_screenManager->GetScreenGamutMap(id, mode);
}

void DoGetScreenHDRFormat(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegralInRange<ScreenId>(0, INVALID_SCREEN_ID - 1);
    ScreenHDRFormat hdrFormat = static_cast<ScreenHDRFormat>(fdp.ConsumeIntegral<uint8_t>() % SCREEN_HDR_FORMAT_SIZE);
    g_screenManager->GetScreenHDRFormat(id, hdrFormat);
}

void DoGetScreenSupportedHDRFormats(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    fdp.ConsumeBool();
    uint8_t count = fdp.ConsumeIntegral<uint8_t>() % MAX_FUZZ_ENUM_VECTOR_SIZE;
    std::vector<ScreenHDRFormat> hdrFormats;
    for (uint8_t i = 0; i < count; i++) {
        hdrFormats.push_back(static_cast<ScreenHDRFormat>(fdp.ConsumeIntegral<uint8_t>() % SCREEN_HDR_FORMAT_SIZE));
    }
    g_screenManager->GetScreenSupportedHDRFormats(id, hdrFormats);
}

void DoGetScreenHDRCapability(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    RSScreenHDRCapability screenHdrCapability;
    screenHdrCapability.SetMaxLum(fdp.ConsumeFloatingPoint<float>());
    screenHdrCapability.SetMinLum(fdp.ConsumeFloatingPoint<float>());
    screenHdrCapability.SetMaxAverageLum(fdp.ConsumeFloatingPoint<float>());
    uint8_t count = fdp.ConsumeIntegral<uint8_t>() % MAX_FUZZ_ENUM_VECTOR_SIZE;
    std::vector<ScreenHDRFormat> formats;
    for (uint8_t i = 0; i < count; i++) {
        formats.push_back(static_cast<ScreenHDRFormat>(fdp.ConsumeIntegral<uint8_t>() % SCREEN_HDR_FORMAT_SIZE));
    }
    screenHdrCapability.SetHdrFormats(formats);
    g_screenManager->GetScreenHDRCapability(id, screenHdrCapability);
}

void DoGetScreenSupportedMetaDataKeys(FuzzedDataProvider& fdp)
{
    fdp.ConsumeIntegralInRange<uint8_t>(0, SCREEN_HDR_METADATA_KEY_SIZE);
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    std::vector<ScreenHDRMetadataKey> keys;
    g_screenManager->GetScreenSupportedMetaDataKeys(id, keys);
}

void DoGetScreenColorSpace(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    fdp.ConsumeBool();
    GraphicCM_ColorSpaceType colorSpace = static_cast<GraphicCM_ColorSpaceType>(
        fdp.ConsumeIntegral<uint8_t>() % GRAPHIC_CM_COLOR_SPACE_TYPE_SIZE);
    g_screenManager->GetScreenColorSpace(id, colorSpace);
}

void DoGetScreenSupportedColorSpaces(FuzzedDataProvider& fdp)
{
    fdp.ConsumeIntegral<uint32_t>();
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    std::vector<GraphicCM_ColorSpaceType> colorSpaces;
    g_screenManager->GetScreenSupportedColorSpaces(id, colorSpaces);
}

void DoGetPixelFormat(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    fdp.ConsumeIntegral<uint32_t>();
    GraphicPixelFormat pixelFormat = static_cast<GraphicPixelFormat>(
        fdp.ConsumeIntegral<uint8_t>() % GRAPHIC_PIXEL_FORMAT_SIZE);
    g_screenManager->GetPixelFormat(id, pixelFormat);
}

} // namespace

} // namespace Rosen
} // namespace OHOS

extern "C" int LLVMFuzzerInitialize(int* argc, char*** argv)
{
    OHOS::Rosen::g_screenManager = OHOS::sptr<OHOS::Rosen::RSScreenManager>::MakeSptr();
    if (!OHOS::Rosen::g_screenManager) {
        return -1;
    }
    auto runner = OHOS::AppExecFwk::EventRunner::Create(false);
    auto handler = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner);
    OHOS::Rosen::g_screenManager->Init(handler);
    return 0;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return -1;
    }
    if (!OHOS::Rosen::g_screenManager) {
        return -1;
    }

    FuzzedDataProvider fdp(data, size);

    uint8_t tarPos = fdp.ConsumeIntegral<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_GET_SCREEN_COLOR_GAMUT:
            OHOS::Rosen::DoGetScreenColorGamut(fdp);
            break;
        case OHOS::Rosen::DO_GET_SCREEN_SUPPORTED_COLOR_GAMUTS:
            OHOS::Rosen::DoGetScreenSupportedColorGamuts(fdp);
            break;
        case OHOS::Rosen::DO_GET_SCREEN_GAMUT_MAP:
            OHOS::Rosen::DoGetScreenGamutMap(fdp);
            break;
        case OHOS::Rosen::DO_GET_SCREEN_HDR_FORMAT:
            OHOS::Rosen::DoGetScreenHDRFormat(fdp);
            break;
        case OHOS::Rosen::DO_GET_SCREEN_SUPPORTED_HDR_FORMATS:
            OHOS::Rosen::DoGetScreenSupportedHDRFormats(fdp);
            break;
        case OHOS::Rosen::DO_GET_SCREEN_HDR_CAPABILITY:
            OHOS::Rosen::DoGetScreenHDRCapability(fdp);
            break;
        case OHOS::Rosen::DO_GET_SCREEN_SUPPORTED_META_DATA_KEYS:
            OHOS::Rosen::DoGetScreenSupportedMetaDataKeys(fdp);
            break;
        case OHOS::Rosen::DO_GET_SCREEN_COLOR_SPACE:
            OHOS::Rosen::DoGetScreenColorSpace(fdp);
            break;
        case OHOS::Rosen::DO_GET_SCREEN_SUPPORTED_COLOR_SPACES:
            OHOS::Rosen::DoGetScreenSupportedColorSpaces(fdp);
            break;
        case OHOS::Rosen::DO_GET_PIXEL_FORMAT:
            OHOS::Rosen::DoGetPixelFormat(fdp);
            break;
        default:
            return -1;
    }
    return 0;
}