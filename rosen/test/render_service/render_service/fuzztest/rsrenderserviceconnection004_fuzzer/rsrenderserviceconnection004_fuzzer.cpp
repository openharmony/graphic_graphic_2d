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

#include "rsrenderserviceconnection004_fuzzer.h"

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
#include "platform/ohos/transaction/zidl/rs_irender_service.h"
#include "render_server/transaction/zidl/rs_client_to_service_connection_stub.h"
#include "transaction/rs_client_to_render_connection.h"
#include "transaction/zidl/rs_client_to_render_connection_stub.h"
#include "transaction/rs_transaction_proxy.h"
#include "ipc_callbacks/pointer_render/pointer_luminance_callback_stub.h"
#include "ipc_callbacks/buffer_available_callback_stub.h"
#include "ipc_callbacks/buffer_clear_callback_stub.h"
#include "transaction/rs_render_to_service_connection.h"
#include "feature/hyper_graphic_manager/hgm_render_context.h"

namespace OHOS {
namespace Rosen {
auto g_pid = getpid();
sptr<OHOS::Rosen::RSScreenManager> screenManagerPtr_ = OHOS::sptr<OHOS::Rosen::RSScreenManager>::MakeSptr();
auto mainThread_ = RSMainThread::Instance();
sptr<RSIConnectionToken> token_ = new IRemoteStub<RSIConnectionToken>();
sptr<RSClientToServiceConnectionStub> toServiceConnectionStub_ = nullptr;
sptr<RSClientToRenderConnectionStub> toRenderConnectionStub_ = nullptr;
sptr<OHOS::Rosen::RSRenderService> renderService_ = nullptr;

namespace {
const uint8_t DO_SET_POINTER_COLOR_INVERSION_CONFIG = 0;
const uint8_t DO_SET_POINTER_COLOR_INVERSION_ENABLED = 1;
const uint8_t DO_REGISTER_POINTER_LUMINANCE_CALLBACK = 2;
const uint8_t DO_UNREGISTER_POINTER_LUMINANCE_CALLBACK = 3;
const uint8_t DO_REGISTER_APPLICATION_AGENT = 4;
const uint8_t DO_SET_BUFFER_AVAILABLE_LISTENER = 5;
const uint8_t DO_SET_BUFFER_CLEAR_LISTENER = 6;
const uint8_t TARGET_SIZE = 7;

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

void DoSetPointerColorInversionConfig()
{
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    MessageParcel dataParcel;
    MessageParcel reply;
    MessageOption option;
    if (!dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return;
    }
    float darkBuffer = GetData<float>();
    float brightBuffer = GetData<float>();
    int64_t interval = GetData<int64_t>();
    int32_t rangeSize = GetData<int64_t>();
    dataParcel.WriteFloat(darkBuffer);
    dataParcel.WriteFloat(brightBuffer);
    dataParcel.WriteInt64(interval);
    dataParcel.WriteInt32(rangeSize);
    uint32_t code = static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::SET_POINTER_COLOR_INVERSION_CONFIG);
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, reply, option);
#endif
}

void DoSetPointerColorInversionEnabled()
{
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    MessageParcel dataParcel;
    MessageParcel reply;
    MessageOption option;
    if (!dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return;
    }
    bool enable = GetData<bool>();
    dataParcel.WriteBool(enable);
    uint32_t code = static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::SET_POINTER_COLOR_INVERSION_ENABLED);
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, reply, option);
#endif
}

#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
class CustomTestPointerLuminanceChangeCallback : public RSPointerLuminanceChangeCallbackStub {
public:
    explicit CustomTestPointerLuminanceChangeCallback(const PointerLuminanceChangeCallback &callback) : cb_(callback)
    {}
    ~CustomTestPointerLuminanceChangeCallback() override{};

    void OnPointerLuminanceChanged(int32_t brightness) override
    {
        if (cb_ != nullptr) {
            cb_(brightness);
        }
    }

private:
    PointerLuminanceChangeCallback cb_;
};
#endif

void DoRegisterPointerLuminanceChangeCallback()
{
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    uint32_t code = static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::REGISTER_POINTER_LUMINANCE_CALLBACK);

    int32_t brightness = GetData<int32_t>();
    auto callback = [&brightness](int32_t brightness_) { brightness = brightness_; };
    sptr<CustomTestPointerLuminanceChangeCallback> rsIPointerLuminanceChangeCallback_ =
        new CustomTestPointerLuminanceChangeCallback(callback);

    if (!dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return;
    }
    dataParcel.WriteRemoteObject(rsIPointerLuminanceChangeCallback_->AsObject());
    dataParcel.RewindRead(0);
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
#endif
}

void DoUnRegisterPointerLuminanceChangeCallback()
{
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    uint32_t code = static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::REGISTER_POINTER_LUMINANCE_CALLBACK);

    if (!dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return;
    }
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
#endif
}

class ApplicationAgentImpl : public IRemoteStub<IApplicationAgent> {
public:
    int OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) override
    {
        return 0;
    }
    void OnTransaction(std::shared_ptr<RSTransactionData> transactionData) override
    {
    }
};

void DoRegisterApplicationAgent()
{
    uint32_t code =
        static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::REGISTER_APPLICATION_AGENT);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    sptr<ApplicationAgentImpl> agent = new ApplicationAgentImpl();
    dataParcel.WriteRemoteObject(agent);
    dataParcel.RewindRead(0);
    toRenderConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

class CustomTestBufferAvailableCallback : public RSBufferAvailableCallbackStub {
public:
    CustomTestBufferAvailableCallback() = default;
    ~CustomTestBufferAvailableCallback() = default;

    void OnBufferAvailable() override
    {}
};

void DoRegisterBufferAvailableListener()
{
    uint32_t code =
        static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SET_BUFFER_AVAILABLE_LISTENER);

    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    sptr<RSIBufferAvailableCallback> rsIBufferAvailableCallback_ = new CustomTestBufferAvailableCallback();
    auto nodeId = static_cast<NodeId>(g_pid) << 32;
    bool isFromRenderThread = GetData<bool>();
    dataParcel.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    dataParcel.WriteUint64(nodeId);
    dataParcel.WriteRemoteObject(rsIBufferAvailableCallback_->AsObject());
    dataParcel.WriteBool(isFromRenderThread);
    dataParcel.RewindRead(0);
    toRenderConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

class CustomTestBufferClearCallback : public RSBufferClearCallbackStub {
public:
    CustomTestBufferClearCallback() = default;
    ~CustomTestBufferClearCallback() = default;

    void OnBufferClear() override
    {}
};

void DoRegisterBufferClearListener()
{
    NodeId nodeId = GetData<NodeId>();
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    sptr<RSIBufferClearCallback> rsBufferClearCallback = new CustomTestBufferClearCallback();

    dataP.WriteUint64(nodeId);
    dataP.WriteRemoteObject(rsBufferClearCallback->AsObject());
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SET_BUFFER_CLEAR_LISTENER);

    toRenderConnectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

} // namespace Rosen
} // namespace OHOS

/* Fuzzer envirement */
extern "C" int LLVMFuzzerInitialize(int *argc, char ***argv)
{
    OHOS::Rosen::g_pid = getpid();
    OHOS::sptr<OHOS::Rosen::RSIConnectionToken> token_ = new OHOS::IRemoteStub<OHOS::Rosen::RSIConnectionToken>();
    OHOS::Rosen::DVSyncFeatureParam dvsyncParam;
    auto generator = OHOS::Rosen::CreateVSyncGenerator();
    auto appVSyncController = new OHOS::Rosen::VSyncController(generator, 0);
    OHOS::sptr<OHOS::Rosen::VSyncDistributor> appVSyncDistributor_ =
        new OHOS::Rosen::VSyncDistributor(appVSyncController, "app", dvsyncParam);

    OHOS::Rosen::renderService_ = new OHOS::Rosen::RSRenderService();
    OHOS::Rosen::RSUniRenderThread::Instance().InitGrContext();
    
    auto runner = OHOS::AppExecFwk::EventRunner::Create(true);
    runner->Run();
    OHOS::Rosen::renderService_->handler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner);

    OHOS::Rosen::renderService_->vsyncManager_ = OHOS::sptr<OHOS::Rosen::RSVsyncManager>::MakeSptr();
    OHOS::Rosen::renderService_->screenManager_ = OHOS::sptr<OHOS::Rosen::RSScreenManager>::MakeSptr();
    OHOS::Rosen::renderService_->vsyncManager_->init(OHOS::Rosen::renderService_->screenManager_);

    OHOS::Rosen::renderService_->renderProcessManager_ =
        OHOS::Rosen::RSRenderProcessManager::Create(*OHOS::Rosen::renderService_);

    auto renderServiceAgent_ = OHOS::sptr<OHOS::Rosen::RSRenderServiceAgent>::MakeSptr(*OHOS::Rosen::renderService_);
    OHOS::sptr<OHOS::Rosen::RSRenderProcessManagerAgent> renderProcessManagerAgent_ =
        OHOS::sptr<OHOS::Rosen::RSRenderProcessManagerAgent>::MakeSptr(
            OHOS::Rosen::renderService_->renderProcessManager_);

    OHOS::sptr<OHOS::Rosen::RSScreenManagerAgent> screenManagerAgent_ =
        new OHOS::Rosen::RSScreenManagerAgent(OHOS::Rosen::screenManagerPtr_);
    OHOS::Rosen::renderService_->rsRenderComposerManager_ = std::make_shared<OHOS::Rosen::RSRenderComposerManager>(
        OHOS::Rosen::renderService_->handler_);

    OHOS::Rosen::toServiceConnectionStub_ = new OHOS::Rosen::RSClientToServiceConnection(
        OHOS::Rosen::g_pid, renderServiceAgent_, renderProcessManagerAgent_,
        screenManagerAgent_, token_->AsObject(), OHOS::Rosen::renderService_->vsyncManager_->GetVsyncManagerAgent());

    OHOS::sptr<OHOS::Rosen::RSRenderPipelineAgent> renderPipelineAgent_ =
        OHOS::sptr<OHOS::Rosen::RSRenderPipelineAgent>::MakeSptr(OHOS::Rosen::renderService_->renderPipeline_);
    OHOS::sptr<OHOS::Rosen::RSClientToRenderConnection> toRenderConnection =
        new OHOS::Rosen::RSClientToRenderConnection(OHOS::Rosen::g_pid, renderPipelineAgent_, token_->AsObject());
    OHOS::Rosen::toRenderConnectionStub_ = toRenderConnection;
    toRenderConnection->cleanDone_ = true;

    OHOS::sptr<OHOS::Rosen::RSRenderToServiceConnection> g_rsConn =
        OHOS::sptr<OHOS::Rosen::RSRenderToServiceConnection>::MakeSptr(
            renderServiceAgent_, renderProcessManagerAgent_, screenManagerAgent_);
    OHOS::Rosen::RSMainThread::Instance()->hgmRenderContext_ =
        std::make_shared<OHOS::Rosen::HgmRenderContext>(g_rsConn);

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
        case OHOS::Rosen::DO_SET_POINTER_COLOR_INVERSION_CONFIG:
            OHOS::Rosen::DoSetPointerColorInversionConfig();
            break;
        case OHOS::Rosen::DO_SET_POINTER_COLOR_INVERSION_ENABLED:
            OHOS::Rosen::DoSetPointerColorInversionEnabled();
            break;
        case OHOS::Rosen::DO_REGISTER_POINTER_LUMINANCE_CALLBACK:
            OHOS::Rosen::DoRegisterPointerLuminanceChangeCallback();
            break;
        case OHOS::Rosen::DO_UNREGISTER_POINTER_LUMINANCE_CALLBACK:
            OHOS::Rosen::DoUnRegisterPointerLuminanceChangeCallback();
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
        default:
            return -1;
    }
    return 0;
}