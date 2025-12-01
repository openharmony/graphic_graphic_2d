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
#include "platform/ohos/rs_client_to_service_connection_proxy.h"
#include "platform/ohos/rs_client_to_render_connection_proxy.h"
#include "ipc_callbacks/rs_frame_rate_linker_expected_fps_update_callback_proxy.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* g_data = nullptr;
const std::string XCOMPONENT_ID = "xcomponentId";
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
    int32_t sourceType = GetData<int32_t>();
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
    sptr<RSIScreenSwitchingNotifyCallback> switchingNotifyCallback;
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
    switchingNotifyCallback = iface_cast<RSIScreenSwitchingNotifyCallback>(remoteObject);
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
    RSClientToServiceConnectionProxy rsClientToServiceConnectionProxy(remoteObject);
    RSClientToRenderConnectionProxy rsClientToRenderConnectionProxy(remoteObject);
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

    rsClientToServiceConnectionProxy.CommitTransaction(transactionData);
    rsClientToServiceConnectionProxy.ExecuteSynchronousTask(task);
    rsClientToServiceConnectionProxy.GetMemoryGraphic(pid, memoryGraphic);
    rsClientToServiceConnectionProxy.GetMemoryGraphics(memoryGraphics);
    rsClientToServiceConnectionProxy.GetTotalAppMemSize(cpuMemSize, gpuMemSize);
    rsClientToServiceConnectionProxy.GetUniRenderEnabled(getUniRenderEnable);
    rsClientToServiceConnectionProxy.CreateNode(config, createNodeSuccess);
    sptr<Surface> sface = nullptr;
    rsClientToServiceConnectionProxy.CreateNodeAndSurface(config, sface);
    sptr<IVSyncConnection> conn = nullptr;
    VSyncConnParam vsyncConnParam = {id1, windowNodeId, false};
    bool enable = GetData<bool>();
    uint32_t powerStatus = GetData<uint32_t>();
    int32_t backlightlevel = GetData<int32_t>();
    uint64_t screenId = GetData<uint64_t>();
    RSScreenModeInfo modeInfo;
    int32_t resCode;

    rsClientToServiceConnectionProxy.CreateVSyncConnection(conn, name, token, vsyncConnParam);
    std::shared_ptr<Media::PixelMap> pixelMap = nullptr;
    rsClientToServiceConnectionProxy.CreatePixelMapFromSurface(surface, srcRect, pixelMap);
    rsClientToRenderConnectionProxy.SetFocusAppInfo(appInfo, repCode);
    rsClientToServiceConnectionProxy.GetDefaultScreenId(screenId);
    rsClientToServiceConnectionProxy.GetActiveScreenId(screenId);
    rsClientToServiceConnectionProxy.GetAllScreenIds();
    rsClientToServiceConnectionProxy.CreateVirtualScreen(name, width, height, surface);
    rsClientToServiceConnectionProxy.SetVirtualScreenSurface(id1, surface);
    rsClientToServiceConnectionProxy.RemoveVirtualScreen(id1);
    rsClientToServiceConnectionProxy.SetScreenChangeCallback(callback);
    rsClientToServiceConnectionProxy.SetScreenSwitchingNotifyCallback(switchingNotifyCallback);
    rsClientToServiceConnectionProxy.SetScreenActiveMode(id1, width);
    rsClientToServiceConnectionProxy.SetScreenActiveRect(id1, activeRect, retureCode);
    rsClientToServiceConnectionProxy.SetScreenRefreshRate(id1, pid1, uid);
    rsClientToServiceConnectionProxy.SetRefreshRateMode(pid1);
    rsClientToServiceConnectionProxy.SyncFrameRateRange(id1, range, 0);
    rsClientToServiceConnectionProxy.GetScreenCurrentRefreshRate(id1);
    rsClientToServiceConnectionProxy.GetCurrentRefreshRateMode();
    rsClientToServiceConnectionProxy.GetScreenSupportedRefreshRates(id1);
    rsClientToServiceConnectionProxy.GetShowRefreshRateEnabled(enable);
    rsClientToServiceConnectionProxy.SetShowRefreshRateEnabled(true, 0);
    rsClientToServiceConnectionProxy.GetRealtimeRefreshRate(id1);
    rsClientToServiceConnectionProxy.GetRefreshInfo(id1, getRefreshInfoEnable);
    rsClientToServiceConnectionProxy.GetRefreshInfoToSP(id1, getRefreshInfoToSPEnable);
    rsClientToServiceConnectionProxy.SetPhysicalScreenResolution(id1, width, height);
    rsClientToServiceConnectionProxy.SetVirtualScreenResolution(id1, width, height);
    rsClientToServiceConnectionProxy.SetRogScreenResolution(id1, width, height);
    rsClientToServiceConnectionProxy.SetScreenPowerStatus(id1, status);
    rsClientToServiceConnectionProxy.RegisterApplicationAgent(width, app);
    rsClientToServiceConnectionProxy.GetVirtualScreenResolution(id1);
    rsClientToServiceConnectionProxy.GetScreenActiveMode(id1, modeInfo);
    rsClientToServiceConnectionProxy.GetScreenSupportedModes(id1);
    rsClientToServiceConnectionProxy.GetScreenCapability(id1);
    rsClientToServiceConnectionProxy.GetScreenPowerStatus(id1, powerStatus);
    rsClientToServiceConnectionProxy.GetScreenData(id1);
    rsClientToServiceConnectionProxy.GetScreenBacklight(id1, backlightlevel);
    rsClientToServiceConnectionProxy.SetScreenBacklight(id1, width);
    rsClientToServiceConnectionProxy.RegisterBufferAvailableListener(id1, callback2, true);
    rsClientToServiceConnectionProxy.RegisterBufferClearListener(id1, callback3);
    rsClientToServiceConnectionProxy.GetScreenSupportedColorGamuts(id1, mode);
    rsClientToServiceConnectionProxy.GetScreenSupportedMetaDataKeys(id1, keys);
    rsClientToServiceConnectionProxy.GetScreenColorGamut(id1, screenColorGamut);
    rsClientToServiceConnectionProxy.SetScreenColorGamut(id1, uid);
    rsClientToServiceConnectionProxy.SetScreenGamutMap(id1, screenGamutMap);
    rsClientToServiceConnectionProxy.GetScreenHDRCapability(id1, screenHdrCapability);
    rsClientToServiceConnectionProxy.GetPixelFormat(id1, pixelFormat, resCode);
    rsClientToServiceConnectionProxy.SetPixelFormat(id1, pixelFormat, resCode);
    rsClientToServiceConnectionProxy.GetScreenSupportedHDRFormats(id1, hdrFormats, resCode);
    rsClientToServiceConnectionProxy.GetScreenHDRFormat(id1, screenHDRFormat, resCode);
    rsClientToServiceConnectionProxy.SetScreenHDRFormat(id1, pid1, resCode);
    rsClientToServiceConnectionProxy.GetScreenSupportedColorSpaces(id1, colorSpaces, resCode);
    rsClientToServiceConnectionProxy.GetScreenColorSpace(id1, colorSpace, resCode);
    rsClientToServiceConnectionProxy.SetScreenColorSpace(id1, colorSpace, resCode);
    rsClientToServiceConnectionProxy.GetScreenType(id1, screenType);
    rsClientToServiceConnectionProxy.GetBitmap(id1, bitmap, getBitmapSuccess);
    rsClientToServiceConnectionProxy.GetPixelmap(id1, pixelmap, &rect, drawCmdList, getPixelmapSuccess);
    rsClientToServiceConnectionProxy.RegisterTypeface(id1, typeface);
    rsClientToServiceConnectionProxy.RegisterTypeface(id1, id2, screenId, resCode, id1);
    rsClientToServiceConnectionProxy.UnRegisterTypeface(id1);
    rsClientToServiceConnectionProxy.SetScreenSkipFrameInterval(id1, width, resCode);
    rsClientToServiceConnectionProxy.RegisterOcclusionChangeCallback(rsIOcclusionChangeCallback, repCode);
    rsClientToServiceConnectionProxy.RegisterSurfaceOcclusionChangeCallback(id1, callbackTwo, partitionPoints);
    rsClientToServiceConnectionProxy.UnRegisterSurfaceOcclusionChangeCallback(id1);
    rsClientToServiceConnectionProxy.RegisterHgmConfigChangeCallback(rsIHgmConfigChangeCallback);
    rsClientToServiceConnectionProxy.RegisterHgmRefreshRateModeChangeCallback(rsIHgmConfigChangeCallback);
    rsClientToServiceConnectionProxy.RegisterHgmRefreshRateUpdateCallback(rsIHgmConfigChangeCallback);
    rsClientToServiceConnectionProxy.RegisterFirstFrameCommitCallback(rsIFirstFrameCommitCallback);
    rsClientToServiceConnectionProxy.SetAppWindowNum(width);
    rsClientToServiceConnectionProxy.SetSystemAnimatedScenes(systemAnimatedScenes, false, success);
    rsClientToServiceConnectionProxy.ShowWatermark(watermarkImg, true);
    rsClientToServiceConnectionProxy.ResizeVirtualScreen(id1, width, height);
    rsClientToServiceConnectionProxy.ReportJankStats();
    rsClientToServiceConnectionProxy.NotifyLightFactorStatus(1);
    rsClientToServiceConnectionProxy.NotifyPackageEvent(width, packageList);
    rsClientToServiceConnectionProxy.NotifyRefreshRateEvent(eventInfo);
    rsClientToServiceConnectionProxy.NotifySoftVsyncRateDiscountEvent(1, name, 1);
    rsClientToServiceConnectionProxy.NotifyTouchEvent(pid1, uid, sourceType);
    rsClientToServiceConnectionProxy.NotifyDynamicModeEvent(true);
    rsClientToServiceConnectionProxy.NotifyHgmConfigEvent(name, true);
    rsClientToServiceConnectionProxy.NotifyXComponentExpectedFrameRate(name, expectedFrameRate);
    rsClientToServiceConnectionProxy.ReportEventResponse(info);
    rsClientToServiceConnectionProxy.ReportEventComplete(info);
    rsClientToServiceConnectionProxy.ReportEventJankFrame(info);
    rsClientToServiceConnectionProxy.ReportGameStateData(gameStateDataInfo);
    rsClientToServiceConnectionProxy.SetHardwareEnabled(id1, true, SelfDrawingNodeType::DEFAULT, true);
    rsClientToServiceConnectionProxy.SetCacheEnabledForRotation(true);
    rsClientToServiceConnectionProxy.SetOnRemoteDiedCallback(onRemoteDiedCallback);
    rsClientToServiceConnectionProxy.RunOnRemoteDiedCallback();
    rsClientToServiceConnectionProxy.GetActiveDirtyRegionInfo();
    rsClientToServiceConnectionProxy.GetGlobalDirtyRegionInfo();
    rsClientToServiceConnectionProxy.GetLayerComposeInfo();
    rsClientToServiceConnectionProxy.GetHwcDisabledReasonInfo();
    rsClientToServiceConnectionProxy.GetHdrOnDuration(id2);
    rsClientToServiceConnectionProxy.SetVmaCacheStatus(true);
    rsClientToServiceConnectionProxy.SetVmaCacheStatus(false);
    rsClientToServiceConnectionProxy.SetVirtualScreenUsingStatus(true);
    rsClientToServiceConnectionProxy.SetVirtualScreenUsingStatus(false);
    rsClientToServiceConnectionProxy.SetCurtainScreenUsingStatus(true);
    rsClientToServiceConnectionProxy.FillParcelWithTransactionData(transactionData, parcel);
    rsClientToServiceConnectionProxy.ReportDataBaseRs(messageParcel, reply, option, info);
    rsClientToServiceConnectionProxy.ReportGameStateDataRs(messageParcel, reply, option, gameStateDataInfo);
    rsClientToServiceConnectionProxy.SetFreeMultiWindowStatus(true);
    rsClientToServiceConnectionProxy.SetFreeMultiWindowStatus(false);
    rsClientToServiceConnectionProxy.RegisterFrameRateLinkerExpectedFpsUpdateCallback(pid1,
        rsIFrameRateLinkerExpectedFpsUpdateCallback);
    rsFrameRateLinkerExpectedFpsUpdateCallbackProxy.OnFrameRateLinkerExpectedFpsUpdate(pid, XCOMPONENT_ID, expectedFps);
    return true;
}

#ifdef TP_FEATURE_ENABLE
bool OHOS::Rosen::DoSetTpFeatureConfigFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // get data
    int32_t tpFeature = GetData<int32_t>();
    std::string tpConfig = GetData<std::string>();
    auto tpFeatureConfigType = static_cast<TpFeatureConfigType>(GetData<uint8_t>());

    // test
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    RSClientToServiceConnectionProxy rsClientToServiceConnectionProxy(remoteObject);
    RSClientToServiceConnectionProxy.SetTpFeatureConfig(tpFeature, tpConfig.c_str(), tpFeatureConfigType);
    return true;
}
#endif

#ifdef RS_ENABLE_OVERLAY_DISPLAY
bool DoSetOverlayDisplayModeFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // get data
    int32_t mode = GetData<int32_t>();

    // test
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    RSClientToServiceConnectionProxy rsClientToServiceConnectionProxy(remoteObject);
    rsClientToServiceConnectionProxy.SetOverlayDisplayMode(mode);
    return true;
}
#endif

bool DoTakeSurfaceCapture(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

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
    RSClientToRenderConnectionProxy rsClientToRenderConnectionProxy(remoteObject);
    rsClientToRenderConnectionProxy.TakeSurfaceCapture(nodeId, callback, captureConfig, blurParam);
    return true;
}

bool DoBehindWindowFilterEnabled(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // get data
    bool enabled = GetData<bool>();
    bool res = GetData<bool>();

    // test
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    RSClientToServiceConnectionProxy rsClientToServiceConnectionProxy(remoteObject);
    rsClientToServiceConnectionProxy.SetBehindWindowFilterEnabled(enabled);
    rsClientToServiceConnectionProxy.GetBehindWindowFilterEnabled(res);
    return true;
}

bool DoSetVirtualScreenAutoRotation(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // get data
    ScreenId screenId = GetData<ScreenId>();
    bool isAutoRotation = GetData<bool>();

    // test
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    RSClientToServiceConnectionProxy rsClientToServiceConnectionProxy(remoteObject);
    rsClientToServiceConnectionProxy.SetVirtualScreenAutoRotation(screenId, isAutoRotation);
    return true;
}

bool DoSetVirtualScreenBlackList(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

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
    RSClientToServiceConnectionProxy rsClientToServiceConnectionProxy(remoteObject);
    rsClientToServiceConnectionProxy.SetVirtualScreenBlackList(screenId, blackList);
    return true;
}

bool DoAddVirtualScreenBlackList(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

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
    RSClientToServiceConnectionProxy rsClientToServiceConnectionProxy(remoteObject);
    int32_t repCode = 0;
    rsClientToServiceConnectionProxy.AddVirtualScreenBlackList(screenId, blackList, repCode);
    return true;
}

bool DoRemoveVirtualScreenBlackList(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

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
    RSClientToServiceConnectionProxy rsClientToServiceConnectionProxy(remoteObject);
    int32_t repCode = 0;
    rsClientToServiceConnectionProxy.RemoveVirtualScreenBlackList(screenId, blackList, repCode);
    return true;
}

bool DoResizeVirtualScreen(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // get data
    ScreenId screenId = GetData<ScreenId>();
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();

    // test
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    RSClientToServiceConnectionProxy rsClientToServiceConnectionProxy(remoteObject);
    rsClientToServiceConnectionProxy.ResizeVirtualScreen(screenId, width, height);
    return true;
}

bool DoProfilerServiceFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

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
    RSClientToServiceConnectionProxy rsClientToServiceConnectionProxy(remoteObject);

    rsClientToServiceConnectionProxy.ProfilerServiceOpenFile(dirInfo, fileName, flags, outFd);
    rsClientToServiceConnectionProxy.ProfilerServicePopulateFiles(dirInfo, 0, outFiles);
    rsClientToServiceConnectionProxy.ProfilerIsSecureScreen();
    return true;
}

bool DoClearUifirstCache(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // get data
    NodeId nodeId = GetData<NodeId>();

    // test
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    RSClientToRenderConnectionProxy rsClientToRenderConnectionProxy(remoteObject);
    rsClientToRenderConnectionProxy.ClearUifirstCache(nodeId);
    return true;
}

bool DoGetScreenHDRStatus(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    static std::vector<HdrStatus> statusVec = { HdrStatus::NO_HDR, HdrStatus::HDR_PHOTO, HdrStatus::HDR_VIDEO,
        HdrStatus::AI_HDR_VIDEO_GTM, HdrStatus::HDR_EFFECT };

    // get data
    ScreenId screenId = GetData<ScreenId>();
    HdrStatus hdrStatus = statusVec[GetData<uint8_t>() % statusVec.size()];
    int32_t resCode = GetData<int32_t>();

    // test
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    RSClientToRenderConnectionProxy rsClientToRenderConnectionProxy(remoteObject);
    rsClientToRenderConnectionProxy.GetScreenHDRStatus(screenId, hdrStatus, resCode);
    return true;
}

bool DoTakeSurfaceCaptureWithAllWindows(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // get data
    uint64_t nodeId = GetData<uint64_t>();
    bool checkDrmAndSurfaceLock = GetData<bool>();
    sptr<RSISurfaceCaptureCallback> callback;
    RSSurfaceCaptureConfig captureConfig;

    // test
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    RSClientToRenderConnectionProxy rsClientToRenderConnectionProxy(remoteObject);
    rsClientToRenderConnectionProxy.TakeSurfaceCaptureWithAllWindows(
        nodeId, callback, captureConfig, checkDrmAndSurfaceLock);
    return true;
}

bool DoFreezeScreen(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // get data
    uint64_t nodeId = GetData<uint64_t>();
    bool isFreeze = GetData<bool>();

    // test
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    RSClientToRenderConnectionProxy rsClientToRenderConnectionProxy(remoteObject);
    rsClientToRenderConnectionProxy.FreezeScreen(nodeId, isFreeze);
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    // initialize
    OHOS::Rosen::g_data = data;
    OHOS::Rosen::g_size = size;
    OHOS::Rosen::g_pos = 0;

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
    OHOS::Rosen::DoTakeSurfaceCaptureWithAllWindows(data, size);
    OHOS::Rosen::DoFreezeScreen(data, size);
    return 0;
}
