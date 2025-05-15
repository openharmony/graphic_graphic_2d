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

#include "run_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

namespace OHOS {
namespace Rosen {
void RunFuzzTest(const uint8_t* data, size_t size)
{
    FuzzedDataProvider fdp(data, size);
    SPText::ParagraphStyle paragraphStyle;
    std::shared_ptr<txt::FontCollection> fontCollection = std::make_shared<txt::FontCollection>();
    fontCollection->SetupDefaultFontManager();
    std::shared_ptr<SPText::ParagraphBuilder> paragraphBuilder =
        SPText::ParagraphBuilder::Create(paragraphStyle, fontCollection);
    std::string text = fdp.ConsumeRandomLengthString();
    paragraphBuilder->AddText(Str8ToStr16ByIcu(text));
    auto paragraph = paragraphBuilder->Build();
    paragraph->Layout(1000.0f);
    auto textLineBases = paragraph->GetTextLines();
    if (textLineBases.empty()) {
        return;
    }
    auto runs = textLineBases[0]->GetGlyphRuns();
    if (runs.empty()) {
        return;
    }
    const auto& run = runs[0];
    run->GetGlyphCount();
    run->GetGlyphs();
    run->GetPositions();
    run->GetOffsets();
    run->GetImageBounds();
    run->GetFont();
    run->GetStringIndices(fdp.ConsumeIntegral<int64_t>(), fdp.ConsumeIntegral<int64_t>());
    run->Paint(nullptr, fdp.ConsumeFloatingPoint<double>(), fdp.ConsumeFloatingPoint<double>());
    run->GetTypographicBounds(nullptr, nullptr, nullptr);
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::RunFuzzTest(data, size);
    return 0;
}
