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
#include "dtk_test_ext.h"
#include <string>
#include "text/font.h"
#include "text/font_mgr.h"
#include "text/font_style_set.h"
#include "text/font_style.h"
#include "text/typeface.h"
#include <sstream>

namespace OHOS {
namespace Rosen {

static Drawing::Font MakeFont1(Drawing::Font& font,
    std::shared_ptr<Drawing::Typeface> typeface, float size, float scalex, float skewx)
{
    font.SetTypeface(typeface);
    font.SetSize(size);
    font.SetScaleX(scalex);
    font.SetSkewX(skewx);
    auto font1 = Drawing::Font(font.GetTypeface(), font.GetSize(), font.GetScaleX(), font.GetSkewX());
    font.SetSubpixel(false);
    font1.SetSubpixel(false);

    return font1;
}

static std::string g_text1 = "DDGR ddgr 鸿蒙 !@#%￥^&*;：，。";
static std::string g_text2 = "-_=+()123`.---~|{}【】,./?、？<>《》";
static std::string g_text3 = "\xE2\x99\x88\xE2\x99\x8A\xE2\x99\x88\xE2\x99\x8C\xE2\x99\x8D\xE2\x99\x8D";
static std::string g_text = g_text1 + g_text2 + g_text3;

static void DrawTexts(std::vector<std::string>& texts, Drawing::Font& font,
                      Drawing::Font& font1, TestPlaybackCanvas* playbackCanvas)
{
    int line = 200;
    int interval1 = 100;
    int interval2 = 200;
    int interval3 = 300;
    int interval4 = 400;

    for (auto text : texts) {
        std::shared_ptr<Drawing::TextBlob> textBlob = Drawing::TextBlob::MakeFromText(text.c_str(), text.size(), font);
        std::shared_ptr<Drawing::TextBlob> textinfo = Drawing::TextBlob::MakeFromText(text.c_str(), text.size(), font1);
        Drawing::Brush brush;
        playbackCanvas->AttachBrush(brush);
        playbackCanvas->DrawTextBlob(textBlob.get(), interval2, line);
        playbackCanvas->DrawTextBlob(textinfo.get(), interval2, line + interval1);
        playbackCanvas->DetachBrush();
        Drawing::Pen pen;
        playbackCanvas->AttachPen(pen);
        playbackCanvas->DrawTextBlob(textBlob.get(), interval2, line + interval2);
        playbackCanvas->DrawTextBlob(textBlob.get(), interval2, line + interval3);
        playbackCanvas->DetachPen();
        line += interval4;
    }
}

// 对应用例 font_font_3378
DEF_DTK(font_font_4, TestLevel::L2, 378)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->MatchStyle({
        Drawing::FontStyle::INVISIBLE_WEIGHT,
        Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT}));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 500.f, -1.f, -1.f);
    font.SetBaselineSnap(false);
    font1.SetBaselineSnap(font.IsBaselineSnap());
    font.SetEdging(Drawing::FontEdging::SUBPIXEL_ANTI_ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::FULL);
    font1.SetHinting(font.GetHinting());
    font.SetForceAutoHinting(false);
    font1.SetForceAutoHinting(font.IsForceAutoHinting());
    font.SetEmbeddedBitmaps(false);
    font1.SetEmbeddedBitmaps(font.IsEmbeddedBitmaps());
    font.SetEmbolden(true);
    font1.SetEmbolden(font.IsEmbolden());
    font.SetLinearMetrics(false);
    font1.SetLinearMetrics(font.IsLinearMetrics());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    auto glyphsCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID);
    std::string text4 = "glyphsCount = " + std::to_string(glyphsCount);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3379
DEF_DTK(font_font_4, TestLevel::L2, 379)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -50.f, -1.f, 0);
    font.SetBaselineSnap(true);
    font1.SetBaselineSnap(font.IsBaselineSnap());
    font.SetEdging(Drawing::FontEdging::ANTI_ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetForceAutoHinting(true);
    font1.SetForceAutoHinting(font.IsForceAutoHinting());
    font.SetEmbeddedBitmaps(true);
    font1.SetEmbeddedBitmaps(font.IsEmbeddedBitmaps());
    font.SetEmbolden(true);
    font1.SetEmbolden(font.IsEmbolden());
    font.SetLinearMetrics(true);
    font1.SetLinearMetrics(font.IsLinearMetrics());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    int glyphCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF8);
    uint16_t glyphs[glyphCount + 1];
    int count = font.TextToGlyphs(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID,
        glyphs, glyphCount + 1);
    std::string text4 = "GlyphsCount = " + std::to_string(count);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3380
DEF_DTK(font_font_4, TestLevel::L2, 380)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -50.f, -1.f, 0);
    font.SetBaselineSnap(false);
    font1.SetBaselineSnap(font.IsBaselineSnap());
    font.SetEdging(Drawing::FontEdging::ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::SLIGHT);
    font1.SetHinting(font.GetHinting());
    font.SetForceAutoHinting(false);
    font1.SetForceAutoHinting(font.IsForceAutoHinting());
    font.SetEmbeddedBitmaps(false);
    font1.SetEmbeddedBitmaps(font.IsEmbeddedBitmaps());
    font.SetEmbolden(false);
    font1.SetEmbolden(font.IsEmbolden());
    font.SetLinearMetrics(false);
    font1.SetLinearMetrics(font.IsLinearMetrics());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    Drawing::Rect bounds;
    auto scalar = font.MeasureText(g_text.c_str(), g_text.size(), Drawing::TextEncoding::UTF32, &bounds);
    std::string text4 = "MeasureTextWidths = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3381
DEF_DTK(font_font_4, TestLevel::L2, 381)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -50.f, -0.5f, -0.5f);
    font.SetHinting(Drawing::FontHinting::NORMAL);
    font1.SetHinting(font.GetHinting());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    int glyphCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF8);
    uint16_t glyphs[glyphCount - 1];
    int count = font.TextToGlyphs(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID,
        glyphs, glyphCount - 1);
    std::string text4 = "GlyphsCount = " + std::to_string(count);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3382
DEF_DTK(font_font_4, TestLevel::L2, 382)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -31.f, -0.5f, 0.7f);
    font.SetBaselineSnap(true);
    font1.SetBaselineSnap(font.IsBaselineSnap());
    font.SetHinting(Drawing::FontHinting::FULL);
    font1.SetHinting(font.GetHinting());
    font.SetForceAutoHinting(true);
    font1.SetForceAutoHinting(font.IsForceAutoHinting());
    font.SetEmbeddedBitmaps(true);
    font1.SetEmbeddedBitmaps(font.IsEmbeddedBitmaps());
    font.SetEmbolden(true);
    font1.SetEmbolden(font.IsEmbolden());
    font.SetLinearMetrics(true);
    font1.SetLinearMetrics(font.IsLinearMetrics());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    std::vector<std::string> texts = {g_text1, g_text2, g_text3};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3383
DEF_DTK(font_font_4, TestLevel::L2, 383)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -31.f, -0.5f, 0.7f);
    font.SetBaselineSnap(false);
    font1.SetBaselineSnap(font.IsBaselineSnap());
    font.SetEdging(Drawing::FontEdging::SUBPIXEL_ANTI_ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::NONE);
    font1.SetHinting(font.GetHinting());
    font.SetForceAutoHinting(false);
    font1.SetForceAutoHinting(font.IsForceAutoHinting());
    font.SetEmbeddedBitmaps(false);
    font1.SetEmbeddedBitmaps(font.IsEmbeddedBitmaps());
    font.SetEmbolden(false);
    font1.SetEmbolden(font.IsEmbolden());
    font.SetLinearMetrics(false);
    font1.SetLinearMetrics(font.IsLinearMetrics());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    int glyphCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF16);
    uint16_t glyphs[glyphCount - 1];
    int count = font.TextToGlyphs(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID,
        glyphs, glyphCount + 1);
    std::string text4 = "GlyphsCount = " + std::to_string(count);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3384
DEF_DTK(font_font_4, TestLevel::L2, 384)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -31.f, -0.5f, 0.7f);
    font.SetEdging(Drawing::FontEdging::ANTI_ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::NORMAL);
    font1.SetHinting(font.GetHinting());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    auto scalar = font.MeasureSingleCharacter(0xEEEEE);
    std::string text4 = "Recommended spaceing between lines = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3385
DEF_DTK(font_font_4, TestLevel::L2, 385)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -31.f, 0, 0);
    font.SetBaselineSnap(true);
    font1.SetBaselineSnap(font.IsBaselineSnap());
    font.SetEdging(Drawing::FontEdging::ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::FULL);
    font1.SetHinting(font.GetHinting());
    font.SetForceAutoHinting(true);
    font1.SetForceAutoHinting(font.IsForceAutoHinting());
    font.SetEmbeddedBitmaps(true);
    font1.SetEmbeddedBitmaps(font.IsEmbeddedBitmaps());
    font.SetEmbolden(true);
    font1.SetEmbolden(font.IsEmbolden());
    font.SetLinearMetrics(true);
    font1.SetLinearMetrics(font.IsLinearMetrics());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    int glyphCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF8);
    uint16_t glyphs[glyphCount - 1];
    int count = font.TextToGlyphs(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID, glyphs, glyphCount);
    std::string text4 = "GlyphsCount = " + std::to_string(count);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3386
DEF_DTK(font_font_4, TestLevel::L2, 386)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -15.34f, 0, 1.f);
    font.SetBaselineSnap(true);
    font1.SetBaselineSnap(font.IsBaselineSnap());
    font.SetEdging(Drawing::FontEdging::SUBPIXEL_ANTI_ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::FULL);
    font1.SetHinting(font.GetHinting());
    font.SetForceAutoHinting(true);
    font1.SetForceAutoHinting(font.IsForceAutoHinting());
    font.SetEmbeddedBitmaps(true);
    font1.SetEmbeddedBitmaps(font.IsEmbeddedBitmaps());
    font.SetEmbolden(true);
    font1.SetEmbolden(font.IsEmbolden());
    font.SetLinearMetrics(true);
    font1.SetLinearMetrics(font.IsLinearMetrics());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    auto glyphsCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF8);
    std::string text4 = "glyphsCount = " + std::to_string(glyphsCount);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3387
DEF_DTK(font_font_4, TestLevel::L2, 387)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -15.34f, 0, 1.f);
    font.SetBaselineSnap(false);
    font1.SetBaselineSnap(font.IsBaselineSnap());
    font.SetEdging(Drawing::FontEdging::ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetForceAutoHinting(false);
    font1.SetForceAutoHinting(font.IsForceAutoHinting());
    font.SetEmbeddedBitmaps(false);
    font1.SetEmbeddedBitmaps(font.IsEmbeddedBitmaps());
    font.SetEmbolden(false);
    font1.SetEmbolden(font.IsEmbolden());
    font.SetLinearMetrics(false);
    font1.SetLinearMetrics(font.IsLinearMetrics());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    Drawing::FontMetrics metrics;
    auto SpaceLine = font.GetMetrics(&metrics);
    std::string text4 = "Recommended spaceing between lines = " + std::to_string(SpaceLine);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3388
DEF_DTK(font_font_4, TestLevel::L2, 388)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -15.34f, 0, 1.f);
    font.SetHinting(Drawing::FontHinting::SLIGHT);
    font1.SetHinting(font.GetHinting());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    int glyphCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF32);
    uint16_t glyphs[glyphCount - 1];
    int count = font.TextToGlyphs(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID,
        glyphs, glyphCount + 1);
    std::string text4 = "GlyphsCount = " + std::to_string(count);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3389
DEF_DTK(font_font_4, TestLevel::L2, 389)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -15.34f, 0.7f, 0.7f);
    font.SetBaselineSnap(false);
    font1.SetBaselineSnap(font.IsBaselineSnap());
    font.SetEdging(Drawing::FontEdging::ANTI_ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetForceAutoHinting(false);
    font1.SetForceAutoHinting(font.IsForceAutoHinting());
    font.SetEmbeddedBitmaps(false);
    font1.SetEmbeddedBitmaps(font.IsEmbeddedBitmaps());
    font.SetEmbolden(false);
    font1.SetEmbolden(font.IsEmbolden());
    font.SetLinearMetrics(false);
    font1.SetLinearMetrics(font.IsLinearMetrics());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    int glyphCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF8);
    uint16_t glyphs[glyphCount - 1];
    int count = font.TextToGlyphs(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID,
        glyphs, glyphCount - 1);
    std::string text4 = "GlyphsCount = " + std::to_string(count);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3390
DEF_DTK(font_font_4, TestLevel::L2, 390)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 0, 0.7f, 30.f);
    font.SetBaselineSnap(true);
    font1.SetBaselineSnap(font.IsBaselineSnap());
    font.SetEdging(Drawing::FontEdging::ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::NORMAL);
    font1.SetHinting(font.GetHinting());
    font.SetForceAutoHinting(true);
    font1.SetForceAutoHinting(font.IsForceAutoHinting());
    font.SetEmbeddedBitmaps(true);
    font1.SetEmbeddedBitmaps(font.IsEmbeddedBitmaps());
    font.SetEmbolden(true);
    font1.SetEmbolden(font.IsEmbolden());
    font.SetLinearMetrics(true);
    font1.SetLinearMetrics(font.IsLinearMetrics());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    int glyphCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID);
    uint16_t glyphs[glyphCount - 1];
    int count = font.TextToGlyphs(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID,
        glyphs, glyphCount + 1);
    std::string text4 = "GlyphsCount = " + std::to_string(count);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3391
DEF_DTK(font_font_4, TestLevel::L2, 391)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 0, 0.7f, 30.f);
    font.SetBaselineSnap(false);
    font1.SetBaselineSnap(font.IsBaselineSnap());
    font.SetForceAutoHinting(false);
    font1.SetForceAutoHinting(font.IsForceAutoHinting());
    font.SetEmbeddedBitmaps(false);
    font1.SetEmbeddedBitmaps(font.IsEmbeddedBitmaps());
    font.SetEmbolden(false);
    font1.SetEmbolden(font.IsEmbolden());
    font.SetLinearMetrics(false);
    font1.SetLinearMetrics(font.IsLinearMetrics());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    auto glyphsCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF16);
    std::string text4 = "glyphsCount = " + std::to_string(glyphsCount);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3392
DEF_DTK(font_font_4, TestLevel::L2, 392)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 0, 0.7f, 30.f);
    font.SetEdging(Drawing::FontEdging::ANTI_ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::NONE);
    font1.SetHinting(font.GetHinting());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    auto SpaceLine = font.GetMetrics(nullptr);
    std::string text4 = "Recommended spaceing between lines = " + std::to_string(SpaceLine);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3393
DEF_DTK(font_font_4, TestLevel::L2, 393)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 0, 1.f, 1.f);
    font.SetEdging(Drawing::FontEdging::SUBPIXEL_ANTI_ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::NONE);
    font1.SetHinting(font.GetHinting());
    font.SetEmbolden(false);
    font1.SetEmbolden(font.IsEmbolden());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    Drawing::Rect bounds;
    auto scalar = font.MeasureText(g_text.c_str(), g_text.size(), Drawing::TextEncoding::GLYPH_ID, &bounds);
    std::string text4 = "MeasureTextWidths = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3394
DEF_DTK(font_font_4, TestLevel::L2, 394)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 27.13f, 1.f, -30.f);
    font.SetBaselineSnap(true);
    font1.SetBaselineSnap(font.IsBaselineSnap());
    font.SetEdging(Drawing::FontEdging::SUBPIXEL_ANTI_ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::SLIGHT);
    font1.SetHinting(font.GetHinting());
    font.SetForceAutoHinting(true);
    font1.SetForceAutoHinting(font.IsForceAutoHinting());
    font.SetEmbeddedBitmaps(true);
    font1.SetEmbeddedBitmaps(font.IsEmbeddedBitmaps());
    font.SetEmbolden(true);
    font1.SetEmbolden(font.IsEmbolden());
    font.SetLinearMetrics(true);
    font1.SetLinearMetrics(font.IsLinearMetrics());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    int glyphCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF16);
    float widths[glyphCount];
    uint16_t glyphs[glyphCount];
    font.TextToGlyphs(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF8, glyphs, glyphCount);
    font.GetWidths(glyphs, glyphCount, widths);
    std::string text4 = "GetWidths = " + std::to_string(widths[0]);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3395
DEF_DTK(font_font_4, TestLevel::L2, 395)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 27.13f, 1.f, -30.f);
    font.SetBaselineSnap(false);
    font1.SetBaselineSnap(font.IsBaselineSnap());
    font.SetEdging(Drawing::FontEdging::ANTI_ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::FULL);
    font1.SetHinting(font.GetHinting());
    font.SetForceAutoHinting(false);
    font1.SetForceAutoHinting(font.IsForceAutoHinting());
    font.SetEmbeddedBitmaps(false);
    font1.SetEmbeddedBitmaps(font.IsEmbeddedBitmaps());
    font.SetEmbolden(false);
    font1.SetEmbolden(font.IsEmbolden());
    font.SetLinearMetrics(false);
    font1.SetLinearMetrics(font.IsLinearMetrics());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    Drawing::Rect bounds;
    auto scalar = font.MeasureText(g_text.c_str(), g_text.size(), Drawing::TextEncoding::UTF8, &bounds);
    std::string text4 = "MeasureTextWidths = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3396
DEF_DTK(font_font_4, TestLevel::L2, 396)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 27.13f, 1.f, -30.f);
    font.SetEdging(Drawing::FontEdging::ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::NONE);
    font1.SetHinting(font.GetHinting());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    auto glyphsCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF32);
    std::string text4 = "glyphsCount = " + std::to_string(glyphsCount);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3397
DEF_DTK(font_font_4, TestLevel::L2, 397)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 27.13f, 30.f, 30.f);
    font.SetBaselineSnap(true);
    font1.SetBaselineSnap(font.IsBaselineSnap());
    font.SetHinting(Drawing::FontHinting::SLIGHT);
    font1.SetHinting(font.GetHinting());
    font.SetForceAutoHinting(true);
    font1.SetForceAutoHinting(font.IsForceAutoHinting());
    font.SetEmbeddedBitmaps(true);
    font1.SetEmbeddedBitmaps(font.IsEmbeddedBitmaps());
    font.SetLinearMetrics(true);
    font1.SetLinearMetrics(font.IsLinearMetrics());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    int glyphCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF8);
    uint16_t glyphs[glyphCount];
    int count = font.TextToGlyphs(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID,
        glyphs, glyphCount + 1);
    std::string text4 = "GlyphsCount = " + std::to_string(count);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3398
DEF_DTK(font_font_4, TestLevel::L2, 398)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 71.f, -30.f, -30.f);
    font.SetBaselineSnap(false);
    font1.SetBaselineSnap(font.IsBaselineSnap());
    font.SetEdging(Drawing::FontEdging::ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetForceAutoHinting(false);
    font1.SetForceAutoHinting(font.IsForceAutoHinting());
    font.SetEmbeddedBitmaps(false);
    font1.SetEmbeddedBitmaps(font.IsEmbeddedBitmaps());
    font.SetEmbolden(true);
    font1.SetEmbolden(font.IsEmbolden());
    font.SetLinearMetrics(false);
    font1.SetLinearMetrics(font.IsLinearMetrics());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    int glyphCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF8);
    uint16_t glyphs[glyphCount - 1];
    int count = font.TextToGlyphs(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID,
        glyphs, glyphCount + 1);
    std::string text4 = "GlyphsCount = " + std::to_string(count);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3399
DEF_DTK(font_font_4, TestLevel::L2, 399)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 71.f, 30.f, -1.f);
    font.SetBaselineSnap(true);
    font1.SetBaselineSnap(font.IsBaselineSnap());
    font.SetEdging(Drawing::FontEdging::ANTI_ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::SLIGHT);
    font1.SetHinting(font.GetHinting());
    font.SetForceAutoHinting(true);
    font1.SetForceAutoHinting(font.IsForceAutoHinting());
    font.SetEmbeddedBitmaps(true);
    font1.SetEmbeddedBitmaps(font.IsEmbeddedBitmaps());
    font.SetEmbolden(true);
    font1.SetEmbolden(font.IsEmbolden());
    font.SetLinearMetrics(true);
    font1.SetLinearMetrics(font.IsLinearMetrics());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    auto glyphsCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID);
    std::string text4 = "glyphsCount = " + std::to_string(glyphsCount);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3400
DEF_DTK(font_font_4, TestLevel::L2, 400)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 71.f, 30.f, -1.f);
    font.SetBaselineSnap(false);
    font1.SetBaselineSnap(font.IsBaselineSnap());
    font.SetHinting(Drawing::FontHinting::NORMAL);
    font1.SetHinting(font.GetHinting());
    font.SetForceAutoHinting(false);
    font1.SetForceAutoHinting(font.IsForceAutoHinting());
    font.SetEmbeddedBitmaps(false);
    font1.SetEmbeddedBitmaps(font.IsEmbeddedBitmaps());
    font.SetEmbolden(false);
    font1.SetEmbolden(font.IsEmbolden());
    font.SetLinearMetrics(false);
    font1.SetLinearMetrics(font.IsLinearMetrics());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    auto scalar = font.UnicharToGlyph(0x44);
    std::string text4 = "Glyphs = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3401
DEF_DTK(font_font_4, TestLevel::L2, 401)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 71.f, 30.f, -1.f);
    font.SetEdging(Drawing::FontEdging::SUBPIXEL_ANTI_ALIAS);
    font1.SetEdging(font.GetEdging());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    Drawing::Rect bounds;
    auto scalar = font.MeasureText(g_text.c_str(), g_text.size(), Drawing::TextEncoding::UTF8, &bounds);
    std::string text4 = "MeasureTextWidths = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3402
DEF_DTK(font_font_4, TestLevel::L2, 402)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 500.f, -30.f, -0.5f);
    font.SetBaselineSnap(true);
    font1.SetBaselineSnap(font.IsBaselineSnap());
    font.SetHinting(Drawing::FontHinting::NONE);
    font1.SetHinting(font.GetHinting());
    font.SetForceAutoHinting(true);
    font1.SetForceAutoHinting(font.IsForceAutoHinting());
    font.SetEmbeddedBitmaps(true);
    font1.SetEmbeddedBitmaps(font.IsEmbeddedBitmaps());
    font.SetEmbolden(true);
    font1.SetEmbolden(font.IsEmbolden());
    font.SetLinearMetrics(true);
    font1.SetLinearMetrics(font.IsLinearMetrics());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    Drawing::Rect bounds;
    auto scalar = font.MeasureText(g_text.c_str(), g_text.size(), Drawing::TextEncoding::UTF16, &bounds);
    std::string text4 = "MeasureTextWidths = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3403
DEF_DTK(font_font_4, TestLevel::L2, 403)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 500.f, -30.f, -0.5f);
    font.SetEdging(Drawing::FontEdging::ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::FULL);
    font1.SetHinting(font.GetHinting());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    auto scalar = font.UnicharToGlyph(0xEEEEE);
    std::string text4 = "Glyphs = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3404
DEF_DTK(font_font_4, TestLevel::L2, 404)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 500.f, -1.f, -1.f);
    font.SetEdging(Drawing::FontEdging::ANTI_ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::NONE);
    font1.SetHinting(font.GetHinting());
    font.SetEmbolden(false);
    font1.SetEmbolden(font.IsEmbolden());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    auto scalar = font.MeasureSingleCharacter(0x44);
    std::string text4 = "Recommended spaceing between lines = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3405
DEF_DTK(font_font_4, TestLevel::L2, 405)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->MatchStyle({
        Drawing::FontStyle::INVISIBLE_WEIGHT,
        Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT}));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -50.f, -1.f, 0);
    font.SetBaselineSnap(true);
    font1.SetBaselineSnap(font.IsBaselineSnap());
    font.SetEdging(Drawing::FontEdging::SUBPIXEL_ANTI_ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::NONE);
    font1.SetHinting(font.GetHinting());
    font.SetForceAutoHinting(true);
    font1.SetForceAutoHinting(font.IsForceAutoHinting());
    font.SetEmbeddedBitmaps(true);
    font1.SetEmbeddedBitmaps(font.IsEmbeddedBitmaps());
    font.SetEmbolden(true);
    font1.SetEmbolden(font.IsEmbolden());
    font.SetLinearMetrics(true);
    font1.SetLinearMetrics(font.IsLinearMetrics());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    int glyphCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF16);
    uint16_t glyphs[glyphCount - 1];
    int count = font.TextToGlyphs(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID,
        glyphs, glyphCount + 1);
    std::string text4 = "GlyphsCount = " + std::to_string(count);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3406
DEF_DTK(font_font_4, TestLevel::L2, 406)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->MatchStyle({
        Drawing::FontStyle::INVISIBLE_WEIGHT,
        Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT}));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -50.f, -1.f, 0);
    font.SetBaselineSnap(false);
    font1.SetBaselineSnap(font.IsBaselineSnap());
    font.SetEdging(Drawing::FontEdging::ANTI_ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::NORMAL);
    font1.SetHinting(font.GetHinting());
    font.SetForceAutoHinting(false);
    font1.SetForceAutoHinting(font.IsForceAutoHinting());
    font.SetEmbeddedBitmaps(false);
    font1.SetEmbeddedBitmaps(font.IsEmbeddedBitmaps());
    font.SetEmbolden(false);
    font1.SetEmbolden(font.IsEmbolden());
    font.SetLinearMetrics(false);
    font1.SetLinearMetrics(font.IsLinearMetrics());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    auto glyphsCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF8);
    std::string text4 = "glyphsCount = " + std::to_string(glyphsCount);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3407
DEF_DTK(font_font_4, TestLevel::L2, 407)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->MatchStyle({
        Drawing::FontStyle::INVISIBLE_WEIGHT,
        Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT}));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -50.f, -1.f, 0);
    font.SetHinting(Drawing::FontHinting::FULL);
    font1.SetHinting(font.GetHinting());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    Drawing::FontMetrics metrics;
    auto SpaceLine = font.GetMetrics(&metrics);
    std::string text4 = "Recommended spaceing between lines = " + std::to_string(SpaceLine);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3408
DEF_DTK(font_font_4, TestLevel::L2, 408)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->MatchStyle({
        Drawing::FontStyle::INVISIBLE_WEIGHT,
        Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT}));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -50.f, -0.5f, -0.5f);
    font.SetBaselineSnap(true);
    font1.SetBaselineSnap(font.IsBaselineSnap());
    font.SetEdging(Drawing::FontEdging::SUBPIXEL_ANTI_ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetForceAutoHinting(true);
    font1.SetForceAutoHinting(font.IsForceAutoHinting());
    font.SetEmbeddedBitmaps(true);
    font1.SetEmbeddedBitmaps(font.IsEmbeddedBitmaps());
    font.SetEmbolden(true);
    font1.SetEmbolden(font.IsEmbolden());
    font.SetLinearMetrics(true);
    font1.SetLinearMetrics(font.IsLinearMetrics());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    Drawing::Rect bounds;
    auto scalar = font.MeasureText(g_text.c_str(), g_text.size(), Drawing::TextEncoding::UTF32, &bounds);
    std::string text4 = "MeasureTextWidths = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3409
DEF_DTK(font_font_4, TestLevel::L2, 409)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->MatchStyle({
        Drawing::FontStyle::INVISIBLE_WEIGHT,
        Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT}));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -31.f, -0.5f, 0.7f);
    font.SetBaselineSnap(true);
    font1.SetBaselineSnap(font.IsBaselineSnap());
    font.SetEdging(Drawing::FontEdging::ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetForceAutoHinting(true);
    font1.SetForceAutoHinting(font.IsForceAutoHinting());
    font.SetEmbeddedBitmaps(true);
    font1.SetEmbeddedBitmaps(font.IsEmbeddedBitmaps());
    font.SetEmbolden(true);
    font1.SetEmbolden(font.IsEmbolden());
    font.SetLinearMetrics(true);
    font1.SetLinearMetrics(font.IsLinearMetrics());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    auto SpaceLine = font.GetMetrics(nullptr);
    std::string text4 = "Recommended spaceing between lines = " + std::to_string(SpaceLine);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3410
DEF_DTK(font_font_4, TestLevel::L2, 410)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->MatchStyle({
        Drawing::FontStyle::INVISIBLE_WEIGHT,
        Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT}));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -31.f, -0.5f, 0.7f);
    font.SetBaselineSnap(false);
    font1.SetBaselineSnap(font.IsBaselineSnap());
    font.SetHinting(Drawing::FontHinting::SLIGHT);
    font1.SetHinting(font.GetHinting());
    font.SetForceAutoHinting(false);
    font1.SetForceAutoHinting(font.IsForceAutoHinting());
    font.SetEmbeddedBitmaps(false);
    font1.SetEmbeddedBitmaps(font.IsEmbeddedBitmaps());
    font.SetEmbolden(false);
    font1.SetEmbolden(font.IsEmbolden());
    font.SetLinearMetrics(false);
    font1.SetLinearMetrics(font.IsLinearMetrics());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    int glyphCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID);
    uint16_t glyphs[glyphCount - 1];
    int count = font.TextToGlyphs(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID,
        glyphs, glyphCount + 1);
    std::string text4 = "GlyphsCount = " + std::to_string(count);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3411
DEF_DTK(font_font_4, TestLevel::L2, 411)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->MatchStyle({
        Drawing::FontStyle::INVISIBLE_WEIGHT,
        Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT}));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -31.f, -0.5f, 0.7f);
    font.SetEdging(Drawing::FontEdging::SUBPIXEL_ANTI_ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::FULL);
    font1.SetHinting(font.GetHinting());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    auto glyphsCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF32);
    std::string text4 = "glyphsCount = " + std::to_string(glyphsCount);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3412
DEF_DTK(font_font_4, TestLevel::L2, 412)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->MatchStyle({
        Drawing::FontStyle::INVISIBLE_WEIGHT,
        Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT}));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -31.f, 0, 0);
    font.SetBaselineSnap(false);
    font1.SetBaselineSnap(font.IsBaselineSnap());
    font.SetHinting(Drawing::FontHinting::NONE);
    font1.SetHinting(font.GetHinting());
    font.SetForceAutoHinting(false);
    font1.SetForceAutoHinting(font.IsForceAutoHinting());
    font.SetEmbeddedBitmaps(false);
    font1.SetEmbeddedBitmaps(font.IsEmbeddedBitmaps());
    font.SetEmbolden(false);
    font1.SetEmbolden(font.IsEmbolden());
    font.SetLinearMetrics(false);
    font1.SetLinearMetrics(font.IsLinearMetrics());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    int glyphCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF32);
    uint16_t glyphs[glyphCount - 1];
    int count = font.TextToGlyphs(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID,
        glyphs, glyphCount + 1);
    std::string text4 = "GlyphsCount = " + std::to_string(count);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3413
DEF_DTK(font_font_4, TestLevel::L2, 413)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->MatchStyle({
        Drawing::FontStyle::INVISIBLE_WEIGHT,
        Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT}));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -15.34f, 0, 1.f);
    font.SetBaselineSnap(true);
    font1.SetBaselineSnap(font.IsBaselineSnap());
    font.SetForceAutoHinting(true);
    font1.SetForceAutoHinting(font.IsForceAutoHinting());
    font.SetEmbeddedBitmaps(true);
    font1.SetEmbeddedBitmaps(font.IsEmbeddedBitmaps());
    font.SetEmbolden(true);
    font1.SetEmbolden(font.IsEmbolden());
    font.SetLinearMetrics(true);
    font1.SetLinearMetrics(font.IsLinearMetrics());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    auto glyphsCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID);
    std::string text4 = "glyphsCount = " + std::to_string(glyphsCount);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3414
DEF_DTK(font_font_4, TestLevel::L2, 414)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->MatchStyle({
        Drawing::FontStyle::INVISIBLE_WEIGHT,
        Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT}));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -15.34f, 0, 1.f);
    font.SetBaselineSnap(false);
    font1.SetBaselineSnap(font.IsBaselineSnap());
    font.SetEdging(Drawing::FontEdging::ANTI_ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::NONE);
    font1.SetHinting(font.GetHinting());
    font.SetForceAutoHinting(false);
    font1.SetForceAutoHinting(font.IsForceAutoHinting());
    font.SetEmbeddedBitmaps(false);
    font1.SetEmbeddedBitmaps(font.IsEmbeddedBitmaps());
    font.SetEmbolden(false);
    font1.SetEmbolden(font.IsEmbolden());
    font.SetLinearMetrics(false);
    font1.SetLinearMetrics(font.IsLinearMetrics());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    int glyphCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF16);
    float widths[glyphCount];
    uint16_t glyphs[glyphCount];
    font.TextToGlyphs(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF8, glyphs, glyphCount);
    font.GetWidths(glyphs, glyphCount, widths);
    std::string text4 = "GetWidths = " + std::to_string(widths[0]);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3415
DEF_DTK(font_font_4, TestLevel::L2, 415)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->MatchStyle({
        Drawing::FontStyle::INVISIBLE_WEIGHT,
        Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT}));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -15.34f, 0, 1.f);
    font.SetEdging(Drawing::FontEdging::ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::NORMAL);
    font1.SetHinting(font.GetHinting());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    Drawing::Rect bounds;
    auto scalar = font.MeasureText(g_text.c_str(), g_text.size(), Drawing::TextEncoding::UTF8, &bounds);
    std::string text4 = "MeasureTextWidths = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3416
DEF_DTK(font_font_4, TestLevel::L2, 416)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->MatchStyle({
        Drawing::FontStyle::INVISIBLE_WEIGHT,
        Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT}));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -15.34f, 0.7f, 0.7f);
    font.SetEdging(Drawing::FontEdging::ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::SLIGHT);
    font1.SetHinting(font.GetHinting());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    auto scalar = font.MeasureSingleCharacter(0xEEEEE);
    std::string text4 = "Recommended spaceing between lines = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3417
DEF_DTK(font_font_4, TestLevel::L2, 417)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->MatchStyle({
        Drawing::FontStyle::INVISIBLE_WEIGHT,
        Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT}));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 0, 0.7f, 30.f);
    font.SetBaselineSnap(true);
    font1.SetBaselineSnap(font.IsBaselineSnap());
    font.SetEdging(Drawing::FontEdging::ANTI_ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::FULL);
    font1.SetHinting(font.GetHinting());
    font.SetForceAutoHinting(true);
    font1.SetForceAutoHinting(font.IsForceAutoHinting());
    font.SetEmbeddedBitmaps(true);
    font1.SetEmbeddedBitmaps(font.IsEmbeddedBitmaps());
    font.SetEmbolden(true);
    font1.SetEmbolden(font.IsEmbolden());
    font.SetLinearMetrics(true);
    font1.SetLinearMetrics(font.IsLinearMetrics());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    Drawing::Rect bounds;
    auto scalar = font.MeasureText(g_text.c_str(), g_text.size(), Drawing::TextEncoding::UTF8, &bounds);
    std::string text4 = "MeasureTextWidths = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3418
DEF_DTK(font_font_4, TestLevel::L2, 418)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->MatchStyle({
        Drawing::FontStyle::INVISIBLE_WEIGHT,
        Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT}));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 0, 0.7f, 30.f);
    font.SetEdging(Drawing::FontEdging::SUBPIXEL_ANTI_ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::SLIGHT);
    font1.SetHinting(font.GetHinting());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    int glyphCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF8);
    uint16_t glyphs[glyphCount - 1];
    int count = font.TextToGlyphs(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID,
        glyphs, glyphCount + 1);
    std::string text4 = "GlyphsCount = " + std::to_string(count);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3419
DEF_DTK(font_font_4, TestLevel::L2, 419)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->MatchStyle({
        Drawing::FontStyle::INVISIBLE_WEIGHT,
        Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT}));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 0, 1.f, 1.f);
    font.SetBaselineSnap(true);
    font1.SetBaselineSnap(font.IsBaselineSnap());
    font.SetEdging(Drawing::FontEdging::ANTI_ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::NORMAL);
    font1.SetHinting(font.GetHinting());
    font.SetForceAutoHinting(true);
    font1.SetForceAutoHinting(font.IsForceAutoHinting());
    font.SetEmbeddedBitmaps(true);
    font1.SetEmbeddedBitmaps(font.IsEmbeddedBitmaps());
    font.SetLinearMetrics(true);
    font1.SetLinearMetrics(font.IsLinearMetrics());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    int glyphCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF8);
    uint16_t glyphs[glyphCount - 1];
    int count = font.TextToGlyphs(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID, glyphs, glyphCount);
    std::string text4 = "GlyphsCount = " + std::to_string(count);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3420
DEF_DTK(font_font_4, TestLevel::L2, 420)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->MatchStyle({
        Drawing::FontStyle::INVISIBLE_WEIGHT,
        Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT}));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 27.13f, 1.f, -30.f);
    font.SetBaselineSnap(true);
    font1.SetBaselineSnap(font.IsBaselineSnap());
    font.SetHinting(Drawing::FontHinting::NORMAL);
    font1.SetHinting(font.GetHinting());
    font.SetForceAutoHinting(true);
    font1.SetForceAutoHinting(font.IsForceAutoHinting());
    font.SetEmbeddedBitmaps(true);
    font1.SetEmbeddedBitmaps(font.IsEmbeddedBitmaps());
    font.SetEmbolden(true);
    font1.SetEmbolden(font.IsEmbolden());
    font.SetLinearMetrics(true);
    font1.SetLinearMetrics(font.IsLinearMetrics());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    int glyphCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF8);
    uint16_t glyphs[glyphCount];
    int count = font.TextToGlyphs(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID,
        glyphs, glyphCount + 1);
    std::string text4 = "GlyphsCount = " + std::to_string(count);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3421
DEF_DTK(font_font_4, TestLevel::L2, 421)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->MatchStyle({
        Drawing::FontStyle::INVISIBLE_WEIGHT,
        Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT}));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 27.13f, 1.f, -30.f);
    font.SetBaselineSnap(false);
    font1.SetBaselineSnap(font.IsBaselineSnap());
    font.SetEdging(Drawing::FontEdging::SUBPIXEL_ANTI_ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetForceAutoHinting(false);
    font1.SetForceAutoHinting(font.IsForceAutoHinting());
    font.SetEmbeddedBitmaps(false);
    font1.SetEmbeddedBitmaps(font.IsEmbeddedBitmaps());
    font.SetEmbolden(false);
    font1.SetEmbolden(font.IsEmbolden());
    font.SetLinearMetrics(false);
    font1.SetLinearMetrics(font.IsLinearMetrics());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    Drawing::Rect bounds;
    auto scalar = font.MeasureText(g_text.c_str(), g_text.size(), Drawing::TextEncoding::UTF16, &bounds);
    std::string text4 = "MeasureTextWidths = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3422
DEF_DTK(font_font_4, TestLevel::L2, 422)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->MatchStyle({
        Drawing::FontStyle::INVISIBLE_WEIGHT,
        Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT}));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 27.13f, 30.f, 30.f);
    font.SetBaselineSnap(false);
    font1.SetBaselineSnap(font.IsBaselineSnap());
    font.SetEdging(Drawing::FontEdging::SUBPIXEL_ANTI_ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::FULL);
    font1.SetHinting(font.GetHinting());
    font.SetForceAutoHinting(false);
    font1.SetForceAutoHinting(font.IsForceAutoHinting());
    font.SetEmbeddedBitmaps(false);
    font1.SetEmbeddedBitmaps(font.IsEmbeddedBitmaps());
    font.SetEmbolden(true);
    font1.SetEmbolden(font.IsEmbolden());
    font.SetLinearMetrics(false);
    font1.SetLinearMetrics(font.IsLinearMetrics());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    auto glyphsCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF16);
    std::string text4 = "glyphsCount = " + std::to_string(glyphsCount);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3423
DEF_DTK(font_font_4, TestLevel::L2, 423)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->MatchStyle({
        Drawing::FontStyle::INVISIBLE_WEIGHT,
        Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT}));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 71.f, -30.f, -30.f);
    font.SetHinting(Drawing::FontHinting::SLIGHT);
    font1.SetHinting(font.GetHinting());
    font.SetEmbolden(false);
    font1.SetEmbolden(font.IsEmbolden());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    auto scalar = font.UnicharToGlyph(0xEEEEE);
    std::string text4 = "Glyphs = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3424
DEF_DTK(font_font_4, TestLevel::L2, 424)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->MatchStyle({
        Drawing::FontStyle::INVISIBLE_WEIGHT,
        Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT}));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 71.f, 30.f, -1.f);
    font.SetBaselineSnap(false);
    font1.SetBaselineSnap(font.IsBaselineSnap());
    font.SetEdging(Drawing::FontEdging::ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::FULL);
    font1.SetHinting(font.GetHinting());
    font.SetForceAutoHinting(false);
    font1.SetForceAutoHinting(font.IsForceAutoHinting());
    font.SetEmbeddedBitmaps(false);
    font1.SetEmbeddedBitmaps(font.IsEmbeddedBitmaps());
    font.SetEmbolden(false);
    font1.SetEmbolden(font.IsEmbolden());
    font.SetLinearMetrics(false);
    font1.SetLinearMetrics(font.IsLinearMetrics());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    int glyphCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF8);
    uint16_t glyphs[glyphCount + 1];
    int count = font.TextToGlyphs(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID,
        glyphs, glyphCount + 1);
    std::string text4 = "GlyphsCount = " + std::to_string(count);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3425
DEF_DTK(font_font_4, TestLevel::L2, 425)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->MatchStyle({
        Drawing::FontStyle::INVISIBLE_WEIGHT,
        Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT}));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 71.f, 30.f, -1.f);
    font.SetHinting(Drawing::FontHinting::NONE);
    font1.SetHinting(font.GetHinting());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    Drawing::Rect bounds;
    auto scalar = font.MeasureText(g_text.c_str(), g_text.size(), Drawing::TextEncoding::GLYPH_ID, &bounds);
    std::string text4 = "MeasureTextWidths = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3426
DEF_DTK(font_font_4, TestLevel::L2, 426)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->MatchStyle({
        Drawing::FontStyle::INVISIBLE_WEIGHT,
        Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT}));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 500.f, -30.f, -0.5f);
    font.SetBaselineSnap(true);
    font1.SetBaselineSnap(font.IsBaselineSnap());
    font.SetEdging(Drawing::FontEdging::ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::SLIGHT);
    font1.SetHinting(font.GetHinting());
    font.SetForceAutoHinting(true);
    font1.SetForceAutoHinting(font.IsForceAutoHinting());
    font.SetEmbeddedBitmaps(true);
    font1.SetEmbeddedBitmaps(font.IsEmbeddedBitmaps());
    font.SetEmbolden(true);
    font1.SetEmbolden(font.IsEmbolden());
    font.SetLinearMetrics(true);
    font1.SetLinearMetrics(font.IsLinearMetrics());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    auto scalar = font.MeasureSingleCharacter(0x44);
    std::string text4 = "Recommended spaceing between lines = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3427
DEF_DTK(font_font_4, TestLevel::L2, 427)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->MatchStyle({
        Drawing::FontStyle::INVISIBLE_WEIGHT,
        Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT}));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 500.f, -30.f, -0.5f);
    font.SetBaselineSnap(false);
    font1.SetBaselineSnap(font.IsBaselineSnap());
    font.SetEdging(Drawing::FontEdging::SUBPIXEL_ANTI_ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::NORMAL);
    font1.SetHinting(font.GetHinting());
    font.SetForceAutoHinting(false);
    font1.SetForceAutoHinting(font.IsForceAutoHinting());
    font.SetEmbeddedBitmaps(false);
    font1.SetEmbeddedBitmaps(font.IsEmbeddedBitmaps());
    font.SetEmbolden(false);
    font1.SetEmbolden(font.IsEmbolden());
    font.SetLinearMetrics(false);
    font1.SetLinearMetrics(font.IsLinearMetrics());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    std::vector<std::string> texts = {g_text1, g_text2, g_text3};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3428
DEF_DTK(font_font_4, TestLevel::L2, 428)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->MatchStyle({
        Drawing::FontStyle::INVISIBLE_WEIGHT,
        Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT}));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 500.f, -30.f, -0.5f);
    font.SetEdging(Drawing::FontEdging::ANTI_ALIAS);
    font1.SetEdging(font.GetEdging());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    int glyphCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF8);
    uint16_t glyphs[glyphCount - 1];
    int count = font.TextToGlyphs(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID,
        glyphs, glyphCount - 1);
    std::string text4 = "GlyphsCount = " + std::to_string(count);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3429
DEF_DTK(font_font_4, TestLevel::L2, 429)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->MatchStyle({
        Drawing::FontStyle::INVISIBLE_WEIGHT,
        Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT}));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 500.f, -1.f, -1.f);
    font.SetBaselineSnap(true);
    font1.SetBaselineSnap(font.IsBaselineSnap());
    font.SetEdging(Drawing::FontEdging::ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::NORMAL);
    font1.SetHinting(font.GetHinting());
    font.SetForceAutoHinting(true);
    font1.SetForceAutoHinting(font.IsForceAutoHinting());
    font.SetEmbeddedBitmaps(true);
    font1.SetEmbeddedBitmaps(font.IsEmbeddedBitmaps());
    font.SetLinearMetrics(true);
    font1.SetLinearMetrics(font.IsLinearMetrics());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    auto scalar = font.UnicharToGlyph(0x44);
    std::string text4 = "Glyphs = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3430
DEF_DTK(font_font_4, TestLevel::L2, 430)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -50.f, -1.f, 0);
    font.SetBaselineSnap(true);
    font1.SetBaselineSnap(font.IsBaselineSnap());
    font.SetHinting(Drawing::FontHinting::SLIGHT);
    font1.SetHinting(font.GetHinting());
    font.SetForceAutoHinting(true);
    font1.SetForceAutoHinting(font.IsForceAutoHinting());
    font.SetEmbeddedBitmaps(true);
    font1.SetEmbeddedBitmaps(font.IsEmbeddedBitmaps());
    font.SetEmbolden(true);
    font1.SetEmbolden(font.IsEmbolden());
    font.SetLinearMetrics(true);
    font1.SetLinearMetrics(font.IsLinearMetrics());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    Drawing::Rect bounds;
    auto scalar = font.MeasureText(g_text.c_str(), g_text.size(), Drawing::TextEncoding::GLYPH_ID, &bounds);
    std::string text4 = "MeasureTextWidths = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3431
DEF_DTK(font_font_4, TestLevel::L2, 431)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -50.f, -1.f, 0);
    font.SetEdging(Drawing::FontEdging::ALIAS);
    font1.SetEdging(font.GetEdging());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    int glyphCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF8);
    uint16_t glyphs[glyphCount - 1];
    int count = font.TextToGlyphs(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID, glyphs, glyphCount);
    std::string text4 = "GlyphsCount = " + std::to_string(count);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3432
DEF_DTK(font_font_4, TestLevel::L2, 432)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -50.f, -0.5f, -0.5f);
    font.SetBaselineSnap(false);
    font1.SetBaselineSnap(font.IsBaselineSnap());
    font.SetEdging(Drawing::FontEdging::ANTI_ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::SLIGHT);
    font1.SetHinting(font.GetHinting());
    font.SetForceAutoHinting(false);
    font1.SetForceAutoHinting(font.IsForceAutoHinting());
    font.SetEmbeddedBitmaps(false);
    font1.SetEmbeddedBitmaps(font.IsEmbeddedBitmaps());
    font.SetEmbolden(false);
    font1.SetEmbolden(font.IsEmbolden());
    font.SetLinearMetrics(false);
    font1.SetLinearMetrics(font.IsLinearMetrics());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    std::vector<std::string> texts = {g_text1, g_text2, g_text3};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3433
DEF_DTK(font_font_4, TestLevel::L2, 433)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -31.f, -0.5f, 0.7f);
    font.SetBaselineSnap(true);
    font1.SetBaselineSnap(font.IsBaselineSnap());
    font.SetEdging(Drawing::FontEdging::ANTI_ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::NONE);
    font1.SetHinting(font.GetHinting());
    font.SetForceAutoHinting(true);
    font1.SetForceAutoHinting(font.IsForceAutoHinting());
    font.SetEmbeddedBitmaps(true);
    font1.SetEmbeddedBitmaps(font.IsEmbeddedBitmaps());
    font.SetEmbolden(true);
    font1.SetEmbolden(font.IsEmbolden());
    font.SetLinearMetrics(true);
    font1.SetLinearMetrics(font.IsLinearMetrics());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    int glyphCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF8);
    uint16_t glyphs[glyphCount - 1];
    int count = font.TextToGlyphs(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID,
        glyphs, glyphCount - 1);
    std::string text4 = "GlyphsCount = " + std::to_string(count);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3434
DEF_DTK(font_font_4, TestLevel::L2, 434)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -31.f, -0.5f, 0.7f);
    font.SetBaselineSnap(false);
    font1.SetBaselineSnap(font.IsBaselineSnap());
    font.SetEdging(Drawing::FontEdging::ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::NORMAL);
    font1.SetHinting(font.GetHinting());
    font.SetForceAutoHinting(false);
    font1.SetForceAutoHinting(font.IsForceAutoHinting());
    font.SetEmbeddedBitmaps(false);
    font1.SetEmbeddedBitmaps(font.IsEmbeddedBitmaps());
    font.SetEmbolden(false);
    font1.SetEmbolden(font.IsEmbolden());
    font.SetLinearMetrics(false);
    font1.SetLinearMetrics(font.IsLinearMetrics());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    auto scalar = font.MeasureSingleCharacter(0x44);
    std::string text4 = "Recommended spaceing between lines = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3435
DEF_DTK(font_font_4, TestLevel::L2, 435)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDynamicFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -31.f, 0, 0);
    font.SetEdging(Drawing::FontEdging::SUBPIXEL_ANTI_ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::NORMAL);
    font1.SetHinting(font.GetHinting());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    int glyphCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF8);
    uint16_t glyphs[glyphCount + 1];
    int count = font.TextToGlyphs(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID,
        glyphs, glyphCount + 1);
    std::string text4 = "GlyphsCount = " + std::to_string(count);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

}
}
