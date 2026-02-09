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
#include "common/rs_background_thread.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_canvas_drawing_node.h"
#include "feature/capture/rs_surface_capture_task_parallel.h"
#include "pipeline/render_thread/rs_uni_render_engine.h"
#include "pipeline/rs_context.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_test_util.h"
#include "feature/capture/rs_capture_pixelmap_manager.h"
#include "transaction/rs_interfaces.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "platform/common/rs_system_properties.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "ipc_callbacks/surface_capture_callback_stub.h"
#include "pipeline/render_thread/rs_render_engine.h"
#include "pipeline/rs_screen_render_node.h"
#include "pipeline/rs_logical_display_render_node.h"
#include "surface_buffer_impl.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
constexpr uint32_t MAX_TIME_WAITING_FOR_CALLBACK = 200;
constexpr uint32_t SLEEP_TIME_IN_US = 10000; // 10ms
constexpr uint32_t SLEEP_TIME_FOR_PROXY = 100000; // 100ms
constexpr uint32_t DEFAULT_WEIGHT = 1260;
constexpr uint32_t DEFAULT_HIGHT = 2720;

class CustomizedSurfaceCapture : public SurfaceCaptureCallback {
public:
    CustomizedSurfaceCapture() {}

    ~CustomizedSurfaceCapture() override {}

    void OnSurfaceCapture(std::shared_ptr<Media::PixelMap> pixelmap) override
    {
        testSuccess = (pixelmap != nullptr);
        isCallbackCalled_ = true;
    }

    void OnSurfaceCaptureHDR(std::shared_ptr<Media::PixelMap> pixelmap,
        std::shared_ptr<Media::PixelMap> pixelMapHDR) override {}

    bool IsTestSuccess()
    {
        return testSuccess;
    }

    bool IsCallbackCalled()
    {
        return isCallbackCalled_;
    }

    void Reset()
    {
        testSuccess = false;
        isCallbackCalled_ = false;
    }

private:
    bool testSuccess = false;
    bool isCallbackCalled_ = false;
}; // class CustomizedSurfaceCapture

class RSSurfaceCaptureCallbackStubMock : public RSSurfaceCaptureCallbackStub {
public:
    RSSurfaceCaptureCallbackStubMock() = default;
    virtual ~RSSurfaceCaptureCallbackStubMock() = default;
    void OnSurfaceCapture(NodeId id, const RSSurfaceCaptureConfig& captureConfig,
        Media::PixelMap* pixelmap, CaptureError captureErrorCode = CaptureError::CAPTURE_OK,
        Media::PixelMap* pixelmapHDR = nullptr) override {};
};

class SurfaceImplMock : public Drawing::SurfaceImpl {
public:
    SurfaceImplMock() {};
    ~SurfaceImplMock() override {};

    Drawing::AdapterType GetType() const override { return Drawing::AdapterType::DDGR_ADAPTER; };
    bool Bind(const Drawing::Bitmap& bitmap) override { return true; };
#ifdef RS_ENABLE_GPU
    bool Bind(const Drawing::Image& image) override { return true; };
    bool Bind(const Drawing::FrameBuffer& frameBuffer) override { return true; };
#endif
    std::shared_ptr<Drawing::Canvas> GetCanvas() const override { return nullptr; };
    std::shared_ptr<Drawing::Image> GetImageSnapshot() const override { return nullptr; };
    std::shared_ptr<Drawing::Image> GetImageSnapshot(const Drawing::RectI& bounds,
        bool allowRefCache = true) const override
    {
        return nullptr;
    };
    std::shared_ptr<Drawing::Surface> MakeSurface(int width, int height) const override { return nullptr; };
    std::shared_ptr<Drawing::Surface> MakeSurface(const Drawing::ImageInfo& imageInfo) const override
    {
        return nullptr;
    };
    Drawing::BackendTexture GetBackendTexture(Drawing::BackendAccess access) const override
    {
        return Drawing::BackendTexture(returnValidTexture_);
    }
    void FlushAndSubmit(bool syncCpu) override {};
    Drawing::SemaphoresSubmited Flush(Drawing::FlushInfo *drawingflushInfo = nullptr) override
    {
        return Drawing::SemaphoresSubmited::DRAWING_SUBMIT_NO;
    }
#ifdef RS_ENABLE_VK
    void Wait(int32_t time, const VkSemaphore& semaphore) override {};
    void SetDrawingArea(const std::vector<Drawing::RectI>& rects) override {};
    void ClearDrawingArea() override {};
#endif
    void SetHeadroom(float headroom) {};
    float GetHeadroom() const { return 0.0; };
    int Width() const override { return 0; };
    int Height() const override { return 0; };

    bool returnValidTexture_ = true;
};

class RSSurfaceCaptureTaskParallelTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    bool CheckSurfaceCaptureCallback();
    static std::shared_ptr<CustomizedSurfaceCapture> surfaceCaptureCb_;
    static RSInterfaces* rsInterfaces_;
    static std::shared_ptr<RSSurfaceNode> surfaceNode_;
    static std::shared_ptr<RSSurfaceNode> CreateSurface(std::string surfaceName);
    static std::shared_ptr<RSCanvasNode> canvasNode_;
};

RSInterfaces* RSSurfaceCaptureTaskParallelTest::rsInterfaces_ = nullptr;
std::shared_ptr<RSSurfaceNode> RSSurfaceCaptureTaskParallelTest::surfaceNode_ = nullptr;
std::shared_ptr<CustomizedSurfaceCapture> RSSurfaceCaptureTaskParallelTest::surfaceCaptureCb_ = nullptr;
std::shared_ptr<RSCanvasNode> RSSurfaceCaptureTaskParallelTest::canvasNode_ = nullptr;

bool RSSurfaceCaptureTaskParallelTest::CheckSurfaceCaptureCallback()
{
    if (surfaceCaptureCb_ == nullptr) {
        return false;
    }
    uint32_t times = 0;
    do {
        if (surfaceCaptureCb_->IsCallbackCalled()) {
            return true;
        }
        usleep(SLEEP_TIME_IN_US);
        ++times;
    } while (times <= MAX_TIME_WAITING_FOR_CALLBACK);
    return false;
}

std::shared_ptr<RSSurfaceNode> RSSurfaceCaptureTaskParallelTest::CreateSurface(std::string surfaceNodeName)
{
    RSSurfaceNodeConfig config;
    config.SurfaceNodeName = surfaceNodeName;
    return RSSurfaceNode::Create(config);
}

void RSSurfaceCaptureTaskParallelTest::SetUpTestCase()
{
    RSTestUtil::InitRenderNodeGC();
    rsInterfaces_ = &(RSInterfaces::GetInstance());
    if (rsInterfaces_ == nullptr) {
        std::cout << "rsInterfaces_ is nullptr" << std::endl;
        return;
    }

    surfaceNode_ = CreateSurface("SurfaceCaptureTestNode");
    if (surfaceNode_ == nullptr) {
        return;
    }
    surfaceNode_->SetBounds({0, 0, DEFAULT_WEIGHT, DEFAULT_HIGHT});
    canvasNode_ = RSCanvasNode::Create();
    if (canvasNode_ == nullptr) {
        return;
    }
    canvasNode_->SetBounds({0, 0, DEFAULT_WEIGHT, DEFAULT_HIGHT});
    surfaceNode_->AddChild(canvasNode_, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    surfaceCaptureCb_ = std::make_shared<CustomizedSurfaceCapture>();
    if (surfaceCaptureCb_ == nullptr) {
        return;
    }
}

void RSSurfaceCaptureTaskParallelTest::TearDownTestCase()
{
    surfaceCaptureCb_->Reset();
    surfaceCaptureCb_ = nullptr;
    rsInterfaces_ = nullptr;
    surfaceNode_ = nullptr;
    canvasNode_ = nullptr;
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
}
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
    auto pxiemap = task.CreatePixelMapBySurfaceNode(node, false);
    EXPECT_EQ(pxiemap, nullptr);

    node->renderProperties_.SetBoundsWidth(1.0f);
    node->renderProperties_.SetBoundsHeight(1.0f);
    pxiemap = task.CreatePixelMapBySurfaceNode(node, false);
    EXPECT_NE(pxiemap, nullptr);
    pxiemap = task.CreatePixelMapBySurfaceNode(node, true);
    EXPECT_NE(pxiemap, nullptr);
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
    std::shared_ptr<RSLogicalDisplayRenderNode> node = std::make_shared<RSLogicalDisplayRenderNode>(0, config);
    ASSERT_EQ(nullptr, task.CreatePixelMapByDisplayNode(node));
}

/*
 * @tc.name: CreatePixelMapByDisplayNode003
 * @tc.desc: Test RSSurfaceCaptureTaskParallel.CreatePixelMapByDisplayNode with pixelmap not nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAHND9
*/
HWTEST_F(RSSurfaceCaptureTaskParallelTest, CreatePixelMapByDisplayNode003, TestSize.Level2)
{
    RSSurfaceCaptureConfig captureConfig;
    RSSurfaceCaptureTaskParallel task(0, captureConfig);
    RSDisplayNodeConfig config;
    std::shared_ptr<RSLogicalDisplayRenderNode> node = std::make_shared<RSLogicalDisplayRenderNode>(0, config);

    NodeId id = 0;
    ScreenId screenId = 1;
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, screenId, context);
    ASSERT_NE(screenNode, nullptr);
    screenNode->AddChild(node);
    ASSERT_EQ(nullptr, task.CreatePixelMapByDisplayNode(node));
    ASSERT_EQ(nullptr, task.CreatePixelMapByDisplayNode(node, true));
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
 * @tc.name: CreateResources004
 * @tc.desc: Test RSSurfaceCaptureTaskParallel.CreateResources with node not nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAHND9
 */
HWTEST_F(RSSurfaceCaptureTaskParallelTest, CreateResources004, TestSize.Level2)
{
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 1.0f;
    captureConfig.scaleY = 1.0f;
    captureConfig.useCurWindow = true;

    NodeId nodeId = 1000123;
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(nodeId);
    node->GetMutableRenderProperties().SetBounds({ 0, 0, 1260, 2720 });
    ASSERT_NE(node, nullptr);
    node->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE;
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    RSRenderNodeMap& nodeMap = mainThread->GetContext().GetMutableNodeMap();
    nodeMap.RegisterRenderNode(node);

    node->shouldPaint_ = true;
    RSSurfaceCaptureTaskParallel task(nodeId, captureConfig);
    ASSERT_EQ(true, task.CreateResources());
}

/*
 * @tc.name: CreateResources005
 * @tc.desc: Test RSSurfaceCaptureTaskParallel.CreateResources with node not nullptr
 * @tc.type: FUNC
 * @tc.require: issueIAHND9
 */
HWTEST_F(RSSurfaceCaptureTaskParallelTest, CreateResources005, TestSize.Level2)
{
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 1.0f;
    captureConfig.scaleY = 1.0f;
    captureConfig.useCurWindow = true;

    NodeId nodeId = 1000124;
    RSDisplayNodeConfig config;
    std::shared_ptr<RSLogicalDisplayRenderNode> node = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    ASSERT_NE(node, nullptr);
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    RSRenderNodeMap& nodeMap = mainThread->GetContext().GetMutableNodeMap();
    nodeMap.RegisterRenderNode(node);

    RSSurfaceCaptureTaskParallel task(nodeId, captureConfig);
    EXPECT_EQ(false, task.CreateResources());

    captureConfig.isHdrCapture = true;
    RSSurfaceCaptureTaskParallel task1(nodeId, captureConfig);
    task1.CreateResources();

    NodeId id = 0;
    ScreenId screenId = 1;
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, screenId, context);
    ASSERT_NE(screenNode, nullptr);

    node->parent_ = screenNode;
    RSSurfaceCaptureTaskParallel task2(nodeId, captureConfig);
    ASSERT_EQ(false, task2.CreateResources());

    RSSurfaceCaptureTaskParallel task3(nodeId, captureConfig);
    screenNode->CollectHdrStatus(HdrStatus::HDR_VIDEO);
    ASSERT_EQ(false, task3.CreateResources());
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
    ASSERT_EQ(nullptr, task.pixelMap_);
    RSSurfaceCaptureParam captureParam;
    ASSERT_EQ(false, task.Run(nullptr, captureParam));
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
    RSSurfaceCaptureParam captureParam;
    ASSERT_EQ(false, task.Run(nullptr, captureParam));
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
    RSSurfaceCaptureParam captureParam;
    ASSERT_EQ(false, task.Run(nullptr, captureParam));
}

/*
 * @tc.name: Run004
 * @tc.desc: Test RSSurfaceCaptureTaskParallel.Run004, TEST CreateResources
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
    RSSurfaceCaptureParam captureParam;
    ASSERT_EQ(false, task.Run(nullptr, captureParam));

    auto renderEngine = std::make_shared<RSRenderEngine>();
    renderEngine->Init();
    RSUniRenderThread::Instance().uniRenderEngine_ = renderEngine;
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);

    NodeId nodeId= 1000235;
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(nodeId);
    node->GetMutableRenderProperties().SetBounds({0, 0, 1260, 2720});
    ASSERT_NE(node, nullptr);
    node->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE;
    RSRenderNodeMap& nodeMap = mainThread->GetContext().GetMutableNodeMap();
    nodeMap.RegisterRenderNode(node);
    // define callBack

    RSSurfaceCaptureTaskParallel task1(nodeId, captureConfig);
    sptr<RSISurfaceCaptureCallback> callback = new RSSurfaceCaptureCallbackStubMock();

    ASSERT_EQ(callback != nullptr, true);
    bool ret = task1.CreateResources();
    EXPECT_EQ(ret, true);

    ASSERT_EQ(task1.Run(callback, captureParam), true);

    task1.finalRotationAngle_ = RS_ROTATION_90;
    ASSERT_EQ(task1.Run(callback, captureParam), true);
    // Reset
    nodeMap.UnregisterRenderNode(nodeId);
    RSUniRenderThread::Instance().uniRenderEngine_ = nullptr;
}

/*
 * @tc.name: Run006
 * @tc.desc: Test RSSurfaceCaptureTaskParallel.Run006
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceCaptureTaskParallelTest, Run006, TestSize.Level2)
{
    NodeId id = 0;
    RSSurfaceCaptureConfig captureConfig;
    RSSurfaceCaptureTaskParallel task(id, captureConfig);
    task.displayNodeDrawable_ = nullptr;
    RSSurfaceCaptureParam captureParam;
    ASSERT_EQ(false, task.Run(nullptr, captureParam));

    auto renderEngine = std::make_shared<RSRenderEngine>();
    renderEngine->Init();
    RSUniRenderThread::Instance().uniRenderEngine_ = renderEngine;
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);

    NodeId nodeId= 1000777;
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(nodeId);
    node->GetMutableRenderProperties().SetBounds({0, 0, 1260, 2720});
    ASSERT_NE(node, nullptr);
    node->nodeType_ = RSSurfaceNodeType::SELF_DRAWING_WINDOW_NODE;
    RSRenderNodeMap& nodeMap = mainThread->GetContext().GetMutableNodeMap();
    nodeMap.RegisterRenderNode(node);
    // define callBack
    Drawing::Rect rect = {0, 0, 1260, 2720};
    RSSurfaceCaptureTaskParallel task1(nodeId, captureConfig);

    sptr<RSISurfaceCaptureCallback> callback = new RSSurfaceCaptureCallbackStubMock();

    ASSERT_EQ(callback != nullptr, true);
    bool ret = task1.CreateResources();
    EXPECT_EQ(ret, true);
    task1.surfaceNodeDrawable_ = std::static_pointer_cast<DrawableV2::RSRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(node));
    ASSERT_EQ(task1.Run(callback, captureParam), true);
    // Reset
    nodeMap.UnregisterRenderNode(nodeId);
    RSUniRenderThread::Instance().uniRenderEngine_ = nullptr;
}

/*
 * @tc.name: RunWithDisplayNode
 * @tc.desc: Test RSSurfaceCaptureTaskParallel.RunWithDisplayNode
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceCaptureTaskParallelTest, RunWithDisplayNode, TestSize.Level2)
{
    NodeId nodeId = 0;
    RSDisplayNodeConfig config;
    auto displayRenderNode = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    ASSERT_NE(displayRenderNode, nullptr);
    auto displayNodeDrawable = std::static_pointer_cast<DrawableV2::RSRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(displayRenderNode));
    ASSERT_NE(displayNodeDrawable, nullptr);

    Media::InitializationOptions opts;
    opts.size.width = 100;
    opts.size.height = 100;

    sptr<RSISurfaceCaptureCallback> callback = new RSSurfaceCaptureCallbackStubMock();
    ASSERT_EQ(callback != nullptr, true);

    auto renderEngine = std::make_shared<RSRenderEngine>();
    renderEngine->Init();
    RSUniRenderThread::Instance().uniRenderEngine_ = renderEngine;

    RSSurfaceCaptureConfig captureConfig;
    auto taskHandle = std::make_shared<RSSurfaceCaptureTaskParallel>(nodeId, captureConfig);
    taskHandle->pixelMap_ = Media::PixelMap::Create(opts);
    taskHandle->displayNodeDrawable_ = displayNodeDrawable;
    RSSurfaceCaptureParam captureParam;
    ASSERT_EQ(true, taskHandle->Run(callback, captureParam));

    captureConfig.mainScreenRect = Drawing::Rect(0, 0, 10, 0);
    taskHandle = std::make_shared<RSSurfaceCaptureTaskParallel>(nodeId, captureConfig);
    taskHandle->pixelMap_ = Media::PixelMap::Create(opts);
    taskHandle->displayNodeDrawable_ = displayNodeDrawable;
    ASSERT_EQ(true, taskHandle->Run(callback, captureParam));

    captureConfig.mainScreenRect = Drawing::Rect(0, 0, 10, 10);
    taskHandle = std::make_shared<RSSurfaceCaptureTaskParallel>(nodeId, captureConfig);
    taskHandle->pixelMap_ = Media::PixelMap::Create(opts);
    taskHandle->displayNodeDrawable_ = displayNodeDrawable;
    ASSERT_EQ(true, taskHandle->Run(callback, captureParam));

    // Reset
    RSUniRenderThread::Instance().uniRenderEngine_ = nullptr;
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
    bool needSyncSurface = false;
    task.CheckModifiers(nodeId, captureConfig.useCurWindow, &needSyncSurface);
    ASSERT_TRUE(mainThread->IsOcclusionNodesNeedSync(nodeId, true));
    needSyncSurface = true;
    task.CheckModifiers(nodeId, captureConfig.useCurWindow, &needSyncSurface);
}

/*
 * @tc.name: CreateClientPixelMap
 * @tc.desc: Test CreateClientPixelMap
 * @tc.type: FUNC
 * @tc.require:
*/
HWTEST_F(RSSurfaceCaptureTaskParallelTest, CreateClientPixelMap, TestSize.Level2)
{
    // TEST0: vail Capture Size and Alloc Share mem/Heap mem
    {
        Drawing::Rect rect = {0, 0 ,1260, 2720};
        RSSurfaceCaptureConfig captureConfig;
        auto pixelMap = RSCapturePixelMapManager::GetClientCapturePixelMap(rect, captureConfig,
            UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL);
        EXPECT_EQ(pixelMap != nullptr, true);
        //Heap Mem
        auto pixelMap1 = RSCapturePixelMapManager::CreatePixelMap(rect, captureConfig);
        bool ret = RSCapturePixelMapManager::AttachHeapMem(pixelMap1);
        EXPECT_EQ(ret, true);
    }

    // TEST1:: Vail Caputre size and use DMA
    {
        Drawing::Rect rect = {0, 0, 1260, 2720};
        RSSurfaceCaptureConfig captureConfig;
        captureConfig.useDma = true;
        auto pixelMap = RSCapturePixelMapManager::GetClientCapturePixelMap(rect, captureConfig,
            UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL);
        EXPECT_EQ(pixelMap != nullptr, true);
    }

    // TEST2:: Invalid scalex01
    {
        Drawing::Rect rect = {0, 0, 1260, 2720};
        RSSurfaceCaptureConfig captureConfig;
        captureConfig.scaleX = 2;
        auto pixelMap = RSCapturePixelMapManager::GetClientCapturePixelMap(rect, captureConfig,
            UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL);
        EXPECT_EQ(pixelMap == nullptr, true);
    }

    // TESt: Invalid scalex01

    {
        Drawing::Rect rect = {0, 0 ,1260, 2720};
        RSSurfaceCaptureConfig captureConfig;
        captureConfig.scaleX = 0.0;
        auto pixelMap = RSCapturePixelMapManager::GetClientCapturePixelMap(rect, captureConfig,
            UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL);
        EXPECT_EQ(pixelMap == nullptr, true);
    }
}

