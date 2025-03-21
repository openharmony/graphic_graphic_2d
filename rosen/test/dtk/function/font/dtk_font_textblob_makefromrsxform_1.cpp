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
rs_demo_draw_textblob_makefromrsxform_L1
测试类：textblob
测试接口：makefromrsxform
测试内容：对接口入参字体样式、text内容、text元素个数、RSXform、编码格式进行参数组合，通过drawtextblob接口将内容绘制在画布上。
*/

namespace OHOS {
namespace Rosen {

static void FillRSXform(Drawing::RSXform xform[], int maxGlyphCount, int mode)
{
    const int mode1 = 1;
    const int mode2 = 2;
    const int mode3 = 3;
    const int mode4 = 4;
    if (mode == mode1) {
        const int cosvalue = 1;
        const int t = 100;
        for (int i = 0; i < maxGlyphCount; ++i) {
            xform[i].cos_ = i+cosvalue;
            xform[i].sin_ = 0;
            xform[i].tx_ = t;
            xform[i].ty_ = t;
        }
    } else if (mode == mode2) {
        for (int i = 0; i < maxGlyphCount; ++i) {
            const int factor = 10;
            const int t = 100;

            xform[i].cos_ = cos(factor*i);
            xform[i].sin_ = sin(factor*i);
            xform[i].tx_ = t;
            xform[i].ty_ = t;
        }
    } else if (mode == mode3) {
        const int cosvalue = 1;
        const int t = 100;
        const int factor = 30;
        for (int i = 0; i < maxGlyphCount; ++i) {
            xform[i].cos_ = cosvalue;
            xform[i].sin_ = 0;
            xform[i].tx_ = t+factor*i;
            xform[i].ty_ = t;
        }
    } else if (mode == mode4) {
        const int m = 10;
        const float f = 0.1;
        const int t = 100;
        const int factor = 40;
        for (int i = 0; i < maxGlyphCount; ++i) {
            xform[i].cos_ = cos(m * i) + f * i;
            xform[i].sin_ = sin(m * i);
            xform[i].tx_ = factor*i+t;
            xform[i].ty_ = t;
        }
    }
}

static void DrawTexts(TestPlaybackCanvas* playbackCanvas,
    std::string textInfo, size_t length, int symbol,
    Drawing::TextEncoding encoding)
{
    int line1 = 200;
    int interval = 200;
    int line2 = 800;
    Drawing::Brush brush;
    Drawing::Pen pen;

    char* p = nullptr;
    if (textInfo != "harmony_os") {
        p = (char*)textInfo.c_str();
    }
    std::string name = "HarmonyOS Sans SC";
    if (textInfo == "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99") {
        name = "HMOS Color Emoji";
    }

    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    auto font = Drawing::Font(Drawing::Typeface::MakeDefault(), 100.f, 1.f, 0.f);
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    const int count = textInfo.size();
    pen.SetAntiAlias(true);
    pen.SetJoinStyle(Drawing::Pen::JoinStyle::ROUND_JOIN);
    pen.SetWidth(2.25f);
    int maxGlyphCount = font.CountText(textInfo.c_str(), count, Drawing::TextEncoding::UTF8);
    Drawing::RSXform xform[maxGlyphCount];
    FillRSXform(xform, maxGlyphCount, symbol);
    auto infoTextBlob = Drawing::TextBlob::MakeFromRSXform(
        p,
        length,
        &xform[0],
        font,
        encoding);

    playbackCanvas->AttachPen(pen);
    playbackCanvas->DrawTextBlob(infoTextBlob.get(), interval, line1);
    playbackCanvas->DetachPen();

    playbackCanvas->AttachBrush(brush);
    playbackCanvas->DrawTextBlob(infoTextBlob.get(), interval, line2);
    playbackCanvas->DetachBrush();
}

//对应用例makefromrsxform_3001
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 1)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size(), 1, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3002
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 2)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size(), 1, Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3003
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 3)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size(), 1, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3004
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 4)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size(), 1, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3005
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 5)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size(), 2, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3006
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 6)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size(), 2, Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3007
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 7)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size(), 2, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3008
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 8)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size(), 2, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3009
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 9)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size(), 3, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3010
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 10)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size(), 3, Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3011
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 11)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size(), 3, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3012
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 12)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size(), 3, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3013
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 13)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size(), 4, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3014
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 14)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size(), 4, Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3015
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 15)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size(), 4, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3016
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 16)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size(), 4, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3017
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 17)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size()+1, 1, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3018
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 18)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size()+1, 1, Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3019
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 19)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size()+1, 1, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3020
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 20)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size()+1, 1, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3021
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 21)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size()+1, 2, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3022
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 22)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size()+1, 2, Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3023
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 23)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size()+1, 2, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3024
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 24)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size()+1, 2, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3025
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 25)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size()+1, 3, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3026
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 26)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size()+1, 3, Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3027
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 27)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size()+1, 3, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3028
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 28)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size()+1, 3, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3029
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 29)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size()+1, 4, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3030
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 30)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size()+1, 4, Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3031
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 31)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size()+1, 4, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3032
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 32)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size()+1, 4, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3033
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 33)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        0, 1, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3034
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 34)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        0, 1, Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3035
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 35)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        0, 1, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3036
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 36)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        0, 1, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3037
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 37)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        0, 2, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3038
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 38)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        0, 2, Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3039
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 39)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        0, 2, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3040
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 40)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        0, 2, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3041
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 41)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        0, 3, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3042
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 42)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        0, 3, Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3043
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 43)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        0, 3, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3044
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 44)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        0, 3, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3045
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 45)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        0, 4, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3046
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 46)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        0, 4, Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3047
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 47)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        0, 4, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3048
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 48)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        0, 4, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3049
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 49)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        -5, 1, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3050
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 50)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        -5, 1, Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3051
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 51)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        -5, 1, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3052
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 52)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        -5, 1, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3053
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 53)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        -5, 2, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3054
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 54)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        -5, 2, Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3055
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 55)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        -5, 2, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3056
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 56)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        -5, 2, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3057
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 57)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        -5, 3, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3058
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 58)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        -5, 3, Drawing::TextEncoding::UTF16);
}
//对应用例makefromrsxform_3059
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 59)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        -5, 3, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3060
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 60)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        -5, 3, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3061
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 61)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        -5, 4, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3062
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 62)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        -5, 4, Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3063
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 63)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        -5, 4, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3064
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 64)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts(playbackCanvas_, textInfo,
        -5, 4, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3065
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 65)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size(), 1,
        Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3066
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 66)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size(), 1,
        Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3067
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 67)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size(), 1, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3068
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 68)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size(), 1, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3069
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 69)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size(), 2, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3070
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 70)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size(), 2, Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3071
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 71)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size(), 2, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3072
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 72)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size(), 2, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3073
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 73)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size(), 3, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3074
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 74)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size(), 3, Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3075
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 75)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size(), 3, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3076
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 76)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size(), 3, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3077
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 77)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size(), 4, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3078
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 78)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size(), 4, Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3079
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 79)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size(), 4, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3080
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 80)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size(), 4, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3081
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 81)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size()+1, 1, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3082
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 82)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size()+1, 1, Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3083
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 83)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size()+1, 1, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3084
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 84)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size()+1, 1, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3085
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 85)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size()+1, 2, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3086
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 86)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size()+1, 2, Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3087
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 87)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size()+1, 2, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3088
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 88)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size()+1, 2, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3089
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 89)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size()+1, 3, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3090
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 90)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size()+1, 3, Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3091
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 91)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size()+1, 3, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3092
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 92)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size()+1, 3, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3093
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 93)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size()+1, 4, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3094
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 94)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size()+1, 4, Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3095
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 95)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size()+1, 4, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3096
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 96)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size()+1, 4, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3097
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 97)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        0, 1, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3098
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 98)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        0, 1, Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3099
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 99)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        0, 1, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3100
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 100)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        0, 1, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3101
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 101)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        0, 2, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3102
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 102)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        0, 2, Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3103
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 103)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        0, 2, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3104
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 104)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        0, 2, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3105
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 105)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        0, 3, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3106
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 106)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        0, 3, Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3107
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 107)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        0, 3, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3108
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 108)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        0, 3, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3109
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 109)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        0, 4, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3110
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 110)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        0, 4, Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3111
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 111)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        0, 4, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3112
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 112)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        0, 4, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3113
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 113)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        -5, 1, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3114
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 114)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        -5, 1, Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3115
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 115)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        -5, 1, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3116
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 116)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        -5, 1, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3117
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 117)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        -5, 2, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3118
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 118)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        -5, 2, Drawing::TextEncoding::UTF16);
}
//对应用例makefromrsxform_3119
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 119)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        -5, 2, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3120
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 120)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        -5, 2, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3121
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 121)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        -5, 3, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3122
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 122)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        -5, 3, Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3123
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 123)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        -5, 3, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3124
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 124)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        -5, 3, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3125
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 125)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        -5, 4, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3126
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 126)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        -5, 4, Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3127
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 127)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        -5, 4, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3128
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 128)
{
    std::string textInfo = "111111111111111111111111111111111";
    DrawTexts(playbackCanvas_, textInfo,
        -5, 4, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3129
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 129)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size(), 1, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3130
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 130)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size(), 1, Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3131
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 131)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size(), 1, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3132
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 132)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size(), 1, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3133
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 133)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size(), 2, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3134
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 134)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size(), 2, Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3135
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 135)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size(), 2, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3136
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 136)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size(), 2, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3137
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 137)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size(), 3, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3138
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 138)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size(), 3, Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3139
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 139)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size(), 3, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3140
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 140)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size(), 3, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3141
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 141)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size(), 4, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3142
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 142)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size(), 4, Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3143
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 143)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size(), 4, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3144
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 144)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size(), 4, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3145
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 145)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size()+1, 1, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3146
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 146)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size()+1, 1, Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3147
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 147)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size()+1, 1, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3148
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 148)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size()+1, 1, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3149
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 149)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size()+1, 2, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3150
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 150)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size()+1, 2, Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3151
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 151)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size()+1, 2, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3152
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 152)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size()+1, 2, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3153
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 153)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size()+1, 3, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3154
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 154)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size()+1, 3, Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3155
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 155)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size()+1, 3, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3156
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 156)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size()+1, 3, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3157
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 157)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size()+1, 4, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3158
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 158)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size()+1, 4, Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3159
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 159)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size()+1, 4, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3160
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 160)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        textInfo.size()+1, 4, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3161
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 161)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        0, 1, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3162
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 162)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        0, 1, Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3163
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 163)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        0, 1, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3164
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 164)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        0, 1, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3165
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 165)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        0, 2, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3166
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 166)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        0, 2, Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3167
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 167)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        0, 2, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3168
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 168)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        0, 2, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3169
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 169)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        0, 3, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3170
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 170)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        0, 3, Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3171
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 171)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        0, 3, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3172
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 172)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        0, 3, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3173
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 173)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        0, 4, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3174
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 174)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        0, 4, Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3175
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 175)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        0, 4, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3176
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 176)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        0, 4, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3177
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 177)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        -5, 1, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3178
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 178)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        -5, 1, Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3179
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 179)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        -5, 1, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3180
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 180)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        -5, 1, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3181
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 181)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        -5, 2, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3182
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 182)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        -5, 2, Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3183
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 183)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        -5, 2, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3184
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 184)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        -5, 2, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3185
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 185)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        -5, 3, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3186
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 186)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        -5, 3, Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3187
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 187)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        -5, 3, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3188
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 188)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        -5, 3, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3189
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 189)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        -5, 4, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3190
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 190)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        -5, 4, Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3191
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 191)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        -5, 4, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3192
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 192)
{
    std::string textInfo = "harmony_os";
    DrawTexts(playbackCanvas_, textInfo,
        -5, 4, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3193
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 193)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        textInfo.size(), 1,
        Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3194
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 194)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        textInfo.size(), 1,
        Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3195
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 195)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        textInfo.size(), 1,
        Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3196
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 196)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        textInfo.size(), 1,
        Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3197
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 197)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        textInfo.size(), 2,
        Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3198
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 198)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        textInfo.size(), 2,
        Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3199
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 199)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        textInfo.size(), 2,
        Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3200
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 200)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        textInfo.size(), 2,
        Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3201
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 201)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        textInfo.size(), 3,
        Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3202
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 202)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        textInfo.size(), 3,
        Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3203
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 203)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        textInfo.size(), 3,
        Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3204
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 204)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        textInfo.size(), 3,
        Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3205
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 205)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        textInfo.size(), 4,
        Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3206
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 206)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        textInfo.size(), 4,
        Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3207
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 207)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        textInfo.size(), 4,
        Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3208
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 208)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        textInfo.size(), 4,
        Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3209
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 209)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        textInfo.size()+1, 1,
        Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3210
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 210)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        textInfo.size()+1, 1,
        Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3211
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 211)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        textInfo.size()+1, 1,
        Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3212
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 212)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        textInfo.size()+1, 1,
        Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3213
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 213)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        textInfo.size()+1, 2,
        Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3214
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 214)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        textInfo.size()+1, 2,
        Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3215
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 215)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        textInfo.size()+1, 2,
        Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3216
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 216)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        textInfo.size()+1, 2,
        Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3217
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 217)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        textInfo.size()+1, 3,
        Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3218
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 218)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        textInfo.size()+1, 3,
        Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3219
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 219)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        textInfo.size()+1, 3,
        Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3220
DEF_DTK(textblob_makefromrsxform_1, TestLevel::L1, 220)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        textInfo.size()+1, 3,
        Drawing::TextEncoding::GLYPH_ID);
}

}
}
