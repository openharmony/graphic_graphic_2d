/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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
#include "drawable/rs_display_render_node_drawable.h"
#include "feature/capture/rs_surface_capture_task.h"
#include "pipeline/render_thread/rs_uni_render_engine.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "transaction/rs_interfaces.h"
#include "ui/rs_surface_extractor.h"
#include "pipeline/rs_test_util.h"
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "platform/common/rs_system_properties.h"
#include "feature/capture/rs_capture_pixelmap_manager.h"
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS {
namespace Rosen {
using namespace HiviewDFX;
using DisplayId = ScreenId;
namespace {
    constexpr HiLogLabel LOG_LABEL = { LOG_CORE, 0xD001400, "RSSurfaceCaptureTaskTest" };
    constexpr uint32_t MAX_TIME_WAITING_FOR_CALLBACK = 200; // 10ms
    constexpr uint32_t SLEEP_TIME_IN_US = 10000; // 10ms
    constexpr uint32_t SLEEP_TIME_FOR_PROXY = 100000; // 100ms
    constexpr float DEFAULT_BOUNDS_WIDTH = 100.f;
    constexpr float DEFAULT_BOUNDS_HEIGHT = 200.f;
    constexpr uint32_t DEFAULT_CANVAS_WIDTH = 800;
    constexpr uint32_t DEFAULT_CANVAS_HEIGHT = 600;
    constexpr float DEFAULT_CANVAS_SCALE = 1.0f;
    constexpr NodeId DEFAULT_NODEID = 0;
    std::shared_ptr<Drawing::Canvas> drawingCanvas_;
    std::shared_ptr<RSSurfaceCaptureVisitor> visitor_;
}

class CustomizedSurfaceCapture : public SurfaceCaptureCallback {
public:
    CustomizedSurfaceCapture() : showNode_(nullptr) {}

    explicit CustomizedSurfaceCapture(std::shared_ptr<RSSurfaceNode> surfaceNode) : showNode_(surfaceNode) {}

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
        showNode_ = nullptr;
    }

private:
    bool testSuccess = false;
    bool isCallbackCalled_ = false;
    std::shared_ptr<RSSurfaceNode> showNode_;
}; // class CustomizedSurfaceCapture

class RSSurfaceCaptureTaskTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    void SetUp() override;
    void TearDown() override;

    static std::shared_ptr<RSSurfaceNode> CreateSurface(std::string surfaceNodeName = "DefaultSurfaceNode");
    static void InitRenderContext();
    static void FillSurface(std::shared_ptr<RSSurfaceNode> surfaceNode,
        const Drawing::Color color = Drawing::Color::COLOR_WHITE);
    bool CheckSurfaceCaptureCallback();

    static RSInterfaces* rsInterfaces_;
    static RenderContext* renderContext_;
    static RSDisplayNodeConfig defaultConfig_;
    static RSDisplayNodeConfig mirrorConfig_;
    static std::shared_ptr<RSSurfaceNode> surfaceNode_;
    static std::shared_ptr<CustomizedSurfaceCapture> surfaceCaptureCb_;
};
RSInterfaces* RSSurfaceCaptureTaskTest::rsInterfaces_ = nullptr;
RenderContext* RSSurfaceCaptureTaskTest::renderContext_ = nullptr;
RSDisplayNodeConfig RSSurfaceCaptureTaskTest::defaultConfig_ = {INVALID_SCREEN_ID, false, INVALID_SCREEN_ID};
RSDisplayNodeConfig RSSurfaceCaptureTaskTest::mirrorConfig_ = {INVALID_SCREEN_ID, true, INVALID_SCREEN_ID};
std::shared_ptr<RSSurfaceNode> RSSurfaceCaptureTaskTest::surfaceNode_ = nullptr;
std::shared_ptr<CustomizedSurfaceCapture> RSSurfaceCaptureTaskTest::surfaceCaptureCb_ = nullptr;

void RSSurfaceCaptureTaskTest::SetUp()
{
    if (RSUniRenderJudgement::IsUniRender()) {
        auto& uniRenderThread = RSUniRenderThread::Instance();
        uniRenderThread.uniRenderEngine_ = std::make_shared<RSUniRenderEngine>();
    }
    surfaceCaptureCb_->Reset();
    bool isUnirender = RSUniRenderJudgement::IsUniRender();
    RSSurfaceCaptureConfig captureConfig;
    visitor_ = std::make_shared<RSSurfaceCaptureVisitor>(captureConfig, isUnirender);
    if (visitor_ == nullptr) {
        return;
    }
    drawingCanvas_ = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    visitor_->canvas_ = std::make_unique<RSPaintFilterCanvas>(drawingCanvas_.get());
}

void RSSurfaceCaptureTaskTest::TearDown()
{
    visitor_ = nullptr;
}

void RSSurfaceCaptureTaskTest::SetUpTestCase()
{
    RSTestUtil::InitRenderNodeGC();
    rsInterfaces_ = &(RSInterfaces::GetInstance());
    if (rsInterfaces_ == nullptr) {
        HiLog::Error(LOG_LABEL, "%s: rsInterfaces_ == nullptr", __func__);
        return;
    }
    ScreenId screenId = rsInterfaces_->GetDefaultScreenId();
    defaultConfig_.screenId = screenId;
    RSScreenModeInfo modeInfo = rsInterfaces_->GetScreenActiveMode(screenId);
    DisplayId virtualDisplayId = rsInterfaces_->CreateVirtualScreen("virtualDisplayTest",
        modeInfo.GetScreenWidth(), modeInfo.GetScreenHeight(), nullptr);
    mirrorConfig_.screenId = virtualDisplayId;
    mirrorConfig_.mirrorNodeId = screenId;

    surfaceNode_ = CreateSurface("SurfaceCaptureTestNode");
    surfaceNode_->SetBounds(0, 0, 1, 1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    if (surfaceNode_ == nullptr) {
        HiLog::Error(LOG_LABEL, "%s: surfaceNode_ == nullptr", __func__);
        return;
    }
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::OPENGL) {
        FillSurface(surfaceNode_);
    }
    surfaceCaptureCb_ = std::make_shared<CustomizedSurfaceCapture>(surfaceNode_);
    if (surfaceCaptureCb_ == nullptr) {
        HiLog::Error(LOG_LABEL, "%s: surfaceCaptureCb_ == nullptr", __func__);
        return;
    }
}

void RSSurfaceCaptureTaskTest::TearDownTestCase()
{
    rsInterfaces_->RemoveVirtualScreen(mirrorConfig_.screenId);
    rsInterfaces_ = nullptr;
    renderContext_ = nullptr;
    surfaceNode_ = nullptr;
    surfaceCaptureCb_->Reset();
    surfaceCaptureCb_ = nullptr;
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
}

std::shared_ptr<RSSurfaceNode> RSSurfaceCaptureTaskTest::CreateSurface(std::string surfaceNodeName)
{
    RSSurfaceNodeConfig config;
    config.SurfaceNodeName = surfaceNodeName;
    return RSSurfaceNode::Create(config);
}

void RSSurfaceCaptureTaskTest::InitRenderContext()
{
#ifdef ACE_ENABLE_GL
    if (RSSystemProperties::IsUseVulkan()) {
        GTEST_LOG_(INFO) << "vulkan enable! skip opengl test case";
        return;
    }
    if (renderContext_ == nullptr) {
        HiLog::Info(LOG_LABEL, "%s: init renderContext_", __func__);
        renderContext_ = RenderContextFactory::GetInstance().CreateEngine();
        renderContext_->InitializeEglContext();
    }
#endif // ACE_ENABLE_GL
}

void RSSurfaceCaptureTaskTest::FillSurface(std::shared_ptr<RSSurfaceNode> surfaceNode, const Drawing::Color color)
{
    Vector4f bounds = { 0.0f, 0.0f, DEFAULT_BOUNDS_WIDTH, DEFAULT_BOUNDS_HEIGHT};
    surfaceNode->SetBounds(bounds); // x, y, w, h
    std::shared_ptr<RSSurface> rsSurface = RSSurfaceExtractor::ExtractRSSurface(surfaceNode);
    if (rsSurface == nullptr) {
        HiLog::Error(LOG_LABEL, "%s: rsSurface == nullptr", __func__);
        return;
    }
#ifdef ACE_ENABLE_GL
    if (!RSSystemProperties::IsUseVulkan()) {
        HiLog::Info(LOG_LABEL, "ACE_ENABLE_GL");
        InitRenderContext();
        rsSurface->SetRenderContext(renderContext_);
    }
#endif // ACE_ENABLE_GL
    auto frame = rsSurface->RequestFrame(DEFAULT_BOUNDS_WIDTH, DEFAULT_BOUNDS_HEIGHT);
    std::unique_ptr<RSSurfaceFrame> framePtr = std::move(frame);
    auto canvas = framePtr->GetCanvas();
    auto rect = Drawing::Rect(0.0f, 0.0f, DEFAULT_BOUNDS_WIDTH, DEFAULT_BOUNDS_HEIGHT);
    Drawing::Brush brush;
    brush.SetColor(color);
    canvas->AttachBrush(brush);
    canvas->DrawRect(rect);
    canvas->DetachBrush();
    framePtr->SetDamageRegion(0.0f, 0.0f, DEFAULT_BOUNDS_WIDTH, DEFAULT_BOUNDS_HEIGHT);
    auto framePtr1 = std::move(framePtr);
    rsSurface->FlushFrame(framePtr1);
    usleep(SLEEP_TIME_FOR_PROXY); // wait for finishing flush
}

bool RSSurfaceCaptureTaskTest::CheckSurfaceCaptureCallback()
{
    if (surfaceCaptureCb_ == nullptr) {
        HiLog::Error(LOG_LABEL, "%s: surfaceCaptureCb_ is nullptr", __func__);
        return false;
    }
    uint32_t times = 0;
    do {
        if (surfaceCaptureCb_->IsCallbackCalled()) {
            HiLog::Info(LOG_LABEL, "%s: get callback at times %d", __func__, times);
            return true;
        }
        usleep(SLEEP_TIME_IN_US);
        ++times;
    } while (times <= MAX_TIME_WAITING_FOR_CALLBACK);
    HiLog::Error(LOG_LABEL, "%s: fail to get callback in time", __func__);
    return false;
}

/*
 * @tc.name: Run001
 * @tc.desc: Test RSSurfaceCaptureTaskTest.Run
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RSSurfaceCaptureTaskTest, Run001, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 0.f;
    captureConfig.scaleY = 0.f;
    RSSurfaceCaptureTask task(id, captureConfig);
    ASSERT_EQ(false, task.Run(nullptr));
}

/*
 * @tc.name: Run002
 * @tc.desc: Test RSSurfaceCaptureTaskTest.Run
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RSSurfaceCaptureTaskTest, Run002, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleY = 0.f;
    RSSurfaceCaptureTask task(id, captureConfig);
    ASSERT_EQ(false, task.Run(nullptr));
}

/*
 * @tc.name: Run003
 * @tc.desc: Test RSSurfaceCaptureTaskTest.Run
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RSSurfaceCaptureTaskTest, Run003, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 0.f;
    RSSurfaceCaptureTask task(id, captureConfig);
    ASSERT_EQ(false, task.Run(nullptr));
}

/*
 * @tc.name: Run004
 * @tc.desc: Test RSSurfaceCaptureTaskTest.Run
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RSSurfaceCaptureTaskTest, Run004, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceCaptureConfig captureConfig;
    RSSurfaceCaptureTask task(id, captureConfig);
    ASSERT_EQ(false, task.Run(nullptr));
}

/*
 * @tc.name: Run005
 * @tc.desc: Test RSSurfaceCaptureTaskTest.Run
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RSSurfaceCaptureTaskTest, Run005, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = -1.0f;
    RSSurfaceCaptureTask task(id, captureConfig);
    ASSERT_EQ(false, task.Run(nullptr));
}

/*
 * @tc.name: Run007
 * @tc.desc: Test RSSurfaceCaptureTaskTest.Run
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RSSurfaceCaptureTaskTest, Run007, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleY = -1.0f;
    RSSurfaceCaptureTask task(id, captureConfig);
    ASSERT_EQ(false, task.Run(nullptr));
}

/*
 * @tc.name: CreatePixelMapByDisplayNode001
 * @tc.desc: Test RSSurfaceCaptureTaskTest.CreatePixelMapByDisplayNode
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RSSurfaceCaptureTaskTest, CreatePixelMapByDisplayNode001, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 0.f;
    captureConfig.scaleY = 0.f;
    RSSurfaceCaptureTask task(id, captureConfig);
    ASSERT_EQ(nullptr, task.CreatePixelMapByDisplayNode(nullptr));
}

/*
 * @tc.name: CreatePixelMapBySurfaceNode001
 * @tc.desc: Test RSSurfaceCaptureTaskTest.CreatePixelMapBySurfaceNode
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RSSurfaceCaptureTaskTest, CreatePixelMapBySurfaceNode001, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 0.f;
    captureConfig.scaleY = 0.f;
    RSSurfaceCaptureTask task(id, captureConfig);
    ASSERT_EQ(nullptr, task.CreatePixelMapBySurfaceNode(nullptr, false));
}

/*
 * @tc.name: CreatePixelMapBySurfaceNode002
 * @tc.desc: Test RSSurfaceCaptureTaskTest.CreatePixelMapBySurfaceNode
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RSSurfaceCaptureTaskTest, CreatePixelMapBySurfaceNode002, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 0.f;
    captureConfig.scaleY = 0.f;
    RSSurfaceCaptureTask task(id, captureConfig);
    ASSERT_EQ(nullptr, task.CreatePixelMapBySurfaceNode(nullptr, true));
}

/*
 * @tc.name: CreatePixelMapBySurfaceNode003
 * @tc.desc: Test RSSurfaceCaptureTaskTest.CreatePixelMapBySurfaceNode
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RSSurfaceCaptureTaskTest, CreatePixelMapBySurfaceNode003, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 0.f;
    captureConfig.scaleY = 0.f;
    RSSurfaceCaptureTask task(id, captureConfig);
    RSSurfaceRenderNodeConfig config;
    auto node = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_EQ(nullptr, task.CreatePixelMapBySurfaceNode(node, false));
}

/*
 * @tc.name: CreatePixelMapBySurfaceNode004
 * @tc.desc: Test RSSurfaceCaptureTaskTest.CreatePixelMapBySurfaceNode
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RSSurfaceCaptureTaskTest, CreatePixelMapBySurfaceNode004, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 0.f;
    captureConfig.scaleY = 0.f;
    RSSurfaceCaptureTask task(id, captureConfig);
    RSSurfaceRenderNodeConfig config;
    auto node = std::make_shared<RSSurfaceRenderNode>(config);
    ASSERT_EQ(nullptr, task.CreatePixelMapBySurfaceNode(node, true));
}

/*
 * @tc.name: CreateSurface001
 * @tc.desc: Test RSSurfaceCaptureTaskTest.CreateSurface001
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RSSurfaceCaptureTaskTest, CreateSurface001, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 0.f;
    captureConfig.scaleY = 0.f;
    RSSurfaceCaptureTask task(id, captureConfig);
    ASSERT_EQ(nullptr, task.CreateSurface(nullptr));
}

/*
 * @tc.name: CreateSurface002
 * @tc.desc: Test RSSurfaceCaptureTaskTest.CreateSurface002
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RSSurfaceCaptureTaskTest, CreateSurface002, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 0.f;
    captureConfig.scaleY = 0.f;
    RSSurfaceCaptureTask task(id, captureConfig);
    std::unique_ptr<Media::PixelMap> pixelmap = nullptr;
    ASSERT_EQ(nullptr, task.CreateSurface(pixelmap));
}

/*
 * @tc.name: SetSurface
 * @tc.desc: Test RSSurfaceCaptureTaskTest.SetSurface
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RSSurfaceCaptureTaskTest, SetSurface, Function | SmallTest | Level2)
{
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 0.f;
    captureConfig.scaleY = 0.f;
    std::shared_ptr<RSSurfaceCaptureVisitor> visitor =
        std::make_shared<RSSurfaceCaptureVisitor>(captureConfig, false);
    ASSERT_NE(nullptr, visitor);
    visitor->canvas_ = nullptr;
    visitor->SetSurface(nullptr);
    ASSERT_EQ(nullptr, visitor->canvas_);
}

/*
 * @tc.name: ProcessRootRenderNode001
 * @tc.desc: Test RSSurfaceCaptureTaskTest.ProcessRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RSSurfaceCaptureTaskTest, ProcessRootRenderNode001, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 0.f;
    captureConfig.scaleY = 0.f;
    std::shared_ptr<RSSurfaceCaptureVisitor> visitor =
        std::make_shared<RSSurfaceCaptureVisitor>(captureConfig, false);
    ASSERT_NE(nullptr, visitor);
    visitor->canvas_ = nullptr;
    visitor->SetSurface(nullptr);
    ASSERT_EQ(nullptr, visitor->canvas_);
    RSRootRenderNode node(id);
    visitor->ProcessRootRenderNode(node);
}

/*
 * @tc.name: ProcessCanvasRenderNode001
 * @tc.desc: Test RSSurfaceCaptureTaskTest.ProcessCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RSSurfaceCaptureTaskTest, ProcessCanvasRenderNode001, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 0.f;
    captureConfig.scaleY = 0.f;
    std::shared_ptr<RSSurfaceCaptureVisitor> visitor =
        std::make_shared<RSSurfaceCaptureVisitor>(captureConfig, false);
    ASSERT_NE(nullptr, visitor);
    RSCanvasRenderNode node(id);
    visitor->isUniRender_ = true;
    visitor->ProcessCanvasRenderNode(node);
}

/*
 * @tc.name: ProcessCanvasRenderNode002
 * @tc.desc: Test RSSurfaceCaptureTaskTest.ProcessCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RSSurfaceCaptureTaskTest, ProcessCanvasRenderNode002, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 0.f;
    captureConfig.scaleY = 0.f;
    std::shared_ptr<RSSurfaceCaptureVisitor> visitor =
        std::make_shared<RSSurfaceCaptureVisitor>(captureConfig, false);
    ASSERT_NE(nullptr, visitor);
    RSCanvasRenderNode node(id);
    visitor->isUniRender_ = false;
    visitor->ProcessCanvasRenderNode(node);
}

/*
 * @tc.name: ProcessSurfaceRenderNode001
 * @tc.desc: Test RSSurfaceCaptureTaskTest.ProcessSurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RSSurfaceCaptureTaskTest, ProcessSurfaceRenderNode001, Function | SmallTest | Level2)
{
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 0.f;
    captureConfig.scaleY = 0.f;
    std::shared_ptr<RSSurfaceCaptureVisitor> visitor =
        std::make_shared<RSSurfaceCaptureVisitor>(captureConfig, false);
    ASSERT_NE(nullptr, visitor);
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode node(config);
    visitor->canvas_ = nullptr;
    visitor->ProcessSurfaceRenderNode(node);
}

/*
 * @tc.name: ProcessSurfaceRenderNode002
 * @tc.desc: Test RSSurfaceCaptureTaskTest.ProcessSurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RSSurfaceCaptureTaskTest, ProcessSurfaceRenderNode002, Function | SmallTest | Level2)
{
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 0.f;
    captureConfig.scaleY = 0.f;
    std::shared_ptr<RSSurfaceCaptureVisitor> visitor =
        std::make_shared<RSSurfaceCaptureVisitor>(captureConfig, false);
    ASSERT_NE(nullptr, visitor);
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode node(config);
    node.GetMutableRenderProperties().SetAlpha(0.0f);
    visitor->ProcessSurfaceRenderNode(node);
}

/*
 * @tc.name: ProcessSurfaceRenderNode004
 * @tc.desc: Test RSSurfaceCaptureTaskTest.ProcessSurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RSSurfaceCaptureTaskTest, ProcessSurfaceRenderNode004, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, visitor_);
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode node(config);
    node.GetMutableRenderProperties().SetAlpha(0.0f);
    visitor_->IsDisplayNode(true);
    Drawing::Canvas drawingCanvas(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    visitor_->canvas_ = std::make_unique<RSPaintFilterCanvas>(&drawingCanvas);
    visitor_->ProcessSurfaceRenderNodeWithoutUni(node);
}

/*
 * @tc.name: ProcessSurfaceRenderNode005
 * @tc.desc: Test RSSurfaceCaptureTaskTest.ProcessSurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RSSurfaceCaptureTaskTest, ProcessSurfaceRenderNode005, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, visitor_);
    RSSurfaceRenderNodeConfig config;
    RSSurfaceRenderNode node(config);
    node.GetMutableRenderProperties().SetAlpha(0.0f);
    visitor_->IsDisplayNode(true);
    Drawing::Canvas drawingCanvas(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    visitor_->canvas_ = std::make_unique<RSPaintFilterCanvas>(&drawingCanvas);
    visitor_->ProcessSurfaceRenderNodeWithoutUni(node);
}

/*
 * @tc.name: ProcessRootRenderNode002
 * @tc.desc: Test RSSurfaceCaptureTaskTest.ProcessRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RSSurfaceCaptureTaskTest, ProcessRootRenderNode002, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, visitor_);
    RSRootRenderNode node(0);
    node.GetMutableRenderProperties().SetVisible(true);
    node.GetMutableRenderProperties().SetAlpha(DEFAULT_CANVAS_SCALE);
    visitor_->ProcessRootRenderNode(node);
    visitor_->ProcessCanvasRenderNode(node);
}

/*
 * @tc.name: ProcessRootRenderNode003
 * @tc.desc: Test RSSurfaceCaptureTaskTest.ProcessRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RSSurfaceCaptureTaskTest, ProcessRootRenderNode003, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, visitor_);
    RSRootRenderNode node(0);
    node.GetMutableRenderProperties().SetVisible(true);
    node.GetMutableRenderProperties().SetAlpha(DEFAULT_CANVAS_SCALE);
    visitor_->canvas_ = nullptr;
    visitor_->ProcessRootRenderNode(node);
    visitor_->ProcessCanvasRenderNode(node);
}

/*
 * @tc.name: ProcessRootRenderNode004
 * @tc.desc: Test RSSurfaceCaptureTaskTest.ProcessRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RSSurfaceCaptureTaskTest, ProcessRootRenderNode004, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, visitor_);
    RSRootRenderNode node(0);
    node.GetMutableRenderProperties().SetVisible(true);
    node.GetMutableRenderProperties().SetAlpha(.0f);
    visitor_->ProcessRootRenderNode(node);
    visitor_->ProcessCanvasRenderNode(node);
}

/*
 * @tc.name: CaptureSingleSurfaceNodeWithoutUni003
 * @tc.desc: Test RSSurfaceCaptureTaskTest.CaptureSingleSurfaceNodeWithoutUni
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RSSurfaceCaptureTaskTest, CaptureSingleSurfaceNodeWithoutUni003, Function | SmallTest | Level2)
{
    bool isUnirender = RSUniRenderJudgement::IsUniRender();
    ASSERT_NE(nullptr, visitor_);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    surfaceNode->SetSecurityLayer(false);
    if (!isUnirender) {
        visitor_->CaptureSingleSurfaceNodeWithoutUni(*surfaceNode);
    }
}

/*
 * @tc.name: CaptureSurfaceInDisplayWithoutUni002
 * @tc.desc: Test RSSurfaceCaptureTaskTest.CaptureSurfaceInDisplayWithoutUni
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RSSurfaceCaptureTaskTest, CaptureSurfaceInDisplayWithoutUni002, Function | SmallTest | Level2)
{
    bool isUnirender = RSUniRenderJudgement::IsUniRender();
    ASSERT_NE(nullptr, visitor_);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    surfaceNode->SetSecurityLayer(false);
    if (!isUnirender) {
        visitor_->CaptureSingleSurfaceNodeWithoutUni(*surfaceNode);
    }
}

/*
 * @tc.name: CaptureSurfaceInDisplayWithoutUni003
 * @tc.desc: Test RSSurfaceCaptureTaskTest.CaptureSurfaceInDisplayWithoutUni
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RSSurfaceCaptureTaskTest, CaptureSurfaceInDisplayWithoutUni003, Function | SmallTest | Level2)
{
    bool isUnirender = RSUniRenderJudgement::IsUniRender();
    ASSERT_NE(nullptr, visitor_);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    surfaceNode->SetSecurityLayer(false);
    if (!isUnirender) {
        visitor_->CaptureSingleSurfaceNodeWithoutUni(*surfaceNode);
    }
}

/*
 * @tc.name: ProcessSurfaceRenderNodeWithoutUni001
 * @tc.desc: Test RSSurfaceCaptureTaskTest.ProcessSurfaceRenderNodeWithoutUni
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RSSurfaceCaptureTaskTest, ProcessSurfaceRenderNodeWithoutUni001, Function | SmallTest | Level2)
{
    bool isUnirender = RSUniRenderJudgement::IsUniRender();
    ASSERT_NE(nullptr, visitor_);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    surfaceNode->SetSecurityLayer(true);
    visitor_->isDisplayNode_ = true;
    if (!isUnirender) {
        visitor_->ProcessSurfaceRenderNodeWithoutUni(*surfaceNode);
    }
}

/*
 * @tc.name: ProcessEffectRenderNode
 * @tc.desc: Test RSSurfaceCaptureTaskTest.ProcessEffectRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI7G9F0
 */
HWTEST_F(RSSurfaceCaptureTaskTest, ProcessEffectRenderNode, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, visitor_);
    NodeId id = DEFAULT_NODEID;
    RSEffectRenderNode node(id);
    Drawing::Canvas drawingCanvas(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    visitor_->canvas_ = std::make_unique<RSPaintFilterCanvas>(&drawingCanvas);
    visitor_->ProcessEffectRenderNode(node);
}

/*
 * @tc.name: ProcessDisplayRenderNode
 * @tc.desc: Test RSSurfaceCaptureTaskTest.ProcessDisplayRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI7G9F0
 */
HWTEST_F(RSSurfaceCaptureTaskTest, ProcessDisplayRenderNode, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, visitor_);
    NodeId id = DEFAULT_NODEID;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    RSDisplayNodeConfig config;
    RSDisplayRenderNode node(id, config);
    visitor_->ProcessDisplayRenderNode(node);
}

/*
 * @tc.name: ProcessCanvasRenderNode
 * @tc.desc: Test RSSurfaceCaptureTaskTest.ProcessCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI7G9F0
 */
HWTEST_F(RSSurfaceCaptureTaskTest, ProcessCanvasRenderNode, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, visitor_);
    auto isUniRender = visitor_->isUniRender_;
    visitor_->isUniRender_ = true;
    NodeId id = 1;
    RSCanvasRenderNode node1(id);
    node1.shouldPaint_ = false;
    visitor_->ProcessCanvasRenderNode(node1);
    id = 2;
    RSCanvasRenderNode node2(id);
    node2.shouldPaint_ = true;
    node2.GetMutableRenderProperties().isSpherizeValid_ = true;
    visitor_->ProcessCanvasRenderNode(node2);
    id = 3;
    RSCanvasRenderNode node3(id);
    node3.shouldPaint_ = true;
    node3.GetMutableRenderProperties().isSpherizeValid_ = false;
    node3.SetCacheType(CacheType::CONTENT);
    visitor_->ProcessCanvasRenderNode(node3);
    visitor_->isUniRender_ = isUniRender;
}

/*
 * @tc.name: ProcessDisplayRenderNode002
 * @tc.desc: Test RSSurfaceCaptureTaskTest.ProcessDisplayRenderNode002
 * @tc.type: FUNC
 * @tc.require: issueI7G9F0
 */
HWTEST_F(RSSurfaceCaptureTaskTest, ProcessDisplayRenderNode002, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, visitor_);
    NodeId id = 1;
    RSDisplayNodeConfig config;
    config.isMirrored = true;
    RSDisplayRenderNode node(id, config);
    visitor_->ProcessDisplayRenderNode(node);
}

/*
 * @tc.name: ProcessDisplayRenderNode003
 * @tc.desc: Test RSSurfaceCaptureTaskTest.ProcessDisplayRenderNode003
 * @tc.type: FUNC
 * @tc.require: issueI7G9F0
 */
HWTEST_F(RSSurfaceCaptureTaskTest, ProcessDisplayRenderNode003, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, visitor_);
    NodeId id = 1;
    RSDisplayNodeConfig config;
    RSDisplayRenderNode node(id, config);
    visitor_->ProcessDisplayRenderNode(node);
}

/*
 * @tc.name: ProcessDisplayRenderNode004
 * @tc.desc: Test RSSurfaceCaptureTaskTest.ProcessDisplayRenderNode004
 * @tc.type: FUNC
 * @tc.require: issueI7G9F0
 */
HWTEST_F(RSSurfaceCaptureTaskTest, ProcessDisplayRenderNode004, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, visitor_);
    NodeId id = 1;
    RSDisplayNodeConfig config;
    auto node = std::make_shared<RSDisplayRenderNode>(id, config);
    sptr<IConsumerSurface> consumer = IConsumerSurface::Create("test");
    auto displayDrawable =
        static_cast<RSDisplayRenderNodeDrawable*>(RSDisplayRenderNodeDrawable::OnGenerate(node));
    ASSERT_NE(nullptr, displayDrawable);
    auto surfaceHandler = displayDrawable->GetMutableRSSurfaceHandlerOnDraw();
    surfaceHandler->SetConsumer(consumer);
    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    int64_t timestamp = 0;
    Rect damage;
    sptr<OHOS::SurfaceBuffer> buffer = new SurfaceBufferImpl(0);
    surfaceHandler->SetBuffer(buffer, acquireFence, damage, timestamp);
    visitor_->ProcessDisplayRenderNode(*node);
}

/*
 * @tc.name: ProcessChildren001
 * @tc.desc: Test RSSurfaceCaptureTaskTest.ProcessChildren001
 * @tc.type: FUNC
 * @tc.require: issueI7G9F0
 */
HWTEST_F(RSSurfaceCaptureTaskTest, ProcessChildren001, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, visitor_);
    NodeId id = 1;
    auto node = std::make_shared<RSSurfaceRenderNode>(id);
    visitor_->ProcessChildren(*node);
}

/*
 * @tc.name: ProcessDisplayRenderNode005
 * @tc.desc: Test RSSurfaceCaptureTaskTest.ProcessDisplayRenderNode while curtain screen is on
 * @tc.type: FUNC
 * @tc.require: issueI7G9F0
 */
HWTEST_F(RSSurfaceCaptureTaskTest, ProcessDisplayRenderNode005, Function | SmallTest | Level2)
{
    NodeId id = 1;
    RSDisplayNodeConfig config;
    auto displayNode = std::make_shared<RSDisplayRenderNode>(id, config);

    RSMainThread::Instance()->SetCurtainScreenUsingStatus(true);

    ASSERT_NE(visitor_, nullptr);
    visitor_->isUniRender_ =true;
    visitor_->ProcessDisplayRenderNode(*displayNode);
    // restore curtain screen status
    RSMainThread::Instance()->SetCurtainScreenUsingStatus(false);
}

/*
 * @tc.name: TakeSurfaceCaptureWithBlurTest
 * @tc.desc: Test RSSurfaceCaptureTaskTest.TakeSurfaceCaptureWithBlur
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceCaptureTaskTest, TakeSurfaceCaptureWithBlurTest, Function | SmallTest | Level2)
{
    RSSurfaceCaptureConfig captureConfig;

    // test blurRadius is negative
    float blurRadius = -10;
    bool ret = rsInterfaces_->TakeSurfaceCaptureWithBlur(surfaceNode_, surfaceCaptureCb_, captureConfig, blurRadius);
    ASSERT_EQ(ret, true);
    ASSERT_EQ(CheckSurfaceCaptureCallback(), true);
#if defined(RS_ENABLE_UNI_RENDER)
    ASSERT_EQ(surfaceCaptureCb_->IsTestSuccess(), true);
#endif
    surfaceCaptureCb_->Reset();

    blurRadius = 10;
    ret = rsInterfaces_->TakeSurfaceCaptureWithBlur(surfaceNode_, surfaceCaptureCb_, captureConfig, blurRadius);
    ASSERT_EQ(ret, true);
    ASSERT_EQ(CheckSurfaceCaptureCallback(), true);
#if defined(RS_ENABLE_UNI_RENDER)
    ASSERT_EQ(surfaceCaptureCb_->IsTestSuccess(), true);
#endif
    surfaceCaptureCb_->Reset();

    // test blurRadius is large
    blurRadius = 100000;
    ret = rsInterfaces_->TakeSurfaceCaptureWithBlur(surfaceNode_, surfaceCaptureCb_, captureConfig, blurRadius);
    ASSERT_EQ(ret, true);
    ASSERT_EQ(CheckSurfaceCaptureCallback(), true);
#if defined(RS_ENABLE_UNI_RENDER)
    ASSERT_EQ(surfaceCaptureCb_->IsTestSuccess(), true);
#endif
}

/*
 * @tc.name: TakeSelfSurfaceCaptureTest001
 * @tc.desc: Test TakeSelfSurfaceCapture with null node or null callback
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceCaptureTaskTest, TakeSelfSurfaceCaptureTest001, Function | SmallTest | Level2)
{
    RSSurfaceCaptureConfig captureConfig;

    bool ret = rsInterfaces_->TakeSelfSurfaceCapture(nullptr, surfaceCaptureCb_, captureConfig);
    ASSERT_EQ(ret, false);

    ret = rsInterfaces_->TakeSelfSurfaceCapture(surfaceNode_, nullptr, captureConfig);
    ASSERT_EQ(ret, false);

    ret = rsInterfaces_->TakeSelfSurfaceCapture(surfaceNode_, surfaceCaptureCb_, captureConfig);
    ASSERT_EQ(ret, true);
#if defined(RS_ENABLE_UNI_RENDER)
    ASSERT_EQ(CheckSurfaceCaptureCallback(), true);
    ASSERT_EQ(surfaceCaptureCb_->IsTestSuccess(), true);
#endif
}

/*
 * @tc.name: TakeSelfSurfaceCaptureTest002
 * @tc.desc: Test TakeSelfSurfaceCapture with different scale
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceCaptureTaskTest, TakeSelfSurfaceCaptureTest002, Function | SmallTest | Level2)
{
    RSSurfaceCaptureConfig captureConfig;

    captureConfig.scaleX = -1.f;
    captureConfig.scaleY = -1.f;
    bool ret = rsInterfaces_->TakeSelfSurfaceCapture(surfaceNode_, surfaceCaptureCb_, captureConfig);
    ASSERT_EQ(ret, true);
#ifdef RS_ENABLE_UNI_RENDER
    ASSERT_EQ(CheckSurfaceCaptureCallback(), true);
    ASSERT_EQ(surfaceCaptureCb_->IsTestSuccess(), false);
#endif
    surfaceCaptureCb_->Reset();

    captureConfig.scaleX = 2.f;
    captureConfig.scaleY = 2.f;
    ret = rsInterfaces_->TakeSelfSurfaceCapture(surfaceNode_, surfaceCaptureCb_, captureConfig);
    ASSERT_EQ(ret, true);
#ifdef RS_ENABLE_UNI_RENDER
    ASSERT_EQ(CheckSurfaceCaptureCallback(), true);
    ASSERT_EQ(surfaceCaptureCb_->IsTestSuccess(), false);
#endif
    surfaceCaptureCb_->Reset();

    captureConfig.scaleX = 0.5;
    captureConfig.scaleY = 0.5;
    ret = rsInterfaces_->TakeSelfSurfaceCapture(surfaceNode_, surfaceCaptureCb_, captureConfig);
    ASSERT_EQ(ret, true);
#ifdef RS_ENABLE_UNI_RENDER
    ASSERT_EQ(CheckSurfaceCaptureCallback(), true);
    ASSERT_EQ(surfaceCaptureCb_->IsTestSuccess(), true);
#endif
}

/*
 * @tc.name: TakeSelfSurfaceCaptureTest003
 * @tc.desc: Test TakeSelfSurfaceCapture with different useDma
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceCaptureTaskTest, TakeSelfSurfaceCaptureTest003, Function | SmallTest | Level2)
{
    RSSurfaceCaptureConfig captureConfig;

    captureConfig.useDma = true;
    bool ret = rsInterfaces_->TakeSelfSurfaceCapture(surfaceNode_, surfaceCaptureCb_, captureConfig);
    ASSERT_EQ(ret, true);
#ifdef RS_ENABLE_UNI_RENDER
    ASSERT_EQ(CheckSurfaceCaptureCallback(), true);
    ASSERT_EQ(surfaceCaptureCb_->IsTestSuccess(), true);
#endif
    surfaceCaptureCb_->Reset();

    captureConfig.useDma = false;
    ret = rsInterfaces_->TakeSelfSurfaceCapture(surfaceNode_, surfaceCaptureCb_, captureConfig);
    ASSERT_EQ(ret, true);
#ifdef RS_ENABLE_UNI_RENDER
    ASSERT_EQ(CheckSurfaceCaptureCallback(), true);
    ASSERT_EQ(surfaceCaptureCb_->IsTestSuccess(), true);
#endif
}

/*
 * @tc.name: TakeSelfSurfaceCaptureTest004
 * @tc.desc: Test TakeSelfSurfaceCapture with different useCurWindow
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceCaptureTaskTest, TakeSelfSurfaceCaptureTest004, Function | SmallTest | Level2)
{
    RSSurfaceCaptureConfig captureConfig;

    captureConfig.useCurWindow = true;
    bool ret = rsInterfaces_->TakeSelfSurfaceCapture(surfaceNode_, surfaceCaptureCb_, captureConfig);
    ASSERT_EQ(ret, true);
#ifdef RS_ENABLE_UNI_RENDER
    ASSERT_EQ(CheckSurfaceCaptureCallback(), true);
    ASSERT_EQ(surfaceCaptureCb_->IsTestSuccess(), true);
#endif
    surfaceCaptureCb_->Reset();

    captureConfig.useCurWindow = false;
    ret = rsInterfaces_->TakeSelfSurfaceCapture(surfaceNode_, surfaceCaptureCb_, captureConfig);
    ASSERT_EQ(ret, true);
#ifdef RS_ENABLE_UNI_RENDER
    ASSERT_EQ(CheckSurfaceCaptureCallback(), true);
    ASSERT_EQ(surfaceCaptureCb_->IsTestSuccess(), true);
#endif
}

/*
 * @tc.name: CreateClientPixelMap
 * @tc.desc: Test CreateClientPixelMap
 * @tc.type: FUNC
 */
HWTEST_F(RSSurfaceCaptureTaskTest, CreateClientPixelMap, Function | SmallTest | Level2)
{
    Drawing::Rect rect = {0, 0, 1260, 2720};
    RSSurfaceCaptureConfig captureConfig;
    auto pixelMap = RSCapturePixelMapManager::GetClientCapturePixelMap(rect, captureConfig,
        UniRenderEnabledType::UNI_RENDER_DISABLED);
    EXPECT_EQ(pixelMap == nullptr, true);
}

} // namespace Rosen
} // namespace OHOS
