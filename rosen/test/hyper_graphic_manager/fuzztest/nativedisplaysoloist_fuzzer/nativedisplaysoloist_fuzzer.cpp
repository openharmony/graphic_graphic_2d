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

#include "nativedisplaysoloist_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "feature/hyper_graphic_manager/rs_ui_display_soloist.h"
#include "native_display_soloist.h"
#include "transaction/rs_interfaces.h"
#include "ui/rs_ui_context.h"
#include "ui/rs_ui_context_manager.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t DO_CREATE = 0;
const uint8_t DO_DESTROY = 1;
const uint8_t DO_START = 2;
const uint8_t DO_STOP = 3;
const uint8_t DO_SET_EXPECTED_FRAME_RATE_RANGE = 4;
const uint8_t TARGET_SIZE = 5;

std::shared_ptr<RSUIContext> g_rsUIContext = nullptr;
OH_DisplaySoloist* g_displaySoloist = nullptr;
OH_DisplaySoloist* g_displaySoloist_1 = nullptr;

std::shared_ptr<RSUIContext> CreateRSUIContext()
{
    auto screenId = RSInterfaces::GetInstance().GetDefaultScreenId();
    sptr<IRemoteObject> connectToRender = RSInterfaces::GetInstance().GetConnectToRenderToken(screenId);
    auto rsUIContext = RSUIContextManager::MutableInstance().CreateRSUIContext(connectToRender);
    return rsUIContext;
}

struct OH_DisplaySoloistLayout {
    std::shared_ptr<SoloistId> soloistId_;
};

OH_DisplaySoloist* OH_DisplaySoloist_Create_1()
{
    std::shared_ptr<SoloistId> soloistId = SoloistId::Create();
    RSDisplaySoloistManager& soloistManager = RSDisplaySoloistManager::GetInstance();
    soloistManager.InsertUseExclusiveThreadFlag(soloistId->GetId(), false);

    OH_DisplaySoloistLayout* displaySoloist = new OH_DisplaySoloistLayout({ soloistId });
    g_displaySoloist_1 = reinterpret_cast<OH_DisplaySoloist*>(displaySoloist);
    return g_displaySoloist_1;
}


void DoCreate(FuzzedDataProvider& fdp)
{
    if (g_displaySoloist != nullptr) {
        OH_DisplaySoloist_Destroy(g_displaySoloist);
        g_displaySoloist = nullptr;
    }
    bool useExclusiveThread = fdp.ConsumeBool();
    g_displaySoloist = OH_DisplaySoloist_Create(useExclusiveThread);
    RSDisplaySoloistManager::GetInstance().idToSoloistMap_.clear();
    OH_DisplaySoloist_Destroy(g_displaySoloist);
    g_displaySoloist = nullptr;
}

void DoDestroy(FuzzedDataProvider& fdp)
{
    bool useNullSoloist = fdp.ConsumeBool();
    OH_DisplaySoloist* soloist = useNullSoloist ? nullptr : g_displaySoloist_1;
    OH_DisplaySoloist_Destroy(soloist);
    soloist = OH_DisplaySoloist_Create_1();
    reinterpret_cast<OH_DisplaySoloistLayout*>(soloist)->soloistId_ = nullptr;
    OH_DisplaySoloist_Destroy(soloist);
    if (!useNullSoloist) {
        g_displaySoloist = nullptr;
    }
}

void DoStart(FuzzedDataProvider& fdp)
{
    bool useNullSoloist = fdp.ConsumeBool();
    OH_DisplaySoloist* soloist = useNullSoloist ? nullptr : g_displaySoloist_1;
    bool useNullCallback = fdp.ConsumeBool();
    OH_DisplaySoloist_FrameCallback callback = nullptr;

    if (!useNullCallback) {
        callback = [](long long timestamp, long long targetTimestamp, void* data) {
            (void)timestamp;
            (void)targetTimestamp;
            (void)data;
        };
    }
    OH_DisplaySoloist_Start(soloist, callback, nullptr);
    soloist = OH_DisplaySoloist_Create_1();
    reinterpret_cast<OH_DisplaySoloistLayout*>(soloist)->soloistId_ = nullptr;
    OH_DisplaySoloist_Start(soloist, callback, nullptr);
}

void DoStop(FuzzedDataProvider& fdp)
{
    bool useNullSoloist = fdp.ConsumeBool();
    OH_DisplaySoloist* soloist = useNullSoloist ? nullptr : g_displaySoloist_1;
    OH_DisplaySoloist_Stop(soloist);
    soloist = OH_DisplaySoloist_Create_1();
    reinterpret_cast<OH_DisplaySoloistLayout*>(soloist)->soloistId_ = nullptr;
    OH_DisplaySoloist_Stop(soloist);
}

void DoSetExpectedFrameRateRange(FuzzedDataProvider& fdp)
{
    bool useNullSoloist = fdp.ConsumeBool();
    OH_DisplaySoloist* soloist = useNullSoloist ? nullptr : g_displaySoloist_1;

    bool useNullRange = fdp.ConsumeBool();
    if (useNullRange) {
        OH_DisplaySoloist_SetExpectedFrameRateRange(soloist, nullptr);
        return;
    }

    DisplaySoloist_ExpectedRateRange range;
    range.min = fdp.ConsumeIntegral<int32_t>();
    range.max = fdp.ConsumeIntegral<int32_t>();
    range.expected = fdp.ConsumeIntegral<int32_t>();
    OH_DisplaySoloist_SetExpectedFrameRateRange(soloist, &range);
    soloist = OH_DisplaySoloist_Create_1();
    reinterpret_cast<OH_DisplaySoloistLayout*>(soloist)->soloistId_ = nullptr;
    OH_DisplaySoloist_SetExpectedFrameRateRange(soloist, &range);
}
} // namespace
} // namespace Rosen
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0) {
        return -1;
    }

    FuzzedDataProvider fdp(data, size);

    auto& soloistManager = OHOS::Rosen::RSDisplaySoloistManager::GetInstance();
    OHOS::Rosen::g_rsUIContext = OHOS::Rosen::CreateRSUIContext();
    soloistManager.frameRateLinker_->rsUIContext_ = OHOS::Rosen::g_rsUIContext;

    uint8_t tarPos = fdp.ConsumeIntegral<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_CREATE:
            OHOS::Rosen::DoCreate(fdp);
            break;
        case OHOS::Rosen::DO_DESTROY:
            OHOS::Rosen::DoDestroy(fdp);
            break;
        case OHOS::Rosen::DO_START:
            OHOS::Rosen::DoStart(fdp);
            break;
        case OHOS::Rosen::DO_STOP:
            OHOS::Rosen::DoStop(fdp);
            break;
        case OHOS::Rosen::DO_SET_EXPECTED_FRAME_RATE_RANGE:
            OHOS::Rosen::DoSetExpectedFrameRateRange(fdp);
            break;
        default:
            return -1;
    }
    return 0;
}