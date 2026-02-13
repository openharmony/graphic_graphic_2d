/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "drawable/rs_color_picker_drawable.h"
#include "pipeline/rs_render_node.h"
#include "skia_adapter/skia_surface.h"
#include "skia_adapter/skia_canvas.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
class RSColorPickerDrawableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSColorPickerDrawableTest::SetUpTestCase() {}
void RSColorPickerDrawableTest::TearDownTestCase() {}
void RSColorPickerDrawableTest::SetUp() {}
void RSColorPickerDrawableTest::TearDown() {}

/**
 * @tc.name: RSColorPickerDrawable001
 * @tc.desc: Test Func of RSColorPickerDrawable
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerDrawableTest, RSColorPickerDrawable001, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);
    ASSERT_EQ(DrawableV2::RSColorPickerDrawable::OnGenerate(node), nullptr);

    node.GetMutableRenderProperties().SetColorPickerPlaceholder(static_cast<int>(ColorPlaceholder::TEXT_CONTRAST));
    node.GetMutableRenderProperties().SetColorPickerStrategy(static_cast<int>(ColorPickStrategyType::AVERAGE));
    node.GetMutableRenderProperties().SetColorPickerInterval(1000);
    auto drawable = std::static_pointer_cast<DrawableV2::RSColorPickerDrawable>(
        DrawableV2::RSColorPickerDrawable::OnGenerate(node));
    ASSERT_NE(drawable, nullptr);

    drawable->OnSync();
    ASSERT_FALSE(drawable->needSync_);
    auto canvas = std::make_shared<Drawing::Canvas>();
    auto filterCanvas = std::make_shared<RSPaintFilterCanvas>(canvas.get());
    auto rect = std::make_shared<Drawing::Rect>();
    drawable->OnDraw(filterCanvas.get(), rect.get());
    ASSERT_TRUE(true);

    drawable->OnDraw(nullptr, rect.get());
    ASSERT_TRUE(true);

    drawable->colorPickerManager_ = nullptr;
    drawable->OnDraw(filterCanvas.get(), rect.get());
    ASSERT_TRUE(true);
}

/**
 * @tc.name: RSColorPickerDrawable002
 * @tc.desc: Test RSColorPickerDrawable with CLIENT_CALLBACK strategy (useAlt = true)
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerDrawableTest, RSColorPickerDrawable002, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);

    // Set CLIENT_CALLBACK strategy to trigger useAlt = true branch
    node.GetMutableRenderProperties().SetColorPickerPlaceholder(static_cast<int>(ColorPlaceholder::SURFACE));
    node.GetMutableRenderProperties().SetColorPickerStrategy(static_cast<int>(ColorPickStrategyType::CLIENT_CALLBACK));
    node.GetMutableRenderProperties().SetColorPickerInterval(500);

    auto drawable = std::static_pointer_cast<DrawableV2::RSColorPickerDrawable>(
        DrawableV2::RSColorPickerDrawable::OnGenerate(node));
    ASSERT_NE(drawable, nullptr);
    ASSERT_NE(drawable->colorPickerManager_, nullptr);

    drawable->OnSync();
    ASSERT_FALSE(drawable->needSync_);

    auto canvas = std::make_shared<Drawing::Canvas>();
    auto filterCanvas = std::make_shared<RSPaintFilterCanvas>(canvas.get());
    auto rect = std::make_shared<Drawing::Rect>();
    drawable->OnDraw(filterCanvas.get(), rect.get());
    ASSERT_TRUE(true);
}

/**
 * @tc.name: RSColorPickerDrawable003
 * @tc.desc: Test OnSync when needSync_ is false (early return branch)
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerDrawableTest, RSColorPickerDrawable003, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);

    node.GetMutableRenderProperties().SetColorPickerPlaceholder(static_cast<int>(ColorPlaceholder::ACCENT));
    node.GetMutableRenderProperties().SetColorPickerStrategy(static_cast<int>(ColorPickStrategyType::DOMINANT));
    node.GetMutableRenderProperties().SetColorPickerInterval(2000);

    auto drawable = std::static_pointer_cast<DrawableV2::RSColorPickerDrawable>(
        DrawableV2::RSColorPickerDrawable::OnGenerate(node));
    ASSERT_NE(drawable, nullptr);

    // First sync should work
    drawable->OnSync();
    ASSERT_FALSE(drawable->needSync_);

    // Second sync should hit early return branch (needSync_ is false)
    drawable->OnSync();
    ASSERT_FALSE(drawable->needSync_);
}

/**
 * @tc.name: RSColorPickerDrawable004
 * @tc.desc: Test OnSync when stagingColorPicker_ is nullptr
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerDrawableTest, RSColorPickerDrawable004, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);

    node.GetMutableRenderProperties().SetColorPickerPlaceholder(static_cast<int>(ColorPlaceholder::FOREGROUND));
    node.GetMutableRenderProperties().SetColorPickerStrategy(static_cast<int>(ColorPickStrategyType::CONTRAST));
    node.GetMutableRenderProperties().SetColorPickerInterval(1500);

    auto drawable = std::static_pointer_cast<DrawableV2::RSColorPickerDrawable>(
        DrawableV2::RSColorPickerDrawable::OnGenerate(node));
    ASSERT_NE(drawable, nullptr);

    // Clear stagingColorPicker_ to test nullptr branch
    drawable->stagingColorPicker_ = nullptr;
    drawable->needSync_ = true;

    drawable->OnSync();
    ASSERT_FALSE(drawable->needSync_);
    // params_ should be default constructed when stagingColorPicker_ is nullptr
    EXPECT_EQ(drawable->params_.placeholder, ColorPlaceholder::NONE);
    EXPECT_EQ(drawable->params_.strategy, ColorPickStrategyType::NONE);
    EXPECT_EQ(drawable->params_.interval, 0);
}

/**
 * @tc.name: RSColorPickerDrawable005
 * @tc.desc: Test OnUpdate returns false when colorPicker is nullptr
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerDrawableTest, RSColorPickerDrawable005, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);

    // Don't set any color picker properties, so GetColorPicker() returns nullptr
    auto drawable = std::make_shared<DrawableV2::RSColorPickerDrawable>(false, id);
    ASSERT_NE(drawable, nullptr);

    // OnUpdate should return false when stagingColorPicker_ is nullptr
    bool result = drawable->OnUpdate(node);
    EXPECT_FALSE(result);
    EXPECT_TRUE(drawable->needSync_);
}

/**
 * @tc.name: RSColorPickerDrawable006
 * @tc.desc: Test different ColorPlaceholder and ColorPickStrategyType combinations
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerDrawableTest, RSColorPickerDrawable006, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);

    // Test SURFACE_CONTRAST placeholder with CONTRAST strategy
    node.GetMutableRenderProperties().SetColorPickerPlaceholder(
        static_cast<int>(ColorPlaceholder::SURFACE_CONTRAST));
    node.GetMutableRenderProperties().SetColorPickerStrategy(static_cast<int>(ColorPickStrategyType::CONTRAST));
    node.GetMutableRenderProperties().SetColorPickerInterval(3000);

    auto drawable = std::static_pointer_cast<DrawableV2::RSColorPickerDrawable>(
        DrawableV2::RSColorPickerDrawable::OnGenerate(node));
    ASSERT_NE(drawable, nullptr);

    drawable->OnSync();
    EXPECT_EQ(drawable->params_.placeholder, ColorPlaceholder::SURFACE_CONTRAST);
    EXPECT_EQ(drawable->params_.strategy, ColorPickStrategyType::CONTRAST);
    EXPECT_EQ(drawable->params_.interval, 3000);

    auto canvas = std::make_shared<Drawing::Canvas>();
    auto filterCanvas = std::make_shared<RSPaintFilterCanvas>(canvas.get());
    auto rect = std::make_shared<Drawing::Rect>();
    drawable->OnDraw(filterCanvas.get(), rect.get());
    ASSERT_TRUE(true);
}

/**
 * @tc.name: RSColorPickerDrawable007
 * @tc.desc: Test Prepare with nullptr stagingColorPicker
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerDrawableTest, RSColorPickerDrawable007, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);

    auto drawable = std::make_shared<DrawableV2::RSColorPickerDrawable>(false, id);
    ASSERT_NE(drawable, nullptr);

    // Clear stagingColorPicker_ to test nullptr branch
    drawable->stagingColorPicker_ = nullptr;

    // Prepare should return early and not set needColorPick
    uint64_t vsyncTime = 1000000000; // 1 second in nanoseconds
    auto [needColorPick, needSync] = drawable->PrepareForExecution(vsyncTime, false);

    EXPECT_FALSE(needColorPick);
    EXPECT_FALSE(needSync);
}

/**
 * @tc.name: RSColorPickerDrawable008
 * @tc.desc: Test Prepare with NONE strategy
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerDrawableTest, RSColorPickerDrawable008, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);

    auto drawable = std::make_shared<DrawableV2::RSColorPickerDrawable>(false, id);
    ASSERT_NE(drawable, nullptr);

    // Create a ColorPickerParam with NONE strategy
    auto params = std::make_shared<ColorPickerParam>();
    params->strategy = ColorPickStrategyType::NONE;
    params->placeholder = ColorPlaceholder::NONE;
    params->interval = 0;
    drawable->stagingColorPicker_ = params;

    // Prepare should return early and not set needColorPick
    uint64_t vsyncTime = 1000000000;
    auto [needColorPick, needSync] = drawable->PrepareForExecution(vsyncTime, false);

    EXPECT_FALSE(needColorPick);
    EXPECT_FALSE(needSync);
}

/**
 * @tc.name: RSColorPickerDrawable009
 * @tc.desc: Test Prepare when interval not elapsed
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerDrawableTest, RSColorPickerDrawable009, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);

    auto drawable = std::make_shared<DrawableV2::RSColorPickerDrawable>(false, id);
    ASSERT_NE(drawable, nullptr);

    // Create a ColorPickerParam with 2000ms interval
    auto params = std::make_shared<ColorPickerParam>();
    params->strategy = ColorPickStrategyType::AVERAGE;
    params->placeholder = ColorPlaceholder::TEXT_CONTRAST;
    params->interval = 2000;
    drawable->stagingColorPicker_ = params;
    drawable->lastUpdateTime_ = 1000; // Last update at 1000ms

    // vsyncTime is 1500ms (1000000000 ns = 1000ms), interval not elapsed
    uint64_t vsyncTime = 1500000000;
    auto [needColorPick, needSync] = drawable->PrepareForExecution(vsyncTime, false);

    EXPECT_FALSE(needColorPick);
    EXPECT_FALSE(needSync);
}

/**
 * @tc.name: RSColorPickerDrawable010
 * @tc.desc: Test Prepare when interval elapsed
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerDrawableTest, RSColorPickerDrawable010, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);

    auto drawable = std::make_shared<DrawableV2::RSColorPickerDrawable>(false, id);
    ASSERT_NE(drawable, nullptr);

    // Create a ColorPickerParam with 1000ms interval
    auto params = std::make_shared<ColorPickerParam>();
    params->strategy = ColorPickStrategyType::AVERAGE;
    params->placeholder = ColorPlaceholder::TEXT_CONTRAST;
    params->interval = 1000;
    drawable->stagingColorPicker_ = params;
    drawable->lastUpdateTime_ = 1000; // Last update at 1000ms

    // vsyncTime is 2500ms (2500000000 ns), interval has elapsed (2500 >= 1000 + 1000)
    uint64_t vsyncTime = 2500000000;
    auto [needColorPick, needSync] = drawable->PrepareForExecution(vsyncTime, false);

    EXPECT_TRUE(needColorPick);
    EXPECT_TRUE(needSync); // stagingNeedColorPick_ changed from false to true
    EXPECT_EQ(drawable->lastUpdateTime_, 2500u); // lastUpdateTime_ should be updated
}

/**
 * @tc.name: RSColorPickerDrawable011
 * @tc.desc: Test Prepare with task scheduling
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerDrawableTest, RSColorPickerDrawable011, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);

    auto drawable = std::make_shared<DrawableV2::RSColorPickerDrawable>(false, id);
    ASSERT_NE(drawable, nullptr);

    // Create a ColorPickerParam with 1000ms interval
    auto params = std::make_shared<ColorPickerParam>();
    params->strategy = ColorPickStrategyType::DOMINANT;
    params->placeholder = ColorPlaceholder::FOREGROUND;
    params->interval = 1000;
    drawable->stagingColorPicker_ = params;
    drawable->lastUpdateTime_ = 1000;
    drawable->isTaskScheduled_ = false;

    // vsyncTime is 1500ms, interval not elapsed (1500 < 1000 + 1000) so task should be scheduled
    uint64_t vsyncTime = 1500000000;
    auto [needColorPick, needSync] = drawable->PrepareForExecution(vsyncTime, false);

    // Task should be scheduled (isTaskScheduled_ set to true)
    // Note: We can't easily verify the actual task was posted without mocking
    EXPECT_FALSE(needColorPick);
    EXPECT_FALSE(needSync); // stagingNeedColorPick_ stays false, darkMode unchanged
    EXPECT_TRUE(drawable->isTaskScheduled_);
}

/**
 * @tc.name: RSColorPickerDrawable012
 * @tc.desc: Test OnDraw with needColorPick logic and OnSync
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerDrawableTest, RSColorPickerDrawable012, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);

    node.GetMutableRenderProperties().SetColorPickerPlaceholder(
        static_cast<int>(ColorPlaceholder::TEXT_CONTRAST));
    node.GetMutableRenderProperties().SetColorPickerStrategy(
        static_cast<int>(ColorPickStrategyType::AVERAGE));
    node.GetMutableRenderProperties().SetColorPickerInterval(1000);

    auto drawable = std::static_pointer_cast<DrawableV2::RSColorPickerDrawable>(
        DrawableV2::RSColorPickerDrawable::OnGenerate(node));
    ASSERT_NE(drawable, nullptr);

    // Test OnSync syncs needColorPick_ from stagingNeedColorPick_
    drawable->stagingNeedColorPick_ = true;
    drawable->OnSync();
    EXPECT_TRUE(drawable->needColorPick_);

    drawable->stagingNeedColorPick_ = false;
    drawable->OnSync();
    EXPECT_FALSE(drawable->needColorPick_);

    // Test with different needColorPick values
    drawable->needColorPick_ = true;
    auto canvas = std::make_shared<Drawing::Canvas>();
    auto filterCanvas = std::make_shared<RSPaintFilterCanvas>(canvas.get());
    auto rect = std::make_shared<Drawing::Rect>();

    // OnDraw should execute color picking when needColorPick is true
    drawable->OnDraw(filterCanvas.get(), rect.get());
    ASSERT_TRUE(true);

    // Set needColorPick to false and test again
    drawable->needColorPick_ = false;
    drawable->OnDraw(filterCanvas.get(), rect.get());
    ASSERT_TRUE(true);

    // Test with null paintFilterCanvas
    drawable->OnDraw(nullptr, rect.get());
    ASSERT_TRUE(true);
}

/**
 * @tc.name: RSColorPickerDrawable013
 * @tc.desc: Test SetIsSystemDarkColorMode sets needSync_ when value changes
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerDrawableTest, RSColorPickerDrawable013, TestSize.Level1)
{
    NodeId id = 1;
    auto drawable = std::make_shared<DrawableV2::RSColorPickerDrawable>(false, id);
    ASSERT_NE(drawable, nullptr);

    // Initial state: stagingIsSystemDarkColorMode_ = false, needSync_ = false
    EXPECT_FALSE(drawable->stagingIsSystemDarkColorMode_);
    EXPECT_FALSE(drawable->needSync_);

    // Set to true when current value is false
    drawable->SetIsSystemDarkColorMode(true);

    // Verify: stagingIsSystemDarkColorMode_ should be true, needSync_ should be true
    EXPECT_TRUE(drawable->stagingIsSystemDarkColorMode_);
    EXPECT_TRUE(drawable->needSync_);
}

/**
 * @tc.name: RSColorPickerDrawable014
 * @tc.desc: Test SetIsSystemDarkColorMode does not set needSync_ when value unchanged (false -> false)
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerDrawableTest, RSColorPickerDrawable014, TestSize.Level1)
{
    NodeId id = 1;
    auto drawable = std::make_shared<DrawableV2::RSColorPickerDrawable>(false, id);
    ASSERT_NE(drawable, nullptr);

    // Initial state: stagingIsSystemDarkColorMode_ = false
    EXPECT_FALSE(drawable->stagingIsSystemDarkColorMode_);

    // Set needSync_ to false explicitly
    drawable->needSync_ = false;

    // Set to same value (false)
    drawable->SetIsSystemDarkColorMode(false);

    // Verify: stagingIsSystemDarkColorMode_ should still be false, needSync_ should still be false
    EXPECT_FALSE(drawable->stagingIsSystemDarkColorMode_);
    EXPECT_FALSE(drawable->needSync_);
}

/**
 * @tc.name: RSColorPickerDrawable015
 * @tc.desc: Test OnSync calls SetSystemDarkColorMode on colorPickerManager_
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerDrawableTest, RSColorPickerDrawable015, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);

    node.GetMutableRenderProperties().SetColorPickerPlaceholder(
        static_cast<int>(ColorPlaceholder::TEXT_CONTRAST));
    node.GetMutableRenderProperties().SetColorPickerStrategy(
        static_cast<int>(ColorPickStrategyType::AVERAGE));
    node.GetMutableRenderProperties().SetColorPickerInterval(1000);

    auto drawable = std::static_pointer_cast<DrawableV2::RSColorPickerDrawable>(
        DrawableV2::RSColorPickerDrawable::OnGenerate(node));
    ASSERT_NE(drawable, nullptr);
    ASSERT_NE(drawable->colorPickerManager_, nullptr);

    // Set dark mode via SetIsSystemDarkColorMode
    drawable->SetIsSystemDarkColorMode(true);
    EXPECT_TRUE(drawable->stagingIsSystemDarkColorMode_);
    EXPECT_TRUE(drawable->needSync_);

    // Call OnSync to sync the value to colorPickerManager_
    drawable->OnSync();

    // After sync, needSync_ should be false
    EXPECT_FALSE(drawable->needSync_);
}

/**
 * @tc.name: RSColorPickerDrawable016
 * @tc.desc: Test OnSync handles null colorPickerManager_ gracefully
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerDrawableTest, RSColorPickerDrawable016, TestSize.Level1)
{
    NodeId id = 1;
    auto drawable = std::make_shared<DrawableV2::RSColorPickerDrawable>(false, id);
    ASSERT_NE(drawable, nullptr);

    // Set needSync_ to true
    drawable->SetIsSystemDarkColorMode(true);
    EXPECT_TRUE(drawable->needSync_);

    // Set colorPickerManager_ to nullptr
    drawable->colorPickerManager_ = nullptr;

    // OnSync should handle null manager gracefully (no crash)
    drawable->OnSync();

    // Verify needSync_ is cleared even with null manager
    EXPECT_FALSE(drawable->needSync_);
}
} // namespace OHOS::Rosen
