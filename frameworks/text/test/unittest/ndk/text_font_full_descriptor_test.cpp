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
constexpr const char* NOTO_SANS_FILE = "/system/fonts/NotoSans[wdth,wght].ttf";
constexpr const char* HARMONYOS_SANS_FILE = "/system/fonts/HarmonyOS_Sans.ttf";

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

FontDesc NOTO_SANS_DESC = {
    .path = "",
    .postScriptName = "NotoSans-Regular",
    .fullName = "Noto Sans Regular",
    .fontFamily = "Noto Sans",
    .fontSubfamily = "Regular",
    .weight = 400,
    .width = 5,
    .italic = false,
    .monoSpace = false,
    .symbolic = false,
    .localPostscriptName = "NotoSans-Regular",
    .localFullName = "Noto Sans Regular",
    .localFamilyName = "Noto Sans",
    .localSubFamilyName = "Regular",
    .version = "Version 2.010",
    .manufacture = "Monotype Imaging Inc.",
    .copyright = "Copyright 2022 The Noto Project Authors (https://github.com/notofonts/latin-greek-cyrillic)",
    .license = "This Font Software is licensed under the SIL Open Font License,"
        " Version 1.1. This license is available with a FAQ at: https://scripts.sil.org/OFL",
    .index = 0
};

FontDesc HARMONYOS_SANS_DESC = {
    .path = "",
    .postScriptName = "HarmonyOS_Sans",
    .fullName = "HarmonyOS Sans",
    .fontFamily = "HarmonyOS Sans",
    .fontSubfamily = "Regular",
    .weight = 400,
    .width = 5,
    .italic = false,
    .monoSpace = false,
    .symbolic = false,
    .variationAxisRecords = {
        {
            .key = "wght",
            .minValue = 40.0,
            .maxValue = 900.0,
            .defaultValue = 400.0,
            .flags = 0,
            .name = "Weight",
            .localName = "",
        }
    },
    .variationInstanceRecords = {
        {
            .name = "Thin",
            .localName = "Thin",
            .coordinates = {{.axis = "wght", .value = 100.0}},
        },
        {
            .name = "UltraLight",
            .localName = "UltraLight",
            .coordinates = {{.axis = "wght", .value = 173.0}},
        },
        {
            .name = "Light",
            .localName = "Light",
            .coordinates = {{.axis = "wght", .value = 247.0}},
        },
        {
            .name = "Regular",
            .localName = "Regular",
            .coordinates = {{.axis = "wght", .value = 400.0}},
        },
        {
            .name = "Medium",
            .localName = "Medium",
            .coordinates = {{.axis = "wght", .value = 500.0}},
        },
        {
            .name = "SemiBold",
            .localName = "SemiBold",
            .coordinates = {{.axis = "wght", .value = 603.0}},
        },
        {
            .name = "Bold",
            .localName = "Bold",
            .coordinates = {{.axis = "wght", .value = 706.0}},
        },
        {
            .name = "Heavy",
            .localName = "Heavy",
            .coordinates = {{.axis = "wght", .value = 772.0}},
        },
        {
            .name = "Black",
            .localName = "Black",
            .coordinates = {{.axis = "wght", .value = 844.0}},
        },
    },
};

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
    std::vector<char> content;
    OH_Drawing_Array* fontFullDescArr = OH_Drawing_GetFontFullDescriptorsFromStream(nullptr, 1);
    EXPECT_EQ(fontFullDescArr, nullptr);
    fontFullDescArr = OH_Drawing_GetFontFullDescriptorsFromStream(content.data(), content.size());
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

/*
 * @tc.name: NdkFontFullDescriptorTest006
 * @tc.desc: test for OH_Drawing_GetFontFullDescriptorAttributeString with local attribute id
 * @tc.type: FUNC
 */
HWTEST_F(NdkFontFullDescriptorTest, NdkFontFullDescriptorTest006, TestSize.Level0)
{
    OH_Drawing_Array* fontFullDescArr = OH_Drawing_GetFontFullDescriptorsFromPath(NOTO_SANS_FILE);
    EXPECT_NE(fontFullDescArr, nullptr);
    size_t size = OH_Drawing_GetDrawingArraySize(fontFullDescArr);
    EXPECT_EQ(size, 1);
    auto desc = OH_Drawing_GetFontFullDescriptorByIndex(fontFullDescArr, 0);
    EXPECT_NE(desc, nullptr);

    OH_Drawing_String str;
    OH_Drawing_GetFontFullDescriptorAttributeString(desc, FULL_DESCRIPTOR_ATTR_S_LOCAL_POSTSCRIPT_NAME, &str);
    EXPECT_EQ(NOTO_SANS_DESC.localPostscriptName,
        OHOS::Str16ToStr8(std::u16string(reinterpret_cast<char16_t*>(str.strData), str.strLen / sizeof(char16_t))));
    free(str.strData);

    OH_Drawing_GetFontFullDescriptorAttributeString(desc, FULL_DESCRIPTOR_ATTR_S_LOCAL_FULL_NAME, &str);
    EXPECT_EQ(NOTO_SANS_DESC.localFullName,
        OHOS::Str16ToStr8(std::u16string(reinterpret_cast<char16_t*>(str.strData), str.strLen / sizeof(char16_t))));
    free(str.strData);

    OH_Drawing_GetFontFullDescriptorAttributeString(desc, FULL_DESCRIPTOR_ATTR_S_LOCAL_FAMILY_NAME, &str);
    EXPECT_EQ(NOTO_SANS_DESC.localFamilyName,
        OHOS::Str16ToStr8(std::u16string(reinterpret_cast<char16_t*>(str.strData), str.strLen / sizeof(char16_t))));
    free(str.strData);

    OH_Drawing_GetFontFullDescriptorAttributeString(desc, FULL_DESCRIPTOR_ATTR_S_LOCAL_SUB_FAMILY_NAME, &str);
    EXPECT_EQ(NOTO_SANS_DESC.localSubFamilyName,
        OHOS::Str16ToStr8(std::u16string(reinterpret_cast<char16_t*>(str.strData), str.strLen / sizeof(char16_t))));
    free(str.strData);

    OH_Drawing_GetFontFullDescriptorAttributeString(desc, FULL_DESCRIPTOR_ATTR_S_VERSION, &str);
    EXPECT_EQ(NOTO_SANS_DESC.version,
        OHOS::Str16ToStr8(std::u16string(reinterpret_cast<char16_t*>(str.strData), str.strLen / sizeof(char16_t))));
    free(str.strData);

    OH_Drawing_GetFontFullDescriptorAttributeString(desc, FULL_DESCRIPTOR_ATTR_S_MANUFACTURE, &str);
    EXPECT_EQ(NOTO_SANS_DESC.manufacture,
        OHOS::Str16ToStr8(std::u16string(reinterpret_cast<char16_t*>(str.strData), str.strLen / sizeof(char16_t))));
    free(str.strData);

    OH_Drawing_GetFontFullDescriptorAttributeString(desc, FULL_DESCRIPTOR_ATTR_S_COPYRIGHT, &str);
    EXPECT_EQ(NOTO_SANS_DESC.copyright,
        OHOS::Str16ToStr8(std::u16string(reinterpret_cast<char16_t*>(str.strData), str.strLen / sizeof(char16_t))));
    free(str.strData);

    OH_Drawing_GetFontFullDescriptorAttributeString(desc, FULL_DESCRIPTOR_ATTR_S_TRADEMARK, &str);
    std::string trademarkRes =
        OHOS::Str16ToStr8(std::u16string(reinterpret_cast<char16_t*>(str.strData), str.strLen / sizeof(char16_t)));
    const int notoSansTrademarkSize = 34;
    EXPECT_EQ(trademarkRes.size(), notoSansTrademarkSize);
    free(str.strData);

    OH_Drawing_GetFontFullDescriptorAttributeString(desc, FULL_DESCRIPTOR_ATTR_S_LICENSE, &str);
    EXPECT_EQ(NOTO_SANS_DESC.license,
        OHOS::Str16ToStr8(std::u16string(reinterpret_cast<char16_t*>(str.strData), str.strLen / sizeof(char16_t))));
    free(str.strData);

    int32_t intAttr;
    OH_Drawing_GetFontFullDescriptorAttributeInt(desc, FULL_DESCRIPTOR_ATTR_I_INDEX, &intAttr);
    EXPECT_EQ(NOTO_SANS_DESC.index, intAttr);
}

/*
 * @tc.name: NdkFontFullDescriptorTest007
 * @tc.desc: test for OH_Drawing_GetFontFullDescriptorAttributeArray with HarmonyOS_Sans variable font
 * @tc.type: FUNC
 */
