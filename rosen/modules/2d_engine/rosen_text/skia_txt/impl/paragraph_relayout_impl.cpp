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

#include "paragraph_impl.h"

#include <algorithm>
#include <bitset>
#include <memory>
#include <string>
#include <tuple>

#include "include/private/SkTArray.h"
#include "paragraph_style.h"
#include "text_font_utils.h"
#include "SkScalar.h"
#include "symbol_engine/hm_symbol_run.h"
#include "symbol_engine/hm_symbol_txt.h"
#include "text_style.h"

namespace OHOS {
namespace Rosen {
namespace SPText {
using PaintID = skt::ParagraphPainter::PaintID;

namespace {
std::vector<std::function<void(skt::Paragraph&, const ParagraphStyle&, skt::InternalState&)>>
    g_paragraphStyleHandlers = {
    [](skt::Paragraph& paragraph, const ParagraphStyle& style, skt::InternalState& state) {
        paragraph.getParagraphStyle().exportTextStyle().setFontSize(style.fontSize);

        SkTArray<skt::Block, true>& textStyles = paragraph.exportTextStyles();
        if (!textStyles.empty()) {
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
        if (paragraph.lineNumber() > style.maxLines) {
            state = std::min(skt::InternalState::kShaped, state);
        }
    },

    [](skt::Paragraph& paragraph, const ParagraphStyle& style, skt::InternalState& state) {
        RSFontStyle skiaFontStyle = paragraph.getParagraphStyle().getTextStyle().getFontStyle();
        RSFontStyle fontStyle{TextFontUtils::GetSkiaFontWeight(style.fontWeight),
            skiaFontStyle.GetWidth(), skiaFontStyle.GetSlant()};
        paragraph.getParagraphStyle().exportTextStyle().setFontStyle(fontStyle);
        SkTArray<skt::Block, true>& textStyles = paragraph.exportTextStyles();

        if (!textStyles.empty()) {
            textStyles.front().fStyle.setFontStyle(fontStyle);
        }
        state = std::min(skt::InternalState::kIndexed, state);
    },

    [](skt::Paragraph& paragraph, const ParagraphStyle& style, skt::InternalState& state) {
        RSFontStyle skiaFontStyle = paragraph.getParagraphStyle().getTextStyle().getFontStyle();
        RSFontStyle fontStyle{skiaFontStyle.GetWeight(), skiaFontStyle.GetWidth(),
            TextFontUtils::GetSkiaFontSlant(style.fontStyle)};
        paragraph.getParagraphStyle().exportTextStyle().setFontStyle(fontStyle);

        SkTArray<skt::Block, true>& textStyles = paragraph.exportTextStyles();
        if (!textStyles.empty()) {
            textStyles.front().fStyle.setFontStyle(fontStyle);
        }
        state = std::min(skt::InternalState::kIndexed, state);
    },

    [](skt::Paragraph& paragraph, const ParagraphStyle& style, skt::InternalState& state) {
        std::vector<std::string> spFontFamiles {style.fontFamily};
        std::vector<SkString> skiaFontFamiles = TextFontUtils::GetSkiaFontfamilies(spFontFamiles);
        paragraph.getParagraphStyle().exportTextStyle().setFontFamilies(skiaFontFamiles);

        SkTArray<skt::Block, true>& textStyles = paragraph.exportTextStyles();
        if (!textStyles.empty()) {
            textStyles.front().fStyle.setFontFamilies(skiaFontFamiles);
        }
        state = std::min(skt::InternalState::kIndexed, state);
    },

    [](skt::Paragraph& paragraph, const ParagraphStyle& style, skt::InternalState& state) {
        paragraph.getParagraphStyle().exportTextStyle().setHeight(style.height);

        SkTArray<skt::Block, true>& textStyles = paragraph.exportTextStyles();
        if (!textStyles.empty()) {
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
        if (!textStyles.empty()) {
            textStyles.front().fStyle.setHeightOverride(style.heightOverride);
        }
        state = std::min(skt::InternalState::kIndexed, state);
    },

    [](skt::Paragraph& paragraph, const ParagraphStyle& style, skt::InternalState& state) {
        skt::TextStyle& skiaTextStyle =  paragraph.getParagraphStyle().exportTextStyle();
        skiaTextStyle.setHalfLeading(style.spTextStyle.halfLeading);

        SkTArray<skt::Block, true>& textStyles = paragraph.exportTextStyles();
        if (!textStyles.empty()) {
            textStyles.front().fStyle.setHalfLeading(style.spTextStyle.halfLeading);
        }
        state = std::min(skt::InternalState::kIndexed, state);
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
        state = std::min(skt::InternalState::kLineBroken, state);
    }
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

void ParagraphImpl::ApplyParagraphStyleChanges(const ParagraphStyle& style)
{
    if (paragraph_ == nullptr) {
        return;
    }

    skt::InternalState state = paragraph_->getState();
    ParagraphStyleUpdater(*paragraph_, style, state);
    paragraph_->setState(state);
}

void ParagraphImpl::Relayout(double width, const ParagraphStyle& paragrahStyle,
    const std::vector<TextStyle>& textStyles)
{
    lineMetrics_.reset();
    lineMetricsStyles_.clear();
    std::ignore = textStyles;
    ApplyParagraphStyleChanges(paragrahStyle);
    paragraph_->layout(width);
}

} // namespace SPText
} // namespace Rosen
} // namespace OHOS