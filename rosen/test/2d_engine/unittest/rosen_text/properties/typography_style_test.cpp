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
#include "typography_style.h"

#include "gtest/gtest.h"
#include <climits>
#include <vector>
#include <string>

using namespace rosen;
using namespace testing;
using namespace testing::ext;

namespace OHOS {
class OH_Drawing_TypographyStyleTest : public testing::Test {
};

/*
 * @tc.name: OH_Drawing_TypographyStyleTest001
 * @tc.desc: test for creating TypographyStyle
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyStyleTest, OH_Drawing_TypographyStyleTest001, TestSize.Level1)
{
    TypographyStyle typoStyle;
    TextStyle textstyle = typoStyle.GetTextStyle();
    EXPECT_EQ(typoStyle.fontWeight_, textstyle.fontWeight_);
    EXPECT_EQ(typoStyle.fontStyle_, textstyle.fontStyle_);
    EXPECT_EQ(typoStyle.fontSize_, textstyle.fontSize_);
    EXPECT_EQ(typoStyle.locale_, textstyle.locale_);
    EXPECT_EQ(typoStyle.height_, textstyle.height_);
    EXPECT_EQ(typoStyle.hasHeightOverride_, textstyle.hasHeightOverride_);
}

/*
 * @tc.name: OH_Drawing_TypographyStyleTest002
 * @tc.desc: test for TypographyStyle EffectiveAlign
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyStyleTest, OH_Drawing_TypographyStyleTest002, TestSize.Level1)
{
    TypographyStyle typoStyle;
    typoStyle.textAlign_ = TextAlign::START;
    TextAlign textAlign = typoStyle.EffectiveAlign();
    EXPECT_EQ(textAlign, TextAlign::LEFT);

    typoStyle.textAlign_ = TextAlign::END;
    textAlign = typoStyle.EffectiveAlign();
    EXPECT_EQ(textAlign, TextAlign::RIGHT);

    typoStyle.textAlign_ = TextAlign::CENTER;
    textAlign = typoStyle.EffectiveAlign();
    EXPECT_EQ(textAlign, TextAlign::CENTER);
}
}