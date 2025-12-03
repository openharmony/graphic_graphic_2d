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
#include "font_collection_mgr.h"
#include "gtest/gtest.h"
#include "txt/platform.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class OH_Drawing_FontCollectionTest : public testing::Test {
protected:
    void SetUp() override;
    static void SetUpTestSuite();

private:
    static std::vector<uint8_t> GetFileData(const std::string& path);

    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection_;
    std::shared_ptr<Drawing::FontMgr> fontMgr_;
    static std::vector<uint8_t> symbolData_;
    static std::vector<uint8_t> cjkData_;
    static std::vector<uint8_t> sansData_;
    static std::vector<uint8_t> mathData_;
};

const char* symbolFile_ = "/system/fonts/HMSymbolVF.ttf";
const char* symbolConfigFile_ = "/system/fonts/hm_symbol_config_next.json";
const char* cjkFile_ = "/system/fonts/NotoSansCJK-Regular.ttc";
const char* sansFile_ = "/system/fonts/NotoSans[wdth,wght].ttf";
const char* mathFile_ = "/system/fonts/NotoSansMath-Regular.ttf";
const char* cjkFamily_ = "Noto Sans CJK JP";
const char* sansFamily_ = "Noto Sans";
const char* mathFamily_ = "Noto Sans Math";

std::vector<uint8_t> OH_Drawing_FontCollectionTest::symbolData_{};
std::vector<uint8_t> OH_Drawing_FontCollectionTest::cjkData_{};
std::vector<uint8_t> OH_Drawing_FontCollectionTest::sansData_{};
std::vector<uint8_t> OH_Drawing_FontCollectionTest::mathData_{};

void OH_Drawing_FontCollectionTest::SetUpTestSuite()
{
    symbolData_ = GetFileData(symbolFile_);
    cjkData_ = GetFileData(cjkFile_);
    sansData_ = GetFileData(sansFile_);
    mathData_ = GetFileData(mathFile_);
}

void OH_Drawing_FontCollectionTest::SetUp()
{
    OHOS::Rosen::Drawing::Typeface::RegisterCallBackFunc([](auto tf) { return tf->GetFd(); });
    auto callback = [](const FontCollection* fc, const FontEventInfo& info) {
        EXPECT_NE(fc, nullptr);
        EXPECT_FALSE(info.familyName.empty());
    };
    OHOS::Rosen::Drawing::Typeface::RegisterOnTypefaceDestroyed([](uint32_t uniqueId) { EXPECT_NE(uniqueId, 0); });
    FontCollection::RegisterLoadFontStartCallback(callback);
    FontCollection::RegisterUnloadFontStartCallback(callback);
    FontCollection::RegisterLoadFontFinishCallback(callback);
    FontCollection::RegisterUnloadFontFinishCallback(callback);
    FontCollection::RegisterLoadFontStartCallback(nullptr);
    auto fc = std::make_shared<txt::FontCollection>();
    ASSERT_NE(fc, nullptr);
    fc->CreateSktFontCollection();
    fontCollection_ = OHOS::Rosen::FontCollection::From(std::move(fc));
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
HWTEST_F(OH_Drawing_FontCollectionTest, OH_Drawing_FontCollectionTest001, TestSize.Level0)
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
HWTEST_F(OH_Drawing_FontCollectionTest, OH_Drawing_FontCollectionTest002, TestSize.Level0)
{
    auto typefaces = fontCollection_->LoadThemeFont("familyname", { { sansData_.data(), sansData_.size() } });
    auto typefaces1 = fontCollection_->LoadThemeFont("familyname", { { sansData_.data(), sansData_.size() } });
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
HWTEST_F(OH_Drawing_FontCollectionTest, OH_Drawing_FontCollectionTest003, TestSize.Level0)
{
    fontCollection_->ClearCaches();
    EXPECT_EQ(fontMgr_->CountFamilies(), 0);
}

/*
 * @tc.name: OH_Drawing_FontCollectionTest004
 * @tc.desc: test for LoadSymbolFont
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontCollectionTest, OH_Drawing_FontCollectionTest004, TestSize.Level0)
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
HWTEST_F(OH_Drawing_FontCollectionTest, OH_Drawing_FontCollectionTest005, TestSize.Level0)
{
    fontCollection_->ClearThemeFont();
    auto typefaces = fontCollection_->LoadThemeFont(
        "familyname", { { sansData_.data(), sansData_.size() }, { cjkData_.data(), cjkData_.size() } });
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
HWTEST_F(OH_Drawing_FontCollectionTest, OH_Drawing_FontCollectionTest006, TestSize.Level0)
{
    fontCollection_->ClearThemeFont();
    auto typefaces = fontCollection_->LoadThemeFont(
        "familyname", { { sansData_.data(), sansData_.size() },
        { cjkData_.data(), cjkData_.size() }, { mathData_.data(), mathData_.size() } });
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
HWTEST_F(OH_Drawing_FontCollectionTest, OH_Drawing_FontCollectionTest007, TestSize.Level0)
{
    fontCollection_->ClearThemeFont();
    auto typefaces = fontCollection_->LoadThemeFont(
        "familyname", { { sansData_.data(), sansData_.size() },
                          // size of "12345" is 5
                        { reinterpret_cast<const uint8_t*>("12345"), 5 }, { cjkData_.data(), cjkData_.size() } });
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
HWTEST_F(OH_Drawing_FontCollectionTest, OH_Drawing_FontCollectionTest008, TestSize.Level0)
{
    std::string familyName = "familyname";
    auto typeface = fontCollection_->LoadFont(familyName, sansData_.data(), sansData_.size());
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
HWTEST_F(OH_Drawing_FontCollectionTest, OH_Drawing_FontCollectionTest009, TestSize.Level0)
{
    std::string familyName = "familyname";
    auto sansTypeface = fontCollection_->LoadFont(familyName, sansData_.data(), sansData_.size());
    auto cjkTypeface = fontCollection_->LoadFont(familyName, cjkData_.data(), cjkData_.size());
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
HWTEST_F(OH_Drawing_FontCollectionTest, OH_Drawing_FontCollectionTest010, TestSize.Level0)
{
    std::string familyName1 = "familyname1";
    std::string familyName2 = "familyname2";
    auto sansTypeface = fontCollection_->LoadFont(familyName1, sansData_.data(), sansData_.size());
    auto cjkTypeface = fontCollection_->LoadFont(familyName2, cjkData_.data(), cjkData_.size());
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
HWTEST_F(OH_Drawing_FontCollectionTest, OH_Drawing_FontCollectionTest011, TestSize.Level0)
{
    auto sansTypeface = fontCollection_->LoadFont("", sansData_.data(), sansData_.size());
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
HWTEST_F(OH_Drawing_FontCollectionTest, OH_Drawing_FontCollectionTest012, TestSize.Level0)
{
    fontCollection_->ClearThemeFont();
    auto typefaces = fontCollection_->LoadThemeFont(
        "familyname", { { sansData_.data(), sansData_.size() }, { cjkData_.data(), cjkData_.size() } });
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

/*
 * @tc.name: OH_Drawing_FontCollectionTest013
 * @tc.desc: test for unload system font and empty but failed
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontCollectionTest, OH_Drawing_FontCollectionTest013, TestSize.Level0)
{
    EXPECT_EQ(fontCollection_->LoadFont("", nullptr, 0), nullptr);
    EXPECT_FALSE(fontCollection_->UnloadFont(""));
    EXPECT_TRUE(fontCollection_->UnloadFont("Noto Sans"));
    EXPECT_TRUE(fontCollection_->UnloadFont("Noto Sans Mono"));
}

/*
 * @tc.name: OH_Drawing_FontCollectionTest014
 * @tc.desc: test for load symbol config
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontCollectionTest, OH_Drawing_FontCollectionTest014, TestSize.Level0)
{
    std::ifstream fileStream(symbolConfigFile_);
    fileStream.seekg(0, std::ios::end);
    uint32_t bufferSize = fileStream.tellg();
    fileStream.seekg(0, std::ios::beg);
    std::unique_ptr buffer = std::make_unique<uint8_t[]>(bufferSize);
    fileStream.read(reinterpret_cast<char*>(buffer.get()), bufferSize);
    fileStream.close();

    fontCollection_->ClearThemeFont();
    LoadSymbolErrorCode res = fontCollection_->LoadSymbolJson("testCustomSymbol", nullptr, 0);
    EXPECT_EQ(res, LoadSymbolErrorCode::LOAD_FAILED);

    uint8_t invalidBuffer[] = { 0, 0, 0, 0, 0 };
    res = fontCollection_->LoadSymbolJson("testCustomSymbol", invalidBuffer, sizeof(invalidBuffer));
    EXPECT_EQ(res, LoadSymbolErrorCode::JSON_ERROR);

    res = fontCollection_->LoadSymbolJson("testCustomSymbol", buffer.get(), bufferSize);
    EXPECT_EQ(res, LoadSymbolErrorCode::SUCCESS);
}

/*
 * @tc.name: OH_Drawing_FontCollectionTest015
 * @tc.desc: test load theme font and custom font
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontCollectionTest, OH_Drawing_FontCollectionTest015, TestSize.Level0)
{
    std::string familyName = "familyname";
    fontCollection_->ClearThemeFont();
    auto typefaces = fontCollection_->LoadThemeFont(
        familyName, { { sansData_.data(), sansData_.size() }, { cjkData_.data(), cjkData_.size() } });
    // 2 is the theme families' size
    EXPECT_EQ(typefaces.size(), 2);
    for (auto& typeface : typefaces) {
        ASSERT_NE(typeface, nullptr);
        EXPECT_TRUE(typeface->IsCustomTypeface());
        EXPECT_TRUE(typeface->IsThemeTypeface());
    }
    fontCollection_->ClearThemeFont();

    auto typeface = fontCollection_->LoadFont(familyName, sansData_.data(), sansData_.size());
    ASSERT_NE(typeface, nullptr);
    EXPECT_TRUE(typeface->IsCustomTypeface());
    EXPECT_TRUE(typeface->IsThemeTypeface());
    fontCollection_->UnloadFont(familyName);
}

/*
 * @tc.name: OH_Drawing_FontCollectionTest016
 * @tc.desc: test for enable global font mgr
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontCollectionTest, OH_Drawing_FontCollectionTest016, TestSize.Level0)
{
    auto adaptFontCollection = reinterpret_cast<AdapterTxt::FontCollection*>(fontCollection_.get());
    adaptFontCollection->EnableGlobalFontMgr();
    constexpr size_t targetSize = 3;
    EXPECT_EQ(adaptFontCollection->fontCollection_->GetFontManagersCount(), targetSize);
}

/*
 * @tc.name: OH_Drawing_FontCollectionTest017
 * @tc.desc: test for max size in local font collection
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontCollectionTest, OH_Drawing_FontCollectionTest017, TestSize.Level0)
{
    std::string familyNameSans = "Sans";
    auto typeface = fontCollection_->LoadFont(familyNameSans, sansData_.data(), sansData_.size());
    EXPECT_NE(typeface, nullptr);

    auto adaptFontCollection = reinterpret_cast<AdapterTxt::FontCollection*>(fontCollection_.get());
    adaptFontCollection->enableGlobalFontMgr_ = true;

    std::string mathNameSans = "Math";
    typeface = fontCollection_->LoadFont(mathNameSans, mathData_.data(), mathData_.size());
    EXPECT_NE(typeface, nullptr);

    const size_t targetMaxSize = 10000;
    FontCollectionMgr::GetInstance().SetLocalFontCollectionMaxSize(targetMaxSize);
    typeface = fontCollection_->LoadFont(mathNameSans, mathData_.data(), mathData_.size());
    EXPECT_EQ(typeface, nullptr);
}

/*
 * @tc.name: OH_Drawing_FontCollectionTest018
 * @tc.desc: test for load ttc font
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontCollectionTest, OH_Drawing_FontCollectionTest018, TestSize.Level0)
{
    std::string familyName1 = "familyname1";
    auto cTypeface = fontCollection_->LoadFont(familyName1, cjkData_.data(), cjkData_.size(), 0);
    auto jTypeface = fontCollection_->LoadFont(familyName1, cjkData_.data(), cjkData_.size(), 1);
    // the 2nd index typeface in ttc
    auto kTypeface = fontCollection_->LoadFont(familyName1, cjkData_.data(), cjkData_.size(), 2);
    // the invalid typeface index 100
    auto nTypeface = fontCollection_->LoadFont(familyName1, cjkData_.data(), cjkData_.size(), 100);

    ASSERT_NE(cTypeface, nullptr);
    ASSERT_NE(jTypeface, nullptr);
    ASSERT_NE(kTypeface, nullptr);
    EXPECT_EQ(nTypeface, nullptr);

    EXPECT_EQ(cTypeface->GetFamilyName(), cjkFamily_);
    EXPECT_EQ(jTypeface->GetFamilyName(), "Noto Sans CJK KR");
    EXPECT_EQ(kTypeface->GetFamilyName(), "Noto Sans CJK SC");

    EXPECT_EQ(fontMgr_->CountFamilies(), 1);
    auto styleSet = fontMgr_->MatchFamily(familyName1.c_str());
    ASSERT_NE(styleSet, nullptr);
    auto tf1 = styleSet->CreateTypeface(0);
    auto tf2 = styleSet->CreateTypeface(1);
    // the 2nd index typeface in ttc
    auto tf3 = styleSet->CreateTypeface(2);
    // the 3rd index typeface in ttc
    auto tf4 = styleSet->CreateTypeface(3);

    EXPECT_EQ(tf1->GetFamilyName(), cjkFamily_);
    EXPECT_EQ(tf2->GetFamilyName(), "Noto Sans CJK KR");
    EXPECT_EQ(tf3->GetFamilyName(), "Noto Sans CJK SC");
    EXPECT_EQ(tf4, nullptr);

    fontCollection_->UnloadFont(familyName1);
    EXPECT_EQ(fontMgr_->CountFamilies(), 0);
}

/*
 * @tc.name: OH_Drawing_FontCollectionTest019
 * @tc.desc: test for load ttf font with invalid index
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontCollectionTest, OH_Drawing_FontCollectionTest019, TestSize.Level0)
{
    std::string familyName1 = "familyname1";
    // 100 is an invalid index
    auto typeface = fontCollection_->LoadFont(familyName1, sansData_.data(), sansData_.size(), 100);
    EXPECT_EQ(typeface, nullptr);
}

/*
 * @tc.name: OH_Drawing_FontCollectionTest020
 * @tc.desc: test for load same ttf font
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontCollectionTest, OH_Drawing_FontCollectionTest020, TestSize.Level0)
{
    std::string familyName1 = "familyname1";
    // 100 is an invalid index
    auto typeface = fontCollection_->LoadFont(familyName1, sansData_.data(), sansData_.size());
    EXPECT_NE(typeface, nullptr);
    auto typeface1 = fontCollection_->LoadFont(familyName1, sansData_.data(), sansData_.size());
    EXPECT_NE(typeface1, nullptr);
    EXPECT_EQ(typeface->GetFamilyName(), typeface1->GetFamilyName());
    fontCollection_->UnloadFont(familyName1);
}
} // namespace Rosen
} // namespace OHOS