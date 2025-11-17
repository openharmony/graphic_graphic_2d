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

#include "rsrenderserviceclient_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "transaction/rs_render_service_client.h"
#include "transaction/rs_render_pipeline_client.h"
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
constexpr size_t STR_LEN = 10;
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

struct DrawBuffer {
    int32_t w;
    int32_t h;
};

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
bool DoExecuteSynchronousTask(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSIRenderClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    uint64_t timeoutNS = GetData<uint64_t>();
    auto task = std::make_shared<DerivedSyncTask>(timeoutNS);
    renderServiceClient->ExecuteSynchronousTask(task);

    return true;
}

bool DoGetMemoryGraphic(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    int pid = GetData<int>();
    renderServiceClient->GetMemoryGraphic(pid);

    return true;
}

bool DoGetMemoryGraphics()
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    renderServiceClient->GetMemoryGraphics();
    return true;
}

bool DoGetTotalAppMemSize(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    float cpuMemSize = GetData<int>();
    float gpuMemSize  = GetData<int>();
    renderServiceClient->GetTotalAppMemSize(cpuMemSize, gpuMemSize);
    return true;
}

bool DoCreateNode(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    RSDisplayNodeConfig displayNodeconfig = {
        .screenId = GetData<uint64_t>(),
        .isMirrored = GetData<bool>(),
        .mirrorNodeId = GetData<NodeId>(),
        .isSync = GetData<bool>(),
    };
    NodeId nodeId = GetData<NodeId>();
    renderServiceClient->CreateNode(displayNodeconfig, nodeId);

    RSSurfaceRenderNodeConfig config = { .id = GetData<NodeId>(), .name = "fuzztest" };
    renderServiceClient->CreateNode(config);
    return true;
}

bool DoCreateNodeAndSurface(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    RSSurfaceRenderNodeConfig config = { .id = GetData<NodeId>(), .name = "fuzztest" };
    renderServiceClient->CreateNodeAndSurface(config);
    return true;
}

bool DoCreateRSSurface()
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    sptr<Surface> surface;
    renderServiceClient->CreateRSSurface(surface);
    return true;
}

#ifdef ACE_ENABLE_GL
bool DoCreateRSSurface02(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    sptr<Surface> surfaceOpengl;
    renderServiceClient->CreateRSSurface(surfaceOpengl);
    return true;
}
#endif

bool DoCreateVSyncReceiver(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    std::string name = "fuzztest";
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> looper;
    uint64_t id = GetData<uint64_t>();
    NodeId windowNodeId = GetData<uint64_t>();
    renderServiceClient->CreateVSyncReceiver(name, looper, id, windowNodeId);
    return true;
}

bool DoCreatePixelMapFromSurfaceId(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    uint64_t surfaceId = GetData<uint64_t>();
    Rect srcRect = {0, 0, 100, 100};
    renderServiceClient->CreatePixelMapFromSurfaceId(surfaceId, srcRect);
    return true;
}

bool DoTriggerSurfaceCaptureCallback(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderPipelineClient> renderPipelineClient = std::make_shared<RSRenderPipelineClient>();
    NodeId id = GetData<NodeId>();
    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    Media::InitializationOptions opts;
    opts.size.width = width;
    opts.size.height = height;
    opts.srcPixelFormat = static_cast<Media::PixelFormat>(GetData<int32_t>());
    opts.pixelFormat = static_cast<Media::PixelFormat>(GetData<int32_t>());
    opts.alphaType = static_cast<Media::AlphaType>(GetData<int32_t>());
    opts.scaleMode = static_cast<Media::ScaleMode>(GetData<int32_t>());
    opts.editable = GetData<bool>();
    opts.useSourceIfMatch = GetData<bool>();
    std::shared_ptr<Media::PixelMap>  pixelmap = Media::PixelMap::Create(opts);
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = GetData<float>();
    captureConfig.scaleY = GetData<float>();
    captureConfig.useDma = GetData<bool>();
    captureConfig.useCurWindow = GetData<bool>();
    uint8_t type = GetData<uint8_t>();
    captureConfig.captureType = static_cast<SurfaceCaptureType>(type);
    captureConfig.isSync = GetData<bool>();
    captureConfig.mainScreenRect.left_ = GetData<float>();
    captureConfig.mainScreenRect.top_ = GetData<float>();
    captureConfig.mainScreenRect.right_ = GetData<float>();
    captureConfig.mainScreenRect.bottom_ = GetData<float>();

    renderPipelineClient->TriggerSurfaceCaptureCallback(id, captureConfig, pixelmap);
    return true;
}

bool DoTakeSurfaceCapture(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderPipelineClient> renderPipelineClient = std::make_shared<RSRenderPipelineClient>();
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
    uint8_t listSize = GetData<uint8_t>();
    for (uint8_t i = 0; i < listSize; ++i) {
        uint64_t listNodeId = GetData<uint64_t>();
        captureConfig.blackList.push_back(listNodeId);
    }
    captureConfig.mainScreenRect.left_ = GetData<float>();
    captureConfig.mainScreenRect.top_ = GetData<float>();
    captureConfig.mainScreenRect.right_ = GetData<float>();
    captureConfig.mainScreenRect.bottom_ = GetData<float>();

    renderPipelineClient->TakeSurfaceCapture(nodeId, callback, captureConfig);
    return true;
}

bool DoSetFocusAppInfo(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderPipelineClient> renderPipelineClient = std::make_shared<RSRenderPipelineClient>();
    int32_t pid = GetData<int32_t>();
    int32_t uid = GetData<int32_t>();
    std::string bundleName = "bundleName";
    std::string abilityName = "abilityName";
    uint64_t focusNodeId = GetData<uint64_t>();
    FocusAppInfo info = {
        .pid = pid,
        .uid = uid,
        .bundleName = bundleName,
        .abilityName = abilityName,
        .focusNodeId = focusNodeId};
    renderPipelineClient->SetFocusAppInfo(info);
    return true;
}

bool DoGetDefaultScreenId()
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    renderServiceClient->GetDefaultScreenId();
    return true;
}

bool DoGetActiveScreenId()
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    renderServiceClient->GetActiveScreenId();
    return true;
}

bool DoGetAllScreenIds()
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    renderServiceClient->GetAllScreenIds();
    return true;
}

bool DoCreateVirtualScreen(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    std::string name = "name";
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("DisplayNode");
    sptr<IBufferProducer> bp = cSurface->GetProducer();
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(bp);
    ScreenId mirrorId = GetData<ScreenId>();
    int32_t flags = GetData<int32_t>();
    std::vector<NodeId> whiteList;
    renderServiceClient->CreateVirtualScreen(name, width, height, pSurface, mirrorId, flags, whiteList);
    return true;
}

bool DoSetVirtualScreenAutoRotation(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ScreenId screenId = GetData<ScreenId>();
    bool isAutoRotation = GetData<bool>();
    renderServiceClient->SetVirtualScreenAutoRotation(screenId, isAutoRotation);
    return true;
}

bool DoSetVirtualScreenBlackList(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    std::vector<NodeId> blackListVector;
    renderServiceClient->SetVirtualScreenBlackList(id, blackListVector);
    return true;
}

bool DoAddVirtualScreenBlackList(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    std::vector<NodeId> blackListVector;
    renderServiceClient->AddVirtualScreenBlackList(id, blackListVector);
    return true;
}

bool DoRemoveVirtualScreenBlackList(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    std::vector<NodeId> blackListVector;
    renderServiceClient->RemoveVirtualScreenBlackList(id, blackListVector);
    return true;
}

bool DoDropFrameByPid(const uint8_t* data, size_t size)
{
    std::vector<int32_t> pidList;
    uint8_t pidListSize = GetData<uint8_t>();
    for (size_t i = 0; i < pidListSize; i++) {
        pidList.push_back(GetData<int32_t>());
    }

    std::shared_ptr<RSRenderPipelineClient> renderPipelineClient = std::make_shared<RSRenderPipelineClient>();
    renderPipelineClient->DropFrameByPid(pidList);
    return true;
}

bool DoSetWatermark(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    std::string name = "name";

    Media::InitializationOptions opts;
    opts.size.width = GetData<int32_t>();
    opts.size.height = GetData<int32_t>();
    opts.srcPixelFormat = static_cast<Media::PixelFormat>(GetData<int32_t>());
    opts.pixelFormat = static_cast<Media::PixelFormat>(GetData<int32_t>());
    opts.alphaType = static_cast<Media::AlphaType>(GetData<int32_t>());
    opts.scaleMode = static_cast<Media::ScaleMode>(GetData<int32_t>());
    opts.editable = GetData<bool>();
    opts.useSourceIfMatch = GetData<bool>();
    std::shared_ptr<Media::PixelMap>  watermark = Media::PixelMap::Create(opts);
    renderServiceClient->SetWatermark(name, watermark);
    return true;
}

bool DoSetVirtualScreenSecurityExemptionList(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    std::vector<NodeId> securityExemptionList;
    renderServiceClient->SetVirtualScreenSecurityExemptionList(id, securityExemptionList);
    return true;
}

bool DoSetCastScreenEnableSkipWindow(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    bool enable = GetData<bool>();
    renderServiceClient->SetCastScreenEnableSkipWindow(id, enable);
    return true;
}

bool DoSetVirtualScreenSurface(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    sptr<Surface> surface;
    renderServiceClient->SetVirtualScreenSurface(id, surface);
    return true;
}

bool DoRemoveVirtualScreen(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    renderServiceClient->RemoveVirtualScreen(id);
    return true;
}

bool DoSetScreenRefreshRate(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    int32_t sceneId = GetData<int32_t>();
    int32_t rate = GetData<int32_t>();
    renderServiceClient->SetScreenRefreshRate(id, sceneId, rate);
    return true;
}

bool DoSetRefreshRateMode(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    int32_t refreshRateMode = GetData<int32_t>();
    renderServiceClient->SetRefreshRateMode(refreshRateMode);
    return true;
}

bool DoSyncFrameRateRange(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    FrameRateLinkerId id = GetData<FrameRateLinkerId>();
    FrameRateRange range;
    int32_t animatorExpectedFrameRate = GetData<int32_t>();
    renderServiceClient->SyncFrameRateRange(id, range, animatorExpectedFrameRate);
    return true;
}

bool DoUnregisterFrameRateLinker(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    FrameRateLinkerId id = GetData<FrameRateLinkerId>();
    renderServiceClient->UnregisterFrameRateLinker(id);
    return true;
}

bool DoGetScreenCurrentRefreshRate(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    renderServiceClient->GetScreenCurrentRefreshRate(id);
    return true;
}

bool DoGetCurrentRefreshRateMode()
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    renderServiceClient->GetCurrentRefreshRateMode();
    return true;
}

bool DoGetScreenSupportedRefreshRates(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    renderServiceClient->GetScreenSupportedRefreshRates(id);
    return true;
}

bool DoGetShowRefreshRateEnabled()
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    renderServiceClient->GetShowRefreshRateEnabled();
    return true;
}

bool DoGetRefreshInfo(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    pid_t pid = GetData<pid_t>();
    renderServiceClient->GetRefreshInfo(pid);
    return true;
}

bool DoGetRefreshInfoToSP(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    NodeId id = GetData<NodeId>();
    renderServiceClient->GetRefreshInfoToSP(id);
    return true;
}

bool DoSetShowRefreshRateEnabled(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    bool enabled = GetData<bool>();
    int32_t type = GetData<int32_t>();
    renderServiceClient->SetShowRefreshRateEnabled(enabled, type);
    return true;
}

bool DoGetRealtimeRefreshRate(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    renderServiceClient->GetRealtimeRefreshRate(id);
    return true;
}

bool DoSetPhysicalScreenResolution(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    renderServiceClient->SetPhysicalScreenResolution(id, width, height);
    return true;
}

bool DoSetVirtualScreenResolution(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    renderServiceClient->SetVirtualScreenResolution(id, width, height);
    return true;
}

bool DoGetVirtualScreenResolution(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    renderServiceClient->GetVirtualScreenResolution(id);
    return true;
}

bool DoMarkPowerOffNeedProcessOneFrame()
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    renderServiceClient->MarkPowerOffNeedProcessOneFrame();
    return true;
}

bool DoDisablePowerOffRenderControl(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    renderServiceClient->DisablePowerOffRenderControl(id);
    return true;
}

bool DoSetScreenPowerStatus(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    ScreenPowerStatus status = ScreenPowerStatus::INVALID_POWER_STATUS;
    renderServiceClient->SetScreenPowerStatus(id, status);
    return true;
}

bool DoGetScreenActiveMode(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    renderServiceClient->GetScreenActiveMode(id);
    return true;
}

bool DoGetScreenSupportedModes(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    uint32_t level = GetData<uint32_t>();
    renderServiceClient->GetScreenSupportedModes(id);
    renderServiceClient->GetScreenCapability(id);
    renderServiceClient->GetScreenPowerStatus(id);
    renderServiceClient->GetScreenData(id);
    renderServiceClient->GetScreenBacklight(id);
    renderServiceClient->SetScreenBacklight(id, level);

    return true;
}

bool DoRegisterBufferAvailableListener(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    NodeId id = GetData<ScreenId>();
    BufferAvailableCallback callback;
    bool isFromRenderThread = GetData<bool>();
    renderServiceClient->RegisterBufferAvailableListener(id, callback, isFromRenderThread);
    renderServiceClient->UnregisterBufferAvailableListener(id);
    return true;
}

