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

#include "rsrenderserviceconnection00C_fuzzer.h"

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
#include "transaction/rs_render_to_service_connection.h"
#include "render_server/transaction/rs_client_to_service_connection.h"
#include "platform/ohos/transaction/zidl/rs_irender_service.h"
#include "transaction/zidl/rs_client_to_render_connection_stub.h"
#include "render_server/transaction/zidl/rs_client_to_service_connection_stub.h"
#include "transaction/rs_transaction_proxy.h"
#include "message_parcel.h"
#include "securec.h"
#include <iservice_registry.h>
#include <system_ability_definition.h>
#include "pipeline/render_thread/rs_render_engine.h"
#include "feature/hyper_graphic_manager/hgm_render_context.h"

namespace OHOS {
namespace Rosen {

int32_t g_pid;
sptr<OHOS::Rosen::RSScreenManager> screenManagerPtr_ = OHOS::sptr<OHOS::Rosen::RSScreenManager>::MakeSptr();
RSMainThread* mainThread_ = RSMainThread::Instance();
sptr<RSClientToRenderConnectionStub> toRenderConnectionStub_ = nullptr;
sptr<RSClientToServiceConnectionStub> toServiceConnectionStub_ = nullptr;
sptr<OHOS::Rosen::RSRenderService> renderService_ = nullptr;
namespace {
const uint8_t DO_NOTIFY_TOUCH_EVENT = 0;
const uint8_t TARGET_SIZE = 1;
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

void DoNotifyTouchEvent()
{
    uint32_t touchStatus = 0;
    uint32_t touchCnt = GetData<uint32_t>();
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!dataP.WriteUint32(touchStatus)) {
        return;
    }
    if (!dataP.WriteUint32(touchCnt)) {
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::NOTIFY_TOUCH_EVENT);
    toServiceConnectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoSetHardwareEnabled()
{
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SET_HARDWARE_ENABLED);
    uint64_t id = static_cast<NodeId>(g_pid) << 32;
    bool isEnabled = GetData<bool>();
    uint8_t selfDrawingType = GetData<uint8_t>();
    bool dynamicHardwareEnable = GetData<bool>();
    dataP.WriteUint64(id);
    dataP.WriteBool(isEnabled);
    dataP.WriteUint8(selfDrawingType);
    dataP.WriteBool(dynamicHardwareEnable);
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
        case OHOS::Rosen::DO_NOTIFY_TOUCH_EVENT:
            OHOS::Rosen::DoNotifyTouchEvent();
            break;
        default:
            return -1;
    }
    return 0;
}