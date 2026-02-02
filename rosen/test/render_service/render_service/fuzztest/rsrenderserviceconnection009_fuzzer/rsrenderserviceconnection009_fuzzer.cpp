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
const uint8_t DO_SHOW_WATERMARK = 0;
const uint8_t DO_SET_WATERMARK = 1;
const uint8_t DO_SET_SURFACE_WATERMARK = 2;
const uint8_t DO_CLEAR_SURFACE_WATERMARK_FOR_NODES = 3;
const uint8_t DO_CLEAR_SURFACE_WATERMARK = 4;
const uint8_t TARGET_SIZE = 5;

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

void DoShowWatermark()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SHOW_WATERMARK);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;

    bool isShow = GetData<bool>();
    int width = GetData<int32_t>();
    int height = GetData<int32_t>();
    Media::InitializationOptions opts;
    opts.size.width = width;
    opts.size.height = height;
    std::shared_ptr<Media::PixelMap> pixelmap = Media::PixelMap::Create(opts);
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteParcelable(pixelmap.get());
    dataParcel.WriteBool(isShow);
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetWatermark()
{
    std::string name = GetData<std::string>();
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    int width = GetData<int32_t>();
    int height = GetData<int32_t>();
    Media::InitializationOptions opts;
    opts.size.width = width;
    opts.size.height = height;
    std::shared_ptr<Media::PixelMap> pixelmap = Media::PixelMap::Create(opts);
    dataP.WriteString(name);
    dataP.WriteParcelable(pixelmap.get());
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_WATERMARK);
    if (toServiceConnectionStub_ == nullptr) {
        return;
    }
    toServiceConnectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoSetSurfaceWatermark()
{
    MessageParcel dataParcel;
    MessageParcel reply;
    MessageOption option;
    if (!dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    pid_t pid = GetData<int32_t>();
    std::string name = GetData<std::string>();
    bool hasPixelMap = GetData<bool>();
    int width = GetData<int32_t>();
    int height = GetData<int32_t>();
    Media::InitializationOptions opts;
    opts.size.width = width;
    opts.size.height = height;
    std::shared_ptr<Media::PixelMap> pixelmap = Media::PixelMap::Create(opts);
    std::vector<NodeId> nodeList;
    uint8_t listSize = GetData<uint8_t>();
    for (int i = 0; i < listSize; i++) {
        NodeId nodeId = GetData<NodeId>();
        nodeList.push_back(nodeId);
    }
    uint8_t watermarkType = GetData<uint8_t>();
    dataParcel.WriteInt32(pid);
    dataParcel.WriteString(name);
    dataParcel.WriteBool(hasPixelMap);
    dataParcel.WriteParcelable(pixelmap.get());
    dataParcel.WriteUInt64Vector(nodeList);
    dataParcel.WriteUint8(watermarkType);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SURFACE_WATERMARK);
    if (toServiceConnectionStub_ == nullptr) {
        return;
    }
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, reply, option);
}

void DoClearSurfaceWatermarkForNodes()
{
    MessageParcel dataParcel;
    MessageParcel reply;
    MessageOption option;
    if (!dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    pid_t pid = GetData<int32_t>();
    std::string name = GetData<std::string>();
    std::vector<NodeId> nodeList;
    uint8_t listSize = GetData<uint8_t>();
    for (int i = 0; i < listSize; i++) {
        NodeId nodeId = GetData<NodeId>();
        nodeList.push_back(nodeId);
    }
    dataParcel.WriteInt32(pid);
    dataParcel.WriteString(name);
    dataParcel.WriteUInt64Vector(nodeList);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CLEAR_SURFACE_WATERMARK_FOR_NODES);
    if (toServiceConnectionStub_ == nullptr) {
        return;
    }
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, reply, option);
}

void DoClearSurfaceWatermark()
{
    MessageParcel dataParcel;
    MessageParcel reply;
    MessageOption option;
    if (!dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    pid_t pid = GetData<int32_t>();
    std::string name = GetData<std::string>();
    dataParcel.WriteInt32(pid);
    dataParcel.WriteString(name);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::CLEAR_SURFACE_WATERMARK);
    if (toServiceConnectionStub_ == nullptr) {
        return;
    }
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, reply, option);
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
        case OHOS::Rosen::DO_SHOW_WATERMARK:
            OHOS::Rosen::DoShowWatermark();
            break;
        case OHOS::Rosen::DO_SET_WATERMARK:
            OHOS::Rosen::DoSetWatermark();
            break;
        case OHOS::Rosen::DO_SET_SURFACE_WATERMARK:
            OHOS::Rosen::DoSetSurfaceWatermark();
            break;
        case OHOS::Rosen::DO_CLEAR_SURFACE_WATERMARK_FOR_NODES:
            OHOS::Rosen::DoClearSurfaceWatermarkForNodes();
            break;
        case OHOS::Rosen::DO_CLEAR_SURFACE_WATERMARK:
            OHOS::Rosen::DoClearSurfaceWatermark();
            break;
        default:
            return -1;
    }
    return 0;
}