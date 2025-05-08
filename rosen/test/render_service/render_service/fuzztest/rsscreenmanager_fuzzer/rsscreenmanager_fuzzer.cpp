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
using FunctionPtr = bool (*)();
namespace OHOS {
namespace Rosen {
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
constexpr uint8_t INIT_SCREENMANGER = 0;
constexpr uint8_t GET_ACTIVESCREENID = 1;
constexpr uint8_t SET_VIRTUAL_SCREENREFRESHRATE = 2;
constexpr uint8_t IS_ALLSCREENSPOWEROFF = 3;
constexpr uint8_t POST_FORCE_REFRESHTASK = 4;
constexpr uint8_t REMOVE_FORCE_REFRESHTASK = 5;
constexpr uint8_t TRY_SIMPLEPROCESS_HOTPLUGEVENTS = 6;
constexpr uint8_t PROCESS_SCREENHOTPLUGEVENTS = 7;
constexpr uint8_t PROCESS_PENDINGCONNECTIONS = 8;
constexpr uint8_t SET_DEFAULTSCREENID = 9;
constexpr uint8_t GET_VIRTUAL_SCREENRESOLUTION = 10;
constexpr uint8_t GET_SCREENACTIVEMODE = 11;
constexpr uint8_t GET_SCREENSUPPORTEDMODES = 12;
constexpr uint8_t GET_SCREENCAPABILITY = 13;
constexpr uint8_t GET_SCREENPOWERSTATUS = 14;
constexpr uint8_t GET_SCREENCORRECTION = 15;
constexpr uint8_t GET_DEFAULTSCREENID = 16;
constexpr uint8_t GET_ALLSCREENIDS = 17;
constexpr uint8_t CREATE_VIRTUAL_SCREEN = 18;
constexpr uint8_t SET_VIRTUAL_SCREENBLACKLIST = 19;
constexpr uint8_t SET_VIRTUAL_SCREENTYPEBLACKLIST = 20;
constexpr uint8_t ADD_VIRTUAL_SCREENBLACKLIST = 21;
constexpr uint8_t REMOVE_VIRTUAL_SCREENBLACKLIST = 22;
constexpr uint8_t SET_VIRTUAL_SCREENSECURITYEXEMPTIONLIST = 23;
constexpr uint8_t GET_VIRTUAL_SCREENSECURITYEXEMPTIONLIST = 24;
constexpr uint8_t GET_SCREENSECURITYMASK = 25;
constexpr uint8_t SET_MIRRORSCREENVISIBLERECT = 26;
constexpr uint8_t GET_MIRRORSCREENVISIBLERECT = 27;
constexpr uint8_t SET_CASTSCREENENABLESKIPWINDOW = 28;
constexpr uint8_t GET_VIRTUAL_SCREENBLACKLIST = 29;
constexpr uint8_t GET_VIRTUAL_SCREENTYPEBLACKLIST = 30;
constexpr uint8_t GET_ALLBLACKLIST = 31;
constexpr uint8_t GET_ALLWHITELIST = 32;
constexpr uint8_t GET_ANDRESET_VIRTUAL_SURFACEUPDATEFLAG = 33;
constexpr uint8_t REMOVE_VIRTUAL_SCREEN = 34;
constexpr uint8_t SET_SCREENACTIVEMODE = 35;
constexpr uint8_t SET_SCREENACTIVERECT = 36;
constexpr uint8_t SET_PHYSICALSCREENRESOLUTION = 37;
constexpr uint8_t SET_VIRTUAL_SCREENRESOLUTION = 38;
constexpr uint8_t SET_ROGSCREENRESOLUTION = 39;
constexpr uint8_t SET_SCREENPOWERSTATUS = 40;
constexpr uint8_t IS_SCREENPOWERINGOFF = 41;
constexpr uint8_t SET_VIRTUAL_MIRRORSCREENCANVASROTATION = 42;
constexpr uint8_t SET_VIRTUAL_MIRRORSCREENSCALEMODE = 43;
constexpr uint8_t RELEASESCREENDMABUFFER = 44;
constexpr uint8_t GET_SCREENDATA = 45;
constexpr uint8_t RESIZEVIRTUAL_SCREEN = 46;
constexpr uint8_t GET_SCREENBACKLIGHT = 47;
constexpr uint8_t SET_SCREENBACKLIGHT = 48;
constexpr uint8_t QUERYSCREENINFO = 49;
constexpr uint8_t GET_CANVASROTATION = 50;
constexpr uint8_t GET_SCALEMODE = 51;
constexpr uint8_t GET_PRODUCERSURFACE = 52;
constexpr uint8_t GET_OUTPUT = 53;
constexpr uint8_t DIS_PLAY_DUMP = 54;
constexpr uint8_t SURFACE_DUMP = 55;
constexpr uint8_t FPS_DUMP = 56;
constexpr uint8_t CLEARFPS_DUMP = 57;
constexpr uint8_t HITCHS_DUMP = 58;
constexpr uint8_t SET_SCREENCONSTEXPRRAINT = 59;
constexpr uint8_t GET_SCREENSUPPORTEDCOLORGAMUTS = 60;
constexpr uint8_t GET_SCREENCOLORGAMUT = 61;
constexpr uint8_t SET_SCREENCOLORGAMUT = 62;
constexpr uint8_t SET_SCREENGAMUTMAP = 63;
constexpr uint8_t SET_SCREENCORRECTION = 64;
constexpr uint8_t GET_SCREENGAMUTMAP = 65;
constexpr uint8_t GET_SCREENHDRCAPABILITY = 66;
constexpr uint8_t GET_SCREENTYPE = 67;
constexpr uint8_t SET_SCREENSKIPFRAMEINTERVAL = 68;
constexpr uint8_t SET_EQUALVSYNCPERIOD = 69;
constexpr uint8_t GET_DIS_PLAYIDENTIFICATIONDATA = 70;
constexpr uint8_t SET_PIXELFORMAT = 71;
constexpr uint8_t SET_SCREENHDRFORMAT = 72;
constexpr uint8_t GET_SCREENHDRFORMAT = 73;
constexpr uint8_t GET_SCREENSUPPORTEDHDRFORMATS = 74;
constexpr uint8_t GET_SCREENCOLORSPACE = 75;
constexpr uint8_t SET_SCREENCOLORSPACE = 76;
constexpr uint8_t GET_SCREENSUPPORTEDCOLORSPACES = 77;
constexpr uint8_t IS_SCREENPOWEROFF = 78;
constexpr uint8_t DIS_ABLEPOWEROFFRENDERCONTROL = 79;
constexpr uint8_t GET_DIS_PLAYPROPERTYFORHARDCURSOR = 80;
constexpr uint8_t SET_SCREENHASPROTECTEDLAYER = 81;
constexpr uint8_t IS_VIS_IBLERECTSUPPORTROTATION = 82;
constexpr uint8_t TARGET_SIZE = 83;
namespace {
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

bool InitScreenManger()
{
    CreateOrGetScreenManager()->Init();
    return true;
}

bool GetActiveScreenId()
{
    CreateOrGetScreenManager()->GetActiveScreenId();
    return true;
}

bool SetVirtualScreenRefreshRate()
{
    // get data
    Rosen::ScreenId screenId = GetData<Rosen::ScreenId>();
    uint32_t maxRefreshRate = GetData<uint32_t>();
    uint32_t actualRefreshRate = GetData<uint32_t>();
    CreateOrGetScreenManager()->SetVirtualScreenRefreshRate(screenId, maxRefreshRate, actualRefreshRate);
    return true;
}

bool IsAllScreensPowerOff()
{
    CreateOrGetScreenManager()->IsAllScreensPowerOff();
    return true;
}

bool PostForceRefreshTask()
{
    CreateOrGetScreenManager()->PostForceRefreshTask();
    return true;
}

bool RemoveForceRefreshTask()
{
    CreateOrGetScreenManager()->RemoveForceRefreshTask();
    return true;
}

bool TrySimpleProcessHotPlugEvents()
{
    CreateOrGetScreenManager()->TrySimpleProcessHotPlugEvents();
    return true;
}

bool ProcessScreenHotPlugEvents()
{
    CreateOrGetScreenManager()->ProcessScreenHotPlugEvents();
    return true;
}

bool ProcessPendingConnections()
{
    CreateOrGetScreenManager()->ProcessPendingConnections();
    return true;
}

bool SetDefaultScreenId()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    CreateOrGetScreenManager()->SetDefaultScreenId(screenId);
    return true;
}

bool GetVirtualScreenResolution()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    RSVirtualScreenResolution virtualScreenResolution;
    CreateOrGetScreenManager()->GetVirtualScreenResolution(screenId, virtualScreenResolution);
    return true;
}

bool GetScreenActiveMode()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    RSScreenModeInfo screenModeInfo;
    CreateOrGetScreenManager()->GetScreenActiveMode(screenId, screenModeInfo);
    return true;
}

