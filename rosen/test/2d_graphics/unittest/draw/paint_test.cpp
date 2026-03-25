/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gtest/gtest.h"

#include "draw/paint.h"
#include "draw/ui_color.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace Drawing {
class PaintTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void PaintTest::SetUpTestCase() {}
void PaintTest::TearDownTestCase() {}
void PaintTest::SetUp() {}
void PaintTest::TearDown() {}

/**
 * @tc.name: SetUIColor001
 * @tc.desc: Test SetUIColor
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PaintTest, SetUIColor001, TestSize.Level1)
{
    Paint paint;
    UIColor color(0.5f, 0.6f, 0.7f, 0.8f, 2.0f);
    paint.SetUIColor(color);
    EXPECT_TRUE(paint.HasUIColor());
    EXPECT_FLOAT_EQ(paint.GetUIColor().GetRed(), 0.5f);
    EXPECT_FLOAT_EQ(paint.GetUIColor().GetGreen(), 0.6f);
    EXPECT_FLOAT_EQ(paint.GetUIColor().GetBlue(), 0.7f);
    EXPECT_FLOAT_EQ(paint.GetUIColor().GetAlpha(), 0.8f);
    EXPECT_FLOAT_EQ(paint.GetUIColor().GetHeadroom(), 2.0f);
}

/**
 * @tc.name: SetUIColorThenSetColor001
 * @tc.desc: Test SetUIColor Then SetColor
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PaintTest, SetUIColorThenSetColor001, TestSize.Level1)
{
    Paint paint;
    UIColor uiColor(0.5f, 0.6f, 0.7f, 0.8f, 2.0f);
    paint.SetUIColor(uiColor);
    EXPECT_TRUE(paint.HasUIColor());
    Color normalColor(100, 150, 200, 255);
    paint.SetColor(normalColor);
    EXPECT_FALSE(paint.HasUIColor());
}

/**
 * @tc.name: CopyConstructorWithUIColor001
 * @tc.desc: Test Copy Constructor with SetUIColor
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PaintTest, CopyConstructorWithUIColor001, TestSize.Level1)
{
    Paint paint1;
    UIColor color(0.5f, 0.6f, 0.7f, 0.8f, 2.0f);
    paint1.SetUIColor(color);
    Paint paint2(paint1);
    EXPECT_TRUE(paint2.HasUIColor());
    EXPECT_FLOAT_EQ(paint2.GetUIColor().GetRed(), 0.5f);
    EXPECT_FLOAT_EQ(paint2.GetUIColor().GetGreen(), 0.6f);

    Paint paint3;
    paint3.SetARGB(255, 100, 150, 200);
    Paint paint4(paint3);
    EXPECT_FALSE(paint4.HasUIColor());
}

/**
 * @tc.name: AssignmentOperatorWithUIColor001
 * @tc.desc: Test Assignment Operator With UIColor
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PaintTest, AssignmentOperatorWithUIColor001, TestSize.Level1)
{
    Paint paint1;
    UIColor color(0.5f, 0.6f, 0.7f, 0.8f, 2.0f);
    paint1.SetUIColor(color);
    Paint paint2;
    paint2 = paint1;
    EXPECT_TRUE(paint2.HasUIColor());
    EXPECT_FLOAT_EQ(paint2.GetUIColor().GetRed(), 0.5f);
    EXPECT_FLOAT_EQ(paint2.GetUIColor().GetGreen(), 0.6f);

    paint1.SetARGB(255, 100, 150, 200);
    Paint paint3;
    paint3 = paint1;
    EXPECT_FALSE(paint3.HasUIColor());
}

/**
 * @tc.name: AttachBrushWithUIColor001
 * @tc.desc: Test Attach Brush With UIColor
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PaintTest, AttachBrushWithUIColor001, TestSize.Level1)
{
    Paint paint;
    Brush brush;
    UIColor color(0.5f, 0.6f, 0.7f, 0.8f, 2.0f);
    brush.SetUIColor(color, nullptr);
    paint.AttachBrush(brush);
    EXPECT_TRUE(paint.HasUIColor());
    EXPECT_FLOAT_EQ(paint.GetUIColor().GetRed(), 0.5f);

    brush.SetARGB(255, 100, 150, 200);
    paint.AttachBrush(brush);
    EXPECT_FALSE(paint.HasUIColor());
}

/**
 * @tc.name: AttachPenWithUIColor001
 * @tc.desc: Test Attach Pen With UIColor
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PaintTest, AttachPenWithUIColor001, TestSize.Level1)
{
    Paint paint;
    Pen pen;
    UIColor color(0.5f, 0.6f, 0.7f, 0.8f, 2.0f);
    pen.SetUIColor(color, nullptr);
    paint.AttachPen(pen);
    EXPECT_TRUE(paint.HasUIColor());
    EXPECT_FLOAT_EQ(paint.GetUIColor().GetRed(), 0.5f);

    pen.SetARGB(255, 100, 150, 200);
    paint.AttachPen(pen);
    EXPECT_FALSE(paint.HasUIColor());
}

/**
 * @tc.name: CanCombinePaintWithUIColor001
 * @tc.desc: Test CanCombinePaint With UIColor
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PaintTest, CanCombinePaintWithUIColor001, TestSize.Level1)
{
    Paint paint1;
    Paint paint2;
    UIColor color(0.5f, 0.6f, 0.7f, 0.8f, 2.0f);
    paint1.SetUIColor(color);
    paint2.SetUIColor(color);
    EXPECT_TRUE(paint1 == paint2);
    EXPECT_TRUE(Paint::CanCombinePaint(paint1, paint2));

    UIColor color3(0.5f, 0.6f, 0.7f, 0.8f, 2.0f);
    UIColor color4(0.3f, 0.4f, 0.5f, 0.6f, 1.5f);
    paint1.SetUIColor(color3);
    paint2.SetUIColor(color4);
    EXPECT_TRUE(paint1 != paint2);
    EXPECT_FALSE(Paint::CanCombinePaint(paint1, paint2));
}

/**
 * @tc.name: Reset001
 * @tc.desc: Test Reset
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PaintTest, Reset001, TestSize.Level1)
{
    Paint paint;
    UIColor color(0.5f, 0.6f, 0.7f, 0.8f, 2.0f);
    paint.SetUIColor(color);
    EXPECT_TRUE(paint.HasUIColor());

    paint.Reset();
    EXPECT_FALSE(paint.HasUIColor());
}

/**
 * @tc.name: Dump001
 * @tc.desc: Test Dump
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(PaintTest, Dump001, TestSize.Level1)
{
    Paint paint;
    UIColor color(0.5f, 0.6f, 0.7f, 0.8f, 2.0f);
    paint.SetUIColor(color);
    std::string out;
    paint.Dump(out);
    EXPECT_FALSE(out.empty());
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
