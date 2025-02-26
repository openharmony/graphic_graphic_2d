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
#include "../../dtk_test_ext.h"
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

// 对应用例 font_font_3026
DEF_DTK(font_font_1, TestLevel::L2, 26)
{
    // 创建typeface
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/SanJiMengMengCaiSeJianTi-2.ttf");
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -50.f, -0.5f, 0.7f);
    font.SetBaselineSnap(true);
    font1.SetBaselineSnap(font.IsBaselineSnap());
    font.SetEdging(Drawing::FontEdging::ANTI_ALIAS);
    font1.SetEdging(font.GetEdging());
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

// 对应用例 font_font_3027
DEF_DTK(font_font_1, TestLevel::L2, 27)
{
    // 创建typeface
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/SanJiMengMengCaiSeJianTi-2.ttf");
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -50.f, 0, -0.5f);
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
    auto scalar = font.MeasureText(g_text.c_str(), g_text.size(), Drawing::TextEncoding::UTF16, &bounds);
    std::string text4 = "MeasureTextWidths = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3028
DEF_DTK(font_font_1, TestLevel::L2, 28)
{
    // 创建typeface
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/SanJiMengMengCaiSeJianTi-2.ttf");
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -50.f, 0.7f, -1.f);
    font.SetHinting(Drawing::FontHinting::NORMAL);
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

// 对应用例 font_font_3029
DEF_DTK(font_font_1, TestLevel::L2, 29)
{
    // 创建typeface
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/SanJiMengMengCaiSeJianTi-2.ttf");
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -31.f, -0.5f, -0.5f);
    font.SetEdging(Drawing::FontEdging::ALIAS);
    font1.SetEdging(font.GetEdging());

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

// 对应用例 font_font_3030
DEF_DTK(font_font_1, TestLevel::L2, 30)
{
    // 创建typeface
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/SanJiMengMengCaiSeJianTi-2.ttf");
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -31.f, -0.5f, 0);
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
    auto scalar = font.MeasureSingleCharacter(0x44);
    std::string text4 = "Recommended spaceing between lines = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3031
DEF_DTK(font_font_1, TestLevel::L2, 31)
{
    // 创建typeface
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/SanJiMengMengCaiSeJianTi-2.ttf");
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -31.f, 0, 0.7f);
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

// 对应用例 font_font_3032
DEF_DTK(font_font_1, TestLevel::L2, 32)
{
    // 创建typeface
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/SanJiMengMengCaiSeJianTi-2.ttf");
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -31.f, 0, 1.f);
    font.SetBaselineSnap(false);
    font1.SetBaselineSnap(font.IsBaselineSnap());
    font.SetEdging(Drawing::FontEdging::SUBPIXEL_ANTI_ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::NONE);
    font1.SetHinting(font.GetHinting());
    font.SetEmbeddedBitmaps(true);
    font1.SetEmbeddedBitmaps(font.IsEmbeddedBitmaps());
    font.SetEmbolden(false);
    font1.SetEmbolden(font.IsEmbolden());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    int glyphCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF8);
    uint16_t glyphs[glyphCount - 1];
    int count = font.TextToGlyphs(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID,
        glyphs, glyphCount - 1);
    std::string text4 = "GlyphsCount = " + std::to_string(count);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3033
DEF_DTK(font_font_1, TestLevel::L2, 33)
{
    // 创建typeface
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/SanJiMengMengCaiSeJianTi-2.ttf");
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -31.f, 30.f, 0);
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

// 对应用例 font_font_3034
DEF_DTK(font_font_1, TestLevel::L2, 34)
{
    // 创建typeface
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/SanJiMengMengCaiSeJianTi-2.ttf");
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -15.34f, -0.5f, 0);
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

// 对应用例 font_font_3035
DEF_DTK(font_font_1, TestLevel::L2, 35)
{
    // 创建typeface
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/SanJiMengMengCaiSeJianTi-2.ttf");
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -15.34f, 0.7f, -1.f);
    font.SetEdging(Drawing::FontEdging::ALIAS);
    font1.SetEdging(font.GetEdging());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    auto glyphsCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF32);
    std::string text4 = "glyphsCount = " + std::to_string(glyphsCount);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3036
DEF_DTK(font_font_1, TestLevel::L2, 36)
{
    // 创建typeface
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/SanJiMengMengCaiSeJianTi-2.ttf");
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -15.34f, 0.7f, 30.f);
    font.SetHinting(Drawing::FontHinting::SLIGHT);
    font1.SetHinting(font.GetHinting());
    font.SetForceAutoHinting(true);
    font1.SetForceAutoHinting(font.IsForceAutoHinting());
    font.SetEmbeddedBitmaps(false);
    font1.SetEmbeddedBitmaps(font.IsEmbeddedBitmaps());
    font.SetEmbolden(true);
    font1.SetEmbolden(font.IsEmbolden());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    int glyphCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF16);
    uint16_t glyphs[glyphCount - 1];
    int count = font.TextToGlyphs(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID,
        glyphs, glyphCount + 1);
    std::string text4 = "GlyphsCount = " + std::to_string(count);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3037
DEF_DTK(font_font_1, TestLevel::L2, 37)
{
    // 创建typeface
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/SanJiMengMengCaiSeJianTi-2.ttf");
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -15.34f, 30.f, 30.f);
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
    auto scalar = font.UnicharToGlyph(0x44);
    std::string text4 = "Glyphs = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3038
DEF_DTK(font_font_1, TestLevel::L2, 38)
{
    // 创建typeface
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/SanJiMengMengCaiSeJianTi-2.ttf");
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 0, 0.7f, 30.f);
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
    auto glyphsCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID);
    std::string text4 = "glyphsCount = " + std::to_string(glyphsCount);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3039
DEF_DTK(font_font_1, TestLevel::L2, 39)
{
    // 创建typeface
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/SanJiMengMengCaiSeJianTi-2.ttf");
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 0, 30.f, -30.f);
    font.SetBaselineSnap(true);
    font1.SetBaselineSnap(font.IsBaselineSnap());
    font.SetEdging(Drawing::FontEdging::ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::NORMAL);
    font1.SetHinting(font.GetHinting());
    font.SetForceAutoHinting(true);
    font1.SetForceAutoHinting(font.IsForceAutoHinting());
    font.SetEmbeddedBitmaps(false);
    font1.SetEmbeddedBitmaps(font.IsEmbeddedBitmaps());
    font.SetEmbolden(false);
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

// 对应用例 font_font_3040
DEF_DTK(font_font_1, TestLevel::L2, 40)
{
    // 创建typeface
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/SanJiMengMengCaiSeJianTi-2.ttf");
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 27.13f, -30.f, -30.f);
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
    int glyphCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF8);
    uint16_t glyphs[glyphCount + 1];
    int count = font.TextToGlyphs(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID,
        glyphs, glyphCount + 1);
    std::string text4 = "GlyphsCount = " + std::to_string(count);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3041
DEF_DTK(font_font_1, TestLevel::L2, 41)
{
    // 创建typeface
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/SanJiMengMengCaiSeJianTi-2.ttf");
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 27.13f, -30.f, -1.f);
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

// 对应用例 font_font_3042
DEF_DTK(font_font_1, TestLevel::L2, 42)
{
    // 创建typeface
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/SanJiMengMengCaiSeJianTi-2.ttf");
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 27.13f, -1.f, 30.f);
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
    auto scalar = font.MeasureText(g_text.c_str(), g_text.size(), Drawing::TextEncoding::GLYPH_ID, &bounds);
    std::string text4 = "MeasureTextWidths = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3043
DEF_DTK(font_font_1, TestLevel::L2, 43)
{
    // 创建typeface
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/SanJiMengMengCaiSeJianTi-2.ttf");
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 27.13f, 30.f, -1.f);
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
    auto scalar = font.UnicharToGlyph(0xEEEEE);
    std::string text4 = "Glyphs = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3044
DEF_DTK(font_font_1, TestLevel::L2, 44)
{
    // 创建typeface
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/SanJiMengMengCaiSeJianTi-2.ttf");
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 71.f, -30.f, 0.7f);
    font.SetEdging(Drawing::FontEdging::SUBPIXEL_ANTI_ALIAS);
    font1.SetEdging(font.GetEdging());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    Drawing::Rect bounds;
    auto scalar = font.MeasureText(g_text.c_str(), g_text.size(), Drawing::TextEncoding::UTF32, &bounds);
    std::string text4 = "MeasureTextWidths = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3045
DEF_DTK(font_font_1, TestLevel::L2, 45)
{
    // 创建typeface
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/SanJiMengMengCaiSeJianTi-2.ttf");
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 71.f, -1.f, -0.5f);
    font.SetBaselineSnap(true);
    font1.SetBaselineSnap(font.IsBaselineSnap());
    font.SetEdging(Drawing::FontEdging::ANTI_ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::NONE);
    font1.SetHinting(font.GetHinting());
    font.SetForceAutoHinting(true);
    font1.SetForceAutoHinting(font.IsForceAutoHinting());
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

// 对应用例 font_font_3046
DEF_DTK(font_font_1, TestLevel::L2, 46)
{
    // 创建typeface
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/SanJiMengMengCaiSeJianTi-2.ttf");
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 71.f, 0.7f, 0.7f);
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
    auto SpaceLine = font.GetMetrics(nullptr);
    std::string text4 = "Recommended spaceing between lines = " + std::to_string(SpaceLine);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3047
DEF_DTK(font_font_1, TestLevel::L2, 47)
{
    // 创建typeface
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/SanJiMengMengCaiSeJianTi-2.ttf");
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 71.f, 1.f, -0.5f);
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
    Drawing::FontMetrics metrics;
    auto SpaceLine = font.GetMetrics(&metrics);
    std::string text4 = "Recommended spaceing between lines = " + std::to_string(SpaceLine);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3048
DEF_DTK(font_font_1, TestLevel::L2, 48)
{
    // 创建typeface
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/SanJiMengMengCaiSeJianTi-2.ttf");
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 500.f, 0, -30.f);
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
    int glyphCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF8);
    uint16_t glyphs[glyphCount];
    int count = font.TextToGlyphs(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID,
        glyphs, glyphCount + 1);
    std::string text4 = "GlyphsCount = " + std::to_string(count);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3049
DEF_DTK(font_font_1, TestLevel::L2, 49)
{
    // 创建typeface
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/SanJiMengMengCaiSeJianTi-2.ttf");
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 500.f, 1.f, -1.f);
    font.SetEdging(Drawing::FontEdging::ANTI_ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::SLIGHT);
    font1.SetHinting(font.GetHinting());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    std::vector<std::string> texts = {g_text1, g_text2, g_text3};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3050
DEF_DTK(font_font_1, TestLevel::L2, 50)
{
    // 创建typeface
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/SanJiMengMengCaiSeJianTi-2.ttf");
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 500.f, 1.f, 0);
    font.SetBaselineSnap(false);
    font1.SetBaselineSnap(font.IsBaselineSnap());
    font.SetEdging(Drawing::FontEdging::SUBPIXEL_ANTI_ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::SLIGHT);
    font1.SetHinting(font.GetHinting());
    font.SetForceAutoHinting(true);
    font1.SetForceAutoHinting(font.IsForceAutoHinting());
    font.SetEmbolden(true);
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

// 对应用例 font_font_3051
DEF_DTK(font_font_1, TestLevel::L2, 51)
{
    // 创建typeface
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HmosColorEmojiCompat.ttf");
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -50.f, -0.5f, -1.f);
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
    uint16_t glyphs[glyphCount];
    int count = font.TextToGlyphs(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID,
        glyphs, glyphCount + 1);
    std::string text4 = "GlyphsCount = " + std::to_string(count);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3052
DEF_DTK(font_font_1, TestLevel::L2, 52)
{
    // 创建typeface
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HmosColorEmojiCompat.ttf");
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -50.f, 0.7f, -0.5f);
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
    font.SetLinearMetrics(true);
    font1.SetLinearMetrics(font.IsLinearMetrics());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    Drawing::Rect bounds;
    auto scalar = font.MeasureText(g_text.c_str(), g_text.size(), Drawing::TextEncoding::UTF16, &bounds);
    std::string text4 = "MeasureTextWidths = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3053
DEF_DTK(font_font_1, TestLevel::L2, 53)
{
    // 创建typeface
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HmosColorEmojiCompat.ttf");
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -31.f, 1.f, 30.f);
    font.SetBaselineSnap(true);
    font1.SetBaselineSnap(font.IsBaselineSnap());
    font.SetEdging(Drawing::FontEdging::ANTI_ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::NORMAL);
    font1.SetHinting(font.GetHinting());
    font.SetEmbeddedBitmaps(true);
    font1.SetEmbeddedBitmaps(font.IsEmbeddedBitmaps());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    Drawing::Rect bounds;
    auto scalar = font.MeasureText(g_text.c_str(), g_text.size(), Drawing::TextEncoding::UTF32, &bounds);
    std::string text4 = "MeasureTextWidths = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3054
DEF_DTK(font_font_1, TestLevel::L2, 54)
{
    // 创建typeface
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HmosColorEmojiCompat.ttf");
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -31.f, 30.f, -1.f);
    font.SetEdging(Drawing::FontEdging::ANTI_ALIAS);
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

// 对应用例 font_font_3055
DEF_DTK(font_font_1, TestLevel::L2, 55)
{
    // 创建typeface
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HmosColorEmojiCompat.ttf");
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -15.34f, -1.f, -0.5f);
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
    int glyphCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF8);
    uint16_t glyphs[glyphCount - 1];
    int count = font.TextToGlyphs(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID, glyphs, glyphCount);
    std::string text4 = "GlyphsCount = " + std::to_string(count);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3056
DEF_DTK(font_font_1, TestLevel::L2, 56)
{
    // 创建typeface
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HmosColorEmojiCompat.ttf");
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -15.34f, 0, 0.7f);
    font.SetHinting(Drawing::FontHinting::FULL);
    font1.SetHinting(font.GetHinting());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    Drawing::Rect bounds;
    auto scalar = font.MeasureText(g_text.c_str(), g_text.size(), Drawing::TextEncoding::UTF8, &bounds);
    std::string text4 = "MeasureTextWidths = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3057
DEF_DTK(font_font_1, TestLevel::L2, 57)
{
    // 创建typeface
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HmosColorEmojiCompat.ttf");
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -15.34f, 0.7f, -30.f);
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
    int glyphCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF16);
    float widths[glyphCount];
    uint16_t glyphs[glyphCount];
    font.TextToGlyphs(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF8, glyphs, glyphCount);
    font.GetWidths(glyphs, glyphCount, widths);
    std::string text4 = "GetWidths = " + std::to_string(widths[0]);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3058
DEF_DTK(font_font_1, TestLevel::L2, 58)
{
    // 创建typeface
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HmosColorEmojiCompat.ttf");
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -15.34f, 30.f, 0.7f);
    font.SetForceAutoHinting(true);
    font1.SetForceAutoHinting(font.IsForceAutoHinting());
    font.SetEmbeddedBitmaps(true);
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

// 对应用例 font_font_3059
DEF_DTK(font_font_1, TestLevel::L2, 59)
{
    // 创建typeface
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HmosColorEmojiCompat.ttf");
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 0, -30.f, 0);
    font.SetEdging(Drawing::FontEdging::SUBPIXEL_ANTI_ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetForceAutoHinting(false);
    font1.SetForceAutoHinting(font.IsForceAutoHinting());
    font.SetEmbeddedBitmaps(false);
    font1.SetEmbeddedBitmaps(font.IsEmbeddedBitmaps());
    font.SetEmbolden(false);
    font1.SetEmbolden(font.IsEmbolden());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    auto scalar = font.MeasureSingleCharacter(0x44);
    std::string text4 = "Recommended spaceing between lines = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3060
DEF_DTK(font_font_1, TestLevel::L2, 60)
{
    // 创建typeface
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HmosColorEmojiCompat.ttf");
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 0, -0.5f, 1.f);
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
    int glyphCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF32);
    uint16_t glyphs[glyphCount - 1];
    int count = font.TextToGlyphs(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID,
        glyphs, glyphCount + 1);
    std::string text4 = "GlyphsCount = " + std::to_string(count);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3061
DEF_DTK(font_font_1, TestLevel::L2, 61)
{
    // 创建typeface
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HmosColorEmojiCompat.ttf");
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 0, 0, 1.f);
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
    Drawing::Rect bounds;
    auto scalar = font.MeasureText(g_text.c_str(), g_text.size(), Drawing::TextEncoding::UTF8, &bounds);
    std::string text4 = "MeasureTextWidths = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3062
DEF_DTK(font_font_1, TestLevel::L2, 62)
{
    // 创建typeface
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HmosColorEmojiCompat.ttf");
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 0, 0.7f, 1.f);
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
    std::vector<std::string> texts = {g_text1, g_text2, g_text3};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3063
DEF_DTK(font_font_1, TestLevel::L2, 63)
{
    // 创建typeface
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HmosColorEmojiCompat.ttf");
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 0, 1.f, -0.5f);
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
    uint16_t glyphs[glyphCount - 1];
    int count = font.TextToGlyphs(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID,
        glyphs, glyphCount + 1);
    std::string text4 = "GlyphsCount = " + std::to_string(count);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3064
DEF_DTK(font_font_1, TestLevel::L2, 64)
{
    // 创建typeface
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HmosColorEmojiCompat.ttf");
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 0, 1.f, -0.5f);
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
    int glyphCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID);
    uint16_t glyphs[glyphCount - 1];
    int count = font.TextToGlyphs(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID,
        glyphs, glyphCount + 1);
    std::string text4 = "GlyphsCount = " + std::to_string(count);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3065
DEF_DTK(font_font_1, TestLevel::L2, 65)
{
    // 创建typeface
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HmosColorEmojiCompat.ttf");
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 27.13f, -30.f, 1.f);
    font.SetBaselineSnap(false);
    font1.SetBaselineSnap(font.IsBaselineSnap());
    font.SetEdging(Drawing::FontEdging::SUBPIXEL_ANTI_ALIAS);
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
    auto scalar = font.UnicharToGlyph(0x44);
    std::string text4 = "Glyphs = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3066
DEF_DTK(font_font_1, TestLevel::L2, 66)
{
    // 创建typeface
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HmosColorEmojiCompat.ttf");
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 27.13f, -1.f, -30.f);
    font.SetEdging(Drawing::FontEdging::SUBPIXEL_ANTI_ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::NONE);
    font1.SetHinting(font.GetHinting());
    font.SetEmbolden(true);
    font1.SetEmbolden(font.IsEmbolden());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    auto scalar = font.MeasureSingleCharacter(0xEEEEE);
    std::string text4 = "Recommended spaceing between lines = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3067
DEF_DTK(font_font_1, TestLevel::L2, 67)
{
    // 创建typeface
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HmosColorEmojiCompat.ttf");
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 27.13f, 0, -0.5f);
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
    auto glyphsCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF32);
    std::string text4 = "glyphsCount = " + std::to_string(glyphsCount);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3068
DEF_DTK(font_font_1, TestLevel::L2, 68)
{
    // 创建typeface
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HmosColorEmojiCompat.ttf");
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 27.13f, 1.f, 0);
    font.SetHinting(Drawing::FontHinting::NORMAL);
    font1.SetHinting(font.GetHinting());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    Drawing::FontMetrics metrics;
    auto SpaceLine = font.GetMetrics(&metrics);
    std::string text4 = "Recommended spaceing between lines = " + std::to_string(SpaceLine);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3069
DEF_DTK(font_font_1, TestLevel::L2, 69)
{
    // 创建typeface
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HmosColorEmojiCompat.ttf");
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 27.13f, 1.f, 0.7f);
    font.SetEdging(Drawing::FontEdging::ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::NONE);
    font1.SetHinting(font.GetHinting());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    int glyphCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF16);
    uint16_t glyphs[glyphCount - 1];
    int count = font.TextToGlyphs(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID,
        glyphs, glyphCount + 1);
    std::string text4 = "GlyphsCount = " + std::to_string(count);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3070
DEF_DTK(font_font_1, TestLevel::L2, 70)
{
    // 创建typeface
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HmosColorEmojiCompat.ttf");
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 71.f, -1.f, 0);
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
    int glyphCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF8);
    uint16_t glyphs[glyphCount - 1];
    int count = font.TextToGlyphs(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID,
        glyphs, glyphCount - 1);
    std::string text4 = "GlyphsCount = " + std::to_string(count);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3071
DEF_DTK(font_font_1, TestLevel::L2, 71)
{
    // 创建typeface
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HmosColorEmojiCompat.ttf");
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 71.f, -0.5f, -1.f);
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
    font.SetEmbolden(false);
    font1.SetEmbolden(font.IsEmbolden());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    auto glyphsCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF8);
    std::string text4 = "glyphsCount = " + std::to_string(glyphsCount);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3072
DEF_DTK(font_font_1, TestLevel::L2, 72)
{
    // 创建typeface
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HmosColorEmojiCompat.ttf");
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 71.f, 0.7f, 0);
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

// 对应用例 font_font_3073
DEF_DTK(font_font_1, TestLevel::L2, 73)
{
    // 创建typeface
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HmosColorEmojiCompat.ttf");
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 71.f, 30.f, 30.f);
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
    auto SpaceLine = font.GetMetrics(nullptr);
    std::string text4 = "Recommended spaceing between lines = " + std::to_string(SpaceLine);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3074
