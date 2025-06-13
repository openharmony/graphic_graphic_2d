/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.. All rights reserved.
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

#include <bitset>

#include "font_collection.h"
#include "gtest/gtest.h"
#include "hm_symbol_txt.h"
#include "impl/paragraph_impl.h"
#include "modules/skparagraph/include/ParagraphStyle.h"
#include "typography.h"
#include "typography_create.h"
#include "typography_style.h"
#include "typography_types.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
const double LAYOUT_WIDTH = 1000;
const double UNIQUEID = 999;
class TypographyRelayoutTest : public testing::Test {
};

/*
 * @tc.name: OHDrawingTypographyRelayoutTest001
 * @tc.desc: test for relayout but not change paragraph style and text style
 * @tc.type: FUNC
 */
HWTEST_F(TypographyRelayoutTest, OHDrawingTypographyRelayoutTest001, TestSize.Level0)
{
    double maxWidth = LAYOUT_WIDTH;
    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    std::u16string text = u"relayout test";
    OHOS::Rosen::TextStyle textStyle;
    textStyle.fontSize = 14;
    typographyCreate->PushStyle(textStyle);
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    typography->Layout(maxWidth);
    double preLongestLineWithIndent = typography->GetLongestLineWithIndent();
    std::vector<OHOS::Rosen::TextStyle> relayoutTextStyles;
    relayoutTextStyles.push_back(textStyle);
    typography->Relayout(maxWidth, typographyStyle, relayoutTextStyles);
    double relayoutLongestLineWithIndent = typography->GetLongestLineWithIndent();

    EXPECT_TRUE(skia::textlayout::nearlyEqual(preLongestLineWithIndent, relayoutLongestLineWithIndent));
}

/*
 * @tc.name: OHDrawingTypographyRelayoutTest002
 * @tc.desc: test for relayout but not layout
 * @tc.type: FUNC
 */
HWTEST_F(TypographyRelayoutTest, OHDrawingTypographyRelayoutTest002, TestSize.Level0)
{
    double maxWidth = LAYOUT_WIDTH;
    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    std::u16string text = u"relayout test";
    OHOS::Rosen::TextStyle textStyle;
    textStyle.fontSize = 14;
    typographyCreate->PushStyle(textStyle);
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    double preLongestLineWithIndent = typography->GetLongestLineWithIndent();

    std::vector<OHOS::Rosen::TextStyle> relayoutTextStyles;
    relayoutTextStyles.push_back(textStyle);
    typography->Relayout(maxWidth, typographyStyle, relayoutTextStyles);
    double relayoutLongestLineWithIndent = typography->GetLongestLineWithIndent();

    EXPECT_TRUE(skia::textlayout::nearlyEqual(preLongestLineWithIndent, 0));
    EXPECT_TRUE(skia::textlayout::nearlyEqual(relayoutLongestLineWithIndent, 0));
}

/*
 * @tc.name: OHDrawingTypographyRelayoutTest003
 * @tc.desc: test for relayout and changing relayout width
 * @tc.type: FUNC
 */
HWTEST_F(TypographyRelayoutTest, OHDrawingTypographyRelayoutTest003, TestSize.Level0)
{
    double maxWidth = LAYOUT_WIDTH / 10;
    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    std::u16string text = u"relayout test for double relayout width";
    OHOS::Rosen::TextStyle textStyle;
    textStyle.fontSize = 14;
    typographyCreate->PushStyle(textStyle);
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    typography->Layout(maxWidth);
    double preLongestLineWithIndent = typography->GetLongestLineWithIndent();

    std::vector<OHOS::Rosen::TextStyle> relayoutTextStyles;
    relayoutTextStyles.push_back(textStyle);
    typography->Relayout(maxWidth * 2, typographyStyle, relayoutTextStyles);
    double relayoutLongestLineWithIndent = typography->GetLongestLineWithIndent();

    EXPECT_FALSE(skia::textlayout::nearlyEqual(preLongestLineWithIndent, relayoutLongestLineWithIndent));
}

/*
 * @tc.name: OHDrawingTypographyRelayoutTest004
 * @tc.desc: test for relayout paragraph style default font size
 * @tc.type: FUNC
 */
HWTEST_F(TypographyRelayoutTest, OHDrawingTypographyRelayoutTest004, TestSize.Level0)
{
    double maxWidth = LAYOUT_WIDTH;
    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    std::u16string text = u"relayout test";
    OHOS::Rosen::TextStyle textStyle;
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    typography->Layout(maxWidth);
    double preLongestLineWithIndent = typography->GetLongestLineWithIndent();

    typographyStyle.fontSize = 100;
    std::bitset<static_cast<size_t>(RelayoutParagraphStyleAttribute::PARAGRAPH_STYLE_ATTRIBUTE_BUTT)> styleBitset;
    styleBitset.set(static_cast<size_t>(RelayoutParagraphStyleAttribute::FONT_SIZE));
    typographyStyle.relayoutChangeBitmap = styleBitset;
    std::vector<OHOS::Rosen::TextStyle> relayoutTextStyles;
    relayoutTextStyles.push_back(textStyle);
    typography->Relayout(maxWidth, typographyStyle, relayoutTextStyles);
    double relayoutLongestLineWithIndent = typography->GetLongestLineWithIndent();

    EXPECT_FALSE(skia::textlayout::nearlyEqual(preLongestLineWithIndent, relayoutLongestLineWithIndent));
}

/*
 * @tc.name: OHDrawingTypographyRelayoutTest005
 * @tc.desc: test for relayout paragraph style maxlines
 * @tc.type: FUNC
 */
HWTEST_F(TypographyRelayoutTest, OHDrawingTypographyRelayoutTest005, TestSize.Level0)
{
    double maxWidth = LAYOUT_WIDTH;
    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    std::u16string text = u"relayout test";
    OHOS::Rosen::TextStyle textStyle;
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    typography->Layout(maxWidth);
    int preLineCnt = typography->GetLineCount();

    typographyStyle.maxLines = 0;
    std::bitset<static_cast<size_t>(RelayoutParagraphStyleAttribute::PARAGRAPH_STYLE_ATTRIBUTE_BUTT)> styleBitset;
    styleBitset.set(static_cast<size_t>(RelayoutParagraphStyleAttribute::MAXLINES));
    typographyStyle.relayoutChangeBitmap = styleBitset;
    std::vector<OHOS::Rosen::TextStyle> relayoutTextStyles;
    relayoutTextStyles.push_back(textStyle);
    typography->Relayout(maxWidth, typographyStyle, relayoutTextStyles);
    int relayoutLineCnt = typography->GetLineCount();

    EXPECT_NE(preLineCnt, relayoutLineCnt);
}

/*
 * @tc.name: OHDrawingTypographyRelayoutTest006
 * @tc.desc: test for relayout paragraph style ellipsis
 * @tc.type: FUNC
 */
HWTEST_F(TypographyRelayoutTest, OHDrawingTypographyRelayoutTest006, TestSize.Level0)
{
    double maxWidth = 50;
    OHOS::Rosen::TypographyStyle typographyStyle;
    typographyStyle.wordBreakType = WordBreakType::NORMAL;
    typographyStyle.ellipsisModal = EllipsisModal::TAIL;
    typographyStyle.maxLines = 1;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    std::u16string text = u"relayout test";
    OHOS::Rosen::TextStyle textStyle;
    typographyCreate->PushStyle(textStyle);
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    typography->Layout(maxWidth);
    double preLongestLineWithIndent = typography->GetLongestLineWithIndent();

    typographyStyle.ellipsis = u"省略号";
    std::bitset<static_cast<size_t>(RelayoutParagraphStyleAttribute::PARAGRAPH_STYLE_ATTRIBUTE_BUTT)> styleBitset;
    styleBitset.set(static_cast<size_t>(RelayoutParagraphStyleAttribute::ELLIPSIS));
    typographyStyle.relayoutChangeBitmap = styleBitset;
    std::vector<OHOS::Rosen::TextStyle> relayoutTextStyles;
    relayoutTextStyles.push_back(textStyle);
    typography->Relayout(maxWidth, typographyStyle, relayoutTextStyles);
    double relayoutLongestLineWithIndent = typography->GetLongestLineWithIndent();

    EXPECT_FALSE(skia::textlayout::nearlyEqual(preLongestLineWithIndent, relayoutLongestLineWithIndent));
}

/*
 * @tc.name: OHDrawingTypographyRelayoutTest007
 * @tc.desc: test for relayout paragraph style ellipsis modal
 * @tc.type: FUNC
 */
HWTEST_F(TypographyRelayoutTest, OHDrawingTypographyRelayoutTest007, TestSize.Level0)
{
    double maxWidth = 50;
    OHOS::Rosen::TypographyStyle typographyStyle;
    typographyStyle.ellipsisModal = EllipsisModal::TAIL;
    typographyStyle.maxLines = 1;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    std::u16string text = u"测试relayout test";
    OHOS::Rosen::TextStyle textStyle;
    typographyCreate->PushStyle(textStyle);
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    typography->Layout(maxWidth);
    double preLongestLineWithIndent = typography->GetLongestLineWithIndent();

    typographyStyle.ellipsisModal = EllipsisModal::HEAD;
    std::bitset<static_cast<size_t>(RelayoutParagraphStyleAttribute::PARAGRAPH_STYLE_ATTRIBUTE_BUTT)> styleBitset;
    styleBitset.set(static_cast<size_t>(RelayoutParagraphStyleAttribute::ELLIPSIS_MODAL));
    typographyStyle.relayoutChangeBitmap = styleBitset;
    std::vector<OHOS::Rosen::TextStyle> relayoutTextStyles;
    relayoutTextStyles.push_back(textStyle);
    typography->Relayout(maxWidth, typographyStyle, relayoutTextStyles);
    double relayoutLongestLineWithIndent = typography->GetLongestLineWithIndent();

    EXPECT_FALSE(skia::textlayout::nearlyEqual(preLongestLineWithIndent, relayoutLongestLineWithIndent));
}

/*
 * @tc.name: OHDrawingTypographyRelayoutTest008
 * @tc.desc: test for relayout textstyle font size
 * @tc.type: FUNC
 */
HWTEST_F(TypographyRelayoutTest, OHDrawingTypographyRelayoutTest008, TestSize.Level0)
{
    double maxWidth = LAYOUT_WIDTH;
    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    std::u16string text = u"relayout test";
    OHOS::Rosen::TextStyle textStyle;
    textStyle.textStyleUid = UNIQUEID;
    textStyle.fontSize = 10;
    typographyCreate->PushStyle(textStyle);
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    typography->Layout(maxWidth);
    double preLongestLineWithIndent = typography->GetLongestLineWithIndent();

    std::bitset<static_cast<size_t>(RelayoutTextStyleAttribute::TEXT_STYLE_ATTRIBUTE_BUTT)> styleBitset;
    styleBitset.set(static_cast<size_t>(RelayoutTextStyleAttribute::FONT_SIZE));
    textStyle.fontSize = 100;
    textStyle.relayoutChangeBitmap = styleBitset;
    std::vector<OHOS::Rosen::TextStyle> relayoutTextStyles;
    relayoutTextStyles.push_back(textStyle);
    typography->Relayout(maxWidth, typographyStyle, relayoutTextStyles);
    double relayoutLongestLineWithIndent = typography->GetLongestLineWithIndent();

    EXPECT_TRUE(skia::textlayout::nearlyEqual(10 * preLongestLineWithIndent, relayoutLongestLineWithIndent));
}

/*
 * @tc.name: OHDrawingTypographyRelayoutTest009
 * @tc.desc: test for relayout textstyle font variation
 * @tc.type: FUNC
 */
HWTEST_F(TypographyRelayoutTest, OHDrawingTypographyRelayoutTest009, TestSize.Level0)
{
    double maxWidth = LAYOUT_WIDTH;
    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    std::u16string text = u"relayout test";
    OHOS::Rosen::TextStyle textStyle;
    textStyle.textStyleUid = UNIQUEID;
    typographyCreate->PushStyle(textStyle);
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    typography->Layout(maxWidth);
    double preLongestLineWithIndent = typography->GetLongestLineWithIndent();

    std::bitset<static_cast<size_t>(RelayoutTextStyleAttribute::TEXT_STYLE_ATTRIBUTE_BUTT)> styleBitset;
    styleBitset.set(static_cast<size_t>(RelayoutTextStyleAttribute::FONT_VARIATIONS));
    textStyle.relayoutChangeBitmap = styleBitset;
    textStyle.fontVariations.SetAxisValue("wght", 900);
    std::vector<OHOS::Rosen::TextStyle> relayoutTextStyles;
    relayoutTextStyles.push_back(textStyle);
    typography->Relayout(maxWidth, typographyStyle, relayoutTextStyles);
    double relayoutLongestLineWithIndent = typography->GetLongestLineWithIndent();

    EXPECT_FALSE(skia::textlayout::nearlyEqual(preLongestLineWithIndent, relayoutLongestLineWithIndent));
}

/*
 * @tc.name: OHDrawingTypographyRelayoutTest010
 * @tc.desc: test for relayout textstyle font weight
 * @tc.type: FUNC
 */
HWTEST_F(TypographyRelayoutTest, OHDrawingTypographyRelayoutTest010, TestSize.Level0)
{
    double maxWidth = LAYOUT_WIDTH;
    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    std::u16string text = u"relayout test";
    OHOS::Rosen::TextStyle textStyle;
    textStyle.textStyleUid = UNIQUEID;
    textStyle.fontWeight = FontWeight::W100;
    typographyCreate->PushStyle(textStyle);
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    typography->Layout(maxWidth);
    double preLongestLineWithIndent = typography->GetLongestLineWithIndent();

    std::bitset<static_cast<size_t>(RelayoutTextStyleAttribute::TEXT_STYLE_ATTRIBUTE_BUTT)> styleBitset;
    styleBitset.set(static_cast<size_t>(RelayoutTextStyleAttribute::FONT_WEIGHT));
    textStyle.fontWeight = FontWeight::W900;
    textStyle.relayoutChangeBitmap = styleBitset;
    std::vector<OHOS::Rosen::TextStyle> relayoutTextStyles;
    relayoutTextStyles.push_back(textStyle);
    typography->Relayout(maxWidth, typographyStyle, relayoutTextStyles);
    double relayoutLongestLineWithIndent = typography->GetLongestLineWithIndent();

    EXPECT_FALSE(skia::textlayout::nearlyEqual(preLongestLineWithIndent, relayoutLongestLineWithIndent));
}

/*
 * @tc.name: OHDrawingTypographyRelayoutTest011
 * @tc.desc: test for relayout textstyle letter spacing
 * @tc.type: FUNC
 */
HWTEST_F(TypographyRelayoutTest, OHDrawingTypographyRelayoutTest011, TestSize.Level0)
{
    double maxWidth = LAYOUT_WIDTH;
    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    std::u16string text = u"test";
    OHOS::Rosen::TextStyle textStyle;
    textStyle.textStyleUid = UNIQUEID;
    typographyCreate->PushStyle(textStyle);
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    typography->Layout(maxWidth);
    double preLongestLineWithIndent = typography->GetLongestLineWithIndent();

    std::bitset<static_cast<size_t>(RelayoutTextStyleAttribute::TEXT_STYLE_ATTRIBUTE_BUTT)> styleBitset;
    styleBitset.set(static_cast<size_t>(RelayoutTextStyleAttribute::LETTER_SPACING));
    textStyle.relayoutChangeBitmap = styleBitset;
    textStyle.letterSpacing = 10;
    std::vector<OHOS::Rosen::TextStyle> relayoutTextStyles;
    relayoutTextStyles.push_back(textStyle);
    typography->Relayout(maxWidth, typographyStyle, relayoutTextStyles);
    double relayoutLongestLineWithIndent = typography->GetLongestLineWithIndent();

    EXPECT_TRUE(skia::textlayout::nearlyEqual(relayoutLongestLineWithIndent, (preLongestLineWithIndent + 4 * 10)));
}

