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

#include "rsrenderserviceconnectioncapture_fuzzer.h"

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
#include "transaction/rs_client_to_render_connection.h"
#include "platform/ohos/rs_irender_service.h"
#include "transaction/zidl/rs_client_to_render_connection_stub.h"
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
sptr<RSClientToRenderConnectionStub> toRenderConnectionStub_ = new RSClientToRenderConnection(
    g_pid, nullptr, mainThread_, screenManagerPtr_, token_->AsObject(), appVSyncDistributor_);
namespace {
const uint8_t DO_TAKE_SURFACE_CAPTURE = 0;
const uint8_t DO_TAKE_SURFACE_CAPTURE_SOLO = 1;
const uint8_t DO_TAKE_SELF_SURFACE_CAPTURE = 2;
const uint8_t DO_SET_WINDOW_FREEZE_IMMEDIATELY = 3;
const uint8_t TARGET_SIZE = 4;

sptr<RSIClientToServiceConnection> CONN = nullptr;
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

bool WriteSurfaceCaptureConfig(RSSurfaceCaptureConfig& captureConfig, MessageParcel& data)
{
    captureConfig.scaleX = GetData<float>();
    captureConfig.scaleY = GetData<float>();
    captureConfig.useDma = GetData<bool>();
    captureConfig.useCurWindow = GetData<bool>();
    captureConfig.captureType = (SurfaceCaptureType)GetData<uint8_t>();
    captureConfig.isSync = GetData<bool>();
    captureConfig.isHdrCapture = GetData<bool>();
    captureConfig.needF16WindowCaptureForScRGB = GetData<bool>();
    captureConfig.mainScreenRect.left_ = GetData<float>();
    captureConfig.mainScreenRect.top_ = GetData<float>();
    captureConfig.mainScreenRect.right_ = GetData<float>();
    captureConfig.mainScreenRect.bottom_ = GetData<float>();
    NodeId nodeId = GetData<NodeId>();
    captureConfig.blackList.push_back(nodeId);
    captureConfig.uiCaptureInRangeParam.endNodeId = GetData<uint64_t>();
    captureConfig.uiCaptureInRangeParam.useBeginNodeSize = GetData<bool>();
    captureConfig.specifiedAreaRect.left_ = GetData<float>();
    captureConfig.specifiedAreaRect.top_ = GetData<float>();
    captureConfig.specifiedAreaRect.right_ = GetData<float>();
    captureConfig.specifiedAreaRect.bottom_ = GetData<float>();
    captureConfig.backGroundColor = GetData<uint32_t>();
    if (!data.WriteFloat(captureConfig.scaleX) || !data.WriteFloat(captureConfig.scaleY) ||
        !data.WriteBool(captureConfig.useDma) || !data.WriteBool(captureConfig.useCurWindow) ||
        !data.WriteUint8(static_cast<uint8_t>(captureConfig.captureType)) || !data.WriteBool(captureConfig.isSync) ||
        !data.WriteBool(captureConfig.isHdrCapture) ||
        !data.WriteBool(captureConfig.needF16WindowCaptureForScRGB) ||
        !data.WriteFloat(captureConfig.mainScreenRect.left_) ||
        !data.WriteFloat(captureConfig.mainScreenRect.top_) ||
        !data.WriteFloat(captureConfig.mainScreenRect.right_) ||
        !data.WriteFloat(captureConfig.mainScreenRect.bottom_) ||
        !data.WriteUInt64Vector(captureConfig.blackList) ||
        !data.WriteUint64(captureConfig.uiCaptureInRangeParam.endNodeId) ||
        !data.WriteBool(captureConfig.uiCaptureInRangeParam.useBeginNodeSize) ||
        !data.WriteFloat(captureConfig.specifiedAreaRect.left_) ||
        !data.WriteFloat(captureConfig.specifiedAreaRect.top_) ||
        !data.WriteFloat(captureConfig.specifiedAreaRect.right_) ||
        !data.WriteFloat(captureConfig.specifiedAreaRect.bottom_) ||
        !data.WriteUint32(captureConfig.backGroundColor)) {
        return false;
    }
    return true;
}

bool WriteSurfaceCaptureBlurParam(RSSurfaceCaptureBlurParam& blurParam, MessageParcel& data)
{
    blurParam.isNeedBlur = GetData<bool>();
    blurParam.blurRadius = GetData<float>();
    if (!data.WriteBool(blurParam.isNeedBlur) || !data.WriteFloat(blurParam.blurRadius)) {
        return false;
    }
    return true;
}

bool WriteSurfaceCaptureAreaRect(Drawing::Rect& specifiedAreaRect, MessageParcel& data)
{
    specifiedAreaRect.left_ = GetData<float>();
    specifiedAreaRect.top_ = GetData<float>();
    specifiedAreaRect.right_ = GetData<float>();
    specifiedAreaRect.bottom_ = GetData<float>();
    if (!data.WriteFloat(specifiedAreaRect.left_) || !data.WriteFloat(specifiedAreaRect.top_) ||
        !data.WriteFloat(specifiedAreaRect.right_) || !data.WriteFloat(specifiedAreaRect.bottom_)) {
        return false;
    }
    return true;
}

void DoTakeSurfaceCapture()
{
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;

    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::TAKE_SURFACE_CAPTURE);
    uint64_t id = static_cast<NodeId>(g_pid) << 32;
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    sptr<RSISurfaceCaptureCallback> surfaceCaptureCallback = iface_cast<RSISurfaceCaptureCallback>(remoteObject);
    if (!dataP.WriteUint64(id)) {
        return;
    }
    if (!dataP.WriteRemoteObject(surfaceCaptureCallback->AsObject())) {
        return;
    }
    RSSurfaceCaptureConfig captureConfig;
    if (!WriteSurfaceCaptureConfig(captureConfig, dataP)) {
        return;
    }
    RSSurfaceCaptureBlurParam blurParam;
    if (!WriteSurfaceCaptureBlurParam(blurParam, dataP)) {
        return;
    }
    Drawing::Rect specifiedAreaRect;
    if (!WriteSurfaceCaptureAreaRect(specifiedAreaRect, dataP)) {
        return;
    }

    toRenderConnectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoTakeSurfaceCaptureSolo()
{
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;

    option.SetFlags(MessageOption::TF_SYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::TAKE_SURFACE_CAPTURE_SOLO);
    uint64_t id = static_cast<NodeId>(g_pid) << 32;
    if (!dataP.WriteUint64(id)) {
        return;
    }
    RSSurfaceCaptureConfig captureConfig;
    if (!WriteSurfaceCaptureConfig(captureConfig, dataP)) {
        return;
    }

    toRenderConnectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoTakeSelfSurfaceCapture()
{
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;

    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::TAKE_SELF_SURFACE_CAPTURE);
    uint64_t id = static_cast<NodeId>(g_pid) << 32;
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    sptr<RSISurfaceCaptureCallback> surfaceCaptureCallback = iface_cast<RSISurfaceCaptureCallback>(remoteObject);
    if (!dataP.WriteUint64(id)) {
        return;
    }
    if (!dataP.WriteRemoteObject(surfaceCaptureCallback->AsObject())) {
        return;
    }
    RSSurfaceCaptureConfig captureConfig;
    if (!WriteSurfaceCaptureConfig(captureConfig, dataP)) {
        return;
    }

    toRenderConnectionStub_->OnRemoteRequest(code, dataP, reply, option);
}

void DoSetWindowFreezeImmediately()
{
    MessageParcel dataP;
    MessageParcel reply;
    MessageOption option;

    option.SetFlags(MessageOption::TF_ASYNC);
    uint32_t code = static_cast<uint32_t>(RSIRenderServiceConnectionInterfaceCode::SET_WINDOW_FREEZE_IMMEDIATELY);
    uint64_t id = static_cast<NodeId>(g_pid) << 32;
    if (!dataP.WriteUint64(id)) {
        return;
    }
    bool isFreeze = GetData<bool>();
    if (!dataP.WriteBool(isFreeze)) {
        return;
    }
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = samgr->GetSystemAbility(RENDER_SERVICE);
    sptr<RSISurfaceCaptureCallback> surfaceCaptureCallback = iface_cast<RSISurfaceCaptureCallback>(remoteObject);
    if (!dataP.WriteRemoteObject(surfaceCaptureCallback->AsObject())) {
        return;
    }
    RSSurfaceCaptureConfig captureConfig;
    if (!WriteSurfaceCaptureConfig(captureConfig, dataP)) {
        return;
    }
    RSSurfaceCaptureBlurParam blurParam;
    if (!WriteSurfaceCaptureBlurParam(blurParam, dataP)) {
        return;
    }

    toRenderConnectionStub_->OnRemoteRequest(code, dataP, reply, option);
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
        case OHOS::Rosen::DO_TAKE_SURFACE_CAPTURE:
            OHOS::Rosen::DoTakeSurfaceCapture();
            break;
        case OHOS::Rosen::DO_TAKE_SURFACE_CAPTURE_SOLO:
            OHOS::Rosen::DoTakeSurfaceCaptureSolo();
            break;
        case OHOS::Rosen::DO_TAKE_SELF_SURFACE_CAPTURE:
            OHOS::Rosen::DoTakeSelfSurfaceCapture();
            break;
        case OHOS::Rosen::DO_SET_WINDOW_FREEZE_IMMEDIATELY:
            OHOS::Rosen::DoSetWindowFreezeImmediately();
            break;
        default:
            return -1;
    }
    return 0;
}