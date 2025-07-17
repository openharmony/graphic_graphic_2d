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

#include "rsrenderserviceclient007_fuzzer.h"

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
const uint8_t DO_GET_PIXELMAP_BY_PROCESSID = 0;
const uint8_t DO_TAKE_SELF_SURFACE_CAPTURE = 1;
const uint8_t DO_SET_WINDOW_FREEZE_IMMEDIATELY = 2;
const uint8_t DO_SET_SCREEN_SECURITY_MASK = 3;
const uint8_t DO_REPAINT_EVERYTHING = 4;
const uint8_t DO_FORCE_REFRESH_ONE_FRAME_WITH_NEXT_VSYNC = 5;
const uint8_t DO_REGISTER_TYPEFACE = 6;
const uint8_t DO_REPORT_RS_SCENE_JANK = 7;
const uint8_t DO_GET_HDR_ON_DURATION = 8;
const uint8_t DO_SET_WINDOW_CONTAINER = 9;
const uint8_t TARGET_SIZE = 10;

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

bool DoGetPixelMapByProcessId()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    std::vector<PixelMapInfo> pixelMapInfoVector;
    pid_t pid = GetData<pid_t>();
    client->GetPixelMapByProcessId(pixelMapInfoVector, pid);
    return true;
}

bool DoTakeSelfSurfaceCapture()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    NodeId nodeId = GetData<NodeId>();
    std::shared_ptr<SurfaceCaptureCallback> callback;
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = GetData<float>();
    captureConfig.scaleY = GetData<float>();
    captureConfig.useDma = GetData<bool>();
    captureConfig.useCurWindow = GetData<bool>();
    uint8_t type = GetData<uint8_t>();
    captureConfig.captureType = (SurfaceCaptureType)type;
    captureConfig.isSync = GetData<bool>();
    client->TakeSelfSurfaceCapture(nodeId, callback, captureConfig);
    return true;
}

bool DoSetWindowFreezeImmediately()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    NodeId nodeId = GetData<NodeId>();
    bool isFreeze = GetData<bool>();
    std::shared_ptr<SurfaceCaptureCallback> callback;
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = GetData<float>();
    captureConfig.scaleY = GetData<float>();
    captureConfig.useDma = GetData<bool>();
    captureConfig.useCurWindow = GetData<bool>();
    uint8_t type = GetData<uint8_t>();
    captureConfig.captureType = (SurfaceCaptureType)type;
    captureConfig.isSync = GetData<bool>();
    client->SetWindowFreezeImmediately(nodeId, isFreeze, callback, captureConfig);
    return true;
}

bool DoSetScreenSecurityMask()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    Media::InitializationOptions opts;
    opts.size.width = GetData<int32_t>();
    opts.size.height = GetData<int32_t>();
    opts.srcPixelFormat = static_cast<Media::PixelFormat>(GetData<int32_t>());
    opts.pixelFormat = static_cast<Media::PixelFormat>(GetData<int32_t>());
    opts.alphaType = static_cast<Media::AlphaType>(GetData<int32_t>());
    opts.scaleMode = static_cast<Media::ScaleMode>(GetData<int32_t>());
    opts.editable = GetData<bool>();
    opts.useSourceIfMatch = GetData<bool>();
    std::shared_ptr<Media::PixelMap> securityMask = Media::PixelMap::Create(opts);
    client->SetScreenSecurityMask(id, securityMask);
    return true;
}

bool DoRepaintEverything()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    client->RepaintEverything();
    return true;
}

bool DoForceRefreshOneFrameWithNextVSync()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    client->ForceRefreshOneFrameWithNextVSync();
    return true;
}

bool DoRegisterTypeface()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId screenId = GetData<ScreenId>();
    RSScreenType screenType = RSScreenType::BUILT_IN_TYPE_SCREEN;
    Drawing::Bitmap bm;
    NodeId nodeId = GetData<uint64_t>();
    std::shared_ptr<Media::PixelMap> pixelmap;
    Drawing::Rect *rect;
    std::shared_ptr<Drawing::DrawCmdList> drawCmdList;
    std::shared_ptr<Drawing::Typeface> typeface;
    RSRenderServiceConnectHub::GetInstance()->Destroy();
    client->GetScreenType(screenId, screenType);
    client->GetBitmap(nodeId, bm);
    client->GetPixelmap(nodeId, pixelmap, rect, drawCmdList);
    client->RegisterTypeface(typeface);
    client->UnRegisterTypeface(typeface);
    return true;
}

bool DoReportRsSceneJank()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    AppInfo info;
    info.startTime = GetData<int64_t>();
    info.endTime = GetData<int64_t>();
    info.pid = GetData<int32_t>();
    info.versionName = GetStringFromData(STR_LEN);
    info.versionCode = GetData<int32_t>();
    info.bundleName = GetStringFromData(STR_LEN);
    info.processName = GetStringFromData(STR_LEN);
    client->ReportRsSceneJankStart(info);
    client->ReportRsSceneJankEnd(info);
    return true;
}

bool DoGetHdrOnDuration()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    client->GetHdrOnDuration();
    return true;
}

bool DoSetWindowContainer()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    NodeId nodeId = GetData<NodeId>();
    bool value = GetData<bool>();

    std::string packageName = GetStringFromData(STR_LEN);
    std::string pageName = GetStringFromData(STR_LEN);
    bool isEnter = GetData<bool>();

    std::string nodeIdStr = GetStringFromData(STR_LEN);
    bool isColorFollow = GetData<bool>();

    client->SetWindowContainer(nodeId, value);
    client->NotifyPageName(packageName, pageName, isEnter);
    client->SetColorFollow(nodeIdStr, isColorFollow);
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
        case OHOS::Rosen::DO_GET_PIXELMAP_BY_PROCESSID:
            OHOS::Rosen::DoGetPixelMapByProcessId();
            break;
        case OHOS::Rosen::DO_TAKE_SELF_SURFACE_CAPTURE:
            OHOS::Rosen::DoTakeSelfSurfaceCapture();
            break;
        case OHOS::Rosen::DO_SET_WINDOW_FREEZE_IMMEDIATELY:
            OHOS::Rosen::DoSetWindowFreezeImmediately();
            break;
        case OHOS::Rosen::DO_SET_SCREEN_SECURITY_MASK:
            OHOS::Rosen::DoSetScreenSecurityMask();
            break;
        case OHOS::Rosen::DO_REPAINT_EVERYTHING:
            OHOS::Rosen::DoRepaintEverything();
            break;
        case OHOS::Rosen::DO_FORCE_REFRESH_ONE_FRAME_WITH_NEXT_VSYNC:
            OHOS::Rosen::DoForceRefreshOneFrameWithNextVSync();
            break;
        case OHOS::Rosen::DO_REGISTER_TYPEFACE:
            OHOS::Rosen::DoRegisterTypeface();
            break;
        case OHOS::Rosen::DO_REPORT_RS_SCENE_JANK:
            OHOS::Rosen::DoReportRsSceneJank();
            break;
        case OHOS::Rosen::DO_GET_HDR_ON_DURATION:
            OHOS::Rosen::DoGetHdrOnDuration();
            break;
        case OHOS::Rosen::DO_SET_WINDOW_CONTAINER:
            OHOS::Rosen::DoSetWindowContainer();
            break;
        default:
            return -1;
    }
    return 0;
}