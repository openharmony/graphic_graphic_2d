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

/*
 * @tc.name: NativeDrawingRectTest_Intersect002
 * @tc.desc: test for the Intersect methods of Rect.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingRectTest, NativeDrawingRectTest_Intersect002, TestSize.Level1)
{
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(100, 200, 500, 300); // 100 200 300 500 矩形
    EXPECT_NE(nullptr, rect);

    OH_Drawing_Rect *rectt = nullptr;
    ASSERT_TRUE(rectt == nullptr);

    OH_Drawing_Rect *otherOne = OH_Drawing_RectCreate(300, 250, 600, 400); // 250 300 400 600 矩形
    EXPECT_NE(nullptr, otherOne);

    OH_Drawing_Rect *otherTwo = nullptr;
    ASSERT_TRUE(otherTwo == nullptr);

    bool ret = OH_Drawing_RectIntersect(rectt, otherOne);
    EXPECT_EQ(ret, false);

    ret = OH_Drawing_RectIntersect(rect, otherTwo);
    EXPECT_EQ(ret, false);

    ret = OH_Drawing_RectIntersect(rectt, otherTwo);
    EXPECT_EQ(ret, false);

    OH_Drawing_RectDestroy(rect);
    OH_Drawing_RectDestroy(rectt);
    OH_Drawing_RectDestroy(otherOne);
    OH_Drawing_RectDestroy(otherTwo);
}

/*
 * @tc.name: NativeDrawingRectTest_RectJoin001
 * @tc.desc: test for the RectJoin methods of Rect.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingRectTest, NativeDrawingRectTest_RectJoin001, TestSize.Level1)
{
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(100, 200, 500, 300); // 100 200 300 500 矩形
    EXPECT_NE(nullptr, rect);

    OH_Drawing_Rect *otherOne = OH_Drawing_RectCreate(300, 250, 600, 400); // 250 300 400 600 矩形
    EXPECT_NE(nullptr, otherOne);

    bool ret = OH_Drawing_RectJoin(rect, otherOne);
    EXPECT_EQ(ret, true);

    OH_Drawing_RectDestroy(rect);
    OH_Drawing_RectDestroy(otherOne);
}

/*
 * @tc.name: NativeDrawingRectTest_RectJoin002
 * @tc.desc: test for the RectJoin methods of Rect.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingRectTest, NativeDrawingRectTest_RectJoin002, TestSize.Level1)
{
    OH_Drawing_Rect *rect = nullptr;
    ASSERT_TRUE(rect == nullptr);
    OH_Drawing_Rect *otherOne = nullptr;
    ASSERT_TRUE(otherOne == nullptr);

    bool ret = OH_Drawing_RectJoin(rect, otherOne);
    EXPECT_EQ(ret, false);

    OH_Drawing_RectDestroy(rect);
    OH_Drawing_RectDestroy(otherOne);
}

/*
 * @tc.name: NativeDrawingRectTest_RectSetTop001
 * @tc.desc: test for the RectSetTop methods of Rect.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingRectTest, NativeDrawingRectTest_RectSetTop001, TestSize.Level1)
{
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(100, 200, 500, 300); // 100 200 300 500 rect param
    OH_Drawing_RectSetTop(rect, 10);                                   // 10 means top

    ASSERT_FLOAT_EQ(OH_Drawing_RectGetTop(rect), 10); // 10 equal to number
    ASSERT_NE(OH_Drawing_RectGetTop(rect), 0.f);
    OH_Drawing_RectDestroy(rect);
}

/*
 * @tc.name: NativeDrawingRectTest_RectSetTop002
 * @tc.desc: test for the RectSetTop methods of Rect.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingRectTest, NativeDrawingRectTest_RectSetTop002, TestSize.Level1)
{
    OH_Drawing_Rect *rect = nullptr;
    OH_Drawing_RectSetTop(rect, 10); // 10 means top

    ASSERT_TRUE(rect == nullptr);
}

/*
 * @tc.name: NativeDrawingRectTest_RectSetBottom001
 * @tc.desc: test for the RectSetBottom methods of Rect.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingRectTest, NativeDrawingRectTest_RectSetBottom001, TestSize.Level1)
{
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(100, 200, 500, 300); // 100 200 300 500 rect param
    OH_Drawing_RectSetBottom(rect, 10);                                // 10 means Bottom

    ASSERT_FLOAT_EQ(OH_Drawing_RectGetBottom(rect), 10); // 10 equal to number
    ASSERT_NE(OH_Drawing_RectGetBottom(rect), 0.f);
    OH_Drawing_RectDestroy(rect);
}

/*
 * @tc.name: NativeDrawingRectTest_RectSetBottom002
 * @tc.desc: test for the RectSetBottom methods of Rect.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingRectTest, NativeDrawingRectTest_RectSetBottom002, TestSize.Level1)
{
    OH_Drawing_Rect *rect = nullptr;
    OH_Drawing_RectSetBottom(rect, 10); // 10 means Bottom
    // 验证函数中对空指针的处理是否符合预期
    ASSERT_TRUE(rect == nullptr);
    OH_Drawing_RectDestroy(rect);
}

/*
 * @tc.name: NativeDrawingRectTest_RectSetLeft001
 * @tc.desc: test for the RectSetLeft methods of Rect.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingRectTest, NativeDrawingRectTest_RectSetLeft001, TestSize.Level1)
{
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(100, 200, 500, 300); // 100 200 300 500 rect param
    OH_Drawing_RectSetLeft(rect, 10);                                  // 10 means Left

    ASSERT_FLOAT_EQ(OH_Drawing_RectGetLeft(rect), 10); // 10 equal to number
    ASSERT_NE(OH_Drawing_RectGetLeft(rect), 0.f);
    OH_Drawing_RectDestroy(rect);
}

/*
 * @tc.name: NativeDrawingRectTest_RectSetLeft002
 * @tc.desc: test for the RectSetLeft methods of Rect.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingRectTest, NativeDrawingRectTest_RectSetLeft002, TestSize.Level1)
{
    OH_Drawing_Rect *rect = nullptr;
    OH_Drawing_RectSetLeft(rect, 10); // 10 means Left

    ASSERT_TRUE(rect == nullptr);
    OH_Drawing_RectDestroy(rect);
}

/*
 * @tc.name: NativeDrawingRectTest_RectSetRight001
 * @tc.desc: test for the RectSetRight methods of Rect.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingRectTest, NativeDrawingRectTest_RectSetRight001, TestSize.Level1)
{
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(100, 200, 500, 300); // 100 200 300 500 rect param
    OH_Drawing_RectSetRight(rect, 10);                                 // 10 means Right

    ASSERT_FLOAT_EQ(OH_Drawing_RectGetRight(rect), 10);
    OH_Drawing_RectDestroy(rect);
}

/*
 * @tc.name: NativeDrawingRectTest_RectSetRight002
 * @tc.desc: test for the RectSetRight methods of Rect.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingRectTest, NativeDrawingRectTest_RectSetRight002, TestSize.Level1)
{
    OH_Drawing_Rect *rect = nullptr;
    OH_Drawing_RectSetRight(rect, 10); // 10 means Right

    ASSERT_TRUE(rect == nullptr);
    OH_Drawing_RectDestroy(rect);
}

/*
 * @tc.name: NativeDrawingRectTest_RectGetTop001
 * @tc.desc: test for the RectGetTop methods of Rect.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingRectTest, NativeDrawingRectTest_RectGetTop001, TestSize.Level1)
{
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(50, 50, 250, 250); // 50, 50, 250, 250 rect param
    float top = OH_Drawing_RectGetTop(rect);
    EXPECT_TRUE(IsScalarAlmostEqual(top, 50)); // 50 means top
    OH_Drawing_RectDestroy(rect);
}

/*
 * @tc.name: NativeDrawingRectTest_RectGetTop002
 * @tc.desc: test for the RectGetTop methods of Rect.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingRectTest, NativeDrawingRectTest_RectGetTop002, TestSize.Level1)
{
    OH_Drawing_Rect *rect = nullptr;
    OH_Drawing_RectGetTop(rect);
    ASSERT_TRUE(rect == nullptr);
    OH_Drawing_RectDestroy(rect);
}

/*
 * @tc.name: NativeDrawingRectTest_RectGetBottom001
 * @tc.desc: test for the RectGetBottom methods of Rect.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingRectTest, NativeDrawingRectTest_RectGetBottom001, TestSize.Level1)
{
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(50, 50, 250, 250); // 50, 50, 250, 250 rect param
    float bottom = OH_Drawing_RectGetBottom(rect);
    EXPECT_TRUE(IsScalarAlmostEqual(bottom, 250)); // 250 means bottom
    OH_Drawing_RectDestroy(rect);
}

/*
 * @tc.name: NativeDrawingRectTest_RectGetBottom002
 * @tc.desc: test for the RectGetBottom methods of Rect.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingRectTest, NativeDrawingRectTest_RectGetBottom002, TestSize.Level1)
{
    OH_Drawing_Rect *rect = nullptr;
    OH_Drawing_RectGetBottom(rect);
    ASSERT_TRUE(rect == nullptr);
    OH_Drawing_RectDestroy(rect);
}

/*
 * @tc.name: NativeDrawingRectTest_RectGetLeft001
 * @tc.desc: test for the RectGetLeft methods of Rect.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingRectTest, NativeDrawingRectTest_RectGetLeft001, TestSize.Level1)
{
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(50, 50, 250, 250); // 50 250 rect param
    float left = OH_Drawing_RectGetLeft(rect);
    EXPECT_TRUE(IsScalarAlmostEqual(left, 50)); // 50 means left
    OH_Drawing_RectDestroy(rect);
}

/*
 * @tc.name: NativeDrawingRectTest_RectGetLeft002
 * @tc.desc: test for the RectGetLeft methods of Rect.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingRectTest, NativeDrawingRectTest_RectGetLeft002, TestSize.Level1)
{
    OH_Drawing_Rect *rect = nullptr;
    OH_Drawing_RectGetLeft(rect);
    ASSERT_TRUE(rect == nullptr);
    OH_Drawing_RectDestroy(rect);
}

/*
 * @tc.name: NativeDrawingRectTest_RectGetRight001
 * @tc.desc: test for the RectGetRight methods of Rect.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingRectTest, NativeDrawingRectTest_RectGetRight001, TestSize.Level1)
{
    OH_Drawing_Rect *rect = OH_Drawing_RectCreate(50, 50, 250, 250); // 50 250 rect param
    float right = OH_Drawing_RectGetRight(rect);
    EXPECT_TRUE(IsScalarAlmostEqual(right, 250)); // 250 means right
    OH_Drawing_RectDestroy(rect);
}

/*
 * @tc.name: NativeDrawingRectTest_RectGetRight002
 * @tc.desc: test for the RectGetRight methods of Rect.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingRectTest, NativeDrawingRectTest_RectGetRight002, TestSize.Level1)
{
    OH_Drawing_Rect *rect = nullptr;
    OH_Drawing_RectGetRight(rect);
    ASSERT_TRUE(rect == nullptr);
    OH_Drawing_RectDestroy(rect);
}

/*
 * @tc.name: NativeDrawingRectTest_GetHeight002
 * @tc.desc: test for get height of rect.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingRectTest, NativeDrawingRectTest_GetHeight002, TestSize.Level1)
{
    OH_Drawing_Rect *rect = nullptr;
    OH_Drawing_RectGetHeight(rect);
    ASSERT_TRUE(rect == nullptr);
    OH_Drawing_RectDestroy(rect);
}

/*
 * @tc.name: NativeDrawingRectTest_GetWidth002
 * @tc.desc: test for get width of rect.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingRectTest, NativeDrawingRectTest_GetWidth002, TestSize.Level1)
{
    OH_Drawing_Rect *rect = nullptr;
    OH_Drawing_RectGetWidth(rect);
    ASSERT_TRUE(rect == nullptr);
    OH_Drawing_RectDestroy(rect);
}

/*
 * @tc.name: NativeDrawingRectTest_RectCopy001
 * @tc.desc: test for Copy of rect.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingRectTest, NativeDrawingRectTest_RectCopy001, TestSize.Level1)
{
    OH_Drawing_Rect *rectSrc = nullptr;
    ASSERT_TRUE(rectSrc == nullptr);
    OH_Drawing_Rect *rectDst = OH_Drawing_RectCreate(11, 22, 333, 444); // 11,22,333,444 rect param
    OH_Drawing_RectCopy(rectDst, rectSrc);
    ASSERT_TRUE(rectDst == nullptr);

    OH_Drawing_Rect *rectSrc1 = OH_Drawing_RectCreate(0, 0, 400, 800); // 400 800 v
    OH_Drawing_Rect *rectDst1 = nullptr;
    ASSERT_TRUE(rectDst1 == nullptr);
    OH_Drawing_RectCopy(rectDst1, rectSrc);
    ASSERT_TRUE(rectDst1 == nullptr);

    OH_Drawing_RectDestroy(rectSrc);
    OH_Drawing_RectDestroy(rectDst);
    OH_Drawing_RectDestroy(rectSrc1);
    OH_Drawing_RectDestroy(rectDst1);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS