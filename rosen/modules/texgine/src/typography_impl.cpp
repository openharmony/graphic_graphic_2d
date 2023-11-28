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

#include "typography_impl.h"

#include <cassert>
#include <functional>
#include <variant>

#include "font_collection.h"
#include "shaper.h"
#include "texgine/any_span.h"
#include "texgine_exception.h"
#include "text_span.h"
#include "texgine/typography_types.h"
#include "texgine/utils/exlog.h"
#ifdef LOGGER_ENABLE_SCOPE
#include "texgine/utils/trace.h"
#endif
#include "word_breaker.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
#define MAXWIDTH 1e9
#define HALF(a) ((a) / 2)
#define MINDEV 1e-3
#define SUCCESSED 0
#define FAILED 1

void LineMetrics::AddSpanAndUpdateMetrics(const VariantSpan &span)
{
    lineSpans.push_back(span);
    width += span.GetWidth();
}

Boundary::Boundary(size_t left, size_t right)
{
    leftIndex = left;
    rightIndex = right;
}

TypographyImpl::TypographyImpl(TypographyStyle &ys, std::vector<VariantSpan> &spans,
    std::shared_ptr<FontProviders> providers
    ): typographyStyle_(std::move(ys)), spans_(std::move(spans)), fontProviders_(providers)
{
}

double TypographyImpl::GetAlphabeticBaseline() const
{
    if (lineMaxCoveredAscent_.empty()) {
        return 0.0;
    }

    return lineMaxCoveredAscent_[0];
}

double TypographyImpl::GetIdeographicBaseline() const
{
    if (lineMaxCoveredAscent_.empty() || lineMaxCoveredDescent_.empty()) {
        return 0.0;
    }

    return lineMaxCoveredAscent_[0] + lineMaxCoveredDescent_[0];
}

bool TypographyImpl::DidExceedMaxLines() const
{
    return didExceedMaxLines_;
}

double TypographyImpl::GetHeight() const
{
    return height_;
}

double TypographyImpl::GetMaxWidth() const
{
    return maxWidth_;
}

double TypographyImpl::GetActualWidth() const
{
    return maxLineWidth_;
}

double TypographyImpl::GetMaxIntrinsicWidth() const
{
    return maxIntrinsicWidth_;
}

double TypographyImpl::GetMinIntrinsicWidth() const
{
    return minIntrinsicWidth_;
}

int TypographyImpl::GetLineCount() const
{
    return lineMetrics_.size();
}

void TypographyImpl::SetIndents(const std::vector<float> &indents)
{
    indents_ = indents;
}

size_t TypographyImpl::FindGlyphTargetLine(double y) const
{
    int targetLine = 0;
    for (auto i = 0; i < static_cast<int>(lineMetrics_.size()); i++) {
        if (y < baselines_[i] + lineMaxCoveredDescent_[i]) {
            break;
        }
        targetLine = i + 1;
    }
    return targetLine;
}

size_t TypographyImpl::FindGlyphTargetIndex(size_t line,
    double x, double &offsetX, std::vector<double> &widths) const
{
    // gather widths
    widths = { lineMetrics_[line].indent };
    for (const auto &vs : lineMetrics_[line].lineSpans) {
        if (vs == nullptr) {
            continue;
        }

        auto ws = vs.GetGlyphWidths();
        if (vs.GetJustifyGap() > 0) {
            widths.insert(widths.end(), -vs.GetJustifyGap());
        }
        widths.insert(widths.end(), ws.begin(), ws.end());
    }

    offsetX = 0;
    size_t targetIndex = 0;
    for (const auto &width : widths) {
        if (x < offsetX + HALF(fabs(width))) {
            break;
        }

        if (width >= 0) {
            targetIndex++;
        }
        offsetX += fabs(width);
    }
    return targetIndex;
}

IndexAndAffinity TypographyImpl::GetGlyphIndexByCoordinate(double x, double y) const
{
    std::stringstream ss;
    ss << "GetGlyphPositionAtCoordinate(" << x << "," << y << ")";
    LOGSCOPED(sl, LOGEX_FUNC_LINE_DEBUG(), ss.str());

    // process y < 0
    if (fabs(height_) < DBL_EPSILON) {
        return {0, Affinity::NEXT};
    }

    // find targetLine
    int targetLine = static_cast<int>(FindGlyphTargetLine(y));
    if (targetLine == static_cast<int>(lineMetrics_.size())) {
        // process y more than typography, (lineMetrics_.size() - 1) is the last line
        targetLine = static_cast<int>(lineMetrics_.size() - 1);
    }

    // count glyph before targetLine
    size_t count = 0;
    for (auto i = 0; i < targetLine; i++) {
        for (const auto &span : lineMetrics_[i].lineSpans) {
            count += span.GetNumberOfChar();
        }
    }

    // find targetIndex
    double offsetX = 0;
    std::vector<double> widths;
    auto targetIndex = FindGlyphTargetIndex(targetLine, x, offsetX, widths);
    count += targetIndex;

    // process first line left part
    if (targetIndex == 0 && targetLine == 0) {
        return {0, Affinity::NEXT};
    }

    auto affinity = Affinity::PREV;
    if (targetIndex == widths.size()) {
        // process right part, (count - 1) is the index of last chargroup
        return {count - 1, affinity};
    }

    // calc affinity
    if (targetIndex > 0 && targetIndex < widths.size()) {
        auto mid = offsetX + HALF(fabs(widths[targetIndex]));
        if (x < mid) {
            count--;
            affinity = Affinity::NEXT;
        } else {
            affinity = Affinity::PREV;
        }
    }

    return {count, affinity};
}

void TypographyImpl::ComputeWordBoundary() const
{
    if (!boundariesCache_.empty()) {
        return;
    }

    for (const auto &span : spans_) {
        auto offset = boundariesCache_.empty() ? 0 : boundariesCache_.back().rightIndex;
        if (span.TryToAnySpan()) {
            boundariesCache_.emplace_back(offset, offset + 1);
            continue;
        }

        if (const auto &ts = span.TryToTextSpan(); ts != nullptr) {
            WordBreaker wb;
            wb.SetLocale(icu::Locale::createFromName(span.GetTextStyle().locale.c_str()));
            wb.SetRange(0, ts->u16vect_.size());
            auto boundaries = wb.GetBoundary(ts->u16vect_, true);
            for (const auto &[left, right] : boundaries) {
                boundariesCache_.emplace_back(left + offset, right + offset);
            }
        }
    }
}

Boundary TypographyImpl::GetWordBoundaryByIndex(size_t index) const
{
    ComputeWordBoundary();
    if (boundariesCache_.empty()) {
        return {0, 0};
    }

    for (const auto &boundary : boundariesCache_) {
        if (boundary.leftIndex <= index && index < boundary.rightIndex) {
            return boundary;
        }
    }

    auto right = boundariesCache_.back().rightIndex;
    return {right, right};
}

void TypographyImpl::Layout(double maxWidth)
{
    boundariesCache_ = {};
    try {
#ifdef LOGGER_ENABLE_SCOPE
        ScopedTrace scope("TypographyImpl::Layout");
#endif
        LOGSCOPED(sl, LOGEX_FUNC_LINE_DEBUG(), "TypographyImpl::Layout");
        LOGEX_FUNC_LINE_DEBUG(INFO) << "Layout maxWidth: " << maxWidth << ", spans.size(): " << spans_.size();
        maxWidth_ = floor(maxWidth);
        if (spans_.empty()) {
            LOGEX_FUNC_LINE(ERROR) << "Empty spans";
            return;
        }

        Shaper shaper;
        shaper.SetIndents(indents_);
        lineMetrics_ = shaper.DoShape(spans_, typographyStyle_, fontProviders_, maxWidth_);
        if (lineMetrics_.size() == 0) {
            LOGEX_FUNC_LINE(ERROR) << "Shape failed";
            return;
        }

        didExceedMaxLines_ = shaper.DidExceedMaxLines();
        maxIntrinsicWidth_ = shaper.GetMaxIntrinsicWidth();
        minIntrinsicWidth_ = shaper.GetMinIntrinsicWidth();
        ProcessHardBreak();

        auto ret = ComputeStrut();
        if (ret) {
            LOGEX_FUNC_LINE(ERROR) << "ComputeStrut failed";
            return;
        }

        ret = UpdateMetrics();
        if (ret) {
            LOGEX_FUNC_LINE(ERROR) << "UpdateMetrics failed";
            return;
        }

        DoLayout();
        ApplyAlignment();
    } catch (struct TexgineException &e) {
        LOGEX_FUNC_LINE(ERROR) << "catch exception: " << e.message;
    }
}

