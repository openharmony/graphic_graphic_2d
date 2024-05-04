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
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    static inline std::shared_ptr<Paragraph> paragraph_ = nullptr;
    static inline std::vector<std::unique_ptr<SPText::TextLineBase>> textLineBases_;
    static inline std::unique_ptr<SPText::TextLineImpl> textLineImpl_ = nullptr;
};

void TextLineBaseTest::SetUpTestCase()
{
    ParagraphStyle paragraphStyle;
    std::shared_ptr<FontCollection> fontCollection = std::make_shared<FontCollection>();
    if (!fontCollection) {
        std::cout << "TextLineBaseTest::SetUpTestCase error fontCollection is nullptr" << std::endl;
        return;
    }
    fontCollection->SetupDefaultFontManager();
    std::shared_ptr<ParagraphBuilder> paragraphBuilder = ParagraphBuilder::Create(paragraphStyle, fontCollection);
    if (!paragraphBuilder) {
        std::cout << "TextLineBaseTest::SetUpTestCase error paragraphBuilder is nullptr" << std::endl;
        return;
    }
    std::u16string text(u"TextLineBaseTest");
    paragraphBuilder->AddText(text);
    paragraph_ = paragraphBuilder->Build();
    if (!paragraph_) {
        std::cout << "TextLineBaseTest::SetUpTestCase error paragraph_ is nullptr" << std::endl;
        return;
    }
    // 50 just for test
    paragraph_->Layout(50);
    Canvas canvas;
    paragraph_->Paint(&canvas, 0.0, 0.0);
    textLineBases_ = paragraph_->GetTextLines();
    if (!textLineBases_.size() || !textLineBases_.at(0)) {
        std::cout << "TextLineBaseTest::SetUpTestCase error textLineBases_ variable acquisition exception"
            << std::endl;
    }

    std::vector<PaintRecord> paints;
    std::unique_ptr<skia::textlayout::TextLineBase> textLineBase = nullptr;
    textLineImpl_ = std::make_unique<SPText::TextLineImpl>(std::move(textLineBase), paints);
    if (!textLineImpl_) {
        std::cout << "TextLineBaseTest::SetUpTestCase error textLineImpl_ variable acquisition exception" << std::endl;
    }
}

void TextLineBaseTest::TearDownTestCase()
{
}

/*
 * @tc.name: TextLineBaseTest001
 * @tc.desc: test for GetGlyphCount
 * @tc.type: FUNC
 */
HWTEST_F(TextLineBaseTest, TextLineBaseTest001, TestSize.Level1)
{
    EXPECT_EQ(paragraph_ != nullptr, true);
    EXPECT_EQ(textLineBases_.size() != 0, true);
    EXPECT_EQ(textLineBases_.at(0) != nullptr, true);
    EXPECT_EQ(textLineBases_.at(0)->GetGlyphCount() > 0, true);
}

/*
 * @tc.name: TextLineBaseTest002
 * @tc.desc: test for GetGlyphRuns
 * @tc.type: FUNC
 */
HWTEST_F(TextLineBaseTest, TextLineBaseTest002, TestSize.Level1)
{
    EXPECT_EQ(paragraph_ != nullptr, true);
    EXPECT_EQ(textLineBases_.size() != 0, true);
    EXPECT_EQ(textLineBases_.at(0) != nullptr, true);
    EXPECT_EQ(textLineBases_.at(0)->GetGlyphRuns().size() > 0, true);
}

/*
 * @tc.name: TextLineBaseTest003
 * @tc.desc: test for GetTextRange
 * @tc.type: FUNC
 */
HWTEST_F(TextLineBaseTest, TextLineBaseTest003, TestSize.Level1)
{
    EXPECT_EQ(paragraph_ != nullptr, true);
    EXPECT_EQ(textLineBases_.size() != 0, true);
    EXPECT_EQ(textLineBases_.at(0) != nullptr, true);
    SPText::Range<size_t> rangeDefault;
    SPText::Range<size_t> range(0, 1);
    EXPECT_EQ(rangeDefault == range, false);
    EXPECT_EQ(textLineBases_.at(0)->GetTextRange().start, 0);
}

/*
 * @tc.name: TextLineBaseTest004
 * @tc.desc: test for Paint
 * @tc.type: FUNC
 */
HWTEST_F(TextLineBaseTest, TextLineBaseTest004, TestSize.Level1)
{
    EXPECT_EQ(paragraph_ != nullptr, true);
    EXPECT_EQ(textLineBases_.size() != 0, true);
    EXPECT_EQ(textLineBases_.at(0) != nullptr, true);
    Canvas canvas;
    textLineBases_.at(0)->Paint(&canvas, 0.0, 0.0);
}

/*
 * @tc.name: TextLineBaseTest005
 * @tc.desc: test for GetGlyphCount
 * @tc.type: FUNC
 */
HWTEST_F(TextLineBaseTest, TextLineBaseTest005, TestSize.Level1)
{
    EXPECT_EQ(textLineImpl_ != nullptr, true);
    EXPECT_EQ(textLineImpl_->GetGlyphCount(), 0);
}

/*
 * @tc.name: TextLineBaseTest006
 * @tc.desc: test for GetGlyphRuns
 * @tc.type: FUNC
 */
HWTEST_F(TextLineBaseTest, TextLineBaseTest006, TestSize.Level1)
{
    EXPECT_EQ(textLineImpl_ != nullptr, true);
    EXPECT_EQ(textLineImpl_->GetGlyphRuns().size(), 0);
}

/*
 * @tc.name: TextLineBaseTest007
 * @tc.desc: test for GetTextRange
 * @tc.type: FUNC
 */
HWTEST_F(TextLineBaseTest, TextLineBaseTest007, TestSize.Level1)
{
    EXPECT_EQ(textLineImpl_ != nullptr, true);
    EXPECT_EQ(textLineImpl_->GetTextRange().start, 0);
}

/*
 * @tc.name: TextLineBaseTest008
 * @tc.desc: test for Paint
 * @tc.type: FUNC
 */
HWTEST_F(TextLineBaseTest, TextLineBaseTest008, TestSize.Level1)
{
    EXPECT_EQ(textLineImpl_ != nullptr, true);
    textLineImpl_->Paint(nullptr, 0.0, 0.0);
}
} // namespace txt