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
测试接口：MakeFromFile
测试内容：对接口入参file文件路径和index取值组合，构造typeface字体格式，并指定在font上，最终通过drawtextblob接口将text内容绘制在画布上
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

//对应用例 TypeFace_3013
DEF_DTK(typeface_makefromfile, TestLevel::L2, 13)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/SanJiMengMengCaiSeJianTi-2.ttf");
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetFamilyName=" + typeface->GetFamilyName();
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3014
DEF_DTK(typeface_makefromfile, TestLevel::L2, 14)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/SanJiMengMengCaiSeJianTi-2.ttf");
    std::shared_ptr<Drawing::Typeface> typeface0 =
        Drawing::Typeface::MakeFromName(typeface->GetFamilyName().c_str(), typeface->GetFontStyle());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetHash=" + std::to_string(typeface0->GetHash());
    std::string text5 = "GetHash=" + std::to_string(typeface->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4, text5};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3015
DEF_DTK(typeface_makefromfile, TestLevel::L2, 15)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/SanJiMengMengCaiSeJianTi-2.ttf");
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("DSIG");
    std::string text4 = "GetTableSize=" + std::to_string(typeface->GetTableSize(tagid));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3016
DEF_DTK(typeface_makefromfile, TestLevel::L2, 16)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/SanJiMengMengCaiSeJianTi-2.ttf");
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("CPAL");
    std::string text4 = "GetTableSize=" + std::to_string(typeface->GetTableSize(tagid));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3017
DEF_DTK(typeface_makefromfile, TestLevel::L2, 17)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/SanJiMengMengCaiSeJianTi-2.ttf");
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("DSIG");
    int a = MakeOffset("011c50");
    char abc[100];
    std::string text4 = "GetTableData=" +
        std::to_string(typeface->GetTableData(tagid, a, typeface->GetTableSize(tagid), (void*)abc));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3018
DEF_DTK(typeface_makefromfile, TestLevel::L2, 18)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/SanJiMengMengCaiSeJianTi-2.ttf");
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("CPAL");
    int a = MakeOffset("015e7260");
    std::string text4 = "GetTableData=" +
        std::to_string(typeface->GetTableData(tagid, a, typeface->GetTableSize(tagid) + 1, nullptr));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3019
DEF_DTK(typeface_makefromfile, TestLevel::L2, 19)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/SanJiMengMengCaiSeJianTi-2.ttf");
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetItalic=" + std::to_string(typeface->GetItalic());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3020
DEF_DTK(typeface_makefromfile, TestLevel::L2, 20)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/SanJiMengMengCaiSeJianTi-2.ttf");
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetUniqueID=" + std::to_string(typeface->GetUniqueID());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3021
DEF_DTK(typeface_makefromfile, TestLevel::L2, 21)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/SanJiMengMengCaiSeJianTi-2.ttf");
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetUnitsPerEm=" + std::to_string(typeface->GetUnitsPerEm());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3022
DEF_DTK(typeface_makefromfile, TestLevel::L2, 22)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/SanJiMengMengCaiSeJianTi-2.ttf");
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "IsCustomTypeface=" +  std::to_string(typeface->IsCustomTypeface());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3023
DEF_DTK(typeface_makefromfile, TestLevel::L2, 23)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/SanJiMengMengCaiSeJianTi-2.ttf");
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetHash=" + std::to_string(typeface->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3024
DEF_DTK(typeface_makefromfile, TestLevel::L2, 24)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/SanJiMengMengCaiSeJianTi-2.ttf");
    auto data = typeface->Serialize();
    auto typeface0 = Drawing::Typeface::Deserialize(data->GetData(), typeface->GetSize());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetHash=" + std::to_string(typeface0->GetHash());
    std::string text5 = "GetHash=" + std::to_string(typeface->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4, text5};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3025
DEF_DTK(typeface_makefromfile, TestLevel::L2, 25)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HMOSColorEmojiCompat.ttf");
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetFamilyName=" + typeface->GetFamilyName();
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3026
DEF_DTK(typeface_makefromfile, TestLevel::L2, 26)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HMOSColorEmojiCompat.ttf");
    std::shared_ptr<Drawing::Typeface> typeface0 =
        Drawing::Typeface::MakeFromName(typeface->GetFamilyName().c_str(), typeface->GetFontStyle());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetHash=" + std::to_string(typeface0->GetHash());
    std::string text5 = "GetHash=" + std::to_string(typeface->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4, text5};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3027
DEF_DTK(typeface_makefromfile, TestLevel::L2, 27)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HMOSColorEmojiCompat.ttf");
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("DSIG");
    std::string text4 = "GetTableSize=" + std::to_string(typeface->GetTableSize(tagid));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3028
DEF_DTK(typeface_makefromfile, TestLevel::L2, 28)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HMOSColorEmojiCompat.ttf");
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("CPAL");
    std::string text4 = "GetTableSize=" + std::to_string(typeface->GetTableSize(tagid));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3029
DEF_DTK(typeface_makefromfile, TestLevel::L2, 29)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HMOSColorEmojiCompat.ttf");
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("DSIG");
    int a = MakeOffset("011c50");
    char abc[100];
    std::string text4 = "GetTableData=" +
        std::to_string(typeface->GetTableData(tagid, a, typeface->GetTableSize(tagid), (void*)abc));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3030
DEF_DTK(typeface_makefromfile, TestLevel::L2, 30)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HMOSColorEmojiCompat.ttf");
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("CPAL");
    int a = MakeOffset("015e7260");
    std::string text4 = "GetTableData=" +
        std::to_string(typeface->GetTableData(tagid, a, typeface->GetTableSize(tagid) + 1, nullptr));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3031
DEF_DTK(typeface_makefromfile, TestLevel::L2, 31)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HMOSColorEmojiCompat.ttf");
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetItalic=" + std::to_string(typeface->GetItalic());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3032
DEF_DTK(typeface_makefromfile, TestLevel::L2, 32)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HMOSColorEmojiCompat.ttf");
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetUniqueID=" + std::to_string(typeface->GetUniqueID());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3033
DEF_DTK(typeface_makefromfile, TestLevel::L2, 33)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HMOSColorEmojiCompat.ttf");
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetUnitsPerEm=" + std::to_string(typeface->GetUnitsPerEm());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3034
DEF_DTK(typeface_makefromfile, TestLevel::L2, 34)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HMOSColorEmojiCompat.ttf");
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "IsCustomTypeface=" +  std::to_string(typeface->IsCustomTypeface());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3035
DEF_DTK(typeface_makefromfile, TestLevel::L2, 35)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HMOSColorEmojiCompat.ttf");
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetHash=" + std::to_string(typeface->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3036
DEF_DTK(typeface_makefromfile, TestLevel::L2, 36)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HMOSColorEmojiCompat.ttf");
    auto data = typeface->Serialize();
    auto typeface0 = Drawing::Typeface::Deserialize(data->GetData(), typeface->GetSize());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetHash=" + std::to_string(typeface0->GetHash());
    std::string text5 = "GetHash=" + std::to_string(typeface->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4, text5};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3037
DEF_DTK(typeface_makefromfile, TestLevel::L2, 37)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromFile("/system/fonts/HarmonyOS_Sans.ttf");
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetFamilyName=" + typeface->GetFamilyName();
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3038
DEF_DTK(typeface_makefromfile, TestLevel::L2, 38)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromFile("/system/fonts/HarmonyOS_Sans.ttf");
    std::shared_ptr<Drawing::Typeface> typeface0 =
        Drawing::Typeface::MakeFromName(typeface->GetFamilyName().c_str(), typeface->GetFontStyle());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetHash=" + std::to_string(typeface0->GetHash());
    std::string text5 = "GetHash=" + std::to_string(typeface->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4, text5};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3039
DEF_DTK(typeface_makefromfile, TestLevel::L2, 39)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromFile("/system/fonts/HarmonyOS_Sans.ttf");
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("DSIG");
    std::string text4 = "GetTableSize=" + std::to_string(typeface->GetTableSize(tagid));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3040
DEF_DTK(typeface_makefromfile, TestLevel::L2, 40)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromFile("/system/fonts/HarmonyOS_Sans.ttf");
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("CPAL");
    std::string text4 = "GetTableSize=" + std::to_string(typeface->GetTableSize(tagid));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3041
DEF_DTK(typeface_makefromfile, TestLevel::L2, 41)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromFile("/system/fonts/HarmonyOS_Sans.ttf");
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("DSIG");
    int a = MakeOffset("011c50");
    char abc[100];
    std::string text4 = "GetTableData=" +
        std::to_string(typeface->GetTableData(tagid, a, typeface->GetTableSize(tagid), (void*)abc));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3042
DEF_DTK(typeface_makefromfile, TestLevel::L2, 42)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromFile("/system/fonts/HarmonyOS_Sans.ttf");
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("CPAL");
    int a = MakeOffset("015e7260");
    std::string text4 = "GetTableData=" +
        std::to_string(typeface->GetTableData(tagid, a, typeface->GetTableSize(tagid) + 1, nullptr));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3043
DEF_DTK(typeface_makefromfile, TestLevel::L2, 43)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromFile("/system/fonts/HarmonyOS_Sans.ttf");
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetItalic=" + std::to_string(typeface->GetItalic());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3044
DEF_DTK(typeface_makefromfile, TestLevel::L2, 44)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromFile("/system/fonts/HarmonyOS_Sans.ttf");
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetUniqueID=" + std::to_string(typeface->GetUniqueID());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3045
DEF_DTK(typeface_makefromfile, TestLevel::L2, 45)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromFile("/system/fonts/HarmonyOS_Sans.ttf");
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetUnitsPerEm=" + std::to_string(typeface->GetUnitsPerEm());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3046
DEF_DTK(typeface_makefromfile, TestLevel::L2, 46)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromFile("/system/fonts/HarmonyOS_Sans.ttf");
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "IsCustomTypeface=" +  std::to_string(typeface->IsCustomTypeface());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3047
DEF_DTK(typeface_makefromfile, TestLevel::L2, 47)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromFile("/system/fonts/HarmonyOS_Sans.ttf");
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetHash=" + std::to_string(typeface->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3048
DEF_DTK(typeface_makefromfile, TestLevel::L2, 48)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromFile("/system/fonts/HarmonyOS_Sans.ttf");
    auto data = typeface->Serialize();
    auto typeface0 = Drawing::Typeface::Deserialize(data->GetData(), typeface->GetSize());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetHash=" + std::to_string(typeface0->GetHash());
    std::string text5 = "GetHash=" + std::to_string(typeface->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4, text5};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3049
DEF_DTK(typeface_makefromfile, TestLevel::L2, 49)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromFile("/system/fonts/test.ttf");
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetFamilyName=" + typeface->GetFamilyName();
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3050
DEF_DTK(typeface_makefromfile, TestLevel::L2, 50)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromFile("/system/fonts/test.ttf");
    std::shared_ptr<Drawing::Typeface> typeface0 =
        Drawing::Typeface::MakeFromName(typeface->GetFamilyName().c_str(), typeface->GetFontStyle());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetHash=" + std::to_string(typeface0->GetHash());
    std::string text5 = "GetHash=" + std::to_string(typeface->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4, text5};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3051
DEF_DTK(typeface_makefromfile, TestLevel::L2, 51)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromFile("/system/fonts/test.ttf");
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("DSIG");
    std::string text4 = "GetTableSize=" + std::to_string(typeface->GetTableSize(tagid));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3052
DEF_DTK(typeface_makefromfile, TestLevel::L2, 52)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromFile("/system/fonts/test.ttf");
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("CPAL");
    std::string text4 = "GetTableSize=" + std::to_string(typeface->GetTableSize(tagid));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3053
DEF_DTK(typeface_makefromfile, TestLevel::L2, 53)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromFile("/system/fonts/test.ttf");
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("DSIG");
    int a = MakeOffset("011c50");
    char abc[100];
    std::string text4 = "GetTableData=" +
        std::to_string(typeface->GetTableData(tagid, a, typeface->GetTableSize(tagid), (void*)abc));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3054
DEF_DTK(typeface_makefromfile, TestLevel::L2, 54)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromFile("/system/fonts/test.ttf");
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("CPAL");
    int a = MakeOffset("015e7260");
    std::string text4 = "GetTableData=" +
        std::to_string(typeface->GetTableData(tagid, a, typeface->GetTableSize(tagid) + 1, nullptr));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3055
DEF_DTK(typeface_makefromfile, TestLevel::L2, 55)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromFile("/system/fonts/test.ttf");
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetItalic=" + std::to_string(typeface->GetItalic());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3056
DEF_DTK(typeface_makefromfile, TestLevel::L2, 56)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromFile("/system/fonts/test.ttf");
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetUniqueID=" + std::to_string(typeface->GetUniqueID());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3057
DEF_DTK(typeface_makefromfile, TestLevel::L2, 57)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromFile("/system/fonts/test.ttf");
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetUnitsPerEm=" + std::to_string(typeface->GetUnitsPerEm());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3058
DEF_DTK(typeface_makefromfile, TestLevel::L2, 58)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromFile("/system/fonts/test.ttf");
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "IsCustomTypeface=" +  std::to_string(typeface->IsCustomTypeface());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3059
DEF_DTK(typeface_makefromfile, TestLevel::L2, 59)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromFile("/system/fonts/test.ttf");
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetHash=" + std::to_string(typeface->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3060
DEF_DTK(typeface_makefromfile, TestLevel::L2, 60)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromFile("/system/fonts/test.ttf");
    auto data = typeface->Serialize();
    auto typeface0 = Drawing::Typeface::Deserialize(data->GetData(), typeface->GetSize());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetHash=" + std::to_string(typeface0->GetHash());
    std::string text5 = "GetHash=" + std::to_string(typeface->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4, text5};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3061
DEF_DTK(typeface_makefromfile, TestLevel::L2, 61)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromFile("/system/app");
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetFamilyName=" + typeface->GetFamilyName();
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3062
DEF_DTK(typeface_makefromfile, TestLevel::L2, 62)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromFile("/system/app");
    std::shared_ptr<Drawing::Typeface> typeface0 =
        Drawing::Typeface::MakeFromName(typeface->GetFamilyName().c_str(), typeface->GetFontStyle());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetHash=" + std::to_string(typeface0->GetHash());
    std::string text5 = "GetHash=" + std::to_string(typeface->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4, text5};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3063
DEF_DTK(typeface_makefromfile, TestLevel::L2, 63)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromFile("/system/app");
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("DSIG");
    std::string text4 = "GetTableSize=" + std::to_string(typeface->GetTableSize(tagid));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3064
DEF_DTK(typeface_makefromfile, TestLevel::L2, 64)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromFile("/system/app");
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("CPAL");
    std::string text4 = "GetTableSize=" + std::to_string(typeface->GetTableSize(tagid));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3065
DEF_DTK(typeface_makefromfile, TestLevel::L2, 65)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromFile("/system/app");
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("DSIG");
    int a = MakeOffset("011c50");
    char abc[100];
    std::string text4 = "GetTableData=" +
        std::to_string(typeface->GetTableData(tagid, a, typeface->GetTableSize(tagid), (void*)abc));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3066
DEF_DTK(typeface_makefromfile, TestLevel::L2, 66)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromFile("/system/app");
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("CPAL");
    int a = MakeOffset("015e7260");
    std::string text4 = "GetTableData=" +
        std::to_string(typeface->GetTableData(tagid, a, typeface->GetTableSize(tagid) + 1, nullptr));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3067
DEF_DTK(typeface_makefromfile, TestLevel::L2, 67)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromFile("/system/app");
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetItalic=" + std::to_string(typeface->GetItalic());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3068
DEF_DTK(typeface_makefromfile, TestLevel::L2, 68)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromFile("/system/app");
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetUniqueID=" + std::to_string(typeface->GetUniqueID());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3069
DEF_DTK(typeface_makefromfile, TestLevel::L2, 69)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromFile("/system/app");
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetUnitsPerEm=" + std::to_string(typeface->GetUnitsPerEm());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3070
DEF_DTK(typeface_makefromfile, TestLevel::L2, 70)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromFile("/system/app");
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "IsCustomTypeface=" +  std::to_string(typeface->IsCustomTypeface());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3071
DEF_DTK(typeface_makefromfile, TestLevel::L2, 71)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromFile("/system/app");
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetHash=" + std::to_string(typeface->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3072
DEF_DTK(typeface_makefromfile, TestLevel::L2, 72)
{
    std::shared_ptr<Drawing::Typeface> typeface = Drawing::Typeface::MakeFromFile("/system/app");
    auto data = typeface->Serialize();
    auto typeface0 = Drawing::Typeface::Deserialize(data->GetData(), typeface->GetSize());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetHash=" + std::to_string(typeface0->GetHash());
    std::string text5 = "GetHash=" + std::to_string(typeface->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4, text5};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3073
DEF_DTK(typeface_makefromfile, TestLevel::L2, 73)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HarmonyOS_Sans.ttf", 0);
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetFamilyName=" + typeface->GetFamilyName();
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3074
DEF_DTK(typeface_makefromfile, TestLevel::L2, 74)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HarmonyOS_Sans.ttf", 0);
    std::shared_ptr<Drawing::Typeface> typeface0 =
        Drawing::Typeface::MakeFromName(typeface->GetFamilyName().c_str(), typeface->GetFontStyle());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetHash=" + std::to_string(typeface0->GetHash());
    std::string text5 = "GetHash=" + std::to_string(typeface->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4, text5};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3075
DEF_DTK(typeface_makefromfile, TestLevel::L2, 75)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HarmonyOS_Sans.ttf", 0);
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("DSIG");
    std::string text4 = "GetTableSize=" + std::to_string(typeface->GetTableSize(tagid));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3076
DEF_DTK(typeface_makefromfile, TestLevel::L2, 76)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HarmonyOS_Sans.ttf", 0);
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("CPAL");
    std::string text4 = "GetTableSize=" + std::to_string(typeface->GetTableSize(tagid));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3077
DEF_DTK(typeface_makefromfile, TestLevel::L2, 77)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HarmonyOS_Sans.ttf", 0);
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("DSIG");
    int a = MakeOffset("011c50");
    char abc[100];
    std::string text4 = "GetTableData=" +
        std::to_string(typeface->GetTableData(tagid, a, typeface->GetTableSize(tagid), (void*)abc));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3078
DEF_DTK(typeface_makefromfile, TestLevel::L2, 78)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HarmonyOS_Sans.ttf", 0);
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("CPAL");
    int a = MakeOffset("015e7260");
    std::string text4 = "GetTableData=" +
        std::to_string(typeface->GetTableData(tagid, a, typeface->GetTableSize(tagid) + 1, nullptr));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3079
DEF_DTK(typeface_makefromfile, TestLevel::L2, 79)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HarmonyOS_Sans.ttf", 0);
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetItalic=" + std::to_string(typeface->GetItalic());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3080
DEF_DTK(typeface_makefromfile, TestLevel::L2, 80)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HarmonyOS_Sans.ttf", 0);
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetUniqueID=" + std::to_string(typeface->GetUniqueID());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3081
DEF_DTK(typeface_makefromfile, TestLevel::L2, 81)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HarmonyOS_Sans.ttf", 0);
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetUnitsPerEm=" + std::to_string(typeface->GetUnitsPerEm());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3082
DEF_DTK(typeface_makefromfile, TestLevel::L2, 82)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HarmonyOS_Sans.ttf", 0);
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "IsCustomTypeface=" +  std::to_string(typeface->IsCustomTypeface());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3083
DEF_DTK(typeface_makefromfile, TestLevel::L2, 83)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HarmonyOS_Sans.ttf", 0);
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetHash=" + std::to_string(typeface->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3084
DEF_DTK(typeface_makefromfile, TestLevel::L2, 84)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HarmonyOS_Sans.ttf", 0);
    auto data = typeface->Serialize();
    auto typeface0 = Drawing::Typeface::Deserialize(data->GetData(), typeface->GetSize());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetHash=" + std::to_string(typeface0->GetHash());
    std::string text5 = "GetHash=" + std::to_string(typeface->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4, text5};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3085
DEF_DTK(typeface_makefromfile, TestLevel::L2, 85)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HarmonyOS_Sans.ttf", -5);
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetFamilyName=" + typeface->GetFamilyName();
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3086
DEF_DTK(typeface_makefromfile, TestLevel::L2, 86)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HarmonyOS_Sans.ttf", -5);
    std::shared_ptr<Drawing::Typeface> typeface0 =
        Drawing::Typeface::MakeFromName(typeface->GetFamilyName().c_str(), typeface->GetFontStyle());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetHash=" + std::to_string(typeface0->GetHash());
    std::string text5 = "GetHash=" + std::to_string(typeface->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4, text5};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3087
DEF_DTK(typeface_makefromfile, TestLevel::L2, 87)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HarmonyOS_Sans.ttf", -5);
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("DSIG");
    std::string text4 = "GetTableSize=" + std::to_string(typeface->GetTableSize(tagid));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3088
DEF_DTK(typeface_makefromfile, TestLevel::L2, 88)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HarmonyOS_Sans.ttf", -5);
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("CPAL");
    std::string text4 = "GetTableSize=" + std::to_string(typeface->GetTableSize(tagid));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3089
DEF_DTK(typeface_makefromfile, TestLevel::L2, 89)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HarmonyOS_Sans.ttf", -5);
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("DSIG");
    int a = MakeOffset("011c50");
    char abc[100];
    std::string text4 = "GetTableData=" +
        std::to_string(typeface->GetTableData(tagid, a, typeface->GetTableSize(tagid), (void*)abc));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3090
DEF_DTK(typeface_makefromfile, TestLevel::L2, 90)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HarmonyOS_Sans.ttf", -5);
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("CPAL");
    int a = MakeOffset("015e7260");
    std::string text4 = "GetTableData=" +
        std::to_string(typeface->GetTableData(tagid, a, typeface->GetTableSize(tagid) + 1, nullptr));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3091
DEF_DTK(typeface_makefromfile, TestLevel::L2, 91)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HarmonyOS_Sans.ttf", -5);
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetItalic=" + std::to_string(typeface->GetItalic());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3092
DEF_DTK(typeface_makefromfile, TestLevel::L2, 92)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HarmonyOS_Sans.ttf", -5);
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetUniqueID=" + std::to_string(typeface->GetUniqueID());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3093
DEF_DTK(typeface_makefromfile, TestLevel::L2, 93)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HarmonyOS_Sans.ttf", -5);
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetUnitsPerEm=" + std::to_string(typeface->GetUnitsPerEm());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3094
DEF_DTK(typeface_makefromfile, TestLevel::L2, 94)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HarmonyOS_Sans.ttf", -5);
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "IsCustomTypeface=" +  std::to_string(typeface->IsCustomTypeface());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3095
DEF_DTK(typeface_makefromfile, TestLevel::L2, 95)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HarmonyOS_Sans.ttf", -5);
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetHash=" + std::to_string(typeface->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3096
DEF_DTK(typeface_makefromfile, TestLevel::L2, 96)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HarmonyOS_Sans.ttf", -5);
    auto data = typeface->Serialize();
    auto typeface0 = Drawing::Typeface::Deserialize(data->GetData(), typeface->GetSize());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetHash=" + std::to_string(typeface0->GetHash());
    std::string text5 = "GetHash=" + std::to_string(typeface->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4, text5};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3097
DEF_DTK(typeface_makefromfile, TestLevel::L2, 97)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HarmonyOS_Sans.ttf", 100);
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetFamilyName=" + typeface->GetFamilyName();
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3098
DEF_DTK(typeface_makefromfile, TestLevel::L2, 98)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HarmonyOS_Sans.ttf", 100);
    std::shared_ptr<Drawing::Typeface> typeface0 =
        Drawing::Typeface::MakeFromName(typeface->GetFamilyName().c_str(), typeface->GetFontStyle());
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetHash=" + std::to_string(typeface0->GetHash());
    std::string text5 = "GetHash=" + std::to_string(typeface->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4, text5};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3099
DEF_DTK(typeface_makefromfile, TestLevel::L2, 99)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HarmonyOS_Sans.ttf", 100);
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("DSIG");
    std::string text4 = "GetTableSize=" + std::to_string(typeface->GetTableSize(tagid));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3100
DEF_DTK(typeface_makefromfile, TestLevel::L2, 100)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HarmonyOS_Sans.ttf", 100);
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("CPAL");
    std::string text4 = "GetTableSize=" + std::to_string(typeface->GetTableSize(tagid));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3101
DEF_DTK(typeface_makefromfile, TestLevel::L2, 101)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HarmonyOS_Sans.ttf", 100);
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("DSIG");
    int a = MakeOffset("011c50");
    char abc[100];
    std::string text4 = "GetTableData=" +
        std::to_string(typeface->GetTableData(tagid, a, typeface->GetTableSize(tagid), (void*)abc));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3102
DEF_DTK(typeface_makefromfile, TestLevel::L2, 102)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HarmonyOS_Sans.ttf", 100);
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    uint32_t tagid = MakeTagId("CPAL");
    int a = MakeOffset("015e7260");
    std::string text4 = "GetTableData=" +
        std::to_string(typeface->GetTableData(tagid, a, typeface->GetTableSize(tagid) + 1, nullptr));
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3103
DEF_DTK(typeface_makefromfile, TestLevel::L2, 103)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HarmonyOS_Sans.ttf", 100);
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetItalic=" + std::to_string(typeface->GetItalic());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3104
DEF_DTK(typeface_makefromfile, TestLevel::L2, 104)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HarmonyOS_Sans.ttf", 100);
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetUniqueID=" + std::to_string(typeface->GetUniqueID());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3105
DEF_DTK(typeface_makefromfile, TestLevel::L2, 105)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HarmonyOS_Sans.ttf", 100);
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetUnitsPerEm=" + std::to_string(typeface->GetUnitsPerEm());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3106
DEF_DTK(typeface_makefromfile, TestLevel::L2, 106)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HarmonyOS_Sans.ttf", 100);
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "IsCustomTypeface=" +  std::to_string(typeface->IsCustomTypeface());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3107
DEF_DTK(typeface_makefromfile, TestLevel::L2, 107)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HarmonyOS_Sans.ttf", 100);
    auto font = Drawing::Font(typeface, 50.f, 1.0f, 0.f);

    std::string text4 = "GetHash=" + std::to_string(typeface->GetHash());
    std::vector<std::string> texts = {g_text1, g_text2, g_text3, text4};

    DrawTexts(texts, font, playbackCanvas_);
}

//对应用例 TypeFace_3108
DEF_DTK(typeface_makefromfile, TestLevel::L2, 108)
{
    std::shared_ptr<Drawing::Typeface> typeface =
        Drawing::Typeface::MakeFromFile("/system/fonts/HarmonyOS_Sans.ttf", 100);
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
