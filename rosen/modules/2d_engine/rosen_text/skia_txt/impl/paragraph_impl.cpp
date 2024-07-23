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
#include "text/font_metrics.h"
#include "paragraph_builder_impl.h"
#include "text_line_impl.h"

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

FontStyle GetTxtFontStyle(RSFontStyle::Slant slant)
{
    return slant == RSFontStyle::Slant::UPRIGHT_SLANT ?
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
    : paragraph_(std::move(paragraph)), paints_(std::move(paints))
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

double ParagraphImpl::GetLongestLineWithIndent()
{
    return paragraph_->getLongestLineWithIndent();
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

void ParagraphImpl::MarkDirty()
{
    if (paragraph_ == nullptr) {
        return;
    }
    paragraph_->markDirty();
}

int32_t ParagraphImpl::GetUnresolvedGlyphsCount()
{
    if (paragraph_ == nullptr) {
        return 0;
    }
    return paragraph_->unresolvedGlyphs();
}

void ParagraphImpl::UpdateFontSize(size_t from, size_t to, float fontSize)
{
    if (paragraph_ == nullptr) {
        return;
    }
    paragraph_->updateFontSize(from, to, fontSize);
}

void ParagraphImpl::SetIndents(const std::vector<float>& indents)
{
    paragraph_->setIndents(indents);
}

float ParagraphImpl::DetectIndents(size_t index)
{
    return paragraph_->detectIndents(index);
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
    return paragraph_->measureText();
}

void ParagraphImpl::Paint(SkCanvas* canvas, double x, double y)
{
    paragraph_->paint(canvas, x, y);
}

void ParagraphImpl::Paint(Drawing::Canvas* canvas, double x, double y)
{
    RSCanvasParagraphPainter painter(canvas, paints_);
    painter.SetAnimation(animationFunc_);
    painter.SetParagraphId(id_);
    paragraph_->paint(&painter, x, y);
}

void ParagraphImpl::Paint(Drawing::Canvas* canvas, Drawing::Path* path, double hOffset, double vOffset)
{
    RSCanvasParagraphPainter painter(canvas, paints_);
    painter.SetAnimation(animationFunc_);
    painter.SetParagraphId(id_);
    paragraph_->paint(&painter, path, hOffset, vOffset);
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

std::vector<skia::textlayout::LineMetrics> ParagraphImpl::GetLineMetrics()
{
    std::vector<skt::LineMetrics> metrics;
    if (!lineMetrics_) {
        paragraph_->getLineMetrics(metrics);
    }
    return metrics;
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
    txt.fontWeight = GetTxtFontWeight(skStyle.getFontStyle().GetWeight());
    txt.fontStyle = GetTxtFontStyle(skStyle.getFontStyle().GetSlant());

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

Drawing::FontMetrics ParagraphImpl::GetFontMetricsResult(const SPText::TextStyle& textStyle)
{
    auto skTextStyle = ParagraphBuilderImpl::ConvertTextStyleToSkStyle(textStyle);
    OHOS::Rosen::Drawing::FontMetrics fontMetrics;
    skTextStyle.getFontMetrics(&fontMetrics);
    return fontMetrics;
}

bool ParagraphImpl::GetLineFontMetrics(const size_t lineNumber, size_t& charNumber,
    std::vector<Drawing::FontMetrics>& fontMetrics)
{
    if (!paragraph_) {
        return false;
    }
    return paragraph_->GetLineFontMetrics(lineNumber, charNumber, fontMetrics);
}

std::vector<std::unique_ptr<SPText::TextLineBase>> ParagraphImpl::GetTextLines() const
{
    if (!paragraph_) {
        return {};
    }
    std::vector<std::unique_ptr<skt::TextLineBase>> textLineBases = paragraph_->GetTextLines();
    std::vector<std::unique_ptr<SPText::TextLineBase>> lines;
    for (std::unique_ptr<skt::TextLineBase>& textLineBase : textLineBases) {
        std::unique_ptr<SPText::TextLineImpl> textLinePtr =
            std::make_unique<SPText::TextLineImpl>(std::move(textLineBase), paints_);
        lines.emplace_back(std::move(textLinePtr));
    }
    return lines;
}

std::unique_ptr<Paragraph> ParagraphImpl::CloneSelf()
{
    if (!paragraph_) {
        return nullptr;
    }
    std::vector<PaintRecord> paints = paints_;
    std::unique_ptr<skt::Paragraph> sktParagraph = paragraph_->CloneSelf();
    std::unique_ptr<ParagraphImpl> paragraph = std::make_unique<ParagraphImpl>(std::move(sktParagraph),
        std::move(paints));
    return paragraph;
}

void ParagraphImpl::UpdateColor(size_t from, size_t to, const RSColor& color)
{
    if (!paragraph_) {
        return;
    }
    auto unresolvedPaintID = paragraph_->updateColor(from, to,
        SkColorSetARGB(color.GetAlpha(), color.GetRed(), color.GetGreen(), color.GetBlue()));
    for (auto paintID : unresolvedPaintID) {
        paints_[paintID].SetColor(color);
    }
}

} // namespace SPText
} // namespace Rosen
} // namespace OHOS
