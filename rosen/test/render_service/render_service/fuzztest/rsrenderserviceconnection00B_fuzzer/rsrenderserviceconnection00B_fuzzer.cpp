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

#include "rsrenderserviceconnection00B_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <unistd.h>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <unordered_map>
#include <fuzzer/FuzzedDataProvider.h>

#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/main_thread/rs_render_service_connection.h"
#include "platform/ohos/rs_irender_service.h"
#include "transaction/rs_render_service_connection_stub.h"
#include "transaction/rs_transaction_proxy.h"
#include "message_parcel.h"
#include "securec.h"
#include <iservice_registry.h>
#include <system_ability_definition.h>

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
const uint8_t DO_REPORT_JANK_STATS = 0;
const uint8_t DO_REPORT_EVENT_RESPONSE = 1;
const uint8_t DO_REPORT_EVENT_COMPLETE = 2;
const uint8_t DO_REPORT_EVENT_JANK_FRAME = 3;
const uint8_t DO_REPORT_RS_SCENE_JANK_START = 4;
const uint8_t DO_REPORT_RS_SCENE_JANK_END = 5;
const uint8_t DO_REPORT_EVENT_GAMESTATE = 6;
const uint8_t TARGET_SIZE = 7;

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
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_JANK_STATS);

    connectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoReportEventResponse()
{
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_EVENT_RESPONSE);
    DataBaseRs info;
    WriteDataBaseRs(info, dataP);
    connectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoReportEventComplete()
{
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_EVENT_COMPLETE);
    DataBaseRs info;
    WriteDataBaseRs(info, dataP);
    connectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoReportEventJankFrame()
{
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_EVENT_JANK_FRAME);
    DataBaseRs info;
    WriteDataBaseRs(info, dataP);
    connectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoReportRsSceneJankStart()
{
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_RS_SCENE_JANK_START);
    AppInfo info;
    WriteAppInfo(info, dataP);
    connectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoReportRsSceneJankEnd()
{
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_RS_SCENE_JANK_END);
    AppInfo info;
    WriteAppInfo(info, dataP);
    connectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoReportGameStateData()
{
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;
    if (!dataP.WriteInterfaceToken(RSIRenderServiceConnection::GetDescriptor())) {
        return;
    }
    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::REPORT_EVENT_GAMESTATE);
    GameStateData info;
    WriteGameStateDataRs(info, dataP);
    connectionStub_->OnRemoteRequest(code, dataP, reply, option);
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
        default:
            return -1;
    }
    return 0;
}