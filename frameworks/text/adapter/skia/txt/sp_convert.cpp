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

#include "sp_convert.h"

#include "impl/text_font_utils.h"
#include "paint_record.h"
#include "paragraph_style.h"
#include "modules/skparagraph/include/ParagraphPainter.h"
#include "modules/skparagraph/include/TextStyle.h"
#include "utils/text_log.h"

namespace OHOS {
namespace Rosen {
namespace SPText {
namespace skt = skia::textlayout;
using PaintID = skt::ParagraphPainter::PaintID;

void GetExtraTextStyleAttributes(const skt::TextStyle& skStyle, TextStyle& textStyle)
{
    for (const auto& [tag, value] : skStyle.getFontFeatures()) {
        textStyle.fontFeatures.SetFeature(tag.c_str(), value);
    }
    textStyle.textShadows.clear();
    for (const skt::TextShadow& skShadow : skStyle.getShadows()) {
        TextShadow shadow;
        shadow.offset = skShadow.fOffset;
        shadow.blurSigma = skShadow.fBlurSigma;
        shadow.color = skShadow.fColor;
        textStyle.textShadows.emplace_back(shadow);
    }
    textStyle.badgeType = static_cast<OHOS::Rosen::TextBadgeType>(skStyle.getTextBadgeType());
    textStyle.maxLineHeight = skStyle.getMaxLineHeight();
    textStyle.minLineHeight = skStyle.getMinLineHeight();
    textStyle.lineHeightStyle = static_cast<OHOS::Rosen::LineHeightStyle>(skStyle.getLineHeightStyle());
}

TextStyle SkStyleToSPTextStyle(const skia::textlayout::TextStyle& skStyle,
    const std::vector<PaintRecord>& paints)
{
    TextStyle txt;
    txt.color = skStyle.getColor();
    txt.decoration = static_cast<TextDecoration>(skStyle.getDecorationType());
    txt.decorationColor = skStyle.getDecorationColor();
    txt.decorationStyle = static_cast<TextDecorationStyle>(skStyle.getDecorationStyle());
    txt.decorationThicknessMultiplier = SkScalarToDouble(skStyle.getDecorationThicknessMultiplier());
    txt.fontWeight = TextFontUtils::GetTxtFontWeight(skStyle.getFontStyle().GetWeight());
    txt.fontStyle = TextFontUtils::GetTxtFontStyle(skStyle.getFontStyle().GetSlant());

    txt.baseline = static_cast<TextBaseline>(skStyle.getTextBaseline());

    for (const SkString& fontFamily : skStyle.getFontFamilies()) {
        txt.fontFamilies.emplace_back(fontFamily.c_str());
    }

    txt.fontSize = SkScalarToDouble(skStyle.getFontSize());
    txt.fontWidth = static_cast<FontWidth>(skStyle.getFontStyle().GetWidth());
    txt.styleId = skStyle.getStyleId();
    txt.letterSpacing = SkScalarToDouble(skStyle.getLetterSpacing());
    txt.wordSpacing = SkScalarToDouble(skStyle.getWordSpacing());
    txt.height = SkScalarToDouble(skStyle.getHeight());
    txt.heightOverride = skStyle.getHeightOverride();
    txt.halfLeading = skStyle.getHalfLeading();
    txt.baseLineShift = SkScalarToDouble(skStyle.getBaselineShift());
    txt.locale = skStyle.getLocale().c_str();
    txt.backgroundRect = { skStyle.getBackgroundRect().color, skStyle.getBackgroundRect().leftTopRadius,
        skStyle.getBackgroundRect().rightTopRadius, skStyle.getBackgroundRect().rightBottomRadius,
        skStyle.getBackgroundRect().leftBottomRadius };
    if (skStyle.hasBackground()) {
        PaintID backgroundId = std::get<PaintID>(skStyle.getBackgroundPaintOrID());
        if ((0 <= backgroundId) && (backgroundId < static_cast<int>(paints.size()))) {
            txt.background = paints[backgroundId];
        } else {
            TEXT_LOGW("Invalid background id %{public}d", backgroundId);
        }
    }
    if (skStyle.hasForeground()) {
        PaintID foregroundId = std::get<PaintID>(skStyle.getForegroundPaintOrID());
        if ((0 <= foregroundId) && (foregroundId < static_cast<int>(paints.size()))) {
            txt.foreground = paints[foregroundId];
            txt.colorPlaceholder = static_cast<uint8_t>(paints[foregroundId].color.GetPlaceholder());
        } else {
            TEXT_LOGW("Invalid foreground id %{public}d", foregroundId);
        }
    }
    GetExtraTextStyleAttributes(skStyle, txt);
    return txt;
}

ParagraphStyle SkParagraphStyleToParagraphStyle(const skt::ParagraphStyle& skStyle)
{
    ParagraphStyle paraStyle;
    paraStyle.fontWeight = static_cast<FontWeight>(skStyle.getTextStyle().getFontStyle().GetWeight());
    paraStyle.fontWidth = static_cast<FontWidth>(skStyle.getTextStyle().getFontStyle().GetWidth());
    paraStyle.fontStyle = static_cast<FontStyle>(skStyle.getTextStyle().getFontStyle().GetSlant());
    paraStyle.fontSize = SkScalarToDouble(skStyle.getTextStyle().getFontSize());
    paraStyle.fontFamily = skStyle.getTextStyle().getFontFamilies().empty() ? "" :
        skStyle.getTextStyle().getFontFamilies()[0].c_str();
    paraStyle.height = SkScalarToDouble(skStyle.getHeight());
    paraStyle.heightOverride = false;
    paraStyle.textAlign = static_cast<TextAlign>(skStyle.getTextAlign());
    paraStyle.textDirection = static_cast<TextDirection>(skStyle.getTextDirection());
    paraStyle.maxLines = skStyle.getMaxLines();
    paraStyle.ellipsis = skStyle.getEllipsisUtf16();
    paraStyle.locale = skStyle.getTextStyle().getLocale().c_str();
    paraStyle.textHeightBehavior = static_cast<TextHeightBehavior>(skStyle.getTextHeightBehavior());
    paraStyle.hintingIsOn = skStyle.hintingIsOn();
    paraStyle.breakStrategy = BreakStrategy::GREEDY;
    paraStyle.paragraphSpacing = SkScalarToDouble(skStyle.getParagraphSpacing());
    paraStyle.halfLeading = skStyle.getTextStyle().getHalfLeading();

    // Strut style
    const auto& strutStyle = skStyle.getStrutStyle();
    paraStyle.strutEnabled = strutStyle.getStrutEnabled();
    paraStyle.strutFontWeight = static_cast<FontWeight>(strutStyle.getFontStyle().GetWeight());
    paraStyle.strutFontWidth = static_cast<FontWidth>(strutStyle.getFontStyle().GetWidth());
    paraStyle.strutFontStyle = static_cast<FontStyle>(strutStyle.getFontStyle().GetSlant());
    paraStyle.strutFontSize = SkScalarToDouble(strutStyle.getFontSize());
    paraStyle.strutHeight = SkScalarToDouble(strutStyle.getHeight());
    paraStyle.strutHeightOverride = strutStyle.getHeightOverride();
    paraStyle.strutHalfLeading = strutStyle.getHalfLeading();
    paraStyle.strutLeading = SkScalarToDouble(strutStyle.getLeading());
    paraStyle.forceStrutHeight = strutStyle.getForceStrutHeight();

    // Text style
    paraStyle.spTextStyle.color = skStyle.getTextStyle().getColor();
    paraStyle.spTextStyle.fontWeight = static_cast<FontWeight>(skStyle.getTextStyle().getFontStyle().GetWeight());
    paraStyle.spTextStyle.fontStyle = static_cast<FontStyle>(skStyle.getTextStyle().getFontStyle().GetSlant());
    paraStyle.spTextStyle.fontSize = SkScalarToDouble(skStyle.getTextStyle().getFontSize());
    // paraStyle.spTextStyle.fontFamilies = skStyle.getTextStyle().getFontFamilies();
    paraStyle.spTextStyle.letterSpacing = SkScalarToDouble(skStyle.getTextStyle().getLetterSpacing());
    paraStyle.spTextStyle.height = SkScalarToDouble(skStyle.getTextStyle().getHeight());
    paraStyle.spTextStyle.heightOverride = skStyle.getTextStyle().getHeightOverride();
    paraStyle.spTextStyle.halfLeading = skStyle.getTextStyle().getHalfLeading();
    paraStyle.spTextStyle.locale = skStyle.getTextStyle().getLocale().c_str();

    return paraStyle;
}

} // namespace SPText
} // namespace Rosen
} // namespace OHOS