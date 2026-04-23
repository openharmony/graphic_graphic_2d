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
namespace {
const std::string CONFIG_FILE = "/etc/unirender.config";
const uint8_t DO_CREATE_VIRTUAL_SCREEN = 0;
const uint8_t DO_SET_VIRTUAL_SCREEN_TYPE_BLACK_LIST = 1;
const uint8_t DO_SET_VIRTUAL_SCREEN_SECURITY_EXEMPTION_LIST = 2;
const uint8_t DO_SET_SCREEN_ACTIVE_MODE = 3;
const uint8_t DO_SET_VIRTUAL_SCREEN_RESOLUTION = 4;
const uint8_t DO_GET_SCREEN_ACTIVE_MODE = 5;
const uint8_t DO_GET_SCREEN_SUPPORTED_MODES = 6;
const uint8_t DO_GET_SCREEN_CAPABILITY = 7;
const uint8_t DO_GET_SCREEN_POWER_STATUS = 8;
const uint8_t DO_GET_SCREEN_DATA = 9;
const uint8_t DO_GET_SCREEN_BACK_LIGHT = 10;
const uint8_t DO_SET_SCREEN_BACK_LIGHT = 11;
const uint8_t DO_GET_SCREEN_SUPPORTED_COLOR_GAMUTS = 12;
const uint8_t DO_GET_SCREEN_SUPPORTED_META_DATA_KEYS = 13;
const uint8_t DO_GET_SCREEN_COLOR_GAMUT = 14;
const uint8_t DO_SET_SCREEN_COLOR_GAMUT = 15;
const uint8_t DO_SET_SCREEN_GAMUT_MAP = 16;
const uint8_t DO_GET_SCREEN_GAMUT_MAP = 17;
const uint8_t DO_GET_DISPLAY_IDENTIFICATION_DATA = 18;
const uint8_t DO_RESIZE_VIRTUAL_SCREEN = 19;
constexpr uint8_t DO_SET_ROG_SCREEN_RESOLUTION = 20;
constexpr uint8_t DO_SET_MIRROR_SCREEN_VISIBLE_RECT = 21;
constexpr uint8_t TARGET_SIZE = 22;
} // namespace

RSMainThread* g_mainThread = nullptr;
sptr<RSIConnectionToken> g_token = nullptr;
sptr<RSClientToServiceConnectionStub> g_toServiceConnectionStub = nullptr;
sptr<RSClientToServiceConnection> g_toServiceConnection = nullptr;
sptr<OHOS::Rosen::RSRenderService> renderService_ = nullptr;
sptr<RSScreenManager> screenManagerPtr_ = sptr<RSScreenManager>::MakeSptr();
std::string g_originTag = "";
auto g_pid = getpid();
/* Call once in the Fuzzer Initialize function */
int Initialize()
{
    OHOS::Rosen::g_pid = getpid();
    OHOS::sptr<OHOS::Rosen::RSIConnectionToken> token_ = new OHOS::IRemoteStub<OHOS::Rosen::RSIConnectionToken>();
    OHOS::Rosen::DVSyncFeatureParam dvsyncParam;
    auto generator = OHOS::Rosen::CreateVSyncGenerator();
    auto appVSyncController = new OHOS::Rosen::VSyncController(generator, 0);
    OHOS::sptr<OHOS::Rosen::VSyncDistributor> appVSyncDistributor_ =
        new OHOS::Rosen::VSyncDistributor(appVSyncController, "app", dvsyncParam);

    OHOS::Rosen::renderService_ = new OHOS::Rosen::RSRenderService();

    OHOS::Rosen::RSUniRenderThread::Instance().InitGrContext();
    auto runner = OHOS::AppExecFwk::EventRunner::Create(true);
    runner->Run();
    OHOS::Rosen::renderService_->handler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner);

    OHOS::Rosen::renderService_->vsyncManager_ = OHOS::sptr<OHOS::Rosen::RSVsyncManager>::MakeSptr();
    OHOS::Rosen::renderService_->screenManager_ = OHOS::sptr<OHOS::Rosen::RSScreenManager>::MakeSptr();
    OHOS::Rosen::renderService_->screenManager_->Init(OHOS::Rosen::renderService_->handler_);
    OHOS::Rosen::renderService_->vsyncManager_->init(OHOS::Rosen::renderService_->screenManager_);

    OHOS::Rosen::renderService_->renderProcessManager_ =
        OHOS::Rosen::RSRenderProcessManager::Create(*OHOS::Rosen::renderService_);

    auto renderServiceAgent_ = OHOS::sptr<OHOS::Rosen::RSRenderServiceAgent>::MakeSptr(*OHOS::Rosen::renderService_);
    OHOS::sptr<OHOS::Rosen::RSRenderProcessManagerAgent> renderProcessManagerAgent_ =
        OHOS::sptr<OHOS::Rosen::RSRenderProcessManagerAgent>::MakeSptr(
            OHOS::Rosen::renderService_->renderProcessManager_);

    OHOS::sptr<OHOS::Rosen::RSScreenManagerAgent> screenManagerAgent_ =
        new OHOS::Rosen::RSScreenManagerAgent(OHOS::Rosen::screenManagerPtr_);
    OHOS::Rosen::renderService_->rsRenderComposerManager_ = std::make_shared<OHOS::Rosen::RSRenderComposerManager>(
        OHOS::Rosen::renderService_->handler_);

    OHOS::Rosen::g_toServiceConnectionStub = new OHOS::Rosen::RSClientToServiceConnection(
        OHOS::Rosen::g_pid, renderServiceAgent_, renderProcessManagerAgent_,
        screenManagerAgent_, token_->AsObject(), OHOS::Rosen::renderService_->vsyncManager_->GetVsyncManagerAgent());

    OHOS::sptr<OHOS::Rosen::RSRenderPipelineAgent> renderPipelineAgent_ =
        OHOS::sptr<OHOS::Rosen::RSRenderPipelineAgent>::MakeSptr(OHOS::Rosen::renderService_->renderPipeline_);

    OHOS::sptr<OHOS::Rosen::RSRenderToServiceConnection> g_rsConn =
        OHOS::sptr<OHOS::Rosen::RSRenderToServiceConnection>::MakeSptr(
            renderServiceAgent_, renderProcessManagerAgent_, screenManagerAgent_);
    OHOS::Rosen::RSMainThread::Instance()->hgmRenderContext_ =
        std::make_shared<OHOS::Rosen::HgmRenderContext>(g_rsConn);

    OHOS::Rosen::RSMainThread::Instance()->receiver_->connection_ = nullptr;
    OHOS::Rosen::RSMainThread::Instance()->receiver_ = nullptr;
    OHOS::Rosen::RSMainThread::Instance()->mainLoop_ = []() {};
#ifdef RS_ENABLE_VK
    RsVulkanContext::GetSingleton().InitVulkanContextForUniRender("");
#endif
    // RSHardwareThread::Instance().Start();
    return 0;
}

void WriteUnirenderConfig(std::string& tag)
{
    std::ofstream file;
    file.open(CONFIG_FILE);
    if (file.is_open()) {
        file << tag << "\r";
        file.close();
    }
}

std::string ReadUnirenderConfig()
{
    std::ifstream file(CONFIG_FILE);
    if (file.is_open()) {
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
        return content;
    }
    return "";
}

void SetUp(FuzzedDataProvider& fdp)
{
    g_originTag = ReadUnirenderConfig();
    bool enableForAll = fdp.ConsumeBool();
    std::string tag = enableForAll ? "ENABLED_FOR_ALL" : "DISABLED";
    WriteUnirenderConfig(tag);
    RSUniRenderJudgement::InitUniRenderConfig();
}

void TearDown()
{
    WriteUnirenderConfig(g_originTag);
}

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
    g_toServiceConnectionStub->OnRemoteRequest(code, dataP, reply, option);
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
    g_toServiceConnectionStub->OnRemoteRequest(code, dataP, reply, option);
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
    g_toServiceConnection->SetVirtualScreenSecurityExemptionList(id, secExemptionListVector);
}

void DoSetScreenSecurityMask(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    std::shared_ptr<Media::PixelMap> securityMask = nullptr;
    g_toServiceConnection->SetScreenSecurityMask(id, securityMask);

    auto screenMgrAgent = g_toServiceConnection->screenManagerAgent_;
    g_toServiceConnection->screenManagerAgent_ = nullptr;
    g_toServiceConnection->SetScreenSecurityMask(id, securityMask);
    g_toServiceConnection->screenManagerAgent_ = screenMgrAgent;
}

void DoSetMirrorScreenVisibleRect(FuzzedDataProvider& fdp)
{
    uint64_t screenId = fdp.ConsumeIntegral<uint64_t>();
    int32_t x = fdp.ConsumeIntegral<int32_t>();
    int32_t y = fdp.ConsumeIntegral<int32_t>();
    int32_t w = fdp.ConsumeIntegral<int32_t>();
    int32_t h = fdp.ConsumeIntegral<int32_t>();
    auto mainScreenRect = Rect {
        .x = x,
        .y = y,
        .w = w,
        .h = h
    };
    bool supportRotation = fdp.ConsumeBool();
    g_toServiceConnection->SetMirrorScreenVisibleRect(screenId, mainScreenRect, supportRotation);

    auto screenMgrAgent = g_toServiceConnection->screenManagerAgent_;
    g_toServiceConnection->screenManagerAgent_ = nullptr;
    g_toServiceConnection->SetMirrorScreenVisibleRect(screenId, mainScreenRect, supportRotation);
    g_toServiceConnection->screenManagerAgent_ = screenMgrAgent;
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
    g_toServiceConnectionStub->OnRemoteRequest(code, dataP, reply, option);
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
    g_toServiceConnectionStub->OnRemoteRequest(code, dataP, reply, option);
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
    g_toServiceConnectionStub->OnRemoteRequest(code, dataP, reply, option);
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
    g_toServiceConnectionStub->OnRemoteRequest(code, dataP, reply, option);
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
    g_toServiceConnectionStub->OnRemoteRequest(code, dataP, reply, option);
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
    g_toServiceConnectionStub->OnRemoteRequest(code, dataP, reply, option);
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
    g_toServiceConnectionStub->OnRemoteRequest(code, dataP, reply, option);
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
    g_toServiceConnectionStub->OnRemoteRequest(code, dataP, reply, option);
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
    g_toServiceConnectionStub->OnRemoteRequest(code, dataP, reply, option);
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
    g_toServiceConnectionStub->OnRemoteRequest(code, dataP, reply, option);
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
    g_toServiceConnectionStub->OnRemoteRequest(code, dataP, reply, option);
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
    g_toServiceConnectionStub->OnRemoteRequest(code, dataP, reply, option);
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
    g_toServiceConnectionStub->OnRemoteRequest(code, dataP, reply, option);
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
    g_toServiceConnectionStub->OnRemoteRequest(code, dataP, reply, option);
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
    g_toServiceConnectionStub->OnRemoteRequest(code, dataP, reply, option);
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
    g_toServiceConnectionStub->OnRemoteRequest(code, dataP, reply, option);
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
    g_toServiceConnectionStub->OnRemoteRequest(code, dataP, reply, option);
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
    g_toServiceConnectionStub->OnRemoteRequest(code, dataP, reply, option);
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
    g_toServiceConnectionStub->OnRemoteRequest(code, dataP, reply, option);
}

} // namespace Rosen
} // namespace OHOS

/* Fuzzer envirement */
extern "C" int LLVMFuzzerInitialize(int *argc, char ***argv)
{
    return OHOS::Rosen::Initialize();
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return -1;
    }

    auto fdp = FuzzedDataProvider(data, size);
    OHOS::Rosen::SetUp(fdp);

    /* Run your code on data */
    uint8_t tarPos = fdp.ConsumeIntegral<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_CREATE_VIRTUAL_SCREEN:
            // OHOS::Rosen::DoCreateVirtualScreen(fdp);
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_SCREEN_TYPE_BLACK_LIST:
            OHOS::Rosen::DoSetVirtualScreenTypeBlackList(fdp);
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_SCREEN_SECURITY_EXEMPTION_LIST:
            // OHOS::Rosen::DoSetVirtualScreenSecurityExemptionList(fdp);
            break;
        case OHOS::Rosen::DO_SET_MIRROR_SCREEN_VISIBLE_RECT:
            OHOS::Rosen::DoSetMirrorScreenVisibleRect(fdp);
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
    OHOS::Rosen::TearDown();
    return 0;
}