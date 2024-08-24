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

#include "line_breaker.h"

#include "char_groups.h"
#include "floating.h"
#include "texgine/any_span.h"
#include "texgine_exception.h"
#include "texgine/utils/exlog.h"
#ifdef LOGGER_ENABLE_SCOPE
#include "texgine/utils/trace.h"
#endif
#include "text_merger.h"
#include "text_span.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
#define FAILED 1

std::vector<LineMetrics> LineBreaker::BreakLines(std::vector<VariantSpan> &spans,
    const TypographyStyle &tstyle, const double widthLimit, const std::vector<float> &indents)
{
    LOGSCOPED(sl, LOGEX_FUNC_LINE_DEBUG(), "BreakLines");
    auto ss = GenerateScoreSpans(spans);
    DoBreakLines(ss, widthLimit, tstyle, indents);
    auto lineBreaks = GenerateBreaks(spans, ss);
    std::vector<LineMetrics> lineMetrics = GenerateLineMetrics(widthLimit, spans, lineBreaks, indents);
    ProcessHardBreak(lineMetrics);
    return lineMetrics;
}

std::vector<struct ScoredSpan> LineBreaker::GenerateScoreSpans(const std::vector<VariantSpan> &spans)
{
    LOGSCOPED(sl, LOGEX_FUNC_LINE_DEBUG(), "GenerateScoreSpans");
    std::vector<struct ScoredSpan> scoredSpans = {{}};
    double offset = 0;
    double preBreak = 0.0;
    double postBreak = 0.0;
    CharGroups lastcgs;
    for (const auto &span : spans) {
        if (span == nullptr) {
            throw TEXGINE_EXCEPTION(INVALID_ARGUMENT);
        }

        if (auto ts = span.TryToTextSpan(); ts != nullptr) {
            if (lastcgs.IsSameCharGroups(ts->cgs_) == false) {
                // not same word break
                offset = scoredSpans.back().postBreak;
                lastcgs = ts->cgs_;
            }
            preBreak = ts->GetPreBreak();
            postBreak = ts->GetPostBreak();
        }

        if (auto as = span.TryToAnySpan(); as != nullptr) {
            offset = scoredSpans.back().postBreak;
            preBreak = as->GetWidth();
            postBreak = preBreak;
            // prevent after as is a SameCharGroups ts with before this as
            lastcgs = {};
        }

        LOGEX_FUNC_LINE_DEBUG() << "[" << scoredSpans.size() << "]"
            << ": offset: " << offset
            << ", preBreak: " << preBreak
            << ", postBreak: " << postBreak;
        scoredSpans.push_back({
            .span = span,
            .preBreak = offset + preBreak,
            .postBreak = offset + postBreak,
            .score = 0,
            .prev = 0,
        });
    }
    scoredSpans.erase(scoredSpans.begin());
    return scoredSpans;
}

static double GetIndent(const double widthLimit, const int index, const std::vector<float> &indents)
{
    double indent = 0.0;
    if (indents.size() > 0 && index < static_cast<int>(indents.size())) {
        indent = indents[index];
    } else {
        indent = indents.size() > 0 ? indents.back() : 0.0;
    }

    return indent;
}

void LineBreaker::DoBreakLines(std::vector<struct ScoredSpan> &scoredSpans, const double widthLimit,
    const TypographyStyle &tstyle, const std::vector<float> &indents)
{
    LOGSCOPED(sl, LOGEX_FUNC_LINE_DEBUG(), "UpadateLineBreaksData");
    scoredSpans.emplace(scoredSpans.cbegin());
    int index = 0;
    for (size_t i = 1; i < scoredSpans.size(); i++) {
        auto &is = scoredSpans[i];
        is.prev = scoredSpans[i - 1].prev;
        double newWidthLimit = widthLimit - GetIndent(widthLimit, index, indents);
        LOGEX_FUNC_LINE_DEBUG() << "[" << i << "]: is.preBreak: " << is.preBreak
            << ", prev.postBreak: " << scoredSpans[is.prev].postBreak;
        if (scoredSpans[i].span.IsHardBreak()) {
            is.prev = static_cast<int>(i - 1);
            index = 0;
        }

        if (FLOATING_GT(is.preBreak - scoredSpans[is.prev].postBreak, newWidthLimit)) {
            is.prev = static_cast<int>(i - 1);
            index++;
            LOGEX_FUNC_LINE_DEBUG() << "  -> [" << is.prev
                << "]: prev.postBreak: " << scoredSpans[is.prev].postBreak;
        }

        if (tstyle.breakStrategy == BreakStrategy::GREEDY) {
            continue;
        }

        LOGSCOPED(sl1, LOGEX_FUNC_LINE_DEBUG(), "algo");
        double delta = newWidthLimit - (is.preBreak - scoredSpans[is.prev].postBreak);
        is.score = delta * delta + scoredSpans[is.prev].score;

        std::stringstream ss;
        ss << i;
        LOGSCOPED(sl, LOGEX_FUNC_LINE_DEBUG(), ss.str());
        for (size_t j = 0; j < i; j++) {
            const auto &js = scoredSpans[j];
            double jdelta = newWidthLimit - (is.preBreak - js.postBreak);
            if (jdelta < 0) {
                continue;
            }

            double jscore = js.score + jdelta * jdelta;
            LOGEX_FUNC_LINE_DEBUG() << "[" << j << "]"
                << " s(" << jscore << ")" << " = js(" << js.score << ")"
                << " + dd(" << jdelta * jdelta << ")" << " (jdelta: " << jdelta << ")";

            if (jscore < is.score) {
                is.score = jscore;
                is.prev = static_cast<int>(j);
            }
        }
        LOGEX_FUNC_LINE_DEBUG() << "[" << i << "] Any{" << is.prev << "<-" << " b(" << is.preBreak << ", "
                      << is.postBreak << ")" << " s(" << is.score << ")}";
    }
    scoredSpans.erase(scoredSpans.begin());
}

std::vector<int32_t> LineBreaker::GenerateBreaks(std::vector<VariantSpan> &spans,
    const std::vector<struct ScoredSpan> &scoredSpans)
{
    LOGSCOPED(sl, LOGEX_FUNC_LINE_DEBUG(), "GenerateBreaks");

    std::vector<int32_t> lineBreaks;
    for (int i = static_cast<int>(scoredSpans.size()); i > 0; i = scoredSpans[i - 1].prev) {
        if (scoredSpans[i - 1].prev >= i) {
            throw TEXGINE_EXCEPTION(ERROR_STATUS);
        }

        LOGEX_FUNC_LINE_DEBUG() << "break at " << i;
        lineBreaks.push_back(i);
    }
    std::reverse(lineBreaks.begin(), lineBreaks.end());
    for (size_t i = 0; i < spans.size(); i++) {
        if (spans[i].IsHardBreak()) {
            auto it = find(lineBreaks.begin(), lineBreaks.end(), i);
            if (it == lineBreaks.end()) {
                lineBreaks.push_back(i);
            }

            auto next = find(lineBreaks.begin(), lineBreaks.end(), (i + 1));
            if (next == lineBreaks.end()) {
                lineBreaks.push_back(i + 1);
            }
        }
    }
    std::sort(lineBreaks.begin(), lineBreaks.end(), [](int32_t lb1, const int32_t lb2) {
        return (lb1 < lb2);
    });
    return lineBreaks;
}

std::vector<LineMetrics> LineBreaker::GenerateLineMetrics(const double widthLimit, std::vector<VariantSpan> &spans,
    std::vector<int32_t> &breaks, const std::vector<float> &indents)
{
    LOGSCOPED(sl, LOGEX_FUNC_LINE_DEBUG(), "GenerateLineMetrics");
    LOGEX_FUNC_LINE_DEBUG() << "breaks.size(): " << breaks.size();

    std::vector<LineMetrics> lineMetrics;
    if (!breaks.empty() && breaks.back() > static_cast<int>(spans.size())) {
        throw TEXGINE_EXCEPTION(OUT_OF_RANGE);
    }

    if (breaks.empty()) {
        return {};
    }
    if (breaks.front() != 0) {
        breaks.insert(breaks.begin(), 0);
    }

    int32_t prev = breaks[0];
    int32_t index = 0;
    for (size_t i = 1; i < breaks.size(); i++) {
        std::vector<VariantSpan> vss;
        int32_t next = breaks[i];
        if (next <= prev) {
            return {};
        }

        for (; prev < next; prev++) {
            vss.push_back(spans[prev]);
            if (spans[prev].IsHardBreak()) {
                index = 0;
            }
        }
        double indent = GetIndent(widthLimit, index, indents);
        lineMetrics.push_back({
            .lineSpans = vss,
            .indent = indent,
        });
        prev = next;
        index++;
    }

    return lineMetrics;
}

void LineBreaker::ProcessHardBreak(std::vector<LineMetrics> &lineMetrics)
{
    bool isAllHardBreak = false;
    int lineCount = static_cast<int>(lineMetrics.size());
    // If the number of lines equal 1 and the char is hard break, add a new line.
    if (lineCount == 1 && lineMetrics.back().lineSpans.back().IsHardBreak()) {
        isAllHardBreak = true;
        // When the number of lines more than 1, and the text ending with two hard breaks, add a new line.
    } else if (lineCount > 1) {
        // 1 is the last line
        isAllHardBreak = lineMetrics[lineCount - 1].lineSpans.front().IsHardBreak() &&
            lineMetrics[lineCount - 2].lineSpans.back().IsHardBreak(); // 2 is the penultimate line
    }

    if (isAllHardBreak) {
        lineMetrics.push_back(lineMetrics.back());
    }

     // lineMetrics.size() - 2 means more than 2 rows are processed
    for (auto i = 0; i < static_cast<int>(lineMetrics.size() - 2); i++) {
        if (!lineMetrics[i].lineSpans.back().IsHardBreak() &&
                lineMetrics[i + 1].lineSpans.front().IsHardBreak()) {
            lineMetrics[i].lineSpans.push_back(lineMetrics[i + 1].lineSpans.front());
            lineMetrics[i + 1].lineSpans.erase(lineMetrics[i + 1].lineSpans.begin());
        }
        if (lineMetrics[i + 1].lineSpans.empty()) {
            lineMetrics.erase(lineMetrics.begin() + (i + 1));
        }
    }
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
