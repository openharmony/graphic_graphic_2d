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

#include <chrono>
#include <numeric>
#include <vector>

#include "gtest/gtest.h"
#include "rosen_text/font_collection.h"
#include "rosen_text/typography.h"
#include "rosen_text/typography_create.h"
#include "unicode/unistr.h"

#include "text/font.h"
#include "text/font_mgr.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
constexpr int ASCII_COUNT = 500;
constexpr int CJK_COUNT = 100;
constexpr int EMOJI_COUNT = 50;

std::string BuildMixedTestString()
{
    std::string testStr = std::string(ASCII_COUNT, 'A');
    for (int i = 0; i < CJK_COUNT; i++) {
        testStr += "\xE6\x8F\x8F";
    }
    for (int i = 0; i < EMOJI_COUNT; i++) {
        testStr += "\xF0\x9F\x98\x80";
    }
    return testStr;
}

std::vector<int32_t> DecodeUtf8ToCodepoints(const std::string& testStr)
{
    icu::UnicodeString ustr = icu::UnicodeString::fromUTF8(testStr);
    std::vector<int32_t> codepoints;
    codepoints.reserve(ustr.length());
    for (int32_t i = 0; i < ustr.length();) {
        UChar32 c = ustr.char32At(i);
        codepoints.push_back(static_cast<int32_t>(c));
        i += U16_LENGTH(c);
    }
    return codepoints;
}

int64_t BenchPerCharMatch(std::shared_ptr<Drawing::FontMgr> fontMgr,
    const std::vector<int32_t>& codepoints, int iterations)
{
    Drawing::FontStyle fontStyle;
    auto t0 = std::chrono::steady_clock::now();
    for (int i = 0; i < iterations; i++) {
        for (auto cp : codepoints) {
            Drawing::Typeface* tp = fontMgr->MatchFamilyStyleCharacter(nullptr, fontStyle, nullptr, 0, cp);
            delete tp;
        }
    }
    auto t1 = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
}

int64_t BenchBatchFallback(std::shared_ptr<Drawing::FontMgr> fontMgr, const std::vector<int32_t>& codepoints,
    int iterations, std::vector<Drawing::FontFallbackInfo>& result)
{
    auto t0 = std::chrono::steady_clock::now();
    for (int i = 0; i < iterations; i++) {
        result = fontMgr->GetFallbacksForString(codepoints);
    }
    auto t1 = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
}

int64_t BenchTypographyLayout(const std::string& testStr, int iterations, std::unique_ptr<Typography>& lastTypography)
{
    auto u16Text = icu::UnicodeString::fromUTF8(testStr).getBuffer();
    auto fontCollection = FontCollection::Create();
    auto t0 = std::chrono::steady_clock::now();
    for (int i = 0; i < iterations; i++) {
        TypographyStyle typoStyle;
        auto builder = TypographyCreate::Create(typoStyle, fontCollection);
        TextStyle textStyle;
        builder->PushStyle(textStyle);
        builder->AppendText(u16Text);
        builder->PopStyle();
        lastTypography = builder->CreateTypography();
        lastTypography->Layout(static_cast<double>(INT_MAX));
        fontCollection->ClearCaches();
    }
    auto t1 = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
}

} // namespace

class DrawingFontMgrTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

protected:
    static std::shared_ptr<Drawing::FontMgr> fontMgr_;
};

std::shared_ptr<Drawing::FontMgr> DrawingFontMgrTest::fontMgr_ = nullptr;

void DrawingFontMgrTest::SetUpTestCase()
{
    fontMgr_ = Drawing::FontMgr::CreateDefaultFontMgr();
}

void DrawingFontMgrTest::TearDownTestCase()
{
    fontMgr_ = nullptr;
}

void DrawingFontMgrTest::SetUp() {}
void DrawingFontMgrTest::TearDown() {}

/**
 * @tc.name: MatchFamilyStyleCharacter_ASCII_001
 * @tc.desc: Test MatchFamilyStyleCharacter with ASCII character
 * @tc.type: FUNC
 */
HWTEST_F(DrawingFontMgrTest, MatchFamilyStyleCharacter_ASCII_001, TestSize.Level1)
{
    ASSERT_NE(fontMgr_, nullptr);
    Drawing::FontStyle fontStyle;
    Drawing::Typeface* tp = fontMgr_->MatchFamilyStyleCharacter(nullptr, fontStyle, nullptr, 0, 'A');
    ASSERT_NE(tp, nullptr);
    EXPECT_FALSE(tp->GetFamilyName().empty());
    delete tp;
}

/**
 * @tc.name: MatchFamilyStyleCharacter_Chinese_002
 * @tc.desc: Test MatchFamilyStyleCharacter with Chinese character
 * @tc.type: FUNC
 */
HWTEST_F(DrawingFontMgrTest, MatchFamilyStyleCharacter_Chinese_002, TestSize.Level1)
{
    ASSERT_NE(fontMgr_, nullptr);
    Drawing::FontStyle fontStyle;
    const char* bcp47[] = { "zh" };
    Drawing::Typeface* tp = fontMgr_->MatchFamilyStyleCharacter(nullptr, fontStyle, bcp47, 1, 0x63CF);
    ASSERT_NE(tp, nullptr);
    EXPECT_FALSE(tp->GetFamilyName().empty());
    delete tp;
}

/**
 * @tc.name: MatchFamilyStyleCharacter_Arabic_003
 * @tc.desc: Test MatchFamilyStyleCharacter with Arabic character
 * @tc.type: FUNC
 */
HWTEST_F(DrawingFontMgrTest, MatchFamilyStyleCharacter_Arabic_003, TestSize.Level1)
{
    ASSERT_NE(fontMgr_, nullptr);
    Drawing::FontStyle fontStyle;
    const char* bcp47[] = { "ar" };
    Drawing::Typeface* tp = fontMgr_->MatchFamilyStyleCharacter(nullptr, fontStyle, bcp47, 1, 0x0626);
    ASSERT_NE(tp, nullptr);
    EXPECT_FALSE(tp->GetFamilyName().empty());
    delete tp;
}

/**
 * @tc.name: MatchFamilyStyleCharacter_Emoji_004
 * @tc.desc: Test MatchFamilyStyleCharacter with Emoji
 * @tc.type: FUNC
 */
HWTEST_F(DrawingFontMgrTest, MatchFamilyStyleCharacter_Emoji_004, TestSize.Level1)
{
    ASSERT_NE(fontMgr_, nullptr);
    Drawing::FontStyle fontStyle;
    Drawing::Typeface* tp = fontMgr_->MatchFamilyStyleCharacter(nullptr, fontStyle, nullptr, 0, 0x1F600);
    ASSERT_NE(tp, nullptr);
    EXPECT_FALSE(tp->GetFamilyName().empty());
    delete tp;
}

/**
 * @tc.name: MatchFamilyStyleCharacter_NullFamily_005
 * @tc.desc: Test MatchFamilyStyleCharacter with null family and null bcp47
 * @tc.type: FUNC
 */
HWTEST_F(DrawingFontMgrTest, MatchFamilyStyleCharacter_NullFamily_005, TestSize.Level1)
{
    ASSERT_NE(fontMgr_, nullptr);
    Drawing::FontStyle fontStyle;
    Drawing::Typeface* tp = fontMgr_->MatchFamilyStyleCharacter(nullptr, fontStyle, nullptr, 0, 'Z');
    ASSERT_NE(tp, nullptr);
    delete tp;
}

/**
 * @tc.name: GetFallbacksForString_ASCII_001
 * @tc.desc: Test GetFallbacksForString with ASCII codepoints
 * @tc.type: FUNC
 */
HWTEST_F(DrawingFontMgrTest, GetFallbacksForString_ASCII_001, TestSize.Level1)
{
    ASSERT_NE(fontMgr_, nullptr);
    std::vector<int32_t> codepoints = { 'H', 'e', 'l', 'l', 'o', 'O', 'p', 'e', 'n', 'H', 'a', 'r', 'm', 'o', 'n',
        'y' };
    auto results = fontMgr_->GetFallbacksForString(codepoints);
    ASSERT_EQ(results.size(), 1);
    ASSERT_NE(results[0].typeface, nullptr);
    auto totalGlyphs = std::accumulate(
        results.begin(), results.end(), size_t(0), [](size_t sum, const auto& r) { return sum + r.glyphIds.size(); });
    EXPECT_EQ(totalGlyphs, codepoints.size());
    std::vector<uint16_t> expectedGlyphIds = { 43, 72, 79, 79, 82, 50, 83, 72, 81, 43, 68, 85, 80, 82, 81, 92 };
    EXPECT_EQ(totalGlyphs, expectedGlyphIds.size());
    for (size_t i = 0; i < results[0].glyphIds.size(); i++) {
        EXPECT_EQ(results[0].glyphIds[i], expectedGlyphIds[i]);
    }
}

/**
 * @tc.name: GetFallbacksForString_MixedScript_002
 * @tc.desc: Test GetFallbacksForString with mixed ASCII + CJK codepoints
 * @tc.type: FUNC
 */
HWTEST_F(DrawingFontMgrTest, GetFallbacksForString_MixedScript_002, TestSize.Level1)
{
    ASSERT_NE(fontMgr_, nullptr);
    std::vector<int32_t> codepoints = { 'A', 'B', 0x63CF, 0x63CF };
    auto results = fontMgr_->GetFallbacksForString(codepoints);
    ASSERT_EQ(results.size(), 2);
    auto totalGlyphs = std::accumulate(
        results.begin(), results.end(), size_t(0), [](size_t sum, const auto& r) { return sum + r.glyphIds.size(); });
    EXPECT_EQ(totalGlyphs, codepoints.size());
    // Run 1: ASCII
    ASSERT_NE(results[0].typeface, nullptr);
    ASSERT_EQ(results[0].glyphIds.size(), 2);
    EXPECT_EQ(results[0].glyphIds[0], 36);
    EXPECT_EQ(results[0].glyphIds[1], 37);
    // Run 2: CJK
    ASSERT_NE(results[1].typeface, nullptr);
    ASSERT_EQ(results[1].glyphIds.size(), 2);
    EXPECT_EQ(results[1].glyphIds[0], 13055);
    EXPECT_EQ(results[1].glyphIds[1], 13055);
}

/**
 * @tc.name: GetFallbacksForString_UTF32_003
 * @tc.desc: Test GetFallbacksForString with mixed script codepoints
 * @tc.type: FUNC
 */
HWTEST_F(DrawingFontMgrTest, GetFallbacksForString_UTF32_003, TestSize.Level1)
{
    ASSERT_NE(fontMgr_, nullptr);
    std::vector<int32_t> codepoints = { 0x0041, 0x0042, 0x0626, 0x63CF, 0x63CF };
    auto results = fontMgr_->GetFallbacksForString(codepoints);
    ASSERT_EQ(results.size(), 3);
    auto totalGlyphs = std::accumulate(
        results.begin(), results.end(), size_t(0), [](size_t sum, const auto& r) { return sum + r.glyphIds.size(); });
    EXPECT_EQ(totalGlyphs, codepoints.size());
    // Run 1: ASCII
    ASSERT_NE(results[0].typeface, nullptr);
    ASSERT_EQ(results[0].glyphIds.size(), 2);
    EXPECT_EQ(results[0].glyphIds[0], 36);
    EXPECT_EQ(results[0].glyphIds[1], 37);
    // Run 2: Arabic
    ASSERT_NE(results[1].typeface, nullptr);
    ASSERT_EQ(results[1].glyphIds.size(), 1);
    EXPECT_EQ(results[1].glyphIds[0], 331);
    // Run 3: CJK
    ASSERT_NE(results[2].typeface, nullptr);
    ASSERT_EQ(results[2].glyphIds.size(), 2);
    EXPECT_EQ(results[2].glyphIds[0], 13055);
    EXPECT_EQ(results[2].glyphIds[1], 13055);
}

