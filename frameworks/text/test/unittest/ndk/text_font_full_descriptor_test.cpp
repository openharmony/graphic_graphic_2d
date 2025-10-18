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

#include "drawing_text_font_descriptor.h"
#include "file_ex.h"
#include "font_parser.h"
#include "gtest/gtest.h"
#include "string_ex.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace {
constexpr const char* SYMBOL_FILE = "/system/fonts/HMSymbolVF.ttf";
constexpr const char* SYMBOL_CONFIG_FILE = "/system/fonts/hm_symbol_config_next.json";
constexpr const char* CJK_FILE = "/system/fonts/NotoSansCJK-Regular.ttc";

#define CJK_SANS_DESC(lower, upper)                                                                                    \
    {                                                                                                                  \
        .path = "", .postScriptName = "NotoSansCJK" #lower "-Regular", .fullName = "Noto Sans CJK " #upper,            \
        .fontFamily = "Noto Sans CJK " #upper, .fontSubfamily = "Regular", .weight = 400, .width = 5, .italic = false, \
        .monoSpace = false, .symbolic = false                                                                          \
    }

#define CJK_MONO_DESC(lower, upper)                                                                                  \
    {                                                                                                                \
        .path = "", .postScriptName = "NotoSansMonoCJK" #lower "-Regular", .fullName = "Noto Sans Mono CJK " #upper, \
        .fontFamily = "Noto Sans Mono CJK " #upper, .fontSubfamily = "Regular", .weight = 400, .width = 5,           \
        .italic = false, .monoSpace = false, .symbolic = false                                                       \
    }

using FontDesc = Rosen::TextEngine::FontParser::FontDescriptor;
FontDesc SYMBOL_DESC = { .path = "",
    .postScriptName = "HMSymbol-Regular",
    .fullName = "HM Symbol Regular",
    .fontFamily = "HM Symbol",
    .fontSubfamily = "Regular",
    .weight = 400,
    .width = 5,
    .italic = false,
    .monoSpace = false,
    .symbolic = false };

std::vector<FontDesc> CJK_DESCS {
    CJK_SANS_DESC(jp, JP),
    CJK_SANS_DESC(kr, KR),
    CJK_SANS_DESC(sc, SC),
    CJK_SANS_DESC(tc, TC),
    CJK_SANS_DESC(hk, HK),
    CJK_MONO_DESC(jp, JP),
    CJK_MONO_DESC(kr, KR),
    CJK_MONO_DESC(sc, SC),
    CJK_MONO_DESC(tc, TC),
    CJK_MONO_DESC(hk, HK),
};
} // namespace

class NdkFontFullDescriptorTest : public testing::Test {};

/*
 * @tc.name: NdkFontFullDescriptorTest001
 * @tc.desc: test for OH_Drawing_GetFontFullDescriptorsFromStream with invalid param
 * @tc.type: FUNC
 */
HWTEST_F(NdkFontFullDescriptorTest, NdkFontFullDescriptorTest001, TestSize.Level0)
{
    OH_Drawing_Array* fontFullDescArr = OH_Drawing_GetFontFullDescriptorsFromStream(nullptr, 0);
    EXPECT_EQ(fontFullDescArr, nullptr);
    size_t num = OH_Drawing_GetDrawingArraySize(fontFullDescArr);
    EXPECT_EQ(num, 0);

    const OH_Drawing_FontFullDescriptor* desc = OH_Drawing_GetFontFullDescriptorByIndex(fontFullDescArr, 0);
    EXPECT_EQ(desc, nullptr);

    OH_Drawing_DestroyFontFullDescriptors(fontFullDescArr);
}

/*
 * @tc.name: NdkFontFullDescriptorTest002
 * @tc.desc: test for OH_Drawing_GetFontFullDescriptorsFromStream with SYMBOL_FILE
 * @tc.type: FUNC
 */
HWTEST_F(NdkFontFullDescriptorTest, NdkFontFullDescriptorTest002, TestSize.Level0)
{
    std::vector<char> content;
    EXPECT_TRUE(OHOS::LoadBufferFromFile(SYMBOL_FILE, content));
    OH_Drawing_Array* fontFullDescArr = OH_Drawing_GetFontFullDescriptorsFromStream(content.data(), content.size());
    EXPECT_NE(fontFullDescArr, nullptr);
    size_t size = OH_Drawing_GetDrawingArraySize(fontFullDescArr);
    EXPECT_EQ(size, 1);
    auto desc = OH_Drawing_GetFontFullDescriptorByIndex(fontFullDescArr, 0);
    EXPECT_NE(desc, nullptr);
    OH_Drawing_String str;
    OH_Drawing_GetFontFullDescriptorAttributeString(desc, FULL_DESCRIPTOR_ATTR_S_FULL_NAME, &str);
    EXPECT_EQ(SYMBOL_DESC.fullName,
        OHOS::Str16ToStr8(std::u16string(reinterpret_cast<char16_t*>(str.strData), str.strLen / sizeof(char16_t))));
    free(str.strData);
    OH_Drawing_GetFontFullDescriptorAttributeString(desc, FULL_DESCRIPTOR_ATTR_S_FAMILY_NAME, &str);
    EXPECT_EQ(SYMBOL_DESC.fontFamily,
        OHOS::Str16ToStr8(std::u16string(reinterpret_cast<char16_t*>(str.strData), str.strLen / sizeof(char16_t))));
    OH_Drawing_GetFontFullDescriptorAttributeString(desc, FULL_DESCRIPTOR_ATTR_S_POSTSCRIPT_NAME, &str);
    EXPECT_EQ(SYMBOL_DESC.postScriptName,
        OHOS::Str16ToStr8(std::u16string(reinterpret_cast<char16_t*>(str.strData), str.strLen / sizeof(char16_t))));
    free(str.strData);
    OH_Drawing_GetFontFullDescriptorAttributeString(desc, FULL_DESCRIPTOR_ATTR_S_PATH, &str);
    EXPECT_EQ(SYMBOL_DESC.path,
        OHOS::Str16ToStr8(std::u16string(reinterpret_cast<char16_t*>(str.strData), str.strLen / sizeof(char16_t))));
    free(str.strData);
    OH_Drawing_GetFontFullDescriptorAttributeString(desc, FULL_DESCRIPTOR_ATTR_S_SUB_FAMILY_NAME, &str);
    EXPECT_EQ(SYMBOL_DESC.fontSubfamily,
        OHOS::Str16ToStr8(std::u16string(reinterpret_cast<char16_t*>(str.strData), str.strLen / sizeof(char16_t))));
    free(str.strData);
    int32_t intAttr;
    OH_Drawing_GetFontFullDescriptorAttributeInt(desc, FULL_DESCRIPTOR_ATTR_I_WEIGHT, &intAttr);
    EXPECT_EQ(SYMBOL_DESC.weight, intAttr);
    OH_Drawing_GetFontFullDescriptorAttributeInt(desc, FULL_DESCRIPTOR_ATTR_I_WIDTH, &intAttr);
    EXPECT_EQ(SYMBOL_DESC.width, intAttr);
    OH_Drawing_GetFontFullDescriptorAttributeInt(desc, FULL_DESCRIPTOR_ATTR_I_ITALIC, &intAttr);
    EXPECT_EQ(SYMBOL_DESC.italic, intAttr);
    bool boolAttr;
    OH_Drawing_GetFontFullDescriptorAttributeBool(desc, FULL_DESCRIPTOR_ATTR_B_MONO, &boolAttr);
    EXPECT_EQ(SYMBOL_DESC.monoSpace, boolAttr);
    OH_Drawing_GetFontFullDescriptorAttributeBool(desc, FULL_DESCRIPTOR_ATTR_B_SYMBOLIC, &boolAttr);
    EXPECT_EQ(SYMBOL_DESC.symbolic, boolAttr);
}

/*
 * @tc.name: NdkFontFullDescriptorTest003
 * @tc.desc: test for OH_Drawing_GetFontFullDescriptorsFromStream with invalid ttf file
 * @tc.type: FUNC
 */
HWTEST_F(NdkFontFullDescriptorTest, NdkFontFullDescriptorTest003, TestSize.Level0)
{
    std::vector<char> content;
    EXPECT_TRUE(OHOS::LoadBufferFromFile(SYMBOL_FILE, content));
    for (int i = 0; i < 100 && i < content.size(); i++) {
        content[i] = '0';
    }
    OH_Drawing_Array* fontFullDescArr = OH_Drawing_GetFontFullDescriptorsFromStream(content.data(), content.size());
    EXPECT_EQ(fontFullDescArr, nullptr);
    size_t size = OH_Drawing_GetDrawingArraySize(fontFullDescArr);
    EXPECT_EQ(size, 0);
    OH_Drawing_DestroyFontFullDescriptors(fontFullDescArr);
}

/*
 * @tc.name: NdkFontFullDescriptorTest004
 * @tc.desc: test for OH_Drawing_GetFontFullDescriptorsFromStream with SYMBOL_CONFIG_FILE
 * @tc.type: FUNC
 */
HWTEST_F(NdkFontFullDescriptorTest, NdkFontFullDescriptorTest004, TestSize.Level0)
{
    std::vector<char> content;
    EXPECT_TRUE(OHOS::LoadBufferFromFile(SYMBOL_CONFIG_FILE, content));
    OH_Drawing_Array* fontFullDescArr = OH_Drawing_GetFontFullDescriptorsFromStream(content.data(), content.size());
    EXPECT_EQ(fontFullDescArr, nullptr);
    size_t size = OH_Drawing_GetDrawingArraySize(fontFullDescArr);
    EXPECT_EQ(size, 0);
    OH_Drawing_DestroyFontFullDescriptors(fontFullDescArr);
}

/*
 * @tc.name: NdkFontFullDescriptorTest005
 * @tc.desc: test for OH_Drawing_GetFontFullDescriptorsFromStream with CJK_FILE
 * @tc.type: FUNC
 */
HWTEST_F(NdkFontFullDescriptorTest, NdkFontFullDescriptorTest005, TestSize.Level0)
{
    std::vector<char> content;
    EXPECT_TRUE(OHOS::LoadBufferFromFile(CJK_FILE, content));
    OH_Drawing_Array* fontFullDescArr = OH_Drawing_GetFontFullDescriptorsFromStream(content.data(), content.size());
    EXPECT_NE(fontFullDescArr, nullptr);
    size_t size = OH_Drawing_GetDrawingArraySize(fontFullDescArr);
    // CJK ttc has 10 ttf
    EXPECT_EQ(size, 10);
    for (size_t i = 0; i < size; i++) {
        auto desc = OH_Drawing_GetFontFullDescriptorByIndex(fontFullDescArr, i);
        EXPECT_NE(desc, nullptr);
        OH_Drawing_String str;
        OH_Drawing_GetFontFullDescriptorAttributeString(desc, FULL_DESCRIPTOR_ATTR_S_FULL_NAME, &str);
        EXPECT_EQ(CJK_DESCS[i].fullName,
            OHOS::Str16ToStr8(std::u16string(reinterpret_cast<char16_t*>(str.strData), str.strLen / sizeof(char16_t))));
        free(str.strData);
        OH_Drawing_GetFontFullDescriptorAttributeString(desc, FULL_DESCRIPTOR_ATTR_S_FAMILY_NAME, &str);
        EXPECT_EQ(CJK_DESCS[i].fontFamily,
            OHOS::Str16ToStr8(std::u16string(reinterpret_cast<char16_t*>(str.strData), str.strLen / sizeof(char16_t))));
        OH_Drawing_GetFontFullDescriptorAttributeString(desc, FULL_DESCRIPTOR_ATTR_S_POSTSCRIPT_NAME, &str);
        EXPECT_EQ(CJK_DESCS[i].postScriptName,
            OHOS::Str16ToStr8(std::u16string(reinterpret_cast<char16_t*>(str.strData), str.strLen / sizeof(char16_t))));
        free(str.strData);
        OH_Drawing_GetFontFullDescriptorAttributeString(desc, FULL_DESCRIPTOR_ATTR_S_PATH, &str);
        EXPECT_EQ(CJK_DESCS[i].path,
            OHOS::Str16ToStr8(std::u16string(reinterpret_cast<char16_t*>(str.strData), str.strLen / sizeof(char16_t))));
        free(str.strData);
        OH_Drawing_GetFontFullDescriptorAttributeString(desc, FULL_DESCRIPTOR_ATTR_S_SUB_FAMILY_NAME, &str);
        EXPECT_EQ(CJK_DESCS[i].fontSubfamily,
            OHOS::Str16ToStr8(std::u16string(reinterpret_cast<char16_t*>(str.strData), str.strLen / sizeof(char16_t))));
        free(str.strData);
        int32_t intAttr;
        OH_Drawing_GetFontFullDescriptorAttributeInt(desc, FULL_DESCRIPTOR_ATTR_I_WEIGHT, &intAttr);
        EXPECT_EQ(CJK_DESCS[i].weight, intAttr);
        OH_Drawing_GetFontFullDescriptorAttributeInt(desc, FULL_DESCRIPTOR_ATTR_I_WIDTH, &intAttr);
        EXPECT_EQ(CJK_DESCS[i].width, intAttr);
        OH_Drawing_GetFontFullDescriptorAttributeInt(desc, FULL_DESCRIPTOR_ATTR_I_ITALIC, &intAttr);
        EXPECT_EQ(CJK_DESCS[i].italic, intAttr);
        bool boolAttr;
        OH_Drawing_GetFontFullDescriptorAttributeBool(desc, FULL_DESCRIPTOR_ATTR_B_MONO, &boolAttr);
        EXPECT_EQ(CJK_DESCS[i].monoSpace, boolAttr);
        OH_Drawing_GetFontFullDescriptorAttributeBool(desc, FULL_DESCRIPTOR_ATTR_B_SYMBOLIC, &boolAttr);
        EXPECT_EQ(CJK_DESCS[i].symbolic, boolAttr);
    }
}
} // namespace OHOS