/*
 * @tc.name: TestSurfaceCaputreIt
 * @tc.desc: Test TestSurfaceCaputreIt
 * @tc.type: FUNC
 * @tc.require:
*/
HWTEST_F(RSSurfaceCaptureTaskParallelTest, TestSurfaceCaputreIt, TestSize.Level2)
{
    if (RSUniRenderJudgement::IsUniRender()) {
        RSSurfaceCaptureConfig captureConfig;
        captureConfig.useCurWindow = true;

        bool ret = rsInterfaces_->TakeSurfaceCapture(surfaceNode_, surfaceCaptureCb_);
        EXPECT_EQ(ret, true);
        EXPECT_EQ(CheckSurfaceCaptureCallback(), true);
        EXPECT_EQ(surfaceCaptureCb_->IsTestSuccess(), true);
        // DMA Surface Cpautre
        surfaceCaptureCb_->Reset();
        captureConfig.useDma = true;
        ret = rsInterfaces_->TakeSurfaceCapture(surfaceNode_, surfaceCaptureCb_, captureConfig);
        EXPECT_EQ(ret, true);
        EXPECT_EQ(CheckSurfaceCaptureCallback(), true);
        EXPECT_EQ(surfaceCaptureCb_->IsTestSuccess(), true);
        // No DMA Canvas Node
        surfaceCaptureCb_->Reset();
        ret = rsInterfaces_->TakeSurfaceCaptureForUI(surfaceNode_, surfaceCaptureCb_, 0.5, 0.5);
        EXPECT_EQ(ret, true);
        EXPECT_EQ(CheckSurfaceCaptureCallback(), true);
        EXPECT_EQ(surfaceCaptureCb_->IsTestSuccess(), true);
        // DMA Canvas Node
        surfaceCaptureCb_->Reset();
        captureConfig.useDma = true;
        ret = rsInterfaces_->TakeSurfaceCaptureForUI(surfaceNode_, surfaceCaptureCb_, 0.5, 0.5);
        EXPECT_EQ(ret, true);
        EXPECT_EQ(CheckSurfaceCaptureCallback(), true);
        EXPECT_EQ(surfaceCaptureCb_->IsTestSuccess(), true);
    }
}

/*
 * @tc.name: CaptureDisplayNode
 * @tc.desc: Test CaptureDisplayNode
 * @tc.type: FUNC
 * @tc.require: #ICQ74B
*/
HWTEST_F(RSSurfaceCaptureTaskParallelTest, CaptureDisplayNode, TestSize.Level2)
{
    NodeId id = 0;
    RSSurfaceCaptureConfig captureConfig;
    RSSurfaceCaptureParam captureParam;
    captureParam.needCaptureSpecialLayer = true;
    auto type = RSPaintFilterCanvas::ScreenshotType::SDR_SCREENSHOT;
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    RSSurfaceCaptureTaskParallel task(id, captureConfig);
    auto node = std::make_shared<RSRenderNode>(id);
    auto drawable = std::static_pointer_cast<DrawableV2::RSRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(node));
    ASSERT_NE(drawable, nullptr);
    RSRenderThreadParamsManager::Instance().renderThreadParams_ = nullptr;
    task.CaptureDisplayNode(*drawable, canvas, captureParam, type);
    EXPECT_EQ(RSUniRenderThread::GetCaptureParam().needCaptureSpecialLayer_, true);
}

/*
 * @tc.name: Capture001
 * @tc.desc: Test RSSurfaceCaptureTaskParallel::Capture
 * @tc.type: FUNC
 * @tc.require: issueIAIT5Z
 */
HWTEST_F(RSSurfaceCaptureTaskParallelTest, Capture001, TestSize.Level0)
{
    auto renderEngine = std::make_shared<RSRenderEngine>();
    renderEngine->Init();
    RSUniRenderThread::Instance().uniRenderEngine_ = renderEngine;
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);

    NodeId nodeId = 111;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    ASSERT_NE(node, nullptr);
    NodeId id = 0;
    ScreenId screenId = 1;
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, screenId, context);
    ASSERT_NE(screenNode, nullptr);
    screenNode->CollectHdrStatus(HdrStatus::HDR_VIDEO);
    node->parent_ = screenNode;
    // set screen
    auto virtualScreenId = RSScreenManager::GetInstance()->CreateVirtualScreen("virtualDisplayTest", 480, 320, nullptr);
    node->screenId_ = virtualScreenId;
    RSRenderNodeMap& nodeMap = mainThread->GetContext().GetMutableNodeMap();
    nodeMap.RegisterRenderNode(node);

    sptr<RSISurfaceCaptureCallback> callback = new RSSurfaceCaptureCallbackStubMock();
    ASSERT_EQ(callback != nullptr, true);

    RSSurfaceCaptureParam captureParam;
    captureParam.id = nodeId;
    captureParam.config.isHdrCapture = true;
    captureParam.config.mainScreenRect = {0.f, 0.f, 480.f, 320.f};
    RSSurfaceCaptureTaskParallel::Capture(callback, captureParam);

    // Reset
    RSScreenManager::GetInstance()->RemoveVirtualScreen(virtualScreenId);
    nodeMap.UnregisterRenderNode(nodeId);
    RSUniRenderThread::Instance().uniRenderEngine_ = nullptr;
}

/*
 * @tc.name: Capture002
 * @tc.desc: Test RSSurfaceCaptureTaskParallel::Capture
 * @tc.type: FUNC
 * @tc.require: issueIAIT5Z
 */
HWTEST_F(RSSurfaceCaptureTaskParallelTest, Capture002, TestSize.Level0)
{
    auto renderEngine = std::make_shared<RSRenderEngine>();
    renderEngine->Init();
    RSUniRenderThread::Instance().uniRenderEngine_ = renderEngine;
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);

    NodeId nodeId = 111;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    ASSERT_NE(node, nullptr);
    // set screen
    auto virtualScreenId = RSScreenManager::GetInstance()->CreateVirtualScreen("virtualDisplayTest", 480, 320, nullptr);
    node->screenId_ = virtualScreenId;
    RSRenderNodeMap& nodeMap = mainThread->GetContext().GetMutableNodeMap();
    nodeMap.RegisterRenderNode(node);

    sptr<RSISurfaceCaptureCallback> callback = new RSSurfaceCaptureCallbackStubMock();
    ASSERT_EQ(callback != nullptr, true);

    RSSurfaceCaptureParam captureParam;
    captureParam.id = nodeId;
    captureParam.config.isHdrCapture = true;
    captureParam.config.mainScreenRect = {0.f, 0.f, 480.f, 320.f};
    RSSurfaceCaptureTaskParallel::Capture(callback, captureParam);

    // Reset
    RSScreenManager::GetInstance()->RemoveVirtualScreen(virtualScreenId);
    nodeMap.UnregisterRenderNode(nodeId);
    RSUniRenderThread::Instance().uniRenderEngine_ = nullptr;
}

/*
 * @tc.name: Capture003
 * @tc.desc: Test RSSurfaceCaptureTaskParallel::Capture
 * @tc.type: FUNC
 * @tc.require: issueIAIT5Z
 */
