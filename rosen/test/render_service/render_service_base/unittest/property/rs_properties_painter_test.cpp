/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>

#include "pipeline/rs_paint_filter_canvas.h"
#include "property/rs_properties_painter.h"
#include "property/rs_point_light_manager.h"
#include "render/rs_foreground_effect_filter.h"
#include "render/rs_skia_filter.h"
#include "render/rs_shadow.h"
#include "pipeline/rs_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSPropertiesPainterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSPropertiesPainterTest::SetUpTestCase() {}
void RSPropertiesPainterTest::TearDownTestCase() {}
void RSPropertiesPainterTest::SetUp() {}
void RSPropertiesPainterTest::TearDown() {}

/**
 * @tc.name: Rect2DrawingRect001
 * @tc.desc: test results of Rect2DrawingRect
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, Rect2DrawingRect001, TestSize.Level1)
{
    RectF rect;
    RSPropertiesPainter::Rect2DrawingRect(rect);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: RRect2DrawingRRect001
 * @tc.desc: test results of RRect2DrawingRRect
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, RRect2DrawingRRect001, TestSize.Level1)
{
    RRect rrect;
    RSPropertiesPainter::RRect2DrawingRRect(rrect);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: GetGravityMatrix001
 * @tc.desc: test results of GetGravityMatrix
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, GetGravityMatrix001, TestSize.Level1)
{
    RectF rect;
    float w = 0.1;
    float h = 0.1;
    Drawing::Matrix mat;
    EXPECT_TRUE(RSPropertiesPainter::GetGravityMatrix(Gravity::CENTER, rect, w, h, mat));
    RSPropertiesPainter::GetGravityMatrix(Gravity::TOP, rect, w, h, mat);
    RSPropertiesPainter::GetGravityMatrix(Gravity::BOTTOM, rect, w, h, mat);
    RSPropertiesPainter::GetGravityMatrix(Gravity::LEFT, rect, w, h, mat);
    RSPropertiesPainter::GetGravityMatrix(Gravity::RIGHT, rect, w, h, mat);
    RSPropertiesPainter::GetGravityMatrix(Gravity::TOP_LEFT, rect, w, h, mat);
    RSPropertiesPainter::GetGravityMatrix(Gravity::TOP_RIGHT, rect, w, h, mat);
    RSPropertiesPainter::GetGravityMatrix(Gravity::BOTTOM_LEFT, rect, w, h, mat);
    RSPropertiesPainter::GetGravityMatrix(Gravity::BOTTOM_RIGHT, rect, w, h, mat);
    RSPropertiesPainter::GetGravityMatrix(Gravity::RESIZE, rect, w, h, mat);
    RSPropertiesPainter::GetGravityMatrix(Gravity::RESIZE_ASPECT, rect, w, h, mat);
    RSPropertiesPainter::GetGravityMatrix(Gravity::RESIZE_ASPECT_TOP_LEFT, rect, w, h, mat);
    RSPropertiesPainter::GetGravityMatrix(Gravity::RESIZE_ASPECT_BOTTOM_RIGHT, rect, w, h, mat);
    RSPropertiesPainter::GetGravityMatrix(Gravity::RESIZE_ASPECT_FILL, rect, w, h, mat);
    RSPropertiesPainter::GetGravityMatrix(Gravity::RESIZE_ASPECT_FILL_TOP_LEFT, rect, w, h, mat);
    RSPropertiesPainter::GetGravityMatrix(Gravity::RESIZE_ASPECT_FILL_BOTTOM_RIGHT, rect, w, h, mat);
    RSPropertiesPainter::GetGravityMatrix(Gravity::DEFAULT, rect, w, h, mat);
    Gravity gravity = static_cast<Gravity>(100);
    EXPECT_FALSE(RSPropertiesPainter::GetGravityMatrix(gravity, rect, w, h, mat));
}

/**
 * @tc.name: Clip001
 * @tc.desc: test results of Clip
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, Clip001, TestSize.Level1)
{
    Drawing::Canvas canvas;
    RectF rect;
    RSPropertiesPainter::Clip(canvas, rect, true);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: GetShadowDirtyRect001
 * @tc.desc: test results of GetShadowDirtyRect
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, GetShadowDirtyRect001, TestSize.Level1)
{
    RectI dirtyShadow;
    RSProperties properties;
    RRect rrect;
    RSPropertiesPainter::GetShadowDirtyRect(dirtyShadow, properties, &rrect);
}

/**
 * @tc.name: GetShadowDirtyRect002
 * @tc.desc: test results of GetShadowDirtyRect
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, GetShadowDirtyRect002, TestSize.Level1)
{
    RectI dirtyShadow;
    RSProperties properties;
    properties.SetShadowRadius(10.f);
    RRect rrect;
    RSPropertiesPainter::GetShadowDirtyRect(dirtyShadow, properties, &rrect);
}

/**
 * @tc.name: GetShadowDirtyRect003
 * @tc.desc: test results of GetShadowDirtyRect
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, GetShadowDirtyRect003, TestSize.Level1)
{
    RectI dirtyShadow;
    RSProperties properties;
    std::shared_ptr<RSPath> rsPath = std::make_shared<RSPath>();
    Drawing::Path path;
    rsPath->SetDrawingPath(path);
    properties.SetShadowPath(rsPath);
    properties.SetShadowRadius(10.f);
    RRect rrect;
    RSPropertiesPainter::GetShadowDirtyRect(dirtyShadow, properties, &rrect);
}

/**
 * @tc.name: GetShadowDirtyRect004
 * @tc.desc: test results of GetShadowDirtyRect
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, GetShadowDirtyRect004, TestSize.Level1)
{
    RectI dirtyShadow;
    RSProperties properties;
    properties.SetShadowRadius(10.f);
    std::shared_ptr<RSPath> rsPath = std::make_shared<RSPath>();
    properties.SetClipBounds(rsPath);
    RectF rect;
    rect.SetAll(1.f, 1.f, 1.f, 1.f);
    RRect rrect(rect, 1.f, 1.f);
    RSPropertiesPainter::GetShadowDirtyRect(dirtyShadow, properties, &rrect);
}

/**
 * @tc.name: GetShadowDirtyRect005
 * @tc.desc: test results of GetShadowDirtyRect
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, GetShadowDirtyRect005, TestSize.Level1)
{
    RectI dirtyShadow;
    RSProperties properties;
    properties.SetShadowElevation(0.f);
    RectF rect;
    Color color(10, 10, 10, 10);
    properties.SetShadowColor(color);
    rect.SetAll(1.f, 1.f, 1.f, 1.f);
    RRect rrect(rect, 1.f, 1.f);
    RSPropertiesPainter::GetShadowDirtyRect(dirtyShadow, properties, &rrect);
}

/**
 * @tc.name: GetShadowDirtyRect006
 * @tc.desc: test results of GetShadowDirtyRect
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, GetShadowDirtyRect006, TestSize.Level1)
{
    RectI dirtyShadow;
    RSProperties properties;
    properties.SetShadowElevation(1.f);
    Color color(10, 10, 10, 10);
    properties.SetShadowColor(color);
    RectF rect;
    rect.SetAll(1.f, 1.f, 1.f, 1.f);
    RRect rrect(rect, 1.f, 1.f);
    RSPropertiesPainter::GetShadowDirtyRect(dirtyShadow, properties, &rrect);
}

/**
 * @tc.name: DrawShadow001
 * @tc.desc: test results of DrawShadow
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, DrawShadow001, TestSize.Level1)
{
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    RSProperties properties;
    RRect rrect;
    RSPropertiesPainter::DrawShadow(properties, canvas, &rrect);
}

/**
 * @tc.name: DrawShadow002
 * @tc.desc: test results of DrawShadow
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, DrawShadow002, TestSize.Level1)
{
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    canvas.SetCacheType(RSPaintFilterCanvas::CacheType::ENABLED);
    RSProperties properties;
    properties.SetShadowRadius(10.f);
    RRect rrect;
    RSPropertiesPainter::DrawShadow(properties, canvas, &rrect);
}

/**
 * @tc.name: DrawShadow003
 * @tc.desc: test results of DrawShadow
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, DrawShadow003, TestSize.Level1)
{
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    RSProperties properties;
    properties.SetShadowRadius(10.f);
    std::shared_ptr<RSPath> rsPath = std::make_shared<RSPath>();
    Drawing::Path path;
    rsPath->SetDrawingPath(path);
    properties.SetShadowPath(rsPath);
    RRect rrect;
    RSPropertiesPainter::DrawShadow(properties, canvas, &rrect);
}

/**
 * @tc.name: DrawShadow004
 * @tc.desc: test results of DrawShadow
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, DrawShadow004, TestSize.Level1)
{
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    RSProperties properties;
    properties.SetShadowRadius(10.f);
    std::shared_ptr<RSPath> rsPath = std::make_shared<RSPath>();
    properties.SetClipBounds(rsPath);
    RRect rrect;
    RSPropertiesPainter::DrawShadow(properties, canvas, &rrect);
}

/**
 * @tc.name: DrawShadow005
 * @tc.desc: test results of DrawShadow
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, DrawShadow005, TestSize.Level1)
{
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    RSProperties properties;
    RSPropertiesPainter::DrawShadow(properties, canvas, nullptr);
    RectF rect;
    rect.SetAll(1.f, 1.f, 1.f, 1.f);
    RRect rrect(rect, 1.f, 1.f);
    RSPropertiesPainter::DrawShadow(properties, canvas, &rrect);
}

/**
 * @tc.name: DrawShadow006
 * @tc.desc: test results of DrawShadow
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, DrawShadow006, TestSize.Level1)
{
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    RSProperties properties;
    properties.SetShadowElevation(0.f);
    Color color(10, 10, 10, 10);
    properties.SetShadowColor(color);
    RectF rect;
    rect.SetAll(1.f, 1.f, 1.f, 1.f);
    RRect rrect(rect, 1.f, 1.f);
    RSPropertiesPainter::DrawShadow(properties, canvas, &rrect);
}

/**
 * @tc.name: DrawShadow007
 * @tc.desc: test results of DrawShadow
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, DrawShadow007, TestSize.Level1)
{
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    RSProperties properties;
    properties.SetShadowElevation(1.f);
    Color color(10, 10, 10, 10);
    properties.SetShadowColor(color);
    RectF rect;
    rect.SetAll(1.f, 1.f, 1.f, 1.f);
    RRect rrect(rect, 1.f, 1.f);
    RSPropertiesPainter::DrawShadow(properties, canvas, &rrect);
}

/**
 * @tc.name: DrawColorfulShadowInner001
 * @tc.desc: test results of DrawColorfulShadowInner
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, DrawColorfulShadowInner001, TestSize.Level1)
{
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    RSProperties properties;
    Drawing::Path path;
    RSPropertiesPainter::DrawColorfulShadowInner(properties, canvas, path);
}

/**
 * @tc.name: GetDarkColor001
 * @tc.desc: test results of GetDarkColor
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, GetDarkColor001, TestSize.Level1)
{
    RSColor color;
    color.SetGreen(200);
    RSPropertiesPainter::GetDarkColor(color);
}

/**
 * @tc.name: PickColor001
 * @tc.desc: test results of PickColor
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, PickColor001, TestSize.Level1)
{
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas drCanvas(&drawingCanvas);
    RSProperties properties;
    Drawing::Path drPath;
    Drawing::Matrix matrix;
    Drawing::RectI deviceClipBounds;
    RSColor color;
    RSPropertiesPainter::PickColor(properties, drCanvas, drPath, matrix, deviceClipBounds, color);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: DrawShadowInner001
 * @tc.desc: test results of DrawShadowInner
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, DrawShadowInner001, TestSize.Level1)
{
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas drCanvas(&drawingCanvas);
    RSProperties properties;
    Drawing::Path drPath;
    RSPropertiesPainter::DrawShadowInner(properties, drCanvas, drPath);
    EXPECT_TRUE(true);

    properties.SetShadowElevation(1.0f);
    RSPropertiesPainter::DrawShadowInner(properties, drCanvas, drPath);
    EXPECT_TRUE(true);

    properties.SetShadowElevation(0.f);
    RSPropertiesPainter::DrawShadowInner(properties, drCanvas, drPath);
    EXPECT_TRUE(true);

    properties.SetShadowColorStrategy(2);
    RSPropertiesPainter::DrawShadowInner(properties, drCanvas, drPath);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: MakeGreyAdjustmentEffect001
 * @tc.desc: test results of MakeGreyAdjustmentEffect
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, MakeGreyAdjustmentEffect001, TestSize.Level1)
{
    RSPropertiesPainter::MakeGreyAdjustmentEffect();
    EXPECT_TRUE(true);
}

/**
 * @tc.name: DrawGreyAdjustment001
 * @tc.desc: test results of DrawGreyAdjustment
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, DrawGreyAdjustment001, TestSize.Level1)
{
    Drawing::Canvas canvas;
    std::shared_ptr<Drawing::Image> image;
    Vector2f greyCoeff;
    RSPropertiesPainter::DrawGreyAdjustment(canvas, image, greyCoeff);
    EXPECT_TRUE(true);

    image = std::make_shared<Drawing::Image>();
    RSPropertiesPainter::DrawGreyAdjustment(canvas, image, greyCoeff);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: DrawFilter001
 * @tc.desc: test results of DrawFilter
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, DrawFilter001, TestSize.Level1)
{
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    std::shared_ptr<RSFilter> rsFilter = RSFilter::CreateBlurFilter(1.f, 1.f);
    RSProperties properties;
    properties.SetBackgroundFilter(rsFilter);
    RSPropertiesPainter::DrawFilter(
        properties, canvas, FilterType::BACKGROUND_FILTER, Drawing::Rect(0.f, 0.f, 1.f, 1.f));
}

/**
 * @tc.name: DrawFilter002
 * @tc.desc: test results of DrawFilter
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, DrawFilter002, TestSize.Level1)
{
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    std::shared_ptr<RSFilter> rsFilter = RSFilter::CreateBlurFilter(1.f, 1.f);
    RSProperties properties;
    properties.SetBackgroundFilter(rsFilter);
    std::shared_ptr<RSPath> rsPath = std::make_shared<RSPath>();
    properties.SetClipBounds(rsPath);
    RSPropertiesPainter::DrawFilter(properties, canvas, FilterType::BACKGROUND_FILTER);
}

/**
 * @tc.name: DrawBackgroundImageAsEffect001
 * @tc.desc: test results of DrawBackgroundImageAsEffect
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, DrawBackgroundImageAsEffect001, TestSize.Level1)
{
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas drCanvas(&drawingCanvas);
    RSProperties properties;
    RSPropertiesPainter::DrawBackgroundImageAsEffect(properties, drCanvas);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: DrawBackgroundEffect001
 * @tc.desc: test results of DrawBackgroundEffect
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, DrawBackgroundEffect001, TestSize.Level1)
{
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas drCanvas(&drawingCanvas);
    RSProperties properties;
    RSPropertiesPainter::DrawBackgroundEffect(properties, drCanvas);
    EXPECT_TRUE(true);

    auto backgroundFilter = std::make_shared<RSFilter>();
    properties.SetBackgroundFilter(backgroundFilter);
    RSPropertiesPainter::DrawBackgroundEffect(properties, drCanvas);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: ApplyBackgroundEffectFallback001
 * @tc.desc: test results of ApplyBackgroundEffectFallback
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, ApplyBackgroundEffectFallback001, TestSize.Level1)
{
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas drCanvas(&drawingCanvas);
    RSProperties properties;
    RSPropertiesPainter::ApplyBackgroundEffectFallback(properties, drCanvas);
    EXPECT_TRUE(true);

    auto share = std::make_shared<RSRenderNode>(1);
    std::weak_ptr<RSRenderNode> backref = share;
    RSPropertiesPainter::ApplyBackgroundEffectFallback(properties, drCanvas);
    EXPECT_TRUE(true);

    auto backgroundFilter = std::make_shared<RSFilter>();
    properties.SetBackgroundFilter(backgroundFilter);
    RSPropertiesPainter::ApplyBackgroundEffectFallback(properties, drCanvas);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: ClipVisibleCanvas001
 * @tc.desc: test results of ClipVisibleCanvas
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, ClipVisibleCanvas001, TestSize.Level1)
{
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas drCanvas(&drawingCanvas);
    RSProperties properties;
    Drawing::RectI rectI;
    rectI.SetRight(1);
    rectI.SetBottom(1);
    RSPropertiesPainter::ClipVisibleCanvas(properties, drCanvas);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: ApplyBackgroundEffect001
 * @tc.desc: test results of ApplyBackgroundEffect
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, ApplyBackgroundEffect001, TestSize.Level1)
{
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas drCanvas(&drawingCanvas);
    RSProperties properties;
    RSPropertiesPainter::ApplyBackgroundEffect(properties, drCanvas);
    EXPECT_TRUE(true);

    auto effectData = std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    drCanvas.SetEffectData(effectData);
    RSPropertiesPainter::ApplyBackgroundEffect(properties, drCanvas);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: DrawPixelStretch001
 * @tc.desc: test results of DrawPixelStretch
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, DrawPixelStretch001, TestSize.Level1)
{
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas drCanvas(&drawingCanvas);
    RSProperties properties;
    RSPropertiesPainter::DrawPixelStretch(properties, drCanvas);
    EXPECT_TRUE(true);

    Vector4f newSize = {1.f, 1.f, 1.f, 1.f};
    std::optional<Vector4f> stretchSize = newSize;
    RSPropertiesPainter::DrawPixelStretch(properties, drCanvas);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: CalcAverageColor001
 * @tc.desc: test results of CalcAverageColor
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, CalcAverageColor001, TestSize.Level1)
{
    auto imageSnapshot = std::make_shared<Drawing::Image>();
    RSPropertiesPainter::CalcAverageColor(imageSnapshot);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: GetAndResetBlurCnt001
 * @tc.desc: test results of GetAndResetBlurCnt
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, GetAndResetBlurCnt001, TestSize.Level1)
{
    int res = RSPropertiesPainter::GetAndResetBlurCnt();
    EXPECT_NE(res, 1);
}

/**
 * @tc.name: SetBgAntiAlias001
 * @tc.desc: test results of SetBgAntiAlias
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, SetBgAntiAlias001, TestSize.Level1)
{
    bool forceBgAntiAlias = true;
    RSPropertiesPainter::SetBgAntiAlias(forceBgAntiAlias);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: GetBgAntiAlias001
 * @tc.desc: test results of GetBgAntiAlias
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, GetBgAntiAlias001, TestSize.Level1)
{
    bool res = RSPropertiesPainter::GetBgAntiAlias();
    EXPECT_EQ(res, true);
}

/**
 * @tc.name: GetRRectForDrawingBorder001
 * @tc.desc: test results of GetRRectForDrawingBorder
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, GetRRectForDrawingBorder001, TestSize.Level1)
{
    RSProperties properties;
    std::shared_ptr<RSBorder> border;
    bool isOutline = true;
    RSPropertiesPainter::GetRRectForDrawingBorder(properties, border, isOutline);
    EXPECT_TRUE(true);

    border = std::make_shared<RSBorder>();
    RSPropertiesPainter::GetRRectForDrawingBorder(properties, border, isOutline);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: GetInnerRRectForDrawingBorder001
 * @tc.desc: test results of GetInnerRRectForDrawingBorder
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, GetInnerRRectForDrawingBorder001, TestSize.Level1)
{
    RSProperties properties;
    std::shared_ptr<RSBorder> border;
    bool isOutline = true;
    RSPropertiesPainter::GetInnerRRectForDrawingBorder(properties, border, isOutline);
    EXPECT_TRUE(true);

    border = std::make_shared<RSBorder>();
    RSPropertiesPainter::GetInnerRRectForDrawingBorder(properties, border, isOutline);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: GetPhongShaderBuilder001
 * @tc.desc: test results of GetPhongShaderBuilder
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, GetPhongShaderBuilder001, TestSize.Level1)
{
    RSPropertiesPainter::GetPhongShaderBuilder();
    EXPECT_TRUE(true);

    RSPropertiesPainter::GetPhongShaderBuilder();
    EXPECT_TRUE(true);
}

/**
 * @tc.name: DrawLight001
 * @tc.desc: test results of DrawLight
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, DrawLight001, TestSize.Level1)
{
    RSProperties properties;
    Drawing::Canvas canvas;
    properties.SetIlluminatedType(-1);
    RSPropertiesPainter::DrawLight(properties, canvas);
    EXPECT_TRUE(true);

    auto lightSourcePtr = std::make_shared<RSLightSource>();
    RSIlluminated rsIlluminated;
    Vector4f lightPos;
    rsIlluminated.AddLightSourcesAndPos(lightSourcePtr, lightPos);
    RSPropertiesPainter::DrawLight(properties, canvas);
    EXPECT_TRUE(true);

    RSPropertiesPainter::DrawLight(properties, canvas);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: DrawLightInner001
 * @tc.desc: test results of DrawLightInner
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, DrawLightInner001, TestSize.Level1)
{
    Drawing::Canvas canvas;
    RSProperties properties;
    properties.SetIlluminatedType(-1);
    auto lightBuilder = RSPropertiesPainter::GetPhongShaderBuilder();
    std::shared_ptr<RSLightSource> newLightSource = std::make_shared<RSLightSource>();
    std::vector<std::pair<std::shared_ptr<RSLightSource>, Vector4f>> lightSourcesAndPosVec;
    std::shared_ptr<RSObjAbsGeometry> geoPtr;
    auto instance = RSPointLightManager::Instance();
    auto lightPos = instance->CalculateLightPosForIlluminated(newLightSource, geoPtr);
    lightSourcesAndPosVec.push_back(std::make_pair(newLightSource, lightPos));
    RSPropertiesPainter::DrawLightInner(properties, canvas, lightBuilder, lightSourcesAndPosVec, geoPtr);
    EXPECT_TRUE(true);

    properties.SetIlluminatedBorderWidth(0.f);
    IlluminatedType illuminatedType = IlluminatedType::CONTENT;
    RSIlluminated rsIlluminated;
    rsIlluminated.SetIlluminatedType(illuminatedType);
    RSPropertiesPainter::DrawLightInner(properties, canvas, lightBuilder, lightSourcesAndPosVec, geoPtr);
    EXPECT_TRUE(true);

    illuminatedType = IlluminatedType::BORDER;
    rsIlluminated.SetIlluminatedType(illuminatedType);
    RSPropertiesPainter::DrawLightInner(properties, canvas, lightBuilder, lightSourcesAndPosVec, geoPtr);
    EXPECT_TRUE(true);

    illuminatedType = IlluminatedType::BORDER_CONTENT;
    rsIlluminated.SetIlluminatedType(illuminatedType);
    RSPropertiesPainter::DrawLightInner(properties, canvas, lightBuilder, lightSourcesAndPosVec, geoPtr);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: DrawBorderBase001
 * @tc.desc: test results of DrawBorderBase
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, DrawBorderBase001, TestSize.Level1)
{
    RSProperties properties;
    Drawing::Canvas canvas;
    auto border = std::make_shared<RSBorder>();
    bool isOutline = false;
    RSPropertiesPainter::DrawBorderBase(properties, canvas, border, isOutline);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: GetOutlineDirtyRect001
 * @tc.desc: test results of GetOutlineDirtyRect
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, GetOutlineDirtyRect001, TestSize.Level1)
{
    RSProperties properties;
    RectI dirtyOutline;
    bool isAbsCoordinate = true;
    RSPropertiesPainter::GetOutlineDirtyRect(dirtyOutline, properties, isAbsCoordinate);
    EXPECT_TRUE(true);

    Vector4f radius = {0, 0, 0, 0};
    properties.SetOutlineRadius(radius);
    RSPropertiesPainter::GetOutlineDirtyRect(dirtyOutline, properties, isAbsCoordinate);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: DrawOutline001
 * @tc.desc: test results of DrawOutline
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, DrawOutline001, TestSize.Level1)
{
    RSProperties properties;
    Drawing::Canvas canvas;
    Vector4f radius = {0, 0, 0, 0};
    properties.SetOutlineRadius(radius);
    RSPropertiesPainter::DrawOutline(properties, canvas);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: DrawSpherize001
 * @tc.desc: test results of DrawSpherize
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, DrawSpherize001, TestSize.Level1)
{
    RSProperties properties;
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas drCanvas(&drawingCanvas);
    std::shared_ptr<Drawing::Surface> spherizeSurface;
    RSPropertiesPainter::DrawSpherize(properties, drCanvas, spherizeSurface);
    EXPECT_TRUE(true);

    spherizeSurface = std::make_shared<Drawing::Surface>();
    RSPropertiesPainter::DrawSpherize(properties, drCanvas, spherizeSurface);
    EXPECT_TRUE(true);

    RSPropertiesPainter::DrawSpherize(properties, drCanvas, spherizeSurface);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: DrawColorFilter001
 * @tc.desc: test results of DrawColorFilter
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, DrawColorFilter001, TestSize.Level1)
{
    RSProperties properties;
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas drCanvas(&drawingCanvas);
    RSPropertiesPainter::DrawColorFilter(properties, drCanvas);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: DrawBinarizationShader001
 * @tc.desc: test results of DrawBinarizationShader
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, DrawBinarizationShader001, TestSize.Level1)
{
    RSProperties properties;
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas drCanvas(&drawingCanvas);
    RSPropertiesPainter::DrawBinarizationShader(properties, drCanvas);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: MakeBinarizationShader001
 * @tc.desc: test results of MakeBinarizationShader
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, MakeBinarizationShader001, TestSize.Level1)
{
    RSProperties properties;
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas drCanvas(&drawingCanvas);
    auto imageShader = std::make_shared<Drawing::ShaderEffect>();
    RSPropertiesPainter::MakeBinarizationShader(1.0f, 1.0f, 1.0f, 1.0f, imageShader);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: DrawLightUpEffect001
 * @tc.desc: test results of DrawLightUpEffect
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, DrawLightUpEffect001, TestSize.Level1)
{
    RSProperties properties;
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas drCanvas(&drawingCanvas);
    RSPropertiesPainter::DrawLightUpEffect(properties, drCanvas);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: MakeDynamicLightUpBlender001
 * @tc.desc: test results of MakeDynamicLightUpBlender
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, MakeDynamicLightUpBlender001, TestSize.Level1)
{
    RSPropertiesPainter::MakeDynamicLightUpBlender(1.0f, 1.0f);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: DrawParticle001
 * @tc.desc: test results of DrawParticle
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, DrawParticle001, TestSize.Level1)
{
    RSProperties properties;
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas drCanvas(&drawingCanvas);
    RSPropertiesPainter::DrawParticle(properties, drCanvas);
    EXPECT_TRUE(true);

    auto particleParams = std::make_shared<ParticleRenderParams>();
    std::shared_ptr<RSRenderParticle> newv = std::make_shared<RSRenderParticle>(particleParams);
    auto rect = std::make_shared<RectF>();
    properties.SetDrawRegion(rect);
    properties.particles_.renderParticleVector_.push_back(newv);
    newv->lifeTime_ = 1;
    RSPropertiesPainter::DrawParticle(properties, drCanvas);
    EXPECT_TRUE(true);

    Vector2f position = {1.0f, 1.0f};
    newv->SetPosition(position);
    RSPropertiesPainter::DrawParticle(properties, drCanvas);
    EXPECT_TRUE(true);

    newv->SetParticleType(ParticleType::IMAGES);
    RSPropertiesPainter::DrawParticle(properties, drCanvas);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: IsDangerousBlendMode001
 * @tc.desc: test results of IsDangerousBlendMode
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, IsDangerousBlendMode001, TestSize.Level1)
{
    RSPropertiesPainter::IsDangerousBlendMode(1, 0);
    EXPECT_TRUE(true);

    RSPropertiesPainter::IsDangerousBlendMode(1, 1);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: BeginBlendMode001
 * @tc.desc: test results of BeginBlendMode
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, BeginBlendMode001, TestSize.Level1)
{
    RSProperties properties;
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas drCanvas(&drawingCanvas);
    RSPropertiesPainter::BeginBlendMode(drCanvas, properties);
    EXPECT_TRUE(true);

    properties.SetColorBlendMode(2);
    RSPropertiesPainter::BeginBlendMode(drCanvas, properties);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: EndBlendMode001
 * @tc.desc: test results of EndBlendMode
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, EndBlendMode001, TestSize.Level1)
{
    RSProperties properties;
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas drCanvas(&drawingCanvas);
    RSPropertiesPainter::EndBlendMode(drCanvas, properties);
    EXPECT_TRUE(true);

    properties.SetColorBlendMode(2);
    drCanvas.AddBlendOffscreenLayer(true);
    RSPropertiesPainter::EndBlendMode(drCanvas, properties);
    EXPECT_TRUE(true);

    properties.SetColorBlendApplyType(1);
    drCanvas.AddBlendOffscreenLayer(true);
    RSPropertiesPainter::EndBlendMode(drCanvas, properties);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: DrawLinearGradientBlurFilter001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, DrawLinearGradientBlurFilter001, TestSize.Level1)
{
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);

    std::vector<std::pair<float, float>> fractionStops;
    fractionStops.push_back(std::make_pair(0.f, 0.f));
    fractionStops.push_back(std::make_pair(1.f, 1.f));
    std::shared_ptr<RSLinearGradientBlurPara> linearGradientBlurPara =
        std::make_shared<RSLinearGradientBlurPara>(16, fractionStops, GradientDirection::BOTTOM);
    RSProperties properties;
    properties.SetLinearGradientBlurPara(linearGradientBlurPara);

    RSPropertiesPainter::DrawFilter(properties, canvas, FilterType::FOREGROUND_FILTER);
}

/**
 * @tc.name: DrawBackground001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, DrawBackground001, TestSize.Level1)
{
    RSProperties properties;
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    RSPropertiesPainter::DrawBackground(properties, canvas);
}

/**
 * @tc.name: DrawFrame001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, DrawFrame001, TestSize.Level1)
{
    RSProperties properties;
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    int32_t w;
    int32_t h;
    auto cmds = std::make_shared<Drawing::DrawCmdList>(w, h);
    RSPropertiesPainter::DrawFrame(properties, canvas, cmds);
}

/**
 * @tc.name: DrawFrame002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, DrawFrame002, TestSize.Level1)
{
    RSProperties properties;
    properties.SetFrameGravity(Gravity::LEFT);
    Vector4f frame(10.f, 10.f, 10.f, 10.f);
    properties.SetFrame(frame);
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    auto cmds = std::make_shared<Drawing::DrawCmdList>(5, 5);
    RSPropertiesPainter::DrawFrame(properties, canvas, cmds);
    canvas.SetCacheType(RSPaintFilterCanvas::CacheType::ENABLED);
    RSPropertiesPainter::DrawFrame(properties, canvas, cmds);
}

/**
 * @tc.name: DrawBorder001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, DrawBorder001, TestSize.Level1)
{
    RSProperties properties;
    Drawing::Canvas drawingCanvas;
    RSPropertiesPainter::DrawBorder(properties, drawingCanvas);
}

/**
 * @tc.name: DrawForegroundColor001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, DrawForegroundColor001, TestSize.Level1)
{
    RSProperties properties;
    Drawing::Canvas drawingCanvas;
    RSPropertiesPainter::DrawForegroundColor(properties, drawingCanvas);
}

/**
 * @tc.name: DrawForegroundColor002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, DrawForegroundColor002, TestSize.Level1)
{
    RSProperties properties;
    Color color(10, 10, 10, 10);
    properties.SetForegroundColor(color);
    std::shared_ptr<RSPath> rsPath = std::make_shared<RSPath>();
    properties.SetClipBounds(rsPath);
    Drawing::Canvas drawingCanvas;
    RSPropertiesPainter::DrawForegroundColor(properties, drawingCanvas);
}

/**
 * @tc.name: DrawForegroundColor003
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, DrawForegroundColor003, TestSize.Level1)
{
    RSProperties properties;
    Color color(10, 10, 10, 10);
    properties.SetForegroundColor(color);
    properties.SetClipToBounds(true);
    Drawing::Canvas drawingCanvas;
    RSPropertiesPainter::DrawForegroundColor(properties, drawingCanvas);
}

/**
 * @tc.name: DrawMask001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, DrawMask001, TestSize.Level1)
{
    RSProperties properties;
    int32_t w = 1;
    int32_t h = 1;
    Drawing::Canvas drawingCanvas;
    Drawing::Rect maskBounds = Drawing::Rect(0, 0, w, h);
    RSPropertiesPainter::DrawMask(properties, drawingCanvas, maskBounds);
}

/**
 * @tc.name: DrawMask002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, DrawMask002, TestSize.Level1)
{
    RSProperties properties;
    auto mask = std::make_shared<RSMask>();
    mask->SetMaskType(MaskType::SVG);
    sk_sp<SkSVGDOM> svgDom;
    mask->SetSvgDom(svgDom);
    properties.SetMask(mask);
    int32_t w = 1;
    int32_t h = 1;
    Drawing::Canvas drawingCanvas;
    Drawing::Rect maskBounds = Drawing::Rect(0, 0, w, h);
    RSPropertiesPainter::DrawMask(properties, drawingCanvas, maskBounds);
}

/**
 * @tc.name: DrawMask003
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, DrawMask003, TestSize.Level1)
{
    RSProperties properties;
    auto mask = std::make_shared<RSMask>();
    mask->SetMaskType(MaskType::GRADIENT);
    properties.SetMask(mask);
    int32_t w = 1;
    int32_t h = 1;
    Drawing::Canvas drawingCanvas;
    Drawing::Rect maskBounds = Drawing::Rect(0, 0, w, h);
    RSPropertiesPainter::DrawMask(properties, drawingCanvas, maskBounds);
}

/**
 * @tc.name: DrawMask004
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, DrawMask004, TestSize.Level1)
{
    RSProperties properties;
    auto mask = std::make_shared<RSMask>();
    mask->SetMaskType(MaskType::PATH);
    properties.SetMask(mask);
    int32_t w = 1;
    int32_t h = 1;
    Drawing::Canvas drawingCanvas;
    Drawing::Rect maskBounds = Drawing::Rect(0, 0, w, h);
    RSPropertiesPainter::DrawMask(properties, drawingCanvas, maskBounds);
}

/**
 * @tc.name: DrawDynamicLightUp001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertiesPainterTest, DrawDynamicLightUp001, TestSize.Level1)
{
    RSProperties properties;
    properties.SetDynamicLightUpRate(0.5);
    properties.SetDynamicLightUpDegree(0.5);
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    RSPropertiesPainter::DrawDynamicLightUp(properties, canvas);
}

/**
 * @tc.name: DrawDynamicLightUp002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertiesPainterTest, DrawDynamicLightUp002, TestSize.Level1)
{
    RSProperties properties;
    properties.SetDynamicLightUpRate(0.3);
    properties.SetDynamicLightUpDegree(0.8);
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    RSPropertiesPainter::DrawDynamicLightUp(properties, canvas);
}

/**
 * @tc.name: DrawForegroundFilter001
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertiesPainterTest, DrawForegroundFilter001, TestSize.Level1)
{
    float blurRadius = 10.f; // foreground effect blur radius
    auto foregroundEffectFilter = std::make_shared<RSForegroundEffectFilter>(blurRadius);
    RSProperties properties;
    properties.SetForegroundFilter(foregroundEffectFilter);

    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);

    RSPropertiesPainter::DrawForegroundFilter(properties, canvas);
}

/**
 * @tc.name: DrawForegroundFilter002
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertiesPainterTest, DrawForegroundFilter002, TestSize.Level1)
{
    float blurRadius = 20.f; // foreground effect blur radius
    auto foregroundEffectFilter = std::make_shared<RSForegroundEffectFilter>(blurRadius);
    RSProperties properties;
    properties.SetForegroundFilter(foregroundEffectFilter);

    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);

    RSPropertiesPainter::DrawForegroundFilter(properties, canvas);
}
} // namespace Rosen
} // namespace OHOS