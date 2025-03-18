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
#include "rosen/test/dtk/dtk_test_ext.h"
#include <string>
#include "rosen/modules/2d_graphics/include/text/font.h"
#include "rosen/modules/2d_graphics/include/text/font_mgr.h"
#include "rosen/modules/2d_graphics/include/text/font_style_set.h"
#include "rosen/modules/2d_graphics/include/text/font_style.h"
#include "rosen/modules/2d_graphics/include/text/typeface.h"
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

// 对应用例 font_font_3321
DEF_DTK(font_font_3, TestLevel::L2, 321)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->MatchStyle({
        Drawing::FontStyle::INVISIBLE_WEIGHT,
        Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT}));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 71.f, -30.f, -30.f);
    font.SetBaselineSnap(false);
    font1.SetBaselineSnap(font.IsBaselineSnap());
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
    std::vector<std::string> texts = {g_text1, g_text2, g_text3};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3322
DEF_DTK(font_font_3, TestLevel::L2, 322)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->MatchStyle({
        Drawing::FontStyle::INVISIBLE_WEIGHT,
        Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT}));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 71.f, 30.f, -1.f);
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
    int glyphCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF32);
    uint16_t glyphs[glyphCount - 1];
    int count = font.TextToGlyphs(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID,
        glyphs, glyphCount + 1);
    std::string text4 = "GlyphsCount = " + std::to_string(count);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3323
DEF_DTK(font_font_3, TestLevel::L2, 323)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
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
    auto scalar = font.MeasureSingleCharacter(0xEEEEE);
    std::string text4 = "Recommended spaceing between lines = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3324
DEF_DTK(font_font_3, TestLevel::L2, 324)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->MatchStyle({
        Drawing::FontStyle::INVISIBLE_WEIGHT,
        Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT}));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 71.f, 30.f, -1.f);
    font.SetHinting(Drawing::FontHinting::SLIGHT);
    font1.SetHinting(font.GetHinting());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    auto scalar = font.UnicharToGlyph(0x44);
    std::string text4 = "Glyphs = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3325
DEF_DTK(font_font_3, TestLevel::L2, 325)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
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
    auto scalar = font.UnicharToGlyph(0xEEEEE);
    std::string text4 = "Glyphs = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3326
DEF_DTK(font_font_3, TestLevel::L2, 326)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
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

// 对应用例 font_font_3327
DEF_DTK(font_font_3, TestLevel::L2, 327)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->MatchStyle({
        Drawing::FontStyle::INVISIBLE_WEIGHT,
        Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT}));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 500.f, -30.f, -0.5f);
    font.SetEdging(Drawing::FontEdging::SUBPIXEL_ANTI_ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::SLIGHT);
    font1.SetHinting(font.GetHinting());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    auto glyphsCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID);
    std::string text4 = "glyphsCount = " + std::to_string(glyphsCount);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3328
DEF_DTK(font_font_3, TestLevel::L2, 328)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->MatchFamily(familyName.c_str()));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->MatchStyle({
        Drawing::FontStyle::INVISIBLE_WEIGHT,
        Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT}));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 500.f, -1.f, -1.f);
    font.SetEdging(Drawing::FontEdging::ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetEmbolden(false);
    font1.SetEmbolden(font.IsEmbolden());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    auto glyphsCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF8);
    std::string text4 = "glyphsCount = " + std::to_string(glyphsCount);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3329
DEF_DTK(font_font_3, TestLevel::L2, 329)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -50.f, -1.f, 0);
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
    std::vector<std::string> texts = {g_text1, g_text2, g_text3};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3330
DEF_DTK(font_font_3, TestLevel::L2, 330)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -50.f, -1.f, 0);
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
    int glyphCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF16);
    uint16_t glyphs[glyphCount - 1];
    int count = font.TextToGlyphs(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID,
        glyphs, glyphCount + 1);
    std::string text4 = "GlyphsCount = " + std::to_string(count);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3331
DEF_DTK(font_font_3, TestLevel::L2, 331)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -50.f, -1.f, 0);
    font.SetEdging(Drawing::FontEdging::ANTI_ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::SLIGHT);
    font1.SetHinting(font.GetHinting());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    auto scalar = font.MeasureSingleCharacter(0x44);
    std::string text4 = "Recommended spaceing between lines = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3332
DEF_DTK(font_font_3, TestLevel::L2, 332)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -50.f, -0.5f, -0.5f);
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
    auto scalar = font.MeasureSingleCharacter(0xEEEEE);
    std::string text4 = "Recommended spaceing between lines = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3333
DEF_DTK(font_font_3, TestLevel::L2, 333)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -31.f, -0.5f, 0.7f);
    font.SetBaselineSnap(true);
    font1.SetBaselineSnap(font.IsBaselineSnap());
    font.SetEdging(Drawing::FontEdging::SUBPIXEL_ANTI_ALIAS);
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
    auto glyphsCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF16);
    std::string text4 = "glyphsCount = " + std::to_string(glyphsCount);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3334
DEF_DTK(font_font_3, TestLevel::L2, 334)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
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
    int glyphCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF16);
    float widths[glyphCount];
    uint16_t glyphs[glyphCount];
    font.TextToGlyphs(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF8, glyphs, glyphCount);
    font.GetWidths(glyphs, glyphCount, widths);
    std::string text4 = "GetWidths = " + std::to_string(widths[0]);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3335
DEF_DTK(font_font_3, TestLevel::L2, 335)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -31.f, -0.5f, 0.7f);
    font.SetHinting(Drawing::FontHinting::NONE);
    font1.SetHinting(font.GetHinting());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    int glyphCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID);
    uint16_t glyphs[glyphCount - 1];
    int count = font.TextToGlyphs(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID,
        glyphs, glyphCount + 1);
    std::string text4 = "GlyphsCount = " + std::to_string(count);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3336
DEF_DTK(font_font_3, TestLevel::L2, 336)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -31.f, 0, 0);
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
    int glyphCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF8);
    uint16_t glyphs[glyphCount];
    int count = font.TextToGlyphs(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID,
        glyphs, glyphCount + 1);
    std::string text4 = "GlyphsCount = " + std::to_string(count);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3337
DEF_DTK(font_font_3, TestLevel::L2, 337)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -15.34f, 0, 1.f);
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
    Drawing::Rect bounds;
    auto scalar = font.MeasureText(g_text.c_str(), g_text.size(), Drawing::TextEncoding::UTF8, &bounds);
    std::string text4 = "MeasureTextWidths = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3338
DEF_DTK(font_font_3, TestLevel::L2, 338)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -15.34f, 0, 1.f);
    font.SetBaselineSnap(false);
    font1.SetBaselineSnap(font.IsBaselineSnap());
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
    auto glyphsCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF32);
    std::string text4 = "glyphsCount = " + std::to_string(glyphsCount);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3339
DEF_DTK(font_font_3, TestLevel::L2, 339)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -15.34f, 0, 1.f);
    font.SetEdging(Drawing::FontEdging::ANTI_ALIAS);
    font1.SetEdging(font.GetEdging());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    auto scalar = font.UnicharToGlyph(0x44);
    std::string text4 = "Glyphs = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3340
DEF_DTK(font_font_3, TestLevel::L2, 340)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -15.34f, 0.7f, 0.7f);
    font.SetHinting(Drawing::FontHinting::NONE);
    font1.SetHinting(font.GetHinting());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    auto glyphsCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF8);
    std::string text4 = "glyphsCount = " + std::to_string(glyphsCount);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3341
DEF_DTK(font_font_3, TestLevel::L2, 341)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 0, 0.7f, 30.f);
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
    auto scalar = font.UnicharToGlyph(0xEEEEE);
    std::string text4 = "Glyphs = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3342
DEF_DTK(font_font_3, TestLevel::L2, 342)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 0, 0.7f, 30.f);
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
    Drawing::Rect bounds;
    auto scalar = font.MeasureText(g_text.c_str(), g_text.size(), Drawing::TextEncoding::UTF8, &bounds);
    std::string text4 = "MeasureTextWidths = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3343
DEF_DTK(font_font_3, TestLevel::L2, 343)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 0, 0.7f, 30.f);
    font.SetEdging(Drawing::FontEdging::ALIAS);
    font1.SetEdging(font.GetEdging());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    auto glyphsCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID);
    std::string text4 = "glyphsCount = " + std::to_string(glyphsCount);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3344
