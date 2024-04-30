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
#include "drawing_font.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class NativeFontTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void NativeFontTest::SetUpTestCase() {}
void NativeFontTest::TearDownTestCase() {}
void NativeFontTest::SetUp() {}
void NativeFontTest::TearDown() {}

/*
 * @tc.name: NativeFontTest_GetMetrics001
 * @tc.desc: test for GetMetrics.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeFontTest, NativeFontTest_GetMetrics001, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    OH_Drawing_Font_Metrics cFontMetrics;
    EXPECT_TRUE(OH_Drawing_FontGetMetrics(font, &cFontMetrics) >= 0);
    EXPECT_TRUE(OH_Drawing_FontGetMetrics(font, nullptr) < 0);
    EXPECT_TRUE(OH_Drawing_FontGetMetrics(nullptr, nullptr) < 0);
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: NativeFontTest_IsAndSetBaselineSnap002
 * @tc.desc: test for SetBaselineSnap and IsBaselineSnap.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeFontTest, NativeFontTest_IsAndSetBaselineSnap002, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    OH_Drawing_FontSetBaselineSnap(nullptr, true);
    EXPECT_EQ(OH_Drawing_FontIsBaselineSnap(nullptr), false);
    OH_Drawing_FontSetBaselineSnap(nullptr, false);
    EXPECT_EQ(OH_Drawing_FontIsBaselineSnap(nullptr), false);
    OH_Drawing_FontSetBaselineSnap(font, true);
    EXPECT_EQ(OH_Drawing_FontIsBaselineSnap(font), true);
    OH_Drawing_FontSetBaselineSnap(font, false);
    EXPECT_EQ(OH_Drawing_FontIsBaselineSnap(font), false);
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: NativeFontTest_IsAndSetSubpixel003
 * @tc.desc: test for SetSubpixel and IsSubpixel.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeFontTest, NativeFontTest_IsAndSetSubpixel003, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    OH_Drawing_FontSetSubpixel(nullptr, false);
    EXPECT_EQ(OH_Drawing_FontIsSubpixel(nullptr), false);
    OH_Drawing_FontSetSubpixel(nullptr, true);
    EXPECT_EQ(OH_Drawing_FontIsSubpixel(nullptr), false);
    OH_Drawing_FontSetSubpixel(font, true);
    EXPECT_EQ(OH_Drawing_FontIsSubpixel(font), true);
    OH_Drawing_FontSetSubpixel(font, false);
    EXPECT_EQ(OH_Drawing_FontIsSubpixel(font), false);
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: NativeFontTest_TextToGlyphs004
 * @tc.desc: test for TextToGlyphs.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeFontTest, NativeFontTest_TextToGlyphs004, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    OH_Drawing_FontSetTextSize(font, 100); // 100 means font text size
    EXPECT_NE(font, nullptr);
    const char *str = "hello world";
    uint32_t count = 0;
    count = OH_Drawing_FontCountText(font, str, strlen(str), OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8);
    EXPECT_EQ(11, count); // 11 means str length

    uint16_t glyphs[50] = {0}; // 50 means glyphs array number
    int glyphsCount = 0;
    glyphsCount = OH_Drawing_FontTextToGlyphs(font, str, 0,
        OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8, glyphs, 0);
    EXPECT_EQ(0, glyphsCount);

    glyphsCount = OH_Drawing_FontTextToGlyphs(font, str, strlen(str),
        OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8, glyphs, count);
    EXPECT_EQ(11, glyphsCount); // 11 means glyphsCount
    EXPECT_EQ(257, glyphs[0]); // 257 means glyphs[0] value

    float widths[50] = {0.f}; // 50 means widths array number
    OH_Drawing_FontGetWidths(font, glyphs, glyphsCount, widths);
    EXPECT_EQ(58.0, widths[0]); // 58.0 means glyphs[0] width
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: NativeFontTest_SetAndGetScaleX005
 * @tc.desc: test for SetAndGetScaleX.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeFontTest, NativeFontTest_SetAndGetScaleX005, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    OH_Drawing_FontSetScaleX(nullptr, 2);
    EXPECT_TRUE(OH_Drawing_FontGetScaleX(nullptr) == -1);
    EXPECT_TRUE(OH_Drawing_FontGetScaleX(font) == 1);
    OH_Drawing_FontSetScaleX(font, 2);
    EXPECT_TRUE(OH_Drawing_FontGetScaleX(font) == 2);
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: NativeFontTest_GetAndSetEdging006
 * @tc.desc: test for GetAndSetEdging.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeFontTest, NativeFontTest_GetAndSetEdging006, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    EXPECT_EQ(OH_Drawing_FontGetEdging(font), OH_Drawing_FontEdging::FONT_EDGING_ANTI_ALIAS);
    EXPECT_EQ(OH_Drawing_FontGetEdging(nullptr), OH_Drawing_FontEdging::FONT_EDGING_ALIAS);
    OH_Drawing_FontSetEdging(nullptr, OH_Drawing_FontEdging::FONT_EDGING_ALIAS);
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
 * @tc.name: NativeFontTest_GetAndSetForceAutoHinting007
 * @tc.desc: test for GetAndSetForceAutoHinting.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeFontTest, NativeFontTest_GetAndSetForceAutoHinting007, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    EXPECT_EQ(OH_Drawing_FontIsForceAutoHinting(nullptr), false);
    OH_Drawing_FontSetForceAutoHinting(nullptr, true);
    EXPECT_EQ(OH_Drawing_FontIsForceAutoHinting(font), false);
    OH_Drawing_FontSetForceAutoHinting(font, true);
    EXPECT_EQ(OH_Drawing_FontIsForceAutoHinting(font), true);
    OH_Drawing_FontSetForceAutoHinting(font, false);
    EXPECT_EQ(OH_Drawing_FontIsForceAutoHinting(font), false);
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: NativeFontTest_GetAndSetHinting008
 * @tc.desc: test for GetHinting and SetHinting.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeFontTest, NativeFontTest_GetAndSetHinting008, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    EXPECT_TRUE(OH_Drawing_FontGetHinting(nullptr) == OH_Drawing_FontHinting::FONT_HINTING_NONE);
    OH_Drawing_FontSetHinting(font, OH_Drawing_FontHinting::FONT_HINTING_NONE);
    EXPECT_TRUE(OH_Drawing_FontGetHinting(font) == OH_Drawing_FontHinting::FONT_HINTING_NONE);
    OH_Drawing_FontSetHinting(font, OH_Drawing_FontHinting::FONT_HINTING_SLIGHT);
    EXPECT_TRUE(OH_Drawing_FontGetHinting(font) == OH_Drawing_FontHinting::FONT_HINTING_SLIGHT);
    OH_Drawing_FontSetHinting(font, OH_Drawing_FontHinting::FONT_HINTING_SLIGHT);
    EXPECT_TRUE(OH_Drawing_FontGetHinting(font) == OH_Drawing_FontHinting::FONT_HINTING_SLIGHT);
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: NativeFontTest_GetAndSetEmbeddedBitmaps009
 * @tc.desc: test for GetEmbeddedBitmaps and SetEmbeddedBitmaps.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeFontTest, NativeFontTest_GetAndSetEmbeddedBitmaps009, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    EXPECT_TRUE(OH_Drawing_FontIsEmbeddedBitmaps(nullptr) == false);
    OH_Drawing_FontSetEmbeddedBitmaps(font, true);
    EXPECT_TRUE(OH_Drawing_FontIsEmbeddedBitmaps(font) == true);
    OH_Drawing_FontSetEmbeddedBitmaps(font, false);
    EXPECT_TRUE(OH_Drawing_FontIsEmbeddedBitmaps(font) == false);
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: NativeFontTest_GetTextSize010
 * @tc.desc: test for GetTextSize.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeFontTest, NativeFontTest_GetTextSize010, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    OH_Drawing_FontSetTextSize(font, 100);
    float size = OH_Drawing_FontGetTextSize(font);
    EXPECT_EQ(size, 100);
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: NativeFontTest_GetTextSkewX011
 * @tc.desc: test for GetTextSkewX.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeFontTest, NativeFontTest_GetTextSkewX011, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    OH_Drawing_FontSetTextSkewX(font, 10);
    float size = OH_Drawing_FontGetTextSkewX(font);
    EXPECT_EQ(size, 10);
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: NativeFontTest_IsLinearText012
 * @tc.desc: test for IsLinearText.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeFontTest, NativeFontTest_IsLinearText012, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    bool ret = OH_Drawing_FontIsLinearText(font);
    EXPECT_EQ(ret, false);
    OH_Drawing_FontSetLinearText(font, true);
    ret = OH_Drawing_FontIsLinearText(font);
    EXPECT_EQ(ret, true);
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: NativeFontTest_SetFakeBoldText013
 * @tc.desc: test for SetFakeBoldText.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeFontTest, NativeFontTest_SetFakeBoldText013, TestSize.Level1)
{
    OH_Drawing_Font* font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    bool ret = OH_Drawing_FontIsFakeBoldText(font);
    EXPECT_EQ(ret, false);
    OH_Drawing_FontSetFakeBoldText(font, true);
    ret = OH_Drawing_FontIsFakeBoldText(font);
    EXPECT_EQ(ret, true);
    OH_Drawing_FontDestroy(font);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS