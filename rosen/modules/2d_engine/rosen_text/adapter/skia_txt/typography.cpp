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

#include "skia_adapter/skia_canvas.h"
#include "impl/paragraph_impl.h"

#include "convert.h"

namespace OHOS {
namespace Rosen {
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

void Typography::Layout(double width)
{
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

void Typography::Paint(SkCanvas *canvas, double x, double y)
{
    return paragraph_->Paint(canvas, x, y);
}

void Typography::Paint(Drawing::Canvas *drawCanvas, double x, double y)
{
#ifndef USE_ROSEN_DRAWING
    std::shared_ptr<Drawing::CoreCanvasImpl> coreCanvas = drawCanvas->GetCanvasData();
    auto drawingCanvas = static_cast<Drawing::SkiaCanvas *>(coreCanvas.get());
    auto canvas = drawingCanvas->ExportSkCanvas();
    paragraph_->Paint(canvas, x, y);
#else
    paragraph_->Paint(drawCanvas, x, y);
#endif
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
} // namespace AdapterTxt
} // namespace Rosen
} // namespace OHOS
