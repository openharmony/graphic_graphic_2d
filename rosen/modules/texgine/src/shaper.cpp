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
#include "texgine/utils/trace.h"
#include "text_breaker.h"
#include "text_merger.h"
#include "text_reverser.h"
#include "text_shaper.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
namespace {
void DumpLineMetrics(std::vector<LineMetrics> &lineMetrics)
{
    LOGSCOPED(sl, LOG2EX_DEBUG(), "DumpLineMetrics");
    for (auto &metric : lineMetrics) {
        for (auto &span : metric.lineSpans_) {
            span.Dump();
        }
    }
}
} // namespace

std::vector<LineMetrics> Shaper::DoShape(std::vector<VariantSpan> spans,
                                         const TypographyStyle &ys,
                                         const std::unique_ptr<FontProviders> &fontProviders,
                                         const double widthLimit)
{
    ScopedTrace scope("Shaper::DoShape");
    TextBreaker tb;
    auto ret = tb.WordBreak(spans, ys, fontProviders);
    if (ret) {
        LOG2EX(ERROR) << "word break failed";
        return {};
    }

    BidiProcesser bp;
    auto newSpans = bp.ProcessBidiText(spans, ys.direction_);
    if (newSpans.empty()) {
        LOG2EX(ERROR) << "Process BidiText failed";
        return {};
    }

    LineBreaker lb;
    auto lineMetrics = lb.BreakLines(newSpans, ys, widthLimit);

    TextMerger tm;
    for (auto &metric : lineMetrics) {
        auto ret = tm.MergeSpans(metric.lineSpans_);
        std::swap(ret, metric.lineSpans_);
    }

    TextReverser tr;
    for (auto &metric : lineMetrics) {
        tr.ReverseRTLText(metric.lineSpans_);
        tr.ProcessTypoDirection(metric.lineSpans_, ys.direction_);
    }

    TextShaper textShaper;
    for (const auto &metric : lineMetrics) {
        for (const auto &span : metric.lineSpans_) {
            textShaper.Shape(span, ys, fontProviders);
        }
    }
    DumpLineMetrics(lineMetrics);
    return lineMetrics;
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
