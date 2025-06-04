/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "common/rs_obj_abs_geometry.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "property/rs_point_light_manager.h"
#include "property/rs_properties_painter.h"
#include "render/rs_foreground_effect_filter.h"
#include "render/rs_shadow.h"
#include "render/rs_skia_filter.h"
#include "skia_adapter/skia_surface.h"
#include "skia_image.h"
#include "skia_image_info.h"
#include "skia_runtime_effect.h"
#include "render/rs_colorful_shadow_filter.h"

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
 * @tc.type: FUNC
 * @tc.require: issueI9W24N
 */
HWTEST_F(RSPropertiesPainterTest, GetGravityMatrix001, TestSize.Level1)
{
    RectF rect;
    float w = 0.1;
    float h = 0.1;
    Drawing::Matrix mat;
    EXPECT_FALSE(RSPropertiesPainter::GetGravityMatrix(Gravity::CENTER, rect, 0.f, 0.f, mat));
    EXPECT_TRUE(RSPropertiesPainter::GetGravityMatrix(Gravity::CENTER, rect, w, h, mat));
    EXPECT_TRUE(RSPropertiesPainter::GetGravityMatrix(Gravity::TOP, rect, w, h, mat));
    EXPECT_TRUE(RSPropertiesPainter::GetGravityMatrix(Gravity::BOTTOM, rect, w, h, mat));
    EXPECT_TRUE(RSPropertiesPainter::GetGravityMatrix(Gravity::LEFT, rect, w, h, mat));
    EXPECT_TRUE(RSPropertiesPainter::GetGravityMatrix(Gravity::RIGHT, rect, w, h, mat));
    EXPECT_FALSE(RSPropertiesPainter::GetGravityMatrix(Gravity::TOP_LEFT, rect, w, h, mat));
    EXPECT_TRUE(RSPropertiesPainter::GetGravityMatrix(Gravity::TOP_RIGHT, rect, w, h, mat));
    EXPECT_TRUE(RSPropertiesPainter::GetGravityMatrix(Gravity::BOTTOM_LEFT, rect, w, h, mat));
    EXPECT_TRUE(RSPropertiesPainter::GetGravityMatrix(Gravity::BOTTOM_RIGHT, rect, w, h, mat));
    EXPECT_TRUE(RSPropertiesPainter::GetGravityMatrix(Gravity::RESIZE, rect, w, h, mat));
    EXPECT_FALSE(RSPropertiesPainter::GetGravityMatrix(Gravity::RESIZE, rect, w, 0.f, mat));
    EXPECT_FALSE(RSPropertiesPainter::GetGravityMatrix(Gravity::RESIZE, rect, 0.f, 0.f, mat));
    EXPECT_FALSE(RSPropertiesPainter::GetGravityMatrix(Gravity::RESIZE_ASPECT, rect, w, 0.f, mat));
    EXPECT_FALSE(RSPropertiesPainter::GetGravityMatrix(Gravity::RESIZE_ASPECT, rect, 0.f, 0.f, mat));
    EXPECT_FALSE(RSPropertiesPainter::GetGravityMatrix(Gravity::RESIZE_ASPECT, rect, w, h, mat));
    rect.width_ = 0.2f;
    rect.height_ = 0.3f;
    EXPECT_TRUE(RSPropertiesPainter::GetGravityMatrix(Gravity::RESIZE_ASPECT, rect, w, h, mat));
}

/**
 * @tc.name: GetGravityMatrix002
 * @tc.desc: test results of GetGravityMatrix
 * @tc.type: FUNC
 * @tc.require: issueI9W24N
 */