void TypographyImpl::ProcessHardBreak()
{
    bool isAllHardBreak = false;
    int lineCount = static_cast<int>(lineMetrics_.size());
    // If the number of lines equal 1 and the char is hard break, add a new line.
    if (lineCount == 1 && lineMetrics_.back().lineSpans.back().IsHardBreak()) {
        isAllHardBreak = true;
        // When the number of lines more than 1, and the text ending with two hard breaks, add a new line.
    } else if (lineCount > 1) {
        // 1 is the last line, 2 is the penultimate line.
        isAllHardBreak = lineMetrics_[lineCount - 1].lineSpans.front().IsHardBreak() &&
            lineMetrics_[lineCount - 2].lineSpans.back().IsHardBreak();
    }

    if (isAllHardBreak) {
        lineMetrics_.push_back(lineMetrics_.back());
    }

    for (auto i = 0; i < static_cast<int>(lineMetrics_.size() - 2); i++) {
        if (!lineMetrics_[i].lineSpans.back().IsHardBreak() &&
                lineMetrics_[i + 1].lineSpans.front().IsHardBreak()) {
            lineMetrics_[i].lineSpans.push_back(lineMetrics_[i + 1].lineSpans.front());
            lineMetrics_[i + 1].lineSpans.erase(lineMetrics_[i + 1].lineSpans.begin());
        }

        if (lineMetrics_[i + 1].lineSpans.empty()) {
                lineMetrics_.erase(lineMetrics_.begin() + (i + 1));
        }
    }
}

int TypographyImpl::UpdateMetrics()
{
    LOGSCOPED(sl, LOGEX_FUNC_LINE_DEBUG(), "UpdateMetrics");
    baselines_ = {};
    lineMaxAscent_ = {};
    lineMaxCoveredAscent_ = {};
    lineMaxCoveredDescent_ = {};
    height_ = 0.0;
    double prevMaxDescent = 0.0;
    double yOffset = 0.0;

    for (auto i = 0; i < static_cast<int>(lineMetrics_.size()); i++) {
        lineMaxAscent_.push_back(strut_.ascent);
        lineMaxCoveredAscent_.push_back(strut_.ascent + strut_.halfLeading);
        lineMaxCoveredDescent_.push_back(strut_.descent + strut_.halfLeading);

        for (auto &span : lineMetrics_[i].lineSpans) {
            if (span == nullptr) {
                LOGEX_FUNC_LINE(ERROR) << "span is nullptr";
                return FAILED;
            }

            double coveredAscent = 0;
            auto ret = UpdateSpanMetrics(span, coveredAscent);
            if (ret) {
                LOGEX_FUNC_LINE(ERROR) << "UpdateMerics is failed";
                return FAILED;
            }

            if (auto as = span.TryToAnySpan(); as != nullptr) {
                span.AdjustOffsetY(-coveredAscent);
            }
        }

        height_ += ceil(lineMaxCoveredAscent_.back() + lineMaxCoveredDescent_.back());
        baselines_.push_back(height_ - lineMaxCoveredDescent_.back());
        yOffset += ceil(lineMaxCoveredAscent_.back() + prevMaxDescent);
        yOffsets_.push_back(yOffset);
        prevMaxDescent = lineMaxCoveredDescent_.back();
        LOGEX_FUNC_LINE_DEBUG() << "[" << i << "] ascent: " << lineMaxAscent_.back() <<
            ", coveredAscent: " << lineMaxCoveredAscent_.back() <<
            ", coveredDescent: " << lineMaxCoveredDescent_.back();
    }

    return SUCCESSED;
}

void TypographyImpl::DoLayout()
{
    maxLineWidth_ = 0.0;
    bool needMerge = false;
    int size = static_cast<int>(lineMetrics_.size());
    if (size > 1) {
        needMerge = !lineMetrics_[size - 2].lineSpans.back().IsHardBreak() &&
            lineMetrics_[size - 1].lineSpans.front().IsHardBreak();
    }

    if (needMerge) {
        lineMetrics_[size - 2].lineSpans.push_back(lineMetrics_[size - 1].lineSpans.front());
        lineMetrics_[size - 1].lineSpans.erase(lineMetrics_[size - 1].lineSpans.begin());
    }

    for (auto i = 0; i < static_cast<int>(lineMetrics_.size()); i++) {
        double offsetX = 0;
        for (auto &vs : lineMetrics_[i].lineSpans) {
            vs.AdjustOffsetY(yOffsets_[i]);
            offsetX += HALF(vs.GetTextStyle().letterSpacing);
            vs.AdjustOffsetX(offsetX);
            offsetX += vs.GetWidth();

            lineMetrics_[i].width = offsetX;
        }
        maxLineWidth_ = std::max(maxLineWidth_, lineMetrics_[i].width);
    }
}

