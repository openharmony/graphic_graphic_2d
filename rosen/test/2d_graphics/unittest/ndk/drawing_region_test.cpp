/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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
#include "utils/region.h"

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

/*
 * @tc.name: NativeDrawingRegionTest_region006
 * @tc.desc: test for create drawing_region by copy.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingRegionTest, NativeDrawingRegionTest_RegionCopy001, TestSize.Level1)
{
    OH_Drawing_Region* region = OH_Drawing_RegionCreate();
    ASSERT_TRUE(region != nullptr);
    OH_Drawing_Region* region2 = OH_Drawing_RegionCopy(region);
    ASSERT_TRUE(region2 != nullptr);
    OH_Drawing_Region* region3 = OH_Drawing_RegionCopy(nullptr);
    ASSERT_TRUE(region3 == nullptr);

    OH_Drawing_RegionDestroy(region);
    OH_Drawing_RegionDestroy(region2);
    OH_Drawing_RegionDestroy(region3);
}

/*
 * @tc.name: NativeDrawingRegionTest_region007
 * @tc.desc: test for set drawing_region empty.
 * @tc.type: FUNC
 * @tc.require: 20005
 */
HWTEST_F(NativeDrawingRegionTest, NativeDrawingRegionTest_RegionEmpty007, TestSize.Level1)
{
    OH_Drawing_Region* region = OH_Drawing_RegionCreate();
    EXPECT_TRUE(region != nullptr);
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(100.0f, 100.0f, 256.0f, 256.0f);
    OH_Drawing_RegionSetRect(region, rect);
    Region* regionOriginal = reinterpret_cast<Region*>(region);
    bool isEmptyOriginal = regionOriginal->IsEmpty();
    EXPECT_FALSE(isEmptyOriginal);
    EXPECT_EQ(OH_Drawing_RegionEmpty(nullptr), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_RegionEmpty(region), OH_DRAWING_SUCCESS);
    Region* regionFinal = reinterpret_cast<Region*>(region);
    bool isEmptyFinal = regionFinal->IsEmpty();
    EXPECT_TRUE(isEmptyFinal);

    OH_Drawing_RegionDestroy(region);
    OH_Drawing_RectDestroy(rect);
}

/*
 * @tc.name: NativeDrawingRegionTest_region008
 * @tc.desc: test for get boundary path.
 * @tc.type: FUNC
 * @tc.require: 20649
 */
HWTEST_F(NativeDrawingRegionTest, NativeDrawingRegionTest_RegionGetBoundaryPath008, TestSize.Level1)
{
    OH_Drawing_Region* region1 = OH_Drawing_RegionCreate();
    OH_Drawing_Rect* rect1 = OH_Drawing_RectCreate(100.0f, 100.0f, 300.0f, 300.0f);
    OH_Drawing_Region* region2 = OH_Drawing_RegionCreate();
    OH_Drawing_Rect* rect2 = OH_Drawing_RectCreate(200.0f, 200.0f, 400.0f, 400.0f);
    OH_Drawing_Path* path = OH_Drawing_PathCreate();

    EXPECT_EQ(OH_Drawing_RegionGetBoundaryPath(region1, path), OH_DRAWING_SUCCESS);
    bool isEmpty = false;
    EXPECT_EQ(OH_Drawing_PathIsEmpty(path, &isEmpty), OH_DRAWING_SUCCESS);
    EXPECT_EQ(isEmpty, true);

    OH_Drawing_RegionSetRect(region1, rect1);
    OH_Drawing_RegionSetRect(region2, rect2);
    EXPECT_TRUE(OH_Drawing_RegionOp(region1, region2, OH_Drawing_RegionOpMode::REGION_OP_MODE_UNION));

    EXPECT_EQ(OH_Drawing_RegionGetBoundaryPath(nullptr, path), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_RegionGetBoundaryPath(region1, nullptr), OH_DRAWING_ERROR_INCORRECT_PARAMETER);

    EXPECT_EQ(OH_Drawing_RegionGetBoundaryPath(region1, path), OH_DRAWING_SUCCESS);
    EXPECT_FALSE(OH_Drawing_PathContains(path, 300.01f, 199.99f));
    EXPECT_TRUE(OH_Drawing_PathContains(path, 300.01f, 200.01f));
    EXPECT_TRUE(OH_Drawing_PathContains(path, 299.99f, 199.99f));
    EXPECT_TRUE(OH_Drawing_PathContains(path, 299.99f, 200.01f));

    OH_Drawing_RegionDestroy(region1);
    OH_Drawing_RectDestroy(rect1);
    OH_Drawing_RegionDestroy(region2);
    OH_Drawing_RectDestroy(rect2);
    OH_Drawing_PathDestroy(path);
}

/*
 * @tc.name: NativeDrawingRegionTest_region009
 * @tc.desc: test for get bounds.
 * @tc.type: FUNC
 * @tc.require: 20649
 */
HWTEST_F(NativeDrawingRegionTest, NativeDrawingRegionTest_RegionGetBounds009, TestSize.Level1)
{
    OH_Drawing_Region* region1 = OH_Drawing_RegionCreate();
    OH_Drawing_Rect* rect1 = OH_Drawing_RectCreate(100.0f, 100.0f, 300.0f, 300.0f);
    OH_Drawing_Region* region2 = OH_Drawing_RegionCreate();
    OH_Drawing_Rect* rect2 = OH_Drawing_RectCreate(200.0f, 200.0f, 400.0f, 400.0f);
    OH_Drawing_RegionSetRect(region1, rect1);
    OH_Drawing_RegionSetRect(region2, rect2);
    EXPECT_TRUE(OH_Drawing_RegionOp(region1, region2, OH_Drawing_RegionOpMode::REGION_OP_MODE_UNION));

    EXPECT_EQ(OH_Drawing_RegionGetBounds(nullptr, rect1), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_RegionGetBounds(region1, nullptr), OH_DRAWING_ERROR_INCORRECT_PARAMETER);

    EXPECT_EQ(OH_Drawing_RegionGetBounds(region1, rect1), OH_DRAWING_SUCCESS);
    EXPECT_EQ(OH_Drawing_RectGetLeft(rect1), 100.0f);
    EXPECT_EQ(OH_Drawing_RectGetTop(rect1), 100.0f);
    EXPECT_EQ(OH_Drawing_RectGetRight(rect1), 400.0f);
    EXPECT_EQ(OH_Drawing_RectGetBottom(rect1), 400.0f);

    EXPECT_EQ(OH_Drawing_RegionEmpty(region1), OH_DRAWING_SUCCESS);
    EXPECT_EQ(OH_Drawing_RegionGetBounds(region1, rect1), OH_DRAWING_SUCCESS);
    bool isEmpty = false;
    EXPECT_EQ(OH_Drawing_RectIsEmpty(rect1, &isEmpty), OH_DRAWING_SUCCESS);
    EXPECT_EQ(isEmpty, true);

    OH_Drawing_RegionDestroy(region1);
    OH_Drawing_RectDestroy(rect1);
    OH_Drawing_RegionDestroy(region2);
    OH_Drawing_RectDestroy(rect2);
}

/*
 * @tc.name: NativeDrawingRegionTest_region0010
 * @tc.desc: test for is complex.
 * @tc.type: FUNC
 * @tc.require: 20649
 */
HWTEST_F(NativeDrawingRegionTest, NativeDrawingRegionTest_RegionIsComplex010, TestSize.Level1)
{
    OH_Drawing_Region* region1 = OH_Drawing_RegionCreate();
    OH_Drawing_Rect* rect1 = OH_Drawing_RectCreate(100.0f, 100.0f, 300.0f, 300.0f);
    OH_Drawing_Region* region2 = OH_Drawing_RegionCreate();
    OH_Drawing_Rect* rect2 = OH_Drawing_RectCreate(200.0f, 200.0f, 400.0f, 400.0f);
    OH_Drawing_RegionSetRect(region1, rect1);
    OH_Drawing_RegionSetRect(region2, rect2);
    EXPECT_TRUE(OH_Drawing_RegionOp(region1, region2, OH_Drawing_RegionOpMode::REGION_OP_MODE_UNION));

    bool isComplex = false;
    EXPECT_EQ(OH_Drawing_RegionIsComplex(nullptr, &isComplex), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_RegionIsComplex(region1, nullptr), OH_DRAWING_ERROR_INCORRECT_PARAMETER);

    EXPECT_EQ(OH_Drawing_RegionIsComplex(region1, &isComplex), OH_DRAWING_SUCCESS);
    EXPECT_EQ(isComplex, true);
    EXPECT_EQ(OH_Drawing_RegionIsComplex(region2, &isComplex), OH_DRAWING_SUCCESS);
    EXPECT_EQ(isComplex, false);

    EXPECT_EQ(OH_Drawing_RegionEmpty(region1), OH_DRAWING_SUCCESS);
    EXPECT_EQ(OH_Drawing_RegionIsComplex(region1, &isComplex), OH_DRAWING_SUCCESS);
    EXPECT_EQ(isComplex, false);

    OH_Drawing_RegionDestroy(region1);
    OH_Drawing_RectDestroy(rect1);
    OH_Drawing_RegionDestroy(region2);
    OH_Drawing_RectDestroy(rect2);
}

/*
 * @tc.name: NativeDrawingRegionTest_region0011
 * @tc.desc: test for is empty.
 * @tc.type: FUNC
 * @tc.require: 20649
 */
HWTEST_F(NativeDrawingRegionTest, NativeDrawingRegionTest_RegionIsEmpty011, TestSize.Level1)
{
    OH_Drawing_Region* region1 = OH_Drawing_RegionCreate();
    OH_Drawing_Rect* rect1 = OH_Drawing_RectCreate(100.0f, 100.0f, 300.0f, 300.0f);
    OH_Drawing_Rect* rect2 = OH_Drawing_RectCreate(0.0f, 0.0f, 0.0f, 0.0f);

    bool isEmpty = false;
    EXPECT_EQ(OH_Drawing_RegionIsEmpty(nullptr, &isEmpty), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_RegionIsEmpty(region1, nullptr), OH_DRAWING_ERROR_INCORRECT_PARAMETER);

    EXPECT_EQ(OH_Drawing_RegionIsEmpty(region1, &isEmpty), OH_DRAWING_SUCCESS);
    EXPECT_EQ(isEmpty, true);

    OH_Drawing_RegionSetRect(region1, rect1);
    EXPECT_EQ(OH_Drawing_RegionIsEmpty(region1, &isEmpty), OH_DRAWING_SUCCESS);
    EXPECT_EQ(isEmpty, false);

    EXPECT_EQ(OH_Drawing_RegionEmpty(region1), OH_DRAWING_SUCCESS);
    EXPECT_EQ(OH_Drawing_RegionIsEmpty(region1, &isEmpty), OH_DRAWING_SUCCESS);
    EXPECT_EQ(isEmpty, true);

    OH_Drawing_RegionSetRect(region1, rect2);
    EXPECT_EQ(OH_Drawing_RegionIsEmpty(region1, &isEmpty), OH_DRAWING_SUCCESS);
    EXPECT_EQ(isEmpty, true);

    OH_Drawing_RegionDestroy(region1);
    OH_Drawing_RectDestroy(rect1);
    OH_Drawing_RectDestroy(rect2);
}

/*
 * @tc.name: NativeDrawingRegionTest_region0012
 * @tc.desc: test for is rect.
 * @tc.type: FUNC
 * @tc.require: 20649
 */
