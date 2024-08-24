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

#include "typography.h"

#include "skia_adapter/skia_canvas.h"
#include "txt/paragraph_txt.h"

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
Typography::Typography(std::unique_ptr<txt::Paragraph> paragraph): paragraph_(std::move(paragraph))
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
    auto paragraphTxt = reinterpret_cast<txt::ParagraphTxt *>(paragraph_.get());
    if (paragraphTxt) {
        return paragraphTxt->GetLineCount();
    }
    return 0;
}

void Typography::SetIndents(const std::vector<float>& indents)
{
    auto paragraphTxt = reinterpret_cast<txt::ParagraphTxt *>(paragraph_.get());
    if (paragraphTxt) {
        paragraphTxt->SetIndents(indents);
    }
}

void Typography::Layout(double width)
{
    return paragraph_->Layout(width);
}

void Typography::Paint(SkCanvas *canvas, double x, double y)
{
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
    return paragraph_->GetLineHeight(lineNumber);
}

double Typography::GetLineWidth(int lineNumber)
{
    return paragraph_->GetLineWidth(lineNumber);
}
} // namespace AdapterTxt
} // namespace Rosen
} // namespace OHOS