HWTEST_F(RSPropertiesPainterTest, GetGravityMatrix002, TestSize.Level1)
{
    RectF rect;
    float w = 0.1;
    float h = 0.1;
    Drawing::Matrix mat;
    EXPECT_TRUE(RSPropertiesPainter::GetGravityMatrix(Gravity::RESIZE_ASPECT_TOP_LEFT, rect, w, h, mat));
    EXPECT_FALSE(RSPropertiesPainter::GetGravityMatrix(Gravity::RESIZE_ASPECT_TOP_LEFT, rect, w, 0.f, mat));
    EXPECT_FALSE(RSPropertiesPainter::GetGravityMatrix(Gravity::RESIZE_ASPECT_TOP_LEFT, rect, 0.f, 0.f, mat));
    rect.width_ = 0.f;
    rect.height_ = 0.f;
    EXPECT_FALSE(RSPropertiesPainter::GetGravityMatrix(Gravity::RESIZE_ASPECT_BOTTOM_RIGHT, rect, w, h, mat));
    EXPECT_FALSE(RSPropertiesPainter::GetGravityMatrix(Gravity::RESIZE_ASPECT_BOTTOM_RIGHT, rect, w, 0.f, mat));
    EXPECT_FALSE(RSPropertiesPainter::GetGravityMatrix(Gravity::RESIZE_ASPECT_BOTTOM_RIGHT, rect, 0.f, 0.f, mat));
    rect.width_ = 0.2f;
    rect.height_ = 0.3f;
    EXPECT_TRUE(RSPropertiesPainter::GetGravityMatrix(Gravity::RESIZE_ASPECT_BOTTOM_RIGHT, rect, w, h, mat));
    rect.width_ = 0.f;
    rect.height_ = 0.f;
    EXPECT_FALSE(RSPropertiesPainter::GetGravityMatrix(Gravity::RESIZE_ASPECT_FILL, rect, w, h, mat));
    EXPECT_FALSE(RSPropertiesPainter::GetGravityMatrix(Gravity::RESIZE_ASPECT_FILL, rect, w, 0.f, mat));
    EXPECT_FALSE(RSPropertiesPainter::GetGravityMatrix(Gravity::RESIZE_ASPECT_FILL, rect, 0.f, 0.f, mat));
    rect.width_ = 0.2f;
    rect.height_ = 0.3f;
    EXPECT_TRUE(RSPropertiesPainter::GetGravityMatrix(Gravity::RESIZE_ASPECT_FILL, rect, w, h, mat));
    EXPECT_TRUE(RSPropertiesPainter::GetGravityMatrix(Gravity::RESIZE_ASPECT_FILL_TOP_LEFT, rect, w, h, mat));
    EXPECT_FALSE(RSPropertiesPainter::GetGravityMatrix(Gravity::RESIZE_ASPECT_FILL_TOP_LEFT, rect, w, 0.f, mat));
    EXPECT_FALSE(RSPropertiesPainter::GetGravityMatrix(Gravity::RESIZE_ASPECT_FILL_TOP_LEFT, rect, 0.f, 0.f, mat));
    rect.width_ = 0.f;
    rect.height_ = 0.f;
    EXPECT_FALSE(RSPropertiesPainter::GetGravityMatrix(Gravity::RESIZE_ASPECT_FILL_BOTTOM_RIGHT, rect, w, h, mat));
    EXPECT_FALSE(RSPropertiesPainter::GetGravityMatrix(Gravity::RESIZE_ASPECT_FILL_BOTTOM_RIGHT, rect, w, 0.f, mat));
    EXPECT_FALSE(RSPropertiesPainter::GetGravityMatrix(Gravity::RESIZE_ASPECT_FILL_BOTTOM_RIGHT, rect, 0.f, 0.f, mat));
    rect.width_ = 0.2f;
    rect.height_ = 0.3f;
    EXPECT_TRUE(RSPropertiesPainter::GetGravityMatrix(Gravity::RESIZE_ASPECT_FILL_BOTTOM_RIGHT, rect, w, h, mat));
    RSPropertiesPainter::GetGravityMatrix(Gravity::DEFAULT, rect, w, h, mat);
    Gravity gravity = static_cast<Gravity>(100);
    EXPECT_FALSE(RSPropertiesPainter::GetGravityMatrix(gravity, rect, w, h, mat));
}

/**
 * @tc.name: GetScalingModeMatrix
 * @tc.desc: test results of GetScalingModeMatrix
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, GetScalingModeMatrix001, TestSize.Level1)
{
    constexpr float defaultBoundsWidth = 800.f;
    constexpr float defaultBoundsHeight = 600.f;
    RectF bounds(0.f, 0.f, defaultBoundsWidth, defaultBoundsHeight);
    float bufferWidth = 30.f;
    float bufferHeight = 40.f;
    Drawing::Matrix scalingModeMatrix;
    EXPECT_TRUE(RSPropertiesPainter::GetScalingModeMatrix(ScalingMode::SCALING_MODE_FREEZE, bounds,
        bufferWidth, bufferHeight, scalingModeMatrix));
    Drawing::Matrix freezeMatrix = Drawing::Matrix();
    ASSERT_EQ(scalingModeMatrix, freezeMatrix);
    EXPECT_TRUE(RSPropertiesPainter::GetScalingModeMatrix(ScalingMode::SCALING_MODE_SCALE_TO_WINDOW, bounds,
        bufferWidth, bufferHeight, scalingModeMatrix));
    Drawing::Matrix scaleWindowMatrix = Drawing::Matrix();
    ASSERT_EQ(scalingModeMatrix, scaleWindowMatrix);
    EXPECT_TRUE(RSPropertiesPainter::GetScalingModeMatrix(ScalingMode::SCALING_MODE_SCALE_CROP, bounds,
        bufferWidth, bufferHeight, scalingModeMatrix));
    Drawing::Matrix scaleCropMatrix = Drawing::Matrix();
    scaleCropMatrix.SetMatrix(26.666666f, 0.f, 0.f, 0.f, 26.666666f, -233.333328f, 0.f, 0.f, 1.f);
    ASSERT_EQ(scalingModeMatrix, scaleCropMatrix);
    EXPECT_TRUE(RSPropertiesPainter::GetScalingModeMatrix(ScalingMode::SCALING_MODE_NO_SCALE_CROP, bounds,
        bufferWidth, bufferHeight, scalingModeMatrix));
    Drawing::Matrix noScaleCropMatrix = Drawing::Matrix();
    noScaleCropMatrix.SetMatrix(1.f, 0.f, 385.f, 0.f, 1.f, 280.f, 0.f, 0.f, 1.f);
    ASSERT_EQ(scalingModeMatrix, noScaleCropMatrix);
    EXPECT_TRUE(RSPropertiesPainter::GetScalingModeMatrix(ScalingMode::SCALING_MODE_SCALE_FIT, bounds,
        bufferWidth, bufferHeight, scalingModeMatrix));
    Drawing::Matrix scaleFitMatrix = Drawing::Matrix();
    scaleFitMatrix.SetMatrix(15.f, 0.f, 174.999985f, 0.f, 15.f, 0.f, 0.f, 0.f, 1.f);
    ASSERT_EQ(scalingModeMatrix, scaleFitMatrix);
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
 * @tc.type: FUNC
 * @tc.require: issueI9RBVH
 */
