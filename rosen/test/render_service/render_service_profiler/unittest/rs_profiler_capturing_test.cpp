/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gtest/gtest.h"
#include "recording/draw_cmd.h"
#include "rs_profiler.h"
#include "rs_profiler_capture_recorder.h"
#include "rs_profiler_command.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS::Rosen {

class RSProfilerCapturingTest : public testing::Test {
public:
    static void SetUpTestCase() {};
    static void TearDownTestCase() {};
    void SetUp() override {};
    void TearDown() override {};
};

/*
 * @tc.name: RSKeepDrawCmdTest
 * @tc.desc: Test setting the flag responsible for clearing/keeping drawCmdList in RSRenderNode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerCapturingTest, RSKeepDrawCmdTest, testing::ext::TestSize.Level1)
{
    RSProfiler::testing_ = true;
    EXPECT_NO_THROW({
        bool drawCmdSyncVar = false;
        // by default should be true
        RSProfiler::KeepDrawCmd(drawCmdSyncVar);
        EXPECT_TRUE(drawCmdSyncVar == true);
        // set to true via HRP command
        RSProfiler::SetRenderNodeKeepDrawCmd(true);
        RSProfiler::KeepDrawCmd(drawCmdSyncVar);
        EXPECT_TRUE(drawCmdSyncVar == false);
        // set to false via HRP command
        RSProfiler::SetRenderNodeKeepDrawCmd(false);
        RSProfiler::KeepDrawCmd(drawCmdSyncVar);
        EXPECT_TRUE(drawCmdSyncVar == true);
    });
}

/*
 * @tc.name: RSDrawingRedrawTest
 * @tc.desc: Test setting the flag responsible for clearing/keeping drawCmdList for DrawingCanvasNode
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerCapturingTest, RSDrawingRedrawTest, testing::ext::TestSize.Level1)
{
    RSProfiler::testing_ = true;
    EXPECT_NO_THROW({
        const auto res = 1000;
        auto drawCmdList =
            std::make_shared<Drawing::DrawCmdList>(res, res, Drawing::DrawCmdList::UnmarshalMode::DEFERRED);
        EXPECT_TRUE(drawCmdList != nullptr);
        const auto roundRect = Drawing::RoundRect(

        Drawing::Rect(0, 0, res, res), static_cast<float>(res), static_cast<float>(res));
        auto paint = Drawing::Paint(Drawing::Color::COLOR_RED);
        paint.SetStyle(Drawing::Paint::PaintStyle::PAINT_FILL);
        const auto drawRoundRect = std::make_shared<Drawing::DrawRoundRectOpItem>(roundRect, paint);
        drawCmdList->AddDrawOp(drawRoundRect);

        EXPECT_TRUE(drawCmdList->drawOpItems_.size() == 1);
        // set to true via HRP command
        RSProfiler::SetDrawingCanvasNodeRedraw(true);
        RSProfiler::DrawingNodeAddClearOp(drawCmdList);
        EXPECT_TRUE(drawCmdList->drawOpItems_.size() == 1); // should not clear
        // set to false via HRP command
        RSProfiler::SetDrawingCanvasNodeRedraw(false);
        RSProfiler::DrawingNodeAddClearOp(drawCmdList);
        EXPECT_TRUE(drawCmdList->drawOpItems_.size() == 0); // should clear
    });
}

/*
 * @tc.name: RSComponentCaptureTest
 * @tc.desc: Test triggering ComponentScreenshot capturing and getting recording canvas correctly
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerCapturingTest, RSComponentCaptureTest, testing::ext::TestSize.Level1)
{
    RSProfiler::testing_ = true;
    EXPECT_NO_THROW({
        const auto res = 1000.0f;
        // by default should return nullptr
        auto canvas = RSCaptureRecorder::GetInstance().TryComponentScreenshotCapture(res, res);
        EXPECT_TRUE(canvas == nullptr);
        RSCaptureRecorder::GetInstance().EndComponentScreenshotCapture();
        // set to true via HRP command
        RSCaptureRecorder::GetInstance().SetComponentScreenshotFlag(true);
        canvas = RSCaptureRecorder::GetInstance().TryComponentScreenshotCapture(res, res);
        EXPECT_TRUE(canvas != nullptr);
        RSCaptureRecorder::GetInstance().EndComponentScreenshotCapture(); // should return on skp saving
        // set to false via HRP command
        RSCaptureRecorder::GetInstance().SetComponentScreenshotFlag(false);
        canvas = RSCaptureRecorder::GetInstance().TryComponentScreenshotCapture(res, res);
        EXPECT_TRUE(canvas == nullptr);
        RSCaptureRecorder::GetInstance().EndComponentScreenshotCapture();
    });
}

/*
 * @tc.name: RSOffscreenCaptureTest
 * @tc.desc: Test workflow of offscreen CanvasNode capturing and getting recording canvas correctly
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerCapturingTest, RSOffscreenCaptureTest, testing::ext::TestSize.Level1)
{
    RSProfiler::testing_ = true;
    EXPECT_NO_THROW({
        const auto res = 1000.0f;
        auto canvas = RSCaptureRecorder::GetInstance().TryOffscreenCanvasCapture(res, res);
        EXPECT_TRUE(canvas != nullptr);
        RSCaptureRecorder::GetInstance().EndOffscreenCanvasCapture(); // should return on skp saving
    });
}

/*
 * @tc.name: RSDrawingCaptureTest
 * @tc.desc: Test triggering DrawingCanasNode capturing and getting recording canvas correctly
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSProfilerCapturingTest, RSDrawingCaptureTest, testing::ext::TestSize.Level1)
{
    RSProfiler::testing_ = true;
    EXPECT_NO_THROW({
        const auto res = 1000.0f;
        const uint64_t nodeId = 12345;
        // by default nodeId is 0, should return nullptr
        auto canvas = RSCaptureRecorder::GetInstance().TryDrawingCanvasCapture(res, res, nodeId);
        EXPECT_TRUE(canvas == nullptr);
        RSCaptureRecorder::GetInstance().EndDrawingCanvasCapture();
        // set nodeId to the same value as used for capturing
        RSCaptureRecorder::GetInstance().SetDrawingCanvasNodeId(nodeId);
        canvas = RSCaptureRecorder::GetInstance().TryDrawingCanvasCapture(res, res, nodeId);
        EXPECT_TRUE(canvas != nullptr);
        RSCaptureRecorder::GetInstance().EndDrawingCanvasCapture(); // should return on skp saving
        // invalidate nodeId
        RSCaptureRecorder::GetInstance().InvalidateDrawingCanvasNodeId();
        canvas = RSCaptureRecorder::GetInstance().TryDrawingCanvasCapture(res, res, nodeId);
        EXPECT_TRUE(canvas == nullptr);
        RSCaptureRecorder::GetInstance().EndDrawingCanvasCapture();
    });
}

HWTEST_F(RSProfilerCapturingTest, RSDefaultSetModeTest, testing::ext::TestSize.Level1)
{
    RSProfiler::testing_ = true;
    RSCaptureRecorder::testingTriggering_ = true;
    EXPECT_NO_THROW({
        const auto res = 1000.0f;
        // no argument corresponds to DEFAULT capturing mode
        RSProfiler::SaveSkpImgCache(ArgList(std::vector<std::string>{""}));
        auto canvas = RSCaptureRecorder::GetInstance().TryInstantCapture(res, res, SkpCaptureType::DEFAULT);
        EXPECT_TRUE(canvas != nullptr);
        // argument corresponding to DEFAULT capturing mode
        RSProfiler::SaveSkpImgCache(ArgList(std::vector<std::string>{"0"}));
        canvas = RSCaptureRecorder::GetInstance().TryInstantCapture(res, res, SkpCaptureType::DEFAULT);
        EXPECT_TRUE(canvas != nullptr);
        RSCaptureRecorder::GetInstance().EndInstantCapture(SkpCaptureType::DEFAULT);
    });
}

HWTEST_F(RSProfilerCapturingTest, RSImgCachedSetModeTest, testing::ext::TestSize.Level1)
{
    RSProfiler::testing_ = true;
    RSCaptureRecorder::testingTriggering_ = true;
    EXPECT_NO_THROW({
        const auto res = 1000.0f;
        RSProfiler::SaveSkpImgCache(ArgList(std::vector<std::string>{"123"})); // incorrect argument
        auto canvas = RSCaptureRecorder::GetInstance().TryInstantCapture(res, res, SkpCaptureType::IMG_CACHED);
        EXPECT_TRUE(canvas == nullptr);
        // argument corresponding to IMG_CACHED capturing mode
        RSProfiler::SaveSkpImgCache(ArgList(std::vector<std::string>{"1"}));
        canvas = RSCaptureRecorder::GetInstance().TryInstantCapture(res, res, SkpCaptureType::IMG_CACHED);
        EXPECT_TRUE(canvas != nullptr);
        RSCaptureRecorder::GetInstance().EndInstantCapture(SkpCaptureType::IMG_CACHED);
    });
}

HWTEST_F(RSProfilerCapturingTest, RSOnCaptureSetModeTest, testing::ext::TestSize.Level1)
{
    RSProfiler::testing_ = true;
    RSCaptureRecorder::testingTriggering_ = true;
    EXPECT_NO_THROW({
        const auto res = 1000.0f;
        RSProfiler::SaveSkpOnCapture(ArgList(std::vector<std::string>{"123"}));
        auto canvas = RSCaptureRecorder::GetInstance().TryInstantCapture(res, res, SkpCaptureType::ON_CAPTURE);
        EXPECT_TRUE(canvas == nullptr);
        RSProfiler::SaveSkpOnCapture(ArgList(std::vector<std::string>{})); // should not use any argument
        canvas = RSCaptureRecorder::GetInstance().TryInstantCapture(res, res, SkpCaptureType::ON_CAPTURE);
        EXPECT_TRUE(canvas != nullptr);
        RSCaptureRecorder::GetInstance().EndInstantCapture(SkpCaptureType::ON_CAPTURE);
    });
}

HWTEST_F(RSProfilerCapturingTest, RSExtendedSetModeTest, testing::ext::TestSize.Level1)
{
    RSProfiler::testing_ = true;
    RSCaptureRecorder::testingTriggering_ = true;
    EXPECT_NO_THROW({
        const auto res = 1000.0f;
        RSProfiler::SaveSkpExtended(ArgList(std::vector<std::string>{"123"}));
        auto canvas = RSCaptureRecorder::GetInstance().TryInstantCapture(res, res, SkpCaptureType::EXTENDED);
        EXPECT_TRUE(canvas == nullptr);
        RSProfiler::SaveSkpExtended(ArgList(std::vector<std::string>{})); // should not use any argument
        canvas = RSCaptureRecorder::GetInstance().TryInstantCapture(res, res, SkpCaptureType::EXTENDED);
        EXPECT_TRUE(canvas != nullptr);
        RSCaptureRecorder::GetInstance().EndInstantCapture(SkpCaptureType::EXTENDED);
    });
}

HWTEST_F(RSProfilerCapturingTest, RSSetModeMixedTest, testing::ext::TestSize.Level1)
{
    RSProfiler::testing_ = true;
    RSCaptureRecorder::testingTriggering_ = true;
    EXPECT_NO_THROW({
        const auto res = 1000.0f;
        RSProfiler::SaveSkpImgCache(ArgList(std::vector<std::string>{"0"}));
        // should not trigger any other capturing other than DEFAULT
        auto canvas = RSCaptureRecorder::GetInstance().TryInstantCapture(res, res, SkpCaptureType::IMG_CACHED);
        EXPECT_TRUE(canvas == nullptr);
        canvas = RSCaptureRecorder::GetInstance().TryInstantCapture(res, res, SkpCaptureType::ON_CAPTURE);
        EXPECT_TRUE(canvas == nullptr);
        canvas = RSCaptureRecorder::GetInstance().TryInstantCapture(res, res, SkpCaptureType::EXTENDED);
        EXPECT_TRUE(canvas == nullptr);
        canvas = RSCaptureRecorder::GetInstance().TryInstantCapture(res, res, SkpCaptureType::DEFAULT);
        EXPECT_TRUE(canvas != nullptr);
        RSCaptureRecorder::GetInstance().EndInstantCapture(SkpCaptureType::DEFAULT);

        RSProfiler::SaveSkpImgCache(ArgList(std::vector<std::string>{"1"}));
        // should not trigger any other capturing other than IMG_CACHED
        canvas = RSCaptureRecorder::GetInstance().TryInstantCapture(res, res, SkpCaptureType::DEFAULT);
        EXPECT_TRUE(canvas == nullptr);
        canvas = RSCaptureRecorder::GetInstance().TryInstantCapture(res, res, SkpCaptureType::ON_CAPTURE);
        EXPECT_TRUE(canvas == nullptr);
        canvas = RSCaptureRecorder::GetInstance().TryInstantCapture(res, res, SkpCaptureType::EXTENDED);
        EXPECT_TRUE(canvas == nullptr);
        canvas = RSCaptureRecorder::GetInstance().TryInstantCapture(res, res, SkpCaptureType::IMG_CACHED);
        EXPECT_TRUE(canvas != nullptr);
        RSCaptureRecorder::GetInstance().EndInstantCapture(SkpCaptureType::IMG_CACHED);

        RSProfiler::SaveSkpOnCapture(ArgList(std::vector<std::string>{}));
        // should not trigger any other capturing other than ON_CAPTURE
        canvas = RSCaptureRecorder::GetInstance().TryInstantCapture(res, res, SkpCaptureType::IMG_CACHED);
        EXPECT_TRUE(canvas == nullptr);
        canvas = RSCaptureRecorder::GetInstance().TryInstantCapture(res, res, SkpCaptureType::DEFAULT);
        EXPECT_TRUE(canvas == nullptr);
        canvas = RSCaptureRecorder::GetInstance().TryInstantCapture(res, res, SkpCaptureType::EXTENDED);
        EXPECT_TRUE(canvas == nullptr);
        canvas = RSCaptureRecorder::GetInstance().TryInstantCapture(res, res, SkpCaptureType::ON_CAPTURE);
        EXPECT_TRUE(canvas != nullptr);
        RSCaptureRecorder::GetInstance().EndInstantCapture(SkpCaptureType::ON_CAPTURE);

        RSProfiler::SaveSkpExtended(ArgList(std::vector<std::string>{}));
        // should not trigger any other capturing other than EXTENDED
        canvas = RSCaptureRecorder::GetInstance().TryInstantCapture(res, res, SkpCaptureType::IMG_CACHED);
        EXPECT_TRUE(canvas == nullptr);
        canvas = RSCaptureRecorder::GetInstance().TryInstantCapture(res, res, SkpCaptureType::ON_CAPTURE);
        EXPECT_TRUE(canvas == nullptr);
        canvas = RSCaptureRecorder::GetInstance().TryInstantCapture(res, res, SkpCaptureType::DEFAULT);
        EXPECT_TRUE(canvas == nullptr);
        canvas = RSCaptureRecorder::GetInstance().TryInstantCapture(res, res, SkpCaptureType::EXTENDED);
        EXPECT_TRUE(canvas != nullptr);
        RSCaptureRecorder::GetInstance().EndInstantCapture(SkpCaptureType::EXTENDED);
    });
}

HWTEST_F(RSProfilerCapturingTest, RSPrintFuncTest, testing::ext::TestSize.Level1)
{
    RSProfiler::testing_ = true;
    EXPECT_NO_THROW({
        bool ret = false;
        {
            ret = RSCaptureRecorder::GetInstance().PrintMessage("test", false);
            EXPECT_TRUE(ret == false);
            ret = RSCaptureRecorder::GetInstance().PrintMessage("test", true);
            EXPECT_TRUE(ret == true);
        }
        {
            RSCaptureRecorder::testingTriggering_ = true;
            ret = RSCaptureRecorder::GetInstance().PrintMessage("test", false);
            EXPECT_TRUE(ret == false);
            ret = RSCaptureRecorder::GetInstance().PrintMessage("test", true);
            EXPECT_TRUE(ret == true);
        }
    });
}
} // namespace OHOS::Rosen