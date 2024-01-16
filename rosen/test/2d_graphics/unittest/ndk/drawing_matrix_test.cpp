/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "c/drawing_matrix.h"

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

void NativeDrawingMatrixTest::SetUpTestCase() {}
void NativeDrawingMatrixTest::TearDownTestCase() {}
void NativeDrawingMatrixTest::SetUp() {}
void NativeDrawingMatrixTest::TearDown() {}

/*
 * @tc.name: NativeDrawingMatrixTest_SetMatrix
 * @tc.desc: test for SetMatrix.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingMatrixTest, NativeDrawingMatrixTest_SetMatrix, TestSize.Level1)
{
    OH_Drawing_Matrix* matrix = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(
        matrix,
        1, 0, 0,
        0, -1, 0,
        0, 0, 1);
    OH_Drawing_MatrixDestroy(matrix);
    OH_Drawing_MatrixDestroy(nullptr);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
