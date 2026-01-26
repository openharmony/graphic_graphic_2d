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

#include "rsrenderserviceconnection012_fuzzer.h"

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
sptr<OHOS::Rosen::RSRenderService> renderService_ = nullptr;
namespace {
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

void DoGetHighContrastTextState()
{
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::GET_HIGH_CONTRAST_TEXT_STATE);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    dataParcel.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    toRenderConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
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
    OHOS::Rosen::renderService_->rsVSyncDistributor_ =
        new OHOS::Rosen::VSyncDistributor(nullptr, "rs_test", dvsyncParam);

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
        screenManagerAgent_, token_->AsObject(), appVSyncDistributor_);
    OHOS::sptr<OHOS::Rosen::RSClientToRenderConnection> toRenderConnection =
        new OHOS::Rosen::RSClientToRenderConnection(OHOS::Rosen::g_pid, renderPipelineAgent_, token_->AsObject());
    OHOS::Rosen::toRenderConnectionStub_ = toRenderConnection;
    toRenderConnection->cleanDone_ = true;

    // reset recevier, otherwise maybe crash
    OHOS::Rosen::renderService_->rsVSyncDistributor_->connections_.clear();
    OHOS::Rosen::renderService_->rsVSyncDistributor_->connMap_.clear();
    OHOS::Rosen::renderService_->rsVSyncDistributor_->connectionsMap_.clear();
    OHOS::Rosen::renderService_->rsVSyncDistributor_ = nullptr;
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
    OHOS::Rosen::DoGetHighContrastTextState();
    return 0;
}