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

#include <fstream>

#include "font_collection.h"
#include "gtest/gtest.h"
#include "txt/platform.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class OH_Drawing_FontCollectionTest : public testing::Test {
protected:
    void SetUp() override;

private:
    std::vector<uint8_t> GetFileData(const std::string& path);

    const char* symbolFile_ = "/system/fonts/HMSymbolVF.ttf";
    const char* cjkFile_ = "/system/fonts/NotoSansCJK-Regular.ttc";
    const char* sansFile_ = "/system/fonts/NotoSans[wdth,wght].ttf";
    const char* mathFile_ = "/system/fonts/NotoSansMath-Regular.ttf";
    const char* cjkFamily_ = "Noto Sans CJK JP";
    const char* sansFamily_ = "Noto Sans";
    const char* mathFamily_ = "Noto Sans Math";

    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection_;
    std::shared_ptr<Drawing::FontMgr> fontMgr_;
};

void OH_Drawing_FontCollectionTest::SetUp()
{
    OHOS::Rosen::Drawing::Typeface::RegisterCallBackFunc([](auto) { return true; });
    OHOS::Rosen::Drawing::Typeface::UnRegisterCallBackFunc([](auto) { return true; });
    fontCollection_ = OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    ASSERT_NE(fontCollection_, nullptr);
    fontMgr_ = fontCollection_->GetFontMgr();
    ASSERT_NE(fontMgr_, nullptr);
}

std::vector<uint8_t> OH_Drawing_FontCollectionTest::GetFileData(const std::string& path)
{
    std::ifstream file(path, std::ios::binary);
    std::stringstream fileStream;
    fileStream << file.rdbuf();
    std::string fileData = fileStream.str();
    return std::vector<uint8_t>(fileData.begin(), fileData.end());
}

/*
 * @tc.name: OH_Drawing_FontCollectionTest001
 * @tc.desc: test for constuctor of FontCollection
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontCollectionTest, OH_Drawing_FontCollectionTest001, TestSize.Level1)
{
    fontCollection_->DisableFallback();
    fontCollection_->DisableSystemFont();
    const uint8_t* data = nullptr;
    auto typeface = fontCollection_->LoadFont("familyname", data, 0);
    EXPECT_EQ(typeface == nullptr, true);
    typeface = fontCollection_->LoadThemeFont("familynametest", data, 0);
    EXPECT_EQ(typeface == nullptr, true);
}

/*
 * @tc.name: OH_Drawing_FontCollectionTest002
 * @tc.desc: test for register same typeface
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontCollectionTest, OH_Drawing_FontCollectionTest002, TestSize.Level1)
{
    std::vector<uint8_t> sansData = GetFileData(sansFile_);
    // 10000 just for test
    const size_t minSize = 10000;
    EXPECT_GE(sansData.size(), minSize);
    auto typefaces = fontCollection_->LoadThemeFont("familyname", { { sansData.data(), sansData.size() } });
    auto typefaces1 = fontCollection_->LoadThemeFont("familyname", { { sansData.data(), sansData.size() } });
    ASSERT_EQ(typefaces.size(), 1);
    ASSERT_EQ(typefaces1.size(), typefaces.size());
    EXPECT_EQ(typefaces[0]->GetFamilyName(), typefaces1[0]->GetFamilyName());
    auto themeFamilies = SPText::DefaultFamilyNameMgr::GetInstance().GetThemeFontFamilies();
    ASSERT_EQ(themeFamilies.size(), typefaces.size());
    ASSERT_EQ(fontMgr_->CountFamilies(), typefaces.size());

    const char* result[] = { sansFamily_, cjkFamily_ };
    for (size_t i = 0; i < themeFamilies.size(); i += 1) {
        auto themeFamily = SPText::DefaultFamilyNameMgr::GetInstance().GenerateThemeFamilyName(i);
        EXPECT_EQ(themeFamilies[i], themeFamily);
        EXPECT_EQ(typefaces[i]->GetFamilyName(), result[i]);
        auto styleSet = fontMgr_->MatchFamily(themeFamily.c_str());
        ASSERT_NE(styleSet, nullptr);
        EXPECT_EQ(styleSet->Count(), 1);
        auto typeface = styleSet->CreateTypeface(0);
        ASSERT_NE(typeface, nullptr);
        EXPECT_EQ(typeface->GetFamilyName(), result[i]);
    }
    fontCollection_->ClearThemeFont();
    EXPECT_EQ(fontMgr_->CountFamilies(), 0);
}

/*
 * @tc.name: OH_Drawing_FontCollectionTest003
 * @tc.desc: test for ClearCaches
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontCollectionTest, OH_Drawing_FontCollectionTest003, TestSize.Level1)
{
    fontCollection_->ClearCaches();
    EXPECT_EQ(fontMgr_->CountFamilies(), 0);
}

/*
 * @tc.name: OH_Drawing_FontCollectionTest004
 * @tc.desc: test for LoadSymbolFont
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontCollectionTest, OH_Drawing_FontCollectionTest004, TestSize.Level1)
{
    std::ifstream fileStream(symbolFile_);
    fileStream.seekg(0, std::ios::end);
    uint32_t bufferSize = fileStream.tellg();
    fileStream.seekg(0, std::ios::beg);
    std::unique_ptr buffer = std::make_unique<uint8_t[]>(bufferSize);
    fileStream.read(reinterpret_cast<char*>(buffer.get()), bufferSize);
    fileStream.close();

    fontCollection_->ClearThemeFont();
    LoadSymbolErrorCode res = fontCollection_->LoadSymbolFont("testCustomSymbol", nullptr, 0);
    EXPECT_EQ(res, LoadSymbolErrorCode::LOAD_FAILED);

    uint8_t invalidBuffer[] = { 0, 0, 0, 0, 0 };
    res = fontCollection_->LoadSymbolFont("testCustomSymbol", invalidBuffer, sizeof(invalidBuffer));
    EXPECT_EQ(res, LoadSymbolErrorCode::LOAD_FAILED);

    res = fontCollection_->LoadSymbolFont("testCustomSymbol", buffer.get(), bufferSize);
    EXPECT_EQ(res, LoadSymbolErrorCode::SUCCESS);
    auto adaptFontCollection = reinterpret_cast<AdapterTxt::FontCollection*>(fontCollection_.get());
    EXPECT_EQ(adaptFontCollection->typefaceSet_.size(), 1);

    // When loading the same data repeatedly, return success without increasing the count;
    res = fontCollection_->LoadSymbolFont("testCustomSymbol", buffer.get(), bufferSize);
    EXPECT_EQ(res, LoadSymbolErrorCode::SUCCESS);
    EXPECT_EQ(adaptFontCollection->typefaceSet_.size(), 1);
}

/*
 * @tc.name: OH_Drawing_FontCollectionTest005
 * @tc.desc: test for LoadThemeFont with 2 fonts
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontCollectionTest, OH_Drawing_FontCollectionTest005, TestSize.Level1)
{
    std::vector<uint8_t> sansData = GetFileData(sansFile_);
    std::vector<uint8_t> cjkData = GetFileData(cjkFile_);
    // 10000 just for test
    const size_t minSize = 10000;
    EXPECT_GE(sansData.size(), minSize);
    EXPECT_GE(cjkData.size(), minSize);
    fontCollection_->ClearThemeFont();
    auto typefaces = fontCollection_->LoadThemeFont(
        "familyname", { { sansData.data(), sansData.size() }, { cjkData.data(), cjkData.size() } });
    // 2 is the theme families' size
    ASSERT_EQ(typefaces.size(), 2);
    auto themeFamilies = SPText::DefaultFamilyNameMgr::GetInstance().GetThemeFontFamilies();
    ASSERT_EQ(themeFamilies.size(), typefaces.size());
    ASSERT_EQ(fontMgr_->CountFamilies(), typefaces.size());

    const char* result[] = { sansFamily_, cjkFamily_ };
    for (size_t i = 0; i < themeFamilies.size(); i += 1) {
        auto themeFamily = SPText::DefaultFamilyNameMgr::GetInstance().GenerateThemeFamilyName(i);
        EXPECT_EQ(themeFamilies[i], themeFamily);
        EXPECT_EQ(typefaces[i]->GetFamilyName(), result[i]);
        auto styleSet = fontMgr_->MatchFamily(themeFamily.c_str());
        ASSERT_NE(styleSet, nullptr);
        EXPECT_EQ(styleSet->Count(), 1);
        auto typeface = styleSet->CreateTypeface(0);
        ASSERT_NE(typeface, nullptr);
        EXPECT_EQ(typeface->GetFamilyName(), result[i]);
    }

    fontCollection_->ClearThemeFont();
    EXPECT_EQ(fontMgr_->CountFamilies(), 0);
}

/*
 * @tc.name: OH_Drawing_FontCollectionTest006
 * @tc.desc: test for LoadThemeFont with 3 fonts
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontCollectionTest, OH_Drawing_FontCollectionTest006, TestSize.Level1)
{
    std::vector<uint8_t> sansData = GetFileData(sansFile_);
    std::vector<uint8_t> cjkData = GetFileData(cjkFile_);
    std::vector<uint8_t> mathData = GetFileData(mathFile_);
    // 10000 just for test
    const size_t minSize = 10000;
    EXPECT_GE(sansData.size(), minSize);
    EXPECT_GE(cjkData.size(), minSize);
    EXPECT_GE(mathData.size(), minSize);
    fontCollection_->ClearThemeFont();
    auto typefaces = fontCollection_->LoadThemeFont(
        "familyname", { { sansData.data(), sansData.size() },
        { cjkData.data(), cjkData.size() }, { mathData.data(), mathData.size() } });
    // 3 is the theme families' size
    ASSERT_EQ(typefaces.size(), 3);
    auto themeFamilies = SPText::DefaultFamilyNameMgr::GetInstance().GetThemeFontFamilies();
    ASSERT_EQ(themeFamilies.size(), typefaces.size());
    ASSERT_EQ(fontMgr_->CountFamilies(), typefaces.size());

    const char* result[] = { sansFamily_, cjkFamily_, mathFamily_ };
    for (size_t i = 0; i < themeFamilies.size(); i += 1) {
        auto themeFamily = SPText::DefaultFamilyNameMgr::GetInstance().GenerateThemeFamilyName(i);
        EXPECT_EQ(themeFamilies[i], themeFamily);
        EXPECT_EQ(typefaces[i]->GetFamilyName(), result[i]);
        auto styleSet = fontMgr_->MatchFamily(themeFamily.c_str());
        ASSERT_NE(styleSet, nullptr);
        EXPECT_EQ(styleSet->Count(), 1);
        auto typeface = styleSet->CreateTypeface(0);
        ASSERT_NE(typeface, nullptr);
        EXPECT_EQ(typeface->GetFamilyName(), result[i]);
    }

    fontCollection_->ClearThemeFont();
    EXPECT_EQ(fontMgr_->CountFamilies(), 0);
}

/*
 * @tc.name: OH_Drawing_FontCollectionTest007
 * @tc.desc: test for LoadThemeFont with 3 fonts but 1 is null
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontCollectionTest, OH_Drawing_FontCollectionTest007, TestSize.Level1)
{
    std::vector<uint8_t> sansData = GetFileData(sansFile_);
    std::vector<uint8_t> cjkData = GetFileData(cjkFile_);
    // 10000 just for test
    const size_t minSize = 10000;
    EXPECT_GE(sansData.size(), minSize);
    EXPECT_GE(cjkData.size(), minSize);
    fontCollection_->ClearThemeFont();
    auto typefaces = fontCollection_->LoadThemeFont(
        "familyname", { { sansData.data(), sansData.size() },
                          // size of "12345" is 5
                        { reinterpret_cast<const uint8_t*>("12345"), 5 }, { cjkData.data(), cjkData.size() } });
    // 2 is the theme families' size
    ASSERT_EQ(typefaces.size(), 2);
    auto themeFamilies = SPText::DefaultFamilyNameMgr::GetInstance().GetThemeFontFamilies();
    ASSERT_EQ(themeFamilies.size(), typefaces.size());
    ASSERT_EQ(fontMgr_->CountFamilies(), typefaces.size());

    const char* result[] = { sansFamily_, cjkFamily_ };
    for (size_t i = 0; i < themeFamilies.size(); i += 1) {
        auto themeFamily = SPText::DefaultFamilyNameMgr::GetInstance().GenerateThemeFamilyName(i);
        EXPECT_EQ(themeFamilies[i], themeFamily);
        EXPECT_EQ(typefaces[i]->GetFamilyName(), result[i]);
        auto styleSet = fontMgr_->MatchFamily(themeFamily.c_str());
        ASSERT_NE(styleSet, nullptr);
        EXPECT_EQ(styleSet->Count(), 1);
        auto typeface = styleSet->CreateTypeface(0);
        ASSERT_NE(typeface, nullptr);
        EXPECT_EQ(typeface->GetFamilyName(), result[i]);
    }

    fontCollection_->ClearThemeFont();
    EXPECT_EQ(fontMgr_->CountFamilies(), 0);
}

/*
 * @tc.name: OH_Drawing_FontCollectionTest008
 * @tc.desc: test for LoadFont with 1 font and unload it
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontCollectionTest, OH_Drawing_FontCollectionTest008, TestSize.Level1)
{
    std::vector<uint8_t> sansData = GetFileData(sansFile_);
    // 10000 just for test
    const size_t minSize = 10000;
    EXPECT_GE(sansData.size(), minSize);
    std::string familyName = "familyname";
    auto typeface = fontCollection_->LoadFont(familyName, sansData.data(), sansData.size());
    ASSERT_NE(typeface, nullptr);
    EXPECT_EQ(typeface->GetFamilyName(), sansFamily_);
    EXPECT_EQ(fontMgr_->CountFamilies(), 1);
    auto styleSet = fontMgr_->MatchFamily(familyName.c_str());
    ASSERT_NE(styleSet, nullptr);
    EXPECT_EQ(styleSet->Count(), 1);
    EXPECT_EQ(styleSet->CreateTypeface(0)->GetFamilyName(), sansFamily_);
    EXPECT_TRUE(fontCollection_->UnloadFont(familyName));
    EXPECT_EQ(fontMgr_->CountFamilies(), 0);
}

/*
 * @tc.name: OH_Drawing_FontCollectionTest009
 * @tc.desc: test for LoadFont use 2 font with same family and unload it
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontCollectionTest, OH_Drawing_FontCollectionTest009, TestSize.Level1)
{
    std::vector<uint8_t> sansData = GetFileData(sansFile_);
    std::vector<uint8_t> cjkData = GetFileData(cjkFile_);
    // 10000 just for test
    const size_t minSize = 10000;
    EXPECT_GE(sansData.size(), minSize);
    std::string familyName = "familyname";
    auto sansTypeface = fontCollection_->LoadFont(familyName, sansData.data(), sansData.size());
    auto cjkTypeface = fontCollection_->LoadFont(familyName, cjkData.data(), cjkData.size());
    ASSERT_NE(sansTypeface, nullptr);
    ASSERT_NE(cjkTypeface, nullptr);
    EXPECT_EQ(sansTypeface->GetFamilyName(), sansFamily_);
    EXPECT_EQ(cjkTypeface->GetFamilyName(), cjkFamily_);
    EXPECT_EQ(fontMgr_->CountFamilies(), 1);
    auto styleSet = fontMgr_->MatchFamily(familyName.c_str());
    ASSERT_NE(styleSet, nullptr);
    EXPECT_EQ(styleSet->Count(), 2);
    EXPECT_EQ(styleSet->CreateTypeface(0)->GetFamilyName(), sansFamily_);
    EXPECT_EQ(styleSet->CreateTypeface(1)->GetFamilyName(), cjkFamily_);
    EXPECT_TRUE(fontCollection_->UnloadFont(familyName));
    EXPECT_EQ(fontMgr_->CountFamilies(), 0);
}

/*
 * @tc.name: OH_Drawing_FontCollectionTest010
 * @tc.desc: test for LoadFont use 2 font with different family and unload it
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontCollectionTest, OH_Drawing_FontCollectionTest010, TestSize.Level1)
{
    std::vector<uint8_t> sansData = GetFileData(sansFile_);
    std::vector<uint8_t> cjkData = GetFileData(cjkFile_);
    // 10000 just for test
    const size_t minSize = 10000;
    EXPECT_GE(sansData.size(), minSize);
    std::string familyName1 = "familyname1";
    std::string familyName2 = "familyname2";
    auto sansTypeface = fontCollection_->LoadFont(familyName1, sansData.data(), sansData.size());
    auto cjkTypeface = fontCollection_->LoadFont(familyName2, cjkData.data(), cjkData.size());
    ASSERT_NE(sansTypeface, nullptr);
    EXPECT_EQ(sansTypeface->GetFamilyName(), sansFamily_);
    ASSERT_NE(cjkTypeface, nullptr);
    EXPECT_EQ(cjkTypeface->GetFamilyName(), cjkFamily_);
    EXPECT_EQ(fontMgr_->CountFamilies(), 2);
    auto styleSet1 = fontMgr_->MatchFamily(familyName1.c_str());
    ASSERT_NE(styleSet1, nullptr);
    EXPECT_EQ(styleSet1->Count(), 1);
    EXPECT_EQ(styleSet1->CreateTypeface(0)->GetFamilyName(), sansFamily_);
    EXPECT_TRUE(fontCollection_->UnloadFont(familyName1));
    EXPECT_EQ(fontMgr_->CountFamilies(), 1);

    auto styleSet2 = fontMgr_->MatchFamily(familyName2.c_str());
    ASSERT_NE(styleSet2, nullptr);
    EXPECT_EQ(styleSet2->Count(), 1);
    EXPECT_EQ(styleSet2->CreateTypeface(0)->GetFamilyName(), cjkFamily_);
    EXPECT_TRUE(fontCollection_->UnloadFont(familyName2));
    EXPECT_EQ(fontMgr_->CountFamilies(), 0);
}

/*
 * @tc.name: OH_Drawing_FontCollectionTest011
 * @tc.desc: test for LoadFont use font with empty family name and unload it
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontCollectionTest, OH_Drawing_FontCollectionTest011, TestSize.Level1)
{
    std::vector<uint8_t> sansData = GetFileData(sansFile_);
    // 10000 just for test
    const size_t minSize = 10000;
    EXPECT_GE(sansData.size(), minSize);
    auto sansTypeface = fontCollection_->LoadFont("", sansData.data(), sansData.size());
    ASSERT_NE(sansTypeface, nullptr);
    EXPECT_EQ(sansTypeface->GetFamilyName(), sansFamily_);
    EXPECT_EQ(fontMgr_->CountFamilies(), 1);
    auto styleSet1 = fontMgr_->MatchFamily(sansFamily_);
    ASSERT_NE(styleSet1, nullptr);
    EXPECT_EQ(styleSet1->Count(), 1);
    EXPECT_EQ(styleSet1->CreateTypeface(0)->GetFamilyName(), sansFamily_);
    EXPECT_TRUE(fontCollection_->UnloadFont(sansFamily_));
    EXPECT_EQ(fontMgr_->CountFamilies(), 0);
}

/*
 * @tc.name: OH_Drawing_FontCollectionTest012
 * @tc.desc: test for unload theme font but failed
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontCollectionTest, OH_Drawing_FontCollectionTest012, TestSize.Level1)
{
    std::vector<uint8_t> sansData = GetFileData(sansFile_);
    std::vector<uint8_t> cjkData = GetFileData(cjkFile_);
    // 10000 just for test
    const size_t minSize = 10000;
    EXPECT_GE(sansData.size(), minSize);
    EXPECT_GE(cjkData.size(), minSize);
    fontCollection_->ClearThemeFont();
    auto typefaces = fontCollection_->LoadThemeFont(
        "familyname", { { sansData.data(), sansData.size() }, { cjkData.data(), cjkData.size() } });
    // 2 is the theme families' size
    ASSERT_EQ(typefaces.size(), 2);
    auto themeFamilies = SPText::DefaultFamilyNameMgr::GetInstance().GetThemeFontFamilies();
    ASSERT_EQ(themeFamilies.size(), typefaces.size());
    ASSERT_EQ(fontMgr_->CountFamilies(), typefaces.size());

    EXPECT_FALSE(fontCollection_->UnloadFont(SPText::DefaultFamilyNameMgr::GenerateThemeFamilyName(0)));
    EXPECT_FALSE(fontCollection_->UnloadFont(SPText::DefaultFamilyNameMgr::GenerateThemeFamilyName(1)));
    const char* result[] = { sansFamily_, cjkFamily_ };
    for (size_t i = 0; i < themeFamilies.size(); i += 1) {
        auto themeFamily = SPText::DefaultFamilyNameMgr::GetInstance().GenerateThemeFamilyName(i);
        EXPECT_EQ(themeFamilies[i], themeFamily);
        EXPECT_EQ(typefaces[i]->GetFamilyName(), result[i]);
        auto styleSet = fontMgr_->MatchFamily(themeFamily.c_str());
        ASSERT_NE(styleSet, nullptr);
        EXPECT_EQ(styleSet->Count(), 1);
        auto typeface = styleSet->CreateTypeface(0);
        ASSERT_NE(typeface, nullptr);
        EXPECT_EQ(typeface->GetFamilyName(), result[i]);
    }

    fontCollection_->ClearThemeFont();
    EXPECT_EQ(fontMgr_->CountFamilies(), 0);
}
} // namespace Rosen
} // namespace OHOS