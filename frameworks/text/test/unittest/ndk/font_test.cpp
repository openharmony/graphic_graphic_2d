/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, Hardware
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gtest/gtest.h"
#include "drawing_brush.h"
#include "drawing_error_code.h"
#include "drawing_font.h"
#include "drawing_path.h"
#include "drawing_rect.h"
#include "drawing_typeface.h"
#include "drawing_pen.h"
#include "drawing_point.h"

#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
constexpr float FLOAT_DATA_EPSILON = 1e-6f;
constexpr char TTF_FILE_PATH[] = {0x2F, 0x73, 0x79, 0x73, 0x74, 0x65, 0x6D, 0x2F, 0x66, 0x6F, 0x6E, 0x74, 0x73,
    0x2F, 0x48, 0x61, 0x72, 0x6D, 0x6F, 0x6E, 0x79, 0x4F, 0x53, 0x5F, 0x53, 0x61, 0x6E, 0x73, 0x5F,
    0x53, 0x43, 0x2E, 0x74, 0x74, 0x66, 0x00};
class NdkFontTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void NdkFontTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
}
void NdkFontTest::TearDownTestCase() {}
void NdkFontTest::SetUp() {}
void NdkFontTest::TearDown() {}

/*
 * @tc.name: GetMetrics001
 * @tc.desc: test for GetMetrics.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NdkFontTest, GetMetrics001, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    OH_Drawing_Font_Metrics cFontMetrics;
    EXPECT_TRUE(OH_Drawing_FontGetMetrics(font, &cFontMetrics) >= 0);
    EXPECT_TRUE(OH_Drawing_FontGetMetrics(font, nullptr) < 0);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_TRUE(OH_Drawing_FontGetMetrics(nullptr, nullptr) < 0);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: GetMetrics002
 * @tc.desc: test for sans sc metrics data.
 * @tc.type: FUNC
 */
HWTEST_F(NdkFontTest, GetMetrics002, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    OH_Drawing_Typeface* typeface = OH_Drawing_TypefaceCreateFromFile(TTF_FILE_PATH, 0);
    OH_Drawing_FontSetTypeface(font, typeface);
    OH_Drawing_Font_Metrics fontMetrics;
    OH_Drawing_FontGetMetrics(font, &fontMetrics);
    EXPECT_EQ(fontMetrics.flags, 31);
    EXPECT_NEAR(fontMetrics.top, -12.672000, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.ascent, -11.136000, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.descent, 2.928000, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.bottom, 3.252000, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.leading, 0, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.avgCharWidth, 6.000000, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.maxCharWidth, 29.832001, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.xMin, -6.576000, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.xMax, 23.256001, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.xHeight, 6.000000, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.capHeight, 8.400000, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.underlineThickness, 0.600000, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.underlinePosition, 2.484000, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.strikeoutThickness, 0.600000, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.strikeoutPosition, -3.600000, FLOAT_DATA_EPSILON);
    OH_Drawing_TypefaceDestroy(typeface);
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: GetMetrics003
 * @tc.desc: test for symbol metrics data.
 * @tc.type: FUNC
 */
HWTEST_F(NdkFontTest, GetMetrics003, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    OH_Drawing_Typeface* typeface = OH_Drawing_TypefaceCreateFromFile("/system/fonts/HMSymbolVF.ttf", 0);
    OH_Drawing_FontSetTypeface(font, typeface);
    OH_Drawing_Font_Metrics fontMetrics;
    OH_Drawing_FontGetMetrics(font, &fontMetrics);
    EXPECT_EQ(fontMetrics.flags, 31);
    EXPECT_NEAR(fontMetrics.top, -10.559999, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.ascent, -10.559999, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.descent, 1.440000, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.bottom, 1.440000, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.leading, 0, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.avgCharWidth, 12, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.maxCharWidth, 19.092000961303711, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.xMin, -0.684000, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.xMax, 18.408000946044922, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.xHeight, 6.000000, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.capHeight, 8.400000, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.underlineThickness, 0.600000, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.underlinePosition, 1.200000, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.strikeoutThickness, 0.600000, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.strikeoutPosition, -3.000000, FLOAT_DATA_EPSILON);
    OH_Drawing_TypefaceDestroy(typeface);
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: GetMetrics004
 * @tc.desc: test for emoji metrics data.
 * @tc.type: FUNC
 */
HWTEST_F(NdkFontTest, GetMetrics004, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    OH_Drawing_Typeface* typeface = OH_Drawing_TypefaceCreateFromFile("/system/fonts/HMOSColorEmojiFlags.ttf", 0);
    OH_Drawing_FontSetTypeface(font, typeface);
    OH_Drawing_Font_Metrics fontMetrics;
    OH_Drawing_FontGetMetrics(font, &fontMetrics);
    EXPECT_EQ(fontMetrics.flags, 31);
    EXPECT_NEAR(fontMetrics.top, -11.119267, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.ascent, -11.119267, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.descent, 2.972477, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.bottom, 2.972477, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.leading, 0, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.avgCharWidth, 14.941406, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.maxCharWidth, 14.972477, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.xMin, 0, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.xMax, 14.972477, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.xHeight, 11.119267, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.capHeight, 11.132812, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.underlineThickness, 0.767578, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.underlinePosition, 7.289062, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.strikeoutThickness, 0.597656, FLOAT_DATA_EPSILON);
    EXPECT_NEAR(fontMetrics.strikeoutPosition, -3.105469, FLOAT_DATA_EPSILON);
    OH_Drawing_TypefaceDestroy(typeface);
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: IsAndSetBaselineSnap002
 * @tc.desc: test for SetBaselineSnap and IsBaselineSnap.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NdkFontTest, IsAndSetBaselineSnap002, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    OH_Drawing_FontSetBaselineSnap(nullptr, true);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_FontIsBaselineSnap(nullptr), false);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_FontSetBaselineSnap(nullptr, false);
    EXPECT_EQ(OH_Drawing_FontIsBaselineSnap(nullptr), false);
    OH_Drawing_FontSetBaselineSnap(font, true);
    EXPECT_EQ(OH_Drawing_FontIsBaselineSnap(font), true);
    OH_Drawing_FontSetBaselineSnap(font, false);
    EXPECT_EQ(OH_Drawing_FontIsBaselineSnap(font), false);
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: IsAndSetSubpixel003
 * @tc.desc: test for SetSubpixel and IsSubpixel.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NdkFontTest, IsAndSetSubpixel003, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    OH_Drawing_FontSetSubpixel(nullptr, false);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_FontIsSubpixel(nullptr), false);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_FontSetSubpixel(nullptr, true);
    EXPECT_EQ(OH_Drawing_FontIsSubpixel(nullptr), false);
    OH_Drawing_FontSetSubpixel(font, true);
    EXPECT_EQ(OH_Drawing_FontIsSubpixel(font), true);
    OH_Drawing_FontSetSubpixel(font, false);
    EXPECT_EQ(OH_Drawing_FontIsSubpixel(font), false);
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: TextToGlyphs004
 * @tc.desc: test for TextToGlyphs.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NdkFontTest, TextToGlyphs004, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    OH_Drawing_FontSetTextSize(font, 100); // 100 means font text size
    EXPECT_NE(font, nullptr);
    const char *str = "hello world";
    OH_Drawing_FontCountText(nullptr, str, strlen(str), OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_FontCountText(font, nullptr, strlen(str), OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    uint32_t count = 0;
    count = OH_Drawing_FontCountText(font, str, strlen(str), OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8);
    EXPECT_EQ(11, count); // 11 means str length

    uint16_t glyphs[50] = {0}; // 50 means glyphs array number
    OH_Drawing_FontTextToGlyphs(nullptr, str, 0, OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8, glyphs, 0);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_FontTextToGlyphs(font, nullptr, 0, OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8, glyphs, 0);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_FontTextToGlyphs(font, str, strlen(str), OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8, nullptr, 0);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_FontTextToGlyphs(font, str, strlen(str), OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8, glyphs, 0);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    int glyphsCount = 1;
    glyphsCount = OH_Drawing_FontTextToGlyphs(font, str, 0,
        OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8, glyphs, 0);
    EXPECT_EQ(0, glyphsCount);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);

    glyphsCount = OH_Drawing_FontTextToGlyphs(font, str, strlen(str),
        OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8, glyphs, count);
    EXPECT_EQ(11, glyphsCount); // 11 means glyphsCount

    float widths[50] = {0.f}; // 50 means widths array number
    OH_Drawing_FontGetWidths(nullptr, glyphs, glyphsCount, widths);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_FontGetWidths(font, nullptr, glyphsCount, widths);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_FontGetWidths(font, glyphs, 0, widths);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_FontGetWidths(font, glyphs, glyphsCount, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_FontGetWidths(font, glyphs, glyphsCount, widths);
    EXPECT_EQ(58.0, widths[0]); // 58.0 means glyphs[0] width
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: SetAndGetScaleX005
 * @tc.desc: test for SetAndGetScaleX.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NdkFontTest, SetAndGetScaleX005, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    OH_Drawing_FontSetScaleX(nullptr, 2);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_TRUE(OH_Drawing_FontGetScaleX(nullptr) == -1);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_TRUE(OH_Drawing_FontGetScaleX(font) == 1);
    OH_Drawing_FontSetScaleX(font, 2);
    EXPECT_TRUE(OH_Drawing_FontGetScaleX(font) == 2);
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: GetAndSetEdging006
 * @tc.desc: test for GetAndSetEdging.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NdkFontTest, GetAndSetEdging006, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    EXPECT_EQ(OH_Drawing_FontGetEdging(font), OH_Drawing_FontEdging::FONT_EDGING_ANTI_ALIAS);
    EXPECT_EQ(OH_Drawing_FontGetEdging(nullptr), OH_Drawing_FontEdging::FONT_EDGING_ALIAS);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_FontSetEdging(nullptr, OH_Drawing_FontEdging::FONT_EDGING_ALIAS);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_FontGetEdging(font), OH_Drawing_FontEdging::FONT_EDGING_ANTI_ALIAS);
    OH_Drawing_FontSetEdging(font, OH_Drawing_FontEdging::FONT_EDGING_ALIAS);
    EXPECT_EQ(OH_Drawing_FontGetEdging(font), OH_Drawing_FontEdging::FONT_EDGING_ALIAS);
    OH_Drawing_FontSetEdging(font, OH_Drawing_FontEdging::FONT_EDGING_ANTI_ALIAS);
    EXPECT_EQ(OH_Drawing_FontGetEdging(font), OH_Drawing_FontEdging::FONT_EDGING_ANTI_ALIAS);
    OH_Drawing_FontSetEdging(font, OH_Drawing_FontEdging::FONT_EDGING_SUBPIXEL_ANTI_ALIAS);
    EXPECT_EQ(OH_Drawing_FontGetEdging(font), OH_Drawing_FontEdging::FONT_EDGING_SUBPIXEL_ANTI_ALIAS);
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: GetAndSetForceAutoHinting007
 * @tc.desc: test for GetAndSetForceAutoHinting.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NdkFontTest, GetAndSetForceAutoHinting007, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    EXPECT_EQ(OH_Drawing_FontIsForceAutoHinting(nullptr), false);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_FontSetForceAutoHinting(nullptr, true);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_FontIsForceAutoHinting(font), false);
    OH_Drawing_FontSetForceAutoHinting(font, true);
    EXPECT_EQ(OH_Drawing_FontIsForceAutoHinting(font), true);
    OH_Drawing_FontSetForceAutoHinting(font, false);
    EXPECT_EQ(OH_Drawing_FontIsForceAutoHinting(font), false);
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: GetAndSetHinting008
 * @tc.desc: test for GetHinting and SetHinting.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NdkFontTest, GetAndSetHinting008, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    EXPECT_TRUE(OH_Drawing_FontGetHinting(nullptr) == OH_Drawing_FontHinting::FONT_HINTING_NONE);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_FontSetHinting(nullptr, OH_Drawing_FontHinting::FONT_HINTING_NONE);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_FontSetHinting(font, OH_Drawing_FontHinting::FONT_HINTING_NONE);
    EXPECT_TRUE(OH_Drawing_FontGetHinting(font) == OH_Drawing_FontHinting::FONT_HINTING_NONE);
    OH_Drawing_FontSetHinting(font, OH_Drawing_FontHinting::FONT_HINTING_SLIGHT);
    EXPECT_TRUE(OH_Drawing_FontGetHinting(font) == OH_Drawing_FontHinting::FONT_HINTING_SLIGHT);
    OH_Drawing_FontSetHinting(font, OH_Drawing_FontHinting::FONT_HINTING_SLIGHT);
    EXPECT_TRUE(OH_Drawing_FontGetHinting(font) == OH_Drawing_FontHinting::FONT_HINTING_SLIGHT);
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: GetAndSetEmbeddedBitmaps009
 * @tc.desc: test for GetEmbeddedBitmaps and SetEmbeddedBitmaps.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NdkFontTest, GetAndSetEmbeddedBitmaps009, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    EXPECT_TRUE(OH_Drawing_FontIsEmbeddedBitmaps(nullptr) == false);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_FontSetEmbeddedBitmaps(nullptr, true);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_FontSetEmbeddedBitmaps(font, true);
    EXPECT_TRUE(OH_Drawing_FontIsEmbeddedBitmaps(font) == true);
    OH_Drawing_FontSetEmbeddedBitmaps(font, false);
    EXPECT_TRUE(OH_Drawing_FontIsEmbeddedBitmaps(font) == false);
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: GetTextSize010
 * @tc.desc: test for GetTextSize.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NdkFontTest, GetTextSize010, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    OH_Drawing_FontSetTextSize(nullptr, 100);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_FontGetTextSize(nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_FontSetTextSize(font, 100);
    float size = OH_Drawing_FontGetTextSize(font);
    EXPECT_EQ(size, 100);
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: GetTextSkewX011
 * @tc.desc: test for GetTextSkewX.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NdkFontTest, GetTextSkewX011, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    OH_Drawing_FontSetTextSkewX(nullptr, 10);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_FontGetTextSkewX(nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_FontSetTextSkewX(font, 10);
    float size = OH_Drawing_FontGetTextSkewX(font);
    EXPECT_EQ(size, 10);
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: IsLinearText012
 * @tc.desc: test for IsLinearText.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NdkFontTest, IsLinearText012, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    OH_Drawing_FontSetLinearText(nullptr, true);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_FontIsLinearText(nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    bool ret = OH_Drawing_FontIsLinearText(font);
    EXPECT_EQ(ret, false);
    OH_Drawing_FontSetLinearText(font, true);
    ret = OH_Drawing_FontIsLinearText(font);
    EXPECT_EQ(ret, true);
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: SetFakeBoldText013
 * @tc.desc: test for SetFakeBoldText.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NdkFontTest, SetFakeBoldText013, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    OH_Drawing_FontSetFakeBoldText(nullptr, true);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_FontIsFakeBoldText(nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    bool ret = OH_Drawing_FontIsFakeBoldText(font);
    EXPECT_EQ(ret, false);
    OH_Drawing_FontSetFakeBoldText(font, true);
    ret = OH_Drawing_FontIsFakeBoldText(font);
    EXPECT_EQ(ret, true);
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: FontMeasureText014
 * @tc.desc: test for FontMeasureText.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NdkFontTest, FontMeasureText014, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    OH_Drawing_FontSetTextSize(font, 50);
    const char* str = "hello world";
    float textWidth = 0.f;
    OH_Drawing_ErrorCode drawingErrorCode = OH_DRAWING_SUCCESS;
    drawingErrorCode = OH_Drawing_FontMeasureText(nullptr, str, strlen(str),
                                                  OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8, nullptr, &textWidth);
    EXPECT_EQ(drawingErrorCode, OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(textWidth, 0.f);
    drawingErrorCode = OH_Drawing_FontMeasureText(font, str, 0, OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8,
                                                  nullptr, &textWidth);
    EXPECT_EQ(drawingErrorCode, OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(textWidth, 0.f);
    drawingErrorCode = OH_Drawing_FontMeasureText(font, str, strlen(str), OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8,
                                                  nullptr, &textWidth);
    EXPECT_EQ(drawingErrorCode, OH_DRAWING_SUCCESS);
    EXPECT_EQ(textWidth, 254.0); // 254.0 is textWidth

    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: FontMeasureSingleCharacter015
 * @tc.desc: test for OH_Drawing_FontMeasureSingleCharacter.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NdkFontTest, FontMeasureSingleCharacter015, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    OH_Drawing_FontSetTextSize(font, 50); // 50 means font text size
    const char* strOne = "a";
    const char* strTwo = "你好";
    float textWidth = 0.f;
    OH_Drawing_ErrorCode drawingErrorCode = OH_DRAWING_SUCCESS;
    drawingErrorCode = OH_Drawing_FontMeasureSingleCharacter(nullptr, strOne, &textWidth);
    EXPECT_EQ(drawingErrorCode, OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(textWidth, 0.f);
    drawingErrorCode = OH_Drawing_FontMeasureSingleCharacter(font, nullptr, &textWidth);
    EXPECT_EQ(drawingErrorCode, OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(textWidth, 0.f);
    drawingErrorCode = OH_Drawing_FontMeasureSingleCharacter(font, strOne, nullptr);
    EXPECT_EQ(drawingErrorCode, OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(textWidth, 0.f);
    const char* strThree = "";
    drawingErrorCode = OH_Drawing_FontMeasureSingleCharacter(font, strThree, &textWidth);
    EXPECT_EQ(drawingErrorCode, OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(textWidth, 0.f);
    drawingErrorCode = OH_Drawing_FontMeasureSingleCharacter(font, strOne, &textWidth);
    EXPECT_EQ(drawingErrorCode, OH_DRAWING_SUCCESS);
    EXPECT_TRUE(textWidth > 0);
    drawingErrorCode = OH_Drawing_FontMeasureSingleCharacter(font, strTwo, &textWidth);
    EXPECT_EQ(drawingErrorCode, OH_DRAWING_SUCCESS);
    EXPECT_TRUE(textWidth > 0);
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: FontCreatePathForGlyph001
 * @tc.desc: test for common character of glyph ID.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NdkFontTest, FontGetPathForGlyph001, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    OH_Drawing_FontSetTextSize(font, 50);
    const char* str = "helloworld";
    uint32_t count = OH_Drawing_FontCountText(font, str, strlen(str), OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8);
    EXPECT_EQ(strlen(str), count);
    uint16_t glyphs[count];
    OH_Drawing_FontTextToGlyphs(font, str, strlen(str), OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8, glyphs, count);
    std::vector result = { 160.50444, 116.570892, 84.6191406, 84.6191406, 80.983223, 195.274139, 80.983223, 83.3096237,
        84.6191406, 126.002419 };
    for (int i = 0; i < count; i++) {
        OH_Drawing_Path* path = OH_Drawing_PathCreate();
        ASSERT_NE(path, nullptr);
        EXPECT_EQ(OH_Drawing_FontGetPathForGlyph(font, glyphs[i], path), OH_DRAWING_SUCCESS);
        EXPECT_LE(std::abs(OH_Drawing_PathGetLength(path, false) - result[i]), 0.01);
        EXPECT_TRUE(OH_Drawing_PathIsClosed(path, false));
        OH_Drawing_PathDestroy(path);
    }
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: FontCreatePathForGlyph002
 * @tc.desc: test for space character of glyph ID.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NdkFontTest, FontGetPathForGlyph002, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    OH_Drawing_FontSetTextSize(font, 50);
    const char* space = " ";
    uint32_t count = OH_Drawing_FontCountText(font, space, strlen(space), OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8);
    EXPECT_EQ(strlen(space), count);
    uint16_t glyphs[count];
    OH_Drawing_FontTextToGlyphs(font, space, strlen(space), OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8,
        glyphs, count);
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    ASSERT_NE(path, nullptr);
    EXPECT_EQ(OH_Drawing_FontGetPathForGlyph(font, glyphs[0], path), OH_DRAWING_SUCCESS);
    EXPECT_TRUE(OH_Drawing_PathGetLength(path, false) == 0);
    EXPECT_FALSE(OH_Drawing_PathIsClosed(path, false));
    if (path != nullptr) {
        OH_Drawing_PathDestroy(path);
    }
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: FontCreatePathForGlyph003
 * @tc.desc: test for abnormal parameter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NdkFontTest, FontGetPathForGlyph003, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    OH_Drawing_FontSetTextSize(font, 50);
    const char* str = "hello world";
    uint32_t count = OH_Drawing_FontCountText(font, str, strlen(str), OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8);
    EXPECT_EQ(strlen(str), count);
    uint16_t glyphs[count];
    OH_Drawing_FontTextToGlyphs(font, str, strlen(str), OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8,
        glyphs, count);

    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    ASSERT_NE(path, nullptr);
    EXPECT_EQ(OH_Drawing_FontGetPathForGlyph(nullptr, glyphs[0], path), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_FontGetPathForGlyph(font, glyphs[0], nullptr), OH_DRAWING_ERROR_INVALID_PARAMETER);
    if (path != nullptr) {
        OH_Drawing_PathDestroy(path);
    }
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: FontCreatePathForGlyph004
 * @tc.desc: test for non exist glyph ID.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NdkFontTest, FontGetPathForGlyph004, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    OH_Drawing_FontSetTextSize(font, 50);
    uint16_t glyphsNotExist = 65535;
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    ASSERT_NE(path, nullptr);
    EXPECT_EQ(OH_Drawing_FontGetPathForGlyph(font, glyphsNotExist, path), OH_DRAWING_ERROR_INVALID_PARAMETER);
    if (path != nullptr) {
        OH_Drawing_PathDestroy(path);
    }
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: FontGetBounds001
 * @tc.desc: test for common character of glyph ID.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NdkFontTest, FontGetBounds001, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    OH_Drawing_FontSetTextSize(font, 50);
    const char* str = "helloworld";
    uint32_t count = OH_Drawing_FontCountText(font, str, strlen(str), OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8);
    EXPECT_EQ(strlen(str), count);
    uint16_t glyphs[count];
    OH_Drawing_FontTextToGlyphs(font, str, strlen(str), OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8,
        glyphs, count);
    OH_Drawing_Array *outRectarr = OH_Drawing_RectCreateArray(count);
    ASSERT_NE(outRectarr, nullptr);
    size_t size = 0;
    EXPECT_EQ(OH_Drawing_RectGetArraySize(outRectarr, &size), OH_DRAWING_SUCCESS);
    EXPECT_EQ(size, count);
    EXPECT_EQ(OH_Drawing_FontGetBounds(font, glyphs, count, outRectarr), OH_DRAWING_SUCCESS);
    std::vector<std::array<int, 4>> arr = { { 23, 39, -39, 3 }, { 25, 28, -27, 2 }, { 6, 39, -39, 3 },
        { 6, 39, -39, 3 }, { 26, 28, -27, 2 }, { 39, 26, -26, 0 }, { 26, 28, -27, 2 }, { 16, 27, -27, 3 },
        { 6, 39, -39, 3 }, { 25, 40, -39, 2 } };
    for (int i = 0; i < count; i++) {
        OH_Drawing_Rect* iter = nullptr;
        EXPECT_EQ(OH_Drawing_RectGetArrayElement(outRectarr, i, &iter), OH_DRAWING_SUCCESS);
        ASSERT_NE(iter, nullptr);
        EXPECT_EQ(static_cast<int>(OH_Drawing_RectGetWidth(iter)), arr[i][0]);
        EXPECT_EQ(static_cast<int>(OH_Drawing_RectGetHeight(iter)), arr[i][1]);
        EXPECT_EQ(static_cast<int>(OH_Drawing_RectGetTop(iter)), arr[i][2]);
        EXPECT_EQ(static_cast<int>(OH_Drawing_RectGetLeft(iter)), arr[i][3]);
        EXPECT_EQ(OH_Drawing_RectGetBottom(iter) - OH_Drawing_RectGetTop(iter), OH_Drawing_RectGetHeight(iter));
        EXPECT_EQ(OH_Drawing_RectGetRight(iter) - OH_Drawing_RectGetLeft(iter), OH_Drawing_RectGetWidth(iter));
    }
    EXPECT_EQ(OH_Drawing_RectDestroyArray(outRectarr), OH_DRAWING_SUCCESS);

    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: FontGetBounds002
 * @tc.desc: test for space character of glyph ID.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NdkFontTest, FontGetBounds002, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    OH_Drawing_FontSetTextSize(font, 50);
    const char* space = "   ";
    uint32_t count = OH_Drawing_FontCountText(font, space, strlen(space), OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8);
    EXPECT_EQ(strlen(space), count);
    uint16_t glyphs[count];
    OH_Drawing_FontTextToGlyphs(font, space, strlen(space), OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8,
        glyphs, count);
    OH_Drawing_Array *outRectarr = OH_Drawing_RectCreateArray(count);
    ASSERT_NE(outRectarr, nullptr);
    size_t size = 0;
    EXPECT_EQ(OH_Drawing_RectGetArraySize(outRectarr, &size), OH_DRAWING_SUCCESS);
    EXPECT_EQ(size, count);
    EXPECT_EQ(OH_Drawing_FontGetBounds(font, glyphs, count, outRectarr), OH_DRAWING_SUCCESS);
    for (int i = 0; i < count; i++) {
        OH_Drawing_Rect *iter = nullptr;
        EXPECT_EQ(OH_Drawing_RectGetArrayElement(outRectarr, i, &iter), OH_DRAWING_SUCCESS);
        ASSERT_NE(iter, nullptr);
        EXPECT_EQ(OH_Drawing_RectGetWidth(iter), 0);
        EXPECT_EQ(OH_Drawing_RectGetHeight(iter), 0);
    }
    EXPECT_EQ(OH_Drawing_RectDestroyArray(outRectarr), OH_DRAWING_SUCCESS);
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: FontGetBounds003
 * @tc.desc: test for abnormal parameter
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(NdkFontTest, FontGetBounds003, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    OH_Drawing_FontSetTextSize(font, 50);
    const char* space = "   ";
    uint32_t count = OH_Drawing_FontCountText(font, space, strlen(space), OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8);
    EXPECT_EQ(strlen(space), count);
    uint16_t glyphs[count];
    OH_Drawing_FontTextToGlyphs(font, space, strlen(space), OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8,
        glyphs, count);
    OH_Drawing_Array *outRectarr = OH_Drawing_RectCreateArray(count - 1);
    ASSERT_NE(outRectarr, nullptr);
    // not enough size
    EXPECT_EQ(OH_Drawing_FontGetBounds(font, glyphs, count, outRectarr), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_FontGetBounds(nullptr, glyphs, count, outRectarr), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_FontGetBounds(font, nullptr, count, outRectarr), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_FontGetBounds(font, glyphs, count, nullptr), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_RectDestroyArray(outRectarr), OH_DRAWING_SUCCESS);
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: FontGetTextPath001
 * @tc.desc: test for common character of textpath.
 * @tc.type: FUNC
 * @tc.require: IAKP0I
 */
HWTEST_F(NdkFontTest, FontGetTextPath001, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    const char* str = "Hello 中文";
    size_t length = std::char_traits<char>::length(str);
    float x = 12.0f;
    float y = 150.0f;
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    ASSERT_NE(path, nullptr);
    EXPECT_EQ(OH_Drawing_FontGetTextPath(font, str, sizeof(char) * length, TEXT_ENCODING_UTF8, x, y, path),
        OH_DRAWING_SUCCESS);
    ASSERT_NE(path, nullptr);
    EXPECT_TRUE(OH_Drawing_PathGetLength(path, false) > 0);
    if (path != nullptr) {
        OH_Drawing_PathDestroy(path);
    }
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: FontGetTextPath002
 * @tc.desc: test for UTF16 and UTF32 character of textpath.
 * @tc.type: FUNC
 * @tc.require: IAKP0I
 */
HWTEST_F(NdkFontTest, FontGetTextPath002, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);

    float x = 12.0f;
    float y = 150.0f;
    const char16_t* u16str = u"Hello 中文";
    size_t u16strLen = std::char_traits<char16_t>::length(u16str);
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    EXPECT_EQ(OH_Drawing_FontGetTextPath(font, u16str, sizeof(char16_t) * u16strLen, TEXT_ENCODING_UTF16, x, y, path),
        OH_DRAWING_SUCCESS);
    ASSERT_NE(path, nullptr);
    float u16PathLen = OH_Drawing_PathGetLength(path, false);

    const char32_t* u32str = U"Hello 中文";
    size_t u32strLen = std::char_traits<char32_t>::length(u32str);
    EXPECT_EQ(OH_Drawing_FontGetTextPath(font, u32str, sizeof(char32_t) * u32strLen, TEXT_ENCODING_UTF32, x, y, path),
        OH_DRAWING_SUCCESS);
    ASSERT_NE(path, nullptr);
    float u32PathLen = OH_Drawing_PathGetLength(path, false);
    ASSERT_TRUE(u16PathLen > 0 && u32PathLen > 0);
    ASSERT_EQ(u16PathLen, u32PathLen);
    if (path != nullptr) {
        OH_Drawing_PathDestroy(path);
    }
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: FontGetTextPath003
 * @tc.desc: test for space character of textpath.
 * @tc.type: FUNC
 * @tc.require: IAKP0I
 */
HWTEST_F(NdkFontTest, FontGetTextPath003, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    const char* space = " ";
    size_t length = std::char_traits<char>::length(space);
    float x = 12.0f;
    float y = 150.0f;
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    ASSERT_NE(path, nullptr);
    EXPECT_EQ(OH_Drawing_FontGetTextPath(font, space, sizeof(char) * length, TEXT_ENCODING_UTF8, x, y, path),
        OH_DRAWING_SUCCESS);
    ASSERT_NE(path, nullptr);
    EXPECT_TRUE(OH_Drawing_PathGetLength(path, false) == 0);
    EXPECT_FALSE(OH_Drawing_PathIsClosed(path, false));
    if (path != nullptr) {
        OH_Drawing_PathDestroy(path);
    }
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: FontGetTextPath004
 * @tc.desc: test for abnormal paramete of textpath.
 * @tc.type: FUNC
 * @tc.require: IAKP0I
 */
HWTEST_F(NdkFontTest, FontGetTextPath004, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    const char* str = "Hello 中文";
    size_t length = std::char_traits<char>::length(str);
    float x = 12.0f;
    float y = 150.0f;
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    ASSERT_NE(path, nullptr);
    EXPECT_EQ(OH_Drawing_FontGetTextPath(nullptr, str, sizeof(char) * length, TEXT_ENCODING_UTF8, x, y, path),
        OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_FontGetTextPath(font, nullptr, sizeof(char) * length, TEXT_ENCODING_UTF8, x, y, path),
        OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_FontGetTextPath(font, str, 0, TEXT_ENCODING_UTF8, x, y, path),
        OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_FontGetTextPath(font, str, sizeof(char) * length, TEXT_ENCODING_UTF8, x, y, nullptr),
        OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: FontGetTextPath005
 * @tc.desc: test for negative coordinates of textpath.
 * @tc.type: FUNC
 * @tc.require: IAKP0I
 */
HWTEST_F(NdkFontTest, FontGetTextPath005, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    const char* str = "Hello 中文";
    size_t length = std::char_traits<char>::length(str);
    float x = -1.0f;
    float y = -1.0f;
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    ASSERT_NE(path, nullptr);
    EXPECT_EQ(OH_Drawing_FontGetTextPath(font, str, sizeof(char) * length, TEXT_ENCODING_UTF8, x, y, path),
        OH_DRAWING_SUCCESS);
    ASSERT_NE(path, nullptr);
    if (path != nullptr) {
        EXPECT_TRUE(OH_Drawing_PathGetLength(path, false) > 0);
        OH_Drawing_PathDestroy(path);
    }
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: FontGetTextPath006
 * @tc.desc: test for conversion of glyphsID to path.
 * @tc.type: FUNC
 * @tc.require: IAKP0I
 */
HWTEST_F(NdkFontTest, FontGetTextPath006, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    const char* str = "Hello 中文";
    uint32_t count = 0;
    count = OH_Drawing_FontCountText(font, str, strlen(str), TEXT_ENCODING_UTF8);
    EXPECT_NE(count, 0);
    uint16_t glyphs[count];
    OH_Drawing_FontTextToGlyphs(font, str, strlen(str), TEXT_ENCODING_UTF8, glyphs, count);

    float x = 12.0f;
    float y = 150.0f;
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    ASSERT_NE(path, nullptr);
    EXPECT_EQ(OH_Drawing_FontGetTextPath(font, glyphs, sizeof(glyphs), TEXT_ENCODING_GLYPH_ID, x, y, path),
        OH_DRAWING_SUCCESS);
    ASSERT_NE(path, nullptr);
    if (path != nullptr) {
        EXPECT_TRUE(OH_Drawing_PathGetLength(path, false) > 0);
        OH_Drawing_PathDestroy(path);
    }
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: FontThemeFont001
 * @tc.desc: test for theme font.
 * @tc.type: FUNC
 * @tc.require: IAKP0I
 */
HWTEST_F(NdkFontTest, FontThemeFont001, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    bool followed = true;
    OH_Drawing_FontIsThemeFontFollowed(font, &followed);
    EXPECT_EQ(followed, false);
    OH_Drawing_FontSetThemeFontFollowed(font, true);
    OH_Drawing_FontIsThemeFontFollowed(font, &followed);
    EXPECT_EQ(followed, true);
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: FontMeasureTextWithBrushOrPen001
 * @tc.desc: test for FontMeasureTextWithBrushOrPen.
 * @tc.type: FUNC
 * @tc.require: AR20250515745872
 */
HWTEST_F(NdkFontTest, FontMeasureTextWithBrushOrPen001, TestSize.Level1)
{
    OH_Drawing_Font *font = OH_Drawing_FontCreate();
    OH_Drawing_FontSetTextSize(font, 50);
    const char* text = "你好世界";
    float textWidth;
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    EXPECT_EQ(OH_Drawing_FontMeasureTextWithBrushOrPen(nullptr, text, strlen(text),
        TEXT_ENCODING_UTF8, brush, nullptr, nullptr, &textWidth), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_FontMeasureTextWithBrushOrPen(font, nullptr, strlen(text),
        TEXT_ENCODING_UTF8, brush, nullptr, nullptr, &textWidth), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_FontMeasureTextWithBrushOrPen(font, text, 0,
        TEXT_ENCODING_UTF8, brush, nullptr, nullptr, &textWidth), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_FontMeasureTextWithBrushOrPen(nullptr, text, strlen(text),
        TEXT_ENCODING_UTF8, brush, pen, nullptr, &textWidth), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_ErrorCode errorCode = OH_Drawing_FontMeasureTextWithBrushOrPen(font, text, strlen(text),
        static_cast<OH_Drawing_TextEncoding>(10), nullptr, nullptr, nullptr, &textWidth);
    EXPECT_EQ(errorCode, OH_DRAWING_SUCCESS);
    EXPECT_EQ(static_cast<int>(textWidth), 0);

    errorCode = OH_Drawing_FontMeasureTextWithBrushOrPen(font, text, strlen(text),
        TEXT_ENCODING_UTF8, nullptr, nullptr, nullptr, &textWidth);
    EXPECT_EQ(errorCode, OH_DRAWING_SUCCESS);
    EXPECT_EQ(static_cast<int>(textWidth), 200);

    const char* text1 = "hello world";
    errorCode = OH_Drawing_FontMeasureTextWithBrushOrPen(font, text1, strlen(text),
        TEXT_ENCODING_UTF8, brush, nullptr, nullptr, &textWidth);
    EXPECT_EQ(errorCode, OH_DRAWING_SUCCESS);
    EXPECT_EQ(static_cast<int>(textWidth), 304);

    OH_Drawing_Rect* bounds = OH_Drawing_RectCreate(0, 0, 200, 200);
    errorCode = OH_Drawing_FontMeasureTextWithBrushOrPen(font, text1, strlen(text),
        TEXT_ENCODING_UTF8, brush, nullptr, bounds, &textWidth);
    EXPECT_EQ(errorCode, OH_DRAWING_SUCCESS);
    EXPECT_EQ(static_cast<int>(textWidth), 304);
    EXPECT_EQ(static_cast<int>(OH_Drawing_RectGetLeft(bounds)), 3);
    EXPECT_EQ(static_cast<int>(OH_Drawing_RectGetRight(bounds)), 251);
    EXPECT_EQ(static_cast<int>(OH_Drawing_RectGetRight(bounds)), 251);
    EXPECT_EQ(static_cast<int>(OH_Drawing_RectGetBottom(bounds)), 1);

    const char* text2 = "1234567890 !@#$%^&*(";
    errorCode = OH_Drawing_FontMeasureTextWithBrushOrPen(font, text2, strlen(text2),
        TEXT_ENCODING_UTF8, nullptr, pen, nullptr, &textWidth);
    EXPECT_EQ(errorCode, OH_DRAWING_SUCCESS);
    EXPECT_EQ(static_cast<int>(textWidth), 555);
    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_PenDestroy(pen);
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: FontGetWidthsBounds001
 * @tc.desc: test for FontMeasureTextWithBrushOrPen.
 * @tc.type: FUNC
 * @tc.require: AR20250515745872
 */
HWTEST_F(NdkFontTest, OH_Drawing_FontGetWidthsBounds001, TestSize.Level1)
{
    OH_Drawing_Font *font = OH_Drawing_FontCreate();
    OH_Drawing_FontSetTextSize(font, 50);
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    const char* text = "你好世界";
    uint32_t count = 0;
    count = OH_Drawing_FontCountText(font, text, strlen(text), TEXT_ENCODING_UTF8);
    uint16_t glyphs[count];
    OH_Drawing_FontTextToGlyphs(font, text, strlen(text), TEXT_ENCODING_UTF8, glyphs, count);
    int glyphsCount = 0;
    glyphsCount = OH_Drawing_FontTextToGlyphs(
        font, text, strlen(text), OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8, glyphs, count);
    // paramter font is nullptr
    float widths[50] = {0.f}; // 50 means widths array number
    OH_Drawing_Array *outRectarr = OH_Drawing_RectCreateArray(count);
    EXPECT_EQ(OH_Drawing_FontGetWidthsBounds(nullptr, glyphs, glyphsCount, brush, pen, widths, outRectarr),
        OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_FontGetWidthsBounds(font, nullptr, glyphsCount, brush, pen, widths, outRectarr),
        OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_FontGetWidthsBounds(font, glyphs, 0, brush, pen, widths, outRectarr),
        OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_FontGetWidthsBounds(font, glyphs, 0, brush, pen, widths, outRectarr),
        OH_DRAWING_ERROR_INVALID_PARAMETER);

    std::vector<int> widthArr = {50, 50, 50, 50};
    OH_Drawing_ErrorCode errorCode = OH_Drawing_FontGetWidthsBounds(
        font, glyphs, glyphsCount, brush, nullptr, widths, nullptr);
    EXPECT_EQ(errorCode, OH_DRAWING_SUCCESS);
    for (int i = 0; i < count; i++) {
        EXPECT_EQ(static_cast<int>(widths[i]), widthArr[i]);
    }

    errorCode = OH_Drawing_FontGetWidthsBounds(
        font, glyphs, glyphsCount, nullptr, pen, nullptr, outRectarr);
    EXPECT_EQ(errorCode, OH_DRAWING_SUCCESS);

    std::vector<std::array<int, 4>> arr = { { 0, -42, 48, 3 }, { 1, -42, 48, 3 }, { 2, -42, 47, 2 }, { 1, -40, 48, 3 }};
    for (int i = 0; i < count; i++) {
        OH_Drawing_Rect* iter = nullptr;
        EXPECT_EQ(OH_Drawing_RectGetArrayElement(outRectarr, i, &iter), OH_DRAWING_SUCCESS);
        ASSERT_NE(iter, nullptr);
        EXPECT_EQ(static_cast<int>(OH_Drawing_RectGetLeft(iter)), arr[i][0]);
        EXPECT_EQ(static_cast<int>(OH_Drawing_RectGetTop(iter)), arr[i][1]);
        EXPECT_EQ(static_cast<int>(OH_Drawing_RectGetRight(iter)), arr[i][2]);
        EXPECT_EQ(static_cast<int>(OH_Drawing_RectGetBottom(iter)), arr[i][3]);
        EXPECT_EQ(OH_Drawing_RectGetBottom(iter) - OH_Drawing_RectGetTop(iter), OH_Drawing_RectGetHeight(iter));
        EXPECT_EQ(OH_Drawing_RectGetRight(iter) - OH_Drawing_RectGetLeft(iter), OH_Drawing_RectGetWidth(iter));
    }
    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_PenDestroy(pen);
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: FontGetPos001
 * @tc.desc: test for FontGetPos.
 * @tc.type: FUNC
 * @tc.require: AR20250515745872
 */
HWTEST_F(NdkFontTest, OH_Drawing_FontGetPos001, TestSize.Level1)
{
    OH_Drawing_Font *font = OH_Drawing_FontCreate();
    OH_Drawing_FontSetTextSize(font, 50);
    const char* text = "你好世界";
    uint32_t count = 0;
    count = OH_Drawing_FontCountText(font, text, strlen(text), TEXT_ENCODING_UTF8);
    OH_Drawing_Point *point = OH_Drawing_PointCreate(10.0, 10.0);
    uint16_t glyphs[count];
    OH_Drawing_FontTextToGlyphs(font, text, strlen(text), TEXT_ENCODING_UTF8, glyphs, count);
    int glyphsCount = 0;
    glyphsCount = OH_Drawing_FontTextToGlyphs(
        font, text, strlen(text), OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8, glyphs, count);
    // paramter font is nullptr
    OH_Drawing_Point2D* points = new OH_Drawing_Point2D[count];
    EXPECT_EQ(OH_Drawing_FontGetPos(nullptr, glyphs, glyphsCount, point, points), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_FontGetPos(font, nullptr, glyphsCount, point, points), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_FontGetPos(font, glyphs, 0, point, points), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_FontGetPos(font, glyphs, glyphsCount, nullptr, nullptr), OH_DRAWING_ERROR_INVALID_PARAMETER);

    std::vector<std::array<int, 2>> testPoints = {{10, 10}, {60, 10}, {110, 10}, {160, 10}};
    OH_Drawing_ErrorCode errorCode = OH_Drawing_FontGetPos(font, glyphs, glyphsCount, point, points);
    EXPECT_EQ(errorCode, OH_DRAWING_SUCCESS);
    for (int i = 0; i < count; i++) {
        EXPECT_EQ(static_cast<int>(points[i].x), testPoints[i][0]);
        EXPECT_EQ(static_cast<int>(points[i].y), testPoints[i][1]);
    }
    OH_Drawing_PointDestroy(point);
    if (points != nullptr) {
        delete[] points;
    }
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: FontGetSpacing001
 * @tc.desc: test for FontGetSpacing.
 * @tc.type: FUNC
 * @tc.require: AR20250515745872
 */
HWTEST_F(NdkFontTest, OH_Drawing_FontGetSpacing001, TestSize.Level1)
{
    OH_Drawing_Font *font = OH_Drawing_FontCreate();
    OH_Drawing_FontSetTextSize(font, 30);
    float spacing = 0.0f;
    EXPECT_EQ(OH_Drawing_FontGetSpacing(nullptr, &spacing), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_FontGetSpacing(font, nullptr), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_ErrorCode errorCode = OH_Drawing_FontGetSpacing(font, &spacing);
    EXPECT_EQ(errorCode, OH_DRAWING_SUCCESS);
    EXPECT_EQ(std::fabs(spacing - 35.16) < 1e-6, true);
    OH_Drawing_FontDestroy(font);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS