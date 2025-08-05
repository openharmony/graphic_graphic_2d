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

#include "rsrenderserviceconnectionproxy_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <hilog/log.h>
#include <iservice_registry.h>
#include <memory>
#include <securec.h>
#include <system_ability_definition.h>
#include <unistd.h>

#include "command/rs_animation_command.h"
#include "command/rs_node_showing_command.h"
#include "platform/ohos/rs_render_service_connection_proxy.h"
#include "ipc_callbacks/rs_frame_rate_linker_expected_fps_update_callback_proxy.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;

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

template<>
std::string GetData()
{
    size_t objectSize = GetData<uint8_t>();
    std::string object(objectSize, '\0');
    if (g_data == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    object.assign(reinterpret_cast<const char*>(g_data + g_pos), objectSize);
    g_pos += objectSize;
    return object;
}
} // namespace

bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    Rect srcRect;
    sptr<Surface> surface;
    sptr<IConsumerSurface> consumer = IConsumerSurface::Create("DisplayNode");
    sptr<IBufferProducer> producer = consumer->GetProducer();
    surface = Surface::CreateSurfaceAsProducer(producer);
    float cpuMemSize = GetData<float>();
    float gpuMemSize = GetData<float>();
    int pid = GetData<int>();
    uint64_t timeoutNS = GetData<uint64_t>();
    RSSurfaceRenderNodeConfig config;
    config.id = timeoutNS;
    auto transactionData = std::make_unique<RSTransactionData>();
    std::shared_ptr<RSSyncTask> task;
    sptr<VSyncIConnectionToken> token;
    std::string name("name");
    uint64_t id1 = GetData<uint64_t>();
    int64_t id2 = GetData<int64_t>();
    uint64_t windowNodeId = GetData<uint64_t>();
    int32_t pid1 = GetData<int32_t>();
    int32_t uid = GetData<int32_t>();
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    int32_t expectedFps = GetData<int32_t>();
    int32_t x = GetData<int32_t>();
    int32_t y = GetData<int32_t>();
    int32_t w = GetData<int32_t>();
    int32_t h = GetData<int32_t>();
    Rect activeRect {
        .x = x,
        .y = y,
        .w = w,
        .h = h
    };
    Drawing::Bitmap bitmap;
    RSScreenHDRCapability screenHdrCapability;
    GraphicCM_ColorSpaceType colorSpace = GraphicCM_ColorSpaceType::GRAPHIC_CM_SRGB_FULL;
    RSScreenType screenType = RSScreenType::VIRTUAL_TYPE_SCREEN;
    sptr<IApplicationAgent> app;
    ScreenPowerStatus status = (ScreenPowerStatus)width;
    ScreenColorGamut screenColorGamut = (ScreenColorGamut)width;
    ScreenGamutMap screenGamutMap = (ScreenGamutMap)width;
    ScreenHDRFormat screenHDRFormat = (ScreenHDRFormat)width;
    SystemAnimatedScenes systemAnimatedScenes = (SystemAnimatedScenes)width;
    GraphicPixelFormat pixelFormat = GRAPHIC_PIXEL_FMT_BGRA_8888;
    FrameRateRange range;
    sptr<RSIScreenChangeCallback> callback;
    sptr<RSISurfaceCaptureCallback> callback1;
    sptr<RSIBufferAvailableCallback> callback2;
    sptr<RSIBufferClearCallback> callback3;
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    sptr<RSIOcclusionChangeCallback> rsIOcclusionChangeCallback = iface_cast<RSIOcclusionChangeCallback>(remoteObject);
    sptr<RSISurfaceOcclusionChangeCallback> callbackTwo = iface_cast<RSISurfaceOcclusionChangeCallback>(remoteObject);
    sptr<RSIHgmConfigChangeCallback> rsIHgmConfigChangeCallback = iface_cast<RSIHgmConfigChangeCallback>(remoteObject);
    sptr<RSIFirstFrameCommitCallback> rsIFirstFrameCommitCallback =
        iface_cast<RSIFirstFrameCommitCallback>(remoteObject);
    sptr<RSIFrameRateLinkerExpectedFpsUpdateCallback> rsIFrameRateLinkerExpectedFpsUpdateCallback =
        iface_cast<RSIFrameRateLinkerExpectedFpsUpdateCallback>(remoteObject);
    std::vector<ScreenColorGamut> mode;
    std::vector<ScreenHDRMetadataKey> keys;
    std::vector<ScreenHDRFormat> hdrFormats;
    std::vector<GraphicCM_ColorSpaceType> colorSpaces;
    std::vector<float> partitionPoints;
    std::vector<std::string> packageList;
    OnRemoteDiedCallback onRemoteDiedCallback = []() {};
    callback = iface_cast<RSIScreenChangeCallback>(remoteObject);
    std::shared_ptr<Media::PixelMap> pixelmap = std::make_shared<Media::PixelMap>();
    std::shared_ptr<Drawing::DrawCmdList> drawCmdList = std::make_shared<Drawing::DrawCmdList>();
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeDefault();
    std::shared_ptr<Media::PixelMap> watermarkImg = std::make_shared<Media::PixelMap>();
    std::shared_ptr<MessageParcel> parcel = std::make_shared<MessageParcel>();
    MessageOption option;
    Drawing::Rect rect;
    EventInfo eventInfo;
    DataBaseRs info;
    GameStateData gameStateDataInfo;
    MessageParcel messageParcel;
    MessageParcel reply;
    RSFrameRateLinkerExpectedFpsUpdateCallbackProxy rsFrameRateLinkerExpectedFpsUpdateCallbackProxy(remoteObject);
    RSRenderServiceConnectionProxy rsRenderServiceConnectionProxy(remoteObject);
    MemoryGraphic memoryGraphic;
    std::vector<MemoryGraphic> memoryGraphics;
    bool getUniRenderEnable;
    bool createNodeSuccess;
    std::string getRefreshInfoEnable;
    std::string getRefreshInfoToSPEnable;
    bool getBitmapSuccess;
    bool getPixelmapSuccess;
    FocusAppInfo appInfo = {
        .pid = pid1,
        .uid = uid,
        .bundleName = name,
        .abilityName = name,
        .focusNodeId = id1};
    int32_t repCode = GetData<int32_t>();
    uint32_t retureCode = GetData<uint32_t>();
    bool success = GetData<bool>();
    int32_t expectedFrameRate = GetData<int32_t>();

    rsRenderServiceConnectionProxy.CommitTransaction(transactionData);
    rsRenderServiceConnectionProxy.ExecuteSynchronousTask(task);
    rsRenderServiceConnectionProxy.GetMemoryGraphic(pid, memoryGraphic);
    rsRenderServiceConnectionProxy.GetMemoryGraphics(memoryGraphics);
    rsRenderServiceConnectionProxy.GetTotalAppMemSize(cpuMemSize, gpuMemSize);
    rsRenderServiceConnectionProxy.GetUniRenderEnabled(getUniRenderEnable);
    rsRenderServiceConnectionProxy.CreateNode(config, createNodeSuccess);
    sptr<Surface> sface = nullptr;
    rsRenderServiceConnectionProxy.CreateNodeAndSurface(config, sface);
    sptr<IVSyncConnection> conn = nullptr;
    VSyncConnParam vsyncConnParam = {id1, windowNodeId, false};
    bool enable = GetData<bool>();
    uint32_t powerStatus = GetData<uint32_t>();
    int32_t backlightlevel = GetData<int32_t>();
    uint64_t screenId = GetData<uint64_t>();
    RSScreenModeInfo modeInfo;
    int32_t resCode;

    rsRenderServiceConnectionProxy.CreateVSyncConnection(conn, name, token, vsyncConnParam);
    std::shared_ptr<Media::PixelMap> pixelMap = nullptr;
    rsRenderServiceConnectionProxy.CreatePixelMapFromSurface(surface, srcRect, pixelMap);
    rsRenderServiceConnectionProxy.SetFocusAppInfo(appInfo, repCode);
    rsRenderServiceConnectionProxy.GetDefaultScreenId(screenId);
    rsRenderServiceConnectionProxy.GetActiveScreenId(screenId);
    rsRenderServiceConnectionProxy.GetAllScreenIds();
    rsRenderServiceConnectionProxy.CreateVirtualScreen(name, width, height, surface);
    rsRenderServiceConnectionProxy.SetVirtualScreenSurface(id1, surface);
    rsRenderServiceConnectionProxy.RemoveVirtualScreen(id1);
    rsRenderServiceConnectionProxy.SetScreenChangeCallback(callback);
    rsRenderServiceConnectionProxy.SetScreenActiveMode(id1, width);
    rsRenderServiceConnectionProxy.SetScreenActiveRect(id1, activeRect, retureCode);
    rsRenderServiceConnectionProxy.SetScreenRefreshRate(id1, pid1, uid);
    rsRenderServiceConnectionProxy.SetRefreshRateMode(pid1);
    rsRenderServiceConnectionProxy.SyncFrameRateRange(id1, range, 0);
    rsRenderServiceConnectionProxy.GetScreenCurrentRefreshRate(id1);
    rsRenderServiceConnectionProxy.GetCurrentRefreshRateMode();
    rsRenderServiceConnectionProxy.GetScreenSupportedRefreshRates(id1);
    rsRenderServiceConnectionProxy.GetShowRefreshRateEnabled(enable);
    rsRenderServiceConnectionProxy.SetShowRefreshRateEnabled(true, 0);
    rsRenderServiceConnectionProxy.GetRealtimeRefreshRate(id1);
    rsRenderServiceConnectionProxy.GetRefreshInfo(id1, getRefreshInfoEnable);
    rsRenderServiceConnectionProxy.GetRefreshInfoToSP(id1, getRefreshInfoToSPEnable);
    rsRenderServiceConnectionProxy.SetPhysicalScreenResolution(id1, width, height);
    rsRenderServiceConnectionProxy.SetVirtualScreenResolution(id1, width, height);
    rsRenderServiceConnectionProxy.SetScreenPowerStatus(id1, status);
    rsRenderServiceConnectionProxy.RegisterApplicationAgent(width, app);
    rsRenderServiceConnectionProxy.GetVirtualScreenResolution(id1);
    rsRenderServiceConnectionProxy.GetScreenActiveMode(id1, modeInfo);
    rsRenderServiceConnectionProxy.GetScreenSupportedModes(id1);
    rsRenderServiceConnectionProxy.GetScreenCapability(id1);
    rsRenderServiceConnectionProxy.GetScreenPowerStatus(id1, powerStatus);
    rsRenderServiceConnectionProxy.GetScreenData(id1);
    rsRenderServiceConnectionProxy.GetScreenBacklight(id1, backlightlevel);
    rsRenderServiceConnectionProxy.SetScreenBacklight(id1, width);
    rsRenderServiceConnectionProxy.RegisterBufferAvailableListener(id1, callback2, true);
    rsRenderServiceConnectionProxy.RegisterBufferClearListener(id1, callback3);
    rsRenderServiceConnectionProxy.GetScreenSupportedColorGamuts(id1, mode);
    rsRenderServiceConnectionProxy.GetScreenSupportedMetaDataKeys(id1, keys);
    rsRenderServiceConnectionProxy.GetScreenColorGamut(id1, screenColorGamut);
    rsRenderServiceConnectionProxy.SetScreenColorGamut(id1, uid);
    rsRenderServiceConnectionProxy.SetScreenGamutMap(id1, screenGamutMap);
    rsRenderServiceConnectionProxy.GetScreenHDRCapability(id1, screenHdrCapability);
    rsRenderServiceConnectionProxy.GetPixelFormat(id1, pixelFormat, resCode);
    rsRenderServiceConnectionProxy.SetPixelFormat(id1, pixelFormat, resCode);
    rsRenderServiceConnectionProxy.GetScreenSupportedHDRFormats(id1, hdrFormats, resCode);
    rsRenderServiceConnectionProxy.GetScreenHDRFormat(id1, screenHDRFormat, resCode);
    rsRenderServiceConnectionProxy.SetScreenHDRFormat(id1, pid1, resCode);
    rsRenderServiceConnectionProxy.GetScreenSupportedColorSpaces(id1, colorSpaces, resCode);
    rsRenderServiceConnectionProxy.GetScreenColorSpace(id1, colorSpace, resCode);
    rsRenderServiceConnectionProxy.SetScreenColorSpace(id1, colorSpace, resCode);
    rsRenderServiceConnectionProxy.GetScreenType(id1, screenType);
    rsRenderServiceConnectionProxy.GetBitmap(id1, bitmap, getBitmapSuccess);
    rsRenderServiceConnectionProxy.GetPixelmap(id1, pixelmap, &rect, drawCmdList, getPixelmapSuccess);
    rsRenderServiceConnectionProxy.RegisterTypeface(id1, typeface);
    rsRenderServiceConnectionProxy.UnRegisterTypeface(id1);
    rsRenderServiceConnectionProxy.SetScreenSkipFrameInterval(id1, width, resCode);
    rsRenderServiceConnectionProxy.RegisterOcclusionChangeCallback(rsIOcclusionChangeCallback, repCode);
    rsRenderServiceConnectionProxy.RegisterSurfaceOcclusionChangeCallback(id1, callbackTwo, partitionPoints);
    rsRenderServiceConnectionProxy.UnRegisterSurfaceOcclusionChangeCallback(id1);
    rsRenderServiceConnectionProxy.RegisterHgmConfigChangeCallback(rsIHgmConfigChangeCallback);
    rsRenderServiceConnectionProxy.RegisterHgmRefreshRateModeChangeCallback(rsIHgmConfigChangeCallback);
    rsRenderServiceConnectionProxy.RegisterHgmRefreshRateUpdateCallback(rsIHgmConfigChangeCallback);
    rsRenderServiceConnectionProxy.RegisterFirstFrameCommitCallback(rsIFirstFrameCommitCallback);
    rsRenderServiceConnectionProxy.SetAppWindowNum(width);
    rsRenderServiceConnectionProxy.SetSystemAnimatedScenes(systemAnimatedScenes, false, success);
    rsRenderServiceConnectionProxy.ShowWatermark(watermarkImg, true);
    rsRenderServiceConnectionProxy.ResizeVirtualScreen(id1, width, height);
    rsRenderServiceConnectionProxy.ReportJankStats();
    rsRenderServiceConnectionProxy.NotifyLightFactorStatus(1);
    rsRenderServiceConnectionProxy.NotifyPackageEvent(width, packageList);
    rsRenderServiceConnectionProxy.NotifyRefreshRateEvent(eventInfo);
    rsRenderServiceConnectionProxy.NotifySoftVsyncRateDiscountEvent(1, name, 1);
    rsRenderServiceConnectionProxy.NotifyTouchEvent(pid1, uid);
    rsRenderServiceConnectionProxy.NotifyDynamicModeEvent(true);
    rsRenderServiceConnectionProxy.NotifyHgmConfigEvent(name, true);
    rsRenderServiceConnectionProxy.NotifyXComponentExpectedFrameRate(name, expectedFrameRate);
    rsRenderServiceConnectionProxy.ReportEventResponse(info);
    rsRenderServiceConnectionProxy.ReportEventComplete(info);
    rsRenderServiceConnectionProxy.ReportEventJankFrame(info);
    rsRenderServiceConnectionProxy.ReportGameStateData(gameStateDataInfo);
    rsRenderServiceConnectionProxy.SetHardwareEnabled(id1, true, SelfDrawingNodeType::DEFAULT, true);
    rsRenderServiceConnectionProxy.SetCacheEnabledForRotation(true);
    rsRenderServiceConnectionProxy.SetOnRemoteDiedCallback(onRemoteDiedCallback);
    rsRenderServiceConnectionProxy.RunOnRemoteDiedCallback();
    rsRenderServiceConnectionProxy.GetActiveDirtyRegionInfo();
    rsRenderServiceConnectionProxy.GetGlobalDirtyRegionInfo();
    rsRenderServiceConnectionProxy.GetLayerComposeInfo();
    rsRenderServiceConnectionProxy.GetHwcDisabledReasonInfo();
    rsRenderServiceConnectionProxy.GetHdrOnDuration(id2);
    rsRenderServiceConnectionProxy.SetVmaCacheStatus(true);
    rsRenderServiceConnectionProxy.SetVmaCacheStatus(false);
    rsRenderServiceConnectionProxy.SetVirtualScreenUsingStatus(true);
    rsRenderServiceConnectionProxy.SetVirtualScreenUsingStatus(false);
    rsRenderServiceConnectionProxy.SetCurtainScreenUsingStatus(true);
    rsRenderServiceConnectionProxy.FillParcelWithTransactionData(transactionData, parcel);
    rsRenderServiceConnectionProxy.ReportDataBaseRs(messageParcel, reply, option, info);
    rsRenderServiceConnectionProxy.ReportGameStateDataRs(messageParcel, reply, option, gameStateDataInfo);
    rsRenderServiceConnectionProxy.SetFreeMultiWindowStatus(true);
    rsRenderServiceConnectionProxy.SetFreeMultiWindowStatus(false);
    rsRenderServiceConnectionProxy.RegisterFrameRateLinkerExpectedFpsUpdateCallback(pid1,
        rsIFrameRateLinkerExpectedFpsUpdateCallback);
    rsFrameRateLinkerExpectedFpsUpdateCallbackProxy.OnFrameRateLinkerExpectedFpsUpdate(pid, expectedFps);
    return true;
}

#ifdef TP_FEATURE_ENABLE
bool OHOS::Rosen::DoSetTpFeatureConfigFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // get data
    int32_t tpFeature = GetData<int32_t>();
    std::string tpConfig = GetData<std::string>();
    auto tpFeatureConfigType = static_cast<TpFeatureConfigType>(GetData<uint8_t>());

    // test
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    RSRenderServiceConnectionProxy rsRenderServiceConnectionProxy(remoteObject);
    RSRenderServiceConnectionProxy.SetTpFeatureConfig(tpFeature, tpConfig.c_str(), tpFeatureConfigType);
    return true;
}
#endif

#ifdef RS_ENABLE_OVERLAY_DISPLAY
bool DoSetOverlayDisplayModeFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // get data
    int32_t mode = GetData<int32_t>();

    // test
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    RSRenderServiceConnectionProxy rsRenderServiceConnectionProxy(remoteObject);
    rsRenderServiceConnectionProxy.SetOverlayDisplayMode(mode);
    return true;
}
#endif

bool DoTakeSurfaceCapture(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // get data
    uint64_t nodeId = GetData<uint64_t>();
    sptr<RSISurfaceCaptureCallback> callback;
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = GetData<float>();
    captureConfig.scaleY = GetData<float>();
    captureConfig.useDma = GetData<bool>();
    captureConfig.useCurWindow = GetData<bool>();
    uint8_t type = GetData<uint8_t>();
    captureConfig.captureType = (SurfaceCaptureType)type;
    captureConfig.isSync = GetData<bool>();
    uint8_t listSize = GetData<uint8_t>();
    for (uint8_t i = 0; i < listSize; ++i) {
        uint64_t nodeid = GetData<uint64_t>();
        captureConfig.blackList.push_back(nodeid);
    }
    captureConfig.mainScreenRect.left_ = GetData<float>();
    captureConfig.mainScreenRect.top_ = GetData<float>();
    captureConfig.mainScreenRect.right_ = GetData<float>();
    captureConfig.mainScreenRect.bottom_ = GetData<float>();
    RSSurfaceCaptureBlurParam blurParam;

    // test
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    RSRenderServiceConnectionProxy rsRenderServiceConnectionProxy(remoteObject);
    rsRenderServiceConnectionProxy.TakeSurfaceCapture(nodeId, callback, captureConfig, blurParam);
    return true;
}

bool DoBehindWindowFilterEnabled(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // get data
    bool enabled = GetData<bool>();
    bool res = GetData<bool>();

    // test
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    RSRenderServiceConnectionProxy rsRenderServiceConnectionProxy(remoteObject);
    rsRenderServiceConnectionProxy.SetBehindWindowFilterEnabled(enabled);
    rsRenderServiceConnectionProxy.GetBehindWindowFilterEnabled(res);
    return true;
}

bool DoSetVirtualScreenAutoRotation(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // get data
    ScreenId screenId = GetData<ScreenId>();
    bool isAutoRotation = GetData<bool>();

    // test
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    RSRenderServiceConnectionProxy rsRenderServiceConnectionProxy(remoteObject);
    rsRenderServiceConnectionProxy.SetVirtualScreenAutoRotation(screenId, isAutoRotation);
    return true;
}

bool DoSetVirtualScreenBlackList(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // get data
    ScreenId screenId = GetData<ScreenId>();
    std::vector<NodeId> blackList;
    uint8_t listSize = GetData<uint8_t>();
    for (uint8_t i = 0; i < listSize; ++i) {
        NodeId nodeId = GetData<NodeId>();
        blackList.push_back(nodeId);
    }

    // test
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    RSRenderServiceConnectionProxy rsRenderServiceConnectionProxy(remoteObject);
    rsRenderServiceConnectionProxy.SetVirtualScreenBlackList(screenId, blackList);
    return true;
}

