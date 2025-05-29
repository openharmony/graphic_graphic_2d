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

private:
    // 50 is the width of the layout, just for test
    int layoutWidth_ = 50;
    int fontSize_ = 100;
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
    paragraph_->Layout(layoutWidth_);
    auto textLineBases = paragraph_->GetTextLines();
    runs_ = textLineBases[0]->GetGlyphRuns();
}

/*
 * @tc.name: RunTest001
 * @tc.desc: test for GetFont
 * @tc.type: FUNC
 */
HWTEST_F(RunTest, RunTest001, TestSize.Level1)
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
HWTEST_F(RunTest, RunTest002, TestSize.Level1)
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
HWTEST_F(RunTest, RunTest003, TestSize.Level1)
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
HWTEST_F(RunTest, RunTest004, TestSize.Level1)
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
HWTEST_F(RunTest, RunTest005, TestSize.Level1)
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
HWTEST_F(RunTest, RunTest006, TestSize.Level1)
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
HWTEST_F(RunTest, RunTest007, TestSize.Level1)
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
HWTEST_F(RunTest, RunTest008, TestSize.Level1)
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
HWTEST_F(RunTest, RunGlyphDrawingTest001, TestSize.Level1)
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
HWTEST_F(RunTest, RunGlyphDrawingTest002, TestSize.Level1)
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
HWTEST_F(RunTest, RunGlyphDrawingTest003, TestSize.Level1)
{
    std::vector<PaintRecord> testVec;
    std::unique_ptr<SPText::Run> runNull = std::make_unique<SPText::RunImpl>(nullptr, testVec);
    EXPECT_EQ(runNull->GetTextDirection(), OHOS::Rosen::SPText::TextDirection::LTR);
    EXPECT_EQ(runNull->GetAdvances(0, 0), std::vector<Drawing::Point>());
}
} // namespace txt