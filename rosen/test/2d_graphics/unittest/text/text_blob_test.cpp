/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class TextBlobTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void TextBlobTest::SetUpTestCase() {}
void TextBlobTest::TearDownTestCase() {}
void TextBlobTest::SetUp() {}
void TextBlobTest::TearDown() {}

/**
 * @tc.name: TextContrastTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(TextBlobTest, TextContrastTest001, TestSize.Level1)
{
    Font font;
    auto textBlob1 = TextBlob::MakeFromString("11", font, TextEncoding::UTF8);
    ASSERT_NE(textBlob1, nullptr);
    EXPECT_EQ(textBlob1->GetTextContrast(), TextContrast::FOLLOW_SYSTEM);
    ProcessTextConstrast::Instance().SetTextContrast(TextContrast::ENABLE_CONTRAST);
    auto textBlob2 = TextBlob::MakeFromString("11", font, TextEncoding::UTF8);
    ASSERT_NE(textBlob2, nullptr);
    EXPECT_EQ(textBlob2->GetTextContrast(), ProcessTextConstrast::Instance().GetTextContrast());
    textBlob2->SetTextContrast(TextContrast::DISABLE_CONTRAST);
    EXPECT_EQ(textBlob2->GetTextContrast(), TextContrast::DISABLE_CONTRAST);
}

/**
 * @tc.name: MakeFromTextWithFallback_001
 * @tc.desc: test for MakeFromTextWithFallback with normal utf8 text.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TextBlobTest, MakeFromTextWithFallback_001, TestSize.Level1)
{
    Font font;
    const char* text = "Hello你好";
    auto textBlobs = TextBlob::MakeFromTextWithFallback(text, strlen(text), font, TextEncoding::UTF8);
    EXPECT_FALSE(textBlobs.empty());
    for (const auto& textBlob : textBlobs) {
        EXPECT_NE(textBlob, nullptr);
    }
}

/**
 * @tc.name: MakeFromTextWithFallback_002
 * @tc.desc: test for MakeFromTextWithFallback with nullptr text and zero byteLength.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TextBlobTest, MakeFromTextWithFallback_002, TestSize.Level2)
{
    Font font;
    const char* text = "test";
    auto textBlobs = TextBlob::MakeFromTextWithFallback(nullptr, strlen(text), font, TextEncoding::UTF8);
    EXPECT_TRUE(textBlobs.empty());
    textBlobs = TextBlob::MakeFromTextWithFallback(text, 0, font, TextEncoding::UTF8);
    EXPECT_TRUE(textBlobs.empty());
}

/**
 * @tc.name: MakeFromTextWithFallback_003
 * @tc.desc: test for MakeFromTextWithFallback with glyph id encoding returns a single blob.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TextBlobTest, MakeFromTextWithFallback_003, TestSize.Level2)
{
    Font font;
    const char* text = "test";
    int count = font.CountText(text, strlen(text), TextEncoding::UTF8);
    ASSERT_GT(count, 0);
    std::vector<uint16_t> glyphs(count);
    font.TextToGlyphs(text, strlen(text), TextEncoding::UTF8, glyphs.data(), count);
    auto textBlobs = TextBlob::MakeFromTextWithFallback(glyphs.data(), glyphs.size() * sizeof(uint16_t),
        font, TextEncoding::GLYPH_ID);
    EXPECT_EQ(textBlobs.size(), 1u);
}

/**
 * @tc.name: MakeFromPosTextWithFallback_001
 * @tc.desc: test for MakeFromPosTextWithFallback with normal utf8 text and positions.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TextBlobTest, MakeFromPosTextWithFallback_001, TestSize.Level1)
{
    Font font;
    const char* text = "Hi你好";
    ASSERT_EQ(font.CountText(text, strlen(text), TextEncoding::UTF8), 4);
    Point pos[] = { Point(0.f, 0.f), Point(10.f, 0.f), Point(20.f, 0.f), Point(30.f, 0.f) };
    auto textBlobs = TextBlob::MakeFromPosTextWithFallback(text, strlen(text), pos, font, TextEncoding::UTF8);
    EXPECT_FALSE(textBlobs.empty());
    for (const auto& textBlob : textBlobs) {
        EXPECT_NE(textBlob, nullptr);
    }
}

/**
 * @tc.name: MakeFromPosTextWithFallback_002
 * @tc.desc: test for MakeFromPosTextWithFallback with nullptr text, nullptr pos and zero byteLength.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TextBlobTest, MakeFromPosTextWithFallback_002, TestSize.Level2)
{
    Font font;
    const char* text = "test";
    Point pos[] = { Point(0.f, 0.f), Point(10.f, 0.f), Point(20.f, 0.f), Point(30.f, 0.f) };
    auto textBlobs = TextBlob::MakeFromPosTextWithFallback(nullptr, strlen(text), pos, font,
        TextEncoding::UTF8);
    EXPECT_TRUE(textBlobs.empty());
    textBlobs = TextBlob::MakeFromPosTextWithFallback(text, 0, pos, font, TextEncoding::UTF8);
    EXPECT_TRUE(textBlobs.empty());
    textBlobs = TextBlob::MakeFromPosTextWithFallback(text, strlen(text), nullptr, font, TextEncoding::UTF8);
    EXPECT_TRUE(textBlobs.empty());
}

/**
 * @tc.name: MakeFromPosTextWithFallback_003
 * @tc.desc: test for MakeFromPosTextWithFallback with glyph id encoding returns a single blob.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TextBlobTest, MakeFromPosTextWithFallback_003, TestSize.Level2)
{
    Font font;
    const char* text = "test";
    int count = font.CountText(text, strlen(text), TextEncoding::UTF8);
    ASSERT_GT(count, 0);
    std::vector<uint16_t> glyphs(count);
    font.TextToGlyphs(text, strlen(text), TextEncoding::UTF8, glyphs.data(), count);
    Point pos[] = { Point(0.f, 0.f), Point(10.f, 0.f), Point(20.f, 0.f), Point(30.f, 0.f) };
    auto textBlobs = TextBlob::MakeFromPosTextWithFallback(glyphs.data(), glyphs.size() * sizeof(uint16_t),
        pos, font, TextEncoding::GLYPH_ID);
    EXPECT_EQ(textBlobs.size(), 1u);
}

/**
 * @tc.name: MakeFromStringWithFallback_001
 * @tc.desc: test for MakeFromStringWithFallback with normal utf8 string.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TextBlobTest, MakeFromStringWithFallback_001, TestSize.Level1)
{
    Font font;
    auto textBlobs = TextBlob::MakeFromStringWithFallback("Hello你好", font, TextEncoding::UTF8);
    EXPECT_FALSE(textBlobs.empty());
    for (const auto& textBlob : textBlobs) {
        EXPECT_NE(textBlob, nullptr);
    }
}

/**
 * @tc.name: MakeFromStringWithFallback_002
 * @tc.desc: test for MakeFromStringWithFallback with nullptr string and empty string.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TextBlobTest, MakeFromStringWithFallback_002, TestSize.Level2)
{
    Font font;
    auto textBlobs = TextBlob::MakeFromStringWithFallback(nullptr, font, TextEncoding::UTF8);
    EXPECT_TRUE(textBlobs.empty());
    textBlobs = TextBlob::MakeFromStringWithFallback("", font, TextEncoding::UTF8);
    EXPECT_TRUE(textBlobs.empty());
}

/**
 * @tc.name: MakeFromStringWithFallback_003
 * @tc.desc: test for MakeFromStringWithFallback glyph count equals codepoint count.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TextBlobTest, MakeFromStringWithFallback_003, TestSize.Level3)
{
    Font font;
    const char* text = "Hello你好"; // 7 codepoints
    auto textBlobs = TextBlob::MakeFromStringWithFallback(text, font, TextEncoding::UTF8);
    ASSERT_FALSE(textBlobs.empty());
    size_t glyphCount = 0;
    for (const auto& textBlob : textBlobs) {
        ASSERT_NE(textBlob, nullptr);
        std::vector<uint16_t> glyphIds;
        TextBlob::GetDrawingGlyphIDforTextBlob(textBlob.get(), glyphIds);
        glyphCount += glyphIds.size();
    }
    EXPECT_EQ(glyphCount, 7u);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

