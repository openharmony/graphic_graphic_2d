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

//对应用例makefromrsxform_3221
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 221)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        textInfo.size()+1, 4,
        Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3222
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 222)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        textInfo.size()+1, 4,
        Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3223
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 223)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        textInfo.size()+1, 4,
        Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3224
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 224)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        textInfo.size()+1, 4,
        Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3225
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 225)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        0, 1, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3226
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 226)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        0, 1, Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3227
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 227)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        0, 1, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3228
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 228)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        0, 1, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3229
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 229)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        0, 2, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3230
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 230)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        0, 2, Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3231
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 231)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        0, 2, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3232
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 232)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        0, 2, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3233
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 233)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        0, 3, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3234
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 234)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        0, 3, Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3235
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 235)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        0, 3, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3236
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 236)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        0, 3, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3237
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 237)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        0, 4, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3238
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 238)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        0, 4, Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3239
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 239)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        0, 4, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3240
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 240)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        0, 4, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3241
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 241)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        -5, 1, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3242
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 242)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        -5, 1, Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3243
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 243)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        -5, 1, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3244
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 244)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        -5, 1, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3245
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 245)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        -5, 2, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3246
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 246)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        -5, 2, Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3247
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 247)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        -5, 2, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3248
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 248)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        -5, 2, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3249
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 249)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        -5, 3, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3250
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 250)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        -5, 3, Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3251
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 251)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        -5, 3, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3252
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 252)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        -5, 3, Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefromrsxform_3253
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 253)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        -5, 4, Drawing::TextEncoding::UTF8);
}

//对应用例makefromrsxform_3254
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 254)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        -5, 3, Drawing::TextEncoding::UTF16);
}

//对应用例makefromrsxform_3255
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 255)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        -5, 3, Drawing::TextEncoding::UTF32);
}

//对应用例makefromrsxform_3256
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 256)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_,
        textInfo,
        -5, 3, Drawing::TextEncoding::GLYPH_ID);
}

}
}
