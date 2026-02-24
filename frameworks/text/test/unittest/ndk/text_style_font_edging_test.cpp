/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <limits>
#include <string>

#include "drawing_color.h"
#include "drawing_error_code.h"
#include "drawing_font.h"
#include "drawing_font_collection.h"
#include "drawing_text_typography.h"
#include "gtest/gtest.h"
#include "text_style.h"
#include "typography_types.h"

#include "text/font_types.h"

using namespace OHOS::Rosen;
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace {
const double DEFAULT_FONT_SIZE = 50;
const char* DEFAULT_TEXT = "text";
} // namespace

class NdkTextStyleFontEdgingTest : public testing::Test {
public:
    void SetUp() override {}
    void TearDown() override {}

protected:
    int fLayoutWidth { 50 };
};

static TextStyle* ConvertToOriginalText(OH_Drawing_TextStyle* style)
{
    return reinterpret_cast<TextStyle*>(style);
}

/*
 * @tc.name: TextStyleAttributeFontEdgingTest001
 * @tc.desc: test invalid data for font edging.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTextStyleFontEdgingTest, TextStyleAttributeFontEdgingTest001, TestSize.Level0)
{
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(txtStyle, nullptr);
    // Test value less than FONT_EDGING_ALIAS (0)
    auto res = OH_Drawing_SetTextStyleAttributeInt(txtStyle, TEXT_STYLE_ATTR_I_FONT_EDGING, -1);
    EXPECT_EQ(res, OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE);
    // Test value greater than FONT_EDGING_SUBPIXEL_ANTI_ALIAS (2)
    res = OH_Drawing_SetTextStyleAttributeInt(txtStyle, TEXT_STYLE_ATTR_I_FONT_EDGING, 3);
    EXPECT_EQ(res, OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE);
    // Test extreme negative value
    res = OH_Drawing_SetTextStyleAttributeInt(txtStyle, TEXT_STYLE_ATTR_I_FONT_EDGING, INT_MIN);
    EXPECT_EQ(res, OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE);
    // Test extreme large positive value
    res = OH_Drawing_SetTextStyleAttributeInt(txtStyle, TEXT_STYLE_ATTR_I_FONT_EDGING, INT_MAX);
    EXPECT_EQ(res, OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE);
    // Verify default value is unchanged after failed sets
    int outResult = 0;
    res = OH_Drawing_GetTextStyleAttributeInt(txtStyle, TEXT_STYLE_ATTR_I_FONT_EDGING, &outResult);
    EXPECT_EQ(res, OH_DRAWING_SUCCESS);
    EXPECT_EQ(outResult, static_cast<int>(FONT_EDGING_ANTI_ALIAS));
    OH_Drawing_DestroyTextStyle(txtStyle);
}

/*
 * @tc.name: TextStyleAttributeFontEdgingTest002
 * @tc.desc: test valid data for font edging.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTextStyleFontEdgingTest, TextStyleAttributeFontEdgingTest002, TestSize.Level0)
{
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(txtStyle, nullptr);

    // Test FONT_EDGING_ALIAS (0)
    auto res = OH_Drawing_SetTextStyleAttributeInt(
        txtStyle, TEXT_STYLE_ATTR_I_FONT_EDGING, static_cast<int>(FONT_EDGING_ALIAS));
    EXPECT_EQ(res, OH_DRAWING_SUCCESS);
    int outResult = 0;
    res = OH_Drawing_GetTextStyleAttributeInt(txtStyle, TEXT_STYLE_ATTR_I_FONT_EDGING, &outResult);
    EXPECT_EQ(res, OH_DRAWING_SUCCESS);
    EXPECT_EQ(outResult, static_cast<int>(FONT_EDGING_ALIAS));

    // Test FONT_EDGING_ANTI_ALIAS (1)
    res = OH_Drawing_SetTextStyleAttributeInt(
        txtStyle, TEXT_STYLE_ATTR_I_FONT_EDGING, static_cast<int>(FONT_EDGING_ANTI_ALIAS));
    EXPECT_EQ(res, OH_DRAWING_SUCCESS);
    res = OH_Drawing_GetTextStyleAttributeInt(txtStyle, TEXT_STYLE_ATTR_I_FONT_EDGING, &outResult);
    EXPECT_EQ(res, OH_DRAWING_SUCCESS);
    EXPECT_EQ(outResult, static_cast<int>(FONT_EDGING_ANTI_ALIAS));

    // Test FONT_EDGING_SUBPIXEL_ANTI_ALIAS (2)
    res = OH_Drawing_SetTextStyleAttributeInt(
        txtStyle, TEXT_STYLE_ATTR_I_FONT_EDGING, static_cast<int>(FONT_EDGING_SUBPIXEL_ANTI_ALIAS));
    EXPECT_EQ(res, OH_DRAWING_SUCCESS);
    res = OH_Drawing_GetTextStyleAttributeInt(txtStyle, TEXT_STYLE_ATTR_I_FONT_EDGING, &outResult);
    EXPECT_EQ(res, OH_DRAWING_SUCCESS);
    EXPECT_EQ(outResult, static_cast<int>(FONT_EDGING_SUBPIXEL_ANTI_ALIAS));

    OH_Drawing_DestroyTextStyle(txtStyle);
}

/*
 * @tc.name: TextStyleAttributeFontEdgingTest003
 * @tc.desc: test font edging with nullptr parameter.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTextStyleFontEdgingTest, TextStyleAttributeFontEdgingTest003, TestSize.Level0)
{
    // Test set with nullptr
    auto res = OH_Drawing_SetTextStyleAttributeInt(
        nullptr, TEXT_STYLE_ATTR_I_FONT_EDGING, static_cast<int>(FONT_EDGING_ALIAS));
    EXPECT_EQ(res, OH_DRAWING_ERROR_INVALID_PARAMETER);

    // Test get with nullptr style
    int outResult = 0;
    res = OH_Drawing_GetTextStyleAttributeInt(nullptr, TEXT_STYLE_ATTR_I_FONT_EDGING, &outResult);
    EXPECT_EQ(res, OH_DRAWING_ERROR_INVALID_PARAMETER);

    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(txtStyle, nullptr);

    // Test get with nullptr output parameter
    res = OH_Drawing_GetTextStyleAttributeInt(txtStyle, TEXT_STYLE_ATTR_I_FONT_EDGING, nullptr);
    EXPECT_EQ(res, OH_DRAWING_ERROR_INVALID_PARAMETER);

    OH_Drawing_DestroyTextStyle(txtStyle);
}

/*
 * @tc.name: TextStyleAttributeFontEdgingTest004
 * @tc.desc: test font edging with mismatched attribute ID.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTextStyleFontEdgingTest, TextStyleAttributeFontEdgingTest004, TestSize.Level0)
{
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(txtStyle, nullptr);

    // Test with invalid attribute ID (using double attribute ID for int setter)
    auto res = OH_Drawing_SetTextStyleAttributeInt(
        txtStyle, TEXT_STYLE_ATTR_D_LINE_HEIGHT_MAXIMUM, static_cast<int>(FONT_EDGING_ALIAS));
    EXPECT_EQ(res, OH_DRAWING_ERROR_ATTRIBUTE_ID_MISMATCH);

    // Test get with invalid attribute ID
    int outResult = 0;
    res = OH_Drawing_GetTextStyleAttributeInt(txtStyle, TEXT_STYLE_ATTR_D_LINE_HEIGHT_MAXIMUM, &outResult);
    EXPECT_EQ(res, OH_DRAWING_ERROR_ATTRIBUTE_ID_MISMATCH);

    OH_Drawing_DestroyTextStyle(txtStyle);
}

/*
 * @tc.name: TextStyleAttributeFontEdgingTest005
 * @tc.desc: test font edging conversion to native TextStyle.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTextStyleFontEdgingTest, TextStyleAttributeFontEdgingTest005, TestSize.Level0)
{
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(txtStyle, nullptr);

    // Set to ALIAS and verify native conversion
    OH_Drawing_SetTextStyleAttributeInt(txtStyle, TEXT_STYLE_ATTR_I_FONT_EDGING, static_cast<int>(FONT_EDGING_ALIAS));
    int outResult = 0;
    OH_Drawing_GetTextStyleAttributeInt(txtStyle, TEXT_STYLE_ATTR_I_FONT_EDGING, &outResult);
    EXPECT_EQ(outResult, static_cast<int>(FONT_EDGING_ALIAS));
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->fontEdging, Drawing::FontEdging::ALIAS);

    // Set to SUBPIXEL_ANTI_ALIAS and verify native conversion
    OH_Drawing_SetTextStyleAttributeInt(
        txtStyle, TEXT_STYLE_ATTR_I_FONT_EDGING, static_cast<int>(FONT_EDGING_SUBPIXEL_ANTI_ALIAS));
    OH_Drawing_GetTextStyleAttributeInt(txtStyle, TEXT_STYLE_ATTR_I_FONT_EDGING, &outResult);
    EXPECT_EQ(outResult, static_cast<int>(FONT_EDGING_SUBPIXEL_ANTI_ALIAS));
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->fontEdging, Drawing::FontEdging::SUBPIXEL_ANTI_ALIAS);

    OH_Drawing_DestroyTextStyle(txtStyle);
}

/*
 * @tc.name: TextStyleAttributeFontEdgingTest006
 * @tc.desc: test font edging default value.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTextStyleFontEdgingTest, TextStyleAttributeFontEdgingTest006, TestSize.Level0)
{
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(txtStyle, nullptr);

    // Verify default value is ANTI_ALIAS
    int outResult = 0;
    auto res = OH_Drawing_GetTextStyleAttributeInt(txtStyle, TEXT_STYLE_ATTR_I_FONT_EDGING, &outResult);
    EXPECT_EQ(res, OH_DRAWING_SUCCESS);
    EXPECT_EQ(outResult, static_cast<int>(FONT_EDGING_ANTI_ALIAS));
    EXPECT_EQ(ConvertToOriginalText(txtStyle)->fontEdging, Drawing::FontEdging::ANTI_ALIAS);

    OH_Drawing_DestroyTextStyle(txtStyle);
}

/*
 * @tc.name: TextStyleAttributeFontEdgingTest007
 * @tc.desc: test font edging with typography workflow.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTextStyleFontEdgingTest, TextStyleAttributeFontEdgingTest007, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_SetTypographyTextFontSize(typoStyle, DEFAULT_FONT_SIZE);

    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(txtStyle, nullptr);
    OH_Drawing_SetTextStyleFontSize(txtStyle, DEFAULT_FONT_SIZE);

    // Set font edging to SUBPIXEL_ANTI_ALIAS
    OH_Drawing_SetTextStyleAttributeInt(
        txtStyle, TEXT_STYLE_ATTR_I_FONT_EDGING, static_cast<int>(FONT_EDGING_SUBPIXEL_ANTI_ALIAS));

    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    ASSERT_NE(handler, nullptr);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    OH_Drawing_TypographyHandlerAddText(handler, DEFAULT_TEXT);

    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    ASSERT_NE(typography, nullptr);
    OH_Drawing_TypographyLayout(typography, fLayoutWidth);

    // Verify font edging is preserved
    int outResult = 0;
    auto res = OH_Drawing_GetTextStyleAttributeInt(txtStyle, TEXT_STYLE_ATTR_I_FONT_EDGING, &outResult);
    EXPECT_EQ(res, OH_DRAWING_SUCCESS);
    EXPECT_EQ(outResult, static_cast<int>(FONT_EDGING_SUBPIXEL_ANTI_ALIAS));

    OH_Drawing_DestroyTextStyle(txtStyle);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: TextStyleAttributeFontEdgingTest008
 * @tc.desc: test font edging boundary values.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTextStyleFontEdgingTest, TextStyleAttributeFontEdgingTest008, TestSize.Level0)
{
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(txtStyle, nullptr);

    // Test lower boundary (FONT_EDGING_ALIAS = 0)
    auto res = OH_Drawing_SetTextStyleAttributeInt(txtStyle, TEXT_STYLE_ATTR_I_FONT_EDGING, 0);
    EXPECT_EQ(res, OH_DRAWING_SUCCESS);
    int outResult = 0;
    OH_Drawing_GetTextStyleAttributeInt(txtStyle, TEXT_STYLE_ATTR_I_FONT_EDGING, &outResult);
    EXPECT_EQ(outResult, 0);

    // Test upper boundary (FONT_EDGING_SUBPIXEL_ANTI_ALIAS = 2)
    res = OH_Drawing_SetTextStyleAttributeInt(txtStyle, TEXT_STYLE_ATTR_I_FONT_EDGING, 2);
    EXPECT_EQ(res, OH_DRAWING_SUCCESS);
    OH_Drawing_GetTextStyleAttributeInt(txtStyle, TEXT_STYLE_ATTR_I_FONT_EDGING, &outResult);
    EXPECT_EQ(outResult, 2);

    // Test value just below lower boundary
    res = OH_Drawing_SetTextStyleAttributeInt(txtStyle, TEXT_STYLE_ATTR_I_FONT_EDGING, -1);
    EXPECT_EQ(res, OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE);

    // Test value just above upper boundary
    res = OH_Drawing_SetTextStyleAttributeInt(txtStyle, TEXT_STYLE_ATTR_I_FONT_EDGING, 3);
    EXPECT_EQ(res, OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE);

    OH_Drawing_DestroyTextStyle(txtStyle);
}

/*
 * @tc.name: TextStyleAttributeFontEdgingTest009
 * @tc.desc: test font edging state persistence across multiple sets.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTextStyleFontEdgingTest, TextStyleAttributeFontEdgingTest009, TestSize.Level0)
{
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(txtStyle, nullptr);

    // Set to ALIAS
    OH_Drawing_SetTextStyleAttributeInt(txtStyle, TEXT_STYLE_ATTR_I_FONT_EDGING, static_cast<int>(FONT_EDGING_ALIAS));
    int outResult = 0;
    OH_Drawing_GetTextStyleAttributeInt(txtStyle, TEXT_STYLE_ATTR_I_FONT_EDGING, &outResult);
    EXPECT_EQ(outResult, static_cast<int>(FONT_EDGING_ALIAS));

    // Change to ANTI_ALIAS
    OH_Drawing_SetTextStyleAttributeInt(
        txtStyle, TEXT_STYLE_ATTR_I_FONT_EDGING, static_cast<int>(FONT_EDGING_ANTI_ALIAS));
    OH_Drawing_GetTextStyleAttributeInt(txtStyle, TEXT_STYLE_ATTR_I_FONT_EDGING, &outResult);
    EXPECT_EQ(outResult, static_cast<int>(FONT_EDGING_ANTI_ALIAS));

    // Change to SUBPIXEL_ANTI_ALIAS
    OH_Drawing_SetTextStyleAttributeInt(
        txtStyle, TEXT_STYLE_ATTR_I_FONT_EDGING, static_cast<int>(FONT_EDGING_SUBPIXEL_ANTI_ALIAS));
    OH_Drawing_GetTextStyleAttributeInt(txtStyle, TEXT_STYLE_ATTR_I_FONT_EDGING, &outResult);
    EXPECT_EQ(outResult, static_cast<int>(FONT_EDGING_SUBPIXEL_ANTI_ALIAS));

    // Change back to ALIAS
    OH_Drawing_SetTextStyleAttributeInt(txtStyle, TEXT_STYLE_ATTR_I_FONT_EDGING, static_cast<int>(FONT_EDGING_ALIAS));
    OH_Drawing_GetTextStyleAttributeInt(txtStyle, TEXT_STYLE_ATTR_I_FONT_EDGING, &outResult);
    EXPECT_EQ(outResult, static_cast<int>(FONT_EDGING_ALIAS));

    OH_Drawing_DestroyTextStyle(txtStyle);
}

} // namespace OHOS
