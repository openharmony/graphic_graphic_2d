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

#include "rsbackgroundmodifier_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#include "modifier_ng/background/rs_background_color_modifier.h"
#include "modifier_ng/background/rs_background_image_modifier.h"
#include "modifier_ng/background/rs_background_shader_modifier.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t DO_COLOR = 0;
const uint8_t DO_IMAGE = 1;
const uint8_t DO_SHADER = 2;
const uint8_t TARGET_SIZE = 3;
}

void DoBgColorModifier(FuzzedDataProvider& fdp)
{
    auto modifier = std::make_shared<ModifierNG::RSBackgroundColorModifier>();
    Vector4f brightnessRates(fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>(),
        fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>());
    modifier->SetBgBrightnessRates(brightnessRates);
    modifier->GetBgBrightnessRates();
    float brightnessFract = fdp.ConsumeFloatingPoint<float>();
    modifier->SetBgBrightnessFract(brightnessFract);
    modifier->GetBgBrightnessFract();
}

void DoBgImageModifier(FuzzedDataProvider& fdp)
{
    auto modifier = std::make_shared<ModifierNG::RSBackgroundImageModifier>();
    Vector4f innerRect(fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>(),
        fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>());
    modifier->SetBgImageInnerRect(innerRect);
    modifier->GetBgImageInnerRect();
    modifier->SetBgImageWidth(fdp.ConsumeFloatingPoint<float>());
    modifier->GetBgImageWidth();
    modifier->SetBgImageHeight(fdp.ConsumeFloatingPoint<float>());
    modifier->GetBgImageHeight();
    modifier->SetBgImagePositionX(fdp.ConsumeFloatingPoint<float>());
    modifier->GetBgImagePositionX();
    modifier->SetBgImagePositionY(fdp.ConsumeFloatingPoint<float>());
    modifier->GetBgImagePositionY();
    Vector4f dstRect(fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>(),
        fdp.ConsumeFloatingPoint<float>(), fdp.ConsumeFloatingPoint<float>());
    modifier->SetBgImageDstRect(dstRect);
    modifier->GetBgImageDstRect();
}

void DoBgShaderModifier(FuzzedDataProvider& fdp)
{
    auto modifier = std::make_shared<ModifierNG::RSBackgroundShaderModifier>();
    modifier->SetBackgroundShaderProgress(fdp.ConsumeFloatingPoint<float>());
    modifier->GetBackgroundShaderProgress();
    std::vector<float> param = {fdp.ConsumeFloatingPoint<float>()};
    modifier->SetComplexShaderParam(param);
    modifier->GetComplexShaderParam();
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
        case OHOS::Rosen::DO_COLOR:
            OHOS::Rosen::DoBgColorModifier(fdp);
            break;
        case OHOS::Rosen::DO_IMAGE:
            OHOS::Rosen::DoBgImageModifier(fdp);
            break;
        case OHOS::Rosen::DO_SHADER:
            OHOS::Rosen::DoBgShaderModifier(fdp);
            break;
        default:
            break;
    }
    return 0;
}
