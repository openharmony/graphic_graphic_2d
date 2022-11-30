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
    skiaMatrix.MapPoints(dst, src, 20);
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

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS