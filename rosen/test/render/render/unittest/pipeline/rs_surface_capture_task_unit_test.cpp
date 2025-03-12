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
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "platform/common/rs_system_properties.h"

using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS {
namespace Rosen {
using namespace HiviewDFX;
using DisplayId = ScreenId;
namespace {
    constexpr HiLogLabel LOG_LABEL = { LOG_CORE, 0xD140, "RsSurfaceCaptureTaskTest" };
    constexpr uint32_t SLEEP_TIME_FOR_PROXY = 1000; // 10ms
    constexpr float DEFAULT_BOUNDS_WIDTH = 10.f;
    constexpr float DEFAULT_BOUNDS_HEIGHT = 20.f;
    constexpr uint32_t DEFAULT_CANVAS_WIDTH = 80;
    constexpr uint32_t DEFAULT_CANVAS_HEIGHT = 60;
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

class RsSurfaceCaptureTaskTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();

    void SetUp() override;
    void TearDown() override;

    static std::shared_ptr<RSSurfaceNode> CreateSurface(std::string surfaceNodeName = "DefaultSurfaceNode");
    static void InitRenderContext();
    static void FillSurface(std::shared_ptr<RSSurfaceNode> surfaceNode,
        const Drawing::Color color = Drawing::Color::COLOR_WHITE);

    static RSInterfaces* rsInterfaces_;
    static RenderContext* renderContext_;
    static RSDisplayNodeConfig defaultConfig_;
    static RSDisplayNodeConfig mirrorConfig_;
    static std::shared_ptr<RSSurfaceNode> surfaceNode_;
    static std::shared_ptr<CustomizedSurfaceCapture> surfaceCaptureCb_;
};
RSInterfaces* RsSurfaceCaptureTaskTest::rsInterfaces_ = nullptr;
RenderContext* RsSurfaceCaptureTaskTest::renderContext_ = nullptr;
RSDisplayNodeConfig RsSurfaceCaptureTaskTest::defaultConfig_ = {INVALID_SCREEN_ID, false, INVALID_SCREEN_ID};
RSDisplayNodeConfig RsSurfaceCaptureTaskTest::mirrorConfig_ = {INVALID_SCREEN_ID, true, INVALID_SCREEN_ID};
std::shared_ptr<RSSurfaceNode> RsSurfaceCaptureTaskTest::surfaceNode_ = nullptr;
std::shared_ptr<CustomizedSurfaceCapture> RsSurfaceCaptureTaskTest::surfaceCaptureCb_ = nullptr;

void RsSurfaceCaptureTaskTest::SetUp()
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

void RsSurfaceCaptureTaskTest::TearDown()
{
    visitor_ = nullptr;
}

void RsSurfaceCaptureTaskTest::SetUpTestCase()
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

void RsSurfaceCaptureTaskTest::TearDownTestCase()
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

std::shared_ptr<RSSurfaceNode> RsSurfaceCaptureTaskTest::CreateSurface(std::string surfaceNodeName)
{
    RSSurfaceNodeConfig config;
    config.SurfaceNodeName = surfaceNodeName;
    return RSSurfaceNode::Create(config);
}

void RsSurfaceCaptureTaskTest::InitRenderContext()
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

void RsSurfaceCaptureTaskTest::FillSurface(std::shared_ptr<RSSurfaceNode> surfaceNode, const Drawing::Color color)
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

/*
 * @tc.name: Run1
 * @tc.desc: Test RsSurfaceCaptureTaskTest.Run
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RsSurfaceCaptureTaskTest, Run1, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 0.f;
    captureConfig.scaleY = 0.f;
    RSSurfaceCaptureTask task(id, captureConfig);
    ASSERT_EQ(false, task.Run(nullptr));
}

/*
 * @tc.name: Run02
 * @tc.desc: Test RsSurfaceCaptureTaskTest.Run
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RsSurfaceCaptureTaskTest, Run02, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleY = 0.f;
    RSSurfaceCaptureTask task(id, captureConfig);
    ASSERT_EQ(false, task.Run(nullptr));
}

/*
 * @tc.name: Run03
 * @tc.desc: Test RsSurfaceCaptureTaskTest.Run
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RsSurfaceCaptureTaskTest, Run03, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 0.f;
    RSSurfaceCaptureTask task(id, captureConfig);
    ASSERT_EQ(false, task.Run(nullptr));
}

/*
 * @tc.name: Run04
 * @tc.desc: Test RsSurfaceCaptureTaskTest.Run
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RsSurfaceCaptureTaskTest, Run04, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceCaptureConfig captureConfig;
    RSSurfaceCaptureTask task(id, captureConfig);
    ASSERT_EQ(false, task.Run(nullptr));
}

/*
 * @tc.name: Run05
 * @tc.desc: Test RsSurfaceCaptureTaskTest.Run
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RsSurfaceCaptureTaskTest, Run05, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = -1.0f;
    RSSurfaceCaptureTask task(id, captureConfig);
    ASSERT_EQ(false, task.Run(nullptr));
}

/*
 * @tc.name: Run07
 * @tc.desc: Test RsSurfaceCaptureTaskTest.Run
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RsSurfaceCaptureTaskTest, Run07, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleY = -1.0f;
    RSSurfaceCaptureTask task(id, captureConfig);
    ASSERT_EQ(false, task.Run(nullptr));
}

/*
 * @tc.name: CreatePixelMapByDisplayNode1
 * @tc.desc: Test RsSurfaceCaptureTaskTest.CreatePixelMapByDisplayNode
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RsSurfaceCaptureTaskTest, CreatePixelMapByDisplayNode1, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 0.f;
    captureConfig.scaleY = 0.f;
    RSSurfaceCaptureTask task(id, captureConfig);
    ASSERT_EQ(nullptr, task.CreatePixelMapByDisplayNode(nullptr));
}

/*
 * @tc.name: ProcessRootRenderNode1
 * @tc.desc: Test RsSurfaceCaptureTaskTest.ProcessRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RsSurfaceCaptureTaskTest, ProcessRootRenderNode1, Function | SmallTest | Level2)
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
 * @tc.name: CreatePixelMapBySurfaceNode1
 * @tc.desc: Test RsSurfaceCaptureTaskTest.CreatePixelMapBySurfaceNode
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RsSurfaceCaptureTaskTest, CreatePixelMapBySurfaceNode1, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 0.f;
    captureConfig.scaleY = 0.f;
    RSSurfaceCaptureTask task(id, captureConfig);
    ASSERT_EQ(nullptr, task.CreatePixelMapBySurfaceNode(nullptr, false));
}

/*
 * @tc.name: CreatePixelMapBySurfaceNode02
 * @tc.desc: Test RsSurfaceCaptureTaskTest.CreatePixelMapBySurfaceNode
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RsSurfaceCaptureTaskTest, CreatePixelMapBySurfaceNode02, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 0.f;
    captureConfig.scaleY = 0.f;
    RSSurfaceCaptureTask task(id, captureConfig);
    ASSERT_EQ(nullptr, task.CreatePixelMapBySurfaceNode(nullptr, true));
}

/*
 * @tc.name: CreatePixelMapBySurfaceNode03
 * @tc.desc: Test RsSurfaceCaptureTaskTest.CreatePixelMapBySurfaceNode
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RsSurfaceCaptureTaskTest, CreatePixelMapBySurfaceNode03, Function | SmallTest | Level2)
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
 * @tc.name: CreatePixelMapBySurfaceNode04
 * @tc.desc: Test RsSurfaceCaptureTaskTest.CreatePixelMapBySurfaceNode
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RsSurfaceCaptureTaskTest, CreatePixelMapBySurfaceNode04, Function | SmallTest | Level2)
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
 * @tc.name: CreateSurface1
 * @tc.desc: Test RsSurfaceCaptureTaskTest.CreateSurface1
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RsSurfaceCaptureTaskTest, CreateSurface1, Function | SmallTest | Level2)
{
    NodeId id = 0;
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 0.f;
    captureConfig.scaleY = 0.f;
    RSSurfaceCaptureTask task(id, captureConfig);
    ASSERT_EQ(nullptr, task.CreateSurface(nullptr));
}

/*
 * @tc.name: CreateSurface02
 * @tc.desc: Test RsSurfaceCaptureTaskTest.CreateSurface02
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RsSurfaceCaptureTaskTest, CreateSurface02, Function | SmallTest | Level2)
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
 * @tc.desc: Test RsSurfaceCaptureTaskTest.SetSurface
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RsSurfaceCaptureTaskTest, SetSurface, Function | SmallTest | Level2)
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
 * @tc.name: ProcessCanvasRenderNode1
 * @tc.desc: Test RsSurfaceCaptureTaskTest.ProcessCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RsSurfaceCaptureTaskTest, ProcessCanvasRenderNode1, Function | SmallTest | Level2)
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
 * @tc.name: ProcessCanvasRenderNode02
 * @tc.desc: Test RsSurfaceCaptureTaskTest.ProcessCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RsSurfaceCaptureTaskTest, ProcessCanvasRenderNode02, Function | SmallTest | Level2)
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
 * @tc.name: ProcessSurfaceRenderNode1
 * @tc.desc: Test RsSurfaceCaptureTaskTest.ProcessSurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RsSurfaceCaptureTaskTest, ProcessSurfaceRenderNode1, Function | SmallTest | Level2)
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
 * @tc.name: ProcessSurfaceRenderNode02
 * @tc.desc: Test RsSurfaceCaptureTaskTest.ProcessSurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RsSurfaceCaptureTaskTest, ProcessSurfaceRenderNode02, Function | SmallTest | Level2)
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
 * @tc.name: ProcessSurfaceRenderNode04
 * @tc.desc: Test RsSurfaceCaptureTaskTest.ProcessSurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RsSurfaceCaptureTaskTest, ProcessSurfaceRenderNode04, Function | SmallTest | Level2)
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
 * @tc.name: ProcessSurfaceRenderNode05
 * @tc.desc: Test RsSurfaceCaptureTaskTest.ProcessSurfaceRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RsSurfaceCaptureTaskTest, ProcessSurfaceRenderNode05, Function | SmallTest | Level2)
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
 * @tc.name: ProcessSurfaceRenderNodeWithoutUni1
 * @tc.desc: Test RsSurfaceCaptureTaskTest.ProcessSurfaceRenderNodeWithoutUni
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RsSurfaceCaptureTaskTest, ProcessSurfaceRenderNodeWithoutUni1, Function | SmallTest | Level2)
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
 * @tc.name: ProcessRootRenderNode02
 * @tc.desc: Test RsSurfaceCaptureTaskTest.ProcessRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RsSurfaceCaptureTaskTest, ProcessRootRenderNode02, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, visitor_);
    RSRootRenderNode node(0);
    node.GetMutableRenderProperties().SetVisible(true);
    node.GetMutableRenderProperties().SetAlpha(DEFAULT_CANVAS_SCALE);
    visitor_->ProcessRootRenderNode(node);
    visitor_->ProcessCanvasRenderNode(node);
}

/*
 * @tc.name: ProcessRootRenderNode03
 * @tc.desc: Test RsSurfaceCaptureTaskTest.ProcessRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RsSurfaceCaptureTaskTest, ProcessRootRenderNode03, Function | SmallTest | Level2)
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
 * @tc.name: ProcessRootRenderNode04
 * @tc.desc: Test RsSurfaceCaptureTaskTest.ProcessRootRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RsSurfaceCaptureTaskTest, ProcessRootRenderNode04, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, visitor_);
    RSRootRenderNode node(0);
    node.GetMutableRenderProperties().SetVisible(true);
    node.GetMutableRenderProperties().SetAlpha(.0f);
    visitor_->ProcessRootRenderNode(node);
    visitor_->ProcessCanvasRenderNode(node);
}

/*
 * @tc.name: CaptureSingleSurfaceNodeWithoutUni03
 * @tc.desc: Test RsSurfaceCaptureTaskTest.CaptureSingleSurfaceNodeWithoutUni
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RsSurfaceCaptureTaskTest, CaptureSingleSurfaceNodeWithoutUni03, Function | SmallTest | Level2)
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
 * @tc.name: CaptureSurfaceInDisplayWithoutUni02
 * @tc.desc: Test RsSurfaceCaptureTaskTest.CaptureSurfaceInDisplayWithoutUni
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RsSurfaceCaptureTaskTest, CaptureSurfaceInDisplayWithoutUni02, Function | SmallTest | Level2)
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
 * @tc.name: CaptureSurfaceInDisplayWithoutUni03
 * @tc.desc: Test RsSurfaceCaptureTaskTest.CaptureSurfaceInDisplayWithoutUni
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RsSurfaceCaptureTaskTest, CaptureSurfaceInDisplayWithoutUni03, Function | SmallTest | Level2)
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
 * @tc.name: ProcessEffectRenderNode
 * @tc.desc: Test RsSurfaceCaptureTaskTest.ProcessEffectRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI7G9F0
 */
HWTEST_F(RsSurfaceCaptureTaskTest, ProcessEffectRenderNode, Function | SmallTest | Level2)
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
 * @tc.desc: Test RsSurfaceCaptureTaskTest.ProcessDisplayRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI7G9F0
 */
HWTEST_F(RsSurfaceCaptureTaskTest, ProcessDisplayRenderNode, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, visitor_);
    NodeId id = DEFAULT_NODEID;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    RSDisplayNodeConfig config;
    RSDisplayRenderNode node(id, config);
    visitor_->ProcessDisplayRenderNode(node);
}

/*
 * @tc.name: ProcessChildren1
 * @tc.desc: Test RsSurfaceCaptureTaskTest.ProcessChildren1
 * @tc.type: FUNC
 * @tc.require: issueI7G9F0
 */
HWTEST_F(RsSurfaceCaptureTaskTest, ProcessChildren1, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, visitor_);
    NodeId id = 1;
    auto node = std::make_shared<RSSurfaceRenderNode>(id);
    visitor_->ProcessChildren(*node);
}

/*
 * @tc.name: ProcessCanvasRenderNode
 * @tc.desc: Test RsSurfaceCaptureTaskTest.ProcessCanvasRenderNode
 * @tc.type: FUNC
 * @tc.require: issueI7G9F0
 */
HWTEST_F(RsSurfaceCaptureTaskTest, ProcessCanvasRenderNode, Function | SmallTest | Level2)
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
 * @tc.name: ProcessDisplayRenderNode02
 * @tc.desc: Test RsSurfaceCaptureTaskTest.ProcessDisplayRenderNode02
 * @tc.type: FUNC
 * @tc.require: issueI7G9F0
 */
HWTEST_F(RsSurfaceCaptureTaskTest, ProcessDisplayRenderNode02, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, visitor_);
    NodeId id = 1;
    RSDisplayNodeConfig config;
    config.isMirrored = true;
    RSDisplayRenderNode node(id, config);
    visitor_->ProcessDisplayRenderNode(node);
}

/*
 * @tc.name: ProcessDisplayRenderNode03
 * @tc.desc: Test RsSurfaceCaptureTaskTest.ProcessDisplayRenderNode03
 * @tc.type: FUNC
 * @tc.require: issueI7G9F0
 */
HWTEST_F(RsSurfaceCaptureTaskTest, ProcessDisplayRenderNode03, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, visitor_);
    NodeId id = 1;
    RSDisplayNodeConfig config;
    RSDisplayRenderNode node(id, config);
    visitor_->ProcessDisplayRenderNode(node);
}

/*
 * @tc.name: ProcessDisplayRenderNode04
 * @tc.desc: Test RsSurfaceCaptureTaskTest.ProcessDisplayRenderNode04
 * @tc.type: FUNC
 * @tc.require: issueI7G9F0
 */
HWTEST_F(RsSurfaceCaptureTaskTest, ProcessDisplayRenderNode04, Function | SmallTest | Level2)
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
 * @tc.name: ProcessDisplayRenderNode05
 * @tc.desc: Test RsSurfaceCaptureTaskTest.ProcessDisplayRenderNode while curtain screen is on
 * @tc.type: FUNC
 * @tc.require: issueI7G9F0
 */
HWTEST_F(RsSurfaceCaptureTaskTest, ProcessDisplayRenderNode05, Function | SmallTest | Level2)
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
} // namespace Rosen
} // namespace OHOS