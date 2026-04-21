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
#include <test_header.h>

#include "frame_collector.h"
#include "frame_painter.h"
#include "draw/canvas.h"
#include "image/bitmap.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class FramePainterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void FramePainterTest::SetUpTestCase() {}
void FramePainterTest::TearDownTestCase() {}
void FramePainterTest::SetUp() {}
void FramePainterTest::TearDown() {}

/**
 * @tc.name: Create001
 * @tc.desc: test
 * @tc.type:FUNC
 * @tc.require:
 */
HWTEST_F(FramePainterTest, Create001, TestSize.Level1)
{
    FramePainter* painter_ptr = new FramePainter(FrameCollector::GetInstance());
    ASSERT_TRUE(painter_ptr != nullptr);
}

/**
 * @tc.name: FramePainterTest
 * @tc.desc: Test RsNodeCostManagerTest.SwitchFunction2
 * @tc.type: FUNC
 * @tc.require: I6R1BQ
 */
HWTEST_F(FramePainterTest, DrawTest1, TestSize.Level1)
{
    FrameCollector& controller = FrameCollector::GetInstance();
    controller.SetEnabled(true);
    auto framePainter = std::make_shared<FramePainter>(controller);
    Drawing::Bitmap bitmap;
    Drawing::Canvas canvas;
    canvas.Bind(bitmap);
    framePainter->Draw(canvas);
    ASSERT_TRUE(controller.IsEnabled());
}

/**
 * @tc.name: FramePainterTest
 * @tc.desc: Test RsNodeCostManagerTest.SwitchFunction2
 * @tc.type: FUNC
 * @tc.require: I6R1BQ
 */
HWTEST_F(FramePainterTest, DrawTest2, TestSize.Level1)
{
    FrameCollector& controller = FrameCollector::GetInstance();
    controller.SetEnabled(false);
    auto framePainter = std::make_shared<FramePainter>(controller);
    Drawing::Bitmap bitmap;
    Drawing::Canvas canvas;
    canvas.Bind(bitmap);
    framePainter->Draw(canvas);
    ASSERT_FALSE(controller.IsEnabled());
}

/**
 * @tc.name: FramePainterTest
 * @tc.desc: Test RsNodeCostManagerTest.DrawFPSLine
 * @tc.type: FUNC
 * @tc.require: I6R1BQ
 */
HWTEST_F(FramePainterTest, DrawFPSLine1, TestSize.Level1)
{
    FrameCollector& controller = FrameCollector::GetInstance();
    controller.SetEnabled(false);
    auto framePainter = std::make_shared<FramePainter>(controller);
    Drawing::Bitmap bitmap;
    Drawing::Canvas canvas;
    canvas.Bind(bitmap);
    framePainter->DrawFPSLine(canvas, 0, 2.0f, 0xbf00ff00);
    ASSERT_FALSE(controller.IsEnabled());
}

/**
 * @tc.name: FramePainterTest
 * @tc.desc: Test RsNodeCostManagerTest.DrawFPSLine
 * @tc.type: FUNC
 * @tc.require: I6R1BQ
 */
HWTEST_F(FramePainterTest, DrawFPSLine2, TestSize.Level1)
{
    FrameCollector& controller = FrameCollector::GetInstance();
    controller.SetEnabled(false);
    auto framePainter = std::make_shared<FramePainter>(controller);
    Drawing::Bitmap bitmap;
    Drawing::Canvas canvas;
    canvas.Bind(bitmap);
    framePainter->DrawFPSLine(canvas, 1, 2.0f, 0xbf00ff00);
    ASSERT_FALSE(controller.IsEnabled());
}

/**
 * @tc.name: FramePainterTest
 * @tc.desc: Test RsNodeCostManagerTest.GenerateTimeBars
 * @tc.type: FUNC
 * @tc.require: I6R1BQ
 */
HWTEST_F(FramePainterTest, GenerateTimeBars, TestSize.Level1)
{
    FrameCollector& controller = FrameCollector::GetInstance();
    controller.SetEnabled(false);
    auto framePainter = std::make_shared<FramePainter>(controller);
    framePainter->GenerateTimeBars(1600, 800, 1);
    ASSERT_FALSE(controller.IsEnabled());
}

/**
 * @tc.name: FramePainterTest
 * @tc.desc: Test RsNodeCostManagerTest.GenerateTimeBars
 * @tc.type: FUNC
 * @tc.require: I6R1BQ
 */
HWTEST_F(FramePainterTest, SumTimesInMs, TestSize.Level1)
{
    FrameCollector& controller = FrameCollector::GetInstance();
    controller.SetEnabled(false);
    auto framePainter = std::make_shared<FramePainter>(controller);
    FrameInfo info;
    double result = framePainter->SumTimesInMs(info);
    ASSERT_FALSE(controller.IsEnabled());
    ASSERT_EQ(result, 0);
}

/**
 * @tc.name: GenerateTimeBars_FpsZero
 * @tc.desc: Test GenerateTimeBars with fps=0 (isHeavy should be false)
 * @tc.type: FUNC
 * @tc.require: I6R1BQ
 */
HWTEST_F(FramePainterTest, GenerateTimeBars_FpsZero, TestSize.Level1)
{
    FrameCollector& controller = FrameCollector::GetInstance();
    controller.SetEnabled(true);
    controller.ClearEvents();
    auto framePainter = std::make_shared<FramePainter>(controller);
    auto bars = framePainter->GenerateTimeBars(1600, 800, 0);
    ASSERT_TRUE(controller.IsEnabled());
}

/**
 * @tc.name: GenerateTimeBars_WithFrameData
 * @tc.desc: Test GenerateTimeBars with frame data containing time events
 * @tc.type: FUNC
 * @tc.require: I6R1BQ
 */
HWTEST_F(FramePainterTest, GenerateTimeBars_WithFrameData, TestSize.Level1)
{
    FrameCollector& controller = FrameCollector::GetInstance();
    controller.SetEnabled(true);
    controller.ClearEvents();
    auto framePainter = std::make_shared<FramePainter>(controller);
    int64_t baseTime = 1000000000;
    controller.MarkFrameEvent(FrameEventType::AnimateStart, baseTime);
    controller.MarkFrameEvent(FrameEventType::AnimateEnd, baseTime + 16000000);
    controller.MarkFrameEvent(FrameEventType::BuildStart, baseTime + 20000000);
    controller.MarkFrameEvent(FrameEventType::BuildEnd, baseTime + 36000000);
    controller.MarkFrameEvent(FrameEventType::LayoutStart, baseTime + 40000000);
    controller.MarkFrameEvent(FrameEventType::LayoutEnd, baseTime + 56000000);
    controller.MarkFrameEvent(FrameEventType::DrawStart, baseTime + 60000000);
    controller.MarkFrameEvent(FrameEventType::DrawEnd, baseTime + 76000000);
    controller.MarkFrameEvent(FrameEventType::WaitVsyncStart, baseTime + 80000000);
    controller.MarkFrameEvent(FrameEventType::WaitVsyncEnd, baseTime + 96000000);
    controller.MarkFrameEvent(FrameEventType::FlushStart, baseTime + 100000000);
    controller.MarkFrameEvent(FrameEventType::FlushEnd, baseTime + 116000000);
    controller.MarkFrameEvent(FrameEventType::ReleaseEnd, baseTime + 130000000);
    auto bars = framePainter->GenerateTimeBars(1600, 800, 60);
    ASSERT_TRUE(bars.size() > 0);
}

/**
 * @tc.name: GenerateTimeBars_HeavyFrame
 * @tc.desc: Test GenerateTimeBars with heavy frame (exceeds 1/fps threshold)
 * @tc.type: FUNC
 * @tc.require: I6R1BQ
 */
HWTEST_F(FramePainterTest, GenerateTimeBars_HeavyFrame, TestSize.Level1)
{
    FrameCollector& controller = FrameCollector::GetInstance();
    controller.SetEnabled(true);
    controller.ClearEvents();
    auto framePainter = std::make_shared<FramePainter>(controller);
    int64_t baseTime = 1000000000;
    controller.MarkFrameEvent(FrameEventType::AnimateStart, baseTime);
    controller.MarkFrameEvent(FrameEventType::AnimateEnd, baseTime + 20000000);
    auto bars = framePainter->GenerateTimeBars(1600, 800, 60);
    ASSERT_TRUE(controller.IsEnabled());
}

/**
 * @tc.name: GenerateTimeBars_ZeroDiffMs
 * @tc.desc: Test GenerateTimeBars with zero time difference (diffMs == 0)
 * @tc.type: FUNC
 * @tc.require: I6R1BQ
 */
HWTEST_F(FramePainterTest, GenerateTimeBars_ZeroDiffMs, TestSize.Level1)
{
    FrameCollector& controller = FrameCollector::GetInstance();
    controller.SetEnabled(true);
    controller.ClearEvents();
    auto framePainter = std::make_shared<FramePainter>(controller);
    int64_t baseTime = 1000000000;
    controller.MarkFrameEvent(FrameEventType::AnimateStart, baseTime);
    controller.MarkFrameEvent(FrameEventType::AnimateEnd, baseTime);
    auto bars = framePainter->GenerateTimeBars(1600, 800, 60);
    ASSERT_TRUE(controller.IsEnabled());
}

/**
 * @tc.name: GenerateTimeBars_NegativeDiffMs
 * @tc.desc: Test GenerateTimeBars with negative time difference (diffMs < 0)
 * @tc.type: FUNC
 * @tc.require: I6R1BQ
 */
HWTEST_F(FramePainterTest, GenerateTimeBars_NegativeDiffMs, TestSize.Level1)
{
    FrameCollector& controller = FrameCollector::GetInstance();
    controller.SetEnabled(true);
    controller.ClearEvents();
    auto framePainter = std::make_shared<FramePainter>(controller);
    controller.UnlockFrameQueue();
    FrameInfo info;
    info.frameNumber = 1;
    info.times[static_cast<size_t>(FrameEventType::AnimateStart)] = 2000000000;
    info.times[static_cast<size_t>(FrameEventType::AnimateEnd)] = 1000000000; // End before start
    auto bars = framePainter->GenerateTimeBars(1600, 800, 60);
    ASSERT_TRUE(controller.IsEnabled());
}

/**
 * @tc.name: SumTimesInMs_PositiveDiff
 * @tc.desc: Test SumTimesInMs with positive time differences
 * @tc.type: FUNC
 * @tc.require: I6R1BQ
 */
HWTEST_F(FramePainterTest, SumTimesInMs_PositiveDiff, TestSize.Level1)
{
    FrameCollector& controller = FrameCollector::GetInstance();
    controller.SetEnabled(false);
    auto framePainter = std::make_shared<FramePainter>(controller);
    FrameInfo info;
    info.frameNumber = 1;
    int64_t baseTime = 1000000000;
    info.times[static_cast<size_t>(FrameEventType::AnimateStart)] = baseTime;
    info.times[static_cast<size_t>(FrameEventType::AnimateEnd)] = baseTime + 10000000; // 10ms
    info.times[static_cast<size_t>(FrameEventType::BuildStart)] = baseTime + 20000000;
    info.times[static_cast<size_t>(FrameEventType::BuildEnd)] = baseTime + 35000000; // 15ms
    info.times[static_cast<size_t>(FrameEventType::LayoutStart)] = baseTime + 40000000;
    info.times[static_cast<size_t>(FrameEventType::LayoutEnd)] = baseTime + 50000000; // 10ms
    double result = framePainter->SumTimesInMs(info);
    EXPECT_NEAR(result, 35.0, 1.0);
}

/**
 * @tc.name: SumTimesInMs_NegativeDiff
 * @tc.desc: Test SumTimesInMs with negative time differences (should be skipped)
 * @tc.type: FUNC
 * @tc.require: I6R1BQ
 */
HWTEST_F(FramePainterTest, SumTimesInMs_NegativeDiff, TestSize.Level1)
{
    FrameCollector& controller = FrameCollector::GetInstance();
    controller.SetEnabled(false);
    auto framePainter = std::make_shared<FramePainter>(controller);
    FrameInfo info;
    info.frameNumber = 1;
    info.times[static_cast<size_t>(FrameEventType::AnimateStart)] = 2000000000;
    info.times[static_cast<size_t>(FrameEventType::AnimateEnd)] = 1000000000; // Negative diff
    double result = framePainter->SumTimesInMs(info);
    EXPECT_EQ(result, 0.0);
}

/**
 * @tc.name: SumTimesInMs_ZeroDiff
 * @tc.desc: Test SumTimesInMs with zero time differences (should be skipped)
 * @tc.type: FUNC
 * @tc.require: I6R1BQ
 */
HWTEST_F(FramePainterTest, SumTimesInMs_ZeroDiff, TestSize.Level1)
{
    FrameCollector& controller = FrameCollector::GetInstance();
    controller.SetEnabled(false);
    auto framePainter = std::make_shared<FramePainter>(controller);
    FrameInfo info;
    info.frameNumber = 1;
    int64_t sameTime = 1000000000;
    info.times[static_cast<size_t>(FrameEventType::AnimateStart)] = sameTime;
    info.times[static_cast<size_t>(FrameEventType::AnimateEnd)] = sameTime;
    info.times[static_cast<size_t>(FrameEventType::BuildStart)] = sameTime;
    info.times[static_cast<size_t>(FrameEventType::BuildEnd)] = sameTime;
    double result = framePainter->SumTimesInMs(info);
    EXPECT_EQ(result, 0.0);
}

/**
 * @tc.name: SumTimesInMs_MixedDiffs
 * @tc.desc: Test SumTimesInMs with mixed positive, negative, and zero diffs
 * @tc.type: FUNC
 * @tc.require: I6R1BQ
 */
HWTEST_F(FramePainterTest, SumTimesInMs_MixedDiffs, TestSize.Level1)
{
    FrameCollector& controller = FrameCollector::GetInstance();
    controller.SetEnabled(false);
    auto framePainter = std::make_shared<FramePainter>(controller);
    FrameInfo info;
    info.frameNumber = 1;
    int64_t baseTime = 1000000000;
    info.times[static_cast<size_t>(FrameEventType::AnimateStart)] = baseTime;
    info.times[static_cast<size_t>(FrameEventType::AnimateEnd)] = baseTime + 10000000;
    info.times[static_cast<size_t>(FrameEventType::BuildStart)] = baseTime;
    info.times[static_cast<size_t>(FrameEventType::BuildEnd)] = baseTime;
    info.times[static_cast<size_t>(FrameEventType::LayoutStart)] = baseTime + 20000000;
    info.times[static_cast<size_t>(FrameEventType::LayoutEnd)] = baseTime + 10000000;
    info.times[static_cast<size_t>(FrameEventType::DrawStart)] = baseTime + 30000000;
    info.times[static_cast<size_t>(FrameEventType::DrawEnd)] = baseTime + 35000000;
    double result = framePainter->SumTimesInMs(info);
    EXPECT_NEAR(result, 15.0, 1.0);
}

/**
 * @tc.name: Draw_WithEnabledAndCanvas
 * @tc.desc: Test Draw with enabled collector and valid canvas dimensions
 * @tc.type: FUNC
 * @tc.require: I6R1BQ
 */
