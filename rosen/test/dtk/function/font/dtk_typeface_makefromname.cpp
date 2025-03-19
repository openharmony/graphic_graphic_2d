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

//对应用例 TypeFace_3169
DEF_DTK(typeface_makefromname, TestLevel::L2, 169)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromName(typeface0->GetFamilyName().c_str(), typeface0->GetFontStyle());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetFamilyName=" + typeface->GetFamilyName();
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3170
DEF_DTK(typeface_makefromname, TestLevel::L2, 170)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromName(typeface0->GetFamilyName().c_str(), typeface0->GetFontStyle());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetHash=" + std::to_string(typeface0->GetHash());
    std::string text5 = "GetHash=" + std::to_string(typeface->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4, text5};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3171
DEF_DTK(typeface_makefromname, TestLevel::L2, 171)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromName(typeface0->GetFamilyName().c_str(), typeface0->GetFontStyle());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("DSIG");
    std::string text4 = "GetTableSize=" + std::to_string(typeface->GetTableSize(tagid));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3172
DEF_DTK(typeface_makefromname, TestLevel::L2, 172)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromName(typeface0->GetFamilyName().c_str(), typeface0->GetFontStyle());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("CPAL");
    std::string text4 = "GetTableSize=" + std::to_string(typeface->GetTableSize(tagid));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3173
DEF_DTK(typeface_makefromname, TestLevel::L2, 173)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromName(typeface0->GetFamilyName().c_str(), typeface0->GetFontStyle());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("DSIG");
    int a = MakeOffset("011c50");
    char abc[100];
    std::string text4 = "GetTableData=" +
        std::to_string(typeface->GetTableData(tagid, a, typeface->GetTableSize(tagid), (void*)abc));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3174
DEF_DTK(typeface_makefromname, TestLevel::L2, 174)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromName(typeface0->GetFamilyName().c_str(), typeface0->GetFontStyle());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("CPAL");
    int a = MakeOffset("015e7260");
    std::string text4 = "GetTableData=" +
        std::to_string(typeface->GetTableData(tagid, a, typeface->GetTableSize(tagid) + 1, nullptr));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3175
DEF_DTK(typeface_makefromname, TestLevel::L2, 175)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromName(typeface0->GetFamilyName().c_str(), typeface0->GetFontStyle());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetItalic=" + std::to_string(typeface->GetItalic());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3176
DEF_DTK(typeface_makefromname, TestLevel::L2, 176)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromName(typeface0->GetFamilyName().c_str(), typeface0->GetFontStyle());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetUniqueID=" + std::to_string(typeface->GetUniqueID());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3177
DEF_DTK(typeface_makefromname, TestLevel::L2, 177)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromName(typeface0->GetFamilyName().c_str(), typeface0->GetFontStyle());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetUnitsPerEm=" + std::to_string(typeface->GetUnitsPerEm());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3178
DEF_DTK(typeface_makefromname, TestLevel::L2, 178)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromName(typeface0->GetFamilyName().c_str(), typeface0->GetFontStyle());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "IsCustomTypeface=" +  std::to_string(typeface->IsCustomTypeface());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3179
DEF_DTK(typeface_makefromname, TestLevel::L2, 179)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromName(typeface0->GetFamilyName().c_str(), typeface0->GetFontStyle());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetHash=" + std::to_string(typeface->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3180
DEF_DTK(typeface_makefromname, TestLevel::L2, 180)
{
    std::shared_ptr<Drawing::Typeface> typeface0 = Drawing::Typeface::MakeDefault();
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromName(typeface0->GetFamilyName().c_str(), typeface0->GetFontStyle());
    auto data = typeface->Serialize();
    auto typeface1 = Drawing::Typeface::Deserialize(data->GetData(), typeface->GetSize());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetHash=" + std::to_string(typeface->GetHash());
    std::string text5 = "GetHash=" + std::to_string(typeface1->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4, text5};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3181
DEF_DTK(typeface_makefromname, TestLevel::L2, 181)
{
    std::shared_ptr<Drawing::Typeface> typeface0 =
        Drawing::Typeface::MakeFromFile("/system/fonts/HMOSColorEmojiCompat.ttf");
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromName(typeface0->GetFamilyName().c_str(), typeface0->GetFontStyle());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetFamilyName=" + typeface->GetFamilyName();
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3182
DEF_DTK(typeface_makefromname, TestLevel::L2, 182)
{
    std::shared_ptr<Drawing::Typeface> typeface0 =
        Drawing::Typeface::MakeFromFile("/system/fonts/HMOSColorEmojiCompat.ttf");
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromName(typeface0->GetFamilyName().c_str(), typeface0->GetFontStyle());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetHash=" + std::to_string(typeface0->GetHash());
    std::string text5 = "GetHash=" + std::to_string(typeface->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4, text5};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3183
DEF_DTK(typeface_makefromname, TestLevel::L2, 183)
{
    std::shared_ptr<Drawing::Typeface> typeface0 =
        Drawing::Typeface::MakeFromFile("/system/fonts/HMOSColorEmojiCompat.ttf");
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromName(typeface0->GetFamilyName().c_str(), typeface0->GetFontStyle());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("DSIG");
    std::string text4 = "GetTableSize=" + std::to_string(typeface->GetTableSize(tagid));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3184
DEF_DTK(typeface_makefromname, TestLevel::L2, 184)
{
    std::shared_ptr<Drawing::Typeface> typeface0 =
        Drawing::Typeface::MakeFromFile("/system/fonts/HMOSColorEmojiCompat.ttf");
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromName(typeface0->GetFamilyName().c_str(), typeface0->GetFontStyle());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("CPAL");
    std::string text4 = "GetTableSize=" + std::to_string(typeface->GetTableSize(tagid));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3185
DEF_DTK(typeface_makefromname, TestLevel::L2, 185)
{
    std::shared_ptr<Drawing::Typeface> typeface0 =
        Drawing::Typeface::MakeFromFile("/system/fonts/HMOSColorEmojiCompat.ttf");
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromName(typeface0->GetFamilyName().c_str(), typeface0->GetFontStyle());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("DSIG");
    int a = MakeOffset("011c50");
    char abc[100];
    std::string text4 = "GetTableData=" +
        std::to_string(typeface->GetTableData(tagid, a, typeface->GetTableSize(tagid), (void*)abc));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3186
DEF_DTK(typeface_makefromname, TestLevel::L2, 186)
{
    std::shared_ptr<Drawing::Typeface> typeface0 =
        Drawing::Typeface::MakeFromFile("/system/fonts/HMOSColorEmojiCompat.ttf");
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromName(typeface0->GetFamilyName().c_str(), typeface0->GetFontStyle());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("CPAL");
    int a = MakeOffset("015e7260");
    std::string text4 = "GetTableData=" +
        std::to_string(typeface->GetTableData(tagid, a, typeface->GetTableSize(tagid) + 1, nullptr));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3187
DEF_DTK(typeface_makefromname, TestLevel::L2, 187)
{
    std::shared_ptr<Drawing::Typeface> typeface0 =
        Drawing::Typeface::MakeFromFile("/system/fonts/HMOSColorEmojiCompat.ttf");
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromName(typeface0->GetFamilyName().c_str(), typeface0->GetFontStyle());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetItalic=" + std::to_string(typeface->GetItalic());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3188
DEF_DTK(typeface_makefromname, TestLevel::L2, 188)
{
    std::shared_ptr<Drawing::Typeface> typeface0 =
        Drawing::Typeface::MakeFromFile("/system/fonts/HMOSColorEmojiCompat.ttf");
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromName(typeface0->GetFamilyName().c_str(), typeface0->GetFontStyle());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetUniqueID=" + std::to_string(typeface->GetUniqueID());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3189
DEF_DTK(typeface_makefromname, TestLevel::L2, 189)
{
    std::shared_ptr<Drawing::Typeface> typeface0 =
        Drawing::Typeface::MakeFromFile("/system/fonts/HMOSColorEmojiCompat.ttf");
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromName(typeface0->GetFamilyName().c_str(), typeface0->GetFontStyle());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetUnitsPerEm=" + std::to_string(typeface->GetUnitsPerEm());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3190
DEF_DTK(typeface_makefromname, TestLevel::L2, 190)
{
    std::shared_ptr<Drawing::Typeface> typeface0 =
        Drawing::Typeface::MakeFromFile("/system/fonts/HMOSColorEmojiCompat.ttf");
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromName(typeface0->GetFamilyName().c_str(), typeface0->GetFontStyle());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "IsCustomTypeface=" +  std::to_string(typeface->IsCustomTypeface());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3191
DEF_DTK(typeface_makefromname, TestLevel::L2, 191)
{
    std::shared_ptr<Drawing::Typeface> typeface0 =
        Drawing::Typeface::MakeFromFile("/system/fonts/HMOSColorEmojiCompat.ttf");
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromName(typeface0->GetFamilyName().c_str(), typeface0->GetFontStyle());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetHash=" + std::to_string(typeface->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3192
DEF_DTK(typeface_makefromname, TestLevel::L2, 192)
{
    std::shared_ptr<Drawing::Typeface> typeface0 =
        Drawing::Typeface::MakeFromFile("/system/fonts/HMOSColorEmojiCompat.ttf");
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromName(typeface0->GetFamilyName().c_str(), typeface0->GetFontStyle());
    auto data = typeface->Serialize();
    auto typeface1 = Drawing::Typeface::Deserialize(data->GetData(), typeface->GetSize());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetHash=" + std::to_string(typeface1->GetHash());
    std::string text5 = "GetHash=" + std::to_string(typeface->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4, text5};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3193
DEF_DTK(typeface_makefromname, TestLevel::L2, 193)
{
    std::shared_ptr<Drawing::Typeface> typeface0 =
        Drawing::Typeface::MakeFromFile("/system/fonts/SanJiMengMengCaiSeJianTi-2.ttf");
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromName(typeface0->GetFamilyName().c_str(), typeface0->GetFontStyle());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetFamilyName=" + typeface->GetFamilyName();
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3194
DEF_DTK(typeface_makefromname, TestLevel::L2, 194)
{
    std::shared_ptr<Drawing::Typeface> typeface0 =
        Drawing::Typeface::MakeFromFile("/system/fonts/SanJiMengMengCaiSeJianTi-2.ttf");
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromName(typeface0->GetFamilyName().c_str(), typeface0->GetFontStyle());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetHash=" + std::to_string(typeface0->GetHash());
    std::string text5 = "GetHash=" + std::to_string(typeface->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4, text5};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3195
DEF_DTK(typeface_makefromname, TestLevel::L2, 195)
{
    std::shared_ptr<Drawing::Typeface> typeface0 =
        Drawing::Typeface::MakeFromFile("/system/fonts/SanJiMengMengCaiSeJianTi-2.ttf");
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromName(typeface0->GetFamilyName().c_str(), typeface0->GetFontStyle());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("DSIG");
    std::string text4 = "GetTableSize=" + std::to_string(typeface->GetTableSize(tagid));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3196
DEF_DTK(typeface_makefromname, TestLevel::L2, 196)
{
    std::shared_ptr<Drawing::Typeface> typeface0 =
        Drawing::Typeface::MakeFromFile("/system/fonts/SanJiMengMengCaiSeJianTi-2.ttf");
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromName(typeface0->GetFamilyName().c_str(), typeface0->GetFontStyle());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("CPAL");
    std::string text4 = "GetTableSize=" + std::to_string(typeface->GetTableSize(tagid));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3197
DEF_DTK(typeface_makefromname, TestLevel::L2, 197)
{
    std::shared_ptr<Drawing::Typeface> typeface0 =
        Drawing::Typeface::MakeFromFile("/system/fonts/SanJiMengMengCaiSeJianTi-2.ttf");
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromName(typeface0->GetFamilyName().c_str(), typeface0->GetFontStyle());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("DSIG");
    int a = MakeOffset("011c50");
    char abc[100];
    std::string text4 = "GetTableData=" +
        std::to_string(typeface->GetTableData(tagid, a, typeface->GetTableSize(tagid), (void*)abc));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3198
DEF_DTK(typeface_makefromname, TestLevel::L2, 198)
{
    std::shared_ptr<Drawing::Typeface> typeface0 =
        Drawing::Typeface::MakeFromFile("/system/fonts/SanJiMengMengCaiSeJianTi-2.ttf");
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromName(typeface0->GetFamilyName().c_str(), typeface0->GetFontStyle());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("CPAL");
    int a = MakeOffset("015e7260");
    std::string text4 = "GetTableData=" +
        std::to_string(typeface->GetTableData(tagid, a, typeface->GetTableSize(tagid) + 1, nullptr));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3199
DEF_DTK(typeface_makefromname, TestLevel::L2, 199)
{
    std::shared_ptr<Drawing::Typeface> typeface0 =
        Drawing::Typeface::MakeFromFile("/system/fonts/SanJiMengMengCaiSeJianTi-2.ttf");
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromName(typeface0->GetFamilyName().c_str(), typeface0->GetFontStyle());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetItalic=" + std::to_string(typeface->GetItalic());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3200
DEF_DTK(typeface_makefromname, TestLevel::L2, 200)
{
    std::shared_ptr<Drawing::Typeface> typeface0 =
        Drawing::Typeface::MakeFromFile("/system/fonts/SanJiMengMengCaiSeJianTi-2.ttf");
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromName(typeface0->GetFamilyName().c_str(), typeface0->GetFontStyle());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetUniqueID=" + std::to_string(typeface->GetUniqueID());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3201
DEF_DTK(typeface_makefromname, TestLevel::L2, 201)
{
    std::shared_ptr<Drawing::Typeface> typeface0 =
        Drawing::Typeface::MakeFromFile("/system/fonts/SanJiMengMengCaiSeJianTi-2.ttf");
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromName(typeface0->GetFamilyName().c_str(), typeface0->GetFontStyle());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetUnitsPerEm=" + std::to_string(typeface->GetUnitsPerEm());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3202
DEF_DTK(typeface_makefromname, TestLevel::L2, 202)
{
    std::shared_ptr<Drawing::Typeface> typeface0 =
        Drawing::Typeface::MakeFromFile("/system/fonts/SanJiMengMengCaiSeJianTi-2.ttf");
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromName(typeface0->GetFamilyName().c_str(), typeface0->GetFontStyle());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "IsCustomTypeface=" +  std::to_string(typeface->IsCustomTypeface());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3203
DEF_DTK(typeface_makefromname, TestLevel::L2, 203)
{
    std::shared_ptr<Drawing::Typeface> typeface0 =
        Drawing::Typeface::MakeFromFile("/system/fonts/SanJiMengMengCaiSeJianTi-2.ttf");
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromName(typeface0->GetFamilyName().c_str(), typeface0->GetFontStyle());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetHash=" + std::to_string(typeface->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3204
DEF_DTK(typeface_makefromname, TestLevel::L2, 204)
{
    std::shared_ptr<Drawing::Typeface> typeface0 =
        Drawing::Typeface::MakeFromFile("/system/fonts/SanJiMengMengCaiSeJianTi-2.ttf");
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromName(typeface0->GetFamilyName().c_str(), typeface0->GetFontStyle());
    auto data = typeface->Serialize();
    auto typeface1 = Drawing::Typeface::Deserialize(data->GetData(), typeface->GetSize());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetHash=" + std::to_string(typeface1->GetHash());
    std::string text5 = "GetHash=" + std::to_string(typeface->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4, text5};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3205
DEF_DTK(typeface_makefromname, TestLevel::L2, 205)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromName("family_name",
        {
            Drawing::FontStyle::BOLD_WEIGHT,
            Drawing::FontStyle::NORMAL_WIDTH,
            Drawing::FontStyle::ITALIC_SLANT
        });
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetFamilyName=" + typeface->GetFamilyName();
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3206
DEF_DTK(typeface_makefromname, TestLevel::L2, 206)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromName("family_name",
        {
            Drawing::FontStyle::BOLD_WEIGHT,
            Drawing::FontStyle::NORMAL_WIDTH,
            Drawing::FontStyle::ITALIC_SLANT
        });
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text5 = "GetHash=" + std::to_string(typeface->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text5};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3207
DEF_DTK(typeface_makefromname, TestLevel::L2, 207)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromName("family_name",
        {
            Drawing::FontStyle::BOLD_WEIGHT,
            Drawing::FontStyle::NORMAL_WIDTH,
            Drawing::FontStyle::ITALIC_SLANT
        });
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("DSIG");
    std::string text4 = "GetTableSize=" + std::to_string(typeface->GetTableSize(tagid));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3208
DEF_DTK(typeface_makefromname, TestLevel::L2, 208)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromName("family_name",
        {
            Drawing::FontStyle::BOLD_WEIGHT,
            Drawing::FontStyle::NORMAL_WIDTH,
            Drawing::FontStyle::ITALIC_SLANT
        });
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("CPAL");
    std::string text4 = "GetTableSize=" + std::to_string(typeface->GetTableSize(tagid));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3209
DEF_DTK(typeface_makefromname, TestLevel::L2, 209)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromName("family_name",
        {
            Drawing::FontStyle::BOLD_WEIGHT,
            Drawing::FontStyle::NORMAL_WIDTH,
            Drawing::FontStyle::ITALIC_SLANT
        });
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("DSIG");
    int a = MakeOffset("011c50");
    char abc[100];
    std::string text4 = "GetTableData=" +
        std::to_string(typeface->GetTableData(tagid, a, typeface->GetTableSize(tagid), (void*)abc));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3210
DEF_DTK(typeface_makefromname, TestLevel::L2, 210)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromName("family_name",
        {
            Drawing::FontStyle::BOLD_WEIGHT,
            Drawing::FontStyle::NORMAL_WIDTH,
            Drawing::FontStyle::ITALIC_SLANT
        });
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("CPAL");
    int a = MakeOffset("015e7260");
    std::string text4 = "GetTableData=" +
        std::to_string(typeface->GetTableData(tagid, a, typeface->GetTableSize(tagid) + 1, nullptr));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3211
DEF_DTK(typeface_makefromname, TestLevel::L2, 211)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromName("family_name",
        {
            Drawing::FontStyle::BOLD_WEIGHT,
            Drawing::FontStyle::NORMAL_WIDTH,
            Drawing::FontStyle::ITALIC_SLANT
        });
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetItalic=" + std::to_string(typeface->GetItalic());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3212
DEF_DTK(typeface_makefromname, TestLevel::L2, 212)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromName("family_name",
        {
            Drawing::FontStyle::BOLD_WEIGHT,
            Drawing::FontStyle::NORMAL_WIDTH,
            Drawing::FontStyle::ITALIC_SLANT
        });
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetUniqueID=" + std::to_string(typeface->GetUniqueID());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3213
DEF_DTK(typeface_makefromname, TestLevel::L2, 213)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromName("family_name",
        {
            Drawing::FontStyle::BOLD_WEIGHT,
            Drawing::FontStyle::NORMAL_WIDTH,
            Drawing::FontStyle::ITALIC_SLANT
        });
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetUnitsPerEm=" + std::to_string(typeface->GetUnitsPerEm());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3214
DEF_DTK(typeface_makefromname, TestLevel::L2, 214)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromName("family_name",
        {
            Drawing::FontStyle::BOLD_WEIGHT,
            Drawing::FontStyle::NORMAL_WIDTH,
            Drawing::FontStyle::ITALIC_SLANT
        });
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "IsCustomTypeface=" +  std::to_string(typeface->IsCustomTypeface());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3215
DEF_DTK(typeface_makefromname, TestLevel::L2, 215)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromName("family_name",
        {
            Drawing::FontStyle::BOLD_WEIGHT,
            Drawing::FontStyle::NORMAL_WIDTH,
            Drawing::FontStyle::ITALIC_SLANT
        });
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetHash=" + std::to_string(typeface->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3216
DEF_DTK(typeface_makefromname, TestLevel::L2, 216)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromName("family_name",
        {
            Drawing::FontStyle::BOLD_WEIGHT,
            Drawing::FontStyle::NORMAL_WIDTH,
            Drawing::FontStyle::ITALIC_SLANT
        });
    auto data = typeface->Serialize();
    auto typeface0 = Drawing::Typeface::Deserialize(data->GetData(), typeface->GetSize());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetHash=" + std::to_string(typeface0->GetHash());
    std::string text5 = "GetHash=" + std::to_string(typeface->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4, text5};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3217
DEF_DTK(typeface_makefromname, TestLevel::L2, 217)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromName("HarmonyOS Sans SC",
        {
            Drawing::FontStyle::EXTRA_BLACK_WEIGHT,
            Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
            Drawing::FontStyle::UPRIGHT_SLANT
        });
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetFamilyName=" + typeface->GetFamilyName();
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3218
DEF_DTK(typeface_makefromname, TestLevel::L2, 218)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromName("HarmonyOS Sans SC",
        {
            Drawing::FontStyle::EXTRA_BLACK_WEIGHT,
            Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
            Drawing::FontStyle::UPRIGHT_SLANT
        });
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text5 = "GetHash=" + std::to_string(typeface->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text5};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3219
DEF_DTK(typeface_makefromname, TestLevel::L2, 219)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromName("HarmonyOS Sans SC",
        {
            Drawing::FontStyle::EXTRA_BLACK_WEIGHT,
            Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
            Drawing::FontStyle::UPRIGHT_SLANT
        });
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("DSIG");
    std::string text4 = "GetTableSize=" + std::to_string(typeface->GetTableSize(tagid));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3220
DEF_DTK(typeface_makefromname, TestLevel::L2, 220)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromName("HarmonyOS Sans SC",
        {
            Drawing::FontStyle::EXTRA_BLACK_WEIGHT,
            Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
            Drawing::FontStyle::UPRIGHT_SLANT
        });
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("CPAL");
    std::string text4 = "GetTableSize=" + std::to_string(typeface->GetTableSize(tagid));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3221
DEF_DTK(typeface_makefromname, TestLevel::L2, 221)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromName("HarmonyOS Sans SC",
        {
            Drawing::FontStyle::EXTRA_BLACK_WEIGHT,
            Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
            Drawing::FontStyle::UPRIGHT_SLANT
        });
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("DSIG");
    int a = MakeOffset("011c50");
    char abc[100];
    std::string text4 = "GetTableData=" +
        std::to_string(typeface->GetTableData(tagid, a, typeface->GetTableSize(tagid), (void*)abc));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3222
DEF_DTK(typeface_makefromname, TestLevel::L2, 222)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromName("HarmonyOS Sans SC",
        {
            Drawing::FontStyle::EXTRA_BLACK_WEIGHT,
            Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
            Drawing::FontStyle::UPRIGHT_SLANT
        });
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("CPAL");
    int a = MakeOffset("015e7260");
    std::string text4 = "GetTableData=" +
        std::to_string(typeface->GetTableData(tagid, a, typeface->GetTableSize(tagid) + 1, nullptr));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3223
DEF_DTK(typeface_makefromname, TestLevel::L2, 223)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromName("HarmonyOS Sans SC",
        {
            Drawing::FontStyle::EXTRA_BLACK_WEIGHT,
            Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
            Drawing::FontStyle::UPRIGHT_SLANT
        });
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetItalic=" + std::to_string(typeface->GetItalic());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3224
DEF_DTK(typeface_makefromname, TestLevel::L2, 224)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromName("HarmonyOS Sans SC",
        {
            Drawing::FontStyle::EXTRA_BLACK_WEIGHT,
            Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
            Drawing::FontStyle::UPRIGHT_SLANT
        });
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetUniqueID=" + std::to_string(typeface->GetUniqueID());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3225
DEF_DTK(typeface_makefromname, TestLevel::L2, 225)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromName("HarmonyOS Sans SC",
        {
            Drawing::FontStyle::EXTRA_BLACK_WEIGHT,
            Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
            Drawing::FontStyle::UPRIGHT_SLANT
        });
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetUnitsPerEm=" + std::to_string(typeface->GetUnitsPerEm());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3226
DEF_DTK(typeface_makefromname, TestLevel::L2, 226)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromName("HarmonyOS Sans SC",
        {
            Drawing::FontStyle::EXTRA_BLACK_WEIGHT,
            Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
            Drawing::FontStyle::UPRIGHT_SLANT
        });
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "IsCustomTypeface=" +  std::to_string(typeface->IsCustomTypeface());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3227
DEF_DTK(typeface_makefromname, TestLevel::L2, 227)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromName("HarmonyOS Sans SC",
        {
            Drawing::FontStyle::EXTRA_BLACK_WEIGHT,
            Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
            Drawing::FontStyle::UPRIGHT_SLANT
        });
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetHash=" + std::to_string(typeface->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3228
DEF_DTK(typeface_makefromname, TestLevel::L2, 228)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromName("HarmonyOS Sans SC",
        {
            Drawing::FontStyle::EXTRA_BLACK_WEIGHT,
            Drawing::FontStyle::ULTRA_CONDENSED_WIDTH,
            Drawing::FontStyle::UPRIGHT_SLANT
        });
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
