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
#include <hilog/log.h>
#include <memory>
#include <unistd.h>

#include "surface_buffer_impl.h"
#include "feature/capture/rs_surface_capture_task.h"
#include "feature/capture/rs_ui_capture_task_parallel.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "transaction/rs_interfaces.h"
#include "ui/rs_surface_extractor.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_canvas_drawing_node.h"
#include "ui/rs_proxy_node.h"
#include "pipeline/render_thread/rs_render_engine.h"
#include "pipeline/render_thread/rs_uni_render_engine.h"
#include "pipeline/rs_test_util.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "pipeline/rs_render_node_gc.h"
#include "platform/common/rs_system_properties.h"
#include "feature/capture/rs_capture_pixelmap_manager.h"
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using namespace HiviewDFX;
using DisplayId = ScreenId;
namespace {
constexpr HiLogLabel LOG_LABEL = { LOG_CORE, 0xD001400, "RSUiCaptureTaskParallelTest" };
constexpr uint32_t MAX_TIME_WAITING_FOR_CALLBACK = 200;
constexpr uint32_t SLEEP_TIME_IN_US = 10000; // 10ms
constexpr uint32_t SLEEP_TIME_FOR_PROXY = 100000; // 100ms
constexpr float DEFAULT_BOUNDS_WIDTH = 100.f;
constexpr float DEFAULT_BOUNDS_HEIGHT = 200.f;
constexpr float HALF_BOUNDS_WIDTH = 50.0f;
constexpr float HALF_BOUNDS_HEIGHT = 100.0f;

class CustomizedSurfaceCapture : public SurfaceCaptureCallback {
public:
    void OnSurfaceCapture(std::shared_ptr<Media::PixelMap> pixelmap) override
    {
        captureSuccess_ = (pixelmap != nullptr);
        isCallbackCalled_ = true;
    }

    void OnSurfaceCaptureHDR(std::shared_ptr<Media::PixelMap> pixelMap,
        std::shared_ptr<Media::PixelMap> pixelMapHDR) override {}

    void Reset()
    {
        captureSuccess_ = false;
        isCallbackCalled_ = false;
    }

    bool captureSuccess_ = false;
    bool isCallbackCalled_ = false;
};

class RSC_EXPORT MockSurfaceCaptureCallback : public RSISurfaceCaptureCallback {
    sptr<IRemoteObject> AsObject()
    {
        return nullptr;
    }

    void OnSurfaceCapture(NodeId id, const RSSurfaceCaptureConfig& captureConfig, Media::PixelMap* pixelmap,
        Media::PixelMap* pixelmapHDR = nullptr) override
    {
        // DO NOTHING
    }
};
}

class RSUiCaptureTaskParallelTest : public testing::Test {
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

    bool CheckSurfaceCaptureCallback(std::shared_ptr<CustomizedSurfaceCapture> callback)
    {
        if (!callback) {
            return false;
        }

        uint32_t times = 0;
        while (times < MAX_TIME_WAITING_FOR_CALLBACK) {
            if (callback->isCallbackCalled_) {
                return true;
            }
            usleep(SLEEP_TIME_IN_US);
            ++times;
        }
        HiLog::Error(LOG_LABEL, "CheckSurfaceCaptureCallback timeout");
        return false;
    }

    std::shared_ptr<RSUiCaptureTaskParallel> BuildTaskParallel(NodeId nodeId, float width = 0.0f, float height = 0.0f)
    {
        RSSurfaceCaptureConfig config;
        auto renderNode = std::make_shared<RSSurfaceRenderNode>(nodeId, std::make_shared<RSContext>(), true);
        renderNode->renderProperties_.SetBoundsWidth(width);
        renderNode->renderProperties_.SetBoundsHeight(height);
        RSMainThread::Instance()->GetContext().nodeMap.RegisterRenderNode(renderNode);
        auto renderNodeHandle = std::make_shared<RSUiCaptureTaskParallel>(nodeId, config);
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
RSInterfaces* RSUiCaptureTaskParallelTest::rsInterfaces_ = nullptr;
RenderContext* RSUiCaptureTaskParallelTest::renderContext_ = nullptr;
RSDisplayNodeConfig RSUiCaptureTaskParallelTest::mirrorConfig_ = {INVALID_SCREEN_ID, true, INVALID_SCREEN_ID};
std::shared_ptr<RSDisplayNode> RSUiCaptureTaskParallelTest::displayNode_ = nullptr;

/*
 * @tc.name: TakeSurfaceCaptureForUiInvalidSurface
 * @tc.desc: Test TakeSurfaceCaptureForUI with invalid surface
 * @tc.type: FUNC
 * @tc.require: issueIA6QID
 */
HWTEST_F(RSUiCaptureTaskParallelTest, TakeSurfaceCaptureForUiInvalidSurface, Function | SmallTest | Level2)
{
    RSSurfaceNodeConfig config;
    auto surfaceNode = RSSurfaceNode::Create(config);
    auto callback = std::make_shared<CustomizedSurfaceCapture>();

    bool ret = rsInterfaces_->TakeSurfaceCaptureForUI(surfaceNode, callback);
    ASSERT_EQ(ret, true);
    ASSERT_EQ(CheckSurfaceCaptureCallback(callback), true);
#ifdef RS_ENABLE_UNI_RENDER
    ASSERT_EQ(callback->captureSuccess_, false);
#endif
}

/*
 * @tc.name: TakeSurfaceCaptureForUiSurfaceNode
 * @tc.desc: Test TakeSurfaceCaptureForUI with surface node
 * @tc.type: FUNC
 * @tc.require: issueIA6QID
 */
HWTEST_F(RSUiCaptureTaskParallelTest, TakeSurfaceCaptureForUiSurfaceNode, Function | SmallTest | Level2)
{
    SetUpSurface();

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    bool ret = rsInterfaces_->TakeSurfaceCaptureForUI(surfaceNode_, callback);
    ASSERT_EQ(ret, true);
    ASSERT_EQ(CheckSurfaceCaptureCallback(callback), true);
#ifdef RS_ENABLE_UNI_RENDER
    ASSERT_EQ(callback->captureSuccess_, true);
#endif
}

/*
 * @tc.name: TakeSurfaceCaptureForUiCanvasNode001
 * @tc.desc: Test TakeSurfaceCaptureForUI with canvas node
 * @tc.type: FUNC
 * @tc.require: issueIA6QID
 */
HWTEST_F(RSUiCaptureTaskParallelTest, TakeSurfaceCaptureForUiCanvasNode001, Function | SmallTest | Level2)
{
    SetUpSurface();

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    bool ret = rsInterfaces_->TakeSurfaceCaptureForUI(canvasNode_, callback);
    ASSERT_EQ(ret, true);
    ASSERT_EQ(CheckSurfaceCaptureCallback(callback), true);
#ifdef RS_ENABLE_UNI_RENDER
    ASSERT_EQ(callback->captureSuccess_, true);
#endif
}

/*
 * @tc.name: TakeSurfaceCaptureForUiCanvasNode002
 * @tc.desc: Test TakeSurfaceCaptureForUI with canvas node bounds inf
 * @tc.type: FUNC
 * @tc.require: issueIA6QID
 */
HWTEST_F(RSUiCaptureTaskParallelTest, TakeSurfaceCaptureForUiCanvasNode002, Function | SmallTest | Level2)
{
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds(0.0f, 0.0f, 30000, 30000);
    canvasNode->SetFrame(0.0f, 0.0f, 30000, 30000);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    bool ret = rsInterfaces_->TakeSurfaceCaptureForUI(canvasNode, callback);
    ASSERT_EQ(ret, true);
    ASSERT_EQ(CheckSurfaceCaptureCallback(callback), true);
#ifdef RS_ENABLE_UNI_RENDER
    ASSERT_EQ(callback->captureSuccess_, false);
#endif
}

/*
 * @tc.name: TakeSurfaceCaptureForUiCanvasNode003
 * @tc.desc: Test TakeSurfaceCaptureForUI with canvas node valid rect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUiCaptureTaskParallelTest, TakeSurfaceCaptureForUiCanvasNode003, Function | SmallTest | Level2)
{
    SetUpSurface();

    Drawing::Rect specifiedRect(1.f, 1.f, 5.f, 5.f);
    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    bool ret = rsInterfaces_->TakeSurfaceCaptureForUI(canvasNode_, callback, 1.f, 1.f, false, specifiedRect);
    ASSERT_EQ(ret, true);
    ASSERT_EQ(CheckSurfaceCaptureCallback(callback), true);
#ifdef RS_ENABLE_UNI_RENDER
    ASSERT_EQ(callback->captureSuccess_, true);
#endif
}

/*
 * @tc.name: TakeSurfaceCaptureForUiCanvasDrawingNode
 * @tc.desc: Test TakeSurfaceCaptureForUI with canvasdrawing node
 * @tc.type: FUNC
 * @tc.require: issueIA6QID
 */
HWTEST_F(RSUiCaptureTaskParallelTest, TakeSurfaceCaptureForUiCanvasDrawingNode, Function | SmallTest | Level2)
{
    SetUpSurface();

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    bool ret = rsInterfaces_->TakeSurfaceCaptureForUI(canvasDrawingNode_, callback);
    ASSERT_EQ(ret, true);
    ASSERT_EQ(CheckSurfaceCaptureCallback(callback), true);
#ifdef RS_ENABLE_UNI_RENDER
    ASSERT_EQ(callback->captureSuccess_, true);
#endif
}

/*
 * @tc.name: TakeSurfaceCaptureForUiProxyNode
 * @tc.desc: Test TakeSurfaceCaptureForUI with proxy node
 * @tc.type: FUNC
 * @tc.require: issueIA6QID
 */
HWTEST_F(RSUiCaptureTaskParallelTest, TakeSurfaceCaptureForUiProxyNode, Function | SmallTest | Level2)
{
    SetUpSurface();

    auto proxyNode = RSProxyNode::Create(canvasNode_->GetId());
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    bool ret = rsInterfaces_->TakeSurfaceCaptureForUI(proxyNode, callback);
    ASSERT_EQ(ret, false);
    ASSERT_EQ(CheckSurfaceCaptureCallback(callback), false);
#ifdef RS_ENABLE_UNI_RENDER
    ASSERT_EQ(callback->captureSuccess_, false);
#endif
}

/*
 * @tc.name: TakeSurfaceCaptureForUiSync001
 * @tc.desc: Test TakeSurfaceCaptureForUI sync is false
 * @tc.type: FUNC
 * @tc.require: issueIA6QID
 */
HWTEST_F(RSUiCaptureTaskParallelTest, TakeSurfaceCaptureForUiSync001, Function | SmallTest | Level2)
{
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds(0.0f, 0.0f, HALF_BOUNDS_WIDTH, HALF_BOUNDS_HEIGHT);
    canvasNode->SetFrame(0.0f, 0.0f, HALF_BOUNDS_WIDTH, HALF_BOUNDS_HEIGHT);
    canvasNode->SetBackgroundColor(Drawing::Color::COLOR_YELLOW);
    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    bool ret = rsInterfaces_->TakeSurfaceCaptureForUI(canvasNode, callback);
    ASSERT_EQ(ret, true);
    ASSERT_EQ(CheckSurfaceCaptureCallback(callback), true);
#ifdef RS_ENABLE_UNI_RENDER
    ASSERT_EQ(callback->captureSuccess_, false);
#endif
}

/*
 * @tc.name: TakeSurfaceCaptureForUiSync002
 * @tc.desc: Test TakeSurfaceCaptureForUI sync is true
 * @tc.type: FUNC
 * @tc.require: issueIA6QID
 */
HWTEST_F(RSUiCaptureTaskParallelTest, TakeSurfaceCaptureForUiSync002, Function | SmallTest | Level2)
{
    auto canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds(0.0f, 0.0f, HALF_BOUNDS_WIDTH, HALF_BOUNDS_HEIGHT);
    canvasNode->SetFrame(0.0f, 0.0f, HALF_BOUNDS_WIDTH, HALF_BOUNDS_HEIGHT);
    canvasNode->SetBackgroundColor(Drawing::Color::COLOR_YELLOW);
    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    bool ret = rsInterfaces_->TakeSurfaceCaptureForUI(canvasNode, callback, 1.0, 1.0, true);
    ASSERT_EQ(ret, true);
    ASSERT_EQ(CheckSurfaceCaptureCallback(callback), true);
#ifdef RS_ENABLE_UNI_RENDER
    ASSERT_EQ(callback->captureSuccess_, true);
#endif
}

/*
 * @tc.name: TakeSurfaceCaptureForUiScale001
 * @tc.desc: Test TakeSurfaceCaptureForUI scale
 * @tc.type: FUNC
 * @tc.require: issueIA6QID
 */
HWTEST_F(RSUiCaptureTaskParallelTest, TakeSurfaceCaptureForUiScale001, Function | SmallTest | Level2)
{
    SetUpSurface();

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    bool ret = rsInterfaces_->TakeSurfaceCaptureForUI(canvasNode_, callback, 0, 0);
    ASSERT_EQ(ret, true);
    ASSERT_EQ(CheckSurfaceCaptureCallback(callback), true);
#ifdef RS_ENABLE_UNI_RENDER
    ASSERT_EQ(callback->captureSuccess_, false);
#endif
}

/*
 * @tc.name: TakeSurfaceCaptureForUiScale002
 * @tc.desc: Test TakeSurfaceCaptureForUI scale
 * @tc.type: FUNC
 * @tc.require: issueIA6QID
 */
HWTEST_F(RSUiCaptureTaskParallelTest, TakeSurfaceCaptureForUiScale002, Function | SmallTest | Level2)
{
    SetUpSurface();

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    bool ret = rsInterfaces_->TakeSurfaceCaptureForUI(canvasNode_, callback, -1, -1);
    ASSERT_EQ(ret, true);
    ASSERT_EQ(CheckSurfaceCaptureCallback(callback), true);
#ifdef RS_ENABLE_UNI_RENDER
    ASSERT_EQ(callback->captureSuccess_, false);
#endif
}

/*
 * @tc.name: TakeSurfaceCaptureForUiScale003
 * @tc.desc: Test TakeSurfaceCaptureForUI scale
 * @tc.type: FUNC
 * @tc.require: issueIA6QID
 */
HWTEST_F(RSUiCaptureTaskParallelTest, TakeSurfaceCaptureForUiScale003, Function | SmallTest | Level2)
{
    SetUpSurface();

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    bool ret = rsInterfaces_->TakeSurfaceCaptureForUI(canvasNode_, callback, 10000, 10000);
    ASSERT_EQ(ret, true);
    ASSERT_EQ(CheckSurfaceCaptureCallback(callback), true);
#ifdef RS_ENABLE_UNI_RENDER
    ASSERT_EQ(callback->captureSuccess_, false);
#endif
}

/*
 * @tc.name: TakeSurfaceCaptureForUiNotOnTree
 * @tc.desc: Test TakeSurfaceCaptureForUI with node not on tree
 * @tc.type: FUNC
 * @tc.require: issueIA6QID
 */
HWTEST_F(RSUiCaptureTaskParallelTest, TakeSurfaceCaptureForUiNotOnTree, Function | SmallTest | Level2)
{
    SetUpSurface();

    displayNode_->RemoveChild(surfaceNode_);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    bool ret = rsInterfaces_->TakeSurfaceCaptureForUI(canvasNode_, callback);
    ASSERT_EQ(ret, true);
    ASSERT_EQ(CheckSurfaceCaptureCallback(callback), true);
#ifdef RS_ENABLE_UNI_RENDER
    ASSERT_EQ(callback->captureSuccess_, true);
#endif
}

/*
 * @tc.name: CreateResources001
 * @tc.desc: Test RSUiCaptureTaskParallel::CreateResources
 * @tc.type: FUNC
 * @tc.require: issueIA6QID
 */
HWTEST_F(RSUiCaptureTaskParallelTest, CreateResources001, Function | SmallTest | Level2)
{
    NodeId id = -1; // invalid id
    RSSurfaceCaptureConfig captureConfig;
    auto handle = std::make_shared<RSUiCaptureTaskParallel>(id, captureConfig);
    Drawing::Rect specifiedAreaRect(0.f, 0.f, 0.f, 0.f);
    ASSERT_EQ(handle->CreateResources(specifiedAreaRect), false);
    ASSERT_EQ(handle->captureConfig_.scaleX, 1.0f);
    ASSERT_EQ(handle->captureConfig_.scaleY, 1.0f);
    ASSERT_EQ(handle->pixelMap_, nullptr);
    ASSERT_EQ(handle->nodeDrawable_, nullptr);
    ASSERT_EQ(specifiedAreaRect.left_, 0.f);
    ASSERT_EQ(specifiedAreaRect.top_, 0.f);
    ASSERT_EQ(specifiedAreaRect.right_, 0.f);
    ASSERT_EQ(specifiedAreaRect.bottom_, 0.f);
}

/*
 * @tc.name: CreateResources002
 * @tc.desc: Test RSUiCaptureTaskParallel::CreateResources
 * @tc.type: FUNC
 * @tc.require: issueIA6QID
 */
HWTEST_F(RSUiCaptureTaskParallelTest, CreateResources002, Function | SmallTest | Level2)
{
    auto& nodeMap = RSMainThread::Instance()->GetContext().nodeMap;

    // RSSurfaceRenderNode
    NodeId surfaceRenderNodeId = 101;
    RSSurfaceCaptureConfig config1;
    auto surfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(surfaceRenderNodeId, std::make_shared<RSContext>(),
        true);
    nodeMap.RegisterRenderNode(surfaceRenderNode);
    auto surfaceRenderNodeHandle = std::make_shared<RSUiCaptureTaskParallel>(surfaceRenderNodeId, config1);
    Drawing::Rect surfaceRect(0.f, 0.f, 0.f, 0.f);
    ASSERT_EQ(surfaceRenderNodeHandle->CreateResources(surfaceRect), false);

    // RSCanvasRenderNode
    NodeId canvasRenderNodeId = 102;
    RSSurfaceCaptureConfig config2;
    auto canvasRenderNode = std::make_shared<RSCanvasRenderNode>(canvasRenderNodeId, std::make_shared<RSContext>(),
        true);
    nodeMap.RegisterRenderNode(canvasRenderNode);
    auto canvasRenderNodeHandle = std::make_shared<RSUiCaptureTaskParallel>(canvasRenderNodeId, config2);
    Drawing::Rect canvasRect(0.f, 0.f, 0.f, 0.f);
    ASSERT_EQ(canvasRenderNodeHandle->CreateResources(canvasRect), false);
}

/*
 * @tc.name: CreateResources003
 * @tc.desc: Test RSUiCaptureTaskParallel::CreateResources
 * @tc.type: FUNC
 * @tc.require: issueIA6QID
 */
HWTEST_F(RSUiCaptureTaskParallelTest, CreateResources003, Function | SmallTest | Level2)
{
    auto& nodeMap = RSMainThread::Instance()->GetContext().nodeMap;

    NodeId nodeId = 103;
    NodeId parentNodeId = 1003;
    RSSurfaceCaptureConfig config;
    auto renderNode = std::make_shared<RSSurfaceRenderNode>(nodeId, std::make_shared<RSContext>(), true);
    renderNode->renderProperties_.SetBoundsWidth(1024.0f);
    renderNode->renderProperties_.SetBoundsHeight(1024.0f);
    nodeMap.RegisterRenderNode(renderNode);
    Drawing::Rect specifiedAreaRect(0.f, 0.f, 0.f, 0.f);

    auto renderNodeHandle = std::make_shared<RSUiCaptureTaskParallel>(nodeId, config);
    ASSERT_EQ(renderNodeHandle->CreateResources(specifiedAreaRect), true);

    auto parent1 = std::make_shared<RSSurfaceRenderNode>(parentNodeId, std::make_shared<RSContext>(), true);
    parent1->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    parent1->hasSubNodeShouldPaint_ = false;
    renderNode->parent_ = parent1;
    ASSERT_EQ(renderNodeHandle->CreateResources(specifiedAreaRect), true);

    auto parent2 = std::make_shared<RSSurfaceRenderNode>(parentNodeId, std::make_shared<RSContext>(), true);
    parent2->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    parent2->hasSubNodeShouldPaint_ = true;
    renderNode->parent_ = parent2;
    ASSERT_EQ(renderNodeHandle->CreateResources(specifiedAreaRect), true);

    auto parent3 = std::make_shared<RSSurfaceRenderNode>(parentNodeId, std::make_shared<RSContext>(), true);
    parent3->nodeType_ = RSSurfaceNodeType::LEASH_WINDOW_NODE;
    parent3->hasSubNodeShouldPaint_ = true;
    parent3->lastFrameUifirstFlag_ = MultiThreadCacheType::NONFOCUS_WINDOW;
    parent3->renderProperties_.SetBoundsWidth(1024.0f);
    parent3->renderProperties_.SetBoundsHeight(1024.0f);
    renderNode->parent_ = parent3;
    ASSERT_EQ(renderNodeHandle->CreateResources(specifiedAreaRect), true);
}

/*
 * @tc.name: RSUiCaptureTaskParallel_IsRectValid001
 * @tc.desc: Test RSUiCaptureTaskParallel::IsRectValid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUiCaptureTaskParallelTest, RSUiCaptureTaskParallel_IsRectValid001, Function | SmallTest | Level2)
{
    NodeId id = -1; // invalid id
    RSSurfaceCaptureConfig captureConfig;
    auto handle = std::make_shared<RSUiCaptureTaskParallel>(id, captureConfig);
    auto node = RSTestUtil::CreateSurfaceNode();
    Drawing::Rect specifiedAreaRect(0.f, 0.f, 0.f, 0.f);
    ASSERT_EQ(handle->IsRectValid(id, specifiedAreaRect), false);
}

/*
 * @tc.name: RSUiCaptureTaskParallel_IsRectValid002
 * @tc.desc: Test RSUiCaptureTaskParallel::IsRectValid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUiCaptureTaskParallelTest, RSUiCaptureTaskParallel_IsRectValid002, Function | SmallTest | Level2)
{
    auto& nodeMap = RSMainThread::Instance()->GetContext().nodeMap;
    NodeId surfaceRenderNodeId = 104;
    RSSurfaceCaptureConfig config;
    auto surfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(surfaceRenderNodeId, std::make_shared<RSContext>(),
        true);
    nodeMap.RegisterRenderNode(surfaceRenderNode);
    auto surfaceRenderNodeHandle = std::make_shared<RSUiCaptureTaskParallel>(surfaceRenderNodeId, config);
    Drawing::Rect specifiedAreaRect(0.f, 0.f, 0.f, 0.f);
    ASSERT_EQ(surfaceRenderNodeHandle->IsRectValid(surfaceRenderNodeId, specifiedAreaRect), false);
}

/*
 * @tc.name: RSUiCaptureTaskParallel_IsRectValid003
 * @tc.desc: Test RSUiCaptureTaskParallel::IsRectValid
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUiCaptureTaskParallelTest, RSUiCaptureTaskParallel_IsRectValid003, Function | SmallTest | Level2)
{
    auto& nodeMap = RSMainThread::Instance()->GetContext().nodeMap;
    NodeId surfaceRenderNodeId = 105;
    RSSurfaceCaptureConfig config;
    auto surfaceRenderNode = std::make_shared<RSSurfaceRenderNode>(surfaceRenderNodeId, std::make_shared<RSContext>(),
        true);
    nodeMap.RegisterRenderNode(surfaceRenderNode);
    auto surfaceRenderNodeHandle = std::make_shared<RSUiCaptureTaskParallel>(surfaceRenderNodeId, config);
    surfaceRenderNode->renderProperties_.SetBoundsWidth(HALF_BOUNDS_WIDTH);
    surfaceRenderNode->renderProperties_.SetBoundsHeight(HALF_BOUNDS_HEIGHT);
    Drawing::Rect specifiedAreaRect(0.f, 0.f, DEFAULT_BOUNDS_WIDTH, DEFAULT_BOUNDS_HEIGHT);
    ASSERT_EQ(surfaceRenderNodeHandle->IsRectValid(surfaceRenderNodeId, specifiedAreaRect), false);
}

/*
 * @tc.name: RSUiCaptureTaskParallel_CreatePixelMapByRect
 * @tc.desc: Test RSUiCaptureTaskParallel::CreatePixelMapByRect
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUiCaptureTaskParallelTest, RSUiCaptureTaskParallel_CreatePixelMapByRect, Function | SmallTest | Level2)
{
    NodeId id = -1; // invalid id
    RSSurfaceCaptureConfig captureConfig;
    auto handle = std::make_shared<RSUiCaptureTaskParallel>(id, captureConfig);
    auto node = RSTestUtil::CreateSurfaceNode();
    Drawing::Rect specifiedAreaRect(0.f, 0.f, 0.f, 0.f);
    ASSERT_EQ(handle->CreatePixelMapByRect(specifiedAreaRect), nullptr);
}

/*
 * @tc.name: Run001
 * @tc.desc: Test RSUiCaptureTaskParallel::Run
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUiCaptureTaskParallelTest, Run001, Function | SmallTest | Level2)
{
    auto renderEngine = std::make_shared<RSRenderEngine>();
    renderEngine->Init();
    RSUniRenderThread::Instance().uniRenderEngine_ = renderEngine;
    auto mockCallback = sptr<MockSurfaceCaptureCallback>(new MockSurfaceCaptureCallback);
    auto handle = BuildTaskParallel(-1, 0.0f, 0.0f);
    Drawing::Rect specifiedAreRect(0.f, 0.f, 0.f, 0.f);
    ASSERT_EQ(handle->Run(mockCallback, specifiedAreRect), false);

    handle->CreateResources(specifiedAreRect);
    ASSERT_EQ(handle->Run(mockCallback, specifiedAreRect), false);
    RSUniRenderThread::Instance().uniRenderEngine_ = nullptr;
}

/*
 * @tc.name: Run002
 * @tc.desc: Test RSUiCaptureTaskParallel::Run
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUiCaptureTaskParallelTest, Run002, Function | SmallTest | Level2)
{
    auto renderEngine = std::make_shared<RSRenderEngine>();
    renderEngine->Init();
    RSUniRenderThread::Instance().uniRenderEngine_ = renderEngine;
    auto mockCallback = sptr<MockSurfaceCaptureCallback>(new MockSurfaceCaptureCallback);
    auto handle = BuildTaskParallel(200, 1024.0f, 1024.0f);
    Drawing::Rect specifiedAreRect(0.f, 0.f, 0.f, 0.f);
    ASSERT_EQ(handle->Run(mockCallback, specifiedAreRect), false);

    handle->CreateResources(specifiedAreRect);
    ASSERT_EQ(handle->Run(mockCallback, specifiedAreRect), true);

    handle->nodeDrawable_ = nullptr;
    ASSERT_EQ(handle->Run(mockCallback, specifiedAreRect), false);
    RSUniRenderThread::Instance().uniRenderEngine_ = nullptr;
}

/*
 * @tc.name: RSUiCaptureTaskParallel_CreatePixelMapByNode
 * @tc.desc: Test RSUiCaptureTaskParallel::CreatePixelMapByNode
 * @tc.type: FUNC
 * @tc.require: issueIA6QID
 */
HWTEST_F(RSUiCaptureTaskParallelTest, RSUiCaptureTaskParallel_CreatePixelMapByNode, Function | SmallTest | Level2)
{
    NodeId id = -1; // invalid id
    RSSurfaceCaptureConfig captureConfig;
    auto handle = std::make_shared<RSUiCaptureTaskParallel>(id, captureConfig);
    auto node = RSTestUtil::CreateSurfaceNode();
    ASSERT_EQ(handle->CreatePixelMapByNode(node), nullptr);
}

/*
 * @tc.name: RSUiCaptureTaskParallel_CreateSurfaceSyncCopyTask
 * @tc.desc: Test RSUiCaptureTaskParallel::CreateSurfaceSyncCopyTask
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUiCaptureTaskParallelTest, RSUiCaptureTaskParallel_CreateSurfaceSyncCopyTask, Function | SmallTest | Level2)
{
    auto node = RSTestUtil::CreateSurfaceNode();
    auto mainThread = RSMainThread::Instance();

    mainThread->context_->nodeMap.RegisterRenderNode(node);

    auto mockCallback = sptr<MockSurfaceCaptureCallback>(new MockSurfaceCaptureCallback);
    RSSurfaceCaptureConfig captureConfig;
    auto pixelMap = std::make_unique<Media::PixelMap>();
    ASSERT_NE(pixelMap, nullptr);
    auto surface = std::make_shared<Drawing::Surface>();
    ASSERT_NE(surface, nullptr);
#ifdef RS_ENABLE_UNI_RENDER
    auto copytask =
        RSUiCaptureTaskParallel::CreateSurfaceSyncCopyTask(surface, std::move(pixelMap),
            node->GetId(), captureConfig, mockCallback);

    ASSERT_FALSE(copytask);
    mainThread->context_->nodeMap.UnregisterRenderNode(node->GetId());
#endif
}

/*
 * @tc.name: CreateClientPixelMap
 * @tc.desc: CreateClientPixelMap
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(RSUiCaptureTaskParallelTest, CreateClientPixelMap, Function | SmallTest | Level2)
{
    // TEST0: Vail CaputreSzie and alloc shrae mem
    {
        Drawing::Rect rect = {0, 0, 1260, 2720};
        RSSurfaceCaptureConfig captureConfig;
        captureConfig.captureType = SurfaceCaptureType::UICAPTURE;
        auto pixelMap = RSCapturePixelMapManager::GetClientCapturePixelMap(rect, captureConfig,
            UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL);
        EXPECT_EQ(pixelMap != nullptr, true);
    }

    // TEST1: dmna
    {
        Drawing::Rect rect = {0, 0, 1260, 2720};
        RSSurfaceCaptureConfig captureConfig;
        captureConfig.captureType = SurfaceCaptureType::UICAPTURE;
        captureConfig.useDma = true;
        auto pixelMap = RSCapturePixelMapManager::GetClientCapturePixelMap(rect, captureConfig,
            UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL);
        EXPECT_EQ(pixelMap != nullptr, true);
    }

    // TEST2: Invaild specifiedAreadRect
    {
        Drawing::Rect nodeRect = {0, 0, 1260, 2720};
        RSSurfaceCaptureConfig captureConfig;
        captureConfig.captureType = SurfaceCaptureType::UICAPTURE;
        auto pixelMap = RSCapturePixelMapManager::GetClientCapturePixelMap(nodeRect, captureConfig,
            UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL);
        EXPECT_EQ(pixelMap != nullptr, true);

        EXPECT_EQ(pixelMap->GetWidth(), nodeRect.GetWidth());
        EXPECT_EQ(pixelMap->GetHeight(), nodeRect.GetHeight());
    }

    
    // TEST3: vaild specifiedAreadRect
    {
        Drawing::Rect rect = {0, 0, 1260, 2720};
        Drawing::Rect specifiAreaRect = {0, 0, 1259, 2710};
        RSSurfaceCaptureConfig captureConfig;
        captureConfig.captureType = SurfaceCaptureType::UICAPTURE;
        auto pixelMap = RSCapturePixelMapManager::GetClientCapturePixelMap(rect, captureConfig,
            UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL, specifiAreaRect);
        EXPECT_EQ(pixelMap != nullptr, true);

        EXPECT_EQ(pixelMap->GetWidth(), specifiAreaRect.GetWidth());
        EXPECT_EQ(pixelMap->GetHeight(), specifiAreaRect.GetHeight());
    }
}
} // namespace Rosen
} // namespace OHOS
