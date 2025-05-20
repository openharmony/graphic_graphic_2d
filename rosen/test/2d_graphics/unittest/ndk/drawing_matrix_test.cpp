/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#include "drawing_error_code.h"
#include "drawing_matrix.h"
#include "drawing_rect.h"
#include "utils/scalar.h"

#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class NativeDrawingMatrixTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

constexpr uint32_t INTNUM_TEN = 10;
constexpr int32_t NEGATIVE_ONE = -1;
constexpr uint32_t MAPPOINTS_SIZE = 5;
constexpr uint32_t MAPPOINTS_COUNT = 2;

void NativeDrawingMatrixTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
}
void NativeDrawingMatrixTest::TearDownTestCase() {}
void NativeDrawingMatrixTest::SetUp() {}
void NativeDrawingMatrixTest::TearDown() {}

/*
 * @tc.name: NativeDrawingMatrixTest_SetMatrix001
 * @tc.desc: test for SetMatrix.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingMatrixTest, NativeDrawingMatrixTest_SetMatrix001, TestSize.Level1)
{
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(
        matrix,
        1, 0, 0,
        0, -1, 0,
        0, 0, 1);
    float value = OH_Drawing_MatrixGetValue(matrix, 0);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 1.0f));
    OH_Drawing_MatrixDestroy(matrix);
    OH_Drawing_MatrixDestroy(nullptr);
}

/*
 * @tc.name: NativeDrawingMatrixTest_SetRectToRect002
 * @tc.desc: test for SetRectToRect.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingMatrixTest, NativeDrawingMatrixTest_SetRectToRect002, TestSize.Level1)
{
    OH_Drawing_Rect *rectSrcOne = OH_Drawing_RectCreate(0, 0, 0, 0);
    OH_Drawing_Rect *rectDstOne = OH_Drawing_RectCreate(0, 0, 0, 0);
    OH_Drawing_Matrix *matrixOne = OH_Drawing_MatrixCreate();
    bool isSuccess = OH_Drawing_MatrixSetRectToRect(matrixOne,
        rectSrcOne, rectDstOne, OH_Drawing_ScaleToFit::SCALE_TO_FIT_FILL);
    EXPECT_EQ(isSuccess, false);
    isSuccess = OH_Drawing_MatrixSetRectToRect(nullptr,
        rectSrcOne, rectDstOne, OH_Drawing_ScaleToFit::SCALE_TO_FIT_FILL);
    EXPECT_EQ(isSuccess, false);
    isSuccess = OH_Drawing_MatrixSetRectToRect(matrixOne,
        nullptr, rectDstOne, OH_Drawing_ScaleToFit::SCALE_TO_FIT_FILL);
    EXPECT_EQ(isSuccess, false);
    isSuccess = OH_Drawing_MatrixSetRectToRect(matrixOne,
        rectSrcOne, nullptr, OH_Drawing_ScaleToFit::SCALE_TO_FIT_FILL);
    EXPECT_EQ(isSuccess, false);
    isSuccess = OH_Drawing_MatrixSetRectToRect(nullptr, nullptr, nullptr, OH_Drawing_ScaleToFit::SCALE_TO_FIT_FILL);
    EXPECT_EQ(isSuccess, false);

    OH_Drawing_Rect *rectSrcTwo = OH_Drawing_RectCreate(1, 2, 3, 4);
    OH_Drawing_Rect *rectDstTwo = OH_Drawing_RectCreate(5, 6, 8, 9);
    OH_Drawing_Matrix *matrixTwo = OH_Drawing_MatrixCreate();
    bool isSuccessTwo = OH_Drawing_MatrixSetRectToRect(matrixTwo,
        rectSrcTwo, rectDstTwo, OH_Drawing_ScaleToFit::SCALE_TO_FIT_FILL);
    EXPECT_EQ(isSuccessTwo, true);
    float value;
    value = OH_Drawing_MatrixGetValue(matrixTwo, 0);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 1.5f));
    value = OH_Drawing_MatrixGetValue(matrixTwo, 1);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0.0f));
    value = OH_Drawing_MatrixGetValue(matrixTwo, 2);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 3.5f));

    value = OH_Drawing_MatrixGetValue(matrixTwo, 3);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0.0f));
    value = OH_Drawing_MatrixGetValue(matrixTwo, 4);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 1.5f));
    value = OH_Drawing_MatrixGetValue(matrixTwo, 5);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 3.0f));

    value = OH_Drawing_MatrixGetValue(matrixTwo, 6);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0.0f));
    value = OH_Drawing_MatrixGetValue(matrixTwo, 7);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0.0f));
    value = OH_Drawing_MatrixGetValue(matrixTwo, 8);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 1.0f));

    OH_Drawing_MatrixDestroy(matrixOne);
    OH_Drawing_MatrixDestroy(matrixTwo);
    OH_Drawing_RectDestroy(rectSrcOne);
    OH_Drawing_RectDestroy(rectDstOne);
    OH_Drawing_RectDestroy(rectSrcTwo);
    OH_Drawing_RectDestroy(rectDstTwo);
}

/*
 * @tc.name: NativeDrawingMatrixTest_Reset003
 * @tc.desc: test for Reset.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingMatrixTest, NativeDrawingMatrixTest_Reset003, TestSize.Level1)
{
    OH_Drawing_Matrix *matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(nullptr, 2, 0, 0, 0, 1, 2, 0, 0, 1);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_MatrixSetMatrix(matrix, 2, 0, 0, 0, 1, 2, 0, 0, 1);
    OH_Drawing_MatrixReset(nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_MatrixReset(matrix);
    bool isIdentity = OH_Drawing_MatrixIsIdentity(matrix);
    EXPECT_EQ(isIdentity, true);
    OH_Drawing_MatrixDestroy(matrix);
}

/*
 * @tc.name: NativeDrawingMatrixTest_CreateRotation004
 * @tc.desc: test for CreateRotation.
 * @tc.type: FUNC
 * @tc.require: SR000S9F0C
 */
HWTEST_F(NativeDrawingMatrixTest, NativeDrawingMatrixTest_CreateRotation004, TestSize.Level1)
{
    // rotate deg: 180 pivot, point (1, 1)
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreateRotation(180, 1, 1);
    OH_Drawing_MatrixGetValue(nullptr, 0);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_MatrixGetValue(matrix, NEGATIVE_ONE);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE);
    OH_Drawing_MatrixGetValue(matrix, INTNUM_TEN);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE);
    float value;
    value = OH_Drawing_MatrixGetValue(matrix, 0);
    EXPECT_TRUE(IsScalarAlmostEqual(value, -1));
    value = OH_Drawing_MatrixGetValue(matrix, 1);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 2);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 2));

    value = OH_Drawing_MatrixGetValue(matrix, 3);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 4);
    EXPECT_TRUE(IsScalarAlmostEqual(value, -1));
    value = OH_Drawing_MatrixGetValue(matrix, 5);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 2));

    value = OH_Drawing_MatrixGetValue(matrix, 6);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 7);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 8);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 1));

    OH_Drawing_MatrixDestroy(matrix);
}

/*
 * @tc.name: NativeDrawingMatrixTest_CreateScale005
 * @tc.desc: test for CreateScale.
 * @tc.type: FUNC
 * @tc.require: SR000S9F0C
 */
HWTEST_F(NativeDrawingMatrixTest, NativeDrawingMatrixTest_CreateScale005, TestSize.Level1)
{
    /* The first 10 is horizontal scale factor.
    The second 10 is vertical scale factor.
    The third 10 is pivot on x-axis.
    The fourth 10 is the pivot on y-axis.
    */
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreateScale(10, 10, 10, 10);
    float value;
    value = OH_Drawing_MatrixGetValue(matrix, 0);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 10));
    value = OH_Drawing_MatrixGetValue(matrix, 1);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 2);
    EXPECT_TRUE(IsScalarAlmostEqual(value, -90));

    value = OH_Drawing_MatrixGetValue(matrix, 3);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 4);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 10));
    value = OH_Drawing_MatrixGetValue(matrix, 5);
    EXPECT_TRUE(IsScalarAlmostEqual(value, -90));

    value = OH_Drawing_MatrixGetValue(matrix, 6);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 7);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 8);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 1));

    OH_Drawing_MatrixDestroy(matrix);
}

/*
 * @tc.name: NativeDrawingMatrixTest_CreateTranslation006
 * @tc.desc: test for CreateTranslation.
 * @tc.type: FUNC
 * @tc.require: SR000S9F0C
 */
HWTEST_F(NativeDrawingMatrixTest, NativeDrawingMatrixTest_CreateTranslation006, TestSize.Level1)
{
    // translate x= 100, y = 200
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreateTranslation(100, 200);
    float value;
    value = OH_Drawing_MatrixGetValue(matrix, 0);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 1));
    value = OH_Drawing_MatrixGetValue(matrix, 1);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 2);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 100));

    value = OH_Drawing_MatrixGetValue(matrix, 3);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 4);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 1));
    value = OH_Drawing_MatrixGetValue(matrix, 5);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 200));

    value = OH_Drawing_MatrixGetValue(matrix, 6);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 7);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 8);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 1));

    OH_Drawing_MatrixDestroy(matrix);
}

/*
 * @tc.name: NativeDrawingMatrixTest_Concat007
 * @tc.desc: test for Concat.
 * @tc.type: FUNC
 * @tc.require: SR000S9F0C
 */
HWTEST_F(NativeDrawingMatrixTest, NativeDrawingMatrixTest_Concat007, TestSize.Level1)
{
    OH_Drawing_Matrix* matrixA = OH_Drawing_MatrixCreate();
    OH_Drawing_Matrix* matrixB = OH_Drawing_MatrixCreate();
    bool ret;
    ret = OH_Drawing_MatrixIsEqual(nullptr, matrixB);
    EXPECT_TRUE(!ret);
    ret = OH_Drawing_MatrixIsEqual(matrixA, nullptr);
    EXPECT_TRUE(!ret);
    ret = OH_Drawing_MatrixIsEqual(matrixA, matrixB);
    EXPECT_TRUE(ret);
    OH_Drawing_MatrixSetMatrix(
        matrixA,
        1, 0, 0,
        0, -1, 0,
        0, 0, 1);
    OH_Drawing_MatrixSetMatrix(
        matrixB,
        1, 0, 100,
        0, -1, 200,
        0, 0, 1);
    ret = OH_Drawing_MatrixIsEqual(matrixA, matrixB);
    EXPECT_TRUE(!ret);
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixConcat(nullptr, matrixA, matrixB);
    OH_Drawing_MatrixConcat(matrix, nullptr, matrixB);
    OH_Drawing_MatrixConcat(matrix, matrixA, nullptr);
    OH_Drawing_MatrixConcat(matrix, matrixA, matrixB);
    float value;
    value = OH_Drawing_MatrixGetValue(matrix, 0);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 1));
    value = OH_Drawing_MatrixGetValue(matrix, 1);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 2);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 100));

    value = OH_Drawing_MatrixGetValue(matrix, 3);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 4);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 1));
    value = OH_Drawing_MatrixGetValue(matrix, 5);
    EXPECT_TRUE(IsScalarAlmostEqual(value, -200));

    value = OH_Drawing_MatrixGetValue(matrix, 6);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 7);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 8);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 1));

    OH_Drawing_MatrixDestroy(matrix);
    OH_Drawing_MatrixDestroy(matrixA);
    OH_Drawing_MatrixDestroy(matrixB);
}

/*
 * @tc.name: NativeDrawingMatrixTest_Rotate008
 * @tc.desc: test for Rotate.
 * @tc.type: FUNC
 * @tc.require: SR000S9F0C
 */
HWTEST_F(NativeDrawingMatrixTest, NativeDrawingMatrixTest_Rotate008, TestSize.Level1)
{
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    // rotate deg: 180 pivot, point (1, 1)
    OH_Drawing_MatrixRotate(nullptr, 180, 1, 1);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_MatrixRotate(matrix, 180, 1, 1);
    float value;
    value = OH_Drawing_MatrixGetValue(matrix, 0);
    EXPECT_TRUE(IsScalarAlmostEqual(value, -1));
    value = OH_Drawing_MatrixGetValue(matrix, 1);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 2);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 2));

    value = OH_Drawing_MatrixGetValue(matrix, 3);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 4);
    EXPECT_TRUE(IsScalarAlmostEqual(value, -1));
    value = OH_Drawing_MatrixGetValue(matrix, 5);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 2));

    value = OH_Drawing_MatrixGetValue(matrix, 6);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 7);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 8);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 1));

    //GetValue exception
    value = OH_Drawing_MatrixGetValue(nullptr, 8);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, -1);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 9);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));

    OH_Drawing_MatrixDestroy(matrix);
}

/*
 * @tc.name: NativeDrawingMatrixTest_PreRotate009
 * @tc.desc: test for PreRotate.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingMatrixTest, NativeDrawingMatrixTest_PreRotate009, TestSize.Level1)
{
    OH_Drawing_Matrix *matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(matrix, 2, 0, 0, 0, 1, 2, 0, 0, 1);
    OH_Drawing_MatrixPreRotate(matrix, 5, 10, 20);
    OH_Drawing_MatrixPreRotate(nullptr, 5, 10, 20);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);

    OH_Drawing_MatrixSetMatrix(matrix, 1, 2, 3, 4, 5, 6, 7, 8, 9);
    OH_Drawing_MatrixPreRotate(matrix, 90, 1, 0);
    OH_Drawing_Matrix* matrix2 = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(matrix2, 2, -1, 2, 5, -4, 5, 8, -7, 8);
    bool ret = false;
    ret = OH_Drawing_MatrixIsEqual(matrix, matrix2);
    EXPECT_EQ(ret, true);
    OH_Drawing_MatrixDestroy(matrix);
    OH_Drawing_MatrixDestroy(matrix2);
}

/*
 * @tc.name: NativeDrawingMatrixTest_PostRotate010
 * @tc.desc: test for PostRotate.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingMatrixTest, NativeDrawingMatrixTest_PostRotate010, TestSize.Level1)
{
    OH_Drawing_Matrix *matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(matrix, 2, 0, 0, 0, 1, 2, 0, 0, 1);
    OH_Drawing_MatrixPostRotate(matrix, 5, 10, 20);
    OH_Drawing_MatrixPostRotate(nullptr, 5, 10, 20);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);

    OH_Drawing_MatrixSetMatrix(matrix, 1, 2, 3, 4, 5, 6, 7, 8, 9);
    OH_Drawing_MatrixPostRotate(matrix, 90, 1, 0);
    OH_Drawing_Matrix* matrix2 = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(matrix2, 3, 3, 3, -6, -6, -6, 7, 8, 9);
    bool ret = false;
    ret = OH_Drawing_MatrixIsEqual(matrix, matrix2);
    EXPECT_EQ(ret, true);
    OH_Drawing_MatrixDestroy(matrix);
    OH_Drawing_MatrixDestroy(matrix2);
}

/*
 * @tc.name: NativeDrawingMatrixTest_Scale011
 * @tc.desc: test for Scale.
 * @tc.type: FUNC
 * @tc.require: SR000S9F0C
 */
HWTEST_F(NativeDrawingMatrixTest, NativeDrawingMatrixTest_Scale011, TestSize.Level1)
{
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    /* The first 10 is horizontal scale factor.
    The second 10 is vertical scale factor.
    The third 10 is pivot on x-axis.
    The fourth 10 is the pivot on y-axis.
    */
    OH_Drawing_MatrixScale(nullptr, 10, 10, 10, 10);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_MatrixScale(matrix, 10, 10, 10, 10);
    float value;
    value = OH_Drawing_MatrixGetValue(matrix, 0);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 10));
    value = OH_Drawing_MatrixGetValue(matrix, 1);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 2);
    EXPECT_TRUE(IsScalarAlmostEqual(value, -90));

    value = OH_Drawing_MatrixGetValue(matrix, 3);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 4);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 10));
    value = OH_Drawing_MatrixGetValue(matrix, 5);
    EXPECT_TRUE(IsScalarAlmostEqual(value, -90));

    value = OH_Drawing_MatrixGetValue(matrix, 6);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 7);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 8);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 1));

    OH_Drawing_MatrixDestroy(matrix);
}

/*
 * @tc.name: NativeDrawingMatrixTest_PreScale012
 * @tc.desc: test for PreScale.
 * @tc.type: FUNC
 * @tc.require: SR000S9F0C
 */
HWTEST_F(NativeDrawingMatrixTest, NativeDrawingMatrixTest_PreScale012, TestSize.Level1)
{
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixPreScale(nullptr, 10, 10, 10, 10);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_MatrixPreScale(matrix, 10, 10, 10, 10);
    float value;
    value = OH_Drawing_MatrixGetValue(matrix, 0);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 10));

    OH_Drawing_MatrixSetMatrix(matrix, 2, 1, 3, 1, 2, 2, 3, 1, 1);
    OH_Drawing_MatrixPreScale(matrix, 4, 6, 5, 7);
    OH_Drawing_Matrix* matrix2 = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(matrix2, 8, 6, -62, 4, 12, -83, 12, 6, -79);
    bool ret = false;
    ret = OH_Drawing_MatrixIsEqual(matrix, matrix2);
    EXPECT_EQ(ret, true);

    OH_Drawing_MatrixDestroy(matrix);
    OH_Drawing_MatrixDestroy(matrix2);
}

/*
 * @tc.name: NativeDrawingMatrixTest_PostScale013
 * @tc.desc: test for PostScale.
 * @tc.type: FUNC
 * @tc.require: SR000S9F0C
 */
