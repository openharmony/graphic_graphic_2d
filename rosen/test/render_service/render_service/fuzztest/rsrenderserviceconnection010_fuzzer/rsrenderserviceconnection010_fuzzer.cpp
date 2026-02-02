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

#include "rsrenderserviceconnection010_fuzzer.h"

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
const uint8_t DO_GET_DISPLAY_IDENTIFICATION_DATA = 0;
const uint8_t DO_SET_OVERLAY_DISPLAY_MODE = 1;
const uint8_t DO_GET_EHIND_WINDOW_FILTER_ENABLED = 2;
const uint8_t DO_NOTIFY_XCOMPONENT_EXPECTED_FRAME_RATE = 3;
const uint8_t DO_SET_OPTIMIZE_CANVAS_DRITY_PIDLIST = 4;
const uint8_t DO_SET_GPU_CRCDIRTY_ENABLE_PIDLIST = 5;
const uint8_t DO_SET_VIRTUAL_SCREEN_AUTO_ROTATION = 6;
const uint8_t TARGET_SIZE = 7;

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

void DoGetDisplayIdentificationData()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_DISPLAY_IDENTIFICATION_DATA);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;

    ScreenId id = GetData<uint64_t>();
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteUint64(id);
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetOverlayDisplayMode()
{
#ifdef RS_ENABLE_OVERLAY_DISPLAY
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_OVERLAY_DISPLAY_MODE);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;

    int32_t mode = GetData<int32_t>();
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteInt32(mode);
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
#endif
}

bool DoGetBehindWindowFilterEnabled()
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_BEHIND_WINDOW_FILTER_ENABLED);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;
    bool enabled = GetData<bool>();
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteBool(enabled);
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
    return true;
}

bool DoNotifyXComponentExpectedFrameRate()
{
    uint32_t code = static_cast<uint32_t>(
        RSIRenderServiceConnectionInterfaceCode::NOTIFY_XCOMPONENT_EXPECTED_FRAMERATE);

    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    std::string id = GetData<std::string>();
    int32_t expectedFrameRate = GetData<int32_t>();
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteString(id);
    dataParcel.WriteInt32(expectedFrameRate);
    toServiceConnectionStub_->OnRemoteRequest(code, dataParcel, replyParcel, option);
    return true;
}

bool DoSetOptimizeCanvasDirtyPidList()
{
    std::vector<int32_t> pidList;
    uint8_t pidListSize = GetData<uint8_t>();
    for (size_t i = 0; i < pidListSize; i++) {
        pidList.push_back(GetData<int32_t>());
    }

    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return false;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!dataP.WriteInt32Vector(pidList)) {
        return false;
    }

    uint32_t code =
        static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_OPTIMIZE_CANVAS_DIRTY_ENABLED_PIDLIST);
    if (toServiceConnectionStub_ == nullptr) {
        return false;
    }
    toServiceConnectionStub_->OnRemoteRequest(code, dataP, reply, option);
    return true;
}

bool DoSetGpuCrcDirtyEnabledPidList()
{
    std::vector<int32_t> pidList;
    uint8_t pidListSize = GetData<uint8_t>();
    for (size_t i = 0; i < pidListSize; i++) {
        pidList.push_back(GetData<int32_t>());
    }
 
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return false;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!dataP.WriteInt32Vector(pidList)) {
        return false;
    }
 
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_GPU_CRC_DIRTY_ENABLED_PIDLIST);
    if (toServiceConnectionStub_ == nullptr) {
        return false;
    }
    toServiceConnectionStub_->OnRemoteRequest(code, dataP, reply, option);
    return true;
}

bool DoSetVirtualScreenAutoRotation()
{
    uint64_t screenId = GetData<uint64_t>();
    bool isAutoRotation = GetData<bool>();
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return false;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    if (!dataP.WriteUint64(screenId)) {
        return false;
    }
    if (!dataP.WriteBool(isAutoRotation)) {
        return false;
    }

    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_AUTO_ROTATION);
    if (toServiceConnectionStub_ == nullptr) {
        return false;
    }
    toServiceConnectionStub_->OnRemoteRequest(code, dataP, reply, option);
    return true;
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
        case OHOS::Rosen::DO_GET_DISPLAY_IDENTIFICATION_DATA:
            OHOS::Rosen::DoGetDisplayIdentificationData();
            break;
        case OHOS::Rosen::DO_SET_OVERLAY_DISPLAY_MODE:
            OHOS::Rosen::DoSetOverlayDisplayMode();
            break;
        case OHOS::Rosen::DO_GET_EHIND_WINDOW_FILTER_ENABLED:
            OHOS::Rosen::DoGetBehindWindowFilterEnabled();
            break;
        case OHOS::Rosen::DO_NOTIFY_XCOMPONENT_EXPECTED_FRAME_RATE:
            OHOS::Rosen::DoNotifyXComponentExpectedFrameRate();
            break;
        case OHOS::Rosen::DO_SET_OPTIMIZE_CANVAS_DRITY_PIDLIST:
            OHOS::Rosen::DoSetOptimizeCanvasDirtyPidList();
            break;
        case OHOS::Rosen::DO_SET_GPU_CRCDIRTY_ENABLE_PIDLIST:
            OHOS::Rosen::DoSetGpuCrcDirtyEnabledPidList();
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_SCREEN_AUTO_ROTATION:
            OHOS::Rosen::DoSetVirtualScreenAutoRotation();
            break;
        default:
            return -1;
    }
    return 0;
}