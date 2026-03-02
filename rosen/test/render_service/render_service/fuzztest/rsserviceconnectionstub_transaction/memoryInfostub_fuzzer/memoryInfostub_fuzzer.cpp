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

#include "memoryInfostub_fuzzer.h"

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
RSMainThread* mainThread_ = RSMainThread::Instance();
sptr<RSClientToServiceConnectionStub> toServiceConnectionStub_ = nullptr;

namespace {
void DoGetMemoryGraphics(FuzzedDataProvider& fdp)
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_MEMORY_GRAPHIC);
    
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    
    int32_t pid = fdp.ConsumeIntegral<int32_t>();
    
    if (!dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!dataP.WriteInt32(pid)) {
        return;
    }
    toServiceConnectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

} // namespace
} // namespace Rosen
} // namespace OHOS

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

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (OHOS::Rosen::toServiceConnectionStub_ == nullptr || data == nullptr) {
        return -1;
    }
    
    FuzzedDataProvider fdp(data, size);
    OHOS::Rosen::DoGetMemoryGraphics(fdp);
    return 0;
}