int TypographyImpl::ComputeStrut()
{
    strut_ = {};

    bool strutValid = typographyStyle_.useLineStyle && typographyStyle_.lineStyle.fontSize >= 0;
    if (!strutValid) {
        return SUCCESSED;
    }

    auto fontCollection = fontProviders_->GenerateFontCollection(
        typographyStyle_.lineStyle.fontFamilies);
    if (fontCollection == nullptr) {
        LOGEX_FUNC_LINE(ERROR) << "fontCollection is null";
        return FAILED;
    }

    FontStyles style(typographyStyle_.lineStyle.fontWeight, typographyStyle_.lineStyle.fontStyle);
    auto typeface = fontCollection->GetTypefaceForFontStyles(style, {}, {});
    if (typeface == nullptr) {
        LOGEX_FUNC_LINE(ERROR) << "seek typeface failed";
        return FAILED;
    }

    TexgineFontMetrics strutMetrics;
    TexgineFont font;
    font.SetTypeface(typeface->Get());
    font.SetSize(typographyStyle_.lineStyle.fontSize);
    font.GetMetrics(&strutMetrics);

    double strutLeading = typographyStyle_.lineStyle.spacingScale.value_or(0) * typographyStyle_.lineStyle.fontSize;
    auto leading = strutLeading;
    if (typographyStyle_.lineStyle.heightOnly) {
        double metricsHeight = -*strutMetrics.fAscent_ + *strutMetrics.fDescent_;
        if (fabs(metricsHeight) < DBL_EPSILON) {
            LOGEX_FUNC_LINE(ERROR) << "strutMetrics is error";
            return FAILED;
        }

        double scale = typographyStyle_.lineStyle.heightScale * typographyStyle_.lineStyle.fontSize;
        strut_.ascent = (-(*strutMetrics.fAscent_) / metricsHeight) * scale;
        strut_.descent = (*strutMetrics.fDescent_ / metricsHeight) * scale;
    } else {
        strut_.ascent = -(*strutMetrics.fAscent_);
        strut_.descent = *strutMetrics.fDescent_;
        leading = fabs(leading) < DBL_EPSILON ? *strutMetrics.fLeading_ : strutLeading;
    }
    strut_.halfLeading = HALF(leading);
    return SUCCESSED;
}

int TypographyImpl::UpdateSpanMetrics(VariantSpan &span, double &coveredAscent)
{
    auto style = span.GetTextStyle();
    TexgineFontMetrics metrics;
    if (auto ts = span.TryToTextSpan(); ts != nullptr) {
        metrics = ts->tmetrics_;
    } else {
        auto as = span.TryToAnySpan();
        auto families = style.fontFamilies;
        if (families.empty()) {
            families = typographyStyle_.fontFamilies;
        }
        auto fontCollection = fontProviders_->GenerateFontCollection(families);
        if (fontCollection == nullptr) {
            LOGEX_FUNC_LINE(ERROR) << "fontCollection is nullptr";
            return FAILED;
        }

        FontStyles fs(style.fontWeight, style.fontStyle);
        // 0xFFFC is a placeholder, use it to get typeface when text is empty.
        auto typeface = fontCollection->GetTypefaceForChar(0xFFFC, fs, "Latn", style.locale);
        if (typeface == nullptr) {
            typeface = fontCollection->GetTypefaceForFontStyles(fs, "Latn", style.locale);
        }
        if (typeface == nullptr) {
            LOGEX_FUNC_LINE(ERROR) << "typeface is nullptr";
            return FAILED;
        }

        TexgineFont font;
        font.SetTypeface(typeface->Get());
        font.SetSize(style.fontSize);
        font.GetMetrics(&metrics);
    }

    if (DoUpdateSpanMetrics(span, metrics, style, coveredAscent)) {
        LOGEX_FUNC_LINE(ERROR) << "DoUpdateSpanMetrics is error";
        return FAILED;
    }

    return SUCCESSED;
}

int TypographyImpl::DoUpdateSpanMetrics(const VariantSpan &span, const TexgineFontMetrics &metrics,
    const TextStyle &style, double &coveredAscent)
{
    bool onlyUseStrut = typographyStyle_.useLineStyle;
    onlyUseStrut = onlyUseStrut && (typographyStyle_.lineStyle.fontSize >= 0);
    onlyUseStrut = onlyUseStrut && typographyStyle_.lineStyle.only;
    double ascent = -*metrics.fAscent_;
    if (!onlyUseStrut) {
        double coveredDescent = 0;
        if (style.heightOnly) {
            double metricsHeight = -*metrics.fAscent_ + *metrics.fDescent_;
            if (fabs(metricsHeight) < DBL_EPSILON) {
                LOGEX_FUNC_LINE(ERROR) << "metrics is error";
                return FAILED;
            }

            coveredAscent = (-*metrics.fAscent_ / metricsHeight) * style.heightScale * style.fontSize;
            coveredDescent = (*metrics.fDescent_ / metricsHeight) * style.heightScale * style.fontSize;
        } else {
            coveredAscent = (-*metrics.fAscent_ + HALF(*metrics.fLeading_));
            coveredDescent = (*metrics.fDescent_ + HALF(*metrics.fLeading_));
        }
        if (auto as = span.TryToAnySpan(); as != nullptr) {
            UpadateAnySpanMetrics(as, coveredAscent, coveredDescent);
            ascent = coveredAscent;
        }
        if (style.halfLeading) {
            double halfLeading = strut_.halfLeading == 0 ? HALF(style.fontSize) : strut_.halfLeading;
            double lineHeight = style.heightScale * style.fontSize + halfLeading;
            coveredAscent = HALF(lineHeight);
            coveredDescent = HALF(lineHeight - style.fontSize);
        }
        lineMaxCoveredAscent_.back() = std::max(coveredAscent, lineMaxCoveredAscent_.back());
        lineMaxCoveredDescent_.back() = std::max(coveredDescent, lineMaxCoveredDescent_.back());
    }
    lineMaxAscent_.back() = std::max(lineMaxAscent_.back(), ascent);
    return SUCCESSED;
}

void TypographyImpl::UpadateAnySpanMetrics(std::shared_ptr<AnySpan> &span, double &coveredAscent,
    double &coveredDescent)
{
    if (span == nullptr) {
        throw TEXGINE_EXCEPTION(INVALID_ARGUMENT);
    }

    double as = coveredAscent;
    double de = coveredDescent;
    double aj = span->GetBaseline() == TextBaseline::IDEOGRAPHIC ? HALF(-de) : 0;
    double lo = span->GetLineOffset();
    double he = span->GetHeight();

    using CalcAscentFunc = std::function<double()>;
    std::map<AnySpanAlignment, CalcAscentFunc> calcMap = {
        {AnySpanAlignment::OFFSET_AT_BASELINE, [&] { return aj + lo; }},
        {AnySpanAlignment::ABOVE_BASELINE, [&] { return aj + he; }},
        {AnySpanAlignment::BELOW_BASELINE, [&] { return -aj; }},
        {AnySpanAlignment::TOP_OF_ROW_BOX, [&] { return as; }},
        {AnySpanAlignment::BOTTOM_OF_ROW_BOX, [&] { return he - de; }},
        {AnySpanAlignment::CENTER_OF_ROW_BOX, [&] { return HALF(as - de + he); }},
    };

    coveredAscent = calcMap[span->GetAlignment()]();
    coveredDescent = he - coveredAscent;
}

void TypographyImpl::Paint(TexgineCanvas &canvas, double offsetX, double offsetY)
{
    for (auto &metric : lineMetrics_) {
        for (auto &span : metric.lineSpans) {
            span.Paint(canvas, offsetX + span.GetOffsetX(), offsetY + span.GetOffsetY());
        }
    }
}

std::vector<TextRect> TypographyImpl::GetTextRectsByBoundary(Boundary boundary, TextRectHeightStyle heightStyle,
    TextRectWidthStyle widthStyle) const
{
    if (boundary.leftIndex > boundary.rightIndex || boundary.leftIndex < 0 || boundary.rightIndex < 0) {
        LOGEX_FUNC_LINE(ERROR) << "the box range is error";
        return {};
    }
    std::vector<TextRect> totalBoxes;
    for (auto i = 0; i < static_cast<int>(lineMetrics_.size()); i++) {
        if (baselines_.empty() || lineMaxAscent_.empty() || lineMaxCoveredAscent_.empty() ||
            lineMaxCoveredDescent_.empty()) {
            return {};
        }
        std::vector<TextRect> lineBoxes;
        auto baseline = baselines_[i];
        auto as = lineMaxAscent_[i];
        auto ca = lineMaxCoveredAscent_[i];
        auto cd = lineMaxCoveredDescent_[i];
        auto hl = ca - as;
        auto fl = i == 0 ? 1 : 0;
        auto ll = i == static_cast<int>(lineMetrics_.size()) - 1 ? 1 : 0;
        constexpr auto tgh = TextRectHeightStyle::TIGHT;
        constexpr auto ctb = TextRectHeightStyle::COVER_TOP_AND_BOTTOM;
        constexpr auto chf = TextRectHeightStyle::COVER_HALF_TOP_AND_BOTTOM;
        constexpr auto ctp = TextRectHeightStyle::COVER_TOP;
        constexpr auto cbm = TextRectHeightStyle::COVER_BOTTOM;

        std::map<TextRectHeightStyle, std::function<struct CalcResult()>> calcMap = {
            {tgh, [&] { return CalcResult{false}; }},
            {ctb, [&] { return CalcResult{true, as, cd}; }},
            {chf, [&] { return CalcResult{true, as + HALF(fl * hl), cd + HALF(ll * hl)}; }},
            {ctp, [&] { return CalcResult{true, as + fl * hl, cd}; }},
            {cbm, [&] { return CalcResult{true, as, cd + ll * hl}; }},
            {TextRectHeightStyle::FOLLOW_BY_LINE_STYLE, [&] {
                if (typographyStyle_.useLineStyle && typographyStyle_.lineStyle.fontSize >= 0) {
                    return CalcResult{true, strut_.ascent, strut_.descent};
                }
                return CalcResult{false};
            }},
        };

        const auto &result = calcMap[heightStyle]();
        ComputeSpans(i, baseline, result, lineBoxes);
        if (widthStyle == TextRectWidthStyle::MAX_WIDTH) {
            if (lineBoxes.empty()) {
                LOGEX_FUNC_LINE(ERROR) << "rects is null";
            } else {
                *lineBoxes.back().rect.fRight_ += maxLineWidth_ - lineMetrics_[i].width;
            }
        }
        totalBoxes.insert(totalBoxes.end(), lineBoxes.begin(), lineBoxes.end());
    }
    return MergeRects(totalBoxes, boundary);
}

void TypographyImpl::ComputeSpans(int lineIndex, double baseline, const CalcResult &calcResult,
    std::vector<TextRect> &lineBoxes) const
{
    for (const auto &span : lineMetrics_[lineIndex].lineSpans) {
        if (span == nullptr) {
            continue;
        }

        std::vector<TextRect> spanBoxes;
        auto offsetX = span.GetOffsetX();
        auto offsetY = span.GetOffsetY();

        if (auto as = span.TryToAnySpan(); as != nullptr) {
            auto rect = TexgineRect::MakeXYWH(offsetX, offsetY, span.GetWidth(), span.GetHeight());
            spanBoxes.push_back({.rect = rect, .direction = TextDirection::LTR});
        }

        if (auto ts = span.TryToTextSpan(); ts != nullptr) {
            std::vector<TextRect> boxes = GenTextRects(ts, offsetX, offsetY);
            spanBoxes.insert(spanBoxes.end(), boxes.begin(), boxes.end());
        }

        if (calcResult.need) {
            for (const auto &box : spanBoxes) {
                *(box.rect.fTop_) = baseline - calcResult.ascent;
                *(box.rect.fBottom_) = baseline + calcResult.descent;
            }
        }

        lineBoxes.insert(lineBoxes.end(), spanBoxes.begin(), spanBoxes.end());
    }
}

std::vector<TextRect> TypographyImpl::GenTextRects(std::shared_ptr<TextSpan> &ts, double offsetX, double offsetY) const
{
    double top = *(ts->tmetrics_.fAscent_);
    double height = *(ts->tmetrics_.fDescent_) - *(ts->tmetrics_.fAscent_);

    std::vector<TextRect> boxes;
    double width = 0.0;
    for (int i = 0; i < static_cast<int>(ts->glyphWidths_.size()); i++) {
        auto cg = ts->cgs_.Get(i);
        // If is emoji, don`t need process ligature, so set chars size to 1
        int charsSize = cg.IsEmoji() ? 1 : static_cast<int>(cg.chars.size());
        for (int j = 0; j < charsSize; j++) {
            auto rect = TexgineRect::MakeXYWH(offsetX + width, offsetY + top,
                ts->glyphWidths_[i] / charsSize, height);
            boxes.push_back({.rect = rect, .direction = TextDirection::LTR});
            width += ts->glyphWidths_[i] / charsSize;
        }
    }

    return boxes;
}

std::vector<TextRect> TypographyImpl::MergeRects(const std::vector<TextRect> &boxes, Boundary boundary) const
{
    if (boxes.size() == 0) {
        return {};
    }

    if (boundary.leftIndex > boxes.size()) {
        boundary.leftIndex = boxes.size() - 1;
    }

    if (boundary.rightIndex > boxes.size()) {
        boundary.rightIndex = boxes.size();
    }

    std::optional<TextRect> pre = std::nullopt;
    std::vector<TextRect> rects;
    for (auto it = boxes.cbegin() + boundary.leftIndex; it < boxes.cbegin() + boundary.rightIndex; it++) {
        const auto &rect = *it;
        if (!pre.has_value()) {
            pre = rect;
            continue;
        }

        if (*pre->rect.fTop_ == *rect.rect.fTop_ && *pre->rect.fBottom_ == *rect.rect.fBottom_ &&
            std::fabs(*pre->rect.fRight_ - *rect.rect.fLeft_) < MINDEV) {
            *pre->rect.fRight_ = *rect.rect.fRight_;
        } else {
            rects.push_back(pre.value());
            pre = rect;
        }
    }

    if (pre.has_value()) {
        rects.push_back(pre.value());
    }

    return rects;
}

std::vector<TextRect> TypographyImpl::GetTextRectsOfPlaceholders() const
{
    std::vector<TextRect> rects;
    for (const auto &line : lineMetrics_) {
        for (const auto &span : line.lineSpans) {
            if (span == nullptr || span.TryToTextSpan() != nullptr) {
                continue;
            }

            auto as = span.TryToAnySpan();
            auto rect = TexgineRect::MakeXYWH(span.GetOffsetX(), span.GetOffsetY(), span.GetWidth(), span.GetHeight());
            rects.push_back({.rect = rect, .direction = TextDirection::LTR});
        }
    }

    return rects;
}

void TypographyImpl::ApplyAlignment()
{
    TextAlign align_ = typographyStyle_.GetEquivalentAlign();
    size_t lineIndex = 0;
    for (auto &line : lineMetrics_) {
        bool isJustify = false;
        double spanGapWidth = 0.0;
        double typographyOffsetX = line.indent;
        if (TextAlign::RIGHT == align_ || (TextAlign::JUSTIFY == align_ &&
            TextDirection::RTL == typographyStyle_.direction)) {
            typographyOffsetX = maxWidth_ - line.width - line.indent;
        } else if (TextAlign::CENTER == align_) {
            if (typographyStyle_.direction == TextDirection::LTR) {
                typographyOffsetX = HALF(maxWidth_ - line.width) + line.indent;
            } else if (typographyStyle_.direction == TextDirection::RTL) {
                typographyOffsetX = HALF(maxWidth_ - line.width) - line.indent;
            }
        } else {
            // lineMetrics_.size() - 1 is last line index
            isJustify = align_ == TextAlign::JUSTIFY && lineIndex != lineMetrics_.size() - 1 &&
                !line.lineSpans.back().IsHardBreak() && line.lineSpans.size() > 1;
            if (isJustify) {
                // line.lineSpans.size() - 1 is gap count
                spanGapWidth = (maxWidth_ - line.width) / (line.lineSpans.size() - 1);
            }
        }

        size_t spanIndex = 0;
        for (auto &span : line.lineSpans) {
            span.AdjustOffsetX(typographyOffsetX + spanGapWidth * spanIndex);
            span.SetJustifyGap(spanIndex > 0 && isJustify ? spanGapWidth : 0.0);
            spanIndex++;
        }
        line.indent = typographyOffsetX;
        lineIndex++;
    }
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
