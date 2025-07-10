/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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

#include "font_harfbuzz.h"

#include "text/font.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
using HBFace = FontHarfbuzz::HBFace;
using HBFont = FontHarfbuzz::HBFont;

constexpr size_t COUNT_REQUESTED = 256;
#ifdef USE_M133_SKIA
using SkUnicodeArray = skia_private::AutoSTMalloc<COUNT_REQUESTED, SkUnichar>;
using SkGlyphArray = skia_private::AutoSTMalloc<COUNT_REQUESTED, SkGlyphID>;
using SkScalarArray = skia_private::AutoSTMalloc<COUNT_REQUESTED, SkScalar>;
#else
using SkUnicodeArray = SkAutoSTMalloc<COUNT_REQUESTED, SkUnichar>;
using SkGlyphArray = SkAutoSTMalloc<COUNT_REQUESTED, SkGlyphID>;
using SkScalarArray = SkAutoSTMalloc<COUNT_REQUESTED, SkScalar>;
#endif

hb_bool_t FontHarfbuzz::SetGlyph(hb_font_t* hbFont, void* fontData, hb_codepoint_t unicode,
    hb_codepoint_t variation_selector, hb_codepoint_t* glyph, void* userData)
{
    Font& font = *reinterpret_cast<Font*>(fontData);
    *glyph = font.UnicharToGlyph(unicode);
    return *glyph != 0;
}

hb_bool_t FontHarfbuzz::SetNominalGlyph(hb_font_t* hbFont, void* fontData, hb_codepoint_t unicode,
    hb_codepoint_t* glyph, void* userData)
{
    return SetGlyph(hbFont, fontData, unicode, 0, glyph, userData);
}

unsigned FontHarfbuzz::SetNominalGlyphs(hb_font_t *hbFont, void *fontData, unsigned int count,
    const hb_codepoint_t *unicodes, unsigned int unicodeStride, hb_codepoint_t *glyphs, unsigned int glyphStride,
    void *userData)
{
    Font& font = *reinterpret_cast<Font*>(fontData);
    SkUnicodeArray unicode(count);
    for (unsigned i = 0; i < count; i++) {
        unicode[i] = *unicodes;
        unicodes = SkTAddOffset<const hb_codepoint_t>(unicodes, unicodeStride);
    }
    SkGlyphArray glyph(count);
    font.TextToGlyphs(unicode.get(), count * sizeof(SkUnichar), TextEncoding::UTF32, glyph.get(), count);

    // Copy the results back to array.
    unsigned done = 0;
    for (; done < count && glyph[done] != 0; done++) {
        *glyphs = glyph[done];
        glyphs = SkTAddOffset<hb_codepoint_t>(glyphs, glyphStride);
    }
    return done;
}

hb_position_t GetPosition(SkScalar value)
{
    // Treat hb_position_t as 16.16 fixed-point.
    constexpr int hbPosition1 = 1 << 16;
    return SkScalarRoundToInt(value * hbPosition1);
}

hb_position_t FontHarfbuzz::SetGlyphAdvance(hb_font_t* hbFont, void* fontData, hb_codepoint_t hbGlyph, void* userData)
{
    Font& font = *reinterpret_cast<Font*>(fontData);

    SkScalar advance;
    SkGlyphID skGlyph = SkTo<SkGlyphID>(hbGlyph);

    font.GetWidths(&skGlyph, 1, &advance);
    if (!font.IsSubpixel()) {
        advance = SkScalarRoundToInt(advance);
    }
    return GetPosition(advance);
}

hb_bool_t FontHarfbuzz::GetGlyphExtents(hb_font_t* hbFont, void* fontData, hb_codepoint_t hbGlyph,
    hb_glyph_extents_t* extents, void* userData)
{
    Font& font = *reinterpret_cast<Font*>(fontData);

    Rect bounds;
    SkGlyphID skGlyph = SkTo<SkGlyphID>(hbGlyph);
    font.GetWidths(&skGlyph, 1, nullptr, &bounds);
    if (!font.IsSubpixel()) {
        bounds = Rect(bounds.RoundOut());
    }

    // HarfBuzz is y-up but Skia is y-down.
    extents->x_bearing = GetPosition(bounds.left_);
    extents->y_bearing = GetPosition(-bounds.top_);
    extents->width = GetPosition(bounds.GetWidth());
    extents->height = GetPosition(-bounds.GetHeight());
    return true;
}

void FontHarfbuzz::SetGlyphAdvances(hb_font_t* hbFont, void* fontData, unsigned count, const hb_codepoint_t* glyphs,
    unsigned int glyphStride, hb_position_t* advances, unsigned int advanceStride, void* userData)
{
    Font& font = *reinterpret_cast<Font*>(fontData);

    SkGlyphArray glyph(count);
    for (unsigned i = 0; i < count; i++) {
        glyph[i] = *glyphs;
        glyphs = SkTAddOffset<const hb_codepoint_t>(glyphs, glyphStride);
    }
    SkScalarArray advance(count);
    font.GetWidths(glyph.get(), count, advance.get());
    if (!font.IsSubpixel()) {
        for (unsigned i = 0; i < count; i++) {
            advance[i] = SkScalarRoundToInt(advance[i]);
        }
    }
    // Copy the results back to array.
    for (unsigned i = 0; i < count; i++) {
        *advances = GetPosition(advance[i]);
        advances = SkTAddOffset<hb_position_t>(advances, advanceStride);
    }
}

// Extract OpenType/TrueType font table data from the font system and provide it to HarfBuzz in a compatible format.
hb_blob_t* GetTable(hb_face_t* face, hb_tag_t tag, void* userData)
{
    Typeface& typeface = *reinterpret_cast<Typeface*>(userData);

    auto size = typeface.GetTableSize(tag);
    if (!size) {
        return nullptr;
    }
    auto data = std::make_unique<char[]>(size);
    if (!data) {
        return nullptr;
    }
    auto relTableSize = typeface.GetTableData(tag, 0, size, data.get());
    if (relTableSize != size) {
        return nullptr;
    }

    auto rawData = data.release();
    return hb_blob_create(rawData, size,
                          HB_MEMORY_MODE_READONLY, rawData, [](void* ctx) {
                              std::unique_ptr<char[]>((char*)ctx);
                          });
}

HBFace CreateHbFace(const Typeface& typeface)
{
    int index = 0;
    HBFace face;
    face.reset(hb_face_create_for_tables(
        GetTable,
        const_cast<Typeface*>(std::make_unique<Typeface>(typeface).release()),
        [](void* userData) { std::unique_ptr<Typeface>(reinterpret_cast<Typeface*>(userData)); }));
    hb_face_set_index(face.get(), (unsigned)index);
    if (!face) {
        return nullptr;
    }
    hb_face_set_upem(face.get(), typeface.GetUnitsPerEm());
    return face;
}

HBFont FontHarfbuzz::CreateTypefaceHbFont(const Typeface& typeface)
{
    HBFace face(CreateHbFace(typeface));
    if (!face) {
        return nullptr;
    }

    HBFont otFont(hb_font_create(face.get()));
    if (!otFont) {
        return nullptr;
    }
    hb_ot_font_set_funcs(otFont.get());

    return otFont;
}

#define SK_HB_VERSION_CHECK(x, y, z) \
    (HB_VERSION_MAJOR >  (x)) || \
    (HB_VERSION_MAJOR == (x) && HB_VERSION_MINOR >  (y)) || \
    (HB_VERSION_MAJOR == (x) && HB_VERSION_MINOR == (y) && HB_VERSION_MICRO >= (z))

hb_font_funcs_t* GetFontFuncs()
{
    static hb_font_funcs_t* const funcs = [] {
        // HarfBuzz will use the default (parent) implementation if they aren't set.
        hb_font_funcs_t* const funcs = hb_font_funcs_create();
        hb_font_funcs_set_variation_glyph_func(funcs, FontHarfbuzz::SetGlyph, nullptr, nullptr);
        hb_font_funcs_set_nominal_glyph_func(funcs, FontHarfbuzz::SetNominalGlyph, nullptr, nullptr);
#if SK_HB_VERSION_CHECK(2, 0, 0)
        hb_font_funcs_set_nominal_glyphs_func(funcs, FontHarfbuzz::SetNominalGlyphs, nullptr, nullptr);
#else
        sk_ignore_unused_variable(FontHarfbuzz::SetNominalGlyphs);
#endif
        hb_font_funcs_set_glyph_h_advance_func(funcs, FontHarfbuzz::SetGlyphAdvance, nullptr, nullptr);
#if SK_HB_VERSION_CHECK(1, 8, 6)
        hb_font_funcs_set_glyph_h_advances_func(funcs, FontHarfbuzz::SetGlyphAdvances, nullptr, nullptr);
#else
        sk_ignore_unused_variable(FontHarfbuzz::SetGlyphAdvances);
#endif
        hb_font_funcs_set_glyph_extents_func(funcs, FontHarfbuzz::GetGlyphExtents, nullptr, nullptr);
        hb_font_funcs_make_immutable(funcs);
        return funcs;
    }();
    return funcs;
}

HBFont FontHarfbuzz::CreateSubHbFont(const Font& font, const HBFont& typefaceFont)
{
    // Creating a sub font means that non-available functions
    // are found from the parent.
    HBFont skFont(hb_font_create_sub_font(typefaceFont.get()));
    hb_font_set_funcs(skFont.get(), GetFontFuncs(),
                      reinterpret_cast<void *>(std::make_unique<Font>(font).release()),
                      [](void* userData) { std::unique_ptr<Font>(reinterpret_cast<Font*>(userData)); });
    int scale = GetPosition(font.GetSize());
    hb_font_set_scale(skFont.get(), scale, scale);
    return skFont;
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
