/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "drawing_font_collection.h"
#include "drawing_text_typography.h"
#include "gtest/gtest.h"
#include "impl/paragraph_impl.h"
#include "modules/skparagraph/include/TextStyle.h"
#include "rosen_text/typography.h"
#include "rosen_text/typography_create.h"
#include "text_style.h"
#include "typography_types.h"

using namespace OHOS::Rosen;
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace {

const char* DEFAULT_TEXT_UTF8 = "Heน้ำl👨‍👩‍👧lo1️⃣World";
const char16_t* DEFAULT_TEXT_UTF16 = u"Heน้ำl👨‍👩‍👧lo1️⃣World";
const char* ARABIC_TEXT_UTF8 = "للآباء والأمهات";
const char16_t* ARABIC_TEXT_UTF16 = u"للآباء والأمهات";
const char* COMPLEX_TEXT_UTF8 = "Heللآبllo कि";
const char16_t* COMPLEX_TEXT_UTF16 = u"Heللآبllo कि";

const double DEFAULT_FONT_SIZE = 50.0;
const double MAX_WIDTH = 400.0;
const double TEST_COORDINATE_X = 10.0;
const double TEST_COORDINATE_Y = 10.0;
const double BIG_VALUE_COORDINATE = 1000.0;
const double NEGATIVE_COORDINATE = -10.0;
const int AFFINITY_UPSTREAM = 0;
const int AFFINITY_DOWNSTREAM = 1;
}

class NdkTypographyCharacterIndexTest : public testing::Test {
public:
    void SetUp() override;
    void TearDown() override;
    void CreateTypography();
    void SetupTypography(const char* text);
    void SetupTypographyWithEncodedText(const char16_t* text);

protected:
    OH_Drawing_TypographyCreate* fHandler{nullptr};
    OH_Drawing_Typography* fTypography{nullptr};
};

void NdkTypographyCharacterIndexTest::SetUp()
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_SetTypographyTextFontSize(typoStyle, DEFAULT_FONT_SIZE);
    fHandler = OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    ASSERT_NE(fHandler, nullptr);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

void NdkTypographyCharacterIndexTest::TearDown()
{
    if (fHandler != nullptr) {
        OH_Drawing_DestroyTypographyHandler(fHandler);
        fHandler = nullptr;
    }
    if (fTypography != nullptr) {
        OH_Drawing_DestroyTypography(fTypography);
        fTypography = nullptr;
    }
}

void NdkTypographyCharacterIndexTest::CreateTypography()
{
    fTypography = OH_Drawing_CreateTypography(fHandler);
    ASSERT_NE(fTypography, nullptr);
}

void NdkTypographyCharacterIndexTest::SetupTypography(const char* text)
{
    OH_Drawing_TypographyHandlerAddText(fHandler, text);
    CreateTypography();
    OH_Drawing_TypographyLayout(fTypography, MAX_WIDTH);
}

void NdkTypographyCharacterIndexTest::SetupTypographyWithEncodedText(const char16_t* text)
{
    size_t byteLength = std::char_traits<char16_t>::length(text) * sizeof(char16_t);
    OH_Drawing_TypographyHandlerAddEncodedText(fHandler, text, byteLength, TEXT_ENCODING_UTF16);
    CreateTypography();
    OH_Drawing_TypographyLayout(fTypography, MAX_WIDTH);
}

