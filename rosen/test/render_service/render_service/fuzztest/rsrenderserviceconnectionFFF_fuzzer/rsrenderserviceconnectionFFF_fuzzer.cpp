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

#include "rsrenderserviceconnectionFFF_fuzzer.h"

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
namespace {
const uint8_t DO_NOTIFY_LIGHT_FACTOR_STATUS = 0;
const uint8_t DO_NOTIFY_PACKAGE_EVENT = 1;
const uint8_t DO_NOTIFY_REFRESH_RATE_EVENT = 2;
const uint8_t DO_NOTIFY_DYNAMIC_MODE_EVENT = 3;
const uint8_t DO_NOTIFY_SOFT_VSYNC_EVENT = 4;
const uint8_t DO_NOTIFY_APP_STRATEGY_CONFIG_CHANGE_EVENT = 5;
const uint8_t DO_NOTIFY_HGMCONFIG_EVENT = 6;
const uint8_t DO_NOTIFY_SCREEN_SWITCHED = 7;
const uint8_t DO_NOTIFY_SOFT_VSYNC_RATE_DISCOUNT_EVENT = 8;
const uint8_t TARGET_SIZE = 9;

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

void DoNotifyLightFactorStatus()
{
    int32_t lightFactorStatus = GetData<int32_t>();
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    dataP.WriteInt32(lightFactorStatus);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_LIGHT_FACTOR_STATUS);
    if (connectionStub_ == nullptr) {
        return;
    }
    connectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoNotifyPackageEvent()
{
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t listSize = GetData<uint32_t>();
    auto package = GetData<std::string>();
    dataP.WriteUint32(listSize);
    dataP.WriteString(package);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_PACKAGE_EVENT);
    if (connectionStub_ == nullptr) {
        return;
    }
    connectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoNotifyRefreshRateEvent()
{
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    std::string eventName = GetData<std::string>();
    bool eventStatus = GetData<bool>();
    uint32_t minRefreshRate = GetData<uint32_t>();
    uint32_t maxRefreshRate = GetData<uint32_t>();
    std::string description = GetData<std::string>();
    dataP.WriteString(eventName);
    dataP.WriteBool(eventStatus);
    dataP.WriteUint32(minRefreshRate);
    dataP.WriteUint32(maxRefreshRate);
    dataP.WriteString(description);

    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_REFRESH_RATE_EVENT);
    if (connectionStub_ == nullptr) {
        return;
    }
    connectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoNotifyDynamicModeEvent()
{
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    bool enableDynamicMode = GetData<bool>();
    dataP.WriteBool(enableDynamicMode);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_DYNAMIC_MODE_EVENT);
    if (connectionStub_ == nullptr) {
        return;
    }
    connectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoNotifySoftVsyncEvent()
{
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);

    uint32_t pid = GetData<uint32_t>();
    uint32_t rateDiscount = GetData<uint32_t>();
    dataP.WriteUint32(pid);
    dataP.WriteUint32(rateDiscount);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_SOFT_VSYNC_EVENT);
    if (connectionStub_ == nullptr) {
        return;
    }
    connectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoNotifyAppStrategyConfigChangeEvent()
{
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    auto pkgName = GetData<std::string>();
    uint32_t listSize = GetData<uint32_t>();
    auto configKey = GetData<std::string>();
    auto configValue = GetData<std::string>();
    dataP.WriteString(pkgName);
    dataP.WriteUint32(listSize);
    dataP.WriteString(configKey);
    dataP.WriteString(configValue);

    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_PACKAGE_EVENT);
    if (connectionStub_ == nullptr) {
        return;
    }
    connectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoNotifyHgmConfigEvent()
{
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    std::string eventName = GetData<std::string>();
    bool state = GetData<bool>();
    dataP.WriteString(eventName);
    dataP.WriteBool(state);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_PACKAGE_EVENT);
    if (connectionStub_ == nullptr) {
        return;
    }
    connectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoNotifyScreenSwitched()
{
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_SCREEN_SWITCHED);
    if (connectionStub_ == nullptr) {
        return;
    }
    connectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoNotifySoftVsyncRateDiscountEvent()
{
    uint32_t code =
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::NOTIFY_SOFT_VSYNC_RATE_DISCOUNT_EVENT);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;

    uint32_t pid = GetData<uint32_t>();
    std::string name = GetData<std::string>();
    uint32_t rateDiscount = GetData<uint32_t>();
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteUint32(pid);
    dataParcel.WriteString(name);
    dataParcel.WriteUint32(rateDiscount);
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
        case OHOS::Rosen::DO_NOTIFY_LIGHT_FACTOR_STATUS:
            OHOS::Rosen::DoNotifyLightFactorStatus();
            break;
        case OHOS::Rosen::DO_NOTIFY_PACKAGE_EVENT:
            OHOS::Rosen::DoNotifyPackageEvent();
            break;
        case OHOS::Rosen::DO_NOTIFY_REFRESH_RATE_EVENT:
            OHOS::Rosen::DoNotifyRefreshRateEvent();
            break;
        case OHOS::Rosen::DO_NOTIFY_DYNAMIC_MODE_EVENT:
            OHOS::Rosen::DoNotifyDynamicModeEvent();
            break;
        case OHOS::Rosen::DO_NOTIFY_SOFT_VSYNC_EVENT:
            OHOS::Rosen::DoNotifySoftVsyncEvent();
            break;
        case OHOS::Rosen::DO_NOTIFY_APP_STRATEGY_CONFIG_CHANGE_EVENT:
            OHOS::Rosen::DoNotifyAppStrategyConfigChangeEvent();
            break;
        case OHOS::Rosen::DO_NOTIFY_HGMCONFIG_EVENT:
            OHOS::Rosen::DoNotifyHgmConfigEvent();
            break;
        case OHOS::Rosen::DO_NOTIFY_SCREEN_SWITCHED:
            OHOS::Rosen::DoNotifyScreenSwitched();
            break;
        case OHOS::Rosen::DO_NOTIFY_SOFT_VSYNC_RATE_DISCOUNT_EVENT:
            OHOS::Rosen::DoNotifySoftVsyncRateDiscountEvent();
            break;
        default:
            return -1;
    }
    return 0;
}