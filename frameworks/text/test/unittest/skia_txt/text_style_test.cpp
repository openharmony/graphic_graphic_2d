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
#include "include/TextStyle.h"
#include "modules/skparagraph/src/ParagraphImpl.h"
#include "paragraph_builder.h"
#include "paragraph_impl.h"
#include "text_style.h"
#include "text_types.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen;

namespace txt {
constexpr float LINE_HEIGHT = 100;
constexpr float FONT_SIZE = 100;

class TextStyleTest : public testing::Test {
public:
    void SetUp() override;
    void TearDown() override;

private:
    std::shared_ptr<SPText::TextStyle> textStyle_;
};

void TextStyleTest::SetUp()
{
    textStyle_ = std::make_shared<SPText::TextStyle>();
    ASSERT_NE(textStyle_, nullptr);
}

void TextStyleTest::TearDown() {}

/*
 * @tc.name: TextStyleTest001
 * @tc.desc: test for Equal
 * @tc.type: FUNC
 */
HWTEST_F(TextStyleTest, TextStyleTest001, TestSize.Level0)
{
    SPText::TextStyle textStyleCompare;
    EXPECT_EQ((*textStyle_), textStyleCompare);
}

/*
 * @tc.name: TextStyleTest002
 * @tc.desc: test for change fontFeatures
 * @tc.type: FUNC
 */
HWTEST_F(TextStyleTest, TextStyleTest002, TestSize.Level0)
{
    SPText::FontFeatures fontFeatures;
    fontFeatures.SetFeature("tag", 0);
    textStyle_->fontFeatures = fontFeatures;
    ASSERT_EQ(textStyle_->fontFeatures.GetFontFeatures().size(), 1);
    EXPECT_EQ(textStyle_->fontFeatures.GetFontFeatures().at(0).first, fontFeatures.GetFontFeatures().at(0).first);
    EXPECT_EQ(textStyle_->fontFeatures.GetFontFeatures().at(0).second, fontFeatures.GetFontFeatures().at(0).second);
    EXPECT_EQ(textStyle_->fontFeatures.GetFeatureSettings(), "tag=0");
}

/*
 * @tc.name: TextStyleTest003
 * @tc.desc: test for change fontVariations
 * @tc.type: FUNC
 */
HWTEST_F(TextStyleTest, TextStyleTest003, TestSize.Level0)
{
    SPText::FontVariations fontVariations;
    fontVariations.SetAxisValue("tag", 0.0);
    textStyle_->fontVariations = fontVariations;
    ASSERT_EQ(textStyle_->fontVariations.GetAxisValues().size(), 1);
    EXPECT_EQ(textStyle_->fontVariations.GetAxisValues().at("tag"), fontVariations.GetAxisValues().at("tag"));
}

/*
 * @tc.name: TextStyleTest004
 * @tc.desc: test for change textShadows
 * @tc.type: FUNC
 */
HWTEST_F(TextStyleTest, TextStyleTest004, TestSize.Level0)
{
    SPText::TextShadow textShadowDefault;
    SkColor color = 255; // 255 just fot test
    SkPoint offset { 0, 0 };
    SPText::TextShadow textShadow(color, offset, 0.0);
    EXPECT_NE(textShadowDefault, textShadow);
    EXPECT_FALSE(textShadow.HasShadow());

    std::vector<SPText::TextShadow> textShadows;
    textShadows.emplace_back(textShadowDefault);
    textShadows.emplace_back(textShadow);
    textStyle_->textShadows = textShadows;
    EXPECT_EQ(textStyle_->textShadows.size(), 2);
}

/*
 * @tc.name: TextStyleTest005
 * @tc.desc: test for change backgroundRect
 * @tc.type: FUNC
 */
HWTEST_F(TextStyleTest, TextStyleTest005, TestSize.Level0)
{
    SPText::RectStyle backgroundRect = { 0, 1.0f, 0.0f, 0.0f, 0.0f };
    EXPECT_NE(textStyle_->backgroundRect, backgroundRect);
    textStyle_->backgroundRect = backgroundRect;
    EXPECT_EQ(textStyle_->backgroundRect, backgroundRect);
}

/*
 * @tc.name: TextStyleTest006
 * @tc.desc: test font features' GetFeatureSettings
 * @tc.type: FUNC
 */
HWTEST_F(TextStyleTest, TextStyleTest006, TestSize.Level0)
{
    SPText::FontFeatures fontFeatures;
    EXPECT_TRUE(fontFeatures.GetFeatureSettings().empty());
    fontFeatures.SetFeature("a", 0);
    fontFeatures.SetFeature("b", 1);
    EXPECT_EQ(fontFeatures.GetFeatureSettings(), "a=0,b=1");
}

/*
 * @tc.name: TextStyleTest007
 * @tc.desc: text shadow equal test
 * @tc.type: FUNC
 */
HWTEST_F(TextStyleTest, TextStyleTest007, TestSize.Level0)
{
    SPText::TextShadow shadowA;
    SPText::TextShadow shadowB;
    EXPECT_EQ(shadowA, shadowB);
    // 0.2 is just for test
    shadowB.blurSigma = 0.2;
    EXPECT_NE(shadowA, shadowB);
}

/*
 * @tc.name: TextStyleTest008
 * @tc.desc: Test for lineHeightStyle, minLineHeight and maxLineHeight from runMetrics's textStyle
 * @tc.type: FUNC
 */
HWTEST_F(TextStyleTest, TextStyleTest008, TestSize.Level0)
{
    SPText::ParagraphStyle paragraphStyle;
    auto fontCollection = std::make_shared<FontCollection>();
    ASSERT_NE(fontCollection, nullptr);
    fontCollection->SetupDefaultFontManager();
    std::unique_ptr<SPText::ParagraphBuilder> paragraphBuilder =
        SPText::ParagraphBuilder::Create(paragraphStyle, fontCollection);
    ASSERT_NE(paragraphBuilder, nullptr);
    OHOS::Rosen::SPText::TextStyle style;
    style.lineHeightStyle = OHOS::Rosen::LineHeightStyle::kFontHeight;
    style.maxLineHeight = LINE_HEIGHT;
    style.minLineHeight = LINE_HEIGHT;
    paragraphBuilder->PushStyle(style);
    paragraphBuilder->AddText(u"你好世界");
    std::shared_ptr<SPText::Paragraph> paragraph = paragraphBuilder->Build();
    ASSERT_NE(paragraph, nullptr);
    paragraph->Layout(200);
    skia::textlayout::LineMetrics lineMetrics;
    paragraph->GetLineMetricsAt(0, &lineMetrics);
    for (const auto& lineMetric : lineMetrics.fLineMetrics) {
        const auto& runMetricsStyle = lineMetric.second.text_style;
        EXPECT_EQ(runMetricsStyle->getLineHeightStyle(), skia::textlayout::LineHeightStyle::kFontHeight);
        EXPECT_TRUE(skia::textlayout::nearlyEqual(runMetricsStyle->getMinLineHeight(), LINE_HEIGHT));
        EXPECT_TRUE(skia::textlayout::nearlyEqual(runMetricsStyle->getMaxLineHeight(), LINE_HEIGHT));
    }
}

/*
 * @tc.name: TextStyleTest009
 * @tc.desc: test for fontEdging default value and equality
 * @tc.type: FUNC
 */
HWTEST_F(TextStyleTest, TextStyleTest009, TestSize.Level0)
{
    // Verify default fontEdging is ANTI_ALIAS
    EXPECT_EQ(textStyle_->fontEdging, Drawing::FontEdging::ANTI_ALIAS);

    // Test equality with default fontEdging
    SPText::TextStyle otherStyle;
    EXPECT_EQ((*textStyle_), otherStyle);

    // Test inequality with different fontEdging
    textStyle_->fontEdging = Drawing::FontEdging::ALIAS;
    EXPECT_NE((*textStyle_), otherStyle);

    // Test equality with same fontEdging
    otherStyle.fontEdging = Drawing::FontEdging::ALIAS;
    EXPECT_EQ((*textStyle_), otherStyle);
}

/*
 * @tc.name: TextStyleTest010
 * @tc.desc: test for fontEdging in paragraph builder
 * @tc.type: FUNC
 */
HWTEST_F(TextStyleTest, TextStyleTest010, TestSize.Level0)
{
    SPText::ParagraphStyle paragraphStyle;
    auto fontCollection = std::make_shared<FontCollection>();
    ASSERT_NE(fontCollection, nullptr);
    fontCollection->SetupDefaultFontManager();
    std::unique_ptr<SPText::ParagraphBuilder> paragraphBuilder =
        SPText::ParagraphBuilder::Create(paragraphStyle, fontCollection);
    ASSERT_NE(paragraphBuilder, nullptr);

    OHOS::Rosen::SPText::TextStyle style;
    style.fontEdging = Drawing::FontEdging::SUBPIXEL_ANTI_ALIAS;
    style.fontSize = FONT_SIZE;
    paragraphBuilder->PushStyle(style);
    paragraphBuilder->AddText(u"Hello World");
    std::shared_ptr<SPText::Paragraph> paragraph = paragraphBuilder->Build();
    ASSERT_NE(paragraph, nullptr);
    paragraph->Layout(200);

    skia::textlayout::LineMetrics lineMetrics;
    paragraph->GetLineMetricsAt(0, &lineMetrics);
    for (const auto& lineMetric : lineMetrics.fLineMetrics) {
        const auto& runMetricsStyle = lineMetric.second.text_style;
        EXPECT_EQ(runMetricsStyle->getFontEdging(), Drawing::FontEdging::SUBPIXEL_ANTI_ALIAS);
    }
}

/*
 * @tc.name: TextStyleTest011
 * @tc.desc: test for fontEdging with ALIAS value
 * @tc.type: FUNC
 */
HWTEST_F(TextStyleTest, TextStyleTest011, TestSize.Level0)
{
    SPText::ParagraphStyle paragraphStyle;
    auto fontCollection = std::make_shared<FontCollection>();
    ASSERT_NE(fontCollection, nullptr);
    fontCollection->SetupDefaultFontManager();
    std::unique_ptr<SPText::ParagraphBuilder> paragraphBuilder =
        SPText::ParagraphBuilder::Create(paragraphStyle, fontCollection);
    ASSERT_NE(paragraphBuilder, nullptr);

    OHOS::Rosen::SPText::TextStyle style;
    style.fontEdging = Drawing::FontEdging::ALIAS;
    style.fontSize = FONT_SIZE;
    paragraphBuilder->PushStyle(style);
    paragraphBuilder->AddText(u"Test Text");
    std::shared_ptr<SPText::Paragraph> paragraph = paragraphBuilder->Build();
    ASSERT_NE(paragraph, nullptr);
    paragraph->Layout(200);

    skia::textlayout::LineMetrics lineMetrics;
    paragraph->GetLineMetricsAt(0, &lineMetrics);
    for (const auto& lineMetric : lineMetrics.fLineMetrics) {
        const auto& runMetricsStyle = lineMetric.second.text_style;
        EXPECT_EQ(runMetricsStyle->getFontEdging(), Drawing::FontEdging::ALIAS);
    }
}

/*
 * @tc.name: TextStyleTest012
 * @tc.desc: test for fontEdging with ANTI_ALIAS value (default)
 * @tc.type: FUNC
 */
HWTEST_F(TextStyleTest, TextStyleTest012, TestSize.Level0)
{
    SPText::ParagraphStyle paragraphStyle;
    auto fontCollection = std::make_shared<FontCollection>();
    ASSERT_NE(fontCollection, nullptr);
    fontCollection->SetupDefaultFontManager();
    std::unique_ptr<SPText::ParagraphBuilder> paragraphBuilder =
        SPText::ParagraphBuilder::Create(paragraphStyle, fontCollection);
    ASSERT_NE(paragraphBuilder, nullptr);

    OHOS::Rosen::SPText::TextStyle style;
    // Explicitly set to ANTI_ALIAS (default value)
    style.fontEdging = Drawing::FontEdging::ANTI_ALIAS;
    style.fontSize = FONT_SIZE;
    paragraphBuilder->PushStyle(style);
    paragraphBuilder->AddText(u"Default Edging");
    std::shared_ptr<SPText::Paragraph> paragraph = paragraphBuilder->Build();
    ASSERT_NE(paragraph, nullptr);
    paragraph->Layout(200);

    skia::textlayout::LineMetrics lineMetrics;
    paragraph->GetLineMetricsAt(0, &lineMetrics);
    for (const auto& lineMetric : lineMetrics.fLineMetrics) {
        const auto& runMetricsStyle = lineMetric.second.text_style;
        EXPECT_EQ(runMetricsStyle->getFontEdging(), Drawing::FontEdging::ANTI_ALIAS);
    }
}

/*
 * @tc.name: TextStyleTest013
 * @tc.desc: test for fontEdging with multiple style changes
 * @tc.type: FUNC
 */
HWTEST_F(TextStyleTest, TextStyleTest013, TestSize.Level0)
{
    SPText::ParagraphStyle paragraphStyle;
    auto fontCollection = std::make_shared<FontCollection>();
    ASSERT_NE(fontCollection, nullptr);
    fontCollection->SetupDefaultFontManager();
    std::unique_ptr<SPText::ParagraphBuilder> paragraphBuilder =
        SPText::ParagraphBuilder::Create(paragraphStyle, fontCollection);
    ASSERT_NE(paragraphBuilder, nullptr);

    OHOS::Rosen::SPText::TextStyle style1;
    style1.fontEdging = Drawing::FontEdging::ALIAS;
    style1.fontSize = FONT_SIZE;
    paragraphBuilder->PushStyle(style1);
    paragraphBuilder->AddText(u"ALIAS");

    OHOS::Rosen::SPText::TextStyle style2;
    style2.fontEdging = Drawing::FontEdging::ANTI_ALIAS;
    style2.fontSize = FONT_SIZE;
    paragraphBuilder->PushStyle(style2);
    paragraphBuilder->AddText(u"ANTI_ALIAS");

    OHOS::Rosen::SPText::TextStyle style3;
    style3.fontEdging = Drawing::FontEdging::SUBPIXEL_ANTI_ALIAS;
    style3.fontSize = FONT_SIZE;
    paragraphBuilder->PushStyle(style3);
    paragraphBuilder->AddText(u"SUBPIXEL");

    std::shared_ptr<SPText::Paragraph> paragraph = paragraphBuilder->Build();
    ASSERT_NE(paragraph, nullptr);
    paragraph->Layout(500);

    skia::textlayout::LineMetrics lineMetrics;
    paragraph->GetLineMetricsAt(0, &lineMetrics);
    size_t runCount = 0;
    for (const auto& lineMetric : lineMetrics.fLineMetrics) {
        const auto& runMetricsStyle = lineMetric.second.text_style;
        if (runCount == 0) {
            EXPECT_EQ(runMetricsStyle->getFontEdging(), Drawing::FontEdging::ALIAS);
        } else if (runCount == 1) {
            EXPECT_EQ(runMetricsStyle->getFontEdging(), Drawing::FontEdging::ANTI_ALIAS);
        } else if (runCount == 2) {
            EXPECT_EQ(runMetricsStyle->getFontEdging(), Drawing::FontEdging::SUBPIXEL_ANTI_ALIAS);
        }
        runCount++;
    }
    EXPECT_GE(runCount, 3); // At least 3 runs for 3 different styles
}

/*
 * @tc.name: TextStyleTest014
 * @tc.desc: test for fontEdging in paragraph style default
 * @tc.type: FUNC
 */
HWTEST_F(TextStyleTest, TextStyleTest014, TestSize.Level0)
{
    SPText::ParagraphStyle paragraphStyle;
    auto fontCollection = std::make_shared<FontCollection>();
    ASSERT_NE(fontCollection, nullptr);
    fontCollection->SetupDefaultFontManager();
    std::unique_ptr<SPText::ParagraphBuilder> paragraphBuilder =
        SPText::ParagraphBuilder::Create(paragraphStyle, fontCollection);
    ASSERT_NE(paragraphBuilder, nullptr);

    // When no style is pushed, default paragraph style should have ANTI_ALIAS
    paragraphBuilder->AddText(u"Default Style");
    std::shared_ptr<SPText::Paragraph> paragraph = paragraphBuilder->Build();
    ASSERT_NE(paragraph, nullptr);
    paragraph->Layout(200);

    skia::textlayout::LineMetrics lineMetrics;
    paragraph->GetLineMetricsAt(0, &lineMetrics);
    for (const auto& lineMetric : lineMetrics.fLineMetrics) {
        const auto& runMetricsStyle = lineMetric.second.text_style;
        EXPECT_EQ(runMetricsStyle->getFontEdging(), Drawing::FontEdging::ANTI_ALIAS);
    }
}

/*
 * @tc.name: TextStyleTest015
 * @tc.desc: test for fontEdging with all enum values
 * @tc.type: FUNC
 */
HWTEST_F(TextStyleTest, TextStyleTest015, TestSize.Level0)
{
    std::vector<Drawing::FontEdging> edgingValues = { Drawing::FontEdging::ALIAS, Drawing::FontEdging::ANTI_ALIAS,
        Drawing::FontEdging::SUBPIXEL_ANTI_ALIAS };

    for (const auto& edging : edgingValues) {
        OHOS::Rosen::SPText::TextStyle style;
        style.fontEdging = edging;

        OHOS::Rosen::SPText::TextStyle otherStyle;
        otherStyle.fontEdging = edging;
        EXPECT_EQ(style, otherStyle);

        // Test inequality with different edging
        for (const auto& otherEdging : edgingValues) {
            if (otherEdging != edging) {
                otherStyle.fontEdging = otherEdging;
                EXPECT_NE(style, otherStyle);
            }
        }
    }
}

/*
 * @tc.name: TextStyleTest016
 * @tc.desc: test font edging affects run combination in normal layout
 * @tc.type: FUNC
 */
HWTEST_F(TextStyleTest, TextStyleTest016, TestSize.Level0)
{
    SPText::ParagraphStyle paragraphStyle;
    auto fontCollection = std::make_shared<FontCollection>();
    ASSERT_NE(fontCollection, nullptr);
    fontCollection->SetupDefaultFontManager();

    // Test 1: Different font edging results in two runs
    std::unique_ptr<SPText::ParagraphBuilder> paragraphBuilder1 =
        SPText::ParagraphBuilder::Create(paragraphStyle, fontCollection);
    ASSERT_NE(paragraphBuilder1, nullptr);

    OHOS::Rosen::SPText::TextStyle style1;
    style1.fontSize = FONT_SIZE;
    style1.fontEdging = Drawing::FontEdging::ANTI_ALIAS;
    paragraphBuilder1->PushStyle(style1);
    paragraphBuilder1->AddText(u"Hello");

    OHOS::Rosen::SPText::TextStyle style2;
    style2.fontSize = FONT_SIZE;
    style2.fontEdging = Drawing::FontEdging::SUBPIXEL_ANTI_ALIAS;
    paragraphBuilder1->PushStyle(style2);
    paragraphBuilder1->AddText(u"World");

    std::shared_ptr<SPText::Paragraph> paragraph1 = paragraphBuilder1->Build();
    ASSERT_NE(paragraph1, nullptr);
    paragraph1->Layout(500);

    auto paragraphImpl = reinterpret_cast<SPText::ParagraphImpl*>(paragraph1.get());
    auto skParagraph = reinterpret_cast<skia::textlayout::ParagraphImpl*>(paragraphImpl->paragraph_.get());
    ASSERT_NE(skParagraph, nullptr);
    EXPECT_EQ(skParagraph->fRuns.size(), 2); // Expect 2 runs for different font edging
    
    // Test 2: Same font edging results in one run (merged)
    std::unique_ptr<SPText::ParagraphBuilder> paragraphBuilder2 =
        SPText::ParagraphBuilder::Create(paragraphStyle, fontCollection);
    ASSERT_NE(paragraphBuilder2, nullptr);

    OHOS::Rosen::SPText::TextStyle style3;
    style3.fontSize = FONT_SIZE;
    style3.fontEdging = Drawing::FontEdging::ANTI_ALIAS;
    paragraphBuilder2->PushStyle(style3);
    paragraphBuilder2->AddText(u"Hello");

    OHOS::Rosen::SPText::TextStyle style4;
    style4.fontSize = FONT_SIZE;
    style4.fontEdging = Drawing::FontEdging::ANTI_ALIAS;
    paragraphBuilder2->PushStyle(style4);
    paragraphBuilder2->AddText(u"World");

    std::shared_ptr<SPText::Paragraph> paragraph2 = paragraphBuilder2->Build();
    ASSERT_NE(paragraph2, nullptr);
    paragraph2->Layout(500);

    paragraphImpl = reinterpret_cast<SPText::ParagraphImpl*>(paragraph2.get());
    skParagraph = reinterpret_cast<skia::textlayout::ParagraphImpl*>(paragraphImpl->paragraph_.get());
    ASSERT_NE(skParagraph, nullptr);
    EXPECT_EQ(skParagraph->fRuns.size(), 1); // Expect 1 run for same font edging
}
} // namespace txt