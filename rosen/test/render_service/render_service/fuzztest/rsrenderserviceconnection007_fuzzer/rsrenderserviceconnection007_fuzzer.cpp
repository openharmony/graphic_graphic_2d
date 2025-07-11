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

#include "rsrenderserviceconnection007_fuzzer.h"

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
const uint8_t DO_SET_SCREEN_GAMUT = 0;
const uint8_t DO_SET_SCREEN_GAMUT_MAP = 1;
const uint8_t DO_SET_SCREEN_CORRECTION = 2;
const uint8_t DO_SET_VIRTUAL_MIRROR_SCREEN_CANVAS_ROTATION = 3;
const uint8_t DO_SET_VIRTUAL_MIRROR_SCREEN_SCALE_MODE = 4;
const uint8_t DO_SET_GLOBAL_DARK_COLOR_MODE = 5;
const uint8_t TARGET_SIZE = 6;

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

void DoSetScreenColorGamut()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_GAMUT);
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

void DoSetScreenGamutMap()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_GAMUT_MAP);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    ScreenId id = GetData<uint64_t>();
    int32_t mode = GetData<int32_t>();
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteUint64(id);
    dataParcel.WriteInt32(mode);
    rsConnStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetScreenCorrection()
{
    uint64_t id = GetData<uint64_t>();
    uint32_t screenRotation = GetData<uint32_t>();
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
    if (!dataP.WriteUint32(screenRotation)) {
        return;
    }

    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_CORRECTION);
    if (connectionStub_ == nullptr) {
        return;
    }
    connectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoSetVirtualMirrorScreenCanvasRotation()
{
    uint64_t id = GetData<uint64_t>();
    bool canvasRotation = GetData<bool>();
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
    if (!dataP.WriteBool(canvasRotation)) {
        return;
    }

    uint32_t code = static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_MIRROR_SCREEN_CANVAS_ROTATION);
    if (rsConnStub_ == nullptr) {
        return;
    }
    rsConnStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoSetVirtualMirrorScreenScaleMode()
{
    uint64_t id = GetData<uint64_t>();
    uint32_t scaleMode = GetData<uint32_t>();
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
    if (!dataP.WriteUint32(scaleMode)) {
        return;
    }
    uint32_t code = static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_MIRROR_SCREEN_SCALE_MODE);
    if (connectionStub_ == nullptr) {
        return;
    }
    connectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoSetGlobalDarkColorMode()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_GLOBAL_DARK_COLOR_MODE);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    bool isDark = GetData<bool>();
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteBool(isDark);
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
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
        case OHOS::Rosen::DO_SET_SCREEN_GAMUT:
            OHOS::Rosen::DoSetScreenColorGamut();
            break;
        case OHOS::Rosen::DO_SET_SCREEN_GAMUT_MAP:
            OHOS::Rosen::DoSetScreenGamutMap();
            break;
        case OHOS::Rosen::DO_SET_SCREEN_CORRECTION:
            OHOS::Rosen::DoSetScreenCorrection();
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_MIRROR_SCREEN_CANVAS_ROTATION:
            OHOS::Rosen::DoSetVirtualMirrorScreenCanvasRotation();
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_MIRROR_SCREEN_SCALE_MODE:
            OHOS::Rosen::DoSetVirtualMirrorScreenScaleMode();
            break;
        case OHOS::Rosen::DO_SET_GLOBAL_DARK_COLOR_MODE:
            OHOS::Rosen::DoSetGlobalDarkColorMode();
            break;
        default:
            return -1;
    }
    return 0;
}