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
#include "text/typeface.h"

/*
测试类：textblob
测试接口：makefrompostext
测试内容：对接口入参text内容、text元素个数、位置、编码格式进行参数组合，通过drawtextblob接口将内容绘制在画布上。
*/

namespace OHOS {
namespace Rosen {

static void DrawTexts(const std::string& fontName, const std::string& textInfo, size_t textSize,
    Drawing::TextEncoding textEncoding, TestPlaybackCanvas* playbackCanvas)
{
    int line = 200;
    int interval1 = 100;
    int interval2 = 200;

    std::shared_ptr<Drawing::FontMgr> font_mgr(Drawing::FontMgr::CreateDefaultFontMgr());
    std::shared_ptr<Drawing::FontStyleSet> fontStyleSet(font_mgr->MatchFamily(fontName.c_str()));
    auto typeface = Drawing::Typeface::MakeDefault();
    typeface = std::shared_ptr<Drawing::Typeface>(fontStyleSet->CreateTypeface(0));

    auto font = Drawing::Font();
    font.SetTypeface(typeface);
    font.SetSize(48.f);
    std::shared_ptr<Drawing::TextBlob> infoTextBlob =
        Drawing::TextBlob::MakeFromText(textInfo.c_str(), textSize, font, textEncoding);

    Drawing::Brush brush;
    playbackCanvas->AttachBrush(brush);
    playbackCanvas->DrawTextBlob(infoTextBlob.get(), interval2, line);
    playbackCanvas->DetachBrush();
    Drawing::Pen pen;
    playbackCanvas->AttachPen(pen);
    playbackCanvas->DrawTextBlob(infoTextBlob.get(), interval2, line + interval1);
    playbackCanvas->DetachPen();
}

// 对应用例makefromtext_3001
DEF_DTK(textblob_makefromtext, TestLevel::L1, 1)
{
    std::string textInfo = "harmony_os";
    DrawTexts("HarmonyOS Sans SC", textInfo, textInfo.size(), Drawing::TextEncoding::GLYPH_ID, playbackCanvas_);
}

// 对应用例makefromtext_3002
DEF_DTK(textblob_makefromtext, TestLevel::L1, 2)
{
    std::string textInfo = "harmony_os";
    DrawTexts("HarmonyOS Sans SC", textInfo, 10, Drawing::TextEncoding::UTF8, playbackCanvas_);
}

// 对应用例makefromtext_3003
DEF_DTK(textblob_makefromtext, TestLevel::L1, 3)
{
    std::string textInfo = "harmony_os";
    DrawTexts("HarmonyOS Sans SC", textInfo, textInfo.size() - 1, Drawing::TextEncoding::UTF32, playbackCanvas_);
}

// 对应用例makefromtext_3004
DEF_DTK(textblob_makefromtext, TestLevel::L1, 4)
{
    std::string textInfo = "harmony_os";
    DrawTexts("HarmonyOS Sans SC", textInfo, textInfo.size() + 1, Drawing::TextEncoding::UTF16, playbackCanvas_);
}

// 对应用例makefromtext_3005
DEF_DTK(textblob_makefromtext, TestLevel::L1, 5)
{
    std::string textInfo = "harmony_os";
    DrawTexts("HarmonyOS Sans SC", textInfo, 0, Drawing::TextEncoding::UTF32, playbackCanvas_);
}

// 对应用例makefromtext_3006
DEF_DTK(textblob_makefromtext, TestLevel::L1, 6)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts("HarmonyOS Sans SC", textInfo, textInfo.size(), Drawing::TextEncoding::UTF8, playbackCanvas_);
}

// 对应用例makefromtext_3007
DEF_DTK(textblob_makefromtext, TestLevel::L1, 7)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts("HarmonyOS Sans SC", textInfo, 10, Drawing::TextEncoding::UTF16, playbackCanvas_);
}

// 对应用例makefromtext_3008
DEF_DTK(textblob_makefromtext, TestLevel::L1, 8)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts("HarmonyOS Sans SC", textInfo, textInfo.size() - 1, Drawing::TextEncoding::GLYPH_ID, playbackCanvas_);
}

// 对应用例makefromtext_3009
DEF_DTK(textblob_makefromtext, TestLevel::L1, 9)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts("HarmonyOS Sans SC", textInfo, textInfo.size() + 1, Drawing::TextEncoding::UTF32, playbackCanvas_);
}

// 对应用例makefromtext_3010
DEF_DTK(textblob_makefromtext, TestLevel::L1, 10)
{
    std::string textInfo = "鸿蒙鸿蒙鸿蒙鸿蒙鸿蒙";
    DrawTexts("HarmonyOS Sans SC", textInfo, 0, Drawing::TextEncoding::GLYPH_ID, playbackCanvas_);
}

// 对应用例makefromtext_3011
DEF_DTK(textblob_makefromtext, TestLevel::L1, 11)
{
    std::string textInfo = "!@#$%^&*()";
    DrawTexts("HarmonyOS Sans SC", textInfo, textInfo.size(), Drawing::TextEncoding::UTF16, playbackCanvas_);
}

// 对应用例makefromtext_3012
DEF_DTK(textblob_makefromtext, TestLevel::L1, 12)
{
    std::string textInfo = "!@#$%^&*()";
    DrawTexts("HarmonyOS Sans SC", textInfo, 10, Drawing::TextEncoding::UTF32, playbackCanvas_);
}

// 对应用例makefromtext_3013
DEF_DTK(textblob_makefromtext, TestLevel::L1, 13)
{
    std::string textInfo = "!@#$%^&*()";
    DrawTexts("HarmonyOS Sans SC", textInfo, textInfo.size() - 1, Drawing::TextEncoding::UTF8, playbackCanvas_);
}

// 对应用例makefromtext_3014
DEF_DTK(textblob_makefromtext, TestLevel::L1, 14)
{
    std::string textInfo = "!@#$%^&*()";
    DrawTexts("HarmonyOS Sans SC", textInfo, textInfo.size() + 1, Drawing::TextEncoding::GLYPH_ID, playbackCanvas_);
}

// 对应用例makefromtext_3015
DEF_DTK(textblob_makefromtext, TestLevel::L1, 15)
{
    std::string textInfo = "!@#$%^&*()";
    DrawTexts("HarmonyOS Sans SC", textInfo, 0, Drawing::TextEncoding::UTF8, playbackCanvas_);
}

// 对应用例makefromtext_3016
DEF_DTK(textblob_makefromtext, TestLevel::L1, 16)
{
    std::string textInfo = "1234567890";
    DrawTexts("HarmonyOS Sans SC", textInfo, textInfo.size(), Drawing::TextEncoding::UTF32, playbackCanvas_);
}

// 对应用例makefromtext_3017
DEF_DTK(textblob_makefromtext, TestLevel::L1, 17)
{
    std::string textInfo = "1234567890";
    DrawTexts("HarmonyOS Sans SC", textInfo, 10, Drawing::TextEncoding::GLYPH_ID, playbackCanvas_);
}

// 对应用例makefromtext_3018
DEF_DTK(textblob_makefromtext, TestLevel::L1, 18)
{
    std::string textInfo = "1234567890";
    DrawTexts("HarmonyOS Sans SC", textInfo, textInfo.size() - 1, Drawing::TextEncoding::UTF16, playbackCanvas_);
}

// 对应用例makefromtext_3019
DEF_DTK(textblob_makefromtext, TestLevel::L1, 19)
{
    std::string textInfo = "1234567890";
    DrawTexts("HarmonyOS Sans SC", textInfo, textInfo.size() + 1, Drawing::TextEncoding::UTF8, playbackCanvas_);
}

// 对应用例makefromtext_3020
DEF_DTK(textblob_makefromtext, TestLevel::L1, 20)
{
    std::string textInfo = "1234567890";
    DrawTexts("HarmonyOS Sans SC", textInfo, 0, Drawing::TextEncoding::UTF16, playbackCanvas_);
}

// 对应用例makefromtext_3021
DEF_DTK(textblob_makefromtext, TestLevel::L1, 21)
{
    std::string textInfo = "harmony_os";
    DrawTexts("HarmonyOS Sans SC", nullptr, textInfo.size(), Drawing::TextEncoding::UTF32, playbackCanvas_);
}

// 对应用例makefromtext_3022
DEF_DTK(textblob_makefromtext, TestLevel::L1, 22)
{
    DrawTexts("HarmonyOS Sans SC", nullptr, 10, Drawing::TextEncoding::GLYPH_ID, playbackCanvas_);
}

// 对应用例makefromtext_3023
DEF_DTK(textblob_makefromtext, TestLevel::L1, 23)
{
    std::string textInfo = "harmony_os";
    DrawTexts("HarmonyOS Sans SC", nullptr, textInfo.size() - 1, Drawing::TextEncoding::UTF8, playbackCanvas_);
}

// 对应用例makefromtext_3024
DEF_DTK(textblob_makefromtext, TestLevel::L1, 24)
{
    std::string textInfo = "harmony_os";
    DrawTexts("HarmonyOS Sans SC", nullptr, textInfo.size() + 1, Drawing::TextEncoding::UTF16, playbackCanvas_);
}

// 对应用例makefromtext_3025
DEF_DTK(textblob_makefromtext, TestLevel::L1, 25)
{
    DrawTexts("HarmonyOS Sans SC", nullptr, 0, Drawing::TextEncoding::UTF8, playbackCanvas_);
}

// 对应用例makefromtext_3026
DEF_DTK(textblob_makefromtext, TestLevel::L1, 26)
{
    std::string name = "HMOS Color Emoji";
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(name, textInfo.c_str(), textInfo.size(), Drawing::TextEncoding::GLYPH_ID, playbackCanvas_);
}

// 对应用例makefromtext_3027
DEF_DTK(textblob_makefromtext, TestLevel::L1, 27)
{
    std::string name = "HMOS Color Emoji";
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(name, textInfo.c_str(), 10, Drawing::TextEncoding::UTF8, playbackCanvas_);
}

// 对应用例makefromtext_3028
DEF_DTK(textblob_makefromtext, TestLevel::L1, 28)
{
    std::string name = "HMOS Color Emoji";
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(name, textInfo.c_str(), textInfo.size() - 1, Drawing::TextEncoding::UTF16, playbackCanvas_);
}

// 对应用例makefromtext_3029
DEF_DTK(textblob_makefromtext, TestLevel::L1, 29)
{
    std::string name = "HMOS Color Emoji";
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(name, textInfo.c_str(), textInfo.size() + 1, Drawing::TextEncoding::UTF32, playbackCanvas_);
}

// 对应用例makefromtext_3030
DEF_DTK(textblob_makefromtext, TestLevel::L1, 30)
{
    std::string name = "HMOS Color Emoji";
    std::string textInfo = "\xE2\x99\x88\xE2\x99\x89\xE2\x99\x8A\xE2\x99\x8B\xE2\x99\x89\xE2\x99\x8A\xE2\x99";
    DrawTexts(name, textInfo.c_str(), 0, Drawing::TextEncoding::UTF16, playbackCanvas_);
}

} // namespace Rosen
} // namespace OHOS