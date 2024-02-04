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

#ifndef ROSEN_MODULES_SPTEXT_PARAGRAPH_BUILDER_IMPL_H
#define ROSEN_MODULES_SPTEXT_PARAGRAPH_BUILDER_IMPL_H

#include "modules/skparagraph/include/ParagraphBuilder.h"
#include "paragraph_impl.h"
#include "txt/paragraph_builder.h"

namespace OHOS {
namespace Rosen {
namespace SPText {
class ParagraphBuilderImpl : public ParagraphBuilder {
public:
    ParagraphBuilderImpl(const ParagraphStyle& style, std::shared_ptr<txt::FontCollection> fontCollection);

    virtual ~ParagraphBuilderImpl();

    void PushStyle(const TextStyle& style) override;
    void Pop() override;
    const TextStyle& PeekStyle() override;
    void AddText(const std::u16string& text) override;
    void AddPlaceholder(PlaceholderRun& span) override;
    std::unique_ptr<Paragraph> Build() override;

private:
    skia::textlayout::ParagraphPainter::PaintID AllocPaintID(const PaintRecord& paint);
    skia::textlayout::ParagraphStyle TextStyleToSkStyle(const ParagraphStyle& txt);
    skia::textlayout::TextStyle TextStyleToSkStyle(const TextStyle& txt);
    void CopyTextStylePaint(const TextStyle& txt, skia::textlayout::TextStyle& skStyle);

    std::shared_ptr<skia::textlayout::ParagraphBuilder> builder_;
    TextStyle baseStyle_;

    std::stack<TextStyle> styleStack_;
    std::vector<PaintRecord> paints_;
};
} // namespace SPText
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_SPTEXT_PARAGRAPH_BUILDER_IMPL_H
