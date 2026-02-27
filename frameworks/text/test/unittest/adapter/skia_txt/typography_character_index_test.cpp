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

#include <locale>
#include <string>
#include <codecvt>
#include <tuple>
#include "gtest/gtest.h"

#include "font_collection.h"
#include "impl/paragraph_impl.h"
#include "modules/skparagraph/include/TextStyle.h"
#include "paragraph.h"
#include "src/ParagraphImpl.h"
#include "typography.h"
#include "typography_create.h"
#include "utils/string_util.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {

const char* DEFAULT_TEXT_UTF8 = "Hello こんにちは 你好 🌟 World";
const char16_t* DEFAULT_TEXT_UTF16 = u"Hello こんにちは 你好 🌟 World";
const double DEFAULT_FONT_SIZE = 50.0;
const double MAX_WIDTH = 400.0;
const double TEST_COORDINATE_X = 50.0;
const double TEST_COORDINATE_Y = 10.0;
const double NEGATIVE_COORDINATE = -10.0;

}

class TypographyCharacterIndexTest : public testing::Test {
public:
    void SetUp() override;
    void TearDown() override;
    std::unique_ptr<Typography> CreateTypography(const std::u16string& text);
    std::unique_ptr<Typography> CreateTypography(const char* utf8Text);

protected:
    std::shared_ptr<FontCollection> fontCollection_;
    TypographyStyle typographyStyle_;
};

void TypographyCharacterIndexTest::SetUp()
{
    fontCollection_ = FontCollection::From(std::make_shared<txt::FontCollection>());
    typographyStyle_.textDirection = TextDirection::LTR;
    typographyStyle_.textAlign = TextAlign::LEFT;
    typographyStyle_.fontSize = DEFAULT_FONT_SIZE;
}

void TypographyCharacterIndexTest::TearDown()
{
    fontCollection_.reset();
}

std::unique_ptr<Typography> TypographyCharacterIndexTest::CreateTypography(const std::u16string& text)
{
    auto typographyCreate = TypographyCreate::Create(typographyStyle_, fontCollection_);
    TextStyle textStyle;
    textStyle.fontSize = DEFAULT_FONT_SIZE;
    typographyCreate->PushStyle(textStyle);
    typographyCreate->AppendText(text);

    std::unique_ptr<Typography> typography = typographyCreate->CreateTypography();
    typography->Layout(MAX_WIDTH);
    return typography;
}

std::unique_ptr<Typography> TypographyCharacterIndexTest::CreateTypography(const char* utf8Text)
{
    return CreateTypography(Str8ToStr16ByIcu(utf8Text));
}

/*
 * @tc.name: TypographyCharacterIndexNormalCoordinateUtf8Test001
 * @tc.desc: Scene test: GetCharacterIndexByCoordinate with normal coordinate in UTF-8 encoding
 * @tc.type: FUNC
 */
HWTEST_F(TypographyCharacterIndexTest, TypographyCharacterIndexNormalCoordinateUtf8Test001, TestSize.Level0)
{
    auto typographyUtf8 = CreateTypography(DEFAULT_TEXT_UTF8);
    IndexAndAffinity resultUtf8 = typographyUtf8->GetCharacterIndexByCoordinate(
        TEST_COORDINATE_X, TEST_COORDINATE_Y, TextEncoding::UTF8);
    EXPECT_EQ(resultUtf8.affinity, Affinity::NEXT);
    EXPECT_EQ(resultUtf8.index, 1);
}

/*
 * @tc.name: TypographyCharacterIndexNormalCoordinateUtf16Test001
 * @tc.desc: Scene test: GetCharacterIndexByCoordinate with normal coordinate in UTF-16 encoding
 * @tc.type: FUNC
 */
HWTEST_F(TypographyCharacterIndexTest, TypographyCharacterIndexNormalCoordinateUtf16Test001, TestSize.Level0)
{
    auto typographyUtf16 = CreateTypography(DEFAULT_TEXT_UTF16);
    IndexAndAffinity resultUtf16 = typographyUtf16->GetCharacterIndexByCoordinate(
        TEST_COORDINATE_X, TEST_COORDINATE_Y, TextEncoding::UTF16);
    EXPECT_EQ(resultUtf16.affinity, Affinity::NEXT);
    EXPECT_EQ(resultUtf16.index, 1);
}

/*
 * @tc.name: TypographyCharacterIndexNegativeCoordinateTest001
 * @tc.desc: Scene test: GetCharacterIndexByCoordinate with negative coordinate values
 * @tc.type: FUNC
 */
HWTEST_F(TypographyCharacterIndexTest, TypographyCharacterIndexNegativeCoordinateTest001, TestSize.Level0)
{
    auto typography = CreateTypography(DEFAULT_TEXT_UTF16);
    IndexAndAffinity resultNegative = typography->GetCharacterIndexByCoordinate(
        NEGATIVE_COORDINATE, NEGATIVE_COORDINATE, TextEncoding::UTF8);
    EXPECT_EQ(resultNegative.index, 0);
    EXPECT_EQ(resultNegative.affinity, Affinity::NEXT);
}

/*
 * @tc.name: TypographyCharacterIndexEmptyTextTest001
 * @tc.desc: Scene test: GetCharacterIndexByCoordinate with empty text
 * @tc.type: FUNC
 */
HWTEST_F(TypographyCharacterIndexTest, TypographyCharacterIndexEmptyTextTest001, TestSize.Level0)
{
    auto typographyEmpty = CreateTypography("");
    IndexAndAffinity resultEmpty = typographyEmpty->GetCharacterIndexByCoordinate(
        TEST_COORDINATE_X, TEST_COORDINATE_Y, TextEncoding::UTF8);
    EXPECT_EQ(resultEmpty.index, 0);
    EXPECT_EQ(resultEmpty.affinity, Affinity::NEXT);
}

/*
 * @tc.name: TypographyGetCharacterRangeForGlyphRangeUtf8Test001
 * @tc.desc: Scene test: GetCharacterRangeForGlyphRange with mixed text in UTF-8 encoding
 * @tc.type: FUNC
 */
HWTEST_F(TypographyCharacterIndexTest, TypographyGetCharacterRangeForGlyphRangeUtf8Test001, TestSize.Level0)
{
    auto typographyUtf8 = CreateTypography(DEFAULT_TEXT_UTF8);
    Boundary actualGlyphRange = {0, 0};
    Boundary charRange = typographyUtf8->GetCharacterRangeForGlyphRange(
        0, 5, &actualGlyphRange, TextEncoding::UTF8);
    EXPECT_EQ(actualGlyphRange.leftIndex, 0);
    EXPECT_EQ(actualGlyphRange.rightIndex, 5);
    EXPECT_EQ(charRange.leftIndex, 0);
    EXPECT_EQ(charRange.rightIndex, 5);
}

/*
 * @tc.name: TypographyGetGlyphRangeForCharacterRangeUtf8Test001
 * @tc.desc: Scene test: GetGlyphRangeForCharacterRange with mixed text in UTF-8 encoding
 * @tc.type: FUNC
 */
HWTEST_F(TypographyCharacterIndexTest, TypographyGetGlyphRangeForCharacterRangeUtf8Test001, TestSize.Level0)
{
    auto typographyUtf8 = CreateTypography(DEFAULT_TEXT_UTF8);
    Boundary actualCharRange = {0, 0};
    Boundary glyphRange = typographyUtf8->GetGlyphRangeForCharacterRange(
        0, 5, &actualCharRange, TextEncoding::UTF8);
    EXPECT_EQ(actualCharRange.leftIndex, 0);
    EXPECT_EQ(actualCharRange.rightIndex, 5);
    EXPECT_EQ(glyphRange.leftIndex, 0);
    EXPECT_EQ(glyphRange.rightIndex, 5);
}

/*
 * @tc.name: TypographyGetCharacterRangeForGlyphRangeUtf16Test001
 * @tc.desc: Scene test: GetCharacterRangeForGlyphRange with mixed text in UTF-16 encoding
 * @tc.type: FUNC
 */
HWTEST_F(TypographyCharacterIndexTest, TypographyGetCharacterRangeForGlyphRangeUtf16Test001, TestSize.Level0)
{
    auto typographyUtf16 = CreateTypography(DEFAULT_TEXT_UTF16);
    Boundary actualGlyphRangeUtf16 = {0, 0};
    Boundary charRangeUtf16 = typographyUtf16->GetCharacterRangeForGlyphRange(
        0, 5, &actualGlyphRangeUtf16, TextEncoding::UTF16);
    EXPECT_EQ(actualGlyphRangeUtf16.leftIndex, 0);
    EXPECT_EQ(actualGlyphRangeUtf16.rightIndex, 5);
    EXPECT_EQ(charRangeUtf16.leftIndex, 0);
    EXPECT_EQ(charRangeUtf16.rightIndex, 5);
}

/*
 * @tc.name: TypographyGetGlyphRangeForCharacterRangeUtf16Test001
 * @tc.desc: Scene test: GetGlyphRangeForCharacterRange with mixed text in UTF-16 encoding
 * @tc.type: FUNC
 */
HWTEST_F(TypographyCharacterIndexTest, TypographyGetGlyphRangeForCharacterRangeUtf16Test001, TestSize.Level0)
{
    auto typographyUtf16 = CreateTypography(DEFAULT_TEXT_UTF16);
    Boundary actualCharRangeUtf16 = {0, 0};
    Boundary glyphRangeUtf16 = typographyUtf16->GetGlyphRangeForCharacterRange(
        0, 5, &actualCharRangeUtf16, TextEncoding::UTF16);
    EXPECT_EQ(actualCharRangeUtf16.leftIndex, 0);
    EXPECT_EQ(actualCharRangeUtf16.rightIndex, 5);
    EXPECT_EQ(glyphRangeUtf16.leftIndex, 0);
    EXPECT_EQ(glyphRangeUtf16.rightIndex, 5);
}

