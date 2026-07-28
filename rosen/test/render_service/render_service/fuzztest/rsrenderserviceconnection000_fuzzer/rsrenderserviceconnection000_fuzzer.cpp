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
#include <thread>
#include <fcntl.h>
#include <unordered_map>
#include <fuzzer/FuzzedDataProvider.h>

#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "transaction/rs_client_to_render_connection.h"
#include "render_server/transaction/rs_client_to_service_connection.h"
#include "pipeline/rs_render_node_gc.h"
#include "pipeline/rs_surface_buffer_callback_manager.h"
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
#include "platform/ohos/backend/surface_buffer_utils.h"
#endif
#include "platform/ohos/transaction/zidl/rs_irender_service.h"
#include "transaction/zidl/rs_client_to_render_connection_stub.h"
#include "render_server/transaction/zidl/rs_client_to_service_connection_stub.h"
#include "transaction/rs_transaction_proxy.h"
#include "message_parcel.h"
#include "securec.h"
#include <iservice_registry.h>
#include "ivsync_connection.h"
#include "ipc_callbacks/rs_occlusion_change_callback_stub.h"
#include "ipc_callbacks/hgm_config_change_callback_stub.h"
#include "ipc_callbacks/rs_uiextension_callback_stub.h"
#include "ipc_callbacks/rs_self_drawing_node_rect_change_callback_stub.h"
#include <system_ability_definition.h>
#include "vsync_iconnection_token.h"
#include "iremote_proxy.h"
#include "iremote_stub.h"
#include "feature/hyper_graphic_manager/hgm_render_context.h"

namespace OHOS {
namespace Rosen {

class MockRenderProcessManager : public RSRenderProcessManager {
public:
    explicit MockRenderProcessManager(RSRenderService& renderService) : RSRenderProcessManager(renderService) {}

    sptr<RSIServiceToRenderConnection> GetServiceToRenderConn(ScreenId screenId) const override { return nullptr; }
    std::vector<sptr<RSIServiceToRenderConnection>> GetServiceToRenderConns() const override { return {}; }
    sptr<RSIConnectToRenderProcess> GetConnectToRenderConnection(ScreenId screenId) const override
    {
        return nullptr;
    }
    sptr<IRemoteObject> CreateRenderToServiceConnection(pid_t callingPid) override { return nullptr; }

    sptr<IRemoteObject> OnScreenConnected(ScreenId id, const std::shared_ptr<HdiOutput>& output,
        const sptr<RSScreenProperty>& property) override
    {
        return nullptr;
    }
    void OnScreenDisconnected(ScreenId id) override {}
    void OnScreenPropertyChanged(ScreenId id, ScreenPropertyType type,
        const sptr<ScreenPropertyBase>& property) override {}
    void OnScreenRefresh(ScreenId id) override {}
    void OnVirtualScreenConnected(ScreenId id, ScreenId associatedScreenId,
        const sptr<RSScreenProperty>& property) override {}
    void OnVirtualScreenDisconnected(ScreenId id) override {}
};

int32_t g_pid;
constexpr const int WAIT_HANDLER_TIME = 1; // 1s
constexpr const int WAIT_HANDLER_TIME_COUNT = 5;

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
const uint8_t DO_SET_SCREEN_OFFSET = 32;
const uint8_t TARGET_SIZE = 33;
const uint8_t DO_SET_HDR_FORCE_HWC_ENABLED = 34;
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
void WaitHandlerTask(RSMainThread* mainThread, RSUniRenderThread* uniRenderThread)
{
    if (mainThread == nullptr || mainThread->handler_ == nullptr ||
        uniRenderThread == nullptr || uniRenderThread->handler_ == nullptr) {
        return;
    }
    auto count = 0;
    auto isMainThreadRunning = !mainThread->handler_->IsIdle();
    auto isUniRenderThreadRunning = !uniRenderThread->handler_->IsIdle();
    while (count < WAIT_HANDLER_TIME_COUNT && (isMainThreadRunning || isUniRenderThreadRunning)) {
        std::this_thread::sleep_for(std::chrono::seconds(WAIT_HANDLER_TIME));
        isMainThreadRunning = !mainThread->handler_->IsIdle();
        isUniRenderThreadRunning = !uniRenderThread->handler_->IsIdle();
        count++;
    }
    if (count >= WAIT_HANDLER_TIME_COUNT) {
        mainThread->handler_->RemoveAllEvents();
        uniRenderThread->handler_->RemoveAllEvents();
    }
}
} // namespace

// Global variables - ORDER MATTERS for C++ reverse destruction.
// We declare runners and handlers FIRST so they are destroyed LAST.
// This ensures connections' destructors (which PostTask to runner threads)
// always find EventHandler/EventRunner alive.
std::shared_ptr<AppExecFwk::EventRunner> g_serviceRunner = nullptr;
std::shared_ptr<AppExecFwk::EventRunner> g_mainRunner = nullptr;
std::shared_ptr<AppExecFwk::EventRunner> g_uniRunner = nullptr;

// Extra references to EventHandlers so they outlive connections.
// Without these, the Meyers-singleton RSMainThread may be atexit-destroyed
// before g_renderConnection, leaving handler_ dangling during ~RSClientToRenderConnection().
std::shared_ptr<AppExecFwk::EventHandler> g_serviceHandler = nullptr;
std::shared_ptr<AppExecFwk::EventHandler> g_mainHandler = nullptr;
std::shared_ptr<AppExecFwk::EventHandler> g_uniHandler = nullptr;

sptr<RSRenderService> g_renderService = nullptr;
sptr<RSClientToServiceConnection> g_serviceConnection = nullptr;
sptr<RSClientToRenderConnection> g_renderConnection = nullptr;

void DoGetUniRenderEnabled()
{
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_UNI_RENDER_ENABLED);
    g_serviceConnection->OnRemoteRequest(code, dataP, reply, option);
}

void DoSetPhysicalScreenResolution()
{
    uint32_t code =
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_PHYSICAL_SCREEN_RESOLUTION);

    MessageOption option1;
    MessageParcel dataParcel1;
    MessageParcel replyParcel1;
    ScreenId id = GetData<ScreenId>();
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    if (!dataParcel1.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor()) ||
        !dataParcel1.WriteUint64(id) || !dataParcel1.WriteUint32(width) || !dataParcel1.WriteUint32(height)) {
        return;
    }
    g_serviceConnection->OnRemoteRequest(code, dataParcel1, replyParcel1, option1);

    MessageOption option2;
    MessageParcel dataParcel2;
    MessageParcel replyParcel2;
    if (!dataParcel2.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return;
    }
    g_serviceConnection->OnRemoteRequest(code, dataParcel2, replyParcel2, option2);

    MessageOption option3;
    MessageParcel dataParcel3;
    MessageParcel replyParcel3;
    if (!dataParcel3.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor()) ||
        !dataParcel3.WriteUint64(INVALID_SCREEN_ID)) {
        return;
    }
    g_serviceConnection->OnRemoteRequest(code, dataParcel3, replyParcel3, option3);

    MessageOption option4;
    MessageParcel dataParcel4;
    MessageParcel replyParcel4;
    if (!dataParcel4.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor()) ||
        !dataParcel4.WriteUint64(INVALID_SCREEN_ID) || !dataParcel4.WriteUint32(0)) {
        return;
    }
    g_serviceConnection->OnRemoteRequest(code, dataParcel4, replyParcel4, option4);

    MessageOption option5;
    MessageParcel dataParcel5;
    MessageParcel replyParcel5;
    if (!dataParcel5.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor()) ||
        !dataParcel5.WriteUint64(INVALID_SCREEN_ID) || !dataParcel5.WriteUint32(0) ||
        !dataParcel5.WriteUint32(0)) {
        return;
    }
    g_serviceConnection->OnRemoteRequest(code, dataParcel5, replyParcel5, option5);

    MessageOption option6;
    MessageParcel dataParcel6;
    MessageParcel replyParcel6;
    if (!dataParcel6.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor()) ||
        !dataParcel6.WriteUint64(INVALID_SCREEN_ID) || !dataParcel6.WriteUint32(0) || !dataParcel6.WriteUint32(0)) {
        return;
    }
    replyParcel6.writable_ = false;
    replyParcel6.data_ = nullptr;
    g_serviceConnection->OnRemoteRequest(code, dataParcel6, replyParcel6, option6);
}

void DoSetScreenSecurityMask()
{
    uint32_t code =
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_SECURITY_MASK);

    MessageParcel dataP1;
    MessageParcel reply1;
    MessageOption option1;
    ScreenId id = GetData<ScreenId>();
    bool enable = GetData<bool>();
    if (!dataP1.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor()) ||
        !dataP1.WriteUint64(id) || !dataP1.WriteBool(enable) || g_serviceConnection == nullptr) {
        return;
    }
    g_serviceConnection->OnRemoteRequest(code, dataP1, reply1, option1);

    MessageParcel dataP2;
    MessageParcel reply2;
    MessageOption option2;
    if (!dataP2.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return;
    }
    g_serviceConnection->OnRemoteRequest(code, dataP2, reply2, option2);

    MessageParcel dataP3;
    MessageParcel reply3;
    MessageOption option3;
    if (!dataP3.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor()) ||
        !dataP3.WriteUint64(INVALID_SCREEN_ID)) {
        return;
    }
    g_serviceConnection->OnRemoteRequest(code, dataP3, reply3, option3);

    MessageParcel dataP4;
    MessageParcel reply4;
    MessageOption option4;
    if (!dataP4.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor()) ||
        !dataP4.WriteUint64(INVALID_SCREEN_ID) || !dataP4.WriteBool(true)) {
        return;
    }
    g_serviceConnection->OnRemoteRequest(code, dataP4, reply4, option4);

    MessageParcel dataP5;
    MessageParcel reply5;
    MessageOption option5;
    if (!dataP5.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor()) ||
        !dataP5.WriteUint64(INVALID_SCREEN_ID) || !dataP5.WriteBool(false)) {
        return;
    }
    g_serviceConnection->OnRemoteRequest(code, dataP5, reply5, option5);

    MessageParcel dataP6;
    MessageParcel reply6;
    MessageOption option6;
    if (!dataP6.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor()) ||
        !dataP6.WriteUint64(INVALID_SCREEN_ID) || !dataP6.WriteUInt64Vector({})) {
        return;
    }
    reply6.writable_ = false;
    reply6.data_ = nullptr;
    g_serviceConnection->OnRemoteRequest(code, dataP6, reply6, option6);
}

void DoSetMirrorScreenVisibleRect()
{
    uint32_t code =
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_MIRROR_SCREEN_VISIBLE_RECT);

    MessageParcel dataP1;
    MessageParcel reply1;
    MessageOption option1;
    ScreenId id = GetData<ScreenId>();
    int32_t x = GetData<uint32_t>();
    int32_t y = GetData<uint32_t>();
    int32_t w = GetData<uint32_t>();
    int32_t h = GetData<uint32_t>();
    bool supportRotation = GetData<bool>();
    if (!dataP1.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor()) ||
        !dataP1.WriteUint64(id) || !dataP1.WriteInt32(x) || !dataP1.WriteInt32(y) ||
        !dataP1.WriteInt32(w) || !dataP1.WriteInt32(h) || !dataP1.WriteBool(supportRotation) ||
        g_serviceConnection == nullptr) {
        return;
    }
    g_serviceConnection->OnRemoteRequest(code, dataP1, reply1, option1);

    MessageParcel dataP2;
    MessageParcel reply2;
    MessageOption option2;
    if (!dataP2.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return;
    }
    g_serviceConnection->OnRemoteRequest(code, dataP2, reply2, option2);

    MessageParcel dataP3;
    MessageParcel reply3;
    MessageOption option3;
    if (!dataP3.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor()) ||
        !dataP3.WriteUint64(INVALID_SCREEN_ID)) {
        return;
    }
    g_serviceConnection->OnRemoteRequest(code, dataP3, reply3, option3);

    MessageParcel dataP4;
    MessageParcel reply4;
    MessageOption option4;
    if (!dataP4.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor()) ||
        !dataP4.WriteUint64(INVALID_SCREEN_ID) || !dataP4.WriteInt32(0)) {
        return;
    }
    g_serviceConnection->OnRemoteRequest(code, dataP4, reply4, option4);

    MessageParcel dataP5;
    MessageParcel reply5;
    MessageOption option5;
    if (!dataP5.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor()) ||
        !dataP5.WriteUint64(INVALID_SCREEN_ID) || !dataP5.WriteInt32(0) || !dataP5.WriteInt32(0)) {
        return;
    }
    g_serviceConnection->OnRemoteRequest(code, dataP5, reply5, option5);

    MessageParcel dataP6;
    MessageParcel reply6;
    MessageOption option6;
    if (!dataP6.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor()) ||
        !dataP6.WriteUint64(INVALID_SCREEN_ID) || !dataP6.WriteInt32(0) ||
        !dataP6.WriteInt32(0) || !dataP6.WriteInt32(0)) {
        return;
    }
    g_serviceConnection->OnRemoteRequest(code, dataP6, reply6, option6);

    MessageParcel dataP7;
    MessageParcel reply7;
    MessageOption option7;
    if (!dataP7.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor()) ||
        !dataP7.WriteUint64(INVALID_SCREEN_ID) || !dataP7.WriteInt32(0) || !dataP7.WriteInt32(0) ||
        !dataP7.WriteInt32(0) || !dataP7.WriteInt32(0)) {
        return;
    }
    g_serviceConnection->OnRemoteRequest(code, dataP7, reply7, option7);

    MessageParcel dataP8;
    MessageParcel reply8;
    MessageOption option8;
    if (!dataP8.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor()) ||
        !dataP8.WriteUint64(INVALID_SCREEN_ID) || !dataP8.WriteInt32(0) || !dataP8.WriteInt32(0) ||
        !dataP8.WriteInt32(0) || !dataP8.WriteInt32(0) || !dataP8.WriteBool(true)) {
        return;
    }
    g_serviceConnection->OnRemoteRequest(code, dataP8, reply8, option8);

    MessageParcel dataP9;
    MessageParcel reply9;
    MessageOption option9;
    if (!dataP9.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor()) ||
        !dataP9.WriteUint64(INVALID_SCREEN_ID) || !dataP9.WriteInt32(0) || !dataP9.WriteInt32(0) ||
        !dataP9.WriteInt32(0) || !dataP9.WriteInt32(0) || !dataP9.WriteBool(true)) {
        return;
    }
    reply9.writable_ = false;
    reply9.data_ = nullptr;
    g_serviceConnection->OnRemoteRequest(code, dataP9, reply9, option9);
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

    uint32_t code =
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_CAST_SCREEN_ENABLE_SKIP_WINDOW);
    g_serviceConnection->OnRemoteRequest(code, dataP, reply, option);
}

void DoMarkPowerOffNeedProcessOneFrame()
{
    uint32_t code =
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::MARK_POWER_OFF_NEED_PROCESS_ONE_FRAME);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    g_serviceConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoDisablePowerOffRenderControl()
{
    uint32_t code =
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::DISABLE_RENDER_CONTROL_SCREEN);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    uint64_t screenId = GetData<uint64_t>();
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteUint64(screenId);
    g_serviceConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetScreenPowerStatus()
{
    uint32_t code =
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_POWER_STATUS);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    uint64_t screenId = GetData<uint64_t>();
    uint32_t status = GetData<uint32_t>();
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteUint64(screenId);
    dataParcel.WriteUint32(status);
    g_serviceConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetScreenBacklight()
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_BACK_LIGHT);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    ScreenId id = GetData<uint64_t>();
    uint32_t level = GetData<uint32_t>();
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteUint64(id);
    dataParcel.WriteUint32(level);
    g_serviceConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoGetPixelMapByProcessId()
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_PIXELMAP_BY_PROCESSID);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    uint64_t pid = GetData<uint64_t>();
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteUint64(pid);
    g_serviceConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoCreateVSyncConnection()
{
    if (g_serviceConnection == nullptr) {
        return;
    }
    uint64_t id = GetData<uint64_t>();
    NodeId windowNodeID = GetData<NodeId>();
    std::string name = GetData<std::string>();
    bool fromXcomponent = false;
    sptr<VSyncIConnectionToken> token = new IRemoteStub<VSyncIConnectionToken>();
    sptr<IVSyncConnection> conn = nullptr;
    VSyncConnParam param = {id, windowNodeID, fromXcomponent};
    g_serviceConnection->CreateVSyncConnection(conn, name, token, param);
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
        static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::REGISTER_OCCLUSION_CHANGE_CALLBACK);

    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    std::shared_ptr<RSOcclusionData> occlusionData = std::make_shared<RSOcclusionData>();
    auto callback = [&occlusionData](std::shared_ptr<RSOcclusionData> data) { occlusionData = data; };
    sptr<CustomTestOcclusionChangeCallback> rsIOcclusionChangeCallback_ =
        new CustomTestOcclusionChangeCallback(callback);

    dataParcel.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    dataParcel.WriteRemoteObject(rsIOcclusionChangeCallback_->AsObject());
    dataParcel.RewindRead(0);
    g_renderConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetSystemAnimatedScenes()
{
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t systemAnimatedScenes = GetData<uint32_t>();
    bool isRegularAnimation = GetData<bool>();
    if (!dataP.WriteUint32(systemAnimatedScenes) || !dataP.WriteBool(isRegularAnimation)) {
        return;
    }
    uint32_t code = static_cast<uint32_t>(
        RSIClientToRenderConnectionInterfaceCode::SET_SYSTEM_ANIMATED_SCENES);
    g_renderConnection->OnRemoteRequest(code, dataP, reply, option);
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
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::REGISTER_HGM_CFG_CALLBACK);

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
    g_serviceConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
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
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_ROTATION_CACHE_ENABLED);
    g_serviceConnection->OnRemoteRequest(code, dataP, reply, option);
}

void DoSetTpFeatureConfig()
{
#ifdef TP_FEATURE_ENABLE
    uint32_t code =
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_TP_FEATURE_CONFIG);

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
    g_serviceConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
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

    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_CURTAIN_SCREEN_USING_STATUS);
    g_serviceConnection->OnRemoteRequest(code, dataP, reply, option);
}

void DoGetLayerComposeInfo()
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_LAYER_COMPOSE_INFO);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    g_serviceConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoGetHwcDisabledReasonInfo()
{
    uint32_t code =
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_HARDWARE_COMPOSE_DISABLED_REASON_INFO);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    g_serviceConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoGetHdrOnDuration()
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_HDR_ON_DURATION);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    g_serviceConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
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
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::REGISTER_UIEXTENSION_CALLBACK);

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
    g_serviceConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetVmaCacheStatus()
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_VMA_CACHE_STATUS);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    bool flag = GetData<bool>();
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteBool(flag);
    g_serviceConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetFreeMultiWindowStatus()
{
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SET_FREE_MULTI_WINDOW_STATUS);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    bool enable = GetData<bool>();
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteBool(enable);
    g_renderConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetLayerTop()
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_LAYER_TOP);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    std::string nodeIdStr = GetData<std::string>();
    bool isTop = GetData<bool>();
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteString(nodeIdStr);
    dataParcel.WriteBool(isTop);
    g_serviceConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetHdrForceHwcEnabled()
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_HDR_FORCE_HWC_ENABLED);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    std::string nodeIdStr = GetData<std::string>();
    bool isHdrForceHwcEnabled = GetData<bool>();
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteString(nodeIdStr);
    dataParcel.WriteBool(isHdrForceHwcEnabled);
    g_serviceConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetForceRefresh()
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_FORCE_REFRESH);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    std::string nodeIdStr = GetData<std::string>();
    bool isForceRefresh = GetData<bool>();
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteString(nodeIdStr);
    dataParcel.WriteBool(isForceRefresh);
    g_serviceConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
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
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_ACTIVE_RECT);
    g_serviceConnection->OnRemoteRequest(code, dataP, reply, option);
}

void DoSetScreenOffset()
{
    uint32_t code =
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_OFFSET);

    MessageParcel dataP1;
    MessageParcel reply1;
    MessageOption option1;
    ScreenId id = GetData<uint64_t>();
    int32_t offsetX = GetData<int32_t>();
    int32_t offsetY = GetData<int32_t>();
    if (!dataP1.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor()) ||
        !dataP1.WriteUint64(id) || !dataP1.WriteInt32(offsetX) || !dataP1.WriteInt32(offsetY)) {
        return;
    }
    g_serviceConnection->OnRemoteRequest(code, dataP1, reply1, option1);

    MessageParcel dataP2;
    MessageParcel reply2;
    MessageOption option2;
    if (!dataP2.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return;
    }
    g_serviceConnection->OnRemoteRequest(code, dataP2, reply2, option2);

    MessageParcel dataP3;
    MessageParcel reply3;
    MessageOption option3;
    if (!dataP3.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor()) ||
        !dataP3.WriteUint64(INVALID_SCREEN_ID)) {
        return;
    }
    g_serviceConnection->OnRemoteRequest(code, dataP3, reply3, option3);

    MessageParcel dataP4;
    MessageParcel reply4;
    MessageOption option4;
    if (!dataP4.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor()) ||
        !dataP4.WriteUint64(INVALID_SCREEN_ID) || !dataP4.WriteInt32(0)) {
        return;
    }
    g_serviceConnection->OnRemoteRequest(code, dataP4, reply4, option4);

    MessageParcel dataP5;
    MessageParcel reply5;
    MessageOption option5;
    if (!dataP5.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor()) ||
        !dataP5.WriteUint64(INVALID_SCREEN_ID) || !dataP5.WriteInt32(0) || !dataP5.WriteInt32(0)) {
        return;
    }
    g_serviceConnection->OnRemoteRequest(code, dataP5, reply5, option5);
}

void DoRepaintEverything()
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::REPAINT_EVERYTHING);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    g_serviceConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoForceRefreshOneFrameWithNextVSync()
{
    uint32_t code =
        static_cast<uint32_t>(DO_FORCE_REFRESH_ONE_FRAME_WITH_NEXT_VSYNC);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    g_serviceConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
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
        RSIClientToServiceConnectionInterfaceCode::REGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK);
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

    g_serviceConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoNotifyPageName()
{
    uint32_t code =
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::NOTIFY_PAGE_NAME);
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
    g_serviceConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetColorFollow()
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_COLOR_FOLLOW);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    std::string nodeIdStr = GetData<std::string>();
    bool isColorFollow = GetData<bool>();
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteString(nodeIdStr);
    dataParcel.WriteBool(isColorFollow);
    g_serviceConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
}
} // namespace Rosen
} // namespace OHOS


/* Fallback cleanup registered via atexit, in case LLVMFuzzerFinalize is not invoked. */
static void FuzzerAtExitCleanup()
{
    using namespace OHOS::Rosen;
    using namespace OHOS::AppExecFwk;
    // Release connections BEFORE RSMainThread Meyers-singleton is destroyed
    // by atexit, otherwise ~RSClientToRenderConnection() -> PostTask() UAF.
    g_renderConnection = nullptr;
    g_serviceConnection = nullptr;

    // Stop runners and wait for worker threads to actually exit.
    // Without this, ~RSMainThread() may free RSContext while T9 is still
    // running CleanResources() inside the event loop.
    if (g_serviceRunner != nullptr) {
        g_serviceRunner->Stop();
    }
    if (g_mainRunner != nullptr) {
        g_mainRunner->Stop();
    }
    if (g_uniRunner != nullptr) {
        g_uniRunner->Stop();
    }
    auto waitRunnerStopped = [](const std::shared_ptr<EventRunner>& runner) {
        if (runner == nullptr) {
            return;
        }
        int count = 0;
        while (runner->IsRunning() && count < 500) { // max 5s
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            count++;
        }
    };
    waitRunnerStopped(g_serviceRunner);
    waitRunnerStopped(g_mainRunner);
    waitRunnerStopped(g_uniRunner);
}

/* Fuzzer environment */
extern "C" int LLVMFuzzerInitialize(int* argc, char*** argv)
{
    (void)argc;
    (void)argv;

    OHOS::sptr<OHOS::Rosen::RSIConnectionToken> token =
        new OHOS::IRemoteStub<OHOS::Rosen::RSIConnectionToken>();

    // Step 1: Create RSRenderService and basic handlers
    OHOS::Rosen::g_renderService = new OHOS::Rosen::RSRenderService();
    OHOS::sptr<OHOS::Rosen::RSRenderService>& renderService = OHOS::Rosen::g_renderService;
    OHOS::Rosen::g_serviceRunner = OHOS::AppExecFwk::EventRunner::Create(true);
    OHOS::Rosen::g_serviceRunner->Run();
    OHOS::Rosen::g_serviceHandler = std::make_shared<OHOS::AppExecFwk::EventHandler>(OHOS::Rosen::g_serviceRunner);
    renderService->handler_ = OHOS::Rosen::g_serviceHandler;

    // Step 2: Initialize vsync and screen managers
    renderService->vsyncManager_ = OHOS::sptr<OHOS::Rosen::RSVsyncManager>::MakeSptr();
    renderService->screenManager_ = OHOS::sptr<OHOS::Rosen::RSScreenManager>::MakeSptr();
    renderService->vsyncManager_->init(renderService->screenManager_);

    // Step 3: Initialize render pipeline with main thread and uni render thread
    // Note: We intentionally skip RSRenderProcessManager::Create here because
    // in single-process mode (the default), RSSingleRenderProcessManager's
    // constructor would create VSyncReceiver + overwrite renderPipeline_ +
    // call RSUniRenderThread::Start(), spawning an uncontrollable runner thread.
    // For the interfaces we fuzz (GetUniRenderEnabled, SetGlobalDarkColorMode),
    // renderProcessManager is not needed.
    renderService->renderPipeline_ = std::make_shared<OHOS::Rosen::RSRenderPipeline>();
    OHOS::Rosen::RSMainThread* mainThread = OHOS::Rosen::RSMainThread::Instance();
    OHOS::Rosen::g_mainRunner = OHOS::AppExecFwk::EventRunner::Create(true);
    OHOS::Rosen::g_mainRunner->Run();
    OHOS::Rosen::g_mainHandler = std::make_shared<OHOS::AppExecFwk::EventHandler>(OHOS::Rosen::g_mainRunner);
    mainThread->handler_ = OHOS::Rosen::g_mainHandler;
    renderService->renderPipeline_->mainThread_ = mainThread;

    OHOS::Rosen::RSUniRenderThread* uniRenderThread = &(OHOS::Rosen::RSUniRenderThread::Instance());
    OHOS::Rosen::g_uniRunner = OHOS::AppExecFwk::EventRunner::Create(true);
    OHOS::Rosen::g_uniRunner->Run();
    OHOS::Rosen::g_uniHandler = std::make_shared<OHOS::AppExecFwk::EventHandler>(OHOS::Rosen::g_uniRunner);
    uniRenderThread->handler_ = OHOS::Rosen::g_uniHandler;
    uniRenderThread->runner_ = OHOS::Rosen::g_uniRunner;
    renderService->renderPipeline_->uniRenderThread_ = uniRenderThread;

    // Step 5: Initialize composer manager
    renderService->rsRenderComposerManager_ =
        std::make_shared<OHOS::Rosen::RSRenderComposerManager>(renderService->handler_);

    // Step 4: Create agents for service connection
    auto renderServiceAgent = OHOS::sptr<OHOS::Rosen::RSRenderServiceAgent>::MakeSptr(*renderService);
    auto screenManagerAgent = OHOS::sptr<OHOS::Rosen::RSScreenManagerAgent>::MakeSptr(renderService->screenManager_);
    auto vsyncManagerAgent = renderService->vsyncManager_->GetVsyncManagerAgent();

    // Step 5: Create RSClientToServiceConnection
    // Use a mock RenderProcessManager to avoid nullptr crash in fuzzer tests,
    // while skipping the real RSSingleRenderProcessManager which spawns uncontrollable threads.
    auto mockRenderProcessManager = OHOS::sptr<OHOS::Rosen::MockRenderProcessManager>::MakeSptr(*renderService);
    auto renderProcessManagerAgent =
        OHOS::sptr<OHOS::Rosen::RSRenderProcessManagerAgent>::MakeSptr(mockRenderProcessManager);
    OHOS::Rosen::g_serviceConnection = new OHOS::Rosen::RSClientToServiceConnection(
        getpid(), renderServiceAgent, renderProcessManagerAgent,
        screenManagerAgent, token->AsObject(), vsyncManagerAgent);

    // Step 8: Create RSClientToRenderConnection
    auto renderPipelineAgent =
        OHOS::sptr<OHOS::Rosen::RSRenderPipelineAgent>::MakeSptr(renderService->renderPipeline_);
    OHOS::Rosen::g_renderConnection = new OHOS::Rosen::RSClientToRenderConnection(
        getpid(), renderPipelineAgent, token->AsObject());

    // Register atexit cleanup AFTER all Meyers-singletons (RSMainThread/RSUniRenderThread)
    // have been initialized. Meyers singletons register their destructors via atexit
    // on first Instance() call. atexit is LIFO, so FuzzerAtExitCleanup must be
    // registered AFTER them to ensure it runs BEFORE ~RSMainThread().
    std::atexit(FuzzerAtExitCleanup);

    return 0;
}

/* Fuzzer cleanup: stop event runners before process exit to avoid UAF.
 * Pattern borrowed from RSClientToRenderConnectionStubTest::TearDownTestCase
 * and RSClientToServiceConnectionStubTest::TearDownTestCase.
 */
extern "C" int LLVMFuzzerFinalize(void)
{
    using namespace OHOS::Rosen;
    using namespace OHOS::AppExecFwk;

    // 1. Release connections FIRST while runners/handlers are still alive.
    //    ~RSClientToRenderConnection() calls CleanAll() which uses
    //    ScheduleMainThreadTask().wait() -- runners must be running.
    g_renderConnection = nullptr;
    g_serviceConnection = nullptr;

    // 2. Wait for pending handler tasks to finish (borrowed from WaitHandlerTask).
    RSMainThread* mainThread = RSMainThread::Instance();
    RSUniRenderThread* uniRenderThread = &RSUniRenderThread::Instance();
    WaitHandlerTask(mainThread, uniRenderThread);

    // 3. Stop runners.
    if (g_serviceRunner != nullptr) {
        g_serviceRunner->Stop();
    }
    if (g_mainRunner != nullptr) {
        g_mainRunner->Stop();
    }
    if (g_uniRunner != nullptr) {
        g_uniRunner->Stop();
    }

    // 4. Wait for runner worker threads to ACTUALLY exit.
    //    Stop() only sets queue_->Finish(); T9 may still be processing
    //    the current event (e.g. CleanResources). IsRunning() becomes
    //    false only after Run() returns in the worker thread.
    auto waitRunnerStopped = [](const std::shared_ptr<EventRunner>& runner) {
        if (runner == nullptr) {
            return;
        }
        int count = 0;
        while (runner->IsRunning() && count < 500) { // max 5s
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            count++;
        }
    };
    waitRunnerStopped(g_serviceRunner);
    waitRunnerStopped(g_mainRunner);
    waitRunnerStopped(g_uniRunner);

    // 5. Explicitly reset handler_/runner_ pointers on singletons
    //    (same pattern as TearDownTestCase in the stub tests).
    if (mainThread != nullptr) {
        mainThread->handler_ = nullptr;
        mainThread->receiver_ = nullptr;
    }
    if (uniRenderThread != nullptr) {
        uniRenderThread->handler_ = nullptr;
        uniRenderThread->runner_ = nullptr;
    }

    // 6. Release renderService (and its renderPipeline_) AFTER connections/agents
    //    are dead and runners/handlers are stopped.
    g_renderService = nullptr;

    // 7. Release runners and extra handler references.
    g_serviceRunner.reset();
    g_mainRunner.reset();
    g_uniRunner.reset();
    g_serviceHandler.reset();
    g_mainHandler.reset();
    g_uniHandler.reset();

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
        case OHOS::Rosen::DO_SET_HDR_FORCE_HWC_ENABLED:
            OHOS::Rosen::DoSetHdrForceHwcEnabled();
            break;
        case OHOS::Rosen::DO_SET_FORCE_REFRESH:
            OHOS::Rosen::DoSetForceRefresh();
            break;
        case OHOS::Rosen::DO_SET_SCREEN_ACTIVE_RECT:
            OHOS::Rosen::DoSetScreenActiveRect();
            break;
        case OHOS::Rosen::DO_SET_SCREEN_OFFSET:
            OHOS::Rosen::DoSetScreenOffset();
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