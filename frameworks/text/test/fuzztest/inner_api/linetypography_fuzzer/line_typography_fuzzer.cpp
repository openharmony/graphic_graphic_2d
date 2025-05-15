/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "line_typography_fuzzer.h"

#include <codecvt>
#include <cstddef>
#include <fuzzer/FuzzedDataProvider.h>
#include <iostream>
#include <locale>
#include <string>

#include "line_typography.h"
#include "typography_create.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
void OHLineTypographyFuzz1(const uint8_t* data, size_t size)
{
    FuzzedDataProvider fdp(data, size);
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection = OHOS::Rosen::FontCollection::Create();
    OHOS::Rosen::TypographyStyle typographyStyle;
    typographyStyle.fontSize = std::fabs(fdp.ConsumeFloatingPoint<double>() + 1);
    std::unique_ptr<TypographyCreate> handler = TypographyCreate::Create(typographyStyle, fontCollection);
    std::string text = "world";
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    std::u16string wideText = convert.from_bytes(text);
    handler->AppendText(wideText);
    std::unique_ptr<LineTypography> lineTypography = handler->CreateLineTypography();
    if (lineTypography == nullptr) {
        return;
    }
    size_t count = lineTypography->GetLineBreak(0, fdp.ConsumeIntegral<uint32_t>() + 1);
    lineTypography->CreateLine(0, count);
    lineTypography->GetTempTypography();
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::OHLineTypographyFuzz1(data, size);
    return 0;
}
