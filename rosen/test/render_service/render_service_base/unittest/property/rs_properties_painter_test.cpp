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
} // namespace Rosen
} // namespace OHOS