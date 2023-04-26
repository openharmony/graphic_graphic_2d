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
#include "draw/pen.h"
#include "effect/color_space.h"
#include "effect/filter.h"
#include "effect/mask_filter.h"
#include "effect/path_effect.h"
#include "effect/shader_effect.h"

using namespace testing;
using namespace testing::ext;

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
 * @tc.name: BrushToSkPaint001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaPaintTest, BrushToSkPaint001, TestSize.Level1)
{
    Brush brush;
    SkPaint skPaint;
    SkiaPaint skiaPaint;
    skiaPaint.BrushToSkPaint(brush, skPaint);
    EXPECT_TRUE(skiaPaint.stroke_ != nullptr);
}

/**
 * @tc.name: BrushToSkPaint002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaPaintTest, BrushToSkPaint002, TestSize.Level1)
{
    Brush brush;
    SkPaint skPaint;
    SkiaPaint skiaPaint;
    Color4f color4f;
    std::shared_ptr<ColorSpace> colorSpace = ColorSpace::CreateSRGB();
    brush.SetColor(color4f, colorSpace);
    skiaPaint.BrushToSkPaint(brush, skPaint);
    EXPECT_TRUE(skiaPaint.stroke_ != nullptr);
}

/**
 * @tc.name: BrushToSkPaint003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaPaintTest, BrushToSkPaint003, TestSize.Level1)
{
    Brush brush;
    SkPaint skPaint;
    SkiaPaint skiaPaint;
    std::shared_ptr<ShaderEffect> shaderEffect = ShaderEffect::CreateColorShader(20);
    brush.SetShaderEffect(shaderEffect);
    skiaPaint.BrushToSkPaint(brush, skPaint);
    EXPECT_TRUE(skiaPaint.stroke_ != nullptr);
}

/**
 * @tc.name: PenToSkPaint001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaPaintTest, PenToSkPaint001, TestSize.Level1)
{
    Pen pen;
    SkPaint paint;
    SkiaPaint skiaPaint;
    skiaPaint.PenToSkPaint(pen, paint);
    EXPECT_TRUE(skiaPaint.stroke_ != nullptr);
}

/**
 * @tc.name: PenToSkPaint002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaPaintTest, PenToSkPaint002, TestSize.Level1)
{
    Pen pen;
    SkPaint paint;
    SkiaPaint skiaPaint;
    Color4f color4f;
    std::shared_ptr<ColorSpace> colorSpace = ColorSpace::CreateSRGB();
    pen.SetColor(color4f, colorSpace);
    skiaPaint.PenToSkPaint(pen, paint);
    EXPECT_TRUE(skiaPaint.stroke_ != nullptr);
}

/**
 * @tc.name: PenToSkPaint003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaPaintTest, PenToSkPaint003, TestSize.Level1)
{
    Pen pen;
    SkPaint paint;
    SkiaPaint skiaPaint;
    pen.SetCapStyle(Pen::CapStyle::FLAT_CAP);
    skiaPaint.PenToSkPaint(pen, paint);
    pen.SetCapStyle(Pen::CapStyle::SQUARE_CAP);
    skiaPaint.PenToSkPaint(pen, paint);
    pen.SetCapStyle(Pen::CapStyle::ROUND_CAP);
    skiaPaint.PenToSkPaint(pen, paint);
    EXPECT_TRUE(skiaPaint.stroke_ != nullptr);
}

/**
 * @tc.name: PenToSkPaint004
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaPaintTest, PenToSkPaint004, TestSize.Level1)
{
    Pen pen;
    SkPaint paint;
    SkiaPaint skiaPaint;
    pen.SetJoinStyle(Pen::JoinStyle::BEVEL_JOIN);
    skiaPaint.PenToSkPaint(pen, paint);
    pen.SetJoinStyle(Pen::JoinStyle::MITER_JOIN);
    skiaPaint.PenToSkPaint(pen, paint);
    pen.SetJoinStyle(Pen::JoinStyle::ROUND_JOIN);
    skiaPaint.PenToSkPaint(pen, paint);
    EXPECT_TRUE(skiaPaint.stroke_ != nullptr);
}

/**
 * @tc.name: PenToSkPaint005
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaPaintTest, PenToSkPaint005, TestSize.Level1)
{
    Pen pen;
    SkPaint paint;
    SkiaPaint skiaPaint;
    skiaPaint.PenToSkPaint(pen, paint);
    EXPECT_TRUE(skiaPaint.stroke_ != nullptr);
}

/**
 * @tc.name: PenToSkPaint006
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaPaintTest, PenToSkPaint006, TestSize.Level1)
{
    Pen pen;
    SkPaint paint;
    SkiaPaint skiaPaint;
    std::shared_ptr<ShaderEffect> shaderEffect = ShaderEffect::CreateColorShader(25);
    pen.SetShaderEffect(shaderEffect);
    skiaPaint.PenToSkPaint(pen, paint);
    EXPECT_TRUE(skiaPaint.stroke_ != nullptr);
}

/**
 * @tc.name: PenToSkPaint007
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaPaintTest, PenToSkPaint007, TestSize.Level1)
{
    Pen pen;
    SkPaint paint;
    SkiaPaint skiaPaint;
    std::shared_ptr<PathEffect> pathEffect = PathEffect::CreateCornerPathEffect(25.0f);
    pen.SetPathEffect(pathEffect);
    skiaPaint.PenToSkPaint(pen, paint);
    EXPECT_TRUE(skiaPaint.stroke_ != nullptr);
}

/**
 * @tc.name: GetSortedPaints001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaPaintTest, GetSortedPaints001, TestSize.Level1)
{
    SkiaPaint skiaPaint;
    skiaPaint.GetSortedPaints();
    EXPECT_TRUE(skiaPaint.stroke_ != nullptr);
}

/**
 * @tc.name: GetSortedPaints002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaPaintTest, GetSortedPaints002, TestSize.Level1)
{
    Pen pen;
    Brush brush;
    SkiaPaint skiaPaint;
    skiaPaint.SetStrokeFirst(true);
    skiaPaint.ApplyPenToStroke(pen);
    skiaPaint.ApplyBrushToFill(brush);
    skiaPaint.GetSortedPaints();
    EXPECT_TRUE(skiaPaint.stroke_ != nullptr);
}

/**
 * @tc.name: GetSortedPaints003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaPaintTest, GetSortedPaints003, TestSize.Level1)
{
    Pen pen;
    SkiaPaint skiaPaint;
    skiaPaint.SetStrokeFirst(true);
    skiaPaint.ApplyPenToStroke(pen);
    skiaPaint.GetSortedPaints();
    EXPECT_TRUE(skiaPaint.stroke_ != nullptr);
}

/**
 * @tc.name: GetSortedPaints004
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaPaintTest, GetSortedPaints004, TestSize.Level1)
{
    Brush brush;
    SkiaPaint skiaPaint;
    skiaPaint.SetStrokeFirst(true);
    skiaPaint.ApplyBrushToFill(brush);
    skiaPaint.GetSortedPaints();
    EXPECT_TRUE(skiaPaint.stroke_ != nullptr);
}

/**
 * @tc.name: ApplyFilter001
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaPaintTest, ApplyFilter001, TestSize.Level1)
{
    Brush brush;
    SkPaint paint;
    SkiaPaint skiaPaint;
    skiaPaint.BrushToSkPaint(brush, paint);
    EXPECT_TRUE(skiaPaint.stroke_ != nullptr);
}

/**
 * @tc.name: ApplyFilter002
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaPaintTest, ApplyFilter002, TestSize.Level1)
{
    Brush brush;
    SkPaint paint;
    Filter filter;
    SkiaPaint skiaPaint;
    filter.SetFilterQuality(Filter::FilterQuality::LOW);
    brush.SetFilter(filter);
    skiaPaint.BrushToSkPaint(brush, paint);
    filter.SetFilterQuality(Filter::FilterQuality::MEDIUM);
    brush.SetFilter(filter);
    skiaPaint.BrushToSkPaint(brush, paint);
    filter.SetFilterQuality(Filter::FilterQuality::HIGH);
    brush.SetFilter(filter);
    skiaPaint.BrushToSkPaint(brush, paint);
    filter.SetFilterQuality(Filter::FilterQuality::NONE);
    brush.SetFilter(filter);
    skiaPaint.BrushToSkPaint(brush, paint);
    EXPECT_TRUE(skiaPaint.stroke_ != nullptr);
}

/**
 * @tc.name: ApplyFilter003
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.author:
 */
HWTEST_F(SkiaPaintTest, ApplyFilter003, TestSize.Level1)
{
    Brush brush;
    SkPaint paint;
    Filter filter;
    SkiaPaint skiaPaint;
    std::shared_ptr<MaskFilter> maskFilter = MaskFilter::CreateBlurMaskFilter(BlurType::INNER, 20.0f);
    filter.SetMaskFilter(maskFilter);
    skiaPaint.BrushToSkPaint(brush, paint);
    EXPECT_TRUE(skiaPaint.stroke_ != nullptr);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS