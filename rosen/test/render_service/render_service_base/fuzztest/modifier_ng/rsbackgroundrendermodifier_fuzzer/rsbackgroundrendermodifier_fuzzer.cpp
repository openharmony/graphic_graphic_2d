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

#include "rsbackgroundrendermodifier_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "modifier_ng/background/rs_background_color_render_modifier.h"
#include "modifier_ng/background/rs_background_image_render_modifier.h"
#include "modifier_ng/background/rs_background_shader_render_modifier.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr uint8_t DO_BACKGROUND_COLOR = 0;
constexpr uint8_t DO_BACKGROUND_IMAGE = 1;
constexpr uint8_t DO_BACKGROUND_SHADER = 2;
constexpr uint8_t TARGET_SIZE = 3;
} // namespace

void DoBackgroundColor(FuzzedDataProvider& fdp)
{
    auto modifier = std::make_shared<ModifierNG::RSBackgroundColorRenderModifier>();
    RSProperties properties;
    properties.SetBgBrightnessSaturation(fdp.ConsumeFloatingPoint<float>());
    modifier->ResetProperties(properties);
}

void DoBackgroundImage(FuzzedDataProvider& fdp)
{
    auto modifier = std::make_shared<ModifierNG::RSBackgroundImageRenderModifier>();
    RSProperties properties;
    properties.SetBgImageInnerRect(Vector4f(fdp.ConsumeFloatingPoint<float>()));
    modifier->ResetProperties(properties);
}

void DoBackgroundShader(FuzzedDataProvider& fdp)
{
    auto modifier = std::make_shared<ModifierNG::RSBackgroundShaderRenderModifier>();
    RSProperties properties;
    properties.SetBackgroundShaderProgress(fdp.ConsumeFloatingPoint<float>());
    modifier->ResetProperties(properties);
}

void FuzzBody(FuzzedDataProvider& fdp)
{
    uint8_t tarPos = fdp.ConsumeIntegral<uint8_t>() % TARGET_SIZE;
    switch (tarPos) {
        case DO_BACKGROUND_COLOR:
            DoBackgroundColor(fdp);
            break;
        case DO_BACKGROUND_IMAGE:
            DoBackgroundImage(fdp);
            break;
        case DO_BACKGROUND_SHADER:
            DoBackgroundShader(fdp);
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
