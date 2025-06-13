/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "drawing_font_collection.h"
#include "drawing_text_typography.h"
#include "gtest/gtest.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace {
const double DEFAULT_FONT_SIZE = 50;
const char* DEFAULT_TEXT = "text";
} // namespace

class NdkTypographyAlignmentTest : public testing::Test {
public:
    void SetUp() override;
    void TearDown() override;
    void PrepareWorkForAlignmentTest();
protected:
    OH_Drawing_TypographyCreate* fHandler{nullptr};
    OH_Drawing_Typography* fTypography{nullptr};
    OH_Drawing_TypographyStyle* fTypoStyle{nullptr};
    OH_Drawing_TextStyle* fTxtStyle{nullptr};
    int fLayoutWidth{50};
};

void NdkTypographyAlignmentTest::SetUp()
{
    fTypoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(fTypoStyle, nullptr);
    OH_Drawing_SetTypographyTextFontSize(fTypoStyle, DEFAULT_FONT_SIZE);
}

void NdkTypographyAlignmentTest::TearDown()
{
    if (fHandler != nullptr) {
        OH_Drawing_DestroyTypographyHandler(fHandler);
        fHandler = nullptr;
    }
    if (fTypography != nullptr) {
        OH_Drawing_DestroyTypography(fTypography);
        fTypography = nullptr;
    }
    if (fTypoStyle != nullptr) {
        OH_Drawing_DestroyTypographyStyle(fTypoStyle);
        fTypoStyle = nullptr;
    }
    if (fTxtStyle != nullptr) {
        OH_Drawing_DestroyTextStyle(fTxtStyle);
        fTxtStyle = nullptr;
    }
}

void NdkTypographyAlignmentTest::PrepareWorkForAlignmentTest()
{
    fHandler = OH_Drawing_CreateTypographyHandler(fTypoStyle, OH_Drawing_CreateFontCollection());
    ASSERT_NE(fHandler, nullptr);
    fTxtStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(fTxtStyle, nullptr);
    OH_Drawing_SetTextStyleFontSize(fTxtStyle, DEFAULT_FONT_SIZE);
    OH_Drawing_TypographyHandlerPushTextStyle(fHandler, fTxtStyle);
    OH_Drawing_TypographyHandlerAddText(fHandler, DEFAULT_TEXT);
    fTypography = OH_Drawing_CreateTypography(fHandler);
    ASSERT_NE(fTypography, nullptr);
    OH_Drawing_TypographyLayout(fTypography, fLayoutWidth);
}

/*
 * @tc.name: TypographyAlignmentTest001
 * @tc.desc: test for gets the typoStyle alignment mode and whether to enable text prompts
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyAlignmentTest, TypographyAlignmentTest001, TestSize.Level0)
{
    OH_Drawing_SetTypographyTextAlign(fTypoStyle, TEXT_ALIGN_START);
    OH_Drawing_SetTypographyTextDirection(fTypoStyle, TEXT_DIRECTION_LTR);
    EXPECT_EQ(OH_Drawing_TypographyStyleGetEffectiveAlignment(fTypoStyle), TEXT_ALIGN_LEFT);
    OH_Drawing_SetTypographyTextDirection(fTypoStyle, TEXT_DIRECTION_RTL);
    EXPECT_EQ(OH_Drawing_TypographyStyleGetEffectiveAlignment(fTypoStyle), TEXT_ALIGN_RIGHT);
    PrepareWorkForAlignmentTest();
    OH_Drawing_TextBox* boxes =
        OH_Drawing_TypographyGetRectsForRange(fTypography, 1, 2, RECT_HEIGHT_STYLE_TIGHT, RECT_WIDTH_STYLE_TIGHT);
    EXPECT_EQ(::round(OH_Drawing_GetLeftFromTextBox(boxes, 0)), 23);
}

/*
 * @tc.name: TypographyAlignmentTest002
 * @tc.desc: test for gets the typoStyle alignment mode and whether to enable text prompts
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyAlignmentTest, TypographyAlignmentTest002, TestSize.Level0)
{
    OH_Drawing_SetTypographyTextAlign(fTypoStyle, TEXT_ALIGN_END);
    OH_Drawing_SetTypographyTextDirection(fTypoStyle, TEXT_DIRECTION_LTR);
    EXPECT_EQ(OH_Drawing_TypographyStyleGetEffectiveAlignment(fTypoStyle), TEXT_ALIGN_RIGHT);
    OH_Drawing_SetTypographyTextDirection(fTypoStyle, TEXT_DIRECTION_RTL);
    EXPECT_EQ(OH_Drawing_TypographyStyleGetEffectiveAlignment(fTypoStyle), TEXT_ALIGN_LEFT);
    PrepareWorkForAlignmentTest();
    OH_Drawing_TextBox* boxes =
    OH_Drawing_TypographyGetRectsForRange(fTypography, 1, 2, RECT_HEIGHT_STYLE_TIGHT, RECT_WIDTH_STYLE_TIGHT);
    EXPECT_EQ(::round(OH_Drawing_GetLeftFromTextBox(boxes, 0)), 18);
}

/*
 * @tc.name: TypographyAlignmentTest003
 * @tc.desc: test for gets the typoStyle alignment mode and whether to enable text prompts
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyAlignmentTest, TypographyAlignmentTest003, TestSize.Level0)
{
    OH_Drawing_SetTypographyTextAlign(fTypoStyle, TEXT_ALIGN_CENTER);
    EXPECT_EQ(OH_Drawing_TypographyStyleGetEffectiveAlignment(fTypoStyle), TEXT_ALIGN_CENTER);
    PrepareWorkForAlignmentTest();
    OH_Drawing_TextBox* boxes =
    OH_Drawing_TypographyGetRectsForRange(fTypography, 1, 2, RECT_HEIGHT_STYLE_TIGHT, RECT_WIDTH_STYLE_TIGHT);
    EXPECT_EQ(::round(OH_Drawing_GetLeftFromTextBox(boxes, 0)), 20);
}

/*
 * @tc.name: TypographyAlignmentTest004
 * @tc.desc: test for gets the typoStyle alignment mode and whether to enable text prompts
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyAlignmentTest, TypographyAlignmentTest004, TestSize.Level0)
{
    OH_Drawing_SetTypographyTextAlign(fTypoStyle, TEXT_ALIGN_JUSTIFY);
    EXPECT_EQ(OH_Drawing_TypographyStyleGetEffectiveAlignment(fTypoStyle), TEXT_ALIGN_JUSTIFY);
    PrepareWorkForAlignmentTest();
    OH_Drawing_TextBox* boxes =
    OH_Drawing_TypographyGetRectsForRange(fTypography, 1, 2, RECT_HEIGHT_STYLE_TIGHT, RECT_WIDTH_STYLE_TIGHT);
    EXPECT_EQ(::round(OH_Drawing_GetLeftFromTextBox(boxes, 0)), 18);
}

/*
 * @tc.name: TypographyAlignmentTest005
 * @tc.desc: test for gets the typoStyle alignment mode and whether to enable text prompts
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyAlignmentTest, TypographyAlignmentTest005, TestSize.Level0)
{
    OH_Drawing_SetTypographyTextAlign(fTypoStyle, TEXT_ALIGN_LEFT);
    EXPECT_EQ(OH_Drawing_TypographyStyleGetEffectiveAlignment(fTypoStyle), TEXT_ALIGN_LEFT);
    PrepareWorkForAlignmentTest();
    OH_Drawing_TextBox* boxes =
    OH_Drawing_TypographyGetRectsForRange(fTypography, 1, 2, RECT_HEIGHT_STYLE_TIGHT, RECT_WIDTH_STYLE_TIGHT);
    EXPECT_EQ(::round(OH_Drawing_GetLeftFromTextBox(boxes, 0)), 18);
}

/*
 * @tc.name: TypographyAlignmentTest006
 * @tc.desc: test for gets the typoStyle alignment mode and whether to enable text prompts
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyAlignmentTest, TypographyAlignmentTest006, TestSize.Level0)
{
    OH_Drawing_SetTypographyTextAlign(fTypoStyle, TEXT_ALIGN_RIGHT);
    EXPECT_EQ(OH_Drawing_TypographyStyleGetEffectiveAlignment(fTypoStyle), TEXT_ALIGN_RIGHT);
    PrepareWorkForAlignmentTest();
    OH_Drawing_TextBox* boxes =
    OH_Drawing_TypographyGetRectsForRange(fTypography, 1, 2, RECT_HEIGHT_STYLE_TIGHT, RECT_WIDTH_STYLE_TIGHT);
    EXPECT_EQ(::round(OH_Drawing_GetLeftFromTextBox(boxes, 0)), 23);
}

/*
 * @tc.name: TypographyTrailingSpaceOptimizedTest001
 * @tc.desc: test for setting the different value of trailingSpaceOptimized and improve code coverage
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyAlignmentTest, TypographyTrailingSpaceOptimizedTest001, TestSize.Level0)
{
    OH_Drawing_SetTypographyTextAlign(fTypoStyle, TEXT_ALIGN_RIGHT);
    OH_Drawing_SetTypographyTextTrailingSpaceOptimized(nullptr, true);
    OH_Drawing_SetTypographyTextTrailingSpaceOptimized(nullptr, false);
    OH_Drawing_SetTypographyTextTrailingSpaceOptimized(fTypoStyle, true);
    OH_Drawing_SetTypographyTextTrailingSpaceOptimized(fTypoStyle, false);
    PrepareWorkForAlignmentTest();
}
} // namespace OHOS