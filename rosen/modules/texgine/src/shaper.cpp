/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "shaper.h"

#include <queue>
#include <variant>

#include "bidi_processer.h"
#include "line_breaker.h"
#include "measurer.h"
#include "texgine/any_span.h"
#include "texgine_exception.h"
#include "texgine/utils/exlog.h"
#ifdef LOGGER_ENABLE_SCOPE
#include "texgine/utils/trace.h"
#endif
#include "text_breaker.h"
#include "text_merger.h"
#include "text_reverser.h"
#include "text_shaper.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
#define MAXWIDTH 1e9
namespace {
void DumpLineMetrics(const std::vector<LineMetrics> &lineMetrics)
{
    LOGSCOPED(sl, LOGEX_FUNC_LINE_DEBUG(), "DumpLineMetrics");
    for (const auto &metric : lineMetrics) {
        for (const auto &span : metric.lineSpans) {
            span.Dump();
        }
    }
}
} // namespace

bool Shaper::DidExceedMaxLines() const
{
    return didExceedMaxLines_;
}

double Shaper::GetMinIntrinsicWidth() const
{
    return minIntrinsicWidth_;
}

double Shaper::GetMaxIntrinsicWidth() const
{
    return maxIntrinsicWidth_;
}

std::vector<LineMetrics> Shaper::CreateEllipsisSpan(const TypographyStyle &ys, const TextStyle &textStyle,
    const std::shared_ptr<FontProviders> &fontProviders)
{
    if (ys.ellipsis.empty()) {
        return {};
    }

    TextStyle xs;
    xs.fontSize = textStyle.fontSize;
    xs.fontFamilies = textStyle.fontFamilies;

    std::vector<VariantSpan> spans = {TextSpan::MakeFromText(ys.ellipsis)};
    for (auto &span : spans) {
        span.SetTextStyle(xs);
    }
    auto ys2 = ys;
    ys2.wordBreakType = WordBreakType::BREAK_ALL;
    ys2.breakStrategy = BreakStrategy::GREEDY;
    return DoShapeBeforeEllipsis(spans, ys2, fontProviders, MAXWIDTH);
}

void Shaper::ConsiderEllipsis(const TypographyStyle &tstyle,
    const std::shared_ptr<FontProviders> &fontProviders, const double widthLimit)
{
    didExceedMaxLines_ = false;
    auto maxLines = tstyle.maxLines;
    if (maxLines < 0) {
        maxLines = 1;
    }

    if (lineMetrics_.size() <= maxLines) {
        return;
    }

    auto &textStyle = lineMetrics_.back().lineSpans.back().GetTextStyle();
    std::vector<LineMetrics> ellipsisMertics = CreateEllipsisSpan(tstyle, textStyle, fontProviders);
    double ellipsisWidth = 0.0;
    std::vector<VariantSpan> ellipsisSpans;
    for (auto &metric : ellipsisMertics) {
        for (auto &es : metric.lineSpans) {
            ellipsisWidth += es.GetWidth();
            ellipsisSpans.push_back(es);
        }
    }

    EllipsisParams params{ellipsisSpans, ellipsisWidth, maxLines, widthLimit};
    if (maxLines == 1) { // single line
        switch (tstyle.ellipsisModal) {
            case EllipsisModal::HEAD:
                ConsiderHeadEllipsis(tstyle, fontProviders, params);
                break;
            case EllipsisModal::MIDDLE:
                ConsiderMiddleEllipsis(ellipsisSpans, ellipsisWidth, maxLines, widthLimit);
                break;
            case EllipsisModal::TAIL:
            default:
                ConsiderTailEllipsis(tstyle, fontProviders, params);
                break;
        }
    } else if (maxLines > 1) { // multi line
        if (tstyle.ellipsisModal == EllipsisModal::TAIL) {
            ConsiderTailEllipsis(tstyle, fontProviders, params);
        } else if (maxLines < lineMetrics_.size()) {
            lineMetrics_.erase(lineMetrics_.begin() + maxLines, lineMetrics_.end());
        }
    }
    didExceedMaxLines_ = true;
}

