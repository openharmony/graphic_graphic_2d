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
#include "modules/2d_graphics/include/text/font.h"
#include "modules/2d_graphics/include/recording/mem_allocator.h"
#include "modules/2d_graphics/include/text/font_mgr.h"
#include "modules/2d_graphics/include/text/font_style_set.h"
#include "modules/2d_graphics/include/text/rs_xform.h"
#include "modules/2d_graphics/include/utils/point.h"
#include "modules/2d_graphics/include/utils/rect.h"
#include "modules/2d_graphics/include/text/typeface.h"

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

//对应用例 FontMgr_LoadThemeFont1_3193
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 193)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3194
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 194)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3195
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 195)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
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

//对应用例 FontMgr_LoadThemeFont1_3196
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 196)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
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

//对应用例 FontMgr_LoadThemeFont1_3197
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 197)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
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

//对应用例 FontMgr_LoadThemeFont1_3198
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 198)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3199
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 199)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3200
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 200)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3201
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 201)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3202
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 202)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3203
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 203)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3204
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 204)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
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

//对应用例 FontMgr_LoadThemeFont1_3205
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 205)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3206
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 206)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3207
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 207)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3208
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 208)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3209
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 209)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3210
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 210)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3211
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 211)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3212
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 212)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
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

//对应用例 FontMgr_LoadThemeFont1_3213
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 213)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
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

//对应用例 FontMgr_LoadThemeFont1_3214
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 214)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3215
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 215)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3216
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 216)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3217
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 217)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3218
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 218)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
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

//对应用例 FontMgr_LoadThemeFont1_3219
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 219)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3220
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 220)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3221
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 221)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3222
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 222)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
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

//对应用例 FontMgr_LoadThemeFont1_3223
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 223)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3224
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 224)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
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

//对应用例 FontMgr_LoadThemeFont1_3225
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 225)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3226
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 226)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3227
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 227)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
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

//对应用例 FontMgr_LoadThemeFont1_3228
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 228)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3229
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 229)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
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

//对应用例 FontMgr_LoadThemeFont1_3230
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 230)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
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

//对应用例 FontMgr_LoadThemeFont1_3231
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 231)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
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

//对应用例 FontMgr_LoadThemeFont1_3232
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 232)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
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

//对应用例 FontMgr_LoadThemeFont1_3233
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 233)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3234
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 234)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3235
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 235)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3236
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 236)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3237
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 237)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3238
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 238)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3239
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 239)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
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

//对应用例 FontMgr_LoadThemeFont1_3240
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 240)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3241
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 241)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3242
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 242)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3243
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 243)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3244
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 244)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
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

//对应用例 FontMgr_LoadThemeFont1_3245
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 245)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3246
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 246)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3247
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 247)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3248
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 248)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3249
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 249)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3250
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 250)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3251
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 251)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
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

//对应用例 FontMgr_LoadThemeFont1_3252
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 252)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
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

//对应用例 FontMgr_LoadThemeFont1_3253
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 253)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
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

//对应用例 FontMgr_LoadThemeFont1_3254
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 254)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3255
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 255)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3256
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 256)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3257
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 257)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3258
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 258)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3259
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 259)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
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

//对应用例 FontMgr_LoadThemeFont1_3260
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 260)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3261
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 261)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
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

//对应用例 FontMgr_LoadThemeFont1_3262
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 262)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3263
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 263)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
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

//对应用例 FontMgr_LoadThemeFont1_3264
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 264)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3265
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 265)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3266
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 266)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
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

//对应用例 FontMgr_LoadThemeFont1_3267
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 267)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3268
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 268)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3269
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 269)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3270
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 270)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3271
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 271)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3272
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 272)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3273
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 273)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3274
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 274)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3275
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 275)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3276
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 276)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3277
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 277)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
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

//对应用例 FontMgr_LoadThemeFont1_3278
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 278)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
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

//对应用例 FontMgr_LoadThemeFont1_3279
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 279)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
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

//对应用例 FontMgr_LoadThemeFont1_3280
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 280)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "";
    fontMgr->GetFamilyName(0, familyName);
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), 0
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3281
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 281)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, nullptr, data1->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3282
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 282)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3283
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 283)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "";
    fontMgr->GetFamilyName(0, themeName);

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3284
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 284)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
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

//对应用例 FontMgr_LoadThemeFont1_3285
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 285)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "abcd";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3286
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 286)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "abcd";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)emoji_data->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

//对应用例 FontMgr_LoadThemeFont1_3287
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 287)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
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

//对应用例 FontMgr_LoadThemeFont1_3288
DEF_DTK(fontmgr_loadthemefont1_3, TestLevel::L1, 288)
{
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::string themeName = "HMOS Color Emoji";

    auto emoji_data = MakeData(fontMgr, false);
    auto data1 = MakeData(fontMgr, true);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->LoadThemeFont(
        familyName, themeName, std::move((uint8_t *)data1->GetData()), emoji_data->GetSize()
    ));

    DrawTexts(playbackCanvas_, typeface);
}

}
}
