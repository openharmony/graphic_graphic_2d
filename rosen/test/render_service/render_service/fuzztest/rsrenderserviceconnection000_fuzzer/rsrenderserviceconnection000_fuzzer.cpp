/*
* Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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
#include "pipeline/main_thread/rs_render_service_connection.h"
#include "platform/ohos/rs_irender_service.h"
#include "transaction/rs_render_service_connection_stub.h"
#include "transaction/rs_transaction_proxy.h"
#include "message_parcel.h"
#include "securec.h"
#include <iservice_registry.h>
#include <system_ability_definition.h>

namespace OHOS {
namespace Rosen {
DECLARE_INTERFACE_DESCRIPTOR(u"ohos.rosen.RenderServiceConnection");

auto g_pid = getpid();
auto screenManagerPtr_ = impl::RSScreenManager::GetInstance();
auto mainThread_ = RSMainThread::Instance();
sptr<RSIConnectionToken> token_ = new IRemoteStub<RSIConnectionToken>();

DVSyncFeatureParam dvsyncParam;
auto generator = CreateVSyncGenerator();
auto appVSyncController = new VSyncController(generator, 0);
sptr<VSyncDistributor> appVSyncDistributor_ = new VSyncDistributor(appVSyncController, "app", dvsyncParam);
sptr<RSRenderServiceConnectionStub> connectionStub_ = new RSRenderServiceConnection(
    g_pid, nullptr, mainThread_, screenManagerPtr_, token_->AsObject(), appVSyncDistributor_);
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
const uint8_t DO_SET_FORCE_REFRESH = 49;
const uint8_t DO_CLEAR_UIFIRST_CACHE = 50;
const uint8_t TARGET_SIZE = 51;

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

namespace Mock {
void CreateVirtualScreenStubbing(ScreenId screenId)
{
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    int32_t flags = GetData<int32_t>();
    std::string name = GetData<std::string>();
    // Random name of IBufferProducer is not necessary
    sptr<IBufferProducer> bp = IConsumerSurface::Create("DisplayNode")->GetProducer();
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(bp);

    CONN->CreateVirtualScreen(name, width, height, pSurface, screenId, flags);
}
} // namespace Mock

void DoCommitTransaction()
{
    uint32_t code =
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::COMMIT_TRANSACTION);

    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    dataParcel.WriteInt32(0);
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoGetUniRenderEnabled()
{
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_UNI_RENDER_ENABLED);
    connectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoCreateNode()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_NODE);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;

    NodeId id = static_cast<NodeId>(g_pid) << 32;
    dataParcel.WriteUint64(id);
    dataParcel.WriteString("SurfaceName");
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoCreateNodeAndSurface()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_NODE_AND_SURFACE);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;

    FuzzedDataProvider fdp(DATA, g_size);
    uint8_t type = fdp.ConsumeIntegralInRange<uint8_t>(0, 13);
    uint8_t surfaceWindowType = fdp.ConsumeIntegralInRange<uint8_t>(1, 6);
    NodeId id = static_cast<NodeId>(g_pid) << 32;
    bool isTextureExportNode = GetData<bool>();
    bool isSync = GetData<bool>();
    bool unobscured = GetData<bool>();
    dataParcel.WriteUint64(id);
    dataParcel.WriteString("SurfaceName");
    dataParcel.WriteUint8(type);
    dataParcel.WriteBool(isTextureExportNode);
    dataParcel.WriteBool(isSync);
    dataParcel.WriteUint8(surfaceWindowType);
    dataParcel.WriteBool(unobscured);
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetFocusAppInfo()
{
    int32_t pid = GetData<int32_t>();
    int32_t uid = GetData<int32_t>();
    std::string bundleName = GetData<std::string>();
    std::string abilityName = GetData<std::string>();
    uint64_t focusNodeId = GetData<uint64_t>();
    
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!dataP.WriteInt32(pid)) {
        return;
    }
    if (!dataP.WriteInt32(uid)) {
        return;
    }
    if (!dataP.WriteString(bundleName)) {
        return;
    }
    if (!dataP.WriteString(abilityName)) {
        return;
    }
    if (!dataP.WriteUint64(focusNodeId)) {
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_FOCUS_APP_INFO);
    connectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoSetPhysicalScreenResolution()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
    connectionStub_->OnRemoteRequest(code, data, reply, option);
}

void DoSetScreenSecurityMask()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_SECURITY_MASK);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;

    uint64_t screenId = GetData<uint64_t>();
    bool enable = GetData<bool>();
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteUint64(screenId);
    dataParcel.WriteBool(enable);
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
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
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteUint64(screenId);
    dataParcel.WriteInt32(x);
    dataParcel.WriteInt32(y);
    dataParcel.WriteInt32(w);
    dataParcel.WriteInt32(h);
    dataParcel.WriteBool(supportRotation);
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetCastScreenEnableSkipWindow()
{
    uint64_t id = GetData<uint64_t>();
    bool enable = GetData<bool>();
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
    connectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoMarkPowerOffNeedProcessOneFrame()
{
    uint32_t code =
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::MARK_POWER_OFF_NEED_PROCESS_ONE_FRAME);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    dataParcel.WriteInterfaceToken(GetDescriptor());
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoDisablePowerOffRenderControl()
{
    uint32_t code =
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::DISABLE_RENDER_CONTROL_SCREEN);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    uint64_t screenId = GetData<uint64_t>();
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteUint64(screenId);
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
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
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteUint64(screenId);
    dataParcel.WriteUint32(status);
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetScreenBacklight()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_BACK_LIGHT);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    ScreenId id = GetData<uint64_t>();
    uint32_t level = GetData<uint32_t>();
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteUint64(id);
    dataParcel.WriteUint32(level);
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoTakeSurfaceCapture()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::TAKE_SURFACE_CAPTURE);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    sptr<RSISurfaceCaptureCallback> surfaceCaptureCallback = iface_cast<RSISurfaceCaptureCallback>(remoteObject);
    auto nodeId = static_cast<NodeId>(g_pid) << 32;
    float scaleX = GetData<float>();
    float scaleY = GetData<float>();
    bool useDma = GetData<bool>();
    bool useCurWindow = GetData<bool>();
    bool isSync = GetData<bool>();
    bool isClientPixelMap = GetData<bool>();
    float left = GetData<float>();
    float top = GetData<float>();
    float right = GetData<float>();
    float bottom = GetData<float>();
    std::vector<NodeId> surfaceCaptureConfigBlackList{GetData<NodeId>()};
    NodeId endNodeId = GetData<uint64_t>();
    bool useBeginNodeSize = GetData<bool>();
    bool isNeedBlur = GetData<bool>();
    bool blurRadius = GetData<bool>();
    float areaRectLeft = GetData<float>();
    float areaRectTop = GetData<float>();
    float areaRectRight = GetData<float>();
    float areaRectBottom = GetData<float>();
    dataParcel.WriteUint64(nodeId);
    dataParcel.WriteRemoteObject(surfaceCaptureCallback->AsObject());
    // Write SurfaceCaptureConfig
    dataParcel.WriteFloat(scaleX);
    dataParcel.WriteFloat(scaleY);
    dataParcel.WriteBool(useDma);
    dataParcel.WriteBool(useCurWindow);
    dataParcel.WriteUint8(0);
    dataParcel.WriteBool(isSync);
    dataParcel.WriteBool(isClientPixelMap);
    dataParcel.WriteFloat(left);
    dataParcel.WriteFloat(top);
    dataParcel.WriteFloat(right);
    dataParcel.WriteFloat(bottom);
    dataParcel.WriteUInt64Vector(surfaceCaptureConfigBlackList);
    dataParcel.WriteUint64(endNodeId);
    dataParcel.WriteBool(useBeginNodeSize);
    // Write SurfaceCaptureBlurParam
    dataParcel.WriteBool(isNeedBlur);
    dataParcel.WriteFloat(blurRadius);
    // Write SurfaceCaptureAreaRect
    dataParcel.WriteFloat(areaRectLeft);
    dataParcel.WriteFloat(areaRectTop);
    dataParcel.WriteFloat(areaRectRight);
    dataParcel.WriteFloat(areaRectBottom);
    dataParcel.RewindRead(0);
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetWindowFreezeImmediately()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_WINDOW_FREEZE_IMMEDIATELY);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;

    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    sptr<RSISurfaceCaptureCallback> surfaceCaptureCallback = iface_cast<RSISurfaceCaptureCallback>(remoteObject);
    bool isFreeze = GetData<bool>();
    auto nodeId = static_cast<NodeId>(g_pid) << 32;
    float scaleX = GetData<float>();
    float scaleY = GetData<float>();
    bool useDma = GetData<bool>();
    bool useCurWindow = GetData<bool>();
    bool isSync = GetData<bool>();
    bool isClientPixelMap = GetData<bool>();
    float left = GetData<float>();
    float top = GetData<float>();
    float right = GetData<float>();
    float bottom = GetData<float>();
    std::vector<NodeId> surfaceCaptureConfigBlackList{GetData<NodeId>()};
    NodeId endNodeId = GetData<uint64_t>();
    bool useBeginNodeSize = GetData<bool>();

    bool isNeedBlur = GetData<bool>();
    bool blurRadius = GetData<bool>();

    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteUint64(nodeId);
    dataParcel.WriteBool(isFreeze);
    dataParcel.WriteRemoteObject(surfaceCaptureCallback->AsObject());
    // Write SurfaceCaptureConfig
    dataParcel.WriteFloat(scaleX);
    dataParcel.WriteFloat(scaleY);
    dataParcel.WriteBool(useDma);
    dataParcel.WriteBool(useCurWindow);
    dataParcel.WriteUint8(0);
    dataParcel.WriteBool(isSync);
    dataParcel.WriteBool(isClientPixelMap);
    dataParcel.WriteFloat(left);
    dataParcel.WriteFloat(top);
    dataParcel.WriteFloat(right);
    dataParcel.WriteFloat(bottom);
    dataParcel.WriteUInt64Vector(surfaceCaptureConfigBlackList);
    dataParcel.WriteUint64(endNodeId);
    dataParcel.WriteBool(useBeginNodeSize);
    // Write SurfaceCaptureBlurParam
    dataParcel.WriteBool(isNeedBlur);
    dataParcel.WriteFloat(blurRadius);
    dataParcel.RewindRead(0);
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetHwcNodeBounds()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_NODE_AND_SURFACE);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;

    NodeId id = static_cast<NodeId>(g_pid) << 32;
    float positionX = GetData<float>();
    float positionY = GetData<float>();
    float positionZ = GetData<float>();
    float positionW = GetData<float>();

    dataParcel.WriteUint64(id);
    dataParcel.WriteFloat(positionX);
    dataParcel.WriteFloat(positionY);
    dataParcel.WriteFloat(positionZ);
    dataParcel.WriteFloat(positionW);
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoGetPixelMapByProcessId()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_PIXELMAP_BY_PROCESSID);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    uint64_t pid = GetData<uint64_t>();
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteUint64(pid);
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoRegisterApplicationAgent()
{
    uint32_t code =
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_APPLICATION_AGENT);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    sptr<IApplicationAgent> iApplicationAgent_ = iface_cast<IApplicationAgent>(remoteObject);

    dataParcel.WriteRemoteObject(iApplicationAgent_->AsObject());
    dataParcel.RewindRead(0);
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoRegisterBufferAvailableListener()
{
    uint32_t code =
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_BUFFER_AVAILABLE_LISTENER);

    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    sptr<RSIBufferAvailableCallback> rsIBufferAvailableCallback_ = iface_cast<RSIBufferAvailableCallback>(remoteObject);
    auto nodeId = static_cast<NodeId>(g_pid) << 32;
    bool isFromRenderThread = GetData<bool>();
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteUint64(nodeId);
    dataParcel.WriteRemoteObject(rsIBufferAvailableCallback_->AsObject());
    dataParcel.WriteBool(isFromRenderThread);
    dataParcel.RewindRead(0);
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoRegisterBufferClearListener()
{
    NodeId nodeId = GetData<NodeId>();
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    dataP.WriteUint64(nodeId);
    dataP.WriteRemoteObject(remoteObject);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_BUFFER_CLEAR_LISTENER);

    connectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoCreateVSyncConnection()
{
    uint32_t code =
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_VSYNC_CONNECTION);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    sptr<VSyncIConnectionToken> vsyncIConnectionToken_ = iface_cast<VSyncIConnectionToken>(remoteObject);
    uint64_t id = GetData<uint64_t>();
    NodeId windowNodeID = GetData<NodeId>();
    std::string name = GetData<std::string>();
    dataParcel.WriteString(name);
    dataParcel.WriteRemoteObject(vsyncIConnectionToken_->AsObject());
    dataParcel.WriteUint64(id);
    dataParcel.WriteUint64(windowNodeID);
    dataParcel.RewindRead(0);
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoRegisterOcclusionChangeCallback()
{
    uint32_t code =
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_OCCLUSION_CHANGE_CALLBACK);

    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    sptr<RSIOcclusionChangeCallback> rsIOcclusionChangeCallback_ = iface_cast<RSIOcclusionChangeCallback>(remoteObject);

    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteRemoteObject(rsIOcclusionChangeCallback_->AsObject());
    dataParcel.RewindRead(0);
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetAppWindowNum()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_APP_WINDOW_NUM);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;

    uint32_t num = GetData<uint32_t>();
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteUint32(num);
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetSystemAnimatedScenes()
{
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
    connectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoRegisterHgmConfigChangeCallback()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_HGM_CFG_CALLBACK);

    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    sptr<RSIHgmConfigChangeCallback> rsIHgmConfigChangeCallback_ = iface_cast<RSIHgmConfigChangeCallback>(remoteObject);

    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteRemoteObject(rsIHgmConfigChangeCallback_->AsObject());
    dataParcel.RewindRead(0);
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetCacheEnabledForRotation()
{
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return;
    }
    bool isEnabled = GetData<bool>();
    if (!dataP.WriteBool(isEnabled)) {
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_ROTATION_CACHE_ENABLED);
    connectionStub_->OnRemoteRequest(code, dataP, reply, option);
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
    uint8_t tpFeatureConfigType = GetData<uint8_t>();

    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteInt32(feature);
    dataParcel.WriteCString(config.c_str());
    dataParcel.WriteUint8(tpFeatureConfigType);
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
#endif
}

void DoSetCurtainScreenUsingStatus()
{
    bool status = GetData<bool>();
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!dataP.WriteBool(status)) {
        return;
    }

    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_CURTAIN_SCREEN_USING_STATUS);
    connectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoDropFrameByPid()
{
    std::vector<int32_t> pidList;
    uint8_t pidListSize = GetData<uint8_t>();
    for (size_t i = 0; i < pidListSize; i++) {
        pidList.push_back(GetData<int32_t>());
    }

    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!dataP.WriteInt32Vector(pidList)) {
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::DROP_FRAME_BY_PID);
    connectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoGetLayerComposeInfo()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_LAYER_COMPOSE_INFO);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    dataParcel.WriteInterfaceToken(GetDescriptor());
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoGetHwcDisabledReasonInfo()
{
    uint32_t code =
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_HARDWARE_COMPOSE_DISABLED_REASON_INFO);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    dataParcel.WriteInterfaceToken(GetDescriptor());
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoGetHdrOnDuration()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_HDR_ON_DURATION);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    dataParcel.WriteInterfaceToken(GetDescriptor());
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoRegisterUIExtensionCallback()
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    uint32_t code =
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_UIEXTENSION_CALLBACK);
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    sptr<RSIUIExtensionCallback> rsIUIExtensionCallback = iface_cast<RSIUIExtensionCallback>(remoteObject);
    int64_t userId = GetData<int64_t>();
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteInt64(userId);
    dataParcel.WriteRemoteObject(rsIUIExtensionCallback->AsObject());
    dataParcel.RewindRead(0);
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetAncoForceDoDirect()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_ANCO_FORCE_DO_DIRECT);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    bool direct = GetData<bool>();
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteBool(direct);
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetVmaCacheStatus()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VMA_CACHE_STATUS);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    bool flag = GetData<bool>();
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteBool(flag);
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetFreeMultiWindowStatus()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VMA_CACHE_STATUS);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    bool enable = GetData<bool>();
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteBool(enable);
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoRegisterSurfaceBufferCallback()
{
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return;
    }
    auto pid = GetData<int32_t>();
    auto uid = GetData<uint64_t>();
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    dataP.WriteInt32(pid);
    dataP.WriteUint64(uid);
    dataP.WriteRemoteObject(remoteObject);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_SURFACE_BUFFER_CALLBACK);
    connectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoUnregisterSurfaceBufferCallback()
{
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return;
    }
    auto pid = GetData<int32_t>();
    auto uid = GetData<uint64_t>();
    dataP.WriteInt32(pid);
    dataP.WriteUint64(uid);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::UNREGISTER_SURFACE_BUFFER_CALLBACK);
    connectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoSetLayerTop()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_LAYER_TOP);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    std::string nodeIdStr = GetData<std::string>();
    bool isTop = GetData<bool>();
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteString(nodeIdStr);
    dataParcel.WriteBool(isTop);
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetForceRefresh()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_FORCE_REFRESH);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    std::string nodeIdStr = GetData<std::string>();
    bool isForceRefresh = GetData<bool>();
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteString(nodeIdStr);
    dataParcel.WriteBool(isForceRefresh);
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
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
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
    connectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoSetHidePrivacyContent()
{
    pid_t  newPid = getpid();
    NodeId nodeId = static_cast<NodeId>(newPid) << 32;
    bool needHidePrivacyContent = GetData<bool>();
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    dataP.WriteUint64(nodeId);
    dataP.WriteBool(needHidePrivacyContent);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_HIDE_PRIVACY_CONTENT);
    connectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoRepaintEverything()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPAINT_EVERYTHING);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    dataParcel.WriteInterfaceToken(GetDescriptor());
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoForceRefreshOneFrameWithNextVSync()
{
    uint32_t code =
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::FORCE_REFRESH_ONE_FRAME_WITH_NEXT_VSYNC);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    dataParcel.WriteInterfaceToken(GetDescriptor());
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetWindowContainer()
{
    uint32_t code =
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_WINDOW_CONTAINER);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    NodeId nodeId = static_cast<NodeId>(g_pid) << 32;
    bool isEnabled = GetData<bool>();
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteUint64(nodeId);
    dataParcel.WriteBool(isEnabled);
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoRegisterSelfDrawingNodeRectChangeCallback()
{
    uint32_t code = static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::REGISTER_SELF_DRAWING_NODE_RECT_CHANGE_CALLBACK);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    dataParcel.WriteInterfaceToken(GetDescriptor());
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
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
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteString(packageName);
    dataParcel.WriteString(pageName);
    dataParcel.WriteBool(isEnter);
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoTakeSelfSurfaceCapture()
{
    uint32_t code =
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::TAKE_SELF_SURFACE_CAPTURE);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    NodeId nodeId = static_cast<NodeId>(g_pid) << 32;
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    sptr<RSISurfaceCaptureCallback> surfaceCaptureCallback = iface_cast<RSISurfaceCaptureCallback>(remoteObject);

    float scaleX = GetData<float>();
    float scaleY = GetData<float>();
    bool useDma = GetData<bool>();
    bool useCurWindow = GetData<bool>();
    bool isSync = GetData<bool>();
    bool isClientPixelMap = GetData<bool>();
    float left = GetData<float>();
    float top = GetData<float>();
    float right = GetData<float>();
    float bottom = GetData<float>();
    std::vector<NodeId> surfaceCaptureConfigBlackList{GetData<NodeId>()};
    NodeId endNodeId = GetData<uint64_t>();
    bool useBeginNodeSize = GetData<bool>();

    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteUint64(nodeId);
    dataParcel.WriteRemoteObject(surfaceCaptureCallback->AsObject());
    // Write SurfaceCaptureConfig
    dataParcel.WriteFloat(scaleX);
    dataParcel.WriteFloat(scaleY);
    dataParcel.WriteBool(useDma);
    dataParcel.WriteBool(useCurWindow);
    dataParcel.WriteUint8(0);
    dataParcel.WriteBool(isSync);
    dataParcel.WriteBool(isClientPixelMap);
    dataParcel.WriteFloat(left);
    dataParcel.WriteFloat(top);
    dataParcel.WriteFloat(right);
    dataParcel.WriteFloat(bottom);
    dataParcel.WriteUInt64Vector(surfaceCaptureConfigBlackList);
    dataParcel.WriteUint64(endNodeId);
    dataParcel.WriteBool(useBeginNodeSize);
    dataParcel.RewindRead(0);
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetColorFollow()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_COLOR_FOLLOW);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    std::string nodeIdStr = GetData<std::string>();
    bool isColorFollow = GetData<bool>();
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteString(nodeIdStr);
    dataParcel.WriteBool(isColorFollow);
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoClearUifirstCache()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CLEAR_UIFIRST_CACHE);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    NodeId id = GetData<NodeId>();
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteBool(id);
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer envirement */
extern "C" int LLVMFuzzerInitialize(const uint8_t* data, size_t size)
{
    auto newPid = getpid();
    auto mainThread = OHOS::Rosen::RSMainThread::Instance();
    auto screenManagerPtr = OHOS::Rosen::impl::RSScreenManager::GetInstance();
    OHOS::Rosen::CONN = new OHOS::Rosen::RSRenderServiceConnection(
        newPid,
        nullptr,
        mainThread,
        screenManagerPtr,
        nullptr,
        nullptr
    );
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
        case OHOS::Rosen::DO_SET_FORCE_REFRESH:
            OHOS::Rosen::DoSetForceRefresh();
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
        case OHOS::Rosen::DO_CLEAR_UIFIRST_CACHE:
            OHOS::Rosen::DoClearUifirstCache();
            break;
        default:
            return -1;
    }
    return 0;
}