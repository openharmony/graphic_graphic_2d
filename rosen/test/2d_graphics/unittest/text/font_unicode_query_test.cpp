/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gtest/gtest.h"

#include <unordered_map>
#include <string>
#include <cstdint>

#include "text/font_unicode_query.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
static constexpr uint32_t UNICODE_SURROGATE_HIGH_MIN = 0xD800;
static constexpr uint32_t UNICODE_SURROGATE_LOW_MAX = 0xDFFF;
static constexpr uint32_t UNICODE_NONCHARACTER_BLOCK_START = 0xFDD0;
static constexpr uint32_t UNICODE_NONCHARACTER_BLOCK_END = 0xFDEF;
static constexpr uint32_t UNICODE_PLANE_FINAL_MASK = 0xFFFF;
static constexpr uint32_t UNICODE_PLANE_FINAL_THRESHOLD = 0xFFFE;

struct FontInfo {
    size_t size;
    uint32_t front;
    uint32_t mid;
    uint32_t back;
};

static const std::unordered_map<std::string, FontInfo> FONT_DATA = {
    {"DejaVuMathTeXGyre.ttf", {2112, 32, 10217, 120831}},
    {"FTToken.ttf", {222, 61472, 61583, 61694}},
    {"FTToken2.ttf", {217, 61472, 61581, 61689}},
    {"FTToken3.ttf", {208, 61472, 61576, 61680}},
    {"FTTokens.ttf", {223, 61472, 61584, 61695}},
    {"HMOSColorEmojiCompat.ttf", {1513, 0, 128292, 1048727}},
    {"HMOSColorEmojiFlags.ttf", {26, 127462, 127474, 127487}},
    {"HMSymbolVF.ttf", {4329, 32, 985230, 987450}},
    {"HYQiHeiL3.ttf", {60130, 32, 161155, 194988}},
    {"HarmonyOS_Sans.ttf", {874, 10, 943, 12539}},
    {"HarmonyOS_Sans_Condensed.ttf", {810, 32, 931, 12318}},
    {"HarmonyOS_Sans_Condensed_Italic.ttf", {810, 32, 931, 12318}},
    {"HarmonyOS_Sans_Digit.ttf", {13, 32, 53, 160}},
    {"HarmonyOS_Sans_Digit_Medium.ttf", {13, 32, 53, 160}},
    {"HarmonyOS_Sans_Italic.ttf", {810, 32, 931, 12318}},
    {"HarmonyOS_Sans_Naskh_Arabic.ttf", {583, 0, 64500, 65276}},
    {"HarmonyOS_Sans_Naskh_Arabic_UI.ttf", {583, 0, 64500, 65276}},
    {"HarmonyOS_Sans_Notdef.ttf", {0, 0, 0, 0}},
    {"HarmonyOS_Sans_SC.ttf", {29508, 32, 26729, 201035}},
    {"HarmonyOS_Sans_TC.ttf", {14593, 32, 29242, 65510}},
    {"NotoSansAdlam[wght].ttf", {436, 0, 304, 125279}},
    {"NotoSansAnatolianHieroglyphs-Regular.ttf", {919, 0, 83067, 83526}},
    {"NotoSansArmenian[wdth,wght].ttf", {434, 0, 303, 64279}},
    {"NotoSansAvestan-Regular.ttf", {398, 0, 275, 68415}},
    {"NotoSansBalinese[wght].ttf", {461, 0, 322, 9676}},
    {"NotoSansBamum[wght].ttf", {991, 0, 92233, 92728}},
    {"NotoSansBassaVah[wght].ttf", {372, 0, 260, 92917}},
    {"NotoSansBatak-Regular.ttf", {395, 0, 274, 9676}},
    {"NotoSansBengaliUI-Bold.ttf", {456, 0, 316, 43249}},
    {"NotoSansBengaliUI-Medium.ttf", {456, 0, 316, 43249}},
    {"NotoSansBengaliUI-Regular.ttf", {456, 0, 316, 43249}},
    {"NotoSansBengaliUI-SemiBold.ttf", {456, 0, 316, 43249}},
    {"NotoSansBengali[wdth,wght].ttf", {456, 0, 316, 43249}},
    {"NotoSansBhaiksuki-Regular.ttf", {434, 0, 303, 72812}},
    {"NotoSansBrahmi-Regular.ttf", {448, 0, 313, 69759}},
    {"NotoSansBuginese-Regular.ttf", {370, 0, 259, 43471}},
    {"NotoSansBuhid-Regular.ttf", {361, 0, 255, 9676}},
    {"NotoSansCJK-Regular.ttc", {44810, 32, 33065, 200812}},
    {"NotoSansCanadianAboriginal[wght].ttf", {1062, 0, 5339, 72383}},
    {"NotoSansCarian-Regular.ttf", {382, 0, 267, 66256}},
    {"NotoSansChakma-Regular.ttf", {429, 0, 299, 69959}},
    {"NotoSansCham[wght].ttf", {421, 0, 291, 43615}},
    {"NotoSansCherokee[wght].ttf", {511, 0, 352, 43967}},
    {"NotoSansCoptic-Regular.ttf", {523, 0, 364, 66299}},
    {"NotoSansCuneiform-Regular.ttf", {1569, 0, 74177, 75075}},
    {"NotoSansCypriot-Regular.ttf", {390, 0, 271, 67647}},
    {"NotoSansDeseret-Regular.ttf", {415, 0, 288, 66639}},
    {"NotoSansDevanagariUI-Bold.ttf", {555, 0, 379, 43263}},
    {"NotoSansDevanagariUI-Medium.ttf", {555, 0, 379, 43263}},
    {"NotoSansDevanagariUI-Regular.ttf", {555, 0, 379, 43263}},
    {"NotoSansDevanagariUI-SemiBold.ttf", {555, 0, 379, 43263}},
    {"NotoSansDevanagari[wdth,wght].ttf", {555, 0, 379, 43263}},
    {"NotoSansEgyptianHieroglyphs-Regular.ttf", {1409, 0, 78190, 78894}},
    {"NotoSansElbasan-Regular.ttf", {403, 0, 280, 66855}},
    {"NotoSansEthiopic[wdth,wght].ttf", {860, 0, 4734, 124926}},
    {"NotoSansGeorgian[wdth,wght].ttf", {512, 0, 352, 11565}},
    {"NotoSansGlagolitic-Regular.ttf", {477, 0, 331, 122922}},
    {"NotoSansGothic-Regular.ttf", {365, 0, 257, 66378}},
    {"NotoSansGrantha-Regular.ttf", {452, 0, 315, 70516}},
    {"NotoSansGujaratiUI-Bold.ttf", {447, 0, 311, 43065}},
    {"NotoSansGujaratiUI-Regular.ttf", {447, 0, 311, 43065}},
    {"NotoSansGujarati[wdth,wght].ttf", {447, 0, 311, 43065}},
    {"NotoSansGunjalaGondi[wght].ttf", {399, 0, 278, 73129}},
    {"NotoSansGurmukhiUI-Bold.ttf", {437, 0, 304, 43065}},
    {"NotoSansGurmukhiUI-Medium.ttf", {437, 0, 304, 43065}},
    {"NotoSansGurmukhiUI-Regular.ttf", {437, 0, 304, 43065}},
    {"NotoSansGurmukhiUI-SemiBold.ttf", {437, 0, 304, 43065}},
    {"NotoSansGurmukhi[wdth,wght].ttf", {437, 0, 304, 43065}},
    {"NotoSansHanifiRohingya[wght].ttf", {394, 0, 273, 68921}},
    {"NotoSansHanunoo-Regular.ttf", {362, 0, 255, 9676}},
    {"NotoSansHatran-Regular.ttf", {362, 0, 255, 67839}},
    {"NotoSansHebrew[wdth,wght].ttf", {476, 0, 330, 64335}},
    {"NotoSansImperialAramaic-Regular.ttf", {366, 0, 257, 67679}},
    {"NotoSansInscriptionalPahlavi-Regular.ttf", {362, 0, 255, 68479}},
    {"NotoSansInscriptionalParthian-Regular.ttf", {363, 0, 256, 68447}},
    {"NotoSansJavanese[wght].ttf", {430, 0, 299, 43487}},
    {"NotoSansKaithi-Regular.ttf", {427, 0, 298, 69837}},
    {"NotoSansKannadaUI-Bold.ttf", {447, 0, 311, 43061}},
    {"NotoSansKannadaUI-Medium.ttf", {447, 0, 311, 43061}},
    {"NotoSansKannadaUI-Regular.ttf", {447, 0, 311, 43061}},
    {"NotoSansKannadaUI-SemiBold.ttf", {447, 0, 311, 43061}},
    {"NotoSansKannada[wdth,wght].ttf", {447, 0, 311, 43061}},
    {"NotoSansKayahLi[wght].ttf", {387, 0, 270, 43311}},
    {"NotoSansKharoshthi-Regular.ttf", {406, 0, 281, 68184}},
    {"NotoSansKhmer[wdth,wght].ttf", {487, 0, 337, 9676}},
    {"NotoSansKhojki-Regular.ttf", {420, 0, 290, 70206}},
    {"NotoSansLao[wdth,wght].ttf", {407, 0, 282, 9676}},
    {"NotoSansLepcha-Regular.ttf", {411, 0, 286, 9676}},
    {"NotoSansLimbu-Regular.ttf", {409, 0, 283, 9676}},
    {"NotoSansLinearA-Regular.ttf", {676, 0, 67074, 67431}},
    {"NotoSansLinearB-Regular.ttf", {603, 0, 774, 65855}},
    {"NotoSansLisu[wght].ttf", {390, 0, 271, 73648}},
    {"NotoSansLycian-Regular.ttf", {364, 0, 256, 66204}},
    {"NotoSansLydian-Regular.ttf", {363, 0, 256, 67903}},
    {"NotoSansMalayalamUI-Bold.ttf", {469, 0, 325, 43058}},
    {"NotoSansMalayalamUI-Medium.ttf", {469, 0, 325, 43058}},
    {"NotoSansMalayalamUI-Regular.ttf", {469, 0, 325, 43058}},
    {"NotoSansMalayalamUI-SemiBold.ttf", {469, 0, 325, 43058}},
    {"NotoSansMalayalam[wdth,wght].ttf", {469, 0, 325, 43058}},
    {"NotoSansMandaic-Regular.ttf", {368, 0, 258, 9676}},
    {"NotoSansManichaean-Regular.ttf", {391, 0, 272, 68342}},
    {"NotoSansMarchen-Regular.ttf", {404, 0, 280, 72886}},
    {"NotoSansMasaramGondi-Regular.ttf", {413, 0, 287, 73049}},
    {"NotoSansMath-Regular.ttf", {2817, 13, 10871, 128984}},
    {"NotoSansMedefaidrin[wght].ttf", {426, 0, 295, 93850}},
    {"NotoSansMeeteiMayek[wght].ttf", {419, 0, 290, 44025}},
    {"NotoSansMeroitic-Regular.ttf", {458, 0, 318, 68095}},
    {"NotoSansMiao-Regular.ttf", {485, 0, 337, 94111}},
    {"NotoSansModi-Regular.ttf", {428, 0, 298, 71257}},
    {"NotoSansMongolian-Regular.ttf", {545, 0, 375, 71276}},
    {"NotoSansMono[wdth,wght].ttf", {3367, 0, 7840, 128639}},
    {"NotoSansMro-Regular.ttf", {378, 0, 263, 92783}},
    {"NotoSansMultani-Regular.ttf", {385, 0, 269, 70313}},
    {"NotoSansMyanmar[wdth,wght].ttf", {564, 0, 462, 65024}},
    {"NotoSansNKo-Regular.ttf", {410, 0, 283, 64831}},
    {"NotoSansNabataean-Regular.ttf", {375, 0, 262, 67759}},
    {"NotoSansNewTaiLue[wght].ttf", {437, 0, 305, 65380}},
    {"NotoSansNewa-Regular.ttf", {434, 0, 303, 70753}},
    {"NotoSansOgham-Regular.ttf", {364, 0, 256, 8722}},
    {"NotoSansOlChiki[wght].ttf", {384, 0, 268, 8722}},
    {"NotoSansOldItalic-Regular.ttf", {374, 0, 261, 66351}},
    {"NotoSansOldNorthArabian-Regular.ttf", {367, 0, 258, 68255}},
    {"NotoSansOldPermic-Regular.ttf", {382, 0, 267, 66426}},
    {"NotoSansOldPersian-Regular.ttf", {385, 0, 269, 66517}},
    {"NotoSansOldSouthArabian-Regular.ttf", {367, 0, 258, 68223}},
    {"NotoSansOldTurkic-Regular.ttf", {409, 0, 283, 68680}},
    {"NotoSansOriya[wdth,wght].ttf", {435, 0, 304, 9676}},
    {"NotoSansOsage-Regular.ttf", {409, 0, 283, 66811}},
    {"NotoSansOsmanya-Regular.ttf", {375, 0, 262, 66729}},
    {"NotoSansPahawhHmong-Regular.ttf", {465, 0, 324, 93071}},
    {"NotoSansPalmyrene-Regular.ttf", {367, 0, 258, 67711}},
    {"NotoSansPauCinHau-Regular.ttf", {392, 0, 272, 72440}},
    {"NotoSansPhags-Pa-Regular.ttf", {424, 0, 294, 65024}},
    {"NotoSansPhoenician-Regular.ttf", {364, 0, 256, 67871}},
    {"NotoSansRejang-Regular.ttf", {376, 0, 262, 43359}},
    {"NotoSansRunic-Regular.ttf", {426, 0, 295, 9676}},
    {"NotoSansSamaritan-Regular.ttf", {397, 0, 275, 11825}},
    {"NotoSansSaurashtra-Regular.ttf", {419, 0, 290, 43225}},
    {"NotoSansSharada-Regular.ttf", {435, 0, 310, 70111}},
    {"NotoSansShavian-Regular.ttf", {383, 0, 268, 66687}},
    {"NotoSansSinhalaUI-Bold.ttf", {453, 0, 315, 70132}},
    {"NotoSansSinhalaUI-Medium.ttf", {453, 0, 315, 70132}},
    {"NotoSansSinhalaUI-Regular.ttf", {453, 0, 315, 70132}},
    {"NotoSansSinhalaUI-SemiBold.ttf", {453, 0, 315, 70132}},
    {"NotoSansSinhala[wdth,wght].ttf", {453, 0, 315, 70132}},
    {"NotoSansSoraSompeng[wght].ttf", {371, 0, 260, 69881}},
    {"NotoSansSoyombo-Regular.ttf", {419, 0, 290, 72354}},
    {"NotoSansSundanese[wght].ttf", {410, 0, 283, 9676}},
    {"NotoSansSylotiNagri-Regular.ttf", {399, 0, 278, 43052}},
    {"NotoSansSymbols-Regular.ttf", {1109, 0, 9369, 128883}},
    {"NotoSansSymbols2-Regular.ttf", {2909, 0, 66012, 130041}},
    {"NotoSansSyriacEastern[wght].ttf", {456, 0, 317, 9841}},
    {"NotoSansSyriacWestern[wght].ttf", {456, 0, 317, 9841}},
    {"NotoSansTagalog-Regular.ttf", {364, 0, 256, 9676}},
    {"NotoSansTagbanwa-Regular.ttf", {359, 0, 254, 9676}},
    {"NotoSansTaiLe-Regular.ttf", {390, 0, 271, 12299}},
    {"NotoSansTaiTham[wght].ttf", {466, 0, 324, 8729}},
    {"NotoSansTaiViet-Regular.ttf", {411, 0, 286, 43743}},
    {"NotoSansTakri-Regular.ttf", {417, 0, 289, 71369}},
    {"NotoSansTamilUI-Bold.ttf", {430, 0, 294, 70460}},
    {"NotoSansTamilUI-Medium.ttf", {430, 0, 294, 70460}},
    {"NotoSansTamilUI-Regular.ttf", {430, 0, 294, 70460}},
    {"NotoSansTamilUI-SemiBold.ttf", {430, 0, 294, 70460}},
    {"NotoSansTamil[wdth,wght].ttf", {430, 0, 294, 70460}},
    {"NotoSansTeluguUI-Bold.ttf", {444, 0, 307, 9676}},
    {"NotoSansTeluguUI-Medium.ttf", {444, 0, 307, 9676}},
    {"NotoSansTeluguUI-Regular.ttf", {444, 0, 307, 9676}},
    {"NotoSansTeluguUI-SemiBold.ttf", {444, 0, 307, 9676}},
    {"NotoSansTelugu[wdth,wght].ttf", {444, 0, 307, 9676}},
    {"NotoSansThaana[wght].ttf", {409, 0, 283, 65021}},
    {"NotoSansThai[wdth,wght].ttf", {430, 0, 299, 9676}},
    {"NotoSansTifinagh-Regular.ttf", {399, 0, 278, 11647}},
    {"NotoSansUgaritic-Regular.ttf", {366, 0, 257, 66463}},
    {"NotoSansVai-Regular.ttf", {635, 0, 7838, 42539}},
    {"NotoSansWancho-Regular.ttf", {395, 0, 274, 123647}},
    {"NotoSansWarangCiti-Regular.ttf", {419, 32, 294, 71935}},
    {"NotoSansYi-Regular.ttf", {1586, 0, 41396, 65381}},
    {"NotoSans[wdth,wght].ttf", {3093, 0, 7565, 122654}},
    {"NotoSerifAhom-Regular.ttf", {394, 0, 273, 71487}},
    {"NotoSerifArmenian[wdth,wght].ttf", {434, 0, 303, 64279}},
    {"NotoSerifBengali[wdth,wght].ttf", {456, 0, 316, 43249}},
    {"NotoSerifCJK-Regular.ttc", {44777, 32, 33039, 200812}},
    {"NotoSerifDevanagari[wdth,wght].ttf", {555, 0, 379, 43263}},
    {"NotoSerifDogra-Regular.ttf", {401, 0, 279, 71739}},
    {"NotoSerifEthiopic[wdth,wght].ttf", {860, 0, 4734, 124926}},
    {"NotoSerifGeorgian[wdth,wght].ttf", {512, 0, 352, 11565}},
    {"NotoSerifGujarati[wght].ttf", {447, 0, 311, 43065}},
    {"NotoSerifGurmukhi[wght].ttf", {437, 0, 304, 43065}},
    {"NotoSerifHebrew[wdth,wght].ttf", {475, 0, 330, 64335}},
    {"NotoSerifKannada[wght].ttf", {447, 0, 311, 43061}},
    {"NotoSerifKhmer[wdth,wght].ttf", {487, 0, 337, 9676}},
    {"NotoSerifLao[wdth,wght].ttf", {407, 0, 282, 9676}},
    {"NotoSerifMalayalam[wght].ttf", {469, 0, 325, 43058}},
    {"NotoSerifMyanmar[wdth,wght].ttf", {564, 0, 462, 65024}},
    {"NotoSerifSinhala[wdth,wght].ttf", {453, 0, 315, 70132}},
    {"NotoSerifTamil[wdth,wght].ttf", {430, 0, 294, 70460}},
    {"NotoSerifTelugu[wght].ttf", {444, 0, 307, 9676}},
    {"NotoSerifThai[wdth,wght].ttf", {430, 0, 299, 9676}},
    {"NotoSerifTibetan[wght].ttf", {552, 0, 378, 9784}},
    {"NotoSerifYezidi[wght].ttf", {387, 0, 270, 69297}},
    {"NotoSerif[wdth,wght].ttf", {2841, 0, 7576, 65533}}
};

class FontUnicodeQueryTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void FontUnicodeQueryTest::SetUpTestCase() {}
void FontUnicodeQueryTest::TearDownTestCase() {}
void FontUnicodeQueryTest::SetUp() {}
void FontUnicodeQueryTest::TearDown() {}

inline std::vector<uint32_t> GenerateRange(uint32_t start, uint32_t end)
{
    std::vector<uint32_t> result(end - start + 1);
    int value = start;
    std::generate(result.begin(),
                  result.end(),
                  [&value]() { return value++; });
    return result;
}

inline bool WithinSurrogateRegion(std::vector<uint32_t>& unicodeRange)
{
    std::vector<uint32_t> surrogateRegion = GenerateRange(UNICODE_SURROGATE_HIGH_MIN,
                                                          UNICODE_SURROGATE_LOW_MAX);
    for (auto& i : surrogateRegion) {
        if (std::find(unicodeRange.begin(), unicodeRange.end(), i) != unicodeRange.end()) {
            return true;
        }
    }
    return false;    
}

inline bool WithinNonCharacterRegion(std::vector<uint32_t>& unicodeRange)
{
    std::vector<uint32_t> nonCharacterRegion = GenerateRange(UNICODE_NONCHARACTER_BLOCK_START,
                                                             UNICODE_NONCHARACTER_BLOCK_END);
    for (auto& j : nonCharacterRegion) {
        if (std::find(unicodeRange.begin(), unicodeRange.end(), j) != unicodeRange.end()) {
            return true;
        }
    }
    return false;
}

inline bool IsInvalidCharacterWithinPlane(std::vector<uint32_t>& unicodeRange)
{
    if ((std::find(unicodeRange.begin(), unicodeRange.end(), UNICODE_PLANE_FINAL_MASK) != unicodeRange.end()) ||
        (std::find(unicodeRange.begin(), unicodeRange.end(), UNICODE_PLANE_FINAL_THRESHOLD) != unicodeRange.end())) {
        return true;
    }
    return false;
}

inline bool JudgeIfInvalidCodePointExist(std::vector<uint32_t>& unicodeRange)
{
    return WithinSurrogateRegion(unicodeRange) ||
           WithinNonCharacterRegion(unicodeRange) ||
           IsInvalidCharacterWithinPlane(unicodeRange);
}

inline bool JudgeIfCodePointExist(std::vector<uint32_t>& unicodeRange, uint32_t codeToTest)
{
    if (codeToTest == 0) {
        return true;
    }

    if (std::find(unicodeRange.begin(), unicodeRange.end(), codeToTest) == unicodeRange.end()) {
        return false;
    }
    return true;
}

inline std::string GenerateRelativePathForFontResource(const std::string& ttfName)
{
    std::string prefix = "/system/fonts/";
    std::string ttfPath = prefix + ttfName;
    return ttfPath;
}

