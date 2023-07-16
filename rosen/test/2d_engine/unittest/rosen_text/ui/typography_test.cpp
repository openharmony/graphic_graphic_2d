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

using namespace rosen;
using namespace testing;
using namespace testing::ext;
using RectHeightStyle = rosen::TypographyProperties::RectHeightStyle;
using RectWidthStyle = rosen::TypographyProperties::RectWidthStyle;

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
    std::unique_ptr<TypographyCreate> builder = TypographyCreate::CreateRosenBuilder(
        typoStype, FontCollection::GetInstance());
    std::unique_ptr<rosen::Typography> typography = builder->Build();
    auto result = typography->GetRectsForRange(0, 0, RectHeightStyle::TIGHT, RectWidthStyle::TIGHT);
    EXPECT_EQ(result.empty(), true);
    result = typography->GetRectsForRange(0, 0, RectHeightStyle::MAX, RectWidthStyle::MAX);
    EXPECT_EQ(result.empty(), true);
    result = typography->GetRectsForRange(0, 0, RectHeightStyle::INCLUDELINESPACEMIDDLE,
        RectWidthStyle::TIGHT);
    EXPECT_EQ(result.empty(), true);
    result = typography->GetRectsForRange(0, 0, RectHeightStyle::INCLUDELINESPACETOP,
        RectWidthStyle::TIGHT);
    EXPECT_EQ(result.empty(), true);
    result = typography->GetRectsForRange(0, 0, RectHeightStyle::INCLUDELINESPACEBOTTOM,
        RectWidthStyle::MAX);
    EXPECT_EQ(result.empty(), true);
    result = typography->GetRectsForRange(0, 0, RectHeightStyle::STRUCT,
        RectWidthStyle::MAX);
    EXPECT_EQ(result.empty(), true);
    result = typography->GetRectsForPlaceholders();
    EXPECT_EQ(result.empty(), true);
}
}