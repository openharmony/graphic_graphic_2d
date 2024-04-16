/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "rsrenderserviceconnection_fuzzer.h"
#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>
#include <unordered_map>
#include "ipc_object_proxy.h"
#include "ipc_object_stub.h"
#include "iremote_object.h"
#include "message_parcel.h"
#include "securec.h"
#include "ipc_callbacks/rs_occlusion_change_callback_stub.h"
#include "pipeline/rs_render_service.h"
#include "pipeline/rs_render_service_connection.h"
#include "platform/ohos/rs_render_service_connect_hub.cpp"
#include "screen_manager/rs_screen_manager.h"
#include "transaction/rs_render_service_client.h"
#include "transaction/rs_render_service_connection_stub.h"
#include "transaction/rs_transaction_proxy.h"

namespace OHOS {
namespace Rosen {
constexpr size_t MAX_SIZE = 4;
constexpr size_t SCREEN_WIDTH = 100;
constexpr size_t SCREEN_HEIGHT = 100;
const std::u16string RENDERSERVICECONNECTION_INTERFACE_TOKEN = u"ohos.rosen.RenderServiceConnection";
auto rsConn = RSRenderServiceConnectHub::GetRenderService();

namespace {
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;
} // namespace

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

bool DoRegisterApplicationAgent(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    uint32_t pid = GetData<uint32_t>();
    MessageParcel message;
    auto remoteObject = message.ReadRemoteObject();
    sptr<IApplicationAgent> app = iface_cast<IApplicationAgent>(remoteObject);
    rsConn->RegisterApplicationAgent(pid, app);
    return true;
}

bool DoCommitTransaction(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    auto transactionData = std::make_unique<RSTransactionData>();
    rsConn->CommitTransaction(transactionData);
    return true;
}

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

    if (size < MAX_SIZE) {
        return false;
    }
    uint64_t timeoutNS = GetData<uint64_t>();
    auto task = std::make_shared<DerivedSyncTask>(timeoutNS);
    rsConn->ExecuteSynchronousTask(task);
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

    g_data = data;
    g_size = size;
    g_pos = 0;

    int pid = GetData<int>();
    rsConn->GetMemoryGraphic(pid);
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
    rsConn->CreateNode(config);
    rsConn->CreateNodeAndSurface(config);
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

    ScreenId id = GetData<uint64_t>();
    uint32_t level = GetData<uint32_t>();
    rsConn->SetScreenBacklight(id, level);
    rsConn->GetScreenBacklight(id);
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

    ScreenId id = GetData<uint64_t>();
    uint32_t level = GetData<uint32_t>();
    RSScreenType type = (RSScreenType)level;
    rsConn->GetScreenType(id, type);
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

    uint64_t id = GetData<uint64_t>();
    bool isFromRenderThread = GetData<bool>();
    sptr<RSIBufferAvailableCallback> cb = nullptr;
    rsConn->RegisterBufferAvailableListener(id, cb, isFromRenderThread);
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
    ScreenId id = GetData<uint64_t>();
    uint32_t skipFrameInterval = GetData<uint32_t>();
    rsConn->SetScreenSkipFrameInterval(id, skipFrameInterval);
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

    g_data = data;
    g_size = size;
    g_pos = 0;
    ScreenId id = GetData<uint64_t>();
    rsConn->SetVirtualScreenResolution(id, SCREEN_WIDTH, SCREEN_HEIGHT);
    rsConn->GetVirtualScreenResolution(id);
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
    ScreenId id = GetData<uint64_t>();
    uint32_t screenCol = GetData<uint32_t>();
    mode.push_back((ScreenColorGamut)screenCol);
    rsConn->GetScreenSupportedColorGamuts(id, mode);
    std::vector<ScreenHDRMetadataKey> keys;
    keys.push_back((ScreenHDRMetadataKey)screenCol);
    rsConn->GetScreenSupportedMetaDataKeys(id, keys);
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

    ScreenId id = GetData<uint64_t>();
    rsConn->GetScreenSupportedModes(id);
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

    ScreenId id = GetData<uint64_t>();
    int32_t modeIdx = GetData<int32_t>();
    rsConn->SetScreenColorGamut(id, modeIdx);
    uint32_t mod = GetData<uint32_t>();
    ScreenColorGamut mode = (ScreenColorGamut)mod;
    rsConn->GetScreenColorGamut(id, mode);
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

    ScreenId id = GetData<uint64_t>();
    uint32_t status = GetData<uint32_t>();
    rsConn->SetScreenPowerStatus(id, static_cast<ScreenPowerStatus>(status));
    rsConn->GetScreenPowerStatus(id);
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

    ScreenId id = GetData<uint64_t>();
    uint32_t mapMode = GetData<uint32_t>();
    ScreenGamutMap mode = (ScreenGamutMap)mapMode;
    rsConn->SetScreenGamutMap(id, mode);
    rsConn->GetScreenGamutMap(id, mode);
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
    rsConn->SetAppWindowNum(num);
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
    ScreenId id = GetData<uint64_t>();
    int32_t flags = GetData<int32_t>();
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("DisplayNode");
    sptr<IBufferProducer> bp = cSurface->GetProducer();
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(bp);
    rsConn->CreateVirtualScreen("name", SCREEN_WIDTH, SCREEN_HEIGHT, pSurface, id, flags);
    rsConn->SetVirtualScreenSurface(id, pSurface);
    rsConn->RemoveVirtualScreen(id);
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

    ScreenId id = GetData<uint64_t>();
    uint32_t modeId = GetData<uint32_t>();
    rsConn->SetScreenActiveMode(id, modeId);
    rsConn->GetScreenActiveMode(id);
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
    int32_t refreshRateMode = GetData<int32_t>();
    rsConn->SetRefreshRateMode(refreshRateMode);
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

    uint64_t id = GetData<uint64_t>();
    sptr<VSyncIConnectionToken> token = new IRemoteStub<VSyncIConnectionToken>();
    rsConn->CreateVSyncConnection("test", token, id);
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

    ScreenId id = GetData<uint64_t>();
    int32_t sceneId = GetData<int32_t>();
    int32_t rate = GetData<int32_t>();
    rsConn->SetScreenRefreshRate(id, sceneId, rate);
    rsConn->GetScreenCurrentRefreshRate(id);
    rsConn->GetScreenSupportedRefreshRates(id);
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
    NodeId id = GetData<uint64_t>();
    rsConn->GetBitmap(id, bm);
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

    ScreenId id = GetData<uint64_t>();
    rsConn->GetScreenCapability(id);
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

    ScreenId id = GetData<uint64_t>();
    rsConn->GetScreenData(id);
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

    ScreenId id = GetData<uint64_t>();
    RSScreenHDRCapability screenHDRCapability;
    rsConn->GetScreenHDRCapability(id, screenHDRCapability);
    return true;
}

class CustomOcclusionChangeCallback : public RSOcclusionChangeCallbackStub {
public:
    explicit CustomOcclusionChangeCallback(const OcclusionChangeCallback& callback) : cb_(callback) {}
    ~CustomOcclusionChangeCallback() override {};

    void OnOcclusionVisibleChanged(std::shared_ptr<RSOcclusionData> occlusionData) override
    {
        if (cb_ != nullptr) {
            cb_(occlusionData);
        }
    }

private:
    OcclusionChangeCallback cb_;
};

bool DoRegisterOcclusionChangeCallback(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    OcclusionChangeCallback callback = [](std::shared_ptr<RSOcclusionData> data) {};
    sptr<CustomOcclusionChangeCallback> cb = new CustomOcclusionChangeCallback(callback);
    rsConn->RegisterOcclusionChangeCallback(cb);
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
    rsConn->ShowWatermark(pixelMap1, isShow);
    return true;
}

bool DoTakeSurfaceCapture(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    if (size < MAX_SIZE) {
        return false;
    }

    g_data = data;
    g_size = size;
    g_pos = 0;

    uint64_t nodeId = GetData<uint64_t>();
    float scaleX = GetData<float>();
    float scaleY = GetData<float>();
    sptr<RSISurfaceCaptureCallback> callback = nullptr;
    uint8_t type = GetData<uint8_t>();
    SurfaceCaptureType surfaceCaptureType = (SurfaceCaptureType)type;
    bool isSync = GetData<bool>();
    rsConn->TakeSurfaceCapture(nodeId, callback, scaleX, scaleY, surfaceCaptureType, isSync);
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

    g_data = data;
    g_size = size;
    g_pos = 0;

    sptr<RSIScreenChangeCallback> callback = nullptr;
    rsConn->SetScreenChangeCallback(callback);
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
    rsConn->SetFocusAppInfo(pid, uid, bundleName, abilityName, focusNodeId);
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoRegisterApplicationAgent(data, size);
    OHOS::Rosen::DoCommitTransaction(data, size);
    OHOS::Rosen::DoExecuteSynchronousTask(data, size);
    OHOS::Rosen::DoGetMemoryGraphic(data, size);
    OHOS::Rosen::DoCreateNodeAndSurface(data, size);
    OHOS::Rosen::DoGetScreenBacklight(data, size);
    OHOS::Rosen::DoGetScreenType(data, size);
    OHOS::Rosen::DoRegisterBufferAvailableListener(data, size);
    OHOS::Rosen::DoSetScreenSkipFrameInterval(data, size);
    OHOS::Rosen::DoSetVirtualScreenResolution(data, size);
    OHOS::Rosen::DoGetScreenSupportedColorGamuts(data, size);
    OHOS::Rosen::DoGetScreenSupportedModes(data, size);
    OHOS::Rosen::DoGetScreenColorGamut(data, size);
    OHOS::Rosen::DoSetScreenPowerStatus(data, size);
    OHOS::Rosen::DoSetScreenGamutMap(data, size);
    OHOS::Rosen::DoSetAppWindowNum(data, size);
    OHOS::Rosen::DoCreateVirtualScreen(data, size);
    OHOS::Rosen::DoSetScreenActiveMode(data, size);
    OHOS::Rosen::DoSetRefreshRateMode(data, size);
    OHOS::Rosen::DoCreateVSyncConnection(data, size);
    OHOS::Rosen::DoSetScreenRefreshRate(data, size);
    OHOS::Rosen::DoGetBitmap(data, size);
    OHOS::Rosen::DoGetScreenCapability(data, size);
    OHOS::Rosen::DoGetScreenData(data, size);
    OHOS::Rosen::DoGetScreenHDRCapability(data, size);
    OHOS::Rosen::DoRegisterOcclusionChangeCallback(data, size);
    OHOS::Rosen::DoShowWatermark(data, size);
    OHOS::Rosen::DoTakeSurfaceCapture(data, size);
    OHOS::Rosen::DoSetScreenChangeCallback(data, size);
    OHOS::Rosen::DoSetFocusAppInfo(data, size);
    return 0;
}