HWTEST_F(NativeDrawingMatrixTest, NativeDrawingMatrixTest_PostScale013, TestSize.Level1)
{
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixPostScale(nullptr, 10, 10, 10, 10);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_MatrixPostScale(matrix, 10, 10, 10, 10);
    float value;
    value = OH_Drawing_MatrixGetValue(matrix, 0);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 10));

    OH_Drawing_MatrixSetMatrix(matrix, 2, 1, 3, 1, 2, 2, 3, 1, 1);
    OH_Drawing_MatrixPostScale(matrix, 4, 6, 5, 7);
    OH_Drawing_Matrix* matrix2 = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(matrix2, -37, -11, -3, -99, -23, -23, 3, 1, 1);
    bool ret = false;
    ret = OH_Drawing_MatrixIsEqual(matrix, matrix2);
    EXPECT_EQ(ret, true);
    OH_Drawing_MatrixDestroy(matrix);
    OH_Drawing_MatrixDestroy(matrix2);
}

/*
 * @tc.name: NativeDrawingMatrixTest_Translate014
 * @tc.desc: test for Translate.
 * @tc.type: FUNC
 * @tc.require: SR000S9F0C
 */
HWTEST_F(NativeDrawingMatrixTest, NativeDrawingMatrixTest_Translate014, TestSize.Level1)
{
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    bool ret;
    ret = OH_Drawing_MatrixIsIdentity(matrix);
    EXPECT_TRUE(ret);
    ret = OH_Drawing_MatrixIsIdentity(nullptr);
    EXPECT_TRUE(!ret);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    // translate x = 100, y = 200
    OH_Drawing_MatrixTranslate(nullptr, 100, 200);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_MatrixTranslate(matrix, 100, 200);
    ret = OH_Drawing_MatrixIsIdentity(matrix);
    EXPECT_TRUE(!ret);
    float value;
    value = OH_Drawing_MatrixGetValue(matrix, 0);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 1));
    value = OH_Drawing_MatrixGetValue(matrix, 1);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 2);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 100));

    value = OH_Drawing_MatrixGetValue(matrix, 3);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 4);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 1));
    value = OH_Drawing_MatrixGetValue(matrix, 5);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 200));

    value = OH_Drawing_MatrixGetValue(matrix, 6);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 7);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrix, 8);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 1));

    OH_Drawing_MatrixDestroy(matrix);
}

/*
 * @tc.name: NativeDrawingMatrixTest_PreTranslate015
 * @tc.desc: test for PreTranslate.
 * @tc.type: FUNC
 * @tc.require: SR000S9F0C
 */
HWTEST_F(NativeDrawingMatrixTest, NativeDrawingMatrixTest_PreTranslate015, TestSize.Level1)
{
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixPreTranslate(nullptr, 10, 10);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_MatrixPreTranslate(matrix, 10, 10);
    float value;
    value = OH_Drawing_MatrixGetValue(matrix, 0);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 1));

    OH_Drawing_MatrixSetMatrix(matrix, 2, 1, 3, 1, 2, 2, 3, 1, 1);
    OH_Drawing_MatrixPreTranslate(matrix, 2, 4);
    OH_Drawing_Matrix* matrix2 = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(matrix2, 2, 1, 11, 1, 2, 12, 3, 1, 11);
    bool ret = false;
    ret = OH_Drawing_MatrixIsEqual(matrix, matrix2);
    EXPECT_EQ(ret, true);
    OH_Drawing_MatrixDestroy(matrix);
    OH_Drawing_MatrixDestroy(matrix2);
}

/*
 * @tc.name: NativeDrawingMatrixTest_PostTranslate016
 * @tc.desc: test for PostTranslate.
 * @tc.type: FUNC
 * @tc.require: SR000S9F0C
 */
HWTEST_F(NativeDrawingMatrixTest, NativeDrawingMatrixTest_PostTranslate016, TestSize.Level1)
{
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixPostTranslate(nullptr, 10, 10);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_MatrixPostTranslate(matrix, 10, 10);
    float value;
    value = OH_Drawing_MatrixGetValue(matrix, 0);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 1));

    OH_Drawing_MatrixSetMatrix(matrix, 2, 1, 3, 1, 2, 2, 3, 1, 1);
    OH_Drawing_MatrixPostTranslate(matrix, 2, 4);
    OH_Drawing_Matrix* matrix2 = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(matrix2, 8, 3, 5, 13, 6, 6, 3, 1, 1);
    bool ret = false;
    ret = OH_Drawing_MatrixIsEqual(matrix, matrix2);
    EXPECT_EQ(ret, true);

    OH_Drawing_MatrixDestroy(matrix);
    OH_Drawing_MatrixDestroy(matrix2);
}

/*
 * @tc.name: NativeDrawingMatrixTest_Invert017
 * @tc.desc: test for Invert.
 * @tc.type: FUNC
 * @tc.require: SR000S9F0C
 */
HWTEST_F(NativeDrawingMatrixTest, NativeDrawingMatrixTest_Invert017, TestSize.Level1)
{
    bool ret;
    ret = OH_Drawing_MatrixInvert(nullptr, nullptr);
    EXPECT_TRUE(!ret);
    OH_Drawing_Matrix* matrixA = OH_Drawing_MatrixCreate();
    OH_Drawing_Matrix* matrixB = OH_Drawing_MatrixCreate();
    ret = OH_Drawing_MatrixInvert(nullptr, matrixB);
    EXPECT_TRUE(!ret);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    ret = OH_Drawing_MatrixInvert(matrixA, nullptr);
    EXPECT_TRUE(!ret);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    ret = OH_Drawing_MatrixInvert(matrixA, matrixB);
    EXPECT_TRUE(ret);
    OH_Drawing_MatrixSetMatrix(
        matrixA,
        1, 0, 0,
        0, -0.5, 0,
        0, 0, 1);
    ret = OH_Drawing_MatrixInvert(matrixA, matrixB);
    EXPECT_TRUE(ret);

    float value;
    value = OH_Drawing_MatrixGetValue(matrixB, 0);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 1));
    value = OH_Drawing_MatrixGetValue(matrixB, 1);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrixB, 2);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));

    value = OH_Drawing_MatrixGetValue(matrixB, 3);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrixB, 4);
    EXPECT_TRUE(IsScalarAlmostEqual(value, -2));
    value = OH_Drawing_MatrixGetValue(matrixB, 5);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));

    value = OH_Drawing_MatrixGetValue(matrixB, 6);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrixB, 7);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 0));
    value = OH_Drawing_MatrixGetValue(matrixB, 8);
    EXPECT_TRUE(IsScalarAlmostEqual(value, 1));

    OH_Drawing_MatrixDestroy(matrixA);
    OH_Drawing_MatrixDestroy(matrixB);
}

/**
 * @tc.name: NativeDrawingMatrixTest_SetPolyToPoly018
 * @tc.desc: test for set poly to poly of Matrix.
 * @tc.type: FUNC
 * @tc.require: AR20240104201189
 */
HWTEST_F(NativeDrawingMatrixTest, NativeDrawingMatrixTest_SetPolyToPoly018, TestSize.Level1)
{
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    ASSERT_TRUE(matrix != nullptr);
    OH_Drawing_MatrixSetMatrix(
        matrix,
        1, 0, 0,
        0, -1, 0,
        0, 0, 1);
    OH_Drawing_Point2D src[] = {{0, 0}, {100, 0}, {100, 100}, {0, 100}, {0, 100}};
    OH_Drawing_Point2D dst[] = {{0, 0}, {100, 30}, {100, 70}, {0, 100}, {0, 100}};
    EXPECT_TRUE(OH_Drawing_MatrixSetPolyToPoly(matrix, src, dst, 0));
    OH_Drawing_MatrixSetPolyToPoly(nullptr, src, dst, 0);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_MatrixSetPolyToPoly(matrix, src, dst, NEGATIVE_ONE);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE);
    OH_Drawing_MatrixSetPolyToPoly(matrix, src, dst, INTNUM_TEN);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_PARAMETER_OUT_OF_RANGE);
    OH_Drawing_MatrixDestroy(matrix);
}

/**
 * @tc.name: NativeDrawingMatrixTest_SetPolyToPoly019
 * @tc.desc: test for set poly to poly of Matrix.
 * @tc.type: FUNC
 * @tc.require: AR20240104201189
 */
HWTEST_F(NativeDrawingMatrixTest, NativeDrawingMatrixTest_SetPolyToPoly019, TestSize.Level1)
{
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    ASSERT_TRUE(matrix != nullptr);
    OH_Drawing_MatrixSetMatrix(
        matrix,
        1, 0, 0,
        0, -1, 0,
        0, 0, 1);
    OH_Drawing_Point2D src[] = {{0, 0}, {100, 0}, {100, 100}, {0, 100}};
    OH_Drawing_Point2D dst[] = {{0, 0}, {100, 30}, {100, 70}, {0, 100}};
    EXPECT_TRUE(OH_Drawing_MatrixSetPolyToPoly(matrix, src, dst, 1));
    OH_Drawing_MatrixDestroy(matrix);
}

/**
 * @tc.name: NativeDrawingMatrixTest_SetPolyToPoly020
 * @tc.desc: test for set poly to poly of Matrix.
 * @tc.type: FUNC
 * @tc.require: AR20240104201189
 */
HWTEST_F(NativeDrawingMatrixTest, NativeDrawingMatrixTest_SetPolyToPoly020, TestSize.Level1)
{
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    ASSERT_TRUE(matrix != nullptr);
    OH_Drawing_MatrixSetMatrix(
        matrix,
        1, 0, 0,
        0, -1, 0,
        0, 0, 1);
    OH_Drawing_Point2D src[] = {{0, 0}, {100, 0}, {100, 100}, {0, 100}, {0, 100}};
    OH_Drawing_Point2D dst[] = {{0, 0}, {100, 30}, {100, 70}, {0, 100}, {0, 100}};
    EXPECT_TRUE(OH_Drawing_MatrixSetPolyToPoly(matrix, src, dst, 4));
    OH_Drawing_MatrixDestroy(matrix);
}

/**
 * @tc.name: NativeDrawingMatrixTest_SetPolyToPoly021
 * @tc.desc: test for set poly to poly of Matrix.
 * @tc.type: FUNC
 * @tc.require: AR20240104201189
 */
HWTEST_F(NativeDrawingMatrixTest, NativeDrawingMatrixTest_SetPolyToPoly021, TestSize.Level1)
{
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    ASSERT_TRUE(matrix != nullptr);
    OH_Drawing_MatrixSetMatrix(
        matrix,
        1, 0, 0,
        0, -1, 0,
        0, 0, 1);
    OH_Drawing_Point2D src[] = {{0, 0}, {100, 0}, {100, 100}, {0, 100}, {0, 100}};
    OH_Drawing_Point2D dst[] = {{0, 0}, {100, 30}, {100, 70}, {0, 100}, {0, 100}};
    EXPECT_FALSE(OH_Drawing_MatrixSetPolyToPoly(matrix, src, dst, 5));
    OH_Drawing_MatrixDestroy(matrix);
}

/**
 * @tc.name: NativeDrawingMatrixTest_GetAll022
 * @tc.desc: test for Copies nine scalar values contained by Matrix into buffer.
 * @tc.type: FUNC
 * @tc.require: AR20240104201189
 */
HWTEST_F(NativeDrawingMatrixTest, NativeDrawingMatrixTest_GetAll022, TestSize.Level1)
{
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    ASSERT_TRUE(matrix != nullptr);
    float buffer[9];
    float emptyBuffer[9] = {1, 2, 3, 3, 2, 1, 4, 5, 6};
    OH_Drawing_MatrixSetMatrix(matrix, 1, 2, 3, 3, 2, 1, 4, 5, 6);
    EXPECT_EQ(OH_Drawing_MatrixGetAll(matrix, buffer), OH_DRAWING_SUCCESS);
    for (int i = 0; i < 9; ++i) {
        EXPECT_TRUE(IsScalarAlmostEqual(buffer[i], emptyBuffer[i]));
    }
    OH_Drawing_MatrixDestroy(matrix);
}

/*
 * @tc.name: NativeDrawingMatrixTest_IsEqualAndConcat023
 * @tc.desc: test for IsEqual and Concat.
 * @tc.type: FUNC
 * @tc.require: SR000S9F0C
 */
HWTEST_F(NativeDrawingMatrixTest, NativeDrawingMatrixTest_IsEqualAndConcat023, TestSize.Level1)
{
    OH_Drawing_Matrix* matrixA = OH_Drawing_MatrixCreate();
    OH_Drawing_Matrix* matrixB = OH_Drawing_MatrixCreate();
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixIsEqual(nullptr, matrixB);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_MatrixIsEqual(matrixA, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);

    OH_Drawing_MatrixConcat(nullptr, matrixA, matrixB);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_MatrixConcat(matrix, nullptr, matrixB);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_MatrixConcat(matrix, matrixA, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);

    OH_Drawing_MatrixDestroy(matrix);
    OH_Drawing_MatrixDestroy(matrixA);
    OH_Drawing_MatrixDestroy(matrixB);
}

/*
 * @tc.name: NativeDrawingMatrixTest_SetRectToRect024
 * @tc.desc: test for SetRectToRect.
 * @tc.type: FUNC
 * @tc.require: SR000S9F0C
 */
HWTEST_F(NativeDrawingMatrixTest, NativeDrawingMatrixTest_SetRectToRect024, TestSize.Level1)
{
    OH_Drawing_Rect *rectSrcOne = OH_Drawing_RectCreate(0, 0, 0, 0);
    OH_Drawing_Rect *rectDstOne = OH_Drawing_RectCreate(0, 0, 0, 0);
    OH_Drawing_Matrix *matrixOne = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetRectToRect(nullptr, rectSrcOne, rectDstOne, OH_Drawing_ScaleToFit::SCALE_TO_FIT_FILL);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_MatrixSetRectToRect(matrixOne, nullptr, rectDstOne, OH_Drawing_ScaleToFit::SCALE_TO_FIT_FILL);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_MatrixSetRectToRect(matrixOne, rectSrcOne, nullptr, OH_Drawing_ScaleToFit::SCALE_TO_FIT_FILL);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);

    OH_Drawing_MatrixDestroy(matrixOne);
    OH_Drawing_RectDestroy(rectSrcOne);
    OH_Drawing_RectDestroy(rectDstOne);
}

/**
 * @tc.name: NativeDrawingMatrixTest_MapPoints025
 * @tc.desc: test for maps the src point array to the dst point array by matrix transformation.
 * @tc.type: FUNC
 * @tc.require: AR20240104201189
 */
HWTEST_F(NativeDrawingMatrixTest, NativeDrawingMatrixTest_MapPoints025, TestSize.Level1)
{
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    ASSERT_TRUE(matrix != nullptr);
    OH_Drawing_Point2D src[] = {{0, 0}, {100, 0}, {100, 100}, {0, 100}, {0, 100}};
    OH_Drawing_Point2D dst[MAPPOINTS_SIZE];

    OH_Drawing_MatrixMapPoints(nullptr, src, dst, MAPPOINTS_COUNT);
    OH_Drawing_MatrixTranslate(matrix, 100, 200);
    OH_Drawing_MatrixMapPoints(matrix, src, dst, MAPPOINTS_COUNT);

    EXPECT_EQ(dst[0].x, 100);
    EXPECT_EQ(dst[0].y, 200);
    OH_Drawing_MatrixDestroy(matrix);
}

/**
 * @tc.name: NativeDrawingMatrixTest_MapRect026
 * @tc.desc: test for sets dst to bounds of src corners mapped by matrix transformation.
 * @tc.type: FUNC
 * @tc.require: AR20240104201189
 */
HWTEST_F(NativeDrawingMatrixTest, NativeDrawingMatrixTest_MapRect026, TestSize.Level1)
{
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    ASSERT_TRUE(matrix != nullptr);
    OH_Drawing_Rect* src = OH_Drawing_RectCreate(0, 100, 200, 200);
    OH_Drawing_Rect* dst = OH_Drawing_RectCreate(0, 0, 0, 0);

    EXPECT_FALSE(OH_Drawing_MatrixMapRect(nullptr, src, dst));
    OH_Drawing_MatrixTranslate(matrix, 100, 200);
    EXPECT_TRUE(OH_Drawing_MatrixMapRect(matrix, src, dst));

    EXPECT_TRUE(IsScalarAlmostEqual(OH_Drawing_RectGetHeight(dst), 100.f));
    EXPECT_TRUE(IsScalarAlmostEqual(OH_Drawing_RectGetLeft(dst), 100.f));
    OH_Drawing_RectDestroy(src);
    OH_Drawing_RectDestroy(dst);
    OH_Drawing_MatrixDestroy(matrix);
}

/**
 * @tc.name: NativeDrawingMatrixTest_OH_Drawing_MatrixCopy027
 * @tc.desc: test for sets create matrix by copy.
 * @tc.type: FUNC
 * @tc.require: AR20240104201189
 */
HWTEST_F(NativeDrawingMatrixTest, NativeDrawingMatrixTest_OH_Drawing_MatrixCopy001, TestSize.Level1)
{
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    ASSERT_TRUE(matrix != nullptr);
    OH_Drawing_Matrix* matrix2 = OH_Drawing_MatrixCopy(matrix);
    ASSERT_TRUE(matrix2 != nullptr);
    OH_Drawing_Matrix* matrix3 = OH_Drawing_MatrixCopy(nullptr);
    ASSERT_TRUE(matrix3 == nullptr);

    OH_Drawing_MatrixDestroy(matrix);
    OH_Drawing_MatrixDestroy(matrix2);
    OH_Drawing_MatrixDestroy(matrix3);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
