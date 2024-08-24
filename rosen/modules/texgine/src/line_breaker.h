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

#ifndef ROSEN_MODULES_TEXGINE_SRC_LINE_BREAKER_H
#define ROSEN_MODULES_TEXGINE_SRC_LINE_BREAKER_H

#include <vector>

#include "line_metrics.h"
#include "texgine/typography_types.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
struct ScoredSpan {
    VariantSpan span;
    double preBreak;
    double postBreak;
    double score;
    int32_t prev;
};

class LineBreaker {
public:
    static std::vector<LineMetrics> BreakLines(std::vector<VariantSpan> &spans,
        const TypographyStyle &tstyle, const double widthLimit, const std::vector<float> &indents) noexcept(false);
    static std::vector<struct ScoredSpan> GenerateScoreSpans(const std::vector<VariantSpan> &spans) noexcept(false);
    static void DoBreakLines(std::vector<struct ScoredSpan> &scoredSpans, const double widthLimit,
        const TypographyStyle &tstyle, const std::vector<float> &indents) noexcept(false);
    static std::vector<int32_t> GenerateBreaks(std::vector<VariantSpan> &spans,
        const std::vector<struct ScoredSpan> &scoredSpans) noexcept(false);
    static std::vector<LineMetrics> GenerateLineMetrics(const double widthLimit, std::vector<VariantSpan> &spans,
        std::vector<int32_t> &breaks, const std::vector<float> &indents) noexcept(false);
    static void ProcessHardBreak(std::vector<LineMetrics> &lineMetrics);
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_LINE_BREAKER_H