void Shaper::ComputeIntrinsicWidth(const size_t maxLines)
{
    maxIntrinsicWidth_ = 0.0;
    minIntrinsicWidth_ = 0.0;
    double lastInvisibleWidth = 0;
    for (const auto &line : lineMetrics_) {
        for (const auto &span : line.lineSpans) {
            if (span == nullptr) {
                continue;
            }

            auto width = span.GetWidth();
            auto visibleWidth = span.GetVisibleWidth();
            maxIntrinsicWidth_ += width;
            minIntrinsicWidth_ = std::max(visibleWidth, minIntrinsicWidth_);
            lastInvisibleWidth = width - visibleWidth;
        }
    }

    maxIntrinsicWidth_ -= lastInvisibleWidth;
    if (maxLines > 1) {
        minIntrinsicWidth_ = std::min(maxIntrinsicWidth_, minIntrinsicWidth_);
    } else {
        minIntrinsicWidth_ = maxIntrinsicWidth_;
    }
}

std::vector<LineMetrics> Shaper::DoShapeBeforeEllipsis(std::vector<VariantSpan> spans, const TypographyStyle &tstyle,
    const std::shared_ptr<FontProviders> &fontProviders, const double widthLimit)
{
    TextBreaker tb;
    tb.SetWidthLimit(widthLimit);
    auto ret = tb.WordBreak(spans, tstyle, fontProviders);
    if (ret) {
        LOGEX_FUNC_LINE(ERROR) << "word break failed";
        return {};
    }

    BidiProcesser bp;
    auto newSpans = bp.ProcessBidiText(spans, tstyle.direction);
    if (newSpans.empty()) {
        LOGEX_FUNC_LINE(ERROR) << "Process BidiText failed";
        return {};
    }

    LineBreaker lb;
    return lb.BreakLines(newSpans, tstyle, widthLimit);
}

std::vector<LineMetrics> Shaper::DoShape(std::vector<VariantSpan> spans, const TypographyStyle &tstyle,
    const std::shared_ptr<FontProviders> &fontProviders, const double widthLimit)
{
#ifdef LOGGER_ENABLE_SCOPE
    ScopedTrace scope("Shaper::DoShape");
#endif

    lineMetrics_= DoShapeBeforeEllipsis(spans, tstyle, fontProviders, widthLimit);
    ComputeIntrinsicWidth(tstyle.maxLines);
    ConsiderEllipsis(tstyle, fontProviders, widthLimit);

    TextMerger tm;
    for (auto &metric : lineMetrics_) {
        auto res = tm.MergeSpans(metric.lineSpans);
        std::swap(res, metric.lineSpans);
    }

    TextReverser tr;
    for (auto &metric : lineMetrics_) {
        tr.ReverseRTLText(metric.lineSpans);
        tr.ProcessTypoDirection(metric.lineSpans, tstyle.direction);
    }

    TextShaper textShaper;
    for (const auto &metric : lineMetrics_) {
        for (const auto &span : metric.lineSpans) {
            textShaper.Shape(span, tstyle, fontProviders);
        }
    }
    DumpLineMetrics(lineMetrics_);
    return lineMetrics_;
}

void Shaper::ConsiderHeadEllipsis(const TypographyStyle &ys, const std::shared_ptr<FontProviders> &fontProviders,
    EllipsisParams params)
{
    bool isErase = false;
    auto &lastLine = lineMetrics_.back();
    double lastLineWidth = lastLine.GetAllSpanWidth();
    if (params.maxLines < lineMetrics_.size()) {
        if (params.ellipsisWidth > 0) {
            while (lastLineWidth + params.ellipsisWidth < params.widthLimit) {
                // lineMetrics_.size() - 2 is the index of second to last
                auto lastSpan = lineMetrics_[lineMetrics_.size() - 2].lineSpans.back();
                lineMetrics_[lineMetrics_.size() - 2].lineSpans.pop_back();
                lastLine.lineSpans.insert(lastLine.lineSpans.begin(), lastSpan);
                lastLineWidth = lastLine.GetAllSpanWidth();
            }
            lineMetrics_.erase(lineMetrics_.begin(), lineMetrics_.end() - params.maxLines);
            isErase = true;
        } else {
            lineMetrics_.erase(lineMetrics_.begin() + params.maxLines, lineMetrics_.end());
        }
    }

    if (params.ellipsisSpans.empty() || (!isErase && lastLineWidth <= params.widthLimit)) {
        return;
    }

    std::vector<VariantSpan> &lastLineSpans = lineMetrics_.back().lineSpans;
    auto ts = lastLineSpans.front().TryToTextSpan();
    if (ts == nullptr) {
        if (lastLineWidth + params.ellipsisWidth > params.widthLimit) {
            lastLineSpans.erase(lastLineSpans.begin());
            lastLineSpans.insert(lastLineSpans.begin(), params.ellipsisSpans.begin(), params.ellipsisSpans.end());
        } else if (isErase) {
            lastLineSpans.insert(lastLineSpans.begin(), params.ellipsisSpans.begin(), params.ellipsisSpans.end());
        }
    } else {
        if (lastLineWidth + params.ellipsisWidth > params.widthLimit) {
            double exceedWidth = lastLineWidth + params.ellipsisWidth - params.widthLimit;
            auto firstSpan = lastLineSpans.front();
            lastLineSpans.erase(lastLineSpans.begin());
            std::vector<LineMetrics> partlySpan = CreatePartlySpan(false, ys, fontProviders, firstSpan, exceedWidth);
            if (!partlySpan.empty()) {
                std::vector<VariantSpan> partlyLineSpans = partlySpan.front().lineSpans;
                lastLineSpans.insert(lastLineSpans.begin(), partlyLineSpans.begin(), partlyLineSpans.end());
            }
            lastLineSpans.insert(lastLineSpans.begin(), params.ellipsisSpans.begin(), params.ellipsisSpans.end());
        } else if (isErase) {
            lastLineSpans.insert(lastLineSpans.begin(), params.ellipsisSpans.begin(), params.ellipsisSpans.end());
        }
    }
}

void Shaper::ConsiderOneMidEllipsis(const std::vector<VariantSpan> &ellipsisSpans, const double ellipsisWidth,
    const double widthLimit)
{
    bool isErase = false;
    auto lineSize = lineMetrics_.size();
    lineMetrics_.erase(lineMetrics_.begin() + 1, lineMetrics_.end() - 1);
    auto &firstline = lineMetrics_.front();
    double firstlineWidth = firstline.GetAllSpanWidth();
    auto &lastline = lineMetrics_.back();
    double lastlineWidth = lastline.GetAllSpanWidth();
    while (static_cast<int>(firstlineWidth + lastlineWidth) > static_cast<int>(widthLimit - ellipsisWidth) &&
        static_cast<int>(lastline.lineSpans.size()) > 1) {
        lastlineWidth -= lastline.lineSpans.front().GetWidth();
        lastline.lineSpans.erase(lastline.lineSpans.begin());
        isErase = true;
    }
    while ((!isErase) || (static_cast<int>(firstline.lineSpans.size()) > 1 &&
        static_cast<int>(firstlineWidth + lastlineWidth) > static_cast<int>(widthLimit - ellipsisWidth))) {
        firstlineWidth -= firstline.lineSpans.back().GetWidth();
        firstline.lineSpans.pop_back();
        isErase = true;
    }
    if (isErase || (lineSize > lineMetrics_.size())) {
        firstline.lineSpans.insert(firstline.lineSpans.end(), ellipsisSpans.begin(), ellipsisSpans.end());
    }
    firstline.lineSpans.insert(firstline.lineSpans.end(), lastline.lineSpans.begin(), lastline.lineSpans.end());
    lineMetrics_.pop_back();
}

void Shaper::ConsiderMiddleEllipsis(const std::vector<VariantSpan> &ellipsisSpans, const double ellipsisWidth,
    const size_t maxLines, const double widthLimit)
{
    if (maxLines == 1) { // single line
        ConsiderOneMidEllipsis(ellipsisSpans, ellipsisWidth, widthLimit);
    } else if (maxLines > 1) { // multi line
        bool isErase = false;
        auto lineSize = lineMetrics_.size();
        size_t middleLineStart = (maxLines - 1) / 2; // calculate the middle start row
        // calculate the middle end row
        size_t middleLineEnd = lineMetrics_.size() - (maxLines - (maxLines - 1) / 2 - 1);
        auto &middleLine = lineMetrics_.at(middleLineStart);
        double middleLineWidth = middleLine.GetAllSpanWidth();

        lineMetrics_.erase(lineMetrics_.begin() + middleLineStart + 1, lineMetrics_.begin() + middleLineEnd);
        if (middleLineStart) {
            while (static_cast<int>(middleLineWidth) > static_cast<int>(widthLimit - ellipsisWidth) &&
                (!middleLine.lineSpans.empty())) {
                middleLineWidth -= middleLine.lineSpans.back().GetWidth();
                middleLine.lineSpans.pop_back();
                isErase = true;
            }
        } else {
            while (static_cast<int>(middleLineWidth) > static_cast<int>(widthLimit - ellipsisWidth) &&
                (static_cast<int>(middleLine.lineSpans.size()) > 1)) {
                middleLineWidth -= middleLine.lineSpans.back().GetWidth();
                middleLine.lineSpans.pop_back();
                isErase = true;
            }
        }
        if (isErase || (lineSize > lineMetrics_.size())) {
            middleLine.lineSpans.insert(middleLine.lineSpans.end(), ellipsisSpans.begin(), ellipsisSpans.end());
        }
    }
}

void Shaper::ConsiderTailEllipsis(const TypographyStyle &ys, const std::shared_ptr<FontProviders> &fontProviders,
    EllipsisParams params)
{
    bool isErase = false;
    // maxLines - 1 is the index of last LineMetrics
    auto &lastLine = params.maxLines < lineMetrics_.size() ? lineMetrics_[params.maxLines - 1] : lineMetrics_.back();
    double lastLineWidth = lastLine.GetAllSpanWidth();
    if (params.maxLines < lineMetrics_.size()) {
        if (params.ellipsisWidth > 0 && lastLineWidth + params.ellipsisWidth < params.widthLimit) {
            lastLine.lineSpans.push_back(lineMetrics_[params.maxLines].lineSpans.front());
            lastLineWidth = lastLine.GetAllSpanWidth();
        }
        lineMetrics_.erase(lineMetrics_.begin() + params.maxLines, lineMetrics_.end());
        isErase = true;
    }

    if (params.ellipsisSpans.empty() || (!isErase && lastLineWidth <= params.widthLimit)) {
        return;
    }

    auto ts = lastLine.lineSpans.back().TryToTextSpan();
    if (ts == nullptr) {
        if (lastLineWidth + params.ellipsisWidth > params.widthLimit) {
            lastLine.lineSpans.pop_back();
            lastLine.lineSpans.insert(lastLine.lineSpans.end(), params.ellipsisSpans.begin(),
                params.ellipsisSpans.end());
        } else if (isErase) {
            lastLine.lineSpans.insert(lastLine.lineSpans.end(), params.ellipsisSpans.begin(),
                params.ellipsisSpans.end());
        }
    } else {
        if (lastLineWidth + params.ellipsisWidth > params.widthLimit) {
            double exceedWidth = lastLineWidth + params.ellipsisWidth - params.widthLimit;
            auto lastSpan = lastLine.lineSpans.back();
            lastLine.lineSpans.pop_back();
            std::vector<LineMetrics> partlySpan = CreatePartlySpan(true, ys, fontProviders, lastSpan, exceedWidth);
            if (!partlySpan.empty()) {
                std::vector<VariantSpan> partlyLineSpans = partlySpan.front().lineSpans;
                lastLine.lineSpans.insert(lastLine.lineSpans.end(), partlyLineSpans.begin(), partlyLineSpans.end());
            }
            lastLine.lineSpans.insert(lastLine.lineSpans.end(), params.ellipsisSpans.begin(),
                params.ellipsisSpans.end());
        } else if (isErase) {
            lastLine.lineSpans.insert(lastLine.lineSpans.end(), params.ellipsisSpans.begin(),
                params.ellipsisSpans.end());
        }
    }
}

std::vector<LineMetrics> Shaper::CreatePartlySpan(const bool cutRight, const TypographyStyle &ys,
    const std::shared_ptr<FontProviders> &fontProviders, const VariantSpan &span, const double exceedWidth) {
    auto textSpan = span.TryToTextSpan();
    int startIndex = textSpan->cgs_.GetRange().start;
    int endIndex = textSpan->cgs_.GetRange().end - 1;  // end - 1 is the index of end
    double deletedWidth = 0.0;
    while (endIndex >= startIndex && deletedWidth < exceedWidth) {
        if (cutRight) {
            deletedWidth += textSpan->cgs_.GetCharWidth(endIndex);
            endIndex--;
        } else {
            deletedWidth += textSpan->cgs_.GetCharWidth(startIndex);
            startIndex++;
        }
    }

    if (endIndex >= startIndex) {
        std::vector<uint16_t> chars = textSpan->cgs_.GetCharsToU16(startIndex, endIndex, cutRight);
        VariantSpan partlySpan(TextSpan::MakeFromText(chars));
        partlySpan.SetTextStyle(span.GetTextStyle());
        std::vector<VariantSpan> spans = {partlySpan};
        return DoShapeBeforeEllipsis(spans, ys, fontProviders, MAXWIDTH);
    } else {
        return {};
    }
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
