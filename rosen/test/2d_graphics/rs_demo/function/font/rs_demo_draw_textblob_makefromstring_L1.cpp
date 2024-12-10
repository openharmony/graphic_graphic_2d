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
#include "text/font_mgr.h"
#include "text/font_style_set.h"
#include "text/text_style.h"
#include "text/typeface.h"

namespace OHOS {
namespace Rosen {

//对应用例makefromstring_3001
DEF_RSDEMO(textblob_makefromstring, TestLevel::L1, 1)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    playbackCanvas_->AttachBrush(brush);
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS Sans SC";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "harmony_os";
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 =
        Drawing::TextBlob::MakeFromString(textInfo.c_str(), textInfo.size(), font, Drawing::TextEncoding::GLYPH_ID);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 200);
    playbackCanvas_->DetachBrush();

    playbackCanvas_->AttachPen(pen);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 300);
    playbackCanvas_->DetachPen();
}

//对应用例makefromstring_3002
DEF_RSDEMO(textblob_makefromstring, TestLevel::L1, 2)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    playbackCanvas_->AttachBrush(brush);
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS Sans SC";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "harmony_os";
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 =
        Drawing::TextBlob::MakeFromString(textInfo.c_str(), 10, font, Drawing::TextEncoding::UTF8);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 200);
    playbackCanvas_->DetachBrush();

    playbackCanvas_->AttachPen(pen);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 300);
    playbackCanvas_->DetachPen();
}

//对应用例makefromstring_3003
DEF_RSDEMO(textblob_makefromstring, TestLevel::L1, 3)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    playbackCanvas_->AttachBrush(brush);
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS Sans SC";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "harmony_os";
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 =
        Drawing::TextBlob::MakeFromString(textInfo.c_str(), textInfo.size()-1, font, Drawing::TextEncoding::UTF32);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 200);
    playbackCanvas_->DetachBrush();

    playbackCanvas_->AttachPen(pen);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 300);
    playbackCanvas_->DetachPen();
}

//对应用例makefromstring_3004
DEF_RSDEMO(textblob_makefromstring, TestLevel::L1, 4)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    playbackCanvas_->AttachBrush(brush);
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS Sans SC";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "harmony_os";
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 =
        Drawing::TextBlob::MakeFromString(textInfo.c_str(), textInfo.size()+1, font, Drawing::TextEncoding::UTF16);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 200);
    playbackCanvas_->DetachBrush();

    playbackCanvas_->AttachPen(pen);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 300);
    playbackCanvas_->DetachPen();
}

//对应用例makefromstring_3005
DEF_RSDEMO(textblob_makefromstring, TestLevel::L1, 5)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    playbackCanvas_->AttachBrush(brush);
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS Sans SC";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "harmony_os";
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 =
        Drawing::TextBlob::MakeFromString(textInfo.c_str(), 0, font, Drawing::TextEncoding::UTF32);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 200);
    playbackCanvas_->DetachBrush();

    playbackCanvas_->AttachPen(pen);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 300);
    playbackCanvas_->DetachPen();
}

//对应用例makefromstring_3006
DEF_RSDEMO(textblob_makefromstring, TestLevel::L1, 6)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    playbackCanvas_->AttachBrush(brush);
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS Sans SC";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 =
        Drawing::TextBlob::MakeFromString(textInfo.c_str(), textInfo.size(), font, Drawing::TextEncoding::UTF8);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 200);
    playbackCanvas_->DetachBrush();

    playbackCanvas_->AttachPen(pen);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 300);
    playbackCanvas_->DetachPen();
}

//对应用例makefromstring_3007
DEF_RSDEMO(textblob_makefromstring, TestLevel::L1, 7)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    playbackCanvas_->AttachBrush(brush);
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS Sans SC";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 =
        Drawing::TextBlob::MakeFromString(textInfo.c_str(), 10, font, Drawing::TextEncoding::UTF16);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 200);
    playbackCanvas_->DetachBrush();

    playbackCanvas_->AttachPen(pen);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 300);
    playbackCanvas_->DetachPen();
}

//对应用例makefromstring_3008
DEF_RSDEMO(textblob_makefromstring, TestLevel::L1, 8)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    playbackCanvas_->AttachBrush(brush);
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS Sans SC";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 =
        Drawing::TextBlob::MakeFromString(textInfo.c_str(), textInfo.size()-1, font, Drawing::TextEncoding::GLYPH_ID);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 200);
    playbackCanvas_->DetachBrush();

    playbackCanvas_->AttachPen(pen);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 300);
    playbackCanvas_->DetachPen();
}

//对应用例makefromstring_3009
DEF_RSDEMO(textblob_makefromstring, TestLevel::L1, 9)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    playbackCanvas_->AttachBrush(brush);
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS Sans SC";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 =
        Drawing::TextBlob::MakeFromString(textInfo.c_str(), textInfo.size()+1, font, Drawing::TextEncoding::UTF32);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 200);
    playbackCanvas_->DetachBrush();

    playbackCanvas_->AttachPen(pen);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 300);
    playbackCanvas_->DetachPen();
}

//对应用例makefromstring_3010
DEF_RSDEMO(textblob_makefromstring, TestLevel::L1, 10)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    playbackCanvas_->AttachBrush(brush);
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS Sans SC";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 =
        Drawing::TextBlob::MakeFromString(textInfo.c_str(), 0, font, Drawing::TextEncoding::GLYPH_ID);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 200);
    playbackCanvas_->DetachBrush();

    playbackCanvas_->AttachPen(pen);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 300);
    playbackCanvas_->DetachPen();
}

//对应用例makefromstring_3011
DEF_RSDEMO(textblob_makefromstring, TestLevel::L1, 11)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    playbackCanvas_->AttachBrush(brush);
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS Sans SC";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "!@#$%^&*()";
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 =
        Drawing::TextBlob::MakeFromString(textInfo.c_str(), textInfo.size(), font, Drawing::TextEncoding::UTF16);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 200);
    playbackCanvas_->DetachBrush();

    playbackCanvas_->AttachPen(pen);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 300);
    playbackCanvas_->DetachPen();
}

//对应用例makefromstring_3012
DEF_RSDEMO(textblob_makefromstring, TestLevel::L1, 12)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    playbackCanvas_->AttachBrush(brush);
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS Sans SC";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "!@#$%^&*()";
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 =
        Drawing::TextBlob::MakeFromString(textInfo.c_str(), 10, font, Drawing::TextEncoding::UTF32);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 200);
    playbackCanvas_->DetachBrush();

    playbackCanvas_->AttachPen(pen);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 300);
    playbackCanvas_->DetachPen();
}

//对应用例makefromstring_3013
DEF_RSDEMO(textblob_makefromstring, TestLevel::L1, 13)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    playbackCanvas_->AttachBrush(brush);
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS Sans SC";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "!@#$%^&*()";
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 =
        Drawing::TextBlob::MakeFromString(textInfo.c_str(), textInfo.size()-1, font, Drawing::TextEncoding::UTF8);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 200);
    playbackCanvas_->DetachBrush();

    playbackCanvas_->AttachPen(pen);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 300);
    playbackCanvas_->DetachPen();
}

//对应用例makefromstring_3014
DEF_RSDEMO(textblob_makefromstring, TestLevel::L1, 14)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    playbackCanvas_->AttachBrush(brush);
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS Sans SC";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "!@#$%^&*()";
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 =
        Drawing::TextBlob::MakeFromString(textInfo.c_str(), textInfo.size()+1, font, Drawing::TextEncoding::GLYPH_ID);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 200);
    playbackCanvas_->DetachBrush();

    playbackCanvas_->AttachPen(pen);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 300);
    playbackCanvas_->DetachPen();
}

//对应用例makefromstring_3015
DEF_RSDEMO(textblob_makefromstring, TestLevel::L1, 15)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    playbackCanvas_->AttachBrush(brush);
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS Sans SC";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "!@#$%^&*()";
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 =
        Drawing::TextBlob::MakeFromString(textInfo.c_str(), 0, font, Drawing::TextEncoding::UTF8);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 200);
    playbackCanvas_->DetachBrush();

    playbackCanvas_->AttachPen(pen);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 300);
    playbackCanvas_->DetachPen();
}

//对应用例makefromstring_3016
DEF_RSDEMO(textblob_makefromstring, TestLevel::L1, 16)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    playbackCanvas_->AttachBrush(brush);
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS Sans SC";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "1234567890";
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 =
        Drawing::TextBlob::MakeFromString(textInfo.c_str(), textInfo.size(), font, Drawing::TextEncoding::UTF32);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 200);
    playbackCanvas_->DetachBrush();

    playbackCanvas_->AttachPen(pen);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 300);
    playbackCanvas_->DetachPen();
}

//对应用例makefromstring_3017
DEF_RSDEMO(textblob_makefromstring, TestLevel::L1, 17)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    playbackCanvas_->AttachBrush(brush);
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS Sans SC";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "1234567890";
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 =
        Drawing::TextBlob::MakeFromString(textInfo.c_str(), 10, font, Drawing::TextEncoding::GLYPH_ID);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 200);
    playbackCanvas_->DetachBrush();

    playbackCanvas_->AttachPen(pen);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 300);
    playbackCanvas_->DetachPen();
}

//对应用例makefromstring_3018
DEF_RSDEMO(textblob_makefromstring, TestLevel::L1, 18)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    playbackCanvas_->AttachBrush(brush);
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS Sans SC";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "1234567890";
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 =
        Drawing::TextBlob::MakeFromString(textInfo.c_str(), textInfo.size()-1, font, Drawing::TextEncoding::UTF16);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 200);
    playbackCanvas_->DetachBrush();

    playbackCanvas_->AttachPen(pen);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 300);
    playbackCanvas_->DetachPen();
}

//对应用例makefromstring_3019
DEF_RSDEMO(textblob_makefromstring, TestLevel::L1, 19)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    playbackCanvas_->AttachBrush(brush);
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS Sans SC";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "1234567890";
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 =
        Drawing::TextBlob::MakeFromString(textInfo.c_str(), textInfo.size()+1, font, Drawing::TextEncoding::UTF8);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 200);
    playbackCanvas_->DetachBrush();

    playbackCanvas_->AttachPen(pen);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 300);
    playbackCanvas_->DetachPen();
}

//对应用例makefromstring_3020
DEF_RSDEMO(textblob_makefromstring, TestLevel::L1, 20)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    playbackCanvas_->AttachBrush(brush);
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS Sans SC";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "1234567890";
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 =
        Drawing::TextBlob::MakeFromString(textInfo.c_str(), 0, font, Drawing::TextEncoding::UTF16);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 200);
    playbackCanvas_->DetachBrush();

    playbackCanvas_->AttachPen(pen);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 300);
    playbackCanvas_->DetachPen();
}

//对应用例makefromstring_3021
DEF_RSDEMO(textblob_makefromstring, TestLevel::L1, 21)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    playbackCanvas_->AttachBrush(brush);
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS Sans SC";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "harmony_os";
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 =
        Drawing::TextBlob::MakeFromString(nullptr, textInfo.size(), font, Drawing::TextEncoding::UTF32);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 200);
    playbackCanvas_->DetachBrush();

    playbackCanvas_->AttachPen(pen);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 300);
    playbackCanvas_->DetachPen();
}

//对应用例makefromstring_3022
DEF_RSDEMO(textblob_makefromstring, TestLevel::L1, 22)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    playbackCanvas_->AttachBrush(brush);
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS Sans SC";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "harmony_os";
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 =
        Drawing::TextBlob::MakeFromString(nullptr, 10, font, Drawing::TextEncoding::GLYPH_ID);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 200);
    playbackCanvas_->DetachBrush();

    playbackCanvas_->AttachPen(pen);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 300);
    playbackCanvas_->DetachPen();
}

//对应用例makefromstring_3023
DEF_RSDEMO(textblob_makefromstring, TestLevel::L1, 23)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    playbackCanvas_->AttachBrush(brush);
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS Sans SC";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "harmony_os";
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 =
        Drawing::TextBlob::MakeFromString(nullptr, textInfo.size()-1, font, Drawing::TextEncoding::UTF8);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 200);
    playbackCanvas_->DetachBrush();

    playbackCanvas_->AttachPen(pen);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 300);
    playbackCanvas_->DetachPen();
}

//对应用例makefromstring_3024
DEF_RSDEMO(textblob_makefromstring, TestLevel::L1, 24)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    playbackCanvas_->AttachBrush(brush);
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS Sans SC";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "harmony_os";
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 =
        Drawing::TextBlob::MakeFromString(nullptr, textInfo.size()+1, font, Drawing::TextEncoding::UTF16);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 200);
    playbackCanvas_->DetachBrush();

    playbackCanvas_->AttachPen(pen);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 300);
    playbackCanvas_->DetachPen();
}

//对应用例makefromstring_3025
DEF_RSDEMO(textblob_makefromstring, TestLevel::L1, 25)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    playbackCanvas_->AttachBrush(brush);
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS Sans SC";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "harmony_os";
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 =
        Drawing::TextBlob::MakeFromString(nullptr, 0, font, Drawing::TextEncoding::UTF8);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 200);
    playbackCanvas_->DetachBrush();

    playbackCanvas_->AttachPen(pen);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 300);
    playbackCanvas_->DetachPen();
}

//对应用例makefromstring_3026
DEF_RSDEMO(textblob_makefromstring, TestLevel::L1, 26)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    playbackCanvas_->AttachBrush(brush);
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 =
        Drawing::TextBlob::MakeFromString(textInfo.c_str(), textInfo.size(), font, Drawing::TextEncoding::GLYPH_ID);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 200);
    playbackCanvas_->DetachBrush();

    playbackCanvas_->AttachPen(pen);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 300);
    playbackCanvas_->DetachPen();
}

//对应用例makefromstring_3027
DEF_RSDEMO(textblob_makefromstring, TestLevel::L1, 27)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    playbackCanvas_->AttachBrush(brush);
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 =
        Drawing::TextBlob::MakeFromString(textInfo.c_str(), 10, font, Drawing::TextEncoding::UTF8);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 200);
    playbackCanvas_->DetachBrush();

    playbackCanvas_->AttachPen(pen);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 300);
    playbackCanvas_->DetachPen();
}

//对应用例makefromstring_3028
DEF_RSDEMO(textblob_makefromstring, TestLevel::L1, 28)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    playbackCanvas_->AttachBrush(brush);
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 =
        Drawing::TextBlob::MakeFromString(textInfo.c_str(), textInfo.size()-1, font, Drawing::TextEncoding::UTF16);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 200);
    playbackCanvas_->DetachBrush();

    playbackCanvas_->AttachPen(pen);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 300);
    playbackCanvas_->DetachPen();
}

//对应用例makefromstring_3029
DEF_RSDEMO(textblob_makefromstring, TestLevel::L1, 29)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    playbackCanvas_->AttachBrush(brush);
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 =
        Drawing::TextBlob::MakeFromString(textInfo.c_str(), textInfo.size()+1, font, Drawing::TextEncoding::UTF32);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 200);
    playbackCanvas_->DetachBrush();

    playbackCanvas_->AttachPen(pen);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 300);
    playbackCanvas_->DetachPen();
}

//对应用例makefromstring_3030
DEF_RSDEMO(textblob_makefromstring, TestLevel::L1, 30)
{
    Drawing::Brush brush;
    Drawing::Pen pen;
    playbackCanvas_->AttachBrush(brush);
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyle> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 =
        Drawing::TextBlob::MakeFromString(textInfo.c_str(), 0, font, Drawing::TextEncoding::UTF16);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 200);
    playbackCanvas_->DetachBrush();

    playbackCanvas_->AttachPen(pen);
    playbackCanvas_->DrawTextBlob(infoTextBlob1.get(), 200, 300);
    playbackCanvas_->DetachPen();
}

}
}
