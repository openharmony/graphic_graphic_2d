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

#include "paragraph_line_fetcher_impl.h"
#include "paragraph_impl.h"
namespace OHOS {
namespace Rosen {
namespace SPText {
ParagraphLineFetcherImpl::ParagraphLineFetcherImpl(
    std::unique_ptr<skia::textlayout::ParagraphLineFetcher> lineFetcher, std::vector<PaintRecord>&& paints)
    : lineFetcher_(std::move(lineFetcher)), paints_(std::move(paints))
{}
size_t ParagraphLineFetcherImpl::GetLineBreak(size_t startIndex, SkScalar width) const
{
    if (lineFetcher_ == nullptr) {
        return 0;
    }
    return lineFetcher_->getLineBreak(startIndex, width);
}
std::unique_ptr<TextLineBase> ParagraphLineFetcherImpl::CreateLine(size_t startIndex, size_t count)
{
    if (lineFetcher_ == nullptr) {
        return nullptr;
    }
    return std::make_unique<TextLineImpl>(lineFetcher_->createLine(startIndex, count), paints_);
}
std::unique_ptr<Paragraph> ParagraphLineFetcherImpl::GetTempParagraph()
{
    if (lineFetcher_ == nullptr) {
        return nullptr;
    }
    std::vector<PaintRecord> paints = paints_;
    return std::make_unique<ParagraphImpl>(lineFetcher_->getTempParagraph(), std::move(paints));
}
} // namespace SPText
} // namespace Rosen
} // namespace OHOS
