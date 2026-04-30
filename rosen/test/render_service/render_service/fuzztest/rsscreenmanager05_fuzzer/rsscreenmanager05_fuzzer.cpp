/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "rsscreenmanager05_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>
#include <memory>

#include "screen_manager/rs_screen_manager.h"
#include "screen_manager/screen_types.h"

namespace OHOS {
namespace Rosen {

sptr<RSScreenManager> g_screenManager;

namespace {
constexpr uint8_t DO_SET_CAST_SCREEN_ENABLE_SKIP_WINDOW = 0;
constexpr uint8_t DO_ADD_VIRTUAL_SCREEN_BLACK_LIST = 1;
constexpr uint8_t DO_REMOVE_VIRTUAL_SCREEN_BLACK_LIST = 2;
constexpr uint8_t DO_SET_VIRTUAL_SCREEN_TYPE_BLACK_LIST = 3;
constexpr uint8_t DO_REMOVE_VIRTUAL_SCREEN_WHITE_LIST = 4;
constexpr uint8_t DO_SET_VIRTUAL_SCREEN_SECURITY_EXEMPTION_LIST = 5;
constexpr uint8_t DO_SET_VIRTUAL_SCREEN_REFRESH_RATE = 6;
constexpr uint8_t DO_DISABLE_POWER_OFF_RENDER_CONTROL = 7;
constexpr uint8_t DO_UPDATE_VSYNC_ENABLED_SCREEN_ID = 8;
constexpr uint8_t DO_JUDGE_VSYNC_ENABLED_SCREEN_WHILE_POWER_STATUS_CHANGED = 9;
constexpr uint8_t TARGET_SIZE = 10;

constexpr uint8_t MAX_FUZZ_LIST_SIZE = 8;
constexpr uint8_t MAX_FUZZ_TYPE_LIST_SIZE = 31;
constexpr uint8_t SCREEN_POWER_STATUS_SIZE = 11;

void DoSetCastScreenEnableSkipWindow(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    bool enable = fdp.ConsumeBool();
    g_screenManager->SetCastScreenEnableSkipWindow(id, enable);
}

void DoAddVirtualScreenBlackList(FuzzedDataProvider& fdp)
{
    fdp.ConsumeBool();
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    uint8_t count = fdp.ConsumeIntegral<uint8_t>() % MAX_FUZZ_LIST_SIZE;
    std::vector<uint64_t> bList;
    for (uint8_t i = 0; i < count; i++) {
        bList.push_back(fdp.ConsumeIntegral<uint64_t>());
    }
    g_screenManager->AddVirtualScreenBlackList(id, bList);
}

void DoRemoveVirtualScreenBlackList(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    uint8_t count = fdp.ConsumeIntegral<uint8_t>() % MAX_FUZZ_LIST_SIZE;
    std::vector<uint64_t> bList;
    for (uint8_t i = 0; i < count; i++) {
        bList.push_back(fdp.ConsumeIntegral<uint64_t>());
    }
    g_screenManager->RemoveVirtualScreenBlackList(id, bList);
}

void DoSetVirtualScreenTypeBlackList(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegralInRange<ScreenId>(0, 255);
    uint8_t count = fdp.ConsumeIntegral<uint8_t>() % MAX_FUZZ_TYPE_LIST_SIZE;
    std::vector<uint8_t> typeBList;
    for (uint8_t i = 0; i < count; i++) {
        typeBList.push_back(fdp.ConsumeIntegral<uint8_t>());
    }
    g_screenManager->SetVirtualScreenTypeBlackList(id, typeBList);
}

void DoRemoveVirtualScreenWhiteList(FuzzedDataProvider& fdp)
{
    fdp.ConsumeIntegral<uint8_t>();
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    uint8_t count = fdp.ConsumeIntegral<uint8_t>() % MAX_FUZZ_LIST_SIZE;
    std::vector<NodeId> whiteList;
    for (uint8_t i = 0; i < count; i++) {
        whiteList.push_back(fdp.ConsumeIntegral<NodeId>());
    }
    g_screenManager->RemoveVirtualScreenWhiteList(id, whiteList);
}

void DoSetVirtualScreenSecurityExemptionList(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    fdp.ConsumeBool();
    uint8_t count = fdp.ConsumeIntegral<uint8_t>() % MAX_FUZZ_LIST_SIZE;
    std::vector<uint64_t> securityExemptionList;
    for (uint8_t i = 0; i < count; i++) {
        securityExemptionList.push_back(fdp.ConsumeIntegral<uint64_t>());
    }
    g_screenManager->SetVirtualScreenSecurityExemptionList(id, securityExemptionList);
}

void DoSetVirtualScreenRefreshRate(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    uint32_t maxRefreshRate = fdp.ConsumeIntegral<uint32_t>();
    uint32_t actualRefreshRate = 0;
    g_screenManager->SetVirtualScreenRefreshRate(id, maxRefreshRate, actualRefreshRate);
}

void DoDisablePowerOffRenderControl(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegralInRange<ScreenId>(0, INVALID_SCREEN_ID - 1);
    g_screenManager->DisablePowerOffRenderControl(id);
}

void DoUpdateVsyncEnabledScreenId(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegral<ScreenId>();
    g_screenManager->UpdateVsyncEnabledScreenId(id);
}

void DoJudgeVSyncEnabledScreenWhilePowerStatusChanged(FuzzedDataProvider& fdp)
{
    ScreenId screenId = fdp.ConsumeIntegralInRange<ScreenId>(0, 255);
    ScreenPowerStatus status = static_cast<ScreenPowerStatus>(
        fdp.ConsumeIntegral<uint8_t>() % SCREEN_POWER_STATUS_SIZE);
    uint64_t enabledScreenId = fdp.ConsumeIntegral<uint64_t>();
    g_screenManager->JudgeVSyncEnabledScreenWhilePowerStatusChanged(screenId, status, enabledScreenId);
}

} // namespace

} // namespace Rosen
} // namespace OHOS

extern "C" int LLVMFuzzerInitialize(int* argc, char*** argv)
{
    OHOS::Rosen::g_screenManager = OHOS::sptr<OHOS::Rosen::RSScreenManager>::MakeSptr();
    if (!OHOS::Rosen::g_screenManager) {
        return -1;
    }
    auto runner = OHOS::AppExecFwk::EventRunner::Create(false);
    auto handler = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner);
    OHOS::Rosen::g_screenManager->Init(handler);
    return 0;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return -1;
    }
    if (!OHOS::Rosen::g_screenManager) {
        return -1;
    }

    FuzzedDataProvider fdp(data, size);

    uint8_t tarPos = fdp.ConsumeIntegral<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_SET_CAST_SCREEN_ENABLE_SKIP_WINDOW:
            OHOS::Rosen::DoSetCastScreenEnableSkipWindow(fdp);
            break;
        case OHOS::Rosen::DO_ADD_VIRTUAL_SCREEN_BLACK_LIST:
            OHOS::Rosen::DoAddVirtualScreenBlackList(fdp);
            break;
        case OHOS::Rosen::DO_REMOVE_VIRTUAL_SCREEN_BLACK_LIST:
            OHOS::Rosen::DoRemoveVirtualScreenBlackList(fdp);
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_SCREEN_TYPE_BLACK_LIST:
            OHOS::Rosen::DoSetVirtualScreenTypeBlackList(fdp);
            break;
        case OHOS::Rosen::DO_REMOVE_VIRTUAL_SCREEN_WHITE_LIST:
            OHOS::Rosen::DoRemoveVirtualScreenWhiteList(fdp);
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_SCREEN_SECURITY_EXEMPTION_LIST:
            OHOS::Rosen::DoSetVirtualScreenSecurityExemptionList(fdp);
            break;
        case OHOS::Rosen::DO_SET_VIRTUAL_SCREEN_REFRESH_RATE:
            OHOS::Rosen::DoSetVirtualScreenRefreshRate(fdp);
            break;
        case OHOS::Rosen::DO_DISABLE_POWER_OFF_RENDER_CONTROL:
            OHOS::Rosen::DoDisablePowerOffRenderControl(fdp);
            break;
        case OHOS::Rosen::DO_UPDATE_VSYNC_ENABLED_SCREEN_ID:
            OHOS::Rosen::DoUpdateVsyncEnabledScreenId(fdp);
            break;
        case OHOS::Rosen::DO_JUDGE_VSYNC_ENABLED_SCREEN_WHILE_POWER_STATUS_CHANGED:
            OHOS::Rosen::DoJudgeVSyncEnabledScreenWhilePowerStatusChanged(fdp);
            break;
        default:
            return -1;
    }
    return 0;
}