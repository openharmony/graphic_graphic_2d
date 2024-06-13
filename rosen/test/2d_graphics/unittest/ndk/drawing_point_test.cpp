/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "drawing_point.h"
#include "gtest/gtest.h"
#include "utils/scalar.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class NativeDrawingPointTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void NativeDrawingPointTest::SetUpTestCase() {}
void NativeDrawingPointTest::TearDownTestCase() {}
void NativeDrawingPointTest::SetUp() {}
void NativeDrawingPointTest::TearDown() {}

/*
 * @tc.name: NativeDrawingPointTest_PointGetAndSet001
 * @tc.desc: test for set and get the x-axis and y-axis coordinates of the point.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPointTest, NativeDrawingPointTest_PointGetAndSet001, TestSize.Level1)
{
    OH_Drawing_Point* centerPt = OH_Drawing_PointCreate(0, 0);
    EXPECT_EQ(OH_Drawing_PointSet(nullptr, 150, 250), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_PointSet(centerPt, 150, 250), OH_DRAWING_SUCCESS); // 150: point's x, 250: point's y
    float x, y;
    EXPECT_EQ(OH_Drawing_PointGetX(centerPt, &x), OH_DRAWING_SUCCESS);
    EXPECT_EQ(OH_Drawing_PointGetY(centerPt, &y), OH_DRAWING_SUCCESS);

    EXPECT_TRUE(IsScalarAlmostEqual(x, 150));
    EXPECT_TRUE(IsScalarAlmostEqual(y, 250));
    OH_Drawing_PointDestroy(centerPt);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS