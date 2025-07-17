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

#include "rsinterfacesFFF_fuzzer.h"

#include <securec.h>

#include "transaction/rs_interfaces.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t DO_NOTIFY_LIGHT_FACTOR_STATUS = 0;
const uint8_t DO_NOTIFY_PACKAGE_EVENT = 1;
const uint8_t DO_NOTIFY_REFRESH_RATE_EVENT = 2;
const uint8_t DO_NOTIFY_DYNAMIC_MODE_EVENT = 3;
const uint8_t DO_NOTIFY_APP_STRATEGY_CONFIG_CHANGE_EVENT = 4;
const uint8_t DO_NOTIFY_HGMCONFIG_EVENT = 5;
const uint8_t DO_NOTIFY_SOFT_VSYNC_RATE_DISCOUNT_EVENT = 6;
const uint8_t TARGET_SIZE = 7;

const uint8_t* DATA = nullptr;
size_t g_size = 0;
size_t g_pos;
constexpr size_t STR_LEN = 10;

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

namespace Mock {

} // namespace Mock

void DoNotifyLightFactorStatus()
{
    int32_t lightFactorStatus = GetData<int32_t>();
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.NotifyLightFactorStatus(lightFactorStatus);
}

void DoNotifyPackageEvent()
{
    uint32_t listSize = GetData<uint32_t>();
    std::string data = GetStringFromData(STR_LEN);
    std::vector<std::string> packageList = { data };
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.NotifyPackageEvent(listSize, packageList);
}

void DoNotifyRefreshRateEvent()
{
    EventInfo eventInfo = {
        .eventName = GetData<std::string>(),
        .eventStatus = GetData<bool>(),
        .minRefreshRate = GetData<uint32_t>(),
        .maxRefreshRate = GetData<uint32_t>(),
        .description = GetStringFromData(STR_LEN),
    };
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.NotifyRefreshRateEvent(eventInfo);
}

void DoNotifyDynamicModeEvent()
{
    bool enableDynamicMode = GetData<bool>();
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.NotifyDynamicModeEvent(enableDynamicMode);
}

void DoNotifyAppStrategyConfigChangeEvent()
{
    std::string pkgName = GetStringFromData(STR_LEN);
    uint8_t listSize = GetData<uint8_t>();
    std::vector<std::pair<std::string, std::string>> newConfig;
    for (auto i = 0; i < listSize; i++) {
        std::string configKey = GetStringFromData(STR_LEN);
        std::string configValue = GetStringFromData(STR_LEN);
        newConfig.push_back(make_pair(configKey, configValue));
    }
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.NotifyAppStrategyConfigChangeEvent(pkgName, listSize, newConfig);
}

void DoNotifyHgmConfigEvent()
{
    std::string eventName = GetStringFromData(STR_LEN);
    bool state = GetData<bool>();
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.NotifyHgmConfigEvent(eventName, state);
}

void DoNotifySoftVsyncRateDiscountEvent()
{
    uint32_t pid = GetData<uint32_t>();
    std::string name = GetStringFromData(STR_LEN);
    uint32_t rateDiscount = GetData<uint32_t>();
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.NotifySoftVsyncRateDiscountEvent(pid, name, rateDiscount);
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (!OHOS::Rosen::Init(data, size)) {
        return -1;
    }
    /* Run your code on data */
    uint8_t tarPos = OHOS::Rosen::GetData<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_NOTIFY_LIGHT_FACTOR_STATUS:
            OHOS::Rosen::DoNotifyLightFactorStatus();
            break;
        case OHOS::Rosen::DO_NOTIFY_PACKAGE_EVENT:
            OHOS::Rosen::DoNotifyPackageEvent();
            break;
        case OHOS::Rosen::DO_NOTIFY_REFRESH_RATE_EVENT:
            OHOS::Rosen::DoNotifyRefreshRateEvent();
            break;
        case OHOS::Rosen::DO_NOTIFY_DYNAMIC_MODE_EVENT:
            OHOS::Rosen::DoNotifyDynamicModeEvent();
            break;
        case OHOS::Rosen::DO_NOTIFY_APP_STRATEGY_CONFIG_CHANGE_EVENT:
            OHOS::Rosen::DoNotifyAppStrategyConfigChangeEvent();
            break;
        case OHOS::Rosen::DO_NOTIFY_HGMCONFIG_EVENT:
            OHOS::Rosen::DoNotifyHgmConfigEvent();
            break;
        case OHOS::Rosen::DO_NOTIFY_SOFT_VSYNC_RATE_DISCOUNT_EVENT:
            OHOS::Rosen::DoNotifySoftVsyncRateDiscountEvent();
            break;
        default:
            return -1;
    }
    return 0;
}