HWTEST_F(FramePainterTest, Draw_WithEnabledAndCanvas, TestSize.Level1)
{
    FrameCollector& controller = FrameCollector::GetInstance();
    controller.SetEnabled(true);
    controller.ClearEvents();
    auto framePainter = std::make_shared<FramePainter>(controller);
    int64_t baseTime = 1000000000;
    controller.MarkFrameEvent(FrameEventType::AnimateStart, baseTime);
    controller.MarkFrameEvent(FrameEventType::AnimateEnd, baseTime + 10000000);
    Drawing::BitmapFormat format = { Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL };
    Drawing::Bitmap bitmap;
    bitmap.Build(100, 100, format);
    Drawing::Canvas canvas;
    canvas.Bind(bitmap);
    framePainter->Draw(canvas);
    ASSERT_TRUE(controller.IsEnabled());
}

/**
 * @tc.name: DrawFPSLine_WithValidParams
 * @tc.desc: Test DrawFPSLine with various valid parameters
 * @tc.type: FUNC
 * @tc.require: I6R1BQ
 */
HWTEST_F(FramePainterTest, DrawFPSLine_WithValidParams, TestSize.Level1)
{
    FrameCollector& controller = FrameCollector::GetInstance();
    controller.SetEnabled(false);
    auto framePainter = std::make_shared<FramePainter>(controller);
    Drawing::BitmapFormat format = { Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL };
    Drawing::Bitmap bitmap;
    bitmap.Build(100, 100, format);
    Drawing::Canvas canvas;
    canvas.Bind(bitmap);
    framePainter->DrawFPSLine(canvas, 30, 2.0, 0xbf00ff00);
    framePainter->DrawFPSLine(canvas, 60, 4.0, 0xbfff0000);
    framePainter->DrawFPSLine(canvas, 120, 1.0, 0xbf0000ff);
    ASSERT_FALSE(controller.IsEnabled());
}

/**
 * @tc.name: GenerateTimeBars_MultipleFrames
 * @tc.desc: Test GenerateTimeBars with multiple frames in queue
 * @tc.type: FUNC
 * @tc.require: I6R1BQ
 */
HWTEST_F(FramePainterTest, GenerateTimeBars_MultipleFrames, TestSize.Level1)
{
    FrameCollector& controller = FrameCollector::GetInstance();
    controller.SetEnabled(true);
    controller.ClearEvents();
    auto framePainter = std::make_shared<FramePainter>(controller);
    int64_t baseTime = 1000000000;
    for (int i = 0; i < 5; i++) {
        controller.MarkFrameEvent(FrameEventType::AnimateStart, baseTime + i * 50000000);
        controller.MarkFrameEvent(FrameEventType::AnimateEnd, baseTime + i * 50000000 + 16000000);
    }
    auto bars = framePainter->GenerateTimeBars(1600, 800, 60);
    ASSERT_TRUE(controller.IsEnabled());
}

/**
 * @tc.name: GenerateTimeBars_SmallDimensions
 * @tc.desc: Test GenerateTimeBars with small canvas dimensions
 * @tc.type: FUNC
 * @tc.require: I6R1BQ
 */
HWTEST_F(FramePainterTest, GenerateTimeBars_SmallDimensions, TestSize.Level1)
{
    FrameCollector& controller = FrameCollector::GetInstance();
    controller.SetEnabled(true);
    controller.ClearEvents();
    auto framePainter = std::make_shared<FramePainter>(controller);
    auto bars = framePainter->GenerateTimeBars(10, 10, 60);
    ASSERT_TRUE(controller.IsEnabled());
}

/**
 * @tc.name: GenerateTimeBars_LargeDimensions
 * @tc.desc: Test GenerateTimeBars with large canvas dimensions
 * @tc.type: FUNC
 * @tc.require: I6R1BQ
 */
HWTEST_F(FramePainterTest, GenerateTimeBars_LargeDimensions, TestSize.Level1)
{
    FrameCollector& controller = FrameCollector::GetInstance();
    controller.SetEnabled(true);
    controller.ClearEvents();
    auto framePainter = std::make_shared<FramePainter>(controller);
    int64_t baseTime = 1000000000;
    controller.MarkFrameEvent(FrameEventType::AnimateStart, baseTime);
    controller.MarkFrameEvent(FrameEventType::AnimateEnd, baseTime + 10000000);
    auto bars = framePainter->GenerateTimeBars(4096, 2160, 60);
    ASSERT_TRUE(controller.IsEnabled());
}

/**
 * @tc.name: SumTimesInMs_AllEventTypes
 * @tc.desc: Test SumTimesInMs with all valid event types
 * @tc.type: FUNC
 * @tc.require: I6R1BQ
 */
HWTEST_F(FramePainterTest, SumTimesInMs_AllEventTypes, TestSize.Level1)
{
    FrameCollector& controller = FrameCollector::GetInstance();
    controller.SetEnabled(false);
    auto framePainter = std::make_shared<FramePainter>(controller);
    FrameInfo info;
    info.frameNumber = 1;
    int64_t baseTime = 1000000000;
    int64_t offset = 0;
    info.times[static_cast<size_t>(FrameEventType::AnimateStart)] = baseTime + offset;
    info.times[static_cast<size_t>(FrameEventType::AnimateEnd)] = baseTime + offset + 5000000;
    offset += 10000000;
    info.times[static_cast<size_t>(FrameEventType::BuildStart)] = baseTime + offset;
    info.times[static_cast<size_t>(FrameEventType::BuildEnd)] = baseTime + offset + 5000000;
    offset += 10000000;
    info.times[static_cast<size_t>(FrameEventType::LayoutStart)] = baseTime + offset;
    info.times[static_cast<size_t>(FrameEventType::LayoutEnd)] = baseTime + offset + 5000000;
    offset += 10000000;
    info.times[static_cast<size_t>(FrameEventType::DrawStart)] = baseTime + offset;
    info.times[static_cast<size_t>(FrameEventType::DrawEnd)] = baseTime + offset + 5000000;
    offset += 10000000;
    info.times[static_cast<size_t>(FrameEventType::WaitVsyncStart)] = baseTime + offset;
    info.times[static_cast<size_t>(FrameEventType::WaitVsyncEnd)] = baseTime + offset + 5000000;
    offset += 10000000;
    info.times[static_cast<size_t>(FrameEventType::ReleaseStart)] = baseTime + offset;
    info.times[static_cast<size_t>(FrameEventType::ReleaseEnd)] = baseTime + offset + 5000000;
    offset += 10000000;
    info.times[static_cast<size_t>(FrameEventType::FlushStart)] = baseTime + offset;
    info.times[static_cast<size_t>(FrameEventType::FlushEnd)] = baseTime + offset + 5000000;
    double result = framePainter->SumTimesInMs(info);
    EXPECT_NEAR(result, 35.0, 1.0);
}

/**
 * @tc.name: GenerateTimeBars_FpsZeroWithFrameData
 * @tc.desc: Test GenerateTimeBars with fps=0 and non-empty queue (cover line 106 false branch)
 * @tc.type: FUNC
 * @tc.require: I6R1BQ
 */
HWTEST_F(FramePainterTest, GenerateTimeBars_FpsZeroWithFrameData, TestSize.Level1)
{
    FrameCollector& controller = FrameCollector::GetInstance();
    controller.SetEnabled(true);
    controller.ClearEvents();
    auto framePainter = std::make_shared<FramePainter>(controller);
    int64_t baseTime = 1000000000;
    controller.MarkFrameEvent(FrameEventType::AnimateStart, baseTime);
    controller.MarkFrameEvent(FrameEventType::AnimateEnd, baseTime + 10000000);
    controller.MarkFrameEvent(FrameEventType::DrawStart, baseTime + 20000000);
    controller.MarkFrameEvent(FrameEventType::DrawEnd, baseTime + 30000000);
    controller.MarkFrameEvent(FrameEventType::WaitVsyncStart, baseTime + 40000000);
    controller.MarkFrameEvent(FrameEventType::WaitVsyncEnd, baseTime + 50000000);
    controller.MarkFrameEvent(FrameEventType::ReleaseEnd, baseTime + 60000000);
    auto bars = framePainter->GenerateTimeBars(1600, 800, 0);
    ASSERT_TRUE(controller.IsEnabled());
}

/**
 * @tc.name: GenerateTimeBars_EventTypeNotInColorMap
 * @tc.desc: Test GenerateTimes iterates event types not in colorMap (cover line 115)
 * @tc.type: FUNC
 * @tc.require: I6R1BQ
 */
HWTEST_F(FramePainterTest, GenerateTimeBars_EventTypeNotInColorMap, TestSize.Level1)
{
    FrameCollector& controller = FrameCollector::GetInstance();
    controller.SetEnabled(true);
    controller.ClearEvents();
    auto framePainter = std::make_shared<FramePainter>(controller);
    int64_t baseTime = 1000000000;
    controller.MarkFrameEvent(FrameEventType::AnimateStart, baseTime);
    controller.MarkFrameEvent(FrameEventType::AnimateEnd, baseTime + 10000000);
    controller.MarkFrameEvent(FrameEventType::DrawEnd, baseTime + 20000000);
    controller.MarkFrameEvent(FrameEventType::WaitVsyncStart, baseTime + 30000000);
    controller.MarkFrameEvent(FrameEventType::WaitVsyncEnd, baseTime + 40000000);
    controller.MarkFrameEvent(FrameEventType::ReleaseEnd, baseTime + 50000000);
    auto bars = framePainter->GenerateTimeBars(1600, 800, 60);
    ASSERT_TRUE(bars.size() >= 2);
}

/**
 * @tc.name: GenerateTimeBars_NegativeDiffMsInQueue
 * @tc.desc: Test GenerateTimeBars with frame having negative diffMs in queue (cover line 122)
 * @tc.type: FUNC
 * @tc.require: I6R1BQ
 */
HWTEST_F(FramePainterTest, GenerateTimeBars_NegativeDiffMsInQueue, TestSize.Level1)
{
    FrameCollector& controller = FrameCollector::GetInstance();
    controller.SetEnabled(true);
    controller.ClearEvents();
    auto framePainter = std::make_shared<FramePainter>(controller);
    auto& frameQueue = const_cast<FrameInfoQueue&>(controller.LockGetFrameQueue());
    FrameInfo info;
    info.frameNumber = 1;
    info.times[static_cast<size_t>(FrameEventType::AnimateStart)] = 2000000000;
    info.times[static_cast<size_t>(FrameEventType::AnimateEnd)] = 1000000000;
    frameQueue.Push(info);
    controller.UnlockFrameQueue();
    auto bars = framePainter->GenerateTimeBars(1600, 800, 60);
    ASSERT_TRUE(controller.IsEnabled());
}

/**
 * @tc.name: GenerateTimeBars_ZeroDiffMsInQueue
 * @tc.desc: Test GenerateTimeBars with frame having zero diffMs in queue (cover line 125)
 * @tc.type: FUNC
 * @tc.require: I6R1BQ
 */
HWTEST_F(FramePainterTest, GenerateTimeBars_ZeroDiffMsInQueue, TestSize.Level1)
{
    FrameCollector& controller = FrameCollector::GetInstance();
    controller.SetEnabled(true);
    controller.ClearEvents();
    auto framePainter = std::make_shared<FramePainter>(controller);
    auto& frameQueue = const_cast<FrameInfoQueue&>(controller.LockGetFrameQueue());
    FrameInfo info;
    info.frameNumber = 1;
    int64_t sameTime = 1000000000;
    info.times[static_cast<size_t>(FrameEventType::AnimateStart)] = sameTime;
    info.times[static_cast<size_t>(FrameEventType::AnimateEnd)] = sameTime;
    frameQueue.Push(info);
    controller.UnlockFrameQueue();
    auto bars = framePainter->GenerateTimeBars(1600, 800, 60);
    ASSERT_TRUE(controller.IsEnabled());
}

} // namespace Rosen
} // namespace OHOS
