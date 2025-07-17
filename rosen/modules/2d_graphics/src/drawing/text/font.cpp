/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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

#include "text/font.h"

#include "modules/skunicode/include/SkUnicode.h"

#include "impl_factory.h"
#include "impl_interface/font_impl.h"
#include "text/font_mgr.h"
#include "utils/log.h"
#include "font_harfbuzz.h"


namespace OHOS {
namespace Rosen {
namespace Drawing {
constexpr const char* LANGUAGE_HAN = "zh-Hans";
constexpr size_t COUNT_REQUESTED = 32;
#ifdef USE_M133_SKIA
using SkFeaturesArray = skia_private::STArray<COUNT_REQUESTED, hb_feature_t>;
#else
using SkFeaturesArray = SkSTArray<COUNT_REQUESTED, hb_feature_t>;
#endif

Font::Font() : fontImpl_(ImplFactory::CreateFontImpl()) {}

Font::Font(std::shared_ptr<Typeface> typeface, scalar size, scalar scaleX, scalar skewX)
    : fontImpl_(ImplFactory::CreateFontImpl(typeface, size, scaleX, skewX)) {}

Font::Font(const Font& font) : fontImpl_(ImplFactory::CreateFontImpl(font)) {}

void Font::SetEdging(FontEdging edging)
{
    fontImpl_->SetEdging(edging);
}

void Font::SetBaselineSnap(bool baselineSnap)
{
    fontImpl_->SetBaselineSnap(baselineSnap);
}

void Font::SetForceAutoHinting(bool isForceAutoHinting)
{
    fontImpl_->SetForceAutoHinting(isForceAutoHinting);
}

void Font::SetSubpixel(bool isSubpixel)
{
    fontImpl_->SetSubpixel(isSubpixel);
}

void Font::SetHinting(FontHinting hintingLevel)
{
    fontImpl_->SetHinting(hintingLevel);
}

void Font::SetEmbeddedBitmaps(bool embeddedBitmaps)
{
    fontImpl_->SetEmbeddedBitmaps(embeddedBitmaps);
}

void Font::SetTypeface(std::shared_ptr<Typeface> typeface)
{
    fontImpl_->SetTypeface(typeface);
}

void Font::SetSize(scalar textSize)
{
    fontImpl_->SetSize(textSize);
}

void Font::SetEmbolden(bool isEmbolden)
{
    fontImpl_->SetEmbolden(isEmbolden);
}

void Font::SetScaleX(scalar scaleX)
{
    fontImpl_->SetScaleX(scaleX);
}

void Font::SetSkewX(scalar skewX)
{
    fontImpl_->SetSkewX(skewX);
}

void Font::SetLinearMetrics(bool isLinearMetrics)
{
    fontImpl_->SetLinearMetrics(isLinearMetrics);
}

scalar Font::GetMetrics(FontMetrics* metrics) const
{
    return fontImpl_->GetMetrics(metrics);
}

void Font::GetWidths(const uint16_t glyphs[], int count, scalar widths[]) const
{
    fontImpl_->GetWidths(glyphs, count, widths);
}

void Font::GetWidths(const uint16_t glyphs[], int count, scalar widths[], Rect bounds[]) const
{
    fontImpl_->GetWidths(glyphs, count, widths, bounds);
}

scalar Font::GetSize() const
{
    return fontImpl_->GetSize();
}

std::shared_ptr<Typeface> Font::GetTypeface() const
{
    return fontImpl_->GetTypeface();
}

FontEdging Font::GetEdging() const
{
    return fontImpl_->GetEdging();
}

FontHinting Font::GetHinting() const
{
    return fontImpl_->GetHinting();
}

bool Font::IsEmbeddedBitmaps() const
{
    return fontImpl_->IsEmbeddedBitmaps();
}

scalar Font::GetScaleX() const
{
    return fontImpl_->GetScaleX();
}

scalar Font::GetSkewX() const
{
    return fontImpl_->GetSkewX();
}

bool Font::IsBaselineSnap() const
{
    return fontImpl_->IsBaselineSnap();
}

bool Font::IsForceAutoHinting() const
{
    return fontImpl_->IsForceAutoHinting();
}

bool Font::IsSubpixel() const
{
    return fontImpl_->IsSubpixel();
}

bool Font::IsLinearMetrics() const
{
    return fontImpl_->IsLinearMetrics();
}

bool Font::IsEmbolden() const
{
    return fontImpl_->IsEmbolden();
}

uint16_t Font::UnicharToGlyph(int32_t uni) const
{
    return fontImpl_->UnicharToGlyph(uni);
}

void ValidateAndCopyFontFeaturesToHbFeatures(const Drawing::DrawingFontFeatures& fontFeatures,
    SkFeaturesArray& hbFeatures)
{
    for (const auto& featureMap : fontFeatures) {
        for (const auto& [key, value] : featureMap) {
            if (key.size() != 4) { // 4 OpenType font feature name is fixed to be 4 chars.
                LOGW("Invalid feature name. font feature name has to be 4 chars");
                continue;
            }
            SkFourByteTag tag = SkSetFourByteTag(key[0], key[1], key[2], key[3]);
            hbFeatures.push_back({(hb_tag_t)tag, (uint32_t)value, HB_FEATURE_GLOBAL_START, HB_FEATURE_GLOBAL_END});
        }
    }
}

uint16_t Font::UnicharToGlyphWithFeatures(const char* uni,
    std::shared_ptr<Drawing::DrawingFontFeatures> fontFeatures) const
{
    if (fontFeatures == nullptr) {
        LOGE("font features is null, return glyphId as 0");
        return 0;
    }

    const size_t utf8Bytes = strlen(uni);
    const char* utf8Start = uni;
    const char* utf8End = utf8Start + utf8Bytes;

    FontHarfbuzz::HBBuffer buffer(hb_buffer_create());
    hb_buffer_t *buffer1 = buffer.get();
    SkAutoTCallVProc<hb_buffer_t, hb_buffer_clear_contents> autoClearBuffer(buffer1);
    hb_buffer_set_content_type(buffer1, HB_BUFFER_CONTENT_TYPE_UNICODE);
    hb_buffer_set_cluster_level(buffer1, HB_BUFFER_CLUSTER_LEVEL_MONOTONE_CHARACTERS);
    hb_buffer_add_utf8(buffer1, uni, 0, utf8Bytes, 0);

    const char* utf8Current = utf8Start;
    while (utf8Current < utf8End) {
        uint32_t cluster = utf8Current - uni;
        SkUnichar val = SkUTF::NextUTF8(&utf8Current, utf8End);
        hb_codepoint_t u = val < 0 ? 0XFFDD : static_cast<hb_codepoint_t>(val);
        hb_buffer_add(buffer1, u, cluster);
    }

    const hb_language_t language = hb_language_from_string(LANGUAGE_HAN, -1);
    hb_buffer_set_language(buffer1, language);
    hb_buffer_guess_segment_properties(buffer1);

    FontHarfbuzz::HBFont hbFont;
    {
        std::shared_ptr<Typeface> typeface = GetTypeface();
        if (!typeface) {
            LOGW("typeface is null, return glyphId as 0");
            return 0;
        }
        FontHarfbuzz::HBFont typefaceFont(FontHarfbuzz::CreateTypefaceHbFont(*typeface));
        hbFont = FontHarfbuzz::CreateSubHbFont(*this, typefaceFont);
    }

    SkFeaturesArray hbFeatures;
    ValidateAndCopyFontFeaturesToHbFeatures(*fontFeatures, hbFeatures);
    hb_shape(hbFont.get(), buffer1, hbFeatures.data(), hbFeatures.size());
    if (hb_buffer_get_length(buffer1) == 0) {
        LOGW("buffer is empty, return glyphId as 0");
        return 0;
    }

    hb_glyph_info_t* info = hb_buffer_get_glyph_infos(buffer1, nullptr);
    if (info == nullptr) {
        LOGW("glyph info generate failed, return glyphId as 0");
        return 0;
    }
    return info[0].codepoint;
}

int Font::TextToGlyphs(const void* text, size_t byteLength, TextEncoding encoding,
    uint16_t glyphs[], int maxGlyphCount) const
{
    return fontImpl_->TextToGlyphs(text, byteLength, encoding, glyphs, maxGlyphCount);
}

scalar Font::MeasureText(const void* text, size_t byteLength, TextEncoding encoding, Rect* bounds) const
{
    return fontImpl_->MeasureText(text, byteLength, encoding, bounds);
}

scalar Font::MeasureText(const void* text, size_t byteLength, TextEncoding encoding, Rect* bounds, const Brush* brush,
    const Pen* pen) const
{
    if (brush != nullptr && pen != nullptr) {
        LOGE("Font::MeasureText brush and pen are both not nullptr");
        return 0.0f;
    }
    return fontImpl_->MeasureText(text, byteLength, encoding, bounds, brush, pen);
}

void Font::GetWidthsBounds(
    const uint16_t glyphs[], int count, float widths[], Rect bounds[], const Brush* brush, const Pen* pen) const
{
    if (brush != nullptr && pen != nullptr) {
        LOGE("Font::GetWidthsBounds brush and pen are both not nullptr");
        return;
    }
    fontImpl_->GetWidthsBounds(glyphs, count, widths, bounds, brush, pen);
}

void Font::GetPos(const uint16_t glyphs[], int count, Point points[], Point origin) const
{
    fontImpl_->GetPos(glyphs, count, points, origin);
}

float Font::GetSpacing() const
{
    return fontImpl_->GetSpacing();
}

scalar Font::MeasureSingleCharacter(int32_t unicode) const
{
    scalar textWidth = 0.0f;
    uint16_t glyph = UnicharToGlyph(unicode);
    if (glyph != 0) {
        textWidth = MeasureText(&glyph, sizeof(uint16_t), TextEncoding::GLYPH_ID);
    } else {
        std::shared_ptr<Font> fallbackFont = GetFallbackFont(unicode);
        if (fallbackFont) {
            uint16_t fallbackGlyph = fallbackFont->UnicharToGlyph(unicode);
            textWidth = fallbackFont->MeasureText(&fallbackGlyph, sizeof(uint16_t), TextEncoding::GLYPH_ID);
        }
    }
    return textWidth;
}

scalar Font::MeasureSingleCharacterWithFeatures(const char* unicode, int32_t unicodeId,
    std::shared_ptr<Drawing::DrawingFontFeatures> fontFeatures) const
{
    scalar textWidth = 0.0f;
    uint16_t glyph = UnicharToGlyphWithFeatures(unicode, fontFeatures);
    if (glyph != 0) {
        textWidth = MeasureText(&glyph, sizeof(uint16_t), TextEncoding::GLYPH_ID);
    } else {
        std::shared_ptr<Font> fallbackFont = GetFallbackFont(unicodeId);
        if (fallbackFont) {
            uint16_t fallbackGlyph = fallbackFont->UnicharToGlyphWithFeatures(unicode, fontFeatures);
            textWidth = fallbackFont->MeasureText(&fallbackGlyph, sizeof(uint16_t), TextEncoding::GLYPH_ID);
        }
    }
    return textWidth;
}

std::shared_ptr<Font> Font::GetFallbackFont(int32_t unicode) const
{
    std::shared_ptr<FontMgr> fontMgr = FontMgr::CreateDefaultFontMgr();
    if (fontMgr == nullptr) {
        LOGE("Font::GetFallbackFont, default fontMgr is nullptr.");
        return nullptr;
    }
    std::shared_ptr<Typeface> currentTypeface = GetTypeface();
    std::shared_ptr<Typeface> fallbackTypeface = nullptr;
    if (currentTypeface) {
        fallbackTypeface = std::shared_ptr<Typeface>(fontMgr->MatchFamilyStyleCharacter(nullptr,
            currentTypeface->GetFontStyle(), nullptr, 0, unicode < 0 ? 0xFFFD : unicode));
    } else {
        std::shared_ptr<Typeface> defaultTypeface = Typeface::MakeDefault();
        fallbackTypeface = std::shared_ptr<Typeface>(fontMgr->MatchFamilyStyleCharacter(nullptr, defaultTypeface
            ? defaultTypeface->GetFontStyle() : FontStyle(), nullptr, 0, unicode < 0 ? 0xFFFD : unicode));
    }
    if (fallbackTypeface == nullptr) {
        LOGE("Font::GetFallbackFont, fallback typeface is nullptr.");
        return nullptr;
    }
    std::shared_ptr<Font> fallbackFont = std::make_shared<Font>(*this);
    fallbackFont->SetTypeface(fallbackTypeface);
    return fallbackFont;
}

int Font::CountText(const void* text, size_t byteLength, TextEncoding encoding) const
{
    return fontImpl_->CountText(text, byteLength, encoding);
}

bool Font::GetPathForGlyph(uint16_t glyph, Path* path) const
{
    return fontImpl_->GetPathForGlyph(glyph, path);
}

void Font::GetTextPath(const void* text, size_t byteLength, TextEncoding encoding, float x, float y, Path* path) const
{
    fontImpl_->GetTextPath(text, byteLength, encoding, x, y, path);
}

void Font::SetThemeFontFollowed(bool followed)
{
    themeFontFollowed_ = followed;
}

bool Font::IsThemeFontFollowed() const
{
    return themeFontFollowed_;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
