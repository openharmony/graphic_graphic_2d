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
#include "platform/ohos/transaction/zidl/rs_irender_service.h"
#include "render_server/transaction/zidl/rs_client_to_service_connection_stub.h"
#include "transaction/rs_transaction_proxy.h"
#include "transaction/rs_render_to_service_connection.h"
#include "feature/hyper_graphic_manager/hgm_render_context.h"
#include "transaction/rs_client_to_render_connection.h"
#include "transaction/zidl/rs_client_to_render_connection_stub.h"

namespace OHOS {
namespace Rosen {
class MockRenderProcessManager : public RSRenderProcessManager {
public:
    explicit MockRenderProcessManager(RSRenderService& renderService) : RSRenderProcessManager(renderService) {}

    sptr<RSIServiceToRenderConnection> GetServiceToRenderConn(ScreenId screenId) const override { return nullptr; }
    std::vector<sptr<RSIServiceToRenderConnection>> GetServiceToRenderConns() const override { return {}; }
    sptr<RSIConnectToRenderProcess> GetConnectToRenderConnection(ScreenId screenId) const override
    {
        return nullptr;
    }
    sptr<IRemoteObject> CreateRenderToServiceConnection(pid_t callingPid) override { return nullptr; }

    sptr<IRemoteObject> OnScreenConnected(ScreenId id, const std::shared_ptr<HdiOutput>& output,
        const sptr<RSScreenProperty>& property) override
    {
        return nullptr;
    }
    void OnScreenDisconnected(ScreenId id) override {}
    void OnScreenPropertyChanged(ScreenId id, ScreenPropertyType type,
        const sptr<ScreenPropertyBase>& property) override {}
    void OnScreenRefresh(ScreenId id) override {}
    void OnVirtualScreenConnected(ScreenId id, ScreenId associatedScreenId,
        const sptr<RSScreenProperty>& property) override {}
    void OnVirtualScreenDisconnected(ScreenId id) override {}
};

constexpr const int WAIT_HANDLER_TIME = 1; // 1s
constexpr const int WAIT_HANDLER_TIME_COUNT = 5;
constexpr size_t MAX_APS_PARAMS_SIZE = 128;


namespace {
const uint8_t DO_GET_DISPLAY_IDENTIFICATION_DATA = 0;
const uint8_t DO_SET_OVERLAY_DISPLAY_MODE = 1;
const uint8_t DO_GET_EHIND_WINDOW_FILTER_ENABLED = 2;
const uint8_t DO_NOTIFY_XCOMPONENT_EXPECTED_FRAME_RATE = 3;
const uint8_t DO_SET_OPTIMIZE_CANVAS_DRITY_PIDLIST = 4;
const uint8_t DO_SET_GPU_CRCDIRTY_ENABLE_PIDLIST = 5;
const uint8_t DO_SET_VIRTUAL_SCREEN_AUTO_ROTATION = 6;
const uint8_t DO_GET_SCREEN_VCP_FEATURE = 7;
const uint8_t DO_SET_SCREEN_VCP_FEATURE = 8;
const uint8_t DO_SET_APS_CONFIG_PARAMS = 9;
const uint8_t TARGET_SIZE = 10;

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


void DoGetDisplayIdentificationData()
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_DISPLAY_IDENTIFICATION_DATA);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;

    ScreenId id = GetData<uint64_t>();
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteUint64(id);
    g_serviceConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
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
    g_serviceConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
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
    g_serviceConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
    return true;
}

bool DoSetApsConfigParams()
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_APS_CONFIG_PARAMS);
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    MessageOption option;

    uint32_t eventVal = GetData<uint32_t>();
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteUint32(eventVal);
    uint32_t paramsSize = GetData<uint32_t>();
    dataParcel.WriteUint32(paramsSize);
    for (uint32_t i = 0; i < paramsSize && i < MAX_APS_PARAMS_SIZE; ++i) {
        std::string key = GetData<std::string>();
        std::string value = GetData<std::string>();
        dataParcel.WriteString(key);
        dataParcel.WriteString(value);
    }
    g_serviceConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
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
    g_serviceConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
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
    if (g_serviceConnection == nullptr) {
        return false;
    }
    g_serviceConnection->OnRemoteRequest(code, dataP, reply, option);
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
    if (g_serviceConnection == nullptr) {
        return false;
    }
    g_serviceConnection->OnRemoteRequest(code, dataP, reply, option);
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
    if (g_serviceConnection == nullptr) {
        return false;
    }
    g_serviceConnection->OnRemoteRequest(code, dataP, reply, option);
    return true;
}

void DoGetScreenVCPFeature()
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_VCP_FEATURE);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    ScreenId id = GetData<uint64_t>();
    uint8_t vcpCode = GetData<uint8_t>();
    if (!dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return;
    }
    if (!dataParcel.WriteUint64(id)) {
        return;
    }
    if (!dataParcel.WriteUint8(vcpCode)) {
        return;
    }
    if (g_serviceConnection == nullptr) {
        return;
    }
    g_serviceConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetScreenVCPFeature()
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_VCP_FEATURE);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    ScreenId id = GetData<uint64_t>();
    uint8_t vcpCode = GetData<uint8_t>();
    uint16_t currentValue = GetData<uint16_t>();
    if (!dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return;
    }
    if (!dataParcel.WriteUint64(id)) {
        return;
    }
    if (!dataParcel.WriteUint8(vcpCode)) {
        return;
    }
    if (!dataParcel.WriteUint16(currentValue)) {
        return;
    }
    if (g_serviceConnection == nullptr) {
        return;
    }
    g_serviceConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
}
} // namespace Rosen
} // namespace OHOS

/* Fallback cleanup registered via atexit, in case LLVMFuzzerFinalize is not invoked. */
static void FuzzerAtExitCleanup()
{
    using namespace OHOS::Rosen;
    using namespace OHOS::AppExecFwk;
    // Release connections BEFORE RSMainThread Meyers-singleton is destroyed
    // by atexit, otherwise ~RSClientToRenderConnection() -> PostTask() UAF.
    g_renderConnection = nullptr;
    g_serviceConnection = nullptr;

    // Stop runners and wait for worker threads to actually exit.
    // Without this, ~RSMainThread() may free RSContext while T9 is still
    // running CleanResources() inside the event loop.
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

    // Step 1: Create RSRenderService and basic handlers
    OHOS::Rosen::g_renderService = new OHOS::Rosen::RSRenderService();
    OHOS::sptr<OHOS::Rosen::RSRenderService>& renderService = OHOS::Rosen::g_renderService;
    OHOS::Rosen::g_serviceRunner = OHOS::AppExecFwk::EventRunner::Create(true);
    OHOS::Rosen::g_serviceRunner->Run();
    OHOS::Rosen::g_serviceHandler = std::make_shared<OHOS::AppExecFwk::EventHandler>(OHOS::Rosen::g_serviceRunner);
    renderService->handler_ = OHOS::Rosen::g_serviceHandler;

    // Step 2: Initialize vsync and screen managers
    renderService->vsyncManager_ = OHOS::sptr<OHOS::Rosen::RSVsyncManager>::MakeSptr();
    renderService->screenManager_ = OHOS::sptr<OHOS::Rosen::RSScreenManager>::MakeSptr();
    renderService->vsyncManager_->init(renderService->screenManager_);

    // Step 3: Initialize render pipeline with main thread and uni render thread
    // Note: We intentionally skip RSRenderProcessManager::Create here because
    // in single-process mode (the default), RSSingleRenderProcessManager's
    // constructor would create VSyncReceiver + overwrite renderPipeline_ +
    // call RSUniRenderThread::Start(), spawning an uncontrollable runner thread.
    // For the interfaces we fuzz (GetUniRenderEnabled, SetGlobalDarkColorMode),
    // renderProcessManager is not needed.
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

    // Step 5: Initialize composer manager
    renderService->rsRenderComposerManager_ =
        std::make_shared<OHOS::Rosen::RSRenderComposerManager>(renderService->handler_);

    // Step 4: Create agents for service connection
    auto renderServiceAgent = OHOS::sptr<OHOS::Rosen::RSRenderServiceAgent>::MakeSptr(*renderService);
    auto screenManagerAgent = OHOS::sptr<OHOS::Rosen::RSScreenManagerAgent>::MakeSptr(renderService->screenManager_);
    auto vsyncManagerAgent = renderService->vsyncManager_->GetVsyncManagerAgent();

    // Step 5: Create RSClientToServiceConnection
    // Use a mock RenderProcessManager to avoid nullptr crash in fuzzer tests,
    // while skipping the real RSSingleRenderProcessManager which spawns uncontrollable threads.
    auto mockRenderProcessManager = OHOS::sptr<OHOS::Rosen::MockRenderProcessManager>::MakeSptr(*renderService);
    auto renderProcessManagerAgent =
        OHOS::sptr<OHOS::Rosen::RSRenderProcessManagerAgent>::MakeSptr(mockRenderProcessManager);
    OHOS::Rosen::g_serviceConnection = new OHOS::Rosen::RSClientToServiceConnection(
        getpid(), renderServiceAgent, renderProcessManagerAgent,
        screenManagerAgent, token->AsObject(), vsyncManagerAgent);

    // Step 8: Create RSClientToRenderConnection
    auto renderPipelineAgent =
        OHOS::sptr<OHOS::Rosen::RSRenderPipelineAgent>::MakeSptr(renderService->renderPipeline_);
    OHOS::Rosen::g_renderConnection = new OHOS::Rosen::RSClientToRenderConnection(
        getpid(), renderPipelineAgent, token->AsObject());

    // Register atexit cleanup AFTER all Meyers-singletons (RSMainThread/RSUniRenderThread)
    // have been initialized. Meyers singletons register their destructors via atexit
    // on first Instance() call. atexit is LIFO, so FuzzerAtExitCleanup must be
    // registered AFTER them to ensure it runs BEFORE ~RSMainThread().
    std::atexit(FuzzerAtExitCleanup);

    return 0;
}

