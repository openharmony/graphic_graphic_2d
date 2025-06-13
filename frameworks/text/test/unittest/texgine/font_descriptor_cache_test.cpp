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
#include <gtest/gtest.h>

#include "font_descriptor_cache.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
static const std::string FILE_NAME = "/data/service/el1/public/for-all-app/fonts/install_fontconfig.json";
static const std::string FILE_NAME_PATH = "/system/fonts/hm_symbol_config.json";
static const std::string EFFECTIVE_FILE_NAME = "/system/fonts/HarmonyOS_Sans.ttf";
constexpr uint32_t WEIGHT_500 = 500;

class FontDescriptorTest : public testing::Test {};

/**
 * @tc.name: ClearFontFileCacheTest
 * @tc.desc: test the ClearFontFileCache function
 * @tc.type: FUNC
 */
HWTEST_F(FontDescriptorTest, ClearFontFileCacheTest, TestSize.Level0)
{
    auto fontDescriptorCache = std::make_shared<FontDescriptorCache>();
    fontDescriptorCache->ClearFontFileCache();
    EXPECT_NE(fontDescriptorCache, nullptr);
}

/**
 * @tc.name: ParserStylishFontsTest
 * @tc.desc: test the ParserStylishFonts function
 * @tc.type: FUNC
 */
HWTEST_F(FontDescriptorTest, ParserStylishFontsTest, TestSize.Level0)
{
    auto fontDescriptorCache = std::make_shared<FontDescriptorCache>();
    TextEngine::FontParser::FontDescriptor descriptor;
    descriptor.fullName = "test";
    fontDescriptorCache->parser_.visibilityFonts_.push_back(descriptor);
    EXPECT_NE(fontDescriptorCache->parser_.visibilityFonts_.size(), 0);
    fontDescriptorCache->ParserStylishFonts();
}

/**
 * @tc.name: FontDescriptorScatterTest
 * @tc.desc: test the FontDescriptorScatter function
 * @tc.type: FUNC
 */
HWTEST_F(FontDescriptorTest, FontDescriptorScatterTest, TestSize.Level0)
{
    auto fontDescriptorCache = std::make_shared<FontDescriptorCache>();
    FontDescSharedPtr desc = std::make_shared<TextEngine::FontParser::FontDescriptor>();
    desc->symbolic = false;
    fontDescriptorCache->FontDescriptorScatter(desc);
    EXPECT_EQ(desc->symbolic, false);
    desc->symbolic = true;
    fontDescriptorCache->FontDescriptorScatter(desc);
    EXPECT_NE(fontDescriptorCache, nullptr);
}

/**
 * @tc.name: MatchFromFontDescriptorAndIsDefaultTest
 * @tc.desc: test the MatchFromFontDescriptor and IsDefault function
 * @tc.type: FUNC
 */
HWTEST_F(FontDescriptorTest, MatchFromFontDescriptorTest, TestSize.Level0)
{
    auto fontDescriptorCache = std::make_shared<FontDescriptorCache>();
    EXPECT_NE(fontDescriptorCache, nullptr);

    FontDescSharedPtr desc = nullptr;
    std::set<FontDescSharedPtr> result;
    fontDescriptorCache->MatchFromFontDescriptor(desc, result);

    desc = std::make_shared<TextEngine::FontParser::FontDescriptor>();
    EXPECT_NE(desc, nullptr);

    EXPECT_TRUE(fontDescriptorCache->IsDefault(desc));

    desc->fontFamily = "IsDefaultFamily";
    desc->fullName = "IsDefaultName";
    desc->postScriptName = "IsDefaultName";
    desc->fontSubfamily = "IsDefaultFamily";
    desc->weight = 1;
    desc->width = 1;
    desc->italic = 1;
    desc->monoSpace = true;
    desc->symbolic = true;
    EXPECT_FALSE(fontDescriptorCache->IsDefault(desc));
}

/**
 * @tc.name: FontDescriptorCacheFilterTest
 * @tc.desc: test the FontDescriptorCacheFilter function
 * @tc.type: FUNC
 */
HWTEST_F(FontDescriptorTest, FontDescriptorCacheFilterTest, TestSize.Level0)
{
    auto fontDescriptorCache = std::make_shared<FontDescriptorCache>();
    EXPECT_NE(fontDescriptorCache, nullptr);

    std::set<FontDescSharedPtr> finishRet;
    EXPECT_TRUE(fontDescriptorCache->FilterSymbolicCache(false, finishRet));
    EXPECT_TRUE(fontDescriptorCache->FilterMonoSpaceCache(false, finishRet));
    EXPECT_TRUE(fontDescriptorCache->FilterItalicCache(0, finishRet));
    EXPECT_FALSE(fontDescriptorCache->FilterWidthCache(-1, finishRet));
    EXPECT_TRUE(fontDescriptorCache->FilterWidthCache(0, finishRet));
    EXPECT_FALSE(fontDescriptorCache->FilterBoldCache(-1, finishRet));
    EXPECT_TRUE(fontDescriptorCache->FilterBoldCache(0, finishRet));

    FontDescSharedPtr finish = std::make_shared<TextEngine::FontParser::FontDescriptor>();
    EXPECT_NE(finish, nullptr);
    finish->weight = 1;
    finishRet.emplace(finish);
    EXPECT_TRUE(fontDescriptorCache->FilterBoldCache(1, finishRet));
}

/**
 * @tc.name: GetSystemFontFullNamesByTypeTest
 * @tc.desc: test the GetSystemFontFullNamesByType function
 * @tc.type: FUNC
 */
HWTEST_F(FontDescriptorTest, GetSystemFontFullNamesByTypeTest, TestSize.Level0)
{
    auto fontDescriptorCache = std::make_shared<FontDescriptorCache>();
    EXPECT_NE(fontDescriptorCache, nullptr);

    std::string fullName = "GetSystemFontFullNamesByTypeTests";
    std::unordered_set<std::string> fontLis;
    fontLis.emplace(fullName);
    fontDescriptorCache->GetSystemFontFullNamesByType(128, fontLis);
    EXPECT_EQ(fontLis.size(), 0);
}

/**
 * @tc.name: HandleMapIntersectionTest
 * @tc.desc: test the HandleMapIntersection function
 * @tc.type: FUNC
 */
HWTEST_F(FontDescriptorTest, HandleMapIntersectionTest, TestSize.Level0)
{
    auto fontDescriptorCache = std::make_shared<FontDescriptorCache>();
    EXPECT_NE(fontDescriptorCache, nullptr);

    std::set<FontDescSharedPtr> finishRet;
    std::string name = "";
    std::unordered_map<std::string, std::set<FontDescSharedPtr>> map;
    EXPECT_TRUE(fontDescriptorCache->HandleMapIntersection(finishRet, name, map));

    FontDescSharedPtr finish = std::make_shared<TextEngine::FontParser::FontDescriptor>();
    EXPECT_NE(finish, nullptr);
    finish->weight = 1;
    finishRet.emplace(finish);
    name = "test";
    std::string nameMap = "HandleMapIntersectionTest";
    map[nameMap] = finishRet;
    EXPECT_FALSE(fontDescriptorCache->HandleMapIntersection(finishRet, name, map));

    FontDescSharedPtr finishTest = std::make_shared<TextEngine::FontParser::FontDescriptor>();
    EXPECT_NE(finishTest, nullptr);
    finishTest->fullName = "test";
    finishRet.emplace(finishTest);
    name = "HandleMapIntersectionTest";
    EXPECT_TRUE(fontDescriptorCache->HandleMapIntersection(finishRet, name, map));

    finishRet.clear();
    finishRet.emplace(finish);
    EXPECT_TRUE(fontDescriptorCache->HandleMapIntersection(finishRet, name, map));
}

/**
 * @tc.name: GetFontDescSharedPtrByFullNameTest
 * @tc.desc: test the GetFontDescSharedPtrByFullName function
 * @tc.type: FUNC
 */
HWTEST_F(FontDescriptorTest, GetFontDescSharedPtrByFullNameTest, TestSize.Level0)
{
    auto fontDescriptorCache = std::make_shared<FontDescriptorCache>();
    EXPECT_NE(fontDescriptorCache, nullptr);

    FontDescSharedPtr result = nullptr;
    std::string fullName = "";
    int32_t systemFontType = 16;
    fontDescriptorCache->GetFontDescSharedPtrByFullName(fullName, systemFontType, result);

    fullName = "HandleMapIntersectionTest";
    fontDescriptorCache->GetFontDescSharedPtrByFullName(fullName, systemFontType, result);

    FontDescSharedPtr fontDescSharedPtr = std::make_shared<TextEngine::FontParser::FontDescriptor>();
    EXPECT_NE(fontDescSharedPtr, nullptr);
    std::set<FontDescSharedPtr> fontDescSet;
    fontDescSet.emplace(fontDescSharedPtr);
    fontDescriptorCache->fullNameMap_.emplace(fullName, fontDescSet);
    systemFontType = 14;
    fontDescriptorCache->GetFontDescSharedPtrByFullName(fullName, systemFontType, result);
    EXPECT_TRUE(result != nullptr);

    fontDescriptorCache->fullNameMap_.clear();
    fontDescriptorCache->GetFontDescSharedPtrByFullName(fullName, systemFontType, result);
    EXPECT_TRUE(result == nullptr);

    fontDescriptorCache->stylishFullNameMap_.emplace("HandleMapIntersectionTest", fontDescSet);
    fontDescriptorCache->GetFontDescSharedPtrByFullName(fullName, systemFontType, result);
    EXPECT_TRUE(result != nullptr);

    fontDescriptorCache->stylishFullNameMap_.clear();
    fontDescriptorCache->GetFontDescSharedPtrByFullName(fullName, systemFontType, result);
    EXPECT_TRUE(result == nullptr);
}

/**
 * @tc.name: FilterSymbolicCacheTest
 * @tc.desc: test the FilterSymbolicCache function
 * @tc.type: FUNC
 */
HWTEST_F(FontDescriptorTest, FilterSymbolicCacheTest, TestSize.Level0)
{
    auto fontDescriptorCache = std::make_shared<FontDescriptorCache>();
    EXPECT_NE(fontDescriptorCache, nullptr);
    bool symbolic = false;
    std::set<FontDescSharedPtr> finishRet;
    bool res = fontDescriptorCache->FilterSymbolicCache(symbolic, finishRet);
    EXPECT_EQ(res, true);
    symbolic = true;
    fontDescriptorCache->symbolicCache_.clear();
    res = fontDescriptorCache->FilterSymbolicCache(symbolic, finishRet);
    EXPECT_EQ(res, false);
    FontDescSharedPtr desc = std::make_shared<TextEngine::FontParser::FontDescriptor>();
    desc->symbolic = false;
    finishRet.insert(desc);
    res = fontDescriptorCache->FilterSymbolicCache(symbolic, finishRet);
    EXPECT_EQ(res, false);
    FontDescSharedPtr descPtr = std::make_shared<TextEngine::FontParser::FontDescriptor>();
    descPtr->symbolic = true;
    finishRet.insert(descPtr);
    res = fontDescriptorCache->FilterSymbolicCache(symbolic, finishRet);
    EXPECT_EQ(res, true);
    fontDescriptorCache->MatchFromFontDescriptor(desc, finishRet);
}

/**
 * @tc.name: FilterMonoSpaceCacheTest
 * @tc.desc: test the FilterMonoSpaceCache function
 * @tc.type: FUNC
 */
HWTEST_F(FontDescriptorTest, FilterMonoSpaceCacheTest, TestSize.Level0)
{
    auto fontDescriptorCache = std::make_shared<FontDescriptorCache>();
    EXPECT_NE(fontDescriptorCache, nullptr);
    bool monoSpace = false;
    std::set<FontDescSharedPtr> finishRet;
    bool res = fontDescriptorCache->FilterMonoSpaceCache(monoSpace, finishRet);
    EXPECT_EQ(res, true);
    monoSpace = true;
    fontDescriptorCache->monoSpaceCache_.clear();
    res = fontDescriptorCache->FilterMonoSpaceCache(monoSpace, finishRet);
    EXPECT_EQ(res, false);
    FontDescSharedPtr desc = std::make_shared<TextEngine::FontParser::FontDescriptor>();
    desc->monoSpace = false;
    finishRet.insert(desc);
    res = fontDescriptorCache->FilterMonoSpaceCache(monoSpace, finishRet);
    EXPECT_EQ(res, false);
    FontDescSharedPtr descPtr = std::make_shared<TextEngine::FontParser::FontDescriptor>();
    descPtr->monoSpace = true;
    finishRet.insert(descPtr);
    res = fontDescriptorCache->FilterMonoSpaceCache(monoSpace, finishRet);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: FilterItalicCacheTest
 * @tc.desc: test the FilterItalicCache function
 * @tc.type: FUNC
 */
HWTEST_F(FontDescriptorTest, FilterItalicCacheTest, TestSize.Level0)
{
    auto fontDescriptorCache = std::make_shared<FontDescriptorCache>();
    EXPECT_NE(fontDescriptorCache, nullptr);
    int italic = 0;
    std::set<FontDescSharedPtr> finishRet;
    bool res = fontDescriptorCache->FilterItalicCache(italic, finishRet);
    EXPECT_EQ(res, true);
    italic = 1;
    fontDescriptorCache->italicCache_.clear();
    res = fontDescriptorCache->FilterItalicCache(italic, finishRet);
    EXPECT_EQ(res, false);
    FontDescSharedPtr desc = std::make_shared<TextEngine::FontParser::FontDescriptor>();
    desc->italic = 0;
    finishRet.insert(desc);
    res = fontDescriptorCache->FilterItalicCache(italic, finishRet);
    EXPECT_EQ(res, false);
    FontDescSharedPtr descPtr = std::make_shared<TextEngine::FontParser::FontDescriptor>();
    descPtr->italic = 1;
    finishRet.insert(descPtr);
    res = fontDescriptorCache->FilterItalicCache(italic, finishRet);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: FilterWidthCacheTest
 * @tc.desc: test the FilterWidthCache function
 * @tc.type: FUNC
 */
HWTEST_F(FontDescriptorTest, FilterWidthCacheTest, TestSize.Level0)
{
    auto fontDescriptorCache = std::make_shared<FontDescriptorCache>();
    EXPECT_NE(fontDescriptorCache, nullptr);
    int width = 0;
    std::set<FontDescSharedPtr> finishRet;
    bool res = fontDescriptorCache->FilterWidthCache(width, finishRet);
    EXPECT_EQ(res, true);
    width = -1;
    res = fontDescriptorCache->FilterWidthCache(width, finishRet);
    EXPECT_EQ(res, false);
    width = 1;
    FontDescSharedPtr desc = std::make_shared<TextEngine::FontParser::FontDescriptor>();
    desc->width = 0;
    fontDescriptorCache->allFontDescriptor_.insert(desc);
    res = fontDescriptorCache->FilterWidthCache(width, finishRet);
    EXPECT_EQ(res, false);
    desc->width = 1;
    FontDescSharedPtr descPtr = std::make_shared<TextEngine::FontParser::FontDescriptor>();
    descPtr->width = 1;
    fontDescriptorCache->allFontDescriptor_.insert(descPtr);
    res = fontDescriptorCache->FilterWidthCache(width, finishRet);
    EXPECT_EQ(res, true);
    finishRet.insert(descPtr);
    res = fontDescriptorCache->FilterWidthCache(width, finishRet);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: FilterBoldCacheTest
 * @tc.desc: test the FilterBoldCache function
 * @tc.type: FUNC
 */
HWTEST_F(FontDescriptorTest, FilterBoldCacheTest, TestSize.Level0)
{
    auto fontDescriptorCache = std::make_shared<FontDescriptorCache>();
    EXPECT_NE(fontDescriptorCache, nullptr);
    int weight = 0;
    std::set<FontDescSharedPtr> finishRet;
    bool res = fontDescriptorCache->FilterBoldCache(weight, finishRet);
    EXPECT_EQ(res, true);
    weight = -1;
    res = fontDescriptorCache->FilterBoldCache(weight, finishRet);
    EXPECT_EQ(res, false);
    weight = 1;
    FontDescSharedPtr desc = std::make_shared<TextEngine::FontParser::FontDescriptor>();
    desc->weight = 0;
    fontDescriptorCache->allFontDescriptor_.insert(desc);
    res = fontDescriptorCache->FilterBoldCache(weight, finishRet);
    EXPECT_EQ(res, false);
    FontDescSharedPtr descPtr = std::make_shared<TextEngine::FontParser::FontDescriptor>();
    descPtr->weight = 1;
    fontDescriptorCache->allFontDescriptor_.insert(descPtr);
    res = fontDescriptorCache->FilterBoldCache(weight, finishRet);
    EXPECT_EQ(res, false);
    weight = WEIGHT_500;
    res = fontDescriptorCache->FilterBoldCache(weight, finishRet);
    EXPECT_EQ(res, false);
    finishRet.insert(desc);
    res = fontDescriptorCache->FilterBoldCache(weight, finishRet);
    EXPECT_EQ(res, false);
}
} // namespace Rosen
} // namespace OHOS
