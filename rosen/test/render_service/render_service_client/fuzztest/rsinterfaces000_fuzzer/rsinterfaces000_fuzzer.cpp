/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "rsinterfaces000_fuzzer.h"

#include <securec.h>

#include "transaction/rs_interfaces.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t DO_SET_FOCUS_APP_INFO = 0;
const uint8_t DO_SET_PHYSICAL_SCREEN_RESOLUTION = 1;
const uint8_t DO_SET_SCREEN_SECURITY_MASK = 2;
const uint8_t DO_SET_MIRROR_SCREEN_VISIBLE_RECT = 3;
const uint8_t DO_SET_CAST_SCREEN_ENABLE_SKIP_WINDOW = 4;
const uint8_t DO_DISABLE_RENDER_CONTROL_SCREEN = 5;
const uint8_t DO_SET_SCREEN_POWER_STATUS = 6;
const uint8_t DO_SET_SCREEN_BACK_LIGHT = 7;
const uint8_t DO_TAKE_SURFACE_CAPTURE = 8;
const uint8_t DO_SET_WINDOW_FREEZE_IMMEDIATELY = 9;
const uint8_t DO_SET_POINTER_POSITION = 10;
const uint8_t DO_SET_APP_WINDOW_NUM = 11;
const uint8_t DO_SET_SYSTEM_ANIMATED_SCENES = 12;
const uint8_t DO_SET_TP_FEATURE_CONFIG = 13;
const uint8_t DO_SET_CURTAIN_SCREEN_USING_STATUS = 14;
const uint8_t DO_DROP_FRAME_BY_PID = 15;
const uint8_t DO_REGISTER_UIEXTENSION_CALLBACK = 16;
const uint8_t DO_SET_ANCO_FORCE_DO_DIRECT = 17;
const uint8_t DO_SET_VMA_CACHE_STATUS = 18;
const uint8_t DO_SET_FREE_MULTI_WINDOW_STATUS = 19;
const uint8_t DO_REGISTER_SURFACE_BUFFER_CALLBACK = 20;
const uint8_t DO_UNREGISTER_SURFACE_BUFFER_CALLBACK = 21;
const uint8_t DO_SET_LAYER_TOP = 22;
const uint8_t DO_SET_SCREEN_ACTIVE_RECT = 23;
const uint8_t DO_SET_WINDOW_CONTAINER = 24;
const uint8_t DO_NOTIFY_PAGE_NAME = 25;
const uint8_t DO_TAKE_SELF_SURFACE_CAPTURE = 26;
const uint8_t DO_SET_COLOR_FOLLOW = 27;
const uint8_t TARGET_SIZE = 28;

const uint8_t* DATA = nullptr;
size_t g_size = 0;
size_t g_pos;
constexpr size_t STR_LEN = 10;
constexpr uint8_t SCREEN_POWER_STATUS_SIZE = 11;

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

namespace Mock {

} // namespace Mock

void DoSetFocusAppInfo()
{
    FocusAppInfo info;
    info.pid = GetData<int32_t>();
    info.uid = GetData<int32_t>();
    info.bundleName = GetStringFromData(STR_LEN);
    info.abilityName = GetStringFromData(STR_LEN);
    info.focusNodeId = GetData<uint64_t>();
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetFocusAppInfo(info);
}

void DoSetPhysicalScreenResolution()
{
#ifndef ROSEN_ARKUI_X
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    ScreenId id = GetData<uint64_t>();
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetPhysicalScreenResolution(id, width, height);
#endif
}

void DoSetScreenSecurityMask()
{
    auto& rsInterfaces = RSInterfaces::GetInstance();
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
    rsInterfaces.SetScreenSecurityMask(id, securityMask);
}

void DoSetMirrorScreenVisibleRect()
{
    Rect rect;
    rect.x = GetData<int32_t>();
    rect.y = GetData<int32_t>();
    rect.w = GetData<int32_t>();
    rect.h = GetData<int32_t>();
    ScreenId id = GetData<uint64_t>();
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetMirrorScreenVisibleRect(id, rect);
}

void DoSetCastScreenEnableSkipWindow()
{
    uint64_t id = GetData<uint64_t>();
    bool enable = GetData<bool>();
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetCastScreenEnableSkipWindow(id, enable);
}

void DoDisablePowerOffRenderControl()
{
    auto& rsInterfaces = RSInterfaces::GetInstance();
    ScreenId id = GetData<ScreenId>();
    rsInterfaces.DisablePowerOffRenderControl(id);
}

void DoSetScreenPowerStatus()
{
    auto& rsInterfaces = RSInterfaces::GetInstance();
    ScreenId id = GetData<ScreenId>();
    ScreenPowerStatus status = static_cast<ScreenPowerStatus>(GetData<uint8_t>() % SET_SCREEN_POWER_STATUS);
    rsInterfaces.SetScreenPowerStatus(id, status);
}

void DoSetScreenBacklight()
{
    ScreenId id = GetData<uint64_t>();
    uint32_t level = GetData<uint32_t>();
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetScreenBacklight(id, level);
}

class TestSurfaceCaptureCallback : public SurfaceCaptureCallback {
public:
    explicit TestSurfaceCaptureCallback() {}
    ~TestSurfaceCaptureCallback() override {}
    void OnSurfaceCapture(std::shared_ptr<Media::PixelMap> pixelmap) override
    {
    }
};

void DoTakeSurfaceCapture()
{
    auto& rsInterfaces = RSInterfaces::GetInstance();
    RSDisplayNodeConfig config;
    config.mirrorNodeId = GetData<uint64_t>();
    config.screenId = GetData<uint64_t>();
    config.isMirrored = GetData<bool>();
    config.isSync = GetData<bool>();
    auto displayNode = RSDisplayNode::Create(config);

    std::shared_ptr<TestSurfaceCaptureCallback> callback = std::make_shared<TestSurfaceCaptureCallback>();
    RSSurfaceNodeConfig surfaceConfig;
    surfaceConfig.surfaceId = static_cast<NodeId>(GetData<uint64_t>());
    auto surfaceNode = RSSurfaceNode::Create(surfaceConfig);

    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = GetData<float>();
    captureConfig.scaleY = GetData<float>();
    captureConfig.useDma = GetData<bool>();
    captureConfig.useCurWindow = GetData<bool>();
    uint8_t type = GetData<uint8_t>();
    captureConfig.captureType = static_cast<SurfaceCaptureType>(type);
    captureConfig.isSync = GetData<bool>();
    uint8_t listSize = GetData<uint8_t>();
    for (auto i = 0; i < listSize; i++) {
        uint64_t nodeId = GetData<uint64_t>();
        captureConfig.blackList.push_back(nodeId);
    }
    captureConfig.mainScreenRect.left_ = GetData<float>();
    captureConfig.mainScreenRect.top_ = GetData<float>();
    captureConfig.mainScreenRect.right_ = GetData<float>();
    captureConfig.mainScreenRect.bottom_ = GetData<float>();
    rsInterfaces.TakeSurfaceCapture(surfaceNode, callback, captureConfig);
    uint64_t nodeId2 = GetData<uint64_t>();
    rsInterfaces.TakeSurfaceCapture(nodeId2, callback, captureConfig);
    rsInterfaces.TakeSurfaceCapture(displayNode, callback, captureConfig);
}

void DoSetWindowFreezeImmediately()
{
    RSSurfaceNodeConfig surfaceConfig;
    surfaceConfig.surfaceId = static_cast<NodeId>(GetData<uint64_t>());
    auto surfaceNode = RSSurfaceNode::Create(surfaceConfig);
    bool isFreeze = GetData<bool>();
    std::shared_ptr<TestSurfaceCaptureCallback> callback = std::make_shared<TestSurfaceCaptureCallback>();

    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = GetData<float>();
    captureConfig.scaleY = GetData<float>();
    captureConfig.useDma = GetData<bool>();
    captureConfig.useCurWindow = GetData<bool>();
    uint8_t type = GetData<uint8_t>();
    captureConfig.captureType = static_cast<SurfaceCaptureType>(type);
    captureConfig.isSync = GetData<bool>();
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetWindowFreezeImmediately(surfaceNode, isFreeze, callback, captureConfig);
}

void DoSetHwcNodeBounds()
{
    NodeId id = GetData<uint64_t>();
    float positionX = GetData<float>();
    float positionY = GetData<float>();
    float positionZ = GetData<float>();
    float positionW = GetData<float>();
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetWindowFreezeImmediately(id, positionX, positionY, positionZ, positionW);
}

void DoSetAppWindowNum()
{
    uint32_t num = GetData<uint32_t>();
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetAppWindowNum(num);
}

void DoSetSystemAnimatedScenes()
{
    uint32_t systemAnimatedScenes = GetData<uint32_t>();
    bool isRegularAnimation = GetData<bool>();
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetSystemAnimatedScenes(systemAnimatedScenes, isRegularAnimation);
}

void DoSetTpFeatureConfig()
{
#ifdef TP_FEATURE_ENABLE
    auto& rsInterfaces = RSInterfaces::GetInstance();
    int32_t feature = GetData<int32_t>();
    char config = GetData<char>();
    auto tpFeatureConfigType = static_cast<TpFeatureConfigType>(GetData<uint8_t>());
    rsInterfaces.SetTpFeatureConfig(feature, config.c_str(), tpFeatureConfigType);
#endif
}

void DoSetCurtainScreenUsingStatus()
{
    auto& rsInterfaces = RSInterfaces::GetInstance();
    bool isCurtainScreenOn = GetData<bool>();
    rsInterfaces.SetCurtainScreenUsingStatus(isCurtainScreenOn);
}

void DoDropFrameByPid()
{
    auto& rsInterfaces = RSInterfaces::GetInstance();
    std::vector<int32_t> pidList;
    uint8_t pidListSize = GetData<uint8_t>();
    for (size_t i = 0; i < pidListSize; i++) {
        pidList.push_back(GetData<int32_t>());
    }
    rsInterfaces.DropFrameByPid(pidList);
}

void DoRegisterUIExtensionCallback()
{
    auto& rsInterfaces = RSInterfaces::GetInstance();
    uint64_t userId = GetData<uint64_t>();
    UIExtensionCallback callback;
    bool unobscured = GetData<bool>();
    rsInterfaces.RegisterUIExtensionCallback(userId, callback, unobscured);
}

void DoSetAncoForceDoDirect()
{
    bool direct = GetData<bool>();
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetAncoForceDoDirect(direct);
}

void DoSetVmaCacheStatus()
{
    bool flag = GetData<bool>();
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetVmaCacheStatus(flag);
}

void DoSetFreeMultiWindowStatus()
{
    bool enable = GetData<bool>();
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetFreeMultiWindowStatus(enable);
}

void DoRegisterSurfaceBufferCallback()
{
    pid_t pid = GetData<pid_t>();
    uint64_t uid = GetData<uint64_t>();
    std::shared_ptr<SurfaceBufferCallback> callback;
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.RegisterSurfaceBufferCallback(pid, uid, callback);
}

void DoUnregisterSurfaceBufferCallback()
{
    pid_t pid = GetData<pid_t>();
    uint64_t uid = GetData<uint64_t>();
    std::shared_ptr<SurfaceBufferCallback> callback;
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.RegisterSurfaceBufferCallback(pid, uid, callback);
    rsInterfaces.UnregisterSurfaceBufferCallback(pid, uid);
}

void DoSetLayerTop()
{
    std::string nodeIdStr = GetStringFromData(STR_LEN);
    bool isTop = GetData<bool>();
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetLayerTop(nodeIdStr, isTop);
}

void DoSetScreenActiveRect()
{
    ScreenId id = GetData<uint64_t>();
    int32_t x = GetData<int32_t>();
    int32_t y = GetData<int32_t>();
    int32_t w = GetData<int32_t>();
    int32_t h = GetData<int32_t>();
    Rect activeRect {
        .x = x,
        .y = y,
        .w = w,
        .h = h
    };
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetScreenActiveRect(id, activeRect);
}

void DoSetWindowContainer()
{
    NodeId nodeId = GetData<NodeId>();
    bool value = GetData<bool>();
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetWindowContainer(nodeId, value);
}

void DoNotifyPageName()
{
    std::string packageName = GetStringFromData(STR_LEN);
    std::string pageName = GetStringFromData(STR_LEN);
    bool isEnter = GetData<bool>();
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.NotifyPageName(packageName, pageName, isEnter);
}

void DoTakeSelfSurfaceCapture()
{
    RSSurfaceNodeConfig surfaceConfig;
    surfaceConfig.surfaceId = static_cast<NodeId>(GetData<uint64_t>());
    auto surfaceNode = RSSurfaceNode::Create(surfaceConfig);
    std::shared_ptr<SurfaceCaptureCallback> callback;
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = GetData<float>();
    captureConfig.scaleY = GetData<float>();
    captureConfig.useDma = GetData<bool>();
    captureConfig.useCurWindow = GetData<bool>();
    uint8_t type = GetData<uint8_t>();
    captureConfig.captureType = static_cast<SurfaceCaptureType>(type);
    captureConfig.isSync = GetData<bool>();
    uint8_t listSize = GetData<uint8_t>();
    for (auto i = 0; i < listSize; i++) {
        uint64_t nodeId = GetData<uint64_t>();
        captureConfig.blackList.push_back(nodeId);
    }
    captureConfig.mainScreenRect.left_ = GetData<float>();
    captureConfig.mainScreenRect.top_ = GetData<float>();
    captureConfig.mainScreenRect.right_ = GetData<float>();
    captureConfig.mainScreenRect.bottom_ = GetData<float>();
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.TakeSelfSurfaceCapture(surfaceNode, callback, captureConfig);
}

void DoSetColorFollow()
{
    std::string nodeIdStr = GetStringFromData(STR_LEN);
    bool isColorFollow = GetData<bool>();
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetColorFollow(nodeIdStr, isColorFollow);
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
if (!OHOS::Rosen::Init(data, size)) {
        return -1;
    }
    /* Run your code on data */
    uint8_t tarPos = OHOS::Rosen::GetData<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_SET_FOCUS_APP_INFO:
            OHOS::Rosen::DoSetFocusAppInfo();
            break;
        case OHOS::Rosen::DO_SET_PHYSICAL_SCREEN_RESOLUTION:
            OHOS::Rosen::DoSetPhysicalScreenResolution();
            break;
        case OHOS::Rosen::DO_SET_SCREEN_SECURITY_MASK:
            OHOS::Rosen::DoSetScreenSecurityMask();
            break;
        case OHOS::Rosen::DO_SET_MIRROR_SCREEN_VISIBLE_RECT:
            OHOS::Rosen::DoSetMirrorScreenVisibleRect();
            break;
        case OHOS::Rosen::DO_SET_CAST_SCREEN_ENABLE_SKIP_WINDOW:
            OHOS::Rosen::DoSetCastScreenEnableSkipWindow();
            break;
        case OHOS::Rosen::DO_DISABLE_RENDER_CONTROL_SCREEN:
            OHOS::Rosen::DoDisablePowerOffRenderControl();
            break;
        case OHOS::Rosen::DO_SET_SCREEN_POWER_STATUS:
            OHOS::Rosen::DoSetScreenPowerStatus();
            break;
        case OHOS::Rosen::DO_SET_SCREEN_BACK_LIGHT:
            OHOS::Rosen::DoSetScreenBacklight();
            break;
        case OHOS::Rosen::DO_TAKE_SURFACE_CAPTURE:
            OHOS::Rosen::DoTakeSurfaceCapture();
            break;
        case OHOS::Rosen::DO_SET_WINDOW_FREEZE_IMMEDIATELY:
            OHOS::Rosen::DoSetWindowFreezeImmediately();
            break;
        case OHOS::Rosen::DO_SET_POINTER_POSITION:
            OHOS::Rosen::DoSetHwcNodeBounds();
            break;
        case OHOS::Rosen::DO_SET_APP_WINDOW_NUM:
            OHOS::Rosen::DoSetAppWindowNum();
            break;
        case OHOS::Rosen::DO_SET_SYSTEM_ANIMATED_SCENES:
            OHOS::Rosen::DoSetSystemAnimatedScenes();
            break;
        case OHOS::Rosen::DO_SET_TP_FEATURE_CONFIG:
            OHOS::Rosen::DoSetTpFeatureConfig();
            break;
        case OHOS::Rosen::DO_SET_CURTAIN_SCREEN_USING_STATUS:
            OHOS::Rosen::DoSetCurtainScreenUsingStatus();
            break;
        case OHOS::Rosen::DO_DROP_FRAME_BY_PID:
            OHOS::Rosen::DoDropFrameByPid();
            break;
        case OHOS::Rosen::DO_REGISTER_UIEXTENSION_CALLBACK:
            OHOS::Rosen::DoRegisterUIExtensionCallback();
            break;
        case OHOS::Rosen::DO_SET_ANCO_FORCE_DO_DIRECT:
            OHOS::Rosen::DoSetAncoForceDoDirect();
            break;
        case OHOS::Rosen::DO_SET_VMA_CACHE_STATUS:
            OHOS::Rosen::DoSetVmaCacheStatus();
            break;
        case OHOS::Rosen::DO_SET_FREE_MULTI_WINDOW_STATUS:
            OHOS::Rosen::DoSetFreeMultiWindowStatus();
            break;
        case OHOS::Rosen::DO_REGISTER_SURFACE_BUFFER_CALLBACK:
            OHOS::Rosen::DoRegisterSurfaceBufferCallback();
            break;
        case OHOS::Rosen::DO_UNREGISTER_SURFACE_BUFFER_CALLBACK:
            OHOS::Rosen::DoUnregisterSurfaceBufferCallback();
            break;
        case OHOS::Rosen::DO_SET_LAYER_TOP:
            OHOS::Rosen::DoSetLayerTop();
            break;
        case OHOS::Rosen::DO_SET_SCREEN_ACTIVE_RECT:
            OHOS::Rosen::DoSetScreenActiveRect();
            break;
        case OHOS::Rosen::DO_SET_WINDOW_CONTAINER:
            OHOS::Rosen::DoSetWindowContainer();
            break;
        case OHOS::Rosen::DO_NOTIFY_PAGE_NAME:
            OHOS::Rosen::DoNotifyPageName();
            break;
        case OHOS::Rosen::DO_TAKE_SELF_SURFACE_CAPTURE:
            OHOS::Rosen::DoTakeSelfSurfaceCapture();
            break;
        case OHOS::Rosen::DO_SET_COLOR_FOLLOW:
            OHOS::Rosen::DoSetColorFollow();
            break;
        default:
            return -1;
    }
    return 0;
}
