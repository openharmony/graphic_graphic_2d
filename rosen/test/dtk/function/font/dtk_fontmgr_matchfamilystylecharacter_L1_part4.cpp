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

//对应用例 FontMgr_MatchFamilyStyleCharacter_3175
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 175)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, -1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3176
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 176)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, -1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3177
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 177)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3178
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 178)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, -1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3179
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 179)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, 0, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3180
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 180)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, -1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3181
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 181)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, -1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3182
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 182)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, 1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3183
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 183)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3184
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 184)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3185
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 185)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, -1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3186
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 186)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, -1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3187
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 187)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, 0, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3188
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 188)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, -1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3189
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 189)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3190
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 190)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, 0, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3191
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 191)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, 0, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3192
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 192)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, 1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3193
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 193)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3194
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 194)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3195
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 195)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, -1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3196
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 196)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3197
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 197)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, -1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3198
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 198)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3199
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 199)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3200
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 200)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3201
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 201)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3202
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 202)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3203
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 203)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3204
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 204)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3205
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 205)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3206
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 206)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, -1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3207
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 207)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3208
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 208)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, -1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3209
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 209)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3210
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 210)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, -1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3211
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 211)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3212
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 212)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3213
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 213)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, 1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3214
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 214)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3215
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 215)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, 0, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3216
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 216)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, 1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3217
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 217)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3218
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 218)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, 0, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3219
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 219)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, -1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3220
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 220)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3221
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 221)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3222
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 222)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3223
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 223)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, -1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3224
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 224)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, -1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3225
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 225)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, 1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3226
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 226)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3227
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 227)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "HMOS Color Emoji";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"zh"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, -1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3228
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 228)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, -1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3229
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 229)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "abcd";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 0, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3230
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 230)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    const char* bcp47[] = {"en"};
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, bcp47, 1, 0x4E2D
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3231
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 231)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, 1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_MatchFamilyStyleCharacter_3232
DEF_DTK(fontmgr_matchfamilystylecharater_4, TestLevel::L1, 232)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";

    Drawing::FontStyle fontStyle = GetFontStyle(fontMgr, false);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, 1, 0x74
    ));

    DrawTexts(playbackCanvas_, typeface);
}

}
}