/*
 * @tc.name: TypographyGetCharacterPositionAtCoordinateNullTypographyUtf8Test001
 * @tc.desc: Test null typography with UTF-8 encoding
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyCharacterIndexTest, TypographyGetCharacterPositionAtCoordinateNullTypographyUtf8Test001,
    TestSize.Level0)
{
    auto positionAndAffinity = OH_Drawing_TypographyGetCharacterPositionAtCoordinateWithBuffer(
        nullptr, TEST_COORDINATE_X, TEST_COORDINATE_Y, TEXT_ENCODING_UTF8);
    EXPECT_EQ(positionAndAffinity, nullptr);
    OH_Drawing_DestroyPositionAndAffinity(positionAndAffinity);
}

/*
 * @tc.name: TypographyGetCharacterPositionAtCoordinateNullTypographyUtf16Test001
 * @tc.desc: Test null typography with UTF-16 encoding
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyCharacterIndexTest, TypographyGetCharacterPositionAtCoordinateNullTypographyUtf16Test001,
    TestSize.Level0)
{
    auto positionAndAffinity = OH_Drawing_TypographyGetCharacterPositionAtCoordinateWithBuffer(
        nullptr, TEST_COORDINATE_X, TEST_COORDINATE_Y, TEXT_ENCODING_UTF16);
    EXPECT_EQ(positionAndAffinity, nullptr);
    OH_Drawing_DestroyPositionAndAffinity(positionAndAffinity);
}

/*
 * @tc.name: TypographyGetCharacterPositionAtCoordinateInvalidEncodingTest003
 * @tc.desc: Test with invalid encoding type
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyCharacterIndexTest, TypographyGetCharacterPositionAtCoordinateInvalidEncodingTest003,
    TestSize.Level0)
{
    SetupTypography(DEFAULT_TEXT_UTF8);
    auto positionAndAffinity = OH_Drawing_TypographyGetCharacterPositionAtCoordinateWithBuffer(
        fTypography, TEST_COORDINATE_X, TEST_COORDINATE_Y, (OH_Drawing_TextEncoding)999);
    EXPECT_EQ(positionAndAffinity, nullptr);
    OH_Drawing_DestroyPositionAndAffinity(positionAndAffinity);
}

/*
 * @tc.name: TypographyGetCharacterPositionAtCoordinateNormalUtf8Test001
 * @tc.desc: Test UTF-8 encoding at normal coordinate
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyCharacterIndexTest, TypographyGetCharacterPositionAtCoordinateNormalUtf8Test001,
    TestSize.Level0)
{
    SetupTypography(DEFAULT_TEXT_UTF8);
    auto positionAndAffinity = OH_Drawing_TypographyGetCharacterPositionAtCoordinateWithBuffer(
        fTypography, TEST_COORDINATE_X, TEST_COORDINATE_Y, TEXT_ENCODING_UTF8);
    ASSERT_NE(positionAndAffinity, nullptr);
    size_t pos = OH_Drawing_GetPositionFromPositionAndAffinity(positionAndAffinity);
    EXPECT_EQ(pos, 0);
    int affinity = OH_Drawing_GetAffinityFromPositionAndAffinity(positionAndAffinity);
    EXPECT_EQ(affinity, AFFINITY_DOWNSTREAM);
    OH_Drawing_DestroyPositionAndAffinity(positionAndAffinity);
}

/*
 * @tc.name: TypographyGetCharacterPositionAtCoordinateNegativeUtf8Test001
 * @tc.desc: Test UTF-8 encoding at negative coordinates
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyCharacterIndexTest, TypographyGetCharacterPositionAtCoordinateNegativeUtf8Test001,
    TestSize.Level0)
{
    SetupTypography(DEFAULT_TEXT_UTF8);
    auto positionAndAffinity = OH_Drawing_TypographyGetCharacterPositionAtCoordinateWithBuffer(
        fTypography, NEGATIVE_COORDINATE, NEGATIVE_COORDINATE, TEXT_ENCODING_UTF8);
    ASSERT_NE(positionAndAffinity, nullptr);
    size_t pos = OH_Drawing_GetPositionFromPositionAndAffinity(positionAndAffinity);
    EXPECT_EQ(pos, 0);
    int affinity = OH_Drawing_GetAffinityFromPositionAndAffinity(positionAndAffinity);
    EXPECT_EQ(affinity, AFFINITY_DOWNSTREAM);
    OH_Drawing_DestroyPositionAndAffinity(positionAndAffinity);
}

/*
 * @tc.name: TypographyGetCharacterPositionAtCoordinateOutOfBoundUtf8Test001
 * @tc.desc: Test UTF-8 encoding at out of bounds coordinates
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyCharacterIndexTest, TypographyGetCharacterPositionAtCoordinateOutOfBoundUtf8Test001,
    TestSize.Level0)
{
    SetupTypography(DEFAULT_TEXT_UTF8);
    auto positionAndAffinity = OH_Drawing_TypographyGetCharacterPositionAtCoordinateWithBuffer(
        fTypography, BIG_VALUE_COORDINATE, BIG_VALUE_COORDINATE, TEXT_ENCODING_UTF8);
    ASSERT_NE(positionAndAffinity, nullptr);
    size_t pos = OH_Drawing_GetPositionFromPositionAndAffinity(positionAndAffinity);
    EXPECT_EQ(pos, 44);
    int affinity = OH_Drawing_GetAffinityFromPositionAndAffinity(positionAndAffinity);
    EXPECT_EQ(affinity, AFFINITY_UPSTREAM);
    OH_Drawing_DestroyPositionAndAffinity(positionAndAffinity);
}

/*
 * @tc.name: TypographyGetCharacterPositionAtCoordinateComplexUtf8Test001
 * @tc.desc: Test complex text with UTF-8 encoding
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyCharacterIndexTest, TypographyGetCharacterPositionAtCoordinateComplexUtf8Test001,
    TestSize.Level0)
{
    SetupTypography(COMPLEX_TEXT_UTF8);
    auto positionAndAffinity = OH_Drawing_TypographyGetCharacterPositionAtCoordinateWithBuffer(
        fTypography, TEST_COORDINATE_X, TEST_COORDINATE_Y, TEXT_ENCODING_UTF8);
    ASSERT_NE(positionAndAffinity, nullptr);
    size_t pos = OH_Drawing_GetPositionFromPositionAndAffinity(positionAndAffinity);
    EXPECT_EQ(pos, 0);
    int affinity = OH_Drawing_GetAffinityFromPositionAndAffinity(positionAndAffinity);
    EXPECT_EQ(affinity, AFFINITY_DOWNSTREAM);
    OH_Drawing_DestroyPositionAndAffinity(positionAndAffinity);
}

/*
 * @tc.name: TypographyGetCharacterPositionAtCoordinateArabicUtf8Test001
 * @tc.desc: Test Arabic text with UTF-8 encoding
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyCharacterIndexTest, TypographyGetCharacterPositionAtCoordinateArabicUtf8Test001,
    TestSize.Level0)
{
    SetupTypography(ARABIC_TEXT_UTF8);
    auto positionAndAffinity = OH_Drawing_TypographyGetCharacterPositionAtCoordinateWithBuffer(
        fTypography, TEST_COORDINATE_X, TEST_COORDINATE_Y, TEXT_ENCODING_UTF8);
    ASSERT_NE(positionAndAffinity, nullptr);
    size_t pos = OH_Drawing_GetPositionFromPositionAndAffinity(positionAndAffinity);
    EXPECT_EQ(pos, 29);
    int affinity = OH_Drawing_GetAffinityFromPositionAndAffinity(positionAndAffinity);
    EXPECT_EQ(affinity, AFFINITY_UPSTREAM);
    OH_Drawing_DestroyPositionAndAffinity(positionAndAffinity);
}

/*
 * @tc.name: TypographyGetCharacterPositionAtCoordinateNormalUtf16Test001
 * @tc.desc: Test UTF-16 encoding at normal coordinate
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyCharacterIndexTest, TypographyGetCharacterPositionAtCoordinateNormalUtf16Test001,
    TestSize.Level0)
{
    SetupTypographyWithEncodedText(DEFAULT_TEXT_UTF16);
    auto positionAndAffinity = OH_Drawing_TypographyGetCharacterPositionAtCoordinateWithBuffer(
        fTypography, TEST_COORDINATE_X, TEST_COORDINATE_Y, TEXT_ENCODING_UTF16);
    ASSERT_NE(positionAndAffinity, nullptr);
    size_t pos = OH_Drawing_GetPositionFromPositionAndAffinity(positionAndAffinity);
    EXPECT_EQ(pos, 0);
    int affinity = OH_Drawing_GetAffinityFromPositionAndAffinity(positionAndAffinity);
    EXPECT_EQ(affinity, AFFINITY_DOWNSTREAM);
    OH_Drawing_DestroyPositionAndAffinity(positionAndAffinity);
}

/*
 * @tc.name: TypographyGetCharacterPositionAtCoordinateNegativeUtf16Test001
 * @tc.desc: Test UTF-16 encoding at negative coordinates
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyCharacterIndexTest, TypographyGetCharacterPositionAtCoordinateNegativeUtf16Test001,
    TestSize.Level0)
{
    SetupTypographyWithEncodedText(DEFAULT_TEXT_UTF16);
    auto positionAndAffinity = OH_Drawing_TypographyGetCharacterPositionAtCoordinateWithBuffer(
        fTypography, NEGATIVE_COORDINATE, NEGATIVE_COORDINATE, TEXT_ENCODING_UTF16);
    ASSERT_NE(positionAndAffinity, nullptr);
    size_t pos = OH_Drawing_GetPositionFromPositionAndAffinity(positionAndAffinity);
    EXPECT_EQ(pos, 0);
    int affinity = OH_Drawing_GetAffinityFromPositionAndAffinity(positionAndAffinity);
    EXPECT_EQ(affinity, AFFINITY_DOWNSTREAM);
    OH_Drawing_DestroyPositionAndAffinity(positionAndAffinity);
}

/*
 * @tc.name: TypographyGetCharacterPositionAtCoordinateOutOfBoundUtf16Test001
 * @tc.desc: Test UTF-16 encoding at out of bounds coordinates
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyCharacterIndexTest, TypographyGetCharacterPositionAtCoordinateOutOfBoundUtf16Test001,
    TestSize.Level0)
{
    SetupTypographyWithEncodedText(DEFAULT_TEXT_UTF16);
    auto positionAndAffinity = OH_Drawing_TypographyGetCharacterPositionAtCoordinateWithBuffer(
        fTypography, BIG_VALUE_COORDINATE, BIG_VALUE_COORDINATE, TEXT_ENCODING_UTF16);
    ASSERT_NE(positionAndAffinity, nullptr);
    size_t pos = OH_Drawing_GetPositionFromPositionAndAffinity(positionAndAffinity);
    EXPECT_EQ(pos, 24);
    int affinity = OH_Drawing_GetAffinityFromPositionAndAffinity(positionAndAffinity);
    EXPECT_EQ(affinity, AFFINITY_UPSTREAM);
    OH_Drawing_DestroyPositionAndAffinity(positionAndAffinity);
}

/*
 * @tc.name: TypographyGetCharacterPositionAtCoordinateComplexUtf16Test001
 * @tc.desc: Test complex text with UTF-16 encoding
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyCharacterIndexTest, TypographyGetCharacterPositionAtCoordinateComplexUtf16Test001,
    TestSize.Level0)
{
    SetupTypographyWithEncodedText(COMPLEX_TEXT_UTF16);
    auto positionAndAffinity = OH_Drawing_TypographyGetCharacterPositionAtCoordinateWithBuffer(
        fTypography, TEST_COORDINATE_X, TEST_COORDINATE_Y, TEXT_ENCODING_UTF16);
    ASSERT_NE(positionAndAffinity, nullptr);
    size_t pos = OH_Drawing_GetPositionFromPositionAndAffinity(positionAndAffinity);
    EXPECT_EQ(pos, 0);
    int affinity = OH_Drawing_GetAffinityFromPositionAndAffinity(positionAndAffinity);
    EXPECT_EQ(affinity, AFFINITY_DOWNSTREAM);
    OH_Drawing_DestroyPositionAndAffinity(positionAndAffinity);
}

/*
 * @tc.name: TypographyGetCharacterPositionAtCoordinateArabicUtf16Test001
 * @tc.desc: Test Arabic text with UTF-16 encoding
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyCharacterIndexTest, TypographyGetCharacterPositionAtCoordinateArabicUtf16Test001,
    TestSize.Level0)
{
    SetupTypographyWithEncodedText(ARABIC_TEXT_UTF16);
    auto positionAndAffinity = OH_Drawing_TypographyGetCharacterPositionAtCoordinateWithBuffer(
        fTypography, TEST_COORDINATE_X, TEST_COORDINATE_Y, TEXT_ENCODING_UTF16);
    ASSERT_NE(positionAndAffinity, nullptr);
    size_t pos = OH_Drawing_GetPositionFromPositionAndAffinity(positionAndAffinity);
    EXPECT_EQ(pos, 15);
    int affinity = OH_Drawing_GetAffinityFromPositionAndAffinity(positionAndAffinity);
    EXPECT_EQ(affinity, AFFINITY_UPSTREAM);
    OH_Drawing_DestroyPositionAndAffinity(positionAndAffinity);
}

/*
 * @tc.name: TypographyCharacterIndexMultilingualTest001
 * @tc.desc: Test glyph to character conversion with UTF-8
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyCharacterIndexTest, TypographyCharacterIndexMultilingualTest001, TestSize.Level0)
{
    SetupTypography(DEFAULT_TEXT_UTF8);
    OH_Drawing_Range* actualGlyphRange = nullptr;
    OH_Drawing_Range* characterRange = OH_Drawing_TypographyGetCharacterRangeForGlyphRangeWithBuffer(
        fTypography, 3, 9, &actualGlyphRange, TEXT_ENCODING_UTF8);
    ASSERT_NE(characterRange, nullptr);
    ASSERT_NE(actualGlyphRange, nullptr);
    EXPECT_EQ(OH_Drawing_GetStartFromRange(characterRange), 2);
    EXPECT_EQ(OH_Drawing_GetEndFromRange(characterRange), 31);
    EXPECT_EQ(OH_Drawing_GetStartFromRange(actualGlyphRange), 2);
    EXPECT_EQ(OH_Drawing_GetEndFromRange(actualGlyphRange), 9);
    OH_Drawing_ReleaseRangeBuffer(nullptr);
    OH_Drawing_ReleaseRangeBuffer(characterRange);
    OH_Drawing_ReleaseRangeBuffer(actualGlyphRange);
}

/*
 * @tc.name: TypographyCharacterIndexMultilingualTest002
 * @tc.desc: Test character to glyph conversion with UTF-8
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyCharacterIndexTest, TypographyCharacterIndexMultilingualTest002, TestSize.Level0)
{
    SetupTypography(DEFAULT_TEXT_UTF8);
    OH_Drawing_Range* actualCharacterRange = nullptr;
    OH_Drawing_Range* glyphRange = OH_Drawing_TypographyGetGlyphRangeForCharacterRangeWithBuffer(
        fTypography, 3, 9, &actualCharacterRange, TEXT_ENCODING_UTF8);
    ASSERT_NE(glyphRange, nullptr);
    ASSERT_NE(actualCharacterRange, nullptr);
    EXPECT_EQ(OH_Drawing_GetStartFromRange(glyphRange), 2);
    EXPECT_EQ(OH_Drawing_GetEndFromRange(glyphRange), 6);
    EXPECT_EQ(OH_Drawing_GetStartFromRange(actualCharacterRange), 2);
    EXPECT_EQ(OH_Drawing_GetEndFromRange(actualCharacterRange), 11);
    OH_Drawing_ReleaseRangeBuffer(glyphRange);
    OH_Drawing_ReleaseRangeBuffer(actualCharacterRange);
}

/*
 * @tc.name: TypographyCharacterIndexMultilingualTest003
 * @tc.desc: Test glyph to character conversion with UTF-16
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyCharacterIndexTest, TypographyCharacterIndexMultilingualTest003, TestSize.Level0)
{
    SetupTypographyWithEncodedText(DEFAULT_TEXT_UTF16);
    OH_Drawing_Range* actualGlyphRange = nullptr;
    OH_Drawing_Range* characterRange = OH_Drawing_TypographyGetCharacterRangeForGlyphRangeWithBuffer(
        fTypography, 3, 8, &actualGlyphRange, TEXT_ENCODING_UTF16);
    ASSERT_NE(characterRange, nullptr);
    ASSERT_NE(actualGlyphRange, nullptr);
    EXPECT_EQ(OH_Drawing_GetStartFromRange(characterRange), 2);
    EXPECT_EQ(OH_Drawing_GetEndFromRange(characterRange), 14);
    EXPECT_EQ(OH_Drawing_GetStartFromRange(actualGlyphRange), 2);
    EXPECT_EQ(OH_Drawing_GetEndFromRange(actualGlyphRange), 8);
    OH_Drawing_ReleaseRangeBuffer(characterRange);
    OH_Drawing_ReleaseRangeBuffer(actualGlyphRange);
}

/*
 * @tc.name: TypographyCharacterIndexMultilingualTest004
 * @tc.desc: Test character to glyph conversion with UTF-16
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyCharacterIndexTest, TypographyCharacterIndexMultilingualTest004, TestSize.Level0)
{
    SetupTypographyWithEncodedText(DEFAULT_TEXT_UTF16);
    OH_Drawing_Range* actualCharacterRange = nullptr;
    OH_Drawing_Range* glyphRange = OH_Drawing_TypographyGetGlyphRangeForCharacterRangeWithBuffer(
        fTypography, 3, 9, &actualCharacterRange, TEXT_ENCODING_UTF16);
    ASSERT_NE(glyphRange, nullptr);
    ASSERT_NE(actualCharacterRange, nullptr);
    EXPECT_EQ(OH_Drawing_GetStartFromRange(glyphRange), 2);
    EXPECT_EQ(OH_Drawing_GetEndFromRange(glyphRange), 8);
    EXPECT_EQ(OH_Drawing_GetStartFromRange(actualCharacterRange), 2);
    EXPECT_EQ(OH_Drawing_GetEndFromRange(actualCharacterRange), 14);

    OH_Drawing_ReleaseRangeBuffer(glyphRange);
    OH_Drawing_ReleaseRangeBuffer(actualCharacterRange);
}

/*
 * @tc.name: TypographyCharacterIndexMultilingualTest005
 * @tc.desc: Test glyph to character conversion with Arabic RTL using UTF-8
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyCharacterIndexTest, TypographyCharacterIndexMultilingualTest005, TestSize.Level0)
{
    SetupTypography(ARABIC_TEXT_UTF8);
    OH_Drawing_Range* actualGlyphRange = nullptr;
    OH_Drawing_Range* characterRange = OH_Drawing_TypographyGetCharacterRangeForGlyphRangeWithBuffer(
        fTypography, 0, 5, &actualGlyphRange, TEXT_ENCODING_UTF8);
    ASSERT_NE(characterRange, nullptr);
    ASSERT_NE(actualGlyphRange, nullptr);
    EXPECT_EQ(OH_Drawing_GetStartFromRange(characterRange), 0);
    EXPECT_EQ(OH_Drawing_GetEndFromRange(characterRange), 12);
    OH_Drawing_ReleaseRangeBuffer(characterRange);
    OH_Drawing_ReleaseRangeBuffer(actualGlyphRange);
}

/*
 * @tc.name: TypographyCharacterIndexMultilingualTest006
 * @tc.desc: Test character to glyph conversion with Arabic RTL using UTF-8
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyCharacterIndexTest, TypographyCharacterIndexMultilingualTest006, TestSize.Level0)
{
    SetupTypography(ARABIC_TEXT_UTF8);
    OH_Drawing_Range* actualCharacterRange = nullptr;
    OH_Drawing_Range* glyphRange = OH_Drawing_TypographyGetGlyphRangeForCharacterRangeWithBuffer(
        fTypography, 3, 9, &actualCharacterRange, TEXT_ENCODING_UTF8);
    ASSERT_NE(glyphRange, nullptr);
    ASSERT_NE(actualCharacterRange, nullptr);
    EXPECT_EQ(OH_Drawing_GetStartFromRange(glyphRange), 1);
    EXPECT_EQ(OH_Drawing_GetEndFromRange(glyphRange), 4);
    OH_Drawing_ReleaseRangeBuffer(glyphRange);
    OH_Drawing_ReleaseRangeBuffer(actualCharacterRange);
}

/*
 * @tc.name: TypographyCharacterIndexMultilingualTest007
 * @tc.desc: Test glyph to character conversion with Arabic RTL using UTF-16
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyCharacterIndexTest, TypographyCharacterIndexMultilingualTest007, TestSize.Level0)
{
    SetupTypographyWithEncodedText(ARABIC_TEXT_UTF16);
    OH_Drawing_Range* actualGlyphRange = nullptr;
    OH_Drawing_Range* characterRange = OH_Drawing_TypographyGetCharacterRangeForGlyphRangeWithBuffer(
        fTypography, 0, 5, &actualGlyphRange, TEXT_ENCODING_UTF16);
    ASSERT_NE(characterRange, nullptr);
    ASSERT_NE(actualGlyphRange, nullptr);
    EXPECT_EQ(OH_Drawing_GetStartFromRange(characterRange), 0);
    EXPECT_EQ(OH_Drawing_GetEndFromRange(characterRange), 6);
    OH_Drawing_ReleaseRangeBuffer(characterRange);
    OH_Drawing_ReleaseRangeBuffer(actualGlyphRange);
}

/*
 * @tc.name: TypographyCharacterIndexMultilingualTest008
 * @tc.desc: Test character to glyph conversion with Arabic RTL using UTF-16
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyCharacterIndexTest, TypographyCharacterIndexMultilingualTest008, TestSize.Level0)
{
    SetupTypographyWithEncodedText(ARABIC_TEXT_UTF16);
    OH_Drawing_Range* actualCharacterRange = nullptr;
    OH_Drawing_Range* glyphRange = OH_Drawing_TypographyGetGlyphRangeForCharacterRangeWithBuffer(
        fTypography, 3, 5, &actualCharacterRange, TEXT_ENCODING_UTF16);
    ASSERT_NE(glyphRange, nullptr);
    ASSERT_NE(actualCharacterRange, nullptr);
    EXPECT_EQ(OH_Drawing_GetStartFromRange(glyphRange), 2);
    EXPECT_EQ(OH_Drawing_GetEndFromRange(glyphRange), 4);
    EXPECT_EQ(OH_Drawing_GetStartFromRange(actualCharacterRange), 3);
    EXPECT_EQ(OH_Drawing_GetEndFromRange(actualCharacterRange), 5);
    OH_Drawing_ReleaseRangeBuffer(glyphRange);
    OH_Drawing_ReleaseRangeBuffer(actualCharacterRange);
}

/*
 * @tc.name: TypographyCharacterIndexNullActualRangeTest001
 * @tc.desc: Test glyph to character conversion with null actualRange
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyCharacterIndexTest, TypographyCharacterIndexNullActualRangeTest001, TestSize.Level0)
{
    SetupTypographyWithEncodedText(ARABIC_TEXT_UTF16);
    OH_Drawing_Range* characterRange = OH_Drawing_TypographyGetCharacterRangeForGlyphRangeWithBuffer(
        fTypography, 0, 5, nullptr, TEXT_ENCODING_UTF16);
    ASSERT_NE(characterRange, nullptr);
    EXPECT_EQ(OH_Drawing_GetStartFromRange(characterRange), 0);
    EXPECT_EQ(OH_Drawing_GetEndFromRange(characterRange), 6);
    OH_Drawing_ReleaseRangeBuffer(characterRange);
}

/*
 * @tc.name: TypographyCharacterIndexNullActualRangeTest002
 * @tc.desc: Test character to glyph conversion with null actualRange
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyCharacterIndexTest, TypographyCharacterIndexNullActualRangeTest002, TestSize.Level0)
{
    SetupTypographyWithEncodedText(ARABIC_TEXT_UTF16);
    OH_Drawing_Range* glyphRange = OH_Drawing_TypographyGetGlyphRangeForCharacterRangeWithBuffer(
        fTypography, 3, 5, nullptr, TEXT_ENCODING_UTF16);
    ASSERT_NE(glyphRange, nullptr);
    EXPECT_EQ(OH_Drawing_GetStartFromRange(glyphRange), 2);
    EXPECT_EQ(OH_Drawing_GetEndFromRange(glyphRange), 4);
    OH_Drawing_ReleaseRangeBuffer(glyphRange);
}

/*
 * @tc.name: TypographyGetGlyphRangeForCharacterRangeNullTypographyTest001
 * @tc.desc: Test error handling for glyph range conversion with null typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyCharacterIndexTest, TypographyGetGlyphRangeForCharacterRangeNullTypographyTest001,
    TestSize.Level0)
{
    SetupTypography(DEFAULT_TEXT_UTF8);
    OH_Drawing_Range* range = OH_Drawing_TypographyGetGlyphRangeForCharacterRangeWithBuffer(
        nullptr, 0, 5, nullptr, TEXT_ENCODING_UTF8);
    EXPECT_EQ(range, nullptr);
}

/*
 * @tc.name: TypographyGetGlyphRangeForCharacterRangeInvalidRange1Test001
 * @tc.desc: Test error handling for glyph range conversion with start > end
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyCharacterIndexTest, TypographyGetGlyphRangeForCharacterRangeInvalidRange1Test001, TestSize.Level0)
{
    SetupTypography(DEFAULT_TEXT_UTF8);
    OH_Drawing_Range* range = OH_Drawing_TypographyGetGlyphRangeForCharacterRangeWithBuffer(
        fTypography, 5, 0, nullptr, TEXT_ENCODING_UTF8);
    EXPECT_EQ(range, nullptr);
}

/*
 * @tc.name: TypographyGetGlyphRangeForCharacterRangeEmptyRangeTest001
 * @tc.desc: Test error handling for glyph range conversion with empty range
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyCharacterIndexTest, TypographyGetGlyphRangeForCharacterRangeEmptyRangeTest001, TestSize.Level0)
{
    SetupTypography(DEFAULT_TEXT_UTF8);
    OH_Drawing_Range* actualCharacterRange = nullptr;
    OH_Drawing_Range* range = OH_Drawing_TypographyGetGlyphRangeForCharacterRangeWithBuffer(
        fTypography, 0, 0, &actualCharacterRange, TEXT_ENCODING_UTF8);
    EXPECT_EQ(range, nullptr);
}

/*
 * @tc.name: TypographyGetGlyphRangeForCharacterRangeInvalidEncodingTest001
 * @tc.desc: Test error handling for glyph range conversion with invalid encoding
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyCharacterIndexTest, TypographyGetGlyphRangeForCharacterRangeInvalidEncodingTest001,
    TestSize.Level0)
{
    SetupTypography(DEFAULT_TEXT_UTF8);
    OH_Drawing_Range* actualCharacterRange = nullptr;
    OH_Drawing_Range* range = OH_Drawing_TypographyGetGlyphRangeForCharacterRangeWithBuffer(
        fTypography, 0, 5, &actualCharacterRange, (OH_Drawing_TextEncoding)999);
    EXPECT_EQ(range, nullptr);
}

/*
 * @tc.name: TypographyGetCharacterRangeForGlyphRangeNullTypographyTest001
 * @tc.desc: Test error handling for character range conversion with null typography
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyCharacterIndexTest, TypographyGetCharacterRangeForGlyphRangeNullTypographyTest001,
    TestSize.Level0)
{
    SetupTypography(DEFAULT_TEXT_UTF8);
    OH_Drawing_Range* range = OH_Drawing_TypographyGetCharacterRangeForGlyphRangeWithBuffer(
        nullptr, 0, 5, nullptr, TEXT_ENCODING_UTF8);
    EXPECT_EQ(range, nullptr);
}

/*
 * @tc.name: TypographyGetCharacterRangeForGlyphRangeInvalidRange1Test001
 * @tc.desc: Test error handling for character range conversion with start > end
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyCharacterIndexTest, TypographyGetCharacterRangeForGlyphRangeInvalidRange1Test001, TestSize.Level0)
{
    SetupTypography(DEFAULT_TEXT_UTF8);
    OH_Drawing_Range* range = OH_Drawing_TypographyGetCharacterRangeForGlyphRangeWithBuffer(
        fTypography, 5, 0, nullptr, TEXT_ENCODING_UTF8);
    EXPECT_EQ(range, nullptr);
}

/*
 * @tc.name: TypographyGetCharacterRangeForGlyphRangeEmptyRangeTest001
 * @tc.desc: Test error handling for character range conversion with empty range
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyCharacterIndexTest, TypographyGetCharacterRangeForGlyphRangeEmptyRangeTest001, TestSize.Level0)
{
    SetupTypography(DEFAULT_TEXT_UTF8);
    OH_Drawing_Range* actualGlyphRange = nullptr;
    OH_Drawing_Range* range = OH_Drawing_TypographyGetCharacterRangeForGlyphRangeWithBuffer(
        fTypography, 0, 0, &actualGlyphRange, TEXT_ENCODING_UTF8);
    EXPECT_EQ(range, nullptr);
}

/*
 * @tc.name: TypographyGetCharacterRangeForGlyphRangeInvalidEncodingTest001
 * @tc.desc: Test error handling for character range conversion with invalid encoding
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyCharacterIndexTest, TypographyGetCharacterRangeForGlyphRangeInvalidEncodingTest001,
    TestSize.Level0)
{
    SetupTypography(DEFAULT_TEXT_UTF8);
    OH_Drawing_Range* actualGlyphRange = nullptr;
    OH_Drawing_Range* range = OH_Drawing_TypographyGetCharacterRangeForGlyphRangeWithBuffer(
        fTypography, 0, 5, &actualGlyphRange, (OH_Drawing_TextEncoding)999);
    EXPECT_EQ(range, nullptr);
}
} // namespace OHOS