/**
 * @tc.name: GetFallbacksForString_Emoji_004
 * @tc.desc: Test GetFallbacksForString with Emoji codepoints
 * @tc.type: FUNC
 */
HWTEST_F(DrawingFontMgrTest, GetFallbacksForString_Emoji_004, TestSize.Level1)
{
    ASSERT_NE(fontMgr_, nullptr);
    std::vector<int32_t> codepoints = { 0x1F600, 0x1F600, 0x1F600 };
    auto results = fontMgr_->GetFallbacksForString(codepoints);
    ASSERT_EQ(results.size(), 1);
    ASSERT_NE(results[0].typeface, nullptr);
    auto totalGlyphs = std::accumulate(
        results.begin(), results.end(), size_t(0), [](size_t sum, const auto& r) { return sum + r.glyphIds.size(); });
    EXPECT_EQ(totalGlyphs, codepoints.size());
    ASSERT_EQ(results[0].glyphIds.size(), 3);
    EXPECT_EQ(results[0].glyphIds[0], 857);
    EXPECT_EQ(results[0].glyphIds[1], 857);
    EXPECT_EQ(results[0].glyphIds[2], 857);
}

/**
 * @tc.name: GetFallbacksForString_NullText_005
 * @tc.desc: Test GetFallbacksForString with empty input
 * @tc.type: FUNC
 */
HWTEST_F(DrawingFontMgrTest, GetFallbacksForString_NullText_005, TestSize.Level1)
{
    ASSERT_NE(fontMgr_, nullptr);
    std::vector<int32_t> codepoints;
    auto results = fontMgr_->GetFallbacksForString(codepoints);
    EXPECT_TRUE(results.empty());
}

/**
 * @tc.name: GetFallbacksForString_Deduplication_006
 * @tc.desc: Test that consecutive same-Typeface characters are merged into one run
 * @tc.type: FUNC
 */
HWTEST_F(DrawingFontMgrTest, GetFallbacksForString_Deduplication_006, TestSize.Level1)
{
    ASSERT_NE(fontMgr_, nullptr);
    std::vector<int32_t> codepoints(10, 'A');
    auto results = fontMgr_->GetFallbacksForString(codepoints);
    ASSERT_EQ(results.size(), 1);
    ASSERT_NE(results[0].typeface, nullptr);
    auto totalGlyphs = std::accumulate(
        results.begin(), results.end(), size_t(0), [](size_t sum, const auto& r) { return sum + r.glyphIds.size(); });
    EXPECT_EQ(totalGlyphs, codepoints.size());
    ASSERT_EQ(results[0].glyphIds.size(), 10);
    for (size_t i = 0; i < results[0].glyphIds.size(); i++) {
        EXPECT_EQ(results[0].glyphIds[i], 36);
    }
}

/**
 * @tc.name: GetFallbacksForString_BCP47_007
 * @tc.desc: Test GetFallbacksForString with bcp47 locale hint for CJK
 * @tc.type: FUNC
 */
HWTEST_F(DrawingFontMgrTest, GetFallbacksForString_BCP47_007, TestSize.Level1)
{
    ASSERT_NE(fontMgr_, nullptr);
    std::vector<int32_t> codepoints = { 0x63CF, 0x63CF };
    auto results = fontMgr_->GetFallbacksForString(codepoints, nullptr, {}, { "zh-Hant" });
    ASSERT_EQ(results.size(), 1);
    ASSERT_NE(results[0].typeface, nullptr);
    auto totalGlyphs = std::accumulate(
        results.begin(), results.end(), size_t(0), [](size_t sum, const auto& r) { return sum + r.glyphIds.size(); });
    EXPECT_EQ(totalGlyphs, codepoints.size());
    ASSERT_EQ(results[0].glyphIds.size(), 2);
    EXPECT_EQ(results[0].glyphIds[0], 4184);
    EXPECT_EQ(results[0].glyphIds[1], 4184);
    EXPECT_FALSE(results[0].typeface->GetFamilyName().empty());
}

/**
 * @tc.name: GetFallbacksForString_MixedAll_008
 * @tc.desc: Test GetFallbacksForString with mixed ASCII + CJK + Emoji codepoints
 * @tc.type: FUNC
 */
HWTEST_F(DrawingFontMgrTest, GetFallbacksForString_MixedAll_008, TestSize.Level1)
{
    ASSERT_NE(fontMgr_, nullptr);
    std::vector<int32_t> codepoints = { 'A', 'B', 0x63CF, 0x63CF, 0x1F600, 0x1F600 };
    auto results = fontMgr_->GetFallbacksForString(codepoints);
    ASSERT_EQ(results.size(), 3);
    auto totalGlyphs = std::accumulate(
        results.begin(), results.end(), size_t(0), [](size_t sum, const auto& r) { return sum + r.glyphIds.size(); });
    EXPECT_EQ(totalGlyphs, codepoints.size());
    // Run 1: ASCII
    ASSERT_NE(results[0].typeface, nullptr);
    ASSERT_EQ(results[0].glyphIds.size(), 2);
    EXPECT_EQ(results[0].glyphIds[0], 36);
    EXPECT_EQ(results[0].glyphIds[1], 37);
    // Run 2: CJK
    ASSERT_NE(results[1].typeface, nullptr);
    ASSERT_EQ(results[1].glyphIds.size(), 2);
    EXPECT_EQ(results[1].glyphIds[0], 13055);
    EXPECT_EQ(results[1].glyphIds[1], 13055);
    // Run 3: Emoji
    ASSERT_NE(results[2].typeface, nullptr);
    ASSERT_EQ(results[2].glyphIds.size(), 2);
    EXPECT_EQ(results[2].glyphIds[0], 857);
    EXPECT_EQ(results[2].glyphIds[1], 857);
}

/**
 * @tc.name: GetFallbacksForString_UnmatchedCodepoint_012
 * @tc.desc: Test GetFallbacksForString with invalid codepoint produces nullptr run with glyphId 0
 * @tc.type: FUNC
 */
HWTEST_F(DrawingFontMgrTest, GetFallbacksForString_UnmatchedCodepoint_012, TestSize.Level1)
{
    ASSERT_NE(fontMgr_, nullptr);
    std::vector<int32_t> codepoints = { 0x110000 };
    auto results = fontMgr_->GetFallbacksForString(codepoints);
    ASSERT_EQ(results.size(), 1);
    auto totalGlyphs = std::accumulate(
        results.begin(), results.end(), size_t(0), [](size_t sum, const auto& r) { return sum + r.glyphIds.size(); });
    EXPECT_EQ(totalGlyphs, codepoints.size());
    EXPECT_EQ(results[0].typeface, nullptr);
    ASSERT_EQ(results[0].glyphIds.size(), 1);
    EXPECT_EQ(results[0].glyphIds[0], 0);
}

/**
 * @tc.name: GetFallbacksForString_WithTypeface_013
 * @tc.desc: Test GetFallbacksForString with input typeface that covers all codepoints
 * @tc.type: FUNC
 */
HWTEST_F(DrawingFontMgrTest, GetFallbacksForString_WithTypeface_013, TestSize.Level1)
{
    ASSERT_NE(fontMgr_, nullptr);
    Drawing::FontStyle fontStyle;
    Drawing::Typeface* tp = fontMgr_->MatchFamilyStyleCharacter(nullptr, fontStyle, nullptr, 0, 'A');
    ASSERT_NE(tp, nullptr);
    auto typeface = std::shared_ptr<Drawing::Typeface>(tp);

    std::vector<int32_t> codepoints = { 'A', 'B', 'C' };
    auto results = fontMgr_->GetFallbacksForString(codepoints, typeface);
    ASSERT_EQ(results.size(), 1);
    EXPECT_EQ(results[0].typeface, typeface);
    auto totalGlyphs = std::accumulate(
        results.begin(), results.end(), size_t(0), [](size_t sum, const auto& r) { return sum + r.glyphIds.size(); });
    EXPECT_EQ(totalGlyphs, codepoints.size());
    ASSERT_EQ(results[0].glyphIds.size(), 3);
    EXPECT_EQ(results[0].glyphIds[0], 36);
    EXPECT_EQ(results[0].glyphIds[1], 37);
    EXPECT_EQ(results[0].glyphIds[2], 38);
}

/**
 * @tc.name: GetFallbacksForString_TypefaceFallbackNull_014
 * @tc.desc: Test GetFallbacksForString where input typeface covers 'A' but
 *           MatchFamilyStyleCharacter returns nullptr for invalid codepoint (rawTp == nullptr).
 *           Result has two runs: matched run then nullptr run.
 * @tc.type: FUNC
 */
HWTEST_F(DrawingFontMgrTest, GetFallbacksForString_TypefaceFallbackNull_014, TestSize.Level1)
{
    ASSERT_NE(fontMgr_, nullptr);
    Drawing::FontStyle fontStyle;
    Drawing::Typeface* tp = fontMgr_->MatchFamilyStyleCharacter(nullptr, fontStyle, nullptr, 0, 'A');
    ASSERT_NE(tp, nullptr);
    auto typeface = std::shared_ptr<Drawing::Typeface>(tp);

    // 'A' covered by input typeface, 0x110000 has no font (rawTp == nullptr)
    std::vector<int32_t> codepoints = { 'A', 0x110000 };
    auto results = fontMgr_->GetFallbacksForString(codepoints, typeface);
    ASSERT_EQ(results.size(), 2);
    auto totalGlyphs = std::accumulate(
        results.begin(), results.end(), size_t(0), [](size_t sum, const auto& r) { return sum + r.glyphIds.size(); });
    EXPECT_EQ(totalGlyphs, codepoints.size());
    // Run 1: matched by input typeface
    EXPECT_EQ(results[0].typeface, typeface);
    ASSERT_EQ(results[0].glyphIds.size(), 1);
    EXPECT_EQ(results[0].glyphIds[0], 36);
    // Run 2: nullptr with glyphId 0
    EXPECT_EQ(results[1].typeface, nullptr);
    ASSERT_EQ(results[1].glyphIds.size(), 1);
    EXPECT_EQ(results[1].glyphIds[0], 0);
}

/**
 * @tc.name: GetFallbacksForString_NullptrRunFirst_015
 * @tc.desc: Test that nullptr run appears first when leading codepoints are unmatched
 * @tc.type: FUNC
 */
HWTEST_F(DrawingFontMgrTest, GetFallbacksForString_NullptrRunFirst_015, TestSize.Level1)
{
    ASSERT_NE(fontMgr_, nullptr);
    // 0x110000 unmatched (rawTp == nullptr), then 'A' matched by system fallback
    std::vector<int32_t> codepoints = { 0x110000, 'A' };
    auto results = fontMgr_->GetFallbacksForString(codepoints);
    ASSERT_EQ(results.size(), 2);
    auto totalGlyphs = std::accumulate(
        results.begin(), results.end(), size_t(0), [](size_t sum, const auto& r) { return sum + r.glyphIds.size(); });
    EXPECT_EQ(totalGlyphs, codepoints.size());
    // Run 1: nullptr
    EXPECT_EQ(results[0].typeface, nullptr);
    ASSERT_EQ(results[0].glyphIds.size(), 1);
    EXPECT_EQ(results[0].glyphIds[0], 0);
    // Run 2: matched
    EXPECT_NE(results[1].typeface, nullptr);
    ASSERT_EQ(results[1].glyphIds.size(), 1);
    EXPECT_EQ(results[1].glyphIds[0], 36);
}

/**
 * @tc.name: GetFallbacksForString_NullptrBetweenMatched_016
 * @tc.desc: Test nullptr run between two matched runs with same typeface
 * @tc.type: FUNC
 */
HWTEST_F(DrawingFontMgrTest, GetFallbacksForString_NullptrBetweenMatched_016, TestSize.Level1)
{
    ASSERT_NE(fontMgr_, nullptr);
    // 'A' and 'B' likely same typeface, 0x110000 unmatched in between
    std::vector<int32_t> codepoints = { 'A', 0x110000, 'B' };
    auto results = fontMgr_->GetFallbacksForString(codepoints);
    ASSERT_EQ(results.size(), 3);
    auto totalGlyphs = std::accumulate(
        results.begin(), results.end(), size_t(0), [](size_t sum, const auto& r) { return sum + r.glyphIds.size(); });
    EXPECT_EQ(totalGlyphs, codepoints.size());
    // Run 1: matched
    EXPECT_NE(results[0].typeface, nullptr);
    ASSERT_EQ(results[0].glyphIds.size(), 1);
    EXPECT_EQ(results[0].glyphIds[0], 36);
    // Run 2: nullptr
    EXPECT_EQ(results[1].typeface, nullptr);
    ASSERT_EQ(results[1].glyphIds.size(), 1);
    EXPECT_EQ(results[1].glyphIds[0], 0);
    // Run 3: matched (may or may not be same typeface as run 1)
    EXPECT_NE(results[2].typeface, nullptr);
    ASSERT_EQ(results[2].glyphIds.size(), 1);
    EXPECT_EQ(results[2].glyphIds[0], 37);
}

/**
 * @tc.name: GetFallbacksForString_ConsecutiveNullptr_017
 * @tc.desc: Test that consecutive unmatched codepoints merge into one nullptr run
 * @tc.type: FUNC
 */
HWTEST_F(DrawingFontMgrTest, GetFallbacksForString_ConsecutiveNullptr_017, TestSize.Level1)
{
    ASSERT_NE(fontMgr_, nullptr);
    std::vector<int32_t> codepoints = { 0x110000, 0x110001, 0x110002 };
    auto results = fontMgr_->GetFallbacksForString(codepoints);
    ASSERT_EQ(results.size(), 1);
    auto totalGlyphs = std::accumulate(
        results.begin(), results.end(), size_t(0), [](size_t sum, const auto& r) { return sum + r.glyphIds.size(); });
    EXPECT_EQ(totalGlyphs, codepoints.size());
    EXPECT_EQ(results[0].typeface, nullptr);
    ASSERT_EQ(results[0].glyphIds.size(), 3);
    for (auto gid : results[0].glyphIds) {
        EXPECT_EQ(gid, 0);
    }
}

/**
 * @tc.name: Perf_CompareFallbackApproaches_001
 * @tc.desc: Performance comparison - per-char loop vs batch API vs Typography layout
 * @tc.type: FUNC
 */
HWTEST_F(DrawingFontMgrTest, Perf_CompareFallbackApproaches_001, TestSize.Level1)
{
    ASSERT_NE(fontMgr_, nullptr);
    std::string testStr = BuildMixedTestString();
    auto codepoints = DecodeUtf8ToCodepoints(testStr);
    constexpr int iterations = 100;

    auto perCharUs = BenchPerCharMatch(fontMgr_, codepoints, iterations);
    std::vector<Drawing::FontFallbackInfo> batchResult;
    auto batchUs = BenchBatchFallback(fontMgr_, codepoints, iterations, batchResult);
    std::unique_ptr<Typography> lastTypography;
    auto layoutUs = BenchTypographyLayout(testStr, iterations, lastTypography);

    GTEST_LOG_(INFO) << "PerChar: " << perCharUs << " us, Batch: " << batchUs << " us, Layout: " << layoutUs << " us";
    EXPECT_LT(batchUs, perCharUs);
    EXPECT_LT(batchUs, layoutUs);

    ASSERT_FALSE(batchResult.empty());
    auto totalGlyphs = std::accumulate(batchResult.begin(), batchResult.end(), size_t(0),
        [](size_t sum, const auto& r) { return sum + r.glyphIds.size(); });
    EXPECT_EQ(totalGlyphs, codepoints.size());
    ASSERT_NE(lastTypography, nullptr);
    EXPECT_GT(lastTypography->GetHeight(), 0.0);
}

} // namespace Rosen
} // namespace OHOS
