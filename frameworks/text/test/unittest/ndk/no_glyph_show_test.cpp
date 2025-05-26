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

#include <gtest/gtest.h>

#include "drawing_bitmap.h"
#include "drawing_font_collection.h"
#include "drawing_rect.h"
#include "drawing_text_global.h"
#include "drawing_text_line.h"
#include "drawing_text_typography.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class NdkNoGlyphShowTest : public testing::Test {
public:
    OH_Drawing_Typography* PrepareCreateTextLine(const std::string& text);

private:
    static constexpr const char* text_ = "Hello 测 World \uffff\n!@#$%^&*~(){\uffff\uffff}[]90 - = ,.\n\uffff"
                                         "testlp\uffff试\uffff Drawing\uffff";
    static constexpr const char* onlyNoGlyph_ = "\uffff";
    static constexpr float defaultResult_[][4] = { { 2.0, 2.0, 206.63979, 29.0 }, { 1.0, 5.0, 388.10962, 37.0 },
        { 0, 8.0, 319.4397, 42.0 } };
    static constexpr float tofuResult_[][4] = { { 2.0, 2.0, 228.63979, 29.0 }, { 1.0, 5.0, 388.10962, 37.0 },
        { 8.0, 8.0, 341.4397, 42.0 } };
    static constexpr float onlyTofuResult_[][4] = { { 8.0, 0, 22.0, 22.0 } };
    static constexpr float onlyDefaultResult_[][4] = { { 0, 0, 0, 0 } };
};

OH_Drawing_Typography* NdkNoGlyphShowTest::PrepareCreateTextLine(const std::string& text)
{
    double maxWidth = 500.0;
    OH_Drawing_TypographyStyle* typoStyle = OH_Drawing_CreateTypographyStyle();
    EXPECT_NE(typoStyle, nullptr);
    OH_Drawing_TextStyle* txtStyle = OH_Drawing_CreateTextStyle();
    EXPECT_NE(txtStyle, nullptr);
    OH_Drawing_FontCollection* fontCollection = OH_Drawing_GetFontCollectionGlobalInstance();
    EXPECT_NE(fontCollection, nullptr);
    OH_Drawing_TypographyCreate* handler = OH_Drawing_CreateTypographyHandler(typoStyle, fontCollection);
    EXPECT_NE(handler, nullptr);
    OH_Drawing_SetTextStyleColor(txtStyle, OH_Drawing_ColorSetArgb(0xFF, 0x00, 0x00, 0x00));
    double fontSize = 30;
    OH_Drawing_SetTextStyleFontSize(txtStyle, fontSize);
    OH_Drawing_SetTextStyleFontWeight(txtStyle, FONT_WEIGHT_400);
    OH_Drawing_TypographyHandlerPushTextStyle(handler, txtStyle);
    OH_Drawing_TypographyHandlerAddText(handler, text.c_str());
    OH_Drawing_Typography* typography = OH_Drawing_CreateTypography(handler);
    EXPECT_NE(typography, nullptr);
    OH_Drawing_TypographyLayout(typography, maxWidth);
    OH_Drawing_DestroyTypographyStyle(typoStyle);
    OH_Drawing_DestroyTextStyle(txtStyle);
    OH_Drawing_DestroyTypographyHandler(handler);
    return typography;
}

#define BoundsResult(rectResult, size, text)                                                 \
    do {                                                                                     \
        OH_Drawing_Typography* typography = PrepareCreateTextLine(text);                     \
        OH_Drawing_Array* textLines = OH_Drawing_TypographyGetTextLines(typography);         \
        size_t arraySize = OH_Drawing_GetDrawingArraySize(textLines);                        \
        EXPECT_EQ(size, arraySize);                                                          \
        for (size_t index = 0; index < arraySize; index++) {                                 \
            OH_Drawing_TextLine* textLine = OH_Drawing_GetTextLineByIndex(textLines, index); \
            OH_Drawing_Rect* rect = OH_Drawing_TextLineGetImageBounds(textLine);             \
            EXPECT_FLOAT_EQ(rectResult[index][0], OH_Drawing_RectGetLeft(rect));             \
            EXPECT_FLOAT_EQ(rectResult[index][1], OH_Drawing_RectGetTop(rect));              \
            EXPECT_FLOAT_EQ(rectResult[index][2], OH_Drawing_RectGetRight(rect));            \
            EXPECT_FLOAT_EQ(rectResult[index][3], OH_Drawing_RectGetBottom(rect));           \
            OH_Drawing_RectDestroy(rect);                                                    \
            OH_Drawing_DestroyTextLine(textLine);                                            \
        }                                                                                    \
        OH_Drawing_DestroyTextLines(textLines);                                              \
        OH_Drawing_DestroyTypography(typography);                                            \
    } while (0)

/**
 * @tc.name: NdkNoGlyphShowTest001
 * @tc.desc: Test no glyph show use tofu
 * @tc.type: FUNC
 */
HWTEST_F(NdkNoGlyphShowTest, NdkNoGlyphShowTest001, TestSize.Level1)
{
    OH_Drawing_SetTextNoGlyphShow(TEXT_NO_GLYPH_USE_TOFU);
    BoundsResult(tofuResult_, 3, text_);
}

/**
 * @tc.name: NdkNoGlyphShowTest002
 * @tc.desc: Test no glyph show use default
 * @tc.type: FUNC
 */
HWTEST_F(NdkNoGlyphShowTest, NdkNoGlyphShowTest002, TestSize.Level1)
{
    OH_Drawing_SetTextNoGlyphShow(TEXT_NO_GLYPH_USE_DEFAULT);
    BoundsResult(defaultResult_, 3, text_);
}

/**
 * @tc.name: NdkNoGlyphShowTest003
 * @tc.desc: Test no glyph show use invalid input
 * @tc.type: FUNC
 */
HWTEST_F(NdkNoGlyphShowTest, NdkNoGlyphShowTest003, TestSize.Level1)
{
    OH_Drawing_SetTextNoGlyphShow(TEXT_NO_GLYPH_USE_DEFAULT);
    OH_Drawing_SetTextNoGlyphShow(static_cast<OH_Drawing_TextNoGlyphShow>(100));
    BoundsResult(defaultResult_, 3, text_);
    OH_Drawing_SetTextNoGlyphShow(TEXT_NO_GLYPH_USE_TOFU);
    OH_Drawing_SetTextNoGlyphShow(static_cast<OH_Drawing_TextNoGlyphShow>(100));
    BoundsResult(tofuResult_, 3, text_);
}

/**
 * @tc.name: NdkNoGlyphShowTest004
 * @tc.desc: Test no glyph show use only no glyph
 * @tc.type: FUNC
 */
HWTEST_F(NdkNoGlyphShowTest, NdkNoGlyphShowTest004, TestSize.Level1)
{
    OH_Drawing_SetTextNoGlyphShow(TEXT_NO_GLYPH_USE_DEFAULT);
    OH_Drawing_SetTextNoGlyphShow(static_cast<OH_Drawing_TextNoGlyphShow>(100));
    BoundsResult(onlyDefaultResult_, 1, onlyNoGlyph_);
    OH_Drawing_SetTextNoGlyphShow(TEXT_NO_GLYPH_USE_TOFU);
    OH_Drawing_SetTextNoGlyphShow(static_cast<OH_Drawing_TextNoGlyphShow>(100));
    BoundsResult(onlyTofuResult_, 1, onlyNoGlyph_);
}
} // namespace Rosen
} // namespace OHOS