HWTEST_F(RSSurfaceCaptureTaskParallelTest, Capture003, TestSize.Level0)
{
    auto renderEngine = std::make_shared<RSRenderEngine>();
    renderEngine->Init();
    RSUniRenderThread::Instance().uniRenderEngine_ = renderEngine;

    sptr<RSISurfaceCaptureCallback> callback = new RSSurfaceCaptureCallbackStubMock();
    ASSERT_EQ(callback != nullptr, true);

    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 1.0f;
    captureConfig.scaleY = 1.0f;
    captureConfig.useCurWindow = true;

    NodeId nodeId = 1000124;
    RSDisplayNodeConfig config;
    std::shared_ptr<RSLogicalDisplayRenderNode> node = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    ASSERT_NE(node, nullptr);
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);
    RSRenderNodeMap& nodeMap = mainThread->GetContext().GetMutableNodeMap();
    nodeMap.RegisterRenderNode(node);
    RSSurfaceCaptureTaskParallel task(nodeId, captureConfig);
    captureConfig.isHdrCapture = true;
    NodeId id = 0;
    ScreenId screenId = 1;
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, screenId, context);
    ASSERT_NE(screenNode, nullptr);
    node->parent_ = screenNode;
    screenNode->CollectHdrStatus(HdrStatus::HDR_VIDEO);

    RSSurfaceCaptureParam captureParam;
    captureParam.id = nodeId;
    captureParam.config = captureConfig;
    RSSurfaceCaptureTaskParallel::Capture(callback, captureParam);

    // Reset
    nodeMap.UnregisterRenderNode(nodeId);
    RSUniRenderThread::Instance().uniRenderEngine_ = nullptr;
}

/*
 * @tc.name: RunHDR001
 * @tc.desc: Test RSSurfaceCaptureTaskParallel.RunHDR002, TEST RunHDR
 * @tc.type: FUNC
 * @tc.require: issueIAIT5Z
 */
HWTEST_F(RSSurfaceCaptureTaskParallelTest, RunHDR001, TestSize.Level2)
{
    auto renderEngine = std::make_shared<RSRenderEngine>();
    renderEngine->Init();
    RSUniRenderThread::Instance().uniRenderEngine_ = renderEngine;
    auto mainThread = RSMainThread::Instance();
    ASSERT_NE(mainThread, nullptr);

    NodeId nodeId = 111;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSLogicalDisplayRenderNode>(nodeId, config);
    ASSERT_NE(node, nullptr);
    NodeId id = 0;
    ScreenId screenId = 1;
    std::shared_ptr<RSContext> context = std::make_shared<RSContext>();
    auto screenNode = std::make_shared<RSScreenRenderNode>(id, screenId, context);
    ASSERT_NE(screenNode, nullptr);
    screenNode->CollectHdrStatus(HdrStatus::HDR_VIDEO);
    node->parent_ = screenNode;
    // set screen
    auto virtualScreenId = RSScreenManager::GetInstance()->CreateVirtualScreen("virtualDisplayTest", 480, 320, nullptr);
    node->screenId_ = virtualScreenId;
    RSRenderNodeMap& nodeMap = mainThread->GetContext().GetMutableNodeMap();
    nodeMap.RegisterRenderNode(node);

    RSSurfaceCaptureConfig captureConfig;
    captureConfig.isHdrCapture = true;
    captureConfig.mainScreenRect = {0.f, 0.f, 480.f, 320.f};
    RSSurfaceCaptureTaskParallel task(nodeId, captureConfig);
    bool ret = task.CreateResources();
    EXPECT_EQ(ret, false);

    sptr<RSISurfaceCaptureCallback> callback = new RSSurfaceCaptureCallbackStubMock();
    ASSERT_EQ(callback != nullptr, true);
    RSSurfaceCaptureParam captureParam;
    captureParam.id = nodeId;
    captureParam.config.isHdrCapture = true;
    captureParam.config.mainScreenRect = {0.f, 0.f, 480.f, 320.f};
    ret = task.RunHDR(callback, captureParam);
    EXPECT_EQ(ret, false);

    task.finalRotationAngle_ = RS_ROTATION_90;
    ret = task.RunHDR(callback, captureParam);
    EXPECT_EQ(ret, false);

    // Reset
    RSScreenManager::GetInstance()->RemoveVirtualScreen(virtualScreenId);
    nodeMap.UnregisterRenderNode(nodeId);
    RSUniRenderThread::Instance().uniRenderEngine_ = nullptr;
}

/*
 * @tc.name: RunHDR002
 * @tc.desc: Test RSSurfaceCaptureTaskParallel.RunHDR002, TEST RunHDR
 * @tc.type: FUNC
 * @tc.require: issueIAIT5Z
 */
HWTEST_F(RSSurfaceCaptureTaskParallelTest, RunHDR002, TestSize.Level2)
{
    NodeId nodeId = 111;
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.isHdrCapture = true;
    RSSurfaceCaptureTaskParallel task(nodeId, captureConfig);

    RSSurfaceCaptureParam captureParam;
    captureParam.config.isHdrCapture = true;
    task.useScreenShotWithHDR_ = false;
    EXPECT_EQ(task.RunHDR(nullptr, captureParam), false);
    task.useScreenShotWithHDR_ = true;
    EXPECT_EQ(task.RunHDR(nullptr, captureParam), false);
    sptr<RSISurfaceCaptureCallback> callback = new RSSurfaceCaptureCallbackStubMock();
    ASSERT_EQ(callback != nullptr, true);
    EXPECT_EQ(task.RunHDR(callback, captureParam), false);
}

/*
 * @tc.name: RunHDR003
 * @tc.desc: Test RSSurfaceCaptureTaskParallel.RunHDR003, TEST RunHDR
 * @tc.type: FUNC
 * @tc.require: issueIAIT5Z
 */
HWTEST_F(RSSurfaceCaptureTaskParallelTest, RunHDR003, TestSize.Level2)
{
    NodeId nodeId = 111;
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.mainScreenRect = {0.f, 0.f, 480.f, 320.f};
    captureConfig.isHdrCapture = true;
    RSSurfaceCaptureTaskParallel task(nodeId, captureConfig);
    auto node = std::make_shared<RSRenderNode>(nodeId);

    RSSurfaceCaptureParam captureParam;
    sptr<RSISurfaceCaptureCallback> callback = new RSSurfaceCaptureCallbackStubMock();
    ASSERT_NE(callback, nullptr);
    task.useScreenShotWithHDR_ = true;

    Media::InitializationOptions opts;
    opts.size.width = 480;
    opts.size.height = 320;
    task.pixelMap_ = Media::PixelMap::Create(opts);
    task.pixelMapHDR_ = Media::PixelMap::Create(opts);
    Media::ImageInfo imageInfo;
    // 200, 300 means size
    imageInfo.size.width = 200;
    imageInfo.size.height = 300;
    imageInfo.pixelFormat = Media::PixelFormat::ASTC_4x4;
    imageInfo.colorSpace = Media::ColorSpace::SRGB;
    task.pixelMap_->SetImageInfo(imageInfo);
    task.pixelMapHDR_->SetImageInfo(imageInfo);

    // 200 means rowDataSize
    int32_t rowDataSize = 200;
    // 300 means height
    uint32_t bufferSize = rowDataSize * 300;
    void *buffer = malloc(bufferSize);
    char *ch = static_cast<char *>(buffer);
    for (unsigned int i = 0; i < bufferSize; i++) {
        *(ch++) = (char)i;
    }

    task.pixelMap_->data_ = static_cast<uint8_t *>(buffer);
    EXPECT_TRUE(task.pixelMap_->data_ != nullptr);

    task.displayNodeDrawable_ = std::static_pointer_cast<DrawableV2::RSRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(node));
    EXPECT_EQ(task.RunHDR(callback, captureParam), false);

    void *bufferHDR = malloc(bufferSize);
    char *ch1 = static_cast<char *>(bufferHDR);
    for (unsigned int i = 0; i < bufferSize; i++) {
        *(ch1++) = (char)i;
    }
    task.pixelMapHDR_->data_ = static_cast<uint8_t *>(bufferHDR);
    EXPECT_TRUE(task.pixelMapHDR_->data_ != nullptr);
    EXPECT_EQ(task.RunHDR(callback, captureParam), false);
}

#ifdef RS_ENABLE_UNI_RENDER
/*
 * @tc.name: CreateSurfaceSyncCopyTask001
 * @tc.desc: Test RSSurfaceCaptureTaskParallel.CreateSurfaceSyncCopyTask
 * @tc.type: FUNC
 * @tc.require: issueIAIT5Z
 */
HWTEST_F(RSSurfaceCaptureTaskParallelTest, CreateSurfaceSyncCopyTask001, TestSize.Level2)
{
    NodeId nodeId = 111;
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.isHdrCapture = true;
    RSSurfaceCaptureTaskParallel task(nodeId, captureConfig);

    Media::InitializationOptions opts;
    opts.size.width = 480;
    opts.size.height = 320;
    std::unique_ptr<Media::PixelMap> pixelmap = Media::PixelMap::Create(opts);
    ASSERT_NE(pixelmap, nullptr);
    auto address = const_cast<uint32_t*>(pixelmap->GetPixel32(0, 0));
    ASSERT_NE(address, nullptr);
    Drawing::ImageInfo info { pixelmap->GetWidth(), pixelmap->GetHeight(), Drawing::ColorType::COLORTYPE_RGBA_8888,
        Drawing::AlphaType::ALPHATYPE_PREMUL };
    auto surface = Drawing::Surface::MakeRasterDirect(info, address, pixelmap->GetRowBytes());
    ASSERT_NE(surface, nullptr);
    std::shared_ptr<Drawing::SurfaceImpl> impl = std::make_shared<SurfaceImplMock>();
    surface->impl_ = impl;
    Drawing::BackendTexture backendTexture = surface->GetBackendTexture();
    EXPECT_TRUE(backendTexture.IsValid());
    sptr<RSISurfaceCaptureCallback> callback = new RSSurfaceCaptureCallbackStubMock();
    auto copyTask = task.CreateSurfaceSyncCopyTask(surface, std::move(pixelmap), nodeId, captureConfig, callback,
        0, false);
    EXPECT_NE(copyTask, nullptr);
    copyTask();
    copyTask = task.CreateSurfaceSyncCopyTask(surface, std::move(pixelmap), nodeId, captureConfig, callback, 1, true);
    EXPECT_NE(copyTask, nullptr);
    copyTask();
}

/*
 * @tc.name: CreateSurfaceSyncCopyTaskWithDoublePixelMap001
 * @tc.desc: Test RSSurfaceCaptureTaskParallel.CreateSurfaceSyncCopyTaskWithDoublePixelMap
 * @tc.type: FUNC
 * @tc.require: issueIAIT5Z
 */
HWTEST_F(RSSurfaceCaptureTaskParallelTest, CreateSurfaceSyncCopyTaskWithDoublePixelMap001, TestSize.Level2)
{
    NodeId nodeId = 111;
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.isHdrCapture = true;
    RSSurfaceCaptureTaskParallel task(nodeId, captureConfig);

    task.CreateSurfaceSyncCopyTaskWithDoublePixelMap(nullptr, nullptr, nullptr, nullptr, nodeId, captureConfig,
        nullptr, 0);

    Media::InitializationOptions opts;
    opts.size.width = 480;
    opts.size.height = 320;
    std::unique_ptr<Media::PixelMap> pixelmap = Media::PixelMap::Create(opts);
    ASSERT_NE(pixelmap, nullptr);
    auto address = const_cast<uint32_t*>(pixelmap->GetPixel32(0, 0));
    ASSERT_NE(address, nullptr);
    Drawing::ImageInfo info { pixelmap->GetWidth(), pixelmap->GetHeight(), Drawing::ColorType::COLORTYPE_RGBA_8888,
        Drawing::AlphaType::ALPHATYPE_PREMUL };
    auto surface = Drawing::Surface::MakeRasterDirect(info, address, pixelmap->GetRowBytes());
    auto copyTask = task.CreateSurfaceSyncCopyTaskWithDoublePixelMap(surface, std::move(pixelmap), nullptr, nullptr,
        nodeId, captureConfig, nullptr, 0);
    EXPECT_EQ(copyTask, nullptr);
}

/*
 * @tc.name: CreateSurfaceSyncCopyTaskWithDoublePixelMap002
 * @tc.desc: Test RSSurfaceCaptureTaskParallel.CreateSurfaceSyncCopyTaskWithDoublePixelMap
 * @tc.type: FUNC
 * @tc.require: issueIAIT5Z
 */
HWTEST_F(RSSurfaceCaptureTaskParallelTest, CreateSurfaceSyncCopyTaskWithDoublePixelMap002, TestSize.Level2)
{
    NodeId nodeId = 111;
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.isHdrCapture = true;
    RSSurfaceCaptureTaskParallel task(nodeId, captureConfig);

    Media::InitializationOptions opts;
    opts.size.width = 480;
    opts.size.height = 320;
    std::unique_ptr<Media::PixelMap> pixelmap = Media::PixelMap::Create(opts);
    ASSERT_NE(pixelmap, nullptr);
    auto address = const_cast<uint32_t*>(pixelmap->GetPixel32(0, 0));
    ASSERT_NE(address, nullptr);
    Drawing::ImageInfo info { pixelmap->GetWidth(), pixelmap->GetHeight(), Drawing::ColorType::COLORTYPE_RGBA_8888,
        Drawing::AlphaType::ALPHATYPE_PREMUL };
    auto surface = Drawing::Surface::MakeRasterDirect(info, address, pixelmap->GetRowBytes());
    std::shared_ptr<Drawing::SurfaceImpl> impl = std::make_shared<SurfaceImplMock>();
    surface->impl_ = impl;

    Media::InitializationOptions optsHDR;
    optsHDR.size.width = 480;
    optsHDR.size.height = 320;
    std::unique_ptr<Media::PixelMap> pixelmapHDR = Media::PixelMap::Create(optsHDR);
    ASSERT_NE(pixelmapHDR, nullptr);
    auto addressHDR = const_cast<uint32_t*>(pixelmapHDR->GetPixel32(0, 0));
    ASSERT_NE(addressHDR, nullptr);
    Drawing::ImageInfo infoHDR { pixelmapHDR->GetWidth(), pixelmapHDR->GetHeight(),
        Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL };
    auto surfaceHDR = Drawing::Surface::MakeRasterDirect(infoHDR, addressHDR, pixelmapHDR->GetRowBytes());
    std::shared_ptr<Drawing::SurfaceImpl> implHDR = std::make_shared<SurfaceImplMock>();
    surfaceHDR->impl_ = implHDR;

    sptr<RSISurfaceCaptureCallback> callback = new RSSurfaceCaptureCallbackStubMock();

    auto copyTask = task.CreateSurfaceSyncCopyTaskWithDoublePixelMap(surface, nullptr, surfaceHDR,
        nullptr, nodeId, captureConfig, callback, 0);
    EXPECT_NE(copyTask, nullptr);
    copyTask();
    copyTask = task.CreateSurfaceSyncCopyTaskWithDoublePixelMap(surface, std::move(pixelmap), surfaceHDR,
        nullptr, nodeId, captureConfig, callback, 0);
    EXPECT_NE(copyTask, nullptr);
    copyTask();
    delete callback;
}

/*
 * @tc.name: CreateSurfaceSyncCopyTaskWithDoublePixelMap003
 * @tc.desc: Test RSSurfaceCaptureTaskParallel.CreateSurfaceSyncCopyTaskWithDoublePixelMap
 * @tc.type: FUNC
 * @tc.require: issueIAIT5Z
 */
HWTEST_F(RSSurfaceCaptureTaskParallelTest, CreateSurfaceSyncCopyTaskWithDoublePixelMap003, TestSize.Level2)
{
    NodeId nodeId = 111;
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.isHdrCapture = true;
    RSSurfaceCaptureTaskParallel task(nodeId, captureConfig);

    task.CreateSurfaceSyncCopyTaskWithDoublePixelMap(nullptr, nullptr, nullptr, nullptr, nodeId, captureConfig,
        nullptr, 0);

    Media::InitializationOptions opts;
    opts.size.width = 480;
    opts.size.height = 320;
    std::unique_ptr<Media::PixelMap> pixelmap = Media::PixelMap::Create(opts);
    ASSERT_NE(pixelmap, nullptr);
    auto address = const_cast<uint32_t*>(pixelmap->GetPixel32(0, 0));
    ASSERT_NE(address, nullptr);
    Drawing::ImageInfo info { pixelmap->GetWidth(), pixelmap->GetHeight(), Drawing::ColorType::COLORTYPE_RGBA_8888,
        Drawing::AlphaType::ALPHATYPE_PREMUL };
    auto surface = Drawing::Surface::MakeRasterDirect(info, address, pixelmap->GetRowBytes());
    std::shared_ptr<Drawing::SurfaceImpl> impl = std::make_shared<SurfaceImplMock>();
    surface->impl_ = impl;

    Media::InitializationOptions optsHDR;
    optsHDR.size.width = 480;
    optsHDR.size.height = 320;
    std::unique_ptr<Media::PixelMap> pixelmapHDR = Media::PixelMap::Create(optsHDR);
    ASSERT_NE(pixelmapHDR, nullptr);
    auto addressHDR = const_cast<uint32_t*>(pixelmapHDR->GetPixel32(0, 0));
    ASSERT_NE(addressHDR, nullptr);
    Drawing::ImageInfo infoHDR { pixelmapHDR->GetWidth(), pixelmapHDR->GetHeight(),
        Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL };
    auto surfaceHDR = Drawing::Surface::MakeRasterDirect(infoHDR, addressHDR, pixelmapHDR->GetRowBytes());
    std::shared_ptr<Drawing::SurfaceImpl> implHDR = std::make_shared<SurfaceImplMock>();
    surfaceHDR->impl_ = implHDR;

    sptr<RSISurfaceCaptureCallback> callback = new RSSurfaceCaptureCallbackStubMock();

    auto copyTask = task.CreateSurfaceSyncCopyTaskWithDoublePixelMap(surface, std::move(pixelmap), surfaceHDR,
        std::move(pixelmapHDR), nodeId, captureConfig, callback, 0);
    EXPECT_NE(copyTask, nullptr);
    copyTask();
    delete callback;
}

/*
 * @tc.name: CreateSurfaceSyncCopyTaskWithDoublePixelMap004
 * @tc.desc: Test RSSurfaceCaptureTaskParallel.CreateSurfaceSyncCopyTaskWithDoublePixelMap
 * @tc.type: FUNC
 * @tc.require: issueIAIT5Z
 */
HWTEST_F(RSSurfaceCaptureTaskParallelTest, CreateSurfaceSyncCopyTaskWithDoublePixelMap004, TestSize.Level2)
{
    NodeId nodeId = 111;
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.isHdrCapture = true;
    RSSurfaceCaptureTaskParallel task(nodeId, captureConfig);

    Media::InitializationOptions opts;
    opts.size.width = 480;
    opts.size.height = 320;
    std::unique_ptr<Media::PixelMap> pixelmap = Media::PixelMap::Create(opts);
    ASSERT_NE(pixelmap, nullptr);
    auto address = const_cast<uint32_t*>(pixelmap->GetPixel32(0, 0));
    ASSERT_NE(address, nullptr);
    Drawing::ImageInfo info { pixelmap->GetWidth(), pixelmap->GetHeight(), Drawing::ColorType::COLORTYPE_RGBA_8888,
        Drawing::AlphaType::ALPHATYPE_PREMUL };
    auto surface = Drawing::Surface::MakeRasterDirect(info, address, pixelmap->GetRowBytes());
    std::shared_ptr<SurfaceImplMock> impl = std::make_shared<SurfaceImplMock>();

    surface->impl_ = impl;
    Media::InitializationOptions optsHDR;
    optsHDR.size.width = 480;
    optsHDR.size.height = 320;
    std::unique_ptr<Media::PixelMap> pixelmapHDR = Media::PixelMap::Create(optsHDR);
    ASSERT_NE(pixelmapHDR, nullptr);
    auto addressHDR = const_cast<uint32_t*>(pixelmapHDR->GetPixel32(0, 0));
    ASSERT_NE(addressHDR, nullptr);
    Drawing::ImageInfo infoHDR { pixelmapHDR->GetWidth(), pixelmapHDR->GetHeight(),
        Drawing::ColorType::COLORTYPE_RGBA_8888, Drawing::AlphaType::ALPHATYPE_PREMUL };
    auto surfaceHDR = Drawing::Surface::MakeRasterDirect(infoHDR, addressHDR, pixelmapHDR->GetRowBytes());
    std::shared_ptr<SurfaceImplMock> implHDR = std::make_shared<SurfaceImplMock>();
    surfaceHDR->impl_ = implHDR;

    sptr<RSISurfaceCaptureCallback> callback = new RSSurfaceCaptureCallbackStubMock();

    impl->returnValidTexture_ = false;
    implHDR->returnValidTexture_ = false;
    auto copyTask = task.CreateSurfaceSyncCopyTaskWithDoublePixelMap(surface, nullptr, surfaceHDR, nullptr, nodeId,
        captureConfig, callback, 0);

    impl->returnValidTexture_ = true;
    implHDR->returnValidTexture_ = false;
    copyTask = task.CreateSurfaceSyncCopyTaskWithDoublePixelMap(surface, nullptr, surfaceHDR, nullptr, nodeId,
        captureConfig, callback, 0);

    impl->returnValidTexture_ = true;
    implHDR->returnValidTexture_ = true;
    copyTask = task.CreateSurfaceSyncCopyTaskWithDoublePixelMap(surface, nullptr, surfaceHDR, nullptr, nodeId,
        captureConfig, callback, 0);

    EXPECT_NE(copyTask, nullptr);
    copyTask();
    delete callback;
}

/*
 * @tc.name: AddBlurTest
 * @tc.desc: Test AddBlurFunc
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceCaptureTaskParallelTest, AddBlurTest, TestSize.Level2)
{
    NodeId id = 33;
    RSSurfaceCaptureConfig captureConfig;
    RSSurfaceCaptureTaskParallel task(id, captureConfig);
    std::shared_ptr<RSSurfaceRenderNode> node = std::make_shared<RSSurfaceRenderNode>(id);
    ASSERT_NE(node, nullptr);
    task.surfaceNode_ = node;
    auto pixmap = task.CreatePixelMapBySurfaceNode(node, false);
    EXPECT_EQ(pixmap, nullptr);
    node->renderProperties_.SetBoundsWidth(50.0f);
    node->renderProperties_.SetBoundsHeight(50.0f);
    pixmap = task.CreatePixelMapBySurfaceNode(node, false);
    EXPECT_NE(pixmap, nullptr);

    std::shared_ptr<Drawing::Surface> surfaceNull = std::make_shared<Drawing::Surface>();
    Drawing::Canvas drawingCanvas;
    RSPaintFilterCanvas canvas(&drawingCanvas);
    task.AddBlur(canvas, surfaceNull, 35.0f);

    Media::InitializationOptions opts;
    opts.size.width = 480;
    opts.size.height = 320;
    std::unique_ptr<Media::PixelMap> pixelmap = Media::PixelMap::Create(opts);
    ASSERT_NE(pixelmap, nullptr);
    auto address = const_cast<uint32_t*>(pixelmap->GetPixel32(0, 0));
    ASSERT_NE(address, nullptr);
    Drawing::ImageInfo info { pixelmap->GetWidth(), pixelmap->GetHeight(), Drawing::ColorType::COLORTYPE_RGBA_8888,
        Drawing::AlphaType::ALPHATYPE_PREMUL };
    auto surface = Drawing::Surface::MakeRasterDirect(info, address, pixelmap->GetRowBytes());
    ASSERT_NE(surface, nullptr);

    RSPaintFilterCanvas canvasFilter(surface.get());
    task.AddBlur(canvasFilter, surface, 35.0f);
    task.AddBlur(canvasFilter, surface, 275.0f);
    EXPECT_NE(pixmap, nullptr);
}

/*
 * @tc.name: PixelMapCopy001
 * @tc.desc: Test RSSurfaceCaptureTaskParallel.PixelMapCopy
 * @tc.type: FUNC
 * @tc.require: issueIAIT5Z
 */
HWTEST_F(RSSurfaceCaptureTaskParallelTest, PixelMapCopy001, TestSize.Level2)
{
    NodeId nodeId = 111;
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.isHdrCapture = true;
    RSSurfaceCaptureTaskParallel task(nodeId, captureConfig);

    Drawing::BackendTexture texture;
    Drawing::ColorType type = Drawing::ColorType::COLORTYPE_RGBA_8888;
    std::unique_ptr<Media::PixelMap> pixelmap = nullptr;
    auto ret = task.PixelMapCopy(pixelmap, nullptr, texture, type, false, 0, false);
    ASSERT_EQ(ret, false);
    ret = task.PixelMapCopy(pixelmap, nullptr, texture, type, false, 0, true);

    Media::InitializationOptions opts;
    opts.size.width = 480;
    opts.size.height = 320;
    std::unique_ptr<Media::PixelMap> pixelmap2 = Media::PixelMap::Create(opts);
    ASSERT_NE(pixelmap2, nullptr);
    RS_LOGE("RSSurfaceCaptureTaskParallel PixelMapCopy in");
    ret = task.PixelMapCopy(pixelmap2, nullptr, texture, type, false, 0, false);
    EXPECT_EQ(ret, false);
}

/*
 * @tc.name: DrawHDRSurfaceContent001
 * @tc.desc: Test RSSurfaceCaptureTaskParallel.DrawHDRSurfaceContent
 * @tc.type: FUNC
 * @tc.require: issueIAIT5Z
 */
HWTEST_F(RSSurfaceCaptureTaskParallelTest, DrawHDRSurfaceContent001, TestSize.Level2)
{
    NodeId nodeId = 111;
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.isHdrCapture = true;
    auto task = std::make_shared<RSSurfaceCaptureTaskParallel>(nodeId, captureConfig);
    auto node = std::make_shared<RSRenderNode>(nodeId);


    Media::InitializationOptions opts;
    opts.size.width = 480;
    opts.size.height = 320;
    std::unique_ptr<Media::PixelMap> pixelmap = Media::PixelMap::Create(opts);
    ASSERT_NE(pixelmap, nullptr);
    auto address = const_cast<uint32_t*>(pixelmap->GetPixel32(0, 0));
    ASSERT_NE(address, nullptr);
    Drawing::ImageInfo info { pixelmap->GetWidth(), pixelmap->GetHeight(), Drawing::ColorType::COLORTYPE_RGBA_8888,
        Drawing::AlphaType::ALPHATYPE_PREMUL };
    auto surface = Drawing::Surface::MakeRasterDirect(info, address, pixelmap->GetRowBytes());
    task->surfaceNodeDrawable_ = nullptr;
    task->displayNodeDrawable_ = nullptr;

    captureConfig.mainScreenRect = {0.f, 0.f, -1.f, 1.f};

    captureConfig.mainScreenRect = {0.f, 0.f, 1.f, -1.f};


    captureConfig.mainScreenRect = {0.f, 0.f, 480.f, 320.f};
    task->displayNodeDrawable_ = std::static_pointer_cast<DrawableV2::RSRenderNodeDrawable>(
        DrawableV2::RSRenderNodeDrawableAdapter::OnGenerate(node));
}
/*
 * @tc.name: PixelMapCopy002
 * @tc.desc: Test RSSurfaceCaptureTaskParallel.PixelMapCopy
 * @tc.type: FUNC
 * @tc.require: issueIAIT5Z
 */
HWTEST_F(RSSurfaceCaptureTaskParallelTest, PixelMapCopy002, TestSize.Level2)
{
    NodeId nodeId = 1000;
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.isHdrCapture = true;
    RSSurfaceCaptureTaskParallel task(nodeId, captureConfig);

    Media::InitializationOptions opts;
    opts.size.width = 480;
    opts.size.height = 320;
    std::unique_ptr<Media::PixelMap> pixelmap = Media::PixelMap::Create(opts);
    ASSERT_NE(pixelmap, nullptr);

    auto colorSpace = std::make_shared<Drawing::ColorSpace>(Drawing::ColorSpace::ColorSpaceType::REF_IMAGE);
    ASSERT_FALSE(colorSpace->IsSRGB());

    RSBackgroundThread::Instance().gpuContext_ = std::make_shared<Drawing::GPUContext>();
    auto grContext = RSBackgroundThread::Instance().GetShareGPUContext();
    ASSERT_NE(grContext, nullptr);

    Drawing::BackendTexture texture;
    bool useDmaType = RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
                      RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR;
    ASSERT_TRUE(useDmaType);

    auto ret = task.PixelMapCopy(pixelmap, colorSpace, texture, Drawing::ColorType::COLORTYPE_RGBA_8888,
        true, 90, false);
    ASSERT_FALSE(ret);
    ret = task.PixelMapCopy(pixelmap, colorSpace, texture, Drawing::ColorType::COLORTYPE_RGBA_8888, true, 90, true);

    ret = task.PixelMapCopy(pixelmap, colorSpace, texture, Drawing::ColorType::COLORTYPE_RGBA_8888, false, 90, false);
    ASSERT_FALSE(ret);
}
#endif
}
}
