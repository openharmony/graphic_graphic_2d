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

#include "rsrenderserviceclient002_fuzzer.h"

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
const uint8_t DO_CREATE_VIRTUAL_SCREEN = 0;
const uint8_t DO_SET_VIRTUAL_SCREEN_BLACK_LIST = 1;
const uint8_t DO_DROP_FRAME_BY_PID = 2;
const uint8_t DO_SET_WATERMARK = 3;
const uint8_t DO_SET_VIRTUAL_SCREEN_SECURITY_EXEMPTION_LIST = 4;
const uint8_t DO_SET_CAST_SCREEN_ENABLE_SKIP_WINDOW = 5;
const uint8_t DO_SET_VIRTUAL_SCREEN_SURFACE = 6;
const uint8_t DO_REMOVE_VIRTUAL_SCREEN = 7;
const uint8_t DO_SET_SCREEN_REFRESH_RATE = 8;
const uint8_t DO_SET_REFRESH_RATE_MODE = 9;
const uint8_t DO_SYNC_FRAME_RATE_RANGE = 10;
const uint8_t DO_UNREGISTER_FRAME_RATE_LINKER = 11;
const uint8_t DO_GET_SCREEN_CURRENT_REFRESH_RATE = 12;
const uint8_t DO_GET_CURRENT_REFRESH_RATE_MODE = 13;
const uint8_t DO_GET_SCREEN_SUPPORTED_REFRESH_RATES = 14;
const uint8_t DO_GET_SHOW_REFRESH_RATE_ENABLED = 15;
const uint8_t DO_GET_REFRESH_INFO = 16;
const uint8_t TARGET_SIZE = 17;

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

bool DoCreateVirtualScreen()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    std::string name = GetStringFromData(STR_LEN);
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("DisplayNode");
    sptr<IBufferProducer> bp = cSurface->GetProducer();
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(bp);
    ScreenId mirrorId = GetData<ScreenId>();
    int32_t flags = GetData<int32_t>();
    std::vector<NodeId> fuzzWhiteList;
    uint8_t fuzzWhiteListSize = GetData<uint8_t>();
    for (size_t i = 0; i < fuzzWhiteListSize; i++) {
        fuzzWhiteList.push_back(GetData<NodeId>());
    }
    client->CreateVirtualScreen(name, width, height, pSurface, mirrorId, flags, fuzzWhiteList);
    return true;
}

bool DoSetVirtualScreenBlackList()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    std::vector<NodeId> blackListVector;
    uint8_t blackListVectorSize = GetData<uint8_t>();
    for (size_t i = 0; i < blackListVectorSize; i++) {
        blackListVector.push_back(GetData<NodeId>());
    }
    client->SetVirtualScreenBlackList(id, blackListVector);
    return true;
}

bool DoDropFrameByPid()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    std::vector<int32_t> pidList;
    uint8_t pidListSize = GetData<uint8_t>();
    for (size_t i = 0; i < pidListSize; i++) {
        pidList.push_back(GetData<int32_t>());
    }
    client->DropFrameByPid(pidList);
    return true;
}

bool DoSetWatermark()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
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
    std::shared_ptr<Media::PixelMap>  watermark = Media::PixelMap::Create(opts);
    client->SetWatermark(name, watermark);
    return true;
}

bool DoSetVirtualScreenSecurityExemptionList()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    std::vector<NodeId> securityExemptionList;
    uint8_t securityExemptionListSize = GetData<uint8_t>();
    for (size_t i = 0; i < securityExemptionListSize; i++) {
        securityExemptionList.push_back(GetData<NodeId>());
    }
    client->SetVirtualScreenSecurityExemptionList(id, securityExemptionList);
    return true;
}

bool DoSetCastScreenEnableSkipWindow()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    bool enable = GetData<bool>();
    client->SetCastScreenEnableSkipWindow(id, enable);
    return true;
}

bool DoSetVirtualScreenSurface()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    sptr<Surface> surface;
    client->SetVirtualScreenSurface(id, surface);
    return true;
}

bool DoRemoveVirtualScreen()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    client->RemoveVirtualScreen(id);
    return true;
}

bool DoSetScreenRefreshRate()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    int32_t sceneId = GetData<int32_t>();
    int32_t rate = GetData<int32_t>();
    client->SetScreenRefreshRate(id, sceneId, rate);
    return true;
}

bool DoSetRefreshRateMode()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    int32_t refreshRateMode = GetData<int32_t>();
    client->SetRefreshRateMode(refreshRateMode);
    return true;
}

bool DoSyncFrameRateRange()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    FrameRateLinkerId id = GetData<FrameRateLinkerId>();
    FrameRateRange range;
    int32_t animatorExpectedFrameRate = GetData<int32_t>();
    client->SyncFrameRateRange(id, range, animatorExpectedFrameRate);
    return true;
}

bool DoUnregisterFrameRateLinker()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    FrameRateLinkerId id = GetData<FrameRateLinkerId>();
    client->UnregisterFrameRateLinker(id);
    return true;
}

bool DoGetScreenCurrentRefreshRate()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    client->GetScreenCurrentRefreshRate(id);
    return true;
}

bool DoGetCurrentRefreshRateMode()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    client->GetCurrentRefreshRateMode();
    return true;
}

bool DoGetScreenSupportedRefreshRates()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    client->GetScreenSupportedRefreshRates(id);
    return true;
}

bool DoGetShowRefreshRateEnabled()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    client->GetShowRefreshRateEnabled();
    return true;
}

bool DoGetRefreshInfo()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    pid_t pid = GetData<pid_t>();
    client->GetRefreshInfo(pid);
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
        case OHOS::Rosen::DO_CREATE_VIRTUAL_SCREEN:
            OHOS::Rosen::DoCreateVirtualScreen();
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_SCREEN_BLACK_LIST:
            OHOS::Rosen::DoSetVirtualScreenBlackList();
            break;
        case OHOS::Rosen::DO_DROP_FRAME_BY_PID:
            OHOS::Rosen::DoDropFrameByPid();
            break;
        case OHOS::Rosen::DO_SET_WATERMARK:
            OHOS::Rosen::DoSetWatermark();
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_SCREEN_SECURITY_EXEMPTION_LIST:
            OHOS::Rosen::DoSetVirtualScreenSecurityExemptionList();
            break;
        case OHOS::Rosen::DO_SET_CAST_SCREEN_ENABLE_SKIP_WINDOW:
            OHOS::Rosen::DoSetCastScreenEnableSkipWindow();
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_SCREEN_SURFACE:
            OHOS::Rosen::DoSetVirtualScreenSurface();
            break;
        case OHOS::Rosen::DO_REMOVE_VIRTUAL_SCREEN:
            OHOS::Rosen::DoRemoveVirtualScreen();
            break;
        case OHOS::Rosen::DO_SET_SCREEN_REFRESH_RATE:
            OHOS::Rosen::DoSetScreenRefreshRate();
            break;
        case OHOS::Rosen::DO_SET_REFRESH_RATE_MODE:
            OHOS::Rosen::DoSetRefreshRateMode();
            break;
        case OHOS::Rosen::DO_SYNC_FRAME_RATE_RANGE:
            OHOS::Rosen::DoSyncFrameRateRange();
            break;
        case OHOS::Rosen::DO_UNREGISTER_FRAME_RATE_LINKER:
            OHOS::Rosen::DoUnregisterFrameRateLinker();
            break;
        case OHOS::Rosen::DO_GET_SCREEN_CURRENT_REFRESH_RATE:
            OHOS::Rosen::DoGetScreenCurrentRefreshRate();
            break;
        case OHOS::Rosen::DO_GET_CURRENT_REFRESH_RATE_MODE:
            OHOS::Rosen::DoGetCurrentRefreshRateMode();
            break;
        case OHOS::Rosen::DO_GET_SCREEN_SUPPORTED_REFRESH_RATES:
            OHOS::Rosen::DoGetScreenSupportedRefreshRates();
            break;
        case OHOS::Rosen::DO_GET_SHOW_REFRESH_RATE_ENABLED:
            OHOS::Rosen::DoGetShowRefreshRateEnabled();
            break;
        case OHOS::Rosen::DO_GET_REFRESH_INFO:
            OHOS::Rosen::DoGetRefreshInfo();
            break;
        default:
            return -1;
    }
    return 0;
}