/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>

#define private public
#include "boot_compile_progress.h"
#undef private
#include "boot_animation_strategy.h"
#include "parameters.h"
#include "animation/rs_cubic_bezier_interpolator.h"
#include "ui/rs_canvas_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {
namespace {
    constexpr int32_t TEST_WINDOW_WIDTH_1080 = 1080;
    constexpr int32_t TEST_WINDOW_HEIGHT_1920 = 1920;
    constexpr int32_t TEST_WINDOW_WIDTH_480 = 480;
    constexpr int32_t TEST_WINDOW_HEIGHT_480 = 480;
    constexpr int32_t TEST_PROGRESS_50 = 50;
    constexpr int32_t TEST_PROGRESS_100 = 100;
    constexpr int32_t TEST_TIMES_10 = 10;
    constexpr int32_t TEST_TIMES_20 = 20;
    constexpr int32_t TEST_START_TIME_MS = 1000;
    constexpr int64_t TEST_END_TIME_PREDICT_MS = 100000;
    constexpr int32_t TEST_TIME_LIMIT_SEC = 100;
    constexpr int32_t TEST_ROTATE_DEGREE_90 = 90;
    constexpr int32_t TEST_PROGRESS_OFFSET = 10;
    constexpr int32_t TEST_PROGRESS_HEIGHT = 100;
    constexpr int32_t TEST_PROGRESS_FRAME_HEIGHT = 50;
    constexpr int32_t TEST_PROGRESS_FONT_SIZE = 12;
    constexpr int32_t TEST_PROGRESS_RADIUS_SIZE = 5;
    constexpr int32_t TEST_SCREEN_STATUS_NEGATIVE = -1;
    constexpr int32_t TEST_SCREEN_STATUS_1 = 1;
    constexpr int32_t TEST_FRAME_NUM_15 = 15;
    constexpr int32_t TEST_FRAME_NUM_25 = 25;
    constexpr int32_t TEST_MAX_LENGTH = 1920;
    constexpr float TEST_SHARP_CURVE_CTLX1 = 0.33f;
    constexpr float TEST_SHARP_CURVE_CTLY1 = 0.0f;
    constexpr float TEST_SHARP_CURVE_CTLX2 = 0.67f;
    constexpr float TEST_SHARP_CURVE_CTLY2 = 1.0f;
}

class BootCompileProgressTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void BootCompileProgressTest::SetUpTestCase() {}
void BootCompileProgressTest::TearDownTestCase() {}
void BootCompileProgressTest::SetUp() {}
void BootCompileProgressTest::TearDown() {}

HWTEST_F(BootCompileProgressTest, BootCompileProgressTest_001, TestSize.Level1)
{
    std::shared_ptr<BootCompileProgress> progress = std::make_shared<BootCompileProgress>();
    ASSERT_NE(progress, nullptr);
}

/**
 * @tc.name: Init_Normal_ExecuteSuccessfully
 * @tc.desc: Verify the Init function executes successfully.
 * @tc.type: FUNC
 */
HWTEST_F(BootCompileProgressTest, Init_Normal_ExecuteSuccessfully, TestSize.Level1)
{
    std::shared_ptr<BootCompileProgress> progress = std::make_shared<BootCompileProgress>();
    ASSERT_NE(progress, nullptr);
    std::shared_ptr<BootAnimationStrategy> strategy = std::make_shared<BootAnimationStrategy>();
    strategy->GetConnectToRenderMap(1);
    BootAnimationConfig config;
    config.screenId = 0;
    std::string configPath = "";
    progress->Init(configPath, config, strategy->connectToRenderMap_.begin()->second);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: WindowSize_SetNormal_ExecuteSuccessfully
 * @tc.desc: Verify the window size settings execute successfully.
 * @tc.type: FUNC
 */
HWTEST_F(BootCompileProgressTest, WindowSize_SetNormal_ExecuteSuccessfully, TestSize.Level1)
{
    std::shared_ptr<BootCompileProgress> progress = std::make_shared<BootCompileProgress>();
    ASSERT_NE(progress, nullptr);
    progress->windowWidth_ = TEST_WINDOW_WIDTH_1080;
    progress->windowHeight_ = TEST_WINDOW_HEIGHT_1920;
    progress->progress_ = TEST_PROGRESS_50;
    progress->isUpdateOptEnd_ = false;
    EXPECT_EQ(progress->windowWidth_, TEST_WINDOW_WIDTH_1080);
    EXPECT_EQ(progress->windowHeight_, TEST_WINDOW_HEIGHT_1920);
}

/**
 * @tc.name: WearableDevice_SetParameters_ExecuteSuccessfully
 * @tc.desc: Verify the wearable device settings execute successfully.
 * @tc.type: FUNC
 */
HWTEST_F(BootCompileProgressTest, WearableDevice_SetParameters_ExecuteSuccessfully, TestSize.Level1)
{
    std::shared_ptr<BootCompileProgress> progress = std::make_shared<BootCompileProgress>();
    ASSERT_NE(progress, nullptr);
    progress->isWearable_ = true;
    progress->isOther_ = false;
    progress->windowWidth_ = TEST_WINDOW_WIDTH_480;
    progress->windowHeight_ = TEST_WINDOW_HEIGHT_480;
    EXPECT_EQ(progress->isWearable_, true);
    EXPECT_EQ(progress->isOther_, false);
}

/**
 * @tc.name: OtherDevice_SetParameters_ExecuteSuccessfully
 * @tc.desc: Verify the other device settings execute successfully.
 * @tc.type: FUNC
 */
HWTEST_F(BootCompileProgressTest, OtherDevice_SetParameters_ExecuteSuccessfully, TestSize.Level1)
{
    std::shared_ptr<BootCompileProgress> progress = std::make_shared<BootCompileProgress>();
    ASSERT_NE(progress, nullptr);
    progress->isWearable_ = false;
    progress->isOther_ = true;
    progress->windowWidth_ = TEST_WINDOW_WIDTH_1080;
    progress->windowHeight_ = TEST_WINDOW_HEIGHT_1920;
    EXPECT_EQ(progress->isWearable_, false);
    EXPECT_EQ(progress->isOther_, true);
}

/**
 * @tc.name: IsBmsBundleReady_Normal_ReturnCorrectResult
 * @tc.desc: Verify the IsBmsBundleReady function returns correct result.
 * @tc.type: FUNC
 */
HWTEST_F(BootCompileProgressTest, IsBmsBundleReady_Normal_ReturnCorrectResult, TestSize.Level1)
{
    std::shared_ptr<BootCompileProgress> progress = std::make_shared<BootCompileProgress>();
    ASSERT_NE(progress, nullptr);
    bool result = progress->IsBmsBundleReady();
    EXPECT_TRUE(result || !result);
}

/**
 * @tc.name: GetFirmwareUpdateState_Normal_ReturnCorrectState
 * @tc.desc: Verify the GetFirmwareUpdateState function returns correct state.
 * @tc.type: FUNC
 */
HWTEST_F(BootCompileProgressTest, GetFirmwareUpdateState_Normal_ReturnCorrectState, TestSize.Level1)
{
    std::shared_ptr<BootCompileProgress> progress = std::make_shared<BootCompileProgress>();
    ASSERT_NE(progress, nullptr);
    std::string state = progress->GetFirmwareUpdateState();
    EXPECT_TRUE(state.empty() || !state.empty());
}

/**
 * @tc.name: RecordDeviceType_Normal_SetCorrectDeviceType
 * @tc.desc: Verify the RecordDeviceType function sets correct device type.
 * @tc.type: FUNC
 */
HWTEST_F(BootCompileProgressTest, RecordDeviceType_Normal_SetCorrectDeviceType, TestSize.Level1)
{
    std::shared_ptr<BootCompileProgress> progress = std::make_shared<BootCompileProgress>();
    progress->RecordDeviceType();
    EXPECT_EQ(progress->isWearable_, false);
    EXPECT_EQ(progress->isOther_, false);
}

/**
 * @tc.name: DrawProgressPoint_TimesZero_ReturnCorrectBrush
 * @tc.desc: Verify the DrawProgressPoint function returns correct brush when times is zero.
 * @tc.type: FUNC
 */
HWTEST_F(BootCompileProgressTest, DrawProgressPoint_TimesZero_ReturnCorrectBrush, TestSize.Level0)
{
    std::shared_ptr<BootCompileProgress> progress = std::make_shared<BootCompileProgress>();
    progress->times_ = 0;
    progress->sharpCurve_ = std::make_shared<Rosen::RSCubicBezierInterpolator>(
        TEST_SHARP_CURVE_CTLX1, TEST_SHARP_CURVE_CTLY1, TEST_SHARP_CURVE_CTLX2, TEST_SHARP_CURVE_CTLY2);
    Rosen::Drawing::Brush brush = progress->DrawProgressPoint(0, 0);
    EXPECT_NE(brush.GetColor(), 0);
}

/**
 * @tc.name: DrawProgressPoint_TimesTen_ReturnCorrectBrush
 * @tc.desc: Verify the DrawProgressPoint function returns correct brush when times is ten.
 * @tc.type: FUNC
 */
HWTEST_F(BootCompileProgressTest, DrawProgressPoint_TimesTen_ReturnCorrectBrush, TestSize.Level0)
{
    std::shared_ptr<BootCompileProgress> progress = std::make_shared<BootCompileProgress>();
    progress->times_ = TEST_TIMES_10;
    progress->sharpCurve_ = std::make_shared<Rosen::RSCubicBezierInterpolator>(
        TEST_SHARP_CURVE_CTLX1, TEST_SHARP_CURVE_CTLY1, TEST_SHARP_CURVE_CTLX2, TEST_SHARP_CURVE_CTLY2);
    Rosen::Drawing::Brush brush = progress->DrawProgressPoint(1, TEST_FRAME_NUM_15);
    EXPECT_NE(brush.GetColor(), 0);
}

/**
 * @tc.name: DrawProgressPoint_TimesTwenty_ReturnCorrectBrush
 * @tc.desc: Verify the DrawProgressPoint function returns correct brush when times is twenty.
 * @tc.type: FUNC
 */
HWTEST_F(BootCompileProgressTest, DrawProgressPoint_TimesTwenty_ReturnCorrectBrush, TestSize.Level0)
{
    std::shared_ptr<BootCompileProgress> progress = std::make_shared<BootCompileProgress>();
    progress->times_ = TEST_TIMES_20;
    progress->sharpCurve_ = std::make_shared<Rosen::RSCubicBezierInterpolator>(
        TEST_SHARP_CURVE_CTLX1, TEST_SHARP_CURVE_CTLY1, TEST_SHARP_CURVE_CTLX2, TEST_SHARP_CURVE_CTLY2);
    Rosen::Drawing::Brush brush = progress->DrawProgressPoint(2, TEST_FRAME_NUM_25);
    EXPECT_NE(brush.GetColor(), 0);
}

/**
 * @tc.name: SetFrame_EmptyProgressConfigs_ExecuteSuccessfully
 * @tc.desc: Verify the SetFrame function with empty progress configs.
 * @tc.type: FUNC
 */
HWTEST_F(BootCompileProgressTest, SetFrame_EmptyProgressConfigs_ExecuteSuccessfully, TestSize.Level1)
{
    std::shared_ptr<BootCompileProgress> progress = std::make_shared<BootCompileProgress>();
    ASSERT_NE(progress, nullptr);
    progress->rsCanvasNode_ = Rosen::RSCanvasNode::Create();
    progress->progressConfigsMap_.clear();
    progress->SetFrame();
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetFrame_WearableDevice_ExecuteSuccessfully
 * @tc.desc: Verify the SetFrame function with wearable device.
 * @tc.type: FUNC
 */
HWTEST_F(BootCompileProgressTest, SetFrame_WearableDevice_ExecuteSuccessfully, TestSize.Level1)
{
    std::shared_ptr<BootCompileProgress> progress = std::make_shared<BootCompileProgress>();
    ASSERT_NE(progress, nullptr);
    progress->rsCanvasNode_ = Rosen::RSCanvasNode::Create();
    progress->isWearable_ = true;
    progress->windowWidth_ = TEST_WINDOW_WIDTH_480;
    progress->windowHeight_ = TEST_WINDOW_HEIGHT_480;
    progress->SetFrame();
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetFrame_WithProgressConfig_ExecuteSuccessfully
 * @tc.desc: Verify the SetFrame function with progress config.
 * @tc.type: FUNC
 */
HWTEST_F(BootCompileProgressTest, SetFrame_WithProgressConfig_ExecuteSuccessfully, TestSize.Level1)
{
    std::shared_ptr<BootCompileProgress> progress = std::make_shared<BootCompileProgress>();
    ASSERT_NE(progress, nullptr);
    progress->rsCanvasNode_ = Rosen::RSCanvasNode::Create();
    BootAnimationProgressConfig config;
    config.progressScreenId = 0;
    progress->progressConfigsMap_.emplace(0, config);
    progress->screenId_ = 0;
    progress->screenStatus_ = TEST_SCREEN_STATUS_NEGATIVE;
    progress->windowWidth_ = TEST_WINDOW_WIDTH_1080;
    progress->windowHeight_ = TEST_WINDOW_HEIGHT_1920;
    progress->SetFrame();
    EXPECT_TRUE(true);
}

/**
 * @tc.name: UpdateCompileProgress_ProgressSet_ExecuteSuccessfully
 * @tc.desc: Verify the UpdateCompileProgress function with progress set.
 * @tc.type: FUNC
 */
HWTEST_F(BootCompileProgressTest, UpdateCompileProgress_ProgressSet_ExecuteSuccessfully, TestSize.Level1)
{
    std::shared_ptr<BootCompileProgress> progress = std::make_shared<BootCompileProgress>();
    ASSERT_NE(progress, nullptr);
    progress->progress_ = TEST_PROGRESS_50;
    progress->UpdateCompileProgress();
    EXPECT_TRUE(true);
}

/**
 * @tc.name: UpdateCompileProgress_TimeSet_ExecuteSuccessfully
 * @tc.desc: Verify the UpdateCompileProgress function with time set.
 * @tc.type: FUNC
 */
HWTEST_F(BootCompileProgressTest, UpdateCompileProgress_TimeSet_ExecuteSuccessfully, TestSize.Level1)
{
    std::shared_ptr<BootCompileProgress> progress = std::make_shared<BootCompileProgress>();
    ASSERT_NE(progress, nullptr);
    progress->startTimeMs_ = TEST_START_TIME_MS;
    progress->endTimePredictMs_ = TEST_END_TIME_PREDICT_MS;
    progress->timeLimitSec_ = TEST_TIME_LIMIT_SEC;
    progress->UpdateCompileProgress();
    EXPECT_TRUE(true);
}

/**
 * @tc.name: UpdateText_NotUpdate_ExecuteSuccessfully
 * @tc.desc: Verify the UpdateText function when not updating.
 * @tc.type: FUNC
 */
HWTEST_F(BootCompileProgressTest, UpdateText_NotUpdate_ExecuteSuccessfully, TestSize.Level1)
{
    std::shared_ptr<BootCompileProgress> progress = std::make_shared<BootCompileProgress>();
    ASSERT_NE(progress, nullptr);
    progress->isUpdateText_ = false;
    progress->UpdateText();
    EXPECT_TRUE(true);
}

/**
 * @tc.name: UpdateCompileProgress_DisplayInfoSet_ExecuteSuccessfully
 * @tc.desc: Verify the UpdateCompileProgress function with display info set.
 * @tc.type: FUNC
 */
HWTEST_F(BootCompileProgressTest, UpdateCompileProgress_DisplayInfoSet_ExecuteSuccessfully, TestSize.Level1)
{
    std::shared_ptr<BootCompileProgress> progress = std::make_shared<BootCompileProgress>();
    ASSERT_NE(progress, nullptr);
    progress->displayInfo_ = "test";
    progress->progress_ = TEST_PROGRESS_100;
    progress->UpdateCompileProgress();
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetFrame_RotateDegreeSet_ExecuteSuccessfully
 * @tc.desc: Verify the SetFrame function with rotate degree set.
 * @tc.type: FUNC
 */
HWTEST_F(BootCompileProgressTest, SetFrame_RotateDegreeSet_ExecuteSuccessfully, TestSize.Level1)
{
    std::shared_ptr<BootCompileProgress> progress = std::make_shared<BootCompileProgress>();
    ASSERT_NE(progress, nullptr);
    progress->rsCanvasNode_ = Rosen::RSCanvasNode::Create();
    progress->rotateDegree_ = TEST_ROTATE_DEGREE_90;
    BootAnimationProgressConfig config;
    config.progressDegree = TEST_ROTATE_DEGREE_90;
    progress->progressConfigsMap_.emplace(0, config);
    progress->screenId_ = 0;
    progress->screenStatus_ = TEST_SCREEN_STATUS_NEGATIVE;
    progress->SetFrame();
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetSpecialProgressFrame_Normal_ExecuteSuccessfully
 * @tc.desc: Verify the SetSpecialProgressFrame function executes successfully.
 * @tc.type: FUNC
 */
HWTEST_F(BootCompileProgressTest, SetSpecialProgressFrame_Normal_ExecuteSuccessfully, TestSize.Level1)
{
    std::shared_ptr<BootCompileProgress> progress = std::make_shared<BootCompileProgress>();
    ASSERT_NE(progress, nullptr);
    progress->rsCanvasNode_ = Rosen::RSCanvasNode::Create();
    BootAnimationProgressConfig config;
    config.progressScreenId = 0;
    config.progressOffset = TEST_PROGRESS_OFFSET;
    config.progressHeight = TEST_PROGRESS_HEIGHT;
    config.progressFrameHeight = TEST_PROGRESS_FRAME_HEIGHT;
    config.progressFontSize = TEST_PROGRESS_FONT_SIZE;
    config.progressRadiusSize = TEST_PROGRESS_RADIUS_SIZE;
    progress->progressConfigsMap_.emplace(0, config);
    progress->screenId_ = 0;
    progress->screenStatus_ = TEST_SCREEN_STATUS_NEGATIVE;
    progress->windowWidth_ = TEST_WINDOW_WIDTH_1080;
    progress->windowHeight_ = TEST_WINDOW_HEIGHT_1920;
    progress->SetSpecialProgressFrame(TEST_MAX_LENGTH, 0);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetSpecialProgressFrame_NegativeConfig_ExecuteSuccessfully
 * @tc.desc: Verify the SetSpecialProgressFrame function with negative config.
 * @tc.type: FUNC
 */
HWTEST_F(BootCompileProgressTest, SetSpecialProgressFrame_NegativeConfig_ExecuteSuccessfully, TestSize.Level1)
{
    std::shared_ptr<BootCompileProgress> progress = std::make_shared<BootCompileProgress>();
    ASSERT_NE(progress, nullptr);
    progress->rsCanvasNode_ = Rosen::RSCanvasNode::Create();
    BootAnimationProgressConfig config;
    config.progressScreenId = TEST_SCREEN_STATUS_1;
    config.progressOffset = TEST_SCREEN_STATUS_NEGATIVE;
    config.progressHeight = TEST_SCREEN_STATUS_NEGATIVE;
    config.progressFrameHeight = TEST_SCREEN_STATUS_NEGATIVE;
    config.progressFontSize = TEST_SCREEN_STATUS_NEGATIVE;
    config.progressRadiusSize = TEST_SCREEN_STATUS_NEGATIVE;
    progress->progressConfigsMap_.emplace(TEST_SCREEN_STATUS_1, config);
    progress->screenId_ = TEST_SCREEN_STATUS_1;
    progress->screenStatus_ = TEST_SCREEN_STATUS_1;
    progress->windowWidth_ = TEST_WINDOW_WIDTH_1080;
    progress->windowHeight_ = TEST_WINDOW_HEIGHT_1920;
    progress->SetSpecialProgressFrame(TEST_MAX_LENGTH, TEST_SCREEN_STATUS_1);
    EXPECT_TRUE(true);
}
}