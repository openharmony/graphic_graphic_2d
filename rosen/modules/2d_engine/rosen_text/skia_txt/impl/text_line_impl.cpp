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

#include "text_line_impl.h"
#include "drawing_painter_impl.h"
#include "run_impl.h"

namespace OHOS {
namespace Rosen {
namespace SPText {
namespace skt = skia::textlayout;
TextLineImpl::TextLineImpl(std::unique_ptr<skt::TextLineBase> textLineBase, const std::vector<PaintRecord>& paints)
    : textLineBase_(std::move(textLineBase)), paints_(paints)
{}

size_t TextLineImpl::GetGlyphCount() const
{
    if (!textLineBase_) {
        return 0;
    }
    return textLineBase_->getGlyphCount();
}

std::vector<std::unique_ptr<Run>> TextLineImpl::GetGlyphRuns() const
{
    if (!textLineBase_) {
        return {};
    }

    std::vector<std::unique_ptr<skt::RunBase>> runBases = textLineBase_->getGlyphRuns();
    std::vector<std::unique_ptr<SPText::Run>> runs;
    for (std::unique_ptr<skt::RunBase>& runBase : runBases) {
        std::unique_ptr<SPText::RunImpl> runImplPtr = std::make_unique<SPText::RunImpl>(std::move(runBase), paints_);
        runs.emplace_back(std::move(runImplPtr));
    }
    return runs;
}

Range<size_t> TextLineImpl::GetTextRange() const
{
    if (!textLineBase_) {
        Range<size_t> range(0, 0);
        return range;
    }
    skt::SkRange<size_t> range = textLineBase_->getTextRange();
    return Range<size_t>(range.start, range.end);
}

void TextLineImpl::Paint(Drawing::Canvas* canvas, double x, double y)
{
    if (!textLineBase_ || !canvas) {
        return;
    }
    RSCanvasParagraphPainter painter(canvas, paints_);
    textLineBase_->paint(&painter, x, y);
}
} // namespace SPText
} // namespace Rosen
} // namespace OHOS