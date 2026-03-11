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

#include "watermarkstub_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "pipeline/main_thread/rs_main_thread.h"
#include "transaction/rs_client_to_render_connection.h"
#include "render_server/transaction/rs_client_to_service_connection.h"
#include "pipeline/rs_render_node_gc.h"
#include "pipeline/rs_surface_buffer_callback_manager.h"
#include "platform/ohos/transaction/zidl/rs_irender_service.h"
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
auto g_pid = getpid();
sptr<OHOS::Rosen::RSScreenManager> screenManagerPtr_ = OHOS::sptr<OHOS::Rosen::RSScreenManager>::MakeSptr();
auto mainThread_ = RSMainThread::Instance();

sptr<RSClientToServiceConnectionStub> toServiceConnectionStub_ = nullptr;
sptr<OHOS::Rosen::RSRenderService> renderService_ = nullptr;

namespace {
const uint8_t DO_SET_WATERMARK = 0;
const uint8_t DO_SET_SURFACE_WATERMARK = 1;
const uint8_t DO_SHOW_WATERMARK = 2;
const uint8_t DO_CLEAR_SURFACE_WATERMARK = 3;
const uint8_t DO_CLEAR_SURFACE_WATERMARK_FOR_NODES = 4;
constexpr uint8_t TARGET_SIZE = 5;
constexpr size_t STR_LEN = 10;

std::shared_ptr<Media::PixelMap> CreateTestPixelMap(FuzzedDataProvider& fdp)
{
    Media::InitializationOptions opts;
    opts.size.width = fdp.ConsumeIntegral<int32_t>();
    opts.size.height = fdp.ConsumeIntegral<int32_t>();
    opts.srcPixelFormat = static_cast<Media::PixelFormat>(fdp.ConsumeIntegral<int32_t>());
    opts.pixelFormat = static_cast<Media::PixelFormat>(fdp.ConsumeIntegral<int32_t>());
    opts.alphaType = static_cast<Media::AlphaType>(fdp.ConsumeIntegral<int32_t>());
    opts.scaleMode = static_cast<Media::ScaleMode>(fdp.ConsumeIntegral<int32_t>());
    opts.editable = fdp.ConsumeBool();
    opts.useSourceIfMatch = fdp.ConsumeBool();
    
    return Media::PixelMap::Create(opts);
}

void DoSetWatermark(FuzzedDataProvider& fdp)
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_WATERMARK);
    
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    std::string name = fdp.ConsumeRandomLengthString(STR_LEN);
    auto watermark = CreateTestPixelMap(fdp);
    if (!dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!dataP.WriteString(name) || !dataP.WriteParcelable(watermark.get())) {
        return;
    }
    toServiceConnectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoSetSurfaceWatermark(FuzzedDataProvider& fdp)
{
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SET_SURFACE_WATERMARK);
    
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    pid_t pid = fdp.ConsumeIntegral<pid_t>();
    std::string name = fdp.ConsumeRandomLengthString(STR_LEN);
    bool hasWatermark = fdp.ConsumeBool();
    auto watermark = hasWatermark ? CreateTestPixelMap(fdp) : nullptr;
    uint8_t vectorSize = fdp.ConsumeIntegral<uint8_t>();
    std::vector<NodeId> nodeIdList;
    for (uint8_t i = 0; i < vectorSize; i++) {
        nodeIdList.push_back(fdp.ConsumeIntegral<NodeId>());
    }
    SurfaceWatermarkType watermarkType = static_cast<SurfaceWatermarkType>(fdp.ConsumeIntegral<uint8_t>());
    
    if (!dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!dataP.WriteInt32(pid) || !dataP.WriteString(name)) {
        return;
    }
    if (!dataP.WriteBool(hasWatermark)) {
        return;
    }
    if (hasWatermark && watermark != nullptr) {
        if (!dataP.WriteParcelable(watermark.get())) {
            return;
        }
    }
    if (!dataP.WriteUInt64Vector(nodeIdList) || !dataP.WriteUint8(static_cast<uint8_t>(watermarkType))) {
        return;
    }
    toServiceConnectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoShowWatermark(FuzzedDataProvider& fdp)
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SHOW_WATERMARK);
    
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    auto watermarkImg = CreateTestPixelMap(fdp);
    bool isShow = fdp.ConsumeBool();
    
    if (!dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!dataP.WriteParcelable(watermarkImg.get()) || !dataP.WriteBool(isShow)) {
        return;
    }
    toServiceConnectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoClearSurfaceWatermark(FuzzedDataProvider& fdp)
{
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::CLEAR_SURFACE_WATERMARK);
    
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    pid_t pid = fdp.ConsumeIntegral<pid_t>();
    std::string name = fdp.ConsumeRandomLengthString(STR_LEN);
    
    if (!dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!dataP.WriteInt32(pid) || !dataP.WriteString(name)) {
        return;
    }
    toServiceConnectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoClearSurfaceWatermarkForNodes(FuzzedDataProvider& fdp)
{
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::CLEAR_SURFACE_WATERMARK_FOR_NODES);
    
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    pid_t pid = fdp.ConsumeIntegral<pid_t>();
    std::string name = fdp.ConsumeRandomLengthString(STR_LEN);
    uint8_t vectorSize = fdp.ConsumeIntegral<uint8_t>();
    std::vector<NodeId> nodeIdList;
    for (uint8_t i = 0; i < vectorSize; i++) {
        nodeIdList.push_back(fdp.ConsumeIntegral<NodeId>());
    }
    
    if (!dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!dataP.WriteInt32(pid) || !dataP.WriteString(name) || !dataP.WriteUInt64Vector(nodeIdList)) {
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
        OHOS::sptr<OHOS::Rosen::RSRenderProcessManagerAgent>::MakeSptr(
            OHOS::Rosen::renderService_->renderProcessManager_);

    OHOS::sptr<OHOS::Rosen::RSScreenManagerAgent> screenManagerAgent_ =
        new OHOS::Rosen::RSScreenManagerAgent(OHOS::Rosen::screenManagerPtr_);

    OHOS::Rosen::toServiceConnectionStub_ = new OHOS::Rosen::RSClientToServiceConnection(
        OHOS::Rosen::g_pid, renderServiceAgent_, renderProcessManagerAgent_,
        screenManagerAgent_, token_->AsObject(), vsyncManager->GetVsyncManagerAgent());

    // reset recevier, otherwise maybe crash
    OHOS::Rosen::renderService_->renderPipeline_->uniRenderThread_->uniRenderEngine_ = nullptr;

    OHOS::Rosen::RSMainThread::Instance()->receiver_->connection_ = nullptr;
    OHOS::Rosen::RSMainThread::Instance()->receiver_ = nullptr;
    OHOS::Rosen::RSMainThread::Instance()->mainLoop_ = []() {};
    return 0;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (OHOS::Rosen::toServiceConnectionStub_ == nullptr || data == nullptr) {
        return -1;
    }
    
    FuzzedDataProvider fdp(data, size);
    
    uint8_t tarPos = fdp.ConsumeIntegral<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_SET_WATERMARK:
            OHOS::Rosen::DoSetWatermark(fdp);
            break;
        case OHOS::Rosen::DO_SET_SURFACE_WATERMARK:
            OHOS::Rosen::DoSetSurfaceWatermark(fdp);
            break;
        case OHOS::Rosen::DO_SHOW_WATERMARK:
            OHOS::Rosen::DoShowWatermark(fdp);
            break;
        case OHOS::Rosen::DO_CLEAR_SURFACE_WATERMARK:
            OHOS::Rosen::DoClearSurfaceWatermark(fdp);
            break;
        case OHOS::Rosen::DO_CLEAR_SURFACE_WATERMARK_FOR_NODES:
            OHOS::Rosen::DoClearSurfaceWatermarkForNodes(fdp);
            break;
        default:
            return -1;
    }
    return 0;
}
