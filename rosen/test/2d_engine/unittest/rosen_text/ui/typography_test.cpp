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

#ifndef USE_GRAPHIC_TEXT_GINE
using namespace rosen;
#else
using namespace OHOS::Rosen;
#endif
using namespace testing;
using namespace testing::ext;
#ifndef USE_GRAPHIC_TEXT_GINE
using RectHeightStyle = rosen::TypographyProperties::RectHeightStyle;
using RectWidthStyle = rosen::TypographyProperties::RectWidthStyle;
#else
using RectHeightStyle = OHOS::Rosen::TextRectHeightStyle;
using RectWidthStyle = OHOS::Rosen::TextRectWidthStyle;
#endif

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
#ifndef USE_GRAPHIC_TEXT_GINE
    std::unique_ptr<TypographyCreate> builder = TypographyCreate::CreateRosenBuilder(
        typoStype, FontCollection::GetInstance());
    std::unique_ptr<rosen::Typography> typography = builder->Build();
    auto result = typography->GetRectsForRange(0, 0, RectHeightStyle::TIGHT, RectWidthStyle::TIGHT);
#else
    std::unique_ptr<TypographyCreate> builder = TypographyCreate::Create(
        typoStype, FontCollection::Create());
    std::unique_ptr<Rosen::Typography> typography = builder->CreateTypography();
    auto result = typography->GetTextRectsByBoundary(0, 0, RectHeightStyle::TIGHT, RectWidthStyle::TIGHT);
#endif
    EXPECT_EQ(result.empty(), true);
#ifndef USE_GRAPHIC_TEXT_GINE
    result = typography->GetRectsForRange(0, 0, RectHeightStyle::MAX, RectWidthStyle::MAX);
#else
    result = typography->GetTextRectsByBoundary(0, 0, RectHeightStyle::COVER_TOP_AND_BOTTOM, RectWidthStyle::MAX);
#endif
    EXPECT_EQ(result.empty(), true);
#ifndef USE_GRAPHIC_TEXT_GINE
    result = typography->GetRectsForRange(0, 0, RectHeightStyle::INCLUDELINESPACEMIDDLE,
#else
    result = typography->GetTextRectsByBoundary(0, 0, RectHeightStyle::COVER_HALF_TOP_AND_BOTTOM,
#endif
        RectWidthStyle::TIGHT);
    EXPECT_EQ(result.empty(), true);
#ifndef USE_GRAPHIC_TEXT_GINE
    result = typography->GetRectsForRange(0, 0, RectHeightStyle::INCLUDELINESPACETOP,
#else
    result = typography->GetTextRectsByBoundary(0, 0, RectHeightStyle::COVER_TOP,
#endif
        RectWidthStyle::TIGHT);
    EXPECT_EQ(result.empty(), true);
#ifndef USE_GRAPHIC_TEXT_GINE
    result = typography->GetRectsForRange(0, 0, RectHeightStyle::INCLUDELINESPACEBOTTOM,
#else
    result = typography->GetTextRectsByBoundary(0, 0, RectHeightStyle::COVER_BOTTOM,
#endif
        RectWidthStyle::MAX);
    EXPECT_EQ(result.empty(), true);
#ifndef USE_GRAPHIC_TEXT_GINE
    result = typography->GetRectsForRange(0, 0, RectHeightStyle::STRUCT,
#else
    result = typography->GetTextRectsByBoundary(0, 0, RectHeightStyle::FOLLOW_BY_STRUT,
#endif
        RectWidthStyle::MAX);
    EXPECT_EQ(result.empty(), true);
#ifndef USE_GRAPHIC_TEXT_GINE
    result = typography->GetRectsForPlaceholders();
#else
    result = typography->GetTextRectsOfPlaceholders();
#endif
    EXPECT_EQ(result.empty(), true);
}

/*
 * @tc.name: OH_Drawing_UI_TypographyTest001
 * @tc.desc: test for DidExceedMaxLines metrics
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_UI_TypographyTest, OH_Drawing_UI_DidExceedMaxLinesTest001, TestSize.Level1)
{
    TypographyStyle typoStype;
    #ifndef USE_GRAPHIC_TEXT_GINE
    result = typography->GetGlyphPositionAtCoordinate();
    #endif
}
}