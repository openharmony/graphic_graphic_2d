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

#include "rsrenderserviceclient004_fuzzer.h"

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
const uint8_t DO_SET_VIRTUAL_MIRROR_SCREEN_SCALE_MODE = 0;
const uint8_t DO_SET_GLOBAL_DARK_COLOR_MODE = 1;
const uint8_t DO_GET_SCREEN_HDR_CAPABILITY = 2;
const uint8_t DO_GET_PIXEL_FORMAT = 3;
const uint8_t DO_GET_SCREEN_SUPPORTED_HDR_FORMATS = 4;
const uint8_t DO_GET_SCREEN_SUPPORTED_COLORSPACES = 5;
const uint8_t DO_GET_SCREEN_TYPE = 6;
const uint8_t DO_SET_VIRTUAL_SCREEN_REFRESH_RATE = 7;
const uint8_t DO_REGISTER_OCCLUSION_CHANGE_CALLBACK = 8;
const uint8_t DO_REGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK = 9;
const uint8_t DO_REGISTER_HGM_CONFIG_CHANGE_CALLBACK = 10;
const uint8_t DO_SET_SYSTEM_ANIMATED_SCENES = 11;
const uint8_t DO_REGISTER_FRAME_RATE_LINKER_EXPECTED_FPS_CALLBACK = 12;
const uint8_t DO_UN_REGISTER_FRAME_RATE_LINKER_EXPECTED_FPS_CALLBACK = 13;
const uint8_t DO_SHOW_WATERMARK = 14;
const uint8_t DO_RESIZE_VIRTUAL_SCREEN = 15;
const uint8_t DO_REPORT_JANK_STATS = 16;
const uint8_t DO_REPORT_EVENT_RESPONSE = 17;
const uint8_t DO_REPORT_GAME_STATE_DATA = 18;
const uint8_t DO_GET_SCREEN_HDR_STATUS = 19;
const uint8_t TARGET_SIZE = 20;

sptr<RSIRenderServiceConnection> CONN = nullptr;
const uint8_t* DATA = nullptr;
size_t g_size = 0;
size_t g_pos;
constexpr size_t SCREEN_SCALE_MODE_SIZE = 3;
constexpr size_t GRAPHIC_PIXEL_FORMAT_SIZE = 43;
constexpr size_t GRAPHIC_CM_COLOR_SPACE_TYPE_SIZE = 32;
constexpr size_t SCREEN_TYPE_SIZE = 4;

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

bool DoSetVirtualMirrorScreenScaleMode()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    ScreenScaleMode scaleMode = static_cast<ScreenScaleMode>(GetData<uint8_t>() % SCREEN_SCALE_MODE_SIZE);
    client->SetVirtualMirrorScreenScaleMode(id, scaleMode);
    return true;
}

bool DoSetGlobalDarkColorMode()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    bool isDark = GetData<bool>();
    client->SetGlobalDarkColorMode(isDark);
    return true;
}

bool DoGetScreenHDRCapability()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    RSScreenHDRCapability screenHdrCapability;
    client->GetScreenHDRCapability(id, screenHdrCapability);
    return true;
}

bool DoGetPixelFormat()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    GraphicPixelFormat pixelFormat = static_cast<GraphicPixelFormat>(GetData<uint8_t>() % GRAPHIC_PIXEL_FORMAT_SIZE);
    client->SetPixelFormat(id, pixelFormat);
    client->GetPixelFormat(id, pixelFormat);
    return true;
}

bool DoGetScreenSupportedHDRFormats()
{
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

bool DoGetScreenSupportedColorSpaces()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();

    GraphicCM_ColorSpaceType colorSpace =
        static_cast<GraphicCM_ColorSpaceType>(GetData<uint8_t>() % GRAPHIC_CM_COLOR_SPACE_TYPE_SIZE);
    std::vector<GraphicCM_ColorSpaceType> colorSpaces = { colorSpace };
    client->SetScreenColorSpace(id, colorSpace);
    client->GetScreenColorSpace(id, colorSpace);
    client->GetScreenSupportedColorSpaces(id, colorSpaces);
    return true;
}

bool DoGetScreenType()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    RSScreenType screenType = static_cast<RSScreenType>(GetData<uint8_t>() % SCREEN_TYPE_SIZE);
    client->GetScreenType(id, screenType);
    return true;
}

bool DoSetVirtualScreenRefreshRate()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    uint32_t maxRefreshRate = GetData<uint32_t>();
    uint32_t actualRefreshRate = GetData<uint32_t>();
    client->SetVirtualScreenRefreshRate(id, maxRefreshRate, actualRefreshRate);
    return true;
}

bool DoRegisterOcclusionChangeCallback()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    OcclusionChangeCallback callback;
    client->RegisterOcclusionChangeCallback(callback);
    return true;
}

bool DoRegisterSurfaceOcclusionChangeCallback()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    SurfaceOcclusionChangeCallback callback;
    NodeId id = GetData<NodeId>();
    std::vector<float> partitionPoints;
    client->RegisterSurfaceOcclusionChangeCallback(id, callback, partitionPoints);
    client->UnRegisterSurfaceOcclusionChangeCallback(id);
    return true;
}

bool DoRegisterHgmConfigChangeCallback()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    HgmConfigChangeCallback callback;
    client->RegisterHgmConfigChangeCallback(callback);
    HgmRefreshRateModeChangeCallback callback2;
    client->RegisterHgmRefreshRateModeChangeCallback(callback2);
    HgmRefreshRateUpdateCallback callback3;
    client->RegisterHgmRefreshRateUpdateCallback(callback3);
    uint32_t dstPid = GetData<uint32_t>();
    FrameRateLinkerExpectedFpsUpdateCallback callback4;
    client->RegisterFrameRateLinkerExpectedFpsUpdateCallback(dstPid, callback4);
    return true;
}

bool DoSetSystemAnimatedScenes()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    SystemAnimatedScenes systemAnimatedScenes = SystemAnimatedScenes::ENTER_MISSION_CENTER;
    client->SetSystemAnimatedScenes(systemAnimatedScenes, false);
    return true;
}

bool DoRegisterFrameRateLinkerExpectedFpsUpdateCallback()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    uint32_t dstPid = GetData<uint32_t>();
    FrameRateLinkerExpectedFpsUpdateCallback callback;
    client->RegisterFrameRateLinkerExpectedFpsUpdateCallback(dstPid, callback);
    return true;
}

bool DoUnRegisterFrameRateLinkerExpectedFpsUpdateCallback()
{
    std::shared_ptr<RSInterfaces> rsInterfaces = std::make_shared<RSInterfaces>();
    uint32_t dstPid = GetData<uint32_t>();
    rsInterfaces->UnRegisterFrameRateLinkerExpectedFpsUpdateCallback(dstPid);
    return true;
}

bool DoShowWatermark()
{
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

bool DoResizeVirtualScreen()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    client->ResizeVirtualScreen(id, width, height);
    return true;
}

bool DoReportJankStats()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    client->ReportJankStats();
    return true;
}

bool DoReportEventResponse()
{
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

bool DoReportGameStateData()
{
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

bool DoGetScreenHDRStatus()
{
    static std::vector<HdrStatus> statusVec = { HdrStatus::NO_HDR, HdrStatus::HDR_PHOTO, HdrStatus::HDR_VIDEO,
        HdrStatus::AI_HDR_VIDEO, HdrStatus::HDR_EFFECT };
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    HdrStatus status = statusVec[GetData<uint8_t>() % statusVec.size()];
    client->GetScreenHDRStatus(id, status);
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
        case OHOS::Rosen::DO_SET_VIRTUAL_MIRROR_SCREEN_SCALE_MODE:
            OHOS::Rosen::DoSetVirtualMirrorScreenScaleMode();
            break;
        case OHOS::Rosen::DO_SET_GLOBAL_DARK_COLOR_MODE:
            OHOS::Rosen::DoSetGlobalDarkColorMode();
            break;
        case OHOS::Rosen::DO_GET_SCREEN_HDR_CAPABILITY:
            OHOS::Rosen::DoGetScreenHDRCapability();
            break;
        case OHOS::Rosen::DO_GET_PIXEL_FORMAT:
            OHOS::Rosen::DoGetPixelFormat();
            break;
        case OHOS::Rosen::DO_GET_SCREEN_SUPPORTED_HDR_FORMATS:
            OHOS::Rosen::DoGetScreenSupportedHDRFormats();
            break;
        case OHOS::Rosen::DO_GET_SCREEN_SUPPORTED_COLORSPACES:
            OHOS::Rosen::DoGetScreenSupportedColorSpaces();
            break;
        case OHOS::Rosen::DO_GET_SCREEN_TYPE:
            OHOS::Rosen::DoGetScreenType();
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_SCREEN_REFRESH_RATE:
            OHOS::Rosen::DoSetVirtualScreenRefreshRate();
            break;
        case OHOS::Rosen::DO_REGISTER_OCCLUSION_CHANGE_CALLBACK:
            OHOS::Rosen::DoRegisterOcclusionChangeCallback();
            break;
        case OHOS::Rosen::DO_REGISTER_SURFACE_OCCLUSION_CHANGE_CALLBACK:
            OHOS::Rosen::DoRegisterSurfaceOcclusionChangeCallback();
            break;
        case OHOS::Rosen::DO_REGISTER_HGM_CONFIG_CHANGE_CALLBACK:
            OHOS::Rosen::DoRegisterHgmConfigChangeCallback();
            break;
        case OHOS::Rosen::DO_SET_SYSTEM_ANIMATED_SCENES:
            OHOS::Rosen::DoSetSystemAnimatedScenes();
            break;
        case OHOS::Rosen::DO_REGISTER_FRAME_RATE_LINKER_EXPECTED_FPS_CALLBACK:
            OHOS::Rosen::DoRegisterFrameRateLinkerExpectedFpsUpdateCallback();
            break;
        case OHOS::Rosen::DO_UN_REGISTER_FRAME_RATE_LINKER_EXPECTED_FPS_CALLBACK:
            OHOS::Rosen::DoUnRegisterFrameRateLinkerExpectedFpsUpdateCallback();
            break;
        case OHOS::Rosen::DO_SHOW_WATERMARK:
            OHOS::Rosen::DoShowWatermark();
            break;
        case OHOS::Rosen::DO_RESIZE_VIRTUAL_SCREEN:
            OHOS::Rosen::DoResizeVirtualScreen();
            break;
        case OHOS::Rosen::DO_REPORT_JANK_STATS:
            OHOS::Rosen::DoReportJankStats();
            break;
        case OHOS::Rosen::DO_REPORT_EVENT_RESPONSE:
            OHOS::Rosen::DoReportEventResponse();
            break;
        case OHOS::Rosen::DO_REPORT_GAME_STATE_DATA:
            OHOS::Rosen::DoReportGameStateData();
            break;
        case OHOS::Rosen::DO_GET_SCREEN_HDR_STATUS:
            OHOS::Rosen::DoGetScreenHDRStatus();
            break;
        default:
            return -1;
    }
    return 0;
}