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

#include "feature/capture/rs_surface_capture_task_parallel.h"
#include "pipeline/render_thread/rs_uni_render_engine.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_test_util.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RSSurfaceCaptureTaskParallelTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSSurfaceCaptureTaskParallelTest::SetUpTestCase()
{
    RSTestUtil::InitRenderNodeGC();
}
void RSSurfaceCaptureTaskParallelTest::TearDownTestCase() {}
void RSSurfaceCaptureTaskParallelTest::SetUp() {}
void RSSurfaceCaptureTaskParallelTest::TearDown() {}

/*
 * @tc.name: CalPixelMapRotation
 * @tc.desc: function test
 * @tc.type: FUNC
 * @tc.require: issueI9PKY5
*/
HWTEST_F(RSSurfaceCaptureTaskParallelTest, CalPixelMapRotation, TestSize.Level2)
{
    RSSurfaceCaptureConfig captureConfig;
    RSSurfaceCaptureTaskParallel task(0, captureConfig);
    task.screenCorrection_ = ScreenRotation::ROTATION_90;
    task.screenRotation_ = ScreenRotation::ROTATION_270;
    ASSERT_EQ(task.CalPixelMapRotation(), RS_ROTATION_180);
}

/*
 * @tc.name: SetupGpuContext
 * @tc.desc: Test RSSurfaceCaptureTaskParallel.SetupGpuContext while gpuContext_ is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAHND9
*/
HWTEST_F(RSSurfaceCaptureTaskParallelTest, SetupGpuContext, TestSize.Level2)
{
    RSSurfaceCaptureConfig captureConfig;
    RSSurfaceCaptureTaskParallel task(0, captureConfig);
    if (RSUniRenderJudgement::IsUniRender()) {
        auto& uniRenderThread = RSUniRenderThread::Instance();
        ASSERT_EQ(uniRenderThread.uniRenderEngine_, nullptr);
        uniRenderThread.uniRenderEngine_ = std::make_shared<RSUniRenderEngine>();
        ASSERT_NE(uniRenderThread.uniRenderEngine_, nullptr);
    }
    task.gpuContext_ = nullptr;
    task.SetupGpuContext();
}

/*
 * @tc.name: CreatePixelMapBySurfaceNode
 * @tc.desc: Test RSSurfaceCaptureTaskParallel.CreatePixelMapBySurfaceNode with not nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAHND9
*/
HWTEST_F(RSSurfaceCaptureTaskParallelTest, CreatePixelMapBySurfaceNode, TestSize.Level2)
{
    RSSurfaceCaptureConfig captureConfig;
    RSSurfaceCaptureTaskParallel task(0, captureConfig);
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(0);
    ASSERT_NE(node, nullptr);
    const float imgWidth = 1.0f;
    const float imgHeight = 1.0f;
    node->GetGravityTranslate(imgWidth, imgHeight);
    task.surfaceNode_ = node;
    auto pxiemap = task.CreatePixelMapBySurfaceNode(node);
    ASSERT_EQ(pxiemap, nullptr);
}

/*
 * @tc.name: CreatePixelMapByDisplayNode001
 * @tc.desc: Test RSSurfaceCaptureTaskParallel.CreatePixelMapByDisplayNode while node is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAHND9
*/
HWTEST_F(RSSurfaceCaptureTaskParallelTest, CreatePixelMapByDisplayNode001, TestSize.Level2)
{
    RSSurfaceCaptureConfig captureConfig;
    RSSurfaceCaptureTaskParallel task(0, captureConfig);
    ASSERT_EQ(nullptr, task.CreatePixelMapByDisplayNode(nullptr));
}

/*
 * @tc.name: CreatePixelMapByDisplayNode002
 * @tc.desc: Test RSSurfaceCaptureTaskParallel.CreatePixelMapByDisplayNode with not nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAHND9
*/
HWTEST_F(RSSurfaceCaptureTaskParallelTest, CreatePixelMapByDisplayNode002, TestSize.Level2)
{
    RSSurfaceCaptureConfig captureConfig;
    RSSurfaceCaptureTaskParallel task(0, captureConfig);
    RSDisplayNodeConfig config;
    std::shared_ptr<RSDisplayRenderNode> node = std::make_shared<RSDisplayRenderNode>(0, config);
    ASSERT_EQ(nullptr, task.CreatePixelMapByDisplayNode(node));
}

/*
 * @tc.name: CreateSurface001
 * @tc.desc: Test RSSurfaceCaptureTaskParallel.CreateSurface with pixelmap is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAHND9
*/
HWTEST_F(RSSurfaceCaptureTaskParallelTest, CreateSurface001, TestSize.Level2)
{
    RSSurfaceCaptureConfig captureConfig;
    RSSurfaceCaptureTaskParallel task(0, captureConfig);
    std::unique_ptr<Media::PixelMap> pixelmap = nullptr;
    ASSERT_EQ(nullptr, task.CreateSurface(pixelmap));
}

/*
 * @tc.name: CreateResources001
 * @tc.desc: Test RSSurfaceCaptureTaskParallel.CreateResources: while SurfaceCapture scale is invalid
 * @tc.type: FUNC
 * @tc.require: issueIAHND9
*/
HWTEST_F(RSSurfaceCaptureTaskParallelTest, CreateResources001, TestSize.Level2)
{
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 0.f;
    captureConfig.scaleY = 0.f;
    RSSurfaceCaptureTaskParallel task(0, captureConfig);
    ASSERT_EQ(false, task.CreateResources());
}

/*
 * @tc.name: CreateResources002
 * @tc.desc: Test RSSurfaceCaptureTaskParallel.CreateResources while node is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAHND9
*/
HWTEST_F(RSSurfaceCaptureTaskParallelTest, CreateResources002, TestSize.Level2)
{
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 1.0f;
    captureConfig.scaleY = 1.0f;
    RSSurfaceCaptureTaskParallel task(0, captureConfig);
    ASSERT_EQ(false, task.CreateResources());
}

/*
 * @tc.name: CreateResources003
 * @tc.desc: Test RSSurfaceCaptureTaskParallel.CreateResources with not nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAHND9
*/
HWTEST_F(RSSurfaceCaptureTaskParallelTest, CreateResources003, TestSize.Level2)
{
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 1.0f;
    captureConfig.scaleY = 1.0f;
    captureConfig.useCurWindow = false;
    RSSurfaceCaptureTaskParallel task(1, captureConfig);
    ASSERT_EQ(false, task.CreateResources());
}

/*
 * @tc.name: Run001
 * @tc.desc: Test RSSurfaceCaptureTaskParallel.Run while surface is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAIT5Z
*/
HWTEST_F(RSSurfaceCaptureTaskParallelTest, Run001, TestSize.Level2)
{
    NodeId id = 0;
    RSSurfaceCaptureConfig captureConfig;
    RSSurfaceCaptureTaskParallel task(id, captureConfig);
    task.pixelMap_ = nullptr;
    ASSERT_EQ(nullptr, task.pixelMap_);
    RSSurfaceCaptureBlurParam blurParam;
    ASSERT_EQ(false, task.Run(nullptr, blurParam, false));
}

/*
 * @tc.name: Run002
 * @tc.desc: Test RSSurfaceCaptureTaskParallel.Run while surfaceNodeDrawable_ is not nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAIT5Z
*/
HWTEST_F(RSSurfaceCaptureTaskParallelTest, Run002, TestSize.Level2)
{
    NodeId id = 0;
    RSSurfaceCaptureConfig captureConfig;
    RSSurfaceCaptureTaskParallel task(id, captureConfig);
    auto node = std::make_shared<RSRenderNode>(id);
    task.surfaceNodeDrawable_ = std::static_pointer_cast<DrawableV2::RSRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(node));
    RSSurfaceCaptureBlurParam blurParam;
    ASSERT_EQ(false, task.Run(nullptr, blurParam, false));
}

/*
 * @tc.name: Run003
 * @tc.desc: Test RSSurfaceCaptureTaskParallel.Run while displayNodeDrawable_ is not nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAIT5Z
*/
HWTEST_F(RSSurfaceCaptureTaskParallelTest, Run003, TestSize.Level2)
{
    NodeId id = 0;
    RSSurfaceCaptureConfig captureConfig;
    RSSurfaceCaptureTaskParallel task(id, captureConfig);
    auto node = std::make_shared<RSRenderNode>(id);
    task.surfaceNodeDrawable_ = nullptr;
    task.displayNodeDrawable_ = std::static_pointer_cast<DrawableV2::RSRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(node));
    RSSurfaceCaptureBlurParam blurParam;
    ASSERT_EQ(false, task.Run(nullptr, blurParam, false));
}

/*
 * @tc.name: Run004
 * @tc.desc: Test RSSurfaceCaptureTaskParallel.Run while surfaceNodeDrawable_ and displayNodeDrawable_ is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAIT5Z
*/
HWTEST_F(RSSurfaceCaptureTaskParallelTest, Run004, TestSize.Level2)
{
    NodeId id = 0;
    RSSurfaceCaptureConfig captureConfig;
    RSSurfaceCaptureTaskParallel task(id, captureConfig);
    task.surfaceNodeDrawable_ = nullptr;
    task.displayNodeDrawable_ = nullptr;
    RSSurfaceCaptureBlurParam blurParam;
    ASSERT_EQ(false, task.Run(nullptr, blurParam, false));
}

/*
 * @tc.name: CheckModifiers
 * @tc.desc: Test CheckModifiers
 * @tc.type: FUNC
 * @tc.require:
*/
HWTEST_F(RSSurfaceCaptureTaskParallelTest, CheckModifiers, TestSize.Level2)
{
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    mainThread->nodeTreeDumpTasks_.clear();
    NodeId nodeId = 1;
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(nodeId);
    ASSERT_NE(node, nullptr);
    node->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE;
    RSRenderNodeMap& nodeMap = mainThread->GetContext().GetMutableNodeMap();
    nodeMap.RegisterRenderNode(node);
    ASSERT_TRUE(mainThread->IsOcclusionNodesNeedSync(nodeId, true));
    mainThread->GetContext().AddPendingSyncNode(node);
    RSSurfaceCaptureConfig captureConfig;
    RSSurfaceCaptureTaskParallel task(nodeId, captureConfig);
    captureConfig.useCurWindow = true;
    task.CheckModifiers(nodeId, captureConfig.useCurWindow);
}

}
}
