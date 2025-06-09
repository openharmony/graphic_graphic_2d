/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <cstddef>
#include "gtest/gtest.h"
#include "skia_adapter/skia_matrix.h"
#include "utils/matrix.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaMatrixTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SkiaMatrixTest::SetUpTestCase() {}
void SkiaMatrixTest::TearDownTestCase() {}
void SkiaMatrixTest::SetUp() {}
void SkiaMatrixTest::TearDown() {}

/**
 * @tc.name: Multiply001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaMatrixTest, Multiply001, TestSize.Level1)
{
    Matrix matrix1;
    Matrix matrix2;
    SkiaMatrix skiaMatrix;
    skiaMatrix.Multiply(matrix1, matrix2);
    EXPECT_EQ(skiaMatrix.Get(0), 1);
}

/**
 * @tc.name: MapPoints001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaMatrixTest, MapPoints001, TestSize.Level1)
{
    PointF point1;
    PointF point2;
    std::vector<Point> dst { point1 };
    std::vector<Point> src { point2 };
    SkiaMatrix skiaMatrix;
    skiaMatrix.MapPoints(dst, src, 0);
    skiaMatrix.MapPoints(dst, src, 1);
    EXPECT_EQ(skiaMatrix.Get(0), 1);
}

/**
 * @tc.name: Equals001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaMatrixTest, Equals001, TestSize.Level1)
{
    Matrix matrix1;
    Matrix matrix2;
    SkiaMatrix skiaMatrix;
    EXPECT_EQ(skiaMatrix.Equals(matrix1, matrix2), true);
}

/**
 * @tc.name: Scale001
 * @tc.desc: Test Scale
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaMatrixTest, Scale001, TestSize.Level1)
{
    SkiaMatrix skiaMatrix;
    scalar matrixParam = 5;
    skiaMatrix.SetMatrix(matrixParam, matrixParam, matrixParam, matrixParam, matrixParam,
                         matrixParam, matrixParam, matrixParam, matrixParam);
    scalar scale = 2;
    skiaMatrix.Scale(scale, scale, scale, scale);
    ASSERT_TRUE(IsScalarAlmostEqual(skiaMatrix.Get(Matrix::SCALE_X), scale));
}

/**
 * @tc.name: PreRotate001
 * @tc.desc: Test PreRotate
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaMatrixTest, PreRotate001, TestSize.Level1)
{
    SkiaMatrix skiaMatrix;
    scalar matrixParam = 5;
    skiaMatrix.SetMatrix(matrixParam, matrixParam, matrixParam, matrixParam, matrixParam,
                         matrixParam, matrixParam, matrixParam, matrixParam);
    skiaMatrix.PreRotate(90); // 90: degree
    ASSERT_TRUE(IsScalarAlmostEqual(skiaMatrix.Get(Matrix::SCALE_X), matrixParam));
}

/**
 * @tc.name: PreRotate002
 * @tc.desc: Test PreRotate
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaMatrixTest, PreRotate002, TestSize.Level1)
{
    SkiaMatrix skiaMatrix;
    scalar matrixParam = 5;
    skiaMatrix.SetMatrix(matrixParam, matrixParam, matrixParam, matrixParam, matrixParam,
                         matrixParam, matrixParam, matrixParam, matrixParam);
    skiaMatrix.PreRotate(90, 0, 0); // 90: degree
    ASSERT_TRUE(IsScalarAlmostEqual(skiaMatrix.Get(Matrix::SCALE_X), matrixParam));
}

/**
 * @tc.name: PostRotate001
 * @tc.desc: Test PostRotate
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaMatrixTest, PostRotate001, TestSize.Level1)
{
    SkiaMatrix skiaMatrix;
    scalar matrixParam = 5;
    skiaMatrix.SetMatrix(matrixParam, matrixParam, matrixParam, matrixParam, matrixParam,
                         matrixParam, matrixParam, matrixParam, matrixParam);
    skiaMatrix.PostRotate(90); // 90: degree
    ASSERT_TRUE(IsScalarAlmostEqual(skiaMatrix.Get(Matrix::SCALE_X), -matrixParam));
}

/**
 * @tc.name: PostTranslate001
 * @tc.desc: Test PostTranslate
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaMatrixTest, PostTranslate001, TestSize.Level1)
{
    SkiaMatrix skiaMatrix;
    scalar matrixParam = 5;
    skiaMatrix.SetMatrix(matrixParam, matrixParam, matrixParam, matrixParam, matrixParam,
                         matrixParam, matrixParam, matrixParam, matrixParam);
    scalar trans = 100;
    skiaMatrix.PostTranslate(trans, trans);
    ASSERT_TRUE(IsScalarAlmostEqual(skiaMatrix.Get(Matrix::TRANS_X), matrixParam * trans + matrixParam));
}

/**
 * @tc.name: PreScale001
 * @tc.desc: Test PreScale
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaMatrixTest, PreScale001, TestSize.Level1)
{
    SkiaMatrix skiaMatrix;
    scalar matrixParam = 5;
    skiaMatrix.SetMatrix(matrixParam, matrixParam, matrixParam, matrixParam, matrixParam,
                         matrixParam, matrixParam, matrixParam, matrixParam);
    scalar scale = 2;
    skiaMatrix.PreScale(scale, scale);
    ASSERT_TRUE(IsScalarAlmostEqual(skiaMatrix.Get(Matrix::SCALE_X), scale * matrixParam));
}

/**
 * @tc.name: PostScale001
 * @tc.desc: Test PostScale
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaMatrixTest, PostScale001, TestSize.Level1)
{
    SkiaMatrix skiaMatrix;
    scalar matrixParam = 5;
    skiaMatrix.SetMatrix(matrixParam, matrixParam, matrixParam, matrixParam, matrixParam,
                         matrixParam, matrixParam, matrixParam, matrixParam);
    scalar scale = 2;
    skiaMatrix.PostScale(scale, scale);
    ASSERT_TRUE(IsScalarAlmostEqual(skiaMatrix.Get(Matrix::SCALE_X), matrixParam * scale));
}

/**
 * @tc.name: PreConcat001
 * @tc.desc: Test PreConcat
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaMatrixTest, PreConcat001, TestSize.Level1)
{
    SkiaMatrix skiaMatrix;
    scalar matrixParam = 5;
    skiaMatrix.SetMatrix(matrixParam, matrixParam, matrixParam, matrixParam, matrixParam,
                         matrixParam, matrixParam, matrixParam, matrixParam);
    Matrix44 matrix44;
    skiaMatrix.PreConcat(matrix44);
    ASSERT_TRUE(IsScalarAlmostEqual(skiaMatrix.Get(Matrix::SCALE_X), matrixParam));
}

/**
 * @tc.name: PostConcat001
 * @tc.desc: Test PostConcat
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaMatrixTest, PostConcat001, TestSize.Level1)
{
    SkiaMatrix skiaMatrix;
    scalar matrixParam = 5;
    skiaMatrix.SetMatrix(matrixParam, matrixParam, matrixParam, matrixParam, matrixParam,
                         matrixParam, matrixParam, matrixParam, matrixParam);
    Matrix matrix;
    skiaMatrix.PostConcat(matrix);
    ASSERT_TRUE(IsScalarAlmostEqual(skiaMatrix.Get(Matrix::SCALE_X), matrixParam));
}

/**
 * @tc.name: PostConcat002
 * @tc.desc: Test PostConcat
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaMatrixTest, PostConcat002, TestSize.Level1)
{
    SkiaMatrix skiaMatrix;
    scalar matrixParam = 5;
    skiaMatrix.SetMatrix(matrixParam, matrixParam, matrixParam, matrixParam, matrixParam,
                         matrixParam, matrixParam, matrixParam, matrixParam);
    Matrix44 matrix44;
    skiaMatrix.PostConcat(matrix44);
    ASSERT_TRUE(IsScalarAlmostEqual(skiaMatrix.Get(Matrix::SCALE_X), matrixParam));
}

/**
 * @tc.name: SetMatrix001
 * @tc.desc: Test SetMatrix
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaMatrixTest, SetMatrix001, TestSize.Level1)
{
    SkiaMatrix skiaMatrix;
    skiaMatrix.SetMatrix(5, 5, 5, 5, 5, 5, 5, 5, 5);
    ASSERT_TRUE(skiaMatrix.Get(Matrix::SKEW_X) == 5);
    ASSERT_TRUE(skiaMatrix.Get(Matrix::TRANS_X) == 5);
    ASSERT_TRUE(skiaMatrix.Get(Matrix::SCALE_X) == 5);
    ASSERT_TRUE(skiaMatrix.Get(Matrix::SCALE_Y) == 5);
    ASSERT_TRUE(skiaMatrix.Get(Matrix::TRANS_X) == 5);
    ASSERT_TRUE(skiaMatrix.Get(Matrix::TRANS_Y) == 5);
}

/**
 * @tc.name: SetSkew001
 * @tc.desc: Test SetSkew
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaMatrixTest, SetSkew001, TestSize.Level1)
{
    SkiaMatrix skiaMatrix;
    skiaMatrix.SetSkew(10.0, 10.0);
    ASSERT_TRUE(skiaMatrix.Get(Matrix::SKEW_X) == 10);
    ASSERT_TRUE(skiaMatrix.Get(Matrix::SKEW_Y) == 10);
}

/**
 * @tc.name: SetSkew002
 * @tc.desc: Test SetSkew
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaMatrixTest, SetSkew002, TestSize.Level1)
{
    SkiaMatrix skiaMatrix;
    skiaMatrix.SetSkew(10.0, 10.0, 20.0, 20);
    ASSERT_TRUE(skiaMatrix.Get(Matrix::SKEW_X) == 10);
    ASSERT_TRUE(skiaMatrix.Get(Matrix::SKEW_Y) == 10);
    ASSERT_TRUE(skiaMatrix.Get(Matrix::TRANS_X) == -10*20);
    ASSERT_TRUE(skiaMatrix.Get(Matrix::TRANS_Y) == -10*20);
}

/**
 * @tc.name: PreSkew001
 * @tc.desc: Test PreSkew
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaMatrixTest, PreSkew001, TestSize.Level1)
{
    SkiaMatrix skiaMatrix;
    skiaMatrix.PreSkew(5.0, 5.0);
    ASSERT_TRUE(skiaMatrix.Get(Matrix::SKEW_X) == 5);
    ASSERT_TRUE(skiaMatrix.Get(Matrix::SKEW_Y) == 5);
}

/**
 * @tc.name: PreSkew002
 * @tc.desc: Test PreSkew
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaMatrixTest, PreSkew002, TestSize.Level1)
{
    SkiaMatrix skiaMatrix;
    skiaMatrix.PreSkew(10.0, 10.0, 20.0, 20);
    ASSERT_TRUE(skiaMatrix.Get(Matrix::SKEW_X) == 10);
    ASSERT_TRUE(skiaMatrix.Get(Matrix::SKEW_Y) == 10);
    ASSERT_TRUE(skiaMatrix.Get(Matrix::TRANS_X) == -10*20);
    ASSERT_TRUE(skiaMatrix.Get(Matrix::TRANS_Y) == -10*20);
}

/**
 * @tc.name: PostSkew001
 * @tc.desc: Test PostSkew
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaMatrixTest, PostSkew001, TestSize.Level1)
{
    SkiaMatrix skiaMatrix;
    skiaMatrix.PostSkew(5.0, 5.0);
    ASSERT_TRUE(skiaMatrix.Get(Matrix::SKEW_X) == 5);
    ASSERT_TRUE(skiaMatrix.Get(Matrix::SKEW_Y) == 5);
}

/**
 * @tc.name: PostSkew002
 * @tc.desc: Test PostSkew
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaMatrixTest, PostSkew002, TestSize.Level1)
{
    SkiaMatrix skiaMatrix;
    skiaMatrix.PostSkew(10.0, 10.0, 20.0, 20);
    ASSERT_TRUE(skiaMatrix.Get(Matrix::SKEW_X) == 10);
    ASSERT_TRUE(skiaMatrix.Get(Matrix::SKEW_Y) == 10);
    ASSERT_TRUE(skiaMatrix.Get(Matrix::TRANS_X) == -10*20);
    ASSERT_TRUE(skiaMatrix.Get(Matrix::TRANS_Y) == -10*20);
}

/**
 * @tc.name: Invert001
 * @tc.desc: Test Invert
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaMatrixTest, Invert001, TestSize.Level1)
{
    SkiaMatrix skiaMatrix;
    Matrix inverse;
    ASSERT_TRUE(skiaMatrix.Invert(inverse) == true);
}

/**
 * @tc.name: SetSinCos001
 * @tc.desc: Test SetSinCos
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaMatrixTest, SetSinCos001, TestSize.Level1)
{
    SkiaMatrix skiaMatrix;
    skiaMatrix.SetSinCos(2, 0.5, 0.5, 2);
    ASSERT_TRUE(skiaMatrix.Get(Matrix::SCALE_X) == 0.5);
    ASSERT_TRUE(skiaMatrix.Get(Matrix::SCALE_Y) == 0.5);
}

/**
 * @tc.name: MapRadius001
 * @tc.desc: Test MapRadius
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaMatrixTest, MapRadius001, TestSize.Level1)
{
    SkiaMatrix skiaMatrix;
    skiaMatrix.SetSkew(2, 0.5, 0.5, 2);
    float error = skiaMatrix.MapRadius(1) - 1.581139;
    ASSERT_TRUE(error <= 1e-4);
}

/**
 * @tc.name: IsAffine001
 * @tc.desc: Test IsAffine
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaMatrixTest, IsAffine001, TestSize.Level1)
{
    SkiaMatrix skiaMatrix;
    skiaMatrix.SetSkew(2, 0.5, 0.5, 2);
    ASSERT_TRUE(skiaMatrix.IsAffine());
}

/**
 * @tc.name: RectStayRect001
 * @tc.desc: Test RectStayRect
 * @tc.type: FUNC
 * @tc.require: I91EH1
 */
HWTEST_F(SkiaMatrixTest, RectStayRect001, TestSize.Level1)
{
    SkiaMatrix skiaMatrix;
    skiaMatrix.SetSkew(2, 0.5, 0.5, 2);
    ASSERT_FALSE(skiaMatrix.RectStaysRect());
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS