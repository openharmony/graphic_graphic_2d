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

#include "rsoverlaymodifier_fuzzer.h"
#include "modifier_ng/overlay/rs_overlay_style_modifier.h"
#include "draw/canvas.h"
#include <cstddef>
#include <cstdint>
#include <fuzzer/FuzzedDataProvider.h>

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t DO_DRAW = 0;
constexpr size_t DO_SET_CONTENT_TRANSITION_PARAM = 1;
constexpr size_t TARGET_SIZE = 2;
}

void DoDraw(FuzzedDataProvider &fdp)
{
    auto modifier = std::make_shared<ModifierNG::RSOverlayStyleModifier>();
    ModifierNG::RSDrawingContext context;
    Drawing::Canvas canvas;
    context.canvas = &canvas;
    context.width = fdp.ConsumeFloatingPoint<float>();
    context.height = fdp.ConsumeFloatingPoint<float>();
    modifier->Draw(context);
}

void DoSetContentTransitionParam(FuzzedDataProvider &fdp)
{
    auto modifier = std::make_shared<ModifierNG::RSOverlayStyleModifier>();
    auto type = static_cast<ContentTransitionType>(fdp.ConsumeIntegral<uint32_t>());
    modifier->SetContentTransitionParam(type);
}

bool RSOverlayStyleModifierFuzzTest(const uint8_t* data, size_t size)
{
    FuzzedDataProvider fdp(data, size);
    uint32_t code = fdp.ConsumeIntegral<uint32_t>();
    switch (code % TARGET_SIZE) {
        case DO_DRAW:
            DoDraw(fdp);
            break;
        case DO_SET_CONTENT_TRANSITION_PARAM:
            DoSetContentTransitionParam(fdp);
            break;
        default:
            break;
    }
    return true;
}
} // namespace Rosen
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return -1;
    }
    OHOS::Rosen::RSOverlayStyleModifierFuzzTest(data, size);
    return 0;
}
