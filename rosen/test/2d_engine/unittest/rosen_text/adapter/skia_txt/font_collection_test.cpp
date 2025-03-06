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
private:
    const char* symbolFile = "/system/fonts/HMSymbolVF.ttf";
};

/*
 * @tc.name: OH_Drawing_FontCollectionTest001
 * @tc.desc: test for constuctor of FontCollection
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontCollectionTest, OH_Drawing_FontCollectionTest001, TestSize.Level1)
{
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection = OHOS::Rosen::FontCollection::Create();
    fontCollection->DisableFallback();
    fontCollection->DisableSystemFont();
    const uint8_t* data = nullptr;
    auto typeface = fontCollection->LoadFont("familyname", data, 0);
    EXPECT_EQ(typeface == nullptr, true);
    typeface = fontCollection->LoadThemeFont("familynametest", data, 0);
    EXPECT_EQ(typeface == nullptr, true);
    std::shared_ptr<Drawing::FontMgr> fontMgr = fontCollection->GetFontMgr();
    EXPECT_EQ(fontMgr != nullptr, true);
}

/*
 * @tc.name: OH_Drawing_FontCollectionTest002
 * @tc.desc: test for register same typeface
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontCollectionTest, OH_Drawing_FontCollectionTest002, TestSize.Level1)
{
    auto fontCollection = OHOS::Rosen::FontCollection::Create();
    OHOS::Rosen::Drawing::Typeface::RegisterCallBackFunc([](auto) { return true; });
    std::ifstream shuc("/system/fonts/NotoSans[wdth,wght].ttf", std::ios::binary);
    std::stringstream shucStream;
    shucStream << shuc.rdbuf();
    std::string shucData = shucStream.str();
    // 10000 just for test
    const size_t minSize = 10000;
    EXPECT_GE(shucData.size(), minSize);
    auto typefaces = fontCollection->LoadThemeFont(
        "familyname", { { reinterpret_cast<const uint8_t*>(shucData.c_str()), shucData.size() } });
    auto typefaces1 = fontCollection->LoadThemeFont(
        "familyname", { { reinterpret_cast<const uint8_t*>(shucData.c_str()), shucData.size() } });
    ASSERT_EQ(typefaces.size(), 1);
    ASSERT_EQ(typefaces1.size(), 1);
    EXPECT_EQ(typefaces[0]->GetFamilyName(), typefaces1[0]->GetFamilyName());
    auto themeFamilies = SPText::DefaultFamilyNameMgr::GetInstance().GetThemeFontFamilies();
    // 1 is the default families' size
    ASSERT_EQ(themeFamilies.size(), 1);
    EXPECT_EQ(themeFamilies[0], SPText::OHOS_THEME_FONT);
}

/*
 * @tc.name: OH_Drawing_FontCollectionTest003
 * @tc.desc: test for ClearCaches
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontCollectionTest, OH_Drawing_FontCollectionTest003, TestSize.Level1)
{
    auto fontCollection = OHOS::Rosen::FontCollection::From(nullptr);
    fontCollection->ClearCaches();
    std::shared_ptr<Drawing::FontMgr> fontMgr = fontCollection->GetFontMgr();
    EXPECT_EQ(fontMgr != nullptr, true);
}

/*
 * @tc.name: OH_Drawing_FontCollectionTest004
 * @tc.desc: test for LoadSymbolFont
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontCollectionTest, OH_Drawing_FontCollectionTest004, TestSize.Level1)
{
    std::ifstream fileStream(symbolFile);
    fileStream.seekg(0, std::ios::end);
    uint32_t bufferSize = fileStream.tellg();
    fileStream.seekg(0, std::ios::beg);
    std::unique_ptr buffer = std::make_unique<uint8_t[]>(bufferSize);
    fileStream.read(reinterpret_cast<char*>(buffer.get()), bufferSize);
    fileStream.close();

    std::shared_ptr<FontCollection> fontCollection = FontCollection::Create();
    fontCollection->ClearThemeFont();
    LoadSymbolErrorCode res = fontCollection->LoadSymbolFont("testCustomSymbol", nullptr, 0);
    EXPECT_EQ(res, LoadSymbolErrorCode::LOAD_FAILED);

    uint8_t invalidBuffer[] = { 0, 0, 0, 0, 0 };
    res = fontCollection->LoadSymbolFont("testCustomSymbol", invalidBuffer, sizeof(invalidBuffer));
    EXPECT_EQ(res, LoadSymbolErrorCode::LOAD_FAILED);

    res = fontCollection->LoadSymbolFont("testCustomSymbol", buffer.get(), bufferSize);
    EXPECT_EQ(res, LoadSymbolErrorCode::SUCCESS);
    auto adaptFontCollection = reinterpret_cast<AdapterTxt::FontCollection*>(fontCollection.get());
    EXPECT_EQ(adaptFontCollection->typefaceSet_.size(), 1);

    // When loading the same data repeatedly, return success without increasing the count;
    res = fontCollection->LoadSymbolFont("testCustomSymbol", buffer.get(), bufferSize);
    EXPECT_EQ(res, LoadSymbolErrorCode::SUCCESS);
    EXPECT_EQ(adaptFontCollection->typefaceSet_.size(), 1);
}

/*
 * @tc.name: OH_Drawing_FontCollectionTest005
 * @tc.desc: test for LoadThemeFont
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_FontCollectionTest, OH_Drawing_FontCollectionTest005, TestSize.Level1)
{
    auto fontCollection = OHOS::Rosen::FontCollection::Create();
    OHOS::Rosen::Drawing::Typeface::RegisterCallBackFunc([](auto) { return true; });
    std::ifstream notosans("/system/fonts/NotoSans[wdth,wght].ttf", std::ios::binary);
    std::ifstream cjk("/system/fonts/NotoSansCJK-Regular.ttc", std::ios::binary);
    ASSERT_TRUE(cjk.is_open() && notosans.is_open());
    std::stringstream italicStream;
    std::stringstream shucStream;
    italicStream << notosans.rdbuf();
    shucStream << cjk.rdbuf();
    std::string italicData = italicStream.str();
    std::string shucData = shucStream.str();
    // 10000 just for test
    const size_t minSize = 10000;
    EXPECT_GE(italicData.size(), minSize);
    EXPECT_GE(shucData.size(), minSize);
    fontCollection->ClearThemeFont();
    auto typefaces = fontCollection->LoadThemeFont(
        "familyname",
        { { reinterpret_cast<const uint8_t*>(italicData.c_str()), italicData.size() },
        { reinterpret_cast<const uint8_t*>(shucData.c_str()), shucData.size() } });
    // 2 is the theme families' size
    ASSERT_EQ(typefaces.size(), 2);
    auto themeFamilies = SPText::DefaultFamilyNameMgr::GetInstance().GetThemeFontFamilies();
    // 2 is the theme families' size
    ASSERT_EQ(themeFamilies.size(), 2);
    EXPECT_EQ(themeFamilies[0], SPText::OHOS_THEME_FONT);
    EXPECT_EQ(themeFamilies[1], SPText::DefaultFamilyNameMgr::GetInstance().GenerateThemeFamilyName(1));

    auto styleSet =
        fontCollection->GetFontMgr()->MatchFamily(SPText::DefaultFamilyNameMgr::GenerateThemeFamilyName(0).c_str());
    ASSERT_NE(styleSet, nullptr);
    EXPECT_EQ(styleSet->Count(), 1);
    auto typeface = styleSet->CreateTypeface(0);
    ASSERT_NE(typeface, nullptr);
    EXPECT_EQ(typeface->GetFamilyName(), "Noto Sans");

    styleSet =
        fontCollection->GetFontMgr()->MatchFamily(SPText::DefaultFamilyNameMgr::GenerateThemeFamilyName(1).c_str());
    ASSERT_NE(styleSet, nullptr);
    EXPECT_EQ(styleSet->Count(), 1);
    typeface = styleSet->CreateTypeface(0);
    ASSERT_NE(typeface, nullptr);
    EXPECT_EQ(typeface->GetFamilyName(), "Noto Sans CJK JP");

    fontCollection->ClearThemeFont();
    styleSet =
        fontCollection->GetFontMgr()->MatchFamily(SPText::DefaultFamilyNameMgr::GenerateThemeFamilyName(0).c_str());
    ASSERT_NE(styleSet, nullptr);
    EXPECT_EQ(styleSet->Count(), 0);
}
} // namespace Rosen
} // namespace OHOS