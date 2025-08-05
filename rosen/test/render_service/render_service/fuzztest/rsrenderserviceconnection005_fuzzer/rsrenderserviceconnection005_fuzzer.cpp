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

#include "rsrenderserviceconnection005_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <fuzzer/FuzzedDataProvider.h>
#include <iservice_registry.h>
#include <system_ability_definition.h>
#include <unistd.h>
#include <unordered_map>

#include "message_parcel.h"
#include "securec.h"

#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/main_thread/rs_render_service_connection.h"
#include "platform/ohos/rs_irender_service.h"
#include "transaction/rs_render_service_connection_stub.h"
#include "transaction/rs_transaction_proxy.h"

namespace OHOS {
namespace Rosen {
DECLARE_INTERFACE_DESCRIPTOR(u"ohos.rosen.RenderServiceConnection");
int32_t g_pid;
sptr<OHOS::Rosen::RSScreenManager> screenManagerPtr_ = nullptr;
RSMainThread* mainThread_ = RSMainThread::Instance();
sptr<RSRenderServiceConnectionStub> connectionStub_ = nullptr;

namespace {
const uint8_t DO_SET_SCREEN_CHANGE_CALLBACK = 0;
const uint8_t DO_SET_SCREEN_ACTIVE_MODE = 1;
const uint8_t DO_SET_SCREEN_REFRESH_RATE = 2;
const uint8_t DO_SET_REFRESH_RATE_MODE = 3;
const uint8_t DO_SYNC_FRAME_RATE_RANGE = 4;
const uint8_t DO_UNREGISTER_FRAME_RATE_LINKER = 5;
const uint8_t DO_GET_SCREEN_CURRENT_REFRESH_RATE = 6;
const uint8_t DO_GET_CURRENT_REFRESH_RATE_MODE = 7;
const uint8_t DO_GET_SCREEN_SUPPORTED_REFRESH_RATES = 8;
const uint8_t DO_GET_SHOW_REFRESH_RATE_ENABLED = 9;
const uint8_t DO_SET_SHOW_REFRESH_RATE_ENABLED = 10;
const uint8_t DO_GET_REALTIME_REFRESH_RATE = 11;
const uint8_t DO_GET_REFRESH_INFO = 12;
const uint8_t DO_REFRESH_RATE_UPDATE_CALLBACK = 13;
const uint8_t DO_REGISTER_FRAME_RATE_LINKER_EXPECTED_FPS_CALLBACK = 14;
const uint8_t DO_REFRESH_RATE_MODE_CHANGE_CALLBACK = 15;
const uint8_t DO_GET_REFRESH_INFO_TO_SP = 16;
const uint8_t TARGET_SIZE = 17;

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

void DoSetScreenChangeCallback()
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    uint32_t code =
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_CHANGE_CALLBACK);
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    sptr<RSIScreenChangeCallback> rsIScreenChangeCallback_ = iface_cast<RSIScreenChangeCallback>(remoteObject);

    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteRemoteObject(rsIScreenChangeCallback_->AsObject());
    dataParcel.RewindRead(0);
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetScreenActiveMode()
{
    uint32_t code =
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_ACTIVE_MODE);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    ScreenId id = GetData<uint64_t>();
    uint32_t modeId = GetData<uint32_t>();
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteUint64(id);
    dataParcel.WriteUint32(modeId);
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetScreenRefreshRate()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_REFRESH_RATE);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    uint64_t id = GetData<uint64_t>();
    int32_t sceneId = GetData<int32_t>();
    int32_t rate = GetData<int32_t>();
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteUint64(id);
    dataParcel.WriteInt32(sceneId);
    dataParcel.WriteInt32(rate);
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetRefreshRateMode()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_REFRESH_RATE_MODE);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    int32_t mode = GetData<int32_t>();
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteInt32(mode);
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void CreateVSyncConnection(uint64_t id)
{
    uint32_t code =
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_VSYNC_CONNECTION);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    sptr<VSyncIConnectionToken> vsyncIConnectionToken_ = iface_cast<VSyncIConnectionToken>(remoteObject);
    NodeId windowNodeID = GetData<NodeId>();
    std::string name = GetData<std::string>();
    dataParcel.WriteString(name);
    dataParcel.WriteRemoteObject(vsyncIConnectionToken_->AsObject());
    dataParcel.WriteUint64(id);
    dataParcel.WriteUint64(windowNodeID);
    dataParcel.WriteBool(false);
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void UnregisterFrameRateLinker(uint64_t id)
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::UNREGISTER_FRAME_RATE_LINKER);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteUint64(id);
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSyncFrameRateRange()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SYNC_FRAME_RATE_RANGE);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    uint32_t linkId = GetData<uint32_t>();
    uint64_t id = (static_cast<NodeId>(OHOS::Rosen::g_pid) << 32) + linkId;
    uint32_t min = GetData<uint32_t>();
    uint32_t max = GetData<uint32_t>();
    uint32_t preferred = GetData<uint32_t>();
    uint32_t type = GetData<uint32_t>();
    uint32_t componentScene = GetData<uint32_t>();
    int32_t animatorExpectedFrameRate = GetData<int32_t>();
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteUint64(id);
    dataParcel.WriteUint32(min);
    dataParcel.WriteUint32(max);
    dataParcel.WriteUint32(preferred);
    dataParcel.WriteUint32(type);
    dataParcel.WriteUint32(componentScene);
    dataParcel.WriteInt32(animatorExpectedFrameRate);

    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);

    usleep(10000);
    CreateVSyncConnection(id);

    usleep(10000);
    MessageOption option2;
    MessageParcel dataParcel2;
    MessageParcel replyParcel2;
    dataParcel2.WriteInterfaceToken(GetDescriptor());
    dataParcel2.WriteUint64(id);
    dataParcel2.WriteUint32(min);
    dataParcel2.WriteUint32(max);
    dataParcel2.WriteUint32(preferred);
    dataParcel2.WriteUint32(OHOS::Rosen::NATIVE_VSYNC_FRAME_RATE_TYPE);
    dataParcel2.WriteUint32(componentScene);
    dataParcel2.WriteInt32(animatorExpectedFrameRate);

    connectionStub_->OnRemoteRequest(code, dataParcel2, replyParcel2, option2);

    usleep(10000);
    UnregisterFrameRateLinker(id);
}

void DoUnregisterFrameRateLinker()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::UNREGISTER_FRAME_RATE_LINKER);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    uint32_t linkId = GetData<uint32_t>();
    uint64_t id = (static_cast<NodeId>(OHOS::Rosen::g_pid) << 32) + linkId;
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteUint64(id);
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoGetScreenCurrentRefreshRate()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_CURRENT_REFRESH_RATE);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    uint64_t id = GetData<uint64_t>();
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteUint64(id);
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoGetCurrentRefreshRateMode()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_CURRENT_REFRESH_RATE_MODE);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    dataParcel.WriteInterfaceToken(GetDescriptor());
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoGetScreenSupportedRefreshRates()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_REFRESH_RATES);

    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    ScreenId id = GetData<uint64_t>();
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteUint64(id);
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoGetShowRefreshRateEnabled()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SHOW_REFRESH_RATE_ENABLED);

    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    dataParcel.WriteInterfaceToken(GetDescriptor());
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetShowRefreshRateEnabled()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SHOW_REFRESH_RATE_ENABLED);

    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    bool enabled = GetData<bool>();
    int32_t type = GetData<int32_t>();
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteBool(enabled);
    dataParcel.WriteInt32(type);
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoGetRealtimeRefreshRate()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_REALTIME_REFRESH_RATE);

    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    ScreenId id = GetData<uint64_t>();
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteUint64(id);
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoGetRefreshInfo()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_REFRESH_INFO);

    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    int32_t pid = GetData<int32_t>();
    dataParcel.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor());
    dataParcel.WriteInt32(pid);
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoRegisterHgmRefreshRateUpdateCallback()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REFRESH_RATE_UPDATE_CALLBACK);

    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    bool readRemoteObject = GetData<bool>();
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    sptr<RSIHgmConfigChangeCallback> rsIHgmConfigChangeCallback = iface_cast<RSIHgmConfigChangeCallback>(remoteObject);
    dataParcel.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor());
    dataParcel.WriteBool(readRemoteObject);
    dataParcel.WriteRemoteObject(rsIHgmConfigChangeCallback->AsObject());
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoRegisterFrameRateLinkerExpectedFpsUpdateCallback()
{
    uint32_t code = static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::REGISTER_FRAME_RATE_LINKER_EXPECTED_FPS_CALLBACK);

    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    int32_t dstPid = GetData<int32_t>();
    bool readRemoteObject = GetData<bool>();
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    sptr<RSIFrameRateLinkerExpectedFpsUpdateCallback> fpsUpdateCallback =
        iface_cast<RSIFrameRateLinkerExpectedFpsUpdateCallback>(remoteObject);
    dataParcel.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor());
    dataParcel.WriteInt32(dstPid);
    dataParcel.WriteBool(readRemoteObject);
    dataParcel.WriteRemoteObject(fpsUpdateCallback->AsObject());
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoRegisterHgmRefreshRateModeChangeCallback()
{
    uint32_t code = static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::REFRESH_RATE_MODE_CHANGE_CALLBACK);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    sptr<RSIHgmConfigChangeCallback> rsIHgmConfigChangeCallback =
        iface_cast<RSIHgmConfigChangeCallback>(remoteObject);
    dataParcel.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor());
    dataParcel.WriteRemoteObject(rsIHgmConfigChangeCallback->AsObject());
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoGetRefreshInfoToSP()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_REFRESH_INFO_TO_SP);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    uint64_t nodeId = GetData<uint64_t>();
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteUint64(nodeId);
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer envirement */
extern "C" int LLVMFuzzerInitialize(int *argc, char ***argv)
{
    OHOS::Rosen::g_pid = getpid();
    OHOS::Rosen::screenManagerPtr_ = OHOS::Rosen::impl::RSScreenManager::GetInstance();
    OHOS::Rosen::mainThread_ = OHOS::Rosen::RSMainThread::Instance();
    OHOS::Rosen::mainThread_->runner_ = OHOS::AppExecFwk::EventRunner::Create(true);
    OHOS::Rosen::mainThread_->handler_ =
        std::make_shared<OHOS::AppExecFwk::EventHandler>(OHOS::Rosen::mainThread_->runner_);
    OHOS::sptr<OHOS::Rosen::RSIConnectionToken> token_ = new OHOS::IRemoteStub<OHOS::Rosen::RSIConnectionToken>();

    OHOS::Rosen::DVSyncFeatureParam dvsyncParam;
    auto generator = OHOS::Rosen::CreateVSyncGenerator();
    auto appVSyncController = new OHOS::Rosen::VSyncController(generator, 0);
    OHOS::sptr<OHOS::Rosen::VSyncDistributor> appVSyncDistributor =
        new OHOS::Rosen::VSyncDistributor(appVSyncController, "app", dvsyncParam);
    OHOS::Rosen::connectionStub_ =
        new OHOS::Rosen::RSRenderServiceConnection(OHOS::Rosen::g_pid, nullptr, OHOS::Rosen::mainThread_,
            OHOS::Rosen::screenManagerPtr_, token_->AsObject(), appVSyncDistributor);
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
        case OHOS::Rosen::DO_SET_SCREEN_CHANGE_CALLBACK:
            OHOS::Rosen::DoSetScreenChangeCallback();
            break;
        case OHOS::Rosen::DO_SET_SCREEN_ACTIVE_MODE:
            OHOS::Rosen::DoSetScreenActiveMode();
            break;
        case OHOS::Rosen::DO_SET_SCREEN_REFRESH_RATE:
            OHOS::Rosen::DoSetScreenRefreshRate();
            break;
        case OHOS::Rosen::DO_SET_REFRESH_RATE_MODE:
            OHOS::Rosen::DoSetRefreshRateMode();
            break;
        case OHOS::Rosen::DO_SYNC_FRAME_RATE_RANGE:
            OHOS::Rosen::DoSyncFrameRateRange();
            break;
        case OHOS::Rosen::DO_UNREGISTER_FRAME_RATE_LINKER:
            OHOS::Rosen::DoUnregisterFrameRateLinker();
            break;
        case OHOS::Rosen::DO_GET_SCREEN_CURRENT_REFRESH_RATE:
            OHOS::Rosen::DoGetScreenCurrentRefreshRate();
            break;
        case OHOS::Rosen::DO_GET_CURRENT_REFRESH_RATE_MODE:
            OHOS::Rosen::DoGetCurrentRefreshRateMode();
            break;
        case OHOS::Rosen::DO_GET_SCREEN_SUPPORTED_REFRESH_RATES:
            OHOS::Rosen::DoGetScreenSupportedRefreshRates();
            break;
        case OHOS::Rosen::DO_GET_SHOW_REFRESH_RATE_ENABLED:
            OHOS::Rosen::DoGetShowRefreshRateEnabled();
            break;
        case OHOS::Rosen::DO_SET_SHOW_REFRESH_RATE_ENABLED:
            OHOS::Rosen::DoSetShowRefreshRateEnabled();
            break;
        case OHOS::Rosen::DO_GET_REALTIME_REFRESH_RATE:
            OHOS::Rosen::DoGetRealtimeRefreshRate();
            break;
        case OHOS::Rosen::DO_GET_REFRESH_INFO:
            OHOS::Rosen::DoGetRefreshInfo();
            break;
        case OHOS::Rosen::DO_REFRESH_RATE_UPDATE_CALLBACK:
            OHOS::Rosen::DoRegisterHgmRefreshRateUpdateCallback();
            break;
        case OHOS::Rosen::DO_REGISTER_FRAME_RATE_LINKER_EXPECTED_FPS_CALLBACK:
            OHOS::Rosen::DoRegisterFrameRateLinkerExpectedFpsUpdateCallback();
            break;
        case OHOS::Rosen::DO_REFRESH_RATE_MODE_CHANGE_CALLBACK:
            OHOS::Rosen::DoRegisterHgmRefreshRateModeChangeCallback();
            break;
        case OHOS::Rosen::DO_GET_REFRESH_INFO_TO_SP:
            OHOS::Rosen::DoGetRefreshInfoToSP();
            break;
        default:
            return -1;
    }
    usleep(20000);
    return 0;
}