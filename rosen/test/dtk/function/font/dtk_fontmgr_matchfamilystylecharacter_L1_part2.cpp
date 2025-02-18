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

//对应用例 FontMgr_MatchFamilyStyleCharacter_3059
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 59)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, 1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3060
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 60)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, 1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3061
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 61)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, -1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3062
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 62)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, 0, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3063
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 63)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3064
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 64)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, -1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3065
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 65)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3066
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 66)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3067
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 67)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, 1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3068
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 68)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3069
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 69)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, -1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3070
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 70)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, 1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3071
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 71)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3072
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 72)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, -1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3073
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 73)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3074
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 74)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3075
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 75)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, -1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3076
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 76)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, -1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3077
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 77)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, -1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3078
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 78)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3079
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 79)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, -1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3080
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 80)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3081
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 81)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3082
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 82)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3083
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 83)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, -1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3084
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 84)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, 1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3085
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 85)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3086
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 86)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, 1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3087
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 87)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, -1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3088
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 88)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, -1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3089
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 89)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, 1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3090
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 90)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, -1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3091
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 91)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3092
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 92)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3093
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 93)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3094
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 94)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, -1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3095
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 95)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, -1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3096
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 96)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, -1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3097
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 97)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3098
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 98)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, -1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3099
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 99)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3100
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 100)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, -1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3101
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 101)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3102
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 102)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, -1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3103
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 103)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3104
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 104)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3105
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 105)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, -1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3106
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 106)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3107
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 107)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, 0, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3108
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 108)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, -1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3109
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 109)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, -1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3110
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 110)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3111
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 111)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, 0, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3112
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 112)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, 1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3113
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 113)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, 0, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3114
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 114)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, -1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3115
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 115)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3116
DEF_DTK(fontmgr_matchfamilystylecharater_2, TestLevel::L1, 116)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, -1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

}
}
