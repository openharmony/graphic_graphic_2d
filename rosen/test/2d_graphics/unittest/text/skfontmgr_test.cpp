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

const char* TTF_FILE_PATH = {0x2F, 0x73, 0x79, 0x73, 0x74, 0x65, 0x6D, 0x2F, 0x66, 0x6F, 0x6E, 0x74, 0x73,
    0x2F, 0x48, 0x61, 0x72, 0x6D, 0x6F, 0x6E, 0x79, 0x4F, 0x53, 0x5F, 0x53, 0x61, 0x6E, 0x73, 0x5F,
    0x53, 0x43, 0x2E, 0x74, 0x74, 0x66};

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
    std::string testRes = {0x48, 0x61, 0x72, 0x6D, 0x6F, 0x6E, 0x79, 0x4F, 0x53, 0x20,
        0x53, 0x61, 0x6E, 0x73, 0x20, 0x53, 0x43};
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
    std::string testRes1 = {0x48, 0x61, 0x72, 0x6D, 0x6F, 0x6E, 0x79, 0x4F, 0x53, 0x2D, 0x53, 0x61, 0x6E, 0x73};
    EXPECT_STREQ(str.c_str(), testRes1);
    m_fontMgrOhosImpl->getFamilyName(5, &str);
    std::string testRes2 = {0x48, 0x61, 0x72, 0x6D, 0x6F, 0x6E, 0x79, 0x4F, 0x53, 0x2D, 0x53, 0x61, 0x6E, 0x73, 0x2D,
        0x43, 0x6F, 0x6E, 0x64, 0x65, 0x6E, 0x73, 0x65, 0x64};
    EXPECT_STREQ(str.c_str(), testRes2);
    m_fontMgrOhosImpl->getFamilyName(6, &str);
    std::string testRes3 = {0x48, 0x61, 0x72, 0x6D, 0x6F, 0x6E, 0x79, 0x4F, 0x53, 0x2D, 0x53, 0x61, 0x6E, 0x73, 0x2D,
        0x44, 0x69, 0x67, 0x69, 0x74};
    EXPECT_STREQ(str.c_str(), testRes3);
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
    std::string testRes1 = {0x48, 0x61, 0x72, 0x6D, 0x6F, 0x6E, 0x79, 0x4F, 0x53, 0x53, 0x61, 0x6E, 0x73};
    SkFontStyleSet* styleSet1 = m_fontMgrOhosImpl->matchFamily(testRes1);
    ASSERT_NE(styleSet1, nullptr);
    ASSERT_EQ(styleSet1->count(), 0);

    std::string testRes2 = {0x48, 0x61, 0x72, 0x6D, 0x6F, 0x6E, 0x79, 0x4F, 0x53, 0x2D, 0x53, 0x61, 0x6E, 0x73};
    SkFontStyleSet* styleSet2 = m_fontMgrOhosImpl->matchFamily(testRes2);
    ASSERT_NE(styleSet2, nullptr);
    ASSERT_EQ(styleSet2->count(), 2);
    std::string testRes3 = {0x48, 0x61, 0x72, 0x6D, 0x6F, 0x6E, 0x79, 0x4F, 0x53, 0x20, 0x53, 0x61, 0x6E, 0x73,
        0x20, 0x53, 0x43};
    SkFontStyleSet* styleSet3 = m_fontMgrOhosImpl->matchFamily(testRes3);
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
    std::string testRes1 = {0x48, 0x61, 0x72, 0x6D, 0x6F, 0x6E, 0x79, 0x4F, 0x53, 0x2D, 0x53, 0x61, 0x6E, 0x73};
    ASSERT_STREQ(familyName.c_str(), testRes1);
    SkSafeUnref(tp);

    std::string testRes2 = {0x48, 0x61, 0x72, 0x6D, 0x6F, 0x6E, 0x79, 0x4F, 0x53, 0x2D, 0x53, 0x61, 0x6E, 0x73};
    SkTypeface* tp1 = m_fontMgrOhosImpl->matchFamilyStyle(testRes2, style);
    ASSERT_NE(tp1, nullptr);
    SkString familyName1;
    tp1->getFamilyName(&familyName1);
    std::string testRes3 = {0x48, 0x61, 0x72, 0x6D, 0x6F, 0x6E, 0x79, 0x4F, 0x53, 0x2D, 0x53, 0x61, 0x6E, 0x73};
    ASSERT_STREQ(familyName1.c_str(), testRes3);
    SkSafeUnref(tp1);

    SkTypeface* tp2 = m_fontMgrOhosImpl->matchFamilyStyle("TestNoFound", style);
    ASSERT_EQ(tp2, nullptr);

    std::string testRes4 = {0x48, 0x61, 0x72, 0x6D, 0x6F, 0x6E, 0x79, 0x4F, 0x53, 0x20, 0x53, 0x61, 0x6E, 0x73,
        0x20, 0x53, 0x43};
    SkTypeface* tp3 = m_fontMgrOhosImpl->matchFamilyStyle(testRes4, style);
    ASSERT_NE(tp3, nullptr);
    SkString familyName3;
    tp3->getFamilyName(&familyName3);
    std::string testRes5 = {0x48, 0x61, 0x72, 0x6D, 0x6F, 0x6E, 0x79, 0x4F, 0x53, 0x20, 0x53, 0x61, 0x6E, 0x73,
        0x20, 0x53, 0x43};
    ASSERT_STREQ(familyName3.c_str(), testRes5);
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
    std::string testRes1 = {0x48, 0x61, 0x72, 0x6D, 0x6F, 0x6E, 0x79, 0x4F, 0x53, 0x2D, 0x53, 0x61, 0x6E, 0x73,
        0x4E, 0x61, 0x73, 0x6B, 0x68, 0x20, 0x41, 0x72, 0x61, 0x62, 0x69, 0x63, 0x20, 0x55, 0x49};
    ASSERT_STREQ(familyName.c_str(), testRes1);

    std::vector<const char*> bcp47;
    SkTypeface* tp1 = m_fontMgrOhosImpl->matchFamilyStyleCharacter(nullptr, style, bcp47.data(), bcp47.size(), 0x63CF);
    ASSERT_NE(tp1, nullptr);
    tp1->getFamilyName(&familyName);
    std::string testRes2 = {0x48, 0x61, 0x72, 0x6D, 0x6F, 0x6E, 0x79, 0x4F, 0x53, 0x20, 0x53, 0x61, 0x6E, 0x73,
        0x20, 0x53, 0x43};
    ASSERT_STREQ(familyName.c_str(), testRes2);

    bcp47.push_back("zh-Hant");
    SkTypeface* tp2 = m_fontMgrOhosImpl->matchFamilyStyleCharacter(nullptr, style, bcp47.data(), bcp47.size(), 0x63CF);
    ASSERT_NE(tp2, nullptr);
    tp2->getFamilyName(&familyName);
    std::string testRes3 = {0x48, 0x61, 0x72, 0x6D, 0x6F, 0x6E, 0x79, 0x4F, 0x53, 0x20, 0x53, 0x61, 0x6E, 0x73, 0x20,
        0x53, 0x43, 0x20, 0x54, 0x43};
    ASSERT_STREQ(familyName.c_str(), testRes3);
   
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
    std::string testRes = {0x48, 0x61, 0x72, 0x6D, 0x6F, 0x6E, 0x79, 0x4F, 0x53, 0x20, 0x53, 0x61, 0x6E, 0x73,
        0x20, 0x53, 0x43};
    ASSERT_STREQ(familyName.c_str(), testRes);
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
    std::string testRes = {0x48, 0x61, 0x72, 0x6D, 0x6F, 0x6E, 0x79, 0x4F, 0x53, 0x20, 0x53, 0x61, 0x6E, 0x73,
        0x20, 0x53, 0x43};
    ASSERT_STREQ(familyName.c_str(), testRes);
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
    std::string testRes = {0x48, 0x61, 0x72, 0x6D, 0x6F, 0x6E, 0x79, 0x4F, 0x53, 0x20, 0x53, 0x61, 0x6E, 0x73,
        0x20, 0x53, 0x43};
    ASSERT_STREQ(familyName.c_str(), testRes);
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
    std::string testRes1 = {0x48, 0x61, 0x72, 0x6D, 0x6F, 0x6E, 0x79, 0x4F, 0x53, 0x2D, 0x53, 0x61, 0x6E, 0x73};
    ASSERT_STREQ(familyName.c_str(), testRes1);

    std::string testRes2 = {0x48, 0x61, 0x72, 0x6D, 0x6F, 0x6E, 0x79, 0x4F, 0x53, 0x2D, 0x53, 0x61, 0x6E, 0x73,
        0x2D, 0x43, 0x6F, 0x6E, 0x64, 0x65, 0x6E, 0x73, 0x65, 0x64};
    sk_sp<SkTypeface> sktp1 = m_fontMgrOhosImpl->legacyMakeTypeface(testRes2, style);
    ASSERT_NE(sktp1, nullptr);
    sktp1->getFamilyName(&familyName);
    std::string testRes3 = {0x48, 0x61, 0x72, 0x6D, 0x6F, 0x6E, 0x79, 0x4F, 0x53, 0x2D, 0x53, 0x61, 0x6E, 0x73,
        0x2D, 0x43, 0x6F, 0x6E, 0x64, 0x65, 0x6E, 0x73, 0x65, 0x64};
    ASSERT_STREQ(familyName.c_str(), testRes3);
}