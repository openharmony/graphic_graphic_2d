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
#include "dtk_test_ext.h"
#include "text/font_types.h"
#include "text/text_blob_builder.h"
#include "text/font_mgr.h"
#include "text/font.h"
#include "recording/mem_allocator.h"
#include "text/font_style_set.h"
#include "text/rs_xform.h"
#include "utils/point.h"
#include "utils/rect.h"
#include "text/typeface.h"
#include "text/font_style.h"
#include <sstream>

namespace OHOS {
namespace Rosen {

static void DrawTextBlob(Drawing::Brush brush, std::vector<std::string>& texts,
    std::shared_ptr<Drawing::TextBlob> textBlob, Drawing::Font& font1, TestPlaybackCanvas* playbackCanvas)
{
    int line = 200;
    int interval1 = 100;
    int interval2 = 200;
    int interval3 = 300;
    int interval4 = 400;

    for (auto text : texts) {
        std::shared_ptr<Drawing::TextBlob> textinfo = Drawing::TextBlob::MakeFromText(text.c_str(), text.size(), font1);
        playbackCanvas->AttachBrush(brush);
        playbackCanvas->DrawTextBlob(textBlob.get(), interval2, line);
        playbackCanvas->DrawTextBlob(textinfo.get(), interval2, line + interval1);
        playbackCanvas->DetachBrush();
        Drawing::Pen pen;
        playbackCanvas->AttachPen(pen);
        playbackCanvas->DrawTextBlob(textBlob.get(), interval2, line + interval2);
        playbackCanvas->DrawTextBlob(textinfo.get(), interval2, line + interval3);
        playbackCanvas->DetachPen();
        line += interval4;
    }
}

static std::shared_ptr<Drawing::TextBlob> MakeTextBlob(Drawing::Font& font1)
{
    Drawing::TextBlobBuilder builder;
    const int count = 20;
    const int dimension = 2;
    auto buffer = builder.AllocRunPos(font1, count, nullptr);
    for (int i = 0; i < count; i++) {
        buffer.glyphs[i] = font1.UnicharToGlyph(0x9088);
        buffer.pos[i * dimension] = 50.f * i;
        buffer.pos[i * dimension + 1] = 0;
    }
    std::shared_ptr<Drawing::TextBlob> textBlob = builder.Make();
    return textBlob;
}

static std::vector<Drawing::BlendMode> MakeBlendModes()
{
    std::vector<Drawing::BlendMode> blendModes = {Drawing::BlendMode::CLEAR, Drawing::BlendMode::SRC,
        Drawing::BlendMode::DST, Drawing::BlendMode::SRC_OVER, Drawing::BlendMode::DST_OVER,
        Drawing::BlendMode::SRC_IN, Drawing::BlendMode::DST_IN, Drawing::BlendMode::SRC_OUT,
        Drawing::BlendMode::DST_OUT, Drawing::BlendMode::SRC_ATOP, Drawing::BlendMode::DST_ATOP,
        Drawing::BlendMode::XOR, Drawing::BlendMode::PLUS, Drawing::BlendMode::MODULATE,
        Drawing::BlendMode::SCREEN, Drawing::BlendMode::OVERLAY, Drawing::BlendMode::DARKEN,
        Drawing::BlendMode::LIGHTEN, Drawing::BlendMode::COLOR_DODGE, Drawing::BlendMode::COLOR_BURN,
        Drawing::BlendMode::HARD_LIGHT, Drawing::BlendMode::SOFT_LIGHT, Drawing::BlendMode::DIFFERENCE,
        Drawing::BlendMode::EXCLUSION, Drawing::BlendMode::MULTIPLY, Drawing::BlendMode::HUE,
        Drawing::BlendMode::STATURATION, Drawing::BlendMode::COLOR_MODE, Drawing::BlendMode::LUMINOSITY};

    return blendModes;
}

static std::shared_ptr<Drawing::TextBlob> MakeTextBlob2(Drawing::Font& font1)
{
    const int count = 20;
    const int offset = 4;
    const int t = 100;
    const int r = 18;
    Drawing::TextBlobBuilder builder;
    auto buffer = builder.AllocRunRSXform(font1, count);
    for (int i = 0; i < count; i++) {
        buffer.glyphs[i] = font1.UnicharToGlyph(0x30);
        int idx = i * offset;
        buffer.pos[idx++] = cos(i * r);
        buffer.pos[idx++] = sin(r * i);
        buffer.pos[idx++] = t;
        buffer.pos[idx++] = t;
    }
    std::shared_ptr<Drawing::TextBlob> textBlob = builder.Make();

    return textBlob;
}

// 用例 Font_Scene_Effect_0070
DEF_DTK(Font_Scene_Effect_3, TestLevel::L2, 70)
{
    // 1.创建typeface
    std::string name = "HarmonyOS-Sans";
    auto typeface = Drawing::Typeface::MakeFromName(name.c_str(), Drawing::FontStyle());
    // 2.组合typeface操作接口
    std::string str = "CPAL";
    reverse(str.begin(), str.end());
    uint32_t tagid = *(reinterpret_cast<uint32_t*>(const_cast<char*>(str.c_str())));
    std::string typefacestr = "GetTableSize = " + std::to_string(typeface->GetTableSize(tagid));
    // 3.组合Font类接口,如果是操作类有返回值的接口,获取接口返回值加入vector容器
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 1.0f);
    font.SetSubpixel(false);
    font.SetScaleX(1);
    auto font1 = Drawing::Font(typeface, 50.f, font.GetScaleX(), 1.0f);
    font1.SetSubpixel(font.IsSubpixel());

    // 4.创建TextBlob
    std::string textInfo = "1111111111111111111111111111111111";
    int maxGlyphCount = font1.CountText(textInfo.c_str(), textInfo.size(), Drawing::TextEncoding::UTF8);
    Drawing::RSXform xform[maxGlyphCount];
    for (int i = 0; i < maxGlyphCount; ++i) {
        xform[i].cos_ = cos(10 * i) + 0.1 * i;
        xform[i].sin_ = sin(10 * i);
        xform[i].tx_ = 40 * i + 100;
        xform[i].ty_ = 100;
    }
    std::shared_ptr<Drawing::TextBlob> textBlob = Drawing::TextBlob::MakeFromRSXform(
        textInfo.c_str(), textInfo.size(), &xform[0], font1, Drawing::TextEncoding::UTF8);

    // 5.组合textBlob类接口,如果有返回值则获取上一步创建的textBlob返回值打印
    auto rect = textBlob->Bounds();
    playbackCanvas_->DrawRect(*rect);

    //6. 得到需要绘制的所有返回值text,全部适应固定的textBlob构造方式打印
    std::vector<std::string> texts = {typefacestr};

    //7.组合视效接口并绘制
}

// 用例 Font_Scene_Effect_0071
DEF_DTK(Font_Scene_Effect_3, TestLevel::L2, 71)
{
    // 1.创建typeface
    std::string name = "HarmonyOS-Sans";
    auto typeface = Drawing::Typeface::MakeFromName(name.c_str(), Drawing::FontStyle());
    // 2.组合typeface操作接口
    std::string typefacestr = "IsCustomTypeface = " + std::to_string(typeface->IsCustomTypeface());
    // 3.组合Font类接口,如果是操作类有返回值的接口,获取接口返回值加入vector容器
    auto font1 = Drawing::Font(typeface, 50.f, 1.0f, 1.0f);
    font1.SetSubpixel(false);
    std::string text0 = "DDGR ddgr 鸿蒙 !@#￥%^&*; : , 。";
    Drawing::FontMetrics metrics;
    auto SpaceLine = font1.GetMetrics(&metrics);
    std::string text4 = "Recommended spacing between lines = " + std::to_string(SpaceLine);

    // 4.创建TextBlob
    std::string textInfo = "1111111111111111111111111111111111";
    int maxGlyphCount = font1.CountText(textInfo.c_str(), textInfo.size(), Drawing::TextEncoding::UTF8);
    Drawing::RSXform xform[maxGlyphCount];
    for (int i = 0; i < maxGlyphCount; ++i) {
        xform[i].cos_ = cos(10 * i) + 0.1 * i;
        xform[i].sin_ = sin(10 * i);
        xform[i].tx_ = 40 * i + 100;
        xform[i].ty_ = 100;
    }
    std::shared_ptr<Drawing::TextBlob> textBlob = Drawing::TextBlob::MakeFromRSXform(
        textInfo.c_str(), textInfo.size(), &xform[0], font1, Drawing::TextEncoding::UTF8);

    // 5.组合textBlob类接口,如果有返回值则获取上一步创建的textBlob返回值打印
    std::vector<Drawing::Point> points;
    Drawing::TextBlob::GetDrawingPointsForTextBlob(textBlob.get(), points);
    std::string text2 = "";
    for (int i = 0; i < points.size(); i++) {
        text2 += std::to_string(i) + "-- X：" + std::to_string(points[i].GetX())
                     + "Y：" + std::to_string(points[i].GetY());
    }

    //6. 得到需要绘制的所有返回值text,全部适应固定的textBlob构造方式打印
    std::vector<std::string> texts = {typefacestr, text2, text4};

    //7.组合视效接口并绘制
    Drawing::Brush brush;
    brush.SetColor(0xFF0000FF);
    auto filter = Drawing::Filter();
    filter.SetColorFilter(Drawing::ColorFilter::CreateLumaColorFilter());
    brush.SetFilter(filter);
    DrawTextBlob(brush, texts, textBlob, font1, playbackCanvas_);
}

// 用例 Font_Scene_Effect_0072
DEF_DTK(Font_Scene_Effect_3, TestLevel::L2, 72)
{
    // 1.创建typeface
    std::string name = "HarmonyOS-Sans";
    auto typeface = Drawing::Typeface::MakeFromName(name.c_str(), Drawing::FontStyle());
    // 2.组合typeface操作接口
    std::string str = "CPAL";
    reverse(str.begin(), str.end());
    uint32_t tagid = *(reinterpret_cast<uint32_t*>(const_cast<char*>(str.c_str())));
    std::string typefacestr = "GetTableSize = " + std::to_string(typeface->GetTableSize(tagid));
    // 3.组合Font类接口,如果是操作类有返回值的接口,获取接口返回值加入vector容器
    auto font1 = Drawing::Font(typeface, 50.f, 1.0f, 1.0f);
    font1.SetSubpixel(false);
    std::string text0 = "DDGR ddgr 鸿蒙 !@#￥%^&*; : , 。";
    auto glyphsCount = font1.CountText(text0.c_str(), text0.length(), Drawing::TextEncoding::GLYPH_ID);
    std::string text4 = "glyphsCount = " + std::to_string(glyphsCount);

    // 4.创建TextBlob
    std::string textInfo = "1111111111111111111111111111111111";
    int maxGlyphCount = font1.CountText(textInfo.c_str(), textInfo.size(), Drawing::TextEncoding::UTF8);
    Drawing::RSXform xform[maxGlyphCount];
    for (int i = 0; i < maxGlyphCount; ++i) {
        xform[i].cos_ = cos(10 * i) + 0.1 * i;
        xform[i].sin_ = sin(10 * i);
        xform[i].tx_ = 40 * i + 100;
        xform[i].ty_ = 100;
    }
    std::shared_ptr<Drawing::TextBlob> textBlob = Drawing::TextBlob::MakeFromRSXform(
        textInfo.c_str(), textInfo.size(), &xform[0], font1, Drawing::TextEncoding::UTF8);

    // 5.组合textBlob类接口,如果有返回值则获取上一步创建的textBlob返回值打印
    std::string text = "harmony_os";
    std::vector<uint16_t> glyphid;
    Drawing::TextBlob::GetDrawingGlyphIDforTextBlob(textBlob.get(), glyphid);
    playbackCanvas_->Translate(200, 200);
    for (int row = 0; row < text.size() && row < glyphid.size(); row++) {
        auto path = Drawing::TextBlob::GetDrawingPathforTextBlob(glyphid[row], textBlob.get());
        playbackCanvas_->DrawPath(path);
        playbackCanvas_->Translate(0, 100);
    }

    //6. 得到需要绘制的所有返回值text,全部适应固定的textBlob构造方式打印
    std::vector<std::string> texts = {typefacestr, text4};

    //7.组合视效接口并绘制
    Drawing::Brush brush;
    brush.SetColor(0xFFFF0000);
    auto filter = Drawing::Filter();
    filter.SetImageFilter(Drawing::ImageFilter::CreateBlurImageFilter(
        10.0f, 10.0f, Drawing::TileMode::CLAMP, nullptr, Drawing::ImageBlurType::GAUSS));
    brush.SetFilter(filter);
    DrawTextBlob(brush, texts, textBlob, font1, playbackCanvas_);
}

// 用例 Font_Scene_Effect_0073
DEF_DTK(Font_Scene_Effect_3, TestLevel::L2, 73)
{
    // 1.创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    const uint8_t emptyFontData[10] = { 0 };
    auto typefaceDynamic = fontMgr->LoadDynamicFont("EmptyFont", emptyFontData, 10);
    auto typeface = std::shared_ptr<Drawing::Typeface>(typefaceDynamic);
    // 2.组合typeface操作接口
    typeface->SetHash(100);
    std::string  typefacestr = "GetHash = " + std::to_string(typeface->GetHash());
    // 3.组合Font类接口,如果是操作类有返回值的接口,获取接口返回值加入vector容器
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 1.0f);
    font.SetSubpixel(false);
    font.SetEdging(Drawing::FontEdging::ANTI_ALIAS);
    auto font1 = Drawing::Font(typeface, 50.f, 1.0f, 1.0f);
    font1.SetSubpixel(font.IsSubpixel());
    font1.SetEdging(font.GetEdging());

    // 4.创建TextBlob
    std::shared_ptr<Drawing::TextBlob> textBlob = MakeTextBlob(font1);

    // 5.组合textBlob类接口,如果有返回值则获取上一步创建的textBlob返回值打印
    std::string text = "harmony_os";
    std::vector<uint16_t> glyphid;
    Drawing::TextBlob::GetDrawingGlyphIDforTextBlob(textBlob.get(), glyphid);
    playbackCanvas_->Translate(200, 200);
    for (int row = 0; row < text.size() && row < glyphid.size(); row++) {
        auto path = Drawing::TextBlob::GetDrawingPathforTextBlob(glyphid[row], textBlob.get());
        playbackCanvas_->DrawPath(path);
        playbackCanvas_->Translate(0, 100);
    }

    //6. 得到需要绘制的所有返回值text,全部适应固定的textBlob构造方式打印
    std::vector<std::string> texts = {typefacestr};

    //7.组合视效接口并绘制
    Drawing::Brush brush;
    brush.SetColor(0xFF0000FF);
    auto filter = Drawing::Filter();
    filter.SetColorFilter(Drawing::ColorFilter::CreateLumaColorFilter());
    brush.SetFilter(filter);
    DrawTextBlob(brush, texts, textBlob, font1, playbackCanvas_);
}

// 用例 Font_Scene_Effect_0074
DEF_DTK(Font_Scene_Effect_3, TestLevel::L2, 74)
{
    // 1.创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    const uint8_t emptyFontData[10] = { 0 };
    auto typefaceDynamic = fontMgr->LoadDynamicFont("EmptyFont", emptyFontData, 10);
    auto typeface = std::shared_ptr<Drawing::Typeface>(typefaceDynamic);
    // 2.组合typeface操作接口
    auto data = typeface->Serialize();
    uint32_t size = 10;
    typeface->SetSize(size);
    std::shared_ptr<Drawing::Typeface> typeface1 = Drawing::Typeface::Deserialize(data->GetData(), typeface->GetSize());
    // 3.组合Font类接口,如果是操作类有返回值的接口,获取接口返回值加入vector容器
    auto font = Drawing::Font(typeface1, 50.f, 1.0f, 1.0f);
    font.SetSubpixel(false);
    font.SetSize(100.f);
    auto font1 = Drawing::Font(typeface1, font.GetSize(), 1.0f, 1.0f);
    font1.SetSubpixel(font.IsSubpixel());

    // 4.创建TextBlob
    std::shared_ptr<Drawing::TextBlob> textBlob = MakeTextBlob(font1);

    // 5.组合textBlob类接口,如果有返回值则获取上一步创建的textBlob返回值打印
    std::string text = "harmony_os";
    std::vector<uint16_t> glyphid;
    Drawing::TextBlob::GetDrawingGlyphIDforTextBlob(textBlob.get(), glyphid);
    std::string text2 = "";
    for (int row = 0; row < text.size() && row < glyphid.size(); row++) {
        text2 += std::string(1, text[row]) + "：" + std::to_string(glyphid[row]);
    }

    //6. 得到需要绘制的所有返回值text,全部适应固定的textBlob构造方式打印
    std::vector<std::string> texts = {text2};

    //7.组合视效接口并绘制
}

// 用例 Font_Scene_Effect_0075
DEF_DTK(Font_Scene_Effect_3, TestLevel::L2, 75)
{
    // 1.创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    const uint8_t emptyFontData[10] = { 0 };
    auto typefaceDynamic = fontMgr->LoadDynamicFont("EmptyFont", emptyFontData, 10);
    auto typeface = std::shared_ptr<Drawing::Typeface>(typefaceDynamic);
    // 2.组合typeface操作接口
    std::string str = "CPAL";
    reverse(str.begin(), str.end());
    uint32_t tagid = *(reinterpret_cast<uint32_t*>(const_cast<char*>(str.c_str())));
    std::string typefacestr = "GetTableSize = " + std::to_string(typeface->GetTableSize(tagid));
    // 3.组合Font类接口,如果是操作类有返回值的接口,获取接口返回值加入vector容器
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 1.0f);
    font.SetSubpixel(false);
    font.SetSkewX(1.0f);
    auto font1 = Drawing::Font(typeface, 50.f, font.GetSkewX(), 1.0f);
    font1.SetSubpixel(font.IsSubpixel());

    // 4.创建TextBlob
    std::shared_ptr<Drawing::TextBlob> textBlob = MakeTextBlob(font1);

    // 5.组合textBlob类接口,如果有返回值则获取上一步创建的textBlob返回值打印
    std::string textinfo1 = "Deserialize @Hello World";
    Drawing::TextBlob::Context* Ctx = new (std::nothrow) Drawing::TextBlob::Context(typeface, false);
    auto data2 = textBlob->Serialize(Ctx);
    std::shared_ptr<Drawing::TextBlob> infoTextBlob2 =
        Drawing::TextBlob::Deserialize(data2->GetData(), data2->GetSize(), Ctx);

    //6. 得到需要绘制的所有返回值text,全部适应固定的textBlob构造方式打印
    std::vector<std::string> texts = {typefacestr};

    //7.组合视效接口并绘制
    Drawing::Brush brush;
    std::vector<Drawing::ColorQuad> colors =
        {Drawing::Color::COLOR_RED, Drawing::Color::COLOR_GREEN, Drawing::Color::COLOR_BLUE};
    std::vector<Drawing::scalar> pos = {0.00f, 0.50f, 1.00f};
    auto linearGradient =
        Drawing::ShaderEffect::CreateLinearGradient({0, 0}, {1000, 1000}, colors, pos, Drawing::TileMode::CLAMP);
    brush.SetShaderEffect(linearGradient);
    DrawTextBlob(brush, texts, textBlob, font1, playbackCanvas_);
}

// 用例 Font_Scene_Effect_0076
DEF_DTK(Font_Scene_Effect_3, TestLevel::L2, 76)
{
    // 1.创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    const uint8_t emptyFontData[10] = { 0 };
    auto typefaceDynamic = fontMgr->LoadDynamicFont("EmptyFont", emptyFontData, 10);
    auto typeface = std::shared_ptr<Drawing::Typeface>(typefaceDynamic);
    // 2.组合typeface操作接口
    auto data = typeface->Serialize();
    uint32_t size = 10;
    typeface->SetSize(size);
    std::shared_ptr<Drawing::Typeface> typeface1 = Drawing::Typeface::Deserialize(data->GetData(), typeface->GetSize());
    // 3.组合Font类接口,如果是操作类有返回值的接口,获取接口返回值加入vector容器
    auto font = Drawing::Font(typeface1, 50.f, 1.0f, 1.0f);
    font.SetSubpixel(false);
    font.SetBaselineSnap(true);
    auto font1 = Drawing::Font(typeface1, 50.f, 1.0f, 1.0f);
    font1.SetSubpixel(font.IsSubpixel());
    font1.SetBaselineSnap(font.IsBaselineSnap());

    // 4.创建TextBlob
    Drawing::TextBlobBuilder builder;
    auto buffer = builder.AllocRunRSXform(font1, 20);
    for (int i = 0; i < 20; i++) {
        buffer.glyphs[i] = font1.UnicharToGlyph(0x30);
        buffer.pos[i * 4] = cos(i * 18);
        buffer.pos[i * 4 + 1] = sin(18 * i);
        buffer.pos[i * 4 + 2] = 100;
        buffer.pos[i * 4 + 3] = 100;
    }
    std::shared_ptr<Drawing::TextBlob> textBlob = builder.Make();

    // 5.组合textBlob类接口,如果有返回值则获取上一步创建的textBlob返回值打印
    std::vector<Drawing::Point> points;
    Drawing::TextBlob::GetDrawingPointsForTextBlob(textBlob.get(), points);
    std::string text2 = "";
    for (int i = 0; i < points.size(); i++) {
        text2 += std::to_string(i) + "-- X：" + std::to_string(points[i].GetX())
                     + "Y：" + std::to_string(points[i].GetY());
    }

    //6. 得到需要绘制的所有返回值text,全部适应固定的textBlob构造方式打印
    std::vector<std::string> texts = {text2};

    //7.组合视效接口并绘制
    Drawing::Brush brush;
    brush.SetColor(0x4CB21933);
    std::vector<Drawing::BlendMode> blendModes = MakeBlendModes();
    
    int rectPos = 0;
    for (auto blendMode : blendModes) {
        auto background = Drawing::ImageFilter::CreateBlurImageFilter(
            1.0f, 1.0f, Drawing::TileMode::REPEAT, nullptr, Drawing::ImageBlurType::GAUSS);
        auto foreground = Drawing::ImageFilter::CreateBlurImageFilter(
            1.0f, 1.0f, Drawing::TileMode::REPEAT, nullptr, Drawing::ImageBlurType::GAUSS);
        auto filter = Drawing::Filter();
        filter.SetImageFilter(Drawing::ImageFilter::CreateBlendImageFilter(blendMode, background, foreground));
        brush.SetFilter(filter);
        DrawTextBlob(brush, texts, textBlob, font1, playbackCanvas_);
        rectPos += 200;
    }
}

// 用例 Font_Scene_Effect_0077
DEF_DTK(Font_Scene_Effect_3, TestLevel::L2, 77)
{
    // 1.创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    const uint8_t emptyFontData[10] = { 0 };
    auto typefaceDynamic = fontMgr->LoadDynamicFont("EmptyFont", emptyFontData, 10);
    auto typeface = std::shared_ptr<Drawing::Typeface>(typefaceDynamic);
    // 2.组合typeface操作接口
    std::string typefacestr = "IsCustomTypeface = " + std::to_string(typeface->IsCustomTypeface());
    // 3.组合Font类接口,如果是操作类有返回值的接口,获取接口返回值加入vector容器
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 1.0f);
    font.SetSubpixel(false);
    font.SetEmbolden(false);
    auto font1 = Drawing::Font(typeface, 50.f, 1.0f, 1.0f);
    font1.SetSubpixel(font.IsSubpixel());
    font1.SetEmbolden(font.IsEmbolden());

    // 4.创建TextBlob
    Drawing::TextBlobBuilder builder;
    auto buffer = builder.AllocRunRSXform(font1, 20);
    for (int i = 0; i < 20; i++) {
        buffer.glyphs[i] = font1.UnicharToGlyph(0x30);
        buffer.pos[i * 4] = cos(i * 18);
        buffer.pos[i * 4 + 1] = sin(18 * i);
        buffer.pos[i * 4 + 2] = 100;
        buffer.pos[i * 4 + 3] = 100;
    }
    std::shared_ptr<Drawing::TextBlob> textBlob = builder.Make();

    // 5.组合textBlob类接口,如果有返回值则获取上一步创建的textBlob返回值打印
    std::string text = "harmony_os";
    std::vector<uint16_t> glyphid;
    Drawing::TextBlob::GetDrawingGlyphIDforTextBlob(textBlob.get(), glyphid);
    playbackCanvas_->Translate(200, 200);
    for (int row = 0; row < text.size() && row < glyphid.size(); row++) {
        auto path = Drawing::TextBlob::GetDrawingPathforTextBlob(glyphid[row], textBlob.get());
        playbackCanvas_->DrawPath(path);
        playbackCanvas_->Translate(0, 100);
    }

    //6. 得到需要绘制的所有返回值text,全部适应固定的textBlob构造方式打印
    std::vector<std::string> texts = {typefacestr};

    //7.组合视效接口并绘制
    Drawing::Brush brush;
    brush.SetColor(0xFFFF0000);
    std::vector<Drawing::BlendMode> blendModes = MakeBlendModes();
    
    int rectPos = 0;

    auto dst = Drawing::ShaderEffect::CreateColorShader(Drawing::Color::COLOR_RED);
    auto src = Drawing::ShaderEffect::CreateColorShader(Drawing::Color::COLOR_GREEN);
    for (auto blendMode : blendModes) {
        auto blendShader = Drawing::ShaderEffect::CreateBlendShader(*dst, *src, blendMode);
        brush.SetShaderEffect(blendShader);
        DrawTextBlob(brush, texts, textBlob, font1, playbackCanvas_);
        rectPos += 200;
    }
}

// 用例 Font_Scene_Effect_0078
DEF_DTK(Font_Scene_Effect_3, TestLevel::L2, 78)
{
    // 1.创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    const uint8_t emptyFontData[10] = { 0 };
    auto typefaceDynamic = fontMgr->LoadDynamicFont("EmptyFont", emptyFontData, 10);
    auto typeface = std::shared_ptr<Drawing::Typeface>(typefaceDynamic);
    // 2.组合typeface操作接口
    std::string s = "011c50";
    int a;
    std::stringstream ss;
    ss<<std::hex<<s;
    ss>>a;
    uint32_t tagid = *(reinterpret_cast<uint32_t*>(const_cast<char*>(s.c_str())));
    std::string typefacestr = "GetTableData = "
        + std::to_string(typeface->GetTableData(tagid, a, typeface->GetTableSize(tagid), nullptr));
    // 3.组合Font类接口,如果是操作类有返回值的接口,获取接口返回值加入vector容器
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 1.0f);
    font.SetSubpixel(false);
    font.SetLinearMetrics(false);
    auto font1 = Drawing::Font(typeface, 50.f, 1.0f, 1.0f);
    font1.SetSubpixel(font.IsSubpixel());
    font1.SetLinearMetrics(font.IsLinearMetrics());

    // 4.创建TextBlob
    Drawing::TextBlobBuilder builder;
    auto buffer = builder.AllocRunRSXform(font1, 20);
    for (int i = 0; i < 20; i++) {
        buffer.glyphs[i] = font1.UnicharToGlyph(0x30);
        buffer.pos[i * 4] = cos(i * 18);
        buffer.pos[i * 4 + 1] = sin(18 * i);
        buffer.pos[i * 4 + 2] = 100;
        buffer.pos[i * 4 + 3] = 100;
    }
    std::shared_ptr<Drawing::TextBlob> textBlob = builder.Make();

    // 5.组合textBlob类接口,如果有返回值则获取上一步创建的textBlob返回值打印
    std::string name1 = "HMOS Color Emoji";
    std::string textInfo1 = "😊😂🤣😍😒💕😘😁👍🙌👌";
    std::shared_ptr<Drawing::TextBlob> textBlob1 = Drawing::TextBlob::MakeFromText(textInfo1.c_str(),
                                                      textInfo1.size(), font1, Drawing::TextEncoding::UTF8);
    if (textBlob1->IsEmoji()) {
        playbackCanvas_->DrawBackground(0xFF0000FF);
    } else {
        playbackCanvas_->DrawBackground(0xFFFF0000);
    }

    //6. 得到需要绘制的所有返回值text,全部适应固定的textBlob构造方式打印
    std::vector<std::string> texts = {typefacestr};

    //7.组合视效接口并绘制
    Drawing::Brush brush;
    brush.SetColor(0xFFFF0000);
    auto colorShader = Drawing::ShaderEffect::CreateColorShader(Drawing::Color::COLOR_RED);
    brush.SetShaderEffect(colorShader);
    DrawTextBlob(brush, texts, textBlob, font1, playbackCanvas_);
}

// 用例 Font_Scene_Effect_0079
DEF_DTK(Font_Scene_Effect_3, TestLevel::L2, 79)
{
    // 1.创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    const uint8_t emptyFontData[10] = { 0 };
    auto typefaceDynamic = fontMgr->LoadDynamicFont("EmptyFont", emptyFontData, 10);
    auto typeface = std::shared_ptr<Drawing::Typeface>(typefaceDynamic);
    // 2.组合typeface操作接口
    auto familyNamex = typeface->GetFamilyName();
    std::string typefacestr = "GetFamilyName = " + familyNamex;
    // 3.组合Font类接口,如果是操作类有返回值的接口,获取接口返回值加入vector容器
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 1.0f);
    font.SetSubpixel(false);
    font.SetForceAutoHinting(false);
    auto font1 = Drawing::Font(typeface, 50.f, 1.0f, 1.0f);
    font1.SetSubpixel(font.IsSubpixel());
    font1.SetForceAutoHinting(font.IsForceAutoHinting());

    // 4.创建TextBlob
    std::string textInfo = "😊😂🤣😍😒💕😘😁👍🙌👌";
    std::shared_ptr<Drawing::TextBlob> textBlob = Drawing::TextBlob::MakeFromText(
        textInfo.c_str(), textInfo.size(), font1, Drawing::TextEncoding::UTF8);

    // 5.组合textBlob类接口,如果有返回值则获取上一步创建的textBlob返回值打印
    auto rect = textBlob->Bounds();
    playbackCanvas_->DrawRect(*rect);

    //6. 得到需要绘制的所有返回值text,全部适应固定的textBlob构造方式打印
    std::vector<std::string> texts = {typefacestr};

    //7.组合视效接口并绘制
    Drawing::Brush brush;
    brush.SetColor(0xFFFF0000);
    std::vector<Drawing::BlendMode> blendModes = MakeBlendModes();
    
    int rectPos = 0;

    auto dst = Drawing::ShaderEffect::CreateColorShader(Drawing::Color::COLOR_RED);
    auto src = Drawing::ShaderEffect::CreateColorShader(Drawing::Color::COLOR_GREEN);
    for (auto blendMode : blendModes) {
        auto blendShader = Drawing::ShaderEffect::CreateBlendShader(*dst, *src, blendMode);
        brush.SetShaderEffect(blendShader);
        DrawTextBlob(brush, texts, textBlob, font1, playbackCanvas_);
        rectPos += 200;
    }
}

// 用例 Font_Scene_Effect_0080
DEF_DTK(Font_Scene_Effect_3, TestLevel::L2, 80)
{
    // 1.创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    const uint8_t emptyFontData[10] = { 0 };
    auto typefaceDynamic = fontMgr->LoadDynamicFont("EmptyFont", emptyFontData, 10);
    auto typeface = std::shared_ptr<Drawing::Typeface>(typefaceDynamic);
    // 2.组合typeface操作接口
    std::string typefacestr = "GetUniqueID = " + std::to_string(typeface->GetUniqueID());
    // 3.组合Font类接口,如果是操作类有返回值的接口,获取接口返回值加入vector容器
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 1.0f);
    font.SetSubpixel(false);
    font.SetScaleX(1);
    auto font1 = Drawing::Font(typeface, 50.f, font.GetScaleX(), 1.0f);
    font1.SetSubpixel(font.IsSubpixel());

    // 4.创建TextBlob
    std::string textInfo = "😊😂🤣😍😒💕😘😁👍🙌👌";
    std::shared_ptr<Drawing::TextBlob> textBlob = Drawing::TextBlob::MakeFromText(
        textInfo.c_str(), textInfo.size(), font1, Drawing::TextEncoding::UTF8);

    // 5.组合textBlob类接口,如果有返回值则获取上一步创建的textBlob返回值打印
    std::vector<Drawing::Point> points;
    Drawing::TextBlob::GetDrawingPointsForTextBlob(textBlob.get(), points);
    std::string text2 = "";
    for (int i = 0; i < points.size(); i++) {
        text2 += std::to_string(i) + "-- X：" + std::to_string(points[i].GetX())
                     + "Y：" + std::to_string(points[i].GetY());
    }

    //6. 得到需要绘制的所有返回值text,全部适应固定的textBlob构造方式打印
    std::vector<std::string> texts = {typefacestr, text2};

    //7.组合视效接口并绘制
    Drawing::Brush brush;
    brush.SetColor(0xFFFF0000);
    auto filter = Drawing::Filter();
    filter.SetImageFilter(Drawing::ImageFilter::CreateBlurImageFilter(
        10.0f, 10.0f, Drawing::TileMode::CLAMP, nullptr, Drawing::ImageBlurType::GAUSS));
    brush.SetFilter(filter);
    DrawTextBlob(brush, texts, textBlob, font1, playbackCanvas_);
}

// 用例 Font_Scene_Effect_0081
DEF_DTK(Font_Scene_Effect_3, TestLevel::L2, 81)
{
    // 1.创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    const uint8_t emptyFontData[10] = { 0 };
    auto typefaceDynamic = fontMgr->LoadDynamicFont("EmptyFont", emptyFontData, 10);
    auto typeface = std::shared_ptr<Drawing::Typeface>(typefaceDynamic);
    // 2.组合typeface操作接口
    std::string typefacestr = "GetItalic = " + std::to_string(typeface->GetItalic());
    // 3.组合Font类接口,如果是操作类有返回值的接口,获取接口返回值加入vector容器
    auto font1 = Drawing::Font(typeface, 50.f, 1.0f, 1.0f);
    font1.SetSubpixel(false);
    std::string text0 = "DDGR ddgr 鸿蒙 !@#￥%^&*; : , 。";
    Drawing::FontMetrics metrics;
    auto SpaceLine = font1.GetMetrics(&metrics);
    std::string text4 = "Recommended spacing between lines = " + std::to_string(SpaceLine);

    // 4.创建TextBlob
    std::string textInfo = "😊😂🤣😍😒💕😘😁👍🙌👌";
    std::shared_ptr<Drawing::TextBlob> textBlob = Drawing::TextBlob::MakeFromText(
        textInfo.c_str(), textInfo.size(), font1, Drawing::TextEncoding::UTF8);

    // 5.组合textBlob类接口,如果有返回值则获取上一步创建的textBlob返回值打印
    Drawing::Paint paint;
    paint.SetColor(0xFFFF0000);
    float boundsx[] = {1, 2, 3};
    int intercepts = textBlob->GetIntercepts(boundsx, nullptr, &paint);
    std::string text2 = "intercepts = " + std::to_string(intercepts);

    //6. 得到需要绘制的所有返回值text,全部适应固定的textBlob构造方式打印
    std::vector<std::string> texts = {typefacestr, text2, text4};

    //7.组合视效接口并绘制
    Drawing::Brush brush;
    brush.SetColor(0xFFFF0000);
    std::shared_ptr<Drawing::ColorFilter> colorFilter1 =
        Drawing::ColorFilter::CreateBlendModeColorFilter(0xFFFF0000, Drawing::BlendMode::SRC_IN);
    std::shared_ptr<Drawing::ColorFilter> colorFilter2 =
        Drawing::ColorFilter::CreateBlendModeColorFilter(0xFFFF00FF, Drawing::BlendMode::MODULATE);
    std::shared_ptr<Drawing::ColorFilter> colorFilter3
        = Drawing::ColorFilter::CreateComposeColorFilter(*colorFilter1, *colorFilter2);
    auto filter = Drawing::Filter();
    filter.SetColorFilter(colorFilter3);
    brush.SetFilter(filter);
    DrawTextBlob(brush, texts, textBlob, font1, playbackCanvas_);
}

// 用例 Font_Scene_Effect_0082
DEF_DTK(Font_Scene_Effect_3, TestLevel::L2, 82)
{
    // 1.创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    const uint8_t emptyFontData[10] = { 0 };
    auto typefaceDynamic = fontMgr->LoadDynamicFont("EmptyFont", emptyFontData, 10);
    auto typeface = std::shared_ptr<Drawing::Typeface>(typefaceDynamic);
    // 2.组合typeface操作接口
    std::string typefacestr = "IsCustomTypeface = " + std::to_string(typeface->IsCustomTypeface());
    // 3.组合Font类接口,如果是操作类有返回值的接口,获取接口返回值加入vector容器
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 1.0f);
    font.SetSubpixel(false);
    font.SetHinting(Drawing::FontHinting::NORMAL);
    auto font1 = Drawing::Font(typeface, 50.f, 1.0f, 1.0f);
    font1.SetSubpixel(font.IsSubpixel());
    font1.SetHinting(font.GetHinting());

    // 4.创建TextBlob
    std::string textInfo = "harmony_os";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(-100 + 50 * i);
        p[i].SetY(1000 - 50 * i);
    }
    std::shared_ptr<Drawing::TextBlob> textBlob = Drawing::TextBlob::MakeFromPosText(
        textInfo.c_str(), 10, p, font1, Drawing::TextEncoding::UTF8);

    // 5.组合textBlob类接口,如果有返回值则获取上一步创建的textBlob返回值打印
    std::string textinfo1 = "Deserialize @Hello World";
    Drawing::TextBlob::Context* Ctx = new (std::nothrow) Drawing::TextBlob::Context(typeface, false);
    auto data2 = textBlob->Serialize(Ctx);
    std::shared_ptr<Drawing::TextBlob> infoTextBlob2 =
        Drawing::TextBlob::Deserialize(data2->GetData(), data2->GetSize(), Ctx);

    //6. 得到需要绘制的所有返回值text,全部适应固定的textBlob构造方式打印
    std::vector<std::string> texts = {typefacestr};

    //7.组合视效接口并绘制
    Drawing::Brush brush;
    brush.SetColor(0xFFFF0000);
    std::shared_ptr<Drawing::ColorFilter> colorFilter1 =
        Drawing::ColorFilter::CreateBlendModeColorFilter(0xFFFF0000, Drawing::BlendMode::SRC_IN);
    std::shared_ptr<Drawing::ColorFilter> colorFilter2 =
        Drawing::ColorFilter::CreateBlendModeColorFilter(0xFFFF00FF, Drawing::BlendMode::MODULATE);
    std::shared_ptr<Drawing::ColorFilter> colorFilter3
        = Drawing::ColorFilter::CreateComposeColorFilter(*colorFilter1, *colorFilter2);
    auto filter = Drawing::Filter();
    filter.SetColorFilter(colorFilter3);
    brush.SetFilter(filter);
    DrawTextBlob(brush, texts, textBlob, font1, playbackCanvas_);
}

// 用例 Font_Scene_Effect_0083
DEF_DTK(Font_Scene_Effect_3, TestLevel::L2, 83)
{
    // 1.创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    const uint8_t emptyFontData[10] = { 0 };
    auto typefaceDynamic = fontMgr->LoadDynamicFont("EmptyFont", emptyFontData, 10);
    auto typeface = std::shared_ptr<Drawing::Typeface>(typefaceDynamic);
    // 2.组合typeface操作接口
    std::string typefacestr = "GetUniqueID = " + std::to_string(typeface->GetUniqueID());
    // 3.组合Font类接口,如果是操作类有返回值的接口,获取接口返回值加入vector容器
    auto font1 = Drawing::Font(typeface, 50.f, 1.0f, 1.0f);
    font1.SetSubpixel(false);
    std::string text0 = "DDGR ddgr 鸿蒙 !@#￥%^&*; : , 。";
    int glyphCount = font1.CountText(text0.c_str(), text0.length(), Drawing::TextEncoding::UTF8);
    float widths[glyphCount];
    uint16_t glyphs[glyphCount];
    font1.TextToGlyphs(text0.c_str(), text0.length(), Drawing::TextEncoding::UTF8, glyphs, glyphCount);
    font1.GetWidths(glyphs, glyphCount, widths);
    std::string text4 = "Getwidths = " + std::to_string(widths[0]);

    // 4.创建TextBlob
    std::string textInfo = "harmony_os";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(-100 + 50 * i);
        p[i].SetY(1000 - 50 * i);
    }
    std::shared_ptr<Drawing::TextBlob> textBlob = Drawing::TextBlob::MakeFromPosText(
        textInfo.c_str(), 10, p, font1, Drawing::TextEncoding::UTF8);

    // 5.组合textBlob类接口,如果有返回值则获取上一步创建的textBlob返回值打印
    std::string text = "harmony_os";
    std::vector<uint16_t> glyphid;
    Drawing::TextBlob::GetDrawingGlyphIDforTextBlob(textBlob.get(), glyphid);
    std::string text2 = "";
    for (int row = 0; row < text.size() && row < glyphid.size(); row++) {
        text2 += std::string(1, text[row]) + "：" + std::to_string(glyphid[row]);
    }

    //6. 得到需要绘制的所有返回值text,全部适应固定的textBlob构造方式打印
    std::vector<std::string> texts = {typefacestr, text2, text4};

    //7.组合视效接口并绘制
    std::vector<Drawing::BlendMode> blendModes = MakeBlendModes();

    Drawing::Brush brush;
    brush.SetColor(0xFFFF0000);

    int rectPos = 0;
    for (auto blendMode : blendModes) {
        std::shared_ptr<Drawing::ColorFilter> colorFilter
            = Drawing::ColorFilter::CreateBlendModeColorFilter(0xFFFF0000, blendMode);
        auto filter = Drawing::Filter();
        filter.SetColorFilter(colorFilter);
        brush.SetFilter(filter);
        DrawTextBlob(brush, texts, textBlob, font1, playbackCanvas_);
        rectPos += 200;
    }
}

// 用例 Font_Scene_Effect_0084
DEF_DTK(Font_Scene_Effect_3, TestLevel::L2, 84)
{
    // 1.创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    const uint8_t emptyFontData[10] = { 0 };
    auto typefaceDynamic = fontMgr->LoadDynamicFont("EmptyFont", emptyFontData, 10);
    auto typeface = std::shared_ptr<Drawing::Typeface>(typefaceDynamic);
    // 2.组合typeface操作接口
    auto familyNamex = typeface->GetFamilyName();
    std::string typefacestr = "GetFamilyName = " + familyNamex;
    // 3.组合Font类接口,如果是操作类有返回值的接口,获取接口返回值加入vector容器
    auto font1 = Drawing::Font(typeface, 50.f, 1.0f, 1.0f);
    font1.SetSubpixel(false);
    std::string text0 = "DDGR ddgr 鸿蒙 !@#￥%^&*; : , 。";
    Drawing::Rect bounds;
    auto scalar = font1.MeasureText(text0.c_str(), text0.length(), Drawing::TextEncoding::UTF32, &bounds);
    std::string text4 = "MeasureTextWidths = " + std::to_string(scalar);

    // 4.创建TextBlob
    std::string textInfo = "harmony_os";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(-100 + 50 * i);
        p[i].SetY(1000 - 50 * i);
    }
    std::shared_ptr<Drawing::TextBlob> textBlob = Drawing::TextBlob::MakeFromPosText(
        textInfo.c_str(), 10, p, font1, Drawing::TextEncoding::UTF8);

    // 5.组合textBlob类接口,如果有返回值则获取上一步创建的textBlob返回值打印
    auto rect = textBlob->Bounds();
    playbackCanvas_->DrawRect(*rect);

    //6. 得到需要绘制的所有返回值text,全部适应固定的textBlob构造方式打印
    std::vector<std::string> texts = {typefacestr, text4};

    //7.组合视效接口并绘制
}

// 用例 Font_Scene_Effect_0085
DEF_DTK(Font_Scene_Effect_3, TestLevel::L2, 85)
{
    // 1.创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    const uint8_t emptyFontData[10] = { 0 };
    auto typefaceDynamic = fontMgr->LoadDynamicFont("EmptyFont", emptyFontData, 10);
    auto typeface = std::shared_ptr<Drawing::Typeface>(typefaceDynamic);
    // 2.组合typeface操作接口
    typeface->SetHash(100);
    std::string  typefacestr = "GetHash = " + std::to_string(typeface->GetHash());
    // 3.组合Font类接口,如果是操作类有返回值的接口,获取接口返回值加入vector容器
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 1.0f);
    font.SetSubpixel(false);
    font.SetEmbeddedBitmaps(true);
    auto font1 = Drawing::Font(typeface, 50.f, 1.0f, 1.0f);
    font1.SetSubpixel(font.IsSubpixel());
    font1.SetEmbeddedBitmaps(font.IsEmbeddedBitmaps());

    // 4.创建TextBlob
    std::string textInfo = "harmony_os";
    std::shared_ptr<Drawing::TextBlob> textBlob = Drawing::TextBlob::MakeFromString(
        textInfo.c_str(), font1, Drawing::TextEncoding::UTF8);

    // 5.组合textBlob类接口,如果有返回值则获取上一步创建的textBlob返回值打印
    std::string name1 = "HMOS Color Emoji";
    std::string textInfo1 = "😊😂🤣😍😒💕😘😁👍🙌👌";
    std::shared_ptr<Drawing::TextBlob> textBlob1 = Drawing::TextBlob::MakeFromText(textInfo1.c_str(),
                                                      textInfo1.size(), font1, Drawing::TextEncoding::UTF8);
    if (textBlob1->IsEmoji()) {
        playbackCanvas_->DrawBackground(0xFF0000FF);
    } else {
        playbackCanvas_->DrawBackground(0xFFFF0000);
    }

    //6. 得到需要绘制的所有返回值text,全部适应固定的textBlob构造方式打印
    std::vector<std::string> texts = {typefacestr};

    //7.组合视效接口并绘制
    Drawing::Brush brush;
    brush.SetColor(0xFF4F7091);
    auto filter = Drawing::Filter();
    filter.SetColorFilter(Drawing::ColorFilter::CreateLinearToSrgbGamma());
    brush.SetFilter(filter);
    DrawTextBlob(brush, texts, textBlob, font1, playbackCanvas_);
}

// 用例 Font_Scene_Effect_0086
DEF_DTK(Font_Scene_Effect_3, TestLevel::L2, 86)
{
    // 1.创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    const uint8_t emptyFontData[10] = { 0 };
    auto typefaceDynamic = fontMgr->LoadDynamicFont("EmptyFont", emptyFontData, 10);
    auto typeface = std::shared_ptr<Drawing::Typeface>(typefaceDynamic);
    // 2.组合typeface操作接口
    std::string typefacestr = "GetUnitsPerEm = " + std::to_string(typeface->GetUnitsPerEm());
    // 3.组合Font类接口,如果是操作类有返回值的接口,获取接口返回值加入vector容器
    auto font1 = Drawing::Font(typeface, 50.f, 1.0f, 1.0f);
    font1.SetSubpixel(false);
    std::string text0 = "DDGR ddgr 鸿蒙 !@#￥%^&*; : , 。";
    auto scalar = font1.UnicharToGlyph(0x44);
    std::string text4 = "Glyphs = " + std::to_string(scalar);

    // 4.创建TextBlob
    std::string textInfo = "harmony_os";
    std::shared_ptr<Drawing::TextBlob> textBlob = Drawing::TextBlob::MakeFromString(
        textInfo.c_str(), font1, Drawing::TextEncoding::UTF8);

    // 5.组合textBlob类接口,如果有返回值则获取上一步创建的textBlob返回值打印
    auto rect = textBlob->Bounds();
    playbackCanvas_->DrawRect(*rect);

    //6. 得到需要绘制的所有返回值text,全部适应固定的textBlob构造方式打印
    std::vector<std::string> texts = {typefacestr, text4};

    //7.组合视效接口并绘制
    Drawing::Brush brush;
    brush.SetColor(0xFF4F7091);
    auto filter = Drawing::Filter();
    filter.SetColorFilter(Drawing::ColorFilter::CreateSrgbGammaToLinear());
    brush.SetFilter(filter);
    DrawTextBlob(brush, texts, textBlob, font1, playbackCanvas_);
}

// 用例 Font_Scene_Effect_0087
DEF_DTK(Font_Scene_Effect_3, TestLevel::L2, 87)
{
    // 1.创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    const uint8_t emptyFontData[10] = { 0 };
    auto typefaceDynamic = fontMgr->LoadDynamicFont("EmptyFont", emptyFontData, 10);
    auto typeface = std::shared_ptr<Drawing::Typeface>(typefaceDynamic);
    // 2.组合typeface操作接口
    std::string str = "CPAL";
    reverse(str.begin(), str.end());
    uint32_t tagid = *(reinterpret_cast<uint32_t*>(const_cast<char*>(str.c_str())));
    std::string typefacestr = "GetTableSize = " + std::to_string(typeface->GetTableSize(tagid));
    // 3.组合Font类接口,如果是操作类有返回值的接口,获取接口返回值加入vector容器
    auto font1 = Drawing::Font(typeface, 50.f, 1.0f, 1.0f);
    font1.SetSubpixel(false);
    std::string text0 = "DDGR ddgr 鸿蒙 !@#￥%^&*; : , 。";
    auto SpaceLine = font1.MeasureSingleCharacter(0x44);
    std::string text4 = "Recommended spacing between lines = " + std::to_string(SpaceLine);

    // 4.创建TextBlob
    std::string textInfo = "harmony_os";
    std::shared_ptr<Drawing::TextBlob> textBlob = Drawing::TextBlob::MakeFromString(
        textInfo.c_str(), font1, Drawing::TextEncoding::UTF8);

    // 5.组合textBlob类接口,如果有返回值则获取上一步创建的textBlob返回值打印
    std::string textinfo1 = "Deserialize @Hello World";
    Drawing::TextBlob::Context* Ctx = new (std::nothrow) Drawing::TextBlob::Context(typeface, false);
    auto data2 = textBlob->Serialize(Ctx);
    std::shared_ptr<Drawing::TextBlob> infoTextBlob2 =
        Drawing::TextBlob::Deserialize(data2->GetData(), data2->GetSize(), Ctx);

    //6. 得到需要绘制的所有返回值text,全部适应固定的textBlob构造方式打印
    std::vector<std::string> texts = {typefacestr, text4};

    //7.组合视效接口并绘制
    Drawing::Brush brush;
    brush.SetColor(0xFFFF0000);
    auto colorShader = Drawing::ShaderEffect::CreateColorShader(Drawing::Color::COLOR_RED);
    brush.SetShaderEffect(colorShader);
    DrawTextBlob(brush, texts, textBlob, font1, playbackCanvas_);
}

// 用例 Font_Scene_Effect_0088
DEF_DTK(Font_Scene_Effect_3, TestLevel::L2, 88)
{
    // 1.创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    const uint8_t emptyFontData[10] = { 0 };
    auto typefaceDynamic = fontMgr->LoadDynamicFont("EmptyFont", emptyFontData, 10);
    auto typeface = std::shared_ptr<Drawing::Typeface>(typefaceDynamic);
    // 2.组合typeface操作接口
    auto data = typeface->Serialize();
    uint32_t size = 10;
    typeface->SetSize(size);
    std::shared_ptr<Drawing::Typeface> typeface1 = Drawing::Typeface::Deserialize(data->GetData(), typeface->GetSize());
    // 3.组合Font类接口,如果是操作类有返回值的接口,获取接口返回值加入vector容器
    auto font = Drawing::Font(typeface1, 50.f, 1.0f, 1.0f);
    font.SetSubpixel(false);
    font.SetTypeface(typeface1);
    auto font1 = Drawing::Font(font.GetTypeface(), 50.f, 1.0f, 1.0f);
    font1.SetSubpixel(font.IsSubpixel());

    // 4.创建TextBlob
    std::string textInfo = "1111111111111111111111111111111111";
    int maxGlyphCount = font1.CountText(textInfo.c_str(), textInfo.size(), Drawing::TextEncoding::UTF8);
    Drawing::RSXform xform[maxGlyphCount];
    for (int i = 0; i < maxGlyphCount; ++i) {
        xform[i].cos_ = cos(10 * i) + 0.1 * i;
        xform[i].sin_ = sin(10 * i);
        xform[i].tx_ = 40 * i + 100;
        xform[i].ty_ = 100;
    }
    std::shared_ptr<Drawing::TextBlob> textBlob = Drawing::TextBlob::MakeFromRSXform(
        textInfo.c_str(), textInfo.size(), &xform[0], font1, Drawing::TextEncoding::UTF8);

    // 5.组合textBlob类接口,如果有返回值则获取上一步创建的textBlob返回值打印
    Drawing::Paint paint;
    paint.SetColor(0xFFFF0000);
    float boundsx[] = {1, 2, 3};
    int intercepts = textBlob->GetIntercepts(boundsx, nullptr, &paint);
    std::string text2 = "intercepts = " + std::to_string(intercepts);

    //6. 得到需要绘制的所有返回值text,全部适应固定的textBlob构造方式打印
    std::vector<std::string> texts = {text2};

    //7.组合视效接口并绘制
    Drawing::Brush brush;
    brush.SetColor(0xFFFF0000);
    std::vector<Drawing::BlurType> blurTypes = {Drawing::BlurType::NORMAL, Drawing::BlurType::SOLID,
        Drawing::BlurType::OUTER, Drawing::BlurType::INNER};
    int rectPos = 0;
    for (auto & blurType : blurTypes) {
        auto filter = Drawing::Filter();
        filter.SetMaskFilter(Drawing::MaskFilter::CreateBlurMaskFilter(blurType, 10.0f, true));
        brush.SetFilter(filter);
        DrawTextBlob(brush, texts, textBlob, font1, playbackCanvas_);
        rectPos += 200;
    }
}

// 用例 Font_Scene_Effect_0089
DEF_DTK(Font_Scene_Effect_3, TestLevel::L2, 89)
{
    // 1.创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    const uint8_t emptyFontData[10] = { 0 };
    auto typefaceDynamic = fontMgr->LoadDynamicFont("EmptyFont", emptyFontData, 10);
    auto typeface = std::shared_ptr<Drawing::Typeface>(typefaceDynamic);
    // 2.组合typeface操作接口
    std::string typefacestr = "IsCustomTypeface = " + std::to_string(typeface->IsCustomTypeface());
    // 3.组合Font类接口,如果是操作类有返回值的接口,获取接口返回值加入vector容器
    auto font1 = Drawing::Font(typeface, 50.f, 1.0f, 1.0f);
    font1.SetSubpixel(false);
    std::string text0 = "DDGR ddgr 鸿蒙 !@#￥%^&*; : , 。";
    int glyphCount = font1.CountText(text0.c_str(), text0.length(), Drawing::TextEncoding::UTF16);
    uint16_t glyphs[glyphCount - 1];
    int count = font1.TextToGlyphs(text0.c_str(), text0.length(), Drawing::TextEncoding::UTF16, glyphs, glyphCount + 1);
    std::string text4 = "TextToGlyphs = " + std::to_string(count);

    // 4.创建TextBlob
    std::string textInfo = "1111111111111111111111111111111111";
    int maxGlyphCount = font1.CountText(textInfo.c_str(), textInfo.size(), Drawing::TextEncoding::UTF8);
    Drawing::RSXform xform[maxGlyphCount];
    for (int i = 0; i < maxGlyphCount; ++i) {
        xform[i].cos_ = cos(10 * i) + 0.1 * i;
        xform[i].sin_ = sin(10 * i);
        xform[i].tx_ = 40 * i + 100;
        xform[i].ty_ = 100;
    }
    std::shared_ptr<Drawing::TextBlob> textBlob = Drawing::TextBlob::MakeFromRSXform(
        textInfo.c_str(), textInfo.size(), &xform[0], font1, Drawing::TextEncoding::UTF8);

    // 5.组合textBlob类接口,如果有返回值则获取上一步创建的textBlob返回值打印
    std::vector<Drawing::Point> points;
    Drawing::TextBlob::GetDrawingPointsForTextBlob(textBlob.get(), points);
    std::string text2 = "";
    for (int i = 0; i < points.size(); i++) {
        text2 += std::to_string(i) + "-- X：" + std::to_string(points[i].GetX())
                     + "Y：" + std::to_string(points[i].GetY());
    }

    //6. 得到需要绘制的所有返回值text,全部适应固定的textBlob构造方式打印
    std::vector<std::string> texts = {typefacestr, text2, text4};

    //7.组合视效接口并绘制
    Drawing::Brush brush;
    Drawing::ColorMatrix matrix;
    float arr[] = {
        1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    };
    matrix.SetArray(arr);
    auto cf = Drawing::ColorFilter::CreateMatrixColorFilter(matrix);
    auto filter = Drawing::Filter();
    filter.SetImageFilter(Drawing::ImageFilter::CreateColorFilterImageFilter(*cf, nullptr));
    brush.SetFilter(filter);
    DrawTextBlob(brush, texts, textBlob, font1, playbackCanvas_);
}

// 用例 Font_Scene_Effect_0090
DEF_DTK(Font_Scene_Effect_3, TestLevel::L2, 90)
{
    // 1.创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    const uint8_t emptyFontData[10] = { 0 };
    auto typefaceDynamic = fontMgr->LoadDynamicFont("EmptyFont", emptyFontData, 10);
    auto typeface = std::shared_ptr<Drawing::Typeface>(typefaceDynamic);
    // 2.组合typeface操作接口
    std::string s = "011c50";
    int a;
    std::stringstream ss;
    ss<<std::hex<<s;
    ss>>a;
    uint32_t tagid = *(reinterpret_cast<uint32_t*>(const_cast<char*>(s.c_str())));
    std::string typefacestr = "GetTableData = "
        + std::to_string(typeface->GetTableData(tagid, a, typeface->GetTableSize(tagid), nullptr));
    // 3.组合Font类接口,如果是操作类有返回值的接口,获取接口返回值加入vector容器
    auto font1 = Drawing::Font(typeface, 50.f, 1.0f, 1.0f);
    font1.SetSubpixel(false);
    std::string text0 = "DDGR ddgr 鸿蒙 !@#￥%^&*; : , 。";
    auto glyphsCount = font1.CountText(text0.c_str(), text0.length(), Drawing::TextEncoding::GLYPH_ID);
    std::string text4 = "glyphsCount = " + std::to_string(glyphsCount);

    // 4.创建TextBlob
    std::string textInfo = "1111111111111111111111111111111111";
    int maxGlyphCount = font1.CountText(textInfo.c_str(), textInfo.size(), Drawing::TextEncoding::UTF8);
    Drawing::RSXform xform[maxGlyphCount];
    for (int i = 0; i < maxGlyphCount; ++i) {
        xform[i].cos_ = cos(10 * i) + 0.1 * i;
        xform[i].sin_ = sin(10 * i);
        xform[i].tx_ = 40 * i + 100;
        xform[i].ty_ = 100;
    }
    std::shared_ptr<Drawing::TextBlob> textBlob = Drawing::TextBlob::MakeFromRSXform(
        textInfo.c_str(), textInfo.size(), &xform[0], font1, Drawing::TextEncoding::UTF8);

    // 5.组合textBlob类接口,如果有返回值则获取上一步创建的textBlob返回值打印
    std::string name1 = "HMOS Color Emoji";
    std::string textInfo1 = "😊😂🤣😍😒💕😘😁👍🙌👌";
    std::shared_ptr<Drawing::TextBlob> textBlob1 = Drawing::TextBlob::MakeFromText(textInfo1.c_str(),
                                                      textInfo1.size(), font1, Drawing::TextEncoding::UTF8);
    if (textBlob1->IsEmoji()) {
        playbackCanvas_->DrawBackground(0xFF0000FF);
    } else {
        playbackCanvas_->DrawBackground(0xFFFF0000);
    }

    //6. 得到需要绘制的所有返回值text,全部适应固定的textBlob构造方式打印
    std::vector<std::string> texts = {typefacestr, text4};

    //7.组合视效接口并绘制
    Drawing::Brush brush;
    std::vector<Drawing::ColorQuad> colors =
        {Drawing::Color::COLOR_RED, Drawing::Color::COLOR_GREEN, Drawing::Color::COLOR_BLUE};
    std::vector<Drawing::scalar> pos = {0.00f, 0.50f, 1.00f};
    auto linearGradient =
        Drawing::ShaderEffect::CreateLinearGradient({0, 0}, {1000, 1000}, colors, pos, Drawing::TileMode::CLAMP);
    brush.SetShaderEffect(linearGradient);
    DrawTextBlob(brush, texts, textBlob, font1, playbackCanvas_);
}

// 用例 Font_Scene_Effect_0091
DEF_DTK(Font_Scene_Effect_3, TestLevel::L2, 91)
{
    // 1.创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    int familyCount = fontMgr->CountFamilies();
    std::string name = "HarmonyOS-Sans" + std::to_string(familyCount);
    const uint8_t emptyFontData[10] = { 0 };
    auto typefaceTheme = fontMgr->LoadThemeFont("EmptyFont", name.c_str(), emptyFontData, 10);
    auto typeface = std::shared_ptr<Drawing::Typeface>(typefaceTheme);
    // 2.组合typeface操作接口
    std::string str = "CPAL";
    reverse(str.begin(), str.end());
    uint32_t tagid = *(reinterpret_cast<uint32_t*>(const_cast<char*>(str.c_str())));
    std::string typefacestr = "GetTableSize = " + std::to_string(typeface->GetTableSize(tagid));
    // 3.组合Font类接口,如果是操作类有返回值的接口,获取接口返回值加入vector容器
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 1.0f);
    font.SetSubpixel(false);
    font.SetHinting(Drawing::FontHinting::NORMAL);
    auto font1 = Drawing::Font(typeface, 50.f, 1.0f, 1.0f);
    font1.SetSubpixel(font.IsSubpixel());
    font1.SetHinting(font.GetHinting());

    // 4.创建TextBlob
    std::shared_ptr<Drawing::TextBlob> textBlob = MakeTextBlob(font1);

    // 5.组合textBlob类接口,如果有返回值则获取上一步创建的textBlob返回值打印
    std::vector<Drawing::Point> points;
    Drawing::TextBlob::GetDrawingPointsForTextBlob(textBlob.get(), points);
    std::string text2 = "";
    for (int i = 0; i < points.size(); i++) {
        text2 += std::to_string(i) + "-- X：" + std::to_string(points[i].GetX())
                     + "Y：" + std::to_string(points[i].GetY());
    }

    //6. 得到需要绘制的所有返回值text,全部适应固定的textBlob构造方式打印
    std::vector<std::string> texts = {typefacestr, text2};

    //7.组合视效接口并绘制
    Drawing::Brush brush;
    brush.SetColor(0xFFFF0000);
    auto colorShader = Drawing::ShaderEffect::CreateColorShader(Drawing::Color::COLOR_RED);
    brush.SetShaderEffect(colorShader);
    DrawTextBlob(brush, texts, textBlob, font1, playbackCanvas_);
}

// 用例 Font_Scene_Effect_0092
DEF_DTK(Font_Scene_Effect_3, TestLevel::L2, 92)
{
    // 1.创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    int familyCount = fontMgr->CountFamilies();
    std::string name = "HarmonyOS-Sans" + std::to_string(familyCount);
    const uint8_t emptyFontData[10] = { 0 };
    auto typefaceTheme = fontMgr->LoadThemeFont("EmptyFont", name.c_str(), emptyFontData, 10);
    auto typeface = std::shared_ptr<Drawing::Typeface>(typefaceTheme);
    // 2.组合typeface操作接口
    std::string typefacestr = "GetUniqueID = " + std::to_string(typeface->GetUniqueID());
    // 3.组合Font类接口,如果是操作类有返回值的接口,获取接口返回值加入vector容器
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 1.0f);
    font.SetSubpixel(false);
    font.SetSkewX(1.0f);
    auto font1 = Drawing::Font(typeface, 50.f, font.GetSkewX(), 1.0f);
    font1.SetSubpixel(font.IsSubpixel());

    // 4.创建TextBlob
    std::shared_ptr<Drawing::TextBlob> textBlob = MakeTextBlob(font1);

    // 5.组合textBlob类接口,如果有返回值则获取上一步创建的textBlob返回值打印
    std::string textinfo1 = "Deserialize @Hello World";
    Drawing::TextBlob::Context* Ctx = new (std::nothrow) Drawing::TextBlob::Context(typeface, false);
    auto data2 = textBlob->Serialize(Ctx);
    std::shared_ptr<Drawing::TextBlob> infoTextBlob2 =
        Drawing::TextBlob::Deserialize(data2->GetData(), data2->GetSize(), Ctx);

    //6. 得到需要绘制的所有返回值text,全部适应固定的textBlob构造方式打印
    std::vector<std::string> texts = {typefacestr};

    //7.组合视效接口并绘制
    Drawing::Brush brush;
    brush.SetColor(0xFF0000FF);
    auto filter = Drawing::Filter();
    filter.SetColorFilter(Drawing::ColorFilter::CreateLumaColorFilter());
    brush.SetFilter(filter);
    DrawTextBlob(brush, texts, textBlob, font1, playbackCanvas_);
}

// 用例 Font_Scene_Effect_0093
DEF_DTK(Font_Scene_Effect_3, TestLevel::L2, 93)
{
    // 1.创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    int familyCount = fontMgr->CountFamilies();
    std::string name = "HarmonyOS-Sans" + std::to_string(familyCount);
    const uint8_t emptyFontData[10] = { 0 };
    auto typefaceTheme = fontMgr->LoadThemeFont("EmptyFont", name.c_str(), emptyFontData, 10);
    auto typeface = std::shared_ptr<Drawing::Typeface>(typefaceTheme);
    // 2.组合typeface操作接口
    auto familyNamex = typeface->GetFamilyName();
    std::string typefacestr = "GetFamilyName = " + familyNamex;
    // 3.组合Font类接口,如果是操作类有返回值的接口,获取接口返回值加入vector容器
    auto font1 = Drawing::Font(typeface, 50.f, 1.0f, 1.0f);
    font1.SetSubpixel(false);
    std::string text0 = "DDGR ddgr 鸿蒙 !@#￥%^&*; : , 。";
    Drawing::Rect bounds;
    auto scalar = font1.MeasureText(text0.c_str(), text0.length(), Drawing::TextEncoding::UTF32, &bounds);
    std::string text4 = "MeasureTextWidths = " + std::to_string(scalar);

    // 4.创建TextBlob
    std::shared_ptr<Drawing::TextBlob> textBlob = MakeTextBlob(font1);

    // 5.组合textBlob类接口,如果有返回值则获取上一步创建的textBlob返回值打印
    auto rect = textBlob->Bounds();
    playbackCanvas_->DrawRect(*rect);

    //6. 得到需要绘制的所有返回值text,全部适应固定的textBlob构造方式打印
    std::vector<std::string> texts = {typefacestr, text4};

    //7.组合视效接口并绘制
    Drawing::Brush brush;
    brush.SetColor(0xFF4F7091);
    auto filter = Drawing::Filter();
    filter.SetColorFilter(Drawing::ColorFilter::CreateLinearToSrgbGamma());
    brush.SetFilter(filter);
    DrawTextBlob(brush, texts, textBlob, font1, playbackCanvas_);
}

// 用例 Font_Scene_Effect_0094
DEF_DTK(Font_Scene_Effect_3, TestLevel::L2, 94)
{
    // 1.创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    int familyCount = fontMgr->CountFamilies();
    std::string name = "HarmonyOS-Sans" + std::to_string(familyCount);
    const uint8_t emptyFontData[10] = { 0 };
    auto typefaceTheme = fontMgr->LoadThemeFont("EmptyFont", name.c_str(), emptyFontData, 10);
    auto typeface = std::shared_ptr<Drawing::Typeface>(typefaceTheme);
    // 2.组合typeface操作接口
    std::string s = "011c50";
    int a;
    std::stringstream ss;
    ss<<std::hex<<s;
    ss>>a;
    uint32_t tagid = *(reinterpret_cast<uint32_t*>(const_cast<char*>(s.c_str())));
    std::string typefacestr = "GetTableData = "
        + std::to_string(typeface->GetTableData(tagid, a, typeface->GetTableSize(tagid), nullptr));
    // 3.组合Font类接口,如果是操作类有返回值的接口,获取接口返回值加入vector容器
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 1.0f);
    font.SetSubpixel(false);
    font.SetEmbeddedBitmaps(true);
    auto font1 = Drawing::Font(typeface, 50.f, 1.0f, 1.0f);
    font1.SetSubpixel(font.IsSubpixel());
    font1.SetEmbeddedBitmaps(font.IsEmbeddedBitmaps());

    // 4.创建TextBlob
    std::shared_ptr<Drawing::TextBlob> textBlob = MakeTextBlob2(font1);

    // 5.组合textBlob类接口,如果有返回值则获取上一步创建的textBlob返回值打印
    Drawing::Paint paint;
    paint.SetColor(0xFFFF0000);
    float boundsx[] = {1, 2, 3};
    int intercepts = textBlob->GetIntercepts(boundsx, nullptr, &paint);
    std::string text2 = "intercepts = " + std::to_string(intercepts);

    //6. 得到需要绘制的所有返回值text,全部适应固定的textBlob构造方式打印
    std::vector<std::string> texts = {typefacestr, text2};

    //7.组合视效接口并绘制
    Drawing::Brush brush;
    brush.SetColor(0x4CB21933);
    std::vector<Drawing::BlendMode> blendModes = MakeBlendModes();
    
    int rectPos = 0;
    for (auto blendMode : blendModes) {
        auto background = Drawing::ImageFilter::CreateBlurImageFilter(
            1.0f, 1.0f, Drawing::TileMode::REPEAT, nullptr, Drawing::ImageBlurType::GAUSS);
        auto foreground = Drawing::ImageFilter::CreateBlurImageFilter(
            1.0f, 1.0f, Drawing::TileMode::REPEAT, nullptr, Drawing::ImageBlurType::GAUSS);
        auto filter = Drawing::Filter();
        filter.SetImageFilter(Drawing::ImageFilter::CreateBlendImageFilter(blendMode, background, foreground));
        brush.SetFilter(filter);
        DrawTextBlob(brush, texts, textBlob, font1, playbackCanvas_);
        rectPos += 200;
    }
}

// 用例 Font_Scene_Effect_0095
DEF_DTK(Font_Scene_Effect_3, TestLevel::L2, 95)
{
    // 1.创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    int familyCount = fontMgr->CountFamilies();
    std::string name = "HarmonyOS-Sans" + std::to_string(familyCount);
    const uint8_t emptyFontData[10] = { 0 };
    auto typefaceTheme = fontMgr->LoadThemeFont("EmptyFont", name.c_str(), emptyFontData, 10);
    auto typeface = std::shared_ptr<Drawing::Typeface>(typefaceTheme);
    // 2.组合typeface操作接口
    std::string typefacestr = "GetUnitsPerEm = " + std::to_string(typeface->GetUnitsPerEm());
    // 3.组合Font类接口,如果是操作类有返回值的接口,获取接口返回值加入vector容器
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 1.0f);
    font.SetSubpixel(false);
    font.SetLinearMetrics(false);
    auto font1 = Drawing::Font(typeface, 50.f, 1.0f, 1.0f);
    font1.SetSubpixel(font.IsSubpixel());
    font1.SetLinearMetrics(font.IsLinearMetrics());

    // 4.创建TextBlob
    std::shared_ptr<Drawing::TextBlob> textBlob = MakeTextBlob2(font1);

    // 5.组合textBlob类接口,如果有返回值则获取上一步创建的textBlob返回值打印
    std::string name1 = "HMOS Color Emoji";
    std::string textInfo1 = "😊😂🤣😍😒💕😘😁👍🙌👌";
    std::shared_ptr<Drawing::TextBlob> textBlob1 = Drawing::TextBlob::MakeFromText(textInfo1.c_str(),
                                                      textInfo1.size(), font1, Drawing::TextEncoding::UTF8);
    if (textBlob1->IsEmoji()) {
        playbackCanvas_->DrawBackground(0xFF0000FF);
    } else {
        playbackCanvas_->DrawBackground(0xFFFF0000);
    }

    //6. 得到需要绘制的所有返回值text,全部适应固定的textBlob构造方式打印
    std::vector<std::string> texts = {typefacestr};

    //7.组合视效接口并绘制
    Drawing::Brush brush;
    brush.SetColor(0xFFFF0000);
    std::vector<Drawing::BlurType> blurTypes = {Drawing::BlurType::NORMAL, Drawing::BlurType::SOLID,
        Drawing::BlurType::OUTER, Drawing::BlurType::INNER};
    int rectPos = 0;
    for (auto & blurType : blurTypes) {
        auto filter = Drawing::Filter();
        filter.SetMaskFilter(Drawing::MaskFilter::CreateBlurMaskFilter(blurType, 10.0f, true));
        brush.SetFilter(filter);
        DrawTextBlob(brush, texts, textBlob, font1, playbackCanvas_);
        rectPos += 200;
    }
}

// 用例 Font_Scene_Effect_0096
DEF_DTK(Font_Scene_Effect_3, TestLevel::L2, 96)
{
    // 1.创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    int familyCount = fontMgr->CountFamilies();
    std::string name = "HarmonyOS-Sans" + std::to_string(familyCount);
    const uint8_t emptyFontData[10] = { 0 };
    auto typefaceTheme = fontMgr->LoadThemeFont("EmptyFont", name.c_str(), emptyFontData, 10);
    auto typeface = std::shared_ptr<Drawing::Typeface>(typefaceTheme);
    // 2.组合typeface操作接口
    std::string typefacestr = "IsCustomTypeface = " + std::to_string(typeface->IsCustomTypeface());
    // 3.组合Font类接口,如果是操作类有返回值的接口,获取接口返回值加入vector容器
    auto font1 = Drawing::Font(typeface, 50.f, 1.0f, 1.0f);
    font1.SetSubpixel(false);
    std::string text0 = "DDGR ddgr 鸿蒙 !@#￥%^&*; : , 。";
    auto SpaceLine = font1.MeasureSingleCharacter(0x44);
    std::string text4 = "Recommended spacing between lines = " + std::to_string(SpaceLine);

    // 4.创建TextBlob
    std::shared_ptr<Drawing::TextBlob> textBlob = MakeTextBlob2(font1);

    // 5.组合textBlob类接口,如果有返回值则获取上一步创建的textBlob返回值打印
    std::string textinfo1 = "Deserialize @Hello World";
    Drawing::TextBlob::Context* Ctx = new (std::nothrow) Drawing::TextBlob::Context(typeface, false);
    auto data2 = textBlob->Serialize(Ctx);
    std::shared_ptr<Drawing::TextBlob> infoTextBlob2 =
        Drawing::TextBlob::Deserialize(data2->GetData(), data2->GetSize(), Ctx);

    //6. 得到需要绘制的所有返回值text,全部适应固定的textBlob构造方式打印
    std::vector<std::string> texts = {typefacestr, text4};

    //7.组合视效接口并绘制
    Drawing::Brush brush;
    brush.SetColor(0xFF4F7091);
    auto filter = Drawing::Filter();
    filter.SetColorFilter(Drawing::ColorFilter::CreateSrgbGammaToLinear());
    brush.SetFilter(filter);
    DrawTextBlob(brush, texts, textBlob, font1, playbackCanvas_);
}

// 用例 Font_Scene_Effect_0097
DEF_DTK(Font_Scene_Effect_3, TestLevel::L2, 97)
{
    // 1.创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    int familyCount = fontMgr->CountFamilies();
    std::string name = "HarmonyOS-Sans" + std::to_string(familyCount);
    const uint8_t emptyFontData[10] = { 0 };
    auto typefaceTheme = fontMgr->LoadThemeFont("EmptyFont", name.c_str(), emptyFontData, 10);
    auto typeface = std::shared_ptr<Drawing::Typeface>(typefaceTheme);
    // 2.组合typeface操作接口
    std::string typefacestr = "GetItalic = " + std::to_string(typeface->GetItalic());
    // 3.组合Font类接口,如果是操作类有返回值的接口,获取接口返回值加入vector容器
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 1.0f);
    font.SetSubpixel(false);
    font.SetTypeface(typeface);
    auto font1 = Drawing::Font(font.GetTypeface(), 50.f, 1.0f, 1.0f);
    font1.SetSubpixel(font.IsSubpixel());

    // 4.创建TextBlob
    std::string textInfo = "😊😂🤣😍😒💕😘😁👍🙌👌";
    std::shared_ptr<Drawing::TextBlob> textBlob = Drawing::TextBlob::MakeFromText(
        textInfo.c_str(), textInfo.size(), font1, Drawing::TextEncoding::UTF8);

    // 5.组合textBlob类接口,如果有返回值则获取上一步创建的textBlob返回值打印
    std::string text = "harmony_os";
    std::vector<uint16_t> glyphid;
    Drawing::TextBlob::GetDrawingGlyphIDforTextBlob(textBlob.get(), glyphid);
    std::string text2 = "";
    for (int row = 0; row < text.size() && row < glyphid.size(); row++) {
        text2 += std::string(1, text[row]) + "：" + std::to_string(glyphid[row]);
    }

    //6. 得到需要绘制的所有返回值text,全部适应固定的textBlob构造方式打印
    std::vector<std::string> texts = {typefacestr, text2};

    //7.组合视效接口并绘制
    Drawing::Brush brush;
    brush.SetColor(0xFFFF0000);
    std::shared_ptr<Drawing::ColorFilter> colorFilter1 =
        Drawing::ColorFilter::CreateBlendModeColorFilter(0xFFFF0000, Drawing::BlendMode::SRC_IN);
    std::shared_ptr<Drawing::ColorFilter> colorFilter2 =
        Drawing::ColorFilter::CreateBlendModeColorFilter(0xFFFF00FF, Drawing::BlendMode::MODULATE);
    std::shared_ptr<Drawing::ColorFilter> colorFilter3
        = Drawing::ColorFilter::CreateComposeColorFilter(*colorFilter1, *colorFilter2);
    auto filter = Drawing::Filter();
    filter.SetColorFilter(colorFilter3);
    brush.SetFilter(filter);
    DrawTextBlob(brush, texts, textBlob, font1, playbackCanvas_);
}

// 用例 Font_Scene_Effect_0098
DEF_DTK(Font_Scene_Effect_3, TestLevel::L2, 98)
{
    // 1.创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    int familyCount = fontMgr->CountFamilies();
    std::string name = "HarmonyOS-Sans" + std::to_string(familyCount);
    const uint8_t emptyFontData[10] = { 0 };
    auto typefaceTheme = fontMgr->LoadThemeFont("EmptyFont", name.c_str(), emptyFontData, 10);
    auto typeface = std::shared_ptr<Drawing::Typeface>(typefaceTheme);
    // 2.组合typeface操作接口
    std::string typefacestr = "IsCustomTypeface = " + std::to_string(typeface->IsCustomTypeface());
    // 3.组合Font类接口,如果是操作类有返回值的接口,获取接口返回值加入vector容器
    auto font1 = Drawing::Font(typeface, 50.f, 1.0f, 1.0f);
    font1.SetSubpixel(false);
    std::string text0 = "DDGR ddgr 鸿蒙 !@#￥%^&*; : , 。";
    Drawing::FontMetrics metrics;
    auto SpaceLine = font1.GetMetrics(&metrics);
    std::string text4 = "Recommended spacing between lines = " + std::to_string(SpaceLine);

    // 4.创建TextBlob
    std::string textInfo = "😊😂🤣😍😒💕😘😁👍🙌👌";
    std::shared_ptr<Drawing::TextBlob> textBlob = Drawing::TextBlob::MakeFromText(
        textInfo.c_str(), textInfo.size(), font1, Drawing::TextEncoding::UTF8);

    // 5.组合textBlob类接口,如果有返回值则获取上一步创建的textBlob返回值打印
    Drawing::Paint paint;
    paint.SetColor(0xFFFF0000);
    float boundsx[] = {1, 2, 3};
    int intercepts = textBlob->GetIntercepts(boundsx, nullptr, &paint);
    std::string text2 = "intercepts = " + std::to_string(intercepts);

    //6. 得到需要绘制的所有返回值text,全部适应固定的textBlob构造方式打印
    std::vector<std::string> texts = {typefacestr, text2, text4};

    //7.组合视效接口并绘制
    Drawing::Brush brush;
    brush.SetColor(0xFFFF0000);
    auto filter = Drawing::Filter();
    filter.SetImageFilter(Drawing::ImageFilter::CreateBlurImageFilter(
        10.0f, 10.0f, Drawing::TileMode::CLAMP, nullptr, Drawing::ImageBlurType::GAUSS));
    brush.SetFilter(filter);
    DrawTextBlob(brush, texts, textBlob, font1, playbackCanvas_);
}

// 用例 Font_Scene_Effect_0099
DEF_DTK(Font_Scene_Effect_3, TestLevel::L2, 99)
{
    // 1.创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    int familyCount = fontMgr->CountFamilies();
    std::string name = "HarmonyOS-Sans" + std::to_string(familyCount);
    const uint8_t emptyFontData[10] = { 0 };
    auto typefaceTheme = fontMgr->LoadThemeFont("EmptyFont", name.c_str(), emptyFontData, 10);
    auto typeface = std::shared_ptr<Drawing::Typeface>(typefaceTheme);
    // 2.组合typeface操作接口
    auto data = typeface->Serialize();
    uint32_t size = 10;
    typeface->SetSize(size);
    std::shared_ptr<Drawing::Typeface> typeface1 = Drawing::Typeface::Deserialize(data->GetData(), typeface->GetSize());
    // 3.组合Font类接口,如果是操作类有返回值的接口,获取接口返回值加入vector容器
    auto font1 = Drawing::Font(typeface1, 50.f, 1.0f, 1.0f);
    font1.SetSubpixel(false);
    std::string text0 = "DDGR ddgr 鸿蒙 !@#￥%^&*; : , 。";
    auto glyphsCount = font1.CountText(text0.c_str(), text0.length(), Drawing::TextEncoding::GLYPH_ID);
    std::string text4 = "glyphsCount = " + std::to_string(glyphsCount);

    // 4.创建TextBlob
    std::string textInfo = "😊😂🤣😍😒💕😘😁👍🙌👌";
    std::shared_ptr<Drawing::TextBlob> textBlob = Drawing::TextBlob::MakeFromText(
        textInfo.c_str(), textInfo.size(), font1, Drawing::TextEncoding::UTF8);

    // 5.组合textBlob类接口,如果有返回值则获取上一步创建的textBlob返回值打印
    std::string name1 = "HMOS Color Emoji";
    std::string textInfo1 = "😊😂🤣😍😒💕😘😁👍🙌👌";
    std::shared_ptr<Drawing::TextBlob> textBlob1 = Drawing::TextBlob::MakeFromText(textInfo1.c_str(),
                                                      textInfo1.size(), font1, Drawing::TextEncoding::UTF8);
    if (textBlob1->IsEmoji()) {
        playbackCanvas_->DrawBackground(0xFF0000FF);
    } else {
        playbackCanvas_->DrawBackground(0xFFFF0000);
    }

    //6. 得到需要绘制的所有返回值text,全部适应固定的textBlob构造方式打印
    std::vector<std::string> texts = {text4};

    //7.组合视效接口并绘制
}

// 用例 Font_Scene_Effect_0100
DEF_DTK(Font_Scene_Effect_3, TestLevel::L2, 100)
{
    // 1.创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    int familyCount = fontMgr->CountFamilies();
    std::string name = "HarmonyOS-Sans" + std::to_string(familyCount);
    const uint8_t emptyFontData[10] = { 0 };
    auto typefaceTheme = fontMgr->LoadThemeFont("EmptyFont", name.c_str(), emptyFontData, 10);
    auto typeface = std::shared_ptr<Drawing::Typeface>(typefaceTheme);
    // 2.组合typeface操作接口
    auto familyNamex = typeface->GetFamilyName();
    std::string typefacestr = "GetFamilyName = " + familyNamex;
    // 3.组合Font类接口,如果是操作类有返回值的接口,获取接口返回值加入vector容器
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 1.0f);
    font.SetSubpixel(false);
    font.SetEdging(Drawing::FontEdging::ANTI_ALIAS);
    auto font1 = Drawing::Font(typeface, 50.f, 1.0f, 1.0f);
    font1.SetSubpixel(font.IsSubpixel());
    font1.SetEdging(font.GetEdging());

    // 4.创建TextBlob
    std::string textInfo = "harmony_os";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(-100 + 50 * i);
        p[i].SetY(1000 - 50 * i);
    }
    std::shared_ptr<Drawing::TextBlob> textBlob = Drawing::TextBlob::MakeFromPosText(
        textInfo.c_str(), 10, p, font1, Drawing::TextEncoding::UTF8);

    // 5.组合textBlob类接口,如果有返回值则获取上一步创建的textBlob返回值打印
    Drawing::Paint paint;
    paint.SetColor(0xFFFF0000);
    float boundsx[] = {1, 2, 3};
    int intercepts = textBlob->GetIntercepts(boundsx, nullptr, &paint);
    std::string text2 = "intercepts = " + std::to_string(intercepts);

    //6. 得到需要绘制的所有返回值text,全部适应固定的textBlob构造方式打印
    std::vector<std::string> texts = {typefacestr, text2};

    //7.组合视效接口并绘制
    Drawing::Brush brush;
    brush.SetColor(0xFFFF0000);
    auto filter = Drawing::Filter();
    filter.SetImageFilter(Drawing::ImageFilter::CreateBlurImageFilter(
        10.0f, 10.0f, Drawing::TileMode::CLAMP, nullptr, Drawing::ImageBlurType::GAUSS));
    brush.SetFilter(filter);
    DrawTextBlob(brush, texts, textBlob, font1, playbackCanvas_);
}

// 用例 Font_Scene_Effect_0101
DEF_DTK(Font_Scene_Effect_3, TestLevel::L2, 101)
{
    // 1.创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    int familyCount = fontMgr->CountFamilies();
    std::string name = "HarmonyOS-Sans" + std::to_string(familyCount);
    const uint8_t emptyFontData[10] = { 0 };
    auto typefaceTheme = fontMgr->LoadThemeFont("EmptyFont", name.c_str(), emptyFontData, 10);
    auto typeface = std::shared_ptr<Drawing::Typeface>(typefaceTheme);
    // 2.组合typeface操作接口
    std::string typefacestr = "GetUniqueID = " + std::to_string(typeface->GetUniqueID());
    // 3.组合Font类接口,如果是操作类有返回值的接口,获取接口返回值加入vector容器
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 1.0f);
    font.SetSubpixel(false);
    font.SetSize(100.f);
    auto font1 = Drawing::Font(typeface, font.GetSize(), 1.0f, 1.0f);
    font1.SetSubpixel(font.IsSubpixel());

    // 4.创建TextBlob
    std::string textInfo = "harmony_os";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(-100 + 50 * i);
        p[i].SetY(1000 - 50 * i);
    }
    std::shared_ptr<Drawing::TextBlob> textBlob = Drawing::TextBlob::MakeFromPosText(
        textInfo.c_str(), 10, p, font1, Drawing::TextEncoding::UTF8);

    // 5.组合textBlob类接口,如果有返回值则获取上一步创建的textBlob返回值打印
    std::string text = "harmony_os";
    std::vector<uint16_t> glyphid;
    Drawing::TextBlob::GetDrawingGlyphIDforTextBlob(textBlob.get(), glyphid);
    playbackCanvas_->Translate(200, 200);
    for (int row = 0; row < text.size() && row < glyphid.size(); row++) {
        auto path = Drawing::TextBlob::GetDrawingPathforTextBlob(glyphid[row], textBlob.get());
        playbackCanvas_->DrawPath(path);
        playbackCanvas_->Translate(0, 100);
    }

    //6. 得到需要绘制的所有返回值text,全部适应固定的textBlob构造方式打印
    std::vector<std::string> texts = {typefacestr};

    //7.组合视效接口并绘制
    Drawing::Brush brush;
    Drawing::ColorMatrix matrix;
    float arr[] = {
        1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    };
    matrix.SetArray(arr);
    auto cf = Drawing::ColorFilter::CreateMatrixColorFilter(matrix);
    auto filter = Drawing::Filter();
    filter.SetImageFilter(Drawing::ImageFilter::CreateColorFilterImageFilter(*cf, nullptr));
    brush.SetFilter(filter);
    DrawTextBlob(brush, texts, textBlob, font1, playbackCanvas_);
}

// 用例 Font_Scene_Effect_0102
DEF_DTK(Font_Scene_Effect_3, TestLevel::L2, 102)
{
    // 1.创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    int familyCount = fontMgr->CountFamilies();
    std::string name = "HarmonyOS-Sans" + std::to_string(familyCount);
    const uint8_t emptyFontData[10] = { 0 };
    auto typefaceTheme = fontMgr->LoadThemeFont("EmptyFont", name.c_str(), emptyFontData, 10);
    auto typeface = std::shared_ptr<Drawing::Typeface>(typefaceTheme);
    // 2.组合typeface操作接口
    typeface->SetHash(100);
    std::string  typefacestr = "GetHash = " + std::to_string(typeface->GetHash());
    // 3.组合Font类接口,如果是操作类有返回值的接口,获取接口返回值加入vector容器
    auto font1 = Drawing::Font(typeface, 50.f, 1.0f, 1.0f);
    font1.SetSubpixel(false);
    std::string text0 = "DDGR ddgr 鸿蒙 !@#￥%^&*; : , 。";
    int glyphCount = font1.CountText(text0.c_str(), text0.length(), Drawing::TextEncoding::UTF8);
    float widths[glyphCount];
    uint16_t glyphs[glyphCount];
    font1.TextToGlyphs(text0.c_str(), text0.length(), Drawing::TextEncoding::UTF8, glyphs, glyphCount);
    font1.GetWidths(glyphs, glyphCount, widths);
    std::string text4 = "Getwidths = " + std::to_string(widths[0]);

    // 4.创建TextBlob
    std::string textInfo = "harmony_os";
    int cont = textInfo.size();
    Drawing::Point p[cont];
    for (int i = 0; i < cont; i++) {
        p[i].SetX(-100 + 50 * i);
        p[i].SetY(1000 - 50 * i);
    }
    std::shared_ptr<Drawing::TextBlob> textBlob = Drawing::TextBlob::MakeFromPosText(
        textInfo.c_str(), 10, p, font1, Drawing::TextEncoding::UTF8);

    // 5.组合textBlob类接口,如果有返回值则获取上一步创建的textBlob返回值打印
    std::string text = "harmony_os";
    std::vector<uint16_t> glyphid;
    Drawing::TextBlob::GetDrawingGlyphIDforTextBlob(textBlob.get(), glyphid);
    std::string text2 = "";
    for (int row = 0; row < text.size() && row < glyphid.size(); row++) {
        text2 += std::string(1, text[row]) + "：" + std::to_string(glyphid[row]);
    }

    //6. 得到需要绘制的所有返回值text,全部适应固定的textBlob构造方式打印
    std::vector<std::string> texts = {typefacestr, text2, text4};

    //7.组合视效接口并绘制
    Drawing::Brush brush;
    brush.SetColor(0xFFFF0000);
    std::shared_ptr<Drawing::ColorFilter> colorFilter1 =
        Drawing::ColorFilter::CreateBlendModeColorFilter(0xFFFF0000, Drawing::BlendMode::SRC_IN);
    std::shared_ptr<Drawing::ColorFilter> colorFilter2 =
        Drawing::ColorFilter::CreateBlendModeColorFilter(0xFFFF00FF, Drawing::BlendMode::MODULATE);
    std::shared_ptr<Drawing::ColorFilter> colorFilter3
        = Drawing::ColorFilter::CreateComposeColorFilter(*colorFilter1, *colorFilter2);
    auto filter = Drawing::Filter();
    filter.SetColorFilter(colorFilter3);
    brush.SetFilter(filter);
    DrawTextBlob(brush, texts, textBlob, font1, playbackCanvas_);
}

// 用例 Font_Scene_Effect_0103
DEF_DTK(Font_Scene_Effect_3, TestLevel::L2, 103)
{
    // 1.创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    int familyCount = fontMgr->CountFamilies();
    std::string name = "HarmonyOS-Sans" + std::to_string(familyCount);
    const uint8_t emptyFontData[10] = { 0 };
    auto typefaceTheme = fontMgr->LoadThemeFont("EmptyFont", name.c_str(), emptyFontData, 10);
    auto typeface = std::shared_ptr<Drawing::Typeface>(typefaceTheme);
    // 2.组合typeface操作接口
    typeface->SetHash(100);
    std::string  typefacestr = "GetHash = " + std::to_string(typeface->GetHash());
    // 3.组合Font类接口,如果是操作类有返回值的接口,获取接口返回值加入vector容器
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 1.0f);
    font.SetSubpixel(false);
    font.SetBaselineSnap(true);
    auto font1 = Drawing::Font(typeface, 50.f, 1.0f, 1.0f);
    font1.SetSubpixel(font.IsSubpixel());
    font1.SetBaselineSnap(font.IsBaselineSnap());

    // 4.创建TextBlob
    std::string textInfo = "harmony_os";
    std::shared_ptr<Drawing::TextBlob> textBlob = Drawing::TextBlob::MakeFromString(
        textInfo.c_str(), font1, Drawing::TextEncoding::UTF8);

    // 5.组合textBlob类接口,如果有返回值则获取上一步创建的textBlob返回值打印
    std::string text = "harmony_os";
    std::vector<uint16_t> glyphid;
    Drawing::TextBlob::GetDrawingGlyphIDforTextBlob(textBlob.get(), glyphid);
    std::string text2 = "";
    for (int row = 0; row < text.size() && row < glyphid.size(); row++) {
        text2 += std::string(1, text[row]) + "：" + std::to_string(glyphid[row]);
    }

    //6. 得到需要绘制的所有返回值text,全部适应固定的textBlob构造方式打印
    std::vector<std::string> texts = {typefacestr, text2};

    //7.组合视效接口并绘制
    Drawing::Brush brush;
    brush.SetColor(0xFFFF0000);
    std::vector<Drawing::BlendMode> blendModes = MakeBlendModes();
    
    int rectPos = 0;

    auto dst = Drawing::ShaderEffect::CreateColorShader(Drawing::Color::COLOR_RED);
    auto src = Drawing::ShaderEffect::CreateColorShader(Drawing::Color::COLOR_GREEN);
    for (auto blendMode : blendModes) {
        auto blendShader = Drawing::ShaderEffect::CreateBlendShader(*dst, *src, blendMode);
        brush.SetShaderEffect(blendShader);
        DrawTextBlob(brush, texts, textBlob, font1, playbackCanvas_);
        rectPos += 200;
    }
}

}
}
