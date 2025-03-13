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
rs_demo_draw_textblob_makefromrsxform_L1
测试类：textblob
测试接口：makefromrsxform
测试内容：对接口入参字体样式、text内容、text元素个数、RSXform、编码格式进行参数组合，通过drawtextblob接口将内容绘制在画布上。
*/

namespace OHOS {
namespace Rosen {

static void DrawTexts(TestPlaybackCanvas* playbackCanvas,
    std::string name, std::string textInfo, size_t length, int symbol,
    Drawing::TextEncoding encoding, const char* p)
{
    int line1 = 200;
    int interval = 200;
    int line2 = 800;
    Drawing::Brush brush;
    Drawing::Pen pen;

    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    auto font = Drawing::Font(Drawing::Typeface::MakeDefault(), 100.f, 1.f, 0.f);
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
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
        encoding);

    playbackCanvas->AttachPen(pen);
    playbackCanvas->DrawTextBlob(infoTextBlob.get(), interval, line1);
    playbackCanvas->DetachPen();

    playbackCanvas->AttachBrush(brush);
    playbackCanvas->DrawTextBlob(infoTextBlob.get(), interval, line2);
    playbackCanvas->DetachBrush();
}

//对应用例makefromrsxform_3222
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 222)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_, "HMOS Color Emoji",
        textInfo,
        textInfo.size()+1, 4,
        Drawing::TextEncoding::UTF16,
        textInfo.c_str());
}

//对应用例makefromrsxform_3223
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 223)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_, "HMOS Color Emoji",
        textInfo,
        textInfo.size()+1, 4,
        Drawing::TextEncoding::UTF32,
        textInfo.c_str());
}

//对应用例makefromrsxform_3224
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 224)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_, "HMOS Color Emoji",
        textInfo,
        textInfo.size()+1, 4,
        Drawing::TextEncoding::GLYPH_ID,
        textInfo.c_str());
}

//对应用例makefromrsxform_3225
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 225)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_, "HMOS Color Emoji",
        textInfo,
        0, 1, Drawing::TextEncoding::UTF8,
        textInfo.c_str());
}

//对应用例makefromrsxform_3226
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 226)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_, "HMOS Color Emoji",
        textInfo,
        0, 1, Drawing::TextEncoding::UTF16,
        textInfo.c_str());
}

//对应用例makefromrsxform_3227
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 227)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_, "HMOS Color Emoji",
        textInfo,
        0, 1, Drawing::TextEncoding::UTF32,
        textInfo.c_str());
}

//对应用例makefromrsxform_3228
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 228)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_, "HMOS Color Emoji",
        textInfo,
        0, 1, Drawing::TextEncoding::GLYPH_ID,
        textInfo.c_str());
}

//对应用例makefromrsxform_3229
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 229)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_, "HMOS Color Emoji",
        textInfo,
        0, 2, Drawing::TextEncoding::UTF8,
        textInfo.c_str());
}

//对应用例makefromrsxform_3230
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 230)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_, "HMOS Color Emoji",
        textInfo,
        0, 2, Drawing::TextEncoding::UTF16,
        textInfo.c_str());
}

//对应用例makefromrsxform_3231
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 231)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_, "HMOS Color Emoji",
        textInfo,
        0, 2, Drawing::TextEncoding::UTF32,
        textInfo.c_str());
}

//对应用例makefromrsxform_3232
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 232)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_, "HMOS Color Emoji",
        textInfo,
        0, 2, Drawing::TextEncoding::GLYPH_ID,
        textInfo.c_str());
}

//对应用例makefromrsxform_3233
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 233)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_, "HMOS Color Emoji",
        textInfo,
        0, 3, Drawing::TextEncoding::UTF8,
        textInfo.c_str());
}

//对应用例makefromrsxform_3234
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 234)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_, "HMOS Color Emoji",
        textInfo,
        0, 3, Drawing::TextEncoding::UTF16,
        textInfo.c_str());
}

//对应用例makefromrsxform_3235
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 235)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_, "HMOS Color Emoji",
        textInfo,
        0, 3, Drawing::TextEncoding::UTF32,
        textInfo.c_str());
}

//对应用例makefromrsxform_3236
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 236)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_, "HMOS Color Emoji",
        textInfo,
        0, 3, Drawing::TextEncoding::GLYPH_ID,
        textInfo.c_str());
}

//对应用例makefromrsxform_3237
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 237)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_, "HMOS Color Emoji",
        textInfo,
        0, 4, Drawing::TextEncoding::UTF8,
        textInfo.c_str());
}

//对应用例makefromrsxform_3238
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 238)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_, "HMOS Color Emoji",
        textInfo,
        0, 4, Drawing::TextEncoding::UTF16,
        textInfo.c_str());
}

//对应用例makefromrsxform_3239
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 239)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_, "HMOS Color Emoji",
        textInfo,
        0, 4, Drawing::TextEncoding::UTF32,
        textInfo.c_str());
}

//对应用例makefromrsxform_3240
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 240)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_, "HMOS Color Emoji",
        textInfo,
        0, 4, Drawing::TextEncoding::GLYPH_ID,
        textInfo.c_str());
}

//对应用例makefromrsxform_3241
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 241)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_, "HMOS Color Emoji",
        textInfo,
        -5, 1, Drawing::TextEncoding::UTF8,
        textInfo.c_str());
}

//对应用例makefromrsxform_3242
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 242)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_, "HMOS Color Emoji",
        textInfo,
        -5, 1, Drawing::TextEncoding::UTF16,
        textInfo.c_str());
}

//对应用例makefromrsxform_3243
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 243)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_, "HMOS Color Emoji",
        textInfo,
        -5, 1, Drawing::TextEncoding::UTF32,
        textInfo.c_str());
}

//对应用例makefromrsxform_3244
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 244)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_, "HMOS Color Emoji",
        textInfo,
        -5, 1, Drawing::TextEncoding::GLYPH_ID,
        textInfo.c_str());
}

//对应用例makefromrsxform_3245
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 245)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_, "HMOS Color Emoji",
        textInfo,
        -5, 2, Drawing::TextEncoding::UTF8,
        textInfo.c_str());
}

//对应用例makefromrsxform_3246
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 246)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_, "HMOS Color Emoji",
        textInfo,
        -5, 2, Drawing::TextEncoding::UTF16,
        textInfo.c_str());
}

//对应用例makefromrsxform_3247
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 247)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_, "HMOS Color Emoji",
        textInfo,
        -5, 2, Drawing::TextEncoding::UTF32,
        textInfo.c_str());
}

//对应用例makefromrsxform_3248
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 248)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_, "HMOS Color Emoji",
        textInfo,
        -5, 2, Drawing::TextEncoding::GLYPH_ID,
        textInfo.c_str());
}

//对应用例makefromrsxform_3249
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 249)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_, "HMOS Color Emoji",
        textInfo,
        -5, 3, Drawing::TextEncoding::UTF8,
        textInfo.c_str());
}

//对应用例makefromrsxform_3250
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 250)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_, "HMOS Color Emoji",
        textInfo,
        -5, 3, Drawing::TextEncoding::UTF16,
        textInfo.c_str());
}

//对应用例makefromrsxform_3251
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 251)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_, "HMOS Color Emoji",
        textInfo,
        -5, 3, Drawing::TextEncoding::UTF32,
        textInfo.c_str());
}

//对应用例makefromrsxform_3252
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 252)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_, "HMOS Color Emoji",
        textInfo,
        -5, 3, Drawing::TextEncoding::GLYPH_ID,
        textInfo.c_str());
}

//对应用例makefromrsxform_3253
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 253)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_, "HMOS Color Emoji",
        textInfo,
        -5, 4, Drawing::TextEncoding::UTF8,
        textInfo.c_str());
}

//对应用例makefromrsxform_3254
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 254)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_, "HMOS Color Emoji",
        textInfo,
        -5, 3, Drawing::TextEncoding::UTF16,
        textInfo.c_str());
}

//对应用例makefromrsxform_3255
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 255)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_, "HMOS Color Emoji",
        textInfo,
        -5, 3, Drawing::TextEncoding::UTF32,
        textInfo.c_str());
}

//对应用例makefromrsxform_3256
DEF_DTK(textblob_makefromrsxform_2, TestLevel::L1, 256)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(playbackCanvas_, "HMOS Color Emoji",
        textInfo,
        -5, 3, Drawing::TextEncoding::GLYPH_ID,
        textInfo.c_str());
}

}
}
