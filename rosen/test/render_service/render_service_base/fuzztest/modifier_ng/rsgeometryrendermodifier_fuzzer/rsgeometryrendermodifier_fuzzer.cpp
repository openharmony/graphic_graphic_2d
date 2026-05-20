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

#include "rsgeometryrendermodifier_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "draw/canvas.h"
#include "modifier_ng/geometry/rs_bounds_clip_render_modifier.h"
#include "modifier_ng/geometry/rs_frame_clip_render_modifier.h"
#include "modifier_ng/geometry/rs_transform_render_modifier.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint8_t DO_BOUNDS_CLIP = 0;
constexpr uint8_t DO_FRAME_CLIP = 1;
constexpr uint8_t DO_TRANSFORM = 2;
constexpr uint8_t TARGET_SIZE = 3;
} // namespace

void DoBoundsClip(FuzzedDataProvider& fdp)
{
    auto modifier = std::make_shared<ModifierNG::RSBoundsClipRenderModifier>();
    RSProperties properties;
    properties.SetClipToBounds(fdp.ConsumeBool());
    modifier->ResetProperties(properties);
}

void DoFrameClip(FuzzedDataProvider& fdp)
{
    auto modifier = std::make_shared<ModifierNG::RSFrameClipRenderModifier>();
    RSProperties properties;
    properties.SetClipToFrame(fdp.ConsumeBool());
    modifier->ResetProperties(properties);
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    modifier->Apply(&canvas, properties);
}

void DoTransform(FuzzedDataProvider& fdp)
{
    auto modifier = std::make_shared<ModifierNG::RSTransformRenderModifier>();
    RSProperties properties;
    properties.SetRotation(fdp.ConsumeFloatingPoint<float>());
    modifier->ResetProperties(properties);
}

void FuzzBody(FuzzedDataProvider& fdp)
{
    uint8_t tarPos = fdp.ConsumeIntegral<uint8_t>() % TARGET_SIZE;
    switch (tarPos) {
        case DO_BOUNDS_CLIP:
            DoBoundsClip(fdp);
            break;
        case DO_FRAME_CLIP:
            DoFrameClip(fdp);
            break;
        case DO_TRANSFORM:
            DoTransform(fdp);
            break;
        default:
            break;
    }
}
} // namespace Rosen
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0) {
        return -1;
    }
    FuzzedDataProvider fdp(data, size);
    OHOS::Rosen::FuzzBody(fdp);
    return 0;
}
