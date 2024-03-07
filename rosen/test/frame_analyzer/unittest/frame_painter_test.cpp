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

} // namespace Rosen
} // namespace OHOS
