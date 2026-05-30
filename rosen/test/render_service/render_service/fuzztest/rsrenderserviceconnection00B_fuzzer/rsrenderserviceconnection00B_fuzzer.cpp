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

#include "rsrenderserviceconnection00B_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <unistd.h>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <thread>
#include <fcntl.h>
#include <unordered_map>
#include <fuzzer/FuzzedDataProvider.h>

#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/render_thread/rs_uni_render_thread.h"
#include "render_server/transaction/rs_client_to_service_connection.h"
#include "transaction/rs_render_to_service_connection.h"
#include "platform/ohos/transaction/zidl/rs_irender_service.h"
#include "render_server/transaction/zidl/rs_client_to_service_connection_stub.h"
#include "transaction/rs_transaction_proxy.h"
#include "message_parcel.h"
#include "securec.h"
#include <iservice_registry.h>
#include <system_ability_definition.h>
#include "feature/hyper_graphic_manager/hgm_render_context.h"
#include "engine/rs_uni_render_engine.h"
#include "pipeline/render_thread/rs_render_engine.h"
#include "transaction/rs_client_to_render_connection.h"
#include "transaction/zidl/rs_client_to_render_connection_stub.h"

namespace OHOS {
namespace Rosen {
constexpr const int WAIT_HANDLER_TIME = 1; // 1s
constexpr const int WAIT_HANDLER_TIME_COUNT = 5;

namespace {
const uint8_t DO_REPORT_JANK_STATS = 0;
const uint8_t DO_REPORT_EVENT_RESPONSE = 1;
const uint8_t DO_REPORT_EVENT_COMPLETE = 2;
const uint8_t DO_REPORT_EVENT_JANK_FRAME = 3;
const uint8_t DO_REPORT_RS_SCENE_JANK_START = 4;
const uint8_t DO_REPORT_RS_SCENE_JANK_END = 5;
const uint8_t DO_REPORT_EVENT_GAMESTATE = 6;
const uint8_t DO_AVCODEC_VIDEO_START = 7;
const uint8_t DO_AVCODEC_VIDEO_STOP = 8;
const uint8_t DO_AVCODEC_VIDEO_GET = 9;
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

bool WriteDataBaseRs(DataBaseRs info, MessageParcel& data)
{
    info.appPid = GetData<int32_t>();
    info.eventType = GetData<int32_t>();
    info.versionCode = GetData<int32_t>();
    info.uniqueId = GetData<int64_t>();
    info.inputTime = GetData<int64_t>();
    info.beginVsyncTime = GetData<int64_t>();
    info.endVsyncTime = GetData<int64_t>();
    info.isDisplayAnimator = GetData<bool>();
    info.sceneId = GetData<std::string>();
    info.versionName = GetData<std::string>();
    info.bundleName = GetData<std::string>();
    info.processName = GetData<std::string>();
    info.abilityName = GetData<std::string>();
    info.pageUrl = GetData<std::string>();
    info.sourceType = GetData<std::string>();
    info.note = GetData<std::string>();
    if (!data.WriteInt32(info.appPid) || !data.WriteInt32(info.eventType) ||
        !data.WriteInt32(info.versionCode) || !data.WriteInt64(info.uniqueId) ||
        !data.WriteInt64(info.inputTime) || !data.WriteInt64(info.beginVsyncTime) ||
        !data.WriteInt64(info.endVsyncTime) || !data.WriteBool(info.isDisplayAnimator) ||
        !data.WriteString(info.sceneId) || !data.WriteString(info.versionName) ||
        !data.WriteString(info.bundleName) || !data.WriteString(info.processName) ||
        !data.WriteString(info.abilityName) ||!data.WriteString(info.pageUrl) ||
        !data.WriteString(info.sourceType) || !data.WriteString(info.note)) {
        return false;
    }
    return true;
}

bool WriteAppInfo(AppInfo info, MessageParcel& data)
{
    info.startTime = GetData<int64_t>();
    info.endTime = GetData<int64_t>();
    info.pid = GetData<int32_t>();
    info.versionName = GetData<std::string>();
    info.versionCode = GetData<int32_t>();
    info.bundleName = GetData<std::string>();
    info.processName = GetData<std::string>();
    if (!data.WriteInt64(info.startTime)) {
        return false;
    }
    if (!data.WriteInt64(info.endTime)) {
        return false;
    }
    if (!data.WriteInt32(info.pid)) {
        return false;
    }
    if (!data.WriteString(info.versionName)) {
        return false;
    }
    if (!data.WriteInt32(info.versionCode)) {
        return false;
    }
    if (!data.WriteString(info.bundleName)) {
        return false;
    }
    if (!data.WriteString(info.processName)) {
        return false;
    }
    return true;
}

bool WriteGameStateDataRs(GameStateData info, MessageParcel& data)
{
    info.pid = GetData<int32_t>();
    info.uid = GetData<int32_t>();
    info.state = GetData<int32_t>();
    info.renderTid = GetData<int32_t>();
    info.bundleName = GetData<std::string>();
    if (!data.WriteInt32(info.pid) || !data.WriteInt32(info.uid) ||
        !data.WriteInt32(info.state) || !data.WriteInt32(info.renderTid) ||
        !data.WriteString(info.bundleName)) {
        return false;
    }
    return true;
}

void DoReportJankStats()
{
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::REPORT_JANK_STATS);
 
