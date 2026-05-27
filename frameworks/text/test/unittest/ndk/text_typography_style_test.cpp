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

#include <securec.h>

#include "drawing_font_collection.h"
#include "drawing_text_typography.h"
#include "gtest/gtest.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
class NdkTypographyStyleTest : public testing::Test {
public:
    void TearDown() override;

protected:
    OH_Drawing_TypographyStyle* typoStyle_{nullptr};
};

void NdkTypographyStyleTest::TearDown()
{
    if (typoStyle_ != nullptr) {
        OH_Drawing_DestroyTypographyStyle(typoStyle_);
        typoStyle_ = nullptr;
    }
}

/*
 * @tc.name: GlyphPositionAtCoordinateWithClusterDashLine
 * @tc.desc: test for GlyphPositionAtCoordinate with dash
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, GlyphPositionAtCoordinateWithClusterDashLine, TestSize.Level0)
{
    std::string text3 = "————————";

    double maxWidth3 = 1000.0;
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    OH_Drawing_FontCollection* fontCollection = OH_Drawing_GetFontCollectionGlobalInstance();

    OH_Drawing_SetTextStyleFontSize(txtStyle, 30);
    OH_Drawing_SetTextStyleColor(txtStyle, OH_Drawing_ColorSetArgb(0xFF, 0x00, 0x00, 0x00));
    OH_Drawing_TypographyCreate* handler = OH_Drawing_CreateTypographyHandler(typoStyle, fontCollection);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);

    OH_Drawing_TypographyHandlerAddText(handler, text3.c_str());
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    OH_Drawing_TypographyLayout(typography, maxWidth3);

    float xCoords[] = { 0, 20, 30, 45, 60, 75, 100 };
    int expectedPositions[] = { 0, 1, 1, 2, 2, 3, 3 };
    int expectedAffinities[] = { 1, 0, 1, 0, 1, 0, 1 };
    OH_Drawing_PositionAndAffinity* results[7];
    for (int i = 0; i < 7; i++) {
        results[i] = OH_Drawing_TypographyGetGlyphPositionAtCoordinateWithCluster(typography, xCoords[i], 0);
        EXPECT_EQ(OH_Drawing_GetPositionFromPositionAndAffinity(results[i]), expectedPositions[i]);
        EXPECT_EQ(OH_Drawing_GetAffinityFromPositionAndAffinity(results[i]), expectedAffinities[i]);
    }

    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTextStyle(txtStyle);
    for (int i = 0; i < 7; i++) {
        free(results[i]);
    }
}

/*
 * @tc.name: SetTypographyStyleAttributeBool
 * @tc.desc: test for set typography style descriptor attribute bool and error code.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, SetTypographyStyleAttributeBool, TestSize.Level0)
{
    typoStyle_ = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle_, nullptr);
    OH_Drawing_ErrorCode errCodeSet = OH_Drawing_SetTypographyStyleAttributeBool(typoStyle_,
        TYPOGRAPHY_STYLE_ATTR_B_COMPRESS_HEAD_PUNCTUATION, true);
    EXPECT_EQ(errCodeSet, 0);
    errCodeSet = OH_Drawing_SetTypographyStyleAttributeBool(nullptr,
        TYPOGRAPHY_STYLE_ATTR_B_COMPRESS_HEAD_PUNCTUATION, true);
    EXPECT_EQ(errCodeSet, OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    errCodeSet = OH_Drawing_SetTypographyStyleAttributeBool(typoStyle_,
        TYPOGRAPHY_STYLE_ATTR_I_LINE_HEIGHT_STYLE, true);
    EXPECT_EQ(errCodeSet, OH_DRAWING_ERROR_ATTRIBUTE_ID_MISMATCH);
}

/*
 * @tc.name: GetTypographyStyleAttributeBool
 * @tc.desc: test for get typography style descriptor attribute bool and error code.
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, GetTypographyStyleAttributeBool, TestSize.Level0)
{
    typoStyle_ = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle_, nullptr);
    OH_Drawing_ErrorCode errCodeSet = OH_Drawing_SetTypographyStyleAttributeBool(typoStyle_,
        TYPOGRAPHY_STYLE_ATTR_B_COMPRESS_HEAD_PUNCTUATION, true);
    EXPECT_EQ(errCodeSet, 0);

    bool value = false;
    OH_Drawing_ErrorCode errCodeGet = OH_Drawing_GetTypographyStyleAttributeBool(typoStyle_,
        TYPOGRAPHY_STYLE_ATTR_B_COMPRESS_HEAD_PUNCTUATION, &value);
    EXPECT_EQ(errCodeGet, 0);
    EXPECT_TRUE(value);
    errCodeGet = OH_Drawing_GetTypographyStyleAttributeBool(nullptr,
        TYPOGRAPHY_STYLE_ATTR_B_COMPRESS_HEAD_PUNCTUATION, &value);
    EXPECT_EQ(errCodeGet, OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    errCodeGet = OH_Drawing_GetTypographyStyleAttributeBool(typoStyle_,
        TYPOGRAPHY_STYLE_ATTR_B_COMPRESS_HEAD_PUNCTUATION, nullptr);
    EXPECT_EQ(errCodeGet, OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    errCodeGet = OH_Drawing_GetTypographyStyleAttributeBool(typoStyle_,
        TYPOGRAPHY_STYLE_ATTR_I_FONT_WIDTH, &value);
    EXPECT_EQ(errCodeGet, OH_DRAWING_ERROR_ATTRIBUTE_ID_MISMATCH);
}

/*
 * @tc.name: DestroyPositionAndAffinityNullptr
 * @tc.desc: Test for destroy nullptr for OH_Drawing_DestroyPositionAndAffinity
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, DestroyPositionAndAffinityNullptr, TestSize.Level0)
{
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_DestroyPositionAndAffinity(nullptr);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}

/*
 * @tc.name: DestroyPositionAndAffinityObject
 * @tc.desc: Test for destroy positionAndAffinity object
 * @tc.type: FUNC
 */
HWTEST_F(NdkTypographyStyleTest, DestroyPositionAndAffinityObject, TestSize.Level0)
{
    const char* text = "Destroy positionAndAffinity";
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    ASSERT_NE(typoStyle, nullptr);
    OH_Drawing_TypographyCreate* handler =
        OH_Drawing_CreateTypographyHandler(typoStyle, OH_Drawing_CreateFontCollection());
    ASSERT_NE(handler, nullptr);
    OH_Drawing_TextStyle* textStyle = OH_Drawing_CreateTextStyle();
    ASSERT_NE(textStyle, nullptr);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, textStyle);
    OH_Drawing_TypographyHandlerAddText(handler, text);
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    ASSERT_NE(typography, nullptr);
    // Test for layout width 500
    double layoutWidth = 500;
    OH_Drawing_TypographyLayout(typography, layoutWidth);
    OH_Drawing_PositionAndAffinity* positionAndAffinity =
        OH_Drawing_TypographyGetGlyphPositionAtCoordinateWithCluster(typography, 0, 1);
    ASSERT_NE(positionAndAffinity, nullptr);
    OH_Drawing_DestroyPositionAndAffinity(positionAndAffinity);
    OH_Drawing_DestroyTypography(typography);
    OH_Drawing_DestroyTypographyHandler(handler);
    OH_Drawing_DestroyTextStyle(textStyle);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
}
} // namespace OHOS
