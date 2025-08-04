/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "typography.h"

#include <mutex>
#include <numeric>

#include "convert.h"
#include "impl/paragraph_impl.h"
#include "modules/skparagraph/include/TextStyle.h"
#include "skia_adapter/skia_canvas.h"
#include "skia_adapter/skia_convert_utils.h"
#include "text_line_base.h"
#include "text_style.h"
#include "txt/paragraph_style.h"
#include "txt/text_style.h"
#include "typography_style.h"
#include "utils/text_log.h"
#include "utils/text_trace.h"

namespace OHOS {
namespace Rosen {
namespace skt = skia::textlayout;

TextRect::TextRect(Drawing::RectF rec, TextDirection dir)
{
    rect = rec;
    direction = dir;
}

IndexAndAffinity::IndexAndAffinity(size_t charIndex, Affinity charAffinity)
{
    index = charIndex;
    affinity = charAffinity;
}

namespace AdapterTxt {
Typography::Typography(std::unique_ptr<SPText::Paragraph> paragraph): paragraph_(std::move(paragraph))
{
}

double Typography::GetMaxWidth() const
{
    std::shared_lock<std::shared_mutex> readLock(mutex_);
    return paragraph_->GetMaxWidth();
}

double Typography::GetHeight() const
{
    std::shared_lock<std::shared_mutex> readLock(mutex_);
    return paragraph_->GetHeight();
}

double Typography::GetActualWidth() const
{
    std::shared_lock<std::shared_mutex> readLock(mutex_);
    return paragraph_->GetLongestLine();
}

double Typography::GetLongestLineWithIndent() const
{
    std::shared_lock<std::shared_mutex> readLock(mutex_);
    return paragraph_->GetLongestLineWithIndent();
}

double Typography::GetMinIntrinsicWidth()
{
    std::shared_lock<std::shared_mutex> readLock(mutex_);
    return paragraph_->GetMinIntrinsicWidth();
}

double Typography::GetMaxIntrinsicWidth()
{
    std::shared_lock<std::shared_mutex> readLock(mutex_);
    return paragraph_->GetMaxIntrinsicWidth();
}

double Typography::GetAlphabeticBaseline()
{
    std::shared_lock<std::shared_mutex> readLock(mutex_);
    return paragraph_->GetAlphabeticBaseline();
}

double Typography::GetIdeographicBaseline()
{
    std::shared_lock<std::shared_mutex> readLock(mutex_);
    return paragraph_->GetIdeographicBaseline();
}

bool Typography::DidExceedMaxLines() const
{
    std::shared_lock<std::shared_mutex> readLock(mutex_);
    return paragraph_->DidExceedMaxLines();
}

int Typography::GetLineCount() const
{
    std::shared_lock<std::shared_mutex> readLock(mutex_);
    return paragraph_->GetLineCount();
}

void Typography::SetIndents(const std::vector<float>& indents)
{
    std::unique_lock<std::shared_mutex> writeLock(mutex_);
    paragraph_->SetIndents(indents);
}

float Typography::DetectIndents(size_t index)
{
    std::shared_lock<std::shared_mutex> readLock(mutex_);
    return paragraph_->DetectIndents(index);
}

void Typography::Relayout(double width, const TypographyStyle &typograhyStyle, const std::vector<TextStyle> &textStyles)
{
    std::unique_lock<std::shared_mutex> writeLock(mutex_);

    if (!paragraph_->IsLayoutDone()) {
        TEXT_LOGI_LIMIT3_MIN("Need to layout first");
        return;
    }

    bool isTextStyleChange = std::any_of(textStyles.begin(), textStyles.end(), [](const TextStyle& style) {
        return style.relayoutChangeBitmap.any() || style.symbol.GetSymbolBitmap().any();
    });
    if (!typograhyStyle.relayoutChangeBitmap.any() && !isTextStyleChange) {
        if (skt::nearlyEqual(paragraph_->GetMaxWidth(), width)) {
            TEXT_LOGI_LIMIT3_MIN("No relayout required");
            return;
        }
        paragraph_->SetLayoutState(skt::InternalState::kShaped);

        paragraph_->Layout(width);
    } else {
        SPText::ParagraphStyle paragraphStyle = Convert(typograhyStyle);
        std::vector<SPText::TextStyle> spTextStyles;
        for (const TextStyle& style : textStyles) {
            spTextStyles.push_back(Convert(style));
        }

        paragraph_->Relayout(width, paragraphStyle, spTextStyles);
    }
    lineMetrics_.reset();
    lineMetricsStyles_.clear();
}

void Typography::Layout(double width)
{
    TEXT_TRACE_FUNC();
    std::unique_lock<std::shared_mutex> writeLock(mutex_);
    lineMetrics_.reset();
    lineMetricsStyles_.clear();
    return paragraph_->Layout(width);
}

double Typography::GetGlyphsBoundsTop()
{
    std::shared_lock<std::shared_mutex> readLock(mutex_);
    return paragraph_->GetGlyphsBoundsTop();
}

double Typography::GetGlyphsBoundsBottom()
{
    std::shared_lock<std::shared_mutex> readLock(mutex_);
    return paragraph_->GetGlyphsBoundsBottom();
}

double Typography::GetGlyphsBoundsLeft()
{
    std::shared_lock<std::shared_mutex> readLock(mutex_);
    return paragraph_->GetGlyphsBoundsLeft();
}

double Typography::GetGlyphsBoundsRight()
{
    std::shared_lock<std::shared_mutex> readLock(mutex_);
    return paragraph_->GetGlyphsBoundsRight();
}

Drawing::FontMetrics Typography::MeasureText()
{
    std::unique_lock<std::shared_mutex> writeLock(mutex_);
    return paragraph_->MeasureText();
}

void Typography::MarkDirty()
{
    std::unique_lock<std::shared_mutex> writeLock(mutex_);
    if (paragraph_ == nullptr) {
        return;
    }
    paragraph_->MarkDirty();
}

int32_t Typography::GetUnresolvedGlyphsCount()
{
    std::shared_lock<std::shared_mutex> readLock(mutex_);
    if (paragraph_ == nullptr) {
        return 0;
    }
    return paragraph_->GetUnresolvedGlyphsCount();
}

void Typography::UpdateFontSize(size_t from, size_t to, float fontSize)
{
    std::unique_lock<std::shared_mutex> writeLock(mutex_);
    if (paragraph_ == nullptr) {
        return;
    }
    paragraph_->UpdateFontSize(from, to, fontSize);
}

void Typography::Paint(SkCanvas *canvas, double x, double y)
{
    std::unique_lock<std::shared_mutex> writeLock(mutex_);
    return paragraph_->Paint(canvas, x, y);
}

void Typography::Paint(Drawing::Canvas *drawCanvas, double x, double y)
{
    std::unique_lock<std::shared_mutex> writeLock(mutex_);
    paragraph_->Paint(drawCanvas, x, y);
}

void Typography::Paint(Drawing::Canvas* drawCanvas, Drawing::Path* path, double hOffset, double vOffset)
{
    std::unique_lock<std::shared_mutex> writeLock(mutex_);
    paragraph_->Paint(drawCanvas, path, hOffset, vOffset);
}

std::vector<TextRect> Typography::GetTextRectsByBoundary(size_t left, size_t right,
    TextRectHeightStyle heightStyle, TextRectWidthStyle widthStyle)
{
    std::shared_lock<std::shared_mutex> readLock(mutex_);
    auto txtRectHeightStyle = Convert(heightStyle);
    auto txtRectWidthStyle = Convert(widthStyle);
    auto rects = paragraph_->GetRectsForRange(left, right, txtRectHeightStyle, txtRectWidthStyle);

    std::vector<TextRect> boxes;
    for (const auto &rect : rects) {
        boxes.push_back(Convert(rect));
    }
    return boxes;
}

std::vector<TextRect> Typography::GetTextRectsOfPlaceholders()
{
    std::shared_lock<std::shared_mutex> readLock(mutex_);
    auto rects = paragraph_->GetRectsForPlaceholders();

    std::vector<TextRect> boxes;
    for (const auto &rect : rects) {
        boxes.push_back(Convert(rect));
    }
    return boxes;
}

IndexAndAffinity Typography::GetGlyphIndexByCoordinate(double x, double y)
{
    std::shared_lock<std::shared_mutex> readLock(mutex_);
    auto pos = paragraph_->GetGlyphPositionAtCoordinate(x, y);
    return Convert(pos);
}

Boundary Typography::GetWordBoundaryByIndex(size_t index)
{
    std::shared_lock<std::shared_mutex> readLock(mutex_);
    auto range = paragraph_->GetWordBoundary(index);
    return Convert(range);
}

Boundary Typography::GetActualTextRange(int lineNumber, bool includeSpaces)
{
    std::shared_lock<std::shared_mutex> readLock(mutex_);
    auto range = paragraph_->GetActualTextRange(lineNumber, includeSpaces);
    return Convert(range);
}

Boundary Typography::GetEllipsisTextRange()
{
    std::shared_lock<std::shared_mutex> readLock(mutex_);
    auto range = paragraph_->GetEllipsisTextRange();
    return Convert(range);
}

double Typography::GetLineHeight(int lineNumber)
{
    std::shared_lock<std::shared_mutex> readLock(mutex_);
    const auto &lines = paragraph_->GetLineMetrics();
    if ((0 <= lineNumber) && (lineNumber < static_cast<int>(lines.size()))) {
        return lines[lineNumber].fHeight;
    }
    return 0.0;
}

double Typography::GetLineWidth(int lineNumber)
{
    std::shared_lock<std::shared_mutex> readLock(mutex_);
    const auto &lines = paragraph_->GetLineMetrics();
    if ((0 <= lineNumber) && (lineNumber < static_cast<int>(lines.size()))) {
        return lines[lineNumber].fWidth;
    }
    return 0.0;
}

void Typography::SetAnimation(
    std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)>& animationFunc
)
{
    std::unique_lock<std::shared_mutex> writeLock(mutex_);
    if (animationFunc != nullptr && paragraph_ != nullptr) {
        paragraph_->SetAnimation(animationFunc);
    }
}

std::function<bool(const std::shared_ptr<TextEngine::SymbolAnimationConfig>&)> Typography::GetAnimation()
{
    std::shared_lock<std::shared_mutex> readLock(mutex_);
    if (paragraph_ == nullptr) {
        return nullptr;
    }
    return paragraph_->GetAnimation();
}

void Typography::SetParagraghId(uint32_t id)
{
    std::unique_lock<std::shared_mutex> writeLock(mutex_);
    if (paragraph_ != nullptr) {
        paragraph_->SetParagraghId(id);
    }
}

bool Typography::GetLineInfo(int lineNumber, bool oneLine, bool includeWhitespace, LineMetrics* lineMetrics)
{
    std::shared_lock<std::shared_mutex> readLock(mutex_);
    if (paragraph_ == nullptr) {
        return false;
    }
    if (lineNumber < 0 || lineNumber >= static_cast<int>(paragraph_->GetLineCount()) || lineMetrics == nullptr) {
        return false;
    }

    skia::textlayout::LineMetrics sklineMetrics;
    if (!paragraph_->GetLineMetricsAt(lineNumber, &sklineMetrics)) {
        return false;
    }

    if (!sklineMetrics.fLineMetrics.empty()) {
        const auto &skFontMetrics = sklineMetrics.fLineMetrics.begin()->second.font_metrics;
        lineMetrics->firstCharMetrics = skFontMetrics;
        if (oneLine) {
            lineMetrics->ascender = sklineMetrics.fAscent;
            lineMetrics->descender = sklineMetrics.fDescent;
        } else {
            lineMetrics->ascender = skFontMetrics.fAscent;
            lineMetrics->descender = skFontMetrics.fDescent;
        }
        lineMetrics->capHeight = skFontMetrics.fCapHeight;
        lineMetrics->xHeight = skFontMetrics.fXHeight;
    } else {
        if (oneLine) {
            lineMetrics->ascender = sklineMetrics.fAscent;
            lineMetrics->descender = sklineMetrics.fDescent;
        } else {
            lineMetrics->ascender = 0.0;
            lineMetrics->descender = 0.0;
        }
        lineMetrics->capHeight = 0.0;
        lineMetrics->xHeight = 0.0;
    }
    if (includeWhitespace) {
        lineMetrics->width = sklineMetrics.fWidthWithSpaces;
    } else {
        lineMetrics->width = sklineMetrics.fWidth;
    }
    lineMetrics->height = sklineMetrics.fHeight;
    lineMetrics->x = sklineMetrics.fLeft;
    lineMetrics->y = sklineMetrics.fTopHeight;
    lineMetrics->startIndex = sklineMetrics.fStartIndex;
    lineMetrics->endIndex = sklineMetrics.fEndIndex;

    return true;
}

std::vector<LineMetrics> Typography::GetLineMetrics()
{
    std::unique_lock<std::shared_mutex> writeLock(mutex_);
    if (lineMetrics_) {
        return lineMetrics_.value();
    }
    lineMetrics_.emplace();
    if (paragraph_ != nullptr) {
        auto metrics = paragraph_->GetLineMetrics();
        lineMetricsStyles_.reserve(std::accumulate(metrics.begin(), metrics.end(), 0,
            [](const int a, const skia::textlayout::LineMetrics& b) { return a + b.fLineMetrics.size(); }));

        for (const skt::LineMetrics& skLineMetrics : metrics) {
            LineMetrics& line = lineMetrics_->emplace_back();
            if (!skLineMetrics.fLineMetrics.empty()) {
                const auto &skmFontMetrics = skLineMetrics.fLineMetrics.begin()->second.font_metrics;
                line.firstCharMetrics = skmFontMetrics;
                line.capHeight = skmFontMetrics.fCapHeight;
                line.xHeight = skmFontMetrics.fXHeight;
            } else {
                line.capHeight = 0.0;
                line.xHeight = 0.0;
            }
            line.lineNumber = skLineMetrics.fLineNumber;
            line.baseline = skLineMetrics.fBaseline;
            line.ascender = skLineMetrics.fAscent;
            line.descender = skLineMetrics.fDescent;
            line.width = skLineMetrics.fWidth;
            line.height = skLineMetrics.fHeight;
            line.x = skLineMetrics.fLeft;
            line.y = skLineMetrics.fTopHeight;
            line.startIndex = skLineMetrics.fStartIndex;
            line.endIndex = skLineMetrics.fEndIndex;
            for (const auto& [index, styleMtrics] : skLineMetrics.fLineMetrics) {
                SPText::TextStyle spTextStyle = paragraph_->SkStyleToTextStyle(*styleMtrics.text_style);
                lineMetricsStyles_.emplace_back(Convert(spTextStyle));

                line.runMetrics.emplace(std::piecewise_construct, std::forward_as_tuple(index),
                    std::forward_as_tuple(&lineMetricsStyles_.back(), styleMtrics.font_metrics));
            }
        }
    }
    return lineMetrics_.value();
}

bool Typography::GetLineMetricsAt(int lineNumber, LineMetrics* lineMetrics)
{
    if (paragraph_ == nullptr) {
        return false;
    }
    if (lineNumber < 0 || lineNumber >= static_cast<int>(paragraph_->GetLineCount()) || lineMetrics == nullptr) {
        return false;
    }
    std::vector<LineMetrics> vecLineMetrics = GetLineMetrics();

    if (vecLineMetrics.empty()) {
        return false;
    }

    *lineMetrics = vecLineMetrics[lineNumber];

    return true;
}

Drawing::FontMetrics Typography::GetFontMetrics(const OHOS::Rosen::TextStyle& textStyle)
{
    std::shared_lock<std::shared_mutex> readLock(mutex_);
    auto spTextStyle = Convert(textStyle);
    return paragraph_->GetFontMetricsResult(spTextStyle);
}

bool Typography::GetLineFontMetrics(const size_t lineNumber,
    size_t& charNumber, std::vector<Drawing::FontMetrics>& fontMetrics)
{
    std::shared_lock<std::shared_mutex> readLock(mutex_);
    if (!paragraph_) {
        return false;
    }
    return paragraph_->GetLineFontMetrics(lineNumber, charNumber, fontMetrics);
}

std::vector<std::unique_ptr<TextLineBase>> Typography::GetTextLines() const
{
    std::shared_lock<std::shared_mutex> readLock(mutex_);
    if (!paragraph_) {
        return {};
    }
    std::vector<std::unique_ptr<SPText::TextLineBase>> textLines = paragraph_->GetTextLines();
    std::vector<std::unique_ptr<TextLineBase>> lines;

    for (std::unique_ptr<SPText::TextLineBase>& textLine : textLines) {
        std::unique_ptr<TextLineBaseImpl> linePtr = std::make_unique<TextLineBaseImpl>(std::move(textLine));
        lines.emplace_back(std::move(linePtr));
    }
    return lines;
}

std::unique_ptr<OHOS::Rosen::Typography> Typography::CloneSelf()
{
    std::shared_lock<std::shared_mutex> readLock(mutex_);
    if (!paragraph_) {
        return nullptr;
    }
    return std::make_unique<Typography>(paragraph_->CloneSelf());
}

void Typography::UpdateColor(size_t from, size_t to, const Drawing::Color& color)
{
    std::unique_lock<std::shared_mutex> writeLock(mutex_);
    if (!paragraph_) {
        return;
    }
    paragraph_->UpdateColor(from, to, color);
}

void Typography::UpdateAllTextStyles(const TextStyle& textStyleTemplate)
{
    std::unique_lock<std::shared_mutex> writeLock(mutex_);
    if (!paragraph_) {
        return;
    }
    std::vector<SPText::TextStyle> spTextStyles;
    spTextStyles.push_back(Convert(textStyleTemplate));
    paragraph_->ApplyTextStyleChanges(spTextStyles);
}

Drawing::RectI Typography::GeneratePaintRegion(double x, double y) const
{
    std::unique_lock<std::shared_mutex> writeLock(mutex_);
    if (!paragraph_) {
        double left = std::floor(x);
        double top = std::floor(y);
        return Drawing::RectI(left, top, left, top);
    }

    return paragraph_->GeneratePaintRegion(x, y);
}

std::vector<TextBlobRecordInfo> Typography::GetTextBlobRecordInfo() const
{
    std::shared_lock<std::shared_mutex> readLock(mutex_);
    if (paragraph_ == nullptr) {
        return {};
    }
    return paragraph_->GetTextBlobRecordInfo();
}

bool Typography::HasSkipTextBlobDrawing() const
{
    std::shared_lock<std::shared_mutex> readLock(mutex_);
    if (paragraph_ == nullptr) {
        return false;
    }
    return paragraph_->HasSkipTextBlobDrawing();
}

void Typography::SetSkipTextBlobDrawing(bool state)
{
    std::unique_lock<std::shared_mutex> writeLock(mutex_);
    if (paragraph_ == nullptr) {
        return;
    }
    paragraph_->SetSkipTextBlobDrawing(state);
}

bool Typography::CanPaintAllText() const
{
    std::shared_lock<std::shared_mutex> readLock(mutex_);
    if (paragraph_ == nullptr) {
        TEXT_LOGI_LIMIT3_MIN("Failed to get paragraph");
        return false;
    }
    return paragraph_->CanPaintAllText();
}

} // namespace AdapterTxt
} // namespace Rosen
} // namespace OHOS
