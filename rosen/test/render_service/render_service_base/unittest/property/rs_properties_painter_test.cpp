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

#include <gtest/gtest.h>

#include "pipeline/rs_paint_filter_canvas.h"
#include "property/rs_properties_painter.h"
#include "render/rs_skia_filter.h"

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
 * @tc.name: GetGravityMatrix001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, GetGravityMatrix001, TestSize.Level1)
{
    RectF rect;
    float w = 0.1;
    float h = 0.1;
    SkMatrix mat;
    RSPropertiesPainter::GetGravityMatrix(Gravity::TOP, rect, w, h, mat);
    RSPropertiesPainter::GetGravityMatrix(Gravity::BOTTOM, rect, w, h, mat);
    RSPropertiesPainter::GetGravityMatrix(Gravity::LEFT, rect, w, h, mat);
    RSPropertiesPainter::GetGravityMatrix(Gravity::RIGHT, rect, w, h, mat);
    RSPropertiesPainter::GetGravityMatrix(Gravity::TOP_RIGHT, rect, w, h, mat);
    RSPropertiesPainter::GetGravityMatrix(Gravity::BOTTOM_LEFT, rect, w, h, mat);
    RSPropertiesPainter::GetGravityMatrix(Gravity::BOTTOM_RIGHT, rect, w, h, mat);
    RSPropertiesPainter::GetGravityMatrix(Gravity::RESIZE_ASPECT, rect, w, h, mat);
    RSPropertiesPainter::GetGravityMatrix(Gravity::RESIZE_ASPECT_FILL, rect, w, h, mat);
    RSPropertiesPainter::GetGravityMatrix(Gravity::RESIZE, rect, w, h, mat);
    RSPropertiesPainter::GetGravityMatrix(Gravity::DEFAULT, rect, w, h, mat);
}

/**
 * @tc.name: GetShadowDirtyRect001
 * @tc.desc: test
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
 * @tc.desc: test
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
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, GetShadowDirtyRect003, TestSize.Level1)
{
    RectI dirtyShadow;
    RSProperties properties;
    std::shared_ptr<RSPath> rsPath = std::make_shared<RSPath>();
    SkPath skPath;
    rsPath->SetSkiaPath(skPath);
    properties.SetShadowPath(rsPath);
    properties.SetShadowRadius(10.f);
    RRect rrect;
    RSPropertiesPainter::GetShadowDirtyRect(dirtyShadow, properties, &rrect);
}

/**
 * @tc.name: GetShadowDirtyRect004
 * @tc.desc: test
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
 * @tc.desc: test
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
 * @tc.desc: test
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
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, DrawShadow001, TestSize.Level1)
{
    SkCanvas skCanvas;
    RSPaintFilterCanvas canvas(&skCanvas);
    RSProperties properties;
    RRect rrect;
    RSPropertiesPainter::DrawShadow(properties, canvas, &rrect);
}

/**
 * @tc.name: DrawShadow002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, DrawShadow002, TestSize.Level1)
{
    SkCanvas skCanvas;
    RSPaintFilterCanvas canvas(&skCanvas);
    canvas.SetCacheEnabled(true);
    RSProperties properties;
    properties.SetShadowRadius(10.f);
    RRect rrect;
    RSPropertiesPainter::DrawShadow(properties, canvas, &rrect);
}

/**
 * @tc.name: DrawShadow003
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, DrawShadow003, TestSize.Level1)
{
    SkCanvas skCanvas;
    RSPaintFilterCanvas canvas(&skCanvas);
    RSProperties properties;
    properties.SetShadowRadius(10.f);
    std::shared_ptr<RSPath> rsPath = std::make_shared<RSPath>();
    SkPath skPath;
    rsPath->SetSkiaPath(skPath);
    properties.SetShadowPath(rsPath);
    RRect rrect;
    RSPropertiesPainter::DrawShadow(properties, canvas, &rrect);
}

/**
 * @tc.name: DrawShadow004
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, DrawShadow004, TestSize.Level1)
{
    SkCanvas skCanvas;
    RSPaintFilterCanvas canvas(&skCanvas);
    RSProperties properties;
    properties.SetShadowRadius(10.f);
    std::shared_ptr<RSPath> rsPath = std::make_shared<RSPath>();
    properties.SetClipBounds(rsPath);
    RRect rrect;
    RSPropertiesPainter::DrawShadow(properties, canvas, &rrect);
}

/**
 * @tc.name: DrawShadow005
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, DrawShadow005, TestSize.Level1)
{
    SkCanvas skCanvas;
    RSPaintFilterCanvas canvas(&skCanvas);
    RSProperties properties;
    RSPropertiesPainter::DrawShadow(properties, canvas, nullptr);
    RectF rect;
    rect.SetAll(1.f, 1.f, 1.f, 1.f);
    RRect rrect(rect, 1.f, 1.f);
    RSPropertiesPainter::DrawShadow(properties, canvas, &rrect);
}

/**
 * @tc.name: DrawShadow006
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, DrawShadow006, TestSize.Level1)
{
    SkCanvas skCanvas;
    RSPaintFilterCanvas canvas(&skCanvas);
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
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, DrawShadow007, TestSize.Level1)
{
    SkCanvas skCanvas;
    RSPaintFilterCanvas canvas(&skCanvas);
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
 * @tc.name: DrawFilter001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, DrawFilter001, TestSize.Level1)
{
    SkCanvas skCanvas;
    RSPaintFilterCanvas canvas(&skCanvas);
    std::shared_ptr<RSFilter> rsFilter = RSFilter::CreateBlurFilter(1.f, 1.f);
    RSProperties properties;
    properties.SetBackgroundFilter(rsFilter);
    auto filter = std::static_pointer_cast<RSSkiaFilter>(properties.GetBackgroundFilter());
    auto skRectPtr = std::make_unique<SkRect>();
    skRectPtr->setXYWH(0, 0, 1.f, 1.f);
    RSPropertiesPainter::DrawFilter(properties, canvas, filter, skRectPtr, canvas.GetSurface());
}

/**
 * @tc.name: DrawFilter002
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSPropertiesPainterTest, DrawFilter002, TestSize.Level1)
{
    SkCanvas skCanvas;
    RSPaintFilterCanvas canvas(&skCanvas);
    std::shared_ptr<RSFilter> rsFilter = RSFilter::CreateBlurFilter(1.f, 1.f);
    RSProperties properties;
    properties.SetBackgroundFilter(rsFilter);
    auto filter = std::static_pointer_cast<RSSkiaFilter>(properties.GetBackgroundFilter());
    std::shared_ptr<RSPath> rsPath = std::make_shared<RSPath>();
    properties.SetClipBounds(rsPath);
    RSPropertiesPainter::DrawFilter(properties, canvas, filter, nullptr, canvas.GetSurface());
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
    SkCanvas skCanvas;
    RSPaintFilterCanvas canvas(&skCanvas);
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
    SkCanvas skCanvas;
    RSPaintFilterCanvas canvas(&skCanvas);
    int w;
    int h;
    auto cmds = std::make_shared<DrawCmdList>(w, h);
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
    SkCanvas skCanvas;
    RSPaintFilterCanvas canvas(&skCanvas);
    auto cmds = std::make_shared<DrawCmdList>(5, 5);
    RSPropertiesPainter::DrawFrame(properties, canvas, cmds);
    canvas.SetCacheEnabled(true);
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
    SkCanvas skCanvas;
    RSPropertiesPainter::DrawBorder(properties, skCanvas);
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
    SkCanvas skCanvas;
    RSPropertiesPainter::DrawForegroundColor(properties, skCanvas);
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
    SkCanvas skCanvas;
    RSPropertiesPainter::DrawForegroundColor(properties, skCanvas);
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
    SkCanvas skCanvas;
    RSPropertiesPainter::DrawForegroundColor(properties, skCanvas);
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
    SkCanvas skCanvas;
    SkISize size = SkISize::Make(w, h);
    SkRect maskBounds = SkRect::Make(size);
    RSPropertiesPainter::DrawMask(properties, skCanvas, maskBounds);
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
    SkCanvas skCanvas;
    SkISize size = SkISize::Make(w, h);
    SkRect maskBounds = SkRect::Make(size);
    RSPropertiesPainter::DrawMask(properties, skCanvas, maskBounds);
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
    SkCanvas skCanvas;
    SkISize size = SkISize::Make(w, h);
    SkRect maskBounds = SkRect::Make(size);
    RSPropertiesPainter::DrawMask(properties, skCanvas, maskBounds);
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
    SkCanvas skCanvas;
    SkISize size = SkISize::Make(w, h);
    SkRect maskBounds = SkRect::Make(size);
    RSPropertiesPainter::DrawMask(properties, skCanvas, maskBounds);
}
} // namespace Rosen
} // namespace OHOS