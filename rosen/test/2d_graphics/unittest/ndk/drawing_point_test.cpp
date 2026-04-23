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

#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif

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

void NativeDrawingPointTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
}
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
    EXPECT_EQ(OH_Drawing_PointGetX(nullptr, &x), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_PointGetX(centerPt, nullptr), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_PointGetY(centerPt, &y), OH_DRAWING_SUCCESS);
    EXPECT_EQ(OH_Drawing_PointGetY(nullptr, &y), OH_DRAWING_ERROR_INVALID_PARAMETER);
    EXPECT_EQ(OH_Drawing_PointGetY(centerPt, nullptr), OH_DRAWING_ERROR_INVALID_PARAMETER);

    EXPECT_TRUE(IsScalarAlmostEqual(x, 150));
    EXPECT_TRUE(IsScalarAlmostEqual(y, 250));
    OH_Drawing_PointDestroy(centerPt);
    OH_Drawing_PointDestroy(nullptr);
}

/*
 * @tc.name: NativeDrawingPointTest_PointNegate001
 * @tc.desc: test for negating the coordinates of the point.
 * @tc.type: FUNC
 * @tc.require: 22992
 */
HWTEST_F(NativeDrawingPointTest, NativeDrawingPointTest_PointNegate001, TestSize.Level1)
{
    OH_Drawing_Point* point = OH_Drawing_PointCreate(3.5f, -4.5f);
    EXPECT_EQ(OH_Drawing_PointNegate(point), OH_DRAWING_SUCCESS);
    float x, y;
    OH_Drawing_PointGetX(point, &x);
    OH_Drawing_PointGetY(point, &y);
    EXPECT_TRUE(IsScalarAlmostEqual(x, -3.5f));
    EXPECT_TRUE(IsScalarAlmostEqual(y, 4.5f));
    OH_Drawing_PointDestroy(point);
    EXPECT_EQ(OH_Drawing_PointNegate(nullptr), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
}

/*
 * @tc.name: NativeDrawingPointTest_PointNegate002
 * @tc.desc: test for negating the coordinates of the point (0, 0).
 * @tc.type: FUNC
 * @tc.require: 22992
 */
HWTEST_F(NativeDrawingPointTest, NativeDrawingPointTest_PointNegate002, TestSize.Level1)
{
    OH_Drawing_Point* point1 = OH_Drawing_PointCreate(0.0f, 0.0f);
    EXPECT_EQ(OH_Drawing_PointNegate(point1), OH_DRAWING_SUCCESS);
    float x, y;
    OH_Drawing_PointGetX(point1, &x);
    OH_Drawing_PointGetY(point1, &y);
    EXPECT_TRUE(IsScalarAlmostEqual(x, 0.0f));
    EXPECT_TRUE(IsScalarAlmostEqual(y, 0.0f));
    OH_Drawing_PointDestroy(point1);
}

/*
 * @tc.name: NativeDrawingPointTest_PointOffset001
 * @tc.desc: test for offsetting the coordinates of the point.
 * @tc.type: FUNC
 * @tc.require: 22992
 */
HWTEST_F(NativeDrawingPointTest, NativeDrawingPointTest_PointOffset001, TestSize.Level1)
{
    OH_Drawing_Point* point = OH_Drawing_PointCreate(3.5f, 4.5f);
    EXPECT_EQ(OH_Drawing_PointOffset(point, 2.0f, -1.0f), OH_DRAWING_SUCCESS);
    float x, y;
    OH_Drawing_PointGetX(point, &x);
    OH_Drawing_PointGetY(point, &y);
    EXPECT_TRUE(IsScalarAlmostEqual(x, 5.5f));
    EXPECT_TRUE(IsScalarAlmostEqual(y, 3.5f));
    OH_Drawing_PointDestroy(point);
    EXPECT_EQ(OH_Drawing_PointOffset(nullptr, 1.0f, 2.0f), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
}

/*
 * @tc.name: NativeDrawingPointTest_PointOffset002
 * @tc.desc: test for offsetting the coordinates of the point with zero offset.
 * @tc.type: FUNC
 * @tc.require: 22992
 */
HWTEST_F(NativeDrawingPointTest, NativeDrawingPointTest_PointOffset002, TestSize.Level1)
{
    OH_Drawing_Point* point = OH_Drawing_PointCreate(3.5f, 4.5f);
    EXPECT_EQ(OH_Drawing_PointOffset(point, 0.0f, 0.0f), OH_DRAWING_SUCCESS);
    float x, y;
    OH_Drawing_PointGetX(point, &x);
    OH_Drawing_PointGetY(point, &y);
    EXPECT_TRUE(IsScalarAlmostEqual(x, 3.5f));
    EXPECT_TRUE(IsScalarAlmostEqual(y, 4.5f));
    OH_Drawing_PointDestroy(point);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS