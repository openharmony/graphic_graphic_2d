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
#include "pipeline/main_thread/rs_render_service_connection.h"
#include "platform/ohos/rs_irender_service.h"
#include "transaction/rs_render_service_connection_stub.h"
#include "transaction/rs_transaction_proxy.h"

namespace OHOS {
namespace Rosen {
DECLARE_INTERFACE_DESCRIPTOR(u"ohos.rosen.RenderServiceConnection");
auto g_pid = getpid();
auto screenManagerPtr_ = impl::RSScreenManager::GetInstance();
auto mainThread_ = RSMainThread::Instance();
sptr<RSIConnectionToken> token_ = new IRemoteStub<RSIConnectionToken>();

DVSyncFeatureParam dvsyncParam;
auto generator = CreateVSyncGenerator();
auto appVSyncController = new VSyncController(generator, 0);
sptr<VSyncDistributor> appVSyncDistributor_ = new VSyncDistributor(appVSyncController, "app", dvsyncParam);
sptr<RSRenderServiceConnectionStub> connectionStub_ = new RSRenderServiceConnection(
    g_pid, nullptr, mainThread_, screenManagerPtr_, token_->AsObject(), appVSyncDistributor_);
sptr<RSRenderServiceConnectionStub> rsConnStub_ = nullptr;
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

sptr<RSIRenderServiceConnection> CONN = nullptr;
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
    rsConnStub_ = new RSRenderServiceConnection(g_pid, nullptr, nullptr, nullptr, token_->AsObject(), nullptr);
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

void DoCreateVirtualScreen()
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_VIRTUAL_SCREEN);
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    sptr<IBufferProducer> bufferProducer_ = iface_cast<IBufferProducer>(remoteObject);

    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    bool useSurface = GetData<bool>();
    uint64_t mirrorId = GetData<uint64_t>();
    int32_t flags = GetData<int32_t>();
    std::vector<NodeId> whiteList;
    uint8_t listSize = GetData<uint8_t>();
    for (int i = 0; i < listSize; i++) {
        NodeId nodeId = GetData<NodeId>();
        whiteList.push_back(nodeId);
    }
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteString("name");
    dataParcel.WriteUint32(width);
    dataParcel.WriteUint32(height);
    dataParcel.WriteBool(useSurface);
    dataParcel.WriteRemoteObject(bufferProducer_->AsObject());
    dataParcel.WriteUint64(mirrorId);
    dataParcel.WriteInt32(flags);
    dataParcel.WriteUInt64Vector(whiteList);
    dataParcel.RewindRead(0);
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetVirtualScreenResolution()
{
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
    rsConnStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoSetVirtualScreenSurface()
{
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
    connectionStub_->OnRemoteRequest(code, dataP, reply, option);
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
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
    connectionStub_->OnRemoteRequest(code, dataP, reply, option);
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
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
    connectionStub_->OnRemoteRequest(code, dataP, reply, option);
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
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
    connectionStub_->OnRemoteRequest(code, dataP, reply, option);
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
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
    connectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoRemoveVirtualScreen()
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REMOVE_VIRTUAL_SCREEN);
    ScreenId id = GetData<uint64_t>();
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteUint64(id);
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoGetVirtualScreenResolution()
{
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint64_t id = GetData<uint64_t>();
    if (!dataP.WriteUint64(id)) {
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_VIRTUAL_SCREEN_RESOLUTION);
    connectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoResizeVirtualScreen()
{
    uint64_t id = GetData<uint64_t>();
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
    rsConnStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoSetVirtualScreenUsingStatus()
{
    bool isVirtualScreenUsingStatus = GetData<bool>();
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return;
    }
    if (!dataP.WriteBool(isVirtualScreenUsingStatus)) {
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_USING_STATUS);
    connectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoSetVirtualScreenRefreshRate()
{
    uint64_t id = GetData<uint64_t>();
    uint32_t maxRefreshRate = GetData<uint32_t>();
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
    connectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoSetVirtualScreenStatus()
{
    uint64_t id = GetData<uint64_t>();
    uint64_t screenStatus = GetData<uint64_t>();
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
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
    if (connectionStub_ == nullptr) {
        return;
    }
    connectionStub_->OnRemoteRequest(code, dataP, reply, option);
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
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!dataP.WriteUint64(id)) {
        return;
    }
    if (!dataP.WriteUInt8Vector(typeBlackListVector)) {
        return;
    }
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_BLACKLIST);
    connectionStub_->OnRemoteRequest(code, dataP, reply, option);
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer envirement */
extern "C" int LLVMFuzzerInitialize(const uint8_t* data, size_t size)
{
    auto newPid = getpid();
    auto mainThread = OHOS::Rosen::RSMainThread::Instance();
    auto screenManagerPtr = OHOS::Rosen::impl::RSScreenManager::GetInstance();
    OHOS::Rosen::CONN = new OHOS::Rosen::RSRenderServiceConnection(
        newPid,
        nullptr,
        mainThread,
        screenManagerPtr,
        nullptr,
        nullptr
    );
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