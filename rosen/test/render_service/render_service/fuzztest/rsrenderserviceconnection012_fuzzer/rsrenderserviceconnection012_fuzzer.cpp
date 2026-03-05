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
const uint8_t DO_GET_HIGH_CONTRAST_TEXT_STATE = 0;
const uint8_t DO_COMMIT_TRANSACTION = 1;
const uint8_t DO_CREATE_NODE = 2;
const uint8_t DO_CREATE_NODE_AND_SURFACE = 3;
const uint8_t DO_SET_HIDE_PRIVACY_CONTENT = 4;
const uint8_t DO_SET_HARDWARE_ENABLED = 5;
const uint8_t DO_EXECUTE_SYNCHRONOUS_TASK = 6;
const uint8_t DO_GET_PIXELMAP = 7;
const uint8_t DO_GET_BITMAP = 8;
const uint8_t DO_GET_MEMORY_GRAPHICS = 9;
const uint8_t TARGET_SIZE = 10;
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

void DoGetHighContrastTextState()
{
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::GET_HIGH_CONTRAST_TEXT_STATE);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    dataParcel.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    toRenderConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoCommitTransaction()
{
    uint32_t code =
        static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::COMMIT_TRANSACTION);

    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    dataParcel.WriteInt32(0);
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoCreateNode()
{
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::CREATE_NODE);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;

    NodeId id = static_cast<NodeId>(g_pid) << 32;
    dataParcel.WriteUint64(id);
    dataParcel.WriteString("SurfaceName");
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoCreateNodeAndSurface()
{
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::CREATE_NODE_AND_SURFACE);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;

    uint8_t type = GetData<uint8_t>() % 14;
    uint8_t surfaceWindowType = GetData<uint8_t>() % 11;
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
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetHidePrivacyContent()
{
    pid_t  newPid = getpid();
    NodeId nodeId = static_cast<NodeId>(newPid) << 32;
    bool needHidePrivacyContent = GetData<bool>();
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    dataP.WriteUint64(nodeId);
    dataP.WriteBool(needHidePrivacyContent);
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SET_HIDE_PRIVACY_CONTENT);
    toServiceConnectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoSetHardwareEnabled()
{
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
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
    toServiceConnectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoExecuteSynchronousTask()
{
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::EXECUTE_SYNCHRONOUS_TASK);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;

    option.SetFlags(MessageOption::TF_SYNC);
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    std::shared_ptr<RSRenderPropertyBase> property = std::make_shared<RSRenderProperty<bool>>();
    NodeId targetId = static_cast<NodeId>(g_pid) << 32;
    auto task = std::make_shared<RSNodeGetShowingPropertyAndCancelAnimation>(targetId, property);
    task->Marshalling(dataParcel);
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoGetPixelmap()
{
    pid_t  newPid = getpid();
    NodeId nodeId = static_cast<NodeId>(newPid) << 32;
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return;
    }
    dataP.WriteUint64(nodeId);
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::GET_PIXELMAP);
    if (toServiceConnectionStub_ == nullptr) {
        return;
    }
    toServiceConnectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoGetBitmap()
{
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::GET_BITMAP);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;

    NodeId id = static_cast<NodeId>(g_pid) << 32;
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteUint64(id);
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoGetMemoryGraphics()
{
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_MEMORY_GRAPHICS);
    toServiceConnectionStub_->OnRemoteRequest(code, dataP, reply, option);
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
        OHOS::sptr<OHOS::Rosen::RSRenderProcessManagerAgent>::MakeSptr(
            OHOS::Rosen::renderService_->renderProcessManager_);

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
    /* Run your code on data */
    if (!OHOS::Rosen::Init(data, size)) {
        return -1;
    }
    /* Run your code on data */
    uint8_t tarPos = OHOS::Rosen::GetData<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_GET_HIGH_CONTRAST_TEXT_STATE:
            OHOS::Rosen::DoGetHighContrastTextState();
            break;
        case OHOS::Rosen::DO_COMMIT_TRANSACTION:
            OHOS::Rosen::DoCommitTransaction();
            break;
        case OHOS::Rosen::DO_CREATE_NODE:
            OHOS::Rosen::DoCreateNode();
            break;
        case OHOS::Rosen::DO_CREATE_NODE_AND_SURFACE:
            OHOS::Rosen::DoCreateNodeAndSurface();
            break;
        case OHOS::Rosen::DO_SET_HIDE_PRIVACY_CONTENT:
            OHOS::Rosen::DoSetHidePrivacyContent();
            break;
        case OHOS::Rosen::DO_SET_HARDWARE_ENABLED:
            OHOS::Rosen::DoSetHardwareEnabled();
            break;
        case OHOS::Rosen::DO_EXECUTE_SYNCHRONOUS_TASK:
            OHOS::Rosen::DoExecuteSynchronousTask();
            break;
        case OHOS::Rosen::DO_GET_PIXELMAP:
            OHOS::Rosen::DoGetPixelmap();
            break;
        case OHOS::Rosen::DO_GET_BITMAP:
            OHOS::Rosen::DoGetBitmap();
            break;
        case OHOS::Rosen::DO_GET_MEMORY_GRAPHICS:
            OHOS::Rosen::DoGetMemoryGraphics();
            break;
        default:
            return -1;
    }
    return 0;
}