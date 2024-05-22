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

#include "drawing_color.h"
#include "drawing_filter.h"
#include "drawing_mask_filter.h"
#include "drawing_rect.h"
#include "drawing_round_rect.h"
#include "gtest/gtest.h"

#include "utils/round_rect.h"
#include "utils/scalar.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class NativeDrawingRoundRectTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void NativeDrawingRoundRectTest::SetUpTestCase() {}
void NativeDrawingRoundRectTest::TearDownTestCase() {}
void NativeDrawingRoundRectTest::SetUp() {}
void NativeDrawingRoundRectTest::TearDown() {}

/*
 * @tc.name: NativeDrawingRectTest_CCornerPosCastToCornerPos001
 * @tc.desc: test CCornerPosCastToCornerPos
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingRoundRectTest, CCornerPosCastToCornerPos001, TestSize.Level1)
{
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(100, 100, 200, 200);
    OH_Drawing_RoundRect* roundRect = OH_Drawing_RoundRectCreate(rect, 200, 200);
    ASSERT_NE(nullptr, roundRect);

    OH_Drawing_Corner_Radii radius = { 10, 10 };

    OH_Drawing_RoundRectSetCorner(roundRect, OH_Drawing_CornerPos::CORNER_POS_TOP_LEFT, radius);

    OH_Drawing_Corner_Radii retrievedRadius;
    retrievedRadius = OH_Drawing_RoundRectGetCorner(roundRect, OH_Drawing_CornerPos::CORNER_POS_TOP_LEFT);
    EXPECT_EQ(retrievedRadius.x, radius.x);
    EXPECT_EQ(retrievedRadius.y, radius.y);

    OH_Drawing_RoundRectDestroy(roundRect);
    OH_Drawing_RectDestroy(rect);
}

/*
 * @tc.name: NativeDrawingRectTest_CCornerPosCastToCornerPos002
 * @tc.desc: test CCornerPosCastToCornerPos
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingRoundRectTest, CCornerPosCastToCornerPos002, TestSize.Level1)
{
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(100, 100, 200, 200);
    OH_Drawing_RoundRect* roundRect = OH_Drawing_RoundRectCreate(rect, 200, 200);
    ASSERT_NE(nullptr, roundRect);

    OH_Drawing_Corner_Radii radius = { 10, 10 };

    OH_Drawing_RoundRectSetCorner(roundRect, OH_Drawing_CornerPos::CORNER_POS_TOP_RIGHT, radius);

    OH_Drawing_Corner_Radii retrievedRadius;

    retrievedRadius = OH_Drawing_RoundRectGetCorner(roundRect, OH_Drawing_CornerPos::CORNER_POS_TOP_RIGHT);
    EXPECT_EQ(retrievedRadius.x, radius.x);
    EXPECT_EQ(retrievedRadius.y, radius.y);

    OH_Drawing_RoundRectDestroy(roundRect);
    OH_Drawing_RectDestroy(rect);
}

/*
 * @tc.name: NativeDrawingRectTest_CCornerPosCastToCornerPos003
 * @tc.desc: test CCornerPosCastToCornerPos
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingRoundRectTest, CCornerPosCastToCornerPos003, TestSize.Level1)
{
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(100, 100, 200, 200);
    OH_Drawing_RoundRect* roundRect = OH_Drawing_RoundRectCreate(rect, 200, 200);
    ASSERT_NE(nullptr, roundRect);

    OH_Drawing_Corner_Radii radius = { 10, 10 };

    OH_Drawing_RoundRectSetCorner(roundRect, OH_Drawing_CornerPos::CORNER_POS_BOTTOM_RIGHT, radius);

    OH_Drawing_Corner_Radii retrievedRadius;

    retrievedRadius = OH_Drawing_RoundRectGetCorner(roundRect, OH_Drawing_CornerPos::CORNER_POS_BOTTOM_RIGHT);
    EXPECT_EQ(retrievedRadius.x, radius.x);
    EXPECT_EQ(retrievedRadius.y, radius.y);

    OH_Drawing_RoundRectDestroy(roundRect);
    OH_Drawing_RectDestroy(rect);
}

/*
 * @tc.name: NativeDrawingRectTest_CCornerPosCastToCornerPos004
 * @tc.desc: test CCornerPosCastToCornerPos
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingRoundRectTest, CCornerPosCastToCornerPos004, TestSize.Level1)
{
    OH_Drawing_Rect* rect = OH_Drawing_RectCreate(100, 100, 200, 200);
    OH_Drawing_RoundRect* roundRect = OH_Drawing_RoundRectCreate(rect, 200, 200);
    ASSERT_NE(nullptr, roundRect);

    OH_Drawing_Corner_Radii radius = { 10, 10 };

    OH_Drawing_RoundRectSetCorner(roundRect, OH_Drawing_CornerPos::CORNER_POS_BOTTOM_LEFT, radius);

    OH_Drawing_Corner_Radii retrievedRadius;

    retrievedRadius = OH_Drawing_RoundRectGetCorner(roundRect, OH_Drawing_CornerPos::CORNER_POS_BOTTOM_LEFT);
    EXPECT_EQ(retrievedRadius.x, radius.x);
    EXPECT_EQ(retrievedRadius.y, radius.y);

    OH_Drawing_RoundRectDestroy(roundRect);
    OH_Drawing_RectDestroy(rect);
}

/*
 * @tc.name: NativeDrawingRectTest_OH_Drawing_RoundRectGetCorner001
 * @tc.desc: test OH_Drawing_RoundRectGetCorner
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingRoundRectTest, OH_Drawing_RoundRectGetCorner001, TestSize.Level1)
{
    OH_Drawing_RoundRect* nullRoundRect = nullptr;

    OH_Drawing_Corner_Radii radii =
        OH_Drawing_RoundRectGetCorner(nullRoundRect, OH_Drawing_CornerPos::CORNER_POS_TOP_LEFT);

    EXPECT_EQ(radii.x, 0);
    EXPECT_EQ(radii.y, 0);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS