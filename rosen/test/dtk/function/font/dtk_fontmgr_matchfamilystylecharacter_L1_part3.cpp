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

//对应用例 FontMgr_MatchFamilyStyleCharacter_3117
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 117)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, -1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3118
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 118)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3119
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 119)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3120
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 120)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, -1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3121
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 121)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, 0, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3122
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 122)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, -1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3123
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 123)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, 1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3124
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 124)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3125
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 125)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3126
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 126)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3127
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 127)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, 0, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3128
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 128)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3129
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 129)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, 1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3130
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 130)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3131
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 131)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, 0, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3132
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 132)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3133
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 133)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, -1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3134
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 134)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3135
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 135)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, -1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3136
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 136)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3137
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 137)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, 1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3138
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 138)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3139
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 139)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, 0, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3140
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 140)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3141
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 141)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, -1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3142
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 142)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, -1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3143
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 143)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3144
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 144)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3145
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 145)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3146
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 146)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3147
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 147)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, -1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3148
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 148)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3149
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 149)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, 1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3150
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 150)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, -1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3151
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 151)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, -1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3152
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 152)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, -1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3153
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 153)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, -1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3154
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 154)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, 0, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3155
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 155)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3156
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 156)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, 0, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3157
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 157)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, -1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3158
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 158)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3159
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 159)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3160
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 160)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, 0, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3161
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 161)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, -1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3162
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 162)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3163
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 163)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3164
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 164)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3165
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 165)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, 0, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3166
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 166)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, -1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3167
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 167)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, 0, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3168
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 168)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3169
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 169)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, -1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3170
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 170)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3171
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 171)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, -1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3172
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 172)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, 0, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3173
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 173)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3174
DEF_DTK(fontmgr_matchfamilystylecharater_3, TestLevel::L1, 174)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

}
}
