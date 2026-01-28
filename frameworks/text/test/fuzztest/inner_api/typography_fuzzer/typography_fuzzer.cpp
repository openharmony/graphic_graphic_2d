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

#include "typography_fuzzer.h"

#include <climits>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fuzzer/FuzzedDataProvider.h>

#include "font_collection.h"
#include "paragraph_builder.h"
#include "paragraph_style.h"
#include "typography.h"
#include "utils/string_util.h"
namespace OHOS {
namespace Rosen {
void TypographyFuzzTest(const uint8_t* data, size_t size)
{
    FuzzedDataProvider fdp(data, size);
    SPText::ParagraphStyle paragraphStyle;
    std::shared_ptr<txt::FontCollection> fontCollection = std::make_shared<txt::FontCollection>();
    fontCollection->SetupDefaultFontManager();
    std::shared_ptr<SPText::ParagraphBuilder> paragraphBuilder =
        SPText::ParagraphBuilder::Create(paragraphStyle, fontCollection);
    paragraphBuilder->AddText(u"Str8ToStr16ByIcu(text)");
    auto paragraph = paragraphBuilder->Build();
    paragraph->Layout(1000.0f);

    int32_t width = fdp.ConsumeIntegralInRange<int32_t>(1, 4096);
    int32_t height = fdp.ConsumeIntegralInRange<int32_t>(1, 4096);

    ImageOptions options{width, height, fdp.ConsumeFloatingPointInRange<float>(0.0f, 1000.0f),
        fdp.ConsumeFloatingPointInRange<float>(0.0f, 1000.0f)};

    std::shared_ptr<OHOS::Media::PixelMap> pixelMap = paragraph->GetTextPathImageByIndex(
        fdp.ConsumeIntegral<size_t>(), fdp.ConsumeIntegral<size_t>(), options, fdp.ConsumeBool());
    if (pixelMap == nullptr) {
        return;
    }
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::TypographyFuzzTest(data, size);
    return 0;
}
