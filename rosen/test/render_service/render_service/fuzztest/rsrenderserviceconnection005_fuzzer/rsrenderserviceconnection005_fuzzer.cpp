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

#include "rsrenderserviceconnection005_fuzzer.h"

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
#include "transaction/rs_client_to_render_connection.h"
#include "transaction/zidl/rs_client_to_render_connection_stub.h"
#include "render_server/transaction/rs_client_to_service_connection.h"
#include "pipeline/rs_render_node_gc.h"
#include "pipeline/rs_surface_buffer_callback_manager.h"
#include "platform/ohos/transaction/zidl/rs_irender_service.h"
#include "render_server/transaction/zidl/rs_client_to_service_connection_stub.h"
#include "transaction/rs_transaction_proxy.h"
#include "ipc_callbacks/hgm_config_change_callback_stub.h"
#include "ipc_callbacks/screen_change_callback_stub.h"
#include "ipc_callbacks/rs_frame_rate_linker_expected_fps_update_callback_stub.h"
#include "transaction/rs_render_to_service_connection.h"
#include "feature/hyper_graphic_manager/hgm_render_context.h"

namespace OHOS {
namespace Rosen {
auto g_pid = getpid();
constexpr const int WAIT_HANDLER_TIME = 1; // 1s
constexpr const int WAIT_HANDLER_TIME_COUNT = 5;


namespace {
const uint8_t DO_SET_SCREEN_CHANGE_CALLBACK = 0;
const uint8_t DO_SET_SCREEN_ACTIVE_MODE = 1;
const uint8_t DO_SET_SCREEN_REFRESH_RATE = 2;
const uint8_t DO_SET_REFRESH_RATE_MODE = 3;
const uint8_t DO_SYNC_FRAME_RATE_RANGE = 4;
const uint8_t DO_UNREGISTER_FRAME_RATE_LINKER = 5;
const uint8_t DO_GET_SCREEN_CURRENT_REFRESH_RATE = 6;
const uint8_t DO_GET_CURRENT_REFRESH_RATE_MODE = 7;
const uint8_t DO_GET_SCREEN_SUPPORTED_REFRESH_RATES = 8;
const uint8_t DO_GET_SHOW_REFRESH_RATE_ENABLED = 9;
const uint8_t DO_SET_SHOW_REFRESH_RATE_ENABLED = 10;
const uint8_t DO_GET_REALTIME_REFRESH_RATE = 11;
const uint8_t DO_GET_REFRESH_INFO = 12;
const uint8_t DO_REFRESH_RATE_UPDATE_CALLBACK = 13;
const uint8_t DO_REGISTER_FRAME_RATE_LINKER_EXPECTED_FPS_CALLBACK = 14;
const uint8_t DO_REFRESH_RATE_MODE_CHANGE_CALLBACK = 15;
const uint8_t DO_GET_REFRESH_INFO_TO_SP = 16;
const uint8_t TARGET_SIZE = 17;
const uint32_t WAIT_TASK_RUN_TIME_NS = 10000;

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


class CustomTestScreenChangeCallback : public RSScreenChangeCallbackStub {
public:
    CustomTestScreenChangeCallback() = default;
    virtual ~CustomTestScreenChangeCallback() = default;
    void OnScreenChanged(ScreenId id, ScreenEvent event,
        ScreenChangeReason reason, sptr<IRemoteObject> obj = nullptr) override {};
};

void DoSetScreenChangeCallback()
{
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    uint32_t code =
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_CHANGE_CALLBACK);

