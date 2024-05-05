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
    skiaMatrix.Scale(2, 2, 2, 2); // 2: sx, sy, px, py
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
    skiaMatrix.PreRotate(90); // 90: degree
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
    skiaMatrix.PreRotate(90, 0, 0); // 90: degree
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
    skiaMatrix.PostRotate(90); // 90: degree
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
    skiaMatrix.PostTranslate(100, 100); // 100: dx, dy
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
    skiaMatrix.PreScale(2, 2); // 2: sx, sy
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
    skiaMatrix.PostScale(2, 2); // 2: sx, sy
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
    Matrix44 matrix44;
    skiaMatrix.PreConcat(matrix44);
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
    Matrix matrix;
    skiaMatrix.PostConcat(matrix);
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
    Matrix44 matrix44;
    skiaMatrix.PostConcat(matrix44);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS