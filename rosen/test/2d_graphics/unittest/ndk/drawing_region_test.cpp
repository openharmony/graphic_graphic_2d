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

#include "gtest/gtest.h"

#include "drawing_error_code.h"
#include "drawing_rect.h"
#include "drawing_region.h"
#include "drawing_path.h"

#ifdef RS_ENABLE_VK
#include "platform/ohos/backend/rs_vulkan_context.h"
#endif

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class NativeDrawingRegionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void NativeDrawingRegionTest::SetUpTestCase()
{
#ifdef RS_ENABLE_VK
    RsVulkanContext::SetRecyclable(false);
#endif
}
void NativeDrawingRegionTest::TearDownTestCase() {}
void NativeDrawingRegionTest::SetUp() {}
void NativeDrawingRegionTest::TearDown() {}

/*
 * @tc.name: NativeDrawingRegionTest_region001
 * @tc.desc: test for create drawing_region.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingRegionTest, NativeDrawingRegionTest_region001, TestSize.Level1)
{
    OH_Drawing_Region* region = OH_Drawing_RegionCreate();
    EXPECT_NE(region, nullptr);
    OH_Drawing_RegionDestroy(region);
}

/*
 * @tc.name: NativeDrawingRegionSetRectTest_region002
 * @tc.desc: test for constructs a rectangular Region matching the bounds of rect.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingRegionTest, NativeDrawingRegionSetRectTest_region002, TestSize.Level1)
{
    OH_Drawing_Region* region = OH_Drawing_RegionCreate();
    OH_Drawing_Rect* rect=OH_Drawing_RectCreate(0.0f, 0.0f, 256.0f, 256.0f);
    EXPECT_TRUE(OH_Drawing_RegionSetRect(region, rect));
    OH_Drawing_RegionSetRect(nullptr, rect);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_RegionSetRect(region, nullptr);
    EXPECT_EQ(OH_Drawing_ErrorCodeGet(), OH_DRAWING_ERROR_INVALID_PARAMETER);
    OH_Drawing_RegionDestroy(region);
    OH_Drawing_RectDestroy(rect);
}

/*
 * @tc.name: NativeDrawingRegionContainsTest_region003
 * @tc.desc: test for determines whether the region contains the specified coordinates.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingRegionTest, NativeDrawingRegionContainsTest_region003, TestSize.Level1)
{
    OH_Drawing_Region* region = OH_Drawing_RegionCreate();
    // rect left[100.0f], top[100.0f],right[256.0f], bottom[256.0f]
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(100.0f, 100.0f, 256.0f, 256.0f);
    OH_Drawing_RegionSetRect(region, rect);
    EXPECT_TRUE(OH_Drawing_RegionContains(region, 150, 180)); // 150: point's x, 180 point's y
    EXPECT_FALSE(OH_Drawing_RegionContains(region, 10, 20)); // 10: point's x, 20 point's y
    EXPECT_FALSE(OH_Drawing_RegionContains(nullptr, 10, 20)); // 10: point's x, 20 point's y
    OH_Drawing_RegionDestroy(region);
    OH_Drawing_RectDestroy(rect);
}

/*
 * @tc.name: NativeDrawingRegionOpTest_region004
 * @tc.desc: test for combines two regions.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingRegionTest, NativeDrawingRegionOpTest_region004, TestSize.Level1)
{
    OH_Drawing_Region* region = OH_Drawing_RegionCreate();
    // rect left[100.0f], top[100.0f],right[256.0f], bottom[256.0f]
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(100.0f, 100.0f, 256.0f, 256.0f);
    OH_Drawing_RegionSetRect(region, rect);
    OH_Drawing_Region* region2 = OH_Drawing_RegionCreate();
    // rect left[150.0f], top[180.0f],right[200.0f], bottom[250.0f]
    OH_Drawing_Rect* rect2 = OH_Drawing_RectCreate(150.0f, 180.0f, 200.0f, 250.0f);
    OH_Drawing_RegionSetRect(region2, rect2);
    EXPECT_TRUE(OH_Drawing_RegionOp(region, region2, OH_Drawing_RegionOpMode::REGION_OP_MODE_INTERSECT));

    // rect left[10.0f], top[10.0f],right[26.0f], bottom[26.0f]
    OH_Drawing_Rect* rect3 = OH_Drawing_RectCreate(10.0f, 10.0f, 26.0f, 26.0f);
    OH_Drawing_RegionSetRect(region2, rect3);
    EXPECT_FALSE(OH_Drawing_RegionOp(region, region2, OH_Drawing_RegionOpMode::REGION_OP_MODE_INTERSECT));
    EXPECT_FALSE(OH_Drawing_RegionOp(nullptr, region2, OH_Drawing_RegionOpMode::REGION_OP_MODE_INTERSECT));
    EXPECT_FALSE(OH_Drawing_RegionOp(region, nullptr, OH_Drawing_RegionOpMode::REGION_OP_MODE_INTERSECT));
    EXPECT_FALSE(OH_Drawing_RegionOp(region, region2, static_cast<OH_Drawing_RegionOpMode>(-1))); // illegal input
    EXPECT_FALSE(OH_Drawing_RegionOp(region, region2, static_cast<OH_Drawing_RegionOpMode>(99))); // illegal input

    OH_Drawing_RegionDestroy(region);
    OH_Drawing_RegionDestroy(region2);
    OH_Drawing_RegionDestroy(nullptr);
    OH_Drawing_RectDestroy(rect);
    OH_Drawing_RectDestroy(rect2);
    OH_Drawing_RectDestroy(rect3);
}

/*
 * @tc.name: NativeDrawingSetPathTest_region005
 * @tc.desc: test for combines two regions.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingRegionTest, NativeDrawingSetPathTest_region005, TestSize.Level1)
{
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_Region* region = OH_Drawing_RegionCreate();
    OH_Drawing_Region* clip = OH_Drawing_RegionCreate();
    // rect left[150.0f], top[180.0f],right[200.0f], bottom[200.0f]
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(150.0f, 180.0f, 200.0f, 200.0f);
    OH_Drawing_RegionSetRect(clip, rect);
    // rect left[100.0f], top[100.0f],right[256.0f], bottom[256.0f]
    OH_Drawing_PathAddRect(path, 100.0f, 100.0f, 256.0f, 256.0f, OH_Drawing_PathDirection::PATH_DIRECTION_CW);
    EXPECT_FALSE(OH_Drawing_RegionSetPath(nullptr, path, clip));
    EXPECT_FALSE(OH_Drawing_RegionSetPath(region, nullptr, clip));
    EXPECT_FALSE(OH_Drawing_RegionSetPath(region, path, nullptr));
    EXPECT_TRUE(OH_Drawing_RegionSetPath(region, path, clip));

    OH_Drawing_RegionDestroy(region);
    OH_Drawing_RegionDestroy(clip);
    OH_Drawing_PathDestroy(path);
    OH_Drawing_RectDestroy(rect);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS