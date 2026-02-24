/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <string>
#include <vector>

#include "gtest/gtest.h"
#include "variation_font_cache.h"

using namespace testing;
using namespace testing::ext;

namespace txt {

// Global variables to track callback invocations
static uint32_t g_removeCallbackCount = 0;
static uint32_t g_removeCallbackLastId = 0;
static uint32_t g_registerCallbackCount = 0;
static std::shared_ptr<RSTypeface> g_registerCallbackLastFace = nullptr;
constexpr uint32_t WGHT_AXIS = 2003265652;

void TestRemoveCallback(uint32_t uniqueId)
{
    g_removeCallbackCount++;
    g_removeCallbackLastId = uniqueId;
}

int32_t TestRegisterCallback(std::shared_ptr<RSTypeface> typeface)
{
    g_registerCallbackCount++;
    g_registerCallbackLastFace = typeface;
    return 0;
}

class VariationFontCacheTest : public testing::Test {
public:
    void SetUp() override
    {
        g_removeCallbackCount = 0;
        g_removeCallbackLastId = 0;
        g_registerCallbackCount = 0;
        g_registerCallbackLastFace = nullptr;

        cache_ = std::make_unique<VariationFontCache>();
        cache_->SetCacheRemoveCallback(TestRemoveCallback);
    }
    void TearDown() override
    {
        RSDrawing::Typeface::RegisterCallBackFunc(nullptr);
        cache_.reset();
        cache_ = nullptr;
    }

private:
    skia::textlayout::FontArguments GenerateFontArguments(float value)
    {
        std::vector<SkFontArguments::VariationPosition::Coordinate> coordinates{{WGHT_AXIS, value}};
        SkFontArguments::VariationPosition position{coordinates.data(), static_cast<int>(coordinates.size())};
        SkFontArguments skFontArgs;
        skFontArgs.setVariationDesignPosition(position);
        skia::textlayout::FontArguments fontArgs(skFontArgs);
        return fontArgs;
    }
    std::unique_ptr<VariationFontCache> cache_{nullptr};
};

/**
 * @tc.name: SetCacheRemoveCallback001
 * @tc.desc: After setting the callback, removing an entry should trigger the callback
 * @tc.type:FUNC
 */
HWTEST_F(VariationFontCacheTest, SetCacheRemoveCallback001, TestSize.Level0)
{
    auto typeface = RSTypeface::MakeDefault();
    skia::textlayout::FontArguments fontArgs = GenerateFontArguments(100);
    auto variation = cache_->RegisterVariationTypeface(typeface, fontArgs);
    uint32_t uniqueId = variation->GetUniqueID();

    EXPECT_EQ(g_removeCallbackCount, 0);
    cache_->RemoveByOriginalUniqueId(typeface->GetUniqueID());
    EXPECT_EQ(g_removeCallbackCount, 1);
    EXPECT_EQ(g_removeCallbackLastId, uniqueId);
}

/**
 * @tc.name: EvictOldest001
 * @tc.desc: Cache full, inserting a new entry triggers eviction and calls remove callback
 * @tc.type:FUNC
 */
HWTEST_F(VariationFontCacheTest, EvictOldest001, TestSize.Level0)
{
    constexpr size_t MAX_CACHE_SIZE = 32;
    for (size_t i = 0; i < MAX_CACHE_SIZE; ++i) {
        auto typeface = RSTypeface::MakeDefault();
        skia::textlayout::FontArguments fontArgs = GenerateFontArguments(100 + i);
        auto variation = cache_->RegisterVariationTypeface(typeface, fontArgs);
    }

    EXPECT_EQ(g_removeCallbackCount, 0);
    auto newTypeface = RSTypeface::MakeDefault();
    skia::textlayout::FontArguments newArgs = GenerateFontArguments(200);
    auto newVariation = cache_->RegisterVariationTypeface(newTypeface, newArgs);
    EXPECT_NE(newVariation, nullptr);
    EXPECT_EQ(g_removeCallbackCount, 1);
}

/**
 * @tc.name: RegisterVariationTypeface001
 * @tc.desc: Test function RegisterVariationTypeface
 * @tc.type:FUNC
 */
HWTEST_F(VariationFontCacheTest, RegisterVariationTypeface001, TestSize.Level0)
{
    skia::textlayout::FontArguments fontArgs = GenerateFontArguments(100);
    auto result = cache_->RegisterVariationTypeface(nullptr, fontArgs);
    EXPECT_EQ(result, nullptr);
    auto typeface = RSTypeface::MakeDefault();
    std::optional<skia::textlayout::FontArguments> nullOpt;
    result = cache_->RegisterVariationTypeface(typeface, nullOpt);
    EXPECT_EQ(result, typeface);
}

/**
 * @tc.name: RegisterVariationTypeface002
 * @tc.desc: Cache hit, return cached entry and update access time
 * @tc.type:FUNC
 */
HWTEST_F(VariationFontCacheTest, RegisterVariationTypeface002, TestSize.Level0)
{
    auto typeface = RSTypeface::MakeDefault();
    skia::textlayout::FontArguments fontArgs = GenerateFontArguments(100);
    auto first = cache_->RegisterVariationTypeface(typeface, fontArgs);
    EXPECT_NE(first, typeface);
    EXPECT_NE(first, nullptr);
    auto second = cache_->RegisterVariationTypeface(typeface, fontArgs);
    EXPECT_EQ(first, second);
}

/**
 * @tc.name: RegisterVariationTypeface003
 * @tc.desc: Clone fails (MakeClone returns nullptr), return original typeface, no cache entry added
 * @tc.type:FUNC
 */
HWTEST_F(VariationFontCacheTest, RegisterVariationTypeface003, TestSize.Level0)
{
    auto typeface = RSTypeface::MakeDefault();
    skia::textlayout::FontArguments fontArgs = GenerateFontArguments(100);
    typeface->typefaceImpl_ = nullptr;

    auto result1 = cache_->RegisterVariationTypeface(typeface, fontArgs);
    EXPECT_EQ(result1, typeface);
}

/**
 * @tc.name: RegisterVariationTypeface004
 * @tc.desc: When a global registration callback exists, it is called after successful cloning
 * @tc.type:FUNC
 */
HWTEST_F(VariationFontCacheTest, RegisterVariationTypeface004, TestSize.Level0)
{
    // Set global registration callback
    RSDrawing::Typeface::RegisterCallBackFunc(TestRegisterCallback);

    auto typeface = RSTypeface::MakeDefault();
    skia::textlayout::FontArguments fontArgs = GenerateFontArguments(100);

    EXPECT_EQ(g_registerCallbackCount, 0);
    auto result = cache_->RegisterVariationTypeface(typeface, fontArgs);
    ASSERT_NE(result, nullptr);
    EXPECT_EQ(g_registerCallbackCount, 1);
    EXPECT_EQ(g_registerCallbackLastFace, result);
}

/**
 * @tc.name: RemoveByOriginalUniqueId001
 * @tc.desc: Remove matching entries, callbacks are invoked
 * @tc.type:FUNC
 */
HWTEST_F(VariationFontCacheTest, RemoveByOriginalUniqueId001, TestSize.Level0)
{
    auto typeface1 = RSTypeface::MakeDefault();
    auto typeface2 = RSTypeface::MakeDefault();
    skia::textlayout::FontArguments args1 = GenerateFontArguments(100);
    skia::textlayout::FontArguments args2 = GenerateFontArguments(200);
    auto var1 = cache_->RegisterVariationTypeface(typeface1, args1);
    auto var2 = cache_->RegisterVariationTypeface(typeface2, args2);
    // same original, different args
    auto var3 = cache_->RegisterVariationTypeface(typeface1, args2);

    uint32_t id1 = typeface1->GetUniqueID();
    EXPECT_EQ(g_removeCallbackCount, 0);
    cache_->RemoveByOriginalUniqueId(id1);
    EXPECT_EQ(g_removeCallbackCount, 2);
}

/**
 * @tc.name: RemoveByOriginalUniqueId002
 * @tc.desc: No matching entries, callbacks not invoked
 * @tc.type:FUNC
 */
HWTEST_F(VariationFontCacheTest, RemoveByOriginalUniqueId002, TestSize.Level0)
{
    auto typeface = RSTypeface::MakeDefault();
    skia::textlayout::FontArguments args = GenerateFontArguments(100);
    auto var = cache_->RegisterVariationTypeface(typeface, args);
    EXPECT_NE(var, nullptr);

    uint32_t nonExistId = 999999;
    EXPECT_EQ(g_removeCallbackCount, 0);
    cache_->RemoveByOriginalUniqueId(nonExistId);
    EXPECT_EQ(g_removeCallbackCount, 0);
}
} // namespace txt