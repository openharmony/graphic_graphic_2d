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
测试接口：MakeFromStream
测试内容：对接口入参字体的字节流stream和index取值组合，构造typeface字体格式，并指定在font上，最终通过drawtextblob接口将text内容绘制在画布上
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

//对应用例 TypeFace_3109
DEF_DTK(typeface_makefromstream, TestLevel::L2, 109)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    auto data = typeface0->Serialize();
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream));
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetFamilyName=" + typeface->GetFamilyName();
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3110
DEF_DTK(typeface_makefromstream, TestLevel::L2, 110)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    auto data = typeface0->Serialize();
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream));
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetHash=" + std::to_string(typeface0->GetHash());
    std::string text5 = "GetHash=" + std::to_string(typeface->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4, text5};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3111
DEF_DTK(typeface_makefromstream, TestLevel::L2, 111)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    auto data = typeface0->Serialize();
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream));
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("DSIG");
    std::string text4 = "GetTableSize=" + std::to_string(typeface->GetTableSize(tagid));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3112
DEF_DTK(typeface_makefromstream, TestLevel::L2, 112)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    auto data = typeface0->Serialize();
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream));
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("CPAL");
    std::string text4 = "GetTableSize=" + std::to_string(typeface->GetTableSize(tagid));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3113
DEF_DTK(typeface_makefromstream, TestLevel::L2, 113)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    auto data = typeface0->Serialize();
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream));
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("DSIG");
    int a = MakeOffset("011c50");
    char abc[100];
    std::string text4 = "GetTableData=" +
        std::to_string(typeface->GetTableData(tagid, a, typeface->GetTableSize(tagid), (void*)abc));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3114
DEF_DTK(typeface_makefromstream, TestLevel::L2, 114)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    auto data = typeface0->Serialize();
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream));
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("CPAL");
    int a = MakeOffset("015e7260");
    std::string text4 = "GetTableData=" +
        std::to_string(typeface->GetTableData(tagid, a, typeface->GetTableSize(tagid) + 1, nullptr));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3115
DEF_DTK(typeface_makefromstream, TestLevel::L2, 115)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    auto data = typeface0->Serialize();
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream));
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetItalic=" + std::to_string(typeface->GetItalic());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3116
DEF_DTK(typeface_makefromstream, TestLevel::L2, 116)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    auto data = typeface0->Serialize();
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream));
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetUniqueID=" + std::to_string(typeface->GetUniqueID());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3117
DEF_DTK(typeface_makefromstream, TestLevel::L2, 117)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    auto data = typeface0->Serialize();
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream));
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetUnitsPerEm=" + std::to_string(typeface->GetUnitsPerEm());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3118
DEF_DTK(typeface_makefromstream, TestLevel::L2, 118)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    auto data = typeface0->Serialize();
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream));
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "IsCustomTypeface=" +  std::to_string(typeface->IsCustomTypeface());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3119
DEF_DTK(typeface_makefromstream, TestLevel::L2, 119)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    auto data = typeface0->Serialize();
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream));
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetHash=" + std::to_string(typeface->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3120
DEF_DTK(typeface_makefromstream, TestLevel::L2, 120)
{
    std::shared_ptr<Drawing::Typeface> typefacex = Drawing::Typeface::MakeDefault();
    auto data0 = typefacex->Serialize();
    auto stream = std::make_unique<Drawing::MemoryStream>(data0->GetData(), data0->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream));
    auto data = typeface->Serialize();
    auto typeface0 = Drawing::Typeface::Deserialize(data->GetData(), typeface->GetSize());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetHash=" + std::to_string(typeface0->GetHash());
    std::string text5 = "GetHash=" + std::to_string(typeface->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4, text5};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3121
