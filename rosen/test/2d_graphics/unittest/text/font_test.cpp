/*
 * Copyright (c) 2025-2026 Huawei Device Co., Ltd.
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

#include <cstring>

#include "text/font.h"
#include "text/text_blob.h"
#include "utils/text_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class FontTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void FontTest::SetUpTestCase() {}
void FontTest::TearDownTestCase() {}
void FontTest::SetUp() {}
void FontTest::TearDown() {}

/**
 * @tc.name: FontMeasureTextWithBrushOrPenTest001
 * @tc.desc: 
 * @tc.type: FUNC
 * @tc.require:AR20250515745872
 * @tc.author:
 */
HWTEST_F(FontTest, FontMeasureTextWithBrushOrPenTest001, TestSize.Level1)
{
    Font font;
    std::shared_ptr<OHOS::Rosen::Drawing::Typeface> zhCnTypeface = Drawing::Typeface::MakeDefault();
    font.SetTypeface(zhCnTypeface);
    Brush brush;
    Pen pen;
    const char* text = "你好世界";
    auto textWidth = font.MeasureText(text, strlen(text), TextEncoding::UTF8, nullptr, &brush, &pen);
    EXPECT_EQ((int)textWidth, 0);
}

/**
 * @tc.name: FontGetWidthsBoundsTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR20250515745872
 * @tc.author:
 */
HWTEST_F(FontTest, FontGetWidthsBoundsTest001, TestSize.Level1)
{
    Font font;
    std::shared_ptr<OHOS::Rosen::Drawing::Typeface> zhCnTypeface = Drawing::Typeface::MakeDefault();
    font.SetTypeface(zhCnTypeface);
    Brush brush;
    Pen pen;
    float widths[50] = {0.0f};
    Rect* bounds = new Rect[50];
    font.GetWidthsBounds(nullptr, 0, widths, bounds, &brush, &pen);
    EXPECT_EQ((int)widths[0], 0);
    ASSERT_NE(bounds, nullptr);
    if (bounds != nullptr) {
        delete [] bounds;
        bounds = nullptr;
    }
}

/**
 * @tc.name: FontGetPosTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR20250515745872
 * @tc.author:
 */
HWTEST_F(FontTest, FontGetPosTest001, TestSize.Level1)
{
    Font font;
    std::shared_ptr<OHOS::Rosen::Drawing::Typeface> zhCnTypeface = Drawing::Typeface::MakeDefault();
    font.SetTypeface(zhCnTypeface);
    uint16_t glyphs[] = { 0, 0 };
    Point points[] = { {0, 0}, {0, 0} };
    font.GetPos(glyphs, 2, points, {10, 10}); // 2:count
    ASSERT_NE((int)points[0].GetX(), 0);
    ASSERT_NE((int)points[0].GetY(), 0);
}

/**
 * @tc.name: FontGetSpacingTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR20250515745872
 * @tc.author:
 */
HWTEST_F(FontTest, FontGetSpacingTest001, TestSize.Level1)
{
    Font font;
    std::shared_ptr<OHOS::Rosen::Drawing::Typeface> zhCnTypeface = Drawing::Typeface::MakeDefault();
    font.SetTypeface(zhCnTypeface);
    auto spacing = font.GetSpacing();
    ASSERT_NE((int)spacing, 0);
}

/**
 * @tc.name: FontUnicharToGlyphWithFeatures001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:ICG6L3
 * @tc.author:
 */
HWTEST_F(FontTest, FontUnicharToGlyphWithFeatures001, TestSize.Level1)
{
    Font font;
    const char* str = "a";
    const char* strEmpty = "";
    std::shared_ptr<Drawing::DrawingFontFeatures> features = std::make_shared<Drawing::DrawingFontFeatures>();
    uint16_t glyph = font.UnicharToGlyphWithFeatures(str, nullptr);
    ASSERT_EQ(glyph, 0);
    glyph = font.UnicharToGlyphWithFeatures(strEmpty, features);
    ASSERT_EQ(glyph, 0);
    std::shared_ptr<OHOS::Rosen::Drawing::Typeface> zhCnTypeface = Drawing::Typeface::MakeDefault();
    font.SetTypeface(zhCnTypeface);
    glyph = font.UnicharToGlyphWithFeatures(strEmpty, features);
    ASSERT_EQ(glyph, 0);
}

