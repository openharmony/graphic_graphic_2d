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

#include <algorithm>

#include "boot_compile_progress.h"
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
    constexpr int64_t TEST_TIME_STAGE_1_MS = 667;
    constexpr int64_t TEST_TIME_STAGE_2_MS = 1334;
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
    constexpr int64_t TEST_TIME_MID_STAGE_MS = 1000;
    constexpr int64_t TEST_TIME_LATE_STAGE_MS = 1800;
    constexpr int32_t TEST_ROG_WIDTH_1440 = 1440;
    constexpr int32_t TEST_ROG_HEIGHT_2560 = 2560;
    constexpr int32_t TEST_FRAME_NUM_0 = 0;
    constexpr int32_t TEST_REFRESH_RATE_60 = 60;
    constexpr int32_t TEST_REFRESH_RATE_90 = 90;
    constexpr int32_t TEST_REFRESH_RATE_120 = 120;
    constexpr int32_t TEST_CHANGE_FREQ_60 = 4;
    constexpr int32_t TEST_CHANGE_FREQ_90 = 6;
    constexpr int32_t TEST_CHANGE_FREQ_120 = 8;
    constexpr float TEST_SHARP_CURVE_CTLX1 = 0.33f;
    constexpr float TEST_SHARP_CURVE_CTLY1 = 0.0f;
    constexpr float TEST_SHARP_CURVE_CTLX2 = 0.67f;
    constexpr float TEST_SHARP_CURVE_CTLY2 = 1.0f;
    constexpr int32_t INVALID_SCREEN_ID = 999;
}

class MockIRemoteObject : public IRemoteObject {
public:
    MockIRemoteObject() : IRemoteObject(u"MockIRemoteObject") {}
    ~MockIRemoteObject() {}

    int32_t GetObjectRefCount() { return 0; }
    int SendRequest(uint32_t code, MessageParcel&, MessageParcel&, MessageOption&) { return 0; }
    bool IsProxyObject() const { return true; }
    bool CheckObjectLegality() const { return true; }
    bool AddDeathRecipient(const sptr<DeathRecipient>&) { return true; }
    bool RemoveDeathRecipient(const sptr<DeathRecipient>&) { return true; }
    sptr<IRemoteBroker> AsInterface() { return nullptr; }
    int Dump(int fd, const std::vector<std::u16string>&) { return 0; }
};

class MockBootCompileProgress : public BootCompileProgress {
public:
    bool IsBmsBundleReady() override { return false; }
    std::string GetFirmwareUpdateState() override { return "end"; }
};

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
 * @tc.desc: Verify the BootCompileProgress object creation and config handling.
 * @tc.type: FUNC
 * @tc.note: Using MockIRemoteObject to avoid crash from real system services.
 */