/*
 * @tc.name: OHDrawingTypographyRelayoutTest012
 * @tc.desc: test for relayout textstyle height override
 * @tc.type: FUNC
 */
HWTEST_F(TypographyRelayoutTest, OHDrawingTypographyRelayoutTest012, TestSize.Level0)
{
    double maxWidth = LAYOUT_WIDTH;
    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    std::u16string text = u"relayout test";
    OHOS::Rosen::TextStyle textStyle;
    textStyle.textStyleUid = UNIQUEID;
    textStyle.heightOnly = true;
    textStyle.heightScale = 2;
    typographyCreate->PushStyle(textStyle);
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    typography->Layout(maxWidth);
    double preParagraphHeight = typography->GetHeight();

    std::bitset<static_cast<size_t>(RelayoutTextStyleAttribute::TEXT_STYLE_ATTRIBUTE_BUTT)> styleBitset;
    styleBitset.set(static_cast<size_t>(RelayoutTextStyleAttribute::HEIGHT_ONLY));
    textStyle.relayoutChangeBitmap = styleBitset;
    textStyle.heightOnly = false;
    std::vector<OHOS::Rosen::TextStyle> relayoutTextStyles;
    relayoutTextStyles.push_back(textStyle);
    typography->Relayout(maxWidth, typographyStyle, relayoutTextStyles);
    double relayoutParagraphHeight = typography->GetHeight();

    EXPECT_FALSE(skia::textlayout::nearlyEqual(preParagraphHeight, relayoutParagraphHeight));
}

/*
 * @tc.name: OHDrawingTypographyRelayoutTest013
 * @tc.desc: test for relayout textstyle font feature
 * @tc.type: FUNC
 */
HWTEST_F(TypographyRelayoutTest, OHDrawingTypographyRelayoutTest013, TestSize.Level0)
{
    double maxWidth = LAYOUT_WIDTH;
    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    std::u16string text = u"rf";
    OHOS::Rosen::TextStyle textStyle;
    textStyle.textStyleUid = UNIQUEID;
    typographyCreate->PushStyle(textStyle);
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    typography->Layout(maxWidth);
    double preLongestLineWithIndent = typography->GetLongestLineWithIndent();

    std::bitset<static_cast<size_t>(RelayoutTextStyleAttribute::TEXT_STYLE_ATTRIBUTE_BUTT)> styleBitset;
    styleBitset.set(static_cast<size_t>(RelayoutTextStyleAttribute::FONT_FEATURES));
    textStyle.relayoutChangeBitmap = styleBitset;
    textStyle.fontFeatures.SetFeature("liga", 0);
    std::vector<OHOS::Rosen::TextStyle> relayoutTextStyles;
    relayoutTextStyles.push_back(textStyle);
    typography->Relayout(maxWidth, typographyStyle, relayoutTextStyles);
    double relayoutLongestLineWithIndent = typography->GetLongestLineWithIndent();

    EXPECT_FALSE(skia::textlayout::nearlyEqual(preLongestLineWithIndent, relayoutLongestLineWithIndent));
}

/*
 * @tc.name: OHDrawingTypographyRelayoutTest014
 * @tc.desc: test for relayout strut style height and height only
 * @tc.type: FUNC
 */
HWTEST_F(TypographyRelayoutTest, OHDrawingTypographyRelayoutTest014, TestSize.Level0)
{
    double maxWidth = LAYOUT_WIDTH;
    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    std::u16string text = u"relayout test";
    OHOS::Rosen::TextStyle textStyle;
    textStyle.textStyleUid = UNIQUEID;
    textStyle.fontSize = 14;
    typographyCreate->PushStyle(textStyle);
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    typography->Layout(maxWidth);
    double preParagraphHeight = typography->GetHeight();

    typographyStyle.useLineStyle = true;
    typographyStyle.lineStyleHeightScale = 10;
    typographyStyle.lineStyleHeightOnly = true;
    std::bitset<static_cast<size_t>(RelayoutParagraphStyleAttribute::PARAGRAPH_STYLE_ATTRIBUTE_BUTT)> styleBitset;
    styleBitset.set(static_cast<size_t>(RelayoutParagraphStyleAttribute::USE_LINE_STYLE));
    styleBitset.set(static_cast<size_t>(RelayoutParagraphStyleAttribute::LINE_STYLE_HEIGHT_SCALE));
    styleBitset.set(static_cast<size_t>(RelayoutParagraphStyleAttribute::LINE_STYLE_HEIGHT_ONLY));
    typographyStyle.relayoutChangeBitmap = styleBitset;
    std::vector<OHOS::Rosen::TextStyle> relayoutTextStyles;
    relayoutTextStyles.push_back(textStyle);
    typography->Relayout(maxWidth, typographyStyle, relayoutTextStyles);
    double relayoutParagraphHeight = typography->GetHeight();

    EXPECT_FALSE(skia::textlayout::nearlyEqual(preParagraphHeight, relayoutParagraphHeight));
}

/*
 * @tc.name: OHDrawingTypographyRelayoutTest015
 * @tc.desc: test for relayout style use line style
 * @tc.type: FUNC
 */
HWTEST_F(TypographyRelayoutTest, OHDrawingTypographyRelayoutTest015, TestSize.Level0)
{
    double maxWidth = LAYOUT_WIDTH;
    OHOS::Rosen::TypographyStyle typographyStyle;
    typographyStyle.useLineStyle = true;
    typographyStyle.lineStyleFontSize = 50;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    std::u16string text = u"relayout test";
    OHOS::Rosen::TextStyle textStyle;
    textStyle.textStyleUid = UNIQUEID;
    textStyle.fontSize = 14;
    typographyCreate->PushStyle(textStyle);
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    typography->Layout(maxWidth);
    double preParagraphHeight = typography->GetHeight();

    typographyStyle.useLineStyle = false;
    std::bitset<static_cast<size_t>(RelayoutParagraphStyleAttribute::PARAGRAPH_STYLE_ATTRIBUTE_BUTT)> styleBitset;
    styleBitset.set(static_cast<size_t>(RelayoutParagraphStyleAttribute::USE_LINE_STYLE));
    typographyStyle.relayoutChangeBitmap = styleBitset;
    std::vector<OHOS::Rosen::TextStyle> relayoutTextStyles;
    relayoutTextStyles.push_back(textStyle);
    typography->Relayout(maxWidth, typographyStyle, relayoutTextStyles);
    double relayoutParagraphHeight = typography->GetHeight();

    EXPECT_FALSE(skia::textlayout::nearlyEqual(preParagraphHeight, relayoutParagraphHeight));
}

/*
 * @tc.name: OHDrawingTypographyRelayoutTest016
 * @tc.desc: test for relayout strut style font size
 * @tc.type: FUNC
 */
HWTEST_F(TypographyRelayoutTest, OHDrawingTypographyRelayoutTest016, TestSize.Level0)
{
    double maxWidth = LAYOUT_WIDTH;
    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    std::u16string text = u"relayout test";
    OHOS::Rosen::TextStyle textStyle;
    textStyle.textStyleUid = UNIQUEID;
    textStyle.fontSize = 14;
    typographyCreate->PushStyle(textStyle);
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    typography->Layout(maxWidth);
    double preParagraphHeight = typography->GetHeight();

    typographyStyle.useLineStyle = true;
    typographyStyle.lineStyleFontSize = 50;
    std::bitset<static_cast<size_t>(RelayoutParagraphStyleAttribute::PARAGRAPH_STYLE_ATTRIBUTE_BUTT)> styleBitset;
    styleBitset.set(static_cast<size_t>(RelayoutParagraphStyleAttribute::USE_LINE_STYLE));
    styleBitset.set(static_cast<size_t>(RelayoutParagraphStyleAttribute::LINE_STYLE_FONT_SIZE));
    typographyStyle.relayoutChangeBitmap = styleBitset;
    std::vector<OHOS::Rosen::TextStyle> relayoutTextStyles;
    relayoutTextStyles.push_back(textStyle);
    typography->Relayout(maxWidth, typographyStyle, relayoutTextStyles);
    double relayoutParagraphHeight = typography->GetHeight();

    EXPECT_FALSE(skia::textlayout::nearlyEqual(preParagraphHeight, relayoutParagraphHeight));
}

/*
 * @tc.name: OHDrawingTypographyRelayoutTest017
 * @tc.desc: test obtaining some textStyle property from runMetrics part one
 * @tc.type: FUNC
 */
HWTEST_F(TypographyRelayoutTest, OHDrawingTypographyRelayoutTest017, TestSize.Level0)
{
    double maxWidth = LAYOUT_WIDTH;
    OHOS::Rosen::TypographyStyle typographyStyle;
    OHOS::Rosen::TextStyle textStyle;
    textStyle.textStyleUid = UNIQUEID;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    typographyCreate->PushStyle(textStyle);
    std::u16string text = u"relayout";
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    typography->Layout(maxWidth);

    textStyle.fontWidth = OHOS::Rosen::FontWidth::ULTRA_EXPANDED;
    textStyle.fontStyle = OHOS::Rosen::FontStyle::ITALIC;
    textStyle.fontFamilies = {"relayout"};
    textStyle.wordSpacing = 6;
    textStyle.heightOnly = true;
    textStyle.heightScale = 3;
    textStyle.baseLineShift = 11;
    std::bitset<static_cast<size_t>(RelayoutTextStyleAttribute::TEXT_STYLE_ATTRIBUTE_BUTT)> styleBitset;
    styleBitset.set(static_cast<size_t>(RelayoutTextStyleAttribute::FONT_WIDTH));
    styleBitset.set(static_cast<size_t>(RelayoutTextStyleAttribute::FONT_STYLE));
    styleBitset.set(static_cast<size_t>(RelayoutTextStyleAttribute::FONT_FAMILIES));
    styleBitset.set(static_cast<size_t>(RelayoutTextStyleAttribute::WORD_SPACING));
    styleBitset.set(static_cast<size_t>(RelayoutTextStyleAttribute::HEIGHT_ONLY));
    styleBitset.set(static_cast<size_t>(RelayoutTextStyleAttribute::HEIGHT_SCALE));
    styleBitset.set(static_cast<size_t>(RelayoutTextStyleAttribute::BASELINE_SHIFT));
    textStyle.relayoutChangeBitmap = styleBitset;
    std::vector<OHOS::Rosen::TextStyle> relayoutTextStyles;
    relayoutTextStyles.push_back(textStyle);
    typography->Relayout(maxWidth, typographyStyle, relayoutTextStyles);
    std::vector<LineMetrics> myLinesMetric = typography->GetLineMetrics();
    auto runMetrics = myLinesMetric[0].runMetrics;
    for (const auto& item : runMetrics) {
        EXPECT_EQ(static_cast<size_t>(item.second.textStyle->fontWidth),
            static_cast<size_t>(OHOS::Rosen::FontWidth::ULTRA_EXPANDED));
        EXPECT_EQ(item.second.textStyle->fontStyle, OHOS::Rosen::FontStyle::ITALIC);
        EXPECT_TRUE(item.second.textStyle->fontFamilies[0].find("relayout"));
        EXPECT_EQ(item.second.textStyle->wordSpacing, 6);
        EXPECT_EQ(item.second.textStyle->heightOnly, true);
        EXPECT_EQ(item.second.textStyle->heightScale, 3);
        EXPECT_EQ(item.second.textStyle->baseLineShift, 11);
    }
}

/*
 * @tc.name: OHDrawingTypographyRelayoutTest018
 * @tc.desc: test obtaining some textStyle property from runMetrics part two
 * @tc.type: FUNC
 */
HWTEST_F(TypographyRelayoutTest, OHDrawingTypographyRelayoutTest018, TestSize.Level0)
{
    double maxWidth = LAYOUT_WIDTH;
    OHOS::Rosen::TypographyStyle typographyStyle;
    OHOS::Rosen::TextStyle textStyle;
    textStyle.textStyleUid = UNIQUEID;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    typographyCreate->PushStyle(textStyle);
    std::u16string text = u"relayout";
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    typography->Layout(maxWidth);

    textStyle.halfLeading = true;
    TextShadow myShadow;
    myShadow.color = Drawing::Color::ColorQuadSetARGB(255, 0, 255, 255);
    myShadow.blurRadius = 10;
    Drawing::Point myPoint(10, -10);
    myShadow.offset = myPoint;
    TextShadow myShadowNull;
    textStyle.shadows = { myShadow, myShadow, myShadowNull };
    std::bitset<static_cast<size_t>(RelayoutTextStyleAttribute::TEXT_STYLE_ATTRIBUTE_BUTT)> styleBitset;
    styleBitset.set(static_cast<size_t>(RelayoutTextStyleAttribute::SHADOWS));
    styleBitset.set(static_cast<size_t>(RelayoutTextStyleAttribute::HALF_LEADING));
    textStyle.relayoutChangeBitmap = styleBitset;
    std::vector<OHOS::Rosen::TextStyle> relayoutTextStyles;
    relayoutTextStyles.push_back(textStyle);
    typography->Relayout(maxWidth, typographyStyle, relayoutTextStyles);
    auto runMetrics = typography->GetLineMetrics()[0].runMetrics;
    for (const auto& item : runMetrics) {
        EXPECT_EQ(item.second.textStyle->shadows.size(), 3);
        EXPECT_EQ(item.second.textStyle->shadows.at(0).color.CastToColorQuad(),
            Drawing::Color::ColorQuadSetARGB(255, 0, 255, 255));
        EXPECT_EQ(item.second.textStyle->shadows.at(0).HasShadow(), true);
        EXPECT_EQ(item.second.textStyle->shadows.at(0).offset.GetX(), 10);
        EXPECT_EQ(item.second.textStyle->shadows.at(0).offset.GetY(), -10);
        EXPECT_EQ(item.second.textStyle->shadows.at(1).blurRadius, 10);
        EXPECT_EQ(item.second.textStyle->shadows.at(2).HasShadow(), false);
        EXPECT_EQ(item.second.textStyle->halfLeading, true);
    }
}

/*
 * @tc.name: OHDrawingTypographyRelayoutTest019
 * @tc.desc: test obtaining some textStyle property from runMetrics part three
 * @tc.type: FUNC
 */