bool DoRegisterBufferClearListener(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    NodeId id = GetData<ScreenId>();
    BufferClearCallback callback;
    renderServiceClient->RegisterBufferClearListener(id, callback);
    return true;
}

bool DoGetScreenSupportedColorGamuts(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    std::vector<ScreenColorGamut> mode;
    renderServiceClient->GetScreenSupportedColorGamuts(id, mode);
    return true;
}

bool DoGetScreenSupportedMetaDataKeys(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    std::vector<ScreenHDRMetadataKey> keys;
    renderServiceClient->GetScreenSupportedMetaDataKeys(id, keys);
    return true;
}

bool DoGetScreenColorGamut(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    ScreenColorGamut mode = ScreenColorGamut::COLOR_GAMUT_INVALID;
    int32_t modeIdx = GetData<int32_t>();
    renderServiceClient->SetScreenColorGamut(id, modeIdx);
    renderServiceClient->GetScreenColorGamut(id, mode);
    return true;
}

bool DoSetScreenGamutMap(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    ScreenGamutMap mode = ScreenGamutMap::GAMUT_MAP_CONSTANT;
    renderServiceClient->SetScreenGamutMap(id, mode);
    renderServiceClient->GetScreenGamutMap(id, mode);
    return true;
}

bool DoSetScreenCorrection(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    ScreenRotation screenRotation = ScreenRotation::ROTATION_0;
    renderServiceClient->SetScreenCorrection(id, screenRotation);
    return true;
}

bool DoSetVirtualMirrorScreenCanvasRotation(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    bool canvasRotation = GetData<bool>();
    renderServiceClient->SetVirtualMirrorScreenCanvasRotation(id, canvasRotation);
    return true;
}

bool DoSetVirtualMirrorScreenScaleMode(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    ScreenScaleMode scaleMode = ScreenScaleMode::FILL_MODE;
    renderServiceClient->SetVirtualMirrorScreenScaleMode(id, scaleMode);
    return true;
}

bool DoSetGlobalDarkColorMode(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    bool isDark = GetData<bool>();
    renderServiceClient->SetGlobalDarkColorMode(isDark);
    return true;
}

bool DoGetScreenHDRCapability(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    RSScreenHDRCapability screenHdrCapability;
    renderServiceClient->GetScreenHDRCapability(id, screenHdrCapability);
    return true;
}

bool DoGetPixelFormat(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    GraphicPixelFormat pixelFormat = GRAPHIC_PIXEL_FMT_BGRA_8888;
    renderServiceClient->SetPixelFormat(id, pixelFormat);
    renderServiceClient->GetPixelFormat(id, pixelFormat);
    return true;
}

bool DoGetScreenSupportedHDRFormats(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    std::vector<ScreenHDRFormat> hdrFormats;
    ScreenHDRFormat hdrFormat = ScreenHDRFormat::VIDEO_HLG;
    int32_t modeIdx = GetData<int32_t>();

    renderServiceClient->SetScreenHDRFormat(id, modeIdx);
    renderServiceClient->GetScreenHDRFormat(id, hdrFormat);
    renderServiceClient->GetScreenSupportedHDRFormats(id, hdrFormats);
    return true;
}

bool DoGetScreenSupportedColorSpaces(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    std::vector<GraphicCM_ColorSpaceType> colorSpaces;
    GraphicCM_ColorSpaceType colorSpace = GraphicCM_ColorSpaceType::GRAPHIC_CM_BT601_EBU_FULL;

    renderServiceClient->SetScreenColorSpace(id, colorSpace);
    renderServiceClient->GetScreenColorSpace(id, colorSpace);
    renderServiceClient->GetScreenSupportedColorSpaces(id, colorSpaces);
    return true;
}

bool DoGetScreenType(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    RSScreenType screenType = RSScreenType::BUILT_IN_TYPE_SCREEN;
    renderServiceClient->GetScreenType(id, screenType);
    return true;
}

bool DoGetBitmap(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    Drawing::Bitmap bm;
    NodeId id = GetData<uint64_t>();
    renderServiceClient->GetBitmap(id, bm);
    return true;
}

bool DoSetVirtualScreenRefreshRate(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    uint32_t maxRefreshRate = GetData<uint32_t>();
    uint32_t actualRefreshRate = GetData<uint32_t>();
    renderServiceClient->SetVirtualScreenRefreshRate(id, maxRefreshRate, actualRefreshRate);
    return true;
}

bool DoRegisterOcclusionChangeCallback()
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    OcclusionChangeCallback callback;
    renderServiceClient->RegisterOcclusionChangeCallback(callback);
    return true;
}

bool DoRegisterSurfaceOcclusionChangeCallback(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    SurfaceOcclusionChangeCallback callback;
    NodeId id = GetData<NodeId>();
    std::vector<float> partitionPoints;
    renderServiceClient->RegisterSurfaceOcclusionChangeCallback(id, callback, partitionPoints);
    renderServiceClient->UnRegisterSurfaceOcclusionChangeCallback(id);
    return true;
}

bool DoRegisterHgmConfigChangeCallback(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    HgmConfigChangeCallback callback;
    renderServiceClient->RegisterHgmConfigChangeCallback(callback);
    HgmRefreshRateModeChangeCallback callback2;
    renderServiceClient->RegisterHgmRefreshRateModeChangeCallback(callback2);
    HgmRefreshRateUpdateCallback callback3;
    renderServiceClient->RegisterHgmRefreshRateUpdateCallback(callback3);
    uint32_t dstPid = GetData<uint32_t>();
    FrameRateLinkerExpectedFpsUpdateCallback callback4;
    renderServiceClient->RegisterFrameRateLinkerExpectedFpsUpdateCallback(dstPid, callback4);

    return true;
}

bool DoRegisterFrameRateLinkerExpectedFpsUpdateCallback(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSInterfaces> rsInterfaces = std::make_shared<RSInterfaces>();
    uint32_t dstPid = GetData<uint32_t>();
    FrameRateLinkerExpectedFpsUpdateCallback callback;
    rsInterfaces->RegisterFrameRateLinkerExpectedFpsUpdateCallback(dstPid, callback);

    return true;
}

bool DoUnRegisterFrameRateLinkerExpectedFpsUpdateCallback(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSInterfaces> rsInterfaces = std::make_shared<RSInterfaces>();
    uint32_t dstPid = GetData<uint32_t>();
    rsInterfaces->UnRegisterFrameRateLinkerExpectedFpsUpdateCallback(dstPid);

    return true;
}

bool DoSetAppWindowNum(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    uint32_t num = GetData<uint32_t>();
    renderServiceClient->SetAppWindowNum(num);

    return true;
}

bool DoSetSystemAnimatedScenes()
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    SystemAnimatedScenes systemAnimatedScenes = SystemAnimatedScenes::ENTER_MISSION_CENTER;
    renderServiceClient->SetSystemAnimatedScenes(systemAnimatedScenes, false);
    return true;
}

bool DoShowWatermark(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    Media::InitializationOptions opts;
    opts.size.width = GetData<int32_t>();
    opts.size.height = GetData<int32_t>();
    opts.srcPixelFormat = static_cast<Media::PixelFormat>(GetData<int32_t>());
    opts.pixelFormat = static_cast<Media::PixelFormat>(GetData<int32_t>());
    opts.alphaType = static_cast<Media::AlphaType>(GetData<int32_t>());
    opts.scaleMode = static_cast<Media::ScaleMode>(GetData<int32_t>());
    opts.editable = GetData<bool>();
    opts.useSourceIfMatch = GetData<bool>();
    std::shared_ptr<Media::PixelMap>  watermarkImg = Media::PixelMap::Create(opts);
    bool isShow = GetData<bool>();
    renderServiceClient->ShowWatermark(watermarkImg, isShow);
    return true;
}

bool DoResizeVirtualScreen(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    renderServiceClient->ResizeVirtualScreen(id, width, height);
    return true;
}

bool DoReportJankStats()
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    renderServiceClient->ReportJankStats();
    return true;
}

bool DoReportEventResponse(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    DataBaseRs info = {
        .appPid = GetData<int32_t>(),
        .eventType = GetData<int32_t>(),
        .versionCode = GetData<int32_t>(),
        .uniqueId = GetData<int64_t>(),
        .inputTime = GetData<int64_t>(),
        .beginVsyncTime = GetData<int64_t>(),
        .endVsyncTime = GetData<int64_t>(),
        .isDisplayAnimator = GetData<bool>(),
    };
    renderServiceClient->ReportEventResponse(info);
    renderServiceClient->ReportEventComplete(info);
    renderServiceClient->ReportEventJankFrame(info);
    return true;
}

bool DoReportGameStateData(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    GameStateData info = {
        .pid = GetData<int32_t>(),
        .uid = GetData<int32_t>(),
        .state = GetData<int32_t>(),
        .renderTid = GetData<int32_t>(),
        .bundleName = "bundleName",
    };
    renderServiceClient->ReportGameStateData(info);

    return true;
}

bool DoSetHardwareEnabled(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    NodeId id = GetData<NodeId>();
    bool isEnabled = GetData<bool>();
    SelfDrawingNodeType selfDrawingType = SelfDrawingNodeType::DEFAULT;
    bool dynamicHardwareEnable = GetData<bool>();
    renderServiceClient->SetHardwareEnabled(id, isEnabled, selfDrawingType, dynamicHardwareEnable);

    return true;
}

bool DoSetHidePrivacyContent(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    NodeId id = GetData<NodeId>();
    bool needHidePrivacyContent = GetData<bool>();
    renderServiceClient->SetHidePrivacyContent(id, needHidePrivacyContent);
    return true;
}

bool DoNotifyLightFactorStatus(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    int32_t lightFactorStatus = GetData<int32_t>();
    renderServiceClient->NotifyLightFactorStatus(lightFactorStatus);
    return true;
}

bool DoNotifyPackageEvent(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    uint32_t listSize = GetData<uint32_t>();
    std::vector<std::string> packageList;
    renderServiceClient->NotifyPackageEvent(listSize, packageList);
    return true;
}

bool DONotifyAppStrategyConfigChangeEvent(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    std::string pkgName = GetData<std::string>();
    uint32_t listSize = GetData<uint32_t>();
    std::string configKey = GetData<std::string>();
    std::string configValue = GetData<std::string>();
    std::vector<std::pair<std::string, std::string>> newConfig;
    newConfig.push_back(make_pair(configKey, configValue));
    renderServiceClient->NotifyAppStrategyConfigChangeEvent(pkgName, listSize, newConfig);
    return true;
}

bool DoNotifyRefreshRateEvent(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    EventInfo eventInfo = {
        .eventName = "eventName",
        .eventStatus = GetData<bool>(),
        .minRefreshRate = GetData<uint32_t>(),
        .maxRefreshRate = GetData<uint32_t>(),
        .description = "description",
    };
    renderServiceClient->NotifyRefreshRateEvent(eventInfo);
    return true;
}

bool DoNotifySoftVsyncRateDiscountEvent(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    uint32_t pid = GetData<uint32_t>();
    std::string name = "fuzztest";
    uint32_t rateDiscount = GetData<uint32_t>();
    renderServiceClient->NotifySoftVsyncRateDiscountEvent(pid, name, rateDiscount);
    return true;
}

bool DoNotifyTouchEvent(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    int32_t touchStatus = GetData<int32_t>();
    int32_t touchCnt = GetData<int32_t>();
    bool enableDynamicMode = GetData<bool>();
    renderServiceClient->NotifyTouchEvent(touchStatus, touchCnt);
    renderServiceClient->NotifyDynamicModeEvent(enableDynamicMode);
    return true;
}

bool DoNotifyHgmConfigEvent(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    std::string eventName = "eventName";
    bool state = GetData<bool>();
    renderServiceClient->NotifyHgmConfigEvent(eventName, state);
    return true;
}

bool DoNotifyXComponentExpectedFrameRate(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    std::string id = GetStringFromData(STR_LEN);
    int32_t expectedFrameRate = GetData<int32_t>();
    renderServiceClient->NotifyXComponentExpectedFrameRate(id, expectedFrameRate);
    return true;
}

bool DoSetCacheEnabledForRotation(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    bool isEnabled = GetData<bool>();
    renderServiceClient->SetCacheEnabledForRotation(isEnabled);
    return true;
}

bool DoSetOnRemoteDiedCallback()
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    OnRemoteDiedCallback callback;
    renderServiceClient->SetOnRemoteDiedCallback(callback);
    return true;
}

bool DoGetActiveDirtyRegionInfo()
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    renderServiceClient->GetActiveDirtyRegionInfo();
    renderServiceClient->GetGlobalDirtyRegionInfo();
    renderServiceClient->GetLayerComposeInfo();
    renderServiceClient->GetHwcDisabledReasonInfo();
    return true;
}

bool DoSetVmaCacheStatus(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    bool flag = GetData<bool>();
    bool isVirtualScreenUsingStatus = GetData<bool>();
    bool isCurtainScreenOn = GetData<bool>();
    renderServiceClient->SetVmaCacheStatus(flag);
    renderServiceClient->SetVirtualScreenUsingStatus(isVirtualScreenUsingStatus);
    renderServiceClient->SetCurtainScreenUsingStatus(isCurtainScreenOn);
    return true;
}

bool DoRegisterUIExtensionCallback(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    uint64_t userId = GetData<uint64_t>();
    UIExtensionCallback callback;
    renderServiceClient->RegisterUIExtensionCallback(userId, callback);
    return true;
}

bool DoSetAncoForceDoDirect(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderPipelineClient> renderPipelineClient = std::make_shared<RSRenderPipelineClient>();
    bool direct = GetData<bool>();
    renderPipelineClient->SetAncoForceDoDirect(direct);
    return true;
}

