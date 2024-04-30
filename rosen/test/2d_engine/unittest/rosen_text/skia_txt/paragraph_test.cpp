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

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen;
using namespace OHOS::Rosen::Drawing;
using namespace OHOS::Rosen::SPText;

namespace txt {
class ParagraphTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    static bool AnimationFunc(const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig);
    static inline std::shared_ptr<Paragraph> paragraph_ = nullptr;
};

void ParagraphTest::SetUpTestCase()
{
    ParagraphStyle paragraphStyle;
    std::shared_ptr<FontCollection> fontCollection = std::make_shared<FontCollection>();
    if (!fontCollection) {
        std::cout << "ParagraphTest::SetUpTestCase error fontCollection is nullptr" << std::endl;
        return;
    }
    fontCollection->SetupDefaultFontManager();
    std::shared_ptr<ParagraphBuilder> paragraphBuilder = ParagraphBuilder::Create(paragraphStyle, fontCollection);
    if (!paragraphBuilder) {
        std::cout << "ParagraphTest::SetUpTestCase error paragraphBuilder is nullptr" << std::endl;
        return;
    }
    std::u16string text(u"text");
    paragraphBuilder->AddText(text);
    // 50 just for test
    PlaceholderRun placeholderRun(50, 50, PlaceholderAlignment::BASELINE,
        SPText::TextBaseline::ALPHABETIC, 0.0);
    paragraphBuilder->AddPlaceholder(placeholderRun);
    paragraph_ = paragraphBuilder->Build();
    if (!paragraph_) {
        std::cout << "ParagraphTest::SetUpTestCase error paragraph_ is nullptr" << std::endl;
        return;
    }
    // 50 just for test
    paragraph_->Layout(50);
}

void ParagraphTest::TearDownTestCase()
{
}

bool ParagraphTest::AnimationFunc(const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig)
{
    return true;
}

/*
 * @tc.name: ParagraphTest001
 * @tc.desc: test for GetMaxWidth
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTest001, TestSize.Level1)
{
    EXPECT_EQ(paragraph_ != nullptr, true);
    EXPECT_EQ(paragraph_->GetMaxWidth() > 0, true);
}

/*
 * @tc.name: ParagraphTest002
 * @tc.desc: test for GetHeight
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTest002, TestSize.Level1)
{
    EXPECT_EQ(paragraph_ != nullptr, true);
    EXPECT_EQ(paragraph_->GetHeight() > 0, true);
}

/*
 * @tc.name: ParagraphTest003
 * @tc.desc: test for GetLongestLine
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTest003, TestSize.Level1)
{
    EXPECT_EQ(paragraph_ != nullptr, true);
    EXPECT_EQ(paragraph_->GetLongestLine() > 0, true);
}

/*
 * @tc.name: ParagraphTest004
 * @tc.desc: test for GetMinIntrinsicWidth
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTest004, TestSize.Level1)
{
    EXPECT_EQ(paragraph_ != nullptr, true);
    EXPECT_EQ(paragraph_->GetMinIntrinsicWidth() > 0, true);
}

/*
 * @tc.name: ParagraphTest005
 * @tc.desc: test for GetMaxIntrinsicWidth
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTest005, TestSize.Level1)
{
    EXPECT_EQ(paragraph_ != nullptr, true);
    EXPECT_EQ(paragraph_->GetMaxIntrinsicWidth() > 0, true);
}

/*
 * @tc.name: ParagraphTest006
 * @tc.desc: test for GetAlphabeticBaseline
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTest006, TestSize.Level1)
{
    EXPECT_EQ(paragraph_ != nullptr, true);
    EXPECT_EQ(paragraph_->GetAlphabeticBaseline() > 0, true);
}

/*
 * @tc.name: ParagraphTest007
 * @tc.desc: test for GetIdeographicBaseline
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTest007, TestSize.Level1)
{
    EXPECT_EQ(paragraph_ != nullptr, true);
    EXPECT_EQ(paragraph_->GetIdeographicBaseline() > 0, true);
}

/*
 * @tc.name: ParagraphTest008
 * @tc.desc: test for GetGlyphsBoundsTop
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTest008, TestSize.Level1)
{
    EXPECT_EQ(paragraph_ != nullptr, true);
    EXPECT_EQ(paragraph_->GetGlyphsBoundsTop() > 0, true);
}

/*
 * @tc.name: ParagraphTest009
 * @tc.desc: test for GetGlyphsBoundsBottom
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTest009, TestSize.Level1)
{
    EXPECT_EQ(paragraph_ != nullptr, true);
    paragraph_->GetGlyphsBoundsBottom();
}

/*
 * @tc.name: ParagraphTest010
 * @tc.desc: test for GetGlyphsBoundsLeft
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTest010, TestSize.Level1)
{
    EXPECT_EQ(paragraph_ != nullptr, true);
    paragraph_->GetGlyphsBoundsLeft();
}

/*
 * @tc.name: ParagraphTest011
 * @tc.desc: test for GetGlyphsBoundsRight
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTest011, TestSize.Level1)
{
    EXPECT_EQ(paragraph_ != nullptr, true);
    EXPECT_EQ(paragraph_->GetGlyphsBoundsRight() > 0, true);
}

/*
 * @tc.name: ParagraphTest012
 * @tc.desc: test for DidExceedMaxLines
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTest012, TestSize.Level1)
{
    EXPECT_EQ(paragraph_ != nullptr, true);
    EXPECT_EQ(paragraph_->DidExceedMaxLines(), true);
}

/*
 * @tc.name: ParagraphTest013
 * @tc.desc: test for GetLineCount
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTest013, TestSize.Level1)
{
    EXPECT_EQ(paragraph_ != nullptr, true);
    EXPECT_EQ(paragraph_->GetLineCount() > 0, true);
}

/*
 * @tc.name: ParagraphTest014
 * @tc.desc: test for SetIndents
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTest014, TestSize.Level1)
{
    EXPECT_EQ(paragraph_ != nullptr, true);
    std::vector<float> indents;
    // 0.5 just for test
    indents.emplace_back(0.5);
    paragraph_->SetIndents(indents);
}

/*
 * @tc.name: ParagraphTest015
 * @tc.desc: test for DetectIndents
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTest015, TestSize.Level1)
{
    EXPECT_EQ(paragraph_ != nullptr, true);
    EXPECT_EQ(paragraph_->DetectIndents(0) > 0, true);
}

/*
 * @tc.name: ParagraphTest016
 * @tc.desc: test for MarkDirty
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTest016, TestSize.Level1)
{
    EXPECT_EQ(paragraph_ != nullptr, true);
    paragraph_->MarkDirty();
}

/*
 * @tc.name: ParagraphTest017
 * @tc.desc: test for GetUnresolvedGlyphsCount
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTest017, TestSize.Level1)
{
    EXPECT_EQ(paragraph_ != nullptr, true);
    // 50 just for test
    paragraph_->Layout(50);
    EXPECT_EQ(paragraph_->GetUnresolvedGlyphsCount(), 0);
}

/*
 * @tc.name: ParagraphTest018
 * @tc.desc: test for UpdateFontSize
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTest018, TestSize.Level1)
{
    EXPECT_EQ(paragraph_ != nullptr, true);
    // 2 24.0 just for test
    paragraph_->UpdateFontSize(0, 2, 24.0);
}

/*
 * @tc.name: ParagraphTest019
 * @tc.desc: test for Paint
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTest019, TestSize.Level1)
{
    EXPECT_EQ(paragraph_ != nullptr, true);
    SkCanvas canvas;
    paragraph_->Paint(&canvas, 0.0, 0.0);
}

/*
 * @tc.name: ParagraphTest020
 * @tc.desc: test for Paint
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTest020, TestSize.Level1)
{
    EXPECT_EQ(paragraph_ != nullptr, true);
    Canvas canvas;
    paragraph_->Paint(&canvas, 0.0, 0.0);
}

/*
 * @tc.name: ParagraphTest021
 * @tc.desc: test for GetRectsForRange
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTest021, TestSize.Level1)
{
    EXPECT_EQ(paragraph_ != nullptr, true);
    // 2 just for test
    EXPECT_EQ(paragraph_->GetRectsForRange(0, 2, RectHeightStyle::MAX, RectWidthStyle::TIGHT).size(), 0);
}

/*
 * @tc.name: ParagraphTest022
 * @tc.desc: test for GetRectsForPlaceholders
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTest022, TestSize.Level1)
{
    EXPECT_EQ(paragraph_ != nullptr, true);
    // 2 just for test
    EXPECT_EQ(paragraph_->GetRectsForPlaceholders().size(), 0);
}

/*
 * @tc.name: ParagraphTest023
 * @tc.desc: test for GetGlyphPositionAtCoordinate
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTest023, TestSize.Level1)
{
    EXPECT_EQ(paragraph_ != nullptr, true);
    paragraph_->GetGlyphPositionAtCoordinate(0.0, 0.0);
}

/*
 * @tc.name: ParagraphTest024
 * @tc.desc: test for GetWordBoundary
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTest024, TestSize.Level1)
{
    EXPECT_EQ(paragraph_ != nullptr, true);
    EXPECT_EQ(paragraph_->GetWordBoundary(0).start, 0);
}

/*
 * @tc.name: ParagraphTest025
 * @tc.desc: test for GetActualTextRange
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTest025, TestSize.Level1)
{
    EXPECT_EQ(paragraph_ != nullptr, true);
    EXPECT_EQ(paragraph_->GetActualTextRange(0, false).start, 0);
}

/*
 * @tc.name: ParagraphTest026
 * @tc.desc: test for GetLineMetrics
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTest026, TestSize.Level1)
{
    EXPECT_EQ(paragraph_ != nullptr, true);
    EXPECT_EQ(paragraph_->GetLineMetrics().size() > 0, true);
}

/*
 * @tc.name: ParagraphTest027
 * @tc.desc: test for GetLineMetricsAt
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTest027, TestSize.Level1)
{
    EXPECT_EQ(paragraph_ != nullptr, true);
    skia::textlayout::LineMetrics lineMetrics;
    EXPECT_EQ(paragraph_->GetLineMetricsAt(0, &lineMetrics), true);
}

/*
 * @tc.name: ParagraphTest028
 * @tc.desc: test for SetAnimation
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTest028, TestSize.Level1)
{
    EXPECT_EQ(paragraph_ != nullptr, true);
    std::function<bool(const std::shared_ptr<TextEngine::SymbolAnimationConfig>&)> animationFunc =
        ParagraphTest::AnimationFunc;
    paragraph_->SetAnimation(animationFunc);
}

/*
 * @tc.name: ParagraphTest029
 * @tc.desc: test for SetParagraghId
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTest029, TestSize.Level1)
{
    EXPECT_EQ(paragraph_ != nullptr, true);
    paragraph_->SetParagraghId(0);
}

/*
 * @tc.name: ParagraphTest030
 * @tc.desc: test for MeasureText
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTest030, TestSize.Level1)
{
    EXPECT_EQ(paragraph_ != nullptr, true);
    EXPECT_EQ(paragraph_->MeasureText().fFlags != 0, true);
}

/*
 * @tc.name: ParagraphTest031
 * @tc.desc: test for GetFontMetricsResult
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTest031, TestSize.Level1)
{
    EXPECT_EQ(paragraph_ != nullptr, true);
    SPText::TextStyle textStyle;
    EXPECT_EQ(paragraph_->GetFontMetricsResult(textStyle).fFlags != 0, true);
}

/*
 * @tc.name: ParagraphTest032
 * @tc.desc: test for GetLineFontMetrics
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTest032, TestSize.Level1)
{
    EXPECT_EQ(paragraph_ != nullptr, true);
    size_t charNumber = 1;
    std::vector<Drawing::FontMetrics> fontMetrics;
    EXPECT_EQ(paragraph_->GetLineFontMetrics(1, charNumber, fontMetrics), true);
}

/*
 * @tc.name: ParagraphTest033
 * @tc.desc: test for GetTextLines
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTest033, TestSize.Level1)
{
    EXPECT_EQ(paragraph_ != nullptr, true);
    EXPECT_EQ(paragraph_->GetTextLines().size() > 0, true);
}

/*
 * @tc.name: ParagraphTest034
 * @tc.desc: test for CloneSelf
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTest034, TestSize.Level1)
{
    EXPECT_EQ(paragraph_ != nullptr, true);
    EXPECT_EQ(paragraph_->CloneSelf() != nullptr, true);
}
} // namespace txt