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

#ifndef ROSEN_MODULES_TEXGINE_SRC_TYPOGRAPHY_IMPL_H
#define ROSEN_MODULES_TEXGINE_SRC_TYPOGRAPHY_IMPL_H

#include <string>
#include <vector>

#include "line_metrics.h"
#include "texgine/typography.h"
#include "texgine/typography_types.h"
#include "symbol_animation_config.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
struct StrutMetrics {
    double ascent = 0;
    double descent = 0;
    double halfLeading = 0;
};

struct CalcResult {
    bool need = true;
    double ascent = 0;
    double descent = 0;
};

class TypographyImpl : public Typography {
public:
    TypographyImpl(TypographyStyle &ys, std::vector<VariantSpan> &spans,
        std::shared_ptr<FontProviders> providers);

    double GetMaxWidth() const override;
    double GetHeight() const override;
    double GetActualWidth() const override;
    double GetMinIntrinsicWidth() const override;
    double GetMaxIntrinsicWidth() const override;
    double GetAlphabeticBaseline() const override;
    double GetIdeographicBaseline() const override;
    bool DidExceedMaxLines() const override;
    int GetLineCount() const override;
    void SetIndents(const std::vector<float> &indents) override;
    void Layout(double maxWidth) override;

    void Paint(TexgineCanvas &canvas, double offsetX, double offsetY) override;

    std::vector<TextRect> GetTextRectsByBoundary(Boundary boundary,
                                                 TextRectHeightStyle heightStyle,
                                                 TextRectWidthStyle widthStyle) const override;
    std::vector<TextRect> GetTextRectsOfPlaceholders() const override;
    IndexAndAffinity GetGlyphIndexByCoordinate(double x, double y) const override;
    Boundary GetWordBoundaryByIndex(size_t index) const override;
    Boundary GetActualTextRange(int lineNumber, bool includeSpaces) const override;
    double GetLineHeight(int lineNumber);
    double GetLineWidth(int lineNumber);

    void SetAnimation(
        std::function<bool(
            const std::shared_ptr<TextEngine::SymbolAnimationConfig>&)>& animationFunc) override
    {
        if (animationFunc) {
            animationFunc_ = animationFunc;
        }
    }
private:

    std::function<bool(const std::shared_ptr<SymbolAnimationConfig>&)> animationFunc_ = nullptr;

    void ReportMemoryUsage(const std::string &member, bool needThis) const override;

    int ComputeStrut();
    void DoLayout();
    int UpdateMetrics();
    int UpdateSpanMetrics(VariantSpan &span, double &coveredAscent);
    int DoUpdateSpanMetrics(const VariantSpan &span, const std::shared_ptr<TexgineFontMetrics> metrics,
        const TextStyle &style, double &coveredAscent);
    void UpadateAnySpanMetrics(std::shared_ptr<AnySpan> &span, double &coveredAscent, double &coveredDescent);
    void ApplyAlignment();
    size_t FindGlyphTargetLine(double y) const;
    size_t FindGlyphTargetIndex(size_t line, double x, double &offsetX, std::vector<double> &widths) const;
    std::vector<TextRect> MergeRects(const std::vector<TextRect> &boxes, Boundary boundary) const;
    void ComputeWordBoundary() const;
    void ComputeSpans(int lineIndex, double baseline, const CalcResult &calcResult,
        std::vector<TextRect> &lineBoxes) const;
    std::vector<TextRect> GenTextRects(std::shared_ptr<TextSpan> &ts, double offsetX, double offsetY,
        double spanGapWidth) const;
    void ComputeRoundRect(VariantSpan& span, int& index, int& preIndex, LineMetrics& metrics,
        std::vector<VariantSpan>& groupSpans);
    TypographyStyle typographyStyle_;
    std::vector<VariantSpan> spans_;
    std::shared_ptr<FontProviders> fontProviders_;

    std::vector<LineMetrics> lineMetrics_;
    mutable std::vector<Boundary> boundariesCache_;
    bool didExceedMaxLines_ = false;
    StrutMetrics strut_;
    std::vector<double> baselines_;
    std::vector<double> lineMaxAscent_;
    std::vector<double> lineMaxCoveredAscent_;
    std::vector<double> lineMaxCoveredDescent_;
    double maxWidth_ = 0.0;
    double maxLineWidth_ = 0.0;
    float descent_ = 0.0;
    double height_ = 0.0;
    std::vector<double> yOffsets_ = {};
    double maxIntrinsicWidth_ = 0.0;
    double minIntrinsicWidth_ = 0.0;
    std::vector<float> indents_;
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_TYPOGRAPHY_IMPL_H
