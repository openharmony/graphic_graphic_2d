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

#include "rsrenderserviceclient008_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "transaction/rs_render_service_client.h"
#include "command/rs_command.h"
#include "command/rs_node_showing_command.h"
#include "core/transaction/rs_interfaces.h"
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
#include "ipc_callbacks/pointer_render/pointer_luminance_callback_stub.h"
#endif
#include "ipc_callbacks/screen_change_callback_stub.h"
#include "platform/ohos/rs_render_service_connect_hub.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t DO_GET_UNI_RENDER_ENABLED_002 = 0;
const uint8_t DO_CREATE_NODE_002 = 1;
const uint8_t DO_GET_TOTAL_APP_MEM_SIZE_002 = 2;
const uint8_t DO_TAKE_SURFACE_CAPTURE_002 = 3;
const uint8_t DO_SET_HWCNODE_BOUNDS_002 = 4;
const uint8_t DO_SET_FOCUS_APP_INFO_002 = 5;
const uint8_t DO_SET_VIRTUAL_SCREEN_BLACK_LIST_002 = 6;
const uint8_t DO_SET_CAST_SCREEN_ENABLE_SKIP_WINDOW_002 = 7;
const uint8_t DO_SYNC_FRAME_RATE_RANGE_002 = 8;
const uint8_t DO_DISABLE_PWOER_OFF_RENDER_CONTROL_002 = 9;
const uint8_t DO_GET_SCREEN_SUPPORTED_COLOR_GAMUTS_002 = 10;
const uint8_t DO_SET_GLOBAL_DARK_COLOR_MODE_002 = 11;
const uint8_t DO_GET_SCREEN_TYPE_002 = 12;
const uint8_t DO_REGISTER_OCCLUSION_CHANGE_CALLBACK_002 = 13;
const uint8_t DO_SET_HIDE_PRIVACY_CONTENT_002 = 14;
const uint8_t DO_SET_BEHIND_WINDOW_FILTER_ENABLED_002 = 15;
const uint8_t DO_GET_BEHIND_WINDOW_FILTER_ENABLED_002 = 16;
const uint8_t DO_ADD_VIRTUAL_SCREEN_BLACK_LIST_002 = 17;
const uint8_t DO_REMOVE_VIRTUAL_SCREEN_BLACK_LIST_002 = 18;
const uint8_t TARGET_SIZE = 19;

sptr<RSIRenderServiceConnection> CONN = nullptr;
const uint8_t* DATA = nullptr;
size_t g_size = 0;
size_t g_pos;
constexpr size_t STR_LEN = 10;

template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (DATA == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, DATA + g_pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_pos += objectSize;
    return object;
}

/*
 * get a string from g_data
 */
std::string GetStringFromData(int strlen)
{
    if (strlen <= 0) {
        return "fuzz";
    }
    char cstr[strlen];
    cstr[strlen - 1] = '\0';
    for (int i = 0; i < strlen - 1; i++) {
        char tmp = GetData<char>();
        if (tmp == '\0') {
            tmp = '1';
        }
        cstr[i] = tmp;
    }
    std::string str(cstr);
    return str;
}

bool Init(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    DATA = data;
    g_size = size;
    g_pos = 0;
    return true;
}
} // namespace

class DerivedSyncTask : public RSSyncTask {
public:
    using RSSyncTask::RSSyncTask;
    bool CheckHeader(Parcel& parcel) const override
    {
        return true;
    }
    bool ReadFromParcel(Parcel& parcel) override
    {
        return true;
    }
    bool Marshalling(Parcel& parcel) const override
    {
        return true;
    }
    void Process(RSContext& context) override {}
};

bool DoGetUniRenderEnabled002()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    int pid = GetData<int>();
    RSRenderServiceConnectHub::GetInstance()->Destroy();
    client->GetUniRenderEnabled();
    client->GetMemoryGraphic(pid);
    client->GetMemoryGraphics();
    return true;
}

bool DoCreateNode002()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    RSDisplayNodeConfig displayNodeconfig = {
        .screenId = GetData<uint64_t>(),
        .isMirrored = GetData<bool>(),
        .mirrorNodeId = GetData<NodeId>(),
        .isSync = GetData<bool>(),
    };
    NodeId nodeId = GetData<NodeId>();
    client->CreateNode(displayNodeconfig, nodeId);

    RSSurfaceRenderNodeConfig config = { .id = GetData<NodeId>(), .name = "fuzztest" };
    client->CreateNode(config);
    return true;
}

bool DoGetTotalAppMemSize002()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    float cpuMemSize = GetData<int>();
    float gpuMemSize  = GetData<int>();
    client->GetTotalAppMemSize(cpuMemSize, gpuMemSize);
    std::string name = GetStringFromData(STR_LEN);
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> looper;
    uint64_t id = GetData<uint64_t>();
    NodeId windowNodeId = GetData<NodeId>();
    client->CreateVSyncReceiver(name, looper, id, windowNodeId);
    uint64_t surfaceId = GetData<uint64_t>();
    Rect srcRect = {0, 0, 100, 100};
    client->CreatePixelMapFromSurfaceId(surfaceId, srcRect);
    return true;
}

bool DoTakeSurfaceCapture002()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    uint64_t nodeId = GetData<uint64_t>();
    std::shared_ptr<SurfaceCaptureCallback> callback;
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = GetData<float>();
    captureConfig.scaleY = GetData<float>();
    captureConfig.useDma = GetData<bool>();
    captureConfig.useCurWindow = GetData<bool>();
    uint8_t type = GetData<uint8_t>();
    captureConfig.captureType = (SurfaceCaptureType)type;
    captureConfig.isSync = GetData<bool>();

    RSRenderServiceConnectHub::GetInstance()->Destroy();
    client->TakeSurfaceCapture(nodeId, callback, captureConfig);
    return true;
}

bool DoSetHwcNodeBounds002()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    int64_t rsNodeId = GetData<int64_t>();
    float positionX = GetData<float>();
    float positionY = GetData<float>();
    float positionZ = GetData<float>();
    float positionW = GetData<float>();
    RSRenderServiceConnectHub::GetInstance()->Destroy();
    client->SetHwcNodeBounds(rsNodeId, positionX, positionY, positionZ, positionW);
    return true;
}

bool DoSetFocusAppInfo002()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    int32_t pid = GetData<int32_t>();
    int32_t uid = GetData<int32_t>();
    std::string bundleName = GetStringFromData(STR_LEN);
    std::string abilityName = GetStringFromData(STR_LEN);
    uint64_t focusNodeId = GetData<uint64_t>();

    RSRenderServiceConnectHub::GetInstance()->Destroy();
    FocusAppInfo info = {
        .pid = pid,
        .uid = uid,
        .bundleName = bundleName,
        .abilityName = abilityName,
        .focusNodeId = focusNodeId};
    client->SetFocusAppInfo(info);
    client->GetDefaultScreenId();
    client->GetActiveScreenId();
    client->GetAllScreenIds();
    client->GetAllScreenIds();
    std::string name = "name";
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("DisplayNode");
    sptr<IBufferProducer> bp = cSurface->GetProducer();
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(bp);
    ScreenId mirrorId = GetData<ScreenId>();
    int32_t flags = GetData<int32_t>();
    std::vector<NodeId> whiteList;
    client->CreateVirtualScreen(name, width, height, pSurface, mirrorId, flags, whiteList);
    return true;
}

bool DoSetVirtualScreenBlackList002()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    std::vector<NodeId> blackListVector;
    RSRenderServiceConnectHub::GetInstance()->Destroy();
    client->SetVirtualScreenBlackList(id, blackListVector);
    std::string name = GetStringFromData(STR_LEN);
    Media::InitializationOptions opts;
    opts.size.width = GetData<int32_t>();
    opts.size.height = GetData<int32_t>();
    opts.srcPixelFormat = static_cast<Media::PixelFormat>(GetData<int32_t>());
    opts.pixelFormat = static_cast<Media::PixelFormat>(GetData<int32_t>());
    opts.alphaType = static_cast<Media::AlphaType>(GetData<int32_t>());
    opts.scaleMode = static_cast<Media::ScaleMode>(GetData<int32_t>());
    opts.editable = GetData<bool>();
    opts.useSourceIfMatch = GetData<bool>();
    std::shared_ptr<Media::PixelMap> watermark = Media::PixelMap::Create(opts);
    client->SetWatermark(name, watermark);
    std::vector<NodeId> securityExemptionList;
    client->SetVirtualScreenSecurityExemptionList(id, securityExemptionList);
    return true;
}

bool DoSetCastScreenEnableSkipWindow002()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    RSRenderServiceConnectHub::GetInstance()->Destroy();
    ScreenId id = GetData<ScreenId>();
    bool enable = GetData<bool>();
    int32_t refreshRateMode = GetData<int32_t>();
    sptr<Surface> surface;
    ScreenChangeCallback cb;
    uint32_t modeId = GetData<uint32_t>();
    int32_t sceneId = GetData<int32_t>();
    int32_t rate = GetData<int32_t>();

    client->SetCastScreenEnableSkipWindow(id, enable);
    client->SetVirtualScreenSurface(id, surface);
    client->RemoveVirtualScreen(id);
    client->SetScreenChangeCallback(cb);
    client->SetScreenActiveMode(id, modeId);
    client->SetScreenRefreshRate(id, sceneId, rate);
    client->SetRefreshRateMode(refreshRateMode);
    return true;
}

bool DoSyncFrameRateRange002()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    FrameRateLinkerId id = GetData<FrameRateLinkerId>();
    FrameRateRange range;
    int32_t animatorExpectedFrameRate = GetData<int32_t>();
    ScreenId screenId = GetData<ScreenId>();
    bool enabled = GetData<bool>();
    int32_t type = GetData<int32_t>();
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    RSRenderServiceConnectHub::GetInstance()->Destroy();

    client->SyncFrameRateRange(id, range, animatorExpectedFrameRate);
    client->UnregisterFrameRateLinker(id);
    client->GetScreenCurrentRefreshRate(screenId);
    client->GetCurrentRefreshRateMode();
    client->GetScreenSupportedRefreshRates(screenId);
    client->GetShowRefreshRateEnabled();
    client->SetShowRefreshRateEnabled(enabled, type);
    client->GetRealtimeRefreshRate(screenId);
    client->SetVirtualScreenResolution(screenId, width, height);
    client->GetVirtualScreenResolution(screenId);
    client->MarkPowerOffNeedProcessOneFrame();
    return true;
}

bool DoDisablePowerOffRenderControl002()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId screenId = GetData<ScreenId>();
    ScreenPowerStatus status = ScreenPowerStatus::INVALID_POWER_STATUS;
    NodeId id = GetData<ScreenId>();
    BufferAvailableCallback callback;
    BufferClearCallback bufferClearCallback;
    bool isFromRenderThread = GetData<bool>();
    RSRenderServiceConnectHub::GetInstance()->Destroy();

    client->DisablePowerOffRenderControl(screenId);
    client->SetScreenPowerStatus(screenId, status);
    client->GetScreenActiveMode(screenId);
    client->GetScreenSupportedModes(screenId);
    client->GetScreenCapability(screenId);
    client->GetScreenPowerStatus(screenId);
    client->GetScreenData(screenId);
    client->GetScreenBacklight(screenId);
    client->RegisterBufferAvailableListener(id, callback, isFromRenderThread);
    client->RegisterBufferClearListener(id, bufferClearCallback);
    return true;
}

bool DoGetScreenSupportedColorGamuts002()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    std::vector<ScreenColorGamut> mode1;
    std::vector<ScreenHDRMetadataKey> keys;
    ScreenColorGamut screenColorGamut = ScreenColorGamut::COLOR_GAMUT_INVALID;
    int32_t modeIdx = GetData<int32_t>();
    ScreenGamutMap screenGamutMap = ScreenGamutMap::GAMUT_MAP_CONSTANT;
    ScreenRotation screenRotation = ScreenRotation::ROTATION_0;
    bool canvasRotation = GetData<bool>();
    ScreenScaleMode scaleMode = ScreenScaleMode::FILL_MODE;
    RSRenderServiceConnectHub::GetInstance()->Destroy();

    client->GetScreenSupportedColorGamuts(id, mode1);
    client->GetScreenSupportedMetaDataKeys(id, keys);
    client->SetScreenColorGamut(id, modeIdx);
    client->GetScreenColorGamut(id, screenColorGamut);
    client->SetScreenGamutMap(id, screenGamutMap);
    client->GetScreenGamutMap(id, screenGamutMap);
    client->SetScreenCorrection(id, screenRotation);
    client->SetVirtualMirrorScreenCanvasRotation(id, canvasRotation);
    client->SetVirtualMirrorScreenScaleMode(id, scaleMode);
    return true;
}

bool DoSetGlobalDarkColorMode002()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    bool isDark = GetData<bool>();
    ScreenId screenId = GetData<ScreenId>();
    RSScreenHDRCapability screenHdrCapability;
    GraphicPixelFormat pixelFormat = GRAPHIC_PIXEL_FMT_BGRA_8888;
    std::vector<ScreenHDRFormat> hdrFormats;
    ScreenHDRFormat hdrFormat = ScreenHDRFormat::VIDEO_HLG;
    int32_t modeIdx = GetData<int32_t>();
    std::vector<GraphicCM_ColorSpaceType> colorSpaces;
    GraphicCM_ColorSpaceType colorSpace = GraphicCM_ColorSpaceType::GRAPHIC_CM_BT601_EBU_FULL;
    RSRenderServiceConnectHub::GetInstance()->Destroy();

    client->SetGlobalDarkColorMode(isDark);
    client->GetScreenHDRCapability(screenId, screenHdrCapability);
    client->SetPixelFormat(screenId, pixelFormat);
    client->GetPixelFormat(screenId, pixelFormat);
    client->SetScreenHDRFormat(screenId, modeIdx);
    client->GetScreenHDRFormat(screenId, hdrFormat);
    client->GetScreenSupportedHDRFormats(screenId, hdrFormats);
    client->SetScreenColorSpace(screenId, colorSpace);
    client->GetScreenColorSpace(screenId, colorSpace);
    client->GetScreenSupportedColorSpaces(screenId, colorSpaces);
    return true;
}

bool DoGetScreenType002()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId screenId = GetData<ScreenId>();
    RSScreenType screenType = RSScreenType::BUILT_IN_TYPE_SCREEN;
    Drawing::Bitmap bm;
    NodeId nodeId = GetData<uint64_t>();
    std::shared_ptr<Media::PixelMap> pixelmap;
    Drawing::Rect *rect;
    std::shared_ptr<Drawing::DrawCmdList> drawCmdList;
    uint32_t skipFrameInterval = GetData<uint32_t>();
    uint32_t maxRefreshRate = GetData<uint32_t>();
    uint32_t actualRefreshRate = GetData<uint32_t>();
    std::shared_ptr<Drawing::Typeface> typeface;
    RSRenderServiceConnectHub::GetInstance()->Destroy();
    client->GetScreenType(screenId, screenType);
    client->GetBitmap(nodeId, bm);
    client->GetPixelmap(nodeId, pixelmap, rect, drawCmdList);
    client->RegisterTypeface(typeface);
    client->UnRegisterTypeface(typeface);
    client->SetScreenSkipFrameInterval(screenId, skipFrameInterval);
    client->SetVirtualScreenRefreshRate(screenId, maxRefreshRate, actualRefreshRate);
    return true;
}

bool DoRegisterOcclusionChangeCallback002()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    OcclusionChangeCallback occlusionChangeCallback;
    SurfaceOcclusionChangeCallback surfaceOcclusionChangeCallback;
    NodeId nodeId = GetData<NodeId>();
    std::vector<float> partitionPoints;
    HgmConfigChangeCallback hgmConfigChangeCallback;
    HgmRefreshRateModeChangeCallback hgmRefreshRateModeChangeCallback;
    HgmRefreshRateUpdateCallback hgmRefreshRateUpdateCallback;
    SystemAnimatedScenes systemAnimatedScenes = SystemAnimatedScenes::ENTER_MISSION_CENTER;
    ScreenId screenId = GetData<ScreenId>();
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    RSRenderServiceConnectHub::GetInstance()->Destroy();

    client->RegisterOcclusionChangeCallback(occlusionChangeCallback);
    client->RegisterSurfaceOcclusionChangeCallback(nodeId, surfaceOcclusionChangeCallback, partitionPoints);
    client->UnRegisterSurfaceOcclusionChangeCallback(nodeId);
    client->RegisterHgmConfigChangeCallback(hgmConfigChangeCallback);
    client->RegisterHgmRefreshRateModeChangeCallback(hgmRefreshRateModeChangeCallback);
    client->RegisterHgmRefreshRateUpdateCallback(hgmRefreshRateUpdateCallback);
    client->SetSystemAnimatedScenes(systemAnimatedScenes, false);
    client->ResizeVirtualScreen(screenId, width, height);
    return true;
}

bool DoSetHidePrivacyContent002()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    OcclusionChangeCallback occlusionChangeCallback;
    SurfaceOcclusionChangeCallback surfaceOcclusionChangeCallback;
    NodeId nodeId = GetData<NodeId>();
    bool needHidePrivacyContent = GetData<bool>();
    bool flag = GetData<bool>();
    bool direct = GetData<bool>();
    UIExtensionCallback uiExtensionCallback;
    uint64_t userId = GetData<uint64_t>();
    RSRenderServiceConnectHub::GetInstance()->Destroy();

    client->SetHidePrivacyContent(nodeId, needHidePrivacyContent);
    client->GetActiveDirtyRegionInfo();
    client->GetGlobalDirtyRegionInfo();
    client->GetLayerComposeInfo();
    client->GetHwcDisabledReasonInfo();
    client->SetVmaCacheStatus(flag);
    client->RegisterUIExtensionCallback(userId, uiExtensionCallback);
    client->SetAncoForceDoDirect(direct);
    return true;
}

bool DoSetBehindWindowFilterEnabled002()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    RSRenderServiceConnectHub::GetInstance()->Destroy();
    bool enabled = GetData<bool>();
    client->SetBehindWindowFilterEnabled(enabled);
    return true;
}

bool DoGetBehindWindowFilterEnabled002()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    RSRenderServiceConnectHub::GetInstance()->Destroy();
    bool enabled = GetData<bool>();
    client->GetBehindWindowFilterEnabled(enabled);
    return true;
}

bool DoAddVirtualScreenBlackList002()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    std::vector<NodeId> blackListVector;
    NodeId nodeId = GetData<ScreenId>();
    blackListVector.push_back(nodeId);
    RSRenderServiceConnectHub::GetInstance()->Destroy();
    client->AddVirtualScreenBlackList(id, blackListVector);
    return true;
}

bool DoRemoveVirtualScreenBlackList002()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    std::vector<NodeId> blackListVector;
    NodeId nodeId = GetData<ScreenId>();
    blackListVector.push_back(nodeId);
    RSRenderServiceConnectHub::GetInstance()->Destroy();
    client->RemoveVirtualScreenBlackList(id, blackListVector);
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    if (!OHOS::Rosen::Init(data, size)) {
        return -1;
    }
    uint8_t tarPos = OHOS::Rosen::GetData<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_GET_UNI_RENDER_ENABLED_002:
            OHOS::Rosen::DoGetUniRenderEnabled002();
            break;
        case OHOS::Rosen::DO_CREATE_NODE_002:
            OHOS::Rosen::DoCreateNode002();
            break;
        case OHOS::Rosen::DO_GET_TOTAL_APP_MEM_SIZE_002:
            OHOS::Rosen::DoGetTotalAppMemSize002();
            break;
        case OHOS::Rosen::DO_TAKE_SURFACE_CAPTURE_002:
            OHOS::Rosen::DoTakeSurfaceCapture002();
            break;
        case OHOS::Rosen::DO_SET_HWCNODE_BOUNDS_002:
            OHOS::Rosen::DoSetHwcNodeBounds002();
            break;
        case OHOS::Rosen::DO_SET_FOCUS_APP_INFO_002:
            OHOS::Rosen::DoSetFocusAppInfo002();
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_SCREEN_BLACK_LIST_002:
            OHOS::Rosen::DoSetVirtualScreenBlackList002();
            break;
        case OHOS::Rosen::DO_SET_CAST_SCREEN_ENABLE_SKIP_WINDOW_002:
            OHOS::Rosen::DoSetCastScreenEnableSkipWindow002();
            break;
        case OHOS::Rosen::DO_SYNC_FRAME_RATE_RANGE_002:
            OHOS::Rosen::DoSyncFrameRateRange002();
            break;
        case OHOS::Rosen::DO_DISABLE_PWOER_OFF_RENDER_CONTROL_002:
            OHOS::Rosen::DoDisablePowerOffRenderControl002();
            break;
        case OHOS::Rosen::DO_GET_SCREEN_SUPPORTED_COLOR_GAMUTS_002:
            OHOS::Rosen::DoGetScreenSupportedColorGamuts002();
            break;
        case OHOS::Rosen::DO_SET_GLOBAL_DARK_COLOR_MODE_002:
            OHOS::Rosen::DoSetGlobalDarkColorMode002();
            break;
        case OHOS::Rosen::DO_GET_SCREEN_TYPE_002:
            OHOS::Rosen::DoGetScreenType002();
            break;
        case OHOS::Rosen::DO_REGISTER_OCCLUSION_CHANGE_CALLBACK_002:
            OHOS::Rosen::DoRegisterOcclusionChangeCallback002();
            break;
        case OHOS::Rosen::DO_SET_HIDE_PRIVACY_CONTENT_002:
            OHOS::Rosen::DoSetHidePrivacyContent002();
            break;
        case OHOS::Rosen::DO_SET_BEHIND_WINDOW_FILTER_ENABLED_002:
            OHOS::Rosen::DoSetBehindWindowFilterEnabled002();
            break;
        case OHOS::Rosen::DO_GET_BEHIND_WINDOW_FILTER_ENABLED_002:
            OHOS::Rosen::DoGetBehindWindowFilterEnabled002();
            break;
        case OHOS::Rosen::DO_ADD_VIRTUAL_SCREEN_BLACK_LIST_002:
            OHOS::Rosen::DoAddVirtualScreenBlackList002();
            break;
        case OHOS::Rosen::DO_REMOVE_VIRTUAL_SCREEN_BLACK_LIST_002:
            OHOS::Rosen::DoRemoveVirtualScreenBlackList002();
            break;
        default:
            return -1;
    }
    return 0;
}