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

#include "rsrenderserviceconnection002_fuzzer.h"

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
sptr<RSClientToServiceConnectionStub> rsToServiceConnStub_ = nullptr;
namespace {
const uint8_t DO_CREATE_VIRTUAL_SCREEN = 0;
const uint8_t DO_SET_VIRTUAL_SCREEN_RESOLUTION = 1;
const uint8_t DO_SET_VIRTUAL_SCREEN_SURFACE = 2;
const uint8_t DO_SET_VIRTUAL_SCREEN_BLACK_LIST = 3;
const uint8_t DO_ADD_VIRTUAL_SCREEN_BLACK_LIST = 4;
const uint8_t DO_REMOVE_VIRTUAL_SCREEN_BLACK_LIST = 5;
const uint8_t DO_SET_VIRTUAL_SCREEN_SECURITY_EXEMPTION_LIST = 6;
const uint8_t DO_REMOVE_VIRTUAL_SCREEN = 7;
const uint8_t DO_GET_VIRTUAL_SCREEN_RESOLUTION = 8;
const uint8_t DO_RESIZE_VIRTUAL_SCREEN = 9;
const uint8_t DO_SET_VIRTUAL_SCREEN_USING_STATUS = 10;
const uint8_t DO_SET_VIRTUAL_SCREEN_REFRESH_RATE = 11;
const uint8_t DO_SET_VIRTUAL_SCREEN_STATUS = 12;
const uint8_t DO_SET_VIRTUAL_SCREEN_TYPE_BLACK_LIST = 13;
const uint8_t TARGET_SIZE = 14;

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
    rsToServiceConnStub_ =
        new RSClientToServiceConnection(g_pid, nullptr, nullptr, nullptr, token_->AsObject(), nullptr);
    return true;
}
} // namespace

void DoCreateVirtualScreen()
{
    uint32_t code =
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_VIRTUAL_SCREEN);

    MessageOption option1;
    MessageParcel dataParcel1;
    MessageParcel replyParcel1;
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("FuzzTest");
    sptr<IBufferProducer> bufferProducer_ = cSurface->GetProducer();
    if (!bufferProducer_) {
        return;
    }
    std::string name = GetData<std::string>();
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    bool useSurface = GetData<bool>();
    uint64_t associatedScreenId = GetData<uint64_t>();
    int32_t flags = GetData<int32_t>();
    std::vector<NodeId> whiteList;
    uint8_t listSize = GetData<uint8_t>();
    for (int i = 0; i < listSize; i++) {
        NodeId nodeId = GetData<NodeId>();
        whiteList.push_back(nodeId);
    }
    if (!dataParcel1.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor()) ||
        !dataParcel1.WriteString(name) || !dataParcel1.WriteUint32(width) || !dataParcel1.WriteUint32(height) ||
        !dataParcel1.WriteBool(useSurface) || !dataParcel1.WriteRemoteObject(bufferProducer_->AsObject()) ||
        !dataParcel1.WriteUint64(associatedScreenId) || !dataParcel1.WriteInt32(flags) ||
        !dataParcel1.WriteUInt64Vector(whiteList)) {
        return;
    }
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel1, replyParcel1, option1);

    MessageOption option2;
    MessageParcel dataParcel2;
    MessageParcel replyParcel2;
    if (!dataParcel2.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return;
    }
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel2, replyParcel2, option2);

    MessageOption option3;
    MessageParcel dataParcel3;
    MessageParcel replyParcel3;
    if (!dataParcel3.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor()) ||
        !dataParcel3.WriteString("test")) {
        return;
    }
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel3, replyParcel3, option3);

    MessageOption option4;
    MessageParcel dataParcel4;
    MessageParcel replyParcel4;
    if (!dataParcel4.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor()) ||
        !dataParcel4.WriteString("test") || !dataParcel4.WriteUint32(0)) {
        return;
    }
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel4, replyParcel4, option4);

    MessageOption option5;
    MessageParcel dataParcel5;
    MessageParcel replyParcel5;
    if (!dataParcel5.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor()) ||
        !dataParcel5.WriteString("test") || !dataParcel5.WriteUint32(0) || !dataParcel5.WriteUint32(0)) {
        return;
    }
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel5, replyParcel5, option5);

    MessageOption option6;
    MessageParcel dataParcel6;
    MessageParcel replyParcel6;
    if (!dataParcel6.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor()) ||
        !dataParcel6.WriteString("test") || !dataParcel6.WriteUint32(0) || !dataParcel6.WriteUint32(0) ||
        !dataParcel6.WriteBool(true)) {
        return;
    }
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel6, replyParcel6, option6);

    MessageOption option7;
    MessageParcel dataParcel7;
    MessageParcel replyParcel7;
    if (!dataParcel7.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor()) ||
        !dataParcel7.WriteString("test") || !dataParcel7.WriteUint32(0) || !dataParcel7.WriteUint32(0) ||
        !dataParcel7.WriteBool(true) || !dataParcel7.WriteRemoteObject(bufferProducer_->AsObject())) {
        return;
    }
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel7, replyParcel7, option7);

    MessageOption option8;
    MessageParcel dataParcel8;
    MessageParcel replyParcel8;
    if (!dataParcel8.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor()) ||
        !dataParcel8.WriteString("test") || !dataParcel8.WriteUint32(0) || !dataParcel8.WriteUint32(0) ||
        !dataParcel8.WriteBool(false) || !dataParcel8.WriteUint64(0)) {
        return;
    }
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel8, replyParcel8, option8);

    MessageOption option9;
    MessageParcel dataParcel9;
    MessageParcel replyParcel9;
    if (!dataParcel9.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor()) ||
        !dataParcel9.WriteString("test") || !dataParcel9.WriteUint32(0) || !dataParcel9.WriteUint32(0) ||
        !dataParcel9.WriteBool(false) || !dataParcel9.WriteUint64(0)) {
        return;
    }
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel9, replyParcel9, option9);

    MessageOption option10;
    MessageParcel dataParcel10;
    MessageParcel replyParcel10;
    if (!dataParcel10.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor()) ||
        !dataParcel10.WriteString("test") || !dataParcel10.WriteUint32(0) || !dataParcel10.WriteUint32(0) ||
        !dataParcel10.WriteBool(false) || !dataParcel10.WriteUint64(0) || !dataParcel10.WriteInt32(0)) {
        return;
    }
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel10, replyParcel10, option10);

    MessageOption option11;
    MessageParcel dataParcel11;
    MessageParcel replyParcel11;
    if (!dataParcel11.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor()) ||
        !dataParcel11.WriteString("test") || !dataParcel11.WriteUint32(0) || !dataParcel11.WriteUint32(0) ||
        !dataParcel11.WriteBool(false) || !dataParcel11.WriteUint64(0) || !dataParcel11.WriteInt32(0) ||
        !dataParcel11.WriteUInt64Vector({})) {
        return;
    }
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel11, replyParcel11, option11);

    MessageOption option12;
    MessageParcel dataParcel12;
    MessageParcel replyParcel12;
    if (!dataParcel12.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor()) ||
        !dataParcel12.WriteString("test") || !dataParcel12.WriteUint32(0) || !dataParcel12.WriteUint32(0) ||
        !dataParcel12.WriteBool(false) || !dataParcel12.WriteUint64(0) || !dataParcel12.WriteInt32(0) ||
        !dataParcel12.WriteUInt64Vector({})) {
        return;
    }
    replyParcel12.writable_ = false;
    replyParcel12.data_ = nullptr;
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel12, replyParcel12, option12);
}

void DoSetVirtualScreenResolution()
{
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint64_t id = GetData<uint64_t>();
    if (!dataP.WriteUint64(id)) {
        return;
    }
    uint32_t width = GetData<uint32_t>();
    if (!dataP.WriteUint32(width)) {
        return;
    }
    uint32_t height = GetData<uint32_t>();
    if (!dataP.WriteUint32(height)) {
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_RESOLUTION);
    rsToServiceConnStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoSetVirtualScreenSurface()
{
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint64_t id = GetData<uint64_t>();
    if (!dataP.WriteUint64(id)) {
        return;
    }
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("DisplayNode");
    sptr<IBufferProducer> bp = cSurface->GetProducer();
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(bp);
    auto producer = pSurface->GetProducer();
    if (!dataP.WriteRemoteObject(producer->AsObject())) {
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_SURFACE);
    toServiceConnectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoSetVirtualScreenBlackList()
{
    uint64_t id = GetData<uint64_t>();
    uint64_t nodeId = GetData<uint64_t>();
    std::vector<uint64_t> blackListVector;
    uint8_t listSize = GetData<uint8_t>();
    for (int i = 0; i < listSize; i++) {
        blackListVector.push_back(nodeId);
    }
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!dataP.WriteUint64(id)) {
        return;
    }
    if (!dataP.WriteUInt64Vector(blackListVector)) {
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_BLACKLIST);
    toServiceConnectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoAddVirtualScreenBlackList()
{
    uint64_t id = GetData<uint64_t>();
    uint64_t nodeId = GetData<uint64_t>();
    std::vector<uint64_t> blackListVector;
    uint8_t listSize = GetData<uint8_t>();
    for (int i = 0; i < listSize; i++) {
        blackListVector.push_back(nodeId);
    }
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!dataP.WriteUint64(id)) {
        return;
    }
    if (!dataP.WriteUInt64Vector(blackListVector)) {
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::ADD_VIRTUAL_SCREEN_BLACKLIST);
    toServiceConnectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoRemoveVirtualScreenBlackList()
{
    uint64_t id = GetData<uint64_t>();
    uint64_t nodeId = GetData<uint64_t>();
    std::vector<uint64_t> blackListVector;
    uint8_t listSize = GetData<uint8_t>();
    for (int i = 0; i < listSize; i++) {
        blackListVector.push_back(nodeId);
    }
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!dataP.WriteUint64(id)) {
        return;
    }
    if (!dataP.WriteUInt64Vector(blackListVector)) {
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REMOVE_VIRTUAL_SCREEN_BLACKLIST);
    toServiceConnectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoSetVirtualScreenSecurityExemptionList()
{
    uint64_t id = GetData<uint64_t>();
    std::vector<uint64_t> secExemptionListVector;
    uint8_t listSize = GetData<uint8_t>();
    for (int i = 0; i < listSize; i++) {
        uint64_t nodeId = GetData<uint64_t>();
        secExemptionListVector.push_back(nodeId);
    }
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
    if (!dataP.WriteUInt64Vector(secExemptionListVector)) {
        return;
    }

    uint32_t code = static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_SECURITY_EXEMPTION_LIST);
    toServiceConnectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoRemoveVirtualScreen()
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REMOVE_VIRTUAL_SCREEN);
    ScreenId id = GetData<uint64_t>();
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteUint64(id);
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoGetVirtualScreenResolution()
{
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint64_t id = GetData<uint64_t>();
    if (!dataP.WriteUint64(id)) {
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_VIRTUAL_SCREEN_RESOLUTION);
    toServiceConnectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoResizeVirtualScreen()
{
    uint64_t id = GetData<uint64_t>();
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
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
    if (!dataP.WriteUint32(width)) {
        return;
    }
    if (!dataP.WriteUint32(height)) {
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::RESIZE_VIRTUAL_SCREEN);
    rsToServiceConnStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoSetVirtualScreenUsingStatus()
{
    bool isVirtualScreenUsingStatus = GetData<bool>();
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return;
    }
    if (!dataP.WriteBool(isVirtualScreenUsingStatus)) {
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_USING_STATUS);
    toServiceConnectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoSetVirtualScreenRefreshRate()
{
    uint64_t id = GetData<uint64_t>();
    uint32_t maxRefreshRate = GetData<uint32_t>();
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
    if (!dataP.WriteUint32(maxRefreshRate)) {
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_REFRESH_RATE);
    toServiceConnectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoSetVirtualScreenStatus()
{
    uint64_t id = GetData<uint64_t>();
    uint64_t screenStatus = GetData<uint64_t>();
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
    if (!dataP.WriteUint8(screenStatus)) {
        return;
    }

    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_STATUS);
    if (toServiceConnectionStub_ == nullptr) {
        return;
    }
    toServiceConnectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoSetVirtualScreenTypeBlackList()
{
    uint64_t id = GetData<uint64_t>();
    uint8_t nodeType = GetData<uint8_t>();
    std::vector<NodeType> typeBlackListVector;
    uint8_t listSize = GetData<uint8_t>();
    for (int i = 0; i < listSize; i++) {
        typeBlackListVector.push_back(nodeType);
    }
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!dataP.WriteUint64(id)) {
        return;
    }
    if (!dataP.WriteUInt8Vector(typeBlackListVector)) {
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_TYPE_BLACKLIST);
    toServiceConnectionStub_->OnRemoteRequest(code, dataP, reply, option);
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
    if (!OHOS::Rosen::Init(data, size)) {
        return -1;
    }
    /* Run your code on data */
    uint8_t tarPos = OHOS::Rosen::GetData<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_CREATE_VIRTUAL_SCREEN:
            OHOS::Rosen::DoCreateVirtualScreen();
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_SCREEN_RESOLUTION:
            OHOS::Rosen::DoSetVirtualScreenResolution();
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_SCREEN_SURFACE:
            OHOS::Rosen::DoSetVirtualScreenSurface();
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_SCREEN_BLACK_LIST:
            OHOS::Rosen::DoSetVirtualScreenBlackList();
            break;
        case OHOS::Rosen::DO_ADD_VIRTUAL_SCREEN_BLACK_LIST:
            OHOS::Rosen::DoAddVirtualScreenBlackList();
            break;
        case OHOS::Rosen::DO_REMOVE_VIRTUAL_SCREEN_BLACK_LIST:
            OHOS::Rosen::DoRemoveVirtualScreenBlackList();
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_SCREEN_SECURITY_EXEMPTION_LIST:
            OHOS::Rosen::DoSetVirtualScreenSecurityExemptionList();
            break;
        case OHOS::Rosen::DO_REMOVE_VIRTUAL_SCREEN:
            OHOS::Rosen::DoRemoveVirtualScreen();
            break;
        case OHOS::Rosen::DO_GET_VIRTUAL_SCREEN_RESOLUTION:
            OHOS::Rosen::DoGetVirtualScreenResolution();
            break;
        case OHOS::Rosen::DO_RESIZE_VIRTUAL_SCREEN:
            OHOS::Rosen::DoResizeVirtualScreen();
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_SCREEN_USING_STATUS:
            OHOS::Rosen::DoSetVirtualScreenUsingStatus();
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_SCREEN_REFRESH_RATE:
            OHOS::Rosen::DoSetVirtualScreenRefreshRate();
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_SCREEN_STATUS:
            OHOS::Rosen::DoSetVirtualScreenStatus();
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_SCREEN_TYPE_BLACK_LIST:
            OHOS::Rosen::DoSetVirtualScreenTypeBlackList();
            break;
        default:
            return -1;
    }
    return 0;
}