HWTEST_F(NdkFontFullDescriptorTest, NdkFontFullDescriptorTest007, TestSize.Level0)
{
    OH_Drawing_Array* fontFullDescArr = OH_Drawing_GetFontFullDescriptorsFromPath(HARMONYOS_SANS_FILE);
    EXPECT_NE(fontFullDescArr, nullptr);
    size_t size = OH_Drawing_GetDrawingArraySize(fontFullDescArr);
    EXPECT_EQ(size, 1);
    auto desc = OH_Drawing_GetFontFullDescriptorByIndex(fontFullDescArr, 0);
    EXPECT_NE(desc, nullptr);

    OH_Drawing_Array* axisArray = OH_Drawing_GetFontFullDescriptorAttributeArray(desc,
        FULL_DESCRIPTOR_ATTR_O_VARIATION_AXIS);
    EXPECT_NE(axisArray, nullptr);
    size_t axisSize = OH_Drawing_GetDrawingArraySize(axisArray);
    EXPECT_EQ(axisSize, 1);

    auto axis = OH_Drawing_GetFontVariationAxisByIndex(axisArray, 0);
    EXPECT_NE(axis, nullptr);

    OH_Drawing_String str;
    OH_Drawing_GetFontVariationAxisAttributeStr(axis, FONT_VARIATION_AIXS_ATTR_S_KEY, &str);
    std::string axisKey = OHOS::Str16ToStr8(std::u16string(reinterpret_cast<char16_t*>(str.strData),
        str.strLen / sizeof(char16_t)));
    EXPECT_EQ(axisKey, HARMONYOS_SANS_DESC.variationAxisRecords[0].key);
    free(str.strData);

    OH_Drawing_GetFontVariationAxisAttributeStr(axis, FONT_VARIATION_AIXS_ATTR_S_NAME, &str);
    std::string axisName = OHOS::Str16ToStr8(std::u16string(reinterpret_cast<char16_t*>(str.strData),
        str.strLen / sizeof(char16_t)));
    EXPECT_EQ(axisName, HARMONYOS_SANS_DESC.variationAxisRecords[0].name);
    free(str.strData);

    double doubleValue;
    OH_Drawing_GetFontVariationAxisAttributeDouble(axis, FONT_VARIATION_AIXS_ATTR_D_MIN_VALUE, &doubleValue);
    EXPECT_EQ(doubleValue, HARMONYOS_SANS_DESC.variationAxisRecords[0].minValue);

    OH_Drawing_GetFontVariationAxisAttributeDouble(axis, FONT_VARIATION_AIXS_ATTR_D_DEFAULT_VALUE,
        &doubleValue);
    EXPECT_EQ(doubleValue, HARMONYOS_SANS_DESC.variationAxisRecords[0].defaultValue);

    OH_Drawing_GetFontVariationAxisAttributeDouble(axis, FONT_VARIATION_AIXS_ATTR_D_MAX_VALUE, &doubleValue);
    EXPECT_EQ(doubleValue, HARMONYOS_SANS_DESC.variationAxisRecords[0].maxValue);

    OH_Drawing_DestroyFontVariationAxis(axisArray);
}

/*
 * @tc.name: NdkFontFullDescriptorTest008
 * @tc.desc: test for OH_Drawing_GetFontFullDescriptorByFullName with various parameters
 * @tc.type: FUNC
 */
HWTEST_F(NdkFontFullDescriptorTest, NdkFontFullDescriptorTest008, TestSize.Level0)
{
    // Test 1: nullptr fullName should return nullptr
    const OH_Drawing_FontFullDescriptor* desc = OH_Drawing_GetFontFullDescriptorByFullName(nullptr,
        OH_Drawing_SystemFontType::ALL);
    EXPECT_EQ(desc, nullptr);

    // Test 2: empty fullName should return nullptr
    OH_Drawing_String emptyStr = { .strData = nullptr, .strLen = 0 };
    desc = OH_Drawing_GetFontFullDescriptorByFullName(&emptyStr, OH_Drawing_SystemFontType::ALL);
    EXPECT_EQ(desc, nullptr);

    // Test 3: non-existent fullName should return nullptr
    std::string nonExistent = "NonExistentFontName12345";
    std::u16string u16NonExistent = OHOS::Str8ToStr16(nonExistent);
    OH_Drawing_String fullNameStr = {
        .strData = reinterpret_cast<char*>(u16NonExistent.data()),
        .strLen = static_cast<uint32_t>(u16NonExistent.length() * sizeof(char16_t))
    };
    desc = OH_Drawing_GetFontFullDescriptorByFullName(&fullNameStr, OH_Drawing_SystemFontType::ALL);
    EXPECT_EQ(desc, nullptr);

    // Test 4: Register a font and query it by fullName
    OH_Drawing_FontCollection* fontCollection = OH_Drawing_CreateFontCollection();
    ASSERT_NE(fontCollection, nullptr);

    // Register Symbol font from file
    std::vector<char> symbolContent;
    EXPECT_TRUE(OHOS::LoadBufferFromFile(SYMBOL_FILE, symbolContent));
    uint32_t result = OH_Drawing_RegisterFontBuffer(fontCollection, SYMBOL_DESC.fontFamily.c_str(),
        reinterpret_cast<uint8_t*>(symbolContent.data()), symbolContent.size());
    EXPECT_EQ(result, 0);

    // Now query the registered font by fullName
    std::u16string u16SymbolName = OHOS::Str8ToStr16(SYMBOL_DESC.fullName);
    OH_Drawing_String symbolNameStr = {
        .strData = reinterpret_cast<char*>(u16SymbolName.data()),
        .strLen = static_cast<uint32_t>(u16SymbolName.length() * sizeof(char16_t))
    };
    desc = OH_Drawing_GetFontFullDescriptorByFullName(&symbolNameStr, OH_Drawing_SystemFontType::ALL);
    EXPECT_NE(desc, nullptr);

    // Verify the returned descriptor matches expected values
    if (desc != nullptr) {
        OH_Drawing_String str;
        OH_Drawing_GetFontFullDescriptorAttributeString(desc, FULL_DESCRIPTOR_ATTR_S_FULL_NAME, &str);
        std::string result = OHOS::Str16ToStr8(std::u16string(reinterpret_cast<char16_t*>(str.strData),
            str.strLen / sizeof(char16_t)));
        EXPECT_EQ(result, SYMBOL_DESC.fullName);
        free(str.strData);

        OH_Drawing_GetFontFullDescriptorAttributeString(desc, FULL_DESCRIPTOR_ATTR_S_FAMILY_NAME, &str);
        result = OHOS::Str16ToStr8(std::u16string(reinterpret_cast<char16_t*>(str.strData),
            str.strLen / sizeof(char16_t)));
        EXPECT_EQ(result, SYMBOL_DESC.fontFamily);
        free(str.strData);

        int32_t weight;
        OH_Drawing_GetFontFullDescriptorAttributeInt(desc, FULL_DESCRIPTOR_ATTR_I_WEIGHT, &weight);
        EXPECT_EQ(SYMBOL_DESC.weight, weight);

        delete desc;
    }

    OH_Drawing_DestroyFontCollection(fontCollection);
}

/*
 * @tc.name: NdkFontFullDescriptorTest009
 * @tc.desc: test for all variation instances weight values in HarmonyOS_Sans variable font
 * @tc.type: FUNC
 */
HWTEST_F(NdkFontFullDescriptorTest, NdkFontFullDescriptorTest009, TestSize.Level0)
{
    OH_Drawing_Array* fontFullDescArr = OH_Drawing_GetFontFullDescriptorsFromPath(HARMONYOS_SANS_FILE);
    EXPECT_NE(fontFullDescArr, nullptr);
    size_t size = OH_Drawing_GetDrawingArraySize(fontFullDescArr);
    EXPECT_EQ(size, 1);
    auto desc = OH_Drawing_GetFontFullDescriptorByIndex(fontFullDescArr, 0);
    EXPECT_NE(desc, nullptr);

    OH_Drawing_Array* instanceArray = OH_Drawing_GetFontFullDescriptorAttributeArray(desc,
        FULL_DESCRIPTOR_ATTR_O_VARIATION_INSTANCE);
    EXPECT_NE(instanceArray, nullptr);
    size_t instanceSize = OH_Drawing_GetDrawingArraySize(instanceArray);
    EXPECT_EQ(instanceSize, 9);

    for (size_t i = 0; i < instanceSize; i++) {
        auto instance = OH_Drawing_GetFontVariationInstanceByIndex(instanceArray, i);
        EXPECT_NE(instance, nullptr);

        OH_Drawing_String str;
        OH_Drawing_GetFontVariationInstanceAttributeStr(instance, FONT_VARIATION_INSTANCE_ATTR_S_NAME, &str);
        std::string instanceName = OHOS::Str16ToStr8(std::u16string(reinterpret_cast<char16_t*>(str.strData),
            str.strLen / sizeof(char16_t)));
        EXPECT_EQ(instanceName, HARMONYOS_SANS_DESC.variationInstanceRecords[i].name);
        free(str.strData);

        size_t coordLength = 0;
        OH_Drawing_FontVariationInstanceCoordinate* coords =
            OH_Drawing_GetFontVariationInstanceCoordinate(instance, &coordLength);
        EXPECT_NE(coords, nullptr);
        EXPECT_EQ(coordLength, HARMONYOS_SANS_DESC.variationInstanceRecords[i].coordinates.size());
        for (size_t j = 0; j < coordLength; j++) {
            EXPECT_EQ(std::string(coords[j].axisKey),
                HARMONYOS_SANS_DESC.variationInstanceRecords[i].coordinates[j].axis);
            EXPECT_EQ(coords[j].value, HARMONYOS_SANS_DESC.variationInstanceRecords[i].coordinates[j].value);
        }
        free(coords);
    }

    OH_Drawing_DestroyFontVariationInstance(instanceArray);
}
} // namespace OHOS