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

#include "notifyEvents_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>
#include <string>
#include <vector>

#include "transaction/rs_interfaces.h"

namespace OHOS {
namespace Rosen {

// Global RSInterfaces instance, initialized in LLVMFuzzerInitialize
RSInterfaces* g_rsInterfaces = nullptr;

namespace {
const uint8_t DO_NOTIFY_LIGHT_FACTOR_STATUS = 0;
const uint8_t DO_NOTIFY_PACKAGE_EVENT = 1;
const uint8_t DO_NOTIFY_REFRESH_RATE_EVENT = 2;
const uint8_t DO_NOTIFY_DYNAMIC_MODE_EVENT = 3;
const uint8_t DO_NOTIFY_APP_STRATEGY_CONFIG_CHANGE_EVENT = 4;
const uint8_t DO_NOTIFY_SOFT_VSYNC_RATE_DISCOUNT_EVENT = 5;
const uint8_t DO_NOTIFY_TOUCH_EVENT = 6;
const uint8_t DO_NOTIFY_XCOMPONENT_EXPECTED_FRAMERATE = 7;
const uint8_t TARGET_SIZE = 8;
constexpr size_t STR_LEN = 10;

void DoNotifyLightFactorStatus(FuzzedDataProvider& fdp)
{
    int32_t lightFactorStatus = fdp.ConsumeIntegral<int32_t>();
    g_rsInterfaces->NotifyLightFactorStatus(lightFactorStatus);
}

void DoNotifyPackageEvent(FuzzedDataProvider& fdp)
{
    uint8_t listSize = fdp.ConsumeIntegral<uint8_t>();
    std::vector<std::string> packageList;
    for (auto i = 0; i < listSize; i++) {
        packageList.push_back(fdp.ConsumeRandomLengthString(STR_LEN));
    }
    g_rsInterfaces->NotifyPackageEvent(listSize, packageList);
}

void DoNotifyRefreshRateEvent(FuzzedDataProvider& fdp)
{
    EventInfo eventInfo = {
        .eventName = fdp.ConsumeRandomLengthString(STR_LEN),
        .eventStatus = fdp.ConsumeBool(),
        .minRefreshRate = fdp.ConsumeIntegral<uint32_t>(),
        .maxRefreshRate = fdp.ConsumeIntegral<uint32_t>(),
        .description = fdp.ConsumeRandomLengthString(STR_LEN),
    };
    g_rsInterfaces->NotifyRefreshRateEvent(eventInfo);
}

void DoNotifyDynamicModeEvent(FuzzedDataProvider& fdp)
{
    bool enableDynamicMode = fdp.ConsumeBool();
    g_rsInterfaces->NotifyDynamicModeEvent(enableDynamicMode);
}

void DoNotifyAppStrategyConfigChangeEvent(FuzzedDataProvider& fdp)
{
    std::string pkgName = fdp.ConsumeRandomLengthString(STR_LEN);
    uint8_t listSize = fdp.ConsumeIntegral<uint8_t>();
    std::vector<std::pair<std::string, std::string>> newConfig;
    for (auto i = 0; i < listSize; i++) {
        std::string configKey = fdp.ConsumeRandomLengthString(STR_LEN);
        std::string configValue = fdp.ConsumeRandomLengthString(STR_LEN);
        newConfig.push_back(make_pair(configKey, configValue));
    }
    g_rsInterfaces->NotifyAppStrategyConfigChangeEvent(pkgName, listSize, newConfig);
}

void DoNotifySoftVsyncRateDiscountEvent(FuzzedDataProvider& fdp)
{
    uint32_t pid = fdp.ConsumeIntegral<uint32_t>();
    std::string name = fdp.ConsumeRandomLengthString(STR_LEN);
    uint32_t rateDiscount = fdp.ConsumeIntegral<uint32_t>();
    g_rsInterfaces->NotifySoftVsyncRateDiscountEvent(pid, name, rateDiscount);
}

void DoNotifyTouchEvent(FuzzedDataProvider& fdp)
{
    int32_t touchStatus = fdp.ConsumeIntegral<int32_t>();
    int32_t touchCnt = fdp.ConsumeIntegral<int32_t>();
    int32_t sourceType = fdp.ConsumeIntegral<int32_t>();
    g_rsInterfaces->NotifyTouchEvent(touchStatus, touchCnt, sourceType);
}

void DoNotifyXComponentExpectedFrameRate(FuzzedDataProvider& fdp)
{
    std::string id = fdp.ConsumeRandomLengthString(STR_LEN);
    int32_t expectedFrameRate = fdp.ConsumeIntegral<int32_t>();
    g_rsInterfaces->NotifyXComponentExpectedFrameRate(id, expectedFrameRate);
}

} // namespace

} // namespace Rosen
} // namespace OHOS

/* Fuzzer environment initialization */
extern "C" int LLVMFuzzerInitialize(int *argc, char ***argv)
{
    // Pre-initialize RSInterfaces singleton to avoid runtime initialization overhead
    OHOS::Rosen::g_rsInterfaces = &OHOS::Rosen::RSInterfaces::GetInstance();
    return 0;
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (OHOS::Rosen::g_rsInterfaces == nullptr || data == nullptr) {
        return -1;
    }

    FuzzedDataProvider fdp(data, size);

    /* Run your code on data */
    uint8_t tarPos = fdp.ConsumeIntegral<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_NOTIFY_LIGHT_FACTOR_STATUS:
            OHOS::Rosen::DoNotifyLightFactorStatus(fdp);
            break;
        case OHOS::Rosen::DO_NOTIFY_PACKAGE_EVENT:
            OHOS::Rosen::DoNotifyPackageEvent(fdp);
            break;
        case OHOS::Rosen::DO_NOTIFY_REFRESH_RATE_EVENT:
            OHOS::Rosen::DoNotifyRefreshRateEvent(fdp);
            break;
        case OHOS::Rosen::DO_NOTIFY_DYNAMIC_MODE_EVENT:
            OHOS::Rosen::DoNotifyDynamicModeEvent(fdp);
            break;
        case OHOS::Rosen::DO_NOTIFY_APP_STRATEGY_CONFIG_CHANGE_EVENT:
            OHOS::Rosen::DoNotifyAppStrategyConfigChangeEvent(fdp);
            break;
        case OHOS::Rosen::DO_NOTIFY_SOFT_VSYNC_RATE_DISCOUNT_EVENT:
            OHOS::Rosen::DoNotifySoftVsyncRateDiscountEvent(fdp);
            break;
        case OHOS::Rosen::DO_NOTIFY_TOUCH_EVENT:
            OHOS::Rosen::DoNotifyTouchEvent(fdp);
            break;
        case OHOS::Rosen::DO_NOTIFY_XCOMPONENT_EXPECTED_FRAMERATE:
            OHOS::Rosen::DoNotifyXComponentExpectedFrameRate(fdp);
            break;
        default:
            return -1;
    }
    return 0;
}
