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
#include "text_style.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::Rosen::SPText;

namespace txt {
class TextStyleTest : public testing::Test {
public:
    void SetUp() override;
    void TearDown() override;

private:
    std::shared_ptr<TextStyle> textStyle_;
};

void TextStyleTest::SetUp()
{
    textStyle_ = std::make_shared<TextStyle>();
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
    TextStyle textStyleCompare;
    EXPECT_EQ((*textStyle_), textStyleCompare);
}

/*
 * @tc.name: TextStyleTest002
 * @tc.desc: test for change fontFeatures
 * @tc.type: FUNC
 */
HWTEST_F(TextStyleTest, TextStyleTest002, TestSize.Level0)
{
    FontFeatures fontFeatures;
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
    FontVariations fontVariations;
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
    TextShadow textShadowDefault;
    SkColor color = 255; // 255 just fot test
    SkPoint offset { 0, 0 };
    TextShadow textShadow(color, offset, 0.0);
    EXPECT_NE(textShadowDefault, textShadow);
    EXPECT_FALSE(textShadow.HasShadow());

    std::vector<TextShadow> textShadows;
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
    RectStyle backgroundRect = { 0, 1.0f, 0.0f, 0.0f, 0.0f };
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
    FontFeatures fontFeatures;
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
    TextShadow shadowA;
    TextShadow shadowB;
    EXPECT_EQ(shadowA, shadowB);
    // 0.2 is just for test
    shadowB.blurSigma = 0.2;
    EXPECT_NE(shadowA, shadowB);
}
} // namespace txt