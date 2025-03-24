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
#include "text/font.h"
#include "text/font_mgr.h"
#include "text/font_style_set.h"
#include "text/text_blob_builder.h"
#include "text/typeface.h"

/*
测试类：textblobbuilder
测试接口：allocrunpos
测试内容：
1、对接口入参字体样式、需生成textblob字符个数、字体样式生效范围进行参数组合创建buffer。
2、对buffer里每个元素设置glyphsID并进行排版。
3、通过make接口创建textblob，并调用drawtextblob接口将内容绘制在画布上。
*/

namespace OHOS {
namespace Rosen {

static void DrawText(Drawing::TextBlobBuilder& builder, TestPlaybackCanvas* playbackCanvas)
{
    Drawing::Brush brush;
    Drawing::Pen pen;

    int line2 = 200;
    int line5 = 500;
    int interval2 = 200;

    playbackCanvas->AttachBrush(brush);
    std::shared_ptr<Drawing::TextBlob> infoTextBlob1 = builder.Make();
    playbackCanvas->DrawTextBlob(infoTextBlob1.get(), interval2, line2);
    playbackCanvas->DetachBrush();

    playbackCanvas->AttachPen(pen);
    playbackCanvas->DrawTextBlob(infoTextBlob1.get(), interval2, line5);
    playbackCanvas->DetachPen();
}

static Drawing::Font MakeFont()
{
    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::string name = "HarmonyOS Sans SC";
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(font_mgr->MatchFamily(name.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));
    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(100.f);

    return font;
}

//对应用例allocrunpos_3001
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 1)
{
    auto font = MakeFont();
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(100, 100, 1000, 1500);
    auto buffer = builder.AllocRunPos(font, 20, &rect1);
    for (int i = 0; i < 20; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 50.f*i;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3002
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 2)
{
    auto font = MakeFont();
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(100, 100, 1000, 1500);
    auto buffer = builder.AllocRunPos(font, 20, &rect1);
    for (int i = 0; i < 20; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 0;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3003
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 3)
{
    auto font = MakeFont();
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(100, 100, 1000, 1500);
    auto buffer = builder.AllocRunPos(font, 20, &rect1);
    for (int i = 0; i < 20; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 50.f*i;
        buffer.pos[i * 2 + 1] = 0;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3004
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 4)
{
    auto font = MakeFont();
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(100, 100, 1000, 1500);
    auto buffer = builder.AllocRunPos(font, 20, &rect1);
    for (int i = 0; i < 20; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 1000.0f-50.f*i;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3005
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 5)
{
    auto font = MakeFont();
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(-100, -100, 1000, 1500);
    auto buffer = builder.AllocRunPos(font, 20, &rect1);
    for (int i = 0; i < 20; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 50.f*i;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3006
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 6)
{
    auto font = MakeFont();
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(-100, -100, 1000, 1500);
    auto buffer = builder.AllocRunPos(font, 20, &rect1);
    for (int i = 0; i < 20; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 0;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3007
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 7)
{
    auto font = MakeFont();
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(-100, -100, 1000, 1500);
    auto buffer = builder.AllocRunPos(font, 20, &rect1);
    for (int i = 0; i < 20; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 50.f*i;
        buffer.pos[i * 2 + 1] = 0;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3008
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 8)
{
    auto font = MakeFont();
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(-100, -100, 1000, 1500);
    auto buffer = builder.AllocRunPos(font, 20, &rect1);
    for (int i = 0; i < 20; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 1000.0f-50.f*i;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3009
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 9)
{
    auto font = MakeFont();
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(300, 300, 320, 500);
    auto buffer = builder.AllocRunPos(font, 20, &rect1);
    for (int i = 0; i < 20; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 50.f*i;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3010
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 10)
{
    auto font = MakeFont();
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(300, 300, 320, 500);
    auto buffer = builder.AllocRunPos(font, 20, &rect1);
    for (int i = 0; i < 20; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 0;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3011
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 11)
{
    auto font = MakeFont();
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(300, 300, 320, 500);
    auto buffer = builder.AllocRunPos(font, 20, &rect1);
    for (int i = 0; i < 20; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 50.f*i;
        buffer.pos[i * 2 + 1] = 0;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3012
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 12)
{
    auto font = MakeFont();
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(300, 300, 320, 500);
    auto buffer = builder.AllocRunPos(font, 20, &rect1);
    for (int i = 0; i < 20; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 1000.0f-50.f*i;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3013
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 13)
{
    auto font = MakeFont();
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(300, 300, 100, 800);
    auto buffer = builder.AllocRunPos(font, 20, &rect1);
    for (int i = 0; i < 20; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 50.f*i;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3014
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 14)
{
    auto font = MakeFont();
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(300, 300, 100, 800);
    auto buffer = builder.AllocRunPos(font, 20, &rect1);
    for (int i = 0; i < 20; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 0;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3015
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 15)
{
    auto font = MakeFont();
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(300, 300, 100, 800);
    auto buffer = builder.AllocRunPos(font, 20, &rect1);
    for (int i = 0; i < 20; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 50.f*i;
        buffer.pos[i * 2 + 1] = 0;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3016
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 16)
{
    auto font = MakeFont();
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(300, 300, 100, 800);
    auto buffer = builder.AllocRunPos(font, 20, &rect1);
    for (int i = 0; i < 20; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 1000.0f-50.f*i;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3017
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 17)
{
    auto font = MakeFont();
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1;
    auto buffer = builder.AllocRunPos(font, 20, &rect1);
    for (int i = 0; i < 20; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 50.f*i;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3018
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 18)
{
    auto font = MakeFont();
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1;
    auto buffer = builder.AllocRunPos(font, 20, &rect1);
    for (int i = 0; i < 20; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 0;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3019
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 19)
{
    auto font = MakeFont();
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1;
    auto buffer = builder.AllocRunPos(font, 20, &rect1);
    for (int i = 0; i < 20; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 50.f*i;
        buffer.pos[i * 2 + 1] = 0;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3020
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 20)
{
    auto font = MakeFont();
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1;
    auto buffer = builder.AllocRunPos(font, 20, &rect1);
    for (int i = 0; i < 20; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 1000.0f-50.f*i;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3021
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 21)
{
    auto font = MakeFont();
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(100, 100, 1000, 1500);
    auto buffer = builder.AllocRunPos(font, 10, &rect1);
    for (int i = 0; i < 10; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 50.f*i;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3022
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 22)
{
    auto font = MakeFont();
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(100, 100, 1000, 1500);
    auto buffer = builder.AllocRunPos(font, 10, &rect1);
    for (int i = 0; i < 10; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 0;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3023
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 23)
{
    auto font = MakeFont();
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(100, 100, 1000, 1500);
    auto buffer = builder.AllocRunPos(font, 10, &rect1);
    for (int i = 0; i < 10; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 50.f*i;
        buffer.pos[i * 2 + 1] = 0;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3024
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 24)
{
    auto font = MakeFont();
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(100, 100, 1000, 1500);
    auto buffer = builder.AllocRunPos(font, 10, &rect1);
    for (int i = 0; i < 10; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 1000.0f-50.f*i;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3025
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 25)
{
    auto font = MakeFont();
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(-100, -100, 1000, 1500);
    auto buffer = builder.AllocRunPos(font, 10, &rect1);
    for (int i = 0; i < 10; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 50.f*i;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3026
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 26)
{
    auto font = MakeFont();
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(-100, -100, 1000, 1500);
    auto buffer = builder.AllocRunPos(font, 10, &rect1);
    for (int i = 0; i < 10; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 0;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3027
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 27)
{
    auto font = MakeFont();
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(-100, -100, 1000, 1500);
    auto buffer = builder.AllocRunPos(font, 10, &rect1);
    for (int i = 0; i < 10; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 50.f*i;
        buffer.pos[i * 2 + 1] = 0;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3028
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 28)
{
    auto font = MakeFont();
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(-100, -100, 1000, 1500);
    auto buffer = builder.AllocRunPos(font, 10, &rect1);
    for (int i = 0; i < 10; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 1000.0f-50.f*i;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3029
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 29)
{
    auto font = MakeFont();
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(300, 300, 320, 500);
    auto buffer = builder.AllocRunPos(font, 10, &rect1);
    for (int i = 0; i < 10; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 50.f*i;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3030
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 30)
{
    auto font = MakeFont();
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(300, 300, 320, 500);
    auto buffer = builder.AllocRunPos(font, 10, &rect1);
    for (int i = 0; i < 10; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 0;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3031
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 31)
{
    auto font = MakeFont();
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(300, 300, 320, 500);
    auto buffer = builder.AllocRunPos(font, 10, &rect1);
    for (int i = 0; i < 10; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 50.f*i;
        buffer.pos[i * 2 + 1] = 0;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3032
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 32)
{
    auto font = MakeFont();
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(300, 300, 320, 500);
    auto buffer = builder.AllocRunPos(font, 10, &rect1);
    for (int i = 0; i < 10; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 1000.0f-50.f*i;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3033
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 33)
{
    auto font = MakeFont();
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(300, 300, 100, 800);
    auto buffer = builder.AllocRunPos(font, 10, &rect1);
    for (int i = 0; i < 10; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 50.f*i;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3034
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 34)
{
    auto font = MakeFont();
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(300, 300, 100, 800);
    auto buffer = builder.AllocRunPos(font, 10, &rect1);
    for (int i = 0; i < 10; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 0;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3035
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 35)
{
    auto font = MakeFont();
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(300, 300, 100, 800);
    auto buffer = builder.AllocRunPos(font, 10, &rect1);
    for (int i = 0; i < 10; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 50.f*i;
        buffer.pos[i * 2 + 1] = 0;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3036
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 36)
{
    auto font = MakeFont();
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1(300, 300, 100, 800);
    auto buffer = builder.AllocRunPos(font, 10, &rect1);
    for (int i = 0; i < 10; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 1000.0f-50.f*i;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3037
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 37)
{
    auto font = MakeFont();
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1;
    auto buffer = builder.AllocRunPos(font, 10, &rect1);
    for (int i = 0; i < 10; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 50.f*i;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3038
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 38)
{
    auto font = MakeFont();
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1;
    auto buffer = builder.AllocRunPos(font, 10, &rect1);
    for (int i = 0; i < 10; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 0;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3039
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 39)
{
    auto font = MakeFont();
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1;
    auto buffer = builder.AllocRunPos(font, 10, &rect1);
    for (int i = 0; i < 10; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 50.f*i;
        buffer.pos[i * 2 + 1] = 0;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3040
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 40)
{
    auto font = MakeFont();
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1;
    auto buffer = builder.AllocRunPos(font, 10, &rect1);
    for (int i = 0; i < 10; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 1000.0f-50.f*i;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3041
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 41)
{
    auto font = MakeFont();
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
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 42)
{
    auto font = MakeFont();
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
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 43)
{
    auto font = MakeFont();
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
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 44)
{
    auto font = MakeFont();
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
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 45)
{
    auto font = MakeFont();
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
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 46)
{
    auto font = MakeFont();
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
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 47)
{
    auto font = MakeFont();
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
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 48)
{
    auto font = MakeFont();
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
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 49)
{
    auto font = MakeFont();
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
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 50)
{
    auto font = MakeFont();
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
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 51)
{
    auto font = MakeFont();
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
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 52)
{
    auto font = MakeFont();
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
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 53)
{
    auto font = MakeFont();
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
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 54)
{
    auto font = MakeFont();
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
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 55)
{
    auto font = MakeFont();
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
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 56)
{
    auto font = MakeFont();
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
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 57)
{
    auto font = MakeFont();
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1;
    auto buffer = builder.AllocRunPos(font, 0, &rect1);
    for (int i = 0; i < 0; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 50.f*i;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3058
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 58)
{
    auto font = MakeFont();
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1;
    auto buffer = builder.AllocRunPos(font, 0, &rect1);
    for (int i = 0; i < 0; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 0;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3059
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 59)
{
    auto font = MakeFont();
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1;
    auto buffer = builder.AllocRunPos(font, 0, &rect1);
    for (int i = 0; i < 0; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 50.f*i;
        buffer.pos[i * 2 + 1] = 0;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3060
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 60)
{
    auto font = MakeFont();
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1;
    auto buffer = builder.AllocRunPos(font, 0, &rect1);
    for (int i = 0; i < 0; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 1000.0f-50.f*i;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3061
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 61)
{
    auto font = MakeFont();
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
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 62)
{
    auto font = MakeFont();
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
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 63)
{
    auto font = MakeFont();
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
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 64)
{
    auto font = MakeFont();
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
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 65)
{
    auto font = MakeFont();
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
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 66)
{
    auto font = MakeFont();
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
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 67)
{
    auto font = MakeFont();
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
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 68)
{
    auto font = MakeFont();
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
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 69)
{
    auto font = MakeFont();
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
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 70)
{
    auto font = MakeFont();
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
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 71)
{
    auto font = MakeFont();
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
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 72)
{
    auto font = MakeFont();
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
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 73)
{
    auto font = MakeFont();
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
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 74)
{
    auto font = MakeFont();
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
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 75)
{
    auto font = MakeFont();
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
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 76)
{
    auto font = MakeFont();
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
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 77)
{
    auto font = MakeFont();
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1;
    auto buffer = builder.AllocRunPos(font, -1, &rect1);
    for (int i = 0; i < -1; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 50.f*i;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3078
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 78)
{
    auto font = MakeFont();
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1;
    auto buffer = builder.AllocRunPos(font, -1, &rect1);
    for (int i = 0; i < -1; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 0;
        buffer.pos[i * 2 + 1] = 30.f*i;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3079
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 79)
{
    auto font = MakeFont();
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1;
    auto buffer = builder.AllocRunPos(font, -1, &rect1);
    for (int i = 0; i < -1; i++) {
        buffer.glyphs[i] = font.UnicharToGlyph(0X9088);
        buffer.pos[i * 2] = 50.f*i;
        buffer.pos[i * 2 + 1] = 0;
    }
    DrawText(builder, playbackCanvas_);
}

//对应用例allocrunpos_3080
DEF_DTK(textblobbuilder_allocrunpos, TestLevel::L1, 80)
{
    auto font = MakeFont();
    Drawing::TextBlobBuilder builder;
    Drawing::Rect rect1;
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
