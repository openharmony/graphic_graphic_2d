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
    typography->GetGlyphsBoundsTop();
    typography->GetGlyphsBoundsBottom();
    typography->GetGlyphsBoundsLeft();
    typography->GetGlyphsBoundsRight();
    EXPECT_EQ(typography->DidExceedMaxLines() == false, true);
    EXPECT_EQ(typography->GetLineCount() > 0, true);
    typography->MarkDirty();
    EXPECT_EQ(typography->GetUnresolvedGlyphsCount() < 0, true);
}

/*
 * @tc.name: OH_Drawing_TypographyTest006
 * @tc.desc: test for updatting font size for typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest006, TestSize.Level1)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection = OHOS::Rosen::FontCollection::Create();
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate = OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    typography->UpdateFontSize(1,10,20.0);
    std::vector<float> indents = {1.2, 3.4};
    typography->SetIndents(indents);
    EXPECT_EQ(typography->DetectIndents(3), indents[1]);//3 > indents.size(), return indents.back()
    typography->Layout(100);
    SkCanvas* canvas = nullptr;
    typography->Paint(canvas, 10.0, 10.0);
    OHOS::Rosen::Drawing::Canvas* drawingCanvas = nullptr;
    typography->Paint(drawingCanvas, 20.0, 20.0);
    TextRectHeightStyle textRectHeightStyle = TextRectHeightStyle::TIGHT;
    TextRectWidthStyle textRectWidthtStyle = TextRectWidthStyle::MAX;
    std::vector<TextRect> vectorTextRect = typography->GetTextRectsByBoundary(
        1, 2, textRectHeightStyle, textRectWidthtStyle);
    vectorTextRect = typography->GetTextRectsOfPlaceholders();
    typography->GetGlyphIndexByCoordinate(1.0, 2.0);
}

/*
 * @tc.name: OH_Drawing_TypographyTest007
 * @tc.desc: test for getting word boundary by index for typography
 * @tc.type: FUNC
 */
HWTEST_F(OH_Drawing_TypographyTest, OH_Drawing_TypographyTest007, TestSize.Level1)
{
    OHOS::Rosen::TypographyStyle typographyStyle;
    std::shared_ptr<OHOS::Rosen::FontCollection> fontCollection = OHOS::Rosen::FontCollection::Create();
    std::unique_ptr<OHOS::Rosen::TypographyCreate> typographyCreate = OHOS::Rosen::TypographyCreate::Create(typographyStyle, fontCollection);
    std::unique_ptr<OHOS::Rosen::Typography> typography = typographyCreate->CreateTypography();
    typography->GetWordBoundaryByIndex(0);
    typography->GetActualTextRange(0, false);
    EXPECT_EQ(typography->GetLineHeight(0) == 0.0, true);
    EXPECT_EQ(typography->GetLineWidth(0)  == 0.0, true);
    std::function<bool(const std::shared_ptr<OHOS::Rosen::TextEngine::SymbolAnimationConfig>&)> animationFunc;
    typography->SetAnimation(animationFunc);
    typography->SetParagraghId(0);
    typography->MeasureText();
    LineMetrics* lineMetrics = nullptr;
    EXPECT_EQ(typography->GetLineInfo(0, true, true, lineMetrics) == false, true);
    typography->GetLineMetrics();
    EXPECT_EQ(typography->GetLineMetricsAt(0, lineMetrics) == false, true);
    OHOS::Rosen::TextStyle txtStyle;
    typography->GetFontMetrics(txtStyle);
    size_t charNum = 0;
    std::vector<Drawing::FontMetrics> vectorFontMetrics;
    EXPECT_EQ(typography->GetLineFontMetrics(0, charNum, vectorFontMetrics) == false, true);
    typography->GetTextLines();
    typography->CloneSelf();
}
} // namespace Rosen
} // namespace OHOS