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

#include "drawing_color.h"
#include "drawing_color_filter.h"
#include "drawing_filter.h"
#include "drawing_pen.h"
#include "effect/color_filter.h"
#include "effect/filter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class NativeDrawingPenTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void NativeDrawingPenTest::SetUpTestCase() {}
void NativeDrawingPenTest::TearDownTestCase() {}
void NativeDrawingPenTest::SetUp() {}
void NativeDrawingPenTest::TearDown() {}

static Filter* CastToFilter(OH_Drawing_Filter* cFilter)
{
    return reinterpret_cast<Filter*>(cFilter);
}

/*
 * @tc.name: NativeDrawingPenTest_penCreate001
 * @tc.desc: test for create drawing_pen.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_penCreate001, TestSize.Level1)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    EXPECT_EQ(pen == nullptr, false);
    OH_Drawing_PenDestroy(pen);
}

/*
 * @tc.name: NativeDrawingPenTest_penSetAntiAlias002
 * @tc.desc: test for the get and set methods about AntiAlias for a pen.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_penSetAntiAlias002, TestSize.Level1)
{
    OH_Drawing_Pen* pen1 = OH_Drawing_PenCreate();
    OH_Drawing_PenSetAntiAlias(pen1, true);
    EXPECT_EQ(OH_Drawing_PenIsAntiAlias(pen1), true);
    OH_Drawing_PenSetAntiAlias(pen1, false);
    EXPECT_EQ(OH_Drawing_PenIsAntiAlias(pen1), false);
    OH_Drawing_PenDestroy(pen1);
}

/*
 * @tc.name: NativeDrawingPenTest_penSetColor003
 * @tc.desc: test for the get and set methods about the color for a pen.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_penSetColor003, TestSize.Level1)
{
    OH_Drawing_Pen* pen2 = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen2, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0x00));
    EXPECT_EQ(OH_Drawing_PenGetColor(pen2), 0xFFFF0000);
    OH_Drawing_PenDestroy(pen2);
}

/*
 * @tc.name: NativeDrawingPenTest_penSetWidth004
 * @tc.desc: test for the get and set methods about the width for a pen.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_penSetWidth004, TestSize.Level1)
{
    OH_Drawing_Pen* pen3 = OH_Drawing_PenCreate();
    OH_Drawing_PenSetWidth(pen3, 10);
    EXPECT_EQ(OH_Drawing_PenGetWidth(pen3), 10);
    OH_Drawing_PenDestroy(pen3);
}

/*
 * @tc.name: NativeDrawingPenTest_penSetMiterLimit005
 * @tc.desc: test for the get and set methods about the miterLimit for a pen.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_penSetMiterLimit005, TestSize.Level1)
{
    OH_Drawing_Pen* pen4 = OH_Drawing_PenCreate();
    OH_Drawing_PenSetMiterLimit(pen4, 5);
    EXPECT_EQ(OH_Drawing_PenGetMiterLimit(pen4), 5);
    OH_Drawing_PenDestroy(pen4);
}

/*
 * @tc.name: NativeDrawingPenTest_penSetCap006
 * @tc.desc: test for the get and set methods about the line cap style for a pen.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_penSetCap006, TestSize.Level1)
{
    OH_Drawing_Pen* pen5 = OH_Drawing_PenCreate();
    OH_Drawing_PenSetCap(pen5, OH_Drawing_PenLineCapStyle::LINE_SQUARE_CAP);
    EXPECT_EQ(OH_Drawing_PenGetCap(pen5), OH_Drawing_PenLineCapStyle::LINE_SQUARE_CAP);
    OH_Drawing_PenSetCap(pen5, OH_Drawing_PenLineCapStyle::LINE_FLAT_CAP);
    EXPECT_EQ(OH_Drawing_PenGetCap(pen5), OH_Drawing_PenLineCapStyle::LINE_FLAT_CAP);
    OH_Drawing_PenSetCap(pen5, OH_Drawing_PenLineCapStyle::LINE_ROUND_CAP);
    EXPECT_EQ(OH_Drawing_PenGetCap(pen5), OH_Drawing_PenLineCapStyle::LINE_ROUND_CAP);
    OH_Drawing_PenDestroy(pen5);
}

/*
 * @tc.name: NativeDrawingPenTest_penSetJoin007
 * @tc.desc: test for the get and set methods about the line join style for a pen.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_penSetJoin007, TestSize.Level1)
{
    OH_Drawing_Pen* pen6 = OH_Drawing_PenCreate();
    OH_Drawing_PenSetJoin(pen6, OH_Drawing_PenLineJoinStyle::LINE_ROUND_JOIN);
    EXPECT_EQ(OH_Drawing_PenGetJoin(pen6), OH_Drawing_PenLineJoinStyle::LINE_ROUND_JOIN);
    OH_Drawing_PenSetJoin(pen6, OH_Drawing_PenLineJoinStyle::LINE_MITER_JOIN);
    EXPECT_EQ(OH_Drawing_PenGetJoin(pen6), OH_Drawing_PenLineJoinStyle::LINE_MITER_JOIN);
    OH_Drawing_PenSetJoin(pen6, OH_Drawing_PenLineJoinStyle::LINE_BEVEL_JOIN);
    EXPECT_EQ(OH_Drawing_PenGetJoin(pen6), OH_Drawing_PenLineJoinStyle::LINE_BEVEL_JOIN);
    OH_Drawing_PenDestroy(pen6);
}

/*
 * @tc.name: NativeDrawingPenTest_penSetBlendMode008
 * @tc.desc: test for the get and set methods about the line join style for a pen.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_penSetBlendMode008, TestSize.Level1)
{
    OH_Drawing_Pen* pen8 = OH_Drawing_PenCreate();
    EXPECT_NE(pen8, nullptr);
    OH_Drawing_PenSetBlendMode(pen8, OH_Drawing_BlendMode::BLEND_MODE_SRC);
    OH_Drawing_PenSetBlendMode(nullptr, OH_Drawing_BlendMode::BLEND_MODE_SRC);
    OH_Drawing_PenDestroy(pen8);
}

/*
 * @tc.name: NativeDrawingPenTest_penReset009
 * @tc.desc: test for the reset method for a pen.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_penReset009, TestSize.Level1)
{
    OH_Drawing_Pen* pen9 = OH_Drawing_PenCreate();
    OH_Drawing_PenSetAntiAlias(pen9, true);
    OH_Drawing_PenSetColor(pen9, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0x00));
    OH_Drawing_PenSetWidth(pen9, 10);
    OH_Drawing_PenSetMiterLimit(pen9, 5);
    OH_Drawing_PenSetCap(pen9, OH_Drawing_PenLineCapStyle::LINE_ROUND_CAP);
    OH_Drawing_PenSetJoin(pen9, OH_Drawing_PenLineJoinStyle::LINE_BEVEL_JOIN);

    OH_Drawing_PenReset(pen9);
    EXPECT_EQ(OH_Drawing_PenIsAntiAlias(pen9), false);
    EXPECT_EQ(OH_Drawing_PenGetColor(pen9), 0xFF000000);
    EXPECT_EQ(OH_Drawing_PenGetWidth(pen9), 0);
    EXPECT_EQ(OH_Drawing_PenGetMiterLimit(pen9), -1);
    EXPECT_EQ(OH_Drawing_PenGetCap(pen9), OH_Drawing_PenLineCapStyle::LINE_FLAT_CAP);
    EXPECT_EQ(OH_Drawing_PenGetJoin(pen9), OH_Drawing_PenLineJoinStyle::LINE_MITER_JOIN);

    OH_Drawing_PenDestroy(pen9);
}

/*
 * @tc.name: NativeDrawingPenTest_penGetFilter010
 * @tc.desc: gets the filter from a pen.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_penGetFilter010, TestSize.Level1)
{
    OH_Drawing_Pen* pen9 = OH_Drawing_PenCreate();
    EXPECT_NE(pen9, nullptr);
    OH_Drawing_Filter* cFilter_ = OH_Drawing_FilterCreate();
    EXPECT_NE(cFilter_, nullptr);

    OH_Drawing_ColorFilter* colorFilterTmp = OH_Drawing_ColorFilterCreateLinearToSrgbGamma();
    OH_Drawing_FilterSetColorFilter(cFilter_, nullptr);
    OH_Drawing_FilterGetColorFilter(cFilter_, colorFilterTmp);
    EXPECT_EQ((reinterpret_cast<ColorFilter*>(colorFilterTmp))->GetType(),
        ColorFilter::FilterType::NO_TYPE);

    OH_Drawing_Filter* tmpFilter_ = OH_Drawing_FilterCreate();
    EXPECT_NE(cFilter_, nullptr);
    EXPECT_NE(tmpFilter_, nullptr);
    OH_Drawing_ColorFilter* cColorFilter_ = OH_Drawing_ColorFilterCreateBlendMode(0xFF0000FF, BLEND_MODE_COLOR);
    OH_Drawing_FilterSetColorFilter(cFilter_, cColorFilter_);
    OH_Drawing_PenSetFilter(pen9, cFilter_);
    OH_Drawing_PenGetFilter(pen9, tmpFilter_);

    EXPECT_NE(CastToFilter(tmpFilter_)->GetColorFilter(), nullptr);
    EXPECT_EQ(CastToFilter(tmpFilter_)->GetColorFilter()->GetType(), ColorFilter::FilterType::BLEND_MODE);
    OH_Drawing_FilterDestroy(cFilter_);
    OH_Drawing_FilterDestroy(tmpFilter_);
    OH_Drawing_ColorFilterDestroy(cColorFilter_);
    OH_Drawing_ColorFilterDestroy(colorFilterTmp);
    OH_Drawing_PenDestroy(pen9);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS