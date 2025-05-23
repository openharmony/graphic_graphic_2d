/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
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
#include <hilog/log.h>
#include <memory>
#include <unistd.h>

#include "surface_buffer_impl.h"
#include "feature/capture/rs_surface_capture_task.h"
#include "feature/capture/rs_ui_capture_solo_task_parallel.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "transaction/rs_interfaces.h"
#include "ui/rs_surface_extractor.h"
#include "ui/rs_root_node.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_effect_node.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_canvas_drawing_node.h"
#include "ui/rs_proxy_node.h"
#include "pipeline/render_thread/rs_render_engine.h"
#include "pipeline/render_thread/rs_uni_render_engine.h"
#include "pipeline/rs_test_util.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_uni_render_judgement.h"

using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using namespace HiviewDFX;
using DisplayId = ScreenId;
namespace {
constexpr HiLogLabel LOG_LABEL = { LOG_CORE, 0xD001400, "RSUiCaptureSoloTaskParallelTest" };
constexpr uint32_t SLEEP_TIME_FOR_PROXY = 1000000; // 1000ms
constexpr float DEFAULT_BOUNDS_WIDTH = 100.f;
constexpr float DEFAULT_BOUNDS_HEIGHT = 200.f;
constexpr float HALF_BOUNDS_WIDTH = 50.0f;
constexpr float HALF_BOUNDS_HEIGHT = 100.0f;
}

class RSUiCaptureSoloTaskParallelTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
#ifdef RS_ENABLE_VK
        RsVulkanContext::SetRecyclable(false);
#endif
        rsInterfaces_ = &RSInterfaces::GetInstance();

        RSTestUtil::InitRenderNodeGC();
        ScreenId screenId = rsInterfaces_->GetDefaultScreenId();
        RSScreenModeInfo modeInfo = rsInterfaces_->GetScreenActiveMode(screenId);
        DisplayId virtualDisplayId = rsInterfaces_->CreateVirtualScreen("virtualDisplayTest",
            modeInfo.GetScreenWidth(), modeInfo.GetScreenHeight(), nullptr);
        mirrorConfig_.screenId = virtualDisplayId;
        mirrorConfig_.mirrorNodeId = screenId;
        displayNode_ = RSDisplayNode::Create(mirrorConfig_);

        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);
    }

    static void TearDownTestCase()
    {
        rsInterfaces_->RemoveVirtualScreen(mirrorConfig_.screenId);
        rsInterfaces_ = nullptr;
        renderContext_ = nullptr;
        displayNode_ = nullptr;
        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);
    }

    static void InitRenderContext()
    {
#ifdef ACE_ENABLE_GL
        if (renderContext_ == nullptr) {
            HiLog::Info(LOG_LABEL, "%s: init renderContext_", __func__);
            renderContext_ = RenderContextFactory::GetInstance().CreateEngine();
            renderContext_->InitializeEglContext();
        }
#endif // ACE_ENABLE_GL
    }

    void SetUp() override
    {
    }

    void TearDown() override
    {
        if (surfaceNode_) {
            displayNode_->RemoveChild(surfaceNode_);
            surfaceNode_ = nullptr;
            canvasNode_ = nullptr;

            RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
            usleep(SLEEP_TIME_FOR_PROXY);
        }
    }

    std::shared_ptr<RSSurfaceNode> SetUpSurface()
    {
        RSSurfaceNodeConfig config;
        surfaceNode_ = RSSurfaceNode::Create(config);
        surfaceNode_->SetBounds(0.0f, 0.0f, DEFAULT_BOUNDS_WIDTH, DEFAULT_BOUNDS_HEIGHT);
        surfaceNode_->SetFrame(0.0f, 0.0f, DEFAULT_BOUNDS_WIDTH, DEFAULT_BOUNDS_HEIGHT);
        surfaceNode_->SetBackgroundColor(Drawing::Color::COLOR_RED);

        canvasNode_ = RSCanvasNode::Create();
        canvasNode_->SetBounds(0.0f, 0.0f, HALF_BOUNDS_WIDTH, HALF_BOUNDS_HEIGHT);
        canvasNode_->SetFrame(0.0f, 0.0f, HALF_BOUNDS_WIDTH, HALF_BOUNDS_HEIGHT);
        canvasNode_->SetBackgroundColor(Drawing::Color::COLOR_YELLOW);

        canvasDrawingNode_ = RSCanvasDrawingNode::Create();
        canvasDrawingNode_->SetBounds(0.0f, 0.0f, HALF_BOUNDS_WIDTH, HALF_BOUNDS_HEIGHT);
        canvasDrawingNode_->SetFrame(0.0f, 0.0f, HALF_BOUNDS_WIDTH, HALF_BOUNDS_HEIGHT);
        canvasDrawingNode_->SetBackgroundColor(Drawing::Color::COLOR_YELLOW);

        canvasNode_->AddChild(canvasDrawingNode_, -1);
        surfaceNode_->AddChild(canvasNode_, -1);
        displayNode_->AddChild(surfaceNode_, -1);
        RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
        usleep(SLEEP_TIME_FOR_PROXY);

        return surfaceNode_;
    }

    std::shared_ptr<RSUiCaptureSoloTaskParallel> BuildTaskParallel(NodeId nodeId, float width = 0.0f,
        float height = 0.0f)
    {
        RSSurfaceCaptureConfig config;
        auto renderNode = std::make_shared<RSSurfaceRenderNode>(nodeId, std::make_shared<RSContext>(), true);
        renderNode->renderContent_->renderProperties_.SetBoundsWidth(width);
        renderNode->renderContent_->renderProperties_.SetBoundsHeight(height);
        RSMainThread::Instance()->GetContext().nodeMap.RegisterRenderNode(renderNode);

        auto renderNodeHandle = std::make_shared<RSUiCaptureSoloTaskParallel>(nodeId, config);
        return renderNodeHandle;
    }

    static RSInterfaces* rsInterfaces_;
    static RenderContext* renderContext_;
    static RSDisplayNodeConfig mirrorConfig_;
    static std::shared_ptr<RSDisplayNode> displayNode_;

    std::shared_ptr<RSSurfaceNode> surfaceNode_;
    std::shared_ptr<RSCanvasNode> canvasNode_;
    std::shared_ptr<RSCanvasDrawingNode> canvasDrawingNode_;
};
RSInterfaces* RSUiCaptureSoloTaskParallelTest::rsInterfaces_ = nullptr;
RenderContext* RSUiCaptureSoloTaskParallelTest::renderContext_ = nullptr;
RSDisplayNodeConfig RSUiCaptureSoloTaskParallelTest::mirrorConfig_ = {INVALID_SCREEN_ID, true, INVALID_SCREEN_ID};
std::shared_ptr<RSDisplayNode> RSUiCaptureSoloTaskParallelTest::displayNode_ = nullptr;

/*
* @tc.name: RSUiCaptureSoloTaskParallelValid
* @tc.desc: Test RSUiCaptureSoloTaskParallel with valid node
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(RSUiCaptureSoloTaskParallelTest, RSUiCaptureSoloTaskParallelValid, Function | SmallTest | Level2)
{
    SetUpSurface();
    std::vector<std::pair<NodeId, std::shared_ptr<Media::PixelMap>>> res;
    res = rsInterfaces_->TakeSurfaceCaptureSoloNodeList(surfaceNode_);
    EXPECT_GT(res.size(), 0);
}

/*
* @tc.name: RSUiCaptureSoloNode
* @tc.desc: Test RSUiCaptureSoloTaskParallel using static function
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(RSUiCaptureSoloTaskParallelTest, RSUiCaptureSoloNode, Function | SmallTest | Level2)
{
    RSSurfaceNodeConfig config;
    auto surfaceNode = RSSurfaceNode::Create(config, false);
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.isSoloNodeUiCapture = true;
    std::vector<std::pair<NodeId, std::shared_ptr<Media::PixelMap>>> pixelMapIdPairVector =
        RSUiCaptureSoloTaskParallel::CaptureSoloNode(surfaceNode->GetId(), captureConfig);
    EXPECT_EQ(pixelMapIdPairVector.size(), 0);
}

/*
* @tc.name: CreateResources001
* @tc.desc: Test RSUiCaptureSoloTaskParallel::CreateResources
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(RSUiCaptureSoloTaskParallelTest, CreateResources001, Function | SmallTest | Level2)
{
    NodeId id = -1; // invalid id
    RSSurfaceCaptureConfig captureConfig;
    auto handle = std::make_shared<RSUiCaptureSoloTaskParallel>(id, captureConfig);
    EXPECT_EQ(handle->CreateResources(), false);
    EXPECT_EQ(handle->captureConfig_.scaleX, 1.0f);
    EXPECT_EQ(handle->captureConfig_.scaleY, 1.0f);
    EXPECT_EQ(handle->pixelMap_, nullptr);
    EXPECT_EQ(handle->nodeDrawable_, nullptr);
}

/*
* @tc.name: CreateResources002
* @tc.desc: Test RSUiCaptureSoloTaskParallel::CreateResources
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(RSUiCaptureSoloTaskParallelTest, CreateResources002, Function | SmallTest | Level2)
{
    auto& nodeMap = RSMainThread::Instance()->GetContext().nodeMap;

    // RSSurfaceRenderNode
    NodeId surfaceRenderNodeId = 101;
    RSSurfaceCaptureConfig config1;
    auto surfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(surfaceRenderNodeId, std::make_shared<RSContext>(),
        true);
    nodeMap.RegisterRenderNode(surfaceRenderNode);
    auto surfaceRenderNodeHandle = std::make_shared<RSUiCaptureSoloTaskParallel>(surfaceRenderNodeId, config1);
    EXPECT_EQ(surfaceRenderNodeHandle->CreateResources(), false);

    // RSCanvasRenderNode
    NodeId canvasRenderNodeId = 102;
    RSSurfaceCaptureConfig config2;
    auto canvasRenderNode = std::make_shared<RSCanvasRenderNode>(canvasRenderNodeId, std::make_shared<RSContext>(),
        true);
    nodeMap.RegisterRenderNode(canvasRenderNode);
    auto canvasRenderNodeHandle = std::make_shared<RSUiCaptureSoloTaskParallel>(canvasRenderNodeId, config2);
    EXPECT_EQ(canvasRenderNodeHandle->CreateResources(), false);
}

/*
* @tc.name: CreateResources003
* @tc.desc: Test RSUiCaptureSoloTaskParallel::CreateResources
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(RSUiCaptureSoloTaskParallelTest, CreateResources003, Function | SmallTest | Level2)
{
    auto& nodeMap = RSMainThread::Instance()->GetContext().nodeMap;

    NodeId nodeId = 103;
    NodeId parentNodeId = 1003;
    RSSurfaceCaptureConfig config;
    auto renderNode = std::make_shared<RSSurfaceRenderNode>(nodeId, std::make_shared<RSContext>(), true);
    renderNode->renderContent_->renderProperties_.SetBoundsWidth(1024.0f);
    renderNode->renderContent_->renderProperties_.SetBoundsHeight(1024.0f);
    nodeMap.RegisterRenderNode(renderNode);

    auto renderNodeHandle = std::make_shared<RSUiCaptureSoloTaskParallel>(nodeId, config);
    EXPECT_EQ(renderNodeHandle->CreateResources(), true);

    auto parent1 = std::make_shared<RSSurfaceRenderNode>(parentNodeId, std::make_shared<RSContext>(), true);
    parent1->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    parent1->hasSubNodeShouldPaint_ = false;
    renderNode->parent_ = parent1;
    EXPECT_EQ(renderNodeHandle->CreateResources(), true);

    auto parent2 = std::make_shared<RSSurfaceRenderNode>(parentNodeId, std::make_shared<RSContext>(), true);
    parent2->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    parent2->hasSubNodeShouldPaint_ = true;
    renderNode->parent_ = parent2;
    EXPECT_EQ(renderNodeHandle->CreateResources(), true);

    auto parent3 = std::make_shared<RSSurfaceRenderNode>(parentNodeId, std::make_shared<RSContext>(), true);
    parent3->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    parent3->hasSubNodeShouldPaint_ = true;
    parent3->lastFrameUifirstFlag_ = MultiThreadCacheType::NONFOCUS_WINDOW;
    parent3->renderContent_->renderProperties_.SetBoundsWidth(1024.0f);
    parent3->renderContent_->renderProperties_.SetBoundsHeight(1024.0f);
    renderNode->parent_ = parent3;
    EXPECT_EQ(renderNodeHandle->CreateResources(), true);
}

/*
* @tc.name: Run001
* @tc.desc: Test RSUiCaptureSoloTaskParallel::Run
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(RSUiCaptureSoloTaskParallelTest, Run001, Function | SmallTest | Level2)
{
    auto renderEngine = std::make_shared<RSRenderEngine>();
    renderEngine->Init();
    RSUniRenderThread::Instance().uniRenderEngine_ = renderEngine;
    auto handle = BuildTaskParallel(-1, 0.0f, 0.0f);
    EXPECT_EQ(handle->Run(), false);

    handle->CreateResources();
    EXPECT_EQ(handle->Run(), false);
    RSUniRenderThread::Instance().uniRenderEngine_ = nullptr;
}

/*
* @tc.name: Run002
* @tc.desc: Test RSUiCaptureSoloTaskParallel::Run
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(RSUiCaptureSoloTaskParallelTest, Run002, Function | SmallTest | Level2)
{
    auto renderEngine = std::make_shared<RSRenderEngine>();
    renderEngine->Init();
    RSUniRenderThread::Instance().uniRenderEngine_ = renderEngine;
    auto handle = BuildTaskParallel(200, 1024.0f, 1024.0f);
    EXPECT_EQ(handle->Run(), false);

    handle->nodeDrawable_ = nullptr;
    EXPECT_EQ(handle->Run(), false);
    RSUniRenderThread::Instance().uniRenderEngine_ = nullptr;
}

/*
* @tc.name: TestCreatePixelMapByNode
* @tc.desc: Test RSUiCaptureSoloTaskParallel::CreatePixelMapByNode
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(RSUiCaptureSoloTaskParallelTest, TestCreatePixelMapByNode, Function | SmallTest | Level2)
{
    NodeId id = -1; // invalid id
    RSSurfaceCaptureConfig captureConfig;
    auto handle = std::make_shared<RSUiCaptureSoloTaskParallel>(id, captureConfig);
    auto node = RSTestUtil::CreateSurfaceNode();
    EXPECT_EQ(handle->CreatePixelMapByNode(node), nullptr);
}

/*
* @tc.name: TestCreateSurfaceSyncCopyTask
* @tc.desc: Test RSUiCaptureSoloTaskParallel::CreateSurfaceSyncCopyTask
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(RSUiCaptureSoloTaskParallelTest, TestCreateSurfaceSyncCopyTask, Function | SmallTest | Level2)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    auto mainThread = RSMainThread::Instance();

    mainThread->context_->nodeMap.RegisterRenderNode(node);

    RSSurfaceCaptureConfig captureConfig;
    auto pixelMap = std::make_unique<Media::PixelMap>();
    EXPECT_NE(pixelMap, nullptr);
    auto surface = std::make_shared<Drawing::Surface>();
    EXPECT_NE(surface, nullptr);
#ifdef RS_ENABLE_UNI_RENDER
    auto copytask =
        RSUiCaptureSoloTaskParallel::CreateSurfaceSyncCopyTask(surface, pixelMap,
            node->GetId(), captureConfig);

    EXPECT_FALSE(copytask);
    mainThread->context_->nodeMap.UnregisterRenderNode(node->GetId());
#endif
}
} // namespace Rosen
} // namespace OHOS
