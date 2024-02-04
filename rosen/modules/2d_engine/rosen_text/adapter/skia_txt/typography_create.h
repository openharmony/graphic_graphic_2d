/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.. All rights reserved.
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

#ifndef ROSEN_TEXT_ADAPTER_TXT_TYPOGRAPHY_CREATE_H
#define ROSEN_TEXT_ADAPTER_TXT_TYPOGRAPHY_CREATE_H

#include "rosen_text/typography_create.h"

#include "txt/paragraph_builder.h"

namespace OHOS {
namespace Rosen {
namespace AdapterTxt {
class TypographyCreate : public ::OHOS::Rosen::TypographyCreate {
public:
    TypographyCreate(const TypographyStyle& style, std::shared_ptr<FontCollection> collection);
    void PushStyle(const TextStyle& style) override;
    void PopStyle() override;
    void AppendText(const std::u16string& text) override;
    void AppendPlaceholder(const PlaceholderSpan& span) override;
    std::unique_ptr<Typography> CreateTypography() override;
    void AppendSymbol(const uint32_t& symbolId) override;
    std::vector<uint16_t> SymbolToUTF16(const std::vector<uint32_t> &utf32Text);

private:
    std::unique_ptr<SPText::ParagraphBuilder> builder_ = nullptr;
};
} // namespace AdapterTxt
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_TEXT_ADAPTER_TXT_TYPOGRAPHY_CREATE_H
