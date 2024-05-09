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

#include "gtest/gtest.h"

#include "pipeline/rs_paint_filter_canvas.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
const float_t SET_ALPHA = 0.1f;
const float_t SET_XORY1 = 0.1f;
const float_t SET_XORY2 = 0.2f;
const float_t ANGLE_SWEEP_START = 1.0f;
const float_t SET_RADIUS = 1.0f;
const float_t SET_SCALAR = 1.0f;
const uint32_t NUMBER = 2;
const uint32_t SET_POINT_SIZE1 = 12;
const uint32_t SET_POINT_SIZE2 = 4;
const uint32_t SET_COLOR_QUAD_SIZE = 4;
const uint32_t SET_RIGHT = 10;
const uint32_t SET_TOP_BOTTOM = 80;
const uint32_t SCALAR_XORY = 255;
const uint32_t SET_COLOR = 0xFF000000;
class RSPaintFilterCanvasTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static inline RSPaintFilterCanvas* paintFilterCanvas_;
    static inline Drawing::Canvas drawingCanvas_;

    inline bool AlphaStackClear()
    {
        while (!paintFilterCanvas_->alphaStack_.empty()) {
            paintFilterCanvas_->alphaStack_.pop();
        }
        return true;
    }
    inline bool EnvStackClear()
    {
        while (!paintFilterCanvas_->envStack_.empty()) {
            paintFilterCanvas_->envStack_.pop();
        }
        return true;
    }
};

void RSPaintFilterCanvasTest::SetUpTestCase()
{
    paintFilterCanvas_ = new RSPaintFilterCanvas(&drawingCanvas_);
}
void RSPaintFilterCanvasTest::TearDownTestCase()
{
    delete paintFilterCanvas_;
    paintFilterCanvas_ = nullptr;
}
void RSPaintFilterCanvasTest::SetUp() {}
void RSPaintFilterCanvasTest::TearDown() {}

/**
 * @tc.name: SetHighContrastTest001
 * @tc.desc: SetHighContrast Test
 * @tc.type: FUNC
 * @tc.require: issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, SetHighContrastTest001, TestSize.Level1)
{
    bool mode = true;
    paintFilterCanvas_->SetHighContrast(mode);
    ASSERT_EQ(paintFilterCanvas_->isHighContrastEnabled(), mode);

    mode = false;
    paintFilterCanvas_->SetHighContrast(mode);
    ASSERT_EQ(paintFilterCanvas_->isHighContrastEnabled(), mode);
}

/**
 * @tc.name: RestoreAlphaTest001
 * @tc.desc: RestoreAlpha Test
 * @tc.type: FUNC
 * @tc.require: issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, RestoreAlphaTest001, TestSize.Level1)
{
    paintFilterCanvas_->RestoreAlpha();
}

/**
 * @tc.name: RestoreAlphaToCountTest
 * @tc.desc: RestoreAlphaToCount Test
 * @tc.type: FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, RestoreAlphaToCountTest, TestSize.Level1)
{
    int count = 0;
    paintFilterCanvas_->RestoreAlphaToCount(count);
    ASSERT_EQ(paintFilterCanvas_->GetAlphaSaveCount(), 1);
}

/**
 * @tc.name: RestoreEnvTest
 * @tc.desc: RestoreEnv Test
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, RestoreEnvTest, TestSize.Level1)
{
    paintFilterCanvas_->RestoreEnv();
}

/**
 * @tc.name: RestoreEnvToCountTest
 * @tc.desc: RestoreEnvToCount Test
 * @tc.type: FUNC
 * @tc.require: issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, RestoreEnvToCountTest, TestSize.Level1)
{
    int count = 0;
    paintFilterCanvas_->RestoreEnvToCount(count);
    ASSERT_EQ(paintFilterCanvas_->GetAlphaSaveCount(), 1);
}

/**
 * @tc.name: SetEnvForegroundColorTest
 * @tc.desc: SetEnvForegroundColor Test
 * @tc.type: FUNC
 * @tc.require: issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, SetEnvForegroundColorTest, TestSize.Level1)
{
    Color color;
    paintFilterCanvas_->SetEnvForegroundColor(color);
}

/**
 * @tc.name: GetEnvForegroundColorTest
 * @tc.desc: GetEnvForegroundColor Test
 * @tc.type: FUNC
 * @tc.require: issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, GetEnvForegroundColorTest, TestSize.Level1)
{
    Color color { SET_COLOR };
    Color setColor {};
    paintFilterCanvas_->SetEnvForegroundColor(setColor);
}

/**
 * @tc.name: onFilterTest
 * @tc.desc: onFilter Test
 * @tc.type: FUNC
 * @tc.require: issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, onFilterTest, TestSize.Level1)
{
    Drawing::Color color { 1 };
    Drawing::Brush brush;
    brush.SetColor(color);
    paintFilterCanvas_->OnFilterWithBrush(brush);
}

/**
 * @tc.name: DrawPointTest
 * @tc.desc: DrawPoint Test
 * @tc.type: FUNC
 * @tc.require: issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, DrawPointTest, TestSize.Level1)
{
    Drawing::Point point;
    paintFilterCanvas_->SetAlpha(SET_ALPHA);
    paintFilterCanvas_->DrawPoint(point);
    EXPECT_EQ(paintFilterCanvas_->GetAlpha(), SET_ALPHA);
}

/**
 * @tc.name: DrawPointsTest
 * @tc.desc: DrawPoints Test
 * @tc.type: FUNC
 * @tc.require: issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, DrawPointsTest, TestSize.Level1)
{
    // fot test
    Drawing::Point pts[] = { Drawing::Point(SET_XORY1, SET_XORY1), Drawing::Point(SET_XORY2, SET_XORY2) };
    paintFilterCanvas_->DrawPoints(Drawing::PointMode::POINTS_POINTMODE, NUMBER, pts);
    EXPECT_TRUE(paintFilterCanvas_);
}

/**
 * @tc.name: DrawLineTest
 * @tc.desc: DrawLine Test
 * @tc.type: FUNC
 * @tc.require: issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, DrawLineTest, TestSize.Level1)
{
    // fot test
    Drawing::Point startPt(SET_XORY1, SET_XORY1);
    Drawing::Point endPt(SET_XORY2, SET_XORY2);
    paintFilterCanvas_->DrawLine(startPt, endPt);
    EXPECT_TRUE(paintFilterCanvas_);
}

/**
 * @tc.name: DrawRectTest
 * @tc.desc: DrawRect Test
 * @tc.type: FUNC
 * @tc.require: issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, DrawRectTest, TestSize.Level1)
{
    Drawing::Rect rect;
    paintFilterCanvas_->DrawRect(rect);
    EXPECT_TRUE(paintFilterCanvas_);
}

/**
 * @tc.name: DrawRoundRectTest
 * @tc.desc: DrawRoundRect Test
 * @tc.type: FUNC
 * @tc.require: issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, DrawRoundRectTest, TestSize.Level1)
{
    Drawing::RoundRect roundRect;
    paintFilterCanvas_->DrawRoundRect(roundRect);
    EXPECT_TRUE(paintFilterCanvas_);
}

/**
 * @tc.name: DrawNestedRoundRectTest
 * @tc.desc: DrawNestedRoundRect Test
 * @tc.type: FUNC
 * @tc.require: issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, DrawNestedRoundRectTest, TestSize.Level1)
{
    Drawing::RoundRect outer;
    Drawing::RoundRect inner;
    paintFilterCanvas_->DrawNestedRoundRect(outer, inner);
    EXPECT_TRUE(paintFilterCanvas_);
}

/**
 * @tc.name: DrawArcTest
 * @tc.desc: DrawArc Test
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, DrawArcTest, TestSize.Level1)
{
    Drawing::Rect oval;
    paintFilterCanvas_->DrawArc(oval, ANGLE_SWEEP_START, ANGLE_SWEEP_START);
    EXPECT_TRUE(paintFilterCanvas_);
}

/**
 * @tc.name: DrawPieTest
 * @tc.desc: DrawPie Test
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, DrawPieTest, TestSize.Level1)
{
    Drawing::Rect oval;
    paintFilterCanvas_->DrawPie(oval, ANGLE_SWEEP_START, ANGLE_SWEEP_START);
    EXPECT_TRUE(paintFilterCanvas_);
}

/**
 * @tc.name: DrawOvalTest
 * @tc.desc: DrawOval Test
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, DrawOvalTest, TestSize.Level1)
{
    Drawing::Rect oval;
    paintFilterCanvas_->DrawOval(oval);
    EXPECT_TRUE(paintFilterCanvas_);
}

/**
 * @tc.name: DrawCircleTest
 * @tc.desc: DrawCircle Test
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, DrawCircleTest, TestSize.Level1)
{
    Drawing::Point point;
    paintFilterCanvas_->DrawCircle(point, SET_RADIUS);
    EXPECT_TRUE(paintFilterCanvas_);
}

/**
 * @tc.name: DrawPathTest
 * @tc.desc: DrawPath Test
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, DrawPathTest, TestSize.Level1)
{
    Drawing::Path path;
    paintFilterCanvas_->DrawPath(path);
    EXPECT_TRUE(paintFilterCanvas_);
}

/**
 * @tc.name: DrawBackgroundTest
 * @tc.desc: DrawBackground Test
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, DrawBackgroundTest, TestSize.Level1)
{
    Drawing::Brush brush;
    paintFilterCanvas_->DrawBackground(brush);
    EXPECT_TRUE(paintFilterCanvas_);
}

/**
 * @tc.name: DrawShadowTest
 * @tc.desc: DrawShadow Test
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, DrawShadowTest, TestSize.Level1)
{
    Drawing::Path path;
    Drawing::Point3 planeParams;
    Drawing::Point3 devLightPos;
    // for test
    Drawing::Color ambientColor(0, 0, 0, 0);
    Drawing::Color spotColor(SCALAR_XORY, 0, 0, 0);
    paintFilterCanvas_->DrawShadow(
        path, planeParams, devLightPos, 0.0f, ambientColor, spotColor, Drawing::ShadowFlags::NONE);
    EXPECT_TRUE(paintFilterCanvas_);
}

/**
 * @tc.name: DrawColorTest
 * @tc.desc: DrawColor Test
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, DrawColorTest, TestSize.Level1)
{
    paintFilterCanvas_->DrawColor(1, Drawing::BlendMode::CLEAR);
    EXPECT_TRUE(paintFilterCanvas_);
}

/**
 * @tc.name: DrawRegionTest
 * @tc.desc: DrawRegion Test
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, DrawRegionTest, TestSize.Level1)
{
    Drawing::Region region;
    paintFilterCanvas_->DrawRegion(region);
    EXPECT_TRUE(paintFilterCanvas_);
}

/**
 * @tc.name: DrawPatchTest
 * @tc.desc: DrawPatch Test
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, DrawPatchTest, TestSize.Level1)
{
    // for test
    Drawing::Point cubics[SET_POINT_SIZE1];
    Drawing::ColorQuad colors[SET_COLOR_QUAD_SIZE];
    Drawing::Point texCoords[SET_POINT_SIZE2];
    paintFilterCanvas_->DrawPatch(cubics, colors, texCoords, Drawing::BlendMode::CLEAR);
    EXPECT_TRUE(paintFilterCanvas_);
}

/**
 * @tc.name: DrawVerticesTest
 * @tc.desc: DrawVertices Test
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, DrawVerticesTest, TestSize.Level1)
{
    Drawing::Vertices vertices;
    paintFilterCanvas_->DrawVertices(vertices, Drawing::BlendMode::CLEAR);
    EXPECT_TRUE(paintFilterCanvas_);
}

/**
 * @tc.name: OpCalculateBeforeTest
 * @tc.desc: OpCalculateBefore Test
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, OpCalculateBeforeTest, TestSize.Level1)
{
    Drawing::Matrix matrix;
    EXPECT_FALSE(paintFilterCanvas_->OpCalculateBefore(matrix));
    paintFilterCanvas_->SetAlpha(SET_ALPHA);
    EXPECT_TRUE(paintFilterCanvas_->OpCalculateBefore(matrix));
}

/**
 * @tc.name: OpCalculateAfterTest
 * @tc.desc: OpCalculateAfter Test
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, OpCalculateAfterTest, TestSize.Level1)
{
    Drawing::Rect bound;
    EXPECT_FALSE(paintFilterCanvas_->OpCalculateAfter(bound));
    paintFilterCanvas_->SetAlpha(0.0f);
    EXPECT_FALSE(paintFilterCanvas_->OpCalculateAfter(bound));
}

/**
 * @tc.name: DrawBitmapTest
 * @tc.desc: DrawBitmap Test
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, DrawBitmapTest, TestSize.Level1)
{
    Drawing::Bitmap bitmap;
    paintFilterCanvas_->DrawBitmap(bitmap, SET_XORY1, SET_XORY1);
    paintFilterCanvas_->SetAlpha(SET_ALPHA);
    paintFilterCanvas_->DrawBitmap(bitmap, SET_XORY1, SET_XORY1);
    EXPECT_TRUE(paintFilterCanvas_);
}

/**
 * @tc.name: DrawImageNineTest
 * @tc.desc: DrawImageNine Test
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, DrawImageNineTest, TestSize.Level1)
{
    Drawing::Image image;
    Drawing::RectI center;
    Drawing::Rect dst;
    Drawing::Brush brush;
    paintFilterCanvas_->SetAlpha(SET_ALPHA);
    paintFilterCanvas_->DrawImageNine(&image, center, dst, Drawing::FilterMode::LINEAR, &brush);
    EXPECT_TRUE(paintFilterCanvas_);
}

/**
 * @tc.name: DrawImageLatticeTest
 * @tc.desc: DrawImageLattice Test
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, DrawImageLatticeTest, TestSize.Level1)
{
    Drawing::Image image;
    Drawing::Lattice lattice;
    Drawing::Rect dst;
    Drawing::Brush brush;
    paintFilterCanvas_->DrawImageLattice(&image, lattice, dst, Drawing::FilterMode::LINEAR, &brush);
    EXPECT_TRUE(paintFilterCanvas_);
}

/**
 * @tc.name: DrawImageTest
 * @tc.desc: DrawImage Test
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, DrawImageTest, TestSize.Level1)
{
    Drawing::Image image;
    Drawing::SamplingOptions sampling;
    paintFilterCanvas_->DrawImage(image, SET_XORY1, SET_XORY1, sampling);
    EXPECT_TRUE(paintFilterCanvas_);
}

/**
 * @tc.name: DrawImageRectTest001
 * @tc.desc: DrawImageRect Test
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, DrawImageRectTest001, TestSize.Level1)
{
    Drawing::Image image;
    Drawing::Rect src;
    Drawing::Rect dst;
    Drawing::SamplingOptions sampling;
    paintFilterCanvas_->DrawImageRect(image, src, dst, sampling, Drawing::SrcRectConstraint::FAST_SRC_RECT_CONSTRAINT);
    EXPECT_TRUE(paintFilterCanvas_);
}

/**
 * @tc.name: DrawImageRectTest002
 * @tc.desc: DrawImageRect Test
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, DrawImageRectTest002, TestSize.Level1)
{
    Drawing::Image image;
    Drawing::Rect dst;
    Drawing::SamplingOptions sampling;
    paintFilterCanvas_->DrawImageRect(image, dst, sampling);
    EXPECT_TRUE(paintFilterCanvas_);
}

/**
 * @tc.name: DrawPictureTest
 * @tc.desc: DrawPicture Test
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, DrawPictureTest, TestSize.Level1)
{
    Drawing::Picture picture;
    paintFilterCanvas_->DrawPicture(picture);
    EXPECT_TRUE(paintFilterCanvas_);
}

/**
 * @tc.name: DrawTextBlobTest
 * @tc.desc: DrawTextBlob Test
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, DrawTextBlobTest, TestSize.Level1)
{
    // for tset
    const char* text = "text";
    Drawing::Font font;
    auto blob = Drawing::TextBlob::MakeFromString(text, font);
    paintFilterCanvas_->DrawTextBlob(blob.get(), SET_XORY1, SET_XORY1);
    EXPECT_TRUE(paintFilterCanvas_);
}

/**
 * @tc.name: ClipRectTest
 * @tc.desc: ClipRect Test
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, ClipRectTest, TestSize.Level1)
{
    Drawing::Rect rect;
    paintFilterCanvas_->ClipRect(rect, Drawing::ClipOp::DIFFERENCE, true);
    EXPECT_TRUE(paintFilterCanvas_);
}

/**
 * @tc.name: ClipIRectTest
 * @tc.desc: ClipIRect Test
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, ClipIRectTest, TestSize.Level1)
{
    Drawing::RectI rect;
    paintFilterCanvas_->ClipIRect(rect, Drawing::ClipOp::DIFFERENCE);
    EXPECT_TRUE(paintFilterCanvas_);
}

/**
 * @tc.name: ClipRoundRectTest001
 * @tc.desc: ClipRoundRect Test
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, ClipRoundRectTest001, TestSize.Level1)
{
    Drawing::RoundRect roundRect;
    paintFilterCanvas_->ClipRoundRect(roundRect, Drawing::ClipOp::DIFFERENCE, true);
    EXPECT_TRUE(paintFilterCanvas_);
}

/**
 * @tc.name: ClipRoundRectTest002
 * @tc.desc: ClipRoundRect Test
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, ClipRoundRectTest002, TestSize.Level1)
{
    Drawing::Rect rect;
    std::vector<Drawing::Point> pts;
    paintFilterCanvas_->ClipRoundRect(rect, pts, true);
    EXPECT_TRUE(paintFilterCanvas_);
}

/**
 * @tc.name: ClipPathTest
 * @tc.desc: ClipPath Test
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, ClipPathTest, TestSize.Level1)
{
    Drawing::Path path;
    paintFilterCanvas_->ClipPath(path, Drawing::ClipOp::DIFFERENCE, true);
    EXPECT_TRUE(paintFilterCanvas_);
}

/**
 * @tc.name: ClipRegionTest
 * @tc.desc: ClipRegion Test
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, ClipRegionTest, TestSize.Level1)
{
    Drawing::Region region;
    paintFilterCanvas_->ClipRegion(region, Drawing::ClipOp::DIFFERENCE);
    EXPECT_TRUE(paintFilterCanvas_);
}

/**
 * @tc.name: SetMatrixTest
 * @tc.desc: SetMatrix Test
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, SetMatrixTest, TestSize.Level1)
{
    Drawing::Matrix matrix;
    paintFilterCanvas_->SetMatrix(matrix);
    EXPECT_TRUE(paintFilterCanvas_);
}

/**
 * @tc.name: ResetMatrixTest
 * @tc.desc: ResetMatrix Test
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, ResetMatrixTest, TestSize.Level1)
{
    paintFilterCanvas_->ResetMatrix();
    EXPECT_TRUE(paintFilterCanvas_);
}

/**
 * @tc.name: ConcatMatrixTest
 * @tc.desc: ConcatMatrix Test
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, ConcatMatrixTest, TestSize.Level1)
{
    Drawing::Matrix matrix;
    paintFilterCanvas_->ConcatMatrix(matrix);
    EXPECT_TRUE(paintFilterCanvas_);
}

/**
 * @tc.name: TranslateTest
 * @tc.desc: Verify function Translate
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, TranslateTest, TestSize.Level1)
{
    paintFilterCanvas_->Translate(SET_XORY1, SET_XORY1);
    EXPECT_TRUE(paintFilterCanvas_);
}

/**
 * @tc.name: ScaleTest
 * @tc.desc: Scale Test
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, ScaleTest, TestSize.Level1)
{
    paintFilterCanvas_->Scale(SET_XORY1, SET_XORY1);
    EXPECT_TRUE(paintFilterCanvas_);
}

/**
 * @tc.name: RotateTest
 * @tc.desc: Rotate Test
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, RotateTest, TestSize.Level1)
{
    paintFilterCanvas_->Rotate(SET_SCALAR, SET_XORY1, SET_XORY1);
    EXPECT_TRUE(paintFilterCanvas_);
}

/**
 * @tc.name: ShearTest
 * @tc.desc: Shear Test
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, ShearTest, TestSize.Level1)
{
    paintFilterCanvas_->Shear(SET_XORY1, SET_XORY1);
    EXPECT_TRUE(paintFilterCanvas_);
}

/**
 * @tc.name: FlushTest
 * @tc.desc: Flush Test
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, FlushTest, TestSize.Level1)
{
    paintFilterCanvas_->Flush();
    EXPECT_TRUE(paintFilterCanvas_);
}

/**
 * @tc.name: ClearTest
 * @tc.desc: Clear Test
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, ClearTest, TestSize.Level1)
{
    paintFilterCanvas_->Clear(1);
    EXPECT_TRUE(paintFilterCanvas_);
}

/**
 * @tc.name: SaveTest
 * @tc.desc: Save Test
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, SaveTest, TestSize.Level1)
{
    EXPECT_EQ(paintFilterCanvas_->Save(), 1);
    paintFilterCanvas_->canvas_ = nullptr;
    EXPECT_EQ(paintFilterCanvas_->Save(), 0);
}

/**
 * @tc.name: SaveLayerTest
 * @tc.desc: SaveLayer Test
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, SaveLayerTest, TestSize.Level1)
{
    Drawing::SaveLayerOps saveLayerRec;
    paintFilterCanvas_->SaveLayer(saveLayerRec);
    paintFilterCanvas_->canvas_ = &drawingCanvas_;
    paintFilterCanvas_->SaveLayer(saveLayerRec);
    Drawing::Brush brush;
    saveLayerRec.brush_ = &brush;
    paintFilterCanvas_->SaveLayer(saveLayerRec);
    EXPECT_TRUE(paintFilterCanvas_);
}

/**
 * @tc.name: RestoreTest
 * @tc.desc: Restore Test
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, RestoreTest, TestSize.Level1)
{
    paintFilterCanvas_->Restore();
    EXPECT_TRUE(paintFilterCanvas_);
}

/**
 * @tc.name: DiscardTest
 * @tc.desc: Discard Test
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, DiscardTest, TestSize.Level1)
{
    paintFilterCanvas_->Discard();
    EXPECT_TRUE(paintFilterCanvas_);
}

/**
 * @tc.name: AttachPenTest001
 * @tc.desc: AttachPen Test
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, AttachPenTest001, TestSize.Level1)
{
    Drawing::Pen pen;
    EXPECT_TRUE(paintFilterCanvas_->AttachPen(pen).impl_);
}

/**
 * @tc.name: AttachBrushTest
 * @tc.desc: AttachBrush Test
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, AttachBrushTest, TestSize.Level1)
{
    Drawing::Brush brush;
    EXPECT_TRUE(paintFilterCanvas_->AttachBrush(brush).impl_);
}

/**
 * @tc.name: AttachPaintTest
 * @tc.desc: AttachPaint Test
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, AttachPaintTest, TestSize.Level1)
{
    Drawing::Paint paint;
    EXPECT_TRUE(paintFilterCanvas_->AttachPaint(paint).impl_);
}

/**
 * @tc.name: DetachPenTest
 * @tc.desc: DetachPen Test
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, DetachPenTest, TestSize.Level1)
{
    EXPECT_TRUE(paintFilterCanvas_->DetachPen().impl_);
}

/**
 * @tc.name: DetachBrushTest
 * @tc.desc: DetachBrush Test
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, DetachBrushTest, TestSize.Level1)
{
    EXPECT_TRUE(paintFilterCanvas_->DetachBrush().impl_);
}

/**
 * @tc.name: DetachPaintTest
 * @tc.desc: DetachPaint Test
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, DetachPaintTest, TestSize.Level1)
{
    EXPECT_TRUE(paintFilterCanvas_->DetachPaint().impl_);
}

/**
 * @tc.name: AttachPenTest002
 * @tc.desc: AttachPen Test
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, AttachPenTest002, TestSize.Level1)
{
    Drawing::Pen pen;

    paintFilterCanvas_->canvas_ = nullptr;
    EXPECT_TRUE(paintFilterCanvas_->AttachPen(pen).impl_);

    paintFilterCanvas_->canvas_ = &drawingCanvas_;
    pen.SetColor(1);

    paintFilterCanvas_->SetAlpha(SET_ALPHA);
    paintFilterCanvas_->SetBlendMode(1);
    EXPECT_TRUE(paintFilterCanvas_->AttachPen(pen).impl_);
}

/**
 * @tc.name: AttachBrushTest001
 * @tc.desc: AttachBrush Test
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, AttachBrushTest001, TestSize.Level1)
{
    Drawing::Brush brush;
    paintFilterCanvas_->canvas_ = nullptr;
    EXPECT_TRUE(paintFilterCanvas_->AttachBrush(brush).impl_);

    paintFilterCanvas_->canvas_ = &drawingCanvas_;
    brush.SetColor(1);

    paintFilterCanvas_->SetAlpha(SET_ALPHA);
    paintFilterCanvas_->SetBlendMode(1);
    EXPECT_TRUE(paintFilterCanvas_->AttachBrush(brush).impl_);
}

/**
 * @tc.name: AttachPaintTest001
 * @tc.desc: AttachPaint Test
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, AttachPaintTest001, TestSize.Level1)
{
    Drawing::Paint paint;
    paintFilterCanvas_->canvas_ = nullptr;
    EXPECT_TRUE(paintFilterCanvas_->AttachPaint(paint).impl_);

    paintFilterCanvas_->canvas_ = &drawingCanvas_;
    paint.SetColor(1);

    paintFilterCanvas_->SetAlpha(SET_ALPHA);
    paintFilterCanvas_->SetBlendMode(1);
    EXPECT_TRUE(paintFilterCanvas_->AttachPaint(paint).impl_);
}

/**
 * @tc.name: AlphaStackToPushOrOutbackTest
 * @tc.desc: AlphaStackToPushOrOutback Test
 * @tc.type:FUNC
 * @tc.require:issueI9L0ZK
 */
HWTEST_F(RSPaintFilterCanvasTest, AlphaStackToPushOrOutbackTest, TestSize.Level1)
{
    // Ensure that the stack is empty before testing
    EXPECT_TRUE(AlphaStackClear());
    // RestoreAlpha Test
    paintFilterCanvas_->RestoreAlpha();
    paintFilterCanvas_->alphaStack_.push(SET_ALPHA);
    EXPECT_EQ(paintFilterCanvas_->SaveAlpha(), 1);
    EXPECT_EQ(paintFilterCanvas_->GetAlphaSaveCount(), NUMBER);
    paintFilterCanvas_->RestoreAlpha();
    EXPECT_EQ(paintFilterCanvas_->GetAlphaSaveCount(), 1);

    // RestoreAlphaToCount Test
    EXPECT_EQ(paintFilterCanvas_->SaveAlpha(), 1);
    paintFilterCanvas_->RestoreAlphaToCount(0);
    EXPECT_EQ(paintFilterCanvas_->GetAlphaSaveCount(), 1);

    // Ensure that the stack is empty after testing
    EXPECT_TRUE(AlphaStackClear());
}

/**
 * @tc.name: EnvStackToPushOrOutbackTest
 * @tc.desc: EnvStackToPushOrOutback Test
 * @tc.type:FUNC
 * @tc.require:issueI9L0ZK
 */