HWTEST_F(NativeDrawingRegionTest, NativeDrawingRegionTest_RegionIsRect012, TestSize.Level1)
{
    OH_Drawing_Region* region1 = OH_Drawing_RegionCreate();
    OH_Drawing_Rect* rect1 = OH_Drawing_RectCreate(100.0f, 100.0f, 300.0f, 300.0f);
    OH_Drawing_Region* region2 = OH_Drawing_RegionCreate();
    OH_Drawing_Rect* rect2 = OH_Drawing_RectCreate(200.0f, 200.0f, 400.0f, 400.0f);
    OH_Drawing_RegionSetRect(region1, rect1);
    OH_Drawing_RegionSetRect(region2, rect2);
    EXPECT_TRUE(OH_Drawing_RegionOp(region1, region2, OH_Drawing_RegionOpMode::REGION_OP_MODE_UNION));

    bool isRect = false;
    EXPECT_EQ(OH_Drawing_RegionIsRect(nullptr, &isRect), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_RegionIsRect(region1, nullptr), OH_DRAWING_ERROR_INCORRECT_PARAMETER);

    EXPECT_EQ(OH_Drawing_RegionIsRect(region1, &isRect), OH_DRAWING_SUCCESS);
    EXPECT_EQ(isRect, false);
    EXPECT_EQ(OH_Drawing_RegionIsRect(region2, &isRect), OH_DRAWING_SUCCESS);
    EXPECT_EQ(isRect, true);
    EXPECT_EQ(OH_Drawing_RegionEmpty(region2), OH_DRAWING_SUCCESS);
    EXPECT_EQ(OH_Drawing_RegionIsRect(region2, &isRect), OH_DRAWING_SUCCESS);
    EXPECT_EQ(isRect, false);

    OH_Drawing_RegionDestroy(region1);
    OH_Drawing_RectDestroy(rect1);
    OH_Drawing_RegionDestroy(region2);
    OH_Drawing_RectDestroy(rect2);
}

/*
 * @tc.name: NativeDrawingRegionTest_region0013
 * @tc.desc: test for quick contains.
 * @tc.type: FUNC
 * @tc.require: 20649
 */
HWTEST_F(NativeDrawingRegionTest, NativeDrawingRegionTest_RegionQuickContains013, TestSize.Level1)
{
    OH_Drawing_Region* region1 = OH_Drawing_RegionCreate();
    OH_Drawing_Rect* rect1 = OH_Drawing_RectCreate(100.0f, 100.0f, 300.0f, 300.0f);
    OH_Drawing_Region* region2 = OH_Drawing_RegionCreate();
    OH_Drawing_Rect* rect2 = OH_Drawing_RectCreate(200.0f, 200.0f, 400.0f, 400.0f);
    OH_Drawing_RegionSetRect(region1, rect1);
    OH_Drawing_RegionSetRect(region2, rect2);
    EXPECT_TRUE(OH_Drawing_RegionOp(region1, region2, OH_Drawing_RegionOpMode::REGION_OP_MODE_UNION));

    bool isContains = false;
    EXPECT_EQ(OH_Drawing_RegionQuickContains(nullptr, 100, 100, 200, 200, &isContains),
        OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_RegionQuickContains(region1, 100, 100, 200, 200, nullptr),
        OH_DRAWING_ERROR_INCORRECT_PARAMETER);

    EXPECT_EQ(OH_Drawing_RegionQuickContains(region1, 100, 100, 200, 200, &isContains), OH_DRAWING_SUCCESS);
    EXPECT_EQ(isContains, false);
    EXPECT_EQ(OH_Drawing_RegionQuickContains(region2, 200, 200, 300, 300, &isContains), OH_DRAWING_SUCCESS);
    EXPECT_EQ(isContains, true);
    EXPECT_EQ(OH_Drawing_RegionQuickContains(region2, 200, 200, 500, 500, &isContains), OH_DRAWING_SUCCESS);
    EXPECT_EQ(isContains, false);

    OH_Drawing_RegionDestroy(region1);
    OH_Drawing_RectDestroy(rect1);
    OH_Drawing_RegionDestroy(region2);
    OH_Drawing_RectDestroy(rect2);
}

/*
 * @tc.name: NativeDrawingRegionTest_region0014
 * @tc.desc: test for quick reject.
 * @tc.type: FUNC
 * @tc.require: 20649
 */
HWTEST_F(NativeDrawingRegionTest, NativeDrawingRegionTest_RegionQuickReject014, TestSize.Level1)
{
    OH_Drawing_Region* region1 = OH_Drawing_RegionCreate();
    bool isReject = false;
    EXPECT_EQ(OH_Drawing_RegionQuickReject(region1, 100, 100, 200, 200, &isReject), OH_DRAWING_SUCCESS);
    EXPECT_EQ(isReject, true);

    OH_Drawing_Rect* rect1 = OH_Drawing_RectCreate(100.0f, 100.0f, 300.0f, 300.0f);
    OH_Drawing_Region* region2 = OH_Drawing_RegionCreate();
    OH_Drawing_Rect* rect2 = OH_Drawing_RectCreate(200.0f, 200.0f, 400.0f, 400.0f);
    OH_Drawing_RegionSetRect(region1, rect1);
    OH_Drawing_RegionSetRect(region2, rect2);
    EXPECT_TRUE(OH_Drawing_RegionOp(region1, region2, OH_Drawing_RegionOpMode::REGION_OP_MODE_UNION));

    EXPECT_EQ(OH_Drawing_RegionQuickReject(nullptr, 100, 100, 200, 200, &isReject),
        OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_RegionQuickReject(region1, 100, 100, 200, 200, nullptr),
        OH_DRAWING_ERROR_INCORRECT_PARAMETER);

    EXPECT_EQ(OH_Drawing_RegionQuickReject(region1, 200, 200, 200, 200, &isReject), OH_DRAWING_SUCCESS);
    EXPECT_EQ(isReject, true);
    EXPECT_EQ(OH_Drawing_RegionQuickReject(region1, 200, 200, 500, 500, &isReject), OH_DRAWING_SUCCESS);
    EXPECT_EQ(isReject, false);
    EXPECT_EQ(OH_Drawing_RegionQuickReject(region1, 600, 600, 700, 700, &isReject), OH_DRAWING_SUCCESS);
    EXPECT_EQ(isReject, true);

    OH_Drawing_RegionDestroy(region1);
    OH_Drawing_RectDestroy(rect1);
    OH_Drawing_RegionDestroy(region2);
    OH_Drawing_RectDestroy(rect2);
}

/*
 * @tc.name: NativeDrawingRegionTest_region0015
 * @tc.desc: test for translate.
 * @tc.type: FUNC
 * @tc.require: 20649
 */
HWTEST_F(NativeDrawingRegionTest, NativeDrawingRegionTest_RegionTranslate015, TestSize.Level1)
{
    OH_Drawing_Region* region = OH_Drawing_RegionCreate();
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(100.0f, 100.0f, 300.0f, 300.0f);
    OH_Drawing_RegionSetRect(region, rect);

    EXPECT_EQ(OH_Drawing_RegionTranslate(nullptr, 100, 100), OH_DRAWING_ERROR_INCORRECT_PARAMETER);
    EXPECT_EQ(OH_Drawing_RegionTranslate(region, 100, 100), OH_DRAWING_SUCCESS);
    EXPECT_FALSE(OH_Drawing_RegionContains(region, 150, 150));
    EXPECT_TRUE(OH_Drawing_RegionContains(region, 350, 350));

    OH_Drawing_RegionDestroy(region);
    OH_Drawing_RectDestroy(rect);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS