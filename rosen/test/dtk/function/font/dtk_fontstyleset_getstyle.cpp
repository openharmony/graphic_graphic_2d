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
测试类：FontStyleSet
测试接口：GetStyle
测试内容：对接口入参输入string出参接收styleName，构造typeface字体格式，并指定在font上，最终通过drawtextblob接口将styleName内容绘制在画布上
*/

namespace OHOS {
namespace Rosen {

void DrawFontstylesetGetstyle(TestPlaybackCanvas* playbackCanvas,
    std::shared_ptr<Drawing::FontMgr> font_mgr, std::string name, std::int8_t index)
{
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(
        name.empty()
            ? font_mgr->CreateStyleSet(0)
            : font_mgr->MatchFamily(name.c_str())
    );
    std::string styleName = "";
    Drawing::FontStyle fontStyle;
    fontStyleSet->GetStyle(index, &fontStyle, &styleName);

    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);
    std::string text1 = "DDGR ddgr 鸿蒙 !@#%￥^&*;：，。";
    std::string text2 = "-_=+()123`.---~|{}【】,./?、？<>《》";
    std::string text3 = "\xE2\x99\x88\xE2\x99\x8A\xE2\x99\x88\xE2\x99\x8C\xE2\x99\x8D\xE2\x99\x8D";
    std::string text4 = "styleName = " + styleName;
    std::string texts[] = {text1, text2, text3, text4};
    int line = 200;
    int baseLine = 200;
    for (auto text :texts) {
        std::shared_ptr<Drawing::TextBlob> textBlob = Drawing::TextBlob::MakeFromText(text.c_str(), text.size(), font);
        Drawing::Brush brush;
        playbackCanvas->AttachBrush(brush);
        playbackCanvas->DrawTextBlob(textBlob.get(), baseLine, line);
        line += baseLine;
        playbackCanvas->DetachBrush();
        Drawing::Pen pen;
        playbackCanvas->AttachPen(pen);
        playbackCanvas->DrawTextBlob(textBlob.get(), baseLine, line);
        line += baseLine;
        playbackCanvas->DetachPen();
    }
}

//对应用例 FontStyleSet_GetStyle_3001
DEF_DTK(fontstyleset_getstyle, TestLevel::L1, 1)
{
    DrawFontstylesetGetstyle(playbackCanvas_, Drawing::FontMgr::CreateDefaultFontMgr(), "", 1);
}

//对应用例 FontStyleSet_GetStyle_3002
DEF_DTK(fontstyleset_getstyle, TestLevel::L1, 2)
{
    DrawFontstylesetGetstyle(playbackCanvas_, Drawing::FontMgr::CreateDefaultFontMgr(), "", -1);
}

//对应用例 FontStyleSet_GetStyle_3003
DEF_DTK(fontstyleset_getstyle, TestLevel::L1, 3)
{
    DrawFontstylesetGetstyle(playbackCanvas_, Drawing::FontMgr::CreateDefaultFontMgr(), "HarmonyOS Sans", 0);
}

//对应用例 FontStyleSet_GetStyle_3004
DEF_DTK(fontstyleset_getstyle, TestLevel::L1, 4)
{
    DrawFontstylesetGetstyle(playbackCanvas_, Drawing::FontMgr::CreateDefaultFontMgr(), "HarmonyOS Sans", -1);
}

//对应用例 FontStyleSet_GetStyle_3005
DEF_DTK(fontstyleset_getstyle, TestLevel::L1, 5)
{
    DrawFontstylesetGetstyle(playbackCanvas_, Drawing::FontMgr::CreateDynamicFontMgr(), "", 50);
}

//对应用例 FontStyleSet_GetStyle_3006
DEF_DTK(fontstyleset_getstyle, TestLevel::L1, 6)
{
    DrawFontstylesetGetstyle(playbackCanvas_, Drawing::FontMgr::CreateDynamicFontMgr(), "", 1);
}

//对应用例 FontStyleSet_GetStyle_3007
DEF_DTK(fontstyleset_getstyle, TestLevel::L1, 7)
{
    DrawFontstylesetGetstyle(playbackCanvas_, Drawing::FontMgr::CreateDynamicFontMgr(), "HarmonyOS Sans", -1);
}

//对应用例 FontStyleSet_GetStyle_3008
DEF_DTK(fontstyleset_getstyle, TestLevel::L1, 8)
{
    DrawFontstylesetGetstyle(playbackCanvas_, Drawing::FontMgr::CreateDynamicFontMgr(), "", 0);
}

//对应用例 FontStyleSet_GetStyle_3009
DEF_DTK(fontstyleset_getstyle, TestLevel::L1, 9)
{
    DrawFontstylesetGetstyle(playbackCanvas_, Drawing::FontMgr::CreateDefaultFontMgr(), "HarmonyOS Sans", 1);
}

//对应用例 FontStyleSet_GetStyle_3010
DEF_DTK(fontstyleset_getstyle, TestLevel::L1, 10)
{
    DrawFontstylesetGetstyle(playbackCanvas_, Drawing::FontMgr::CreateDefaultFontMgr(), "", 0);
}

//对应用例 FontStyleSet_GetStyle_3011
DEF_DTK(fontstyleset_getstyle, TestLevel::L1, 11)
{
    DrawFontstylesetGetstyle(playbackCanvas_, Drawing::FontMgr::CreateDefaultFontMgr(), "HarmonyOS Sans", 50);
}

//对应用例 FontStyleSet_GetStyle_3012
DEF_DTK(fontstyleset_getstyle, TestLevel::L1, 12)
{
    DrawFontstylesetGetstyle(playbackCanvas_, Drawing::FontMgr::CreateDynamicFontMgr(), "HarmonyOS Sans", 0);
}

//对应用例 FontStyleSet_GetStyle_3013
DEF_DTK(fontstyleset_getstyle, TestLevel::L1, 13)
{
    DrawFontstylesetGetstyle(playbackCanvas_, Drawing::FontMgr::CreateDynamicFontMgr(), "HarmonyOS Sans", 50);
}

//对应用例 FontStyleSet_GetStyle_3014
DEF_DTK(fontstyleset_getstyle, TestLevel::L1, 14)
{
    DrawFontstylesetGetstyle(playbackCanvas_, Drawing::FontMgr::CreateDefaultFontMgr(), "", 50);
}

//对应用例 FontStyleSet_GetStyle_3015
DEF_DTK(fontstyleset_getstyle, TestLevel::L1, 15)
{
    DrawFontstylesetGetstyle(playbackCanvas_, Drawing::FontMgr::CreateDynamicFontMgr(), "", -1);
}

//对应用例 FontStyleSet_GetStyle_3016
DEF_DTK(fontstyleset_getstyle, TestLevel::L1, 16)
{
    DrawFontstylesetGetstyle(playbackCanvas_, Drawing::FontMgr::CreateDynamicFontMgr(), "HarmonyOS Sans", 1);
}

}
}