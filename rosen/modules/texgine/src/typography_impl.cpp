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

#include <unicode/ubidi.h>

#include "font_collection.h"
#include "shaper.h"
#include "texgine/any_span.h"
#include "texgine_exception.h"
#include "text_span.h"
#include "texgine/typography_types.h"
#include "texgine/utils/exlog.h"
#include "texgine/utils/trace.h"
#include "word_breaker.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
#define MAXWIDTH 1e9
#define HALF 0.5f
#define MINDEV 1e-3

namespace {
std::vector<LineMetrics> CreateEllipsisSpan(const TypographyStyle &ys,
    const std::unique_ptr<FontProviders> &fontProviders)
{
    if (ys.ellipsis_.empty()) {
        return {};
    }

    TextStyle xs;
    xs.fontSize_ = ys.lineStyle_.fontSize_;
    xs.fontFamilies_ = ys.lineStyle_.fontFamilies_;

    std::vector<VariantSpan> spans = {TextSpan::MakeFromText(ys.ellipsis_)};
    spans[0].SetTextStyle(xs);
    auto ys2 = ys;
    ys2.wordBreakType_ = WordBreakType::BREAKALL;
    ys2.breakStrategy_ = BreakStrategy::GREEDY;
    return Shaper::Shape(spans, ys2, fontProviders, MAXWIDTH);
}
} // namespace

void LineMetrics::AddSpanAndUpdateMetrics(const VariantSpan &span)
{
    lineSpans_.push_back(span);
    width_ += span.GetWidth();
}

Boundary::Boundary(size_t left, size_t right)
{
    leftIndex_ = left;
    rightIndex_ = right;
}

TypographyImpl::TypographyImpl(TypographyStyle &ys, std::vector<VariantSpan> &spans,
    std::unique_ptr<FontProviders> providers)
{
    typographyStyle_ = std::move(ys);
    spans_ = std::move(spans);
    fontProviders_ = std::move(providers);
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
    // to be done: set indents
}

size_t TypographyImpl::FindGlyphTargetLine(double y) const
{
    int targetLine = 0;
    for (auto i = 0; i < lineMetrics_.size(); i++) {
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
    widths = { lineMetrics_[line].indent_ };
    for (auto &vs : lineMetrics_[line].lineSpans_) {
        if (vs == nullptr) {
            continue;
        }

        auto ws = vs.GetGlyphWidths();
        widths.insert(widths.end(), ws.begin(), ws.end());
    }

    offsetX = 0;
    size_t targetIndex = 0;
    for (const auto &width : widths) {
        if (x < offsetX + width) {
            break;
        }

        targetIndex++;
        offsetX += width;
    }
    return targetIndex;
}

IndexAndAffinity TypographyImpl::GetGlyphIndexByCoordinate(double x, double y) const
{
    std::stringstream ss;
    ss << "GetGlyphPositionAtCoordinate(" << x << "," << y << ")";
    LOGSCOPED(sl, LOG2EX_DEBUG(), ss.str());

    // process y < 0
    if (fabs(height_) < DBL_EPSILON || y < 0) {
        LOG2EX_DEBUG() << "special: y < 0";
        return {0, Affinity::NEXT};
    }

    // find targetLine
    int targetLine = FindGlyphTargetLine(y);
    LOG2EX_DEBUG() << "targetLine: " << targetLine;

    // count glyph before targetLine
    size_t count = 0;
    for (auto i = 0; i < targetLine; i++) {
        for (auto &span : lineMetrics_[i].lineSpans_) {
            count += span.GetNumberOfCharGroup();
        }
    }
    LOG2EX_DEBUG() << "count: " << count;

    // process y more than typography
    if (targetLine == lineMetrics_.size()) {
        LOG2EX_DEBUG() << "special: y >= max";
        return {count - 1, Affinity::PREV};
    }

    // find targetIndex
    double offsetX = 0;
    std::vector<double> widths;
    auto targetIndex = FindGlyphTargetIndex(targetLine, x, offsetX, widths);
    count += targetIndex;
    LOG2EX_DEBUG() << "targetIndex: " << targetIndex;

    // process first line left part
    if (targetIndex == 0 && targetLine == 0) {
        LOG2EX_DEBUG() << "special: first line left part";
        return {0, Affinity::NEXT};
    }

    // process right part
    if (targetIndex == widths.size()) {
        count--;
    }

    // calc affinity
    auto affinity = Affinity::PREV;
    if (targetIndex > 0 && targetIndex < widths.size()) {
        auto mid = offsetX + widths[targetIndex] * HALF;
        affinity = x < mid ? Affinity::NEXT : Affinity::PREV;
    }
    LOG2EX_DEBUG() << "affinity: " << (affinity == Affinity::PREV ? "upstream" : "downstream");

    return {count - 1, affinity};
}

void TypographyImpl::ComputeWordBoundary() const
{
    if (!boundariesCache_.empty()) {
        return;
    }

    for (const auto &span : spans_) {
        auto offset = boundariesCache_.empty() ? 0 : boundariesCache_.back().rightIndex_;
        if (span.TryToAnySpan()) {
            boundariesCache_.emplace_back(offset, offset + 1);
            continue;
        }

        if (const auto &ts = span.TryToTextSpan(); ts != nullptr) {
            WordBreaker wb;
            wb.SetLocale(icu::Locale::createFromName(span.GetTextStyle().locale_.c_str()));
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
        if (boundary.leftIndex_ <= index && index < boundary.rightIndex_) {
            return boundary;
        }
    }

    auto right = boundariesCache_.back().rightIndex_;
    return {right, right};
}

void TypographyImpl::Layout(double maxWidth)
{
    boundariesCache_ = {};
    try {
        ScopedTrace scope("TypographyImpl::Layout");
        LOGSCOPED(sl, LOG2EX_DEBUG(), "TypographyImpl::Layout");
        LOG2EX(INFO) << "Layout maxWidth: " << maxWidth << ", spans.size(): " << spans_.size();
        maxWidth_ = maxWidth;

        lineMetrics_ = Shaper::Shape(spans_, typographyStyle_, fontProviders_, maxWidth);
        if (lineMetrics_.size() == 0) {
            LOG2EX(ERROR) << "Shape failed";
            return;
        }

        ComputeIntrinsicWidth();

        ConsiderEllipsis();
        auto ret = ComputeStrut();
        if (ret) {
            LOG2EX(ERROR) << "ComputeStrut failed";
            return;
        }

        ret = UpdateMetrics();
        if (ret) {
            LOG2EX(ERROR) << "UpdateMetrics failed";
            return;
        }

        DoLayout();
        ApplyAlignment();
    } catch (struct TexgineException e) {
        LOG2EX(ERROR) << "catch exception: " << e.message;
    }
}

void TypographyImpl::ComputeIntrinsicWidth()
{
    maxIntrinsicWidth_ = 0.0;
    minIntrinsicWidth_ = 0.0;
    double lastInvisibleWidth = 0;
    for (const auto &line : lineMetrics_) {
        for (const auto &span : line.lineSpans_) {
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
    if (typographyStyle_.maxLines_ > 1) {
        minIntrinsicWidth_ = std::min(maxIntrinsicWidth_, minIntrinsicWidth_);
    } else {
        minIntrinsicWidth_ = maxIntrinsicWidth_;
    }
}

void TypographyImpl::ConsiderEllipsis()
{
    didExceedMaxLines_ = false;
    auto maxLines = typographyStyle_.maxLines_;
    if (lineMetrics_.size() <= maxLines) {
        return;
    }
    lineMetrics_.erase(lineMetrics_.begin() + maxLines, lineMetrics_.end());

    std::vector<LineMetrics> ellipsisMertics = CreateEllipsisSpan(typographyStyle_, fontProviders_);
    double ellipsisWidth = 0.0;
    std::vector<VariantSpan> ellipsisSpans;
    for (auto &metric : ellipsisMertics) {
        for (auto &es : metric.lineSpans_) {
            ellipsisWidth += es.GetWidth();
            ellipsisSpans.push_back(es);
        }
    }

    double width = 0;
    auto &lastline = lineMetrics_[maxLines - 1];
    for (auto &span : lastline.lineSpans_) {
        width += span.GetWidth();
    }

    // protected the first span and ellipsis
    while (width > maxWidth_ - ellipsisWidth && lastline.lineSpans_.size() > 1) {
        width -= lastline.lineSpans_.back().GetWidth();
        lastline.lineSpans_.pop_back();
    }

    // Add ellipsisSpans
    lastline.lineSpans_.insert(lastline.lineSpans_.end(), ellipsisSpans.begin(), ellipsisSpans.end());
    didExceedMaxLines_ = true;
}

int TypographyImpl::UpdateMetrics()
{
    LOGSCOPED(sl, LOG2EX_DEBUG(), "UpdateMetrics");
    baselines_ = {};
    lineMaxAscent_ = {};
    lineMaxCoveredAscent_ = {};
    lineMaxCoveredDescent_ = {};
    height_ = 0.0;

    for (auto i = 0; i < lineMetrics_.size(); i++) {
        lineMaxAscent_.push_back(strut_.ascent_);
        lineMaxCoveredAscent_.push_back(strut_.ascent_ + strut_.halfLeading_);
        lineMaxCoveredDescent_.push_back(strut_.descent_ + strut_.halfLeading_);

        for (auto &span : lineMetrics_[i].lineSpans_) {
            if (span == nullptr) {
                LOG2EX(ERROR) << "span is nullptr";
                return 1;
            }

            double coveredAscent = 0;
            auto ret = UpdateSpanMetrics(span, coveredAscent);
            if (ret) {
                LOG2EX(ERROR) << "UpdateMerics is failed";
                return 1;
            }

            if (auto as = span.TryToAnySpan(); as != nullptr) {
                span.AdjustOffsetY(-coveredAscent);
            }
        }

        height_ += lineMaxCoveredAscent_.back() + lineMaxCoveredDescent_.back();
        baselines_.push_back(height_ - lineMaxCoveredDescent_.back());
        LOG2EX_DEBUG() << "[" << i << "] ascent: " << lineMaxAscent_.back()
            << ", coveredAscent: " << lineMaxCoveredAscent_.back()
            << ", coveredDescent: " << lineMaxCoveredDescent_.back();
    }

    return 0;
}

void TypographyImpl::DoLayout()
{
    maxLineWidth_ = 0.0;
    for (auto i = 0; i < lineMetrics_.size(); i++) {
        double offsetX = 0;
        for (auto &vs : lineMetrics_[i].lineSpans_) {
            vs.AdjustOffsetY(baselines_[i]);
            vs.AdjustOffsetX(offsetX);
            offsetX += vs.GetWidth();

            lineMetrics_[i].width_ = offsetX;
        }
        maxLineWidth_ = std::max(maxLineWidth_, lineMetrics_[i].width_);
    }
}

int TypographyImpl::ComputeStrut()
{
    strut_ = {};

    bool strutValid = typographyStyle_.useLineStyle_ && typographyStyle_.lineStyle_.fontSize_ >= 0;
    if (!strutValid) {
        return 0;
    }

    auto fontCollection = fontProviders_->GenerateFontCollection(
        typographyStyle_.lineStyle_.fontFamilies_);
    if (fontCollection == nullptr) {
        LOG2EX(ERROR) << "fontCollection is null";
        return 1;
    }

    FontStyles style(typographyStyle_.lineStyle_.fontWeight_, typographyStyle_.lineStyle_.fontStyle_);
    auto typeface = fontCollection->GetTypefaceForFontStyles(style, {}, {});
    if (typeface == nullptr) {
        LOG2EX_DEBUG() << "seek typeface failed";
        return 1;
    }

    TexgineFontMetrics strutMetrics;
    TexgineFont font;
    font.SetTypeface(typeface->Get());
    font.SetSize(typographyStyle_.lineStyle_.fontSize_);
    font.GetMetrics(&strutMetrics);

    double strutLeading = typographyStyle_.lineStyle_.spacingScale_.value_or(0) * typographyStyle_.lineStyle_.fontSize_;
    auto leading = strutLeading;
    if (typographyStyle_.lineStyle_.heightOnly_) {
        double metricsHeight = -*strutMetrics.fAscent_ + *strutMetrics.fDescent_;
        double scale = typographyStyle_.lineStyle_.heightScale_ * typographyStyle_.lineStyle_.fontSize_;
        strut_.ascent_ = (-(*strutMetrics.fAscent_) / metricsHeight) * scale;
        strut_.descent_ = (*strutMetrics.fDescent_ / metricsHeight) * scale;
    } else {
        strut_.ascent_ = -(*strutMetrics.fAscent_);
        strut_.descent_ = *strutMetrics.fDescent_;
        leading = leading == 0 ? *strutMetrics.fLeading_ : strutLeading;
    }
    strut_.halfLeading_ = leading * HALF;
    return 0;
}

int TypographyImpl::UpdateSpanMetrics(VariantSpan &span, double &coveredAscent)
{
    auto style = span.GetTextStyle();
    TexgineFontMetrics metrics;
    if (auto ts = span.TryToTextSpan(); ts != nullptr) {
        metrics = ts->tmetrics_;
    } else {
        auto as = span.TryToAnySpan();
        auto families = style.fontFamilies_;
        if (families.empty()) {
            families = typographyStyle_.fontFamilies_;
        }

        auto fontCollection = fontProviders_->GenerateFontCollection(families);
        FontStyles fs(style.fontWeight_, style.fontStyle_);
        auto typeface = fontCollection->GetTypefaceForChar(0xFFFC, fs, "Latn", style.locale_);
        if (typeface == nullptr) {
            typeface = fontCollection->GetTypefaceForFontStyles(fs, "Latn", style.locale_);
        }

        TexgineFont font;
        font.SetTypeface(typeface->Get());
        font.SetSize(style.fontSize_);
        font.GetMetrics(&metrics);
    }

    bool onlyUseStrut = typographyStyle_.useLineStyle_;
    onlyUseStrut = onlyUseStrut && (typographyStyle_.lineStyle_.fontSize_ >= 0);
    onlyUseStrut = onlyUseStrut && typographyStyle_.lineStyle_.only_;
    double ascent = -*metrics.fAscent_;
    if (!onlyUseStrut) {
        double coveredDescent = 0;
        if (style.heightOnly_) {
            double metricsHeight = -*metrics.fAscent_ + *metrics.fDescent_;
            coveredAscent = (-*metrics.fAscent_ / metricsHeight) * style.heightScale_ * style.fontSize_;
            coveredDescent = (*metrics.fDescent_ / metricsHeight) * style.heightScale_ * style.fontSize_;
        } else {
            coveredAscent = (-*metrics.fAscent_ + *metrics.fLeading_ * HALF);
            coveredDescent = (*metrics.fDescent_ + *metrics.fLeading_ * HALF);
        }

        if (auto as = span.TryToAnySpan(); as != nullptr) {
            UpadateAnySpanMetrics(as, coveredAscent, coveredDescent);
            ascent = coveredAscent;
        }

        lineMaxCoveredAscent_.back() = std::max(coveredAscent, lineMaxCoveredAscent_.back());
        lineMaxCoveredDescent_.back() = std::max(coveredDescent, lineMaxCoveredDescent_.back());
    }

    lineMaxAscent_.back() = std::max(lineMaxAscent_.back(), ascent);
    return 0;
}

void TypographyImpl::UpadateAnySpanMetrics(std::shared_ptr<AnySpan> &span, double &coveredAscent,
    double &coveredDescent)
{
    if (span == nullptr) {
        throw TEXGINE_EXCEPTION(InvalidArgument);
    }

    double as = coveredAscent;
    double de = coveredDescent;
    double aj = span->GetBaseline() == TextBaseline::IDEOGRAPHIC ? -de * HALF : 0;
    double lo = span->GetLineOffset();
    double he = span->GetHeight();

    using CalcAscentFunc = std::function<double()>;
    std::map<AnySpanAlignment, CalcAscentFunc> calcMap = {
        {AnySpanAlignment::OFFSET_AT_BASELINE, [&] { return aj + lo; }},
        {AnySpanAlignment::ABOVE_BASELINE,     [&] { return aj + he; }},
        {AnySpanAlignment::BELOW_BASELINE,     [&] { return -aj; }},
        {AnySpanAlignment::TOP_OF_ROW_BOX,     [&] { return as; }},
        {AnySpanAlignment::BOTTOM_OF_ROW_BOX,  [&] { return he - de; }},
        {AnySpanAlignment::CENTER_OF_ROW_BOX,  [&] { return (as - de + he) * HALF; }},
    };

    coveredAscent = calcMap[span->GetAlignment()]();
    coveredDescent = he - coveredAscent;
}

void TypographyImpl::Paint(TexgineCanvas &canvas, double offsetx, double offsety)
{
    for (auto &metric : lineMetrics_) {
        for (auto &span : metric.lineSpans_) {
            span.PaintShadow(canvas, offsetx + span.GetOffsetX(), offsety + span.GetOffsetY());
        }

        for (auto &span : metric.lineSpans_) {
            span.Paint(canvas, offsetx + span.GetOffsetX(), offsety + span.GetOffsetY());
        }
    }
}

std::vector<TextRect> TypographyImpl::GetTextRectsByBoundary(Boundary boundary, TextRectHeightStyle heightStyle,
    TextRectWidthStyle widthStyle) const
{
    if (boundary.leftIndex_ > boundary.rightIndex_ || boundary.leftIndex_ < 0 || boundary.rightIndex_ < 0) {
        LOG2EX(ERROR) << "the box range is error";
        return {};
    }
    std::vector<TextRect> totalBoxes;
    for (auto i = 0; i < lineMetrics_.size(); i++) {
        std::vector<TextRect> lineBoxes;
        auto baseline = baselines_[i];
        auto as = lineMaxAscent_[i];
        auto ca = lineMaxCoveredAscent_[i];
        auto cd = lineMaxCoveredDescent_[i];
        auto hl = ca - as;
        auto fl = i == 0 ? 1 : 0;
        auto ll = i == lineMetrics_.size() - 1 ? 1 : 0;
        constexpr auto tgh = TextRectHeightStyle::TIGHT;
        constexpr auto ctb = TextRectHeightStyle::COVER_TOP_AND_BOTTOM;
        constexpr auto chf = TextRectHeightStyle::COVER_HALF_TOP_AND_BOTTOM;
        constexpr auto ctp = TextRectHeightStyle::COVER_TOP;
        constexpr auto cbm = TextRectHeightStyle::COVER_BOTTOM;

        std::map<TextRectHeightStyle, std::function<struct CalcResult()>> calcMap = {
        {tgh, [&] { return CalcResult{false}; }},
        {ctb, [&] { return CalcResult{true, as, cd}; }},
        {chf, [&] { return CalcResult{true, as + fl * hl * HALF, cd + ll * hl * HALF}; }},
        {ctp, [&] { return CalcResult{true, as + fl * hl, cd}; }},
        {cbm, [&] { return CalcResult{true, as, cd + ll * hl}; }},
        {TextRectHeightStyle::FOLLOW_BY_LINE_STYLE, [&] {
            if (typographyStyle_.useLineStyle_ && typographyStyle_.lineStyle_.fontSize_ >= 0) {
                return CalcResult{true, strut_.ascent_, strut_.descent_};
                }
                return CalcResult{false};
            }},
        };

        const auto &result = calcMap[heightStyle]();
        ComputeSpans(i, baseline, result, lineBoxes);
        if (widthStyle == TextRectWidthStyle::MAX_WIDTH) {
            if (lineBoxes.empty()) {
                LOG2EX(ERROR) << "rects is null";
            } else {
                *lineBoxes.back().rect_.fRight_ += maxLineWidth_ - lineMetrics_[i].width_;
            }
        }
        totalBoxes.insert(totalBoxes.end(), lineBoxes.begin(), lineBoxes.end());
    }
    return MergeRects(totalBoxes, boundary);
}

void TypographyImpl::ComputeSpans(int i, double baseline, const CalcResult &result,
    std::vector<TextRect> &lineBoxes) const
{
    for (auto &span : lineMetrics_[i].lineSpans_) {
        if (span == nullptr) {
            continue;
        }

        std::vector<TextRect> spanBoxes;
        auto offsetX = span.GetOffsetX();
        auto offsetY = span.GetOffsetY();

        if (auto as = span.TryToAnySpan(); as != nullptr) {
            auto rect = TexgineRect::MakeXYWH(offsetX, offsetY, span.GetWidth(), span.GetHeight());
            spanBoxes.push_back({.rect_ = rect, .direction_ = TextDirection::LTR});
        }

        if (auto ts = span.TryToTextSpan(); ts != nullptr) {
            double top = *(ts->tmetrics_.fAscent_);
            double height = *(ts->tmetrics_.fDescent_) - *(ts->tmetrics_.fAscent_);

            std::vector<TextRect> boxes;
            double width = 0.0;
            for (const auto &gw : ts->glyphWidths_) {
                auto rect = TexgineRect::MakeXYWH(offsetX + width, offsetY + top, gw, height);
                boxes.push_back({.rect_ = rect, .direction_ = TextDirection::LTR});
                width += gw;
            }

            spanBoxes.insert(spanBoxes.end(), boxes.begin(), boxes.end());
        }

        if (result.need_) {
            for (auto &box : spanBoxes) {
                *(box.rect_.fTop_) = baseline - result.ascent_;
                *(box.rect_.fBottom_) = baseline + result.descent_;
            }
        }

        lineBoxes.insert(lineBoxes.end(), spanBoxes.begin(), spanBoxes.end());
    }
}

std::vector<TextRect> TypographyImpl::MergeRects(const std::vector<TextRect> &boxes, Boundary boundary) const
{
    if (boundary.leftIndex_ > boxes.size()) {
        return {};
    }

    if (boundary.rightIndex_ > boxes.size()) {
        boundary.rightIndex_ = boxes.size();
    }

    std::optional<TextRect> pre = std::nullopt;
    std::vector<TextRect> rects;
    for (auto it = boxes.cbegin() + boundary.leftIndex_; it < boxes.cbegin() + boundary.rightIndex_; it++) {
        const auto &rect = *it;
        if (!pre.has_value()) {
            pre = rect;
            continue;
        }

        if (*pre->rect_.fTop_ == *rect.rect_.fTop_ && *pre->rect_.fBottom_ == *rect.rect_.fBottom_ &&
            std::fabs(*pre->rect_.fRight_ - *rect.rect_.fLeft_) < MINDEV) {
            *pre->rect_.fRight_ = *rect.rect_.fRight_;
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
    for (auto &line : lineMetrics_) {
        for (auto &span : line.lineSpans_) {
            if (span == nullptr || span.TryToTextSpan() != nullptr) {
                continue;
            }

            auto as = span.TryToAnySpan();
            auto rect = TexgineRect::MakeXYWH(span.GetOffsetX(), span.GetOffsetY(), span.GetWidth(), span.GetHeight());
            rects.push_back({.rect_ = rect, .direction_ = TextDirection::LTR});
        }
    }

    return rects;
}

void TypographyImpl::ApplyAlignment()
{
    TextAlign align_ = typographyStyle_.GetEquivalentAlign();
    for (auto &line : lineMetrics_) {
        double typographyOffsetX = 0.0;
        if (TextAlign::RIGHT == align_ || (TextAlign::JUSTIFY == align_ &&
            TextDirection::RTL == typographyStyle_.direction_)) {
            typographyOffsetX = maxWidth_ - line.width_;
        } else if (TextAlign::CENTER == align_) {
            typographyOffsetX = (maxWidth_ - line.width_) * HALF;
        }

        for (auto &span : line.lineSpans_) {
            span.AdjustOffsetX(typographyOffsetX);
        }
        line.indent_ = typographyOffsetX;
    }
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
