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
测试接口：LoadDynamicFont
测试内容：对接口入参familyName、data和size取值组合，构造typeface字体格式，并指定在font上，最终通过drawtextblob接口将text内容绘制在画布上
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

//对应用例 FontMgr_LoadDynamicFont_3044
DEF_DTK(fontmgr_loaddynamicfont_2, TestLevel::L1, 44)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
	
    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadDynamicFont(
        familyName, std::move((uint8_t *)data1->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadDynamicFont_3045
DEF_DTK(fontmgr_loaddynamicfont_2, TestLevel::L1, 45)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
	
    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadDynamicFont(
        familyName, std::move((uint8_t *)emoji_data->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadDynamicFont_3046
DEF_DTK(fontmgr_loaddynamicfont_2, TestLevel::L1, 46)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "abcd";
	
    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadDynamicFont(
        familyName, nullptr, data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadDynamicFont_3047
DEF_DTK(fontmgr_loaddynamicfont_2, TestLevel::L1, 47)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
	
    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadDynamicFont(
        familyName, nullptr, emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadDynamicFont_3048
DEF_DTK(fontmgr_loaddynamicfont_2, TestLevel::L1, 48)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
	
    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadDynamicFont(
        familyName, std::move((uint8_t *)data1->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadDynamicFont_3049
DEF_DTK(fontmgr_loaddynamicfont_2, TestLevel::L1, 49)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
	
    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadDynamicFont(
        familyName, std::move((uint8_t *)data1->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadDynamicFont_3050
DEF_DTK(fontmgr_loaddynamicfont_2, TestLevel::L1, 50)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
	
    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadDynamicFont(
        familyName, std::move((uint8_t *)data1->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadDynamicFont_3051
DEF_DTK(fontmgr_loaddynamicfont_2, TestLevel::L1, 51)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
	
    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadDynamicFont(
        familyName, std::move((uint8_t *)emoji_data->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadDynamicFont_3052
DEF_DTK(fontmgr_loaddynamicfont_2, TestLevel::L1, 52)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
	
    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadDynamicFont(
        familyName, std::move((uint8_t *)emoji_data->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadDynamicFont_3053
DEF_DTK(fontmgr_loaddynamicfont_2, TestLevel::L1, 53)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
	
    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadDynamicFont(
        familyName, nullptr, 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadDynamicFont_3054
DEF_DTK(fontmgr_loaddynamicfont_2, TestLevel::L1, 54)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "abcd";
	
    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadDynamicFont(
        familyName, nullptr, emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadDynamicFont_3055
DEF_DTK(fontmgr_loaddynamicfont_2, TestLevel::L1, 55)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "abcd";
	
    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadDynamicFont(
        familyName, std::move((uint8_t *)emoji_data->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadDynamicFont_3056
DEF_DTK(fontmgr_loaddynamicfont_2, TestLevel::L1, 56)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
	
    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadDynamicFont(
        familyName, std::move((uint8_t *)emoji_data->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadDynamicFont_3057
DEF_DTK(fontmgr_loaddynamicfont_2, TestLevel::L1, 57)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
	
    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadDynamicFont(
        familyName, std::move((uint8_t *)emoji_data->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadDynamicFont_3058
DEF_DTK(fontmgr_loaddynamicfont_2, TestLevel::L1, 58)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
	
    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadDynamicFont(
        familyName, nullptr, data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadDynamicFont_3059
DEF_DTK(fontmgr_loaddynamicfont_2, TestLevel::L1, 59)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
	
    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadDynamicFont(
        familyName, std::move((uint8_t *)emoji_data->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadDynamicFont_3060
DEF_DTK(fontmgr_loaddynamicfont_2, TestLevel::L1, 60)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
	
    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadDynamicFont(
        familyName, std::move((uint8_t *)emoji_data->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadDynamicFont_3061
DEF_DTK(fontmgr_loaddynamicfont_2, TestLevel::L1, 61)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
	
    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadDynamicFont(
        familyName, nullptr, emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadDynamicFont_3062
DEF_DTK(fontmgr_loaddynamicfont_2, TestLevel::L1, 62)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "abcd";
	
    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadDynamicFont(
        familyName, std::move((uint8_t *)data1->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadDynamicFont_3063
DEF_DTK(fontmgr_loaddynamicfont_2, TestLevel::L1, 63)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
	
    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadDynamicFont(
        familyName, nullptr, emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadDynamicFont_3064
DEF_DTK(fontmgr_loaddynamicfont_2, TestLevel::L1, 64)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
	
    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadDynamicFont(
        familyName, nullptr, data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadDynamicFont_3065
DEF_DTK(fontmgr_loaddynamicfont_2, TestLevel::L1, 65)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
	
    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadDynamicFont(
        familyName, std::move((uint8_t *)data1->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadDynamicFont_3066
DEF_DTK(fontmgr_loaddynamicfont_2, TestLevel::L1, 66)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
	
    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadDynamicFont(
        familyName, std::move((uint8_t *)emoji_data->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadDynamicFont_3067
DEF_DTK(fontmgr_loaddynamicfont_2, TestLevel::L1, 67)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
	
    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadDynamicFont(
        familyName, std::move((uint8_t *)emoji_data->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadDynamicFont_3068
DEF_DTK(fontmgr_loaddynamicfont_2, TestLevel::L1, 68)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
	
    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadDynamicFont(
        familyName, std::move((uint8_t *)emoji_data->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadDynamicFont_3069
DEF_DTK(fontmgr_loaddynamicfont_2, TestLevel::L1, 69)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
	
    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadDynamicFont(
        familyName, nullptr, emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadDynamicFont_3070
DEF_DTK(fontmgr_loaddynamicfont_2, TestLevel::L1, 70)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "abcd";
	
    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadDynamicFont(
        familyName, std::move((uint8_t *)emoji_data->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadDynamicFont_3071
DEF_DTK(fontmgr_loaddynamicfont_2, TestLevel::L1, 71)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
	
    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadDynamicFont(
        familyName, nullptr, 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadDynamicFont_3072
DEF_DTK(fontmgr_loaddynamicfont_2, TestLevel::L1, 72)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
	
    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);
	
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadDynamicFont(
        familyName, std::move((uint8_t *)data1->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

}
}
