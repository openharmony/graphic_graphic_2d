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

#include "convert.h"
#include "drawing_painter_impl.h"
#include "text_font_utils.h"
#include "include/core/SkMatrix.h"
#include "modules/skparagraph/include/Paragraph.h"
#include "modules/skparagraph/include/TextStyle.h"
#include "paragraph_builder_impl.h"
#include "skia_adapter/skia_convert_utils.h"
#include "symbol_engine/hm_symbol_run.h"
#include "text/font_metrics.h"
#include "text_line_impl.h"
#include "utils/text_log.h"
#include "utils/text_trace.h"

namespace OHOS {
namespace Rosen {
namespace SPText {
using PaintID = skt::ParagraphPainter::PaintID;

namespace {
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
{
    threadId_ = pthread_self();
}

double ParagraphImpl::GetMaxWidth()
{
    RecordDifferentPthreadCall(__FUNCTION__);
    return paragraph_->getMaxWidth();
}

double ParagraphImpl::GetHeight()
{
    RecordDifferentPthreadCall(__FUNCTION__);
    return paragraph_->lineNumber() == 0 ? 0 : paragraph_->getHeight();
}

double ParagraphImpl::GetLongestLine()
{
    RecordDifferentPthreadCall(__FUNCTION__);
    return paragraph_->getLongestLine();
}

double ParagraphImpl::GetLongestLineWithIndent()
{
    RecordDifferentPthreadCall(__FUNCTION__);
    return paragraph_->getLongestLineWithIndent();
}

double ParagraphImpl::GetMinIntrinsicWidth()
{
    RecordDifferentPthreadCall(__FUNCTION__);
    return paragraph_->getMinIntrinsicWidth();
}

double ParagraphImpl::GetMaxIntrinsicWidth()
{
    RecordDifferentPthreadCall(__FUNCTION__);
    return paragraph_->getMaxIntrinsicWidth();
}

double ParagraphImpl::GetAlphabeticBaseline()
{
    RecordDifferentPthreadCall(__FUNCTION__);
    return paragraph_->getAlphabeticBaseline();
}

double ParagraphImpl::GetIdeographicBaseline()
{
    RecordDifferentPthreadCall(__FUNCTION__);
    return paragraph_->getIdeographicBaseline();
}

bool ParagraphImpl::DidExceedMaxLines()
{
    RecordDifferentPthreadCall(__FUNCTION__);
    return paragraph_->didExceedMaxLines();
}

size_t ParagraphImpl::GetLineCount() const
{
    RecordDifferentPthreadCall(__FUNCTION__);
    if (paragraph_ == nullptr || paragraph_->GetMaxLines() == 0) {
        return 0;
    }
    return paragraph_->lineNumber();
}

void ParagraphImpl::MarkDirty()
{
    RecordDifferentPthreadCall(__FUNCTION__);
    if (paragraph_ == nullptr) {
        return;
    }
    paragraph_->markDirty();
}

int32_t ParagraphImpl::GetUnresolvedGlyphsCount()
{
    RecordDifferentPthreadCall(__FUNCTION__);
    if (paragraph_ == nullptr) {
        return 0;
    }
    return paragraph_->unresolvedGlyphs();
}

void ParagraphImpl::UpdateFontSize(size_t from, size_t to, float fontSize)
{
    RecordDifferentPthreadCall(__FUNCTION__);
    if (paragraph_ == nullptr) {
        return;
    }
    paragraph_->updateFontSize(from, to, fontSize);
}

void ParagraphImpl::SetIndents(const std::vector<float>& indents)
{
    RecordDifferentPthreadCall(__FUNCTION__);
    paragraph_->setIndents(indents);
}

float ParagraphImpl::DetectIndents(size_t index)
{
    RecordDifferentPthreadCall(__FUNCTION__);
    return paragraph_->detectIndents(index);
}

void ParagraphImpl::InitSymbolRuns()
{
    std::call_once(initSymbolRunsFlag_, [&paints_ = paints_, &hmSymbols_ = hmSymbols_,
        &animationFunc_ = animationFunc_]() {
        for (const PaintRecord& p : paints_) {
            if (!p.isSymbolGlyph) {
                continue;
            }

            std::shared_ptr<HMSymbolRun> hmSymbolRun = std::make_shared<HMSymbolRun>();
            hmSymbolRun->SetAnimation(animationFunc_);
            hmSymbolRun->SetSymbolUid(p.symbol.GetSymbolUid());
            hmSymbolRun->SetSymbolTxt(p.symbol);
            hmSymbols_.push_back(std::move(hmSymbolRun));
        }
    });
}

void ParagraphImpl::Layout(double width)
{
    RecordDifferentPthreadCall(__FUNCTION__);
    lineMetrics_.reset();
    lineMetricsStyles_.clear();
    InitSymbolRuns();
    paragraph_->layout(width);
}

double ParagraphImpl::GetGlyphsBoundsTop()
{
    RecordDifferentPthreadCall(__FUNCTION__);
    return paragraph_->getGlyphsBoundsTop();
}

double ParagraphImpl::GetGlyphsBoundsBottom()
{
    RecordDifferentPthreadCall(__FUNCTION__);
    return paragraph_->getGlyphsBoundsBottom();
}

double ParagraphImpl::GetGlyphsBoundsLeft()
{
    RecordDifferentPthreadCall(__FUNCTION__);
    return paragraph_->getGlyphsBoundsLeft();
}

double ParagraphImpl::GetGlyphsBoundsRight()
{
    RecordDifferentPthreadCall(__FUNCTION__);
    return paragraph_->getGlyphsBoundsRight();
}

OHOS::Rosen::Drawing::FontMetrics ParagraphImpl::MeasureText()
{
    RecordDifferentPthreadCall(__FUNCTION__);
    return paragraph_->measureText();
}

void ParagraphImpl::Paint(SkCanvas* canvas, double x, double y)
{
    RecordDifferentPthreadCall(__FUNCTION__);
    paragraph_->paint(canvas, x, y);
}

void ParagraphImpl::Paint(Drawing::Canvas* canvas, double x, double y)
{
    RecordDifferentPthreadCall(__FUNCTION__);
    RSCanvasParagraphPainter painter(canvas, paints_);
    painter.SetAnimation(animationFunc_);
    painter.SetHmSymbols(hmSymbols_);
    paragraph_->paint(&painter, x, y);
}

void ParagraphImpl::Paint(Drawing::Canvas* canvas, Drawing::Path* path, double hOffset, double vOffset)
{
    RecordDifferentPthreadCall(__FUNCTION__);
    RSCanvasParagraphPainter painter(canvas, paints_);
    painter.SetAnimation(animationFunc_);
    paragraph_->paint(&painter, path, hOffset, vOffset);
}

std::vector<TextBox> ParagraphImpl::GetRectsForRange(size_t start, size_t end,
    RectHeightStyle rectHeightStyle, RectWidthStyle rectWidthStyle)
{
    RecordDifferentPthreadCall(__FUNCTION__);
    std::vector<skt::TextBox> boxes =
        paragraph_->getRectsForRange(start, end, static_cast<skt::RectHeightStyle>(rectHeightStyle),
            static_cast<skt::RectWidthStyle>(rectWidthStyle));
    return GetTxtTextBoxes(boxes);
}

std::vector<TextBox> ParagraphImpl::GetRectsForPlaceholders()
{
    RecordDifferentPthreadCall(__FUNCTION__);
    return GetTxtTextBoxes(paragraph_->getRectsForPlaceholders());
}

PositionWithAffinity ParagraphImpl::GetGlyphPositionAtCoordinate(double dx, double dy)
{
    RecordDifferentPthreadCall(__FUNCTION__);
    skt::PositionWithAffinity pos = paragraph_->getGlyphPositionAtCoordinate(dx, dy);
    return PositionWithAffinity(pos.position, static_cast<Affinity>(pos.affinity));
}

Range<size_t> ParagraphImpl::GetWordBoundary(size_t offset)
{
    RecordDifferentPthreadCall(__FUNCTION__);
    skt::SkRange<size_t> range = paragraph_->getWordBoundary(offset);
    return Range<size_t>(range.start, range.end);
}

Range<size_t> ParagraphImpl::GetActualTextRange(int lineNumber, bool includeSpaces)
{
    RecordDifferentPthreadCall(__FUNCTION__);
    if (lineNumber >= 0 && lineNumber < static_cast<int>(paragraph_->lineNumber())) {
        skt::SkRange<size_t> range = paragraph_->getActualTextRange(lineNumber, includeSpaces);
        if (range == skia::textlayout::EMPTY_TEXT) {
            TEXT_LOGE_LIMIT3_MIN("Invalid line number %{public}d", lineNumber);
            return Range<size_t>(0, 0);
        } else {
            return Range<size_t>(range.start, range.end);
        }
    } else {
        TEXT_LOGE_LIMIT3_MIN("Invalid line number %{public}d", lineNumber);
        return Range<size_t>(0, 0);
    }
}

Range<size_t> ParagraphImpl::GetEllipsisTextRange()
{
    RecordDifferentPthreadCall(__FUNCTION__);
    skt::SkRange<size_t> range = paragraph_->getEllipsisTextRange();
    return Range<size_t>(range.start, range.end);
}

std::vector<skt::LineMetrics> ParagraphImpl::GetLineMetrics()
{
    RecordDifferentPthreadCall(__FUNCTION__);
    std::vector<skt::LineMetrics> metrics;
    if (!lineMetrics_) {
        paragraph_->getLineMetrics(metrics);
    }
    return metrics;
}

bool ParagraphImpl::GetLineMetricsAt(int lineNumber, skt::LineMetrics* lineMetrics) const
{
    RecordDifferentPthreadCall(__FUNCTION__);
    return paragraph_->getLineMetricsAt(lineNumber, lineMetrics);
}

void ParagraphImpl::GetExtraTextStyleAttributes(const skia::textlayout::TextStyle& skStyle, TextStyle& textstyle)
{
    for (const auto& [tag, value] : skStyle.getFontFeatures()) {
        textstyle.fontFeatures.SetFeature(tag.c_str(), value);
    }
    textstyle.textShadows.clear();
    for (const skt::TextShadow& skShadow : skStyle.getShadows()) {
        TextShadow shadow;
        shadow.offset = skShadow.fOffset;
        shadow.blurSigma = skShadow.fBlurSigma;
        shadow.color = skShadow.fColor;
        textstyle.textShadows.emplace_back(shadow);
    }
}

TextStyle ParagraphImpl::SkStyleToTextStyle(const skt::TextStyle& skStyle)
{
    RecordDifferentPthreadCall(__FUNCTION__);

    TextStyle txt;
    txt.color = skStyle.getColor();
    txt.decoration = static_cast<TextDecoration>(skStyle.getDecorationType());
    txt.decorationColor = skStyle.getDecorationColor();
    txt.decorationStyle = static_cast<TextDecorationStyle>(skStyle.getDecorationStyle());
    txt.decorationThicknessMultiplier = SkScalarToDouble(skStyle.getDecorationThicknessMultiplier());
    txt.fontWeight = TextFontUtils::GetTxtFontWeight(skStyle.getFontStyle().GetWeight());
    txt.fontStyle = TextFontUtils::GetTxtFontStyle(skStyle.getFontStyle().GetSlant());

    txt.baseline = static_cast<TextBaseline>(skStyle.getTextBaseline());

    for (const SkString& fontFamily : skStyle.getFontFamilies()) {
        txt.fontFamilies.emplace_back(fontFamily.c_str());
    }

    txt.fontSize = SkScalarToDouble(skStyle.getFontSize());
    txt.fontWidth = static_cast<FontWidth>(skStyle.getFontStyle().GetWidth());
    txt.styleId = skStyle.getStyleId();
    txt.letterSpacing = SkScalarToDouble(skStyle.getLetterSpacing());
    txt.wordSpacing = SkScalarToDouble(skStyle.getWordSpacing());
    txt.height = SkScalarToDouble(skStyle.getHeight());
    txt.heightOverride = skStyle.getHeightOverride();
    txt.halfLeading = skStyle.getHalfLeading();
    txt.baseLineShift = SkScalarToDouble(skStyle.getBaselineShift());
    txt.locale = skStyle.getLocale().c_str();
    txt.backgroundRect = { skStyle.getBackgroundRect().color, skStyle.getBackgroundRect().leftTopRadius,
        skStyle.getBackgroundRect().rightTopRadius, skStyle.getBackgroundRect().rightBottomRadius,
        skStyle.getBackgroundRect().leftBottomRadius };
    if (skStyle.hasBackground()) {
        PaintID backgroundId = std::get<PaintID>(skStyle.getBackgroundPaintOrID());
        if ((0 <= backgroundId) && (backgroundId < static_cast<int>(paints_.size()))) {
            txt.background = paints_[backgroundId];
        } else {
            TEXT_LOGW("Invalid background id %{public}d", backgroundId);
        }
    }
    if (skStyle.hasForeground()) {
        PaintID foregroundId = std::get<PaintID>(skStyle.getForegroundPaintOrID());
        if ((0 <= foregroundId) && (foregroundId < static_cast<int>(paints_.size()))) {
            txt.foreground = paints_[foregroundId];
        } else {
            TEXT_LOGW("Invalid foreground id %{public}d", foregroundId);
        }
    }
    GetExtraTextStyleAttributes(skStyle, txt);
    return txt;
}

Drawing::FontMetrics ParagraphImpl::GetFontMetricsResult(const SPText::TextStyle& textStyle)
{
    RecordDifferentPthreadCall(__FUNCTION__);

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
    RecordDifferentPthreadCall(__FUNCTION__);
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
    RecordDifferentPthreadCall(__FUNCTION__);
    if (!paragraph_) {
        return nullptr;
    }
    std::vector<PaintRecord> paints = paints_;
    std::unique_ptr<skt::Paragraph> sktParagraph = paragraph_->CloneSelf();
    std::unique_ptr<ParagraphImpl> paragraph = std::make_unique<ParagraphImpl>(std::move(sktParagraph),
        std::move(paints));
    return paragraph;
}

void ParagraphImpl::UpdateColor(size_t from, size_t to, const RSColor& color,
    skia::textlayout::UtfEncodeType encodeType)
{
    RecordDifferentPthreadCall(__FUNCTION__);
    if (!paragraph_) {
        return;
    }
    auto unresolvedPaintID = paragraph_->updateColor(from, to,
        SkColorSetARGB(color.GetAlpha(), color.GetRed(), color.GetGreen(), color.GetBlue()), encodeType);
    for (auto paintID : unresolvedPaintID) {
        paints_[paintID].SetColor(color);
    }
}

void ParagraphImpl::UpdatePaintsBySkiaBlock(skt::Block& skiaBlock, const std::optional<RSBrush>& brush)
{
    PaintID foregroundId = std::get<PaintID>(skiaBlock.fStyle.getForegroundPaintOrID());
    if ((foregroundId < 0) || (foregroundId >= static_cast<int>(paints_.size()))) {
        return;
    }
    if (paints_[foregroundId].isSymbolGlyph) {
        return;
    }
    paints_[foregroundId].brush = brush;
}

void ParagraphImpl::UpdateForegroundBrushWithValidData(SkTArray<skt::Block, true>& skiaTextStyles,
    const std::optional<RSBrush>& brush)
{
    TEXT_TRACE_FUNC();
    PaintID newId = static_cast<int>(paints_.size());
    bool needAddNewBrush = true;

    for (size_t i = 0; i < skiaTextStyles.size(); i++) {
        skt::Block& skiaBlock = skiaTextStyles[i];
        if (skiaBlock.fStyle.hasForeground()) {
            UpdatePaintsBySkiaBlock(skiaBlock, brush);
        } else {
            skiaBlock.fStyle.setForegroundPaintID(newId);
            if (needAddNewBrush) {
                PaintRecord pr(brush, std::nullopt);
                paints_.push_back(pr);
                needAddNewBrush = false;
            }
        }
    }
}

void ParagraphImpl::UpdateForegroundBrushWithNullopt(SkTArray<skt::Block, true>& skiaTextStyles)
{
    TEXT_TRACE_FUNC();
    for (size_t i = 0; i < skiaTextStyles.size(); i++) {
        skt::Block& skiaBlock = skiaTextStyles[i];
        if (!skiaBlock.fStyle.hasForeground()) {
            continue;
        }
        UpdatePaintsBySkiaBlock(skiaBlock, std::nullopt);
    }
}

void ParagraphImpl::UpdateForegroundBrush(const TextStyle& spTextStyle)
{
    RecordDifferentPthreadCall(__FUNCTION__);
    if (paragraph_ == nullptr) {
        return;
    }

    SkTArray<skt::Block, true>& skiaTextStyles = paragraph_->exportTextStyles();

    if (spTextStyle.foreground.has_value() && spTextStyle.foreground.value().brush.has_value()) {
        UpdateForegroundBrushWithValidData(skiaTextStyles, spTextStyle.foreground.value().brush);
    } else {
        UpdateForegroundBrushWithNullopt(skiaTextStyles);
    }
}

std::vector<TextBlobRecordInfo> ParagraphImpl::GetTextBlobRecordInfo() const
{
    RecordDifferentPthreadCall(__FUNCTION__);
    if (paragraph_ == nullptr) {
        return {};
    }
    std::vector<TextBlobRecordInfo> textBlobRecordInfos;
    std::vector<skt::TextBlobRecordInfo> infos = paragraph_->getTextBlobRecordInfo();
    for (auto& info : infos) {
        TextBlobRecordInfo recordInfo;
        recordInfo.blob = info.fBlob;
        recordInfo.offset = info.fOffset;
        int index = std::get<int>(info.fPaint);
        if (index >= 0 && index < static_cast<int>(paints_.size())) {
            recordInfo.color = paints_[index].color;
        }
        textBlobRecordInfos.emplace_back(recordInfo);
    }
    return textBlobRecordInfos;
}

bool ParagraphImpl::HasEnabledTextEffect() const
{
    RecordDifferentPthreadCall(__FUNCTION__);
    if (paragraph_ == nullptr) {
        return false;
    }
    return paragraph_->hasEnabledTextEffect();
}

void ParagraphImpl::SetTextEffectState(bool state)
{
    RecordDifferentPthreadCall(__FUNCTION__);
    if (paragraph_ == nullptr) {
        return;
    }
    paragraph_->setTextEffectState(state);
}

void ParagraphImpl::RecordDifferentPthreadCall(const char* caller) const
{
    pthread_t currenetThreadId = pthread_self();
    if (threadId_ != currenetThreadId) {
        TEXT_LOGE_LIMIT3_HOUR("New pthread access paragraph builder, old %{public}lu, caller %{public}s",
            threadId_, caller);
        threadId_ = currenetThreadId;
    }
}

Drawing::RectI ParagraphImpl::GeneratePaintRegion(double x, double y)
{
    RecordDifferentPthreadCall("GeneratePaintRegion");
    if (!paragraph_) {
        double left = std::floor(x);
        double top = std::floor(y);
        return Drawing::RectI(left, top, left, top);
    }

    SkIRect skIRect = paragraph_->generatePaintRegion(SkDoubleToScalar(x), SkDoubleToScalar(y));
    return Drawing::RectI(skIRect.left(), skIRect.top(), skIRect.right(), skIRect.bottom());
}

bool ParagraphImpl::IsLayoutDone()
{
    RecordDifferentPthreadCall(__FUNCTION__);
    return paragraph_->getState() >= skt::kFormatted ? true : false;
}

void ParagraphImpl::SetLayoutState(size_t state)
{
    RecordDifferentPthreadCall(__FUNCTION__);
    paragraph_->setState(static_cast<skt::InternalState>(state));
}
} // namespace SPText
} // namespace Rosen
} // namespace OHOS