DEF_DTK(typeface_makefromstream, TestLevel::L2, 121)
{
    std::shared_ptr<Drawing::Typeface> typeface0 =
        Drawing::Typeface::MakeFromFile("/system/fonts/HmosColorEmojiCompat.ttf");
    auto data = typeface0->Serialize();
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream));
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetFamilyName=" + typeface->GetFamilyName();
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3122
DEF_DTK(typeface_makefromstream, TestLevel::L2, 122)
{
    std::shared_ptr<Drawing::Typeface> typeface0 =
        Drawing::Typeface::MakeFromFile("/system/fonts/HmosColorEmojiCompat.ttf");
    auto data = typeface0->Serialize();
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream));
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetHash=" + std::to_string(typeface0->GetHash());
    std::string text5 = "GetHash=" + std::to_string(typeface->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4, text5};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3123
DEF_DTK(typeface_makefromstream, TestLevel::L2, 123)
{
    std::shared_ptr<Drawing::Typeface> typeface0 =
        Drawing::Typeface::MakeFromFile("/system/fonts/HmosColorEmojiCompat.ttf");
    auto data = typeface0->Serialize();
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream));
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("DSIG");
    std::string text4 = "GetTableSize=" + std::to_string(typeface->GetTableSize(tagid));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3124
DEF_DTK(typeface_makefromstream, TestLevel::L2, 124)
{
    std::shared_ptr<Drawing::Typeface> typeface0 =
        Drawing::Typeface::MakeFromFile("/system/fonts/HmosColorEmojiCompat.ttf");
    auto data = typeface0->Serialize();
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream));
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("CPAL");
    std::string text4 = "GetTableSize=" + std::to_string(typeface->GetTableSize(tagid));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3125
DEF_DTK(typeface_makefromstream, TestLevel::L2, 125)
{
    std::shared_ptr<Drawing::Typeface> typeface0 =
        Drawing::Typeface::MakeFromFile("/system/fonts/HmosColorEmojiCompat.ttf");
    auto data = typeface0->Serialize();
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream));
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("DSIG");
    int a = MakeOffset("011c50");
    char abc[100];
    std::string text4 = "GetTableData=" +
        std::to_string(typeface->GetTableData(tagid, a, typeface->GetTableSize(tagid), (void*)abc));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3126
DEF_DTK(typeface_makefromstream, TestLevel::L2, 126)
{
    std::shared_ptr<Drawing::Typeface> typeface0 =
        Drawing::Typeface::MakeFromFile("/system/fonts/HmosColorEmojiCompat.ttf");
    auto data = typeface0->Serialize();
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream));
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("CPAL");
    int a = MakeOffset("015e7260");
    std::string text4 = "GetTableData=" +
        std::to_string(typeface->GetTableData(tagid, a, typeface->GetTableSize(tagid) + 1, nullptr));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3127
DEF_DTK(typeface_makefromstream, TestLevel::L2, 127)
{
    std::shared_ptr<Drawing::Typeface> typeface0 =
        Drawing::Typeface::MakeFromFile("/system/fonts/HmosColorEmojiCompat.ttf");
    auto data = typeface0->Serialize();
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream));
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetItalic=" + std::to_string(typeface->GetItalic());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3128
DEF_DTK(typeface_makefromstream, TestLevel::L2, 128)
{
    std::shared_ptr<Drawing::Typeface> typeface0 =
        Drawing::Typeface::MakeFromFile("/system/fonts/HmosColorEmojiCompat.ttf");
    auto data = typeface0->Serialize();
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream));
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetUniqueID=" + std::to_string(typeface->GetUniqueID());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3129
DEF_DTK(typeface_makefromstream, TestLevel::L2, 129)
{
    std::shared_ptr<Drawing::Typeface> typeface0 =
        Drawing::Typeface::MakeFromFile("/system/fonts/HmosColorEmojiCompat.ttf");
    auto data = typeface0->Serialize();
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream));
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetUnitsPerEm=" + std::to_string(typeface->GetUnitsPerEm());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3130
DEF_DTK(typeface_makefromstream, TestLevel::L2, 130)
{
    std::shared_ptr<Drawing::Typeface> typeface0 =
        Drawing::Typeface::MakeFromFile("/system/fonts/HmosColorEmojiCompat.ttf");
    auto data = typeface0->Serialize();
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream));
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "IsCustomTypeface=" +  std::to_string(typeface->IsCustomTypeface());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3131
DEF_DTK(typeface_makefromstream, TestLevel::L2, 131)
{
    std::shared_ptr<Drawing::Typeface> typeface0 =
        Drawing::Typeface::MakeFromFile("/system/fonts/HmosColorEmojiCompat.ttf");
    auto data = typeface0->Serialize();
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream));
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetHash=" + std::to_string(typeface->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3132
DEF_DTK(typeface_makefromstream, TestLevel::L2, 132)
{
    std::shared_ptr<Drawing::Typeface> typefacex =
        Drawing::Typeface::MakeFromFile("/system/fonts/HmosColorEmojiCompat.ttf");
    auto data0 = typefacex->Serialize();
    auto stream = std::make_unique<Drawing::MemoryStream>(data0->GetData(), data0->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream));
    auto data = typeface->Serialize();
    auto typeface0 = Drawing::Typeface::Deserialize(data->GetData(), typeface->GetSize());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetHash=" + std::to_string(typeface0->GetHash());
    std::string text5 = "GetHash=" + std::to_string(typeface->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4, text5};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3133
DEF_DTK(typeface_makefromstream, TestLevel::L2, 133)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    std::string text0 = "Serialize(TextBlob)";
    std::shared_ptr<Drawing::TextBlob> textBlob =
        Drawing::TextBlob::MakeFromText(text0.c_str(), text0.size(), Drawing::Font(typeface0, 50.f, 1.0f, 0.f));
    Drawing::TextBlob::Context* ctx = new (std::nothrow) Drawing::TextBlob::Context(typeface0, false);
    auto data = textBlob->Serialize(ctx);
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream));
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetFamilyName=" + typeface->GetFamilyName();
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3134
DEF_DTK(typeface_makefromstream, TestLevel::L2, 134)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    std::string text0 = "Serialize(TextBlob)";
    std::shared_ptr<Drawing::TextBlob> textBlob =
        Drawing::TextBlob::MakeFromText(text0.c_str(), text0.size(), Drawing::Font(typeface0, 50.f, 1.0f, 0.f));
    Drawing::TextBlob::Context* ctx = new (std::nothrow) Drawing::TextBlob::Context(typeface0, false);
    auto data = textBlob->Serialize(ctx);
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream));
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetHash=" + std::to_string(typeface0->GetHash());
    std::string text5 = "GetHash=" + std::to_string(typeface->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4, text5};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3135
DEF_DTK(typeface_makefromstream, TestLevel::L2, 135)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    std::string text0 = "Serialize(TextBlob)";
    std::shared_ptr<Drawing::TextBlob> textBlob =
        Drawing::TextBlob::MakeFromText(text0.c_str(), text0.size(), Drawing::Font(typeface0, 50.f, 1.0f, 0.f));
    Drawing::TextBlob::Context* ctx = new (std::nothrow) Drawing::TextBlob::Context(typeface0, false);
    auto data = textBlob->Serialize(ctx);
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream));
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("DSIG");
    std::string text4 = "GetTableSize=" + std::to_string(typeface->GetTableSize(tagid));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3136
DEF_DTK(typeface_makefromstream, TestLevel::L2, 136)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    std::string text0 = "Serialize(TextBlob)";
    std::shared_ptr<Drawing::TextBlob> textBlob =
        Drawing::TextBlob::MakeFromText(text0.c_str(), text0.size(), Drawing::Font(typeface0, 50.f, 1.0f, 0.f));
    Drawing::TextBlob::Context* ctx = new (std::nothrow) Drawing::TextBlob::Context(typeface0, false);
    auto data = textBlob->Serialize(ctx);
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream));
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("CPAL");
    std::string text4 = "GetTableSize=" + std::to_string(typeface->GetTableSize(tagid));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3137
DEF_DTK(typeface_makefromstream, TestLevel::L2, 137)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    std::string text0 = "Serialize(TextBlob)";
    std::shared_ptr<Drawing::TextBlob> textBlob =
        Drawing::TextBlob::MakeFromText(text0.c_str(), text0.size(), Drawing::Font(typeface0, 50.f, 1.0f, 0.f));
    Drawing::TextBlob::Context* ctx = new (std::nothrow) Drawing::TextBlob::Context(typeface0, false);
    auto data = textBlob->Serialize(ctx);
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream));
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("DSIG");
    int a = MakeOffset("011c50");
    char abc[100];
    std::string text4 = "GetTableData=" +
        std::to_string(typeface->GetTableData(tagid, a, typeface->GetTableSize(tagid), (void*)abc));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3138
DEF_DTK(typeface_makefromstream, TestLevel::L2, 138)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    std::string text0 = "Serialize(TextBlob)";
    std::shared_ptr<Drawing::TextBlob> textBlob =
        Drawing::TextBlob::MakeFromText(text0.c_str(), text0.size(), Drawing::Font(typeface0, 50.f, 1.0f, 0.f));
    Drawing::TextBlob::Context* ctx = new (std::nothrow) Drawing::TextBlob::Context(typeface0, false);
    auto data = textBlob->Serialize(ctx);
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream));
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("CPAL");
    int a = MakeOffset("015e7260");
    std::string text4 = "GetTableData=" +
        std::to_string(typeface->GetTableData(tagid, a, typeface->GetTableSize(tagid) + 1, nullptr));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3139
DEF_DTK(typeface_makefromstream, TestLevel::L2, 139)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    std::string text0 = "Serialize(TextBlob)";
    std::shared_ptr<Drawing::TextBlob> textBlob =
        Drawing::TextBlob::MakeFromText(text0.c_str(), text0.size(), Drawing::Font(typeface0, 50.f, 1.0f, 0.f));
    Drawing::TextBlob::Context* ctx = new (std::nothrow) Drawing::TextBlob::Context(typeface0, false);
    auto data = textBlob->Serialize(ctx);
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream));
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetItalic=" + std::to_string(typeface->GetItalic());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3140
DEF_DTK(typeface_makefromstream, TestLevel::L2, 140)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    std::string text0 = "Serialize(TextBlob)";
    std::shared_ptr<Drawing::TextBlob> textBlob =
        Drawing::TextBlob::MakeFromText(text0.c_str(), text0.size(), Drawing::Font(typeface0, 50.f, 1.0f, 0.f));
    Drawing::TextBlob::Context* ctx = new (std::nothrow) Drawing::TextBlob::Context(typeface0, false);
    auto data = textBlob->Serialize(ctx);
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream));
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetUniqueID=" + std::to_string(typeface->GetUniqueID());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3141
DEF_DTK(typeface_makefromstream, TestLevel::L2, 141)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    std::string text0 = "Serialize(TextBlob)";
    std::shared_ptr<Drawing::TextBlob> textBlob =
        Drawing::TextBlob::MakeFromText(text0.c_str(), text0.size(), Drawing::Font(typeface0, 50.f, 1.0f, 0.f));
    Drawing::TextBlob::Context* ctx = new (std::nothrow) Drawing::TextBlob::Context(typeface0, false);
    auto data = textBlob->Serialize(ctx);
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream));
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetUnitsPerEm=" + std::to_string(typeface->GetUnitsPerEm());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3142
DEF_DTK(typeface_makefromstream, TestLevel::L2, 142)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    std::string text0 = "Serialize(TextBlob)";
    std::shared_ptr<Drawing::TextBlob> textBlob =
        Drawing::TextBlob::MakeFromText(text0.c_str(), text0.size(), Drawing::Font(typeface0, 50.f, 1.0f, 0.f));
    Drawing::TextBlob::Context* ctx = new (std::nothrow) Drawing::TextBlob::Context(typeface0, false);
    auto data = textBlob->Serialize(ctx);
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream));
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "IsCustomTypeface=" +  std::to_string(typeface->IsCustomTypeface());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3143
DEF_DTK(typeface_makefromstream, TestLevel::L2, 143)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    std::string text0 = "Serialize(TextBlob)";
    std::shared_ptr<Drawing::TextBlob> textBlob =
        Drawing::TextBlob::MakeFromText(text0.c_str(), text0.size(), Drawing::Font(typeface0, 50.f, 1.0f, 0.f));
    Drawing::TextBlob::Context* ctx = new (std::nothrow) Drawing::TextBlob::Context(typeface0, false);
    auto data = textBlob->Serialize(ctx);
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream));
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetHash=" + std::to_string(typeface->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3144
DEF_DTK(typeface_makefromstream, TestLevel::L2, 144)
{
    std::shared_ptr<Drawing::Typeface> typefacex = Drawing::Typeface::MakeDefault();
    std::string text0 = "Serialize(TextBlob)";
    std::shared_ptr<Drawing::TextBlob> textBlob =
        Drawing::TextBlob::MakeFromText(text0.c_str(), text0.size(), Drawing::Font(typefacex, 50.f, 1.0f, 0.f));
    Drawing::TextBlob::Context* ctx = new (std::nothrow) Drawing::TextBlob::Context(typefacex, false);
    auto data0 = textBlob->Serialize(ctx);
    auto stream = std::make_unique<Drawing::MemoryStream>(data0->GetData(), data0->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream));
    auto data = typeface->Serialize();
    auto typeface0 = Drawing::Typeface::Deserialize(data->GetData(), typeface->GetSize());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetHash=" + std::to_string(typeface0->GetHash());
    std::string text5 = "GetHash=" + std::to_string(typeface->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4, text5};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3145
DEF_DTK(typeface_makefromstream, TestLevel::L2, 145)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    auto data = typeface0->Serialize();
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream), 100);
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetFamilyName=" + typeface->GetFamilyName();
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3146
DEF_DTK(typeface_makefromstream, TestLevel::L2, 146)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    auto data = typeface0->Serialize();
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream), 100);
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetHash=" + std::to_string(typeface0->GetHash());
    std::string text5 = "GetHash=" + std::to_string(typeface->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4, text5};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3147
DEF_DTK(typeface_makefromstream, TestLevel::L2, 147)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    auto data = typeface0->Serialize();
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream), 100);
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("DSIG");
    std::string text4 = "GetTableSize=" + std::to_string(typeface->GetTableSize(tagid));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3148
DEF_DTK(typeface_makefromstream, TestLevel::L2, 148)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    auto data = typeface0->Serialize();
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream), 100);
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("CPAL");
    std::string text4 = "GetTableSize=" + std::to_string(typeface->GetTableSize(tagid));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3149
DEF_DTK(typeface_makefromstream, TestLevel::L2, 149)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    auto data = typeface0->Serialize();
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream), 100);
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("DSIG");
    int a = MakeOffset("011c50");
    char abc[100];
    std::string text4 = "GetTableData=" +
        std::to_string(typeface->GetTableData(tagid, a, typeface->GetTableSize(tagid), (void*)abc));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3150
DEF_DTK(typeface_makefromstream, TestLevel::L2, 150)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    auto data = typeface0->Serialize();
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream), 100);
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("CPAL");
    int a = MakeOffset("015e7260");
    std::string text4 = "GetTableData=" +
        std::to_string(typeface->GetTableData(tagid, a, typeface->GetTableSize(tagid) + 1, nullptr));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3151
DEF_DTK(typeface_makefromstream, TestLevel::L2, 151)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    auto data = typeface0->Serialize();
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream), 100);
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetItalic=" + std::to_string(typeface->GetItalic());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3152
DEF_DTK(typeface_makefromstream, TestLevel::L2, 152)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    auto data = typeface0->Serialize();
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream), 100);
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetUniqueID=" + std::to_string(typeface->GetUniqueID());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3153
DEF_DTK(typeface_makefromstream, TestLevel::L2, 153)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    auto data = typeface0->Serialize();
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream), 100);
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetUnitsPerEm=" + std::to_string(typeface->GetUnitsPerEm());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3154
DEF_DTK(typeface_makefromstream, TestLevel::L2, 154)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    auto data = typeface0->Serialize();
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream), 100);
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "IsCustomTypeface=" +  std::to_string(typeface->IsCustomTypeface());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3155
DEF_DTK(typeface_makefromstream, TestLevel::L2, 155)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    auto data = typeface0->Serialize();
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream), 100);
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetHash=" + std::to_string(typeface->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3156
DEF_DTK(typeface_makefromstream, TestLevel::L2, 156)
{
    std::shared_ptr<Drawing::Typeface> typefacex = Drawing::Typeface::MakeDefault();
    auto data0 = typefacex->Serialize();
    auto stream = std::make_unique<Drawing::MemoryStream>(data0->GetData(), data0->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream), 100);
    auto data = typeface->Serialize();
    auto typeface0 = Drawing::Typeface::Deserialize(data->GetData(), typeface->GetSize());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetHash=" + std::to_string(typeface0->GetHash());
    std::string text5 = "GetHash=" + std::to_string(typeface->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4, text5};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3157
DEF_DTK(typeface_makefromstream, TestLevel::L2, 157)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    auto data = typeface0->Serialize();
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream), -5);
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetFamilyName=" + typeface->GetFamilyName();
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3158
DEF_DTK(typeface_makefromstream, TestLevel::L2, 158)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    auto data = typeface0->Serialize();
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream), -5);
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetHash=" + std::to_string(typeface0->GetHash());
    std::string text5 = "GetHash=" + std::to_string(typeface->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4, text5};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3159
DEF_DTK(typeface_makefromstream, TestLevel::L2, 159)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    auto data = typeface0->Serialize();
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream), -5);
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("DSIG");
    std::string text4 = "GetTableSize=" + std::to_string(typeface->GetTableSize(tagid));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3160
DEF_DTK(typeface_makefromstream, TestLevel::L2, 160)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    auto data = typeface0->Serialize();
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream), -5);
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("CPAL");
    std::string text4 = "GetTableSize=" + std::to_string(typeface->GetTableSize(tagid));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3161
DEF_DTK(typeface_makefromstream, TestLevel::L2, 161)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    auto data = typeface0->Serialize();
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream), -5);
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("DSIG");
    int a = MakeOffset("011c50");
    char abc[100];
    std::string text4 = "GetTableData=" +
        std::to_string(typeface->GetTableData(tagid, a, typeface->GetTableSize(tagid), (void*)abc));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3162
DEF_DTK(typeface_makefromstream, TestLevel::L2, 162)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    auto data = typeface0->Serialize();
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream), -5);
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("CPAL");
    int a = MakeOffset("015e7260");
    std::string text4 = "GetTableData=" +
        std::to_string(typeface->GetTableData(tagid, a, typeface->GetTableSize(tagid) + 1, nullptr));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3163
DEF_DTK(typeface_makefromstream, TestLevel::L2, 163)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    auto data = typeface0->Serialize();
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream), -5);
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetItalic=" + std::to_string(typeface->GetItalic());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3164
DEF_DTK(typeface_makefromstream, TestLevel::L2, 164)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    auto data = typeface0->Serialize();
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream), -5);
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetUniqueID=" + std::to_string(typeface->GetUniqueID());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3165
DEF_DTK(typeface_makefromstream, TestLevel::L2, 165)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    auto data = typeface0->Serialize();
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream), -5);
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetUnitsPerEm=" + std::to_string(typeface->GetUnitsPerEm());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3166
DEF_DTK(typeface_makefromstream, TestLevel::L2, 166)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    auto data = typeface0->Serialize();
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream), -5);
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "IsCustomTypeface=" +  std::to_string(typeface->IsCustomTypeface());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3167
DEF_DTK(typeface_makefromstream, TestLevel::L2, 167)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    auto data = typeface0->Serialize();
    auto stream = std::make_unique<Drawing::MemoryStream>(data->GetData(), data->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream), -5);
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetHash=" + std::to_string(typeface->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3168
DEF_DTK(typeface_makefromstream, TestLevel::L2, 168)
{
    std::shared_ptr<Drawing::Typeface> typefacex = Drawing::Typeface::MakeDefault();
    auto data0 = typefacex->Serialize();
    auto stream = std::make_unique<Drawing::MemoryStream>(data0->GetData(), data0->GetSize());
    auto typeface = Drawing::Typeface::MakeFromStream(std::move(stream), -5);
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