/**
 * @tc.name: GenerateUnicodeItemEmpty
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemEmpty, TestSize.Level1)
{
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(nullptr);
    EXPECT_TRUE(unicodeRange.empty());
}

/**
 * @tc.name: GenerateUnicodeItemDejaVuMathTeXGyre
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemDejaVuMathTeXGyre, TestSize.Level1)
{
    std::string ttfName = "DejaVuMathTeXGyre.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemFTToken
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemFTToken, TestSize.Level1)
{
    std::string ttfName = "FTToken.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemFTToken2
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemFTToken2, TestSize.Level1)
{
    std::string ttfName = "FTToken2.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemFTToken3
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemFTToken3, TestSize.Level1)
{
    std::string ttfName = "FTToken3.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemFTTokens
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemFTTokens, TestSize.Level1)
{
    std::string ttfName = "FTTokens.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemHMOSColorEmojiCompat
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemHMOSColorEmojiCompat, TestSize.Level1)
{
    std::string ttfName = "HMOSColorEmojiCompat.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemHMOSColorEmojiFlags
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemHMOSColorEmojiFlags, TestSize.Level1)
{
    std::string ttfName = "HMOSColorEmojiFlags.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemHMSymbolVF
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemHMSymbolVF, TestSize.Level1)
{
    std::string ttfName = "HMSymbolVF.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemHYQiHeiL3
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemHYQiHeiL3, TestSize.Level1)
{
    std::string ttfName = "HYQiHeiL3.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemHarmonyOS_Sans
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemHarmonyOS_Sans, TestSize.Level1)
{
    std::string ttfName = "HarmonyOS_Sans.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemHarmonyOS_Sans_Condensed
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemHarmonyOS_Sans_Condensed, TestSize.Level1)
{
    std::string ttfName = "HarmonyOS_Sans_Condensed.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemHarmonyOS_Sans_Condensed_Italic
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemHarmonyOS_Sans_Condensed_Italic, TestSize.Level1)
{
    std::string ttfName = "HarmonyOS_Sans_Condensed_Italic.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemHarmonyOS_Sans_Digit
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemHarmonyOS_Sans_Digit, TestSize.Level1)
{
    std::string ttfName = "HarmonyOS_Sans_Digit.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemHarmonyOS_Sans_Digit_Medium
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemHarmonyOS_Sans_Digit_Medium, TestSize.Level1)
{
    std::string ttfName = "HarmonyOS_Sans_Digit_Medium.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemHarmonyOS_Sans_Italic
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemHarmonyOS_Sans_Italic, TestSize.Level1)
{
    std::string ttfName = "HarmonyOS_Sans_Italic.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemHarmonyOS_Sans_Naskh_Arabic
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemHarmonyOS_Sans_Naskh_Arabic, TestSize.Level1)
{
    std::string ttfName = "HarmonyOS_Sans_Naskh_Arabic.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemHarmonyOS_Sans_Naskh_Arabic_UI
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemHarmonyOS_Sans_Naskh_Arabic_UI, TestSize.Level1)
{
    std::string ttfName = "HarmonyOS_Sans_Naskh_Arabic_UI.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemHarmonyOS_Sans_Notdef
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemHarmonyOS_Sans_Notdef, TestSize.Level1)
{
    std::string ttfName = "HarmonyOS_Sans_Notdef.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemHarmonyOS_Sans_SC
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemHarmonyOS_Sans_SC, TestSize.Level1)
{
    std::string ttfName = "HarmonyOS_Sans_SC.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemHarmonyOS_Sans_TC
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemHarmonyOS_Sans_TC, TestSize.Level1)
{
    std::string ttfName = "HarmonyOS_Sans_TC.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

// ... 接上面的测试用例

/**
 * @tc.name: GenerateUnicodeItemNotoSansAdlam
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansAdlam, TestSize.Level1)
{
    std::string ttfName = "NotoSansAdlam[wght].ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansAnatolianHieroglyphs
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansAnatolianHieroglyphs, TestSize.Level1)
{
    std::string ttfName = "NotoSansAnatolianHieroglyphs-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansArmenian
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansArmenian, TestSize.Level1)
{
    std::string ttfName = "NotoSansArmenian[wdth,wght].ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansAvestan
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansAvestan, TestSize.Level1)
{
    std::string ttfName = "NotoSansAvestan-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansBalinese
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansBalinese, TestSize.Level1)
{
    std::string ttfName = "NotoSansBalinese[wght].ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansBamum
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansBamum, TestSize.Level1)
{
    std::string ttfName = "NotoSansBamum[wght].ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansBassaVah
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansBassaVah, TestSize.Level1)
{
    std::string ttfName = "NotoSansBassaVah[wght].ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansBatak
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansBatak, TestSize.Level1)
{
    std::string ttfName = "NotoSansBatak-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansBengaliUI_Bold
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansBengaliUI_Bold, TestSize.Level1)
{
    std::string ttfName = "NotoSansBengaliUI-Bold.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansBengaliUI_Medium
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansBengaliUI_Medium, TestSize.Level1)
{
    std::string ttfName = "NotoSansBengaliUI-Medium.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansBengaliUI_Regular
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansBengaliUI_Regular, TestSize.Level1)
{
    std::string ttfName = "NotoSansBengaliUI-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansBengaliUI_SemiBold
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansBengaliUI_SemiBold, TestSize.Level1)
{
    std::string ttfName = "NotoSansBengaliUI-SemiBold.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansBengali
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansBengali, TestSize.Level1)
{
    std::string ttfName = "NotoSansBengali[wdth,wght].ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansBhaiksuki
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansBhaiksuki, TestSize.Level1)
{
    std::string ttfName = "NotoSansBhaiksuki-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansBrahmi
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansBrahmi, TestSize.Level1)
{
    std::string ttfName = "NotoSansBrahmi-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansBuginese
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansBuginese, TestSize.Level1)
{
    std::string ttfName = "NotoSansBuginese-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansBuhid
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansBuhid, TestSize.Level1)
{
    std::string ttfName = "NotoSansBuhid-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansCJK_Regular
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansCJK_Regular, TestSize.Level1)
{
    std::string ttfName = "NotoSansCJK-Regular.ttc";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansCanadianAboriginal
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansCanadianAboriginal, TestSize.Level1)
{
    std::string ttfName = "NotoSansCanadianAboriginal[wght].ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansCarian
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansCarian, TestSize.Level1)
{
    std::string ttfName = "NotoSansCarian-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansChakma
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansChakma, TestSize.Level1)
{
    std::string ttfName = "NotoSansChakma-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansCham
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansCham, TestSize.Level1)
{
    std::string ttfName = "NotoSansCham[wght].ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansCherokee
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansCherokee, TestSize.Level1)
{
    std::string ttfName = "NotoSansCherokee[wght].ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansCoptic
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansCoptic, TestSize.Level1)
{
    std::string ttfName = "NotoSansCoptic-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansCuneiform
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansCuneiform, TestSize.Level1)
{
    std::string ttfName = "NotoSansCuneiform-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansCypriot
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansCypriot, TestSize.Level1)
{
    std::string ttfName = "NotoSansCypriot-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansDeseret
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansDeseret, TestSize.Level1)
{
    std::string ttfName = "NotoSansDeseret-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansDevanagariUI_Bold
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansDevanagariUI_Bold, TestSize.Level1)
{
    std::string ttfName = "NotoSansDevanagariUI-Bold.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansDevanagariUI_Medium
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansDevanagariUI_Medium, TestSize.Level1)
{
    std::string ttfName = "NotoSansDevanagariUI-Medium.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansDevanagariUI_Regular
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansDevanagariUI_Regular, TestSize.Level1)
{
    std::string ttfName = "NotoSansDevanagariUI-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansDevanagariUI_SemiBold
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansDevanagariUI_SemiBold, TestSize.Level1)
{
    std::string ttfName = "NotoSansDevanagariUI-SemiBold.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansDevanagari
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansDevanagari, TestSize.Level1)
{
    std::string ttfName = "NotoSansDevanagari[wdth,wght].ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansEgyptianHieroglyphs
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansEgyptianHieroglyphs, TestSize.Level1)
{
    std::string ttfName = "NotoSansEgyptianHieroglyphs-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansElbasan
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansElbasan, TestSize.Level1)
{
    std::string ttfName = "NotoSansElbasan-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansEthiopic
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansEthiopic, TestSize.Level1)
{
    std::string ttfName = "NotoSansEthiopic[wdth,wght].ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansGeorgian
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansGeorgian, TestSize.Level1)
{
    std::string ttfName = "NotoSansGeorgian[wdth,wght].ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansGlagolitic
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansGlagolitic, TestSize.Level1)
{
    std::string ttfName = "NotoSansGlagolitic-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansGothic
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansGothic, TestSize.Level1)
{
    std::string ttfName = "NotoSansGothic-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansGrantha
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansGrantha, TestSize.Level1)
{
    std::string ttfName = "NotoSansGrantha-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansGujaratiUI_Bold
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansGujaratiUI_Bold, TestSize.Level1)
{
    std::string ttfName = "NotoSansGujaratiUI-Bold.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansGujaratiUI_Regular
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansGujaratiUI_Regular, TestSize.Level1)
{
    std::string ttfName = "NotoSansGujaratiUI-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansGujarati
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansGujarati, TestSize.Level1)
{
    std::string ttfName = "NotoSansGujarati[wdth,wght].ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansGunjalaGondi
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansGunjalaGondi, TestSize.Level1)
{
    std::string ttfName = "NotoSansGunjalaGondi[wght].ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansGurmukhiUI_Bold
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansGurmukhiUI_Bold, TestSize.Level1)
{
    std::string ttfName = "NotoSansGurmukhiUI-Bold.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansGurmukhiUI_Medium
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansGurmukhiUI_Medium, TestSize.Level1)
{
    std::string ttfName = "NotoSansGurmukhiUI-Medium.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansGurmukhiUI_Regular
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansGurmukhiUI_Regular, TestSize.Level1)
{
    std::string ttfName = "NotoSansGurmukhiUI-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansGurmukhiUI_SemiBold
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansGurmukhiUI_SemiBold, TestSize.Level1)
{
    std::string ttfName = "NotoSansGurmukhiUI-SemiBold.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansGurmukhi
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansGurmukhi, TestSize.Level1)
{
    std::string ttfName = "NotoSansGurmukhi[wdth,wght].ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansHanifiRohingya
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansHanifiRohingya, TestSize.Level1)
{
    std::string ttfName = "NotoSansHanifiRohingya[wght].ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansHanunoo
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansHanunoo, TestSize.Level1)
{
    std::string ttfName = "NotoSansHanunoo-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansHatran
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansHatran, TestSize.Level1)
{
    std::string ttfName = "NotoSansHatran-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansHebrew
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansHebrew, TestSize.Level1)
{
    std::string ttfName = "NotoSansHebrew[wdth,wght].ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansImperialAramaic
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansImperialAramaic, TestSize.Level1)
{
    std::string ttfName = "NotoSansImperialAramaic-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansInscriptionalPahlavi
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansInscriptionalPahlavi, TestSize.Level1)
{
    std::string ttfName = "NotoSansInscriptionalPahlavi-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansInscriptionalParthian
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansInscriptionalParthian, TestSize.Level1)
{
    std::string ttfName = "NotoSansInscriptionalParthian-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansJavanese
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansJavanese, TestSize.Level1)
{
    std::string ttfName = "NotoSansJavanese[wght].ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansKaithi
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansKaithi, TestSize.Level1)
{
    std::string ttfName = "NotoSansKaithi-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansKannadaUI_Bold
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansKannadaUI_Bold, TestSize.Level1)
{
    std::string ttfName = "NotoSansKannadaUI-Bold.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansKannadaUI_Medium
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansKannadaUI_Medium, TestSize.Level1)
{
    std::string ttfName = "NotoSansKannadaUI-Medium.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansKannadaUI_Regular
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansKannadaUI_Regular, TestSize.Level1)
{
    std::string ttfName = "NotoSansKannadaUI-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansKannadaUI_SemiBold
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansKannadaUI_SemiBold, TestSize.Level1)
{
    std::string ttfName = "NotoSansKannadaUI-SemiBold.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansKannada
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansKannada, TestSize.Level1)
{
    std::string ttfName = "NotoSansKannada[wdth,wght].ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansKayahLi
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansKayahLi, TestSize.Level1)
{
    std::string ttfName = "NotoSansKayahLi[wght].ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansKharoshthi
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansKharoshthi, TestSize.Level1)
{
    std::string ttfName = "NotoSansKharoshthi-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansKhmer
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansKhmer, TestSize.Level1)
{
    std::string ttfName = "NotoSansKhmer[wdth,wght].ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansKhojki
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansKhojki, TestSize.Level1)
{
    std::string ttfName = "NotoSansKhojki-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansLao
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansLao, TestSize.Level1)
{
    std::string ttfName = "NotoSansLao[wdth,wght].ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansLepcha
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansLepcha, TestSize.Level1)
{
    std::string ttfName = "NotoSansLepcha-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansLimbu
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansLimbu, TestSize.Level1)
{
    std::string ttfName = "NotoSansLimbu-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansLinearA
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansLinearA, TestSize.Level1)
{
    std::string ttfName = "NotoSansLinearA-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansLinearB
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansLinearB, TestSize.Level1)
{
    std::string ttfName = "NotoSansLinearB-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansLisu
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansLisu, TestSize.Level1)
{
    std::string ttfName = "NotoSansLisu[wght].ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansLycian
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansLycian, TestSize.Level1)
{
    std::string ttfName = "NotoSansLycian-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansLydian
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansLydian, TestSize.Level1)
{
    std::string ttfName = "NotoSansLydian-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansMalayalamUI_Bold
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansMalayalamUI_Bold, TestSize.Level1)
{
    std::string ttfName = "NotoSansMalayalamUI-Bold.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansMalayalamUI_Medium
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansMalayalamUI_Medium, TestSize.Level1)
{
    std::string ttfName = "NotoSansMalayalamUI-Medium.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansMalayalamUI_Regular
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansMalayalamUI_Regular, TestSize.Level1)
{
    std::string ttfName = "NotoSansMalayalamUI-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansMalayalamUI_SemiBold
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansMalayalamUI_SemiBold, TestSize.Level1)
{
    std::string ttfName = "NotoSansMalayalamUI-SemiBold.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansMalayalam
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansMalayalam, TestSize.Level1)
{
    std::string ttfName = "NotoSansMalayalam[wdth,wght].ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansMandaic
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansMandaic, TestSize.Level1)
{
    std::string ttfName = "NotoSansMandaic-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansManichaean
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansManichaean, TestSize.Level1)
{
    std::string ttfName = "NotoSansManichaean-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansMarchen
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansMarchen, TestSize.Level1)
{
    std::string ttfName = "NotoSansMarchen-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansMasaramGondi
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansMasaramGondi, TestSize.Level1)
{
    std::string ttfName = "NotoSansMasaramGondi-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansMath
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansMath, TestSize.Level1)
{
    std::string ttfName = "NotoSansMath-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansMedefaidrin
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansMedefaidrin, TestSize.Level1)
{
    std::string ttfName = "NotoSansMedefaidrin[wght].ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansMeeteiMayek
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansMeeteiMayek, TestSize.Level1)
{
    std::string ttfName = "NotoSansMeeteiMayek[wght].ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansMeroitic
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansMeroitic, TestSize.Level1)
{
    std::string ttfName = "NotoSansMeroitic-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansMiao
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansMiao, TestSize.Level1)
{
    std::string ttfName = "NotoSansMiao-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansModi
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansModi, TestSize.Level1)
{
    std::string ttfName = "NotoSansModi-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansMongolian
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansMongolian, TestSize.Level1)
{
    std::string ttfName = "NotoSansMongolian-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansMono
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansMono, TestSize.Level1)
{
    std::string ttfName = "NotoSansMono[wdth,wght].ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansMro
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansMro, TestSize.Level1)
{
    std::string ttfName = "NotoSansMro-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansMultani
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansMultani, TestSize.Level1)
{
    std::string ttfName = "NotoSansMultani-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansMyanmar
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansMyanmar, TestSize.Level1)
{
    std::string ttfName = "NotoSansMyanmar[wdth,wght].ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansNKo
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansNKo, TestSize.Level1)
{
    std::string ttfName = "NotoSansNKo-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansNabataean
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansNabataean, TestSize.Level1)
{
    std::string ttfName = "NotoSansNabataean-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansNewTaiLue
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansNewTaiLue, TestSize.Level1)
{
    std::string ttfName = "NotoSansNewTaiLue[wght].ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansNewa
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansNewa, TestSize.Level1)
{
    std::string ttfName = "NotoSansNewa-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansOgham
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansOgham, TestSize.Level1)
{
    std::string ttfName = "NotoSansOgham-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansOlChiki
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansOlChiki, TestSize.Level1)
{
    std::string ttfName = "NotoSansOlChiki[wght].ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansOldItalic
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansOldItalic, TestSize.Level1)
{
    std::string ttfName = "NotoSansOldItalic-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansOldNorthArabian
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansOldNorthArabian, TestSize.Level1)
{
    std::string ttfName = "NotoSansOldNorthArabian-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansOldPermic
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansOldPermic, TestSize.Level1)
{
    std::string ttfName = "NotoSansOldPermic-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansOldPersian
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansOldPersian, TestSize.Level1)
{
    std::string ttfName = "NotoSansOldPersian-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansOldSouthArabian
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansOldSouthArabian, TestSize.Level1)
{
    std::string ttfName = "NotoSansOldSouthArabian-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansOldTurkic
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansOldTurkic, TestSize.Level1)
{
    std::string ttfName = "NotoSansOldTurkic-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansOriya
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansOriya, TestSize.Level1)
{
    std::string ttfName = "NotoSansOriya[wdth,wght].ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansOsage
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansOsage, TestSize.Level1)
{
    std::string ttfName = "NotoSansOsage-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansOsmanya
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansOsmanya, TestSize.Level1)
{
    std::string ttfName = "NotoSansOsmanya-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansPahawhHmong
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansPahawhHmong, TestSize.Level1)
{
    std::string ttfName = "NotoSansPahawhHmong-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansPalmyrene
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansPalmyrene, TestSize.Level1)
{
    std::string ttfName = "NotoSansPalmyrene-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansPauCinHau
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansPauCinHau, TestSize.Level1)
{
    std::string ttfName = "NotoSansPauCinHau-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansPhags_Pa
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansPhags_Pa, TestSize.Level1)
{
    std::string ttfName = "NotoSansPhags-Pa-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansPhoenician
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansPhoenician, TestSize.Level1)
{
    std::string ttfName = "NotoSansPhoenician-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansRejang
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansRejang, TestSize.Level1)
{
    std::string ttfName = "NotoSansRejang-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansRunic
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansRunic, TestSize.Level1)
{
    std::string ttfName = "NotoSansRunic-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansSamaritan
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansSamaritan, TestSize.Level1)
{
    std::string ttfName = "NotoSansSamaritan-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansSaurashtra
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansSaurashtra, TestSize.Level1)
{
    std::string ttfName = "NotoSansSaurashtra-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansSharada
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansSharada, TestSize.Level1)
{
    std::string ttfName = "NotoSansSharada-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansShavian
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansShavian, TestSize.Level1)
{
    std::string ttfName = "NotoSansShavian-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansSinhalaUI_Bold
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansSinhalaUI_Bold, TestSize.Level1)
{
    std::string ttfName = "NotoSansSinhalaUI-Bold.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansSinhalaUI_Medium
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansSinhalaUI_Medium, TestSize.Level1)
{
    std::string ttfName = "NotoSansSinhalaUI-Medium.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansSinhalaUI_Regular
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansSinhalaUI_Regular, TestSize.Level1)
{
    std::string ttfName = "NotoSansSinhalaUI-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansSinhalaUI_SemiBold
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansSinhalaUI_SemiBold, TestSize.Level1)
{
    std::string ttfName = "NotoSansSinhalaUI-SemiBold.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansSinhala
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansSinhala, TestSize.Level1)
{
    std::string ttfName = "NotoSansSinhala[wdth,wght].ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansSoraSompeng
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansSoraSompeng, TestSize.Level1)
{
    std::string ttfName = "NotoSansSoraSompeng[wght].ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansSoyombo
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansSoyombo, TestSize.Level1)
{
    std::string ttfName = "NotoSansSoyombo-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansSundanese
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansSundanese, TestSize.Level1)
{
    std::string ttfName = "NotoSansSundanese[wght].ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansSylotiNagri
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansSylotiNagri, TestSize.Level1)
{
    std::string ttfName = "NotoSansSylotiNagri-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansSymbols
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansSymbols, TestSize.Level1)
{
    std::string ttfName = "NotoSansSymbols-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansSymbols2
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansSymbols2, TestSize.Level1)
{
    std::string ttfName = "NotoSansSymbols2-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansSyriacEastern
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansSyriacEastern, TestSize.Level1)
{
    std::string ttfName = "NotoSansSyriacEastern[wght].ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansSyriacWestern
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansSyriacWestern, TestSize.Level1)
{
    std::string ttfName = "NotoSansSyriacWestern[wght].ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansTagalog
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansTagalog, TestSize.Level1)
{
    std::string ttfName = "NotoSansTagalog-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansTagbanwa
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansTagbanwa, TestSize.Level1)
{
    std::string ttfName = "NotoSansTagbanwa-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansTaiLe
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansTaiLe, TestSize.Level1)
{
    std::string ttfName = "NotoSansTaiLe-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansTaiTham
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansTaiTham, TestSize.Level1)
{
    std::string ttfName = "NotoSansTaiTham[wght].ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansTaiViet
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansTaiViet, TestSize.Level1)
{
    std::string ttfName = "NotoSansTaiViet-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansTakri
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansTakri, TestSize.Level1)
{
    std::string ttfName = "NotoSansTakri-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansTamilUI_Bold
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansTamilUI_Bold, TestSize.Level1)
{
    std::string ttfName = "NotoSansTamilUI-Bold.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansTamilUI_Medium
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansTamilUI_Medium, TestSize.Level1)
{
    std::string ttfName = "NotoSansTamilUI-Medium.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansTamilUI_Regular
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansTamilUI_Regular, TestSize.Level1)
{
    std::string ttfName = "NotoSansTamilUI-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansTamilUI_SemiBold
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansTamilUI_SemiBold, TestSize.Level1)
{
    std::string ttfName = "NotoSansTamilUI-SemiBold.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansTamil
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansTamil, TestSize.Level1)
{
    std::string ttfName = "NotoSansTamil[wdth,wght].ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansTeluguUI_Bold
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansTeluguUI_Bold, TestSize.Level1)
{
    std::string ttfName = "NotoSansTeluguUI-Bold.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansTeluguUI_Medium
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansTeluguUI_Medium, TestSize.Level1)
{
    std::string ttfName = "NotoSansTeluguUI-Medium.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansTeluguUI_Regular
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansTeluguUI_Regular, TestSize.Level1)
{
    std::string ttfName = "NotoSansTeluguUI-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansTeluguUI_SemiBold
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansTeluguUI_SemiBold, TestSize.Level1)
{
    std::string ttfName = "NotoSansTeluguUI-SemiBold.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansTelugu
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansTelugu, TestSize.Level1)
{
    std::string ttfName = "NotoSansTelugu[wdth,wght].ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansThaana
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansThaana, TestSize.Level1)
{
    std::string ttfName = "NotoSansThaana[wght].ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansThai
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansThai, TestSize.Level1)
{
    std::string ttfName = "NotoSansThai[wdth,wght].ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansTifinagh
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansTifinagh, TestSize.Level1)
{
    std::string ttfName = "NotoSansTifinagh-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansUgaritic
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansUgaritic, TestSize.Level1)
{
    std::string ttfName = "NotoSansUgaritic-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansVai
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansVai, TestSize.Level1)
{
    std::string ttfName = "NotoSansVai-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansWancho
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansWancho, TestSize.Level1)
{
    std::string ttfName = "NotoSansWancho-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansWarangCiti
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansWarangCiti, TestSize.Level1)
{
    std::string ttfName = "NotoSansWarangCiti-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSansYi
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSansYi, TestSize.Level1)
{
    std::string ttfName = "NotoSansYi-Regular.ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}

/**
 * @tc.name: GenerateUnicodeItemNotoSans
 * @tc.desc: Test GenerateUnicodeItem
 * @tc.type: FUNC
 * @tc.require: SR20251027820925
 */
HWTEST_F(FontUnicodeQueryTest, GenerateUnicodeItemNotoSans, TestSize.Level1)
{
    std::string ttfName = "NotoSans[wdth,wght].ttf";
    std::string path = GenerateRelativePathForFontResource(ttfName);
    auto typeface = Typeface::MakeFromFile(path.c_str());
    ASSERT_TRUE(typeface);
    auto unicodeRange = FontUnicodeQuery::GenerateUnicodeItem(typeface);
    EXPECT_FALSE(JudgeIfInvalidCodePointExist(unicodeRange));
    auto it = FONT_DATA.find(ttfName);
    ASSERT_TRUE(it != FONT_DATA.end());
    const FontInfo& fontInfo = it->second;
    EXPECT_EQ(unicodeRange.size(), fontInfo.size);
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.front));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.mid));
    EXPECT_TRUE(JudgeIfCodePointExist(unicodeRange, fontInfo.back));
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS