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

#include "securec.h"

#include "screen_manager/rs_screen_manager.h"

using FunctionPtr = void (*)();

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
size_t g_pos;
} // namespace

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
    CreateOrGetScreenManager()->Init();
}

void GetActiveScreenId()
{
    CreateOrGetScreenManager()->GetActiveScreenId();
}

void SetVirtualScreenRefreshRate()
{
    // get data
    ScreenId screenId = GetData<Rosen::ScreenId>();
    uint32_t maxRefreshRate = GetData<uint32_t>();
    uint32_t actualRefreshRate = GetData<uint32_t>();
    CreateOrGetScreenManager()->SetVirtualScreenRefreshRate(screenId, maxRefreshRate, actualRefreshRate);
}

void IsAllScreensPowerOff()
{
    CreateOrGetScreenManager()->IsAllScreensPowerOff();
}

void PostForceRefreshTask()
{
    CreateOrGetScreenManager()->PostForceRefreshTask();
}

void RemoveForceRefreshTask()
{
    CreateOrGetScreenManager()->RemoveForceRefreshTask();
}

void TrySimpleProcessHotPlugEvents()
{
    CreateOrGetScreenManager()->TrySimpleProcessHotPlugEvents();
}

void ProcessScreenHotPlugEvents()
{
    CreateOrGetScreenManager()->ProcessScreenHotPlugEvents();
}

void SetDefaultScreenId()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    CreateOrGetScreenManager()->SetDefaultScreenId(screenId);
}

void GetVirtualScreenResolution()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    RSVirtualScreenResolution virtualScreenResolution;
    CreateOrGetScreenManager()->GetVirtualScreenResolution(screenId, virtualScreenResolution);
}

void GetScreenActiveMode()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    RSScreenModeInfo screenModeInfo;
    CreateOrGetScreenManager()->GetScreenActiveMode(screenId, screenModeInfo);
}

void GetScreenSupportedModes()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    CreateOrGetScreenManager()->GetScreenSupportedModes(screenId);
}

void GetScreenCapability()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    CreateOrGetScreenManager()->GetScreenCapability(screenId);
}

void GetScreenPowerStatus()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    CreateOrGetScreenManager()->GetScreenPowerStatus(screenId);
}

void GetScreenCorrection()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    CreateOrGetScreenManager()->GetScreenCorrection(screenId);
}

void GetDefaultScreenId()
{
    CreateOrGetScreenManager()->GetDefaultScreenId();
}

void GetAllScreenIds()
{
    CreateOrGetScreenManager()->GetAllScreenIds();
}

void CreateVirtualScreen()
{
    std::string name(STRING_LEN, GetData<char>());
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    sptr<Surface> surface;
    ScreenId mirrorId = GetData<Rosen::ScreenId>();
    int32_t flags = GetData<int32_t>();
    NodeId nodeId = GetData<NodeId>();
    std::vector<NodeId> whiteList = { nodeId };
    CreateOrGetScreenManager()->CreateVirtualScreen(name, width, height, surface, mirrorId, flags, whiteList);
}

void SetVirtualScreenBlackList()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    uint64_t id = GetData<uint64_t>();
    std::vector<uint64_t> blocklist = { id };
    CreateOrGetScreenManager()->SetVirtualScreenBlackList(screenId, blocklist);
}

void SetVirtualScreenTypeBlackList()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    uint8_t id = GetData<uint8_t>();
    std::vector<uint8_t> typeBlackList = { id };
    CreateOrGetScreenManager()->SetVirtualScreenTypeBlackList(screenId, typeBlackList);
}

void AddVirtualScreenBlackList()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    uint64_t nodeId = GetData<uint64_t>();
    std::vector<uint64_t> blocklist = { nodeId };
    CreateOrGetScreenManager()->AddVirtualScreenBlackList(screenId, blocklist);
}

void RemoveVirtualScreenBlackList()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    uint64_t nodeId = GetData<uint64_t>();
    std::vector<uint64_t> blocklist = { nodeId };
    CreateOrGetScreenManager()->RemoveVirtualScreenBlackList(screenId, blocklist);
}

void SetVirtualScreenSecurityExemptionList()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    uint64_t nodeId = GetData<uint64_t>();
    std::vector<uint64_t> securityExemptionList = { nodeId };
    CreateOrGetScreenManager()->SetVirtualScreenSecurityExemptionList(screenId, securityExemptionList);
}

void GetVirtualScreenSecurityExemptionList()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    CreateOrGetScreenManager()->GetVirtualScreenSecurityExemptionList(screenId);
}

void GetScreenSecurityMask()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    CreateOrGetScreenManager()->GetScreenSecurityMask(screenId);
}

void SetMirrorScreenVisibleRect()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    Rect mainScreenRect;
    bool supportRotation = GetData<bool>();
    CreateOrGetScreenManager()->SetMirrorScreenVisibleRect(screenId, mainScreenRect, supportRotation);
}

void GetMirrorScreenVisibleRect()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    CreateOrGetScreenManager()->GetMirrorScreenVisibleRect(screenId);
}

void SetCastScreenEnableSkipWindow()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    bool enable = GetData<bool>();
    CreateOrGetScreenManager()->SetCastScreenEnableSkipWindow(screenId, enable);
}

void GetVirtualScreenBlackList()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    CreateOrGetScreenManager()->GetVirtualScreenBlackList(screenId);
}

void GetVirtualScreenTypeBlackList()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    CreateOrGetScreenManager()->GetVirtualScreenTypeBlackList(screenId);
}

void GetAllBlackList()
{
    CreateOrGetScreenManager()->GetAllBlackList();
}

void GetAllWhiteList()
{
    CreateOrGetScreenManager()->GetAllWhiteList();
}

void GetAndResetVirtualSurfaceUpdateFlag()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    CreateOrGetScreenManager()->GetAndResetVirtualSurfaceUpdateFlag(screenId);
}

void RemoveVirtualScreen()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    CreateOrGetScreenManager()->RemoveVirtualScreen(screenId);
}

void SetScreenActiveMode()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    uint32_t modeId = GetData<uint32_t>();
    CreateOrGetScreenManager()->SetScreenActiveMode(screenId, modeId);
}

void SetScreenActiveRect()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    GraphicIRect activeRect;
    CreateOrGetScreenManager()->SetScreenActiveRect(screenId, activeRect);
}
void SetPhysicalScreenResolution()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    CreateOrGetScreenManager()->SetPhysicalScreenResolution(screenId, width, height);
}

void SetVirtualScreenResolution()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    CreateOrGetScreenManager()->SetVirtualScreenResolution(screenId, width, height);
}

void SetRogScreenResolution()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    CreateOrGetScreenManager()->SetRogScreenResolution(screenId, width, height);
}

void SetScreenPowerStatus()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    ScreenPowerStatus status = static_cast<ScreenPowerStatus>(GetData<uint32_t>() % SCREEN_POWER_STATUS_SIZE);
    CreateOrGetScreenManager()->SetScreenPowerStatus(screenId, status);
}

void IsScreenPoweringOff()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    CreateOrGetScreenManager()->IsScreenPoweringOff(screenId);
}

void SetVirtualMirrorScreenCanvasRotation()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    bool canvasRotation = GetData<bool>();
    CreateOrGetScreenManager()->SetVirtualMirrorScreenCanvasRotation(screenId, canvasRotation);
}

void SetVirtualMirrorScreenScaleMode()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    ScreenScaleMode scaleMode = static_cast<ScreenScaleMode>(GetData<uint8_t>() % SCREEN_SCALE_MODE_SIZE);
    CreateOrGetScreenManager()->SetVirtualMirrorScreenScaleMode(screenId, scaleMode);
}

void ReleaseScreenDmaBuffer()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    CreateOrGetScreenManager()->ReleaseScreenDmaBuffer(screenId);
}

void GetScreenData()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    CreateOrGetScreenManager()->GetScreenData(screenId);
}

void ResizeVirtualScreen()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    CreateOrGetScreenManager()->ResizeVirtualScreen(screenId, width, height);
}

void GetScreenBacklight()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    CreateOrGetScreenManager()->GetScreenBacklight(screenId);
}

void SetScreenBacklight()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    uint32_t level = GetData<uint32_t>();
    CreateOrGetScreenManager()->SetScreenBacklight(screenId, level);
}

void QueryScreenInfo()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    CreateOrGetScreenManager()->QueryScreenInfo(screenId);
}

void GetCanvasRotation()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    CreateOrGetScreenManager()->GetCanvasRotation(screenId);
}

void GetScaleMode()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    CreateOrGetScreenManager()->GetScaleMode(screenId);
}

void GetProducerSurface()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    CreateOrGetScreenManager()->GetProducerSurface(screenId);
}

void GetOutput()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    CreateOrGetScreenManager()->GetOutput(screenId);
}

void DisplayDump()
{
    std::string dumpString(STRING_LEN, GetData<char>());
    CreateOrGetScreenManager()->DisplayDump(dumpString);
}

void SurfaceDump()
{
    std::string dumpString(STRING_LEN, GetData<char>());
    CreateOrGetScreenManager()->SurfaceDump(dumpString);
}

void FpsDump()
{
    std::string dumpString(STRING_LEN, GetData<char>());
    std::string arg(STRING_LEN, GetData<char>());
    CreateOrGetScreenManager()->FpsDump(dumpString, arg);
}

void ClearFpsDump()
{
    std::string dumpString(STRING_LEN, GetData<char>());
    std::string arg(STRING_LEN, GetData<char>());
    CreateOrGetScreenManager()->ClearFpsDump(dumpString, arg);
}

void HitchsDump()
{
    std::string dumpString(STRING_LEN, GetData<char>());
    std::string arg(STRING_LEN, GetData<char>());
    CreateOrGetScreenManager()->HitchsDump(dumpString, arg);
}

void SetScreenConstraint()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    uint64_t timestamp = GetData<uint64_t>();
    ScreenConstraintType type = static_cast<ScreenConstraintType>(GetData<uint8_t>() % SCREEN_CONSTRAINT_TYPE_SIZE);
    CreateOrGetScreenManager()->SetScreenConstraint(screenId, timestamp, type);
}

void GetScreenSupportedColorGamuts()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    ScreenColorGamut type = static_cast<ScreenColorGamut>(GetData<uint8_t>() % SCREEN_COLOR_GAMUT_SIZE);
    std::vector<ScreenColorGamut> mode = { type };
    CreateOrGetScreenManager()->GetScreenSupportedColorGamuts(screenId, mode);
}

void GetScreenColorGamut()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    ScreenColorGamut type = static_cast<ScreenColorGamut>(GetData<uint8_t>() % SCREEN_COLOR_GAMUT_SIZE);
    CreateOrGetScreenManager()->GetScreenColorGamut(screenId, type);
}

void SetScreenColorGamut()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    int32_t modeIdx = GetData<int32_t>();
    CreateOrGetScreenManager()->SetScreenColorGamut(screenId, modeIdx);
}

void SetScreenGamutMap()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    ScreenGamutMap mode = static_cast<ScreenGamutMap>(GetData<uint8_t>() % SCREEN_GAMUT_MAP_SIZE);
    CreateOrGetScreenManager()->SetScreenGamutMap(screenId, mode);
}

void SetScreenCorrection()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    ScreenRotation screenRotation = static_cast<ScreenRotation>(GetData<uint8_t>() % SCREEN_ROTATION_SIZE);
    CreateOrGetScreenManager()->SetScreenCorrection(screenId, screenRotation);
}

void GetScreenGamutMap()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    ScreenGamutMap mode = static_cast<ScreenGamutMap>(GetData<uint8_t>() % SCREEN_GAMUT_MAP_SIZE);
    CreateOrGetScreenManager()->GetScreenGamutMap(screenId, mode);
}

void GetScreenHDRCapability()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    RSScreenHDRCapability screenHdrCapability;
    CreateOrGetScreenManager()->GetScreenHDRCapability(screenId, screenHdrCapability);
}

void GetScreenType()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    RSScreenType type = static_cast<RSScreenType>(GetData<uint8_t>() % SCREEN_SCREEN_TYPE_SIZE);
    CreateOrGetScreenManager()->GetScreenType(screenId, type);
}

void SetScreenSkipFrameInterval()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    uint32_t skipFrameInterval = GetData<uint32_t>();
    CreateOrGetScreenManager()->SetScreenSkipFrameInterval(screenId, skipFrameInterval);
}

void SetEqualVsyncPeriod()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    bool isEqualVsyncPeriod = GetData<bool>();
    CreateOrGetScreenManager()->SetEqualVsyncPeriod(screenId, isEqualVsyncPeriod);
}

void GetDisplayIdentificationData()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    uint8_t outPort = GetData<uint8_t>();
    uint8_t data = GetData<uint8_t>();
    std::vector<uint8_t> edidData = { data };
    CreateOrGetScreenManager()->GetDisplayIdentificationData(screenId, outPort, edidData);
}

void SetPixelFormat()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    GraphicPixelFormat pixelFormat = static_cast<GraphicPixelFormat>(GetData<uint8_t>() % GRAPHIC_PIXEL_FORMAT_SIZE);
    CreateOrGetScreenManager()->SetPixelFormat(screenId, pixelFormat);
    CreateOrGetScreenManager()->GetPixelFormat(screenId, pixelFormat);
}

void SetScreenHDRFormat()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    int32_t modeIdx = GetData<int32_t>();
    CreateOrGetScreenManager()->SetScreenHDRFormat(screenId, modeIdx);
}

void GetScreenHDRFormat()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    ScreenHDRFormat hdrFormat = static_cast<ScreenHDRFormat>(GetData<uint8_t>() % SCREEN_HDR_FORMAT_SIZE);
    CreateOrGetScreenManager()->GetScreenHDRFormat(screenId, hdrFormat);
}

void GetScreenSupportedHDRFormats()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    ScreenHDRFormat hdrFormat = static_cast<ScreenHDRFormat>(GetData<uint8_t>() % SCREEN_HDR_FORMAT_SIZE);
    std::vector<ScreenHDRFormat> hdrFormats = { hdrFormat };
    CreateOrGetScreenManager()->GetScreenSupportedHDRFormats(screenId, hdrFormats);
}

void GetScreenColorSpace()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    GraphicCM_ColorSpaceType colorSpace =
        static_cast<GraphicCM_ColorSpaceType>(GetData<uint8_t>() % GRAPHIC_CM_COLOR_SPACE_TYPE_SIZE);
    CreateOrGetScreenManager()->GetScreenColorSpace(screenId, colorSpace);
}

void SetScreenColorSpace()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    GraphicCM_ColorSpaceType colorSpace =
        static_cast<GraphicCM_ColorSpaceType>(GetData<uint8_t>() % GRAPHIC_CM_COLOR_SPACE_TYPE_SIZE);
    CreateOrGetScreenManager()->SetScreenColorSpace(screenId, colorSpace);
}

void GetScreenSupportedColorSpaces()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    GraphicCM_ColorSpaceType colorSpace =
        static_cast<GraphicCM_ColorSpaceType>(GetData<uint8_t>() % GRAPHIC_CM_COLOR_SPACE_TYPE_SIZE);
    std::vector<GraphicCM_ColorSpaceType> colorSpaces = { colorSpace };
    CreateOrGetScreenManager()->GetScreenSupportedColorSpaces(screenId, colorSpaces);
}

void IsScreenPowerOff()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    CreateOrGetScreenManager()->IsScreenPowerOff(screenId);
}

void DisablePowerOffRenderControl()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    CreateOrGetScreenManager()->DisablePowerOffRenderControl(screenId);
}

void GetDisplayPropertyForHardCursor()
{
    uint32_t screenId = GetData<uint32_t>();
    CreateOrGetScreenManager()->GetDisplayPropertyForHardCursor(screenId);
}

void SetScreenHasProtectedLayer()
{
    uint32_t screenId = GetData<uint32_t>();
    bool hasProtectedLayer = GetData<bool>();
    CreateOrGetScreenManager()->SetScreenHasProtectedLayer(screenId, hasProtectedLayer);
}

void IsVisibleRectSupportRotation()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    CreateOrGetScreenManager()->IsVisibleRectSupportRotation(screenId);
}
} // namespace Rosen
} // namespace OHOS

void InitFuncVectorPartOne(std::vector<FunctionPtr> &funcVector ) {
    funcVector.push_back(OHOS::Rosen::SetCastScreenEnableSkipWindow);
    funcVector.push_back(OHOS::Rosen::GetVirtualScreenBlackList);
    funcVector.push_back(OHOS::Rosen::GetVirtualScreenTypeBlackList);
    funcVector.push_back(OHOS::Rosen::GetAllBlackList);
	funcVector.push_back(OHOS::Rosen::GetAllWhiteList);
	funcVector.push_back(OHOS::Rosen::GetAndResetVirtualSurfaceUpdateFlag);
	funcVector.push_back(OHOS::Rosen::RemoveVirtualScreen);
	funcVector.push_back(OHOS::Rosen::SetScreenActiveMode);
	funcVector.push_back(OHOS::Rosen::SetScreenActiveRect);
	funcVector.push_back(OHOS::Rosen::SetPhysicalScreenResolution);
	funcVector.push_back(OHOS::Rosen::SetVirtualScreenResolution);
	funcVector.push_back(OHOS::Rosen::SetRogScreenResolution);
	funcVector.push_back(OHOS::Rosen::SetScreenPowerStatus);
	funcVector.push_back(OHOS::Rosen::IsScreenPoweringOff);
	funcVector.push_back(OHOS::Rosen::SetVirtualMirrorScreenCanvasRotation);
	funcVector.push_back(OHOS::Rosen::SetVirtualMirrorScreenScaleMode);
	funcVector.push_back(OHOS::Rosen::ReleaseScreenDmaBuffer);
	funcVector.push_back(OHOS::Rosen::GetScreenData);
	funcVector.push_back(OHOS::Rosen::ResizeVirtualScreen);
	funcVector.push_back(OHOS::Rosen::GetScreenBacklight);
	funcVector.push_back(OHOS::Rosen::SetScreenBacklight);
	funcVector.push_back(OHOS::Rosen::QueryScreenInfo);
	funcVector.push_back(OHOS::Rosen::GetCanvasRotation);
	funcVector.push_back(OHOS::Rosen::GetScaleMode);
	funcVector.push_back(OHOS::Rosen::GetProducerSurface);
	funcVector.push_back(OHOS::Rosen::GetOutput);
	funcVector.push_back(OHOS::Rosen::DisplayDump);
	funcVector.push_back(OHOS::Rosen::SurfaceDump);
	funcVector.push_back(OHOS::Rosen::FpsDump);
}

void InitFuncVectorPartTwo(std::vector<FunctionPtr> &funcVector ) {
	funcVector.push_back(OHOS::Rosen::ClearFpsDump);
	funcVector.push_back(OHOS::Rosen::HitchsDump);
	funcVector.push_back(OHOS::Rosen::SetScreenConstraint);
	funcVector.push_back(OHOS::Rosen::GetScreenSupportedColorGamuts);
	funcVector.push_back(OHOS::Rosen::GetScreenColorGamut);
	funcVector.push_back(OHOS::Rosen::SetScreenColorGamut);
	funcVector.push_back(OHOS::Rosen::SetScreenGamutMap);
	funcVector.push_back(OHOS::Rosen::SetScreenCorrection);
	funcVector.push_back(OHOS::Rosen::GetScreenGamutMap);
	funcVector.push_back(OHOS::Rosen::GetScreenHDRCapability);
	funcVector.push_back(OHOS::Rosen::GetScreenType);
	funcVector.push_back(OHOS::Rosen::SetScreenSkipFrameInterval);
	funcVector.push_back(OHOS::Rosen::SetEqualVsyncPeriod);
	funcVector.push_back(OHOS::Rosen::GetDisplayIdentificationData);
	funcVector.push_back(OHOS::Rosen::SetPixelFormat);
	funcVector.push_back(OHOS::Rosen::SetScreenHDRFormat);
	funcVector.push_back(OHOS::Rosen::GetScreenHDRFormat);
	funcVector.push_back(OHOS::Rosen::GetScreenSupportedHDRFormats);
	funcVector.push_back(OHOS::Rosen::GetScreenColorSpace);
	funcVector.push_back(OHOS::Rosen::SetScreenColorSpace);
	funcVector.push_back(OHOS::Rosen::GetScreenSupportedColorSpaces);
	funcVector.push_back(OHOS::Rosen::IsScreenPowerOff);
	funcVector.push_back(OHOS::Rosen::DisablePowerOffRenderControl);
	funcVector.push_back(OHOS::Rosen::GetDisplayPropertyForHardCursor);
	funcVector.push_back(OHOS::Rosen::SetScreenHasProtectedLayer);
	funcVector.push_back(OHOS::Rosen::IsVisibleRectSupportRotation);
}


/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (!OHOS::Rosen::Init(data, size)) {
        return 0;
    }
    std::vector<FunctionPtr> funcVector = {
        OHOS::Rosen::InitScreenManger,
        OHOS::Rosen::GetActiveScreenId,
        OHOS::Rosen::SetVirtualScreenRefreshRate,
        OHOS::Rosen::IsAllScreensPowerOff,
        OHOS::Rosen::PostForceRefreshTask,
        OHOS::Rosen::RemoveForceRefreshTask,
        OHOS::Rosen::TrySimpleProcessHotPlugEvents,
        OHOS::Rosen::ProcessScreenHotPlugEvents,
        OHOS::Rosen::SetDefaultScreenId,
        OHOS::Rosen::GetVirtualScreenResolution,
        OHOS::Rosen::GetScreenActiveMode,
        OHOS::Rosen::GetScreenSupportedModes,
        OHOS::Rosen::GetScreenCapability,
        OHOS::Rosen::GetScreenPowerStatus,
        OHOS::Rosen::GetScreenCorrection,
        OHOS::Rosen::GetDefaultScreenId,
        OHOS::Rosen::GetAllScreenIds,
        OHOS::Rosen::CreateVirtualScreen,
        OHOS::Rosen::SetVirtualScreenBlackList,
        OHOS::Rosen::SetVirtualScreenTypeBlackList,
        OHOS::Rosen::AddVirtualScreenBlackList,
        OHOS::Rosen::RemoveVirtualScreenBlackList,
        OHOS::Rosen::SetVirtualScreenSecurityExemptionList,
        OHOS::Rosen::GetVirtualScreenSecurityExemptionList,
        OHOS::Rosen::GetScreenSecurityMask,
        OHOS::Rosen::SetMirrorScreenVisibleRect,
        OHOS::Rosen::GetMirrorScreenVisibleRect       
    };
    InitFuncVectorPartOne(funcVector);
    InitFuncVectorPartTwo(funcVector);
    /* Run your code on data */
    uint8_t pos = OHOS::Rosen::GetData<uint8_t>() % funcVector.size();
    funcVector[pos]();
    return 0;
}