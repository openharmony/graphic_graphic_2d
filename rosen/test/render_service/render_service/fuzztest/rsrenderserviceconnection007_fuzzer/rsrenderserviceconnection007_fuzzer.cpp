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

#include "rsrenderserviceconnection007_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <thread>
#include <fcntl.h>
#include <fuzzer/FuzzedDataProvider.h>
#include <iservice_registry.h>
#include <system_ability_definition.h>
#include <unistd.h>
#include <unordered_map>

#include "message_parcel.h"
#include "securec.h"

#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "render_server/transaction/rs_client_to_service_connection.h"
#include "transaction/rs_client_to_render_connection.h"
#include "platform/ohos/transaction/zidl/rs_irender_service.h"
#include "render_server/transaction/zidl/rs_client_to_service_connection_stub.h"
#include "transaction/zidl/rs_client_to_render_connection_stub.h"
#include "transaction/rs_transaction_proxy.h"
#include "ipc_callbacks/screen_switching_notify_callback_stub.h"
#include "transaction/rs_render_to_service_connection.h"
#include "feature/hyper_graphic_manager/hgm_render_context.h"

namespace OHOS {
namespace Rosen {
constexpr const int WAIT_HANDLER_TIME = 1; // 1s
constexpr const int WAIT_HANDLER_TIME_COUNT = 5;

namespace {
const uint8_t DO_SET_SCREEN_GAMUT = 0;
const uint8_t DO_SET_SCREEN_GAMUT_MAP = 1;
const uint8_t DO_SET_SCREEN_CORRECTION = 2;
const uint8_t DO_SET_VIRTUAL_MIRROR_SCREEN_CANVAS_ROTATION = 3;
const uint8_t DO_SET_VIRTUAL_MIRROR_SCREEN_SCALE_MODE = 4;
const uint8_t DO_SET_GLOBAL_DARK_COLOR_MODE = 5;
const uint8_t DO_SET_SCREEN_SWITCHING_NOTIFY_CALLBACK = 6;
const uint8_t DO_ADD_VIRTUAL_SCREEN_WHITE_LIST = 7;
const uint8_t DO_REMOVE_VIRTUAL_SCREEN_WHITE_LIST = 8;
const uint8_t DO_SET_UI_MODE_3D = 9;
const uint8_t DO_SET_LOGICAL_CAMERA_ROTATION_CORRECTION = 10;
const uint8_t TARGET_SIZE = 12;

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

void WaitHandlerTask(RSMainThread* mainThread, RSUniRenderThread* uniRenderThread)
{
    if (mainThread == nullptr || mainThread->handler_ == nullptr ||
        uniRenderThread == nullptr || uniRenderThread->handler_ == nullptr) {
        return;
    }
    auto count = 0;
    auto isMainThreadRunning = !mainThread->handler_->IsIdle();
    auto isUniRenderThreadRunning = !uniRenderThread->handler_->IsIdle();
    while (count < WAIT_HANDLER_TIME_COUNT && (isMainThreadRunning || isUniRenderThreadRunning)) {
        std::this_thread::sleep_for(std::chrono::seconds(WAIT_HANDLER_TIME));
        isMainThreadRunning = !mainThread->handler_->IsIdle();
        isUniRenderThreadRunning = !uniRenderThread->handler_->IsIdle();
        count++;
    }
    if (count >= WAIT_HANDLER_TIME_COUNT) {
        mainThread->handler_->RemoveAllEvents();
        uniRenderThread->handler_->RemoveAllEvents();
    }
}
} // namespace

// Global variables - ORDER MATTERS for C++ reverse destruction.
// We declare runners and handlers FIRST so they are destroyed LAST.
// This ensures connections' destructors (which PostTask to runner threads)
// always find EventHandler/EventRunner alive.
std::shared_ptr<AppExecFwk::EventRunner> g_serviceRunner = nullptr;
std::shared_ptr<AppExecFwk::EventRunner> g_mainRunner = nullptr;
std::shared_ptr<AppExecFwk::EventRunner> g_uniRunner = nullptr;

// Extra references to EventHandlers so they outlive connections.
// Without these, the Meyers-singleton RSMainThread may be atexit-destroyed
// before g_renderConnection, leaving handler_ dangling during ~RSClientToRenderConnection().
std::shared_ptr<AppExecFwk::EventHandler> g_serviceHandler = nullptr;
std::shared_ptr<AppExecFwk::EventHandler> g_mainHandler = nullptr;
std::shared_ptr<AppExecFwk::EventHandler> g_uniHandler = nullptr;

sptr<RSRenderService> g_renderService = nullptr;
sptr<RSClientToServiceConnection> g_serviceConnection = nullptr;
sptr<RSClientToRenderConnection> g_renderConnection = nullptr;


void DoSetScreenColorGamut()
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_GAMUT);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    ScreenId id = GetData<uint64_t>();
    int32_t modeIdx = GetData<int32_t>();
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteUint64(id);
    dataParcel.WriteInt32(modeIdx);
    g_serviceConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetScreenGamutMap()
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_GAMUT_MAP);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    ScreenId id = GetData<uint64_t>();
    int32_t mode = GetData<int32_t>();
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteUint64(id);
    dataParcel.WriteInt32(mode);
    g_serviceConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetUIMode3D()
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_UI_MODE_3D);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    uint32_t mode = GetData<uint32_t>();
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteUint32(mode);
    g_serviceConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetScreenCorrection()
{
    uint64_t id = GetData<uint64_t>();
    uint32_t screenRotation = GetData<uint32_t>();
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
    if (!dataP.WriteUint32(screenRotation)) {
        return;
    }

    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_CORRECTION);
    if (g_serviceConnection == nullptr) {
        return;
    }
    g_serviceConnection->OnRemoteRequest(code, dataP, reply, option);
}

