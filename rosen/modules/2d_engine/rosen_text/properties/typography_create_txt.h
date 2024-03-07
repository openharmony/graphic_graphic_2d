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

#ifndef ROSEN_TEXT_PROPERTIES_TYPOGRAPHY_CREATE_TXT_H
#define ROSEN_TEXT_PROPERTIES_TYPOGRAPHY_CREATE_TXT_H

#include <string>

#include "rosen_text/properties/placeholder_run.h"
#include "rosen_text/properties/text_style.h"
#include "rosen_text/properties/typography_create_base.h"
#include "rosen_text/properties/typography_style.h"
#include "rosen_text/ui/font_collection.h"
#include "txt/paragraph_builder_txt.h"

namespace rosen {
class TypographyCreateTxt : public TypographyCreateBase {
public:
    TypographyCreateTxt(const TypographyStyle& style, std::shared_ptr<FontCollection> font_collection);
    virtual ~TypographyCreateTxt() override;
    void PushStyle(const TextStyle& style) override;
    void Pop() override;
    void AddText(const std::u16string& text) override;
    void AddPlaceholder(PlaceholderRun& span) override;
    std::unique_ptr<Typography> Build() override;
    std::shared_ptr<txt::ParagraphBuilderTxt> GetParagraphBuilderTxt() const
    {
        return paragraphBuilderTxt_;
    }
    TypographyCreateTxt(const TypographyCreateTxt&) = delete;
    TypographyCreateTxt& operator = (const TypographyCreateTxt&) = delete;
    std::shared_ptr<txt::ParagraphBuilderTxt> paragraphBuilderTxt_;
};
} // namespace rosen
#endif // ROSEN_TEXT_PROPERTIES_TYPOGRAPHY_CREATE_TXT_H
