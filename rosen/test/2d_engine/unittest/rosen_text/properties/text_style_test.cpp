/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "text_style.h"
#include "gtest/gtest.h"


using namespace rosen;
using namespace testing;
using namespace testing::ext;

namespace OHOS {
class OH_Drawing_TextShadowTest : public testing::Test {
};

/*
 * @tc.name: OH_Drawing_TextShadowTest002
 * @tc.desc: test for TextShadow
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TextShadowTest, OH_Drawing_TextShadowTest001, TestSize.Level1)
{
    OHOS::Rosen::Drawing::Point offset;
    offset.SetX(2.0f);
    TextShadow textShadow(Rosen::Drawing::Color::COLOR_BLACK, offset, 0.0);
    TextShadow textShadow2(Rosen::Drawing::Color::COLOR_BLACK, offset, 0.0);
    EXPECT_EQ(textShadow == textShadow2, true);
    textShadow2.blurRadius_ = 1.0;
    EXPECT_EQ(textShadow == textShadow2, false);
    EXPECT_EQ(textShadow != textShadow2, true);
    textShadow2.offset_.SetX(3.0f);
    EXPECT_EQ(textShadow == textShadow2, false);
    textShadow2.color_ = Rosen::Drawing::Color::COLOR_RED;
    EXPECT_EQ(textShadow == textShadow2, false);
}

HWTEST_F(OH_Drawing_TextShadowTest, OH_Drawing_TextShadowTest002, TestSize.Level1)
{
    OHOS::Rosen::Drawing::Point offset;
    TextShadow textShadow(Rosen::Drawing::Color::COLOR_BLACK, offset, 0.0);
    EXPECT_EQ(textShadow.hasShadow(), false);
    textShadow.blurRadius_ = 1.0f;
    EXPECT_EQ(textShadow.hasShadow(), true);
    textShadow.offset_.SetY(1.0f);
    EXPECT_EQ(textShadow.hasShadow(), true);
    textShadow.offset_.SetX(1.0f);
    EXPECT_EQ(textShadow.hasShadow(), true);
}


class OH_Drawing_TextStyleTest : public testing::Test {
};

/*
 * @tc.name: OH_Drawing_TextStyleTest001
 * @tc.desc: test for TextStyle comparison
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TextStyleTest, OH_Drawing_TextStyleTest001, TestSize.Level1)
{
    TextStyle textStyle;
    TextStyle textStyle2;
    EXPECT_EQ(textStyle.equals(textStyle2), true);
}

/*
 * @tc.name: OH_Drawing_TextStyleTest002
 * @tc.desc: test for TextStyle font features
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TextStyleTest, OH_Drawing_TextStyleTest002, TestSize.Level1)
{
    TextStyle textStyle;
    std::string features = textStyle.fontFeatures_.GetFeatureSettings();
    EXPECT_EQ(features.empty(), true);
}
}
