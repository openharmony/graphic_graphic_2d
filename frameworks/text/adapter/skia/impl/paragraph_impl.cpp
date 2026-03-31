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
#include <limits>
#include <numeric>

#include "common_utils/path_util.h"
#include "common_utils/pixel_map_util.h"
#include "txt/sp_convert.h"
#include "drawing_painter_impl.h"
#include "include/DartTypes.h"
#include "include/core/SkMatrix.h"
#include "modules/skparagraph/include/Paragraph.h"
#include "modules/skparagraph/include/TextStyle.h"
#include "paragraph_builder_impl.h"
#ifdef ENABLE_OHOS_ENHANCE
#include "pixel_map.h"
#endif
#include "skia_adapter/skia_convert_utils.h"
#include "static_factory.h"
#include "symbol_engine/hm_symbol_run.h"
#include "text_font_utils.h"
#include "text/font_metrics.h"
#include "text_line_impl.h"
#include "utils/text_log.h"
#include "utils/text_trace.h"

namespace OHOS {
namespace Rosen {
namespace SPText {
using PaintID = skt::ParagraphPainter::PaintID;
constexpr double INFINITE_WIDTH = std::numeric_limits<double>::max();
constexpr size_t INFINITE_RANGE_INDEX = std::numeric_limits<size_t>::max();
// Invalid text range representing an invalid or empty range
constexpr TextRange INVALID_TEXT_RANGE = {0, 0};

namespace {
std::vector<TextBox> GetTxtTextBoxes(const std::vector<skt::TextBox>& skiaBoxes)
{
    std::vector<TextBox> boxes;
    for (const skt::TextBox& box : skiaBoxes) {
        boxes.emplace_back(box.rect, static_cast<TextDirection>(box.direction));
    }
    return boxes;
}

Range<size_t> FromSkRange(const skt::SkRange<size_t>& skRange)
{
    return Range<size_t>(skRange.start, skRange.end);
}

Range<size_t> FromTextRange(const skt::TextRange& textRange)
{
    return Range<size_t>(textRange.start, textRange.end);
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
    if (paragraph_ == nullptr || paragraph_->GetMaxLines() == 0) {
        return 0;
    }
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
    lineMetrics_.reset();
    lineMetricsStyles_.clear();
    InitSymbolRuns();
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
    painter.SetHmSymbols(hmSymbols_);
    paragraph_->paint(&painter, x, y);
}

void ParagraphImpl::Paint(Drawing::Canvas* canvas, Drawing::Path* path, double hOffset, double vOffset)
{
    RSCanvasParagraphPainter painter(canvas, paints_);
    painter.SetAnimation(animationFunc_);
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

Range<size_t> ParagraphImpl::GetEllipsisTextRange() const
{
    skt::SkRange<size_t> range = paragraph_->getEllipsisTextRange();
    return Range<size_t>(range.start, range.end);
}

std::vector<TextRange> ParagraphImpl::GetVisibleTextRanges() const
{
    std::vector<TextRange> visibleRanges;
    BuildFitStrRange(visibleRanges);
    return visibleRanges;
}

std::vector<skt::LineMetrics> ParagraphImpl::GetLineMetrics()
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
    return OHOS::Rosen::SPText::SkStyleToSPTextStyle(skStyle, paints_);
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

PositionWithAffinity ParagraphImpl::GetCharacterPositionAtCoordinate(double dx, double dy, TextEncoding encoding) const
{
    if (paragraph_ == nullptr) {
        TEXT_LOGE("paragraph_ is null");
        return {0, Affinity::DOWNSTREAM};
    }

    skt::PositionWithAffinity pos = paragraph_->getCharacterPositionAtCoordinate(dx, dy,
        static_cast<RSDrawing::TextEncoding>(encoding));
    return PositionWithAffinity(pos.position, static_cast<Affinity>(pos.affinity));
}

Range<size_t> ParagraphImpl::GetCharacterRangeForGlyphRange(size_t glyphStart, size_t glyphEnd,
    Range<size_t>* actualGlyphRange, TextEncoding encoding) const
{
    if (paragraph_ == nullptr) {
        TEXT_LOGE("paragraph_ is null");
        return {0, 0};
    }

    skt::SkRange<size_t> tempGlyphRange;
    skt::TextRange result = paragraph_->getCharacterRangeForGlyphRange(glyphStart, glyphEnd,
        &tempGlyphRange, static_cast<RSDrawing::TextEncoding>(encoding));

    // Only set actualGlyphRange if caller requested it (non-NULL)
    if (actualGlyphRange != nullptr) {
        *actualGlyphRange = FromSkRange(tempGlyphRange);
    }
    return FromTextRange(result);
}

Range<size_t> ParagraphImpl::GetGlyphRangeForCharacterRange(size_t charStart, size_t charEnd,
    Range<size_t>* actualCharRange, TextEncoding encoding) const
{
    if (paragraph_ == nullptr) {
        TEXT_LOGE("paragraph_ is null");
        return {0, 0};
    }

    skt::TextRange actualTextRange;
    skt::SkRange<size_t> result = paragraph_->getGlyphRangeForCharacterRange(charStart, charEnd,
        &actualTextRange, static_cast<RSDrawing::TextEncoding>(encoding));

    // Only set actualCharRange if caller requested it (non-NULL)
    if (actualCharRange != nullptr) {
        *actualCharRange = FromTextRange(actualTextRange);
    }
    return FromSkRange(result);
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

void ParagraphImpl::UpdateColor(size_t from, size_t to, const RSColor& color,
    skia::textlayout::UtfEncodeType encodeType)
{
    if (!paragraph_) {
        return;
    }
    auto unresolvedPaintID = paragraph_->updateColor(from, to,
        SkColorSetARGB(color.GetAlpha(), color.GetRed(), color.GetGreen(), color.GetBlue()), encodeType);
    for (auto paintID : unresolvedPaintID) {
        paints_[paintID].SetColor(color);
    }
    MarkAttributeUpdated();
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
    MarkAttributeUpdated();
}
#ifdef USE_M133_SKIA
void ParagraphImpl::UpdateForegroundBrushWithValidData(skia_private::TArray<skt::Block, true>& skiaTextStyles,
    const std::optional<RSBrush>& brush)
#else
void ParagraphImpl::UpdateForegroundBrushWithValidData(SkTArray<skt::Block, true>& skiaTextStyles,
    const std::optional<RSBrush>& brush)
#endif
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
    MarkAttributeUpdated();
}
#ifdef USE_M133_SKIA
void ParagraphImpl::UpdateForegroundBrushWithNullopt(skia_private::TArray<skt::Block, true>& skiaTextStyles)
#else
void ParagraphImpl::UpdateForegroundBrushWithNullopt(SkTArray<skt::Block, true>& skiaTextStyles)
#endif
{
    TEXT_TRACE_FUNC();
    for (size_t i = 0; i < skiaTextStyles.size(); i++) {
        skt::Block& skiaBlock = skiaTextStyles[i];
        if (!skiaBlock.fStyle.hasForeground()) {
            continue;
        }
        UpdatePaintsBySkiaBlock(skiaBlock, std::nullopt);
    }
    MarkAttributeUpdated();
}

void ParagraphImpl::UpdateForegroundBrush(const TextStyle& spTextStyle)
{
    if (paragraph_ == nullptr) {
        return;
    }

#ifdef USE_M133_SKIA
    skia_private::TArray<skt::Block, true>& skiaTextStyles = paragraph_->exportTextStyles();
#else
    SkTArray<skt::Block, true>& skiaTextStyles = paragraph_->exportTextStyles();
#endif

    if (spTextStyle.foreground.has_value() && spTextStyle.foreground.value().brush.has_value()) {
        UpdateForegroundBrushWithValidData(skiaTextStyles, spTextStyle.foreground.value().brush);
    } else {
        UpdateForegroundBrushWithNullopt(skiaTextStyles);
    }
    MarkAttributeUpdated();
}

std::vector<TextBlobRecordInfo> ParagraphImpl::GetTextBlobRecordInfo() const
{
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

bool ParagraphImpl::HasSkipTextBlobDrawing() const
{
    if (paragraph_ == nullptr) {
        return false;
    }
    return paragraph_->hasSkipTextBlobDrawing();
}

void ParagraphImpl::SetSkipTextBlobDrawing(bool state)
{
    if (paragraph_ == nullptr) {
        return;
    }
    paragraph_->setSkipTextBlobDrawing(state);
}

bool ParagraphImpl::CanPaintAllText() const
{
    if (paragraph_ == nullptr) {
        return false;
    }
    return paragraph_->canPaintAllText();
}

Drawing::RectI ParagraphImpl::GeneratePaintRegion(double x, double y)
{
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
    return paragraph_->getState() >= skt::kFormatted ? true : false;
}

void ParagraphImpl::SetLayoutState(size_t state)
{
    paragraph_->setState(static_cast<skt::InternalState>(state));
}

std::string ParagraphImpl::GetDumpInfo() const
{
    return paragraph_->GetDumpInfo();
}

ParagraphStyle ParagraphImpl::GetParagraphStyle() const
{
    return SkParagraphStyleToParagraphStyle(paragraph_->getParagraphStyle(), paints_);
}

TextProcessState ParagraphImpl::GetProcessState() const
{
    skt::InternalState state = paragraph_->getState();
    TextProcessState resultState = TextProcessState::INIT;
    switch (state) {
        case skt::InternalState::kUnknown:
            resultState = TextProcessState::INIT;
            break;
        case skt::InternalState::kIndexed:
            resultState = TextProcessState::INDEXED;
            break;
        case skt::InternalState::kShaped:
            resultState = TextProcessState::SHAPED;
            break;
        case skt::InternalState::kLineBroken:
            resultState = TextProcessState::LINE_BROKEN;
            break;
        case skt::InternalState::kFormatted:
            resultState = TextProcessState::FORMATTED;
            break;
        case skt::InternalState::kDrawn:
            resultState = TextProcessState::PAINT;
            break;
        default:
            resultState = TextProcessState::INIT;
    }
    if (state < skt::InternalState::kDrawn && updateAttr) {
        resultState = TextProcessState::UPDATE_ATTRIBUTE;
    }
    return resultState;
}

TextDisplayState ParagraphImpl::GetTextDisplayState() const
{
    if (paragraph_->getState() < skt::kFormatted) {
        return TextDisplayState::UNKNOWN;
    }
    skt::SkRange<size_t> range = paragraph_->getEllipsisTextRange();
    if (range.start != INFINITE_RANGE_INDEX) {
        return TextDisplayState::OMITTED;
    }
    if (paragraph_->didExceedMaxLines()) {
        return TextDisplayState::CLIP;
    }
    return TextDisplayState::ALL;
}

#ifdef ENABLE_OHOS_ENHANCE
std::shared_ptr<OHOS::Media::PixelMap> ParagraphImpl::GetTextPathImageByIndex(
    size_t start, size_t end, const ImageOptions& options, bool fill) const
{
    if (start >= end) {
        TEXT_LOGW("Invalid range: [%{public}zu, %{public}zu)", start, end);
        return nullptr;
    }
    
    skt::SkRange<size_t> range{start, end};
    std::vector<skt::PathInfo> pathInfos = paragraph_->getTextPathByClusterRange(range);
    if (pathInfos.empty()) {
        TEXT_LOGD("No path info found for range: [%{public}zu, %{public}zu)", start, end);
        return nullptr;
    }
    
    if (fill) {
        for (size_t i = 0; i < pathInfos.size(); i++) {
            auto& pathInfo = pathInfos[i];
            Drawing::Path tempPath;
            std::vector<Drawing::Path> allPaths;
            Drawing::StaticFactory::PathOutlineDecompose(pathInfo.path, allPaths);
            TextPathUtil::ExtractOuterPath(allPaths, tempPath);
            pathInfo.path = tempPath;
        }
    }
    return TextPixelMapUtil::CreatePixelMap(options, pathInfos);
}
#endif

TextLayoutResult ParagraphImpl::LayoutWithConstraints(const TextRectSize& limitRect)
{
    if (limitRect.width > 0 && limitRect.height > 0) {
        paragraph_->setLayoutConstraintsFlag(true);
        paragraph_->setLayoutConstraintsHeight(limitRect.height);
        paragraph_->setLayoutConstraintsWidth(limitRect.width);
    }

    paragraph_->layout(INFINITE_WIDTH);

    // Build result after layout
    TextLayoutResult layoutResult;
    layoutResult.correctRect.height = GetHeight();
    layoutResult.correctRect.width = GetLongestLineWithIndent();

    BuildFitStrRange(layoutResult.fitStrRange);

    return layoutResult;
}

void ParagraphImpl::BuildFitStrRange(std::vector<TextRange>& fitRanges) const
{
    Range<size_t> ellipsisRange = GetEllipsisTextRange();
    skt::TextRange textRange = paragraph_->getUtf16TextRange();
    skt::TextRange lastLineTextRange = paragraph_->getLineUtf16TextRange(GetLineCount() - 1, true);
    // If there is no ellipsis, the fit range is 0 to end index of last line.
    if (ellipsisRange.start == INFINITE_RANGE_INDEX) {
        fitRanges.push_back({0, lastLineTextRange.end});
        return;
    }

    // If there is one line head ellipsis, the fit range is the text range after ellipsis.
    if (ellipsisRange.start == 0) {
        if (ellipsisRange.end < lastLineTextRange.end) {
            fitRanges.push_back({ellipsisRange.end, lastLineTextRange.end});
        } else {
            fitRanges.push_back(INVALID_TEXT_RANGE);
        }
    } else {
        fitRanges.push_back({textRange.start, ellipsisRange.start});
        // If there is middle ellipsis or multiple line head ellipsis, the fit ranges are 2 ranges.
        if (paragraph_->getParagraphStyle().getEllipsisMod() != skt::EllipsisModal::TAIL) {
            fitRanges.push_back({ellipsisRange.end, lastLineTextRange.end});
        }
    }
}
} // namespace SPText
} // namespace Rosen
} // namespace OHOS
