/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gtest/gtest.h"

#include "drawing_brush.h"
#include "drawing_font.h"
#include "drawing_typeface.h"
#include "drawing_path.h"
#include "drawing_pen.h"
#include "drawing_rect.h"
#include "drawing_error_code.h"

#ifdef RS_ENABLE_VK
#include "vulkan_context/rs_vulkan_context.h"
#endif

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class NativeFontTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void NativeFontTest::SetUpTestCase() {}
void NativeFontTest::TearDownTestCase() {}
void NativeFontTest::SetUp() {}
void NativeFontTest::TearDown() {}

namespace {
constexpr OH_Drawing_TextEncoding INVALID_ENCODING =
    static_cast<OH_Drawing_TextEncoding>(TEXT_ENCODING_GLYPH_ID + 1);
constexpr OH_Drawing_TextEncoding INVALID_ENCODING_MIN =
    static_cast<OH_Drawing_TextEncoding>(TEXT_ENCODING_UTF8 - 1);
}

/*
 * @tc.name: NativeFontTest_FontFeatures001
 * @tc.desc: test for manipulate fontfeatures.
 * @tc.type: FUNC
 * @tc.require: ICG6L3
 */
HWTEST_F(NativeFontTest, NativeFontTest_OH_Drawing_FontFeatures001, TestSize.Level1)
{
    const char* name = "ccmp";
    const float value = 0;
    OH_Drawing_FontFeatures* features = OH_Drawing_FontFeaturesCreate();
    EXPECT_NE(features, nullptr);
    OH_Drawing_ErrorCode ret = OH_Drawing_FontFeaturesAddFeature(features, name, value);
    EXPECT_EQ(ret, OH_DRAWING_SUCCESS);
    ret = OH_Drawing_FontFeaturesAddFeature(nullptr, name, value);
    EXPECT_EQ(ret, OH_DRAWING_ERROR_INVALID_PARAMETER);
    ret = OH_Drawing_FontFeaturesAddFeature(features, nullptr, value);
    EXPECT_EQ(ret, OH_DRAWING_ERROR_INVALID_PARAMETER);
    
    ret = OH_Drawing_FontFeaturesDestroy(features);
    EXPECT_EQ(ret, OH_DRAWING_SUCCESS);
    ret = OH_Drawing_FontFeaturesDestroy(nullptr);
    EXPECT_EQ(ret, OH_DRAWING_ERROR_INVALID_PARAMETER);
}

/*
 * @tc.name: NativeFontTest_MeasureSingleCharacterWithFeatures001
 * @tc.desc: test for measure single character with features.
 * @tc.type: FUNC
 * @tc.require: ICG6L3
 */
HWTEST_F(NativeFontTest, NativeFontTest_OH_Drawing_MeasureSingleCharacterWithFeatures001, TestSize.Level1)
{
    const char* strOne = "a";
    const char* strTwo = "你好";
    const char* strThree = "";
    float textWidth = 0.f;
    OH_Drawing_FontFeatures* featuresEmpty = OH_Drawing_FontFeaturesCreate();
    EXPECT_NE(featuresEmpty, nullptr);
    OH_Drawing_FontFeatures* featuresOne = OH_Drawing_FontFeaturesCreate();
    EXPECT_NE(featuresOne, nullptr);
    OH_Drawing_ErrorCode ret = OH_Drawing_FontFeaturesAddFeature(featuresOne, "ccmp", 0);
    EXPECT_EQ(ret, OH_DRAWING_SUCCESS);
    OH_Drawing_Font *font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    OH_Drawing_Font *fontWithTypeface = OH_Drawing_FontCreate();
    EXPECT_NE(fontWithTypeface, nullptr);
    OH_Drawing_Typeface* typeface = OH_Drawing_TypefaceCreateFromFile("/system/fonts/HarmonyOS_Sans_SC.ttf", 0);
    EXPECT_NE(typeface, nullptr);
    OH_Drawing_FontSetTypeface(fontWithTypeface, typeface);

    EXPECT_EQ(OH_Drawing_FontMeasureSingleCharacterWithFeatures(font, strOne, featuresEmpty, &textWidth),
        OH_DRAWING_SUCCESS);
    EXPECT_EQ(OH_Drawing_FontMeasureSingleCharacterWithFeatures(font, strTwo, featuresEmpty, &textWidth),
        OH_DRAWING_SUCCESS);
    EXPECT_EQ(OH_Drawing_FontMeasureSingleCharacterWithFeatures(font, strOne, featuresOne, &textWidth),
        OH_DRAWING_SUCCESS);
    EXPECT_EQ(OH_Drawing_FontMeasureSingleCharacterWithFeatures(fontWithTypeface, strOne, featuresOne, &textWidth),
        OH_DRAWING_SUCCESS);
    EXPECT_EQ(OH_Drawing_FontMeasureSingleCharacterWithFeatures(nullptr, strOne, featuresEmpty, &textWidth),
        OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_FontMeasureSingleCharacterWithFeatures(font, nullptr, featuresEmpty, &textWidth),
        OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_FontMeasureSingleCharacterWithFeatures(font, strOne, nullptr, &textWidth),
        OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_FontMeasureSingleCharacterWithFeatures(font, strOne, featuresEmpty, nullptr),
        OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_FontMeasureSingleCharacterWithFeatures(font, strThree, featuresEmpty, &textWidth),
        OH_DRAWING_ERROR_INVALID_PARAMETER);

    OH_Drawing_FontDestroy(font);
    OH_Drawing_FontDestroy(fontWithTypeface);
    OH_Drawing_FontFeaturesDestroy(featuresOne);
    OH_Drawing_FontFeaturesDestroy(featuresEmpty);
}

/*
 * @tc.name: NativeFontTest_GetTextPathWithFallback001
 * @tc.desc: test for GetTextPathWithFallback with invalid parameters.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NativeFontTest, NativeFontTest_OH_Drawing_GetTextPathWithFallback001, TestSize.Level1)
{
    const char* text = "Hello";
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    EXPECT_NE(path, nullptr);

    EXPECT_EQ(OH_Drawing_FontGetTextPathWithFallback(nullptr, text, strlen(text),
        TEXT_ENCODING_UTF8, 0, 0, path), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_FontGetTextPathWithFallback(font, nullptr, strlen(text),
        TEXT_ENCODING_UTF8, 0, 0, path), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_FontGetTextPathWithFallback(font, text, 0,
        TEXT_ENCODING_UTF8, 0, 0, path), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_FontGetTextPathWithFallback(font, text, strlen(text),
        TEXT_ENCODING_UTF8, 0, 0, nullptr), OH_DRAWING_ERROR_INCORRECT_PARAMETER);

    OH_Drawing_FontDestroy(font);
    OH_Drawing_PathDestroy(path);
}

/*
 * @tc.name: NativeFontTest_GetTextPathWithFallback002
 * @tc.desc: test for GetTextPathWithFallback with GLYPH_ID encoding.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NativeFontTest, NativeFontTest_OH_Drawing_GetTextPathWithFallback002, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    EXPECT_NE(path, nullptr);
    uint16_t glyphs[] = { 65, 66 };

    EXPECT_EQ(OH_Drawing_FontGetTextPathWithFallback(font, glyphs, sizeof(glyphs),
        TEXT_ENCODING_GLYPH_ID, 0, 0, path), OH_DRAWING_SUCCESS);

    OH_Drawing_FontDestroy(font);
    OH_Drawing_PathDestroy(path);
}

/*
 * @tc.name: NativeFontTest_GetTextPathWithFallback003
 * @tc.desc: test for GetTextPathWithFallback with UTF8 encoding.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NativeFontTest, NativeFontTest_OH_Drawing_GetTextPathWithFallback003, TestSize.Level1)
{
    const char* text = "Hello你好World";
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    EXPECT_NE(path, nullptr);

    EXPECT_EQ(OH_Drawing_FontGetTextPathWithFallback(font, text, strlen(text),
        TEXT_ENCODING_UTF8, 0, 0, path), OH_DRAWING_SUCCESS);

    OH_Drawing_FontDestroy(font);
    OH_Drawing_PathDestroy(path);
}

/*
 * @tc.name: NativeFontTest_GetTextPathWithFallback004
 * @tc.desc: test for GetTextPathWithFallback with UTF16 encoding.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NativeFontTest, NativeFontTest_OH_Drawing_GetTextPathWithFallback004, TestSize.Level1)
{
    const char16_t* text = u"Hello你好";
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    EXPECT_NE(path, nullptr);
    size_t byteLength = std::char_traits<char16_t>::length(text) * sizeof(char16_t);

    EXPECT_EQ(OH_Drawing_FontGetTextPathWithFallback(font, text, byteLength,
        TEXT_ENCODING_UTF16, 0, 0, path), OH_DRAWING_SUCCESS);

    OH_Drawing_FontDestroy(font);
    OH_Drawing_PathDestroy(path);
}

/*
 * @tc.name: NativeFontTest_GetTextPathWithFallback005
 * @tc.desc: test for GetTextPathWithFallback with UTF32 encoding.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NativeFontTest, NativeFontTest_OH_Drawing_GetTextPathWithFallback005, TestSize.Level1)
{
    const char32_t* text = U"Hello你好";
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    EXPECT_NE(path, nullptr);
    size_t byteLength = std::char_traits<char32_t>::length(text) * sizeof(char32_t);

    EXPECT_EQ(OH_Drawing_FontGetTextPathWithFallback(font, text, byteLength,
        TEXT_ENCODING_UTF32, 0, 0, path), OH_DRAWING_SUCCESS);

    OH_Drawing_FontDestroy(font);
    OH_Drawing_PathDestroy(path);
}

/*
 * @tc.name: NativeFontTest_GetTextPathWithFallback006
 * @tc.desc: test for GetTextPathWithFallback with typeface set.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NativeFontTest, NativeFontTest_OH_Drawing_GetTextPathWithFallback006, TestSize.Level1)
{
    const char* text = "你好世界";
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    OH_Drawing_Typeface* typeface = OH_Drawing_TypefaceCreateFromFile("/system/fonts/HarmonyOS_Sans.ttf", 0);
    if (typeface != nullptr) {
        OH_Drawing_FontSetTypeface(font, typeface);
    }
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    EXPECT_NE(path, nullptr);

    EXPECT_EQ(OH_Drawing_FontGetTextPathWithFallback(font, text, strlen(text),
        TEXT_ENCODING_UTF8, 0, 0, path), OH_DRAWING_SUCCESS);

    OH_Drawing_FontDestroy(font);
    OH_Drawing_PathDestroy(path);
    if (typeface != nullptr) {
        OH_Drawing_TypefaceDestroy(typeface);
    }
}

/*
 * @tc.name: NativeFontTest_GetTextPathWithFallback007
 * @tc.desc: test for GetTextPathWithFallback with empty text.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NativeFontTest, NativeFontTest_OH_Drawing_GetTextPathWithFallback007, TestSize.Level1)
{
    const char* text = "";
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    EXPECT_NE(path, nullptr);

    EXPECT_EQ(OH_Drawing_FontGetTextPathWithFallback(font, text, 0,
        TEXT_ENCODING_UTF8, 0, 0, path), OH_DRAWING_ERROR_INCORRECT_PARAMETER);

    OH_Drawing_FontDestroy(font);
    OH_Drawing_PathDestroy(path);
}

/*
 * @tc.name: NativeFontTest_GetTextPathWithFallback008
 * @tc.desc: test for GetTextPathWithFallback with emoji.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NativeFontTest, NativeFontTest_OH_Drawing_GetTextPathWithFallback008, TestSize.Level1)
{
    const char* text = "Hello😀World";
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    EXPECT_NE(path, nullptr);

    EXPECT_EQ(OH_Drawing_FontGetTextPathWithFallback(font, text, strlen(text),
        TEXT_ENCODING_UTF8, 0, 0, path), OH_DRAWING_SUCCESS);

    OH_Drawing_FontDestroy(font);
    OH_Drawing_PathDestroy(path);
}

/*
 * @tc.name: NativeFontTest_GetTextPathWithFallback009
 * @tc.desc: test for GetTextPathWithFallback with invalid UTF8 sequence.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NativeFontTest, NativeFontTest_OH_Drawing_GetTextPathWithFallback009, TestSize.Level1)
{
    unsigned char invalidUTF8[] = { 0xC0, 0x80, 0xC1, 0x81 };
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    EXPECT_NE(path, nullptr);

    EXPECT_EQ(OH_Drawing_FontGetTextPathWithFallback(font, invalidUTF8, sizeof(invalidUTF8),
        TEXT_ENCODING_UTF8, 0, 0, path), OH_DRAWING_SUCCESS);

    OH_Drawing_FontDestroy(font);
    OH_Drawing_PathDestroy(path);
}

/*
 * @tc.name: NativeFontTest_GetTextPathWithFallback010
 * @tc.desc: test for GetTextPathWithFallback with truncated UTF8.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NativeFontTest, NativeFontTest_OH_Drawing_GetTextPathWithFallback010, TestSize.Level1)
{
    unsigned char truncatedUTF8[] = { 0xE0, 0xA0 };
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    EXPECT_NE(path, nullptr);

    EXPECT_EQ(OH_Drawing_FontGetTextPathWithFallback(font, truncatedUTF8, sizeof(truncatedUTF8),
        TEXT_ENCODING_UTF8, 0, 0, path), OH_DRAWING_SUCCESS);

    OH_Drawing_FontDestroy(font);
    OH_Drawing_PathDestroy(path);
}

/*
 * @tc.name: NativeFontTest_GetTextPathWithFallback011
 * @tc.desc: test for GetTextPathWithFallback with surrogate pair.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NativeFontTest, NativeFontTest_OH_Drawing_GetTextPathWithFallback011, TestSize.Level1)
{
    const char16_t surrogatePair[] = { 0xD83D, 0xDE00 };
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    EXPECT_NE(path, nullptr);

    EXPECT_EQ(OH_Drawing_FontGetTextPathWithFallback(font, surrogatePair, sizeof(surrogatePair),
        TEXT_ENCODING_UTF16, 0, 0, path), OH_DRAWING_SUCCESS);

    OH_Drawing_FontDestroy(font);
    OH_Drawing_PathDestroy(path);
}

/*
 * @tc.name: NativeFontTest_GetTextPathWithFallback012
 * @tc.desc: test for GetTextPathWithFallback with orphaned surrogate.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NativeFontTest, NativeFontTest_OH_Drawing_GetTextPathWithFallback012, TestSize.Level1)
{
    const char16_t orphanedHigh[] = { 0xD83D };
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    EXPECT_NE(path, nullptr);

    EXPECT_EQ(OH_Drawing_FontGetTextPathWithFallback(font, orphanedHigh, sizeof(orphanedHigh),
        TEXT_ENCODING_UTF16, 0, 0, path), OH_DRAWING_SUCCESS);

    OH_Drawing_FontDestroy(font);
    OH_Drawing_PathDestroy(path);
}

/*
 * @tc.name: NativeFontTest_GetTextPathWithFallback013
 * @tc.desc: test for GetTextPathWithFallback with incomplete UTF16 length.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NativeFontTest, NativeFontTest_OH_Drawing_GetTextPathWithFallback013, TestSize.Level1)
{
    const char16_t* text = u"Hello";
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    EXPECT_NE(path, nullptr);

    EXPECT_EQ(OH_Drawing_FontGetTextPathWithFallback(font, text, 1,
        TEXT_ENCODING_UTF16, 0, 0, path), OH_DRAWING_SUCCESS);

    OH_Drawing_FontDestroy(font);
    OH_Drawing_PathDestroy(path);
}

/*
 * @tc.name: NativeFontTest_GetTextPathWithFallback014
 * @tc.desc: test for GetTextPathWithFallback with incomplete UTF32 length.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NativeFontTest, NativeFontTest_OH_Drawing_GetTextPathWithFallback014, TestSize.Level1)
{
    const char32_t* text = U"Hello";
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    EXPECT_NE(path, nullptr);

    EXPECT_EQ(OH_Drawing_FontGetTextPathWithFallback(font, text, 1,
        TEXT_ENCODING_UTF32, 0, 0, path), OH_DRAWING_SUCCESS);

    OH_Drawing_FontDestroy(font);
    OH_Drawing_PathDestroy(path);
}

/*
 * @tc.name: NativeFontTest_GetTextPathWithFallback015
 * @tc.desc: test for GetTextPathWithFallback with null in middle.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NativeFontTest, NativeFontTest_OH_Drawing_GetTextPathWithFallback015, TestSize.Level1)
{
    char textWithNull[] = "Hello\0World";
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    EXPECT_NE(path, nullptr);

    EXPECT_EQ(OH_Drawing_FontGetTextPathWithFallback(font, textWithNull, sizeof(textWithNull),
        TEXT_ENCODING_UTF8, 0, 0, path), OH_DRAWING_SUCCESS);

    OH_Drawing_FontDestroy(font);
    OH_Drawing_PathDestroy(path);
}

/*
 * @tc.name: NativeFontTest_GetTextPathWithFallback016
 * @tc.desc: test for GetTextPathWithFallback with non-printable chars.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NativeFontTest, NativeFontTest_OH_Drawing_GetTextPathWithFallback016, TestSize.Level1)
{
    unsigned char nonPrintable[] = { 0x00, 0x01, 0x02, 0x03 };
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    EXPECT_NE(path, nullptr);

    EXPECT_EQ(OH_Drawing_FontGetTextPathWithFallback(font, nonPrintable, sizeof(nonPrintable),
        TEXT_ENCODING_UTF8, 0, 0, path), OH_DRAWING_SUCCESS);

    OH_Drawing_FontDestroy(font);
    OH_Drawing_PathDestroy(path);
}

/*
 * @tc.name: NativeFontTest_GetTextPathWithFallback017
 * @tc.desc: test for GetTextPathWithFallback with large text.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NativeFontTest, NativeFontTest_OH_Drawing_GetTextPathWithFallback017, TestSize.Level1)
{
    std::string largeText = "Hello你好";
    for (int i = 0; i < 10; i++) {
        largeText += largeText;
    }
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    EXPECT_NE(path, nullptr);

    EXPECT_EQ(OH_Drawing_FontGetTextPathWithFallback(font, largeText.c_str(), largeText.length(),
        TEXT_ENCODING_UTF8, 0, 0, path), OH_DRAWING_SUCCESS);

    OH_Drawing_FontDestroy(font);
    OH_Drawing_PathDestroy(path);
}

/*
 * @tc.name: NativeFontTest_GetTextPathWithFallback018
 * @tc.desc: test for GetTextPathWithFallback with different coordinates.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NativeFontTest, NativeFontTest_OH_Drawing_GetTextPathWithFallback018, TestSize.Level1)
{
    const char* text = "Hello你好";
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    EXPECT_NE(path, nullptr);

    EXPECT_EQ(OH_Drawing_FontGetTextPathWithFallback(font, text, strlen(text),
        TEXT_ENCODING_UTF8, 100.0f, 200.0f, path), OH_DRAWING_SUCCESS);

    OH_Drawing_FontDestroy(font);
    OH_Drawing_PathDestroy(path);
}

/*
 * @tc.name: NativeFontTest_GetTextPathWithFallback019
 * @tc.desc: test for GetTextPathWithFallback with mixed encoding text.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NativeFontTest, NativeFontTest_OH_Drawing_GetTextPathWithFallback019, TestSize.Level1)
{
    const char* text = "abc你好def世界ghi😀";
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    EXPECT_NE(path, nullptr);

    EXPECT_EQ(OH_Drawing_FontGetTextPathWithFallback(font, text, strlen(text),
        TEXT_ENCODING_UTF8, 0, 0, path), OH_DRAWING_SUCCESS);

    OH_Drawing_FontDestroy(font);
    OH_Drawing_PathDestroy(path);
}

/*
 * @tc.name: NativeFontTest_GetTextPathWithFallback020
 * @tc.desc: test for GetTextPathWithFallback with themeFontFollowed.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NativeFontTest, NativeFontTest_OH_Drawing_GetTextPathWithFallback020, TestSize.Level1)
{
    const char* text = "你好世界";
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    OH_Drawing_Typeface* typeface = OH_Drawing_TypefaceCreateFromFile("/system/fonts/HarmonyOS_Sans_SC.ttf", 0);
    if (typeface != nullptr) {
        OH_Drawing_FontSetTypeface(font, typeface);
        OH_Drawing_FontSetThemeFontFollowed(font, true);
    }
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    EXPECT_NE(path, nullptr);

    EXPECT_EQ(OH_Drawing_FontGetTextPathWithFallback(font, text, strlen(text),
        TEXT_ENCODING_UTF8, 0, 0, path), OH_DRAWING_SUCCESS);

    OH_Drawing_FontDestroy(font);
    OH_Drawing_PathDestroy(path);
    if (typeface != nullptr) {
        OH_Drawing_TypefaceDestroy(typeface);
    }
}

/*
 * @tc.name: NativeFontTest_GetTextPathWithFallback021
 * @tc.desc: test for GetTextPathWithFallback with themeFontFollowed false.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NativeFontTest, NativeFontTest_OH_Drawing_GetTextPathWithFallback021, TestSize.Level1)
{
    const char* text = "你好世界";
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    OH_Drawing_FontSetThemeFontFollowed(font, false);
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    EXPECT_NE(path, nullptr);

    EXPECT_EQ(OH_Drawing_FontGetTextPathWithFallback(font, text, strlen(text),
        TEXT_ENCODING_UTF8, 0, 0, path), OH_DRAWING_SUCCESS);

    OH_Drawing_FontDestroy(font);
    OH_Drawing_PathDestroy(path);
}

/*
 * @tc.name: OH_Drawing_FontTextToGlyphsWithFallback_001
 * @tc.desc: test for OH_Drawing_FontTextToGlyphsWithFallback with normal utf8 text.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NativeFontTest, OH_Drawing_FontTextToGlyphsWithFallback_001, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    ASSERT_NE(font, nullptr);
    const char* text = "Hello你好"; // 7 codepoints
    OH_Drawing_TypefaceFallbackInfo* typefaceFallbackInfo = nullptr;
    uint32_t infosCount = 0;
    EXPECT_EQ(OH_Drawing_FontTextToGlyphsWithFallback(font, text, strlen(text), TEXT_ENCODING_UTF8,
        &typefaceFallbackInfo, &infosCount), OH_DRAWING_SUCCESS);
    EXPECT_GT(infosCount, 0);
    uint32_t glyphTotal = 0;
    for (uint32_t i = 0; i < infosCount; i++) {
        if (typefaceFallbackInfo[i].glyphCount > 0) {
            EXPECT_NE(typefaceFallbackInfo[i].glyphIds, nullptr);
        }
        glyphTotal += typefaceFallbackInfo[i].glyphCount;
    }
    EXPECT_EQ(glyphTotal, 7u);
    EXPECT_EQ(OH_Drawing_FontTypefaceFallbackInfoDestroy(typefaceFallbackInfo, infosCount),
        OH_DRAWING_SUCCESS);
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: OH_Drawing_FontTextToGlyphsWithFallback_002
 * @tc.desc: test for OH_Drawing_FontTextToGlyphsWithFallback with invalid parameters.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NativeFontTest, OH_Drawing_FontTextToGlyphsWithFallback_002, TestSize.Level2)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    ASSERT_NE(font, nullptr);
    const char* text = "Hello";
    OH_Drawing_TypefaceFallbackInfo* typefaceFallbackInfo = nullptr;
    uint32_t infosCount = 0;
    EXPECT_EQ(OH_Drawing_FontTextToGlyphsWithFallback(nullptr, text, strlen(text), TEXT_ENCODING_UTF8,
        &typefaceFallbackInfo, &infosCount), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_FontTextToGlyphsWithFallback(font, nullptr, strlen(text), TEXT_ENCODING_UTF8,
        &typefaceFallbackInfo, &infosCount), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_FontTextToGlyphsWithFallback(font, text, 0, TEXT_ENCODING_UTF8,
        &typefaceFallbackInfo, &infosCount), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_FontTextToGlyphsWithFallback(font, text, strlen(text), TEXT_ENCODING_UTF8,
        nullptr, &infosCount), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_FontTextToGlyphsWithFallback(font, text, strlen(text), TEXT_ENCODING_UTF8,
        &typefaceFallbackInfo, nullptr), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_FontTextToGlyphsWithFallback(font, text, strlen(text), INVALID_ENCODING,
        &typefaceFallbackInfo, &infosCount), OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE);
    EXPECT_EQ(OH_Drawing_FontTextToGlyphsWithFallback(font, text, strlen(text), INVALID_ENCODING_MIN,
        &typefaceFallbackInfo, &infosCount), OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE);
    EXPECT_EQ(OH_Drawing_FontTypefaceFallbackInfoDestroy(nullptr, 1), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_FontTypefaceFallbackInfoDestroy(typefaceFallbackInfo, 0),
        OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    OH_Drawing_TypefaceFallbackInfo infos[1];
    EXPECT_EQ(OH_Drawing_FontTypefaceFallbackInfoDestroy(infos, 0),
        OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: OH_Drawing_FontTextToGlyphsWithFallback_003
 * @tc.desc: test for OH_Drawing_FontTextToGlyphsWithFallback with utf16 and utf32 encoding.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NativeFontTest, OH_Drawing_FontTextToGlyphsWithFallback_003, TestSize.Level3)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    ASSERT_NE(font, nullptr);
    const char16_t text16[] = u"你好";
    size_t byteLength16 = sizeof(text16) - sizeof(char16_t);
    OH_Drawing_TypefaceFallbackInfo* typefaceFallbackInfo16 = nullptr;
    uint32_t infosCount16 = 0;
    EXPECT_EQ(OH_Drawing_FontTextToGlyphsWithFallback(font, text16, byteLength16, TEXT_ENCODING_UTF16,
        &typefaceFallbackInfo16, &infosCount16), OH_DRAWING_SUCCESS);
    uint32_t glyphTotal16 = 0;
    for (uint32_t i = 0; i < infosCount16; i++) {
        glyphTotal16 += typefaceFallbackInfo16[i].glyphCount;
    }
    EXPECT_EQ(glyphTotal16, 2u); // u"你好" has 2 codepoints
    EXPECT_EQ(OH_Drawing_FontTypefaceFallbackInfoDestroy(typefaceFallbackInfo16, infosCount16),
        OH_DRAWING_SUCCESS);

    const char32_t text32[] = U"你好";
    size_t byteLength32 = sizeof(text32) - sizeof(char32_t);
    OH_Drawing_TypefaceFallbackInfo* typefaceFallbackInfo32 = nullptr;
    uint32_t infosCount32 = 0;
    EXPECT_EQ(OH_Drawing_FontTextToGlyphsWithFallback(font, text32, byteLength32, TEXT_ENCODING_UTF32,
        &typefaceFallbackInfo32, &infosCount32), OH_DRAWING_SUCCESS);
    uint32_t glyphTotal32 = 0;
    for (uint32_t i = 0; i < infosCount32; i++) {
        glyphTotal32 += typefaceFallbackInfo32[i].glyphCount;
    }
    EXPECT_EQ(glyphTotal32, 2u); // U"你好" has 2 codepoints
    EXPECT_EQ(OH_Drawing_FontTypefaceFallbackInfoDestroy(typefaceFallbackInfo32, infosCount32),
        OH_DRAWING_SUCCESS);
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: OH_Drawing_FontTextToGlyphsWithFallback_004
 * @tc.desc: test for OH_Drawing_FontTextToGlyphsWithFallback with glyph id encoding.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NativeFontTest, OH_Drawing_FontTextToGlyphsWithFallback_004, TestSize.Level2)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    ASSERT_NE(font, nullptr);
    constexpr int testGlyphCount = 4; // "test" has 4 codepoints
    const char* text = "test";
    uint16_t glyphs[testGlyphCount] = { 0 };
    ASSERT_EQ(OH_Drawing_FontCountText(font, text, strlen(text), TEXT_ENCODING_UTF8), testGlyphCount);
    EXPECT_EQ(OH_Drawing_FontTextToGlyphs(font, text, strlen(text), TEXT_ENCODING_UTF8, glyphs,
        testGlyphCount), static_cast<uint32_t>(testGlyphCount));
    // GLYPH_ID has no characters to fall back on, so the glyph ids are reported as a single run
    OH_Drawing_TypefaceFallbackInfo* typefaceFallbackInfo = nullptr;
    uint32_t infosCount = 0;
    EXPECT_EQ(OH_Drawing_FontTextToGlyphsWithFallback(font, glyphs, sizeof(glyphs), TEXT_ENCODING_GLYPH_ID,
        &typefaceFallbackInfo, &infosCount), OH_DRAWING_SUCCESS);
    ASSERT_EQ(infosCount, 1u);
    ASSERT_EQ(typefaceFallbackInfo[0].glyphCount, static_cast<uint32_t>(testGlyphCount));
    for (uint32_t i = 0; i < typefaceFallbackInfo[0].glyphCount; i++) {
        EXPECT_EQ(typefaceFallbackInfo[0].glyphIds[i], glyphs[i]);
    }
    EXPECT_EQ(OH_Drawing_FontTypefaceFallbackInfoDestroy(typefaceFallbackInfo, infosCount),
        OH_DRAWING_SUCCESS);
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: OH_Drawing_FontMeasureTextWithFallback_001
 * @tc.desc: test for OH_Drawing_FontMeasureTextWithFallback with normal utf8 text.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NativeFontTest, OH_Drawing_FontMeasureTextWithFallback_001, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    ASSERT_NE(font, nullptr);
    const char* text = "Hello你好";
    float textWidth = 0.f;
    EXPECT_EQ(OH_Drawing_FontMeasureTextWithFallback(font, text, strlen(text), TEXT_ENCODING_UTF8,
        nullptr, &textWidth), OH_DRAWING_SUCCESS);
    EXPECT_GT(textWidth, 0.f);
    OH_Drawing_Rect* bounds = OH_Drawing_RectCreate(0.f, 0.f, 0.f, 0.f);
    ASSERT_NE(bounds, nullptr);
    textWidth = 0.f;
    EXPECT_EQ(OH_Drawing_FontMeasureTextWithFallback(font, text, strlen(text), TEXT_ENCODING_UTF8,
        bounds, &textWidth), OH_DRAWING_SUCCESS);
    EXPECT_GT(textWidth, 0.f);
    OH_Drawing_RectDestroy(bounds);
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: OH_Drawing_FontMeasureTextWithFallback_002
 * @tc.desc: test for OH_Drawing_FontMeasureTextWithFallback with invalid parameters.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NativeFontTest, OH_Drawing_FontMeasureTextWithFallback_002, TestSize.Level2)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    ASSERT_NE(font, nullptr);
    const char* text = "Hello";
    float textWidth = 0.f;
    EXPECT_EQ(OH_Drawing_FontMeasureTextWithFallback(nullptr, text, strlen(text), TEXT_ENCODING_UTF8,
        nullptr, &textWidth), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_FontMeasureTextWithFallback(font, nullptr, strlen(text), TEXT_ENCODING_UTF8,
        nullptr, &textWidth), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_FontMeasureTextWithFallback(font, text, 0, TEXT_ENCODING_UTF8,
        nullptr, &textWidth), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_FontMeasureTextWithFallback(font, text, strlen(text), TEXT_ENCODING_UTF8,
        nullptr, nullptr), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_FontMeasureTextWithFallback(font, text, strlen(text), INVALID_ENCODING,
        nullptr, &textWidth), OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE);
    EXPECT_EQ(OH_Drawing_FontMeasureTextWithFallback(font, text, strlen(text), INVALID_ENCODING_MIN,
        nullptr, &textWidth), OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE);
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: OH_Drawing_FontMeasureTextWithFallback_003
 * @tc.desc: test for OH_Drawing_FontMeasureTextWithFallback with utf16, utf32 and glyph id encoding.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NativeFontTest, OH_Drawing_FontMeasureTextWithFallback_003, TestSize.Level3)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    ASSERT_NE(font, nullptr);
    const char16_t text16[] = u"你好";
    size_t byteLength16 = sizeof(text16) - sizeof(char16_t);
    float textWidth = 0.f;
    EXPECT_EQ(OH_Drawing_FontMeasureTextWithFallback(font, text16, byteLength16, TEXT_ENCODING_UTF16,
        nullptr, &textWidth), OH_DRAWING_SUCCESS);
    EXPECT_GT(textWidth, 0.f);

    const char32_t text32[] = U"你好";
    size_t byteLength32 = sizeof(text32) - sizeof(char32_t);
    textWidth = 0.f;
    EXPECT_EQ(OH_Drawing_FontMeasureTextWithFallback(font, text32, byteLength32, TEXT_ENCODING_UTF32,
        nullptr, &textWidth), OH_DRAWING_SUCCESS);
    EXPECT_GT(textWidth, 0.f);

    // GLYPH_ID has no characters to fall back on and is measured by the plain MeasureText path
    constexpr int testGlyphCount = 4; // "test" has 4 codepoints
    const char* text = "test";
    uint16_t glyphs[testGlyphCount] = { 0 };
    ASSERT_EQ(OH_Drawing_FontCountText(font, text, strlen(text), TEXT_ENCODING_UTF8), testGlyphCount);
    EXPECT_EQ(OH_Drawing_FontTextToGlyphs(font, text, strlen(text), TEXT_ENCODING_UTF8, glyphs,
        testGlyphCount), static_cast<uint32_t>(testGlyphCount));
    float expectedWidth = 0.f;
    EXPECT_EQ(OH_Drawing_FontMeasureText(font, glyphs, sizeof(glyphs), TEXT_ENCODING_GLYPH_ID,
        nullptr, &expectedWidth), OH_DRAWING_SUCCESS);
    textWidth = 0.f;
    EXPECT_EQ(OH_Drawing_FontMeasureTextWithFallback(font, glyphs, sizeof(glyphs), TEXT_ENCODING_GLYPH_ID,
        nullptr, &textWidth), OH_DRAWING_SUCCESS);
    EXPECT_FLOAT_EQ(textWidth, expectedWidth);
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: OH_Drawing_FontMeasureTextWithBrushOrPenWithFallback_001
 * @tc.desc: test for OH_Drawing_FontMeasureTextWithBrushOrPenWithFallback with brush or pen, with and without bounds.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NativeFontTest, OH_Drawing_FontMeasureTextWithBrushOrPenWithFallback_001, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    ASSERT_NE(font, nullptr);
    const char* text = "Hello你好";
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    ASSERT_NE(brush, nullptr);
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    ASSERT_NE(pen, nullptr);
    OH_Drawing_Rect* bounds = OH_Drawing_RectCreate(0.f, 0.f, 0.f, 0.f);
    ASSERT_NE(bounds, nullptr);
    float textWidth = 0.f;
    EXPECT_EQ(OH_Drawing_FontMeasureTextWithBrushOrPenWithFallback(font, text, strlen(text),
        TEXT_ENCODING_UTF8, brush, nullptr, nullptr, &textWidth), OH_DRAWING_SUCCESS);
    EXPECT_GT(textWidth, 0.f);
    textWidth = 0.f;
    EXPECT_EQ(OH_Drawing_FontMeasureTextWithBrushOrPenWithFallback(font, text, strlen(text),
        TEXT_ENCODING_UTF8, nullptr, pen, nullptr, &textWidth), OH_DRAWING_SUCCESS);
    EXPECT_GT(textWidth, 0.f);
    textWidth = 0.f;
    EXPECT_EQ(OH_Drawing_FontMeasureTextWithBrushOrPenWithFallback(font, text, strlen(text),
        TEXT_ENCODING_UTF8, brush, nullptr, bounds, &textWidth), OH_DRAWING_SUCCESS);
    EXPECT_GT(textWidth, 0.f);
    textWidth = 0.f;
    EXPECT_EQ(OH_Drawing_FontMeasureTextWithBrushOrPenWithFallback(font, text, strlen(text),
        TEXT_ENCODING_UTF8, nullptr, pen, bounds, &textWidth), OH_DRAWING_SUCCESS);
    EXPECT_GT(textWidth, 0.f);
    OH_Drawing_RectDestroy(bounds);
    OH_Drawing_PenDestroy(pen);
    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: OH_Drawing_FontMeasureTextWithBrushOrPenWithFallback_002
 * @tc.desc: test for OH_Drawing_FontMeasureTextWithBrushOrPenWithFallback with invalid parameters.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NativeFontTest, OH_Drawing_FontMeasureTextWithBrushOrPenWithFallback_002, TestSize.Level2)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    ASSERT_NE(font, nullptr);
    const char* text = "Hello";
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    ASSERT_NE(brush, nullptr);
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    ASSERT_NE(pen, nullptr);
    float textWidth = 0.f;
    EXPECT_EQ(OH_Drawing_FontMeasureTextWithBrushOrPenWithFallback(font, text, strlen(text),
        TEXT_ENCODING_UTF8, brush, pen, nullptr, &textWidth), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_FontMeasureTextWithBrushOrPenWithFallback(nullptr, text, strlen(text),
        TEXT_ENCODING_UTF8, nullptr, nullptr, nullptr, &textWidth), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_FontMeasureTextWithBrushOrPenWithFallback(font, nullptr, strlen(text),
        TEXT_ENCODING_UTF8, nullptr, nullptr, nullptr, &textWidth), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_FontMeasureTextWithBrushOrPenWithFallback(font, text, 0,
        TEXT_ENCODING_UTF8, nullptr, nullptr, nullptr, &textWidth), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_FontMeasureTextWithBrushOrPenWithFallback(font, text, strlen(text),
        TEXT_ENCODING_UTF8, nullptr, nullptr, nullptr, nullptr), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_FontMeasureTextWithBrushOrPenWithFallback(font, text, strlen(text),
        INVALID_ENCODING, nullptr, nullptr, nullptr, &textWidth), OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE);
    EXPECT_EQ(OH_Drawing_FontMeasureTextWithBrushOrPenWithFallback(font, text, strlen(text),
        INVALID_ENCODING_MIN, nullptr, nullptr, nullptr, &textWidth), OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE);
    OH_Drawing_PenDestroy(pen);
    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: OH_Drawing_FontMeasureTextWithBrushOrPenWithFallback_003
 * @tc.desc: test for OH_Drawing_FontMeasureTextWithBrushOrPenWithFallback with utf16, utf32 and glyph id encoding.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NativeFontTest, OH_Drawing_FontMeasureTextWithBrushOrPenWithFallback_003, TestSize.Level3)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    ASSERT_NE(font, nullptr);
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    ASSERT_NE(pen, nullptr);
    const char16_t text16[] = u"你好";
    size_t byteLength16 = sizeof(text16) - sizeof(char16_t);
    float textWidth = 0.f;
    EXPECT_EQ(OH_Drawing_FontMeasureTextWithBrushOrPenWithFallback(font, text16, byteLength16,
        TEXT_ENCODING_UTF16, nullptr, pen, nullptr, &textWidth), OH_DRAWING_SUCCESS);
    EXPECT_GT(textWidth, 0.f);

    const char32_t text32[] = U"你好";
    size_t byteLength32 = sizeof(text32) - sizeof(char32_t);
    textWidth = 0.f;
    EXPECT_EQ(OH_Drawing_FontMeasureTextWithBrushOrPenWithFallback(font, text32, byteLength32,
        TEXT_ENCODING_UTF32, nullptr, nullptr, nullptr, &textWidth), OH_DRAWING_SUCCESS);
    EXPECT_GT(textWidth, 0.f);

    // GLYPH_ID has no characters to fall back on and is measured by the plain MeasureText path
    constexpr int testGlyphCount = 4; // "test" has 4 codepoints
    const char* text = "test";
    uint16_t glyphs[testGlyphCount] = { 0 };
    ASSERT_EQ(OH_Drawing_FontCountText(font, text, strlen(text), TEXT_ENCODING_UTF8), testGlyphCount);
    EXPECT_EQ(OH_Drawing_FontTextToGlyphs(font, text, strlen(text), TEXT_ENCODING_UTF8, glyphs,
        testGlyphCount), static_cast<uint32_t>(testGlyphCount));
    float expectedWidth = 0.f;
    EXPECT_EQ(OH_Drawing_FontMeasureTextWithBrushOrPen(font, glyphs, sizeof(glyphs), TEXT_ENCODING_GLYPH_ID,
        nullptr, pen, nullptr, &expectedWidth), OH_DRAWING_SUCCESS);
    textWidth = 0.f;
    EXPECT_EQ(OH_Drawing_FontMeasureTextWithBrushOrPenWithFallback(font, glyphs, sizeof(glyphs),
        TEXT_ENCODING_GLYPH_ID, nullptr, pen, nullptr, &textWidth), OH_DRAWING_SUCCESS);
    EXPECT_FLOAT_EQ(textWidth, expectedWidth);
    OH_Drawing_PenDestroy(pen);
    OH_Drawing_FontDestroy(font);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS