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
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_surface_render_node.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class RsSurfaceCaptureTaskParallelTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RsSurfaceCaptureTaskParallelTest::SetUpTestCase() {}
void RsSurfaceCaptureTaskParallelTest::TearDownTestCase() {}
void RsSurfaceCaptureTaskParallelTest::SetUp() {}
void RsSurfaceCaptureTaskParallelTest::TearDown() {}

/*
 * @tc.name: CalPixelMapRotation01
 * @tc.desc: function test
 * @tc.type: FUNC
 * @tc.require: issueI9PKY5
*/
HWTEST_F(RsSurfaceCaptureTaskParallelTest, CalPixelMapRotation01, TestSize.Level2)
{
    RSSurfaceCaptureConfig captureConfig;
    RSSurfaceCaptureTaskParallel task(0, captureConfig);
    task.screenCorrection_ = ScreenRotation::ROTATION_90;
    task.screenRotation_ = ScreenRotation::ROTATION_270;
    ASSERT_EQ(task.CalPixelMapRotation(), RS_ROTATION_180);
}

/*
 * @tc.name: CreatePixelMapBySurfaceNode01
 * @tc.desc: Test RSSurfaceCaptureTaskParallel.CreatePixelMapBySurfaceNode with not nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAHND9
*/
HWTEST_F(RsSurfaceCaptureTaskParallelTest, CreatePixelMapBySurfaceNode01, TestSize.Level2)
{
    RSSurfaceCaptureConfig captureConfig;
    RSSurfaceCaptureTaskParallel task(0, captureConfig);
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(0);
    ASSERT_NE(node, nullptr);
    const float imgWidth = 1.0f;
    const float imgHeight = 1.0f;
    node->GetGravityTranslate(imgWidth, imgHeight);
    auto pxiemap = task.CreatePixelMapBySurfaceNode(node);
    ASSERT_EQ(pxiemap, nullptr);
}

/*
 * @tc.name: SetupGpuContext01
 * @tc.desc: Test RSSurfaceCaptureTaskParallel.SetupGpuContext while gpuContext_ is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAHND9
*/
HWTEST_F(RsSurfaceCaptureTaskParallelTest, SetupGpuContext01, TestSize.Level2)
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
 * @tc.name: CreatePixelMapByDisplayNode01
 * @tc.desc: Test RSSurfaceCaptureTaskParallel.CreatePixelMapByDisplayNode while node is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAHND9
*/
HWTEST_F(RsSurfaceCaptureTaskParallelTest, CreatePixelMapByDisplayNode01, TestSize.Level2)
{
    RSSurfaceCaptureConfig captureConfig;
    RSSurfaceCaptureTaskParallel task(0, captureConfig);
    ASSERT_EQ(nullptr, task.CreatePixelMapByDisplayNode(nullptr));
}

/*
 * @tc.name: CreatePixelMapByDisplayNode02
 * @tc.desc: Test RSSurfaceCaptureTaskParallel.CreatePixelMapByDisplayNode with not nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAHND9
*/
HWTEST_F(RsSurfaceCaptureTaskParallelTest, CreatePixelMapByDisplayNode02, TestSize.Level2)
{
    RSSurfaceCaptureConfig captureConfig;
    RSSurfaceCaptureTaskParallel task(0, captureConfig);
    RSDisplayNodeConfig config;
    std::shared_ptr<RSDisplayRenderNode> node = std::make_shared<RSDisplayRenderNode>(0, config);
    ASSERT_EQ(nullptr, task.CreatePixelMapByDisplayNode(node));
}

/*
 * @tc.name: CreateResources01
 * @tc.desc: Test RSSurfaceCaptureTaskParallel.CreateResources: while SurfaceCapture scale is invalid
 * @tc.type: FUNC
 * @tc.require: issueIAHND9
*/
HWTEST_F(RsSurfaceCaptureTaskParallelTest, CreateResources01, TestSize.Level2)
{
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 0.f;
    captureConfig.scaleY = 0.f;
    RSSurfaceCaptureTaskParallel task(0, captureConfig);
    ASSERT_EQ(false, task.CreateResources());
}

/*
 * @tc.name: CreateSurface01
 * @tc.desc: Test RSSurfaceCaptureTaskParallel.CreateSurface with pixelmap is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAHND9
*/
HWTEST_F(RsSurfaceCaptureTaskParallelTest, CreateSurface01, TestSize.Level2)
{
    RSSurfaceCaptureConfig captureConfig;
    RSSurfaceCaptureTaskParallel task(0, captureConfig);
    std::unique_ptr<Media::PixelMap> pixelmap = nullptr;
    ASSERT_EQ(nullptr, task.CreateSurface(pixelmap));
}

/*
 * @tc.name: CreateResources02
 * @tc.desc: Test RSSurfaceCaptureTaskParallel.CreateResources while node is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAHND9
*/
HWTEST_F(RsSurfaceCaptureTaskParallelTest, CreateResources02, TestSize.Level2)
{
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 1.0f;
    captureConfig.scaleY = 1.0f;
    RSSurfaceCaptureTaskParallel task(0, captureConfig);
    ASSERT_EQ(false, task.CreateResources());
}

/*
 * @tc.name: CreateResources03
 * @tc.desc: Test RSSurfaceCaptureTaskParallel.CreateResources with not nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAHND9
*/
HWTEST_F(RsSurfaceCaptureTaskParallelTest, CreateResources03, TestSize.Level2)
{
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 1.0f;
    captureConfig.scaleY = 1.0f;
    captureConfig.useCurWindow = false;
    RSSurfaceCaptureTaskParallel task(1, captureConfig);
    ASSERT_EQ(false, task.CreateResources());
}

/*
 * @tc.name: Run01
 * @tc.desc: Test RSSurfaceCaptureTaskParallel.Run while surface is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAIT5Z
*/
HWTEST_F(RsSurfaceCaptureTaskParallelTest, Run01, TestSize.Level2)
{
    NodeId id = 0;
    RSSurfaceCaptureConfig captureConfig;
    RSSurfaceCaptureTaskParallel task(id, captureConfig);
    task.pixelMap_ = nullptr;
    RSSurfaceCaptureParam captureParam;
    ASSERT_EQ(nullptr, task.pixelMap_);
    ASSERT_EQ(false, task.Run(nullptr, captureParam));
}

/*
 * @tc.name: Run02
 * @tc.desc: Test RSSurfaceCaptureTaskParallel.Run while displayNodeDrawable_ is not nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAIT5Z
*/
HWTEST_F(RsSurfaceCaptureTaskParallelTest, Run02, TestSize.Level2)
{
    NodeId id = 0;
    RSSurfaceCaptureConfig captureConfig;
    RSSurfaceCaptureTaskParallel task(id, captureConfig);
    auto node = std::make_shared<RSRenderNode>(id);
    task.surfaceNodeDrawable_ = nullptr;
    task.displayNodeDrawable_ = std::static_pointer_cast<DrawableV2::RSRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(node));
    RSSurfaceCaptureParam captureParam;
    ASSERT_EQ(false, task.Run(nullptr, captureParam));
}

/*
 * @tc.name: Run03
 * @tc.desc: Test RSSurfaceCaptureTaskParallel.Run while surfaceNodeDrawable_ is not nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAIT5Z
*/
HWTEST_F(RsSurfaceCaptureTaskParallelTest, Run03, TestSize.Level2)
{
    NodeId id = 0;
    RSSurfaceCaptureConfig captureConfig;
    RSSurfaceCaptureTaskParallel task(id, captureConfig);
    auto node = std::make_shared<RSRenderNode>(id);
    task.surfaceNodeDrawable_ = std::static_pointer_cast<DrawableV2::RSRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(node));
    RSSurfaceCaptureParam captureParam;
    ASSERT_EQ(false, task.Run(nullptr, captureParam));
}

/*
 * @tc.name: Run04
 * @tc.desc: Test RSSurfaceCaptureTaskParallel.Run while surfaceNodeDrawable_ and displayNodeDrawable_ is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAIT5Z
*/
HWTEST_F(RsSurfaceCaptureTaskParallelTest, Run04, TestSize.Level2)
{
    NodeId id = 0;
    RSSurfaceCaptureConfig captureConfig;
    RSSurfaceCaptureTaskParallel task(id, captureConfig);
    task.surfaceNodeDrawable_ = nullptr;
    task.displayNodeDrawable_ = nullptr;
    RSSurfaceCaptureParam captureParam;
    ASSERT_EQ(false, task.Run(nullptr, captureParam));
}
}
}
