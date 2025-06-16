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

#include "font_collection.h"
#include "gtest/gtest.h"
#include "txt/text_bundle_config_parser.h"
#include "typography.h"
#include "typography_create.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
constexpr static float FLOAT_DATA_EPSILON = 1e-6f;
const double DOUBLE_TEST_SIZE_NOT_DEFAULT = 20.0;
const float FLOAT_TEST_SIZE_NOT_DEFAULT = 10.0f;
const int INT_TEST_SIZE_NOT_DEFAULT = 2;
class OH_Drawing_StyleCopyTest : public testing::Test {};

/*
 * @tc.name: copy textstyle
 * @tc.desc: test copyteststyle001: textStyleCopy = textStyle
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_StyleCopyTest, OH_Drawing_CopyTextStyle001, TestSize.Level0)
{
    OHOS::Rosen::TextStyle textStyle;
    std::u16string myEllipisis = u"......";
    textStyle.decorationColor = Drawing::Color::ColorQuadSetARGB(255, 255, 255, 0);
    textStyle.decorationStyle = TextDecorationStyle::DOTTED;
    textStyle.decoration = TextDecoration::OVERLINE;
    textStyle.decorationThicknessScale = DOUBLE_TEST_SIZE_NOT_DEFAULT;
    textStyle.ellipsis = myEllipisis;
    textStyle.ellipsisModal = EllipsisModal::MIDDLE;
    textStyle.textStyleUid = 1;
    textStyle.styleId = 1;
    textStyle.isSymbolGlyph = true;
    HMSymbolTxt mySymbol;
    mySymbol.SetSymbolUid(1);
    HMSymbolTxt mySymbol2 = mySymbol;
    textStyle.symbol = mySymbol2;
    std::bitset<static_cast<size_t>(RelayoutTextStyleAttribute::TEXT_STYLE_ATTRIBUTE_BUTT)> MyRelayoutChangeBitmap;
    textStyle.relayoutChangeBitmap = MyRelayoutChangeBitmap;
    textStyle.badgeType = TextBadgeType::SUBSCRIPT;

    OHOS::Rosen::TextStyle textStyleCopy = textStyle;

    EXPECT_EQ(textStyleCopy.ellipsis, myEllipisis);
    EXPECT_EQ(textStyleCopy.ellipsisModal, EllipsisModal::MIDDLE);
    EXPECT_EQ(textStyleCopy.decorationColor, Drawing::Color::ColorQuadSetARGB(255, 255, 255, 0));
    EXPECT_EQ(textStyleCopy.decorationStyle, TextDecorationStyle::DOTTED);
    EXPECT_EQ(textStyleCopy.decoration, TextDecoration::OVERLINE);
    EXPECT_NEAR(textStyleCopy.decorationThicknessScale, DOUBLE_TEST_SIZE_NOT_DEFAULT, FLOAT_DATA_EPSILON);
    EXPECT_EQ(textStyleCopy.textStyleUid, 1);
    EXPECT_EQ(textStyleCopy.styleId, 1);
    EXPECT_TRUE(textStyleCopy.isSymbolGlyph);
    EXPECT_EQ(textStyleCopy.symbol.GetSymbolUid(), 1);
    EXPECT_EQ(textStyleCopy.relayoutChangeBitmap, MyRelayoutChangeBitmap);
    EXPECT_EQ(textStyleCopy.badgeType, TextBadgeType::SUBSCRIPT);
}

/*
 * @tc.name: copy textstyle
 * @tc.desc: test copyteststyle002: textStyleCopy(textStyle)
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_StyleCopyTest, OH_Drawing_CopyTextStyle002, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    OHOS::Rosen::TextStyle textStyle;
    HMSymbolTxt mySymbol;
    mySymbol.SetSymbolUid(1);
    HMSymbolTxt mySymbol2(mySymbol);
    textStyle.symbol = mySymbol2;
    FontVariations myFontVariations;
    myFontVariations.SetAxisValue("test", FLOAT_TEST_SIZE_NOT_DEFAULT);
    FontFeatures myFontFeatures;
    myFontFeatures.SetFeature("frac", 1);
    myFontFeatures.SetFeature("sups", 1);
    textStyle.fontFeatures = myFontFeatures;
    std::optional<Drawing::Brush> myBrush;
    myBrush->SetARGB(255, 255, 0, 0);
    textStyle.foregroundBrush = myBrush;
    textStyle.backgroundBrush = myBrush;
    std::optional<Drawing::Pen> myPen;
    myPen->SetARGB(255, 0, 255, 255);
    textStyle.foregroundPen = myPen;
    textStyle.backgroundPen = myPen;
    RectStyle myBackgroundRect = { Drawing::Color::ColorQuadSetARGB(255, 255, 255, 0), DOUBLE_TEST_SIZE_NOT_DEFAULT,
        DOUBLE_TEST_SIZE_NOT_DEFAULT, DOUBLE_TEST_SIZE_NOT_DEFAULT, DOUBLE_TEST_SIZE_NOT_DEFAULT };
    textStyle.backgroundRect = myBackgroundRect;

    OHOS::Rosen::TextStyle textStyleCopy(textStyle);

    EXPECT_EQ(textStyleCopy.symbol.GetSymbolUid(), 1);
    EXPECT_FALSE(textStyleCopy.fontVariations.GetAxisValues().empty());
    const std::map<std::string, float>& axisValues = textStyleCopy.fontVariations.GetAxisValues();
    ASSERT_TRUE(axisValues.find("test") != axisValues.end()) << "Key 'test' not found in font variations axis";
    EXPECT_FLOAT_EQ(axisValues.at("test"), FLOAT_TEST_SIZE_NOT_DEFAULT);
    EXPECT_EQ(textStyleCopy.fontFeatures.GetFontFeatures().at(0).first, "frac");
    EXPECT_EQ(textStyleCopy.fontFeatures.GetFontFeatures().at(0).second, 1);
    EXPECT_EQ(textStyleCopy.fontFeatures.GetFontFeatures().at(1).first, "sups");
    EXPECT_EQ(textStyleCopy.fontFeatures.GetFontFeatures().size(), INT_TEST_SIZE_NOT_DEFAULT);
    EXPECT_EQ(textStyleCopy.fontFeatures.GetFeatureSettings(), "frac=1,sups=1");
    EXPECT_EQ(textStyleCopy.fontFeatures.featureSet_.size(), INT_TEST_SIZE_NOT_DEFAULT);
    EXPECT_EQ(textStyleCopy.fontFeatures.featureSet_.at(1).second, 1);
    EXPECT_EQ(textStyleCopy.foregroundBrush, myBrush);
    EXPECT_EQ(textStyleCopy.backgroundBrush, myBrush);
    EXPECT_EQ(textStyleCopy.foregroundPen, myPen);
    EXPECT_EQ(textStyleCopy.backgroundPen, myPen);
    EXPECT_EQ(textStyleCopy.backgroundRect, myBackgroundRect);
}

/*
 * @tc.name: OH_Drawing_CopyParagraphStyle001
 * @tc.desc: test copyParagraphstyle: paragraphStyleCopy = paragraphStyle
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_StyleCopyTest, OH_Drawing_CopyParagraphStyle001, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    typographyStyle.fontWidth = FontWidth::EXTRA_CONDENSED;
    typographyStyle.lineStyleFontWidth = FontWidth::SEMI_CONDENSED;
    typographyStyle.lineStyleHeightOnly = true;
    typographyStyle.lineStyleHeightOnlyInit = true;
    typographyStyle.paragraphSpacing = FLOAT_TEST_SIZE_NOT_DEFAULT;
    typographyStyle.isEndAddParagraphSpacing = true;
    typographyStyle.verticalAlignment = TextVerticalAlign::BOTTOM;
    typographyStyle.textHeightBehavior = TextHeightBehavior::DISABLE_FIRST_ASCENT;
    std::bitset<static_cast<size_t>(RelayoutParagraphStyleAttribute::PARAGRAPH_STYLE_ATTRIBUTE_BUTT)>
        myRelayoutChangeBitmap;
    typographyStyle.relayoutChangeBitmap = myRelayoutChangeBitmap;
    typographyStyle.defaultTextStyleUid = 1;
    typographyStyle.ellipsisModal = EllipsisModal::HEAD;
    typographyStyle.locale = "test";
    typographyStyle.breakStrategy = BreakStrategy::BALANCED;
    typographyStyle.wordBreakType = WordBreakType::BREAK_HYPHEN;
    typographyStyle.textSplitRatio = FLOAT_TEST_SIZE_NOT_DEFAULT;
    typographyStyle.enableAutoSpace = true;
    typographyStyle.isTrailingSpaceOptimized = true;
    TextTab myTab;
    myTab.alignment = TextAlign::CENTER;
    typographyStyle.tab = myTab;

    OHOS::Rosen::TypographyStyle typographyStyleCopy = typographyStyle;
    EXPECT_EQ(typographyStyleCopy.fontWidth, FontWidth::EXTRA_CONDENSED);
    EXPECT_EQ(typographyStyleCopy.lineStyleFontWidth, FontWidth::SEMI_CONDENSED);
    EXPECT_TRUE(typographyStyleCopy.lineStyleHeightOnly);
    EXPECT_TRUE(typographyStyleCopy.lineStyleHeightOnlyInit);
    EXPECT_NEAR(typographyStyleCopy.paragraphSpacing, FLOAT_TEST_SIZE_NOT_DEFAULT, FLOAT_DATA_EPSILON);
    EXPECT_TRUE(typographyStyleCopy.isEndAddParagraphSpacing);
    EXPECT_EQ(typographyStyleCopy.verticalAlignment, TextVerticalAlign::BOTTOM);
    EXPECT_EQ(typographyStyleCopy.textHeightBehavior, TextHeightBehavior::DISABLE_FIRST_ASCENT);
    EXPECT_EQ(typographyStyleCopy.relayoutChangeBitmap, myRelayoutChangeBitmap);
    EXPECT_EQ(typographyStyleCopy.defaultTextStyleUid, 1);
    EXPECT_EQ(typographyStyleCopy.ellipsisModal, EllipsisModal::HEAD);
    EXPECT_EQ(typographyStyleCopy.locale, "test");
    EXPECT_EQ(typographyStyleCopy.breakStrategy, BreakStrategy::BALANCED);
    EXPECT_EQ(typographyStyleCopy.wordBreakType, WordBreakType::BREAK_HYPHEN);
    EXPECT_NEAR(typographyStyleCopy.textSplitRatio, FLOAT_TEST_SIZE_NOT_DEFAULT, FLOAT_DATA_EPSILON);
    EXPECT_TRUE(typographyStyleCopy.enableAutoSpace);
    EXPECT_TRUE(typographyStyleCopy.isTrailingSpaceOptimized);
    EXPECT_EQ(typographyStyleCopy.tab.alignment, TextAlign::CENTER);
}

/*
 * @tc.name: OH_Drawing_CopyParagraphstyleStyle002
 * @tc.desc: test copyParagraphstylestyle: paragraphstyleStyleCopy(paragraphstyleStyle)
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_StyleCopyTest, OH_Drawing_CopyParagraphstyleStyle002, TestSize.Level0)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    TextStyle myTextStyle;
    myTextStyle.fontSize = DOUBLE_TEST_SIZE_NOT_DEFAULT;
    TextShadow myShadow;
    myShadow.color = Drawing::Color::ColorQuadSetARGB(255, 0, 255, 255);
    myShadow.blurRadius = DOUBLE_TEST_SIZE_NOT_DEFAULT;
    Drawing::Point myPoint(FLOAT_TEST_SIZE_NOT_DEFAULT, FLOAT_TEST_SIZE_NOT_DEFAULT);
    myShadow.offset = myPoint;
    TextShadow myShadowNull;
    myTextStyle.shadows = { myShadow, myShadow, myShadowNull };
    typographyStyle.insideTextStyle = myTextStyle;
    typographyStyle.customTextStyle = true;

    OHOS::Rosen::TypographyStyle typographyStyleCopy(typographyStyle);
    EXPECT_NEAR(typographyStyleCopy.insideTextStyle.fontSize, DOUBLE_TEST_SIZE_NOT_DEFAULT, FLOAT_DATA_EPSILON);
    EXPECT_TRUE(typographyStyleCopy.customTextStyle);
    EXPECT_NEAR(
        typographyStyleCopy.insideTextStyle.shadows[0].offset.GetY(), FLOAT_TEST_SIZE_NOT_DEFAULT, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(
        typographyStyleCopy.insideTextStyle.shadows[0].blurRadius, DOUBLE_TEST_SIZE_NOT_DEFAULT, FLOAT_DATA_EPSILON);
    EXPECT_EQ(typographyStyleCopy.insideTextStyle.shadows.size(), myTextStyle.shadows.size());
}
} // namespace Rosen
} // namespace OHOS