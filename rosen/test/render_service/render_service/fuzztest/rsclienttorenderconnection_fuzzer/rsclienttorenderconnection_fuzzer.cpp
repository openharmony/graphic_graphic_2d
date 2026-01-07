/*
* Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "rsclienttorenderconnection_fuzzer.h"

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
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
#include "platform/ohos/backend/surface_buffer_utils.h"
#endif
#include "platform/ohos/rs_irender_service.h"
#include "transaction/zidl/rs_client_to_render_connection_stub.h"
#include "render_server/transaction/zidl/rs_client_to_service_connection_stub.h"
#include "transaction/rs_transaction_proxy.h"
#include "message_parcel.h"
#include "securec.h"
#include <iservice_registry.h>
#include "ipc_callbacks/surface_capture_callback_stub.h"
#include "ipc_callbacks/rs_surface_buffer_callback_stub.h"
#include "ipc_callbacks/rs_surface_buffer_callback.h"
#include "ipc_callbacks/rs_transaction_data_callback_stub.h"
#include <system_ability_definition.h>

namespace OHOS {
namespace Rosen {

int32_t g_pid;
sptr<OHOS::Rosen::RSScreenManager> screenManagerPtr_ = nullptr;
[[maybe_unused]] auto& rsRenderNodeGC = RSRenderNodeGC::Instance();
[[maybe_unused]] auto& rsSurfaceBufferCallbackManager = RSSurfaceBufferCallbackManager::Instance();
RSMainThread* mainThread_ = RSMainThread::Instance();
sptr<RSClientToRenderConnectionStub> toRenderConnectionStub_ = nullptr;

namespace {
const uint8_t DO_SET_FOCUS_APP_INFO = 0;
const uint8_t DO_TAKE_SURFACE_CAPTURE = 1;
const uint8_t DO_SET_WINDOW_FREEZE_IMMEDIATELY = 2;
const uint8_t DO_SET_POINTER_POSITION = 3;
const uint8_t DO_DROP_FRAME_BY_PID = 4;
const uint8_t DO_SET_ANCO_FORCE_DO_DIRECT = 5;
const uint8_t DO_REGISTER_SURFACE_BUFFER_CALLBACK = 6;
const uint8_t DO_UNREGISTER_SURFACE_BUFFER_CALLBACK = 7;
const uint8_t DO_SET_WINDOW_CONTAINER = 8;
const uint8_t DO_TAKE_SELF_SURFACE_CAPTURE = 9;
const uint8_t DO_CLEAR_UIFIRST_CACHE = 10;
const uint8_t DO_REGISTER_TRANSACTION_DATA_CALLBACK = 11;
const uint8_t DO_TAKE_SURFACE_CAPTURE_WITH_ALLWINDOWS = 12;
const uint8_t DO_FREEZE_SCREEN = 13;
const uint8_t DO_REGISTER_CANVAS_CALLBACK = 14;
const uint8_t DO_SUBMIT_CANVAS_PRE_ALLOCATED_BUFFER = 15;
const uint8_t TARGET_SIZE = 16;
const uint8_t* DATA = nullptr;
size_t g_size = 0;
size_t g_pos;
constexpr uint32_t FUZZ_MAX_DROP_FRAME_LIST = 10000;

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
    if (!dataP.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
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
    toRenderConnectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

class RSSurfaceCaptureCallbackStubMock : public RSSurfaceCaptureCallbackStub {
public:
    RSSurfaceCaptureCallbackStubMock() = default;
    virtual ~RSSurfaceCaptureCallbackStubMock() = default;
    void OnSurfaceCapture(NodeId id, const RSSurfaceCaptureConfig& captureConfig,
        Media::PixelMap* pixelmap, CaptureError captureErrorCode = CaptureError::CAPTURE_OK,
        Media::PixelMap* pixelmapHDR = nullptr) override {};
};

void DoTakeSurfaceCapture()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::TAKE_SURFACE_CAPTURE);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;

    sptr<RSISurfaceCaptureCallback> surfaceCaptureCallback = new RSSurfaceCaptureCallbackStubMock();
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
    float specifiedAreaRectLeft = GetData<float>();
    float specifiedAreaRectTop = GetData<float>();
    float specifiedAreaRectRight = GetData<float>();
    float specifiedAreaRectBottom = GetData<float>();
    uint32_t backGroundColor = GetData<uint32_t>();
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
    dataParcel.WriteFloat(specifiedAreaRectLeft);
    dataParcel.WriteFloat(specifiedAreaRectTop);
    dataParcel.WriteFloat(specifiedAreaRectRight);
    dataParcel.WriteFloat(specifiedAreaRectBottom);
    dataParcel.WriteUint32(backGroundColor);
    // Write SurfaceCaptureBlurParam
    dataParcel.WriteBool(isNeedBlur);
    dataParcel.WriteFloat(blurRadius);
    // Write SurfaceCaptureAreaRect
    dataParcel.WriteFloat(areaRectLeft);
    dataParcel.WriteFloat(areaRectTop);
    dataParcel.WriteFloat(areaRectRight);
    dataParcel.WriteFloat(areaRectBottom);
    dataParcel.RewindRead(0);
    toRenderConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetWindowFreezeImmediately()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_WINDOW_FREEZE_IMMEDIATELY);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;

    sptr<RSISurfaceCaptureCallback> surfaceCaptureCallback = new RSSurfaceCaptureCallbackStubMock();
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

    dataParcel.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
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
    toRenderConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetHwcNodeBounds()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_POINTER_POSITION);
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
    toRenderConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoDropFrameByPid()
{
    std::vector<int32_t> pidList;
    // MAX_DROP_FRAME_PID_LIST_SIZE = 1024
    uint8_t pidListSize = GetData<uint8_t>();
    int32_t status = GetData<int32_t>() % 3;
    if (status == 0) {
        pidListSize = (pidListSize % FUZZ_MAX_DROP_FRAME_LIST) + MAX_DROP_FRAME_PID_LIST_SIZE;
    } else if (status == 1) {
        pidListSize = pidListSize % MAX_DROP_FRAME_PID_LIST_SIZE;
    }
    for (size_t i = 0; i < pidListSize; i++) {
        pidList.push_back(GetData<int32_t>());
    }

    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!dataP.WriteInt32Vector(pidList)) {
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::DROP_FRAME_BY_PID);
    toRenderConnectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoSetAncoForceDoDirect()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_ANCO_FORCE_DO_DIRECT);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    bool direct = GetData<bool>();
    dataParcel.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    dataParcel.WriteBool(direct);
    toRenderConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

class CustomTestSurfaceBufferCallback : public RSSurfaceBufferCallbackStub {
public:
    CustomTestSurfaceBufferCallback() = default;
    ~CustomTestSurfaceBufferCallback() = default;

    void OnFinish(const FinishCallbackRet &ret) override
    {}
    void OnAfterAcquireBuffer(const AfterAcquireBufferRet &ret) override
    {}
};

void DoRegisterSurfaceBufferCallback()
{
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        return;
    }
    auto pid = GetData<int32_t>();
    auto uid = GetData<uint64_t>();
    sptr<RSISurfaceBufferCallback> surfaceCaptureCallback = new CustomTestSurfaceBufferCallback();
    dataP.WriteInt32(pid);
    dataP.WriteUint64(uid);
    dataP.WriteRemoteObject(surfaceCaptureCallback->AsObject());
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_SURFACE_BUFFER_CALLBACK);
    toRenderConnectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoUnregisterSurfaceBufferCallback()
{
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        return;
    }
    auto pid = GetData<int32_t>();
    auto uid = GetData<uint64_t>();
    dataP.WriteInt32(pid);
    dataP.WriteUint64(uid);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::UNREGISTER_SURFACE_BUFFER_CALLBACK);
    toRenderConnectionStub_->OnRemoteRequest(code, dataP, reply, option);
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
    dataParcel.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    dataParcel.WriteUint64(nodeId);
    dataParcel.WriteBool(isEnabled);
    toRenderConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoTakeSelfSurfaceCapture()
{
    uint32_t code =
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::TAKE_SELF_SURFACE_CAPTURE);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    NodeId nodeId = static_cast<NodeId>(g_pid) << 32;
    sptr<RSISurfaceCaptureCallback> surfaceCaptureCallback = new RSSurfaceCaptureCallbackStubMock();
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

    dataParcel.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
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
    toRenderConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoClearUifirstCache()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CLEAR_UIFIRST_CACHE);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    NodeId id = GetData<NodeId>();
    dataParcel.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    dataParcel.WriteUint64(id);
    toRenderConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

class CustomTestTransactionDataCallback : public RSTransactionDataCallbackStub {
public:
    CustomTestTransactionDataCallback() = default;
    ~CustomTestTransactionDataCallback() = default;

    void OnAfterProcess(uint64_t multiToken, uint64_t timeStamp) override
    {}
};

void DoRegisterTransactionDataCallback()
{
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return;
    }
    uint64_t token = GetData<uint64_t>();
    auto timeStamp = GetData<uint64_t>();

    sptr<RSITransactionDataCallback> transactionDataCallback = new CustomTestTransactionDataCallback();
    dataP.WriteUint64(token);
    dataP.WriteUint64(timeStamp);
    dataP.WriteRemoteObject(transactionDataCallback->AsObject());
    uint32_t code = static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::REGISTER_TRANSACTION_DATA_CALLBACK);
    if (toRenderConnectionStub_ == nullptr) {
        return;
    }
    toRenderConnectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

bool DoTakeSurfaceCaptureWithAllWindows()
{
    auto newPid = getpid();
    auto screenManagerPtr = RSScreenManager::GetInstance();
    auto mainThread = RSMainThread::Instance();
    sptr<RSIConnectionToken> token_ = new IRemoteStub<RSIConnectionToken>();
    sptr<RSClientToRenderConnectionStub> toRenderConnectionStub_ =
        new RSClientToRenderConnection(newPid, nullptr, mainThread, screenManagerPtr, token_->AsObject(), nullptr);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;

    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    sptr<RSISurfaceCaptureCallback> surfaceCaptureCallback = iface_cast<RSISurfaceCaptureCallback>(remoteObject);

    NodeId nodeId = GetData<NodeId>();
    bool checkDrmAndSurfaceLock = GetData<bool>();
    float scaleX = GetData<float>();
    float scaleY = GetData<float>();
    bool useDma = GetData<bool>();
    bool useCurWindow = GetData<bool>();
    uint8_t captureType = GetData<uint8_t>();
    bool isSync = GetData<bool>();
    bool isHdrCapture = GetData<bool>();
    bool needF16WindowCaptureForScRGB = GetData<bool>();
    float left = GetData<float>();
    float top = GetData<float>();
    float right = GetData<float>();
    float bottom = GetData<float>();
    std::vector<NodeId> blackList{GetData<NodeId>()};
    NodeId endNodeId = GetData<NodeId>();
    bool useBeginNodeSize = GetData<bool>();
    float areaRectLeft = GetData<float>();
    float areaRectTop = GetData<float>();
    float areaRectRight = GetData<float>();
    float areaRectBottom = GetData<float>();
    uint32_t backGroundColor = GetData<uint32_t>();
    dataParcel.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    dataParcel.WriteUint64(nodeId);
    dataParcel.WriteBool(checkDrmAndSurfaceLock);
    dataParcel.WriteRemoteObject(surfaceCaptureCallback->AsObject());
    dataParcel.WriteFloat(scaleX);
    dataParcel.WriteFloat(scaleY);
    dataParcel.WriteBool(useDma);
    dataParcel.WriteBool(useCurWindow);
    dataParcel.WriteUint8(captureType);
    dataParcel.WriteBool(isSync);
    dataParcel.WriteBool(isHdrCapture);
    dataParcel.WriteBool(needF16WindowCaptureForScRGB);
    dataParcel.WriteFloat(left);
    dataParcel.WriteFloat(top);
    dataParcel.WriteFloat(right);
    dataParcel.WriteFloat(bottom);
    dataParcel.WriteUInt64Vector(blackList);
    dataParcel.WriteUint64(endNodeId);
    dataParcel.WriteBool(useBeginNodeSize);
    dataParcel.WriteFloat(areaRectLeft);
    dataParcel.WriteFloat(areaRectRight);
    dataParcel.WriteFloat(areaRectTop);
    dataParcel.WriteFloat(areaRectBottom);
    dataParcel.WriteUint32(backGroundColor);
    dataParcel.RewindRead(0);

    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::TAKE_SURFACE_CAPTURE_WITH_ALL_WINDOWS);
    toRenderConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
    return true;
}

bool DoFreezeScreen()
{
    auto newPid = getpid();
    auto screenManagerPtr = RSScreenManager::GetInstance();
    auto mainThread = RSMainThread::Instance();
    sptr<RSIConnectionToken> token_ = new IRemoteStub<RSIConnectionToken>();
    sptr<RSClientToRenderConnectionStub> toRenderConnectionStub_ =
        new RSClientToRenderConnection(newPid, nullptr, mainThread, screenManagerPtr, token_->AsObject(), nullptr);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;

    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    sptr<RSISurfaceCaptureCallback> surfaceCaptureCallback = iface_cast<RSISurfaceCaptureCallback>(remoteObject);

    NodeId nodeId = GetData<NodeId>();
    bool isFreeze = GetData<bool>();
    dataParcel.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    dataParcel.WriteUint64(nodeId);
    dataParcel.WriteBool(isFreeze);
    dataParcel.RewindRead(0);

    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::FREEZE_SCREEN);
    toRenderConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
    return true;
}

#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
void DoRegisterCanvasCallback()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_CANVAS_CALLBACK);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    sptr<RSICanvasSurfaceBufferCallback> callback = iface_cast<RSICanvasSurfaceBufferCallback>(remoteObject);
    bool hasCallback = GetData<bool>();
    dataParcel.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    dataParcel.WriteBool(hasCallback);
    dataParcel.WriteRemoteObject(callback->AsObject());
    dataParcel.RewindRead(0);
    toRenderConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSubmitCanvasPreAllocatedBuffer()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SUBMIT_CANVAS_PRE_ALLOCATED_BUFFER);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    NodeId nodeId = GetData<uint64_t>();
    uint32_t resetSurfaceIndex = GetData<uint32_t>();
    uint32_t sequence = GetData<uint32_t>();
    bool hasBuffer = GetData<bool>();
    int width = GetData<int>();
    int height = GetData<int>();
    pid_t pid = GetData<pid_t>();
    auto buffer = SurfaceBufferUtils::CreateCanvasSurfaceBuffer(pid, width, height);
    dataParcel.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    dataParcel.WriteUint64(nodeId);
    dataParcel.WriteUint32(resetSurfaceIndex);
    dataParcel.WriteUint32(sequence);
    dataParcel.WriteBool(hasBuffer);
    if (buffer != nullptr) {
        buffer->WriteToMessageParcel(dataParcel);
    }
    toRenderConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}
#endif
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
    OHOS::Rosen::toRenderConnectionStub_ = new OHOS::Rosen::RSClientToRenderConnection(OHOS::Rosen::g_pid, nullptr,
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
        case OHOS::Rosen::DO_SET_FOCUS_APP_INFO:
            OHOS::Rosen::DoSetFocusAppInfo();
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
        case OHOS::Rosen::DO_DROP_FRAME_BY_PID:
            OHOS::Rosen::DoDropFrameByPid();
            break;
        case OHOS::Rosen::DO_SET_ANCO_FORCE_DO_DIRECT:
            OHOS::Rosen::DoSetAncoForceDoDirect();
            break;
        case OHOS::Rosen::DO_REGISTER_SURFACE_BUFFER_CALLBACK:
            OHOS::Rosen::DoRegisterSurfaceBufferCallback();
            break;
        case OHOS::Rosen::DO_UNREGISTER_SURFACE_BUFFER_CALLBACK:
            OHOS::Rosen::DoUnregisterSurfaceBufferCallback();
            break;
        case OHOS::Rosen::DO_SET_WINDOW_CONTAINER:
            OHOS::Rosen::DoSetWindowContainer();
            break;
        case OHOS::Rosen::DO_TAKE_SELF_SURFACE_CAPTURE:
            OHOS::Rosen::DoTakeSelfSurfaceCapture();
            break;
        case OHOS::Rosen::DO_CLEAR_UIFIRST_CACHE:
            OHOS::Rosen::DoClearUifirstCache();
            break;
        case OHOS::Rosen::DO_REGISTER_TRANSACTION_DATA_CALLBACK:
            OHOS::Rosen::DoRegisterTransactionDataCallback();
            break;
        case OHOS::Rosen::DO_TAKE_SURFACE_CAPTURE_WITH_ALLWINDOWS:
            OHOS::Rosen::DoTakeSurfaceCaptureWithAllWindows();
            break;
        case OHOS::Rosen::DO_FREEZE_SCREEN:
            OHOS::Rosen::DoFreezeScreen();
            break;
        case OHOS::Rosen::DO_REGISTER_CANVAS_CALLBACK:
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
            OHOS::Rosen::DoRegisterCanvasCallback();
#endif
            break;
        case OHOS::Rosen::DO_SUBMIT_CANVAS_PRE_ALLOCATED_BUFFER:
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
            OHOS::Rosen::DoSubmitCanvasPreAllocatedBuffer();
#endif
            break;
        default:
            return -1;
    }
    return 0;
}