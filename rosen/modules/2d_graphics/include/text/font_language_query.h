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
    UR_BASIC_LATIN,                             // bit 0
    UR_LATIN_1_SUPPLEMENT,                      // bit 1
    UR_LATIN_EXTENDED_A,                        // bit 2
    UR_LATIN_EXTENDED_B,                        // bit 3
    UR_IPA_EXTENSIONS,                          // bit 4
    UR_SPACING_MODIFIER_LETTERS,                // bit 5
    UR_COMBINING_DIACRITICAL_MARKS,             // bit 6
    UR_GREEK_AND_COPTIC,                        // bit 7
    UR_COPTIC,                                  // bit 8
    UR_CYRILLIC,                                // bit 9
    UR_ARMENIAN,                                // bit 10
    UR_HEBREW,                                  // bit 11
    UR_VAI,                                     // bit 12
    UR_ARABIC,                                  // bit 13
    UR_NKO,                                     // bit 14
    UR_SAMARITAN,
    UR_MANDAIC,
    UR_DEVANAGARI,                              // bit 15
    UR_BANGLA,                                  // bit 16
    UR_GURMUKHI,                                // bit 17
    UR_GUJARATI,                                // bit 18
    UR_ODIA,                                    // bit 19
    UR_TAMIL,                                   // bit 20
    UR_TELUGU,                                  // bit 21
    UR_KANNADA,                                 // bit 22
    UR_MALAYALAM,                               // bit 23
    UR_THAI,                                    // bit 24
    UR_LAO,                                     // bit 25
    UR_GEORGIAN,                                // bit 26
    UR_BALINESE,                                // bit 27
    UR_HANGUL_JAMO,                             // bit 28
    UR_LATIN_EXTENDED_ADDITIONAL,               // bit 29
    UR_GREEK_EXTENDED,                          // bit 30
    UR_GENERAL_PUNCTUATION,                     // bit 31
    // ulUnicodeRange2 (bits 32-63)
    UR_SUPERSCRIPTS_AND_SUBSCRIPTS,             // bit 32
    UR_CURRENCY_SYMBOLS,                        // bit 33
    UR_COMBINING_DIACRITICAL_MARKS_FOR_SYMBOLS, // bit 34
    UR_LETTERLIKE_SYMBOLS,                      // bit 35
    UR_NUMBER_FORMS,                            // bit 36
    UR_ARROWS,                                  // bit 37
    UR_MATHEMATICAL_OPERATORS,                  // bit 38
    UR_MISCELLANEOUS_TECHNICAL,                 // bit 39
    UR_CONTROL_PICTURES,                        // bit 40
    UR_OPTICAL_CHARACTER_RECOGNITION,           // bit 41
    UR_ENCLOSED_ALPHANUMERICS,                  // bit 42
    UR_BOX_DRAWING,                             // bit 43
    UR_BLOCK_ELEMENTS,                          // bit 44
    UR_GEOMETRIC_SHAPES,                        // bit 45
    UR_MISCELLANEOUS_SYMBOLS,                   // bit 46
    UR_DINGBATS,                                // bit 47
    UR_CJK_SYMBOLS_AND_PUNCTUATION,             // bit 48
    UR_HIRAGANA,                                // bit 49
    UR_KATAKANA,                                // bit 50
    UR_BOPOMOFO,                                // bit 51
    UR_HANGUL_COMPATIBILITY_JAMO,               // bit 52
    UR_PHAGS_PA,                                // bit 53
    UR_ENCLOSED_CJK_LETTERS_AND_MONTHS,         // bit 54
    UR_CJK_COMPATIBILITY,                       // bit 55
    UR_HANGUL_SYLLABLES,                        // bit 56
    UR_NON_PLANE_0,                             // bit 57
    UR_PHOENICIAN,                              // bit 58
    UR_CJK_UNIFIED_IDEOGRAPHS,                  // bit 59
    UR_PRIVATE_USE_AREA_PLANE_0,                // bit 60
    UR_CJK_STROKES,                             // bit 61
    UR_ALPHABETIC_PRESENTATION_FORMS,           // bit 62
    UR_ARABIC_PRESENTATION_FORMS_A,             // bit 63
    // ulUnicodeRange3 (bits 64-95)
    UR_COMBINING_HALF_MARKS,                    // bit 64
    UR_VERTICAL_FORMS,                          // bit 65
    UR_SMALL_FORM_VARIANTS,                     // bit 66
    UR_ARABIC_PRESENTATION_FORMS_B,             // bit 67
    UR_HALFWIDTH_AND_FULLWIDTH_FORMS,           // bit 68
    UR_SPECIALS,                                // bit 69
    UR_TIBETAN,                                 // bit 70
    UR_SYRIAC,                                  // bit 71
    UR_THAANA,                                  // bit 72
    UR_SINHALA,                                 // bit 73
    UR_MYANMAR,                                 // bit 74
    UR_ETHIOPIC,                                // bit 75
    UR_ETHIOPIC_SUPPLEMENT,
    UR_ETHIOPIC_EXTENDED,
    UR_CHEROKEE,                                // bit 76
    UR_UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS,   // bit 77
    UR_OGHAM,                                   // bit 78
    UR_RUNIC,                                   // bit 79
    UR_KHMER,                                   // bit 80
    UR_MONGOLIAN,                               // bit 81
    UR_BRAILLE_PATTERNS,                        // bit 82
    UR_YI_SYLLABLES,                            // bit 83
    UR_TAGALOG,                                 // bit 84
    UR_HANUNOO,
    UR_BUHID,
    UR_TABGANWA,
    UR_OLD_ITALIC,                              // bit 85
    UR_GOTHIC,                                  // bit 86
    UR_DESERET,                                 // bit 87
    UR_BYZANTINE_MUSICAL_SYMBOLS,               // bit 88
    UR_MATHEMATICAL_ALPHANUMERIC_SYMBOLS,       // bit 89
    UR_PRIVATE_USE_AREA_PLANE_15,               // bit 90
    UR_VARIATION_SELECTORS,                     // bit 91
    UR_TAGS,                                    // bit 92
    UR_LIMBU,                                   // bit 93
    UR_TAI_LE,                                  // bit 94
    UR_NEW_TAI_LUE,                             // bit 95
    // ulUnicodeRange4 (bits 96-127)
    UR_BUGINESE,                                // bit 96
    UR_GLAGOLITIC,                              // bit 97
    UR_TIFINAGH,                                // bit 98
    UR_YIJING_HEXAGRAM_SYMBOLS,                 // bit 99
    UR_SYLOTI_NAGRI,                            // bit 100
    UR_LINEAR_B_SYLLABARY,                      // bit 101
    UR_ANCIENT_GREEK_NUMBERS,                   // bit 102
    UR_UGARITIC,                                // bit 103
    UR_OLD_PERSIAN,                             // bit 104
    UR_SHAVIAN,                                 // bit 105
    UR_OSMANYA,                                 // bit 106
    UR_CYPRIOT_SYLLABARY,                       // bit 107
    UR_KHAROSHTHI,                              // bit 108
    UR_TAI_XUAN_JING_SYMBOLS,                   // bit 109
    UR_CUNEIFORM,                               // bit 110
    UR_COUNTING_ROD_NUMERALS,                   // bit 111
    UR_SUNDANESE,                               // bit 112
    UR_LEPCHA,                                  // bit 113
    UR_OL_CHIKI,                                // bit 114
    UR_SAURASHTRA,                              // bit 115
    UR_KAYAH_LI,                                // bit 116
    UR_REJANG,                                  // bit 117
    UR_CHAM,                                    // bit 118
    UR_ANCIENT_SYMBOLS,                         // bit 119
    UR_PHAISTOS_DISC,                           // bit 120
    UR_CARIAN,                                  // bit 121
    UR_LYCIAN,
    UR_LYDIAN,
    UR_DOMINO_TILES_AND_MAHJONG_TILES,          // bit 122
    UR_RESERVED_123,  // Reserved
    UR_RESERVED_124,  // Reserved
    UR_RESERVED_125,  // Reserved
    UR_RESERVED_126,  // Reserved
    UR_RESERVED_127   // Reserved
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