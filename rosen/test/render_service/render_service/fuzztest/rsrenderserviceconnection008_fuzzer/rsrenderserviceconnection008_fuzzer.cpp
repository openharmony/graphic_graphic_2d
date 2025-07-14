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

#include "rsrenderserviceconnection008_fuzzer.h"

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
const uint8_t DO_CREATE_PIXEL_MAP_FROM_SURFACE = 0;
const uint8_t DO_GET_SCREEN_HDR_CAPABILITY = 1;
const uint8_t DO_SET_PIXEL_FORMAT = 2;
const uint8_t DO_GET_PIXEL_FORMAT = 3;
const uint8_t DO_GET_SCREEN_SUPPORTED_HDR_FORMATS = 4;
const uint8_t DO_GET_SCREEN_HDR_FORMAT = 5;
const uint8_t DO_SET_SCREEN_HDR_FORMAT = 6;
const uint8_t DO_GET_SCREEN_SUPPORTED_COLORSPACES = 7;
const uint8_t DO_GET_SCREEN_COLORSPACE = 8;
const uint8_t DO_SET_SCREEN_COLORSPACE = 9;
const uint8_t DO_GET_SCREEN_TYPE = 10;
const uint8_t DO_SET_SCREEN_SKIP_FRAME_INTERVAL = 11;
const uint8_t DO_GET_BITMAP = 12;
const uint8_t DO_GET_PIXELMAP = 13;
const uint8_t TARGET_SIZE = 14;
const uint8_t DO_GET_SCREEN_HDR_STATUS = 15;

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

void DoCreatePixelMapFromSurface()
{
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("FuzzTest");
    sptr<IBufferProducer> bp = cSurface->GetProducer();
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(bp);
    if (pSurface == nullptr) {
        return;
    }

    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return;
    }
    if (!dataP.WriteRemoteObject(pSurface->GetProducer()->AsObject())) {
        return;
    }
    if (!dataP.WriteInt32(GetData<int32_t>()) || !dataP.WriteInt32(GetData<int32_t>()) ||
        !dataP.WriteInt32(GetData<int32_t>()) || !dataP.WriteInt32(GetData<int32_t>())) {
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CREATE_PIXEL_MAP_FROM_SURFACE);
    if (connectionStub_ == nullptr) {
        return;
    }
    connectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoGetScreenHDRCapability()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_HDR_CAPABILITY);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    ScreenId id = GetData<uint64_t>();
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteUint64(id);
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetPixelFormat()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_PIXEL_FORMAT);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    ScreenId id = GetData<uint64_t>();
    int32_t pixel = GetData<int32_t>();
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteUint64(id);
    dataParcel.WriteInt32(pixel);
    rsConnStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoGetPixelFormat()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_PIXEL_FORMAT);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    ScreenId id = GetData<uint64_t>();
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteUint64(id);
    rsConnStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoGetScreenSupportedHDRFormats()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_HDR_FORMATS);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    ScreenId id = GetData<uint64_t>();
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteUint64(id);
    rsConnStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoGetScreenHDRFormat()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_HDR_FORMAT);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    ScreenId id = GetData<uint64_t>();
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteUint64(id);
    rsConnStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetScreenHDRFormat()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_HDR_FORMAT);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    ScreenId id = GetData<uint64_t>();
    int32_t modeIdx = GetData<int32_t>();
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteUint64(id);
    dataParcel.WriteInt32(modeIdx);
    rsConnStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoGetScreenSupportedColorSpaces()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_COLORSPACES);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    ScreenId id = GetData<uint64_t>();
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteUint64(id);
    rsConnStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoGetScreenColorSpace()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_COLORSPACE);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    ScreenId id = GetData<uint64_t>();
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteUint64(id);
    rsConnStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetScreenColorSpace()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_COLORSPACE);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    ScreenId id = GetData<uint64_t>();
    int32_t color = GetData<int32_t>();
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteUint64(id);
    dataParcel.WriteInt32(color);
    rsConnStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoGetScreenType()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_TYPE);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    ScreenId id = GetData<uint64_t>();
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteUint64(id);
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetScreenSkipFrameInterval()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_SKIP_FRAME_INTERVAL);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    ScreenId id = GetData<uint64_t>();
    uint32_t skipFrameInterval = GetData<uint32_t>();
    int32_t statusCode = GetData<int32_t>();
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteUint64(id);
    dataParcel.WriteUint32(skipFrameInterval);
    dataParcel.WriteInt32(statusCode);
    rsConnStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoGetBitmap()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_BITMAP);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;

    NodeId id = static_cast<NodeId>(g_pid) << 32;
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteUint64(id);
    rsConnStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoGetPixelmap()
{
    pid_t  newPid = getpid();
    NodeId nodeId = static_cast<NodeId>(newPid) << 32;
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return;
    }
    dataP.WriteUint64(nodeId);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_PIXELMAP);
    if (rsConnStub_ == nullptr) {
        return;
    }
    rsConnStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoGetScreenHDRStatus()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_HDR_STATUS);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    ScreenId id = GetData<uint64_t>();
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteUint64(id);
    rsConnStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
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
        case OHOS::Rosen::DO_CREATE_PIXEL_MAP_FROM_SURFACE:
            OHOS::Rosen::DoCreatePixelMapFromSurface();
            break;
        case OHOS::Rosen::DO_GET_SCREEN_HDR_CAPABILITY:
            OHOS::Rosen::DoGetScreenHDRCapability();
            break;
        case OHOS::Rosen::DO_SET_PIXEL_FORMAT:
            OHOS::Rosen::DoSetPixelFormat();
            break;
        case OHOS::Rosen::DO_GET_PIXEL_FORMAT:
            OHOS::Rosen::DoGetPixelFormat();
            break;
        case OHOS::Rosen::DO_GET_SCREEN_SUPPORTED_HDR_FORMATS:
            OHOS::Rosen::DoGetScreenSupportedHDRFormats();
            break;
        case OHOS::Rosen::DO_GET_SCREEN_HDR_FORMAT:
            OHOS::Rosen::DoGetScreenHDRFormat();
            break;
        case OHOS::Rosen::DO_SET_SCREEN_HDR_FORMAT:
            OHOS::Rosen::DoSetScreenHDRFormat();
            break;
        case OHOS::Rosen::DO_GET_SCREEN_SUPPORTED_COLORSPACES:
            OHOS::Rosen::DoGetScreenSupportedColorSpaces();
            break;
        case OHOS::Rosen::DO_GET_SCREEN_COLORSPACE:
            OHOS::Rosen::DoGetScreenColorSpace();
            break;
        case OHOS::Rosen::DO_SET_SCREEN_COLORSPACE:
            OHOS::Rosen::DoSetScreenColorSpace();
            break;
        case OHOS::Rosen::DO_GET_SCREEN_TYPE:
            OHOS::Rosen::DoGetScreenType();
            break;
        case OHOS::Rosen::DO_SET_SCREEN_SKIP_FRAME_INTERVAL:
            OHOS::Rosen::DoSetScreenSkipFrameInterval();
            break;
        case OHOS::Rosen::DO_GET_BITMAP:
            OHOS::Rosen::DoGetBitmap();
            break;
        case OHOS::Rosen::DO_GET_PIXELMAP:
            OHOS::Rosen::DoGetPixelmap();
            break;
        case OHOS::Rosen::DO_GET_SCREEN_HDR_STATUS:
            OHOS::Rosen::DoGetScreenHDRStatus();
            break;
        default:
            return -1;
    }
    return 0;
}