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

#include "paragraph_impl.h"

#include <algorithm>
#include <numeric>

#include "include/core/SkMatrix.h"
#include "drawing_painter_impl.h"
#include "skia_adapter/skia_convert_utils.h"

namespace OHOS {
namespace Rosen {
namespace SPText {
namespace skt = skia::textlayout;
using PaintID = skt::ParagraphPainter::PaintID;

namespace {
FontWeight GetTxtFontWeight(int fontWeight)
{
    constexpr int minWeight = static_cast<int>(FontWeight::W100);
    constexpr int maxWeight = static_cast<int>(FontWeight::W900);

    int weight = std::clamp((fontWeight - 100) / 100, minWeight, maxWeight);
    return static_cast<FontWeight>(weight);
}

FontStyle GetTxtFontStyle(SkFontStyle::Slant slant)
{
    return slant == SkFontStyle::Slant::kUpright_Slant ?
        FontStyle::NORMAL : FontStyle::ITALIC;
}

std::vector<TextBox> GetTxtTextBoxes(const std::vector<skt::TextBox>& skiaBoxes)
{
    std::vector<TextBox> boxes;
    for (const skt::TextBox& box : skiaBoxes) {
        boxes.emplace_back(box.rect, static_cast<TextDirection>(box.direction));
    }
    return boxes;
}
} // anonymous namespace

ParagraphImpl::ParagraphImpl(std::unique_ptr<skt::Paragraph> paragraph, std::vector<PaintRecord>&& paints)
    : paragraph_(std::move(paragraph)), paints_(paints)
{}

double ParagraphImpl::GetMaxWidth()
{
    return paragraph_->getMaxWidth();
}

double ParagraphImpl::GetHeight()
{
    return paragraph_->lineNumber() == 0 ? 0 : paragraph_->getHeight();
}

double ParagraphImpl::GetLongestLine()
{
    return paragraph_->getLongestLine();
}

double ParagraphImpl::GetMinIntrinsicWidth()
{
    return paragraph_->getMinIntrinsicWidth();
}

double ParagraphImpl::GetMaxIntrinsicWidth()
{
    return paragraph_->getMaxIntrinsicWidth();
}

double ParagraphImpl::GetAlphabeticBaseline()
{
    return paragraph_->getAlphabeticBaseline();
}

double ParagraphImpl::GetIdeographicBaseline()
{
    return paragraph_->getIdeographicBaseline();
}

bool ParagraphImpl::DidExceedMaxLines()
{
    return paragraph_->didExceedMaxLines();
}

size_t ParagraphImpl::GetLineCount() const
{
    return paragraph_->lineNumber();
}

void ParagraphImpl::SetIndents(const std::vector<float>& indents)
{
    paragraph_->setIndents(indents);
}

void ParagraphImpl::Layout(double width)
{
    lineMetrics_.reset();
    lineMetricsStyles_.clear();
    paragraph_->layout(width);
}

double ParagraphImpl::GetGlyphsBoundsTop()
{
    return paragraph_->getGlyphsBoundsTop();
}

double ParagraphImpl::GetGlyphsBoundsBottom()
{
    return paragraph_->getGlyphsBoundsBottom();
}

double ParagraphImpl::GetGlyphsBoundsLeft()
{
    return paragraph_->getGlyphsBoundsLeft();
}

double ParagraphImpl::GetGlyphsBoundsRight()
{
    return paragraph_->getGlyphsBoundsRight();
}

OHOS::Rosen::Drawing::FontMetrics ParagraphImpl::MeasureText()
{
    auto skFontMetrics = paragraph_->measureText();
    OHOS::Rosen::Drawing::FontMetrics fontMetrics;
    OHOS::Rosen::Drawing::SkiaConvertUtils::SkFontMetricsCastToDrawingFontMetrics(skFontMetrics, fontMetrics);
    return fontMetrics;
}

void ParagraphImpl::Paint(SkCanvas* canvas, double x, double y)
{
    paragraph_->paint(canvas, x, y);
}

void ParagraphImpl::Paint(Drawing::Canvas* canvas, double x, double y)
{
    RSCanvasParagraphPainter painter(canvas, paints_);
    painter.SetAnimation(animationFunc_);
    paragraph_->paint(&painter, x, y);
}

std::vector<TextBox> ParagraphImpl::GetRectsForRange(size_t start, size_t end,
    RectHeightStyle rectHeightStyle, RectWidthStyle rectWidthStyle)
{
    std::vector<skt::TextBox> boxes =
        paragraph_->getRectsForRange(start, end, static_cast<skt::RectHeightStyle>(rectHeightStyle),
            static_cast<skt::RectWidthStyle>(rectWidthStyle));
    return GetTxtTextBoxes(boxes);
}

std::vector<TextBox> ParagraphImpl::GetRectsForPlaceholders()
{
    return GetTxtTextBoxes(paragraph_->getRectsForPlaceholders());
}

PositionWithAffinity ParagraphImpl::GetGlyphPositionAtCoordinate(double dx, double dy)
{
    skt::PositionWithAffinity pos = paragraph_->getGlyphPositionAtCoordinate(dx, dy);
    return PositionWithAffinity(pos.position, static_cast<Affinity>(pos.affinity));
}

Range<size_t> ParagraphImpl::GetWordBoundary(size_t offset)
{
    skt::SkRange<size_t> range = paragraph_->getWordBoundary(offset);
    return Range<size_t>(range.start, range.end);
}

Range<size_t> ParagraphImpl::GetActualTextRange(int lineNumber, bool includeSpaces)
{
    if (lineNumber >=0 && lineNumber <= static_cast<int>(paragraph_->lineNumber())) {
        skt::SkRange<size_t> range = paragraph_->getActualTextRange(lineNumber, includeSpaces);
        return Range<size_t>(range.start, range.end);
    } else {
        return Range<size_t>(0, 0);
    }
}

std::vector<LineMetrics>& ParagraphImpl::GetLineMetrics()
{
    if (!lineMetrics_) {
        std::vector<skt::LineMetrics> metrics;
        paragraph_->getLineMetrics(metrics);

        lineMetrics_.emplace();
        lineMetricsStyles_.reserve(std::accumulate(metrics.begin(), metrics.end(), 0,
            [](const int a, const skt::LineMetrics& b) { return a + b.fLineMetrics.size(); }));

        for (const skt::LineMetrics& skm : metrics) {
            LineMetrics& txtm = lineMetrics_->emplace_back();
            txtm.startIndex = skm.fStartIndex;
            txtm.endExcludingWhitespace = skm.fEndExcludingWhitespaces;
            txtm.endIncludingNewline = skm.fEndIncludingNewline;
            txtm.hardBreak = skm.fHardBreak;
            txtm.ascent = skm.fAscent;
            txtm.descent = skm.fDescent;
            txtm.unscaledAscent = skm.fUnscaledAscent;
            txtm.height = skm.fHeight;
            txtm.width = skm.fWidth;
            txtm.left = skm.fLeft;
            txtm.baseline = skm.fBaseline;
            txtm.lineNumber = skm.fLineNumber;

            for (const auto& [index, styleMtrics] : skm.fLineMetrics) {
                lineMetricsStyles_.push_back(SkStyleToTextStyle(*styleMtrics.text_style));
                txtm.runMetrics.emplace(std::piecewise_construct, std::forward_as_tuple(index),
                    std::forward_as_tuple(&lineMetricsStyles_.back(), styleMtrics.font_metrics));
            }
        }
    }

    return lineMetrics_.value();
}

bool ParagraphImpl::GetLineMetricsAt(int lineNumber, skt::LineMetrics* lineMetrics) const
{
    return paragraph_->getLineMetricsAt(lineNumber, lineMetrics);
}

TextStyle ParagraphImpl::SkStyleToTextStyle(const skt::TextStyle& skStyle)
{
    TextStyle txt;

    txt.color = skStyle.getColor();
    txt.decoration = static_cast<TextDecoration>(skStyle.getDecorationType());
    txt.decorationColor = skStyle.getDecorationColor();
    txt.decorationStyle = static_cast<TextDecorationStyle>(skStyle.getDecorationStyle());
    txt.decorationThicknessMultiplier = SkScalarToDouble(skStyle.getDecorationThicknessMultiplier());
    txt.fontWeight = GetTxtFontWeight(skStyle.getFontStyle().weight());
    txt.fontStyle = GetTxtFontStyle(skStyle.getFontStyle().slant());

    txt.baseline = static_cast<TextBaseline>(skStyle.getTextBaseline());

    for (const SkString& fontFamily : skStyle.getFontFamilies()) {
        txt.fontFamilies.emplace_back(fontFamily.c_str());
    }

    txt.fontSize = SkScalarToDouble(skStyle.getFontSize());
    txt.letterSpacing = SkScalarToDouble(skStyle.getLetterSpacing());
    txt.wordSpacing = SkScalarToDouble(skStyle.getWordSpacing());
    txt.height = SkScalarToDouble(skStyle.getHeight());

    txt.locale = skStyle.getLocale().c_str();
    if (skStyle.hasBackground()) {
        PaintID backgroundId = std::get<PaintID>(skStyle.getBackgroundPaintOrID());
        txt.background = paints_[backgroundId];
    }
    if (skStyle.hasForeground()) {
        PaintID foregroundId = std::get<PaintID>(skStyle.getForegroundPaintOrID());
        txt.foreground = paints_[foregroundId];
    }

    txt.textShadows.clear();
    for (const skt::TextShadow& skShadow : skStyle.getShadows()) {
        TextShadow shadow;
        shadow.offset = skShadow.fOffset;
        shadow.blurSigma = skShadow.fBlurSigma;
        shadow.color = skShadow.fColor;
        txt.textShadows.emplace_back(shadow);
    }

    return txt;
}
} // namespace SPText
} // namespace Rosen
} // namespace OHOS
