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

#include "rsrenderserviceclient003_fuzzer.h"

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
const uint8_t DO_SET_SHOW_REFRESH_RATE_ENABLED = 0;
const uint8_t DO_GET_REALTIME_REFRESH_RATE = 1;
const uint8_t DO_SET_PHYSICAL_SCREEN_RESOLUTION = 2;
const uint8_t DO_SET_VIRTUAL_SCREEN_RESOLUTION = 3;
const uint8_t DO_GET_VIRTUAL_SCREEN_RESOLUTION = 4;
const uint8_t DO_MARK_POWER_OFF_NEED_PROCESS_ONE_FRAME = 5;
const uint8_t DO_DISABLE_PWOER_OFF_RENDER_CONTROL = 6;
const uint8_t DO_GET_SCREEN_ACTIVE_MODE = 7;
const uint8_t DO_GET_SCREEN_SUPPORTED_MODES = 8;
const uint8_t DO_REGISTER_BUFFER_AVAILABLE_LISTENER = 9;
const uint8_t DO_REGISTER_BUFFER_CLEAR_LISTENER = 10;
const uint8_t DO_GET_SCREEN_SUPPORTED_COLOR_GAMUTS = 11;
const uint8_t DO_GET_SCREEN_SUPPORTED_METADATAKEYS = 12;
const uint8_t DO_GET_SCREEN_COLOR_GAMUT = 13;
const uint8_t DO_SET_SCREEN_GAMUT_MAP = 14;
const uint8_t DO_SET_SCREEN_CORRECTION = 15;
const uint8_t DO_SET_VIRTUAL_MIRROR_SCREEN_CANVAS_ROTATION = 16;
const uint8_t TARGET_SIZE = 17;

sptr<RSIRenderServiceConnection> CONN = nullptr;
const uint8_t* DATA = nullptr;
size_t g_size = 0;
size_t g_pos;

constexpr size_t SCREEN_COLOR_GAMUT_SIZE = 12;
constexpr size_t SCREEN_HDR_METAS_DATA_KRY_SIZE = 12;
constexpr size_t SCREEN_ROTATION_SIZE = 5;

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

bool DoSetShowRefreshRateEnabled()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    bool enabled = GetData<bool>();
    int32_t type = GetData<int32_t>();
    client->SetShowRefreshRateEnabled(enabled, type);
    return true;
}

bool DoGetRealtimeRefreshRate()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    client->GetRealtimeRefreshRate(id);
    return true;
}

bool DoSetPhysicalScreenResolution()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    client->SetPhysicalScreenResolution(id, width, height);
    return true;
}

bool DoSetVirtualScreenResolution()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    client->SetPhysicalScreenResolution(id, width, height);
    return true;
}

bool DoGetVirtualScreenResolution()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    client->GetVirtualScreenResolution(id);
    return true;
}

bool DoMarkPowerOffNeedProcessOneFrame()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    client->MarkPowerOffNeedProcessOneFrame();
    return true;
}

bool DoDisablePowerOffRenderControl()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    client->DisablePowerOffRenderControl(id);
    return true;
}

bool DoGetScreenActiveMode()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    client->GetScreenActiveMode(id);
    return true;
}

bool DoGetScreenSupportedModes()
{
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

bool DoRegisterBufferAvailableListener()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    NodeId id = GetData<ScreenId>();
    BufferAvailableCallback callback;
    bool isFromRenderThread = GetData<bool>();
    client->RegisterBufferAvailableListener(id, callback, isFromRenderThread);
    client->UnregisterBufferAvailableListener(id);
    return true;
}

bool DoRegisterBufferClearListener()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    NodeId id = GetData<ScreenId>();
    BufferClearCallback callback;
    client->RegisterBufferClearListener(id, callback);
    return true;
}

bool DoGetScreenSupportedColorGamuts()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    ScreenColorGamut type = static_cast<ScreenColorGamut>(GetData<uint8_t>() % SCREEN_COLOR_GAMUT_SIZE);
    std::vector<ScreenColorGamut> mode = { type };
    client->GetScreenSupportedColorGamuts(id, mode);
    return true;
}

bool DoGetScreenSupportedMetaDataKeys()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    ScreenHDRMetadataKey type = static_cast<ScreenHDRMetadataKey>(GetData<uint8_t>() % SCREEN_HDR_METAS_DATA_KRY_SIZE);
    std::vector<ScreenHDRMetadataKey> keys = { type };
    client->GetScreenSupportedMetaDataKeys(id, keys);
    return true;
}

bool DoGetScreenColorGamut()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    ScreenColorGamut mode = static_cast<ScreenColorGamut>(GetData<uint8_t>() % SCREEN_COLOR_GAMUT_SIZE);
    int32_t modeIdx = GetData<int32_t>();
    client->SetScreenColorGamut(id, modeIdx);
    client->GetScreenColorGamut(id, mode);
    return true;
}

bool DoSetScreenGamutMap()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    ScreenGamutMap mode = static_cast<ScreenGamutMap>(GetData<uint8_t>() % SCREEN_COLOR_GAMUT_SIZE);
    client->SetScreenGamutMap(id, mode);
    client->GetScreenGamutMap(id, mode);
    return true;
}

bool DoSetScreenCorrection()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    ScreenRotation screenRotation = static_cast<ScreenRotation>(GetData<uint8_t>() % SCREEN_ROTATION_SIZE);
    client->SetScreenCorrection(id, screenRotation);
    return true;
}

bool DoSetVirtualMirrorScreenCanvasRotation()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    bool canvasRotation = GetData<bool>();
    client->SetVirtualMirrorScreenCanvasRotation(id, canvasRotation);
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
        case OHOS::Rosen::DO_SET_SHOW_REFRESH_RATE_ENABLED:
            OHOS::Rosen::DoSetShowRefreshRateEnabled();
            break;
        case OHOS::Rosen::DO_GET_REALTIME_REFRESH_RATE:
            OHOS::Rosen::DoGetRealtimeRefreshRate();
            break;
        case OHOS::Rosen::DO_SET_PHYSICAL_SCREEN_RESOLUTION:
            OHOS::Rosen::DoSetPhysicalScreenResolution();
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_SCREEN_RESOLUTION:
            OHOS::Rosen::DoSetVirtualScreenResolution();
            break;
        case OHOS::Rosen::DO_GET_VIRTUAL_SCREEN_RESOLUTION:
            OHOS::Rosen::DoGetVirtualScreenResolution();
            break;
        case OHOS::Rosen::DO_MARK_POWER_OFF_NEED_PROCESS_ONE_FRAME:
            OHOS::Rosen::DoMarkPowerOffNeedProcessOneFrame();
            break;
        case OHOS::Rosen::DO_DISABLE_PWOER_OFF_RENDER_CONTROL:
            OHOS::Rosen::DoDisablePowerOffRenderControl();
            break;
        case OHOS::Rosen::DO_GET_SCREEN_ACTIVE_MODE:
            OHOS::Rosen::DoGetScreenActiveMode();
            break;
        case OHOS::Rosen::DO_GET_SCREEN_SUPPORTED_MODES:
            OHOS::Rosen::DoGetScreenSupportedModes();
            break;
        case OHOS::Rosen::DO_REGISTER_BUFFER_AVAILABLE_LISTENER:
            OHOS::Rosen::DoRegisterBufferAvailableListener();
            break;
        case OHOS::Rosen::DO_REGISTER_BUFFER_CLEAR_LISTENER:
            OHOS::Rosen::DoRegisterBufferClearListener();
            break;
        case OHOS::Rosen::DO_GET_SCREEN_SUPPORTED_COLOR_GAMUTS:
            OHOS::Rosen::DoGetScreenSupportedColorGamuts();
            break;
        case OHOS::Rosen::DO_GET_SCREEN_SUPPORTED_METADATAKEYS:
            OHOS::Rosen::DoGetScreenSupportedMetaDataKeys();
            break;
        case OHOS::Rosen::DO_GET_SCREEN_COLOR_GAMUT:
            OHOS::Rosen::DoGetScreenColorGamut();
            break;
        case OHOS::Rosen::DO_SET_SCREEN_GAMUT_MAP:
            OHOS::Rosen::DoSetScreenGamutMap();
            break;
        case OHOS::Rosen::DO_SET_SCREEN_CORRECTION:
            OHOS::Rosen::DoSetScreenCorrection();
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_MIRROR_SCREEN_CANVAS_ROTATION:
            OHOS::Rosen::DoSetVirtualMirrorScreenCanvasRotation();
            break;
        default:
            return -1;
    }
    return 0;
}