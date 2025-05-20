/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.. All rights reserved.
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

#include "hm_symbol_txt.h"
#include "modules/skparagraph/include/Paragraph.h"
#include "paragraph_impl.h"

#include <algorithm>
#include <bitset>
#include <memory>
#include <string>
#include <tuple>

#include "include/private/SkTArray.h"
#include "paragraph_style.h"
#include "SkScalar.h"
#include "symbol_engine/hm_symbol_run.h"
#include "symbol_engine/hm_symbol_txt.h"
#include "text_font_utils.h"
#include "text_style.h"

namespace OHOS {
namespace Rosen {
namespace SPText {
using PaintID = skt::ParagraphPainter::PaintID;
using ParagraphStyleFuncVector = std::vector<std::function<void(skt::Paragraph&, const ParagraphStyle&,
    skt::InternalState&)>>;
using TextStyleFuncVector = std::vector<std::function<void(Paragraph&, skt::Block&, const TextStyle&,
    skt::InternalState&)>>;
using SymbolFuncVecotr = std::vector<std::function<void(const HMSymbolTxt&, std::shared_ptr<HMSymbolRun>&,
    skt::InternalState&)>>;

namespace {
    ParagraphStyleFuncVector g_paragraphStyleHandlers = {
        [](skt::Paragraph& paragraph, const ParagraphStyle& style, skt::InternalState& state) {
            paragraph.getParagraphStyle().exportTextStyle().setFontSize(style.fontSize);

            SkTArray<skt::Block, true>& textStyles = paragraph.exportTextStyles();
            if (!textStyles.empty() && (textStyles.front().fStyle.getTextStyleUid() == style.defaultTextStyleUid)) {
                textStyles.front().fStyle.setFontSize(style.fontSize);
            }
            state = std::min(skt::InternalState::kIndexed, state);
        },

        [](skt::Paragraph& paragraph, const ParagraphStyle& style, skt::InternalState& state) {
            paragraph.getParagraphStyle().setTextDirection(static_cast<skt::TextDirection>(style.textDirection));
            state = std::min(skt::InternalState::kIndexed, state);
        },

        [](skt::Paragraph& paragraph, const ParagraphStyle& style, skt::InternalState& state) {
            paragraph.getParagraphStyle().setMaxLines(style.maxLines);
            state = std::min(skt::InternalState::kShaped, state);
        },

        [](skt::Paragraph& paragraph, const ParagraphStyle& style, skt::InternalState& state) {
            RSFontStyle skiaFontStyle = paragraph.getParagraphStyle().getTextStyle().getFontStyle();
            RSFontStyle fontStyle{TextFontUtils::GetSkiaFontWeight(style.fontWeight),
                skiaFontStyle.GetWidth(), skiaFontStyle.GetSlant()};
            paragraph.getParagraphStyle().exportTextStyle().setFontStyle(fontStyle);
            SkTArray<skt::Block, true>& textStyles = paragraph.exportTextStyles();

            if (!textStyles.empty() && (textStyles.front().fStyle.getTextStyleUid() == style.defaultTextStyleUid)) {
                textStyles.front().fStyle.setFontStyle(fontStyle);
                state = std::min(skt::InternalState::kIndexed, state);
            }
        },

        [](skt::Paragraph& paragraph, const ParagraphStyle& style, skt::InternalState& state) {
            RSFontStyle skiaFontStyle = paragraph.getParagraphStyle().getTextStyle().getFontStyle();
            RSFontStyle fontStyle{skiaFontStyle.GetWeight(), skiaFontStyle.GetWidth(),
                TextFontUtils::GetSkiaFontSlant(style.fontStyle)};
            paragraph.getParagraphStyle().exportTextStyle().setFontStyle(fontStyle);

            SkTArray<skt::Block, true>& textStyles = paragraph.exportTextStyles();
            if (!textStyles.empty() && (textStyles.front().fStyle.getTextStyleUid() == style.defaultTextStyleUid)) {
                textStyles.front().fStyle.setFontStyle(fontStyle);
                state = std::min(skt::InternalState::kIndexed, state);
            }
        },

        [](skt::Paragraph& paragraph, const ParagraphStyle& style, skt::InternalState& state) {
            std::vector<std::string> spFontFamiles {style.fontFamily};
            std::vector<SkString> skiaFontFamiles = TextFontUtils::GetSkiaFontfamilies(spFontFamiles);
            paragraph.getParagraphStyle().exportTextStyle().setFontFamilies(skiaFontFamiles);

            SkTArray<skt::Block, true>& textStyles = paragraph.exportTextStyles();
            if (!textStyles.empty() && (textStyles.front().fStyle.getTextStyleUid() == style.defaultTextStyleUid)) {
                textStyles.front().fStyle.setFontFamilies(skiaFontFamiles);
                state = std::min(skt::InternalState::kIndexed, state);
            }
        },

        [](skt::Paragraph& paragraph, const ParagraphStyle& style, skt::InternalState& state) {
            paragraph.getParagraphStyle().exportTextStyle().setHeight(style.height);

            SkTArray<skt::Block, true>& textStyles = paragraph.exportTextStyles();
            if (!textStyles.empty() && (textStyles.front().fStyle.getTextStyleUid() == style.defaultTextStyleUid)) {
                textStyles.front().fStyle.setHeight(style.height);
            }

            if (!textStyles.front().fStyle.getHeightOverride()) {
                return;
            }
            state = std::min(skt::InternalState::kIndexed, state);
        },

        [](skt::Paragraph& paragraph, const ParagraphStyle& style, skt::InternalState& state) {
            paragraph.getParagraphStyle().exportTextStyle().setHeightOverride(style.heightOverride);
            SkTArray<skt::Block, true>& textStyles = paragraph.exportTextStyles();
            if (!textStyles.empty() && (textStyles.front().fStyle.getTextStyleUid() == style.defaultTextStyleUid)) {
                textStyles.front().fStyle.setHeightOverride(style.heightOverride);
                state = std::min(skt::InternalState::kIndexed, state);
            }
        },

        [](skt::Paragraph& paragraph, const ParagraphStyle& style, skt::InternalState& state) {
            skt::TextStyle& skiaTextStyle =  paragraph.getParagraphStyle().exportTextStyle();
            skiaTextStyle.setHalfLeading(style.halfLeading);

            SkTArray<skt::Block, true>& textStyles = paragraph.exportTextStyles();
            if (!textStyles.empty() && (textStyles.front().fStyle.getTextStyleUid() == style.defaultTextStyleUid)) {
                textStyles.front().fStyle.setHalfLeading(style.halfLeading);
                state = std::min(skt::InternalState::kIndexed, state);
            }
        },

        [](skt::Paragraph& paragraph, const ParagraphStyle& style, skt::InternalState& state) {
            skt::StrutStyle& strutStyle =  paragraph.getParagraphStyle().exportStrutStyle();

            if (strutStyle.getStrutEnabled() == style.strutEnabled) {
                return;
            }
            strutStyle.setStrutEnabled(style.strutEnabled);
            state = std::min(skt::InternalState::kIndexed, state);
        },

        [](skt::Paragraph& paragraph, const ParagraphStyle& style, skt::InternalState& state) {
            skt::StrutStyle& strutStyle =  paragraph.getParagraphStyle().exportStrutStyle();
            RSFontStyle fontStyle = strutStyle.getFontStyle();
            strutStyle.setFontStyle(RSFontStyle(TextFontUtils::GetSkiaFontWeight(style.strutFontWeight),
                fontStyle.GetWidth(), fontStyle.GetSlant()));

            if (!strutStyle.getStrutEnabled()) {
                return;
            }
            state = std::min(skt::InternalState::kIndexed, state);
        },

        [](skt::Paragraph& paragraph, const ParagraphStyle& style, skt::InternalState& state) {
            skt::StrutStyle& strutStyle =  paragraph.getParagraphStyle().exportStrutStyle();
            RSFontStyle fontStyle = strutStyle.getFontStyle();
            strutStyle.setFontStyle(RSFontStyle(fontStyle.GetWeight(),
                TextFontUtils::GetSkiaFontWidth(style.strutFontWidth), fontStyle.GetSlant()));

            if (!strutStyle.getStrutEnabled()) {
                return;
            }
            state = std::min(skt::InternalState::kIndexed, state);
        },

        [](skt::Paragraph& paragraph, const ParagraphStyle& style, skt::InternalState& state) {
            skt::StrutStyle& strutStyle =  paragraph.getParagraphStyle().exportStrutStyle();
            RSFontStyle fontStyle = strutStyle.getFontStyle();
            strutStyle.setFontStyle(RSFontStyle(fontStyle.GetWeight(),
                fontStyle.GetWidth(), TextFontUtils::GetSkiaFontSlant(style.strutFontStyle)));

            if (!strutStyle.getStrutEnabled()) {
                return;
            }
            state = std::min(skt::InternalState::kIndexed, state);
        },

        [](skt::Paragraph& paragraph, const ParagraphStyle& style, skt::InternalState& state) {
            skt::StrutStyle& strutStyle =  paragraph.getParagraphStyle().exportStrutStyle();
            std::vector<SkString> skiaFontFamiles = TextFontUtils::GetSkiaFontfamilies(style.strutFontFamilies);
            strutStyle.setFontFamilies(skiaFontFamiles);

            if (!strutStyle.getStrutEnabled()) {
                return;
            }
            state = std::min(skt::InternalState::kIndexed, state);
        },

        [](skt::Paragraph& paragraph, const ParagraphStyle& style, skt::InternalState& state) {
            skt::StrutStyle& strutStyle =  paragraph.getParagraphStyle().exportStrutStyle();
            strutStyle.setFontSize(style.strutFontSize);

            if (!strutStyle.getStrutEnabled()) {
                return;
            }
            state = std::min(skt::InternalState::kIndexed, state);
        },

        [](skt::Paragraph& paragraph, const ParagraphStyle& style, skt::InternalState& state) {
            skt::StrutStyle& strutStyle =  paragraph.getParagraphStyle().exportStrutStyle();
            strutStyle.setHeight(style.strutHeight);

            if (!strutStyle.getStrutEnabled()) {
                return;
            }
            state = std::min(skt::InternalState::kIndexed, state);
        },

        [](skt::Paragraph& paragraph, const ParagraphStyle& style, skt::InternalState& state) {
            skt::StrutStyle& strutStyle =  paragraph.getParagraphStyle().exportStrutStyle();
            strutStyle.setHeightOverride(style.strutHeightOverride);

            if (!strutStyle.getStrutEnabled()) {
                return;
            }
            state = std::min(skt::InternalState::kIndexed, state);
        },

        [](skt::Paragraph& paragraph, const ParagraphStyle& style, skt::InternalState& state) {
            skt::StrutStyle& strutStyle =  paragraph.getParagraphStyle().exportStrutStyle();
            strutStyle.setHalfLeading(style.strutHalfLeading);

            if (!strutStyle.getStrutEnabled()) {
                return;
            }
            state = std::min(skt::InternalState::kIndexed, state);
        },

        [](skt::Paragraph& paragraph, const ParagraphStyle& style, skt::InternalState& state) {
            skt::StrutStyle& strutStyle =  paragraph.getParagraphStyle().exportStrutStyle();
            strutStyle.setForceStrutHeight(style.strutHeight);

            if (!strutStyle.getStrutEnabled()) {
                return;
            }
            state = std::min(skt::InternalState::kIndexed, state);
        },

        [](skt::Paragraph& paragraph, const ParagraphStyle& style, skt::InternalState& state) {
            skt::StrutStyle& strutStyle =  paragraph.getParagraphStyle().exportStrutStyle();
            strutStyle.setLineBreakStrategy(static_cast<skt::LineBreakStrategy>(style.breakStrategy));
            state = std::min(skt::InternalState::kShaped, state);
        },

        [](skt::Paragraph& paragraph, const ParagraphStyle& style, skt::InternalState& state) {
            paragraph.getParagraphStyle().exportStrutStyle()
                .setWordBreakType(static_cast<skt::WordBreakType>(style.wordBreakType));
            state = std::min(skt::InternalState::kShaped, state);
        },

        [](skt::Paragraph& paragraph, const ParagraphStyle& style, skt::InternalState& state) {
            paragraph.getParagraphStyle().setEllipsis(style.ellipsis.c_str());
            state = std::min(skt::InternalState::kShaped, state);
        },

        [](skt::Paragraph& paragraph, const ParagraphStyle& style, skt::InternalState& state) {
            paragraph.getParagraphStyle().setEllipsisMod(static_cast<skt::EllipsisModal>(style.ellipsisModal));
            state = std::min(skt::InternalState::kShaped, state);
        },

        [](skt::Paragraph& paragraph, const ParagraphStyle& style, skt::InternalState& state) {
            paragraph.updateTextAlign(static_cast<skt::TextAlign>(style.textAlign));
            state = std::min(skt::InternalState::kShaped, state);
        },

        [](skt::Paragraph& paragraph, const ParagraphStyle& style, skt::InternalState& state) {
            paragraph.getParagraphStyle().setParagraphSpacing(style.paragraphSpacing);
            state = std::min(skt::InternalState::kShaped, state);
        },

        [](skt::Paragraph& paragraph, const ParagraphStyle& style, skt::InternalState& state) {
            paragraph.getParagraphStyle().setIsEndAddParagraphSpacing(style.isEndAddParagraphSpacing);
            state = std::min(skt::InternalState::kShaped, state);
        },

        [](skt::Paragraph& paragraph, const ParagraphStyle& style, skt::InternalState& state) {
            paragraph.getParagraphStyle().setTextHeightBehavior(
                static_cast<skt::TextHeightBehavior>(style.textHeightBehavior));
            state = std::min(skt::InternalState::kIndexed, state);
        }
    };

    TextStyleFuncVector g_textStyleHandlers = {
        [](Paragraph& paragraph, skt::Block& skiaBlock, const TextStyle& spTextStyle, skt::InternalState& state) {
            skt::TextStyle& skiaTextStyle = skiaBlock.fStyle;
            skiaTextStyle.setFontSize(spTextStyle.fontSize);
            state = std::min(skt::InternalState::kIndexed, state);
        },

        [](Paragraph& paragraph, skt::Block& skiaBlock, const TextStyle& spTextStyle, skt::InternalState& state) {
            skt::TextStyle& skiaTextStyle = skiaBlock.fStyle;
            RSFontStyle skiaFontStyle = skiaTextStyle.getFontStyle();
            skiaTextStyle.setFontStyle(RSFontStyle(TextFontUtils::GetSkiaFontWeight(spTextStyle.fontWeight),
                skiaFontStyle.GetWidth(), skiaFontStyle.GetSlant()));
            FontVariations updateFontVariations;
            updateFontVariations.SetAxisValue("wght", TextFontUtils::GetSkiaFontWeight(spTextStyle.fontWeight));
            if (!updateFontVariations.GetAxisValues().empty()) {
                TextFontUtils::MakeFontArguments(skiaTextStyle, updateFontVariations);
            }
            state = std::min(skt::InternalState::kIndexed, state);
        },

        [](Paragraph& paragraph, skt::Block& skiaBlock, const TextStyle& spTextStyle, skt::InternalState& state) {
            skt::TextStyle& skiaTextStyle = skiaBlock.fStyle;
            RSFontStyle skiaFontStyle = skiaTextStyle.getFontStyle();
            skiaTextStyle.setFontStyle(RSFontStyle(skiaFontStyle.GetWeight(),
                TextFontUtils::GetSkiaFontWidth(spTextStyle.fontWidth), skiaFontStyle.GetSlant()));
            state = std::min(skt::InternalState::kIndexed, state);
        },

        [](Paragraph& paragraph, skt::Block& skiaBlock, const TextStyle& spTextStyle, skt::InternalState& state) {
            skt::TextStyle& skiaTextStyle = skiaBlock.fStyle;
            RSFontStyle skiaFontStyle = skiaTextStyle.getFontStyle();
            skiaTextStyle.setFontStyle(RSFontStyle(skiaFontStyle.GetWeight(), skiaFontStyle.GetWidth(),
                TextFontUtils::GetSkiaFontSlant(spTextStyle.fontStyle)));
            state = std::min(skt::InternalState::kIndexed, state);
        },

        [](Paragraph& paragraph, skt::Block& skiaBlock, const TextStyle& spTextStyle, skt::InternalState& state) {
            skt::TextStyle& skiaTextStyle = skiaBlock.fStyle;
            std::vector<SkString> skiaFontFamiles = TextFontUtils::GetSkiaFontfamilies(spTextStyle.fontFamilies);
            skiaTextStyle.setFontFamilies(skiaFontFamiles);
            state = std::min(skt::InternalState::kIndexed, state);
        },

        [](Paragraph& paragraph, skt::Block& skiaBlock, const TextStyle& spTextStyle, skt::InternalState& state) {
            skt::TextStyle& skiaTextStyle = skiaBlock.fStyle;
            skiaTextStyle.setLetterSpacing(spTextStyle.letterSpacing);
            state = std::min(skt::InternalState::kIndexed, state);
        },

        [](Paragraph& paragraph, skt::Block& skiaBlock, const TextStyle& spTextStyle, skt::InternalState& state) {
            skt::TextStyle& skiaTextStyle = skiaBlock.fStyle;
            skiaTextStyle.setWordSpacing(spTextStyle.wordSpacing);
            state = std::min(skt::InternalState::kIndexed, state);
        },

        [](Paragraph& paragraph, skt::Block& skiaBlock, const TextStyle& spTextStyle, skt::InternalState& state) {
            skt::TextStyle& skiaTextStyle = skiaBlock.fStyle;
            skiaTextStyle.setHeightOverride(spTextStyle.heightOverride);
            state = std::min(skt::InternalState::kIndexed, state);
        },

        [](Paragraph& paragraph, skt::Block& skiaBlock, const TextStyle& spTextStyle, skt::InternalState& state) {
            skt::TextStyle& skiaTextStyle = skiaBlock.fStyle;
            skiaTextStyle.setHeight(spTextStyle.height);
            if (skiaTextStyle.getHeightOverride()) {
                state = std::min(skt::InternalState::kIndexed, state);
            }
        },

        [](Paragraph& paragraph, skt::Block& skiaBlock, const TextStyle& spTextStyle, skt::InternalState& state) {
            skt::TextStyle& skiaTextStyle = skiaBlock.fStyle;
            skiaTextStyle.resetFontFeatures();
            for (std::pair<std::string, int> spFontFeature: spTextStyle.fontFeatures.GetFontFeatures()) {
                skiaTextStyle.addFontFeature(static_cast<SkString>(spFontFeature.first.c_str()), spFontFeature.second);
            }
            state = std::min(skt::InternalState::kIndexed, state);
        },

        [](Paragraph& paragraph, skt::Block& skiaBlock, const TextStyle& spTextStyle, skt::InternalState& state) {
            skt::TextStyle& skiaTextStyle = skiaBlock.fStyle;
            if (!spTextStyle.fontVariations.GetAxisValues().empty()) {
                TextFontUtils::MakeFontArguments(skiaTextStyle, spTextStyle.fontVariations);
                state = std::min(skt::InternalState::kIndexed, state);
            }
        },

        [](Paragraph& paragraph, skt::Block& skiaBlock, const TextStyle& spTextStyle, skt::InternalState& state) {
            skt::TextStyle& skiaTextStyle = skiaBlock.fStyle;
            skiaTextStyle.setBaselineShift(spTextStyle.baseLineShift);
            state = std::min(skt::InternalState::kIndexed, state);
        },

        [](Paragraph& paragraph, skt::Block& skiaBlock, const TextStyle& spTextStyle, skt::InternalState& state) {
            skt::TextStyle& skiaTextStyle = skiaBlock.fStyle;
            skiaTextStyle.setDecoration(static_cast<skt::TextDecoration>(spTextStyle.decoration));
            state = std::min(skt::InternalState::kFormatted, state);
        },

        [](Paragraph& paragraph, skt::Block& skiaBlock, const TextStyle& spTextStyle, skt::InternalState& state) {
            skt::TextStyle& skiaTextStyle = skiaBlock.fStyle;
            skiaTextStyle.setDecorationColor(spTextStyle.decorationColor);
            state = std::min(skt::InternalState::kFormatted, state);
        },

        [](Paragraph& paragraph, skt::Block& skiaBlock, const TextStyle& spTextStyle, skt::InternalState& state) {
            skt::TextStyle& skiaTextStyle = skiaBlock.fStyle;
            skiaTextStyle.setDecorationStyle(static_cast<skt::TextDecorationStyle>(spTextStyle.decorationStyle));
            state = std::min(skt::InternalState::kFormatted, state);
        },

        [](Paragraph& paragraph, skt::Block& skiaBlock, const TextStyle& spTextStyle, skt::InternalState& state) {
            skt::TextStyle& skiaTextStyle = skiaBlock.fStyle;
            skiaTextStyle.setDecorationThicknessMultiplier(SkDoubleToScalar(spTextStyle.decorationThicknessMultiplier));
            state = std::min(skt::InternalState::kFormatted, state);
        },

        [](Paragraph& paragraph, skt::Block& skiaBlock, const TextStyle& spTextStyle, skt::InternalState& state) {
            skt::TextStyle& skiaTextStyle = skiaBlock.fStyle;
            skiaTextStyle.setBackgroundRect({spTextStyle.backgroundRect.color, spTextStyle.backgroundRect.leftTopRadius,
                spTextStyle.backgroundRect.rightTopRadius, spTextStyle.backgroundRect.rightBottomRadius,
                spTextStyle.backgroundRect.leftBottomRadius});
            state = std::min(skt::InternalState::kFormatted, state);
        },

        [](Paragraph& paragraph, skt::Block& skiaBlock, const TextStyle& spTextStyle, skt::InternalState& state) {
            skt::TextStyle& skiaTextStyle = skiaBlock.fStyle;
            skiaTextStyle.setStyleId(spTextStyle.styleId);
            state = std::min(skt::InternalState::kFormatted, state);
        },

        [](Paragraph& paragraph, skt::Block& skiaBlock, const TextStyle& spTextStyle, skt::InternalState& state) {
            skt::TextRange textRange = skiaBlock.fRange;
            paragraph.UpdateColor(textRange.start, textRange.end, spTextStyle.color, skt::UtfEncodeType::kUtf16);
            state = std::min(skt::InternalState::kFormatted, state);
        },

        [](Paragraph& paragraph, skt::Block& skiaBlock, const TextStyle& spTextStyle, skt::InternalState& state) {
            skt::TextStyle& skiaTextStyle = skiaBlock.fStyle;
            bool hasShadow = skiaTextStyle.getShadowNumber() > 0;
            skiaTextStyle.resetShadows();
            for (const TextShadow& txtShadow : spTextStyle.textShadows) {
                skiaTextStyle.addShadow(TextFontUtils::MakeTextShadow(txtShadow));
            }
            if (hasShadow && spTextStyle.textShadows.size() > 0) {
                state = std::min(skt::InternalState::kFormatted, state);
                return;
            }
            state = std::min(skt::InternalState::kShaped, state);
        },

        [](Paragraph& paragraph, skt::Block& skiaBlock, const TextStyle& spTextStyle, skt::InternalState& state) {
            skt::TextStyle& skiaTextStyle = skiaBlock.fStyle;
            skiaTextStyle.setHalfLeading(spTextStyle.halfLeading);
            state = std::min(skt::InternalState::kIndexed, state);
        }
    };

    SymbolFuncVecotr g_symbolStyleHandlers = {
        [](const HMSymbolTxt& symbolStyle, std::shared_ptr<HMSymbolRun>& symbolRun, skt::InternalState& state) {
            symbolRun->SetSymbolEffect(symbolStyle.GetEffectStrategy());
        },
        [](const HMSymbolTxt& symbolStyle, std::shared_ptr<HMSymbolRun>& symbolRun, skt::InternalState& state) {
            symbolRun->SetAnimationMode(symbolStyle.GetAnimationMode());
        },
        [](const HMSymbolTxt& symbolStyle, std::shared_ptr<HMSymbolRun>& symbolRun, skt::InternalState& state) {
            symbolRun->SetAnimationStart(symbolStyle.GetAnimationStart());
        },
        [](const HMSymbolTxt& symbolStyle, std::shared_ptr<HMSymbolRun>& symbolRun, skt::InternalState& state) {
            symbolRun->SetCommonSubType(symbolStyle.GetCommonSubType());
        },
        [](const HMSymbolTxt& symbolStyle, std::shared_ptr<HMSymbolRun>& symbolRun, skt::InternalState& state) {
            symbolRun->SetRenderColor(symbolStyle.GetRenderColor());
        },
        [](const HMSymbolTxt& symbolStyle, std::shared_ptr<HMSymbolRun>& symbolRun, skt::InternalState& state) {
            symbolRun->SetRenderMode(symbolStyle.GetRenderMode());
        },
    };
}

void ParagraphImpl::ParagraphStyleUpdater(skt::Paragraph& skiaParagraph, const ParagraphStyle& spParagraphStyle,
    skt::InternalState& state)
{
    const std::bitset<static_cast<size_t>(RelayoutParagraphStyleAttribute::PARAGRAPH_STYLE_ATTRIBUTE_BUTT)>&
        paragraphStyleChangeBitmap = spParagraphStyle.relayoutChangeBitmap;
    if (!paragraphStyleChangeBitmap.any()) {
        return;
    }

    for (size_t i = 0; i < paragraphStyleChangeBitmap.size(); ++i) {
        if (!paragraphStyleChangeBitmap.test(i)) {
            continue;
        }
        g_paragraphStyleHandlers[i](skiaParagraph, spParagraphStyle, state);
    }
}

void ParagraphImpl::TextStyleUpdater(skt::Block& skiaBlock, const TextStyle& spTextStyle, skt::InternalState& state)
{
    const std::bitset<static_cast<size_t>(RelayoutTextStyleAttribute::TEXT_STYLE_ATTRIBUTE_BUTT)>&
        relayoutChangeBitmap = spTextStyle.relayoutChangeBitmap;

    for (size_t i = 0; i < relayoutChangeBitmap.size(); ++i) {
        if (!relayoutChangeBitmap.test(i)) {
            continue;
        }

        g_textStyleHandlers[i](*this, skiaBlock, spTextStyle, state);
    }
}

void ParagraphImpl::UpdateSymbolRun(const HMSymbolTxt& symbolStyle, std::shared_ptr<HMSymbolRun>& hmSymbolRun,
    skt::InternalState& state, size_t index)
{
    if (hmSymbolRun->GetSymbolUid() != symbolStyle.GetSymbolUid()) {
        return;
    }

    g_symbolStyleHandlers[index](symbolStyle, hmSymbolRun, state);
    for (PaintRecord& p : paints_) {
        if (p.symbol.GetSymbolUid() != hmSymbolRun->GetSymbolUid()) {
            continue;
        }
        p.symbol = hmSymbolRun->GetSymbolTxt();
    }
}

void ParagraphImpl::SymbolStyleUpdater(const HMSymbolTxt& symbolStyle, std::vector<std::shared_ptr<HMSymbolRun>>&
    hmSymbolRuns, skt::InternalState& state)
{
    const SymbolBitmapType& symbolStyleBitmap = symbolStyle.GetSymbolBitmap();

    for (size_t i = 0; i < symbolStyleBitmap.size(); ++i) {
        if (!symbolStyleBitmap.test(i)) {
            continue;
        }

        for (size_t j = 0; j < hmSymbolRuns.size(); ++j) {
            UpdateSymbolRun(symbolStyle, hmSymbolRuns[j], state, i);
        }
    }
}

void ParagraphImpl::ApplyParagraphStyleChanges(const ParagraphStyle& style)
{
    if (paragraph_ == nullptr) {
        return;
    }

    skt::InternalState state = paragraph_->getState();
    ParagraphStyleUpdater(*paragraph_, style, state);
    paragraph_->setState(state);
}

void ParagraphImpl::ApplyTextStyleChanges(const std::vector<TextStyle>& textStyles)
{
    if (paragraph_ == nullptr) {
        return;
    }

    SkTArray<skt::Block, true>& skiaTextStyles = paragraph_->exportTextStyles();
    skt::InternalState state = paragraph_->getState();

    for (size_t i = 0; i < textStyles.size(); ++i) {
        const TextStyle& spTextStyle = textStyles[i];
        size_t spTextStyleUid = spTextStyle.textStyleUid;

        for (size_t j = 0; j < skiaTextStyles.size(); ++j) {
            skt::Block& skiaBlock = skiaTextStyles[j];
            size_t skiaTextStyleUid = skiaBlock.fStyle.getTextStyleUid();
            if (skiaTextStyleUid != spTextStyleUid) {
                continue;
            }

            if (spTextStyle.isSymbolGlyph && spTextStyle.symbol.GetSymbolBitmap().any()) {
                SymbolStyleUpdater(spTextStyle.symbol, hmSymbols_, state);
            }

            if (spTextStyle.relayoutChangeBitmap.any()) {
                TextStyleUpdater(skiaBlock, spTextStyle, state);
            }
        }
    }
    paragraph_->setState(state);
}

void ParagraphImpl::Relayout(double width, const ParagraphStyle& paragrahStyle,
    const std::vector<TextStyle>& textStyles)
{
    lineMetrics_.reset();
    lineMetricsStyles_.clear();
    ApplyParagraphStyleChanges(paragrahStyle);
    ApplyTextStyleChanges(textStyles);
    paragraph_->layout(width);
}
} // namespace SPText
} // namespace Rosen
} // namespace OHOS