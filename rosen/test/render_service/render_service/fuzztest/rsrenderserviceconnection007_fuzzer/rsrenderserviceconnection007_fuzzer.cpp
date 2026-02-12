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

#include "rsrenderserviceconnection007_fuzzer.h"

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
#include "render_server/transaction/rs_client_to_service_connection.h"
#include "transaction/rs_client_to_render_connection.h"
#include "render_server/transaction/zidl/rs_client_to_service_connection_stub.h"
#include "transaction/zidl/rs_client_to_render_connection_stub.h"
#include "platform/ohos/transaction/zidl/rs_irender_service.h"
#include "ipc_callbacks/screen_switching_notify_callback_stub.h"
#include "transaction/rs_transaction_proxy.h"

namespace OHOS {
namespace Rosen {
int32_t g_pid;
sptr<OHOS::Rosen::RSScreenManager> screenManagerPtr_ = OHOS::sptr<OHOS::Rosen::RSScreenManager>::MakeSptr();
auto mainThread_ = RSMainThread::Instance();
sptr<RSClientToServiceConnectionStub> toServiceConnectionStub_ = nullptr;
sptr<RSClientToRenderConnectionStub> toRenderConnectionStub_ = nullptr;
sptr<OHOS::Rosen::RSRenderService> renderService_ = nullptr;

namespace {
const uint8_t DO_SET_SCREEN_GAMUT = 0;
const uint8_t DO_SET_SCREEN_GAMUT_MAP = 1;
const uint8_t DO_SET_SCREEN_CORRECTION = 2;
const uint8_t DO_SET_VIRTUAL_MIRROR_SCREEN_CANVAS_ROTATION = 3;
const uint8_t DO_SET_VIRTUAL_MIRROR_SCREEN_SCALE_MODE = 4;
const uint8_t DO_SET_GLOBAL_DARK_COLOR_MODE = 5;
const uint8_t DO_SET_SCREEN_SWITCHING_NOTIFY_CALLBACK = 6;
const uint8_t DO_ADD_VIRTUAL_SCREEN_WHITE_LIST = 7;
const uint8_t DO_REMOVE_VIRTUAL_SCREEN_WHITE_LIST = 8;
const uint8_t TARGET_SIZE = 9;

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

void DoSetScreenColorGamut()
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_GAMUT);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    ScreenId id = GetData<uint64_t>();
    int32_t modeIdx = GetData<int32_t>();
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteUint64(id);
    dataParcel.WriteInt32(modeIdx);
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetScreenGamutMap()
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_GAMUT_MAP);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    ScreenId id = GetData<uint64_t>();
    int32_t mode = GetData<int32_t>();
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteUint64(id);
    dataParcel.WriteInt32(mode);
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetScreenCorrection()
{
    uint64_t id = GetData<uint64_t>();
    uint32_t screenRotation = GetData<uint32_t>();
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!dataP.WriteUint64(id)) {
        return;
    }
    if (!dataP.WriteUint32(screenRotation)) {
        return;
    }

    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_CORRECTION);
    if (toServiceConnectionStub_ == nullptr) {
        return;
    }
    toServiceConnectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoSetVirtualMirrorScreenCanvasRotation()
{
    uint64_t id = GetData<uint64_t>();
    bool canvasRotation = GetData<bool>();
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!dataP.WriteUint64(id)) {
        return;
    }
    if (!dataP.WriteBool(canvasRotation)) {
        return;
    }

    uint32_t code = static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::SET_VIRTUAL_MIRROR_SCREEN_CANVAS_ROTATION);
    if (toServiceConnectionStub_ == nullptr) {
        return;
    }
    toServiceConnectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoSetVirtualMirrorScreenScaleMode()
{
    uint64_t id = GetData<uint64_t>();
    uint32_t scaleMode = GetData<uint32_t>();
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!dataP.WriteUint64(id)) {
        return;
    }
    if (!dataP.WriteUint32(scaleMode)) {
        return;
    }
    uint32_t code = static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::SET_VIRTUAL_MIRROR_SCREEN_SCALE_MODE);
    if (toServiceConnectionStub_ == nullptr) {
        return;
    }
    toServiceConnectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoSetGlobalDarkColorMode()
{
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SET_GLOBAL_DARK_COLOR_MODE);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    bool isDark = GetData<bool>();
    dataParcel.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    dataParcel.WriteBool(isDark);
    toRenderConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
    sleep(1);
}

void DoDropFrameByPid()
{
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::DROP_FRAME_BY_PID);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    std::vector<int32_t> pidList;
    uint8_t pidListSize = GetData<uint8_t>();
    for (size_t i = 0; i < pidListSize; i++) {
        pidList.push_back(GetData<int32_t>());
    }
    dataParcel.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    dataParcel.WriteInt32Vector(pidList);
    toRenderConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
    sleep(1);
}
class CustomTestScreenSwitchingNotifyCallback : public RSScreenSwitchingNotifyCallbackStub {
public:
    explicit CustomTestScreenSwitchingNotifyCallback(const ScreenSwitchingNotifyCallback &callback) : cb_(callback)
    {}
    ~CustomTestScreenSwitchingNotifyCallback() override{};

    void OnScreenSwitchingNotify(bool status) override
    {
        if (cb_ != nullptr) {
            cb_(status);
        }
    }

private:
    ScreenSwitchingNotifyCallback cb_;
};


void DoSetScreenSwitchingNotifyCallback()
{
    uint32_t code =
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_SWITCHING_NOTIFY_CALLBACK);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    bool status = false;
    auto callback = [&status](bool switchingStatus) {
        status = switchingStatus;
    };
    sptr<CustomTestScreenSwitchingNotifyCallback> rsIScreenSwitchingNotifyCallback_ =
        new CustomTestScreenSwitchingNotifyCallback(callback);

    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteRemoteObject(rsIScreenSwitchingNotifyCallback_->AsObject());
    dataParcel.RewindRead(0);
    if (toServiceConnectionStub_ == nullptr) {
        return;
    }
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

 
bool DoAddVirtualScreenWhiteList()
{
    ScreenId id = GetData<ScreenId>();
 
    // generate whitelist
    std::vector<NodeId> whiteList;
    uint8_t whiteListSize = GetData<uint8_t>();
    for (uint8_t i = 0; i < whiteListSize; i++) {
        NodeId nodeId = GetData<NodeId>();
        whiteList.push_back(nodeId);
    }
 
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return false;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!dataP.WriteUint64(id)) {
        return false;
    }
    if (!dataP.WriteUInt64Vector(whiteList)) {
        return false;
    }
 
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::ADD_VIRTUAL_SCREEN_WHITELIST);
    if (toServiceConnectionStub_ == nullptr) {
        return false;
    }
    toServiceConnectionStub_->OnRemoteRequest(code, dataP, reply, option);
    return true;
}
 
bool DoRemoveVirtualScreenWhiteList()
{
    ScreenId id = GetData<ScreenId>();
 
    // generate whitelist
    std::vector<NodeId> whiteList;
    uint8_t whiteListSize = GetData<uint8_t>();
    for (uint8_t i = 0; i < whiteListSize; i++) {
        NodeId nodeId = GetData<NodeId>();
        whiteList.push_back(nodeId);
    }
 
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return false;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!dataP.WriteUint64(id)) {
        return false;
    }
    if (!dataP.WriteUInt64Vector(whiteList)) {
        return false;
    }
 
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REMOVE_VIRTUAL_SCREEN_WHITELIST);
    if (toServiceConnectionStub_ == nullptr) {
        return false;
    }
    toServiceConnectionStub_->OnRemoteRequest(code, dataP, reply, option);
    return true;
}

} // namespace Rosen
} // namespace OHOS

/* Fuzzer envirement */
extern "C" int LLVMFuzzerInitialize(int *argc, char ***argv)
{
    OHOS::Rosen::g_pid = getpid();
    OHOS::Rosen::mainThread_ = OHOS::Rosen::RSMainThread::Instance();
    OHOS::Rosen::mainThread_->handler_ =
        std::make_shared<OHOS::AppExecFwk::EventHandler>(OHOS::AppExecFwk::EventRunner::Create(true));
    OHOS::sptr<OHOS::Rosen::RSIConnectionToken> token_ = new OHOS::IRemoteStub<OHOS::Rosen::RSIConnectionToken>();

    OHOS::Rosen::DVSyncFeatureParam dvsyncParam;
    auto generator = OHOS::Rosen::CreateVSyncGenerator();
    auto appVSyncController = new OHOS::Rosen::VSyncController(generator, 0);
    OHOS::sptr<OHOS::Rosen::VSyncDistributor> appVSyncDistributor_ =
        new OHOS::Rosen::VSyncDistributor(appVSyncController, "app", dvsyncParam);

    OHOS::Rosen::renderService_ = OHOS::sptr<OHOS::Rosen::RSRenderService>::MakeSptr();
    auto vsyncManager = OHOS::sptr<OHOS::Rosen::RSVsyncManager>::MakeSptr();
    vsyncManager->appVSyncDistributor_ = appVSyncDistributor_;

    auto runner = OHOS::AppExecFwk::EventRunner::Create(true);
    runner->Run();
    OHOS::Rosen::renderService_->handler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner);
    OHOS::Rosen::renderService_->renderProcessManager_ =
        OHOS::Rosen::RSRenderProcessManager::Create(*OHOS::Rosen::renderService_);
    auto renderServiceAgent_ = OHOS::sptr<OHOS::Rosen::RSRenderServiceAgent>::MakeSptr(*OHOS::Rosen::renderService_);
    OHOS::sptr<OHOS::Rosen::RSRenderProcessManagerAgent> renderProcessManagerAgent_ =
        OHOS::sptr<OHOS::Rosen::RSRenderProcessManagerAgent>::MakeSptr(OHOS::Rosen::renderService_->renderProcessManager_);

    OHOS::sptr<OHOS::Rosen::RSScreenManagerAgent> screenManagerAgent_ =
        new OHOS::Rosen::RSScreenManagerAgent(OHOS::Rosen::screenManagerPtr_);

    OHOS::sptr<OHOS::Rosen::RSRenderPipelineAgent> renderPipelineAgent_ =
        OHOS::sptr<OHOS::Rosen::RSRenderPipelineAgent>::MakeSptr(OHOS::Rosen::renderService_->renderPipeline_);

    OHOS::Rosen::toServiceConnectionStub_ = new OHOS::Rosen::RSClientToServiceConnection(
        OHOS::Rosen::g_pid, renderServiceAgent_, renderProcessManagerAgent_,
        screenManagerAgent_, token_->AsObject(), vsyncManager->GetVsyncManagerAgent());
    OHOS::sptr<OHOS::Rosen::RSClientToRenderConnection> toRenderConnection =
        new OHOS::Rosen::RSClientToRenderConnection(OHOS::Rosen::g_pid, renderPipelineAgent_, token_->AsObject());
    OHOS::Rosen::toRenderConnectionStub_ = toRenderConnection;
    toRenderConnection->cleanDone_ = true;

    // reset recevier, otherwise maybe crash
    OHOS::Rosen::renderService_->renderPipeline_->uniRenderThread_->uniRenderEngine_ = nullptr;


    OHOS::Rosen::RSMainThread::Instance()->receiver_->connection_ = nullptr;
    OHOS::Rosen::RSMainThread::Instance()->receiver_ = nullptr;
    OHOS::Rosen::RSMainThread::Instance()->mainLoop_ = []() {};
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
        case OHOS::Rosen::DO_SET_SCREEN_GAMUT:
            OHOS::Rosen::DoSetScreenColorGamut();
            break;
        case OHOS::Rosen::DO_SET_SCREEN_GAMUT_MAP:
            OHOS::Rosen::DoSetScreenGamutMap();
            break;
        case OHOS::Rosen::DO_SET_SCREEN_CORRECTION:
            OHOS::Rosen::DoSetScreenCorrection();
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_MIRROR_SCREEN_CANVAS_ROTATION:
            OHOS::Rosen::DoSetVirtualMirrorScreenCanvasRotation();
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_MIRROR_SCREEN_SCALE_MODE:
            OHOS::Rosen::DoSetVirtualMirrorScreenScaleMode();
            break;
        case OHOS::Rosen::DO_SET_GLOBAL_DARK_COLOR_MODE:
            OHOS::Rosen::DoSetGlobalDarkColorMode();
            break;
        case OHOS::Rosen::DO_SET_SCREEN_SWITCHING_NOTIFY_CALLBACK:
            OHOS::Rosen::DoSetScreenSwitchingNotifyCallback();
            break;
        case OHOS::Rosen::DO_ADD_VIRTUAL_SCREEN_WHITE_LIST:
            OHOS::Rosen::DoAddVirtualScreenWhiteList();
            break;
        case OHOS::Rosen::DO_REMOVE_VIRTUAL_SCREEN_WHITE_LIST:
            OHOS::Rosen::DoRemoveVirtualScreenWhiteList();
            break;
        default:
            return -1;
    }
    return 0;
}