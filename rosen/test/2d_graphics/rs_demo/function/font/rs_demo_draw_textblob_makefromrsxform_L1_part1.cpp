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

#include "../../rs_demo_test_ext.h"
#include "text/font.h"
#include "recording/mem_allocator.h"
#include "text/font_mgr.h"
#include "text/font_style_set.h"
#include "text/rs_xform.h"
#include "utils/point.h"
#include "utils/rect.h"
#include "text/typeface.h"

/*
rs_demo_draw_textblob_makefromrsxform_L1
测试类：textblob
测试接口：makefromrsxform
测试内容：对接口入参字体样式、text内容、text元素个数、RSXform、编码格式进行参数组合，通过drawtextblob接口将内容绘制在画布上。
*/

namespace OHOS {
namespace Rosen {

void DrawTexts(
    std::shared_ptr<Drawing::TextBlob>& infoTextBlob, TestPlaybackCanvas* playbackCanvas,
    std::string name, std::string textInfo, size_t length, int symbol,
    Drawing::TextEncoding encoding, const char* p)
{
    int line = 200;
    int interval1 = 100;
    int interval2 = 200;
    int interval3 = 300;
    Drawing::Brush brush;
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    auto font = Drawing::Font(Drawing::Typeface::MakeDefault(), 100.f, 1.f, 0.f);
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    const int count = textInfo.size();
    pen.SetAntiAlias(true);
    pen.SetJoinStyle(Drawing::Pen::JoinStyle::ROUND_JOIN);
    pen.SetWidth(2.25f);
    int maxGlyphCount = font.CountText(textInfo.c_str(), count, Drawing::TextEncoding::UTF8);
    Drawing::RSXform xform[maxGlyphCount];
    switch (symbol) {
        case 1:
            for (int i = 0; i < maxGlyphCount; ++i) {
                xform[i].cos_ = i+1;
                xform[i].sin_ = 0;
                xform[i].tx_ = 100;
                xform[i].ty_ = 100;
            }
            break;
        case 2:
            for (int i = 0; i < maxGlyphCount; ++i) {
                xform[i].cos_ = cos(10*i);
                xform[i].sin_ = sin(10*i);
                xform[i].tx_ = 100;
                xform[i].ty_ = 100;
            }
            break;
        case 3:
            for (int i = 0; i < maxGlyphCount; ++i) {
                xform[i].cos_ = 1;
                xform[i].sin_ = 0;
                xform[i].tx_ = 100+30*i;
                xform[i].ty_ = 100;
            }
            break;
        case 4:
            for (int i = 0; i < maxGlyphCount; ++i) {
                xform[i].cos_ = cos(10 * i) + 0.1 * i;
                xform[i].sin_ = sin(10 * i);
                xform[i].tx_ = 40*i+100;
                xform[i].ty_ = 100;
            }
            break;
    }
    auto infoTextBlob = Drawing::TextBlob::MakeFromRSXform(
        p,
        length,
        &xform[0],
        font,
        Drawing::TextEncoding::UTF8);
    DrawTexts(infoTextBlob, playbackCanvas_);
    playbackCanvas->AttachBrush(brush);
    playbackCanvas->DrawTextBlob(infoTextBlob.get(), interval2, line);
    playbackCanvas->DetachBrush();
    Drawing::Pen pen;
    playbackCanvas->AttachPen(pen);
    playbackCanvas->DrawTextBlob(infoTextBlob.get(), interval3, line + interval1);
    playbackCanvas->DetachPen();
}

//对应用例makefromrsxform_3001
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 1)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".size(), 1, Drawing::TextEncoding::UTF8, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3002
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 2)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".size(), 1, Drawing::TextEncoding::UTF16, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3003
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 3)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".size(), 1, Drawing::TextEncoding::UTF32, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3004
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 4)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".size(), 1, Drawing::TextEncoding::GLYPH_ID, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3005
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 5)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".size(), 2, Drawing::TextEncoding::UTF8, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3006
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 6)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".size(), 2, Drawing::TextEncoding::UTF16, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3007
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 7)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".size(), 2, Drawing::TextEncoding::UTF32, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3008
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 8)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".size(), 2, Drawing::TextEncoding::GLYPH_ID, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3009
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 9)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".size(), 3, Drawing::TextEncoding::UTF8, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3010
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 10)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".size(), 3, Drawing::TextEncoding::UTF16, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3011
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 11)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".size(), 3, Drawing::TextEncoding::UTF32, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3012
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 12)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".size(), 3, Drawing::TextEncoding::GLYPH_ID, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3013
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 13)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".size(), 4, Drawing::TextEncoding::UTF8, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3014
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 14)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".size(), 4, Drawing::TextEncoding::UTF16, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3015
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 15)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".size(), 4, Drawing::TextEncoding::UTF32, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3016
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 16)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".size(), 4, Drawing::TextEncoding::GLYPH_ID, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3017
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 17)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".size()+1, 1, Drawing::TextEncoding::UTF8, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3018
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 18)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".size()+1, 1, Drawing::TextEncoding::UTF16, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3019
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 19)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".size()+1, 1, Drawing::TextEncoding::UTF32, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3020
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 20)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".size()+1, 1, Drawing::TextEncoding::GLYPH_ID, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3021
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 21)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".size()+1, 2, Drawing::TextEncoding::UTF8, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3022
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 22)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".size()+1, 2, Drawing::TextEncoding::UTF16, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3023
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 23)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".size()+1, 2, Drawing::TextEncoding::UTF32, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3024
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 24)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".size()+1, 2, Drawing::TextEncoding::GLYPH_ID, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3025
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 25)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".size()+1, 3, Drawing::TextEncoding::UTF8, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3026
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 26)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".size()+1, 3, Drawing::TextEncoding::UTF16, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3027
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 27)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".size()+1, 3, Drawing::TextEncoding::UTF32, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3028
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 28)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".size()+1, 3, Drawing::TextEncoding::GLYPH_ID, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3029
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 29)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".size()+1, 4, Drawing::TextEncoding::UTF8, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3030
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 30)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".size()+1, 4, Drawing::TextEncoding::UTF16, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3031
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 31)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".size()+1, 4, Drawing::TextEncoding::UTF32, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3032
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 32)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".size()+1, 4, Drawing::TextEncoding::GLYPH_ID, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3033
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 33)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    0, 1, Drawing::TextEncoding::UTF8, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3034
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 34)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    0, 1, Drawing::TextEncoding::UTF16, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3035
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 35)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    0, 1, Drawing::TextEncoding::UTF32, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3036
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 36)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    0, 1, Drawing::TextEncoding::GLYPH_ID, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3037
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 37)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    0, 2, Drawing::TextEncoding::UTF8, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3038
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 38)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    0, 2, Drawing::TextEncoding::UTF16, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3039
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 39)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    0, 2, Drawing::TextEncoding::UTF32, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3040
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 40)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    0, 2, Drawing::TextEncoding::GLYPH_ID, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3041
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 41)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    0, 3, Drawing::TextEncoding::UTF8, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3042
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 42)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    0, 3, Drawing::TextEncoding::UTF16, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3043
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 43)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    0, 3, Drawing::TextEncoding::UTF32, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3044
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 44)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    0, 3, Drawing::TextEncoding::GLYPH_ID, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3045
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 45)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    0, 4, Drawing::TextEncoding::UTF8, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3046
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 46)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    0, 4, Drawing::TextEncoding::UTF16, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3047
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 47)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    0, 4, Drawing::TextEncoding::UTF32, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3048
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 48)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    0, 4, Drawing::TextEncoding::GLYPH_ID, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3049
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 49)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    -5, 1, Drawing::TextEncoding::UTF8, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3050
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 50)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    -5, 1, Drawing::TextEncoding::UTF16, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3051
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 51)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    -5, 1, Drawing::TextEncoding::UTF32, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3052
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 52)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    -5, 1, Drawing::TextEncoding::GLYPH_ID, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3053
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 53)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    -5, 2, Drawing::TextEncoding::UTF8, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3054
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 54)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    -5, 2, Drawing::TextEncoding::UTF16, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3055
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 55)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    -5, 2, Drawing::TextEncoding::UTF32, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3056
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 56)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    -5, 2, Drawing::TextEncoding::GLYPH_ID, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3057
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 57)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    -5, 3, Drawing::TextEncoding::UTF8, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3058
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 58)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    -5, 3, Drawing::TextEncoding::UTF16, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}
//对应用例makefromrsxform_3059
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 59)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    -5, 3, Drawing::TextEncoding::UTF32, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3060
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 60)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    -5, 3, Drawing::TextEncoding::GLYPH_ID, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3061
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 61)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    -5, 4, Drawing::TextEncoding::UTF8, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3062
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 62)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    -5, 4, Drawing::TextEncoding::UTF16, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3063
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 63)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    -5, 4, Drawing::TextEncoding::UTF32, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3064
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 64)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
    -5, 4, Drawing::TextEncoding::GLYPH_ID, "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".c_str());
}

//对应用例makefromrsxform_3065
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 65)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    "111111111111111111111111111111111".size(), 1,
    Drawing::TextEncoding::UTF8, "111111111111111111111111111111111".c_str());
}

//对应用例makefromrsxform_3066
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 66)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    "111111111111111111111111111111111".size(), 1,
    Drawing::TextEncoding::UTF16, "111111111111111111111111111111111".c_str());
}

//对应用例makefromrsxform_3067
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 67)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    "111111111111111111111111111111111".size(), 1, Drawing::TextEncoding::UTF32,
    "111111111111111111111111111111111".c_str());
}

//对应用例makefromrsxform_3068
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 68)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    "111111111111111111111111111111111".size(), 1, Drawing::TextEncoding::GLYPH_ID,
    "111111111111111111111111111111111".c_str());
}

//对应用例makefromrsxform_3069
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 69)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    "111111111111111111111111111111111".size(), 2, Drawing::TextEncoding::UTF8,
    "111111111111111111111111111111111".c_str());
}

//对应用例makefromrsxform_3070
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 70)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    "111111111111111111111111111111111".size(), 2, Drawing::TextEncoding::UTF16,
    "111111111111111111111111111111111".c_str());
}

//对应用例makefromrsxform_3071
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 71)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    "111111111111111111111111111111111".size(), 2, Drawing::TextEncoding::UTF32,
    "111111111111111111111111111111111".c_str());
}

//对应用例makefromrsxform_3072
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 72)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    "111111111111111111111111111111111".size(), 2, Drawing::TextEncoding::GLYPH_ID,
    "111111111111111111111111111111111".c_str());
}

//对应用例makefromrsxform_3073
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 73)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    "111111111111111111111111111111111".size(), 3, Drawing::TextEncoding::UTF8,
    "111111111111111111111111111111111".c_str());
}

//对应用例makefromrsxform_3074
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 74)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    "111111111111111111111111111111111".size(), 3, Drawing::TextEncoding::UTF16,
    "111111111111111111111111111111111".c_str());
}

//对应用例makefromrsxform_3075
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 75)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    "111111111111111111111111111111111".size(), 3, Drawing::TextEncoding::UTF32,
    "111111111111111111111111111111111".c_str());
}

//对应用例makefromrsxform_3076
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 76)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    "111111111111111111111111111111111".size(), 3, Drawing::TextEncoding::GLYPH_ID,
    "111111111111111111111111111111111".c_str());
}

//对应用例makefromrsxform_3077
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 77)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    "111111111111111111111111111111111".size(), 4, Drawing::TextEncoding::UTF8,
    "111111111111111111111111111111111".c_str());
}

//对应用例makefromrsxform_3078
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 78)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    "111111111111111111111111111111111".size(), 4, Drawing::TextEncoding::UTF16,
    "111111111111111111111111111111111".c_str());
}

//对应用例makefromrsxform_3079
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 79)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    "111111111111111111111111111111111".size(), 4, Drawing::TextEncoding::UTF32,
    "111111111111111111111111111111111".c_str());
}

//对应用例makefromrsxform_3080
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 80)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    "111111111111111111111111111111111".size(), 4, Drawing::TextEncoding::GLYPH_ID,
    "111111111111111111111111111111111".c_str());
}

//对应用例makefromrsxform_3081
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 81)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    "111111111111111111111111111111111".size()+1, 1, Drawing::TextEncoding::UTF8,
    "111111111111111111111111111111111".c_str());
}

//对应用例makefromrsxform_3082
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 82)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    "111111111111111111111111111111111".size()+1, 1, Drawing::TextEncoding::UTF16,
    "111111111111111111111111111111111".c_str());
}

//对应用例makefromrsxform_3083
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 83)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    "111111111111111111111111111111111".size()+1, 1, Drawing::TextEncoding::UTF32,
    "111111111111111111111111111111111".c_str());
}

//对应用例makefromrsxform_3084
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 84)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    "111111111111111111111111111111111".size()+1, 1, Drawing::TextEncoding::GLYPH_ID,
    "111111111111111111111111111111111".c_str());
}

//对应用例makefromrsxform_3085
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 85)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    "111111111111111111111111111111111".size()+1, 2, Drawing::TextEncoding::UTF8,
    "111111111111111111111111111111111".c_str());
}

//对应用例makefromrsxform_3086
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 86)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    "111111111111111111111111111111111".size()+1, 2, Drawing::TextEncoding::UTF16,
    "111111111111111111111111111111111".c_str());
}

//对应用例makefromrsxform_3087
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 87)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    "111111111111111111111111111111111".size()+1, 2, Drawing::TextEncoding::UTF32,
    "111111111111111111111111111111111".c_str());
}

//对应用例makefromrsxform_3088
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 88)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    "111111111111111111111111111111111".size()+1, 2, Drawing::TextEncoding::GLYPH_ID,
    "111111111111111111111111111111111".c_str());
}

//对应用例makefromrsxform_3089
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 89)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    "111111111111111111111111111111111".size()+1, 3, Drawing::TextEncoding::UTF8,
    "111111111111111111111111111111111".c_str());
}

//对应用例makefromrsxform_3090
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 90)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    "111111111111111111111111111111111".size()+1, 3, Drawing::TextEncoding::UTF16,
    "111111111111111111111111111111111".c_str());
}

//对应用例makefromrsxform_3091
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 91)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    "111111111111111111111111111111111".size()+1, 3, Drawing::TextEncoding::UTF32,
    "111111111111111111111111111111111".c_str());
}

//对应用例makefromrsxform_3092
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 92)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    "111111111111111111111111111111111".size()+1, 3, Drawing::TextEncoding::GLYPH_ID,
    "111111111111111111111111111111111".c_str());
}

//对应用例makefromrsxform_3093
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 93)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    "111111111111111111111111111111111".size()+1, 4, Drawing::TextEncoding::UTF8,
    "111111111111111111111111111111111".c_str());
}

//对应用例makefromrsxform_3094
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 94)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    "111111111111111111111111111111111".size()+1, 4, Drawing::TextEncoding::UTF16,
    "111111111111111111111111111111111".c_str());
}

//对应用例makefromrsxform_3095
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 95)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    "111111111111111111111111111111111".size()+1, 4, Drawing::TextEncoding::UTF32,
    "111111111111111111111111111111111".c_str());
}

//对应用例makefromrsxform_3096
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 96)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    "111111111111111111111111111111111".size()+1, 4, Drawing::TextEncoding::GLYPH_ID,
    "111111111111111111111111111111111".c_str());
}

//对应用例makefromrsxform_3097
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 97)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    0, 1, Drawing::TextEncoding::UTF8, "111111111111111111111111111111111".c_str());
}

//对应用例makefromrsxform_3098
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 98)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    0, 1, Drawing::TextEncoding::UTF16, "111111111111111111111111111111111".c_str());
}

//对应用例makefromrsxform_3099
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 99)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    0, 1, Drawing::TextEncoding::UTF32, "111111111111111111111111111111111".c_str());
}

//对应用例makefromrsxform_3100
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 100)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    0, 1, Drawing::TextEncoding::GLYPH_ID, "111111111111111111111111111111111".c_str());
}

//对应用例makefromrsxform_3101
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 101)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    0, 2, Drawing::TextEncoding::UTF8, "111111111111111111111111111111111".c_str());
}

//对应用例makefromrsxform_3102
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 102)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    0, 2, Drawing::TextEncoding::UTF16, "111111111111111111111111111111111".c_str());
}

//对应用例makefromrsxform_3103
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 103)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    0, 2, Drawing::TextEncoding::UTF32, "111111111111111111111111111111111".c_str());
}

//对应用例makefromrsxform_3104
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 104)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    0, 2, Drawing::TextEncoding::GLYPH_ID, "111111111111111111111111111111111".c_str());
}

//对应用例makefromrsxform_3105
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 105)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    0, 3, Drawing::TextEncoding::UTF8, "111111111111111111111111111111111".c_str());
}

//对应用例makefromrsxform_3106
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 106)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    0, 3, Drawing::TextEncoding::UTF16, "111111111111111111111111111111111".c_str());
}

//对应用例makefromrsxform_3107
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 107)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    0, 3, Drawing::TextEncoding::UTF32, "111111111111111111111111111111111".c_str());
}

//对应用例makefromrsxform_3108
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 108)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    0, 3, Drawing::TextEncoding::GLYPH_ID, "111111111111111111111111111111111".c_str());
}

//对应用例makefromrsxform_3109
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 109)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    0, 4, Drawing::TextEncoding::UTF8, "111111111111111111111111111111111".c_str());
}

//对应用例makefromrsxform_3110
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 110)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    0, 4, Drawing::TextEncoding::UTF16, "111111111111111111111111111111111".c_str());
}

//对应用例makefromrsxform_3111
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 111)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    0, 4, Drawing::TextEncoding::UTF32, "111111111111111111111111111111111".c_str());
}

//对应用例makefromrsxform_3112
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 112)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    0, 4, Drawing::TextEncoding::GLYPH_ID, "111111111111111111111111111111111".c_str());
}

//对应用例makefromrsxform_3113
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 113)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    -5, 1, Drawing::TextEncoding::UTF8, "111111111111111111111111111111111".c_str());
}

//对应用例makefromrsxform_3114
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 114)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    -5, 1, Drawing::TextEncoding::UTF16, "111111111111111111111111111111111".c_str());
}

//对应用例makefromrsxform_3115
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 115)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    -5, 1, Drawing::TextEncoding::UTF32, "111111111111111111111111111111111".c_str());
}

//对应用例makefromrsxform_3116
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 116)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    -5, 1, Drawing::TextEncoding::GLYPH_ID, "111111111111111111111111111111111".c_str());
}

//对应用例makefromrsxform_3117
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 117)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    -5, 2, Drawing::TextEncoding::UTF8, "111111111111111111111111111111111".c_str());
}

//对应用例makefromrsxform_3118
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 118)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HarmonyOS Sans SC", "111111111111111111111111111111111",
    -5, 2, Drawing::TextEncoding::UTF16, "111111111111111111111111111111111".c_str());
}

