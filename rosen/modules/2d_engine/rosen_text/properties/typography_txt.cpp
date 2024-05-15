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

#include "engine_adapter/skia_adapter/skia_canvas.h"
#include "rosen_text/properties/rosen_converter_txt.h"
#include "rosen_text/properties/typography_create_base.h"
#include "rosen_text/properties/typography_create_txt.h"
#include "rosen_text/properties/typography_txt.h"
#include "txt/paragraph_txt.h"

namespace rosen {
TypographyTxt::TypographyTxt()
{
    paragraphTxt_ = std::make_unique<txt::ParagraphTxt>();
}

TypographyTxt::~TypographyTxt()
{
}

void TypographyTxt::Init(std::shared_ptr<TypographyCreateBase> typographyCreateBase)
{
    const TypographyCreateTxt* typographyCreateTxt = static_cast<const TypographyCreateTxt*>(
        typographyCreateBase.get());
    paragraphTxt_ = typographyCreateTxt->GetParagraphBuilderTxt()->Build();
}


double TypographyTxt::GetMaxWidth()
{
    return paragraphTxt_->GetMaxWidth();
}

double TypographyTxt::GetHeight()
{
    return paragraphTxt_->GetHeight();
}

double TypographyTxt::GetLongestLine()
{
    return paragraphTxt_->GetLongestLine();
}

double TypographyTxt::GetMinIntrinsicWidth()
{
    return paragraphTxt_->GetMinIntrinsicWidth();
}

double TypographyTxt::GetMaxIntrinsicWidth()
{
    return paragraphTxt_->GetMaxIntrinsicWidth();
}

double TypographyTxt::GetAlphabeticBaseline()
{
    return paragraphTxt_->GetAlphabeticBaseline();
}

double TypographyTxt::GetIdeographicBaseline()
{
    return paragraphTxt_->GetIdeographicBaseline();
}

bool TypographyTxt::DidExceedMaxLines()
{
    return paragraphTxt_->DidExceedMaxLines();
}

void TypographyTxt::Layout(double width)
{
    paragraphTxt_->Layout(width);
}

void TypographyTxt::Paint(Canvas* drawCanvas, double x, double y)
{
    paragraphTxt_->Paint(drawCanvas, x, y);
}

void TypographyTxt::Paint(Canvas* drawCanvas, Path* path, double hOffset, double vOffset)
{
    paragraphTxt_->Paint(drawCanvas, path, hOffset, vOffset));
}

std::vector<TypographyProperties::TextBox> TypographyTxt::GetRectsForRange(
    size_t start,
    size_t end,
    TypographyProperties::RectHeightStyle heightStyle,
    TypographyProperties::RectWidthStyle widthStyle)
{
    txt::Paragraph::RectHeightStyle txtHeightStyle = RosenConvertTxtRectHeightStyle(heightStyle);
    txt::Paragraph::RectWidthStyle txtWidthStyle = RosenConvertTxtRectWidthStyle(widthStyle);
    std::vector<txt::Paragraph::TextBox> txtTextBox =
        paragraphTxt_->GetRectsForRange(start, end, txtHeightStyle, txtWidthStyle);
    std::vector<TypographyProperties::TextBox> rosenTextBox;
    for (auto& txtbox : txtTextBox) {
        rosenTextBox.push_back(TxtConvertRosenTextBox(txtbox));
    }
    return rosenTextBox;
}

std::vector<TypographyProperties::TextBox> TypographyTxt::GetRectsForPlaceholders()
{
    std::vector<txt::Paragraph::TextBox> txtTextBox = paragraphTxt_->GetRectsForPlaceholders();
    std::vector<TypographyProperties::TextBox> rosenTextBox;
    for (auto& txtbox : txtTextBox) {
        rosenTextBox.push_back(TxtConvertRosenTextBox(txtbox));
    }
    return rosenTextBox;
}

TypographyProperties::PositionAndAffinity TypographyTxt::GetGlyphPositionAtCoordinate(double dx, double dy)
{
    txt::Paragraph::PositionWithAffinity posAndAffinity = paragraphTxt_->GetGlyphPositionAtCoordinate(dx, dy);
    return TxtConvertPosAndAffinity(posAndAffinity);
}

TypographyProperties::PositionAndAffinity TypographyTxt::GetGlyphPositionAtCoordinateWithCluster(double dx, double dy)
{
    txt::Paragraph::PositionWithAffinity posAndAffinity =
#if defined(USE_CANVASKIT0310_SKIA) || defined(NEW_SKIA)
        // new flutter libtxt not have GetGlyphPositionAtCoordinateWithCluster
        paragraphTxt_->GetGlyphPositionAtCoordinate(dx, dy);
#else
        paragraphTxt_->GetGlyphPositionAtCoordinateWithCluster(dx, dy);
#endif
    return TxtConvertPosAndAffinity(posAndAffinity);
}

TypographyProperties::Range<size_t> TypographyTxt::GetWordBoundary(size_t offset)
{
    txt::Paragraph::Range<size_t> range = paragraphTxt_->GetWordBoundary(offset);
    return TxtConvertRange(range);
}

TypographyProperties::Range<size_t> TypographyTxt::GetActualTextRange(int lineNumber, bool includeSpaces)
{
    txt::Paragraph::Range<size_t> range = paragraphTxt_->GetActualTextRange(lineNumber, includeSpaces);
    return TxtConvertRange(range);
}

size_t TypographyTxt::GetLineCount()
{
    return paragraphTxt_->GetLineCount();
}

double TypographyTxt::GetLineHeight(int lineNumber)
{
    return paragraphTxt_->GetLineHeight();
}

double TypographyTxt::GetLineWidth(int lineNumber)
{
    return paragraphTxt_->GetLineWidth();
}
} // namespace rosen