bool GetScreenSupportedModes()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    CreateOrGetScreenManager()->GetScreenSupportedModes(screenId);
    return true;
}

bool GetScreenCapability()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    CreateOrGetScreenManager()->GetScreenCapability(screenId);
    return true;
}

bool GetScreenPowerStatus()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    CreateOrGetScreenManager()->GetScreenPowerStatus(screenId);
    return true;
}

bool GetScreenCorrection()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    CreateOrGetScreenManager()->GetScreenCorrection(screenId);
    return true;
}

bool GetDefaultScreenId()
{
    CreateOrGetScreenManager()->GetDefaultScreenId();
    return true;
}

bool GetAllScreenIds()
{
    CreateOrGetScreenManager()->GetAllScreenIds();
    return true;
}

bool CreateVirtualScreen()
{
    std::string name(STRING_LEN, GetData<char>());
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    sptr<Surface> surface;
    ScreenId mirrorId = GetData<Rosen::ScreenId>();
    int32_t flags = GetData<bool>();
    NodeId nodeId = GetData<NodeId>();
    std::vector<NodeId> whiteList = { nodeId };
    CreateOrGetScreenManager()->CreateVirtualScreen(name, width, height, surface, mirrorId, flags, whiteList);
    return true;
}

bool SetVirtualScreenBlackList()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    uint64_t id = GetData<uint64_t>();
    std::vector<uint64_t> blocklist = { id };
    CreateOrGetScreenManager()->SetVirtualScreenBlackList(screenId, blocklist);
    return true;
}

bool SetVirtualScreenTypeBlackList()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    uint8_t id = GetData<uint8_t>();
    std::vector<uint8_t> typeBlackList = { id };
    CreateOrGetScreenManager()->SetVirtualScreenTypeBlackList(screenId, typeBlackList);
    return true;
}

bool AddVirtualScreenBlackList()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    uint64_t nodeId = GetData<uint64_t>();
    std::vector<uint64_t> blocklist = { nodeId };
    CreateOrGetScreenManager()->AddVirtualScreenBlackList(screenId, blocklist);
    return true;
}

bool RemoveVirtualScreenBlackList()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    uint64_t nodeId = GetData<uint64_t>();
    std::vector<uint64_t> blocklist = { nodeId };
    CreateOrGetScreenManager()->RemoveVirtualScreenBlackList(screenId, blocklist);
    return true;
}

bool SetVirtualScreenSecurityExemptionList()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    uint64_t nodeId = GetData<uint64_t>();
    std::vector<uint64_t> securityExemptionList = { nodeId };
    CreateOrGetScreenManager()->SetVirtualScreenSecurityExemptionList(screenId, securityExemptionList);
    return true;
}

bool GetVirtualScreenSecurityExemptionList()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    CreateOrGetScreenManager()->GetVirtualScreenSecurityExemptionList(screenId);
    return true;
}

bool GetScreenSecurityMask()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    CreateOrGetScreenManager()->GetScreenSecurityMask(screenId);
    return true;
}

bool SetMirrorScreenVisibleRect()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    Rect mainScreenRect;
    bool supportRotation = GetData<bool>();
    CreateOrGetScreenManager()->SetMirrorScreenVisibleRect(screenId, mainScreenRect, supportRotation);
    return true;
}

bool GetMirrorScreenVisibleRect()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    CreateOrGetScreenManager()->GetMirrorScreenVisibleRect(screenId);
    return true;
}

bool SetCastScreenEnableSkipWindow()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    bool enable = GetData<bool>();
    CreateOrGetScreenManager()->SetCastScreenEnableSkipWindow(screenId, enable);
    return true;
}

bool GetVirtualScreenBlackList()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    CreateOrGetScreenManager()->GetVirtualScreenBlackList(screenId);
    return true;
}

bool GetVirtualScreenTypeBlackList()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    CreateOrGetScreenManager()->GetVirtualScreenTypeBlackList(screenId);
    return true;
}

bool GetAllBlackList()
{
    CreateOrGetScreenManager()->GetAllBlackList();
    return true;
}

bool GetAllWhiteList()
{
    CreateOrGetScreenManager()->GetAllWhiteList();
    return true;
}

bool GetAndResetVirtualSurfaceUpdateFlag()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    CreateOrGetScreenManager()->GetAndResetVirtualSurfaceUpdateFlag(screenId);
    return true;
}

bool RemoveVirtualScreen()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    CreateOrGetScreenManager()->RemoveVirtualScreen(screenId);
    return true;
}
bool SetScreenActiveMode()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    uint32_t modeId = GetData<uint32_t>();
    CreateOrGetScreenManager()->SetScreenActiveMode(screenId, modeId);
    return true;
}

bool SetScreenActiveRect()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    GraphicIRect activeRect;
    CreateOrGetScreenManager()->SetScreenActiveRect(screenId, activeRect);
    return true;
}
bool SetPhysicalScreenResolution()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    CreateOrGetScreenManager()->SetPhysicalScreenResolution(screenId, width, height);
    return true;
}

bool SetVirtualScreenResolution()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    CreateOrGetScreenManager()->SetVirtualScreenResolution(screenId, width, height);
    return true;
}

bool SetRogScreenResolution()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    CreateOrGetScreenManager()->SetRogScreenResolution(screenId, width, height);
    return true;
}

bool SetScreenPowerStatus()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    ScreenPowerStatus status = static_cast<ScreenPowerStatus>(GetData<uint32_t>() % SCREEN_POWER_STATUS_SIZE);
    CreateOrGetScreenManager()->SetScreenPowerStatus(screenId, status);
    return true;
}

bool IsScreenPoweringOff()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    CreateOrGetScreenManager()->IsScreenPoweringOff(screenId);
    return true;
}

bool SetVirtualMirrorScreenCanvasRotation()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    bool canvasRotation = GetData<bool>();
    CreateOrGetScreenManager()->SetVirtualMirrorScreenCanvasRotation(screenId, canvasRotation);
    return true;
}

bool SetVirtualMirrorScreenScaleMode()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    ScreenScaleMode scaleMode = static_cast<ScreenScaleMode>(GetData<uint8_t>() % SCREEN_SCALE_MODE_SIZE);
    CreateOrGetScreenManager()->SetVirtualMirrorScreenScaleMode(screenId, scaleMode);
    return true;
}

bool ReleaseScreenDmaBuffer()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    CreateOrGetScreenManager()->ReleaseScreenDmaBuffer(screenId);
    return true;
}

bool GetScreenData()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    CreateOrGetScreenManager()->GetScreenData(screenId);
    return true;
}

bool ResizeVirtualScreen()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    CreateOrGetScreenManager()->ResizeVirtualScreen(screenId, width, height);
    return true;
}

bool GetScreenBacklight()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    CreateOrGetScreenManager()->GetScreenBacklight(screenId);
    return true;
}

bool SetScreenBacklight()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    uint32_t level = GetData<uint32_t>();
    CreateOrGetScreenManager()->SetScreenBacklight(screenId, level);
    return true;
}

bool QueryScreenInfo()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    CreateOrGetScreenManager()->QueryScreenInfo(screenId);
    return true;
}

bool GetCanvasRotation()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    CreateOrGetScreenManager()->GetCanvasRotation(screenId);
    return true;
}

bool GetScaleMode()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    CreateOrGetScreenManager()->GetScaleMode(screenId);
    return true;
}

bool GetProducerSurface()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    CreateOrGetScreenManager()->GetProducerSurface(screenId);
    return true;
}

bool GetOutput()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    CreateOrGetScreenManager()->GetOutput(screenId);
    return true;
}

bool DisplayDump()
{
    std::string dumpString(STRING_LEN, GetData<char>());
    CreateOrGetScreenManager()->DisplayDump(dumpString);
    return true;
}

bool SurfaceDump()
{
    std::string dumpString(STRING_LEN, GetData<char>());
    CreateOrGetScreenManager()->SurfaceDump(dumpString);
    return true;
}

bool FpsDump()
{
    std::string dumpString(STRING_LEN, GetData<char>());
    std::string arg(STRING_LEN, GetData<char>());
    CreateOrGetScreenManager()->FpsDump(dumpString, arg);
    return true;
}

bool ClearFpsDump()
{
    std::string dumpString(STRING_LEN, GetData<char>());
    std::string arg(STRING_LEN, GetData<char>());
    CreateOrGetScreenManager()->ClearFpsDump(dumpString, arg);
    return true;
}

bool HitchsDump()
{
    std::string dumpString(STRING_LEN, GetData<char>());
    std::string arg(STRING_LEN, GetData<char>());
    CreateOrGetScreenManager()->HitchsDump(dumpString, arg);
    return true;
}

bool SetScreenConstraint()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    uint64_t timestamp = GetData<uint64_t>();
    ScreenConstraintType type = static_cast<ScreenConstraintType>(GetData<uint8_t>() % SCREEN_CONSTRAINT_TYPE_SIZE);
    CreateOrGetScreenManager()->SetScreenConstraint(screenId, timestamp, type);
    return true;
}

bool GetScreenSupportedColorGamuts()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    ScreenColorGamut type = static_cast<ScreenColorGamut>(GetData<uint8_t>() % SCREEN_COLOR_GAMUT_SIZE);
    std::vector<ScreenColorGamut> mode = { type };
    CreateOrGetScreenManager()->GetScreenSupportedColorGamuts(screenId, mode);
    return true;
}

bool GetScreenColorGamut()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    ScreenColorGamut type = static_cast<ScreenColorGamut>(GetData<uint8_t>() % SCREEN_COLOR_GAMUT_SIZE);
    std::vector<ScreenColorGamut> mode = { type };
    CreateOrGetScreenManager()->GetScreenColorGamut(screenId, mode);
    return true;
}

bool SetScreenColorGamut()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    int32_t modeIdx = GetData<int32_t>();
    CreateOrGetScreenManager()->SetScreenColorGamut(screenId, modeIdx);
    return true;
}

bool SetScreenGamutMap()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    ScreenGamutMap mode = static_cast<ScreenGamutMap>(GetData<uint8_t>() % SCREEN_GAMUT_MAP_SIZE);
    CreateOrGetScreenManager()->SetScreenGamutMap(screenId, mode);
    return true;
}

bool SetScreenCorrection()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    ScreenRotation screenRotation = static_cast<ScreenRotation>(GetData<uint8_t>() % SCREEN_ROTATION_SIZE);
    CreateOrGetScreenManager()->SetScreenCorrection(screenId, screenRotation);
    return true;
}

bool GetScreenGamutMap()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    ScreenGamutMap mode = static_cast<ScreenGamutMap>(GetData<uint8_t>() % SCREEN_GAMUT_MAP_SIZE);
    CreateOrGetScreenManager()->GetScreenGamutMap(screenId, mode);
    return true;
}

bool GetScreenHDRCapability()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    RSScreenHDRCapability screenHdrCapability;
    CreateOrGetScreenManager()->GetScreenHDRCapability(screenId, screenHdrCapability);
    return true;
}

bool GetScreenType()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    RSScreenType type = static_cast<RSScreenType>(GetData<uint8_t>() % SCREEN_SCREEN_TYPE_SIZE);
    CreateOrGetScreenManager()->GetScreenType(screenId, type);
    return true;
}

bool SetScreenSkipFrameInterval()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    uint32_t skipFrameInterval = GetData<uint32_t>();
    CreateOrGetScreenManager()->SetScreenSkipFrameInterval(screenId, skipFrameInterval);
    return true;
}

bool SetEqualVsyncPeriod()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    bool isEqualVsyncPeriod = GetData<bool>();
    CreateOrGetScreenManager()->SetEqualVsyncPeriod(screenId, isEqualVsyncPeriod);
    return true;
}

bool GetDisplayIdentificationData()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    uint8_t outPort = GetData<uint8_t>();
    uint8_t data = GetData<uint8_t>();
    std::vector<uint8_t> edidData = { data };
    CreateOrGetScreenManager()->GetDisplayIdentificationData(screenId, outPort, edidData);
    return true;
}

bool SetPixelFormat()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    GraphicPixelFormat pixelFormat = static_cast<GraphicPixelFormat>(GetData<uint8_t>() % GRAPHIC_PIXEL_FORMAT_SIZE);
    CreateOrGetScreenManager()->SetPixelFormat(screenId, pixelFormat);
    CreateOrGetScreenManager()->GetPixelFormat(screenId, pixelFormat);
    return true;
}

bool SetScreenHDRFormat()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    int32_t modeIdx = GetData<int32_t>();
    CreateOrGetScreenManager()->SetScreenHDRFormat(screenId, modeIdx);
    return true;
}

bool GetScreenHDRFormat()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    ScreenHDRFormat hdrFormat = static_cast<ScreenHDRFormat>(GetData<uint8_t>() % SCREEN_HDR_FORMAT_SIZE);
    CreateOrGetScreenManager()->GetScreenHDRFormat(screenId, hdrFormat);
    return true;
}

bool GetScreenSupportedHDRFormats()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    ScreenHDRFormat hdrFormat = static_cast<ScreenHDRFormat>(GetData<uint8_t>() % SCREEN_HDR_FORMAT_SIZE);
    std::vector<ScreenHDRFormat> hdrFormats = { hdrFormat };
    CreateOrGetScreenManager()->GetScreenSupportedHDRFormats(screenId, hdrFormats);
    return true;
}

bool GetScreenColorSpace()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    GraphicCM_ColorSpaceType colorSpace =
        static_cast<GraphicCM_ColorSpaceType>(GetData<uint8_t>() % GRAPHIC_CM_COLOR_SPACE_TYPE_SIZE);
    CreateOrGetScreenManager()->GetScreenColorSpace(screenId, colorSpace);
    return true;
}

