/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef FONT_LANGUAGE_QUERY_H
#define FONT_LANGUAGE_QUERY_H

#include <string>
#include <vector>

#include "text/font_style.h"
#include "typeface.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
enum class UnicodeRangeId : uint32_t {
    // ulUnicodeRange1 (bits 0-31)
    UR_BASIC_LATIN = 0,
    UR_LATIN_1_SUPPLEMENT = 1,
    UR_LATIN_EXTENDED_A = 2,
    UR_LATIN_EXTENDED_B = 3,
    UR_IPA_EXTENSIONS = 4,
    UR_SPACING_MODIFIER_LETTERS = 5,
    UR_COMBINING_DIACRITICAL_MARKS = 6,
    UR_GREEK_AND_COPTIC = 7,
    UR_COPTIC = 8,
    UR_CYRILLIC = 9,
    UR_ARMENIAN = 10,
    UR_HEBREW = 11,
    UR_VAI = 12,
    UR_ARABIC = 13,
    UR_NKO = 14,
    UR_DEVANAGARI = 15,
    UR_BANGLA = 16,
    UR_GURMUKHI = 17,
    UR_GUJARATI = 18,
    UR_ODIA = 19,
    UR_TAMIL = 20,
    UR_TELUGU = 21,
    UR_KANNADA = 22,
    UR_MALAYALAM = 23,
    UR_THAI = 24,
    UR_LAO = 25,
    UR_GEORGIAN = 26,
    UR_BALINESE = 27,
    UR_HANGUL_JAMO = 28,
    UR_LATIN_EXTENDED_ADDITIONAL = 29,
    UR_GREEK_EXTENDED = 30,
    UR_GENERAL_PUNCTUATION = 31,
    // ulUnicodeRange2 (bits 32-63)
    UR_SUPERSCRIPTS_AND_SUBSCRIPTS = 32,
    UR_CURRENCY_SYMBOLS = 33,
    UR_COMBINING_DIACRITICAL_MARKS_FOR_SYMBOLS = 34,
    UR_LETTERLIKE_SYMBOLS = 35,
    UR_NUMBER_FORMS = 36,
    UR_ARROWS = 37,
    UR_MATHEMATICAL_OPERATORS = 38,
    UR_MISCELLANEOUS_TECHNICAL = 39,
    UR_CONTROL_PICTURES = 40,
    UR_OPTICAL_CHARACTER_RECOGNITION = 41,
    UR_ENCLOSED_ALPHANUMERICS = 42,
    UR_BOX_DRAWING = 43,
    UR_BLOCK_ELEMENTS = 44,
    UR_GEOMETRIC_SHAPES = 45,
    UR_MISCELLANEOUS_SYMBOLS = 46,
    UR_DINGBATS = 47,
    UR_CJK_SYMBOLS_AND_PUNCTUATION = 48,
    UR_HIRAGANA = 49,
    UR_KATAKANA = 50,
    UR_BOPOMOFO = 51,
    UR_HANGUL_COMPATIBILITY_JAMO = 52,
    UR_PHAGS_PA = 53,
    UR_ENCLOSED_CJK_LETTERS_AND_MONTHS = 54,
    UR_CJK_COMPATIBILITY = 55,
    UR_HANGUL_SYLLABLES = 56,
    UR_NON_PLANE_0 = 57,
    UR_PHOENICIAN = 58,
    UR_CJK_UNIFIED_IDEOGRAPHS = 59,
    UR_PRIVATE_USE_AREA_PLANE_0 = 60,
    UR_CJK_STROKES = 61,
    UR_ALPHABETIC_PRESENTATION_FORMS = 62,
    UR_ARABIC_PRESENTATION_FORMS_A = 63,
    // ulUnicodeRange3 (bits 64-95)
    UR_COMBINING_HALF_MARKS = 64,
    UR_VERTICAL_FORMS = 65,
    UR_SMALL_FORM_VARIANTS = 66,
    UR_ARABIC_PRESENTATION_FORMS_B = 67,
    UR_HALFWIDTH_AND_FULLWIDTH_FORMS = 68,
    UR_SPECIALS = 69,
    UR_TIBETAN = 70,
    UR_SYRIAC = 71,
    UR_THAANA = 72,
    UR_SINHALA = 73,
    UR_MYANMAR = 74,
    UR_ETHIOPIC = 75,
    UR_CHEROKEE = 76,
    UR_UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS = 77,
    UR_OGHAM = 78,
    UR_RUNIC = 79,
    UR_KHMER = 80,
    UR_MONGOLIAN = 81,
    UR_BRAILLE_PATTERNS = 82,
    UR_YI_SYLLABLES = 83,
    UR_TAGALOG = 84,
    UR_OLD_ITALIC = 85,
    UR_GOTHIC = 86,
    UR_DESERET = 87,
    UR_BYZANTINE_MUSICAL_SYMBOLS = 88,
    UR_MATHEMATICAL_ALPHANUMERIC_SYMBOLS = 89,
    UR_PRIVATE_USE_AREA_PLANE_15 = 90,
    UR_VARIATION_SELECTORS = 91,
    UR_TAGS = 92,
    UR_LIMBU = 93,
    UR_TAI_LE = 94,
    UR_NEW_TAI_LUE = 95,
    // ulUnicodeRange4 (bits 96-127)
    UR_BUGINESE = 96,
    UR_GLAGOLITIC = 97,
    UR_TIFINAGH = 98,
    UR_YIJING_HEXAGRAM_SYMBOLS = 99,
    UR_SYLOTI_NAGRI = 100,
    UR_LINEAR_B_SYLLABARY = 101,
    UR_ANCIENT_GREEK_NUMBERS = 102,
    UR_UGARITIC = 103,
    UR_OLD_PERSIAN = 104,
    UR_SHAVIAN = 105,
    UR_OSMANYA = 106,
    UR_CYPRIOT_SYLLABARY = 107,
    UR_KHAROSHTHI = 108,
    UR_TAI_XUAN_JING_SYMBOLS = 109,
    UR_CUNEIFORM = 110,
    UR_COUNTING_ROD_NUMERALS = 111,
    UR_SUNDANESE = 112,
    UR_LEPCHA = 113,
    UR_OL_CHIKI = 114,
    UR_SAURASHTRA = 115,
    UR_KAYAH_LI = 116,
    UR_REJANG = 117,
    UR_CHAM = 118,
    UR_ANCIENT_SYMBOLS = 119,
    UR_PHAISTOS_DISC = 120,
    UR_CARIAN = 121,
    UR_DOMINO_TILES_AND_MAHJONG_TILES = 122,
    UR_RESERVED_123 = 123,  // Reserved
    UR_RESERVED_124 = 124,  // Reserved
    UR_RESERVED_125 = 125,  // Reserved
    UR_RESERVED_126 = 126,  // Reserved
    UR_RESERVED_127 = 127   // Reserved
};

class DRAWING_API FontLanguageQuery {
public:
    /**
     * @brief            Returns all supported and valid font languages in given typeface.
     * @param typeface   given typeface.
     * @return           vector of all supported and valid font languages in given typeface
     */
    static std::vector<std::string> GenerateFontSupportedLanguages(const std::shared_ptr<Typeface>& typeface);

    /**
     * @brief            Returns all supported and valid font languages in given fontpath.
     * @param fontPath   given fontPath.
     * @return           vector of all supported and valid font languages in given fontpath
     */
    static std::vector<std::string> GenerateFontSupportedLanguages(const std::string& fontPath);

    /**
     * @brief            Returns all supported and valid font languages in given fontData.
     * @param fontData   given fontData.
     * @return           vector of all supported and valid font languages in given fontData
     */
    static std::vector<std::string> GenerateFontSupportedLanguages(const std::vector<uint8_t>& fontData);
};
} // Drawing
} // Rosen
} // OHOS
#endif // FONT_LANGUAGE_QUERY_H