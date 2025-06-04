/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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
#include "render/rs_filter.h"

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
const float_t SET_BRIGHTNESS_RATIO = 0.5f;
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

class SDFShapeBaseTest : public Drawing::SDFShapeBase {
public:
    const std::string& Getshader() const override
    {
        return std::nullptr_t();
    };
    float GetSize() const override
    {
        return 0.0f;
    };
    void SetSize(float size) override
    {
        return;
    };
    void SetTranslate(float dx, float dy) override
    {
        return;
    };
    float GetTranslateX() const override
    {
        return 0.0f;
    };
    float GetTranslateY() const override
    {
        return 0.0f;
    };
    void SetBoundsRect(const Rosen::Drawing::Rect& rect) override
    {
        return;
    };
    Rosen::Drawing::Rect GetBoundsRect() const override
    {
        Rosen::Drawing::Rect temp;
        return temp;
    };
    int GetShapeId() const override
    {
        return 0;
    };
    void SetTransparent(int transparent) override
    {
        return;
    };
    int GetTransparent() const override
    {
        return 0;
    };
    std::vector<float> GetPaintPara() const override
    {
        return {};
    };
    std::vector<float> GetPointAndColorPara() const override
    {
        return {};
    };
    int GetParaNum() const override
    {
        return 0;
    };
    int GetFillType() const override
    {
        return 0;
    };
    std::vector<float> GetPara() const override
    {
        return {};
    };
    std::vector<float> GetTransPara() const override
    {
        return {};
    };
    std::vector<float> GetColorPara() const override
    {
        return {};
    };

    void UpdateTime(float time) override
    {
        return;
    };
    void BuildShader() override
    {
        return;
    };
    void SetColor(std::string fillColor, std::string strokeColor, float alpha)
    {
        return;
    };
};

class PaintFilterCanvasBaseTest : public RSPaintFilterCanvasBase {
public:
    explicit PaintFilterCanvasBaseTest(Drawing::Canvas* canvas) : RSPaintFilterCanvasBase(canvas) {};
    bool OnFilter() const override
    {
        return true;
    };
    bool OnFilterWithBrush(Drawing::Brush& brush) const override
    {
        return true;
    };
    Drawing::Brush* GetFilteredBrush() const override
    {
        return nullptr;
    };
};

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
    ASSERT_EQ(paintFilterCanvas_->GetAlphaSaveCount(), 1);
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
    ASSERT_EQ(paintFilterCanvas_->GetAlphaSaveCount(), 1);
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
    ASSERT_EQ(paintFilterCanvas_->GetEnvForegroundColor(), 0);
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
    ASSERT_EQ(paintFilterCanvas_->GetEnvForegroundColor(), 0);
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
    ASSERT_TRUE(paintFilterCanvas_->OnFilterWithBrush(brush));
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
 * @tc.name: DrawPathWithStencil
 * @tc.desc: DrawPathWithStencil Test
 * @tc.type:FUNC
 * @tc.require:issuesIBROZ2
 */
