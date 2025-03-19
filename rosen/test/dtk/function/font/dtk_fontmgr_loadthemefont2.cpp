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
#include "dtk_test_ext.h"
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
测试接口：LoadThemeFont
测试内容：对接口入参themeName和typeface取值组合，构造typeface字体格式，并指定在font上，最终通过drawtextblob接口将text内容绘制在画布上
*/

namespace OHOS {
namespace Rosen {

static void DrawTexts(TestPlaybackCanvas* playbackCanvas, std::shared_ptr<Drawing::Typeface>& emoji_typeface)
{
    auto font = Drawing::Font(emoji_typeface, 50.f, 1.0f, 0.f);
    std::string text1 = "DDGR ddgr 鸿蒙 !@#%￥^&*;：，。";
    std::string text2 = "-_=+()123`.---~|{}【】,./?、？<>《》";
    std::string text3 = "\xE2\x99\x88\xE2\x99\x8A\xE2\x99\x88\xE2\x99\x8C\xE2\x99\x8D\xE2\x99\x8D";
    std::string texts[] = {text1, text2, text3};
    int line = 200;
    int interval2 = 200;
    for (auto text :texts) {
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

//对应用例 FontMgr_LoadThemeFont2_3001
DEF_DTK(fontmgr_loadthemefont2, TestLevel::L1, 1)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "abcd";
    std::string emoji_familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(emoji_familyName.c_str()));
    auto emoji_typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    fontMgr->LoadThemeFont(themeName, emoji_typeface);

    DrawTexts(playbackCanvas_, emoji_typeface);
}

//对应用例 FontMgr_LoadThemeFont2_3002
DEF_DTK(fontmgr_loadthemefont2, TestLevel::L1, 2)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "abcd";
    std::string emoji_familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(emoji_familyName.c_str()));
    auto emoji_typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    fontMgr->LoadThemeFont(themeName, emoji_typeface);

    DrawTexts(playbackCanvas_, emoji_typeface);
}

//对应用例 FontMgr_LoadThemeFont2_3003
DEF_DTK(fontmgr_loadthemefont2, TestLevel::L1, 3)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);
    std::string emoji_familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(emoji_familyName.c_str()));
    auto emoji_typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    fontMgr->LoadThemeFont(themeName, emoji_typeface);

    DrawTexts(playbackCanvas_, emoji_typeface);
}

//对应用例 FontMgr_LoadThemeFont2_3004
DEF_DTK(fontmgr_loadthemefont2, TestLevel::L1, 4)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);
    std::string emoji_familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    auto emoji_typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    fontMgr->LoadThemeFont(themeName, emoji_typeface);

    DrawTexts(playbackCanvas_, emoji_typeface);
}

//对应用例 FontMgr_LoadThemeFont2_3005
DEF_DTK(fontmgr_loadthemefont2, TestLevel::L1, 5)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "HMOS Color Emoji";
    std::string emoji_familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(emoji_familyName.c_str()));
    auto emoji_typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    fontMgr->LoadThemeFont(themeName, nullptr);

    DrawTexts(playbackCanvas_, emoji_typeface);
}

//对应用例 FontMgr_LoadThemeFont2_3006
DEF_DTK(fontmgr_loadthemefont2, TestLevel::L1, 6)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "abcd";
    std::string emoji_familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(emoji_familyName.c_str()));
    auto emoji_typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    fontMgr->LoadThemeFont(themeName, nullptr);

    DrawTexts(playbackCanvas_, emoji_typeface);
}

//对应用例 FontMgr_LoadThemeFont2_3007
DEF_DTK(fontmgr_loadthemefont2, TestLevel::L1, 7)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);
    std::string emoji_familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(emoji_familyName.c_str()));
    auto emoji_typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    fontMgr->LoadThemeFont(themeName, nullptr);

    DrawTexts(playbackCanvas_, emoji_typeface);
}

//对应用例 FontMgr_LoadThemeFont2_3008
DEF_DTK(fontmgr_loadthemefont2, TestLevel::L1, 8)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "HMOS Color Emoji";
    std::string emoji_familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    auto emoji_typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    fontMgr->LoadThemeFont(themeName, emoji_typeface);

    DrawTexts(playbackCanvas_, emoji_typeface);
}

//对应用例 FontMgr_LoadThemeFont2_3009
DEF_DTK(fontmgr_loadthemefont2, TestLevel::L1, 9)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";
    std::string emoji_familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    auto emoji_typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    fontMgr->LoadThemeFont(themeName, emoji_typeface);

    DrawTexts(playbackCanvas_, emoji_typeface);
}

//对应用例 FontMgr_LoadThemeFont2_3010
DEF_DTK(fontmgr_loadthemefont2, TestLevel::L1, 10)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";
    std::string emoji_familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(emoji_familyName.c_str()));
    auto emoji_typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    fontMgr->LoadThemeFont(themeName, nullptr);

    DrawTexts(playbackCanvas_, emoji_typeface);
}

//对应用例 FontMgr_LoadThemeFont2_3011
DEF_DTK(fontmgr_loadthemefont2, TestLevel::L1, 11)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "HMOS Color Emoji";
    std::string emoji_familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    auto emoji_typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    fontMgr->LoadThemeFont(themeName, emoji_typeface);

    DrawTexts(playbackCanvas_, emoji_typeface);
}

//对应用例 FontMgr_LoadThemeFont2_3012
DEF_DTK(fontmgr_loadthemefont2, TestLevel::L1, 12)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";
    std::string emoji_familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(emoji_familyName.c_str()));
    auto emoji_typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    fontMgr->LoadThemeFont(themeName, emoji_typeface);

    DrawTexts(playbackCanvas_, emoji_typeface);
}

//对应用例 FontMgr_LoadThemeFont2_3013
DEF_DTK(fontmgr_loadthemefont2, TestLevel::L1, 13)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "abcd";
    std::string emoji_familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    auto emoji_typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    fontMgr->LoadThemeFont(themeName, emoji_typeface);

    DrawTexts(playbackCanvas_, emoji_typeface);
}

//对应用例 FontMgr_LoadThemeFont2_3014
DEF_DTK(fontmgr_loadthemefont2, TestLevel::L1, 14)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";
    std::string emoji_familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(emoji_familyName.c_str()));
    auto emoji_typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    fontMgr->LoadThemeFont(themeName, emoji_typeface);

    DrawTexts(playbackCanvas_, emoji_typeface);
}

//对应用例 FontMgr_LoadThemeFont2_3015
DEF_DTK(fontmgr_loadthemefont2, TestLevel::L1, 15)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "HMOS Color Emoji";
    std::string emoji_familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(emoji_familyName.c_str()));
    auto emoji_typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    fontMgr->LoadThemeFont(themeName, nullptr);

    DrawTexts(playbackCanvas_, emoji_typeface);
}

//对应用例 FontMgr_LoadThemeFont2_3016
DEF_DTK(fontmgr_loadthemefont2, TestLevel::L1, 16)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);
    std::string emoji_familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    auto emoji_typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    fontMgr->LoadThemeFont(themeName, emoji_typeface);

    DrawTexts(playbackCanvas_, emoji_typeface);
}

//对应用例 FontMgr_LoadThemeFont2_3017
DEF_DTK(fontmgr_loadthemefont2, TestLevel::L1, 17)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "abcd";
    std::string emoji_familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    auto emoji_typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    fontMgr->LoadThemeFont(themeName, emoji_typeface);

    DrawTexts(playbackCanvas_, emoji_typeface);
}

//对应用例 FontMgr_LoadThemeFont2_3018
DEF_DTK(fontmgr_loadthemefont2, TestLevel::L1, 18)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";
    std::string emoji_familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    auto emoji_typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    fontMgr->LoadThemeFont(themeName, emoji_typeface);

    DrawTexts(playbackCanvas_, emoji_typeface);
}

//对应用例 FontMgr_LoadThemeFont2_3019
DEF_DTK(fontmgr_loadthemefont2, TestLevel::L1, 19)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);
    std::string emoji_familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(emoji_familyName.c_str()));
    auto emoji_typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    fontMgr->LoadThemeFont(themeName, nullptr);

    DrawTexts(playbackCanvas_, emoji_typeface);
}

//对应用例 FontMgr_LoadThemeFont2_3020
DEF_DTK(fontmgr_loadthemefont2, TestLevel::L1, 20)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);
    std::string emoji_familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(emoji_familyName.c_str()));
    auto emoji_typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    fontMgr->LoadThemeFont(themeName, emoji_typeface);

    DrawTexts(playbackCanvas_, emoji_typeface);
}

//对应用例 FontMgr_LoadThemeFont2_3021
DEF_DTK(fontmgr_loadthemefont2, TestLevel::L1, 21)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string themeName = "HMOS Color Emoji";
    std::string emoji_familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(emoji_familyName.c_str()));
    auto emoji_typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    fontMgr->LoadThemeFont(themeName, emoji_typeface);

    DrawTexts(playbackCanvas_, emoji_typeface);
}

//对应用例 FontMgr_LoadThemeFont2_3022
DEF_DTK(fontmgr_loadthemefont2, TestLevel::L1, 22)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "";
    std::string emoji_familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(emoji_familyName.c_str()));
    auto emoji_typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    fontMgr->LoadThemeFont(themeName, nullptr);

    DrawTexts(playbackCanvas_, emoji_typeface);
}

//对应用例 FontMgr_LoadThemeFont2_3023
DEF_DTK(fontmgr_loadthemefont2, TestLevel::L1, 23)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "abcd";
    std::string emoji_familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(emoji_familyName.c_str()));
    auto emoji_typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    fontMgr->LoadThemeFont(themeName, nullptr);

    DrawTexts(playbackCanvas_, emoji_typeface);
}

//对应用例 FontMgr_LoadThemeFont2_3024
DEF_DTK(fontmgr_loadthemefont2, TestLevel::L1, 24)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string themeName = "HMOS Color Emoji";
    std::string emoji_familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(emoji_familyName.c_str()));
    auto emoji_typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    fontMgr->LoadThemeFont(themeName, emoji_typeface);

    DrawTexts(playbackCanvas_, emoji_typeface);
}

}
}