    sptr<CustomTestScreenChangeCallback> rsIScreenChangeCallback_ = new CustomTestScreenChangeCallback();
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteRemoteObject(rsIScreenChangeCallback_->AsObject());
    dataParcel.RewindRead(0);
    g_serviceConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetScreenActiveMode()
{
    uint32_t code =
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_ACTIVE_MODE);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    ScreenId id = GetData<uint64_t>();
    uint32_t modeId = GetData<uint32_t>();
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteUint64(id);
    dataParcel.WriteUint32(modeId);
    g_serviceConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetScreenRefreshRate()
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_REFRESH_RATE);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    uint64_t id = GetData<uint64_t>();
    int32_t sceneId = GetData<int32_t>();
    int32_t rate = GetData<int32_t>();
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteUint64(id);
    dataParcel.WriteInt32(sceneId);
    dataParcel.WriteInt32(rate);
    g_serviceConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetRefreshRateMode()
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_REFRESH_RATE_MODE);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    int32_t mode = GetData<int32_t>();
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteInt32(mode);
    g_serviceConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void CreateVSyncConnection(uint64_t id)
{
    uint32_t code =
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::CREATE_VSYNC_CONNECTION);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    sptr<VSyncIConnectionToken> vsyncIConnectionToken_ = new IRemoteStub<VSyncIConnectionToken>();
    NodeId windowNodeID = GetData<NodeId>();
    std::string name = GetData<std::string>();
    dataParcel.WriteString(name);
    dataParcel.WriteRemoteObject(vsyncIConnectionToken_->AsObject());
    dataParcel.WriteUint64(id);
    dataParcel.WriteUint64(windowNodeID);
    dataParcel.WriteBool(false);
    g_serviceConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void UnregisterFrameRateLinker(uint64_t id)
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::UNREGISTER_FRAME_RATE_LINKER);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteUint64(id);
    g_serviceConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSyncFrameRateRange()
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SYNC_FRAME_RATE_RANGE);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    uint32_t linkId = GetData<uint32_t>();
    uint64_t id = (static_cast<NodeId>(OHOS::Rosen::g_pid) << 32) + linkId;
    uint32_t min = GetData<uint32_t>();
    uint32_t max = GetData<uint32_t>();
    uint32_t preferred = GetData<uint32_t>();
    uint32_t type = GetData<uint32_t>();
    uint32_t componentScene = GetData<uint32_t>();
    int32_t animatorExpectedFrameRate = GetData<int32_t>();
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteUint64(id);
    dataParcel.WriteUint32(min);
    dataParcel.WriteUint32(max);
    dataParcel.WriteUint32(preferred);
    dataParcel.WriteUint32(type);
    dataParcel.WriteUint32(componentScene);
    dataParcel.WriteInt32(animatorExpectedFrameRate);

    g_serviceConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);

    usleep(OHOS::Rosen::WAIT_TASK_RUN_TIME_NS);
    CreateVSyncConnection(id);

    usleep(OHOS::Rosen::WAIT_TASK_RUN_TIME_NS);
    MessageOption option2;
    MessageParcel dataParcel2;
    MessageParcel replyParcel2;
    dataParcel2.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel2.WriteUint64(id);
    dataParcel2.WriteUint32(min);
    dataParcel2.WriteUint32(max);
    dataParcel2.WriteUint32(preferred);
    dataParcel2.WriteUint32(OHOS::Rosen::NATIVE_VSYNC_FRAME_RATE_TYPE);
    dataParcel2.WriteUint32(componentScene);
    dataParcel2.WriteInt32(animatorExpectedFrameRate);

    g_serviceConnection->OnRemoteRequest(code, dataParcel2, replyParcel2, option2);

    usleep(OHOS::Rosen::WAIT_TASK_RUN_TIME_NS);
    UnregisterFrameRateLinker(id);
}

void DoUnregisterFrameRateLinker()
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::UNREGISTER_FRAME_RATE_LINKER);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    uint32_t linkId = GetData<uint32_t>();
    uint64_t id = (static_cast<NodeId>(OHOS::Rosen::g_pid) << 32) + linkId;
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteUint64(id);
    g_serviceConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoGetScreenCurrentRefreshRate()
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_CURRENT_REFRESH_RATE);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    uint64_t id = GetData<uint64_t>();
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteUint64(id);
    g_serviceConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoGetCurrentRefreshRateMode()
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_CURRENT_REFRESH_RATE_MODE);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    g_serviceConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoGetScreenSupportedRefreshRates()
{
    uint32_t code =
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_REFRESH_RATES);

    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    ScreenId id = GetData<uint64_t>();
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteUint64(id);
    g_serviceConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoGetShowRefreshRateEnabled()
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_SHOW_REFRESH_RATE_ENABLED);

    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    g_serviceConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoSetShowRefreshRateEnabled()
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SHOW_REFRESH_RATE_ENABLED);

    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    bool enabled = GetData<bool>();
    int32_t type = GetData<int32_t>();
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteBool(enabled);
    dataParcel.WriteInt32(type);
    g_serviceConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoGetRealtimeRefreshRate()
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_REALTIME_REFRESH_RATE);

    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    ScreenId id = GetData<uint64_t>();
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteUint64(id);
    g_serviceConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoGetRefreshInfoFuzzer()
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_REFRESH_INFO);

    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    int32_t pid = GetData<int32_t>();
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteInt32(pid);
    g_serviceConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void CreateSurfaceNode()
{
    uint32_t code = static_cast<uint32_t>(RSIClientToRenderConnectionInterfaceCode::CREATE_NODE_AND_SURFACE);

    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    dataParcel.WriteUint64(GetData<uint64_t>());
    dataParcel.WriteString(GetData<std::string>());
    dataParcel.WriteUint8(static_cast<uint8_t>(RSSurfaceNodeType::SELF_DRAWING_NODE));
    dataParcel.WriteBool(GetData<bool>());
    dataParcel.WriteBool(false);
    dataParcel.WriteUint8(GetData<uint8_t>());
    dataParcel.WriteBool(GetData<bool>());

    g_renderConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoGetRefreshInfoWithoutSurfaceName()
{
    DoGetRefreshInfoFuzzer();
}

void DoGetRefreshInfoWithSurfaceName()
{
    CreateSurfaceNode();
    DoGetRefreshInfoFuzzer();
}

void DoGetRefreshInfoWithRenderDisable()
{
    auto originRenderType = RSUniRenderJudgement::uniRenderEnabledType_;
    RSUniRenderJudgement::uniRenderEnabledType_ = UniRenderEnabledType::UNI_RENDER_DISABLED;
    CreateSurfaceNode();
    DoGetRefreshInfoFuzzer();
    RSUniRenderJudgement::uniRenderEnabledType_ = originRenderType;
}

class CustomTestHgmConfigChangeCallback : public RSHgmConfigChangeCallbackStub {
public:
    explicit CustomTestHgmConfigChangeCallback(const HgmConfigChangeCallback &callback) : cb_(callback)
    {}
    ~CustomTestHgmConfigChangeCallback() override{};

    void OnHgmConfigChanged(std::shared_ptr<RSHgmConfigData> configData) override
    {
        if (cb_ != nullptr) {
            cb_(configData);
        }
    }
    void OnHgmRefreshRateModeChanged(int32_t refreshRateMode) override
    {}
    void OnHgmRefreshRateUpdate(int32_t refreshRate) override
    {}

private:
    HgmConfigChangeCallback cb_;
};

void DoRegisterHgmRefreshRateUpdateCallback()
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::REFRESH_RATE_UPDATE_CALLBACK);

    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    bool readRemoteObject = GetData<bool>();

    std::shared_ptr<RSHgmConfigData> configData = std::make_shared<RSHgmConfigData>();
    auto callback = [&configData](std::shared_ptr<RSHgmConfigData> data) { configData = data; };
    sptr<CustomTestHgmConfigChangeCallback> rsIHgmConfigChangeCallback =
        new CustomTestHgmConfigChangeCallback(callback);

    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteBool(readRemoteObject);
    dataParcel.WriteRemoteObject(rsIHgmConfigChangeCallback->AsObject());
    g_serviceConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

class CustomTestFrameRateLinkerExpectedFpsUpdateCallback : public RSFrameRateLinkerExpectedFpsUpdateCallbackStub {
public:
    explicit CustomTestFrameRateLinkerExpectedFpsUpdateCallback(
        const FrameRateLinkerExpectedFpsUpdateCallback &callback)
        : cb_(callback)
    {}
    ~CustomTestFrameRateLinkerExpectedFpsUpdateCallback() override{};

    void OnFrameRateLinkerExpectedFpsUpdate(
        int32_t dstPid, const std::string &xcomponentId, int32_t expectedFps) override
    {
        if (cb_ != nullptr) {
            cb_(dstPid, xcomponentId, expectedFps);
        }
    }

private:
    FrameRateLinkerExpectedFpsUpdateCallback cb_;
};

