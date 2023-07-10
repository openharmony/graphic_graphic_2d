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

#include "gtest/gtest.h"
#include "font_collection.h"
#include "typography.h"
#include "typography_create.h"

using namespace OHOS::Rosen;
using namespace testing;
using namespace testing::ext;
using RectHeightStyle = OHOS::Rosen::TextRectHeightStyle;
using RectWidthStyle = OHOS::Rosen::TextRectWidthStyle;

namespace OHOS {
class OH_Drawing_UI_TypographyTest : public testing::Test {
};

/*
 * @tc.name: OH_Drawing_UI_TypographyTest
 * @tc.desc: test for typography metrics
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_UI_TypographyTest, OH_Drawing_UI_TypographyTest001, TestSize.Level1)
{
    TypographyStyle typoStype;
    std::unique_ptr<TypographyCreate> builder = TypographyCreate::Create(
        typoStype, FontCollection::Create());
    std::unique_ptr<Rosen::Typography> typography = builder->CreateTypography();
    auto result = typography->GetTextRectsByBoundary(0, 0, RectHeightStyle::TIGHT, RectWidthStyle::TIGHT);
    EXPECT_EQ(result.empty(), true);
    result = typography->GetTextRectsByBoundary(0, 0, RectHeightStyle::COVER_TOP_AND_BOTTOM, RectWidthStyle::MAX);
    EXPECT_EQ(result.empty(), true);
    result = typography->GetTextRectsByBoundary(0, 0, RectHeightStyle::COVER_HALF_TOP_AND_BOTTOM,
        RectWidthStyle::TIGHT);
    EXPECT_EQ(result.empty(), true);
    result = typography->GetTextRectsByBoundary(0, 0, RectHeightStyle::COVER_TOP,
        RectWidthStyle::TIGHT);
    EXPECT_EQ(result.empty(), true);
    result = typography->GetTextRectsByBoundary(0, 0, RectHeightStyle::COVER_BOTTOM,
        RectWidthStyle::MAX);
    EXPECT_EQ(result.empty(), true);
    result = typography->GetTextRectsByBoundary(0, 0, RectHeightStyle::FOLLOW_BY_STRUT,
        RectWidthStyle::MAX);
    EXPECT_EQ(result.empty(), true);
    result = typography->GetTextRectsOfPlaceholders();
    EXPECT_EQ(result.empty(), true);
}
}