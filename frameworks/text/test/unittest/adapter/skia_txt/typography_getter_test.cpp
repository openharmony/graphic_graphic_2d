/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <bitset>
#include <cstddef>

#include "font_collection.h"
#include "impl/paragraph_impl.h"
#include "ohos/init_data.h"
#include "paragraph.h"
#include "text_style.h"
#include "typography.h"
#include "typography_create.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
const double DEFAULT_FONT_SIZE = 40;
class OH_Drawing_TypographyGetterTest : public testing::Test {
public:
    void SetUp() override;
    void TearDown() override;

protected:
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection_;
    OHOS::Rosen::TypographyStyle typographyStyle_;
    OHOS::Rosen::TextStyle textStyle_;
    std::u16string text_;
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate_;
    std::unique_ptr<OHOS::Rosen::Typography> typography_;
};

void OH_Drawing_TypographyGetterTest::SetUp()
{
    SetHwIcuDirectory();
    fontCollection_ = OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    textStyle_.fontSize = DEFAULT_FONT_SIZE;
    text_ = u"test text";
}

void OH_Drawing_TypographyGetterTest::TearDown()
{
    fontCollection_.reset();
    fontCollection_ = nullptr;
    typographyCreate_.reset();
    typographyCreate_ = nullptr;
    typography_.reset();
    typography_ = nullptr;
}

/*
 * @tc.name: OH_Drawing_TypographyGetProcessState001
 * @tc.desc: test for GetProcessState before layout (INIT state)
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyGetterTest, OH_Drawing_TypographyGetProcessState001, TestSize.Level0)
{
    typographyCreate_ = OHOS::Rosen::TypographyCreate::Create(typographyStyle_, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);
    typographyCreate_->PushStyle(textStyle_);
    typographyCreate_->AppendText(text_);
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);

    // Before layout, process state should be INIT
    TextProcessState state = typography_->GetProcessState();
    EXPECT_EQ(state, TextProcessState::INIT);
}

/*
 * @tc.name: OH_Drawing_TypographyGetProcessState002
 * @tc.desc: test for GetProcessState in INDEXED state
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyGetterTest, OH_Drawing_TypographyGetProcessState002, TestSize.Level0)
{
    typographyCreate_ = OHOS::Rosen::TypographyCreate::Create(typographyStyle_, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);
    typographyCreate_->PushStyle(textStyle_);
    typographyCreate_->AppendText(text_);
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);

    // Set state to INDEXED (1)
    SPText::ParagraphImpl* paragraph = static_cast<SPText::ParagraphImpl*>(typography_->GetParagraph());
    ASSERT_NE(paragraph, nullptr);
    paragraph->SetLayoutState(1);

    TextProcessState state = typography_->GetProcessState();
    EXPECT_EQ(state, TextProcessState::INDEXED);
}

/*
 * @tc.name: OH_Drawing_TypographyGetProcessState003
 * @tc.desc: test for GetProcessState in SHAPED state
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyGetterTest, OH_Drawing_TypographyGetProcessState003, TestSize.Level0)
{
    typographyCreate_ = OHOS::Rosen::TypographyCreate::Create(typographyStyle_, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);
    typographyCreate_->PushStyle(textStyle_);
    typographyCreate_->AppendText(text_);
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);

    // Set state to SHAPED (2)
    SPText::ParagraphImpl* paragraph = static_cast<SPText::ParagraphImpl*>(typography_->GetParagraph());
    ASSERT_NE(paragraph, nullptr);
    paragraph->SetLayoutState(2);

    TextProcessState state = typography_->GetProcessState();
    EXPECT_EQ(state, TextProcessState::SHAPED);
}

/*
 * @tc.name: OH_Drawing_TypographyGetProcessState004
 * @tc.desc: test for GetProcessState in LINE_BROKEN state
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyGetterTest, OH_Drawing_TypographyGetProcessState004, TestSize.Level0)
{
    typographyCreate_ = OHOS::Rosen::TypographyCreate::Create(typographyStyle_, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);
    typographyCreate_->PushStyle(textStyle_);
    typographyCreate_->AppendText(text_);
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);

    // Set state to LINE_BROKEN (5)
    SPText::ParagraphImpl* paragraph = static_cast<SPText::ParagraphImpl*>(typography_->GetParagraph());
    ASSERT_NE(paragraph, nullptr);
    paragraph->SetLayoutState(5);

    TextProcessState state = typography_->GetProcessState();
    EXPECT_EQ(state, TextProcessState::LINE_BROKEN);
}

/*
 * @tc.name: OH_Drawing_TypographyGetProcessState05
 * @tc.desc: test for GetProcessState after layout (FORMATTED state)
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyGetterTest, OH_Drawing_TypographyGetProcessState005, TestSize.Level0)
{
    typographyCreate_ = OHOS::Rosen::TypographyCreate::Create(typographyStyle_, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);
    typographyCreate_->PushStyle(textStyle_);
    typographyCreate_->AppendText(text_);
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);
    double maxWidth = 500;
    typography_->Layout(maxWidth);

    // After layout, process state should be FORMATTED
    TextProcessState state = typography_->GetProcessState();
    EXPECT_EQ(state, TextProcessState::FORMATTED);
}

/*
 * @tc.name: OH_Drawing_TypographyGetProcessState006
 * @tc.desc: test for GetProcessState after paint (PAINT state)
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyGetterTest, OH_Drawing_TypographyGetProcessState006, TestSize.Level0)
{
    typographyCreate_ = OHOS::Rosen::TypographyCreate::Create(typographyStyle_, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);
    typographyCreate_->PushStyle(textStyle_);
    typographyCreate_->AppendText(text_);
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);
    double maxWidth = 500;
    typography_->Layout(maxWidth);

    OHOS::Rosen::Drawing::Canvas canvas;
    typography_->Paint(&canvas, 0, 0);

    // After paint, process state should be PAINT
    TextProcessState state = typography_->GetProcessState();
    EXPECT_EQ(state, TextProcessState::PAINT);
}

/*
 * @tc.name: OH_Drawing_TypographyGetProcessState007
 * @tc.desc: test for GetProcessState after UpdateColor (UPDATE_ATTRIBUTE state)
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyGetterTest, OH_Drawing_TypographyGetProcessState007, TestSize.Level0)
{
    typographyCreate_ = OHOS::Rosen::TypographyCreate::Create(typographyStyle_, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);
    typographyCreate_->PushStyle(textStyle_);
    typographyCreate_->AppendText(text_);
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);
    double maxWidth = 500;
    typography_->Layout(maxWidth);

    // Call UpdateColor to trigger UPDATE_ATTRIBUTE state
    OHOS::Rosen::Drawing::Color color(255, 0, 0, 255);
    typography_->UpdateColor(0, text_.length(), color);

    // After update attribute, process state should be UPDATE_ATTRIBUTE
    TextProcessState state = typography_->GetProcessState();
    EXPECT_EQ(state, TextProcessState::UPDATE_ATTRIBUTE);
}

/*
 * @tc.name: OH_Drawing_TypographyGetProcessState008
 * @tc.desc: test for GetProcessState after Relayout (UPDATE_ATTRIBUTE state)
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyGetterTest, OH_Drawing_TypographyGetProcessState008, TestSize.Level0)
{
    typographyCreate_ = OHOS::Rosen::TypographyCreate::Create(typographyStyle_, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);
    typographyCreate_->PushStyle(textStyle_);
    typographyCreate_->AppendText(text_);
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);
    double maxWidth = 500;
    typography_->Layout(maxWidth);

    // Call Relayout to trigger UPDATE_ATTRIBUTE state
    std::vector<OHOS::Rosen::TextStyle> relayoutTextStyles;
    OHOS::Rosen::TextStyle textStyle;
    textStyle.fontSize = 14;
    std::bitset<static_cast<size_t>(RelayoutTextStyleAttribute::TEXT_STYLE_ATTRIBUTE_BUTT)> styleBitset;
    styleBitset.set(static_cast<size_t>(RelayoutTextStyleAttribute::FONT_SIZE));
    textStyle.relayoutChangeBitmap = styleBitset;
    relayoutTextStyles.push_back(textStyle);
    typography_->Relayout(maxWidth, typographyStyle_, relayoutTextStyles);

    // After update attribute, process state should be UPDATE_ATTRIBUTE
    TextProcessState state = typography_->GetProcessState();
    EXPECT_EQ(state, TextProcessState::UPDATE_ATTRIBUTE);
}

/*
 * @tc.name: OH_Drawing_TypographyGetTextDisplayState001
 * @tc.desc: test for GetTextDisplayState before layout
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyGetterTest, OH_Drawing_TypographyGetTextDisplayState001, TestSize.Level0)
{
    typographyCreate_ = OHOS::Rosen::TypographyCreate::Create(typographyStyle_, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);
    typographyCreate_->PushStyle(textStyle_);
    typographyCreate_->AppendText(text_);
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);

    // Before layout, display state should be UNKNOWN
    TextDisplayState state = typography_->GetTextDisplayState();
    EXPECT_EQ(state, TextDisplayState::UNKNOWN);
}

/*
 * @tc.name: OH_Drawing_TypographyGetTextDisplayState002
 * @tc.desc: test for GetTextDisplayState after layout without ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyGetterTest, OH_Drawing_TypographyGetTextDisplayState002, TestSize.Level0)
{
    typographyCreate_ = OHOS::Rosen::TypographyCreate::Create(typographyStyle_, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);
    typographyCreate_->PushStyle(textStyle_);
    typographyCreate_->AppendText(text_);
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);
    // Use a large width to ensure no ellipsis
    double maxWidth = 1000;
    typography_->Layout(maxWidth);

    // After layout without ellipsis, display state should be ALL
    TextDisplayState state = typography_->GetTextDisplayState();
    EXPECT_EQ(state, TextDisplayState::ALL);
}

/*
 * @tc.name: OH_Drawing_TypographyGetTextDisplayState003
 * @tc.desc: test for GetTextDisplayState after layout with ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyGetterTest, OH_Drawing_TypographyGetTextDisplayState003, TestSize.Level0)
{
    typographyStyle_.maxLines = 1;
    std::u16string ellipsisStr = TypographyStyle::ELLIPSIS;
    typographyStyle_.ellipsis = ellipsisStr;
    typographyCreate_ = OHOS::Rosen::TypographyCreate::Create(typographyStyle_, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);
    typographyCreate_->PushStyle(textStyle_);
    typographyCreate_->AppendText(text_);
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);
    // Use a small width to trigger ellipsis
    double maxWidth = 100;
    typography_->Layout(maxWidth);

    // After layout with ellipsis, display state should be OMITTED
    TextDisplayState state = typography_->GetTextDisplayState();
    EXPECT_EQ(state, TextDisplayState::OMITTED);
}

/*
 * @tc.name: OH_Drawing_TypographyGetTextDisplayState004
 * @tc.desc: test for GetTextDisplayState after layout with clip (exceed max lines without ellipsis)
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyGetterTest, OH_Drawing_TypographyGetTextDisplayState004, TestSize.Level0)
{
    typographyStyle_.maxLines = 1;
    // No ellipsis set, so it should be CLIP when exceeding max lines
    typographyCreate_ = OHOS::Rosen::TypographyCreate::Create(typographyStyle_, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);
    typographyCreate_->PushStyle(textStyle_);
    typographyCreate_->AppendText(text_);
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);
    // Use a small width to trigger exceeding max lines
    double maxWidth = 100;
    typography_->Layout(maxWidth);

    // After layout exceeding max lines without ellipsis, display state should be CLIP
    TextDisplayState state = typography_->GetTextDisplayState();
    EXPECT_EQ(state, TextDisplayState::CLIP);
}

/*
 * @tc.name: OH_Drawing_TypographyGetParagraphStyle001
 * @tc.desc: test for GetParagraphStyle with font properties
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyGetterTest, OH_Drawing_TypographyGetParagraphStyle001, TestSize.Level0)
{
    typographyStyle_.fontWeight = FontWeight::W700;
    typographyStyle_.fontWidth = FontWidth::EXPANDED;
    typographyStyle_.fontStyle = FontStyle::ITALIC;
    typographyStyle_.fontFamily = "Roboto";
    typographyStyle_.fontSize = 32.0;
    typographyStyle_.heightScale = 1.5;
    typographyStyle_.halfLeading = true;
    typographyStyle_.heightOnly = true;
    typographyStyle_.useLineStyle = true;

    typographyCreate_ = OHOS::Rosen::TypographyCreate::Create(typographyStyle_, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);
    typographyCreate_->PushStyle(textStyle_);
    typographyCreate_->AppendText(text_);
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);
    typography_->Layout(500);

    TypographyStyle resultStyle = typography_->GetParagraphStyle();
    EXPECT_EQ(resultStyle.fontWeight, typographyStyle_.fontWeight);
    EXPECT_EQ(resultStyle.fontWidth, typographyStyle_.fontWidth);
    EXPECT_EQ(resultStyle.fontStyle, typographyStyle_.fontStyle);
    EXPECT_EQ(resultStyle.fontFamily, typographyStyle_.fontFamily);
    EXPECT_FLOAT_EQ(resultStyle.fontSize, typographyStyle_.fontSize);
    EXPECT_FLOAT_EQ(resultStyle.heightScale, typographyStyle_.heightScale);
    EXPECT_EQ(resultStyle.halfLeading, typographyStyle_.halfLeading);
    EXPECT_EQ(resultStyle.heightOnly, typographyStyle_.heightOnly);
    EXPECT_EQ(resultStyle.useLineStyle, typographyStyle_.useLineStyle);
}

/*
 * @tc.name: OH_Drawing_TypographyGetParagraphStyle002
 * @tc.desc: test for GetParagraphStyle with line style properties
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyGetterTest, OH_Drawing_TypographyGetParagraphStyle002, TestSize.Level0)
{
    typographyStyle_.useLineStyle = true;
    typographyStyle_.lineStyleFontWeight = FontWeight::W500;
    typographyStyle_.lineStyleFontWidth = FontWidth::SEMI_EXPANDED;
    typographyStyle_.lineStyleFontStyle = FontStyle::ITALIC;
    typographyStyle_.lineStyleFontFamilies = {"Arial", "Helvetica"};
    typographyStyle_.lineStyleFontSize = 24.0;
    typographyStyle_.lineStyleHeightScale = 1.2;
    typographyStyle_.lineStyleHalfLeading = true;
    typographyStyle_.lineStyleSpacingScale = 1;
    typographyStyle_.lineStyleOnly = true;

    typographyCreate_ = OHOS::Rosen::TypographyCreate::Create(typographyStyle_, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);
    typographyCreate_->PushStyle(textStyle_);
    typographyCreate_->AppendText(text_);
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);
    typography_->Layout(500);

    TypographyStyle resultStyle = typography_->GetParagraphStyle();
    EXPECT_EQ(resultStyle.lineStyleFontWeight, typographyStyle_.lineStyleFontWeight);
    EXPECT_EQ(resultStyle.lineStyleFontWidth, typographyStyle_.lineStyleFontWidth);
    EXPECT_EQ(resultStyle.lineStyleFontStyle, typographyStyle_.lineStyleFontStyle);
    EXPECT_EQ(resultStyle.lineStyleFontFamilies, typographyStyle_.lineStyleFontFamilies);
    EXPECT_FLOAT_EQ(resultStyle.lineStyleFontSize, typographyStyle_.lineStyleFontSize);
    EXPECT_FLOAT_EQ(resultStyle.lineStyleHeightScale, typographyStyle_.lineStyleHeightScale);
    EXPECT_EQ(resultStyle.lineStyleHalfLeading, typographyStyle_.lineStyleHalfLeading);
    EXPECT_EQ(resultStyle.lineStyleSpacingScale, typographyStyle_.lineStyleSpacingScale);
    EXPECT_EQ(resultStyle.lineStyleOnly, typographyStyle_.lineStyleOnly);
}

/*
 * @tc.name: OH_Drawing_TypographyGetParagraphStyle003
 * @tc.desc: test for GetParagraphStyle with text layout properties
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyGetterTest, OH_Drawing_TypographyGetParagraphStyle003, TestSize.Level0)
{
    typographyStyle_.textAlign = TextAlign::JUSTIFY;
    typographyStyle_.textDirection = TextDirection::RTL;
    typographyStyle_.maxLines = 10;
    typographyStyle_.ellipsis = u"...";
    typographyStyle_.locale = "zh-Hans";
    typographyStyle_.breakStrategy = BreakStrategy::BALANCED;
    typographyStyle_.wordBreakType = WordBreakType::BREAK_ALL;
    typographyStyle_.ellipsisModal = EllipsisModal::MIDDLE;

    typographyCreate_ = OHOS::Rosen::TypographyCreate::Create(typographyStyle_, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);
    typographyCreate_->PushStyle(textStyle_);
    typographyCreate_->AppendText(text_);
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);
    typography_->Layout(500);

    TypographyStyle resultStyle = typography_->GetParagraphStyle();
    EXPECT_EQ(resultStyle.textAlign, typographyStyle_.textAlign);
    EXPECT_EQ(resultStyle.textDirection, typographyStyle_.textDirection);
    EXPECT_EQ(resultStyle.maxLines, typographyStyle_.maxLines);
    EXPECT_EQ(resultStyle.ellipsis, typographyStyle_.ellipsis);
    EXPECT_EQ(resultStyle.locale, typographyStyle_.locale);
    EXPECT_EQ(resultStyle.breakStrategy, typographyStyle_.breakStrategy);
    EXPECT_EQ(resultStyle.wordBreakType, typographyStyle_.wordBreakType);
    EXPECT_EQ(resultStyle.ellipsisModal, typographyStyle_.ellipsisModal);
}

/*
 * @tc.name: OH_Drawing_TypographyGetParagraphStyle004
 * @tc.desc: test for GetParagraphStyle with spacing and behavior properties
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyGetterTest, OH_Drawing_TypographyGetParagraphStyle004, TestSize.Level0)
{
    typographyStyle_.paragraphSpacing = 8.0f;
    typographyStyle_.isEndAddParagraphSpacing = true;
    typographyStyle_.verticalAlignment = TextVerticalAlign::CENTER;
    typographyStyle_.textHeightBehavior = TextHeightBehavior::DISABLE_ALL;
    typographyStyle_.hintingIsOn = true;
    typographyStyle_.maxLineHeight = 100.0;
    typographyStyle_.minLineHeight = 20.0;
    typographyStyle_.lineSpacing = 5.0;
    typographyStyle_.lineHeightStyle = LineHeightStyle::kFontHeight;
    typographyStyle_.includeFontPadding = true;
    typographyStyle_.fallbackLineSpacing = true;
    typographyStyle_.orphanCharOptimization = true;

    typographyCreate_ = OHOS::Rosen::TypographyCreate::Create(typographyStyle_, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);
    typographyCreate_->PushStyle(textStyle_);
    typographyCreate_->AppendText(text_);
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);
    typography_->Layout(500);

    TypographyStyle resultStyle = typography_->GetParagraphStyle();
    EXPECT_FLOAT_EQ(resultStyle.paragraphSpacing, typographyStyle_.paragraphSpacing);
    EXPECT_EQ(resultStyle.isEndAddParagraphSpacing, typographyStyle_.isEndAddParagraphSpacing);
    EXPECT_EQ(resultStyle.verticalAlignment, typographyStyle_.verticalAlignment);
    EXPECT_EQ(resultStyle.textHeightBehavior, typographyStyle_.textHeightBehavior);
    EXPECT_EQ(resultStyle.hintingIsOn, typographyStyle_.hintingIsOn);
    EXPECT_FLOAT_EQ(resultStyle.maxLineHeight, typographyStyle_.maxLineHeight);
    EXPECT_FLOAT_EQ(resultStyle.minLineHeight, typographyStyle_.minLineHeight);
    EXPECT_FLOAT_EQ(resultStyle.lineSpacing, typographyStyle_.lineSpacing);
    EXPECT_EQ(resultStyle.lineHeightStyle, typographyStyle_.lineHeightStyle);
    EXPECT_EQ(resultStyle.includeFontPadding, typographyStyle_.includeFontPadding);
    EXPECT_EQ(resultStyle.fallbackLineSpacing, typographyStyle_.fallbackLineSpacing);
    EXPECT_EQ(resultStyle.orphanCharOptimization, typographyStyle_.orphanCharOptimization);
}

/*
 * @tc.name: OH_Drawing_TypographyGetParagraphStyle005
 * @tc.desc: test for GetParagraphStyle with insideTextStyle font properties
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyGetterTest, OH_Drawing_TypographyGetParagraphStyle005, TestSize.Level0)
{
    typographyStyle_.customTextStyle = true;
    typographyStyle_.insideTextStyle.color = OHOS::Rosen::Drawing::Color::ColorQuadSetARGB(255, 100, 150, 200);
    typographyStyle_.insideTextStyle.fontWeight = FontWeight::W600;
    typographyStyle_.insideTextStyle.fontWidth = FontWidth::EXPANDED;
    typographyStyle_.insideTextStyle.fontStyle = FontStyle::ITALIC;
    typographyStyle_.insideTextStyle.fontFamilies = {"TestFont", "Arial"};
    typographyStyle_.insideTextStyle.fontSize = 40.0;
    typographyStyle_.insideTextStyle.letterSpacing = 1.5;
    typographyStyle_.insideTextStyle.wordSpacing = 2.0;
    typographyStyle_.insideTextStyle.heightScale = 1.8;
    typographyStyle_.insideTextStyle.halfLeading = true;
    typographyStyle_.insideTextStyle.heightOnly = true;

    typographyCreate_ = OHOS::Rosen::TypographyCreate::Create(typographyStyle_, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);
    typographyCreate_->PushStyle(textStyle_);
    typographyCreate_->AppendText(text_);
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);
    typography_->Layout(500);

    TypographyStyle resultStyle = typography_->GetParagraphStyle();
    EXPECT_EQ(resultStyle.customTextStyle, typographyStyle_.customTextStyle);
    EXPECT_EQ(resultStyle.insideTextStyle.color, typographyStyle_.insideTextStyle.color);
    EXPECT_EQ(resultStyle.insideTextStyle.fontWeight, typographyStyle_.insideTextStyle.fontWeight);
    EXPECT_EQ(resultStyle.insideTextStyle.fontWidth, typographyStyle_.insideTextStyle.fontWidth);
    EXPECT_EQ(resultStyle.insideTextStyle.fontStyle, typographyStyle_.insideTextStyle.fontStyle);
    EXPECT_FLOAT_EQ(resultStyle.insideTextStyle.fontSize, typographyStyle_.insideTextStyle.fontSize);
    EXPECT_FLOAT_EQ(resultStyle.insideTextStyle.letterSpacing, typographyStyle_.insideTextStyle.letterSpacing);
    EXPECT_FLOAT_EQ(resultStyle.insideTextStyle.wordSpacing, typographyStyle_.insideTextStyle.wordSpacing);
    EXPECT_FLOAT_EQ(resultStyle.insideTextStyle.heightScale, typographyStyle_.insideTextStyle.heightScale);
    EXPECT_EQ(resultStyle.insideTextStyle.halfLeading, typographyStyle_.insideTextStyle.halfLeading);
    EXPECT_EQ(resultStyle.insideTextStyle.heightOnly, typographyStyle_.insideTextStyle.heightOnly);
}

/*
 * @tc.name: OH_Drawing_TypographyGetParagraphStyle006
 * @tc.desc: test for GetParagraphStyle with insideTextStyle decoration and other properties
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyGetterTest, OH_Drawing_TypographyGetParagraphStyle006, TestSize.Level0)
{
    typographyStyle_.customTextStyle = true;
    typographyStyle_.ellipsis = u"...";
    typographyStyle_.ellipsisModal = EllipsisModal::MIDDLE;
    typographyStyle_.insideTextStyle.decoration =
        static_cast<TextDecoration>(TextDecoration::UNDERLINE | TextDecoration::LINE_THROUGH);
    typographyStyle_.insideTextStyle.decorationColor = OHOS::Rosen::Drawing::Color::ColorQuadSetARGB(255, 50, 75, 100);
    typographyStyle_.insideTextStyle.decorationStyle = TextDecorationStyle::DASHED;
    typographyStyle_.insideTextStyle.decorationThicknessScale = 2.0;
    typographyStyle_.insideTextStyle.baseline = TextBaseline::IDEOGRAPHIC;
    typographyStyle_.insideTextStyle.ellipsis = u"...";
    typographyStyle_.insideTextStyle.ellipsisModal = EllipsisModal::MIDDLE;
    typographyStyle_.insideTextStyle.locale = "en-US";
    typographyStyle_.insideTextStyle.baseLineShift = 5.0;
    typographyStyle_.insideTextStyle.badgeType = TextBadgeType::SUPERSCRIPT;
    typographyStyle_.insideTextStyle.maxLineHeight = 80.0;
    typographyStyle_.insideTextStyle.minLineHeight = 20.0;
    typographyStyle_.insideTextStyle.lineHeightStyle = LineHeightStyle::kFontHeight;
    typographyStyle_.insideTextStyle.fontEdging = OHOS::Rosen::Drawing::FontEdging::SUBPIXEL_ANTI_ALIAS;

    typographyCreate_ = OHOS::Rosen::TypographyCreate::Create(typographyStyle_, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);
    typographyCreate_->PushStyle(textStyle_);
    typographyCreate_->AppendText(text_);
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);
    typography_->Layout(500);

    TypographyStyle resultStyle = typography_->GetParagraphStyle();
    EXPECT_EQ(resultStyle.insideTextStyle.decoration, typographyStyle_.insideTextStyle.decoration);
    EXPECT_EQ(resultStyle.insideTextStyle.decorationColor, typographyStyle_.insideTextStyle.decorationColor);
    EXPECT_EQ(resultStyle.insideTextStyle.decorationStyle, typographyStyle_.insideTextStyle.decorationStyle);
    EXPECT_FLOAT_EQ(resultStyle.insideTextStyle.decorationThicknessScale,
        typographyStyle_.insideTextStyle.decorationThicknessScale);
    EXPECT_EQ(resultStyle.insideTextStyle.baseline, typographyStyle_.insideTextStyle.baseline);
    EXPECT_EQ(resultStyle.insideTextStyle.ellipsis, typographyStyle_.insideTextStyle.ellipsis);
    EXPECT_EQ(resultStyle.insideTextStyle.ellipsisModal, typographyStyle_.insideTextStyle.ellipsisModal);
    EXPECT_EQ(resultStyle.insideTextStyle.locale, typographyStyle_.insideTextStyle.locale);
    EXPECT_FLOAT_EQ(resultStyle.insideTextStyle.baseLineShift, typographyStyle_.insideTextStyle.baseLineShift);
    EXPECT_EQ(resultStyle.insideTextStyle.badgeType, typographyStyle_.insideTextStyle.badgeType);
    EXPECT_FLOAT_EQ(resultStyle.insideTextStyle.maxLineHeight, typographyStyle_.insideTextStyle.maxLineHeight);
    EXPECT_FLOAT_EQ(resultStyle.insideTextStyle.minLineHeight, typographyStyle_.insideTextStyle.minLineHeight);
    EXPECT_EQ(resultStyle.insideTextStyle.lineHeightStyle, typographyStyle_.insideTextStyle.lineHeightStyle);
    EXPECT_EQ(resultStyle.insideTextStyle.fontEdging, typographyStyle_.insideTextStyle.fontEdging);
}

/*
 * @tc.name: OH_Drawing_TypographyGetParagraphStyle007
 * @tc.desc: test for GetParagraphStyle with tab property
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyGetterTest, OH_Drawing_TypographyGetParagraphStyle007, TestSize.Level0)
{
    typographyStyle_.tab.alignment = TextAlign::CENTER;
    typographyStyle_.tab.location = 200.0f;

    typographyCreate_ = OHOS::Rosen::TypographyCreate::Create(typographyStyle_, fontCollection_);
    ASSERT_NE(typographyCreate_, nullptr);
    typographyCreate_->PushStyle(textStyle_);
    typographyCreate_->AppendText(text_);
    typography_ = typographyCreate_->CreateTypography();
    ASSERT_NE(typography_, nullptr);
    typography_->Layout(500);

    TypographyStyle resultStyle = typography_->GetParagraphStyle();
    EXPECT_EQ(resultStyle.tab.alignment, typographyStyle_.tab.alignment);
    EXPECT_FLOAT_EQ(resultStyle.tab.location, typographyStyle_.tab.location);
}
} // namespace Rosen
} // namespace OHOS