DEF_DTK(font_font_1, TestLevel::L2, 74)
{
    // 创建typeface
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HmosColorEmojiCompat.ttf");
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 500.f, -30.f, 0.7f);
    font.SetEdging(Drawing::FontEdging::SUBPIXEL_ANTI_ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::NONE);
    font1.SetHinting(font.GetHinting());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    auto scalar = font.UnicharToGlyph(0xEEEEE);
    std::string text4 = "Glyphs = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3075
DEF_DTK(font_font_1, TestLevel::L2, 75)
{
    // 创建typeface
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HmosColorEmojiCompat.ttf");
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 500.f, 0, -0.5f);
    font.SetForceAutoHinting(false);
    font1.SetForceAutoHinting(font.IsForceAutoHinting());
    font.SetEmbeddedBitmaps(false);
    font1.SetEmbeddedBitmaps(font.IsEmbeddedBitmaps());
    font.SetLinearMetrics(false);
    font1.SetLinearMetrics(font.IsLinearMetrics());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    auto glyphsCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF16);
    std::string text4 = "glyphsCount = " + std::to_string(glyphsCount);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3225
DEF_DTK(font_font_1, TestLevel::L2, 225)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    std::string themeName = "HMOS Color Emoji";
    std::string styleName = "";
    Drawing::FontStyle fontStyle;
    fontStyleSet->GetStyle(0, &fontStyle, &styleName);
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, 1, 0x74));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 500.f, -30.f, -0.5f);
    font.SetEdging(Drawing::FontEdging::SUBPIXEL_ANTI_ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::NORMAL);
    font1.SetHinting(font.GetHinting());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    int glyphCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF8);
    uint16_t glyphs[glyphCount - 1];
    int count = font.TextToGlyphs(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID, glyphs, glyphCount);
    std::string text4 = "GlyphsCount = " + std::to_string(count);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3226
DEF_DTK(font_font_1, TestLevel::L2, 226)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    std::string themeName = "HMOS Color Emoji";
    std::string styleName = "";
    Drawing::FontStyle fontStyle;
    fontStyleSet->GetStyle(0, &fontStyle, &styleName);
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyleCharacter(
        themeName.c_str(), fontStyle, nullptr, 1, 0x74));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 500.f, -1.f, -0.5f);
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
    font.SetEmbolden(true);
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

// 对应用例 font_font_3227
DEF_DTK(font_font_1, TestLevel::L2, 227)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    std::string styleName = "";
    Drawing::FontStyle fontStyle;
    fontStyleSet->GetStyle(0, &fontStyle, &styleName);
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyle(familyName.c_str(), fontStyle));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -50.f, -1.f, 0);
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
    Drawing::Rect bounds;
    auto scalar = font.MeasureText(g_text.c_str(), g_text.size(), Drawing::TextEncoding::UTF16, &bounds);
    std::string text4 = "MeasureTextWidths = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3228
DEF_DTK(font_font_1, TestLevel::L2, 228)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    std::string styleName = "";
    Drawing::FontStyle fontStyle;
    fontStyleSet->GetStyle(0, &fontStyle, &styleName);
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyle(familyName.c_str(), fontStyle));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -50.f, -1.f, 0);
    font.SetEdging(Drawing::FontEdging::ANTI_ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::NORMAL);
    font1.SetHinting(font.GetHinting());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    int glyphCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF8);
    uint16_t glyphs[glyphCount];
    int count = font.TextToGlyphs(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID,
        glyphs, glyphCount + 1);
    std::string text4 = "GlyphsCount = " + std::to_string(count);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3229
DEF_DTK(font_font_1, TestLevel::L2, 229)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    std::string styleName = "";
    Drawing::FontStyle fontStyle;
    fontStyleSet->GetStyle(0, &fontStyle, &styleName);
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyle(familyName.c_str(), fontStyle));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -50.f, -0.5f, -0.5f);
    font.SetEdging(Drawing::FontEdging::ANTI_ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::NONE);
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

// 对应用例 font_font_3230
DEF_DTK(font_font_1, TestLevel::L2, 230)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    std::string styleName = "";
    Drawing::FontStyle fontStyle;
    fontStyleSet->GetStyle(0, &fontStyle, &styleName);
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyle(familyName.c_str(), fontStyle));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -31.f, -0.5f, 0.7f);
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
    int glyphCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF8);
    uint16_t glyphs[glyphCount + 1];
    int count = font.TextToGlyphs(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID,
        glyphs, glyphCount + 1);
    std::string text4 = "GlyphsCount = " + std::to_string(count);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3231
DEF_DTK(font_font_1, TestLevel::L2, 231)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    std::string styleName = "";
    Drawing::FontStyle fontStyle;
    fontStyleSet->GetStyle(0, &fontStyle, &styleName);
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyle(familyName.c_str(), fontStyle));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -31.f, -0.5f, 0.7f);
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
    Drawing::Rect bounds;
    auto scalar = font.MeasureText(g_text.c_str(), g_text.size(), Drawing::TextEncoding::UTF32, &bounds);
    std::string text4 = "MeasureTextWidths = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3232
DEF_DTK(font_font_1, TestLevel::L2, 232)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    std::string styleName = "";
    Drawing::FontStyle fontStyle;
    fontStyleSet->GetStyle(0, &fontStyle, &styleName);
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyle(familyName.c_str(), fontStyle));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -31.f, 0, -1.f);
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
    Drawing::FontMetrics metrics;
    auto SpaceLine = font.GetMetrics(&metrics);
    std::string text4 = "Recommended spaceing between lines = " + std::to_string(SpaceLine);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3233
DEF_DTK(font_font_1, TestLevel::L2, 233)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    std::string styleName = "";
    Drawing::FontStyle fontStyle;
    fontStyleSet->GetStyle(0, &fontStyle, &styleName);
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyle(familyName.c_str(), fontStyle));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -15.34f, 0, 1.f);
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
    int glyphCount = font.CountText(g_text.c_str(), g_text.length(), Drawing::TextEncoding::UTF8);
    uint16_t glyphs[glyphCount - 1];
    int count = font.TextToGlyphs(g_text.c_str(), g_text.length(), Drawing::TextEncoding::GLYPH_ID, glyphs, glyphCount);
    std::string text4 = "GlyphsCount = " + std::to_string(count);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3234
DEF_DTK(font_font_1, TestLevel::L2, 234)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    std::string styleName = "";
    Drawing::FontStyle fontStyle;
    fontStyleSet->GetStyle(0, &fontStyle, &styleName);
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyle(familyName.c_str(), fontStyle));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -15.34f, 0, 1.f);
    font.SetEdging(Drawing::FontEdging::SUBPIXEL_ANTI_ALIAS);
    font1.SetEdging(font.GetEdging());
    font.SetHinting(Drawing::FontHinting::SLIGHT);
    font1.SetHinting(font.GetHinting());

    // font操作接口，如果涉及，获取返回值赋值给text4，并将text4加入vector容器
    Drawing::Rect bounds;
    auto scalar = font.MeasureText(g_text.c_str(), g_text.size(), Drawing::TextEncoding::GLYPH_ID, &bounds);
    std::string text4 = "MeasureTextWidths = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3235
DEF_DTK(font_font_1, TestLevel::L2, 235)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    std::string styleName = "";
    Drawing::FontStyle fontStyle;
    fontStyleSet->GetStyle(0, &fontStyle, &styleName);
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyle(familyName.c_str(), fontStyle));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, -15.34f, 0.7f, 0.7f);
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
    Drawing::Rect bounds;
    auto scalar = font.MeasureText(g_text.c_str(), g_text.size(), Drawing::TextEncoding::UTF8, &bounds);
    std::string text4 = "MeasureTextWidths = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

// 对应用例 font_font_3236
DEF_DTK(font_font_1, TestLevel::L2, 236)
{
    // 创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string familyName = "HMOS Color Emoji";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    std::string styleName = "";
    Drawing::FontStyle fontStyle;
    fontStyleSet->GetStyle(0, &fontStyle, &styleName);
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontMgr->MatchFamilyStyle(familyName.c_str(), fontStyle));
    
    // 设置fontsize到LinearMetrics的属性
    auto font = Drawing::Font();
    auto font1 = MakeFont1(font, typeface, 0, 0.7f, 30.f);
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
    auto scalar = font.MeasureSingleCharacter(0xEEEEE);
    std::string text4 = "Recommended spaceing between lines = " + std::to_string(scalar);
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};
    DrawTexts(texts, font, font1, playbackCanvas_);
}

}
}