bool SetScreenColorSpace()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    GraphicCM_ColorSpaceType colorSpace =
        static_cast<GraphicCM_ColorSpaceType>(GetData<uint8_t>() % GRAPHIC_CM_COLOR_SPACE_TYPE_SIZE);
    CreateOrGetScreenManager()->SetScreenColorSpace(screenId, colorSpace);
    return true;
}

bool GetScreenSupportedColorSpaces()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    GraphicCM_ColorSpaceType colorSpace =
        static_cast<GraphicCM_ColorSpaceType>(GetData<uint8_t>() % GRAPHIC_CM_COLOR_SPACE_TYPE_SIZE);
    std::vector<GraphicCM_ColorSpaceType> colorSpaces = { colorSpace };
    CreateOrGetScreenManager()->GetScreenSupportedColorSpaces(screenId, colorSpaces);
    return true;
}

bool IsScreenPowerOff()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    CreateOrGetScreenManager()->IsScreenPowerOff(screenId);
    return true;
}

bool DisablePowerOffRenderControl()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    CreateOrGetScreenManager()->DisablePowerOffRenderControl(screenId);
    return true;
}

bool GetDisplayPropertyForHardCursor()
{
    uint32_t screenId = GetData<uint32_t>();
    CreateOrGetScreenManager()->GetDisplayPropertyForHardCursor(screenId);
    return true;
}

bool SetScreenHasProtectedLayer()
{
    uint32_t screenId = GetData<uint32_t>();
    bool hasProtectedLayer = GetData<bool>();
    CreateOrGetScreenManager()->SetScreenHasProtectedLayer(screenId, hasProtectedLayer);
    return true;
}

bool IsVisibleRectSupportRotation()
{
    ScreenId screenId = GetData<Rosen::ScreenId>();
    CreateOrGetScreenManager()->IsVisibleRectSupportRotation(screenId);
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (!OHOS::Rosen::Init(data, size)) {
        return 0;
    }
    std::map<uint8_t, FunctionPtr> funcMap = { { OHOS::Rosen::INIT_SCREENMANGER, OHOS::Rosen::InitScreenManger },
        { OHOS::Rosen::GET_ACTIVESCREENID, OHOS::Rosen::GetActiveScreenId },
        { OHOS::Rosen::SET_VIRTUAL_SCREENREFRESHRATE, OHOS::Rosen::SetVirtualScreenRefreshRate },
        { OHOS::Rosen::IS_ALLSCREENSPOWEROFF, OHOS::Rosen::IsAllScreensPowerOff },
        { OHOS::Rosen::POST_FORCE_REFRESHTASK, OHOS::Rosen::PostForceRefreshTask },
        { OHOS::Rosen::REMOVE_FORCE_REFRESHTASK, OHOS::Rosen::RemoveForceRefreshTask },
        { OHOS::Rosen::TRY_SIMPLEPROCESS_HOTPLUGEVENTS, OHOS::Rosen::TrySimpleProcessHotPlugEvents },
        { OHOS::Rosen::PROCESS_SCREENHOTPLUGEVENTS, OHOS::Rosen::ProcessScreenHotPlugEvents },
        { OHOS::Rosen::PROCESS_PENDINGCONNECTIONS, OHOS::Rosen::ProcessPendingConnections },
        { OHOS::Rosen::SET_DEFAULTSCREENID, OHOS::Rosen::SetDefaultScreenId },
        { OHOS::Rosen::GET_VIRTUAL_SCREENRESOLUTION, OHOS::Rosen::GetVirtualScreenResolution },
        { OHOS::Rosen::GET_SCREENACTIVEMODE, OHOS::Rosen::GetScreenActiveMode },
        { OHOS::Rosen::GET_SCREENSUPPORTEDMODES, OHOS::Rosen::GetScreenSupportedModes },
        { OHOS::Rosen::GET_SCREENCAPABILITY, OHOS::Rosen::GetScreenCapability },
        { OHOS::Rosen::GET_SCREENPOWERSTATUS, OHOS::Rosen::GetScreenPowerStatus },
        { OHOS::Rosen::GET_SCREENCORRECTION, OHOS::Rosen::GetScreenCorrection },
        { OHOS::Rosen::GET_DEFAULTSCREENID, OHOS::Rosen::GetDefaultScreenId },
        { OHOS::Rosen::GET_ALLSCREENIDS, OHOS::Rosen::GetAllScreenIds },
        { OHOS::Rosen::CREATE_VIRTUAL_SCREEN, OHOS::Rosen::CreateVirtualScreen },
        { OHOS::Rosen::SET_VIRTUAL_SCREENBLACKLIST, OHOS::Rosen::SetVirtualScreenBlackList },
        { OHOS::Rosen::SET_VIRTUAL_SCREENTYPEBLACKLIST, OHOS::Rosen::SetVirtualScreenTypeBlackList },
        { OHOS::Rosen::ADD_VIRTUAL_SCREENBLACKLIST, OHOS::Rosen::AddVirtualScreenBlackList },
        { OHOS::Rosen::REMOVE_VIRTUAL_SCREENBLACKLIST, OHOS::Rosen::RemoveVirtualScreenBlackList },
        { OHOS::Rosen::SET_VIRTUAL_SCREENSECURITYEXEMPTIONLIST, OHOS::Rosen::SetVirtualScreenSecurityExemptionList },
        { OHOS::Rosen::GET_VIRTUAL_SCREENSECURITYEXEMPTIONLIST, OHOS::Rosen::GetVirtualScreenSecurityExemptionList },
        { OHOS::Rosen::GET_SCREENSECURITYMASK, OHOS::Rosen::GetScreenSecurityMask },
        { OHOS::Rosen::SET_MIRRORSCREENVISIBLERECT, OHOS::Rosen::SetMirrorScreenVisibleRect },
        { OHOS::Rosen::GET_MIRRORSCREENVISIBLERECT, OHOS::Rosen::GetMirrorScreenVisibleRect },
        { OHOS::Rosen::SET_CASTSCREENENABLESKIPWINDOW, OHOS::Rosen::SetCastScreenEnableSkipWindow },
        { OHOS::Rosen::GET_VIRTUAL_SCREENBLACKLIST, OHOS::Rosen::GetVirtualScreenBlackList },
        { OHOS::Rosen::GET_VIRTUAL_SCREENTYPEBLACKLIST, OHOS::Rosen::GetVirtualScreenTypeBlackList },
        { OHOS::Rosen::GET_ALLBLACKLIST, OHOS::Rosen::GetAllBlackList },
        { OHOS::Rosen::GET_ALLWHITELIST, OHOS::Rosen::GetAllWhiteList },
        { OHOS::Rosen::GET_ANDRESET_VIRTUAL_SURFACEUPDATEFLAG, OHOS::Rosen::GetAndResetVirtualSurfaceUpdateFlag },
        { OHOS::Rosen::REMOVE_VIRTUAL_SCREEN, OHOS::Rosen::RemoveVirtualScreen },
        { OHOS::Rosen::SET_SCREENACTIVEMODE, OHOS::Rosen::SetScreenActiveMode },
        { OHOS::Rosen::SET_SCREENACTIVERECT, OHOS::Rosen::SetScreenActiveRect },
        { OHOS::Rosen::SET_PHYSICALSCREENRESOLUTION, OHOS::Rosen::SetPhysicalScreenResolution },
        { OHOS::Rosen::SET_VIRTUAL_SCREENRESOLUTION, OHOS::Rosen::SetVirtualScreenResolution },
        { OHOS::Rosen::SET_ROGSCREENRESOLUTION, OHOS::Rosen::SetRogScreenResolution },
        { OHOS::Rosen::SET_SCREENPOWERSTATUS, OHOS::Rosen::SetScreenPowerStatus },
        { OHOS::Rosen::IS_SCREENPOWERINGOFF, OHOS::Rosen::IsScreenPoweringOff },
        { OHOS::Rosen::SET_VIRTUAL_MIRRORSCREENCANVASROTATION, OHOS::Rosen::SetVirtualMirrorScreenCanvasRotation },
        { OHOS::Rosen::SET_VIRTUAL_MIRRORSCREENSCALEMODE, OHOS::Rosen::SetVirtualMirrorScreenScaleMode },
        { OHOS::Rosen::RELEASESCREENDMABUFFER, OHOS::Rosen::ReleaseScreenDmaBuffer },
        { OHOS::Rosen::GET_SCREENDATA, OHOS::Rosen::GetScreenData },
        { OHOS::Rosen::RESIZEVIRTUAL_SCREEN, OHOS::Rosen::ResizeVirtualScreen },
        { OHOS::Rosen::GET_SCREENBACKLIGHT, OHOS::Rosen::GetScreenBacklight },
        { OHOS::Rosen::SET_SCREENBACKLIGHT, OHOS::Rosen::SetScreenBacklight },
        { OHOS::Rosen::QUERYSCREENINFO, OHOS::Rosen::QueryScreenInfo },
        { OHOS::Rosen::GET_CANVASROTATION, OHOS::Rosen::GetCanvasRotation },
        { OHOS::Rosen::GET_SCALEMODE, OHOS::Rosen::GetScaleMode },
        { OHOS::Rosen::GET_PRODUCERSURFACE, OHOS::Rosen::GetProducerSurface },
        { OHOS::Rosen::GET_OUTPUT, OHOS::Rosen::GetOutput }, { OHOS::Rosen::DIS_PLAY_DUMP, OHOS::Rosen::DisplayDump },
        { OHOS::Rosen::SURFACE_DUMP, OHOS::Rosen::SurfaceDump }, { OHOS::Rosen::FPS_DUMP, OHOS::Rosen::FpsDump },
        { OHOS::Rosen::CLEARFPS_DUMP, OHOS::Rosen::ClearFpsDump },
        { OHOS::Rosen::HITCHS_DUMP, OHOS::Rosen::HitchsDump },
        { OHOS::Rosen::SET_SCREENCONSTEXPRRAINT, OHOS::Rosen::SetScreenConstraint },
        { OHOS::Rosen::GET_SCREENSUPPORTEDCOLORGAMUTS, OHOS::Rosen::GetScreenSupportedColorGamuts },
        { OHOS::Rosen::GET_SCREENCOLORGAMUT, OHOS::Rosen::GetScreenColorGamut },
        { OHOS::Rosen::SET_SCREENCOLORGAMUT, OHOS::Rosen::SetScreenColorGamut },
        { OHOS::Rosen::SET_SCREENGAMUTMAP, OHOS::Rosen::SetScreenGamutMap },
        { OHOS::Rosen::SET_SCREENCORRECTION, OHOS::Rosen::SetScreenCorrection },
        { OHOS::Rosen::GET_SCREENGAMUTMAP, OHOS::Rosen::GetScreenGamutMap },
        { OHOS::Rosen::GET_SCREENHDRCAPABILITY, OHOS::Rosen::GetScreenHDRCapability },
        { OHOS::Rosen::GET_SCREENTYPE, OHOS::Rosen::GetScreenType },
        { OHOS::Rosen::SET_SCREENSKIPFRAMEINTERVAL, OHOS::Rosen::SetScreenSkipFrameInterval },
        { OHOS::Rosen::SET_EQUALVSYNCPERIOD, OHOS::Rosen::SetEqualVsyncPeriod },
        { OHOS::Rosen::GET_DIS_PLAYIDENTIFICATIONDATA, OHOS::Rosen::GetDisplayIdentificationData },
        { OHOS::Rosen::SET_PIXELFORMAT, OHOS::Rosen::SetPixelFormat },
        { OHOS::Rosen::SET_SCREENHDRFORMAT, OHOS::Rosen::SetScreenHDRFormat },
        { OHOS::Rosen::GET_SCREENHDRFORMAT, OHOS::Rosen::GetScreenHDRFormat },
        { OHOS::Rosen::GET_SCREENSUPPORTEDHDRFORMATS, OHOS::Rosen::GetScreenSupportedHDRFormats },
        { OHOS::Rosen::GET_SCREENCOLORSPACE, OHOS::Rosen::GetScreenColorSpace },
        { OHOS::Rosen::SET_SCREENCOLORSPACE, OHOS::Rosen::SetScreenColorSpace },
        { OHOS::Rosen::GET_SCREENSUPPORTEDCOLORSPACES, OHOS::Rosen::GetScreenSupportedColorSpaces },
        { OHOS::Rosen::IS_SCREENPOWEROFF, OHOS::Rosen::IsScreenPowerOff },
        { OHOS::Rosen::DIS_ABLEPOWEROFFRENDERCONTROL, OHOS::Rosen::DisablePowerOffRenderControl },
        { OHOS::Rosen::GET_DIS_PLAYPROPERTYFORHARDCURSOR, OHOS::Rosen::GetDisplayPropertyForHardCursor },
        { OHOS::Rosen::SET_SCREENHASPROTECTEDLAYER, OHOS::Rosen::SetScreenHasProtectedLayer },
        { OHOS::Rosen::IS_VIS_IBLERECTSUPPORTROTATION, OHOS::Rosen::IsVisibleRectSupportRotation } };
    /* Run your code on data */
    uint8_t tarpos = OHOS::Rosen::GetData<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    if (funcMap.find(tarpos) != funcMap.end()) {
        return funcMap[tarpos]();
    }
    return 0;
}