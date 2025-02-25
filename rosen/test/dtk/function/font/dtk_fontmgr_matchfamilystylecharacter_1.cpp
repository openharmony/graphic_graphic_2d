/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "../../dtk_test_ext.h"
#include "text/font.h"
#include "recording/mem_allocator.h"
#include "text/font_mgr.h"
#include "text/font_style_set.h"
#include "text/rs_xform.h"
#include "utils/point.h"
#include "utils/rect.h"
#include "text/typeface.h"

/*
测试类：FontMgr
测试接口：MatchFamilyStyleCharacter
测试内容：对接口入参themeName、fontStyle和code取值组合，构造typeface字体格式，并指定在font上，最终通过drawtextblob接口将text内容绘制在画布上
*/
namespace OHOS {
namespace Rosen {

static Drawing::FontStyle GetFontStyle(std::shared_ptr<Drawing::FontMgr>& fontMgr, bool bSlant)
{
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    std::string styleName = "";
    Drawing::FontStyle fontStyle;
    fontStyleSet->GetStyle(0, &fontStyle, &styleName);
    if (bSlant) {
        fontStyle = Drawing::FontStyle{fontStyle.GetWeight(), fontStyle.GetWidth(), Drawing::FontStyle::OBLIQUE_SLANT};
    }

    return fontStyle;
}

static void DrawTexts(TestPlaybackCanvas* playbackCanvas, std::shared_ptr<Drawing::Typeface>& typeface)
{
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);
    std::string text1 = "DDGR ddgr 鸿蒙 !@#%￥^&*;：，。";
    std::string text2 = "-_=+()123`.---~|{}【】,./?、？<>《》";
    std::string text3 = "\xE2\x99\x88\xE2\x99\x8A\xE2\x99\x88\xE2\x99\x8C\xE2\x99\x8D\xE2\x99\x8D";
    std::string texts[] = {text1, text2, text3};
    int line = 200;
    int interval2 = 200;
    for (auto text : texts) {
        std::shared_ptr<Drawing::TextBlob> textBlob = Drawing::TextBlob::MakeFromText(text.c_str(), text.size(), font);
        Drawing::Brush brush;
        playbackCanvas->AttachBrush(brush);
        playbackCanvas->DrawTextBlob(textBlob.get(), interval2, line);
        line += interval2;
        playbackCanvas->DetachBrush();
        Drawing::Pen pen;
        playbackCanvas->AttachPen(pen);
        playbackCanvas->DrawTextBlob(textBlob.get(), interval2, line);
        line += interval2;
        playbackCanvas->DetachPen();
    }
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3001
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 1)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, 1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3002
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 2)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, -1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3003
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 3)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3004
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 4)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3005
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 5)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3006
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 6)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3007
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 7)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, 1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3008
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 8)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, -1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3009
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 9)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, 0, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3010
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 10)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3011
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 11)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3012
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 12)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, -1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3013
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 13)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, 1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3014
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 14)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3015
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 15)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, -1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3016
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 16)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, 0, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3017
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 17)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, -1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3018
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 18)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, -1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3019
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 19)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3020
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 20)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3021
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 21)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3022
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 22)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, -1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3023
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 23)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, -1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3024
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 24)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3025
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 25)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, -1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3026
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 26)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3027
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 27)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3028
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 28)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3029
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 29)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, -1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3030
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 30)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, 1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3031
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 31)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, -1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3032
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 32)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, 0, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3033
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 33)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, 0, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3034
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 34)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, -1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3035
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 35)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3036
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 36)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, -1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3037
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 37)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3038
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 38)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3039
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 39)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, 0, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3040
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 40)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, -1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3041
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 41)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3042
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 42)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, -1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3043
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 43)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3044
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 44)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3045
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 45)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3046
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 46)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3047
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 47)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, -1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3048
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 48)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3049
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 49)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3050
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 50)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, -1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3051
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 51)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3052
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 52)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3053
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 53)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3054
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 54)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, -1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3055
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 55)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3056
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 56)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, -1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3057
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 57)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, 1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3058
DEF_DTK(fontmgr_matchfamilystylecharater_1, TestLevel::L1, 58)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

}
}
