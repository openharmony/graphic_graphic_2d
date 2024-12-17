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
#include "text/text_blob_builder.h"
#include "text/typeface.h"

namespace OHOS {
namespace Rosen {

void DrawText(Drawing::TextBlobBuilder& builder, TestPlaybackCanvas* playbackCanvas) {
    Drawing::Brush brush;
    Drawing::Pen pen;

    playbackCanvas->AttachBrush(brush);
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 = builder.Make();
    playbackCanvas->DrawTextBlob(infoTextBlob1.get(), 200, 200);
    playbackCanvas->DetachBrush();

    playbackCanvas->AttachPen(pen);
    playbackCanvas->DrawTextBlob(infoTextBlob1.get(), 200, 500);
    playbackCanvas->DetachPen();
}

//对应用例allocrunpos_3041
DEF_RSDEMO(textblobbuilder_allocrunpos, TestLevel::L1, 41)
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
    font.SetSize(100.f);
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(100, 100, 1000, 1500);
    auto buffer = builder.AllocRunPos(font, 0, &rect1);
    for (int i = 0; i < 0; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 50.f*i;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3042
DEF_RSDEMO(textblobbuilder_allocrunpos, TestLevel::L1, 42)
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
    font.SetSize(100.f);
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(100, 100, 1000, 1500);
    auto buffer = builder.AllocRunPos(font, 0, &rect1);
    for (int i = 0; i < 0; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 0;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3043
DEF_RSDEMO(textblobbuilder_allocrunpos, TestLevel::L1, 43)
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
    font.SetSize(100.f);
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(100, 100, 1000, 1500);
    auto buffer = builder.AllocRunPos(font, 0, &rect1);
    for (int i = 0; i < 0; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 50.f*i;
        buffer.pos[i * 2 + 1] = 0;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3044
DEF_RSDEMO(textblobbuilder_allocrunpos, TestLevel::L1, 44)
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
    font.SetSize(100.f);
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(100, 100, 1000, 1500);
    auto buffer = builder.AllocRunPos(font, 0, &rect1);
    for (int i = 0; i < 0; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 1000.0f-50.f*i;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3045
DEF_RSDEMO(textblobbuilder_allocrunpos, TestLevel::L1, 45)
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
    font.SetSize(100.f);
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(-100, -100, 1000, 1500);
    auto buffer = builder.AllocRunPos(font, 0, &rect1);
    for (int i = 0; i < 0; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 50.f*i;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3046
DEF_RSDEMO(textblobbuilder_allocrunpos, TestLevel::L1, 46)
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
    font.SetSize(100.f);
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(-100, -100, 1000, 1500);
    auto buffer = builder.AllocRunPos(font, 0, &rect1);
    for (int i = 0; i < 0; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 0;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3047
DEF_RSDEMO(textblobbuilder_allocrunpos, TestLevel::L1, 47)
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
    font.SetSize(100.f);
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(-100, -100, 1000, 1500);
    auto buffer = builder.AllocRunPos(font, 0, &rect1);
    for (int i = 0; i < 0; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 50.f*i;
        buffer.pos[i * 2 + 1] = 0;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3048
DEF_RSDEMO(textblobbuilder_allocrunpos, TestLevel::L1, 48)
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
    font.SetSize(100.f);
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(-100, -100, 1000, 1500);
    auto buffer = builder.AllocRunPos(font, 0, &rect1);
    for (int i = 0; i < 0; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 1000.0f-50.f*i;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3049
DEF_RSDEMO(textblobbuilder_allocrunpos, TestLevel::L1, 49)
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
    font.SetSize(100.f);
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(300, 300, 320, 500);
    auto buffer = builder.AllocRunPos(font, 0, &rect1);
    for (int i = 0; i < 0; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 50.f*i;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3050
DEF_RSDEMO(textblobbuilder_allocrunpos, TestLevel::L1, 50)
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
    font.SetSize(100.f);
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(300, 300, 320, 500);
    auto buffer = builder.AllocRunPos(font, 0, &rect1);
    for (int i = 0; i < 0; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 0;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3051
DEF_RSDEMO(textblobbuilder_allocrunpos, TestLevel::L1, 51)
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
    font.SetSize(100.f);
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(300, 300, 320, 500);
    auto buffer = builder.AllocRunPos(font, 0, &rect1);
    for (int i = 0; i < 0; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 50.f*i;
        buffer.pos[i * 2 + 1] = 0;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3052
DEF_RSDEMO(textblobbuilder_allocrunpos, TestLevel::L1, 52)
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
    font.SetSize(100.f);
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(300, 300, 320, 500);
    auto buffer = builder.AllocRunPos(font, 0, &rect1);
    for (int i = 0; i < 0; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 1000.0f-50.f*i;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3053
DEF_RSDEMO(textblobbuilder_allocrunpos, TestLevel::L1, 53)
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
    font.SetSize(100.f);
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(300, 300, 100, 800);
    auto buffer = builder.AllocRunPos(font, 0, &rect1);
    for (int i = 0; i < 0; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 50.f*i;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3054
DEF_RSDEMO(textblobbuilder_allocrunpos, TestLevel::L1, 54)
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
    font.SetSize(100.f);
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(300, 300, 100, 800);
    auto buffer = builder.AllocRunPos(font, 0, &rect1);
    for (int i = 0; i < 0; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 0;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3055
DEF_RSDEMO(textblobbuilder_allocrunpos, TestLevel::L1, 55)
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
    font.SetSize(100.f);
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(300, 300, 100, 800);
    auto buffer = builder.AllocRunPos(font, 0, &rect1);
    for (int i = 0; i < 0; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 50.f*i;
        buffer.pos[i * 2 + 1] = 0;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3056
DEF_RSDEMO(textblobbuilder_allocrunpos, TestLevel::L1, 56)
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
    font.SetSize(100.f);
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(300, 300, 100, 800);
    auto buffer = builder.AllocRunPos(font, 0, &rect1);
    for (int i = 0; i < 0; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 1000.0f-50.f*i;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3057
DEF_RSDEMO(textblobbuilder_allocrunpos, TestLevel::L1, 57)
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
    font.SetSize(100.f);
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(nullptr);
    auto buffer = builder.AllocRunPos(font, 0, &rect1);
    for (int i = 0; i < 0; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 50.f*i;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3058
DEF_RSDEMO(textblobbuilder_allocrunpos, TestLevel::L1, 58)
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
    font.SetSize(100.f);
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(nullptr);
    auto buffer = builder.AllocRunPos(font, 0, &rect1);
    for (int i = 0; i < 0; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 0;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3059
DEF_RSDEMO(textblobbuilder_allocrunpos, TestLevel::L1, 59)
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
    font.SetSize(100.f);
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(nullptr);
    auto buffer = builder.AllocRunPos(font, 0, &rect1);
    for (int i = 0; i < 0; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 50.f*i;
        buffer.pos[i * 2 + 1] = 0;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3060
DEF_RSDEMO(textblobbuilder_allocrunpos, TestLevel::L1, 60)
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
    font.SetSize(100.f);
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(nullptr);
    auto buffer = builder.AllocRunPos(font, 0, &rect1);
    for (int i = 0; i < 0; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 1000.0f-50.f*i;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3061
DEF_RSDEMO(textblobbuilder_allocrunpos, TestLevel::L1, 61)
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
    font.SetSize(100.f);
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(100, 100, 1000, 1500);
    auto buffer = builder.AllocRunPos(font, -1, &rect1);
    for (int i = 0; i < -1; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 50.f*i;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3062
DEF_RSDEMO(textblobbuilder_allocrunpos, TestLevel::L1, 62)
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
    font.SetSize(100.f);
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(100, 100, 1000, 1500);
    auto buffer = builder.AllocRunPos(font, -1, &rect1);
    for (int i = 0; i < -1; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 0;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3063
DEF_RSDEMO(textblobbuilder_allocrunpos, TestLevel::L1, 63)
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
    font.SetSize(100.f);
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(100, 100, 1000, 1500);
    auto buffer = builder.AllocRunPos(font, -1, &rect1);
    for (int i = 0; i < -1; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 50.f*i;
        buffer.pos[i * 2 + 1] = 0;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3064
DEF_RSDEMO(textblobbuilder_allocrunpos, TestLevel::L1, 64)
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
    font.SetSize(100.f);
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(100, 100, 1000, 1500);
    auto buffer = builder.AllocRunPos(font, -1, &rect1);
    for (int i = 0; i < -1; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 1000.0f-50.f*i;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3065
DEF_RSDEMO(textblobbuilder_allocrunpos, TestLevel::L1, 65)
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
    font.SetSize(100.f);
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(-100, -100, 1000, 1500);
    auto buffer = builder.AllocRunPos(font, -1, &rect1);
    for (int i = 0; i < -1; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 50.f*i;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3066
DEF_RSDEMO(textblobbuilder_allocrunpos, TestLevel::L1, 66)
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
    font.SetSize(100.f);
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(-100, -100, 1000, 1500);
    auto buffer = builder.AllocRunPos(font, -1, &rect1);
    for (int i = 0; i < -1; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 0;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3067
DEF_RSDEMO(textblobbuilder_allocrunpos, TestLevel::L1, 67)
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
    font.SetSize(100.f);
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(-100, -100, 1000, 1500);
    auto buffer = builder.AllocRunPos(font, -1, &rect1);
    for (int i = 0; i < -1; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 50.f*i;
        buffer.pos[i * 2 + 1] = 0;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3068
DEF_RSDEMO(textblobbuilder_allocrunpos, TestLevel::L1, 68)
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
    font.SetSize(100.f);
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(-100, -100, 1000, 1500);
    auto buffer = builder.AllocRunPos(font, -1, &rect1);
    for (int i = 0; i < -1; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 1000.0f-50.f*i;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3069
DEF_RSDEMO(textblobbuilder_allocrunpos, TestLevel::L1, 69)
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
    font.SetSize(100.f);
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(300, 300, 320, 500);
    auto buffer = builder.AllocRunPos(font, -1, &rect1);
    for (int i = 0; i < -1; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 50.f*i;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3070
DEF_RSDEMO(textblobbuilder_allocrunpos, TestLevel::L1, 70)
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
    font.SetSize(100.f);
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(300, 300, 320, 500);
    auto buffer = builder.AllocRunPos(font, -1, &rect1);
    for (int i = 0; i < -1; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 0;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3071
DEF_RSDEMO(textblobbuilder_allocrunpos, TestLevel::L1, 71)
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
    font.SetSize(100.f);
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(300, 300, 320, 500);
    auto buffer = builder.AllocRunPos(font, -1, &rect1);
    for (int i = 0; i < -1; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 50.f*i;
        buffer.pos[i * 2 + 1] = 0;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3072
DEF_RSDEMO(textblobbuilder_allocrunpos, TestLevel::L1, 72)
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
    font.SetSize(100.f);
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(300, 300, 320, 500);
    auto buffer = builder.AllocRunPos(font, -1, &rect1);
    for (int i = 0; i < -1; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 1000.0f-50.f*i;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3073
DEF_RSDEMO(textblobbuilder_allocrunpos, TestLevel::L1, 73)
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
    font.SetSize(100.f);
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(300, 300, 100, 800);
    auto buffer = builder.AllocRunPos(font, -1, &rect1);
    for (int i = 0; i < -1; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 50.f*i;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3074
DEF_RSDEMO(textblobbuilder_allocrunpos, TestLevel::L1, 74)
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
    font.SetSize(100.f);
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(300, 300, 100, 800);
    auto buffer = builder.AllocRunPos(font, -1, &rect1);
    for (int i = 0; i < -1; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 0;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3075
DEF_RSDEMO(textblobbuilder_allocrunpos, TestLevel::L1, 75)
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
    font.SetSize(100.f);
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(300, 300, 100, 800);
    auto buffer = builder.AllocRunPos(font, -1, &rect1);
    for (int i = 0; i < -1; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 50.f*i;
        buffer.pos[i * 2 + 1] = 0;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3076
DEF_RSDEMO(textblobbuilder_allocrunpos, TestLevel::L1, 76)
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
    font.SetSize(100.f);
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(300, 300, 100, 800);
    auto buffer = builder.AllocRunPos(font, -1, &rect1);
    for (int i = 0; i < -1; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 1000.0f-50.f*i;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3077
DEF_RSDEMO(textblobbuilder_allocrunpos, TestLevel::L1, 77)
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
    font.SetSize(100.f);
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(nullptr);
    auto buffer = builder.AllocRunPos(font, -1, &rect1);
    for (int i = 0; i < -1; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 50.f*i;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3078
DEF_RSDEMO(textblobbuilder_allocrunpos, TestLevel::L1, 78)
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
    font.SetSize(100.f);
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(nullptr);
    auto buffer = builder.AllocRunPos(font, -1, &rect1);
    for (int i = 0; i < -1; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 0;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3079
DEF_RSDEMO(textblobbuilder_allocrunpos, TestLevel::L1, 79)
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
    font.SetSize(100.f);
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(nullptr);
    auto buffer = builder.AllocRunPos(font, -1, &rect1);
    for (int i = 0; i < -1; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 50.f*i;
        buffer.pos[i * 2 + 1] = 0;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3080
DEF_RSDEMO(textblobbuilder_allocrunpos, TestLevel::L1, 80)
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
    font.SetSize(100.f);
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(nullptr);
    auto buffer = builder.AllocRunPos(font, -1, &rect1);
    for (int i = 0; i < -1; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 1000.0f-50.f*i;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

}
}
