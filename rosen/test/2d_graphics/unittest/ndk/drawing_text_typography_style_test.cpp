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
namespace {
constexpr static float FLOAT_DATA_EPSILON = 1e-6f;
} // namespace
class OhDrawingTypographyStyleTest : public testing::Test {};
/*
 * @tc.name: OH_Drawing_SetTypographyTextAutoSpaceTest001
 * @tc.desc: test for set auto space when paragraph with single run
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyStyleTest, OH_Drawing_SetTypographyTextAutoSpaceTest001, TestSize.Level1)
{
    std::string text = "SingRun©2002-2001";
    OH_Drawing_TextStyle* txtStyle_ = OH_Drawing_CreateTextStyle();
    OH_Drawing_FontCollection* fontCollection_ = OH_Drawing_CreateSharedFontCollection();
    double myFontSize = 40;
    OH_Drawing_SetTextStyleFontSize(txtStyle_, myFontSize);

    // paragraph1
    OH_Drawing_TypographyStyle* typoStyle_ = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextAutoSpace(typoStyle_, true);

    OH_Drawing_TypographyCreate* handler_ = OH_Drawing_CreateTypographyHandler(typoStyle_, fontCollection_);
    OH_Drawing_TypographyHandlerPushTextStyle(handler_, txtStyle_);
    OH_Drawing_TypographyHandlerAddText(handler_, text.c_str());
    OH_Drawing_Typography* typography_ = OH_Drawing_CreateTypography(handler_);
    OH_Drawing_TypographyLayout(typography_, 1000);
    double longestLineTrue = OH_Drawing_TypographyGetLongestLine(typography_);
    double line1True = OH_Drawing_TypographyGetLineWidth(typography_, 0);
    EXPECT_NEAR(longestLineTrue, line1True, FLOAT_DATA_EPSILON);

    // paragraph2
    OH_Drawing_TypographyStyle* typoStyle2_ = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextAutoSpace(typoStyle2_, false);

    OH_Drawing_TypographyCreate* handler2_ = OH_Drawing_CreateTypographyHandler(typoStyle2_, fontCollection_);
    OH_Drawing_TypographyHandlerPushTextStyle(handler2_, txtStyle_);
    OH_Drawing_TypographyHandlerAddText(handler2_, text.c_str());
    OH_Drawing_Typography* typography2_ = OH_Drawing_CreateTypography(handler2_);
    OH_Drawing_TypographyLayout(typography2_, 1000);
    double longestLineFalse = OH_Drawing_TypographyGetLongestLine(typography2_);
    double line1False = OH_Drawing_TypographyGetLineWidth(typography2_, 0);
    EXPECT_NEAR(longestLineFalse, line1False, FLOAT_DATA_EPSILON);

    EXPECT_NEAR(longestLineTrue, longestLineFalse + myFontSize / 8 * 2, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(line1True, line1False + myFontSize / 8 * 2, FLOAT_DATA_EPSILON);

    OH_Drawing_DestroyFontCollection(fontCollection_);
    OH_Drawing_DestroyTypographyHandler(handler_);
    OH_Drawing_DestroyTypography(typography_);
    OH_Drawing_DestroyTypographyStyle(typoStyle_);
    OH_Drawing_DestroyTextStyle(txtStyle_);
    OH_Drawing_DestroyTypographyHandler(handler2_);
    OH_Drawing_DestroyTypography(typography2_);
    OH_Drawing_DestroyTypographyStyle(typoStyle2_);
}

/*
 * @tc.name: OH_Drawing_SetTypographyTextAutoSpaceTest002
 * @tc.desc: test for set auto space when paragraph with single run and the layout width is at the boundary value.
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyStyleTest, OH_Drawing_SetTypographyTextAutoSpaceTest002, TestSize.Level1)
{
    std::string text = "SingRun©2002-2001";
    OH_Drawing_TextStyle* txtStyle_ = OH_Drawing_CreateTextStyle();
    OH_Drawing_FontCollection* fontCollection_ = OH_Drawing_CreateSharedFontCollection();
    OH_Drawing_SetTextStyleFontSize(txtStyle_, 40);

    // test boundary value：Use longestline without autospace as layout width when autospace enabled, line count + 1
    OH_Drawing_TypographyStyle* typoStyle3_ = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextAutoSpace(typoStyle3_, true);
    OH_Drawing_TypographyCreate* handler3_ = OH_Drawing_CreateTypographyHandler(typoStyle3_, fontCollection_);
    OH_Drawing_TypographyHandlerPushTextStyle(handler3_, txtStyle_);
    OH_Drawing_TypographyHandlerAddText(handler3_, text.c_str());
    OH_Drawing_Typography* typography3_ = OH_Drawing_CreateTypography(handler3_);
    double layoutWidth = 388.319641;
    OH_Drawing_TypographyLayout(typography3_, layoutWidth);
    double longestLineTrue3 = OH_Drawing_TypographyGetLongestLine(typography3_);
    double line1True3 = OH_Drawing_TypographyGetLineWidth(typography3_, 0);
    double line2True3 = OH_Drawing_TypographyGetLineWidth(typography3_, 1);
    size_t lineCount3 = OH_Drawing_TypographyGetLineCount(typography3_);
    EXPECT_NEAR(longestLineTrue3, std::max(line1True3, line2True3), FLOAT_DATA_EPSILON);
    EXPECT_GT(layoutWidth, longestLineTrue3);
    EXPECT_GT(layoutWidth, line1True3);
    EXPECT_GT(line2True3, 0);

    OH_Drawing_TypographyStyle* typoStyle4_ = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextAutoSpace(typoStyle4_, false);
    OH_Drawing_TypographyCreate* handler4_ = OH_Drawing_CreateTypographyHandler(typoStyle4_, fontCollection_);
    OH_Drawing_TypographyHandlerPushTextStyle(handler4_, txtStyle_);
    OH_Drawing_TypographyHandlerAddText(handler4_, text.c_str());
    OH_Drawing_Typography* typography4_ = OH_Drawing_CreateTypography(handler4_);
    OH_Drawing_TypographyLayout(typography4_, layoutWidth);
    double longestLineFalse4 = OH_Drawing_TypographyGetLongestLine(typography4_);
    double line1False4 = OH_Drawing_TypographyGetLineWidth(typography4_, 0);
    size_t lineCount4 = OH_Drawing_TypographyGetLineCount(typography4_);
    EXPECT_NEAR(longestLineFalse4, line1False4, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(longestLineFalse4, layoutWidth, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(line1False4, layoutWidth, FLOAT_DATA_EPSILON);

    EXPECT_GT(longestLineFalse4, longestLineTrue3);
    EXPECT_GT(line1False4, line1True3);
    EXPECT_EQ(lineCount3, lineCount4 + 1);

    OH_Drawing_DestroyFontCollection(fontCollection_);
    OH_Drawing_DestroyTextStyle(txtStyle_);
    OH_Drawing_DestroyTypographyHandler(handler3_);
    OH_Drawing_DestroyTypography(typography3_);
    OH_Drawing_DestroyTypographyStyle(typoStyle3_);
    OH_Drawing_DestroyTypographyHandler(handler4_);
    OH_Drawing_DestroyTypography(typography4_);
    OH_Drawing_DestroyTypographyStyle(typoStyle4_);
}

/*
 * @tc.name: OH_Drawing_SetTypographyTextAutoSpaceTest003
 * @tc.desc: test for set auto space when paragraph with many lines
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyStyleTest, OH_Drawing_SetTypographyTextAutoSpaceTest003, TestSize.Level1)
{
    std::string text = "嫌疑者X的牺牲\n版权所有©2002-2001华为技术有限公司保留一切权利\n卸载USB设备";
    OH_Drawing_TextStyle* txtStyle_ = OH_Drawing_CreateTextStyle();
    OH_Drawing_FontCollection* fontCollection_ = OH_Drawing_CreateSharedFontCollection();
    double myFontSize = 40;
    OH_Drawing_SetTextStyleFontSize(txtStyle_, myFontSize);

    // paragraph1
    OH_Drawing_TypographyStyle* typoStyle_ = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextAutoSpace(typoStyle_, true);

    OH_Drawing_TypographyCreate* handler_ = OH_Drawing_CreateTypographyHandler(typoStyle_, fontCollection_);
    OH_Drawing_TypographyHandlerPushTextStyle(handler_, txtStyle_);
    OH_Drawing_TypographyHandlerAddText(handler_, text.c_str());
    OH_Drawing_Typography* typography_ = OH_Drawing_CreateTypography(handler_);
    OH_Drawing_TypographyLayout(typography_, 1000);
    double longestLineTrue = OH_Drawing_TypographyGetLongestLine(typography_);
    double line1True = OH_Drawing_TypographyGetLineWidth(typography_, 0);
    double line2True = OH_Drawing_TypographyGetLineWidth(typography_, 1);
    double line3True = OH_Drawing_TypographyGetLineWidth(typography_, 2);
    EXPECT_NEAR(longestLineTrue, std::max(line1True, std::max(line2True, line3True)), FLOAT_DATA_EPSILON);

    // paragraph2
    OH_Drawing_TypographyStyle* typoStyle2_ = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextAutoSpace(typoStyle2_, false);

    OH_Drawing_TypographyCreate* handler2_ = OH_Drawing_CreateTypographyHandler(typoStyle2_, fontCollection_);
    OH_Drawing_TypographyHandlerPushTextStyle(handler2_, txtStyle_);
    OH_Drawing_TypographyHandlerAddText(handler2_, text.c_str());
    OH_Drawing_Typography* typography2_ = OH_Drawing_CreateTypography(handler2_);
    OH_Drawing_TypographyLayout(typography2_, 1000);
    double longestLineFalse = OH_Drawing_TypographyGetLongestLine(typography2_);
    double line1False = OH_Drawing_TypographyGetLineWidth(typography2_, 0);
    double line2False = OH_Drawing_TypographyGetLineWidth(typography2_, 1);
    double line3False = OH_Drawing_TypographyGetLineWidth(typography2_, 2);
    EXPECT_NEAR(longestLineFalse, std::max(line1False, std::max(line2False, line3False)), FLOAT_DATA_EPSILON);

    EXPECT_NEAR(longestLineTrue, longestLineFalse + myFontSize / 8 * 3, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(line1True, line1False + myFontSize / 8 * 2, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(line2True, line2False + myFontSize / 8 * 3, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(line3True, line3False + myFontSize / 8 * 2, FLOAT_DATA_EPSILON);

    OH_Drawing_DestroyFontCollection(fontCollection_);
    OH_Drawing_DestroyTypographyHandler(handler_);
    OH_Drawing_DestroyTypography(typography_);
    OH_Drawing_DestroyTypographyStyle(typoStyle_);
    OH_Drawing_DestroyTextStyle(txtStyle_);
    OH_Drawing_DestroyTypographyHandler(handler2_);
    OH_Drawing_DestroyTypography(typography2_);
    OH_Drawing_DestroyTypographyStyle(typoStyle2_);
}

/*
 * @tc.name: OH_Drawing_SetTypographyTextAutoSpaceTest004
 * @tc.desc: test for set auto space when paragraph is many lines and the layout width is at the boundary value.
 * @tc.type: FUNC
 */
HWTEST_F(OhDrawingTypographyStyleTest, OH_Drawing_SetTypographyTextAutoSpaceTest004, TestSize.Level1)
{
    std::string text = "嫌疑者X的牺牲\n版权所有©2002-2001华为技术有限公司保留一切权利\n卸载USB设备";
    OH_Drawing_TextStyle* txtStyle_ = OH_Drawing_CreateTextStyle();
    OH_Drawing_FontCollection* fontCollection_ = OH_Drawing_CreateSharedFontCollection();
    OH_Drawing_SetTextStyleFontSize(txtStyle_, 40);

    // test boundary value：Use longestline without autospace as layout width when autospace enabled, line count + 1
    OH_Drawing_TypographyStyle* typoStyle3_ = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextAutoSpace(typoStyle3_, true);
    OH_Drawing_TypographyCreate* handler3_ = OH_Drawing_CreateTypographyHandler(typoStyle3_, fontCollection_);
    OH_Drawing_TypographyHandlerPushTextStyle(handler3_, txtStyle_);
    OH_Drawing_TypographyHandlerAddText(handler3_, text.c_str());
    OH_Drawing_Typography* typography3_ = OH_Drawing_CreateTypography(handler3_);
    double layoutWidth = 956.159546;
    OH_Drawing_TypographyLayout(typography3_, layoutWidth);
    double longestLineTrue3 = OH_Drawing_TypographyGetLongestLine(typography3_);
    double line1True3 = OH_Drawing_TypographyGetLineWidth(typography3_, 0);
    double line2True3 = OH_Drawing_TypographyGetLineWidth(typography3_, 1);
    double line3True3 = OH_Drawing_TypographyGetLineWidth(typography3_, 1);
    double line4True3 = OH_Drawing_TypographyGetLineWidth(typography3_, 1);
    size_t lineCount3 = OH_Drawing_TypographyGetLineCount(typography3_);
    EXPECT_GT(layoutWidth, longestLineTrue3);
    EXPECT_GT(layoutWidth, line1True3);
    EXPECT_GT(line3True3, 0);

    OH_Drawing_TypographyStyle* typoStyle4_ = OH_Drawing_CreateTypographyStyle();
    OH_Drawing_SetTypographyTextAutoSpace(typoStyle4_, false);
    OH_Drawing_TypographyCreate* handler4_ = OH_Drawing_CreateTypographyHandler(typoStyle4_, fontCollection_);
    OH_Drawing_TypographyHandlerPushTextStyle(handler4_, txtStyle_);
    OH_Drawing_TypographyHandlerAddText(handler4_, text.c_str());
    OH_Drawing_Typography* typography4_ = OH_Drawing_CreateTypography(handler4_);
    OH_Drawing_TypographyLayout(typography4_, layoutWidth);
    double longestLineFalse4 = OH_Drawing_TypographyGetLongestLine(typography4_);
    double line1False4 = OH_Drawing_TypographyGetLineWidth(typography4_, 0);
    double line2False4 = OH_Drawing_TypographyGetLineWidth(typography4_, 1);
    double line3False4 = OH_Drawing_TypographyGetLineWidth(typography4_, 2);
    size_t lineCount4 = OH_Drawing_TypographyGetLineCount(typography4_);
    EXPECT_NEAR(longestLineFalse4, layoutWidth, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(line2False4, layoutWidth, FLOAT_DATA_EPSILON);

    EXPECT_GT(longestLineFalse4, longestLineTrue3);
    EXPECT_GT(line1True3, line1False4);
    // The critical width value squeezed the second line down, so the characters are fewer, and the width is shorter.
    EXPECT_GT(line2False4, line2True3);
    EXPECT_GT(line4True3, line3False4);
    EXPECT_EQ(lineCount3, lineCount4 + 1);

    OH_Drawing_DestroyFontCollection(fontCollection_);
    OH_Drawing_DestroyTextStyle(txtStyle_);
    OH_Drawing_DestroyTypographyHandler(handler3_);
    OH_Drawing_DestroyTypography(typography3_);
    OH_Drawing_DestroyTypographyStyle(typoStyle3_);
    OH_Drawing_DestroyTypographyHandler(handler4_);
    OH_Drawing_DestroyTypography(typography4_);
    OH_Drawing_DestroyTypographyStyle(typoStyle4_);
}
} // namespace OHOS