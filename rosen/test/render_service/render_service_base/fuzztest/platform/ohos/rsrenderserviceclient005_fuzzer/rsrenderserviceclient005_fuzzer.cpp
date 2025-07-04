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

#include "rsrenderserviceclient005_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "transaction/rs_render_service_client.h"
#include "command/rs_command.h"
#include "command/rs_node_showing_command.h"
#include "core/transaction/rs_interfaces.h"
#ifdef OHOS_BUILD_ENABLE_MAGICCURSOR
#include "ipc_callbacks/pointer_render/pointer_luminance_callback_stub.h"
#endif
#include "ipc_callbacks/screen_change_callback_stub.h"
#include "platform/ohos/rs_render_service_connect_hub.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t DO_SET_HARDWARE_ENABLED = 0;
const uint8_t DO_SET_HIDE_PRIVACY_CONTENT = 1;
const uint8_t DO_NOTIFY_LIGHT_FACTOR_STATUS = 2;
const uint8_t DO_NOTIFY_PACKAGE_EVENT = 3;
const uint8_t DO_NOTIFY_APP_STRATEGY_CONFIG_CHANGE_EVENT = 4;
const uint8_t DO_NOTIFY_REFRESH_RATE_EVENT = 5;
const uint8_t DO_NOTIFY_SOFT_VSYNC_RATE_DISCOUNT_EVENT = 6;
const uint8_t DO_NOTIFY_TOUCH_EVENT = 7;
const uint8_t DO_SET_CACHE_ENABLED_FOR_ROTATION = 8;
const uint8_t DO_NOTIFY_HGMCONFIG_EVENT = 9;
const uint8_t DO_SET_ON_REMOTE_DIED_CALLBACK = 10;
const uint8_t DO_GET_ACTIVE_DIRTY_REGION_INFO = 11;
const uint8_t DO_SET_VMA_CACHE_STATUS = 12;
const uint8_t DO_REGISTER_UIEXTENSION_CALLBACK = 13;
const uint8_t DO_SET_ANCO_FORCE_DO_DIRECT = 14;
const uint8_t DO_SET_VIRTUAL_SCREEN_STATUS = 15;
const uint8_t DO_REGISTER_SURFACE_BUFFER_CALLBACK = 16;
const uint8_t DO_TRIGGER_SURFACE_BUFFER_CALLBACK = 17;
const uint8_t DO_SET_LAYER_TOP = 18;
const uint8_t TARGET_SIZE = 19;

sptr<RSIRenderServiceConnection> CONN = nullptr;
const uint8_t* DATA = nullptr;
size_t g_size = 0;
size_t g_pos;
constexpr size_t STR_LEN = 10;
constexpr uint8_t SELF_DRAWING_TYPE_SIZE = 3;
constexpr uint8_t VIRTUAL_SCREEN_STATUS_SIZE = 3;


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

/*
 * get a string from g_data
 */
std::string GetStringFromData(int strlen)
{
    if (strlen <= 0) {
        return "fuzz";
    }
    char cstr[strlen];
    cstr[strlen - 1] = '\0';
    for (int i = 0; i < strlen - 1; i++) {
        char tmp = GetData<char>();
        if (tmp == '\0') {
            tmp = '1';
        }
        cstr[i] = tmp;
    }
    std::string str(cstr);
    return str;
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

bool DoSetHardwareEnabled()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    NodeId id = GetData<NodeId>();
    bool isEnabled = GetData<bool>();
    SelfDrawingNodeType selfDrawingType = static_cast<SelfDrawingNodeType>(GetData<uint8_t>() % SELF_DRAWING_TYPE_SIZE);
    bool dynamicHardwareEnable = GetData<bool>();
    client->SetHardwareEnabled(id, isEnabled, selfDrawingType, dynamicHardwareEnable);
    return true;
}

bool DoSetHidePrivacyContent()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    NodeId id = GetData<NodeId>();
    bool needHidePrivacyContent = GetData<bool>();
    client->SetHidePrivacyContent(id, needHidePrivacyContent);
    return true;
}

bool DoNotifyLightFactorStatus()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    int32_t lightFactorStatus = GetData<int32_t>();
    client->NotifyLightFactorStatus(lightFactorStatus);
    return true;
}

bool DoNotifyPackageEvent()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    uint32_t listSize = GetData<uint32_t>();
    std::string data = GetStringFromData(STR_LEN);
    std::vector<std::string> packageList = { data };
    client->NotifyPackageEvent(listSize, packageList);
    return true;
}

bool DoNotifyAppStrategyConfigChangeEvent()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    std::string pkgName = GetData<std::string>();
    uint8_t listSize = GetData<uint8_t>();
    std::vector<std::pair<std::string, std::string>> newConfig;
    for (auto i = 0; i < listSize; i++) {
        std::string configKey = GetData<std::string>();
        std::string configValue = GetData<std::string>();
        newConfig.push_back(make_pair(configKey, configValue));
    }
    client->NotifyAppStrategyConfigChangeEvent(pkgName, listSize, newConfig);
    return true;
}

bool DoNotifyRefreshRateEvent()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    EventInfo eventInfo = {
        .eventName = GetData<std::string>(),
        .eventStatus = GetData<bool>(),
        .minRefreshRate = GetData<uint32_t>(),
        .maxRefreshRate = GetData<uint32_t>(),
        .description = GetData<std::string>()
    };
    client->NotifyRefreshRateEvent(eventInfo);
    return true;
}

bool DoNotifySoftVsyncRateDiscountEvent()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    uint32_t pid = GetData<uint32_t>();
    std::string name = GetData<std::string>();
    uint32_t rateDiscount = GetData<uint32_t>();
    client->NotifySoftVsyncRateDiscountEvent(pid, name, rateDiscount);
    return true;
}

bool DoNotifyTouchEvent()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    int32_t touchStatus = GetData<int32_t>();
    int32_t touchCnt = GetData<int32_t>();
    bool enableDynamicMode = GetData<bool>();
    client->NotifyTouchEvent(touchStatus, touchCnt);
    client->NotifyDynamicModeEvent(enableDynamicMode);
    return true;
}

bool DoSetCacheEnabledForRotation()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    bool isEnabled = GetData<bool>();
    client->SetCacheEnabledForRotation(isEnabled);
    return true;
}

bool DoNotifyHgmConfigEvent()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    std::string eventName = GetData<std::string>();
    bool state = GetData<bool>();
    client->NotifyHgmConfigEvent(eventName, state);
    return true;
}

bool DoSetOnRemoteDiedCallback()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    OnRemoteDiedCallback callback;
    client->SetOnRemoteDiedCallback(callback);
    return true;
}

bool DoGetActiveDirtyRegionInfo()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    client->GetActiveDirtyRegionInfo();
    client->GetGlobalDirtyRegionInfo();
    client->GetLayerComposeInfo();
    client->GetHwcDisabledReasonInfo();
    return true;
}

bool DoSetVmaCacheStatus()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    bool flag = GetData<bool>();
    bool isVirtualScreenUsingStatus = GetData<bool>();
    bool isCurtainScreenOn = GetData<bool>();
    client->SetVmaCacheStatus(flag);
    client->SetVirtualScreenUsingStatus(isVirtualScreenUsingStatus);
    client->SetCurtainScreenUsingStatus(isCurtainScreenOn);
    return true;
}

bool DoRegisterUIExtensionCallback()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    uint64_t userId = GetData<uint64_t>();
    UIExtensionCallback callback;
    client->RegisterUIExtensionCallback(userId, callback);
    return true;
}

bool DoSetAncoForceDoDirect()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    bool direct = GetData<bool>();
    client->SetAncoForceDoDirect(direct);
    return true;
}

bool DoSetVirtualScreenStatus()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    ScreenId id = GetData<ScreenId>();
    VirtualScreenStatus screenStatus =
        static_cast<VirtualScreenStatus>(GetData<uint8_t>() % VIRTUAL_SCREEN_STATUS_SIZE);
    client->SetVirtualScreenStatus(id, screenStatus);
    return true;
}

bool DoRegisterSurfaceBufferCallback()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    pid_t pid = GetData<pid_t>();
    uint64_t uid = GetData<uint64_t>();
    std::shared_ptr<SurfaceBufferCallback> callback;
    client->RegisterSurfaceBufferCallback(pid, uid, callback);
    client->UnregisterSurfaceBufferCallback(pid, uid);
    return true;
}

bool DoTriggerSurfaceBufferCallback()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    uint64_t uid = GetData<uint64_t>();
    std::vector<uint32_t> surfaceBufferIds;
    std::vector<uint8_t> isRenderedFlags;
    client->TriggerOnFinish({
        .uid = uid,
        .surfaceBufferIds = surfaceBufferIds,
        .isRenderedFlags = isRenderedFlags,
    });
    return true;
}

bool DoSetLayerTop()
{
    std::shared_ptr<RSRenderServiceClient> client = std::make_shared<RSRenderServiceClient>();
    std::string nodeIdStr = GetData<std::string>();
    bool isTop = GetData<bool>();
    client->SetLayerTop(nodeIdStr, isTop);
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    if (!OHOS::Rosen::Init(data, size)) {
        return -1;
    }
    uint8_t tarPos = OHOS::Rosen::GetData<uint8_t>() % OHOS::Rosen::TARGET_SIZE;

    switch (tarPos) {
        case OHOS::Rosen::DO_SET_HARDWARE_ENABLED:
            OHOS::Rosen::DoSetHardwareEnabled();
            break;
        case OHOS::Rosen::DO_SET_HIDE_PRIVACY_CONTENT:
            OHOS::Rosen::DoSetHidePrivacyContent();
            break;
        case OHOS::Rosen::DO_NOTIFY_LIGHT_FACTOR_STATUS:
            OHOS::Rosen::DoNotifyLightFactorStatus();
            break;
        case OHOS::Rosen::DO_NOTIFY_PACKAGE_EVENT:
            OHOS::Rosen::DoNotifyPackageEvent();
            break;
        case OHOS::Rosen::DO_NOTIFY_APP_STRATEGY_CONFIG_CHANGE_EVENT:
            OHOS::Rosen::DoNotifyAppStrategyConfigChangeEvent();
            break;
        case OHOS::Rosen::DO_NOTIFY_REFRESH_RATE_EVENT:
            OHOS::Rosen::DoNotifyRefreshRateEvent();
            break;
        case OHOS::Rosen::DO_NOTIFY_SOFT_VSYNC_RATE_DISCOUNT_EVENT:
            OHOS::Rosen::DoNotifySoftVsyncRateDiscountEvent();
            break;
        case OHOS::Rosen::DO_NOTIFY_TOUCH_EVENT:
            OHOS::Rosen::DoNotifyTouchEvent();
            break;
        case OHOS::Rosen::DO_SET_CACHE_ENABLED_FOR_ROTATION:
            OHOS::Rosen::DoSetCacheEnabledForRotation();
            break;
        case OHOS::Rosen::DO_NOTIFY_HGMCONFIG_EVENT:
            OHOS::Rosen::DoNotifyHgmConfigEvent();
            break;
        case OHOS::Rosen::DO_SET_ON_REMOTE_DIED_CALLBACK:
            OHOS::Rosen::DoSetOnRemoteDiedCallback();
            break;
        case OHOS::Rosen::DO_GET_ACTIVE_DIRTY_REGION_INFO:
            OHOS::Rosen::DoGetActiveDirtyRegionInfo();
            break;
        case OHOS::Rosen::DO_SET_VMA_CACHE_STATUS:
            OHOS::Rosen::DoSetVmaCacheStatus();
            break;
        case OHOS::Rosen::DO_REGISTER_UIEXTENSION_CALLBACK:
            OHOS::Rosen::DoRegisterUIExtensionCallback();
            break;
        case OHOS::Rosen::DO_SET_ANCO_FORCE_DO_DIRECT:
            OHOS::Rosen::DoSetAncoForceDoDirect();
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_SCREEN_STATUS:
            OHOS::Rosen::DoSetVirtualScreenStatus();
            break;
        case OHOS::Rosen::DO_REGISTER_SURFACE_BUFFER_CALLBACK:
            OHOS::Rosen::DoRegisterSurfaceBufferCallback();
            break;
        case OHOS::Rosen::DO_TRIGGER_SURFACE_BUFFER_CALLBACK:
            OHOS::Rosen::DoTriggerSurfaceBufferCallback();
            break;
        case OHOS::Rosen::DO_SET_LAYER_TOP:
            OHOS::Rosen::DoSetLayerTop();
            break;
        default:
            return -1;
    }
    return 0;
}