bool DoAddVirtualScreenBlackList(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // get data
    ScreenId screenId = GetData<ScreenId>();
    std::vector<NodeId> blackList;
    uint8_t listSize = GetData<uint8_t>();
    for (uint8_t i = 0; i < listSize; ++i) {
        NodeId nodeId = GetData<NodeId>();
        blackList.push_back(nodeId);
    }

    // test
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    RSRenderServiceConnectionProxy rsRenderServiceConnectionProxy(remoteObject);
    int32_t repCode = 0;
    rsRenderServiceConnectionProxy.AddVirtualScreenBlackList(screenId, blackList, repCode);
    return true;
}

bool DoRemoveVirtualScreenBlackList(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // get data
    ScreenId screenId = GetData<ScreenId>();
    std::vector<NodeId> blackList;
    uint8_t listSize = GetData<uint8_t>();
    for (uint8_t i = 0; i < listSize; ++i) {
        NodeId nodeId = GetData<NodeId>();
        blackList.push_back(nodeId);
    }

    // test
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    RSRenderServiceConnectionProxy rsRenderServiceConnectionProxy(remoteObject);
    int32_t repCode = 0;
    rsRenderServiceConnectionProxy.RemoveVirtualScreenBlackList(screenId, blackList, repCode);
    return true;
}

bool DoResizeVirtualScreen(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // get data
    ScreenId screenId = GetData<ScreenId>();
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();

    // test
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    RSRenderServiceConnectionProxy rsRenderServiceConnectionProxy(remoteObject);
    rsRenderServiceConnectionProxy.ResizeVirtualScreen(screenId, width, height);
    return true;
}

bool DoProfilerServiceFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // get data
    HrpServiceDir baseDirType = HrpServiceDir::HRP_SERVICE_DIR_UNKNOWN;

    std::string subDir = GetData<std::string>();
    std::string subDir2 = GetData<std::string>();
    std::string fileName = GetData<std::string>();
    int32_t flags = GetData<int32_t>();
    int32_t outFd = GetData<int32_t>();
    HrpServiceDirInfo dirInfo{baseDirType, subDir, subDir2};

    std::vector<HrpServiceFileInfo> outFiles;

    // test
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    RSRenderServiceConnectionProxy rsRenderServiceConnectionProxy(remoteObject);

    rsRenderServiceConnectionProxy.ProfilerServiceOpenFile(dirInfo, fileName, flags, outFd);
    rsRenderServiceConnectionProxy.ProfilerServicePopulateFiles(dirInfo, 0, outFiles);
    rsRenderServiceConnectionProxy.ProfilerIsSecureScreen();
    return true;
}

bool DoClearUifirstCache(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // get data
    NodeId nodeId = GetData<NodeId>();

    // test
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    RSRenderServiceConnectionProxy rsRenderServiceConnectionProxy(remoteObject);
    rsRenderServiceConnectionProxy.ClearUifirstCache(nodeId);
    return true;
}

bool DoGetScreenHDRStatus(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    static std::vector<HdrStatus> statusVec = { HdrStatus::NO_HDR, HdrStatus::HDR_PHOTO, HdrStatus::HDR_VIDEO,
        HdrStatus::AI_HDR_VIDEO, HdrStatus::HDR_EFFECT };

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // get data
    ScreenId screenId = GetData<ScreenId>();
    HdrStatus hdrStatus = statusVec[GetData<uint8_t>() % statusVec.size()];
    int32_t resCode = GetData<int32_t>();

    // test
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    RSRenderServiceConnectionProxy rsRenderServiceConnectionProxy(remoteObject);
    rsRenderServiceConnectionProxy.GetScreenHDRStatus(screenId, hdrStatus, resCode);
    return true;
}

bool DoSetScreenFreezeImmediately(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    // get data
    uint64_t nodeId = GetData<uint64_t>();
    bool isFreeze = GetData<bool>();
    sptr<RSISurfaceCaptureCallback> callback;
    RSSurfaceCaptureConfig captureConfig;

    // test
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    RSRenderServiceConnectionProxy rsRenderServiceConnectionProxy(remoteObject);
    rsRenderServiceConnectionProxy.SetScreenFreezeImmediately(nodeId, isFreeze, callback, captureConfig);
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoSomethingInterestingWithMyAPI(data, size);
#ifdef TP_FEATURE_ENABLE
    OHOS::Rosen::DoSetTpFeatureConfigFuzzTest(data, size);
#endif
#ifdef RS_ENABLE_OVERLAY_DISPLAY
    OHOS::Rosen::DoSetOverlayDisplayModeFuzzTest(data, size);
#endif
    OHOS::Rosen::DoTakeSurfaceCapture(data, size);
    OHOS::Rosen::DoBehindWindowFilterEnabled(data, size);
    OHOS::Rosen::DoSetVirtualScreenBlackList(data, size);
    OHOS::Rosen::DoAddVirtualScreenBlackList(data, size);
    OHOS::Rosen::DoRemoveVirtualScreenBlackList(data, size);
    OHOS::Rosen::DoResizeVirtualScreen(data, size);
    OHOS::Rosen::DoSetVirtualScreenAutoRotation(data, size);
    OHOS::Rosen::DoProfilerServiceFuzzTest(data, size);
    OHOS::Rosen::DoClearUifirstCache(data, size);
    OHOS::Rosen::DoGetScreenHDRStatus(data, size);
    OHOS::Rosen::DoSetScreenFreezeImmediately(data, size);
    return 0;
}