    g_serviceConnection->OnRemoteRequest(code, dataP, reply, option);
}

void DoReportEventResponse()
{
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::REPORT_EVENT_RESPONSE);
    DataBaseRs info;
    WriteDataBaseRs(info, dataP);
    g_serviceConnection->OnRemoteRequest(code, dataP, reply, option);
}

void DoReportEventComplete()
{
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::REPORT_EVENT_COMPLETE);
    DataBaseRs info;
    WriteDataBaseRs(info, dataP);
    g_serviceConnection->OnRemoteRequest(code, dataP, reply, option);
}

void DoReportEventJankFrame()
{
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::REPORT_EVENT_JANK_FRAME);
    DataBaseRs info;
    WriteDataBaseRs(info, dataP);
    g_serviceConnection->OnRemoteRequest(code, dataP, reply, option);
}

void DoReportRsSceneJankStart()
{
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::REPORT_RS_SCENE_JANK_START);
    AppInfo info;
    WriteAppInfo(info, dataP);
    g_serviceConnection->OnRemoteRequest(code, dataP, reply, option);
}

void DoReportRsSceneJankEnd()
{
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::REPORT_RS_SCENE_JANK_END);
    AppInfo info;
    WriteAppInfo(info, dataP);
    g_serviceConnection->OnRemoteRequest(code, dataP, reply, option);
}

void DoReportGameStateData()
{
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::REPORT_EVENT_GAMESTATE);
    GameStateData info;
    WriteGameStateDataRs(info, dataP);
    g_serviceConnection->OnRemoteRequest(code, dataP, reply, option);
}

void DoAvcodecVideoStart()
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::AVCODEC_VIDEO_START);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    std::vector<uint64_t> uniqueIdList;
    std::vector<std::string> surfaceNameList;
    // MAX_MONITOR_VIDEO_LENGTH = 1024
    uint8_t monitorListSize = GetData<int8_t>();
    for (size_t i = 0; i < monitorListSize; i++) {
        uniqueIdList.push_back(GetData<uint64_t>());
        surfaceNameList.push_back(GetData<std::string>());
    }
    uint32_t fps = GetData<uint32_t>();
    uint64_t reportTime = GetData<uint64_t>();
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteUInt64Vector(uniqueIdList);
    dataParcel.WriteStringVector(surfaceNameList);
    dataParcel.WriteUint32(fps);
    dataParcel.WriteUint64(reportTime);
    g_serviceConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoAvcodecVideoStop()
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::AVCODEC_VIDEO_STOP);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    std::vector<uint64_t> uniqueIdList;
    std::vector<std::string> surfaceNameList;
    // MAX_MONITOR_VIDEO_LENGTH = 1024
    uint8_t listSize = GetData<int8_t>();
    for (uint8_t i = 0; i < listSize; i++) {
        uniqueIdList.push_back(GetData<uint64_t>());
        surfaceNameList.push_back(GetData<std::string>());
    }
    uint32_t fps = GetData<uint32_t>();
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteUInt64Vector(uniqueIdList);
    dataParcel.WriteStringVector(surfaceNameList);
    dataParcel.WriteUint32(fps);
    g_serviceConnection->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

void DoAvcodecVideoGet()
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::AVCODEC_VIDEO_GET);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    uint64_t uniqueId = GetData<uint64_t>();
    dataParcel.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataParcel.WriteUint64(uniqueId);
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
        case OHOS::Rosen::DO_REPORT_JANK_STATS:
            OHOS::Rosen::DoReportJankStats();
            break;
        case OHOS::Rosen::DO_REPORT_EVENT_RESPONSE:
            OHOS::Rosen::DoReportEventResponse();
            break;
        case OHOS::Rosen::DO_REPORT_EVENT_COMPLETE:
            OHOS::Rosen::DoReportEventComplete();
            break;
        case OHOS::Rosen::DO_REPORT_EVENT_JANK_FRAME:
            OHOS::Rosen::DoReportEventJankFrame();
            break;
        case OHOS::Rosen::DO_REPORT_RS_SCENE_JANK_START:
            OHOS::Rosen::DoReportRsSceneJankStart();
            break;
        case OHOS::Rosen::DO_REPORT_RS_SCENE_JANK_END:
            OHOS::Rosen::DoReportRsSceneJankEnd();
            break;
        case OHOS::Rosen::DO_REPORT_EVENT_GAMESTATE:
            OHOS::Rosen::DoReportGameStateData();
            break;
        case OHOS::Rosen::DO_AVCODEC_VIDEO_START:
            OHOS::Rosen::DoAvcodecVideoStart();
            break;
        case OHOS::Rosen::DO_AVCODEC_VIDEO_STOP:
            OHOS::Rosen::DoAvcodecVideoStop();
            break;
        case OHOS::Rosen::DO_AVCODEC_VIDEO_GET:
            OHOS::Rosen::DoAvcodecVideoGet();
            break;
        default:
            return -1;
    }
    return 0;
}