/*
 * @tc.name: TypographyGetCharacterRangeForGlyphRangeNullptrTest001
 * @tc.desc: Scene test: GetCharacterRangeForGlyphRange with nullptr parameter
 * @tc.type: FUNC
 */
HWTEST_F(TypographyCharacterIndexTest, TypographyGetCharacterRangeForGlyphRangeNullptrTest001, TestSize.Level0)
{
    auto typography = CreateTypography(DEFAULT_TEXT_UTF16);
    Boundary charRangeNull = typography->GetCharacterRangeForGlyphRange(0, 5, nullptr, TextEncoding::UTF8);
    EXPECT_EQ(charRangeNull.leftIndex, 0);
    EXPECT_EQ(charRangeNull.rightIndex, 5);
}

/*
 * @tc.name: TypographyGetGlyphRangeForCharacterRangeNullptrTest001
 * @tc.desc: Scene test: GetGlyphRangeForCharacterRange with nullptr parameter
 * @tc.type: FUNC
 */
HWTEST_F(TypographyCharacterIndexTest, TypographyGetGlyphRangeForCharacterRangeNullptrTest001, TestSize.Level0)
{
    auto typography = CreateTypography(DEFAULT_TEXT_UTF16);
    Boundary glyphRangeNull = typography->GetGlyphRangeForCharacterRange(0, 5, nullptr, TextEncoding::UTF8);
    EXPECT_EQ(glyphRangeNull.leftIndex, 0);
    EXPECT_EQ(glyphRangeNull.rightIndex, 5);
}

/*
 * @tc.name: TypographyLargeRangeGetCharacterRangeTest001
 * @tc.desc: Scene test: GetCharacterRangeForGlyphRange with very large range values
 * @tc.type: FUNC
 */
HWTEST_F(TypographyCharacterIndexTest, TypographyLargeRangeGetCharacterRangeTest001, TestSize.Level0)
{
    auto typography = CreateTypography(DEFAULT_TEXT_UTF16);
    Boundary actualGlyphRangeLarge = {0, 0};
    Boundary charRangeLarge = typography->GetCharacterRangeForGlyphRange(
        0, 10000, &actualGlyphRangeLarge, TextEncoding::UTF8);
    EXPECT_EQ(actualGlyphRangeLarge.leftIndex, 0);
    EXPECT_EQ(actualGlyphRangeLarge.rightIndex, 22);
    EXPECT_EQ(charRangeLarge.rightIndex, 39);
}

/*
 * @tc.name: TypographyLargeRangeGetGlyphRangeTest001
 * @tc.desc: Scene test: GetGlyphRangeForCharacterRange with very large range values
 * @tc.type: FUNC
 */
HWTEST_F(TypographyCharacterIndexTest, TypographyLargeRangeGetGlyphRangeTest001, TestSize.Level0)
{
    auto typography = CreateTypography(DEFAULT_TEXT_UTF16);
    Boundary actualCharRangeLarge = {0, 0};
    Boundary glyphRangeLarge = typography->GetGlyphRangeForCharacterRange(
        0, 10000, &actualCharRangeLarge, TextEncoding::UTF8);
    EXPECT_EQ(actualCharRangeLarge.leftIndex, 0);
    EXPECT_EQ(actualCharRangeLarge.rightIndex, 39);
    EXPECT_EQ(glyphRangeLarge.rightIndex, 22);
}

/*
 * @tc.name: TypographyMixedConsistencyTest001
 * @tc.desc: Scene test: Mixed text consistency between UTF-8 and UTF-16 encoding
 * @tc.type: FUNC
 */
HWTEST_F(TypographyCharacterIndexTest, TypographyMixedConsistencyTest001, TestSize.Level0)
{
    auto typographyMixedUtf8 = CreateTypography(DEFAULT_TEXT_UTF8);
    auto typographyMixedUtf16 = CreateTypography(DEFAULT_TEXT_UTF16);
    IndexAndAffinity resultMixedUtf8 = typographyMixedUtf8->GetCharacterIndexByCoordinate(
        TEST_COORDINATE_X, TEST_COORDINATE_Y, TextEncoding::UTF8);
    IndexAndAffinity resultMixedUtf16 = typographyMixedUtf16->GetCharacterIndexByCoordinate(
        TEST_COORDINATE_X, TEST_COORDINATE_Y, TextEncoding::UTF16);
    EXPECT_EQ(resultMixedUtf8.affinity, resultMixedUtf16.affinity);
    EXPECT_EQ(resultMixedUtf8.index, resultMixedUtf16.index);
}

} // namespace Rosen
} // namespace OHOS
