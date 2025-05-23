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

//对应用例 FontMgr_LoadThemeFont1_3001
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 1)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
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

//对应用例 FontMgr_LoadThemeFont1_3002
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 2)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3003
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 3)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3004
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 4)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3005
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 5)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3006
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 6)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3007
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 7)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3008
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 8)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
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

//对应用例 FontMgr_LoadThemeFont1_3009
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 9)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3010
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 10)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
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

//对应用例 FontMgr_LoadThemeFont1_3011
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 11)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3012
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 12)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3013
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 13)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3014
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 14)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3015
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 15)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3016
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 16)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3017
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 17)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3018
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 18)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3019
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 19)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3020
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 20)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3021
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 21)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3022
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 22)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3023
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 23)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3024
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 24)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3025
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 25)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3026
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 26)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
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

//对应用例 FontMgr_LoadThemeFont1_3027
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 27)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
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

//对应用例 FontMgr_LoadThemeFont1_3028
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 28)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
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

//对应用例 FontMgr_LoadThemeFont1_3029
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 29)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3030
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 30)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3031
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 31)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3032
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 32)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3033
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 33)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3034
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 34)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3035
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 35)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
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

//对应用例 FontMgr_LoadThemeFont1_3036
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 36)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3037
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 37)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3038
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 38)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3039
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 39)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3040
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 40)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3041
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 41)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
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

//对应用例 FontMgr_LoadThemeFont1_3042
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 42)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3043
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 43)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3044
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 44)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
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

//对应用例 FontMgr_LoadThemeFont1_3045
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 45)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3046
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 46)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3047
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 47)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3048
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 48)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3049
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 49)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
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

//对应用例 FontMgr_LoadThemeFont1_3050
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 50)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3051
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 51)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3052
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 52)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3053
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 53)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3054
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 54)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
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

//对应用例 FontMgr_LoadThemeFont1_3055
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 55)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
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

//对应用例 FontMgr_LoadThemeFont1_3056
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 56)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3057
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 57)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3058
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 58)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3059
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 59)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3060
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 60)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3061
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 61)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3062
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 62)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3063
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 63)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3064
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 64)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3065
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 65)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
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

//对应用例 FontMgr_LoadThemeFont1_3066
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 66)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3067
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 67)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3068
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 68)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3069
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 69)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3070
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 70)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3071
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 71)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3072
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 72)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3073
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 73)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3074
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 74)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3075
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 75)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
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

//对应用例 FontMgr_LoadThemeFont1_3076
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 76)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3077
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 77)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3078
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 78)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
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

//对应用例 FontMgr_LoadThemeFont1_3079
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 79)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3080
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 80)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3081
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 81)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3082
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 82)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3083
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 83)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3084
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 84)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3085
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 85)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3086
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 86)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3087
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 87)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3088
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 88)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3089
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 89)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3090
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 90)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3091
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 91)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3092
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 92)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3093
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 93)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3094
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 94)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
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

//对应用例 FontMgr_LoadThemeFont1_3095
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 95)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3096
DEF_DTK(fontmgr_loadthemefont1_1, TestLevel::L1, 96)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
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

}
}
