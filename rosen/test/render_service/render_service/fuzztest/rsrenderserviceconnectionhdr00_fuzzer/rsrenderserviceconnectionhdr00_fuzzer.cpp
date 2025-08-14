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

#include "rsrenderserviceconnectionhdr00_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <fuzzer/FuzzedDataProvider.h>
#include <iservice_registry.h>
#include <system_ability_definition.h>
#include <unistd.h>
#include <unordered_map>

#include "message_parcel.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/main_thread/rs_render_service_connection.h"
#include "platform/ohos/rs_irender_service.h"
#include "securec.h"
#include "transaction/rs_render_service_connection_stub.h"
#include "transaction/rs_transaction_proxy.h"

namespace OHOS {
namespace Rosen {
namespace {
const std::string CONFIG_FILE = "/etc/unirender.config";
const uint8_t DO_GET_SCREEN_SUPPORTED_HDRFORMATS = 0;
const uint8_t DO_GET_SCREEN_HDR_FORMAT = 1;
const uint8_t DO_SET_SCREEN_HDR_FORMAT = 2;
const uint8_t DO_GET_SCREEN_SUPPORTED_COLOR_SPACES = 3;
const uint8_t DO_GET_SCREEN_COLORSPACE = 4;
const uint8_t DO_SET_SCREEN_COLORSPACE = 5;
const uint8_t DO_GET_PIXEL_FORMAT = 6;
const uint8_t DO_SET_PIXELFORMAT = 7;
const uint8_t DO_SET_COLOR_FOLLOW = 8;
const uint8_t TARGET_SIZE = 9;
} // namespace
DECLARE_INTERFACE_DESCRIPTOR(u"ohos.rosen.RenderServiceConnection");
RSMainThread* g_mainThread = nullptr;
sptr<RSIConnectionToken> g_token = nullptr;
sptr<RSRenderServiceConnectionStub> g_connectionStub = nullptr;
sptr<RSRenderServiceConnection> g_connection = nullptr;
std::string g_originTag = "";

/* Call once in the Fuzzer Initialize function */
int Initialize()
{
    g_mainThread = RSMainThread::Instance();
    g_mainThread->runner_ = AppExecFwk::EventRunner::Create(true);
    g_mainThread->handler_ = std::make_shared<AppExecFwk::EventHandler>(g_mainThread->runner_);
    g_token = new IRemoteStub<RSIConnectionToken>();
    auto generator = impl::VSyncGenerator::GetInstance();
    auto appVSyncController = new VSyncController(generator, 0);
    DVSyncFeatureParam dvsyncParam;
    auto appVSyncDistributor = new VSyncDistributor(appVSyncController, "app", dvsyncParam);
    g_connection = new RSRenderServiceConnection(getpid(), nullptr, nullptr,
        impl::RSScreenManager::GetInstance(), g_token->AsObject(), appVSyncDistributor);
    g_connectionStub = g_connection;
#ifdef RS_ENABLE_VK
    RsVulkanContext::GetSingleton().InitVulkanContextForUniRender("");
#endif
    RSHardwareThread::Instance().Start();
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
    g_connection->mainThread_ = g_mainThread;
}

void TearDown()
{
    WriteUnirenderConfig(g_originTag);
    g_connection->mainThread_ = nullptr;
}

/* Fuzzer test GetPixelFormat */
void DoGetPixelFormat(FuzzedDataProvider& fdp)
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_PIXEL_FORMAT);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteUint64(id);
    g_connectionStub->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

/* Fuzzer test SetPixelFormat */
void DoSetPixelFormat(FuzzedDataProvider& fdp)
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_PIXEL_FORMAT);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    int32_t pixelFormat = fdp.ConsumeIntegral<int32_t>();
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteUint64(id);
    dataParcel.WriteInt32(pixelFormat);
    g_connectionStub->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

/* Fuzzer test GetScreenSupportedHDRFormats */
void DoGetScreenSupportedHDRFormats(FuzzedDataProvider& fdp)
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_HDR_FORMATS);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteUint64(id);
    g_connectionStub->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

/* Fuzzer test GetScreenHDRFormat */
void DoGetScreenHDRFormat(FuzzedDataProvider& fdp)
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_HDR_FORMAT);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteUint64(id);
    g_connectionStub->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

/* Fuzzer test SetScreenHDRFormat */
void DoSetScreenHDRFormat(FuzzedDataProvider& fdp)
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_HDR_FORMAT);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    int32_t modeIdx = fdp.ConsumeIntegral<int32_t>();
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteUint64(id);
    dataParcel.WriteInt32(modeIdx);
    g_connectionStub->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

/* Fuzzer test GetScreenSupportedColorSpaces */
void DoGetScreenSupportedColorSpaces(FuzzedDataProvider& fdp)
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_SUPPORTED_COLORSPACES);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteUint64(id);
    g_connectionStub->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

/* Fuzzer test GetScreenColorSpace */
void DoGetScreenColorSpace(FuzzedDataProvider& fdp)
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::GET_SCREEN_COLORSPACE);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteUint64(id);
    g_connectionStub->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

/* Fuzzer test SetScreenColorSpace */
void DoSetScreenColorSpace(FuzzedDataProvider& fdp)
{
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_SCREEN_COLORSPACE);
    MessageOption option;
    MessageParcel dataParcel;
    MessageParcel replyParcel;
    ScreenId id = fdp.ConsumeIntegral<uint64_t>();
    int32_t color = fdp.ConsumeIntegral<int32_t>();
    dataParcel.WriteInterfaceToken(GetDescriptor());
    dataParcel.WriteUint64(id);
    dataParcel.WriteInt32(color);
    g_connectionStub->OnRemoteRequest(code, dataParcel, replyParcel, option);
}

/* Fuzzer test SetColorFollow */
void DoSetColorFollow(FuzzedDataProvider& fdp)
{
    RSSurfaceRenderNodeConfig config = { .id = 1, .nodeType = RSSurfaceNodeType::SELF_DRAWING_NODE };
    auto mainThread = g_mainThread;
    mainThread->ScheduleTask([=]() {
        auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(config.id);
        surfaceNode->nodeType_ = config.nodeType;
        surfaceNode->stagingRenderParams_ = std::make_unique<RSSurfaceRenderParams>(config.id);
        auto& context = mainThread->GetContext();
        context.GetMutableNodeMap().surfaceNodeMap_.emplace(config.id, surfaceNode);
    }).wait();

    bool isColorFollow = fdp.ConsumeBool();
    g_connectionStub->SetColorFollow(config.name, isColorFollow);

    mainThread->ScheduleTask([=]() {
        auto& context = mainThread->GetContext();
        context.GetMutableNodeMap().surfaceNodeMap_.erase(config.id);
    }).wait();
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer envirement */
extern "C" int LLVMFuzzerInitialize(const uint8_t* data, size_t size)
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
        case OHOS::Rosen::DO_GET_SCREEN_SUPPORTED_HDRFORMATS:
            OHOS::Rosen::DoGetScreenSupportedHDRFormats(fdp);
            break;
        case OHOS::Rosen::DO_GET_SCREEN_HDR_FORMAT:
            OHOS::Rosen::DoGetScreenHDRFormat(fdp);
            break;
        case OHOS::Rosen::DO_SET_SCREEN_HDR_FORMAT:
            OHOS::Rosen::DoSetScreenHDRFormat(fdp);
            break;
        case OHOS::Rosen::DO_GET_SCREEN_SUPPORTED_COLOR_SPACES:
            OHOS::Rosen::DoGetScreenSupportedColorSpaces(fdp);
            break;
        case OHOS::Rosen::DO_GET_SCREEN_COLORSPACE:
            OHOS::Rosen::DoGetScreenColorSpace(fdp);
            break;
        case OHOS::Rosen::DO_SET_SCREEN_COLORSPACE:
            OHOS::Rosen::DoSetScreenColorSpace(fdp);
            break;
        case OHOS::Rosen::DO_GET_PIXEL_FORMAT:
            OHOS::Rosen::DoGetPixelFormat(fdp);
            break;
        case OHOS::Rosen::DO_SET_PIXELFORMAT:
            OHOS::Rosen::DoSetPixelFormat(fdp);
            break;
        case OHOS::Rosen::DO_SET_COLOR_FOLLOW:
            OHOS::Rosen::DoSetColorFollow(fdp);
            break;
        default:
            // do nothing
            break;
    }
    OHOS::Rosen::TearDown();
    return 0;
}