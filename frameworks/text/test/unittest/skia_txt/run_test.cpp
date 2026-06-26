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

#include "gtest/gtest.h"
#include "font_collection.h"
#include "paragraph_builder.h"
#include "paragraph_style.h"
#include "run_impl.h"
#include "typography_types.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen;
using namespace OHOS::Rosen::Drawing;
using namespace OHOS::Rosen::SPText;

namespace txt {
namespace {
    constexpr static float FLOAT_DATA_EPSILON = 1e-6f;
}
class RunTest : public testing::Test {
public:
    void SetUp() override;
    void TearDown() override;
    void PrepareCreateRunForGlyphDrawing();
    // Builds a paragraph from text_ with compressHeadPunctuation toggled and fills runs_.
    void PrepareRunForHeadPunctuationCompression(bool compress);

private:
    // 50 is the width of the layout, just for test
    int layoutWidth_ = 50;
    int fontSize_ = 100;
    int layoutWidthForGlyph_ = 1200;
    // this is the default font family name, just for test
    std::string familyName_ = { 0x48, 0x61, 0x72, 0x6d, 0x6f, 0x6e, 0x79, 0x4f, 0x53, 0x2d, 0x53, 0x61, 0x6e, 0x73 };

    std::shared_ptr<Paragraph> paragraph_;
    std::vector<std::unique_ptr<SPText::Run>> runs_;
    std::u16string text_ = u"RunTest";
};

void RunTest::SetUp()
{
    ParagraphStyle paragraphStyle;
    std::shared_ptr<FontCollection> fontCollection = std::make_shared<FontCollection>();
    ASSERT_NE(fontCollection, nullptr);
    fontCollection->SetupDefaultFontManager();
    std::shared_ptr<ParagraphBuilder> paragraphBuilder = ParagraphBuilder::Create(paragraphStyle, fontCollection);
    ASSERT_NE(paragraphBuilder, nullptr);
    std::u16string text = u"RunTest";
    paragraphBuilder->AddText(text);
    paragraph_ = paragraphBuilder->Build();
    ASSERT_NE(paragraph_, nullptr);
    paragraph_->Layout(layoutWidth_);
    auto textLineBases = paragraph_->GetTextLines();
    runs_ = textLineBases[0]->GetGlyphRuns();
}

void RunTest::TearDown()
{
    paragraph_.reset();
    runs_.clear();
}

void RunTest::PrepareCreateRunForGlyphDrawing()
{
    ParagraphStyle paragraphStyle;
    paragraphStyle.fontSize = fontSize_;
    std::shared_ptr<FontCollection> fontCollection = std::make_shared<FontCollection>();
    ASSERT_NE(fontCollection, nullptr);
    fontCollection->SetupDefaultFontManager();
    std::shared_ptr<ParagraphBuilder> paragraphBuilder = ParagraphBuilder::Create(paragraphStyle, fontCollection);
    ASSERT_NE(paragraphBuilder, nullptr);
    paragraphBuilder->AddText(text_);
    paragraph_ = paragraphBuilder->Build();
    ASSERT_NE(paragraph_, nullptr);
    paragraph_->Layout(layoutWidthForGlyph_);
    auto textLineBases = paragraph_->GetTextLines();
    runs_ = textLineBases[0]->GetGlyphRuns();
}

void RunTest::PrepareRunForHeadPunctuationCompression(bool compress)
{
    ParagraphStyle paragraphStyle;
    paragraphStyle.fontSize = fontSize_;
    paragraphStyle.compressHeadPunctuation = compress;
    std::shared_ptr<FontCollection> fontCollection = std::make_shared<FontCollection>();
    ASSERT_NE(fontCollection, nullptr);
    fontCollection->SetupDefaultFontManager();
    std::shared_ptr<ParagraphBuilder> paragraphBuilder = ParagraphBuilder::Create(paragraphStyle, fontCollection);
    ASSERT_NE(paragraphBuilder, nullptr);
    paragraphBuilder->AddText(text_);
    paragraph_ = paragraphBuilder->Build();
    ASSERT_NE(paragraph_, nullptr);
    paragraph_->Layout(layoutWidthForGlyph_);
    auto textLineBases = paragraph_->GetTextLines();
    ASSERT_NE(textLineBases.size(), 0);
    runs_ = textLineBases[0]->GetGlyphRuns();
    ASSERT_NE(runs_.size(), 0);
}

/*
 * @tc.name: RunTest001
 * @tc.desc: test for GetFont
 * @tc.type: FUNC
 */
HWTEST_F(RunTest, RunTest001, TestSize.Level0)
{
    EXPECT_EQ(runs_.size(), 1);
    ASSERT_NE(runs_.at(0), nullptr);
    EXPECT_EQ(runs_[0]->GetFont().GetTypeface()->GetFamilyName(), familyName_);
}

/*
 * @tc.name: RunTest002
 * @tc.desc: test for GetGlyphCount
 * @tc.type: FUNC
 */
HWTEST_F(RunTest, RunTest002, TestSize.Level0)
{
    EXPECT_EQ(runs_.size(), 1);
    ASSERT_NE(runs_.at(0), nullptr);
    EXPECT_EQ(runs_[0]->GetGlyphCount(), 7);
}

/*
 * @tc.name: RunTest003
 * @tc.desc: test for GetGlyphs
 * @tc.type: FUNC
 */
HWTEST_F(RunTest, RunTest003, TestSize.Level0)
{
    EXPECT_EQ(runs_.size(), 1);
    ASSERT_NE(runs_.at(0), nullptr);
    auto glyphs = runs_[0]->GetGlyphs();
    EXPECT_NE(glyphs[0], 0);
}

/*
 * @tc.name: RunTest004
 * @tc.desc: test for GetPositions
 * @tc.type: FUNC
 */
HWTEST_F(RunTest, RunTest004, TestSize.Level0)
{
    EXPECT_EQ(runs_.size(), 1);
    ASSERT_NE(runs_.at(0), nullptr);
    EXPECT_EQ(runs_[0]->GetPositions().size(), 7);
    EXPECT_EQ(runs_[0]->GetPositions()[0].GetX(), 0);
}

/*
 * @tc.name: RunTest005
 * @tc.desc: test for GetOffsets
 * @tc.type: FUNC
 */
HWTEST_F(RunTest, RunTest005, TestSize.Level0)
{
    EXPECT_EQ(runs_.size(), 1);
    ASSERT_NE(runs_[0], nullptr);
    EXPECT_EQ(runs_[0]->GetOffsets().size(), 7);
    EXPECT_EQ(runs_[0]->GetOffsets()[0].GetX(), 0);
}

/*
 * @tc.name: RunTest006
 * @tc.desc: test for Paint
 * @tc.type: FUNC
 */
HWTEST_F(RunTest, RunTest006, TestSize.Level0)
{
    EXPECT_EQ(runs_.size(), 1);
    ASSERT_NE(runs_.at(0), nullptr);
    Canvas canvas;
    runs_.at(0)->Paint(&canvas, 0.0, 0.0);
    runs_.at(0)->Paint(nullptr, 0.0, 0.0);
}

/*
 * @tc.name: RunTest007
 * @tc.desc: branch coverage
 * @tc.type: FUNC
 */
HWTEST_F(RunTest, RunTest007, TestSize.Level0)
{
    EXPECT_EQ(runs_[0]->GetGlyphs(0, 12).size(), 7);
    EXPECT_EQ(runs_[0]->GetPositions(0, 10).size(), 7);

    uint64_t location = 0;
    uint64_t length = 0;
    runs_[0]->GetStringRange(&location, &length);
    EXPECT_EQ(location, 0);
    EXPECT_EQ(length, 7);

    EXPECT_EQ(runs_[0]->GetStringIndices(0, 10).size(), 7);
    EXPECT_EQ(runs_[0]->GetImageBounds().GetLeft(), 1.000000f);
}

/*
 * @tc.name: RunTest009
 * @tc.desc: branch coverage
 * @tc.type: FUNC
 */
HWTEST_F(RunTest, RunTest008, TestSize.Level0)
{
    EXPECT_EQ(runs_[0]->GetGlyphs(-1, 12), std::vector<uint16_t>());
    EXPECT_EQ(runs_[0]->GetGlyphs(0, -1), std::vector<uint16_t>());
    EXPECT_EQ(runs_[0]->GetPositions(-1, 10), std::vector<Drawing::Point>());
    EXPECT_EQ(runs_[0]->GetPositions(0, -1), std::vector<Drawing::Point>());

    uint64_t location = 0;
    uint64_t length = 0;
    runs_[0]->GetStringRange(nullptr, &length);
    EXPECT_EQ(length, 0);

    runs_[0]->GetStringRange(&location, nullptr);
    EXPECT_EQ(location, 0);

    std::vector<PaintRecord> testVec;
    std::unique_ptr<SPText::Run> runNull = std::make_unique<SPText::RunImpl>(nullptr, testVec);
    location = 10;
    length = 10;
    runNull->GetStringRange(&location, &length);
    EXPECT_EQ(location, 0);
    EXPECT_EQ(length, 0);
    EXPECT_EQ(runNull->GetImageBounds().GetLeft(), 0.000000f);

    EXPECT_EQ(runs_[0]->GetStringIndices(-1, 10), std::vector<uint64_t>());
    EXPECT_EQ(runs_[0]->GetStringIndices(0, -1), std::vector<uint64_t>());
}

/*
 * @tc.name: RunGlyphDrawingTest001
 * @tc.desc: Test for the glyph drawing of the English text.
 * @tc.type: FUNC
 */
HWTEST_F(RunTest, RunGlyphDrawingTest001, TestSize.Level0)
{
    text_ = u"Hello你好";
    PrepareCreateRunForGlyphDrawing();
    EXPECT_EQ(runs_[0]->GetTextDirection(), OHOS::Rosen::SPText::TextDirection::LTR);
    std::vector<Drawing::Point> pointResult = runs_[0]->GetAdvances(0, 0);
    EXPECT_EQ(pointResult.size(), 5);
    EXPECT_NEAR(pointResult[0].GetX(), 74.4999237, FLOAT_DATA_EPSILON);
    pointResult = runs_[0]->GetAdvances(0, 1);
    EXPECT_EQ(pointResult.size(), 1);
}

/*
 * @tc.name: RunGlyphDrawingTest002
 * @tc.desc: Test for the glyph drawing of the RTL text.
 * @tc.type: FUNC
 */
HWTEST_F(RunTest, RunGlyphDrawingTest002, TestSize.Level0)
{
    text_ = u"مرحبا";
    PrepareCreateRunForGlyphDrawing();
    EXPECT_EQ(runs_[0]->GetTextDirection(), OHOS::Rosen::SPText::TextDirection::RTL);
    std::vector<Drawing::Point> pointResult = runs_[0]->GetAdvances(0, 0);
    EXPECT_EQ(pointResult.size(), 5);
    EXPECT_NEAR(pointResult[0].GetX(), 27.3999786, FLOAT_DATA_EPSILON);
    pointResult = runs_[0]->GetAdvances(0, 1);
    EXPECT_EQ(pointResult.size(), 1);
}

/*
 * @tc.name: RunGlyphDrawingTest003
 * @tc.desc: test for nullptr, only for the branch coverage
 * @tc.type: FUNC
 */
HWTEST_F(RunTest, RunGlyphDrawingTest003, TestSize.Level0)
{
    std::vector<PaintRecord> testVec;
    std::unique_ptr<SPText::Run> runNull = std::make_unique<SPText::RunImpl>(nullptr, testVec);
    EXPECT_EQ(runNull->GetTextDirection(), OHOS::Rosen::SPText::TextDirection::LTR);
    EXPECT_EQ(runNull->GetAdvances(0, 0), std::vector<Drawing::Point>());
}

/*
 * @tc.name: RunGetTextStyleTest001
 * @tc.desc: test for nullptr, only for get text style
 * @tc.type: FUNC
 */
HWTEST_F(RunTest, RunGetTextStyleTest001, TestSize.Level0)
{
    std::vector<PaintRecord> testVec;
    std::unique_ptr<SPText::Run> runNull = std::make_unique<SPText::RunImpl>(nullptr, testVec);
    EXPECT_DOUBLE_EQ(runNull->GetTextStyle().fontSize, 14.0); // 14.0: the default fontsize
}

/*
 * @tc.name: RunCompressHeadPunctuation001
 * @tc.desc: head punctuation advance shrinks under compression (fGlyphAdvances)
 * @tc.type: FUNC
 */
HWTEST_F(RunTest, RunCompressHeadPunctuation001, TestSize.Level0)
{
    // Four common-script brackets keep them in one run; only the line head is compressed.
    text_ = u"（《「『";
    PrepareRunForHeadPunctuationCompression(false);
    ASSERT_NE(runs_.size(), 0);
    auto baselineAdv = runs_[0]->GetAdvances(0, 1);
    ASSERT_EQ(baselineAdv.size(), 1);

    PrepareRunForHeadPunctuationCompression(true);
    ASSERT_NE(runs_.size(), 0);
    auto compressedAdv = runs_[0]->GetAdvances(0, 1);
    ASSERT_EQ(compressedAdv.size(), 1);

    // ss08 substitutes a half-width variant, so the head bracket advance must strictly shrink.
    EXPECT_LT(compressedAdv[0].GetX(), baselineAdv[0].GetX());
}

/*
 * @tc.name: RunCompressHeadPunctuation002
 * @tc.desc: glyph count and string indices are preserved (1:1 replacement, fGlyphs/fClusterIndexes)
 * @tc.type: FUNC
 */
HWTEST_F(RunTest, RunCompressHeadPunctuation002, TestSize.Level0)
{
    text_ = u"（《「『";
    PrepareRunForHeadPunctuationCompression(false);
    ASSERT_NE(runs_.size(), 0);
    auto baselineGlyphs = runs_[0]->GetGlyphs();
    auto baselineIndices = runs_[0]->GetStringIndices(0, baselineGlyphs.size());
    size_t baselineRunCount = runs_.size();

    PrepareRunForHeadPunctuationCompression(true);
    ASSERT_EQ(runs_.size(), baselineRunCount);
    auto compressedGlyphs = runs_[0]->GetGlyphs();
    // No split: the run keeps the same glyph count.
    EXPECT_EQ(compressedGlyphs.size(), baselineGlyphs.size());
    // replaceCompressedGlyphs refills new glyphs with the original cluster index.
    EXPECT_EQ(runs_[0]->GetStringIndices(0, compressedGlyphs.size()), baselineIndices);
}

/*
 * @tc.name: RunCompressHeadPunctuation003
 * @tc.desc: positions after the head glyph cascade left by a constant delta (fPositions)
 * @tc.type: FUNC
 */
HWTEST_F(RunTest, RunCompressHeadPunctuation003, TestSize.Level0)
{
    text_ = u"（《「『";
    PrepareRunForHeadPunctuationCompression(false);
    ASSERT_NE(runs_.size(), 0);
    auto baselinePos = runs_[0]->GetPositions();
    float baselineHeadAdv = runs_[0]->GetAdvances(0, 1)[0].GetX();
    ASSERT_GE(baselinePos.size(), 2);

    PrepareRunForHeadPunctuationCompression(true);
    ASSERT_NE(runs_.size(), 0);
    auto compressedPos = runs_[0]->GetPositions();
    float compressedHeadAdv = runs_[0]->GetAdvances(0, 1)[0].GetX();

    EXPECT_EQ(compressedPos.size(), baselinePos.size());
    // Run start position is untouched.
    EXPECT_EQ(compressedPos[0].GetX(), baselinePos[0].GetX());
    float delta = baselineHeadAdv - compressedHeadAdv;
    EXPECT_GT(delta, 0.0f);
    // Every subsequent position shifts left by exactly the head advance reduction.
    for (size_t i = 1; i < baselinePos.size(); ++i) {
        EXPECT_NEAR(compressedPos[i].GetX(), baselinePos[i].GetX() - delta, FLOAT_DATA_EPSILON)
            << "position cascade mismatch at index " << i;
    }
}

/*
 * @tc.name: RunCompressHeadPunctuation004
 * @tc.desc: offsets of the untouched tail glyphs stay unchanged (fOffsets)
 * @tc.type: FUNC
 */
HWTEST_F(RunTest, RunCompressHeadPunctuation004, TestSize.Level0)
{
    text_ = u"（《「『";
    PrepareRunForHeadPunctuationCompression(false);
    ASSERT_NE(runs_.size(), 0);
    auto baselineOffsets = runs_[0]->GetOffsets();
    ASSERT_GE(baselineOffsets.size(), 2);

    PrepareRunForHeadPunctuationCompression(true);
    ASSERT_NE(runs_.size(), 0);
    auto compressedOffsets = runs_[0]->GetOffsets();

    EXPECT_EQ(compressedOffsets.size(), baselineOffsets.size());
    // Only the replaced head glyph's offset is rewritten; the tail is structurally shifted, not modified.
    for (size_t i = 1; i < baselineOffsets.size(); ++i) {
        EXPECT_NEAR(compressedOffsets[i].GetX(), baselineOffsets[i].GetX(), FLOAT_DATA_EPSILON)
            << "offset changed at untouched index " << i;
        EXPECT_NEAR(compressedOffsets[i].GetY(), baselineOffsets[i].GetY(), FLOAT_DATA_EPSILON)
            << "offset changed at untouched index " << i;
    }
}

/*
 * @tc.name: RunCompressHeadPunctuation005
 * @tc.desc: non-compressible line head is unchanged between the two modes (negative path)
 * @tc.type: FUNC
 */
HWTEST_F(RunTest, RunCompressHeadPunctuation005, TestSize.Level0)
{
    // '你' is not in the compressible set, so replaceCompressedGlyphs must not run at all.
    text_ = u"你好（《";
    PrepareRunForHeadPunctuationCompression(false);
    ASSERT_NE(runs_.size(), 0);
    auto baselineGlyphs = runs_[0]->GetGlyphs();
    float baselineHeadAdv = runs_[0]->GetAdvances(0, 1)[0].GetX();
    size_t baselineRunCount = runs_.size();

    PrepareRunForHeadPunctuationCompression(true);
    ASSERT_EQ(runs_.size(), baselineRunCount);
    EXPECT_EQ(runs_[0]->GetGlyphs(), baselineGlyphs);
    EXPECT_NEAR(runs_[0]->GetAdvances(0, 1)[0].GetX(), baselineHeadAdv, FLOAT_DATA_EPSILON);
}
} // namespace txt