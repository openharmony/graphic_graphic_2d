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

    // Test OnDraw with valid canvas - should not crash
    drawable->OnDraw(filterCanvas.get(), rect.get());

    // Test OnDraw with null canvas - should handle gracefully
    drawable->OnDraw(nullptr, rect.get());

    // Test OnDraw with null colorPickerManager - should handle gracefully
    drawable->colorPickerManager_ = nullptr;
    drawable->OnDraw(filterCanvas.get(), rect.get());
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
    // Test OnDraw with CLIENT_CALLBACK strategy - should not crash
    drawable->OnDraw(filterCanvas.get(), rect.get());
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
    // Test OnDraw with SURFACE_CONTRAST placeholder - should not crash
    drawable->OnDraw(filterCanvas.get(), rect.get());
}

// ============================================================================
// State Machine Tests (OnPrepare, ScheduleColorPickIfReady, SetState, GetState)
// ============================================================================

/**
 * @tc.name: RSColorPickerDrawable007
 * @tc.desc: Test OnPrepare with COLOR_PICK_THIS_FRAME state
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerDrawableTest, RSColorPickerDrawable007, TestSize.Level1)
{
    NodeId id = 1;
    auto drawable = std::make_shared<DrawableV2::RSColorPickerDrawable>(false, id);
    ASSERT_NE(drawable, nullptr);

    // Set state to COLOR_PICK_THIS_FRAME
    drawable->stagingState_ = DrawableV2::ColorPickerState::COLOR_PICK_THIS_FRAME;
    drawable->stagingNeedColorPick_ = false;

    bool needSync = drawable->OnPrepare(false);
    EXPECT_TRUE(needSync);
    EXPECT_TRUE(drawable->stagingNeedColorPick_);
}

/**
 * @tc.name: RSColorPickerDrawable008
 * @tc.desc: Test OnPrepare with PREPARING state (needColorPick should be false)
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerDrawableTest, RSColorPickerDrawable008, TestSize.Level1)
{
    NodeId id = 1;
    auto drawable = std::make_shared<DrawableV2::RSColorPickerDrawable>(false, id);
    ASSERT_NE(drawable, nullptr);

    // Set state to PREPARING
    drawable->stagingState_ = DrawableV2::ColorPickerState::PREPARING;
    drawable->stagingNeedColorPick_ = true; // Was true before

    bool needSync = drawable->OnPrepare(false);
    EXPECT_TRUE(needSync); // Changed from true to false
    EXPECT_FALSE(drawable->stagingNeedColorPick_);
}

/**
 * @tc.name: RSColorPickerDrawable009
 * @tc.desc: Test OnPrepare with darkMode change
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerDrawableTest, RSColorPickerDrawable009, TestSize.Level1)
{
    NodeId id = 1;
    auto drawable = std::make_shared<DrawableV2::RSColorPickerDrawable>(false, id);
    ASSERT_NE(drawable, nullptr);

    // Set initial dark mode to false
    drawable->stagingIsSystemDarkColorMode_ = false;

    // Call OnPrepare with darkMode = true
    bool needSync = drawable->OnPrepare(true);
    EXPECT_TRUE(needSync); // Dark mode changed
    EXPECT_TRUE(drawable->stagingIsSystemDarkColorMode_);
}

/**
 * @tc.name: RSColorPickerDrawable010
 * @tc.desc: Test ScheduleColorPickIfReady returns early with nullptr stagingColorPicker
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerDrawableTest, RSColorPickerDrawable010, TestSize.Level1)
{
    NodeId id = 1;
    auto drawable = std::make_shared<DrawableV2::RSColorPickerDrawable>(false, id);
    ASSERT_NE(drawable, nullptr);

    drawable->stagingColorPicker_ = nullptr;
    drawable->stagingState_ = DrawableV2::ColorPickerState::PREPARING;

    // Should return early without scheduling
    drawable->ScheduleColorPickIfReady(1000000000);
    EXPECT_EQ(drawable->stagingState_, DrawableV2::ColorPickerState::PREPARING);
}

/**
 * @tc.name: RSColorPickerDrawable011
 * @tc.desc: Test ScheduleColorPickIfReady returns early with NONE strategy
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerDrawableTest, RSColorPickerDrawable011, TestSize.Level1)
{
    NodeId id = 1;
    auto drawable = std::make_shared<DrawableV2::RSColorPickerDrawable>(false, id);
    ASSERT_NE(drawable, nullptr);

    auto params = std::make_shared<ColorPickerParam>();
    params->strategy = ColorPickStrategyType::NONE;
    drawable->stagingColorPicker_ = params;
    drawable->stagingState_ = DrawableV2::ColorPickerState::PREPARING;

    drawable->ScheduleColorPickIfReady(1000000000);
    EXPECT_EQ(drawable->stagingState_, DrawableV2::ColorPickerState::PREPARING);
}

/**
 * @tc.name: RSColorPickerDrawable012
 * @tc.desc: Test ScheduleColorPickIfReady returns early when not in PREPARING state
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerDrawableTest, RSColorPickerDrawable012, TestSize.Level1)
{
    NodeId id = 1;
    auto drawable = std::make_shared<DrawableV2::RSColorPickerDrawable>(false, id);
    ASSERT_NE(drawable, nullptr);

    auto params = std::make_shared<ColorPickerParam>();
    params->strategy = ColorPickStrategyType::AVERAGE;
    params->interval = 1000;
    drawable->stagingColorPicker_ = params;
    drawable->stagingState_ = DrawableV2::ColorPickerState::SCHEDULED;

    // Should return early because state is not PREPARING
    drawable->ScheduleColorPickIfReady(1000000000);
    EXPECT_EQ(drawable->stagingState_, DrawableV2::ColorPickerState::SCHEDULED);
}

/**
 * @tc.name: RSColorPickerDrawable013
 * @tc.desc: Test ScheduleColorPickIfReady schedules with delay when cooldown not elapsed
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerDrawableTest, RSColorPickerDrawable013, TestSize.Level1)
{
    NodeId id = 1;
    auto drawable = std::make_shared<DrawableV2::RSColorPickerDrawable>(false, id);
    ASSERT_NE(drawable, nullptr);

    auto params = std::make_shared<ColorPickerParam>();
    params->strategy = ColorPickStrategyType::AVERAGE;
    params->interval = 2000;
    drawable->stagingColorPicker_ = params;
    drawable->stagingState_ = DrawableV2::ColorPickerState::PREPARING;
    drawable->lastUpdateTime_ = 1000; // Last update at 1000ms

    // vsyncTime is 1500ms (cooldown not elapsed: 1500 < 1000 + 2000)
    uint64_t vsyncTime = 1500000000;
    drawable->ScheduleColorPickIfReady(vsyncTime);

    // State should transition to SCHEDULED
    EXPECT_EQ(drawable->stagingState_, DrawableV2::ColorPickerState::SCHEDULED);
}

/**
 * @tc.name: RSColorPickerDrawable014
 * @tc.desc: Test ScheduleColorPickIfReady schedules immediately when cooldown elapsed
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerDrawableTest, RSColorPickerDrawable014, TestSize.Level1)
{
    NodeId id = 1;
    auto drawable = std::make_shared<DrawableV2::RSColorPickerDrawable>(false, id);
    ASSERT_NE(drawable, nullptr);

    auto params = std::make_shared<ColorPickerParam>();
    params->strategy = ColorPickStrategyType::AVERAGE;
    params->interval = 1000;
    drawable->stagingColorPicker_ = params;
    drawable->stagingState_ = DrawableV2::ColorPickerState::PREPARING;
    drawable->lastUpdateTime_ = 1000;

    // vsyncTime is 2500ms (cooldown elapsed: 2500 >= 1000 + 1000)
    uint64_t vsyncTime = 2500000000;
    drawable->ScheduleColorPickIfReady(vsyncTime);

    EXPECT_EQ(drawable->stagingState_, DrawableV2::ColorPickerState::SCHEDULED);
    EXPECT_EQ(drawable->lastUpdateTime_, 2500u);
}

/**
 * @tc.name: RSColorPickerDrawable015
 * @tc.desc: Test SetState valid transition from PREPARING to SCHEDULED
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerDrawableTest, RSColorPickerDrawable015, TestSize.Level1)
{
    NodeId id = 1;
    auto drawable = std::make_shared<DrawableV2::RSColorPickerDrawable>(false, id);
    ASSERT_NE(drawable, nullptr);

    drawable->stagingState_ = DrawableV2::ColorPickerState::PREPARING;
    drawable->SetState(DrawableV2::ColorPickerState::SCHEDULED);

    EXPECT_EQ(drawable->stagingState_, DrawableV2::ColorPickerState::SCHEDULED);
}

/**
 * @tc.name: RSColorPickerDrawable016
 * @tc.desc: Test SetState valid transition from SCHEDULED to COLOR_PICK_THIS_FRAME
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerDrawableTest, RSColorPickerDrawable016, TestSize.Level1)
{
    NodeId id = 1;
    auto drawable = std::make_shared<DrawableV2::RSColorPickerDrawable>(false, id);
    ASSERT_NE(drawable, nullptr);

    drawable->stagingState_ = DrawableV2::ColorPickerState::SCHEDULED;
    drawable->SetState(DrawableV2::ColorPickerState::COLOR_PICK_THIS_FRAME);

    EXPECT_EQ(drawable->stagingState_, DrawableV2::ColorPickerState::COLOR_PICK_THIS_FRAME);
}

/**
 * @tc.name: RSColorPickerDrawable017
 * @tc.desc: Test SetState valid transition from COLOR_PICK_THIS_FRAME to PREPARING
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerDrawableTest, RSColorPickerDrawable017, TestSize.Level1)
{
    NodeId id = 1;
    auto drawable = std::make_shared<DrawableV2::RSColorPickerDrawable>(false, id);
    ASSERT_NE(drawable, nullptr);

    drawable->stagingState_ = DrawableV2::ColorPickerState::COLOR_PICK_THIS_FRAME;
    drawable->SetState(DrawableV2::ColorPickerState::PREPARING);

    EXPECT_EQ(drawable->stagingState_, DrawableV2::ColorPickerState::PREPARING);
}

/**
 * @tc.name: RSColorPickerDrawable018
 * @tc.desc: Test SetState invalid transition from PREPARING to COLOR_PICK_THIS_FRAME
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerDrawableTest, RSColorPickerDrawable018, TestSize.Level1)
{
    NodeId id = 1;
    auto drawable = std::make_shared<DrawableV2::RSColorPickerDrawable>(false, id);
    ASSERT_NE(drawable, nullptr);

    drawable->stagingState_ = DrawableV2::ColorPickerState::PREPARING;
    drawable->SetState(DrawableV2::ColorPickerState::COLOR_PICK_THIS_FRAME);

    // Should not change state (invalid transition)
    EXPECT_EQ(drawable->stagingState_, DrawableV2::ColorPickerState::PREPARING);
}

/**
 * @tc.name: RSColorPickerDrawable019
 * @tc.desc: Test SetState invalid transition from SCHEDULED to PREPARING
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerDrawableTest, RSColorPickerDrawable019, TestSize.Level1)
{
    NodeId id = 1;
    auto drawable = std::make_shared<DrawableV2::RSColorPickerDrawable>(false, id);
    ASSERT_NE(drawable, nullptr);

    drawable->stagingState_ = DrawableV2::ColorPickerState::SCHEDULED;
    drawable->SetState(DrawableV2::ColorPickerState::PREPARING);

    // Should not change state (invalid transition)
    EXPECT_EQ(drawable->stagingState_, DrawableV2::ColorPickerState::SCHEDULED);
}

/**
 * @tc.name: RSColorPickerDrawable020
 * @tc.desc: Test GetState returns current state
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerDrawableTest, RSColorPickerDrawable020, TestSize.Level1)
{
    NodeId id = 1;
    auto drawable = std::make_shared<DrawableV2::RSColorPickerDrawable>(false, id);
    ASSERT_NE(drawable, nullptr);

    drawable->stagingState_ = DrawableV2::ColorPickerState::PREPARING;
    EXPECT_EQ(drawable->GetState(), DrawableV2::ColorPickerState::PREPARING);

    drawable->stagingState_ = DrawableV2::ColorPickerState::SCHEDULED;
    EXPECT_EQ(drawable->GetState(), DrawableV2::ColorPickerState::SCHEDULED);

    drawable->stagingState_ = DrawableV2::ColorPickerState::COLOR_PICK_THIS_FRAME;
    EXPECT_EQ(drawable->GetState(), DrawableV2::ColorPickerState::COLOR_PICK_THIS_FRAME);
}

/**
 * @tc.name: RSColorPickerDrawable021
 * @tc.desc: Test ResetColorMemory resets state to PREPARING
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerDrawableTest, RSColorPickerDrawable021, TestSize.Level1)
{
    NodeId id = 1;
    auto drawable = std::make_shared<DrawableV2::RSColorPickerDrawable>(false, id);
    ASSERT_NE(drawable, nullptr);

    drawable->stagingState_ = DrawableV2::ColorPickerState::COLOR_PICK_THIS_FRAME;
    drawable->stagingNeedColorPick_ = true;

    drawable->ResetColorMemory();

    EXPECT_EQ(drawable->stagingState_, DrawableV2::ColorPickerState::PREPARING);
    EXPECT_FALSE(drawable->stagingNeedColorPick_);
}

/**
 * @tc.name: RSColorPickerDrawable022
 * @tc.desc: Test OnDraw with needColorPick logic and OnSync
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerDrawableTest, RSColorPickerDrawable022, TestSize.Level1)
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

    // OnDraw should execute color picking when needColorPick is true - should not crash
    drawable->OnDraw(filterCanvas.get(), rect.get());

    // Set needColorPick to false and test again - should skip color picking
    drawable->needColorPick_ = false;
    drawable->OnDraw(filterCanvas.get(), rect.get());

    // Test with null paintFilterCanvas - should handle gracefully
    drawable->OnDraw(nullptr, rect.get());
}

/**
 * @tc.name: RSColorPickerDrawable023
 * @tc.desc: Test SetState with same state (no-op transition)
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerDrawableTest, RSColorPickerDrawable023, TestSize.Level1)
{
    NodeId id = 1;
    auto drawable = std::make_shared<DrawableV2::RSColorPickerDrawable>(false, id);
    ASSERT_NE(drawable, nullptr);

    // Test PREPARING to PREPARING (same state - should be allowed)
    drawable->stagingState_ = DrawableV2::ColorPickerState::PREPARING;
    drawable->SetState(DrawableV2::ColorPickerState::PREPARING);
    EXPECT_EQ(drawable->stagingState_, DrawableV2::ColorPickerState::PREPARING);

    // Test SCHEDULED to SCHEDULED (same state - should be allowed)
    drawable->stagingState_ = DrawableV2::ColorPickerState::SCHEDULED;
    drawable->SetState(DrawableV2::ColorPickerState::SCHEDULED);
    EXPECT_EQ(drawable->stagingState_, DrawableV2::ColorPickerState::SCHEDULED);
}

/**
 * @tc.name: RSColorPickerDrawable024
 * @tc.desc: Test RSRenderNode::PrepareColorPicker function
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(RSColorPickerDrawableTest, RSColorPickerDrawable024, TestSize.Level1)
{
    NodeId id = 1;
    RSRenderNode node(id);

    // Test 1: Returns false when no color picker drawable exists
    bool result = node.PrepareColorPicker(false);
    EXPECT_FALSE(result);

    // Set up color picker properties to create drawable
    node.GetMutableRenderProperties().SetColorPickerPlaceholder(
        static_cast<int>(ColorPlaceholder::TEXT_CONTRAST));
    node.GetMutableRenderProperties().SetColorPickerStrategy(
        static_cast<int>(ColorPickStrategyType::AVERAGE));
    node.GetMutableRenderProperties().SetColorPickerInterval(1000);

    // Force drawable generation
    auto drawable = DrawableV2::RSColorPickerDrawable::OnGenerate(node);
    ASSERT_NE(drawable, nullptr);
    node.GetDrawableVec(__func__)[static_cast<int8_t>(RSDrawableSlot::COLOR_PICKER)] = drawable;

    // Test 2: Returns false when state is PREPARING (not COLOR_PICK_THIS_FRAME)
    auto colorPickerDrawable = std::static_pointer_cast<DrawableV2::RSColorPickerDrawable>(drawable);
    colorPickerDrawable->stagingState_ = DrawableV2::ColorPickerState::PREPARING;
    result = node.PrepareColorPicker(false);
    EXPECT_FALSE(result);

    // Test 3: Returns true when state is COLOR_PICK_THIS_FRAME
    colorPickerDrawable->stagingState_ = DrawableV2::ColorPickerState::COLOR_PICK_THIS_FRAME;
    result = node.PrepareColorPicker(false);
    EXPECT_TRUE(result);
    EXPECT_TRUE(colorPickerDrawable->stagingNeedColorPick_);
}
} // namespace OHOS::Rosen
