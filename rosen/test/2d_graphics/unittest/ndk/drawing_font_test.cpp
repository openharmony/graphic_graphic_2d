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

#include "drawing_font.h"
#include "drawing_typeface.h"
#include "drawing_path.h"
#include "drawing_error_code.h"

#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
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

void NativeFontTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
}
void NativeFontTest::TearDownTestCase() {}
void NativeFontTest::SetUp() {}
void NativeFontTest::TearDown() {}

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
        TEXT_ENCODING_UTF8, 0, 0, path), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_FontGetTextPathWithFallback(font, nullptr, strlen(text),
        TEXT_ENCODING_UTF8, 0, 0, path), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_FontGetTextPathWithFallback(font, text, 0,
        TEXT_ENCODING_UTF8, 0, 0, path), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_FontGetTextPathWithFallback(font, text, strlen(text),
        TEXT_ENCODING_UTF8, 0, 0, nullptr), OH_DRAWING_ERROR_INVALID_PARAMETER);

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
        TEXT_ENCODING_UTF8, 0, 0, path), OH_DRAWING_ERROR_INVALID_PARAMETER);

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

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS