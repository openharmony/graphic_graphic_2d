/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "text_flip_effect_fuzzer.h"
#include "text_flip_effect.h"
#include "typography.h"
#include "typography_create.h"
#include "font_collection.h"
#include "draw/canvas.h"

#include <fuzzer/FuzzedDataProvider.h>

namespace OHOS {
namespace Rosen {
void TextFlipEffectFuzzTest(const uint8_t* data, size_t size)
{
    FuzzedDataProvider fdp(data, size);
    Drawing::Canvas canvas;
    auto textEffect = TextEffectFactoryCreator::GetInstance().CreateTextEffect(TextEffectStrategy::FLIP);
    std::unordered_map<TextEffectAttribute, std::string> config = {
        {TextEffectAttribute::FLIP_DIRECTION, "up"},
        {TextEffectAttribute::BLUR_ENABLE, "false"},
    };
    textEffect->UpdateEffectConfig(config);
    OHOS::Rosen::TypographyStyle typographyStyle0;
    typographyStyle0.enableAutoSpace = false;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection0 =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle0, fontCollection0);
    std::u16string text = u"88";
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography0 = typographyCreate->CreateTypography();
    typography0->Layout(fdp.ConsumeFloatingPoint<double>());

    std::shared_ptr<Typography> typography(typography0.release());
    TypographyConfig typographyConfig = { typography, {0, text.length()} };
    std::vector<std::pair<TypographyConfig, TypographyConfig>> updateConfig = {
        {typographyConfig, typographyConfig}
    };

    textEffect->AppendTypography({typographyConfig});
    textEffect->RemoveTypography({typographyConfig});
    textEffect->UpdateTypography(updateConfig);
    textEffect->StartEffect(&canvas, fdp.ConsumeFloatingPoint<double>(), fdp.ConsumeFloatingPoint<double>());
    textEffect->StopEffect();
    textEffect->NoEffect(&canvas, fdp.ConsumeFloatingPoint<double>(), fdp.ConsumeFloatingPoint<double>());
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::TextFlipEffectFuzzTest(data, size);
    return 0;
}
