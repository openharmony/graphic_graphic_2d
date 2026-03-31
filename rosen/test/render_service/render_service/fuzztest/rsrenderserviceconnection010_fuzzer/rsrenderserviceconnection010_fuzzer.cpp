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
#include "platform/ohos/transaction/zidl/rs_irender_service.h"
#include "render_server/transaction/zidl/rs_client_to_service_connection_stub.h"
#include "transaction/rs_transaction_proxy.h"

namespace OHOS {
namespace Rosen {
auto g_pid = getpid();
sptr<OHOS::Rosen::RSScreenManager> screenManagerPtr_ = OHOS::sptr<OHOS::Rosen::RSScreenManager>::MakeSptr();
auto mainThread_ = RSMainThread::Instance();

sptr<RSClientToServiceConnectionStub> toServiceConnectionStub_ = nullptr;
sptr<OHOS::Rosen::RSRenderService> renderService_ = nullptr;

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
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_DISPLAY_IDENTIFICATION_DATA);
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
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_OVERLAY_DISPLAY_MODE);
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
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_BEHIND_WINDOW_FILTER_ENABLED);
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
        RSIClientToServiceConnectionInterfaceCode::NOTIFY_XCOMPONENT_EXPECTED_FRAMERATE);

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
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_OPTIMIZE_CANVAS_DIRTY_ENABLED_PIDLIST);
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
 
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_GPU_CRC_DIRTY_ENABLED_PIDLIST);
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

    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_AUTO_ROTATION);
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

    OHOS::Rosen::toServiceConnectionStub_ = new OHOS::Rosen::RSClientToServiceConnection(
        OHOS::Rosen::g_pid, renderServiceAgent_, renderProcessManagerAgent_,
        screenManagerAgent_, token_->AsObject(), vsyncManager->GetVsyncManagerAgent());

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