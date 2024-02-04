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
struct SpansWidth {
    double leftWidth = 0.0;
    double rightWidth = 0.0;
};
struct SpanPosition {
    size_t leftSpanIndex = 0;
    size_t rightSpanIndex = 0;
    size_t maxSpanIndex = 0;
    int leftCharIndex = -1;
    int rightCharIndex = -1;
    int avalibleWidth = -1;
    size_t curIndex = 0;
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
    std::vector<LineMetrics> DoShape(const std::vector<VariantSpan> spans, const TypographyStyle &tstyle,
        const std::shared_ptr<FontProviders> &fontProviders, const double widthLimit);
    bool DidExceedMaxLines() const;
    double GetMinIntrinsicWidth() const;
    double GetMaxIntrinsicWidth() const;
    void SetIndents(const std::vector<float> &indents);

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
    void ConsiderLastLine(const TypographyStyle &style, const std::shared_ptr<FontProviders> &fontProviders,
        EllipsisParams params, const bool isErase);
    void ConsiderTailEllipsis(const TypographyStyle &style, const std::shared_ptr<FontProviders> &fontProviders,
        EllipsisParams params);
    std::vector<LineMetrics> CreatePartlySpan(const bool cutRight, const TypographyStyle &ys,
        const std::shared_ptr<FontProviders> &fontProviders, const VariantSpan &span, const double exceedWidth);
    bool CalcCharsIndex(const std::shared_ptr<TextSpan> textSpan, size_t &leftIndex,
        size_t &rightIndex, size_t &maxIndex, const int avalibleWidth) const;
    void SplitJointLeftSpans(const EllipsisParams &params, const size_t leftIndex,
        const TypographyStyle &style, const std::shared_ptr<FontProviders> &fontProviders, const VariantSpan &span);
    void SplitJointRightSpans(const EllipsisParams &params, const size_t rightIndex,
        const TypographyStyle &style, const std::shared_ptr<FontProviders> &fontProviders, const VariantSpan &span);
    bool CalcMidSpanIndex(const std::vector<VariantSpan> &spans, size_t &leftIndex, size_t &rightIndex,
        struct SpansWidth &spansWidth, const int avalibleWidth);
    void ConsideMidSpanEllipsis(const TypographyStyle &style, const std::shared_ptr<FontProviders> &fontProviders,
        const EllipsisParams &params, const std::vector<VariantSpan> &spans);
    void ConsiderMiddleEllipsis(const TypographyStyle &style, const std::shared_ptr<FontProviders> &fontProviders,
        EllipsisParams params);
    void GetLoopNum(VariantSpan &span, std::vector<VariantSpan> &spans,
        int &loopNum, int &breakPos, bool &haveAnySpan);
    void GetAllTextSpan(std::vector<VariantSpan> &spans, int &loopNum, int &breakPos);
    bool HaveExceedWidth(const std::vector<VariantSpan> &spans, struct SpanPosition &spanPos,
        const int &charsWidth, struct SpansWidth &spanWidth);
    bool CalcAvalibleWidth(const std::vector<VariantSpan> &spans, struct SpanPosition &spanPos,
        bool &isLeft, int &charsWidth, struct SpansWidth &spanWidth);
    bool CalcSpanPosition(const std::vector<VariantSpan> &spans, struct SpanPosition &spanPos,
        const int &breakPos, const int loopNum);
    void JointCriticalLeftSpans(const std::vector<VariantSpan> &spans, std::vector<VariantSpan> &leftLineSpans,
        struct SpanPosition &spanPos, const TypographyStyle &style,
        const std::shared_ptr<FontProviders> &fontProviders);
    void SplitJointLeftLineSpans(const std::vector<VariantSpan> &spans, std::vector<VariantSpan> &leftLineSpans,
        struct SpanPosition &spanPos, const TypographyStyle &style,
        const std::shared_ptr<FontProviders> &fontProviders);
    void JointRightLineSpans(const std::vector<VariantSpan> &spans, std::vector<VariantSpan> &rightLineSpans,
        struct SpanPosition &spanPos, const TypographyStyle &style,
        const std::shared_ptr<FontProviders> &fontProviders);
    void SplitJointRightLineSpans(const std::vector<VariantSpan> &spans, std::vector<VariantSpan> &rightLineSpans,
        struct SpanPosition &spanPos, const TypographyStyle &style,
        const std::shared_ptr<FontProviders> &fontProviders);
    void SplitJointSpans(const std::vector<VariantSpan> &spans, const EllipsisParams &params,
        struct SpanPosition &spanPos, const TypographyStyle &style,
        const std::shared_ptr<FontProviders> &fontProviders);

    void SetEllipsisProperty(std::vector<VariantSpan> &ellipsisSpans,
        std::vector<LineMetrics> &ellipsisMertics, double &ellipsisWidth);
    std::vector<LineMetrics> lineMetrics_;
    bool didExceedMaxLines_ = false;
    double maxIntrinsicWidth_ = 0.0;
    double minIntrinsicWidth_ = 0.0;
    std::vector<float> indents_;
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_SHAPER_H
