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

#include <algorithm>
#include <codecvt>
#include <cstddef>
#include <fcntl.h>
#include <fstream>
#include <locale>
#include <sys/stat.h>

#include "gtest/gtest.h"
#include "impl_factory.h"
#include "impl_interface/font_mgr_impl.h"

#include "text/font_mgr.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class FontMgrTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void FontMgrTest::SetUpTestCase() {}
void FontMgrTest::TearDownTestCase() {}
void FontMgrTest::SetUp() {}
void FontMgrTest::TearDown() {}

/**
 * @tc.name:LoadDynamicFont002
 * @tc.desc: Test LoadDynamicFont
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(FontMgrTest, LoadDynamicFont002, TestSize.Level1)
{
    std::shared_ptr<FontMgr> FontMgr = FontMgr::CreateDynamicFontMgr();
    std::vector<uint8_t> emptyFontData;
    Typeface* typeface = FontMgr->LoadDynamicFont("EmptyFont", emptyFontData.data(), emptyFontData.size());
    ASSERT_EQ(typeface, nullptr);
}

/**
 *
 * @tc.name:MatchFamilyStyleCharacter001
 * @tc.desc: Test MatchFamilyStyleCharacter
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(FontMgrTest, MatchFamilyStyleCharacter001, TestSize.Level1)
{
    std::shared_ptr<FontMgr> FontMgr = FontMgr::CreateDefaultFontMgr();
    FontStyle fontStyle;
    const char* bcp47[] = { "en-US" };
    int bcp47Count = 1;
    int32_t character = 'A';
    Typeface* typeface = FontMgr->MatchFamilyStyleCharacter("serif", fontStyle, bcp47, bcp47Count, character);
    ASSERT_NE(typeface, nullptr);
}

/**
 * @tc.name:MatchFamily002
 * @tc.desc: Test MatchFamily
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(FontMgrTest, MatchFamily002, TestSize.Level1)
{
    std::shared_ptr<FontMgr> FontMgr = FontMgr::CreateDefaultFontMgr();
    const char* familyName = "serif";
    FontStyleSet* fontStyleSet = FontMgr->MatchFamily(familyName);
    ASSERT_NE(fontStyleSet, nullptr);
}

/**
 * @tc.name:CountFamilies001
 * @tc.desc: Test CountFamilies
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(FontMgrTest, CountFamilies001, TestSize.Level1)
{
    std::shared_ptr<FontMgr> FontMgr = FontMgr::CreateDefaultFontMgr();
    int familyCount = FontMgr->CountFamilies();
    EXPECT_EQ(familyCount, 10);
}

/**
 * @tc.name:GetFamilyName001
 * @tc.desc: Test GetFamilyName
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(FontMgrTest, GetFamilyName001, TestSize.Level1)
{
    std::shared_ptr<FontMgr> FontMgr = FontMgr::CreateDefaultFontMgr();
    std::string familyName;
    FontMgr->GetFamilyName(0, familyName);
    std::string result = { 0x48, 0x61, 0x72, 0x6d, 0x6f, 0x6e, 0x79, 0x4f, 0x53, 0x2d, 0x53, 0x61, 0x6e, 0x73 };
    EXPECT_EQ(familyName, result);
}

/**
 * @tc.name:CreateStyleSet001
 * @tc.desc: Test CreateStyleSet
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(FontMgrTest, CreateStyleSet001, TestSize.Level1)
{
    std::shared_ptr<FontMgr> FontMgr = FontMgr::CreateDefaultFontMgr();
    ASSERT_NE(FontMgr, nullptr);
    FontStyleSet* fontStyleSet = FontMgr->CreateStyleSet(0);
    ASSERT_NE(fontStyleSet, nullptr);
}

/**
 * @tc.name:MatchFallbackc001
 * @tc.desc: Test fallback typeface
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(FontMgrTest, MatchFallbackc001, TestSize.Level1)
{
    const char* ja = "Noto Sans CJK JP";
    const char* hmSymbol = "HM Symbol";
    std::shared_ptr<FontMgr> fontMgr = FontMgr::CreateDefaultFontMgr();
    auto typeface = fontMgr->MatchFamilyStyleCharacter(nullptr, {}, nullptr, 0, u'❶');
    ASSERT_NE(typeface, nullptr);
    EXPECT_EQ(typeface->GetFamilyName(), ja);
    delete typeface;

    // 0xF0000 is edge case of ulUnicodeRange
    typeface = fontMgr->MatchFamilyStyleCharacter(nullptr, {}, nullptr, 0, 0xF0000);
    ASSERT_NE(typeface, nullptr);
    EXPECT_EQ(typeface->GetFamilyName(), hmSymbol);
    delete typeface;

    typeface = fontMgr->MatchFamilyStyleCharacter(nullptr, {}, nullptr, 0, u'✿');
    ASSERT_NE(typeface, nullptr);
    EXPECT_EQ(typeface->GetFamilyName(), ja);
    delete typeface;

    typeface = fontMgr->MatchFamilyStyleCharacter(nullptr, {}, nullptr, 0, u'♲');
    ASSERT_NE(typeface, nullptr);
    EXPECT_EQ(typeface->GetFamilyName(), ja);
    delete typeface;

    typeface = fontMgr->MatchFamilyStyleCharacter(nullptr, {}, nullptr, 0, u'❀');
    ASSERT_NE(typeface, nullptr);
    EXPECT_EQ(typeface->GetFamilyName(), ja);
    delete typeface;
}

const char* TTF_FILE_PATH = "/system/fonts/Roboto-Regular.ttf";
// The ttf font file fullname is the utf16BE format content corresponding to "Noto Sans Regular"
const uint8_t TTF_FULLNAME[] = {0x0, 0x4e, 0x0, 0x6f, 0x0, 0x74, 0x0, 0x6f, 0x0, 0x20, 0x0, 0x53,
                                0x0, 0x61, 0x0, 0x6e, 0x0, 0x73, 0x0, 0x20, 0x0, 0x52, 0x0, 0x65,
                                0x0, 0x67, 0x0, 0x75, 0x0, 0x6c, 0x0, 0x61, 0x0, 0x72};
const char* OTF_FILE_PATH = "/data/fonts/Igiari-2.otf";
// The otf font file fullname is the utf16BE format content corresponding to "Igiari"
const uint8_t OTF_FULLNAME[] = {0x0, 0x49, 0x0, 0x67, 0x0, 0x69, 0x0, 0x61, 0x0, 0x72, 0x0, 0x69};
const char* TTC_FILE_PATH = "/system/fonts/NotoSerifCJK-Regular.ttc";
// The ttc font file fullname is the utf16BE format content corresponding to "Noto Serif CJK JP"
const uint8_t TTC_FULLNAME[] = {0x0, 0x4e, 0x0, 0x6f, 0x0, 0x74, 0x0, 0x6f, 0x0, 0x20, 0x0, 0x53,
                                0x0, 0x65, 0x0, 0x72, 0x0, 0x69, 0x0, 0x66, 0x0, 0x20, 0x0, 0x43,
                                0x0, 0x4a, 0x0, 0x4b, 0x0, 0x20, 0x0, 0x4a, 0x0, 0x50};
const char* ERRORPATH_FILE_PATH = "/system/fonts/error_path.ttf";
const char* ERRORFORMAT_FILE_PATH = "/system/etc/fontconfig.json";
const char* JSON_CONFIG_PATH = "/data/fonts/install_fontconfig.json";
const char* ERROR_JSON_CONFIG_PATH = "/data/fonts/error_path.json";
const char* CONFIG_FIRST_FONT_PATH = "/data/fonts/Igiari-2.otf";
/**
 * @tc.name:GetFontFullName001
 * @tc.desc: Check the validity of the TTF file and obtain the full name
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(FontMgrTest, GetFontFullName001, TestSize.Level1)
{
    std::shared_ptr<FontMgr> fontMgr = FontMgr::CreateDefaultFontMgr();
    EXPECT_NE(fontMgr, nullptr);
    std::vector<FontByteArray> fullnameVec;
    std::string fontPath = TTF_FILE_PATH;
    int fd = open(fontPath.c_str(), O_RDONLY);
    if (fd != -1) {
        int ret = fontMgr->GetFontFullName(fd, fullnameVec);
        close(fd);
        EXPECT_TRUE(ret == 0 && fullnameVec.size() > 0);
        EXPECT_EQ(fullnameVec[0].strLen, sizeof(TTF_FULLNAME));
        EXPECT_EQ(memcmp(fullnameVec[0].strData.get(), TTF_FULLNAME, fullnameVec[0].strLen), 0);
    }
}

/**
 * @tc.name:GetFontFullName002
 * @tc.desc: Check the validity of the OTF file and obtain the full name
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(FontMgrTest, GetFontFullName002, TestSize.Level1)
{
    std::shared_ptr<FontMgr> fontMgr = FontMgr::CreateDefaultFontMgr();
    EXPECT_NE(fontMgr, nullptr);
    std::vector<FontByteArray> fullnameVec;
    std::string fontPath = OTF_FILE_PATH;
    int fd = open(fontPath.c_str(), O_RDONLY);
    if (fd != -1) {
        int ret = fontMgr->GetFontFullName(fd, fullnameVec);
        close(fd);
        EXPECT_TRUE(ret == 0 && fullnameVec.size() > 0);
        EXPECT_EQ(fullnameVec[0].strLen, sizeof(OTF_FULLNAME));
        EXPECT_EQ(memcmp(fullnameVec[0].strData.get(), OTF_FULLNAME, fullnameVec[0].strLen), 0);
    }
}

/**
 * @tc.name:GetFontFullName003
 * @tc.desc: Check the validity of the TTC file and obtain the full name
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(FontMgrTest, GetFontFullName003, TestSize.Level1)
{
    std::shared_ptr<FontMgr> fontMgr = FontMgr::CreateDefaultFontMgr();
    EXPECT_NE(fontMgr, nullptr);
    std::vector<FontByteArray> fullnameVec;
    std::string fontPath = TTC_FILE_PATH;
    int fd = open(fontPath.c_str(), O_RDONLY);
    if (fd != -1) {
        int ret = fontMgr->GetFontFullName(fd, fullnameVec);
        close(fd);
        EXPECT_TRUE(ret == 0 && fullnameVec.size() > 0);
        EXPECT_EQ(fullnameVec[0].strLen, sizeof(TTC_FULLNAME));
        EXPECT_EQ(memcmp(fullnameVec[0].strData.get(), TTC_FULLNAME, fullnameVec[0].strLen), 0);
    }
}

/**
 * @tc.name:GetFontFullName004
 * @tc.desc: Enter an incorrect path and return the corresponding error reason code
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(FontMgrTest, GetFontFullName004, TestSize.Level1)
{
    std::shared_ptr<FontMgr> fontMgr = FontMgr::CreateDefaultFontMgr();
    EXPECT_NE(fontMgr, nullptr);
    std::vector<FontByteArray> fullnameVec;
    std::string fontPath = ERRORPATH_FILE_PATH;
    int fd = open(fontPath.c_str(), O_RDONLY);
    int ret = fontMgr->GetFontFullName(fd, fullnameVec);
    if (fd != -1) {
        close(fd);
    }
    EXPECT_EQ(ret, ERROR_TYPE_OTHER);
}
/**
 * @tc.name:GetFontFullName005
 * @tc.desc: Enter an empty path and return the corresponding error reason code
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(FontMgrTest, GetFontFullName005, TestSize.Level1)
{
    std::shared_ptr<FontMgr> fontMgr = FontMgr::CreateDefaultFontMgr();
    EXPECT_NE(fontMgr, nullptr);
    std::vector<FontByteArray> fullnameVec;
    std::string filepath = "";
    int fd = open(filepath.c_str(), O_RDONLY);
    int ret = fontMgr->GetFontFullName(fd, fullnameVec);
    if (fd != -1) {
        close(fd);
    }
    EXPECT_EQ(ret, ERROR_TYPE_OTHER);
}
/**
 * @tc.name:GetFontFullName006
 * @tc.desc: Enter the file path of an incorrectly formatted font file and return the corresponding error reason code
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(FontMgrTest, GetFontFullName006, TestSize.Level1)
{
    std::shared_ptr<FontMgr> fontMgr = FontMgr::CreateDefaultFontMgr();
    EXPECT_NE(fontMgr, nullptr);
    std::vector<FontByteArray> fullnameVec;
    std::string fontPath = ERRORFORMAT_FILE_PATH;
    int fd = open(fontPath.c_str(), O_RDONLY);
    int ret = fontMgr->GetFontFullName(fd, fullnameVec);
    if (fd != -1) {
        close(fd);
    }
    EXPECT_EQ(ret, ERROR_TYPE_OTHER);
}

/**
 * @tc.name:ParseInstallFontConfig001
 * @tc.desc: Enter the path of a properly formatted JSON configuration file and parse it successfully
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(FontMgrTest, ParseInstallFontConfig001, TestSize.Level1)
{
    std::shared_ptr<FontMgr> fontMgr = FontMgr::CreateDynamicFontMgr();
    EXPECT_NE(fontMgr, nullptr);
    std::vector<std::string> fontPathVec;
    std::string configPath = JSON_CONFIG_PATH;
    std::ifstream configFile(configPath, std::ios::in);
    if (configFile.is_open()) {
        configFile.close();
        int ret = fontMgr->ParseInstallFontConfig(configPath, fontPathVec);
        EXPECT_TRUE(ret == 0 && fontPathVec.size() > 0);
        EXPECT_EQ(fontPathVec[0], CONFIG_FIRST_FONT_PATH);
    }
}
/**
 * @tc.name:ParseInstallFontConfig002
 * @tc.desc: Enter the path of a valid JSON configuration file and fail to parse it
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(FontMgrTest, ParseInstallFontConfig002, TestSize.Level1)
{
    std::shared_ptr<FontMgr> fontMgr = FontMgr::CreateDynamicFontMgr();
    EXPECT_NE(fontMgr, nullptr);
    std::vector<std::string> fontPathVec;
    std::string configPath = ERROR_JSON_CONFIG_PATH;
    int ret = fontMgr->ParseInstallFontConfig(configPath, fontPathVec);
    EXPECT_EQ(ret, ERROR_PARSE_CONFIG_FAILED);
}
/**
 * @tc.name:ParseInstallFontConfig003
 * @tc.desc: Enter an empty file path and fail to parse
 * @tc.type: FUNC
 * @tc.require: I91F9L
 */
HWTEST_F(FontMgrTest, ParseInstallFontConfig003, TestSize.Level1)
{
    std::shared_ptr<FontMgr> fontMgr = FontMgr::CreateDynamicFontMgr();
    EXPECT_NE(fontMgr, nullptr);
    std::vector<std::string> fontPathVec;
    std::string configPath = "";
    int ret = fontMgr->ParseInstallFontConfig(configPath, fontPathVec);
    EXPECT_EQ(ret, ERROR_PARSE_CONFIG_FAILED);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
