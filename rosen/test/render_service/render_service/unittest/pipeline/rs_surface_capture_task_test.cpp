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

#include "rs_test_util.h"
#include "surface_buffer_impl.h"
#include "pipeline/rs_surface_capture_task.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "transaction/rs_interfaces.h"
#include "ui/rs_surface_extractor.h"
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
    constexpr HiLogLabel LOG_LABEL = { LOG_CORE, 0xD001400, "RSSurfaceCaptureTaskTest" };
    constexpr uint32_t SLEEP_TIME_FOR_PROXY = 100000; // 100ms
    constexpr float DEFAULT_BOUNDS_WIDTH = 100.f;
    constexpr float DEFAULT_BOUNDS_HEIGHT = 200.f;
    constexpr uint32_t DEFAULT_CANVAS_WIDTH = 800;
    constexpr uint32_t DEFAULT_CANVAS_HEIGHT = 600;
    constexpr float DEFAULT_CANVAS_SCALE = 1.0f;
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
    surfaceCaptureCb_->Reset();
    bool isUnirender = RSUniRenderJudgement::IsUniRender();
    visitor_ = std::make_shared<RSSurfaceCaptureVisitor>(DEFAULT_CANVAS_SCALE, DEFAULT_CANVAS_SCALE, isUnirender);
    if (visitor_ == nullptr) {
        return;
    }
    drawingCanvas_ = std::make_shared<Drawing::Canvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    visitor_->canvas_ = std::make_unique<RSPaintFilterCanvas>(drawingCanvas_.get());
    visitor_->renderEngine_ = std::make_shared<RSUniRenderEngine>();
    visitor_->renderEngine_->Init();
}

void RSSurfaceCaptureTaskTest::TearDown()
{
    visitor_ = nullptr;
}

void RSSurfaceCaptureTaskTest::SetUpTestCase()
{
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
    FillSurface(surfaceNode_);
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
    if (RSSystemProperties::GetGpuApiType() == GpuApiType::VULKAN ||
        RSSystemProperties::GetGpuApiType() == GpuApiType::DDGR) {
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
    if (RSSystemProperties::GetGpuApiType() != GpuApiType::VULKAN &&
        RSSystemProperties::GetGpuApiType() != GpuApiType::DDGR) {
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
 * @tc.name: Run001
 * @tc.desc: Test RSSurfaceCaptureTaskTest.Run
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RSSurfaceCaptureTaskTest, Run001, Function | SmallTest | Level2)
{
    NodeId id = 0;
    float scaleX = 0.f;
    float scaleY = 0.f;
    RSSurfaceCaptureTask task(id, scaleX, scaleY);
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
    float scaleX = DEFAULT_CANVAS_SCALE;
    float scaleY = 0.f;
    RSSurfaceCaptureTask task(id, scaleX, scaleY);
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
    float scaleX = 0.f;
    float scaleY = DEFAULT_BOUNDS_HEIGHT;
    RSSurfaceCaptureTask task(id, scaleX, scaleY);
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
    float scaleX = DEFAULT_CANVAS_SCALE;
    float scaleY = DEFAULT_CANVAS_SCALE;
    RSSurfaceCaptureTask task(id, scaleX, scaleY);
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
    float scaleX = -1.0f;
    float scaleY = DEFAULT_CANVAS_SCALE;
    RSSurfaceCaptureTask task(id, scaleX, scaleY);
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
    float scaleX = DEFAULT_CANVAS_SCALE;
    float scaleY = -1.0f;
    RSSurfaceCaptureTask task(id, scaleX, scaleY);
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
    float scaleX = 0.f;
    float scaleY = 0.f;
    RSSurfaceCaptureTask task(id, scaleX, scaleY);
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
    float scaleX = 0.f;
    float scaleY = 0.f;
    RSSurfaceCaptureTask task(id, scaleX, scaleY);
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
    float scaleX = 0.f;
    float scaleY = 0.f;
    RSSurfaceCaptureTask task(id, scaleX, scaleY);
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
    float scaleX = 0.f;
    float scaleY = 0.f;
    RSSurfaceCaptureTask task(id, scaleX, scaleY);
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
    float scaleX = 0.f;
    float scaleY = 0.f;
    RSSurfaceCaptureTask task(id, scaleX, scaleY);
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
    float scaleX = 0.f;
    float scaleY = 0.f;
    RSSurfaceCaptureTask task(id, scaleX, scaleY);
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
    float scaleX = 0.f;
    float scaleY = 0.f;
    RSSurfaceCaptureTask task(id, scaleX, scaleY);
    std::unique_ptr<Media::PixelMap> pixelmap = nullptr;
    ASSERT_EQ(nullptr, task.CreateSurface(pixelmap));
}

/*
 * @tc.name: FindSecurityOrSkipLayer001
 * @tc.desc: Test RSSurfaceCaptureTaskTest.CreateSurface002
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RSSurfaceCaptureTaskTest, FindSecurityOrSkipLayer001, Function | SmallTest | Level2)
{
    NodeId id = 0;
    float scaleX = 0.f;
    float scaleY = 0.f;
    RSSurfaceCaptureTask task(id, scaleX, scaleY);
    EXPECT_EQ(false, task.FindSecurityOrSkipLayer());
}

/*
 * @tc.name: SetSurface
 * @tc.desc: Test RSSurfaceCaptureTaskTest.SetSurface
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RSSurfaceCaptureTaskTest, SetSurface, Function | SmallTest | Level2)
{
    float scaleX = 0.f;
    float scaleY = 0.f;
    std::shared_ptr<RSSurfaceCaptureVisitor> visitor =
        std::make_shared<RSSurfaceCaptureVisitor>(scaleX, scaleY, false);
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
    float scaleX = 0.f;
    float scaleY = 0.f;
    std::shared_ptr<RSSurfaceCaptureVisitor> visitor =
        std::make_shared<RSSurfaceCaptureVisitor>(scaleX, scaleY, false);
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
    float scaleX = 0.f;
    float scaleY = 0.f;
    std::shared_ptr<RSSurfaceCaptureVisitor> visitor =
        std::make_shared<RSSurfaceCaptureVisitor>(scaleX, scaleY, false);
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
    float scaleX = 0.f;
    float scaleY = 0.f;
    std::shared_ptr<RSSurfaceCaptureVisitor> visitor =
        std::make_shared<RSSurfaceCaptureVisitor>(scaleX, scaleY, false);
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
    float scaleX = 0.f;
    float scaleY = 0.f;
    std::shared_ptr<RSSurfaceCaptureVisitor> visitor =
        std::make_shared<RSSurfaceCaptureVisitor>(scaleX, scaleY, false);
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
    float scaleX = 0.f;
    float scaleY = 0.f;
    std::shared_ptr<RSSurfaceCaptureVisitor> visitor =
        std::make_shared<RSSurfaceCaptureVisitor>(scaleX, scaleY, false);
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
 * @tc.name: AdjustZOrderAndDrawSurfaceNode001
 * @tc.desc: Test RSSurfaceCaptureTaskTest.AdjustZOrderAndDrawSurfaceNode
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RSSurfaceCaptureTaskTest, AdjustZOrderAndDrawSurfaceNode001, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, visitor_);
    RSSurfaceRenderNodeConfig config;
    auto node = std::make_shared<RSSurfaceRenderNode>(config);
    visitor_->AdjustZOrderAndDrawSurfaceNode(visitor_->hardwareEnabledNodes_);
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
 * @tc.name: CaptureSurfaceInDisplayWithUni003
 * @tc.desc: Test RSSurfaceCaptureTaskTest.CaptureSurfaceInDisplayWithUni
 * @tc.type: FUNC
 * @tc.require: issueI794H6
*/
HWTEST_F(RSSurfaceCaptureTaskTest, CaptureSurfaceInDisplayWithUni001, Function | SmallTest | Level2)
{
    bool isUnirender = RSUniRenderJudgement::IsUniRender();
    ASSERT_NE(nullptr, visitor_);
    auto surfaceNode = RSTestUtil::CreateSurfaceNode();
    surfaceNode->SetSecurityLayer(false);
    if (!isUnirender) {
        visitor_->CaptureSingleSurfaceNodeWithUni(*surfaceNode);
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
    NodeId id = 0;
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
    NodeId id = 0;
    auto surfaceNode = std::make_shared<RSSurfaceRenderNode>(id);
    (visitor_->hardwareEnabledNodes_).emplace_back(surfaceNode);
    RSDisplayNodeConfig config;
    RSDisplayRenderNode node(id, config);
    visitor_->ProcessDisplayRenderNode(node);
}

/*
 * @tc.name: DrawBlurInCache001
 * @tc.desc: Test RSSurfaceCaptureTaskTest.DrawBlurInCache001
 * @tc.type: FUNC
 * @tc.require: issueI7G9F0
 */
HWTEST_F(RSSurfaceCaptureTaskTest, DrawBlurInCache001, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, visitor_);
    NodeId id = 1;
    visitor_->curCacheFilterRects_.insert(id);
    auto node = std::make_shared<RSRenderNode>(id);
    ASSERT_TRUE(visitor_->DrawBlurInCache(*node));
    visitor_->curCacheFilterRects_.clear();
}

/*
 * @tc.name: DrawBlurInCache002
 * @tc.desc: Test RSSurfaceCaptureTaskTest.DrawBlurInCache002
 * @tc.type: FUNC
 * @tc.require: issueI7G9F0
 */
HWTEST_F(RSSurfaceCaptureTaskTest, DrawBlurInCache002, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, visitor_);
    NodeId id = 1;
    visitor_->curCacheFilterRects_.insert(id);
    auto node = std::make_shared<RSRenderNode>(id+1);
    node->SetChildHasFilter(false);
    ASSERT_TRUE(visitor_->DrawBlurInCache(*node));
    visitor_->curCacheFilterRects_.clear();
}

/*
 * @tc.name: DrawBlurInCache003
 * @tc.desc: Test RSSurfaceCaptureTaskTest.DrawBlurInCache003
 * @tc.type: FUNC
 * @tc.require: issueI7G9F0
 */
HWTEST_F(RSSurfaceCaptureTaskTest, DrawBlurInCache003, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, visitor_);
    NodeId id = 1;
    visitor_->curCacheFilterRects_.insert(id);
    visitor_->curCacheFilterRects_.insert(id+1);
    auto node = std::make_shared<RSRenderNode>(id);
    node->SetChildHasFilter(false);
    ASSERT_TRUE(visitor_->DrawBlurInCache(*node));
    visitor_->curCacheFilterRects_.clear();
}

/*
 * @tc.name: ProcessCacheFilterRects001
 * @tc.desc: Test RSSurfaceCaptureTaskTest.ProcessCacheFilterRects001
 * @tc.type: FUNC
 * @tc.require: issueI7G9F0
 */
HWTEST_F(RSSurfaceCaptureTaskTest, ProcessCacheFilterRects001, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, visitor_);
    NodeId id = 1;
    auto node = std::make_shared<RSRenderNode>(id);
    visitor_->ProcessCacheFilterRects(*node);
}

/*
 * @tc.name: ProcessCacheFilterRects002
 * @tc.desc: Test RSSurfaceCaptureTaskTest.ProcessCacheFilterRects002
 * @tc.type: FUNC
 * @tc.require: issueI7G9F0
 */
HWTEST_F(RSSurfaceCaptureTaskTest, ProcessCacheFilterRects002, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, visitor_);
    NodeId id = 1;
    auto node = std::make_shared<RSRenderNode>(id);
    visitor_->curCacheFilterRects_.insert(id);
    visitor_->ProcessCacheFilterRects(*node);
    visitor_->curCacheFilterRects_.clear();
}

/*
 * @tc.name: DrawChildRenderNode001
 * @tc.desc: Test RSSurfaceCaptureTaskTest.DrawChildRenderNode001
 * @tc.type: FUNC
 * @tc.require: issueI7G9F0
 */
HWTEST_F(RSSurfaceCaptureTaskTest, DrawChildRenderNode001, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, visitor_);
    NodeId id = 1;
    auto node1 = std::make_shared<RSRenderNode>(id);
    node1->SetCacheType(CacheType::NONE);
    visitor_->DrawChildRenderNode(*node1);
    id = 2;
    auto node2 = std::make_shared<RSRenderNode>(id);
    node2->SetCacheType(CacheType::CONTENT);
    visitor_->DrawChildRenderNode(*node2);
    id = 3;
    auto node3 = std::make_shared<RSRenderNode>(id);
    node3->SetCacheType(CacheType::ANIMATE_PROPERTY);
    visitor_->DrawChildRenderNode(*node3);
}

/*
 * @tc.name: DrawSpherize001
 * @tc.desc: Test RSSurfaceCaptureTaskTest.DrawSpherize001
 * @tc.type: FUNC
 * @tc.require: issueI7G9F0
 */
HWTEST_F(RSSurfaceCaptureTaskTest, DrawSpherize001, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, visitor_);
    NodeId id = 1;
    auto node = std::make_shared<RSRenderNode>(id);
    visitor_->DrawSpherize(*node);
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
 * @tc.name: FindHardwareEnabledNodes
 * @tc.desc: Test RSSurfaceCaptureTaskTest.FindHardwareEnabledNodes
 * @tc.type: FUNC
 * @tc.require: issueI7G9F0
 */
HWTEST_F(RSSurfaceCaptureTaskTest, FindHardwareEnabledNodes, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, visitor_);
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    nodeMap.surfaceNodeMap_[0] = nullptr;
    NodeId id = 1;
    auto node1 = std::make_shared<RSSurfaceRenderNode>(id);
    nodeMap.surfaceNodeMap_[1] = node1;
    id = 2;
    auto node2 = std::make_shared<RSSurfaceRenderNode>(id);
    node2->SetIsOnTheTree(true);
    nodeMap.surfaceNodeMap_[2] = node2;
    id = 3;
    auto node3 = std::make_shared<RSSurfaceRenderNode>(id);
    node3->SetIsOnTheTree(true);
    node3->isLastFrameHardwareEnabled_ = true;
    nodeMap.surfaceNodeMap_[3] = node3;
    visitor_->FindHardwareEnabledNodes();
    visitor_->hardwareEnabledNodes_.clear();
    nodeMap.surfaceNodeMap_.clear();
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
    auto hasSecurityOrSkipLayer = visitor_->hasSecurityOrSkipLayer_;
    visitor_->hasSecurityOrSkipLayer_ = true;
    NodeId id = 1;
    RSDisplayNodeConfig config;
    RSDisplayRenderNode node(id, config);
    visitor_->ProcessDisplayRenderNode(node);
    visitor_->hasSecurityOrSkipLayer_ = hasSecurityOrSkipLayer;
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
    auto hasSecurityOrSkipLayer = visitor_->hasSecurityOrSkipLayer_;
    visitor_->hasSecurityOrSkipLayer_ = true;
    NodeId id = 1;
    RSDisplayNodeConfig config;
    RSDisplayRenderNode node(id, config);
    sptr<IConsumerSurface> consumer = IConsumerSurface::Create("test");
    node.SetConsumer(consumer);
    sptr<SyncFence> acquireFence = SyncFence::INVALID_FENCE;
    int64_t timestamp = 0;
    Rect damage;
    sptr<OHOS::SurfaceBuffer> buffer = new SurfaceBufferImpl(0);
    node.SetBuffer(buffer, acquireFence, damage, timestamp);
    visitor_->ProcessDisplayRenderNode(node);
    visitor_->hasSecurityOrSkipLayer_ = hasSecurityOrSkipLayer;
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
 * @tc.name: ProcessChildren002
 * @tc.desc: Test RSSurfaceCaptureTaskTest.ProcessChildren002
 * @tc.type: FUNC
 * @tc.require: issueI7G9F0
 */
HWTEST_F(RSSurfaceCaptureTaskTest, ProcessChildren002, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, visitor_);
    NodeId id = 1;
    auto node1 = std::make_shared<RSSurfaceRenderNode>(id);
    visitor_->curCacheFilterRects_.insert(id);
    id = 2;
    auto node2 = std::make_shared<RSSurfaceRenderNode>(id);
    node1->AddChild(node2);
    visitor_->ProcessChildren(*node1);
    visitor_->curCacheFilterRects_.clear();
}

/*
 * @tc.name: AdjustZOrderAndDrawSurfaceNode
 * @tc.desc: Test RSSurfaceCaptureTaskTest.AdjustZOrderAndDrawSurfaceNode
 * @tc.type: FUNC
 * @tc.require: issueI7G9F0
 */
HWTEST_F(RSSurfaceCaptureTaskTest, AdjustZOrderAndDrawSurfaceNode, Function | SmallTest | Level2)
{
    ASSERT_NE(nullptr, visitor_);
    auto node1 = RSTestUtil::CreateSurfaceNode();
    auto node2 = RSTestUtil::CreateSurfaceNodeWithBuffer();
    auto node3 = RSTestUtil::CreateSurfaceNodeWithBuffer();
    node3->isLastFrameHardwareEnabled_ = true;
    std::vector<std::shared_ptr<RSSurfaceRenderNode>> nodes;
    nodes.push_back(node1);
    nodes.push_back(node2);
    nodes.push_back(node3);
    visitor_->AdjustZOrderAndDrawSurfaceNode(nodes);
}

/*
 * @tc.name: FindSecurityOrSkipLayer
 * @tc.desc: Test RSSurfaceCaptureTaskTest.FindSecurityOrSkipLayer
 * @tc.type: FUNC
 * @tc.require: issueI7G9F0
 */
HWTEST_F(RSSurfaceCaptureTaskTest, FindSecurityOrSkipLayer, Function | SmallTest | Level2)
{
    float scaleX = 0.f;
    float scaleY = 0.f;
    RSSurfaceCaptureTask task(0, scaleX, scaleY);
    auto& nodeMap = RSMainThread::Instance()->GetContext().GetMutableNodeMap();
    nodeMap.surfaceNodeMap_[0] = nullptr;
    NodeId id = 1;
    auto node1 = std::make_shared<RSSurfaceRenderNode>(id);
    nodeMap.surfaceNodeMap_[1] = node1;
    id = 2;
    auto node2 = std::make_shared<RSSurfaceRenderNode>(id);
    node2->SetIsOnTheTree(true);
    nodeMap.surfaceNodeMap_[2] = node2;
    id = 3;
    auto node3 = std::make_shared<RSSurfaceRenderNode>(id);
    node3->SetIsOnTheTree(true);
    node3->isSecurityLayer_ = true;
    nodeMap.surfaceNodeMap_[3] = node3;
    ASSERT_TRUE(task.FindSecurityOrSkipLayer());
    nodeMap.surfaceNodeMap_.clear();
}
} // namespace Rosen
} // namespace OHOS