HWTEST_F(TypographyRelayoutTest, OHDrawingTypographyRelayoutTest019, TestSize.Level0)
{
    double maxWidth = LAYOUT_WIDTH;
    OHOS::Rosen::TypographyStyle typographyStyle;
    OHOS::Rosen::TextStyle textStyle;
    textStyle.textStyleUid = UNIQUEID;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    typographyCreate->PushStyle(textStyle);
    std::u16string text = u"relayout";
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    typography->Layout(maxWidth);

    textStyle.styleId = 17;
    textStyle.color = Drawing::Color::ColorQuadSetARGB(255, 255, 255, 0);
    textStyle.decorationColor = Drawing::Color::ColorQuadSetARGB(255, 0, 255, 0);
    textStyle.decorationStyle = TextDecorationStyle::DASHED;
    textStyle.decorationThicknessScale = 10;
    textStyle.decoration = TextDecoration::OVERLINE;
    textStyle.backgroundRect = { 0xFFFF0000, 16.0, 16.0, 16.0, 16.0 };
    std::bitset<static_cast<size_t>(RelayoutTextStyleAttribute::TEXT_STYLE_ATTRIBUTE_BUTT)> styleBitset;
    styleBitset.set(static_cast<size_t>(RelayoutTextStyleAttribute::DECORATION));
    styleBitset.set(static_cast<size_t>(RelayoutTextStyleAttribute::DECORATION_COLOR));
    styleBitset.set(static_cast<size_t>(RelayoutTextStyleAttribute::DECORATION_STYLE));
    styleBitset.set(static_cast<size_t>(RelayoutTextStyleAttribute::DECORATION_THICKNESS_SCALE));
    styleBitset.set(static_cast<size_t>(RelayoutTextStyleAttribute::BACKGROUND_RECT));
    styleBitset.set(static_cast<size_t>(RelayoutTextStyleAttribute::STYLE_ID));
    styleBitset.set(static_cast<size_t>(RelayoutTextStyleAttribute::FONT_COLOR));
    textStyle.relayoutChangeBitmap = styleBitset;
    std::vector<OHOS::Rosen::TextStyle> relayoutTextStyles;
    relayoutTextStyles.push_back(textStyle);
    typography->Relayout(maxWidth, typographyStyle, relayoutTextStyles);
    auto runMetrics = typography->GetLineMetrics()[0].runMetrics;
    for (const auto& item : runMetrics) {
        EXPECT_EQ(item.second.textStyle->styleId, 17);
        EXPECT_EQ(item.second.textStyle->color.CastToColorQuad(), Drawing::Color::ColorQuadSetARGB(255, 255, 255, 0));
        EXPECT_EQ(
            item.second.textStyle->decorationColor.CastToColorQuad(), Drawing::Color::ColorQuadSetARGB(255, 0, 255, 0));
        EXPECT_EQ(item.second.textStyle->decorationStyle, TextDecorationStyle::DASHED);
        EXPECT_EQ(item.second.textStyle->decorationThicknessScale, 10);
        EXPECT_EQ(item.second.textStyle->decoration, TextDecoration::OVERLINE);
        EXPECT_EQ(item.second.textStyle->backgroundRect.color, 0xFFFF0000);
        EXPECT_TRUE(skia::textlayout::nearlyEqual(item.second.textStyle->backgroundRect.leftBottomRadius, 16.0));
    }
}

/*
 * @tc.name: OHDrawingTypographyRelayoutTest020
 * @tc.desc: test obtaining some paragraph default text style property part one
 * @tc.type: FUNC
 */
HWTEST_F(TypographyRelayoutTest, OHDrawingTypographyRelayoutTest020, TestSize.Level0)
{
    double maxWidth = LAYOUT_WIDTH;
    OHOS::Rosen::TypographyStyle typographyStyle;
    OHOS::Rosen::TextStyle textStyle;
    typographyStyle.defaultTextStyleUid = UNIQUEID;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    std::u16string text = u"relayout";
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    typography->Layout(maxWidth);

    typographyStyle.textDirection = TextDirection::RTL;
    typographyStyle.fontStyle = OHOS::Rosen::FontStyle::ITALIC;
    typographyStyle.fontWeight = FontWeight::W900;
    typographyStyle.fontFamily = "relayout";

    std::bitset<static_cast<size_t>(RelayoutParagraphStyleAttribute::PARAGRAPH_STYLE_ATTRIBUTE_BUTT)>
        paragraphStyleBitset;
    paragraphStyleBitset.set(static_cast<size_t>(RelayoutParagraphStyleAttribute::DIRECTION));
    paragraphStyleBitset.set(static_cast<size_t>(RelayoutParagraphStyleAttribute::FONT_WEIGHT));
    paragraphStyleBitset.set(static_cast<size_t>(RelayoutParagraphStyleAttribute::FONT_STYLE));
    paragraphStyleBitset.set(static_cast<size_t>(RelayoutParagraphStyleAttribute::FONT_FAMILY));
    typographyStyle.relayoutChangeBitmap = paragraphStyleBitset;
    std::vector<OHOS::Rosen::TextStyle> relayoutTextStyles;
    relayoutTextStyles.push_back(textStyle);
    typography->Relayout(maxWidth, typographyStyle, relayoutTextStyles);
    std::vector<LineMetrics> myLinesMetric = typography->GetLineMetrics();
    auto runMetrics = myLinesMetric[0].runMetrics;
    auto paragraphImpl = reinterpret_cast<OHOS::Rosen::SPText::ParagraphImpl*>(typography->GetParagraph());
    auto skiaParagraphStyle = paragraphImpl->paragraph_->fParagraphStyle;

    EXPECT_EQ(static_cast<size_t>(skiaParagraphStyle.getTextDirection()), 0);
    for (const auto& item : runMetrics) {
        EXPECT_EQ(item.second.textStyle->fontStyle, OHOS::Rosen::FontStyle::ITALIC);
        EXPECT_EQ(static_cast<size_t>(item.second.textStyle->fontWeight),
            static_cast<size_t>(OHOS::Rosen::FontWeight::W900));
        EXPECT_TRUE(item.second.textStyle->fontFamilies[0].find("relayout"));
    }
}

/*
 * @tc.name: OHDrawingTypographyRelayoutTest021
 * @tc.desc: test obtaining some paragraph default text style property part two
 * @tc.type: FUNC
 */
HWTEST_F(TypographyRelayoutTest, OHDrawingTypographyRelayoutTest021, TestSize.Level0)
{
    double maxWidth = LAYOUT_WIDTH;
    OHOS::Rosen::TypographyStyle typographyStyle;
    OHOS::Rosen::TextStyle textStyle;
    typographyStyle.defaultTextStyleUid = UNIQUEID;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    std::u16string text = u"relayout";
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    typography->Layout(maxWidth);

    typographyStyle.heightOnly = true;
    typographyStyle.heightScale = 3;
    typographyStyle.halfLeading = true;
    typographyStyle.breakStrategy = BreakStrategy::BALANCED;
    typographyStyle.textAlign = TextAlign::CENTER;

    std::bitset<static_cast<size_t>(RelayoutParagraphStyleAttribute::PARAGRAPH_STYLE_ATTRIBUTE_BUTT)>
        paragraphStyleBitset;
    paragraphStyleBitset.set(static_cast<size_t>(RelayoutParagraphStyleAttribute::HEIGHT_SCALE));
    paragraphStyleBitset.set(static_cast<size_t>(RelayoutParagraphStyleAttribute::HEIGHT_ONLY));
    paragraphStyleBitset.set(static_cast<size_t>(RelayoutParagraphStyleAttribute::HALF_LEADING));
    paragraphStyleBitset.set(static_cast<size_t>(RelayoutParagraphStyleAttribute::BREAKSTRAGY));
    paragraphStyleBitset.set(static_cast<size_t>(RelayoutParagraphStyleAttribute::TEXT_ALIGN));
    typographyStyle.relayoutChangeBitmap = paragraphStyleBitset;
    std::vector<OHOS::Rosen::TextStyle> relayoutTextStyles;
    relayoutTextStyles.push_back(textStyle);
    typography->Relayout(maxWidth, typographyStyle, relayoutTextStyles);
    std::vector<LineMetrics> myLinesMetric = typography->GetLineMetrics();
    auto runMetrics = myLinesMetric[0].runMetrics;
    auto paragraphImpl = reinterpret_cast<OHOS::Rosen::SPText::ParagraphImpl*>(typography->GetParagraph());
    auto skiaParagraphStyle = paragraphImpl->paragraph_->fParagraphStyle;

    EXPECT_EQ(static_cast<size_t>(skiaParagraphStyle.getTextAlign()), 2);
    EXPECT_EQ(static_cast<size_t>(skiaParagraphStyle.getStrutStyle().getLineBreakStrategy()), 2);
    for (const auto& item : runMetrics) {
        EXPECT_EQ(item.second.textStyle->heightOnly, true);
        EXPECT_EQ(item.second.textStyle->heightScale, 3);
        EXPECT_EQ(item.second.textStyle->halfLeading, true);
    }
}