void DoRegisterFrameRateLinkerExpectedFpsUpdateCallback()
{
    uint32_t code = static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::REGISTER_FRAME_RATE_LINKER_EXPECTED_FPS_CALLBACK);

    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    bool readRemoteObject = GetData<bool>();

    int32_t dstPid = GetData<int32_t>();
    std::string xcomponentId = GetData<std::string>();
    int32_t expectedFps = GetData<int32_t>();
    auto callback = [&dstPid, &xcomponentId, &expectedFps](
                        int32_t dstPid_, const std::string& xcomponentId_, int32_t expectedFps_) {
        dstPid = dstPid_;
        xcomponentId = xcomponentId_;
        expectedFps = expectedFps_;
    };
    sptr<CustomTestFrameRateLinkerExpectedFpsUpdateCallback> fpsUpdateCallback =
        new CustomTestFrameRateLinkerExpectedFpsUpdateCallback(callback);

    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteInt32(dstPid);
    dataParcel.WriteBool(readRemoteObject);
    dataParcel.WriteRemoteObject(fpsUpdateCallback->AsObject());
    g_serviceConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoRegisterHgmRefreshRateModeChangeCallback()
{
    uint32_t code = static_cast<uint32_t>(
        RSIClientToServiceConnectionInterfaceCode::REFRESH_RATE_MODE_CHANGE_CALLBACK);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    std::shared_ptr<RSHgmConfigData> configData = std::make_shared<RSHgmConfigData>();
    auto callback = [&configData](std::shared_ptr<RSHgmConfigData> data) { configData = data; };
    sptr<CustomTestHgmConfigChangeCallback> rsIHgmConfigChangeCallback =
        new CustomTestHgmConfigChangeCallback(callback);

    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteRemoteObject(rsIHgmConfigChangeCallback->AsObject());
    g_serviceConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoGetRefreshInfoToSP()
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_REFRESH_INFO_TO_SP);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;

    uint64_t nodeId = GetData<uint64_t>();
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteUint64(nodeId);
    g_serviceConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
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
        case OHOS::Rosen::DO_SET_SCREEN_CHANGE_CALLBACK:
            OHOS::Rosen::DoSetScreenChangeCallback();
            break;
        case OHOS::Rosen::DO_SET_SCREEN_ACTIVE_MODE:
            OHOS::Rosen::DoSetScreenActiveMode();
            break;
        case OHOS::Rosen::DO_SET_SCREEN_REFRESH_RATE:
            OHOS::Rosen::DoSetScreenRefreshRate();
            break;
        case OHOS::Rosen::DO_SET_REFRESH_RATE_MODE:
            OHOS::Rosen::DoSetRefreshRateMode();
            break;
        case OHOS::Rosen::DO_SYNC_FRAME_RATE_RANGE:
            OHOS::Rosen::DoSyncFrameRateRange();
            break;
        case OHOS::Rosen::DO_UNREGISTER_FRAME_RATE_LINKER:
            OHOS::Rosen::DoUnregisterFrameRateLinker();
            break;
        case OHOS::Rosen::DO_GET_SCREEN_CURRENT_REFRESH_RATE:
            OHOS::Rosen::DoGetScreenCurrentRefreshRate();
            break;
        case OHOS::Rosen::DO_GET_CURRENT_REFRESH_RATE_MODE:
            OHOS::Rosen::DoGetCurrentRefreshRateMode();
            break;
        case OHOS::Rosen::DO_GET_SCREEN_SUPPORTED_REFRESH_RATES:
            OHOS::Rosen::DoGetScreenSupportedRefreshRates();
            break;
        case OHOS::Rosen::DO_GET_SHOW_REFRESH_RATE_ENABLED:
            OHOS::Rosen::DoGetShowRefreshRateEnabled();
            break;
        case OHOS::Rosen::DO_SET_SHOW_REFRESH_RATE_ENABLED:
            OHOS::Rosen::DoSetShowRefreshRateEnabled();
            break;
        case OHOS::Rosen::DO_GET_REALTIME_REFRESH_RATE:
            OHOS::Rosen::DoGetRealtimeRefreshRate();
            break;
        case OHOS::Rosen::DO_GET_REFRESH_INFO:
            OHOS::Rosen::DoGetRefreshInfoWithoutSurfaceName();
            OHOS::Rosen::DoGetRefreshInfoWithSurfaceName();
            OHOS::Rosen::DoGetRefreshInfoWithRenderDisable();
            break;
        case OHOS::Rosen::DO_REFRESH_RATE_UPDATE_CALLBACK:
            OHOS::Rosen::DoRegisterHgmRefreshRateUpdateCallback();
            break;
        case OHOS::Rosen::DO_REGISTER_FRAME_RATE_LINKER_EXPECTED_FPS_CALLBACK:
            OHOS::Rosen::DoRegisterFrameRateLinkerExpectedFpsUpdateCallback();
            break;
        case OHOS::Rosen::DO_REFRESH_RATE_MODE_CHANGE_CALLBACK:
            OHOS::Rosen::DoRegisterHgmRefreshRateModeChangeCallback();
            break;
        case OHOS::Rosen::DO_GET_REFRESH_INFO_TO_SP:
            OHOS::Rosen::DoGetRefreshInfoToSP();
            break;
        default:
            return -1;
    }
    usleep(OHOS::Rosen::WAIT_TASK_RUN_TIME_NS);
    return 0;
}