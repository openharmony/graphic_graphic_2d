/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "customized/random_typeface.h"

#include <filesystem>

#include "file_ex.h"
#include "common/safuzz_log.h"
#include "random/random_data.h"
#include "random/random_engine.h"


namespace OHOS {
namespace Rosen {
namespace {
const std::vector<std::string> FONTS = {
    "DejaVuMathTeXGyre.ttf",
    "FTToken.ttf",
    "FTToken2.ttf",
    "FTToken3.ttf",
    "FTTokens.ttf",
    "HMOSColorEmojiCompat.ttf",
    "HMOSColorEmojiFlags.ttf",
    "HMSymbolVF.ttf",
    "HarmonyOS_Sans.ttf",
    "HarmonyOS_Sans_Condensed.ttf",
    "HarmonyOS_Sans_Condensed_Italic.ttf",
    "HarmonyOS_Sans_Digit.ttf",
    "HarmonyOS_Sans_Digit_Medium.ttf",
    "HarmonyOS_Sans_Italic.ttf",
    "HarmonyOS_Sans_Naskh_Arabic.ttf",
    "HarmonyOS_Sans_Naskh_Arabic_UI.ttf",
    "HarmonyOS_Sans_SC.ttf",
    "HarmonyOS_Sans_TC.ttf",
    "NotoSansAdlam[wght].ttf",
    "NotoSansAnatolianHieroglyphs-Regular.ttf",
    "NotoSansArmenian[wdth,wght].ttf",
    "NotoSansAvestan-Regular.ttf",
    "NotoSansBalinese[wght].ttf",
    "NotoSansBamum[wght].ttf",
    "NotoSansBassaVah[wght].ttf",
    "NotoSansBatak-Regular.ttf",
    "NotoSansBengaliUI-Bold.ttf",
    "NotoSansBengaliUI-Medium.ttf",
    "NotoSansBengaliUI-Regular.ttf",
    "NotoSansBengaliUI-SemiBold.ttf",
    "NotoSansBengali[wdth,wght].ttf",
    "NotoSansBhaiksuki-Regular.ttf",
    "NotoSansBrahmi-Regular.ttf",
    "NotoSansBuginese-Regular.ttf",
    "NotoSansBuhid-Regular.ttf",
    "NotoSansCJK-Regular.ttc",
    "NotoSansCanadianAboriginal[wght].ttf",
    "NotoSansCarian-Regular.ttf",
    "NotoSansChakma-Regular.ttf",
    "NotoSansCham[wght].ttf",
    "NotoSansCherokee[wght].ttf",
    "NotoSansCoptic-Regular.ttf",
    "NotoSansCuneiform-Regular.ttf",
    "NotoSansCypriot-Regular.ttf",
    "NotoSansDeseret-Regular.ttf",
    "NotoSansDevanagariUI-Bold.ttf",
    "NotoSansDevanagariUI-Medium.ttf",
    "NotoSansDevanagariUI-Regular.ttf",
    "NotoSansDevanagariUI-SemiBold.ttf",
    "NotoSansDevanagari[wdth,wght].ttf",
    "NotoSansEgyptianHieroglyphs-Regular.ttf",
    "NotoSansElbasan-Regular.ttf",
    "NotoSansEthiopic[wdth,wght].ttf",
    "NotoSansGeorgian[wdth,wght].ttf",
    "NotoSansGlagolitic-Regular.ttf",
    "NotoSansGothic-Regular.ttf",
    "NotoSansGrantha-Regular.ttf",
    "NotoSansGujaratiUI-Bold.ttf",
    "NotoSansGujaratiUI-Regular.ttf",
    "NotoSansGujarati[wdth,wght].ttf",
    "NotoSansGunjalaGondi[wght].ttf",
    "NotoSansGurmukhiUI-Bold.ttf",
    "NotoSansGurmukhiUI-Medium.ttf",
    "NotoSansGurmukhiUI-Regular.ttf",
    "NotoSansGurmukhiUI-SemiBold.ttf",
    "NotoSansGurmukhi[wdth,wght].ttf",
    "NotoSansHanifiRohingya[wght].ttf",
    "NotoSansHanunoo-Regular.ttf",
    "NotoSansHatran-Regular.ttf",
    "NotoSansHebrew[wdth,wght].ttf",
    "NotoSansImperialAramaic-Regular.ttf",
    "NotoSansInscriptionalPahlavi-Regular.ttf",
    "NotoSansInscriptionalParthian-Regular.ttf",
    "NotoSansJavanese[wght].ttf",
    "NotoSansKaithi-Regular.ttf",
    "NotoSansKannadaUI-Bold.ttf",
    "NotoSansKannadaUI-Medium.ttf",
    "NotoSansKannadaUI-Regular.ttf",
    "NotoSansKannadaUI-SemiBold.ttf",
    "NotoSansKannada[wdth,wght].ttf",
    "NotoSansKayahLi[wght].ttf",
    "NotoSansKharoshthi-Regular.ttf",
    "NotoSansKhmer[wdth,wght].ttf",
    "NotoSansKhojki-Regular.ttf",
    "NotoSansLao[wdth,wght].ttf",
    "NotoSansLepcha-Regular.ttf",
    "NotoSansLimbu-Regular.ttf",
    "NotoSansLinearA-Regular.ttf",
    "NotoSansLinearB-Regular.ttf",
    "NotoSansLisu[wght].ttf",
    "NotoSansLycian-Regular.ttf",
    "NotoSansLydian-Regular.ttf",
    "NotoSansMalayalamUI-Bold.ttf",
    "NotoSansMalayalamUI-Medium.ttf",
    "NotoSansMalayalamUI-Regular.ttf",
    "NotoSansMalayalamUI-SemiBold.ttf",
    "NotoSansMalayalam[wdth,wght].ttf",
    "NotoSansMandaic-Regular.ttf",
    "NotoSansManichaean-Regular.ttf",
    "NotoSansMarchen-Regular.ttf",
    "NotoSansMasaramGondi-Regular.ttf",
    "NotoSansMath-Regular.ttf",
    "NotoSansMedefaidrin[wght].ttf",
    "NotoSansMeeteiMayek[wght].ttf",
    "NotoSansMeroitic-Regular.ttf",
    "NotoSansMiao-Regular.ttf",
    "NotoSansModi-Regular.ttf",
    "NotoSansMongolian-Regular.ttf",
    "NotoSansMono[wdth,wght].ttf",
    "NotoSansMro-Regular.ttf",
    "NotoSansMultani-Regular.ttf",
    "NotoSansMyanmar[wdth,wght].ttf",
    "NotoSansNKo-Regular.ttf",
    "NotoSansNabataean-Regular.ttf",
    "NotoSansNewTaiLue[wght].ttf",
    "NotoSansNewa-Regular.ttf",
    "NotoSansOgham-Regular.ttf",
    "NotoSansOlChiki[wght].ttf",
    "NotoSansOldItalic-Regular.ttf",
    "NotoSansOldNorthArabian-Regular.ttf",
    "NotoSansOldPermic-Regular.ttf",
    "NotoSansOldPersian-Regular.ttf",
    "NotoSansOldSouthArabian-Regular.ttf",
    "NotoSansOldTurkic-Regular.ttf",
    "NotoSansOriya[wdth,wght].ttf",
    "NotoSansOsage-Regular.ttf",
    "NotoSansOsmanya-Regular.ttf",
    "NotoSansPahawhHmong-Regular.ttf",
    "NotoSansPalmyrene-Regular.ttf",
    "NotoSansPauCinHau-Regular.ttf",
    "NotoSansPhags-Pa-Regular.ttf",
    "NotoSansPhoenician-Regular.ttf",
    "NotoSansRejang-Regular.ttf",
    "NotoSansRunic-Regular.ttf",
    "NotoSansSamaritan-Regular.ttf",
    "NotoSansSaurashtra-Regular.ttf",
    "NotoSansSharada-Regular.ttf",
    "NotoSansShavian-Regular.ttf",
    "NotoSansSinhalaUI-Bold.ttf",
    "NotoSansSinhalaUI-Medium.ttf",
    "NotoSansSinhalaUI-Regular.ttf",
    "NotoSansSinhalaUI-SemiBold.ttf",
    "NotoSansSinhala[wdth,wght].ttf",
    "NotoSansSoraSompeng[wght].ttf",
    "NotoSansSoyombo-Regular.ttf",
    "NotoSansSundanese[wght].ttf",
    "NotoSansSylotiNagri-Regular.ttf",
    "NotoSansSymbols-Regular.ttf",
    "NotoSansSymbols2-Regular.ttf",
    "NotoSansSyriacEastern[wght].ttf",
    "NotoSansSyriacWestern[wght].ttf",
    "NotoSansTagalog-Regular.ttf",
    "NotoSansTagbanwa-Regular.ttf",
    "NotoSansTaiLe-Regular.ttf",
    "NotoSansTaiTham[wght].ttf",
    "NotoSansTaiViet-Regular.ttf",
    "NotoSansTakri-Regular.ttf",
    "NotoSansTamilUI-Bold.ttf",
    "NotoSansTamilUI-Medium.ttf",
    "NotoSansTamilUI-Regular.ttf",
    "NotoSansTamilUI-SemiBold.ttf",
    "NotoSansTamil[wdth,wght].ttf",
    "NotoSansTeluguUI-Bold.ttf",
    "NotoSansTeluguUI-Medium.ttf",
    "NotoSansTeluguUI-Regular.ttf",
    "NotoSansTeluguUI-SemiBold.ttf",
    "NotoSansTelugu[wdth,wght].ttf",
    "NotoSansThaana[wght].ttf",
    "NotoSansThai[wdth,wght].ttf",
    "NotoSansTifinagh-Regular.ttf",
    "NotoSansUgaritic-Regular.ttf",
    "NotoSansVai-Regular.ttf",
    "NotoSansWancho-Regular.ttf",
    "NotoSansWarangCiti-Regular.ttf",
    "NotoSansYi-Regular.ttf",
    "NotoSans[wdth,wght].ttf",
    "NotoSerifAhom-Regular.ttf",
    "NotoSerifArmenian[wdth,wght].ttf",
    "NotoSerifBengali[wdth,wght].ttf",
    "NotoSerifCJK-Regular.ttc",
    "NotoSerifDevanagari[wdth,wght].ttf",
    "NotoSerifDogra-Regular.ttf",
    "NotoSerifEthiopic[wdth,wght].ttf",
    "NotoSerifGeorgian[wdth,wght].ttf",
    "NotoSerifGujarati[wght].ttf",
    "NotoSerifGurmukhi[wght].ttf",
    "NotoSerifHebrew[wdth,wght].ttf",
    "NotoSerifKannada[wght].ttf",
    "NotoSerifKhmer[wdth,wght].ttf",
    "NotoSerifLao[wdth,wght].ttf",
    "NotoSerifMalayalam[wght].ttf",
    "NotoSerifMyanmar[wdth,wght].ttf",
    "NotoSerifSinhala[wdth,wght].ttf",
    "NotoSerifTamil[wdth,wght].ttf",
    "NotoSerifTelugu[wght].ttf",
    "NotoSerifThai[wdth,wght].ttf",
    "NotoSerifTibetan[wght].ttf",
    "NotoSerifYezidi[wght].ttf",
    "NotoSerif[wdth,wght].ttf",
    "Roboto-Regular.ttf",
    "ShuS-SC.ttf",
};
constexpr int DEFAULT_FONT_INDEX = 8;

std::string GetFilePath(int index)
{
    std::string filePath = std::string("/system/fonts/") + FONTS[index];
    if (!std::filesystem::exists(filePath)) {
        filePath = std::string("/system/fonts/") + FONTS[DEFAULT_FONT_INDEX];
    }
    return filePath;
}
} // namespace

std::shared_ptr<Drawing::Typeface> RandomTypeface::GetRandomTypeface()
{
    int index = RandomEngine::GetRandomIndex(FONTS.size() - 1);
    std::string filePath = GetFilePath(index);
    std::vector<char> content;
    // The maximum file size is 32 MB.
    if (!LoadBufferFromFile(filePath, content)) {
        SAFUZZ_LOGI("Failed to read file %s", filePath.c_str());
    }
    std::shared_ptr<Drawing::Typeface> typeface = nullptr;

    if (filePath.find("ttc") != std::string::npos) {
        static constexpr int TTC_MAX_INDEX = 5;
        uint32_t ttcIndex = static_cast<uint32_t>(RandomEngine::GetRandomIndex(TTC_MAX_INDEX));
        typeface = Drawing::Typeface::MakeFromAshmem(
            reinterpret_cast<uint8_t*>(content.data()), content.size(), 0, "safuzz", ttcIndex);
    } else {
        typeface = Drawing::Typeface::MakeFromAshmem(
            reinterpret_cast<uint8_t*>(content.data()), content.size(), 0, "safuzz");
    }

    if (typeface == nullptr) {
        typeface = Drawing::Typeface::MakeDefault();
    }

    return typeface;
}
} // namespace Rosen
} // namespace OHOS
