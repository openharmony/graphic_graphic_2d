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
const uint8_t DO_COMMIT_TRANSACTION = 0;
const uint8_t DO_GET_UNI_RENDER_ENABLED = 1;
const uint8_t DO_CREATE_NODE = 2;
const uint8_t DO_CREATE_NODE_AND_SURFACE = 3;
const uint8_t DO_SET_FOCUS_APP_INFO = 4;
const uint8_t DO_SET_PHYSICAL_SCREEN_RESOLUTION = 5;
const uint8_t DO_SET_SCREEN_SECURITY_MASK = 6;
const uint8_t DO_SET_MIRROR_SCREEN_VISIBLE_RECT = 7;
const uint8_t DO_SET_CAST_SCREEN_ENABLE_SKIP_WINDOW = 8;
const uint8_t DO_MARK_POWER_OFF_NEED_PROCESS_ONE_FRAME = 9;
const uint8_t DO_DISABLE_RENDER_CONTROL_SCREEN = 10;
const uint8_t DO_SET_SCREEN_POWER_STATUS = 11;
const uint8_t DO_SET_SCREEN_BACK_LIGHT = 12;
const uint8_t DO_TAKE_SURFACE_CAPTURE = 13;
const uint8_t DO_SET_WINDOW_FREEZE_IMMEDIATELY = 14;
const uint8_t DO_SET_POINTER_POSITION = 15;
const uint8_t DO_GET_PIXELMAP_BY_PROCESSID = 16;
const uint8_t DO_REGISTER_APPLICATION_AGENT = 17;
const uint8_t DO_SET_BUFFER_AVAILABLE_LISTENER = 18;
const uint8_t DO_SET_BUFFER_CLEAR_LISTENER = 19;
const uint8_t DO_CREATE_VSYNC_CONNECTION = 20;
const uint8_t DO_REGISTER_OCCLUSION_CHANGE_CALLBACK = 21;
const uint8_t DO_SET_APP_WINDOW_NUM = 22;
const uint8_t DO_SET_SYSTEM_ANIMATED_SCENES = 23;
const uint8_t DO_REGISTER_HGM_CFG_CALLBACK = 24;
const uint8_t DO_SET_ROTATION_CACHE_ENABLED = 25;
const uint8_t DO_SET_TP_FEATURE_CONFIG = 26;
const uint8_t DO_SET_CURTAIN_SCREEN_USING_STATUS = 27;
const uint8_t DO_DROP_FRAME_BY_PID = 28;
const uint8_t DO_GET_LAYER_COMPOSE_INFO = 29;
const uint8_t DO_GET_HARDWARE_COMPOSE_DISABLED_REASON_INFO = 30;
const uint8_t DO_GET_HDR_ON_DURATION = 31;
const uint8_t DO_REGISTER_UIEXTENSION_CALLBACK = 32;
const uint8_t DO_SET_ANCO_FORCE_DO_DIRECT = 33;
const uint8_t DO_SET_VMA_CACHE_STATUS = 34;
const uint8_t DO_CREATE_DISPLAY_NODE = 35;
const uint8_t DO_SET_FREE_MULTI_WINDOW_STATUS = 36;
const uint8_t DO_REGISTER_SURFACE_BUFFER_CALLBACK = 37;
const uint8_t DO_UNREGISTER_SURFACE_BUFFER_CALLBACK = 38;
const uint8_t DO_SET_LAYER_TOP = 39;
const uint8_t DO_SET_SCREEN_ACTIVE_RECT = 40;
const uint8_t DO_SET_HIDE_PRIVACY_CONTENT = 41;
const uint8_t DO_REPAINT_EVERYTHING = 42;
const uint8_t DO_FORCE_REFRESH_ONE_FRAME_WITH_NEXT_VSYNC = 43;
const uint8_t DO_SET_WINDOW_CONTAINER = 44;
const uint8_t DO_REGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK = 45;
const uint8_t DO_NOTIFY_PAGE_NAME = 46;
const uint8_t DO_TAKE_SELF_SURFACE_CAPTURE = 47;
const uint8_t DO_SET_COLOR_FOLLOW = 48;
const uint8_t TARGET_SIZE = 49;

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

namespace Mock {

} // namespace Mock

void DoCommitTransaction()
{}

void DoGetUniRenderEnabled()
{}

void DoCreateNode()
{}

void DoCreateNodeAndSurface()
{}

void DoSetFocusAppInfo()
{}

void DoSetPhysicalScreenResolution()
{}

void DoSetScreenSecurityMask()
{}

void DoSetMirrorScreenVisibleRect()
{}

void DoSetCastScreenEnableSkipWindow()
{}

void DoMarkPowerOffNeedProcessOneFrame()
{}

void DoDisablePowerOffRenderControl()
{}

void DoSetScreenPowerStatus()
{}

void DoSetScreenBacklight()
{}

void DoTakeSurfaceCapture()
{}

void DoSetWindowFreezeImmediately()
{}

void DoSetHwcNodeBounds()
{}

void DoGetPixelMapByProcessId()
{}

void DoRegisterApplicationAgent()
{}

void DoRegisterBufferAvailableListener()
{}

void DoRegisterBufferClearListener()
{}

void DoCreateVSyncConnection()
{}

void DoRegisterOcclusionChangeCallback()
{}

void DoSetAppWindowNum()
{}

void DoSetSystemAnimatedScenes()
{}

void DoRegisterHgmConfigChangeCallback()
{}

void DoSetCacheEnabledForRotation()
{}

void DoSetTpFeatureConfig()
{}

void DoSetCurtainScreenUsingStatus()
{}

void DoDropFrameByPid()
{}

void DoGetLayerComposeInfo()
{}

void DoGetHwcDisabledReasonInfo()
{}

void DoGetHdrOnDuration()
{}

void DoRegisterUIExtensionCallback()
{}

void DoSetAncoForceDoDirect()
{}

void DoSetVmaCacheStatus()
{}

void DoCreateDisplayNode()
{}

void DoSetFreeMultiWindowStatus()
{}

void DoRegisterSurfaceBufferCallback()
{}

void DoUnregisterSurfaceBufferCallback()
{}

void DoSetLayerTop()
{}

void DoSetScreenActiveRect()
{}

void DoSetHidePrivacyContent()
{}

void DoRepaintEverything()
{}

void DoForceRefreshOneFrameWithNextVSync()
{}

void DoSetWindowContainer()
{}

void DoRegisterSelfDrawingNodeRectChangeCallback()
{}

void DoNotifyPageName()
{}

void DoTakeSelfSurfaceCapture()
{}

void DoSetColorFollow()
{}
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
        case OHOS::Rosen::DO_COMMIT_TRANSACTION:
            OHOS::Rosen::DoCommitTransaction();
            break;
        case OHOS::Rosen::DO_GET_UNI_RENDER_ENABLED:
            OHOS::Rosen::DoGetUniRenderEnabled();
            break;
        case OHOS::Rosen::DO_CREATE_NODE:
            OHOS::Rosen::DoCreateNode();
            break;
        case OHOS::Rosen::DO_CREATE_NODE_AND_SURFACE:
            OHOS::Rosen::DoCreateNodeAndSurface();
            break;
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
        case OHOS::Rosen::DO_MARK_POWER_OFF_NEED_PROCESS_ONE_FRAME:
            OHOS::Rosen::DoMarkPowerOffNeedProcessOneFrame();
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
        case OHOS::Rosen::DO_GET_PIXELMAP_BY_PROCESSID:
            OHOS::Rosen::DoGetPixelMapByProcessId();
            break;
        case OHOS::Rosen::DO_REGISTER_APPLICATION_AGENT:
            OHOS::Rosen::DoRegisterApplicationAgent();
            break;
        case OHOS::Rosen::DO_SET_BUFFER_AVAILABLE_LISTENER:
            OHOS::Rosen::DoRegisterBufferAvailableListener();
            break;
        case OHOS::Rosen::DO_SET_BUFFER_CLEAR_LISTENER:
            OHOS::Rosen::DoRegisterBufferClearListener();
            break;
        case OHOS::Rosen::DO_CREATE_VSYNC_CONNECTION:
            OHOS::Rosen::DoCreateVSyncConnection();
            break;
        case OHOS::Rosen::DO_REGISTER_OCCLUSION_CHANGE_CALLBACK:
            OHOS::Rosen::DoRegisterOcclusionChangeCallback();
            break;
        case OHOS::Rosen::DO_SET_APP_WINDOW_NUM:
            OHOS::Rosen::DoSetAppWindowNum();
            break;
        case OHOS::Rosen::DO_SET_SYSTEM_ANIMATED_SCENES:
            OHOS::Rosen::DoSetSystemAnimatedScenes();
            break;
        case OHOS::Rosen::DO_REGISTER_HGM_CFG_CALLBACK:
            OHOS::Rosen::DoRegisterHgmConfigChangeCallback();
            break;
        case OHOS::Rosen::DO_SET_ROTATION_CACHE_ENABLED:
            OHOS::Rosen::DoSetCacheEnabledForRotation();
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
        case OHOS::Rosen::DO_GET_LAYER_COMPOSE_INFO:
            OHOS::Rosen::DoGetLayerComposeInfo();
            break;
        case OHOS::Rosen::DO_GET_HARDWARE_COMPOSE_DISABLED_REASON_INFO:
            OHOS::Rosen::DoGetHwcDisabledReasonInfo();
            break;
        case OHOS::Rosen::DO_GET_HDR_ON_DURATION:
            OHOS::Rosen::DoGetHdrOnDuration();
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
        case OHOS::Rosen::DO_CREATE_DISPLAY_NODE:
            OHOS::Rosen::DoCreateDisplayNode();
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
        case OHOS::Rosen::DO_SET_HIDE_PRIVACY_CONTENT:
            OHOS::Rosen::DoSetHidePrivacyContent();
            break;
        case OHOS::Rosen::DO_REPAINT_EVERYTHING:
            OHOS::Rosen::DoRepaintEverything();
            break;
        case OHOS::Rosen::DO_FORCE_REFRESH_ONE_FRAME_WITH_NEXT_VSYNC:
            OHOS::Rosen::DoForceRefreshOneFrameWithNextVSync();
            break;
        case OHOS::Rosen::DO_SET_WINDOW_CONTAINER:
            OHOS::Rosen::DoSetWindowContainer();
            break;
        case OHOS::Rosen::DO_REGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK:
            OHOS::Rosen::DoRegisterSelfDrawingNodeRectChangeCallback();
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
