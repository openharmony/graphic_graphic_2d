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

#include "drawing_color.h"
#include "drawing_filter.h"
#include "drawing_mask_filter.h"
#include "drawing_rect.h"
#include "utils/scalar.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class NativeDrawingRectTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void NativeDrawingRectTest::SetUpTestCase() {}
void NativeDrawingRectTest::TearDownTestCase() {}
void NativeDrawingRectTest::SetUp() {}
void NativeDrawingRectTest::TearDown() {}

/*
 * @tc.name: NativeDrawingRectTest_CreateAndDestroy001
 * @tc.desc: test for create Rect and destroy Rect.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingRectTest, NativeDrawingRectTest_CreateAndDestroy001, TestSize.Level1)
{
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(100, 200, 500, 300);
    EXPECT_NE(nullptr, rect);
    OH_Drawing_RectDestroy(rect);
}

/*
 * @tc.name: NativeDrawingRectTest_Intersect002
 * @tc.desc: test for the Intersect methods of Rect.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingRectTest, NativeDrawingRectTest_Intersect002, TestSize.Level1)
{
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(100, 200, 500, 300);
    EXPECT_NE(nullptr, rect);

    OH_Drawing_Rect *otherOne = OH_Drawing_RectCreate(300, 250, 600, 400);
    EXPECT_NE(nullptr, otherOne);

    OH_Drawing_Rect *otherTwo = OH_Drawing_RectCreate(600, 400, 700, 500);
    EXPECT_NE(nullptr, otherTwo);

    bool ret = OH_Drawing_RectIntersect(rect, otherOne);
    EXPECT_EQ(ret, true);

    ret = OH_Drawing_RectIntersect(rect, otherTwo);
    EXPECT_EQ(ret, false);

    OH_Drawing_RectDestroy(rect);
    OH_Drawing_RectDestroy(otherOne);
    OH_Drawing_RectDestroy(otherTwo);
}
/*
 * @tc.name: NativeDrawingRectTest_GetHeight003
 * @tc.desc: test for get height of rect.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingRectTest, NativeDrawingRectTest_GetHeight003, TestSize.Level1)
{
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, 400, 800);
    float height = OH_Drawing_RectGetHeight(rect);
    EXPECT_TRUE(IsScalarAlmostEqual(height, 800)); // 800 means height
    OH_Drawing_RectDestroy(rect);
}

/*
 * @tc.name: NativeDrawingRectTest_GetWidth004
 * @tc.desc: test for get width of rect.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingRectTest, NativeDrawingRectTest_GetWidth004, TestSize.Level1)
{
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, 400, 800);
    float width = OH_Drawing_RectGetWidth(rect);
    EXPECT_TRUE(IsScalarAlmostEqual(width, 400)); // 400 means height
    OH_Drawing_RectDestroy(rect);
}

/*
 * @tc.name: NativeDrawingRectTest_SetAndGet005
 * @tc.desc: test for set and get of rect.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingRectTest, NativeDrawingRectTest_SetAndGet005, TestSize.Level1)
{
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(0, 0, 400, 800);
    OH_Drawing_RectSetLeft(rect, 10);
    OH_Drawing_RectSetTop(rect, 10);
    OH_Drawing_RectSetRight(rect, 300);
    OH_Drawing_RectSetBottom(rect, 400);
    float left = OH_Drawing_RectGetLeft(rect);
    float top = OH_Drawing_RectGetTop(rect);
    float right = OH_Drawing_RectGetRight(rect);
    float bottom = OH_Drawing_RectGetBottom(rect);
    EXPECT_TRUE(IsScalarAlmostEqual(left, 10)); // 10 means left
    EXPECT_TRUE(IsScalarAlmostEqual(top, 10)); // 10 means top
    EXPECT_TRUE(IsScalarAlmostEqual(right, 300)); // 300 means right
    EXPECT_TRUE(IsScalarAlmostEqual(bottom, 400)); // 400 means bottom
    OH_Drawing_RectDestroy(rect);
}

/*
 * @tc.name: NativeDrawingRectTest_Copy006
 * @tc.desc: test for Copy of rect.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingRectTest, NativeDrawingRectTest_Copy006, TestSize.Level1)
{
    OH_Drawing_Rect* rectSrc = OH_Drawing_RectCreate(0, 0, 400, 800);
    OH_Drawing_Rect* rectDst = OH_Drawing_RectCreate(11, 22, 333, 444);
    OH_Drawing_RectCopy(rectDst, rectSrc);
    float left = OH_Drawing_RectGetLeft(rectSrc);
    float top = OH_Drawing_RectGetTop(rectSrc);
    float right = OH_Drawing_RectGetRight(rectSrc);
    float bottom = OH_Drawing_RectGetBottom(rectSrc);
    EXPECT_TRUE(IsScalarAlmostEqual(left, 11)); // 11 means left
    EXPECT_TRUE(IsScalarAlmostEqual(top, 22)); // 22 means top
    EXPECT_TRUE(IsScalarAlmostEqual(right, 333)); // 333 means right
    EXPECT_TRUE(IsScalarAlmostEqual(bottom, 444)); // 444 means bottom
    OH_Drawing_RectDestroy(rectSrc);
    OH_Drawing_RectDestroy(rectDst);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS