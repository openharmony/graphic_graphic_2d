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

#include "rsforegroundmodifier_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "modifier_ng/foreground/rs_env_foreground_color_modifier.h"
#include "modifier_ng/foreground/rs_foreground_color_modifier.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint32_t FG_COLOR_STRATEGY_MAX = 3;
const uint8_t DO_ENV_COLOR = 0;
const uint8_t DO_COLOR = 1;
const uint8_t TARGET_SIZE = 2;
}

void DoEnvFgColorModifier(FuzzedDataProvider& fdp)
{
    auto modifier = std::make_shared<ModifierNG::RSEnvForegroundColorModifier>();
    modifier->GetType();
    ForegroundColorStrategyType strategyType =
        static_cast<ForegroundColorStrategyType>(fdp.ConsumeIntegral<uint32_t>() % FG_COLOR_STRATEGY_MAX);
    modifier->SetEnvForegroundColorStrategy(strategyType);
    modifier->GetEnvForegroundColorStrategy();
}

void DoFgColorModifier(FuzzedDataProvider& fdp)
{
    auto modifier = std::make_shared<ModifierNG::RSForegroundColorModifier>();
    modifier->GetType();
    Color color(fdp.ConsumeIntegral<uint32_t>(), fdp.ConsumeIntegral<uint32_t>(),
        fdp.ConsumeIntegral<uint32_t>(), fdp.ConsumeIntegral<uint32_t>());
    modifier->SetForegroundColor(color);
    modifier->GetForegroundColor();
}

} // namespace Rosen
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return -1;
    }

    FuzzedDataProvider fdp(data, size);
    uint8_t tarPos = fdp.ConsumeIntegral<uint8_t>() % OHOS::Rosen::TARGET_SIZE;
    switch (tarPos) {
        case OHOS::Rosen::DO_ENV_COLOR:
            OHOS::Rosen::DoEnvFgColorModifier(fdp);
            break;
        case OHOS::Rosen::DO_COLOR:
            OHOS::Rosen::DoFgColorModifier(fdp);
            break;
        default:
            break;
    }
    return 0;
}
