/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "rsrenderserviceconnectionstub_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <unistd.h>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>

#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_render_service_connection.h"
#include "platform/ohos/rs_irender_service.h"
#include "transaction/rs_render_service_connection_stub.h"
#include "transaction/rs_transaction_proxy.h"
#include "message_parcel.h"
#include "securec.h"

namespace OHOS {
namespace Rosen {
constexpr size_t MAX_SIZE = 4;
constexpr size_t SCREEN_WIDTH = 100;
constexpr size_t SCREEN_HEIGHT = 100;
constexpr size_t SCREEN_REFRESH_RATE = 60;
const std::u16string RENDERSERVICECONNECTION_INTERFACE_TOKEN = u"ohos.rosen.RenderServiceConnection";
static std::shared_ptr<RSRenderServiceClient> rsClient = std::make_shared<RSRenderServiceClient>();

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

bool DoCommitTransaction(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto transactionData = std::make_unique<RSTransactionData>();
    rsClient->CommitTransaction(transactionData);
    return true;
}

bool DoExecuteSynchronousTask(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    auto property = std::make_shared<RSRenderAnimatableProperty<float>>(0.0f, 0, RSRenderPropertyType::PROPERTY_FLOAT);
    auto task = std::make_shared<RSNodeGetShowingPropertyAndCancelAnimation>(0, property);
    RSTransactionProxy::GetInstance()->ExecuteSynchronousTask(task, true);
    return true;
}

bool DoCreateNodeAndSurface(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    RSSurfaceRenderNodeConfig config = { .id = 0, .name = "test", .bundleName = "test" };
    rsClient->CreateNode(config);
    rsClient->CreateNodeAndSurface(config);
    return true;
}

bool DoSetFocusAppInfo(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    MessageParcel datas;
    datas.WriteBuffer(data, size);
    int32_t pid = datas.ReadInt32();
    int32_t uid = datas.ReadInt32();
    std::string bundleName = datas.ReadString();
    std::string abilityName = datas.ReadString();
    uint64_t focusNodeId = datas.ReadUint64();
    rsClient->SetFocusAppInfo(pid, uid, bundleName, abilityName, focusNodeId);
    return true;
}

bool DoCreateVirtualScreen(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("DisplayNode");
    sptr<IBufferProducer> bp = cSurface->GetProducer();
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(bp);
    rsClient->CreateVirtualScreen("name", SCREEN_WIDTH, SCREEN_HEIGHT, pSurface, 0, 1);
    rsClient->SetVirtualScreenSurface(0, pSurface);
    rsClient->RemoveVirtualScreen(0);
    return true;
}

bool DoSetScreenChangeCallback(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    static ScreenId screenId = INVALID_SCREEN_ID;
    static ScreenEvent screenEvent = ScreenEvent::UNKNOWN;
    static bool callbacked = false;
    auto callback = [](ScreenId id, ScreenEvent event) {
        screenId = id;
        screenEvent = event;
        callbacked = true;
    };
    rsClient->SetScreenChangeCallback(callback);
    return true;
}

bool DoSetScreenActiveMode(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    ScreenId id = GetData<ScreenId>();
    uint32_t modeId = GetData<uint32_t>();
    rsClient->SetScreenActiveMode(id, modeId);
    rsClient->GetScreenActiveMode(id);
    return true;
}

bool DoSetScreenRefreshRate(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    ScreenId id = *(reinterpret_cast<const uint32_t*>(data));
    rsClient->SetScreenRefreshRate(id, 0, SCREEN_REFRESH_RATE);
    rsClient->GetScreenCurrentRefreshRate(id);
    rsClient->GetScreenSupportedRefreshRates(id);
    return true;
}

bool DoSetRefreshRateMode(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    rsClient->SetRefreshRateMode(0);
    return true;
}

bool DoSetVirtualScreenResolution(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    ScreenId id = *(reinterpret_cast<const uint32_t*>(data));
    rsClient->SetVirtualScreenResolution(id, SCREEN_WIDTH, SCREEN_HEIGHT);
    rsClient->GetVirtualScreenResolution(id);
    return true;
}

bool DoSetScreenPowerStatus(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    ScreenId id = *(reinterpret_cast<const uint32_t*>(data));
    rsClient->SetScreenPowerStatus(id, static_cast<ScreenPowerStatus>(0));
    rsClient->GetScreenPowerStatus(id);
    return true;
}

class TestSurfaceCaptureCallback : public SurfaceCaptureCallback {
public:
    explicit TestSurfaceCaptureCallback() {}
    ~TestSurfaceCaptureCallback() override {}
    void OnSurfaceCapture(std::shared_ptr<Media::PixelMap> pixelmap) override {}
};

bool DoTakeSurfaceCapture(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto nodeId = GetData<NodeId>();
    float scaleX = GetData<float>();
    float scaleY = GetData<float>();

    std::shared_ptr<TestSurfaceCaptureCallback> cb = std::make_shared<TestSurfaceCaptureCallback>();
    rsClient->TakeSurfaceCapture(nodeId, cb, scaleX, scaleY);
    return true;
}

bool DoGetScreenSupportedModes(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    ScreenId id = GetData<ScreenId>();
    rsClient->GetScreenSupportedModes(id);
    return true;
}

bool DoGetMemoryGraphic(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    int pid = GetData<int>();
    rsClient->GetMemoryGraphic(pid);
    return true;
}

bool DoGetScreenCapability(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    ScreenId id = GetData<ScreenId>();
    rsClient->GetScreenCapability(id);
    return true;
}

bool DoGetScreenData(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    ScreenId id = GetData<ScreenId>();
    rsClient->GetScreenData(id);
    return true;
}

bool DoGetScreenBacklight(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    ScreenId id = GetData<ScreenId>();
    uint32_t level = GetData<uint32_t>();
    rsClient->SetScreenBacklight(id, level);
    rsClient->GetScreenBacklight(id);
    return true;
}

bool DoRegisterBufferAvailableListener(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    NodeId id = GetData<NodeId>();
    bool isFromRenderThread = GetData<bool>();
    BufferAvailableCallback cb = []() {};
    rsClient->RegisterBufferAvailableListener(id, cb, isFromRenderThread);
    return true;
}

bool DoGetScreenSupportedColorGamuts(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    std::vector<ScreenColorGamut> mode;
    rsClient->GetScreenSupportedColorGamuts(0, mode);

    std::vector<ScreenHDRMetadataKey> keys;
    rsClient->GetScreenSupportedMetaDataKeys(0, keys);
    return true;
}

bool DoGetScreenColorGamut(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    ScreenId id = GetData<ScreenId>();
    int32_t modeIdx = GetData<int32_t>();
    rsClient->SetScreenColorGamut(id, modeIdx);
    ScreenColorGamut mode;
    rsClient->GetScreenColorGamut(id, mode);
    return true;
}

bool DoSetScreenGamutMap(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    ScreenId id = *(reinterpret_cast<const uint32_t*>(data));
    ScreenGamutMap mapMode = ScreenGamutMap::GAMUT_MAP_CONSTANT;
    rsClient->SetScreenGamutMap(id, mapMode);
    rsClient->GetScreenGamutMap(id, mapMode);
    return true;
}

bool DoCreateVSyncConnection(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    std::shared_ptr<AppExecFwk::EventHandler> handler = nullptr;
    rsClient->CreateVSyncReceiver("test", handler);
    return true;
}

bool DoGetScreenHDRCapability(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    ScreenId id = *(reinterpret_cast<const uint32_t*>(data));
    RSScreenHDRCapability screenHDRCapability;
    rsClient->GetScreenHDRCapability(id, screenHDRCapability);
    return true;
}

bool DoGetScreenType(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    ScreenId id = *(reinterpret_cast<const uint32_t*>(data));
    RSScreenType type;
    rsClient->GetScreenType(id, type);
    return true;
}

bool DoGetBitmap(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    Drawing::Bitmap bm;
    rsClient->GetBitmap(0, bm);
    return true;
}

bool DoSetScreenSkipFrameInterval(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    ScreenId id = *(reinterpret_cast<const uint32_t*>(data));
    uint32_t skipFrameInterval = GetData<uint32_t>();
    rsClient->SetScreenSkipFrameInterval(id, skipFrameInterval);
    return true;
}

bool DoRegisterOcclusionChangeCallback(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    OcclusionChangeCallback cb = [](std::shared_ptr<RSOcclusionData> data) {};
    rsClient->RegisterOcclusionChangeCallback(cb);
    return true;
}

bool DoSetAppWindowNum(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t num = GetData<uint32_t>();
    rsClient->SetAppWindowNum(num);
    return true;
}

bool DoShowWatermark(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    bool isShow = GetData<bool>();
    std::shared_ptr<Media::PixelMap> pixelMap1;
    rsClient->ShowWatermark(pixelMap1, isShow);
    return true;
}

bool DoReportEventResponse(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    DataBaseRs info;
    rsClient->ReportEventResponse(info);
    rsClient->ReportEventComplete(info);
    rsClient->ReportEventJankFrame(info);
    return true;
}

bool DoSetHardwareEnabled(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto isEnabled = GetData<bool>();
    rsClient->SetHardwareEnabled(0, isEnabled, SelfDrawingNodeType::DEFAULT);
    return true;
}

bool DoRegisterHgmConfigChangeCallback(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    HgmConfigChangeCallback cb2 = [](std::shared_ptr<RSHgmConfigData> data) {};
    rsClient->RegisterHgmConfigChangeCallback(cb2);
    return true;
}

bool DoRegisterHgmRefreshRateModeChangeCallback(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    HgmRefreshRateModeChangeCallback cb = [](int32_t refreshRateMode) {};
    rsClient->RegisterHgmRefreshRateModeChangeCallback(cb);
    return true;
}
} // Rosen
} // OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoCommitTransaction(data, size);
    OHOS::Rosen::DoExecuteSynchronousTask(data, size);
    OHOS::Rosen::DoCreateNodeAndSurface(data, size);
    OHOS::Rosen::DoSetFocusAppInfo(data, size);
    OHOS::Rosen::DoCreateVirtualScreen(data, size);
    OHOS::Rosen::DoSetScreenChangeCallback(data, size);
    OHOS::Rosen::DoSetScreenActiveMode(data, size);
    OHOS::Rosen::DoSetScreenRefreshRate(data, size);
    OHOS::Rosen::DoSetRefreshRateMode(data, size);
    OHOS::Rosen::DoSetVirtualScreenResolution(data, size);
    OHOS::Rosen::DoSetScreenPowerStatus(data, size);
    OHOS::Rosen::DoTakeSurfaceCapture(data, size);
    OHOS::Rosen::DoGetScreenSupportedModes(data, size);
    OHOS::Rosen::DoGetMemoryGraphic(data, size);
    OHOS::Rosen::DoGetScreenCapability(data, size);
    OHOS::Rosen::DoGetScreenData(data, size);
    OHOS::Rosen::DoGetScreenBacklight(data, size);
    OHOS::Rosen::DoRegisterBufferAvailableListener(data, size);
    OHOS::Rosen::DoGetScreenSupportedColorGamuts(data, size);
    OHOS::Rosen::DoGetScreenColorGamut(data, size);
    OHOS::Rosen::DoSetScreenGamutMap(data, size);
    OHOS::Rosen::DoCreateVSyncConnection(data, size);
    OHOS::Rosen::DoGetScreenHDRCapability(data, size);
    OHOS::Rosen::DoGetScreenType(data, size);
    OHOS::Rosen::DoGetBitmap(data, size);
    OHOS::Rosen::DoSetScreenSkipFrameInterval(data, size);
    OHOS::Rosen::DoRegisterOcclusionChangeCallback(data, size);
    OHOS::Rosen::DoSetAppWindowNum(data, size);
    OHOS::Rosen::DoShowWatermark(data, size);
    OHOS::Rosen::DoReportEventResponse(data, size);
    OHOS::Rosen::DoSetHardwareEnabled(data, size);
    OHOS::Rosen::DoRegisterHgmConfigChangeCallback(data, size);
    OHOS::Rosen::DoRegisterHgmRefreshRateModeChangeCallback(data, size);
    return 0;
}