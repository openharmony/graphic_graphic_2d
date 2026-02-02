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
#include "render_server/transaction/rs_client_to_service_connection.h"
#include "platform/ohos/rs_irender_service.h"
#include "render_server/transaction/zidl/rs_client_to_service_connection_stub.h"
#include "transaction/rs_transaction_proxy.h"

namespace OHOS {
namespace Rosen {
auto g_pid = getpid();
auto screenManagerPtr_ = RSScreenManager::GetInstance();
auto mainThread_ = RSMainThread::Instance();
sptr<RSIConnectionToken> token_ = new IRemoteStub<RSIConnectionToken>();

DVSyncFeatureParam dvsyncParam;
auto generator = CreateVSyncGenerator();
auto appVSyncController = new VSyncController(generator, 0);
sptr<VSyncDistributor> appVSyncDistributor_ = new VSyncDistributor(appVSyncController, "app", dvsyncParam);
sptr<RSClientToServiceConnectionStub> toServiceConnectionStub_ = new RSClientToServiceConnection(
    g_pid, nullptr, mainThread_, screenManagerPtr_, token_->AsObject(), appVSyncDistributor_);
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

void DoGetHighContrastTextState()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_HIGH_CONTRAST_TEXT_STATE);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoCommitTransaction()
{
    uint32_t code =
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::COMMIT_TRANSACTION);

    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    dataParcel.WriteInt32(0);
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoCreateNode()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_NODE);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_NODE_AND_SURFACE);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;

    FuzzedDataProvider fdp(DATA, g_size);
    uint8_t type = fdp.ConsumeIntegralInRange<uint8_t>(0, 13);
    uint8_t surfaceWindowType = fdp.ConsumeIntegralInRange<uint8_t>(1, 6);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_HIDE_PRIVACY_CONTENT);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_HARDWARE_ENABLED);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::EXECUTE_SYNCHRONOUS_TASK);
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
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_PIXELMAP);
    if (toServiceConnectionStub_ == nullptr) {
        return;
    }
    toServiceConnectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoGetBitmap()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_BITMAP);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;

    NodeId id = static_cast<NodeId>(g_pid) << 32;
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteUint64(id);
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer envirement */
extern "C" int LLVMFuzzerInitialize(int *argc, char ***argv)
{
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
        default:
            return -1;
    }
    return 0;
}