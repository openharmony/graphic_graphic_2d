/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

#include "drawing_color.h"
#include "drawing_color_filter.h"
#include "drawing_error_code.h"
#include "drawing_filter.h"
#include "drawing_pen.h"
#include "drawing_point.h"
#include "drawing_shader_effect.h"
#include "drawing_shadow_layer.h"
#include "drawing_types.h"
#include "gtest/gtest.h"

#include "effect/color_filter.h"
#include "effect/filter.h"
#include "draw/pen.h"
#include "drawing_helper.h"
#include "drawing_path.h"
#include "drawing_rect.h"
#include "drawing_matrix.h"
#include "native_color_space_manager.h"


#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif

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

void NativeDrawingPenTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
}
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
    EXPECT_EQ(OH_Drawing_PenGetCap(nullptr), OH_Drawing_PenLineCapStyle::LINE_FLAT_CAP);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
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
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_PenSetBlendMode(pen8, (OH_Drawing_BlendMode)(BLEND_MODE_LUMINOSITY + 1));
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE);
    OH_Drawing_PenSetBlendMode(pen8, (OH_Drawing_BlendMode)(BLEND_MODE_CLEAR - 1));
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE);
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
    NativeHandle<ColorFilter>* colorFilterHandle = Helper::CastTo<OH_Drawing_ColorFilter*,
        NativeHandle<ColorFilter>*>(colorFilterTmp);
    EXPECT_NE(colorFilterHandle, nullptr);
    EXPECT_NE(colorFilterHandle->value, nullptr);
    EXPECT_EQ(colorFilterHandle->value->GetType(), ColorFilter::FilterType::NO_TYPE);

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

/*
 * @tc.name: NativeDrawingPenTest_PenSetShadowLayer011
 * @tc.desc: test for the set methods of pen.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_PenSetShadowLayer011, TestSize.Level1)
{
    // blurRadius:3.f, offset:(-3.f, 3.f), shadowColor:green
    OH_Drawing_ShadowLayer* shadowLayer = OH_Drawing_ShadowLayerCreate(3.f, -3.f, 3.f, 0xFF00FF00);
    EXPECT_NE(shadowLayer, nullptr);
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetShadowLayer(nullptr, shadowLayer);
    OH_Drawing_PenSetShadowLayer(pen, nullptr);
    OH_Drawing_PenSetShadowLayer(pen, shadowLayer);
    OH_Drawing_ShadowLayerDestroy(shadowLayer);
    OH_Drawing_PenDestroy(pen);
}

/*
 * @tc.name: NativeDrawingPenTest_PenCopy012
 * @tc.desc: test for creates an <b>OH_Drawing_Pen</b> copy object.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_PenCopy012, TestSize.Level1)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    EXPECT_NE(OH_Drawing_PenCopy(pen), nullptr);
    pen = nullptr;
    EXPECT_EQ(OH_Drawing_PenCopy(pen), nullptr);
    OH_Drawing_PenDestroy(pen);
}

/*
 * @tc.name: NativeDrawingPenTest_PenGetFillPath013
 * @tc.desc: test for gets the filled equivalent of the src path.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_PenGetFillPath013, TestSize.Level1)
{
    OH_Drawing_Pen *pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetWidth(pen, 10); // 10: width of pen
    OH_Drawing_Path *dst = OH_Drawing_PathCreate();
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(10, 10, 20, 20); // rect left[10], top[10], right[20], bottom[20]
    OH_Drawing_Matrix *matrix = OH_Drawing_MatrixCreate();
    EXPECT_FALSE(OH_Drawing_PenGetFillPath(pen, nullptr, dst, rect, matrix));

    OH_Drawing_Path *src = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(src, 10, 10); // 10: point's x, 10 point's y
    OH_Drawing_PathLineTo(src, 20, 20); // 20: point's x, 20 point's y
    EXPECT_TRUE(OH_Drawing_PenGetFillPath(pen, src, dst, rect, matrix));

    OH_Drawing_PenDestroy(pen);
    OH_Drawing_PathDestroy(src);
    OH_Drawing_PathDestroy(dst);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_MatrixDestroy(matrix);
}

/*
 * @tc.name: NativeDrawingPenTest_PenIsAntiAlias
 * @tc.desc: test for OH_Drawing_PenIsAntiAlias.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_PenIsAntiAlias, TestSize.Level1)
{
    ASSERT_TRUE(OH_Drawing_PenIsAntiAlias(nullptr) == false);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
}

/*
 * @tc.name: NativeDrawingPenTest_PenSetAntiAlias
 * @tc.desc: test for OH_Drawing_PenSetAntiAlias.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_PenSetAntiAlias, TestSize.Level1)
{
    OH_Drawing_PenSetAntiAlias(nullptr, false);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
}

/*
 * @tc.name: NativeDrawingPenTest_PenGetColor
 * @tc.desc: test for OH_Drawing_PenGetColor.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_PenGetColor, TestSize.Level1)
{
    ASSERT_TRUE(OH_Drawing_PenGetColor(nullptr) == 0);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
}

/*
 * @tc.name: NativeDrawingPenTest_PenSetColor
 * @tc.desc: test for OH_Drawing_PenSetColor.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_PenSetColor, TestSize.Level1)
{
    OH_Drawing_PenSetColor(nullptr, 0xFFFFFFFF);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
}

/*
 * @tc.name: NativeDrawingPenTest_PenGetAlpha
 * @tc.desc: test for OH_Drawing_PenGetAlpha.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_PenGetAlpha, TestSize.Level1)
{
    ASSERT_TRUE(OH_Drawing_PenGetAlpha(nullptr) == 0);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
}

/*
 * @tc.name: NativeDrawingPenTest_PenGetAlpha002
 * @tc.desc: test for OH_Drawing_PenGetAlpha.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_PenGetAlpha002, TestSize.Level1)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetAlpha(pen, 0xFF);
    ASSERT_TRUE(OH_Drawing_PenGetAlpha(pen) == 0xFF);
    OH_Drawing_PenDestroy(pen);
}

/*
 * @tc.name: NativeDrawingPenTest_PenSetAlpha
 * @tc.desc: test for OH_Drawing_PenSetAlpha.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_PenSetAlpha, TestSize.Level1)
{
    OH_Drawing_PenSetAlpha(nullptr, 0xFF);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
}

/*
 * @tc.name: NativeDrawingPenTest_PenSetAlpha
 * @tc.desc: test for OH_Drawing_PenSetAlpha.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_PenSetAlpha002, TestSize.Level1)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetAlpha(pen, 0xFF);
    EXPECT_EQ(OH_Drawing_PenGetAlpha(pen), 0xFF);
    OH_Drawing_PenDestroy(pen);
}

/*
 * @tc.name: NativeDrawingPenTest_PenGetWidth
 * @tc.desc: test for OH_Drawing_PenGetWidth.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_PenGetWidth, TestSize.Level1)
{
    EXPECT_EQ(OH_Drawing_PenGetWidth(nullptr), 0);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
}

/*
 * @tc.name: NativeDrawingPenTest_PenSetWidth
 * @tc.desc: test for OH_Drawing_PenSetWidth.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_PenSetWidth, TestSize.Level1)
{
    OH_Drawing_PenSetWidth(nullptr, 10);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
}

/*
 * @tc.name: NativeDrawingPenTest_PenGetMiterLimit
 * @tc.desc: test for OH_Drawing_PenGetMiterLimit.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_PenGetMiterLimit, TestSize.Level1)
{
    OH_Drawing_PenGetMiterLimit(nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
}

/*
 * @tc.name: NativeDrawingPenTest_PenSetMiterLimit
 * @tc.desc: test for OH_Drawing_PenSetMiterLimit.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_PenSetMiterLimit, TestSize.Level1)
{
    OH_Drawing_PenSetMiterLimit(nullptr, 0);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
}

/*
 * @tc.name: NativeDrawingPenTest_PenSetCap
 * @tc.desc: test for OH_Drawing_PenSetCap.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_PenSetCap, TestSize.Level1)
{
    OH_Drawing_PenSetCap(nullptr, OH_Drawing_PenLineCapStyle::LINE_FLAT_CAP);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
}

/*
 * @tc.name: NativeDrawingPenTest_PenGetJoin001
 * @tc.desc: test for OH_Drawing_PenGetJoin.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_PenGetJoin001, TestSize.Level1)
{
    ASSERT_TRUE(OH_Drawing_PenGetJoin(nullptr) == OH_Drawing_PenLineJoinStyle::LINE_MITER_JOIN);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
}

/*
 * @tc.name: NativeDrawingPenTest_PenSetJoin001
 * @tc.desc: test for OH_Drawing_PenSetJoin.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_PenSetJoin001, TestSize.Level1)
{
    OH_Drawing_PenSetJoin(nullptr, OH_Drawing_PenLineJoinStyle::LINE_MITER_JOIN);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
}

/*
 * @tc.name: NativeDrawingPenTest_PenSetShaderEffect001
 * @tc.desc: test for OH_Drawing_PenSetShaderEffect.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_PenSetShaderEffect001, TestSize.Level1)
{
    OH_Drawing_PenSetShaderEffect(nullptr, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
}

/*
 * @tc.name: NativeDrawingPenTest_PenSetShaderEffect002
 * @tc.desc: test for OH_Drawing_PenSetShaderEffect.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_PenSetShaderEffect002, TestSize.Level1)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_Point* point = OH_Drawing_PointCreate(100, 100); // point 100,100
    uint32_t colors[] = { 0xFFFF0000, 0xFF00FF00, 0xFF0000FF };
    float pos[] = { 0, 0.5, 1.0 };
    OH_Drawing_ShaderEffect* effect =
        OH_Drawing_ShaderEffectCreateRadialGradient(point, 100, colors, pos, 3, OH_Drawing_TileMode::CLAMP);
    ASSERT_FALSE(effect == nullptr);
    OH_Drawing_PenSetShaderEffect(pen, effect);
    OH_Drawing_PointDestroy(point);
    OH_Drawing_ShaderEffectDestroy(effect);
    OH_Drawing_PenDestroy(pen);
}

/*
 * @tc.name: NativeDrawingPenTest_PenSetShadowLayer001
 * @tc.desc: test for OH_Drawing_PenSetShadowLayer.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_PenSetShadowLayer001, TestSize.Level1)
{
    OH_Drawing_PenSetShadowLayer(nullptr, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
}

/*
 * @tc.name: NativeDrawingPenTest_PenSetShadowLayer002
 * @tc.desc: test for OH_Drawing_PenSetShadowLayer.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_PenSetShadowLayer002, TestSize.Level1)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    float blurRadius = 10;
    float x = 100;
    float y = 100;
    uint32_t color = 0xFF00FF00;
    OH_Drawing_ShadowLayer* shadowLayer = OH_Drawing_ShadowLayerCreate(blurRadius, x, y, color);
    OH_Drawing_PenSetShadowLayer(pen, shadowLayer);
    auto blurDrawLooperHandle = Helper::CastTo<OH_Drawing_ShadowLayer*, NativeHandle<BlurDrawLooper>*>(shadowLayer);
    EXPECT_EQ(reinterpret_cast<Pen*>(pen)->GetLooper(), blurDrawLooperHandle->value);
    OH_Drawing_ShadowLayerDestroy(shadowLayer);
    OH_Drawing_PenDestroy(pen);
}

/*
 * @tc.name: NativeDrawingPenTest_PenSetFilter001
 * @tc.desc: OH_Drawing_PenSetFilter.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_PenSetFilter001, TestSize.Level1)
{
    OH_Drawing_PenSetFilter(nullptr, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
}

/*
 * @tc.name: NativeDrawingPenTest_PenSetFilter002
 * @tc.desc: test for OH_Drawing_PenSetFilter.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_PenSetFilter002, TestSize.Level1)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetFilter(pen, nullptr);
    OH_Drawing_Filter* cFilter = nullptr;
    OH_Drawing_PenGetFilter(pen, cFilter);
    EXPECT_EQ(cFilter, nullptr);
    OH_Drawing_PenDestroy(pen);
}

/*
 * @tc.name: NativeDrawingPenTest_PenSetFilter003
 * @tc.desc: test for OH_Drawing_PenSetFilter.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_PenSetFilter003, TestSize.Level1)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_Filter* filter = OH_Drawing_FilterCreate();
    OH_Drawing_PenSetFilter(pen, filter);
    OH_Drawing_Filter* cFilter = nullptr;
    OH_Drawing_PenGetFilter(pen, cFilter);
    ASSERT_TRUE(cFilter == nullptr);
    OH_Drawing_FilterDestroy(filter);
    OH_Drawing_PenDestroy(pen);
}

/*
 * @tc.name: NativeDrawingPenTest_PenGetFilter001
 * @tc.desc: test for OH_Drawing_PenGetFilter.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_PenGetFilter001, TestSize.Level1)
{
    OH_Drawing_PenGetFilter(nullptr, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
}

/*
 * @tc.name: NativeDrawingPenTest_PenGetFilter001
 * @tc.desc: test for OH_Drawing_PenGetFilter.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_PenGetFilter002, TestSize.Level1)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_Filter* filter = nullptr;
    OH_Drawing_PenGetFilter(pen, filter);
    ASSERT_TRUE(filter == nullptr);
    OH_Drawing_FilterDestroy(filter);
    OH_Drawing_PenDestroy(pen);
}

/*
 * @tc.name: NativeDrawingPenTest_PenGetFilter001
 * @tc.desc: test for OH_Drawing_PenGetFilter.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_PenGetFilter003, TestSize.Level1)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    OH_Drawing_Filter* filter = nullptr;
    OH_Drawing_PenGetFilter(pen, filter);
    ASSERT_TRUE(filter == nullptr);
    OH_Drawing_FilterDestroy(filter);
    OH_Drawing_PenDestroy(pen);
}

/*
 * @tc.name: NativeDrawingPenTest_PenReset
 * @tc.desc: test for OH_Drawing_PenReset.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_PenReset, TestSize.Level1)
{
    OH_Drawing_PenReset(nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
}

/*
 * @tc.name: OH_Drawing_PenSetColor4f
 * @tc.desc: test for OH_Drawing_PenSetColor4f.
 * @tc.type: FUNC
 * @tc.require: IC9835
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_PenSetColor4f, TestSize.Level1)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    EXPECT_NE(pen, nullptr);
    OH_NativeColorSpaceManager* colorSpaceManager =
        OH_NativeColorSpaceManager_CreateFromName(ColorSpaceName::ADOBE_RGB);
    EXPECT_EQ(OH_Drawing_PenSetColor4f(nullptr, 0.2f, 0.3f, 0.4f, 0.1f, colorSpaceManager),
        OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_PenSetColor4f(pen, 0.2f, 0.3f, 0.4f, 0.1f, colorSpaceManager), OH_DRAWING_SUCCESS);
    EXPECT_EQ(OH_Drawing_PenSetColor4f(pen, 0.2f, 0.3f, 0.4f, 0.1f, nullptr), OH_DRAWING_SUCCESS);
    OH_Drawing_PenDestroy(pen);
    OH_NativeColorSpaceManager_Destroy(colorSpaceManager);
}

/*
 * @tc.name: OH_Drawing_PenSetColor4f001
 * @tc.desc: test for OH_Drawing_PenSetColor4f.
 * @tc.type: FUNC
 * @tc.require: IC9835
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_PenSetColor4f001, TestSize.Level1)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    EXPECT_NE(pen, nullptr);
    OH_NativeColorSpaceManager* colorSpaceManager =
        OH_NativeColorSpaceManager_CreateFromName(ColorSpaceName::ADOBE_RGB);
    float a = 0;
    float r = 0;
    float g = 0;
    float b = 0;
    EXPECT_EQ(OH_Drawing_PenSetColor4f(pen, 1.01f, 1.1f, 1.5f, 1.4f, colorSpaceManager), OH_DRAWING_SUCCESS);
    EXPECT_EQ(OH_Drawing_PenGetAlphaFloat(pen, &a), OH_DRAWING_SUCCESS);
    EXPECT_EQ(OH_Drawing_PenGetRedFloat(pen, &r), OH_DRAWING_SUCCESS);
    EXPECT_EQ(OH_Drawing_PenGetGreenFloat(pen, &g), OH_DRAWING_SUCCESS);
    EXPECT_EQ(OH_Drawing_PenGetBlueFloat(pen, &b), OH_DRAWING_SUCCESS);
    EXPECT_TRUE(std::abs(a - 1.0f) < 0.01);
    EXPECT_TRUE(std::abs(r - 1.0f) < 0.01);
    EXPECT_TRUE(std::abs(g - 1.0f) < 0.01);
    EXPECT_TRUE(std::abs(b - 1.0f) < 0.01);

    EXPECT_EQ(OH_Drawing_PenSetColor4f(pen, -0.1f, -1.1f, -1.5f, -1.4f, colorSpaceManager), OH_DRAWING_SUCCESS);
    EXPECT_EQ(OH_Drawing_PenGetAlphaFloat(pen, &a), OH_DRAWING_SUCCESS);
    EXPECT_EQ(OH_Drawing_PenGetRedFloat(pen, &r), OH_DRAWING_SUCCESS);
    EXPECT_EQ(OH_Drawing_PenGetGreenFloat(pen, &g), OH_DRAWING_SUCCESS);
    EXPECT_EQ(OH_Drawing_PenGetBlueFloat(pen, &b), OH_DRAWING_SUCCESS);
    EXPECT_TRUE(std::abs(a) < 0.01);
    EXPECT_TRUE(std::abs(r) < 0.01);
    EXPECT_TRUE(std::abs(g) < 0.01);
    EXPECT_TRUE(std::abs(b) < 0.01);
    OH_Drawing_PenDestroy(pen);
    OH_NativeColorSpaceManager_Destroy(colorSpaceManager);
}

/*
 * @tc.name: OH_Drawing_PenGetAlphaFloat
 * @tc.desc: test for OH_Drawing_PenGetAlphaFloat.
 * @tc.type: FUNC
 * @tc.require: IC9835
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_PenGetAlphaFloat, TestSize.Level1)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    EXPECT_NE(pen, nullptr);
    float a = 0.4f;
    OH_NativeColorSpaceManager* colorSpaceManager = nullptr;
    OH_Drawing_PenSetColor4f(pen, 0.2f, 0.3f, 0.4f, 0.1f, colorSpaceManager);
    EXPECT_EQ(OH_Drawing_PenGetAlphaFloat(nullptr, &a), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_PenGetAlphaFloat(nullptr, nullptr), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_PenGetAlphaFloat(pen, nullptr), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_PenGetAlphaFloat(pen, &a), OH_DRAWING_SUCCESS);
    EXPECT_TRUE(std::abs(a - 0.2f) < 0.01);
    OH_Drawing_PenDestroy(pen);
}

/*
 * @tc.name: OH_Drawing_PenGetRedFloat
 * @tc.desc: test for OH_Drawing_PenGetRedFloat.
 * @tc.type: FUNC
 * @tc.require: IC9835
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_PenGetRedFloat, TestSize.Level1)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    EXPECT_NE(pen, nullptr);
    float r = 0.1f;
    OH_NativeColorSpaceManager* colorSpaceManager = nullptr;
    OH_Drawing_PenSetColor4f(pen, 0.2f, 0.3f, 0.4f, 0.1f, colorSpaceManager);
    EXPECT_EQ(OH_Drawing_PenGetRedFloat(nullptr, &r), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_PenGetRedFloat(nullptr, nullptr), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_PenGetRedFloat(pen, nullptr), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_PenGetRedFloat(pen, &r), OH_DRAWING_SUCCESS);
    OH_Drawing_PenDestroy(pen);
    EXPECT_TRUE(std::abs(r - 0.3f) < 0.01);
}

/*
 * @tc.name: OH_Drawing_PenGetGreenFloat
 * @tc.desc: test for OH_Drawing_PenGetGreenFloat.
 * @tc.type: FUNC
 * @tc.require: IC9835
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_PenGetGreenFloat, TestSize.Level1)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    EXPECT_NE(pen, nullptr);
    float g = 0.5f;
    OH_NativeColorSpaceManager* colorSpaceManager = nullptr;
    OH_Drawing_PenSetColor4f(pen, 0.2f, 0.3f, 0.4f, 0.1f, colorSpaceManager);
    EXPECT_EQ(OH_Drawing_PenGetGreenFloat(nullptr, &g), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_PenGetGreenFloat(nullptr, nullptr), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_PenGetGreenFloat(pen, nullptr), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_PenGetGreenFloat(pen, &g), OH_DRAWING_SUCCESS);
    EXPECT_TRUE(std::abs(g - 0.4f) < 0.01);
    OH_Drawing_PenDestroy(pen);
}

/*
 * @tc.name: OH_Drawing_PenGetBlueFloat
 * @tc.desc: test for OH_Drawing_PenGetBlueFloat.
 * @tc.type: FUNC
 * @tc.require: IC9835
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_PenGetBlueFloat, TestSize.Level1)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    EXPECT_NE(pen, nullptr);
    float b = 0.5f;
    OH_NativeColorSpaceManager* colorSpaceManager = nullptr;
    OH_Drawing_PenSetColor4f(pen, 0.2f, 0.3f, 0.4f, 0.1f, colorSpaceManager);
    EXPECT_EQ(OH_Drawing_PenGetBlueFloat(nullptr, &b), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_PenGetBlueFloat(nullptr, nullptr), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_PenGetBlueFloat(pen, nullptr), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_PenGetBlueFloat(pen, &b), OH_DRAWING_SUCCESS);
    EXPECT_TRUE(std::abs(b - 0.1f) < 0.01);
    OH_Drawing_PenDestroy(pen);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS