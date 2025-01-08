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
        encoding);
    DrawTexts(infoTextBlob, playbackCanvas_);
    playbackCanvas->AttachBrush(brush);
    playbackCanvas->DrawTextBlob(infoTextBlob.get(), interval2, line);
    playbackCanvas->DetachBrush();
    Drawing::Pen pen;
    playbackCanvas->AttachPen(pen);
    playbackCanvas->DrawTextBlob(infoTextBlob.get(), interval3, line + interval1);
    playbackCanvas->DetachPen();
}

//对应用例makefromrsxform_3246
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 246)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HMOS Color Emoji",
    "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99",
    -5, 2, Drawing::TextEncoding::UTF16,
    "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99".c_str());
}

//对应用例makefromrsxform_3247
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 247)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HMOS Color Emoji",
    "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99",
    -5, 2, Drawing::TextEncoding::UTF32,
    "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99".c_str());
}

//对应用例makefromrsxform_3248
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 248)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HMOS Color Emoji",
    "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99",
    -5, 2, Drawing::TextEncoding::GLYPH_ID,
    "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99".c_str());
}

//对应用例makefromrsxform_3249
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 249)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HMOS Color Emoji",
    "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99",
    -5, 3, Drawing::TextEncoding::UTF8,
    "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99".c_str());
}

//对应用例makefromrsxform_3250
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 250)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HMOS Color Emoji",
    "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99",
    -5, 3, Drawing::TextEncoding::UTF16,
    "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99".c_str());
}

//对应用例makefromrsxform_3251
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 251)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HMOS Color Emoji",
    "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99",
    -5, 3, Drawing::TextEncoding::UTF32,
    "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99".c_str());
}

//对应用例makefromrsxform_3252
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 252)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HMOS Color Emoji",
    "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99",
    -5, 3, Drawing::TextEncoding::GLYPH_ID,
    "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99".c_str());
}

//对应用例makefromrsxform_3253
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 253)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HMOS Color Emoji",
    "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99",
    -5, 4, Drawing::TextEncoding::UTF8,
    "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99".c_str());
}

//对应用例makefromrsxform_3254
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 254)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HMOS Color Emoji",
    "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99",
    -5, 3, Drawing::TextEncoding::UTF16,
    "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99".c_str());
}

//对应用例makefromrsxform_3255
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 255)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HMOS Color Emoji",
    "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99",
    -5, 3, Drawing::TextEncoding::UTF32,
    "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99".c_str());
}

//对应用例makefromrsxform_3256
DEF_RSDEMO(textblob_makefromrsxform, TestLevel::L1, 256)
{
    DrawTexts(infoTextBlob, playbackCanvas_, "HMOS Color Emoji",
    "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99",
    -5, 3, Drawing::TextEncoding::GLYPH_ID,
    "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99".c_str());
}

}
}
