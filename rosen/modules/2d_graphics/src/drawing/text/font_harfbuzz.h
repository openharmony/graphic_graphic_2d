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

#ifndef FONT_HARFBUZZ_H
#define FONT_HARFBUZZ_H

#include <hb.h>
#include <hb-ot.h>

#include "modules/skshaper/include/SkShaper.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class FontHarfbuzz {
public:
#ifdef USE_M133_SKIA
    using HBFont = std::unique_ptr<hb_font_t, SkFunctionObject<hb_font_destroy>>;
    using HBFace = std::unique_ptr<hb_face_t, SkFunctionObject<hb_face_destroy>>;
    using HBBuffer = std::unique_ptr<hb_buffer_t, SkFunctionObject<hb_buffer_destroy>>;
#else
    template <typename T, typename P, P* p> using resource = std::unique_ptr<T, SkFunctionWrapper<P, p>>;
    using HBFont = resource<hb_font_t, decltype(hb_font_destroy), hb_font_destroy>;
    using HBFace = resource<hb_face_t, decltype(hb_face_destroy), hb_face_destroy>;
    using HBBuffer = resource<hb_buffer_t, decltype(hb_buffer_destroy), hb_buffer_destroy>;
#endif

    static hb_bool_t SetGlyph(hb_font_t* hbFont, void* fontData, hb_codepoint_t unicode,
        hb_codepoint_t variation_selector, hb_codepoint_t* glyph, void* userData);
    static hb_bool_t SetNominalGlyph(hb_font_t* hbFont, void* fontData, hb_codepoint_t unicode, hb_codepoint_t* glyph,
        void* userData);
    static unsigned SetNominalGlyphs(hb_font_t *hbFont, void *fontData, unsigned int count,
        const hb_codepoint_t *unicodes, unsigned int unicodeStride, hb_codepoint_t *glyphs, unsigned int glyphStride,
        void *userData);
    static hb_position_t SetGlyphAdvance(hb_font_t* hbFont, void* fontData, hb_codepoint_t hbGlyph, void* userData);
    static hb_bool_t GetGlyphExtents(hb_font_t* hbFont, void* fontData, hb_codepoint_t hbGlyph,
        hb_glyph_extents_t* extents, void* userData);
    static void SetGlyphAdvances(hb_font_t* hbFont, void* fontData, unsigned count, const hb_codepoint_t* glyphs,
        unsigned int glyphStride, hb_position_t* advances, unsigned int advanceStride, void* userData);
    static HBFont CreateTypefaceHbFont(const Typeface& typeface);
    static HBFont CreateSubHbFont(const Font& font, const HBFont& typefaceFont);
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif