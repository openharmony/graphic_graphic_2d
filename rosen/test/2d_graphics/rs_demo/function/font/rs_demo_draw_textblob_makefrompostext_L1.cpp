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
测试类：textblob
测试接口：makefrompostext
测试内容：对接口入参text内容、text元素个数、位置、编码格式进行参数组合，通过drawtextblob接口将内容绘制在画布上。
*/

namespace OHOS {
namespace Rosen {

void MakeTextBlob(TestPlaybackCanvas* playbackCanvas, std::string name, std::string textInfo,
    size_t length, std::string posFlag, Drawing::TextEncoding encoding)
{
    int line = 200, interval1 = 100, interval2 = 200, interval3 = 300;
    Drawing::Brush brush;
    Drawing::Pen pen;
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    int cont = textInfo.size();
    Drawing::Point p[cont];
    switch (posFlag) {
        case "(10,20+100i)":
            for (int i = 0; i < cont; i++) {
                int x = 10, y = 20 + 100 * i;
                p[i].setX(x);
                p[i].SetY(y);
            }
            break;
        case "(-100+50i,1000-50i)":
            for (int i = 0; i < cont; i++) {
                int x = -100 + 50 * i, y = 1000 - 50 * i;
                p[i].setX(x);
                p[i].SetY(y);
            }
            break;
        case "(1000-50i,-100+100i)":
            for (int i = 0; i < cont; i++) {
                int x = 1000 - 50 * i, y = -100 + 100 * i;
                p[i].setX(x);
                p[i].SetY(y);
            }
            break;
    }
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 = Drawing::TextBlob::MakeFromPosText(
        textInfo.c_str(), length, p, font, encoding);
    playbackCanvas->AttachBrush(brush);
    playbackCanvas->DrawTextBlob(infoTextBlob1.get(), interval2, line);
    playbackCanvas->DetachBrush();
    playbackCanvas->AttachPen(pen);
    playbackCanvas->DrawTextBlob(infoTextBlob1.get(), interval3, line + interval1);
    playbackCanvas->DetachPen();
}

//对应用例makefrompostext_3001
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 1)
{
    MakeTextBlob(
        playbackCanvas_,
        "HarmonyOS Sans SC",
        "harmony_os",
        "harmony_os".size(),
        "(10,20+100i)",
        Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefrompostext_3002
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 2)
{
    MakeTextBlob(
        playbackCanvas_,
        "HarmonyOS Sans SC",
        "harmony_os",
        10,
        "(-100+50i,1000-50i)",
        Drawing::TextEncoding::UTF8);
}

//对应用例makefrompostext_3003
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 3)
{
    MakeTextBlob(
        playbackCanvas_,
        "HarmonyOS Sans SC",
        "harmony_os",
        "harmony_os".size() - 1,
        "(1000-50i,-100+100i)",
        Drawing::TextEncoding::UTF32);
}

//对应用例makefrompostext_3004
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 4)
{
    MakeTextBlob(
        playbackCanvas_,
        "HarmonyOS Sans SC",
        "harmony_os",
        "harmony_os".size() + 1,
        "(1000-50i,-100+100i)",
        Drawing::TextEncoding::UTF16);
}

//对应用例makefrompostext_3005
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 5)
{
    MakeTextBlob(
        playbackCanvas_,
        "HarmonyOS Sans SC",
        "harmony_os",
        0,
        "(10,20+100i)",
        Drawing::TextEncoding::UTF32);
}

//对应用例makefrompostext_3006
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 6)
{
    MakeTextBlob(
        playbackCanvas_,
        "HarmonyOS Sans SC",
        "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
        "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".size(),
        "(-100+50i,1000-50i)",
        Drawing::TextEncoding::UTF16);
}

//对应用例makefrompostext_3007
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 7)
{
    MakeTextBlob(
        playbackCanvas_,
        "HarmonyOS Sans SC",
        "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
        10,
        "(1000-50i,-100+100i)",
        Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefrompostext_3008
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 8)
{
    MakeTextBlob(
        playbackCanvas_,
        "HarmonyOS Sans SC",
        "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
        "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".size() - 1,
        "(10,20+100i)",
        Drawing::TextEncoding::UTF8);
}

//对应用例makefrompostext_3009
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 9)
{
    MakeTextBlob(
        playbackCanvas_,
        "HarmonyOS Sans SC",
        "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
        "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙".size() + 1,
        "(-100+50i,1000-50i)",
        Drawing::TextEncoding::UTF32);
}

//对应用例makefrompostext_3010
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 10)
{
    MakeTextBlob(
        playbackCanvas_,
        "HarmonyOS Sans SC",
        "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙",
        0,
        "(1000-50i,-100+100i)",
        Drawing::TextEncoding::UTF18);
}

//对应用例makefrompostext_3011
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 11)
{
    MakeTextBlob(
        playbackCanvas_,
        "HarmonyOS Sans SC",
        "!@#$%^&*()",
        "!@#$%^&*()".size(),
        "(10,20+100i)",
        Drawing::TextEncoding::UTF32);
}

//对应用例makefrompostext_3012
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 12)
{
    MakeTextBlob(
        playbackCanvas_,
        "HarmonyOS Sans SC",
        "!@#$%^&*()",
        "!@#$%^&*()".size(),
        "(1000-50i,-100+100i)",
        Drawing::TextEncoding::UTF8);
}

//对应用例makefrompostext_3013
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 13)
{
    MakeTextBlob(
        playbackCanvas_,
        "HarmonyOS Sans SC",
        "!@#$%^&*()",
        10,
        "(10,20+100i)",
        Drawing::TextEncoding::UTF16);
}

//对应用例makefrompostext_3014
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 14)
{
    MakeTextBlob(
        playbackCanvas_,
        "HarmonyOS Sans SC",
        "!@#$%^&*()",
        "!@#$%^&*()".size() - 1,
        "(-100+50i,1000-50i)",
        Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefrompostext_3015
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 15)
{
    MakeTextBlob(
        playbackCanvas_,
        "HarmonyOS Sans SC",
        "!@#$%^&*()",
        "!@#$%^&*()".size() + 1,
        "(10,20+100i)",
        Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefrompostext_3016
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 16)
{
    MakeTextBlob(
        playbackCanvas_,
        "HarmonyOS Sans SC",
        "!@#$%^&*()",
        0,
        "(-100+50i,1000-50i)",
        Drawing::TextEncoding::UTF16);
}

//对应用例makefrompostext_3017
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 17)
{
    MakeTextBlob(
        playbackCanvas_,
        "HarmonyOS Sans SC",
        "1234567890",
        "1234567890".size(),
        "(1000-50i,-100+100i)",
        Drawing::TextEncoding::UTF32);
}

//对应用例makefrompostext_3018
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 18)
{
    MakeTextBlob(
        playbackCanvas_,
        "HarmonyOS Sans SC",
        "1234567890",
        10,
        "(10,20+100i)",
        Drawing::TextEncoding::UTF32);
}

//对应用例makefrompostext_3019
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 19)
{
    MakeTextBlob(
        playbackCanvas_,
        "HarmonyOS Sans SC",
        "1234567890",
        "1234567890".size() - 1,
        "(-100+50i,1000-50i)",
        Drawing::TextEncoding::UTF16);
}

//对应用例makefrompostext_3020
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 20)
{
    MakeTextBlob(
        playbackCanvas_,
        "HarmonyOS Sans SC",
        "1234567890",
        "1234567890".size() + 1,
        "(10,20+100i)",
        Drawing::TextEncoding::UTF8);
}

//对应用例makefrompostext_3021
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 21)
{
    MakeTextBlob(
        playbackCanvas_,
        "HarmonyOS Sans SC",
        "1234567890",
        0,
        "(-100+50i,1000-50i)",
        Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefrompostext_3022
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 22)
{
    MakeTextBlob(
        playbackCanvas_,
        "HarmonyOS Sans SC",
        "harmony_os",
        "harmony_os".size(),
        "(-100+50i,1000-50i)",
        Drawing::TextEncoding::UTF32);
}

//对应用例makefrompostext_3023
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 23)
{
    MakeTextBlob(
        playbackCanvas_,
        "HarmonyOS Sans SC",
        nullptr,
        10,
        "(1000-50i,-100+100i)",
        Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefrompostext_3024
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 24)
{
    MakeTextBlob(
        playbackCanvas_,
        "HarmonyOS Sans SC",
        nullptr,
        "harmony_os".size() - 1,
        "(10,20+100i)",
        Drawing::TextEncoding::UTF8);
}

//对应用例makefrompostext_3025
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 25)
{
    MakeTextBlob(
        playbackCanvas_,
        "HarmonyOS Sans SC",
        nullptr,
        "harmony_os".size() + 1,
        "(1000-50i,-100+100i)",
        Drawing::TextEncoding::UTF16);
}

//对应用例makefrompostext_3026
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 26)
{
    MakeTextBlob(
        playbackCanvas_,
        "HarmonyOS Sans SC",
        nullptr,
        0,
        "(10,20+100i)",
        Drawing::TextEncoding::UTF8);
}

//对应用例makefrompostext_3027
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 27)
{
    MakeTextBlob(
        playbackCanvas_,
        "HMOS Color Emoji",
        "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99",
        "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99".size(),
        "(10,20+100i)",
        Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefrompostext_3028
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 28)
{
    MakeTextBlob(
        playbackCanvas_,
        "HMOS Color Emoji",
        "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99",
        10,
        "(-100+50i,1000-50i)",
        Drawing::TextEncoding::UTF8);
}

//对应用例makefrompostext_3029
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 29)
{
    MakeTextBlob(
        playbackCanvas_,
        "HMOS Color Emoji",
        "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99",
        "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99".size() - 1,
        "(1000-50i,-100+100i)",
        Drawing::TextEncoding::UTF16);
}

//对应用例makefrompostext_3030
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 30)
{
    MakeTextBlob(
        playbackCanvas_,
        "HMOS Color Emoji",
        "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99",
        "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99".size() + 1,
        "(-100+50i,1000-50i)",
        Drawing::TextEncoding::UTF32);
}

//对应用例makefrompostext_3031
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 31)
{
    MakeTextBlob(
        playbackCanvas_,
        "HMOS Color Emoji",
        "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99",
        0,
        "(1000-50i,-100+100i)",
        Drawing::TextEncoding::UTF16);
}

}
}