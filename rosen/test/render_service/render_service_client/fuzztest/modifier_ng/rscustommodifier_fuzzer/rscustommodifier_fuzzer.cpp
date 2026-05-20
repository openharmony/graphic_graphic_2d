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

#include "rscustommodifier_fuzzer.h"
#include "modifier_ng/custom/rs_background_style_modifier.h"
#include "modifier_ng/custom/rs_content_style_modifier.h"
#include "modifier_ng/custom/rs_custom_modifier.h"
#include "modifier_ng/custom/rs_foreground_style_modifier.h"
#include "modifier_ng/custom/rs_node_modifier.h"
#include "modifier_ng/custom/rs_transition_style_modifier.h"
#include "draw/canvas.h"
#include <cstddef>
#include <cstdint>
#include <fuzzer/FuzzedDataProvider.h>

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t DO_DRAW_BACKGROUND_STYLE = 0;
constexpr size_t DO_DRAW_CONTENT_STYLE = 1;
constexpr size_t DO_DRAW_FOREGROUND_STYLE = 2;
constexpr size_t DO_DRAW_NODE_MODIFIER = 3;
constexpr size_t DO_DRAW_TRANSITION_STYLE = 4;
constexpr size_t TARGET_SIZE = 5;
}

void DoDrawBackgroundStyle(FuzzedDataProvider &fdp)
{
    auto modifier = std::make_shared<ModifierNG::RSBackgroundStyleModifier>();
    Drawing::Canvas drawingCanvas;
    ModifierNG::RSDrawingContext context;
    context.canvas = &drawingCanvas;
    context.width = fdp.ConsumeFloatingPoint<float>();
    context.height = fdp.ConsumeFloatingPoint<float>();
    modifier->Draw(context);
}

void DoDrawContentStyle(FuzzedDataProvider &fdp)
{
    auto modifier = std::make_shared<ModifierNG::RSContentStyleModifier>();
    Drawing::Canvas drawingCanvas;
    ModifierNG::RSDrawingContext context;
    context.canvas = &drawingCanvas;
    context.width = fdp.ConsumeFloatingPoint<float>();
    context.height = fdp.ConsumeFloatingPoint<float>();
    modifier->Draw(context);
}

void DoDrawForegroundStyle(FuzzedDataProvider &fdp)
{
    auto modifier = std::make_shared<ModifierNG::RSForegroundStyleModifier>();
    Drawing::Canvas drawingCanvas;
    ModifierNG::RSDrawingContext context;
    context.canvas = &drawingCanvas;
    context.width = fdp.ConsumeFloatingPoint<float>();
    context.height = fdp.ConsumeFloatingPoint<float>();
    modifier->Draw(context);
}

void DoDrawNodeModifier(FuzzedDataProvider &fdp)
{
    auto modifier = std::make_shared<ModifierNG::RSNodeModifier>();
    Drawing::Canvas drawingCanvas;
    ModifierNG::RSDrawingContext context;
    context.canvas = &drawingCanvas;
    context.width = fdp.ConsumeFloatingPoint<float>();
    context.height = fdp.ConsumeFloatingPoint<float>();
    modifier->Draw(context);
}

void DoDrawTransitionStyle(FuzzedDataProvider &fdp)
{
    auto modifier = std::make_shared<ModifierNG::RSTransitionStyleModifier>();
    Drawing::Canvas drawingCanvas;
    ModifierNG::RSDrawingContext context;
    context.canvas = &drawingCanvas;
    context.width = fdp.ConsumeFloatingPoint<float>();
    context.height = fdp.ConsumeFloatingPoint<float>();
    modifier->Draw(context);
}

bool RSCustomModifierFuzzTest(const uint8_t* data, size_t size)
{
    FuzzedDataProvider fdp(data, size);
    uint32_t code = fdp.ConsumeIntegral<uint32_t>();
    switch (code % TARGET_SIZE) {
        case DO_DRAW_BACKGROUND_STYLE:
            DoDrawBackgroundStyle(fdp);
            break;
        case DO_DRAW_CONTENT_STYLE:
            DoDrawContentStyle(fdp);
            break;
        case DO_DRAW_FOREGROUND_STYLE:
            DoDrawForegroundStyle(fdp);
            break;
        case DO_DRAW_NODE_MODIFIER:
            DoDrawNodeModifier(fdp);
            break;
        case DO_DRAW_TRANSITION_STYLE:
            DoDrawTransitionStyle(fdp);
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
    OHOS::Rosen::RSCustomModifierFuzzTest(data, size);
    return 0;
}
