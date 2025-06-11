/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "pipeline/main_thread/rs_main_thread.h"
#include "pipeline/render_thread/rs_uni_render_engine.h"
#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_display_render_node.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_root_render_node.h"
#include "pipeline/rs_render_node.h"
#include "pipeline/rs_surface_render_node.h"
#include "pipeline/rs_test_util.h"
#include "pipeline/rs_uni_render_judgement.h"
#include "platform/common/rs_system_properties.h"
#include "transaction/rs_interfaces.h"
#include "ui/rs_surface_extractor.h"
using namespace testing::ext;
using namespace OHOS::Rosen::DrawableV2;

namespace OHOS {
namespace Rosen {
using namespace HiviewDFX;
using DisplayId = ScreenId;
namespace {
    constexpr HiLogLabel LOG_LABEL = { LOG_CORE, 0xD001400, "RSWindowCaptureTaskTest" };
    constexpr uint32_t MAX_TIME_WAITING_FOR_CALLBACK = 200; // 10ms
    constexpr uint32_t SLEEP_TIME_IN_US = 10000; // 10ms
    constexpr uint32_t SLEEP_TIME_FOR_PROXY = 100000; // 100ms
    constexpr float DEFAULT_BOUNDS_WIDTH = 100.f;
    constexpr float DEFAULT_BOUNDS_HEIGHT = 200.f;
    constexpr uint32_t DEFAULT_CANVAS_WIDTH = 800;
    constexpr uint32_t DEFAULT_CANVAS_HEIGHT = 600;
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

class RSWindowCaptureTaskTest : public testing::Test {
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
RSInterfaces* RSWindowCaptureTaskTest::rsInterfaces_ = nullptr;
RenderContext* RSWindowCaptureTaskTest::renderContext_ = nullptr;
RSDisplayNodeConfig RSWindowCaptureTaskTest::defaultConfig_ = {INVALID_SCREEN_ID, false, INVALID_SCREEN_ID};
RSDisplayNodeConfig RSWindowCaptureTaskTest::mirrorConfig_ = {INVALID_SCREEN_ID, true, INVALID_SCREEN_ID};
std::shared_ptr<RSSurfaceNode> RSWindowCaptureTaskTest::surfaceNode_ = nullptr;
std::shared_ptr<CustomizedSurfaceCapture> RSWindowCaptureTaskTest::surfaceCaptureCb_ = nullptr;

void RSWindowCaptureTaskTest::SetUp()
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

void RSWindowCaptureTaskTest::TearDown()
{
    visitor_ = nullptr;
}

void RSWindowCaptureTaskTest::SetUpTestCase()
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

void RSWindowCaptureTaskTest::TearDownTestCase()
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

std::shared_ptr<RSSurfaceNode> RSWindowCaptureTaskTest::CreateSurface(std::string surfaceNodeName)
{
    RSSurfaceNodeConfig config;
    config.SurfaceNodeName = surfaceNodeName;
    return RSSurfaceNode::Create(config);
}

void RSWindowCaptureTaskTest::InitRenderContext()
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

void RSWindowCaptureTaskTest::FillSurface(std::shared_ptr<RSSurfaceNode> surfaceNode, const Drawing::Color color)
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

bool RSWindowCaptureTaskTest::CheckSurfaceCaptureCallback()
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
 * @tc.name: RSSurfaceCaptureForNullClientNode
 * @tc.desc: test surface capture when client node is nullptr
 * @tc.type: FUNC
 * @tc.require: issueICE45J
*/
HWTEST_F(RSWindowCaptureTaskTest, RSSurfaceCaptureForNullClientNode, Function | SmallTest | Level2)
{
    std::shared_ptr<RSSurfaceNode> surfaceNode  = nullptr;
    bool ret = rsInterfaces_->TakeSurfaceCapture(surfaceNode, surfaceCaptureCb_);
    EXPECT_EQ(ret, false);
}

/*
 * @tc.name: RSSurfaceCaptureForNullCallback
 * @tc.desc: Test surface capture when callback is nullptr
 * @tc.type: FUNC
 * @tc.require: issueICE45J
*/
HWTEST_F(RSWindowCaptureTaskTest, RSSurfaceCaptureForNullCallback, Function | SmallTest | Level2)
{
    std::shared_ptr<CustomizedSurfaceCapture> callback  = nullptr;
    auto surfaceNode = CreateSurface("RSSurfaceCaptureForNullCallback");
    ASSERT_NE(surfaceNode, nullptr);
    bool ret = rsInterfaces_->TakeSurfaceCapture(surfaceNode, callback);
    EXPECT_EQ(ret, false);
}

/*
 * @tc.name: RSSurfaceCaptureForInvalidScale
 * @tc.desc: test surface capture when scale is invalid
 * @tc.type: FUNC
 * @tc.require: issueICE45J
*/
HWTEST_F(RSWindowCaptureTaskTest, RSSurfaceCaptureForInvalidScale, Function | SmallTest | Level2)
{
    auto surfaceNode = CreateSurface("RSSurfaceCaptureForInvalidScale");
    ASSERT_NE(surfaceNode, nullptr);
    RSSurfaceCaptureConfig captureConfig;
    captureConfig.scaleX = 0.f;
    captureConfig.scaleY = 0.f;
    bool ret = rsInterfaces_->TakeSurfaceCapture(surfaceNode, surfaceCaptureCb_, captureConfig);
    EXPECT_EQ(ret, true);
#ifdef RS_ENABLE_UNI_RENDER
    EXPECT_EQ(CheckSurfaceCaptureCallback(), true);
    EXPECT_EQ(surfaceCaptureCb_->IsTestSuccess(), false);
#endif
}

/*
 * @tc.name: RSSurfaceCaptureForShouldPaint
 * @tc.desc: test surface capture when node should not paint
 * @tc.type: FUNC
 * @tc.require: issueICE45J
*/
HWTEST_F(RSWindowCaptureTaskTest, RSSurfaceCaptureForShouldPaint, Function | SmallTest | Level2)
{
    auto surfaceNode = CreateSurface("RSSurfaceCaptureForShouldPaint");
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetAlpha(0.f);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    bool ret = rsInterfaces_->TakeSurfaceCapture(surfaceNode, surfaceCaptureCb_);
    EXPECT_EQ(ret, true);
#ifdef RS_ENABLE_UNI_RENDER
    EXPECT_EQ(CheckSurfaceCaptureCallback(), true);
    EXPECT_EQ(surfaceCaptureCb_->IsTestSuccess(), false);
#endif

    surfaceNode->SetAlpha(1.f);
    surfaceNode->SetVisible(false);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    ret = rsInterfaces_->TakeSurfaceCapture(surfaceNode, surfaceCaptureCb_);
    EXPECT_EQ(ret, true);
#ifdef RS_ENABLE_UNI_RENDER
    EXPECT_EQ(CheckSurfaceCaptureCallback(), true);
    EXPECT_EQ(surfaceCaptureCb_->IsTestSuccess(), false);
#endif
}

/*
 * @tc.name: RSSurfaceCaptureForPixelMap
 * @tc.desc: test surface capture when node width or height <= 0
 * @tc.type: FUNC
 * @tc.require: issueICE45J
*/
HWTEST_F(RSWindowCaptureTaskTest, RSSurfaceCaptureForPixelMap, Function | SmallTest | Level2)
{
    auto surfaceNode = CreateSurface("RSSurfaceCaptureForPixelMap");
    ASSERT_NE(surfaceNode, nullptr);
    surfaceNode->SetBoundsWidth(0.f);
    surfaceNode->SetBoundsHeight(0.f);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    bool ret = rsInterfaces_->TakeSurfaceCapture(surfaceNode, surfaceCaptureCb_);
    EXPECT_EQ(ret, true);
#ifdef RS_ENABLE_UNI_RENDER
    EXPECT_EQ(CheckSurfaceCaptureCallback(), true);
    EXPECT_EQ(surfaceCaptureCb_->IsTestSuccess(), false);
#endif

    surfaceNode->SetBoundsWidth(-1.f);
    surfaceNode->SetBoundsHeight(-1.f);
    RSTransactionProxy::GetInstance()->FlushImplicitTransaction();
    usleep(SLEEP_TIME_FOR_PROXY);
    ret = rsInterfaces_->TakeSurfaceCapture(surfaceNode, surfaceCaptureCb_);
    EXPECT_EQ(ret, true);
#ifdef RS_ENABLE_UNI_RENDER
    EXPECT_EQ(CheckSurfaceCaptureCallback(), true);
    EXPECT_EQ(surfaceCaptureCb_->IsTestSuccess(), false);
#endif
}
} // namespace Rosen
} // namespace OHOS
