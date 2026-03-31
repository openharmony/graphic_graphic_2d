/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "rsscreenmanager_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>
#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif
#include "securec.h"

#include "screen_manager/rs_screen_manager.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint8_t SCREEN_POWER_STATUS_SIZE = 11;
constexpr size_t STRING_LEN = 10;
constexpr uint8_t SCREEN_CONSTRAINT_TYPE_SIZE = 4;
constexpr uint8_t SCREEN_COLOR_GAMUT_SIZE = 12;
constexpr uint8_t SCREEN_GAMUT_MAP_SIZE = 4;
constexpr uint8_t SCREEN_ROTATION_SIZE = 5;
constexpr uint8_t SCREEN_SCREEN_TYPE_SIZE = 4;
constexpr uint8_t SCREEN_HDR_FORMAT_SIZE = 8;
constexpr uint8_t GRAPHIC_CM_COLOR_SPACE_TYPE_SIZE = 32;
constexpr uint8_t SCREEN_SCALE_MODE_SIZE = 3;
constexpr uint8_t GRAPHIC_PIXEL_FORMAT_SIZE = 43;
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos = 0;
} // namespace

sptr<RSScreenManager> screenManager_ = sptr<RSScreenManager>::MakeSptr();
/*
 * describe: get data from outside untrusted data(g_data) which size is according to sizeof(T)
 * tips: only support basic type
 */
template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (g_data == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, g_data + g_pos, objectSize);
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
    g_data = data;
    g_size = size;
    g_pos = 0;
    return true;
}

void InitScreenManger()
{
    auto runner = AppExecFwk::EventRunner::Create(false);
    auto handler = std::make_shared<AppExecFwk::EventHandler>(runner);
    screenManager_->Init(handler);
}

void GetActiveScreenId()
{
    screenManager_->GetActiveScreenId();
}

void SetVirtualScreenRefreshRate()
{
    // get data
    ScreenId screenId = GetData<ScreenId>();
    uint32_t maxRefreshRate = GetData<uint32_t>();
    uint32_t actualRefreshRate = GetData<uint32_t>();
    screenManager_->SetVirtualScreenRefreshRate(screenId, maxRefreshRate, actualRefreshRate);
}

void GetVirtualScreenResolution()
{
    ScreenId screenId = GetData<ScreenId>();
    RSVirtualScreenResolution virtualScreenResolution;
    screenManager_->GetVirtualScreenResolution(screenId, virtualScreenResolution);
}

void GetScreenActiveMode()
{
    ScreenId screenId = GetData<ScreenId>();
    RSScreenModeInfo screenModeInfo;
    screenManager_->GetScreenActiveMode(screenId, screenModeInfo);
}

void GetScreenSupportedModes()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    screenManager_->GetScreenSupportedModes(screenId);
}

void GetScreenCapability()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    screenManager_->GetScreenCapability(screenId);
}

void GetScreenPowerStatus()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    screenManager_->GetScreenPowerStatus(screenId);
}

void GetDefaultScreenId()
{
    screenManager_->GetDefaultScreenId();
}

void GetAllScreenIds()
{
    screenManager_->GetAllScreenIds();
}

void CreateVirtualScreen()
{
    std::string name(STRING_LEN, GetData<char>());
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    sptr<Surface> surface;
    ScreenId mirrorId = GetData<ScreenId>();
    int32_t flags = GetData<int32_t>();
    NodeId nodeId = GetData<NodeId>();
    std::vector<NodeId> whiteList = { nodeId };
    screenManager_->CreateVirtualScreen(name, width, height, surface, mirrorId, flags, whiteList);
}

void SetVirtualScreenAutoRotation()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    bool isAutoRotation = GetData<bool>();
    screenManager_->SetVirtualScreenAutoRotation(screenId, isAutoRotation);
}

void SetVirtualScreenBlackList()
{
    ScreenId screenId = GetData<ScreenId>();
    uint64_t id = GetData<uint64_t>();
    std::vector<uint64_t> blocklist = { id };
    screenManager_->SetVirtualScreenBlackList(screenId, blocklist);
}

void SetVirtualScreenTypeBlackList()
{
    ScreenId screenId = GetData<ScreenId>();
    uint8_t id = GetData<uint8_t>();
    std::vector<uint8_t> typeBlackList = { id };
    screenManager_->SetVirtualScreenTypeBlackList(screenId, typeBlackList);
}

void AddVirtualScreenBlackList()
{
    ScreenId screenId = GetData<ScreenId>();
    uint64_t nodeId = GetData<uint64_t>();
    std::vector<uint64_t> blocklist = { nodeId };
    screenManager_->AddVirtualScreenBlackList(screenId, blocklist);
}

void RemoveVirtualScreenBlackList()
{
    ScreenId screenId = GetData<ScreenId>();
    uint64_t nodeId = GetData<uint64_t>();
    std::vector<uint64_t> blocklist = { nodeId };
    screenManager_->RemoveVirtualScreenBlackList(screenId, blocklist);
}

void SetVirtualScreenSecurityExemptionList()
{
    ScreenId screenId = GetData<ScreenId>();
    uint64_t nodeId = GetData<uint64_t>();
    std::vector<uint64_t> securityExemptionList = { nodeId };
    screenManager_->SetVirtualScreenSecurityExemptionList(screenId, securityExemptionList);
}

void SetMirrorScreenVisibleRect()
{
    ScreenId screenId = GetData<ScreenId>();
    Rect mainScreenRect;
    bool supportRotation = GetData<bool>();
    screenManager_->SetMirrorScreenVisibleRect(screenId, mainScreenRect, supportRotation);
}

void SetCastScreenEnableSkipWindow()
{
    ScreenId screenId = GetData<ScreenId>();
    bool enable = GetData<bool>();
    screenManager_->SetCastScreenEnableSkipWindow(screenId, enable);
}

void RemoveVirtualScreen()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    screenManager_->RemoveVirtualScreen(screenId);
}

void SetScreenActiveMode()
{
    ScreenId screenId = GetData<ScreenId>();
    uint32_t modeId = GetData<uint32_t>();
    screenManager_->SetScreenActiveMode(screenId, modeId);
}

void SetScreenActiveRect()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    Rect activeRect;
    screenManager_->SetScreenActiveRect(screenId, activeRect);
}

void SetPhysicalScreenResolution()
{
    ScreenId screenId = GetData<ScreenId>();
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    screenManager_->SetPhysicalScreenResolution(screenId, width, height);
}

void SetVirtualScreenResolution()
{
    ScreenId screenId = GetData<ScreenId>();
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    screenManager_->SetVirtualScreenResolution(screenId, width, height);
}

void SetRogScreenResolution()
{
    ScreenId screenId = GetData<ScreenId>();
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    screenManager_->SetRogScreenResolution(screenId, width, height);
}

void SetScreenPowerStatus()
{
    ScreenId screenId = GetData<ScreenId>();
    ScreenPowerStatus status = static_cast<ScreenPowerStatus>(GetData<uint32_t>() % SCREEN_POWER_STATUS_SIZE);
    screenManager_->SetScreenPowerStatus(screenId, status);
}

void SetVirtualMirrorScreenCanvasRotation()
{
    ScreenId screenId = GetData<ScreenId>();
    bool canvasRotation = GetData<bool>();
    screenManager_->SetVirtualMirrorScreenCanvasRotation(screenId, canvasRotation);
}

void SetVirtualMirrorScreenScaleMode()
{
    ScreenId screenId = GetData<ScreenId>();
    ScreenScaleMode scaleMode = static_cast<ScreenScaleMode>(GetData<uint8_t>() % SCREEN_SCALE_MODE_SIZE);
    screenManager_->SetVirtualMirrorScreenScaleMode(screenId, scaleMode);
}

void GetScreenData()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    screenManager_->GetScreenData(screenId);
}

void ResizeVirtualScreen()
{
    ScreenId screenId = GetData<ScreenId>();
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    screenManager_->ResizeVirtualScreen(screenId, width, height);
}

void GetScreenBacklight()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    screenManager_->GetScreenBacklight(screenId);
}

void SetScreenBacklight()
{
    ScreenId screenId = GetData<ScreenId>();
    uint32_t level = GetData<uint32_t>();
    screenManager_->SetScreenBacklight(screenId, level);
}

void GetCanvasRotation()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    screenManager_->GetCanvasRotation(screenId);
}

void DisplayDump()
{
    std::string dumpString(STRING_LEN, GetData<char>());
    screenManager_->DisplayDump(dumpString);
}

void SetScreenConstraint()
{
    ScreenId screenId = GetData<ScreenId>();
    uint64_t timestamp = GetData<uint64_t>();
    ScreenConstraintType type = static_cast<ScreenConstraintType>(GetData<uint8_t>() % SCREEN_CONSTRAINT_TYPE_SIZE);
    screenManager_->SetScreenConstraint(screenId, timestamp, type);
}

void GetScreenSupportedColorGamuts()
{
    ScreenId screenId = GetData<ScreenId>();
    ScreenColorGamut type = static_cast<ScreenColorGamut>(GetData<uint8_t>() % SCREEN_COLOR_GAMUT_SIZE);
    std::vector<ScreenColorGamut> mode = { type };
    screenManager_->GetScreenSupportedColorGamuts(screenId, mode);
}

void GetScreenColorGamut()
{
    ScreenId screenId = GetData<ScreenId>();
    ScreenColorGamut type = static_cast<ScreenColorGamut>(GetData<uint8_t>() % SCREEN_COLOR_GAMUT_SIZE);
    screenManager_->GetScreenColorGamut(screenId, type);
}

void SetScreenColorGamut()
{
    ScreenId screenId = GetData<ScreenId>();
    int32_t modeIdx = GetData<int32_t>();
    screenManager_->SetScreenColorGamut(screenId, modeIdx);
}

void SetScreenGamutMap()
{
    ScreenId screenId = GetData<ScreenId>();
    ScreenGamutMap mode = static_cast<ScreenGamutMap>(GetData<uint8_t>() % SCREEN_GAMUT_MAP_SIZE);
    screenManager_->SetScreenGamutMap(screenId, mode);
}

void SetScreenCorrection()
{
    ScreenId screenId = GetData<ScreenId>();
    ScreenRotation screenRotation = static_cast<ScreenRotation>(GetData<uint8_t>() % SCREEN_ROTATION_SIZE);
    screenManager_->SetScreenCorrection(screenId, screenRotation);
}

void GetScreenGamutMap()
{
    ScreenId screenId = GetData<ScreenId>();
    ScreenGamutMap mode = static_cast<ScreenGamutMap>(GetData<uint8_t>() % SCREEN_GAMUT_MAP_SIZE);
    screenManager_->GetScreenGamutMap(screenId, mode);
}

void GetScreenHDRCapability()
{
    ScreenId screenId = GetData<ScreenId>();
    RSScreenHDRCapability screenHdrCapability;
    screenManager_->GetScreenHDRCapability(screenId, screenHdrCapability);
}

void GetScreenType()
{
    ScreenId screenId = GetData<ScreenId>();
    RSScreenType type = static_cast<RSScreenType>(GetData<uint8_t>() % SCREEN_SCREEN_TYPE_SIZE);
    screenManager_->GetScreenType(screenId, type);
}

void GetScreenConnectionType()
{
    ScreenId screenId = GetData<ScreenId>();
    screenManager_->GetScreenConnectionType(screenId);
}

void SetScreenSkipFrameInterval()
{
    ScreenId screenId = GetData<ScreenId>();
    uint32_t skipFrameInterval = GetData<uint32_t>();
    screenManager_->SetScreenSkipFrameInterval(screenId, skipFrameInterval);
}

void GetDisplayIdentificationData()
{
    ScreenId screenId = GetData<ScreenId>();
    uint8_t outPort = GetData<uint8_t>();
    uint8_t data = GetData<uint8_t>();
    std::vector<uint8_t> edidData = { data };
    screenManager_->GetDisplayIdentificationData(screenId, outPort, edidData);
}

void SetPixelFormat()
{
    ScreenId screenId = GetData<ScreenId>();
    GraphicPixelFormat pixelFormat = static_cast<GraphicPixelFormat>(GetData<uint8_t>() % GRAPHIC_PIXEL_FORMAT_SIZE);
    screenManager_->SetPixelFormat(screenId, pixelFormat);
    screenManager_->GetPixelFormat(screenId, pixelFormat);
}

void SetScreenHDRFormat()
{
    ScreenId screenId = GetData<ScreenId>();
    int32_t modeIdx = GetData<int32_t>();
    screenManager_->SetScreenHDRFormat(screenId, modeIdx);
}

void GetScreenHDRFormat()
{
    ScreenId screenId = GetData<ScreenId>();
    ScreenHDRFormat hdrFormat = static_cast<ScreenHDRFormat>(GetData<uint8_t>() % SCREEN_HDR_FORMAT_SIZE);
    screenManager_->GetScreenHDRFormat(screenId, hdrFormat);
}

void GetScreenSupportedHDRFormats()
{
    ScreenId screenId = GetData<ScreenId>();
    ScreenHDRFormat hdrFormat = static_cast<ScreenHDRFormat>(GetData<uint8_t>() % SCREEN_HDR_FORMAT_SIZE);
    std::vector<ScreenHDRFormat> hdrFormats = { hdrFormat };
    screenManager_->GetScreenSupportedHDRFormats(screenId, hdrFormats);
}

void GetScreenColorSpace()
{
    ScreenId screenId = GetData<ScreenId>();
    GraphicCM_ColorSpaceType colorSpace =
        static_cast<GraphicCM_ColorSpaceType>(GetData<uint8_t>() % GRAPHIC_CM_COLOR_SPACE_TYPE_SIZE);
    screenManager_->GetScreenColorSpace(screenId, colorSpace);
}