bool DoSetVirtualScreenStatus(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    VirtualScreenStatus screenStatus = VirtualScreenStatus::VIRTUAL_SCREEN_PLAY;
    renderServiceClient->SetVirtualScreenStatus(id, screenStatus);
    return true;
}

bool DoRegisterSurfaceBufferCallback(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderPipelineClient> renderPipelineClient = std::make_shared<RSRenderPipelineClient>();
    pid_t pid = GetData<pid_t>();
    uint64_t uid = GetData<uint64_t>();
    std::shared_ptr<SurfaceBufferCallback> callback;
    renderPipelineClient->RegisterSurfaceBufferCallback(pid, uid, callback);
    renderPipelineClient->UnregisterSurfaceBufferCallback(pid, uid);
    return true;
}

bool DoTriggerSurfaceBufferCallback(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderPipelineClient> renderPipelineClient = std::make_shared<RSRenderPipelineClient>();
    uint64_t uid = GetData<uint64_t>();
    std::vector<uint32_t> surfaceBufferIds;
    std::vector<uint8_t> isRenderedFlags;
    renderPipelineClient->TriggerOnFinish({
        .uid = uid,
        .surfaceBufferIds = surfaceBufferIds,
        .isRenderedFlags = isRenderedFlags,
    });
    return true;
}

bool DoSetLayerTop(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    std::string nodeIdStr = "nodeIdStr";
    bool isTop = GetData<bool>();
    renderServiceClient->SetLayerTop(nodeIdStr, isTop);
    return true;
}

bool DoSetForceRefresh(const uint8_t* data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    std::string nodeIdStr = "nodeIdStr";
    bool isForceRefresh = GetData<bool>();
    renderServiceClient->SetForceRefresh(nodeIdStr, isForceRefresh);
    return true;
}

bool DoExecuteSynchronousTask002()
{
    std::shared_ptr<RSIRenderClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    auto task = nullptr;
    renderServiceClient->ExecuteSynchronousTask(task);

    return true;
}

bool DoGetUniRenderEnabled(const uint8_t *data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    int pid = GetData<int>();
    RSRenderServiceConnectHub::GetInstance()->Destroy();

    renderServiceClient->GetUniRenderEnabled();
    renderServiceClient->GetMemoryGraphic(pid);
    renderServiceClient->GetMemoryGraphics();
    return true;
}

bool DoCreateNode002(const uint8_t *data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    RSDisplayNodeConfig displayNodeconfig = {
        .screenId = GetData<uint64_t>(),
        .isMirrored = GetData<bool>(),
        .mirrorNodeId = GetData<NodeId>(),
        .isSync = GetData<bool>(),
    };
    RSRenderServiceConnectHub::GetInstance()->Destroy();
    NodeId nodeId = GetData<NodeId>();
    renderServiceClient->CreateNode(displayNodeconfig, nodeId);
    RSSurfaceRenderNodeConfig config = {.id = GetData<NodeId>(), .name = "fuzztest"};
    renderServiceClient->CreateNode(config);
    renderServiceClient->CreateNodeAndSurface(config);
    return true;
}

bool DoGetTotalAppMemSize002(const uint8_t *data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    float cpuMemSize = GetData<int>();
    float gpuMemSize = GetData<int>();
    RSRenderServiceConnectHub::GetInstance()->Destroy();
    renderServiceClient->GetTotalAppMemSize(cpuMemSize, gpuMemSize);

    std::string name = "fuzztest";
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> looper;
    uint64_t id = GetData<uint64_t>();
    NodeId windowNodeId = GetData<uint64_t>();
    renderServiceClient->CreateVSyncReceiver(name, looper, id, windowNodeId);

    uint64_t surfaceId = GetData<uint64_t>();
    Rect srcRect = {0, 0, 100, 100};
    renderServiceClient->CreatePixelMapFromSurfaceId(surfaceId, srcRect);
    return true;
}

bool DoTakeSurfaceCapture002(const uint8_t *data, size_t size)
{
    std::shared_ptr<RSRenderPipelineClient> renderPipelineClient = std::make_shared<RSRenderPipelineClient>();
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
    renderPipelineClient->TakeSurfaceCapture(nodeId, callback, captureConfig);
    return true;
}

bool DoSetHwcNodeBounds(const uint8_t *data, size_t size)
{
    std::shared_ptr<RSRenderPipelineClient> renderPipelineClient = std::make_shared<RSRenderPipelineClient>();
    int64_t rsNodeId = GetData<int64_t>();
    float positionX = GetData<float>();
    float positionY = GetData<float>();
    float positionZ = GetData<float>();
    float positionW = GetData<float>();

    renderPipelineClient->SetHwcNodeBounds(rsNodeId, positionX, positionY, positionZ, positionW);
    return true;
}

bool DoSetHwcNodeBounds002(const uint8_t *data, size_t size)
{
    std::shared_ptr<RSRenderPipelineClient> renderPipelineClient = std::make_shared<RSRenderPipelineClient>();
    int64_t rsNodeId = GetData<int64_t>();
    float positionX = GetData<float>();
    float positionY = GetData<float>();
    float positionZ = GetData<float>();
    float positionW = GetData<float>();

    RSRenderServiceConnectHub::GetInstance()->Destroy();
    renderPipelineClient->SetHwcNodeBounds(rsNodeId, positionX, positionY, positionZ, positionW);
    return true;
}

bool DoSetFocusAppInfo002(const uint8_t *data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    std::shared_ptr<RSRenderPipelineClient> renderPipelineClient = std::make_shared<RSRenderPipelineClient>();
    int32_t pid = GetData<int32_t>();
    int32_t uid = GetData<int32_t>();
    std::string bundleName = "bundleName";
    std::string abilityName = "abilityName";
    uint64_t focusNodeId = GetData<uint64_t>();

    RSRenderServiceConnectHub::GetInstance()->Destroy();
    FocusAppInfo info = {
        .pid = pid,
        .uid = uid,
        .bundleName = bundleName,
        .abilityName = abilityName,
        .focusNodeId = focusNodeId};
    renderPipelineClient->SetFocusAppInfo(info);
    renderServiceClient->GetDefaultScreenId();
    renderServiceClient->GetActiveScreenId();
    renderServiceClient->GetAllScreenIds();
    renderServiceClient->GetAllScreenIds();
    std::string name = "name";
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("DisplayNode");
    sptr<IBufferProducer> bp = cSurface->GetProducer();
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(bp);
    ScreenId mirrorId = GetData<ScreenId>();
    int32_t flags = GetData<int32_t>();
    std::vector<NodeId> whiteList;
    renderServiceClient->CreateVirtualScreen(name, width, height, pSurface, mirrorId, flags, whiteList);
    return true;
}

bool DoSetVirtualScreenBlackList002(const uint8_t *data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    std::vector<NodeId> blackListVector;
    RSRenderServiceConnectHub::GetInstance()->Destroy();
    renderServiceClient->SetVirtualScreenBlackList(id, blackListVector);

    std::string name = "name";
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
    renderServiceClient->SetWatermark(name, watermark);

    std::vector<NodeId> securityExemptionList;
    renderServiceClient->SetVirtualScreenSecurityExemptionList(id, securityExemptionList);
    return true;
}

bool DoAddVirtualScreenBlackList002(const uint8_t *data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    std::vector<NodeId> blackListVector;
    NodeId nodeId = GetData<ScreenId>();
    blackListVector.push_back(nodeId);
    RSRenderServiceConnectHub::GetInstance()->Destroy();
    renderServiceClient->AddVirtualScreenBlackList(id, blackListVector);
    return true;
}

bool DoRemoveVirtualScreenBlackList002(const uint8_t *data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    std::vector<NodeId> blackListVector;
    NodeId nodeId = GetData<ScreenId>();
    blackListVector.push_back(nodeId);
    RSRenderServiceConnectHub::GetInstance()->Destroy();
    renderServiceClient->RemoveVirtualScreenBlackList(id, blackListVector);
    return true;
}

bool DoSetCastScreenEnableSkipWindow002(const uint8_t *data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    RSRenderServiceConnectHub::GetInstance()->Destroy();
    ScreenId id = GetData<ScreenId>();
    bool enable = GetData<bool>();
    int32_t refreshRateMode = GetData<int32_t>();
    sptr<Surface> surface;
    ScreenChangeCallback cb;
    ScreenSwitchingNotifyCallback switchingCb;
    uint32_t modeId = GetData<uint32_t>();
    int32_t sceneId = GetData<int32_t>();
    int32_t rate = GetData<int32_t>();

    renderServiceClient->SetCastScreenEnableSkipWindow(id, enable);
    renderServiceClient->SetVirtualScreenSurface(id, surface);
    renderServiceClient->RemoveVirtualScreen(id);
    renderServiceClient->SetScreenChangeCallback(cb);
    renderServiceClient->SetScreenSwitchingNotifyCallback(switchingCb);
    renderServiceClient->SetScreenActiveMode(id, modeId);
    renderServiceClient->SetScreenRefreshRate(id, sceneId, rate);
    renderServiceClient->SetRefreshRateMode(refreshRateMode);

    return true;
}

bool DoSyncFrameRateRange002(const uint8_t *data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    FrameRateLinkerId id = GetData<FrameRateLinkerId>();
    FrameRateRange range;
    int32_t animatorExpectedFrameRate = GetData<int32_t>();
    ScreenId screenId = GetData<ScreenId>();
    bool enabled = GetData<bool>();
    int32_t type = GetData<int32_t>();
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    RSRenderServiceConnectHub::GetInstance()->Destroy();

    renderServiceClient->SyncFrameRateRange(id, range, animatorExpectedFrameRate);
    renderServiceClient->UnregisterFrameRateLinker(id);
    renderServiceClient->GetScreenCurrentRefreshRate(screenId);
    renderServiceClient->GetCurrentRefreshRateMode();
    renderServiceClient->GetScreenSupportedRefreshRates(screenId);
    renderServiceClient->GetShowRefreshRateEnabled();
    renderServiceClient->SetShowRefreshRateEnabled(enabled, type);
    renderServiceClient->GetRealtimeRefreshRate(screenId);
    renderServiceClient->SetVirtualScreenResolution(screenId, width, height);
    renderServiceClient->GetVirtualScreenResolution(screenId);
    renderServiceClient->MarkPowerOffNeedProcessOneFrame();
    return true;
}

bool DoDisablePowerOffRenderControl002(const uint8_t *data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    ScreenId screenId = GetData<ScreenId>();
    ScreenPowerStatus status = ScreenPowerStatus::INVALID_POWER_STATUS;
    NodeId id = GetData<ScreenId>();
    BufferAvailableCallback callback;
    BufferClearCallback bufferClearCallback;
    bool isFromRenderThread = GetData<bool>();
    RSRenderServiceConnectHub::GetInstance()->Destroy();

    renderServiceClient->DisablePowerOffRenderControl(screenId);
    renderServiceClient->SetScreenPowerStatus(screenId, status);
    renderServiceClient->GetScreenActiveMode(screenId);
    renderServiceClient->GetScreenSupportedModes(screenId);
    renderServiceClient->GetScreenCapability(screenId);
    renderServiceClient->GetScreenPowerStatus(screenId);
    renderServiceClient->GetScreenData(screenId);
    renderServiceClient->GetScreenBacklight(screenId);
    renderServiceClient->RegisterBufferAvailableListener(id, callback, isFromRenderThread);
    renderServiceClient->RegisterBufferClearListener(id, bufferClearCallback);

    return true;
}

bool DoGetScreenSupportedColorGamuts002(const uint8_t *data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
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

    renderServiceClient->GetScreenSupportedColorGamuts(id, mode1);
    renderServiceClient->GetScreenSupportedMetaDataKeys(id, keys);
    renderServiceClient->SetScreenColorGamut(id, modeIdx);
    renderServiceClient->GetScreenColorGamut(id, screenColorGamut);
    renderServiceClient->SetScreenGamutMap(id, screenGamutMap);
    renderServiceClient->GetScreenGamutMap(id, screenGamutMap);
    renderServiceClient->SetScreenCorrection(id, screenRotation);
    renderServiceClient->SetVirtualMirrorScreenCanvasRotation(id, canvasRotation);
    renderServiceClient->SetVirtualMirrorScreenScaleMode(id, scaleMode);
    return true;
}

bool DoSetGlobalDarkColorMode002(const uint8_t *data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
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

    renderServiceClient->SetGlobalDarkColorMode(isDark);
    renderServiceClient->GetScreenHDRCapability(screenId, screenHdrCapability);
    renderServiceClient->SetPixelFormat(screenId, pixelFormat);
    renderServiceClient->GetPixelFormat(screenId, pixelFormat);
    renderServiceClient->SetScreenHDRFormat(screenId, modeIdx);
    renderServiceClient->GetScreenHDRFormat(screenId, hdrFormat);
    renderServiceClient->GetScreenSupportedHDRFormats(screenId, hdrFormats);
    renderServiceClient->SetScreenColorSpace(screenId, colorSpace);
    renderServiceClient->GetScreenColorSpace(screenId, colorSpace);
    renderServiceClient->GetScreenSupportedColorSpaces(screenId, colorSpaces);
    return true;
}

bool DoGetScreenType002(const uint8_t *data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
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

    renderServiceClient->GetScreenType(screenId, screenType);
    renderServiceClient->GetBitmap(nodeId, bm);
    renderServiceClient->GetPixelmap(nodeId, pixelmap, rect, drawCmdList);
    renderServiceClient->RegisterTypeface(typeface);
    renderServiceClient->RegisterTypeface(GetData<uint32_t>(), GetData<uint32_t>(), GetData<int32_t>());
    if (typeface) {
        renderServiceClient->UnRegisterTypeface(typeface->GetUniqueID());
    }
    renderServiceClient->UnRegisterTypeface(GetData<uint32_t>());
    renderServiceClient->SetScreenSkipFrameInterval(screenId, skipFrameInterval);
    renderServiceClient->SetVirtualScreenRefreshRate(screenId, maxRefreshRate, actualRefreshRate);
    return true;
}

bool DoRegisterOcclusionChangeCallback002(const uint8_t *data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
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

    renderServiceClient->RegisterOcclusionChangeCallback(occlusionChangeCallback);
    renderServiceClient->RegisterSurfaceOcclusionChangeCallback(
        nodeId, surfaceOcclusionChangeCallback, partitionPoints);
    renderServiceClient->UnRegisterSurfaceOcclusionChangeCallback(nodeId);
    renderServiceClient->RegisterHgmConfigChangeCallback(hgmConfigChangeCallback);
    renderServiceClient->RegisterHgmRefreshRateModeChangeCallback(hgmRefreshRateModeChangeCallback);
    renderServiceClient->RegisterHgmRefreshRateUpdateCallback(hgmRefreshRateUpdateCallback);
    renderServiceClient->SetSystemAnimatedScenes(systemAnimatedScenes, false);
    renderServiceClient->ResizeVirtualScreen(screenId, width, height);
    return true;
}

bool DoSetHidePrivacyContent002(const uint8_t *data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    std::shared_ptr<RSRenderPipelineClient> renderPipelineClient = std::make_shared<RSRenderPipelineClient>();
    NodeId nodeId = GetData<NodeId>();
    bool needHidePrivacyContent = GetData<bool>();
    bool flag = GetData<bool>();
    bool direct = GetData<bool>();
    UIExtensionCallback uiExtensionCallback;
    uint64_t userId = GetData<uint64_t>();
    RSRenderServiceConnectHub::GetInstance()->Destroy();

    renderServiceClient->SetHidePrivacyContent(nodeId, needHidePrivacyContent);
    renderServiceClient->GetActiveDirtyRegionInfo();
    renderServiceClient->GetGlobalDirtyRegionInfo();
    renderServiceClient->GetLayerComposeInfo();
    renderServiceClient->GetHwcDisabledReasonInfo();
    renderServiceClient->SetVmaCacheStatus(flag);
    renderServiceClient->RegisterUIExtensionCallback(userId, uiExtensionCallback);
    renderPipelineClient->SetAncoForceDoDirect(direct);
    return true;
}

bool DoSetBehindWindowFilterEnabled(const uint8_t *data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    RSRenderServiceConnectHub::GetInstance()->Destroy();
    bool enabled = GetData<bool>();
    renderServiceClient->SetBehindWindowFilterEnabled(enabled);
    return true;
}

bool DoGetBehindWindowFilterEnabled(const uint8_t *data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    RSRenderServiceConnectHub::GetInstance()->Destroy();
    bool enabled = GetData<bool>();
    renderServiceClient->GetBehindWindowFilterEnabled(enabled);
    return true;
}

bool ProfilerServiceOpenFile(const uint8_t *data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    RSRenderServiceConnectHub::GetInstance()->Destroy();

    uint32_t strlen = GetData<uint32_t>() % 20;

    HrpServiceDir baseDirType = HrpServiceDir::HRP_SERVICE_DIR_UNKNOWN;

    std::string subDir = GetStringFromData(strlen);
    std::string subDir2 = GetStringFromData(strlen);
    std::string fileName = GetStringFromData(strlen);
    int32_t flags = GetData<int32_t>();
    int32_t outFd = GetData<int32_t>();
    HrpServiceDirInfo dirInfo{baseDirType, subDir, subDir2};

    renderServiceClient->ProfilerServiceOpenFile(dirInfo, fileName, flags, outFd);
    return true;
}

bool ProfilerServicePopulateFiles(const uint8_t *data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    RSRenderServiceConnectHub::GetInstance()->Destroy();

    uint32_t strlen = GetData<uint32_t>() % 20;

    HrpServiceDir baseDirType = HrpServiceDir::HRP_SERVICE_DIR_UNKNOWN;

    std::string subDir = GetStringFromData(strlen);
    std::string subDir2 = GetStringFromData(strlen);

    HrpServiceDirInfo dirInfo{baseDirType, subDir, subDir2};
    std::vector<HrpServiceFileInfo> outFiles;

    renderServiceClient->ProfilerServicePopulateFiles(dirInfo, 0, outFiles);
    return true;
}

bool ProfilerIsSecureScreen(const uint8_t *data, size_t size)
{
    std::shared_ptr<RSRenderServiceClient> renderServiceClient = std::make_shared<RSRenderServiceClient>();
    RSRenderServiceConnectHub::GetInstance()->Destroy();

    renderServiceClient->ProfilerIsSecureScreen();
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
    OHOS::Rosen::DoExecuteSynchronousTask(data, size);
    OHOS::Rosen::DoGetMemoryGraphic(data, size);
    OHOS::Rosen::DoGetMemoryGraphics();
    OHOS::Rosen::DoGetTotalAppMemSize(data, size);
    OHOS::Rosen::DoCreateNode(data, size);
    OHOS::Rosen::DoCreateNodeAndSurface(data, size);
    OHOS::Rosen::DoCreateRSSurface();
    OHOS::Rosen::DoCreateVSyncReceiver(data, size);
    OHOS::Rosen::DoCreatePixelMapFromSurfaceId(data, size);
    OHOS::Rosen::DoTriggerSurfaceCaptureCallback(data, size);
    OHOS::Rosen::DoTakeSurfaceCapture(data, size);
    OHOS::Rosen::DoSetFocusAppInfo(data, size);
    OHOS::Rosen::DoGetDefaultScreenId();
    OHOS::Rosen::DoGetActiveScreenId();
    OHOS::Rosen::DoGetAllScreenIds();
    OHOS::Rosen::DoCreateVirtualScreen(data, size);
    OHOS::Rosen::DoSetVirtualScreenAutoRotation(data, size);
    OHOS::Rosen::DoSetVirtualScreenBlackList(data, size);
    OHOS::Rosen::DoDropFrameByPid(data, size);
    OHOS::Rosen::DoSetWatermark(data, size);
    OHOS::Rosen::DoSetVirtualScreenSecurityExemptionList(data, size);
    OHOS::Rosen::DoSetCastScreenEnableSkipWindow(data, size);
    OHOS::Rosen::DoSetVirtualScreenSurface(data, size);
    OHOS::Rosen::DoRemoveVirtualScreen(data, size);
    OHOS::Rosen::DoSetScreenRefreshRate(data, size);
    OHOS::Rosen::DoSetRefreshRateMode(data, size);
    OHOS::Rosen::DoSyncFrameRateRange(data, size);
    OHOS::Rosen::DoUnregisterFrameRateLinker(data, size);
    OHOS::Rosen::DoGetScreenCurrentRefreshRate(data, size);
    OHOS::Rosen::DoGetCurrentRefreshRateMode();
    OHOS::Rosen::DoGetScreenSupportedRefreshRates(data, size);
    OHOS::Rosen::DoGetShowRefreshRateEnabled();
    OHOS::Rosen::DoGetRefreshInfo(data, size);
    OHOS::Rosen::DoGetRefreshInfoToSP(data, size);
    OHOS::Rosen::DoSetShowRefreshRateEnabled(data, size);
    OHOS::Rosen::DoGetRealtimeRefreshRate(data, size);
    OHOS::Rosen::DoSetPhysicalScreenResolution(data, size);
    OHOS::Rosen::DoSetVirtualScreenResolution(data, size);
    OHOS::Rosen::DoGetVirtualScreenResolution(data, size);
    OHOS::Rosen::DoMarkPowerOffNeedProcessOneFrame();
    OHOS::Rosen::DoDisablePowerOffRenderControl(data, size);
    OHOS::Rosen::DoGetScreenActiveMode(data, size);
    OHOS::Rosen::DoGetScreenSupportedModes(data, size);
    OHOS::Rosen::DoRegisterBufferAvailableListener(data, size);
    OHOS::Rosen::DoRegisterBufferClearListener(data, size);
    OHOS::Rosen::DoGetScreenSupportedColorGamuts(data, size);
    OHOS::Rosen::DoGetScreenSupportedMetaDataKeys(data, size);
    OHOS::Rosen::DoGetScreenColorGamut(data, size);
    OHOS::Rosen::DoSetScreenGamutMap(data, size);
    OHOS::Rosen::DoSetScreenCorrection(data, size);
    OHOS::Rosen::DoSetVirtualMirrorScreenCanvasRotation(data, size);
    OHOS::Rosen::DoSetVirtualMirrorScreenScaleMode(data, size);
    OHOS::Rosen::DoSetGlobalDarkColorMode(data, size);
    OHOS::Rosen::DoGetScreenHDRCapability(data, size);
    OHOS::Rosen::DoGetPixelFormat(data, size);
    OHOS::Rosen::DoGetScreenSupportedHDRFormats(data, size);
    OHOS::Rosen::DoGetScreenSupportedColorSpaces(data, size);
    OHOS::Rosen::DoGetScreenType(data, size);
    OHOS::Rosen::DoSetVirtualScreenRefreshRate(data, size);
    OHOS::Rosen::DoRegisterOcclusionChangeCallback();
    OHOS::Rosen::DoRegisterSurfaceOcclusionChangeCallback(data, size);
    OHOS::Rosen::DoRegisterHgmConfigChangeCallback(data, size);
    OHOS::Rosen::DoSetSystemAnimatedScenes();
    OHOS::Rosen::DoRegisterFrameRateLinkerExpectedFpsUpdateCallback(data, size);
    OHOS::Rosen::DoUnRegisterFrameRateLinkerExpectedFpsUpdateCallback(data, size);
    OHOS::Rosen::DoShowWatermark(data, size);
    OHOS::Rosen::DoResizeVirtualScreen(data, size);
    OHOS::Rosen::DoReportJankStats();
    OHOS::Rosen::DoReportEventResponse(data, size);
    OHOS::Rosen::DoReportGameStateData(data, size);
    OHOS::Rosen::DoSetHardwareEnabled(data, size);
    OHOS::Rosen::DoSetHidePrivacyContent(data, size);
    OHOS::Rosen::DoNotifyLightFactorStatus(data, size);
    OHOS::Rosen::DoNotifyPackageEvent(data, size);
    OHOS::Rosen::DONotifyAppStrategyConfigChangeEvent(data, size);
    OHOS::Rosen::DoNotifyRefreshRateEvent(data, size);
    OHOS::Rosen::DoNotifySoftVsyncRateDiscountEvent(data, size);
    OHOS::Rosen::DoNotifyTouchEvent(data, size);
    OHOS::Rosen::DoSetCacheEnabledForRotation(data, size);
    OHOS::Rosen::DoNotifyHgmConfigEvent(data, size);
    OHOS::Rosen::DoNotifyXComponentExpectedFrameRate(data, size);
    OHOS::Rosen::DoSetOnRemoteDiedCallback();
    OHOS::Rosen::DoGetActiveDirtyRegionInfo();
    OHOS::Rosen::DoSetVmaCacheStatus(data, size);
    OHOS::Rosen::DoRegisterUIExtensionCallback(data, size);
    OHOS::Rosen::DoSetAncoForceDoDirect(data, size);
    OHOS::Rosen::DoSetVirtualScreenStatus(data, size);
    OHOS::Rosen::DoRegisterSurfaceBufferCallback(data, size);
    OHOS::Rosen::DoTriggerSurfaceBufferCallback(data, size);
    OHOS::Rosen::DoSetLayerTop(data, size);
    OHOS::Rosen::DoSetForceRefresh(data, size);
    OHOS::Rosen::DoExecuteSynchronousTask002();
    OHOS::Rosen::DoGetUniRenderEnabled(data, size);
    OHOS::Rosen::DoCreateNode002(data, size);
    OHOS::Rosen::DoGetTotalAppMemSize002(data, size);
    OHOS::Rosen::DoTakeSurfaceCapture002(data, size);
    OHOS::Rosen::DoSetHwcNodeBounds(data, size);
    OHOS::Rosen::DoSetHwcNodeBounds002(data, size);
    OHOS::Rosen::DoSetFocusAppInfo002(data, size);
    OHOS::Rosen::DoSetVirtualScreenBlackList002(data, size);
    OHOS::Rosen::DoSetCastScreenEnableSkipWindow002(data, size);
    OHOS::Rosen::DoSyncFrameRateRange002(data, size);
    OHOS::Rosen::DoDisablePowerOffRenderControl002(data, size);
    OHOS::Rosen::DoGetScreenSupportedColorGamuts002(data, size);
    OHOS::Rosen::DoSetGlobalDarkColorMode002(data, size);
    OHOS::Rosen::DoGetScreenType002(data, size);
    OHOS::Rosen::DoRegisterOcclusionChangeCallback002(data, size);
    OHOS::Rosen::DoSetAppWindowNum(data, size);
    OHOS::Rosen::DoSetHidePrivacyContent002(data, size);
    OHOS::Rosen::DoSetBehindWindowFilterEnabled(data, size);
    OHOS::Rosen::DoGetBehindWindowFilterEnabled(data, size);
    return 0;
}