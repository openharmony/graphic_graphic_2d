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

#include "rsrenderserviceconnection004_fuzzer.h"

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

namespace {
const uint8_t DO_SET_POINTER_COLOR_INVERSION_CONFIG = 1;
const uint8_t DO_SET_POINTER_COLOR_INVERSION_ENABLED = 2;
const uint8_t DO_REGISTER_POINTER_LUMINANCE_CALLBACK = 3;
const uint8_t DO_UNREGISTER_POINTER_LUMINANCE_CALLBACK = 4;
const uint8_t TARGET_SIZE = 4;

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

void DoSetPointerColorInversionConfig()
{
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    MessageParcel dataParcel;
    MessageParcel reply;
    MessageOption option;
    if (!dataParcel.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return;
    }
    float darkBuffer = GetData<float>();
    float brightBuffer = GetData<float>();
    int64_t interval = GetData<int64_t>();
    int32_t rangeSize = GetData<int64_t>();
    dataParcel.WriteFloat(darkBuffer);
    dataParcel.WriteFloat(brightBuffer);
    dataParcel.WriteInt64(interval);
    dataParcel.WriteInt32(rangeSize);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_POINTER_COLOR_INVERSION_CONFIG);
    connectionStub_->OnRemoteRequest(code, dataParcel, reply, option);
#endif
}

void DoSetPointerColorInversionEnabled()
{
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    MessageParcel dataParcel;
    MessageParcel reply;
    MessageOption option;
    if (!dataParcel.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return;
    }
    bool enable = GetData<bool>();
    dataParcel.WriteBool(enable);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_POINTER_COLOR_INVERSION_ENABLED);
    connectionStub_->OnRemoteRequest(code, dataParcel, reply, option);
#endif
}

void DoRegisterPointerLuminanceChangeCallback()
{
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_POINTER_LUMINANCE_CALLBACK);

    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    sptr<RSIPointerLuminanceChangeCallback> rsIPointerLuminanceChangeCallback_ =
        iface_cast<RSIPointerLuminanceChangeCallback>(remoteObject);
    if (!dataParcel.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return;
    }
    dataParcel.WriteRemoteObject(rsIPointerLuminanceChangeCallback_->AsObject());
    dataParcel.RewindRead(0);
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
#endif
}

void DoUnRegisterPointerLuminanceChangeCallback()
{
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REGISTER_POINTER_LUMINANCE_CALLBACK);

    if (!dataParcel.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return;
    }
    connectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
#endif
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
        case OHOS::Rosen::DO_SET_POINTER_COLOR_INVERSION_CONFIG:
            OHOS::Rosen::DoSetPointerColorInversionConfig();
            break;
        case OHOS::Rosen::DO_SET_POINTER_COLOR_INVERSION_ENABLED:
            OHOS::Rosen::DoSetPointerColorInversionEnabled();
            break;
        case OHOS::Rosen::DO_REGISTER_POINTER_LUMINANCE_CALLBACK:
            OHOS::Rosen::DoRegisterPointerLuminanceChangeCallback();
            break;
        case OHOS::Rosen::DO_UNREGISTER_POINTER_LUMINANCE_CALLBACK:
            OHOS::Rosen::DoUnRegisterPointerLuminanceChangeCallback();
            break;
        default:
            return -1;
    }
    return 0;
}