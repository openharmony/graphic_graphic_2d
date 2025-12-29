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

#include "rsrenderserviceconnection009_fuzzer.h"

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
#include "transaction/rs_client_to_render_connection.h"
#include "transaction/zidl/rs_client_to_render_connection_stub.h"
#include "render_server/transaction/rs_client_to_service_connection.h"
#include "platform/ohos/transaction/zidl/rs_irender_service.h"
#include "render_server/transaction/zidl/rs_client_to_service_connection_stub.h"
#include "transaction/rs_transaction_proxy.h"

namespace OHOS {
namespace Rosen {
auto g_pid = getpid();
sptr<OHOS::Rosen::RSScreenManager> screenManagerPtr_ = OHOS::sptr<OHOS::Rosen::RSScreenManager>::MakeSptr();
auto mainThread_ = RSMainThread::Instance();
sptr<RSClientToServiceConnectionStub> toServiceConnectionStub_ = nullptr;
sptr<RSClientToRenderConnectionStub> toRenderConnectionStub_ = nullptr;
namespace {
const uint8_t DO_SHOW_WATERMARK = 0;
const uint8_t DO_SET_WATERMARK = 1;
const uint8_t DO_REGISTER_TRANSACTION_DATA_CALLBACK = 2;
const uint8_t TARGET_SIZE = 3;

sptr<RSIClientToServiceConnection> CONN = nullptr;
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

void DoShowWatermark()
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SHOW_WATERMARK);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;

    bool isShow = GetData<bool>();
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteBool(isShow);
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetWatermark()
{
    std::string name = "name";
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    dataP.WriteString(name);
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_WATERMARK);
    if (toServiceConnectionStub_ == nullptr) {
        return;
    }
    toServiceConnectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoRegisterTransactionDataCallback()
{
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        return;
    }
    uint64_t token = GetData<uint64_t>();
    auto timeStamp = GetData<uint64_t>();
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    dataP.WriteUint64(token);
    dataP.WriteUint64(timeStamp);
    dataP.WriteRemoteObject(remoteObject);
    uint32_t code = static_cast<uint32_t>(
        RSIClientToRenderConnectionInterfaceCode::REGISTER_TRANSACTION_DATA_CALLBACK);
    if (toRenderConnectionStub_ == nullptr) {
        return;
    }
    toRenderConnectionStub_->OnRemoteRequest(code, dataP, reply, option);
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
    OHOS::Rosen::mainThread_->mainLoop_ = []() {};

    OHOS::Rosen::DVSyncFeatureParam dvsyncParam;
    auto generator = OHOS::Rosen::CreateVSyncGenerator();
    auto appVSyncController = new OHOS::Rosen::VSyncController(generator, 0);
    OHOS::sptr<OHOS::Rosen::VSyncDistributor> appVSyncDistributor_ =
        new OHOS::Rosen::VSyncDistributor(appVSyncController, "app", dvsyncParam);

    auto handler = std::make_shared<OHOS::AppExecFwk::EventHandler>(OHOS::AppExecFwk::EventRunner::Create(false));
    auto renderPipeline_ = OHOS::Rosen::RSRenderPipeline::Create(handler, nullptr, nullptr, nullptr);
    OHOS::sptr<OHOS::Rosen::RSRenderPipelineAgent> renderPipelineAgent_ =
        OHOS::sptr<OHOS::Rosen::RSRenderPipelineAgent>::MakeSptr(renderPipeline_);

    OHOS::Rosen::RSRenderService renderService_;
    renderService_.Init();

    auto renderServiceAgent_ = OHOS::sptr<OHOS::Rosen::RSRenderServiceAgent>::MakeSptr(renderService_);
    OHOS::sptr<OHOS::Rosen::RSRenderProcessManagerAgent> renderProcessManagerAgent_ =
        OHOS::sptr<OHOS::Rosen::RSRenderProcessManagerAgent>::MakeSptr(renderService_.renderProcessManager_);

    OHOS::sptr<OHOS::Rosen::RSScreenManagerAgent> screenManagerAgent_ =
        new OHOS::Rosen::RSScreenManagerAgent(OHOS::Rosen::screenManagerPtr_);

    OHOS::Rosen::toServiceConnectionStub_ = new OHOS::Rosen::RSClientToServiceConnection(
        OHOS::Rosen::g_pid, renderServiceAgent_, renderProcessManagerAgent_,
        OHOS::Rosen::mainThread_, screenManagerAgent_, token_->AsObject(), appVSyncDistributor_);
    OHOS::Rosen::toRenderConnectionStub_ =
        new OHOS::Rosen::RSClientToRenderConnection(OHOS::Rosen::g_pid, renderPipelineAgent_, token_->AsObject());
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
        case OHOS::Rosen::DO_SHOW_WATERMARK:
            OHOS::Rosen::DoShowWatermark();
            break;
        case OHOS::Rosen::DO_REGISTER_TRANSACTION_DATA_CALLBACK:
            OHOS::Rosen::DoRegisterTransactionDataCallback();
            break;
        case OHOS::Rosen::DO_SET_WATERMARK:
            OHOS::Rosen::DoSetWatermark();
            break;
        default:
            return -1;
    }
    return 0;
}