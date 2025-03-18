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
测试类：FontStyleSet
测试接口：Matchstyle
测试内容：对接口入参familynam去string类型的等价值有效值等，构造typeface字体格式，并指定在font上，最终通过drawtextblob接口将text内容绘制在画布上
*/

namespace OHOS {
namespace Rosen {

void DrawFontStylesetmatchstyle(std::shared_ptr<Drawing::FontMgr> font_mgr,
    std::string name, TestPlaybackCanvas* playbackCanvas,
    Drawing::FontStyle::Weight a, Drawing::FontStyle::Width b, Drawing::FontStyle::Slant c)
{
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(
        name.empty()
            ? font_mgr->CreateStyleSet(0)
            : font_mgr->MatchFamily(name.c_str())
    );
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->MatchStyle({
    a,
    b,
    c}
    ));

    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);
    std::string text1 = "DDGR ddgr 鸿蒙 !@#%￥^&*;：，。";
    std::string text2 = "-_=+()123`.---~|{}【】,./?、？<>《》";
    std::string text3 = "\xE2\x99\x88\xE2\x99\x8A\xE2\x99\x88\xE2\x99\x8C\xE2\x99\x8D\xE2\x99\x8D";
    std::string texts[] = { text1, text2, text3 };
    int interval = 200;
    int line = 200;
    for (auto text :texts) {
        std::shared_ptr<Drawing::TextBlob> textBlob =
            Drawing::TextBlob::MakeFromText(text.c_str(), text.size(), font);
        Drawing::Brush brush;
        playbackCanvas->AttachBrush(brush);
        playbackCanvas->DrawTextBlob(textBlob.get(), interval, line);
        line += interval;
        playbackCanvas->DetachBrush();
        Drawing::Pen pen;
        playbackCanvas->AttachPen(pen);
        playbackCanvas->DrawTextBlob(textBlob.get(), interval, line);
        line += interval;
        playbackCanvas->DetachPen();
    }
}

//对应用例 FontStyleSet_MatchStyle_3001
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 1)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDefaultFontMgr(), "HarmonyOS Sans",
        playbackCanvas_, Drawing::FontStyle::INVISIBLE_WEIGHT, Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3002
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 2)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDefaultFontMgr(), "HarmonyOS Sans",
        playbackCanvas_, Drawing::FontStyle::INVISIBLE_WEIGHT, Drawing::FontStyle::CONDENSED_WIDTH,
        Drawing::FontStyle::UPRIGHT_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3003
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 3)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDefaultFontMgr(), "HarmonyOS Sans",
        playbackCanvas_, Drawing::FontStyle::INVISIBLE_WEIGHT, Drawing::FontStyle::NORMAL_WIDTH,
        Drawing::FontStyle::ITALIC_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3004
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 4)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDefaultFontMgr(), "HarmonyOS Sans",
        playbackCanvas_, Drawing::FontStyle::INVISIBLE_WEIGHT, Drawing::FontStyle::EXPANDED_WIDTH,
        Drawing::FontStyle::UPRIGHT_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3005
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 5)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDefaultFontMgr(), "HarmonyOS Sans",
        playbackCanvas_, Drawing::FontStyle::INVISIBLE_WEIGHT, Drawing::FontStyle::ULTRA_EXPANDED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3006
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 6)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDefaultFontMgr(), "HarmonyOS Sans",
        playbackCanvas_, Drawing::FontStyle::THIN_WEIGHT, Drawing::FontStyle::CONDENSED_WIDTH,
        Drawing::FontStyle::UPRIGHT_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3007
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 7)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDefaultFontMgr(), "HarmonyOS Sans",
        playbackCanvas_, Drawing::FontStyle::THIN_WEIGHT, Drawing::FontStyle::NORMAL_WIDTH,
        Drawing::FontStyle::ITALIC_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3008
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 8)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDefaultFontMgr(), "HarmonyOS Sans",
        playbackCanvas_, Drawing::FontStyle::THIN_WEIGHT, Drawing::FontStyle::EXPANDED_WIDTH,
        Drawing::FontStyle::ITALIC_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3009
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 9)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDefaultFontMgr(), "HarmonyOS Sans",
        playbackCanvas_, Drawing::FontStyle::THIN_WEIGHT, Drawing::FontStyle::EXTRA_EXPANDED_WIDTH,
        Drawing::FontStyle::ITALIC_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3010
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 10)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDefaultFontMgr(), "HarmonyOS Sans",
        playbackCanvas_, Drawing::FontStyle::EXTRA_LIGHT_WEIGHT, Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::ITALIC_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3011
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 11)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDefaultFontMgr(), "HarmonyOS Sans",
        playbackCanvas_, Drawing::FontStyle::EXTRA_LIGHT_WEIGHT, Drawing::FontStyle::CONDENSED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3012
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 12)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDefaultFontMgr(), "HarmonyOS Sans",
        playbackCanvas_, Drawing::FontStyle::EXTRA_LIGHT_WEIGHT, Drawing::FontStyle::NORMAL_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3013
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 13)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDefaultFontMgr(), "HarmonyOS Sans",
        playbackCanvas_, Drawing::FontStyle::EXTRA_LIGHT_WEIGHT, Drawing::FontStyle::EXPANDED_WIDTH,
        Drawing::FontStyle::ITALIC_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3014
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 14)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDefaultFontMgr(), "HarmonyOS Sans",
        playbackCanvas_, Drawing::FontStyle::EXTRA_LIGHT_WEIGHT, Drawing::FontStyle::ULTRA_EXPANDED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3015
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 15)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDefaultFontMgr(), "HarmonyOS Sans",
        playbackCanvas_, Drawing::FontStyle::LIGHT_WEIGHT, Drawing::FontStyle::EXTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::UPRIGHT_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3016
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 16)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDefaultFontMgr(), "HarmonyOS Sans",
        playbackCanvas_, Drawing::FontStyle::LIGHT_WEIGHT, Drawing::FontStyle::SEMI_EXPANDED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3017
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 17)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDefaultFontMgr(), "HarmonyOS Sans",
        playbackCanvas_, Drawing::FontStyle::LIGHT_WEIGHT, Drawing::FontStyle::EXTRA_EXPANDED_WIDTH,
        Drawing::FontStyle::ITALIC_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3018
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 18)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDefaultFontMgr(), "HarmonyOS Sans",
        playbackCanvas_, Drawing::FontStyle::NORMAL_WEIGHT, Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::UPRIGHT_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3019
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 19)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDefaultFontMgr(), "HarmonyOS Sans",
        playbackCanvas_, Drawing::FontStyle::NORMAL_WEIGHT, Drawing::FontStyle::CONDENSED_WIDTH,
        Drawing::FontStyle::ITALIC_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3020
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 20)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDefaultFontMgr(), "HarmonyOS Sans",
        playbackCanvas_, Drawing::FontStyle::NORMAL_WEIGHT, Drawing::FontStyle::SEMI_EXPANDED_WIDTH,
        Drawing::FontStyle::UPRIGHT_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3021
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 21)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDefaultFontMgr(), "HarmonyOS Sans",
        playbackCanvas_, Drawing::FontStyle::NORMAL_WEIGHT, Drawing::FontStyle::EXTRA_EXPANDED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3022
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 22)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDefaultFontMgr(), "HarmonyOS Sans",
        playbackCanvas_, Drawing::FontStyle::MEDIUM_WEIGHT, Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::UPRIGHT_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3023
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 23)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDefaultFontMgr(), "HarmonyOS Sans",
        playbackCanvas_, Drawing::FontStyle::MEDIUM_WEIGHT, Drawing::FontStyle::CONDENSED_WIDTH,
        Drawing::FontStyle::UPRIGHT_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3024
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 24)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDefaultFontMgr(), "HarmonyOS Sans",
        playbackCanvas_, Drawing::FontStyle::MEDIUM_WEIGHT, Drawing::FontStyle::NORMAL_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3025
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 25)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDefaultFontMgr(), "HarmonyOS Sans",
        playbackCanvas_, Drawing::FontStyle::MEDIUM_WEIGHT, Drawing::FontStyle::EXTRA_EXPANDED_WIDTH,
        Drawing::FontStyle::ITALIC_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3026
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 26)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDefaultFontMgr(), "HarmonyOS Sans",
        playbackCanvas_, Drawing::FontStyle::SEMI_BOLD_WEIGHT, Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::UPRIGHT_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3027
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 27)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDefaultFontMgr(), "HarmonyOS Sans",
        playbackCanvas_, Drawing::FontStyle::SEMI_BOLD_WEIGHT, Drawing::FontStyle::EXTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::ITALIC_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3028
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 28)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDefaultFontMgr(), "HarmonyOS Sans",
        playbackCanvas_, Drawing::FontStyle::SEMI_BOLD_WEIGHT, Drawing::FontStyle::SEMI_CONDENSED_WIDTH,
        Drawing::FontStyle::ITALIC_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3029
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 29)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDefaultFontMgr(), "HarmonyOS Sans",
        playbackCanvas_, Drawing::FontStyle::SEMI_BOLD_WEIGHT, Drawing::FontStyle::SEMI_EXPANDED_WIDTH,
        Drawing::FontStyle::UPRIGHT_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3030
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 30)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDefaultFontMgr(), "HarmonyOS Sans",
        playbackCanvas_, Drawing::FontStyle::SEMI_BOLD_WEIGHT, Drawing::FontStyle::ULTRA_EXPANDED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3031
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 31)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDefaultFontMgr(), "HarmonyOS Sans",
        playbackCanvas_, Drawing::FontStyle::BOLD_WEIGHT, Drawing::FontStyle::EXTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::UPRIGHT_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3032
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 32)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDefaultFontMgr(), "HarmonyOS Sans",
        playbackCanvas_, Drawing::FontStyle::BOLD_WEIGHT, Drawing::FontStyle::NORMAL_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3033
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 33)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDefaultFontMgr(), "HarmonyOS Sans",
        playbackCanvas_, Drawing::FontStyle::BOLD_WEIGHT, Drawing::FontStyle::SEMI_EXPANDED_WIDTH,
        Drawing::FontStyle::UPRIGHT_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3034
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 34)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDefaultFontMgr(), "HarmonyOS Sans",
        playbackCanvas_, Drawing::FontStyle::BOLD_WEIGHT, Drawing::FontStyle::EXTRA_EXPANDED_WIDTH,
        Drawing::FontStyle::ITALIC_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3035
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 35)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDefaultFontMgr(), "HarmonyOS Sans",
        playbackCanvas_, Drawing::FontStyle::EXTRA_BOLD_WEIGHT, Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3036
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 36)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDefaultFontMgr(), "HarmonyOS Sans",
        playbackCanvas_, Drawing::FontStyle::EXTRA_BOLD_WEIGHT, Drawing::FontStyle::CONDENSED_WIDTH,
        Drawing::FontStyle::ITALIC_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3037
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 37)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDefaultFontMgr(), "HarmonyOS Sans",
        playbackCanvas_, Drawing::FontStyle::EXTRA_BOLD_WEIGHT, Drawing::FontStyle::SEMI_CONDENSED_WIDTH,
        Drawing::FontStyle::ITALIC_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3038
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 38)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDefaultFontMgr(), "HarmonyOS Sans",
        playbackCanvas_, Drawing::FontStyle::EXTRA_BOLD_WEIGHT, Drawing::FontStyle::EXPANDED_WIDTH,
        Drawing::FontStyle::UPRIGHT_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3040
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 40)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDefaultFontMgr(), "HarmonyOS Sans",
        playbackCanvas_, Drawing::FontStyle::BLACK_WEIGHT, Drawing::FontStyle::EXTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::UPRIGHT_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3041
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 41)
{
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS Sans";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->MatchStyle({
        Drawing::FontStyle::BLACK_WEIGHT,
        Drawing::FontStyle::SEMI_CONDENSED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT}
    ));

    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDefaultFontMgr(), "HarmonyOS Sans",
        playbackCanvas_, Drawing::FontStyle::BOLD_WEIGHT, Drawing::FontStyle::EXTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::UPRIGHT_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3042
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 42)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDefaultFontMgr(), "HarmonyOS Sans",
        playbackCanvas_, Drawing::FontStyle::BLACK_WEIGHT, Drawing::FontStyle::NORMAL_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3043
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 43)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDefaultFontMgr(), "HarmonyOS Sans",
        playbackCanvas_, Drawing::FontStyle::BLACK_WEIGHT, Drawing::FontStyle::EXPANDED_WIDTH,
        Drawing::FontStyle::ITALIC_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3044
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 44)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDefaultFontMgr(), "HarmonyOS Sans",
        playbackCanvas_, Drawing::FontStyle::EXTRA_BLACK_WEIGHT, Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::ITALIC_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3045
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 45)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDefaultFontMgr(), "HarmonyOS Sans",
        playbackCanvas_, Drawing::FontStyle::EXTRA_BLACK_WEIGHT, Drawing::FontStyle::CONDENSED_WIDTH,
        Drawing::FontStyle::ITALIC_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3046
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 46)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDefaultFontMgr(), "HarmonyOS Sans",
        playbackCanvas_, Drawing::FontStyle::EXTRA_BLACK_WEIGHT, Drawing::FontStyle::SEMI_EXPANDED_WIDTH,
        Drawing::FontStyle::UPRIGHT_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3047
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 47)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDefaultFontMgr(), "HarmonyOS Sans",
        playbackCanvas_, Drawing::FontStyle::EXTRA_BLACK_WEIGHT, Drawing::FontStyle::EXTRA_EXPANDED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3048
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 48)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDefaultFontMgr(), "HarmonyOS Sans",
        playbackCanvas_, Drawing::FontStyle::EXTRA_BLACK_WEIGHT, Drawing::FontStyle::ULTRA_EXPANDED_WIDTH,
        Drawing::FontStyle::UPRIGHT_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3049
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 49)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDefaultFontMgr(), "",
        playbackCanvas_, Drawing::FontStyle::LIGHT_WEIGHT, Drawing::FontStyle::SEMI_CONDENSED_WIDTH,
        Drawing::FontStyle::UPRIGHT_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3050
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 50)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDefaultFontMgr(), "",
        playbackCanvas_, Drawing::FontStyle::NORMAL_WEIGHT, Drawing::FontStyle::ULTRA_EXPANDED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3051
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 51)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDynamicFontMgr(), "HarmonyOS Sans",
        playbackCanvas_, Drawing::FontStyle::LIGHT_WEIGHT, Drawing::FontStyle::CONDENSED_WIDTH,
        Drawing::FontStyle::ITALIC_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3052
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 52)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDynamicFontMgr(), "HarmonyOS Sans",
        playbackCanvas_, Drawing::FontStyle::NORMAL_WEIGHT, Drawing::FontStyle::NORMAL_WIDTH,
        Drawing::FontStyle::ITALIC_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3053
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 53)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDynamicFontMgr(), "",
        playbackCanvas_, Drawing::FontStyle::INVISIBLE_WEIGHT, Drawing::FontStyle::EXTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::ITALIC_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3054
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 54)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDynamicFontMgr(), "",
        playbackCanvas_, Drawing::FontStyle::INVISIBLE_WEIGHT, Drawing::FontStyle::SEMI_CONDENSED_WIDTH,
        Drawing::FontStyle::UPRIGHT_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3055
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 55)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDynamicFontMgr(), "",
        playbackCanvas_, Drawing::FontStyle::INVISIBLE_WEIGHT, Drawing::FontStyle::SEMI_EXPANDED_WIDTH,
        Drawing::FontStyle::ITALIC_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3056
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 56)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDynamicFontMgr(), "",
        playbackCanvas_, Drawing::FontStyle::INVISIBLE_WEIGHT, Drawing::FontStyle::EXTRA_EXPANDED_WIDTH,
        Drawing::FontStyle::UPRIGHT_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3057
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 57)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDynamicFontMgr(), "",
        playbackCanvas_, Drawing::FontStyle::THIN_WEIGHT, Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::ITALIC_SLANT);
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDynamicFontMgr(), "",
        playbackCanvas_, Drawing::FontStyle::LIGHT_WEIGHT, Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3067
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 67)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDynamicFontMgr(), "",
        playbackCanvas_, Drawing::FontStyle::LIGHT_WEIGHT, Drawing::FontStyle::NORMAL_WIDTH,
        Drawing::FontStyle::ITALIC_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3068
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 68)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDynamicFontMgr(), "",
        playbackCanvas_, Drawing::FontStyle::LIGHT_WEIGHT, Drawing::FontStyle::EXPANDED_WIDTH,
        Drawing::FontStyle::UPRIGHT_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3069
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 69)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDynamicFontMgr(), "",
        playbackCanvas_, Drawing::FontStyle::LIGHT_WEIGHT, Drawing::FontStyle::ULTRA_EXPANDED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3070
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 70)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDynamicFontMgr(), "",
        playbackCanvas_, Drawing::FontStyle::NORMAL_WEIGHT, Drawing::FontStyle::EXTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::UPRIGHT_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3071
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 71)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDynamicFontMgr(), "",
        playbackCanvas_, Drawing::FontStyle::NORMAL_WEIGHT, Drawing::FontStyle::SEMI_CONDENSED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3072
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 72)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDynamicFontMgr(), "",
        playbackCanvas_, Drawing::FontStyle::NORMAL_WEIGHT, Drawing::FontStyle::EXPANDED_WIDTH,
        Drawing::FontStyle::ITALIC_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3073
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 73)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDynamicFontMgr(), "",
        playbackCanvas_, Drawing::FontStyle::MEDIUM_WEIGHT, Drawing::FontStyle::EXTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::ITALIC_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3074
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 74)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDynamicFontMgr(), "",
        playbackCanvas_, Drawing::FontStyle::MEDIUM_WEIGHT, Drawing::FontStyle::SEMI_CONDENSED_WIDTH,
        Drawing::FontStyle::ITALIC_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3075
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 75)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDynamicFontMgr(), "",
        playbackCanvas_, Drawing::FontStyle::MEDIUM_WEIGHT, Drawing::FontStyle::SEMI_EXPANDED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3076
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 76)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDynamicFontMgr(), "",
        playbackCanvas_, Drawing::FontStyle::MEDIUM_WEIGHT, Drawing::FontStyle::EXPANDED_WIDTH,
        Drawing::FontStyle::UPRIGHT_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3077
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 77)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDynamicFontMgr(), "",
        playbackCanvas_, Drawing::FontStyle::MEDIUM_WEIGHT, Drawing::FontStyle::ULTRA_EXPANDED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3078
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 78)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDynamicFontMgr(), "",
        playbackCanvas_, Drawing::FontStyle::SEMI_BOLD_WEIGHT, Drawing::FontStyle::CONDENSED_WIDTH,
        Drawing::FontStyle::UPRIGHT_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3079
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 79)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDynamicFontMgr(), "",
        playbackCanvas_, Drawing::FontStyle::SEMI_BOLD_WEIGHT, Drawing::FontStyle::NORMAL_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3080
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 80)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDynamicFontMgr(), "",
        playbackCanvas_, Drawing::FontStyle::SEMI_BOLD_WEIGHT, Drawing::FontStyle::EXPANDED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3081
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 81)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDynamicFontMgr(), "",
        playbackCanvas_, Drawing::FontStyle::SEMI_BOLD_WEIGHT, Drawing::FontStyle::EXTRA_EXPANDED_WIDTH,
        Drawing::FontStyle::ITALIC_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3082
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 82)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDynamicFontMgr(), "",
        playbackCanvas_, Drawing::FontStyle::BOLD_WEIGHT, Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::ITALIC_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3083
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 83)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDynamicFontMgr(), "",
        playbackCanvas_, Drawing::FontStyle::BOLD_WEIGHT, Drawing::FontStyle::CONDENSED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3084
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 84)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDynamicFontMgr(), "",
        playbackCanvas_, Drawing::FontStyle::BOLD_WEIGHT, Drawing::FontStyle::SEMI_CONDENSED_WIDTH,
        Drawing::FontStyle::ITALIC_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3085
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 85)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDynamicFontMgr(), "",
        playbackCanvas_, Drawing::FontStyle::BOLD_WEIGHT, Drawing::FontStyle::EXPANDED_WIDTH,
        Drawing::FontStyle::UPRIGHT_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3086
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 86)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDynamicFontMgr(), "",
        playbackCanvas_, Drawing::FontStyle::BOLD_WEIGHT, Drawing::FontStyle::ULTRA_EXPANDED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3087
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 87)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDynamicFontMgr(), "",
        playbackCanvas_, Drawing::FontStyle::EXTRA_BOLD_WEIGHT, Drawing::FontStyle::EXTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::UPRIGHT_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3088
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 88)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDynamicFontMgr(), "",
        playbackCanvas_, Drawing::FontStyle::EXTRA_BOLD_WEIGHT, Drawing::FontStyle::NORMAL_WIDTH,
        Drawing::FontStyle::UPRIGHT_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3089
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 89)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDynamicFontMgr(), "",
        playbackCanvas_, Drawing::FontStyle::EXTRA_BOLD_WEIGHT, Drawing::FontStyle::SEMI_EXPANDED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3090
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 90)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDynamicFontMgr(), "",
        playbackCanvas_, Drawing::FontStyle::EXTRA_BOLD_WEIGHT, Drawing::FontStyle::EXTRA_EXPANDED_WIDTH,
        Drawing::FontStyle::ITALIC_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3091
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 91)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDynamicFontMgr(), "",
        playbackCanvas_, Drawing::FontStyle::BLACK_WEIGHT, Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::UPRIGHT_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3092
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 92)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDynamicFontMgr(), "",
        playbackCanvas_, Drawing::FontStyle::BLACK_WEIGHT, Drawing::FontStyle::CONDENSED_WIDTH,
        Drawing::FontStyle::ITALIC_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3093
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 93)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDynamicFontMgr(), "",
        playbackCanvas_, Drawing::FontStyle::BLACK_WEIGHT, Drawing::FontStyle::SEMI_EXPANDED_WIDTH,
        Drawing::FontStyle::UPRIGHT_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3094
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 94)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDynamicFontMgr(), "",
        playbackCanvas_, Drawing::FontStyle::BLACK_WEIGHT, Drawing::FontStyle::EXTRA_EXPANDED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3095
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 95)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDynamicFontMgr(), "",
        playbackCanvas_, Drawing::FontStyle::BLACK_WEIGHT, Drawing::FontStyle::ULTRA_EXPANDED_WIDTH,
        Drawing::FontStyle::ITALIC_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3096
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 96)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDynamicFontMgr(), "",
        playbackCanvas_, Drawing::FontStyle::EXTRA_BLACK_WEIGHT, Drawing::FontStyle::EXTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::UPRIGHT_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3097
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 97)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDynamicFontMgr(), "",
        playbackCanvas_, Drawing::FontStyle::EXTRA_BLACK_WEIGHT, Drawing::FontStyle::SEMI_CONDENSED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3098
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 98)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDynamicFontMgr(), "",
        playbackCanvas_, Drawing::FontStyle::EXTRA_BLACK_WEIGHT, Drawing::FontStyle::NORMAL_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT);
}

//对应用例 FontStyleSet_MatchStyle_3099
DEF_DTK(fontstyleset_matchstyle, TestLevel::L1, 99)
{
    DrawFontStylesetmatchstyle(Drawing::FontMgr::CreateDynamicFontMgr(), "",
        playbackCanvas_, Drawing::FontStyle::EXTRA_BLACK_WEIGHT, Drawing::FontStyle::EXPANDED_WIDTH,
        Drawing::FontStyle::ITALIC_SLANT);
}

}
}