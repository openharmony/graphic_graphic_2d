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

#include "rsrenderserviceconnection013_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <thread>
#include <fcntl.h>
#include <fstream>
#include <fuzzer/FuzzedDataProvider.h>
#include <iservice_registry.h>
#include <iterator>
#include <string>
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
constexpr const int WAIT_HANDLER_TIME = 1; // 1s
constexpr const int WAIT_HANDLER_TIME_COUNT = 5;

namespace {
const std::string CONFIG_FILE = "/etc/unirender.config";
const uint8_t DO_CREATE_VIRTUAL_SCREEN = 0;
const uint8_t DO_SET_VIRTUAL_SCREEN_TYPE_BLACK_LIST = 1;
const uint8_t DO_SET_VIRTUAL_SCREEN_SECURITY_EXEMPTION_LIST = 2;

const uint8_t DO_SET_SCREEN_ACTIVE_MODE = 4;
const uint8_t DO_SET_VIRTUAL_SCREEN_RESOLUTION = 5;
const uint8_t DO_GET_SCREEN_ACTIVE_MODE = 6;
const uint8_t DO_GET_SCREEN_SUPPORTED_MODES = 7;
const uint8_t DO_GET_SCREEN_CAPABILITY = 8;
const uint8_t DO_GET_SCREEN_POWER_STATUS = 9;
const uint8_t DO_GET_SCREEN_DATA = 10;
const uint8_t DO_GET_SCREEN_BACK_LIGHT = 11;
const uint8_t DO_SET_SCREEN_BACK_LIGHT = 12;
const uint8_t DO_GET_SCREEN_SUPPORTED_COLOR_GAMUTS = 13;
const uint8_t DO_GET_SCREEN_SUPPORTED_META_DATA_KEYS = 14;
const uint8_t DO_GET_SCREEN_COLOR_GAMUT = 15;
const uint8_t DO_SET_SCREEN_COLOR_GAMUT = 16;
const uint8_t DO_SET_SCREEN_GAMUT_MAP = 17;
const uint8_t DO_GET_SCREEN_GAMUT_MAP = 18;
const uint8_t DO_GET_DISPLAY_IDENTIFICATION_DATA = 19;
const uint8_t DO_RESIZE_VIRTUAL_SCREEN = 20;

constexpr uint8_t DO_SET_ROG_SCREEN_RESOLUTION = 22;

constexpr uint8_t TARGET_SIZE = 24;
} // namespace


void WriteUnirenderConfig(std::string& tag)
{
    std::ofstream file;
    file.open(CONFIG_FILE);
    if (file.is_open()) {
        file << tag << "\r";
        file.close();
    }
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


void DoCreateVirtualScreen(FuzzedDataProvider& fdp)
{
    MessageOption option;
    MessageParcel dataP;
    MessageParcel reply;
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::CREATE_VIRTUAL_SCREEN);
    sptr<IBufferProducer> bufferProducer = IConsumerSurface::Create()->GetProducer();
    if (!bufferProducer) {
        return;
    }

    std::string name = fdp.ConsumeRandomLengthString();
    uint32_t width = fdp.ConsumeIntegral<uint32_t>();
    uint32_t height = fdp.ConsumeIntegral<uint32_t>();
    bool useSurface = fdp.ConsumeBool();
    uint64_t mirrorId = fdp.ConsumeIntegral<uint64_t>();
    int32_t flags = fdp.ConsumeIntegral<int32_t>();
    std::vector<NodeId> whiteList;
    uint8_t listSize = fdp.ConsumeIntegral<uint8_t>();
    for (int i = 0; i < listSize; i++) {
        NodeId nodeId = fdp.ConsumeIntegral<NodeId>();
        whiteList.push_back(nodeId);
    }
    dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataP.WriteString(name);
    dataP.WriteUint32(width);
    dataP.WriteUint32(height);
    dataP.WriteBool(useSurface);
    dataP.WriteRemoteObject(useSurface ? bufferProducer->AsObject() : nullptr);
    dataP.WriteUint64(mirrorId);
    dataP.WriteInt32(flags);
    dataP.WriteUInt64Vector(whiteList);
    g_serviceConnection->OnRemoteRequest(code, dataP, reply, option);
}

void DoSetVirtualScreenTypeBlackList(FuzzedDataProvider& fdp)
{
    uint64_t id = fdp.ConsumeIntegral<uint64_t>();
    uint8_t nodeType = fdp.ConsumeIntegral<uint8_t>();
    std::vector<NodeType> typeBlackListVector;
    uint8_t listSize = fdp.ConsumeIntegral<uint8_t>();
    for (int i = 0; i < listSize; i++) {
        typeBlackListVector.push_back(nodeType);
    }
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    option.SetFlags(MessageOption::TF_ASYNC);
    dataP.WriteUint64(id);
    dataP.WriteUInt8Vector(typeBlackListVector);
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_TYPE_BLACKLIST);
    g_serviceConnection->OnRemoteRequest(code, dataP, reply, option);
}

void DoSetVirtualScreenSecurityExemptionList(FuzzedDataProvider& fdp)
{
    uint64_t id = fdp.ConsumeIntegral<uint64_t>();
    std::vector<uint64_t> secExemptionListVector;
    uint8_t listSize = fdp.ConsumeIntegral<uint8_t>();
    for (int i = 0; i < listSize; i++) {
        uint64_t nodeId = fdp.ConsumeIntegral<uint64_t>();
        secExemptionListVector.push_back(nodeId);
    }
    g_serviceConnection->SetVirtualScreenSecurityExemptionList(id, secExemptionListVector);
}

void DoSetScreenSecurityMask(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    std::shared_ptr<Media::PixelMap> securityMask = nullptr;
    g_serviceConnection->SetScreenSecurityMask(id, securityMask);

    auto screenMgrAgent = g_serviceConnection->screenManagerAgent_;
    g_serviceConnection->screenManagerAgent_ = nullptr;
    g_serviceConnection->SetScreenSecurityMask(id, securityMask);
    g_serviceConnection->screenManagerAgent_ = screenMgrAgent;
}

void DoSetScreenActiveMode(FuzzedDataProvider& fdp)
{
    uint32_t code =
        static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_ACTIVE_MODE);
    MessageOption option;
    MessageParcel dataP;
    MessageParcel reply;
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    uint32_t modeId = fdp.ConsumeIntegral<uint32_t>();
    dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataP.WriteUint64(id);
    dataP.WriteUint32(modeId);
    g_serviceConnection->OnRemoteRequest(code, dataP, reply, option);
}

void DoSetRogScreenResolution(FuzzedDataProvider& fdp)
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_ROG_SCREEN_RESOLUTION);
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    uint64_t id = fdp.ConsumeIntegral<uint64_t>();
    dataP.WriteUint64(id);
    uint32_t width = fdp.ConsumeIntegral<uint32_t>();
    dataP.WriteUint32(width);
    uint32_t height = fdp.ConsumeIntegral<uint32_t>();
    dataP.WriteUint32(height);
    g_serviceConnection->OnRemoteRequest(code, dataP, reply, option);
}

void DoGetRogScreenResolution(FuzzedDataProvider& fdp)
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_ROG_SCREEN_RESOLUTION);
    MessageOption option;
    MessageParcel dataP;
    MessageParcel reply;
    dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    dataP.WriteUint64(id);
    g_serviceConnection->OnRemoteRequest(code, dataP, reply, option);
}

void DoSetVirtualScreenResolution(FuzzedDataProvider& fdp)
{
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    option.SetFlags(MessageOption::TF_SYNC);
    uint64_t id = fdp.ConsumeIntegral<uint64_t>();
    dataP.WriteUint64(id);
    uint32_t width = fdp.ConsumeIntegral<uint32_t>();
    dataP.WriteUint32(width);
    uint32_t height = fdp.ConsumeIntegral<uint32_t>();
    dataP.WriteUint32(height);
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_VIRTUAL_SCREEN_RESOLUTION);
    g_serviceConnection->OnRemoteRequest(code, dataP, reply, option);
}

void DoGetScreenActiveMode(FuzzedDataProvider& fdp)
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_ACTIVE_MODE);
    MessageOption option;
    MessageParcel dataP;
    MessageParcel reply;
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataP.WriteUint64(id);
    g_serviceConnection->OnRemoteRequest(code, dataP, reply, option);
}

void DoGetScreenSupportedModes(FuzzedDataProvider& fdp)
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_MODES);
    MessageOption option;
    MessageParcel dataP;
    MessageParcel reply;
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataP.WriteUint64(id);
    g_serviceConnection->OnRemoteRequest(code, dataP, reply, option);
}

void DoGetScreenCapability(FuzzedDataProvider& fdp)
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_CAPABILITY);
    MessageOption option;
    MessageParcel dataP;
    MessageParcel reply;
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataP.WriteUint64(id);
    g_serviceConnection->OnRemoteRequest(code, dataP, reply, option);
}

void DoGetScreenPowerStatus(FuzzedDataProvider& fdp)
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_POWER_STATUS);
    MessageOption option;
    MessageParcel dataP;
    MessageParcel reply;
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataP.WriteUint64(id);
    g_serviceConnection->OnRemoteRequest(code, dataP, reply, option);
}

void DoGetScreenData(FuzzedDataProvider& fdp)
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_DATA);
    MessageOption option;
    MessageParcel dataP;
    MessageParcel reply;
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataP.WriteUint64(id);
    g_serviceConnection->OnRemoteRequest(code, dataP, reply, option);
}

void DoGetScreenBacklight(FuzzedDataProvider& fdp)
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_BACK_LIGHT);
    MessageOption option;
    MessageParcel dataP;
    MessageParcel reply;
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataP.WriteUint64(id);
    g_serviceConnection->OnRemoteRequest(code, dataP, reply, option);
}

void DoSetScreenBacklight(FuzzedDataProvider& fdp)
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_BACK_LIGHT);
    MessageOption option;
    MessageParcel dataP;
    MessageParcel reply;
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    uint32_t level = fdp.ConsumeIntegral<uint32_t>();
    dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataP.WriteUint64(id);
    dataP.WriteUint32(level);
    g_serviceConnection->OnRemoteRequest(code, dataP, reply, option);
}

void DoGetScreenSupportedColorGamuts(FuzzedDataProvider& fdp)
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_GAMUTS);
    MessageOption option;
    MessageParcel dataP;
    MessageParcel reply;
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataP.WriteUint64(id);
    g_serviceConnection->OnRemoteRequest(code, dataP, reply, option);
}

void DoGetScreenSupportedMetaDataKeys(FuzzedDataProvider& fdp)
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_METADATAKEYS);
    MessageOption option;
    MessageParcel dataP;
    MessageParcel reply;
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataP.WriteUint64(id);
    g_serviceConnection->OnRemoteRequest(code, dataP, reply, option);
}

void DoGetScreenColorGamut(FuzzedDataProvider& fdp)
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_GAMUT);
    MessageOption option;
    MessageParcel dataP;
    MessageParcel reply;
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataP.WriteUint64(id);
    g_serviceConnection->OnRemoteRequest(code, dataP, reply, option);
}

void DoSetScreenColorGamut(FuzzedDataProvider& fdp)
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_GAMUT);
    MessageOption option;
    MessageParcel dataP;
    MessageParcel reply;
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    int32_t modeIdx = fdp.ConsumeIntegral<int32_t>();
    dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataP.WriteUint64(id);
    dataP.WriteInt32(modeIdx);
    g_serviceConnection->OnRemoteRequest(code, dataP, reply, option);
}

void DoSetScreenGamutMap(FuzzedDataProvider& fdp)
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::SET_SCREEN_GAMUT_MAP);
    MessageOption option;
    MessageParcel dataP;
    MessageParcel reply;
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    int32_t mode = fdp.ConsumeIntegral<int32_t>();
    dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataP.WriteUint64(id);
    dataP.WriteInt32(mode);
    g_serviceConnection->OnRemoteRequest(code, dataP, reply, option);
}

void DoGetScreenGamutMap(FuzzedDataProvider& fdp)
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_SCREEN_GAMUT_MAP);
    MessageOption option;
    MessageParcel dataP;
    MessageParcel reply;
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataP.WriteUint64(id);
    g_serviceConnection->OnRemoteRequest(code, dataP, reply, option);
}

void DoGetDisplayIdentificationData(FuzzedDataProvider& fdp)
{
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::GET_DISPLAY_IDENTIFICATION_DATA);
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;

    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    dataP.WriteUint64(id);
    g_serviceConnection->OnRemoteRequest(code, dataP, reply, option);
}

void DoResizeVirtualScreen(FuzzedDataProvider& fdp)
{
    uint64_t id = fdp.ConsumeIntegral<uint64_t>();
    uint32_t width = fdp.ConsumeIntegral<uint32_t>();
    uint32_t height = fdp.ConsumeIntegral<uint32_t>();
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    dataP.WriteInterfaceToken(RSIClientToServiceConnection::GetDescriptor());
    option.SetFlags(MessageOption::TF_SYNC);
    dataP.WriteUint64(id);
    dataP.WriteUint32(width);
    dataP.WriteUint32(height);
    uint32_t code = static_cast<uint32_t>(RSIClientToServiceConnectionInterfaceCode::RESIZE_VIRTUAL_SCREEN);
    g_serviceConnection->OnRemoteRequest(code, dataP, reply, option);
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
    if (data == nullptr) {
        return -1;
    }

    auto fdp = FuzzedDataProvider(data, size);

    /* Run your code on data */
    uint8_t tarPos = fdp.ConsumeIntegral<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_CREATE_VIRTUAL_SCREEN:
            OHOS::Rosen::DoCreateVirtualScreen(fdp);
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_SCREEN_TYPE_BLACK_LIST:
            OHOS::Rosen::DoSetVirtualScreenTypeBlackList(fdp);
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_SCREEN_SECURITY_EXEMPTION_LIST:
            OHOS::Rosen::DoSetVirtualScreenSecurityExemptionList(fdp);
            break;
        case OHOS::Rosen::DO_SET_SCREEN_ACTIVE_MODE:
            OHOS::Rosen::DoSetScreenActiveMode(fdp);
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_SCREEN_RESOLUTION:
            OHOS::Rosen::DoSetVirtualScreenResolution(fdp);
            break;
        case OHOS::Rosen::DO_GET_SCREEN_ACTIVE_MODE:
            OHOS::Rosen::DoGetScreenActiveMode(fdp);
            break;
        case OHOS::Rosen::DO_GET_SCREEN_SUPPORTED_MODES:
            OHOS::Rosen::DoGetScreenSupportedModes(fdp);
            break;
        case OHOS::Rosen::DO_GET_SCREEN_CAPABILITY:
            OHOS::Rosen::DoGetScreenCapability(fdp);
            break;
        case OHOS::Rosen::DO_GET_SCREEN_POWER_STATUS:
            OHOS::Rosen::DoGetScreenPowerStatus(fdp);
            break;
        case OHOS::Rosen::DO_GET_SCREEN_DATA:
            OHOS::Rosen::DoGetScreenData(fdp);
            break;
        case OHOS::Rosen::DO_GET_SCREEN_BACK_LIGHT:
            OHOS::Rosen::DoGetScreenBacklight(fdp);
            break;
        case OHOS::Rosen::DO_SET_SCREEN_BACK_LIGHT:
            OHOS::Rosen::DoSetScreenBacklight(fdp);
            break;
        case OHOS::Rosen::DO_GET_SCREEN_SUPPORTED_COLOR_GAMUTS:
            OHOS::Rosen::DoGetScreenSupportedColorGamuts(fdp);
            break;
        case OHOS::Rosen::DO_GET_SCREEN_SUPPORTED_META_DATA_KEYS:
            OHOS::Rosen::DoGetScreenSupportedMetaDataKeys(fdp);
            break;
        case OHOS::Rosen::DO_GET_SCREEN_COLOR_GAMUT:
            OHOS::Rosen::DoGetScreenColorGamut(fdp);
            break;
        case OHOS::Rosen::DO_SET_SCREEN_COLOR_GAMUT:
            OHOS::Rosen::DoSetScreenColorGamut(fdp);
            break;
        case OHOS::Rosen::DO_SET_SCREEN_GAMUT_MAP:
            OHOS::Rosen::DoSetScreenGamutMap(fdp);
            break;
        case OHOS::Rosen::DO_GET_SCREEN_GAMUT_MAP:
            OHOS::Rosen::DoGetScreenGamutMap(fdp);
            break;
        case OHOS::Rosen::DO_GET_DISPLAY_IDENTIFICATION_DATA:
            OHOS::Rosen::DoGetDisplayIdentificationData(fdp);
            break;
        case OHOS::Rosen::DO_RESIZE_VIRTUAL_SCREEN:
            OHOS::Rosen::DoResizeVirtualScreen(fdp);
            break;
        case OHOS::Rosen::DO_SET_ROG_SCREEN_RESOLUTION:
            OHOS::Rosen::DoSetRogScreenResolution(fdp);
            break;
        default:
            return -1;
    }

    return 0;
}