/**
 * @tc.name: MeasureSingleCharacterWithFeatures001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:ICG6L3
 * @tc.author:
 */
HWTEST_F(FontTest, MeasureSingleCharacterWithFeatures001, TestSize.Level1)
{
    Font font;
    const char* str = "a";
    float width = font.MeasureSingleCharacterWithFeatures(str, 0, nullptr);
    ASSERT_NE(width, 0);
    const char* strNoFallback = "\uE000";
    uint32_t unicodeNoFallback = 0xE000;
    width = font.MeasureSingleCharacterWithFeatures(strNoFallback, unicodeNoFallback, nullptr);
    ASSERT_EQ(width, 0);
}

/**
 * @tc.name: GetByteLength001
 * @tc.desc: test for GetByteLength with GLYPH_ID encoding.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontTest, GetByteLength001, TestSize.Level1)
{
    const char* text = "test";
    size_t len = GetByteLength(text, 4, TextEncoding::GLYPH_ID);
    ASSERT_EQ(len, 4);
}

/**
 * @tc.name: GetByteLength002
 * @tc.desc: test for GetByteLength with invalid encoding.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontTest, GetByteLength002, TestSize.Level1)
{
    const char* text = "test";
    size_t len = GetByteLength(text, 4, static_cast<TextEncoding>(100));
    ASSERT_EQ(len, 0);
}

/**
 * @tc.name: DecodeTextToCodepoints001
 * @tc.desc: test for DecodeTextToCodepoints with empty text.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontTest, DecodeTextToCodepoints001, TestSize.Level1)
{
    std::vector<int32_t> out;
    bool result = DecodeTextToCodepoints(nullptr, 4, TextEncoding::UTF8, out);
    ASSERT_EQ(result, false);
    result = DecodeTextToCodepoints("test", 0, TextEncoding::UTF8, out);
    ASSERT_EQ(result, false);
}

/**
 * @tc.name: DecodeTextToCodepoints002
 * @tc.desc: test for DecodeTextToCodepoints with GLYPH_ID encoding.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontTest, DecodeTextToCodepoints002, TestSize.Level1)
{
    std::vector<int32_t> out;
    bool result = DecodeTextToCodepoints("test", 4, TextEncoding::GLYPH_ID, out);
    ASSERT_EQ(result, false);
}

/**
 * @tc.name: DecodeTextToCodepoints003
 * @tc.desc: test for DecodeTextToCodepoints with invalid encoding.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontTest, DecodeTextToCodepoints003, TestSize.Level1)
{
    std::vector<int32_t> out;
    bool result = DecodeTextToCodepoints("test", 4, static_cast<TextEncoding>(100), out);
    ASSERT_EQ(result, false);
}

/**
 * @tc.name: DecodeTextToCodepoints004
 * @tc.desc: test for DecodeTextToCodepoints with UTF32.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontTest, DecodeTextToCodepoints004, TestSize.Level1)
{
    const char32_t text[] = U"你好";
    std::vector<int32_t> out;
    bool result = DecodeTextToCodepoints(text, sizeof(text), TextEncoding::UTF32, out);
    ASSERT_EQ(result, true);
    ASSERT_EQ(out.size(), 3);  // 2 chars + null terminator
}

/**
 * @tc.name: DecodeTextToCodepoints005
 * @tc.desc: test for DecodeTextToCodepoints with UTF16 misaligned.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontTest, DecodeTextToCodepoints005, TestSize.Level1)
{
    const char16_t text[] = u"你好";
    std::vector<int32_t> out;
    bool result = DecodeTextToCodepoints(text, 3, TextEncoding::UTF16, out);
    ASSERT_EQ(result, false);
}

/**
 * @tc.name: DecodeTextToCodepoints006
 * @tc.desc: test for DecodeTextToCodepoints with UTF32 misaligned.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontTest, DecodeTextToCodepoints006, TestSize.Level1)
{
    const char32_t text[] = U"你好";
    std::vector<int32_t> out;
    bool result = DecodeTextToCodepoints(text, 3, TextEncoding::UTF32, out);
    ASSERT_EQ(result, true);
}

/**
 * @tc.name: FontGetTextPathWithFallback001
 * @tc.desc: test for GetTextPathWithFallback with path nullptr.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontTest, FontGetTextPathWithFallback001, TestSize.Level1)
{
    Font font;
    const char* text = "test";
    Path* path = nullptr;
    font.GetTextPathWithFallback(text, 4, TextEncoding::UTF8, 0, 0, path);
    ASSERT_TRUE(path == nullptr);
}

/**
 * @tc.name: FontGetTextPathWithFallback002
 * @tc.desc: test for GetTextPathWithFallback with text nullptr.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontTest, FontGetTextPathWithFallback002, TestSize.Level1)
{
    Font font;
    Path path;
    font.GetTextPathWithFallback(nullptr, 4, TextEncoding::UTF8, 0, 0, &path);
    ASSERT_EQ(path.IsEmpty(), true);
}

/**
 * @tc.name: FontGetTextPathWithFallback003
 * @tc.desc: test for GetTextPathWithFallback with byteLength 0.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontTest, FontGetTextPathWithFallback003, TestSize.Level1)
{
    Font font;
    Path path;
    const char* text = "test";
    font.GetTextPathWithFallback(text, 0, TextEncoding::UTF8, 0, 0, &path);
    ASSERT_EQ(path.IsEmpty(), true);
}

/**
 * @tc.name: TextToGlyphsWithFallback_001
 * @tc.desc: test for TextToGlyphsWithFallback with normal utf8 text.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontTest, TextToGlyphsWithFallback_001, TestSize.Level1)
{
    Font font;
    const char* text = "Hello你好"; // 7 codepoints
    auto fallbacks = font.TextToGlyphsWithFallback(text, strlen(text), TextEncoding::UTF8);
    EXPECT_FALSE(fallbacks.empty());
    size_t glyphCount = 0;
    for (const auto& run : fallbacks) {
        glyphCount += run.glyphIds.size();
    }
    EXPECT_EQ(glyphCount, 7u);
}

/**
 * @tc.name: TextToGlyphsWithFallback_002
 * @tc.desc: test for TextToGlyphsWithFallback with nullptr text and zero byteLength.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontTest, TextToGlyphsWithFallback_002, TestSize.Level2)
{
    Font font;
    const char* text = "test";
    auto fallbacks = font.TextToGlyphsWithFallback(nullptr, strlen(text), TextEncoding::UTF8);
    EXPECT_TRUE(fallbacks.empty());
    fallbacks = font.TextToGlyphsWithFallback(text, 0, TextEncoding::UTF8);
    EXPECT_TRUE(fallbacks.empty());
}

/**
 * @tc.name: TextToGlyphsWithFallback_003
 * @tc.desc: test for TextToGlyphsWithFallback with glyph id encoding returns a single run.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontTest, TextToGlyphsWithFallback_003, TestSize.Level2)
{
    Font font;
    const char* text = "test";
    int count = font.CountText(text, strlen(text), TextEncoding::UTF8);
    ASSERT_GT(count, 0);
    std::vector<uint16_t> glyphs(count);
    font.TextToGlyphs(text, strlen(text), TextEncoding::UTF8, glyphs.data(), count);
    auto fallbacks = font.TextToGlyphsWithFallback(glyphs.data(), glyphs.size() * sizeof(uint16_t),
        TextEncoding::GLYPH_ID);
    ASSERT_EQ(fallbacks.size(), 1u);
    EXPECT_EQ(fallbacks[0].glyphIds, glyphs);
}

/**
 * @tc.name: TextToGlyphsWithFallback_004
 * @tc.desc: test for TextToGlyphsWithFallback with utf16 and utf32 encoding.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontTest, TextToGlyphsWithFallback_004, TestSize.Level3)
{
    Font font;
    const char16_t text16[] = u"你好";
    auto fallbacks16 = font.TextToGlyphsWithFallback(text16, sizeof(text16) - sizeof(char16_t),
        TextEncoding::UTF16);
    size_t glyphCount16 = 0;
    for (const auto& run : fallbacks16) {
        glyphCount16 += run.glyphIds.size();
    }
    EXPECT_EQ(glyphCount16, 2u);

    const char32_t text32[] = U"你好";
    auto fallbacks32 = font.TextToGlyphsWithFallback(text32, sizeof(text32) - sizeof(char32_t),
        TextEncoding::UTF32);
    size_t glyphCount32 = 0;
    for (const auto& run : fallbacks32) {
        glyphCount32 += run.glyphIds.size();
    }
    EXPECT_EQ(glyphCount32, 2u);
}

/**
 * @tc.name: MeasureTextWithFallback_001
 * @tc.desc: test for MeasureTextWithFallback with normal utf8 text.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontTest, MeasureTextWithFallback_001, TestSize.Level1)
{
    Font font;
    const char* text = "Hello你好";
    scalar width = font.MeasureTextWithFallback(text, strlen(text), TextEncoding::UTF8);
    EXPECT_GT(width, 0.f);
    Rect bounds;
    scalar widthWithBounds = font.MeasureTextWithFallback(text, strlen(text), TextEncoding::UTF8, &bounds);
    EXPECT_GT(widthWithBounds, 0.f);
    EXPECT_FLOAT_EQ(width, widthWithBounds);
    EXPECT_GT(bounds.GetWidth(), 0.f);
}

/**
 * @tc.name: MeasureTextWithFallback_002
 * @tc.desc: test for MeasureTextWithFallback with nullptr text and zero byteLength.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontTest, MeasureTextWithFallback_002, TestSize.Level2)
{
    Font font;
    const char* text = "test";
    EXPECT_EQ(font.MeasureTextWithFallback(nullptr, strlen(text), TextEncoding::UTF8), 0.f);
    EXPECT_EQ(font.MeasureTextWithFallback(text, 0, TextEncoding::UTF8), 0.f);
    Rect bounds;
    EXPECT_EQ(font.MeasureTextWithFallback(nullptr, strlen(text), TextEncoding::UTF8, &bounds), 0.f);
    EXPECT_EQ(font.MeasureTextWithFallback(text, 0, TextEncoding::UTF8, &bounds), 0.f);
}

/**
 * @tc.name: MeasureTextWithFallback_003
 * @tc.desc: test for MeasureTextWithFallback with glyph id encoding equals MeasureText.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontTest, MeasureTextWithFallback_003, TestSize.Level2)
{
    Font font;
    const char* text = "test";
    int count = font.CountText(text, strlen(text), TextEncoding::UTF8);
    ASSERT_GT(count, 0);
    std::vector<uint16_t> glyphs(count);
    font.TextToGlyphs(text, strlen(text), TextEncoding::UTF8, glyphs.data(), count);
    size_t byteLength = glyphs.size() * sizeof(uint16_t);
    scalar expected = font.MeasureText(glyphs.data(), byteLength, TextEncoding::GLYPH_ID);
    EXPECT_FLOAT_EQ(font.MeasureTextWithFallback(glyphs.data(), byteLength, TextEncoding::GLYPH_ID), expected);
}

/**
 * @tc.name: MeasureTextWithFallbackBrushOrPen_001
 * @tc.desc: test for MeasureTextWithFallback with brush or pen.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontTest, MeasureTextWithFallbackBrushOrPen_001, TestSize.Level1)
{
    Font font;
    Brush brush;
    Pen pen;
    const char* text = "Hello你好";
    scalar widthWithBrush = font.MeasureTextWithFallback(text, strlen(text), TextEncoding::UTF8,
        nullptr, &brush, nullptr);
    EXPECT_GT(widthWithBrush, 0.f);
    scalar widthWithPen = font.MeasureTextWithFallback(text, strlen(text), TextEncoding::UTF8,
        nullptr, nullptr, &pen);
    EXPECT_GT(widthWithPen, 0.f);
}

/**
 * @tc.name: MeasureTextWithFallbackBrushOrPen_002
 * @tc.desc: test for MeasureTextWithFallback with both brush and pen.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(FontTest, MeasureTextWithFallbackBrushOrPen_002, TestSize.Level2)
{
    Font font;
    Brush brush;
    Pen pen;
    const char* text = "Hello你好";
    // at most one of brush and pen may be non-null, so both set yields no measurable width
    scalar width = font.MeasureTextWithFallback(text, strlen(text), TextEncoding::UTF8, nullptr,
        &brush, &pen);
    EXPECT_EQ(width, 0.f);
    EXPECT_EQ(font.MeasureTextWithFallback(nullptr, strlen(text), TextEncoding::UTF8,
        nullptr, &brush, nullptr), 0.f);
    EXPECT_EQ(font.MeasureTextWithFallback(text, 0, TextEncoding::UTF8,
        nullptr, &brush, nullptr), 0.f);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