HWTEST_F(RSPropertiesPainterTest, GetShadowDirtyRect001, TestSize.Level1)
{
    RectI dirtyShadow;
    RSProperties properties;
    RRect rrect;
    RSPropertiesPainter::GetShadowDirtyRect(dirtyShadow, properties, &rrect);
    EXPECT_TRUE(!properties.IsShadowValid());

    properties.shadow_ = std::make_optional<RSShadow>();
    properties.shadow_->elevation_ = 1.f;
    properties.shadow_->color_.alpha_ = 255;
    properties.shadow_->radius_ = 1.f;
    RSPropertiesPainter::GetShadowDirtyRect(dirtyShadow, properties, &rrect);
    EXPECT_TRUE(properties.IsShadowValid());

    RSPropertiesPainter::GetShadowDirtyRect(dirtyShadow, properties, nullptr);
    EXPECT_TRUE(properties.IsShadowValid());

    properties.clipPath_ = std::make_shared<RSPath>();
    RSPropertiesPainter::GetShadowDirtyRect(dirtyShadow, properties, &rrect);
    EXPECT_TRUE(properties.GetClipBounds());

    properties.shadow_->path_ = std::make_shared<RSPath>();
    properties.shadow_->path_->drPath_ = new Drawing::Path();
    RSPropertiesPainter::GetShadowDirtyRect(dirtyShadow, properties, &rrect);
    EXPECT_TRUE(properties.GetShadowPath());
    delete properties.shadow_->path_->drPath_;
    properties.shadow_->path_->drPath_ = nullptr;
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
    EXPECT_TRUE(properties.IsShadowValid());
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
    EXPECT_TRUE(properties.IsShadowValid());
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
    EXPECT_TRUE(properties.IsShadowValid());
    EXPECT_TRUE(properties.GetClipBounds());
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
    EXPECT_FALSE(properties.IsShadowValid());
    EXPECT_FALSE(properties.GetShadowPath());
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
    EXPECT_TRUE(properties.IsShadowValid());
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
    EXPECT_TRUE(!properties.IsShadowValid());
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
    EXPECT_TRUE(properties.IsShadowValid());
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
    EXPECT_TRUE(properties.GetShadowPath());
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
    EXPECT_NE(rsPath, nullptr);
    properties.SetClipBounds(rsPath);
    RRect rrect;
    RSPropertiesPainter::DrawShadow(properties, canvas, &rrect);
    EXPECT_TRUE(properties.GetClipBounds());
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
    EXPECT_FALSE(properties.IsShadowValid());
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
    EXPECT_FALSE(properties.IsShadowValid());
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
    EXPECT_TRUE(properties.IsShadowValid());
}

/**
 * @tc.name: DrawShadow008
 * @tc.desc: test results of DrawShadow
 * @tc.type: FUNC
 * @tc.require: issueI9RBVH
 */
HWTEST_F(RSPropertiesPainterTest, DrawShadow008, TestSize.Level1)
{
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    RSProperties properties;
    RRect rrect;
    RSPropertiesPainter::DrawShadow(properties, canvas, &rrect);
    EXPECT_TRUE(!properties.IsShadowValid());

    properties.shadow_ = std::make_optional<RSShadow>();
    properties.shadow_->elevation_ = 1.f;
    properties.shadow_->color_.alpha_ = 255;
    properties.shadow_->radius_ = 1.f;
    RSPropertiesPainter::DrawShadow(properties, canvas, &rrect);
    EXPECT_TRUE(properties.IsShadowValid());

    properties.shadow_->isFilled_ = true;
    RSPropertiesPainter::DrawShadow(properties, canvas, &rrect);
    EXPECT_TRUE(properties.GetShadowIsFilled());
    properties.shadow_->isFilled_ = false;

    RSPropertiesPainter::DrawShadow(properties, canvas, nullptr);
    EXPECT_TRUE(properties.IsShadowValid());

    properties.shadow_->isFilled_ = true;
    RSPropertiesPainter::DrawShadow(properties, canvas, nullptr);
    EXPECT_TRUE(properties.GetShadowIsFilled());
    properties.shadow_->isFilled_ = false;

    properties.clipPath_ = std::make_shared<RSPath>();
    RSPropertiesPainter::DrawShadow(properties, canvas, &rrect);
    EXPECT_TRUE(properties.GetClipBounds());

    properties.shadow_->isFilled_ = true;
    RSPropertiesPainter::DrawShadow(properties, canvas, &rrect);
    EXPECT_TRUE(properties.GetShadowIsFilled());

    properties.shadow_->path_ = std::make_shared<RSPath>();
    properties.shadow_->path_->drPath_ = new Drawing::Path();
    properties.shadow_->imageMask_ = true;
    RSPropertiesPainter::DrawShadow(properties, canvas, &rrect);
    EXPECT_TRUE(properties.GetShadowMask());
    delete properties.shadow_->path_->drPath_;
    properties.shadow_->path_->drPath_ = nullptr;
}

/**
 * @tc.name: GetForegroundEffectDirtyRect001
 * @tc.desc: test results of GetForegroundEffectDirtyRect
 * @tc.type: FUNC
 * @tc.require: issueI9W24N
 */
HWTEST_F(RSPropertiesPainterTest, GetForegroundEffectDirtyRect001, TestSize.Level1)
{
    RectI dirtyForegroundEffect;
    RSProperties properties;
    RSPropertiesPainter::GetForegroundEffectDirtyRect(dirtyForegroundEffect, properties);
    EXPECT_TRUE(!properties.GetForegroundFilter());

    properties.foregroundFilter_ = std::make_shared<RSFilter>();
    RSPropertiesPainter::GetForegroundEffectDirtyRect(dirtyForegroundEffect, properties);
    EXPECT_TRUE(properties.GetForegroundFilter());

    properties.foregroundFilter_->type_ = RSFilter::FOREGROUND_EFFECT;
    RSPropertiesPainter::GetForegroundEffectDirtyRect(dirtyForegroundEffect, properties);
    EXPECT_TRUE(properties.GetForegroundFilter());

    RSUniRenderJudgement::uniRenderEnabledType_ = UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL;
    RSPropertiesPainter::GetForegroundEffectDirtyRect(dirtyForegroundEffect, properties);
    EXPECT_TRUE(!properties.GetForegroundFilterCache());

    properties.foregroundFilterCache_ = std::make_shared<RSFilter>();
    RSPropertiesPainter::GetForegroundEffectDirtyRect(dirtyForegroundEffect, properties);
    EXPECT_TRUE(properties.GetForegroundFilterCache());

    properties.foregroundFilterCache_->type_ = RSFilter::FOREGROUND_EFFECT;
    RSPropertiesPainter::GetForegroundEffectDirtyRect(dirtyForegroundEffect, properties);
    EXPECT_TRUE(properties.GetForegroundFilterCache());
}

/**
 * @tc.name: GetForegroundEffectDirtyRect002
 * @tc.desc: test results of GetForegroundEffectDirtyRect
 * @tc.type: FUNC
 */
HWTEST_F(RSPropertiesPainterTest, GetForegroundEffectDirtyRect002, TestSize.Level1)
{
    RectI dirtyForegroundEffect;
    RSProperties properties;
    RSPropertiesPainter::GetForegroundEffectDirtyRect(dirtyForegroundEffect, properties);
    EXPECT_TRUE(dirtyForegroundEffect.IsEmpty());

    properties.foregroundFilterCache_ = std::make_shared<RSFilter>();
    properties.foregroundFilterCache_->type_ = RSFilter::COLORFUL_SHADOW;
    RSShadow shadow;
    RRect rrect({ 0.0f, 0.0f, 10.0f, 10.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });
    shadow.SetMask(true);
    shadow.SetRadius(1.0f);
    properties.shadow_ = shadow;
    properties.rrect_ = rrect;
    RSPropertiesPainter::GetForegroundEffectDirtyRect(dirtyForegroundEffect, properties);
    EXPECT_TRUE(dirtyForegroundEffect.IsEmpty());
}

/**
 * @tc.name: MakeDynamicDimShader001
 * @tc.desc: test results of MakeDynamicDimShader
 * @tc.type: FUNC
 * @tc.require: issueI9W24N
 */
HWTEST_F(RSPropertiesPainterTest, MakeDynamicDimShader001, TestSize.Level1)
{
    std::shared_ptr<Drawing::ShaderEffect> imageShader = std::make_shared<Drawing::ShaderEffect>();
    RSPropertiesPainter::MakeDynamicDimShader(1.0f, imageShader);
    EXPECT_TRUE(imageShader);

    RSPropertiesPainter::MakeDynamicDimShader(1.0f, imageShader);
    EXPECT_TRUE(imageShader);
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
    EXPECT_TRUE(!properties.IsShadowValid());
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
    ASSERT_EQ(color.GetBlue(), 0);
    ASSERT_EQ(color.GetGreen(), 174);
    ASSERT_EQ(color.GetRed(), 0);
    ASSERT_EQ(color.GetAlpha(), 0);
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
    RSPaintFilterCanvas canvas(&drawingCanvas);
    RSProperties properties;
    Drawing::Path path;
    RSPropertiesPainter::DrawShadowInner(properties, canvas, path);
    ASSERT_EQ(properties.GetShadowElevation(), 0.f);

    properties.SetShadowElevation(1.0f);
    RSPropertiesPainter::DrawShadowInner(properties, canvas, path);
    ASSERT_EQ(properties.GetShadowElevation(), 1.f);

    properties.SetShadowElevation(0.f);
    RSPropertiesPainter::DrawShadowInner(properties, canvas, path);

    properties.SetShadowColorStrategy(2);
    RSPropertiesPainter::DrawShadowInner(properties, canvas, path);
    ASSERT_EQ(properties.GetShadowColorStrategy(), 2);
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
 * @tc.name: DrawContentLight001
 * @tc.desc: test results of DrawContentLight
 * @tc.type: FUNC
 * @tc.require: issueI9RBVH
 */
HWTEST_F(RSPropertiesPainterTest, DrawContentLight001, TestSize.Level1)
{
    Drawing::Canvas canvas;
    RSProperties properties;
    auto lightBuilder = RSPropertiesPainter::GetPhongShaderBuilder();
    Drawing::Brush brush;
    std::array<float, MAX_LIGHT_SOURCES> lightIntensityArray = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f }; // for test
    RSPropertiesPainter::DrawContentLight(properties, canvas, lightBuilder, brush, lightIntensityArray);
    EXPECT_NE(lightBuilder, nullptr);
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
    properties.backgroundFilter_ = rsFilter;
    EXPECT_NE(properties.GetBackgroundFilter(), nullptr);
    properties.GenerateBackgroundFilter();
    RSPropertiesPainter::DrawFilter(
        properties, canvas, FilterType::BACKGROUND_FILTER, Drawing::Rect(0.f, 0.f, 1.f, 1.f));
    EXPECT_NE(rsFilter, nullptr);
    EXPECT_EQ(properties.GetBackgroundFilter(), nullptr);
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
    properties.backgroundFilter_ = rsFilter;
    properties.GenerateBackgroundFilter();
    std::shared_ptr<RSPath> rsPath = std::make_shared<RSPath>();
    properties.SetClipBounds(rsPath);
    RSPropertiesPainter::DrawFilter(properties, canvas, FilterType::BACKGROUND_FILTER);
    EXPECT_NE(properties.clipPath_, nullptr);
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
    RSPaintFilterCanvas canvas(&drawingCanvas);
    RSProperties properties;
    RSPropertiesPainter::DrawBackgroundImageAsEffect(properties, canvas);
    EXPECT_TRUE(canvas.GetSurface() == nullptr);
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
    RSPaintFilterCanvas canvas(&drawingCanvas);
    RSProperties properties;
    RSPropertiesPainter::DrawBackgroundEffect(properties, canvas);
    EXPECT_TRUE(true);

    auto backgroundFilter = std::make_shared<RSFilter>();
    properties.backgroundFilter_ = backgroundFilter;
    RSPropertiesPainter::DrawBackgroundEffect(properties, canvas);
    EXPECT_TRUE(backgroundFilter != nullptr);

    auto surfacePtr = std::make_shared<Drawing::Surface>();
    surfacePtr->impl_ = std::make_shared<Drawing::SkiaSurface>();
    canvas.surface_ = surfacePtr.get();
    RSPropertiesPainter::DrawBackgroundEffect(properties, canvas);
    EXPECT_TRUE(surfacePtr != nullptr);
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
    properties.backgroundFilter_ = backgroundFilter;
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
    EXPECT_TRUE(drCanvas.GetEffectData() == nullptr);

    auto effectData = std::make_shared<RSPaintFilterCanvas::CachedEffectData>();
    drCanvas.SetEffectData(effectData);
    RSPropertiesPainter::ApplyBackgroundEffect(properties, drCanvas);
    EXPECT_TRUE(effectData != nullptr);

    effectData->cachedImage_ = std::make_shared<Drawing::Image>();
    RSPropertiesPainter::ApplyBackgroundEffect(properties, drCanvas);
    EXPECT_TRUE(effectData->cachedImage_ != nullptr);
}

/**
 * @tc.name: GetPixelStretchDirtyRect001
 * @tc.desc: test results of GetPixelStretchDirtyRect
 * @tc.type: FUNC
 * @tc.require: issueI9RBVH
 */
HWTEST_F(RSPropertiesPainterTest, GetPixelStretchDirtyRect001, TestSize.Level1)
{
    RectI dirtyPixelStretch;
    RSProperties properties;
    RSPropertiesPainter::GetPixelStretchDirtyRect(dirtyPixelStretch, properties, true);
    EXPECT_TRUE(!properties.needFilter_);

    properties.pixelStretch_ = Vector4f { 1.f, 1.f, 1.f, 1.f };
    RSPropertiesPainter::GetPixelStretchDirtyRect(dirtyPixelStretch, properties, true);
    EXPECT_TRUE(!properties.needFilter_);
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
    RSPaintFilterCanvas canvas(&drawingCanvas);
    RSProperties properties;
    RSPropertiesPainter::DrawPixelStretch(properties, canvas);
    EXPECT_TRUE(!properties.needFilter_);

    properties.pixelStretch_ = Vector4f { 1.f, 1.f, 1.f, 1.f };
    RSPropertiesPainter::DrawPixelStretch(properties, canvas);
    EXPECT_TRUE(!properties.needFilter_);

    auto surfacePtr = std::make_shared<Drawing::Surface>();
    surfacePtr->impl_ = std::make_shared<Drawing::SkiaSurface>();
    canvas.surface_ = surfacePtr.get();
    RSPropertiesPainter::DrawPixelStretch(properties, canvas);
    EXPECT_TRUE(surfacePtr != nullptr);
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
    EXPECT_EQ(res, 2);
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
    EXPECT_TRUE(properties.illuminatedPtr_ != nullptr);

    auto lightSource = std::make_shared<RSLightSource>();
    Vector4f color(1.0f, 1.0f, 1.0f, 1.0f);
    properties.illuminatedPtr_->lightSourcesAndPosMap_.insert(std::make_pair(lightSource, color));
    auto lightSourcePtr = std::make_shared<RSLightSource>();
    RSPropertiesPainter::DrawLight(properties, canvas);
    EXPECT_TRUE(!properties.illuminatedPtr_->lightSourcesAndPosMap_.empty());

    properties.boundsGeo_->width_ = 1;
    properties.boundsGeo_->height_ = 1;
    RSPropertiesPainter::DrawLight(properties, canvas);
    EXPECT_TRUE(properties.illuminatedPtr_ != nullptr);

    for (size_t i = 0; i < 12; ++i) {
        properties.illuminatedPtr_->lightSourcesAndPosMap_.insert(
            std::make_pair(std::make_shared<RSLightSource>(), color));
    }
    RSPropertiesPainter::DrawLight(properties, canvas);
    EXPECT_TRUE(properties.illuminatedPtr_->lightSourcesAndPosMap_.size() >= 12);
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
    auto newLightSource = std::make_shared<RSLightSource>();
    std::vector<std::pair<std::shared_ptr<RSLightSource>, Vector4f>> lightSourcesAndPosVec;
    std::shared_ptr<RSObjAbsGeometry> geoPtr;
    Vector4f lightPos = {1.f, 1.f, 1.f, 1.f}; // for test
    lightSourcesAndPosVec.push_back(std::make_pair(newLightSource, lightPos));
    RSPropertiesPainter::DrawLightInner(properties, canvas, lightBuilder, lightSourcesAndPosVec);

    properties.illuminatedPtr_ = std::make_shared<RSIlluminated>();
    properties.illuminatedPtr_->illuminatedType_ = IlluminatedType::CONTENT;
    RSPropertiesPainter::DrawLightInner(properties, canvas, lightBuilder, lightSourcesAndPosVec);
    EXPECT_NE(properties.illuminatedPtr_, nullptr);

    properties.illuminatedPtr_->illuminatedType_ = IlluminatedType::BORDER;
    RSPropertiesPainter::DrawLightInner(properties, canvas, lightBuilder, lightSourcesAndPosVec);
    EXPECT_EQ(lightSourcesAndPosVec.empty(), false);

    properties.illuminatedPtr_->illuminatedType_ = IlluminatedType::BORDER_CONTENT;
    RSPropertiesPainter::DrawLightInner(properties, canvas, lightBuilder, lightSourcesAndPosVec);
    EXPECT_NE(lightSourcesAndPosVec.empty(), true);
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
    std::shared_ptr<RSBorder> border = nullptr;
    bool isOutline = true;
    RSPropertiesPainter::DrawBorderBase(properties, canvas, border, isOutline);
    EXPECT_TRUE(isOutline);

    border = std::make_shared<RSBorder>();
    Color color(1, 1, 1, 1);
    border->colors_.push_back(color);
    border->widths_.push_back(0.001f);
    BorderStyle style = BorderStyle::DASHED;
    border->styles_.push_back(style);
    RSPropertiesPainter::DrawBorderBase(properties, canvas, border, isOutline);
    EXPECT_TRUE(border != nullptr);

    border->radius_.SetValues(1.f, 1.f, 1.f, 1.f);
    RSPropertiesPainter::DrawBorderBase(properties, canvas, border, isOutline);
    EXPECT_TRUE(border != nullptr);

    border->styles_[0] = BorderStyle::SOLID;
    RSPropertiesPainter::DrawBorderBase(properties, canvas, border, isOutline);
    EXPECT_TRUE(border->styles_.size() == 1);

    border->styles_[0] = BorderStyle::DASHED;
    border->colors_.clear();
    RSPropertiesPainter::DrawBorderBase(properties, canvas, border, isOutline);
    EXPECT_TRUE(border->colors_.size() == 0);
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
    EXPECT_TRUE(properties.outline_ == nullptr);

    properties.outline_ = std::make_shared<RSBorder>();
    Color color(1, 1, 1, 1);
    properties.outline_->colors_.push_back(color);
    properties.outline_->widths_.push_back(0.001f);
    BorderStyle style = BorderStyle::DASHED;
    properties.outline_->styles_.push_back(style);
    RSPropertiesPainter::GetOutlineDirtyRect(dirtyOutline, properties, isAbsCoordinate);
    EXPECT_TRUE(properties.outline_ != nullptr);
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
    RSPropertiesPainter::DrawOutline(properties, canvas);
    EXPECT_TRUE(properties.outline_ == nullptr);

    properties.outline_ = std::make_shared<RSBorder>();
    Color color(1, 1, 1, 1);
    properties.outline_->colors_.push_back(color);
    properties.outline_->widths_.push_back(0.001f);
    BorderStyle style = BorderStyle::DASHED;
    properties.outline_->styles_.push_back(style);
    RSPropertiesPainter::DrawOutline(properties, canvas);
    EXPECT_TRUE(properties.outline_ != nullptr);
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
    RSPaintFilterCanvas canvas(&drawingCanvas);
    std::shared_ptr<Drawing::Surface> spherizeSurface;
    RSPropertiesPainter::DrawSpherize(properties, canvas, spherizeSurface);
    EXPECT_TRUE(spherizeSurface == nullptr);

    spherizeSurface = std::make_shared<Drawing::Surface>();
    RSPropertiesPainter::DrawSpherize(properties, canvas, spherizeSurface);
    EXPECT_TRUE(spherizeSurface != nullptr);
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
    RSPaintFilterCanvas canvas(&drawingCanvas);
    RSPropertiesPainter::DrawColorFilter(properties, canvas);
    EXPECT_TRUE(properties.colorFilter_ == nullptr);

    properties.colorFilter_ = std::make_shared<Drawing::ColorFilter>();
    RSPropertiesPainter::DrawColorFilter(properties, canvas);
    EXPECT_TRUE(properties.colorFilter_ != nullptr);

    auto surfacePtr = std::make_shared<Drawing::Surface>();
    surfacePtr->impl_ = std::make_shared<Drawing::SkiaSurface>();
    canvas.surface_ = surfacePtr.get();
    RSPropertiesPainter::DrawColorFilter(properties, canvas);
    EXPECT_TRUE(canvas.GetSurface() != nullptr);
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
    RSPaintFilterCanvas canvas(&drawingCanvas);
    RSPropertiesPainter::DrawBinarizationShader(properties, canvas);
    EXPECT_TRUE(canvas.GetSurface() == nullptr);

    auto surfacePtr = std::make_shared<Drawing::Surface>();
    surfacePtr->impl_ = std::make_shared<Drawing::SkiaSurface>();
    canvas.surface_ = surfacePtr.get();
    RSPropertiesPainter::DrawBinarizationShader(properties, canvas);
    EXPECT_TRUE(canvas.GetSurface() != nullptr);
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
    RSPaintFilterCanvas canvas(&drawingCanvas);
    RSPropertiesPainter::DrawLightUpEffect(properties, canvas);
    EXPECT_TRUE(canvas.GetSurface() == nullptr);

    auto surfacePtr = std::make_shared<Drawing::Surface>();
    surfacePtr->impl_ = std::make_shared<Drawing::SkiaSurface>();
    canvas.surface_ = surfacePtr.get();
    RSPropertiesPainter::DrawLightUpEffect(properties, canvas);
    EXPECT_TRUE(canvas.GetSurface() != nullptr);

    properties.clipPath_ = std::make_shared<RSPath>();
    RSPropertiesPainter::DrawLightUpEffect(properties, canvas);
    EXPECT_TRUE(properties.clipPath_ != nullptr);
}

/**
 * @tc.name: DynamicBrightnessBlenderTest001
 * @tc.desc: MakeDynamicBrightnessBlender MakeDynamicBrightnessBuilder test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, DynamicBrightnessBlenderTest001, TestSize.Level1)
{
    EXPECT_NE(RSPropertiesPainter::MakeDynamicBrightnessBuilder(), nullptr);
    RSDynamicBrightnessPara params;
    EXPECT_NE(RSPropertiesPainter::MakeDynamicBrightnessBlender(params), nullptr);
}

/**
 * @tc.name: DrawDynamicDim001
 * @tc.desc: test results of DrawDynamicDim
 * @tc.type: FUNC
 * @tc.require: issueI9RBVH
 */
HWTEST_F(RSPropertiesPainterTest, DrawDynamicDim001, TestSize.Level1)
{
    RSProperties properties;
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    RSPropertiesPainter::DrawDynamicDim(properties, canvas);
    EXPECT_TRUE(canvas.GetSurface() == nullptr);

    auto surfacePtr = std::make_shared<Drawing::Surface>();
    surfacePtr->impl_ = std::make_shared<Drawing::SkiaSurface>();
    canvas.surface_ = surfacePtr.get();
    RSPropertiesPainter::DrawDynamicDim(properties, canvas);
    EXPECT_TRUE(canvas.GetSurface() != nullptr);

    properties.clipPath_ = std::make_shared<RSPath>();
    RSPropertiesPainter::DrawDynamicDim(properties, canvas);
    EXPECT_TRUE(properties.clipPath_ != nullptr);
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
    EXPECT_TRUE(properties.isDrawn_);

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
    properties.decoration_ = std::make_optional<Decoration>();
    properties.decoration_->backgroundColor_ = Color(1, 1, 1, 1);
    RSPropertiesPainter::DrawBackground(properties, canvas, true, false);
    EXPECT_TRUE(!properties.contentDirty_);

    properties.decoration_->bgShader_ = std::make_shared<RSShader>();
    RSPropertiesPainter::DrawBackground(properties, canvas, true, false);
    EXPECT_TRUE(!properties.contentDirty_);

    properties.decoration_->bgImage_ = std::make_shared<RSImage>();
    RSPropertiesPainter::DrawBackground(properties, canvas, true, false);
    EXPECT_TRUE(!properties.contentDirty_);

    RRect rect;
    properties.clipRRect_ = rect;
    RSPropertiesPainter::DrawBackground(properties, canvas, true, false);
    EXPECT_TRUE(!properties.contentDirty_);

    properties.clipToBounds_ = true;
    RSPropertiesPainter::DrawBackground(properties, canvas, true, false);
    EXPECT_TRUE(!properties.contentDirty_);

    Vector4f vectorVal(1.f, 1.f, 1.f, 1.f);
    properties.cornerRadius_ = vectorVal;
    RSPropertiesPainter::DrawBackground(properties, canvas, true, false);
    EXPECT_TRUE(!properties.contentDirty_);

    properties.clipPath_ = std::make_shared<RSPath>();
    properties.clipPath_->drPath_ = new Drawing::Path();
    RSPropertiesPainter::DrawBackground(properties, canvas, true, false);
    EXPECT_TRUE(!properties.contentDirty_);
    delete properties.clipPath_->drPath_;
    properties.clipPath_->drPath_ = nullptr;
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
    std::shared_ptr<Drawing::DrawCmdList> cmds = nullptr;
    RSPropertiesPainter::DrawFrame(properties, canvas, cmds);
    EXPECT_TRUE(!properties.contentDirty_);

    int32_t w = 0;
    int32_t h = 0;
    cmds = std::make_shared<Drawing::DrawCmdList>(w, h);
    RSPropertiesPainter::DrawFrame(properties, canvas, cmds);
    EXPECT_TRUE(cmds != nullptr);
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
    EXPECT_TRUE(cmds != nullptr);

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
    Drawing::Canvas canvas;
    RSPropertiesPainter::DrawBorder(properties, canvas);
    EXPECT_TRUE(properties.border_ == nullptr);

    properties.border_ = std::make_shared<RSBorder>();
    Color color(1, 1, 1, 1);
    properties.border_->colors_.push_back(color);
    properties.border_->widths_.push_back(0.001f);
    BorderStyle style = BorderStyle::DASHED;
    properties.border_->styles_.push_back(style);
    RSPropertiesPainter::DrawBorder(properties, canvas);
    EXPECT_TRUE(properties.border_ != nullptr);
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
    EXPECT_TRUE(!properties.clipToBounds_);

    properties.decoration_ = std::make_optional<Decoration>();
    properties.decoration_->foregroundColor_ = Color(1, 1, 1, 1);
    RRect rect;
    properties.clipRRect_ = rect;
    RSPropertiesPainter::DrawForegroundColor(properties, drawingCanvas);
    EXPECT_TRUE(!properties.clipToBounds_);

    properties.clipToBounds_ = true;
    RSPropertiesPainter::DrawForegroundColor(properties, drawingCanvas);
    EXPECT_TRUE(properties.clipToBounds_);

    properties.clipPath_ = std::make_shared<RSPath>();
    RSPropertiesPainter::DrawForegroundColor(properties, drawingCanvas);
    EXPECT_TRUE(properties.clipPath_ != nullptr);
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
    EXPECT_TRUE(properties.clipPath_ != nullptr);
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
    EXPECT_TRUE(properties.clipToBounds_);
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
    Drawing::Canvas canvas;
    Drawing::Rect maskBounds = Drawing::Rect(0, 0, w, h);
    RSPropertiesPainter::DrawMask(properties, canvas, maskBounds);
    EXPECT_TRUE(properties.mask_ == nullptr);

    properties.mask_ = std::make_shared<RSMask>();
    properties.mask_->type_ = MaskType::PIXEL_MAP;
    properties.mask_->image_ = std::make_shared<Drawing::Image>();
    RSPropertiesPainter::DrawMask(properties, canvas, maskBounds);
    EXPECT_TRUE(properties.mask_ != nullptr);

    properties.mask_->type_ = MaskType::PATH;
    RSPropertiesPainter::DrawMask(properties, canvas, maskBounds);
    EXPECT_TRUE(properties.mask_ != nullptr);

    properties.mask_->type_ = MaskType::GRADIENT;
    RSPropertiesPainter::DrawMask(properties, canvas, maskBounds);
    EXPECT_TRUE(properties.mask_ != nullptr);

    properties.mask_->type_ = MaskType::SVG;
    RSPropertiesPainter::DrawMask(properties, canvas, maskBounds);
    EXPECT_TRUE(properties.mask_ != nullptr);

    properties.mask_->svgPicture_ = std::make_shared<Drawing::Picture>();
    RSPropertiesPainter::DrawMask(properties, canvas, maskBounds);
    EXPECT_TRUE(properties.mask_->svgPicture_ != nullptr);
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
    EXPECT_TRUE(properties.mask_ != nullptr);
    EXPECT_TRUE(properties.mask_->svgDom_ == nullptr);
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
    EXPECT_TRUE(properties.mask_ != nullptr);
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
    EXPECT_TRUE(properties.mask_ != nullptr);
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
    EXPECT_TRUE(canvas.GetSurface() == nullptr);

    auto surfacePtr = std::make_shared<Drawing::Surface>();
    surfacePtr->impl_ = std::make_shared<Drawing::SkiaSurface>();
    canvas.surface_ = surfacePtr.get();
    RSPropertiesPainter::DrawDynamicLightUp(properties, canvas);
    EXPECT_TRUE(canvas.GetSurface() != nullptr);
}

/**
 * @tc.name: DrawForegroundFilter003
 * @tc.desc: test results of DrawForegroundFilter
 * @tc.type: FUNC
 * @tc.require: issueI9RBVH
 */
HWTEST_F(RSPropertiesPainterTest, DrawForegroundFilter003, TestSize.Level1)
{
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    RSProperties properties;
    RSPropertiesPainter::DrawForegroundFilter(properties, canvas);
    EXPECT_TRUE(properties.foregroundFilter_ == nullptr);

    properties.foregroundFilter_ = std::make_shared<RSFilter>();
    RSPropertiesPainter::DrawForegroundFilter(properties, canvas);
    EXPECT_TRUE(properties.foregroundFilter_ != nullptr);

    auto surfacePtr = std::make_shared<Drawing::Surface>();
    surfacePtr->impl_ = std::make_shared<Drawing::SkiaSurface>();
    canvas.surface_ = surfacePtr.get();
    RSPropertiesPainter::DrawForegroundFilter(properties, canvas);
    EXPECT_TRUE(surfacePtr != nullptr);
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
    EXPECT_TRUE(canvas.GetSurface() == nullptr);
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
    EXPECT_TRUE(canvas.GetSurface() == nullptr);
    EXPECT_TRUE(properties.foregroundFilter_ != nullptr);
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
    EXPECT_TRUE(canvas.GetSurface() == nullptr);
    EXPECT_TRUE(properties.foregroundFilter_ != nullptr);
}

/**
 * @tc.name: GetDistortionEffectDirtyRect
 * @tc.desc:
 * @tc.type:FUNC
 */
HWTEST_F(RSPropertiesPainterTest, GetDistortionEffectDirtyRect, TestSize.Level1)
{
    RectI localDistortionEffectRect(0, 0, 0, 0);
    RSProperties properties;
    float width = 200.0f; // set width 200
    float height = 200.0f; // set height 200
    Vector4f bounds(0.0, 0.0, width, height);
    properties.SetBounds(bounds);

    // the distortionK is nullptr
    RSPropertiesPainter::GetDistortionEffectDirtyRect(localDistortionEffectRect, properties);
    EXPECT_FALSE(localDistortionEffectRect.width_ > static_cast<int>(width));

    // the distortionK < 0
    properties.SetDistortionK(-0.2f);
    RSPropertiesPainter::GetDistortionEffectDirtyRect(localDistortionEffectRect, properties);
    EXPECT_FALSE(localDistortionEffectRect.width_ > static_cast<int>(width));

    // the distortionK > 0
    properties.SetDistortionK(0.2f);
    RSPropertiesPainter::GetDistortionEffectDirtyRect(localDistortionEffectRect, properties);
    EXPECT_TRUE(localDistortionEffectRect.width_ > static_cast<int>(width));
}
} // namespace Rosen
} // namespace OHOS