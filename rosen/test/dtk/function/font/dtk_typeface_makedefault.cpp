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
#include "modules/2d_graphics/include/text/font.h"
#include "modules/2d_graphics/include/text/font_mgr.h"
#include "modules/2d_graphics/include/text/font_style_set.h"
#include "modules/2d_graphics/include/text/font_style.h"
#include "modules/2d_graphics/include/text/typeface.h"
#include <sstream>

/*
测试类：Typeface
测试接口：MakeDefault
测试内容：通过缺省构造系统默认typeface字体格式，并指定在font上，最终通过drawtextblob接口将text内容绘制在画布上
*/

namespace OHOS {
namespace Rosen {

static std::string g_text1 = "DDGR ddgr 鸿蒙 !@#%￥^&*;：，。";
static std::string g_text2 = "-_=+()123`.---~|{}【】,./?、？<>《》";
static std::string g_text3 = "\xE2\x99\x88\xE2\x99\x8A\xE2\x99\x88\xE2\x99\x8C\xE2\x99\x8D\xE2\x99\x8D";

static void DrawTexts(std::vector<std::string>& texts, Drawing::Font& font, TestPlaybackCanvas* playbackCanvas)
{
    int line = 200;
    int interval = 200;

    for (auto text : texts) {
        std::shared_ptr<Drawing::TextBlob> textBlob = Drawing::TextBlob::MakeFromText(text.c_str(), text.size(), font);
        Drawing::Brush brush;
        playbackCanvas->AttachBrush(brush);
        playbackCanvas->DrawTextBlob(textBlob.get(), interval, line);
        line += interval;
        playbackCanvas->DetachBrush();
        Drawing::Pen pen;
        playbackCanvas->AttachPen(pen);
        playbackCanvas->DrawTextBlob(textBlob.get(), interval, line);
        line += interval;
        playbackCanvas->DetachPen();
    }
}

static uint32_t MakeTagId(std::string str)
{
    reverse(str.begin(), str.end());
    uint32_t tagid = *(uint32_t*)(str.c_str());

    return tagid;
}

static int MakeOffset(std::string str)
{
    int a;
    std::stringstream ss;
    ss<<std::hex<<str;
    ss>>a;
    return a;
}

//对应用例 TypeFace_3001
DEF_DTK(typeface_makefromdefault, TestLevel::L2, 1)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeDefault();
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetFamilyName=" + typeface->GetFamilyName();
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3002
DEF_DTK(typeface_makefromdefault, TestLevel::L2, 2)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeDefault();
    std::shared_ptr<Drawing::Typeface> typeface0 =
        Drawing::Typeface::MakeFromName(typeface->GetFamilyName().c_str(), typeface->GetFontStyle());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetHash=" + std::to_string(typeface0->GetHash());
    std::string text5 = "GetHash=" + std::to_string(typeface->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4, text5};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3003
DEF_DTK(typeface_makefromdefault, TestLevel::L2, 3)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeDefault();
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("DSIG");
    std::string text4 = "GetTableSize=" + std::to_string(typeface->GetTableSize(tagid));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3004
DEF_DTK(typeface_makefromdefault, TestLevel::L2, 4)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeDefault();
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("CPAL");
    std::string text4 = "GetTableSize=" + std::to_string(typeface->GetTableSize(tagid));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3005
DEF_DTK(typeface_makefromdefault, TestLevel::L2, 5)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeDefault();
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("DSIG");
    int a = MakeOffset("011c50");
    char abc[100];
    std::string text4 = "GetTableData=" +
        std::to_string(typeface->GetTableData(tagid, a, typeface->GetTableSize(tagid), (void*)abc));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3006
DEF_DTK(typeface_makefromdefault, TestLevel::L2, 6)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeDefault();
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("CPAL");
    int a = MakeOffset("015e7260");
    std::string text4 = "GetTableData=" +
        std::to_string(typeface->GetTableData(tagid, a, typeface->GetTableSize(tagid) + 1, nullptr));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3007
DEF_DTK(typeface_makefromdefault, TestLevel::L2, 7)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeDefault();
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetItalic=" + std::to_string(typeface->GetItalic());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3008
DEF_DTK(typeface_makefromdefault, TestLevel::L2, 8)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeDefault();
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetUniqueID=" + std::to_string(typeface->GetUniqueID());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3009
DEF_DTK(typeface_makefromdefault, TestLevel::L2, 9)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeDefault();
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetUnitsPerEm=" + std::to_string(typeface->GetUnitsPerEm());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3010
DEF_DTK(typeface_makefromdefault, TestLevel::L2, 10)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeDefault();
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "IsCustomTypeface=" +  std::to_string(typeface->IsCustomTypeface());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3011
DEF_DTK(typeface_makefromdefault, TestLevel::L2, 11)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeDefault();
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetHash=" + std::to_string(typeface->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3012
DEF_DTK(typeface_makefromdefault, TestLevel::L2, 12)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeDefault();
    auto data = typeface->Serialize();
    auto typeface0 = Drawing::Typeface::Deserialize(data->GetData(), typeface->GetSize());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetHash=" + std::to_string(typeface0->GetHash());
    std::string text5 = "GetHash=" + std::to_string(typeface->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4, text5};

    DrawTexts(texts, font, playbackCanvas_);
}

}
}
