/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <cstddef>
#include "core/SkPaint.h"
#include "gtest/gtest.h"
#define private public
#include "skia_adapter/skia_paint.h"
#undef private
#include "draw/brush.h"
#include "draw/color.h"
#include "draw/paint.h"
#include "effect/color_space.h"
#include "effect/filter.h"
#include "effect/mask_filter.h"
#include "effect/path_effect.h"
#include "effect/shader_effect.h"

using namespace testing;
using namespace testing::ext;

#define TDD_PAINT_RADIUS 10

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaPaintTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void SkiaPaintTest::SetUpTestCase() {}
void SkiaPaintTest::TearDownTestCase() {}
void SkiaPaintTest::SetUp() {}
void SkiaPaintTest::TearDown() {}

/**
 * @tc.name: ApplyPaint001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaPaintTest, ApplyPaint001, TestSize.Level1)
{
    Paint paint;
    SkiaPaint skiaPaint;
    skiaPaint.ApplyPaint(paint);
    EXPECT_TRUE(skiaPaint.paintInUse_ == 0);
}

/**
 * @tc.name: ApplyPaint002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaPaintTest, ApplyPaint002, TestSize.Level1)
{
    Paint paint;
    paint.SetStyle(Paint::PaintStyle::PAINT_FILL);
    SkiaPaint skiaPaint;
    skiaPaint.ApplyPaint(paint);
    EXPECT_TRUE(skiaPaint.paintInUse_ != 0);
}

/**
 * @tc.name: AsBlendMode001
 * @tc.desc: Test AsBlendMode
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaPaintTest, AsBlendMode001, TestSize.Level1)
{
    Brush brush;
    SkiaPaint skiaPaint;
    skiaPaint.AsBlendMode(brush);
}

/**
 * @tc.name: BrushToSkPaint001
 * @tc.desc: Test BrushToSkPaint
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaPaintTest, BrushToSkPaint001, TestSize.Level1)
{
    Brush brush;
    brush.SetAntiAlias(true);
    SkPaint skPaint;
    SkiaPaint::BrushToSkPaint(brush, skPaint);
    EXPECT_TRUE(skPaint.isAntiAlias());
}

/**
 * @tc.name: BrushToSkPaint002
 * @tc.desc: Test BrushToSkPaint
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaPaintTest, BrushToSkPaint002, TestSize.Level1)
{
    Brush brush;
    brush.SetAntiAlias(true);
    Color4f color{0, 0, 0, 1};
    auto space = std::make_shared<ColorSpace>();
    brush.SetColor(color, space);
    brush.SetAlpha(100);
    brush.SetShaderEffect(ShaderEffect::CreateColorShader(0xFF000000));
    auto blender = std::make_shared<Blender>();
    brush.SetBlender(blender);
    SkPaint skPaint;
    SkiaPaint::BrushToSkPaint(brush, skPaint);
    EXPECT_TRUE(skPaint.isAntiAlias());
}

/**
 * @tc.name: PenToSkPaint001
 * @tc.desc: Test PenToSkPaint
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaPaintTest, PenToSkPaint001, TestSize.Level1)
{
    Pen pen;
    pen.SetAntiAlias(true);
    SkPaint skPaint;
    SkiaPaint::PenToSkPaint(pen, skPaint);
    EXPECT_TRUE(skPaint.isAntiAlias());
}

/**
 * @tc.name: PenToSkPaint002
 * @tc.desc: Test PenToSkPaint
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaPaintTest, PenToSkPaint002, TestSize.Level1)
{
    Pen pen;
    pen.SetAntiAlias(true);
    pen.SetCapStyle(Pen::CapStyle::SQUARE_CAP);
    pen.SetJoinStyle(Pen::JoinStyle::ROUND_JOIN);
    SkPaint skPaint;
    SkiaPaint::PenToSkPaint(pen, skPaint);
    EXPECT_TRUE(skPaint.isAntiAlias());
}

/**
 * @tc.name: PenToSkPaint003
 * @tc.desc: Test PenToSkPaint
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaPaintTest, PenToSkPaint003, TestSize.Level1)
{
    Pen pen;
    pen.SetAntiAlias(true);
    Color4f color{0, 0, 0, 1};
    auto space = std::make_shared<ColorSpace>();
    pen.SetColor(color, space);
    pen.SetBlendMode(BlendMode::CLEAR);
    pen.SetCapStyle(Pen::CapStyle::ROUND_CAP);
    pen.SetJoinStyle(Pen::JoinStyle::BEVEL_JOIN);
    pen.SetShaderEffect(ShaderEffect::CreateColorShader(0xFF000000));
    pen.SetPathEffect(PathEffect::CreateCornerPathEffect(TDD_PAINT_RADIUS));
    SkPaint skPaint;
    SkiaPaint::PenToSkPaint(pen, skPaint);
    EXPECT_TRUE(skPaint.isAntiAlias());
}

/**
 * @tc.name: PaintToSkPaint001
 * @tc.desc: Test PaintToSkPaint
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaPaintTest, PaintToSkPaint001, TestSize.Level1)
{
    Paint paint;
    paint.SetAntiAlias(true);
    auto space = std::make_shared<ColorSpace>();
    Color4f color{0, 0, 0, 1};
    paint.SetColor(color, space);
    paint.SetBlendMode(BlendMode::CLEAR);
    SkPaint skPaint;
    SkiaPaint::PaintToSkPaint(paint, skPaint);
    EXPECT_TRUE(skPaint.isAntiAlias());
}

/**
 * @tc.name: GetSortedPaints001
 * @tc.desc: Test GetSortedPaints
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaPaintTest, GetSortedPaints001, TestSize.Level1)
{
    Paint paint;
    paint.SetStyle(Paint::PaintStyle::PAINT_FILL);
    SkiaPaint skiaPaint;
    skiaPaint.ApplyPaint(paint);
    skiaPaint.GetSortedPaints();
    EXPECT_TRUE(skiaPaint.paintInUse_ == 0);
}

/**
 * @tc.name: ApplyStrokeParam001
 * @tc.desc: Test ApplyStrokeParam
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaPaintTest, ApplyStrokeParam001, TestSize.Level1)
{
    Paint paint;
    paint.SetStyle(Paint::PaintStyle::PAINT_FILL);
    paint.SetCapStyle(Pen::CapStyle::ROUND_CAP);
    paint.SetJoinStyle(Pen::JoinStyle::BEVEL_JOIN);
    paint.SetPathEffect(PathEffect::CreateCornerPathEffect(TDD_PAINT_RADIUS));
    SkiaPaint skiaPaint;
    SkPaint skPaint;
    skiaPaint.ApplyStrokeParam(paint, skPaint);
    Paint paint2;
    paint2.SetStyle(Paint::PaintStyle::PAINT_FILL);
    paint2.SetCapStyle(Pen::CapStyle::SQUARE_CAP);
    paint2.SetJoinStyle(Pen::JoinStyle::ROUND_JOIN);
    paint2.SetPathEffect(PathEffect::CreateCornerPathEffect(TDD_PAINT_RADIUS));
    skiaPaint.ApplyStrokeParam(paint2, skPaint);
}

/**
 * @tc.name: ComputeFastBounds001
 * @tc.desc: Test ComputeFastBounds
 * @tc.type: FUNC
 * @tc.require: I8VQSW
 */
HWTEST_F(SkiaPaintTest, ComputeFastBounds001, TestSize.Level1)
{
    Brush brush;
    Rect rect;
    SkiaPaint skiaPaint;
    skiaPaint.ComputeFastBounds(brush, rect, nullptr);
    Rect storage;
    skiaPaint.ComputeFastBounds(brush, rect, &storage);
    EXPECT_TRUE(skiaPaint.paintInUse_ == 0);
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS