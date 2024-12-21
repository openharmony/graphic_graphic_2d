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

#include <fcntl.h>
#include <fstream>
#include <iostream>
#include "gtest/gtest.h"
#include "include/core/SkString.h"
#include "include/core/SkStream.h"
#include "include/core/SkTypeface.h"
#include "SkFontMgr.h"

using namespace testing;
using namespace testing::ext;

class SkFontMgrOhosTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
private:
    static sk_sp<SkFontMgr> m_fontMgrOhosImpl;
};

sk_sp<SkFontMgr> SkFontMgrOhosTest::m_fontMgrOhosImpl = nullptr;

void SkFontMgrOhosTest::SetUpTestCase()
{
    m_fontMgrOhosImpl = SkFontMgr::RefDefault();
}
void SkFontMgrOhosTest::TearDownTestCase()
{
    m_fontMgrOhosImpl = nullptr;
}
void SkFontMgrOhosTest::SetUp() {}
void SkFontMgrOhosTest::TearDown() {}

const char* TTF_FILE_PATH = "/system/fonts/0x480x610x720x6D0x6F0x6E0x790x4F0x53_Sans_SC.ttf";

/**
 * @tc.name: GetFontFullName001
 * @tc.desc: Test GetFontFullName
 * @tc.type: FUNC
 */
HWTEST_F(SkFontMgrOhosTest, GetFontFullName001, TestSize.Level1)
{
    int fd = open(TTF_FILE_PATH, O_RDONLY);
    if (fd == -1) {
        ASSERT_TRUE(false);
    }
    std::vector<SkByteArray> fullnameVec;
    int ret = m_fontMgrOhosImpl->GetFontFullName(fd, fullnameVec);
    close(fd);
    EXPECT_EQ(ret, SUCCESSED);

    std::string testRes = "0x480x610x720x6D0x6F0x6E0x790x4F0x53 Sans SC";
    ASSERT_EQ(fullnameVec.size(), 1);
    ASSERT_NE(fullnameVec[0].strData, nullptr);
    ASSERT_EQ(fullnameVec[0].strLen, testRes.size() * 2);
}

/**
 * @tc.name: CountFamilies001
 * @tc.desc: Test CountFamilies
 * @tc.type: FUNC
 */
HWTEST_F(SkFontMgrOhosTest, CountFamilies001, TestSize.Level1)
{
    int count = m_fontMgrOhosImpl->countFamilies();
    ASSERT_EQ(count, 9);
}

/**
 * @tc.name: GetFamilyName001
 * @tc.desc: Test GetFamilyName
 * @tc.type: FUNC
 */
HWTEST_F(SkFontMgrOhosTest, GetFamilyName001, TestSize.Level1)
{
    SkString str;
    m_fontMgrOhosImpl->getFamilyName(0, &str);
    EXPECT_STREQ(str.c_str(), "0x480x610x720x6D0x6F0x6E0x790x4F0x53-Sans");
    m_fontMgrOhosImpl->getFamilyName(5, &str);
    EXPECT_STREQ(str.c_str(), "0x480x610x720x6D0x6F0x6E0x790x4F0x53-Sans-Condensed");
    m_fontMgrOhosImpl->getFamilyName(6, &str);
    EXPECT_STREQ(str.c_str(), "0x480x610x720x6D0x6F0x6E0x790x4F0x53-Sans-Digit");
    m_fontMgrOhosImpl->getFamilyName(7, &str);
    EXPECT_STREQ(str.c_str(), "serif");
    m_fontMgrOhosImpl->getFamilyName(8, &str);
    EXPECT_STREQ(str.c_str(), "monospace");
}

/**
 * @tc.name: CreateStyleSet001
 * @tc.desc: Test CreateStyleSet
 * @tc.type: FUNC
 */
HWTEST_F(SkFontMgrOhosTest, CreateStyleSet001, TestSize.Level1)
{
    SkFontStyleSet* styleSet = m_fontMgrOhosImpl->createStyleSet(9);
    ASSERT_NE(styleSet, nullptr);
    ASSERT_EQ(styleSet->count(), 0);
    delete styleSet;
    styleSet = nullptr;

    styleSet = m_fontMgrOhosImpl->createStyleSet(0);
    ASSERT_NE(styleSet, nullptr);
    ASSERT_EQ(styleSet->count(), 2);
    SkString styleName;
    SkFontStyle style;
    styleSet->getStyle(0, &style, &styleName);
    EXPECT_STREQ(styleName.c_str(), "normal");
    EXPECT_EQ(style.weight(), SkFontStyle::kNormal_Weight);
    EXPECT_EQ(style.width(), SkFontStyle::kNormal_Width);
    EXPECT_EQ(style.slant(), SkFontStyle::kUpright_Slant);
    
    styleSet->getStyle(1, &style, &styleName);
    EXPECT_STREQ(styleName.c_str(), "normal");
    EXPECT_EQ(style.weight(), SkFontStyle::kNormal_Weight);
    EXPECT_EQ(style.width(), SkFontStyle::kNormal_Width);
    EXPECT_EQ(style.slant(), SkFontStyle::kItalic_Slant);

    delete styleSet;
}

/**
 * @tc.name: MatchFamily001
 * @tc.desc: Test MatchFamily
 * @tc.type: FUNC
 */
HWTEST_F(SkFontMgrOhosTest, MatchFamily001, TestSize.Level1)
{
    SkFontStyleSet* styleSet1 = m_fontMgrOhosImpl->matchFamily("0x480x610x720x6D0x6F0x6E0x790x4F0x53 Sans");
    ASSERT_NE(styleSet1, nullptr);
    ASSERT_EQ(styleSet1->count(), 0);

    SkFontStyleSet* styleSet2 = m_fontMgrOhosImpl->matchFamily("0x480x610x720x6D0x6F0x6E0x790x4F0x53-Sans");
    ASSERT_NE(styleSet2, nullptr);
    ASSERT_EQ(styleSet2->count(), 2);
    SkFontStyleSet* styleSet3 = m_fontMgrOhosImpl->matchFamily("0x480x610x720x6D0x6F0x6E0x790x4F0x53 Sans SC");
    ASSERT_NE(styleSet3, nullptr);
    ASSERT_EQ(styleSet3->count(), 1);
    delete styleSet1;
    delete styleSet2;
    delete styleSet3;
}

/**
 * @tc.name: MatchFamilyStyle001
 * @tc.desc: Test MatchFamilyStyle
 * @tc.type: FUNC
 */
HWTEST_F(SkFontMgrOhosTest, MatchFamilyStyle001, TestSize.Level1)
{
    SkFontStyle style;
    SkTypeface* tp = m_fontMgrOhosImpl->matchFamilyStyle(nullptr, style);
    ASSERT_NE(tp, nullptr);
    SkString familyName;
    tp->getFamilyName(&familyName);
    ASSERT_STREQ(familyName.c_str(), "0x480x610x720x6D0x6F0x6E0x790x4F0x53-Sans");
    SkSafeUnref(tp);

    SkTypeface* tp1 = m_fontMgrOhosImpl->matchFamilyStyle("0x480x610x720x6D0x6F0x6E0x790x4F0x53-Sans", style);
    ASSERT_NE(tp1, nullptr);
    SkString familyName1;
    tp1->getFamilyName(&familyName1);
    ASSERT_STREQ(familyName1.c_str(), "0x480x610x720x6D0x6F0x6E0x790x4F0x53-Sans");
    SkSafeUnref(tp1);

    SkTypeface* tp2 = m_fontMgrOhosImpl->matchFamilyStyle("TestNoFound", style);
    ASSERT_EQ(tp2, nullptr);

    SkTypeface* tp3 = m_fontMgrOhosImpl->matchFamilyStyle("0x480x610x720x6D0x6F0x6E0x790x4F0x53 Sans SC", style);
    ASSERT_NE(tp3, nullptr);
    SkString familyName3;
    tp3->getFamilyName(&familyName3);
    ASSERT_STREQ(familyName3.c_str(), "0x480x610x720x6D0x6F0x6E0x790x4F0x53 Sans SC");
    SkSafeUnref(tp3);
}

/**
 * @tc.name: MatchFamilyStyleCharacter001
 * @tc.desc: Test MatchFamilyStyleCharacter
 * @tc.type: FUNC
 */
HWTEST_F(SkFontMgrOhosTest, MatchFamilyStyleCharacter001, TestSize.Level1)
{
    SkFontStyle style;
    SkTypeface* tp = m_fontMgrOhosImpl->matchFamilyStyleCharacter(nullptr, style, nullptr, 0, 0x0626);
    ASSERT_NE(tp, nullptr);
    SkString familyName;
    tp->getFamilyName(&familyName);
    ASSERT_STREQ(familyName.c_str(), "0x480x610x720x6D0x6F0x6E0x790x4F0x53 Sans Naskh Arabic UI");

    std::vector<const char*> bcp47;
    SkTypeface* tp1 = m_fontMgrOhosImpl->matchFamilyStyleCharacter(nullptr, style, bcp47.data(), bcp47.size(), 0x63CF);
    ASSERT_NE(tp1, nullptr);
    tp1->getFamilyName(&familyName);
    ASSERT_STREQ(familyName.c_str(), "0x480x610x720x6D0x6F0x6E0x790x4F0x53 Sans SC");

    bcp47.push_back("zh-Hant");
    SkTypeface* tp2 = m_fontMgrOhosImpl->matchFamilyStyleCharacter(nullptr, style, bcp47.data(), bcp47.size(), 0x63CF);
    ASSERT_NE(tp2, nullptr);
    tp2->getFamilyName(&familyName);
    ASSERT_STREQ(familyName.c_str(), "0x480x610x720x6D0x6F0x6E0x790x4F0x53 Sans TC");
   
    SkSafeUnref(tp);
    SkSafeUnref(tp1);
    SkSafeUnref(tp2);
}

/**
 * @tc.name: MakeFromStreamIndex001
 * @tc.desc: Test MakeFromStreamIndex
 * @tc.type: FUNC
 */
HWTEST_F(SkFontMgrOhosTest, MakeFromStreamIndex001, TestSize.Level1)
{
    std::unique_ptr<SkStreamAsset> skStream = SkStreamAsset::MakeFromFile(TTF_FILE_PATH);
    sk_sp<SkTypeface> sktp = m_fontMgrOhosImpl->makeFromStream(std::move(skStream), 0);
    SkString familyName;
    sktp->getFamilyName(&familyName);
    ASSERT_STREQ(familyName.c_str(), "0x480x610x720x6D0x6F0x6E0x790x4F0x53 Sans SC");
}

/**
 * @tc.name: MakeFromStreamArgs001
 * @tc.desc: Test MakeFromStreamArgs
 * @tc.type: FUNC
 */
HWTEST_F(SkFontMgrOhosTest, MakeFromStreamArgs001, TestSize.Level1)
{
    std::unique_ptr<SkStreamAsset> skStream = SkStreamAsset::MakeFromFile(TTF_FILE_PATH);
    SkFontArguments skFontArguments;
    sk_sp<SkTypeface> sktp = m_fontMgrOhosImpl->makeFromStream(std::move(skStream), skFontArguments);
    SkString familyName;
    sktp->getFamilyName(&familyName);
    ASSERT_STREQ(familyName.c_str(), "0x480x610x720x6D0x6F0x6E0x790x4F0x53 Sans SC");
}

/**
 * @tc.name: MakeFromFile001
 * @tc.desc: Test MakeFromFile
 * @tc.type: FUNC
 */
HWTEST_F(SkFontMgrOhosTest, MakeFromFile001, TestSize.Level1)
{
    sk_sp<SkTypeface> sktp = m_fontMgrOhosImpl->makeFromFile(TTF_FILE_PATH, 0);
    ASSERT_NE(sktp, nullptr);
    SkString familyName;
    sktp->getFamilyName(&familyName);
    ASSERT_STREQ(familyName.c_str(), "0x480x610x720x6D0x6F0x6E0x790x4F0x53 Sans SC");
}

/**
 * @tc.name: LegacyMakeTypeface001
 * @tc.desc: Test LegacyMakeTypeface
 * @tc.type: FUNC
 */
HWTEST_F(SkFontMgrOhosTest, LegacyMakeTypeface001, TestSize.Level1)
{
    SkFontStyle style;
    sk_sp<SkTypeface> sktp = m_fontMgrOhosImpl->legacyMakeTypeface("TestNoFound", style);
    ASSERT_NE(sktp, nullptr);
    SkString familyName;
    sktp->getFamilyName(&familyName);
    ASSERT_STREQ(familyName.c_str(), "0x480x610x720x6D0x6F0x6E0x790x4F0x53-Sans");

    sk_sp<SkTypeface> sktp1 = m_fontMgrOhosImpl->legacyMakeTypeface("0x480x610x720x6D0x6F0x6E0x790x4F0x53-Sans-Condensed",
        style);
    ASSERT_NE(sktp1, nullptr);
    sktp1->getFamilyName(&familyName);
    ASSERT_STREQ(familyName.c_str(), "0x480x610x720x6D0x6F0x6E0x790x4F0x53-Sans-Condensed");
}