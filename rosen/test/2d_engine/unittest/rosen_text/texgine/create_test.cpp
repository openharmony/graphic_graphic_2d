/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "rosen_text/font_collection.h"
#include "rosen_text/text_style.h"
#include "rosen_text/typography.h"
#include "rosen_text/typography_create.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class OHTexgineRosenTypographyTest : public testing::Test {};
namespace {
constexpr int32_t ZERO_VALUE = 0;
}

/*
 * @tc.name: OHTexgineRosenTypographyTest001
 * @tc.desc: test for texgine typography
 * @tc.type: FUNC
 */
HWTEST_F(OHTexgineRosenTypographyTest, OHTexgineRosenTypographyTest001, TestSize.Level1)
{
    TypographyStyle typoStype;
    std::unique_ptr<TypographyCreate> builder = TypographyCreate::Create(typoStype, FontCollection::Create());
    TextStyle style;
    builder->PushStyle(style);
    builder->PopStyle();
    std::u16string text = u"test123";
    builder->AppendText(text);
    std::unique_ptr<Rosen::Typography> typography = builder->CreateTypography();
    auto result = typography->GetTextRectsByBoundary(
        ZERO_VALUE, ZERO_VALUE, TextRectHeightStyle::TIGHT, TextRectWidthStyle::TIGHT);
    EXPECT_EQ(result.empty(), true);
    result = typography->GetTextRectsByBoundary(
        ZERO_VALUE, ZERO_VALUE, TextRectHeightStyle::COVER_TOP_AND_BOTTOM, TextRectWidthStyle::MAX);
    EXPECT_EQ(result.empty(), true);
    result = typography->GetTextRectsByBoundary(
        ZERO_VALUE, ZERO_VALUE, TextRectHeightStyle::COVER_HALF_TOP_AND_BOTTOM, TextRectWidthStyle::TIGHT);
    EXPECT_EQ(result.empty(), true);
    result = typography->GetTextRectsByBoundary(
        ZERO_VALUE, ZERO_VALUE, TextRectHeightStyle::COVER_TOP, TextRectWidthStyle::TIGHT);
    EXPECT_EQ(result.empty(), true);
    result = typography->GetTextRectsByBoundary(
        ZERO_VALUE, ZERO_VALUE, TextRectHeightStyle::COVER_BOTTOM, TextRectWidthStyle::MAX);
    EXPECT_EQ(result.empty(), true);
    result = typography->GetTextRectsByBoundary(
        ZERO_VALUE, ZERO_VALUE, TextRectHeightStyle::FOLLOW_BY_STRUT, TextRectWidthStyle::MAX);
    EXPECT_EQ(result.empty(), true);
    result = typography->GetTextRectsOfPlaceholders();
    EXPECT_EQ(result.empty(), true);
}
} // namespace Rosen
} // namespace OHOS
