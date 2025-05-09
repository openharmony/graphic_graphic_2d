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

#include "rsrenderserviceclient02_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "core/transaction/rs_interfaces.h"

#include "command/rs_command.h"
#include "command/rs_node_showing_command.h"
#include "transaction/rs_render_service_client.h"
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
constexpr int32_t DATA_LENGTH = 200;
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

bool DoGetPixelMapByProcessId(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    std::vector<PixelMapInfo> pixelMapInfoVector;
    pid_t pid = GetData<pid_t>();
    client->GetPixelMapByProcessId(pixelMapInfoVector, pid);
    return true;
}

bool DoTakeSelfSurfaceCapture(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

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
    RSRenderServiceConnectHub::GetInstance()->Destroy();
    client->TakeSelfSurfaceCapture(nodeId, callback, captureConfig);
    return true;
}

bool DoSetWindowFreezeImmediately(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

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
    RSRenderServiceConnectHub::GetInstance()->Destroy();
    client->SetWindowFreezeImmediately(nodeId, isFreeze, callback, captureConfig);
    return true;
}

bool DoSetScreenSecurityMask(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

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
    RSRenderServiceConnectHub::GetInstance()->Destroy();
    client->SetScreenSecurityMask(id, securityMask);
    return true;
}

bool DoRepaintEverything(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    client->RepaintEverything();
    RSRenderServiceConnectHub::GetInstance()->Destroy();
    client->RepaintEverything();
    return true;
}

bool DoForceRefreshOneFrameWithNextVSync(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    client->ForceRefreshOneFrameWithNextVSync();
    RSRenderServiceConnectHub::GetInstance()->Destroy();
    client->ForceRefreshOneFrameWithNextVSync();
    return true;
}

bool DoRegisterTypeface(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    std::shared_ptr<Drawing::Typeface> typeface;
    int32_t dstPid = GetData<int32_t>();
    FrameRateLinkerExpectedFpsUpdateCallback callback;
    uint32_t num = GetData<uint32_t>();
    bool enable = GetData<bool>();
    pid_t pid = GetData<pid_t>();
    uint64_t uid = GetData<uint64_t>();

    client->RegisterTypeface(typeface);
    RSRenderServiceConnectHub::GetInstance()->Destroy();
    client->RegisterFrameRateLinkerExpectedFpsUpdateCallback(dstPid, callback);
    client->SetAppWindowNum(num);
    client->SetFreeMultiWindowStatus(enable);
    client->UnregisterSurfaceBufferCallback(pid, uid);

    return true;
}

bool DoReportRsSceneJank(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    AppInfo info;
    info.startTime = GetData<int64_t>();
    info.endTime = GetData<int64_t>();
    info.pid = GetData<int32_t>();
    info.versionName = GetStringFromData(DATA_LENGTH);
    info.versionCode = GetData<int32_t>();
    info.bundleName = GetStringFromData(DATA_LENGTH);
    info.processName = GetStringFromData(DATA_LENGTH);

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    client->ReportRsSceneJankStart(info);
    client->ReportRsSceneJankEnd(info);
    RSRenderServiceConnectHub::GetInstance()->Destroy();
    client->ReportRsSceneJankStart(info);
    client->ReportRsSceneJankEnd(info);
    return true;
}

bool DoGetHdrOnDuration(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    client->GetHdrOnDuration();
    RSRenderServiceConnectHub::GetInstance()->Destroy();
    client->GetHdrOnDuration();
    return true;
}

bool DoSetWindowContainer(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    DATA = data;
    g_size = size;
    g_pos = 0;
    NodeId nodeId = GetData<NodeId>();
    bool value = GetData<bool>();

    std::string packageName = GetStringFromData(DATA_LENGTH);
    std::string pageName = GetStringFromData(DATA_LENGTH);
    bool isEnter = GetData<bool>();

    std::string nodeIdStr = GetStringFromData(DATA_LENGTH);
    bool isColorFollow = GetData<bool>();

    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    client->SetWindowContainer(nodeId, value);
    client->NotifyPageName(packageName, pageName, isEnter);
    client->SetColorFollow(nodeIdStr, isColorFollow);
    RSRenderServiceConnectHub::GetInstance()->Destroy();
    client->NotifyPageName(packageName, pageName, isEnter);
    return true;
}

} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoGetPixelMapByProcessId(data, size);
    OHOS::Rosen::DoTakeSelfSurfaceCapture(data, size);
    OHOS::Rosen::DoSetWindowFreezeImmediately(data, size);
    OHOS::Rosen::DoSetScreenSecurityMask(data, size);
    OHOS::Rosen::DoRepaintEverything(data, size);
    OHOS::Rosen::DoForceRefreshOneFrameWithNextVSync(data, size);
    OHOS::Rosen::DoRegisterTypeface(data, size);
    OHOS::Rosen::DoReportRsSceneJank(data, size);
    OHOS::Rosen::DoGetHdrOnDuration(data, size);
    OHOS::Rosen::DoSetWindowContainer(data, size);
    return 0;
}