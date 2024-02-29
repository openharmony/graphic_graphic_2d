/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "utils/matrix.h"
#include "utils/point.h"
#include "utils/scalar.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class MatrixTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void MatrixTest::SetUpTestCase() {}
void MatrixTest::TearDownTestCase() {}
void MatrixTest::SetUp() {}
void MatrixTest::TearDown() {}

/**
 * @tc.name: CreateAndDestroy001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(MatrixTest, CreateAndDestroy001, TestSize.Level1)
{
    // The best way to create Matrix.
    std::unique_ptr<Matrix> matrix = std::make_unique<Matrix>();
    ASSERT_TRUE(matrix != nullptr);
}

/**
 * @tc.name: CreateAndDestroy002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(MatrixTest, CreateAndDestroy002, TestSize.Level1)
{
    // The best way to create Matrix.
    Matrix matrix1;
    Matrix matrix2;
    ASSERT_TRUE(matrix1.Invert(matrix2));
}

/**
 * @tc.name: MatrixRotateTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(MatrixTest, MatrixRotateTest001, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrix = std::make_unique<Matrix>();
    ASSERT_TRUE(matrix != nullptr);
    matrix->Rotate(0.1f, 150.1f, 650.9f);
}

/**
 * @tc.name: MatrixRotateTest002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(MatrixTest, MatrixRotateTest002, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrix = std::make_unique<Matrix>();
    ASSERT_TRUE(matrix != nullptr);
    matrix->Rotate(20.8f, 133.0f, 100);
}

/**
 * @tc.name: MatrixTranslateTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(MatrixTest, MatrixTranslateTest001, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrix = std::make_unique<Matrix>();
    ASSERT_TRUE(matrix != nullptr);
    matrix->Translate(20.8f, 100);
}

/**
 * @tc.name: MatrixTranslateTest002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(MatrixTest, MatrixTranslateTest002, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrix = std::make_unique<Matrix>();
    ASSERT_TRUE(matrix != nullptr);
    matrix->Translate(77.7f, 190.2f);
}

/**
 * @tc.name: MatrixScaleTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(MatrixTest, MatrixScaleTest001, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrix = std::make_unique<Matrix>();
    ASSERT_TRUE(matrix != nullptr);
    matrix->Scale(32.1f, 10.6f, 800, 90.1f);
}

/**
 * @tc.name: MatrixScaleTest002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(MatrixTest, MatrixScaleTest002, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrix = std::make_unique<Matrix>();
    ASSERT_TRUE(matrix != nullptr);
    matrix->Scale(16.5f, 50.6f, 150.8f, 560.9f);
}

/**
 * @tc.name: MatrixPreRotateTest001
 * @tc.desc: test for seting Matrix to Matrix multiplied by Matrix constructed from rotating by degrees.
 * @tc.type: FUNC
 * @tc.require: I73WU0
 */
HWTEST_F(MatrixTest, MatrixPreRotateTest001, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrix = std::make_unique<Matrix>();
    ASSERT_TRUE(matrix != nullptr);
    matrix->PreRotate(0.1f);
}

/**
 * @tc.name: MatrixPreTranslateTest001
 * @tc.desc: test for seting Matrix to Matrix constructed from translation (dx, dy) multiplied by Matrix.
 * @tc.type: FUNC
 * @tc.require: I73WU0
 */
HWTEST_F(MatrixTest, MatrixPreTranslateTest001, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrix = std::make_unique<Matrix>();
    ASSERT_TRUE(matrix != nullptr);
    matrix->PreTranslate(20.8f, 100);
}

/**
 * @tc.name: MatrixPreTranslateTest002
 * @tc.desc: test for sets Matrix to Matrix constructed from translation (dx, dy) multiplied by Matrix.
 * @tc.type: FUNC
 * @tc.require: I73WU0
 */
HWTEST_F(MatrixTest, MatrixPreTranslateTest002, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrix = std::make_unique<Matrix>();
    ASSERT_TRUE(matrix != nullptr);
    matrix->PreTranslate(77.7f, 190.2f);
}

/**
 * @tc.name: MatrixPreScaleTest001
 * @tc.desc: test for seting Matrix to Matrix multiplied by Matrix constructed from scaling by (sx, sy).
 * @tc.type: FUNC
 * @tc.require: I73WU0
 */
HWTEST_F(MatrixTest, MatrixPreScaleTest001, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrix = std::make_unique<Matrix>();
    ASSERT_TRUE(matrix != nullptr);
    matrix->PreScale(32.1f, 10.6f);
}

/**
 * @tc.name: MatrixPreScaleTest002
 * @tc.desc: test for seting Matrix to Matrix multiplied by Matrix constructed from scaling by (sx, sy).
 * @tc.type: FUNC
 * @tc.require: I73WU0
 */
HWTEST_F(MatrixTest, MatrixPreScaleTest002, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrix = std::make_unique<Matrix>();
    ASSERT_TRUE(matrix != nullptr);
    matrix->PreScale(16.5f, 50.6f);
}

/**
 * @tc.name: MatrixPreConcatTest001
 * @tc.desc: test for seting Matrix to Matrix other multiplied by Matrix.
 * @tc.type: FUNC
 * @tc.require: I73WU0
 */
HWTEST_F(MatrixTest, MatrixPreConcatTest001, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrix1 = std::make_unique<Matrix>();
    std::unique_ptr<Matrix> matrix2 = std::make_unique<Matrix>();
    ASSERT_TRUE(matrix1 != nullptr);
    ASSERT_TRUE(matrix2 != nullptr);
    matrix1->PreConcat(*matrix2.get());
}

/**
 * @tc.name: MatrixMultiplyTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(MatrixTest, MatrixMultiplyTest001, TestSize.Level1)
{
    Matrix matrix1;
    Matrix matrix2;
    matrix1.Rotate(3.0f, 2.0f, 1.0f);
    matrix2.Rotate(1.5f, 2.5f, 3.5f);
    Matrix matrix3 = matrix1 * matrix2;
    EXPECT_TRUE(matrix3 == matrix1);
    EXPECT_FALSE(matrix3 == matrix2);
}

/**
 * @tc.name: MatrixMultiplyTest002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(MatrixTest, MatrixMultiplyTest002, TestSize.Level1)
{
    Matrix matrix1;
    Matrix matrix2;
    matrix1.Rotate(3.7f, 6.0f, 1.9f);
    matrix2.Rotate(5.5f, 12.5f, 30.5f);
    Matrix matrix3 = matrix1 * matrix2;
    EXPECT_TRUE(matrix3 == matrix1);
    EXPECT_FALSE(matrix3 == matrix2);
}

/**
 * @tc.name: MatrixEqualTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(MatrixTest, MatrixEqualTest001, TestSize.Level1)
{
    Matrix matrix1;
    Matrix matrix2 = matrix1;
    EXPECT_TRUE(matrix1 == matrix2);
}

/**
 * @tc.name: MatrixEqualTest002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(MatrixTest, MatrixEqualTest002, TestSize.Level1)
{
    Matrix matrix1;
    matrix1.Rotate(1.0f, 2.0f, 3.0f);
    Matrix matrix2;
    EXPECT_FALSE(matrix1 == matrix2);
}

/**
 * @tc.name: MatrixSetMatrixTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(MatrixTest, MatrixSetMatrixTest001, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrix = std::make_unique<Matrix>();
    ASSERT_TRUE(matrix != nullptr);
    matrix->SetMatrix(200, 150, 800, 60, 200, 150, 800, 60, 90);
}

/**
 * @tc.name: MatrixSetMatrixTest002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(MatrixTest, MatrixSetMatrixTest002, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrix = std::make_unique<Matrix>();
    ASSERT_TRUE(matrix != nullptr);
    matrix->SetMatrix(20.9f, 15.8f, 80.8f, 60, 2.4f, 99.9f, 60, 60, 900);
}

/**
 * @tc.name: MatrixSetMatrixTest003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(MatrixTest, MatrixSetMatrixTest003, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrix = std::make_unique<Matrix>();
    ASSERT_TRUE(matrix != nullptr);
    matrix->SetMatrix(20.9f, 15.8f, 80.8f, 60.6f, 2.4f, 99.9f, 60.5f, 60.1f, 90.5f);
}

/**
 * @tc.name: MatrixMapPointsTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(MatrixTest, MatrixMapPointsTest001, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrix = std::make_unique<Matrix>();
    ASSERT_TRUE(matrix != nullptr);
    std::vector<Point> dst = { { 1, 2 } };
    std::vector<Point> src = { { 2, 3 } };
    matrix->MapPoints(dst, src, 1);
}

/**
 * @tc.name: MatrixMapPointsTest002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(MatrixTest, MatrixMapPointsTest002, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrix = std::make_unique<Matrix>();
    ASSERT_TRUE(matrix != nullptr);
    std::vector<Point> dst = { { 3, 2 } };
    std::vector<Point> src = { { 1, 3 } };
    matrix->MapPoints(dst, src, 1);
}

/**
 * @tc.name: MatrixMapRectTest001
 * @tc.desc: test for seting dst to bounds of src corners mapped by Matrix.
 * @tc.type: FUNC
 * @tc.require: I73WU0
 */
HWTEST_F(MatrixTest, MatrixMapRectTest001, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrix = std::make_unique<Matrix>();
    ASSERT_TRUE(matrix != nullptr);
    RectF dst;
    RectF src(40, 50, 190, 200);
    EXPECT_TRUE(matrix->MapRect(dst, src));
}

/**
 * @tc.name: MatrixMapRectTest002
 * @tc.desc: test for seting dst to bounds of src corners mapped by Matrix.
 * @tc.type: FUNC
 * @tc.require: I73WU0
 */
HWTEST_F(MatrixTest, MatrixMapRectTest002, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrix = std::make_unique<Matrix>();
    ASSERT_TRUE(matrix != nullptr);
    RectF dst;
    RectF src(80, 100, 190, 200);
    EXPECT_TRUE(matrix->MapRect(dst, src));
}

/**
 * @tc.name: MatrixSetPolyToPolyTest001
 * @tc.desc: test for set poly to poly of Matrix.
 * @tc.type: FUNC
 * @tc.require: AR20240104201189
 */
HWTEST_F(MatrixTest, MatrixSetPolyToPolyTest001, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrix = std::make_unique<Matrix>();
    ASSERT_TRUE(matrix != nullptr);
    matrix->SetMatrix(
        1, 0, 0,
        0, -1, 0,
        0, 0, 1);
    ASSERT_TRUE(matrix != nullptr);
    Point src[] = {{0, 0}, {100, 0}, {100, 100}, {0, 100}, {0, 100}};
    Point dst[] = {{0, 0}, {100, 30}, {100, 70}, {0, 100}, {0, 100}};
    EXPECT_TRUE(matrix->SetPolyToPoly(src, dst, 0));
}

/**
 * @tc.name: MatrixSetPolyToPolyTest002
 * @tc.desc: test for set poly to poly of Matrix.
 * @tc.type: FUNC
 * @tc.require: AR20240104201189
 */
HWTEST_F(MatrixTest, MatrixSetPolyToPolyTest002, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrix = std::make_unique<Matrix>();
    matrix->SetMatrix(
        1, 0, 0,
        0, -1, 0,
        0, 0, 1);
    ASSERT_TRUE(matrix != nullptr);
    Point src[] = {{0, 0}, {100, 0}, {100, 100}, {0, 100}, {0, 100}};
    Point dst[] = {{0, 0}, {100, 30}, {100, 70}, {0, 100}, {0, 100}};
    EXPECT_TRUE(matrix->SetPolyToPoly(src, dst, 1));
}

/**
 * @tc.name: MatrixSetPolyToPolyTest003
 * @tc.desc: test for set poly to poly of Matrix.
 * @tc.type: FUNC
 * @tc.require: AR20240104201189
 */
HWTEST_F(MatrixTest, MatrixSetPolyToPolyTest003, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrix = std::make_unique<Matrix>();
    matrix->SetMatrix(
        1, 0, 0,
        0, -1, 0,
        0, 0, 1);
    ASSERT_TRUE(matrix != nullptr);
    Point src[] = {{0, 0}, {100, 0}, {100, 100}, {0, 100}, {0, 100}};
    Point dst[] = {{0, 0}, {100, 30}, {100, 70}, {0, 100}, {0, 100}};
    EXPECT_TRUE(matrix->SetPolyToPoly(src, dst, 4));
}

/**
 * @tc.name: MatrixSetPolyToPolyTest004
 * @tc.desc: test for set poly to poly of Matrix.
 * @tc.type: FUNC
 * @tc.require: AR20240104201189
 */
HWTEST_F(MatrixTest, MatrixSetPolyToPolyTest004, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrix = std::make_unique<Matrix>();
    matrix->SetMatrix(
        1, 0, 0,
        0, -1, 0,
        0, 0, 1);
    ASSERT_TRUE(matrix != nullptr);
    Point src[] = {{0, 0}, {100, 0}, {100, 100}, {0, 100}, {0, 100}};
    Point dst[] = {{0, 0}, {100, 30}, {100, 70}, {0, 100}, {0, 100}};
    EXPECT_FALSE(matrix->SetPolyToPoly(src, dst, 5));
}
/**
 * @tc.name: MatrixSetTest001
 * @tc.desc: test for seting Matrix value.
 * @tc.type: FUNC
 * @tc.require: I73WU0
 */
HWTEST_F(MatrixTest, MatrixSetTest001, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrix = std::make_unique<Matrix>();
    ASSERT_TRUE(matrix != nullptr);
    matrix->Set(Matrix::SCALE_X, 20.9f);
    EXPECT_EQ(matrix->Get(Matrix::SCALE_X), 20.9f);
    matrix->Set(Matrix::SKEW_X, 15.8f);
    EXPECT_EQ(matrix->Get(Matrix::SKEW_X), 15.8f);
    matrix->Set(Matrix::TRANS_X, 80.8f);
    EXPECT_EQ(matrix->Get(Matrix::TRANS_X), 80.8f);
    matrix->Set(Matrix::SKEW_Y, 60.6f);
    EXPECT_EQ(matrix->Get(Matrix::SKEW_Y), 60.6f);
    matrix->Set(Matrix::SCALE_Y, 2.4f);
    EXPECT_EQ(matrix->Get(Matrix::SCALE_Y), 2.4f);
    matrix->Set(Matrix::TRANS_Y, 99.9f);
    EXPECT_EQ(matrix->Get(Matrix::TRANS_Y), 99.9f);
    matrix->Set(Matrix::PERSP_0, 60.5f);
    EXPECT_EQ(matrix->Get(Matrix::PERSP_0), 60.5f);
    matrix->Set(Matrix::PERSP_1, 60.1f);
    EXPECT_EQ(matrix->Get(Matrix::PERSP_1), 60.1f);
    matrix->Set(Matrix::PERSP_2, 90.5f);
    EXPECT_EQ(matrix->Get(Matrix::PERSP_2), 90.5f);
}

/**
 * @tc.name: MatrixGetTest001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:AR000GGNV3
 * @tc.author:
 */
HWTEST_F(MatrixTest, MatrixGetTest001, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrix = std::make_unique<Matrix>();
    ASSERT_TRUE(matrix != nullptr);
    ASSERT_EQ(1, matrix->Get(0));
}

/**
 * @tc.name: GetAll001
 * @tc.desc: the test copies the nine scalar values contained in the matrix into a buffer.
 * @tc.type: FUNC
 * @tc.require: I73WU0
 */
HWTEST_F(MatrixTest, GetAll001, TestSize.Level1)
{
    std::unique_ptr<Matrix> matrix = std::make_unique<Matrix>();
    ASSERT_TRUE(matrix != nullptr);
    matrix->SetMatrix(20.9f, 15.8f, 80.8f, 60.6f, 2.4f, 99.9f, 60.5f, 60.1f, 90.5f);
    Matrix::Buffer buffer;
    matrix->GetAll(buffer);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

