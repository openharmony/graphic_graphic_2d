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
测试类：textblob
测试接口：makefrompostext
测试内容：对接口入参text内容、text元素个数、位置、编码格式进行参数组合，通过drawtextblob接口将内容绘制在画布上。
*/

namespace OHOS {
namespace Rosen {

static void MakeTextBlob(TestPlaybackCanvas* playbackCanvas, std::string name, std::string textInfo,
    size_t length, Drawing::Point p[], Drawing::TextEncoding encoding)
{
    int line = 200, interval1 = 100, interval2 = 200, interval3 = 300;
    Drawing::Brush brush;
    Drawing::Pen pen;
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
   
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
DEF_DTK(textblob_makefrompostext, TestLevel::L1, 1)
{
    std::string textInfo = "harmony_os";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(10);
        p[i].SetY(20 + 100*i);
    }

    MakeTextBlob(
        playbackCanvas_,
        "HarmonyOS Sans SC",
        textInfo,
        textInfo.size(),
        p,
        Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefrompostext_3002
DEF_DTK(textblob_makefrompostext, TestLevel::L1, 2)
{
    std::string textInfo = "harmony_os";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(-100 + 50 * i);
        p[i].SetY(1000 - 50 * i);
    }

    MakeTextBlob(
        playbackCanvas_,
        "HarmonyOS Sans SC",
        textInfo,
        10,
        p,
        Drawing::TextEncoding::UTF8);
}

//对应用例makefrompostext_3003
DEF_DTK(textblob_makefrompostext, TestLevel::L1, 3)
{
    std::string textInfo = "harmony_os";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(1000-50*i);
        p[i].SetY(-100+100*i);
    }

    MakeTextBlob(
        playbackCanvas_,
        "HarmonyOS Sans SC",
        textInfo,
        textInfo.size() - 1,
        p,
        Drawing::TextEncoding::UTF32);
}

//对应用例makefrompostext_3004
DEF_DTK(textblob_makefrompostext, TestLevel::L1, 4)
{
    std::string textInfo = "harmony_os";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(1000-50*i);
        p[i].SetY(-100+100*i);
    }

    MakeTextBlob(
        playbackCanvas_,
        "HarmonyOS Sans SC",
        textInfo,
        textInfo.size() + 1,
        p,
        Drawing::TextEncoding::UTF16);
}

//对应用例makefrompostext_3005
DEF_DTK(textblob_makefrompostext, TestLevel::L1, 5)
{
    std::string textInfo = "harmony_os";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(10);
        p[i].SetY(20+100*i);
    }

    MakeTextBlob(
        playbackCanvas_,
        "HarmonyOS Sans SC",
        textInfo,
        0,
        p,
        Drawing::TextEncoding::UTF32);
}

//对应用例makefrompostext_3006
DEF_DTK(textblob_makefrompostext, TestLevel::L1, 6)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(-100+50*i);
        p[i].SetY(1000-50*i);
    }

    MakeTextBlob(
        playbackCanvas_,
        "HarmonyOS Sans SC",
        textInfo,
        textInfo.size(),
        p,
        Drawing::TextEncoding::UTF16);
}

//对应用例makefrompostext_3007
DEF_DTK(textblob_makefrompostext, TestLevel::L1, 7)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(1000-50*i);
        p[i].SetY(-100+100*i);
    }

    MakeTextBlob(
        playbackCanvas_,
        "HarmonyOS Sans SC",
        textInfo,
        10,
        p,
        Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefrompostext_3008
DEF_DTK(textblob_makefrompostext, TestLevel::L1, 8)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(10);
        p[i].SetY(20+100*i);
    }

    MakeTextBlob(
        playbackCanvas_,
        "HarmonyOS Sans SC",
        textInfo,
        textInfo.size() - 1,
        p,
        Drawing::TextEncoding::UTF8);
}

//对应用例makefrompostext_3009
DEF_DTK(textblob_makefrompostext, TestLevel::L1, 9)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(-100+50*i);
        p[i].SetY(1000-50*i);
    }

    MakeTextBlob(
        playbackCanvas_,
        "HarmonyOS Sans SC",
        textInfo,
        textInfo.size() + 1,
        p,
        Drawing::TextEncoding::UTF32);
}

//对应用例makefrompostext_3010
DEF_DTK(textblob_makefrompostext, TestLevel::L1, 10)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(1000-50*i);
        p[i].SetY(-100+100*i);
    }

    MakeTextBlob(
        playbackCanvas_,
        "HarmonyOS Sans SC",
        textInfo,
        0,
        p,
        Drawing::TextEncoding::UTF8);
}

//对应用例makefrompostext_3011
DEF_DTK(textblob_makefrompostext, TestLevel::L1, 11)
{
    std::string textInfo = "!@#$%^&*()";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(10);
        p[i].SetY(20+100*i);
    }

    MakeTextBlob(
        playbackCanvas_,
        "HarmonyOS Sans SC",
        textInfo,
        textInfo.size(),
        p,
        Drawing::TextEncoding::UTF32);
}

//对应用例makefrompostext_3012
DEF_DTK(textblob_makefrompostext, TestLevel::L1, 12)
{
    std::string textInfo = "!@#$%^&*()";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(1000-50*i);
        p[i].SetY(-100+100*i);
    }

    MakeTextBlob(
        playbackCanvas_,
        "HarmonyOS Sans SC",
        textInfo,
        textInfo.size(),
        p,
        Drawing::TextEncoding::UTF8);
}

//对应用例makefrompostext_3013
DEF_DTK(textblob_makefrompostext, TestLevel::L1, 13)
{
    std::string textInfo = "!@#$%^&*()";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(10);
        p[i].SetY(20+100*i);
    }

    MakeTextBlob(
        playbackCanvas_,
        "HarmonyOS Sans SC",
        textInfo,
        10,
        p,
        Drawing::TextEncoding::UTF16);
}

//对应用例makefrompostext_3014
DEF_DTK(textblob_makefrompostext, TestLevel::L1, 14)
{
    std::string textInfo = "!@#$%^&*()";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(-100+50*i);
        p[i].SetY(1000-50*i);
    }

    MakeTextBlob(
        playbackCanvas_,
        "HarmonyOS Sans SC",
        textInfo,
        textInfo.size() - 1,
        p,
        Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefrompostext_3015
DEF_DTK(textblob_makefrompostext, TestLevel::L1, 15)
{
    std::string textInfo = "!@#$%^&*()";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(10);
        p[i].SetY(20+100*i);
    }

    MakeTextBlob(
        playbackCanvas_,
        "HarmonyOS Sans SC",
        textInfo,
        textInfo.size() + 1,
        p,
        Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefrompostext_3016
DEF_DTK(textblob_makefrompostext, TestLevel::L1, 16)
{
    std::string textInfo = "!@#$%^&*()";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(-100+50*i);
        p[i].SetY(1000-50*i);
    }

    MakeTextBlob(
        playbackCanvas_,
        "HarmonyOS Sans SC",
        textInfo,
        0,
        p,
        Drawing::TextEncoding::UTF16);
}

//对应用例makefrompostext_3017
DEF_DTK(textblob_makefrompostext, TestLevel::L1, 17)
{
    std::string textInfo = "1234567890";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(1000-50*i);
        p[i].SetY(-100+100*i);
    }

    MakeTextBlob(
        playbackCanvas_,
        "HarmonyOS Sans SC",
        textInfo,
        textInfo.size(),
        p,
        Drawing::TextEncoding::UTF32);
}

//对应用例makefrompostext_3018
DEF_DTK(textblob_makefrompostext, TestLevel::L1, 18)
{
    std::string textInfo = "1234567890";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(10);
        p[i].SetY(20+100*i);
    }

    MakeTextBlob(
        playbackCanvas_,
        "HarmonyOS Sans SC",
        textInfo,
        10,
        p,
        Drawing::TextEncoding::UTF32);
}

//对应用例makefrompostext_3019
DEF_DTK(textblob_makefrompostext, TestLevel::L1, 19)
{
    std::string textInfo = "1234567890";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(-100+50*i);
        p[i].SetY(1000-50*i);
    }

    MakeTextBlob(
        playbackCanvas_,
        "HarmonyOS Sans SC",
        textInfo,
        textInfo.size() - 1,
        p,
        Drawing::TextEncoding::UTF16);
}

//对应用例makefrompostext_3020
DEF_DTK(textblob_makefrompostext, TestLevel::L1, 20)
{
    std::string textInfo = "1234567890";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(10);
        p[i].SetY(20+100*i);
    }

    MakeTextBlob(
        playbackCanvas_,
        "HarmonyOS Sans SC",
        textInfo,
        textInfo.size() + 1,
        p,
        Drawing::TextEncoding::UTF8);
}

//对应用例makefrompostext_3021
DEF_DTK(textblob_makefrompostext, TestLevel::L1, 21)
{
    std::string textInfo = "1234567890";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(-100+50*i);
        p[i].SetY(1000-50*i);
    }

    MakeTextBlob(
        playbackCanvas_,
        "HarmonyOS Sans SC",
        textInfo,
        0,
        p,
        Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefrompostext_3022
DEF_DTK(textblob_makefrompostext, TestLevel::L1, 22)
{
    std::string textInfo = "harmony_os";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(-100+50*i);
        p[i].SetY(1000-50*i);
    }

    MakeTextBlob(
        playbackCanvas_,
        "HarmonyOS Sans SC",
        textInfo,
        textInfo.size(),
        p,
        Drawing::TextEncoding::UTF32);
}

//对应用例makefrompostext_3023
DEF_DTK(textblob_makefrompostext, TestLevel::L1, 23)
{
    std::string textInfo = "harmony_os";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(1000-50*i);
        p[i].SetY(-100+100*i);
    }

    MakeTextBlob(
        playbackCanvas_,
        "HarmonyOS Sans SC",
        nullptr,
        10,
        p,
        Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefrompostext_3024
DEF_DTK(textblob_makefrompostext, TestLevel::L1, 24)
{
    std::string textInfo = "harmony_os";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(10);
        p[i].SetY(20+100*i);
    }

    MakeTextBlob(
        playbackCanvas_,
        "HarmonyOS Sans SC",
        nullptr,
        textInfo.size() - 1,
        p,
        Drawing::TextEncoding::UTF8);
}

//对应用例makefrompostext_3025
DEF_DTK(textblob_makefrompostext, TestLevel::L1, 25)
{
    std::string textInfo = "harmony_os";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(1000-50*i);
        p[i].SetY(-100+100*i);
    }

    MakeTextBlob(
        playbackCanvas_,
        "HarmonyOS Sans SC",
        nullptr,
        textInfo.size() + 1,
        p,
        Drawing::TextEncoding::UTF16);
}

//对应用例makefrompostext_3026
DEF_DTK(textblob_makefrompostext, TestLevel::L1, 26)
{
    std::string textInfo = "harmony_os";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(10);
        p[i].SetY(20+100*i);
    }

    MakeTextBlob(
        playbackCanvas_,
        "HarmonyOS Sans SC",
        nullptr,
        0,
        p,
        Drawing::TextEncoding::UTF8);
}

//对应用例makefrompostext_3027
DEF_DTK(textblob_makefrompostext, TestLevel::L1, 27)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(10);
        p[i].SetY(20+100*i);
    }

    MakeTextBlob(
        playbackCanvas_,
        "HMOS Color Emoji",
        textInfo,
        textInfo.size(),
        p,
        Drawing::TextEncoding::GLYPH_ID);
}

//对应用例makefrompostext_3028
DEF_DTK(textblob_makefrompostext, TestLevel::L1, 28)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(-100+50*i);
        p[i].SetY(1000-50*i);
    }

    MakeTextBlob(
        playbackCanvas_,
        "HMOS Color Emoji",
        textInfo,
        10,
        p,
        Drawing::TextEncoding::UTF8);
}

//对应用例makefrompostext_3029
DEF_DTK(textblob_makefrompostext, TestLevel::L1, 29)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(1000-50*i);
        p[i].SetY(-100+100*i);
    }

    MakeTextBlob(
        playbackCanvas_,
        "HMOS Color Emoji",
        textInfo,
        textInfo.size() - 1,
        p,
        Drawing::TextEncoding::UTF16);
}

//对应用例makefrompostext_3030
DEF_DTK(textblob_makefrompostext, TestLevel::L1, 30)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(-100+50*i);
        p[i].SetY(1000-50*i);
    }

    MakeTextBlob(
        playbackCanvas_,
        "HMOS Color Emoji",
        textInfo,
        textInfo.size() + 1,
        p,
        Drawing::TextEncoding::UTF32);
}

//对应用例makefrompostext_3031
DEF_DTK(textblob_makefrompostext, TestLevel::L1, 31)
{
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(1000-50*i);
        p[i].SetY(-100+100*i);
    }

    MakeTextBlob(
        playbackCanvas_,
        "HMOS Color Emoji",
        textInfo,
        0,
        p,
        Drawing::TextEncoding::UTF16);
}

}
}