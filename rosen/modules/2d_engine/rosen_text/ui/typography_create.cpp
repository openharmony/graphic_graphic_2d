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
#include "rosen_text/ui/typography.h"
#include "rosen_text/ui/typography_create.h"

namespace rosen {
std::unique_ptr<TypographyCreate> TypographyCreate::CreateRosenBuilder(
    const TypographyStyle& style,
    std::shared_ptr<FontCollection> font_collection)
{
    return std::make_unique<TypographyCreate>(style, font_collection);
}

TypographyCreate::TypographyCreate(const TypographyStyle& style,
    std::shared_ptr<FontCollection> font_collection)
{
    TypographyCreateBase_ = std::make_shared<TypographyCreateTxt>(style, font_collection);
}

void TypographyCreate::PushStyle(const TextStyle& style)
{
    TypographyCreateBase_->PushStyle(style);
}

void TypographyCreate::Pop()
{
    TypographyCreateBase_->Pop();
}

void TypographyCreate::AddText(const std::u16string& text)
{
    TypographyCreateBase_->AddText(text);
}

void TypographyCreate::AddPlaceholder(PlaceholderRun& span)
{
    TypographyCreateBase_->AddPlaceholder(span);
}

std::unique_ptr<Typography> TypographyCreate::Build()
{
    std::unique_ptr<Typography> typography = TypographyCreateBase_->Build();
    typography->Init(TypographyCreateBase_);
    return typography;
}
} // namespace rosen
