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

#include "cltrenderserviceclient_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "transaction/rs_render_service_client.h"
#include "command/rs_command.h"
#include "command/rs_node_showing_command.h"
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
#include "ipc_callbacks/pointer_render/pointer_luminance_callback_stub.h"
#endif
#include "ipc_callbacks/screen_change_callback_stub.h"
#include "platform/ohos/rs_render_service_connect_hub.h"

namespace OHOS {
namespace Rosen {

namespace {
const uint8_t* DATA = nullptr;
size_t g_size = 0;
size_t g_pos;
} // namespace

/*
 * describe: get data from outside untrusted data(DATA) which size is according to sizeof(T)
 * tips: only support basic type
 */
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
 * get a string from DATA
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
bool DoExecuteSynchronous(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSIRenderClient> clients = std::make_shared<RSRenderServiceClient>();
    uint64_t timeoutNS = GetData<uint64_t>();
    auto task = std::make_shared<DerivedSyncTask>(timeoutNS);
    clients->ExecuteSynchronousTask(task);
    
    return true;
}

bool DoGetMemoryGraphic(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> clients = std::make_shared<RSRenderServiceClient>();
    int pid = GetData<int>();
    clients->GetMemoryGraphic(pid);

    return true;
}

bool DoCreateRSSurface(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> clients = std::make_shared<RSRenderServiceClient>();
    sptr<Surface> surface;
    clients->CreateRSSurface(surface);
    return true;
}

bool DoCreateVSyncReceiver(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> clients = std::make_shared<RSRenderServiceClient>();
    std::string name = "fuzztest";
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> looper;
    uint64_t id = GetData<uint64_t>();
    NodeId windowNodeId = GetData<uint64_t>();
    clients->CreateVSyncReceiver(name, looper, id, windowNodeId);
    return true;
}

bool DoGetMemoryGraphics(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> clients = std::make_shared<RSRenderServiceClient>();
    clients->GetMemoryGraphics();
    return true;
}

bool DoGetTotalAppMemSize(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> clients = std::make_shared<RSRenderServiceClient>();
    float cpuMemSize = GetData<int>();
    float gpuMemSize  = GetData<int>();
    clients->GetTotalAppMemSize(cpuMemSize, gpuMemSize);
    return true;
}

bool DoCreateNode(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> clients = std::make_shared<RSRenderServiceClient>();
    RSDisplayNodeConfig displayNodeconfig = {
        .screen = GetData<uint64_t>(),
        .isMirrored = GetData<bool>(),
        .mirrorNodeId = GetData<NodeId>(),
        .isSync = GetData<bool>(),
    };
    NodeId nodeIdx = GetData<NodeId>();
    clients->CreateNode(displayNodeconfig, nodeIdx);

    RSSurfaceRenderNodeConfig config = { .id = GetData<NodeId>(), .name = "fuzztest" };
    clients->CreateNode(config);
    return true;
}

bool DoCreateNodeAndSurface(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> clients = std::make_shared<RSRenderServiceClient>();
    RSSurfaceRenderNodeConfig config = { .id = GetData<NodeId>(), .name = "fuzztest" };
    clients->CreateNodeAndSurface(config);
    return true;
}

bool DoCreatePixelMapFromSurfaceId(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> clients = std::make_shared<RSRenderServiceClient>();
    uint64_t surfaceId = GetData<uint64_t>();
    Rect srcRect = {0, 0, 10, 10};
    clients->CreatePixelMapFromSurfaceId(surfaceId, srcRect);
    return true;
}

bool DoTriggerSurfaceCaptureCallback(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> clients = std::make_shared<RSRenderServiceClient>();
    NodeId id = GetData<NodeId>();
    int32_t width = GetData<int32_t>();
    int32_t height = GetData<int32_t>();
    Media::InitializationOptions optss;
    optss.size.width = width;
    optss.size.height = height;
    optss.srcPixelFormat = static_cast<Media::PixelFormat>(GetData<int32_t>());
    optss.pixelformat = static_cast<Media::PixelFormat>(GetData<int32_t>());
    optss.alphaType = static_cast<Media::AlphaType>(GetData<int32_t>());
    optss.scaleMode = static_cast<Media::ScaleMode>(GetData<int32_t>());
    optss.editable = GetData<bool>();
    optss.useSourceIfMatch = GetData<bool>();
    std::shared_ptr<Media::PixelMap>  pixelmap = Media::PixelMap::Create(optss);

    clients->TriggerSurfaceCaptureCallback(id, pixelmap);
    return true;
}

bool DoTakeSurfaceCapture(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> clients = std::make_shared<RSRenderServiceClient>();
    uint64_t nodeIdx = GetData<uint64_t>();
    std::shared_ptr<SurfaceCaptureCallback> callback;
    RSSurfaceconfig config;
    config.scaleX = GetData<float>();
    config.scaleY = GetData<float>();
    config.useDma = GetData<bool>();
    config.useCurWindow = GetData<bool>();
    uint8_t type = GetData<uint8_t>();
    config.captureType = (SurfaceCaptureType)type;
    config.isSync = GetData<bool>();

    clients->TakeSurfaceCapture(nodeIdx, callback, config);
    return true;
}

bool DoSetVirtualScreenBlackList(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> clients = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    std::vector<NodeId> blackListVector;
    clients->SetVirtualScreenBlackList(id, blackListVector);
    return true;
}

bool DoDropFrameByPid(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    std::vector<int32_t> pidList;
    uint8_t pidListSize = GetData<uint8_t>();
    for (size_t i = 0; i < pidListSize; i++) {
        pidList.push_back(GetData<int32_t>());
    }

    std::shared_ptr<RSRenderServiceClient> clients = std::make_shared<RSRenderServiceClient>();
    clients->DropFrameByPid(pidList);
    return true;
}

bool DoSetWatermark(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> clients = std::make_shared<RSRenderServiceClient>();
    std::string name = "name";

    Media::InitializationOptions optss;
    optss.size.width = GetData<int32_t>();
    optss.size.height = GetData<int32_t>();
    optss.srcPixelFormat = static_cast<Media::PixelFormat>(GetData<int32_t>());
    optss.pixelformat = static_cast<Media::PixelFormat>(GetData<int32_t>());
    optss.alphaType = static_cast<Media::AlphaType>(GetData<int32_t>());
    optss.scaleMode = static_cast<Media::ScaleMode>(GetData<int32_t>());
    optss.editable = GetData<bool>();
    optss.useSourceIfMatch = GetData<bool>();
    std::shared_ptr<Media::PixelMap>  watermark = Media::PixelMap::Create(optss);
    clients->SetWatermark(name, watermark);
    return true;
}

bool DoSetFocusAppInfo(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> clients = std::make_shared<RSRenderServiceClient>();
    int32_t pid = GetData<int32_t>();
    int32_t uid = GetData<int32_t>();
    std::string bundleName = "bundleName";
    std::string abilityName = "abilityName";
    uint64_t focusNodeId = GetData<uint64_t>();

    clients->SetFocusAppInfo(pid, uid, bundleName, abilityName, focusNodeId);
    return true;
}

bool DoGetDefaultScreenId(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> clients = std::make_shared<RSRenderServiceClient>();
    clients->GetDefaultScreenId();
    return true;
}

bool DoGetActiveScreenId(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> clients = std::make_shared<RSRenderServiceClient>();
    clients->GetActiveScreenId();
    return true;
}

bool DoGetAllScreenIds(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> clients = std::make_shared<RSRenderServiceClient>();
    clients->GetAllScreenIds();
    return true;
}

bool DoCreateVirtualScreen(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> clients = std::make_shared<RSRenderServiceClient>();
    std::string name = "name";
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("DisplayNode");
    sptr<IBufferProducer> bp = cSurface->GetProducer();
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(bp);
    ScreenId mirrorId = GetData<ScreenId>();
    int32_t flags = GetData<int32_t>();
    std::vector<NodeId> whiteList;
    clients->CreateVirtualScreen(name, width, height, pSurface, mirrorId, flags, whiteList);
    return true;
}

bool DoSetVirtualScreenSurface(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> clients = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    sptr<Surface> surface;
    clients->SetVirtualScreenSurface(id, surface);
    return true;
}

} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoExecuteSynchronous(data, size);
    OHOS::Rosen::DoGetMemoryGraphics(data, size);
    OHOS::Rosen::DoGetMemoryGraphic(data, size);
    OHOS::Rosen::DoGetTotalAppMemSize(data, size);
    OHOS::Rosen::DoCreateRSSurface(data, size);
    OHOS::Rosen::DoCreateVSyncReceiver(data, size);
    OHOS::Rosen::DoCreatePixelMapFromSurfaceId(data, size);
    OHOS::Rosen::DoCreateNode(data, size);
    OHOS::Rosen::DoCreateNodeAndSurface(data, size);
    OHOS::Rosen::DoTriggerSurfaceCaptureCallback(data, size);
    OHOS::Rosen::DoTakeSurfaceCapture(data, size);
    OHOS::Rosen::DoGetActiveScreenId(data, size);
    OHOS::Rosen::DoGetAllScreenIds(data, size);
    OHOS::Rosen::DoCreateVirtualScreen(data, size);
    OHOS::Rosen::DoSetFocusAppInfo(data, size);
    OHOS::Rosen::DoGetDefaultScreenId(data, size);
    OHOS::Rosen::DoSetVirtualScreenBlackList(data, size);
    OHOS::Rosen::DoDropFrameByPid(data, size);
    OHOS::Rosen::DoSetWatermark(data, size);
    OHOS::Rosen::DoSetVirtualScreenSurface(data, size);
    return 0;
}