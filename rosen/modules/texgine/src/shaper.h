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

#ifndef ROSEN_MODULES_TEXGINE_SRC_SHAPER_H
#define ROSEN_MODULES_TEXGINE_SRC_SHAPER_H

#include "line_metrics.h"
#include "texgine/typography_style.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
struct EllipsisParams {
    std::vector<VariantSpan> ellipsisSpans;
    double ellipsisWidth;
    size_t maxLines;
    double widthLimit;
};
class Shaper {
public:
    /*
     * @brief Text shaping, in this, will do text breaker, bidirectional text processing
     *        line breaker, text merge, text reverse, and shape with harfbuzz
     * @param spans The text or non text what user want to panit
     * @param style Typography Style that user want
     * @param fontProviders The font providers setting by user
     * @param widthLimit The maximum display width set by the user
     */
    std::vector<LineMetrics> DoShape(std::vector<VariantSpan> spans, const TypographyStyle &tstyle,
        const std::shared_ptr<FontProviders> &fontProviders, const double widthLimit);
    bool DidExceedMaxLines() const;
    double GetMinIntrinsicWidth() const;
    double GetMaxIntrinsicWidth() const;

private:
    std::vector<LineMetrics> DoShapeBeforeEllipsis(std::vector<VariantSpan> spans, const TypographyStyle &tstyle,
        const std::shared_ptr<FontProviders> &fontProviders, const double widthLimit);
    void ConsiderEllipsis(const TypographyStyle &tstyle,
        const std::shared_ptr<FontProviders> &fontProviders, const double widthLimit);
    std::vector<LineMetrics> CreateEllipsisSpan(const TypographyStyle &ys, const TextStyle &textStyle,
        const std::shared_ptr<FontProviders> &fontProviders);
    void ComputeIntrinsicWidth(const size_t maxLines);
    void ConsiderHeadEllipsis(const TypographyStyle &ys, const std::shared_ptr<FontProviders> &fontProviders,
        EllipsisParams params);
    void ConsiderOneMidEllipsis(const std::vector<VariantSpan> &ellipsisSpans, const double ellipsisWidth,
        const double widthLimit);
    void ConsiderMiddleEllipsis(const std::vector<VariantSpan> &ellipsisSpans, const double ellipsisWidth,
        const size_t maxLines, const double widthLimit);
    void ConsiderTailEllipsis(const TypographyStyle &ys, const std::shared_ptr<FontProviders> &fontProviders,
        EllipsisParams params);
    std::vector<LineMetrics> CreatePartlySpan(const bool cutRight, const TypographyStyle &ys,
        const std::shared_ptr<FontProviders> &fontProviders, const VariantSpan &span, const double exceedWidth);

    std::vector<LineMetrics> lineMetrics_;
    bool didExceedMaxLines_ = false;
    double maxIntrinsicWidth_ = 0.0;
    double minIntrinsicWidth_ = 0.0;
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_SHAPER_H
