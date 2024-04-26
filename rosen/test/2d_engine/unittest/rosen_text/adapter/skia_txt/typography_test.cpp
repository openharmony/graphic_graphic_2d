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

#include "typography.h"
#include "typography_create.h"
#include "font_collection.h"


using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class OH_Drawing_TypographyTest : public testing::Test {
};

/*
 * @tc.name: OH_Drawing_TypographyTest001
 * @tc.desc: test for get max width for Typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest001, TestSize.Level1)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection = OHOS::Rosen::FontCollection::Create();
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate = OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    EXPECT_EQ(typography->GetMaxWidth() == 0, true);
}

/*
 * @tc.name: OH_Drawing_TypographyTest002
 * @tc.desc: test for get height for Typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest002, TestSize.Level1)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection = OHOS::Rosen::FontCollection::Create();
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate = OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    EXPECT_EQ(typography->GetHeight() == 0, true);
    EXPECT_EQ(typography->GetActualWidth() == 0, true);
}

/*
 * @tc.name: OH_Drawing_TypographyTest003
 * @tc.desc: test for get actual width for Typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest003, TestSize.Level1)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection = OHOS::Rosen::FontCollection::Create();
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate = OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    EXPECT_EQ(typography->GetActualWidth() == 0, true);
    EXPECT_EQ(typography->GetMaxIntrinsicWidth() == 0, true);
}

/*
 * @tc.name: OH_Drawing_TypographyTest004
 * @tc.desc: test for get min intrinsic width for typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest004, TestSize.Level1)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection = OHOS::Rosen::FontCollection::Create();
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate = OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    EXPECT_EQ(typography->GetMinIntrinsicWidth() == 0, true);
}

/*
 * @tc.name: OH_Drawing_TypographyTest005
 * @tc.desc: test for get members for typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest005, TestSize.Level1)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection = OHOS::Rosen::FontCollection::Create();
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate = OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    EXPECT_EQ(typography->GetAlphabeticBaseline() == 0, true);    
    EXPECT_EQ(typography->GetIdeographicBaseline() == 0, true);
    EXPECT_EQ(typography->GetGlyphsBoundsTop() > 0, true);
    EXPECT_EQ(typography->GetGlyphsBoundsBottom() > 0, true);
    EXPECT_EQ(typography->GetGlyphsBoundsLeft() > 0, true);
    EXPECT_EQ(typography->GetGlyphsBoundsRight() > 0, true);
    EXPECT_EQ(typography->DidExceedMaxLines() == false, true);
}
}//Rosen
}//OHOS