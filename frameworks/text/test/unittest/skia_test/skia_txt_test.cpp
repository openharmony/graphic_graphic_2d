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

#include <vector>

#include "font_collection.h"
#include "gtest/gtest.h"
#include "text_style.h"
#include "typography_style.h"
#include "typography_types.h"

#include "common/rs_rect.h"
#include "text/font_types.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class OHSkiaTxtTest : public testing::Test {};

/*
 * @tc.name: OHSkiaTxtTest001
 * @tc.desc: test for text
 * @tc.type: FUNC
 */
HWTEST_F(OHSkiaTxtTest, OHSkiaTxtTest001, TestSize.Level0)
{
    RectT<int> rectTest;
    // 1 is used for initialization
    Vector4<int> vecTest(1);
    rectTest = rectTest.MakeOutset(vecTest);
    EXPECT_EQ(rectTest.left_, -1);
    EXPECT_EQ(rectTest.top_, -1);
    EXPECT_EQ(rectTest.width_, 2);
    EXPECT_EQ(rectTest.height_, 2);

    // 0 is used for initialization
    RectStyle rectStyle1 = {0, 0, 0, 0, 0};
    // 1 is used for initialization
    RectStyle rectStyle2 = {1, 1, 1, 1, 1};
    EXPECT_EQ(rectStyle1 != rectStyle2, true);

    Drawing::Color color = Drawing::Color::COLOR_WHITE;
    // 0 is used for initialization
    Drawing::Point offset{0, 0};
    // 0 is used for initialization
    TextShadow TextShadow1(color, offset, 0);
    // 1 is used for initialization
    TextShadow TextShadow2(color, offset, 1);
    EXPECT_EQ(TextShadow1 == TextShadow2, false);
    EXPECT_EQ(TextShadow1 != TextShadow2, true);
    EXPECT_EQ(TextShadow1.HasShadow(), false);
}

/*
 * @tc.name: OHSkiaTxtTest002
 * @tc.desc: test for text
 * @tc.type: FUNC
 */
HWTEST_F(OHSkiaTxtTest, OHSkiaTxtTest002, TestSize.Level0)
{
    TextStyle textStyle1;
    TextStyle textStyle2;
    textStyle2.isPlaceholder = true;
    EXPECT_EQ(textStyle1.EqualByFonts(textStyle2), false);
    EXPECT_EQ(textStyle1.MatchOneAttribute(ALL_ATTRIBUTES, textStyle2), true);
    bool res1;
    res1 = textStyle1.MatchOneAttribute(FOREGROUND, textStyle2);
    res1 = textStyle1.MatchOneAttribute(BACKGROUND, textStyle2);
    res1 = textStyle1.MatchOneAttribute(SHADOW, textStyle2);
    res1 = textStyle1.MatchOneAttribute(DECORATIONS, textStyle2);
    res1 = textStyle1.MatchOneAttribute(LETTER_SPACING, textStyle2);
    res1 = textStyle1.MatchOneAttribute(WORD_SPACING, textStyle2);
    res1 = textStyle1.MatchOneAttribute(FONT, textStyle2);
    bool res2 = textStyle2.MatchOneAttribute(FONT, textStyle1);
    EXPECT_EQ(res1, res2);

    FontFeatures fontFeatures;
    std::string str1 = fontFeatures.GetFeatureSettings();
    // 1 is used for test
    fontFeatures.SetFeature("123", 1);
    str1 = fontFeatures.GetFeatureSettings();
    FontVariations fontVariations1;
    std::string str2 = "123";
    // 1 is used for initialization
    float value = 1;
    fontVariations1.SetAxisValue(str2, value);
    FontVariations fontVariations2;
    EXPECT_EQ(fontVariations1 == fontVariations2, false);
    fontVariations1.Clear();

    auto fontCollection = FontCollection::From(nullptr);
    TypographyStyle typographyStyle1;
    TypographyStyle typographyStyle2;
    // 15 is used for initialization
    typographyStyle2.fontSize = 15;
    EXPECT_EQ(typographyStyle1 == typographyStyle2, false);
    EXPECT_EQ(typographyStyle1.IsUnlimitedLines(), true);
    EXPECT_NE(typographyStyle1.IsEllipsized(), typographyStyle1.ellipsis.empty());
    typographyStyle2.textAlign = TextAlign::END;
    EXPECT_EQ(typographyStyle2.GetEffectiveAlign(), TextAlign::RIGHT);

    // 1 is used for initialization
    Boundary boundary1(1, 1);
    // 1 is used for initialization
    Boundary boundary2(1, 1);
    EXPECT_EQ(boundary1 == boundary2, true);
}

/*
 * @tc.name: OHSkiaTxtTest003
 * @tc.desc: test for fontEdging in EqualByFonts and MatchOneAttribute
 * @tc.type: FUNC
 */
HWTEST_F(OHSkiaTxtTest, OHSkiaTxtTest003, TestSize.Level0)
{
    TextStyle textStyle1;
    TextStyle textStyle2;

    // Test default fontEdging (ANTI_ALIAS) - should be equal
    EXPECT_EQ(textStyle1.fontEdging, Drawing::FontEdging::ANTI_ALIAS);
    EXPECT_EQ(textStyle2.fontEdging, Drawing::FontEdging::ANTI_ALIAS);
    EXPECT_EQ(textStyle1.EqualByFonts(textStyle2), true);
    EXPECT_EQ(textStyle1.MatchOneAttribute(FONT, textStyle2), true);

    // Test different fontEdging values - EqualByFonts should return false
    textStyle1.fontEdging = Drawing::FontEdging::ALIAS;
    textStyle2.fontEdging = Drawing::FontEdging::SUBPIXEL_ANTI_ALIAS;
    EXPECT_EQ(textStyle1.EqualByFonts(textStyle2), false);
    EXPECT_EQ(textStyle1.MatchOneAttribute(FONT, textStyle2), false);

    // Test same fontEdging values - EqualByFonts should return true
    textStyle2.fontEdging = Drawing::FontEdging::ALIAS;
    EXPECT_EQ(textStyle1.EqualByFonts(textStyle2), true);
    EXPECT_EQ(textStyle1.MatchOneAttribute(FONT, textStyle2), true);

    // Test with placeholder - EqualByFonts should return false regardless of fontEdging
    textStyle1.isPlaceholder = true;
    EXPECT_EQ(textStyle1.EqualByFonts(textStyle2), false);

    // Test all fontEdging enum values
    textStyle1.isPlaceholder = false;
    std::vector<Drawing::FontEdging> edgingValues = {
        Drawing::FontEdging::ALIAS,
        Drawing::FontEdging::ANTI_ALIAS,
        Drawing::FontEdging::SUBPIXEL_ANTI_ALIAS
    };

    for (size_t i = 0; i < edgingValues.size(); ++i) {
        textStyle1.fontEdging = edgingValues[i];
        for (size_t j = 0; j < edgingValues.size(); ++j) {
            textStyle2.fontEdging = edgingValues[j];
            bool expectedEqual = (i == j);
            EXPECT_EQ(textStyle1.EqualByFonts(textStyle2), expectedEqual);
            EXPECT_EQ(textStyle1.MatchOneAttribute(FONT, textStyle2), expectedEqual);
        }
    }
}
} // namespace Rosen
} // namespace OHOS
