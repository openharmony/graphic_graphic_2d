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

#include "rsscreenmanager08_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>
#include <memory>
#include <string>

#include "screen_manager/rs_screen_manager.h"
#include "screen_manager/screen_types.h"

namespace OHOS {
namespace Rosen {

sptr<RSScreenManager> g_screenManager;

namespace {
constexpr uint8_t DO_GET_SCREEN_VCP_FEATURE = 0;
constexpr uint8_t DO_SET_SCREEN_VCP_FEATURE = 1;
constexpr uint8_t TARGET_SIZE = 2;

void DoGetScreenVCPFeature(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegralInRange<ScreenId>(0, 255);
    uint8_t vcpCode = fdp.ConsumeIntegral<uint8_t>();
    uint16_t currentValue = fdp.ConsumeIntegral<uint16_t>();
    uint16_t maximumValue = fdp.ConsumeIntegral<uint16_t>();
    int32_t errorCode = fdp.ConsumeIntegral<int32_t>();
    g_screenManager->GetScreenVCPFeature(id, vcpCode, currentValue, maximumValue, errorCode);
}

void DoSetScreenVCPFeature(FuzzedDataProvider& fdp)
{
    ScreenId id = fdp.ConsumeIntegralInRange<ScreenId>(0, 255);
    uint8_t vcpCode = fdp.ConsumeIntegral<uint8_t>();
    uint16_t currentValue = fdp.ConsumeIntegral<uint16_t>();
    g_screenManager->SetScreenVCPFeature(id, vcpCode, currentValue);
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
        case OHOS::Rosen::DO_GET_SCREEN_VCP_FEATURE:
            OHOS::Rosen::DoGetScreenVCPFeature(fdp);
            break;
        case OHOS::Rosen::DO_SET_SCREEN_VCP_FEATURE:
            OHOS::Rosen::DoSetScreenVCPFeature(fdp);
            break;
        default:
            return -1;
    }
    return 0;
}