DEF_DTK(font_font_3, TestLevel::L2, 344)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 0, 1.f, 1.f);
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
    font.SetLinearMetrics(true);
    font1.SetLinearMetrics(font.IsLinearMetrics());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    auto SpaceLine = font.GetMetrics(nullptr);
    std::string text4 = "Recommended spaceing between lines = " + std::to_string(SpaceLine);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3345
DEF_DTK(font_font_3, TestLevel::L2, 345)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 27.13f, 1.f, -30.f);
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
    int glyphCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF8);
    uint16_t glyphs[glyphCount - 1];
    int count = font.TextToGlyphs(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID,
        glyphs, glyphCount + 1);
    std::string text4 = "GlyphsCount = " + std::to_string(count);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3346
DEF_DTK(font_font_3, TestLevel::L2, 346)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 27.13f, 1.f, -30.f);
    font.SetEdging(Drawing::FontEdging::SUBPIXEL_ANTI_ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::FULL);
    font1.SetHinting(font.GetHinting());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    Drawing::Rect bounds;
    auto scalar = font.MeasureText(g_text.c_str(), g_text.size(), Drawing::TextEncoding::UTF16, &bounds);
    std::string text4 = "MeasureTextWidths = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3347
DEF_DTK(font_font_3, TestLevel::L2, 347)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 27.13f, 30.f, 30.f);
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
    font.SetEmbolden(true);
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

// 对应用例 font_font_3348
DEF_DTK(font_font_3, TestLevel::L2, 348)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 71.f, -30.f, -30.f);
    font.SetEdging(Drawing::FontEdging::SUBPIXEL_ANTI_ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::NONE);
    font1.SetHinting(font.GetHinting());
    font.SetEmbolden(false);
    font1.SetEmbolden(font.IsEmbolden());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    int glyphCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF8);
    uint16_t glyphs[glyphCount - 1];
    int count = font.TextToGlyphs(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID, glyphs, glyphCount);
    std::string text4 = "GlyphsCount = " + std::to_string(count);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3349
DEF_DTK(font_font_3, TestLevel::L2, 349)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 71.f, 30.f, -1.f);
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
    Drawing::Rect bounds;
    auto scalar = font.MeasureText(g_text.c_str(), g_text.size(), Drawing::TextEncoding::UTF32, &bounds);
    std::string text4 = "MeasureTextWidths = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3350
DEF_DTK(font_font_3, TestLevel::L2, 350)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 71.f, 30.f, -1.f);
    font.SetEdging(Drawing::FontEdging::ALIAS);
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

// 对应用例 font_font_3351
DEF_DTK(font_font_3, TestLevel::L2, 351)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 500.f, -30.f, -0.5f);
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
    int glyphCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF8);
    uint16_t glyphs[glyphCount - 1];
    int count = font.TextToGlyphs(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID,
        glyphs, glyphCount - 1);
    std::string text4 = "GlyphsCount = " + std::to_string(count);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3352
DEF_DTK(font_font_3, TestLevel::L2, 352)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 500.f, -30.f, -0.5f);
    font.SetBaselineSnap(false);
    font1.SetBaselineSnap(font.IsBaselineSnap());
    font.SetEdging(Drawing::FontEdging::ALIAS);
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
    Drawing::Rect bounds;
    auto scalar = font.MeasureText(g_text.c_str(), g_text.size(), Drawing::TextEncoding::GLYPH_ID, &bounds);
    std::string text4 = "MeasureTextWidths = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3353
DEF_DTK(font_font_3, TestLevel::L2, 353)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 500.f, -1.f, -1.f);
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
    int glyphCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF32);
    uint16_t glyphs[glyphCount - 1];
    int count = font.TextToGlyphs(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID,
        glyphs, glyphCount + 1);
    std::string text4 = "GlyphsCount = " + std::to_string(count);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3354
DEF_DTK(font_font_3, TestLevel::L2, 354)
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
    auto font1 = MakeFont1(font, typeface, -50.f, -1.f, 0);
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
    int glyphCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF16);
    float widths[glyphCount];
    uint16_t glyphs[glyphCount];
    font.TextToGlyphs(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF8, glyphs, glyphCount);
    font.GetWidths(glyphs, glyphCount, widths);
    std::string text4 = "GetWidths = " + std::to_string(widths[0]);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3355
DEF_DTK(font_font_3, TestLevel::L2, 355)
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
    auto font1 = MakeFont1(font, typeface, -50.f, -1.f, 0);
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

// 对应用例 font_font_3356
DEF_DTK(font_font_3, TestLevel::L2, 356)
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
    auto font1 = MakeFont1(font, typeface, -50.f, -1.f, 0);
    font.SetEdging(Drawing::FontEdging::SUBPIXEL_ANTI_ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::NORMAL);
    font1.SetHinting(font.GetHinting());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    auto glyphsCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF16);
    std::string text4 = "glyphsCount = " + std::to_string(glyphsCount);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3357
DEF_DTK(font_font_3, TestLevel::L2, 357)
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
    auto font1 = MakeFont1(font, typeface, -50.f, -0.5f, -0.5f);
    font.SetBaselineSnap(false);
    font1.SetBaselineSnap(font.IsBaselineSnap());
    font.SetEdging(Drawing::FontEdging::ALIAS);
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
    int glyphCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF8);
    uint16_t glyphs[glyphCount + 1];
    int count = font.TextToGlyphs(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID,
        glyphs, glyphCount + 1);
    std::string text4 = "GlyphsCount = " + std::to_string(count);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3358
DEF_DTK(font_font_3, TestLevel::L2, 358)
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
    auto font1 = MakeFont1(font, typeface, -31.f, -0.5f, 0.7f);
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
    auto scalar = font.UnicharToGlyph(0xEEEEE);
    std::string text4 = "Glyphs = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3359
DEF_DTK(font_font_3, TestLevel::L2, 359)
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
    auto font1 = MakeFont1(font, typeface, -31.f, -0.5f, 0.7f);
    font.SetEdging(Drawing::FontEdging::ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::SLIGHT);
    font1.SetHinting(font.GetHinting());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    Drawing::Rect bounds;
    auto scalar = font.MeasureText(g_text.c_str(), g_text.size(), Drawing::TextEncoding::UTF8, &bounds);
    std::string text4 = "MeasureTextWidths = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3360
DEF_DTK(font_font_3, TestLevel::L2, 360)
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
    auto font1 = MakeFont1(font, typeface, -31.f, 0, 0);
    font.SetEdging(Drawing::FontEdging::ANTI_ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::SLIGHT);
    font1.SetHinting(font.GetHinting());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    auto glyphsCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF32);
    std::string text4 = "glyphsCount = " + std::to_string(glyphsCount);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3361
DEF_DTK(font_font_3, TestLevel::L2, 361)
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
    auto font1 = MakeFont1(font, typeface, -15.34f, 0, 1.f);
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

// 对应用例 font_font_3362
DEF_DTK(font_font_3, TestLevel::L2, 362)
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
    auto font1 = MakeFont1(font, typeface, -15.34f, 0, 1.f);
    font.SetEdging(Drawing::FontEdging::SUBPIXEL_ANTI_ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::NONE);
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

// 对应用例 font_font_3363
DEF_DTK(font_font_3, TestLevel::L2, 363)
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
    auto font1 = MakeFont1(font, typeface, -15.34f, 0.7f, 0.7f);
    font.SetBaselineSnap(true);
    font1.SetBaselineSnap(font.IsBaselineSnap());
    font.SetEdging(Drawing::FontEdging::SUBPIXEL_ANTI_ALIAS);
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

// 对应用例 font_font_3364
DEF_DTK(font_font_3, TestLevel::L2, 364)
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
    auto font1 = MakeFont1(font, typeface, 0, 0.7f, 30.f);
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
    int glyphCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF8);
    uint16_t glyphs[glyphCount];
    int count = font.TextToGlyphs(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID,
        glyphs, glyphCount + 1);
    std::string text4 = "GlyphsCount = " + std::to_string(count);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3365
DEF_DTK(font_font_3, TestLevel::L2, 365)
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
    auto font1 = MakeFont1(font, typeface, 0, 0.7f, 30.f);
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

// 对应用例 font_font_3366
DEF_DTK(font_font_3, TestLevel::L2, 366)
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
    auto font1 = MakeFont1(font, typeface, 0, 1.f, 1.f);
    font.SetBaselineSnap(false);
    font1.SetBaselineSnap(font.IsBaselineSnap());
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
    auto scalar = font.UnicharToGlyph(0x44);
    std::string text4 = "Glyphs = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3367
DEF_DTK(font_font_3, TestLevel::L2, 367)
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
    auto font1 = MakeFont1(font, typeface, 27.13f, 1.f, -30.f);
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
    std::vector<std::string> texts = {g_text1, g_text2, g_text3};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3368
DEF_DTK(font_font_3, TestLevel::L2, 368)
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
    auto font1 = MakeFont1(font, typeface, 27.13f, 1.f, -30.f);
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
    int glyphCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF8);
    uint16_t glyphs[glyphCount - 1];
    int count = font.TextToGlyphs(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID, glyphs, glyphCount);
    std::string text4 = "GlyphsCount = " + std::to_string(count);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3369
DEF_DTK(font_font_3, TestLevel::L2, 369)
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
    auto font1 = MakeFont1(font, typeface, 27.13f, 1.f, -30.f);

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    auto scalar = font.MeasureSingleCharacter(0x44);
    std::string text4 = "Recommended spaceing between lines = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3370
DEF_DTK(font_font_3, TestLevel::L2, 370)
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
    auto font1 = MakeFont1(font, typeface, 27.13f, 30.f, 30.f);
    font.SetEdging(Drawing::FontEdging::ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetEmbolden(false);
    font1.SetEmbolden(font.IsEmbolden());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    Drawing::Rect bounds;
    auto scalar = font.MeasureText(g_text.c_str(), g_text.size(), Drawing::TextEncoding::UTF16, &bounds);
    std::string text4 = "MeasureTextWidths = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3371
DEF_DTK(font_font_3, TestLevel::L2, 371)
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
    auto font1 = MakeFont1(font, typeface, 71.f, -30.f, -30.f);
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
    Drawing::Rect bounds;
    auto scalar = font.MeasureText(g_text.c_str(), g_text.size(), Drawing::TextEncoding::GLYPH_ID, &bounds);
    std::string text4 = "MeasureTextWidths = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3372
DEF_DTK(font_font_3, TestLevel::L2, 372)
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
    auto font1 = MakeFont1(font, typeface, 71.f, 30.f, -1.f);
    font.SetBaselineSnap(true);
    font1.SetBaselineSnap(font.IsBaselineSnap());
    font.SetEdging(Drawing::FontEdging::ALIAS);
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
    auto scalar = font.MeasureSingleCharacter(0xEEEEE);
    std::string text4 = "Recommended spaceing between lines = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3373
DEF_DTK(font_font_3, TestLevel::L2, 373)
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
    auto font1 = MakeFont1(font, typeface, 71.f, 30.f, -1.f);
    font.SetBaselineSnap(false);
    font1.SetBaselineSnap(font.IsBaselineSnap());
    font.SetEdging(Drawing::FontEdging::SUBPIXEL_ANTI_ALIAS);
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
    Drawing::FontMetrics metrics;
    auto SpaceLine = font.GetMetrics(&metrics);
    std::string text4 = "Recommended spaceing between lines = " + std::to_string(SpaceLine);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3374
DEF_DTK(font_font_3, TestLevel::L2, 374)
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
    auto font1 = MakeFont1(font, typeface, 71.f, 30.f, -1.f);
    font.SetEdging(Drawing::FontEdging::ANTI_ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::FULL);
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

// 对应用例 font_font_3375
DEF_DTK(font_font_3, TestLevel::L2, 375)
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
    auto font1 = MakeFont1(font, typeface, 500.f, -30.f, -0.5f);
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
    int glyphCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF16);
    uint16_t glyphs[glyphCount - 1];
    int count = font.TextToGlyphs(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID,
        glyphs, glyphCount + 1);
    std::string text4 = "GlyphsCount = " + std::to_string(count);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3376
DEF_DTK(font_font_3, TestLevel::L2, 376)
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
    auto font1 = MakeFont1(font, typeface, 500.f, -30.f, -0.5f);
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
    auto glyphsCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF8);
    std::string text4 = "glyphsCount = " + std::to_string(glyphsCount);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3377
DEF_DTK(font_font_3, TestLevel::L2, 377)
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
    auto font1 = MakeFont1(font, typeface, 500.f, -30.f, -0.5f);
    font.SetHinting(Drawing::FontHinting::NORMAL);
    font1.SetHinting(font.GetHinting());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    auto SpaceLine = font.GetMetrics(nullptr);
    std::string text4 = "Recommended spaceing between lines = " + std::to_string(SpaceLine);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

}
}
