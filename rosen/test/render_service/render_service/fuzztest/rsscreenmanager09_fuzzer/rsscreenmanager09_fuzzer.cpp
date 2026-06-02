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

#include "rsscreenmanager09_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>
#include <memory>
#include <string>

#include "screen_manager/rs_screen_manager.h"
#include "screen_manager/screen_types.h"

namespace OHOS {
namespace Rosen {

sptr<RSScreenManager> g_screenManager;

namespace {
constexpr uint8_t DO_SET_VIRTUAL_SCREEN_SURFACE = 0;
constexpr uint8_t DO_SET_SCREEN_SECURITY_MASK = 1;
constexpr uint8_t DO_REGISTER_CORE_LISTENER = 2;
constexpr uint8_t DO_REGISTER_AGENT_LISTENER = 3;
constexpr uint8_t DO_UN_REGISTER_AGENT_LISTENER = 4;
constexpr uint8_t DO_ON_ACTIVE_SCREEN_ID_CHANGED_CALLBACK_CHANGED = 5;
constexpr uint8_t DO_ON_SCREEN_CHANGE_CALLBACK_CHANGED = 6;
constexpr uint8_t DO_MOCK_HDI_SCREEN_CONNECTED = 7;
constexpr uint8_t DO_ON_PROCESS_DISCONNECTED = 8;
constexpr uint8_t TARGET_SIZE = 9;

constexpr uint8_t MAX_FUZZ_DISCONNECTED_LIST_SIZE = 8;
constexpr uint8_t FUZZ_CALLBACK_TYPE_RANGE_MAX = 7;
constexpr uint32_t FUZZ_STRING_OFFSET_LEN = 16;

ScreenId FuzzConsumeScreenId(FuzzedDataProvider& fdp)
{
    bool useInvalidId = fdp.ConsumeBool();
    if (useInvalidId) {
        return INVALID_SCREEN_ID;
    }
    return fdp.ConsumeIntegral<ScreenId>();
}

void DoSetVirtualScreenSurface(FuzzedDataProvider& fdp)
{
    fdp.ConsumeBool();
    ScreenId id = fdp.ConsumeIntegralInRange<ScreenId>(0, INVALID_SCREEN_ID - 1);
    sptr<Surface> surface;
    g_screenManager->SetVirtualScreenSurface(id, surface);
}

void DoSetScreenSecurityMask(FuzzedDataProvider& fdp)
{
    ScreenId id = FuzzConsumeScreenId(fdp);
    fdp.ConsumeIntegral<uint32_t>();
    std::shared_ptr<Media::PixelMap> securityMask;
    g_screenManager->SetScreenSecurityMask(id, securityMask);
}

void DoRegisterCoreListener(FuzzedDataProvider& fdp)
{
    fdp.ConsumeBool();
    sptr<RSIScreenManagerListener> listener;
    g_screenManager->RegisterCoreListener(listener);
}

void DoRegisterAgentListener(FuzzedDataProvider& fdp)
{
    fdp.ConsumeIntegral<uint16_t>();
    sptr<RSIScreenManagerAgentListener> listener;
    g_screenManager->RegisterAgentListener(listener);
}

void DoUnRegisterAgentListener(FuzzedDataProvider& fdp)
{
    fdp.ConsumeIntegral<uint8_t>();
    sptr<RSIScreenManagerAgentListener> listener;
    g_screenManager->UnRegisterAgentListener(listener);
}

void DoOnActiveScreenIdChangedCallbackChanged(FuzzedDataProvider& fdp)
{
    fdp.ConsumeIntegralInRange<uint8_t>(0, FUZZ_CALLBACK_TYPE_RANGE_MAX);
    sptr<RSIScreenManagerAgentListener> agentListener;
    g_screenManager->OnActiveScreenIdChangedCallbackChanged(agentListener);
}

void DoOnScreenChangeCallbackChanged(FuzzedDataProvider& fdp)
{
    fdp.ConsumeRandomLengthString(FUZZ_STRING_OFFSET_LEN);
    sptr<RSIScreenManagerAgentListener> agentListener;
    g_screenManager->OnScreenChangeCallbackChanged(agentListener);
}

void DoMockHdiScreenConnected(FuzzedDataProvider& fdp)
{
    fdp.ConsumeIntegral<uint64_t>();
    std::shared_ptr<RSScreen> rsScreen;
    g_screenManager->MockHdiScreenConnected(rsScreen);
}

void DoOnProcessDisconnected(FuzzedDataProvider& fdp)
{
    uint8_t count = fdp.ConsumeIntegral<uint8_t>() % MAX_FUZZ_DISCONNECTED_LIST_SIZE;
    std::vector<std::pair<ScreenId, std::shared_ptr<HdiOutput>>> screens;
    for (uint8_t i = 0; i < count; i++) {
        ScreenId screenId = fdp.ConsumeIntegral<ScreenId>();
        std::shared_ptr<HdiOutput> output;
        screens.push_back({screenId, output});
    }
    g_screenManager->OnProcessDisconnected(screens);
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
        case OHOS::Rosen::DO_SET_VIRTUAL_SCREEN_SURFACE:
            OHOS::Rosen::DoSetVirtualScreenSurface(fdp);
            break;
        case OHOS::Rosen::DO_SET_SCREEN_SECURITY_MASK:
            OHOS::Rosen::DoSetScreenSecurityMask(fdp);
            break;
        case OHOS::Rosen::DO_REGISTER_CORE_LISTENER:
            OHOS::Rosen::DoRegisterCoreListener(fdp);
            break;
        case OHOS::Rosen::DO_REGISTER_AGENT_LISTENER:
            OHOS::Rosen::DoRegisterAgentListener(fdp);
            break;
        case OHOS::Rosen::DO_UN_REGISTER_AGENT_LISTENER:
            OHOS::Rosen::DoUnRegisterAgentListener(fdp);
            break;
        case OHOS::Rosen::DO_ON_ACTIVE_SCREEN_ID_CHANGED_CALLBACK_CHANGED:
            OHOS::Rosen::DoOnActiveScreenIdChangedCallbackChanged(fdp);
            break;
        case OHOS::Rosen::DO_ON_SCREEN_CHANGE_CALLBACK_CHANGED:
            OHOS::Rosen::DoOnScreenChangeCallbackChanged(fdp);
            break;
        case OHOS::Rosen::DO_MOCK_HDI_SCREEN_CONNECTED:
            OHOS::Rosen::DoMockHdiScreenConnected(fdp);
            break;
        case OHOS::Rosen::DO_ON_PROCESS_DISCONNECTED:
            OHOS::Rosen::DoOnProcessDisconnected(fdp);
            break;
        default:
            return -1;
    }
    return 0;
}