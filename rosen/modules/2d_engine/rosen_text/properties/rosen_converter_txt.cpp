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

#include "rosen_text/properties/rosen_converter_txt.h"
#include "engine_adapter/skia_adapter/skia_paint.h"

namespace rosen {
txt::FontWeight RosenConvertTxtFontWeight(FontWeight fontWeight)
{
    txt::FontWeight txtFontWeight;
    switch (fontWeight) {
        case FontWeight::W100:
            txtFontWeight = txt::FontWeight::w100;
            break;
        case FontWeight::W200:
            txtFontWeight = txt::FontWeight::w200;
            break;
        case FontWeight::W300:
            txtFontWeight = txt::FontWeight::w300;
            break;
        case FontWeight::W400:
            txtFontWeight = txt::FontWeight::w400;
            break;
        case FontWeight::W500:
            txtFontWeight = txt::FontWeight::w500;
            break;
        case FontWeight::W600:
            txtFontWeight = txt::FontWeight::w600;
            break;
        case FontWeight::W700:
            txtFontWeight = txt::FontWeight::w700;
            break;
        case FontWeight::W800:
            txtFontWeight = txt::FontWeight::w800;
            break;
        case FontWeight::W900:
            txtFontWeight = txt::FontWeight::w900;
            break;
        default:
            txtFontWeight = txt::FontWeight::w400;
            break;
    }
    return txtFontWeight;
}

txt::FontStyle RosenConvertTxtFontStyle(FontStyle fontStyle)
{
    txt::FontStyle txtFontStyle;
    switch (fontStyle) {
        case FontStyle::NORMAL:
            txtFontStyle = txt::FontStyle::normal;
            break;
        case FontStyle::ITALIC:
            txtFontStyle = txt::FontStyle::italic;
            break;
        default:
            txtFontStyle = txt::FontStyle::normal;
            break;
    }
    return txtFontStyle;
}

txt::TextBaseline RosenConvertTxtTextBaseline(TextBaseline textBaseline)
{
    txt::TextBaseline txtTextBaseline;
    switch (textBaseline) {
        case TextBaseline::ALPHABETIC:
            txtTextBaseline = txt::TextBaseline::kAlphabetic;
            break;
        case TextBaseline::IDEOGRAPHIC:
            txtTextBaseline = txt::TextBaseline::kIdeographic;
            break;
        default:
            txtTextBaseline = txt::TextBaseline::kAlphabetic;
            break;
    }
    return txtTextBaseline;
}

txt::TextDecoration RosenConvertTxtTextDecoration(TextDecoration textDecoration)
{
    txt::TextDecoration txtTextDecoration;
    switch (textDecoration) {
        case TextDecoration::NONE:
            txtTextDecoration = txt::TextDecoration::kNone;
            break;
        case TextDecoration::UNDERLINE:
            txtTextDecoration = txt::TextDecoration::kUnderline;
            break;
        case TextDecoration::OVERLINE:
            txtTextDecoration = txt::TextDecoration::kOverline;
            break;
        case TextDecoration::LINETHROUGH:
            txtTextDecoration = txt::TextDecoration::kLineThrough;
            break;
        default:
            txtTextDecoration = txt::TextDecoration::kNone;
            break;
    }
    return txtTextDecoration;
}

txt::TextDecorationStyle RosenConvertTxtTextDecorationStyle(TextDecorationStyle textDecorationStyle)
{
    txt::TextDecorationStyle txtTextDecorationStyle;
    switch (textDecorationStyle) {
        case TextDecorationStyle::SOLID:
            txtTextDecorationStyle = txt::TextDecorationStyle::kSolid;
            break;
        case TextDecorationStyle::DOUBLE:
            txtTextDecorationStyle = txt::TextDecorationStyle::kDouble;
            break;
        case TextDecorationStyle::DOTTED:
            txtTextDecorationStyle = txt::TextDecorationStyle::kDotted;
            break;
        case TextDecorationStyle::DASHED:
            txtTextDecorationStyle = txt::TextDecorationStyle::kDashed;
            break;
        case TextDecorationStyle::WAVY:
            txtTextDecorationStyle = txt::TextDecorationStyle::kWavy;
            break;
        default:
            txtTextDecorationStyle = txt::TextDecorationStyle::kSolid;
            break;
    }
    return txtTextDecorationStyle;
}

minikin::BreakStrategy RosenConverMinkinBreakStrategy(BreakStrategy breakStrategy)
{
    minikin::BreakStrategy minkinBreakStrategy;
    switch (breakStrategy) {
        case BreakStrategy::BreakStrategyGreedy:
            minkinBreakStrategy = minikin::BreakStrategy::kBreakStrategy_Greedy;
            break;
        case BreakStrategy::BreakStrategyHighQuality:
            minkinBreakStrategy = minikin::BreakStrategy::kBreakStrategy_HighQuality;
            break;
        case BreakStrategy::BreakStrategyBalanced:
            minkinBreakStrategy = minikin::BreakStrategy::kBreakStrategy_Balanced;
            break;
        default:
            minkinBreakStrategy = minikin::BreakStrategy::kBreakStrategy_Greedy;
            break;
    }
    return minkinBreakStrategy;
}

minikin::WordBreakType RosenConverMinkinWordBreakType(WordBreakType wordBreakType)
{
    minikin::WordBreakType minkinWordBreakType;
    switch (wordBreakType) {
        case WordBreakType::WordBreakTypeNormal:
            minkinWordBreakType = minikin::WordBreakType::kWordBreakType_Normal;
            break;
        case WordBreakType::WordBreakTypeBreakAll:
            minkinWordBreakType = minikin::WordBreakType::kWordBreakType_BreakAll;
            break;
        case WordBreakType::WordBreakTypeBreakWord:
            minkinWordBreakType = minikin::WordBreakType::kWordBreakType_BreakWord;
            break;
        default:
            minkinWordBreakType = minikin::WordBreakType::kWordBreakType_Normal;
            break;
    }
    return minkinWordBreakType;
}

txt::TextDirection RosenConvertTxtTextDirection(TextDirection textDirection)
{
    txt::TextDirection txtTextDirection;
    switch (textDirection) {
        case TextDirection::RTL:
            txtTextDirection = txt::TextDirection::rtl;
            break;
        case TextDirection::LTR:
            txtTextDirection = txt::TextDirection::ltr;
            break;
        default:
            txtTextDirection = txt::TextDirection::rtl;
            break;
    }
    return txtTextDirection;
}

txt::EllipsisModal RosenConvertTxtTextEllipsisModal(EllipsisModal ellipsisModal)
{
    txt::EllipsisModal txtEllipsisModal;
    switch (ellipsisModal) {
        case EllipsisModal::HEAD:
            txtEllipsisModal = txt::EllipsisModal::HEAD;
            break;
        case EllipsisModal::MIDDLE:
            txtEllipsisModal = txt::EllipsisModal::MIDDLE;
            break;
        case EllipsisModal::TAIL:
        default:
            txtEllipsisModal = txt::EllipsisModal::TAIL;
            break;
    }
    return txtEllipsisModal;
}

txt::TextAlign RosenConvertTxtTextAlign(TextAlign textAlign)
{
    txt::TextAlign txtTextAlign;
    switch (textAlign) {
        case TextAlign::LEFT:
            txtTextAlign = txt::TextAlign::left;
            break;
        case TextAlign::RIGHT:
            txtTextAlign = txt::TextAlign::right;
            break;
        case TextAlign::CENTER:
            txtTextAlign = txt::TextAlign::center;
            break;
        case TextAlign::JUSTIFY:
            txtTextAlign = txt::TextAlign::justify;
            break;
        case TextAlign::START:
            txtTextAlign = txt::TextAlign::start;
            break;
        case TextAlign::END:
            txtTextAlign = txt::TextAlign::end;
            break;
        default:
            txtTextAlign = txt::TextAlign::left;
            break;
    }
    return txtTextAlign;
}

txt::PlaceholderAlignment RosenConvertAlignment(const PlaceholderAlignment& alignment)
{
    txt::PlaceholderAlignment txtAalignment;
    switch (alignment) {
        case PlaceholderAlignment::BASELINE:
            txtAalignment = txt::PlaceholderAlignment::kBaseline;
            break;
        case PlaceholderAlignment::ABOVEBASELINE:
            txtAalignment = txt::PlaceholderAlignment::kAboveBaseline;
            break;
        case PlaceholderAlignment::BELOWBASELINE:
            txtAalignment = txt::PlaceholderAlignment::kBelowBaseline;
            break;
        case PlaceholderAlignment::TOP:
            txtAalignment = txt::PlaceholderAlignment::kTop;
            break;
        case PlaceholderAlignment::BOTTOM:
            txtAalignment = txt::PlaceholderAlignment::kBottom;
            break;
        case PlaceholderAlignment::MIDDLE:
            txtAalignment = txt::PlaceholderAlignment::kMiddle;
            break;
        default:
            txtAalignment = txt::PlaceholderAlignment::kBaseline;
            break;
    }
    return txtAalignment;
}

txt::PlaceholderRun RosenConvertPlaceholderRun(const PlaceholderRun& placeholderRun)
{
    txt::PlaceholderRun txtPlaceholderRun;
    txtPlaceholderRun.width = placeholderRun.width_;
    txtPlaceholderRun.height = placeholderRun.height_;
    txtPlaceholderRun.alignment = RosenConvertAlignment(placeholderRun.placeholderalignment_);
    txtPlaceholderRun.baseline_offset = placeholderRun.baselineOffset_;
    return txtPlaceholderRun;
}

void RosenConvertTxtStyle(const TextStyle& textStyle, txt::TextStyle& txtStyle)
{
    txtStyle.color = textStyle.color_.CastToColorQuad();
    txtStyle.decoration = RosenConvertTxtTextDecoration(textStyle.decoration_);
    txtStyle.decoration_color = textStyle.decorationColor_.CastToColorQuad();
    txtStyle.decoration_style = RosenConvertTxtTextDecorationStyle(textStyle.decorationStyle_);
    txtStyle.decoration_thickness_multiplier = textStyle.decorationThicknessMultiplier_;
    txtStyle.font_weight = RosenConvertTxtFontWeight(textStyle.fontWeight_);
    txtStyle.font_style = RosenConvertTxtFontStyle(textStyle.fontStyle_);
    txtStyle.text_baseline  = RosenConvertTxtTextBaseline(textStyle.textBaseline_);
    txtStyle.font_families = textStyle.fontFamilies_;
    txtStyle.font_size = textStyle.fontSize_;
    txtStyle.letter_spacing = textStyle.letterSpacing_;
    txtStyle.word_spacing = textStyle.wordSpacing_;
    txtStyle.height = textStyle.height_;
    txtStyle.has_height_override = textStyle.hasHeightOverride_;
    txtStyle.locale  = textStyle.locale_;
    RosenConvertTxtPen(textStyle, txtStyle);
    auto textShadows = textStyle.textShadows_;
    if (!textShadows.empty()) {
        for (auto shadow : textShadows) {
            txt::TextShadow txtShadow;
            txtShadow.color = shadow.color_.CastToColorQuad();
            txtShadow.offset  = RSPoint{shadow.offset_.GetX(), shadow.offset_.GetY()};
#if defined(USE_CANVASKIT0310_SKIA) || defined(NEW_SKIA)
            // new flutter libtxt not have blur_radius, use blur_sigma
            txtShadow.blur_sigma = shadow.blurRadius_;
#else
            txtShadow.blur_radius = shadow.blurRadius_;
#endif
            txtStyle.text_shadows.emplace_back(txtShadow);
        }
    }

    auto fontFeatures = textStyle.fontFeatures_.getGetFontFeatures();
    if (!fontFeatures.empty()) {
        txt::FontFeatures features;
        for (auto iter = fontFeatures.begin(); iter != fontFeatures.end(); ++iter) {
            features.SetFeature(iter->first, iter->second);
        }
        txtStyle.font_features = features;
    }
}

void RosenConvertTxtPen(const TextStyle& textStyle, txt::TextStyle& txtStyle)
{
    txtStyle.has_background_pen = textStyle.hasBackground_;
    txtStyle.background_pen = textStyle.background_;
    txtStyle.has_foreground_pen = textStyle.hasForeground_;
    txtStyle.foreground_pen = textStyle.foreground_;
}

void RosenConvertTypographyStyle(const TypographyStyle& typographyStyle, txt::ParagraphStyle& txtParagraphStyle)
{
    txtParagraphStyle.font_weight = RosenConvertTxtFontWeight(typographyStyle.fontWeight_);
    txtParagraphStyle.font_style = RosenConvertTxtFontStyle(typographyStyle.fontStyle_);
    txtParagraphStyle.font_family = typographyStyle.fontFamily_;
    txtParagraphStyle.font_size = typographyStyle.fontSize_;
    txtParagraphStyle.height = typographyStyle.height_;
    txtParagraphStyle.has_height_override = typographyStyle.hasHeightOverride_;
    txtParagraphStyle.strut_enabled = typographyStyle.strutEnabled_;
    txtParagraphStyle.strut_font_weight =  RosenConvertTxtFontWeight(typographyStyle.strutFontWeight_);
    txtParagraphStyle.strut_font_style = RosenConvertTxtFontStyle(typographyStyle.strutFontStyle_);
    txtParagraphStyle.strut_font_families = typographyStyle.strutFontFamilies_;
    txtParagraphStyle.strut_font_size = typographyStyle.strutFontSize_;
    txtParagraphStyle.strut_height = typographyStyle.strutHeight_;
    txtParagraphStyle.strut_has_height_override = typographyStyle.strutHasHeightOverride_;
    txtParagraphStyle.strut_leading = typographyStyle.strutLeading_;
    txtParagraphStyle.force_strut_height = typographyStyle.forceStrutHeight_;
    txtParagraphStyle.text_align = RosenConvertTxtTextAlign(typographyStyle.textAlign_);
    txtParagraphStyle.text_direction = RosenConvertTxtTextDirection(typographyStyle.textDirection_);
    txtParagraphStyle.max_lines = typographyStyle.maxLines_;
    txtParagraphStyle.ellipsis = typographyStyle.ellipsis_;
    txtParagraphStyle.ellipsis_modal = RosenConvertTxtTextEllipsisModal(typographyStyle.ellipsisModal_);
    txtParagraphStyle.locale = typographyStyle.locale_;
    txtParagraphStyle.break_strategy = RosenConverMinkinBreakStrategy(typographyStyle.breakStrategy_);
    txtParagraphStyle.word_break_type = RosenConverMinkinWordBreakType(typographyStyle.wordBreakType_);
}

TextDirection TxtConvertRosenTextDirection(const txt::TextDirection& txtTextBox)
{
    TextDirection textDirection;
    switch (txtTextBox) {
        case txt::TextDirection::rtl:
            textDirection = TextDirection::RTL;
            break;
        case txt::TextDirection::ltr:
            textDirection = TextDirection::LTR;
            break;
        default:
            textDirection = TextDirection::RTL;
            break;
    }
    return textDirection;
}

TypographyProperties::TextBox TxtConvertRosenTextBox(txt::Paragraph::TextBox& txtTextBox)
{
    OHOS::Rosen::Drawing::Rect textRect(txtTextBox.rect.left(), txtTextBox.rect.top(),
                                        txtTextBox.rect.right(), txtTextBox.rect.bottom());
    TypographyProperties::TextBox rosenTextBox =
        TypographyProperties::TextBox(textRect, TxtConvertRosenTextDirection(txtTextBox.direction));
    return rosenTextBox;
}

txt::Paragraph::RectHeightStyle RosenConvertTxtRectHeightStyle(TypographyProperties::RectHeightStyle heightStyle)
{
    txt::Paragraph::RectHeightStyle txtRectHeightStyle;
    switch (heightStyle) {
        case TypographyProperties::RectHeightStyle::TIGHT:
            txtRectHeightStyle = txt::Paragraph::RectHeightStyle::kTight;
            break;
        case TypographyProperties::RectHeightStyle::MAX:
            txtRectHeightStyle = txt::Paragraph::RectHeightStyle::kMax;
            break;
        case TypographyProperties::RectHeightStyle::INCLUDELINESPACEMIDDLE:
            txtRectHeightStyle = txt::Paragraph::RectHeightStyle::kIncludeLineSpacingMiddle;
            break;
        case TypographyProperties::RectHeightStyle::INCLUDELINESPACETOP:
            txtRectHeightStyle = txt::Paragraph::RectHeightStyle::kIncludeLineSpacingTop;
            break;
        case TypographyProperties::RectHeightStyle::INCLUDELINESPACEBOTTOM:
            txtRectHeightStyle = txt::Paragraph::RectHeightStyle::kIncludeLineSpacingBottom;
            break;
        case TypographyProperties::RectHeightStyle::STRUCT:
            txtRectHeightStyle = txt::Paragraph::RectHeightStyle::kStrut;
            break;
        default:
            txtRectHeightStyle = txt::Paragraph::RectHeightStyle::kTight;
            break;
    }
    return txtRectHeightStyle;
}

txt::Paragraph::RectWidthStyle RosenConvertTxtRectWidthStyle(TypographyProperties::RectWidthStyle widthStyle)
{
    txt::Paragraph::RectWidthStyle txtRectWidthStyle;
    switch (widthStyle) {
        case TypographyProperties::RectWidthStyle::TIGHT:
            txtRectWidthStyle = txt::Paragraph::RectWidthStyle::kTight;
            break;
        case TypographyProperties::RectWidthStyle::MAX:
            txtRectWidthStyle = txt::Paragraph::RectWidthStyle::kMax;
            break;
        default:
            txtRectWidthStyle = txt::Paragraph::RectWidthStyle::kTight;
            break;
    }
    return txtRectWidthStyle;
}

TypographyProperties::Affinity TxtConvertRosenAffinity(const txt::Paragraph::Affinity& txtAffinity)
{
    TypographyProperties::Affinity rosenAffinity;
    switch (txtAffinity) {
        case txt::Paragraph::Affinity::UPSTREAM:
            rosenAffinity = TypographyProperties::Affinity::UPSTREAM;
            break;
        case txt::Paragraph::Affinity::DOWNSTREAM:
            rosenAffinity = TypographyProperties::Affinity::DOWNSTREAM;
            break;
        default:
            rosenAffinity = TypographyProperties::Affinity::UPSTREAM;
    }
    return rosenAffinity;
}


TypographyProperties::PositionAndAffinity TxtConvertPosAndAffinity(
    txt::Paragraph::PositionWithAffinity& posAndAffinity)
{
    TypographyProperties::PositionAndAffinity positionAndAffinity =
        TypographyProperties::PositionAndAffinity(posAndAffinity.position,
                                                  TxtConvertRosenAffinity(posAndAffinity.affinity));
    return positionAndAffinity;
}

TypographyProperties::Range<size_t> TxtConvertRange(txt::Paragraph::Range<size_t>& txtRange)
{
    TypographyProperties::Range<size_t> range = TypographyProperties::Range(txtRange.start, txtRange.end);
    return range;
}
} // namespace rosen