/*
 * @tc.name: OHDrawingTypographyRelayoutTest022
 * @tc.desc: test obtaining some paragraph line style part one
 * @tc.type: FUNC
 */
HWTEST_F(TypographyRelayoutTest, OHDrawingTypographyRelayoutTest022, TestSize.Level0)
{
    double maxWidth = LAYOUT_WIDTH;
    OHOS::Rosen::TypographyStyle typographyStyle;
    typographyStyle.defaultTextStyleUid = UNIQUEID;
    OHOS::Rosen::TextStyle textStyle;
    textStyle.textStyleUid = UNIQUEID;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    std::u16string text = u"relayout";
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    typography->Layout(maxWidth);

    typographyStyle.useLineStyle = true;
    typographyStyle.lineStyleFontWeight = FontWeight::W900;
    typographyStyle.lineStyleFontWidth = OHOS::Rosen::FontWidth::ULTRA_EXPANDED;
    typographyStyle.lineStyleFontStyle = OHOS::Rosen::FontStyle::ITALIC;
    typographyStyle.lineStyleFontFamilies = { "relayout" };
    typographyStyle.lineStyleFontSize = 100;
    std::bitset<static_cast<size_t>(RelayoutParagraphStyleAttribute::PARAGRAPH_STYLE_ATTRIBUTE_BUTT)>
    paragraphStyleBitset;
    paragraphStyleBitset.set(static_cast<size_t>(RelayoutParagraphStyleAttribute::USE_LINE_STYLE));
    paragraphStyleBitset.set(static_cast<size_t>(RelayoutParagraphStyleAttribute::LINE_STYLE_FONT_WEIGHT));
    paragraphStyleBitset.set(static_cast<size_t>(RelayoutParagraphStyleAttribute::LINE_STYLE_FONT_WIDTH));
    paragraphStyleBitset.set(static_cast<size_t>(RelayoutParagraphStyleAttribute::LINE_STYLE_FONT_STYLE));
    paragraphStyleBitset.set(static_cast<size_t>(RelayoutParagraphStyleAttribute::LINE_STYLE_FONT_FAMILY));
    paragraphStyleBitset.set(static_cast<size_t>(RelayoutParagraphStyleAttribute::LINE_STYLE_FONT_SIZE));
    typographyStyle.relayoutChangeBitmap = paragraphStyleBitset;
    std::vector<OHOS::Rosen::TextStyle> relayoutTextStyles;
    relayoutTextStyles.push_back(textStyle);
    typography->Relayout(maxWidth, typographyStyle, relayoutTextStyles);
    std::vector<LineMetrics> myLinesMetric = typography->GetLineMetrics();
    auto runMetrics = myLinesMetric[0].runMetrics;
    const auto& paragraphImpl = reinterpret_cast<OHOS::Rosen::SPText::ParagraphImpl*>(typography->GetParagraph());
    const auto& strutStyle = paragraphImpl->paragraph_->getParagraphStyle().getStrutStyle();

    EXPECT_TRUE(static_cast<size_t>(strutStyle.getStrutEnabled()));
    EXPECT_EQ(static_cast<size_t>(static_cast<size_t>(strutStyle.getFontStyle().GetWeight())),
        (static_cast<size_t>(FontWeight::W900) + 1) * 100);
    EXPECT_EQ(static_cast<size_t>(static_cast<size_t>(strutStyle.getFontStyle().GetWidth())),
        static_cast<size_t>(OHOS::Rosen::FontWidth::ULTRA_EXPANDED));
    EXPECT_EQ(static_cast<size_t>(static_cast<size_t>(strutStyle.getFontStyle().GetSlant())),
        static_cast<size_t>(OHOS::Rosen::FontStyle::ITALIC));
    EXPECT_TRUE(strutStyle.getFontFamilies()[0].contains("relayout"));
    EXPECT_EQ(static_cast<size_t>(strutStyle.getFontSize()), 100);
}

/*
 * @tc.name: OHDrawingTypographyRelayoutTest023
 * @tc.desc: test obtaining some paragraph line style part two
 * @tc.type: FUNC
 */
HWTEST_F(TypographyRelayoutTest, OHDrawingTypographyRelayoutTest023, TestSize.Level0)
{
    double maxWidth = LAYOUT_WIDTH;
    OHOS::Rosen::TypographyStyle typographyStyle;
    typographyStyle.defaultTextStyleUid = UNIQUEID;
    OHOS::Rosen::TextStyle textStyle;
    textStyle.textStyleUid = UNIQUEID;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    std::u16string text = u"relayout";
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    typography->Layout(maxWidth);

    typographyStyle.lineStyleHeightScale = 3;
    typographyStyle.lineStyleHeightOnly = true;
    typographyStyle.lineStyleHalfLeading = true;
    typographyStyle.lineStyleOnly = true;
    typographyStyle.wordBreakType = WordBreakType::BREAK_ALL;
    std::bitset<static_cast<size_t>(RelayoutParagraphStyleAttribute::PARAGRAPH_STYLE_ATTRIBUTE_BUTT)>
    paragraphStyleBitset;
    paragraphStyleBitset.set(static_cast<size_t>(RelayoutParagraphStyleAttribute::LINE_STYLE_HEIGHT_SCALE));
    paragraphStyleBitset.set(static_cast<size_t>(RelayoutParagraphStyleAttribute::LINE_STYLE_HEIGHT_ONLY));
    paragraphStyleBitset.set(static_cast<size_t>(RelayoutParagraphStyleAttribute::LINE_STYLE_HALF_LEADING));
    paragraphStyleBitset.set(static_cast<size_t>(RelayoutParagraphStyleAttribute::LINE_STYLE_ONLY));
    paragraphStyleBitset.set(static_cast<size_t>(RelayoutParagraphStyleAttribute::WORD_BREAKTYPE));
    typographyStyle.relayoutChangeBitmap = paragraphStyleBitset;
    std::vector<OHOS::Rosen::TextStyle> relayoutTextStyles;
    relayoutTextStyles.push_back(textStyle);
    typography->Relayout(maxWidth, typographyStyle, relayoutTextStyles);
    std::vector<LineMetrics> myLinesMetric = typography->GetLineMetrics();
    auto runMetrics = myLinesMetric[0].runMetrics;
    const auto& paragraphImpl = reinterpret_cast<OHOS::Rosen::SPText::ParagraphImpl*>(typography->GetParagraph());
    const auto& strutStyle = paragraphImpl->paragraph_->getParagraphStyle().getStrutStyle();

    EXPECT_EQ(static_cast<size_t>(strutStyle.getHeight()), 3);
    EXPECT_TRUE(static_cast<size_t>(strutStyle.getHeightOverride()));
    EXPECT_TRUE(static_cast<size_t>(strutStyle.getHalfLeading()));
    EXPECT_TRUE(static_cast<size_t>(strutStyle.getForceStrutHeight()));
    EXPECT_EQ(static_cast<size_t>(static_cast<size_t>(strutStyle.getWordBreakType())),
        static_cast<size_t>(WordBreakType::BREAK_ALL));
}


/*
 * @tc.name: OHDrawingTypographyRelayoutTest024
 * @tc.desc: test for relayout symbol style
 * @tc.type: FUNC
 */
