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

static std::shared_ptr<Drawing::ImageFilter> CreateImageFilter()
{
    return Drawing::ImageFilter::CreateBlurImageFilter(
        1.0f, 1.0f, Drawing::TileMode::REPEAT, nullptr, Drawing::ImageBlurType::GAUSS);
}

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

static void SettingxForm(Drawing::RSXform xform[], int maxGlyphCount)
{
    const int r = 10;
    const int t = 100;
    const int offset = 40;
    const float f = 0.1;
    for (int i = 0; i < maxGlyphCount; ++i) {
        xform[i].cos_ = cos(r * i) + f * i;
        xform[i].sin_ = sin(r * i);
        xform[i].tx_ = offset * i + t;
        xform[i].ty_ = t;
    }
}

// 用例 Font_Scene_Effect_0319
DEF_DTK(Font_Scene_Effect_20, TestLevel::L2, 319)
{
    // 1.创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->MatchStyle({
        Drawing::FontStyle::INVISIBLE_WEIGHT,
        Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT}));
    // 2.组合typeface操作接口
    std::string typefacestr = "GetItalic = " + std::to_string(typeface->GetItalic());
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

// 用例 Font_Scene_Effect_0320
DEF_DTK(Font_Scene_Effect_20, TestLevel::L2, 320)
{
    // 1.创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->MatchStyle({
        Drawing::FontStyle::INVISIBLE_WEIGHT,
        Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT}));
    // 2.组合typeface操作接口
    auto familyNamex = typeface->GetFamilyName();
    std::string typefacestr = "GetFamilyName = " + familyNamex;
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

// 用例 Font_Scene_Effect_0321
DEF_DTK(Font_Scene_Effect_20, TestLevel::L2, 321)
{
    // 1.创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->MatchStyle({
        Drawing::FontStyle::INVISIBLE_WEIGHT,
        Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT}));
    // 2.组合typeface操作接口
    typeface->SetHash(100);
    std::string  typefacestr = "GetHash = " + std::to_string(typeface->GetHash());
    // 3.组合Font类接口,如果是操作类有返回值的接口,获取接口返回值加入vector容器
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 1.0f);
    font.SetSubpixel(false);
    font.SetLinearMetrics(false);
    auto font1 = Drawing::Font(typeface, 50.f, 1.0f, 1.0f);
    font1.SetSubpixel(font.IsSubpixel());
    font1.SetLinearMetrics(font.IsLinearMetrics());

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

// 用例 Font_Scene_Effect_0322
DEF_DTK(Font_Scene_Effect_20, TestLevel::L2, 322)
{
    // 1.创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->MatchStyle({
        Drawing::FontStyle::INVISIBLE_WEIGHT,
        Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT}));
    // 2.组合typeface操作接口
    std::string typefacestr = "GetUniqueID = " + std::to_string(typeface->GetUniqueID());
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
    Drawing::Paint paint;
    paint.SetColor(0xFFFF0000);
    float boundsx[] = {1, 2, 3};
    int intercepts = textBlob->GetIntercepts(boundsx, nullptr, &paint);
    std::string text2 = "intercepts = " + std::to_string(intercepts);

    //6. 得到需要绘制的所有返回值text,全部适应固定的textBlob构造方式打印
    std::vector<std::string> texts = {typefacestr, text2, text4};

    //7.组合视效接口并绘制
}

// 用例 Font_Scene_Effect_0323
DEF_DTK(Font_Scene_Effect_20, TestLevel::L2, 323)
{
    // 1.创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->MatchStyle({
        Drawing::FontStyle::INVISIBLE_WEIGHT,
        Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT}));
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
    font.SetTypeface(typeface);
    auto font1 = Drawing::Font(font.GetTypeface(), 50.f, 1.0f, 1.0f);
    font1.SetSubpixel(font.IsSubpixel());

    // 4.创建TextBlob
    std::string textInfo = "1111111111111111111111111111111111";
    int maxGlyphCount = font1.CountText(textInfo.c_str(), textInfo.size(), Drawing::TextEncoding::UTF8);
    Drawing::RSXform xform[maxGlyphCount];
    SettingxForm(xform, maxGlyphCount);
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
    std::vector<std::string> texts = {typefacestr};

    //7.组合视效接口并绘制
    Drawing::Brush brush;
    brush.SetColor(0x4CB21933);
    std::vector<Drawing::BlendMode> blendModes = MakeBlendModes();
    
    int rectPos = 0;
    for (auto blendMode : blendModes) {
        auto background = CreateImageFilter();
        auto foreground = CreateImageFilter();
        auto filter = Drawing::Filter();
        filter.SetImageFilter(Drawing::ImageFilter::CreateBlendImageFilter(blendMode, background, foreground));
        brush.SetFilter(filter);
        DrawTextBlob(brush, texts, textBlob, font1, playbackCanvas_);
        rectPos += 200;
    }
}

// 用例 Font_Scene_Effect_0324
DEF_DTK(Font_Scene_Effect_20, TestLevel::L2, 324)
{
    // 1.创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->MatchStyle({
        Drawing::FontStyle::INVISIBLE_WEIGHT,
        Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT}));
    // 2.组合typeface操作接口
    auto data = typeface->Serialize();
    uint32_t size = 10;
    typeface->SetSize(size);
    std::shared_ptr<Drawing::Typeface> typeface1 = Drawing::Typeface::Deserialize(data->GetData(), typeface->GetSize());
    // 3.组合Font类接口,如果是操作类有返回值的接口,获取接口返回值加入vector容器
    auto font1 = Drawing::Font(typeface1, 50.f, 1.0f, 1.0f);
    font1.SetSubpixel(false);
    std::string text0 = "DDGR ddgr 鸿蒙 !@#￥%^&*; : , 。";
    Drawing::FontMetrics metrics;
    auto SpaceLine = font1.GetMetrics(&metrics);
    std::string text4 = "Recommended spacing between lines = " + std::to_string(SpaceLine);

    // 4.创建TextBlob
    std::string textInfo = "1111111111111111111111111111111111";
    int maxGlyphCount = font1.CountText(textInfo.c_str(), textInfo.size(), Drawing::TextEncoding::UTF8);
    Drawing::RSXform xform[maxGlyphCount];
    SettingxForm(xform, maxGlyphCount);
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
    std::vector<std::string> texts = {text4};

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

// 用例 Font_Scene_Effect_0325
DEF_DTK(Font_Scene_Effect_20, TestLevel::L2, 325)
{
    // 1.创建typeface
    std::shared_ptr<Drawing::FontMgr> fontMgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(fontMgr->CreateStyleSet(0));
    auto typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->MatchStyle({
        Drawing::FontStyle::INVISIBLE_WEIGHT,
        Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
        Drawing::FontStyle::OBLIQUE_SLANT}));
    // 2.组合typeface操作接口
    std::string typefacestr = "GetUnitsPerEm = " + std::to_string(typeface->GetUnitsPerEm());
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
    SettingxForm(xform, maxGlyphCount);
    std::shared_ptr<Drawing::TextBlob> textBlob = Drawing::TextBlob::MakeFromRSXform(
        textInfo.c_str(), textInfo.size(), &xform[0], font1, Drawing::TextEncoding::UTF8);

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
    std::vector<Drawing::ColorQuad> colors =
        {Drawing::Color::COLOR_RED, Drawing::Color::COLOR_GREEN, Drawing::Color::COLOR_BLUE};
    std::vector<Drawing::scalar> pos = {0.00f, 0.50f, 1.00f};
    auto linearGradient =
        Drawing::ShaderEffect::CreateLinearGradient({0, 0}, {1000, 1000}, colors, pos, Drawing::TileMode::CLAMP);
    brush.SetShaderEffect(linearGradient);
    DrawTextBlob(brush, texts, textBlob, font1, playbackCanvas_);
}

}
}
