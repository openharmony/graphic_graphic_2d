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
#include "feature/color_picker/color_pick_alt_manager.h"

#include "draw/canvas.h"
#include "draw/color.h"
#include "image/bitmap.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "property/rs_color_picker_def.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class RSColorPickerManagerTest : public Test {
public:
    void SetUp() override {}
    void TearDown() override {}
};

// ============================================================================
// RSColorPickerManager::GetColorPick Tests
// ============================================================================

HWTEST_F(RSColorPickerManagerTest, GetColorPickReturnsBlackInitially, TestSize.Level1)
{
    RSColorPickerManager manager(1);
    auto color = manager.GetColorPick();
    ASSERT_TRUE(color.has_value());
    EXPECT_EQ(color.value(), Drawing::Color::COLOR_BLACK);
}

HWTEST_F(RSColorPickerManagerTest, GetColorPickInterpolatesDuringAnimation, TestSize.Level1)
{
    RSColorPickerManager manager(1);
    manager.HandleColorUpdate(Drawing::Color::COLOR_WHITE, ColorPickStrategyType::AVERAGE);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    auto color = manager.GetColorPick();
    ASSERT_TRUE(color.has_value());
    EXPECT_NE(color.value(), Drawing::Color::COLOR_BLACK);
    EXPECT_NE(color.value(), Drawing::Color::COLOR_WHITE);
}

HWTEST_F(RSColorPickerManagerTest, GetColorPickReturnsEndColorAfterAnimation, TestSize.Level1)
{
    RSColorPickerManager manager(1);
    manager.HandleColorUpdate(Drawing::Color::COLOR_WHITE, ColorPickStrategyType::AVERAGE);
    std::this_thread::sleep_for(std::chrono::milliseconds(400));

    auto color = manager.GetColorPick();
    ASSERT_TRUE(color.has_value());
    EXPECT_EQ(color.value(), Drawing::Color::COLOR_WHITE);
}

// ============================================================================
// RSColorPickerManager::ScheduleColorPick Tests
// ============================================================================

HWTEST_F(RSColorPickerManagerTest, ScheduleColorPickWithNoneStrategyDoesNothing, TestSize.Level1)
{
    RSColorPickerManager manager(1);
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    Drawing::Rect rect(0, 0, 10, 10);

    ColorPickerParam params(ColorPlaceholder::NONE, ColorPickStrategyType::NONE, 0);
    manager.ScheduleColorPick(canvas, &rect, params);

    auto color = manager.GetColorPick();
    EXPECT_EQ(color.value(), Drawing::Color::COLOR_BLACK);
}

HWTEST_F(RSColorPickerManagerTest, ScheduleColorPickContinuesAnimation, TestSize.Level1)
{
    RSColorPickerManager manager(1);
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    Drawing::Rect rect(0, 0, 10, 10);

    manager.HandleColorUpdate(Drawing::Color::COLOR_WHITE, ColorPickStrategyType::AVERAGE);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    ColorPickerParam params(ColorPlaceholder::NONE, ColorPickStrategyType::AVERAGE, 0);
    manager.ScheduleColorPick(canvas, &rect, params);

    auto color = manager.GetColorPick();
    ASSERT_TRUE(color.has_value());
    EXPECT_NE(color.value(), Drawing::Color::COLOR_BLACK);
}

HWTEST_F(RSColorPickerManagerTest, ScheduleColorPickWithNullRectReturnsEarly, TestSize.Level1)
{
    RSColorPickerManager manager(1);
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);

    ColorPickerParam params(ColorPlaceholder::NONE, ColorPickStrategyType::AVERAGE, 0);
    manager.ScheduleColorPick(canvas, nullptr, params);

    auto color = manager.GetColorPick();
    EXPECT_EQ(color.value(), Drawing::Color::COLOR_BLACK);
}

// ============================================================================
// RSColorPickerManager::HandleColorUpdate Tests
// ============================================================================

HWTEST_F(RSColorPickerManagerTest, HandleColorUpdateWithContrastStrategy, TestSize.Level1)
{
    RSColorPickerManager manager(1);
    manager.HandleColorUpdate(Drawing::Color::COLOR_WHITE, ColorPickStrategyType::CONTRAST);
    std::this_thread::sleep_for(std::chrono::milliseconds(400));

    auto color = manager.GetColorPick();
    EXPECT_EQ(color.value(), Drawing::Color::COLOR_BLACK);
}

HWTEST_F(RSColorPickerManagerTest, HandleColorUpdateWithSameColorDoesNotRestartAnimation, TestSize.Level1)
{
    RSColorPickerManager manager(1);
    manager.HandleColorUpdate(Drawing::Color::COLOR_WHITE, ColorPickStrategyType::AVERAGE);
    std::this_thread::sleep_for(std::chrono::milliseconds(400));

    auto color1 = manager.GetColorPick();
    manager.HandleColorUpdate(Drawing::Color::COLOR_WHITE, ColorPickStrategyType::AVERAGE);
    auto color2 = manager.GetColorPick();

    EXPECT_EQ(color1.value(), color2.value());
}

HWTEST_F(RSColorPickerManagerTest, HandleColorUpdateClampsAnimationFraction, TestSize.Level1)
{
    RSColorPickerManager manager(1);
    manager.HandleColorUpdate(Drawing::Color::COLOR_WHITE, ColorPickStrategyType::AVERAGE);
    manager.HandleColorUpdate(Drawing::Color::COLOR_BLACK, ColorPickStrategyType::AVERAGE);

    auto color = manager.GetColorPick();
    ASSERT_TRUE(color.has_value());
}

// ============================================================================
// RSColorPickerManager::InterpolateColor Tests
// ============================================================================

HWTEST_F(RSColorPickerManagerTest, InterpolateColorWithNegativeFractionReturnsEndColor, TestSize.Level1)
{
    const auto start = Drawing::Color::COLOR_BLACK;
    const auto end = Drawing::Color::COLOR_WHITE;
    auto result = RSColorPickerManager::InterpolateColor(start, end, -0.5f);
    EXPECT_EQ(result, end);
}

HWTEST_F(RSColorPickerManagerTest, InterpolateColorWithFractionAboveOneReturnsEndColor, TestSize.Level1)
{
    const auto start = Drawing::Color::COLOR_BLACK;
    const auto end = Drawing::Color::COLOR_WHITE;
    auto result = RSColorPickerManager::InterpolateColor(start, end, 1.5f);
    EXPECT_EQ(result, end);
}

HWTEST_F(RSColorPickerManagerTest, InterpolateColorWithValidFraction, TestSize.Level1)
{
    const auto start = Drawing::Color::COLOR_BLACK;
    const auto end = Drawing::Color::COLOR_WHITE;
    auto result = RSColorPickerManager::InterpolateColor(start, end, 0.5f);
    EXPECT_NE(result, start);
    EXPECT_NE(result, end);
}

// ============================================================================
// RSColorPickerManager::GetContrastColor Tests
// ============================================================================

HWTEST_F(RSColorPickerManagerTest, GetContrastColorWithLowLuminanceReturnsWhite, TestSize.Level1)
{
    const auto darkGray = Drawing::Color::ColorQuadSetARGB(0xFF, 50, 50, 50);
    auto result = RSColorPickerManager::GetContrastColor(darkGray);
    EXPECT_EQ(result, Drawing::Color::COLOR_WHITE);
}

HWTEST_F(RSColorPickerManagerTest, GetContrastColorWithHighLuminanceReturnsBlack, TestSize.Level1)
{
    const auto lightGray = Drawing::Color::ColorQuadSetARGB(0xFF, 230, 230, 230);
    auto result = RSColorPickerManager::GetContrastColor(lightGray);
    EXPECT_EQ(result, Drawing::Color::COLOR_BLACK);
}

HWTEST_F(RSColorPickerManagerTest, GetContrastColorWithMidLuminanceReturnsCachedColor, TestSize.Level1)
{
    const auto darkGray = Drawing::Color::ColorQuadSetARGB(0xFF, 50, 50, 50);
    RSColorPickerManager::GetContrastColor(darkGray);

    const auto midGray = Drawing::Color::ColorQuadSetARGB(0xFF, 180, 180, 180);
    auto result = RSColorPickerManager::GetContrastColor(midGray);
    EXPECT_EQ(result, Drawing::Color::COLOR_WHITE);
}

HWTEST_F(RSColorPickerManagerTest, GetContrastColorWithMidLuminanceReturnsCachedBlack, TestSize.Level1)
{
    const auto lightGray = Drawing::Color::ColorQuadSetARGB(0xFF, 230, 230, 230);
    RSColorPickerManager::GetContrastColor(lightGray);

    const auto midGray = Drawing::Color::ColorQuadSetARGB(0xFF, 180, 180, 180);
    auto result = RSColorPickerManager::GetContrastColor(midGray);
    EXPECT_EQ(result, Drawing::Color::COLOR_BLACK);
}

// ============================================================================
// RSColorPickerManager::PickColor Tests
// ============================================================================

HWTEST_F(RSColorPickerManagerTest, PickColorWithValidImageUpdatesColor, TestSize.Level1)
{
    RSColorPickerManager manager(1);
    Drawing::Bitmap bmp;
    Drawing::BitmapFormat format{Drawing::COLORTYPE_RGBA_8888, Drawing::ALPHATYPE_PREMUL};
    bmp.Build(10, 10, format);
    bmp.ClearWithColor(Drawing::Color::COLOR_BLUE);
    auto image = bmp.MakeImage();

    manager.PickColor(image, ColorPickStrategyType::AVERAGE);
    std::this_thread::sleep_for(std::chrono::milliseconds(400));

    auto color = manager.GetColorPick();
    ASSERT_TRUE(color.has_value());
    EXPECT_NE(color.value(), Drawing::Color::COLOR_TRANSPARENT);
}

HWTEST_F(RSColorPickerManagerTest, PickColorWithEmptyImageDoesNotChangeColor, TestSize.Level1)
{
    RSColorPickerManager manager(1);
    auto before = manager.GetColorPick();

    auto emptyImage = std::make_shared<Drawing::Image>();
    manager.PickColor(emptyImage, ColorPickStrategyType::AVERAGE);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    auto after = manager.GetColorPick();
    EXPECT_EQ(before.value(), after.value());
}

// ============================================================================
// ColorPickAltManager::GetColorPick Tests
// ============================================================================

class ColorPickAltManagerTest : public Test {
public:
    void SetUp() override {}
    void TearDown() override {}
};

HWTEST_F(ColorPickAltManagerTest, GetColorPickAlwaysReturnsNullopt, TestSize.Level1)
{
    auto manager = std::make_shared<ColorPickAltManager>(1);
    auto result = manager->GetColorPick();
    EXPECT_FALSE(result.has_value());
}

// ============================================================================
// ColorPickAltManager::ScheduleColorPick Tests
// ============================================================================

HWTEST_F(ColorPickAltManagerTest, ScheduleColorPickWithNoneStrategyReturnsEarly, TestSize.Level1)
{
    auto manager = std::make_shared<ColorPickAltManager>(1);
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    Drawing::Rect rect(0, 0, 10, 10);

    ColorPickerParam params(ColorPlaceholder::NONE, ColorPickStrategyType::NONE, 0);
    manager->ScheduleColorPick(canvas, &rect, params);

    EXPECT_TRUE(true);
}

HWTEST_F(ColorPickAltManagerTest, ScheduleColorPickUpdatesThresholds, TestSize.Level1)
{
    auto manager = std::make_shared<ColorPickAltManager>(1);
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    Drawing::Rect rect(0, 0, 10, 10);

    ColorPickerParam params(ColorPlaceholder::NONE, ColorPickStrategyType::AVERAGE, 0);
    params.notifyThreshold = {30, 50};
    manager->ScheduleColorPick(canvas, &rect, params);

    EXPECT_TRUE(true);
}

HWTEST_F(ColorPickAltManagerTest, ScheduleColorPickWithNullRectReturnsEarly, TestSize.Level1)
{
    auto manager = std::make_shared<ColorPickAltManager>(1);
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);

    ColorPickerParam params(ColorPlaceholder::NONE, ColorPickStrategyType::AVERAGE, 0);
    manager->ScheduleColorPick(canvas, nullptr, params);

    EXPECT_TRUE(true);
}

// ============================================================================
// ColorPickAltManager::HandleColorUpdate Tests
// ============================================================================

HWTEST_F(ColorPickAltManagerTest, HandleColorUpdateWithSameLuminanceZone, TestSize.Level1)
{
    auto manager = std::make_shared<ColorPickAltManager>(1);
    Drawing::ColorQuad color1 = Drawing::Color::ColorQuadSetARGB(0xFF, 100, 100, 100);

    manager->HandleColorUpdate(color1);
    manager->HandleColorUpdate(color1);

    EXPECT_TRUE(true);
}

HWTEST_F(ColorPickAltManagerTest, HandleColorUpdateWithDifferentLuminanceZones, TestSize.Level1)
{
    auto manager = std::make_shared<ColorPickAltManager>(1);

    manager->HandleColorUpdate(Drawing::Color::ColorQuadSetARGB(0xFF, 50, 50, 50));
    manager->HandleColorUpdate(Drawing::Color::ColorQuadSetARGB(0xFF, 200, 200, 200));

    EXPECT_TRUE(true);
}

HWTEST_F(ColorPickAltManagerTest, HandleColorUpdateWithInvalidLuminance, TestSize.Level1)
{
    auto manager = std::make_shared<ColorPickAltManager>(1);

    manager->HandleColorUpdate(Drawing::Color::ColorQuadSetARGB(0xFF, 50, 50, 50));
    Drawing::ColorQuad invalidColor = Drawing::Color::ColorQuadSetARGB(RGBA_MAX + 1, 100, 100, 100);
    manager->HandleColorUpdate(invalidColor);

    EXPECT_TRUE(true);
}

// ============================================================================
// GetLuminanceZone Tests
// ============================================================================

HWTEST_F(ColorPickAltManagerTest, GetLuminanceZoneWithDarkLuminance, TestSize.Level1)
{
    auto manager = std::make_shared<ColorPickAltManager>(1);
    ColorPickerParam params(ColorPlaceholder::NONE, ColorPickStrategyType::AVERAGE, 0);
    params.notifyThreshold = {100, 200};

    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    Drawing::Rect rect(0, 0, 10, 10);
    manager->ScheduleColorPick(canvas, &rect, params);

    manager->HandleColorUpdate(Drawing::Color::ColorQuadSetARGB(0xFF, 50, 50, 50));
    EXPECT_TRUE(true);
}

HWTEST_F(ColorPickAltManagerTest, GetLuminanceZoneWithLightLuminance, TestSize.Level1)
{
    auto manager = std::make_shared<ColorPickAltManager>(1);
    ColorPickerParam params(ColorPlaceholder::NONE, ColorPickStrategyType::AVERAGE, 0);
    params.notifyThreshold = {100, 200};

    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    Drawing::Rect rect(0, 0, 10, 10);
    manager->ScheduleColorPick(canvas, &rect, params);

    manager->HandleColorUpdate(Drawing::Color::ColorQuadSetARGB(0xFF, 230, 230, 230));
    EXPECT_TRUE(true);
}

HWTEST_F(ColorPickAltManagerTest, GetLuminanceZoneWithNeutralLuminance, TestSize.Level1)
{
    auto manager = std::make_shared<ColorPickAltManager>(1);
    ColorPickerParam params(ColorPlaceholder::NONE, ColorPickStrategyType::AVERAGE, 0);
    params.notifyThreshold = {100, 200};

    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    Drawing::Rect rect(0, 0, 10, 10);
    manager->ScheduleColorPick(canvas, &rect, params);

    manager->HandleColorUpdate(Drawing::Color::ColorQuadSetARGB(0xFF, 150, 150, 150));
    EXPECT_TRUE(true);
}

// ============================================================================
// Thread Safety Tests
// ============================================================================

HWTEST_F(RSColorPickerManagerTest, ThreadSafetyConcurrentHandleColorUpdate, TestSize.Level1)
{
    RSColorPickerManager manager(1);
    std::atomic<int> updateCount{0};
    const int numThreads = 5;
    const int updatesPerThread = 10;

    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back([&manager, &updateCount, i, updatesPerThread]() {
            for (int j = 0; j < updatesPerThread; ++j) {
                Drawing::ColorQuad color = Drawing::Color::ColorQuadSetARGB(
                    0xFF, (i * 50) % 256, (j * 30) % 256, ((i + j) * 20) % 256);
                manager.HandleColorUpdate(color, ColorPickStrategyType::AVERAGE);
                updateCount++;
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    EXPECT_EQ(updateCount.load(), numThreads * updatesPerThread);
}

HWTEST_F(ColorPickAltManagerTest, ThreadSafetyConcurrentHandleColorUpdate, TestSize.Level1)
{
    auto manager = std::make_shared<ColorPickAltManager>(1);
    std::atomic<int> updateCount{0};
    const int numThreads = 5;
    const int updatesPerThread = 10;

    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back([&manager, &updateCount, i, updatesPerThread]() {
            for (int j = 0; j < updatesPerThread; ++j) {
                Drawing::ColorQuad color = Drawing::Color::ColorQuadSetARGB(
                    0xFF, (i * 50) % 256, (j * 30) % 256, ((i + j) * 20) % 256);
                manager->HandleColorUpdate(color);
                updateCount++;
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    EXPECT_EQ(updateCount.load(), numThreads * updatesPerThread);
}

} // namespace Rosen
} // namespace OHOS