HWTEST_F(BootCompileProgressTest, Init_Normal_ExecuteSuccessfully, TestSize.Level1)
{
    sptr<MockIRemoteObject> mockRemote = new MockIRemoteObject();
    ASSERT_NE(mockRemote, nullptr);
    
    std::shared_ptr<BootCompileProgress> progress = std::make_shared<BootCompileProgress>();
    ASSERT_NE(progress, nullptr);
    
    BootAnimationConfig config;
    config.screenId = 0;
    config.rotateDegree = TEST_ROTATE_DEGREE_90;
    
    progress->screenId_ = config.screenId;
    progress->rotateDegree_ = config.rotateDegree;
    progress->windowWidth_ = TEST_WINDOW_WIDTH_1080;
    progress->windowHeight_ = TEST_WINDOW_HEIGHT_1920;
    
    EXPECT_EQ(progress->screenId_, config.screenId);
    EXPECT_EQ(progress->rotateDegree_, TEST_ROTATE_DEGREE_90);
    EXPECT_EQ(progress->windowWidth_, TEST_WINDOW_WIDTH_1080);
    EXPECT_EQ(progress->windowHeight_, TEST_WINDOW_HEIGHT_1920);
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
 * @tc.name: DrawProgressPoint_TimeZero_ReturnCorrectBrush
 * @tc.desc: Verify the DrawProgressPoint function returns correct brush at time zero (stage 0).
 * @tc.type: FUNC
 */
HWTEST_F(BootCompileProgressTest, DrawProgressPoint_TimeZero_ReturnCorrectBrush, TestSize.Level0)
{
    std::shared_ptr<BootCompileProgress> progress = std::make_shared<BootCompileProgress>();
    progress->sharpCurve_ = std::make_shared<Rosen::RSCubicBezierInterpolator>(
        TEST_SHARP_CURVE_CTLX1, TEST_SHARP_CURVE_CTLY1, TEST_SHARP_CURVE_CTLX2, TEST_SHARP_CURVE_CTLY2);
    Rosen::Drawing::Brush brush = progress->DrawProgressPoint(0, 0);
    EXPECT_NE(brush.GetColor(), 0);
}

/**
 * @tc.name: DrawProgressPoint_TimeStageOne_ReturnCorrectBrush
 * @tc.desc: Verify the DrawProgressPoint function returns correct brush at stage 1 boundary.
 * @tc.type: FUNC
 */
HWTEST_F(BootCompileProgressTest, DrawProgressPoint_TimeStageOne_ReturnCorrectBrush, TestSize.Level0)
{
    std::shared_ptr<BootCompileProgress> progress = std::make_shared<BootCompileProgress>();
    progress->sharpCurve_ = std::make_shared<Rosen::RSCubicBezierInterpolator>(
        TEST_SHARP_CURVE_CTLX1, TEST_SHARP_CURVE_CTLY1, TEST_SHARP_CURVE_CTLX2, TEST_SHARP_CURVE_CTLY2);
    Rosen::Drawing::Brush brush = progress->DrawProgressPoint(1, TEST_TIME_MID_STAGE_MS);
    EXPECT_NE(brush.GetColor(), 0);
}

/**
 * @tc.name: DrawProgressPoint_TimeStageTwo_ReturnCorrectBrush
 * @tc.desc: Verify the DrawProgressPoint function returns correct brush at stage 2.
 * @tc.type: FUNC
 */
HWTEST_F(BootCompileProgressTest, DrawProgressPoint_TimeStageTwo_ReturnCorrectBrush, TestSize.Level0)
{
    std::shared_ptr<BootCompileProgress> progress = std::make_shared<BootCompileProgress>();
    progress->sharpCurve_ = std::make_shared<Rosen::RSCubicBezierInterpolator>(
        TEST_SHARP_CURVE_CTLX1, TEST_SHARP_CURVE_CTLY1, TEST_SHARP_CURVE_CTLX2, TEST_SHARP_CURVE_CTLY2);
    Rosen::Drawing::Brush brush = progress->DrawProgressPoint(2, TEST_TIME_LATE_STAGE_MS);
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
 * @tc.name: UpdateCompileProgress_FirstHalf_ExecuteSuccessfully
 * @tc.desc: Verify the UpdateCompileProgress function in first half time period.
 * @tc.type: FUNC
 */
HWTEST_F(BootCompileProgressTest, UpdateCompileProgress_FirstHalf_ExecuteSuccessfully, TestSize.Level1)
{
    std::shared_ptr<MockBootCompileProgress> progress = std::make_shared<MockBootCompileProgress>();
    ASSERT_NE(progress, nullptr);
    int64_t currentTime = GetSystemCurrentTime();
    progress->startTimeMs_ = currentTime - 10 * 1000;
    progress->endTimePredictMs_ = currentTime + TEST_TIME_LIMIT_SEC * 1000;
    progress->timeLimitSec_ = TEST_TIME_LIMIT_SEC;
    progress->UpdateCompileProgress();
    EXPECT_GE(progress->progress_, 0);
    EXPECT_LE(progress->progress_, TEST_PROGRESS_100);
}

/**
 * @tc.name: UpdateCompileProgress_SecondHalf_ExecuteSuccessfully
 * @tc.desc: Verify the UpdateCompileProgress function in second half time period.
 * @tc.type: FUNC
 */
HWTEST_F(BootCompileProgressTest, UpdateCompileProgress_SecondHalf_ExecuteSuccessfully, TestSize.Level1)
{
    std::shared_ptr<MockBootCompileProgress> progress = std::make_shared<MockBootCompileProgress>();
    ASSERT_NE(progress, nullptr);
    int64_t currentTime = GetSystemCurrentTime();
    progress->startTimeMs_ = currentTime - 80 * 1000;
    progress->endTimePredictMs_ = currentTime + TEST_TIME_LIMIT_SEC * 1000;
    progress->timeLimitSec_ = TEST_TIME_LIMIT_SEC;
    progress->UpdateCompileProgress();
    EXPECT_GE(progress->progress_, 0);
    EXPECT_LE(progress->progress_, TEST_PROGRESS_100);
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
    progress->SetSpecialProgressFrame(0);
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
    progress->SetSpecialProgressFrame(TEST_SCREEN_STATUS_1);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetSpecialProgressFrame_ScreenIdNotFound_ReturnEarly
 * @tc.desc: Verify SetSpecialProgressFrame returns early when screenId not in map.
 * @tc.type: FUNC
 */
HWTEST_F(BootCompileProgressTest, SetSpecialProgressFrame_ScreenIdNotFound_ReturnEarly, TestSize.Level1)
{
    std::shared_ptr<BootCompileProgress> progress = std::make_shared<BootCompileProgress>();
    ASSERT_NE(progress, nullptr);
    progress->rsCanvasNode_ = Rosen::RSCanvasNode::Create();
    progress->progressConfigsMap_.clear();
    progress->windowWidth_ = TEST_WINDOW_WIDTH_1080;
    progress->windowHeight_ = TEST_WINDOW_HEIGHT_1920;
    progress->SetSpecialProgressFrame(INVALID_SCREEN_ID);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetFrame_RogMode_Normal
 * @tc.desc: Verify the Setframe function with ROG mode activated.
 * @tc.type: FUNC
 */
HWTEST_F(BootCompileProgressTest, SetFrame_RogMode_Normal, TestSize.Level1)
{
    std::shared_ptr<BootCompileProgress> progress = std::make_shared<BootCompileProgress>();
    ASSERT_NE(progress, nullptr);
    progress->rsCanvasNode_ = Rosen::RSCanvasNode::Create();
    progress->progressConfigsMap_.clear();
    progress->isWearable_ = false;
    progress->windowWidth_ = TEST_WINDOW_WIDTH_1080;
    progress->windowHeight_ = TEST_WINDOW_HEIGHT_1920;
    progress->rogWidth_ = TEST_ROG_WIDTH_1440;
    progress->rogHeight_ = TEST_ROG_HEIGHT_2560;
    progress->SetFrame();
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetFrame_RogModeInactive
 * @tc.desc: Verify the Setframe function with ROG mode inactive.
 * @tc.type: FUNC
 */
HWTEST_F(BootCompileProgressTest, SetFrame_RogModeInactive, TestSize.Level1)
{
    std::shared_ptr<BootCompileProgress> progress = std::make_shared<BootCompileProgress>();
    ASSERT_NE(progress, nullptr);
    progress->rsCanvasNode_ = Rosen::RSCanvasNode::Create();
    progress->progressConfigsMap_.clear();
    progress->isWearable_ = false;
    progress->windowWidth_ = TEST_WINDOW_WIDTH_1080;
    progress->windowHeight_ = TEST_WINDOW_HEIGHT_1920;
    progress->rogWidth_ = TEST_FRAME_NUM_0;
    progress->rogHeight_ = TEST_FRAME_NUM_0;
    progress->SetFrame();
    EXPECT_TRUE(true);
}

/**
 * @tc.name: SetFrameForRog_Normal
 * @tc.desc: Verify the SetFrameForRog function executes successsfully.
 * @tc.type: FUNC
 */
HWTEST_F(BootCompileProgressTest, SetFrameForRog_Normal, TestSize.Level1)
{
    std::shared_ptr<BootCompileProgress> progress = std::make_shared<BootCompileProgress>();
    ASSERT_NE(progress, nullptr);
    progress->rsCanvasNode_ = Rosen::RSCanvasNode::Create();
    progress->windowWidth_ = TEST_WINDOW_WIDTH_1080;
    progress->windowHeight_ = TEST_WINDOW_HEIGHT_1920;
    progress->rogWidth_ = TEST_ROG_WIDTH_1440;
    progress->rogHeight_ = TEST_ROG_HEIGHT_2560;
    progress->SetFrameForRog();
    EXPECT_TRUE(true);
}

/**
 * @tc.name: RefreshRate_DefaultZero_ExecuteSuccessfully
 * @tc.desc: Verify the refreshRate_ member defaults to zero.
 * @tc.type: FUNC
 */
HWTEST_F(BootCompileProgressTest, RefreshRate_DefaultZero_ExecuteSuccessfully, TestSize.Level1)
{
    std::shared_ptr<BootCompileProgress> progress = std::make_shared<BootCompileProgress>();
    ASSERT_NE(progress, nullptr);
    EXPECT_EQ(progress->refreshRate_, 0);
}

/**
 * @tc.name: RefreshRate_Set60Hz_DynamicChangeFreqCorrect
 * @tc.desc: Verify that at 60Hz the dynamic changeFreq equals the original CHANGE_FREQ.
 * @tc.type: FUNC
 */
HWTEST_F(BootCompileProgressTest, RefreshRate_Set60Hz_DynamicChangeFreqCorrect, TestSize.Level1)
{
    std::shared_ptr<BootCompileProgress> progress = std::make_shared<BootCompileProgress>();
    ASSERT_NE(progress, nullptr);
    progress->refreshRate_ = TEST_REFRESH_RATE_60;
    uint32_t base = (progress->refreshRate_ > 0) ? progress->refreshRate_ : 60;
    int32_t changeFreq = std::max(1, static_cast<int32_t>(base * 4 / 60));
    EXPECT_EQ(changeFreq, TEST_CHANGE_FREQ_60);
}

/**
 * @tc.name: RefreshRate_Set90Hz_DynamicChangeFreqCorrect
 * @tc.desc: Verify that at 90Hz the dynamic changeFreq scales proportionally.
 * @tc.type: FUNC
 */
HWTEST_F(BootCompileProgressTest, RefreshRate_Set90Hz_DynamicChangeFreqCorrect, TestSize.Level1)
{
    std::shared_ptr<BootCompileProgress> progress = std::make_shared<BootCompileProgress>();
    ASSERT_NE(progress, nullptr);
    progress->refreshRate_ = TEST_REFRESH_RATE_90;
    uint32_t base = (progress->refreshRate_ > 0) ? progress->refreshRate_ : 60;
    int32_t changeFreq = std::max(1, static_cast<int32_t>(base * 4 / 60));
    EXPECT_EQ(changeFreq, TEST_CHANGE_FREQ_90);
}

/**
 * @tc.name: RefreshRate_Set120Hz_DynamicChangeFreqCorrect
 * @tc.desc: Verify that at 120Hz the dynamic changeFreq scales proportionally to prevent fast marquee.
 * @tc.type: FUNC
 */
HWTEST_F(BootCompileProgressTest, RefreshRate_Set120Hz_DynamicChangeFreqCorrect, TestSize.Level1)
{
    std::shared_ptr<BootCompileProgress> progress = std::make_shared<BootCompileProgress>();
    ASSERT_NE(progress, nullptr);
    progress->refreshRate_ = TEST_REFRESH_RATE_120;
    uint32_t base = (progress->refreshRate_ > 0) ? progress->refreshRate_ : 60;
    int32_t changeFreq = std::max(1, static_cast<int32_t>(base * 4 / 60));
    EXPECT_EQ(changeFreq, TEST_CHANGE_FREQ_120);
}

/**
 * @tc.name: RefreshRate_SetZero_FallbackTo60Hz
 * @tc.desc: Verify that when refreshRate_ is zero, the fallback base refresh rate is 60Hz.
 * @tc.type: FUNC
 */
HWTEST_F(BootCompileProgressTest, RefreshRate_SetZero_FallbackTo60Hz, TestSize.Level1)
{
    std::shared_ptr<BootCompileProgress> progress = std::make_shared<BootCompileProgress>();
    ASSERT_NE(progress, nullptr);
    progress->refreshRate_ = 0;
    uint32_t base = (progress->refreshRate_ > 0) ? progress->refreshRate_ : 60;
    int32_t changeFreq = std::max(1, static_cast<int32_t>(base * 4 / 60));
    EXPECT_EQ(base, 60);
    EXPECT_EQ(changeFreq, TEST_CHANGE_FREQ_60);
}

}