HWTEST_F(RSPaintFilterCanvasTest, DrawPathWithStencilTest, TestSize.Level1)
{
    Drawing::Path path;
    constexpr uint32_t stencilVal{10};
    ASSERT_TRUE(paintFilterCanvas_ != nullptr);
    paintFilterCanvas_->DrawPathWithStencil(path, stencilVal);
    EXPECT_TRUE(paintFilterCanvas_->canvas_ != nullptr);
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
 * @tc.name: DrawShadowStyleTest
 * @tc.desc: DrawShadowStyle Test
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, DrawShadowStyleTest, TestSize.Level1)
{
    Drawing::Path path;
    Drawing::Point3 planeParams;
    Drawing::Point3 devLightPos;
    // for test
    Drawing::Color ambientColor(0, 0, 0, 0);
    Drawing::Color spotColor(SCALAR_XORY, 0, 0, 0);
    paintFilterCanvas_->DrawShadowStyle(
        path, planeParams, devLightPos, 0.0f, ambientColor, spotColor, Drawing::ShadowFlags::NONE, true);
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
 * @tc.name: DrawImageNineTest002
 * @tc.desc: DrawImageNine Test
 * @tc.type:FUNC
 * @tc.require:issueI9VT6E
 */
HWTEST_F(RSPaintFilterCanvasTest, DrawImageNineTest002, TestSize.Level1)
{
    Drawing::Image image;
    Drawing::RectI center;
    Drawing::Rect dst;
    Drawing::Brush* brush = nullptr;
    Drawing::Canvas canvas;
    std::shared_ptr<RSPaintFilterCanvas> paintFilterCanvas = std::make_shared<RSPaintFilterCanvas>(&canvas);
    EXPECT_NE(paintFilterCanvas, nullptr);
    paintFilterCanvas->alphaStack_.push(1.0f);
    paintFilterCanvas->DrawImageNine(&image, center, dst, Drawing::FilterMode::LINEAR, brush);
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
    paintFilterCanvas_->DrawImageLattice(&image, lattice, dst, Drawing::FilterMode::LINEAR);
    EXPECT_TRUE(paintFilterCanvas_);
}

/**
 * @tc.name: DrawImageLatticeTest
 * @tc.desc: DrawImageLattice Test
 * @tc.type:FUNC
 * @tc.require:issueI9VT6E
 */
HWTEST_F(RSPaintFilterCanvasTest, DrawImageLatticeTest002, TestSize.Level1)
{
    Drawing::Image image;
    Drawing::Lattice lattice;
    Drawing::Rect dst;
    Drawing::Canvas canvas;
    std::shared_ptr<RSPaintFilterCanvas> paintFilterCanvas = std::make_shared<RSPaintFilterCanvas>(&canvas);
    EXPECT_NE(paintFilterCanvas, nullptr);
    paintFilterCanvas->alphaStack_.push(1.0f);
    paintFilterCanvas->DrawImageLattice(&image, lattice, dst, Drawing::FilterMode::LINEAR);
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
 * @tc.name: DrawImageWithStencilTest
 * @tc.desc: DrawImageWithStencil Test
 * @tc.type:FUNC
 * @tc.require:issuesIBROZ2
 */
HWTEST_F(RSPaintFilterCanvasTest, DrawImageWithStencilTest, TestSize.Level1)
{
    Drawing::Image image;
    Drawing::SamplingOptions sampling;
    constexpr uint32_t stencilVal{10};
    ASSERT_TRUE(paintFilterCanvas_ != nullptr);
    paintFilterCanvas_->DrawImageWithStencil(image, SET_XORY1, SET_XORY1, sampling, stencilVal);
    EXPECT_TRUE(paintFilterCanvas_->canvas_ != nullptr);
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
 * @tc.name: ClearStencilTest
 * @tc.desc: ClearStencil Test
 * @tc.type:FUNC
 * @tc.require:issuesIBROZ2
 */
HWTEST_F(RSPaintFilterCanvasTest, ClearStencilTest, TestSize.Level1)
{
    Drawing::RectI rect;
    constexpr uint32_t stencilVal{10};
    ASSERT_TRUE(paintFilterCanvas_ != nullptr);
    paintFilterCanvas_->ClearStencil(rect, stencilVal);
    EXPECT_TRUE(paintFilterCanvas_->canvas_ != nullptr);
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
 * @tc.name: OnMultipleScreenTest
 * @tc.desc: OnMultipleScreen Test
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, OnMultipleScreenTest, TestSize.Level1)
{
    ASSERT_NE(paintFilterCanvas_, nullptr);
    paintFilterCanvas_->SetOnMultipleScreen(true);
    EXPECT_EQ(paintFilterCanvas_->IsOnMultipleScreen(), true);
    paintFilterCanvas_->SetOnMultipleScreen(false);
    EXPECT_EQ(paintFilterCanvas_->IsOnMultipleScreen(), false);
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

    paint.SetBlenderEnabled(false);
    paintFilterCanvas_->SetAlpha(SET_ALPHA);
    paintFilterCanvas_->SetBlendMode(1);
    EXPECT_TRUE(paintFilterCanvas_->AttachPaint(paint).impl_);

    EXPECT_TRUE(paintFilterCanvas_->AttachPaint(paint).impl_);

    paintFilterCanvas_->alphaStack_.pop();
    paintFilterCanvas_->alphaStack_.push(1.f);
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
 * @tc.name: CopyConfigurationToOffscreenCanvas
 * @tc.desc: CopyConfigurationToOffscreenCanvas Test
 * @tc.type:FUNC
 * @tc.require:issueI9L0ZK
 */
HWTEST_F(RSPaintFilterCanvasTest, CopyConfigurationToOffscreenCanvasTest, TestSize.Level1)
{
    // building a new RSPaintFilterCanvas object
    auto drawingCanvas = std::make_unique<Drawing::Canvas>();
    RSPaintFilterCanvas::Env env;
    RSPaintFilterCanvas::Env envOther;
    RSPaintFilterCanvas rsPaintFilterCanvas(drawingCanvas.get());

    paintFilterCanvas_->envStack_.push(env);
    rsPaintFilterCanvas.envStack_.push(envOther);

    rsPaintFilterCanvas.SetHighContrast(false);
    paintFilterCanvas_->CopyConfigurationToOffscreenCanvas(rsPaintFilterCanvas);

    rsPaintFilterCanvas.SetHighContrast(true);
    paintFilterCanvas_->CopyConfigurationToOffscreenCanvas(rsPaintFilterCanvas);

    envOther.effectData_ = std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    envOther.effectData_->cachedImage_ = std::make_shared<Drawing::Image>();
    env.effectData_ = std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    env.effectData_->cachedImage_ = std::make_shared<Drawing::Image>();
    paintFilterCanvas_->envStack_.push(env);
    rsPaintFilterCanvas.envStack_.push(envOther);

    EXPECT_TRUE(rsPaintFilterCanvas.envStack_.top().effectData_ != nullptr);
    rsPaintFilterCanvas.SetHighContrast(false);

    paintFilterCanvas_->CopyConfigurationToOffscreenCanvas(rsPaintFilterCanvas);
    rsPaintFilterCanvas.SetHighContrast(true);
    paintFilterCanvas_->CopyConfigurationToOffscreenCanvas(rsPaintFilterCanvas);
    EXPECT_TRUE(EnvStackClear());
}

/**
 * @tc.name: ReplaceOrSwapMainScreenTest001
 * @tc.desc: ReplaceMainScreenData and SwapBackMainScreenData Test
 * @tc.type:FUNC
 * @tc.require:issueI9L0ZK
 */
HWTEST_F(RSPaintFilterCanvasTest, ReplaceOrSwapMainScreenTest001, TestSize.Level1)
{
    auto canvas = std::make_unique<Drawing::Canvas>();
    RSPaintFilterCanvas paintFilterCanvas(canvas.get());
    EXPECT_TRUE(paintFilterCanvas.storeMainScreenSurface_.empty());
    EXPECT_TRUE(paintFilterCanvas.storeMainScreenCanvas_.empty());
    EXPECT_TRUE(paintFilterCanvas.offscreenDataList_.empty());
    paintFilterCanvas.SwapBackMainScreenData();
    EXPECT_TRUE(paintFilterCanvas.storeMainScreenCanvas_.empty());

    std::shared_ptr<Drawing::Surface> surfacePtr = std::make_shared<Drawing::Surface>();
    std::shared_ptr<RSPaintFilterCanvas> canvasPtr = std::make_shared<RSPaintFilterCanvas>(canvas.get());

    // Replace data
    paintFilterCanvas.ReplaceMainScreenData(surfacePtr, canvasPtr);
    EXPECT_EQ(paintFilterCanvas.GetSurface(), surfacePtr.get());
    EXPECT_EQ(paintFilterCanvas.GetRecordingCanvas(), nullptr);

    // Swap data
    EXPECT_FALSE(paintFilterCanvas.storeMainScreenSurface_.empty());
    EXPECT_FALSE(paintFilterCanvas.storeMainScreenCanvas_.empty());
    EXPECT_FALSE(paintFilterCanvas.offscreenDataList_.empty());
    paintFilterCanvas.SwapBackMainScreenData();
    EXPECT_TRUE(paintFilterCanvas.storeMainScreenCanvas_.empty());

    paintFilterCanvas.ReplaceMainScreenData(surfacePtr, canvasPtr);
    paintFilterCanvas.storeMainScreenSurface_.pop();
    EXPECT_TRUE(paintFilterCanvas.storeMainScreenSurface_.empty());
    EXPECT_FALSE(paintFilterCanvas.storeMainScreenCanvas_.empty());
    EXPECT_FALSE(paintFilterCanvas.offscreenDataList_.empty());
    paintFilterCanvas.SwapBackMainScreenData();
    paintFilterCanvas.storeMainScreenCanvas_.pop();
    paintFilterCanvas.offscreenDataList_.pop();
    EXPECT_TRUE(paintFilterCanvas.storeMainScreenCanvas_.empty());

    paintFilterCanvas.ReplaceMainScreenData(surfacePtr, canvasPtr);
    paintFilterCanvas.storeMainScreenCanvas_.pop();
    EXPECT_FALSE(paintFilterCanvas.storeMainScreenSurface_.empty());
    EXPECT_TRUE(paintFilterCanvas.storeMainScreenCanvas_.empty());
    EXPECT_FALSE(paintFilterCanvas.offscreenDataList_.empty());
    paintFilterCanvas.SwapBackMainScreenData();
    paintFilterCanvas.storeMainScreenSurface_.pop();
    paintFilterCanvas.offscreenDataList_.pop();
    EXPECT_TRUE(paintFilterCanvas.storeMainScreenCanvas_.empty());

    paintFilterCanvas.ReplaceMainScreenData(surfacePtr, canvasPtr);
    paintFilterCanvas.offscreenDataList_.pop();
    EXPECT_FALSE(paintFilterCanvas.storeMainScreenSurface_.empty());
    EXPECT_FALSE(paintFilterCanvas.storeMainScreenCanvas_.empty());
    EXPECT_TRUE(paintFilterCanvas.offscreenDataList_.empty());
    paintFilterCanvas.SwapBackMainScreenData();
    paintFilterCanvas.storeMainScreenSurface_.pop();
    paintFilterCanvas.storeMainScreenCanvas_.pop();
    EXPECT_TRUE(paintFilterCanvas.storeMainScreenCanvas_.empty());
}

/**
 * @tc.name: ReplaceOrSwapMainScreenTest002
 * @tc.desc: ReplaceMainScreenData and SwapBackMainScreenData Test
 * @tc.type:FUNC
 * @tc.require:issueI9L0ZK
 */
HWTEST_F(RSPaintFilterCanvasTest, ReplaceOrSwapMainScreenTest002, TestSize.Level1)
{
    auto canvas = std::make_unique<Drawing::Canvas>();
    RSPaintFilterCanvas paintFilterCanvas(canvas.get());
    std::shared_ptr<Drawing::Surface> surfacePtr = nullptr;
    std::shared_ptr<RSPaintFilterCanvas> canvasPtr = nullptr;
    paintFilterCanvas.ReplaceMainScreenData(surfacePtr, canvasPtr);
    surfacePtr = nullptr;
    canvasPtr = std::make_shared<RSPaintFilterCanvas>(canvas.get());
    paintFilterCanvas.ReplaceMainScreenData(surfacePtr, canvasPtr);
    surfacePtr = std::make_shared<Drawing::Surface>();
    canvasPtr = nullptr;
    paintFilterCanvas.ReplaceMainScreenData(surfacePtr, canvasPtr);
    EXPECT_TRUE(paintFilterCanvas.storeMainScreenSurface_.empty());
    EXPECT_TRUE(paintFilterCanvas.storeMainScreenCanvas_.empty());
    EXPECT_TRUE(paintFilterCanvas.offscreenDataList_.empty());

    surfacePtr = std::make_shared<Drawing::Surface>();
    canvasPtr = std::make_shared<RSPaintFilterCanvas>(canvas.get());
    paintFilterCanvas.ReplaceMainScreenData(surfacePtr, canvasPtr);
    paintFilterCanvas.storeMainScreenSurface_.pop();
    paintFilterCanvas.storeMainScreenCanvas_.pop();
    EXPECT_TRUE(paintFilterCanvas.storeMainScreenSurface_.empty());
    EXPECT_TRUE(paintFilterCanvas.storeMainScreenCanvas_.empty());
    EXPECT_FALSE(paintFilterCanvas.offscreenDataList_.empty());
    paintFilterCanvas.SwapBackMainScreenData();
    paintFilterCanvas.offscreenDataList_.pop();
    EXPECT_TRUE(paintFilterCanvas.storeMainScreenCanvas_.empty());

    paintFilterCanvas.ReplaceMainScreenData(surfacePtr, canvasPtr);
    paintFilterCanvas.storeMainScreenSurface_.pop();
    paintFilterCanvas.offscreenDataList_.pop();
    EXPECT_TRUE(paintFilterCanvas.storeMainScreenSurface_.empty());
    EXPECT_FALSE(paintFilterCanvas.storeMainScreenCanvas_.empty());
    EXPECT_TRUE(paintFilterCanvas.offscreenDataList_.empty());
    paintFilterCanvas.SwapBackMainScreenData();
    paintFilterCanvas.storeMainScreenCanvas_.pop();
    EXPECT_TRUE(paintFilterCanvas.storeMainScreenCanvas_.empty());

    paintFilterCanvas.ReplaceMainScreenData(surfacePtr, canvasPtr);
    paintFilterCanvas.offscreenDataList_.pop();
    paintFilterCanvas.storeMainScreenCanvas_.pop();
    EXPECT_FALSE(paintFilterCanvas.storeMainScreenSurface_.empty());
    EXPECT_TRUE(paintFilterCanvas.storeMainScreenCanvas_.empty());
    EXPECT_TRUE(paintFilterCanvas.offscreenDataList_.empty());
    paintFilterCanvas.SwapBackMainScreenData();
    paintFilterCanvas.storeMainScreenSurface_.pop();
    EXPECT_TRUE(paintFilterCanvas.storeMainScreenCanvas_.empty());
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
 * @tc.name: BrightnessRatioTest
 * @tc.desc: SetBrightnessRatio and GetBrightnessRatio Test
 * @tc.type:FUNC
 * @tc.require:issueIAM268
 */
HWTEST_F(RSPaintFilterCanvasTest, BrightnessRatioTest, TestSize.Level1)
{
    paintFilterCanvas_->SetBrightnessRatio(SET_BRIGHTNESS_RATIO);
    EXPECT_EQ(paintFilterCanvas_->GetBrightnessRatio(), SET_BRIGHTNESS_RATIO);
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

/**
 * @tc.name: SetBlenderTest
 * @tc.desc: SetBlender
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, SetBlenderTest, TestSize.Level1)
{
    std::shared_ptr<Drawing::Blender> blender = nullptr;
    auto mode = Drawing::BlendMode::SRC;
    blender = Drawing::Blender::CreateWithBlendMode(mode);
    EXPECT_TRUE(blender != nullptr);
    Drawing::Brush brush;
    brush.SetBlender(blender);
}

/**
 * @tc.name: HdrOnTest
 * @tc.desc: GetHdrOn/SetHdrOn
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, HdrOnTest, TestSize.Level1)
{
    ASSERT_NE(paintFilterCanvas_, nullptr);
    paintFilterCanvas_->SetHdrOn(false);
    EXPECT_EQ(paintFilterCanvas_->GetHdrOn(), false);
    paintFilterCanvas_->SetHdrOn(true);
    EXPECT_EQ(paintFilterCanvas_->GetHdrOn(), true);
}

/**
 * @tc.name: SetHDRBrightness GetHDRBrightness
 * @tc.desc: SetHDRBrightness/GetHDRBrightness
 * @tc.type:FUNC
 * @tc.require:issuesI9J2YE
 */
HWTEST_F(RSPaintFilterCanvasTest, HDRBrightnessTest, TestSize.Level1)
{
    ASSERT_NE(paintFilterCanvas_, nullptr);
    paintFilterCanvas_->SetHDRBrightness(1.0f);
    EXPECT_EQ(paintFilterCanvas_->GetHDRBrightness(), 1.0f);
    paintFilterCanvas_->SetHDRBrightness(0.2f);
    EXPECT_EQ(paintFilterCanvas_->GetHDRBrightness(), 0.2f);
}

/**
 * @tc.name: DrawSdfTest001
 * @tc.desc: DrawSdf Test
 * @tc.type:FUNC
 * @tc.require: issueI9VT6E
 */
HWTEST_F(RSPaintFilterCanvasTest, DrawSdfTest001, TestSize.Level1)
{
    Drawing::Canvas canvas;
    std::shared_ptr<RSPaintFilterCanvas> paintFilterCanvasBase = std::make_shared<RSPaintFilterCanvas>(&canvas);
    EXPECT_NE(paintFilterCanvasBase, nullptr);

    SDFShapeBaseTest shape;
    Drawing::Canvas canvasTest;
    paintFilterCanvasBase->pCanvasList_.emplace_back(&canvasTest);
    paintFilterCanvasBase->alphaStack_.push(1.0f);
    paintFilterCanvasBase->DrawSdf(shape);
}

/**
 * @tc.name: DrawAtlasTest002
 * @tc.desc: DrawAtlas Test
 * @tc.type:FUNC
 * @tc.require: issueI9VT6E
 */
HWTEST_F(RSPaintFilterCanvasTest, DrawAtlasTest002, TestSize.Level1)
{
    Drawing::Canvas canvas;
    std::shared_ptr<RSPaintFilterCanvas> paintFilterCanvasBase = std::make_shared<RSPaintFilterCanvas>(&canvas);
    EXPECT_NE(paintFilterCanvasBase, nullptr);

    paintFilterCanvasBase->alphaStack_.push(1.0f);
    Drawing::Image atlas;
    Drawing::RSXform xform;
    xform.Make(0.0f, 0.0f, 1.0f, 1.0f);
    Drawing::Rect tex = { 0.0f, 0.0f, 1.0f, 1.0f };
    Drawing::SamplingOptions samplingOptions;
    Drawing::Rect cullRect = { 0.0f, 0.0f, 1.0f, 1.0f };
    Drawing::ColorQuad colors = 0;
    paintFilterCanvasBase->DrawAtlas(
        &atlas, &xform, &tex, &colors, 0, Drawing::BlendMode::CLEAR, samplingOptions, &cullRect);
}

/**
 * @tc.name: DrawBlurImageTest003
 * @tc.desc: DrawBlurImage Test
 * @tc.type:FUNC
 * @tc.require: issueI9VT6E
 */
HWTEST_F(RSPaintFilterCanvasTest, DrawBlurImageTest003, TestSize.Level1)
{
    Drawing::Canvas canvas;
    std::shared_ptr<PaintFilterCanvasBaseTest> paintFilterCanvasBase =
        std::make_shared<PaintFilterCanvasBaseTest>(&canvas);
    EXPECT_NE(paintFilterCanvasBase, nullptr);

    Drawing::Image image;
    Drawing::Rect s = { 0.0f, 0.0f, 1.0f, 1.0f };
    Drawing::Rect d = { 0.0f, 0.0f, 1.0f, 1.0f };
    Drawing::HpsBlurParameter blurParams = { s, d, 0.0f, 0.0f, 1.0f };
    paintFilterCanvasBase->DrawBlurImage(image, blurParams);
}

/**
 * @tc.name: AttachPenTest004
 * @tc.desc: AttachPen Test
 * @tc.type:FUNC
 * @tc.require: issueI9VT6E
 */
HWTEST_F(RSPaintFilterCanvasTest, AttachPenTest004, TestSize.Level1)
{
    Drawing::Canvas canvas;
    std::shared_ptr<RSPaintFilterCanvas> paintFilterCanvas = std::make_shared<RSPaintFilterCanvas>(&canvas);
    EXPECT_NE(paintFilterCanvas, nullptr);

    Drawing::Pen pen;
    Drawing::Canvas canvasTest;
    paintFilterCanvas->canvas_ = &canvasTest;
    paintFilterCanvas->multipleScreen_ = false;
    paintFilterCanvas->alphaStack_.push(1.0f);
    pen.brush_.color_ = 0x00000001;
    std::shared_ptr<RSPaintFilterCanvas::CachedEffectData> effectData = nullptr;
    std::shared_ptr<Drawing::Blender> blender = std::make_shared<Drawing::Blender>();
    RSPaintFilterCanvas::Env env = { RSColor(), nullptr, nullptr, blender, false };
    paintFilterCanvas->envStack_.push(env);
    paintFilterCanvas->AttachPen(pen);

    paintFilterCanvas->canvas_ = nullptr;
    paintFilterCanvas->AttachPen(pen);
}

/**
 * @tc.name: AttachBrushTest005
 * @tc.desc: AttachBrush Test
 * @tc.type:FUNC
 * @tc.require: issueI9VT6E
 */
HWTEST_F(RSPaintFilterCanvasTest, AttachBrushTest005, TestSize.Level1)
{
    Drawing::Canvas canvas;
    std::shared_ptr<RSPaintFilterCanvas> paintFilterCanvas = std::make_shared<RSPaintFilterCanvas>(&canvas);
    EXPECT_NE(paintFilterCanvas, nullptr);

    Drawing::Brush brush;
    Drawing::Canvas canvasTest;
    paintFilterCanvas->canvas_ = &canvasTest;
    paintFilterCanvas->multipleScreen_ = false;
    paintFilterCanvas->alphaStack_.push(1.0f);
    brush.color_ = 0x00000001;
    std::shared_ptr<RSPaintFilterCanvas::CachedEffectData> effectData = nullptr;
    std::shared_ptr<Drawing::Blender> blender = std::make_shared<Drawing::Blender>();
    RSPaintFilterCanvas::Env env = { RSColor(), nullptr, nullptr, blender, false };
    paintFilterCanvas->envStack_.push(env);
    paintFilterCanvas->AttachBrush(brush);
    brush.SetBlenderEnabled(false);
    paintFilterCanvas->AttachBrush(brush);
    paintFilterCanvas->canvas_ = nullptr;
    paintFilterCanvas->AttachBrush(brush);
}

/**
 * @tc.name: AttachPaintTest006
 * @tc.desc: AttachPaint Test
 * @tc.type:FUNC
 * @tc.require: issueI9VT6E
 */
HWTEST_F(RSPaintFilterCanvasTest, AttachPaintTest006, TestSize.Level1)
{
    Drawing::Canvas canvas;
    std::shared_ptr<RSPaintFilterCanvas> paintFilterCanvas = std::make_shared<RSPaintFilterCanvas>(&canvas);
    EXPECT_NE(paintFilterCanvas, nullptr);

    Drawing::Paint paint;
    Drawing::Canvas canvasTest;
    paintFilterCanvas->canvas_ = &canvasTest;
    paintFilterCanvas->multipleScreen_ = false;
    paintFilterCanvas->alphaStack_.push(1.0f);
    paint.color_ = 0x00000001;
    std::shared_ptr<RSPaintFilterCanvas::CachedEffectData> effectData = nullptr;
    std::shared_ptr<Drawing::Blender> blender = std::make_shared<Drawing::Blender>();
    RSPaintFilterCanvas::Env env = { RSColor(), nullptr, nullptr, blender, false };
    paintFilterCanvas->AttachPaint(paint);

    paintFilterCanvas->canvas_ = nullptr;
    paintFilterCanvas->AttachPaint(paint);
}

/**
 * @tc.name: AttachBrushAndAttachBrushTest007
 * @tc.desc: RSPaintFilterCanvasBase AttachBrush and AttachBrush Test
 * @tc.type:FUNC
 * @tc.require: issueI9VT6E
 */
HWTEST_F(RSPaintFilterCanvasTest, AttachBrushAndAttachBrushTest007, TestSize.Level1)
{
    Drawing::Canvas canvas;
    std::shared_ptr<PaintFilterCanvasBaseTest> paintFilterCanvasBase =
        std::make_shared<PaintFilterCanvasBaseTest>(&canvas);
    EXPECT_NE(paintFilterCanvasBase, nullptr);
    Drawing::Pen pen;
    paintFilterCanvasBase->AttachPen(pen);
    Drawing::Brush brush;
    paintFilterCanvasBase->AttachBrush(brush);
    Drawing::Paint paint;
    paintFilterCanvasBase->AttachPaint(paint);

    Drawing::SaveLayerOps saveLayerRec;
    paintFilterCanvasBase->canvas_ = nullptr;
    paintFilterCanvasBase->SaveLayer(saveLayerRec);

    Drawing::Canvas canvasTest;
    paintFilterCanvasBase->canvas_ = &canvasTest;
    paintFilterCanvasBase->SaveLayer(saveLayerRec);
    Drawing::Brush brushTest;
    saveLayerRec.brush_ = &brushTest;
    paintFilterCanvasBase->SaveLayer(saveLayerRec);
}

/**
 * @tc.name: DirtyRegionTest
 * @tc.desc: RSPaintFilterCanvas PushDirtyRegion and PopDirtyRegion Test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPaintFilterCanvasTest, DirtyRegionTest, TestSize.Level1)
{
    Drawing::Canvas canvas;
    auto filterCanvas = std::make_shared<RSPaintFilterCanvas>(&canvas);
    EXPECT_NE(filterCanvas, nullptr);
    EXPECT_TRUE(filterCanvas->dirtyRegionStack_.empty());
    filterCanvas->PopDirtyRegion();
    EXPECT_TRUE(filterCanvas->dirtyRegionStack_.empty());
    Drawing::Region region;
    filterCanvas->PushDirtyRegion(region);
    EXPECT_FALSE(filterCanvas->dirtyRegionStack_.empty());
    filterCanvas->PopDirtyRegion();
    EXPECT_TRUE(filterCanvas->dirtyRegionStack_.empty());
}

/**
 * @tc.name: BehindWindowDataTest
 * @tc.desc: SetBehindWindowData and GetBehindWindowData
 * @tc.type: FUNC
 * @tc.require: issueIB0UQV
 */
HWTEST_F(RSPaintFilterCanvasTest, BehindWindowDataTest, TestSize.Level1)
{
    RSPaintFilterCanvas::Env env;
    paintFilterCanvas_->envStack_.push(env);
    auto data = std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    data->cachedImage_ = std::make_shared<Drawing::Image>();
    paintFilterCanvas_->SetBehindWindowData(data);
    ASSERT_NE(paintFilterCanvas_->envStack_.top().behindWindowData_, nullptr);
    ASSERT_NE(paintFilterCanvas_->GetBehindWindowData(), nullptr);
    EXPECT_TRUE(EnvStackClear());
}

/**
 * @tc.name: IsWindowFreezeCaptureTest
 * @tc.desc: GetIsWindowFreezeCapture/SetIsWindowFreezeCapture
 * @tc.type:FUNC
 * @tc.require:issuesIBHUQ7
 */
HWTEST_F(RSPaintFilterCanvasTest, IsWindowFreezeCaptureTest, TestSize.Level1)
{
    ASSERT_NE(paintFilterCanvas_, nullptr);
    paintFilterCanvas_->SetIsWindowFreezeCapture(false);
    EXPECT_EQ(paintFilterCanvas_->GetIsWindowFreezeCapture(), false);
    paintFilterCanvas_->SetIsWindowFreezeCapture(true);
    EXPECT_EQ(paintFilterCanvas_->GetIsWindowFreezeCapture(), true);
}

/**
 * @tc.name: IsDrawingCacheTest
 * @tc.desc: GetIsDrawingCache/SetIsDrawingCache
 * @tc.type:FUNC
 * @tc.require:issuesIC0HM8
 */
HWTEST_F(RSPaintFilterCanvasTest, IsDrawingCacheTest, TestSize.Level1)
{
    ASSERT_NE(paintFilterCanvas_, nullptr);
    paintFilterCanvas_->SetIsDrawingCache(false);
    EXPECT_EQ(paintFilterCanvas_->isDrawingCache_, false);
    paintFilterCanvas_->SetIsDrawingCache(true);
    EXPECT_EQ(paintFilterCanvas_->isDrawingCache_, true);
    EXPECT_EQ(paintFilterCanvas_->GetIsDrawingCache(), true);
}

/**
 * @tc.name: CacheBehindWindowDataTest
 * @tc.desc: GetCacheBehindWindowData/SetCacheBehindWindowData
 * @tc.type:FUNC
 * @tc.require:issuesIC0HM8
 */
HWTEST_F(RSPaintFilterCanvasTest, CacheBehindWindowDataTest, TestSize.Level1)
{
    auto data = std::make_shared<RSPaintFilterCanvas::CacheBehindWindowData>();
    data->filter_ = RSFilter::RSFilter::CreateMaterialFilter(80.0f, 1.9f, 1.0f, 0xFFFFFFE5);
    paintFilterCanvas_->SetCacheBehindWindowData(data);
    ASSERT_NE(paintFilterCanvas_->cacheBehindWindowData_, nullptr);
    ASSERT_NE(paintFilterCanvas_->GetCacheBehindWindowData(), nullptr);
    paintFilterCanvas_->cacheBehindWindowData_ = nullptr;
}

/**
 * @tc.name: SetParallelRender
 * @tc.desc: Test SetParallelRender
 * @tc.type:FUNC
 * @tc.require: IC8TIV
 */
HWTEST_F(RSPaintFilterCanvasTest, SetParallelRender, TestSize.Level1)
{
    Drawing::Canvas canvas;
    std::shared_ptr<RSPaintFilterCanvas> paintFilterCanvasBase = std::make_shared<RSPaintFilterCanvas>(&canvas);
    EXPECT_NE(paintFilterCanvasBase, nullptr);
    paintFilterCanvasBase->SetParallelRender(true);
}

/**
 * @tc.name: isEffectIntersectWithDRMTest
 * @tc.desc: SetEffectIntersectWithDRM/GetIntersectWithDRM
 * @tc.type:FUNC
 * @tc.require:issuesIC0HM8
 */
HWTEST_F(RSPaintFilterCanvasTest, IsEffectIntersectWithDRMTest, TestSize.Level1)
{
    ASSERT_NE(paintFilterCanvas_, nullptr);
    paintFilterCanvas_->SetEffectIntersectWithDRM(false);
    EXPECT_EQ(paintFilterCanvas_->isIntersectWithDRM_, false);
    paintFilterCanvas_->SetEffectIntersectWithDRM(true);
    EXPECT_EQ(paintFilterCanvas_->isIntersectWithDRM_, true);
}

/**
 * @tc.name: IsDarkColorModeTest
 * @tc.desc: SetDarkColorMode/GetDarkColorMode
 * @tc.type:FUNC
 * @tc.require:issuesIC0HM8
 */
HWTEST_F(RSPaintFilterCanvasTest, IsDarkColorModeTestTest, TestSize.Level1)
{
    ASSERT_NE(paintFilterCanvas_, nullptr);
    paintFilterCanvas_->SetDarkColorMode(false);
    EXPECT_EQ(paintFilterCanvas_->isDarkColorMode_, false);
    paintFilterCanvas_->SetDarkColorMode(true);
    EXPECT_EQ(paintFilterCanvas_->isDarkColorMode_, true);
}
} // namespace Rosen
} // namespace OHOS