void DoSetVirtualMirrorScreenCanvasRotation()
{
    uint64_t id = GetData<uint64_t>();
    bool canvasRotation = GetData<bool>();
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
    if (!dataP.WriteBool(canvasRotation)) {
        return;
    }

    uint32_t code = static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::SET_VIRTUAL_MIRROR_SCREEN_CANVAS_ROTATION);
    if (g_serviceConnection == nullptr) {
        return;
    }
    g_serviceConnection->OnRemoteRequest(code, dataP, reply, option);
}

void DoSetVirtualMirrorScreenScaleMode()
{
    uint64_t id = GetData<uint64_t>();
    uint32_t scaleMode = GetData<uint32_t>();
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
    if (!dataP.WriteUint32(scaleMode)) {
        return;
    }
    uint32_t code = static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::SET_VIRTUAL_MIRROR_SCREEN_SCALE_MODE);
    if (g_serviceConnection == nullptr) {
        return;
    }
    g_serviceConnection->OnRemoteRequest(code, dataP, reply, option);
}

void DoSetGlobalDarkColorMode()
{
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SET_GLOBAL_DARK_COLOR_MODE);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    bool isDark = GetData<bool>();
    dataParcel.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    dataParcel.WriteBool(isDark);
    g_renderConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
    sleep(1);
}

class CustomTestScreenSwitchingNotifyCallback : public RSScreenSwitchingNotifyCallbackStub {
public:
    explicit CustomTestScreenSwitchingNotifyCallback(const ScreenSwitchingNotifyCallback &callback) : cb_(callback)
    {}
    ~CustomTestScreenSwitchingNotifyCallback() override{};

    void OnScreenSwitchingNotify(bool status) override
    {
        if (cb_ != nullptr) {
            cb_(status);
        }
    }

private:
    ScreenSwitchingNotifyCallback cb_;
};

void DoSetScreenSwitchingNotifyCallback()
{
    uint32_t code =
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_SWITCHING_NOTIFY_CALLBACK);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    bool status = false;
    auto callback = [&status](bool switchingStatus) {
        status = switchingStatus;
    };
    sptr<CustomTestScreenSwitchingNotifyCallback> rsIScreenSwitchingNotifyCallback_ =
        new CustomTestScreenSwitchingNotifyCallback(callback);

    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteRemoteObject(rsIScreenSwitchingNotifyCallback_->AsObject());
    dataParcel.RewindRead(0);
    if (g_serviceConnection == nullptr) {
        return;
    }
    g_serviceConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

bool DoAddVirtualScreenWhiteList()
{
    ScreenId id = GetData<ScreenId>();

    // generate whitelist
    std::vector<NodeId> whiteList;
    uint8_t whiteListSize = GetData<uint8_t>();
    for (uint8_t i = 0; i < whiteListSize; i++) {
        NodeId nodeId = GetData<NodeId>();
        whiteList.push_back(nodeId);
    }

    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return false;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!dataP.WriteUint64(id)) {
        return false;
    }
    if (!dataP.WriteUInt64Vector(whiteList)) {
        return false;
    }

    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::ADD_VIRTUAL_SCREEN_WHITELIST);
    if (g_serviceConnection == nullptr) {
        return false;
    }
    g_serviceConnection->OnRemoteRequest(code, dataP, reply, option);
    return true;
}

bool DoRemoveVirtualScreenWhiteList()
{
    ScreenId id = GetData<ScreenId>();

    // generate whitelist
    std::vector<NodeId> whiteList;
    uint8_t whiteListSize = GetData<uint8_t>();
    for (uint8_t i = 0; i < whiteListSize; i++) {
        NodeId nodeId = GetData<NodeId>();
        whiteList.push_back(nodeId);
    }

    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return false;
    }
    option.SetFlags(MessageOption::TF_ASYNC);
    if (!dataP.WriteUint64(id)) {
        return false;
    }
    if (!dataP.WriteUInt64Vector(whiteList)) {
        return false;
    }

    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::REMOVE_VIRTUAL_SCREEN_WHITELIST);
    if (g_serviceConnection == nullptr) {
        return false;
    }
    g_serviceConnection->OnRemoteRequest(code, dataP, reply, option);
    return true;
}

void DoSetLogicalCameraRotationCorrection()
{
    uint32_t code =
        static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::SET_LOGICAL_CAMERA_ROTATION_CORRECTION);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    ScreenId id = GetData<uint64_t>();
    uint32_t logicalCorrection = GetData<uint32_t>();
    dataParcel.WriteInterfaceToken(RSIClientToRenderConnection::GetDescriptor());
    dataParcel.WriteUint64(id);
    dataParcel.WriteUint32(logicalCorrection);
    g_renderConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
}
} // namespace Rosen
} // namespace OHOS

/* Fallback cleanup registered via atexit, in case LLVMFuzzerFinalize is not invoked. */
static void FuzzerAtExitCleanup()
{
    using namespace OHOS::Rosen;
    using namespace OHOS::AppExecFwk;
    g_renderConnection = nullptr;
    g_serviceConnection = nullptr;
    g_renderService = nullptr;
    if (g_serviceRunner != nullptr) {
        g_serviceRunner->Stop();
    }
    if (g_mainRunner != nullptr) {
        g_mainRunner->Stop();
    }
    if (g_uniRunner != nullptr) {
        g_uniRunner->Stop();
    }
    auto waitRunnerStopped = [](const std::shared_ptr<EventRunner>& runner) {
        if (runner == nullptr) {
            return;
        }
        int count = 0;
        while (runner->IsRunning() && count < 500) { // max 5s
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            count++;
        }
    };
    waitRunnerStopped(g_serviceRunner);
    waitRunnerStopped(g_mainRunner);
    waitRunnerStopped(g_uniRunner);
}

/* Fuzzer environment */
extern "C" int LLVMFuzzerInitialize(int* argc, char*** argv)
{
    (void)argc;
    (void)argv;

    OHOS::sptr<OHOS::Rosen::RSIConnectionToken> token =
        new OHOS::IRemoteStub<OHOS::Rosen::RSIConnectionToken>();

    OHOS::Rosen::g_renderService = new OHOS::Rosen::RSRenderService();
    OHOS::sptr<OHOS::Rosen::RSRenderService>& renderService = OHOS::Rosen::g_renderService;
    OHOS::Rosen::g_serviceRunner = OHOS::AppExecFwk::EventRunner::Create(true);
    OHOS::Rosen::g_serviceRunner->Run();
    OHOS::Rosen::g_serviceHandler = std::make_shared<OHOS::AppExecFwk::EventHandler>(OHOS::Rosen::g_serviceRunner);
    renderService->handler_ = OHOS::Rosen::g_serviceHandler;

    renderService->vsyncManager_ = OHOS::sptr<OHOS::Rosen::RSVsyncManager>::MakeSptr();
    renderService->screenManager_ = OHOS::sptr<OHOS::Rosen::RSScreenManager>::MakeSptr();
    renderService->vsyncManager_->init(renderService->screenManager_);

    // Skip RSRenderProcessManager::Create to avoid uncontrollable runner threads.
    // renderProcessManager is not needed for the interfaces we fuzz.
    renderService->renderPipeline_ = std::make_shared<OHOS::Rosen::RSRenderPipeline>();
    OHOS::Rosen::RSMainThread* mainThread = OHOS::Rosen::RSMainThread::Instance();
    OHOS::Rosen::g_mainRunner = OHOS::AppExecFwk::EventRunner::Create(true);
    OHOS::Rosen::g_mainRunner->Run();
    OHOS::Rosen::g_mainHandler = std::make_shared<OHOS::AppExecFwk::EventHandler>(OHOS::Rosen::g_mainRunner);
    mainThread->handler_ = OHOS::Rosen::g_mainHandler;
    renderService->renderPipeline_->mainThread_ = mainThread;

    OHOS::Rosen::RSUniRenderThread* uniRenderThread = &(OHOS::Rosen::RSUniRenderThread::Instance());
    OHOS::Rosen::g_uniRunner = OHOS::AppExecFwk::EventRunner::Create(true);
    OHOS::Rosen::g_uniRunner->Run();
    OHOS::Rosen::g_uniHandler = std::make_shared<OHOS::AppExecFwk::EventHandler>(OHOS::Rosen::g_uniRunner);
    uniRenderThread->handler_ = OHOS::Rosen::g_uniHandler;
    uniRenderThread->runner_ = OHOS::Rosen::g_uniRunner;
    renderService->renderPipeline_->uniRenderThread_ = uniRenderThread;

    renderService->rsRenderComposerManager_ =
        std::make_shared<OHOS::Rosen::RSRenderComposerManager>(renderService->handler_);

    auto renderServiceAgent = OHOS::sptr<OHOS::Rosen::RSRenderServiceAgent>::MakeSptr(*renderService);
    auto screenManagerAgent = OHOS::sptr<OHOS::Rosen::RSScreenManagerAgent>::MakeSptr(renderService->screenManager_);
    auto vsyncManagerAgent = renderService->vsyncManager_->GetVsyncManagerAgent();

    OHOS::Rosen::g_serviceConnection = new OHOS::Rosen::RSClientToServiceConnection(
        getpid(), renderServiceAgent, nullptr,
        screenManagerAgent, token->AsObject(), vsyncManagerAgent);
    auto renderPipelineAgent =
        OHOS::sptr<OHOS::Rosen::RSRenderPipelineAgent>::MakeSptr(renderService->renderPipeline_);
    OHOS::Rosen::g_renderConnection = new OHOS::Rosen::RSClientToRenderConnection(
        getpid(), renderPipelineAgent, token->AsObject());

    // Register atexit cleanup AFTER Meyers-singletons initialize, so it runs
    // BEFORE their destructors (atexit is LIFO).
    std::atexit(FuzzerAtExitCleanup);

    return 0;
}

extern "C" int LLVMFuzzerFinalize(void)
{
    using namespace OHOS::Rosen;
    using namespace OHOS::AppExecFwk;

    g_renderConnection = nullptr;
    g_serviceConnection = nullptr;

    RSMainThread* mainThread = RSMainThread::Instance();
    RSUniRenderThread* uniRenderThread = &RSUniRenderThread::Instance();
    WaitHandlerTask(mainThread, uniRenderThread);

    if (g_serviceRunner != nullptr) {
        g_serviceRunner->Stop();
    }
    if (g_mainRunner != nullptr) {
        g_mainRunner->Stop();
    }
    if (g_uniRunner != nullptr) {
        g_uniRunner->Stop();
    }

    auto waitRunnerStopped = [](const std::shared_ptr<EventRunner>& runner) {
        if (runner == nullptr) {
            return;
        }
        int count = 0;
        while (runner->IsRunning() && count < 500) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            count++;
        }
    };
    waitRunnerStopped(g_serviceRunner);
    waitRunnerStopped(g_mainRunner);
    waitRunnerStopped(g_uniRunner);

    if (mainThread != nullptr) {
        mainThread->handler_ = nullptr;
        mainThread->receiver_ = nullptr;
    }
    if (uniRenderThread != nullptr) {
        uniRenderThread->handler_ = nullptr;
        uniRenderThread->runner_ = nullptr;
    }

    g_renderService = nullptr;
    g_serviceRunner.reset();
    g_mainRunner.reset();
    g_uniRunner.reset();
    g_serviceHandler.reset();
    g_mainHandler.reset();
    g_uniHandler.reset();

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
        case OHOS::Rosen::DO_SET_SCREEN_SWITCHING_NOTIFY_CALLBACK:
            OHOS::Rosen::DoSetScreenSwitchingNotifyCallback();
            break;
        case OHOS::Rosen::DO_ADD_VIRTUAL_SCREEN_WHITE_LIST:
            OHOS::Rosen::DoAddVirtualScreenWhiteList();
            break;
        case OHOS::Rosen::DO_REMOVE_VIRTUAL_SCREEN_WHITE_LIST:
            OHOS::Rosen::DoRemoveVirtualScreenWhiteList();
            break;
        case OHOS::Rosen::DO_SET_UI_MODE_3D:
            OHOS::Rosen::DoSetUIMode3D();
            break;
        case OHOS::Rosen::DO_SET_LOGICAL_CAMERA_ROTATION_CORRECTION:
            OHOS::Rosen::DoSetLogicalCameraRotationCorrection();
            break;
        default:
            return -1;
    }
    return 0;
}