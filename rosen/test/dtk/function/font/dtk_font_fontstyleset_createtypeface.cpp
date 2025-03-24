/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
测试类：FontStyleSet
测试接口：CreateTypeface
测试内容：对接口入参int取等价值有效值等，构造typeface字体格式，并指定在font上，最终通过drawtextblob接口将text内容绘制在画布上
*/

namespace OHOS {
namespace Rosen {

static void DrawTexts(std::shared_ptr<Drawing::FontMgr> font_mgr, int index,
    std::string name, TestPlaybackCanvas* playbackCanvas)
{
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(
        name.empty()
            ? font_mgr->CreateStyleSet(0)
            : font_mgr->MatchFamily(name.c_str())
    );
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(index));
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);
    std::string text1 = "DDGR ddgr 鸿蒙 !@#%￥^&*;：，。";
    std::string text2 = "-_=+()123`.---~|{}【】,./?、？<>《》";
    std::string text3 = "\xE2\x99\x88\xE2\x99\x8A\xE2\x99\x88\xE2\x99\x8C\xE2\x99\x8D\xE2\x99\x8D";
    std::string texts[] = { text1, text2, text3 };
    int interval = 200;
    int line = 200;
    for (auto text : texts) {
        std::shared_ptr<Drawing::TextBlob> textBlob = Drawing::TextBlob::MakeFromText(text.c_str(), text.size(), font);
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

// 对应用例 FontStyleSet_CreateTypeface_3001
DEF_DTK(fontstyleset_createtypeface, TestLevel::L1, 1)
{
    DrawTexts(Drawing::FontMgr::CreateDefaultFontMgr(), 1, "", playbackCanvas_);
}

// 对应用例 FontStyleSet_CreateTypeface_3002
DEF_DTK(fontstyleset_createtypeface, TestLevel::L1, 2)
{
    DrawTexts(Drawing::FontMgr::CreateDefaultFontMgr(), -1, "", playbackCanvas_);
}

// 对应用例 FontStyleSet_CreateTypeface_3003
DEF_DTK(fontstyleset_createtypeface, TestLevel::L1, 3)
{
    DrawTexts(Drawing::FontMgr::CreateDefaultFontMgr(), 0, "HarmonyOS Sans", playbackCanvas_);
}

// 对应用例 FontStyleSet_CreateTypeface_3004
DEF_DTK(fontstyleset_createtypeface, TestLevel::L1, 4)
{
    DrawTexts(Drawing::FontMgr::CreateDefaultFontMgr(), -1, "HarmonyOS Sans", playbackCanvas_);
}

// 对应用例 FontStyleSet_CreateTypeface_3005
DEF_DTK(fontstyleset_createtypeface, TestLevel::L1, 5)
{
    DrawTexts(Drawing::FontMgr::CreateDynamicFontMgr(), 50, "", playbackCanvas_);
}

// 对应用例 FontStyleSet_CreateTypeface_3006
DEF_DTK(fontstyleset_createtypeface, TestLevel::L1, 6)
{
    DrawTexts(Drawing::FontMgr::CreateDynamicFontMgr(), 1, "", playbackCanvas_);
}

// 对应用例 FontStyleSet_CreateTypeface_3007
DEF_DTK(fontstyleset_createtypeface, TestLevel::L1, 7)
{
    DrawTexts(Drawing::FontMgr::CreateDynamicFontMgr(), -1, "HarmonyOS Sans", playbackCanvas_);
}

// 对应用例 FontStyleSet_CreateTypeface_3008
DEF_DTK(fontstyleset_createtypeface, TestLevel::L1, 8)
{
    DrawTexts(Drawing::FontMgr::CreateDynamicFontMgr(), 0, "", playbackCanvas_);
}

// 对应用例 FontStyleSet_CreateTypeface_3009
DEF_DTK(fontstyleset_createtypeface, TestLevel::L1, 9)
{
    DrawTexts(Drawing::FontMgr::CreateDefaultFontMgr(), 1, "HarmonyOS Sans", playbackCanvas_);
}

// 对应用例 FontStyleSet_CreateTypeface_3010
DEF_DTK(fontstyleset_createtypeface, TestLevel::L1, 10)
{
    DrawTexts(Drawing::FontMgr::CreateDefaultFontMgr(), 0, "", playbackCanvas_);
}

// 对应用例 FontStyleSet_CreateTypeface_3011
DEF_DTK(fontstyleset_createtypeface, TestLevel::L1, 11)
{
    DrawTexts(Drawing::FontMgr::CreateDefaultFontMgr(), 50, "HarmonyOS Sans", playbackCanvas_);
}

// 对应用例 FontStyleSet_CreateTypeface_3012
DEF_DTK(fontstyleset_createtypeface, TestLevel::L1, 12)
{
    DrawTexts(Drawing::FontMgr::CreateDynamicFontMgr(), 0, "HarmonyOS Sans", playbackCanvas_);
}

// 对应用例 FontStyleSet_CreateTypeface_3013
DEF_DTK(fontstyleset_createtypeface, TestLevel::L1, 13)
{
    DrawTexts(Drawing::FontMgr::CreateDynamicFontMgr(), 50, "HarmonyOS Sans", playbackCanvas_);
}

// 对应用例 FontStyleSet_CreateTypeface_3014
DEF_DTK(fontstyleset_createtypeface, TestLevel::L1, 14)
{
    DrawTexts(Drawing::FontMgr::CreateDefaultFontMgr(), 50, "", playbackCanvas_);
}

// 对应用例 FontStyleSet_CreateTypeface_3015
DEF_DTK(fontstyleset_createtypeface, TestLevel::L1, 15)
{
    DrawTexts(Drawing::FontMgr::CreateDynamicFontMgr(), -1, "", playbackCanvas_);
}

// 对应用例 FontStyleSet_CreateTypeface_3016
DEF_DTK(fontstyleset_createtypeface, TestLevel::L1, 16)
{
    DrawTexts(Drawing::FontMgr::CreateDynamicFontMgr(), 1, "HarmonyOS Sans", playbackCanvas_);
}

}
}