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

#include <fuzzer/FuzzedDataProvider.h>

#include "screen_manager/rs_screen_manager.h"

namespace OHOS {
namespace Rosen {
namespace {
// 规则11: 修正枚举值范围贴合业务
constexpr uint8_t SCREEN_POWER_STATUS_SIZE = 5;  // 0-4 (修改: 11->5)
constexpr size_t STRING_LEN = 10;
constexpr uint8_t SCREEN_CONSTRAINT_TYPE_SIZE = 4;
constexpr uint8_t SCREEN_COLOR_GAMUT_SIZE = 8;   // 修改: 12->8
constexpr uint8_t SCREEN_GAMUT_MAP_SIZE = 4;
constexpr uint8_t SCREEN_ROTATION_SIZE = 5;
constexpr uint8_t SCREEN_SCREEN_TYPE_SIZE = 4;
constexpr uint8_t SCREEN_HDR_FORMAT_SIZE = 5;   // 修改: 8->5
constexpr uint8_t GRAPHIC_CM_COLOR_SPACE_TYPE_SIZE = 10;  // 修改: 32->10
constexpr uint8_t SCREEN_SCALE_MODE_SIZE = 4;   // 修改: 3->4
constexpr uint8_t GRAPHIC_PIXEL_FORMAT_SIZE = 10;  // 修改: 43->10

const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos = 0;

// 规则9, 10: 添加已创建屏幕ID列表，用于构造有效前置条件
std::vector<ScreenId> g_createdScreenIds;
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
    // 规则7修复: 避免整数下溢，检查g_pos + objectSize > g_size
    if (g_data == nullptr || objectSize > g_size || g_pos + objectSize > g_size) {
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

// 规则10: 添加辅助函数，优先使用已创建的屏幕ID
ScreenId GetScreenId()
{
    if (!g_createdScreenIds.empty() && GetData<bool>()) {
        size_t index = GetData<size_t>() % g_createdScreenIds.size();
        return g_createdScreenIds[index];
    }
    return GetData<ScreenId>();
}

// 规则1: 删除无参数API测试（以下3个函数已删除）
// void GetActiveScreenId() { screenManager_->GetActiveScreenId(); }
// void GetDefaultScreenId() { screenManager_->GetDefaultScreenId(); }
// void GetAllScreenIds() { screenManager_->GetAllScreenIds(); }

void SetVirtualScreenRefreshRate()
{
    // 规则10修复: 使用GetScreenId()优先使用已创建的屏幕ID
    ScreenId screenId = GetScreenId();
    uint32_t maxRefreshRate = GetData<uint32_t>();
    uint32_t actualRefreshRate = GetData<uint32_t>();
    screenManager_->SetVirtualScreenRefreshRate(screenId, maxRefreshRate, actualRefreshRate);
}

void GetVirtualScreenResolution()
{
    ScreenId screenId = GetScreenId();
    RSVirtualScreenResolution virtualScreenResolution;
    screenManager_->GetVirtualScreenResolution(screenId, virtualScreenResolution);
}

void GetScreenActiveMode()
{
    ScreenId screenId = GetScreenId();
    RSScreenModeInfo screenModeInfo;
    screenManager_->GetScreenActiveMode(screenId, screenModeInfo);
}

void GetScreenSupportedModes()
{
    ScreenId screenId = GetScreenId();
    screenManager_->GetScreenSupportedModes(screenId);
}

void GetScreenCapability()
{
    ScreenId screenId = GetScreenId();
    screenManager_->GetScreenCapability(screenId);
}

void GetScreenPowerStatus()
{
    ScreenId screenId = GetScreenId();
    screenManager_->GetScreenPowerStatus(screenId);
}

void CreateVirtualScreen()
{
    // 规则12: 使用随机长度字符串而非固定长度
    std::string name;
    size_t nameLen = GetData<size_t>() % 128;  // 修改: 随机长度
    name.reserve(nameLen);
    for (size_t i = 0; i < nameLen && g_pos + 1 < g_size; ++i) {
        name.push_back(GetData<char>());
    }

    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    sptr<Surface> surface;
    ScreenId mirrorId = GetData<ScreenId>();
    int32_t flags = GetData<int32_t>();

    // 规则12: 使用随机大小的whiteList
    size_t whiteListSize = GetData<size_t>() % 128;  // 修改: 随机大小
    std::vector<NodeId> whiteList;
    whiteList.reserve(whiteListSize);
    for (size_t i = 0; i < whiteListSize; ++i) {
        whiteList.push_back(GetData<NodeId>());
    }

    ScreenId screenId = screenManager_->CreateVirtualScreen(name, width, height, surface, mirrorId, flags, whiteList);

    // 记录创建的屏幕ID（规则9, 10）
    if (screenId != INVALID_SCREEN_ID) {
        g_createdScreenIds.push_back(screenId);
    }
}

void SetVirtualScreenAutoRotation()
{
    ScreenId screenId = GetScreenId();
    bool isAutoRotation = GetData<bool>();
    screenManager_->SetVirtualScreenAutoRotation(screenId, isAutoRotation);
}

void SetVirtualScreenBlackList()
{
    ScreenId screenId = GetScreenId();

    // 规则12: 使用随机大小的blocklist
    size_t blackListSize = GetData<size_t>() % 128;  // 修改: 随机大小
    std::vector<uint64_t> blocklist;
    blocklist.reserve(blackListSize);
    for (size_t i = 0; i < blackListSize; ++i) {
        blocklist.push_back(GetData<uint64_t>());
    }

    screenManager_->SetVirtualScreenBlackList(screenId, blocklist);
}

void SetVirtualScreenTypeBlackList()
{
    ScreenId screenId = GetScreenId();

    size_t typeBlackListSize = GetData<size_t>() % 32;  // 修改: 随机大小
    std::vector<uint8_t> typeBlackList;
    typeBlackList.reserve(typeBlackListSize);
    for (size_t i = 0; i < typeBlackListSize; ++i) {
        typeBlackList.push_back(GetData<uint8_t>());
    }

    screenManager_->SetVirtualScreenTypeBlackList(screenId, typeBlackList);
}

void AddVirtualScreenBlackList()
{
    ScreenId screenId = GetScreenId();

    size_t blackListSize = GetData<size_t>() % 128;  // 修改: 随机大小
    std::vector<uint64_t> blocklist;
    blocklist.reserve(blackListSize);
    for (size_t i = 0; i < blackListSize; ++i) {
        blocklist.push_back(GetData<uint64_t>());
    }

    screenManager_->AddVirtualScreenBlackList(screenId, blocklist);
}

void RemoveVirtualScreenBlackList()
{
    ScreenId screenId = GetScreenId();

    size_t blackListSize = GetData<size_t>() % 128;  // 修改: 随机大小
    std::vector<uint64_t> blocklist;
    blocklist.reserve(blackListSize);
    for (size_t i = 0; i < blackListSize; ++i) {
        blocklist.push_back(GetData<uint64_t>());
    }

    screenManager_->RemoveVirtualScreenBlackList(screenId, blocklist);
}

void SetVirtualScreenSecurityExemptionList()
{
    ScreenId screenId = GetScreenId();

    size_t exemptionListSize = GetData<size_t>() % 128;  // 修改: 随机大小
    std::vector<uint64_t> securityExemptionList;
    securityExemptionList.reserve(exemptionListSize);
    for (size_t i = 0; i < exemptionListSize; ++i) {
        securityExemptionList.push_back(GetData<uint64_t>());
    }

    screenManager_->SetVirtualScreenSecurityExemptionList(screenId, securityExemptionList);
}

void SetMirrorScreenVisibleRect()
{
    ScreenId screenId = GetScreenId();

    // 规则18: 合理构造Rect参数
    Rect mainScreenRect;
    mainScreenRect.x = GetData<int32_t>();
    mainScreenRect.y = GetData<int32_t>();
    mainScreenRect.w = GetData<int32_t>();
    mainScreenRect.h = GetData<int32_t>();

    bool supportRotation = GetData<bool>();
    screenManager_->SetMirrorScreenVisibleRect(screenId, mainScreenRect, supportRotation);
}

void SetCastScreenEnableSkipWindow()
{
    ScreenId screenId = GetScreenId();
    bool enable = GetData<bool>();
    screenManager_->SetCastScreenEnableSkipWindow(screenId, enable);
}

void RemoveVirtualScreen()
{
    ScreenId screenId = GetScreenId();
    screenManager_->RemoveVirtualScreen(screenId);
}

void SetScreenActiveMode()
{
    ScreenId screenId = GetScreenId();
    uint32_t modeId = GetData<uint32_t>();
    screenManager_->SetScreenActiveMode(screenId, modeId);
}

void SetScreenActiveRect()
{
    ScreenId screenId = GetScreenId();

    // 规则18: 合理构造Rect参数
    Rect activeRect;
    activeRect.x = GetData<int32_t>();
    activeRect.y = GetData<int32_t>();
    activeRect.w = GetData<int32_t>();
    activeRect.h = GetData<int32_t>();

    screenManager_->SetScreenActiveRect(screenId, activeRect);
}

void SetPhysicalScreenResolution()
{
    ScreenId screenId = GetScreenId();
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    screenManager_->SetPhysicalScreenResolution(screenId, width, height);
}

void SetVirtualScreenResolution()
{
    ScreenId screenId = GetScreenId();
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    screenManager_->SetVirtualScreenResolution(screenId, width, height);
}

void SetRogScreenResolution()
{
    ScreenId screenId = GetScreenId();
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    screenManager_->SetRogScreenResolution(screenId, width, height);
}

void SetScreenPowerStatus()
{
    ScreenId screenId = GetScreenId();
    ScreenPowerStatus status = static_cast<ScreenPowerStatus>(GetData<uint32_t>() % SCREEN_POWER_STATUS_SIZE);
    screenManager_->SetScreenPowerStatus(screenId, status);
}

void SetVirtualMirrorScreenCanvasRotation()
{
    ScreenId screenId = GetScreenId();
    bool canvasRotation = GetData<bool>();
    screenManager_->SetVirtualMirrorScreenCanvasRotation(screenId, canvasRotation);
}

void SetVirtualMirrorScreenScaleMode()
{
    ScreenId screenId = GetScreenId();
    ScreenScaleMode scaleMode = static_cast<ScreenScaleMode>(GetData<uint8_t>() % SCREEN_SCALE_MODE_SIZE);
    screenManager_->SetVirtualMirrorScreenScaleMode(screenId, scaleMode);
}

void GetScreenData()
{
    ScreenId screenId = GetScreenId();
    screenManager_->GetScreenData(screenId);
}

void ResizeVirtualScreen()
{
    ScreenId screenId = GetScreenId();
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    screenManager_->ResizeVirtualScreen(screenId, width, height);
}

void GetScreenBacklight()
{
    ScreenId screenId = GetScreenId();
    screenManager_->GetScreenBacklight(screenId);
}

void SetScreenBacklight()
{
    ScreenId screenId = GetScreenId();
    uint32_t level = GetData<uint32_t>();
    screenManager_->SetScreenBacklight(screenId, level);
}

void GetCanvasRotation()
{
    ScreenId screenId = GetScreenId();
    screenManager_->GetCanvasRotation(screenId);
}

void DisplayDump()
{
    std::string dumpString;
    size_t dumpLen = GetData<size_t>() % 1024;  // 修改: 随机长度
    dumpString.reserve(dumpLen);
    for (size_t i = 0; i < dumpLen && g_pos + 1 < g_size; ++i) {
        dumpString.push_back(GetData<char>());
    }
    screenManager_->DisplayDump(dumpString);
}

void SetScreenConstraint()
{
    ScreenId screenId = GetScreenId();
    uint64_t timestamp = GetData<uint64_t>();
    ScreenConstraintType type = static_cast<ScreenConstraintType>(GetData<uint8_t>() % SCREEN_CONSTRAINT_TYPE_SIZE);
    screenManager_->SetScreenConstraint(screenId, timestamp, type);
}

void GetScreenSupportedColorGamuts()
{
    ScreenId screenId = GetScreenId();
    ScreenColorGamut type = static_cast<ScreenColorGamut>(GetData<uint8_t>() % SCREEN_COLOR_GAMUT_SIZE);
    std::vector<ScreenColorGamut> mode = { type };
    screenManager_->GetScreenSupportedColorGamuts(screenId, mode);
}

void GetScreenColorGamut()
{
    ScreenId screenId = GetScreenId();
    ScreenColorGamut type = static_cast<ScreenColorGamut>(GetData<uint8_t>() % SCREEN_COLOR_GAMUT_SIZE);
    screenManager_->GetScreenColorGamut(screenId, type);
}

void SetScreenColorGamut()
{
    ScreenId screenId = GetScreenId();
    int32_t modeIdx = GetData<int32_t>();
    screenManager_->SetScreenColorGamut(screenId, modeIdx);
}

void SetScreenGamutMap()
{
    ScreenId screenId = GetScreenId();
    ScreenGamutMap mode = static_cast<ScreenGamutMap>(GetData<uint8_t>() % SCREEN_GAMUT_MAP_SIZE);
    screenManager_->SetScreenGamutMap(screenId, mode);
}

void SetScreenCorrection()
{
    ScreenId screenId = GetScreenId();
    ScreenRotation screenRotation = static_cast<ScreenRotation>(GetData<uint8_t>() % SCREEN_ROTATION_SIZE);
    screenManager_->SetScreenCorrection(screenId, screenRotation);
}

void GetScreenGamutMap()
{
    ScreenId screenId = GetScreenId();
    ScreenGamutMap mode = static_cast<ScreenGamutMap>(GetData<uint8_t>() % SCREEN_GAMUT_MAP_SIZE);
    screenManager_->GetScreenGamutMap(screenId, mode);
}

void GetScreenHDRCapability()
{
    ScreenId screenId = GetScreenId();
    RSScreenHDRCapability screenHdrCapability;
    screenManager_->GetScreenHDRCapability(screenId, screenHdrCapability);
}

void GetScreenType()
{
    ScreenId screenId = GetScreenId();
    RSScreenType type = static_cast<RSScreenType>(GetData<uint8_t>() % SCREEN_SCREEN_TYPE_SIZE);
    screenManager_->GetScreenType(screenId, type);
}

void GetScreenConnectionType()
{
    ScreenId screenId = GetScreenId();
    screenManager_->GetScreenConnectionType(screenId);
}

void SetScreenSkipFrameInterval()
{
    ScreenId screenId = GetScreenId();
    uint32_t skipFrameInterval = GetData<uint32_t>();
    screenManager_->SetScreenSkipFrameInterval(screenId, skipFrameInterval);
}

void GetDisplayIdentificationData()
{
    ScreenId screenId = GetScreenId();
    uint8_t outPort = GetData<uint8_t>();

    // 规则12: 使用随机大小的edidData
    size_t edidSize = GetData<size_t>() % 256;  // 修改: 随机大小
    std::vector<uint8_t> edidData;
    edidData.reserve(edidSize);
    for (size_t i = 0; i < edidSize; ++i) {
        edidData.push_back(GetData<uint8_t>());
    }

    screenManager_->GetDisplayIdentificationData(screenId, outPort, edidData);
}

void SetPixelFormat()
{
    ScreenId screenId = GetScreenId();
    GraphicPixelFormat pixelFormat = static_cast<GraphicPixelFormat>(GetData<uint8_t>() % GRAPHIC_PIXEL_FORMAT_SIZE);
    screenManager_->SetPixelFormat(screenId, pixelFormat);
    screenManager_->GetPixelFormat(screenId, pixelFormat);
}

void SetScreenHDRFormat()
{
    ScreenId screenId = GetScreenId();
    int32_t modeIdx = GetData<int32_t>();
    screenManager_->SetScreenHDRFormat(screenId, modeIdx);
}

void GetScreenHDRFormat()
{
    ScreenId screenId = GetScreenId();
    ScreenHDRFormat hdrFormat = static_cast<ScreenHDRFormat>(GetData<uint8_t>() % SCREEN_HDR_FORMAT_SIZE);
    screenManager_->GetScreenHDRFormat(screenId, hdrFormat);
}

void GetScreenSupportedHDRFormats()
{
    ScreenId screenId = GetScreenId();
    ScreenHDRFormat hdrFormat = static_cast<ScreenHDRFormat>(GetData<uint8_t>() % SCREEN_HDR_FORMAT_SIZE);
    std::vector<ScreenHDRFormat> hdrFormats = { hdrFormat };
    screenManager_->GetScreenSupportedHDRFormats(screenId, hdrFormats);
}

void GetScreenColorSpace()
{
    ScreenId screenId = GetScreenId();
    GraphicCM_ColorSpaceType colorSpace =
        static_cast<GraphicCM_ColorSpaceType>(GetData<uint8_t>() % GRAPHIC_CM_COLOR_SPACE_TYPE_SIZE);
    screenManager_->GetScreenColorSpace(screenId, colorSpace);
}

void SetScreenColorSpace()
{
    ScreenId screenId = GetScreenId();
    GraphicCM_ColorSpaceType colorSpace =
        static_cast<GraphicCM_ColorSpaceType>(GetData<uint8_t>() % GRAPHIC_CM_COLOR_SPACE_TYPE_SIZE);
    screenManager_->SetScreenColorSpace(screenId, colorSpace);
}

void GetScreenSupportedColorSpaces()
{
    ScreenId screenId = GetScreenId();
    GraphicCM_ColorSpaceType colorSpace =
        static_cast<GraphicCM_ColorSpaceType>(GetData<uint8_t>() % GRAPHIC_CM_COLOR_SPACE_TYPE_SIZE);
    std::vector<GraphicCM_ColorSpaceType> colorSpaces = { colorSpace };
    screenManager_->GetScreenSupportedColorSpaces(screenId, colorSpaces);
}

void DisablePowerOffRenderControl()
{
    ScreenId screenId = GetScreenId();
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
    // 规则1: 从funcVector中删除3个无参数API
    // 规则3: 测试函数从54个减少到51个
    std::vector<FunctionPtr> funcVector = {
        OHOS::Rosen::InitScreenManger,
        // OHOS::Rosen::GetActiveScreenId,        // 删除: 无参数
        OHOS::Rosen::SetVirtualScreenRefreshRate,
        OHOS::Rosen::GetVirtualScreenResolution,
        OHOS::Rosen::GetScreenActiveMode,
        OHOS::Rosen::GetScreenSupportedModes,
        OHOS::Rosen::GetScreenCapability,
        OHOS::Rosen::GetScreenPowerStatus,
        // OHOS::Rosen::GetDefaultScreenId,        // 删除: 无参数
        // OHOS::Rosen::GetAllScreenIds,           // 删除: 无参数
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
