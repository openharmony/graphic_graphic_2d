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
#include "drawing_error_code.h"
#include "drawing_path.h"
#include "drawing_path_effect.h"
#include "drawing_pen.h"

#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class NativeDrawingPathEffectTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void NativeDrawingPathEffectTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
}
void NativeDrawingPathEffectTest::TearDownTestCase() {}
void NativeDrawingPathEffectTest::SetUp() {}
void NativeDrawingPathEffectTest::TearDown() {}

/*
 * @tc.name: NativeDrawingPathEffectTest_PathEffect001
 * @tc.desc: test for PathEffect.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPathEffectTest, NativeDrawingPathEffectTest_PathEffect001, TestSize.Level1)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    EXPECT_NE(pen, nullptr);
    float intervals[] = {1, 1, 1};
    OH_Drawing_PathEffect* pathEffect = OH_Drawing_CreateDashPathEffect(intervals, 3, 0.0);
    OH_Drawing_PenSetPathEffect(nullptr, pathEffect);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_PenSetPathEffect(pen, pathEffect);
    OH_Drawing_PathEffectDestroy(pathEffect);
    // 3 is the number of elements of the intervals array
    pathEffect = OH_Drawing_CreateDashPathEffect(nullptr, 3, 0.0);
    EXPECT_EQ(pathEffect, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    pathEffect = OH_Drawing_CreateDashPathEffect(intervals, 0, 0.0);
    EXPECT_EQ(pathEffect, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    pathEffect = OH_Drawing_CreateDashPathEffect(intervals, -1, 0.0);
    EXPECT_EQ(pathEffect, nullptr);
}

/*
 * @tc.name: NativeDrawingPathEffectTest_CreatePathDashEffect002
 * @tc.desc: test for CreatePathDashEffect.
 * @tc.type: FUNC
 * @tc.require: IBHEL5
 */
HWTEST_F(NativeDrawingPathEffectTest, NativeDrawingPathEffectTest_CreatePathDashEffect002, TestSize.Level1)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    EXPECT_NE(pen, nullptr);
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    EXPECT_NE(path, nullptr);
    // 10 is the distance between the dashed segments.
    OH_Drawing_PathEffect* pathEffect = OH_Drawing_CreatePathDashEffect(path, 10.0, 10.0,
        OH_Drawing_PathDashStyle::DRAWING_PATH_DASH_STYLE_TRANSLATE);
    EXPECT_NE(pathEffect, nullptr);
    OH_Drawing_PenSetPathEffect(nullptr, pathEffect);
    OH_Drawing_PenSetPathEffect(pen, pathEffect);

    OH_Drawing_PathEffect* pathEffect2 = OH_Drawing_CreatePathDashEffect(nullptr,
        10.0, 10.0, OH_Drawing_PathDashStyle::DRAWING_PATH_DASH_STYLE_TRANSLATE);
    EXPECT_EQ(pathEffect2, nullptr);

    pathEffect2 = OH_Drawing_CreatePathDashEffect(path,
        -10.0, 10.0, OH_Drawing_PathDashStyle::DRAWING_PATH_DASH_STYLE_ROTATE);
    EXPECT_EQ(pathEffect2, nullptr);

    pathEffect2 = OH_Drawing_CreatePathDashEffect(path,
        10.0, -10.0, OH_Drawing_PathDashStyle::DRAWING_PATH_DASH_STYLE_MORPH);
    EXPECT_NE(pathEffect2, nullptr);
    OH_Drawing_PenDestroy(pen);
    OH_Drawing_PathDestroy(path);
    OH_Drawing_PathEffectDestroy(pathEffect);
    OH_Drawing_PathEffectDestroy(pathEffect2);
}

/*
 * @tc.name: NativeDrawingPathEffectTest_CreateSumPathEffect003
 * @tc.desc: test for CreatePathDashEffect.
 * @tc.type: FUNC
 * @tc.require: IBHEL5
 */
HWTEST_F(NativeDrawingPathEffectTest, NativeDrawingPathEffectTest_CreateSumPathEffect003, TestSize.Level1)
{
    float intervals[] = {1, 1, 1};
    // 3 is the number of elements of the intervals array
    OH_Drawing_PathEffect* pathEffectOne = OH_Drawing_CreateDashPathEffect(intervals, 3, 0.0);
    EXPECT_NE(pathEffectOne, nullptr);
    OH_Drawing_PathEffect* pathEffectTwo = OH_Drawing_CreateDashPathEffect(intervals, 3, 0.0);
    EXPECT_NE(pathEffectTwo, nullptr);
    OH_Drawing_PathEffect* pathEffectSum = OH_Drawing_CreateSumPathEffect(nullptr, pathEffectTwo);
    EXPECT_EQ(pathEffectSum, nullptr);
    pathEffectSum = OH_Drawing_CreateSumPathEffect(pathEffectOne, nullptr);
    EXPECT_EQ(pathEffectSum, nullptr);
    pathEffectSum = OH_Drawing_CreateSumPathEffect(pathEffectOne, pathEffectTwo);
    EXPECT_NE(pathEffectSum, nullptr);
    OH_Drawing_PathEffectDestroy(pathEffectOne);
    OH_Drawing_PathEffectDestroy(pathEffectTwo);
    OH_Drawing_PathEffectDestroy(pathEffectSum);
}

/*
 * @tc.name: NativeDrawingPathEffectTest_PathEffect004
 * @tc.desc: test for PathEffect.
 * @tc.type: FUNC
 * @tc.require: IBHEL5
 */
HWTEST_F(NativeDrawingPathEffectTest, NativeDrawingPathEffectTest_CreateDiscretePathEffect004, TestSize.Level1)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    EXPECT_NE(pen, nullptr);
    float segLength = 1.0;
    float dev = 1.0;
    OH_Drawing_PathEffect* pathEffect = OH_Drawing_CreateDiscretePathEffect(segLength, dev);
    EXPECT_NE(pathEffect, nullptr);
    OH_Drawing_PenSetPathEffect(pen, pathEffect);
    OH_Drawing_PathEffect* pathEffect1 = OH_Drawing_CreateDiscretePathEffect(0.0, dev);
    EXPECT_NE(pathEffect1, nullptr);
    OH_Drawing_PathEffect* pathEffect2 = OH_Drawing_CreateDiscretePathEffect(segLength, 0.0);
    EXPECT_NE(pathEffect2, nullptr);
    OH_Drawing_PathEffect* pathEffect3 = OH_Drawing_CreateDiscretePathEffect(-2.0, dev); // -2.0 means segLength
    EXPECT_NE(pathEffect3, nullptr);
    OH_Drawing_PathEffect* pathEffect4 = OH_Drawing_CreateDiscretePathEffect(-2.0, -2.0); // -2.0 means segLength
    EXPECT_NE(pathEffect4, nullptr);

    OH_Drawing_PathEffectDestroy(pathEffect);
    OH_Drawing_PathEffectDestroy(pathEffect1);
    OH_Drawing_PathEffectDestroy(pathEffect2);
    OH_Drawing_PathEffectDestroy(pathEffect3);
    OH_Drawing_PathEffectDestroy(pathEffect4);
    OH_Drawing_PenDestroy(pen);
}

/*
 * @tc.name: NativeDrawingPathEffectTest_PathEffect005
 * @tc.desc: test for PathEffect.
 * @tc.type: FUNC
 * @tc.require: IBHEL5
 */
HWTEST_F(NativeDrawingPathEffectTest, NativeDrawingPathEffectTest_CreateCornerPathEffect005, TestSize.Level1)
{
    OH_Drawing_PathEffect* pathEffect = OH_Drawing_CreateCornerPathEffect(0);
    EXPECT_EQ(pathEffect, nullptr);
    pathEffect = OH_Drawing_CreateCornerPathEffect(-2.0); // -2.0 means radius
    EXPECT_EQ(pathEffect, nullptr);
    uint32_t radius = 1;
    pathEffect = OH_Drawing_CreateCornerPathEffect(radius);
    EXPECT_NE(pathEffect, nullptr);
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    EXPECT_NE(pen, nullptr);
    OH_Drawing_PenSetPathEffect(pen, pathEffect);
    OH_Drawing_PathEffectDestroy(pathEffect);
    OH_Drawing_PenDestroy(pen);
}

/*
 * @tc.name: NativeDrawingPathEffectTest_PathEffect006
 * @tc.desc: test for PathEffect.
 * @tc.type: FUNC
 * @tc.require: IBHEL5
 */
HWTEST_F(NativeDrawingPathEffectTest, NativeDrawingPathEffectTest_CreateComposePathEffect006, TestSize.Level1)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    EXPECT_NE(pen, nullptr);
    uint32_t radius = 1;
    OH_Drawing_PathEffect* pathEffect1 = OH_Drawing_CreateCornerPathEffect(radius);
    EXPECT_NE(pathEffect1, nullptr);
    radius = 2; // 2 means radius
    OH_Drawing_PathEffect* pathEffect2 = OH_Drawing_CreateCornerPathEffect(radius);
    EXPECT_NE(pathEffect2, nullptr);
    OH_Drawing_PathEffect* pathEffect = OH_Drawing_CreateComposePathEffect(nullptr, pathEffect2);
    EXPECT_EQ(pathEffect, nullptr);
    pathEffect = OH_Drawing_CreateComposePathEffect(pathEffect1, nullptr);
    EXPECT_EQ(pathEffect, nullptr);
    pathEffect = OH_Drawing_CreateComposePathEffect(pathEffect1, pathEffect2);
    EXPECT_NE(pathEffect, nullptr);
    OH_Drawing_PenSetPathEffect(pen, pathEffect);

    OH_Drawing_PathEffectDestroy(pathEffect1);
    OH_Drawing_PathEffectDestroy(pathEffect2);
    OH_Drawing_PathEffectDestroy(pathEffect);
    OH_Drawing_PenDestroy(pen);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS