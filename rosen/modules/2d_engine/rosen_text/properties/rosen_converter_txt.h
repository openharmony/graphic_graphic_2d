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

#ifndef ROSEN_TEXT_PROPERTIES_ROSEN_CONVERT_TXT_H
#define ROSEN_TEXT_PROPERTIES_ROSEN_CONVERT_TXT_H

#include "minikin/LineBreaker.h"
#include "rosen_text/properties/text_style.h"
#include "rosen_text/properties/typography_style.h"
#include "rosen_text/properties/placeholder_run.h"
#include "rosen_text/properties/typography_properties.h"
#include "txt/font_weight.h"
#include "txt/text_baseline.h"
#include "txt/text_style.h"
#include "txt/paragraph.h"
#include "txt/paragraph_style.h"
#include "txt/placeholder_run.h"

namespace rosen {
txt::FontWeight RosenConvertTxtFontWeight(FontWeight& fontWeight);
txt::FontStyle RosenConvertTxtFontStyle(FontStyle& fontStyle);
txt::TextBaseline RosenConvertTxtTextBaseline(TextBaseline& textBaseline);
txt::TextAlign RosenConvertTxtTextAlign(TextAlign& textAlign);
minikin::BreakStrategy RosenConverMinkinBreakStrategy(BreakStrategy& breakStrategy);
minikin::WordBreakType RosenConverMinkinWordBreakType(WordBreakType& wordBreakType);
txt::TextDirection RosenConvertTxtTextDirection(TextDirection& textDirection);
txt::TextDecoration RosenConvertTxtTextDecoration(TextDecoration& textDecoration);
txt::TextDecorationStyle RosenConvertTxtTextDecorationStyle(TextDecorationStyle& textDecorationStyle);
txt::PlaceholderAlignment RosenConvertAlignment(const PlaceholderAlignment& alignment);
txt::PlaceholderRun RosenConvertPlaceholderRun(const PlaceholderRun& placeholderRun);
void RosenConvertTxtStyle(const TextStyle& textStyle, txt::TextStyle& txtStyle);
void RosenConvertTxtPen(const TextStyle& textStyle, txt::TextStyle& txtStyle);
void RosenConvertTypographyStyle(const TypographyStyle& typographyStyle, txt::ParagraphStyle& txtParagraphStyle);
txt::Paragraph::RectHeightStyle RosenConvertTxtRectHeightStyle(TypographyProperties::RectHeightStyle heightStyle);
txt::Paragraph::RectWidthStyle RosenConvertTxtRectWidthStyle(TypographyProperties::RectWidthStyle widthStyle);
TextDirection TxtConvertRosenTextDirection(const txt::TextDirection& txtTextBox);
TypographyProperties::TextBox TxtConvertRosenTextBox(txt::Paragraph::TextBox& txtTextBox);
TypographyProperties::Affinity TxtConvertRosenAffinity(const txt::Paragraph::Affinity& txtAffinity);
TypographyProperties::PositionAndAffinity TxtConvertPosAndAffinity(
    txt::Paragraph::PositionWithAffinity& posAndAffinity);
TypographyProperties::Range<size_t> TxtConvertRange(txt::Paragraph::Range<size_t>& txtRange);
} // namespace rosen
#endif // ROSEN_TEXT_PROPERTIES_ROSEN_CONVERT_TXT_H
