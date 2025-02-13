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

#include "rs_test_util.h"
#include "surface_buffer_impl.h"
#include "pipeline/rs_surface_capture_task.h"
#include "pipeline/rs_ui_capture_task_parallel.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "transaction/rs_interfaces.h"
#include "ui/rs_surface_extractor.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_proxy_node.h"
#include "pipeline/rs_main_thread.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "pipeline/rs_uni_render_engine.h"
#include "platform/common/rs_system_properties.h"

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

    void OnSurfaceCapture(NodeId id, Media::PixelMap* pixelmap)
    {
        // DO NOTHING
    }
};
}

class RSUiCaptureTaskParallelTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        InitRenderContext();
        rsInterfaces_ = &RSInterfaces::GetInstance();

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

    static RSInterfaces* rsInterfaces_;
    static RenderContext* renderContext_;
    static RSDisplayNodeConfig mirrorConfig_;
    static std::shared_ptr<RSDisplayNode> displayNode_;

    std::shared_ptr<RSSurfaceNode> surfaceNode_;
    std::shared_ptr<RSCanvasNode> canvasNode_;
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
    ASSERT_EQ(callback->captureSuccess_, false);
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
    ASSERT_EQ(callback->captureSuccess_, false);
}

/*
 * @tc.name: TakeSurfaceCaptureForUiCanvasNode
 * @tc.desc: Test TakeSurfaceCaptureForUI with canvas node
 * @tc.type: FUNC
 * @tc.require: issueIA6QID
*/
HWTEST_F(RSUiCaptureTaskParallelTest, TakeSurfaceCaptureForUiCanvasNode, Function | SmallTest | Level2)
{
    SetUpSurface();

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    bool ret = rsInterfaces_->TakeSurfaceCaptureForUI(canvasNode_, callback);
    ASSERT_EQ(ret, true);
    ASSERT_EQ(CheckSurfaceCaptureCallback(callback), true);
    ASSERT_EQ(callback->captureSuccess_, false);
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
    ASSERT_EQ(callback->captureSuccess_, false);
}

/*
 * @tc.name: TakeSurfaceCaptureForUiSync
 * @tc.desc: Test TakeSurfaceCaptureForUI sync
 * @tc.type: FUNC
 * @tc.require: issueIA6QID
*/
HWTEST_F(RSUiCaptureTaskParallelTest, TakeSurfaceCaptureForUiSync, Function | SmallTest | Level2)
{
    SetUpSurface();

    auto callback = std::make_shared<CustomizedSurfaceCapture>();
    bool ret = rsInterfaces_->TakeSurfaceCaptureForUI(canvasNode_, callback, 1.0, 1.0, true);
    ASSERT_EQ(ret, true);
    ASSERT_EQ(CheckSurfaceCaptureCallback(callback), true);
    ASSERT_EQ(callback->captureSuccess_, false);
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
    ASSERT_EQ(callback->captureSuccess_, false);
}

/*
 * @tc.name: RSUiCaptureTaskParallel_CreateResources
 * @tc.desc: Test RSUiCaptureTaskParallel::CreateResources
 * @tc.type: FUNC
 * @tc.require: issueIA6QID
*/
HWTEST_F(RSUiCaptureTaskParallelTest, RSUiCaptureTaskParallel_CreateResources, Function | SmallTest | Level2)
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
    surfaceRenderNode->renderContent_->renderProperties_.SetBoundsWidth(HALF_BOUNDS_WIDTH);
    surfaceRenderNode->renderContent_->renderProperties_.SetBoundsHeight(HALF_BOUNDS_HEIGHT);
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
    auto pixelMap = std::make_unique<Media::PixelMap>();
    ASSERT_NE(pixelMap, nullptr);
    auto surface = std::make_shared<Drawing::Surface>();
    ASSERT_NE(surface, nullptr);
#ifdef RS_ENABLE_UNI_RENDER
    auto copytask =
        RSUiCaptureTaskParallel::CreateSurfaceSyncCopyTask(surface, std::move(pixelMap), node->GetId(), mockCallback);

    ASSERT_FALSE(copytask);
    mainThread->context_->nodeMap.UnregisterRenderNode(node->GetId());
#endif
}
} // namespace Rosen
} // namespace OHOS