HWTEST_F(RSPaintFilterCanvasTest, EnvStackToPushOrOutbackTest, TestSize.Level1)
{
    // Ensure that the stack is empty before testing
    EXPECT_TRUE(EnvStackClear());

    Rosen::RSColor rsColor;
    RSPaintFilterCanvas::Env env;

    // envStack_ is empty
    EXPECT_EQ(paintFilterCanvas_->GetEnvForegroundColor(), Drawing::Color::COLOR_BLACK);
    paintFilterCanvas_->SetEnvForegroundColor(rsColor);
    paintFilterCanvas_->RestoreEnv();

    // envStack_ is not empty
    paintFilterCanvas_->envStack_.push(env);
    auto asArgbInt = paintFilterCanvas_->envStack_.top().envForegroundColor_.AsArgbInt();
    EXPECT_EQ(paintFilterCanvas_->GetEnvForegroundColor(), asArgbInt);
    paintFilterCanvas_->SetBlendMode(0);
    paintFilterCanvas_->SetEnvForegroundColor(rsColor);
    EXPECT_EQ(paintFilterCanvas_->GetEnvSaveCount(), 1);
    EXPECT_EQ(paintFilterCanvas_->SaveEnv(), 1);

    // envStack_ Outback
    paintFilterCanvas_->RestoreEnv();
    EXPECT_EQ(paintFilterCanvas_->GetEnvSaveCount(), 1);
    EXPECT_EQ(paintFilterCanvas_->SaveEnv(), 1);
    paintFilterCanvas_->RestoreEnvToCount(0);

    // Ensure that the stack is empty after testing
    EXPECT_TRUE(EnvStackClear());
}

/**
 * @tc.name: SaveAllStatusTest
 * @tc.desc: SaveAllStatus Test
 * @tc.type:FUNC
 * @tc.require:issueI9L0ZK
 */
HWTEST_F(RSPaintFilterCanvasTest, SaveAllStatusTest, TestSize.Level1)
{
    // The second condition of the ternary operation
    auto drawingCanvas = std::make_unique<Drawing::Canvas>();
    paintFilterCanvas_->canvas_ = drawingCanvas.get();

    RSPaintFilterCanvas::SaveStatus secondStatus = paintFilterCanvas_->SaveAllStatus(RSPaintFilterCanvas::kNone);
    EXPECT_EQ(paintFilterCanvas_->GetSaveCount(), secondStatus.canvasSaveCount);
    EXPECT_EQ(paintFilterCanvas_->GetAlphaSaveCount(), secondStatus.alphaSaveCount);
    EXPECT_EQ(paintFilterCanvas_->GetEnvSaveCount(), secondStatus.envSaveCount);
    // The first condition of the ternary operation
    paintFilterCanvas_->canvas_ = nullptr;
    RSPaintFilterCanvas::Env env;
    paintFilterCanvas_->envStack_.push(env);
    paintFilterCanvas_->alphaStack_.push(SET_ALPHA);
    RSPaintFilterCanvas::SaveStatus firstStatus = paintFilterCanvas_->SaveAllStatus(RSPaintFilterCanvas::kAll);
    EXPECT_EQ(paintFilterCanvas_->Save(), firstStatus.canvasSaveCount);
    EXPECT_EQ(paintFilterCanvas_->GetAlphaSaveCount() - 1, firstStatus.alphaSaveCount);
    EXPECT_EQ(paintFilterCanvas_->GetEnvSaveCount() - 1, firstStatus.envSaveCount);

    EXPECT_TRUE(AlphaStackClear());
    EXPECT_TRUE(EnvStackClear());
}

/**
 * @tc.name: CopyConfigurationTest
 * @tc.desc: CopyConfiguration Test
 * @tc.type:FUNC
 * @tc.require:issueI9L0ZK
 */
HWTEST_F(RSPaintFilterCanvasTest, CopyConfigurationTest, TestSize.Level1)
{
    // building a new RSPaintFilterCanvas object
    auto drawingCanvas = std::make_unique<Drawing::Canvas>();
    RSPaintFilterCanvas::Env env;
    RSPaintFilterCanvas::Env envOther;
    RSPaintFilterCanvas rsPaintFilterCanvas(drawingCanvas.get());

    paintFilterCanvas_->envStack_.push(env);
    rsPaintFilterCanvas.envStack_.push(envOther);

    rsPaintFilterCanvas.SetHighContrast(false);
    paintFilterCanvas_->CopyConfiguration(rsPaintFilterCanvas);

    rsPaintFilterCanvas.SetHighContrast(true);
    paintFilterCanvas_->CopyConfiguration(rsPaintFilterCanvas);
    EXPECT_TRUE(EnvStackClear());
}

/**
 * @tc.name: ReplaceOrSwapMainScreenTest
 * @tc.desc: ReplaceMainScreenData and SwapBackMainScreenData Test
 * @tc.type:FUNC
 * @tc.require:issueI9L0ZK
 */
HWTEST_F(RSPaintFilterCanvasTest, ReplaceOrSwapMainScreenTest, TestSize.Level1)
{
    Drawing::Surface surface;
    Drawing::Canvas canvas(&surface);
    RSPaintFilterCanvas paintFilterCanvas(&canvas);
    std::shared_ptr<Drawing::Surface> surfacePtr = std::make_shared<Drawing::Surface>();
    std::shared_ptr<RSPaintFilterCanvas> canvasPtr = std::make_shared<RSPaintFilterCanvas>(&canvas);

    // Replace data
    paintFilterCanvas.ReplaceMainScreenData(surfacePtr, canvasPtr);
    EXPECT_EQ(paintFilterCanvas.GetSurface(), surfacePtr.get());
    EXPECT_EQ(paintFilterCanvas.GetRecordingCanvas(), nullptr);

    // Swap data
    paintFilterCanvas.SwapBackMainScreenData();
    EXPECT_TRUE(paintFilterCanvas.offscreenDataList_.empty());
}

/**
 * @tc.name: SaveOrRestoreTest
 * @tc.desc: SavePCanvasList and RestorePCanvasList Test
 * @tc.type:FUNC
 * @tc.require:issueI9L0ZK
 */
HWTEST_F(RSPaintFilterCanvasTest, SaveOrRestoreTest, TestSize.Level1)
{
    paintFilterCanvas_->SavePCanvasList();
    EXPECT_EQ(paintFilterCanvas_->storedPCanvasList_.size(), 1);
    paintFilterCanvas_->RestorePCanvasList();
    EXPECT_TRUE(paintFilterCanvas_->storedPCanvasList_.empty());
    paintFilterCanvas_->pCanvasList_.clear();
    EXPECT_TRUE(paintFilterCanvas_->pCanvasList_.empty());
}

/**
 * @tc.name: GetLocalClipBoundsTest
 * @tc.desc: GetLocalClipBounds Test
 * @tc.type:FUNC
 * @tc.require:issueI9L0ZK
 */
HWTEST_F(RSPaintFilterCanvasTest, GetLocalClipBoundsTest, TestSize.Level1)
{
    Drawing::Canvas canvas;

    Drawing::RectI* clipRects = nullptr;
    std::optional<Drawing::Rect> result = paintFilterCanvas_->GetLocalClipBounds(canvas, clipRects);
    EXPECT_FALSE(result.has_value());

    Drawing::RectI clipRect;
    Drawing::RectI* clipRectPtr = &clipRect;
    result = paintFilterCanvas_->GetLocalClipBounds(canvas, clipRectPtr);
    EXPECT_FALSE(result.has_value());

    Drawing::RectI rectI = { 0, SET_RIGHT, SET_TOP_BOTTOM, SET_TOP_BOTTOM };
    result = paintFilterCanvas_->GetLocalClipBounds(canvas, &rectI);
    EXPECT_TRUE(result.has_value());
}
} // namespace Rosen
} // namespace OHOS