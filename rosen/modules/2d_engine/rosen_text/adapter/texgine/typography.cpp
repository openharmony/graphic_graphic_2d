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

#include "engine_adapter/skia_adapter/skia_canvas.h"
#include "texgine_canvas.h"

#include "convert.h"

namespace OHOS {
namespace Rosen {
TextRect::TextRect(Drawing::RectF rect, TextDirection direction)
{
    rect_ = rect;
    direction_ = direction;
}

IndexAndAffinity::IndexAndAffinity(size_t index, Affinity affinity)
{
    index_ = index;
    affinity_ = affinity;
}

Boundary::Boundary(size_t left, size_t right)
{
    leftIndex_ = left;
    rightIndex_ = right;
}

bool Boundary::operator ==(const Boundary& rhs) const
{
    return leftIndex_ == rhs.leftIndex_ && rightIndex_ == rhs.rightIndex_;
}

namespace AdapterTextEngine {
Typography::Typography(std::shared_ptr<Texgine::Typography> typography)
{
    typography_ = std::move(typography);
}

double Typography::GetMaxWidth() const
{
    return typography_->GetMaxWidth();
}

double Typography::GetHeight() const
{
    return typography_->GetHeight();
}

double Typography::GetActualWidth() const
{
    return typography_->GetActualWidth();
}

double Typography::GetMinIntrinsicWidth()
{
    return typography_->GetMinIntrinsicWidth();
}

double Typography::GetMaxIntrinsicWidth()
{
    return typography_->GetMaxIntrinsicWidth();
}

double Typography::GetAlphabeticBaseline()
{
    return typography_->GetAlphabeticBaseline();
}

double Typography::GetIdeographicBaseline()
{
    return typography_->GetIdeographicBaseline();
}

bool Typography::DidExceedMaxLines() const
{
    return typography_->DidExceedMaxLines();
}

int Typography::GetLineCount() const
{
    return typography_->GetLineCount();
}

void Typography::SetIndents(const std::vector<float>& indents)
{
    typography_->SetIndents(indents);
}

void Typography::Layout(double width)
{
    return typography_->Layout(width);
}

void Typography::Paint(SkCanvas *canvas, double x, double y)
{
    auto texgineCanvas = std::make_shared<TextEngine::TexgineCanvas>();
    texgineCanvas->SetCanvas(canvas);
    return typography_->Paint(*texgineCanvas, x, y);
}

void Typography::Paint(Drawing::Canvas *drawCanvas, double x, double y)
{
    std::shared_ptr<Drawing::CoreCanvasImpl> coreCanvas = drawCanvas->GetCanvasData();
    auto drawingCanvas = static_cast<Drawing::SkiaCanvas *>(coreCanvas.get());
    auto canvas = drawingCanvas->ExportSkCanvas();
    Paint(canvas, x, y);
}

std::vector<TextRect> Typography::GetTextRectsByBoundary(size_t left, size_t right,
                                                        TextRectHeightStyle heightStyle,
                                                        TextRectWidthStyle widthStyle)
{
    auto txtRectHeightStyle = Convert(heightStyle);
    auto txtRectWidthStyle = Convert(widthStyle);
    TextEngine::Boundary boundary(left, right);
    auto rects = typography_->GetTextRectsByBoundary(boundary, txtRectHeightStyle, txtRectWidthStyle);

    std::vector<TextRect> boxes;
    for (const auto &rect : rects) {
        boxes.push_back(Convert(rect));
    }
    return boxes;
}

std::vector<TextRect> Typography::GetTextRectsOfPlaceholders()
{
    auto rects = typography_->GetTextRectsOfPlaceholders();

    std::vector<TextRect> boxes;
    for (const auto &rect : rects) {
        boxes.push_back(Convert(rect));
    }
    return boxes;
}

IndexAndAffinity Typography::GetGlyphIndexByCoordinate(double x, double y)
{
    auto pos = typography_->GetGlyphIndexByCoordinate(x, y);
    return Convert(pos);
}

Boundary Typography::GetWordBoundaryByIndex(size_t index)
{
    auto range = typography_->GetWordBoundaryByIndex(index);
    return Convert(range);
}
} // namespace AdapterTextEngine
} // namespace Rosen
} // namespace OHOS
