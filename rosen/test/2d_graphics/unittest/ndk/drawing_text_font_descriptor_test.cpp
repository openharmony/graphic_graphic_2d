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

#include <filesystem>
#include <fstream>
#include <unordered_set>

#include "drawing_text_font_descriptor.h"
#include "gtest/gtest.h"
#include "unicode/unistr.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace {
namespace fs = std::filesystem;

const std::string STYLISH_FONT_CONFIG_FILE = "/system/fonts/visibility_list.json";
const std::string STYLISH_FONT_CONFIG_PROD_FILE = "/sys_prod/fonts/visibility_list.json";
const std::string INSTALLED_FONT_CONFIG_FILE =
    "/data/service/el1/public/for-all-app/fonts/install_fontconfig.json";
const std::string INSTALLED_FONT_CONFIG_FILE_BAK =
    "/data/service/el1/public/for-all-app/fonts/install_fontconfig.json.bak";

// "Noto Sans Mono CJK KR" exchange with "Noto Sans Mono CJK JP", make "Noto Sans Mono CJK HK" invalid index
const std::string INSTALL_CONFIG = R"(
{
  "fontlist": [
    {
      "fontfullpath": "/system/fonts/NotoSansCJK-Regular.ttc",
      "fullname": [
        "Noto Sans CJK JP",
        "Noto Sans CJK KR",
        "Noto Sans CJK SC",
        "Noto Sans CJK TC",
        "Noto Sans CJK HK",
        "Noto Sans Mono CJK KR",
        "Noto Sans Mono CJK JP",
        "Noto Sans Mono CJK SC",
        "Noto Sans Mono CJK TC",
        "Unknown",
        "Noto Sans Mono CJK HK"
      ]
    },
    { "fontfullpath": "/system/fonts/NotoSans[wdth,wght].ttf", "fullname": ["Noto Sans Regular"] }
  ]
}
)";
}

class OH_Drawing_FontDescriptorTest : public testing::Test {
};

void CreateFile(const std::string& file)
{
    fs::path filePath(file);
    fs::path dirPath = filePath.parent_path();
    if (!fs::exists(dirPath)) {
        fs::create_directories(dirPath);
    }
    std::ofstream ofs(file, std::ios::trunc);
    ofs << INSTALL_CONFIG;
}

void InitInstallConfig()
{
    if (fs::exists(INSTALLED_FONT_CONFIG_FILE)) {
        fs::rename(INSTALLED_FONT_CONFIG_FILE, INSTALLED_FONT_CONFIG_FILE_BAK);
    }
    CreateFile(INSTALLED_FONT_CONFIG_FILE);
}

void DestroyInstallConfig()
{
    if (fs::exists(INSTALLED_FONT_CONFIG_FILE_BAK)) {
        fs::copy_file(INSTALLED_FONT_CONFIG_FILE_BAK, INSTALLED_FONT_CONFIG_FILE, fs::copy_options::overwrite_existing);
        fs::remove(INSTALLED_FONT_CONFIG_FILE_BAK);
    } else {
        fs::remove(INSTALLED_FONT_CONFIG_FILE);
    }
}

class InstallConfig {
public:
    InstallConfig()
    {
        InitInstallConfig();
    }

    ~InstallConfig()
    {
        DestroyInstallConfig();
    }
};

/*
 * @tc.name: OH_Drawing_FontDescriptorTest006
 * @tc.desc: test for abnormal parameters when obtaining the font list and obtaining the FontDescriptor by fullName.
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontDescriptorTest, OH_Drawing_FontDescriptorTest006, TestSize.Level1)
{
    OH_Drawing_SystemFontType fontType = OH_Drawing_SystemFontType(0b10000);
    OH_Drawing_Array *fontList = OH_Drawing_GetSystemFontFullNamesByType(fontType);
    EXPECT_EQ(fontList, nullptr);

    OH_Drawing_FontDescriptor *descriptor = OH_Drawing_GetFontDescriptorByFullName(nullptr, fontType);
    EXPECT_EQ(descriptor, nullptr);

    // The array ttfFullName represents the UTF-16 encoded version of a non-existent font full name "你好openharmony".
    const uint8_t ttfFullName[] = {
        0x4F, 0x60,
        0x59, 0x7D,
        0x00, 0x6F,
        0x00, 0x70,
        0x00, 0x65,
        0x00, 0x6E,
        0x00, 0x68,
        0x00, 0x61,
        0x00, 0x72,
        0x00, 0x6D,
        0x00, 0x6F,
        0x00, 0x6E,
        0x00, 0x79
    };
    OH_Drawing_String drawingString;
    drawingString.strData = const_cast<uint8_t*>(ttfFullName);
    drawingString.strLen = sizeof(ttfFullName);
    OH_Drawing_FontDescriptor *descriptor1 =
        OH_Drawing_GetFontDescriptorByFullName(&drawingString, OH_Drawing_SystemFontType::ALL);
    EXPECT_EQ(descriptor1, nullptr);
}

/*
 * @tc.name: OH_Drawing_FontDescriptorTest007
 * @tc.desc: test for obtaining the array of installed fonts.
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontDescriptorTest, OH_Drawing_FontDescriptorTest007, TestSize.Level1)
{
    InstallConfig installConfig;
    std::unordered_set<std::string> fullnames { "Noto Sans CJK JP", "Noto Sans CJK KR", "Noto Sans CJK SC",
        "Noto Sans CJK TC", "Noto Sans CJK HK", "Noto Sans Mono CJK JP", "Noto Sans Mono CJK KR",
        "Noto Sans Mono CJK SC", "Noto Sans Mono CJK TC", "Noto Sans Mono CJK HK", "Noto Sans Regular" };
    OH_Drawing_SystemFontType fontType = OH_Drawing_SystemFontType::INSTALLED;
    OH_Drawing_Array* fontList = OH_Drawing_GetSystemFontFullNamesByType(fontType);
    EXPECT_NE(fontList, nullptr);
    size_t size = OH_Drawing_GetDrawingArraySize(fontList);
    EXPECT_EQ(size, 12);
    for (size_t i = 0; i < size; i++) {
        const OH_Drawing_String* fullName = OH_Drawing_GetSystemFontFullNameByIndex(fontList, i);
        ASSERT_NE(fullName, nullptr);
        OH_Drawing_FontDescriptor* fd = OH_Drawing_GetFontDescriptorByFullName(fullName, INSTALLED);
        if (fd != nullptr) {
            EXPECT_TRUE(fullnames.count(fd->fullName));
            OH_Drawing_DestroyFontDescriptor(fd);
        } else {
            std::string s;
            icu::UnicodeString ustr(reinterpret_cast<UChar*>(fullName->strData), fullName->strLen / sizeof(char16_t));
            ustr.toUTF8String(s);
            EXPECT_EQ(s, "Unknown");
        }
    }
    std::u16string fullName = u"not exist";
    OH_Drawing_String fn { reinterpret_cast<uint8_t*>(fullName.data()), fullName.size() * sizeof(char16_t) };
    OH_Drawing_FontDescriptor* fd = OH_Drawing_GetFontDescriptorByFullName(&fn, INSTALLED);
    EXPECT_EQ(fd, nullptr);
    OH_Drawing_DestroySystemFontFullNames(fontList);
}

/*
 * @tc.name: OH_Drawing_FontDescriptorTest008
 * @tc.desc: test for obtaining the array of stylish fonts.
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontDescriptorTest, OH_Drawing_FontDescriptorTest008, TestSize.Level1)
{
    if (!fs::exists(STYLISH_FONT_CONFIG_FILE) && !fs::exists(STYLISH_FONT_CONFIG_PROD_FILE)) {
        return;
    }
    OH_Drawing_SystemFontType fontType = OH_Drawing_SystemFontType::STYLISH;
    OH_Drawing_Array *fontList = OH_Drawing_GetSystemFontFullNamesByType(fontType);
    ASSERT_NE(fontList, nullptr);
    size_t size = OH_Drawing_GetDrawingArraySize(fontList);
    EXPECT_NE(size, 0);
    for (size_t i = 0; i < size; i++) {
        const OH_Drawing_String *fontFullName = OH_Drawing_GetSystemFontFullNameByIndex(fontList, i);
        EXPECT_NE(fontFullName, nullptr);
        OH_Drawing_FontDescriptor *descriptor = OH_Drawing_GetFontDescriptorByFullName(fontFullName, fontType);
        EXPECT_NE(descriptor, nullptr);
        OH_Drawing_DestroyFontDescriptor(descriptor);
    }
    OH_Drawing_DestroySystemFontFullNames(fontList);
}

/*
 * @tc.name: OH_Drawing_FontDescriptorTest009
 * @tc.desc: test for obtaining the array of system generic fonts.
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontDescriptorTest, OH_Drawing_FontDescriptorTest009, TestSize.Level1)
{
    OH_Drawing_SystemFontType fontType = OH_Drawing_SystemFontType::GENERIC;
    OH_Drawing_Array *fontList = OH_Drawing_GetSystemFontFullNamesByType(fontType);
    ASSERT_NE(fontList, nullptr);
    size_t size = OH_Drawing_GetDrawingArraySize(fontList);
    EXPECT_NE(size, 0);
    for (size_t i = 0; i < size; i++) {
        const OH_Drawing_String *fontFullName = OH_Drawing_GetSystemFontFullNameByIndex(fontList, i);
        EXPECT_NE(fontFullName, nullptr);
        OH_Drawing_FontDescriptor *descriptor = OH_Drawing_GetFontDescriptorByFullName(fontFullName, fontType);
        EXPECT_NE(descriptor, nullptr);
        OH_Drawing_DestroyFontDescriptor(descriptor);
    }
    OH_Drawing_DestroySystemFontFullNames(fontList);
}

/*
 * @tc.name: OH_Drawing_FontDescriptorTest010
 * @tc.desc: test for obtaining the list of composite fonts.
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontDescriptorTest, OH_Drawing_FontDescriptorTest010, TestSize.Level1)
{
    if (!fs::exists(STYLISH_FONT_CONFIG_FILE) && !fs::exists(STYLISH_FONT_CONFIG_PROD_FILE) &&
        !fs::exists(INSTALLED_FONT_CONFIG_FILE)) {
        return;
    }
    OH_Drawing_SystemFontType fontType = OH_Drawing_SystemFontType(INSTALLED | STYLISH);
    OH_Drawing_Array *fontList = OH_Drawing_GetSystemFontFullNamesByType(fontType);
    ASSERT_NE(fontList, nullptr);
    size_t size = OH_Drawing_GetDrawingArraySize(fontList);
    EXPECT_NE(size, 0);
    for (size_t i = 0; i < size; i++) {
        const OH_Drawing_String *fontFullName = OH_Drawing_GetSystemFontFullNameByIndex(fontList, i);
        EXPECT_NE(fontFullName, nullptr);
        OH_Drawing_FontDescriptor *descriptor = OH_Drawing_GetFontDescriptorByFullName(fontFullName, fontType);
        EXPECT_NE(descriptor, nullptr);
        OH_Drawing_DestroyFontDescriptor(descriptor);
    }
    OH_Drawing_DestroySystemFontFullNames(fontList);
}

/*
 * @tc.name: OH_Drawing_FontDescriptorTest011
 * @tc.desc: test for obtaining the list of composite fonts that include "ALL".
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontDescriptorTest, OH_Drawing_FontDescriptorTest011, TestSize.Level1)
{
    OH_Drawing_SystemFontType fontType = OH_Drawing_SystemFontType(ALL | STYLISH);
    OH_Drawing_Array *fontList = OH_Drawing_GetSystemFontFullNamesByType(fontType);
    ASSERT_NE(fontList, nullptr);
    size_t size = OH_Drawing_GetDrawingArraySize(fontList);
    EXPECT_NE(size, 0);
    for (size_t i = 0; i < size; i++) {
        const OH_Drawing_String *fontFullName = OH_Drawing_GetSystemFontFullNameByIndex(fontList, i);
        EXPECT_NE(fontFullName, nullptr);
        OH_Drawing_FontDescriptor *descriptor = OH_Drawing_GetFontDescriptorByFullName(fontFullName, fontType);
        EXPECT_NE(descriptor, nullptr);
        OH_Drawing_DestroyFontDescriptor(descriptor);
    }
    OH_Drawing_DestroySystemFontFullNames(fontList);
}

/*
 * @tc.name: OH_Drawing_FontDescriptorTest012
 * @tc.desc: test for abnormal parameters when obtaining the fullName by index and releasing array memory.
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontDescriptorTest, OH_Drawing_FontDescriptorTest012, TestSize.Level1)
{
    OH_Drawing_SystemFontType fontType = OH_Drawing_SystemFontType::GENERIC;
    OH_Drawing_Array *fontList = OH_Drawing_GetSystemFontFullNamesByType(fontType);
    ASSERT_NE(fontList, nullptr);
    const OH_Drawing_String* fullName = OH_Drawing_GetSystemFontFullNameByIndex(fontList, 500);
    EXPECT_EQ(fullName, nullptr);
    OH_Drawing_DestroySystemFontFullNames(fontList);

    const OH_Drawing_String* fullName1 = OH_Drawing_GetSystemFontFullNameByIndex(nullptr, 0);
    EXPECT_EQ(fullName1, nullptr);

    const OH_Drawing_String* fullName2 = OH_Drawing_GetSystemFontFullNameByIndex(nullptr, 500);
    EXPECT_EQ(fullName2, nullptr);

    OH_Drawing_DestroySystemFontFullNames(nullptr);
}
}