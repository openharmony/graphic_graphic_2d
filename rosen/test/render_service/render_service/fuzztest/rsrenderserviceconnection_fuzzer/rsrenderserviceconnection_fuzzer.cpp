/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "rsrenderserviceconnection_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>
#include <unordered_map>

#include "accesstoken_kit.h"
#ifdef SUPPORT_ACCESS_TOKEN
#include "nativetoken_kit.h"
#include "token_setproc.h"
#endif
#include "ipc_object_proxy.h"
#include "ipc_object_stub.h"
#include "iremote_object.h"
#include "message_parcel.h"
#include "securec.h"

#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
#include "ipc_callbacks/pointer_render/pointer_luminance_callback_stub.h"
#endif
#include "ipc_callbacks/rs_occlusion_change_callback_stub.h"
#include "ipc_callbacks/rs_first_frame_commit_callback_stub.h"
#include "render_server/rs_render_service.h"
#include "render_server/transaction/rs_client_to_service_connection.h"
#include "transaction/rs_client_to_render_connection.h"
#include "platform/ohos/rs_render_service_connect_hub.cpp"
#include "screen_manager/rs_screen_manager.h"
#include "transaction/rs_render_service_client.h"
#include "render_server/transaction/zidl/rs_client_to_service_connection_stub.h"
#include "transaction/zidl/rs_client_to_render_connection_stub.h"
#include "transaction/rs_transaction_proxy.h"

namespace OHOS {
namespace Rosen {
constexpr size_t SCREEN_WIDTH = 100;
constexpr size_t SCREEN_HEIGHT = 100;
sptr<RSIClientToServiceConnection> rsToServiceConn_ = nullptr;
sptr<RSIClientToRenderConnection> rsToRenderConn_ = nullptr;
namespace {
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;

template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (g_data == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, g_data + g_pos, objectSize);
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
    if (g_data == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    object.assign(reinterpret_cast<const char*>(g_data + g_pos), objectSize);
    g_pos += objectSize;
    return object;
}
} // namespace

bool Init(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    g_data = data;
    g_size = size;
    g_pos = 0;
    
    rsToServiceConn_ = RSRenderServiceConnectHub::GetRenderService().first;
    rsToRenderConn_ = RSRenderServiceConnectHub::GetRenderService().second;
    if (rsToServiceConn_ == nullptr || rsToRenderConn_ == nullptr) {
        return false;
    }
    return true;
}

bool DoRegisterApplicationAgent()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }

    uint32_t pid = GetData<uint32_t>();
    MessageParcel message;
    auto remoteObject = message.ReadRemoteObject();
    sptr<IApplicationAgent> app = iface_cast<IApplicationAgent>(remoteObject);
    rsToServiceConn_->RegisterApplicationAgent(pid, app);
    return true;
}

bool DoCommitTransaction()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }

    auto transactionData = std::make_unique<RSTransactionData>();
    rsToServiceConn_->CommitTransaction(transactionData);
    return true;
}

class DerivedSyncTask : public RSSyncTask {
public:
    using RSSyncTask::RSSyncTask;
    bool CheckHeader(Parcel& parcel) const override
    {
        return true;
    }
    bool ReadFromParcel(Parcel& parcel) override
    {
        return true;
    }
    bool Marshalling(Parcel& parcel) const override
    {
        return true;
    }
    void Process(RSContext& context) override {}
};
bool DoExecuteSynchronousTask()
{
    if (rsToRenderConn_ == nullptr) {
        return false;
    }

    uint64_t timeoutNS = GetData<uint64_t>();
    auto task = std::make_shared<DerivedSyncTask>(timeoutNS);
    rsToRenderConn_->ExecuteSynchronousTask(task);
    return true;
}

bool DoGetMemoryGraphic()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }

    int pid = GetData<int>();
    MemoryGraphic memoryGraphic;
    rsToServiceConn_->GetMemoryGraphic(pid, memoryGraphic);
    return true;
}

bool DoGetMemoryGraphics()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }

    std::vector<MemoryGraphic> memoryGraphics;
    rsToServiceConn_->GetMemoryGraphics(memoryGraphics);
    return true;
}

bool DoCreateNodeAndSurface()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    RSSurfaceRenderNodeConfig config = { .id = 0, .name = "test" };
    bool success;
    rsToServiceConn_->CreateNode(config, success);
    sptr<Surface> surface = nullptr;
    rsToServiceConn_->CreateNodeAndSurface(config, surface);
    return true;
}

bool DoGetScreenBacklight()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    ScreenId id = GetData<uint64_t>();
    uint32_t setLevel = GetData<uint32_t>();
    rsToServiceConn_->SetScreenBacklight(id, setLevel);
    int32_t getLevel = GetData<int32_t>();
    rsToServiceConn_->GetScreenBacklight(id, getLevel);
    return true;
}

bool DoGetScreenType()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    ScreenId id = GetData<uint64_t>();
    uint32_t level = GetData<uint32_t>();
    RSScreenType type = (RSScreenType)level;
    rsToServiceConn_->GetScreenType(id, type);
    return true;
}

bool DoRegisterBufferAvailableListener()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    uint64_t id = GetData<uint64_t>();
    bool isFromRenderThread = GetData<bool>();
    sptr<RSIBufferAvailableCallback> cb = nullptr;
    rsToServiceConn_->RegisterBufferAvailableListener(id, cb, isFromRenderThread);
    return true;
}

bool DoSetScreenSkipFrameInterval()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    ScreenId id = GetData<uint64_t>();
    uint32_t skipFrameInterval = GetData<uint32_t>();
    int32_t resCode;
    rsToServiceConn_->SetScreenSkipFrameInterval(id, skipFrameInterval, resCode);
    return true;
}

bool DoSetPhysicalScreenResolution()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    ScreenId id = GetData<uint64_t>();
    rsToServiceConn_->SetPhysicalScreenResolution(id, SCREEN_WIDTH, SCREEN_HEIGHT);
    return true;
}

bool DoSetVirtualScreenResolution()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    ScreenId id = GetData<uint64_t>();
    rsToServiceConn_->SetVirtualScreenResolution(id, SCREEN_WIDTH, SCREEN_HEIGHT);
    rsToServiceConn_->GetVirtualScreenResolution(id);
    return true;
}

bool DoGetScreenSupportedColorGamuts()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    std::vector<ScreenColorGamut> mode;
    ScreenId id = GetData<uint64_t>();
    uint32_t screenCol = GetData<uint32_t>();
    mode.push_back((ScreenColorGamut)screenCol);
    rsToServiceConn_->GetScreenSupportedColorGamuts(id, mode);
    std::vector<ScreenHDRMetadataKey> keys;
    keys.push_back((ScreenHDRMetadataKey)screenCol);
    rsToServiceConn_->GetScreenSupportedMetaDataKeys(id, keys);
    return true;
}

bool DoGetScreenSupportedModes()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    ScreenId id = GetData<uint64_t>();
    rsToServiceConn_->GetScreenSupportedModes(id);
    return true;
}

bool DoGetScreenColorGamut()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    ScreenId id = GetData<uint64_t>();
    int32_t modeIdx = GetData<int32_t>();
    rsToServiceConn_->SetScreenColorGamut(id, modeIdx);
    uint32_t mod = GetData<uint32_t>();
    ScreenColorGamut mode = (ScreenColorGamut)mod;
    rsToServiceConn_->GetScreenColorGamut(id, mode);
    return true;
}

bool DoSetScreenPowerStatus()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    ScreenId id = GetData<uint64_t>();
    uint32_t status = GetData<uint32_t>();
    rsToServiceConn_->SetScreenPowerStatus(id, static_cast<ScreenPowerStatus>(status));
    rsToServiceConn_->GetScreenPowerStatus(id, status);
    return true;
}

bool DoSetScreenGamutMap()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    ScreenId id = GetData<uint64_t>();
    uint32_t mapMode = GetData<uint32_t>();
    ScreenGamutMap mode = (ScreenGamutMap)mapMode;
    rsToServiceConn_->SetScreenGamutMap(id, mode);
    rsToServiceConn_->GetScreenGamutMap(id, mode);
    return true;
}

bool DoSetAppWindowNum()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    uint32_t num = GetData<uint32_t>();
    rsToServiceConn_->SetAppWindowNum(num);
    return true;
}

bool DoCreateVirtualScreen()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    ScreenId id = GetData<uint64_t>();
    int32_t flags = GetData<int32_t>();
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("DisplayNode");
    sptr<IBufferProducer> bp = cSurface->GetProducer();
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(bp);
    rsToServiceConn_->CreateVirtualScreen("name", SCREEN_WIDTH, SCREEN_HEIGHT, pSurface, id, flags);
    rsToServiceConn_->SetVirtualScreenSurface(id, pSurface);
    rsToServiceConn_->RemoveVirtualScreen(id);
    return true;
}

#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
bool DoSetPointerColorInversionConfig()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    float darkBuffer = GetData<float>();
    float brightBuffer = GetData<float>();
    int64_t interval = GetData<int64_t>();
    int32_t rangeSize = GetData<int32_t>();
    rsToServiceConn_->SetPointerColorInversionConfig(darkBuffer, brightBuffer, interval, rangeSize);
    return true;
}

bool DoSetPointerColorInversionEnabled()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    bool enable = GetData<bool>();
    rsToServiceConn_->SetPointerColorInversionEnabled(enable);
    return true;
}

class CustomPointerLuminanceChangeCallback : public RSPointerLuminanceChangeCallbackStub {
public:
    explicit CustomPointerLuminanceChangeCallback(const PointerLuminanceChangeCallback &callback) : cb_(callback) {}
    ~CustomPointerLuminanceChangeCallback() override {};
 
    void OnPointerLuminanceChanged(int32_t brightness) override
    {
        if (cb_ != nullptr) {
            cb_(brightness);
        }
    }

private:
    PointerLuminanceChangeCallback cb_;
};

bool DoRegisterPointerLuminanceChangeCallback()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    PointerLuminanceChangeCallback callback = [](int32_t brightness) {};
    sptr<CustomPointerLuminanceChangeCallback> cb = new CustomPointerLuminanceChangeCallback(callback);
    rsToServiceConn_->RegisterPointerLuminanceChangeCallback(cb);
    return true;
}

bool DoUnRegisterPointerLuminanceChangeCallback()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    rsToServiceConn_->UnRegisterPointerLuminanceChangeCallback();
    return true;
}
#endif

bool DoSetScreenActiveMode()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    ScreenId id = GetData<uint64_t>();
    uint32_t modeId = GetData<uint32_t>();
    rsToServiceConn_->SetScreenActiveMode(id, modeId);
    RSScreenModeInfo screenModeInfo;
    rsToServiceConn_->GetScreenActiveMode(id, screenModeInfo);
    return true;
}

bool DoSetScreenActiveRect()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    ScreenId id = GetData<uint64_t>();
    int32_t x = GetData<int32_t>();
    int32_t y = GetData<int32_t>();
    int32_t w = GetData<int32_t>();
    int32_t h = GetData<int32_t>();
    Rect activeRect {
        .x = x,
        .y = y,
        .w = w,
        .h = h
    };
    uint32_t repCode;
    rsToServiceConn_->SetScreenActiveRect(id, activeRect, repCode);
    return true;
}

bool DoSetRefreshRateMode()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    int32_t refreshRateMode = GetData<int32_t>();
    rsToServiceConn_->SetRefreshRateMode(refreshRateMode);
    return true;
}

bool DoCreateVSyncConnection()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    uint64_t id = GetData<uint64_t>();
    sptr<VSyncIConnectionToken> token = new IRemoteStub<VSyncIConnectionToken>();
    sptr<IVSyncConnection> conn = nullptr;
    VSyncConnParam vsyncConnParam = {id, 0, false};
    rsToServiceConn_->CreateVSyncConnection(conn, "test", token, vsyncConnParam);
    return true;
}

bool DoSetScreenRefreshRate()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    ScreenId id = GetData<uint64_t>();
    int32_t sceneId = GetData<int32_t>();
    int32_t rate = GetData<int32_t>();
    bool enabled = GetData<bool>();
    int32_t type = GetData<int32_t>();
    rsToServiceConn_->SetScreenRefreshRate(id, sceneId, rate);
    rsToServiceConn_->GetScreenCurrentRefreshRate(id);
    rsToServiceConn_->GetScreenSupportedRefreshRates(id);
    rsToServiceConn_->GetCurrentRefreshRateMode();
    rsToServiceConn_->GetShowRefreshRateEnabled(enabled);
    rsToServiceConn_->SetShowRefreshRateEnabled(enabled, type);
    rsToServiceConn_->GetRealtimeRefreshRate(id);
    return true;
}

bool DoGetBitmap()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    Drawing::Bitmap bm;
    NodeId id = GetData<uint64_t>();
    bool success;
    rsToServiceConn_->GetBitmap(id, bm, success);
    return true;
}

bool DoGetScreenCapability()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    ScreenId id = GetData<uint64_t>();
    rsToServiceConn_->GetScreenCapability(id);
    return true;
}

bool DoGetScreenData()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    ScreenId id = GetData<uint64_t>();
    rsToServiceConn_->GetScreenData(id);
    return true;
}

bool DoGetScreenHDRCapability()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    ScreenId id = GetData<uint64_t>();
    RSScreenHDRCapability screenHDRCapability;
    rsToServiceConn_->GetScreenHDRCapability(id, screenHDRCapability);
    return true;
}

class CustomOcclusionChangeCallback : public RSOcclusionChangeCallbackStub {
public:
    explicit CustomOcclusionChangeCallback(const OcclusionChangeCallback& callback) : cb_(callback) {}
    ~CustomOcclusionChangeCallback() override {};

    void OnOcclusionVisibleChanged(std::shared_ptr<RSOcclusionData> occlusionData) override
    {
        if (cb_ != nullptr) {
            cb_(occlusionData);
        }
    }

private:
    OcclusionChangeCallback cb_;
};

bool DoRegisterOcclusionChangeCallback()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    OcclusionChangeCallback callback = [](std::shared_ptr<RSOcclusionData> data) {};
    sptr<CustomOcclusionChangeCallback> cb = new CustomOcclusionChangeCallback(callback);
    int32_t repCode = GetData<int32_t>();
    rsToServiceConn_->RegisterOcclusionChangeCallback(cb, repCode);
    return true;
}

bool DoShowWatermark()
{
#ifdef SUPPORT_ACCESS_TOKEN
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    const char *perms[1];
    perms[0] = "ohos.permission.UPDATE_CONFIGURATION";
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = 1,
        .aclsNum = 0,
        .dcaps = NULL,
        .perms = perms,
        .acls = NULL,
        .processName = "DoShowWatermark",
        .aplStr = "system_core",
    };
    uint64_t tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
    bool isShow = GetData<bool>();
    std::shared_ptr<Media::PixelMap> pixelMap1;
    rsToServiceConn_->ShowWatermark(pixelMap1, isShow);
#endif
    return true;
}

bool DoTakeSurfaceCapture()
{
    if (rsToRenderConn_ == nullptr) {
        return false;
    }
    uint64_t nodeId = GetData<uint64_t>();
    sptr<RSISurfaceCaptureCallback> callback = nullptr;
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = GetData<float>();
    captureConfig.scaleY = GetData<float>();
    captureConfig.useDma = GetData<bool>();
    captureConfig.useCurWindow = GetData<bool>();
    uint8_t type = GetData<uint8_t>();
    captureConfig.captureType = (SurfaceCaptureType)type;
    captureConfig.isSync = GetData<bool>();
    uint8_t listSize = GetData<uint8_t>();
    for (uint8_t i = 0; i < listSize; ++i) {
        uint64_t listNodeId = GetData<uint64_t>();
        captureConfig.blackList.push_back(listNodeId);
    }
    captureConfig.mainScreenRect.left_ = GetData<float>();
    captureConfig.mainScreenRect.top_ = GetData<float>();
    captureConfig.mainScreenRect.right_ = GetData<float>();
    captureConfig.mainScreenRect.bottom_ = GetData<float>();
    
    rsToRenderConn_->TakeSurfaceCapture(nodeId, callback, captureConfig);
    return true;
}

bool DoSetHwcNodeBounds()
{
    if (rsToRenderConn_ == nullptr) {
        return false;
    }
    uint64_t nodeId = GetData<uint64_t>();
    float positionX = GetData<float>();
    float positionY = GetData<float>();
    float positionZ = GetData<float>();
    float positionW = GetData<float>();
    rsToRenderConn_->SetHwcNodeBounds(nodeId, positionX, positionY, positionZ, positionW);
    return true;
}

bool DoSetScreenChangeCallback()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    sptr<RSIScreenChangeCallback> callback = nullptr;
    rsToServiceConn_->SetScreenChangeCallback(callback);
    return true;
}

bool DoSetScreenSwitchingNotifyCallback()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    sptr<RSIScreenSwitchingNotifyCallback> callback = nullptr;
    rsToServiceConn_->SetScreenSwitchingNotifyCallback(callback);
    return true;
}

bool DoSetFocusAppInfo()
{
    if (rsToRenderConn_ == nullptr) {
        return false;
    }

    int32_t pid = GetData<int32_t>();
    int32_t uid = GetData<int32_t>();
    std::string bundleName = GetData<std::string>();
    std::string abilityName = GetData<std::string>();
    uint64_t focusNodeId = GetData<uint64_t>();
    FocusAppInfo info = {
        .pid = pid,
        .uid = uid,
        .bundleName = bundleName,
        .abilityName = abilityName,
        .focusNodeId = focusNodeId};
    int32_t repCode = GetData<int32_t>();
    rsToRenderConn_->SetFocusAppInfo(info, repCode);
    return true;
}

bool DoSetAncoForceDoDirect()
{
    if (rsToRenderConn_ == nullptr) {
        return false;
    }

    bool direct = GetData<bool>();
    bool res;
    rsToRenderConn_->SetAncoForceDoDirect(direct, res);
    return true;
}

bool DoGetActiveDirtyRegionInfo()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }

    rsToServiceConn_->GetActiveDirtyRegionInfo();
    return true;
}

bool DoGetGlobalDirtyRegionInfo()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }

    rsToServiceConn_->GetGlobalDirtyRegionInfo();
    return true;
}

bool DoNotifySoftVsyncRateDiscountEvent()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    uint32_t pid = GetData<uint32_t>();
    std::string name = GetData<std::string>();
    uint32_t rateDiscount = GetData<uint32_t>();
    bool result = rsToServiceConn_->NotifySoftVsyncRateDiscountEvent(pid, name, rateDiscount);
    return result;
}

bool DoGetLayerComposeInfo()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }

    rsToServiceConn_->GetLayerComposeInfo();
    return true;
}

bool DoGetHwcDisabledReasonInfo()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }

    rsToServiceConn_->GetHwcDisabledReasonInfo();
    return true;
}

bool DoGetUniRenderEnabled()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    bool enable;
    rsToServiceConn_->GetUniRenderEnabled(enable);
    return true;
}

bool DoCreateNode1()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    RSDisplayNodeConfig displayNodeConfig;
    displayNodeConfig.screenId = GetData<uint64_t>();
    displayNodeConfig.isMirrored = GetData<bool>();
    displayNodeConfig.mirrorNodeId = GetData<uint64_t>();
    displayNodeConfig.isSync = GetData<bool>();
    uint64_t nodeId = GetData<uint64_t>();
    bool success;
    rsToServiceConn_->CreateNode(displayNodeConfig, nodeId, success);
    return true;
}

bool DoCreateNode2()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    RSSurfaceRenderNodeConfig config;
    config.id = GetData<uint64_t>();
    config.name = GetData<std::string>();
    bool success;
    rsToServiceConn_->CreateNode(config, success);
    return true;
}

bool DoGetDefaultScreenId()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    uint64_t screenId = GetData<uint64_t>();
    rsToServiceConn_->GetDefaultScreenId(screenId);
    return true;
}

bool DoGetActiveScreenId()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    uint64_t screenId = GetData<uint64_t>();
    rsToServiceConn_->GetActiveScreenId(screenId);
    return true;
}

bool DoGetAllScreenIds()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    rsToServiceConn_->GetAllScreenIds();
    return true;
}

bool DoGetTotalAppMemSize()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    float cpuMemSize = GetData<float>();
    float gpuMemSize = GetData<float>();
    rsToServiceConn_->GetTotalAppMemSize(cpuMemSize, gpuMemSize);
    return true;
}

bool DoSetVirtualScreenAutoRotation()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    ScreenId screenId = GetData<ScreenId>();
    bool isAutoRotation = GetData<bool>();
    rsToServiceConn_->SetVirtualScreenAutoRotation(screenId, isAutoRotation);
    return true;
}

bool DoSetVirtualScreenBlackList()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    uint64_t id = GetData<uint64_t>();
    uint64_t nodeId = GetData<uint64_t>();
    std::vector<uint64_t> blackListVector;
    blackListVector.push_back(nodeId);
    rsToServiceConn_->SetVirtualScreenBlackList(id, blackListVector);
    return true;
}

bool DoAddVirtualScreenBlackList()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    uint64_t id = GetData<uint64_t>();
    std::vector<NodeId> blackListVector;
    uint8_t listSize = GetData<uint8_t>();
    for (uint8_t i = 0; i < listSize; ++i) {
        NodeId nodeId = GetData<NodeId>();
        blackListVector.push_back(nodeId);
    }
    int32_t repCode = 0;
    rsToServiceConn_->AddVirtualScreenBlackList(id, blackListVector, repCode);
    return true;
}

bool DoRemoveVirtualScreenBlackList()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    uint64_t id = GetData<uint64_t>();
    std::vector<NodeId> blackListVector;
    uint8_t listSize = GetData<uint8_t>();
    for (uint8_t i = 0; i < listSize; ++i) {
        NodeId nodeId = GetData<NodeId>();
        blackListVector.push_back(nodeId);
    }
    int32_t repCode = 0;
    rsToServiceConn_->RemoveVirtualScreenBlackList(id, blackListVector, repCode);
    return true;
}

bool DoSetVirtualScreenSecurityExemptionList()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    uint64_t id = GetData<uint64_t>();
    uint64_t nodeId = GetData<uint64_t>();
    std::vector<uint64_t> securityExemptionList;
    securityExemptionList.push_back(nodeId);
    rsToServiceConn_->SetVirtualScreenSecurityExemptionList(id, securityExemptionList);
    return true;
}

bool DoSetCastScreenEnableSkipWindow()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    uint64_t id = GetData<uint64_t>();
    bool enable = GetData<bool>();
    rsToServiceConn_->SetCastScreenEnableSkipWindow(id, enable);
    return true;
}

bool DoSyncFrameRateRange()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    uint64_t id = GetData<uint64_t>();
    FrameRateRange range;
    int32_t animatorExpectedFrameRate = GetData<int32_t>();
    rsToServiceConn_->SyncFrameRateRange(id, range, animatorExpectedFrameRate);
    return true;
}

bool DoUnregisterFrameRateLinker()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    uint64_t id = GetData<uint64_t>();
    rsToServiceConn_->UnregisterFrameRateLinker(id);
    return true;
}

bool DoMarkPowerOffNeedProcessOneFrame()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    rsToServiceConn_->MarkPowerOffNeedProcessOneFrame();
    return true;
}

bool DoDisablePowerOffRenderControl()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    uint64_t id = GetData<uint64_t>();
    rsToServiceConn_->DisablePowerOffRenderControl(id);
    return true;
}

bool DoSetScreenCorrection()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    uint64_t id = GetData<uint64_t>();
    uint32_t screenRotation = GetData<uint32_t>();
    rsToServiceConn_->SetScreenCorrection(id, static_cast<ScreenRotation>(screenRotation));
    return true;
}

bool DoSetVirtualMirrorScreenCanvasRotation()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    uint64_t id = GetData<uint64_t>();
    bool canvasRotation = GetData<bool>();
    rsToServiceConn_->SetVirtualMirrorScreenCanvasRotation(id, canvasRotation);
    return true;
}

bool DoSetVirtualMirrorScreenScaleMode()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    uint64_t id = GetData<uint64_t>();
    uint32_t scaleMode = GetData<uint32_t>();
    rsToServiceConn_->SetVirtualMirrorScreenScaleMode(id, static_cast<ScreenScaleMode>(scaleMode));
    return true;
}

bool DoSetGlobalDarkColorMode()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    bool isDark = GetData<bool>();
    rsToServiceConn_->SetGlobalDarkColorMode(isDark);
    return true;
}

bool DoSetPixelFormat()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    uint64_t id = GetData<uint64_t>();
    uint32_t pixelFormat = GetData<uint32_t>();
    int32_t resCode;
    rsToServiceConn_->SetPixelFormat(id, static_cast<GraphicPixelFormat>(pixelFormat), resCode);
    GraphicPixelFormat pixelFormat1;
    rsToServiceConn_->GetPixelFormat(id, pixelFormat1, resCode);
    return true;
}

bool DOGetScreenSupportedHDRFormats()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    uint64_t id = GetData<uint64_t>();
    int32_t modeIdx = GetData<int32_t>();
    int32_t resCode;
    rsToServiceConn_->SetScreenHDRFormat(id, modeIdx, resCode);
    ScreenHDRFormat hdrFormat;
    rsToServiceConn_->GetScreenHDRFormat(id, hdrFormat, resCode);
    std::vector<ScreenHDRFormat> hdrFormats;
    rsToServiceConn_->GetScreenSupportedHDRFormats(id, hdrFormats, resCode);
    return true;
}

bool DOGetScreenSupportedColorSpaces()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    uint64_t id = GetData<uint64_t>();
    uint32_t colorSpace = GetData<uint32_t>();
    int32_t resCode;
    rsToServiceConn_->SetScreenColorSpace(id, static_cast<GraphicCM_ColorSpaceType>(colorSpace), resCode);
    std::vector<GraphicCM_ColorSpaceType> colorSpaces;
    rsToServiceConn_->GetScreenSupportedColorSpaces(id, colorSpaces, resCode);
    return true;
}

bool DOSetVirtualScreenRefreshRate()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    uint64_t id = GetData<uint64_t>();
    uint32_t maxRefreshRate = GetData<uint32_t>();
    uint32_t actualRefreshRate = GetData<uint32_t>();
    int32_t resCode;
    rsToServiceConn_->SetVirtualScreenRefreshRate(id, maxRefreshRate, actualRefreshRate, resCode);
    return true;
}

bool DOSetSystemAnimatedScenes()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    uint32_t systemAnimatedScenes = GetData<uint32_t>();
    bool success = GetData<bool>();
    rsToServiceConn_->SetSystemAnimatedScenes(static_cast<SystemAnimatedScenes>(systemAnimatedScenes), false, success);
    return true;
}

bool DOResizeVirtualScreen()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    uint64_t id = GetData<uint64_t>();
    uint32_t width = GetData<uint32_t>();
    uint32_t height = GetData<uint32_t>();
    rsToServiceConn_->ResizeVirtualScreen(id, width, height);
    return true;
}

bool DOReportJankStats()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    rsToServiceConn_->ReportJankStats();
    return true;
}

bool DOReportEventResponse()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    DataBaseRs info;
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
    rsToServiceConn_->ReportEventJankFrame(info);
    rsToServiceConn_->ReportEventResponse(info);
    return true;
}

bool DOReportEventComplete()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    DataBaseRs info;
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
    rsToServiceConn_->ReportEventJankFrame(info);
    rsToServiceConn_->ReportEventComplete(info);
    return true;
}

bool DOReportEventJankFrame()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    DataBaseRs info;
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
    rsToServiceConn_->ReportEventJankFrame(info);
    return true;
}

bool DOReportGameStateData()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    GameStateData info;
    info.pid = GetData<int32_t>();
    info.uid = GetData<int32_t>();
    info.state = GetData<int32_t>();
    info.renderTid = GetData<int32_t>();
    info.bundleName = GetData<std::string>();
    rsToServiceConn_->ReportGameStateData(info);
    return true;
}

bool DOSetHidePrivacyContent()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    uint32_t id = GetData<uint32_t>();
    bool needHidePrivacyContent = GetData<bool>();
    uint32_t resCode;
    rsToServiceConn_->SetHidePrivacyContent(id, needHidePrivacyContent, resCode);
    return true;
}

bool DONotifyLightFactorStatus()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    int32_t lightFactorStatus = GetData<int32_t>();
    rsToServiceConn_->NotifyLightFactorStatus(lightFactorStatus);
    return true;
}

bool DONotifyPackageEvent()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    uint32_t listSize = GetData<uint32_t>();
    std::string package = GetData<std::string>();
    std::vector<std::string> packageList;
    packageList.push_back(package);
    rsToServiceConn_->NotifyPackageEvent(listSize, packageList);
    return true;
}


bool DONotifyAppStrategyConfigChangeEvent()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    std::string pkgName = GetData<std::string>();
    uint32_t listSize = GetData<uint32_t>();
    std::string configKey = GetData<std::string>();
    std::string configValue = GetData<std::string>();
    std::vector<std::pair<std::string, std::string>> newConfig;
    newConfig.push_back(make_pair(configKey, configValue));
    rsToServiceConn_->NotifyAppStrategyConfigChangeEvent(pkgName, listSize, newConfig);
    return true;
}

bool DONotifyRefreshRateEvent()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    EventInfo eventInfo;
    eventInfo.eventName = GetData<std::string>();
    eventInfo.eventStatus = GetData<bool>();
    eventInfo.minRefreshRate = GetData<uint32_t>();
    eventInfo.maxRefreshRate = GetData<uint32_t>();
    eventInfo.description = GetData<std::string>();
    rsToServiceConn_->NotifyRefreshRateEvent(eventInfo);
    return true;
}

bool DONotifyTouchEvent()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    int32_t touchStatus = GetData<int32_t>();
    int32_t touchCnt = GetData<int32_t>();
    int32_t sourceType = GetData<int32_t>();
    rsToServiceConn_->NotifyTouchEvent(touchStatus, touchCnt, sourceType);
    return true;
}

bool DONotifyDynamicModeEvent()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    bool enableDynamicMode = GetData<bool>();
    rsToServiceConn_->NotifyDynamicModeEvent(enableDynamicMode);
    return true;
}

bool DONotifyHgmConfigEvent()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    std::string eventName = GetData<std::string>();
    bool state = GetData<bool>();
    rsToServiceConn_->NotifyHgmConfigEvent(eventName, state);
    return true;
}

bool DONotifyXComponentExpectedFrameRate()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    std::string id = GetData<std::string>();
    int32_t expectedFrameRate = GetData<int32_t>();
    rsToServiceConn_->NotifyXComponentExpectedFrameRate(id, expectedFrameRate);
    return true;
}

bool DOSetCacheEnabledForRotation()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    bool isEnabled = GetData<bool>();
    rsToServiceConn_->SetCacheEnabledForRotation(isEnabled);
    return true;
}

bool DOSetOnRemoteDiedCallback()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    OnRemoteDiedCallback callback;
    rsToServiceConn_->SetOnRemoteDiedCallback(callback);
    return true;
}

bool DOSetVmaCacheStatus()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    bool flag = GetData<bool>();
    rsToServiceConn_->SetVmaCacheStatus(flag);
    return true;
}

#ifdef TP_FEATURE_ENABLE
bool DOSetTpFeatureConfig()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    int32_t feature = GetData<int32_t>();
    char config = GetData<char>();
    auto tpFeatureConfigType = static_cast<TpFeatureConfigType>(GetData<uint8_t>());
    rsToServiceConn_->SetTpFeatureConfig(feature, &config, tpFeatureConfigType);
    return true;
}
#endif

bool DOSetVirtualScreenUsingStatus()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    bool isVirtualScreenUsingStatus = GetData<bool>();
    rsToServiceConn_->SetVirtualScreenUsingStatus(isVirtualScreenUsingStatus);
    return true;
}

bool DOSetCurtainScreenUsingStatus()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    bool isCurtainScreenOn = GetData<bool>();
    rsToServiceConn_->SetCurtainScreenUsingStatus(isCurtainScreenOn);
    return true;
}

bool DOSetVirtualScreenStatus()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    uint64_t id = GetData<uint64_t>();
    uint64_t screenStatus = GetData<uint64_t>();
    bool success;
    rsToServiceConn_->SetVirtualScreenStatus(id, static_cast<VirtualScreenStatus>(screenStatus), success);
    return true;
}

bool DOSetLayerTop()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    std::string nodeIdStr = GetData<std::string>();
    bool isTop = GetData<bool>();
    rsToServiceConn_->SetLayerTop(nodeIdStr, isTop);
    return true;
}

bool DoSetForceRefresh()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    std::string nodeIdStr = GetData<std::string>();
    bool isForceRefresh = GetData<bool>();
    rsToServiceConn_->SetForceRefresh(nodeIdStr, isForceRefresh);
    return true;
}

bool DOSetFreeMultiWindowStatus()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }

    bool enable = GetData<bool>();
    rsToServiceConn_->SetFreeMultiWindowStatus(enable);
    return true;
}

bool DoNotifySoftVsyncEvent()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    uint32_t pid = GetData<uint32_t>();
    uint32_t rateDiscount = GetData<uint32_t>();
    rsToServiceConn_->NotifySoftVsyncEvent(pid, rateDiscount);
    return true;
}

bool DoCreatePixelMapFromSurface()
{
    sptr<IConsumerSurface> cSurface = IConsumerSurface::Create("FuzzTest");
    sptr<IBufferProducer> bp = cSurface->GetProducer();
    sptr<Surface> pSurface = Surface::CreateSurfaceAsProducer(bp);
    if (pSurface == nullptr) {
        return false;
    }

    auto srcRect = Rect {
        .x = GetData<int32_t>(),
        .y = GetData<int32_t>(),
        .w = GetData<int32_t>(),
        .h = GetData<int32_t>(),
    };
    std::shared_ptr<Media::PixelMap> pixelMap = nullptr;
    rsToServiceConn_->CreatePixelMapFromSurface(pSurface, srcRect, pixelMap);
    return true;
}

#ifdef RS_ENABLE_OVERLAY_DISPLAY
bool DoSetOverlayDisplayMode()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    int32_t mode = GetData<int32_t>();
    rsToServiceConn_->SetOverlayDisplayMode(mode);
    return true;
}
#endif

bool DoSetBehindWindowFilterEnabled()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    bool enabled = GetData<bool>();
    rsToServiceConn_->SetBehindWindowFilterEnabled(enabled);
    return true;
}

bool DoGetBehindWindowFilterEnabled()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    bool res = true;
    rsToServiceConn_->GetBehindWindowFilterEnabled(res);
    return true;
}

bool DoProfilerServiceOpenFile()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    int32_t fd = 0;
    HrpServiceDirInfo dirInfo{HrpServiceDir::HRP_SERVICE_DIR_UNKNOWN, "", ""};
    rsToServiceConn_->ProfilerServiceOpenFile(dirInfo, "", 0, fd);
    return true;
}

bool DoProfilerServicePopulateFiles()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    std::vector<HrpServiceFileInfo> outFiles;
    HrpServiceDirInfo dirInfo{HrpServiceDir::HRP_SERVICE_DIR_UNKNOWN, "", ""};
    rsToServiceConn_->ProfilerServicePopulateFiles(dirInfo, 0, outFiles);
    return true;
}

bool DoProfilerIsSecureScreen()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    rsToServiceConn_->ProfilerIsSecureScreen();
    return true;
}

class CustomFirstFrameCommitCallback : public RSFirstFrameCommitCallbackStub {
public:
    explicit CustomFirstFrameCommitCallback(const FirstFrameCommitCallback& callback) : cb_(callback) {}
    ~CustomFirstFrameCommitCallback() override {};

    void OnFirstFrameCommit(uint64_t screenId, int64_t timestamp) override
    {
        if (cb_ != nullptr) {
            cb_(screenId, timestamp);
        }
    }

private:
    FirstFrameCommitCallback cb_;
};

bool DoRegisterFirstFrameCommitCallback()
{
    if (rsToServiceConn_ == nullptr) {
        return false;
    }
    FirstFrameCommitCallback callback = [](uint64_t screenId, int64_t timestamp) {};
    sptr<CustomFirstFrameCommitCallback> cb = new CustomFirstFrameCommitCallback(callback);
    rsToServiceConn_->RegisterFirstFrameCommitCallback(cb);
    return true;
}

bool DoClearUifirstCache()
{
    if (rsToRenderConn_ == nullptr) {
        return false;
    }

    NodeId id = GetData<NodeId>();
    rsToRenderConn_->ClearUifirstCache(id);
    return true;
}

bool DoTakeSurfaceCaptureWithAllWindows()
{
    if (rsToRenderConn_ == nullptr) {
        return false;
    }
    NodeId nodeId = GetData<NodeId>();
    bool checkDrmAndSurfaceLock = GetData<bool>();
    sptr<RSISurfaceCaptureCallback> callback = nullptr;
    RSSurfaceCaptureConfig captureConfig;
    rsToRenderConn_->TakeSurfaceCaptureWithAllWindows(nodeId, callback, captureConfig, checkDrmAndSurfaceLock);
    return true;
}

bool DoFreezeScreen()
{
    if (rsToRenderConn_ == nullptr) {
        return false;
    }
    NodeId nodeId = GetData<NodeId>();
    bool isFreeze = GetData<bool>();
    rsToRenderConn_->FreezeScreen(nodeId, isFreeze);
    return true;
}

void DoFuzzerTest1()
{
    DoRegisterApplicationAgent();
    DoCommitTransaction();
    DoExecuteSynchronousTask();
    DoGetMemoryGraphic();
    DoCreateNodeAndSurface();
    DoGetScreenBacklight();
    DoGetScreenType();
    DoRegisterBufferAvailableListener();
    DoSetScreenSkipFrameInterval();
    DoSetPhysicalScreenResolution();
    DoSetVirtualScreenResolution();
    DoGetScreenSupportedColorGamuts();
    DoGetScreenSupportedModes();
    DoGetScreenColorGamut();
    DoSetScreenPowerStatus();
    DoSetScreenGamutMap();
    DoSetAppWindowNum();
    DoCreateVirtualScreen();
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
    DoSetPointerColorInversionConfig();
    DoSetPointerColorInversionEnabled();
    DoRegisterPointerLuminanceChangeCallback();
    DoUnRegisterPointerLuminanceChangeCallback();
#endif
    DoSetScreenActiveMode();
    DoSetScreenActiveRect();
    DoSetRefreshRateMode();
    DoCreateVSyncConnection();
    DoSetScreenRefreshRate();
    DoGetBitmap();
    DoGetScreenCapability();
    DoGetScreenData();
    DoGetScreenHDRCapability();
    DoRegisterOcclusionChangeCallback();
#ifdef SUPPORT_ACCESS_TOKEN
    DoShowWatermark();
#endif
    DoTakeSurfaceCapture();
    DoSetHwcNodeBounds();
    DoSetScreenChangeCallback();
    DoSetScreenSwitchingNotifyCallback();
    DoSetFocusAppInfo();
    DoSetAncoForceDoDirect();
    DoGetActiveDirtyRegionInfo();
    DoGetGlobalDirtyRegionInfo();
    DoGetLayerComposeInfo();
    DoGetHwcDisabledReasonInfo();
}

void DoFuzzerTest2()
{
    DoGetUniRenderEnabled();
    DoCreateNode1();
    DoCreateNode2();
    DoGetDefaultScreenId();
    DoGetActiveScreenId();
    DoGetAllScreenIds();
    DoGetMemoryGraphics();
    DoGetTotalAppMemSize();
    DoSetVirtualScreenBlackList();
    DoAddVirtualScreenBlackList();
    DoRemoveVirtualScreenBlackList();
    DoSetVirtualScreenSecurityExemptionList();
    DoSetCastScreenEnableSkipWindow();
    DoSyncFrameRateRange();
    DoUnregisterFrameRateLinker();
    DoMarkPowerOffNeedProcessOneFrame();
    DoDisablePowerOffRenderControl();
    DoSetScreenCorrection();
    DoSetVirtualMirrorScreenCanvasRotation();
    DoSetVirtualMirrorScreenScaleMode();
    DoSetGlobalDarkColorMode();
    DoSetPixelFormat();
    DOGetScreenSupportedHDRFormats();
    DOGetScreenSupportedColorSpaces();
    DOSetVirtualScreenRefreshRate();
    DOSetSystemAnimatedScenes();
    DOResizeVirtualScreen();
    DOReportJankStats();
    DOReportEventResponse();
    DOReportEventComplete();
    DOReportEventJankFrame();
    DOReportGameStateData();
    DOSetHidePrivacyContent();
    DONotifyLightFactorStatus();
    DONotifyPackageEvent();
    DONotifyAppStrategyConfigChangeEvent();
    DONotifyRefreshRateEvent();
    DONotifyTouchEvent();
    DONotifyDynamicModeEvent();
    DOSetCacheEnabledForRotation();
    DOSetOnRemoteDiedCallback();
    DOSetVmaCacheStatus();
#ifdef TP_FEATURE_ENABLE
    DOSetTpFeatureConfig();
#endif
    DOSetVirtualScreenUsingStatus();
    DOSetCurtainScreenUsingStatus();
    DOSetVirtualScreenStatus();
    DOSetLayerTop();
    DoSetForceRefresh();
    DOSetFreeMultiWindowStatus();
}

void DoFuzzerTest3()
{
    DoNotifySoftVsyncEvent();
    DONotifyHgmConfigEvent();
    DONotifyXComponentExpectedFrameRate();
    DoCreatePixelMapFromSurface();
#ifdef RS_ENABLE_OVERLAY_DISPLAY
    DoSetOverlayDisplayMode();
#endif
    DoRegisterFirstFrameCommitCallback();
    DoNotifySoftVsyncRateDiscountEvent();
    DoSetBehindWindowFilterEnabled();
    DoGetBehindWindowFilterEnabled();
    DoSetVirtualScreenAutoRotation();
    DoProfilerServiceOpenFile();
    DoProfilerServicePopulateFiles();
    DoProfilerIsSecureScreen();
    DoClearUifirstCache();
    DoTakeSurfaceCaptureWithAllWindows();
    DoFreezeScreen();
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (!OHOS::Rosen::Init(data, size)) {
        return 0;
    }
    /* Run your code on data */
    OHOS::Rosen::DoFuzzerTest1();
    OHOS::Rosen::DoFuzzerTest2();
    OHOS::Rosen::DoFuzzerTest3();
    return 0;
}