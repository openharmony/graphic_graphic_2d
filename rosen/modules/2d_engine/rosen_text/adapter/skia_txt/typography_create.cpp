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

#include "typography_create.h"

#include "convert.h"
#include "typography.h"
#include <unicode/utf8.h>

namespace OHOS {
namespace Rosen {
std::unique_ptr<TypographyCreate> TypographyCreate::Create(const TypographyStyle& style,
    std::shared_ptr<FontCollection> collection)
{
    return std::make_unique<AdapterTxt::TypographyCreate>(style, collection);
}

namespace AdapterTxt {
TypographyCreate::TypographyCreate(const TypographyStyle& style,
    std::shared_ptr<OHOS::Rosen::FontCollection> collection)
{
    auto paragraphStyle = Convert(style);
    auto txtFontCollection = Convert(collection)->Get();
    builder_ = SPText::ParagraphBuilder::Create(paragraphStyle, txtFontCollection);
}

void TypographyCreate::PushStyle(const TextStyle& style)
{
    auto txtTextStyle = Convert(style);
    builder_->PushStyle(txtTextStyle);
}

void TypographyCreate::PopStyle()
{
    builder_->Pop();
}

void TypographyCreate::AppendText(const std::u16string& text)
{
    builder_->AddText(text);
}

std::vector<uint16_t> TypographyCreate::SymbolToUTF16(const std::vector<uint32_t> &utf32Text)
{
    size_t utf32Index = 0;
    size_t codePoint = 0;
    int error = 0;
    std::vector<uint16_t> utf16Text;
    while (utf32Index < utf32Text.size()) {
        UTF32_NEXT_CHAR_SAFE(utf32Text.data(), utf32Index, utf32Text.size(), codePoint, error);
        utf16Text.push_back(U16_LEAD(codePoint));
        utf16Text.push_back(U16_TRAIL(codePoint));
    }
    return utf16Text;
}

void TypographyCreate::AppendSymbol(const uint32_t& symbolId)
{
    std::vector<uint32_t> symbolUnicode = {symbolId};
    std::vector<uint16_t> symbolUnicode16 = SymbolToUTF16(symbolUnicode);
    std::u16string text;
    std::copy(symbolUnicode16.begin(), symbolUnicode16.end(), std::back_inserter(text));
    builder_->AddText(text);
}

void TypographyCreate::AppendPlaceholder(const PlaceholderSpan& span)
{
    auto txtPlaceholderRun = Convert(span);
    builder_->AddPlaceholder(txtPlaceholderRun);
}

std::unique_ptr<OHOS::Rosen::Typography> TypographyCreate::CreateTypography()
{
    auto paragraph = builder_->Build();
    return std::make_unique<Typography>(std::move(paragraph));
}
} // namespace AdapterTxt
} // namespace Rosen
} // namespace OHOS
