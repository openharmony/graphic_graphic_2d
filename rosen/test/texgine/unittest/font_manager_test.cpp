/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include <fstream>
#include <gtest/gtest.h>

#include "font_parser.h"
#include "texgine/font_manager.h"
#include "texgine/utils/exlog.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace TextEngine {
namespace {
static constexpr int WEIGHT_TEST = 900;
static const std::string FILE_NAME = "/system/fonts/visibility_list.json";

class FontManagerTest : public testing::Test {};

void ShowVisibilityFonts(std::vector<FontParser::FontDescriptor>& visibilityFonts)
{
    for (auto& it : visibilityFonts) {
        LOGSO_FUNC_LINE(INFO) << "\n fontFamily: " << it.fontFamily << "\n fontSubfamily: " << it.fontSubfamily <<
            "\n fullName: " << it.fullName << "\n italic: " << it.italic <<
            "\n monoSpace: " << it.monoSpace << "\n path: " << it.path <<
            "\n postScriptName: " << it.postScriptName << "\n symbolic: " << it.symbolic <<
            "\n weight: " << it.weight << "\n width: " << it.width;
    }
}

void FindFontPsn(std::vector<FontParser::FontDescriptor>& visibilityFonts)
{
    FontParser::FontDescriptor fontDescriptorPn;
    fontDescriptorPn.postScriptName = "HarmonyOS_Sans_Black";

    auto fontFindPn = FontManager::GetInstance()->FindFont(&visibilityFonts, fontDescriptorPn);
    if (fontFindPn != nullptr) {
        LOGSO_FUNC_LINE(INFO) << "\n postScriptName = [ postScriptName: " << fontDescriptorPn.postScriptName << " ]" <<
            "\n [ FontDescriptor: path: " << fontFindPn->path <<
            "\n postScriptName: " << fontFindPn->postScriptName <<
            "\n fullName: " << fontFindPn->fullName << "\n fontFamily: " << fontFindPn->fontFamily <<
            "\n fontSubfamily: " << fontFindPn->fontSubfamily <<
            "\n weight: " << fontFindPn->weight << "\n width: " << fontFindPn->width <<
            "\n italic: " << fontFindPn->italic << "\n monoSpace: " << fontFindPn->monoSpace <<
            "\n symbolic: " << fontFindPn->symbolic << " ]";
    }
}

void FindFontFn(std::vector<FontParser::FontDescriptor>& visibilityFonts)
{
    FontParser::FontDescriptor fontDescriptorFn;
    fontDescriptorFn.fullName = "HarmonyOS Sans Black Italic";

    auto fontFindFn = FontManager::GetInstance()->FindFont(&visibilityFonts, fontDescriptorFn);
    if (fontFindFn != nullptr) {
        LOGSO_FUNC_LINE(INFO) << "\n fullName = [ fullName: " << fontDescriptorFn.fullName << " ]" <<
            "\n [ FontDescriptor: path: " << fontFindFn->path <<
            "\n postScriptName: " << fontFindFn->postScriptName <<
            "\n fullName: " << fontFindFn->fullName << "\n fontFamily: " << fontFindFn->fontFamily <<
            "\n fontSubfamily: " << fontFindFn->fontSubfamily <<
            "\n weight: " << fontFindFn->weight << "\n width: " << fontFindFn->width <<
            "\n italic: " << fontFindFn->italic << "\n monoSpace: " << fontFindFn->monoSpace <<
            "\n symbolic: " << fontFindFn->symbolic << " ]";
    }
}

void FindFontFsy(std::vector<FontParser::FontDescriptor>& visibilityFonts)
{
    FontParser::FontDescriptor fontDescriptorFsy;
    fontDescriptorFsy.fontFamily = "HarmonyOS Sans";
    fontDescriptorFsy.fontSubfamily = "Black Italic";

    auto fontFindFsy = FontManager::GetInstance()->FindFont(&visibilityFonts, fontDescriptorFsy);
    if (fontFindFsy != nullptr) {
        LOGSO_FUNC_LINE(INFO) << "\n fontFamily + fontSubfamily = [ fontFamily: " << fontDescriptorFsy.fontFamily <<
            "\n fontSubfamily: " << fontDescriptorFsy.fontSubfamily << " ]" <<
            "\n [ FontDescriptor: path: " << fontFindFsy->path <<
            "\n postScriptName: " << fontFindFsy->postScriptName <<
            "\n fullName: " << fontFindFsy->fullName <<
            "\n fontFamily: " << fontFindFsy->fontFamily <<
            "\n fontSubfamily: " << fontFindFsy->fontSubfamily <<
            "\n weight: " << fontFindFsy->weight << "\n width: " << fontFindFsy->width <<
            "\n italic: " << fontFindFsy->italic << "\n monoSpace: " << fontFindFsy->monoSpace <<
            "\n symbolic: " << fontFindFsy->symbolic << " ]";
    }
}

void FindFontFni(std::vector<FontParser::FontDescriptor>& visibilityFonts)
{
    FontParser::FontDescriptor fontDescriptorFni;
    fontDescriptorFni.fontFamily = "HarmonyOS Sans Condensed";
    fontDescriptorFni.italic = false;

    auto fontFindFni = FontManager::GetInstance()->FindFont(&visibilityFonts, fontDescriptorFni);
    if (fontFindFni != nullptr) {
        LOGSO_FUNC_LINE(INFO) << "\n fontFamily + italic = [ fontFamily: " << fontDescriptorFni.fontFamily <<
            "\n italic: " << fontDescriptorFni.italic << " ]" <<
            "\n [ FontDescriptor: path: " << fontFindFni->path <<
            "\n postScriptName: " << fontFindFni->postScriptName <<
            "\n fullName: " << fontFindFni->fullName <<
            "\n fontFamily: " << fontFindFni->fontFamily <<
            "\n fontSubfamily: " << fontFindFni->fontSubfamily <<
            "\n weight: " << fontFindFni->weight << "\n width: " << fontFindFni->width <<
            "\n italic: " << fontFindFni->italic << "\n monoSpace: " << fontFindFni->monoSpace <<
            "\n symbolic: " << fontFindFni->symbolic << " ]";
    }
}

void FindFontFnw(std::vector<FontParser::FontDescriptor>& visibilityFonts)
{
    FontParser::FontDescriptor fontDescriptorFnw;
    fontDescriptorFnw.fontFamily = "HarmonyOS Sans Condensed";
    fontDescriptorFnw.weight = WEIGHT_TEST;

    auto fontFindFnw = FontManager::GetInstance()->FindFont(&visibilityFonts, fontDescriptorFnw);
    if (fontFindFnw != nullptr) {
        LOGSO_FUNC_LINE(INFO) << "\n fontFamily + weight = [ fontFamily: " << fontDescriptorFnw.fontFamily <<
            "\n weight: " << fontDescriptorFnw.weight << " ]" <<
            "\n [ FontDescriptor: path: " << fontFindFnw->path <<
            "\n postScriptName: " << fontFindFnw->postScriptName <<
            "\n fullName: " << fontFindFnw->fullName <<
            "\n fontFamily: " << fontFindFnw->fontFamily <<
            "\n fontSubfamily: " << fontFindFnw->fontSubfamily <<
            "\n weight: " << fontFindFnw->weight << "\n width: " << fontFindFnw->width <<
            "\n italic: " << fontFindFnw->italic << "\n monoSpace: " << fontFindFnw->monoSpace <<
            "\n symbolic: " << fontFindFnw->symbolic << " ]";
    }
}

void FindFontFfw(std::vector<FontParser::FontDescriptor>& visibilityFonts)
{
    FontParser::FontDescriptor fontDescriptorFfw;
    fontDescriptorFfw.fontFamily = "HarmonyOS Sans Naskh Arabic";
    fontDescriptorFfw.fontSubfamily = "Black";
    fontDescriptorFfw.weight = WEIGHT_TEST;

    auto fontFindFfw = FontManager::GetInstance()->FindFont(&visibilityFonts, fontDescriptorFfw);
    if (fontFindFfw != nullptr) {
        LOGSO_FUNC_LINE(INFO) << "\n fontFamily + fontSubfamily + weight = [ fontFamily: " <<
            fontDescriptorFfw.fontFamily << "\n fontSubfamily: " << fontDescriptorFfw.fontSubfamily <<
            "\n weight: " << fontDescriptorFfw.weight << " ]" <<
            "\n [ FontDescriptor: path: " << fontFindFfw->path <<
            "\n postScriptName: " << fontFindFfw->postScriptName <<
            "\n fullName: " << fontFindFfw->fullName <<
            "\n fontFamily: " << fontFindFfw->fontFamily <<
            "\n fontSubfamily: " << fontFindFfw->fontSubfamily <<
            "\n weight: " << fontFindFfw->weight << "\n width: " << fontFindFfw->width <<
            "\n italic: " << fontFindFfw->italic << "\n monoSpace: " << fontFindFfw->monoSpace <<
            "\n symbolic: " << fontFindFfw->symbolic << " ]";
    }
}

void FindFontFfwc(std::vector<FontParser::FontDescriptor>& visibilityFonts)
{
    FontParser::FontDescriptor fontDescriptorFfwc;
    fontDescriptorFfwc.fontFamily = "HarmonyOS Sans Naskh Arabic";
    fontDescriptorFfwc.fontSubfamily = "Black";
    fontDescriptorFfwc.weight = WEIGHT_TEST;

    auto fontFindFfwc = FontManager::GetInstance()->FindFont(&visibilityFonts, fontDescriptorFfwc);
    if (fontFindFfwc != nullptr) {
        LOGSO_FUNC_LINE(INFO) << "\n fontFamily + fontSubfamily + weight = [ fontFamily: " <<
            fontDescriptorFfwc.fontFamily <<
            "\n fontSubfamily: " << fontDescriptorFfwc.fontSubfamily <<
            "\n weight: " << fontDescriptorFfwc.weight << " ]" <<
            "\n [ FontDescriptor: path: " << fontFindFfwc->path <<
            "\n postScriptName: " << fontFindFfwc->postScriptName <<
            "\n fullName: " << fontFindFfwc->fullName <<
            "\n fontFamily: " << fontFindFfwc->fontFamily <<
            "\n fontSubfamily: " << fontFindFfwc->fontSubfamily <<
            "\n weight: " << fontFindFfwc->weight << "\n width: " << fontFindFfwc->width <<
            "\n italic: " << fontFindFfwc->italic << "\n monoSpace: " << fontFindFfwc->monoSpace <<
            "\n symbolic: " << fontFindFfwc->symbolic << " ]";
    }
}

void FindFontFfiw(std::vector<FontParser::FontDescriptor>& visibilityFonts)
{
    FontParser::FontDescriptor fontDescriptorFfiw;
    fontDescriptorFfiw.fontFamily = "HarmonyOS Sans Naskh Arabic UI";
    fontDescriptorFfiw.fontSubfamily = "Black";
    fontDescriptorFfiw.italic = false;
    fontDescriptorFfiw.weight = WEIGHT_TEST;

    auto fontFindFfiw = FontManager::GetInstance()->FindFont(&visibilityFonts, fontDescriptorFfiw);
    if (fontFindFfiw != nullptr) {
        LOGSO_FUNC_LINE(INFO) << "\n fontFamily + fontSubfamily + italic + weight = [ fontFamily: " <<
            fontDescriptorFfiw.fontFamily <<
            "\n fontSubfamily: " << fontDescriptorFfiw.fontSubfamily <<
            "\n italic: " << fontDescriptorFfiw.italic <<
            "\n weight: " << fontDescriptorFfiw.weight << " ]" <<
            "\n [ FontDescriptor: path: " << fontFindFfiw->path <<
            "\n postScriptName: " << fontFindFfiw->postScriptName <<
            "\n fullName: " << fontFindFfiw->fullName <<
            "\n fontFamily: " << fontFindFfiw->fontFamily <<
            "\n fontSubfamily: " << fontFindFfiw->fontSubfamily <<
            "\n weight: " << fontFindFfiw->weight << "\n width: " << fontFindFfiw->width <<
            "\n italic: " << fontFindFfiw->italic << "\n monoSpace: " << fontFindFfiw->monoSpace <<
            "\n symbolic: " << fontFindFfiw->symbolic << " ]";
    }
}

/**
 * @tc.name: FontManager
 * @tc.desc: Verify the FontManager
 * @tc.type FUNC
 */
HWTEST_F(FontManagerTest, FontManager, TestSize.Level1)
{
    FontParser fontParser;
    auto fontSrc = fontParser.GetVisibilityFonts();
    std::ifstream fileStream(FILE_NAME.c_str());
    if (fileStream.is_open()) {
        ASSERT_NE(fontSrc.size(), 0);
        ShowVisibilityFonts(fontSrc);
        FindFontPsn(fontSrc);
        FindFontFn(fontSrc);
        FindFontFsy(fontSrc);
        FindFontFni(fontSrc);
        FindFontFnw(fontSrc);
        FindFontFfw(fontSrc);
        FindFontFfwc(fontSrc);
        FindFontFfiw(fontSrc);
        fileStream.close();
    } else {
        ASSERT_EQ(fontSrc.size(), 0);
    }
}
} // namespace
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
