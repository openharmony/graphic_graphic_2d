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
#include "command/rs_command.h"
#include "command/rs_node_showing_command.h"
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
#include "ipc_callbacks/pointer_render/pointer_luminance_callback_stub.h"
#endif


namespace OHOS {
namespace Rosen {

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
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSIRenderClient> client = std::make_shared<RSRenderServiceClient>();
    uint64_t timeoutNS = GetData<uint64_t>();
    auto task = std::make_shared<DerivedSyncTask>(timeoutNS);
    client->ExecuteSynchronousTask(task);
    
    return true;
}

bool DoGetMemoryGraphic(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    int pid = GetData<int>();
    client->GetMemoryGraphic(pid);

    return true;
}

bool DoGetMemoryGraphics(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    client->GetMemoryGraphics();
    return true;
}

bool DoGetTotalAppMemSize(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    float cpuMemSize = GetData<int>();
    float gpuMemSize  = GetData<int>();
    client->GetTotalAppMemSize(cpuMemSize, gpuMemSize);
    return true;
}

bool DoCreateNode(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

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

bool DoCreateNodeAndSurface(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    RSSurfaceRenderNodeConfig config = { .id = GetData<NodeId>(), .name = "fuzztest" };
    client->CreateNodeAndSurface(config);
    return true;
}

bool DoCreateRSSurface(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    sptr<Surface> surface;
    client->CreateRSSurface(surface);
    return true;
}

bool DoCreateVSyncReceiver(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    std::string name = "fuzztest";
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> looper;
    uint64_t id = GetData<uint64_t>();
    NodeId windowNodeId = GetData<uint64_t>();
    client->CreateVSyncReceiver(name, looper, id, windowNodeId);
    return true;
}

bool DoCreatePixelMapFromSurfaceId(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    uint64_t surfaceId = GetData<uint64_t>();
    Rect srcRect = {0, 0, 100, 100};
    client->CreatePixelMapFromSurfaceId(surfaceId, srcRect);
    return true;
}

bool DoTriggerSurfaceCaptureCallback(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
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

    client->TriggerSurfaceCaptureCallback(id, pixelmap);
    return true;
}

bool DoTakeSurfaceCapture(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

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

    client->TakeSurfaceCapture(nodeId, callback, captureConfig);
    return true;
}

bool DoSetFocusAppInfo(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    int32_t pid = GetData<int32_t>();
    int32_t uid = GetData<int32_t>();
    std::string bundleName = "bundleName";
    std::string abilityName = "abilityName";
    uint64_t focusNodeId = GetData<uint64_t>();

    client->SetFocusAppInfo(pid, uid, bundleName, abilityName, focusNodeId);
    return true;
}

bool DoGetDefaultScreenId(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    client->GetDefaultScreenId();
    return true;
}

bool DoGetActiveScreenId(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    client->GetActiveScreenId();
    return true;
}

bool DoGetAllScreenIds(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    client->GetAllScreenIds();
    return true;
}

bool DoCreateVirtualScreen(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
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

bool DoSetVirtualScreenBlackList(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    std::vector<NodeId> blackListVector;
    client->SetVirtualScreenBlackList(id, blackListVector);
    return true;
}

bool DoSetWatermark(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
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
    client->SetWatermark(name, watermark);
    return true;
}

bool DoSetVirtualScreenSecurityExemptionList(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    std::vector<NodeId> securityExemptionList;
    client->SetVirtualScreenSecurityExemptionList(id, securityExemptionList);
    return true;
}

bool DoSetCastScreenEnableSkipWindow(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    bool enable = GetData<bool>();
    client->SetCastScreenEnableSkipWindow(id, enable);
    return true;
}

bool DoSetVirtualScreenSurface(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    sptr<Surface> surface;
    client->SetVirtualScreenSurface(id, surface);
    return true;
}

bool DoRemoveVirtualScreen(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    client->RemoveVirtualScreen(id);
    return true;
}

bool DoSetScreenRefreshRate(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    int32_t sceneId = GetData<int32_t>();
    int32_t rate = GetData<int32_t>();
    client->SetScreenRefreshRate(id, sceneId, rate);
    return true;
}

bool DoSetRefreshRateMode(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    int32_t refreshRateMode = GetData<int32_t>();
    client->SetRefreshRateMode(refreshRateMode);
    return true;
}

bool DoSyncFrameRateRange(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    FrameRateLinkerId id = GetData<FrameRateLinkerId>();
    FrameRateRange range;
    int32_t animatorExpectedFrameRate = GetData<int32_t>();
    client->SyncFrameRateRange(id, range, animatorExpectedFrameRate);
    return true;
}

bool DoUnregisterFrameRateLinker(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    FrameRateLinkerId id = GetData<FrameRateLinkerId>();
    client->UnregisterFrameRateLinker(id);
    return true;
}

bool DoGetScreenCurrentRefreshRate(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    client->GetScreenCurrentRefreshRate(id);
    return true;
}

bool DoGetCurrentRefreshRateMode(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    client->GetCurrentRefreshRateMode();
    return true;
}

bool DoGetScreenSupportedRefreshRates(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    client->GetScreenSupportedRefreshRates(id);
    return true;
}

bool DoGetShowRefreshRateEnabled(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    client->GetShowRefreshRateEnabled();
    return true;
}

bool DoGetRefreshInfo(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    pid_t pid = GetData<pid_t>();
    client->GetRefreshInfo(pid);
    return true;
}

bool DoSetShowRefreshRateEnabled(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    bool enable = GetData<bool>();
    client->SetShowRefreshRateEnabled(enable);
    return true;
}

bool DoSetVirtualScreenResolution(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    client->SetVirtualScreenResolution(id, width, height);
    return true;
}

bool DoGetVirtualScreenResolution(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    client->GetVirtualScreenResolution(id);
    return true;
}

bool DoMarkPowerOffNeedProcessOneFrame(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    client->MarkPowerOffNeedProcessOneFrame();
    return true;
}

bool DoDisablePowerOffRenderControl(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    client->DisablePowerOffRenderControl(id);
    return true;
}

bool DoSetScreenPowerStatus(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    ScreenPowerStatus status = ScreenPowerStatus::INVALID_POWER_STATUS;
    client->SetScreenPowerStatus(id, status);
    return true;
}

bool DoGetScreenActiveMode(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    client->GetScreenActiveMode(id);
    return true;
}

bool DoGetScreenSupportedModes(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    uint32_t level = GetData<uint32_t>();
    client->GetScreenSupportedModes(id);
    client->GetScreenCapability(id);
    client->GetScreenPowerStatus(id);
    client->GetScreenData(id);
    client->GetScreenBacklight(id);
    client->SetScreenBacklight(id, level);

    return true;
}

bool DoRegisterBufferAvailableListener(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    NodeId id = GetData<ScreenId>();
    BufferAvailableCallback callback;
    bool isFromRenderThread = GetData<bool>();
    client->RegisterBufferAvailableListener(id, callback, isFromRenderThread);
    client->UnregisterBufferAvailableListener(id);
    return true;
}

bool DoRegisterBufferClearListener(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    NodeId id = GetData<ScreenId>();
    BufferClearCallback callback;
    client->RegisterBufferClearListener(id, callback);
    return true;
}

bool DoGetScreenSupportedColorGamuts(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    std::vector<ScreenColorGamut> mode;
    client->GetScreenSupportedColorGamuts(id, mode);
    return true;
}

bool DoGetScreenSupportedMetaDataKeys(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    std::vector<ScreenHDRMetadataKey> keys;
    client->GetScreenSupportedMetaDataKeys(id, keys);
    return true;
}

bool DoGetScreenColorGamut(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    ScreenColorGamut mode = ScreenColorGamut::COLOR_GAMUT_INVALID;
    int32_t modeIdx = GetData<int32_t>();
    client->SetScreenColorGamut(id, modeIdx);
    client->GetScreenColorGamut(id, mode);
    return true;
}

bool DoSetScreenGamutMap(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    ScreenGamutMap mode = ScreenGamutMap::GAMUT_MAP_CONSTANT;
    client->SetScreenGamutMap(id, mode);
    client->GetScreenGamutMap(id, mode);
    return true;
}

bool DoSetScreenCorrection(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    ScreenRotation screenRotation = ScreenRotation::ROTATION_0;
    client->SetScreenCorrection(id, screenRotation);
    return true;
}

bool DoSetVirtualMirrorScreenCanvasRotation(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    bool canvasRotation = GetData<bool>();
    client->SetVirtualMirrorScreenCanvasRotation(id, canvasRotation);
    return true;
}

bool DoSetVirtualMirrorScreenScaleMode(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    ScreenScaleMode scaleMode = ScreenScaleMode::FILL_MODE;
    client->SetVirtualMirrorScreenScaleMode(id, scaleMode);
    return true;
}

bool DoSetGlobalDarkColorMode(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    bool isDark = GetData<bool>();
    client->SetGlobalDarkColorMode(isDark);
    return true;
}

bool DoGetScreenHDRCapability(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    RSScreenHDRCapability screenHdrCapability;
    client->GetScreenHDRCapability(id, screenHdrCapability);
    return true;
}

bool DoGetPixelFormat(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    GraphicPixelFormat pixelFormat = GRAPHIC_PIXEL_FMT_BGRA_8888;
    client->SetPixelFormat(id, pixelFormat);
    client->GetPixelFormat(id, pixelFormat);
    return true;
}

bool DoGetScreenSupportedHDRFormats(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    std::vector<ScreenHDRFormat> hdrFormats;
    ScreenHDRFormat hdrFormat = ScreenHDRFormat::VIDEO_HLG;
    int32_t modeIdx = GetData<int32_t>();
    
    client->SetScreenHDRFormat(id, modeIdx);
    client->GetScreenHDRFormat(id, hdrFormat);
    client->GetScreenSupportedHDRFormats(id, hdrFormats);
    return true;
}

bool DoGetScreenSupportedColorSpaces(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    std::vector<GraphicCM_ColorSpaceType> colorSpaces;
    GraphicCM_ColorSpaceType colorSpace = GraphicCM_ColorSpaceType::GRAPHIC_CM_BT601_EBU_FULL;

    client->SetScreenColorSpace(id, colorSpace);
    client->GetScreenColorSpace(id, colorSpace);
    client->GetScreenSupportedColorSpaces(id, colorSpaces);
    return true;
}

bool DoGetScreenType(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    RSScreenType screenType = RSScreenType::BUILT_IN_TYPE_SCREEN;
    client->GetScreenType(id, screenType);
    return true;
}

bool DoGetBitmap(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    Drawing::Bitmap bm;
    NodeId id = GetData<uint64_t>();
    client->GetBitmap(id, bm);
    return true;
}

bool DoSetVirtualScreenRefreshRate(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    uint32_t maxRefreshRate = GetData<uint32_t>();
    uint32_t actualRefreshRate = GetData<uint32_t>();
    client->SetVirtualScreenRefreshRate(id, maxRefreshRate, actualRefreshRate);
    return true;
}

bool DoRegisterOcclusionChangeCallback(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    OcclusionChangeCallback callback;
    client->RegisterOcclusionChangeCallback(callback);
    return true;
}

bool DoRegisterSurfaceOcclusionChangeCallback(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    SurfaceOcclusionChangeCallback callback;
    NodeId id = GetData<NodeId>();
    std::vector<float> partitionPoints;
    client->RegisterSurfaceOcclusionChangeCallback(id, callback, partitionPoints);
    client->UnRegisterSurfaceOcclusionChangeCallback(id);
    return true;
}

bool DoRegisterHgmConfigChangeCallback(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    HgmConfigChangeCallback callback;
    client->RegisterHgmConfigChangeCallback(callback);
    HgmRefreshRateModeChangeCallback callback2;
    client->RegisterHgmRefreshRateModeChangeCallback(callback2);
    HgmRefreshRateUpdateCallback callback3;
    client->RegisterHgmRefreshRateUpdateCallback(callback3);

    return true;
}

bool DoSetAppWindowNum(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    uint32_t num = GetData<uint32_t>();
    client->SetAppWindowNum(num);

    return true;
}

bool DoSetSystemAnimatedScenes(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    SystemAnimatedScenes systemAnimatedScenes = SystemAnimatedScenes::ENTER_MISSION_CENTER;
    client->SetSystemAnimatedScenes(systemAnimatedScenes);
    return true;
}

bool DoShowWatermark(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
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
    client->ShowWatermark(watermarkImg, isShow);
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

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    client->ResizeVirtualScreen(id, width, height);
    return true;
}

bool DoReportJankStats(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    client->ReportJankStats();
    return true;
}

bool DoReportEventResponse(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
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
    client->ReportEventResponse(info);
    client->ReportEventComplete(info);
    client->ReportEventJankFrame(info);
    return true;
}

bool DoReportGameStateData(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    GameStateData info = {
        .pid = GetData<int32_t>(),
        .uid = GetData<int32_t>(),
        .state = GetData<int32_t>(),
        .renderTid = GetData<int32_t>(),
        .bundleName = "bundleName",
    };
    client->ReportGameStateData(info);

    return true;
}

bool DoSetHardwareEnabled(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    NodeId id = GetData<NodeId>();
    bool isEnabled = GetData<bool>();
    SelfDrawingNodeType selfDrawingType = SelfDrawingNodeType::DEFAULT;
    bool dynamicHardwareEnable = GetData<bool>();
    client->SetHardwareEnabled(id, isEnabled, selfDrawingType, dynamicHardwareEnable);

    return true;
}

bool DoSetHidePrivacyContent(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    NodeId id = GetData<NodeId>();
    bool needHidePrivacyContent = GetData<bool>();
    client->SetHidePrivacyContent(id, needHidePrivacyContent);
    return true;
}

bool DoNotifyLightFactorStatus(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    bool isSafe = GetData<bool>();
    client->NotifyLightFactorStatus(isSafe);
    return true;
}

bool DoNotifyPackageEvent(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    uint32_t listSize = GetData<uint32_t>();
    std::vector<std::string> packageList;
    client->NotifyPackageEvent(listSize, packageList);
    return true;
}

bool DoNotifyRefreshRateEvent(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    EventInfo eventInfo = {
        .eventName = "eventName",
        .eventStatus = GetData<bool>(),
        .minRefreshRate = GetData<uint32_t>(),
        .maxRefreshRate = GetData<uint32_t>(),
        .description = "description",
    };
    client->NotifyRefreshRateEvent(eventInfo);
    return true;
}

bool DoNotifyTouchEvent(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    int32_t touchStatus = GetData<int32_t>();
    int32_t touchCnt = GetData<int32_t>();
    bool enableDynamicMode = GetData<bool>();
    client->NotifyTouchEvent(touchStatus, touchCnt);
    client->NotifyDynamicModeEvent(enableDynamicMode);
    return true;
}

bool DoSetCacheEnabledForRotation(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    bool isEnabled = GetData<bool>();
    uint32_t offset = GetData<uint32_t>();
    client->SetCacheEnabledForRotation(isEnabled);
    client->SetDefaultDeviceRotationOffset(offset);
    return true;
}

bool DoSetOnRemoteDiedCallback(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    OnRemoteDiedCallback callback;
    client->SetOnRemoteDiedCallback(callback);
    return true;
}

bool DoGetActiveDirtyRegionInfo(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    client->GetActiveDirtyRegionInfo();
    client->GetGlobalDirtyRegionInfo();
    client->GetLayerComposeInfo();
    client->GetHwcDisabledReasonInfo();
    return true;
}

bool DoSetVmaCacheStatus(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    bool flag = GetData<bool>();
    bool isVirtualScreenUsingStatus = GetData<bool>();
    bool isCurtainScreenOn = GetData<bool>();
    client->SetVmaCacheStatus(flag);
    client->SetVirtualScreenUsingStatus(isVirtualScreenUsingStatus);
    client->SetCurtainScreenUsingStatus(isCurtainScreenOn);
    return true;
}

bool DoRegisterUIExtensionCallback(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    uint64_t userId = GetData<uint64_t>();
    UIExtensionCallback callback;
    client->RegisterUIExtensionCallback(userId, callback);
    return true;
}

bool DoSetAncoForceDoDirect(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    bool direct = GetData<bool>();
    client->SetAncoForceDoDirect(direct);
    return true;
}

bool DoSetVirtualScreenStatus(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    VirtualScreenStatus screenStatus = VirtualScreenStatus::VIRTUAL_SCREEN_PLAY;
    client->SetVirtualScreenStatus(id, screenStatus);
    return true;
}

bool DoRegisterSurfaceBufferCallback(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    pid_t pid = GetData<pid_t>();
    uint64_t uid = GetData<uint64_t>();
    std::shared_ptr<SurfaceBufferCallback> callback;
    client->RegisterSurfaceBufferCallback(pid, uid, callback);
    client->UnregisterSurfaceBufferCallback(pid, uid);
    return true;
}

bool DoTriggerSurfaceBufferCallback(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    uint64_t uid = GetData<uint64_t>();
    std::vector<uint32_t> surfaceBufferIds;
    client->TriggerSurfaceBufferCallback(uid, surfaceBufferIds);
    return true;
}

bool DoSetLayerTop(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    std::string nodeIdStr = "nodeIdStr";
    bool isTop = GetData<bool>();
    client->SetLayerTop(nodeIdStr, isTop);
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoExecuteSynchronousTask(data, size);
    OHOS::Rosen::DoGetMemoryGraphic(data, size);
    OHOS::Rosen::DoGetMemoryGraphics(data, size);
    OHOS::Rosen::DoGetTotalAppMemSize(data, size);
    OHOS::Rosen::DoCreateNode(data, size);
    OHOS::Rosen::DoCreateNodeAndSurface(data, size);
    OHOS::Rosen::DoCreateRSSurface(data, size);
    OHOS::Rosen::DoCreateVSyncReceiver(data, size);
    OHOS::Rosen::DoCreatePixelMapFromSurfaceId(data, size);
    OHOS::Rosen::DoTriggerSurfaceCaptureCallback(data, size);
    OHOS::Rosen::DoTakeSurfaceCapture(data, size);
    OHOS::Rosen::DoSetFocusAppInfo(data, size);
    OHOS::Rosen::DoGetDefaultScreenId(data, size);
    OHOS::Rosen::DoGetActiveScreenId(data, size);
    OHOS::Rosen::DoGetAllScreenIds(data, size);
    OHOS::Rosen::DoCreateVirtualScreen(data, size);
    OHOS::Rosen::DoSetVirtualScreenBlackList(data, size);
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
    OHOS::Rosen::DoGetCurrentRefreshRateMode(data, size);
    OHOS::Rosen::DoGetScreenSupportedRefreshRates(data, size);
    OHOS::Rosen::DoGetShowRefreshRateEnabled(data, size);
    OHOS::Rosen::DoGetRefreshInfo(data, size);
    OHOS::Rosen::DoSetShowRefreshRateEnabled(data, size);
    OHOS::Rosen::DoSetVirtualScreenResolution(data, size);
    OHOS::Rosen::DoGetVirtualScreenResolution(data, size);
    OHOS::Rosen::DoMarkPowerOffNeedProcessOneFrame(data, size);
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
    OHOS::Rosen::DoRegisterOcclusionChangeCallback(data, size);
    OHOS::Rosen::DoRegisterSurfaceOcclusionChangeCallback(data, size);
    OHOS::Rosen::DoRegisterHgmConfigChangeCallback(data, size);
    OHOS::Rosen::DoSetSystemAnimatedScenes(data, size);
    OHOS::Rosen::DoShowWatermark(data, size);
    OHOS::Rosen::DoResizeVirtualScreen(data, size);
    OHOS::Rosen::DoReportJankStats(data, size);
    OHOS::Rosen::DoReportEventResponse(data, size);
    OHOS::Rosen::DoReportGameStateData(data, size);
    OHOS::Rosen::DoSetHardwareEnabled(data, size);
    OHOS::Rosen::DoSetHidePrivacyContent(data, size);
    OHOS::Rosen::DoNotifyLightFactorStatus(data, size);
    OHOS::Rosen::DoNotifyPackageEvent(data, size);
    OHOS::Rosen::DoNotifyRefreshRateEvent(data, size);
    OHOS::Rosen::DoNotifyTouchEvent(data, size);
    OHOS::Rosen::DoSetCacheEnabledForRotation(data, size);
    OHOS::Rosen::DoSetOnRemoteDiedCallback(data, size);
    OHOS::Rosen::DoGetActiveDirtyRegionInfo(data, size);
    OHOS::Rosen::DoSetVmaCacheStatus(data, size);
    OHOS::Rosen::DoRegisterUIExtensionCallback(data, size);
    OHOS::Rosen::DoSetAncoForceDoDirect(data, size);
    OHOS::Rosen::DoSetVirtualScreenStatus(data, size);
    OHOS::Rosen::DoRegisterSurfaceBufferCallback(data, size);
    OHOS::Rosen::DoTriggerSurfaceBufferCallback(data, size);
    OHOS::Rosen::DoSetLayerTop(data, size);
    return 0;
}