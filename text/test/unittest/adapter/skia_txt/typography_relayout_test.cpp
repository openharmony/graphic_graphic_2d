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
#include "typography.h"
#include "typography_create.h"
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
 * @tc.desc: test for relayout but not change paragraph style
 * @tc.type: FUNC
 */
HWTEST_F(TypographyRelayoutTest, OHDrawingTypographyRelayoutTest001, TestSize.Level1)
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

    typographyStyle.fontSize = 100;
    std::bitset<static_cast<size_t>(RelayoutParagraphStyleAttribute::PARAGRAPH_STYLE_ATTRIBUTE_BUTT)> styleBitset;
    styleBitset.set(0);
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
HWTEST_F(TypographyRelayoutTest, OHDrawingTypographyRelayoutTest002, TestSize.Level1)
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
HWTEST_F(TypographyRelayoutTest, OHDrawingTypographyRelayoutTest003, TestSize.Level1)
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
HWTEST_F(TypographyRelayoutTest, OHDrawingTypographyRelayoutTest004, TestSize.Level1)
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
    styleBitset.set(0);
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
HWTEST_F(TypographyRelayoutTest, OHDrawingTypographyRelayoutTest005, TestSize.Level1)
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
    styleBitset.set(2);
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
HWTEST_F(TypographyRelayoutTest, OHDrawingTypographyRelayoutTest006, TestSize.Level1)
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
    styleBitset.set(21);
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
HWTEST_F(TypographyRelayoutTest, OHDrawingTypographyRelayoutTest007, TestSize.Level1)
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
    styleBitset.set(22);
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
HWTEST_F(TypographyRelayoutTest, OHDrawingTypographyRelayoutTest008, TestSize.Level1)
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
    styleBitset.set(0);
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
HWTEST_F(TypographyRelayoutTest, OHDrawingTypographyRelayoutTest009, TestSize.Level1)
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
    styleBitset.set(10);
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
HWTEST_F(TypographyRelayoutTest, OHDrawingTypographyRelayoutTest010, TestSize.Level1)
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
    styleBitset.set(1);
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
HWTEST_F(TypographyRelayoutTest, OHDrawingTypographyRelayoutTest011, TestSize.Level1)
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
    styleBitset.set(5);
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
 * @tc.desc: test for relayout textstyle height scale and height override
 * @tc.type: FUNC
 */
HWTEST_F(TypographyRelayoutTest, OHDrawingTypographyRelayoutTest012, TestSize.Level1)
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
    styleBitset.set(7);
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
HWTEST_F(TypographyRelayoutTest, OHDrawingTypographyRelayoutTest013, TestSize.Level1)
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
    styleBitset.set(9);
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
 * @tc.desc: test for relayout not change text style and paragraph style
 * @tc.type: FUNC
 */
HWTEST_F(TypographyRelayoutTest, OHDrawingTypographyRelayoutTest014, TestSize.Level1)
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
    double preLongestLineWithIndent = typography->GetLongestLineWithIndent();

    std::vector<OHOS::Rosen::TextStyle> relayoutTextStyles;
    relayoutTextStyles.push_back(textStyle);
    typography->Relayout(maxWidth, typographyStyle, relayoutTextStyles);
    double relayoutLongestLineWithIndent = typography->GetLongestLineWithIndent();

    EXPECT_TRUE(skia::textlayout::nearlyEqual(preLongestLineWithIndent, relayoutLongestLineWithIndent));
}

/*
 * @tc.name: OHDrawingTypographyRelayoutTest015
 * @tc.desc: test for relayout strut style height and height only
 * @tc.type: FUNC
 */
HWTEST_F(TypographyRelayoutTest, OHDrawingTypographyRelayoutTest015, TestSize.Level1)
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
    styleBitset.set(9);
    styleBitset.set(15);
    styleBitset.set(16);
    typographyStyle.relayoutChangeBitmap = styleBitset;
    std::vector<OHOS::Rosen::TextStyle> relayoutTextStyles;
    relayoutTextStyles.push_back(textStyle);
    typography->Relayout(maxWidth, typographyStyle, relayoutTextStyles);
    double relayoutParagraphHeight = typography->GetHeight();

    EXPECT_FALSE(skia::textlayout::nearlyEqual(preParagraphHeight, relayoutParagraphHeight));
}

/*
 * @tc.name: OHDrawingTypographyRelayoutTest016
 * @tc.desc: test for relayout style use line style
 * @tc.type: FUNC
 */
HWTEST_F(TypographyRelayoutTest, OHDrawingTypographyRelayoutTest016, TestSize.Level1)
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
    styleBitset.set(9);
    typographyStyle.relayoutChangeBitmap = styleBitset;
    std::vector<OHOS::Rosen::TextStyle> relayoutTextStyles;
    relayoutTextStyles.push_back(textStyle);
    typography->Relayout(maxWidth, typographyStyle, relayoutTextStyles);
    double relayoutParagraphHeight = typography->GetHeight();

    EXPECT_FALSE(skia::textlayout::nearlyEqual(preParagraphHeight, relayoutParagraphHeight));
}

/*
 * @tc.name: OHDrawingTypographyRelayoutTest017
 * @tc.desc: test for relayout strut style font size
 * @tc.type: FUNC
 */
HWTEST_F(TypographyRelayoutTest, OHDrawingTypographyRelayoutTest017, TestSize.Level1)
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
    styleBitset.set(9);
    styleBitset.set(14);
    typographyStyle.relayoutChangeBitmap = styleBitset;
    std::vector<OHOS::Rosen::TextStyle> relayoutTextStyles;
    relayoutTextStyles.push_back(textStyle);
    typography->Relayout(maxWidth, typographyStyle, relayoutTextStyles);
    double relayoutParagraphHeight = typography->GetHeight();

    EXPECT_FALSE(skia::textlayout::nearlyEqual(preParagraphHeight, relayoutParagraphHeight));
}
} // namespace Rosen
} // namespace OHOS