HWTEST_F(TypographyRelayoutTest, OHDrawingTypographyRelayoutTest024, TestSize.Level0)
{
    double maxWidth = LAYOUT_WIDTH;
    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    OHOS::Rosen::TextStyle textStyle;
    textStyle.isSymbolGlyph = true;
    typographyCreate->PushStyle(textStyle);
    typographyCreate->AppendSymbol(983046);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    typography->Layout(maxWidth);

    textStyle.symbol.SetAnimationMode(1);
    textStyle.symbol.SetAnimationStart(true);
    textStyle.symbol.SetCommonSubType(Drawing::DrawingCommonSubType::UP);
    std::vector<Drawing::DrawingSColor> colors;
    colors.push_back({1.0, 255, 0, 0});
    colors.push_back({1.0, 0, 255, 0});
    textStyle.symbol.colorList_ = colors;

    textStyle.symbol.SetRenderMode(Drawing::DrawingSymbolRenderingStrategy::MULTIPLE_OPACITY);
    std::bitset<static_cast<size_t>(RelayoutSymbolStyleAttribute::SYMBOL_ATTRIBUTE_BUTT)> symbolStyleBitset;
    symbolStyleBitset.set(static_cast<size_t>(RelayoutSymbolStyleAttribute::EFFECT_STRATEGY));
    symbolStyleBitset.set(static_cast<size_t>(RelayoutSymbolStyleAttribute::ANIMATION_MODE));
    symbolStyleBitset.set(static_cast<size_t>(RelayoutSymbolStyleAttribute::ANIMATION_START));
    symbolStyleBitset.set(static_cast<size_t>(RelayoutSymbolStyleAttribute::COMMONSUB_TYPE));
    symbolStyleBitset.set(static_cast<size_t>(RelayoutSymbolStyleAttribute::COLOR_LIST));
    symbolStyleBitset.set(static_cast<size_t>(RelayoutSymbolStyleAttribute::RENDER_MODE));
    textStyle.symbol.SetSymbolBitmap(symbolStyleBitset);
    std::vector<OHOS::Rosen::TextStyle> relayoutTextStyles;
    relayoutTextStyles.push_back(textStyle);
    typography->Relayout(maxWidth, typographyStyle, relayoutTextStyles);
    const auto& paragraphImpl = reinterpret_cast<OHOS::Rosen::SPText::ParagraphImpl*>(typography->GetParagraph());
    const auto& symbolTxt = paragraphImpl->hmSymbols_[0]->GetSymbolTxt();
    const auto& symbolColorList = symbolTxt.GetRenderColor();
    bool checkColorOne = (colors[0].r == symbolColorList[0].r && colors[0].g == symbolColorList[0].g &&
        colors[0].b == symbolColorList[0].b);
    EXPECT_EQ(checkColorOne, true);

    bool checkColorTwo = (colors[1].r == symbolColorList[1].r && colors[1].g == symbolColorList[1].g &&
        colors[1].b == symbolColorList[1].b);
    EXPECT_EQ(checkColorTwo, true);

    EXPECT_EQ(symbolTxt.GetAnimationMode(), 1);
    EXPECT_EQ(symbolTxt.GetAnimationStart(), true);
    EXPECT_EQ(symbolTxt.GetCommonSubType(), Drawing::DrawingCommonSubType::UP);
    EXPECT_EQ(symbolTxt.GetRenderMode(), Drawing::DrawingSymbolRenderingStrategy::MULTIPLE_OPACITY);
}

/*
 * @tc.name: OHDrawingTypographyRelayoutTest025
 * @tc.desc: test the paragraph spacing style and the paragraph end spacing style of relayout
 * @tc.type: FUNC
 */
HWTEST_F(TypographyRelayoutTest, OHDrawingTypographyRelayoutTest025, TestSize.Level0)
{
    const double maxWidth = LAYOUT_WIDTH;
    OHOS::Rosen::TypographyStyle typographyStyle;
    typographyStyle.paragraphSpacing = 0;
    typographyStyle.isEndAddParagraphSpacing = false;

    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    std::u16string text = u"relayout test";
    OHOS::Rosen::TextStyle textStyle;
    typographyCreate->PushStyle(textStyle);
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    typography->Layout(maxWidth);
    double preParagraphyHeight = typography->GetHeight();

    typographyStyle.paragraphSpacing = 40;
    typographyStyle.isEndAddParagraphSpacing = true;
    std::bitset<static_cast<size_t>(RelayoutParagraphStyleAttribute::PARAGRAPH_STYLE_ATTRIBUTE_BUTT)> styleBitset;
    styleBitset.set(static_cast<size_t>(RelayoutParagraphStyleAttribute::SPACING));
    styleBitset.set(static_cast<size_t>(RelayoutParagraphStyleAttribute::SPACING_IS_END));
    typographyStyle.relayoutChangeBitmap = styleBitset;

    std::vector<TextStyle> relayoutTextStyles;
    relayoutTextStyles.push_back(textStyle);
    typography->Relayout(maxWidth, typographyStyle, relayoutTextStyles);
    double relayoutParagraphyHeight = typography->GetHeight();
    EXPECT_TRUE(skia::textlayout::nearlyEqual(preParagraphyHeight + 40, relayoutParagraphyHeight));
}

/*
 * @tc.name: OHDrawingTypographyRelayoutTest026
 * @tc.desc: test the paragraph spacing style and the paragraph end spacing style of relayout(ineffective)
 * @tc.type: FUNC
 */
HWTEST_F(TypographyRelayoutTest, OHDrawingTypographyRelayoutTest026, TestSize.Level0)
{
    const double maxWidth = LAYOUT_WIDTH;
    OHOS::Rosen::TypographyStyle typographyStyle;
    typographyStyle.paragraphSpacing = 0;
    typographyStyle.isEndAddParagraphSpacing = false;

    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    std::u16string text = u"relayout test";
    OHOS::Rosen::TextStyle textStyle;
    typographyCreate->PushStyle(textStyle);
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    typography->Layout(maxWidth);
    double preParagraphyHeight = typography->GetHeight();

    std::bitset<static_cast<size_t>(RelayoutParagraphStyleAttribute::PARAGRAPH_STYLE_ATTRIBUTE_BUTT)> styleBitset;
    styleBitset.set(static_cast<size_t>(RelayoutParagraphStyleAttribute::SPACING));
    typographyStyle.relayoutChangeBitmap = styleBitset;
    typographyStyle.paragraphSpacing = 40;
    std::vector<TextStyle> relayoutTextStyles;
    relayoutTextStyles.push_back(textStyle);
    typography->Relayout(maxWidth, typographyStyle, relayoutTextStyles);
    double relayoutParagraphyHeight1 = typography->GetHeight();
    EXPECT_TRUE(skia::textlayout::nearlyEqual(preParagraphyHeight, relayoutParagraphyHeight1));

    typographyStyle.paragraphSpacing = -40;
    typographyStyle.isEndAddParagraphSpacing = true;
    styleBitset.set(static_cast<size_t>(RelayoutParagraphStyleAttribute::SPACING_IS_END));
    typographyStyle.relayoutChangeBitmap = styleBitset;
    typography->Relayout(maxWidth, typographyStyle, relayoutTextStyles);
    double relayoutParagraphyHeight2 = typography->GetHeight();
    EXPECT_TRUE(skia::textlayout::nearlyEqual(preParagraphyHeight, relayoutParagraphyHeight2));
}

/*
 * @tc.name: OHDrawingTypographyRelayoutTest027
 * @tc.desc: test the text height behavior style of relayout
 * @tc.type: FUNC
 */
HWTEST_F(TypographyRelayoutTest, OHDrawingTypographyRelayoutTest027, TestSize.Level0)
{
    constexpr double maxWidth = 50;
    OHOS::Rosen::TypographyStyle typographyStyle;
    typographyStyle.fontSize = 50;
    typographyStyle.textHeightBehavior = TextHeightBehavior::ALL;
    typographyStyle.heightScale = 10.0;
    typographyStyle.heightOnly = true;

    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    std::u16string text = u"中文你好世界。 English Hello World.中文你好世界。 English Hello World.中文你好世界。 English Hello "
        u"World.中文你好世界。 English Hello World.";
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    typography->Layout(maxWidth);
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typography->GetHeight(), 38000));

    std::bitset<static_cast<size_t>(RelayoutParagraphStyleAttribute::PARAGRAPH_STYLE_ATTRIBUTE_BUTT)> styleBitset;
    styleBitset.set(static_cast<size_t>(RelayoutParagraphStyleAttribute::TEXT_HEIGHT_BEHAVIOR));
    typographyStyle.relayoutChangeBitmap = styleBitset;

    std::vector<TextStyle> relayoutTextStyles;
    typographyStyle.textHeightBehavior = TextHeightBehavior::DISABLE_FIRST_ASCENT;
    typography->Relayout(maxWidth, typographyStyle, relayoutTextStyles);
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typography->GetHeight(), 37650));

    typographyStyle.textHeightBehavior = TextHeightBehavior::DISABLE_LAST_ASCENT;
    typography->Relayout(maxWidth, typographyStyle, relayoutTextStyles);
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typography->GetHeight(), 37908));

    typographyStyle.textHeightBehavior = TextHeightBehavior::DISABLE_ALL;
    typography->Relayout(maxWidth, typographyStyle, relayoutTextStyles);
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typography->GetHeight(), 37558));
}

/*
 * @tc.name: OHDrawingTypographyRelayoutTest028
 * @tc.desc: Test for multiple textStyle when changing for font color
 * @tc.type: FUNC
 */
HWTEST_F(TypographyRelayoutTest, OHDrawingTypographyRelayoutTest028, TestSize.Level0)
{
    double maxWidth = LAYOUT_WIDTH;
    OHOS::Rosen::TypographyStyle typographyStyle;
    OHOS::Rosen::TextStyle textStyle;
    textStyle.textStyleUid = UNIQUEID;
    textStyle.color = Drawing::Color::ColorQuadSetARGB(255, 200, 0, 0);
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    typographyCreate->PushStyle(textStyle);
    std::u16string text = u"你好世界";
    typographyCreate->AppendText(text);
    OHOS::Rosen::TextStyle textStyleExtra;
    textStyleExtra.fontSize = 50;
    textStyleExtra.textStyleUid = UNIQUEID + 1;
    textStyleExtra.color = Drawing::Color::ColorQuadSetARGB(255, 255, 0, 0);
    typographyCreate->PushStyle(textStyleExtra);
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    typography->Layout(maxWidth);

    textStyleExtra.color = Drawing::Color::ColorQuadSetARGB(255, 255, 255, 0);
    std::bitset<static_cast<size_t>(RelayoutTextStyleAttribute::TEXT_STYLE_ATTRIBUTE_BUTT)> styleBitset;
    textStyle.relayoutChangeBitmap = styleBitset;
    styleBitset.set(static_cast<size_t>(RelayoutTextStyleAttribute::FONT_COLOR));
    textStyleExtra.relayoutChangeBitmap = styleBitset;
    std::vector<OHOS::Rosen::TextStyle> relayoutTextStyles;
    relayoutTextStyles.push_back(textStyle);
    relayoutTextStyles.push_back(textStyleExtra);
    typography->Relayout(maxWidth, typographyStyle, relayoutTextStyles);
    auto paragraphImpl = reinterpret_cast<OHOS::Rosen::SPText::ParagraphImpl*>(typography->GetParagraph());
    auto skiaTextStyles = paragraphImpl->paragraph_->exportTextStyles();
    EXPECT_EQ(skiaTextStyles[0].fStyle.getColor(), Drawing::Color::ColorQuadSetARGB(255, 200, 0, 0));
    EXPECT_EQ(skiaTextStyles[1].fStyle.getColor(), Drawing::Color::ColorQuadSetARGB(255, 255, 255, 0));
}

/*
 * @tc.name: OHDrawingTypographyRelayoutTest029
 * @tc.desc: Test for paragraph's paints when changing for font color
 * @tc.type: FUNC
 */
HWTEST_F(TypographyRelayoutTest, OHDrawingTypographyRelayoutTest029, TestSize.Level0)
{
    double maxWidth = LAYOUT_WIDTH;
    OHOS::Rosen::TypographyStyle typographyStyle;
    OHOS::Rosen::TextStyle textStyle;
    textStyle.color = Drawing::Color::ColorQuadSetARGB(255, 200, 0, 0);
    textStyle.textStyleUid = UNIQUEID;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    typographyCreate->PushStyle(textStyle);
    std::u16string text = u"中文你好世界";
    typographyCreate->AppendText(text);
    OHOS::Rosen::TextStyle textStyleExtra;
    textStyleExtra.fontSize = 50;
    textStyleExtra.textStyleUid = UNIQUEID + 1;
    textStyleExtra.color = Drawing::Color::ColorQuadSetARGB(255, 255, 0, 0);
    typographyCreate->PushStyle(textStyleExtra);
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    typography->Layout(maxWidth);

    textStyleExtra.color = Drawing::Color::ColorQuadSetARGB(255, 255, 255, 0);
    std::bitset<static_cast<size_t>(RelayoutTextStyleAttribute::TEXT_STYLE_ATTRIBUTE_BUTT)> styleBitset;
    textStyle.relayoutChangeBitmap = styleBitset;
    styleBitset.set(static_cast<size_t>(RelayoutTextStyleAttribute::FONT_COLOR));
    textStyleExtra.relayoutChangeBitmap = styleBitset;
    std::vector<OHOS::Rosen::TextStyle> relayoutTextStyles;
    relayoutTextStyles.push_back(textStyle);
    relayoutTextStyles.push_back(textStyleExtra);
    typography->Relayout(maxWidth, typographyStyle, relayoutTextStyles);
    auto paragraphImpl = reinterpret_cast<OHOS::Rosen::SPText::ParagraphImpl*>(typography->GetParagraph());
    auto paints = paragraphImpl->paints_;
    EXPECT_EQ(paints[1].color, Drawing::Color::ColorQuadSetARGB(255, 200, 0, 0));
    EXPECT_EQ(paints[2].color, Drawing::Color::ColorQuadSetARGB(255, 255, 255, 0));
}

/*
 * @tc.name: OHDrawingTypographyRelayoutTest030
 * @tc.desc: Test for paragraph's paints when changing for relayout width mutil
 * @tc.type: FUNC
 */
HWTEST_F(TypographyRelayoutTest, OHDrawingTypographyRelayoutTest030, TestSize.Level0)
{
    double maxWidth = LAYOUT_WIDTH;
    OHOS::Rosen::TypographyStyle typographyStyle;
    typographyStyle.textAlign = TextAlign::RIGHT;
    OHOS::Rosen::TextStyle textStyle;
    textStyle.textStyleUid = UNIQUEID;
    textStyle.fontSize = 50;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection =
        OHOS::Rosen::FontCollection::From(std::make_shared<txt::FontCollection>());
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate =
        OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    textStyle.color = Drawing::Color::ColorQuadSetARGB(255, 200, 0, 0);
    typographyCreate->PushStyle(textStyle);
    std::u16string text = u"中文你好世界";
    typographyCreate->AppendText(text);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    typography->Layout(maxWidth);

    std::bitset<static_cast<size_t>(RelayoutTextStyleAttribute::TEXT_STYLE_ATTRIBUTE_BUTT)> styleBitset;
    textStyle.relayoutChangeBitmap = styleBitset;
    std::vector<OHOS::Rosen::TextStyle> relayoutTextStyles;
    relayoutTextStyles.push_back(textStyle);
    typography->Relayout(maxWidth, typographyStyle, relayoutTextStyles);
    typography->Relayout(maxWidth / 2, typographyStyle, relayoutTextStyles);
    EXPECT_TRUE(skia::textlayout::nearlyEqual(typography->GetMaxWidth(), maxWidth / 2));
}
} // namespace Rosen
} // namespace OHOS