void SetScreenColorSpace()
{
    ScreenId screenId = GetData<ScreenId>();
    GraphicCM_ColorSpaceType colorSpace =
        static_cast<GraphicCM_ColorSpaceType>(GetData<uint8_t>() % GRAPHIC_CM_COLOR_SPACE_TYPE_SIZE);
    screenManager_->SetScreenColorSpace(screenId, colorSpace);
}

void GetScreenSupportedColorSpaces()
{
    ScreenId screenId = GetData<ScreenId>();
    GraphicCM_ColorSpaceType colorSpace =
        static_cast<GraphicCM_ColorSpaceType>(GetData<uint8_t>() % GRAPHIC_CM_COLOR_SPACE_TYPE_SIZE);
    std::vector<GraphicCM_ColorSpaceType> colorSpaces = { colorSpace };
    screenManager_->GetScreenSupportedColorSpaces(screenId, colorSpaces);
}

void DisablePowerOffRenderControl()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    screenManager_->DisablePowerOffRenderControl(screenId);
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (!OHOS::Rosen::Init(data, size)) {
        return 0;
    }
    OHOS::Rosen::InitScreenManger();
    using FunctionPtr = void (*)();
    std::vector<FunctionPtr> funcVector = {
        OHOS::Rosen::InitScreenManger,
        OHOS::Rosen::GetActiveScreenId,
        OHOS::Rosen::SetVirtualScreenRefreshRate,
        OHOS::Rosen::GetVirtualScreenResolution,
        OHOS::Rosen::GetScreenActiveMode,
        OHOS::Rosen::GetScreenSupportedModes,
        OHOS::Rosen::GetScreenCapability,
        OHOS::Rosen::GetScreenPowerStatus,
        OHOS::Rosen::GetDefaultScreenId,
        OHOS::Rosen::GetAllScreenIds,
        OHOS::Rosen::CreateVirtualScreen,
        OHOS::Rosen::SetVirtualScreenBlackList,
        OHOS::Rosen::SetVirtualScreenTypeBlackList,
        OHOS::Rosen::AddVirtualScreenBlackList,
        OHOS::Rosen::RemoveVirtualScreenBlackList,
        OHOS::Rosen::SetVirtualScreenSecurityExemptionList,
        OHOS::Rosen::SetMirrorScreenVisibleRect,
        OHOS::Rosen::SetCastScreenEnableSkipWindow,
        OHOS::Rosen::RemoveVirtualScreen,
        OHOS::Rosen::SetScreenActiveMode,
        OHOS::Rosen::SetScreenActiveRect,
        OHOS::Rosen::SetPhysicalScreenResolution,
        OHOS::Rosen::SetVirtualScreenResolution,
        OHOS::Rosen::SetRogScreenResolution,
        OHOS::Rosen::SetScreenPowerStatus,
        OHOS::Rosen::SetVirtualMirrorScreenCanvasRotation,
        OHOS::Rosen::SetVirtualMirrorScreenScaleMode,
        OHOS::Rosen::GetScreenData,
        OHOS::Rosen::ResizeVirtualScreen,
        OHOS::Rosen::GetScreenBacklight,
        OHOS::Rosen::SetScreenBacklight,
        OHOS::Rosen::GetCanvasRotation,
        OHOS::Rosen::DisplayDump,
        OHOS::Rosen::SetScreenConstraint,
        OHOS::Rosen::GetScreenSupportedColorGamuts,
        OHOS::Rosen::GetScreenColorGamut,
        OHOS::Rosen::SetScreenColorGamut,
        OHOS::Rosen::SetScreenGamutMap,
        OHOS::Rosen::SetScreenCorrection,
        OHOS::Rosen::GetScreenGamutMap,
        OHOS::Rosen::GetScreenHDRCapability,
        OHOS::Rosen::GetScreenType,
        OHOS::Rosen::GetScreenConnectionType,
        OHOS::Rosen::SetScreenSkipFrameInterval,
        OHOS::Rosen::GetDisplayIdentificationData,
        OHOS::Rosen::SetPixelFormat,
        OHOS::Rosen::SetScreenHDRFormat,
        OHOS::Rosen::GetScreenHDRFormat,
        OHOS::Rosen::GetScreenSupportedHDRFormats,
        OHOS::Rosen::GetScreenColorSpace,
        OHOS::Rosen::SetScreenColorSpace,
        OHOS::Rosen::GetScreenSupportedColorSpaces,
        OHOS::Rosen::DisablePowerOffRenderControl,
        OHOS::Rosen::SetVirtualScreenAutoRotation
    };
    /* Run your code on data */
    uint8_t pos = OHOS::Rosen::GetData<uint8_t>() % funcVector.size();
    funcVector[pos]();
    return 0;
}