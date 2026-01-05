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

#include "rsrenderserviceconnection000_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <unistd.h>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <unordered_map>
#include <fuzzer/FuzzedDataProvider.h>

#include "pipeline/main_thread/rs_main_thread.h"
#include "transaction/rs_client_to_render_connection.h"
#include "render_server/transaction/rs_client_to_service_connection.h"
#include "pipeline/rs_render_node_gc.h"
#include "pipeline/rs_surface_buffer_callback_manager.h"
#include "platform/ohos/rs_irender_service.h"
#include "transaction/zidl/rs_client_to_render_connection_stub.h"
#include "render_server/transaction/zidl/rs_client_to_service_connection_stub.h"
#include "transaction/rs_transaction_proxy.h"
#include "message_parcel.h"
#include "securec.h"
#include <iservice_registry.h>
#include "ipc_callbacks/rs_occlusion_change_callback_stub.h"
#include "ipc_callbacks/hgm_config_change_callback_stub.h"
#include "ipc_callbacks/rs_uiextension_callback_stub.h"
#include "ipc_callbacks/rs_self_drawing_node_rect_change_callback_stub.h"
#include <system_ability_definition.h>

namespace OHOS {
namespace Rosen {

int32_t g_pid;
sptr<OHOS::Rosen::RSScreenManager> screenManagerPtr_ = nullptr;
[[maybe_unused]] auto& rsRenderNodeGC = RSRenderNodeGC::Instance();
[[maybe_unused]] auto& rsSurfaceBufferCallbackManager = RSSurfaceBufferCallbackManager::Instance();
RSMainThread* mainThread_ = RSMainThread::Instance();
sptr<RSClientToServiceConnectionStub> toServiceConnectionStub_ = nullptr;

namespace {
const uint8_t DO_GET_UNI_RENDER_ENABLED = 0;
const uint8_t DO_SET_PHYSICAL_SCREEN_RESOLUTION = 1;
const uint8_t DO_SET_SCREEN_SECURITY_MASK = 2;
const uint8_t DO_SET_MIRROR_SCREEN_VISIBLE_RECT = 3;
const uint8_t DO_SET_CAST_SCREEN_ENABLE_SKIP_WINDOW = 4;
const uint8_t DO_MARK_POWER_OFF_NEED_PROCESS_ONE_FRAME = 5;
const uint8_t DO_DISABLE_RENDER_CONTROL_SCREEN = 6;
const uint8_t DO_SET_SCREEN_POWER_STATUS = 7;
const uint8_t DO_SET_SCREEN_BACK_LIGHT = 8;
const uint8_t DO_GET_PIXELMAP_BY_PROCESSID = 9;
const uint8_t DO_CREATE_VSYNC_CONNECTION = 10;
const uint8_t DO_REGISTER_OCCLUSION_CHANGE_CALLBACK = 11;
const uint8_t DO_SET_APP_WINDOW_NUM = 12;
const uint8_t DO_SET_SYSTEM_ANIMATED_SCENES = 13;
const uint8_t DO_REGISTER_HGM_CFG_CALLBACK = 14;
const uint8_t DO_SET_ROTATION_CACHE_ENABLED = 15;
const uint8_t DO_SET_TP_FEATURE_CONFIG = 16;
const uint8_t DO_SET_CURTAIN_SCREEN_USING_STATUS = 17;
const uint8_t DO_GET_LAYER_COMPOSE_INFO = 18;
const uint8_t DO_GET_HARDWARE_COMPOSE_DISABLED_REASON_INFO = 19;
const uint8_t DO_GET_HDR_ON_DURATION = 20;
const uint8_t DO_REGISTER_UIEXTENSION_CALLBACK = 21;
const uint8_t DO_SET_VMA_CACHE_STATUS = 22;
const uint8_t DO_SET_FREE_MULTI_WINDOW_STATUS = 23;
const uint8_t DO_SET_LAYER_TOP = 24;
const uint8_t DO_SET_SCREEN_ACTIVE_RECT = 25;
const uint8_t DO_REPAINT_EVERYTHING = 26;
const uint8_t DO_FORCE_REFRESH_ONE_FRAME_WITH_NEXT_VSYNC = 27;
const uint8_t DO_REGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK = 28;
const uint8_t DO_NOTIFY_PAGE_NAME = 29;
const uint8_t DO_SET_COLOR_FOLLOW = 30;
const uint8_t DO_SET_FORCE_REFRESH = 31;
const uint8_t TARGET_SIZE = 32;
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

void DoGetUniRenderEnabled()
{
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_UNI_RENDER_ENABLED);
    toServiceConnectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoSetPhysicalScreenResolution()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return;
    }
    uint64_t id = GetData<uint64_t>();
    if (!data.WriteUint64(id)) {
        return;
    }
    uint32_t width = GetData<uint32_t>();
    if (!data.WriteUint32(width)) {
        return;
    }
    uint32_t height = GetData<uint32_t>();
    if (!data.WriteUint32(height)) {
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_PHYSICAL_SCREEN_RESOLUTION);
    toServiceConnectionStub_->OnRemoteRequest(code, data, reply, option);
}

void DoSetScreenSecurityMask()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_SECURITY_MASK);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;

    uint64_t screenId = GetData<uint64_t>();
    bool enable = GetData<bool>();
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteUint64(screenId);
    dataParcel.WriteBool(enable);
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetMirrorScreenVisibleRect()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_MIRROR_SCREEN_VISIBLE_RECT);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;

    uint64_t screenId = GetData<uint64_t>();
    bool supportRotation = GetData<bool>();
    int32_t x = GetData<int32_t>();
    int32_t y = GetData<int32_t>();
    int32_t w = GetData<int32_t>();
    int32_t h = GetData<int32_t>();
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteUint64(screenId);
    dataParcel.WriteInt32(x);
    dataParcel.WriteInt32(y);
    dataParcel.WriteInt32(w);
    dataParcel.WriteInt32(h);
    dataParcel.WriteBool(supportRotation);
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetCastScreenEnableSkipWindow()
{
    uint64_t id = GetData<uint64_t>();
    bool enable = GetData<bool>();
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!dataP.WriteUint64(id)) {
        return;
    }
    if (!dataP.WriteBool(enable)) {
        return;
    }

    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_CAST_SCREEN_ENABLE_SKIP_WINDOW);
    toServiceConnectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoMarkPowerOffNeedProcessOneFrame()
{
    uint32_t code =
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::MARK_POWER_OFF_NEED_PROCESS_ONE_FRAME);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoDisablePowerOffRenderControl()
{
    uint32_t code =
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::DISABLE_RENDER_CONTROL_SCREEN);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    uint64_t screenId = GetData<uint64_t>();
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteUint64(screenId);
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetScreenPowerStatus()
{
    uint32_t code =
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_POWER_STATUS);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    uint64_t screenId = GetData<uint64_t>();
    uint32_t status = GetData<uint32_t>();
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteUint64(screenId);
    dataParcel.WriteUint32(status);
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetScreenBacklight()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_BACK_LIGHT);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    ScreenId id = GetData<uint64_t>();
    uint32_t level = GetData<uint32_t>();
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteUint64(id);
    dataParcel.WriteUint32(level);
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoGetPixelMapByProcessId()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_PIXELMAP_BY_PROCESSID);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    uint64_t pid = GetData<uint64_t>();
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteUint64(pid);
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoCreateVSyncConnection()
{
    uint32_t code =
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_VSYNC_CONNECTION);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    sptr<VSyncIConnectionToken> vsyncIConnectionToken_ = new IRemoteStub<VSyncIConnectionToken>();
    uint64_t id = GetData<uint64_t>();
    NodeId windowNodeID = GetData<NodeId>();
    std::string name = GetData<std::string>();
    dataParcel.WriteString(name);
    bool fromXcomponent = GetData<bool>();
    dataParcel.WriteRemoteObject(vsyncIConnectionToken_->AsObject());
    dataParcel.WriteUint64(id);
    dataParcel.WriteUint64(windowNodeID);
    dataParcel.WriteBool(fromXcomponent);
    dataParcel.RewindRead(0);
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

class CustomTestOcclusionChangeCallback : public RSOcclusionChangeCallbackStub {
public:
    explicit CustomTestOcclusionChangeCallback(const OcclusionChangeCallback &callback) : cb_(callback)
    {}
    ~CustomTestOcclusionChangeCallback() override{};

    void OnOcclusionVisibleChanged(std::shared_ptr<RSOcclusionData> occlusionData) override
    {
        if (cb_ != nullptr) {
            cb_(occlusionData);
        }
    }

private:
    OcclusionChangeCallback cb_;
};

void DoRegisterOcclusionChangeCallback()
{
    uint32_t code =
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_OCCLUSION_CHANGE_CALLBACK);

    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    std::shared_ptr<RSOcclusionData> occlusionData = std::make_shared<RSOcclusionData>();
    auto callback = [&occlusionData](std::shared_ptr<RSOcclusionData> data) { occlusionData = data; };
    sptr<CustomTestOcclusionChangeCallback> rsIOcclusionChangeCallback_ =
        new CustomTestOcclusionChangeCallback(callback);

    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteRemoteObject(rsIOcclusionChangeCallback_->AsObject());
    dataParcel.RewindRead(0);
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetAppWindowNum()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_APP_WINDOW_NUM);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;

    uint32_t num = GetData<uint32_t>();
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteUint32(num);
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetSystemAnimatedScenes()
{
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t systemAnimatedScenes = GetData<uint32_t>();
    bool isRegularAnimation = GetData<bool>();
    if (!dataP.WriteUint32(systemAnimatedScenes) || !dataP.WriteBool(isRegularAnimation)) {
        return;
    }
    uint32_t code = static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::SET_SYSTEM_ANIMATED_SCENES);
    toServiceConnectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

class CustomTestHgmConfigChangeCallback : public RSHgmConfigChangeCallbackStub {
public:
    explicit CustomTestHgmConfigChangeCallback(const HgmConfigChangeCallback &callback) : cb_(callback)
    {}
    ~CustomTestHgmConfigChangeCallback() override{};

    void OnHgmConfigChanged(std::shared_ptr<RSHgmConfigData> configData) override
    {
        if (cb_ != nullptr) {
            cb_(configData);
        }
    }
    void OnHgmRefreshRateModeChanged(int32_t refreshRateMode) override
    {}
    void OnHgmRefreshRateUpdate(int32_t refreshRate) override
    {}

private:
    HgmConfigChangeCallback cb_;
};

void DoRegisterHgmConfigChangeCallback()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_HGM_CFG_CALLBACK);

    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    std::shared_ptr<RSHgmConfigData> configData = std::make_shared<RSHgmConfigData>();
    auto callback = [&configData](std::shared_ptr<RSHgmConfigData> data) { configData = data; };
    sptr<CustomTestHgmConfigChangeCallback> rsIHgmConfigChangeCallback_ =
        new CustomTestHgmConfigChangeCallback(callback);

    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteRemoteObject(rsIHgmConfigChangeCallback_->AsObject());
    dataParcel.RewindRead(0);
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetCacheEnabledForRotation()
{
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return;
    }
    bool isEnabled = GetData<bool>();
    if (!dataP.WriteBool(isEnabled)) {
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_ROTATION_CACHE_ENABLED);
    toServiceConnectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoSetTpFeatureConfig()
{
#ifdef TP_FEATURE_ENABLE
    uint32_t code =
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_TP_FEATURE_CONFIG);

    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    int32_t feature = GetData<int32_t>();
    std::string config = GetData<std::string>();
    // To ensure that tpFeatureConfigType falls within the valid range more frequently, apply a modulo operation
    // to it. This allows a limited set of seeds to cover all valid values within the range.
    uint8_t tpFeatureConfigType = GetData<uint8_t>() %
        (static_cast<uint8_t>(TpFeatureConfigType::AFT_TP_FEATURE) + 2);

    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteInt32(feature);
    dataParcel.WriteCString(config.c_str());
    dataParcel.WriteUint8(tpFeatureConfigType);
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
#endif
}

void DoSetCurtainScreenUsingStatus()
{
    bool status = GetData<bool>();
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!dataP.WriteBool(status)) {
        return;
    }

    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_CURTAIN_SCREEN_USING_STATUS);
    toServiceConnectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoGetLayerComposeInfo()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_LAYER_COMPOSE_INFO);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoGetHwcDisabledReasonInfo()
{
    uint32_t code =
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_HARDWARE_COMPOSE_DISABLED_REASON_INFO);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoGetHdrOnDuration()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_HDR_ON_DURATION);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

class CustomTestUIExtensionCallback : public RSUIExtensionCallbackStub {
public:
    explicit CustomTestUIExtensionCallback(const UIExtensionCallback &callback) : cb_(callback)
    {}
    ~CustomTestUIExtensionCallback() override{};

    void OnUIExtension(std::shared_ptr<RSUIExtensionData> uiextensionData, uint64_t userId) override
    {
        if (cb_ != nullptr) {
            cb_(uiextensionData, userId);
        }
    }

private:
    UIExtensionCallback cb_;
};

void DoRegisterUIExtensionCallback()
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    uint32_t code =
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_UIEXTENSION_CALLBACK);

    std::shared_ptr<RSUIExtensionData> uiextensionData = std::make_shared<RSUIExtensionData>();
    uint64_t id = GetData<uint64_t>();
    auto callback = [&uiextensionData, &id](std::shared_ptr<RSUIExtensionData> data, uint64_t userId) {
        uiextensionData = data;
        id = userId;
    };
    sptr<CustomTestUIExtensionCallback> rsIUIExtensionCallback =
        new CustomTestUIExtensionCallback(callback);

    bool unobscured = GetData<bool>();
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteUint64(id);
    dataParcel.WriteRemoteObject(rsIUIExtensionCallback->AsObject());
    dataParcel.WriteBool(unobscured);
    dataParcel.RewindRead(0);
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetVmaCacheStatus()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VMA_CACHE_STATUS);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    bool flag = GetData<bool>();
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteBool(flag);
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetFreeMultiWindowStatus()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VMA_CACHE_STATUS);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    bool enable = GetData<bool>();
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteBool(enable);
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetLayerTop()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_LAYER_TOP);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    std::string nodeIdStr = GetData<std::string>();
    bool isTop = GetData<bool>();
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteString(nodeIdStr);
    dataParcel.WriteBool(isTop);
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetForceRefresh()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_FORCE_REFRESH);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    std::string nodeIdStr = GetData<std::string>();
    bool isForceRefresh = GetData<bool>();
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteString(nodeIdStr);
    dataParcel.WriteBool(isForceRefresh);
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetScreenActiveRect()
{
    ScreenId id = GetData<uint64_t>();

    Rect activeRect;
    activeRect.x = GetData<int32_t>();
    activeRect.y = GetData<int32_t>();
    activeRect.w = GetData<int32_t>();
    activeRect.h = GetData<int32_t>();
    
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!dataP.WriteUint64(id)) {
        return;
    }
    if (!dataP.WriteInt32(activeRect.x) || !dataP.WriteInt32(activeRect.y) ||
        !dataP.WriteInt32(activeRect.w) || !dataP.WriteInt32(activeRect.h)) {
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_ACTIVE_RECT);
    toServiceConnectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoRepaintEverything()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPAINT_EVERYTHING);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoForceRefreshOneFrameWithNextVSync()
{
    uint32_t code =
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::FORCE_REFRESH_ONE_FRAME_WITH_NEXT_VSYNC);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

class CustomTestSelfDrawingNodeRectChangeCallback : public RSSelfDrawingNodeRectChangeCallbackStub {
public:
    explicit CustomTestSelfDrawingNodeRectChangeCallback(const SelfDrawingNodeRectChangeCallback &callback)
        : cb_(callback)
    {}
    ~CustomTestSelfDrawingNodeRectChangeCallback() override{};

    void OnSelfDrawingNodeRectChange(std::shared_ptr<RSSelfDrawingNodeRectData> rectData) override
    {
        if (cb_ != nullptr) {
            cb_(rectData);
        }
    }

private:
    SelfDrawingNodeRectChangeCallback cb_;
};

void DoRegisterSelfDrawingNodeRectChangeCallback()
{
    uint32_t code = static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::REGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    uint8_t size = GetData<uint8_t>();
    std::vector<int32_t> pids(size);
    for (int i = 0; i < size; i++) {
        pids[i] = GetData<int32_t>();
    }

    int32_t lowLimitWidth = GetData<int32_t>();
    int32_t lowLimitHeight = GetData<int32_t>();
    int32_t highLimitWidth = GetData<int32_t>();
    int32_t highLimitHeight = GetData<int32_t>();

    std::shared_ptr<RSSelfDrawingNodeRectData> rectData = std::make_shared<RSSelfDrawingNodeRectData>();
    auto callback = [&rectData](std::shared_ptr<RSSelfDrawingNodeRectData> data) { rectData = data; };
    sptr<CustomTestSelfDrawingNodeRectChangeCallback> selfDrawingNodeRectChangeCallback_ =
        new CustomTestSelfDrawingNodeRectChangeCallback(callback);

    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteUint32(size);
    for (const auto item : pids) {
        dataParcel.WriteInt32(item);
    }
    dataParcel.WriteInt32(lowLimitWidth);
    dataParcel.WriteInt32(lowLimitHeight);
    dataParcel.WriteInt32(highLimitWidth);
    dataParcel.WriteInt32(highLimitHeight);
    dataParcel.WriteRemoteObject(selfDrawingNodeRectChangeCallback_->AsObject());

    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoNotifyPageName()
{
    uint32_t code =
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_PAGE_NAME);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    std::string packageName = GetData<std::string>();
    std::string pageName = GetData<std::string>();
    bool isEnter = GetData<bool>();
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteString(packageName);
    dataParcel.WriteString(pageName);
    dataParcel.WriteBool(isEnter);
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetColorFollow()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_COLOR_FOLLOW);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    std::string nodeIdStr = GetData<std::string>();
    bool isColorFollow = GetData<bool>();
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteString(nodeIdStr);
    dataParcel.WriteBool(isColorFollow);
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

} // namespace Rosen
} // namespace OHOS

/* Fuzzer envirement */
extern "C" int LLVMFuzzerInitialize(int *argc, char ***argv)
{
    OHOS::Rosen::g_pid = getpid();
    OHOS::Rosen::screenManagerPtr_ = OHOS::Rosen::RSScreenManager::GetInstance();
    OHOS::Rosen::mainThread_ = OHOS::Rosen::RSMainThread::Instance();
    OHOS::Rosen::mainThread_->runner_ = OHOS::AppExecFwk::EventRunner::Create(true);
    OHOS::Rosen::mainThread_->handler_ =
        std::make_shared<OHOS::AppExecFwk::EventHandler>(OHOS::Rosen::mainThread_->runner_);
    OHOS::sptr<OHOS::Rosen::RSIConnectionToken> token_ = new OHOS::IRemoteStub<OHOS::Rosen::RSIConnectionToken>();
    OHOS::Rosen::mainThread_->mainLoop_ = []() {};

    OHOS::Rosen::DVSyncFeatureParam dvsyncParam;
    auto generator = OHOS::Rosen::CreateVSyncGenerator();
    auto appVSyncController = new OHOS::Rosen::VSyncController(generator, 0);
    OHOS::sptr<OHOS::Rosen::VSyncDistributor> appVSyncDistributor_ =
        new OHOS::Rosen::VSyncDistributor(appVSyncController, "app", dvsyncParam);
    OHOS::Rosen::toServiceConnectionStub_ = new OHOS::Rosen::RSClientToServiceConnection(OHOS::Rosen::g_pid, nullptr,
        OHOS::Rosen::mainThread_, OHOS::Rosen::screenManagerPtr_, token_->AsObject(), appVSyncDistributor_);
    return 0;
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (!OHOS::Rosen::Init(data, size)) {
        return -1;
    }
    /* Run your code on data */
    uint8_t tarPos = OHOS::Rosen::GetData<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_GET_UNI_RENDER_ENABLED:
            OHOS::Rosen::DoGetUniRenderEnabled();
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
        case OHOS::Rosen::DO_GET_PIXELMAP_BY_PROCESSID:
            OHOS::Rosen::DoGetPixelMapByProcessId();
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
        case OHOS::Rosen::DO_SET_VMA_CACHE_STATUS:
            OHOS::Rosen::DoSetVmaCacheStatus();
            break;
        case OHOS::Rosen::DO_SET_FREE_MULTI_WINDOW_STATUS:
            OHOS::Rosen::DoSetFreeMultiWindowStatus();
            break;
        case OHOS::Rosen::DO_SET_LAYER_TOP:
            OHOS::Rosen::DoSetLayerTop();
            break;
        case OHOS::Rosen::DO_SET_FORCE_REFRESH:
            OHOS::Rosen::DoSetForceRefresh();
            break;
        case OHOS::Rosen::DO_SET_SCREEN_ACTIVE_RECT:
            OHOS::Rosen::DoSetScreenActiveRect();
            break;
        case OHOS::Rosen::DO_REPAINT_EVERYTHING:
            OHOS::Rosen::DoRepaintEverything();
            break;
        case OHOS::Rosen::DO_FORCE_REFRESH_ONE_FRAME_WITH_NEXT_VSYNC:
            OHOS::Rosen::DoForceRefreshOneFrameWithNextVSync();
            break;
        case OHOS::Rosen::DO_REGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK:
            OHOS::Rosen::DoRegisterSelfDrawingNodeRectChangeCallback();
            break;
        case OHOS::Rosen::DO_NOTIFY_PAGE_NAME:
            OHOS::Rosen::DoNotifyPageName();
            break;
        case OHOS::Rosen::DO_SET_COLOR_FOLLOW:
            OHOS::Rosen::DoSetColorFollow();
            break;
        default:
            return -1;
    }
    return 0;
}