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

#ifndef ROSEN_MODULES_SPTEXT_PARAGRAPH_LINE_FETCHER_IMPL_H
#define ROSEN_MODULES_SPTEXT_PARAGRAPH_LINE_FETCHER_IMPL_H

#include <optional>

#include "modules/skparagraph/include/ParagraphLineFetcher.h"
#include "text_line_impl.h"
#include "txt/paint_record.h"
#include "txt/paragraph_line_fetcher.h"

namespace OHOS {
namespace Rosen {
namespace SPText {
class ParagraphLineFetcherImpl : public ParagraphLineFetcher {
public:
    ParagraphLineFetcherImpl(std::unique_ptr<skia::textlayout::ParagraphLineFetcher> lineFetcher,
                             std::vector<PaintRecord>&& paints);
    virtual ~ParagraphLineFetcherImpl() = default;
    size_t GetLineBreak(size_t startIndex, SkScalar width) const override;
    std::unique_ptr<TextLineBase> CreateLine(size_t startIndex, size_t count) override;
    std::unique_ptr<Paragraph> GetTempParagraph() override;
    size_t GetUnicodeSize() const override { return lineFetcher_->getUnicodeSize(); };

private:
    std::unique_ptr<skia::textlayout::ParagraphLineFetcher> lineFetcher_ = nullptr;
    std::vector<PaintRecord> paints_;
};
} // namespace SPText
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_SPTEXT_PARAGRAPH_LINE_FETCHER_IMPL_H
