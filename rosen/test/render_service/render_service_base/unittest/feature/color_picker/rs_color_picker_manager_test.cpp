/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <atomic>
#include <chrono>
#include <future>
#include <gtest/gtest.h>
#include <thread>

#include "feature/color_picker/rs_color_picker_manager.h"

#include "draw/canvas.h"
#include "draw/color.h"
#include "image/bitmap.h"
#include "pipeline/rs_paint_filter_canvas.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSColorPickerManagerTest : public Test {
public:
    void SetUp() override {}
    void TearDown() override {}
};

/**
 * @tc.name: InitialColorIsTransparent
 * @tc.desc: GetColorPicked returns transparent when no updates and rect is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RSColorPickerManagerTest, InitialColorIsTransparent, TestSize.Level1)
{
    RSColorPickerManager manager;
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);

    Drawing::ColorQuad color = manager.GetColorPicked(canvas, nullptr, 0 /*nodeId*/, ColorPickStrategyType::AVERAGE, 0);
    EXPECT_EQ(color, Drawing::Color::COLOR_TRANSPARENT);
}

/**
 * @tc.name: AnimationCompletesToEndColor
 * @tc.desc: After HandleColorUpdate and waiting beyond duration, color reaches endColor
 * @tc.type: FUNC
 */
HWTEST_F(RSColorPickerManagerTest, AnimationCompletesToEndColor, TestSize.Level1)
{
    RSColorPickerManager manager;
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);

    // Update to white and wait > animation duration
    manager.HandleColorUpdate(Drawing::Color::COLOR_WHITE, 123 /*nodeId*/);
    std::this_thread::sleep_for(std::chrono::milliseconds(400)); // > 350ms

    Drawing::ColorQuad color = manager.GetColorPicked(canvas, nullptr, 123, ColorPickStrategyType::AVERAGE, 0);
    EXPECT_EQ(color, Drawing::Color::COLOR_WHITE);
}

/**
 * @tc.name: InProgressAnimationBlendsBetweenColors
 * @tc.desc: Immediately after update, color is between start and end (blend)
 * @tc.type: FUNC
 */
HWTEST_F(RSColorPickerManagerTest, InProgressAnimationBlendsBetweenColors, TestSize.Level1)
{
    RSColorPickerManager manager;
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);

    // Trigger update to white and sample during animation
    manager.HandleColorUpdate(Drawing::Color::COLOR_WHITE, 1 /*nodeId*/);
    std::this_thread::sleep_for(std::chrono::milliseconds(50)); // mid-animation (~1/3)

    Drawing::Rect rect(0, 0, 10, 10);
    auto color = manager.GetColorPicked(canvas, &rect, 1, ColorPickStrategyType::AVERAGE, 0);
    // Should be neither fully transparent nor fully white
    EXPECT_NE(color, Drawing::Color::COLOR_TRANSPARENT);
    EXPECT_NE(color, Drawing::Color::COLOR_WHITE);
}

/**
 * @tc.name: UnchangedUpdateDoesNotRestartAnimation
 * @tc.desc: Updating with same color does not start a new animation
 * @tc.type: FUNC
 */
HWTEST_F(RSColorPickerManagerTest, UnchangedUpdateDoesNotRestartAnimation, TestSize.Level1)
{
    RSColorPickerManager manager;
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);

    // First update to white and finish animation
    manager.HandleColorUpdate(Drawing::Color::COLOR_WHITE, 7 /*nodeId*/);
    std::this_thread::sleep_for(std::chrono::milliseconds(400)); // ensure completed (>350ms)
    // Color should be white now
    auto color1 = manager.GetColorPicked(canvas, nullptr, 7, ColorPickStrategyType::AVERAGE, 0);
    EXPECT_EQ(color1, Drawing::Color::COLOR_WHITE);

    // Update with the same color; should not restart animation
    manager.HandleColorUpdate(Drawing::Color::COLOR_WHITE, 7 /*nodeId*/);
    // Immediate query should still be white (no blending back from transparent)
    auto color2 = manager.GetColorPicked(canvas, nullptr, 7, ColorPickStrategyType::AVERAGE, 0);
    EXPECT_EQ(color2, Drawing::Color::COLOR_WHITE);
}

/**
 * @tc.name: InterpolateColorClampsOutOfRange
 * @tc.desc: InterpolateColor returns endColor when fraction is out-of-range
 * @tc.type: FUNC
 */
HWTEST_F(RSColorPickerManagerTest, InterpolateColorClampsOutOfRange, TestSize.Level1)
{
    RSColorPickerManager manager;
    const Drawing::ColorQuad start = Drawing::Color::COLOR_BLACK;
    const Drawing::ColorQuad end = Drawing::Color::COLOR_WHITE;

    // fraction < 0 -> endColor
    auto c1 = manager.InterpolateColor(start, end, -0.5f);
    EXPECT_EQ(c1, end);

    // fraction > 1 -> endColor
    auto c2 = manager.InterpolateColor(start, end, 1.5f);
    EXPECT_EQ(c2, end);
}

/**
 * @tc.name: CooldownReturnsCachedColor
 * @tc.desc: Subsequent calls within interval return cached color without sampling
 * @tc.type: FUNC
 */
HWTEST_F(RSColorPickerManagerTest, CooldownReturnsCachedColor, TestSize.Level1)
{
    RSColorPickerManager manager;
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);

    // First call establishes lastUpdateTime_
    auto first = manager.GetColorPicked(canvas, nullptr, 0, ColorPickStrategyType::AVERAGE, 100000 /*interval*/);
    // Immediate call should return same color due to cooldown
    auto second = manager.GetColorPicked(canvas, nullptr, 0, ColorPickStrategyType::AVERAGE, 100000 /*interval*/);
    EXPECT_EQ(first, second);
}

/**
 * @tc.name: SnapshotFailureReturnsCurrentColor
 * @tc.desc: When surface or snapshot is null, returns current color without crash
 * @tc.type: FUNC
 */
HWTEST_F(RSColorPickerManagerTest, SnapshotFailureReturnsCurrentColor, TestSize.Level1)
{
    RSColorPickerManager manager;
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);

    // Non-null rect triggers snapshot path; with canvas-based filter, surface is nullptr
    Drawing::Rect rect; // default rect; specifics not used when surface is null
    auto colorBefore = manager.GetColorPicked(canvas, nullptr, 0, ColorPickStrategyType::AVERAGE, 0);
    auto colorAfter = manager.GetColorPicked(canvas, &rect, 0, ColorPickStrategyType::AVERAGE, 0);
    EXPECT_EQ(colorBefore, colorAfter);
}

/**
 * @tc.name: RunColorPickTaskSuccessUpdatesColor
 * @tc.desc: Calling RunColorPickTask on a valid image updates picked color
 * @tc.type: FUNC
 */
HWTEST_F(RSColorPickerManagerTest, RunColorPickTaskSuccessUpdatesColor, TestSize.Level1)
{
    RSColorPickerManager manager;
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);

    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format { Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL };
    bmp.Build(10, 10, format);
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    auto image = bmp.MakeImage();
    ASSERT_NE(image, nullptr);

    manager.RunColorPickTask(image, 42 /*nodeId*/, ColorPickStrategyType::AVERAGE);
    std::this_thread::sleep_for(std::chrono::milliseconds(400));
    auto color = manager.GetColorPicked(canvas, nullptr, 42, ColorPickStrategyType::AVERAGE, 0);
    EXPECT_NE(color, Drawing::Color::COLOR_TRANSPARENT);
}

/**
 * @tc.name: RunColorPickTaskFailureKeepsColor
 * @tc.desc: Calling RunColorPickTask on an empty image does not change color
 * @tc.type: FUNC
 */
HWTEST_F(RSColorPickerManagerTest, RunColorPickTaskFailureKeepsColor, TestSize.Level1)
{
    RSColorPickerManager manager;
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);

    auto before = manager.GetColorPicked(canvas, nullptr, 77, ColorPickStrategyType::AVERAGE, 0);
    auto emptyImage = std::make_shared<Drawing::Image>();
    manager.RunColorPickTask(emptyImage, 77 /*nodeId*/, ColorPickStrategyType::AVERAGE);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    auto after = manager.GetColorPicked(canvas, nullptr, 77, ColorPickStrategyType::AVERAGE, 0);
    EXPECT_EQ(before, after);
}
} // namespace Rosen
} // namespace OHOS
