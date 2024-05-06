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

#include "common/rs_rect.h"
#include "gtest/gtest.h"
#include "font_collection.h"
#include "text_style.h"
#include "typography_style.h"
#include "typography_types.h"

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
HWTEST_F(OHSkiaTxtTest, OHSkiaTxtTest001, TestSize.Level1)
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
HWTEST_F(OHSkiaTxtTest, OHSkiaTxtTest002, TestSize.Level1)
{
    TextStyle textStyle1;
    TextStyle textStyle2;
    textStyle2.isPlaceholder = true;
    EXPECT_EQ(textStyle1.EqualByFonts(textStyle2), false);
    EXPECT_EQ(textStyle1.MatchOneAttribute(ALL_ATTRIBUTES, textStyle2), true);

    FontFeatures fontFeatures;
    std::string str1 = fontFeatures.GetFeatureSettings();
    FontVariations fontVariations1;
    std::string str2 = "123";
    // 1 is used for initialization
    float value = 1;
    fontVariations1.SetAxisValue(str2, value);
    FontVariations fontVariations2;
    EXPECT_EQ(fontVariations1 == fontVariations2, false);

    auto fontCollection = FontCollection::From(nullptr);
    TypographyStyle typographyStyle1;
    TypographyStyle typographyStyle2;
    // 15 is used for initialization
    typographyStyle2.fontSize = 15;
    EXPECT_EQ(typographyStyle1 == typographyStyle2, false);
    EXPECT_EQ(typographyStyle1.IsUnlimitedLines(), true);
    EXPECT_NE(typographyStyle1.IsEllipsized(), typographyStyle1.ellipsis.empty());

    // 1 is used for initialization
    Boundary boundary1(1, 1);
    // 1 is used for initialization
    Boundary boundary2(1, 1);
    EXPECT_EQ(boundary1 == boundary2, true);
}
} // namespace Rosen
} // namespace OHOS
