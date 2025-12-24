/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include <cstddef>

#include "gtest/gtest.h"
#include "font_collection.h"
#include "modules/skparagraph/src/ParagraphImpl.h"
#include "ohos/init_data.h"
#include "paragraph_builder.h"
#include "paragraph_impl.h"
#include "paragraph_style.h"
#include "text_style.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen;
using namespace OHOS::Rosen::Drawing;
using namespace OHOS::Rosen::SPText;

namespace txt {
namespace skt = skia::textlayout;
class ParagraphPathTest : public testing::Test {
public:
    void SetUp() override;
    void TearDown() override;

    static skia::textlayout::ParagraphImpl* GetParagraphSkiaImpl(const std::shared_ptr<Paragraph>& paragraph);
    static ParagraphImpl* GetParagraphImpl(const std::shared_ptr<Paragraph>& paragraph);
    void PrepareMiddleEllipsis(size_t& maxLines, const std::u16string& str, const std::u16string& text);

protected:
    std::shared_ptr<Paragraph> paragraph_;
    std::shared_ptr<Paragraph> paragraphMiddleEllipsis_;
    int layoutWidth_ = 50;
    std::u16string text_ = u"text";
};

void ParagraphPathTest::SetUp()
{
    SetHwIcuDirectory();
    ParagraphStyle paragraphStyle;
    std::shared_ptr<FontCollection> fontCollection = std::make_shared<FontCollection>();
    ASSERT_NE(fontCollection, nullptr);
    fontCollection->SetupDefaultFontManager();
    std::shared_ptr<ParagraphBuilder> paragraphBuilder = ParagraphBuilder::Create(paragraphStyle, fontCollection);
    ASSERT_NE(paragraphBuilder, nullptr);
    paragraphBuilder->AddText(text_);
    // 50 just for test
    PlaceholderRun placeholderRun(50, 50, PlaceholderAlignment::BASELINE, SPText::TextBaseline::ALPHABETIC, 0.0);
    paragraphBuilder->AddPlaceholder(placeholderRun);
    paragraph_ = paragraphBuilder->Build();
    ASSERT_NE(paragraph_, nullptr);
    // 50 just for test
    paragraph_->Layout(layoutWidth_);
    paragraph_->MeasureText();
}

void ParagraphPathTest::TearDown()
{
    paragraph_.reset();
}

ParagraphImpl* ParagraphPathTest::GetParagraphImpl(const std::shared_ptr<Paragraph>& paragraph)
{
    return static_cast<ParagraphImpl*>(paragraph.get());
}

skia::textlayout::ParagraphImpl* ParagraphPathTest::GetParagraphSkiaImpl(const std::shared_ptr<Paragraph>& paragraph)
{
    return static_cast<skia::textlayout::ParagraphImpl*>(GetParagraphImpl(paragraph)->paragraph_.get());
}

/*
 * @tc.name: ParagraphPathTestgetTextPathByClusterRange001
 * @tc.desc: test for get text path by glyph range
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphPathTest, ParagraphPathTestgetTextPathByClusterRange001, TestSize.Level0)
{
    ParagraphStyle paragraphStyle;
    auto fontCollection = std::make_shared<FontCollection>();
    ASSERT_NE(fontCollection, nullptr);
    fontCollection->SetupDefaultFontManager();
    std::unique_ptr<ParagraphBuilder> paragraphBuilder = ParagraphBuilder::Create(paragraphStyle, fontCollection);
    ASSERT_NE(paragraphBuilder, nullptr);
    OHOS::Rosen::SPText::TextStyle style;
    style.fontSize = 15.0;
    paragraphBuilder->PushStyle(style);
    paragraphBuilder->AddText(u"012");
    style.fontSize = 20.0;
    paragraphBuilder->PushStyle(style);
    paragraphBuilder->AddText(u"34567");
    style.fontSize = 21.5;
    paragraphBuilder->PushStyle(style);
    paragraphBuilder->AddText(u"89:");
    std::shared_ptr<Paragraph> paragraph = paragraphBuilder->Build();
    OHOS::Rosen::SPText::ParagraphImpl* paragraphImpl = GetParagraphImpl(paragraph);
    ASSERT_NE(paragraphImpl, nullptr);
    std::unique_ptr<skt::Paragraph> skParagraph = nullptr;
    paragraphImpl->paragraph_.swap(skParagraph);
    ASSERT_NE(skParagraph, nullptr);
    skParagraph->layout(200);
    skt::SkRange<skt::TextIndex> range{0, 11};
    std::vector<skt::PathInfo> paths = skParagraph->getTextPathByClusterRange(range);
    EXPECT_EQ(paths.size(), 11);
    EXPECT_EQ(paths[0].textStyle.getFontSize(), 15.0);
    EXPECT_EQ(paths[2].textStyle.getFontSize(), 15.0);
    EXPECT_EQ(paths[3].textStyle.getFontSize(), 20.0);
    EXPECT_EQ(paths[8].textStyle.getFontSize(), 21.5);
}

/*
 * @tc.name: ParagraphPathTestgetTextPathByClusterRange002
 * @tc.desc: test for get text path by glyph range
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphPathTest, ParagraphPathTestgetTextPathByClusterRange002, TestSize.Level0)
{
    ParagraphStyle paragraphStyle;
    auto fontCollection = std::make_shared<FontCollection>();
    ASSERT_NE(fontCollection, nullptr);
    fontCollection->SetupDefaultFontManager();
    std::unique_ptr<ParagraphBuilder> paragraphBuilder = ParagraphBuilder::Create(paragraphStyle, fontCollection);
    ASSERT_NE(paragraphBuilder, nullptr);
    OHOS::Rosen::SPText::TextStyle style;
    style.fontSize = 15.0;
    paragraphBuilder->PushStyle(style);
    paragraphBuilder->AddText(u"你好 ");
    style.fontSize = 20.0;
    paragraphBuilder->PushStyle(style);
    paragraphBuilder->AddText(u"Hello ");
    style.fontSize = 21.5;
    paragraphBuilder->PushStyle(style);
    paragraphBuilder->AddText(u"World");
    std::shared_ptr<Paragraph> paragraph = paragraphBuilder->Build();
    OHOS::Rosen::SPText::ParagraphImpl* paragraphImpl = GetParagraphImpl(paragraph);
    ASSERT_NE(paragraphImpl, nullptr);
    std::unique_ptr<skt::Paragraph> skParagraph = nullptr;
    paragraphImpl->paragraph_.swap(skParagraph);
    ASSERT_NE(skParagraph, nullptr);
    skParagraph->layout(200);
    skt::SkRange<skt::TextIndex> range{0, 20};
    std::vector<skt::PathInfo> paths = skParagraph->getTextPathByClusterRange(range);
    EXPECT_EQ(paths.size(), 14);
    EXPECT_EQ(paths[0].textStyle.getFontSize(), 15.0);
    EXPECT_EQ(paths[2].textStyle.getFontSize(), 15.0);
    EXPECT_EQ(paths[3].textStyle.getFontSize(), 20.0);
    EXPECT_EQ(paths[10].textStyle.getFontSize(), 21.5);
}
} // namespace txt