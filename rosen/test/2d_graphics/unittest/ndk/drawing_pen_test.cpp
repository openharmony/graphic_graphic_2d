/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "c/drawing_color.h"
#include "c/drawing_pen.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class NativeDrawingPenTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void NativeDrawingPenTest::SetUpTestCase() {}
void NativeDrawingPenTest::TearDownTestCase() {}
void NativeDrawingPenTest::SetUp() {}
void NativeDrawingPenTest::TearDown() {}

/*
 * @tc.name: NativeDrawingPenTest_pen001
 * @tc.desc: test for create drawing_pen.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_pen001, TestSize.Level1)
{
    OH_Drawing_Pen* pen = OH_Drawing_PenCreate();
    EXPECT_EQ(pen == nullptr, false);
    OH_Drawing_PenDestroy(pen);
}

/*
 * @tc.name: NativeDrawingPenTest_pen002
 * @tc.desc: test for the get and set methods about AntiAlias for a pen.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_pen002, TestSize.Level1)
{
    OH_Drawing_Pen* pen1 = OH_Drawing_PenCreate();
    OH_Drawing_PenSetAntiAlias(pen1, true);
    EXPECT_EQ(OH_Drawing_PenIsAntiAlias(pen1), true);
    OH_Drawing_PenSetAntiAlias(pen1, false);
    EXPECT_EQ(OH_Drawing_PenIsAntiAlias(pen1), false);
    OH_Drawing_PenDestroy(pen1);
}

/*
 * @tc.name: NativeDrawingPenTest_pen003
 * @tc.desc: test for the get and set methods about the color for a pen.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_pen003, TestSize.Level1)
{
    OH_Drawing_Pen* pen2 = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen2, OH_Drawing_ColorSetArgb(0xFF, 0xFF, 0x00, 0x00));
    EXPECT_EQ(OH_Drawing_PenGetColor(pen2), 0xFFFF0000);
    OH_Drawing_PenDestroy(pen2);
}

/*
 * @tc.name: NativeDrawingPenTest_pen004
 * @tc.desc: test for the get and set methods about the width for a pen.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_pen004, TestSize.Level1)
{
    OH_Drawing_Pen* pen3 = OH_Drawing_PenCreate();
    OH_Drawing_PenSetWidth(pen3, 10);
    EXPECT_EQ(OH_Drawing_PenGetWidth(pen3), 10);
    OH_Drawing_PenDestroy(pen3);
}

/*
 * @tc.name: NativeDrawingPenTest_pen005
 * @tc.desc: test for the get and set methods about the miterLimit for a pen.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_pen005, TestSize.Level1)
{
    OH_Drawing_Pen* pen4 = OH_Drawing_PenCreate();
    OH_Drawing_PenSetMiterLimit(pen4, 5);
    EXPECT_EQ(OH_Drawing_PenGetMiterLimit(pen4), 5);
    OH_Drawing_PenDestroy(pen4);
}

/*
 * @tc.name: NativeDrawingPenTest_pen006
 * @tc.desc: test for the get and set methods about the line cap style for a pen.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_pen006, TestSize.Level1)
{
    OH_Drawing_Pen* pen5 = OH_Drawing_PenCreate();
    OH_Drawing_PenSetCap(pen5, OH_Drawing_PenLineCapStyle::LINE_SQUARE_CAP);
    EXPECT_EQ(OH_Drawing_PenGetCap(pen5), OH_Drawing_PenLineCapStyle::LINE_SQUARE_CAP);
    OH_Drawing_PenSetCap(pen5, OH_Drawing_PenLineCapStyle::LINE_FLAT_CAP);
    EXPECT_EQ(OH_Drawing_PenGetCap(pen5), OH_Drawing_PenLineCapStyle::LINE_FLAT_CAP);
    OH_Drawing_PenSetCap(pen5, OH_Drawing_PenLineCapStyle::LINE_ROUND_CAP);
    EXPECT_EQ(OH_Drawing_PenGetCap(pen5), OH_Drawing_PenLineCapStyle::LINE_ROUND_CAP);
    OH_Drawing_PenDestroy(pen5);
}

/*
 * @tc.name: NativeDrawingPenTest_pen007
 * @tc.desc: test for the get and set methods about the line join style for a pen.
 * @tc.type: FUNC
 * @tc.require: AR000GTO5R
 */
HWTEST_F(NativeDrawingPenTest, NativeDrawingPenTest_pen007, TestSize.Level1)
{
    OH_Drawing_Pen* pen6 = OH_Drawing_PenCreate();
    OH_Drawing_PenSetJoin(pen6, OH_Drawing_PenLineJoinStyle::LINE_ROUND_JOIN);
    EXPECT_EQ(OH_Drawing_PenGetJoin(pen6), OH_Drawing_PenLineJoinStyle::LINE_ROUND_JOIN);
    OH_Drawing_PenSetJoin(pen6, OH_Drawing_PenLineJoinStyle::LINE_MITER_JOIN);
    EXPECT_EQ(OH_Drawing_PenGetJoin(pen6), OH_Drawing_PenLineJoinStyle::LINE_MITER_JOIN);
    OH_Drawing_PenSetJoin(pen6, OH_Drawing_PenLineJoinStyle::LINE_BEVEL_JOIN);
    EXPECT_EQ(OH_Drawing_PenGetJoin(pen6), OH_Drawing_PenLineJoinStyle::LINE_BEVEL_JOIN);
    OH_Drawing_PenDestroy(pen6);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS