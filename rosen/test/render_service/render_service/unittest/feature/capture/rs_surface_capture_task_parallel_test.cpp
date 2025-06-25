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
#include "pipeline/rs_display_render_node.h"
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
        Media::PixelMap* pixelmap) override {};
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

    // TEST2:: InVaild scalex01
    {
        Drawing::Rect rect = {0, 0, 1260, 2720};
        RSSurfaceCaptureConfig captureConfig;
        captureConfig.scaleX = 2;
        auto pixelMap = RSCapturePixelMapManager::GetClientCapturePixelMap(rect, captureConfig,
            UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL);
        EXPECT_EQ(pixelMap == nullptr, true);
    }

    // TESt: invauld scalex01

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

/*
 * @tc.name: TestCopyDataToPixelMap
 * @tc.desc: Test TestCopyDataToPixelMap
 * @tc.type: FUNC
 * @tc.require:
*/
HWTEST_F(RSSurfaceCaptureTaskParallelTest, TestCopyDataToPixelMap, TestSize.Level2)
{
    // Test PixelMap is nullptr
    std::shared_ptr<Drawing::Image> img;
    std::unique_ptr<Media::PixelMap> pixelMap;
    RSSurfaceCaptureConfig captureConfig;
    Drawing::Rect rect = {0, 0, 1260, 2720};
    bool ret = CopyDataToPixelMap(img, pixelMap, captureConfig, UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL);
    EXPECT_EQ(ret, false);
    // vaild pixelMap, invaild img
    auto pixelMap1 = RSCapturePixelMapManager::CreatePixelMap(rect, captureConfig);
    EXPECT_EQ(pixelMap1 != nullptr, true);
    ret = CopyDataToPixelMap(img, pixelMap1, captureConfig, UniRenderEnabledType::UNI_RENDER_ENABLED_FOR_ALL);
    EXPECT_EQ(ret, false);
}

}
}
