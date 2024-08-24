/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.. All rights reserved.
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

#include "rosen_text/ui/typography.h"
#include "rosen_text/properties/typography_create_base.h"
#include "rosen_text/properties/typography_txt.h"

namespace rosen {
Typography::Typography()
{
    typography_ = std::make_unique<TypographyTxt>();
}

void Typography::Init(std::shared_ptr<TypographyCreateBase> TypographyCreateBase_)
{
    typography_->Init(TypographyCreateBase_);
}

double Typography::GetMaxWidth()
{
    return typography_->GetMaxWidth();
}

double Typography::GetHeight()
{
    return typography_->GetHeight();
}

double Typography::GetLongestLine()
{
    return typography_->GetLongestLine();
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

bool Typography::DidExceedMaxLines()
{
    return typography_->DidExceedMaxLines();
}

void Typography::Layout(double width)
{
    typography_->Layout(width);
}

void Typography::Paint(Canvas* canvas, double x, double y)
{
    typography_->Paint(canvas, x, y);
}

std::vector<TypographyProperties::TextBox> Typography::GetRectsForRange(
    size_t start,
    size_t end,
    TypographyProperties::RectHeightStyle heightStyle,
    TypographyProperties::RectWidthStyle widthStyle)
{
    return typography_->GetRectsForRange(start, end, heightStyle, widthStyle);
}

std::vector<TypographyProperties::TextBox> Typography::GetRectsForPlaceholders()
{
    return  typography_->GetRectsForPlaceholders();
}

TypographyProperties::PositionAndAffinity Typography::GetGlyphPositionAtCoordinate(double dx, double dy)
{
    return typography_->GetGlyphPositionAtCoordinate(dx, dy);
}

TypographyProperties::PositionAndAffinity Typography::GetGlyphPositionAtCoordinateWithCluster(double dx, double dy)
{
    return typography_->GetGlyphPositionAtCoordinateWithCluster(dx, dy);
}

TypographyProperties::Range<size_t> Typography::GetWordBoundary(size_t offset)
{
    return typography_->GetWordBoundary(offset);
}

TypographyProperties::Range<size_t> Typography::GetActualTextRange(int lineNumber, bool includeSpaces)
{
    return typography_->GetActualTextRange(lineNumber, includeSpaces);
}

size_t Typography::GetLineCount()
{
    return typography_->GetLineCount();
}

double Typography::GetLineHeight(int lineNumber)
{
    return typography_->GetLineHeight();
}

double Typography::GetLineWidth(int lineNumber)
{
    return typography_->GetLineWidth();
}
} // namespace rosen
