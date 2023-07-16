/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.. All rights reserved.
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

#include "rosen_text/properties/typography_create_txt.h"

#include "rosen_text/properties/font_collection_txt.h"
#include "rosen_text/properties/rosen_converter_txt.h"
#include "rosen_text/ui/typography.h"
#include "txt/placeholder_run.h"
#include "txt/text_style.h"

namespace rosen {
TypographyCreateTxt::TypographyCreateTxt(const TypographyStyle& style,
    std::shared_ptr<FontCollection> font_collection)
{
    txt::ParagraphStyle txtParagraphStyle;
    RosenConvertTypographyStyle(style, txtParagraphStyle);
    const FontCollectionTxt* fontCollectionTxt = static_cast<const FontCollectionTxt*>(
        font_collection->GetFontCollection().get());
    paragraphBuilderTxt_ = std::make_shared<txt::ParagraphBuilderTxt>(
        txtParagraphStyle, fontCollectionTxt->GetFontCollection());
}

TypographyCreateTxt::~TypographyCreateTxt()
{
}

void TypographyCreateTxt::PushStyle(const TextStyle& style)
{
    txt::TextStyle textStyle;
    RosenConvertTxtStyle(style, textStyle);
    paragraphBuilderTxt_->PushStyle(textStyle);
}

void TypographyCreateTxt::Pop()
{
    paragraphBuilderTxt_->Pop();
}

void TypographyCreateTxt::AddText(const std::u16string& text)
{
    paragraphBuilderTxt_->AddText(text);
}

void TypographyCreateTxt::AddPlaceholder(PlaceholderRun& span)
{
    txt::PlaceholderRun txtPlaceholderRun = RosenConvertPlaceholderRun(span);
    paragraphBuilderTxt_->AddPlaceholder(txtPlaceholderRun);
}

std::unique_ptr<Typography> TypographyCreateTxt::Build()
{
    return std::make_unique<Typography>();
}
} // namespace rosen
