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
#include "pipeline/rs_surface_capture_task.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "transaction/rs_interfaces.h"
#include "ui/rs_surface_extractor.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "pipeline/rs_uni_render_engine.h"

using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using namespace HiviewDFX;
using DisplayId = ScreenId;
namespace {
    constexpr HiLogLabel LOG_LABEL = { LOG_CORE, 0xD001400, "RSSurfaceCaptureTaskTest" };
    constexpr uint32_t MAX_TIME_WAITING_FOR_CALLBACK = 200;
    constexpr uint32_t SLEEP_TIME_IN_US = 10000; // 10ms
    constexpr uint32_t SLEEP_TIME_FOR_PROXY = 100000; // 100ms
    constexpr float DEFAULT_BOUNDS_WIDTH = 100.f;
    constexpr float DEFAULT_BOUNDS_HEIGHT = 200.f;
    constexpr uint32_t DEFAULT_CANVAS_WIDTH = 800;
    constexpr uint32_t DEFAULT_CANVAS_HEIGHT = 600;
    constexpr float DEFAULT_CANVAS_SCALE = 1.0f;
    std::shared_ptr<SkCanvas> skCanvas_;
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
    void TearDown() override {};

    static std::shared_ptr<RSSurfaceNode> CreateSurface(std::string surfaceNodeName = "DefaultSurfaceNode");
    static void InitRenderContext();
    static void FillSurface(std::shared_ptr<RSSurfaceNode> surfaceNode, const SkColor color = SK_ColorWHITE);
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
    surfaceCaptureCb_->Reset();
    bool isUnirender = RSUniRenderJudgement::IsUniRender();
    visitor_ = std::make_shared<RSSurfaceCaptureVisitor>(DEFAULT_CANVAS_SCALE, DEFAULT_CANVAS_SCALE, isUnirender);
    if (visitor_ == nullptr) {
        return;
    }
    skCanvas_ = std::make_shared<SkCanvas>(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    visitor_->canvas_ = std::make_unique<RSPaintFilterCanvas>(skCanvas_.get());
    visitor_->renderEngine_ = std::make_shared<RSUniRenderEngine>();
    visitor_->renderEngine_->Init();
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
    if (renderContext_ == nullptr) {
        HiLog::Info(LOG_LABEL, "%s: init renderContext_", __func__);
        renderContext_ = RenderContextFactory::GetInstance().CreateEngine();
        renderContext_->InitializeEglContext();
    }
#endif // ACE_ENABLE_GL
}

void RSSurfaceCaptureTaskTest::FillSurface(std::shared_ptr<RSSurfaceNode> surfaceNode, const SkColor color)
{
    Vector4f bounds = { 0.0f, 0.0f, DEFAULT_BOUNDS_WIDTH, DEFAULT_BOUNDS_HEIGHT};
    surfaceNode->SetBounds(bounds); // x, y, w, h
    std::shared_ptr<RSSurface> rsSurface = RSSurfaceExtractor::ExtractRSSurface(surfaceNode);
    if (rsSurface == nullptr) {
        HiLog::Error(LOG_LABEL, "%s: rsSurface == nullptr", __func__);
        return;
    }
#ifdef ACE_ENABLE_GL
    HiLog::Info(LOG_LABEL, "ACE_ENABLE_GL");
    InitRenderContext();
    rsSurface->SetRenderContext(renderContext_);
#endif // ACE_ENABLE_GL
    auto frame = rsSurface->RequestFrame(DEFAULT_BOUNDS_WIDTH, DEFAULT_BOUNDS_HEIGHT);
    std::unique_ptr<RSSurfaceFrame> framePtr = std::move(frame);
    auto canvas = framePtr->GetCanvas();

    auto skRect = SkRect::MakeXYWH(0.0f, 0.0f, DEFAULT_BOUNDS_WIDTH, DEFAULT_BOUNDS_HEIGHT);
    SkPaint paint;
    paint.setStyle(SkPaint::kFill_Style);
    paint.setColor(color);
    canvas->drawRect(skRect, paint);
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
 * @tc.name: TakeSurfaceCaptureOfInvalidSurfaceNode
 * @tc.desc: Generate surface node without request buffer and take empty capture
 * @tc.type: FUNC
 * @tc.require: issueI5T8FR
*/
HWTEST_F(RSSurfaceCaptureTaskTest, TakeSurfaceCaptureOfInvalidSurfaceNode, Function | SmallTest | Level2)
{
    auto surfaceNode = CreateSurface("SurfaceCaptureTestEmptyNode");
    ASSERT_NE(surfaceNode, nullptr);

    bool ret = rsInterfaces_->TakeSurfaceCapture(surfaceNode, surfaceCaptureCb_);
    ASSERT_EQ(ret, true);
    ASSERT_EQ(CheckSurfaceCaptureCallback(), true);
    ASSERT_EQ(surfaceCaptureCb_->IsTestSuccess(), false);
    surfaceNode = nullptr;
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
}

/*
 * @tc.name: TakeSurfaceCaptureByNodeId
 * @tc.desc: take capture by NodeId
 * @tc.type: FUNC
 * @tc.require: issueI6Q844
*/
HWTEST_F(RSSurfaceCaptureTaskTest, TakeSurfaceCaptureByNodeId, Function | SmallTest | Level2)
{
    auto surfaceNode = CreateSurface("SurfaceCaptureTestEmptyNode");
    ASSERT_NE(surfaceNode, nullptr);

    bool ret = rsInterfaces_->TakeSurfaceCapture(surfaceNode->GetId(), surfaceCaptureCb_);
    ASSERT_EQ(ret, true);
    ASSERT_EQ(CheckSurfaceCaptureCallback(), true);
    ASSERT_EQ(surfaceCaptureCb_->IsTestSuccess(), false);
    surfaceNode = nullptr;
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
}

/*
 * @tc.name: TakeSurfaceCaptureOfInvalidDisplayNode
 * @tc.desc: Generate pure display node and take empty capture
 * @tc.type: FUNC
 * @tc.require: issueI5T8FR
*/
HWTEST_F(RSSurfaceCaptureTaskTest, TakeSurfaceCaptureOfInvalidDisplayNode, Function | SmallTest | Level2)
{
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(defaultConfig_);
    ASSERT_NE(displayNode, nullptr);
    
    HiLog::Info(LOG_LABEL, "TakeSurfaceCaptureOfInvalidDisplayNode, callback status and testsuccess[%d, %d]",
        surfaceCaptureCb_->IsCallbackCalled(), surfaceCaptureCb_->IsTestSuccess());
    bool ret = rsInterfaces_->TakeSurfaceCapture(displayNode, surfaceCaptureCb_);
    ASSERT_EQ(ret, true);
    ASSERT_EQ(CheckSurfaceCaptureCallback(), true);
    displayNode = nullptr;
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
}

/*
 * @tc.name: TakeSurfaceCaptureOfMirrorDisplayNode
 * @tc.desc: Generate valid mirror display node and take valid capture
 * @tc.type: FUNC
 * @tc.require: issueI5T8FR
*/
HWTEST_F(RSSurfaceCaptureTaskTest, TakeSurfaceCaptureOfMirrorDisplayNode, Function | SmallTest | Level2)
{
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(mirrorConfig_);
    ASSERT_NE(displayNode, nullptr);
    displayNode->AddChild(surfaceNode_, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    bool ret = rsInterfaces_->TakeSurfaceCapture(displayNode, surfaceCaptureCb_);
    ASSERT_EQ(ret, true);
    ASSERT_EQ(CheckSurfaceCaptureCallback(), true);
    ASSERT_EQ(surfaceCaptureCb_->IsTestSuccess(), true);
    displayNode = nullptr;
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
}

/*
 * @tc.name: TakeSurfaceCaptureOfSecurityLayer
 * @tc.desc: Generate display node and take securitylayer capture
 * @tc.type: FUNC
 * @tc.require: issueI6Q844
*/
HWTEST_F(RSSurfaceCaptureTaskTest, TakeSurfaceCaptureOfSecurityLayer, Function | SmallTest | Level2)
{
    RSDisplayNode::SharedPtr displayNode = RSDisplayNode::Create(defaultConfig_);
    ASSERT_NE(displayNode, nullptr);
    surfaceNode_->SetSecurityLayer(true);
    displayNode->AddChild(surfaceNode_, -1);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);

    bool ret = rsInterfaces_->TakeSurfaceCapture(displayNode, surfaceCaptureCb_);
    ASSERT_EQ(ret, true);
    ASSERT_EQ(CheckSurfaceCaptureCallback(), true);
    ASSERT_EQ(surfaceCaptureCb_->IsTestSuccess(), true);
    displayNode = nullptr;
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
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
    ASSERT_EQ(nullptr, task.Run());
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
    ASSERT_EQ(nullptr, task.Run());
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
    ASSERT_EQ(nullptr, task.Run());
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
    ASSERT_EQ(nullptr, task.Run());
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
    ASSERT_EQ(nullptr, task.Run());
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
    ASSERT_EQ(nullptr, task.Run());
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
    node.renderProperties_.SetAlpha(0.0f);
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
    node.renderProperties_.SetAlpha(0.0f);
    visitor_->IsDisplayNode(true);
    SkCanvas skCanvas(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    visitor_->canvas_ = std::make_unique<RSPaintFilterCanvas>(&skCanvas);
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
    node.renderProperties_.SetAlpha(0.0f);
    visitor_->IsDisplayNode(true);
    SkCanvas skCanvas(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    visitor_->canvas_ = std::make_unique<RSPaintFilterCanvas>(&skCanvas);
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
    visitor_->AdjustZOrderAndDrawSurfaceNode();
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
    node.renderProperties_.SetVisible(true);
    node.renderProperties_.SetAlpha(DEFAULT_CANVAS_SCALE);
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
    node.renderProperties_.SetVisible(true);
    node.renderProperties_.SetAlpha(DEFAULT_CANVAS_SCALE);
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
    node.renderProperties_.SetVisible(true);
    node.renderProperties_.SetAlpha(.0f);
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
    NodeId id = 0;
    RSEffectRenderNode node(id);
    SkCanvas skCanvas(DEFAULT_CANVAS_WIDTH, DEFAULT_CANVAS_HEIGHT);
    visitor_->canvas_ = std::make_unique<RSPaintFilterCanvas>(&skCanvas);
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
} // namespace Rosen
} // namespace OHOS
