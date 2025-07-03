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

#include "rsrenderserviceclient001_fuzzer.h"

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
const uint8_t DO_EXECUTE_SYNCHRONOUS_TASK = 0;
const uint8_t DO_GET_MEMORY_GRAPHIC = 1;
const uint8_t DO_GET_MEMORY_GRAPHICS = 2;
const uint8_t DO_GET_TOTAL_APP_MEM_SIZE = 3;
const uint8_t DO_CREATE_NODE = 4;
const uint8_t DO_CREATE_NODE_AND_SURFACE = 5;
const uint8_t DO_CREATE_RS_SURFACE = 6;
const uint8_t DO_CREATE_VSYNC_RECEIVER = 7;
const uint8_t DO_CREATE_PIXEL_MAP_FROM_SURFACE_ID = 8;
const uint8_t DO_TRIGGER_SURFACE_CAPTURE_CALLBACK = 9;
const uint8_t DO_TAKE_SURFACE_CAPTURE = 10;
const uint8_t DO_SET_FOCUS_APP_INFO = 11;
const uint8_t DO_GET_DEFAULT_SCREEN_ID = 12;
const uint8_t DO_GET_ACTIVE_SCREEN_ID = 13;
const uint8_t DO_GET_ALL_SCREEN_IDS = 14;
const uint8_t TARGET_SIZE = 15;

sptr<RSIRenderServiceConnection> CONN = nullptr;
const uint8_t* DATA = nullptr;
size_t g_size = 0;
size_t g_pos;

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

template<>
std::string GetData()
{
    size_t objectSize = GetData<uint8_t>();
    std::string object(objectSize, '\0');
    if (DATA == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    object.assign(reinterpret_cast<const char*>(DATA + g_pos), objectSize);
    g_pos += objectSize;
    return object;
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

bool DoExecuteSynchronousTask()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    uint64_t timeOutNs = GetData<uint64_t>();
    auto task = std::make_shared<DerivedSyncTask>(timeOutNs);
    client->ExecuteSynchronousTask(task);
    task = nullptr;
    client->ExecuteSynchronousTask(task);
    return true;
}

bool DoGetMemoryGraphic()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    int pid = GetData<int>();
    client->GetMemoryGraphic(pid);
    return true;
}

bool DoGetMemoryGraphics()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    client->GetMemoryGraphics();
    return true;
}

bool DoGetTotalAppMemSize()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    float cpuMemSize = GetData<float>();
    float gpuMemSize = GetData<float>();
    client->GetTotalAppMemSize(cpuMemSize, gpuMemSize);
    return true;
}

bool DoCreateNode()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    RSDisplayNodeConfig displayNodeConfig = {
        .screenId = GetData<uint64_t>(),
        .isMirrored = GetData<bool>(),
        .mirrorNodeId = GetData<uint64_t>(),
        .isSync = GetData<bool>()
    };
    NodeId nodeId = GetData<NodeId>();
    client->CreateNode(displayNodeConfig, nodeId);
    RSSurfaceRenderNodeConfig surfaceRenderNodeConfig = {
        .id = GetData<NodeId>(),
        .name = "fuzztest"
    };
    client->CreateNode(surfaceRenderNodeConfig);
    return true;
}

bool DoCreateNodeAndSurface()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    RSSurfaceRenderNodeConfig surfaceRenderNodeConfig = {
        .id = GetData<NodeId>(),
        .name = "fuzztest"
    };
    client->CreateNodeAndSurface(surfaceRenderNodeConfig);
    return true;
}

bool DoCreateRSSurface()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    sptr<Surface> surface;
    client->CreateRSSurface(surface);
    return true;
}

bool DoCreateVSyncReceiver()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    std::string name = "fuzztest";
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> looper;
    uint64_t id = GetData<uint64_t>();
    NodeId windowNodeId = GetData<NodeId>();
    bool fromXcomponent = GetData<bool>();
    client->CreateVSyncReceiver(name, looper, id, windowNodeId, fromXcomponent);
    return true;
}

bool DoCreatePixelMapFromSurfaceId()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    uint64_t surfaceId = GetData<uint64_t>();
    Rect srcRect = {0, 0, 100, 100};
    client->CreatePixelMapFromSurfaceId(surfaceId, srcRect);
    return true;
}

bool DoTriggerSurfaceCaptureCallback()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    NodeId id = GetData<NodeId>();
    OHOS::Media::InitializationOptions opts;
    opts.size.width = GetData<int32_t>();
    opts.size.height = GetData<int32_t>();
    opts.srcPixelFormat = static_cast<Media::PixelFormat>(GetData<int32_t>() % 14); // Max Media::PixelFormat is 14
    opts.pixelFormat = static_cast<Media::PixelFormat>(GetData<int32_t>() % 14);    // Max Media::PixelFormat is 14
    opts.alphaType = static_cast<Media::AlphaType>(GetData<int32_t>() % 4);         // Max Media::AlphaType is 4
    opts.scaleMode = static_cast<Media::ScaleMode>(GetData<int32_t>() % 2);         // Max Media::ScaleMode is 2
    opts.editable = GetData<bool>();
    opts.useSourceIfMatch = GetData<bool>();
    std::shared_ptr<Media::PixelMap> pixelMap = Media::PixelMap::Create(opts);
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
    client->TriggerSurfaceCaptureCallback(id, captureConfig, pixelMap);
    return true;
}

bool DoTakeSurfaceCapture()
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
    uint8_t listSize = GetData<uint8_t>();
    for (uint8_t i = 0; i < listSize; ++i) {
        uint64_t listNodeId = GetData<uint64_t>();
        captureConfig.blackList.push_back(listNodeId);
    }
    captureConfig.mainScreenRect.left_ = GetData<float>();
    captureConfig.mainScreenRect.top_ = GetData<float>();
    captureConfig.mainScreenRect.right_ = GetData<float>();
    captureConfig.mainScreenRect.bottom_ = GetData<float>();
    client->TakeSurfaceCapture(nodeId, callback, captureConfig);
    return true;
}

bool DoSetFocusAppInfo()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    int32_t pid = GetData<int32_t>();
    int32_t uid = GetData<int32_t>();
    std::string bundleName = GetData<std::string>();
    std::string abilityName = GetData<std::string>();
    uint64_t focusNodeId = GetData<uint64_t>();
    FocusAppInfo info = {
        .pid = pid,
        .uid = uid,
        .bundleName = bundleName,
        .abilityName = abilityName,
        .focusNodeId = focusNodeId};
    client->SetFocusAppInfo(info);
    return true;
}

bool DoGetDefaultScreenId()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    client->GetDefaultScreenId();
    return true;
}

bool DoGetActiveScreenId()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    client->GetActiveScreenId();
    return true;
}

bool DoGetAllScreenIds()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    client->GetAllScreenIds();
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
        case OHOS::Rosen::DO_EXECUTE_SYNCHRONOUS_TASK:
            OHOS::Rosen::DoExecuteSynchronousTask();
            break;
        case OHOS::Rosen::DO_GET_MEMORY_GRAPHIC:
            OHOS::Rosen::DoGetMemoryGraphic();
            break;
        case OHOS::Rosen::DO_GET_MEMORY_GRAPHICS:
            OHOS::Rosen::DoGetMemoryGraphics();
            break;
        case OHOS::Rosen::DO_GET_TOTAL_APP_MEM_SIZE:
            OHOS::Rosen::DoGetTotalAppMemSize();
            break;
        case OHOS::Rosen::DO_CREATE_NODE:
            OHOS::Rosen::DoCreateNode();
            break;
        case OHOS::Rosen::DO_CREATE_NODE_AND_SURFACE:
            OHOS::Rosen::DoCreateNodeAndSurface();
            break;
        case OHOS::Rosen::DO_CREATE_RS_SURFACE:
            OHOS::Rosen::DoCreateRSSurface();
            break;
        case OHOS::Rosen::DO_CREATE_VSYNC_RECEIVER:
            OHOS::Rosen::DoCreateVSyncReceiver();
            break;
        case OHOS::Rosen::DO_CREATE_PIXEL_MAP_FROM_SURFACE_ID:
            OHOS::Rosen::DoCreatePixelMapFromSurfaceId();
            break;
        case OHOS::Rosen::DO_TRIGGER_SURFACE_CAPTURE_CALLBACK:
            OHOS::Rosen::DoTriggerSurfaceCaptureCallback();
            break;
        case OHOS::Rosen::DO_TAKE_SURFACE_CAPTURE:
            OHOS::Rosen::DoTakeSurfaceCapture();
            break;
        case OHOS::Rosen::DO_SET_FOCUS_APP_INFO:
            OHOS::Rosen::DoSetFocusAppInfo();
            break;
        case OHOS::Rosen::DO_GET_DEFAULT_SCREEN_ID:
            OHOS::Rosen::DoGetDefaultScreenId();
            break;
        case OHOS::Rosen::DO_GET_ACTIVE_SCREEN_ID:
            OHOS::Rosen::DoGetActiveScreenId();
            break;
        case OHOS::Rosen::DO_GET_ALL_SCREEN_IDS:
            OHOS::Rosen::DoGetAllScreenIds();
            break;
        default:
            return -1;
    }
    return 0;
}