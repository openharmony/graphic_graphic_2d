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

#include "gtest/gtest.h"

#include <cstring>

#include "drawing_error_code.h"
#include "drawing_font.h"
#include "drawing_text_blob.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class NativeTextBlobTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void NativeTextBlobTest::SetUpTestCase() {}
void NativeTextBlobTest::TearDownTestCase() {}
void NativeTextBlobTest::SetUp() {}
void NativeTextBlobTest::TearDown() {}

namespace {
constexpr OH_Drawing_TextEncoding INVALID_ENCODING =
    static_cast<OH_Drawing_TextEncoding>(TEXT_ENCODING_GLYPH_ID + 1);
constexpr OH_Drawing_TextEncoding INVALID_ENCODING_MIN =
    static_cast<OH_Drawing_TextEncoding>(TEXT_ENCODING_UTF8 - 1);
}

/**
 * @tc.name: OH_Drawing_TextBlobCreateFromTextWithFallback_001
 * @tc.desc: test for OH_Drawing_TextBlobCreateFromTextWithFallback with normal utf8 text.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NativeTextBlobTest, OH_Drawing_TextBlobCreateFromTextWithFallback_001, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    ASSERT_NE(font, nullptr);
    const char* text = "Hello你好";
    OH_Drawing_TextBlob** textBlobs = nullptr;
    uint32_t textBlobsCount = 0;
    OH_Drawing_ErrorCode ret = OH_Drawing_TextBlobCreateFromTextWithFallback(text, strlen(text), font,
        TEXT_ENCODING_UTF8, &textBlobs, &textBlobsCount);
    EXPECT_EQ(ret, OH_DRAWING_SUCCESS);
    EXPECT_GT(textBlobsCount, 0);
    for (uint32_t i = 0; i < textBlobsCount; i++) {
        EXPECT_NE(textBlobs[i], nullptr);
    }
    EXPECT_EQ(OH_Drawing_TextBlobsArrayDestroy(textBlobs, textBlobsCount), OH_DRAWING_SUCCESS);
    OH_Drawing_FontDestroy(font);
}

/**
 * @tc.name: OH_Drawing_TextBlobCreateFromTextWithFallback_002
 * @tc.desc: test for OH_Drawing_TextBlobCreateFromTextWithFallback with invalid parameters.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NativeTextBlobTest, OH_Drawing_TextBlobCreateFromTextWithFallback_002, TestSize.Level2)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    ASSERT_NE(font, nullptr);
    const char* text = "Hello";
    OH_Drawing_TextBlob** textBlobs = nullptr;
    uint32_t textBlobsCount = 0;
    EXPECT_EQ(OH_Drawing_TextBlobCreateFromTextWithFallback(nullptr, strlen(text), font, TEXT_ENCODING_UTF8,
        &textBlobs, &textBlobsCount), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_TextBlobCreateFromTextWithFallback(text, 0, font, TEXT_ENCODING_UTF8,
        &textBlobs, &textBlobsCount), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_TextBlobCreateFromTextWithFallback(text, strlen(text), nullptr, TEXT_ENCODING_UTF8,
        &textBlobs, &textBlobsCount), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_TextBlobCreateFromTextWithFallback(text, strlen(text), font, TEXT_ENCODING_UTF8,
        nullptr, &textBlobsCount), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_TextBlobCreateFromTextWithFallback(text, strlen(text), font, TEXT_ENCODING_UTF8,
        &textBlobs, nullptr), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_TextBlobCreateFromTextWithFallback(text, strlen(text), font, INVALID_ENCODING,
        &textBlobs, &textBlobsCount), OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE);
    EXPECT_EQ(OH_Drawing_TextBlobCreateFromTextWithFallback(text, strlen(text), font, INVALID_ENCODING_MIN,
        &textBlobs, &textBlobsCount), OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE);
    OH_Drawing_FontDestroy(font);
}

/**
 * @tc.name: OH_Drawing_TextBlobCreateFromTextWithFallback_003
 * @tc.desc: test for OH_Drawing_TextBlobCreateFromTextWithFallback with utf16, utf32 and glyph id encoding.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NativeTextBlobTest, OH_Drawing_TextBlobCreateFromTextWithFallback_003, TestSize.Level3)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    ASSERT_NE(font, nullptr);
    const char16_t text16[] = u"你好";
    size_t byteLength16 = sizeof(text16) - sizeof(char16_t);
    OH_Drawing_TextBlob** textBlobs = nullptr;
    uint32_t textBlobsCount = 0;
    OH_Drawing_ErrorCode ret = OH_Drawing_TextBlobCreateFromTextWithFallback(text16, byteLength16, font,
        TEXT_ENCODING_UTF16, &textBlobs, &textBlobsCount);
    EXPECT_EQ(ret, OH_DRAWING_SUCCESS);
    EXPECT_GT(textBlobsCount, 0);
    EXPECT_EQ(OH_Drawing_TextBlobsArrayDestroy(textBlobs, textBlobsCount), OH_DRAWING_SUCCESS);

    const char32_t text32[] = U"你好";
    size_t byteLength32 = sizeof(text32) - sizeof(char32_t);
    textBlobs = nullptr;
    textBlobsCount = 0;
    ret = OH_Drawing_TextBlobCreateFromTextWithFallback(text32, byteLength32, font,
        TEXT_ENCODING_UTF32, &textBlobs, &textBlobsCount);
    EXPECT_EQ(ret, OH_DRAWING_SUCCESS);
    EXPECT_GT(textBlobsCount, 0);
    EXPECT_EQ(OH_Drawing_TextBlobsArrayDestroy(textBlobs, textBlobsCount), OH_DRAWING_SUCCESS);

    // GLYPH_ID has no characters to fall back on and returns a single blob
    constexpr int testGlyphCount = 4; // "test" has 4 codepoints
    const char* text = "test";
    uint16_t glyphs[testGlyphCount] = { 0 };
    ASSERT_EQ(OH_Drawing_FontCountText(font, text, strlen(text), TEXT_ENCODING_UTF8), testGlyphCount);
    EXPECT_EQ(OH_Drawing_FontTextToGlyphs(font, text, strlen(text), TEXT_ENCODING_UTF8, glyphs,
        testGlyphCount), static_cast<uint32_t>(testGlyphCount));
    textBlobs = nullptr;
    textBlobsCount = 0;
    ret = OH_Drawing_TextBlobCreateFromTextWithFallback(glyphs, sizeof(glyphs), font,
        TEXT_ENCODING_GLYPH_ID, &textBlobs, &textBlobsCount);
    EXPECT_EQ(ret, OH_DRAWING_SUCCESS);
    EXPECT_EQ(textBlobsCount, 1);
    EXPECT_EQ(OH_Drawing_TextBlobsArrayDestroy(textBlobs, textBlobsCount), OH_DRAWING_SUCCESS);
    OH_Drawing_FontDestroy(font);
}

/**
 * @tc.name: OH_Drawing_TextBlobCreateFromPosTextWithFallback_001
 * @tc.desc: test for OH_Drawing_TextBlobCreateFromPosTextWithFallback with normal utf8 text.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NativeTextBlobTest, OH_Drawing_TextBlobCreateFromPosTextWithFallback_001, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    ASSERT_NE(font, nullptr);
    const char* text = "Hi你好";
    OH_Drawing_Point2D points[] = { { 0.f, 0.f }, { 10.f, 0.f }, { 20.f, 0.f }, { 30.f, 0.f } };
    OH_Drawing_TextBlob** textBlobs = nullptr;
    uint32_t textBlobsCount = 0;
    OH_Drawing_ErrorCode ret = OH_Drawing_TextBlobCreateFromPosTextWithFallback(text, strlen(text), points,
        font, TEXT_ENCODING_UTF8, &textBlobs, &textBlobsCount);
    EXPECT_EQ(ret, OH_DRAWING_SUCCESS);
    EXPECT_GT(textBlobsCount, 0);
    for (uint32_t i = 0; i < textBlobsCount; i++) {
        EXPECT_NE(textBlobs[i], nullptr);
    }
    EXPECT_EQ(OH_Drawing_TextBlobsArrayDestroy(textBlobs, textBlobsCount), OH_DRAWING_SUCCESS);
    OH_Drawing_FontDestroy(font);
}

/**
 * @tc.name: OH_Drawing_TextBlobCreateFromPosTextWithFallback_002
 * @tc.desc: test for OH_Drawing_TextBlobCreateFromPosTextWithFallback with utf16 text encoding.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NativeTextBlobTest, OH_Drawing_TextBlobCreateFromPosTextWithFallback_002, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    ASSERT_NE(font, nullptr);
    const char16_t text[] = u"你好";
    size_t byteLength = sizeof(text) - sizeof(char16_t);
    ASSERT_EQ(OH_Drawing_FontCountText(font, text, byteLength, TEXT_ENCODING_UTF16), 2);
    OH_Drawing_Point2D points[] = { { 0.f, 0.f }, { 10.f, 0.f } };
    OH_Drawing_TextBlob** textBlobs = nullptr;
    uint32_t textBlobsCount = 0;
    OH_Drawing_ErrorCode ret = OH_Drawing_TextBlobCreateFromPosTextWithFallback(text, byteLength, points,
        font, TEXT_ENCODING_UTF16, &textBlobs, &textBlobsCount);
    EXPECT_EQ(ret, OH_DRAWING_SUCCESS);
    EXPECT_GT(textBlobsCount, 0);
    EXPECT_EQ(OH_Drawing_TextBlobsArrayDestroy(textBlobs, textBlobsCount), OH_DRAWING_SUCCESS);
    OH_Drawing_FontDestroy(font);
}

/**
 * @tc.name: OH_Drawing_TextBlobCreateFromPosTextWithFallback_003
 * @tc.desc: test for OH_Drawing_TextBlobCreateFromPosTextWithFallback with invalid parameters.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NativeTextBlobTest, OH_Drawing_TextBlobCreateFromPosTextWithFallback_003, TestSize.Level2)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    ASSERT_NE(font, nullptr);
    const char* text = "Hi你好";
    OH_Drawing_Point2D points[] = { { 0.f, 0.f }, { 10.f, 0.f }, { 20.f, 0.f }, { 30.f, 0.f } };
    OH_Drawing_TextBlob** textBlobs = nullptr;
    uint32_t textBlobsCount = 0;
    EXPECT_EQ(OH_Drawing_TextBlobCreateFromPosTextWithFallback(nullptr, strlen(text), points, font,
        TEXT_ENCODING_UTF8, &textBlobs, &textBlobsCount), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_TextBlobCreateFromPosTextWithFallback(text, 0, points, font, TEXT_ENCODING_UTF8,
        &textBlobs, &textBlobsCount), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_TextBlobCreateFromPosTextWithFallback(text, strlen(text), nullptr, font,
        TEXT_ENCODING_UTF8, &textBlobs, &textBlobsCount), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_TextBlobCreateFromPosTextWithFallback(text, strlen(text), points, nullptr,
        TEXT_ENCODING_UTF8, &textBlobs, &textBlobsCount), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_TextBlobCreateFromPosTextWithFallback(text, strlen(text), points, font,
        TEXT_ENCODING_UTF8, nullptr, &textBlobsCount), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_TextBlobCreateFromPosTextWithFallback(text, strlen(text), points, font,
        TEXT_ENCODING_UTF8, &textBlobs, nullptr), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_TextBlobCreateFromPosTextWithFallback(text, strlen(text), points, font,
        INVALID_ENCODING, &textBlobs, &textBlobsCount), OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE);
    EXPECT_EQ(OH_Drawing_TextBlobCreateFromPosTextWithFallback(text, strlen(text), points, font,
        INVALID_ENCODING_MIN, &textBlobs, &textBlobsCount), OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE);
    OH_Drawing_FontDestroy(font);
}

/**
 * @tc.name: OH_Drawing_TextBlobCreateFromStringWithFallback_001
 * @tc.desc: test for OH_Drawing_TextBlobCreateFromStringWithFallback with normal utf8 string.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NativeTextBlobTest, OH_Drawing_TextBlobCreateFromStringWithFallback_001, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    ASSERT_NE(font, nullptr);
    const char* text = "Hello你好";
    OH_Drawing_TextBlob** textBlobs = nullptr;
    uint32_t textBlobsCount = 0;
    OH_Drawing_ErrorCode ret = OH_Drawing_TextBlobCreateFromStringWithFallback(text, font, TEXT_ENCODING_UTF8,
        &textBlobs, &textBlobsCount);
    EXPECT_EQ(ret, OH_DRAWING_SUCCESS);
    EXPECT_GT(textBlobsCount, 0);
    for (uint32_t i = 0; i < textBlobsCount; i++) {
        EXPECT_NE(textBlobs[i], nullptr);
    }
    EXPECT_EQ(OH_Drawing_TextBlobsArrayDestroy(textBlobs, textBlobsCount), OH_DRAWING_SUCCESS);
    OH_Drawing_FontDestroy(font);
}

/**
 * @tc.name: OH_Drawing_TextBlobCreateFromStringWithFallback_002
 * @tc.desc: test for OH_Drawing_TextBlobCreateFromStringWithFallback with invalid parameters.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NativeTextBlobTest, OH_Drawing_TextBlobCreateFromStringWithFallback_002, TestSize.Level2)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    ASSERT_NE(font, nullptr);
    const char* text = "Hello";
    OH_Drawing_TextBlob** textBlobs = nullptr;
    uint32_t textBlobsCount = 0;
    EXPECT_EQ(OH_Drawing_TextBlobCreateFromStringWithFallback(nullptr, font, TEXT_ENCODING_UTF8,
        &textBlobs, &textBlobsCount), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_TextBlobCreateFromStringWithFallback(text, nullptr, TEXT_ENCODING_UTF8,
        &textBlobs, &textBlobsCount), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_TextBlobCreateFromStringWithFallback(text, font, TEXT_ENCODING_UTF8,
        nullptr, &textBlobsCount), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_TextBlobCreateFromStringWithFallback(text, font, TEXT_ENCODING_UTF8,
        &textBlobs, nullptr), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_TextBlobCreateFromStringWithFallback(text, font, INVALID_ENCODING,
        &textBlobs, &textBlobsCount), OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE);
    EXPECT_EQ(OH_Drawing_TextBlobCreateFromStringWithFallback(text, font, INVALID_ENCODING_MIN,
        &textBlobs, &textBlobsCount), OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE);
    OH_Drawing_FontDestroy(font);
}

/**
 * @tc.name: OH_Drawing_TextBlobsArrayDestroy_001
 * @tc.desc: test for OH_Drawing_TextBlobsArrayDestroy with invalid parameters.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NativeTextBlobTest, OH_Drawing_TextBlobsArrayDestroy_001, TestSize.Level2)
{
    EXPECT_EQ(OH_Drawing_TextBlobsArrayDestroy(nullptr, 1), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    // a non-null array with zero count is rejected before any access, so a stack array is safe here
    OH_Drawing_TextBlob* textBlobs[] = { nullptr };
    EXPECT_EQ(OH_Drawing_TextBlobsArrayDestroy(textBlobs, 0), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
