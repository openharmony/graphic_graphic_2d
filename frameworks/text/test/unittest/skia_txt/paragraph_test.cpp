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
namespace {
std::shared_ptr<Paragraph> BuildParagraphWithBorder(double borderWidth, const std::u16string& text)
{
    ParagraphStyle paragraphStyle;
    // The adapter defaults to a tail ellipsis, which forces every paragraph onto a
    // single line; wrapping tests need a plain multi-line paragraph instead.
    paragraphStyle.ellipsis = u"";
    auto fontCollection = std::make_shared<FontCollection>();
    fontCollection->SetupDefaultFontManager();
    auto builder = ParagraphBuilder::Create(paragraphStyle, fontCollection);
    if (builder == nullptr) {
        return nullptr;
    }
    SPText::TextStyle style;
    style.backgroundRect.color = 0xFF00FF00;       // 0xFF00FF00 just for test
    style.backgroundRect.borderColor = 0xFFFF0000; // 0xFFFF0000 just for test
    style.backgroundRect.borderWidth = borderWidth;
    builder->PushStyle(style);
    builder->AddText(text);
    builder->Pop();
    return builder->Build();
}

std::shared_ptr<Paragraph> BuildJustifyParagraphWithBorder(
    OHOS::Rosen::SPText::TextAlign align, double borderWidth, const std::u16string& text)
{
    ParagraphStyle paragraphStyle;
    paragraphStyle.textAlign = align;
    // Same ellipsis clearing as BuildParagraphWithBorder: the adapter default forces
    // a single line, which would keep justify from ever stretching a line.
    paragraphStyle.ellipsis = u"";
    auto fontCollection = std::make_shared<FontCollection>();
    fontCollection->SetupDefaultFontManager();
    auto builder = ParagraphBuilder::Create(paragraphStyle, fontCollection);
    if (builder == nullptr) {
        return nullptr;
    }
    SPText::TextStyle style;
    style.backgroundRect.color = 0xFF00FF00;       // 0xFF00FF00 just for test
    style.backgroundRect.borderColor = 0xFFFF0000; // 0xFFFF0000 just for test
    style.backgroundRect.borderWidth = borderWidth;
    builder->PushStyle(style);
    builder->AddText(text);
    builder->Pop();
    return builder->Build();
}
} // namespace
class ParagraphTest : public testing::Test {
public:
    void SetUp() override;
    void TearDown() override;

    static bool AnimationFunc(const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig);
    static skia::textlayout::ParagraphImpl* GetParagraphSkiaImpl(const std::shared_ptr<Paragraph>& paragraph);
    static ParagraphImpl* GetParagraphImpl(const std::shared_ptr<Paragraph>& paragraph);
    void PrepareMiddleEllipsis(size_t& maxLines, const std::u16string& str, const std::u16string& text);
    static std::shared_ptr<Paragraph> BuildParagraphWithTab(const std::shared_ptr<FontCollection>& fontCollection,
        const OHOS::Rosen::SPText::TextTab& tab, const std::u16string& text);

protected:
    std::shared_ptr<Paragraph> paragraph_;
    std::shared_ptr<Paragraph> paragraphMiddleEllipsis_;
    int layoutWidth_ = 50;
    std::u16string text_ = u"text";
};

void ParagraphTest::SetUp()
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

void ParagraphTest::TearDown()
{
    paragraph_.reset();
}

ParagraphImpl* ParagraphTest::GetParagraphImpl(const std::shared_ptr<Paragraph>& paragraph)
{
    return static_cast<ParagraphImpl*>(paragraph.get());
}

skia::textlayout::ParagraphImpl* ParagraphTest::GetParagraphSkiaImpl(const std::shared_ptr<Paragraph>& paragraph)
{
    return static_cast<skia::textlayout::ParagraphImpl*>(GetParagraphImpl(paragraph)->paragraph_.get());
}

std::shared_ptr<Paragraph> ParagraphTest::BuildParagraphWithTab(const std::shared_ptr<FontCollection>& fontCollection,
    const OHOS::Rosen::SPText::TextTab& tab, const std::u16string& text)
{
    ParagraphStyle paragraphStyle;
    paragraphStyle.tab = tab;
    std::shared_ptr<ParagraphBuilder> paragraphBuilder = ParagraphBuilder::Create(paragraphStyle, fontCollection);
    if (paragraphBuilder == nullptr) {
        return nullptr;
    }
    paragraphBuilder->AddText(text);
    return paragraphBuilder->Build();
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
HWTEST_F(ParagraphTest, ParagraphTest001, TestSize.Level0)
{
    EXPECT_EQ(paragraph_->GetMaxWidth(), layoutWidth_);
    EXPECT_EQ(paragraph_->GetHeight(), 19);
    EXPECT_EQ(static_cast<int>(paragraph_->GetLongestLine()), 44);
    EXPECT_EQ(static_cast<int>(paragraph_->GetMinIntrinsicWidth()), 78);
    EXPECT_EQ(static_cast<int>(paragraph_->GetMaxIntrinsicWidth()), 78);
    EXPECT_EQ(static_cast<int>(paragraph_->GetAlphabeticBaseline()), 14);
    EXPECT_EQ(static_cast<int>(paragraph_->GetIdeographicBaseline()), 18);
    EXPECT_EQ(paragraph_->GetGlyphsBoundsTop(), -11);
    EXPECT_EQ(paragraph_->GetGlyphsBoundsBottom(), 1);
    EXPECT_EQ(paragraph_->GetGlyphsBoundsLeft(), 0);
    EXPECT_EQ(static_cast<int>(paragraph_->GetGlyphsBoundsRight()), 36);
    EXPECT_TRUE(paragraph_->DidExceedMaxLines());
    EXPECT_EQ(paragraph_->GetLineCount(), 1);
    EXPECT_EQ(paragraph_->GetUnresolvedGlyphsCount(), 0);
    EXPECT_EQ(paragraph_->GetRectsForRange(0, text_.size(), RectHeightStyle::MAX, RectWidthStyle::TIGHT).size(), 1);
    EXPECT_EQ(paragraph_->GetRectsForPlaceholders().size(), 0);
    EXPECT_EQ(paragraph_->GetGlyphPositionAtCoordinate(0.0, 0.0).affinity, OHOS::Rosen::SPText::Affinity::DOWNSTREAM);
    EXPECT_EQ(paragraph_->GetGlyphPositionAtCoordinate(0.0, 0.0).position, 0.0);
    EXPECT_EQ(paragraph_->GetWordBoundary(0).start, 0);
    EXPECT_EQ(paragraph_->GetActualTextRange(0, false).start, 0);
    EXPECT_EQ(paragraph_->GetActualTextRange(-1, false).start, 0);
    EXPECT_EQ(paragraph_->GetActualTextRange(paragraph_->GetLineCount(), false).start, 0);
    EXPECT_EQ(paragraph_->GetActualTextRange(paragraph_->GetLineCount(), false).end, 0);
    EXPECT_EQ(paragraph_->GetLineMetrics().size(), 1);
    EXPECT_EQ(static_cast<int>(paragraph_->GetLongestLineWithIndent()), 44);
}

/*
 * @tc.name: ParagraphTest002
 * @tc.desc: test for SetIndents
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTest002, TestSize.Level0)
{
    // 0.5 just for test
    std::vector<float> indents = { 0.5, 1 };
    paragraph_->SetIndents(indents);
    EXPECT_EQ(paragraph_->DetectIndents(0), 0.5);
    EXPECT_EQ(paragraph_->DetectIndents(indents.size()), 1);
}

/*
 * @tc.name: ParagraphTest003
 * @tc.desc: test for MarkDirty
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTest003, TestSize.Level0)
{
    paragraph_->MarkDirty();
    auto paragraphImpl = GetParagraphSkiaImpl(paragraph_);
    EXPECT_EQ(paragraphImpl->fState, skia::textlayout::kIndexed);
}

/*
 * @tc.name: ParagraphTest004
 * @tc.desc: test for UpdateFontSize
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTest004, TestSize.Level0)
{
    // 2 24.0 just for test
    paragraph_->UpdateFontSize(0, text_.size(), 24.0);
    auto paragraphImpl = GetParagraphSkiaImpl(paragraph_);
    ASSERT_GT(paragraphImpl->fTextStyles.size(), 0);
    EXPECT_EQ(paragraphImpl->fTextStyles.at(0).fStyle.getFontSize(), 24);
}

/*
 * @tc.name: ParagraphTest005
 * @tc.desc: test for Paint
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTest005, TestSize.Level0)
{
    SkCanvas skCanvas;
    // redundancy because it has been checked in setup
    ASSERT_NE(paragraph_, nullptr);
    paragraph_->Paint(&skCanvas, 0.0, 0.0);
    Canvas canvas;
    paragraph_->Paint(&canvas, 0.0, 0.0);
    Path path;
    paragraph_->Paint(&canvas, &path, 0.0, 0.0);
}

/*
 * @tc.name: ParagraphTest006
 * @tc.desc: test for GetLineMetricsAt
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTest006, TestSize.Level0)
{
    skia::textlayout::LineMetrics lineMetrics;
    auto metrics = paragraph_->MeasureText();
    ASSERT_TRUE(paragraph_->GetLineMetricsAt(0, &lineMetrics));
    EXPECT_EQ(lineMetrics.fAscent, -metrics.fAscent);
    EXPECT_EQ(lineMetrics.fDescent, metrics.fDescent);
}

/*
 * @tc.name: ParagraphTest007
 * @tc.desc: test for SetAnimation
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTest007, TestSize.Level0)
{
    std::function<bool(const std::shared_ptr<TextEngine::SymbolAnimationConfig>&)> animationFunc =
        ParagraphTest::AnimationFunc;
    paragraph_->SetAnimation(animationFunc);
    auto paragraphImpl = GetParagraphImpl(paragraph_);
    EXPECT_TRUE(paragraphImpl->animationFunc_(nullptr));
}

/*
 * @tc.name: ParagraphTest008
 * @tc.desc: test for SetParagraghId
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTest008, TestSize.Level0)
{
    paragraph_->SetParagraghId(1);
    auto paragraphImpl = GetParagraphImpl(paragraph_);
    EXPECT_EQ(paragraphImpl->id_, 1);
}

/*
 * @tc.name: ParagraphTest009
 * @tc.desc: test for GetFontMetricsResult
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTest009, TestSize.Level0)
{
    SPText::TextStyle textStyle;
    auto fontMetrics = paragraph_->GetFontMetricsResult(textStyle);
    EXPECT_EQ(static_cast<int>(fontMetrics.fTop), -14);
}

/*
 * @tc.name: ParagraphTest010
 * @tc.desc: test for GetLineFontMetrics
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTest010, TestSize.Level0)
{
    size_t charNumber = 1;
    std::vector<Drawing::FontMetrics> fontMetrics;
    ASSERT_TRUE(paragraph_->GetLineFontMetrics(1, charNumber, fontMetrics));
    ASSERT_GT(fontMetrics.size(), 0);
    auto metrics = paragraph_->MeasureText();
    EXPECT_EQ(metrics.fAscent, fontMetrics.at(0).fAscent);
    EXPECT_EQ(metrics.fDescent, fontMetrics.at(0).fDescent);
}

/*
 * @tc.name: ParagraphTest011
 * @tc.desc: test for CloneSelf
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTest011, TestSize.Level0)
{
    auto paragraphClone = paragraph_->CloneSelf();
    EXPECT_EQ(paragraphClone->GetHeight(), paragraph_->GetHeight());
    EXPECT_EQ(paragraphClone->GetMaxWidth(), paragraph_->GetMaxWidth());
    EXPECT_EQ(paragraphClone->GetMinIntrinsicWidth(), paragraph_->GetMinIntrinsicWidth());
    EXPECT_EQ(paragraphClone->GetGlyphsBoundsBottom(), paragraph_->GetGlyphsBoundsBottom());
    EXPECT_EQ(paragraphClone->GetGlyphsBoundsLeft(), paragraph_->GetGlyphsBoundsLeft());
    EXPECT_EQ(paragraphClone->GetLongestLine(), paragraph_->GetLongestLine());
    EXPECT_EQ(paragraphClone->GetLongestLineWithIndent(), paragraph_->GetLongestLineWithIndent());
}

/*
 * @tc.name: ParagraphTest012
 * @tc.desc: test for SkStyleToTextStyle
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTest012, TestSize.Level0)
{
    skt::TextStyle skStyle;
    skStyle.setBackgroundPaintID(-1);
    skStyle.addShadow(skia::textlayout::TextShadow());
    skStyle.setColor(0xFF0000FF);           // used for testing. Set the color to blue.
    skStyle.setDecorationColor(0xFF000000); // used for testing. Set the color to black.
    skStyle.setDecorationThicknessMultiplier(2.0f);
    skStyle.setFontFamilies({ SkString("sans-serif") });
    skStyle.setFontSize(12.0f);
    skStyle.setHeight(1.5f);
    SPText::TextStyle txt = paragraph_->SkStyleToTextStyle(skStyle);
    EXPECT_EQ(txt.color, skStyle.getColor());
    EXPECT_EQ(txt.decorationColor, skStyle.fDecoration.fColor);
    EXPECT_EQ(txt.decorationThicknessMultiplier, skStyle.fDecoration.fThicknessMultiplier);
    EXPECT_EQ(txt.fontSize, skStyle.getFontSize());
    EXPECT_EQ(txt.height, skStyle.getHeight());
    ASSERT_GT(txt.fontFamilies.size(), 0);
    EXPECT_EQ(txt.fontFamilies.back(), std::string(skStyle.fFontFamilies.back().c_str()));
}

/*
 * @tc.name: ParagraphTest013
 * @tc.desc: test for UpdateColor
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTest013, TestSize.Level0)
{
    RSColor color(255, 0, 0, 255);
    std::u16string text = u"text";
    paragraph_->UpdateColor(0, text.length(), color);
    auto paragraphImpl = GetParagraphImpl(paragraph_);
    for (auto p: paragraphImpl->paints_) {
        EXPECT_EQ(p.color, color);
    }
}

/*
 * @tc.name: ParagraphTest014
 * @tc.desc: test for SkStyleToTextStyle
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTest014, TestSize.Level0)
{
    auto metrics = paragraph_->GetLineMetrics();
    for (auto skLineMetrics : metrics) {
        for (const auto& [index, styleMetrics] : skLineMetrics.fLineMetrics) {
            OHOS::Rosen::SPText::TextStyle spTextStyle = paragraph_->SkStyleToTextStyle(*styleMetrics.text_style);
            EXPECT_EQ(spTextStyle.color, styleMetrics.text_style->fColor);
            EXPECT_EQ(spTextStyle.decorationColor, styleMetrics.text_style->fDecoration.fColor);
        }
    }
}

/*
 * @tc.name: ParagraphHyphenTest001
 * @tc.desc: test for text break work with hyphen strategy in en-gb env.
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphHyphenTest001, TestSize.Level0)
{
    OHOS::Rosen::SPText::TextStyle style;
    style.fontSize = 50;
    ParagraphStyle paragraphStyle;
    paragraphStyle.maxLines = 10;
    paragraphStyle.locale = "en-gb";
    paragraphStyle.wordBreakType = OHOS::Rosen::SPText::WordBreakType::BREAK_HYPHEN;
    std::shared_ptr<FontCollection> fontCollection = std::make_shared<FontCollection>();
    ASSERT_NE(fontCollection, nullptr);
    fontCollection->SetupDefaultFontManager();
    std::shared_ptr<ParagraphBuilder> paragraphBuilder = ParagraphBuilder::Create(paragraphStyle, fontCollection);
    ASSERT_NE(paragraphBuilder, nullptr);
    std::u16string text = u"British English is the official language of the United Kingdom. It has some differences in "
                          u"spelling, pronunciation, and vocabulary compared to American English.";
    paragraphBuilder->PushStyle(style);
    paragraphBuilder->AddText(text);
    std::shared_ptr<Paragraph> paragraph = paragraphBuilder->Build();
    ASSERT_NE(paragraph, nullptr);
    paragraph->Layout(687);
    std::vector<std::shared_ptr<SPText::TextLineBase>> textLines = paragraph->GetTextLines();
    ASSERT_EQ(textLines.size(), 6);
    //expect lines 2,3,4 to have hyphenation breakpoints,
    //and the last charater of each line to have a hyphen glyphid of 800
    size_t breakArr[3] = {2, 3, 4};
    for (std::size_t i = 0; i < 3; i++) {
        ASSERT_NE(textLines.at(breakArr[i]), nullptr);
        std::vector<std::unique_ptr<SPText::Run>> runs = textLines.at(breakArr[i])->GetGlyphRuns();
        ASSERT_NE(runs.back(), nullptr);
        std::vector<uint16_t> glyphs = runs.back()->GetGlyphs();
        EXPECT_EQ(glyphs.back(), 800);
    }
}

/*
 * @tc.name: ParagraphHyphenTest002
 * @tc.desc: test for text break work with hyphen strategy in en-gb env, and text word ends with special chars.
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphHyphenTest002, TestSize.Level0)
{
    ParagraphStyle paragraphStyle;
    paragraphStyle.maxLines = 10;
    paragraphStyle.fontSize = 72;
    paragraphStyle.locale = "en-gb";
    paragraphStyle.breakStrategy = OHOS::Rosen::SPText::BreakStrategy::HIGH_QUALITY;
    paragraphStyle.wordBreakType = OHOS::Rosen::SPText::WordBreakType::BREAK_HYPHEN;
    std::shared_ptr<FontCollection> fontCollection = std::make_shared<FontCollection>();
    ASSERT_NE(fontCollection, nullptr);
    fontCollection->SetupDefaultFontManager();
    std::shared_ptr<ParagraphBuilder> paragraphBuilder = ParagraphBuilder::Create(paragraphStyle, fontCollection);
    ASSERT_NE(paragraphBuilder, nullptr);
    std::u16string text = u"Special character =========== Special character a---------- Special character ab,,,,,,,,,,";
    paragraphBuilder->AddText(text);
    std::shared_ptr<Paragraph> paragraph = paragraphBuilder->Build();
    ASSERT_NE(paragraph, nullptr);
    // Test for layout width 519
    paragraph->Layout(519);
    std::vector<std::shared_ptr<SPText::TextLineBase>> textLines = paragraph->GetTextLines();
    EXPECT_EQ(textLines.size(), 8);
    //expect lines 0,3,6 to have hyphenation breakpoints,
    size_t breakArr[3] = {0, 3, 6};
    for (size_t i = 0; i < 3; i++) {
        ASSERT_NE(textLines.at(breakArr[i]), nullptr);
        std::vector<std::unique_ptr<SPText::Run>> runs = textLines.at(breakArr[i])->GetGlyphRuns();
        ASSERT_NE(runs.back(), nullptr);
        std::vector<uint16_t> glyphs = runs.back()->GetGlyphs();
        // The last charater of each line to have a hyphen glyphid of 800
        EXPECT_EQ(glyphs.back(), 800);
    }
}

/*
 * @tc.name: ParagraphTest016
 * @tc.desc: test for combin follow liga
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTest016, TestSize.Level0)
{
    OHOS::Rosen::SPText::TextStyle style;
    style.fontSize = 30;
    ParagraphStyle paragraphStyle;
    paragraphStyle.spTextStyle = style;
    std::shared_ptr<FontCollection> fontCollection = std::make_shared<FontCollection>();
    ASSERT_NE(fontCollection, nullptr);
    fontCollection->SetupDefaultFontManager();
    std::shared_ptr<ParagraphBuilder> paragraphBuilder = ParagraphBuilder::Create(paragraphStyle, fontCollection);
    ASSERT_NE(paragraphBuilder, nullptr);
    std::u16string text = u"brt゙゙";
    paragraphBuilder->PushStyle(style);
    paragraphBuilder->AddText(text);
    std::shared_ptr<Paragraph> paragraph = paragraphBuilder->Build();
    ASSERT_NE(paragraph, nullptr);
    paragraph->Layout(100);
    std::vector<std::shared_ptr<SPText::TextLineBase>> textLines = paragraph->GetTextLines();
    ASSERT_EQ(textLines.size(), 1);
    std::shared_ptr<SPText::TextLineBase>& textLine = textLines.at(0);
    std::vector<std::unique_ptr<SPText::Run>> runs = textLine->GetGlyphRuns();
    ASSERT_EQ(runs.size(), 2);
    // 'a' will hit HM Sans
    std::vector<uint16_t> glyphs1 = runs.at(0)->GetGlyphs();
    ASSERT_EQ(glyphs1.size(), 1);
    EXPECT_EQ(glyphs1.at(0), 217);
    // 'rt゙゙' will hit cjk
    std::vector<uint16_t> glyphs2 = runs.at(1)->GetGlyphs();
    ASSERT_EQ(glyphs2.size(), 4);
    EXPECT_EQ(glyphs2.at(0), 83);
    EXPECT_EQ(glyphs2.at(1), 85);
    EXPECT_EQ(glyphs2.at(2), 1546);
    EXPECT_EQ(glyphs2.at(3), 1546);
}

void ParagraphTest::PrepareMiddleEllipsis(size_t& maxLines, const std::u16string& str, const std::u16string& text)
{
    ParagraphStyle paragraphStyle;
    paragraphStyle.maxLines = maxLines;
    paragraphStyle.ellipsis = str;
    paragraphStyle.ellipsisModal = OHOS::Rosen::SPText::EllipsisModal::MIDDLE;
    std::shared_ptr<FontCollection> fontCollection = std::make_shared<FontCollection>();
    ASSERT_NE(fontCollection, nullptr);
    fontCollection->SetupDefaultFontManager();
    std::shared_ptr<ParagraphBuilder> paragraphBuilder = ParagraphBuilder::Create(paragraphStyle, fontCollection);
    ASSERT_NE(paragraphBuilder, nullptr);
    OHOS::Rosen::SPText::TextStyle style;
    style.fontSize = 30; // 30 default fontsize
    paragraphBuilder->PushStyle(style);
    paragraphBuilder->AddText(text);
    paragraphMiddleEllipsis_ = paragraphBuilder->Build();
    ASSERT_NE(paragraphMiddleEllipsis_, nullptr);
}

/*
 * @tc.name: ParagraphTestMiddleEllipsis001
 * @tc.desc: test for Middle Ellipsis 001, English word
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTestMiddleEllipsis001, TestSize.Level0)
{
    size_t maxLines = 1;
    PrepareMiddleEllipsis(maxLines, u"...", u"Hello World");
    paragraphMiddleEllipsis_->Layout(100);
    OHOS::Rosen::SPText::Range<size_t> range = paragraphMiddleEllipsis_->GetEllipsisTextRange();
    EXPECT_EQ(range.start, 2);
    EXPECT_EQ(range.end, 8);
    EXPECT_FALSE(paragraphMiddleEllipsis_->CanPaintAllText());
    paragraphMiddleEllipsis_->Layout(500);

    OHOS::Rosen::SPText::ParagraphImpl* paragraphImpl = GetParagraphImpl(paragraphMiddleEllipsis_);
    ASSERT_NE(paragraphImpl, nullptr);
    std::unique_ptr<skt::Paragraph> temp = nullptr;
    paragraphImpl->paragraph_.swap(temp);
    EXPECT_FALSE(paragraphImpl->CanPaintAllText());
    paragraphImpl->paragraph_.swap(temp);
    EXPECT_TRUE(paragraphMiddleEllipsis_->CanPaintAllText());
}

/*
 * @tc.name: ParagraphTestMiddleEllipsis002
 * @tc.desc: test for Middle Ellipsis 002,Burmese combine
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTestMiddleEllipsis002, TestSize.Level0)
{
    size_t maxLines = 1;
    PrepareMiddleEllipsis(maxLines, u"...", u"ျမင့္ေသာ ႏွလုံးခုန္ႏႈန္း သတ္မွတ္ခ်က္");
    paragraphMiddleEllipsis_->Layout(320);
    OHOS::Rosen::SPText::Range<size_t> range = paragraphMiddleEllipsis_->GetEllipsisTextRange();
    EXPECT_EQ(range.start, 7);
    EXPECT_EQ(range.end, 24);
}

/*
 * @tc.name: ParagraphTestMiddleEllipsis003
 * @tc.desc: test for Middle Ellipsis 003,emoji
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTestMiddleEllipsis003, TestSize.Level0)
{
    size_t maxLines = 1;
    PrepareMiddleEllipsis(maxLines, u"...", u"😊😂 MM abc 中文 123");
    paragraphMiddleEllipsis_->Layout(200);
    OHOS::Rosen::SPText::Range<size_t> range = paragraphMiddleEllipsis_->GetEllipsisTextRange();
    EXPECT_EQ(range.start, 5);
    EXPECT_EQ(range.end, 13);
}

/*
 * @tc.name: ParagraphTestMiddleEllipsis004
 * @tc.desc: test for Middle Ellipsis 004,chinese & number & English
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTestMiddleEllipsis004, TestSize.Level0)
{
    size_t maxLines = 1;
    PrepareMiddleEllipsis(maxLines, u"...", u"你好       123    Hello ");
    paragraphMiddleEllipsis_->Layout(200);
    OHOS::Rosen::SPText::Range<size_t> range = paragraphMiddleEllipsis_->GetEllipsisTextRange();
    EXPECT_EQ(range.start, 5);
    EXPECT_EQ(range.end, 15);
}

/*
 * @tc.name: ParagraphTestMiddleEllipsis005
 * @tc.desc: test for Middle Ellipsis 005,Tibetan
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTestMiddleEllipsis005, TestSize.Level0)
{
    size_t maxLines = 1;
    PrepareMiddleEllipsis(maxLines, u"...", u"བོད་སྐད་ནི་སྒྲ་གཉིས་ཀྱི་བྱུས་དང་སྤྱི་ཚད་ཁུར་སྒྲ་སེམས་བཞིན་ཡོད།");
    paragraphMiddleEllipsis_->Layout(200);
    OHOS::Rosen::SPText::Range<size_t> range = paragraphMiddleEllipsis_->GetEllipsisTextRange();
    EXPECT_EQ(range.start, 11);
    EXPECT_EQ(range.end, 53);
}

/*
 * @tc.name: ParagraphTestMiddleEllipsis006
 * @tc.desc: test for Middle Ellipsis 006,Empty ellipsis,Empty String
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTestMiddleEllipsis006, TestSize.Level0)
{
    size_t maxLines = 1;
    PrepareMiddleEllipsis(maxLines, u"", u"");
    paragraphMiddleEllipsis_->Layout(200);
    OHOS::Rosen::SPText::Range<size_t> range = paragraphMiddleEllipsis_->GetEllipsisTextRange();
    EXPECT_EQ(range.start, std::numeric_limits<size_t>::max());
    EXPECT_EQ(range.end, std::numeric_limits<size_t>::max());
}

/*
 * @tc.name: ParagraphTestMiddleEllipsis007
 * @tc.desc: test for Middle Ellipsis 007,maxLines != 1
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTestMiddleEllipsis007, TestSize.Level0)
{
    size_t maxLines = 2;
    PrepareMiddleEllipsis(maxLines, u"...", u"Hello World");
    paragraphMiddleEllipsis_->Layout(100);
    OHOS::Rosen::SPText::Range<size_t> range = paragraphMiddleEllipsis_->GetEllipsisTextRange();
    EXPECT_EQ(range.start, std::numeric_limits<size_t>::max());
    EXPECT_EQ(range.end, std::numeric_limits<size_t>::max());
    EXPECT_TRUE(paragraphMiddleEllipsis_->CanPaintAllText());
    ParagraphStyle paragraphStyle;
    paragraphStyle.wordBreakType = OHOS::Rosen::SPText::WordBreakType::BREAK_WORD;
    paragraphStyle.relayoutChangeBitmap.set(static_cast<size_t>(RelayoutParagraphStyleAttribute::WORD_BREAKTYPE));
    std::vector<OHOS::Rosen::SPText::TextStyle> relayoutTextStyles;
    OHOS::Rosen::SPText::TextStyle textStyle;
    relayoutTextStyles.push_back(textStyle);
    paragraphMiddleEllipsis_->Relayout(50, paragraphStyle, relayoutTextStyles);
    EXPECT_FALSE(paragraphMiddleEllipsis_->CanPaintAllText());
}

/*
 * @tc.name: ParagraphTestMiddleEllipsis008
 * @tc.desc: test for Middle Ellipsis 008, uygurqa
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTestMiddleEllipsis008, TestSize.Level0)
{
    size_t maxLines = 1;
    PrepareMiddleEllipsis(maxLines, u"...", u"ياخشىمۇ سىز؟ ھەركىمگە قۇتلۇق كۈنىمىز بولسۇن!");
    paragraphMiddleEllipsis_->Layout(100);
    OHOS::Rosen::SPText::Range<size_t> range = paragraphMiddleEllipsis_->GetEllipsisTextRange();
    EXPECT_EQ(range.start, 3);
    EXPECT_EQ(range.end, 41);
}

/*
 * @tc.name: ParagraphTestMiddleEllipsis009
 * @tc.desc: test for Middle Ellipsis 009, long tail space
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTestMiddleEllipsis009, TestSize.Level0)
{
    size_t maxLines = 1;
    PrepareMiddleEllipsis(maxLines, u"...", u"Hello Wolrd                ");
    paragraphMiddleEllipsis_->Layout(100);
    OHOS::Rosen::SPText::Range<size_t> range = paragraphMiddleEllipsis_->GetEllipsisTextRange();
    EXPECT_EQ(range.start, 2);
    EXPECT_EQ(range.end, 23);
}

/*
 * @tc.name: ParagraphTestMiddleEllipsis010
 * @tc.desc: test for Middle Ellipsis 010, empty text
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTestMiddleEllipsis010, TestSize.Level0)
{
    size_t maxLines = 1;
    PrepareMiddleEllipsis(maxLines, u"...", u"");
    paragraphMiddleEllipsis_->Layout(1);
    OHOS::Rosen::SPText::Range<size_t> range = paragraphMiddleEllipsis_->GetEllipsisTextRange();
    EXPECT_EQ(range.start, std::numeric_limits<size_t>::max());
    EXPECT_EQ(range.end, std::numeric_limits<size_t>::max());
}

/*
 * @tc.name: ParagraphTestMiddleEllipsis011
 * @tc.desc: test for Middle Ellipsis 011, empty ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTestMiddleEllipsis011, TestSize.Level0)
{
    size_t maxLines = 1;
    PrepareMiddleEllipsis(maxLines, u"", u"Hello World");
    paragraphMiddleEllipsis_->Layout(1);
    OHOS::Rosen::SPText::Range<size_t> range = paragraphMiddleEllipsis_->GetEllipsisTextRange();
    EXPECT_EQ(range.start, std::numeric_limits<size_t>::max());
    EXPECT_EQ(range.end, std::numeric_limits<size_t>::max());
}

/*
 * @tc.name: ParagraphTestMiddleEllipsis012
 * @tc.desc: test for Middle Ellipsis 012, add placeholder
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTestMiddleEllipsis012, TestSize.Level0)
{
    size_t maxLines = 1;
    ParagraphStyle paragraphStyle;
    paragraphStyle.maxLines = maxLines;
    paragraphStyle.ellipsis = u"...";
    paragraphStyle.ellipsisModal = OHOS::Rosen::SPText::EllipsisModal::MIDDLE;
    std::shared_ptr<FontCollection> fontCollection = std::make_shared<FontCollection>();
    ASSERT_NE(fontCollection, nullptr);
    fontCollection->SetupDefaultFontManager();
    std::shared_ptr<ParagraphBuilder> paragraphBuilder = ParagraphBuilder::Create(paragraphStyle, fontCollection);
    ASSERT_NE(paragraphBuilder, nullptr);
    OHOS::Rosen::SPText::TextStyle style;
    style.fontSize = 30; // 30 default fontsize
    PlaceholderRun placeholderRun(50, 50, PlaceholderAlignment::BASELINE, SPText::TextBaseline::ALPHABETIC, 0.0);
    paragraphBuilder->AddPlaceholder(placeholderRun);
    paragraphBuilder->PushStyle(style);
    paragraphBuilder->AddText(u"Hello World");
    PlaceholderRun placeholderRun2(150, 150, PlaceholderAlignment::BASELINE, SPText::TextBaseline::ALPHABETIC, 0.0);
    paragraphBuilder->AddPlaceholder(placeholderRun2);
    paragraphMiddleEllipsis_ = paragraphBuilder->Build();
    ASSERT_NE(paragraphMiddleEllipsis_, nullptr);
    paragraphMiddleEllipsis_->Layout(100);
    OHOS::Rosen::SPText::Range<size_t> range = paragraphMiddleEllipsis_->GetEllipsisTextRange();
    EXPECT_EQ(range.start, 1);
    EXPECT_EQ(range.end, 13);
}

/*
 * @tc.name: ParagraphTestMiddleEllipsis013
 * @tc.desc: test for Middle Ellipsis 013, \n
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTestMiddleEllipsis013, TestSize.Level0)
{
    size_t maxLines = 1;
    PrepareMiddleEllipsis(maxLines, u"...", u"A\nB");
    paragraphMiddleEllipsis_->Layout(1);
    OHOS::Rosen::SPText::Range<size_t> range = paragraphMiddleEllipsis_->GetEllipsisTextRange();
    EXPECT_EQ(range.start, 0);
    EXPECT_EQ(range.end, 2);
}

/*
 * @tc.name: ParagraphTestMiddleEllipsis014
 * @tc.desc: test for Middle Ellipsis 014, \n
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTestMiddleEllipsis014, TestSize.Level0)
{
    size_t maxLines = 1;
    PrepareMiddleEllipsis(maxLines, u"...", u"\nB");
    paragraphMiddleEllipsis_->Layout(1);
    OHOS::Rosen::SPText::Range<size_t> range = paragraphMiddleEllipsis_->GetEllipsisTextRange();
    EXPECT_EQ(range.start, std::numeric_limits<size_t>::max());
    EXPECT_EQ(range.end, std::numeric_limits<size_t>::max());
}

/*
 * @tc.name: ParagraphTestMiddleEllipsis015
 * @tc.desc: test for Middle Ellipsis 015, get glyph position
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTestMiddleEllipsis015, TestSize.Level0)
{
    size_t maxLines = 1;
    PrepareMiddleEllipsis(maxLines, u"...", u"你好世界 Hello World");
    paragraphMiddleEllipsis_->Layout(280);
    EXPECT_EQ(paragraphMiddleEllipsis_->GetGlyphPositionAtCoordinate(50, 0.0).position, 2.0);
    EXPECT_EQ(paragraphMiddleEllipsis_->GetGlyphPositionAtCoordinate(150, 0.0).position, 7.0);
    EXPECT_EQ(paragraphMiddleEllipsis_->GetGlyphPositionAtCoordinate(270, 0.0).position, 16.0);
}

/*
 * @tc.name: ParagraphTestMiddleEllipsis016
 * @tc.desc: test for Middle Ellipsis 016, get rects for range
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTestMiddleEllipsis016, TestSize.Level0)
{
    size_t maxLines = 1;
    std::u16string text = u"你好世界12312212\n你好世界";
    PrepareMiddleEllipsis(maxLines, u"...", text);
    paragraphMiddleEllipsis_->Layout(238);
    std::vector<TextBox> boxes =
        paragraphMiddleEllipsis_->GetRectsForRange(0, text.size(), RectHeightStyle::TIGHT, RectWidthStyle::TIGHT);
    EXPECT_EQ(boxes.size(), 3);
    EXPECT_FLOAT_EQ(boxes[0].rect.right(), 119.99988);
    EXPECT_FLOAT_EQ(boxes[1].rect.right(), 140.78986);
    EXPECT_FLOAT_EQ(boxes[2].rect.right(), 226.28981);
}

OHOS::Rosen::SPText::ParagraphImpl* ProcessRelayout(std::shared_ptr<Paragraph> paragraph, std::optional<RSBrush> brush)
{
    std::vector<OHOS::Rosen::SPText::TextStyle> textStyles;
    ParagraphStyle paragraphStyle;
    OHOS::Rosen::SPText::TextStyle changeTextStyle;
    changeTextStyle.foreground = SPText::PaintRecord(brush, Drawing::Pen());
    changeTextStyle.relayoutChangeBitmap.set(static_cast<size_t>(RelayoutTextStyleAttribute::FOREGROUND_BRUSH));
    textStyles.push_back(changeTextStyle);
    paragraph->Relayout(200, paragraphStyle, textStyles);
    return ParagraphTest::GetParagraphImpl(paragraph);
}

/*
 * @tc.name: ParagraphTestRelayoutBrush001
 * @tc.desc: test for relayout foreground brush with multiple textstyle
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTestRelayoutBrush001, TestSize.Level0)
{
    ParagraphStyle paragraphStyle;
    auto fontCollection = std::make_shared<FontCollection>();
    ASSERT_NE(fontCollection, nullptr);
    fontCollection->SetupDefaultFontManager();
    std::unique_ptr<ParagraphBuilder> paragraphBuilder = ParagraphBuilder::Create(paragraphStyle, fontCollection);
    ASSERT_NE(paragraphBuilder, nullptr);

    OHOS::Rosen::SPText::TextStyle style;
    style.foreground = SPText::PaintRecord(Drawing::Brush(Drawing::Color::COLOR_DKGRAY), Drawing::Pen());
    paragraphBuilder->PushStyle(style);
    paragraphBuilder->AddText(text_);
    OHOS::Rosen::SPText::TextStyle style1;
    paragraphBuilder->PushStyle(style1);
    paragraphBuilder->AddText(text_);
    std::shared_ptr<Paragraph> paragraph = paragraphBuilder->Build();
    ASSERT_NE(paragraph, nullptr);
    paragraph->Layout(200);

    Drawing::Brush brush(Drawing::Color::COLOR_MAGENTA);
    OHOS::Rosen::SPText::ParagraphImpl* paragraphImpl = ProcessRelayout(paragraph, brush);
    EXPECT_EQ(paragraphImpl->paints_.size(), 3);
    EXPECT_EQ(paragraphImpl->paints_[1].brush.value().GetColor(), Drawing::Color::COLOR_MAGENTA);
    EXPECT_EQ(paragraphImpl->paints_[2].brush.value().GetColor(), Drawing::Color::COLOR_MAGENTA);
    EXPECT_EQ(paragraphImpl->paints_[2].pen.has_value(), false);

    auto skiaTextStyles = paragraphImpl->paragraph_->exportTextStyles();
    EXPECT_EQ(skiaTextStyles.size(), 2);
    EXPECT_EQ(skiaTextStyles[0].fStyle.hasForeground(), true);
    EXPECT_EQ(skiaTextStyles[1].fStyle.hasForeground(), true);
    EXPECT_EQ(std::get<int>(skiaTextStyles[1].fStyle.getForegroundPaintOrID()), 2);
}

/*
 * @tc.name: ParagraphTestRelayoutBrush002
 * @tc.desc: test for relayout foreground brush with symbol textstyle
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTestRelayoutBrush002, TestSize.Level0)
{
    ParagraphStyle paragraphStyle;
    auto fontCollection = std::make_shared<FontCollection>();
    ASSERT_NE(fontCollection, nullptr);
    fontCollection->SetupDefaultFontManager();
    std::unique_ptr<ParagraphBuilder> paragraphBuilder = ParagraphBuilder::Create(paragraphStyle, fontCollection);
    ASSERT_NE(paragraphBuilder, nullptr);

    OHOS::Rosen::SPText::TextStyle style;
    style.foreground = SPText::PaintRecord(Drawing::Brush(Drawing::Color::COLOR_DKGRAY), Drawing::Pen());
    paragraphBuilder->PushStyle(style);
    paragraphBuilder->AddText(text_);
    OHOS::Rosen::SPText::TextStyle style1;
    style1.isSymbolGlyph = true;
    RSSColor symbolColor = {1.0, 0, 0, 0};
    style1.symbol.SetRenderColor(symbolColor);
    paragraphBuilder->PushStyle(style1);
    paragraphBuilder->AddText(text_);
    std::shared_ptr<Paragraph> paragraph = paragraphBuilder->Build();
    ASSERT_NE(paragraph, nullptr);
    paragraph->Layout(200);

    Drawing::Brush brush(Drawing::Color::COLOR_MAGENTA);
    OHOS::Rosen::SPText::ParagraphImpl* paragraphImpl = ProcessRelayout(paragraph, brush);
    EXPECT_EQ(paragraphImpl->paints_.size(), 3);
    EXPECT_EQ(paragraphImpl->paints_[1].brush.value().GetColor(), Drawing::Color::COLOR_MAGENTA);
    EXPECT_EQ(paragraphImpl->paints_[2].brush.has_value(), false);
}

/*
 * @tc.name: ParagraphTestRelayoutBrush003
 * @tc.desc: test for relayout foreground with nullopt
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTestRelayoutBrush003, TestSize.Level0)
{
    ParagraphStyle paragraphStyle;
    auto fontCollection = std::make_shared<FontCollection>();
    ASSERT_NE(fontCollection, nullptr);
    fontCollection->SetupDefaultFontManager();
    std::unique_ptr<ParagraphBuilder> paragraphBuilder = ParagraphBuilder::Create(paragraphStyle, fontCollection);
    ASSERT_NE(paragraphBuilder, nullptr);

    OHOS::Rosen::SPText::TextStyle style;
    style.foreground = SPText::PaintRecord(Drawing::Brush(Drawing::Color::COLOR_DKGRAY), Drawing::Pen());
    paragraphBuilder->PushStyle(style);
    paragraphBuilder->AddText(text_);
    OHOS::Rosen::SPText::TextStyle style1;
    paragraphBuilder->PushStyle(style1);
    paragraphBuilder->AddText(text_);
    std::shared_ptr<Paragraph> paragraph = paragraphBuilder->Build();
    ASSERT_NE(paragraph, nullptr);
    paragraph->Layout(200);

    OHOS::Rosen::SPText::ParagraphImpl* paragraphImpl = ProcessRelayout(paragraph, std::nullopt);

    EXPECT_EQ(paragraphImpl->paints_.size(), 3);
    EXPECT_EQ(paragraphImpl->paints_[0].brush.has_value(), false);
    EXPECT_EQ(paragraphImpl->paints_[1].brush.has_value(), false);
}

/*
 * @tc.name: ParagraphTestTextEffect001
 * @tc.desc: test for text effect state
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTestTextEffect001, TestSize.Level0)
{
    Canvas canvas;
    paragraph_->Paint(&canvas, 0.0, 0.0);
    OHOS::Rosen::SPText::ParagraphImpl* paragraphImpl = GetParagraphImpl(paragraph_);
    ASSERT_NE(paragraphImpl, nullptr);
    std::unique_ptr<skt::Paragraph> temp = nullptr;
    paragraphImpl->paragraph_.swap(temp);
    EXPECT_EQ(paragraphImpl->GetTextBlobRecordInfo().size(), 0);
    paragraphImpl->SetSkipTextBlobDrawing(true);
    EXPECT_FALSE(paragraphImpl->HasSkipTextBlobDrawing());

    paragraphImpl->paragraph_.swap(temp);
    EXPECT_NE(paragraphImpl->GetTextBlobRecordInfo().size(), 0);
    paragraphImpl->SetSkipTextBlobDrawing(true);
    EXPECT_TRUE(paragraphImpl->HasSkipTextBlobDrawing());
}

/*
 * @tc.name: ParagraphTestCharacterIndexNullParaUtf8001
 * @tc.desc: test for GetCharacterPositionAtCoordinate with null paragraph_ and UTF8 encoding
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTestCharacterIndexNullParaUtf8001, TestSize.Level0)
{
    OHOS::Rosen::SPText::ParagraphImpl* paragraphImpl = GetParagraphImpl(paragraph_);
    ASSERT_NE(paragraphImpl, nullptr);

    std::unique_ptr<skt::Paragraph> originalPara = nullptr;
    paragraphImpl->paragraph_.swap(originalPara);

    auto result = paragraphImpl->GetCharacterPositionAtCoordinate(50.0, 10.0, SPText::TextEncoding::UTF8);
    EXPECT_EQ(result.position, 0);

    paragraphImpl->paragraph_.swap(originalPara);
}

/*
 * @tc.name: ParagraphTestCharRangeForGlyphNullParaWithActualRange001
 * @tc.desc: test for GetCharacterRangeForGlyphRange with null paragraph_ and valid actualRange
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTestCharRangeForGlyphNullParaWithActualRange001, TestSize.Level0)
{
    OHOS::Rosen::SPText::ParagraphImpl* paragraphImpl = GetParagraphImpl(paragraph_);
    ASSERT_NE(paragraphImpl, nullptr);

    std::unique_ptr<skt::Paragraph> originalPara = nullptr;
    paragraphImpl->paragraph_.swap(originalPara);

    SPText::Range<size_t> actualRange;
    auto result = paragraphImpl->GetCharacterRangeForGlyphRange(0, 5, &actualRange, SPText::TextEncoding::UTF8);
    EXPECT_EQ(result.start, 0);
    EXPECT_EQ(result.end, 0);

    paragraphImpl->paragraph_.swap(originalPara);
}

/*
 * @tc.name: ParagraphTestCharRangeForGlyphNullParaWithNullActualRange001
 * @tc.desc: test for GetCharacterRangeForGlyphRange with null paragraph_ and nullptr actualRange
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTestCharRangeForGlyphNullParaWithNullActualRange001, TestSize.Level0)
{
    OHOS::Rosen::SPText::ParagraphImpl* paragraphImpl = GetParagraphImpl(paragraph_);
    ASSERT_NE(paragraphImpl, nullptr);

    std::unique_ptr<skt::Paragraph> originalPara = nullptr;
    paragraphImpl->paragraph_.swap(originalPara);

    auto result = paragraphImpl->GetCharacterRangeForGlyphRange(0, 5, nullptr, SPText::TextEncoding::UTF8);
    EXPECT_EQ(result.start, 0);
    EXPECT_EQ(result.end, 0);

    paragraphImpl->paragraph_.swap(originalPara);
}

/*
 * @tc.name: ParagraphTestGlyphRangeForCharNullParaWithActualRange001
 * @tc.desc: test for GetGlyphRangeForCharacterRange with null paragraph_ and valid actualRange
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTestGlyphRangeForCharNullParaWithActualRange001, TestSize.Level0)
{
    OHOS::Rosen::SPText::ParagraphImpl* paragraphImpl = GetParagraphImpl(paragraph_);
    ASSERT_NE(paragraphImpl, nullptr);

    std::unique_ptr<skt::Paragraph> originalPara = nullptr;
    paragraphImpl->paragraph_.swap(originalPara);

    SPText::Range<size_t> actualRange;
    auto result = paragraphImpl->GetGlyphRangeForCharacterRange(0, 5, &actualRange, SPText::TextEncoding::UTF8);
    EXPECT_EQ(result.start, 0);
    EXPECT_EQ(result.end, 0);

    paragraphImpl->paragraph_.swap(originalPara);
}

/*
 * @tc.name: ParagraphTestGlyphRangeForCharNullParaWithNullActualRange001
 * @tc.desc: test for GetGlyphRangeForCharacterRange with null paragraph_ and nullptr actualRange
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTestGlyphRangeForCharNullParaWithNullActualRange001, TestSize.Level0)
{
    OHOS::Rosen::SPText::ParagraphImpl* paragraphImpl = GetParagraphImpl(paragraph_);
    ASSERT_NE(paragraphImpl, nullptr);

    std::unique_ptr<skt::Paragraph> originalPara = nullptr;
    paragraphImpl->paragraph_.swap(originalPara);

    auto result = paragraphImpl->GetGlyphRangeForCharacterRange(0, 5, nullptr, SPText::TextEncoding::UTF8);
    EXPECT_EQ(result.start, 0);
    EXPECT_EQ(result.end, 0);

    paragraphImpl->paragraph_.swap(originalPara);
}

/*
 * @tc.name: ParagraphTestCacheTab001
 * @tc.desc: text with tab characters must not create any paragraph cache entry
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTestCacheTab001, TestSize.Level0)
{
    std::shared_ptr<FontCollection> fontCollection = std::make_shared<FontCollection>();
    ASSERT_NE(fontCollection, nullptr);
    fontCollection->SetupDefaultFontManager();

    // tab location >= 1.0 substitutes '\t' with ' ' during layout
    std::shared_ptr<Paragraph> replaced = BuildParagraphWithTab(fontCollection,
        OHOS::Rosen::SPText::TextTab(OHOS::Rosen::SPText::TextAlign::LEFT, 4.0f), u"Left\tRight");
    ASSERT_NE(replaced, nullptr);
    replaced->Layout(200.0);
    skia::textlayout::ParagraphImpl* replacedImpl = GetParagraphSkiaImpl(replaced);
    EXPECT_EQ(replacedImpl->fState, skia::textlayout::kFormatted);
    skia::textlayout::ParagraphCache* cache = replacedImpl->fontCollection()->getParagraphCache();
    ASSERT_NE(cache, nullptr);
    EXPECT_EQ(cache->count(), 0);

    // tab location < 1.0 keeps '\t' in the text; both variants must stay out of the cache,
    // otherwise keys with tab and without tab coexist for the same input text
    std::shared_ptr<Paragraph> kept = BuildParagraphWithTab(fontCollection,
        OHOS::Rosen::SPText::TextTab(OHOS::Rosen::SPText::TextAlign::LEFT, -1.0f), u"Left\tRight");
    ASSERT_NE(kept, nullptr);
    kept->Layout(200.0);
    skia::textlayout::ParagraphImpl* keptImpl = GetParagraphSkiaImpl(kept);
    EXPECT_EQ(keptImpl->fState, skia::textlayout::kFormatted);
    EXPECT_EQ(cache->count(), 0);
}

/*
 * @tc.name: ParagraphTestCacheTab002
 * @tc.desc: text without tab characters is still stored in and served from the paragraph cache
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTestCacheTab002, TestSize.Level0)
{
    std::shared_ptr<FontCollection> fontCollection = std::make_shared<FontCollection>();
    ASSERT_NE(fontCollection, nullptr);
    fontCollection->SetupDefaultFontManager();
    const OHOS::Rosen::SPText::TextTab tab(OHOS::Rosen::SPText::TextAlign::LEFT, 1.5f);

    std::shared_ptr<Paragraph> first = BuildParagraphWithTab(fontCollection, tab, u"LeftRight");
    ASSERT_NE(first, nullptr);
    first->Layout(200.0);
    skia::textlayout::ParagraphImpl* firstImpl = GetParagraphSkiaImpl(first);
    EXPECT_EQ(firstImpl->fState, skia::textlayout::kFormatted);
    skia::textlayout::ParagraphCache* cache = firstImpl->fontCollection()->getParagraphCache();
    ASSERT_NE(cache, nullptr);
    EXPECT_EQ(cache->count(), 1);

    // identical paragraph reuses the cached entry instead of adding another one
    std::shared_ptr<Paragraph> second = BuildParagraphWithTab(fontCollection, tab, u"LeftRight");
    ASSERT_NE(second, nullptr);
    second->Layout(200.0);
    EXPECT_EQ(GetParagraphSkiaImpl(second)->fState, skia::textlayout::kFormatted);
    EXPECT_EQ(cache->count(), 1);
}

/*
 * @tc.name: ParagraphTestCacheTab003
 * @tc.desc: different tab stop locations must not share one paragraph cache entry
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphTestCacheTab003, TestSize.Level0)
{
    std::shared_ptr<FontCollection> fontCollection = std::make_shared<FontCollection>();
    ASSERT_NE(fontCollection, nullptr);
    fontCollection->SetupDefaultFontManager();

    std::shared_ptr<Paragraph> first = BuildParagraphWithTab(fontCollection,
        OHOS::Rosen::SPText::TextTab(OHOS::Rosen::SPText::TextAlign::LEFT, 1.5f), u"LeftRight");
    ASSERT_NE(first, nullptr);
    first->Layout(200.0);
    skia::textlayout::ParagraphImpl* firstImpl = GetParagraphSkiaImpl(first);
    skia::textlayout::ParagraphCache* cache = firstImpl->fontCollection()->getParagraphCache();
    ASSERT_NE(cache, nullptr);
    EXPECT_EQ(cache->count(), 1);

    // same text but a different tab location needs a separate cache entry,
    // otherwise the cached layout is reused with wrong tab stop positions
    std::shared_ptr<Paragraph> second = BuildParagraphWithTab(fontCollection,
        OHOS::Rosen::SPText::TextTab(OHOS::Rosen::SPText::TextAlign::LEFT, 2.5f), u"LeftRight");
    ASSERT_NE(second, nullptr);
    second->Layout(200.0);
    EXPECT_EQ(cache->count(), 2);
}

/*
 * @tc.name: ParagraphBorderLayout001
 * @tc.desc: test for border width raising the line height by 2w when the bordered run decides it
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphBorderLayout001, TestSize.Level0)
{
    constexpr double BORDER_WIDTH = 4.0;   // 4.0 just for test
    constexpr double LAYOUT_WIDTH = 300.0; // 300 just for test
    auto plain = BuildParagraphWithBorder(0.0, u"border text");
    auto bordered = BuildParagraphWithBorder(BORDER_WIDTH, u"border text");
    ASSERT_NE(plain, nullptr);
    ASSERT_NE(bordered, nullptr);
    plain->Layout(LAYOUT_WIDTH);
    bordered->Layout(LAYOUT_WIDTH);
    EXPECT_FLOAT_EQ(
        static_cast<float>(bordered->GetHeight() - plain->GetHeight()), 2 * static_cast<float>(BORDER_WIDTH));
}

/*
 * @tc.name: ParagraphBorderWrap001
 * @tc.desc: test for border gap participating in line breaking width
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphBorderWrap001, TestSize.Level0)
{
    constexpr double BORDER_WIDTH = 4.0;   // 4.0 just for test
    constexpr double WIDE_LAYOUT = 1000.0; // 1000 just for test
    auto plain = BuildParagraphWithBorder(0.0, u"border text border text");
    auto bordered = BuildParagraphWithBorder(BORDER_WIDTH, u"border text border text");
    ASSERT_NE(plain, nullptr);
    ASSERT_NE(bordered, nullptr);
    plain->Layout(WIDE_LAYOUT);
    auto implPlain = GetParagraphSkiaImpl(plain);
    ASSERT_NE(implPlain, nullptr);
    ASSERT_EQ(implPlain->lines().size(), 1u);        // sanity: single line without border
    const auto tightWidth = plain->GetLongestLine(); // exact single-line width
    plain->Layout(tightWidth);
    bordered->Layout(tightWidth);
    auto implBordered = GetParagraphSkiaImpl(bordered);
    ASSERT_NE(implBordered, nullptr);
    EXPECT_EQ(implBordered->lines().size(), 2u); // enter gap w forces an earlier break
}

/*
 * @tc.name: ParagraphBorderIntrinsic001
 * @tc.desc: test for border gap participating in min intrinsic width
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphBorderIntrinsic001, TestSize.Level0)
{
    constexpr double BORDER_WIDTH = 4.0;    // 4.0 just for test
    constexpr double NARROW_LAYOUT = 100.0; // 100 just for test: wrap so intrinsic is measured
    auto plain = BuildParagraphWithBorder(0.0, u"border text border text");
    auto bordered = BuildParagraphWithBorder(BORDER_WIDTH, u"border text border text");
    ASSERT_NE(plain, nullptr);
    ASSERT_NE(bordered, nullptr);
    plain->Layout(NARROW_LAYOUT);
    bordered->Layout(NARROW_LAYOUT);
    ASSERT_GT(plain->GetMinIntrinsicWidth(), 0.0); // sanity: measurement path ran
    EXPECT_FLOAT_EQ(static_cast<float>(bordered->GetMinIntrinsicWidth() - plain->GetMinIntrinsicWidth()),
        static_cast<float>(BORDER_WIDTH));
}

/*
 * @tc.name: ParagraphBorderGap001
 * @tc.desc: test for border gap materialized outside the box and counted in line width
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphBorderGap001, TestSize.Level0)
{
    constexpr double BORDER_WIDTH = 4.0;   // 4.0 just for test
    constexpr double LAYOUT_WIDTH = 300.0; // 300 just for test
    auto plain = BuildParagraphWithBorder(0.0, u"border text");
    auto bordered = BuildParagraphWithBorder(BORDER_WIDTH, u"border text");
    ASSERT_NE(plain, nullptr);
    ASSERT_NE(bordered, nullptr);
    plain->Layout(LAYOUT_WIDTH);
    bordered->Layout(LAYOUT_WIDTH);
    auto implPlain = GetParagraphSkiaImpl(plain);
    auto implBordered = GetParagraphSkiaImpl(bordered);
    ASSERT_NE(implPlain, nullptr);
    ASSERT_NE(implBordered, nullptr);
    ASSERT_EQ(implPlain->lines().size(), 1u);
    ASSERT_EQ(implBordered->lines().size(), 1u);
    // line width grows by exactly 2w (enter + trailing gaps)
    const auto widthDelta =
        implBordered->lines()[0].widthWithoutEllipsis() - implPlain->lines()[0].widthWithoutEllipsis();
    EXPECT_FLOAT_EQ(widthDelta, 2 * static_cast<SkScalar>(BORDER_WIDTH));
    // box clips: left edge shifts by w (gap outside the box), box width unchanged
    SkScalar plainLeft = -1.0f, plainRight = -1.0f;
    SkScalar borderLeft = -1.0f, borderRight = -1.0f;
    auto collectClip = [](skt::TextLine& line, SkScalar& left, SkScalar& right) {
        line.scanStyles(skt::StyleType::kBackground, [&left, &right](skt::TextRange range, const skt::TextStyle& style,
                                                         const skt::TextLine::ClipContext& context) {
            left = context.clip.fLeft;
            right = context.clip.fRight;
        });
    };
    collectClip(implPlain->lines()[0], plainLeft, plainRight);
    collectClip(implBordered->lines()[0], borderLeft, borderRight);
    EXPECT_FLOAT_EQ(borderLeft, plainLeft + static_cast<SkScalar>(BORDER_WIDTH));
    EXPECT_FLOAT_EQ(borderRight - borderLeft, plainRight - plainLeft);
}

/*
 * @tc.name: ParagraphBorderRectsForRange001
 * @tc.desc: test for getRectsForRange covering the right border of a box followed by text
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphBorderRectsForRange001, TestSize.Level0)
{
    constexpr double BORDER_WIDTH = 4.0;   // 4.0 just for test
    constexpr double LAYOUT_WIDTH = 300.0; // 300 just for test: single line
    constexpr size_t BOX_END = 11;         // "border text" length
    constexpr size_t FULL_END = 16;        // "border text tail" length
    ParagraphStyle paragraphStyle;
    paragraphStyle.ellipsis = u"";
    auto fontCollection = std::make_shared<FontCollection>();
    fontCollection->SetupDefaultFontManager();
    auto builder = ParagraphBuilder::Create(paragraphStyle, fontCollection);
    ASSERT_NE(builder, nullptr);
    SPText::TextStyle style;
    style.backgroundRect.color = 0xFF00FF00;       // 0xFF00FF00 just for test
    style.backgroundRect.borderColor = 0xFFFF0000; // 0xFFFF0000 just for test
    style.backgroundRect.borderWidth = BORDER_WIDTH;
    builder->PushStyle(style); // "border text": border box
    builder->AddText(u"border text");
    SPText::TextStyle plainStyle;
    builder->PushStyle(plainStyle); // " tail": plain text after the box
    builder->AddText(u" tail");
    builder->Pop();
    std::shared_ptr<Paragraph> paragraph = builder->Build();
    ASSERT_NE(paragraph, nullptr);
    paragraph->Layout(LAYOUT_WIDTH);
    auto impl = GetParagraphSkiaImpl(paragraph);
    ASSERT_NE(impl, nullptr);
    ASSERT_EQ(impl->lines().size(), 1u);
    // the box's painted clip from the background path (left edge hugs the text)
    SkScalar boxLeft = -1.0f;
    SkScalar boxRight = -1.0f;
    impl->lines()[0].scanStyles(skt::StyleType::kBackground,
        [&boxLeft, &boxRight](skt::TextRange range, const skt::TextStyle& style,
                              const skt::TextLine::ClipContext& context) {
            if (style.getBackgroundRect().borderWidth <= 0) {
                return;
            }
            boxLeft = context.clip.fLeft;
            boxRight = context.clip.fRight;
        });
    EXPECT_GT(boxLeft, 0.0f); // sanity: the clip was collected
    // the range over the box spans both border gaps: left edge before the box, right
    // edge past the right border (the left side was already covered before the fix)
    auto boxes = paragraph->GetRectsForRange(0, BOX_END, RectHeightStyle::TIGHT, RectWidthStyle::TIGHT);
    ASSERT_EQ(boxes.size(), 1u);
    EXPECT_FLOAT_EQ(boxes[0].rect.left(), boxLeft - static_cast<SkScalar>(BORDER_WIDTH));
    EXPECT_FLOAT_EQ(boxes[0].rect.right(), boxRight + static_cast<SkScalar>(BORDER_WIDTH));
    // regression: the whole-line range still ends at the line advance (the box's exit
    // gap was charged to the following space and shifts the tail, no extra extension)
    auto fullBoxes = paragraph->GetRectsForRange(0, FULL_END, RectHeightStyle::TIGHT, RectWidthStyle::TIGHT);
    ASSERT_EQ(fullBoxes.size(), 1u);
    EXPECT_FLOAT_EQ(fullBoxes[0].rect.right(), impl->lines()[0].widthWithoutEllipsis());
}

/*
 * @tc.name: ParagraphBorderRectsForRange002
 * @tc.desc: test for getRectsForRange covering the right border of a box at line end
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphBorderRectsForRange002, TestSize.Level0)
{
    constexpr double BORDER_WIDTH = 4.0;   // 4.0 just for test
    constexpr double LAYOUT_WIDTH = 300.0; // 300 just for test: single line, box at line end
    constexpr size_t BOX_END = 11;         // "border text" length
    auto bordered = BuildParagraphWithBorder(BORDER_WIDTH, u"border text");
    ASSERT_NE(bordered, nullptr);
    bordered->Layout(LAYOUT_WIDTH);
    auto impl = GetParagraphSkiaImpl(bordered);
    ASSERT_NE(impl, nullptr);
    ASSERT_EQ(impl->lines().size(), 1u);
    // the line advance counts both gaps (enter + trailing), so the rect over the box
    // spans the full visual border box from the paragraph origin to the line width
    auto boxes = bordered->GetRectsForRange(0, BOX_END, RectHeightStyle::TIGHT, RectWidthStyle::TIGHT);
    ASSERT_EQ(boxes.size(), 1u);
    EXPECT_FLOAT_EQ(boxes[0].rect.left(), 0.0f);
    EXPECT_FLOAT_EQ(boxes[0].rect.right(), impl->lines()[0].widthWithoutEllipsis());
    EXPECT_FLOAT_EQ(boxes[0].rect.width(), impl->lines()[0].widthWithoutEllipsis());
}

/*
 * @tc.name: ParagraphBorderRectsForRange003
 * @tc.desc: test for getRectsForRange not extending inside a border box
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphBorderRectsForRange003, TestSize.Level0)
{
    constexpr double BORDER_WIDTH = 10.0;  // 10.0 just for test: larger than one glyph
                                           // advance, so a wrong extension would push
                                           // the right edge past the box clip
    constexpr double LAYOUT_WIDTH = 300.0; // 300 just for test: single line
    constexpr size_t MID_START = 1;        // skip the box's first cluster
    constexpr size_t MID_END = 10;         // stop before the box's last cluster
    auto bordered = BuildParagraphWithBorder(BORDER_WIDTH, u"border text");
    ASSERT_NE(bordered, nullptr);
    bordered->Layout(LAYOUT_WIDTH);
    auto impl = GetParagraphSkiaImpl(bordered);
    ASSERT_NE(impl, nullptr);
    ASSERT_EQ(impl->lines().size(), 1u);
    SkScalar boxLeft = -1.0f;
    SkScalar boxRight = -1.0f;
    impl->lines()[0].scanStyles(skt::StyleType::kBackground,
        [&boxLeft, &boxRight](skt::TextRange range, const skt::TextStyle& style,
                              const skt::TextLine::ClipContext& context) {
            if (style.getBackgroundRect().borderWidth <= 0) {
                return;
            }
            boxLeft = context.clip.fLeft;
            boxRight = context.clip.fRight;
        });
    EXPECT_GT(boxLeft, 0.0f); // sanity: the clip was collected
    // a range that stops inside the box hugs its own glyphs: no border gap on either
    // side (the entering gap of the interior cluster is zero, no exit border there)
    auto boxes = bordered->GetRectsForRange(MID_START, MID_END, RectHeightStyle::TIGHT, RectWidthStyle::TIGHT);
    ASSERT_EQ(boxes.size(), 1u);
    EXPECT_GT(boxes[0].rect.left(), boxLeft);
    EXPECT_LT(boxes[0].rect.right(), boxRight);
}

/*
 * @tc.name: ParagraphBorderRectsForRange004
 * @tc.desc: test for getRectsForRange staying unchanged without a border
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphBorderRectsForRange004, TestSize.Level0)
{
    constexpr double LAYOUT_WIDTH = 300.0; // 300 just for test: single line
    constexpr size_t BOX_END = 11;         // "border text" length
    auto plain = BuildParagraphWithBorder(0.0, u"border text");
    ASSERT_NE(plain, nullptr);
    plain->Layout(LAYOUT_WIDTH);
    auto impl = GetParagraphSkiaImpl(plain);
    ASSERT_NE(impl, nullptr);
    ASSERT_EQ(impl->lines().size(), 1u);
    auto boxes = plain->GetRectsForRange(0, BOX_END, RectHeightStyle::TIGHT, RectWidthStyle::TIGHT);
    ASSERT_EQ(boxes.size(), 1u);
    EXPECT_FLOAT_EQ(boxes[0].rect.left(), 0.0f);
    EXPECT_FLOAT_EQ(boxes[0].rect.right(), impl->lines()[0].widthWithoutEllipsis());
}

/*
 * @tc.name: ParagraphBorderRectsForRange005
 * @tc.desc: test for getRectsForRange covering the right border of a box cut across lines
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphBorderRectsForRange005, TestSize.Level0)
{
    constexpr double BORDER_WIDTH = 4.0;  // 4.0 just for test
    constexpr double LAYOUT_WIDTH = 40.0; // 40 just for test: wraps CJK text (no spaces)
                                          // into multiple lines, each cut of the box
                                          // carries its own right border
    constexpr size_t FULL_END = 8;        // "你好世界你好世界" length
    auto bordered = BuildParagraphWithBorder(BORDER_WIDTH, u"你好世界你好世界");
    ASSERT_NE(bordered, nullptr);
    bordered->Layout(LAYOUT_WIDTH);
    auto impl = GetParagraphSkiaImpl(bordered);
    ASSERT_NE(impl, nullptr);
    ASSERT_GT(impl->lines().size(), 1u);
    auto boxes = bordered->GetRectsForRange(0, FULL_END, RectHeightStyle::TIGHT, RectWidthStyle::TIGHT);
    ASSERT_EQ(boxes.size(), impl->lines().size());
    for (size_t i = 0; i < impl->lines().size(); ++i) {
        SkScalar boxRight = -1.0f;
        impl->lines()[i].scanStyles(skt::StyleType::kBackground,
            [&boxRight](skt::TextRange range, const skt::TextStyle& style,
                        const skt::TextLine::ClipContext& context) {
                if (style.getBackgroundRect().borderWidth <= 0) {
                    return;
                }
                if (boxRight < 0.0f || context.clip.fRight > boxRight) {
                    boxRight = context.clip.fRight;
                }
            });
        EXPECT_GT(boxRight, 0.0f); // sanity: the clip was collected
        // every line's cut of the box ends with its own right border
        EXPECT_FLOAT_EQ(boxes[i].rect.right(), boxRight + static_cast<SkScalar>(BORDER_WIDTH));
    }
}

/*
 * @tc.name: ParagraphBorderEllipsis001
 * @tc.desc: test for tail ellipsis subtracting border gaps of removed clusters
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphBorderEllipsis001, TestSize.Level0)
{
    constexpr double BORDER_WIDTH = 4.0;   // 4.0 just for test
    constexpr double LAYOUT_WIDTH = 137.0; // 137 just for test (calibrated): line 1 spans both
                                           // border segments while the tail ellipsis removes the
                                           // whole trailing segment (multiple clusters)
    constexpr size_t SEGMENT_A_END = 12;   // "border text " length: kept text must stay in segment A
    ParagraphStyle paragraphStyle;
    paragraphStyle.maxLines = 1;
    paragraphStyle.ellipsis = u"...";
    auto fontCollection = std::make_shared<FontCollection>();
    fontCollection->SetupDefaultFontManager();
    auto builder = ParagraphBuilder::Create(paragraphStyle, fontCollection);
    ASSERT_NE(builder, nullptr);
    SPText::TextStyle style;
    style.backgroundRect.color = 0xFF00FF00;       // 0xFF00FF00 just for test
    style.backgroundRect.borderColor = 0xFFFF0000; // 0xFFFF0000 just for test
    style.backgroundRect.borderWidth = BORDER_WIDTH;
    builder->PushStyle(style); // "border text ": border segment A
    builder->AddText(u"border text ");
    style.backgroundRect.borderColor = 0xFF0000FF; // 0xFF0000FF just for test: a distinct color
                                                   // starts border segment B so its enter gap is
                                                   // carried by B's first cluster
    builder->PushStyle(style);                     // "border text": border segment B
    builder->AddText(u"border text");
    builder->Pop();
    std::shared_ptr<Paragraph> paragraph = builder->Build();
    ASSERT_NE(paragraph, nullptr);
    paragraph->Layout(LAYOUT_WIDTH);
    auto impl = GetParagraphSkiaImpl(paragraph);
    ASSERT_NE(impl, nullptr);
    ASSERT_EQ(impl->lines().size(), 1u);
    auto& line = impl->lines()[0];
    ASSERT_NE(line.ellipsis(), nullptr);
    // the removal must have crossed segment B (its enter gap is the residue under test)
    ASSERT_LE(line.text().end, SEGMENT_A_END);
    // surviving boxes keep their materialized gaps: leftmost box starts at its enter gap, and
    // the line width ends exactly one trailing gap past the rightmost kept box edge (gaps of
    // clusters removed by the ellipsis must be gone; the ellipsis run's own box starts past
    // the kept text and is excluded)
    SkScalar boxLeft = -1.0f;
    SkScalar boxRight = -1.0f;
    const auto keptTextEnd = line.text().end;
    line.scanStyles(
        skt::StyleType::kBackground, [&boxLeft, &boxRight, keptTextEnd](skt::TextRange range,
                                         const skt::TextStyle& style, const skt::TextLine::ClipContext& context) {
            if (range.start >= keptTextEnd) {
                return;
            }
            if (boxLeft < 0.0f || context.clip.fLeft < boxLeft) {
                boxLeft = context.clip.fLeft;
            }
            if (context.clip.fRight > boxRight) {
                boxRight = context.clip.fRight;
            }
        });
    EXPECT_FLOAT_EQ(boxLeft, static_cast<SkScalar>(BORDER_WIDTH));
    EXPECT_FLOAT_EQ(line.widthWithoutEllipsis(), boxRight + static_cast<SkScalar>(BORDER_WIDTH));
}

/*
 * @tc.name: ParagraphBorderJustify001
 * @tc.desc: test for justify keeping border gaps fixed and filling the layout width
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphBorderJustify001, TestSize.Level0)
{
    constexpr double BORDER_WIDTH = 4.0;          // 4.0 just for test
    constexpr double LAYOUT_WIDTH = 160.0;        // 160 just for test (calibrated): wraps the text
                                                  // into 2 lines, the first one holding border boxes
    constexpr double PLAIN_LAYOUT_WIDTH = 152.0;  // LAYOUT_WIDTH - 2 * BORDER_WIDTH: the no-border
                                                  // control keeps the same first-line words
    constexpr double SINGLE_LAYOUT_WIDTH = 300.0; // 300 just for test: keeps "border text" on one line
    auto collectClip = [](skt::TextLine& line, SkScalar& left, SkScalar& right) {
        line.scanStyles(skt::StyleType::kBackground, [&left, &right](skt::TextRange range, const skt::TextStyle& style,
                                                         const skt::TextLine::ClipContext& context) {
            left = context.clip.fLeft;
            right = context.clip.fRight;
        });
    };
    // a single line is also the last line: justify must leave it untouched
    auto single = BuildJustifyParagraphWithBorder(SPText::TextAlign::JUSTIFY, BORDER_WIDTH, u"border text");
    ASSERT_NE(single, nullptr);
    single->Layout(SINGLE_LAYOUT_WIDTH);
    auto implSingle = GetParagraphSkiaImpl(single);
    ASSERT_NE(implSingle, nullptr);
    ASSERT_EQ(implSingle->lines().size(), 1u);
    SkScalar singleLeft = -1.0f, singleRight = -1.0f;
    collectClip(implSingle->lines()[0], singleLeft, singleRight);
    // untouched last line keeps the Task 4 gap geometry: enter gap left of the box,
    // line width one trailing gap past the box
    EXPECT_FLOAT_EQ(singleLeft, static_cast<SkScalar>(BORDER_WIDTH));
    EXPECT_FLOAT_EQ(implSingle->lines()[0].widthWithoutEllipsis(), singleRight + static_cast<SkScalar>(BORDER_WIDTH));
    // 2+ lines: every line but the last one gets stretched by justify
    const std::u16string text = u"border text border text border text";
    auto wrapped = BuildJustifyParagraphWithBorder(SPText::TextAlign::JUSTIFY, BORDER_WIDTH, text);
    ASSERT_NE(wrapped, nullptr);
    wrapped->Layout(LAYOUT_WIDTH);
    auto implWrapped = GetParagraphSkiaImpl(wrapped);
    ASSERT_NE(implWrapped, nullptr);
    ASSERT_GT(implWrapped->lines().size(), 1u);
    // the justified line fills the layout width exactly (gaps included in the base)
    EXPECT_FLOAT_EQ(implWrapped->lines()[0].widthWithoutEllipsis(), static_cast<SkScalar>(LAYOUT_WIDTH));
    SkScalar clipLeft = -1.0f, clipRight = -1.0f;
    collectClip(implWrapped->lines()[0], clipLeft, clipRight);
    ASSERT_GT(clipRight - clipLeft, 0.0f);
    // the enter gap stays fixed: the box starts exactly one border width in
    EXPECT_FLOAT_EQ(clipLeft, static_cast<SkScalar>(BORDER_WIDTH));
    // the trailing gap stays fixed: the line width ends one border width past the box
    EXPECT_FLOAT_EQ(implWrapped->lines()[0].widthWithoutEllipsis(), clipRight + static_cast<SkScalar>(BORDER_WIDTH));
    // box clip width equals the content-matched no-border justify control: stretching
    // spreads the words only, never the gaps
    auto plain = BuildJustifyParagraphWithBorder(SPText::TextAlign::JUSTIFY, 0.0, text);
    ASSERT_NE(plain, nullptr);
    plain->Layout(PLAIN_LAYOUT_WIDTH);
    auto implPlain = GetParagraphSkiaImpl(plain);
    ASSERT_NE(implPlain, nullptr);
    ASSERT_GT(implPlain->lines().size(), 1u);
    ASSERT_EQ(implPlain->lines()[0].text().start, implWrapped->lines()[0].text().start);
    ASSERT_EQ(implPlain->lines()[0].text().end, implWrapped->lines()[0].text().end);
    SkScalar plainLeft = -1.0f, plainRight = -1.0f;
    collectClip(implPlain->lines()[0], plainLeft, plainRight);
    EXPECT_FLOAT_EQ(clipRight - clipLeft, plainRight - plainLeft);
}

/*
 * @tc.name: ParagraphJustifyBorderlessRegress001
 * @tc.desc: test for hyphenated borderless line staying visually full under justify
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphJustifyBorderlessRegress001, TestSize.Level0)
{
    constexpr double LAYOUT_WIDTH = 88.0; // 88 just for test (calibrated): the first line ends on the
                                          // soft hyphen and keeps free space for justify to spread
    // U+00AD makes the first line end on a soft hyphen: the appended hyphen run advance rides in
    // the line width, and justify must still spread only the true free space (borderless
    // regression anchor for the justify distribution base, no hyphenation dictionary needed)
    auto paragraph =
        BuildJustifyParagraphWithBorder(SPText::TextAlign::JUSTIFY, 0.0, u"word border\u00ADmmmm tail words");
    ASSERT_NE(paragraph, nullptr);
    paragraph->Layout(LAYOUT_WIDTH);
    auto impl = GetParagraphSkiaImpl(paragraph);
    ASSERT_NE(impl, nullptr);
    ASSERT_EQ(impl->lines().size(), 3u);
    auto& hyphenLine = impl->lines()[0];
    ASSERT_TRUE(hyphenLine.getBreakWithHyphen());
    // the hyphenated line fills the layout width and its glyphs reach exactly that far: the
    // hyphen advance is part of the line width, so it must neither be counted as free space
    // nor subtracted twice
    EXPECT_FLOAT_EQ(hyphenLine.widthWithoutEllipsis(), static_cast<SkScalar>(LAYOUT_WIDTH));
    SkScalar clipRight = -1.0f;
    hyphenLine.scanStyles(skt::StyleType::kBackground,
        [&clipRight](skt::TextRange range, const skt::TextStyle& style, const skt::TextLine::ClipContext& context) {
            if (context.clip.fRight > clipRight) {
                clipRight = context.clip.fRight;
            }
        });
    EXPECT_FLOAT_EQ(clipRight, static_cast<SkScalar>(LAYOUT_WIDTH));
    // the last line is never justified and keeps its raw width
    EXPECT_LT(impl->lines()[2].widthWithoutEllipsis(), static_cast<SkScalar>(LAYOUT_WIDTH));
}

/*
 * @tc.name: ParagraphBorderZeroOverhead001
 * @tc.desc: test for borderless paragraphs keeping the lazy empty auto-spacing table
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphBorderZeroOverhead001, TestSize.Level0)
{
    // the SetUp fixture is borderless with auto spacing off and already laid out:
    // its metrics must stay on the ParagraphTest001 baseline values
    EXPECT_EQ(paragraph_->GetHeight(), 19);
    EXPECT_EQ(static_cast<int>(paragraph_->GetLongestLine()), 44);
    // zero overhead: no border and no auto spacing means the materialization loop
    // must not run at all, so every run keeps its empty (unallocated) fAutoSpacings
    auto impl = GetParagraphSkiaImpl(paragraph_);
    ASSERT_NE(impl, nullptr);
    for (auto& run : impl->fRuns) {
        EXPECT_TRUE(run.fAutoSpacings.empty());
    }
}

/*
 * @tc.name: ParagraphBorderCacheClone001
 * @tc.desc: test for the border-cluster flag surviving the paragraph cache and clone paths
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphBorderCacheClone001, TestSize.Level0)
{
    constexpr double BORDER_WIDTH = 4.0;       // 4.0 just for test
    constexpr double LAYOUT_WIDTH = 300.0;     // 300 just for test
    constexpr double CACHE_MISS_WIDTH = 260.0; // 260 just for test: same shape key, other layout width
    constexpr double CLONE_WIDTH = 240.0;      // 240 just for test: differs from every stored layout
    auto fontCollection = std::make_shared<FontCollection>();
    fontCollection->SetupDefaultFontManager();
    auto buildBordered = [&fontCollection]() -> std::shared_ptr<Paragraph> {
        ParagraphStyle paragraphStyle;
        // the adapter defaults to a tail ellipsis, which forces single-line layout
        paragraphStyle.ellipsis = u"";
        auto builder = ParagraphBuilder::Create(paragraphStyle, fontCollection);
        if (builder == nullptr) {
            return nullptr;
        }
        SPText::TextStyle style;
        style.backgroundRect.color = 0xFF00FF00;       // 0xFF00FF00 just for test
        style.backgroundRect.borderColor = 0xFFFF0000; // 0xFFFF0000 just for test
        style.backgroundRect.borderWidth = BORDER_WIDTH;
        builder->PushStyle(style);
        builder->AddText(u"border text");
        builder->Pop();
        return builder->Build();
    };
    auto collectClip = [](skt::TextLine& line, SkScalar& left, SkScalar& right) {
        line.scanStyles(skt::StyleType::kBackground, [&left, &right](skt::TextRange range, const skt::TextStyle& style,
                                                         const skt::TextLine::ClipContext& context) {
            left = context.clip.fLeft;
            right = context.clip.fRight;
        });
    };

    auto first = buildBordered();
    ASSERT_NE(first, nullptr);
    first->Layout(LAYOUT_WIDTH);
    auto implFirst = GetParagraphSkiaImpl(first);
    ASSERT_NE(implFirst, nullptr);
    ASSERT_EQ(implFirst->lines().size(), 1u);
    auto* cache = implFirst->fontCollection()->getParagraphCache();
    ASSERT_NE(cache, nullptr);
    ASSERT_EQ(cache->count(), 1); // shaped result of the first paragraph is stored

    // path A (cache hit): same text and styles reuse the cached clusters (updateTo)
    // but the different layout width misses the cached layout, so line assembly
    // (autoSpacing) runs again on inherited clusters
    auto second = buildBordered();
    ASSERT_NE(second, nullptr);
    second->Layout(CACHE_MISS_WIDTH);
    auto implSecond = GetParagraphSkiaImpl(second);
    ASSERT_NE(implSecond, nullptr);
    ASSERT_EQ(implSecond->lines().size(), 1u);
    EXPECT_FLOAT_EQ(implSecond->lines()[0].widthWithoutEllipsis(), implFirst->lines()[0].widthWithoutEllipsis());
    SkScalar secondLeft = -1.0f, secondRight = -1.0f;
    collectClip(implSecond->lines()[0], secondLeft, secondRight);
    EXPECT_FLOAT_EQ(secondLeft, static_cast<SkScalar>(BORDER_WIDTH));

    // path B (clone): the clone keeps the marked clusters without re-shaping, so its
    // re-layout at a new width re-runs line assembly on the copied cluster table
    std::shared_ptr<Paragraph> cloned = first->CloneSelf();
    ASSERT_NE(cloned, nullptr);
    cloned->Layout(CLONE_WIDTH);
    auto implCloned = GetParagraphSkiaImpl(cloned);
    ASSERT_NE(implCloned, nullptr);
    ASSERT_EQ(implCloned->lines().size(), 1u);
    EXPECT_FLOAT_EQ(cloned->GetHeight(), first->GetHeight());
    EXPECT_FLOAT_EQ(implCloned->lines()[0].widthWithoutEllipsis(), implFirst->lines()[0].widthWithoutEllipsis());
    SkScalar cloneLeft = -1.0f, cloneRight = -1.0f;
    collectClip(implCloned->lines()[0], cloneLeft, cloneRight);
    EXPECT_FLOAT_EQ(cloneLeft, static_cast<SkScalar>(BORDER_WIDTH));

    // both extra paragraphs shared the single cache entry of the same shape key
    EXPECT_EQ(cache->count(), 1);
}

/*
 * @tc.name: ParagraphBorderSplitRun001
 * @tc.desc: test for split runs keeping the border width for the line height reservation
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphBorderSplitRun001, TestSize.Level0)
{
    constexpr double BORDER_WIDTH = 4.0;   // 4.0 just for test
    constexpr double LAYOUT_WIDTH = 160.0; // 160 just for test (calibrated in
                                           // ParagraphBorderJustify001): wraps the text so
                                           // the single shaped run crosses line boundaries
    ParagraphStyle paragraphStyle;
    paragraphStyle.ellipsis = u"";
    // any non-BASELINE vertical alignment plus multiple lines makes ParagraphImpl split
    // the crossing run per line; the split copy must carry the border width so the split
    // run keeps the +2w border reservation in the line height metrics
    paragraphStyle.verticalAlignment = TextVerticalAlign::TOP;
    auto fontCollection = std::make_shared<FontCollection>();
    fontCollection->SetupDefaultFontManager();
    auto builder = ParagraphBuilder::Create(paragraphStyle, fontCollection);
    ASSERT_NE(builder, nullptr);
    SPText::TextStyle style;
    style.backgroundRect.color = 0xFF00FF00;       // 0xFF00FF00 just for test
    style.backgroundRect.borderColor = 0xFFFF0000; // 0xFFFF0000 just for test
    style.backgroundRect.borderWidth = BORDER_WIDTH;
    builder->PushStyle(style);
    builder->AddText(u"border text border text border text");
    builder->Pop();
    std::shared_ptr<Paragraph> paragraph = builder->Build();
    ASSERT_NE(paragraph, nullptr);
    paragraph->Layout(LAYOUT_WIDTH);
    auto impl = GetParagraphSkiaImpl(paragraph);
    ASSERT_NE(impl, nullptr);
    ASSERT_GT(impl->lines().size(), 1u);
    // sanity: the run crossing the lines was actually split into per-line runs
    ASSERT_GT(impl->fRuns.size(), 1u);
    for (auto& run : impl->fRuns) {
        // the whole text carries the border, so every run (split or not) must keep it
        EXPECT_GT(run.borderWidth(), 0.0f);
    }
}

/*
 * @tc.name: ParagraphBorderJustifyClamp001
 * @tc.desc: test for justify clamping a negative allocation base when a bordered line overflows
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphBorderJustifyClamp001, TestSize.Level0)
{
    constexpr double BORDER_WIDTH = 4.0;        // 4.0 just for test
    constexpr double CALIBRATION_WIDTH = 160.0; // 160 just for test (calibrated in
                                                // ParagraphBorderJustify001): wraps the text
    constexpr double OVERFLOW = 2.0;            // 2.0 just for test: below the enter gap the
                                                // breaker still counts, so the same words stay
                                                // on line 1 while the raw line overflows
    const std::u16string text = u"border text border text border text";
    // calibration: LEFT alignment never stretches, so line 0 keeps its raw gap-inclusive
    // width (words + enter gap + trailing gap)
    auto calibrator = BuildJustifyParagraphWithBorder(SPText::TextAlign::LEFT, BORDER_WIDTH, text);
    ASSERT_NE(calibrator, nullptr);
    calibrator->Layout(CALIBRATION_WIDTH);
    auto implCalibrator = GetParagraphSkiaImpl(calibrator);
    ASSERT_NE(implCalibrator, nullptr);
    ASSERT_GT(implCalibrator->lines().size(), 1u);
    const auto rawFirstLineWidth = implCalibrator->lines()[0].widthWithoutEllipsis();
    // justify at a width slightly below the raw bordered line: line breaking reserves
    // both the enter gap and the trailing gap of a line-end box, so the paragraph
    // re-wraps instead of painting past the layout width; the negative-allocation
    // clamp in justifyShiftCluster remains as the safety net (unbreakable too-long
    // content), so the shift verification assert must still hold in debug builds and
    // the line stays within the loose upper bound everywhere else
    const double overflowWidth = rawFirstLineWidth - OVERFLOW;
    auto paragraph = BuildJustifyParagraphWithBorder(SPText::TextAlign::JUSTIFY, BORDER_WIDTH, text);
    ASSERT_NE(paragraph, nullptr);
    paragraph->Layout(overflowWidth);
    auto impl = GetParagraphSkiaImpl(paragraph);
    ASSERT_NE(impl, nullptr);
    ASSERT_GT(impl->lines().size(), 1u);
    // graceful degradation: no crash and the justified line stays within the layout
    // width plus one border gap (the loose upper bound)
    EXPECT_LE(impl->lines()[0].widthWithoutEllipsis(), overflowWidth + BORDER_WIDTH);
}

/*
 * @tc.name: ParagraphBorderMultiRun001
 * @tc.desc: test for line height taking the max over runs including border extension
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphBorderMultiRun001, TestSize.Level0)
{
    constexpr double THICK_BORDER = 20.0; // 20 just for test: dominates every run on the line
    ParagraphStyle paragraphStyle;
    auto fontCollection = std::make_shared<FontCollection>();
    fontCollection->SetupDefaultFontManager();
    auto builder = ParagraphBuilder::Create(paragraphStyle, fontCollection);
    ASSERT_NE(builder, nullptr);
    SPText::TextStyle plain;
    plain.fontSize = 30.0; // 30 just for test: taller plain run on the same line
    builder->PushStyle(plain);
    builder->AddText(u"AAAA");
    builder->Pop();
    SPText::TextStyle bordered;
    bordered.fontSize = 14.0; // 14 just for test
    bordered.backgroundRect.color = 0xFF00FF00;      // 0xFF00FF00 just for test
    bordered.backgroundRect.borderColor = 0xFFFF0000; // 0xFFFF0000 just for test
    bordered.backgroundRect.borderWidth = THICK_BORDER;
    builder->PushStyle(bordered);
    builder->AddText(u"bbbb");
    builder->Pop();
    std::shared_ptr<Paragraph> paragraph = builder->Build();
    ASSERT_NE(paragraph, nullptr);
    paragraph->Layout(300.0); // 300 just for test: single line
    auto impl = GetParagraphSkiaImpl(paragraph);
    ASSERT_NE(impl, nullptr);
    ASSERT_EQ(impl->lines().size(), 1u);
    // expected: ascent/descent max over runs with the border extension applied per run
    SkScalar expAscent = SK_ScalarMax;
    SkScalar expDescent = SK_ScalarMin;
    for (auto& run : impl->fRuns) {
        expAscent = std::fmin(expAscent, run.correctAscent() - run.borderWidth());
        expDescent = std::fmax(expDescent, run.correctDescent() + run.borderWidth());
    }
    // height() rounds to whole pixels; the max-over-runs must include the extension
    EXPECT_NEAR(impl->lines()[0].height(), expDescent - expAscent, 1.0);
}

/*
 * @tc.name: ParagraphBorderLineHeight001
 * @tc.desc: test for border reservation surviving the per-span lineHeight compression
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphBorderLineHeight001, TestSize.Level0)
{
    constexpr double BORDER_WIDTH = 8.0; // 8 just for test
    constexpr double LINE_HEIGHT = 2.0;  // 2.0 just for test: span lineHeight multiplier
    auto build = [BORDER_WIDTH](double borderWidth) -> std::shared_ptr<Paragraph> {
        ParagraphStyle paragraphStyle;
        paragraphStyle.ellipsis = u""; // clear the adapter default that forces a single line
        auto fontCollection = std::make_shared<FontCollection>();
        fontCollection->SetupDefaultFontManager();
        auto builder = ParagraphBuilder::Create(paragraphStyle, fontCollection);
        if (builder == nullptr) {
            return nullptr;
        }
        SPText::TextStyle style;
        style.height = LINE_HEIGHT;
        style.heightOverride = true;
        style.backgroundRect.color = 0xFF00FF00;      // 0xFF00FF00 just for test
        style.backgroundRect.borderColor = 0xFFFF0000; // 0xFFFF0000 just for test
        style.backgroundRect.borderWidth = borderWidth;
        builder->PushStyle(style);
        builder->AddText(u"lineheight");
        builder->Pop();
        return builder->Build();
    };
    auto plain = build(0.0);
    auto bordered = build(BORDER_WIDTH);
    ASSERT_NE(plain, nullptr);
    ASSERT_NE(bordered, nullptr);
    plain->Layout(300.0);   // 300 just for test: single line
    bordered->Layout(300.0);
    // if the +-w reservation survives the heightOverride compression, the bordered
    // paragraph must be exactly 2w taller than the plain one
    EXPECT_FLOAT_EQ(static_cast<float>(bordered->GetHeight() - plain->GetHeight()),
        2 * static_cast<float>(BORDER_WIDTH));
}

/*
 * @tc.name: ParagraphBorderVertFit001
 * @tc.desc: test for a bordered box plus its outward border staying inside its own line
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphBorderVertFit001, TestSize.Level0)
{
    constexpr double THICK_BORDER = 12.0; // 12 just for test
    ParagraphStyle paragraphStyle;
    paragraphStyle.ellipsis = u""; // clear the adapter default that forces a single line
    auto fontCollection = std::make_shared<FontCollection>();
    fontCollection->SetupDefaultFontManager();
    auto builder = ParagraphBuilder::Create(paragraphStyle, fontCollection);
    ASSERT_NE(builder, nullptr);
    SPText::TextStyle bordered;
    bordered.backgroundRect.color = 0xFF00FF00;      // 0xFF00FF00 just for test
    bordered.backgroundRect.borderColor = 0xFFFF0000; // 0xFFFF0000 just for test
    bordered.backgroundRect.borderWidth = THICK_BORDER;
    builder->PushStyle(bordered);
    builder->AddText(u"box box box");
    builder->Pop();
    std::shared_ptr<Paragraph> paragraph = builder->Build();
    ASSERT_NE(paragraph, nullptr);
    paragraph->Layout(60.0); // 60 just for test: wraps into several lines
    auto impl = GetParagraphSkiaImpl(paragraph);
    ASSERT_NE(impl, nullptr);
    ASSERT_GT(impl->lines().size(), 1u);
    for (size_t i = 0; i < impl->lines().size(); ++i) {
        auto& line = impl->lines()[i];
        const SkScalar lineHeight = line.height();
        SkScalar clipTop = -1.0f;
        SkScalar clipBottom = -1.0f;
        line.scanStyles(skt::StyleType::kBackground,
            [&clipTop, &clipBottom](skt::TextRange range, const skt::TextStyle& style,
                const skt::TextLine::ClipContext& context) {
                clipTop = context.clip.fTop;
                clipBottom = context.clip.fBottom;
            });
        if (clipTop < 0.0f) {
            continue; // line without a bordered segment
        }
        // height() rounds to whole pixels and runTop() re-derives placement from the rounded
        // height via delta(), so up to ~0.5px of the reservation can be eaten by rounding
        EXPECT_NEAR(clipTop - static_cast<SkScalar>(THICK_BORDER), 0.0f, 1.0f);
        EXPECT_LE(clipBottom + static_cast<SkScalar>(THICK_BORDER), lineHeight + 1.0f);
    }
}

/*
 * @tc.name: ParagraphBorderWrapTrailingGap001
 * @tc.desc: test for line breaking reserving the trailing border gap (text + 2w, not text + w)
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphBorderWrapTrailingGap001, TestSize.Level0)
{
    constexpr double BORDER_WIDTH = 4.0;   // 4.0 just for test
    constexpr double WIDE_LAYOUT = 1000.0; // 1000 just for test
    auto plain = BuildParagraphWithBorder(0.0, u"border text border text");
    auto bordered = BuildParagraphWithBorder(BORDER_WIDTH, u"border text border text");
    ASSERT_NE(plain, nullptr);
    ASSERT_NE(bordered, nullptr);
    plain->Layout(WIDE_LAYOUT);
    auto implPlain = GetParagraphSkiaImpl(plain);
    ASSERT_NE(implPlain, nullptr);
    ASSERT_EQ(implPlain->lines().size(), 1u);        // sanity: single line without border
    const auto tightWidth = plain->GetLongestLine(); // exact single-line width
    // text + w still overflows: the line box needs the enter gap AND the trailing gap,
    // so the paragraph must wrap instead of assembling a line that paints text + 2w
    bordered->Layout(tightWidth + BORDER_WIDTH);
    auto implBordered = GetParagraphSkiaImpl(bordered);
    ASSERT_NE(implBordered, nullptr);
    EXPECT_EQ(implBordered->lines().size(), 2u);
    // text + 2w fits exactly: the whole box stays on one line
    bordered->Layout(tightWidth + 2 * BORDER_WIDTH);
    implBordered = GetParagraphSkiaImpl(bordered);
    ASSERT_NE(implBordered, nullptr);
    EXPECT_EQ(implBordered->lines().size(), 1u);
}

/*
 * @tc.name: ParagraphBorderTrailingSpace001
 * @tc.desc: test for the border box ignoring trailing ghost spaces at a hard line break
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphBorderTrailingSpace001, TestSize.Level0)
{
    constexpr double BORDER_WIDTH = 4.0;   // 4.0 just for test
    constexpr double WIDE_LAYOUT = 1000.0; // 1000 just for test: only the break wraps
    auto fontCollection = std::make_shared<FontCollection>();
    fontCollection->SetupDefaultFontManager();
    auto collectClip = [](skt::TextLine& line, SkScalar& left, SkScalar& right) {
        line.scanStyles(skt::StyleType::kBackground, [&left, &right](skt::TextRange range,
                                         const skt::TextStyle& style,
                                         const skt::TextLine::ClipContext& context) {
            left = context.clip.fLeft;
            right = context.clip.fRight;
        });
    };
    // plain reference for the visible line-0 text width: "border text" on its own line
    auto plain = BuildParagraphWithBorder(0.0, u"border text");
    ASSERT_NE(plain, nullptr);
    plain->Layout(WIDE_LAYOUT);
    const auto textWidth = plain->GetLongestLine();

    // hard line break: the trailing space and "\n" stay on line 0 as ghost clusters
    // (unlike a soft break, where they migrate to the next line and are trimmed there)
    auto build = [&fontCollection](bool styleWholeLine) -> std::shared_ptr<Paragraph> {
        ParagraphStyle paragraphStyle;
        paragraphStyle.ellipsis = u""; // clear the adapter default that forces a single line
        auto builder = ParagraphBuilder::Create(paragraphStyle, fontCollection);
        if (builder == nullptr) {
            return nullptr;
        }
        SPText::TextStyle bordered;
        bordered.backgroundRect.color = 0xFF00FF00;       // 0xFF00FF00 just for test
        bordered.backgroundRect.borderColor = 0xFFFF0000; // 0xFFFF0000 just for test
        bordered.backgroundRect.borderWidth = BORDER_WIDTH;
        builder->PushStyle(bordered);
        builder->AddText(styleWholeLine ? u"border text \n" : u"border text");
        builder->Pop();
        if (!styleWholeLine) {
            // ghost " \n" outside the box: the walk charges the box exit at the space
            builder->AddText(u" \nborder text");
        } else {
            builder->AddText(u"border text");
        }
        return builder->Build();
    };

    // case 1: the ghost " \n" is plain text after the box; the box's exit gap must be
    // materialized exactly once (the walk's transition charge), never twice
    auto outside = build(false);
    ASSERT_NE(outside, nullptr);
    outside->Layout(WIDE_LAYOUT);
    auto implOutside = GetParagraphSkiaImpl(outside);
    ASSERT_NE(implOutside, nullptr);
    ASSERT_EQ(implOutside->lines().size(), 2u);
    SkScalar clipLeft = -1.0f;
    SkScalar clipRight = -1.0f;
    collectClip(implOutside->lines()[0], clipLeft, clipRight);
    EXPECT_FLOAT_EQ(clipLeft, static_cast<SkScalar>(BORDER_WIDTH));
    EXPECT_FLOAT_EQ(clipRight, textWidth + static_cast<SkScalar>(BORDER_WIDTH));
    EXPECT_FLOAT_EQ(implOutside->lines()[0].widthWithoutEllipsis(),
        clipRight + static_cast<SkScalar>(BORDER_WIDTH));

    // case 2: the ghost " \n" is inside the box's own style block; the painted box must
    // hug the last visible cluster and never extend over the ghost space
    auto inside = build(true);
    ASSERT_NE(inside, nullptr);
    inside->Layout(WIDE_LAYOUT);
    auto implInside = GetParagraphSkiaImpl(inside);
    ASSERT_NE(implInside, nullptr);
    ASSERT_EQ(implInside->lines().size(), 2u);
    clipLeft = -1.0f;
    clipRight = -1.0f;
    collectClip(implInside->lines()[0], clipLeft, clipRight);
    EXPECT_FLOAT_EQ(clipLeft, static_cast<SkScalar>(BORDER_WIDTH));
    EXPECT_FLOAT_EQ(clipRight, textWidth + static_cast<SkScalar>(BORDER_WIDTH));
    EXPECT_FLOAT_EQ(implInside->lines()[0].widthWithoutEllipsis(),
        clipRight + static_cast<SkScalar>(BORDER_WIDTH));
    // the paint path iterates the line with ghost spaces included, so verify the box it
    // builds directly: the border attr must stop at the last visible cluster
    implInside->lines()[0].prepareRoundRect();
    const auto& attrs = implInside->lines()[0].fRoundRectAttrs;
    ASSERT_FALSE(attrs.empty());
    const auto& borderAttr = attrs.front();
    EXPECT_FLOAT_EQ(borderAttr.rect.fLeft, static_cast<SkScalar>(BORDER_WIDTH));
    EXPECT_FLOAT_EQ(borderAttr.rect.fRight, textWidth + static_cast<SkScalar>(BORDER_WIDTH));
}

/*
 * @tc.name: ParagraphBorderImageBounds001
 * @tc.desc: test for run and line image bounds excluding the border gaps
 * @tc.type: FUNC
 */
HWTEST_F(ParagraphTest, ParagraphBorderImageBounds001, TestSize.Level0)
{
    constexpr double BORDER_WIDTH = 4.0;   // 4.0 just for test
    constexpr double LAYOUT_WIDTH = 300.0; // 300 just for test: single line
    auto plain = BuildParagraphWithBorder(0.0, u"border text");
    auto bordered = BuildParagraphWithBorder(BORDER_WIDTH, u"border text");
    ASSERT_NE(plain, nullptr);
    ASSERT_NE(bordered, nullptr);
    plain->Layout(LAYOUT_WIDTH);
    bordered->Layout(LAYOUT_WIDTH);
    auto plainLines = plain->GetTextLines();
    auto borderedLines = bordered->GetTextLines();
    ASSERT_EQ(plainLines.size(), 1u);
    ASSERT_EQ(borderedLines.size(), 1u);
    const auto shift = static_cast<float>(BORDER_WIDTH);
    // run bounds: the enter gap shifts the glyphs; the rect must follow the shift
    // without widening, never plain + 2w
    auto plainRuns = plainLines[0]->GetGlyphRuns();
    auto borderedRuns = borderedLines[0]->GetGlyphRuns();
    ASSERT_EQ(plainRuns.size(), 1u);
    ASSERT_EQ(borderedRuns.size(), 1u);
    auto plainRunRect = plainRuns[0]->GetImageBounds();
    auto borderedRunRect = borderedRuns[0]->GetImageBounds();
    EXPECT_FLOAT_EQ(borderedRunRect.GetLeft(), plainRunRect.GetLeft() + shift);
    EXPECT_FLOAT_EQ(borderedRunRect.GetRight(), plainRunRect.GetRight() + shift);
    // line bounds: the same shift and no widening
    auto plainLineRect = plainLines[0]->GetImageBounds();
    auto borderedLineRect = borderedLines[0]->GetImageBounds();
    EXPECT_FLOAT_EQ(borderedLineRect.GetLeft(), plainLineRect.GetLeft() + shift);
    EXPECT_FLOAT_EQ(borderedLineRect.GetRight(), plainLineRect.GetRight() + shift);
}
} // namespace txt