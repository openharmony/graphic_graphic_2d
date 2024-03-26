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

#include "skia_adapter/skia_canvas.h"
#include "skia_adapter/skia_convert_utils.h"
#include "impl/paragraph_impl.h"

#include "convert.h"

namespace OHOS {
namespace Rosen {
namespace {
std::mutex g_layoutMutex;
}

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

Boundary::Boundary(size_t left, size_t right)
{
    leftIndex = left;
    rightIndex = right;
}

bool Boundary::operator ==(const Boundary& rhs) const
{
    return leftIndex == rhs.leftIndex && rightIndex == rhs.rightIndex;
}

namespace AdapterTxt {
Typography::Typography(std::unique_ptr<SPText::Paragraph> paragraph): paragraph_(std::move(paragraph))
{
}

double Typography::GetMaxWidth() const
{
    return paragraph_->GetMaxWidth();
}

double Typography::GetHeight() const
{
    return paragraph_->GetHeight();
}

double Typography::GetActualWidth() const
{
    return paragraph_->GetLongestLine();
}

double Typography::GetMinIntrinsicWidth()
{
    return paragraph_->GetMinIntrinsicWidth();
}

double Typography::GetMaxIntrinsicWidth()
{
    return paragraph_->GetMaxIntrinsicWidth();
}

double Typography::GetAlphabeticBaseline()
{
    return paragraph_->GetAlphabeticBaseline();
}

double Typography::GetIdeographicBaseline()
{
    return paragraph_->GetIdeographicBaseline();
}

bool Typography::DidExceedMaxLines() const
{
    return paragraph_->DidExceedMaxLines();
}

int Typography::GetLineCount() const
{
    return paragraph_->GetLineCount();
}

void Typography::SetIndents(const std::vector<float>& indents)
{
    paragraph_->SetIndents(indents);
}

float Typography::DetectIndents(size_t index)
{
    return paragraph_->DetectIndents(index);
}

void Typography::Layout(double width)
{
    std::unique_lock lock(g_layoutMutex);
    return paragraph_->Layout(width);
}

double Typography::GetGlyphsBoundsTop()
{
    return paragraph_->GetGlyphsBoundsTop();
}

double Typography::GetGlyphsBoundsBottom()
{
    return paragraph_->GetGlyphsBoundsBottom();
}

double Typography::GetGlyphsBoundsLeft()
{
    return paragraph_->GetGlyphsBoundsLeft();
}

double Typography::GetGlyphsBoundsRight()
{
    return paragraph_->GetGlyphsBoundsRight();
}

Drawing::FontMetrics Typography::MeasureText()
{
    return paragraph_->MeasureText();
}

void Typography::MarkDirty()
{
    if (paragraph_ == nullptr) {
        return;
    }
    paragraph_->MarkDirty();
}

int32_t Typography::GetUnresolvedGlyphsCount()
{
    if (paragraph_ == nullptr) {
        return 0;
    }
    return paragraph_->GetUnresolvedGlyphsCount();
}

void Typography::UpdateFontSize(size_t from, size_t to, float fontSize)
{
    if (paragraph_ == nullptr) {
        return;
    }
    paragraph_->UpdateFontSize(from, to, fontSize);
}

void Typography::Paint(SkCanvas *canvas, double x, double y)
{
    return paragraph_->Paint(canvas, x, y);
}

void Typography::Paint(Drawing::Canvas *drawCanvas, double x, double y)
{
    paragraph_->Paint(drawCanvas, x, y);
}

std::vector<TextRect> Typography::GetTextRectsByBoundary(size_t left, size_t right,
    TextRectHeightStyle heightStyle, TextRectWidthStyle widthStyle)
{
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
    auto rects = paragraph_->GetRectsForPlaceholders();

    std::vector<TextRect> boxes;
    for (const auto &rect : rects) {
        boxes.push_back(Convert(rect));
    }
    return boxes;
}

IndexAndAffinity Typography::GetGlyphIndexByCoordinate(double x, double y)
{
    auto pos = paragraph_->GetGlyphPositionAtCoordinate(x, y);
    return Convert(pos);
}

Boundary Typography::GetWordBoundaryByIndex(size_t index)
{
    auto range = paragraph_->GetWordBoundary(index);
    return Convert(range);
}

Boundary Typography::GetActualTextRange(int lineNumber, bool includeSpaces)
{
    auto range = paragraph_->GetActualTextRange(lineNumber, includeSpaces);
    return Convert(range);
}

double Typography::GetLineHeight(int lineNumber)
{
    const auto &lines = paragraph_->GetLineMetrics();
    if (lineNumber < static_cast<int>(lines.size())) {
        return lines[lineNumber].height;
    }
    return 0.0;
}

double Typography::GetLineWidth(int lineNumber)
{
    const auto &lines = paragraph_->GetLineMetrics();
    if (lineNumber < static_cast<int>(lines.size())) {
        return lines[lineNumber].width;
    }
    return 0.0;
}

void Typography::SetAnimation(
    std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)>& animationFunc
)
{
    if (animationFunc != nullptr && paragraph_ != nullptr) {
        paragraph_->SetAnimation(animationFunc);
    }
}

void Typography::SetParagraghId(uint32_t id)
{
    if (paragraph_ != nullptr) {
        paragraph_->SetParagraghId(id);
    }
}

bool Typography::GetLineInfo(int lineNumber, bool oneLine, bool includeWhitespace, LineMetrics* lineMetrics)
{
    if (paragraph_ == nullptr) {
        return false;
    }
    if (lineNumber < 0 || lineNumber >= paragraph_->GetLineCount() || lineMetrics == nullptr) {
        return false;
    }

    skia::textlayout::LineMetrics sklineMetrics;
    if (!paragraph_->GetLineMetricsAt(lineNumber, &sklineMetrics)) {
        return false;
    }

    if (!sklineMetrics.fLineMetrics.empty()) {
        const auto &skFontMetrics = sklineMetrics.fLineMetrics.at(sklineMetrics.fStartIndex).font_metrics;
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
    std::vector<LineMetrics> lineMetrics;
    if (paragraph_ != nullptr) {
        auto metrics = paragraph_->GetLineMetrics();
        for (SPText::LineMetrics& spLineMetrics : metrics) {
            LineMetrics& line = lineMetrics.emplace_back();
            if (!spLineMetrics.runMetrics.empty()) {
                auto &spFontMetrics = spLineMetrics.runMetrics.at(spLineMetrics.startIndex).fontMetrics;
                line.firstCharMetrics = spFontMetrics;
                line.capHeight = spFontMetrics.fCapHeight;
                line.xHeight = spFontMetrics.fXHeight;
            } else {
                line.capHeight = 0.0;
                line.xHeight = 0.0;
            }
            line.ascender = spLineMetrics.ascent;
            line.descender = spLineMetrics.descent;
            line.width = spLineMetrics.width;
            line.height = spLineMetrics.height;
            line.x = spLineMetrics.left;
            line.y = spLineMetrics.topHeight;
            line.startIndex = spLineMetrics.startIndex;
            line.endIndex = spLineMetrics.endIndex;
        }
    }
    return lineMetrics;
}

bool Typography::GetLineMetricsAt(int lineNumber, LineMetrics* lineMetrics)
{
    if (paragraph_ == nullptr) {
        return false;
    }
    if (lineNumber < 0 || lineNumber >= paragraph_->GetLineCount() || lineMetrics == nullptr) {
        return false;
    }
    skia::textlayout::LineMetrics skLineMetrics;
    if (!paragraph_->GetLineMetricsAt(lineNumber, &skLineMetrics)) {
        return false;
    }

    if (!skLineMetrics.fLineMetrics.empty()) {
        const auto &skFontMetrics = skLineMetrics.fLineMetrics.at(skLineMetrics.fStartIndex).font_metrics;
        lineMetrics->firstCharMetrics = skFontMetrics;
        lineMetrics->capHeight = skFontMetrics.fCapHeight;
        lineMetrics->xHeight = skFontMetrics.fXHeight;
    } else {
        lineMetrics->capHeight = 0.0;
        lineMetrics->xHeight = 0.0;
    }

    lineMetrics->ascender = skLineMetrics.fAscent;
    lineMetrics->descender = skLineMetrics.fDescent;

    lineMetrics->width = skLineMetrics.fWidth;
    lineMetrics->height = skLineMetrics.fHeight;
    lineMetrics->x = skLineMetrics.fLeft;
    lineMetrics->y = skLineMetrics.fTopHeight;
    lineMetrics->startIndex = skLineMetrics.fStartIndex;
    lineMetrics->endIndex = skLineMetrics.fEndIndex;

    return true;
}

Drawing::FontMetrics Typography::GetFontMetrics(const OHOS::Rosen::TextStyle& textStyle)
{
    auto spTextStyle = Convert(textStyle);
    return paragraph_->GetFontMetricsResult(spTextStyle);
}

bool Typography::GetLineFontMetrics(const size_t lineNumber,
    size_t& charNumber, std::vector<Drawing::FontMetrics>& fontMetrics)
{
    if (!paragraph_) {
        return false;
    }
    return paragraph_->GetLineFontMetrics(lineNumber, charNumber, fontMetrics);
}
} // namespace AdapterTxt
} // namespace Rosen
} // namespace OHOS
