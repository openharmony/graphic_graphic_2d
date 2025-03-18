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
#include "rosen/test/dtk/dtk_test_ext.h"
#include "rosen/modules/2d_graphics/include/text/font.h"
#include "rosen/modules/2d_graphics/include/recording/mem_allocator.h"
#include "rosen/modules/2d_graphics/include/text/font_mgr.h"
#include "rosen/modules/2d_graphics/include/text/font_style_set.h"
#include "rosen/modules/2d_graphics/include/text/rs_xform.h"
#include "rosen/modules/2d_graphics/include/utils/point.h"
#include "rosen/modules/2d_graphics/include/utils/rect.h"
#include "rosen/modules/2d_graphics/include/text/typeface.h"

/*
测试类：FontMgr
测试接口：LoadThemeFont
测试内容：对接口入参familyName、themeName、data和size取值组合，构造typeface字体格式，并指定在font上，最终通过drawtextblob接口将text内容绘制在画布上
*/
namespace OHOS {
namespace Rosen {

static std::shared_ptr<Drawing::Data> MakeData(std::shared_ptr<Drawing::FontMgr>& fontMgr, bool createstyleset)
{
    std::shared_ptr<Drawing::Typeface> typeface;

    if (createstyleset) {
        std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
        typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    } else {
        std::string emoji_familyName = "HMOS Color Emoji";
        std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(emoji_familyName.c_str()));
        typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    }

    auto data = typeface->Serialize();
    return data;
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

//对应用例 FontMgr_LoadThemeFont1_3097
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 97)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3098
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 98)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3099
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 99)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3100
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 100)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3101
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 101)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3102
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 102)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3103
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 103)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3104
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 104)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3105
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 105)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3106
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 106)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3107
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 107)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3108
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 108)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3109
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 109)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3110
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 110)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3111
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 111)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3112
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 112)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3113
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 113)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3114
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 114)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3115
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 115)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3116
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 116)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3117
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 117)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3118
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 118)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3119
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 119)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3120
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 120)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3121
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 121)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3122
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 122)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3123
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 123)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3124
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 124)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3125
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 125)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3126
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 126)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3127
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 127)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3128
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 128)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3129
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 129)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3130
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 130)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3131
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 131)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3132
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 132)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3133
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 133)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3134
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 134)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3135
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 135)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3136
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 136)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3137
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 137)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3138
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 138)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3139
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 139)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3140
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 140)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3141
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 141)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3142
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 142)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3143
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 143)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3144
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 144)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3145
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 145)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3146
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 146)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3147
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 147)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3148
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 148)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3149
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 149)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3150
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 150)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3151
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 151)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3152
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 152)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3153
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 153)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3154
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 154)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3155
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 155)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3156
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 156)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3157
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 157)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3158
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 158)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3159
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 159)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3160
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 160)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3161
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 161)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3162
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 162)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3163
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 163)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3164
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 164)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3165
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 165)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3166
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 166)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3167
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 167)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3168
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 168)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3169
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 169)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3170
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 170)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3171
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 171)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3172
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 172)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3173
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 173)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3174
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 174)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3175
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 175)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3176
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 176)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3177
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 177)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3178
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 178)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3179
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 179)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3180
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 180)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3181
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 181)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3182
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 182)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3183
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 183)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3184
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 184)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3185
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 185)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3186
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 186)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3187
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 187)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3188
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 188)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3189
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 189)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3190
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 190)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3191
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 191)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3192
DEF_DTK(fontmgr_loadthemefont1_2, TestLevel::L1, 192)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

}
}
