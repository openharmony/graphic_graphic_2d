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
#include "text_line_impl.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen;
using namespace OHOS::Rosen::Drawing;
using namespace OHOS::Rosen::SPText;

namespace txt {
class TextLineBaseTest : public testing::Test {
protected:
    void SetUp() override;
    void TearDown() override;

private:
    // 50 is the width of the layout, just for test
    int layoutWidth_ = 50;
    // 100 is the max lines of the paragraph, just for test
    int maxLines_ = 100;

    std::unique_ptr<Paragraph> paragraph_;
    std::vector<std::unique_ptr<SPText::TextLineBase>> textLine_;
};

void TextLineBaseTest::SetUp()
{
    ParagraphStyle paragraphStyle;
    paragraphStyle.maxLines = maxLines_;
    std::shared_ptr<FontCollection> fontCollection = std::make_shared<FontCollection>();
    ASSERT_NE(fontCollection, nullptr);
    fontCollection->SetupDefaultFontManager();
    std::shared_ptr<ParagraphBuilder> paragraphBuilder = ParagraphBuilder::Create(paragraphStyle, fontCollection);
    ASSERT_NE(paragraphBuilder, nullptr);
    paragraphBuilder->AddText(u"Hello World!");
    paragraph_ = paragraphBuilder->Build();
    ASSERT_NE(paragraph_, nullptr);
    paragraph_->Layout(layoutWidth_);
    textLine_ = paragraph_->GetTextLines();
}

void TextLineBaseTest::TearDown()
{
    paragraph_.reset();
    textLine_.clear();
}

/*
 * @tc.name: TextLineBaseTest001
 * @tc.desc: test for GetGlyphCount and GetGlyphRuns
 * @tc.type: FUNC
 */
HWTEST_F(TextLineBaseTest, TextLineBaseTest001, TestSize.Level1)
{
    EXPECT_EQ(textLine_.size(), 2);
    ASSERT_NE(textLine_.at(0), nullptr);
    EXPECT_EQ(textLine_.at(0)->GetGlyphCount(), textLine_.at(0)->GetGlyphRuns().at(0)->GetGlyphs().size());
    EXPECT_EQ(textLine_.at(0)->GetGlyphCount(), 5);
}

/*
 * @tc.name: TextLineBaseTest002
 * @tc.desc: test for GetTextRange
 * @tc.type: FUNC
 */
HWTEST_F(TextLineBaseTest, TextLineBaseTest002, TestSize.Level1)
{
    EXPECT_EQ(textLine_.size(), 2);
    ASSERT_NE(textLine_.at(0), nullptr);
    EXPECT_EQ(textLine_.at(0)->GetTextRange().start, 0);
    EXPECT_EQ(textLine_.at(0)->GetTextRange().end, 6);
}

/*
 * @tc.name: TextLineBaseTest003
 * @tc.desc: test for Paint
 * @tc.type: FUNC
 */
HWTEST_F(TextLineBaseTest, TextLineBaseTest003, TestSize.Level1)
{
    EXPECT_EQ(textLine_.size(), 2);
    ASSERT_NE(textLine_.at(0), nullptr);
    Canvas canvas;
    textLine_.at(0)->Paint(&canvas, 0.0, 0.0);
}

/*
 * @tc.name: TextLineBaseTest004
 * @tc.desc: test for nullptr, only for the branch coverage
 * @tc.type: FUNC
 */
HWTEST_F(TextLineBaseTest, TextLineBaseTest004, TestSize.Level1)
{
    std::vector<PaintRecord> paints;
    std::unique_ptr<skia::textlayout::TextLineBase> textLineBase = nullptr;
    auto textLineImpl = std::make_unique<SPText::TextLineImpl>(std::move(textLineBase), paints);

    textLineImpl->Paint(nullptr, 0.0, 0.0);
    EXPECT_EQ(textLineImpl->GetGlyphCount(), 0);
    EXPECT_EQ(textLineImpl->GetGlyphRuns().size(), 0);
    EXPECT_EQ(textLineImpl->GetTextRange().start, 0);
    EXPECT_EQ(textLineImpl->GetTextRange().end, 0);
}
} // namespace txt