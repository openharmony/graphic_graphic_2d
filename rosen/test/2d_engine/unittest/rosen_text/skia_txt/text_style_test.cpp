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
    static void SetUpTestCase();
    static void TearDownTestCase();
    static inline std::shared_ptr<TextStyle> textStyle_ = nullptr;
};

void TextStyleTest::SetUpTestCase()
{
    textStyle_ = std::make_shared<TextStyle>();
    if (!textStyle_) {
        std::cout << "TextStyleTest::SetUpTestCase error textStyle_ is nullptr" << std::endl;
    }
}

void TextStyleTest::TearDownTestCase()
{
}

/*
 * @tc.name: TextStyleTest001
 * @tc.desc: test for Equal
 * @tc.type: FUNC
 */
HWTEST_F(TextStyleTest, TextStyleTest001, TestSize.Level1)
{
    EXPECT_EQ(textStyle_ != nullptr, true);
    TextStyle textStyle;
    EXPECT_EQ((*textStyle_) == textStyle, true);
}

/*
 * @tc.name: TextStyleTest002
 * @tc.desc: test for change fontFeatures
 * @tc.type: FUNC
 */
HWTEST_F(TextStyleTest, TextStyleTest002, TestSize.Level1)
{
    EXPECT_EQ(textStyle_ != nullptr, true);
    FontFeatures fontFeatures;
    fontFeatures.SetFeature("tag", 0);
    textStyle_->fontFeatures = fontFeatures;
    EXPECT_EQ(textStyle_->fontFeatures.GetFeatureSettings().empty(), false);
    EXPECT_EQ(textStyle_->fontFeatures.GetFontFeatures().size() > 0, true);
}

/*
 * @tc.name: TextStyleTest003
 * @tc.desc: test for change fontVariations
 * @tc.type: FUNC
 */
HWTEST_F(TextStyleTest, TextStyleTest003, TestSize.Level1)
{
    EXPECT_EQ(textStyle_ != nullptr, true);
    FontVariations fontVariations;
    fontVariations.SetAxisValue("tag", 0.0);
    textStyle_->fontVariations = fontVariations;
    EXPECT_EQ(textStyle_->fontVariations.GetAxisValues().size() > 0, true);
}

/*
 * @tc.name: TextStyleTest004
 * @tc.desc: test for change textShadows
 * @tc.type: FUNC
 */
HWTEST_F(TextStyleTest, TextStyleTest004, TestSize.Level1)
{
    EXPECT_EQ(textStyle_ != nullptr, true);
    TextShadow textShadowDefault;
    SkColor color = 255; // 255 just fot test
    SkPoint offset{0, 0};
    TextShadow textShadow(color, offset, 0.0);
    EXPECT_EQ(textShadowDefault == textShadow, false);
    EXPECT_EQ(textShadowDefault != textShadow, true);
    EXPECT_EQ(textShadow.HasShadow(), false);

    std::vector<TextShadow> textShadows;
    textShadows.emplace_back(textShadowDefault);
    textShadows.emplace_back(textShadow);
    textStyle_->textShadows = textShadows;
    EXPECT_EQ(textStyle_->textShadows.size() > 0, true);
}

/*
 * @tc.name: TextStyleTest005
 * @tc.desc: test for change backgroundRect
 * @tc.type: FUNC
 */
HWTEST_F(TextStyleTest, TextStyleTest005, TestSize.Level1)
{
    EXPECT_EQ(textStyle_ != nullptr, true);
    RectStyle backgroundRect = {0, 1.0f, 0.0f, 0.0f, 0.0f};
    EXPECT_EQ(textStyle_->backgroundRect != backgroundRect, true);
    textStyle_->backgroundRect = backgroundRect;
    EXPECT_EQ(textStyle_->backgroundRect == backgroundRect, true);
}
} // namespace txt