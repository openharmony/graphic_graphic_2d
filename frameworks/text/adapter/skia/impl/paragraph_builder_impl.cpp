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

#include "paragraph_builder_impl.h"

#include <string>

#include "common_utils/string_util.h"
#include "text_font_utils.h"
#include "modules/skparagraph/include/ParagraphStyle.h"
#include "modules/skparagraph/include/TextStyle.h"
#include "paragraph_impl.h"
#include "paragraph_line_fetcher_impl.h"
#include "txt/paragraph_style.h"
#include "txt/text_bundle_config_parser.h"
#include "utils/text_log.h"

namespace OHOS {
namespace Rosen {
namespace SPText {
namespace {
const char* DefaultLocale()
{
    static const char* LOCALE_ZH = "zh-Hans";
    return LOCALE_ZH;
}
} // anonymous namespace

ParagraphBuilderImpl::ParagraphBuilderImpl(
    const ParagraphStyle& style, std::shared_ptr<txt::FontCollection> fontCollection)
    : baseStyle_(style.ConvertToTextStyle())
{
    threadId_ = pthread_self();
    builder_ = skt::ParagraphBuilder::make(TextStyleToSkStyle(style), fontCollection->CreateSktFontCollection());
}

ParagraphBuilderImpl::~ParagraphBuilderImpl() = default;

void ParagraphBuilderImpl::PushStyle(const TextStyle& style)
{
    RecordDifferentPthreadCall(__FUNCTION__);
    builder_->pushStyle(TextStyleToSkStyle(style));
}

void ParagraphBuilderImpl::Pop()
{
    RecordDifferentPthreadCall(__FUNCTION__);
    builder_->pop();
}

void ParagraphBuilderImpl::AddText(const std::u16string& text)
{
    RecordDifferentPthreadCall(__FUNCTION__);
    if (TextBundleConfigParser::GetInstance().IsTargetApiVersion(SINCE_API18_VERSION)) {
        std::u16string wideText = text;
        Utf16Utils::HandleIncompleteSurrogatePairs(wideText);
        builder_->addText(wideText);
    } else {
        builder_->addText(text);
    }
}

void ParagraphBuilderImpl::AddPlaceholder(PlaceholderRun& run)
{
    RecordDifferentPthreadCall(__FUNCTION__);
    skt::PlaceholderStyle placeholderStyle;
    placeholderStyle.fHeight = run.height;
    placeholderStyle.fWidth = run.width;
    placeholderStyle.fBaseline = static_cast<skt::TextBaseline>(run.baseline);
    placeholderStyle.fBaselineOffset = run.baselineOffset;
    placeholderStyle.fAlignment = static_cast<skt::PlaceholderAlignment>(run.alignment);

    builder_->addPlaceholder(placeholderStyle);
}

std::unique_ptr<Paragraph> ParagraphBuilderImpl::Build()
{
    RecordDifferentPthreadCall(__FUNCTION__);
    auto ret = std::make_unique<ParagraphImpl>(builder_->Build(), std::move(paints_));
    builder_->Reset();
    return ret;
}

std::unique_ptr<ParagraphLineFetcher> ParagraphBuilderImpl::BuildLineFetcher()
{
    if (builder_ == nullptr) {
        return nullptr;
    }
    auto lineFetcher = builder_->buildLineFetcher();
    if (lineFetcher == nullptr) {
        return nullptr;
    }
    auto fetcherImpl = std::make_unique<ParagraphLineFetcherImpl>(std::move(lineFetcher), std::move(paints_));
    builder_->Reset();
    return fetcherImpl;
}

skt::ParagraphPainter::PaintID ParagraphBuilderImpl::AllocPaintID(const PaintRecord& paint)
{
    paints_.push_back(paint);
    return static_cast<int>(paints_.size()) - 1;
}

skt::TextTabs ConvertToSkTextTab(const TextTab& tab)
{
    return {
        static_cast<skt::TextAlign>(tab.alignment),
        tab.location,
    };
}

void ParagraphBuilderImpl::TextStyleToSKStrutStyle(skt::StrutStyle& strutStyle, const ParagraphStyle& txt)
{
    strutStyle.setFontStyle(TextFontUtils::MakeFontStyle(txt.strutFontWeight, txt.strutFontWidth, txt.strutFontStyle));
    strutStyle.setFontSize(SkDoubleToScalar(txt.strutFontSize));
    strutStyle.setHeight(SkDoubleToScalar(txt.strutHeight));
    strutStyle.setHeightOverride(txt.strutHeightOverride);

    std::vector<SkString> strutFonts;
    std::transform(txt.strutFontFamilies.begin(), txt.strutFontFamilies.end(), std::back_inserter(strutFonts),
        [](const std::string& f) { return SkString(f.c_str()); });
    strutStyle.setHalfLeading(txt.strutHalfLeading);
    strutStyle.setFontFamilies(strutFonts);
    strutStyle.setLeading(txt.strutLeading);
    strutStyle.setForceStrutHeight(txt.forceStrutHeight);
    strutStyle.setStrutEnabled(txt.strutEnabled);
    strutStyle.setWordBreakType(static_cast<skt::WordBreakType>(txt.wordBreakType));
    strutStyle.setLineBreakStrategy(static_cast<skt::LineBreakStrategy>(txt.breakStrategy));
}

skt::ParagraphStyle ParagraphBuilderImpl::TextStyleToSkStyle(const ParagraphStyle& txt)
{
    skt::ParagraphStyle skStyle;
    skt::TextStyle textStyle;

    PaintRecord paint;
    paint.SetColor(textStyle.getColor());
    if (txt.customSpTextStyle) {
        textStyle = this->TextStyleToSkStyle(txt.spTextStyle);
    } else {
        textStyle.setForegroundPaintID(AllocPaintID(paint));
        textStyle.setFontStyle(TextFontUtils::MakeFontStyle(txt.fontWeight, txt.fontWidth, txt.fontStyle));
        textStyle.setFontSize(SkDoubleToScalar(txt.fontSize));
        textStyle.setHeight(SkDoubleToScalar(txt.height));
        textStyle.setHeightOverride(txt.heightOverride);
        textStyle.setFontFamilies({ SkString(txt.fontFamily.c_str()) });
        textStyle.setLocale(SkString(txt.locale.empty() ? DefaultLocale() : txt.locale.c_str()));
        textStyle.setTextStyleUid(txt.defaultTextStyleUid);
        textStyle.setHalfLeading(txt.halfLeading);
    }

    skStyle.setTextStyle(textStyle);
    skStyle.setTextOverflower(txt.textOverflower);
    skt::StrutStyle strutStyle;
    TextStyleToSKStrutStyle(strutStyle, txt);
    skStyle.setStrutStyle(strutStyle);

    skStyle.setTextAlign(static_cast<skt::TextAlign>(txt.textAlign));
    skStyle.setTextDirection(static_cast<skt::TextDirection>(txt.textDirection));
    skStyle.setEllipsisMod(static_cast<skt::EllipsisModal>(txt.ellipsisModal));
    if (txt.ellipsisModal != EllipsisModal::TAIL) {
        skStyle.setEllipsis(txt.ellipsis);
    }
    skStyle.setMaxLines(txt.maxLines);
    skStyle.setEllipsis(txt.ellipsis);
    skStyle.setTextHeightBehavior(static_cast<skt::TextHeightBehavior>(txt.textHeightBehavior));
    if (!txt.hintingIsOn) {
        skStyle.turnHintingOff();
    }
    skStyle.setReplaceTabCharacters(true);
    skStyle.setTextSplitRatio(txt.textSplitRatio);
    skStyle.setTextHeightBehavior(static_cast<skt::TextHeightBehavior>(txt.textHeightBehavior));
    skStyle.setTextTab(ConvertToSkTextTab(txt.tab));
    skStyle.setParagraphSpacing(txt.paragraphSpacing);
    skStyle.setIsEndAddParagraphSpacing(txt.isEndAddParagraphSpacing);
    skStyle.setEnableAutoSpace(txt.enableAutoSpace);

    return skStyle;
}

skt::TextStyle ParagraphBuilderImpl::TextStyleToSkStyle(const TextStyle& txt)
{
    auto skStyle = ConvertTextStyleToSkStyle(txt);
    CopyTextStylePaint(txt, skStyle);
    return skStyle;
}

skt::TextStyle ParagraphBuilderImpl::ConvertTextStyleToSkStyle(const TextStyle& txt)
{
    skt::TextStyle skStyle;

    skStyle.setColor(txt.color);
    skStyle.setDecoration(static_cast<skt::TextDecoration>(txt.decoration));
    skStyle.setDecorationColor(txt.decorationColor);
    skStyle.setDecorationStyle(static_cast<skt::TextDecorationStyle>(txt.decorationStyle));
    skStyle.setDecorationThicknessMultiplier(SkDoubleToScalar(txt.decorationThicknessMultiplier));
    skStyle.setFontStyle(TextFontUtils::MakeFontStyle(txt.fontWeight, txt.fontWidth, txt.fontStyle));
    skStyle.setTextBaseline(static_cast<skt::TextBaseline>(txt.baseline));

    std::vector<SkString> fonts;
    std::transform(txt.fontFamilies.begin(), txt.fontFamilies.end(), std::back_inserter(fonts),
        [](const std::string& f) { return SkString(f.c_str()); });
    skStyle.setFontFamilies(fonts);

    skStyle.setFontSize(SkDoubleToScalar(txt.fontSize));
    skStyle.setLetterSpacing(SkDoubleToScalar(txt.letterSpacing));
    skStyle.setWordSpacing(SkDoubleToScalar(txt.wordSpacing));
    skStyle.setHeight(SkDoubleToScalar(txt.height));
    skStyle.setHeightOverride(txt.heightOverride);
    skStyle.setHalfLeading(txt.halfLeading);
    skStyle.setBaselineShift(txt.baseLineShift);

    skStyle.setLocale(SkString(txt.locale.empty() ? DefaultLocale() : txt.locale.c_str()));
    skStyle.setStyleId(txt.styleId);
    skStyle.setTextStyleUid(txt.textStyleUid);
    skStyle.setBackgroundRect({ txt.backgroundRect.color, txt.backgroundRect.leftTopRadius,
        txt.backgroundRect.rightTopRadius, txt.backgroundRect.rightBottomRadius,
        txt.backgroundRect.leftBottomRadius });

    skStyle.resetFontFeatures();
    for (const auto& ff : txt.fontFeatures.GetFontFeatures()) {
        skStyle.addFontFeature(SkString(ff.first.c_str()), ff.second);
    }

    if (!txt.fontVariations.GetAxisValues().empty()) {
        TextFontUtils::MakeFontArguments(skStyle, txt.fontVariations);
    }

    skStyle.resetShadows();
    for (const TextShadow& txtShadow : txt.textShadows) {
        skStyle.addShadow(TextFontUtils::MakeTextShadow(txtShadow));
    }

    if (txt.isPlaceholder) {
        skStyle.setPlaceholder();
    }

    if (txt.symbol.GetSymbolType() == SymbolType::CUSTOM) {
        skStyle.setCustomSymbol(true);
    }

    return skStyle;
}

void ParagraphBuilderImpl::CopyTextStylePaint(const TextStyle& txt, skia::textlayout::TextStyle& skStyle)
{
    if (txt.background.has_value()) {
        skStyle.setBackgroundPaintID(AllocPaintID(txt.background.value()));
    }
    if (txt.foreground.has_value()) {
        skStyle.setForegroundPaintID(AllocPaintID(txt.foreground.value()));
    } else {
        PaintRecord paint;
        paint.SetColor(txt.color);
        paint.isSymbolGlyph = txt.isSymbolGlyph;
        paint.symbol.familyName_ = txt.fontFamilies.empty() ? "" : txt.fontFamilies[0];
        paint.symbol.SetRenderColor(txt.symbol.GetRenderColor());
        paint.symbol.SetRenderMode(txt.symbol.GetRenderMode());
        paint.symbol.SetSymbolEffect(txt.symbol.GetEffectStrategy());
        paint.symbol.SetAnimationMode(txt.symbol.GetAnimationMode());
        paint.symbol.SetRepeatCount(txt.symbol.GetRepeatCount());
        paint.symbol.SetAnimationStart(txt.symbol.GetAnimationStart());
        paint.symbol.SetCommonSubType(txt.symbol.GetCommonSubType());
        paint.symbol.SetSymbolType(txt.symbol.GetSymbolType());
        paint.symbol.SetSymbolUid(txt.symbol.GetSymbolUid());
        skStyle.setForegroundPaintID(AllocPaintID(paint));
    }
}

void ParagraphBuilderImpl::RecordDifferentPthreadCall(const char* caller) const
{
    pthread_t currenetThreadId = pthread_self();
    if (threadId_ != currenetThreadId) {
        TEXT_LOGE_LIMIT3_HOUR("New pthread access paragraph builder, old %{public}lu, caller %{public}s",
            threadId_, caller);
        threadId_ = currenetThreadId;
    }
}
} // namespace SPText
} // namespace Rosen
} // namespace OHOS