/* Fuzzer cleanup: stop event runners before process exit to avoid UAF.
 * Pattern borrowed from RSClientToRenderConnectionStubTest::TearDownTestCase
 * and RSClientToServiceConnectionStubTest::TearDownTestCase.
 */
extern "C" int LLVMFuzzerFinalize(void)
{
    using namespace OHOS::Rosen;
    using namespace OHOS::AppExecFwk;

    // 1. Release connections FIRST while runners/handlers are still alive.
    //    ~RSClientToRenderConnection() calls CleanAll() which uses
    //    ScheduleMainThreadTask().wait() -- runners must be running.
    g_renderConnection = nullptr;
    g_serviceConnection = nullptr;

    // 2. Wait for pending handler tasks to finish (borrowed from WaitHandlerTask).
    RSMainThread* mainThread = RSMainThread::Instance();
    RSUniRenderThread* uniRenderThread = &RSUniRenderThread::Instance();
    WaitHandlerTask(mainThread, uniRenderThread);

    // 3. Stop runners.
    if (g_serviceRunner != nullptr) {
        g_serviceRunner->Stop();
    }
    if (g_mainRunner != nullptr) {
        g_mainRunner->Stop();
    }
    if (g_uniRunner != nullptr) {
        g_uniRunner->Stop();
    }

    // 4. Wait for runner worker threads to ACTUALLY exit.
    //    Stop() only sets queue_->Finish(); T9 may still be processing
    //    the current event (e.g. CleanResources). IsRunning() becomes
    //    false only after Run() returns in the worker thread.
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

    // 5. Explicitly reset handler_/runner_ pointers on singletons
    //    (same pattern as TearDownTestCase in the stub tests).
    if (mainThread != nullptr) {
        mainThread->handler_ = nullptr;
        mainThread->receiver_ = nullptr;
    }
    if (uniRenderThread != nullptr) {
        uniRenderThread->handler_ = nullptr;
        uniRenderThread->runner_ = nullptr;
    }

    // 6. Release renderService (and its renderPipeline_) AFTER connections/agents
    //    are dead and runners/handlers are stopped.
    g_renderService = nullptr;

    // 7. Release runners and extra handler references.
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
        case OHOS::Rosen::DO_GET_DISPLAY_IDENTIFICATION_DATA:
            OHOS::Rosen::DoGetDisplayIdentificationData();
            break;
        case OHOS::Rosen::DO_SET_OVERLAY_DISPLAY_MODE:
            OHOS::Rosen::DoSetOverlayDisplayMode();
            break;
        case OHOS::Rosen::DO_GET_EHIND_WINDOW_FILTER_ENABLED:
            OHOS::Rosen::DoGetBehindWindowFilterEnabled();
            break;
        case OHOS::Rosen::DO_SET_APS_CONFIG_PARAMS:
            OHOS::Rosen::DoSetApsConfigParams();
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
        case OHOS::Rosen::DO_GET_SCREEN_VCP_FEATURE:
            OHOS::Rosen::DoGetScreenVCPFeature();
            break;
        case OHOS::Rosen::DO_SET_SCREEN_VCP_FEATURE:
            OHOS::Rosen::DoSetScreenVCPFeature();
            break;
        default:
            return -1;
    }
    return 0;
}