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

#include "text/font_language_query.h"
#include "text/font_unicode_query.h"

#include <vector>
#include <memory>
#include <set>
#include <cstdint>
#include <unordered_map>

#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

struct UnicodeRangeInfo {
    std::vector<std::pair<uint32_t, uint32_t>> ranges;
    std::vector<std::string> languages;
};

static const std::unordered_map<UnicodeRangeId, UnicodeRangeInfo> unicodeRangeBitConfig = {
    // ulUnicodeRange1 (bits 0-31)
    {UnicodeRangeId::UR_BASIC_LATIN, {{{0x0000, 0x007F}},
        {"en", "fr", "de", "es", "pt", "it", "nl", "sv", "da", "no", "fi", "id"}}},
    {UnicodeRangeId::UR_LATIN_1_SUPPLEMENT, {{{0x0080, 0x00FF}},
        {"en", "fr", "de", "es", "pt", "it", "nl", "sv", "da", "no", "fi"}}},
    {UnicodeRangeId::UR_LATIN_EXTENDED_A, {{{0x0100, 0x017F}},
        {"cs", "pl", "ro", "hu", "sk", "hr", "sl", "lt", "lv", "et"}}},
    {UnicodeRangeId::UR_LATIN_EXTENDED_B, {{{0x0180, 0x024F}},
        {"ro", "vi", "af", "ha"}}},
    {UnicodeRangeId::UR_GREEK_AND_COPTIC, {{{0x0370, 0x03FF}},
        {"el", "grc"}}},
    {UnicodeRangeId::UR_COPTIC, {{{0x2C80, 0x2CFF}},
        {"cop"}}},
    {UnicodeRangeId::UR_CYRILLIC, {{{0x0400, 0x052F}, {0x2DE0, 0x2DFF}, {0xA640, 0xA69F}},
        {"ru", "uk", "be", "bg", "sr", "mk"}}},
    {UnicodeRangeId::UR_ARMENIAN, {{{0x0530, 0x058F}},
        {"hy", "hyw", "xcl"}}},
    {UnicodeRangeId::UR_HEBREW, {{{0x0590, 0x05FF}},
        {"he", "yi"}}},
    {UnicodeRangeId::UR_VAI, {{{0xA500, 0xA63F}},
        {"vai"}}},
    {UnicodeRangeId::UR_ARABIC, {{{0x0600, 0x06FF}, {0x0750, 0x077F}},
        {"ar", "fa", "ur", "ps", "ku", "sd", "ug", "kk-arab"}}},
    {UnicodeRangeId::UR_NKO, {{{0x07C0, 0x07FF}},
        {"nqo"}}},
    {UnicodeRangeId::UR_DEVANAGARI, {{{0x0900, 0x097F}},
        {"hi", "mr", "ne", "kok", "bh", "mai", "sa"}}},
    {UnicodeRangeId::UR_BANGLA, {{{0x0980, 0x09FF}},
        {"bn", "as"}}},
    {UnicodeRangeId::UR_GURMUKHI, {{{0x0A00, 0x0A7F}},
        {"pa"}}},
    {UnicodeRangeId::UR_GUJARATI, {{{0x0A80, 0x0AFF}},
        {"gu"}}},
    {UnicodeRangeId::UR_ODIA, {{{0x0B00, 0x0B7F}},
        {"or"}}},
    {UnicodeRangeId::UR_TAMIL, {{{0x0B80, 0x0BFF}},
        {"ta"}}},
    {UnicodeRangeId::UR_TELUGU, {{{0x0C00, 0x0C7F}},
        {"te"}}},
    {UnicodeRangeId::UR_KANNADA, {{{0x0C80, 0x0CFF}},
        {"kn"}}},
    {UnicodeRangeId::UR_MALAYALAM, {{{0x0D00, 0x0D7F}},
        {"ml"}}},
    {UnicodeRangeId::UR_THAI, {{{0x0E00, 0x0E7F}},
        {"th"}}},
    {UnicodeRangeId::UR_LAO, {{{0x0E80, 0x0EFF}},
        {"lo"}}},
    {UnicodeRangeId::UR_GEORGIAN, {{{0x10A0, 0x10FF}, {0x2D00, 0x2D2F}},
        {"ka"}}},
    {UnicodeRangeId::UR_BALINESE, {{{0x1B00, 0x1B7F}},
        {"ban-Bali"}}},
    {UnicodeRangeId::UR_HANGUL_JAMO, {{{0x1100, 0x11FF}},
        {"ko"}}},
    {UnicodeRangeId::UR_LATIN_EXTENDED_ADDITIONAL, {{{0x1E00, 0x1EFF}, {0x2C60, 0x2C7F}, {0xA720, 0xA7FF}},
        {"vi", "za", "bm", "ng"}}},
    {UnicodeRangeId::UR_GREEK_EXTENDED, {{{0x1F00, 0x1FFF}},
        {"el-polyton", "grc"}}},
    // ulUnicodeRange2 (bits 32-63)
    {UnicodeRangeId::UR_CJK_SYMBOLS_AND_PUNCTUATION, {{{0x3000, 0x303F}},
        {"zh-Hans", "zh-Hant", "ja", "ko"}}},
    {UnicodeRangeId::UR_HIRAGANA, {{{0x3040, 0x309F}},
        {"ja"}}},
    {UnicodeRangeId::UR_KATAKANA, {{{0x30A0, 0x30FF}, {0x31F0, 0x31FF}},
        {"ja", "ain"}}},
    {UnicodeRangeId::UR_BOPOMOFO, {{{0x3100, 0x312F}, {0x31A0, 0x31BF}},
        {"zh-Hant"}}},
    {UnicodeRangeId::UR_HANGUL_COMPATIBILITY_JAMO, {{{0x3130, 0x318F}},
        {"ko"}}},
    {UnicodeRangeId::UR_PHAGS_PA, {{{0xA840, 0xA87F}},
        {"zh-Phag", "mn-Phag"}}},
    {UnicodeRangeId::UR_ENCLOSED_CJK_LETTERS_AND_MONTHS, {{{0x3200, 0x32FF}},
        {"zh-Hans", "zh-Hant", "ja", "ko"}}},
    {UnicodeRangeId::UR_CJK_COMPATIBILITY, {{{0x3300, 0x33FF}},
        {"zh-Hans", "zh-Hant", "ja", "ko"}}},
    {UnicodeRangeId::UR_HANGUL_SYLLABLES, {{{0xAC00, 0xD7AF}},
        {"ko"}}},
    {UnicodeRangeId::UR_PHOENICIAN, {{{0x10900, 0x1091F}},
        {"phn"}}},
    {UnicodeRangeId::UR_CJK_UNIFIED_IDEOGRAPHS,
        {{{0x4E00, 0x9FFF}, {0x2E80, 0x2FDF}, {0x2FF0, 0x2FFF}, {0x3400, 0x4DBF}, {0x20000, 0x2A6DF}, {0x3190, 0x319F}},
         {"zh-Hans", "zh-Hant", "ja", "ko"}}},
    {UnicodeRangeId::UR_CJK_STROKES, {{{0x31C0, 0x31EF}, {0xF900, 0xFAFF}, {0x2F800, 0x2FA1F}},
        {"zh-Hans", "zh-Hant", "ja", "ko"}}},
    {UnicodeRangeId::UR_ALPHABETIC_PRESENTATION_FORMS, {{{0xFB00, 0xFB4F}},
        {"en", "he"}}},
    {UnicodeRangeId::UR_ARABIC_PRESENTATION_FORMS_A, {{{0xFB50, 0xFDFF}},
        {"ar"}}},
    // ulUnicodeRange3 (bits 64-95)
    {UnicodeRangeId::UR_VERTICAL_FORMS, {{{0xFE10, 0xFE1F}, {0xFE30, 0xFE4F}},
        {"zh-Hans", "zh-Hant", "ja", "ko"}}},
    {UnicodeRangeId::UR_SMALL_FORM_VARIANTS, {{{0xFE50, 0xFE6F}},
        {"zh-Hant"}}},
    {UnicodeRangeId::UR_ARABIC_PRESENTATION_FORMS_B, {{{0xFE70, 0xFEFF}},
        {"ar"}}},
    {UnicodeRangeId::UR_HALFWIDTH_AND_FULLWIDTH_FORMS, {{{0xFF00, 0xFFEF}},
        {"zh-Hans", "zh-Hant", "ja", "ko"}}},
    {UnicodeRangeId::UR_TIBETAN, {{{0x0F00, 0x0FFF}},
        {"bo", "dz"}}},
    {UnicodeRangeId::UR_SYRIAC, {{{0x0700, 0x074F}},
        {"syr"}}},
    {UnicodeRangeId::UR_THAANA, {{{0x0780, 0x07BF}},
        {"dv"}}},
    {UnicodeRangeId::UR_SINHALA, {{{0x0D80, 0x0DFF}},
        {"si"}}},
    {UnicodeRangeId::UR_MYANMAR, {{{0x1000, 0x109F}},
        {"my"}}},
    {UnicodeRangeId::UR_ETHIOPIC, {{{0x1200, 0x139F}, {0x2D80, 0x2DDF}},
        {"am", "ti", "om", "so", "gez"}}},
    {UnicodeRangeId::UR_CHEROKEE, {{{0x13A0, 0x13FF}},
        {"chr-Cher"}}},
    {UnicodeRangeId::UR_UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS, {{{0x1400, 0x167F}},
        {"iu", "cr", "oj"}}},
    {UnicodeRangeId::UR_OGHAM, {{{0x1680, 0x169F}},
        {"sga-Ogam"}}},
    {UnicodeRangeId::UR_RUNIC, {{{0x16A0, 0x16FF}},
        {"non", "ang"}}},
    {UnicodeRangeId::UR_KHMER, {{{0x1780, 0x17FF}, {0x19E0, 0x19FF}},
        {"km"}}},
    {UnicodeRangeId::UR_MONGOLIAN, {{{0x1800, 0x18AF}},
        {"mn", "bo"}}},
    {UnicodeRangeId::UR_YI_SYLLABLES, {{{0xA000, 0xA4CF}},
        {"ii"}}},
    {UnicodeRangeId::UR_TAGALOG, {{{0x1700, 0x177F}},
        {"tl", "hnn", "bku", "tbw"}}},
    {UnicodeRangeId::UR_OLD_ITALIC, {{{0x10300, 0x1032F}},
        {"ita"}}},
    {UnicodeRangeId::UR_GOTHIC, {{{0x10330, 0x1034F}},
        {"got"}}},
    {UnicodeRangeId::UR_DESERET, {{{0x10400, 0x1044F}},
        {"en-Dsrt"}}},
    {UnicodeRangeId::UR_LIMBU, {{{0x1900, 0x194F}},
        {"lif-Limb"}}},
    {UnicodeRangeId::UR_TAI_LE, {{{0x1950, 0x197F}},
        {"tdd-Tale"}}},
    {UnicodeRangeId::UR_NEW_TAI_LUE, {{{0x1980, 0x19DF}},
        {"khb-Talu"}}},
    // ulUnicodeRange4 (bits 96-127)
    {UnicodeRangeId::UR_BUGINESE, {{{0x1A00, 0x1A1F}},
        {"bug-Bugi"}}},
    {UnicodeRangeId::UR_GLAGOLITIC, {{{0x2C00, 0x2C5F}},
        {"cu-Glag"}}},
    {UnicodeRangeId::UR_TIFINAGH, {{{0x2D30, 0x2D7F}},
        {"ber", "tzm"}}},
    {UnicodeRangeId::UR_YIJING_HEXAGRAM_SYMBOLS, {{{0x4DC0, 0x4DFF}},
        {"zh-Hans", "zh-Hant"}}},
    {UnicodeRangeId::UR_SYLOTI_NAGRI, {{{0xA800, 0xA82F}},
        {"syl-Sylo"}}},
    {UnicodeRangeId::UR_LINEAR_B_SYLLABARY, {{{0x10000, 0x1013F}},
        {"grc-Linb"}}},
    {UnicodeRangeId::UR_ANCIENT_GREEK_NUMBERS, {{{0x10140, 0x1018F}},
        {"grc"}}},
    {UnicodeRangeId::UR_UGARITIC, {{{0x10380, 0x1039F}},
        {"uga-Ugar"}}},
    {UnicodeRangeId::UR_OLD_PERSIAN, {{{0x103A0, 0x103DF}},
        {"peo-Xpeo"}}},
    {UnicodeRangeId::UR_SHAVIAN, {{{0x10450, 0x1047F}},
        {"en-shaw"}}},
    {UnicodeRangeId::UR_OSMANYA, {{{0x10480, 0x104AF}},
        {"som"}}},
    {UnicodeRangeId::UR_CYPRIOT_SYLLABARY, {{{0x10800, 0x1083F}},
        {"grc"}}},
    {UnicodeRangeId::UR_KHAROSHTHI, {{{0x10A00, 0x10A5F}},
        {"pra-Khar", "san-Khar"}}},
    {UnicodeRangeId::UR_TAI_XUAN_JING_SYMBOLS, {{{0x1D300, 0x1D35F}},
        {"zh-Hans", "zh-Hant"}}},
    {UnicodeRangeId::UR_CUNEIFORM, {{{0x12000, 0x1247F}},
        {"akk-Xsux", "sux-Xsux", "hit-Xsux"}}},
    {UnicodeRangeId::UR_SUNDANESE, {{{0x1B80, 0x1BBF}},
        {"su"}}},
    {UnicodeRangeId::UR_LEPCHA, {{{0x1C00, 0x1C4F}},
        {"lep"}}},
    {UnicodeRangeId::UR_OL_CHIKI, {{{0x1C50, 0x1C7F}},
        {"sat-Olck"}}},
    {UnicodeRangeId::UR_SAURASHTRA, {{{0xA880, 0xA8DF}},
        {"saz-Saur"}}},
    {UnicodeRangeId::UR_KAYAH_LI, {{{0xA900, 0xA92F}},
        {"kyu-Kali"}}},
    {UnicodeRangeId::UR_REJANG, {{{0xA930, 0xA95F}},
        {"rej-Rjng"}}},
    {UnicodeRangeId::UR_CHAM, {{{0xAA00, 0xAA5F}},
        {"cja-Cham", "cjm-Cham"}}},
    {UnicodeRangeId::UR_CARIAN, {{{0x10280, 0x102DF}, {0x10920, 0x1093F}},
        {"xcr", "xlc", "xld"}}}
};

namespace {
bool MatchesAnyRange(uint32_t unicode, const std::vector<std::pair<uint32_t, uint32_t>>& ranges)
{
    for (const auto& range : ranges) {
        if (unicode >= range.first && unicode <= range.second) {
            return true;
        }
    }
    return false;
}

bool HasMatchingUnicode(const std::vector<uint32_t>& unicodeList,
    const std::vector<std::pair<uint32_t, uint32_t>>& ranges)
{
    for (uint32_t unicode : unicodeList) {
        if (MatchesAnyRange(unicode, ranges)) {
            return true;
        }
    }
    return false;
}
}

std::vector<std::string> FontLanguageQuery::GenerateFontSupportedLanguages(const std::shared_ptr<Typeface>& typeface)
{
    if (!typeface) {
        LOGE("Drawing_Text [GenerateFontSupportedLanguages] typeface is nullptr!");
        return {};
    }

    std::vector<uint32_t> unicodeList = FontUnicodeQuery::GenerateUnicodeItem(typeface);

    std::set<std::string> languageSet;
    for (const auto& entry : unicodeRangeBitConfig) {
        if (HasMatchingUnicode(unicodeList, entry.second.ranges)) {
            languageSet.insert(entry.second.languages.begin(), entry.second.languages.end());
        }
    }
    return std::vector<std::string>(languageSet.begin(), languageSet.end());
}

std::vector<std::string> FontLanguageQuery::GenerateFontSupportedLanguages(const std::string& fontPath)
{
    if (fontPath.empty()) {
        LOGE("Drawing_Text [GenerateFontSupportedLanguages] fontPath is empty!");
        return {};
    }
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromFile(fontPath.c_str());
    return GenerateFontSupportedLanguages(typeface);
}

std::vector<std::string> FontLanguageQuery::GenerateFontSupportedLanguages(const std::vector<uint8_t>& fontData)
{
    if (fontData.empty()) {
        LOGE("Drawing_Text [GenerateFontSupportedLanguages] fontData is empty!");
        return {};
    }
    auto stream = std::make_unique<Drawing::MemoryStream>(fontData.data(), fontData.size());
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromStream(std::move(stream));
    return GenerateFontSupportedLanguages(typeface);
}
} // Drawing
} // Rosen
} // OHOS