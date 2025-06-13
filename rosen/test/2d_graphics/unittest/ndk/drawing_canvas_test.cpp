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

#include "drawing_bitmap.h"
#include "drawing_brush.h"
#include "drawing_canvas.h"
#include "drawing_color.h"
#include "drawing_color_filter.h"
#include "drawing_error_code.h"
#include "drawing_filter.h"
#include "drawing_font.h"
#include "drawing_helper.h"
#include "drawing_image.h"
#include "drawing_image_filter.h"
#include "drawing_mask_filter.h"
#include "drawing_matrix.h"
#include "drawing_path.h"
#include "drawing_pen.h"
#include "drawing_pixel_map.h"
#include "drawing_point.h"
#include "drawing_record_cmd.h"
#include "drawing_rect.h"
#include "drawing_region.h"
#include "drawing_round_rect.h"
#include "drawing_sampling_options.h"
#include "drawing_shader_effect.h"
#include "drawing_text_blob.h"
#include "drawing_typeface.h"
#include "drawing_memory_stream.h"
#include "effect/color_filter.h"
#include "effect/filter.h"
#include "recording/recording_canvas.h"
#include "image/pixelmap_native.h"

#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class NativeDrawingCanvasTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
protected:
    OH_Drawing_Canvas *canvas_ = nullptr;
    OH_Drawing_Brush *brush_ = nullptr;
};

constexpr uint32_t POINT_PARAMETER = 3;
constexpr uint32_t COLOR_PARAMETER = 3;
constexpr uint32_t INTNUM_TEN = 10;
constexpr int32_t NEGATIVE_ONE = -1;

void NativeDrawingCanvasTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
}
void NativeDrawingCanvasTest::TearDownTestCase() {}
void NativeDrawingCanvasTest::SetUp()
{
    canvas_ = OH_Drawing_CanvasCreate();
    ASSERT_NE(nullptr, canvas_);
    OH_Drawing_CanvasClear(canvas_, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0xFF, 0xFF));
    brush_ = OH_Drawing_BrushCreate();
    EXPECT_NE(brush_, nullptr);
    OH_Drawing_BrushSetColor(brush_, 0xffff0000);
    OH_Drawing_CanvasAttachBrush(canvas_, brush_);
}

void NativeDrawingCanvasTest::TearDown()
{
    if (canvas_ != nullptr) {
        OH_Drawing_CanvasDetachBrush(canvas_);
        OH_Drawing_BrushDestroy(brush_);
        brush_ = nullptr;
        OH_Drawing_CanvasDestroy(canvas_);
        canvas_ = nullptr;
    }
}

/*
 * @tc.name: NativeDrawingCanvasTest_CanvasNULLPTR001
 * @tc.desc: test for OH_Drawing_CanvasBind.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_CanvasNULLPTR001, TestSize.Level1)
{
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreate();
    EXPECT_NE(bitmap, nullptr);
    OH_Drawing_CanvasBind(nullptr, bitmap);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_BitmapDestroy(bitmap);
    OH_Drawing_CanvasAttachPen(canvas_, nullptr);
    OH_Drawing_CanvasTranslate(canvas_, INT32_MIN, INT32_MIN);
    OH_Drawing_CanvasTranslate(canvas_, INT32_MAX, INT32_MAX);
    OH_Drawing_CanvasDrawLine(nullptr, 0, 0, 20, 20);
    OH_Drawing_CanvasDrawLine(canvas_, 0, 0, INT32_MAX, INT32_MAX);
    OH_Drawing_CanvasDrawLine(canvas_, 0, 0, INT32_MIN, INT32_MIN);
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path, INT32_MAX, INT32_MIN);
    OH_Drawing_PathMoveTo(nullptr, 9999, -1000);
    OH_Drawing_PathClose(nullptr);
    OH_Drawing_PathClose(path);
    OH_Drawing_CanvasDrawPath(nullptr, path);
    OH_Drawing_CanvasDrawPath(canvas_, nullptr);
    OH_Drawing_PathDestroy(path);
}

/*
 * @tc.name: NativeDrawingCanvasTest_CanvasBind002
 * @tc.desc: test for OH_Drawing_CanvasBind.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_CanvasBind002, TestSize.Level1)
{
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreate();
    EXPECT_NE(bitmap, nullptr);
    OH_Drawing_CanvasBind(canvas_, bitmap);
    OH_Drawing_CanvasBind(canvas_, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_BitmapDestroy(bitmap);
}

/*
 * @tc.name: NativeDrawingCanvasTest_canvas003
 * @tc.desc: test for OH_Drawing_CanvasAttachPen & OH_Drawing_CanvasDetachPen.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_canvas003, TestSize.Level1)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    EXPECT_NE(pen, nullptr);
    OH_Drawing_CanvasAttachPen(nullptr, pen);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_CanvasAttachPen(canvas_, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_CanvasAttachPen(canvas_, pen);
    OH_Drawing_CanvasDetachPen(canvas_);
    OH_Drawing_CanvasDetachPen(nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_PenDestroy(pen);
}

/*
 * @tc.name: NativeDrawingCanvasTest_DrawLine004
 * @tc.desc: test for OH_Drawing_CanvasDrawLine.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_DrawLine004, TestSize.Level1)
{
    OH_Drawing_CanvasDrawLine(nullptr, 0, 0, 20, 20);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_CanvasDrawLine(canvas_, 0, 0, 20, 20);
    OH_Drawing_CanvasDrawLine(canvas_, -15.2f, -1, 0, 20);
}

/*
 * @tc.name: NativeDrawingCanvasTest_DrawPath005
 * @tc.desc: test for OH_Drawing_CanvasDrawPath.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_DrawPath005, TestSize.Level1)
{
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    EXPECT_NE(path, nullptr);
    constexpr int height = 300;
    constexpr int width = 300;
    constexpr float arc = 18.0f;
    int len = height / 4;
    float aX = width / 2;
    float aY = height / 4;
    float dX = aX - len * std::sin(arc);
    float dY = aY + len * std::cos(arc);
    float cX = aX + len * std::sin(arc);
    float cY = dY;
    float bX = aX + (len / 2.0);
    float bY = aY + std::sqrt((cX - dX) * (cX - dX) + (len / 2.0) * (len / 2.0));
    float eX = aX - (len / 2.0);
    float eY = bY;
    OH_Drawing_PathMoveTo(path, aX, aY);
    OH_Drawing_PathLineTo(path, bX, bY);
    OH_Drawing_PathLineTo(path, cX, cY);
    OH_Drawing_PathLineTo(path, dX, dY);
    OH_Drawing_PathLineTo(path, eX, eY);
    OH_Drawing_PathClose(path);
    OH_Drawing_CanvasDrawPath(canvas_, path);
    OH_Drawing_CanvasDrawPath(nullptr, path);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_CanvasDrawPath(canvas_, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_PathDestroy(path);
}

/*
 * @tc.name: NativeDrawingCanvasTest_DrawPoints006
 * @tc.desc: test for OH_Drawing_CanvasDrawPoints.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_DrawPoints006, TestSize.Level1)
{
    OH_Drawing_Point2D pointOne={250, 500};
    OH_Drawing_Point2D pointTwo={200, 500};
    OH_Drawing_Point2D pointThree={500, 700};
    OH_Drawing_Point2D points[POINT_PARAMETER] = {pointOne, pointTwo, pointThree};
    OH_Drawing_CanvasDrawPoints(canvas_, POINT_MODE_POINTS, POINT_PARAMETER, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_CanvasDrawPoints(nullptr, POINT_MODE_POINTS, POINT_PARAMETER, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_CanvasDrawPoints(canvas_, POINT_MODE_POINTS, 0, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_CanvasDrawPoints(canvas_, POINT_MODE_POINTS, POINT_PARAMETER, points);
}

/*
 * @tc.name: NativeDrawingCanvasTest_DrawVertices007
 * @tc.desc: test for OH_Drawing_CanvasDrawVertices.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_DrawVertices007, TestSize.Level1)
{
    OH_Drawing_Point2D point_one = {0, 0};
    OH_Drawing_Point2D point_two = {100, 100};
    OH_Drawing_Point2D point_three = {300, 100};
    OH_Drawing_Point2D points_vertices[POINT_PARAMETER] = {point_one, point_two, point_three};

    OH_Drawing_Point2D texs_one = {0, 0};
    OH_Drawing_Point2D texs_two = {1, 1};
    OH_Drawing_Point2D texs_three = {2, 0};
    OH_Drawing_Point2D texs_vertices[POINT_PARAMETER] = {texs_one, texs_two, texs_three};
    uint32_t colors[COLOR_PARAMETER] = {0xFFFF0000, 0xFFFF0000, 0xFFFF0000};

    uint16_t indices[COLOR_PARAMETER] = {0, 1, 2};

    // test canvas == nullptr
    OH_Drawing_CanvasDrawVertices(nullptr, VERTEX_MODE_TRIANGLES, POINT_PARAMETER, points_vertices, texs_vertices,
                                  colors, POINT_PARAMETER, indices, BLEND_MODE_COLOR);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);

    // test vertexCount < 3
    OH_Drawing_CanvasDrawVertices(canvas_, VERTEX_MODE_TRIANGLES, 1, points_vertices, texs_vertices,
                                  colors, POINT_PARAMETER, indices, BLEND_MODE_COLOR);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);

    // test position == nullptr
    OH_Drawing_CanvasDrawVertices(canvas_, VERTEX_MODE_TRIANGLES, POINT_PARAMETER, nullptr, texs_vertices, colors,
                                  POINT_PARAMETER, indices, BLEND_MODE_COLOR);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);

    // test indexCount != 0 && indexCount < 3
    OH_Drawing_CanvasDrawVertices(canvas_, VERTEX_MODE_TRIANGLES, POINT_PARAMETER, points_vertices, texs_vertices,
                                  colors, 1, indices, BLEND_MODE_COLOR);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
}

/*
 * @tc.name: NativeDrawingCanvasTest_SaveAndRestore008
 * @tc.desc: test for OH_Drawing_CanvasSave & OH_Drawing_CanvasRestore.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_SaveAndRestore008, TestSize.Level1)
{
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(100, 200, 500, 300);
    EXPECT_NE(nullptr, rect);
    OH_Drawing_CanvasSave(nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_CanvasSave(canvas_);
    OH_Drawing_CanvasTranslate(nullptr, 100, 100);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_CanvasTranslate(canvas_, 100, 100);
    OH_Drawing_CanvasDrawArc(nullptr, rect, 10, 200);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_CanvasDrawArc(canvas_, nullptr, 10, 200);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_CanvasDrawArc(canvas_, rect, 10, 200);
    OH_Drawing_CanvasRestore(nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_CanvasRestore(canvas_);
    OH_Drawing_CanvasTranslate(canvas_, 200, 200);
    OH_Drawing_CanvasDrawOval(nullptr, rect);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_CanvasDrawOval(canvas_, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_CanvasDrawOval(canvas_, rect);
    OH_Drawing_RectDestroy(nullptr);
    OH_Drawing_RectDestroy(rect);
}

/*
 * @tc.name: NativeDrawingCanvasTest_Clear009
 * @tc.desc: test for OH_Drawing_CanvasClear.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_Clear009, TestSize.Level1)
{
    OH_Drawing_CanvasClear(nullptr, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0x00));
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_CanvasClear(canvas_, -100);
    OH_Drawing_CanvasClear(canvas_, OH_Drawing_ColorSetArgb(INT32_MAX, INT32_MAX, INT32_MIN, INT32_MIN));
    OH_Drawing_CanvasClear(canvas_, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0x00));
}

/*
 * @tc.name: NativeDrawingCanvasTest_RestoreToCount010
 * @tc.desc: test for OH_Drawing_CanvasRestoreToCount.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_RestoreToCount010, TestSize.Level1)
{
    OH_Drawing_CanvasSave(canvas_);
    OH_Drawing_CanvasTranslate(canvas_, 300, 300);
    OH_Drawing_CanvasSave(canvas_);
    OH_Drawing_CanvasTranslate(canvas_, 100, 100);
    OH_Drawing_CanvasSave(canvas_);
    OH_Drawing_CanvasTranslate(canvas_, 100, 100);
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(0, 0, 100, 100);
    OH_Drawing_CanvasDrawRect(canvas_, rect);
    int count = OH_Drawing_CanvasGetSaveCount(canvas_);
    EXPECT_EQ(count, 4);
    OH_Drawing_CanvasRestoreToCount(nullptr, count - 2);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_CanvasRestoreToCount(canvas_, count - 2);
    EXPECT_EQ(2, OH_Drawing_CanvasGetSaveCount(canvas_));
    OH_Drawing_RoundRect *roundRect = OH_Drawing_RoundRectCreate(nullptr, 20, 20);
    EXPECT_EQ(roundRect, nullptr);
    roundRect = OH_Drawing_RoundRectCreate(rect, 20, 20);
    EXPECT_NE(roundRect, nullptr);
    OH_Drawing_CanvasDrawRoundRect(nullptr, roundRect);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_CanvasDrawRoundRect(canvas_, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_CanvasDrawRoundRect(canvas_, roundRect);
    OH_Drawing_CanvasRestoreToCount(canvas_, 1);
    OH_Drawing_CanvasDrawRect(canvas_, rect);
    OH_Drawing_RoundRectDestroy(roundRect);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_RoundRectDestroy(nullptr);
    OH_Drawing_RectDestroy(nullptr);
}

/*
 * @tc.name: NativeDrawingCanvasTest_Scale011
 * @tc.desc: test for OH_Drawing_CanvasScale.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_Scale011, TestSize.Level1)
{
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(10, 20, 60, 120);
    OH_Drawing_CanvasTranslate(canvas_, 20, 20);
    OH_Drawing_CanvasDrawRect(canvas_, rect);
    OH_Drawing_CanvasScale(nullptr, 2, .5f);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_CanvasScale(canvas_, 2, .5f);
    OH_Drawing_BrushSetColor(nullptr, 0xFF67C23A);
    OH_Drawing_BrushSetColor(brush_, 0xFF67C23A);
    OH_Drawing_CanvasDrawRect(canvas_, rect);
}

/*
 * @tc.name: NativeDrawingCanvasTest_Skew012
 * @tc.desc: test for OH_Drawing_CanvasSkew.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_Skew012, TestSize.Level1)
{
    /**
     * @tc.steps: step1. test OH_Drawing_CanvasSkew with an nullptr.
     * @tc.expected: function works well not crash.
     */
    OH_Drawing_CanvasSkew(nullptr, 0, 0.3f);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    /**
     * @tc.steps: step2. test OH_Drawing_CanvasSkew with canvas.
     * @tc.expected: function works well.
     */
    OH_Drawing_CanvasSkew(canvas_, 0, 0.3f);

    OH_Drawing_Matrix* matrixSkew= OH_Drawing_MatrixCreate();
    Matrix* cMatrix = reinterpret_cast<Matrix*>(matrixSkew);
    EXPECT_NE(cMatrix, nullptr);
    cMatrix->SetSkew(0, 0.3f);

    OH_Drawing_Matrix* matrixTotal = OH_Drawing_MatrixCreate();
    OH_Drawing_CanvasGetTotalMatrix(canvas_, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_CanvasGetTotalMatrix(nullptr, matrixTotal);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    float valueMatrixSkew;
    float valueCanvasSkew;
    valueMatrixSkew = OH_Drawing_MatrixGetValue(matrixSkew, 0);
    valueCanvasSkew = OH_Drawing_MatrixGetValue(matrixTotal, 0);
    EXPECT_TRUE(IsScalarAlmostEqual(valueMatrixSkew, valueCanvasSkew));

    OH_Drawing_CanvasGetTotalMatrix(canvas_, matrixTotal);
    EXPECT_NE(matrixTotal, nullptr);

    valueMatrixSkew = OH_Drawing_MatrixGetValue(matrixSkew, 0);
    valueCanvasSkew = OH_Drawing_MatrixGetValue(matrixTotal, 0);
    EXPECT_TRUE(IsScalarAlmostEqual(valueMatrixSkew, valueCanvasSkew));

    valueMatrixSkew = OH_Drawing_MatrixGetValue(matrixSkew, 1);
    valueCanvasSkew = OH_Drawing_MatrixGetValue(matrixTotal, 1);
    EXPECT_TRUE(IsScalarAlmostEqual(valueMatrixSkew, valueCanvasSkew));

    valueMatrixSkew = OH_Drawing_MatrixGetValue(matrixSkew, 2);
    valueCanvasSkew = OH_Drawing_MatrixGetValue(matrixTotal, 2);
    EXPECT_TRUE(IsScalarAlmostEqual(valueMatrixSkew, valueCanvasSkew));

    valueMatrixSkew = OH_Drawing_MatrixGetValue(matrixSkew, 3);
    valueCanvasSkew = OH_Drawing_MatrixGetValue(matrixTotal, 3);
    EXPECT_TRUE(IsScalarAlmostEqual(valueMatrixSkew, valueCanvasSkew));

    valueMatrixSkew = OH_Drawing_MatrixGetValue(matrixSkew, 4);
    valueCanvasSkew = OH_Drawing_MatrixGetValue(matrixTotal, 4);
    EXPECT_TRUE(IsScalarAlmostEqual(valueMatrixSkew, valueCanvasSkew));

    valueMatrixSkew = OH_Drawing_MatrixGetValue(matrixSkew, 5);
    valueCanvasSkew = OH_Drawing_MatrixGetValue(matrixTotal, 5);
    EXPECT_TRUE(IsScalarAlmostEqual(valueMatrixSkew, valueCanvasSkew));

    valueMatrixSkew = OH_Drawing_MatrixGetValue(matrixSkew, 6);
    valueCanvasSkew = OH_Drawing_MatrixGetValue(matrixTotal, 6);
    EXPECT_TRUE(IsScalarAlmostEqual(valueMatrixSkew, valueCanvasSkew));

    valueMatrixSkew = OH_Drawing_MatrixGetValue(matrixSkew, 7);
    valueCanvasSkew = OH_Drawing_MatrixGetValue(matrixTotal, 7);
    EXPECT_TRUE(IsScalarAlmostEqual(valueMatrixSkew, valueCanvasSkew));

    valueMatrixSkew = OH_Drawing_MatrixGetValue(matrixSkew, 8);
    valueCanvasSkew = OH_Drawing_MatrixGetValue(matrixTotal, 8);
    EXPECT_TRUE(IsScalarAlmostEqual(valueMatrixSkew, valueCanvasSkew));

    OH_Drawing_MatrixDestroy(matrixSkew);
    OH_Drawing_MatrixDestroy(matrixTotal);
}

/*
 * @tc.name: NativeDrawingCanvasTest_ClipRect013
 * @tc.desc: test for OH_Drawing_CanvasClipRect.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_ClipRect013, TestSize.Level1)
{
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(0, 0, 90, 90);
    OH_Drawing_CanvasRotate(nullptr, 10, 0, 0);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_CanvasRotate(canvas_, 10, 0, 0);
    OH_Drawing_Point *point = OH_Drawing_PointCreate(100, 60);
    OH_Drawing_BrushSetAntiAlias(nullptr, true);
    OH_Drawing_BrushSetAntiAlias(brush_, true);
    for (auto alias : {false, true}) {
        OH_Drawing_CanvasSave(canvas_);
        OH_Drawing_CanvasClipRect(nullptr, rect, OH_Drawing_CanvasClipOp::INTERSECT, alias);
        EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
        OH_Drawing_CanvasClipRect(canvas_, nullptr, OH_Drawing_CanvasClipOp::INTERSECT, alias);
        EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
        OH_Drawing_CanvasClipRect(canvas_, rect, OH_Drawing_CanvasClipOp::INTERSECT, alias);
        OH_Drawing_CanvasDrawCircle(canvas_, nullptr, 60);
        EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
        OH_Drawing_CanvasDrawCircle(nullptr, point, 60);
        EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
        OH_Drawing_CanvasDrawCircle(canvas_, point, 0);
        EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE);
        OH_Drawing_CanvasDrawCircle(canvas_, point, 60);
        OH_Drawing_CanvasRestore(canvas_);
        OH_Drawing_CanvasTranslate(canvas_, 80, 0);
    }
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_PointDestroy(point);
}

/*
 * @tc.name: NativeDrawingCanvasTest_ClipPath014
 * @tc.desc: test for OH_Drawing_CanvasClipPath.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_ClipPath014, TestSize.Level1)
{
    OH_Drawing_Path *path = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path, 100, 300);
    OH_Drawing_PathLineTo(path, 200, 300);
    OH_Drawing_PathLineTo(path, 200, 400);
    OH_Drawing_PathLineTo(path, 100, 350);
    OH_Drawing_PathClose(nullptr);
    OH_Drawing_PathClose(path);
    OH_Drawing_CanvasClipPath(nullptr, path, OH_Drawing_CanvasClipOp::DIFFERENCE, true);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_CanvasClipPath(canvas_, nullptr, OH_Drawing_CanvasClipOp::DIFFERENCE, true);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_CanvasClipPath(canvas_, path, OH_Drawing_CanvasClipOp::DIFFERENCE, true);
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(100, 300, 200, 400);
    OH_Drawing_CanvasDrawRect(nullptr, rect);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_CanvasDrawRect(canvas_, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_CanvasDrawRect(canvas_, rect);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_PathDestroy(path);
}

/*
 * @tc.name: NativeDrawingCanvasTest_LinearGradient015
 * @tc.desc: test for LinearGradient
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_LinearGradient015, TestSize.Level1)
{
    OH_Drawing_Point* startPt = OH_Drawing_PointCreate(100, 400);
    OH_Drawing_Point* endPt = OH_Drawing_PointCreate(200, 500);
    uint32_t color[] = {0xffff0000, 0xff00ff00};
    float pos[] = {0., 1.0};
    OH_Drawing_ShaderEffect* linearGradient = OH_Drawing_ShaderEffectCreateLinearGradient(startPt, endPt,
        color, pos, 2, OH_Drawing_TileMode::CLAMP);
    OH_Drawing_BrushSetShaderEffect(nullptr, linearGradient);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_BrushSetShaderEffect(brush_, nullptr);
    OH_Drawing_BrushSetShaderEffect(brush_, linearGradient);
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(200, 500, 300, 600);
    OH_Drawing_CanvasDrawRect(canvas_, rect);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_ShaderEffectDestroy(nullptr);
    OH_Drawing_ShaderEffectDestroy(linearGradient);
    OH_Drawing_PointDestroy(startPt);
    OH_Drawing_PointDestroy(endPt);
}

/*
 * @tc.name: NativeDrawingCanvasTest_SweepGradient016
 * @tc.desc: test for sweep gradient shader effect
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_SweepGradient016, TestSize.Level1)
{
    OH_Drawing_Point* centerPt = OH_Drawing_PointCreate(350, 450);
    uint32_t colors[] = {0xffff00ff, 0xff00ff00};
    float pos[] = {0., 1.0};
    OH_Drawing_ShaderEffect* sweepGradient = OH_Drawing_ShaderEffectCreateSweepGradient(nullptr, nullptr,
        nullptr, -2, OH_Drawing_TileMode::MIRROR);
    EXPECT_EQ(sweepGradient, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    sweepGradient = OH_Drawing_ShaderEffectCreateSweepGradient(centerPt, colors,
        pos, 2, OH_Drawing_TileMode::MIRROR);
    EXPECT_NE(sweepGradient, nullptr);
    OH_Drawing_BrushSetShaderEffect(brush_, sweepGradient);
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(300, 400, 500, 500);
    OH_Drawing_CanvasDrawRect(canvas_, rect);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_ShaderEffectDestroy(sweepGradient);
    OH_Drawing_PointDestroy(centerPt);
}

/*
 * @tc.name: NativeDrawingCanvasTest_ImageShader017
 * @tc.desc: test for image shader effect
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_ImageShader017, TestSize.Level1)
{
    OH_Drawing_Image *image = OH_Drawing_ImageCreate();
    OH_Drawing_Brush *brush = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetColor(brush, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0xFF, 0x00));
    OH_Drawing_SamplingOptions *options = OH_Drawing_SamplingOptionsCreate(FILTER_MODE_LINEAR, MIPMAP_MODE_LINEAR);
    OH_Drawing_Matrix *matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_ShaderEffect *effect = nullptr;
    effect = OH_Drawing_ShaderEffectCreateImageShader(nullptr, CLAMP, CLAMP, nullptr, nullptr);
    EXPECT_EQ(effect, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    effect = OH_Drawing_ShaderEffectCreateImageShader(image, CLAMP, CLAMP, options, matrix);
    OH_Drawing_BrushSetShaderEffect(brush, effect);
    OH_Drawing_CanvasAttachBrush(nullptr, brush);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_CanvasAttachBrush(canvas_, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_CanvasAttachBrush(canvas_, brush);
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(300, 400, 500, 500);
    OH_Drawing_CanvasDrawRect(canvas_, rect);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_ShaderEffectDestroy(effect);
    OH_Drawing_MatrixDestroy(matrix);
    OH_Drawing_SamplingOptionsDestroy(options);
    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_ImageDestroy(image);
}

/*
 * @tc.name: NativeDrawingCanvasTest_MaskFilter018
 * @tc.desc: test for maskfilter
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_MaskFilter018, TestSize.Level1)
{
    EXPECT_EQ(OH_Drawing_BrushGetColor(brush_), 0xFFFF0000);
    OH_Drawing_MaskFilter* maskFilter = OH_Drawing_MaskFilterCreateBlur(OH_Drawing_BlurType::NORMAL, 10, true);
    OH_Drawing_Filter* filter = OH_Drawing_FilterCreate();
    OH_Drawing_FilterSetMaskFilter(nullptr, maskFilter);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_FilterSetMaskFilter(filter, nullptr);
    OH_Drawing_FilterSetMaskFilter(filter, maskFilter);
    OH_Drawing_BrushSetFilter(brush_, filter);
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(200, 500, 300, 600);
    OH_Drawing_CanvasDrawRect(canvas_, rect);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_MaskFilterDestroy(maskFilter);
    OH_Drawing_FilterDestroy(filter);
}

/*
 * @tc.name: NativeDrawingCanvasTest_ColorFilter019
 * @tc.desc: test for colorfilter create blend mode, linear to srgb gamma, luma, matrix
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_ColorFilterCreateBlendMode019, TestSize.Level1)
{
    EXPECT_EQ(OH_Drawing_BrushGetColor(brush_), 0xFFFF0000);
    OH_Drawing_ColorFilter* colorFilter = OH_Drawing_ColorFilterCreateBlendMode(0xff0000ff,
        OH_Drawing_BlendMode::BLEND_MODE_SRC);
    EXPECT_NE(colorFilter, nullptr);
    OH_Drawing_ColorFilter* colorFilterTmp = OH_Drawing_ColorFilterCreateLinearToSrgbGamma();
    EXPECT_NE(colorFilterTmp, nullptr);
    OH_Drawing_Filter* filter = OH_Drawing_FilterCreate();
    EXPECT_NE(filter, nullptr);

    OH_Drawing_FilterSetColorFilter(nullptr, colorFilter);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_FilterSetColorFilter(filter, nullptr);
    OH_Drawing_FilterGetColorFilter(filter, colorFilterTmp);
    NativeHandle<ColorFilter>* colorFilterHandle = Helper::CastTo<OH_Drawing_ColorFilter*,
        NativeHandle<ColorFilter>*>(colorFilterTmp);
    EXPECT_NE(colorFilterHandle, nullptr);
    EXPECT_NE(colorFilterHandle->value, nullptr);
    EXPECT_EQ(colorFilterHandle->value->GetType(), ColorFilter::FilterType::NO_TYPE);

    OH_Drawing_FilterSetColorFilter(filter, colorFilter);
    OH_Drawing_FilterGetColorFilter(filter, colorFilterTmp);
    NativeHandle<ColorFilter>* colorFilterHandleT = Helper::CastTo<OH_Drawing_ColorFilter*,
        NativeHandle<ColorFilter>*>(colorFilterTmp);
    EXPECT_NE(colorFilterHandleT, nullptr);
    EXPECT_NE(colorFilterHandleT->value, nullptr);
    EXPECT_EQ(colorFilterHandleT->value->GetType(), ColorFilter::FilterType::BLEND_MODE);

    OH_Drawing_BrushSetFilter(brush_, nullptr);
    OH_Drawing_BrushSetFilter(brush_, filter);
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(0, 0, 100, 100);
    OH_Drawing_CanvasDrawRect(canvas_, rect);
    OH_Drawing_ColorFilter* linear = OH_Drawing_ColorFilterCreateLinearToSrgbGamma();
    OH_Drawing_FilterSetColorFilter(filter, linear);
    OH_Drawing_CanvasTranslate(canvas_, 100, 100);
    OH_Drawing_CanvasDrawRect(canvas_, rect);
    OH_Drawing_ColorFilter* luma = OH_Drawing_ColorFilterCreateLuma();
    OH_Drawing_FilterSetColorFilter(filter, luma);
    OH_Drawing_CanvasTranslate(canvas_, 200, 200);
    OH_Drawing_CanvasDrawRect(canvas_, rect);
    const float matrix[20] = {
        1, 0, 0, 0, 0,
        0, 1, 0, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 0, 0.5f, 0
    };
    OH_Drawing_ColorFilter* matrixFilter = OH_Drawing_ColorFilterCreateMatrix(nullptr);
    EXPECT_EQ(matrixFilter, nullptr);
    matrixFilter = OH_Drawing_ColorFilterCreateMatrix(matrix);
    EXPECT_NE(matrixFilter, nullptr);
    OH_Drawing_FilterSetColorFilter(filter, matrixFilter);
    OH_Drawing_CanvasTranslate(canvas_, 300, 300);
    OH_Drawing_CanvasDrawRect(canvas_, rect);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_ColorFilterDestroy(colorFilter);
    OH_Drawing_ColorFilterDestroy(luma);
    OH_Drawing_ColorFilterDestroy(matrixFilter);
    OH_Drawing_ColorFilterDestroy(linear);
    OH_Drawing_ColorFilterDestroy(colorFilterTmp);
    OH_Drawing_FilterDestroy(filter);
    OH_Drawing_FilterDestroy(nullptr);
}

/*
 * @tc.name: NativeDrawingCanvasTest_ColorFilterCreateCompose020
 * @tc.desc: test for create compose color filter
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_ColorFilterCreateCompose020, TestSize.Level1)
{
    EXPECT_EQ(OH_Drawing_BrushGetColor(brush_), 0xFFFF0000);
    OH_Drawing_ColorFilter* outerFilter = OH_Drawing_ColorFilterCreateLuma();
    OH_Drawing_ColorFilter* innerFilter = OH_Drawing_ColorFilterCreateSrgbGammaToLinear();
    OH_Drawing_ColorFilter* compose = OH_Drawing_ColorFilterCreateCompose(nullptr, nullptr);
    EXPECT_EQ(compose, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    compose = OH_Drawing_ColorFilterCreateCompose(outerFilter, innerFilter);
    EXPECT_NE(compose, nullptr);
    OH_Drawing_Filter* filter = OH_Drawing_FilterCreate();
    OH_Drawing_FilterSetColorFilter(filter, compose);
    OH_Drawing_BrushSetFilter(brush_, filter);
    OH_Drawing_Canvas* canvas_ = OH_Drawing_CanvasCreate();
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(200, 500, 300, 600);
    OH_Drawing_CanvasDrawRect(canvas_, rect);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_ColorFilterDestroy(outerFilter);
    OH_Drawing_ColorFilterDestroy(innerFilter);
    OH_Drawing_ColorFilterDestroy(compose);
    OH_Drawing_FilterDestroy(filter);
}

/*
 * @tc.name: NativeDrawingCanvasTest_DrawBitmap021
 * @tc.desc: test for DrawBitmap
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_DrawBitmap021, TestSize.Level1)
{
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat cFormat{COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE};
    constexpr uint32_t width = 200;
    constexpr uint32_t height = 200;
    OH_Drawing_BitmapBuild(bitmap, width, height, &cFormat);
    OH_Drawing_CanvasBind(canvas_, bitmap);
    OH_Drawing_CanvasDrawBitmap(nullptr, bitmap, 0, 0);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_CanvasDrawBitmap(canvas_, nullptr, 0, 0);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_CanvasDrawBitmap(canvas_, bitmap, 0, 0);
    OH_Drawing_CanvasClear(canvas_, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0xFF, 0xFF));
    OH_Drawing_BitmapDestroy(bitmap);
}

/*
 * @tc.name: NativeDrawingCanvasTest_DrawBitmapRect022
 * @tc.desc: test for DrawBitmapRect
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_DrawBitmapRect022, TestSize.Level1)
{
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreate();
    EXPECT_NE(bitmap, nullptr);
    OH_Drawing_BitmapFormat cFormat{COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE};
    constexpr uint32_t width = 200;
    constexpr uint32_t height = 200;
    OH_Drawing_BitmapBuild(bitmap, width, height, &cFormat);
    OH_Drawing_Rect* src = OH_Drawing_RectCreate(0, 0, 200, 200);
    EXPECT_NE(src, nullptr);
    OH_Drawing_Rect* dst = OH_Drawing_RectCreate(0, 0, 200, 200);
    EXPECT_NE(dst, nullptr);
    OH_Drawing_SamplingOptions* options = OH_Drawing_SamplingOptionsCreate(
        OH_Drawing_FilterMode::FILTER_MODE_NEAREST, OH_Drawing_MipmapMode::MIPMAP_MODE_NEAREST);
    EXPECT_NE(options, nullptr);
    OH_Drawing_CanvasDrawBitmapRect(canvas_, bitmap, src, dst, options);
    OH_Drawing_CanvasDrawBitmapRect(canvas_, bitmap, src, dst, nullptr);
    OH_Drawing_CanvasDrawBitmapRect(canvas_, bitmap, src, nullptr, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_CanvasDrawBitmapRect(canvas_, bitmap, nullptr, nullptr, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_CanvasDrawBitmapRect(canvas_, nullptr, nullptr, nullptr, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_CanvasDrawBitmapRect(nullptr, nullptr, nullptr, nullptr, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_BitmapDestroy(bitmap);
}

/*
 * @tc.name: NativeDrawingCanvasTest_SetMatrix023
 * @tc.desc: test for SetMatrix
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_SetMatrix023, TestSize.Level1)
{
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    EXPECT_NE(matrix, nullptr);
    OH_Drawing_MatrixSetMatrix(
        matrix,
        1, 0, 0,
        0, -1, 0,
        0, 0, 1);
    OH_Drawing_CanvasSetMatrix(canvas_, matrix);
    OH_Drawing_CanvasSetMatrix(canvas_, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_CanvasSetMatrix(nullptr, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_MatrixDestroy(matrix);
}

/*
 * @tc.name: NativeDrawingCanvasTest_ResetMatrix024
 * @tc.desc: test for ResetMatrix
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_ResetMatrix024, TestSize.Level1)
{
    OH_Drawing_CanvasResetMatrix(nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_CanvasResetMatrix(canvas_);
}

/*
 * @tc.name: NativeDrawingCanvasTest_DrawImageRect025
 * @tc.desc: test for DrawImageRect
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_DrawImageRect025, TestSize.Level1)
{
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, 200, 200);
    EXPECT_NE(rect, nullptr);
    OH_Drawing_Image* image = OH_Drawing_ImageCreate();
    EXPECT_NE(image, nullptr);
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreate();
    EXPECT_NE(bitmap, nullptr);
    OH_Drawing_BitmapFormat cFormat{COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE};
    constexpr uint32_t width = 200;
    constexpr uint32_t height = 200;
    OH_Drawing_BitmapBuild(bitmap, width, height, &cFormat);
    OH_Drawing_ImageBuildFromBitmap(image, bitmap);
    OH_Drawing_SamplingOptions* options = OH_Drawing_SamplingOptionsCreate(
        OH_Drawing_FilterMode::FILTER_MODE_NEAREST, OH_Drawing_MipmapMode::MIPMAP_MODE_NEAREST);
    EXPECT_NE(options, nullptr);
    OH_Drawing_CanvasDrawImageRect(canvas_, image, rect, options);
    OH_Drawing_CanvasDrawImageRect(canvas_, image, rect, nullptr);
    OH_Drawing_CanvasDrawImageRect(canvas_, image, nullptr, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_CanvasDrawImageRect(canvas_, nullptr, nullptr, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_CanvasDrawImageRect(nullptr, nullptr, nullptr, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_CanvasDrawImageRectWithSrc(canvas_, image, rect, rect, options, STRICT_SRC_RECT_CONSTRAINT);
    OH_Drawing_CanvasDrawImageRectWithSrc(nullptr, image, rect, rect, options, STRICT_SRC_RECT_CONSTRAINT);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_CanvasDrawImageRectWithSrc(canvas_, nullptr, rect, rect, options, STRICT_SRC_RECT_CONSTRAINT);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_CanvasDrawImageRectWithSrc(canvas_, image, nullptr, rect, options, STRICT_SRC_RECT_CONSTRAINT);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_CanvasDrawImageRectWithSrc(nullptr, nullptr, nullptr, nullptr, nullptr, STRICT_SRC_RECT_CONSTRAINT);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_SamplingOptionsDestroy(options);
    OH_Drawing_BitmapDestroy(bitmap);
    OH_Drawing_ImageDestroy(image);
}

/*
 * @tc.name: NativeDrawingCanvasTest_ReadPixelsToBitmap026
 * @tc.desc: test for ReadPixelsToBitmap
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_ReadPixelsToBitmap026, TestSize.Level1)
{
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreate();
    EXPECT_NE(bitmap, nullptr);
    EXPECT_TRUE(!OH_Drawing_CanvasReadPixelsToBitmap(canvas_, bitmap, 100, 100));
    EXPECT_TRUE(!OH_Drawing_CanvasReadPixelsToBitmap(canvas_, nullptr, 100, 100));
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_TRUE(!OH_Drawing_CanvasReadPixelsToBitmap(nullptr, nullptr, 100, 100));
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_BitmapDestroy(bitmap);
}

/*
 * @tc.name: NativeDrawingCanvasTest_ReadPixels027
 * @tc.desc: test for ReadPixels
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_ReadPixels027, TestSize.Level1)
{
    OH_Drawing_Image_Info imageInfo;
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreate();
    EXPECT_NE(bitmap, nullptr);
    OH_Drawing_BitmapFormat cFormat{COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE};
    constexpr uint32_t width = 200;
    constexpr uint32_t height = 200;
    OH_Drawing_BitmapBuild(bitmap, width, height, &cFormat);
    void* pixels = OH_Drawing_BitmapGetPixels(bitmap);
    EXPECT_TRUE(!OH_Drawing_CanvasReadPixels(canvas_, &imageInfo, pixels, 0, 0, 0));
    EXPECT_TRUE(!OH_Drawing_CanvasReadPixels(canvas_, &imageInfo, nullptr, 0, 0, 0));
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_TRUE(!OH_Drawing_CanvasReadPixels(canvas_, nullptr, nullptr, 0, 0, 0));
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_TRUE(!OH_Drawing_CanvasReadPixels(nullptr, nullptr, nullptr, 0, 0, 0));
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_BitmapDestroy(bitmap);
}

/*
 * @tc.name: NativeDrawingCanvasTest_GetWidth028
 * @tc.desc: test for GetWidth
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_GetWidth028, TestSize.Level1)
{
    EXPECT_TRUE(OH_Drawing_CanvasGetWidth(nullptr) == 0);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_TRUE(OH_Drawing_CanvasGetWidth(canvas_) >= 0);
}

/*
 * @tc.name: NativeDrawingCanvasTest_GetHeight029
 * @tc.desc: test for GetHeight
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_GetHeight029, TestSize.Level1)
{
    EXPECT_TRUE(OH_Drawing_CanvasGetHeight(nullptr) == 0);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_TRUE(OH_Drawing_CanvasGetHeight(canvas_) >= 0);
}

/*
 * @tc.name: NativeDrawingCanvasTest_GetLocalClipBounds030
 * @tc.desc: test for GetLocalClipBounds
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_GetLocalClipBounds030, TestSize.Level1)
{
    OH_Drawing_CanvasGetLocalClipBounds(nullptr, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, 1, 1);
    EXPECT_NE(rect, nullptr);
    OH_Drawing_CanvasGetLocalClipBounds(nullptr, rect);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_CanvasGetLocalClipBounds(canvas_, rect);
    OH_Drawing_RectDestroy(rect);
}

/*
 * @tc.name: NativeDrawingCanvasTest_ConcatMatrix031
 * @tc.desc: test for ConcatMatrix
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_ConcatMatrix031, TestSize.Level1)
{
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    EXPECT_NE(matrix, nullptr);
    OH_Drawing_MatrixSetMatrix(
        matrix,
        1, 0, 0,
        0, -1, 0,
        0, 0, 1);
    OH_Drawing_CanvasConcatMatrix(canvas_, matrix);
    OH_Drawing_CanvasConcatMatrix(canvas_, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_CanvasConcatMatrix(nullptr, nullptr);
    OH_Drawing_MatrixDestroy(matrix);
}

/*
 * @tc.name: NativeDrawingCanvasTest_DrawShadow032
 * @tc.desc: test for DrawShadow
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_DrawShadow032, TestSize.Level1)
{
    OH_Drawing_Point3D p1{0.0, 0.0, 0.0};
    OH_Drawing_Point3D p2{10.0, 10.0, 10.0};
    OH_Drawing_CanvasDrawShadow(nullptr, nullptr, p1, p2, 0, 0xFF000000,
        0xFF000000, OH_Drawing_CanvasShadowFlags::SHADOW_FLAGS_ALL);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_CanvasDrawShadow(canvas_, nullptr, p1, p2, 0, 0xFF000000,
        0xFF000000, OH_Drawing_CanvasShadowFlags::SHADOW_FLAGS_ALL);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);

    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    EXPECT_NE(path, nullptr);
    OH_Drawing_CanvasDrawShadow(canvas_, path, p1, p2, 10, 0xFF000000, 0xFF000000,
        OH_Drawing_CanvasShadowFlags::SHADOW_FLAGS_ALL);
    OH_Drawing_CanvasDrawShadow(canvas_, path, p1, p2, 10, 0xFF000000, 0xFF000000,
        static_cast<OH_Drawing_CanvasShadowFlags>(NEGATIVE_ONE));
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE);
    OH_Drawing_CanvasDrawShadow(canvas_, path, p1, p2, 10, 0xFF000000, 0xFF000000,
        static_cast<OH_Drawing_CanvasShadowFlags>(INTNUM_TEN));
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE);
}

/*
 * @tc.name: NativeDrawingCanvasTest_DrawTextBlob033
 * @tc.desc: test for DrawTextBlob
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_DrawTextBlob033, TestSize.Level1)
{
    OH_Drawing_Font *font = OH_Drawing_FontCreate();
    OH_Drawing_FontSetFakeBoldText(font, true);
    OH_Drawing_FontSetTextSize(font, 20);
    OH_Drawing_FontSetTextSkewX(font, 0.25);
    OH_Drawing_Typeface *typeSurface = OH_Drawing_TypefaceCreateDefault();
    OH_Drawing_FontSetTypeface(font, typeSurface);
    auto *builder = OH_Drawing_TextBlobBuilderCreate();
    EXPECT_EQ(nullptr, OH_Drawing_TextBlobBuilderAllocRunPos(nullptr, nullptr, INT32_MAX, nullptr));
    EXPECT_EQ(nullptr, OH_Drawing_TextBlobBuilderAllocRunPos(builder, font, INT32_MIN, nullptr));
    const OH_Drawing_RunBuffer* runBuffer = OH_Drawing_TextBlobBuilderAllocRunPos(builder, font, 9, nullptr);
    ASSERT_NE(runBuffer, nullptr);
    runBuffer->glyphs[0] = 65;
    runBuffer->pos[0] = 0;
    runBuffer->pos[1] = 0;
    runBuffer->glyphs[1] = 227;
    runBuffer->pos[2] = 14.9;
    runBuffer->pos[3] = 0;
    runBuffer->glyphs[2] = 283;
    runBuffer->pos[4] = 25.84;
    runBuffer->pos[5] = 0;
    runBuffer->glyphs[3] = 283;
    runBuffer->pos[6] = 30.62;
    runBuffer->pos[7] = 0;
    runBuffer->glyphs[4] = 299;
    runBuffer->pos[8] = 35.4;
    runBuffer->pos[9] = 0;
    runBuffer->glyphs[5] = 2;
    runBuffer->pos[10] = 47.22;
    runBuffer->pos[11] = 0;
    runBuffer->glyphs[6]= 94;
    runBuffer->pos[12] = 52.62;
    runBuffer->pos[13] = 0;
    runBuffer->glyphs[7] = 37;
    runBuffer->pos[14] = 67.42;
    runBuffer->pos[15] = 0;
    runBuffer->glyphs[8] = 84;
    runBuffer->pos[16] = 81.7;
    runBuffer->pos[17] = 0;
    OH_Drawing_TextBlob *textBlob = OH_Drawing_TextBlobBuilderMake(builder);
    OH_Drawing_CanvasDrawTextBlob(canvas_, textBlob, 100, 700);
    OH_Drawing_TextBlobDestroy(textBlob);
    OH_Drawing_TextBlobBuilderDestroy(builder);
    OH_Drawing_FontDestroy(font);
    OH_Drawing_TypefaceDestroy(typeSurface);
    OH_Drawing_TextBlobDestroy(nullptr);
    OH_Drawing_TextBlobBuilderDestroy(nullptr);
    OH_Drawing_FontDestroy(nullptr);
    OH_Drawing_TypefaceDestroy(nullptr);
}

/*
 * @tc.name: NativeDrawingCanvasTest_DrawTextBlob034
 * @tc.desc: test for DrawTextBlob2
 * @tc.type: FUNC
 * @tc.require: SR000S9F0C
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_DrawTextBlob034, TestSize.Level1)
{
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(0, 0, 0, 0);
    EXPECT_NE(rect, nullptr);
    OH_Drawing_Font *font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    const char* str = "123456";
    EXPECT_EQ(nullptr, OH_Drawing_TextBlobCreateFromString(nullptr,
        font, OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8));
    EXPECT_EQ(nullptr, OH_Drawing_TextBlobCreateFromString(str,
        nullptr, OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8));
    OH_Drawing_TextBlob *textBlob = OH_Drawing_TextBlobCreateFromString(str,
        font, OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8);
    EXPECT_NE(textBlob, nullptr);
    OH_Drawing_CanvasDrawTextBlob(nullptr, textBlob, 0, 0);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_CanvasDrawTextBlob(canvas_, nullptr, 0, 0);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_CanvasDrawTextBlob(canvas_, textBlob, 0, 0);
    OH_Drawing_TextBlobGetBounds(textBlob, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_TextBlobGetBounds(textBlob, rect);
    OH_Drawing_CanvasDrawRect(canvas_, rect);
    OH_Drawing_TextBlobDestroy(textBlob);
    OH_Drawing_FontDestroy(font);
    OH_Drawing_RectDestroy(rect);
}

/*
 * @tc.name: NativeDrawingCanvasTest_DrawTextBlob035
 * @tc.desc: test for DrawTextBlob3
 * @tc.type: FUNC
 * @tc.require: SR000S9F0C
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_DrawTextBlob035, TestSize.Level1)
{
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(0, 0, 0, 0);
    EXPECT_NE(rect, nullptr);
    OH_Drawing_Font *font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    OH_Drawing_Typeface* typeface = OH_Drawing_TypefaceCreateFromFile(nullptr, 0);
    EXPECT_EQ(nullptr, typeface);
    // sub test 1, OH_Drawing_FontGetTypeface
    OH_Drawing_FontGetTypeface(nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(nullptr, typeface);
    OH_Drawing_Typeface *typeSurface = OH_Drawing_TypefaceCreateDefault();
    OH_Drawing_FontSetTypeface(font, typeSurface);
    EXPECT_NE(nullptr, OH_Drawing_FontGetTypeface(font));
    // sub test 2, OH_Drawing_FontCountText
    const char* str = "123456";
    int count = 0;
    count = OH_Drawing_FontCountText(nullptr, str, strlen(str),
        OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8);
    EXPECT_EQ(0, count);
    count = OH_Drawing_FontCountText(font, nullptr, strlen(str),
        OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8);
    EXPECT_EQ(0, count);
    count = OH_Drawing_FontCountText(font, str, strlen(str),
        OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8);
    EXPECT_EQ(strlen(str), count);
    // sub test 3, OH_Drawing_TextBlobCreateFromText
    EXPECT_EQ(nullptr, OH_Drawing_TextBlobCreateFromText(nullptr, strlen(str),
        font, OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8));
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(nullptr, OH_Drawing_TextBlobCreateFromText(str, strlen(str),
        nullptr, OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8));
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_TextBlobCreateFromText(str, strlen(str), font, static_cast<OH_Drawing_TextEncoding>(NEGATIVE_ONE));
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE);
    OH_Drawing_TextBlobCreateFromText(str, strlen(str), font, static_cast<OH_Drawing_TextEncoding>(INTNUM_TEN));
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE);
    OH_Drawing_TextBlob *textBlob = OH_Drawing_TextBlobCreateFromText(str, strlen(str),
        font, OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8);
    EXPECT_NE(textBlob, nullptr);
    EXPECT_TRUE(OH_Drawing_TextBlobUniqueID(nullptr) == 0);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_TRUE(OH_Drawing_TextBlobUniqueID(textBlob) > 0);
    // draw textblob
    OH_Drawing_CanvasDrawTextBlob(canvas_, textBlob, 0, 0);

    OH_Drawing_TextBlobDestroy(textBlob);
    OH_Drawing_FontDestroy(font);
    OH_Drawing_TypefaceDestroy(typeSurface);
}

/*
 * @tc.name: NativeDrawingCanvasTest_DrawTextBlob036
 * @tc.desc: test for DrawTextBlob4
 * @tc.type: FUNC
 * @tc.require: SR000S9F0C
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_DrawTextBlob036, TestSize.Level1)
{
    size_t length = 1;
    OH_Drawing_Font *font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    OH_Drawing_MemoryStream* memoryStream = OH_Drawing_MemoryStreamCreate(nullptr,
        length, false);
    OH_Drawing_MemoryStreamDestroy(memoryStream);
    EXPECT_EQ(nullptr, memoryStream);
    OH_Drawing_Typeface* typeface = OH_Drawing_TypefaceCreateFromStream(
        memoryStream, 0);
    EXPECT_EQ(nullptr, typeface);
    OH_Drawing_Typeface *typeSurface = OH_Drawing_TypefaceCreateDefault();
    OH_Drawing_FontSetTypeface(font, typeSurface);
    EXPECT_NE(nullptr, OH_Drawing_FontGetTypeface(font));
    const char* str = "123456";
    int count = strlen(str);
    OH_Drawing_Point2D pts[count];
    EXPECT_EQ(nullptr, OH_Drawing_TextBlobCreateFromPosText(nullptr, count, &pts[0],
        font, OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8));
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(nullptr, OH_Drawing_TextBlobCreateFromPosText(str, count, nullptr,
        font, OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8));
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(nullptr, OH_Drawing_TextBlobCreateFromPosText(str, count, &pts[0],
        nullptr, OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8));
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_TextBlobCreateFromPosText(str, count, &pts[0], font, static_cast<OH_Drawing_TextEncoding>(NEGATIVE_ONE));
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE);
    OH_Drawing_TextBlobCreateFromPosText(str, count, &pts[0], font, static_cast<OH_Drawing_TextEncoding>(INTNUM_TEN));
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE);
    OH_Drawing_TextBlob *textBlob = OH_Drawing_TextBlobCreateFromPosText(str, count, &pts[0],
        font, OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8);
    EXPECT_NE(textBlob, nullptr);
    OH_Drawing_CanvasDrawTextBlob(canvas_, textBlob, 0, 0);

    OH_Drawing_TextBlobDestroy(textBlob);
    OH_Drawing_FontDestroy(font);
    OH_Drawing_TypefaceDestroy(typeSurface);
}

/*
 * @tc.name: NativeDrawingCanvasTest_SaveLayer037
 * @tc.desc: test for SaveLayer
 * @tc.type: FUNC
 * @tc.require: SR000S9F0C
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_SaveLayer037, TestSize.Level1)
{
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(200, 500, 300, 600);
    EXPECT_NE(rect, nullptr);
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    EXPECT_NE(brush, nullptr);
    // test exception
    OH_Drawing_CanvasSaveLayer(nullptr, rect, brush);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_CanvasSaveLayer(canvas_, rect, brush);
    OH_Drawing_CanvasRestore(canvas_);
    OH_Drawing_RectDestroy(rect);
}

/*
 * @tc.name: NativeDrawingCanvasTest_ClipRoundRect
 * @tc.desc: test for OH_Drawing_ClipRoundRect.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_ClipRoundRect, TestSize.Level1)
{
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(10, 100, 200, 300);
    OH_Drawing_RoundRect* roundRect = OH_Drawing_RoundRectCreate(rect, 10, 10);
    for (auto alias : {false, true}) {
        OH_Drawing_CanvasClipRoundRect(nullptr, roundRect, OH_Drawing_CanvasClipOp::INTERSECT, alias);
        EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
        OH_Drawing_CanvasClipRoundRect(canvas_, nullptr, OH_Drawing_CanvasClipOp::INTERSECT, alias);
        EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
        OH_Drawing_CanvasClipRoundRect(canvas_, roundRect, OH_Drawing_CanvasClipOp::INTERSECT, alias);
    }
    OH_Drawing_CanvasDrawRect(canvas_, rect);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_RoundRectDestroy(roundRect);
}

/*
 * @tc.name: NativeDrawingCanvasTest_RoundRectGetCorner
 * @tc.desc: test for OH_Drawing_RoundRectGetCorner.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_RoundRectGetCorner, TestSize.Level1)
{
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(10, 100, 200, 300);
    OH_Drawing_RoundRect* roundRect = OH_Drawing_RoundRectCreate(rect, 0, 0);
    OH_Drawing_RoundRectSetCorner(nullptr, OH_Drawing_CornerPos::CORNER_POS_BOTTOM_LEFT, {10, 10});
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_RoundRectSetCorner(roundRect, OH_Drawing_CornerPos::CORNER_POS_BOTTOM_LEFT, {10, 10});
    OH_Drawing_RoundRectGetCorner(nullptr, OH_Drawing_CornerPos::CORNER_POS_BOTTOM_LEFT);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_Corner_Radii radiusPoint =
        OH_Drawing_RoundRectGetCorner(roundRect, OH_Drawing_CornerPos::CORNER_POS_BOTTOM_LEFT);
    EXPECT_EQ(10.0f, radiusPoint.x);
    EXPECT_EQ(10.0f, radiusPoint.y);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_RoundRectDestroy(roundRect);
}

/*
 * @tc.name: NativeDrawingCanvasTest_RoundRectSetCorner
 * @tc.desc: test for OH_Drawing_RoundRectSetCorner.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_RoundRectSetCorner, TestSize.Level1)
{
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(10, 100, 200, 300);
    OH_Drawing_RoundRect* roundRect = OH_Drawing_RoundRectCreate(rect, 0, 0);
    OH_Drawing_RoundRectSetCorner(nullptr, OH_Drawing_CornerPos::CORNER_POS_BOTTOM_LEFT, {10, 10});
    OH_Drawing_RoundRectSetCorner(roundRect, OH_Drawing_CornerPos::CORNER_POS_BOTTOM_LEFT, {10, 10});
    OH_Drawing_Corner_Radii corner = OH_Drawing_RoundRectGetCorner(roundRect,
        OH_Drawing_CornerPos::CORNER_POS_BOTTOM_LEFT);
    EXPECT_EQ(corner.x, 10);
    OH_Drawing_CanvasClipRoundRect(canvas_, roundRect, OH_Drawing_CanvasClipOp::INTERSECT, true);
    OH_Drawing_CanvasDrawRect(canvas_, rect);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_RoundRectDestroy(roundRect);
}

/*
 * @tc.name: NativeDrawingCanvasTest_DrawRegion038
 * @tc.desc: test for DrawRegion
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_DrawRegion038, TestSize.Level1)
{
    OH_Drawing_Region* region=OH_Drawing_RegionCreate();
    OH_Drawing_Rect* rect=OH_Drawing_RectCreate(0.0f, 0.0f, 256.0f, 256.0f);
    OH_Drawing_RegionSetRect(region, rect);
    OH_Drawing_CanvasDrawRegion(nullptr, region);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_CanvasDrawRegion(canvas_, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_CanvasDrawRegion(canvas_, region);
    OH_Drawing_RegionDestroy(region);
    OH_Drawing_RectDestroy(rect);
}

/*
 * @tc.name: NativeDrawingCanvasTest_DrawBackground039
 * @tc.desc: test for DrawBackground
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_DrawBackground039, TestSize.Level1)
{
    OH_Drawing_Brush* brush = OH_Drawing_BrushCreate();
    OH_Drawing_BrushSetColor(brush, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0x00));
    OH_Drawing_CanvasDrawBackground(nullptr, brush);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_CanvasDrawBackground(canvas_, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_CanvasDrawBackground(canvas_, brush);
    OH_Drawing_BrushDestroy(brush);
}

/*
 * @tc.name: NativeDrawingCanvasTest_DrawPixelMapRect040
 * @tc.desc: test for DrawPixelMapRect
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_DrawPixelMapRect040, TestSize.Level1)
{
    OH_Pixelmap_InitializationOptions *options = nullptr;
    OH_PixelmapNative *pixelMap = nullptr;
    OH_PixelmapInitializationOptions_Create(&options);
    // 4 means width
    OH_PixelmapInitializationOptions_SetWidth(options, 4);
    // 4 means height
    OH_PixelmapInitializationOptions_SetHeight(options, 4);
    // 4 means RGBA format
    OH_PixelmapInitializationOptions_SetPixelFormat(options, 3);
    // 2 means ALPHA_FORMAT_PREMUL format
    OH_PixelmapInitializationOptions_SetAlphaType(options, 2);
    // 255 means rgba data
    uint8_t data[] = {
        255, 255, 0, 255,
        255, 255, 0, 255,
        255, 255, 0, 255,
        255, 255, 0, 255
    };
    // 16 means data length
    size_t dataLength = 16;
    OH_PixelmapNative_CreatePixelmap(data, dataLength, options, &pixelMap);
    EXPECT_NE(pixelMap, nullptr);
    OH_Drawing_PixelMap *drPixelMap = OH_Drawing_PixelMapGetFromOhPixelMapNative(pixelMap);
    EXPECT_NE(drPixelMap, nullptr);
    OH_Drawing_Rect* srcRect = OH_Drawing_RectCreate(0, 0, 4, 4);
    OH_Drawing_Rect* dstRect = OH_Drawing_RectCreate(0, 0, 4, 4);
    OH_Drawing_SamplingOptions* samplingOptions = OH_Drawing_SamplingOptionsCreate(
        OH_Drawing_FilterMode::FILTER_MODE_NEAREST, OH_Drawing_MipmapMode::MIPMAP_MODE_NEAREST);
    EXPECT_NE(samplingOptions, nullptr);
    OH_Drawing_CanvasDrawPixelMapRect(canvas_, drPixelMap, srcRect, dstRect, samplingOptions);
    OH_Drawing_CanvasDrawPixelMapRect(canvas_, drPixelMap, srcRect, dstRect, nullptr);
    OH_Drawing_CanvasDrawPixelMapRect(canvas_, drPixelMap, srcRect, nullptr, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_CanvasDrawPixelMapRect(canvas_, drPixelMap, nullptr, nullptr, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_CanvasDrawPixelMapRect(canvas_, nullptr, nullptr, nullptr, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_CanvasDrawPixelMapRect(nullptr, nullptr, nullptr, nullptr, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_PixelMapDissolve(drPixelMap);
    OH_PixelmapNative_Release(pixelMap);
    OH_PixelmapInitializationOptions_Release(options);
    OH_Drawing_RectDestroy(srcRect);
    OH_Drawing_RectDestroy(dstRect);
    OH_Drawing_SamplingOptionsDestroy(samplingOptions);
}

/*
 * @tc.name: NativeDrawingCanvasTest_DrawPixelMapRect041
 * @tc.desc: test for DrawPixelMapRect
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_DrawPixelMapRect041, TestSize.Level1)
{
    OH_Pixelmap_InitializationOptions *options = nullptr;
    OH_PixelmapNative *pixelMap = nullptr;
    OH_PixelmapInitializationOptions_Create(&options);
    // 4 means width
    OH_PixelmapInitializationOptions_SetWidth(options, 4);
    // 4 means height
    OH_PixelmapInitializationOptions_SetHeight(options, 4);
    // 4 means RGBA format
    OH_PixelmapInitializationOptions_SetPixelFormat(options, 3);
    // 2 means ALPHA_FORMAT_PREMUL format
    OH_PixelmapInitializationOptions_SetAlphaType(options, 2);
    // 255 means rgba data
    uint8_t data[] = {
        255, 255, 0, 255,
        255, 255, 0, 255,
        255, 255, 0, 255,
        255, 255, 0, 255
    };
    // 16 means data length
    size_t dataLength = 16;
    OH_PixelmapNative_CreatePixelmap(data, dataLength, options, &pixelMap);
    EXPECT_NE(pixelMap, nullptr);
    OH_Drawing_PixelMap *drPixelMap = OH_Drawing_PixelMapGetFromOhPixelMapNative(pixelMap);
    EXPECT_NE(drPixelMap, nullptr);
    auto recordingCanvas = new RecordingCanvas(100, 100);
    OH_Drawing_Canvas *cCanvas = reinterpret_cast<OH_Drawing_Canvas*>(recordingCanvas);
    EXPECT_NE(cCanvas, nullptr);
    OH_Drawing_Rect* srcRect = OH_Drawing_RectCreate(0, 0, 4, 4);
    OH_Drawing_Rect* dstRect = OH_Drawing_RectCreate(0, 0, 4, 4);
    OH_Drawing_SamplingOptions* samplingOptions = OH_Drawing_SamplingOptionsCreate(
        OH_Drawing_FilterMode::FILTER_MODE_NEAREST, OH_Drawing_MipmapMode::MIPMAP_MODE_NEAREST);
    EXPECT_NE(samplingOptions, nullptr);
    OH_Drawing_CanvasDrawPixelMapRect(cCanvas, drPixelMap, srcRect, dstRect, samplingOptions);
    OH_Drawing_CanvasDrawPixelMapRect(cCanvas, drPixelMap, srcRect, dstRect, nullptr);
    OH_Drawing_CanvasDrawPixelMapRect(cCanvas, drPixelMap, srcRect, nullptr, nullptr);
    OH_Drawing_CanvasDrawPixelMapRect(cCanvas, drPixelMap, nullptr, nullptr, nullptr);
    OH_Drawing_CanvasDrawPixelMapRect(cCanvas, nullptr, nullptr, nullptr, nullptr);
    OH_Drawing_CanvasDrawPixelMapRect(nullptr, nullptr, nullptr, nullptr, nullptr);
    auto drawCmdList = recordingCanvas->GetDrawCmdList();
    EXPECT_NE(drawCmdList, nullptr);
    Canvas canvas;
    drawCmdList->Playback(canvas);
    OH_Drawing_PixelMapDissolve(drPixelMap);
    OH_PixelmapNative_Release(pixelMap);
    OH_PixelmapInitializationOptions_Release(options);
    OH_Drawing_RectDestroy(srcRect);
    OH_Drawing_RectDestroy(dstRect);
    OH_Drawing_SamplingOptionsDestroy(samplingOptions);
    delete recordingCanvas;
}

/*
 * @tc.name: NativeDrawingCanvasTest_IsClipEmpty042
 * @tc.desc: test for if clip is empty
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_IsClipEmpty042, TestSize.Level1)
{
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat cFormat{COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE};
    // 720: bitmap's width, 720: bitmap's height
    constexpr uint32_t width = 720;
    constexpr uint32_t height = 720;
    OH_Drawing_BitmapBuild(bitmap, width, height, &cFormat);
    OH_Drawing_CanvasBind(canvas_, bitmap);

    // 150.0f: rect's left, 100.0f: rect's top, 500.0f: rect's right, 500.0f: rect's bottom
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(150.0f, 100.0f, 500.f, 500.f);

    OH_Drawing_CanvasClipRect(canvas_, rect, OH_Drawing_CanvasClipOp::INTERSECT, false);

    bool isClipEmpty = false;
    EXPECT_EQ(OH_Drawing_CanvasIsClipEmpty(canvas_, &isClipEmpty), OH_DRAWING_SUCCESS);
    EXPECT_EQ(isClipEmpty, false);

    OH_Drawing_RectDestroy(rect);
    OH_Drawing_BitmapDestroy(bitmap);
}

/*
 * @tc.name: NativeDrawingCanvasTest_GetImageInfo043
 * @tc.desc: test for Gets ImageInfo of Canvas.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_GetImageInfo043, TestSize.Level1)
{
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat cFormat{COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE};
    // 720: bitmap's width, 720: bitmap's height
    constexpr uint32_t width = 720;
    constexpr uint32_t height = 720;
    OH_Drawing_BitmapBuild(bitmap, width, height, &cFormat);
    OH_Drawing_CanvasBind(canvas_, bitmap);
    OH_Drawing_Image_Info* imageInfo = new OH_Drawing_Image_Info();
    EXPECT_EQ(OH_Drawing_CanvasGetImageInfo(canvas_, imageInfo), OH_DRAWING_SUCCESS);
    EXPECT_EQ(720, imageInfo->width);
    EXPECT_EQ(720, imageInfo->height);
    EXPECT_EQ(1, imageInfo->alphaType);
    EXPECT_EQ(4, imageInfo->colorType);
    delete imageInfo;
}

/*
 * @tc.name: NativeDrawingCanvasTest_ClipRegion044
 * @tc.desc: test for Drawing Canvas Clip Region.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_ClipRegion044, TestSize.Level1)
{
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat cFormat{COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE};
    constexpr uint32_t width = 720; // 720: width of canvas
    constexpr uint32_t height = 720; // 720: height of canvas
    OH_Drawing_BitmapBuild(bitmap, width, height, &cFormat);
    OH_Drawing_CanvasBind(canvas_, bitmap);

    OH_Drawing_Region *region = OH_Drawing_RegionCreate();
    // 0.0f: rect's left, 0.0f: rect's top, 720.0f: rect's right, 720.0f: rect's bottom
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(0.0f, 0.0f, 720.f, 720.f);
    OH_Drawing_RegionSetRect(region, rect);
    EXPECT_EQ(OH_Drawing_CanvasClipRegion(canvas_, region, OH_Drawing_CanvasClipOp::INTERSECT), OH_DRAWING_SUCCESS);
    EXPECT_EQ(OH_Drawing_CanvasClipRegion(nullptr, region, OH_Drawing_CanvasClipOp::INTERSECT),
        OH_DRAWING_ERROR_INVALID_PARAMETER);

    EXPECT_EQ(720, OH_Drawing_CanvasGetWidth(canvas_));
    EXPECT_EQ(720, OH_Drawing_CanvasGetHeight(canvas_));

    OH_Drawing_RegionDestroy(region);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_BitmapDestroy(bitmap);
}

/*
 * @tc.name: NativeDrawingCanvasTest_DrawPoint_045
 * @tc.desc: test for OH_Drawing_CanvasDrawPoint.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_DrawPoint_045, TestSize.Level1)
{
    OH_Drawing_Point2D point_ = {25.0f, 36.0f}; // 25.0f: x, 36.0f: y
    EXPECT_EQ(OH_Drawing_CanvasDrawPoint(canvas_, nullptr), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_CanvasDrawPoint(nullptr, &point_), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_CanvasDrawPoint(canvas_, &point_), OH_DRAWING_SUCCESS);
}

/*
 * @tc.name: NativeDrawingCanvasTest_DrawColor046
 * @tc.desc: test for DrawColor
 * @tc.type: FUNC
 * @tc.require: SR000S9F0C
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_DrawColor046, TestSize.Level1)
{
    EXPECT_EQ(OH_Drawing_CanvasDrawColor(nullptr, 0xFFFF0000, OH_Drawing_BlendMode::BLEND_MODE_SRC),
        OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_CanvasDrawColor(canvas_, 0xFFFF0000, OH_Drawing_BlendMode::BLEND_MODE_COLOR),
        OH_DRAWING_SUCCESS);
}

/*
 * @tc.name: NativeDrawingCanvasTest_DrawTextBlob047
 * @tc.desc: test for DrawTextBlob
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_DrawTextBlob047, TestSize.Level1)
{
    OH_Drawing_Font *font = OH_Drawing_FontCreate();
    auto *builder = OH_Drawing_TextBlobBuilderCreate();
    EXPECT_EQ(nullptr, OH_Drawing_TextBlobBuilderAllocRunPos(nullptr, nullptr, INT32_MAX, nullptr));
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(nullptr, OH_Drawing_TextBlobBuilderAllocRunPos(builder, nullptr, INT32_MAX, nullptr));
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(nullptr, OH_Drawing_TextBlobBuilderAllocRunPos(builder, font, 0, nullptr));
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_TextBlobBuilderDestroy(builder);
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: NativeDrawingCanvasTest_ImageFilterCreateBlur041
 * @tc.desc: test for Creates an OH_Drawing_ImageFilter object that blurs its input by the separate x and y sigmas.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
*/
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_ImageFilterCreateBlur041, TestSize.Level1)
{
    float sifmaX = 0.f;
    float sigmaY = 0.f;
    OH_Drawing_ImageFilter *imagefilter = nullptr;
    imagefilter = OH_Drawing_ImageFilterCreateBlur(sifmaX, sigmaY, CLAMP, nullptr);
    EXPECT_NE(imagefilter, nullptr);
    OH_Drawing_ImageFilterDestroy(imagefilter);
}

/*
 * @tc.name: NativeDrawingCanvasTest_ImageFilterCreateFromColorFilter042
 * @tc.desc: test for Creates an OH_Drawing_ImageFilter object that applies the color filter to the input.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_ImageFilterCreateFromColorFilter042, TestSize.Level1)
{
    float sifmaX = 0.f;
    float sigmaY = 0.f;
    OH_Drawing_ImageFilter *imagefilter = nullptr;
    imagefilter = OH_Drawing_ImageFilterCreateBlur(sifmaX, sigmaY, CLAMP, nullptr);
    EXPECT_NE(imagefilter, nullptr);
    EXPECT_EQ(OH_Drawing_BrushGetColor(brush_), 0xFFFF0000);
    OH_Drawing_ColorFilter* colorFilter = OH_Drawing_ColorFilterCreateBlendMode(0xFF0000FF,
        OH_Drawing_BlendMode::BLEND_MODE_SRC);
    EXPECT_NE(colorFilter, nullptr);
    OH_Drawing_ColorFilter* colorFilterTmp = OH_Drawing_ColorFilterCreateLinearToSrgbGamma();
    EXPECT_NE(colorFilterTmp, nullptr);
    OH_Drawing_ImageFilter *imagefiltercreatefromc = nullptr;
    imagefiltercreatefromc = OH_Drawing_ImageFilterCreateFromColorFilter(nullptr, nullptr);
    EXPECT_EQ(imagefiltercreatefromc, nullptr);
    imagefiltercreatefromc = OH_Drawing_ImageFilterCreateFromColorFilter(colorFilterTmp, imagefilter);
    EXPECT_NE(imagefiltercreatefromc, nullptr);
    OH_Drawing_ColorFilterDestroy(colorFilter);
    OH_Drawing_ColorFilterDestroy(colorFilterTmp);
    OH_Drawing_ImageFilterDestroy(imagefiltercreatefromc);
    OH_Drawing_ImageFilterDestroy(imagefilter);
}

/*
 * @tc.name: NativeDrawingCanvasTest_ImageFilterDestroy043
 * @tc.desc: test for Destroys an OH_Drawing_ImageFilter object and reclaims the memory occupied by the object.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_ImageFilterDestroy043, TestSize.Level1)
{
    float sifmaX = 0.f;
    float sigmaY = 0.f;
    OH_Drawing_ImageFilter *imagefilter = nullptr;
    imagefilter = OH_Drawing_ImageFilterCreateBlur(sifmaX, sigmaY, CLAMP, nullptr);
    EXPECT_NE(imagefilter, nullptr);
    OH_Drawing_ImageFilterDestroy(nullptr);
    OH_Drawing_ImageFilterDestroy(imagefilter);
}

/*
 * @tc.name: NativeDrawingCanvasTest_FilterSetImageFilter044
 * @tc.desc: test for Sets an OH_Drawing_ImageFilter object for an OH_Drawing_Filter object.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_FilterSetImageFilter044, TestSize.Level1)
{
    float sifmaX = 0.f;
    float sigmaY = 0.f;
    OH_Drawing_ImageFilter *imagefilter = OH_Drawing_ImageFilterCreateBlur(sifmaX, sigmaY, CLAMP, nullptr);
    EXPECT_NE(imagefilter, nullptr);
    OH_Drawing_Filter* filter = OH_Drawing_FilterCreate();
    OH_Drawing_FilterSetImageFilter(nullptr, imagefilter);
    OH_Drawing_FilterSetImageFilter(filter, nullptr);
    OH_Drawing_FilterSetImageFilter(filter, imagefilter);
    OH_Drawing_FilterDestroy(filter);
    OH_Drawing_ImageFilterDestroy(imagefilter);
}

/*
 * @tc.name: NativeDrawingCanvasTest_CanvasDrawSingleCharacter045
 * @tc.desc: test for OH_Drawing_CanvasDrawSingleCharacter.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_CanvasDrawSingleCharacter045, TestSize.Level1)
{
    const char* strOne = "a";
    const char* strTwo = "你好";
    OH_Drawing_Font *font = OH_Drawing_FontCreate();
    EXPECT_NE(font, nullptr);
    float x = 0.f;
    float y = 0.f;
    EXPECT_EQ(OH_Drawing_CanvasDrawSingleCharacter(canvas_, strOne, font, x, y), OH_DRAWING_SUCCESS);
    EXPECT_EQ(OH_Drawing_CanvasDrawSingleCharacter(canvas_, strTwo, font, x, y), OH_DRAWING_SUCCESS);
    EXPECT_EQ(OH_Drawing_CanvasDrawSingleCharacter(nullptr, strOne, font, x, y), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_CanvasDrawSingleCharacter(canvas_, nullptr, font, x, y), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_CanvasDrawSingleCharacter(canvas_, strOne, nullptr, x, y),
        OH_DRAWING_ERROR_INVALID_PARAMETER);
    const char* strThree = "";
    EXPECT_EQ(OH_Drawing_CanvasDrawSingleCharacter(canvas_, strThree, font, x, y), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_FontDestroy(font);
}

/*
 * @tc.name: NativeDrawingCanvasTest_CanvasDrawRecordCmd001
 * @tc.desc: test for OH_Drawing_CanvasDrawRecordCmd.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_CanvasDrawRecordCmd001, TestSize.Level1)
{
    int32_t width = 10; // canvas width is 10
    int32_t height = 20; // canvas width is 20
    OH_Drawing_Canvas* canvas = nullptr;
    OH_Drawing_RecordCmdUtils* recordCmdUtils = OH_Drawing_RecordCmdUtilsCreate();
    EXPECT_TRUE(recordCmdUtils != nullptr);
    OH_Drawing_ErrorCode code = OH_Drawing_RecordCmdUtilsBeginRecording(recordCmdUtils, width, height, &canvas);
    EXPECT_TRUE(canvas != nullptr);
    EXPECT_EQ(code, OH_DRAWING_SUCCESS);
    int32_t width1 = OH_Drawing_CanvasGetWidth(canvas);
    int32_t height2 = OH_Drawing_CanvasGetHeight(canvas);
    EXPECT_TRUE(width1 == width);
    EXPECT_TRUE(height2 == height);
    OH_Drawing_RecordCmd* recordCmd = nullptr;
    code = OH_Drawing_RecordCmdUtilsFinishRecording(recordCmdUtils, &recordCmd);
    EXPECT_TRUE(recordCmd != nullptr);
    EXPECT_EQ(code, OH_DRAWING_SUCCESS);
    OH_Drawing_Canvas* recordCanvas = OH_Drawing_CanvasCreate();
    EXPECT_TRUE(recordCanvas != nullptr);
    code = OH_Drawing_CanvasDrawRecordCmd(recordCanvas, recordCmd);
    EXPECT_EQ(code, OH_DRAWING_SUCCESS);
    code = OH_Drawing_CanvasDrawRecordCmd(nullptr, recordCmd);
    EXPECT_EQ(code, OH_DRAWING_ERROR_INVALID_PARAMETER);
    code = OH_Drawing_CanvasDrawRecordCmd(recordCanvas, nullptr);
    EXPECT_EQ(code, OH_DRAWING_ERROR_INVALID_PARAMETER);
    code = OH_Drawing_CanvasDrawRecordCmd(nullptr, nullptr);
    EXPECT_EQ(code, OH_DRAWING_ERROR_INVALID_PARAMETER);
    code = OH_Drawing_RecordCmdDestroy(recordCmd);
    EXPECT_EQ(code, OH_DRAWING_SUCCESS);
    code = OH_Drawing_RecordCmdUtilsDestroy(recordCmdUtils);
    EXPECT_EQ(code, OH_DRAWING_SUCCESS);
    OH_Drawing_CanvasDestroy(recordCanvas);
}

/*
 * @tc.name: NativeDrawingCanvasTest_CanvasDrawRecordCmdNesting001
 * @tc.desc: test for OH_Drawing_CanvasDrawRecordCmdNesting.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_CanvasDrawRecordCmdNesting001, TestSize.Level1)
{
    int32_t width = 10; // canvas width is 10
    int32_t height = 20; // canvas width is 20
    OH_Drawing_Canvas* recordCanvas1 = nullptr;
    OH_Drawing_RecordCmdUtils* recordCmdUtils1 = OH_Drawing_RecordCmdUtilsCreate();
    EXPECT_TRUE(recordCmdUtils1 != nullptr);
    OH_Drawing_ErrorCode code = OH_Drawing_RecordCmdUtilsBeginRecording(recordCmdUtils1, width, height, &recordCanvas1);
    EXPECT_TRUE(recordCanvas1 != nullptr);
    EXPECT_EQ(code, OH_DRAWING_SUCCESS);
    OH_Drawing_CanvasDrawLine(recordCanvas1, 0, 0, static_cast<float>(width), static_cast<float>(height));
    OH_Drawing_RecordCmd* recordCmd1 = nullptr;
    code = OH_Drawing_RecordCmdUtilsFinishRecording(recordCmdUtils1, &recordCmd1);
    EXPECT_TRUE(recordCmd1 != nullptr);
    EXPECT_EQ(code, OH_DRAWING_SUCCESS);
    OH_Drawing_RecordCmdUtilsDestroy(recordCmdUtils1);

    OH_Drawing_Canvas* recordCanvas2 = nullptr;
    OH_Drawing_RecordCmdUtils* recordCmdUtils2 = OH_Drawing_RecordCmdUtilsCreate();
    EXPECT_TRUE(recordCmdUtils2 != nullptr);
    code = OH_Drawing_RecordCmdUtilsBeginRecording(recordCmdUtils2, width, height, &recordCanvas2);
    EXPECT_TRUE(recordCanvas2 != nullptr);
    EXPECT_EQ(code, OH_DRAWING_SUCCESS);
    OH_Drawing_CanvasDrawLine(recordCanvas2, static_cast<float>(width), 0, 0, static_cast<float>(height));
    code = OH_Drawing_CanvasDrawRecordCmdNesting(recordCanvas2, recordCmd1);
    EXPECT_EQ(code, OH_DRAWING_SUCCESS);
    OH_Drawing_RecordCmd* recordCmd2 = nullptr;
    code = OH_Drawing_RecordCmdUtilsFinishRecording(recordCmdUtils2, &recordCmd2);
    EXPECT_TRUE(recordCmd2 != nullptr);
    EXPECT_EQ(code, OH_DRAWING_SUCCESS);
    OH_Drawing_RecordCmdUtilsDestroy(recordCmdUtils2);
    // test exception
    OH_Drawing_Canvas* recordCanvas = OH_Drawing_CanvasCreate();
    EXPECT_TRUE(recordCanvas != nullptr);
    code = OH_Drawing_CanvasDrawRecordCmdNesting(recordCanvas, recordCmd1);
    EXPECT_EQ(code, OH_DRAWING_SUCCESS);
    code = OH_Drawing_CanvasDrawRecordCmdNesting(nullptr, recordCmd1);
    EXPECT_EQ(code, OH_DRAWING_ERROR_INVALID_PARAMETER);
    code = OH_Drawing_CanvasDrawRecordCmdNesting(recordCanvas, nullptr);
    EXPECT_EQ(code, OH_DRAWING_ERROR_INVALID_PARAMETER);
    code = OH_Drawing_CanvasDrawRecordCmdNesting(nullptr, nullptr);
    EXPECT_EQ(code, OH_DRAWING_ERROR_INVALID_PARAMETER);

    OH_Drawing_CanvasDestroy(recordCanvas);
    OH_Drawing_RecordCmdDestroy(recordCmd1);
    OH_Drawing_RecordCmdDestroy(recordCmd2);
}

/*
 * @tc.name: NativeDrawingCanvasTest_CanvasDrawRecordCmdNesting002
 * @tc.desc: test for OH_Drawing_CanvasDrawRecordCmdNesting.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_CanvasDrawRecordCmdNesting002, TestSize.Level1)
{
    int32_t width = 10; // canvas width is 10
    int32_t height = 20; // canvas width is 20
    OH_Drawing_Canvas* recordCanvas1 = nullptr;
    OH_Drawing_RecordCmdUtils* recordCmdUtils1 = OH_Drawing_RecordCmdUtilsCreate();
    EXPECT_TRUE(recordCmdUtils1 != nullptr);
    OH_Drawing_ErrorCode code = OH_Drawing_RecordCmdUtilsBeginRecording(recordCmdUtils1, width, height, &recordCanvas1);
    EXPECT_TRUE(recordCanvas1 != nullptr);
    EXPECT_EQ(code, OH_DRAWING_SUCCESS);
    OH_Drawing_CanvasDrawLine(recordCanvas1, 0, 0, static_cast<float>(width), static_cast<float>(height));
    OH_Drawing_RecordCmd* recordCmd1 = nullptr;
    code = OH_Drawing_RecordCmdUtilsFinishRecording(recordCmdUtils1, &recordCmd1);
    EXPECT_TRUE(recordCmd1 != nullptr);
    EXPECT_EQ(code, OH_DRAWING_SUCCESS);
    OH_Drawing_RecordCmdUtilsDestroy(recordCmdUtils1);

    OH_Drawing_Canvas* recordCanvas2 = nullptr;
    OH_Drawing_RecordCmdUtils* recordCmdUtils2 = OH_Drawing_RecordCmdUtilsCreate();
    EXPECT_TRUE(recordCmdUtils2 != nullptr);
    code = OH_Drawing_RecordCmdUtilsBeginRecording(recordCmdUtils2, width, height, &recordCanvas2);
    EXPECT_TRUE(recordCanvas2 != nullptr);
    EXPECT_EQ(code, OH_DRAWING_SUCCESS);
    OH_Drawing_CanvasDrawLine(recordCanvas2, static_cast<float>(width), 0, 0, static_cast<float>(height));
    code = OH_Drawing_CanvasDrawRecordCmdNesting(recordCanvas2, recordCmd1);
    EXPECT_EQ(code, OH_DRAWING_SUCCESS);
    OH_Drawing_RecordCmd* recordCmd2 = nullptr;
    // test beginRecording repeatedly
    code = OH_Drawing_RecordCmdUtilsBeginRecording(recordCmdUtils2, width, height, &recordCanvas2);
    EXPECT_EQ(code, OH_DRAWING_SUCCESS);
    code = OH_Drawing_CanvasDrawRecordCmdNesting(recordCanvas2, recordCmd1);
    EXPECT_EQ(code, OH_DRAWING_SUCCESS);
    // test DrawRecordCmdNesting repeatedly
    code = OH_Drawing_CanvasDrawRecordCmdNesting(recordCanvas2, recordCmd1);
    EXPECT_EQ(code, OH_DRAWING_SUCCESS);
    // test DrawRecordCmdNesting and DrawRecordCmdNest
    code = OH_Drawing_CanvasDrawRecordCmd(recordCanvas2, recordCmd1);
    EXPECT_EQ(code, OH_DRAWING_SUCCESS);
    OH_Drawing_RecordCmdDestroy(recordCmd1);
    code = OH_Drawing_RecordCmdUtilsFinishRecording(recordCmdUtils2, &recordCmd2);
    EXPECT_TRUE(recordCmd2 != nullptr);
    EXPECT_EQ(code, OH_DRAWING_SUCCESS);
    OH_Drawing_RecordCmdUtilsDestroy(recordCmdUtils2);
    OH_Drawing_RecordCmdDestroy(recordCmd2);
}

/*
 * @tc.name: NativeOH_Drawing_CanvasQuickRejectPath001
 * @tc.desc: test for OH_Drawing_CanvasQuickRejectPath.
 * @tc.type: FUNC
 * @tc.require: IBHFF5
 */
HWTEST_F(NativeDrawingCanvasTest, NativeOH_Drawing_CanvasQuickRejectPath001, TestSize.Level1)
{
    OH_Drawing_Canvas* canvas = OH_Drawing_CanvasCreate();
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat cFormat{COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE};
    constexpr uint32_t width = 200; // 200 means bitmap width
    constexpr uint32_t height = 200; // 200 means bitmap height
    OH_Drawing_BitmapBuild(bitmap, width, height, &cFormat);
    OH_Drawing_CanvasBind(canvas, bitmap);
    bool quickReject = false;
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    EXPECT_TRUE(path != nullptr);
    auto result = OH_Drawing_CanvasQuickRejectPath(nullptr, path, &quickReject);
    EXPECT_EQ(result, OH_DRAWING_ERROR_INVALID_PARAMETER);
    result = OH_Drawing_CanvasQuickRejectPath(canvas, nullptr, &quickReject);
    EXPECT_EQ(result, OH_DRAWING_ERROR_INVALID_PARAMETER);
    result = OH_Drawing_CanvasQuickRejectPath(canvas, path, nullptr);
    EXPECT_EQ(result, OH_DRAWING_ERROR_INVALID_PARAMETER);
    // rect left[50], top[50],right[250], bottom[250]
    OH_Drawing_PathAddRect(path, 50, 50, 250, 250, OH_Drawing_PathDirection::PATH_DIRECTION_CW);
    result = OH_Drawing_CanvasQuickRejectPath(canvas, path, &quickReject);
    EXPECT_EQ(result, OH_DRAWING_SUCCESS);
    EXPECT_FALSE(quickReject);
    OH_Drawing_Path* path2 = OH_Drawing_PathCreate();
    // rect left[300], top[350],right[450], bottom[550]
    OH_Drawing_PathAddRect(path2, 300, 350, 450, 550, OH_Drawing_PathDirection::PATH_DIRECTION_CW);
    result = OH_Drawing_CanvasQuickRejectPath(canvas, path2, &quickReject);
    EXPECT_EQ(result, OH_DRAWING_SUCCESS);
    EXPECT_TRUE(quickReject);
    OH_Drawing_Path* path3 = OH_Drawing_PathCreate();
    // rect left[-100], top[-100],right[0], bottom[0]
    OH_Drawing_PathAddRect(path3, -100, -100, 0, 0, OH_Drawing_PathDirection::PATH_DIRECTION_CW);
    result = OH_Drawing_CanvasQuickRejectPath(canvas, path3, &quickReject);
    EXPECT_EQ(result, OH_DRAWING_SUCCESS);
    EXPECT_FALSE(quickReject);

    OH_Drawing_CanvasDestroy(canvas);
    OH_Drawing_PathDestroy(path);
    OH_Drawing_PathDestroy(path2);
    OH_Drawing_PathDestroy(path3);
    OH_Drawing_BitmapDestroy(bitmap);
}

/*
 * @tc.name: NativeOH_Drawing_CanvasQuickRejectRect001
 * @tc.desc: test for OH_Drawing_CanvasQuickRejectRect.
 * @tc.type: FUNC
 * @tc.require: IBHFF5
 */
HWTEST_F(NativeDrawingCanvasTest, NativeOH_Drawing_CanvasQuickRejectRect001, TestSize.Level1)
{
    OH_Drawing_Canvas* canvas = OH_Drawing_CanvasCreate();
    OH_Drawing_Bitmap* bitmap = OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat cFormat{COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_OPAQUE};
    constexpr uint32_t width = 200; // 200 means bitmap width
    constexpr uint32_t height = 200; // 200 means bitmap height
    OH_Drawing_BitmapBuild(bitmap, width, height, &cFormat);
    OH_Drawing_CanvasBind(canvas, bitmap);
    bool quickReject = false;
    // rect left[50], top[50],right[250], bottom[250]
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(50, 50, 250, 250);
    auto result = OH_Drawing_CanvasQuickRejectRect(nullptr, rect, &quickReject);
    EXPECT_EQ(result, OH_DRAWING_ERROR_INVALID_PARAMETER);
    result = OH_Drawing_CanvasQuickRejectRect(canvas, nullptr, &quickReject);
    EXPECT_EQ(result, OH_DRAWING_ERROR_INVALID_PARAMETER);
    result = OH_Drawing_CanvasQuickRejectRect(canvas, rect, nullptr);
    EXPECT_EQ(result, OH_DRAWING_ERROR_INVALID_PARAMETER);
    result = OH_Drawing_CanvasQuickRejectRect(canvas, rect, &quickReject);
    EXPECT_EQ(result, OH_DRAWING_SUCCESS);
    EXPECT_FALSE(quickReject);
    // rect left[300], top[350],right[450], bottom[550]
    OH_Drawing_Rect* rect2 = OH_Drawing_RectCreate(300, 350, 450, 550);
    result = OH_Drawing_CanvasQuickRejectRect(canvas, rect2, &quickReject);
    EXPECT_EQ(result, OH_DRAWING_SUCCESS);
    EXPECT_TRUE(quickReject);
    // rect left[-100], top[-100],right[450], bottom[550]
    OH_Drawing_Rect* rect3 = OH_Drawing_RectCreate(-100, -100, 450, 550);
    result = OH_Drawing_CanvasQuickRejectRect(canvas, rect3, &quickReject);
    EXPECT_EQ(result, OH_DRAWING_SUCCESS);
    EXPECT_FALSE(quickReject);
    OH_Drawing_CanvasDestroy(canvas);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_RectDestroy(rect2);
    OH_Drawing_RectDestroy(rect3);
}

/*
 * @tc.name: NativeDrawingCanvasTest_CanvasDrawArcWithCenter001
 * @tc.desc: test for OH_Drawing_CanvasDrawArcWithCenter.
 * @tc.type: FUNC
 * @tc.require: IBHFF5
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_CanvasDrawArcWithCenter001, TestSize.Level1)
{
    // rect left[20], top[20],right[120], bottom[120]
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(20, 20, 120, 120);
    OH_Drawing_Canvas* canvas = OH_Drawing_CanvasCreate();
    // 180 means sweepAngle
    EXPECT_EQ(OH_Drawing_CanvasDrawArcWithCenter(nullptr, rect, 0, 180, false),
        OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_CanvasDrawArcWithCenter(canvas, nullptr, 0, 180, false),
        OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_CanvasDrawArcWithCenter(canvas, rect, 0, 180, true),
        OH_DRAWING_SUCCESS);
    EXPECT_EQ(OH_Drawing_CanvasDrawArcWithCenter(canvas, rect, -10.0, 650, true),
        OH_DRAWING_SUCCESS);
    EXPECT_EQ(OH_Drawing_CanvasDrawArcWithCenter(canvas, rect, 66, -120, false),
        OH_DRAWING_SUCCESS);
    OH_Drawing_CanvasDestroy(canvas);
    OH_Drawing_RectDestroy(rect);
}

/*
 * @tc.name: NativeDrawingCanvasTest_CanvasDrawNestedRoundRect001
 * @tc.desc: test for OH_Drawing_CanvasDrawNestedRoundRect.
 * @tc.type: FUNC
 * @tc.require: IBHFF5
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_CanvasDrawNestedRoundRect001, TestSize.Level1)
{
    // rect left[100], top[100],right[300], bottom[300]
    OH_Drawing_Rect* rectOuter = OH_Drawing_RectCreate(100, 100, 300, 300);
    OH_Drawing_RoundRect* outer = OH_Drawing_RoundRectCreate(rectOuter, 10, 10); // 10 means xRad yRad
    // rect left[120], top[120],right[280], bottom[280]
    OH_Drawing_Rect* rectInner = OH_Drawing_RectCreate(120, 120, 280, 280);
    OH_Drawing_RoundRect* inner = OH_Drawing_RoundRectCreate(rectInner, 10, 10); // 10 means xRad yRad
    OH_Drawing_Canvas* canvas = OH_Drawing_CanvasCreate();
    EXPECT_EQ(OH_Drawing_CanvasDrawNestedRoundRect(nullptr, outer, inner),
        OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_CanvasDrawNestedRoundRect(canvas, nullptr, inner),
        OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_CanvasDrawNestedRoundRect(canvas, outer, nullptr),
        OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_CanvasDrawNestedRoundRect(canvas, outer, inner),
        OH_DRAWING_SUCCESS);
    EXPECT_EQ(OH_Drawing_CanvasDrawNestedRoundRect(canvas, inner, outer),
        OH_DRAWING_SUCCESS);
    OH_Drawing_CanvasDestroy(canvas);
    OH_Drawing_RectDestroy(rectOuter);
    OH_Drawing_RectDestroy(rectInner);
    OH_Drawing_RoundRectDestroy(outer);
    OH_Drawing_RoundRectDestroy(inner);
}

/*
 * @tc.name: NativeDrawingCanvasTest_CanvasDrawPixelMapNine001
 * @tc.desc: test for OH_Drawing_CanvasDrawPixelMapNine.
 * @tc.type: FUNC
 * @tc.require: IBHFF5
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_CanvasDrawPixelMapNine001, TestSize.Level1)
{
    OH_Pixelmap_InitializationOptions *options = nullptr;
    OH_PixelmapNative *pixelMap = nullptr;
    OH_PixelmapInitializationOptions_Create(&options);
    // 4 means width
    OH_PixelmapInitializationOptions_SetWidth(options, 4);
    // 4 means height
    OH_PixelmapInitializationOptions_SetHeight(options, 4);
    // 4 means RGBA format
    OH_PixelmapInitializationOptions_SetPixelFormat(options, 3);
    // 2 means ALPHA_FORMAT_PREMUL format
    OH_PixelmapInitializationOptions_SetAlphaType(options, 2);
    // 255 means rgba data
    uint8_t data[] = {
        255, 255, 0, 255,
        255, 255, 0, 255,
        255, 255, 0, 255,
        255, 255, 0, 255
    };
    // 16 means data length
    size_t dataLength = 16;
    OH_PixelmapNative_CreatePixelmap(data, dataLength, options, &pixelMap);
    EXPECT_NE(pixelMap, nullptr);
    OH_Drawing_PixelMap *drPixelMap = OH_Drawing_PixelMapGetFromOhPixelMapNative(pixelMap);
    EXPECT_NE(drPixelMap, nullptr);

    auto recordingCanvas = new RecordingCanvas(100, 100);
    OH_Drawing_Canvas *cCanvas = reinterpret_cast<OH_Drawing_Canvas*>(recordingCanvas);
    EXPECT_NE(cCanvas, nullptr);

    OH_Drawing_Rect* srcRect = OH_Drawing_RectCreate(0, 0, 4, 4);
    OH_Drawing_Rect* dstRect = OH_Drawing_RectCreate(0, 0, 4, 4);

    EXPECT_EQ(OH_Drawing_CanvasDrawPixelMapNine(nullptr, drPixelMap, srcRect, dstRect,
        OH_Drawing_FilterMode::FILTER_MODE_NEAREST), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_CanvasDrawPixelMapNine(cCanvas, nullptr, srcRect, dstRect,
        OH_Drawing_FilterMode::FILTER_MODE_NEAREST), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_CanvasDrawPixelMapNine(cCanvas, drPixelMap, nullptr, dstRect,
        OH_Drawing_FilterMode::FILTER_MODE_NEAREST), OH_DRAWING_SUCCESS);
    EXPECT_EQ(OH_Drawing_CanvasDrawPixelMapNine(cCanvas, drPixelMap, srcRect, nullptr,
        OH_Drawing_FilterMode::FILTER_MODE_NEAREST), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_CanvasDrawPixelMapNine(cCanvas, drPixelMap, srcRect, dstRect,
        OH_Drawing_FilterMode::FILTER_MODE_NEAREST), OH_DRAWING_SUCCESS);

    auto drawCmdList = recordingCanvas->GetDrawCmdList();
    EXPECT_NE(drawCmdList, nullptr);
    Canvas canvas;
    drawCmdList->Playback(canvas);
    OH_Drawing_PixelMapDissolve(drPixelMap);
    OH_PixelmapNative_Release(pixelMap);
    OH_PixelmapInitializationOptions_Release(options);
    OH_Drawing_RectDestroy(srcRect);
    OH_Drawing_RectDestroy(dstRect);
    delete recordingCanvas;
}

/*
 * @tc.name: NativeDrawingCanvasTest_CanvasCreateWithPixelMap001
 * @tc.desc: test for OH_Drawing_CanvasCreateWithPixelMap.
 * @tc.type: FUNC
 * @tc.require: IBHFF5
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_CanvasCreateWithPixelMap001, TestSize.Level1)
{
    OH_Pixelmap_InitializationOptions *options = nullptr;
    OH_PixelmapInitializationOptions_Create(&options);
    EXPECT_NE(options, nullptr);

    // 1 means width
    OH_PixelmapInitializationOptions_SetWidth(options, 1);
    // 1 means height
    OH_PixelmapInitializationOptions_SetHeight(options, 1);
    // 3 means RGBA format
    OH_PixelmapInitializationOptions_SetPixelFormat(options, 3);
    // 2 means ALPHA_FORMAT_PREMUL format
    OH_PixelmapInitializationOptions_SetAlphaType(options, 2);
    // 4 means data length
    size_t bufferSize = 4;
    void *buffer = malloc(bufferSize);
    EXPECT_NE(buffer, nullptr);

    OH_PixelmapNative *pixelMap = nullptr;
    OH_PixelmapNative_CreatePixelmap(static_cast<uint8_t *>(buffer), bufferSize, options, &pixelMap);
    EXPECT_NE(pixelMap, nullptr);
    OH_Drawing_PixelMap *drPixelMap = OH_Drawing_PixelMapGetFromOhPixelMapNative(pixelMap);
    EXPECT_NE(drPixelMap, nullptr);

    OH_Drawing_Canvas *canvas = OH_Drawing_CanvasCreateWithPixelMap(nullptr);
    EXPECT_EQ(canvas, nullptr);
    canvas = OH_Drawing_CanvasCreateWithPixelMap(drPixelMap);
    EXPECT_NE(canvas, nullptr);

    OH_Drawing_CanvasDestroy(canvas);
    OH_Drawing_PixelMapDissolve(drPixelMap);
    OH_PixelmapNative_Release(pixelMap);
    OH_PixelmapInitializationOptions_Release(options);
    free(buffer);
}

/*
 * @tc.name: NativeDrawingCanvasTest_ImageFilterCreateOffsetImageFilter001
 * @tc.desc: test for creates an OH_Drawing_ImageFilter object that instance with the provided x and y offset.
 * @tc.type: FUNC
 * @tc.require: IAYWTV
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_ImageFilterCreateOffset001, TestSize.Level1)
{
    OH_Drawing_ImageFilter* cInput = nullptr;
    float dx = 0.f;
    float dy = 0.f;

    OH_Drawing_ImageFilter* imagefilterTest1 =
        OH_Drawing_ImageFilterCreateOffset(dx, dy, cInput);
    EXPECT_TRUE(imagefilterTest1 != nullptr);

    float sifmaX = 10.0f;
    float sigmaY = 10.0f;
    cInput = OH_Drawing_ImageFilterCreateBlur(sifmaX, sigmaY, CLAMP, nullptr);
    EXPECT_TRUE(cInput != nullptr);
    OH_Drawing_ImageFilter* imagefilterTest2 =
        OH_Drawing_ImageFilterCreateOffset(dx, dy, cInput);
    EXPECT_TRUE(imagefilterTest2 != nullptr);

    OH_Drawing_ImageFilterDestroy(cInput);
    OH_Drawing_ImageFilterDestroy(imagefilterTest1);
    OH_Drawing_ImageFilterDestroy(imagefilterTest2);
}

/*
 * @tc.name: NativeDrawingCanvasTest_ImageFilterCreateShaderImageFilter001
 * @tc.desc: test for creates an OH_Drawing_ImageFilter object that renders the contents of the input Shader.
 * @tc.type: FUNC
 * @tc.require: IAYWTV
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_ImageFilterCreateShaderImageFilter001, TestSize.Level1)
{
    OH_Drawing_ShaderEffect* cShader = nullptr;
    OH_Drawing_ImageFilter* imagefilterTest1 = OH_Drawing_ImageFilterCreateFromShaderEffect(cShader);
    EXPECT_TRUE(imagefilterTest1 == nullptr);
    uint32_t color = 1;
    cShader = OH_Drawing_ShaderEffectCreateColorShader(color);
    EXPECT_TRUE(cShader != nullptr);
    OH_Drawing_ImageFilter* imagefilterTest2 = OH_Drawing_ImageFilterCreateFromShaderEffect(cShader);
    EXPECT_TRUE(imagefilterTest2 != nullptr);

    OH_Drawing_ShaderEffectDestroy(cShader);
    OH_Drawing_ImageFilterDestroy(imagefilterTest1);
    OH_Drawing_ImageFilterDestroy(imagefilterTest2);
}

/*
 * @tc.name: NativeDrawingCanvasTest_ColorFilterCreateLighting001
 * @tc.desc: test for colorfilter create lighting.
 * @tc.type: FUNC
 * @tc.require: IAYWTV
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_ColorFilterCreateLighting001, TestSize.Level1)
{
    OH_Drawing_ColorFilter* colorFilter = OH_Drawing_ColorFilterCreateLighting(0xff0000ff, 0xff000001);
    EXPECT_NE(colorFilter, nullptr);
    OH_Drawing_ColorFilter* colorFilterTmp = OH_Drawing_ColorFilterCreateLinearToSrgbGamma();
    EXPECT_NE(colorFilterTmp, nullptr);
    OH_Drawing_Filter* filter = OH_Drawing_FilterCreate();
    EXPECT_NE(filter, nullptr);

    OH_Drawing_FilterSetColorFilter(nullptr, colorFilter);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_FilterSetColorFilter(filter, nullptr);
    OH_Drawing_FilterGetColorFilter(filter, colorFilterTmp);
    NativeHandle<ColorFilter>* colorFilterHandle = Helper::CastTo<OH_Drawing_ColorFilter*,
        NativeHandle<ColorFilter>*>(colorFilterTmp);
    EXPECT_NE(colorFilterHandle, nullptr);
    EXPECT_NE(colorFilterHandle->value, nullptr);
    EXPECT_EQ(colorFilterHandle->value->GetType(), ColorFilter::FilterType::NO_TYPE);

    OH_Drawing_FilterSetColorFilter(filter, colorFilter);
    OH_Drawing_FilterGetColorFilter(filter, colorFilterTmp);
    NativeHandle<ColorFilter>* colorFilterHandleT = Helper::CastTo<OH_Drawing_ColorFilter*,
        NativeHandle<ColorFilter>*>(colorFilterTmp);
    EXPECT_NE(colorFilterHandleT, nullptr);
    EXPECT_NE(colorFilterHandleT->value, nullptr);
    EXPECT_EQ(colorFilterHandleT->value->GetType(), ColorFilter::FilterType::LIGHTING);

    OH_Drawing_BrushSetFilter(brush_, nullptr);
    OH_Drawing_BrushSetFilter(brush_, filter);
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(0, 0, 100, 100);
    OH_Drawing_CanvasDrawRect(canvas_, rect);
}

/*
 * @tc.name: NativeDrawingCanvasTest_ColorFilterCToCpp001
 * @tc.desc: test for colorfilter object c to cpp.
 * @tc.type: FUNC
 * @tc.require: ICEWQ3
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_ColorFilterCToCpp001, TestSize.Level1)
{
    OH_Drawing_ColorFilter* cColorFilter = OH_Drawing_ColorFilterCreateLighting(0xff0000ff, 0xff000001);
    EXPECT_NE(cColorFilter, nullptr);
    std::shared_ptr<ColorFilter> colorFilter = Helper::ColorFilterCToCpp(cColorFilter);
    EXPECT_EQ(colorFilter->GetType(), ColorFilter::FilterType::LIGHTING);
    OH_Drawing_ColorFilterDestroy(cColorFilter);
}

/*
 * @tc.name: NativeDrawingCanvasTest_ColorFilterCToCpp002
 * @tc.desc: test for nullptr c to cpp.
 * @tc.type: FUNC
 * @tc.require: ICEWQ3
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_ColorFilterCToCpp002, TestSize.Level1)
{
    std::shared_ptr<ColorFilter> colorFilter = Helper::ColorFilterCToCpp(nullptr);
    EXPECT_EQ(colorFilter, nullptr);
}

/*
 * @tc.name: NativeDrawingCanvasTest_ColorFilterCppToC001
 * @tc.desc: test for colorfilter object cpp to c.
 * @tc.type: FUNC
 * @tc.require: ICEWQ3
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_ColorFilterCppToC001, TestSize.Level1)
{
    std::shared_ptr<ColorFilter> colorFilter = ColorFilter::CreateLightingColorFilter(0xff0000ff, 0xff000001);
    EXPECT_NE(colorFilter, nullptr);
    OH_Drawing_ColorFilter* cColorFilter = Helper::ColorFilterCppToC(&colorFilter);
    EXPECT_NE(cColorFilter, nullptr);
}

/*
 * @tc.name: NativeDrawingCanvasTest_ColorFilterCppToC002
 * @tc.desc: test for nullptr cpp to c.
 * @tc.type: FUNC
 * @tc.require: ICEWQ3
 */
HWTEST_F(NativeDrawingCanvasTest, NativeDrawingCanvasTest_ColorFilterCppToC002, TestSize.Level1)
{
    OH_Drawing_ColorFilter* cColorFilter = Helper::ColorFilterCppToC(nullptr);
    EXPECT_EQ(cColorFilter, nullptr);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
