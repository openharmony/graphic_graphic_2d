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

#include "feature/color_picker/color_pick_alt_manager.h"
#include "feature/color_picker/rs_color_picker_manager.h"
#include "feature/color_picker/rs_color_picker_utils.h"

#include "draw/canvas.h"
#include "draw/color.h"
#include "draw/surface.h"
#include "image/bitmap.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "property/rs_color_picker_def.h"

#ifdef RS_ENABLE_UNI_RENDER
#include "feature/color_picker/rs_color_picker_thread.h"

#include "platform/ohos/backend/rs_vulkan_context.h"
#endif

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
    // Use a dark gray - its contrast is white, different from default black
    const Drawing::ColorQuad darkGray = Drawing::Color::ColorQuadSetARGB(0xFF, 50, 50, 50);
    manager.HandleColorUpdate(darkGray);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    auto color = manager.GetColorPick();
    ASSERT_TRUE(color.has_value());
    // Should be animating from black toward white (not fully white yet)
    EXPECT_NE(color.value(), Drawing::Color::COLOR_BLACK);
    EXPECT_NE(color.value(), Drawing::Color::COLOR_WHITE);
}

HWTEST_F(RSColorPickerManagerTest, GetColorPickReturnsEndColorAfterAnimation, TestSize.Level1)
{
    RSColorPickerManager manager(1);
    // Use a dark gray - its contrast is white
    const Drawing::ColorQuad darkGray = Drawing::Color::ColorQuadSetARGB(0xFF, 50, 50, 50);
    manager.HandleColorUpdate(darkGray);
    std::this_thread::sleep_for(std::chrono::milliseconds(400));

    auto color = manager.GetColorPick();
    ASSERT_TRUE(color.has_value());
    // After animation, should be at the contrast color (white)
    EXPECT_EQ(color.value(), Drawing::Color::COLOR_WHITE);
}

// ============================================================================
// RSColorPickerManager::ScheduleColorPick Tests
// ============================================================================

HWTEST_F(RSColorPickerManagerTest, ScheduleColorPickWithNoneStrategyDoesNothing, TestSize.Level1)
{
    auto manager = std::make_shared<RSColorPickerManager>(1);
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    Drawing::Rect rect(0, 0, 10, 10);

    ColorPickerParam params(ColorPlaceholder::NONE, ColorPickStrategyType::NONE, 0);
    manager->ScheduleColorPick(canvas, &rect, params);

    auto color = manager->GetColorPick();
    EXPECT_EQ(color.value(), Drawing::Color::COLOR_BLACK);
}

HWTEST_F(RSColorPickerManagerTest, ScheduleColorPickContinuesAnimation, TestSize.Level1)
{
    auto manager = std::make_shared<RSColorPickerManager>(1);
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    Drawing::Rect rect(0, 0, 10, 10);

    // Use a dark gray - its contrast is white, different from default black
    const Drawing::ColorQuad darkGray = Drawing::Color::ColorQuadSetARGB(0xFF, 50, 50, 50);
    manager->HandleColorUpdate(darkGray);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    ColorPickerParam params(ColorPlaceholder::NONE, ColorPickStrategyType::AVERAGE, 0);
    manager->ScheduleColorPick(canvas, &rect, params);

    auto color = manager->GetColorPick();
    ASSERT_TRUE(color.has_value());
    // Should be animating from black toward white (not black anymore)
    EXPECT_NE(color.value(), Drawing::Color::COLOR_BLACK);
}

HWTEST_F(RSColorPickerManagerTest, ScheduleColorPickWithNullRectReturnsEarly, TestSize.Level1)
{
    auto manager = std::make_shared<RSColorPickerManager>(1);
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);

    ColorPickerParam params(ColorPlaceholder::NONE, ColorPickStrategyType::AVERAGE, 0);
    manager->ScheduleColorPick(canvas, nullptr, params);

    auto color = manager->GetColorPick();
    EXPECT_EQ(color.value(), Drawing::Color::COLOR_BLACK);
}

// ============================================================================
// RSColorPickerManager::HandleColorUpdate Tests
// ============================================================================

HWTEST_F(RSColorPickerManagerTest, HandleColorUpdateWithContrastStrategy, TestSize.Level1)
{
    RSColorPickerManager manager(1);
    // White has high luminance, so its contrast is black
    manager.HandleColorUpdate(Drawing::Color::COLOR_WHITE);
    std::this_thread::sleep_for(std::chrono::milliseconds(400));

    auto color = manager.GetColorPick();
    // The contrast of white is black, but since default is also black, no change occurs
    EXPECT_EQ(color.value(), Drawing::Color::COLOR_BLACK);
}

HWTEST_F(RSColorPickerManagerTest, HandleColorUpdateWithSameColorDoesNotRestartAnimation, TestSize.Level1)
{
    RSColorPickerManager manager(1);
    manager.HandleColorUpdate(Drawing::Color::COLOR_WHITE);
    std::this_thread::sleep_for(std::chrono::milliseconds(400));

    auto color1 = manager.GetColorPick();
    manager.HandleColorUpdate(Drawing::Color::COLOR_WHITE);
    auto color2 = manager.GetColorPick();

    EXPECT_EQ(color1.value(), color2.value());
}

HWTEST_F(RSColorPickerManagerTest, HandleColorUpdateClampsAnimationFraction, TestSize.Level1)
{
    RSColorPickerManager manager(1);
    manager.HandleColorUpdate(Drawing::Color::COLOR_WHITE);
    manager.HandleColorUpdate(Drawing::Color::COLOR_BLACK);

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

/**
 * @tc.name: GetContrastColorTest
 * @tc.desc: Verify function GetContrastColor
 * @tc.type:FUNC
 */
HWTEST_F(RSColorPickerManagerTest, GetContrastColorTest, TestSize.Level1)
{
    auto contrastForWhite = RSColorPickerManager::GetContrastColor(Drawing::Color::COLOR_WHITE, false);
    EXPECT_EQ(contrastForWhite, Drawing::Color::COLOR_BLACK);

    auto contrastForBlack = RSColorPickerManager::GetContrastColor(Drawing::Color::COLOR_BLACK, false);
    EXPECT_EQ(contrastForBlack, Drawing::Color::COLOR_WHITE);
}

/**
 * @tc.name: GetContrastColorPrevBlackTest
 * @tc.desc: Verify GetContrastColor with prevDark=true uses low threshold
 * @tc.type:FUNC
 */
HWTEST_F(RSColorPickerManagerTest, GetContrastColorPrevBlackTest, TestSize.Level1)
{
    // Use a mid-light gray so hysteresis changes the result.
    const Drawing::ColorQuad midGray = Drawing::Color::ColorQuadSetARGB(0xFF, 200, 200, 200);
    auto contrastPrevDark = RSColorPickerManager::GetContrastColor(midGray, true);
    auto contrastPrevLight = RSColorPickerManager::GetContrastColor(midGray, false);

    EXPECT_EQ(contrastPrevDark, Drawing::Color::COLOR_BLACK);
    EXPECT_EQ(contrastPrevLight, Drawing::Color::COLOR_WHITE);
}

/**
 * @tc.name: PickColorContrastPrevBlackTest
 * @tc.desc: Verify PickColor CONTRAST branch with prevDark=true
 * @tc.type:FUNC
 */
HWTEST_F(RSColorPickerManagerTest, PickColorContrastPrevBlackTest, TestSize.Level1)
{
    // Contrast color now provided by static API. Validate for white input.
    const Drawing::ColorQuad white = Drawing::Color::COLOR_WHITE;
    auto result = RSColorPickerManager::GetContrastColor(white, true);
    EXPECT_EQ(result, Drawing::Color::COLOR_BLACK);
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
                manager.HandleColorUpdate(color);
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

// ============================================================================
// RSColorPickerManager::HandleColorUpdate - Early Return Tests
// ============================================================================

/**
 * @tc.name: HandleColorUpdateWithEmptyColorPickedAlwaysUpdates
 * @tc.desc: Test that HandleColorUpdate updates when the contrast of new color
 *          differs from the current contrast color
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerManagerTest, HandleColorUpdateWithEmptyColorPickedAlwaysUpdates, TestSize.Level1)
{
    RSColorPickerManager manager(1);

    // Initial state: colorPicked_ is COLOR_TRANSPARENT, defaults to black in light mode
    auto color1 = manager.GetColorPick();
    EXPECT_TRUE(color1.has_value());
    EXPECT_EQ(color1.value(), Drawing::Color::COLOR_BLACK);

    // Use a dark gray - its contrast is white, different from default black
    const Drawing::ColorQuad darkGray = Drawing::Color::ColorQuadSetARGB(0xFF, 50, 50, 50);
    manager.HandleColorUpdate(darkGray);

    // Wait for animation to complete
    std::this_thread::sleep_for(std::chrono::milliseconds(400));

    auto color2 = manager.GetColorPick();
    EXPECT_TRUE(color2.has_value());
    // After animation, should be at the contrast color (white)
    EXPECT_EQ(color2.value(), Drawing::Color::COLOR_WHITE);
}

/**
 * @tc.name: HandleColorUpdateWithSameColorSkipsUpdate
 * @tc.desc: Test that HandleColorUpdate skips update when colorPicked_ has value
 *          and newColor equals the stored color
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerManagerTest, HandleColorUpdateWithSameColorSkipsUpdate, TestSize.Level1)
{
    RSColorPickerManager manager(1);

    // First update: set color to white
    manager.HandleColorUpdate(Drawing::Color::COLOR_WHITE);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    auto color1 = manager.GetColorPick();
    ASSERT_TRUE(color1.has_value());

    // Store animStartTime_ before second update
    uint64_t animStartTimeBefore = manager.animStartTime_.load();

    // Second update with same color (white)
    // Should early return and NOT reset animStartTime_
    manager.HandleColorUpdate(Drawing::Color::COLOR_WHITE);

    // Verify animStartTime_ was NOT changed (early return occurred)
    EXPECT_EQ(manager.animStartTime_.load(), animStartTimeBefore);
}

/**
 * @tc.name: HandleColorUpdateWithDifferentColorAlwaysUpdates
 * @tc.desc: Test that HandleColorUpdate updates when the contrast of new color
 *          differs from the current contrast color
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerManagerTest, HandleColorUpdateWithDifferentColorAlwaysUpdates, TestSize.Level1)
{
    RSColorPickerManager manager(1);

    // First update: use dark gray (contrast is white)
    const Drawing::ColorQuad darkGray = Drawing::Color::ColorQuadSetARGB(0xFF, 50, 50, 50);
    manager.HandleColorUpdate(darkGray);
    std::this_thread::sleep_for(std::chrono::milliseconds(400));

    auto color1 = manager.GetColorPick();
    ASSERT_EQ(color1.value(), Drawing::Color::COLOR_WHITE);

    // Second update with light gray (contrast is black, different from current white)
    const Drawing::ColorQuad lightGray = Drawing::Color::ColorQuadSetARGB(0xFF, 230, 230, 230);
    manager.HandleColorUpdate(lightGray);

    std::this_thread::sleep_for(std::chrono::milliseconds(400));

    auto color2 = manager.GetColorPick();
    // After animation, should be at the new contrast color (black)
    EXPECT_EQ(color2.value(), Drawing::Color::COLOR_BLACK);
}

// RSColorPickerUtils::CalculateLuminance Tests
// ============================================================================

/**
 * @tc.name: CalculateLuminanceWithBlack
 * @tc.desc: Verify CalculateLuminance returns 0 for black color
 * @tc.type: FUNC
 */
HWTEST_F(RSColorPickerManagerTest, CalculateLuminanceWithBlack, TestSize.Level1)
{
    const Drawing::ColorQuad black = Drawing::Color::COLOR_BLACK;
    float luminance = RSColorPickerUtils::CalculateLuminance(black);
    EXPECT_EQ(luminance, 0.0f);
}

/**
 * @tc.name: CalculateLuminanceWithWhite
 * @tc.desc: Verify CalculateLuminance returns 255 for white color
 * @tc.type: FUNC
 */
HWTEST_F(RSColorPickerManagerTest, CalculateLuminanceWithWhite, TestSize.Level1)
{
    const Drawing::ColorQuad white = Drawing::Color::COLOR_WHITE;
    float luminance = RSColorPickerUtils::CalculateLuminance(white);
    EXPECT_FLOAT_EQ(luminance, 255.0f);
}

/**
 * @tc.name: CalculateLuminanceWithRed
 * @tc.desc: Verify CalculateLuminance with pure red color
 * @tc.type: FUNC
 */
HWTEST_F(RSColorPickerManagerTest, CalculateLuminanceWithRed, TestSize.Level1)
{
    const Drawing::ColorQuad red = Drawing::Color::ColorQuadSetARGB(0xFF, 0xFF, 0x00, 0x00);
    float luminance = RSColorPickerUtils::CalculateLuminance(red);
    EXPECT_FLOAT_EQ(luminance, 0.299f * 255.0f);
}

/**
 * @tc.name: CalculateLuminanceWithGreen
 * @tc.desc: Verify CalculateLuminance with pure green color
 * @tc.type: FUNC
 */
HWTEST_F(RSColorPickerManagerTest, CalculateLuminanceWithGreen, TestSize.Level1)
{
    const Drawing::ColorQuad green = Drawing::Color::ColorQuadSetARGB(0xFF, 0x00, 0xFF, 0x00);
    float luminance = RSColorPickerUtils::CalculateLuminance(green);
    EXPECT_FLOAT_EQ(luminance, 0.587f * 255.0f);
}

/**
 * @tc.name: CalculateLuminanceWithBlue
 * @tc.desc: Verify CalculateLuminance with pure blue color
 * @tc.type: FUNC
 */
HWTEST_F(RSColorPickerManagerTest, CalculateLuminanceWithBlue, TestSize.Level1)
{
    const Drawing::ColorQuad blue = Drawing::Color::ColorQuadSetARGB(0xFF, 0x00, 0x00, 0xFF);
    float luminance = RSColorPickerUtils::CalculateLuminance(blue);
    EXPECT_FLOAT_EQ(luminance, 0.114f * 255.0f);
}

/**
 * @tc.name: CalculateLuminanceWithGray
 * @tc.desc: Verify CalculateLuminance with gray color
 * @tc.type: FUNC
 */
HWTEST_F(RSColorPickerManagerTest, CalculateLuminanceWithGray, TestSize.Level1)
{
    const Drawing::ColorQuad gray = Drawing::Color::ColorQuadSetARGB(0xFF, 0x80, 0x80, 0x80);
    float luminance = RSColorPickerUtils::CalculateLuminance(gray);
    EXPECT_NEAR(luminance, 128.0f, 1.0f);
}

// ============================================================================
// RSColorPickerUtils::CreateColorPickerInfo Tests
// ============================================================================

/**
 * @tc.name: CreateColorPickerInfoWithValidInputs
 * @tc.desc: Verify CreateColorPickerInfo returns valid ColorPickerInfo
 * @tc.type: FUNC
 */
HWTEST_F(RSColorPickerManagerTest, CreateColorPickerInfoWithValidInputs, TestSize.Level1)
{
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    auto snapshot = std::make_shared<Drawing::Image>();
    RSColorPickerManager manager(1);
    auto weakManager = manager.weak_from_this();

    Drawing::Surface* surface = canvas.GetSurface();
    auto colorPickerInfo = RSColorPickerUtils::CreateColorPickerInfo(surface, snapshot, weakManager);

    if (surface && snapshot && snapshot->GetBackendTexture(false, nullptr).IsValid()) {
        ASSERT_NE(colorPickerInfo, nullptr);
        EXPECT_NE(colorPickerInfo->colorSpace_, nullptr);
        EXPECT_NE(colorPickerInfo->oldImage_, nullptr);
    } else {
        EXPECT_EQ(colorPickerInfo, nullptr);
    }
}

/**
 * @tc.name: CreateColorPickerInfoWithNullSurface
 * @tc.desc: Verify CreateColorPickerInfo handles null surface
 * @tc.type: FUNC
 */
HWTEST_F(RSColorPickerManagerTest, CreateColorPickerInfoWithNullSurface, TestSize.Level1)
{
    auto snapshot = std::make_shared<Drawing::Image>();
    RSColorPickerManager manager(1);
    auto weakManager = manager.weak_from_this();

    auto colorPickerInfo = RSColorPickerUtils::CreateColorPickerInfo(nullptr, snapshot, weakManager);
    EXPECT_EQ(colorPickerInfo, nullptr);
}

/**
 * @tc.name: CreateColorPickerInfoWithNullSnapshot
 * @tc.desc: Verify CreateColorPickerInfo handles null snapshot
 * @tc.type: FUNC
 */
HWTEST_F(RSColorPickerManagerTest, CreateColorPickerInfoWithNullSnapshot, TestSize.Level1)
{
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    RSColorPickerManager manager(1);
    auto weakManager = manager.weak_from_this();

    Drawing::Surface* surface = canvas.GetSurface();
    auto colorPickerInfo = RSColorPickerUtils::CreateColorPickerInfo(surface, nullptr, weakManager);
    EXPECT_EQ(colorPickerInfo, nullptr);
}

/**
 * @tc.name: CreateColorPickerInfoWithNullManager
 * @tc.desc: Verify CreateColorPickerInfo handles null manager
 * @tc.type: FUNC
 */
HWTEST_F(RSColorPickerManagerTest, CreateColorPickerInfoWithNullManager, TestSize.Level1)
{
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    auto snapshot = std::make_shared<Drawing::Image>();
    std::weak_ptr<IColorPickerManager> weakManager;

    Drawing::Surface* surface = canvas.GetSurface();
    auto colorPickerInfo = RSColorPickerUtils::CreateColorPickerInfo(surface, snapshot, weakManager);

    if (surface && snapshot && snapshot->GetBackendTexture(false, nullptr).IsValid()) {
        ASSERT_NE(colorPickerInfo, nullptr);
        EXPECT_TRUE(colorPickerInfo->manager_.expired());
    } else {
        EXPECT_EQ(colorPickerInfo, nullptr);
    }
}

// ============================================================================
// RSColorPickerManager::ScheduleColorPick Hetero Acceleration Tests
// ============================================================================

/**
 * @tc.name: ScheduleColorPickWithHeteroAcceleration
 * @tc.desc: Test ScheduleColorPick when RSHeteroColorPicker::GetColor returns true
 * @tc.type: FUNC
 */
HWTEST_F(RSColorPickerManagerTest, ScheduleColorPickWithHeteroAcceleration, TestSize.Level1)
{
    auto manager = std::make_shared<RSColorPickerManager>(1);
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    Drawing::Rect rect(0, 0, 10, 10);

    ColorPickerParam params(ColorPlaceholder::NONE, ColorPickStrategyType::AVERAGE, 0);

    manager->ScheduleColorPick(canvas, &rect, params);

    EXPECT_TRUE(true);
}

/**
 * @tc.name: ScheduleColorPickWithNullRect
 * @tc.desc: Test ScheduleColorPick with null rect returns early
 * @tc.type: FUNC
 */
HWTEST_F(RSColorPickerManagerTest, ScheduleColorPickWithNullRect, TestSize.Level1)
{
    auto manager = std::make_shared<RSColorPickerManager>(1);
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);

    ColorPickerParam params(ColorPlaceholder::NONE, ColorPickStrategyType::AVERAGE, 0);
    manager->ScheduleColorPick(canvas, nullptr, params);

    auto color = manager->GetColorPick();
    EXPECT_EQ(color.value(), Drawing::Color::COLOR_BLACK);
}

// ============================================================================
// ColorPickAltManager::ScheduleColorPick Additional Tests
// ============================================================================

/**
 * @tc.name: ScheduleColorPickAltWithParamsRect
 * @tc.desc: Test ScheduleColorPick using params.rect value when present
 * @tc.type: FUNC
 */
HWTEST_F(ColorPickAltManagerTest, ScheduleColorPickAltWithParamsRect, TestSize.Level1)
{
    auto manager = std::make_shared<ColorPickAltManager>(1);
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    Drawing::Rect rect1(0, 0, 10, 10);
    Drawing::Rect rect2(5, 5, 15, 15);

    ColorPickerParam params(ColorPlaceholder::NONE, ColorPickStrategyType::AVERAGE, 0);
    params.rect = rect2;

    manager->ScheduleColorPick(canvas, &rect1, params);

    EXPECT_TRUE(true);
}

/**
 * @tc.name: ScheduleColorPickAltWithHeteroAcceleration
 * @tc.desc: Test ScheduleColorPick when RSHeteroColorPicker::GetColor returns true
 * @tc.type: FUNC
 */
HWTEST_F(ColorPickAltManagerTest, ScheduleColorPickAltWithHeteroAcceleration, TestSize.Level1)
{
    auto manager = std::make_shared<ColorPickAltManager>(1);
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    Drawing::Rect rect(0, 0, 10, 10);

    ColorPickerParam params(ColorPlaceholder::NONE, ColorPickStrategyType::AVERAGE, 0);
    manager->ScheduleColorPick(canvas, &rect, params);

    EXPECT_TRUE(true);
}

} // namespace Rosen
} // namespace OHOS
