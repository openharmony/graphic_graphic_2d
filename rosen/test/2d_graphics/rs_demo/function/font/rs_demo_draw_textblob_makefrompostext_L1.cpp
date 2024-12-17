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

void DrawTexts(std::shared_ptr<Drawing::TextBlob>& infoTextBlob1, TestPlaybackCanvas* playbackCanvas)
{
    int line = 200;
    int interval1 = 100;
    int interval2 = 200;
    int interval3 = 300;
    Drawing::Brush brush;
    playbackCanvas->AttachBrush(brush);
    playbackCanvas->DrawTextBlob(infoTextBlob1.get(), interval2, line);
    playbackCanvas->DetachBrush();
    Drawing::Pen pen;
    playbackCanvas->AttachPen(pen);
    playbackCanvas->DrawTextBlob(infoTextBlob1.get(), interval3, line + interval1);
    playbackCanvas->DetachPen();
}

//对应用例makefrompostext_3001
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 1)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS Sans SC";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "harmony_os";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(10);
        p[i].SetY(20+100 * i);
    }
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 = Drawing::TextBlob::MakeFromPosText(
        textInfo.c_str(),
        textInfo.size(),
        p,
        font,
        Drawing::TextEncoding::GLYPH_ID
    );
    DrawTexts(infoTextBlob1, playbackCanvas_);
}

//对应用例makefrompostext_3002
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 2)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS Sans SC";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "harmony_os";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(-100+50 * i);
        p[i].SetY(1000-50 * i);
    }
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 = Drawing::TextBlob::MakeFromPosText(
        textInfo.c_str(),
        10,
        p,
        font,
        Drawing::TextEncoding::UTF8
    );
    DrawTexts(infoTextBlob1, playbackCanvas_);
}

//对应用例makefrompostext_3003
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 3)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS Sans SC";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "harmony_os";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(1000-50 * i);
        p[i].SetY(-100+100 * i);
    }
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 = Drawing::TextBlob::MakeFromPosText(
        textInfo.c_str(),
        textInfo.size()-1,
        p,
        font,
        Drawing::TextEncoding::UTF32
    );
    DrawTexts(infoTextBlob1, playbackCanvas_);
}

//对应用例makefrompostext_3004
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 4)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS Sans SC";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "harmony_os";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(1000-50 * i);
        p[i].SetY(-100+100 * i);
    }
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 = Drawing::TextBlob::MakeFromPosText(
        textInfo.c_str(),
        textInfo.size()+1,
        p,
        font,
        Drawing::TextEncoding::UTF16
    );
    DrawTexts(infoTextBlob1, playbackCanvas_);
}

//对应用例makefrompostext_3005
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 5)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS Sans SC";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "harmony_os";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(10);
        p[i].SetY(20+100 * i);
    }
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 = Drawing::TextBlob::MakeFromPosText(
        textInfo.c_str(),
        0,
        p,
        font,
        Drawing::TextEncoding::UTF32
    );
    DrawTexts(infoTextBlob1, playbackCanvas_);
}

//对应用例makefrompostext_3006
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 6)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS Sans SC";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(-100+50 * i);
        p[i].SetY(1000-50 * i);
    }
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 = Drawing::TextBlob::MakeFromPosText(
        textInfo.c_str(),
        textInfo.size(),
        p,
        font,
        Drawing::TextEncoding::UTF16
    );
    DrawTexts(infoTextBlob1, playbackCanvas_);
}

//对应用例makefrompostext_3007
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 7)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS Sans SC";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(1000-50 * i);
        p[i].SetY(-100+100 * i);
    }
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 = Drawing::TextBlob::MakeFromPosText(
        textInfo.c_str(),
        10,
        p,
        font,
        Drawing::TextEncoding::GLYPH_ID
    );
    DrawTexts(infoTextBlob1, playbackCanvas_);
}

//对应用例makefrompostext_3008
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 8)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS Sans SC";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(10);
        p[i].SetY(20+100 * i);
    }
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 = Drawing::TextBlob::MakeFromPosText(
        textInfo.c_str(),
        textInfo.size()-1,
        p,
        font,
        Drawing::TextEncoding::UTF8
    );
    DrawTexts(infoTextBlob1, playbackCanvas_);
}

//对应用例makefrompostext_3009
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 9)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS Sans SC";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(-100+50 * i);
        p[i].SetY(1000-50 * i);
    }
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 = Drawing::TextBlob::MakeFromPosText(
        textInfo.c_str(),
        textInfo.size()+1,
        p,
        font,
        Drawing::TextEncoding::UTF32
    );
    DrawTexts(infoTextBlob1, playbackCanvas_);
}

//对应用例makefrompostext_3010
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 10)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS Sans SC";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(1000-50 * i);
        p[i].SetY(-100+100 * i);
    }
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 = Drawing::TextBlob::MakeFromPosText(
        textInfo.c_str(),
        0,
        p,
        font,
        Drawing::TextEncoding::UTF8
    );
    DrawTexts(infoTextBlob1, playbackCanvas_);
}

//对应用例makefrompostext_3011
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 11)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS Sans SC";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "!@#$%^&*()";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(10);
        p[i].SetY(20+100 * i);
    }
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 = Drawing::TextBlob::MakeFromPosText(
        textInfo.c_str(),
        textInfo.size(),
        p,
        font,
        Drawing::TextEncoding::UTF32
    );
    DrawTexts(infoTextBlob1, playbackCanvas_);
}

//对应用例makefrompostext_3012
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 12)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS Sans SC";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "!@#$%^&*()";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(1000-50 * i);
        p[i].SetY(-100+100 * i);
    }
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 = Drawing::TextBlob::MakeFromPosText(
        textInfo.c_str(),
        textInfo.size(),
        p,
        font,
        Drawing::TextEncoding::UTF8
    );
    DrawTexts(infoTextBlob1, playbackCanvas_);
}

//对应用例makefrompostext_3013
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 13)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS Sans SC";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "!@#$%^&*()";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(10);
        p[i].SetY(20+100 * i);
    }
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 = Drawing::TextBlob::MakeFromPosText(
        textInfo.c_str(),
        10,
        p,
        font,
        Drawing::TextEncoding::UTF16
    );
    DrawTexts(infoTextBlob1, playbackCanvas_);
}

//对应用例makefrompostext_3014
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 14)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS Sans SC";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "!@#$%^&*()";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(-100+50 * i);
        p[i].SetY(1000-50 * i);
    }
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 = Drawing::TextBlob::MakeFromPosText(
        textInfo.c_str(),
        textInfo.size()-1,
        p,
        font,
        Drawing::TextEncoding::GLYPH_ID
    );
    DrawTexts(infoTextBlob1, playbackCanvas_);
}

//对应用例makefrompostext_3015
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 15)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS Sans SC";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "!@#$%^&*()";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(10);
        p[i].SetY(20+100 * i);
    }
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 = Drawing::TextBlob::MakeFromPosText(
        textInfo.c_str(),
        textInfo.size()+1,
        p,
        font,
        Drawing::TextEncoding::GLYPH_ID
    );
    DrawTexts(infoTextBlob1, playbackCanvas_);
}

//对应用例makefrompostext_3016
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 16)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS Sans SC";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "!@#$%^&*()";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(-100+50 * i);
        p[i].SetY(1000-50 * i);
    }
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 = Drawing::TextBlob::MakeFromPosText(
        textInfo.c_str(),
        0,
        p,
        font,
        Drawing::TextEncoding::UTF16
    );
    DrawTexts(infoTextBlob1, playbackCanvas_);
}

//对应用例makefrompostext_3017
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 17)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS Sans SC";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "1234567890";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(1000-50 * i);
        p[i].SetY(-100+100 * i);
    }
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 = Drawing::TextBlob::MakeFromPosText(
        textInfo.c_str(),
        textInfo.size(),
        p,
        font,
        Drawing::TextEncoding::UTF32
    );
    DrawTexts(infoTextBlob1, playbackCanvas_);
}

//对应用例makefrompostext_3018
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 18)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS Sans SC";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "1234567890";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(10);
        p[i].SetY(20+100 * i);
    }
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 = Drawing::TextBlob::MakeFromPosText(
        textInfo.c_str(),
        10,
        p,
        font,
        Drawing::TextEncoding::UTF32
    );
    DrawTexts(infoTextBlob1, playbackCanvas_);
}

//对应用例makefrompostext_3019
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 19)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS Sans SC";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "1234567890";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(-100+50 * i);
        p[i].SetY(1000-50 * i);
    }
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 = Drawing::TextBlob::MakeFromPosText(
        textInfo.c_str(),
        textInfo.size()-1,
        p,
        font,
        Drawing::TextEncoding::UTF16
    );
    DrawTexts(infoTextBlob1, playbackCanvas_);
}

//对应用例makefrompostext_3020
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 20)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS Sans SC";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "1234567890";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(10);
        p[i].SetY(20+100 * i);
    }
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 = Drawing::TextBlob::MakeFromPosText(
        textInfo.c_str(),
        textInfo.size()+1,
        p,
        font,
        Drawing::TextEncoding::UTF8
    );
    DrawTexts(infoTextBlob1, playbackCanvas_);
}

//对应用例makefrompostext_3021
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 21)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS Sans SC";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "1234567890";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(-100+50 * i);
        p[i].SetY(1000-50 * i);
    }
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 = Drawing::TextBlob::MakeFromPosText(
        textInfo.c_str(),
        0,
        p,
        font,
        Drawing::TextEncoding::GLYPH_ID
    );
    DrawTexts(infoTextBlob1, playbackCanvas_);
}

//对应用例makefrompostext_3022
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 22)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS Sans SC";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "harmony_os";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(-100+50 * i);
        p[i].SetY(1000-50 * i);
    }
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 = Drawing::TextBlob::MakeFromPosText(
        nullptr,
        textInfo.size(),
        p,
        font,
        Drawing::TextEncoding::UTF32
    );
    DrawTexts(infoTextBlob1, playbackCanvas_);
}

//对应用例makefrompostext_3023
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 23)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS Sans SC";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "harmony_os";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(1000-50 * i);
        p[i].SetY(-100+100 * i);
    }
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 = Drawing::TextBlob::MakeFromPosText(
        nullptr,
        10,
        p,
        font,
        Drawing::TextEncoding::GLYPH_ID
    );
    DrawTexts(infoTextBlob1, playbackCanvas_);
}

//对应用例makefrompostext_3024
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 24)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS Sans SC";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "harmony_os";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(10);
        p[i].SetY(20+100 * i);
    }
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 = Drawing::TextBlob::MakeFromPosText(
        nullptr,
        textInfo.size()-1,
        p,
        font,
        Drawing::TextEncoding::UTF8
    );
    DrawTexts(infoTextBlob1, playbackCanvas_);
}

//对应用例makefrompostext_3025
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 25)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS Sans SC";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "harmony_os";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(1000-50 * i);
        p[i].SetY(-100+100 * i);
    }
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 = Drawing::TextBlob::MakeFromPosText(
        nullptr,
        textInfo.size()+1,
        p,
        font,
        Drawing::TextEncoding::UTF16
    );
    DrawTexts(infoTextBlob1, playbackCanvas_);
}

//对应用例makefrompostext_3026
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 26)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS Sans SC";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "harmony_os";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(10);
        p[i].SetY(20+100 * i);
    }
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 = Drawing::TextBlob::MakeFromPosText(
        nullptr,
        0,
        p,
        font,
        Drawing::TextEncoding::UTF8
    );
    DrawTexts(infoTextBlob1, playbackCanvas_);
}

//对应用例makefrompostext_3027
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 27)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(10);
        p[i].SetY(20+100 * i);
    }
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 = Drawing::TextBlob::MakeFromPosText(
        textInfo.c_str(),
        textInfo.size(),
        p,
        font,
        Drawing::TextEncoding::GLYPH_ID
    );
    DrawTexts(infoTextBlob1, playbackCanvas_);
}

//对应用例makefrompostext_3028
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 28)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(-100+50 * i);
        p[i].SetY(1000-50 * i);
    }
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 = Drawing::TextBlob::MakeFromPosText(
        textInfo.c_str(),
        10,
        p,
        font,
        Drawing::TextEncoding::UTF8
    );
    DrawTexts(infoTextBlob1, playbackCanvas_);
}

//对应用例makefrompostext_3029
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 29)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(1000-50 * i);
        p[i].SetY(-100+100 * i);
    }
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 = Drawing::TextBlob::MakeFromPosText(
        textInfo.c_str(),
        textInfo.size()-1,
        p,
        font,
        Drawing::TextEncoding::UTF16
    );
    DrawTexts(infoTextBlob1, playbackCanvas_);
}

//对应用例makefrompostext_3030
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 30)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(-100+50 * i);
        p[i].SetY(1000-50 * i);
    }
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 = Drawing::TextBlob::MakeFromPosText(
        textInfo.c_str(),
        textInfo.size()+1,
        p,
        font,
        Drawing::TextEncoding::UTF32
    );
    DrawTexts(infoTextBlob1, playbackCanvas_);
}

//对应用例makefrompostext_3031
DEF_RSDEMO(textblob_makefrompostext, TestLevel::L1, 31)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(1000-50 * i);
        p[i].SetY(-100+100 * i);
    }
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 = Drawing::TextBlob::MakeFromPosText(
        textInfo.c_str(),
        0,
        p,
        font,
        Drawing::TextEncoding::UTF16
    );
    DrawTexts(infoTextBlob1, playbackCanvas_);
}

}
}
